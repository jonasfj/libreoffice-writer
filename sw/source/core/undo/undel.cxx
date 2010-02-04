/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: undel.cxx,v $
 * $Revision: 1.27 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <hintids.hxx>
#include <unotools/charclass.hxx>
#include <svx/brkitem.hxx>
#include <fmtpdsc.hxx>
#include <frmfmt.hxx>
#include <fmtanchr.hxx>
#include <doc.hxx>
#include <swtable.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <pam.hxx>
#include <ndtxt.hxx>
#include <undobj.hxx>
#include <rolbck.hxx>
#include <poolfmt.hxx>
#include <mvsave.hxx>
#include <redline.hxx>
#include <docary.hxx>
#include <sfx2/app.hxx>

#include <fldbas.hxx>
#include <fmtfld.hxx>
#include <comcore.hrc> // #111827#
#include <undo.hrc>

// #include <svx/svxacorr.hxx>
// #include <comphelper/processfactory.hxx>
// #include <svx/unolingu.hxx>
// #include <unotools/localedatawrapper.hxx>

// using namespace comphelper;

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }


// DELETE
/*  lcl_MakeAutoFrms has to call MakeFrms for objects bounded "AtChar" ( == AUTO ),
    if the anchor frame has be moved via _MoveNodes(..) and DelFrms(..)
*/

void lcl_MakeAutoFrms( const SwSpzFrmFmts& rSpzArr, ULONG nMovedIndex )
{
    if( rSpzArr.Count() )
    {
        SwFlyFrmFmt* pFmt;
        const SwFmtAnchor* pAnchor;
        for( USHORT n = 0; n < rSpzArr.Count(); ++n )
        {
            pFmt = (SwFlyFrmFmt*)rSpzArr[n];
            pAnchor = &pFmt->GetAnchor();
            if (pAnchor->GetAnchorId() == FLY_AT_CHAR)
            {
                const SwPosition* pAPos = pAnchor->GetCntntAnchor();
                if( pAPos && nMovedIndex == pAPos->nNode.GetIndex() )
                    pFmt->MakeFrms();
            }
        }
    }
}

/*
SwUndoDelete has to perform a deletion and to record anything that is needed to restore the
situation before the deletion. Unfortunately a part of the deletion will be done after calling
this Ctor, this has to be kept in mind! In this Ctor only the complete paragraphs will be deleted,
the joining of the first and last paragraph of the selection will be handled outside this function.
Here are the main steps of the function:
1. Deletion/recording of content indizes of the selection: footnotes, fly frames and bookmarks
Step 1 could shift all nodes by deletion of footnotes => nNdDiff will be set.
2. If the paragraph where the selection ends, is the last content of a section so that this
section becomes empty when the paragraphs will be joined we have to do some smart actions ;-)
The paragraph will be moved outside the section and replaced by a dummy text node, the complete
section will be deleted in step 3. The difference between replacement dummy and original is
nReplacementDummy.
3. Moving complete selected nodes into the UndoArray. Before this happens the selection has to be
extended if there are sections which would become empty otherwise. BTW: sections will be moved into
the UndoArray if they are complete part of the selection. Sections starting or ending outside of the
selection will not be removed from the DocNodeArray even they got a "dummy"-copy in the UndoArray.
4. We have to anticipate the joining of the two paragraphs if the start paragraph is inside a
section and the end paragraph not. Then we have to move the paragraph into this section and to
record this in nSectDiff.
*/

SwUndoDelete::SwUndoDelete( SwPaM& rPam, BOOL bFullPara, BOOL bCalledByTblCpy )
    : SwUndo(UNDO_DELETE), SwUndRng( rPam ),
    pMvStt( 0 ), pSttStr(0), pEndStr(0), pRedlData(0), pRedlSaveData(0),
    nNode(0), nNdDiff(0), nSectDiff(0), nReplaceDummy(0), nSetPos(0),
    bGroup( FALSE ), bBackSp( FALSE ), bJoinNext( FALSE ), bTblDelLastNd( FALSE ),
    bDelFullPara( bFullPara ), bResetPgDesc( FALSE ), bResetPgBrk( FALSE ),
    bFromTableCopy( bCalledByTblCpy )
{
    bDelFullPara = bFullPara; // This is set e.g. if an empty paragraph before a table is deleted

    bCacheComment = false;

    SwDoc * pDoc = rPam.GetDoc();

    if( !pDoc->IsIgnoreRedline() && pDoc->GetRedlineTbl().Count() )
    {
        pRedlSaveData = new SwRedlineSaveDatas;
        if( !FillSaveData( rPam, *pRedlSaveData ))
            delete pRedlSaveData, pRedlSaveData = 0;
    }

    if( !pHistory )
        pHistory = new SwHistory;

    // loesche erstmal alle Fussnoten
    const SwPosition *pStt = rPam.Start(),
                    *pEnd = rPam.GetPoint() == pStt
                        ? rPam.GetMark()
                        : rPam.GetPoint();

    // Step 1. deletion/record of content indizes
    if( bDelFullPara )
    {
        ASSERT( rPam.HasMark(), "PaM ohne Mark" );
        DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint(),
                        DelCntntType(nsDelCntntType::DELCNT_ALL | nsDelCntntType::DELCNT_CHKNOCNTNT) );

        BOOL bDoesUndo = pDoc->DoesUndo();
        pDoc->DoUndo( FALSE );
        _DelBookmarks(pStt->nNode, pEnd->nNode);
        pDoc->DoUndo( bDoesUndo );
    }
    else
        DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );

    nSetPos = pHistory ? pHistory->Count() : 0;

    // wurde schon was geloescht ??
    nNdDiff = nSttNode - pStt->nNode.GetIndex();

    bJoinNext = !bFullPara && pEnd == rPam.GetPoint();
    bBackSp = !bFullPara && !bJoinNext;

    SwTxtNode *pSttTxtNd = 0, *pEndTxtNd = 0;
    if( !bFullPara )
    {
        pSttTxtNd = pStt->nNode.GetNode().GetTxtNode();
        pEndTxtNd = nSttNode == nEndNode
                    ? pSttTxtNd
                    : pEnd->nNode.GetNode().GetTxtNode();
    }

    BOOL bMoveNds = *pStt == *pEnd      // noch ein Bereich vorhanden ??
                ? FALSE
                : ( SaveCntnt( pStt, pEnd, pSttTxtNd, pEndTxtNd ) || bFromTableCopy );

    if( pSttTxtNd && pEndTxtNd && pSttTxtNd != pEndTxtNd )
    {
        // zwei unterschiedliche TextNodes, also speicher noch die
        // TextFormatCollection fuers
        pHistory->Add( pSttTxtNd->GetTxtColl(),pStt->nNode.GetIndex(), ND_TEXTNODE );
        pHistory->Add( pEndTxtNd->GetTxtColl(),pEnd->nNode.GetIndex(), ND_TEXTNODE );

        if( !bJoinNext )	 	// Selection von Unten nach Oben
        {
            // Beim JoinPrev() werden die AUTO-PageBreak's richtig
            // kopiert. Um diese beim Undo wieder herzustellen, muss das
            // Auto-PageBreak aus dem EndNode zurueckgesetzt werden.
            // - fuer die PageDesc, ColBreak dito !
            if( pEndTxtNd->HasSwAttrSet() )
            {
                SwRegHistory aRegHist( *pEndTxtNd, pHistory );
                if( SFX_ITEM_SET == pEndTxtNd->GetpSwAttrSet()->GetItemState(
                        RES_BREAK, FALSE ) )
                    pEndTxtNd->ResetAttr( RES_BREAK );
                if( pEndTxtNd->HasSwAttrSet() &&
                    SFX_ITEM_SET == pEndTxtNd->GetpSwAttrSet()->GetItemState(
                        RES_PAGEDESC, FALSE ) )
                    pEndTxtNd->ResetAttr( RES_PAGEDESC );
            }
        }
    }


    // verschiebe jetzt noch den PaM !!!
    // der SPoint steht am Anfang der SSelection
    if( pEnd == rPam.GetPoint() && ( !bFullPara || pSttTxtNd || pEndTxtNd ) )
        rPam.Exchange();

    if( !pSttTxtNd && !pEndTxtNd )
        rPam.GetPoint()->nNode--;
    rPam.DeleteMark();			// der SPoint ist aus dem Bereich

    if( !pEndTxtNd )
        nEndCntnt = 0;
    if( !pSttTxtNd )
        nSttCntnt = 0;

    if( bMoveNds )		// sind noch Nodes zu verschieben ?
    {
        SwNodes& rNds = (SwNodes&)*pDoc->GetUndoNds();
        SwNodes& rDocNds = pDoc->GetNodes();
        SwNodeRange aRg( rDocNds, nSttNode - nNdDiff,
                         rDocNds, nEndNode - nNdDiff );
        if( !bFullPara && !pEndTxtNd &&
            &aRg.aEnd.GetNode() != &pDoc->GetNodes().GetEndOfContent() )
        {
            SwNode* pNode = aRg.aEnd.GetNode().StartOfSectionNode();
            if( pNode->GetIndex() >= nSttNode - nNdDiff )
                aRg.aEnd++; // Deletion of a complete table
        }
        SwNode* pTmpNd;
        // Step 2: Expand selection if necessary
        if( bJoinNext || bFullPara )
        {
            // If all content of a section will be moved into Undo,
            // the section itself should be moved complete.
            while( aRg.aEnd.GetIndex() + 2  < rDocNds.Count() &&
                ( (pTmpNd = rDocNds[ aRg.aEnd.GetIndex()+1 ])->IsEndNode() &&
                pTmpNd->StartOfSectionNode()->IsSectionNode() &&
                pTmpNd->StartOfSectionNode()->GetIndex() >= aRg.aStart.GetIndex() ) )
                aRg.aEnd++;
            nReplaceDummy = aRg.aEnd.GetIndex() + nNdDiff - nEndNode;
            if( nReplaceDummy )
            {   // The selection has been expanded, because
                aRg.aEnd++;
                if( pEndTxtNd )
                {
                    // The end text node has to leave the (expanded) selection
                    // The dummy is needed because _MoveNodes deletes empty sections
                    ++nReplaceDummy;
                    SwNodeRange aMvRg( *pEndTxtNd, 0, *pEndTxtNd, 1 );
                    SwPosition aSplitPos( *pEndTxtNd );
                    BOOL bDoesUndo = pDoc->DoesUndo();
                    pDoc->DoUndo( FALSE );
                    pDoc->SplitNode( aSplitPos, false );
                    rDocNds._MoveNodes( aMvRg, rDocNds, aRg.aEnd, TRUE );
                    pDoc->DoUndo( bDoesUndo );
                    aRg.aEnd--;
                }
                else
                    nReplaceDummy = 0;
            }
        }
        if( bBackSp || bFullPara )
        {
            //See above, the selection has to expanded if there are "nearly empty" sections
            // and a replacement dummy has to be set if needed.
            while( 1 < aRg.aStart.GetIndex() &&
                ( (pTmpNd = rDocNds[ aRg.aStart.GetIndex()-1 ])->IsSectionNode() &&
                pTmpNd->EndOfSectionIndex() < aRg.aEnd.GetIndex() ) )
                aRg.aStart--;
            if( pSttTxtNd )
            {
                nReplaceDummy = nSttNode - nNdDiff - aRg.aStart.GetIndex();
                if( nReplaceDummy )
                {
                    SwNodeRange aMvRg( *pSttTxtNd, 0, *pSttTxtNd, 1 );
                    SwPosition aSplitPos( *pSttTxtNd );
                    BOOL bDoesUndo = pDoc->DoesUndo();
                    pDoc->DoUndo( FALSE );
                    pDoc->SplitNode( aSplitPos, false );
                    rDocNds._MoveNodes( aMvRg, rDocNds, aRg.aStart, TRUE );
                    pDoc->DoUndo( bDoesUndo );
                    aRg.aStart--;
                }
            }
        }

        if( bFromTableCopy )
        {
            if( !pEndTxtNd )
            {
                if( pSttTxtNd )
                    aRg.aStart++;
                else if( !bFullPara && !aRg.aEnd.GetNode().IsCntntNode() )
                    aRg.aEnd--;
            }
        }
        else if( pSttTxtNd && ( pEndTxtNd || pSttTxtNd->GetTxt().Len() ) )
            aRg.aStart++;

        // Step 3: Moving into UndoArray...
        nNode = rNds.GetEndOfContent().GetIndex();
        rDocNds._MoveNodes( aRg, rNds, SwNodeIndex( rNds.GetEndOfContent() ));
        pMvStt = new SwNodeIndex( rNds, nNode );
        nNode = rNds.GetEndOfContent().GetIndex() - nNode;		// Differenz merken !
        if( pSttTxtNd && pEndTxtNd )
        {
            //Step 4: Moving around sections
            nSectDiff = aRg.aEnd.GetIndex() - aRg.aStart.GetIndex();
            // nSect is the number of sections which starts(ends) between start and end node of the
            // selection. The "loser" paragraph has to be moved into the section(s) of the
            // "winner" paragraph
            if( nSectDiff )
            {
                if( bJoinNext )
                {
                    SwNodeRange aMvRg( *pEndTxtNd, 0, *pEndTxtNd, 1 );
                    rDocNds._MoveNodes( aMvRg, rDocNds, aRg.aStart, TRUE );
                }
                else
                {
                    SwNodeRange aMvRg( *pSttTxtNd, 0, *pSttTxtNd, 1 );
                    rDocNds._MoveNodes( aMvRg, rDocNds, aRg.aEnd, TRUE );
                }
            }
        }
        if( nSectDiff || nReplaceDummy )
            lcl_MakeAutoFrms( *pDoc->GetSpzFrmFmts(),
                bJoinNext ? pEndTxtNd->GetIndex() : pSttTxtNd->GetIndex() );
    }
    else
        nNode = 0;		// kein Node verschoben -> keine Differenz zum Ende

    // wurden davor noch Nodes geloescht ?? (FootNotes haben ContentNodes!)
    if( !pSttTxtNd && !pEndTxtNd )
    {
        nNdDiff = nSttNode - rPam.GetPoint()->nNode.GetIndex() - (bFullPara ? 0 : 1);
        rPam.Move( fnMoveForward, fnGoNode );
    }
    else
    {
        nNdDiff = nSttNode;
        if( nSectDiff && bBackSp )
            nNdDiff += nSectDiff;
        nNdDiff -= rPam.GetPoint()->nNode.GetIndex();
    }

    if( !rPam.GetNode()->IsCntntNode() )
        rPam.GetPoint()->nContent.Assign( 0, 0 );

    // wird die History ueberhaupt benoetigt ??
    if( pHistory && !pHistory->Count() )
        DELETEZ( pHistory );
}

BOOL SwUndoDelete::SaveCntnt( const SwPosition* pStt, const SwPosition* pEnd,
                    SwTxtNode* pSttTxtNd, SwTxtNode* pEndTxtNd )
{
    ULONG nNdIdx = pStt->nNode.GetIndex();
    // 1 - kopiere den Anfang in den Start-String
    if( pSttTxtNd )
    {
        BOOL bOneNode = nSttNode == nEndNode;
        xub_StrLen nLen = bOneNode ? nEndCntnt - nSttCntnt
                                : pSttTxtNd->GetTxt().Len() - nSttCntnt;
        SwRegHistory aRHst( *pSttTxtNd, pHistory );
        // always save all text atttibutes because of possibly overlapping
        // areas of on/off
        pHistory->CopyAttr( pSttTxtNd->GetpSwpHints(), nNdIdx,
                            0, pSttTxtNd->GetTxt().Len(), true );
        if( !bOneNode && pSttTxtNd->HasSwAttrSet() )
                pHistory->CopyFmtAttr( *pSttTxtNd->GetpSwAttrSet(), nNdIdx );

        // die Laenge kann sich veraendert haben (!!Felder!!)
        nLen = ( bOneNode ? pEnd->nContent.GetIndex() : pSttTxtNd->GetTxt().Len() )
                - pStt->nContent.GetIndex();


        // loesche jetzt noch den Text (alle Attribut-Aenderungen kommen in
        // die Undo-History
        pSttStr = (String*)new String( pSttTxtNd->GetTxt().Copy( nSttCntnt, nLen ));
        pSttTxtNd->EraseText( pStt->nContent, nLen );
        if( pSttTxtNd->GetpSwpHints() )
            pSttTxtNd->GetpSwpHints()->DeRegister();

        // METADATA: store
        bool emptied( pSttStr->Len() && !pSttTxtNd->Len() );
        if (!bOneNode || emptied) // merging may overwrite xmlids...
        {
            m_pMetadataUndoStart = pSttTxtNd->CreateUndo( emptied );
        }

        if( bOneNode )
            return FALSE;           // keine Nodes mehr verschieben
    }


    // 2 - kopiere das Ende in den End-String
    if( pEndTxtNd )
    {
        SwIndex aEndIdx( pEndTxtNd );
        nNdIdx = pEnd->nNode.GetIndex();
        SwRegHistory aRHst( *pEndTxtNd, pHistory );

        // always save all text atttibutes because of possibly overlapping
        // areas of on/off
        pHistory->CopyAttr( pEndTxtNd->GetpSwpHints(), nNdIdx, 0,
                            pEndTxtNd->GetTxt().Len(), true );

        if( pEndTxtNd->HasSwAttrSet() )
            pHistory->CopyFmtAttr( *pEndTxtNd->GetpSwAttrSet(), nNdIdx );


        // loesche jetzt noch den Text (alle Attribut-Aenderungen kommen in
        // die Undo-History
        pEndStr = (String*)new String( pEndTxtNd->GetTxt().Copy( 0,
                                    pEnd->nContent.GetIndex() ));
        pEndTxtNd->EraseText( aEndIdx, pEnd->nContent.GetIndex() );
        if( pEndTxtNd->GetpSwpHints() )
            pEndTxtNd->GetpSwpHints()->DeRegister();

        // METADATA: store
        bool emptied( pEndStr->Len() && !pEndTxtNd->Len() );
        m_pMetadataUndoEnd = pEndTxtNd->CreateUndo( emptied );
    }

    // sind es nur zwei Nodes, dann ist schon alles erledigt.
    if( ( pSttTxtNd || pEndTxtNd ) && nSttNode + 1 == nEndNode )
        return FALSE;           // keine Nodes mehr verschieben

    return TRUE;                // verschiebe die dazwischen liegenden Nodes
}


BOOL SwUndoDelete::CanGrouping( SwDoc* pDoc, const SwPaM& rDelPam )
{
    // ist das Undo groesser als 1 Node ? (sprich: Start und EndString)
    if( pSttStr ? !pSttStr->Len() || pEndStr : TRUE )
        return FALSE;

    // es kann nur das Loeschen von einzelnen char's zusammengefasst werden
    if( nSttNode != nEndNode || ( !bGroup && nSttCntnt+1 != nEndCntnt ))
        return FALSE;

    const SwPosition *pStt = rDelPam.Start(),
                    *pEnd = rDelPam.GetPoint() == pStt
                        ? rDelPam.GetMark()
                        : rDelPam.GetPoint();

    if( pStt->nNode != pEnd->nNode ||
        pStt->nContent.GetIndex()+1 != pEnd->nContent.GetIndex() ||
        pEnd->nNode != nSttNode )
        return FALSE;

    // untercheide zwischen BackSpace und Delete. Es muss dann das
    // Undo-Array unterschiedlich aufgebaut werden !!
    if( pEnd->nContent == nSttCntnt )
    {
        if( bGroup && !bBackSp ) return FALSE;
        bBackSp = TRUE;
    }
    else if( pStt->nContent == nSttCntnt )
    {
        if( bGroup && bBackSp ) return FALSE;
        bBackSp = FALSE;
    }
    else
        return FALSE;

    // sind die beiden Nodes (Nodes-/Undo-Array) ueberhaupt TextNodes?
    SwTxtNode * pDelTxtNd = pStt->nNode.GetNode().GetTxtNode();
    if( !pDelTxtNd ) return FALSE;

    xub_StrLen nUChrPos = bBackSp ? 0 : pSttStr->Len()-1;
    sal_Unicode cDelChar = pDelTxtNd->GetTxt().GetChar( pStt->nContent.GetIndex() );
    CharClass& rCC = GetAppCharClass();
    if( ( CH_TXTATR_BREAKWORD == cDelChar || CH_TXTATR_INWORD == cDelChar ) ||
        rCC.isLetterNumeric( String( cDelChar ), 0 ) !=
        rCC.isLetterNumeric( *pSttStr, nUChrPos ) )
        return FALSE;

    {
        SwRedlineSaveDatas* pTmpSav = new SwRedlineSaveDatas;
        if( !FillSaveData( rDelPam, *pTmpSav, FALSE ))
            delete pTmpSav, pTmpSav = 0;

        BOOL bOk = ( !pRedlSaveData && !pTmpSav ) ||
                   ( pRedlSaveData && pTmpSav &&
                SwUndo::CanRedlineGroup( *pRedlSaveData, *pTmpSav, bBackSp ));
        delete pTmpSav;
        if( !bOk )
            return FALSE;

        pDoc->DeleteRedline( rDelPam, false, USHRT_MAX );
    }

    // Ok, die beiden 'Deletes' koennen zusammen gefasst werden, also
    // 'verschiebe' das enstprechende Zeichen
    if( bBackSp )
        nSttCntnt--;    // BackSpace: Zeichen in Array einfuegen !!
    else
    {
        nEndCntnt++;    // Delete: Zeichen am Ende anhaengen
        nUChrPos++;
    }
    pSttStr->Insert( cDelChar, nUChrPos );
    pDelTxtNd->EraseText( pStt->nContent, 1 );

    bGroup = TRUE;
    return TRUE;
}



SwUndoDelete::~SwUndoDelete()
{
    delete pSttStr;
    delete pEndStr;
    if( pMvStt )		// loesche noch den Bereich aus dem UndoNodes Array
    {
        // Insert speichert den Inhalt in der IconSection
        pMvStt->GetNode().GetNodes().Delete( *pMvStt, nNode );
        delete pMvStt;
    }
    delete pRedlData;
    delete pRedlSaveData;
}

static SwRewriter lcl_RewriterFromHistory(SwHistory & rHistory)
{
    SwRewriter aRewriter;

    bool bDone = false;

    for ( USHORT n = 0; n < rHistory.Count(); n++)
    {
        String aDescr = rHistory[n]->GetDescription();

        if (aDescr.Len() > 0)
        {
            aRewriter.AddRule(UNDO_ARG2, aDescr);

            bDone = true;
            break;
        }
    }

    if (! bDone)
    {
        aRewriter.AddRule(UNDO_ARG2, SW_RES(STR_FIELD));
    }

    return aRewriter;
}

SwRewriter SwUndoDelete::GetRewriter() const
{
    SwRewriter aResult;
    String * pStr = NULL;

    if (nNode != 0)
    {
        if (sTableName.Len() > 0)
        {

            SwRewriter aRewriter;
            aRewriter.AddRule(UNDO_ARG1, SW_RES(STR_START_QUOTE));
            aRewriter.AddRule(UNDO_ARG2, sTableName);
            aRewriter.AddRule(UNDO_ARG3, SW_RES(STR_END_QUOTE));

            String sTmp = aRewriter.Apply(SW_RES(STR_TABLE_NAME));
            aResult.AddRule(UNDO_ARG1, sTmp);
        }
        else
            aResult.AddRule(UNDO_ARG1, String(SW_RES(STR_PARAGRAPHS)));
    }
    else
    {
        String aStr;

        if (pSttStr != NULL && pEndStr != NULL && pSttStr->Len() == 0 &&
            pEndStr->Len() == 0)
        {
            aStr = SW_RES(STR_PARAGRAPH_UNDO);
        }
        else
        {
            if (pSttStr != NULL)
                pStr = pSttStr;
            else if (pEndStr != NULL)
                pStr = pEndStr;

            if (pStr != NULL)
            {
                aStr = DenoteSpecialCharacters(*pStr);
            }
            else
            {
                aStr = UNDO_ARG2;
            }
        }

        aStr = ShortenString(aStr, nUndoStringLength, String(SW_RES(STR_LDOTS)));
        if (pHistory)
        {
            SwRewriter aRewriter = lcl_RewriterFromHistory(*pHistory);
            aStr = aRewriter.Apply(aStr);
        }

        aResult.AddRule(UNDO_ARG1, aStr);
    }

    return aResult;
}

// Every object, anchored "AtCntnt" will be reanchored at rPos
void lcl_ReAnchorAtCntntFlyFrames( const SwSpzFrmFmts& rSpzArr, SwPosition &rPos, ULONG nOldIdx )
{
    if( rSpzArr.Count() )
    {
        SwFlyFrmFmt* pFmt;
        const SwFmtAnchor* pAnchor;
        const SwPosition* pAPos;
        for( USHORT n = 0; n < rSpzArr.Count(); ++n )
        {
            pFmt = (SwFlyFrmFmt*)rSpzArr[n];
            pAnchor = &pFmt->GetAnchor();
            if (pAnchor->GetAnchorId() == FLY_AT_PARA)
            {
                pAPos =  pAnchor->GetCntntAnchor();
                if( pAPos && nOldIdx == pAPos->nNode.GetIndex() )
                {
                    SwFmtAnchor aAnch( *pAnchor );
                    aAnch.SetAnchor( &rPos );
                    pFmt->SetFmtAttr( aAnch );
                }
            }
        }
    }
}

void SwUndoDelete::Undo( SwUndoIter& rUndoIter )
{
    SwDoc* pDoc = &rUndoIter.GetDoc();
    BOOL bUndo = pDoc->DoesUndo();
    pDoc->DoUndo( FALSE );

    ULONG nCalcStt = nSttNode - nNdDiff;

    if( nSectDiff && bBackSp )
        nCalcStt += nSectDiff;

    SwNodeIndex aIdx( pDoc->GetNodes(), nCalcStt );
    SwNode* pInsNd = &aIdx.GetNode();

    {		// Block, damit der SwPosition beim loeschen vom Node
            // abgemeldet ist
        SwPosition aPos( aIdx );
        if( !bDelFullPara )
        {
            if( pInsNd->IsTableNode() )
            {
                pInsNd = pDoc->GetNodes().MakeTxtNode( aIdx,
                        (SwTxtFmtColl*)pDoc->GetDfltTxtFmtColl() );
                aIdx--;
                aPos.nNode = aIdx;
                aPos.nContent.Assign( pInsNd->GetCntntNode(), nSttCntnt );
            }
            else
            {
                if( pInsNd->IsCntntNode() )
                    aPos.nContent.Assign( (SwCntntNode*)pInsNd, nSttCntnt );
                if( !bTblDelLastNd )
                    pInsNd = 0;			// Node nicht loeschen !!
            }
        }
        else
            pInsNd = 0;			// Node nicht loeschen !!

        SwNodes* pUNds = (SwNodes*)pDoc->GetUndoNds();
        BOOL bNodeMove = 0 != nNode;

        if( pEndStr )
        {
            // alle Attribute verwerfen, wurden alle gespeichert!
            SwTxtNode* pTxtNd = aPos.nNode.GetNode().GetTxtNode();

            if( pTxtNd && pTxtNd->HasSwAttrSet() )
                pTxtNd->ResetAllAttr();

            if( pTxtNd && pTxtNd->GetpSwpHints() )
                pTxtNd->ClearSwpHintsArr( true );

            if( pSttStr && !bFromTableCopy )
            {
                ULONG nOldIdx = aPos.nNode.GetIndex();
                pDoc->SplitNode( aPos, false );
                // After the split all objects are anchored at the first paragraph,
                // but the pHistory of the fly frame formats relies on anchoring at
                // the start of the selection => selection backwards needs a correction.
                if( bBackSp )
                    lcl_ReAnchorAtCntntFlyFrames( *pDoc->GetSpzFrmFmts(), aPos, nOldIdx );
                pTxtNd = aPos.nNode.GetNode().GetTxtNode();
            }
            if( pTxtNd )
            {
                pTxtNd->InsertText( *pEndStr, aPos.nContent,
                        IDocumentContentOperations::INS_NOHINTEXPAND );
                // METADATA: restore
                pTxtNd->RestoreMetadata(m_pMetadataUndoEnd);
            }
        }
        else if( pSttStr && bNodeMove )
        {
            SwTxtNode * pNd = aPos.nNode.GetNode().GetTxtNode();
            if( pNd )
            {
                if( nSttCntnt < pNd->GetTxt().Len() )
                {
                    ULONG nOldIdx = aPos.nNode.GetIndex();
                    pDoc->SplitNode( aPos, false );
                    if( bBackSp )
                        lcl_ReAnchorAtCntntFlyFrames( *pDoc->GetSpzFrmFmts(), aPos, nOldIdx );
                }
                else
                    aPos.nNode++;
            }
        }
        SwNode* pMovedNode = NULL;
        if( nSectDiff )
        {
            ULONG nMoveIndex = aPos.nNode.GetIndex();
            int nDiff = 0;
            if( bJoinNext )
            {
                nMoveIndex += nSectDiff + 1;
                pMovedNode = &aPos.nNode.GetNode();
            }
            else
            {
                nMoveIndex -= nSectDiff + 1;
                ++nDiff;
            }
            SwNodeIndex aMvIdx( pDoc->GetNodes(), nMoveIndex );
            SwNodeRange aRg( aPos.nNode, 0 - nDiff, aPos.nNode, 1 - nDiff );
            aPos.nNode--;
            if( !bJoinNext )
                pMovedNode = &aPos.nNode.GetNode();
            pDoc->GetNodes()._MoveNodes( aRg, pDoc->GetNodes(), aMvIdx, TRUE );
            aPos.nNode++;
        }

        if( bNodeMove )
        {
            SwNodeRange aRange( *pMvStt, 0, *pMvStt, nNode );
            SwNodeIndex aCopyIndex( aPos.nNode, -1 );
            pUNds->_Copy( aRange, aPos.nNode );

            if( nReplaceDummy )
            {
                ULONG nMoveIndex;
                if( bJoinNext )
                {
                    nMoveIndex = nEndNode - nNdDiff;
                    aPos.nNode = nMoveIndex + nReplaceDummy;
                }
                else
                {
                    aPos = SwPosition( aCopyIndex );
                    nMoveIndex = aPos.nNode.GetIndex() + nReplaceDummy + 1;
                }
                SwNodeIndex aMvIdx( pDoc->GetNodes(), nMoveIndex );
                SwNodeRange aRg( aPos.nNode, 0, aPos.nNode, 1 );
                pMovedNode = &aPos.nNode.GetNode();
                pDoc->GetNodes()._MoveNodes( aRg, pDoc->GetNodes(), aMvIdx, TRUE );
                pDoc->GetNodes().Delete( aMvIdx, 1 );
            }
        }

        if( pMovedNode )
            lcl_MakeAutoFrms( *pDoc->GetSpzFrmFmts(), pMovedNode->GetIndex() );

        if( pSttStr )
        {
            aPos.nNode = nSttNode - nNdDiff + ( bJoinNext ? 0 : nReplaceDummy );
            SwTxtNode * pTxtNd = aPos.nNode.GetNode().GetTxtNode();
            // wenn mehr als ein Node geloescht wurde, dann wurden auch
            // alle "Node"-Attribute gespeichert

            if (pTxtNd != NULL)
            {
                if( pTxtNd->HasSwAttrSet() && bNodeMove && !pEndStr )
                    pTxtNd->ResetAllAttr();

                if( pTxtNd->GetpSwpHints() )
                    pTxtNd->ClearSwpHintsArr( true );

                // SectionNode-Modus und von oben nach unten selektiert:
                //	-> im StartNode steht noch der Rest vom Join => loeschen
                aPos.nContent.Assign( pTxtNd, nSttCntnt );
                pTxtNd->InsertText( *pSttStr, aPos.nContent,
                        IDocumentContentOperations::INS_NOHINTEXPAND );
                // METADATA: restore
                pTxtNd->RestoreMetadata(m_pMetadataUndoStart);
            }
        }

        if( pHistory )
        {
            pHistory->TmpRollback( pDoc, nSetPos, false );
            if( nSetPos )		// es gab Fussnoten/FlyFrames
            {
                // gibts ausser diesen noch andere ?
                if( nSetPos < pHistory->Count() )
                {
                    // dann sicher die Attribute anderen Attribute
                    SwHistory aHstr;
                    aHstr.Move( 0, pHistory, nSetPos );
                    pHistory->Rollback( pDoc );
                    pHistory->Move( 0, &aHstr );
                }
                else
                {
                    pHistory->Rollback( pDoc );
                    DELETEZ( pHistory );
                }
            }
        }

        if( bResetPgDesc || bResetPgBrk )
        {
            USHORT nStt = static_cast<USHORT>( bResetPgDesc ? RES_PAGEDESC : RES_BREAK );
            USHORT nEnd = static_cast<USHORT>( bResetPgBrk ? RES_BREAK : RES_PAGEDESC );

            SwNode* pNode = pDoc->GetNodes()[ nEndNode + 1 ];
            if( pNode->IsCntntNode() )
                ((SwCntntNode*)pNode)->ResetAttr( nStt, nEnd );
            else if( pNode->IsTableNode() )
                ((SwTableNode*)pNode)->GetTable().GetFrmFmt()->ResetFmtAttr( nStt, nEnd );
        }
    }
    // den temp. eingefuegten Node noch loeschen !!
    if( pInsNd )
        pDoc->GetNodes().Delete( aIdx, 1 );
    if( pRedlSaveData )
        SetSaveData( *pDoc, *pRedlSaveData );

    pDoc->DoUndo( bUndo );			// Undo wieder einschalten
    SetPaM( rUndoIter, TRUE );
}

void SwUndoDelete::Redo( SwUndoIter& rUndoIter )
{
    rUndoIter.SetUpdateAttr( TRUE );

    SwPaM& rPam = *rUndoIter.pAktPam;
    SwDoc& rDoc = *rPam.GetDoc();

    SetPaM( rPam );

    if( pRedlSaveData )
        rDoc.DeleteRedline( rPam, false, USHRT_MAX );

    if( !bDelFullPara )
    {
        SwUndRng aTmpRng( rPam );
        RemoveIdxFromRange( rPam, FALSE );
        aTmpRng.SetPaM( rPam );

        if( !bJoinNext )            // Dann Selektion von unten nach oben
            rPam.Exchange();        // wieder herstellen!
    }

    if( pHistory )      // wurden Attribute gesichert ?
    {
        pHistory->SetTmpEnd( pHistory->Count() );
        SwHistory aHstr;
        aHstr.Move( 0, pHistory );

        if( bDelFullPara )
        {
            ASSERT( rPam.HasMark(), "PaM ohne Mark" );
            DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint(),
                            DelCntntType(nsDelCntntType::DELCNT_ALL | nsDelCntntType::DELCNT_CHKNOCNTNT) );

            _DelBookmarks(rPam.GetMark()->nNode, rPam.GetPoint()->nNode);
        }
        else
            DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );
        nSetPos = pHistory ? pHistory->Count() : 0;

        pHistory->Move( nSetPos, &aHstr );
    }
    else
    {
        if( bDelFullPara )
        {
            ASSERT( rPam.HasMark(), "PaM ohne Mark" );
            DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint(),
                            DelCntntType(nsDelCntntType::DELCNT_ALL | nsDelCntntType::DELCNT_CHKNOCNTNT) );

            _DelBookmarks( rPam.GetMark()->nNode, rPam.GetPoint()->nNode );
        }
        else
            DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );
        nSetPos = pHistory ? pHistory->Count() : 0;
    }

    if( !pSttStr && !pEndStr )
    {
        SwNodeIndex aSttIdx = ( bDelFullPara || bJoinNext )
                                    ? rPam.GetMark()->nNode
                                    : rPam.GetPoint()->nNode;
        SwTableNode* pTblNd = aSttIdx.GetNode().GetTableNode();
        if( pTblNd )
        {
            if( bTblDelLastNd )
            {
                // dann am Ende wieder einen Node einfuegen
                const SwNodeIndex aTmpIdx( *pTblNd->EndOfSectionNode(), 1 );
                rDoc.GetNodes().MakeTxtNode( aTmpIdx,
                        rDoc.GetTxtCollFromPool( RES_POOLCOLL_STANDARD ) );
            }

            SwCntntNode* pNextNd = rDoc.GetNodes()[
                    pTblNd->EndOfSectionIndex()+1 ]->GetCntntNode();
            if( pNextNd )
            {
                SwFrmFmt* pTableFmt = pTblNd->GetTable().GetFrmFmt();

                const SfxPoolItem *pItem;
                if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_PAGEDESC,
                    FALSE, &pItem ) )
                    pNextNd->SetAttr( *pItem );

                if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_BREAK,
                    FALSE, &pItem ) )
                    pNextNd->SetAttr( *pItem );
            }
            pTblNd->DelFrms();
        }

        rPam.SetMark();
        rPam.DeleteMark();

        rDoc.GetNodes().Delete( aSttIdx, nEndNode - nSttNode );

        // setze den Cursor immer in einen ContentNode !!
        if( !rPam.Move( fnMoveBackward, fnGoCntnt ) &&
            !rPam.Move( fnMoveForward, fnGoCntnt ) )
            rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), 0 );
    }
    else if( bDelFullPara )
    {
        // der Pam wurde am Point( == Ende) um eins erhoeht, um einen
        // Bereich fuers Undo zu haben. Der muss jetzt aber wieder entfernt
        // werden!!!
        rPam.End()->nNode--;
        if( rPam.GetPoint()->nNode == rPam.GetMark()->nNode )
            *rPam.GetMark() = *rPam.GetPoint();
        rDoc.DelFullPara( rPam );
    }
    else
        rDoc.DeleteAndJoin( rPam );
}

void SwUndoDelete::Repeat( SwUndoIter& rUndoIter )
{
    if( UNDO_DELETE == rUndoIter.GetLastUndoId() )
        return;

    SwPaM& rPam = *rUndoIter.pAktPam;
    SwDoc& rDoc = *rPam.GetDoc();
    BOOL bGroupUndo = rDoc.DoesGroupUndo();
    rDoc.DoGroupUndo( FALSE );
    if( !rPam.HasMark() )
    {
        rPam.SetMark();
        rPam.Move( fnMoveForward, fnGoCntnt );
    }
    if( bDelFullPara )
        rDoc.DelFullPara( rPam );
    else
        rDoc.DeleteAndJoin( rPam );
    rDoc.DoGroupUndo( bGroupUndo );
    rUndoIter.pLastUndoObj = this;
}


void SwUndoDelete::SetTableName(const String & rName)
{
    sTableName = rName;
}
