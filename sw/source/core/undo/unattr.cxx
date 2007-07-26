/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unattr.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-26 08:20:06 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVDMODEL_HXX //autogen
#include <svx/svdmodel.hxx>
#endif
#ifndef _SVX_TSTPITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif
#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif


#ifndef _FMTFLCNT_HXX //autogen
#include <fmtflcnt.hxx>
#endif
#ifndef _TXTFTN_HXX //autogen
#include <txtftn.hxx>
#endif
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _FMTCNTNT_HXX //autogen
#include <fmtcntnt.hxx>
#endif
#ifndef _FTNIDX_HXX
#include <ftnidx.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer die UndoIds
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _ROLBCK_HXX
#include <rolbck.hxx>
#endif
#ifndef _NDNOTXT_HXX
#include <ndnotxt.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _FTNINFO_HXX
#include <ftninfo.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif

inline SwDoc& SwUndoIter::GetDoc() const
{ return *pAktPam->GetDoc(); }

// -----------------------------------------------------

_UndoFmtAttr::_UndoFmtAttr( SwFmt& rFmt, BOOL bSvDrwPt )
    : SwClient( &rFmt ), pUndo( 0 ), bSaveDrawPt( bSvDrwPt )
{
}

void _UndoFmtAttr::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    if( pOld && pNew )
    {
        if( POOLATTR_END >= pOld->Which() )
        {
            if( pUndo )
                pUndo->PutAttr( *pOld );
            else
                pUndo = new SwUndoFmtAttr( *pOld, *(SwFmt*)pRegisteredIn,
                                            bSaveDrawPt );
        }
        else if( RES_ATTRSET_CHG == pOld->Which() )
        {
            if( pUndo )
            {
                SfxItemIter aIter( *((SwAttrSetChg*)pOld)->GetChgSet() );
                const SfxPoolItem* pItem = aIter.GetCurItem();
                while( pItem )
                {
                    pUndo->PutAttr( *pItem );
                    if( aIter.IsAtEnd() )
                        break;
                    pItem = aIter.NextItem();
                }
            }
            else
            {
                pUndo = new SwUndoFmtAttr( *((SwAttrSetChg*)pOld)->GetChgSet(),
                                           // --> OD 2007-07-11 #i56253#
                                           *((SwAttrSetChg*)pNew)->GetChgSet(),
                                           // <--
                                        *(SwFmt*)pRegisteredIn, bSaveDrawPt );
            }
        }
        else
            SwClient::Modify( pOld, pNew );
    }
    else
        SwClient::Modify( pOld, pNew );
}

// --> OD 2007-07-11 #i56253#
struct TxtNodeNumberingAttrs
{
    TxtNodeNumberingAttrs( ULONG nTxtNodeIdx,
                           int nNumLvl,
                           bool bIsRestart,
                           SwNodeNum::tSwNumTreeNumber nRestartVal,
                           bool bIsCounted )
        : mnTxtNodeIdx( nTxtNodeIdx ),
          mnNumLvl( nNumLvl ),
          mbIsRestart( bIsRestart ),
          mnRestartVal( nRestartVal ),
          mbIsCounted( bIsCounted )
    {}

    const ULONG mnTxtNodeIdx;
    const int mnNumLvl;
    const bool mbIsRestart;
    const SwNodeNum::tSwNumTreeNumber mnRestartVal;
    const bool mbIsCounted;
};
// <--

SwUndoFmtAttr::SwUndoFmtAttr( const SfxItemSet& rOldSet,
                              const SfxItemSet& rNewSet,
                              SwFmt& rChgFmt,
                              BOOL bSvDrwPt )
    : SwUndo( UNDO_INSFMTATTR ),
      pFmt( &rChgFmt ),
      // --> OD 2007-07-11 #i56253#
      pOldSet( 0 ),
      // <--
      nFmtWhich( rChgFmt.Which() ),
      nNode( 0 ),
      bSaveDrawPt( bSvDrwPt ),
      // --> OD 2007-07-11 #i56253#
      mpNumAttrsOfTxtNodes( 0 )
      // <--
{
    pOldSet = new SfxItemSet( rOldSet );
    // --> OD 2007-07-11 #i56253#
    Init( rNewSet );
    // <--
}

SwUndoFmtAttr::SwUndoFmtAttr( const SfxPoolItem& rItem, SwFmt& rChgFmt,
                                BOOL bSvDrwPt )
    : SwUndo( UNDO_INSFMTATTR ),
      pFmt( &rChgFmt ),
      nFmtWhich( rChgFmt.Which() ),
      nNode( 0 ),
      bSaveDrawPt( bSvDrwPt ),
      // --> OD 2007-07-11 #i56253#
      mpNumAttrsOfTxtNodes( 0 )
      // <--
{
    pOldSet = pFmt->GetAttrSet().Clone( FALSE );
    pOldSet->Put( rItem );
    // --> OD 2007-07-11 #i56253#
    Init( *pOldSet );
    // <--
}

void SwUndoFmtAttr::Init( const SfxItemSet& rAffectedItems )
{
    // Ankerwechsel gesondert behandeln
    if( SFX_ITEM_SET == pOldSet->GetItemState( RES_ANCHOR, FALSE ))
        SaveFlyAnchor( bSaveDrawPt );
    else if( RES_FRMFMT == nFmtWhich )
    {
        SwDoc* pDoc = pFmt->GetDoc();
        if( USHRT_MAX !=
            pDoc->GetTblFrmFmts()->GetPos( (const SwFrmFmtPtr)pFmt ))
        {
            // TabellenFormat -> Tabellen Index Position merken, TabellenFormate
            // 						sind fluechtig!
            SwClient* pTbl = SwClientIter( *pFmt ).First( TYPE( SwTable ));
            if( pTbl )
                nNode = ((SwTable*)pTbl)->GetTabSortBoxes()[ 0 ]->
                            GetSttNd()->FindTableNode()->GetIndex();
        }
        else if( USHRT_MAX !=
            pDoc->GetSections().GetPos( (const SwSectionFmtPtr)pFmt ))
            nNode = pFmt->GetCntnt().GetCntntIdx()->GetIndex();
    }

    // --> OD 2007-07-11 #i56253#
    if ( nFmtWhich == RES_TXTFMTCOLL )
    {
        mpNumAttrsOfTxtNodes = GetActualNumAttrsOfTxtNodes( &rAffectedItems );
    }
    // <--
}

// --> OD 2007-07-11 #i56253#
::std::vector<TxtNodeNumberingAttrs>* SwUndoFmtAttr::GetActualNumAttrsOfTxtNodes(
                                            const SfxItemSet* pAffectedItems )
{
    ::std::vector<TxtNodeNumberingAttrs>* pNumAttrsOfTxtNodes = 0;

    if ( !pAffectedItems ||
         SFX_ITEM_SET == pAffectedItems->GetItemState( RES_PARATR_NUMRULE, FALSE ) )
    {
        // Numbering rule attribute is affected by the action.
        // Thus, keep numbering attributes of all depending text nodes.
        pNumAttrsOfTxtNodes = new ::std::vector<TxtNodeNumberingAttrs>;

        SwClientIter aIter( *pFmt );
        for ( SwTxtNode* pTxtNode = static_cast<SwTxtNode*>(aIter.First( TYPE(SwTxtNode) ));
              pTxtNode;
              pTxtNode = static_cast<SwTxtNode*>(aIter.Next()) )
        {
            TxtNodeNumberingAttrs aNumAttrs( pTxtNode->GetIndex(),
                                             pTxtNode->GetLevel(),
                                             pTxtNode->IsRestart(),
                                             pTxtNode->GetStart(),
                                             pTxtNode->IsCounted() );
            pNumAttrsOfTxtNodes->push_back( aNumAttrs );
        }
    }

    return pNumAttrsOfTxtNodes;
}
// <--

SwUndoFmtAttr::~SwUndoFmtAttr()
{
    delete pOldSet;
    // --> OD 2007-07-11 #i56253#
    delete mpNumAttrsOfTxtNodes;
    // <--
}

void SwUndoFmtAttr::Undo( SwUndoIter& rUndoIter)
{
    // OD 2004-10-26 #i35443#
    // Important note: <Undo(..)> also called by <ReDo(..)>

    if( !pOldSet || !pFmt || !IsFmtInDoc( &rUndoIter.GetDoc() ))
        return;

    // --> OD 2004-10-26 #i35443# - If anchor attribute has been successfull
    // restored, all other attributes are also restored.
    // Thus, keep track of its restoration
    bool bAnchorAttrRestored( false );
    if ( SFX_ITEM_SET == pOldSet->GetItemState( RES_ANCHOR, FALSE ))
    {
        bAnchorAttrRestored = RestoreFlyAnchor( rUndoIter );
        if ( bAnchorAttrRestored )
        {
            // Anchor attribute successfull restored.
            // Thus, keep anchor position for redo
            SaveFlyAnchor();
        }
        else
        {
            // Anchor attribute not restored due to invalid anchor position.
            // Thus, delete anchor attribute.
            pOldSet->ClearItem( RES_ANCHOR );
        }
    }

    if ( !bAnchorAttrRestored )
    // <--
    {
        _UndoFmtAttr aTmp( *pFmt, bSaveDrawPt );
        pFmt->SetAttr( *pOldSet );
        if( aTmp.pUndo )
        {
            delete pOldSet;
            pOldSet = aTmp.pUndo->pOldSet;
            aTmp.pUndo->pOldSet = 0;    // den Pointer auf 0 setzen (nicht
                                        // doppelt loeschen) !!
            delete aTmp.pUndo;          // Undo-Object wieder loeschen
        }
        else
            pOldSet->ClearItem();

        if( RES_FLYFRMFMT == nFmtWhich || RES_DRAWFRMFMT == nFmtWhich )
            rUndoIter.pSelFmt = (SwFrmFmt*)pFmt;
    }

    // --> OD 2007-07-11 #i56253#
    if ( mpNumAttrsOfTxtNodes && !mpNumAttrsOfTxtNodes->empty() )
    {
        SwDoc* pDoc = pFmt->GetDoc();
        while ( !mpNumAttrsOfTxtNodes->empty() )
        {
            TxtNodeNumberingAttrs aNumAttrs = mpNumAttrsOfTxtNodes->back();
            mpNumAttrsOfTxtNodes->pop_back();
            SwTxtNode* pTxtNd = pDoc->GetNodes()[ aNumAttrs.mnTxtNodeIdx ]->GetTxtNode();
            if ( pTxtNd &&
                 aNumAttrs.mnNumLvl >= 0 && aNumAttrs.mnNumLvl < MAXLEVEL )
            {
                pTxtNd->SetLevel( aNumAttrs.mnNumLvl );
                pTxtNd->SetRestart( aNumAttrs.mbIsRestart );
                pTxtNd->SetStart( aNumAttrs.mnRestartVal );
                pTxtNd->SetCounted( aNumAttrs.mbIsCounted );
            }
        }
    }
    // <--
}

int SwUndoFmtAttr::IsFmtInDoc( SwDoc* pDoc )
{
    // suche im Dokument nach dem Format. Ist es nicht mehr vorhanden
    // so wird das Attribut nicht mehr gesetzt !
    USHORT nPos = USHRT_MAX;
    switch( nFmtWhich )
    {
    case RES_TXTFMTCOLL:
        nPos = pDoc->GetTxtFmtColls()->GetPos(
                                        (const SwTxtFmtCollPtr)pFmt );
        break;

    case RES_GRFFMTCOLL:
        nPos = pDoc->GetGrfFmtColls()->GetPos(
                                        (const SwGrfFmtCollPtr)pFmt );
        break;
    case RES_CHRFMT:
        nPos = pDoc->GetCharFmts()->GetPos(
                                    (const SwCharFmtPtr)pFmt );
        break;

    case RES_FRMFMT:
        if( nNode && nNode < pDoc->GetNodes().Count() )
        {
            SwNode* pNd = pDoc->GetNodes()[ nNode ];
            if( pNd->IsTableNode() )
            {
                pFmt = ((SwTableNode*)pNd)->GetTable().GetFrmFmt();
                nPos = 0;
                break;
            }
            else if( pNd->IsSectionNode() )
            {
                pFmt = ((SwSectionNode*)pNd)->GetSection().GetFmt();
                nPos = 0;
                break;
            }
        }
        // kein break!
    case RES_DRAWFRMFMT:
    case RES_FLYFRMFMT:
        if( USHRT_MAX == ( nPos = pDoc->GetSpzFrmFmts()->GetPos(
                            (const SwFrmFmtPtr)pFmt )) )
            nPos = pDoc->GetFrmFmts()->GetPos(
                            (const SwFrmFmtPtr)pFmt );
        break;
    }

    // Format nicht mehr vorhanden, zurueck
    if( USHRT_MAX == nPos )
        pFmt = 0;

    return 0 != pFmt;
}

// prueft, ob es noch im Doc ist!
SwFmt* SwUndoFmtAttr::GetFmt( SwDoc& rDoc )
{
    return pFmt && IsFmtInDoc( &rDoc ) ? pFmt : 0;
}

void SwUndoFmtAttr::Redo( SwUndoIter& rUndoIter)
{
    // --> OD 2007-07-11 #i56253#
    ::std::vector<TxtNodeNumberingAttrs>* pNumAttrsOfTxtNodes = 0;
    if ( mpNumAttrsOfTxtNodes != 0 )
    {
        pNumAttrsOfTxtNodes = GetActualNumAttrsOfTxtNodes();
    }

    // --> OD 2004-10-26 #i35443# - Because the undo stores the attributes for
    // redo, the same code as for <Undo(..)> can be applied for <Redo(..)>
    Undo( rUndoIter );
    // <--

    // --> OD 2007-07-11 #i56253#
    if ( pNumAttrsOfTxtNodes != 0 )
    {
        delete mpNumAttrsOfTxtNodes;
        mpNumAttrsOfTxtNodes = pNumAttrsOfTxtNodes;
    }
}

void SwUndoFmtAttr::Repeat( SwUndoIter& rUndoIter)
{
    if( !pOldSet )
        return;

    SwUndoFmtAttr* pLast;
    if( UNDO_INSFMTATTR == rUndoIter.GetLastUndoId() &&
        ( pLast = ((SwUndoFmtAttr*)rUndoIter.pLastUndoObj))->pOldSet &&
        pLast->pFmt )
        return;

    switch( nFmtWhich )
    {
    case RES_GRFFMTCOLL:
        {
            SwNoTxtNode * pNd = rUndoIter.pAktPam->GetNode()->GetNoTxtNode();
            if( pNd )
                rUndoIter.GetDoc().SetAttr( pFmt->GetAttrSet(),
                                            *pNd->GetFmtColl() );
        }
        break;

    case RES_TXTFMTCOLL:
        {
            SwTxtNode * pNd = rUndoIter.pAktPam->GetNode()->GetTxtNode();
            if( pNd )
                rUndoIter.GetDoc().SetAttr( pFmt->GetAttrSet(),
                                            *pNd->GetFmtColl() );
        }
        break;

//	case RES_CHRFMT:
//	case RES_FRMFMT:

    case RES_FLYFRMFMT:
        {
            // erstal pruefen, ob der Cursor ueberhaupt in einem fliegenden
            // Rahmen steht. Der Weg ist: suche in allen FlyFrmFormaten
            // nach dem FlyCntnt-Attribut und teste ob der Cursor in der
            // entsprechenden Section liegt.
            SwFrmFmt* pFly = rUndoIter.pAktPam->GetNode()->GetFlyFmt();
            if( pFly )
            {
                // Bug 43672: es duerfen nicht alle Attribute gesetzt werden!
                if( SFX_ITEM_SET == pFmt->GetAttrSet().GetItemState( RES_CNTNT ))
                {
                    SfxItemSet aTmpSet( pFmt->GetAttrSet() );
                    aTmpSet.ClearItem( RES_CNTNT );
                    if( aTmpSet.Count() )
                        rUndoIter.GetDoc().SetAttr( aTmpSet, *pFly );
                }
                else
                    rUndoIter.GetDoc().SetAttr( pFmt->GetAttrSet(), *pFly );
            }
            break;
        }
    }

    rUndoIter.pLastUndoObj = this;
}

SwRewriter SwUndoFmtAttr::GetRewriter() const
{
    SwRewriter aRewriter;

    if (pFmt)
        aRewriter.AddRule(UNDO_ARG1, pFmt->GetName());

    return aRewriter;
}

void SwUndoFmtAttr::PutAttr( const SfxPoolItem& rItem )
{
    pOldSet->Put( rItem );
    if( RES_ANCHOR == rItem.Which() )
        SaveFlyAnchor( bSaveDrawPt );
}

void SwUndoFmtAttr::SaveFlyAnchor( BOOL bSvDrwPt )
{
    // das Format ist gueltig, sonst wuerde man gar bis hier kommen
    if( bSvDrwPt )
    {
        if( RES_DRAWFRMFMT == pFmt->Which() )
        {
            Point aPt( ((SwFrmFmt*)pFmt)->FindSdrObject()->GetRelativePos() );
            // den alten Wert als zwischenspeichern. Attribut dafuer benutzen,
            // dadurch bleibt der SwUndoFmtAttr klein.
            pOldSet->Put( SwFmtFrmSize( ATT_VAR_SIZE, aPt.X(), aPt.Y() ) );
        }
/*		else
        {
            pOldSet->Put( pFmt->GetVertOrient() );
            pOldSet->Put( pFmt->GetHoriOrient() );
        }
*/	}

    const SwFmtAnchor& rAnchor = (SwFmtAnchor&)pOldSet->Get(
                                                RES_ANCHOR, FALSE );
    if( !rAnchor.GetCntntAnchor() )
        return;

    xub_StrLen nCntnt = 0;
    switch( rAnchor.GetAnchorId() )
    {
    case FLY_IN_CNTNT:
    case FLY_AUTO_CNTNT:
        nCntnt = rAnchor.GetCntntAnchor()->nContent.GetIndex();
    case FLY_AT_CNTNT:
    case FLY_AT_FLY:
        nNode = rAnchor.GetCntntAnchor()->nNode.GetIndex();
        break;
    default:
        return;
    }

    SwFmtAnchor aAnchor( rAnchor.GetAnchorId(), nCntnt );
    pOldSet->Put( aAnchor );
}

// --> OD 2004-10-26 #i35443# - Add return value, type <bool>.
// Return value indicates, if anchor attribute is restored.
// Note: If anchor attribute is restored, all other existing attributes
//       are also restored.
bool SwUndoFmtAttr::RestoreFlyAnchor( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SwFlyFrmFmt* pFrmFmt = (SwFlyFrmFmt*)pFmt;
    const SwFmtAnchor& rAnchor = (SwFmtAnchor&)pOldSet->Get(
                                                RES_ANCHOR, FALSE );

    SwFmtAnchor aNewAnchor( rAnchor.GetAnchorId() );
    if( FLY_PAGE != rAnchor.GetAnchorId() )
    {
        SwNode* pNd = pDoc->GetNodes()[ nNode  ];

        if( FLY_AT_FLY == rAnchor.GetAnchorId() ? ( !pNd->IsStartNode() ||
            SwFlyStartNode != ((SwStartNode*)pNd)->GetStartNodeType() ) :
            !pNd->IsTxtNode() )
        {
            // --> OD 2004-10-26 #i35443# - invalid position.
            // Thus, anchor attribute not restored
            return false;
            // <--
        }

        SwPosition aPos( *pNd );
        if( FLY_IN_CNTNT == rAnchor.GetAnchorId() ||
            FLY_AUTO_CNTNT == rAnchor.GetAnchorId() )
        {
            aPos.nContent.Assign( (SwTxtNode*)pNd, rAnchor.GetPageNum() );
            if( aPos.nContent.GetIndex() > ((SwTxtNode*)pNd)->GetTxt().Len() )
            {
                // --> OD 2004-10-26 #i35443# - invalid position.
                // Thus, anchor attribute not restored
                return false;
                // <--
            }
        }
        aNewAnchor.SetAnchor( &aPos );
    }
    else
        aNewAnchor.SetPageNum( rAnchor.GetPageNum() );

    Point aDrawSavePt, aDrawOldPt;
    if( pDoc->GetRootFrm() )
    {
        if( RES_DRAWFRMFMT == pFrmFmt->Which() )
        {
            // den alten zwischengespeicherten Wert herausholen.
            const SwFmtFrmSize& rOldSize = (const SwFmtFrmSize&)
                                            pOldSet->Get( RES_FRM_SIZE );
            aDrawSavePt.X() = rOldSize.GetWidth();
            aDrawSavePt.Y() = rOldSize.GetHeight();
            pOldSet->ClearItem( RES_FRM_SIZE );

            // den akt. wieder zwischenspeichern
            aDrawOldPt = pFrmFmt->FindSdrObject()->GetRelativePos();
//JP 08.10.97: ist laut AMA/MA nicht mehr noetig
//			pCont->DisconnectFromLayout();
        }
        else
            pFrmFmt->DelFrms(); 		// Frms vernichten.
    }

    const SwFmtAnchor &rOldAnch = pFrmFmt->GetAnchor();
    // --> OD 2006-03-13 #i54336#
    // Consider case, that as-character anchored object has moved its anchor position.
    if ( FLY_IN_CNTNT == rOldAnch.GetAnchorId() )
    // <--
    {
        //Bei InCntnt's wird es spannend: Das TxtAttribut muss vernichtet
        //werden. Leider reisst dies neben den Frms auch noch das Format mit
        //in sein Grab. Um dass zu unterbinden loesen wir vorher die
        //Verbindung zwischen Attribut und Format.
        const SwPosition *pPos = rOldAnch.GetCntntAnchor();
        SwTxtNode *pTxtNode = (SwTxtNode*)&pPos->nNode.GetNode();
        ASSERT( pTxtNode->HasHints(), "Missing FlyInCnt-Hint." );
        const xub_StrLen nIdx = pPos->nContent.GetIndex();
        SwTxtAttr * pHnt = pTxtNode->GetTxtAttr( nIdx, RES_TXTATR_FLYCNT );
#ifndef PRODUCT
        ASSERT( pHnt && pHnt->Which() == RES_TXTATR_FLYCNT,
                    "Missing FlyInCnt-Hint." );
        ASSERT( pHnt && pHnt->GetFlyCnt().GetFrmFmt() == pFrmFmt,
                    "Wrong TxtFlyCnt-Hint." );
#endif
        ((SwFmtFlyCnt&)pHnt->GetFlyCnt()).SetFlyFmt();

        //Die Verbindung ist geloest, jetzt muss noch das Attribut vernichtet
        //werden.
        pTxtNode->Delete( RES_TXTATR_FLYCNT, nIdx, nIdx );
    }

    {
        pOldSet->Put( aNewAnchor );
        _UndoFmtAttr aTmp( *pFmt, bSaveDrawPt );
        pFmt->SetAttr( *pOldSet );
        if( aTmp.pUndo )
        {
            delete pOldSet;
            nNode = aTmp.pUndo->nNode;
            pOldSet = aTmp.pUndo->pOldSet;
            aTmp.pUndo->pOldSet = 0;    // den Pointer auf 0 setzen (nicht
                                        // doppelt loeschen) !!
            delete aTmp.pUndo;          // Undo-Object wieder loeschen
        }
        else
            pOldSet->ClearItem();
    }

    if( RES_DRAWFRMFMT == pFrmFmt->Which() )
    {
        SwDrawContact *pCont = (SwDrawContact*)pFrmFmt->FindContactObj();
        // das Draw-Model hat auch noch ein Undo-Object fuer die
        // richtige Position vorbereitet; dieses ist aber relativ.
        // Darum verhinder hier, das durch setzen des Ankers das
        // Contact-Object seine Position aendert.
//JP 08.10.97: ist laut AMA/MA nicht mehr noetig
//			pCont->ConnectToLayout();
        SdrObject* pObj = pCont->GetMaster();

        if( pCont->GetAnchorFrm() && !pObj->IsInserted() )
        {
            ASSERT( pDoc->GetDrawModel(), "RestoreFlyAnchor without DrawModel" );
            pDoc->GetDrawModel()->GetPage( 0 )->InsertObject( pObj );
        }
        pObj->SetRelativePos( aDrawSavePt );

        // den alten Wert wieder zwischenspeichern.
        pOldSet->Put( SwFmtFrmSize( ATT_VAR_SIZE, aDrawOldPt.X(), aDrawOldPt.Y() ) );
    }

    if( FLY_IN_CNTNT == aNewAnchor.GetAnchorId() )
    {
        const SwPosition* pPos = aNewAnchor.GetCntntAnchor();
        SwTxtNode* pTxtNd = pPos->nNode.GetNode().GetTxtNode();
        ASSERT( pTxtNd, "Kein Textnode an dieser Position" );
        pTxtNd->InsertItem( SwFmtFlyCnt( pFrmFmt ), pPos->nContent.GetIndex(), 0 );
    }


    if( RES_DRAWFRMFMT != pFrmFmt->Which() )
        pFrmFmt->MakeFrms();

    rIter.pSelFmt = pFrmFmt;

    // --> OD 2004-10-26 #i35443# - anchor attribute restored.
    return true;
    // <--
}

/*  */

SwUndoRstAttr::SwUndoRstAttr( const SwPaM& rRange, USHORT nFmt )
    : SwUndo( UNDO_RESETATTR ), SwUndRng( rRange ), nFmtId( nFmt ),
    pHistory( new SwHistory )
{
}

SwUndoRstAttr::SwUndoRstAttr( const SwDoc& rDoc, const SwPosition& rPos,
                                USHORT nWhich )
    : SwUndo( UNDO_RESETATTR ), nFmtId( nWhich ), pHistory( new SwHistory )
{
    nSttNode = nEndNode = rPos.nNode.GetIndex();
    nSttCntnt = nEndCntnt = rPos.nContent.GetIndex();
}

SwUndoRstAttr::~SwUndoRstAttr()
{
    delete pHistory;
}

void SwUndoRstAttr::Undo( SwUndoIter& rUndoIter )
{
    // die alten Werte wieder zurueck
    SwDoc& rDoc = rUndoIter.GetDoc();
    pHistory->TmpRollback( &rDoc, 0 );
    pHistory->SetTmpEnd( pHistory->Count() );

    if( RES_CONDTXTFMTCOLL == nFmtId && nSttNode == nEndNode &&
        nSttCntnt == nEndCntnt )
    {
        SwTxtNode* pTNd = rDoc.GetNodes()[ nSttNode ]->GetTxtNode();
        if( pTNd )
        {
            SwIndex aIdx( pTNd, nSttCntnt );
            pTNd->DontExpandFmt( aIdx, FALSE );
        }
    }

    // setze noch den Cursor auf den Undo-Bereich
    SetPaM( rUndoIter );
}

void SwUndoRstAttr::Redo( SwUndoIter& rUndoIter )
{
    // setze Attribut in dem Bereich:
    SetPaM( rUndoIter );
    SwDoc& rDoc = rUndoIter.GetDoc();
    rUndoIter.pLastUndoObj = 0;
    SvUShortsSort* pIdArr = aIds.Count() ? &aIds : 0;

    switch( nFmtId )
    {
    case RES_CHRFMT:
        rUndoIter.GetDoc().RstTxtAttr( *rUndoIter.pAktPam );
        break;
    case RES_TXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttr( *rUndoIter.pAktPam, FALSE, pIdArr );
        break;
    case RES_CONDTXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttr( *rUndoIter.pAktPam, TRUE, pIdArr );

        break;
    case RES_TXTATR_TOXMARK:
        // Sonderbehandlung fuer TOXMarks
        {
            SwTOXMarks aArr;
            SwNodeIndex aIdx( rDoc.GetNodes(), nSttNode );
            SwPosition aPos( aIdx, SwIndex( aIdx.GetNode().GetCntntNode(),
                                                                nSttCntnt ));

            USHORT nCnt = rDoc.GetCurTOXMark( aPos, aArr );
            if( nCnt )
            {
                if( 1 < nCnt )
                {
                    // dann den richtigen suchen
                    SwHstryHint* pHHint = (*GetHistory())[ 0 ];
                    if( pHHint && HSTRY_SETTOXMARKHNT == pHHint->Which() )
                    {
                        while( nCnt )
                            if( ((SwSetTOXMarkHint*)pHHint)->IsEqual(
                                    *aArr[ --nCnt ] ) )
                            {
                                ++nCnt;
                                break;
                            }
                    }
                    else
                        nCnt = 0;
                }
                // gefunden, also loeschen
                if( nCnt-- )
                    rDoc.Delete( aArr[ nCnt ] );
            }
        }
        break;
    }
    rUndoIter.pLastUndoObj = 0;
}

void SwUndoRstAttr::Repeat( SwUndoIter& rUndoIter )
{
    if( RES_FMT_BEGIN > nFmtId ||
        ( UNDO_RESETATTR == rUndoIter.GetLastUndoId() &&
         nFmtId == ((SwUndoRstAttr*)rUndoIter.pLastUndoObj)->nFmtId ))
        return;

    SvUShortsSort* pIdArr = aIds.Count() ? &aIds : 0;
    switch( nFmtId )
    {
    case RES_CHRFMT:
        rUndoIter.GetDoc().RstTxtAttr( *rUndoIter.pAktPam );
        break;
    case RES_TXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttr( *rUndoIter.pAktPam, FALSE, pIdArr );
        break;
    case RES_CONDTXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttr( *rUndoIter.pAktPam, TRUE, pIdArr );
        break;
    }
    rUndoIter.pLastUndoObj = this;
}


void SwUndoRstAttr::SetAttrs( const SvUShortsSort& rArr )
{
    if( aIds.Count() )
        aIds.Remove( 0, aIds.Count() );
    aIds.Insert( &rArr );
}

// -----------------------------------------------------



SwUndoAttr::SwUndoAttr( const SwPaM& rRange, const SfxPoolItem& rAttr,
                        USHORT nFlags )
    : SwUndo( UNDO_INSATTR ), SwUndRng( rRange ),
    aSet( rRange.GetDoc()->GetAttrPool(), rAttr.Which(), rAttr.Which() ),
    nInsFlags( nFlags ), pHistory( new SwHistory ),
    pRedlData( 0 ), pRedlSaveData( 0 ),
    nNdIdx( ULONG_MAX )
{
    aSet.Put( rAttr );
}

SwUndoAttr::SwUndoAttr( const SwPaM& rRange, const SfxItemSet& rSet,
                        USHORT nFlags )
    : SwUndo( UNDO_INSATTR ), SwUndRng( rRange ), aSet( rSet ),
    nInsFlags( nFlags ), pHistory( new SwHistory ),
    pRedlData( 0 ), pRedlSaveData( 0 ),
    nNdIdx( ULONG_MAX )
{
}

SwUndoAttr::~SwUndoAttr()
{
    delete pHistory;
    delete pRedlData;
    delete pRedlSaveData;
}

void SwUndoAttr::SaveRedlineData( const SwPaM& rPam, BOOL bIsCntnt )
{
    SwDoc* pDoc = rPam.GetDoc();
    if( pDoc->IsRedlineOn() )
        pRedlData = new SwRedlineData( bIsCntnt ? IDocumentRedlineAccess::REDLINE_INSERT
                                                : IDocumentRedlineAccess::REDLINE_FORMAT,
                                        pDoc->GetRedlineAuthor() );

    pRedlSaveData = new SwRedlineSaveDatas;
    if( !FillSaveDataForFmt( rPam, *pRedlSaveData ))
        delete pRedlSaveData, pRedlSaveData = 0;

    SetRedlineMode( pDoc->GetRedlineMode() );
    if( bIsCntnt )
        nNdIdx = rPam.GetPoint()->nNode.GetIndex();
}

void SwUndoAttr::Undo( SwUndoIter& rUndoIter )
{
    SwDoc* pDoc = &rUndoIter.GetDoc();

    RemoveIdx( *pDoc );

    if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
    {
        SwPaM& rPam = *rUndoIter.pAktPam;
        if( ULONG_MAX != nNdIdx )
        {
            rPam.DeleteMark();
            rPam.GetPoint()->nNode = nNdIdx;
            rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), nSttCntnt );
            rPam.SetMark();
            rPam.GetPoint()->nContent++;
            pDoc->DeleteRedline( rPam, false, USHRT_MAX );
        }
        else
        {
            // alle Format-Redlines entfernen, werden ggfs. neu gesetzt
            SetPaM( rUndoIter );
            pDoc->DeleteRedline( rPam, false, IDocumentRedlineAccess::REDLINE_FORMAT );
            if( pRedlSaveData )
                SetSaveData( *pDoc, *pRedlSaveData );
        }
    }

    BOOL bToLast = 1 == aSet.Count() &&
                   RES_TXTATR_FIELD	<= *aSet.GetRanges() &&
                   *aSet.GetRanges() <= RES_TXTATR_HARDBLANK;

    // die alten Werte wieder zurueck
    pHistory->TmpRollback( pDoc, 0, !bToLast );
    pHistory->SetTmpEnd( pHistory->Count() );

    // setze noch den Cursor auf den Undo-Bereich
    SetPaM( rUndoIter );
}

int lcl_HasEqualItems( const SfxItemSet& rSet1, const SfxItemSet& rSet2 )
{
    int nRet = -1;
    SfxItemIter aIter1( rSet1 ), aIter2( rSet2 );
    const SfxPoolItem *pI1 = aIter1.FirstItem(), *pI2 = aIter2.FirstItem();

    while( pI1 && pI2 )
    {
        if( pI1->Which() != pI2->Which() ||
            aIter1.IsAtEnd() != aIter2.IsAtEnd() )
        {
            nRet = 0;
            break;
        }
        if( aIter1.IsAtEnd() )
            break;
        pI1 = aIter1.NextItem();
        pI2 = aIter2.NextItem();
    }
    return nRet;
}

void SwUndoAttr::Repeat( SwUndoIter& rUndoIter )
{
    SwUndoAttr* pLast;
    if( UNDO_INSATTR == rUndoIter.GetLastUndoId() &&
        ( pLast = ((SwUndoAttr*)rUndoIter.pLastUndoObj))->aSet.Count() ==
        aSet.Count() && pLast->nInsFlags == nInsFlags &&
        lcl_HasEqualItems( aSet, pLast->aSet ))
        return;


    // RefMarks sind nicht repeatfaehig
    if( SFX_ITEM_SET != aSet.GetItemState( RES_TXTATR_REFMARK, FALSE ) )
        rUndoIter.GetDoc().Insert( *rUndoIter.pAktPam, aSet, nInsFlags );
    else if( 1 < aSet.Count() )
    {
        SfxItemSet aTmpSet( aSet );
        aTmpSet.ClearItem( RES_TXTATR_REFMARK );
        rUndoIter.GetDoc().Insert( *rUndoIter.pAktPam, aTmpSet, nInsFlags );
    }
    rUndoIter.pLastUndoObj = this;
}

void SwUndoAttr::Redo( SwUndoIter& rUndoIter )
{
    // setze Attribut in dem Bereich:
    SetPaM( rUndoIter );
    SwPaM& rPam = *rUndoIter.pAktPam;
    SwDoc& rDoc = rUndoIter.GetDoc();

    if( pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
    {
        IDocumentRedlineAccess::RedlineMode_t eOld = rDoc.GetRedlineMode();
        rDoc.SetRedlineMode_intern((IDocumentRedlineAccess::RedlineMode_t)(eOld & ~IDocumentRedlineAccess::REDLINE_IGNORE));
        rDoc.Insert( rPam, aSet, nInsFlags );

        if( ULONG_MAX != nNdIdx )
        {
            rPam.SetMark();
            if( rPam.Move( fnMoveBackward ) )
                rDoc.AppendRedline( new SwRedline( *pRedlData, rPam ), true);
            rPam.DeleteMark();
        }
        else
            rDoc.AppendRedline( new SwRedline( *pRedlData, rPam ), true);

        rDoc.SetRedlineMode_intern( eOld );
    }
    else
        rDoc.Insert( rPam, aSet, nInsFlags );

    rUndoIter.pLastUndoObj = 0;
}


void SwUndoAttr::RemoveIdx( SwDoc& rDoc )
{
    if( SFX_ITEM_SET != aSet.GetItemState( RES_TXTATR_FTN, FALSE ))
        return ;

    SwHstryHint* pHstHnt;
    SwNodes& rNds = rDoc.GetNodes();
    for( USHORT n = 0; n < pHistory->Count(); ++n )
    {
        xub_StrLen nCntnt;
        ULONG nNode = 0;
        switch( ( pHstHnt = (*pHistory)[ n ] )->Which() )
        {
        case HSTRY_RESETTXTHNT:
            if( RES_TXTATR_FTN == ((SwResetTxtHint*)pHstHnt)->GetWhich() )
            {
                nNode = ((SwResetTxtHint*)pHstHnt)->GetNode();
                nCntnt = ((SwResetTxtHint*)pHstHnt)->GetCntnt();
            }
            break;
        case HSTRY_RESETATTRSET:
            if( STRING_MAXLEN != ( nCntnt =
                                ((SwHstryResetAttrSet*)pHstHnt)->GetCntnt() ))
            {
                const SvUShorts& rArr = ((SwHstryResetAttrSet*)pHstHnt)->GetArr();
                for( USHORT i = rArr.Count(); i; )
                    if( RES_TXTATR_FTN == rArr[ --i ] )
                    {
                        nNode = ((SwHstryResetAttrSet*)pHstHnt)->GetNode();
                        break;
                    }
            }
            break;
        }

        if( nNode )
        {
            SwTxtNode* pTxtNd = rNds[ nNode ]->GetTxtNode();
            if( pTxtNd )
            {
                SwIndex aIdx( pTxtNd, nCntnt );
                SwTxtAttr * pTxtHt = pTxtNd->GetTxtAttr( aIdx, RES_TXTATR_FTN );
                if( pTxtHt )
                {
                    // ok, dann hole mal die Werte
                    SwTxtFtn* pFtn = (SwTxtFtn*)pTxtHt;
                    RemoveIdxFromSection( rDoc, pFtn->GetStartNode()->GetIndex() );
                    return ;
                }
            }
        }
    }
}

/*  */

SwUndoDefaultAttr::SwUndoDefaultAttr( const SfxItemSet& rSet )
    : SwUndo( UNDO_SETDEFTATTR ), pOldSet( 0 ), pTabStop( 0 )
{
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rSet.GetItemState( RES_PARATR_TABSTOP, FALSE, &pItem ) )
    {
        pTabStop = (SvxTabStopItem*)pItem->Clone();		// gesondert merken, aendert sich !!!
        if( 1 != rSet.Count() )			// gibts noch mehr Attribute ?
            pOldSet = new SfxItemSet( rSet );
    }
    else
        pOldSet = new SfxItemSet( rSet );
}

SwUndoDefaultAttr::~SwUndoDefaultAttr()
{
    if( pOldSet )
        delete pOldSet;
    if( pTabStop )
        delete pTabStop;
}

void SwUndoDefaultAttr::Undo( SwUndoIter& rUndoIter)
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    if( pOldSet )
    {
        _UndoFmtAttr aTmp( *(SwTxtFmtColl*)rDoc.GetDfltTxtFmtColl() );
        rDoc.SetDefault( *pOldSet );
        delete pOldSet;
        if( aTmp.pUndo )
        {
            pOldSet = aTmp.pUndo->pOldSet;
            aTmp.pUndo->pOldSet = 0;    // den Pointer auf 0 setzen (nicht
                                        // doppelt loeschen) !!
            delete aTmp.pUndo;          // Undo-Object wieder loeschen
        }
        else
            pOldSet = 0;
    }
    if( pTabStop )
    {
        SvxTabStopItem* pOld = (SvxTabStopItem*)rDoc.GetDefault(
                                                RES_PARATR_TABSTOP ).Clone();
        rDoc.SetDefault( *pTabStop );
        delete pTabStop;
        pTabStop = pOld;
    }
}

void SwUndoDefaultAttr::Redo( SwUndoIter& rUndoIter)
{
    Undo( rUndoIter );
}

/*  */

SwUndoMoveLeftMargin::SwUndoMoveLeftMargin( const SwPaM& rPam, BOOL bFlag,
                                            BOOL bMod )
    : SwUndo( bFlag ? UNDO_INC_LEFTMARGIN : UNDO_DEC_LEFTMARGIN ),
    SwUndRng( rPam ), bModulus( bMod )
{
    pHistory = new SwHistory;
}

SwUndoMoveLeftMargin::~SwUndoMoveLeftMargin()
{
    delete pHistory;
}

void SwUndoMoveLeftMargin::Undo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    BOOL bUndo = pDoc->DoesUndo();
    pDoc->DoUndo( FALSE );

    // die alten Werte wieder zurueck
    pHistory->TmpRollback( pDoc, 0 );
    pHistory->SetTmpEnd( pHistory->Count() );

    pDoc->DoUndo( bUndo );
    SetPaM( rIter );
}

void SwUndoMoveLeftMargin::Redo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SetPaM( rIter );
    pDoc->MoveLeftMargin( *rIter.pAktPam, GetId() == UNDO_INC_LEFTMARGIN, bModulus );
}

void SwUndoMoveLeftMargin::Repeat( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    pDoc->MoveLeftMargin( *rIter.pAktPam, GetId() == UNDO_INC_LEFTMARGIN, bModulus );
    rIter.pLastUndoObj = this;
}

/*  */

SwUndoChgFtn::SwUndoChgFtn( const SwPaM& rRange, const String& rTxt,
                            USHORT nNum, BOOL bIsEndNote )
    : SwUndo( UNDO_CHGFTN ), SwUndRng( rRange ),
    sTxt( rTxt ), nNo( nNum ), bEndNote( bIsEndNote ),
    pHistory( new SwHistory() )
{
}

SwUndoChgFtn::~SwUndoChgFtn()
{
    delete pHistory;
}

void SwUndoChgFtn::Undo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    SetPaM( rIter );

    BOOL bUndo = rDoc.DoesUndo();
    rDoc.DoUndo( FALSE );

    pHistory->TmpRollback( &rDoc, 0 );
    pHistory->SetTmpEnd( pHistory->Count() );

    rDoc.GetFtnIdxs().UpdateAllFtn();

    SetPaM( rIter );
    rDoc.DoUndo( bUndo );
}

void SwUndoChgFtn::Redo( SwUndoIter& rIter )
{
    SetPaM( rIter );
    rIter.GetDoc().SetCurFtn( *rIter.pAktPam, sTxt, nNo, bEndNote );
    SetPaM( rIter );
}

void SwUndoChgFtn::Repeat( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    rDoc.SetCurFtn( *rIter.pAktPam, sTxt, nNo, bEndNote );
    rIter.pLastUndoObj = this;
}


/*  */


SwUndoFtnInfo::SwUndoFtnInfo( const SwFtnInfo &rInfo ) :
    SwUndo( UNDO_FTNINFO ),
    pFtnInfo( new SwFtnInfo( rInfo ) )
{
}

SwUndoFtnInfo::~SwUndoFtnInfo()
{
    delete pFtnInfo;
}

void SwUndoFtnInfo::Undo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwFtnInfo *pInf = new SwFtnInfo( rDoc.GetFtnInfo() );
    rDoc.SetFtnInfo( *pFtnInfo );
    delete pFtnInfo;
    pFtnInfo = pInf;
}

void SwUndoFtnInfo::Redo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwFtnInfo *pInf = new SwFtnInfo( rDoc.GetFtnInfo() );
    rDoc.SetFtnInfo( *pFtnInfo );
    delete pFtnInfo;
    pFtnInfo = pInf;
}

/*  */

SwUndoEndNoteInfo::SwUndoEndNoteInfo( const SwEndNoteInfo &rInfo ) :
    SwUndo( UNDO_FTNINFO ),
    pEndNoteInfo( new SwEndNoteInfo( rInfo ) )
{
}

SwUndoEndNoteInfo::~SwUndoEndNoteInfo()
{
    delete pEndNoteInfo;
}

void SwUndoEndNoteInfo::Undo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwEndNoteInfo *pInf = new SwEndNoteInfo( rDoc.GetEndNoteInfo() );
    rDoc.SetEndNoteInfo( *pEndNoteInfo );
    delete pEndNoteInfo;
    pEndNoteInfo = pInf;
}

void SwUndoEndNoteInfo::Redo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwEndNoteInfo *pInf = new SwEndNoteInfo( rDoc.GetEndNoteInfo() );
    rDoc.SetEndNoteInfo( *pEndNoteInfo );
    delete pEndNoteInfo;
    pEndNoteInfo = pInf;
}

/*  */

SwUndoDontExpandFmt::SwUndoDontExpandFmt( const SwPosition& rPos )
    : SwUndo( UNDO_DONTEXPAND ),
    nNode( rPos.nNode.GetIndex() ), nCntnt( rPos.nContent.GetIndex() )
{
}

void SwUndoDontExpandFmt::Undo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    SwPosition& rPos = *pPam->GetPoint();
    rPos.nNode = nNode;
    rPos.nContent.Assign( rPos.nNode.GetNode().GetCntntNode(), nCntnt );
    pDoc->DontExpandFmt( rPos, FALSE );
}


void SwUndoDontExpandFmt::Redo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    SwPosition& rPos = *pPam->GetPoint();
    rPos.nNode = nNode;
    rPos.nContent.Assign( rPos.nNode.GetNode().GetCntntNode(), nCntnt );
    pDoc->DontExpandFmt( rPos );
}

void SwUndoDontExpandFmt::Repeat( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();
    pDoc->DontExpandFmt( *pPam->GetPoint() );
}



