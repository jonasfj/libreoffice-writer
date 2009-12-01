/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unattr.cxx,v $
 *
 * $Revision: 1.21 $
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


#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <hintids.hxx>
#include <svx/svdmodel.hxx>
#include <svx/tstpitem.hxx>
#include <svx/svdpage.hxx>
#include <svtools/itemiter.hxx>


#include <fmtflcnt.hxx>
#include <txtftn.hxx>
#include <fmtornt.hxx>
#include <fmtanchr.hxx>
#include <fmtfsize.hxx>
#include <frmfmt.hxx>
#include <fmtcntnt.hxx>
#include <ftnidx.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <pam.hxx>
#include <ndtxt.hxx>
#include <swtable.hxx>
#include <swtblfmt.hxx>
#include <undobj.hxx>
#include <rolbck.hxx>
#include <ndnotxt.hxx>
#include <dcontact.hxx>
#include <ftninfo.hxx>
#include <redline.hxx>
#include <section.hxx>
#include <charfmt.hxx>



inline SwDoc& SwUndoIter::GetDoc() const
{ return *pAktPam->GetDoc(); }

// -----------------------------------------------------

SwUndoFmtAttrHelper::SwUndoFmtAttrHelper( SwFmt& rFmt, bool bSvDrwPt )
    : SwClient( &rFmt )
    , m_pUndo( 0 )
    , m_bSaveDrawPt( bSvDrwPt )
{
}

void SwUndoFmtAttrHelper::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    if( pOld && pNew )
    {
        if( POOLATTR_END >= pOld->Which() )
        {
            if ( GetUndo() )
            {
                m_pUndo->PutAttr( *pOld );
            }
            else
            {
                m_pUndo.reset( new SwUndoFmtAttr( *pOld,
                        *static_cast<SwFmt*>(pRegisteredIn), m_bSaveDrawPt ) );
            }
        }
        else if ( RES_ATTRSET_CHG == pOld->Which() )
        {
            if ( GetUndo() )
            {
                SfxItemIter aIter(
                        *(static_cast<SwAttrSetChg*>(pOld))->GetChgSet() );
                const SfxPoolItem* pItem = aIter.GetCurItem();
                while ( pItem )
                {
                    m_pUndo->PutAttr( *pItem );
                    if( aIter.IsAtEnd() )
                        break;
                    pItem = aIter.NextItem();
                }
            }
            else
            {
                m_pUndo.reset( new SwUndoFmtAttr(
                        *static_cast<SwAttrSetChg*>(pOld)->GetChgSet(),
                        *static_cast<SwFmt*>(pRegisteredIn), m_bSaveDrawPt ) );
            }
        }
        else
            SwClient::Modify( pOld, pNew );
    }
    else
        SwClient::Modify( pOld, pNew );
}

// -----------------------------------------------------

SwUndoFmtAttr::SwUndoFmtAttr( const SfxItemSet& rOldSet,
                              SwFmt& rChgFmt,
                              bool bSaveDrawPt )
    : SwUndo( UNDO_INSFMTATTR )
    , m_pFmt( &rChgFmt )
      // --> OD 2007-07-11 #i56253#
    , m_pOldSet( new SfxItemSet( rOldSet ) )
      // <--
    , m_nNodeIndex( 0 )
    , m_nFmtWhich( rChgFmt.Which() )
    , m_bSaveDrawPt( bSaveDrawPt )
{
    Init();
}

SwUndoFmtAttr::SwUndoFmtAttr( const SfxPoolItem& rItem, SwFmt& rChgFmt,
                              bool bSaveDrawPt )
    : SwUndo( UNDO_INSFMTATTR )
    , m_pFmt( &rChgFmt )
    , m_pOldSet( m_pFmt->GetAttrSet().Clone( FALSE ) )
    , m_nNodeIndex( 0 )
    , m_nFmtWhich( rChgFmt.Which() )
    , m_bSaveDrawPt( bSaveDrawPt )
{
    m_pOldSet->Put( rItem );
    Init();
}

void SwUndoFmtAttr::Init()
{
    // treat change of anchor specially
    if ( SFX_ITEM_SET == m_pOldSet->GetItemState( RES_ANCHOR, FALSE ))
    {
        SaveFlyAnchor( m_bSaveDrawPt );
    }
    else if ( RES_FRMFMT == m_nFmtWhich )
    {
        SwDoc* pDoc = m_pFmt->GetDoc();
        if (USHRT_MAX != pDoc->GetTblFrmFmts()->GetPos(
                            static_cast<const SwFrmFmtPtr>(m_pFmt)))
        {
            // Table Format: save table position, table formats are volatile!
            SwTable * pTbl = static_cast<SwTable*>(
                    SwClientIter( *m_pFmt ).First( TYPE( SwTable )) );
            if ( pTbl )
            {
                m_nNodeIndex = pTbl->GetTabSortBoxes()[ 0 ]->GetSttNd()
                    ->FindTableNode()->GetIndex();
            }
        }
        else if (USHRT_MAX != pDoc->GetSections().GetPos(
                                static_cast<const SwSectionFmtPtr>(m_pFmt)))
        {
            m_nNodeIndex = m_pFmt->GetCntnt().GetCntntIdx()->GetIndex();
        }
        else if ( 0 != dynamic_cast< SwTableBoxFmt* >( m_pFmt ) )
        {
            SwTableBox* pTblBox = static_cast< SwTableBox* >(
                    SwClientIter( *m_pFmt ).First( TYPE( SwTableBox )));
            if ( pTblBox )
            {
                m_nNodeIndex = pTblBox->GetSttIdx();
            }
        }
    }
}

SwUndoFmtAttr::~SwUndoFmtAttr()
{
}

void SwUndoFmtAttr::Undo( SwUndoIter& rUndoIter)
{
    // OD 2004-10-26 #i35443#
    // Important note: <Undo(..)> also called by <ReDo(..)>

    if ( !m_pOldSet.get() || !m_pFmt || !IsFmtInDoc( &rUndoIter.GetDoc() ))
        return;

    // --> OD 2004-10-26 #i35443# - If anchor attribute has been successfull
    // restored, all other attributes are also restored.
    // Thus, keep track of its restoration
    bool bAnchorAttrRestored( false );
    if ( SFX_ITEM_SET == m_pOldSet->GetItemState( RES_ANCHOR, FALSE ))
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
            m_pOldSet->ClearItem( RES_ANCHOR );
        }
    }

    if ( !bAnchorAttrRestored )
    // <--
    {
        SwUndoFmtAttrHelper aTmp( *m_pFmt, m_bSaveDrawPt );
        m_pFmt->SetFmtAttr( *m_pOldSet );
        if ( aTmp.GetUndo() )
        {
            // transfer ownership of helper object's old set
            m_pOldSet = aTmp.GetUndo()->m_pOldSet;
        }
        else
        {
            m_pOldSet->ClearItem();
        }

        if ( RES_FLYFRMFMT == m_nFmtWhich || RES_DRAWFRMFMT == m_nFmtWhich )
        {
            rUndoIter.pSelFmt = static_cast<SwFrmFmt*>(m_pFmt);
        }
    }
}

bool SwUndoFmtAttr::IsFmtInDoc( SwDoc* pDoc )
{
    // search for the Format in the Document; if it does not exist any more,
    // the attribute is not restored!
    USHORT nPos = USHRT_MAX;
    switch ( m_nFmtWhich )
    {
        case RES_TXTFMTCOLL:
            nPos = pDoc->GetTxtFmtColls()->GetPos(
                    static_cast<const SwTxtFmtCollPtr>(m_pFmt) );
            break;

        case RES_GRFFMTCOLL:
            nPos = pDoc->GetGrfFmtColls()->GetPos(
                    static_cast<const SwGrfFmtCollPtr>(m_pFmt) );
            break;

        case RES_CHRFMT:
            nPos = pDoc->GetCharFmts()->GetPos(
                    static_cast<SwCharFmtPtr>(m_pFmt) );
            break;

        case RES_FRMFMT:
            if ( m_nNodeIndex && (m_nNodeIndex < pDoc->GetNodes().Count()) )
            {
                SwNode* pNd = pDoc->GetNodes()[ m_nNodeIndex ];
                if ( pNd->IsTableNode() )
                {
                    m_pFmt =
                        static_cast<SwTableNode*>(pNd)->GetTable().GetFrmFmt();
                    nPos = 0;
                    break;
                }
                else if ( pNd->IsSectionNode() )
                {
                    m_pFmt =
                        static_cast<SwSectionNode*>(pNd)->GetSection().GetFmt();
                    nPos = 0;
                    break;
                }
                else if ( pNd->IsStartNode() && (SwTableBoxStartNode ==
                    static_cast< SwStartNode* >(pNd)->GetStartNodeType()) )
                {
                    SwTableNode* pTblNode = pNd->FindTableNode();
                    if ( pTblNode )
                    {
                        SwTableBox* pBox =
                            pTblNode->GetTable().GetTblBox( m_nNodeIndex );
                        if ( pBox )
                        {
                            m_pFmt = pBox->GetFrmFmt();
                            nPos = 0;
                            break;
                        }
                    }
                }
            }
            // no break!
        case RES_DRAWFRMFMT:
        case RES_FLYFRMFMT:
            nPos = pDoc->GetSpzFrmFmts()->GetPos(
                    static_cast<const SwFrmFmtPtr>(m_pFmt) );
            if ( USHRT_MAX == nPos )
            {
                nPos = pDoc->GetFrmFmts()->GetPos(
                    static_cast<const SwFrmFmtPtr>(m_pFmt) );
            }
            break;
    }

    if ( USHRT_MAX == nPos )
    {
        // Format does not exist; reset
        m_pFmt = 0;
    }

    return 0 != m_pFmt;
}

// prueft, ob es noch im Doc ist!
SwFmt* SwUndoFmtAttr::GetFmt( SwDoc& rDoc )
{
    return m_pFmt && IsFmtInDoc( &rDoc ) ? m_pFmt : 0;
}

void SwUndoFmtAttr::Redo( SwUndoIter& rUndoIter)
{
    // --> OD 2004-10-26 #i35443# - Because the undo stores the attributes for
    // redo, the same code as for <Undo(..)> can be applied for <Redo(..)>
    Undo( rUndoIter );
    // <--
}

void SwUndoFmtAttr::Repeat( SwUndoIter& rUndoIter)
{
    if ( !m_pOldSet.get() )
        return;

    if ( UNDO_INSFMTATTR == rUndoIter.GetLastUndoId())
    {
        SwUndoFmtAttr* pLast
            = static_cast<SwUndoFmtAttr*>(rUndoIter.pLastUndoObj);
        if (pLast->m_pOldSet.get() && pLast->m_pFmt)
        {
            return;
        }
    }

    switch ( m_nFmtWhich )
    {
    case RES_GRFFMTCOLL:
        {
            SwNoTxtNode * pNd = rUndoIter.pAktPam->GetNode()->GetNoTxtNode();
            if( pNd )
            {
                rUndoIter.GetDoc().SetAttr( m_pFmt->GetAttrSet(),
                                            *pNd->GetFmtColl() );
            }
        }
        break;

    case RES_TXTFMTCOLL:
        {
            SwTxtNode * pNd = rUndoIter.pAktPam->GetNode()->GetTxtNode();
            if( pNd )
            {
                rUndoIter.GetDoc().SetAttr( m_pFmt->GetAttrSet(),
                                            *pNd->GetFmtColl() );
            }
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
                if (SFX_ITEM_SET ==
                        m_pFmt->GetAttrSet().GetItemState( RES_CNTNT ))
                {
                    SfxItemSet aTmpSet( m_pFmt->GetAttrSet() );
                    aTmpSet.ClearItem( RES_CNTNT );
                    if( aTmpSet.Count() )
                        rUndoIter.GetDoc().SetAttr( aTmpSet, *pFly );
                }
                else
                {
                    rUndoIter.GetDoc().SetAttr( m_pFmt->GetAttrSet(), *pFly );
                }
            }
            break;
        }
    }

    rUndoIter.pLastUndoObj = this;
}

SwRewriter SwUndoFmtAttr::GetRewriter() const
{
    SwRewriter aRewriter;

    if (m_pFmt)
    {
        aRewriter.AddRule(UNDO_ARG1, m_pFmt->GetName());
    }

    return aRewriter;
}

void SwUndoFmtAttr::PutAttr( const SfxPoolItem& rItem )
{
    m_pOldSet->Put( rItem );
    if ( RES_ANCHOR == rItem.Which() )
    {
        SaveFlyAnchor( m_bSaveDrawPt );
    }
}

void SwUndoFmtAttr::SaveFlyAnchor( bool bSvDrwPt )
{
    // das Format ist gueltig, sonst wuerde man gar bis hier kommen
    if( bSvDrwPt )
    {
        if ( RES_DRAWFRMFMT == m_pFmt->Which() )
        {
            Point aPt( static_cast<SwFrmFmt*>(m_pFmt)->FindSdrObject()
                            ->GetRelativePos() );
            // store old value as attribute, to keep SwUndoFmtAttr small
            m_pOldSet->Put( SwFmtFrmSize( ATT_VAR_SIZE, aPt.X(), aPt.Y() ) );
        }
/*		else
        {
            pOldSet->Put( pFmt->GetVertOrient() );
            pOldSet->Put( pFmt->GetHoriOrient() );
        }
*/	}

    const SwFmtAnchor& rAnchor =
        static_cast<const SwFmtAnchor&>( m_pOldSet->Get( RES_ANCHOR, FALSE ) );
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
        m_nNodeIndex = rAnchor.GetCntntAnchor()->nNode.GetIndex();
        break;
    default:
        return;
    }

    SwFmtAnchor aAnchor( rAnchor.GetAnchorId(), nCntnt );
    m_pOldSet->Put( aAnchor );
}

// --> OD 2004-10-26 #i35443# - Add return value, type <bool>.
// Return value indicates, if anchor attribute is restored.
// Note: If anchor attribute is restored, all other existing attributes
//       are also restored.
bool SwUndoFmtAttr::RestoreFlyAnchor( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SwFlyFrmFmt* pFrmFmt = static_cast<SwFlyFrmFmt*>(m_pFmt);
    const SwFmtAnchor& rAnchor =
        static_cast<const SwFmtAnchor&>( m_pOldSet->Get( RES_ANCHOR, FALSE ) );

    SwFmtAnchor aNewAnchor( rAnchor.GetAnchorId() );
    if( FLY_PAGE != rAnchor.GetAnchorId() )
    {
        SwNode* pNd = pDoc->GetNodes()[ m_nNodeIndex  ];

        if (  (FLY_AT_FLY == rAnchor.GetAnchorId())
            ? ( !pNd->IsStartNode() || (SwFlyStartNode !=
                    static_cast<SwStartNode*>(pNd)->GetStartNodeType()) )
            : !pNd->IsTxtNode() )
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
            if ( aPos.nContent.GetIndex() >
                    static_cast<SwTxtNode*>(pNd)->GetTxt().Len() )
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
            const SwFmtFrmSize& rOldSize = static_cast<const SwFmtFrmSize&>(
                    m_pOldSet->Get( RES_FRM_SIZE ) );
            aDrawSavePt.X() = rOldSize.GetWidth();
            aDrawSavePt.Y() = rOldSize.GetHeight();
            m_pOldSet->ClearItem( RES_FRM_SIZE );

            // den akt. wieder zwischenspeichern
            aDrawOldPt = pFrmFmt->FindSdrObject()->GetRelativePos();
//JP 08.10.97: ist laut AMA/MA nicht mehr noetig
//			pCont->DisconnectFromLayout();
        }
        else
        {
            pFrmFmt->DelFrms();         // delete Frms
        }
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
        const_cast<SwFmtFlyCnt&>(pHnt->GetFlyCnt()).SetFlyFmt();

        //Die Verbindung ist geloest, jetzt muss noch das Attribut vernichtet
        //werden.
        pTxtNode->Delete( RES_TXTATR_FLYCNT, nIdx, nIdx );
    }

    {
        m_pOldSet->Put( aNewAnchor );
        SwUndoFmtAttrHelper aTmp( *m_pFmt, m_bSaveDrawPt );
        m_pFmt->SetFmtAttr( *m_pOldSet );
        if ( aTmp.GetUndo() )
        {
            m_nNodeIndex = aTmp.GetUndo()->m_nNodeIndex;
            // transfer ownership of helper object's old set
            m_pOldSet = aTmp.GetUndo()->m_pOldSet;
        }
        else
        {
            m_pOldSet->ClearItem();
        }
    }

    if ( RES_DRAWFRMFMT == pFrmFmt->Which() )
    {
        SwDrawContact *pCont =
            static_cast<SwDrawContact*>(pFrmFmt->FindContactObj());
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
        m_pOldSet->Put(
            SwFmtFrmSize( ATT_VAR_SIZE, aDrawOldPt.X(), aDrawOldPt.Y() ) );
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

// -----------------------------------------------------

// --> OD 2008-02-12 #newlistlevelattrs#
SwUndoFmtResetAttr::SwUndoFmtResetAttr( SwFmt& rChangedFormat,
                                        const USHORT nWhichId )
    : SwUndo( UNDO_RESETATTR )
    , m_pChangedFormat( &rChangedFormat )
    , m_nWhichId( nWhichId )
    , m_pOldItem( 0 )
{
    const SfxPoolItem* pItem = 0;
    if (rChangedFormat.GetItemState( nWhichId, FALSE, &pItem ) == SFX_ITEM_SET)
    {
        m_pOldItem.reset( pItem->Clone() );
    }
}

SwUndoFmtResetAttr::~SwUndoFmtResetAttr()
{
}

void SwUndoFmtResetAttr::Undo( SwUndoIter& )
{
    if ( m_pOldItem.get() )
    {
        m_pChangedFormat->SetFmtAttr( *m_pOldItem );
    }
}

void SwUndoFmtResetAttr::Redo( SwUndoIter& )
{
    if ( m_pOldItem.get() )
    {
        m_pChangedFormat->ResetFmtAttr( m_nWhichId );
    }
}
// <--

// -----------------------------------------------------

SwUndoResetAttr::SwUndoResetAttr( const SwPaM& rRange, USHORT nFmtId )
    : SwUndo( UNDO_RESETATTR ), SwUndRng( rRange )
    , m_pHistory( new SwHistory )
    , m_nFormatId( nFmtId )
{
}

SwUndoResetAttr::SwUndoResetAttr( const SwPosition& rPos, USHORT nFmtId )
    : SwUndo( UNDO_RESETATTR )
    , m_pHistory( new SwHistory )
    , m_nFormatId( nFmtId )
{
    nSttNode = nEndNode = rPos.nNode.GetIndex();
    nSttCntnt = nEndCntnt = rPos.nContent.GetIndex();
}

SwUndoResetAttr::~SwUndoResetAttr()
{
}

void SwUndoResetAttr::Undo( SwUndoIter& rUndoIter )
{
    // reset old values
    SwDoc& rDoc = rUndoIter.GetDoc();
    m_pHistory->TmpRollback( &rDoc, 0 );
    m_pHistory->SetTmpEnd( m_pHistory->Count() );

    if ((RES_CONDTXTFMTCOLL == m_nFormatId) &&
        (nSttNode == nEndNode) && (nSttCntnt == nEndCntnt))
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

void SwUndoResetAttr::Redo( SwUndoIter& rUndoIter )
{
    // setze Attribut in dem Bereich:
    SetPaM( rUndoIter );
    SwDoc& rDoc = rUndoIter.GetDoc();
    rUndoIter.pLastUndoObj = 0;
    SvUShortsSort* pIdArr = m_Ids.Count() ? &m_Ids : 0;

    switch ( m_nFormatId )
    {
    case RES_CHRFMT:
        rUndoIter.GetDoc().RstTxtAttrs( *rUndoIter.pAktPam );
        break;
    case RES_TXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttrs( *rUndoIter.pAktPam, FALSE, pIdArr );
        break;
    case RES_CONDTXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttrs( *rUndoIter.pAktPam, TRUE, pIdArr );

        break;
    case RES_TXTATR_TOXMARK:
        // special treatment for TOXMarks
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
                    // search for the right one
                    SwHistoryHint* pHHint = (GetHistory())[ 0 ];
                    if( pHHint && HSTRY_SETTOXMARKHNT == pHHint->Which() )
                    {
                        while( nCnt )
                        {
                            if ( static_cast<SwHistorySetTOXMark*>(pHHint)
                                    ->IsEqual( *aArr[ --nCnt ] ) )
                            {
                                ++nCnt;
                                break;
                            }
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

void SwUndoResetAttr::Repeat( SwUndoIter& rUndoIter )
{
    if ( (RES_FMT_BEGIN > m_nFormatId) ||
         ( (UNDO_RESETATTR == rUndoIter.GetLastUndoId()) &&
           (m_nFormatId == static_cast<SwUndoResetAttr*>(rUndoIter.pLastUndoObj)
                            ->m_nFormatId) ) )
    {
        return;
    }

    SvUShortsSort* pIdArr = m_Ids.Count() ? &m_Ids : 0;
    switch ( m_nFormatId )
    {
    case RES_CHRFMT:
        rUndoIter.GetDoc().RstTxtAttrs( *rUndoIter.pAktPam );
        break;
    case RES_TXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttrs( *rUndoIter.pAktPam, FALSE, pIdArr );
        break;
    case RES_CONDTXTFMTCOLL:
        rUndoIter.GetDoc().ResetAttrs( *rUndoIter.pAktPam, TRUE, pIdArr );
        break;
    }
    rUndoIter.pLastUndoObj = this;
}


void SwUndoResetAttr::SetAttrs( const SvUShortsSort& rArr )
{
    if ( m_Ids.Count() )
    {
        m_Ids.Remove( 0, m_Ids.Count() );
    }
    m_Ids.Insert( &rArr );
}

// -----------------------------------------------------


SwUndoAttr::SwUndoAttr( const SwPaM& rRange, const SfxPoolItem& rAttr,
                        USHORT nFlags )
    : SwUndo( UNDO_INSATTR ), SwUndRng( rRange )
    , m_AttrSet( rRange.GetDoc()->GetAttrPool(), rAttr.Which(), rAttr.Which() )
    , m_pHistory( new SwHistory )
    , m_pRedlineData( 0 )
    , m_pRedlineSaveData( 0 )
    , m_nNodeIndex( ULONG_MAX )
    , m_nInsertFlags( nFlags )
{
    m_AttrSet.Put( rAttr );
}

SwUndoAttr::SwUndoAttr( const SwPaM& rRange, const SfxItemSet& rSet,
                        USHORT nFlags )
    : SwUndo( UNDO_INSATTR ), SwUndRng( rRange )
    , m_AttrSet( rSet )
    , m_pHistory( new SwHistory )
    , m_pRedlineData( 0 )
    , m_pRedlineSaveData( 0 )
    , m_nNodeIndex( ULONG_MAX )
    , m_nInsertFlags( nFlags )
{
}

SwUndoAttr::~SwUndoAttr()
{
}

void SwUndoAttr::SaveRedlineData( const SwPaM& rPam, BOOL bIsCntnt )
{
    SwDoc* pDoc = rPam.GetDoc();
    if ( pDoc->IsRedlineOn() )
    {
        m_pRedlineData.reset( new SwRedlineData( bIsCntnt
                                    ? nsRedlineType_t::REDLINE_INSERT
                                    : nsRedlineType_t::REDLINE_FORMAT,
                                pDoc->GetRedlineAuthor() ) );
    }

    m_pRedlineSaveData.reset( new SwRedlineSaveDatas );
    if ( !FillSaveDataForFmt( rPam, *m_pRedlineSaveData ))
    {
        m_pRedlineSaveData.reset(0);
    }

    SetRedlineMode( pDoc->GetRedlineMode() );
    if ( bIsCntnt )
    {
        m_nNodeIndex = rPam.GetPoint()->nNode.GetIndex();
    }
}

void SwUndoAttr::Undo( SwUndoIter& rUndoIter )
{
    SwDoc* pDoc = &rUndoIter.GetDoc();

    RemoveIdx( *pDoc );

    if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
    {
        SwPaM& rPam = *rUndoIter.pAktPam;
        if ( ULONG_MAX != m_nNodeIndex )
        {
            rPam.DeleteMark();
            rPam.GetPoint()->nNode = m_nNodeIndex;
            rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), nSttCntnt );
            rPam.SetMark();
            rPam.GetPoint()->nContent++;
            pDoc->DeleteRedline( rPam, false, USHRT_MAX );
        }
        else
        {
            // alle Format-Redlines entfernen, werden ggfs. neu gesetzt
            SetPaM( rUndoIter );
            pDoc->DeleteRedline( rPam, false, nsRedlineType_t::REDLINE_FORMAT );
            if ( m_pRedlineSaveData.get() )
            {
                SetSaveData( *pDoc, *m_pRedlineSaveData );
            }
        }
    }

    const bool bToLast =  (1 == m_AttrSet.Count())
                       && (RES_TXTATR_FIELD <= *m_AttrSet.GetRanges())
                       && (*m_AttrSet.GetRanges() <= RES_TXTATR_HARDBLANK);

    // restore old values
    m_pHistory->TmpRollback( pDoc, 0, !bToLast );
    m_pHistory->SetTmpEnd( m_pHistory->Count() );

    // set cursor onto Undo area
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
    if ( UNDO_INSATTR == rUndoIter.GetLastUndoId() )
    {
        SwUndoAttr* pLast = static_cast<SwUndoAttr*>(rUndoIter.pLastUndoObj);
        if ((pLast->m_AttrSet.Count() == m_AttrSet.Count()) &&
            (pLast->m_nInsertFlags    == m_nInsertFlags   ) &&
            lcl_HasEqualItems( m_AttrSet, pLast->m_AttrSet ))
        {
            return;
        }
    }


    // RefMarks are not repeat capable
    if ( SFX_ITEM_SET != m_AttrSet.GetItemState( RES_TXTATR_REFMARK, FALSE ) )
    {
        rUndoIter.GetDoc().Insert( *rUndoIter.pAktPam,
                                   m_AttrSet, m_nInsertFlags );
    }
    else if ( 1 < m_AttrSet.Count() )
    {
        SfxItemSet aTmpSet( m_AttrSet );
        aTmpSet.ClearItem( RES_TXTATR_REFMARK );
        rUndoIter.GetDoc().Insert( *rUndoIter.pAktPam,
                                   aTmpSet, m_nInsertFlags );
    }
    rUndoIter.pLastUndoObj = this;
}

void SwUndoAttr::Redo( SwUndoIter& rUndoIter )
{
    // setze Attribut in dem Bereich:
    SetPaM( rUndoIter );
    SwPaM& rPam = *rUndoIter.pAktPam;
    SwDoc& rDoc = rUndoIter.GetDoc();

    if ( m_pRedlineData.get() &&
         IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
    {
        RedlineMode_t eOld = rDoc.GetRedlineMode();
        rDoc.SetRedlineMode_intern(static_cast<RedlineMode_t>(
                    eOld & ~nsRedlineMode_t::REDLINE_IGNORE));
        rDoc.Insert( rPam, m_AttrSet, m_nInsertFlags );

        if ( ULONG_MAX != m_nNodeIndex )
        {
            rPam.SetMark();
            if ( rPam.Move( fnMoveBackward ) )
            {
                rDoc.AppendRedline( new SwRedline( *m_pRedlineData, rPam ),
                        true);
            }
            rPam.DeleteMark();
        }
        else
        {
            rDoc.AppendRedline( new SwRedline( *m_pRedlineData, rPam ), true);
        }

        rDoc.SetRedlineMode_intern( eOld );
    }
    else
    {
        rDoc.Insert( rPam, m_AttrSet, m_nInsertFlags );
    }

    rUndoIter.pLastUndoObj = 0;
}


void SwUndoAttr::RemoveIdx( SwDoc& rDoc )
{
    if ( SFX_ITEM_SET != m_AttrSet.GetItemState( RES_TXTATR_FTN, FALSE ))
        return ;

    SwHistoryHint* pHstHnt;
    SwNodes& rNds = rDoc.GetNodes();
    for ( USHORT n = 0; n < m_pHistory->Count(); ++n )
    {
        xub_StrLen nCntnt = 0;
        ULONG nNode = 0;
        pHstHnt = (*m_pHistory)[ n ];
        switch ( pHstHnt->Which() )
        {
            case HSTRY_RESETTXTHNT:
                {
                    SwHistoryResetTxt * pHistoryHint
                        = static_cast<SwHistoryResetTxt*>(pHstHnt);
                    if ( RES_TXTATR_FTN == pHistoryHint->GetWhich() )
                    {
                        nNode = pHistoryHint->GetNode();
                        nCntnt = pHistoryHint->GetCntnt();
                    }
                }
                break;

            case HSTRY_RESETATTRSET:
                {
                    SwHistoryResetAttrSet * pHistoryHint
                        = static_cast<SwHistoryResetAttrSet*>(pHstHnt);
                    nCntnt = pHistoryHint->GetCntnt();
                    if ( STRING_MAXLEN != nCntnt )
                    {
                        const SvUShorts& rArr = pHistoryHint->GetArr();
                        for ( USHORT i = rArr.Count(); i; )
                        {
                            if ( RES_TXTATR_FTN == rArr[ --i ] )
                            {
                                nNode = pHistoryHint->GetNode();
                                break;
                            }
                        }
                    }
                }
                break;

            default:
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
                    SwTxtFtn* pFtn = static_cast<SwTxtFtn*>(pTxtHt);
                    RemoveIdxFromSection( rDoc, pFtn->GetStartNode()->GetIndex() );
                    return ;
                }
            }
        }
    }
}

// -----------------------------------------------------

SwUndoDefaultAttr::SwUndoDefaultAttr( const SfxItemSet& rSet )
    : SwUndo( UNDO_SETDEFTATTR )
    , m_pOldSet( 0 )
    , m_pTabStop( 0 )
{
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rSet.GetItemState( RES_PARATR_TABSTOP, FALSE, &pItem ) )
    {
        // store separately, because it may change!
        m_pTabStop.reset( static_cast<SvxTabStopItem*>(pItem->Clone()) );
        if ( 1 != rSet.Count() ) // are there more attributes?
        {
            m_pOldSet.reset( new SfxItemSet( rSet ) );
        }
    }
    else
    {
        m_pOldSet.reset( new SfxItemSet( rSet ) );
    }
}

SwUndoDefaultAttr::~SwUndoDefaultAttr()
{
}

void SwUndoDefaultAttr::Undo( SwUndoIter& rUndoIter)
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    if ( m_pOldSet.get() )
    {
        SwUndoFmtAttrHelper aTmp(
                *const_cast<SwTxtFmtColl*>(rDoc.GetDfltTxtFmtColl()) );
        rDoc.SetDefault( *m_pOldSet );
        m_pOldSet.reset( 0 );
        if ( aTmp.GetUndo() )
        {
            // transfer ownership of helper object's old set
            m_pOldSet = aTmp.GetUndo()->m_pOldSet;
        }
    }
    if ( m_pTabStop.get() )
    {
        SvxTabStopItem* pOld = static_cast<SvxTabStopItem*>(
                rDoc.GetDefault( RES_PARATR_TABSTOP ).Clone() );
        rDoc.SetDefault( *m_pTabStop );
        m_pTabStop.reset( pOld );
    }
}

void SwUndoDefaultAttr::Redo( SwUndoIter& rUndoIter)
{
    Undo( rUndoIter );
}

// -----------------------------------------------------

SwUndoMoveLeftMargin::SwUndoMoveLeftMargin(
            const SwPaM& rPam, BOOL bFlag, BOOL bMod )
    : SwUndo( bFlag ? UNDO_INC_LEFTMARGIN : UNDO_DEC_LEFTMARGIN )
    , SwUndRng( rPam )
    , m_pHistory( new SwHistory )
    , m_bModulus( bMod )
{
}

SwUndoMoveLeftMargin::~SwUndoMoveLeftMargin()
{
}

void SwUndoMoveLeftMargin::Undo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    BOOL bUndo = pDoc->DoesUndo();
    pDoc->DoUndo( FALSE );

    // restore old values
    m_pHistory->TmpRollback( pDoc, 0 );
    m_pHistory->SetTmpEnd( m_pHistory->Count() );

    pDoc->DoUndo( bUndo );
    SetPaM( rIter );
}

void SwUndoMoveLeftMargin::Redo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SetPaM( rIter );
    pDoc->MoveLeftMargin( *rIter.pAktPam, GetId() == UNDO_INC_LEFTMARGIN,
            m_bModulus );
}

void SwUndoMoveLeftMargin::Repeat( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    pDoc->MoveLeftMargin( *rIter.pAktPam, GetId() == UNDO_INC_LEFTMARGIN,
            m_bModulus );
    rIter.pLastUndoObj = this;
}

// -----------------------------------------------------

SwUndoChangeFootNote::SwUndoChangeFootNote(
            const SwPaM& rRange, const String& rTxt,
            USHORT nNum, bool bIsEndNote )
    : SwUndo( UNDO_CHGFTN ), SwUndRng( rRange )
    , m_pHistory( new SwHistory() )
    , m_Text( rTxt )
    , m_nNumber( nNum )
    , m_bEndNote( bIsEndNote )
{
}

SwUndoChangeFootNote::~SwUndoChangeFootNote()
{
}

void SwUndoChangeFootNote::Undo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    SetPaM( rIter );

    BOOL bUndo = rDoc.DoesUndo();
    rDoc.DoUndo( FALSE );

    m_pHistory->TmpRollback( &rDoc, 0 );
    m_pHistory->SetTmpEnd( m_pHistory->Count() );

    rDoc.GetFtnIdxs().UpdateAllFtn();

    SetPaM( rIter );
    rDoc.DoUndo( bUndo );
}

void SwUndoChangeFootNote::Redo( SwUndoIter& rIter )
{
    SetPaM( rIter );
    rIter.GetDoc().SetCurFtn( *rIter.pAktPam, m_Text, m_nNumber, m_bEndNote );
    SetPaM( rIter );
}

void SwUndoChangeFootNote::Repeat( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    rDoc.SetCurFtn( *rIter.pAktPam, m_Text, m_nNumber, m_bEndNote );
    rIter.pLastUndoObj = this;
}


// -----------------------------------------------------


SwUndoFootNoteInfo::SwUndoFootNoteInfo( const SwFtnInfo &rInfo )
    : SwUndo( UNDO_FTNINFO )
    , m_pFootNoteInfo( new SwFtnInfo( rInfo ) )
{
}

SwUndoFootNoteInfo::~SwUndoFootNoteInfo()
{
}

void SwUndoFootNoteInfo::Undo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwFtnInfo *pInf = new SwFtnInfo( rDoc.GetFtnInfo() );
    rDoc.SetFtnInfo( *m_pFootNoteInfo );
    m_pFootNoteInfo.reset( pInf );
}

void SwUndoFootNoteInfo::Redo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwFtnInfo *pInf = new SwFtnInfo( rDoc.GetFtnInfo() );
    rDoc.SetFtnInfo( *m_pFootNoteInfo );
    m_pFootNoteInfo.reset( pInf );
}


// -----------------------------------------------------

SwUndoEndNoteInfo::SwUndoEndNoteInfo( const SwEndNoteInfo &rInfo )
    : SwUndo( UNDO_FTNINFO )
    , m_pEndNoteInfo( new SwEndNoteInfo( rInfo ) )
{
}

SwUndoEndNoteInfo::~SwUndoEndNoteInfo()
{
}

void SwUndoEndNoteInfo::Undo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwEndNoteInfo *pInf = new SwEndNoteInfo( rDoc.GetEndNoteInfo() );
    rDoc.SetEndNoteInfo( *m_pEndNoteInfo );
    m_pEndNoteInfo.reset( pInf );
}

void SwUndoEndNoteInfo::Redo( SwUndoIter &rIter )
{
    SwDoc &rDoc = rIter.GetDoc();
    SwEndNoteInfo *pInf = new SwEndNoteInfo( rDoc.GetEndNoteInfo() );
    rDoc.SetEndNoteInfo( *m_pEndNoteInfo );
    m_pEndNoteInfo.reset( pInf );
}

// -----------------------------------------------------

SwUndoDontExpandFmt::SwUndoDontExpandFmt( const SwPosition& rPos )
    : SwUndo( UNDO_DONTEXPAND )
    , m_nNodeIndex( rPos.nNode.GetIndex() )
    , m_nContentIndex( rPos.nContent.GetIndex() )
{
}

void SwUndoDontExpandFmt::Undo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    SwPosition& rPos = *pPam->GetPoint();
    rPos.nNode = m_nNodeIndex;
    rPos.nContent.Assign( rPos.nNode.GetNode().GetCntntNode(), m_nContentIndex);
    pDoc->DontExpandFmt( rPos, FALSE );
}


void SwUndoDontExpandFmt::Redo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    SwPosition& rPos = *pPam->GetPoint();
    rPos.nNode = m_nNodeIndex;
    rPos.nContent.Assign( rPos.nNode.GetNode().GetCntntNode(), m_nContentIndex);
    pDoc->DontExpandFmt( rPos );
}

void SwUndoDontExpandFmt::Repeat( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();
    pDoc->DontExpandFmt( *pPam->GetPoint() );
}

