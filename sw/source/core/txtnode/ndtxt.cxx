/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ndtxt.cxx,v $
 * $Revision: 1.86.66.1 $
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
#include <hints.hxx>

#include <svx/fontitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/escpitem.hxx>
#include <svx/lrspitem.hxx>
// --> OD 2008-01-17 #newlistlevelattrs#
#include <svx/tstpitem.hxx>
// <--
#include <svtools/urihelper.hxx>
#ifndef _SVSTDARR_HXX
#define _SVSTDARR_ULONGS
#include <svtools/svstdarr.hxx>
#endif
#include <svtools/ctloptions.hxx>
#include <swmodule.hxx>
#include <txtfld.hxx>
#include <txtinet.hxx>
#include <fmtinfmt.hxx>
#include <fmtpdsc.hxx>
#include <txtatr.hxx>
#include <fmthbsh.hxx>
#include <fmtrfmrk.hxx>
#include <txttxmrk.hxx>
#include <fchrfmt.hxx>
#include <txtftn.hxx>
#include <fmtflcnt.hxx>
#include <fmtfld.hxx>
#include <frmatr.hxx>
#include <charatr.hxx>
#include <ftnidx.hxx>
#include <ftninfo.hxx>
#include <fmtftn.hxx>
#include <charfmt.hxx>
#include <ndtxt.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <pam.hxx>					// fuer SwPosition
#include <fldbas.hxx>
#include <errhdl.hxx>
#include <paratr.hxx>
#include <txtfrm.hxx>
#include <ftnfrm.hxx>
#include <ftnboss.hxx>
#include <rootfrm.hxx>
#include <pagedesc.hxx>				// fuer SwPageDesc
#include <expfld.hxx>				// fuer SwTblField
#include <section.hxx>				// fuer SwSection
#include <mvsave.hxx>
#include <swcache.hxx>
#include <SwGrammarMarkUp.hxx>
#include <dcontact.hxx>
#include <redline.hxx>
#include <doctxm.hxx>
#include <IMark.hxx>
#include <scriptinfo.hxx>
#include <istyleaccess.hxx>
#include <SwStyleNameMapper.hxx>
#include <numrule.hxx>

//--> #outlinelevel added by zhaojianwei
#include <svtools/intitem.hxx>
//<--end
#include <swtable.hxx>
#include <docsh.hxx>
#include <SwNodeNum.hxx>
// --> OD 2008-02-25 #refactorlists#
#include <svtools/intitem.hxx>
#include <list.hxx>
// <--

SV_DECL_PTRARR( TmpHints, SwTxtAttr*, 0, 4 )

TYPEINIT1( SwTxtNode, SwCntntNode )

SV_DECL_PTRARR(SwpHts,SwTxtAttr*,1,1)

// Leider ist das SwpHints nicht ganz wasserdicht:
// Jeder darf an den Hints rumfummeln, ohne die Sortierreihenfolge
// und Verkettung sicherstellen zu muessen.
#ifdef DBG_UTIL
#define CHECK_SWPHINTS(pNd)  { if( pNd->GetpSwpHints() && \
                                   !pNd->GetDoc()->IsInReading() ) \
                                  pNd->GetpSwpHints()->Check(); }
#else
#define CHECK_SWPHINTS(pNd)
#endif

SwTxtNode *SwNodes::MakeTxtNode( const SwNodeIndex & rWhere,
                                 SwTxtFmtColl *pColl,
                                 SwAttrSet* pAutoAttr )
{
    ASSERT( pColl, "Collectionpointer ist 0." );

    SwTxtNode *pNode = new SwTxtNode( rWhere, pColl, pAutoAttr );

    SwNodeIndex aIdx( *pNode );

    // --> OD 2005-11-03 #125329#
    // call method <UpdateOutlineNode(..)> only for the document nodes array
    if ( IsDocNodes() )
    {
        //if ( pColl && NO_NUMBERING != pColl->GetOutlineLevel() )	//#outline level,removed by zhaojianwei
        //{
        //	UpdateOutlineNode( *pNode, NO_NUMBERING, pColl->GetOutlineLevel() );
        //}
//        if ( pColl && 0 != pColl->GetAttrOutlineLevel() )//#outline level,added by zhaojianwei
//        {
//            UpdateOutlineNode( *pNode, 0, pColl->GetAttrOutlineLevel() );
//        }//<--end
//        else
        {
            UpdateOutlineNode(*pNode);
        }
    }
    // <--

    //Wenn es noch kein Layout gibt oder in einer versteckten Section
    // stehen, brauchen wir uns um das MakeFrms nicht bemuehen.
    const SwSectionNode* pSectNd;
    if( !GetDoc()->GetRootFrm() ||
        ( 0 != (pSectNd = pNode->FindSectionNode()) &&
            pSectNd->GetSection().IsHiddenFlag() ))
        return pNode;

    SwNodeIndex aTmp( rWhere );
    do {
        // max. 2 Durchlaeufe:
        // 1. den Nachfolger nehmen
        // 2. den Vorgaenger

        SwNode *pNd;
        switch( ( pNd = (*this)[aTmp] )->GetNodeType() )
        {
        case ND_TABLENODE:
            ((SwTableNode*)pNd)->MakeFrms( aIdx );
            return pNode;

        case ND_SECTIONNODE:
            if( ((SwSectionNode*)pNd)->GetSection().IsHidden() ||
                ((SwSectionNode*)pNd)->IsCntntHidden() )
            {
                SwNodeIndex aTmpIdx( *pNode );
                pNd = FindPrvNxtFrmNode( aTmpIdx, pNode );
                if( !pNd )
                    return pNode;
                aTmp = *pNd;
                break;
            }
            ((SwSectionNode*)pNd)->MakeFrms( aIdx );
            return pNode;

        case ND_TEXTNODE:
        case ND_GRFNODE:
        case ND_OLENODE:
            ((SwCntntNode*)pNd)->MakeFrms( *pNode );
            return pNode;

        case ND_ENDNODE:
            if( pNd->StartOfSectionNode()->IsSectionNode() &&
                aTmp.GetIndex() < rWhere.GetIndex() )
            {
                if( pNd->StartOfSectionNode()->GetSectionNode()->GetSection().IsHiddenFlag())
                {
                    if( !GoPrevSection( &aTmp, TRUE, FALSE ) ||
                        aTmp.GetNode().FindTableNode() !=
                            pNode->FindTableNode() )
                        return pNode;		// schade, das wars
                }
                else
                    aTmp = *pNd->StartOfSectionNode();
                break;
            }
            else if( pNd->StartOfSectionNode()->IsTableNode() &&
                    aTmp.GetIndex() < rWhere.GetIndex() )
            {
                // wir stehen hinter einem TabellenNode
                aTmp = *pNd->StartOfSectionNode();
                break;
            }
            // kein break !!!
        default:
            if( rWhere == aTmp )
                aTmp -= 2;
            else
                return pNode;
            break;
        }
    } while( TRUE );
}

// --------------------
// SwTxtNode
// --------------------

SwTxtNode::SwTxtNode( const SwNodeIndex &rWhere,
                      SwTxtFmtColl *pTxtColl,
                      const SfxItemSet* pAutoAttr )
    : SwCntntNode( rWhere, ND_TEXTNODE, pTxtColl ),
      m_pSwpHints( 0 ),
      mpNodeNum( 0 ),
      m_bLastOutlineState( false ),
      m_bNotifiable( false ),
      //nOutlineLevel( pTxtColl->GetOutlineLevel() )//#outline level, removed by zhaojianwei.
      // --> OD 2008-11-19 #i70748#
      mbEmptyListStyleSetDueToSetOutlineLevelAttr( false ),
      // <--
      // --> OD 2008-05-06 #refactorlists#
      mbInSetOrResetAttr( false ),
      mpList( 0 )
      // <--
{
    InitSwParaStatistics( true );

    // soll eine Harte-Attributierung gesetzt werden?
    if( pAutoAttr )
        SetAttr( *pAutoAttr );

    // --> OD 2008-03-13 #refactorlists# - no longed needed
//    SyncNumberAndNumRule();
    if ( !IsInList() && GetNumRule() && GetListId().Len() > 0 )
    {
        AddToList();
    }
    // <--
    GetNodes().UpdateOutlineNode(*this);

    m_bNotifiable = true;

    m_bContainsHiddenChars = m_bHiddenCharsHidePara = false;
    m_bRecalcHiddenCharFlags = true;
}

SwTxtNode::~SwTxtNode()
{
    // delete loescht nur die Pointer, nicht die Arrayelemente!
    if ( m_pSwpHints )
    {
        // damit Attribute die ihren Inhalt entfernen nicht doppelt
        // geloescht werden.
        SwpHints* pTmpHints = m_pSwpHints;
        m_pSwpHints = 0;

        for( USHORT j = pTmpHints->Count(); j; )
            // erst muss das Attribut aus dem Array entfernt werden,
            // denn sonst wuerde es sich selbst loeschen (Felder) !!!!
            DestroyAttr( pTmpHints->GetTextHint( --j ) );

        delete pTmpHints;
    }

    // --> OD 2008-03-13 #refactorlists#
//    if ( mpNodeNum )
//    {
//        mpNodeNum->RemoveMe();
//        delete mpNodeNum;
//        mpNodeNum = 0L;
//    }
    RemoveFromList();
    // <--

    InitSwParaStatistics( false );
}

SwCntntFrm *SwTxtNode::MakeFrm()
{
    SwCntntFrm *pFrm = new SwTxtFrm(this);
    return pFrm;
}

xub_StrLen SwTxtNode::Len() const
{
    return m_Text.Len();
}

/*---------------------------------------------------------------------------
 * lcl_ChangeFtnRef
 * 	After a split node, it's necessary to actualize the ref-pointer of the
 *  ftnfrms.
 * --------------------------------------------------------------------------*/

void lcl_ChangeFtnRef( SwTxtNode &rNode )
{
    SwpHints *pSwpHints = rNode.GetpSwpHints();
    if( pSwpHints && rNode.GetDoc()->GetRootFrm() )
    {
        SwTxtAttr* pHt;
        SwCntntFrm* pFrm = NULL;
        // OD 07.11.2002 #104840# - local variable to remember first footnote
        // of node <rNode> in order to invalidate position of its first content.
        // Thus, in its <MakeAll()> it will checked its position relative to its reference.
        SwFtnFrm* pFirstFtnOfNode = 0;
        for( USHORT j = pSwpHints->Count(); j; )
        {
            pHt = pSwpHints->GetTextHint(--j);
            if (RES_TXTATR_FTN == pHt->Which())
            {
                if( !pFrm )
                {
                    SwClientIter aNew( rNode );
                    pFrm = (SwCntntFrm*)aNew.First( TYPE(SwCntntFrm) );
                    if( !pFrm )
                        return;
                }
                SwTxtFtn *pAttr = (SwTxtFtn*)pHt;
                ASSERT( pAttr->GetStartNode(), "FtnAtr ohne StartNode." );
                SwNodeIndex aIdx( *pAttr->GetStartNode(), 1 );
                SwCntntNode *pNd = aIdx.GetNode().GetCntntNode();
                if ( !pNd )
                    pNd = pFrm->GetAttrSet()->GetDoc()->
                            GetNodes().GoNextSection( &aIdx, TRUE, FALSE );
                if ( !pNd )
                    continue;
                SwClientIter aIter( *pNd );
                SwCntntFrm* pCntnt = (SwCntntFrm*)aIter.First(TYPE(SwCntntFrm));
                if( pCntnt )
                {
                    ASSERT( pCntnt->FindRootFrm() == pFrm->FindRootFrm(),
                            "lcl_ChangeFtnRef: Layout double?" );
                    SwFtnFrm *pFtn = pCntnt->FindFtnFrm();
                    if( pFtn && pFtn->GetAttr() == pAttr )
                    {
                        while( pFtn->GetMaster() )
                            pFtn = pFtn->GetMaster();
                        // OD 07.11.2002 #104840# - remember footnote frame
                        pFirstFtnOfNode = pFtn;
                        while ( pFtn )
                        {
                            pFtn->SetRef( pFrm );
                            pFtn = pFtn->GetFollow();
                            ((SwTxtFrm*)pFrm)->SetFtn( TRUE );
                        }
                    }
#ifdef DBG_UTIL
                    while( 0 != (pCntnt = (SwCntntFrm*)aIter.Next()) )
                    {
                        SwFtnFrm *pDbgFtn = pCntnt->FindFtnFrm();
                        ASSERT( !pDbgFtn || pDbgFtn->GetRef() == pFrm,
                                "lcl_ChangeFtnRef: Who's that guy?" );
                    }
#endif
                }
            }
        } // end of for-loop on <SwpHints>
        // OD 08.11.2002 #104840# - invalidate
        if ( pFirstFtnOfNode )
        {
            SwCntntFrm* pCntnt = pFirstFtnOfNode->ContainsCntnt();
            if ( pCntnt )
            {
                pCntnt->_InvalidatePos();
            }
        }
    }
}

SwCntntNode *SwTxtNode::SplitCntntNode( const SwPosition &rPos )
{
    // lege den Node "vor" mir an
    xub_StrLen nSplitPos = rPos.nContent.GetIndex();
    xub_StrLen nTxtLen = m_Text.Len();
    SwTxtNode* pNode = _MakeNewTxtNode( rPos.nNode, FALSE, nSplitPos==nTxtLen );

    // the first paragraph gets the XmlId,
    // _except_ if it is empty and the second is not empty
    if (nSplitPos != 0) {
        pNode->RegisterAsCopyOf(*this, true);
        if (nSplitPos == nTxtLen)
        {
            this->RemoveMetadataReference();
            // NB: SwUndoSplitNode will call pNode->JoinNext,
            // which is sufficient even in this case!
        }
    }

    // --> OD 2008-03-27 #refactorlists#
//    // --> OD 2007-07-09 #i77372#
//    // reset numbering attribute at current node, only if it is numbered.
//    if ( GetNumRule() != NULL )
//    {
//        SetRestart(false);
//        SetStart(1);
//        SetCounted(true);
//    }
    ResetAttr( RES_PARATR_LIST_ISRESTART );
    ResetAttr( RES_PARATR_LIST_RESTARTVALUE );
    ResetAttr( RES_PARATR_LIST_ISCOUNTED );
    if ( GetNumRule() == 0 )
    {
        ResetAttr( RES_PARATR_LIST_ID );
        ResetAttr( RES_PARATR_LIST_LEVEL );
    }
    // <--

    if ( GetDepends() && m_Text.Len() && (nTxtLen / 2) < nSplitPos )
    {
// JP 25.04.95: Optimierung fuer SplitNode:
//				Wird am Ende vom Node gesplittet, dann verschiebe die
//				Frames vom akt. auf den neuen und erzeuge fuer den akt.
//				neue. Dadurch entfaellt das neu aufbauen vom Layout.

        LockModify();	// Benachrichtigungen abschalten

        // werden FlyFrames mit verschoben, so muessen diese nicht ihre
        // Frames zerstoeren. Im SwTxtFly::SetAnchor wird es abgefragt!
        if ( HasHints() )
        {
            pNode->GetOrCreateSwpHints().SetInSplitNode(true);
        }

        //Ersten Teil des Inhalts in den neuen Node uebertragen und
        //im alten Node loeschen.
        SwIndex aIdx( this );
        Cut( pNode, aIdx, nSplitPos );

        if( GetWrong() )
            pNode->SetWrong( GetWrong()->SplitList( nSplitPos ) );
        SetWrongDirty( true );

        if( GetGrammarCheck() )
            pNode->SetGrammarCheck( GetGrammarCheck()->SplitGrammarList( nSplitPos ) );
        SetGrammarCheckDirty( true );

        SetWordCountDirty( true );

        // SMARTTAGS
        if( GetSmartTags() )
            pNode->SetSmartTags( GetSmartTags()->SplitList( nSplitPos ) );
        SetSmartTagDirty( true );

        if ( pNode->HasHints() )
        {
            if ( pNode->m_pSwpHints->CanBeDeleted() )
            {
                delete pNode->m_pSwpHints;
                pNode->m_pSwpHints = 0;
            }
            else
            {
                pNode->m_pSwpHints->SetInSplitNode(false);
            }

            // alle zeichengebundenen Rahmen, die im neuen Absatz laden
            // muessen aus den alten Frame entfernt werden:
            // JP 01.10.96: alle leeren und nicht zu expandierenden
            //				Attribute loeschen
            if ( HasHints() )
            {
                SwTxtAttr* pHt;
                xub_StrLen* pEnd;
                for ( USHORT j = m_pSwpHints->Count(); j; )
                {
                    pHt = m_pSwpHints->GetTextHint( --j );
                    if ( RES_TXTATR_FLYCNT == pHt ->Which() )
                    {
                        pHt->GetFlyCnt().GetFrmFmt()->DelFrms();
                    }
                    else if( pHt->DontExpand() && 0 != ( pEnd = pHt->GetEnd() )
                            && *pHt->GetStart() == *pEnd )
                    {
                        // delete it!
                        m_pSwpHints->DeleteAtPos( j );
                        DestroyAttr( pHt );
                    }
                }
            }

        }

        SwClientIter aIter( *this );
        SwClient* pLastFrm = aIter.GoStart();
        if( pLastFrm )
            do
            {   SwCntntFrm *pFrm = PTR_CAST( SwCntntFrm, pLastFrm );
                if ( pFrm )
                {
                    pNode->Add( pFrm );
                    if( pFrm->IsTxtFrm() && !pFrm->IsFollow() &&
                        ((SwTxtFrm*)pFrm)->GetOfst() )
                        ((SwTxtFrm*)pFrm)->SetOfst( 0 );
                }
            } while( 0 != ( pLastFrm = aIter++ ));

        if ( IsInCache() )
        {
            SwFrm::GetCache().Delete( this );
            SetInCache( FALSE );
        }

        UnlockModify();	// Benachrichtigungen wieder freischalten

        const SwRootFrm *pRootFrm;
        // If there is an accessible layout we must call modify even
        // with length zero, because we have to notify about the changed
        // text node.
        if( nTxtLen != nSplitPos ||
            ( (pRootFrm = pNode->GetDoc()->GetRootFrm()) != 0 &&
              pRootFrm->IsAnyShellAccessible() ) )

        {
            // dann sage den Frames noch, das am Ende etwas "geloescht" wurde
            if( 1 == nTxtLen - nSplitPos )
            {
                SwDelChr aHint( nSplitPos );
                pNode->SwModify::Modify( 0, &aHint );
            }
            else
            {
                SwDelTxt aHint( nSplitPos, nTxtLen - nSplitPos );
                pNode->SwModify::Modify( 0, &aHint );
            }
        }
        if ( HasHints() )
        {
            MoveTxtAttr_To_AttrSet();
        }
        pNode->MakeFrms( *this );		// neue Frames anlegen.
        lcl_ChangeFtnRef( *this );
    }
    else
    {
        SwWrongList *pList = GetWrong();
        SetWrong( 0, false );
        SetWrongDirty( true );

        SwGrammarMarkUp *pList3 = GetGrammarCheck();
        SetGrammarCheck( 0, false );
        SetGrammarCheckDirty( true );

        SetWordCountDirty( true );

        // SMARTTAGS
        SwWrongList *pList2 = GetSmartTags();
        SetSmartTags( 0, false );
        SetSmartTagDirty( true );

        SwIndex aIdx( this );
        Cut( pNode, aIdx, rPos.nContent.GetIndex() );

        // JP 01.10.96: alle leeren und nicht zu expandierenden
        //				Attribute loeschen
        if ( HasHints() )
        {
            SwTxtAttr* pHt;
            xub_StrLen* pEnd;
            for ( USHORT j = m_pSwpHints->Count(); j; )
            {
                pHt = m_pSwpHints->GetTextHint( --j );
                pEnd = pHt->GetEnd();
                if ( pHt->DontExpand() && pEnd && (*pHt->GetStart() == *pEnd) )
                {
                    // delete it!
                    m_pSwpHints->DeleteAtPos( j );
                    DestroyAttr( pHt );
                }
            }
            MoveTxtAttr_To_AttrSet();
        }

        if( pList )
        {
            pNode->SetWrong( pList->SplitList( nSplitPos ) );
            SetWrong( pList, false );
        }

        if( pList3 )
        {
            pNode->SetGrammarCheck( pList3->SplitGrammarList( nSplitPos ) );
            SetGrammarCheck( pList3, false );
        }

        // SMARTTAGS
        if( pList2 )
        {
            pNode->SetSmartTags( pList2->SplitList( nSplitPos ) );
            SetSmartTags( pList2, false );
        }

        if ( GetDepends() )
            MakeFrms( *pNode );		// neue Frames anlegen.
        lcl_ChangeFtnRef( *pNode );
    }

    {
        //Hint fuer Pagedesc versenden. Das mueste eigntlich das Layout im
        //Paste der Frames selbst erledigen, aber das fuehrt dann wiederum
        //zu weiteren Folgefehlern, die mit Laufzeitkosten geloest werden
        //muesten. #56977# #55001# #56135#
        const SfxPoolItem *pItem;
        if( GetDepends() && SFX_ITEM_SET == pNode->GetSwAttrSet().
            GetItemState( RES_PAGEDESC, TRUE, &pItem ) )
            pNode->Modify( (SfxPoolItem*)pItem, (SfxPoolItem*)pItem );
    }
    return pNode;
}

void SwTxtNode::MoveTxtAttr_To_AttrSet()
{
    ASSERT( m_pSwpHints, "MoveTxtAttr_To_AttrSet without SwpHints?" );
    for ( USHORT i = 0; m_pSwpHints && i < m_pSwpHints->Count(); ++i )
    {
        SwTxtAttr *pHt = m_pSwpHints->GetTextHint(i);

        if( *pHt->GetStart() )
            break;

        const xub_StrLen* pHtEndIdx = pHt->GetEnd();

        if( !pHtEndIdx )
            continue;

        if ( *pHtEndIdx < m_Text.Len() || pHt->IsCharFmtAttr() )
            break;

        if( !pHt->IsDontMoveAttr() &&
            SetAttr( pHt->GetAttr() ) )
        {
            m_pSwpHints->DeleteAtPos(i);
            DestroyAttr( pHt );
            --i;
        }
    }

}

SwCntntNode *SwTxtNode::JoinNext()
{
    SwNodes& rNds = GetNodes();
    SwNodeIndex aIdx( *this );
    if( SwCntntNode::CanJoinNext( &aIdx ) )
    {
        SwDoc* pDoc = rNds.GetDoc();
        SvULongs aBkmkArr( 15, 15 );
        _SaveCntntIdx( pDoc, aIdx.GetIndex(), USHRT_MAX, aBkmkArr, SAVEFLY );
        SwTxtNode *pTxtNode = aIdx.GetNode().GetTxtNode();
        xub_StrLen nOldLen = m_Text.Len();

        // METADATA: merge
        this->JoinMetadatable(*pTxtNode, !this->Len(), !pTxtNode->Len());

        SwWrongList *pList = GetWrong();
        if( pList )
        {
            pList->JoinList( pTxtNode->GetWrong(), nOldLen );
            SetWrongDirty( true );
            SetWrong( 0, false );
        }
        else
        {
            pList = pTxtNode->GetWrong();
            if( pList )
            {
                pList->Move( 0, nOldLen );
                SetWrongDirty( true );
                pTxtNode->SetWrong( 0, false );
            }
        }

        SwGrammarMarkUp *pList3 = GetGrammarCheck();
        if( pList3 )
        {
            pList3->JoinGrammarList( pTxtNode->GetGrammarCheck(), nOldLen );
            SetGrammarCheckDirty( true );
            SetGrammarCheck( 0, false );
        }
        else
        {
            pList3 = pTxtNode->GetGrammarCheck();
            if( pList3 )
            {
                pList3->MoveGrammar( 0, nOldLen );
                SetGrammarCheckDirty( true );
                pTxtNode->SetGrammarCheck( 0, false );
            }
        }

        // SMARTTAGS
        SwWrongList *pList2 = GetSmartTags();
        if( pList2 )
        {
            pList2->JoinList( pTxtNode->GetSmartTags(), nOldLen );
            SetSmartTagDirty( true );
            SetSmartTags( 0, false );
        }
        else
        {
            pList2 = pTxtNode->GetSmartTags();
            if( pList2 )
            {
                pList2->Move( 0, nOldLen );
                SetSmartTagDirty( true );
                pTxtNode->SetSmartTags( 0, false );
            }
        }

        { // wg. SwIndex
            pTxtNode->Cut( this, SwIndex(pTxtNode), pTxtNode->Len() );
        }
        // verschiebe noch alle Bookmarks/TOXMarks
        if( aBkmkArr.Count() )
            _RestoreCntntIdx( pDoc, aBkmkArr, GetIndex(), nOldLen );

        if( pTxtNode->HasAnyIndex() )
        {
            // alle Crsr/StkCrsr/UnoCrsr aus dem Loeschbereich verschieben
            pDoc->CorrAbs( aIdx, SwPosition( *this ), nOldLen, TRUE );
        }
        rNds.Delete(aIdx);
        SetWrong( pList, false );
        SetGrammarCheck( pList3, false );
        SetSmartTags( pList2, false ); // SMARTTAGS
        InvalidateNumRule();
    }
    else {
        ASSERT( FALSE, "kein TxtNode." );
    }

    return this;
}

SwCntntNode *SwTxtNode::JoinPrev()
{
    SwNodes& rNds = GetNodes();
    SwNodeIndex aIdx( *this );
    if( SwCntntNode::CanJoinPrev( &aIdx ) )
    {
        SwDoc* pDoc = rNds.GetDoc();
        SvULongs aBkmkArr( 15, 15 );
        _SaveCntntIdx( pDoc, aIdx.GetIndex(), USHRT_MAX, aBkmkArr, SAVEFLY );
        SwTxtNode *pTxtNode = aIdx.GetNode().GetTxtNode();
        xub_StrLen nLen = pTxtNode->Len();

        SwWrongList *pList = pTxtNode->GetWrong();
        if( pList )
        {
            pList->JoinList( GetWrong(), Len() );
            SetWrongDirty( true );
            pTxtNode->SetWrong( 0, false );
            SetWrong( NULL );
        }
        else
        {
            pList = GetWrong();
            if( pList )
            {
                pList->Move( 0, nLen );
                SetWrongDirty( true );
                SetWrong( 0, false );
            }
        }

        SwGrammarMarkUp *pList3 = pTxtNode->GetGrammarCheck();
        if( pList3 )
        {
            pList3->JoinGrammarList( GetGrammarCheck(), Len() );
            SetGrammarCheckDirty( true );
            pTxtNode->SetGrammarCheck( 0, false );
            SetGrammarCheck( NULL );
        }
        else
        {
            pList3 = GetGrammarCheck();
            if( pList3 )
            {
                pList3->MoveGrammar( 0, nLen );
                SetGrammarCheckDirty( true );
                SetGrammarCheck( 0, false );
            }
        }

        // SMARTTAGS
        SwWrongList *pList2 = pTxtNode->GetSmartTags();
        if( pList2 )
        {
            pList2->JoinList( GetSmartTags(), Len() );
            SetSmartTagDirty( true );
            pTxtNode->SetSmartTags( 0, false );
            SetSmartTags( NULL );
        }
        else
        {
            pList2 = GetSmartTags();
            if( pList2 )
            {
                pList2->Move( 0, nLen );
                SetSmartTagDirty( true );
                SetSmartTags( 0, false );
            }
        }

        { // wg. SwIndex
            pTxtNode->Cut( this, SwIndex( this ), SwIndex(pTxtNode), nLen );
        }
        // verschiebe noch alle Bookmarks/TOXMarks
        if( aBkmkArr.Count() )
            _RestoreCntntIdx( pDoc, aBkmkArr, GetIndex() );

        if( pTxtNode->HasAnyIndex() )
        {
            // alle Crsr/StkCrsr/UnoCrsr aus dem Loeschbereich verschieben
            pDoc->CorrAbs( aIdx, SwPosition( *this ), nLen, TRUE );
        }
        rNds.Delete(aIdx);
        SetWrong( pList, false );
        SetGrammarCheck( pList3, false );
        SetSmartTags( pList2, false );
        InvalidateNumRule();
    }
    else {
        ASSERT( FALSE, "kein TxtNode." );
    }

    return this;
}

// erzeugt einen AttrSet mit Bereichen fuer Frame-/Para/Char-Attributen
void SwTxtNode::NewAttrSet( SwAttrPool& rPool )
{
    ASSERT( !mpAttrSet.get(), "AttrSet ist doch gesetzt" );
    SwAttrSet aNewAttrSet( rPool, aTxtNodeSetRange );

    // put names of parent style and conditional style:
    const SwFmtColl* pAnyFmtColl = &GetAnyFmtColl();
    const SwFmtColl* pFmtColl = GetFmtColl();
    String sVal;
    SwStyleNameMapper::FillProgName( pAnyFmtColl->GetName(), sVal, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
    SfxStringItem aAnyFmtColl( RES_FRMATR_STYLE_NAME, sVal );
    if ( pFmtColl != pAnyFmtColl )
        SwStyleNameMapper::FillProgName( pFmtColl->GetName(), sVal, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
    SfxStringItem aFmtColl( RES_FRMATR_CONDITIONAL_STYLE_NAME, sVal );
    aNewAttrSet.Put( aAnyFmtColl );
    aNewAttrSet.Put( aFmtColl );

    aNewAttrSet.SetParent( &pAnyFmtColl->GetAttrSet() );
    mpAttrSet = GetDoc()->GetIStyleAccess().getAutomaticStyle( aNewAttrSet, IStyleAccess::AUTO_STYLE_PARA );
}


// Ueberladen der virtuellen Update-Methode von SwIndexReg. Dadurch
// benoetigen die Text-Attribute nur xub_StrLen statt SwIndizies!
void SwTxtNode::Update( const SwIndex & aPos, xub_StrLen nLen,
                        BOOL bNegativ, BOOL bDelete )
{
    SetAutoCompleteWordDirty( TRUE );

    TmpHints* pCollector = NULL;
    if ( HasHints() )
    {
        xub_StrLen nPos = aPos.GetIndex();
        xub_StrLen* pIdx;
        SwTxtAttr* pHt;
        if( bNegativ )
        {
            xub_StrLen nMax = nPos + nLen;
            for ( USHORT n = 0; n < m_pSwpHints->Count(); ++n )
            {
                BOOL bSttBefore = FALSE;
                pHt = m_pSwpHints->GetTextHint(n);
                pIdx = pHt->GetStart();
                if( *pIdx >= nPos )
                {
                    if( *pIdx > nMax )
                         *pIdx = *pIdx - nLen;
                    else
                    {
                         *pIdx = nPos;
                    }
                }
                else
                    bSttBefore = TRUE;

                if( 0 == (pIdx = pHt->GetEnd()) )
                    continue;

                if( *pIdx >= nPos )
                {
                    if( *pIdx > nMax )
                    {
                        *pIdx = *pIdx - nLen;
                    }
                    else if( *pIdx != nPos )
                    {
                        *pIdx = nPos;
                    }
                }
            }

            m_pSwpHints->MergePortions( *this );
        }
        else
        {
            xub_StrLen* pEnd;
            BOOL bNoExp = FALSE;
            BOOL bResort = FALSE;
            const USHORT coArrSz = static_cast<USHORT>(RES_TXTATR_WITHEND_END) -
                                   static_cast<USHORT>(RES_CHRATR_BEGIN) +
                                   static_cast<USHORT>(RES_UNKNOWNATR_END) -
                                   static_cast<USHORT>(RES_UNKNOWNATR_BEGIN);

            BOOL aDontExp[ coArrSz ];
            memset( &aDontExp, 0, coArrSz * sizeof(BOOL) );

            for ( USHORT n = 0; n < m_pSwpHints->Count(); ++n )
            {
                pHt = m_pSwpHints->GetTextHint(n);
                pIdx = pHt->GetStart();
                pEnd = pHt->GetEnd();
                if ( *pIdx >= nPos )
                {
                    *pIdx = *pIdx + nLen;
                    if ( pEnd )
                    {
                        *pEnd = *pEnd + nLen;
                    }
                }
                else if ( pEnd && *pEnd >= nPos )
                {
                    if( *pEnd > nPos || IsIgnoreDontExpand() )
                    {
                        *pEnd = *pEnd + nLen;
                    }
                    else
                    {
                        USHORT nWhPos, nWhich = pHt->Which();

                        if (isCHRATR(nWhich) || isTXTATR_WITHEND(nWhich))
                        {
                            nWhPos = static_cast<USHORT>(nWhich -
                                        RES_CHRATR_BEGIN);
                        }
                        else if (isUNKNOWNATR(nWhich))
                        {
                            nWhPos = static_cast<USHORT>(
                                nWhich -
                                static_cast<USHORT>(RES_UNKNOWNATR_BEGIN) +
                                static_cast<USHORT>(RES_TXTATR_WITHEND_END) -
                                static_cast<USHORT>(RES_CHRATR_BEGIN) );
                        }
                        else
                            continue;

                        if( aDontExp[ nWhPos ] )
                            continue;

                        if( pHt->DontExpand() )
                        {
                            pHt->SetDontExpand( FALSE );
                            bResort = TRUE;
                            if( pHt->IsCharFmtAttr() )
                            {
                                bNoExp = TRUE;
                                aDontExp[ static_cast<USHORT>(RES_TXTATR_CHARFMT) - static_cast<USHORT>(RES_CHRATR_BEGIN) ]
                                    = TRUE;
                                aDontExp[ static_cast<USHORT>(RES_TXTATR_INETFMT) - static_cast<USHORT>(RES_CHRATR_BEGIN) ]
                                    = TRUE;
                            }
                            else
                                aDontExp[ nWhPos ] = TRUE;
                        }
                        else if( bNoExp )
                        {
                             if( !pCollector )
                                pCollector = new TmpHints;
                             USHORT nCollCnt = pCollector->Count();
                             for( USHORT i = 0; i < nCollCnt; ++i )
                             {
                                SwTxtAttr *pTmp = (*pCollector)[ i ];
                                if( nWhich == pTmp->Which() )
                                {
                                    pCollector->Remove( i );
                                    delete pTmp;
                                    break;
                                }
                             }
                             SwTxtAttr *pTmp = MakeTxtAttr( pHt->GetAttr(),
                                                nPos, nPos + nLen );
                             pCollector->C40_INSERT( SwTxtAttr, pTmp, pCollector->Count() );
                        }
                        else
                        {
                            *pEnd = *pEnd + nLen;
                        }
                    }
                }
            }
            if( bResort )
            {
                m_pSwpHints->Resort();
            }
        }
    }

    SwIndexReg aTmpIdxReg;
    if( !bNegativ && !bDelete )
    {
        SwIndex* pIdx;
        const SwRedlineTbl& rTbl = GetDoc()->GetRedlineTbl();
        if( rTbl.Count() )
            for( USHORT i = 0; i < rTbl.Count(); ++i )
            {
                SwRedline* pRedl = rTbl[ i ];
                if( pRedl->HasMark() )
                {
                    SwPosition* pEnd = pRedl->End();

                    if( this == &pEnd->nNode.GetNode() &&
                        *pRedl->GetPoint() != *pRedl->GetMark() &&
                        aPos.GetIndex() ==
                            (pIdx = &pEnd->nContent)->GetIndex() )
                        pIdx->Assign( &aTmpIdxReg, pIdx->GetIndex() );
                }
                else if( this == &pRedl->GetPoint()->nNode.GetNode() &&
                        aPos.GetIndex() == (pIdx = &pRedl->GetPoint()->
                            nContent)->GetIndex() )
                {
                    pIdx->Assign( &aTmpIdxReg, pIdx->GetIndex() );
                    if( &pRedl->GetBound( TRUE ) == pRedl->GetPoint() )
                    {
                        pRedl->GetBound( FALSE ) = pRedl->GetBound( TRUE );
                        pIdx = &pRedl->GetBound( FALSE ).nContent;
                    }
                    else
                    {
                        pRedl->GetBound( TRUE ) = pRedl->GetBound( FALSE );
                        pIdx = &pRedl->GetBound( TRUE ).nContent;
                    }
                    pIdx->Assign( &aTmpIdxReg, pIdx->GetIndex() );
                }
            }

        const IDocumentMarkAccess* const pMarkAccess = getIDocumentMarkAccess();
        for(IDocumentMarkAccess::const_iterator_t ppMark = pMarkAccess->getMarksBegin();
            ppMark != pMarkAccess->getMarksEnd();
            ppMark++)
        {
            // Bookmarks must never grow to either side, when
            // editing (directly) to the left or right (#i29942#)!
            // And a bookmark with same start and end must remain
            // to the left of the inserted text (used in XML import).
            const ::sw::mark::IMark* const pMark = ppMark->get();
            const SwPosition* pEnd = &pMark->GetMarkEnd();
            pIdx = (SwIndex*)&pEnd->nContent;
            if( this == &pEnd->nNode.GetNode() &&
                aPos.GetIndex() == pIdx->GetIndex() )
                pIdx->Assign( &aTmpIdxReg, pIdx->GetIndex() );
        }
    }
    SwIndexReg::Update( aPos, nLen, bNegativ );
    if( pCollector )
    {
        USHORT nCount = pCollector->Count();
        for( USHORT i = 0; i < nCount; ++i )
        {
            m_pSwpHints->Insert( (*pCollector)[ i ], *this, FALSE );
        }
        delete pCollector;
    }

    aTmpIdxReg.MoveTo( *this );
}

void SwTxtNode::_ChgTxtCollUpdateNum( const SwTxtFmtColl *pOldColl,
                                        const SwTxtFmtColl *pNewColl)
{
    SwDoc* pDoc = GetDoc();
    ASSERT( pDoc, "Kein Doc?" );
    // erfrage die OutlineLevel und update gegebenenfalls das Nodes-Array,
    // falls sich die Level geaendert haben !
    //const BYTE nOldLevel = pOldColl ? pOldColl->GetOutlineLevel():NO_NUMBERING;//#outline level,removed by zhaojianwei
    //const BYTE nNewLevel = pNewColl ? pNewColl->GetOutlineLevel():NO_NUMBERING;//<-end,zhaojianwei
    const int nOldLevel = pOldColl && pOldColl->IsAssignedToListLevelOfOutlineStyle() ?
                     pOldColl->GetAssignedOutlineStyleLevel() : MAXLEVEL;
    const int nNewLevel = pNewColl && pNewColl->IsAssignedToListLevelOfOutlineStyle() ?
                     pNewColl->GetAssignedOutlineStyleLevel() : MAXLEVEL;

//	if ( NO_NUMBERING != nNewLevel )	//#outline level,zhaojianwei
    if ( MAXLEVEL != nNewLevel )	//<-end,zhaojianwei
    {
        SetAttrListLevel(nNewLevel);
    }

    {
        if (pDoc)
            pDoc->GetNodes().UpdateOutlineNode(*this);
    }


    SwNodes& rNds = GetNodes();
    // Update beim Level 0 noch die Fussnoten !!
    if( ( !nNewLevel || !nOldLevel) && pDoc->GetFtnIdxs().Count() &&
        FTNNUM_CHAPTER == pDoc->GetFtnInfo().eNum &&
        rNds.IsDocNodes() )
    {
        SwNodeIndex aTmpIndex( rNds, GetIndex());

        pDoc->GetFtnIdxs().UpdateFtn( aTmpIndex);
    }

//FEATURE::CONDCOLL
    if( /*pOldColl != pNewColl && pNewColl && */
        RES_CONDTXTFMTCOLL == pNewColl->Which() )
    {
        // Erfrage die akt. Condition des TextNodes:
        ChkCondColl();
    }
//FEATURE::CONDCOLL
}

// Wenn man sich genau am Ende einer Text- bzw. INetvorlage befindet,
// bekommt diese das DontExpand-Flag verpasst

BOOL SwTxtNode::DontExpandFmt( const SwIndex& rIdx, bool bFlag,
                                BOOL bFmtToTxtAttributes )
{
    const xub_StrLen nIdx = rIdx.GetIndex();
    if ( bFmtToTxtAttributes && nIdx == m_Text.Len() )
    {
        FmtToTxtAttr( this );
    }

    BOOL bRet = FALSE;
    if ( HasHints() )
    {
        const USHORT nEndCnt = m_pSwpHints->GetEndCount();
        USHORT nPos = nEndCnt;
        while( nPos )
        {
            SwTxtAttr *pTmp = m_pSwpHints->GetEnd( --nPos );
            xub_StrLen *pEnd = pTmp->GetEnd();
            if( !pEnd || *pEnd > nIdx )
                continue;
            if( nIdx != *pEnd )
                nPos = 0;
            else if( bFlag != pTmp->DontExpand() && !pTmp->IsLockExpandFlag()
                     && *pEnd > *pTmp->GetStart())
            {
                bRet = TRUE;
                m_pSwpHints->NoteInHistory( pTmp );
                pTmp->SetDontExpand( bFlag );
            }
        }
    }
    return bRet;
}


// gebe das vorgegebene Attribut, welches an der TextPosition (rIdx)
// gesetzt ist, zurueck. Gibt es keines, returne 0-Pointer.
// (gesetzt heisst, je nach bExpand ?
// 									  Start < rIdx <= End
//									: Start <= rIdx < End )

SwTxtAttr* SwTxtNode::GetTxtAttr( const SwIndex& rIdx, USHORT nWhichHt,
                                  BOOL bExpand ) const
{
    const SwTxtAttr* pRet = 0;
    const SwTxtAttr* pHt = 0;
    const xub_StrLen *pEndIdx = 0;
    const xub_StrLen nIdx = rIdx.GetIndex();
    const USHORT  nSize = m_pSwpHints ? m_pSwpHints->Count() : 0;

    for( USHORT i = 0; i < nSize; ++i )
    {
        // ist der Attribut-Anfang schon groesser als der Idx ?
        pHt = (*m_pSwpHints)[i];
        if ( nIdx < *(pHt->GetStart()) )
            break;			// beenden, kein gueltiges Attribut

        // ist es das gewuenschte Attribut ?
        if( pHt->Which() != nWhichHt )
            continue;		// nein, weiter

        pEndIdx = pHt->GetEnd();
        // liegt innerhalb des Bereiches ??
        if( !pEndIdx )
        {
            if( *pHt->GetStart() == nIdx )
            {
                pRet = pHt;
                break;
            }
        }
        else if( *pHt->GetStart() <= nIdx && nIdx <= *pEndIdx )
        {
            // Wenn bExpand gesetzt ist, wird das Verhalten bei Eingabe
            // simuliert, d.h. der Start wuede verschoben, das Ende expandiert,
            if( bExpand )
            {
                if( *pHt->GetStart() < nIdx )
                    pRet = pHt;
            }
            else
            {
                if( nIdx < *pEndIdx )
                    pRet = pHt;		// den am dichtesten liegenden
            }
        }
    }
    return (SwTxtAttr*)pRet;		// kein gueltiges Attribut gefunden !!
}

/*************************************************************************
 *							CopyHint()
 *************************************************************************/

SwCharFmt* lcl_FindCharFmt( const SwCharFmts* pCharFmts, const XubString& rName )
{
    if( rName.Len() )
    {
        SwCharFmt* pFmt;
        USHORT nArrLen = pCharFmts->Count();
        for( USHORT i = 1; i < nArrLen; i++ )
        {
            pFmt = (*pCharFmts)[ i ];
            if( pFmt->GetName().CompareTo( rName ) == COMPARE_EQUAL )
                return pFmt;
        }
    }
    return NULL;
}

void lcl_CopyHint( const USHORT nWhich, const SwTxtAttr *pHt,
                    SwTxtAttr *pNewHt, SwDoc* pOtherDoc, SwTxtNode *pDest )
{
    ASSERT( nWhich == pHt->Which(), "Falsche Hint-Id" );
    switch( nWhich )
    {
        // Wenn wir es mit einem Fussnoten-Attribut zu tun haben,
        // muessen wir natuerlich auch den Fussnotenbereich kopieren.
        case RES_TXTATR_FTN :
            ((SwTxtFtn*)pHt)->CopyFtn( (SwTxtFtn*)pNewHt );
            break;

        // Beim Kopieren von Feldern in andere Dokumente
        // muessen die Felder bei ihren neuen Feldtypen angemeldet werden.

        // TabellenFormel muessen relativ kopiert werden.
        case RES_TXTATR_FIELD :
            {
                const SwFmtFld& rFld = pHt->GetFld();
                if( pOtherDoc )
                    ((SwTxtFld*)pHt)->CopyFld( (SwTxtFld*)pNewHt );

                // Tabellenformel ??
                if( RES_TABLEFLD == rFld.GetFld()->GetTyp()->Which()
                    && ((SwTblField*)rFld.GetFld())->IsIntrnlName() )
                {
                    // wandel die interne in eine externe Formel um
                    const SwTableNode* pDstTblNd = ((SwTxtFld*)pHt)->
                                            GetTxtNode().FindTableNode();
                    if( pDstTblNd )
                    {
                        SwTblField* pTblFld = (SwTblField*)
                                                pNewHt->GetFld().GetFld();
                        pTblFld->PtrToBoxNm( &pDstTblNd->GetTable() );
                    }
                }
            }
            break;

        case RES_TXTATR_TOXMARK :
            if( pOtherDoc && pDest && pDest->GetpSwpHints()
                && USHRT_MAX != pDest->GetpSwpHints()->GetPos( pNewHt ) )
                // Beim Kopieren von TOXMarks(Client) in andere Dokumente
                // muss der Verzeichnis (Modify) ausgetauscht werden
                ((SwTxtTOXMark*)pNewHt)->CopyTOXMark( pOtherDoc );
            break;

        case RES_TXTATR_CHARFMT :
            // Wenn wir es mit einer Zeichenvorlage zu tun haben,
            // muessen wir natuerlich auch die Formate kopieren.
            if( pDest && pDest->GetpSwpHints()
                && USHRT_MAX != pDest->GetpSwpHints()->GetPos( pNewHt ) )
            {
                SwCharFmt* pFmt = (SwCharFmt*)pHt->GetCharFmt().GetCharFmt();

                if( pFmt && pOtherDoc )
                    pFmt = pOtherDoc->CopyCharFmt( *pFmt );
                ((SwFmtCharFmt&)pNewHt->GetCharFmt()).SetCharFmt( pFmt );
            }
            break;
        case RES_TXTATR_INETFMT :
            // Wenn wir es mit benutzerdefinierten INet-Zeichenvorlagen
            // zu tun haben, muessen wir natuerlich auch die Formate kopieren.
            if( pOtherDoc && pDest && pDest->GetpSwpHints()
                && USHRT_MAX != pDest->GetpSwpHints()->GetPos( pNewHt ) )
            {
                const SwDoc* pDoc;
                if( 0!=( pDoc = ((SwTxtINetFmt*)pHt)->GetTxtNode().GetDoc() ) )
                {
                    const SwCharFmts* pCharFmts = pDoc->GetCharFmts();
                    const SwFmtINetFmt& rFmt = pHt->GetINetFmt();
                    SwCharFmt* pFmt;
                    pFmt = lcl_FindCharFmt( pCharFmts, rFmt.GetINetFmt() );
                    if( pFmt )
                        pOtherDoc->CopyCharFmt( *pFmt );
                    pFmt = lcl_FindCharFmt( pCharFmts, rFmt.GetVisitedFmt() );
                    if( pFmt )
                        pOtherDoc->CopyCharFmt( *pFmt );
                }
            }
            //JP 24.04.98: Bug 49753 - ein TextNode muss am Attribut
            //				gesetzt sein, damit die Vorlagen erzeugt
            //				werden koenne
            if( !((SwTxtINetFmt*)pNewHt)->GetpTxtNode() )
                ((SwTxtINetFmt*)pNewHt)->ChgTxtNode( pDest );

            //JP 22.10.97: Bug 44875 - Verbindung zum Format herstellen
            ((SwTxtINetFmt*)pNewHt)->GetCharFmt();
            break;
    }
}

/*************************************************************************
|*	SwTxtNode::CopyAttr()
|*	Beschreibung	kopiert Attribute an der Position nStart in pDest.
|*	BP 7.6.93:		Es werden mit Absicht nur die Attribute _mit_ EndIdx
|*					kopiert! CopyAttr wird vornehmlich dann gerufen,
|*					wenn Attribute fuer einen Node mit leerem String
|*					gesetzt werden sollen.
*************************************************************************/

void SwTxtNode::CopyAttr( SwTxtNode *pDest, const xub_StrLen nTxtStartIdx,
                          const xub_StrLen nOldPos )
{
    if ( HasHints() )    // keine Attribute, keine Kekse
    {
        const xub_StrLen *pEndIdx = 0;
        const SwTxtAttr *pHt = 0;
        SwTxtAttr *pNewHt = 0;
        xub_StrLen nAttrStartIdx = 0;
        USHORT nWhich;

        SwDoc* pOtherDoc = pDest->GetDoc();
        if( pOtherDoc == GetDoc() )
            pOtherDoc = 0;

        for ( USHORT i = 0; i < m_pSwpHints->Count(); i++ )
        {
            pHt = (*m_pSwpHints)[i];
            if( nTxtStartIdx < ( nAttrStartIdx = *pHt->GetStart() ) )
                break;		// ueber das Textende, da nLen == 0

            pEndIdx = pHt->GetEnd();
            if( pEndIdx )
            {
                if( ( *pEndIdx > nTxtStartIdx ||
                      ( *pEndIdx == nTxtStartIdx &&
                        nAttrStartIdx == nTxtStartIdx ) ) )
                {
                    if( RES_TXTATR_REFMARK != ( nWhich = pHt->Which()) )
                    {
                        // Attribut liegt im Bereich, also kopieren
                        if( 0 != ( pNewHt = pDest->InsertItem( pHt->GetAttr(),
                                                nOldPos, nOldPos ) ) )
                            lcl_CopyHint( nWhich, pHt, pNewHt, pOtherDoc, pDest );
                    }
                    else if( !pOtherDoc ? GetDoc()->IsCopyIsMove()
                                        : 0 == pOtherDoc->GetRefMark(
                                        pHt->GetRefMark().GetRefName() ) )
                        pDest->InsertItem( pHt->GetAttr(), nOldPos, nOldPos );
                }
            }
        }
    }

    if( this != pDest )
    {
        // Frames benachrichtigen, sonst verschwinden die Ftn-Nummern
        SwUpdateAttr aHint( nOldPos, nOldPos, 0 );
        pDest->Modify( 0, &aHint );
    }
}

/*************************************************************************
|*	SwTxtNode::Copy()
|*	Beschreibung		kopiert Zeichen und Attibute in pDest,
|*						wird angehaengt
*************************************************************************/

// --> OD 2008-11-18 #i96213#
// introduction of new optional parameter to control, if all attributes have to be copied.
void SwTxtNode::Copy( SwTxtNode *pDest,
                      const SwIndex &rStart,
                      xub_StrLen nLen,
                      const bool bForceCopyOfAllAttrs )
{
    SwIndex aIdx( pDest, pDest->m_Text.Len() );
    Copy( pDest, aIdx, rStart, nLen, bForceCopyOfAllAttrs );
}
// <--

// --> OD 2008-11-18 #i96213#
// introduction of new optional parameter to control, if all attributes have to be copied.
void SwTxtNode::Copy( SwTxtNode *pDest,
                      const SwIndex &rDestStart,
                      const SwIndex &rStart,
                      xub_StrLen nLen,
                      const bool bForceCopyOfAllAttrs )
// <--
{
    xub_StrLen nTxtStartIdx = rStart.GetIndex();
    xub_StrLen nDestStart = rDestStart.GetIndex();		// alte Pos merken

    if( !nLen )
    {
        // wurde keine Laenge angegeben, dann Kopiere die Attribute
        // an der Position rStart.
        CopyAttr( pDest, nTxtStartIdx, nDestStart );

        // harte Absatz umspannende Attribute kopieren
        if( HasSwAttrSet() )
        {
            // alle, oder nur die CharAttribute ?
            // --> OD 2008-11-18 #i96213#
            if ( !bForceCopyOfAllAttrs &&
                 ( nDestStart ||
                   pDest->HasSwAttrSet() ||
                   nLen != pDest->GetTxt().Len() ) )
            // <--
            {
                SfxItemSet aCharSet( pDest->GetDoc()->GetAttrPool(),
                                    RES_CHRATR_BEGIN, RES_CHRATR_END-1,
                                    RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT,
                                    RES_TXTATR_INETFMT, RES_TXTATR_INETFMT,
                                    RES_UNKNOWNATR_BEGIN, RES_UNKNOWNATR_END-1,
                                    0 );
                aCharSet.Put( *GetpSwAttrSet() );
                if( aCharSet.Count() )
                    pDest->SetAttr( aCharSet, nDestStart, nDestStart );
            }
            else
                GetpSwAttrSet()->CopyToModify( *pDest );
        }
        return;
    }

    // 1. Text kopieren
    xub_StrLen i = pDest->m_Text.Len() - nDestStart;
    //JP 15.02.96: Bug 25537 - Attributbehandlung am Ende fehlt! Darum
    //				ueber die InsertMethode den Text einfuegen und nicht
    //				selbst direkt
    pDest->Insert( m_Text.Copy( nTxtStartIdx, nLen ), rDestStart,
                   INS_EMPTYEXPAND );

    // um reale Groesse Updaten !
    nLen = pDest->m_Text.Len() - nDestStart - i;
    if ( !nLen ) // string not longer?
        return;

    i = 0;
    const xub_StrLen *pEndIdx = 0;
    xub_StrLen nAttrStartIdx = 0;
    const SwTxtAttr *pHt = 0;
    SwTxtAttr *pNewHt = 0;

    SwDoc* pOtherDoc = pDest->GetDoc();
    if( pOtherDoc == GetDoc() )
        pOtherDoc = 0;

    // harte Absatz umspannende Attribute kopieren
    if( HasSwAttrSet() )
    {
        // alle, oder nur die CharAttribute ?
        // --> OD 2008-11-18 #i96213#
        if ( !bForceCopyOfAllAttrs &&
             ( nDestStart ||
               pDest->HasSwAttrSet() ||
               nLen != pDest->GetTxt().Len() ) )
        // <--
        {
            SfxItemSet aCharSet( pDest->GetDoc()->GetAttrPool(),
                                RES_CHRATR_BEGIN, RES_CHRATR_END-1,
                                RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT,
                                RES_TXTATR_INETFMT, RES_TXTATR_INETFMT,
                                RES_UNKNOWNATR_BEGIN, RES_UNKNOWNATR_END-1,
                                0 );
            aCharSet.Put( *GetpSwAttrSet() );
            if( aCharSet.Count() )
                pDest->SetAttr( aCharSet, nDestStart, nDestStart + nLen );
        }
        else
            GetpSwAttrSet()->CopyToModify( *pDest );
    }


    const BOOL bUndoNodes = !pOtherDoc && GetDoc()->GetUndoNds() == &GetNodes();

    // Ende erst jetzt holen, weil beim Kopieren in sich selbst der
    // Start-Index und alle Attribute vorher aktualisiert werden.
    nTxtStartIdx = rStart.GetIndex();
    xub_StrLen nEnd = nTxtStartIdx + nLen;

    // 2. Attribute kopieren
    // durch das Attribute-Array, bis der Anfang des Geltungsbereiches
    // des Attributs hinter dem zu kopierenden Bereich liegt
    USHORT nWhich;
    USHORT nSize = m_pSwpHints ? m_pSwpHints->Count() : 0;
    xub_StrLen nAttrStt, nAttrEnd;

    // wird in sich selbst kopiert, dann kann beim Einfuegen ein
    // Attribut geloescht werden. Darum erst ins Tmp-Array kopieren und
    // dann erst ins eigene uebertragen.
    SwpHts aArr( 5 );

    // Del-Array fuer alle RefMarks ohne Ausdehnung
    SwpHts aRefMrkArr;

        //Achtung: kann ungueltig sein!!
    while ( ( i < nSize ) &&
            ((nAttrStartIdx = *(*m_pSwpHints)[i]->GetStart()) < nEnd) )
    {
        pHt = (*m_pSwpHints)[i];
        pNewHt = 0;
        pEndIdx = pHt->GetEnd();
        nWhich = pHt->Which();

        // JP 26.04.94: REFMARK's werden nie kopiert. Hat das Refmark aber
        //				keinen Bereich umspannt, so steht im Text ein 255
        //				dieses muss entfernt werden. Trick: erst kopieren,
        //				erkennen und sammeln, nach dem kopieren Loeschen.
        //				Nimmt sein Zeichen mit ins Grab !!
        // JP 14.08.95:	Duerfen RefMarks gemovt werden?
        int bCopyRefMark = RES_TXTATR_REFMARK == nWhich && ( bUndoNodes ||
                           (!pOtherDoc ? GetDoc()->IsCopyIsMove()
                                      : 0 == pOtherDoc->GetRefMark(
                                        pHt->GetRefMark().GetRefName() )));

        if( pEndIdx && RES_TXTATR_REFMARK == nWhich && !bCopyRefMark )
        {
            ++i;
            continue;
        }

        if( nAttrStartIdx < nTxtStartIdx )
        {
            // Anfang liegt vor dem Bereich
            if( pEndIdx && ( nAttrEnd = *pEndIdx ) > nTxtStartIdx )
            {
                // Attribut mit einem Bereich
                // und das Ende des Attribut liegt im Bereich
                nAttrStt = nDestStart;
                nAttrEnd = nAttrEnd > nEnd
                            ? rDestStart.GetIndex()
                            : nDestStart + nAttrEnd - nTxtStartIdx;
            }
            else
            {
                ++i;
                continue;
            }
        }
        else
        {
            // der Anfang liegt innerhalb des Bereiches
            nAttrStt = nDestStart + ( nAttrStartIdx - nTxtStartIdx );
            if( pEndIdx )
                nAttrEnd = *pEndIdx > nEnd
                            ? rDestStart.GetIndex()
                            : nDestStart + ( *pEndIdx - nTxtStartIdx );
            else
                nAttrEnd = nAttrStt;
        }

        if( pDest == this )
        {
            // die Daten kopieren
            pNewHt = MakeTxtAttr( pHt->GetAttr(), nAttrStt, nAttrEnd );

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//JP 23.04.95:	erstmal so gesondert hier behandeln. Am Besten ist es
//				aber im CopyFtn wenn die pDestFtn keinen StartNode hat,
//				sich diesen dann anlegt.
//				Aber so kurz vor der BETA besser nicht anfassen.
            if( RES_TXTATR_FTN == nWhich )
            {
                SwTxtFtn* pFtn = (SwTxtFtn*)pNewHt;
                pFtn->ChgTxtNode( this );
                pFtn->MakeNewTextSection( GetNodes() );
                lcl_CopyHint( nWhich, pHt, pFtn, 0, 0 );
                pFtn->ChgTxtNode( 0 );
            }
            else
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            {
                lcl_CopyHint( nWhich, pHt, pNewHt, 0, pDest );
            }
            aArr.C40_INSERT( SwTxtAttr, pNewHt, aArr.Count() );
        }
        else
        {
            pNewHt = pDest->InsertItem( pHt->GetAttr(), nAttrStt,
                                    nAttrEnd, nsSetAttrMode::SETATTR_NOTXTATRCHR );
            if( pNewHt )
                lcl_CopyHint( nWhich, pHt, pNewHt, pOtherDoc, pDest );
            else if( !pEndIdx )
            {
                // Attribut wurde nicht kopiert, hat seinen Inhalt mitgenommen!
                // Damit der rest aber korrekt kopiert werden kann, muss das
                // Zeichen wieder an der Position stehen. Darum hier ein
                // "Dummy-TextAttribut" einfuegen, wird am Ende wieder entfernt!
                pNewHt = pDest->InsertItem( SwFmtHardBlank( 0xB7 ), nAttrStt, 0
                                        /*???, INS_NOHINTEXPAND*/ );
                aRefMrkArr.C40_INSERT( SwTxtAttr, pNewHt, aRefMrkArr.Count() );
            }
        }

        if( RES_TXTATR_REFMARK == nWhich && !pEndIdx && !bCopyRefMark )
        {
            aRefMrkArr.C40_INSERT( SwTxtAttr, pNewHt, aRefMrkArr.Count() );
        }

        ++i;
    }

    // nur falls im Array Attribute stehen (kann nur beim Kopieren
    // sich selbst passieren!!)
    for( i = 0; i < aArr.Count(); ++i )
        Insert( aArr[ i ], nsSetAttrMode::SETATTR_NOTXTATRCHR );

    if( pDest->GetpSwpHints() )
        for( i = 0; i < aRefMrkArr.Count(); ++i )
        {
            pNewHt = aRefMrkArr[i];
            if( pNewHt->GetEnd() )
            {
                pDest->GetpSwpHints()->Delete( pNewHt );
                pDest->DestroyAttr( pNewHt );
            }
            else
            {
                const SwIndex aIdx( pDest, *pNewHt->GetStart() );
                pDest->Erase( aIdx, 1 );
            }
        }

    CHECK_SWPHINTS(this);
}


/*
 * Rudimentaeres Editieren, damit die SwDoc-Insert-Methoden
 * funktionieren.
 */

SwTxtNode& SwTxtNode::Insert( const XubString	&rStr,
                              const SwIndex &rIdx, const USHORT nMode )
{
    ASSERT( rIdx <= m_Text.Len(), "SwTxtNode::Insert: invalid index." );
    ASSERT( (ULONG)m_Text.Len() + (ULONG)rStr.Len() <= STRING_LEN,
            "SwTxtNode::Insert: node text with insertion > STRING_LEN." );

    xub_StrLen aPos = rIdx.GetIndex();
    xub_StrLen nLen = m_Text.Len() - aPos;
    m_Text.Insert( rStr, aPos );
    nLen = m_Text.Len() - aPos - nLen;
    if( !nLen )
        return *this;
    Update( rIdx, nLen );		// um reale Groesse Updaten !

    // analog zu Insert(char) in txtedt.cxx:
    // 1) bei bHintExp leere Hints an rIdx.GetIndex suchen und aufspannen
    // 2) bei bHintExp == FALSE mitgezogene Feldattribute zuruecksetzen

    USHORT i;

    if ( HasHints() )
    {
       for ( i = 0; i < m_pSwpHints->Count() &&
                rIdx >= *(*m_pSwpHints)[i]->GetStart(); ++i )
        {
            SwTxtAttr *pHt = m_pSwpHints->GetTextHint( i );
            xub_StrLen* pEndIdx = pHt->GetEnd();
            if( !pEndIdx )
                continue;

            if( rIdx == *pEndIdx )
            {
                if( nMode & INS_NOHINTEXPAND || pHt->DontExpand() )
                {
                    // bei leeren Attributen auch Start veraendern
                    if( rIdx == *pHt->GetStart() )
                        *pHt->GetStart() = *pHt->GetStart() - nLen;
                    *pEndIdx = *pEndIdx - nLen;
                    m_pSwpHints->DeleteAtPos(i);
                    Insert( pHt, nsSetAttrMode::SETATTR_NOHINTADJUST );
                }
                // leere Hints an rIdx.GetIndex ?
                else if( nMode & INS_EMPTYEXPAND &&
                        *pEndIdx == *pHt->GetStart() )
                {
                    *pHt->GetStart() = *pHt->GetStart() - nLen;
                    const USHORT nAktLen = m_pSwpHints->Count();
                    m_pSwpHints->DeleteAtPos(i);
                    Insert( pHt/* AUTOSTYLES:, nsSetAttrMode::SETATTR_NOHINTADJUST*/ );
                    if ( nAktLen > m_pSwpHints->Count() && i )
                    {
                        --i;
                    }
                    continue;
                }
                else
                {
                    continue;
                }
            }
            if ( !(nMode & INS_NOHINTEXPAND) &&
                 rIdx == nLen && *pHt->GetStart() == rIdx.GetIndex() &&
                 !pHt->IsDontExpandStartAttr() )
            {
                // Kein Feld, am Absatzanfang, HintExpand
                m_pSwpHints->DeleteAtPos(i);
                *pHt->GetStart() = *pHt->GetStart() - nLen;
                Insert( pHt, nsSetAttrMode::SETATTR_NOHINTADJUST );
            }
        }
        TryDeleteSwpHints();
    }

    if ( GetDepends() )
    {
        SwInsTxt aHint( aPos, nLen );
        SwModify::Modify( 0, &aHint );
    }

    // By inserting a character, the hidden flags
    // at the TxtNode can become invalid:
    SetCalcHiddenCharFlags();

    CHECK_SWPHINTS(this);
    return *this;
}

/*************************************************************************
|*
|*	SwTxtNode::Cut()
|*
|*	Beschreibung		text.doc
|*	Ersterstellung		VB 20.03.91
|*	Letzte Aenderung	JP 11.08.94
|*
*************************************************************************/

void SwTxtNode::Cut( SwTxtNode *pDest, const SwIndex &rStart, xub_StrLen nLen )
{
    if(pDest)
    {
        SwIndex aDestStt( pDest, pDest->GetTxt().Len() );
        _Cut( pDest, aDestStt, rStart, nLen, FALSE );
    }
    else
        Erase( rStart, nLen );
}


void SwTxtNode::_Cut( SwTxtNode *pDest, const SwIndex& rDestStart,
                     const SwIndex &rStart, xub_StrLen nLen, BOOL bUpdate )
{
    if(!pDest)
    {
        Erase( rStart, nLen );
        return;
    }

    // nicht im Dokument verschieben ?
    if( GetDoc() != pDest->GetDoc() )
    {
        Copy( pDest, rDestStart, rStart, nLen);
        Erase(rStart,nLen);
        return;
    }

    if( !nLen )
    {
        // wurde keine Laenge angegeben, dann Kopiere die Attribute
        // an der Position rStart.
        CopyAttr( pDest, rStart.GetIndex(), rDestStart.GetIndex() );
        return;
    }

    xub_StrLen nTxtStartIdx = rStart.GetIndex();
    xub_StrLen nDestStart = rDestStart.GetIndex();		// alte Pos merken
    xub_StrLen nInitSize = pDest->m_Text.Len();

    xub_StrLen *pEndIdx = 0;
    xub_StrLen nAttrStartIdx = 0;
    SwTxtAttr *pHt = 0;
    SwTxtAttr *pNewHt = 0;

    // wird in sich selbst verschoben, muss es gesondert behandelt werden !!
    if( pDest == this )
    {
        m_Text.Insert( m_Text, nTxtStartIdx, nLen, nDestStart );
        m_Text.Erase( nTxtStartIdx + (nDestStart<nTxtStartIdx ? nLen : 0), nLen );

        xub_StrLen nEnd = rStart.GetIndex() + nLen;
        USHORT n;

        // dann suche mal alle Attribute zusammen, die im verschobenen
        // Bereich liegen. Diese werden in das extra Array verschoben,
        // damit sich die Indizies beim Updaten nicht veraendern !!!
        SwIndexReg aTmpRegArr;
        SwpHts aArr( 5 );

        // 2. Attribute verschieben
        // durch das Attribute-Array, bis der Anfang des Geltungsbereiches
        // des Attributs hinter dem zu verschiebenden Bereich liegt
        USHORT nAttrCnt = 0, nWhich;
        while ( m_pSwpHints && nAttrCnt < m_pSwpHints->Count() &&
                (nAttrStartIdx = *(pHt = m_pSwpHints->GetTextHint(nAttrCnt))->
                                    GetStart()) < nEnd )
        {
            pNewHt = 0;
            pEndIdx = pHt->GetEnd();

            if(nAttrStartIdx < nTxtStartIdx)
            {
                // Anfang liegt vor dem Bereich
                if( RES_TXTATR_REFMARK != ( nWhich = pHt->Which() ) &&
                    pEndIdx && *pEndIdx > nTxtStartIdx )
                {
                    // Attribut mit einem Bereich
                    // und das Ende des Attribut liegt im Bereich
                    pNewHt = MakeTxtAttr( pHt->GetAttr(), 0,
                                        *pEndIdx > nEnd
                                            ? nLen
                                            : *pEndIdx - nTxtStartIdx );
                }
            }
            else
            {
                // der Anfang liegt vollstaendig im Bereich
                if( !pEndIdx || *pEndIdx < nEnd )
                {
                    // Attribut verschieben
                    m_pSwpHints->Delete( pHt );
                    // die Start/End Indicies neu setzen
                    *pHt->GetStart() = nAttrStartIdx - nTxtStartIdx;
                    if( pEndIdx )
                        *pHt->GetEnd() = *pEndIdx - nTxtStartIdx;
                    aArr.C40_INSERT( SwTxtAttr, pHt, aArr.Count() );
                    continue;			// while-Schleife weiter, ohne ++ !
                }
                    // das Ende liegt dahinter
                else if( RES_TXTATR_REFMARK != ( nWhich = pHt->Which() ))
                {
                    pNewHt = MakeTxtAttr( pHt->GetAttr(),
                            nAttrStartIdx - nTxtStartIdx,
                            !pEndIdx ? 0
                                     : ( *pEndIdx > nEnd
                                            ? nLen
                                            : *pEndIdx - nTxtStartIdx ));
                }
            }
            if( pNewHt )
            {
                // die Daten kopieren
                lcl_CopyHint( nWhich, pHt, pNewHt, 0, this );
                aArr.C40_INSERT( SwTxtAttr, pNewHt, aArr.Count() );
            }
            ++nAttrCnt;
        }

        if( bUpdate )
            // Update aller Indizies
            Update( rDestStart, nLen, FALSE, TRUE );
#ifdef CUTNOEXPAND
        else
            // wird am Ende eingefuegt, nur die Attribut-Indizies verschieben
            if ( 0 < nLen && 0 < nInitSize && m_pSwpHints )
            {
                // check if there was the end of an attribute at the insertion
                // position: if it is not a field, it must be expanded
                for ( n = 0; n < m_pSwpHints->Count(); n++ )
                {
                    pHt = m_pSwpHints->GetTextHint(n);
                    pEndIdx = pHt->GetEnd();
                    if ( pEndIdx  && (*pEndIdx == nInitSize) )
                    {
                        *pEndIdx = *pEndIdx + nLen;
                    }
                }
            }
#endif
        CHECK_SWPHINTS(this);

        Update( rStart, nLen, TRUE, TRUE );

        CHECK_SWPHINTS(this);

        // dann setze die kopierten/geloeschten Attribute in den Node
        if( nDestStart <= nTxtStartIdx )
            nTxtStartIdx = nTxtStartIdx + nLen;
        else
            nDestStart = nDestStart - nLen;

        for( n = 0; n < aArr.Count(); ++n )
        {
            pNewHt = aArr[n];
            *pNewHt->GetStart() = nDestStart + *pNewHt->GetStart();
            if( 0 != ( pEndIdx = pNewHt->GetEnd() ))
                *pEndIdx = nDestStart + *pEndIdx;
            Insert( pNewHt, nsSetAttrMode::SETATTR_NOTXTATRCHR );
        }
    }
    else
    {
        xub_StrLen i = nInitSize - nDestStart;
        pDest->m_Text.Insert( m_Text, nTxtStartIdx, nLen, nDestStart );
        m_Text.Erase( nTxtStartIdx, nLen );
        nLen = pDest->m_Text.Len() - nDestStart - i; // update w/ current size!
        if( !nLen )					// String nicht gewachsen ??
            return;

        i = 0;

        if( bUpdate )
            // Update aller Indizies
            pDest->Update( rDestStart, nLen, FALSE, TRUE);
#ifdef CUTNOEXPAND
        else
            // wird am Ende eingefuegt, nur die Attribut-Indizies verschieben
            if ( 0 < nLen && 0 < nInitSize && pDest->m_pSwpHints )
            {
                // check if there was the end of an attribute at the insertion
                // position: if it is not a field, it must be expanded
                for ( USHORT n = 0; n < pDest->m_pSwpHints->Count(); n++ )
                {
                    pHt = pDest->m_pSwpHints->GetTextHint(n);
                    pEndIdx = pHt->GetEnd();
                    if ( pEndIdx  && (*pEndIdx == nInitSize) )
                    {
                        *pEndIdx = *pEndIdx + nLen;
                    }
                }
            }
#endif
        CHECK_SWPHINTS(pDest);

        USHORT nEnd = rStart.GetIndex() + nLen;
        SwDoc* pOtherDoc = pDest->GetDoc();
        if( pOtherDoc == GetDoc() )
            pOtherDoc = 0;
        const BOOL bUndoNodes = !pOtherDoc && GetDoc()->GetUndoNds() == &GetNodes();

        // harte Absatz umspannende Attribute kopieren
        if( HasSwAttrSet() )
        {
            // alle, oder nur die CharAttribute ?
            if( nInitSize || pDest->HasSwAttrSet() ||
                nLen != pDest->GetTxt().Len() )
            {
                SfxItemSet aCharSet( pDest->GetDoc()->GetAttrPool(),
                                    RES_CHRATR_BEGIN, RES_CHRATR_END-1,
                                    RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT,
                                    RES_TXTATR_INETFMT, RES_TXTATR_INETFMT,
                                    RES_UNKNOWNATR_BEGIN, RES_UNKNOWNATR_END-1,
                                    0 );
                aCharSet.Put( *GetpSwAttrSet() );
                if( aCharSet.Count() )
                    pDest->SetAttr( aCharSet, nDestStart, nDestStart + nLen );
            }
            else
                GetpSwAttrSet()->CopyToModify( *pDest );
        }

        // 2. Attribute verschieben
        // durch das Attribute-Array, bis der Anfang des Geltungsbereiches
        // des Attributs hinter dem zu verschiebenden Bereich liegt
        USHORT nAttrCnt = 0, nWhich;
        while ( m_pSwpHints && nAttrCnt < m_pSwpHints->Count() &&
                ( (nAttrStartIdx = *(pHt = m_pSwpHints->GetTextHint(nAttrCnt))->
                                    GetStart()) < nEnd ) )
        {
            pNewHt = 0;
            pEndIdx = pHt->GetEnd();

            if(nAttrStartIdx < nTxtStartIdx)
            {
                // Anfang liegt vor dem Bereich
                if( ( RES_TXTATR_REFMARK != ( nWhich = pHt->Which() )
                    || bUndoNodes ) && pEndIdx && *pEndIdx > nTxtStartIdx )
                {
                    // Attribut mit einem Bereich
                    // und das Ende des Attribut liegt im Bereich
                    pNewHt = pDest->MakeTxtAttr( pHt->GetAttr(), nDestStart,
                                        nDestStart + (
                                        *pEndIdx > nEnd
                                            ? nLen
                                            : *pEndIdx - nTxtStartIdx ) );
                }
            }
            else
            {
                // der Anfang liegt vollstaendig im Bereich
                nWhich = pHt->Which();
                if( !pEndIdx || *pEndIdx < nEnd ||
                    ( !pOtherDoc && !bUndoNodes && RES_TXTATR_REFMARK
                        == nWhich ) )
                {
                    // do not delete note and later add it -> sidebar flickering
                    if ( GetDoc()->GetDocShell() )
                        GetDoc()->GetDocShell()->Broadcast( SfxSimpleHint(SFX_HINT_USER04));
                    // Attribut verschieben
                    m_pSwpHints->Delete( pHt );
                    // die Start/End Indicies neu setzen
                    *pHt->GetStart() =
                            nDestStart + (nAttrStartIdx - nTxtStartIdx);
                    if( pEndIdx )
                        *pHt->GetEnd() = nDestStart + (
                                        *pEndIdx > nEnd
                                            ? nLen
                                            : *pEndIdx - nTxtStartIdx );
                    pDest->Insert( pHt, nsSetAttrMode::SETATTR_NOTXTATRCHR | nsSetAttrMode::SETATTR_DONTREPLACE );
                    if ( GetDoc()->GetDocShell() )
                        GetDoc()->GetDocShell()->Broadcast( SfxSimpleHint(SFX_HINT_USER04));
                    continue;			// while-Schleife weiter, ohne ++ !
                }
                    // das Ende liegt dahinter
                else if( RES_TXTATR_REFMARK != nWhich || bUndoNodes )
                {
                    pNewHt = MakeTxtAttr( pHt->GetAttr(),
                            nDestStart + (nAttrStartIdx - nTxtStartIdx),
                            !pEndIdx ? 0
                                     : nDestStart + ( *pEndIdx > nEnd
                                            ? nLen
                                            : *pEndIdx - nTxtStartIdx ));
                }
            }
            if ( pNewHt )
            {
                if( pDest->Insert( pNewHt, nsSetAttrMode::SETATTR_NOTXTATRCHR | nsSetAttrMode::SETATTR_DONTREPLACE ))
                    lcl_CopyHint( nWhich, pHt, pNewHt, pOtherDoc, pDest );
            }
            ++nAttrCnt;
        }
        // sollten jetzt noch leere Attribute rumstehen, dann haben diese
        // eine hoehere Praezedenz. Also herausholen und das Array updaten.
        // Die dabei entstehenden leeren Hints werden von den gesicherten
        // "uebergeplaettet".	(Bug: 6977)
        if( m_pSwpHints && nAttrCnt < m_pSwpHints->Count() )
        {
            SwpHts aArr( 5 );
            for ( ; nAttrCnt < m_pSwpHints->Count() &&
                    nEnd == *(pHt = m_pSwpHints->GetTextHint(nAttrCnt))->GetStart();
                   ++nAttrCnt )
            {
                pEndIdx = pHt->GetEnd();
                if ( pEndIdx && *pEndIdx == nEnd )
                {
                    aArr.C40_INSERT( SwTxtAttr, pHt, aArr.Count() );
                    m_pSwpHints->Delete( pHt );
                    --nAttrCnt;
                }
            }
            Update( rStart, nLen, TRUE, TRUE );

            for( nAttrCnt = 0; nAttrCnt < aArr.Count(); ++nAttrCnt )
            {
                pHt = aArr[ nAttrCnt ];
                *pHt->GetStart() = *pHt->GetEnd() = rStart.GetIndex();
                Insert( pHt );
            }
        }
        else
            Update( rStart, nLen, TRUE, TRUE );

        CHECK_SWPHINTS(this);
    }

    TryDeleteSwpHints();

    // Frames benachrichtigen;
    SwInsTxt aInsHint( nDestStart, nLen );
    pDest->Modify( 0, &aInsHint );
    SwDelTxt aDelHint( nTxtStartIdx, nLen );
    Modify( 0, &aDelHint );
}


SwTxtNode& SwTxtNode::Erase(const SwIndex &rIdx, xub_StrLen nCount,
                            const USHORT nMode )
{
    ASSERT( rIdx <= m_Text.Len(), "SwTxtNode::Erase: invalid index." );

    const xub_StrLen nCnt = STRING_LEN == nCount
                      ? m_Text.Len() - rIdx.GetIndex() : nCount;
    m_Text.Erase( rIdx.GetIndex(), nCnt );

    /* GCAttr(); alle leeren weggwerfen ist zu brutal.
     * Es duerfen nur die wegggeworfen werden,
     * die im Bereich liegen und nicht am Ende des Bereiches liegen
     */

    // check m_pSwpHints because fields and FlyFrames delete text (recursion!)
    for ( USHORT i = 0; m_pSwpHints && i < m_pSwpHints->Count(); ++i )
    {
        SwTxtAttr *pHt = m_pSwpHints->GetTextHint(i);

        const xub_StrLen nHtStt = *pHt->GetStart();

        if( nHtStt < rIdx.GetIndex() )
            continue;

        // TextFelder und FlyFrames loeschen Text (Rekursion)!!
        const xub_StrLen nEndIdx = rIdx.GetIndex() + nCnt;
        if( nHtStt > nEndIdx )
            // die Hints sind nach Ende sortiert, also ist Start
            // vom Hint groesser als EndIdx dann Abbrechen
            break;

        const xub_StrLen* pHtEndIdx = pHt->GetEnd();
        const USHORT nWhich = pHt->Which();

        if( !pHtEndIdx )
        {
            if (isTXTATR(nWhich) &&
                (nHtStt >= rIdx.GetIndex()) && (nHtStt < nEndIdx))
            {
                m_pSwpHints->DeleteAtPos(i);
                // Damit im Dtor der TxtAttr ohne End die CH_TXTATR nicht
                // geloescht werden...
                *(pHt->GetStart()) = USHRT_MAX;
                DestroyAttr( pHt );
                --i;
            }
            continue;
        }

        // Delete the hint if:
        // 1. The hint ends before the deletion end position or
        // 2. The hint ends at the deletion end position and
        //    we are not in empty expand mode and
        //    the hint is a [toxmark|refmark|ruby] text attribute
        if( *pHtEndIdx >= nEndIdx &&
            !(
                *pHtEndIdx == nEndIdx &&
                !(INS_EMPTYEXPAND & nMode)  &&
                (nWhich == RES_TXTATR_TOXMARK || nWhich == RES_TXTATR_REFMARK ||
                 // --> FME 2006-03-03 #i62668# Ruby text attribute has to be treated
                 // just like toxmark and refmarks
                 nWhich == RES_TXTATR_CJK_RUBY) )
                 // <--
             )
            continue;

        m_pSwpHints->DeleteAtPos(i);
        DestroyAttr( pHt );
        --i;
    }

    TryDeleteSwpHints();

    Update( rIdx, nCnt, TRUE );

    if( 1 == nCnt )
    {
        SwDelChr aHint( rIdx.GetIndex() );
        SwModify::Modify( 0, &aHint );
    }
    else
    {
        SwDelTxt aHint( rIdx.GetIndex(), nCnt );
        SwModify::Modify( 0, &aHint );
    }

    // By deleting a character, the hidden flags
    // at the TxtNode can become invalid:
    SetCalcHiddenCharFlags();

    CHECK_SWPHINTS(this);
    return *this;
}

/***********************************************************************
#*	Class		:	SwTxtNode
#*	Methode 	:	GCAttr
#*
#*	Beschreibung
#*					text.doc
#*
#*	Datum		:	MS 28.11.90
#*	Update		:	VB 24.07.91
#***********************************************************************/

void SwTxtNode::GCAttr()
{
    if ( !HasHints() )
        return;

    const SwTxtAttr *pHt = 0;
    const xub_StrLen *pEndIdx = 0;
    BOOL   bChanged = FALSE;
    USHORT nMin = m_Text.Len(),
           nMax = 0;
    BOOL bAll = nMin != 0; // Bei leeren Absaetzen werden nur die
                           // INet-Formate entfernt.

    for ( USHORT i = 0; m_pSwpHints && i < m_pSwpHints->Count(); ++i )
    {
        pHt = (*m_pSwpHints)[i];

        // wenn Ende und Start gleich sind --> loeschen
        pEndIdx = pHt->GetEnd();
        if( pEndIdx && (*pEndIdx == *pHt->GetStart())
            && ( bAll || pHt->Which() == RES_TXTATR_INETFMT ) )
        {
            bChanged = TRUE;
            nMin = Min( nMin, *pHt->GetStart() );
            nMax = Max( nMax, *pHt->GetEnd() );
            DestroyAttr( m_pSwpHints->Cut(i) );
            --i;
        }
        else
            ((SwTxtAttr*)pHt)->SetDontExpand( FALSE );
    }
    TryDeleteSwpHints();

    if(bChanged)
    {
        //TxtFrm's reagieren auf aHint, andere auf aNew
        SwUpdateAttr aHint( nMin, nMax, 0 );
        SwModify::Modify( 0, &aHint );
        SwFmtChg aNew( GetTxtColl() );
        SwModify::Modify( 0, &aNew );
    }
}
// #i23726#
SwNumRule* SwTxtNode::_GetNumRule(BOOL bInParent) const
{
    SwNumRule* pRet = 0;

    const SfxPoolItem* pItem = GetNoCondAttr( RES_PARATR_NUMRULE, bInParent );
    bool bNoNumRule = false;
    if ( pItem )
    {
        String sNumRuleName = static_cast<const SwNumRuleItem *>(pItem)->GetValue();
        if (sNumRuleName.Len() > 0)
        {
            pRet = GetDoc()->FindNumRulePtr( sNumRuleName );
        }
        else // numbering is turned off
            bNoNumRule = true;
    }

    if ( !bNoNumRule )
    {
        if ( pRet && pRet == GetDoc()->GetOutlineNumRule() &&
             ( !HasSwAttrSet() ||
               SFX_ITEM_SET !=
                GetpSwAttrSet()->GetItemState( RES_PARATR_NUMRULE, FALSE ) ) )
        {
            SwTxtFmtColl* pColl = GetTxtColl();
            if ( pColl )
            {
                const SwNumRuleItem& rDirectItem = pColl->GetNumRule( FALSE );
                if ( rDirectItem.GetValue().Len() == 0 )
                {
                    pRet = 0L;
                }
            }
        }

        // --> OD 2006-11-20 #i71764#
        // Document setting OUTLINE_LEVEL_YIELDS_OUTLINE_RULE has no influence
        // any more.
//        if ( !pRet &&
//             GetDoc()->get(IDocumentSettingAccess::OUTLINE_LEVEL_YIELDS_OUTLINE_RULE) &&
//             GetOutlineLevel() != NO_NUMBERING )
//        {
//            pRet = GetDoc()->GetOutlineNumRule();
//        }
        // <--
    }
    // old code before tuning
//    // --> OD 2005-10-25 #126347#
//    // determine of numbering/bullet rule, which is set as a hard attribute
//    // at the text node
//    const SfxPoolItem* pItem( 0L );
//    if ( HasSwAttrSet() ) // does text node has hard attributes ?
//    {
//        if ( SFX_ITEM_SET !=
//                GetpSwAttrSet()->GetItemState( RES_PARATR_NUMRULE, FALSE, &pItem ) )
//        {
//            pItem = 0L;
//        }
//        // else: <pItem> contains the numbering/bullet attribute, which is
//        //       hard set at the paragraph.

//    }
//    // <--
//    bool bNoNumRule = false;
//    if (pItem)
//    {
//        String sNumRuleName = static_cast<const SwNumRuleItem *>(pItem)->GetValue();
//        if (sNumRuleName.Len() > 0)
//        {
//            pRet = GetDoc()->FindNumRulePtr(sNumRuleName);
//        }
//        else // numbering is turned off by hard attribute
//            bNoNumRule = true;
//    }

//    if (! bNoNumRule)
//    {
//        if (! pRet && bInParent)
//        {
//            SwTxtFmtColl * pColl = GetTxtColl();

//            if (pColl)
//            {
//                const SwNumRuleItem & rItem = pColl->GetNumRule(TRUE);

//                pRet = const_cast<SwDoc *>(GetDoc())->
//                    FindNumRulePtrWithPool(rItem.GetValue());
//                // --> OD 2005-10-13 #125993# - The outline numbering rule
//                // isn't allowed to be derived from a parent paragraph style
//                // to a derived one.
//                // Thus check, if the found outline numbering rule is directly
//                // set at the paragraph style <pColl>. If not, don't return
//                // the outline numbering rule.
//                if ( pRet && pRet == GetDoc()->GetOutlineNumRule() )
//                {
//                    const SwNumRuleItem& rDirectItem = pColl->GetNumRule(FALSE);
//                    SwNumRule* pNumRuleAtParaStyle = const_cast<SwDoc*>(GetDoc())->
//                        FindNumRulePtrWithPool(rDirectItem.GetValue());
//                    if ( !pNumRuleAtParaStyle )
//                    {
//                        pRet = 0L;
//                    }
//                }
//                // <--
//            }
//        }

//        if (!pRet && GetDoc()->IsOutlineLevelYieldsOutlineRule() &&
//            GetOutlineLevel() != NO_NUMBERING)
//            pRet = GetDoc()->GetOutlineNumRule();
//    }
    // <--

    return pRet;
}

SwNumRule* SwTxtNode::GetNumRule(BOOL bInParent) const
{
    SwNumRule * pRet = _GetNumRule(bInParent);

    return pRet;
}

void SwTxtNode::NumRuleChgd()
{
    // --> OD 2008-04-04 #refactorlists#
    if ( IsInList() )
    {
        SwNumRule* pNumRule = GetNumRule();
        if ( pNumRule && pNumRule != GetNum()->GetNumRule() )
        {
            mpNodeNum->ChangeNumRule( *pNumRule );
        }
    }
    // <--

    if( IsInCache() )
    {
        SwFrm::GetCache().Delete( this );
        SetInCache( FALSE );
    }
    SetInSwFntCache( FALSE );

    SvxLRSpaceItem& rLR = (SvxLRSpaceItem&)GetSwAttrSet().GetLRSpace();

    SwModify::Modify( &rLR, &rLR );
}

// -> #i27615#
BOOL SwTxtNode::IsNumbered() const
{
    BOOL bResult = FALSE;

    SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule && IsCountedInList() )
        bResult = TRUE;

    return bResult;
}

// --> OD 2008-04-02 #refactorlists#
bool SwTxtNode::HasMarkedLabel() const
{
    bool bResult = false;

    if ( IsInList() )
    {
        bResult =
            GetDoc()->getListByName( GetListId() )->IsListLevelMarked( GetActualListLevel() );
    }

    return bResult;
}
// <--
// <- #i27615#

SwTxtNode* SwTxtNode::_MakeNewTxtNode( const SwNodeIndex& rPos, BOOL bNext,
                                        BOOL bChgFollow )
{
    /* hartes PageBreak/PageDesc/ColumnBreak aus AUTO-Set ignorieren */
    SwAttrSet* pNewAttrSet = 0;
    // --> OD 2007-07-10 #i75353#
    bool bClearHardSetNumRuleWhenFmtCollChanges( false );
    // <--
    if( HasSwAttrSet() )
    {
        pNewAttrSet = new SwAttrSet( *GetpSwAttrSet() );
        const SfxItemSet* pTmpSet = GetpSwAttrSet();

        if( bNext )		// der naechste erbt keine Breaks!
            pTmpSet = pNewAttrSet;

        // PageBreaks/PageDesc/ColBreak rausschmeissen.
        BOOL bRemoveFromCache = FALSE;
        std::vector<USHORT> aClearWhichIds;
        if ( bNext )
            bRemoveFromCache = ( 0 != pNewAttrSet->ClearItem( RES_PAGEDESC ) );
        else
            aClearWhichIds.push_back( RES_PAGEDESC );

        if( SFX_ITEM_SET == pTmpSet->GetItemState( RES_BREAK, FALSE ) )
        {
            if ( bNext )
                pNewAttrSet->ClearItem( RES_BREAK );
            else
                aClearWhichIds.push_back( RES_BREAK );
            bRemoveFromCache = TRUE;
        }
        if( SFX_ITEM_SET == pTmpSet->GetItemState( RES_KEEP, FALSE ) )
        {
            if ( bNext )
                pNewAttrSet->ClearItem( RES_KEEP );
            else
                aClearWhichIds.push_back( RES_KEEP );
            bRemoveFromCache = TRUE;
        }
        if( SFX_ITEM_SET == pTmpSet->GetItemState( RES_PARATR_SPLIT, FALSE ) )
        {
            if ( bNext )
                pNewAttrSet->ClearItem( RES_PARATR_SPLIT );
            else
                aClearWhichIds.push_back( RES_PARATR_SPLIT );
            bRemoveFromCache = TRUE;
        }
        if(SFX_ITEM_SET == pTmpSet->GetItemState(RES_PARATR_NUMRULE, FALSE))
        {
            SwNumRule * pRule = GetNumRule();

            if (pRule && IsOutline())
            {
                if ( bNext )
                    pNewAttrSet->ClearItem(RES_PARATR_NUMRULE);
                else
                {
                    // --> OD 2007-07-10 #i75353#
                    // No clear of hard set numbering rule at an outline paragraph at this point.
                    // Only if the paragraph style changes - see below.
//                    aClearWhichIds.push_back( RES_PARATR_NUMRULE );
                    bClearHardSetNumRuleWhenFmtCollChanges = true;
                    // <--
                }
                bRemoveFromCache = TRUE;
            }
        }

        if ( 0 != aClearWhichIds.size() )
            bRemoveFromCache = 0 != ClearItemsFromAttrSet( aClearWhichIds );

        if( !bNext && bRemoveFromCache && IsInCache() )
        {
            SwFrm::GetCache().Delete( this );
            SetInCache( FALSE );
        }
    }
    SwNodes& rNds = GetNodes();

    SwTxtFmtColl* pColl = GetTxtColl();

    SwTxtNode *pNode = new SwTxtNode( rPos, pColl, pNewAttrSet );

    if( pNewAttrSet )
        delete pNewAttrSet;

    const SwNumRule* pRule = GetNumRule();
    if( pRule && pRule == pNode->GetNumRule() && rNds.IsDocNodes() ) // #115901#
    {
        // --> OD 2005-10-18 #i55459#
        // - correction: parameter <bNext> has to be checked, as it was in the
        //   previous implementation.
        if ( !bNext && !IsCountedInList() )
            SetCountedInList(true);
        // <--
    }

    // jetzt kann es sein, das durch die Nummerierung dem neuen Node eine
    // Vorlage aus dem Pool zugewiesen wurde. Dann darf diese nicht
    // nochmal uebergeplaettet werden !!
    if( pColl != pNode->GetTxtColl() ||
        ( bChgFollow && pColl != GetTxtColl() ))
        return pNode;		// mehr duerfte nicht gemacht werden oder ????

    pNode->_ChgTxtCollUpdateNum( 0, pColl ); // fuer Nummerierung/Gliederung
    if( bNext || !bChgFollow )
        return pNode;

    SwTxtFmtColl *pNextColl = &pColl->GetNextTxtFmtColl();
    // --> OD 2009-08-12 #i101870#
    // perform action on different paragraph styles before applying the new paragraph style
    if (pNextColl != pColl)
    {
        // --> OD 2007-07-10 #i75353#
        if ( bClearHardSetNumRuleWhenFmtCollChanges )
        {
            std::vector<USHORT> aClearWhichIds;
            aClearWhichIds.push_back( RES_PARATR_NUMRULE );
            if ( ClearItemsFromAttrSet( aClearWhichIds ) != 0 && IsInCache() )
            {
                SwFrm::GetCache().Delete( this );
                SetInCache( FALSE );
            }
        }
        // <--
    }
    // <--
    ChgFmtColl( pNextColl );

    return pNode;
}

SwCntntNode* SwTxtNode::AppendNode( const SwPosition & rPos )
{
    // Position hinter dem eingefuegt wird
    SwNodeIndex aIdx( rPos.nNode, 1 );
    SwTxtNode* pNew = _MakeNewTxtNode( aIdx, TRUE );

    // --> OD 2008-05-14 #refactorlists#
    // reset list attributes at appended text node
    pNew->ResetAttr( RES_PARATR_LIST_ISRESTART );
    pNew->ResetAttr( RES_PARATR_LIST_RESTARTVALUE );
    pNew->ResetAttr( RES_PARATR_LIST_ISCOUNTED );
    if ( pNew->GetNumRule() == 0 )
    {
        pNew->ResetAttr( RES_PARATR_LIST_ID );
        pNew->ResetAttr( RES_PARATR_LIST_LEVEL );
    }
    // <--
    // --> OD 2008-03-13 #refactorlists#
//    SyncNumberAndNumRule();
    if ( !IsInList() && GetNumRule() && GetListId().Len() > 0 )
    {
        AddToList();
    }
    // <--

    if( GetDepends() )
        MakeFrms( *pNew );
    return pNew;
}

/*************************************************************************
 *						SwTxtNode::GetTxtAttr
 *
 * Diese Methode liefert nur Textattribute auf der Position nIdx
 * zurueck, die kein EndIdx besitzen und denselben Which besitzen.
 * Ueblicherweise steht an dieser Position ein CH_TXTATR.
 * Bei RES_TXTATR_END entfaellt die Pruefung auf den Which-Wert.
 *************************************************************************/

SwTxtAttr *SwTxtNode::GetTxtAttr( const xub_StrLen nIdx,
                                  const USHORT nWhichHt ) const
{
    if ( HasHints() )
    {
        for ( USHORT i = 0; i < m_pSwpHints->Count(); ++i )
        {
            SwTxtAttr *pPos = m_pSwpHints->GetTextHint(i);
            const xub_StrLen nStartPos = *pPos->GetStart();
            if( nIdx < nStartPos )
                return 0;
            if( nIdx == nStartPos && !pPos->GetEnd() )
            {
                if( RES_TXTATR_END == nWhichHt || nWhichHt == pPos->Which() )
                    return pPos;
                else
                    return 0;
            }
        }
    }
    return 0;
}

// -> #i29560#
BOOL SwTxtNode::HasNumber() const
{
    BOOL bResult = FALSE;

    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule )
    {
        SwNumFmt aFmt(pRule->Get( static_cast<USHORT>(GetActualListLevel())));

        // #i40041#
        bResult = aFmt.IsEnumeration() &&
            SVX_NUM_NUMBER_NONE != aFmt.GetNumberingType();
    }

    return bResult;
}

BOOL SwTxtNode::HasBullet() const
{
    BOOL bResult = FALSE;

    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule )
    {
        SwNumFmt aFmt(pRule->Get( static_cast<USHORT>(GetActualListLevel())));

        bResult = aFmt.IsItemize();
    }

    return bResult;
}
// <- #i29560#

// --> OD 2005-11-17 #128041# - introduce parameter <_bInclPrefixAndSuffixStrings>
//i53420 added max outline parameter
XubString SwTxtNode::GetNumString( const bool _bInclPrefixAndSuffixStrings, const unsigned int _nRestrictToThisLevel ) const
{
    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule &&
         IsCountedInList() &&
         pRule->Get( static_cast<USHORT>(GetActualListLevel()) ).IsTxtFmt() )
    {
        return pRule->MakeNumString( GetNum()->GetNumberVector(),
                                     _bInclPrefixAndSuffixStrings ? TRUE : FALSE,
                                     FALSE,
                                     _nRestrictToThisLevel );
    }

    return aEmptyStr;
}

long SwTxtNode::GetLeftMarginWithNum( BOOL bTxtLeft ) const
{
    long nRet = 0;
    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if( pRule )
    {
        const SwNumFmt& rFmt = pRule->Get(static_cast<USHORT>(GetActualListLevel()));
        // --> OD 2008-01-16 #newlistlevelattrs#
        if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
        {
            nRet = rFmt.GetAbsLSpace();

            if( !bTxtLeft )
            {
                if( 0 > rFmt.GetFirstLineOffset() &&
                    nRet > -rFmt.GetFirstLineOffset() )
                    nRet = nRet + rFmt.GetFirstLineOffset();
                else
                    nRet = 0;
            }

            if( pRule->IsAbsSpaces() )
                nRet = nRet - GetSwAttrSet().GetLRSpace().GetLeft();
        }
        else if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
        {
            if ( AreListLevelIndentsApplicable() )
            {
                nRet = rFmt.GetIndentAt();
                // --> OD 2008-06-06 #i90401#
                // Only negative first line indents have consider for the left margin
                if ( !bTxtLeft &&
                     rFmt.GetFirstLineIndent() < 0 )
                {
                    nRet = nRet + rFmt.GetFirstLineIndent();
                }
                // <--
            }
        }
        // <--
    }

    return nRet;
}

BOOL SwTxtNode::GetFirstLineOfsWithNum( short& rFLOffset ) const
{
    BOOL bRet( FALSE );

    // --> OD 2005-11-02 #i51089 - TUNING#
    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule )
    {
        if ( IsCountedInList() )
        {
            // --> OD 2008-01-16 #newlistlevelattrs#
            const SwNumFmt& rFmt = pRule->Get(static_cast<USHORT>(GetActualListLevel()));
            if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
            {
                rFLOffset = pRule->Get( static_cast<USHORT>(GetActualListLevel() )).GetFirstLineOffset();

                if (!getIDocumentSettingAccess()->get(IDocumentSettingAccess::IGNORE_FIRST_LINE_INDENT_IN_NUMBERING))
                {
                    SvxLRSpaceItem aItem = GetSwAttrSet().GetLRSpace();
                    rFLOffset = rFLOffset + aItem.GetTxtFirstLineOfst();
                }
            }
            else if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
            {
                if ( AreListLevelIndentsApplicable() )
                {
                    rFLOffset = static_cast<USHORT>(rFmt.GetFirstLineIndent());
                }
                else if (!getIDocumentSettingAccess()->get(IDocumentSettingAccess::IGNORE_FIRST_LINE_INDENT_IN_NUMBERING))
                {
                    SvxLRSpaceItem aItem = GetSwAttrSet().GetLRSpace();
                    rFLOffset = aItem.GetTxtFirstLineOfst();
                }
            }
            // <--
        }
        else
            rFLOffset = 0;

        bRet = TRUE;
    }
    else
    {
        rFLOffset = GetSwAttrSet().GetLRSpace().GetTxtFirstLineOfst();
    }

    return bRet;
}

// --> OD 2008-12-02 #i96772#
void SwTxtNode::ClearLRSpaceItemDueToListLevelIndents( SvxLRSpaceItem& o_rLRSpaceItem ) const
{
    if ( AreListLevelIndentsApplicable() )
    {
        const SwNumRule* pRule = GetNumRule();
        if ( pRule && GetActualListLevel() >= 0 )
        {
            const SwNumFmt& rFmt = pRule->Get(static_cast<USHORT>(GetActualListLevel()));
            if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
            {
                SvxLRSpaceItem aLR( RES_LR_SPACE );
                o_rLRSpaceItem = aLR;
            }
        }
    }
}
// <--

// --> OD 2008-07-01 #i91133#
long SwTxtNode::GetLeftMarginForTabCalculation() const
{
    long nLeftMarginForTabCalc = 0;

    bool bLeftMarginForTabCalcSetToListLevelIndent( false );
    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0;
    if( pRule )
    {
        const SwNumFmt& rFmt = pRule->Get(static_cast<USHORT>(GetActualListLevel()));
        if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
        {
            if ( AreListLevelIndentsApplicable() )
            {
                nLeftMarginForTabCalc = rFmt.GetIndentAt();
                bLeftMarginForTabCalcSetToListLevelIndent = true;
            }
        }
    }
    if ( !bLeftMarginForTabCalcSetToListLevelIndent )
    {
        nLeftMarginForTabCalc = GetSwAttrSet().GetLRSpace().GetTxtLeft();
    }

    return nLeftMarginForTabCalc;
}
// <--

void SwTxtNode::Replace0xFF( XubString& rTxt, xub_StrLen& rTxtStt,
                            xub_StrLen nEndPos, BOOL bExpandFlds ) const
{
    if( GetpSwpHints() )
    {
        sal_Unicode cSrchChr = CH_TXTATR_BREAKWORD;
        for( int nSrchIter = 0; 2 > nSrchIter; ++nSrchIter,
                                cSrchChr = CH_TXTATR_INWORD )
        {
            xub_StrLen nPos = rTxt.Search( cSrchChr );
            while( STRING_NOTFOUND != nPos && nPos < nEndPos )
            {
                const SwTxtAttr* pAttr = GetTxtAttr( rTxtStt + nPos );
                if( pAttr )
                {
                    switch( pAttr->Which() )
                    {
                    case RES_TXTATR_FIELD:
                        rTxt.Erase( nPos, 1 );
                        if( bExpandFlds )
                        {
                            const XubString aExpand( ((SwTxtFld*)pAttr)->GetFld().
                                                    GetFld()->Expand() );
                            rTxt.Insert( aExpand, nPos );
                            nPos = nPos + aExpand.Len();
                            nEndPos = nEndPos + aExpand.Len();
                            rTxtStt = rTxtStt - aExpand.Len();
                        }
                        ++rTxtStt;
                        break;
                    case RES_TXTATR_HARDBLANK:
                        rTxt.SetChar( nPos, ((SwTxtHardBlank*)pAttr)->GetChar() );
                        ++nPos;
                        ++nEndPos;
                        break;
                    case RES_TXTATR_FTN:
                        rTxt.Erase( nPos, 1 );
                        if( bExpandFlds )
                        {
                            const SwFmtFtn& rFtn = pAttr->GetFtn();
                            XubString sExpand;
                            if( rFtn.GetNumStr().Len() )
                                sExpand = rFtn.GetNumStr();
                            else if( rFtn.IsEndNote() )
                                sExpand = GetDoc()->GetEndNoteInfo().aFmt.
                                                GetNumStr( rFtn.GetNumber() );
                            else
                                sExpand = GetDoc()->GetFtnInfo().aFmt.
                                                GetNumStr( rFtn.GetNumber() );
                            rTxt.Insert( sExpand, nPos );
                            nPos = nPos + sExpand.Len();
                            nEndPos = nEndPos + sExpand.Len();
                            rTxtStt = rTxtStt - sExpand.Len();
                        }
                        ++rTxtStt;
                        break;
                    default:
                        rTxt.Erase( nPos, 1 );
                        ++rTxtStt;
                    }
                }
                else
                    ++nPos, ++nEndPos;
                nPos = rTxt.Search( cSrchChr, nPos );
            }
        }
    }
}

/*************************************************************************
 *                      SwTxtNode::GetExpandTxt
 * Expand fields
 *************************************************************************/
// --> OD 2007-11-15 #i83479# - handling of new parameters
XubString SwTxtNode::GetExpandTxt( const xub_StrLen nIdx,
                                   const xub_StrLen nLen,
                                   const bool bWithNum,
                                   const bool bAddSpaceAfterListLabelStr,
                                   const bool bWithSpacesForLevel ) const
{
    XubString aTxt( GetTxt().Copy( nIdx, nLen ) );
    xub_StrLen nTxtStt = nIdx;
    Replace0xFF( aTxt, nTxtStt, aTxt.Len(), TRUE );
    if( bWithNum )
    {
        XubString aListLabelStr = GetNumString();
        if ( aListLabelStr.Len() > 0 )
        {
            if ( bAddSpaceAfterListLabelStr )
            {
                const sal_Unicode aSpace = ' ';
                aTxt.Insert( aSpace, 0 );
            }
            aTxt.Insert( GetNumString(), 0 );
        }
    }

    if ( bWithSpacesForLevel && GetActualListLevel() > 0 )
    {
        int nLevel( GetActualListLevel() );
        while ( nLevel > 0 )
        {
            const sal_Unicode aSpace = ' ';
            aTxt.Insert( aSpace , 0 );
            aTxt.Insert( aSpace , 0 );
            --nLevel;
        }
    }

    return aTxt;
}
// <--

BOOL SwTxtNode::GetExpandTxt( SwTxtNode& rDestNd, const SwIndex* pDestIdx,
                        xub_StrLen nIdx, xub_StrLen nLen, BOOL bWithNum,
                        BOOL bWithFtn, BOOL bReplaceTabsWithSpaces ) const
{
    if( &rDestNd == this )
        return FALSE;

    SwIndex aDestIdx( &rDestNd, rDestNd.GetTxt().Len() );
    if( pDestIdx )
        aDestIdx = *pDestIdx;
    xub_StrLen nDestStt = aDestIdx.GetIndex();

    // Text einfuegen
    String sTmpText = GetTxt();
    if( bReplaceTabsWithSpaces )
        sTmpText.SearchAndReplaceAll('\t', ' ');

    // mask hidden characters
    const xub_Unicode cChar = CH_TXTATR_BREAKWORD;
    USHORT nHiddenChrs =
        SwScriptInfo::MaskHiddenRanges( *this, sTmpText, 0, sTmpText.Len(), cChar );

    sTmpText = sTmpText.Copy( nIdx, nLen );
    rDestNd.Insert( sTmpText, aDestIdx );
    nLen = aDestIdx.GetIndex() - nDestStt;

    // alle FontAttribute mit CHARSET Symbol in dem Bereich setzen
    if ( HasHints() )
    {
        xub_StrLen nInsPos = nDestStt - nIdx;
        for ( USHORT i = 0; i < m_pSwpHints->Count(); i++ )
        {
            const SwTxtAttr* pHt = (*m_pSwpHints)[i];
            xub_StrLen nAttrStartIdx;
            USHORT nWhich = pHt->Which();
            if( nIdx + nLen <= ( nAttrStartIdx = *pHt->GetStart() ) )
                break;		// ueber das Textende

            const SvxFontItem* pFont;
            const xub_StrLen *pEndIdx = pHt->GetEnd();
            if( pEndIdx && *pEndIdx > nIdx &&
                ( RES_CHRATR_FONT == nWhich ||
                  RES_TXTATR_CHARFMT == nWhich ||
                  RES_TXTATR_AUTOFMT == nWhich ) &&
                  0 != ( pFont = static_cast<const SvxFontItem*>(CharFmt::GetItem( *pHt, RES_CHRATR_FONT )) ) &&
                  RTL_TEXTENCODING_SYMBOL == pFont->GetCharSet() )
            {
                // Attribut liegt im Bereich, also kopieren
                rDestNd.InsertItem( *pFont, nInsPos + nAttrStartIdx,
                                        nInsPos + *pEndIdx );
            }
            else if( !pEndIdx && nAttrStartIdx >= nIdx )
            {
                aDestIdx = nInsPos + nAttrStartIdx;
                switch( nWhich )
                {
                case RES_TXTATR_FIELD:
                    {
                        const XubString aExpand( ((SwTxtFld*)pHt)->GetFld().GetFld()->Expand() );
                        if( aExpand.Len() )
                        {
                            aDestIdx++;		// dahinter einfuegen;
                            rDestNd.Insert( aExpand, aDestIdx );
                            aDestIdx = nInsPos + nAttrStartIdx;
                            nInsPos = nInsPos + aExpand.Len();
                        }
                        rDestNd.Erase( aDestIdx, 1 );
                        --nInsPos;
                    }
                    break;

                case RES_TXTATR_HARDBLANK:
                    rDestNd.m_Text.SetChar( nInsPos + nAttrStartIdx,
                                ((SwTxtHardBlank*)pHt)->GetChar() );
                    break;

                case RES_TXTATR_FTN:
                    {
                        if ( bWithFtn )
                        {
                            const SwFmtFtn& rFtn = pHt->GetFtn();
                            XubString sExpand;
                            if( rFtn.GetNumStr().Len() )
                                sExpand = rFtn.GetNumStr();
                            else if( rFtn.IsEndNote() )
                                sExpand = GetDoc()->GetEndNoteInfo().aFmt.
                                                GetNumStr( rFtn.GetNumber() );
                            else
                                sExpand = GetDoc()->GetFtnInfo().aFmt.
                                                GetNumStr( rFtn.GetNumber() );
                            if( sExpand.Len() )
                            {
                                aDestIdx++;     // dahinter einfuegen;
                                rDestNd.InsertItem( SvxEscapementItem(
                                        SVX_ESCAPEMENT_SUPERSCRIPT ),
                                        aDestIdx.GetIndex(),
                                        aDestIdx.GetIndex() );
                                rDestNd.Insert( sExpand, aDestIdx, INS_EMPTYEXPAND );
                                aDestIdx = nInsPos + nAttrStartIdx;
                                nInsPos = nInsPos + sExpand.Len();
                            }
                        }
                        rDestNd.Erase( aDestIdx, 1 );
                        --nInsPos;
                    }
                    break;

                default:
                    rDestNd.Erase( aDestIdx, 1 );
                    --nInsPos;
                }
            }
        }
    }

    if( bWithNum )
    {
        aDestIdx = nDestStt;
        rDestNd.Insert( GetNumString(), aDestIdx );
    }

    if ( nHiddenChrs > 0 )
    {
        aDestIdx = 0;
        while ( aDestIdx < rDestNd.GetTxt().Len() )
        {
            if ( cChar == rDestNd.GetTxt().GetChar( aDestIdx.GetIndex() ) )
            {
                xub_StrLen nIndex = aDestIdx.GetIndex();
                while ( nIndex < rDestNd.GetTxt().Len() &&
                        cChar == rDestNd.GetTxt().GetChar( ++nIndex ) )
                    ;
                rDestNd.Erase( aDestIdx, nIndex - aDestIdx.GetIndex() );
            }
            else
                ++aDestIdx;
        }
    }

    return TRUE;
}

const ModelToViewHelper::ConversionMap*
        SwTxtNode::BuildConversionMap( rtl::OUString& rRetText ) const
{
    const rtl::OUString& rNodeText = GetTxt();
    rRetText = rNodeText;
    ModelToViewHelper::ConversionMap* pConversionMap = 0;

    const SwpHints* pSwpHints2 = GetpSwpHints();
    xub_StrLen nPos = 0;

    for ( USHORT i = 0; pSwpHints2 && i < pSwpHints2->Count(); ++i )
    {
        const SwTxtAttr* pAttr = (*pSwpHints2)[i];
        if ( RES_TXTATR_FIELD == pAttr->Which() )
        {
            const XubString aExpand( ((SwTxtFld*)pAttr)->GetFld().GetFld()->Expand() );
            if ( aExpand.Len() > 0 )
            {
                const xub_StrLen nFieldPos = *pAttr->GetStart();
                rRetText = rRetText.replaceAt( nPos + nFieldPos, 1, aExpand );
                if ( !pConversionMap )
                    pConversionMap = new ModelToViewHelper::ConversionMap;
                pConversionMap->push_back(
                        ModelToViewHelper::ConversionMapEntry(
                            nFieldPos, nPos + nFieldPos ) );
                nPos += ( aExpand.Len() - 1 );
            }
        }
    }

    if ( pConversionMap && pConversionMap->size() )
        pConversionMap->push_back(
            ModelToViewHelper::ConversionMapEntry(
                rNodeText.getLength()+1, rRetText.getLength()+1 ) );

    return pConversionMap;
}

XubString SwTxtNode::GetRedlineTxt( xub_StrLen nIdx, xub_StrLen nLen,
                                BOOL bExpandFlds, BOOL bWithNum ) const
{
    SvUShorts aRedlArr;
    const SwDoc* pDoc = GetDoc();
    USHORT nRedlPos = pDoc->GetRedlinePos( *this, nsRedlineType_t::REDLINE_DELETE );
    if( USHRT_MAX != nRedlPos )
    {
        // es existiert fuer den Node irgendein Redline-Delete-Object
        const ULONG nNdIdx = GetIndex();
        for( ; nRedlPos < pDoc->GetRedlineTbl().Count() ; ++nRedlPos )
        {
            const SwRedline* pTmp = pDoc->GetRedlineTbl()[ nRedlPos ];
            if( nsRedlineType_t::REDLINE_DELETE == pTmp->GetType() )
            {
                const SwPosition *pRStt = pTmp->Start(), *pREnd = pTmp->End();
                if( pRStt->nNode < nNdIdx )
                {
                    if( pREnd->nNode > nNdIdx )
                        // Absatz ist komplett geloescht
                        return aEmptyStr;
                    else if( pREnd->nNode == nNdIdx )
                    {
                        // von 0 bis nContent ist alles geloescht
                        aRedlArr.Insert( xub_StrLen(0), aRedlArr.Count() );
                        aRedlArr.Insert( pREnd->nContent.GetIndex(), aRedlArr.Count() );
                    }
                }
                else if( pRStt->nNode == nNdIdx )
                {
                    aRedlArr.Insert( pRStt->nContent.GetIndex(), aRedlArr.Count() );
                    if( pREnd->nNode == nNdIdx )
                        aRedlArr.Insert( pREnd->nContent.GetIndex(), aRedlArr.Count() );
                    else
                    {
                        aRedlArr.Insert( GetTxt().Len(), aRedlArr.Count() );
                        break; 		// mehr kann nicht kommen
                    }
                }
                else
                    break; 		// mehr kann nicht kommen
            }
        }
    }

    XubString aTxt( GetTxt().Copy( nIdx, nLen ) );

    xub_StrLen nTxtStt = nIdx, nIdxEnd = nIdx + aTxt.Len();
    for( USHORT n = 0; n < aRedlArr.Count(); n += 2 )
    {
        xub_StrLen nStt = aRedlArr[ n ], nEnd = aRedlArr[ n+1 ];
        if( ( nIdx <= nStt && nStt <= nIdxEnd ) ||
            ( nIdx <= nEnd && nEnd <= nIdxEnd ))
        {
            if( nStt < nIdx ) nStt = nIdx;
            if( nIdxEnd < nEnd ) nEnd = nIdxEnd;
            xub_StrLen nDelCnt = nEnd - nStt;
            aTxt.Erase( nStt - nTxtStt, nDelCnt );
            Replace0xFF( aTxt, nTxtStt, nStt - nTxtStt, bExpandFlds );
            nTxtStt = nTxtStt + nDelCnt;
        }
        else if( nStt >= nIdxEnd )
            break;
    }
    Replace0xFF( aTxt, nTxtStt, aTxt.Len(), bExpandFlds );

    if( bWithNum )
        aTxt.Insert( GetNumString(), 0 );
    return aTxt;
}

/*************************************************************************
 *						SwTxtNode::GetExpandTxt
 *************************************************************************/
// Felder werden expandiert:

void SwTxtNode::Replace( const SwIndex& rStart, xub_Unicode cCh )
{
    ASSERT( rStart.GetIndex() < m_Text.Len(),
        "SwTxtNode::Replace: invalid index." );

    SwTxtAttr* pHt;
    if ( ( CH_TXTATR_BREAKWORD == m_Text.GetChar( rStart.GetIndex() ) ||
           CH_TXTATR_INWORD == m_Text.GetChar( rStart.GetIndex() )) &&
        0 != ( pHt = GetTxtAttr( rStart.GetIndex() ) ))
    {
        Delete( pHt );
        m_Text.Insert( cCh, rStart.GetIndex() );
    }
    else
    {
        m_Text.SetChar( rStart.GetIndex(), cCh );
    }

    SwDelTxt aDelHint( rStart.GetIndex(), 1 );
    SwModify::Modify( 0, &aDelHint );

    SwInsTxt aHint( rStart.GetIndex(), 1 );
    SwModify::Modify( 0, &aHint );
}


void SwTxtNode::Replace( const SwIndex& rStart, xub_StrLen nLen,
                            const XubString& rText )
{
    ASSERT( rStart.GetIndex() < m_Text.Len() &&
            rStart.GetIndex() + nLen <= m_Text.Len(),
            "ausserhalb des Strings" );
    SwTxtAttr* pHt;
    const xub_StrLen nStartPos = rStart.GetIndex();
    xub_StrLen nEnde = nStartPos + nLen;
    xub_StrLen nDelLen = nLen;
    for( xub_StrLen nPos = nStartPos; nPos < nEnde; ++nPos )
    {
        if ( ( CH_TXTATR_BREAKWORD == m_Text.GetChar( nPos ) ||
               CH_TXTATR_INWORD == m_Text.GetChar( nPos )) &&
            0 != ( pHt = GetTxtAttr( nPos ) ))
        {
            Delete( pHt );
            --nEnde;
            --nLen;
        }
    }

    BOOL bOldExpFlg = IsIgnoreDontExpand();
    SetIgnoreDontExpand( TRUE );

    if( nLen && rText.Len() )
    {
        // dann das 1. Zeichen ersetzen den Rest loschen und einfuegen
        // Dadurch wird die Attributierung des 1. Zeichen expandiert!
        m_Text.SetChar( nStartPos, rText.GetChar( 0 ) );

        ((SwIndex&)rStart)++;
        m_Text.Erase( rStart.GetIndex(), nLen - 1 );
        Update( rStart, nLen - 1, TRUE );

        XubString aTmpTxt( rText ); aTmpTxt.Erase( 0, 1 );
        m_Text.Insert( aTmpTxt, rStart.GetIndex() );
        Update( rStart, aTmpTxt.Len(), FALSE );
    }
    else
    {
        m_Text.Erase( nStartPos, nLen );
        Update( rStart, nLen, TRUE );

        m_Text.Insert( rText, nStartPos );
        Update( rStart, rText.Len(), FALSE );
    }
    SetIgnoreDontExpand( bOldExpFlg );
    SwDelTxt aDelHint( nStartPos, nDelLen );
    SwModify::Modify( 0, &aDelHint );

    SwInsTxt aHint( nStartPos, rText.Len() );
    SwModify::Modify( 0, &aHint );
}

// --> OD 2008-03-27 #refactorlists#
namespace {
    // Helper method for special handling of modified attributes at text node.
    // The following is handled:
    // (1) on changing the paragraph style - RES_FMT_CHG:
    // Check, if list style of the text node is changed. If yes, add respectively
    // remove the text node to the corresponding list.
    // (2) on changing the attributes - RES_ATTRSET_CHG:
    // Same as (1).
    // (3) on changing the list style - RES_PARATR_NUMRULE:
    // Same as (1).
    void HandleModifyAtTxtNode( SwTxtNode& rTxtNode,
                                const SfxPoolItem* pOldValue,
                                const SfxPoolItem* pNewValue )
    {
        const USHORT nWhich = pOldValue ? pOldValue->Which() :
                              pNewValue ? pNewValue->Which() : 0 ;
        bool bNumRuleSet = false;
        bool bParagraphStyleChanged = false;
        String sNumRule;
        String sOldNumRule;
        switch ( nWhich )
        {
            case RES_FMT_CHG:
            {
                bParagraphStyleChanged = true;
                if( rTxtNode.GetNodes().IsDocNodes() )
                {
                    // --> OD 2008-12-17 #i70748#
                    // The former list style set at the paragraph can not be
                    // retrieved from the change set.
//                    sOldNumRule =
//                        dynamic_cast<const SwFmtChg*>(pOldValue)->pChangedFmt->GetNumRule().GetValue();
                    const SwNumRule* pFormerNumRuleAtTxtNode =
                        rTxtNode.GetNum() ? rTxtNode.GetNum()->GetNumRule() : 0;
                    if ( pFormerNumRuleAtTxtNode )
                    {
                        sOldNumRule = pFormerNumRuleAtTxtNode->GetName();
                    }
                    // <--
                    // --> OD 2008-11-19 #i70748#
                    if ( rTxtNode.IsEmptyListStyleDueToSetOutlineLevelAttr() )
                    {
                        const SwNumRuleItem& rNumRuleItem = rTxtNode.GetTxtColl()->GetNumRule();
                        if ( rNumRuleItem.GetValue().Len() > 0 )
                        {
                            rTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
                        }
                    }
                    // <--
                    const SwNumRule* pNumRuleAtTxtNode = rTxtNode.GetNumRule();
                    if ( pNumRuleAtTxtNode )
                    {
                        bNumRuleSet = true;
                        sNumRule = pNumRuleAtTxtNode->GetName();
                    }
                }
                break;
            }
            case RES_ATTRSET_CHG:
            {
                const SfxPoolItem* pItem = 0;
                // --> OD 2008-12-19 #i70748#
                // The former list style set at the paragraph can not be
                // retrieved from the change set.
//                if ( dynamic_cast<const SwAttrSetChg*>(pOldValue)->GetChgSet()->GetItemState( RES_PARATR_NUMRULE, FALSE, &pItem ) ==
//                        SFX_ITEM_SET )
//                {
//                    sOldNumRule = dynamic_cast<const SwNumRuleItem*>(pItem)->GetValue();
//                }
                const SwNumRule* pFormerNumRuleAtTxtNode =
                    rTxtNode.GetNum() ? rTxtNode.GetNum()->GetNumRule() : 0;
                if ( pFormerNumRuleAtTxtNode )
                {
                    sOldNumRule = pFormerNumRuleAtTxtNode->GetName();
                }
                // <--
                if ( dynamic_cast<const SwAttrSetChg*>(pNewValue)->GetChgSet()->GetItemState( RES_PARATR_NUMRULE, FALSE, &pItem ) ==
                        SFX_ITEM_SET )
                {
                    // --> OD 2008-11-19 #i70748#
                    rTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
                    // <--
                    bNumRuleSet = true;
                    // The new list style set at the paragraph can not be
                    // retrieved from the change set.
//                    sNumRule = dynamic_cast<const SwNumRuleItem*>(pItem)->GetValue();
                    // <--
                }
                // --> OD 2008-12-17 #i70748#
                // The new list style set at the paragraph.
                const SwNumRule* pNumRuleAtTxtNode = rTxtNode.GetNumRule();
                if ( pNumRuleAtTxtNode )
                {
                    sNumRule = pNumRuleAtTxtNode->GetName();
                }
                // <--
                break;
            }
            case RES_PARATR_NUMRULE:
            {
                if ( rTxtNode.GetNodes().IsDocNodes() )
                {
                    // The former list style set at the paragraph can not be
                    // retrieved from the change set.
//                    if ( pOldValue )
//                    {
//                        sOldNumRule = dynamic_cast<const SwNumRuleItem*>(pOldValue)->GetValue();
//                    }
                    const SwNumRule* pFormerNumRuleAtTxtNode =
                        rTxtNode.GetNum() ? rTxtNode.GetNum()->GetNumRule() : 0;
                    if ( pFormerNumRuleAtTxtNode )
                    {
                        sOldNumRule = pFormerNumRuleAtTxtNode->GetName();
                    }
                    // <--
                    if ( pNewValue )
                    {
                        // --> OD 2008-11-19 #i70748#
                        rTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
                        // <--
                        bNumRuleSet = true;
                        // --> OD 2008-12-17 #i70748#
                        // The new list style set at the paragraph can not be
                        // retrieved from the change set.
//                        sNumRule = dynamic_cast<const SwNumRuleItem*>(pNewValue)->GetValue();
                        // <--
                    }
                    // --> OD 2008-12-17 #i70748#
                    // The new list style set at the paragraph.
                    const SwNumRule* pNumRuleAtTxtNode = rTxtNode.GetNumRule();
                    if ( pNumRuleAtTxtNode )
                    {
                        sNumRule = pNumRuleAtTxtNode->GetName();
                    }
                    // <--
                }
                break;
            }
        }
        if ( sNumRule != sOldNumRule )
        {
            if ( bNumRuleSet )
            {
                if ( sNumRule.Len() == 0 )
                {
                    rTxtNode.RemoveFromList();
                    if ( bParagraphStyleChanged )
                    {
                        SvUShortsSort aResetAttrsArray;
                        aResetAttrsArray.Insert( RES_PARATR_LIST_ID );
                        aResetAttrsArray.Insert( RES_PARATR_LIST_LEVEL );
                        aResetAttrsArray.Insert( RES_PARATR_LIST_ISRESTART );
                        aResetAttrsArray.Insert( RES_PARATR_LIST_RESTARTVALUE );
                        aResetAttrsArray.Insert( RES_PARATR_LIST_ISCOUNTED );
                        SwPaM aPam( rTxtNode );
                        // --> OD 2008-11-28 #i96644#
                        // suppress side effect "send data changed events"
                        rTxtNode.GetDoc()->ResetAttrs( aPam, sal_False,
                                                       &aResetAttrsArray,
                                                       false );
                        // <--
                    }
                }
                else
                {
                    rTxtNode.RemoveFromList();
                    // If new list style is the outline style, apply outline
                    // level as the list level.
                    if ( sNumRule ==
                            String::CreateFromAscii( SwNumRule::GetOutlineRuleName() ) )
                    {
                        // --> OD 2008-09-10 #i70748#
                        ASSERT( rTxtNode.GetTxtColl()->IsAssignedToListLevelOfOutlineStyle(),
                                "<HandleModifyAtTxtNode()> - text node with outline style, but its paragraph style is not assigned to outline style." );
                        int nNewListLevel =
                            rTxtNode.GetTxtColl()->GetAssignedOutlineStyleLevel();
                        // <--
                        if ( 0 <= nNewListLevel && nNewListLevel < MAXLEVEL )
                        {
                            rTxtNode.SetAttrListLevel( nNewListLevel );
                        }
                    }
                    rTxtNode.AddToList();
                }
            }
            else // <sNumRule.Len() == 0 && sOldNumRule.Len() != 0>
            {
                rTxtNode.RemoveFromList();
                if ( bParagraphStyleChanged )
                {
                    SvUShortsSort aResetAttrsArray;
                    aResetAttrsArray.Insert( RES_PARATR_LIST_ID );
                    aResetAttrsArray.Insert( RES_PARATR_LIST_LEVEL );
                    aResetAttrsArray.Insert( RES_PARATR_LIST_ISRESTART );
                    aResetAttrsArray.Insert( RES_PARATR_LIST_RESTARTVALUE );
                    aResetAttrsArray.Insert( RES_PARATR_LIST_ISCOUNTED );
                    SwPaM aPam( rTxtNode );
                    // --> OD 2008-11-28 #i96644#
                    // suppress side effect "send data changed events"
                    rTxtNode.GetDoc()->ResetAttrs( aPam, sal_False,
                                                   &aResetAttrsArray,
                                                   false );
                    // <--
                    // --> OD 2008-11-19 #i70748#
                    if ( dynamic_cast<const SfxUInt16Item &>(rTxtNode.GetAttr( RES_PARATR_OUTLINELEVEL, FALSE )).GetValue() > 0 )
                    {
                        rTxtNode.SetEmptyListStyleDueToSetOutlineLevelAttr();
                    }
                    // <--
                }
            }
        }
        else if ( sNumRule.Len() > 0 && !rTxtNode.IsInList() )
        {
            rTxtNode.AddToList();
        }
        // <--
    }
    // End of method <HandleModifyAtTxtNode>
}
// <--

void SwTxtNode::Modify( SfxPoolItem* pOldValue, SfxPoolItem* pNewValue )
{
    bool bWasNotifiable = m_bNotifiable;
    m_bNotifiable = false;

    // Bug 24616/24617:
    // 		Modify ueberladen, damit beim Loeschen von Vorlagen diese
    // 		wieder richtig verwaltet werden (Outline-Numerierung!!)
    // 	Bug25481:
    //		bei Nodes im Undo nie _ChgTxtCollUpdateNum rufen.
    if( pOldValue && pNewValue && RES_FMT_CHG == pOldValue->Which() &&
        pRegisteredIn == ((SwFmtChg*)pNewValue)->pChangedFmt &&
        GetNodes().IsDocNodes() )
    {
        _ChgTxtCollUpdateNum(
                        (SwTxtFmtColl*)((SwFmtChg*)pOldValue)->pChangedFmt,
                        (SwTxtFmtColl*)((SwFmtChg*)pNewValue)->pChangedFmt );
    }

    // --> OD 2008-03-27 #refactorlists#
    if ( !mbInSetOrResetAttr )
    {
        HandleModifyAtTxtNode( *this, pOldValue, pNewValue );
    }
    // <--

    SwCntntNode::Modify( pOldValue, pNewValue );

    SwDoc * pDoc = GetDoc();
    // --> OD 2005-11-02 #125329# - assure that text node is in document nodes array
    if ( pDoc && !pDoc->IsInDtor() && &pDoc->GetNodes() == &GetNodes() )
    // <--
    {
        pDoc->GetNodes().UpdateOutlineNode(*this);
    }

    m_bNotifiable = bWasNotifiable;
}

SwFmtColl* SwTxtNode::ChgFmtColl( SwFmtColl *pNewColl )
{
    ASSERT( pNewColl,"ChgFmtColl: Collectionpointer ist 0." );
    ASSERT( HAS_BASE( SwTxtFmtColl, pNewColl ),
                "ChgFmtColl: ist kein Text-Collectionpointer." );

    SwTxtFmtColl *pOldColl = GetTxtColl();
    if( pNewColl != pOldColl )
    {
        SetCalcHiddenCharFlags();
        SwCntntNode::ChgFmtColl( pNewColl );
        // --> OD 2008-03-27 #refactorlists#
//        NumRuleChgd();
#if OSL_DEBUG_LEVEL > 1
        ASSERT( !mbInSetOrResetAttr,
                "DEBUG ASSERTION - <SwTxtNode::ChgFmtColl(..)> called during <Set/ResetAttr(..)>" )
#endif
        if ( !mbInSetOrResetAttr )
        {
            SwFmtChg aTmp1( pOldColl );
            SwFmtChg aTmp2( pNewColl );
            HandleModifyAtTxtNode( *this, &aTmp1, &aTmp2  );
        }
        // <--
    }

    // nur wenn im normalen Nodes-Array
    if( GetNodes().IsDocNodes() )
    {
        _ChgTxtCollUpdateNum( pOldColl, static_cast<SwTxtFmtColl *>(pNewColl) );
    }

    GetNodes().UpdateOutlineNode(*this);

    return pOldColl;
}

SwNodeNum* SwTxtNode::CreateNum() const
{
    if ( !mpNodeNum )
    {
        // --> OD 2008-02-19 #refactorlists#
        mpNodeNum = new SwNodeNum( const_cast<SwTxtNode*>(this) );
        // <--
    }
    return mpNodeNum;
}

SwNumberTree::tNumberVector SwTxtNode::GetNumberVector() const
{
    if ( GetNum() )
    {
        return GetNum()->GetNumberVector();
    }
    else
    {
        SwNumberTree::tNumberVector aResult;
        return aResult;
    }
}

bool SwTxtNode::IsOutline() const
{
    bool bResult = false;

    //if ( GetOutlineLevel() != NO_NUMBERING )//#outline level,removed by zhaojianwei
    if ( GetAttrOutlineLevel() > 0 )            //<-end,zhaojianwei
    {
        bResult = !IsInRedlines();
    }
    else
    {
        const SwNumRule* pRule( GetNum() ? GetNum()->GetNumRule() : 0L );
        if ( pRule && pRule->IsOutlineRule() )
        {
            bResult = !IsInRedlines();
        }
    }

    return bResult;
}

bool SwTxtNode::IsOutlineStateChanged() const
{
    return IsOutline() != m_bLastOutlineState;
}

void SwTxtNode::UpdateOutlineState()
{
    m_bLastOutlineState = IsOutline();
}

//#outline level, zhaojianwei
int SwTxtNode::GetAttrOutlineLevel() const
{
    return ((const SfxUInt16Item &)GetAttr(RES_PARATR_OUTLINELEVEL)).GetValue();
}
void SwTxtNode::SetAttrOutlineLevel(int nLevel)
{
    ASSERT( 0 <= nLevel && nLevel <= MAXLEVEL ,"SwTxtNode: Level Out Of Range" );//#outline level,zhaojianwei
    if ( 0 <= nLevel && nLevel <= MAXLEVEL )
    {
        SetAttr( SfxUInt16Item( RES_PARATR_OUTLINELEVEL,
                                static_cast<UINT16>(nLevel) ) );
    }
}
//<-end

// --> OD 2008-11-19 #i70748#
bool SwTxtNode::IsEmptyListStyleDueToSetOutlineLevelAttr()
{
    return mbEmptyListStyleSetDueToSetOutlineLevelAttr;
}

void SwTxtNode::SetEmptyListStyleDueToSetOutlineLevelAttr()
{
    if ( !mbEmptyListStyleSetDueToSetOutlineLevelAttr )
    {
        SetAttr( SwNumRuleItem() );
        mbEmptyListStyleSetDueToSetOutlineLevelAttr = true;
    }
}

void SwTxtNode::ResetEmptyListStyleDueToResetOutlineLevelAttr()
{
    if ( mbEmptyListStyleSetDueToSetOutlineLevelAttr )
    {
        ResetAttr( RES_PARATR_NUMRULE );
        mbEmptyListStyleSetDueToSetOutlineLevelAttr = false;
    }
}
// <--


// --> OD 2008-02-27 #refactorlists#
void SwTxtNode::SetAttrListLevel( int nLevel )
{
    if ( nLevel < 0 || nLevel >= MAXLEVEL )
    {
        ASSERT( false,
                "<SwTxtNode::SetAttrListLevel()> - value of parameter <nLevel> is out of valid range" );
        return;
    }

    SfxInt16Item aNewListLevelItem( RES_PARATR_LIST_LEVEL,
                                    static_cast<INT16>(nLevel) );
    SetAttr( aNewListLevelItem );
}
// <--
// --> OD 2008-02-27 #refactorlists#
bool SwTxtNode::HasAttrListLevel() const
{
    return GetpSwAttrSet() &&
           GetpSwAttrSet()->GetItemState( RES_PARATR_LIST_LEVEL, FALSE ) == SFX_ITEM_SET;
}
// <--
// --> OD 2008-02-27 #refactorlists#
int SwTxtNode::GetAttrListLevel() const
{
    int nAttrListLevel = 0;

    const SfxInt16Item& aListLevelItem =
        dynamic_cast<const SfxInt16Item&>(GetAttr( RES_PARATR_LIST_LEVEL ));
    nAttrListLevel = static_cast<int>(aListLevelItem.GetValue());

    return nAttrListLevel;
}
// <--

int SwTxtNode::GetActualListLevel() const
{
    return GetNum() ? GetNum()->GetLevelInListTree() : -1;
}

// --> OD 2008-02-25 #refactorlists#
void SwTxtNode::SetListRestart( bool bRestart )
{
//    CreateNum()->SetRestart(bRestart);
    if ( !bRestart )
    {
        // attribute not contained in paragraph style's attribute set. Thus,
        // it can be reset to the attribute pool default by resetting the attribute.
        ResetAttr( RES_PARATR_LIST_ISRESTART );
    }
    else
    {
        SfxBoolItem aNewIsRestartItem( RES_PARATR_LIST_ISRESTART,
                                       TRUE );
        SetAttr( aNewIsRestartItem );
    }
}

// --> OD 2008-02-25 #refactorlists#
bool SwTxtNode::IsListRestart() const
{
//    return GetNum() ? GetNum()->IsRestart() : false;
    const SfxBoolItem& aIsRestartItem =
        dynamic_cast<const SfxBoolItem&>(GetAttr( RES_PARATR_LIST_ISRESTART ));

    return aIsRestartItem.GetValue() ? true : false;
}
// <--

/** Returns if the paragraph has a visible numbering or bullet.
    This includes all kinds of numbering/bullet/outlines.
    OD 2008-02-28 #newlistlevelattrs#
    The concrete list label string has to be checked, too.
 */
bool SwTxtNode::HasVisibleNumberingOrBullet() const
{
    bool bRet = false;

    const SwNumRule* pRule = GetNum() ? GetNum()->GetNumRule() : 0L;
    if ( pRule && IsCountedInList())
    {
        // --> OD 2008-03-19 #i87154#
        // Correction of #newlistlevelattrs#:
        // The numbering type has to be checked for bullet lists.
        const SwNumFmt& rFmt = pRule->Get( static_cast<USHORT>(GetActualListLevel() ));
        if ( SVX_NUM_NUMBER_NONE != rFmt.GetNumberingType() ||
             pRule->MakeNumString( *(GetNum()) ).Len() > 0 )
        {
            bRet = true;
        }
        // <--
    }

    return bRet;
}

// --> OD 2008-02-25 #refactorlists#
void SwTxtNode::SetAttrListRestartValue( SwNumberTree::tSwNumTreeNumber nNumber )
{
//    CreateNum()->SetStart(nNumber);
    const bool bChanged( HasAttrListRestartValue()
                         ? GetAttrListRestartValue() != nNumber
                         : nNumber != USHRT_MAX );

    if ( bChanged || !HasAttrListRestartValue() )
    {
        if ( nNumber == USHRT_MAX )
        {
            ResetAttr( RES_PARATR_LIST_RESTARTVALUE );
        }
        else
        {
            SfxInt16Item aNewListRestartValueItem( RES_PARATR_LIST_RESTARTVALUE,
                                                   static_cast<INT16>(nNumber) );
            SetAttr( aNewListRestartValueItem );
        }
    }
}
// <--

// --> OD 2008-02-27 #refactorlists#
bool SwTxtNode::HasAttrListRestartValue() const
{
    return GetpSwAttrSet() &&
           GetpSwAttrSet()->GetItemState( RES_PARATR_LIST_RESTARTVALUE, FALSE ) == SFX_ITEM_SET;
}
// <--
SwNumberTree::tSwNumTreeNumber SwTxtNode::GetAttrListRestartValue() const
{
    ASSERT( HasAttrListRestartValue(),
            "<SwTxtNode::GetAttrListRestartValue()> - only ask for list restart value, if attribute is set at text node." );

    const SfxInt16Item& aListRestartValueItem =
        dynamic_cast<const SfxInt16Item&>(GetAttr( RES_PARATR_LIST_RESTARTVALUE ));
    return static_cast<SwNumberTree::tSwNumTreeNumber>(aListRestartValueItem.GetValue());
}

// --> OD 2008-02-25 #refactorlists#
SwNumberTree::tSwNumTreeNumber SwTxtNode::GetActualListStartValue() const
{
//    return GetNum() ? GetNum()->GetStart() : 1;
    SwNumberTree::tSwNumTreeNumber nListRestartValue = 1;

    if ( IsListRestart() && HasAttrListRestartValue() )
    {
        nListRestartValue = GetAttrListRestartValue();
    }
    else
    {
        SwNumRule* pRule = GetNumRule();
        if ( pRule )
        {
            const SwNumFmt* pFmt =
                    pRule->GetNumFmt( static_cast<USHORT>(GetAttrListLevel()) );
            if ( pFmt )
            {
                nListRestartValue = pFmt->GetStart();
            }
        }
    }

    return nListRestartValue;
}
// <--

bool SwTxtNode::IsNotifiable() const
{
    return m_bNotifiable && IsNotificationEnabled();
}

bool SwTxtNode::IsNotificationEnabled() const
{
    bool bResult = false;
    const SwDoc * pDoc = GetDoc();
    if( pDoc )
    {
        bResult = pDoc->IsInReading() || pDoc->IsInDtor() ? false : true;
    }
    return bResult;
}

// --> OD 2008-02-27 #refactorlists#
void SwTxtNode::SetCountedInList( bool bCounted )
{
    if ( bCounted )
    {
        // attribute not contained in paragraph style's attribute set. Thus,
        // it can be reset to the attribute pool default by resetting the attribute.
        ResetAttr( RES_PARATR_LIST_ISCOUNTED );
    }
    else
    {
        SfxBoolItem aIsCountedInListItem( RES_PARATR_LIST_ISCOUNTED, FALSE );
        SetAttr( aIsCountedInListItem );
    }
}
// <--

bool SwTxtNode::IsCountedInList() const
{
    const SfxBoolItem& aIsCountedInListItem =
        dynamic_cast<const SfxBoolItem&>(GetAttr( RES_PARATR_LIST_ISCOUNTED ));

    return aIsCountedInListItem.GetValue() ? true : false;
}

// --> OD 2008-03-13 #refactorlists#
void SwTxtNode::AddToList()
{
    if ( IsInList() )
    {
        ASSERT( false,
                "<SwTxtNode::AddToList()> - the text node is already added to a list. Serious defect -> please inform OD" );
        return;
    }

    const String sListId = GetListId();
    if ( sListId.Len() > 0 )
    {
        SwList* pList = GetDoc()->getListByName( sListId );
        if ( pList == 0 )
        {
            // Create corresponding list.
            SwNumRule* pNumRule = GetNumRule();
            if ( pNumRule )
            {
                pList = GetDoc()->createList( sListId, GetNumRule()->GetName() );
            }
        }
        ASSERT( pList != 0,
                "<SwTxtNode::AddToList()> - no list for given list id. Serious defect -> please inform OD" );
        if ( pList )
        {
            pList->InsertListItem( *CreateNum(), GetAttrListLevel() );
            mpList = pList;
        }
    }
}

void SwTxtNode::RemoveFromList()
{
    if ( IsInList() )
    {
        mpList->RemoveListItem( *mpNodeNum );
        mpList = 0;
        delete mpNodeNum;
        mpNodeNum = 0L;
    }
}

bool SwTxtNode::IsInList() const
{
    return GetNum() != 0 && GetNum()->GetParent() != 0;
}
// <--

bool SwTxtNode::IsFirstOfNumRule() const
{
    bool bResult = false;

    if ( GetNum() && GetNum()->GetNumRule())
        bResult = GetNum()->IsFirst();

    return bResult;
}

// --> OD 2008-02-20 #refactorlists#
void SwTxtNode::SetListId( const String sListId )
{
    const SfxStringItem& rListIdItem =
            dynamic_cast<const SfxStringItem&>(GetAttr( RES_PARATR_LIST_ID ));
    if ( rListIdItem.GetValue() != sListId )
    {
        if ( sListId.Len() == 0 )
        {
            ResetAttr( RES_PARATR_LIST_ID );
        }
        else
        {
            SfxStringItem aNewListIdItem( RES_PARATR_LIST_ID, sListId );
            SetAttr( aNewListIdItem );
        }
    }
}

String SwTxtNode::GetListId() const
{
    String sListId;

    const SfxStringItem& rListIdItem =
                dynamic_cast<const SfxStringItem&>(GetAttr( RES_PARATR_LIST_ID ));
    sListId = rListIdItem.GetValue();

    // As long as no explicit list id attribute is set, use the list id of
    // the list, which has been created for the applied list style.
    if ( sListId.Len() == 0 )
    {
        SwNumRule* pRule = GetNumRule();
        if ( pRule )
        {
            sListId = pRule->GetDefaultListId();
//#if OSL_DEBUG_LEVEL > 1
//            ASSERT( false,
//                    "DEBUG ASSERTION: default list id of list style is applied." );
//#endif
//            // setting list id directly using <SwCntntNode::SetAttr(..)>,
//            // because no handling of this attribute set is needed and to avoid
//            // recursive calls of <SwTxtNode::SetAttr(..)>
//            SfxStringItem aNewListIdItem( RES_PARATR_LIST_ID, sListId );
//            const_cast<SwTxtNode*>(this)->SwCntntNode::SetAttr( aNewListIdItem );
        }
    }

    return sListId;
}
// <--

/** Determines, if the list level indent attributes can be applied to the
    paragraph.

    OD 2008-01-17 #newlistlevelattrs#
    The list level indents can be applied to the paragraph under the one
    of following conditions:
    - the list style is directly applied to the paragraph and the paragraph
      has no own indent attributes.
    - the list style is applied to the paragraph through one of its paragraph
      styles, the paragraph has no own indent attributes and on the paragraph
      style hierarchy from the paragraph to the paragraph style with the
      list style no indent attributes are found.

    @author OD

    @return boolean
*/
bool SwTxtNode::AreListLevelIndentsApplicable() const
{
    bool bAreListLevelIndentsApplicable( true );

    if ( !GetNum() || !GetNum()->GetNumRule() )
    {
        // no list style applied to paragraph
        bAreListLevelIndentsApplicable = false;
    }
    else if ( HasSwAttrSet() &&
              GetpSwAttrSet()->GetItemState( RES_LR_SPACE, FALSE ) == SFX_ITEM_SET )
    {
        // paragraph has hard-set indent attributes
        bAreListLevelIndentsApplicable = false;
    }
    else if ( HasSwAttrSet() &&
              GetpSwAttrSet()->GetItemState( RES_PARATR_NUMRULE, FALSE ) == SFX_ITEM_SET )
    {
        // list style is directly applied to paragraph and paragraph has no
        // hard-set indent attributes
        bAreListLevelIndentsApplicable = true;
    }
    else
    {
        // list style is applied through one of the paragraph styles and
        // paragraph has no hard-set indent attributes

        // check, paragraph's
        const SwTxtFmtColl* pColl = GetTxtColl();
        while ( pColl )
        {
            if ( pColl->GetAttrSet().GetItemState( RES_LR_SPACE, FALSE ) == SFX_ITEM_SET )
            {
                // indent attributes found in the paragraph style hierarchy.
                bAreListLevelIndentsApplicable = false;
                break;
            }

            if ( pColl->GetAttrSet().GetItemState( RES_PARATR_NUMRULE, FALSE ) == SFX_ITEM_SET )
            {
                // paragraph style with the list style found and until now no
                // indent attributes are found in the paragraph style hierarchy.
                bAreListLevelIndentsApplicable = true;
                break;
            }

            pColl = dynamic_cast<const SwTxtFmtColl*>(pColl->DerivedFrom());
            ASSERT( pColl,
                    "<SwTxtNode::AreListLevelIndentsApplicable()> - something wrong in paragraph's style hierarchy. The applied list style is not found." );
        }
    }

    return bAreListLevelIndentsApplicable;
}

/** Retrieves the list tab stop position, if the paragraph's list level defines
    one and this list tab stop has to merged into the tap stops of the paragraph

    OD 2008-01-17 #newlistlevelattrs#

    @author OD

    @param nListTabStopPosition
    output parameter - containing the list tab stop position

    @return boolean - indicating, if a list tab stop position is provided
*/
bool SwTxtNode::GetListTabStopPosition( long& nListTabStopPosition ) const
{
    bool bListTanStopPositionProvided( false );

    const SwNumRule* pNumRule = GetNum() ? GetNum()->GetNumRule() : 0;
    if ( pNumRule && HasVisibleNumberingOrBullet() && GetActualListLevel() >= 0 )
    {
        const SwNumFmt& rFmt = pNumRule->Get( static_cast<USHORT>(GetActualListLevel()) );
        if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT &&
             rFmt.GetLabelFollowedBy() == SvxNumberFormat::LISTTAB )
        {
            bListTanStopPositionProvided = true;
            nListTabStopPosition = rFmt.GetListtabPos();

            if ( getIDocumentSettingAccess()->get(IDocumentSettingAccess::TABS_RELATIVE_TO_INDENT) )
            {
                // tab stop position are treated to be relative to the "before text"
                // indent value of the paragraph. Thus, adjust <nListTabStopPos>.
                if ( AreListLevelIndentsApplicable() )
                {
                    nListTabStopPosition -= rFmt.GetIndentAt();
                }
                else if (!getIDocumentSettingAccess()->get(IDocumentSettingAccess::IGNORE_FIRST_LINE_INDENT_IN_NUMBERING))
                {
                    SvxLRSpaceItem aItem = GetSwAttrSet().GetLRSpace();
                    nListTabStopPosition -= aItem.GetTxtLeft();
                }
            }
        }
    }

    return bListTanStopPositionProvided;
}

/** Retrieves the character following the list label, if the paragraph's
    list level defines one.

    OD 2008-01-17 #newlistlevelattrs#

    @author OD

    @return XubString - the list tab stop position
*/
XubString SwTxtNode::GetLabelFollowedBy() const
{
    XubString aLabelFollowedBy;

    const SwNumRule* pNumRule = GetNum() ? GetNum()->GetNumRule() : 0;
    if ( pNumRule && HasVisibleNumberingOrBullet() && GetActualListLevel() >= 0 )
    {
        const SwNumFmt& rFmt = pNumRule->Get( static_cast<USHORT>(GetActualListLevel()) );
        if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
        {
            switch ( rFmt.GetLabelFollowedBy() )
            {
                case SvxNumberFormat::LISTTAB:
                {
                    const sal_Unicode aTab = '\t';
                    aLabelFollowedBy.Insert( aTab, 0 );
                }
                break;
                case SvxNumberFormat::SPACE:
                {
                    const sal_Unicode aSpace = ' ';
                    aLabelFollowedBy.Insert( aSpace, 0 );
                }
                break;
                case SvxNumberFormat::NOTHING:
                {
                    // intentionally left blank.
                }
                break;
                default:
                {
                    ASSERT( false,
                            "<SwTxtNode::GetLabelFollowedBy()> - unknown SvxNumberFormat::GetLabelFollowedBy() return value" );
                }
            }
        }
    }

    return aLabelFollowedBy;
}

void SwTxtNode::CalcHiddenCharFlags() const
{
    xub_StrLen nStartPos;
    xub_StrLen nEndPos;
    // Update of the flags is done inside GetBoundsOfHiddenRange()
    SwScriptInfo::GetBoundsOfHiddenRange( *this, 0, nStartPos, nEndPos );
}

// --> FME 2004-06-08 #i12836# enhanced pdf export
bool SwTxtNode::IsHidden() const
{
    if ( HasHiddenParaField() || HasHiddenCharAttribute( true ) )
        return true;

    const SwSectionNode* pSectNd = FindSectionNode();
    if ( pSectNd && pSectNd->GetSection().IsHiddenFlag() )
        return true;

    return false;
}
// <--

// --> OD 2008-03-13 #refactorlists#
namespace {
    // Helper class for special handling of setting attributes at text node:
    // In constructor an instance of the helper class recognize whose attributes
    // are set and perform corresponding actions before the intrinsic set of
    // attributes has been taken place.
    // In the destructor - after the attributes have been set at the text
    // node - corresponding actions are performed.
    // The following is handled:
    // (1) When the list style attribute - RES_PARATR_NUMRULE - is set,
    //     (A) list style attribute is empty -> the text node is removed from
    //         its list.
    //     (B) list style attribute is not empty
    //         (a) text node has no list style -> add text node to its list after
    //             the attributes have been set.
    //         (b) text node has list style -> change of list style is notified
    //             after the attributes have been set.
    // (2) When the list id attribute - RES_PARATR_LIST_ID - is set and changed,
    //     the text node is removed from its current list before the attributes
    //     are set and added to its new list after the attributes have been set.
    // (3) Notify list tree, if list level - RES_PARATR_LIST_LEVEL - is set
    //     and changed after the attributes have been set
    // (4) Notify list tree, if list restart - RES_PARATR_LIST_ISRESTART - is set
    //     and changed after the attributes have been set
    // (5) Notify list tree, if list restart value - RES_PARATR_LIST_RESTARTVALUE -
    //     is set and changed after the attributes have been set
    // (6) Notify list tree, if count in list - RES_PARATR_LIST_ISCOUNTED - is set
    //     and changed after the attributes have been set
    // (7) Set or Reset emtpy list style due to changed outline level - RES_PARATR_OUTLINELEVEL.
    class HandleSetAttrAtTxtNode
    {
        public:
            HandleSetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                    const SfxPoolItem& pItem );
            HandleSetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                    const SfxItemSet& rItemSet );
            ~HandleSetAttrAtTxtNode();

        private:
            SwTxtNode& mrTxtNode;
            bool mbAddTxtNodeToList;
            bool mbUpdateListLevel;
            bool mbUpdateListRestart;
            bool mbUpdateListCount;
            // --> OD 2008-11-19 #i70748#
            bool mbOutlineLevelSet;
            // <--
    };

    HandleSetAttrAtTxtNode::HandleSetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                                    const SfxPoolItem& pItem )
        : mrTxtNode( rTxtNode ),
          mbAddTxtNodeToList( false ),
          mbUpdateListLevel( false ),
          mbUpdateListRestart( false ),
          mbUpdateListCount( false ),
          // --> OD 2008-11-19 #i70748#
          mbOutlineLevelSet( false )
          // <--
    {
        switch ( pItem.Which() )
        {
            // handle RES_PARATR_NUMRULE
            case RES_PARATR_NUMRULE:
            {
                mrTxtNode.RemoveFromList();

                const SwNumRuleItem& pNumRuleItem =
                                dynamic_cast<const SwNumRuleItem&>(pItem);
                if ( pNumRuleItem.GetValue().Len() > 0 )
                {
                    mbAddTxtNodeToList = true;
                }
            }
            break;

            // handle RES_PARATR_LIST_ID
            case RES_PARATR_LIST_ID:
            {
                const SfxStringItem& pListIdItem =
                                        dynamic_cast<const SfxStringItem&>(pItem);
                ASSERT( pListIdItem.GetValue().Len() > 0,
                        "<HandleSetAttrAtTxtNode(..)> - empty list id attribute not excepted. Serious defect -> please inform OD." );
//                const SfxStringItem& rListIdItemOfTxtNode =
//                                    dynamic_cast<const SfxStringItem&>(
//                                        rTxtNode.GetAttr( RES_PARATR_LIST_ID ));
//                if ( pListIdItem.GetValue() != rListIdItemOfTxtNode.GetValue() )
                const String sListIdOfTxtNode = rTxtNode.GetListId();
                if ( pListIdItem.GetValue() != sListIdOfTxtNode )
                {
                    mbAddTxtNodeToList = true;
                    if ( mrTxtNode.IsInList() )
                    {
                        mrTxtNode.RemoveFromList();
                    }
                }
            }
            break;

            // handle RES_PARATR_LIST_LEVEL
            case RES_PARATR_LIST_LEVEL:
            {
                const SfxInt16Item& aListLevelItem =
                                    dynamic_cast<const SfxInt16Item&>(pItem);
                if ( aListLevelItem.GetValue() != mrTxtNode.GetAttrListLevel() )
                {
                    mbUpdateListLevel = true;
                }
            }
            break;

            // handle RES_PARATR_LIST_ISRESTART
            case RES_PARATR_LIST_ISRESTART:
            {
                const SfxBoolItem& aListIsRestartItem =
                                    dynamic_cast<const SfxBoolItem&>(pItem);
                if ( aListIsRestartItem.GetValue() !=
                                    (mrTxtNode.IsListRestart() ? TRUE : FALSE) )
                {
                    mbUpdateListRestart = true;
                }
            }
            break;

            // handle RES_PARATR_LIST_RESTARTVALUE
            case RES_PARATR_LIST_RESTARTVALUE:
            {
                const SfxInt16Item& aListRestartValueItem =
                                    dynamic_cast<const SfxInt16Item&>(pItem);
                if ( !mrTxtNode.HasAttrListRestartValue() ||
                     aListRestartValueItem.GetValue() != mrTxtNode.GetAttrListRestartValue() )
                {
                    mbUpdateListRestart = true;
                }
            }
            break;

            // handle RES_PARATR_LIST_ISCOUNTED
            case RES_PARATR_LIST_ISCOUNTED:
            {
                const SfxBoolItem& aIsCountedInListItem =
                                    dynamic_cast<const SfxBoolItem&>(pItem);
                if ( aIsCountedInListItem.GetValue() !=
                                    (mrTxtNode.IsCountedInList() ? TRUE : FALSE) )
                {
                    mbUpdateListCount = true;
                }
            }
            break;

            // --> OD 2008-11-19 #i70748#
            // handle RES_PARATR_OUTLINELEVEL
            case RES_PARATR_OUTLINELEVEL:
            {
                const SfxUInt16Item& aOutlineLevelItem =
                                    dynamic_cast<const SfxUInt16Item&>(pItem);
                if ( aOutlineLevelItem.GetValue() != mrTxtNode.GetAttrOutlineLevel() )
                {
                    mbOutlineLevelSet = true;
                }
            }
            break;
            // <--
        }

    }

    HandleSetAttrAtTxtNode::HandleSetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                                    const SfxItemSet& rItemSet )
        : mrTxtNode( rTxtNode ),
          mbAddTxtNodeToList( false ),
          mbUpdateListLevel( false ),
          mbUpdateListRestart( false ),
          mbUpdateListCount( false ),
          // --> OD 2008-11-19 #i70748#
          mbOutlineLevelSet( false )
          // <--
    {
        const SfxPoolItem* pItem = 0;
        // handle RES_PARATR_NUMRULE
        if ( rItemSet.GetItemState( RES_PARATR_NUMRULE, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            mrTxtNode.RemoveFromList();

            const SwNumRuleItem* pNumRuleItem =
                            dynamic_cast<const SwNumRuleItem*>(pItem);
            if ( pNumRuleItem->GetValue().Len() > 0 )
            {
                mbAddTxtNodeToList = true;
                // --> OD 2008-11-19 #i70748#
                mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
                // <--
            }
        }

        // handle RES_PARATR_LIST_ID
        if ( rItemSet.GetItemState( RES_PARATR_LIST_ID, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxStringItem* pListIdItem =
                                    dynamic_cast<const SfxStringItem*>(pItem);
//            const SfxStringItem& rListIdItemOfTxtNode =
//                                    dynamic_cast<const SfxStringItem&>(
//                                        mrTxtNode.GetAttr( RES_PARATR_LIST_ID ));
//            if ( pListIdItem &&
//                 pListIdItem->GetValue() != rListIdItemOfTxtNode.GetValue() )
            const String sListIdOfTxtNode = mrTxtNode.GetListId();
            if ( pListIdItem &&
                 pListIdItem->GetValue() != sListIdOfTxtNode )
            {
                mbAddTxtNodeToList = true;
                if ( mrTxtNode.IsInList() )
                {
                    mrTxtNode.RemoveFromList();
                }
            }
        }

        // handle RES_PARATR_LIST_LEVEL
        if ( rItemSet.GetItemState( RES_PARATR_LIST_LEVEL, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxInt16Item* pListLevelItem =
                                dynamic_cast<const SfxInt16Item*>(pItem);
            if ( pListLevelItem->GetValue() != mrTxtNode.GetAttrListLevel() )
            {
                mbUpdateListLevel = true;
            }
        }

        // handle RES_PARATR_LIST_ISRESTART
        if ( rItemSet.GetItemState( RES_PARATR_LIST_ISRESTART, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxBoolItem* pListIsRestartItem =
                                dynamic_cast<const SfxBoolItem*>(pItem);
            if ( pListIsRestartItem->GetValue() !=
                                    (mrTxtNode.IsListRestart() ? TRUE : FALSE) )
            {
                mbUpdateListRestart = true;
            }
        }

        // handle RES_PARATR_LIST_RESTARTVALUE
        if ( rItemSet.GetItemState( RES_PARATR_LIST_RESTARTVALUE, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxInt16Item* pListRestartValueItem =
                                dynamic_cast<const SfxInt16Item*>(pItem);
            if ( !mrTxtNode.HasAttrListRestartValue() ||
                 pListRestartValueItem->GetValue() != mrTxtNode.GetAttrListRestartValue() )
            {
                mbUpdateListRestart = true;
            }
        }

        // handle RES_PARATR_LIST_ISCOUNTED
        if ( rItemSet.GetItemState( RES_PARATR_LIST_ISCOUNTED, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxBoolItem* pIsCountedInListItem =
                                dynamic_cast<const SfxBoolItem*>(pItem);
            if ( pIsCountedInListItem->GetValue() !=
                                (mrTxtNode.IsCountedInList() ? TRUE : FALSE) )
            {
                mbUpdateListCount = true;
            }
        }

        // --> OD 2008-11-19 #i70748#
        // handle RES_PARATR_OUTLINELEVEL
        if ( rItemSet.GetItemState( RES_PARATR_OUTLINELEVEL, FALSE, &pItem ) == SFX_ITEM_SET )
        {
            const SfxUInt16Item* pOutlineLevelItem =
                                dynamic_cast<const SfxUInt16Item*>(pItem);
            if ( pOutlineLevelItem->GetValue() != mrTxtNode.GetAttrOutlineLevel() )
            {
                mbOutlineLevelSet = true;
            }
        }
        // <--
    }

    HandleSetAttrAtTxtNode::~HandleSetAttrAtTxtNode()
    {
        if ( mbAddTxtNodeToList )
        {
            SwNumRule* pNumRuleAtTxtNode = mrTxtNode.GetNumRule();
            if ( pNumRuleAtTxtNode )
            {
                mrTxtNode.AddToList();
            }
        }
        else
        {
            if ( mbUpdateListLevel && mrTxtNode.IsInList() )
            {
                const_cast<SwNodeNum*>(mrTxtNode.GetNum())->SetLevelInListTree(
                                                    mrTxtNode.GetAttrListLevel() );
            }

            if ( mbUpdateListRestart && mrTxtNode.IsInList() )
            {
                SwNodeNum* pNodeNum = const_cast<SwNodeNum*>(mrTxtNode.GetNum());
                pNodeNum->InvalidateMe();
                pNodeNum->NotifyInvalidSiblings();
            }

            if ( mbUpdateListCount && mrTxtNode.IsInList() )
            {
                const_cast<SwNodeNum*>(mrTxtNode.GetNum())->InvalidateAndNotifyTree();
            }
        }

        // --> OD 2008-11-19 #i70748#
        if ( mbOutlineLevelSet )
        {
            if ( mrTxtNode.GetAttrOutlineLevel() == 0 )
            {
                mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
            }
            else
            {
                const SfxPoolItem* pItem = 0;
                if ( mrTxtNode.GetSwAttrSet().GetItemState( RES_PARATR_NUMRULE,
                                                            TRUE, &pItem )
                                                                != SFX_ITEM_SET )
                {
                    mrTxtNode.SetEmptyListStyleDueToSetOutlineLevelAttr();
                }
            }
        }
        // <--
    }
    // End of class <HandleSetAttrAtTxtNode>
}

BOOL SwTxtNode::SetAttr( const SfxPoolItem& pItem )
{
    const bool bOldIsSetOrResetAttr( mbInSetOrResetAttr );
    mbInSetOrResetAttr = true;

    HandleSetAttrAtTxtNode aHandleSetAttr( *this, pItem );

    BOOL bRet = SwCntntNode::SetAttr( pItem );

    mbInSetOrResetAttr = bOldIsSetOrResetAttr;

    return bRet;
}

BOOL SwTxtNode::SetAttr( const SfxItemSet& rSet )
{
    const bool bOldIsSetOrResetAttr( mbInSetOrResetAttr );
    mbInSetOrResetAttr = true;

    HandleSetAttrAtTxtNode aHandleSetAttr( *this, rSet );

    BOOL bRet = SwCntntNode::SetAttr( rSet );

    mbInSetOrResetAttr = bOldIsSetOrResetAttr;

    return bRet;
}

namespace {
    // Helper class for special handling of resetting attributes at text node:
    // In constructor an instance of the helper class recognize whose attributes
    // are reset and perform corresponding actions before the intrinsic reset of
    // attributes has been taken place.
    // In the destructor - after the attributes have been reset at the text
    // node - corresponding actions are performed.
    // The following is handled:
    // (1) When the list style attribute - RES_PARATR_NUMRULE - is reset,
    //     the text is removed from its list before the attributes have been reset.
    // (2) When the list id attribute - RES_PARATR_LIST_ID - is reset,
    //     the text is removed from its list before the attributes have been reset.
    // (3) Notify list tree, if list level - RES_PARATR_LIST_LEVEL - is reset.
    // (4) Notify list tree, if list restart - RES_PARATR_LIST_ISRESTART - is reset.
    // (5) Notify list tree, if list restart value - RES_PARATR_LIST_RESTARTVALUE - is reset.
    // (6) Notify list tree, if count in list - RES_PARATR_LIST_ISCOUNTED - is reset.
    // (7) Reset empty list style, if outline level attribute - RES_PARATR_OUTLINELEVEL - is reset.
    class HandleResetAttrAtTxtNode
    {
        public:
            HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                      const USHORT nWhich1,
                                      const USHORT nWhich2 );
            HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                      const SvUShorts& rWhichArr );
            HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode );

            ~HandleResetAttrAtTxtNode();

        private:
            SwTxtNode& mrTxtNode;
            bool mbListStyleOrIdReset;
            bool mbUpdateListLevel;
            bool mbUpdateListRestart;
            bool mbUpdateListCount;
    };

    HandleResetAttrAtTxtNode::HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                                        const USHORT nWhich1,
                                                        const USHORT nWhich2 )
        : mrTxtNode( rTxtNode ),
          mbListStyleOrIdReset( false ),
          mbUpdateListLevel( false ),
          mbUpdateListRestart( false ),
          mbUpdateListCount( false )
    {
        bool bRemoveFromList( false );
        if ( nWhich2 != 0 && nWhich2 > nWhich1 )
        {
            // RES_PARATR_NUMRULE and RES_PARATR_LIST_ID
            if ( nWhich1 <= RES_PARATR_NUMRULE && RES_PARATR_NUMRULE <= nWhich2 )
            {
                bRemoveFromList = mrTxtNode.GetNumRule() != 0;
                mbListStyleOrIdReset = true;
            }
            else if ( nWhich1 <= RES_PARATR_LIST_ID && RES_PARATR_LIST_ID <= nWhich2 )
            {
                bRemoveFromList = mrTxtNode.GetpSwAttrSet() &&
                    mrTxtNode.GetpSwAttrSet()->GetItemState( RES_PARATR_LIST_ID, FALSE ) == SFX_ITEM_SET;
                // --> OD 2008-10-20 #i92898#
                mbListStyleOrIdReset = true;
                // <--
            }

            if ( !bRemoveFromList )
            {
                // RES_PARATR_LIST_LEVEL
                mbUpdateListLevel = ( nWhich1 <= RES_PARATR_LIST_LEVEL &&
                                      RES_PARATR_LIST_LEVEL <= nWhich2 &&
                                      mrTxtNode.HasAttrListLevel() );

                // RES_PARATR_LIST_ISRESTART and RES_PARATR_LIST_RESTARTVALUE
                mbUpdateListRestart =
                    ( nWhich1 <= RES_PARATR_LIST_ISRESTART && RES_PARATR_LIST_ISRESTART <= nWhich2 &&
                      mrTxtNode.IsListRestart() ) ||
                    ( nWhich1 <= RES_PARATR_LIST_RESTARTVALUE && RES_PARATR_LIST_RESTARTVALUE <= nWhich2 &&
                      mrTxtNode.HasAttrListRestartValue() );

                // RES_PARATR_LIST_ISCOUNTED
                mbUpdateListCount =
                    ( nWhich1 <= RES_PARATR_LIST_ISCOUNTED && RES_PARATR_LIST_ISCOUNTED <= nWhich2 &&
                      !mrTxtNode.IsCountedInList() );
            }

            // --> OD 2008-11-19 #i70748#
            // RES_PARATR_OUTLINELEVEL
            if ( nWhich1 <= RES_PARATR_OUTLINELEVEL && RES_PARATR_OUTLINELEVEL <= nWhich2 )
            {
                mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
            }
            // <--
        }
        else
        {
            // RES_PARATR_NUMRULE and RES_PARATR_LIST_ID
            if ( nWhich1 == RES_PARATR_NUMRULE )
            {
                bRemoveFromList = mrTxtNode.GetNumRule() != 0;
                mbListStyleOrIdReset = true;
            }
            else if ( nWhich1 == RES_PARATR_LIST_ID )
            {
                bRemoveFromList = mrTxtNode.GetpSwAttrSet() &&
                    mrTxtNode.GetpSwAttrSet()->GetItemState( RES_PARATR_LIST_ID, FALSE ) == SFX_ITEM_SET;
                // --> OD 2008-10-20 #i92898#
                mbListStyleOrIdReset = true;
                // <--
            }
            // --> OD 2008-11-19 #i70748#
            // RES_PARATR_OUTLINELEVEL
            else if ( nWhich1 == RES_PARATR_OUTLINELEVEL )
            {
                mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
            }
            // <--

            if ( !bRemoveFromList )
            {
                // RES_PARATR_LIST_LEVEL
                mbUpdateListLevel = nWhich1 == RES_PARATR_LIST_LEVEL &&
                                    mrTxtNode.HasAttrListLevel();

                // RES_PARATR_LIST_ISRESTART and RES_PARATR_LIST_RESTARTVALUE
                mbUpdateListRestart = ( nWhich1 == RES_PARATR_LIST_ISRESTART &&
                                        mrTxtNode.IsListRestart() ) ||
                                      ( nWhich1 == RES_PARATR_LIST_RESTARTVALUE &&
                                        mrTxtNode.HasAttrListRestartValue() );

                // RES_PARATR_LIST_ISCOUNTED
                mbUpdateListCount = nWhich1 == RES_PARATR_LIST_ISCOUNTED &&
                                    !mrTxtNode.IsCountedInList();
            }
        }

        if ( bRemoveFromList && mrTxtNode.IsInList() )
        {
            mrTxtNode.RemoveFromList();
        }
    }

    HandleResetAttrAtTxtNode::HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode,
                                                        const SvUShorts& rWhichArr )
        : mrTxtNode( rTxtNode ),
          mbListStyleOrIdReset( false ),
          mbUpdateListLevel( false ),
          mbUpdateListRestart( false ),
          mbUpdateListCount( false )
    {
        bool bRemoveFromList( false );
        {
            const USHORT nEnd = rWhichArr.Count();
            for ( USHORT n = 0; n < nEnd; ++n )
            {
                // RES_PARATR_NUMRULE and RES_PARATR_LIST_ID
                if ( rWhichArr[ n ] == RES_PARATR_NUMRULE )
                {
                    bRemoveFromList = bRemoveFromList ||
                                      mrTxtNode.GetNumRule() != 0;
                    mbListStyleOrIdReset = true;
                }
                else if ( rWhichArr[ n ] == RES_PARATR_LIST_ID )
                {
                    bRemoveFromList = bRemoveFromList ||
                        ( mrTxtNode.GetpSwAttrSet() &&
                          mrTxtNode.GetpSwAttrSet()->GetItemState( RES_PARATR_LIST_ID, FALSE ) == SFX_ITEM_SET );
                    // --> OD 2008-10-20 #i92898#
                    mbListStyleOrIdReset = true;
                    // <--
                }
                // --> OD 2008-11-19 #i70748#
                // RES_PARATR_OUTLINELEVEL
                else if ( rWhichArr[ n ] == RES_PARATR_OUTLINELEVEL )
                {
                    mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
                }
                // <--

                if ( !bRemoveFromList )
                {
                    // RES_PARATR_LIST_LEVEL
                    mbUpdateListLevel = mbUpdateListLevel ||
                                        ( rWhichArr[ n ] == RES_PARATR_LIST_LEVEL &&
                                          mrTxtNode.HasAttrListLevel() );

                    // RES_PARATR_LIST_ISRESTART and RES_PARATR_LIST_RESTARTVALUE
                    mbUpdateListRestart = mbUpdateListRestart ||
                                          ( rWhichArr[ n ] == RES_PARATR_LIST_ISRESTART &&
                                            mrTxtNode.IsListRestart() ) ||
                                          ( rWhichArr[ n ] == RES_PARATR_LIST_RESTARTVALUE &&
                                            mrTxtNode.HasAttrListRestartValue() );

                    // RES_PARATR_LIST_ISCOUNTED
                    mbUpdateListCount = mbUpdateListCount ||
                                        ( rWhichArr[ n ] == RES_PARATR_LIST_ISCOUNTED &&
                                          !mrTxtNode.IsCountedInList() );
                }
            }
        }

        if ( bRemoveFromList && mrTxtNode.IsInList() )
        {
            mrTxtNode.RemoveFromList();
        }
    }

    HandleResetAttrAtTxtNode::HandleResetAttrAtTxtNode( SwTxtNode& rTxtNode )
        : mrTxtNode( rTxtNode ),
          mbListStyleOrIdReset( false ),
          mbUpdateListLevel( false ),
          mbUpdateListRestart( false ),
          mbUpdateListCount( false )
    {
        mbListStyleOrIdReset = true;
        if ( rTxtNode.IsInList() )
        {
            rTxtNode.RemoveFromList();
        }
        // --> OD 2008-11-19 #i70748#
        mrTxtNode.ResetEmptyListStyleDueToResetOutlineLevelAttr();
        // <--
    }

    HandleResetAttrAtTxtNode::~HandleResetAttrAtTxtNode()
    {
        if ( mbListStyleOrIdReset && !mrTxtNode.IsInList() )
        {
            // check, if in spite of the reset of the list style or the list id
            // the paragraph still has to be added to a list.
            if ( mrTxtNode.GetNumRule() &&
                 mrTxtNode.GetListId().Len() > 0 )
            {
                // --> OD 2009-01-14 #i96062#
                // If paragraph has no list level attribute set and list style
                // is the outline style, apply outline level as the list level.
                if ( !mrTxtNode.HasAttrListLevel() &&
                     mrTxtNode.GetNumRule()->GetName() ==
                        String::CreateFromAscii( SwNumRule::GetOutlineRuleName() ) &&
                     mrTxtNode.GetTxtColl()->IsAssignedToListLevelOfOutlineStyle() )
                {
                    int nNewListLevel = mrTxtNode.GetTxtColl()->GetAssignedOutlineStyleLevel();
                    if ( 0 <= nNewListLevel && nNewListLevel < MAXLEVEL )
                    {
                        mrTxtNode.SetAttrListLevel( nNewListLevel );
                    }
                }
                // <--
                mrTxtNode.AddToList();
            }
            // --> OD 2008-11-19 #i70748#
            else if ( dynamic_cast<const SfxUInt16Item &>(mrTxtNode.GetAttr( RES_PARATR_OUTLINELEVEL, FALSE )).GetValue() > 0 )
            {
                mrTxtNode.SetEmptyListStyleDueToSetOutlineLevelAttr();
            }
            // <--
        }

        if ( mrTxtNode.IsInList() )
        {
            if ( mbUpdateListLevel )
            {
                SwNodeNum* pNodeNum = const_cast<SwNodeNum*>(mrTxtNode.GetNum());
                pNodeNum->SetLevelInListTree( mrTxtNode.GetAttrListLevel() );
            }

            if ( mbUpdateListRestart )
            {
                SwNodeNum* pNodeNum = const_cast<SwNodeNum*>(mrTxtNode.GetNum());
                pNodeNum->InvalidateMe();
                pNodeNum->NotifyInvalidSiblings();
            }

            if ( mbUpdateListCount )
            {
                SwNodeNum* pNodeNum = const_cast<SwNodeNum*>(mrTxtNode.GetNum());
                pNodeNum->InvalidateAndNotifyTree();
            }
        }
    }
    // End of class <HandleResetAttrAtTxtNode>
}

BOOL SwTxtNode::ResetAttr( USHORT nWhich1, USHORT nWhich2 )
{
    const bool bOldIsSetOrResetAttr( mbInSetOrResetAttr );
    mbInSetOrResetAttr = true;

    HandleResetAttrAtTxtNode aHandleResetAttr( *this, nWhich1, nWhich2 );

    BOOL bRet = SwCntntNode::ResetAttr( nWhich1, nWhich2 );

    mbInSetOrResetAttr = bOldIsSetOrResetAttr;

    return bRet;
}

BOOL SwTxtNode::ResetAttr( const SvUShorts& rWhichArr )
{
    const bool bOldIsSetOrResetAttr( mbInSetOrResetAttr );
    mbInSetOrResetAttr = true;

    HandleResetAttrAtTxtNode aHandleResetAttr( *this, rWhichArr );

    BOOL bRet = SwCntntNode::ResetAttr( rWhichArr );

    mbInSetOrResetAttr = bOldIsSetOrResetAttr;

    return bRet;
}

USHORT SwTxtNode::ResetAllAttr()
{
    const bool bOldIsSetOrResetAttr( mbInSetOrResetAttr );
    mbInSetOrResetAttr = true;

    HandleResetAttrAtTxtNode aHandleResetAttr( *this );

    USHORT nRet = SwCntntNode::ResetAllAttr();

    mbInSetOrResetAttr = bOldIsSetOrResetAttr;

    return nRet;
}
// <--


// sw::Metadatable
::sfx2::IXmlIdRegistry& SwTxtNode::GetRegistry()
{
    return GetDoc()->GetXmlIdRegistry();
}

bool SwTxtNode::IsInClipboard() const
{
    return GetDoc()->IsClipBoard();
}

bool SwTxtNode::IsInUndo() const
{
    return &GetNodes() == GetDoc()->GetUndoNds();
}

bool SwTxtNode::IsInContent() const
{
    return !GetDoc()->IsInHeaderFooter( SwNodeIndex(*this) );
}

#include <unoobj.hxx>

::com::sun::star::uno::Reference< ::com::sun::star::rdf::XMetadatable >
SwTxtNode::MakeUnoObject()
{
    // re-use existing SwXParagraph
    SwClientIter iter( *this );
    SwClient * pClient( iter.First( TYPE( SwXParagraph ) ) );
    while (pClient) {
        SwXParagraph *pPara( dynamic_cast<SwXParagraph*>(pClient) );
        if (pPara && pPara->GetCoreObject() == this ) {
            return pPara;
        }
        pClient = iter.Next();
    }

    // create new SwXParagraph
    SwPosition Pos( *this );
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > xParent(
        SwXTextRange::CreateParentXText( GetDoc(), Pos  ) );
    SwXParagraph * pXPara( new SwXParagraph( xParent, this ) );
    return pXPara;
}

