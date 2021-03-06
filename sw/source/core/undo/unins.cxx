/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include <sot/storage.hxx>
#include <editeng/keepitem.hxx>
#include <svx/svdobj.hxx>

#include <docsh.hxx>
#include <fmtcntnt.hxx>
#include <fmtanchr.hxx>
#include <frmfmt.hxx>
#include <doc.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <pam.hxx>
#include <ndtxt.hxx>
#include <undobj.hxx>
#include <rolbck.hxx>
#include <ndgrf.hxx>
#include <ndole.hxx>
#include <grfatr.hxx>
#include <cntfrm.hxx>
#include <flyfrm.hxx>
#include <fesh.hxx>
#include <swtable.hxx>
#include <redline.hxx>
#include <docary.hxx>
#include <acorrect.hxx>
#include <dcontact.hxx>

#include <comcore.hrc> // #111827#
#include <undo.hrc>

using namespace ::com::sun::star;


class _UnReplaceData : private SwUndoSaveCntnt
{
    String m_sOld, m_sIns;
    ULONG m_nSttNd, m_nEndNd, m_nOffset;
    xub_StrLen m_nSttCnt, m_nEndCnt, m_nSetPos, m_nSelEnd;
    BOOL m_bSplitNext : 1;
    BOOL m_bRegExp : 1;
    // metadata references for paragraph and following para (if m_bSplitNext)
    ::boost::shared_ptr< ::sfx2::MetadatableUndo > m_pMetadataUndoStart;
    ::boost::shared_ptr< ::sfx2::MetadatableUndo > m_pMetadataUndoEnd;

public:
    _UnReplaceData( const SwPaM& rPam, const String& rIns, BOOL bRegExp );
    ~_UnReplaceData();

    void Undo( SwUndoIter& rIter );
    void Redo( SwUndoIter& rIter );
    void SetEnd( const SwPaM& rPam );

    const String & GetOld() const { return m_sOld; }
    const String & GetIns() const { return m_sIns; }
};


SV_IMPL_PTRARR( _UnReplaceDatas, _UnReplaceData* )

//------------------------------------------------------------------

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }

// zwei Zugriffs-Funktionen
inline SwPosition* IterPt( SwUndoIter& rUIter )
{	return rUIter.pAktPam->GetPoint();	}
inline SwPosition* IterMk( SwUndoIter& rUIter )
{	return rUIter.pAktPam->GetMark();	}

//------------------------------------------------------------

// INSERT

String * SwUndoInsert::GetTxtFromDoc() const
{
    String * pResult = NULL;

    SwNodeIndex aNd( pDoc->GetNodes(), nNode);
    SwCntntNode* pCNd = aNd.GetNode().GetCntntNode();
    SwPaM aPaM( *pCNd, nCntnt );

    aPaM.SetMark();

    if( pCNd->IsTxtNode() )
    {
        pResult = new String( ((SwTxtNode*)pCNd)->GetTxt().Copy(nCntnt-nLen,
                                                             nLen ) );

    }

    return pResult;
}

void SwUndoInsert::Init(const SwNodeIndex & rNd)
{
    // Redline beachten
    pDoc = rNd.GetNode().GetDoc();
    if( pDoc->IsRedlineOn() )
    {
        pRedlData = new SwRedlineData( nsRedlineType_t::REDLINE_INSERT,
                                       pDoc->GetRedlineAuthor() );
        SetRedlineMode( pDoc->GetRedlineMode() );
    }

    pUndoTxt = GetTxtFromDoc();

    bCacheComment = false;
}

// #111827#
SwUndoInsert::SwUndoInsert( const SwNodeIndex& rNd, xub_StrLen nCnt,
            xub_StrLen nL,
            const IDocumentContentOperations::InsertFlags nInsertFlags,
            BOOL bWDelim )
    : SwUndo(UNDO_TYPING), pPos( 0 ), pTxt( 0 ), pRedlData( 0 ),
        nNode( rNd.GetIndex() ), nCntnt(nCnt), nLen(nL),
        bIsWordDelim( bWDelim ), bIsAppend( FALSE )
    , m_nInsertFlags(nInsertFlags)
{
    Init(rNd);
}

// #111827#
SwUndoInsert::SwUndoInsert( const SwNodeIndex& rNd )
    : SwUndo(UNDO_SPLITNODE), pPos( 0 ), pTxt( 0 ),
        pRedlData( 0 ), nNode( rNd.GetIndex() ), nCntnt(0), nLen(1),
        bIsWordDelim( FALSE ), bIsAppend( TRUE )
    , m_nInsertFlags(IDocumentContentOperations::INS_EMPTYEXPAND)
{
    Init(rNd);
}

// stelle fest, ob das naechste Insert mit dem aktuellen zusammengefasst
// werden kann. Wenn ja, dann aender die Laenge und die InsPos.
// Dann wird von SwDoc::Insert kein neues Object in die Undoliste gestellt.

BOOL SwUndoInsert::CanGrouping( sal_Unicode cIns )
{
    if( !bIsAppend && bIsWordDelim ==
        !GetAppCharClass().isLetterNumeric( String( cIns )) )
    {
        nLen++;
        nCntnt++;

        if (pUndoTxt)
            pUndoTxt->Insert(cIns);

        return TRUE;
    }
    return FALSE;
}

BOOL SwUndoInsert::CanGrouping( const SwPosition& rPos )
{
    BOOL bRet = FALSE;
    if( nNode == rPos.nNode.GetIndex() &&
        nCntnt == rPos.nContent.GetIndex() )
    {
        // Redline beachten
        SwDoc& rDoc = *rPos.nNode.GetNode().GetDoc();
        if( ( ~nsRedlineMode_t::REDLINE_SHOW_MASK & rDoc.GetRedlineMode() ) ==
            ( ~nsRedlineMode_t::REDLINE_SHOW_MASK & GetRedlineMode() ) )
        {
            bRet = TRUE;

            // dann war oder ist noch Redline an:
            // pruefe, ob an der InsPosition ein anderer Redline
            // rumsteht. Wenn der gleiche nur einmalig vorhanden ist,
            // kann zusammen gefasst werden.
            const SwRedlineTbl& rTbl = rDoc.GetRedlineTbl();
            if( rTbl.Count() )
            {
                SwRedlineData aRData( nsRedlineType_t::REDLINE_INSERT, rDoc.GetRedlineAuthor() );
                const SwIndexReg* pIReg = rPos.nContent.GetIdxReg();
                SwIndex* pIdx;
                for( USHORT i = 0; i < rTbl.Count(); ++i )
                {
                    SwRedline* pRedl = rTbl[ i ];
                    if( pIReg == (pIdx = &pRedl->End()->nContent)->GetIdxReg() &&
                        nCntnt == pIdx->GetIndex() )
                    {
                        if( !pRedl->HasMark() || !pRedlData ||
                            *pRedl != *pRedlData || *pRedl != aRData )
                        {
                            bRet = FALSE;
                            break;
                        }
                    }
                }
            }
        }
    }
    return bRet;
}

SwUndoInsert::~SwUndoInsert()
{
    if( pPos )      // loesche noch den Bereich aus dem UndoNodes Array
    {
        // Insert speichert den Inhalt in der IconSection
        SwNodes& rUNds = pPos->nNode.GetNode().GetNodes();
        if( pPos->nContent.GetIndex() )         // nicht den gesamten Node loeschen
        {
            SwTxtNode* pTxtNd = pPos->nNode.GetNode().GetTxtNode();
            ASSERT( pTxtNd, "kein TextNode, aus dem geloescht werden soll" );
            pTxtNd->EraseText( pPos->nContent );
            pPos->nNode++;
        }
        pPos->nContent.Assign( 0, 0 );
        rUNds.Delete( pPos->nNode, rUNds.GetEndOfExtras().GetIndex() -
                                    pPos->nNode.GetIndex() );
        delete pPos;
    }
    else if( pTxt )		// der eingefuegte Text
        delete pTxt;
    delete pRedlData;
}



void SwUndoInsert::Undo( SwUndoIter& rUndoIter )
{
    SwDoc* pTmpDoc = &rUndoIter.GetDoc();

    if( bIsAppend )
    {
        SwPaM* pPam = rUndoIter.pAktPam;
        pPam->GetPoint()->nNode = nNode;

        if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        {
            pPam->GetPoint()->nContent.Assign( pPam->GetCntntNode(), 0 );
            pPam->SetMark();
            pPam->Move( fnMoveBackward );
            pPam->Exchange();
            pTmpDoc->DeleteRedline( *pPam, true, USHRT_MAX );
        }
        pPam->DeleteMark();
        pTmpDoc->DelFullPara( *pPam );
        pPam->GetPoint()->nContent.Assign( pPam->GetCntntNode(), 0 );
    }
    else
    {
        ULONG nNd = nNode;
        xub_StrLen nCnt = nCntnt;
        if( nLen )
        {
            SwNodeIndex aNd( pTmpDoc->GetNodes(), nNode);
            SwCntntNode* pCNd = aNd.GetNode().GetCntntNode();
            SwPaM aPaM( *pCNd, nCntnt );

            aPaM.SetMark();

            SwTxtNode * const pTxtNode( pCNd->GetTxtNode() );
            if ( pTxtNode )
            {
                aPaM.GetPoint()->nContent -= nLen;
                if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
                    pTmpDoc->DeleteRedline( aPaM, true, USHRT_MAX );
                RemoveIdxFromRange( aPaM, FALSE );
                pTxt = new String( pTxtNode->GetTxt().Copy(nCntnt-nLen, nLen) );
                pTxtNode->EraseText( aPaM.GetPoint()->nContent, nLen );
            }
            else				// ansonsten Grafik/OLE/Text/...
            {
                aPaM.Move(fnMoveBackward);
                if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
                    pTmpDoc->DeleteRedline( aPaM, true, USHRT_MAX );
                RemoveIdxFromRange( aPaM, FALSE );
            }

            nNd = aPaM.GetPoint()->nNode.GetIndex();
            nCnt = aPaM.GetPoint()->nContent.GetIndex();

            if( !pTxt )
            {
                pPos = new SwPosition( *aPaM.GetPoint() );
                MoveToUndoNds( aPaM, &pPos->nNode, &pPos->nContent );
            }
            nNode = aPaM.GetPoint()->nNode.GetIndex();
            nCntnt = aPaM.GetPoint()->nContent.GetIndex();
        }

        // setze noch den Cursor auf den Undo-Bereich
        rUndoIter.pAktPam->DeleteMark();

        IterPt(rUndoIter)->nNode = nNd;
        IterPt(rUndoIter)->nContent.Assign( pTmpDoc->GetNodes()[
                IterPt(rUndoIter)->nNode ]->GetCntntNode(), nCnt );
        // SPoint und GetMark auf der gleichen Position
    }

    DELETEZ(pUndoTxt);
}


void SwUndoInsert::Redo( SwUndoIter& rUndoIter )
{
    // setze noch den Cursor auf den Redo-Bereich
    SwPaM* pPam = rUndoIter.pAktPam;
    SwDoc* pTmpDoc = pPam->GetDoc();
    pPam->DeleteMark();

    if( bIsAppend )
    {
        pPam->GetPoint()->nNode = nNode - 1;
        pTmpDoc->AppendTxtNode( *pPam->GetPoint() );

        pPam->SetMark();
        pPam->Move( fnMoveBackward );
        pPam->Exchange();

        if( pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        {
            RedlineMode_t eOld = pTmpDoc->GetRedlineMode();
            pTmpDoc->SetRedlineMode_intern((RedlineMode_t)(eOld & ~nsRedlineMode_t::REDLINE_IGNORE));
            pTmpDoc->AppendRedline( new SwRedline( *pRedlData, *pPam ), true);
            pTmpDoc->SetRedlineMode_intern( eOld );
        }
        else if( !( nsRedlineMode_t::REDLINE_IGNORE & GetRedlineMode() ) &&
                pTmpDoc->GetRedlineTbl().Count() )
            pTmpDoc->SplitRedline( *pPam );

        pPam->DeleteMark();
    }
    else
    {
        pPam->GetPoint()->nNode = nNode;
        SwCntntNode* pCNd = pTmpDoc->GetNodes()[ pPam->GetPoint()->nNode ]->GetCntntNode();
        pPam->GetPoint()->nContent.Assign( pCNd, nCntnt );

        if( nLen )
        {
            BOOL bMvBkwrd = MovePtBackward( *pPam );

            if( pTxt )
            {
                SwTxtNode *const pTxtNode = pCNd->GetTxtNode();
                ASSERT( pTxtNode, "where is my textnode ?" );
                pTxtNode->InsertText( *pTxt, pPam->GetMark()->nContent,
                      m_nInsertFlags );
                DELETEZ( pTxt );
            }
            else
            {
                // Inhalt wieder einfuegen. (erst pPos abmelden !!)
                ULONG nMvNd = pPos->nNode.GetIndex();
                xub_StrLen nMvCnt = pPos->nContent.GetIndex();
                DELETEZ( pPos );
                MoveFromUndoNds( *pTmpDoc, nMvNd, nMvCnt, *pPam->GetMark() );
            }
            nNode = pPam->GetMark()->nNode.GetIndex();
            nCntnt = pPam->GetMark()->nContent.GetIndex();

            MovePtForward( *pPam, bMvBkwrd );
            rUndoIter.pAktPam->Exchange();
            if( pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
            {
                RedlineMode_t eOld = pTmpDoc->GetRedlineMode();
                pTmpDoc->SetRedlineMode_intern((RedlineMode_t)(eOld & ~nsRedlineMode_t::REDLINE_IGNORE));
                pTmpDoc->AppendRedline( new SwRedline( *pRedlData,
                                            *rUndoIter.pAktPam ), true);
                pTmpDoc->SetRedlineMode_intern( eOld );
            }
            else if( !( nsRedlineMode_t::REDLINE_IGNORE & GetRedlineMode() ) &&
                    pTmpDoc->GetRedlineTbl().Count() )
                pTmpDoc->SplitRedline( *rUndoIter.pAktPam );
        }
    }

    pUndoTxt = GetTxtFromDoc();
}


void SwUndoInsert::Repeat( SwUndoIter& rUndoIter )
{
    rUndoIter.pLastUndoObj = this;
    if( !nLen )
        return;

    SwDoc& rDoc = rUndoIter.GetDoc();
    SwNodeIndex aNd( rDoc.GetNodes(), nNode );
    SwCntntNode* pCNd = aNd.GetNode().GetCntntNode();;

    if( !bIsAppend && 1 == nLen )		// >1 dann immer nur Text, ansonsten Grafik/OLE/Text/...
    {
        SwPaM aPaM( *pCNd, nCntnt );
        aPaM.SetMark();
        aPaM.Move(fnMoveBackward);
        pCNd = aPaM.GetCntntNode();
    }

// Was passiert mit dem evt. selektierten Bereich ???

    switch( pCNd->GetNodeType() )
    {
    case ND_TEXTNODE:
        if( bIsAppend )
            rDoc.AppendTxtNode( *rUndoIter.pAktPam->GetPoint() );
        else
        {
            String aTxt( ((SwTxtNode*)pCNd)->GetTxt() );
            BOOL bGroupUndo = rDoc.DoesGroupUndo();
            rDoc.DoGroupUndo( FALSE );
            rDoc.InsertString( *rUndoIter.pAktPam,
                aTxt.Copy( nCntnt - nLen, nLen ) );
            rDoc.DoGroupUndo( bGroupUndo );
        }
        break;
    case ND_GRFNODE:
        {
            SwGrfNode* pGrfNd = (SwGrfNode*)pCNd;
            String sFile, sFilter;
            if( pGrfNd->IsGrfLink() )
                pGrfNd->GetFileFilterNms( &sFile, &sFilter );

            rDoc.Insert( *rUndoIter.pAktPam, sFile, sFilter,
                                &pGrfNd->GetGrf(),
                                0/* Grafik-Collection*/, NULL, NULL );
        }
        break;

    case ND_OLENODE:
        {
            // StarView bietet noch nicht die Moeglichkeit ein StarOBJ zu kopieren
            SvStorageRef aRef = new SvStorage( aEmptyStr );
            SwOLEObj& rSwOLE = (SwOLEObj&)((SwOLENode*)pCNd)->GetOLEObj();

            // temporary storage until object is inserted
            // TODO/MBA: seems that here a physical copy is done - not as in drawing layer! Testing!
            // TODO/LATER: Copying through the container would copy the replacement image as well
            comphelper::EmbeddedObjectContainer aCnt;
            ::rtl::OUString aName = aCnt.CreateUniqueObjectName();
            if ( aCnt.StoreEmbeddedObject( rSwOLE.GetOleRef(), aName, sal_True ) )
            {
                uno::Reference < embed::XEmbeddedObject > aNew = aCnt.GetEmbeddedObject( aName );
                rDoc.Insert( *rUndoIter.pAktPam, svt::EmbeddedObjectRef( aNew, ((SwOLENode*)pCNd)->GetAspect() ), NULL, NULL, NULL );
            }

            break;
        }
    }
}

// #111827#
SwRewriter SwUndoInsert::GetRewriter() const
{
    SwRewriter aResult;
    String * pStr = NULL;
    bool bDone = false;

    if (pTxt)
        pStr = pTxt;
    else if (pUndoTxt)
        pStr = pUndoTxt;

    if (pStr)
    {
        String aString = ShortenString(DenoteSpecialCharacters(*pStr),
                                       nUndoStringLength,
                                       String(SW_RES(STR_LDOTS)));

        aResult.AddRule(UNDO_ARG1, aString);

        bDone = true;
    }

    if ( ! bDone )
    {
        aResult.AddRule(UNDO_ARG1, String("??", RTL_TEXTENCODING_ASCII_US));
    }

    return aResult;
}


/*  */

SwUndoReplace::SwUndoReplace()
    : SwUndo( UNDO_REPLACE ), nAktPos( USHRT_MAX )
{
}

SwUndoReplace::~SwUndoReplace()
{
}

void SwUndoReplace::Undo( SwUndoIter& rUndoIter )
{
    // war dieses nicht die letze Undo-Aktion, dann setze den
    // Count neu
    if( rUndoIter.pLastUndoObj != this )
    {
        nAktPos = aArr.Count();
        rUndoIter.pLastUndoObj = this;
        bOldIterFlag = rUndoIter.bWeiter;
        rUndoIter.bWeiter = TRUE;
    }

    aArr[ --nAktPos ]->Undo( rUndoIter );

    if( !nAktPos )		// alten Status wieder zurueck
        rUndoIter.bWeiter = bOldIterFlag;
}


void SwUndoReplace::Redo( SwUndoIter& rUndoIter )
{
    // war dieses nicht die letze Undo-Aktion, dann setze den
    // Count neu
    if( rUndoIter.pLastUndoObj != this )
    {
        ASSERT( !nAktPos, "Redo ohne vorheriges Undo??" );
        rUndoIter.pLastUndoObj = this;
        bOldIterFlag = rUndoIter.bWeiter;
        rUndoIter.bWeiter = TRUE;
    }

    aArr[ nAktPos ]->Redo( rUndoIter );

    if( ++nAktPos >= aArr.Count() )	// alten Status wieder zurueck
    {
        nAktPos = USHRT_MAX;
        rUndoIter.bWeiter = bOldIterFlag;
    }
}

// #111827#
SwRewriter SwUndoReplace::GetRewriter() const
{
    SwRewriter aResult;

    if (aArr.Count() > 1)
    {
        aResult.AddRule(UNDO_ARG1, String::CreateFromInt32(aArr.Count()));
        aResult.AddRule(UNDO_ARG2, String(SW_RES(STR_OCCURRENCES_OF)));

        String aTmpStr;
        aTmpStr += String(SW_RES(STR_START_QUOTE));
        aTmpStr += ShortenString(aArr[0]->GetOld(), nUndoStringLength,
                                 SW_RES(STR_LDOTS));
        aTmpStr += String(SW_RES(STR_END_QUOTE));
        aResult.AddRule(UNDO_ARG3, aTmpStr);
    }
    else if (aArr.Count() == 1)
    {
        {
            String aTmpStr;

            aTmpStr += String(SW_RES(STR_START_QUOTE));
            // #i33488 #
            aTmpStr += ShortenString(aArr[0]->GetOld(), nUndoStringLength,
                                     SW_RES(STR_LDOTS));
            aTmpStr += String(SW_RES(STR_END_QUOTE));
            aResult.AddRule(UNDO_ARG1, aTmpStr);
        }

        aResult.AddRule(UNDO_ARG2, String(SW_RES(STR_YIELDS)));

        {
            String aTmpStr;

            aTmpStr += String(SW_RES(STR_START_QUOTE));
            // #i33488 #
            aTmpStr += ShortenString(aArr[0]->GetIns(), nUndoStringLength,
                                     SW_RES(STR_LDOTS));
            aTmpStr += String(SW_RES(STR_END_QUOTE));
            aResult.AddRule(UNDO_ARG3, aTmpStr);
        }
    }

    return aResult;
}

void SwUndoReplace::AddEntry( const SwPaM& rPam, const String& rInsert,
                                BOOL bRegExp )
{
    _UnReplaceData* pNew = new _UnReplaceData( rPam, rInsert, bRegExp );
    aArr.C40_INSERT(_UnReplaceData, pNew, aArr.Count() );
}

void SwUndoReplace::SetEntryEnd( const SwPaM& rPam )
{
    _UnReplaceData* pEntry = aArr[ aArr.Count()-1 ];
    pEntry->SetEnd( rPam );
}

_UnReplaceData::_UnReplaceData( const SwPaM& rPam, const String& rIns,
                                BOOL bRgExp )
    : m_sIns( rIns ), m_nOffset( 0 )
{
    m_bRegExp = bRgExp;

    const SwPosition * pStt( rPam.Start() );
    const SwPosition * pEnd( rPam.End() );

    m_nSttNd = m_nEndNd = pStt->nNode.GetIndex();
    m_nSttCnt = pStt->nContent.GetIndex();
    m_nSelEnd = m_nEndCnt = pEnd->nContent.GetIndex();

    m_bSplitNext = m_nSttNd != pEnd->nNode.GetIndex();

    SwTxtNode* pNd = pStt->nNode.GetNode().GetTxtNode();
    ASSERT( pNd, "wo ist der TextNode" );

    pHistory = new SwHistory;
    DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );

    m_nSetPos = pHistory->Count();

    ULONG nNewPos = pStt->nNode.GetIndex();
    m_nOffset = m_nSttNd - nNewPos;

    if ( pNd->GetpSwpHints() )
    {
        pHistory->CopyAttr( pNd->GetpSwpHints(), nNewPos, 0,
                            pNd->GetTxt().Len(), true );
    }

    if ( m_bSplitNext )
    {
        if( pNd->HasSwAttrSet() )
            pHistory->CopyFmtAttr( *pNd->GetpSwAttrSet(), nNewPos );
        pHistory->Add( pNd->GetTxtColl(), nNewPos, ND_TEXTNODE );

        SwTxtNode* pNext = pEnd->nNode.GetNode().GetTxtNode();
        ULONG nTmp = pNext->GetIndex();
        pHistory->CopyAttr( pNext->GetpSwpHints(), nTmp, 0,
                            pNext->GetTxt().Len(), true );
        if( pNext->HasSwAttrSet() )
            pHistory->CopyFmtAttr( *pNext->GetpSwAttrSet(), nTmp );
        pHistory->Add( pNext->GetTxtColl(),nTmp, ND_TEXTNODE );
        // METADATA: store
        m_pMetadataUndoStart = pNd  ->CreateUndo();
        m_pMetadataUndoEnd   = pNext->CreateUndo();
    }

    if( !pHistory->Count() )
        delete pHistory, pHistory = 0;

    xub_StrLen nECnt = m_bSplitNext ? pNd->GetTxt().Len()
        : pEnd->nContent.GetIndex();
    m_sOld = pNd->GetTxt().Copy( m_nSttCnt, nECnt - m_nSttCnt );
}

_UnReplaceData::~_UnReplaceData()
{
}

void _UnReplaceData::Undo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SwPaM& rPam = *rIter.pAktPam;
    rPam.DeleteMark();

    SwTxtNode* pNd = pDoc->GetNodes()[ m_nSttNd - m_nOffset ]->GetTxtNode();
    ASSERT( pNd, "Wo ist der TextNode geblieben?" )

    SwAutoCorrExceptWord* pACEWord = pDoc->GetAutoCorrExceptWord();
    if( pACEWord )
    {
        if( 1 == m_sIns.Len() && 1 == m_sOld.Len() )
        {
            SwPosition aPos( *pNd ); aPos.nContent.Assign( pNd, m_nSttCnt );
            pACEWord->CheckChar( aPos, m_sOld.GetChar( 0 ) );
        }
        pDoc->SetAutoCorrExceptWord( 0 );
    }

    SwIndex aIdx( pNd, m_nSttCnt );
    if( m_nSttNd == m_nEndNd )
    {
        pNd->EraseText( aIdx, m_sIns.Len() );
    }
    else
    {
        rPam.GetPoint()->nNode = *pNd;
        rPam.GetPoint()->nContent.Assign( pNd, m_nSttCnt );
        rPam.SetMark();
        rPam.GetPoint()->nNode = m_nEndNd - m_nOffset;
        rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), m_nEndCnt );

        pDoc->DeleteAndJoin( rPam );
        rPam.DeleteMark();
        pNd = rPam.GetNode()->GetTxtNode();
        ASSERT( pNd, "Wo ist der TextNode geblieben?" );
        aIdx.Assign( pNd, m_nSttCnt );
    }

    if( m_bSplitNext )
    {
        SwPosition aPos( *pNd, aIdx );
        pDoc->SplitNode( aPos, false );
        pNd->RestoreMetadata(m_pMetadataUndoEnd);
        pNd = pDoc->GetNodes()[ m_nSttNd - m_nOffset ]->GetTxtNode();
        aIdx.Assign( pNd, m_nSttCnt );
        // METADATA: restore
        pNd->RestoreMetadata(m_pMetadataUndoStart);
    }

    if( m_sOld.Len() )
    {
        pNd->InsertText( m_sOld, aIdx );
    }

    if( pHistory )
    {
        if( pNd->GetpSwpHints() )
            pNd->ClearSwpHintsArr( true );

        pHistory->TmpRollback( pDoc, m_nSetPos, false );
        if ( m_nSetPos ) // there were footnotes/FlyFrames
        {
            // gibts ausser diesen noch andere ?
            if( m_nSetPos < pHistory->Count() )
            {
                // dann sicher die Attribute anderen Attribute
                SwHistory aHstr;
                aHstr.Move( 0, pHistory, m_nSetPos );
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

    rPam.GetPoint()->nNode = m_nSttNd;
    rPam.GetPoint()->nContent = aIdx;
}

void _UnReplaceData::Redo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    BOOL bUndo = rDoc.DoesUndo();
    rDoc.DoUndo( FALSE );

    SwPaM& rPam = *rIter.pAktPam;
    rPam.DeleteMark();
    rPam.GetPoint()->nNode = m_nSttNd;

    SwTxtNode* pNd = rPam.GetPoint()->nNode.GetNode().GetTxtNode();
    ASSERT( pNd, "Wo ist der TextNode geblieben?" )
    rPam.GetPoint()->nContent.Assign( pNd, m_nSttCnt );
    rPam.SetMark();
    if( m_bSplitNext )
    {
        rPam.GetPoint()->nNode = m_nSttNd + 1;
        pNd = rPam.GetPoint()->nNode.GetNode().GetTxtNode();
    }
    rPam.GetPoint()->nContent.Assign( pNd, m_nSelEnd );

    if( pHistory )
    {
        SwHistory* pSave = pHistory;
        SwHistory aHst;
        pHistory = &aHst;
        DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );
        m_nSetPos = pHistory->Count();

        pHistory = pSave;
        pHistory->Move( 0, &aHst );
    }
    else
    {
        pHistory = new SwHistory;
        DelCntntIndex( *rPam.GetMark(), *rPam.GetPoint() );
        m_nSetPos = pHistory->Count();
        if( !m_nSetPos )
            delete pHistory, pHistory = 0;
    }

    rDoc.ReplaceRange( rPam, m_sIns, m_bRegExp );
    rPam.DeleteMark();
    rDoc.DoUndo( bUndo );
}

void _UnReplaceData::SetEnd( const SwPaM& rPam )
{
    if( rPam.GetPoint()->nNode != rPam.GetMark()->nNode )
    {
        // es wurden mehrere Absaetze eingefuegt
        const SwPosition* pEnd = rPam.End();
        m_nEndNd = m_nOffset + pEnd->nNode.GetIndex();
        m_nEndCnt = pEnd->nContent.GetIndex();
    }
}

/*  */


SwUndoReRead::SwUndoReRead( const SwPaM& rPam, const SwGrfNode& rGrfNd )
    : SwUndo( UNDO_REREAD ), nPos( rPam.GetPoint()->nNode.GetIndex() )
{
    SaveGraphicData( rGrfNd );
}


SwUndoReRead::~SwUndoReRead()
{
    delete pGrf;
    delete pNm;
    delete pFltr;
}


void SwUndoReRead::SetAndSave( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    SwGrfNode* pGrfNd = rDoc.GetNodes()[ nPos ]->GetGrfNode();

    if( !pGrfNd )
        return ;

        // die alten Werte zwischen speichern
    Graphic* pOldGrf = pGrf;
    String* pOldNm = pNm;
    String* pOldFltr = pFltr;
    USHORT nOldMirr = nMirr;

    SaveGraphicData( *pGrfNd );
    if( pOldNm )
    {
        pGrfNd->ReRead( *pOldNm, pFltr ? *pFltr : aEmptyStr, 0, 0, TRUE );
        delete pOldNm;
        delete pOldFltr;
    }
    else
    {
        pGrfNd->ReRead( aEmptyStr, aEmptyStr, pOldGrf, 0, TRUE );
        delete pOldGrf;
    }

    if( RES_MIRROR_GRAPH_DONT != nOldMirr )
        pGrfNd->SetAttr( SwMirrorGrf() );

    rIter.pSelFmt = pGrfNd->GetFlyFmt();
}


void SwUndoReRead::Undo( SwUndoIter& rIter )
{
    SetAndSave( rIter );
}


void SwUndoReRead::Redo( SwUndoIter& rIter )
{
    SetAndSave( rIter );
}


void SwUndoReRead::SaveGraphicData( const SwGrfNode& rGrfNd )
{
    if( rGrfNd.IsGrfLink() )
    {
        pNm = new String;
        pFltr = new String;
        rGrfNd.GetFileFilterNms( pNm, pFltr );
        pGrf = 0;
    }
    else
    {
        ((SwGrfNode&)rGrfNd).SwapIn( TRUE );
        pGrf = new Graphic( rGrfNd.GetGrf() );
        pNm = pFltr = 0;
    }
    nMirr = rGrfNd.GetSwAttrSet().GetMirrorGrf().GetValue();
}

/*  */

SwUndoInsertLabel::SwUndoInsertLabel( const SwLabelType eTyp,
                                      const String &rTxt,
                                      const String& rSeparator,
                                      const String& rNumberSeparator,
                                      const BOOL bBef,
                                      const USHORT nInitId,
                                      const String& rCharacterStyle,
                                      const BOOL bCpyBorder )
    : SwUndo( UNDO_INSERTLABEL ),
      sText( rTxt ),
      sSeparator( rSeparator ),
      sNumberSeparator( rNumberSeparator ),//#i61007# order of captions
      sCharacterStyle( rCharacterStyle ),
      nFldId( nInitId ),
      eType( eTyp ),
      nLayerId( 0 ),
      bBefore( bBef ),
      bCpyBrd( bCpyBorder )
{
    bUndoKeep = FALSE;
    OBJECT.pUndoFly = 0;
    OBJECT.pUndoAttr = 0;
}

SwUndoInsertLabel::~SwUndoInsertLabel()
{
    if( LTYPE_OBJECT == eType || LTYPE_DRAW == eType )
    {
        delete OBJECT.pUndoFly;
        delete OBJECT.pUndoAttr;
    }
    else
        delete NODE.pUndoInsNd;
}

void SwUndoInsertLabel::Undo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();

    if( LTYPE_OBJECT == eType || LTYPE_DRAW == eType )
    {
        ASSERT( OBJECT.pUndoAttr && OBJECT.pUndoFly, "Pointer nicht initialisiert" )
        SwFrmFmt* pFmt;
        SdrObject *pSdrObj = 0;
        if( OBJECT.pUndoAttr &&
            0 != (pFmt = (SwFrmFmt*)OBJECT.pUndoAttr->GetFmt( rDoc )) &&
            ( LTYPE_DRAW != eType ||
              0 != (pSdrObj = pFmt->FindSdrObject()) ) )
        {
            OBJECT.pUndoAttr->Undo( rIter );
            OBJECT.pUndoFly->Undo( rIter );
            if( LTYPE_DRAW == eType )
            {
                pSdrObj->SetLayer( nLayerId );
            }
        }
    }
    else if( NODE.nNode )
    {
        if ( eType == LTYPE_TABLE && bUndoKeep )
        {
            SwTableNode *pNd = rDoc.GetNodes()[
                        rDoc.GetNodes()[NODE.nNode-1]->StartOfSectionIndex()]->GetTableNode();
            if ( pNd )
                pNd->GetTable().GetFrmFmt()->ResetFmtAttr( RES_KEEP );
        }
        SwPaM aPam( *rIter.pAktPam->GetPoint() );
        aPam.GetPoint()->nNode = NODE.nNode;
        aPam.SetMark();
        aPam.GetPoint()->nNode = NODE.nNode + 1;
        NODE.pUndoInsNd = new SwUndoDelete( aPam, TRUE );
    }
}


void SwUndoInsertLabel::Redo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();

    if( LTYPE_OBJECT == eType || LTYPE_DRAW == eType )
    {
        ASSERT( OBJECT.pUndoAttr && OBJECT.pUndoFly, "Pointer nicht initialisiert" )
        SwFrmFmt* pFmt;
        SdrObject *pSdrObj = 0;
        if( OBJECT.pUndoAttr &&
            0 != (pFmt = (SwFrmFmt*)OBJECT.pUndoAttr->GetFmt( rDoc )) &&
            ( LTYPE_DRAW != eType ||
              0 != (pSdrObj = pFmt->FindSdrObject()) ) )
        {
            OBJECT.pUndoFly->Redo( rIter );
            OBJECT.pUndoAttr->Redo( rIter );
            if( LTYPE_DRAW == eType )
            {
                pSdrObj->SetLayer( nLayerId );
                if( pSdrObj->GetLayer() == rDoc.GetHellId() )
                    pSdrObj->SetLayer( rDoc.GetHeavenId() );
                // OD 02.07.2003 #108784#
                else if( pSdrObj->GetLayer() == rDoc.GetInvisibleHellId() )
                    pSdrObj->SetLayer( rDoc.GetInvisibleHeavenId() );
            }
        }
    }
    else if( NODE.pUndoInsNd )
    {
        if ( eType == LTYPE_TABLE && bUndoKeep )
        {
            SwTableNode *pNd = rDoc.GetNodes()[
                        rDoc.GetNodes()[NODE.nNode-1]->StartOfSectionIndex()]->GetTableNode();
            if ( pNd )
                pNd->GetTable().GetFrmFmt()->SetFmtAttr( SvxFmtKeepItem(TRUE, RES_KEEP) );
        }
        NODE.pUndoInsNd->Undo( rIter );
        delete NODE.pUndoInsNd, NODE.pUndoInsNd = 0;
    }
}

void SwUndoInsertLabel::Repeat( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    const SwPosition& rPos = *rIter.pAktPam->GetPoint();

    ULONG nIdx = 0;

    SwCntntNode* pCNd = rPos.nNode.GetNode().GetCntntNode();
    if( pCNd )
        switch( eType )
        {
        case LTYPE_TABLE:
            {
                const SwTableNode* pTNd = pCNd->FindTableNode();
                if( pTNd )
                    nIdx = pTNd->GetIndex();
            }
            break;

        case LTYPE_FLY:
        case LTYPE_OBJECT:
            {
                SwFlyFrm* pFly;
                SwCntntFrm *pCnt = pCNd->GetFrm();
                if( pCnt && 0 != ( pFly = pCnt->FindFlyFrm() ) )
                    nIdx = pFly->GetFmt()->GetCntnt().GetCntntIdx()->GetIndex();
            }
            break;
        case LTYPE_DRAW:
            break;
        }

    if( nIdx )
    {
        rDoc.InsertLabel( eType, sText, sSeparator, sNumberSeparator, bBefore,
            nFldId, nIdx, sCharacterStyle, bCpyBrd );
    }
}

// #111827#
SwRewriter SwUndoInsertLabel::GetRewriter() const
{
    SwRewriter aRewriter;

    String aTmpStr;

    aTmpStr += String(SW_RES(STR_START_QUOTE));
    aTmpStr += ShortenString(sText, nUndoStringLength,
                             String(SW_RES(STR_LDOTS)));
    aTmpStr += String(SW_RES(STR_END_QUOTE));

    aRewriter.AddRule(UNDO_ARG1, aTmpStr);

    return aRewriter;
}

void SwUndoInsertLabel::SetFlys( SwFrmFmt& rOldFly, SfxItemSet& rChgSet,
                                SwFrmFmt& rNewFly )
{
    if( LTYPE_OBJECT == eType || LTYPE_DRAW == eType )
    {
        SwUndoFmtAttrHelper aTmp( rOldFly, false );
        rOldFly.SetFmtAttr( rChgSet );
        if ( aTmp.GetUndo() )
        {
            OBJECT.pUndoAttr = aTmp.ReleaseUndo();
        }
        OBJECT.pUndoFly = new SwUndoInsLayFmt( &rNewFly,0,0 );
    }
}

void SwUndoInsertLabel::SetDrawObj( BYTE nLId )
{
    if( LTYPE_DRAW == eType )
    {
        nLayerId = nLId;
    }
}

