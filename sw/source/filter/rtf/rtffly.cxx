/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: rtffly.cxx,v $
 * $Revision: 1.35 $
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
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
#include <hintids.hxx>
#include <tools/list.hxx>
#include <tools/cachestr.hxx>
#include <svtools/rtftoken.h>
#include <svl/itemiter.hxx>
#include <svx/prntitem.hxx>
#include <svx/opaqitem.hxx>
#include <svx/protitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/frmdiritem.hxx>
#include <fmtfsize.hxx>
#include <fmtanchr.hxx>
#include <fmtpdsc.hxx>
#include <fmtsrnd.hxx>
#include <fmtclds.hxx>
#include <fmtcntnt.hxx>
#include <frmatr.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <shellio.hxx>
#include <swparrtf.hxx>
#include <grfatr.hxx>
#include <paratr.hxx>
#include <rtf.hxx>
#include <ndgrf.hxx>
#include <pagedesc.hxx>
#include <swtable.hxx>
#include <txtflcnt.hxx>
#include <fmtflcnt.hxx>
#include <fltini.hxx>
#include <deque>
#include <map>
#include <utility>
// --> OD 2004-06-30 #i27767#
#include <fmtwrapinfluenceonobjpos.hxx>
// <--
#include <svx/brshitem.hxx>
#include <fmtfollowtextflow.hxx>
// --> OD, FLR 2006-02-16 #131205#
#include "dcontact.hxx"
// <--


using namespace ::com::sun::star;

#define ANCHOR(p) 	((SwFmtAnchor*)p)

// steht in shellio.hxx
extern SwCntntNode* GoNextNds( SwNodeIndex * pIdx, BOOL bChk );

SV_IMPL_PTRARR( SwFlySaveArr, SwFlySave* )

inline const SwFmtFrmSize GetFrmSize(const SfxItemSet& rSet, BOOL bInP=TRUE)
{
    return (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE,bInP);
}

SwFlySave::SwFlySave(const SwPaM& rPam, SfxItemSet& rSet)
    : aFlySet(rSet), nSttNd(rPam.GetPoint()->nNode), nEndNd(nSttNd), nEndCnt(0),
     nPageWidth(ATT_MIN_SIZE), nDropLines(0), nDropAnchor(0)
{
}

int SwFlySave::IsEqualFly( const SwPaM& rPos, SfxItemSet& rSet )
{
    if( rSet.Count() != aFlySet.Count() || nDropAnchor )
        return FALSE;

    // nur TextNodes zusammenfassen
    if( nSttNd == nEndNd && nEndNd.GetNode().IsNoTxtNode() )
        return FALSE;

    // teste auf gleiche / naechste Position
    if( rPos.GetPoint()->nNode.GetIndex() == nEndNd.GetIndex() )
    {
        if( 1 < (rPos.GetPoint()->nContent.GetIndex() - nEndCnt) )
            return FALSE;
    }
    else if( rPos.GetPoint()->nContent.GetIndex() )
        return FALSE;
    else
    {
        SwNodeIndex aIdx( nEndNd );
        SwCntntNode* pCNd = rPos.GetDoc()->GetNodes()[ aIdx ]->GetCntntNode();
        if( !GoNextNds( &aIdx, TRUE ) ||
            aIdx.GetIndex() != rPos.GetPoint()->nNode.GetIndex() ||
            ( pCNd && pCNd->Len() != nEndCnt ))
        {
            return FALSE;
        }
    }

    if( rSet.Count() )
    {
        SfxItemIter aIter( rSet );
        const SfxPoolItem *pItem, *pCurr = aIter.GetCurItem();
        while( TRUE )
        {
            if( SFX_ITEM_SET != aFlySet.GetItemState( pCurr->Which(),
                FALSE, &pItem ) ||
                // Ankerattribute gesondert behandeln
                ( RES_ANCHOR == pCurr->Which()
                    ? (ANCHOR(pCurr)->GetAnchorId() != ANCHOR(pItem)->GetAnchorId() ||
                       ANCHOR(pCurr)->GetPageNum() != ANCHOR(pItem)->GetPageNum())
                    : *pItem != *pCurr ))
                        return FALSE;

            if( aIter.IsAtEnd() )
                break;
            pCurr = aIter.NextItem();
        }
    }
    return TRUE;
}

void SwFlySave::SetFlySize( const SwTableNode& rTblNd )
{
    // sollte der Fly kleiner als diese Tabelle sein, dann
    // korrigiere diesen (nur bei abs. Angaben!)
    SwTwips nWidth = rTblNd.GetTable().GetFrmFmt()->GetFrmSize().GetWidth();
    const SwFmtFrmSize& rSz = GetFrmSize( aFlySet );
    if( nWidth > rSz.GetWidth() )
        aFlySet.Put( SwFmtFrmSize( rSz.GetHeightSizeType(), nWidth, rSz.GetHeight() ));
}

BOOL lcl_HasBreakAttrs( const SwCntntNode& rNd )
{
    BOOL bRet = FALSE;
    const SfxItemSet& rSet = rNd.GetSwAttrSet();
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rSet.GetItemState( RES_BREAK, TRUE, &pItem ) &&
        SVX_BREAK_NONE != ((SvxFmtBreakItem*)pItem)->GetBreak() )
        bRet = TRUE;
    else if( SFX_ITEM_SET == rSet.GetItemState( RES_PAGEDESC, TRUE, &pItem )&&
         0 != ((SwFmtPageDesc*)pItem)->GetPageDesc() )
        bRet = TRUE;
    return bRet;
}


void lcl_CpyBreakAttrs( SwCntntNode* pSrcNd, SwCntntNode* pDstNd,
                        SwNodeIndex* pNewIdx )
{
    const SfxItemSet* pSet;
    if( pSrcNd && pDstNd && 0 != ( pSet = pSrcNd->GetpSwAttrSet() ) )
    {
        const SfxPoolItem *pDescItem, *pBreakItem;

        if( SFX_ITEM_SET != pSet->GetItemState( RES_BREAK,
                                        FALSE, &pBreakItem ) )
            pBreakItem = 0;

        if( SFX_ITEM_SET != pSet->GetItemState( RES_PAGEDESC,
                                        FALSE, &pDescItem ) )
            pDescItem = 0;

        if( pDescItem || pBreakItem )
        {
            if( lcl_HasBreakAttrs( *pDstNd ))
            {
                SwPosition aPos( *pDstNd, SwIndex( pDstNd ));
                aPos.nNode--;
                pDstNd->GetDoc()->AppendTxtNode( aPos );
                if( pNewIdx )
                    *pNewIdx = aPos.nNode;

                SwCntntNode* pOldNd = pDstNd;
                pDstNd = aPos.nNode.GetNode().GetCntntNode();
                pDstNd->ChgFmtColl( pOldNd->GetFmtColl() );
                if( pDstNd->HasSwAttrSet() )
                {
                    SfxItemSet aSet( *pDstNd->GetpSwAttrSet() );
                    aSet.ClearItem( RES_BREAK );
                    aSet.ClearItem( RES_PAGEDESC );
                    pDstNd->SetAttr( aSet );
                }
            }
            if( pBreakItem )
            {
                pDstNd->SetAttr( *pBreakItem );
                pSrcNd->ResetAttr( RES_BREAK );
            }
            if( pDescItem )
            {
                pDstNd->SetAttr( *pDescItem );
                pSrcNd->ResetAttr( RES_PAGEDESC );
            }
        }
    }
}

void SwRTFParser::SetFlysInDoc()
{
    // !! von Oben abarbeiten, CntntPos ist kein Index !
    SwNodes & rNds = pDoc->GetNodes();
    typedef std::pair<SwFlyFrmFmt*, SwFmtAnchor> frameEntry;
    typedef std::deque<frameEntry> rtfframesAtIndex;
    typedef std::map<const SwNode*, rtfframesAtIndex> rtfFmtMap;
    rtfFmtMap aPrevFmts;

    SwFrmFmt* pParent = pDoc->GetFrmFmtFromPool( RES_POOLFRM_FRAME );
    for( USHORT n = 0; n < aFlyArr.Count(); ++n )
    {
        SwFlySave* pFlySave = aFlyArr[ n ];

        ASSERT( !pFlySave->nSttNd.GetNode().FindFlyStartNode(),
                "Content vom Fly steht in einem Fly" );
        ASSERT( pFlySave->nSttNd.GetIndex() <= pFlySave->nEndNd.GetIndex(),
                "Fly hat falschen Bereich" );



        //JP 21.09.98: wenn ein DropCap ist, dann Text im Node belassen, am
        //				Absatz das Absatz Attribut setzen. Ggfs noch die
        //				FontSize zuruecksetzen, damit das DropCap nicht zu
        //				gro? wird.
        if( pFlySave->nDropAnchor )
        {
            SwTxtNode* pSttNd = pFlySave->nSttNd.GetNode().GetTxtNode();
            SwTxtNode* pEndNd = pFlySave->nEndNd.GetNode().GetTxtNode();
            if( pSttNd && pEndNd &&
                pSttNd->GetIndex() + 1 == pEndNd->GetIndex()
                && pSttNd->GetTxt().Len()>0 /* #i38227# leave drop caps with no content as fly frames */ )
            {
                ULONG nPos = pSttNd->GetIndex();
                SwDoc * pDoc1 = pSttNd->GetDoc();

                BOOL bJoined;
                {
                    SwPaM aTmp( *pSttNd, pSttNd->GetTxt().Len(), *pEndNd, 0 );
                    bJoined = pDoc1->DeleteAndJoin( aTmp );
                }

                SwTxtNode * pNd = (pDoc1->GetNodes()[nPos])->GetTxtNode();

                if( bJoined && pNd != NULL)
                {
                    SwFmtDrop aDropCap;
                    aDropCap.GetLines() = (BYTE)pFlySave->nDropLines;
                    aDropCap.GetChars() = 1;

                    SwIndex aIdx( pEndNd );
                    pNd->RstAttr( aIdx, 1, RES_CHRATR_FONTSIZE );
                    pNd->SetAttr( aDropCap );
                }
                delete pFlySave;
                continue;
            }
        }

        // liegt Ende und Start vom Naechsten im gleichen Node, dann muss
        // gesplittet werden
        if( n + 1 < aFlyArr.Count() && pFlySave->nEndCnt &&
            pFlySave->nEndNd == aFlyArr[ n + 1 ]->nSttNd )
        {
            SwCntntNode* pCNd = rNds[ pFlySave->nEndNd ]->GetCntntNode();
            if( pCNd )
            {
                SwPosition aPos( pFlySave->nEndNd,
                                SwIndex( pCNd, pFlySave->nEndCnt ));
                pDoc->SplitNode( aPos, false );
                pFlySave->nEndNd--;
            }
            else
                pFlySave->nEndCnt = 0;
        }

        // verschiebe den Inhalt von diesem Anchor in den Auto-TextBereich
        // und erzeuge dadurch den richtigen SwG-Rahmen
        SwNodeRange aRg(pFlySave->nSttNd, 0, pFlySave->nEndNd, 0);
        //Make a new section, unless there is no content at all
        const bool bMakeEmptySection = aRg.aStart < aRg.aEnd || ((aRg.aStart == aRg.aEnd) && pFlySave->nEndCnt);

        {
            // Nur TextNodes koennen in Tabellen stehen !!
            const SwNode* pNd = &pFlySave->nSttNd.GetNode();
            if( pNd->IsNoTxtNode() )
            {
                // die Size muss noch korrigiert werden!
                nAktPageDesc = 0;		// Standart PageDesc
                if( SFX_ITEM_SET != pFlySave->aFlySet.GetItemState(
                    RES_FRM_SIZE, FALSE ) )
                    _SetPictureSize( *(SwNoTxtNode*)pNd, aRg.aStart,
                                    pFlySave->aFlySet );
                if( 0 != ( pNd = pNd->FindTableNode() ) )
                    pFlySave->SetFlySize( *(SwTableNode*)pNd );
            }
            else
            {
                // Take care for table nodes
                pNd = pNd->GetNodes()[ pNd->GetIndex() - 2 ]->GetTableNode();
                if( pNd ) // if the table starts imediately before aRg -> expand aRg
                    aRg.aStart = *pNd;

                if( bMakeEmptySection )
                {
                    pNd = &aRg.aEnd.GetNode();
                    ULONG nSectEnd = pNd->EndOfSectionIndex()+1;

                    if (!pNd->IsTableNode() && 0 !=(pNd = pNd->FindTableNode())
                        && (pNd->GetIndex() >= aRg.aStart.GetNode().GetIndex()) )
                    {
                        const SwNode* pTblBxNd;

                        // Ende der Tabelle ist hinter dieser Box ??
                        if( pNd->EndOfSectionIndex() == nSectEnd )
                            aRg.aEnd = nSectEnd+1;
                        // is the end in the first box of the table, then
                        // move before the table (Bug 67663)
                        // but the range must not become emtpy, i.e. aStart==aEnd
                        // because otherwise we will get a crash (126506) later on
                        else if( 0 != ( pTblBxNd = aRg.aEnd.GetNode().
                                                FindTableBoxStartNode()) &&
                                 pTblBxNd->GetIndex() - 1 == pNd->GetIndex() &&
                                 &aRg.aStart.GetNode() != pNd )
                            aRg.aEnd = *pNd;
                        else
                        {
                            // Tabelle ist noch groesser, also splitte sie hier.
                            rNds.SplitTable( aRg.aEnd, TRUE );
                            aRg.aEnd = pNd->EndOfSectionIndex() + 1;
                        }
                    }
                }
            }
        }

        // vorm verschieben muss sich der Index auf die alte Position
        // gemerkt werden, der Index wird mit verschoben !!!

        SwNodeIndex aTmpIdx( rNds.GetEndOfAutotext() );
        SwStartNode* pSttNd = bMakeEmptySection
                ? rNds.MakeEmptySection( aTmpIdx, SwFlyStartNode )
                : rNds.MakeTextSection( aTmpIdx, SwFlyStartNode,
                        (SwTxtFmtColl*)pDoc->GetDfltTxtFmtColl() );

        // das ist die Verankerungs-Position (fuers Layout!)
        pFlySave->nSttNd = aRg.aStart.GetIndex()-1;
        if( bMakeEmptySection )
        {
            // check: the move does not clear the surrounded section. If all
            // nodes moved away, then create a new TxtNode
            {
                // i76403: an empty selection is not a good idea
                if( aRg.aStart == aRg.aEnd && aRg.aStart.GetNode().GetTxtNode() )
                    aRg.aEnd++;
                SwNodeIndex aPrev( aRg.aStart, -1 );
                if( aPrev.GetNode().IsStartNode() &&
                    aPrev.GetNode().EndOfSectionNode() == &aRg.aEnd.GetNode())
                {
                    // create new txtnode, because the section does never be empty
                    pDoc->GetNodes().MakeTxtNode( aRg.aEnd,
                            (SwTxtFmtColl*)pDoc->GetDfltTxtFmtColl() );
                    aRg.aEnd--;
                }
            }
            aTmpIdx = *pSttNd->EndOfSectionNode();
            pDoc->MoveNodeRange( aRg, aTmpIdx,
                IDocumentContentOperations::DOC_MOVEDEFAULT );
        }

        // patch from cmc for #i52542#
        if (pSttNd->GetIndex() + 1 == pSttNd->EndOfSectionIndex())
        {
            ASSERT(!this, "nothing in this frame, not legal");
            delete pFlySave;
            continue;
        }

        pFlySave->aFlySet.Put( SwFmtCntnt( pSttNd ));

        CalculateFlySize( pFlySave->aFlySet, pFlySave->nSttNd,
                          pFlySave->nPageWidth );

                // THIS >>>>>
        // if the section only contains one Node and this has a
        // border or backgorund, then put it to the frame
        // Not in our own RTF-Format!
                // <<<<< DOES NOT MAKE SENSE TO ME (flr)
        // #102781#. Added support for transparent frames.
        if( pSttNd->GetIndex() + 1 != pSttNd->EndOfSectionIndex() &&
            !bSwPageDesc )
        {
            SwCntntNode* pSrcNd = pDoc->GetNodes()[ pSttNd->GetIndex() + 1 ]->GetCntntNode();
            SfxItemSet aTmpSet( pDoc->GetAttrPool(),
                                    RES_BACKGROUND, RES_BOX );
            const SvxBrushItem* pBackgroundBrush = (const SvxBrushItem*)pFlySave->aFlySet.GetItem(RES_BACKGROUND, FALSE);
            if( pSrcNd && pSrcNd->HasSwAttrSet() )
                aTmpSet.Put( *pSrcNd->GetpSwAttrSet() );
            if (pBackgroundBrush)
            {
                aTmpSet.Put(*pBackgroundBrush, RES_BACKGROUND);
            }
            else
            {
                pBackgroundBrush = (const SvxBrushItem*)aTmpSet.GetItem(RES_BACKGROUND, FALSE);
                if (pBackgroundBrush)
                {
                    Color& rBackgroundColor = const_cast<SvxBrushItem*>(pBackgroundBrush)->GetColor();
                    rBackgroundColor.SetTransparency(0xFE);
                }
                else
                {
                    Color aColor = Color(0xff, 0xff, 0xff);
                    aColor.SetTransparency( 0xFE);
                    SvxBrushItem aBrush(aColor, RES_BACKGROUND);
                    aTmpSet.Put(aBrush, RES_BACKGROUND);
                }
            }
            // #117914# Topic 6.
            pFlySave->aFlySet.Put( aTmpSet );
            if( pSrcNd && pSrcNd->HasSwAttrSet() )
            {
                pSrcNd->ResetAttr( RES_BACKGROUND, RES_BOX );
            }
        }

        SwFlyFrmFmt* pFmt = pDoc->MakeFlyFrmFmt( aEmptyStr, pParent );
        pFmt->SetFmtAttr( pFlySave->aFlySet );
        const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
        if (FLY_AS_CHAR != rAnchor.GetAnchorId())
        {
            // korrigiere noch den Absatz, ist immer der vorhergehende !
            // JP 20.09.95: wenn es diesen gibt! (DocAnfang!)

            //JP 02.08.99: that is wrong. The anchor is ever the NEXT!
            //JP 05.08.99: there are an Bug in the ExportFilter which will
            //				be fixed in the Version 517 - by SWG-Export
            //				the fly will be after the paragraph - but in RTF
            // 				the	flys will be before the paragraph.
            if( !bSwPageDesc || 5430 < GetVersionNo() )
                pFlySave->nSttNd++;

//            if( !pFlySave->nSttNd.GetNode().IsCntntNode() )
            {
                // Seitenumbrueche in den Bodybereich verschieben!
                SwCntntNode* pSrcNd = aRg.aStart.GetNode().GetCntntNode();
                SwCntntNode* pDstNd = pFlySave->nSttNd.GetNode().GetCntntNode();
                if( !pDstNd )
                    pDstNd = pDoc->GetNodes().GoNext( &pFlySave->nSttNd );

                ::lcl_CpyBreakAttrs( pSrcNd, pDstNd, &pFlySave->nSttNd );
            }

            const SwNodeIndex aSttNd(*pSttNd);
            SwNodeIndex aEndNd(*pSttNd->EndOfSectionNode());
            aEndNd--;

            SwPosition aPos( pFlySave->nSttNd );
            SwFmtAnchor aAnchor(rAnchor);
            aAnchor.SetAnchor(&aPos);

            const SwNode *pCurrentAnchor = &(pFlySave->nSttNd.GetNode());
            aPrevFmts[pCurrentAnchor].push_back(frameEntry(pFmt, aAnchor));

            while (aEndNd > aSttNd)
            {
                typedef rtfframesAtIndex::iterator myIter;
                rtfframesAtIndex &rDeque = aPrevFmts[&(aEndNd.GetNode())];
                myIter aEnd = rDeque.end();
                for (myIter aIter = rDeque.begin(); aIter != aEnd; ++aIter)
                {
                    aIter->second.SetAnchor(&aPos);
                    // --> OD 2004-06-30 #i27767# - push on front to keep order
                    // of objects for the correct object positioning
                    //aPrevFmts[pCurrentAnchor].push_back(*aIter);
                    aPrevFmts[pCurrentAnchor].push_front(*aIter);
                }
                rDeque.clear();
                aEndNd--;
           }
        }

        // --> OD, FLR 2006-02-16 #131205#
        // Create draw contact object, which also creates a <SdrObject> instance,
        // in order to set the order number.
        // The order number is assumed to be the order of the text flow.
        SwFlyDrawContact* pContact =
                new SwFlyDrawContact( pFmt,
                                      pFmt->GetDoc()->GetOrCreateDrawModel() );
        pContact->GetMaster()->SetOrdNum( n );
        // <--

        delete pFlySave;
    }

    typedef rtfFmtMap::reverse_iterator myriter;
    myriter aEnd = aPrevFmts.rend();
    for(myriter aIter = aPrevFmts.rbegin(); aIter != aEnd; ++aIter)
    {
        rtfframesAtIndex &rDeque = aIter->second;
        typedef rtfframesAtIndex::iterator myIter;
        myIter aQEnd = rDeque.end();
        for (myIter aQIter = rDeque.begin(); aQIter != aQEnd; ++aQIter)
        {
            frameEntry &rEntry = *aQIter;
            SwFlyFrmFmt *pFrm = rEntry.first;
            SwFmtAnchor &rAnchor = rEntry.second;
            pFrm->SetFmtAttr(rAnchor);
        }
    }

    aFlyArr.Remove(0, aFlyArr.Count());
}

// clips the text box to the min or max position if it is outside our min or max boundry
long SwRTFParser::GetSafePos(long nPos)
{
    if(nPos > SHRT_MAX)
        nPos = SHRT_MAX;
    else if(nPos < SHRT_MIN)
        nPos = SHRT_MIN;

    return nPos;
}

void SwRTFParser::ReadFly( int nToken, SfxItemSet* pSet )
{
    // ein Set fuer die FrmFmt-Attribute
    SfxItemSet aSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( aSet );

    // der Fly beginnt immer in einem neuen Absatz
    if( pPam->GetPoint()->nContent.GetIndex() )
        InsertPara();

    // RTF-Defaults setzen:
    // --> OD 2004-06-24 #i27767#
    SwFmtAnchor aAnchor( FLY_AT_PARA );

    SwFmtHoriOrient aHori( 0, text::HoriOrientation::LEFT, text::RelOrientation::FRAME );
    SwFmtVertOrient aVert( 0, text::VertOrientation::TOP, text::RelOrientation::FRAME );
    // <--
    SvxFrameDirectionItem aFrmDir( FRMDIR_HORI_LEFT_TOP, RES_FRAMEDIR );

    USHORT nCols = USHRT_MAX, nColSpace = USHRT_MAX, nAktCol = 0;
    SvUShorts aColumns;

    BOOL bChkDropCap = 0 == pSet;
    USHORT nDropCapLines = 0, nDropCapAnchor = 0;
    int nNumOpenBrakets = GetOpenBrakets();

    if( !pSet )
    {
        pSet = &aSet;
    }
    else
    {
        // die Werte aus dem uebergebenen!
        aAnchor = (SwFmtAnchor&)pSet->Get( RES_ANCHOR );
        aHori = (SwFmtHoriOrient&)pSet->Get( RES_HORI_ORIENT );
        aVert = (SwFmtVertOrient&)pSet->Get( RES_VERT_ORIENT );
    }

    // dann sammel mal alle Attribute zusammen
    int bWeiter = TRUE;
    int nAppliedProps=0;
    do {
        USHORT nVal = USHORT(nTokenValue);
        /*
        #i5263#
        Assume that a property genuinely contributes towards creating a frame,
        and if turns out to be a non contributing one reduce the count.
        */
        ++nAppliedProps;
        switch( nToken )
        {
        case RTF_ABSW:
            {
                SwFmtFrmSize aSz( ATT_MIN_SIZE, nTokenValue, 0 );
                const SfxPoolItem* pItem;
                if( SFX_ITEM_SET == pSet->GetItemState( RES_FRM_SIZE, TRUE,
                    &pItem ))
                {
                    aSz.SetHeightSizeType( ((SwFmtFrmSize*)pItem)->GetHeightSizeType() );
                    aSz.SetHeight( ((SwFmtFrmSize*)pItem)->GetHeight() );
                }
                if( MINFLY > nTokenValue )	nTokenValue = MINFLY;
                aSet.Put( aSz );
            }
            break;
        case RTF_ABSH:
            {
                SwFmtFrmSize aSz( ATT_MIN_SIZE, 0, MINFLY );
                const SfxPoolItem* pItem;
                if( SFX_ITEM_SET == pSet->GetItemState( RES_FRM_SIZE, TRUE,
                    &pItem ))
                {
                    aSz.SetWidth( ((SwFmtFrmSize*)pItem)->GetWidth() );
                }

                if( 0 > nTokenValue )
                {
                    nTokenValue = -nTokenValue;
                    aSz.SetHeightSizeType( ATT_FIX_SIZE );
                }
                if( MINFLY > nTokenValue )	nTokenValue = MINFLY;
                aSz.SetHeight( nTokenValue );
                aSet.Put( aSz );
            }
            break;

        case RTF_NOWRAP:
            {
                pSet->Put( SwFmtSurround( SURROUND_NONE ));
            }
            break;
        case RTF_DXFRTEXT:
                {
                    SvxULSpaceItem aUL( RES_UL_SPACE );
                    SvxLRSpaceItem aLR( RES_LR_SPACE );
                    aUL.SetUpper( nVal );	aUL.SetLower( nVal );
                    aLR.SetLeft( nVal );	aLR.SetRight( nVal );
                    pSet->Put( aUL );
                    pSet->Put( aLR );
                }
                break;

        case RTF_DFRMTXTX:
                {
                    SvxLRSpaceItem aLR( RES_LR_SPACE );
                    aLR.SetLeft( nVal );	aLR.SetRight( nVal );
                    pSet->Put( aLR );
                }
                break;
        case RTF_DFRMTXTY:
                {
                    SvxULSpaceItem aUL( RES_UL_SPACE );
                    aUL.SetUpper( nVal );	aUL.SetLower( nVal );
                    pSet->Put( aUL );
                }
                break;

        case RTF_POSNEGX:
        case RTF_POSX:      aHori.SetHoriOrient( text::HoriOrientation::NONE );
                            aHori.SetPos( GetSafePos((long)nTokenValue) );
                            break;
        case RTF_POSXC:     aHori.SetHoriOrient( text::HoriOrientation::CENTER );     break;
        case RTF_POSXI:     aHori.SetHoriOrient( text::HoriOrientation::LEFT );
                            aHori.SetPosToggle( TRUE );
                            break;
        case RTF_POSXO:     aHori.SetHoriOrient( text::HoriOrientation::RIGHT );
                            aHori.SetPosToggle( TRUE );
                            break;
        case RTF_POSXL:     aHori.SetHoriOrient( text::HoriOrientation::LEFT );       break;
        case RTF_POSXR:     aHori.SetHoriOrient( text::HoriOrientation::RIGHT );      break;

        case RTF_POSNEGY:
        case RTF_POSY:      aVert.SetVertOrient( text::VertOrientation::NONE );
                            aVert.SetPos( GetSafePos((long)nTokenValue) );
                            break;
        case RTF_POSYT:     aVert.SetVertOrient( text::VertOrientation::TOP );    break;
        case RTF_POSYB:     aVert.SetVertOrient( text::VertOrientation::BOTTOM ); break;
        case RTF_POSYC:     aVert.SetVertOrient( text::VertOrientation::CENTER ); break;

        case RTF_PHMRG:     aHori.SetRelationOrient( text::RelOrientation::PAGE_PRINT_AREA ); break;
        case RTF_PVMRG:     aVert.SetRelationOrient( text::RelOrientation::PAGE_PRINT_AREA ); break;
        case RTF_PHPG:      aHori.SetRelationOrient( text::RelOrientation::PAGE_FRAME ); break;
        case RTF_PVPG:      aVert.SetRelationOrient( text::RelOrientation::PAGE_FRAME );break;
        case RTF_PHCOL:     aHori.SetRelationOrient( text::RelOrientation::FRAME ); break;
        case RTF_PVPARA:    aVert.SetRelationOrient( text::RelOrientation::FRAME ); break;

        case RTF_POSYIL:
            break;
        case RTF_ABSLOCK:
            /*
            #i5263#
            Not sufficient to make a frame at least word won't do it with just
            an abslock
            */
            --nAppliedProps;
            break;
        case RTF_FRMTXLRTB:
            aFrmDir.SetValue( FRMDIR_HORI_LEFT_TOP );
            break;
        case RTF_FRMTXTBRL:
            aFrmDir.SetValue( FRMDIR_HORI_RIGHT_TOP );
            break;
        case RTF_FRMTXLRTBV:
            aFrmDir.SetValue( FRMDIR_VERT_TOP_LEFT );
            break;
        case RTF_FRMTXTBRLV:
            aFrmDir.SetValue( FRMDIR_VERT_TOP_RIGHT );
            break;

        case RTF_DROPCAPLI:							// Dropcaps !!
                if( bChkDropCap )
                {
                    nDropCapLines = USHORT( nTokenValue );
                    if( !nDropCapAnchor )
                        nDropCapAnchor = 1;
                }
                break;
        case RTF_DROPCAPT:
                if( bChkDropCap )
                {
                    nDropCapAnchor = USHORT( nTokenValue );
                    if( !nDropCapLines )
                        nDropCapLines = 3;
                }
                break;


        // fuer die "alten" Writer - haben die Spaltigkeit falsch heraus-
        // geschrieben
        case RTF_COLS:			nCols = USHORT( nTokenValue );		break;
        case RTF_COLSX:			nColSpace = USHORT( nTokenValue );	break;
        case RTF_COLNO:
            nAktCol = USHORT( nTokenValue );
            if( RTF_COLW == GetNextToken() )
            {
                USHORT nWidth = USHORT( nTokenValue ), nSpace = 0;
                if( RTF_COLSR == GetNextToken() )
                    nSpace = USHORT( nTokenValue );
                else
                    SkipToken( -1 );		// wieder zurueck

                if( --nAktCol == ( aColumns.Count() / 2 ) )
                {
                    aColumns.Insert( nWidth + nSpace, aColumns.Count() );
                    aColumns.Insert( nSpace, aColumns.Count() );
                }
            }
            break;

        case '{':
            {
                short nSkip = 0;
                if( RTF_IGNOREFLAG != ( nToken = GetNextToken() ))
                {
                    if( RTF_SHADINGDEF == (nToken & ~0xff) )
                    {
                        ReadBackgroundAttr( nToken, aSet );
                        GetNextToken();		// Klammer ueberlesen
                    }
                    else
                        nSkip = -1;
                }
                else if( RTF_APOCTL ==
                    ((nToken = GetNextToken() ) & ~(0xff | RTF_SWGDEFS)) )
                {
                    bReadSwFly = true;      // alles kommt in den akt. Fly
                    SvxLRSpaceItem aLR( RES_LR_SPACE );
                    SvxULSpaceItem aUL( RES_UL_SPACE );
                    nCols = USHRT_MAX;		// neu aufsetzen
                    nColSpace = USHRT_MAX;
                    do {
                    nVal = USHORT(nTokenValue);
                    switch( nToken )
                    {
                    // Swg-Frame-Tokens
                    case RTF_FLYPRINT:
                        {
                            pSet->Put( SvxPrintItem( RES_PRINT, FALSE ));
                        }
                        break;
                    case RTF_FLYOPAQUE:
                        {
                            pSet->Put( SvxOpaqueItem( RES_OPAQUE, FALSE ));
                        }
                        break;

                    case RTF_FLYPRTCTD:
                        {
                            RTFProtect aP( (BYTE)nTokenValue );
                            SvxProtectItem aProtectItem( RES_PROTECT );
                            aProtectItem.SetCntntProtect( aP.GetCntnt() );
                            aProtectItem.SetSizeProtect( aP.GetSize() );
                            aProtectItem.SetPosProtect( aP.GetPos() );
                            pSet->Put( aProtectItem );
                        }
                        break;

                    case RTF_FLYMAINCNT:
                        {
                            RTFSurround aMC( (BYTE)nTokenValue );
                            SwFmtSurround aSurr( (SwSurround)aMC.GetOrder());
                            if( aMC.GetGoldCut() )
                                aSurr.SetSurround( SURROUND_IDEAL );
                            pSet->Put( aSurr );
                        }
                        break;
                    case RTF_FLYVERT:
                        {
                            RTFVertOrient aVO( nVal );
                            aVert.SetVertOrient( aVO.GetOrient() );
                            aVert.SetRelationOrient( aVO.GetRelation() );
                        }
                        break;
                    case RTF_FLYHORZ:
                        {
                            RTFHoriOrient aHO( nVal );
                            aHori.SetHoriOrient( aHO.GetOrient() );
                            aHori.SetRelationOrient( aHO.GetRelation() );
                        }
                        break;
                    case RTF_FLYOUTLEFT:		aLR.SetLeft( nVal );		break;
                    case RTF_FLYOUTRIGHT:		aLR.SetRight( nVal );		break;
                    case RTF_FLYOUTUPPER:		aUL.SetUpper( nVal );		break;
                    case RTF_FLYOUTLOWER:		aUL.SetLower( nVal );		break;
                    case RTF_FLYANCHOR:
                            switch( GetNextToken() )
                            {
                            case RTF_FLY_PAGE:
                                aAnchor.SetType( FLY_AT_PAGE );
                                aAnchor.SetPageNum( USHORT(nTokenValue));
                                aAnchor.SetAnchor( 0 );
                                break;

                            case RTF_FLY_CNTNT:
                                {
                                    SwNodeIndex aIdx( pPam->GetPoint()->nNode );
                                    pDoc->GetNodes().GoPrevious( &aIdx );
                                    SwPosition aPos( aIdx );
                                    aAnchor.SetType( FLY_AT_PARA );
                                    aAnchor.SetAnchor( &aPos );
                                }
                                break;

// JP 26.09.94: die Bindung an die Spalte gibt es nicht mehr !!
//							case RTF_FLY_COLUMN:
                            }
                            break;
                    case RTF_COLS:	nCols = USHORT( nTokenValue );		break;
                    case RTF_COLSX:	nColSpace = USHORT( nTokenValue );	break;
                    case RTF_COLNO:
                        nAktCol = USHORT( nTokenValue );
                        if( RTF_COLW == GetNextToken() )
                        {
                            USHORT nWidth = USHORT( nTokenValue ), nSpace = 0;
                            if( RTF_COLSR == GetNextToken() )
                                nSpace = USHORT( nTokenValue );
                            else
                                SkipToken( -1 );		// wieder zurueck

                            if( --nAktCol == ( aColumns.Count() / 2 ) )
                            {
                                aColumns.Insert( nWidth + nSpace, aColumns.Count() );
                                aColumns.Insert( nSpace, aColumns.Count() );
                            }
                        }
                        break;

                    case '{':
                        if( RTF_BRDBOX == ( nToken = GetNextToken() ) )
                            ReadBorderAttr( nToken, aSet );
                        else if( RTF_SHADINGDEF == (nToken & ~0xff ) )
                            ReadBackgroundAttr( nToken, aSet );
                        else if( RTF_IGNOREFLAG == nToken )
                        {
                            int bSkipGrp = TRUE;
                            switch( nToken = GetNextToken() )
                            {
                            case RTF_SHADOW:
                            case RTF_BRDBOX:
                                ReadAttr( SkipToken( -2 ), &aSet );
                                bSkipGrp = FALSE;
                                break;

                            case RTF_BRDRT:
                            case RTF_BRDRB:
                            case RTF_BRDRR:
                            case RTF_BRDRL:
                                bSkipGrp = FALSE;
                                ReadBorderAttr( SkipToken( -2 ), aSet );
                                break;
                            }

                                // keine weitere Klammer mehr ueberlesen!!!
                            if( !bSkipGrp )
                                break;

                            SkipGroup();
                        }
                        else
                            SkipGroup();
                        GetNextToken();		// Klammer ueberlesen
                        break;
                    }
                    } while( IsParserWorking() &&
                                '}' != ( nToken = GetNextToken() ));

                    if( aUL.GetUpper() || aUL.GetLower() )
                        pSet->Put( aUL );
                    if( aLR.GetLeft() || aLR.GetRight() )
                        pSet->Put( aLR );
                }
                else if( RTF_BRDBOX == nToken )
                    ReadBorderAttr( nToken, aSet );
                else if( RTF_SHADOW == nToken )
                    ReadAttr( SkipToken( -2 ), &aSet );
                else if( RTF_SHADINGDEF == (nToken & ~0xff ) )
                    ReadBackgroundAttr( nToken, aSet );
                else if( RTF_UNKNOWNCONTROL == nToken )
                    SkipGroup();
                else
                    nSkip = -2;

                if( nSkip )
                {
                    nToken = SkipToken( nSkip );
                    bWeiter = FALSE;
                }
            }
            break;

        default:
            --nAppliedProps; //Not sufficient to make a frame
            bWeiter = FALSE;
        }

        if( bWeiter )
            nToken = GetNextToken();
    } while( bWeiter && IsParserWorking() );

    pSet->Put( aAnchor );
    pSet->Put( aHori );
    pSet->Put( aVert );

    // --> OD 2004-06-30 #i27767# - set wrapping style influence
    // --> OD 2004-10-18 #i35017# - constant name has changed
    pSet->Put( SwFmtWrapInfluenceOnObjPos(
                    text::WrapInfluenceOnPosition::ONCE_SUCCESSIVE ));
    // <--

    SwFmtFollowTextFlow aFollowTextFlow( FALSE );
    pSet->Put( aFollowTextFlow );

    if( !( aFrmDir == pSet->Get( RES_FRAMEDIR )) )
        pSet->Put( aFrmDir );

    if( nCols && USHRT_MAX != nCols )
    {
        SwFmtCol aCol;
        if( USHRT_MAX == nColSpace )
            nColSpace = 720;

        ULONG nWidth = USHRT_MAX;
        aCol.Init( nCols, nColSpace, USHORT( nWidth ) );
        if( nCols == ( aColumns.Count() / 2 ) )
        {
            for( USHORT n = 0, i = 0; n < aColumns.Count(); n += 2, ++i )
            {
                SwColumn* pCol = aCol.GetColumns()[ i ];
                ULONG nTmp = aColumns[ n ];
                nTmp *= USHRT_MAX;
                nTmp /= nWidth;
                pCol->SetWishWidth( USHORT(nTmp) );
/*
    JP 07.07.95: der Dialog kennt nur eine Breite fuer alle Spalten
                 darum hier nicht weiter beachten
                nTmp = aColumns[ n+1 ];
                if( nTmp )
                    pCol->SetRight( USHORT(nTmp) );
                else
                    pCol->SetRight( 0 );
                pCol->SetLeft( 0 );
*/
            }
        }
        pSet->Put( aCol );
    }

    if( pSet != &aSet )			// wurde der Set uebergeben, dann wars das
        return ;

    // ein neues FlyFormat anlegen oder das alte benutzen ?
    // (teste ob es die selben Attribute besitzt!)
    SwFlySave* pFlySave = 0;
    USHORT nFlyArrCnt = aFlyArr.Count();
    /*
    #i5263#
    There were not enough frame properties found to actually justify creating
    an absolutely positioned frame.
    */
    if (nAppliedProps)
    {
        if( !nFlyArrCnt ||
            !( pFlySave = aFlyArr[ nFlyArrCnt-1 ])->IsEqualFly( *pPam, aSet ))
        {
            pFlySave = new SwFlySave( *pPam, aSet );
            Size aPgSize;
            GetPageSize( aPgSize );
            pFlySave->nPageWidth = aPgSize.Width();

            if( nDropCapAnchor )
            {
                pFlySave->nDropAnchor = nDropCapAnchor;
                pFlySave->nDropLines = nDropCapLines;
            }
            if (nFlyArrCnt >0){
                SwFlySave* pFlySavePrev = aFlyArr[nFlyArrCnt-1];
                if (pFlySave->nSttNd.GetIndex() < pFlySavePrev->nEndNd.GetIndex())
                {
                     pFlySavePrev->nEndNd=pFlySave->nSttNd;
                }
            }
            aFlyArr.Insert(  pFlySave, nFlyArrCnt++ );
            // --> OD 2008-12-22 #i83368# - reset
            mbReadCellWhileReadSwFly = false;
            // <--
        }
    }

    SetPardTokenRead( FALSE );
    const SwTableNode* pTblNd = pPam->GetNode()->FindTableNode();

    while( !IsPardTokenRead() && IsParserWorking() )
    {
        if( RTF_PARD == nToken || nNumOpenBrakets > GetOpenBrakets() )
            break;

        NextToken( nToken );

        if( !IsPardTokenRead() )
        {
            // #102781#. Added support for transparent frames.
            if (nToken == RTF_CBPAT && nFlyArrCnt > 0)
            {
                USHORT _index=USHORT(nTokenValue);
                const Color& rColor = GetColor(_index);
                SvxBrushItem aBrush(rColor, RES_BACKGROUND);
                SwFlySave* pFS = aFlyArr[nFlyArrCnt-1];
                pFS->aFlySet.Put(aBrush, RES_BACKGROUND);
            }

            nToken = GetNextToken();

            // BUG 22036: kommt zwischen Fly-Attributen ein unbekanntes,
            //				dann erzeuge nie 2 FlyFrames, sondern fasse
            //				beide zusammen !!!
            while( RTF_APOCTL == ( nToken & ~(0xff | RTF_SWGDEFS) ))
            {
                if( RTF_FLY_INPARA == nToken )
                    break;

                if( RTF_IGNOREFLAG == SkipToken( -1 ) )
                {
                    if( '{' == SkipToken( -1 ) )
                        nToken = '{';
                    else
                        SkipToken( 2 );
                }
                else
                    SkipToken( 1 );

                ReadFly( nToken, pFlySave ? &pFlySave->aFlySet : 0);
                nToken = GetNextToken();
            }
        }
    }

    /*
    #i5263#
    There were enough frame properties found to actually justify creating
    an absolutely positioned frame.
    */
    if (!nAppliedProps)
    {
        bReadSwFly = false;
        SkipToken( -1 );
        return;
    }

    if( pTblNd && !pPam->GetPoint()->nContent.GetIndex() &&
        pTblNd->EndOfSectionIndex() + 1 ==
            pPam->GetPoint()->nNode.GetIndex() )
    {
        // nicht mehr in der Tabelle, sondern dahinter ?
        // Dann aber wieder zurueck in die Tabelle
        pPam->Move( fnMoveBackward );
    }
    else
        pTblNd = 0;

    // wurde garnichts eingefuegt?
    if( !pTblNd &&
        pPam->GetPoint()->nNode == pFlySave->nSttNd &&
        !pPam->GetPoint()->nContent.GetIndex() )
    {
//		// dann erzeuge mindestens einen leeren TextNode
//		pDoc->AppendTxtNode(*pPam);
        // dann zerstoere den FlySave wieder.
        aFlyArr.DeleteAndDestroy( --nFlyArrCnt );

    }
    else
    {
        BOOL bMovePaM = 0 != pTblNd;

        pFlySave->nEndNd = pPam->GetPoint()->nNode;
        pFlySave->nEndCnt = pPam->GetPoint()->nContent.GetIndex();

        if( bMovePaM )
            pPam->Move( fnMoveForward );

        pTblNd = pFlySave->nSttNd.GetNode().FindTableNode();
        if( pTblNd && !pFlySave->nEndCnt &&
            pTblNd == pFlySave->nEndNd.GetNode().FindTableNode() )
        {
            // dann teste mal, ob das \pard nicht zu spaet kam und
            // eigentlich in die vorherige Zelle gehoert
            const SwStartNode* pSttBoxNd = pFlySave->nSttNd.GetNode().
                                            FindTableBoxStartNode(),
                            * pEndBoxNd = pFlySave->nEndNd.GetNode().
                                            FindTableBoxStartNode();
            if( pSttBoxNd && pEndBoxNd &&
                bMovePaM ? ( pSttBoxNd == pEndBoxNd )
                         : ( pSttBoxNd->EndOfSectionIndex() + 1 ==
                                pEndBoxNd->GetIndex() &&
                                pEndBoxNd->GetIndex() + 1 ==
                                pFlySave->nEndNd.GetIndex() ))
            {
                // dann gehoert das Ende in die vorherige Box!
                SwPosition aPos( *pPam->GetPoint() );
                pPam->GetPoint()->nNode = *pSttBoxNd->EndOfSectionNode();
                pPam->Move( fnMoveBackward, fnGoNode );

                DelLastNode();

                pPam->GetPoint()->nNode = *pSttBoxNd->EndOfSectionNode();
                pPam->Move( fnMoveBackward, fnGoNode );

                pFlySave->nEndNd = pPam->GetPoint()->nNode;
                pFlySave->nEndCnt = pPam->GetPoint()->nContent.GetIndex();

                *pPam->GetPoint() = aPos;
            }
        }
        else if( !bReadSwFly && !pFlySave->nEndCnt &&
            pFlySave->nSttNd.GetIndex() + 1 == pFlySave->nEndNd.GetIndex() &&
            pFlySave->nSttNd.GetNode().IsTxtNode() )
        {

            SwTxtNode* pTxtNd = pFlySave->nSttNd.GetNode().GetTxtNode();
            SwTxtFlyCnt* pFlyCnt = 0;
            if( 1 == pTxtNd->GetTxt().Len() &&
                0 != ( pFlyCnt = (SwTxtFlyCnt*)pTxtNd->GetTxtAttr(
                                                0, RES_TXTATR_FLYCNT )) &&
                pFlyCnt->GetFlyCnt().GetFrmFmt() )
            {
                // then move the content into the surrounded fly
                SwFrmFmt* pFlyFmt = pFlyCnt->GetFlyCnt().GetFrmFmt();
                const SwNodeIndex* pFlySNd = pFlyFmt->GetCntnt().GetCntntIdx();
                SwNodeRange aRg( *pFlySNd, 1,
                                 *pFlySNd->GetNode().EndOfSectionNode(), 0 );

                // merge the itemsets
                SwFmtFrmSize aSz1( (SwFmtFrmSize&)pFlyFmt->GetAttrSet().
                                                Get( RES_FRM_SIZE ));
                SwFmtFrmSize aSz2( (SwFmtFrmSize&)pFlySave->aFlySet.
                                                Get( RES_FRM_SIZE ));
                // if
                if( !aRg.aStart.GetNode().IsNoTxtNode() ||
                    !aSz1.GetHeight() || !aSz1.GetWidth() ||
                    !aSz2.GetHeight() || !aSz2.GetWidth() ||
                    ( aSz1.GetHeight() == aSz2.GetHeight() &&
                      aSz1.GetWidth() == aSz2.GetWidth() ) )
                {
                    SfxItemSet aDiffs( pFlyFmt->GetAttrSet() );
                    aDiffs.ClearItem( RES_ANCHOR );
                    aDiffs.ClearItem( RES_FRM_SIZE );
                    aDiffs.ClearItem( RES_CNTNT );
                    aDiffs.Differentiate( pFlySave->aFlySet );
                    pFlySave->aFlySet.Put( aDiffs );

                    BOOL bSet = FALSE;
                    if( aSz1.GetHeight() && !aSz2.GetHeight() )
                    {
                        bSet = TRUE;
                        aSz2.SetHeight( aSz1.GetHeight() );
                    }
                    if( aSz1.GetWidth() && !aSz2.GetWidth() )
                    {
                        bSet = TRUE;
                        aSz2.SetWidth( aSz1.GetWidth() );
                    }
                    if( bSet )
                        pFlySave->aFlySet.Put( aSz2 );

                    // move any PageBreak/Desc Attr to the next Para
                    {
                        SwCntntNode* pSrcNd = pFlySave->nSttNd.GetNode().GetCntntNode();
                        SwCntntNode* pDstNd = pFlySave->nEndNd.GetNode().GetCntntNode();

                        ::lcl_CpyBreakAttrs( pSrcNd, pDstNd, &pFlySave->nEndNd );
                    }

                    // create new txtnode, because the section does never be empty
                    pDoc->GetNodes().MakeTxtNode( aRg.aStart,
                                (SwTxtFmtColl*)pDoc->GetDfltTxtFmtColl() );

                    SwNodeIndex aTmp( pFlySave->nSttNd, +1 );
                    pDoc->MoveNodeRange( aRg, aTmp,
                            IDocumentContentOperations::DOC_MOVEDEFAULT );

                    // now delete the redundant txtnode
                    pDoc->GetNodes().Delete( pFlySave->nSttNd, 1 );
                }
            }
        }
    }

    bReadSwFly = false;
    SkipToken( -1 );
}


void SwRTFParser::InsPicture( const String& rGrfNm, const Graphic* pGrf,
                                const SvxRTFPictureType* pPicType )
{
    // kennzeichen fuer Swg-Dokumente:
    // (dann ist das FlyFmt fuer die Grafik!)
    SwGrfNode * pGrfNd;
    // --> OD 2008-12-22 #i83368#
    // Assure that graphic node is enclosed by fly frame node.
//    if( bReadSwFly )
    if ( bReadSwFly && !mbReadCellWhileReadSwFly )
    // <--
    {
        // erzeuge nur einen normalen GrafikNode und ersetze diesen gegen
        // den vorhandenen Textnode
        SwNodeIndex& rIdx = pPam->GetPoint()->nNode;
        pGrfNd = pDoc->GetNodes().MakeGrfNode( rIdx,
                    rGrfNm, aEmptyStr,    // Name der Graphic !!
                    pGrf,
                    (SwGrfFmtColl*)pDoc->GetDfltGrfFmtColl() );

        if( pGrfAttrSet )
            pGrfNd->SetAttr( *pGrfAttrSet );

        SwFlySave* pFlySave = aFlyArr[ aFlyArr.Count()-1 ];
        pFlySave->nSttNd = rIdx.GetIndex() - 1;

        if( 1 < aFlyArr.Count() )
        {
            pFlySave = aFlyArr[ aFlyArr.Count() - 2 ];
            if( pFlySave->nEndNd == rIdx )
                pFlySave->nEndNd = rIdx.GetIndex() - 1;
        }
    }
    else
    {
        // wenn normale RTF-Grafik, dann steht diese im Textfluss !
        SwAttrSet aFlySet( pDoc->GetAttrPool(), RES_OPAQUE, /*RES_OPAQUE,
                                                RES_VERT_ORIENT,*/ RES_ANCHOR );
        const SwPosition* pPos = pPam->GetPoint();

        SwFmtAnchor aAnchor( FLY_AS_CHAR );
        aAnchor.SetAnchor( pPos );
        aFlySet.Put( aAnchor );
        aFlySet.Put( SwFmtVertOrient( 0, text::VertOrientation::TOP ));

        if (pDoc->IsInHeaderFooter(pPos->nNode))
        {
            SvxOpaqueItem aOpaqueItem(RES_OPAQUE, FALSE);
            SwFmtSurround aSurroundItem(SURROUND_THROUGHT);
            aFlySet.Put(aOpaqueItem);
            aFlySet.Put(aSurroundItem);
        }

        SwFrmFmt* pFlyFmt = pDoc->Insert( *pPam,
                    rGrfNm, aEmptyStr,		// Name der Graphic !!
                    pGrf,
                    &aFlySet,				// Attribute fuer den FlyFrm
                    pGrfAttrSet, NULL );			// Attribute fuer die Grafik

        pGrfNd = pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()->
                                            GetIndex()+1 ]->GetGrfNode();

        _SetPictureSize( *pGrfNd, pPos->nNode,
                        (SfxItemSet&)pFlyFmt->GetAttrSet(),
                        pPicType );
    }

    if( pGrfAttrSet )
        DELETEZ( pGrfAttrSet );
}

void SwRTFParser::_SetPictureSize( const SwNoTxtNode& rNd,
                                    const SwNodeIndex& rAnchor,
                                    SfxItemSet& rSet,
                                    const SvxRTFPictureType* pPicType )
{
    Size aSize( ((SwNoTxtNode&)rNd).GetTwipSize() );
    if( pPicType )
    {
        if( rNd.IsGrfNode() )
        {
            if( SvxRTFPictureType::WIN_METAFILE != pPicType->eStyle &&
                pPicType->nGoalWidth && pPicType->nGoalHeight )
            {
                aSize.Width() = pPicType->nGoalWidth;
                aSize.Height() =pPicType->nGoalHeight;
            }
            else if( SvxRTFPictureType::MAC_QUICKDRAW == pPicType->eStyle )
            {
                // IMMER auf 72 DPI bezogen, also 1pt == 20 Twip !!
                aSize.Width() = pPicType->nWidth * 20;
                aSize.Height() = pPicType->nHeight * 20;
            }
            else
            {
                // von 100TH_MM nach TWIP umrechenen!
                // #117879# when \picwgoal resp \pichgoal are present, then use them.
                //          The values of \picwgoal and \picwgoal are already given in twips.
                aSize.Width() = (pPicType->nGoalWidth?pPicType->nGoalWidth:(pPicType->nWidth*144)/254);
                aSize.Height() = (pPicType->nGoalHeight?pPicType->nGoalHeight:(pPicType->nHeight*144)/254);
            }
            ((SwGrfNode&)rNd).SetTwipSize( aSize );
        }

        if( 100 != pPicType->nScalX )
            aSize.Width() = (((long)pPicType->nScalX) * ( aSize.Width() -
                        ( pPicType->nCropL + pPicType->nCropR ))) / 100L;

        if( 100 != pPicType->nScalY )
            aSize.Height() = (((long)pPicType->nScalY) * ( aSize.Height() -
                        ( pPicType->nCropT + pPicType->nCropB ))) / 100L;
    }

    //steht der Fly etwa in einer Tabelle ?
    const SwNode* pAnchorNd = pDoc->GetNodes()[ rAnchor ];
    const SwTableNode* pTblNd = pAnchorNd->FindTableNode();
    if( pTblNd )
    {
        // Box feststellen:
        const SwTableBox* pBox = pTblNd->GetTable().GetTblBox(
                                pAnchorNd->StartOfSectionIndex() );
        if( pBox )
        {
            long nBoxWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth();
            if( aSize.Width() > nBoxWidth )
                aSize.Width() = nBoxWidth;
        }
    }

    //JP 8.11.2001: bug 94450 - if no size exist, then the size is set by
    //				the swapin of the graphic.
    SwGrfNode* pGrfNd;
    if( !aSize.Width() && !aSize.Height() &&
        0 != (pGrfNd = (SwGrfNode*)rNd.GetGrfNode() ) && pGrfNd->IsGrfLink() )
        pGrfNd->SetChgTwipSize( TRUE );

        // min. Werte einhalten !!
    if( aSize.Width() < MINFLY )
        aSize.Width() = MINFLY;
    if( aSize.Height() < MINFLY)
        aSize.Height() = MINFLY;

    if( pPicType )
    {
        BOOL bChg = FALSE;
        SwCropGrf aCrop;

/*
 JP 28.07.99: Bug 67800 - no crop by MAC_QUICKDRAW. At time i dont know why
                            it has been coded. But this has used for any
                            RTF-File, but i dont found them.
        if( SvxRTFPictureType::MAC_QUICKDRAW == pPicType->eStyle )
        {
            // evt. ein wenig Croppen ??
            // IMMER auf 72 DPI bezogen, also 1pt == 20 Twip !!
            long nTmp = pPicType->nWidth * 20;
            if( nTmp != aSize.Width() )
            {
                // in der Breite (also rechts) croppen
                aCrop.Right() = nTmp - aSize.Width();
                aSize.Width() = nTmp;
                bChg = TRUE;
            }

            nTmp = pPicType->nHeight * 20;
            if( nTmp != aSize.Height() )
            {
                // in der Hoehe (also unten) croppen
                aCrop.Bottom() = nTmp - aSize.Height();
                aSize.Height() = nTmp;
                bChg = TRUE;
            }
        }
*/
        if( pPicType->nCropT )
        {
            aCrop.SetTop( pPicType->nCropT );
            bChg = TRUE;
        }
        if( pPicType->nCropB )
        {
            aCrop.SetBottom( pPicType->nCropB );
            bChg = TRUE;
        }
        if( pPicType->nCropL )
        {
            aCrop.SetLeft( pPicType->nCropL );
            bChg = TRUE;
        }
        if( pPicType->nCropR )
        {
            aCrop.SetRight( pPicType->nCropR );
            bChg = TRUE;
        }

        if( bChg )
        {
            // dann mal an die CropWerte an die GrafikSize anpassen.
            ((SwNoTxtNode&)rNd).SetAttr( aCrop );
        }
    }
    rSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, aSize.Width(), aSize.Height() ));
}

void SwRTFParser::GetPageSize( Size& rSize )
{
    ASSERT(!maSegments.empty(), "not possible");
    if (maSegments.empty())
    {
        rSize.Width() = 12240 - 1800 - 1800;
        rSize.Height() = 15840 - 1440 - 1440;
        return;
    }

    const rtfSection &rSect = maSegments.back();

    rSize.Width() = rSect.maPageInfo.mnPgwsxn - rSect.maPageInfo.mnMarglsxn - rSect.maPageInfo.mnMargrsxn;
    rSize.Height() = rSect.maPageInfo.mnPghsxn - rSect.maPageInfo.mnMargtsxn - rSect.maPageInfo.mnMargbsxn;

    long nCols = rSect.NoCols();
    if (1 < nCols)
    {
        rSize.Width() /= nCols;
        rSize.Height() /= nCols;
    }
}

void SwRTFParser::ReadBitmapData()
{
    Graphic aGrf;
    SvxRTFPictureType aPicType;
    if( ReadBmpData( aGrf, aPicType ) )
        InsPicture( aEmptyStr, &aGrf, &aPicType );
}

#ifdef READ_OLE_OBJECT
void SwRTFParser::ReadOLEData()
{
    SvCacheStream aTmpFile( 0xA000 );
    Graphic aGrf;
    SvxRTFPictureType aPicType, aOleType;

    int nToken, bValidOle = TRUE, bWeiter = TRUE;
    int nOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!

    String* pStr = 0;
    String sObjClass, sObjName, sObjData;

    while( nOpenBrakets && IsParserWorking() && bWeiter && bValidOle )
    {
        nToken = GetNextToken();
        USHORT nVal = USHORT( nTokenValue );
        switch( nToken )
        {
        case '}':		--nOpenBrakets;	pStr = 0; break;
        case '{':
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                else if( RTF_UNKNOWNCONTROL != GetNextToken() )
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;
                }
                ++nOpenBrakets;
            }
            break;

        case RTF_OBJECT:
        case RTF_OBJEMB:		// default ist embedded
        case RTF_LINKSELF:		// ??
        case RTF_OBJLOCK:		// ??
        case RTF_OBJUPDATE:		// ??
        case RTF_OBJTIME:		// ??
        case RTF_OBJSETSIZE:
        case RTF_OBJALIGN:
        case RTF_OBJTRANSY:
        case RTF_OBJATTPH:
            break;

        case RTF_OBJLINK:		// ?? welche Daten sind das ??
        case RTF_OBJAUTLINK:	// ??       -""-            ??
        case RTF_OBJSUB:
        case RTF_OBJPUB:
        case RTF_OBJICEMB:
        case RTF_OBJOCX:
        case RTF_OBJHTML:
        case RTF_OBJALIAS:
        case RTF_OBJSECT:
            bValidOle = FALSE;		// diese Typen koennen wir nicht
            break;

        case RTF_OBJCLASS:
            // Daten lesen
            pStr = &sObjClass;
            break;

        case RTF_OBJNAME:
            // Daten lesen
            pStr = &sObjName;
            break;

        case RTF_OBJDATA:
            pStr = &sObjData;
            break;

        case RTF_RESULT:
            {
                // hier weitermachen, wenn das OLE-Object ungueltig ist
                bWeiter = FALSE;
            }
            break;
        case RTF_RSLTBMP:			// diese sollten wir ignorieren
        case RTF_RSLTMERGE:
        case RTF_RSLTPICT:
        case RTF_RSLTRTF:
        case RTF_RSLTTXT:
            break;

        case RTF_OBJW:			aOleType.nWidth = nVal; break;
        case RTF_OBJH:			aOleType.nHeight = nVal; break;
        case RTF_OBJCROPT:		aOleType.nCropT = (short)nTokenValue; break;
        case RTF_OBJCROPB:		aOleType.nCropB = (short)nTokenValue; break;
        case RTF_OBJCROPL:		aOleType.nCropL = (short)nTokenValue; break;
        case RTF_OBJCROPR:		aOleType.nCropR = (short)nTokenValue; break;
        case RTF_OBJSCALEX:		aOleType.nScalX = nVal; break;
        case RTF_OBJSCALEY:		aOleType.nScalY = nVal; break;

        case RTF_TEXTTOKEN:
            if( 1 < nOpenBrakets && pStr )
            {
                if( pStr == &sObjData )
                {
                    xub_StrLen nHexLen = HexToBin( aToken );
                    if( STRING_NOTFOUND != nHexLen )
                        bValidOle = FALSE;
                    else
                    {
                        aTmpFile.Write( (sal_Char*)aToken.GetBuffer(), nHexLen );
                        bValidOle = 0 == aTmpFile.GetError();
                    }
                }
                else
                    *pStr += aToken;
            }
            break;
        }
    }

    if( bValidOle )
    {
        bValidOle = FALSE; 		// erstmal
    }

    if( !bWeiter )		// dann stehen wir noch im Result
    {
        // ist das Ole-Object Ok?
        // -> dann solange SkipGroup rufen, bis zur letzten
        //		schliessenden Klammer
        // ansonsten alle Token verarbeiten, bis zur letzten
        //		schliessenden Klammer

        bWeiter = TRUE;
        while( nOpenBrakets && IsParserWorking() && bWeiter )
        {
            switch( nToken = GetNextToken() )
            {
            case '}':		--nOpenBrakets; break;
            case '{':		++nOpenBrakets;	 break;
            }
            if( nOpenBrakets && !bValidOle )
                NextToken( nToken );
        }
    }

    if( !bValidOle && '}' != nToken )
        SkipGroup();

    SkipToken( -1 );		// die schliesende Klammer wird "oben" ausgewertet
}
#endif

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
