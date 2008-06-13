/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: undraw.cxx,v $
 * $Revision: 1.22 $
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

#include <rtl/string.h>

#ifndef _RTL_MEMORY_H
#include <rtl/memory.h>
#endif
#include <hintids.hxx>

#ifndef _RTL_STRING_H
#include <rtl/string.h>
#endif
#include <svx/svdogrp.hxx>
#include <svx/svdundo.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdmark.hxx>
#include <fmtanchr.hxx>
#include <fmtflcnt.hxx>
#include <txtflcnt.hxx>
#include <frmfmt.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <frame.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <pam.hxx>
#include <ndtxt.hxx>
#include <undobj.hxx>
#include <dcontact.hxx>
#include <dview.hxx>
#include <rootfrm.hxx>
#include <viewsh.hxx>

struct SwUndoGroupObjImpl
{
    SwDrawFrmFmt* pFmt;
    SdrObject* pObj;
    ULONG nNodeIdx;

    // OD 2004-04-15 #i26791# - keeping the anchor and the relative position
    // of drawing objects no longer needed
};


inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }

// Draw-Objecte

IMPL_LINK( SwDoc, AddDrawUndo, SdrUndoAction *, pUndo )
{
#if OSL_DEBUG_LEVEL > 1
    USHORT nId = pUndo->GetId();
    (void)nId;
    String sComment( pUndo->GetComment() );
#endif

    if( DoesUndo() && !IsNoDrawUndoObj() )
    {
        ClearRedo();
        const SdrMarkList* pMarkList = 0;
        ViewShell* pSh = GetRootFrm() ? GetRootFrm()->GetCurrShell() : 0;
        if( pSh && pSh->HasDrawView() )
            pMarkList = &pSh->GetDrawView()->GetMarkedObjectList();

        AppendUndo( new SwSdrUndo( pUndo, pMarkList ) );
    }
    else
        delete pUndo;
    return 0;
}

SwSdrUndo::SwSdrUndo( SdrUndoAction* pUndo, const SdrMarkList* pMrkLst )
    : SwUndo( UNDO_DRAWUNDO ), pSdrUndo( pUndo )
{
    if( pMrkLst && pMrkLst->GetMarkCount() )
        pMarkList = new SdrMarkList( *pMrkLst );
    else
        pMarkList = 0;
}

SwSdrUndo::~SwSdrUndo()
{
    delete pSdrUndo;
    delete pMarkList;
}

void SwSdrUndo::Undo( SwUndoIter& rUndoIter )
{
    pSdrUndo->Undo();
    rUndoIter.pMarkList = pMarkList;
}

void SwSdrUndo::Redo( SwUndoIter& rUndoIter )
{
    pSdrUndo->Redo();
    rUndoIter.pMarkList = pMarkList;
}

String SwSdrUndo::GetComment() const
{
    return pSdrUndo->GetComment();
}

//--------------------------------------------

void lcl_SendRemoveToUno( SwFmt& rFmt )
{
    SwPtrMsgPoolItem aMsgHint( RES_REMOVE_UNO_OBJECT, &rFmt );
    rFmt.Modify( &aMsgHint, &aMsgHint );
}

void lcl_SaveAnchor( SwFrmFmt* pFmt, ULONG& rNodePos )
{
    const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
    if( FLY_AT_CNTNT == rAnchor.GetAnchorId() ||
        FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ||
        FLY_AT_FLY == rAnchor.GetAnchorId() ||
        FLY_IN_CNTNT == rAnchor.GetAnchorId() )
    {
        rNodePos = rAnchor.GetCntntAnchor()->nNode.GetIndex();
        xub_StrLen nCntntPos = 0;

        if( FLY_IN_CNTNT == rAnchor.GetAnchorId() )
        {
            nCntntPos = rAnchor.GetCntntAnchor()->nContent.GetIndex();

            // TextAttribut zerstoeren
            SwTxtNode *pTxtNd = pFmt->GetDoc()->GetNodes()[ rNodePos ]->GetTxtNode();
            ASSERT( pTxtNd, "Kein Textnode gefunden" );
            SwTxtFlyCnt* pAttr = (SwTxtFlyCnt*)pTxtNd->GetTxtAttr( nCntntPos );
            // Attribut steht noch im TextNode, loeschen
            if( pAttr && pAttr->GetFlyCnt().GetFrmFmt() == pFmt )
            {
                // Pointer auf 0, nicht loeschen
                ((SwFmtFlyCnt&)pAttr->GetFlyCnt()).SetFlyFmt();
                SwIndex aIdx( pTxtNd, nCntntPos );
                pTxtNd->Erase( aIdx, 1 );
            }
        }
        else if( FLY_AUTO_CNTNT == rAnchor.GetAnchorId() )
            nCntntPos = rAnchor.GetCntntAnchor()->nContent.GetIndex();

        pFmt->SetFmtAttr( SwFmtAnchor( rAnchor.GetAnchorId(), nCntntPos ) );
    }
}

void lcl_RestoreAnchor( SwFrmFmt* pFmt, ULONG& rNodePos )
{
    const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
    if( FLY_AT_CNTNT == rAnchor.GetAnchorId() ||
        FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ||
        FLY_AT_FLY == rAnchor.GetAnchorId() ||
        FLY_IN_CNTNT == rAnchor.GetAnchorId() )
    {
        xub_StrLen nCntntPos = rAnchor.GetPageNum();
        SwNodes& rNds = pFmt->GetDoc()->GetNodes();

        SwNodeIndex aIdx( rNds, rNodePos );
        SwPosition aPos( aIdx );

        SwFmtAnchor aTmp( rAnchor.GetAnchorId() );
        if( FLY_IN_CNTNT == rAnchor.GetAnchorId() ||
            FLY_AUTO_CNTNT == rAnchor.GetAnchorId() )
            aPos.nContent.Assign( aIdx.GetNode().GetCntntNode(), nCntntPos );
        aTmp.SetAnchor( &aPos );
        pFmt->SetFmtAttr( aTmp );

        if( FLY_IN_CNTNT == rAnchor.GetAnchorId() )
        {
            SwTxtNode *pTxtNd = aIdx.GetNode().GetTxtNode();
            ASSERT( pTxtNd, "Kein Textnode gefunden" );
            pTxtNd->InsertItem( SwFmtFlyCnt( (SwFrmFmt*)pFmt ),
                                nCntntPos, nCntntPos );
        }
    }
}

SwUndoDrawGroup::SwUndoDrawGroup( USHORT nCnt )
    : SwUndo( UNDO_DRAWGROUP ), nSize( nCnt + 1 ), bDelFmt( TRUE )
{
    pObjArr = new SwUndoGroupObjImpl[ nSize ];
}

SwUndoDrawGroup::~SwUndoDrawGroup()
{
    if( bDelFmt )
    {
        SwUndoGroupObjImpl* pTmp = pObjArr + 1;
        for( USHORT n = 1; n < nSize; ++n, ++pTmp )
            delete pTmp->pFmt;
    }
    else
        delete pObjArr->pFmt;		// das GroupObject-Format

    delete [] pObjArr;
}

void SwUndoDrawGroup::Undo( SwUndoIter& )
{
    bDelFmt = FALSE;

    // das Group-Object sichern
    SwDrawFrmFmt* pFmt = pObjArr->pFmt;
    SwDrawContact* pDrawContact = (SwDrawContact*)pFmt->FindContactObj();
    SdrObject* pObj	= pDrawContact->GetMaster();
    pObjArr->pObj = pObj;

    //loescht sich selbst!
    pDrawContact->Changed( *pObj, SDRUSERCALL_DELETE, pObj->GetLastBoundRect() );
    pObj->SetUserCall( 0 );

    ::lcl_SaveAnchor( pFmt, pObjArr->nNodeIdx );

    // alle Uno-Objecte sollten sich jetzt abmelden
    ::lcl_SendRemoveToUno( *pFmt );

    // aus dem Array austragen
    SwDoc* pDoc = pFmt->GetDoc();
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pDoc->GetSpzFrmFmts();
    rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));

    for( USHORT n = 1; n < nSize; ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );

        ::lcl_RestoreAnchor( rSave.pFmt, rSave.nNodeIdx );
        rFlyFmts.Insert( rSave.pFmt, rFlyFmts.Count() );

        pObj = rSave.pObj;

        SwDrawContact *pContact = new SwDrawContact( rSave.pFmt, pObj );
        pContact->ConnectToLayout();
        // --> OD 2005-03-22 #i45718# - follow-up of #i35635#
        // move object to visible layer
        pContact->MoveObjToVisibleLayer( pObj );
        // <--
        // --> OD 2005-05-10 #i45952# - notify that position attributes
        // are already set
        ASSERT( rSave.pFmt->ISA(SwDrawFrmFmt),
                "<SwUndoDrawGroup::Undo(..)> - wrong type of frame format for drawing object" );
        if ( rSave.pFmt->ISA(SwDrawFrmFmt) )
        {
            static_cast<SwDrawFrmFmt*>(rSave.pFmt)->PosAttrSet();
        }
        // <--
    }
}

void SwUndoDrawGroup::Redo( SwUndoIter& )
{
    bDelFmt = TRUE;

    // aus dem Array austragen
    SwDoc* pDoc = pObjArr->pFmt->GetDoc();
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pDoc->GetSpzFrmFmts();
    SdrObject* pObj;

    for( USHORT n = 1; n < nSize; ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );

        pObj = rSave.pObj;

        SwDrawContact *pContact = (SwDrawContact*)GetUserCall(pObj);
        //loescht sich selbst!
        pContact->Changed( *pObj, SDRUSERCALL_DELETE, pObj->GetLastBoundRect() );
        pObj->SetUserCall( 0 );

        ::lcl_SaveAnchor( rSave.pFmt, rSave.nNodeIdx );

        // alle Uno-Objecte sollten sich jetzt abmelden
        ::lcl_SendRemoveToUno( *rSave.pFmt );

        rFlyFmts.Remove( rFlyFmts.GetPos( rSave.pFmt ));
    }

    // das Group-Object wieder einfuegen
    ::lcl_RestoreAnchor( pObjArr->pFmt, pObjArr->nNodeIdx );
    rFlyFmts.Insert( pObjArr->pFmt, rFlyFmts.Count() );

    SwDrawContact *pContact = new SwDrawContact( pObjArr->pFmt, pObjArr->pObj );
    // OD 2004-04-15 #i26791# - correction: connect object to layout
    pContact->ConnectToLayout();
    // --> OD 2005-03-22 #i45718# - follow-up of #i35635#
    // move object to visible layer
    pContact->MoveObjToVisibleLayer( pObjArr->pObj );
    // <--
    // --> OD 2005-05-10 #i45952# - notify that position attributes
    // are already set
    ASSERT( pObjArr->pFmt->ISA(SwDrawFrmFmt),
            "<SwUndoDrawGroup::Undo(..)> - wrong type of frame format for drawing object" );
    if ( pObjArr->pFmt->ISA(SwDrawFrmFmt) )
    {
        static_cast<SwDrawFrmFmt*>(pObjArr->pFmt)->PosAttrSet();
    }
    // <--
}

void SwUndoDrawGroup::AddObj( USHORT nPos, SwDrawFrmFmt* pFmt, SdrObject* pObj )
{
    SwUndoGroupObjImpl& rSave = *( pObjArr + nPos + 1 );
    rSave.pObj = pObj;
    rSave.pFmt = pFmt;
    ::lcl_SaveAnchor( pFmt, rSave.nNodeIdx );

    // alle Uno-Objecte sollten sich jetzt abmelden
    ::lcl_SendRemoveToUno( *pFmt );

    // aus dem Array austragen
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pFmt->GetDoc()->GetSpzFrmFmts();
    rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));
}

void SwUndoDrawGroup::SetGroupFmt( SwDrawFrmFmt* pFmt )
{
    pObjArr->pObj = 0;
    pObjArr->pFmt = pFmt;
}


// ------------------------------

SwUndoDrawUnGroup::SwUndoDrawUnGroup( SdrObjGroup* pObj )
    : SwUndo( UNDO_DRAWUNGROUP ), bDelFmt( FALSE )
{
    nSize = (USHORT)pObj->GetSubList()->GetObjCount() + 1;
    pObjArr = new SwUndoGroupObjImpl[ nSize ];

    SwDrawContact *pContact = (SwDrawContact*)GetUserCall(pObj);
    SwDrawFrmFmt* pFmt = (SwDrawFrmFmt*)pContact->GetFmt();

    pObjArr->pObj = pObj;
    pObjArr->pFmt = pFmt;

    //loescht sich selbst!
    pContact->Changed( *pObj, SDRUSERCALL_DELETE, pObj->GetLastBoundRect() );
    pObj->SetUserCall( 0 );

    ::lcl_SaveAnchor( pFmt, pObjArr->nNodeIdx );

    // alle Uno-Objecte sollten sich jetzt abmelden
    ::lcl_SendRemoveToUno( *pFmt );

    // aus dem Array austragen
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pFmt->GetDoc()->GetSpzFrmFmts();
    rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));
}

SwUndoDrawUnGroup::~SwUndoDrawUnGroup()
{
    if( bDelFmt )
    {
        SwUndoGroupObjImpl* pTmp = pObjArr + 1;
        for( USHORT n = 1; n < nSize; ++n, ++pTmp )
            delete pTmp->pFmt;
    }
    else
        delete pObjArr->pFmt;		// das GroupObject-Format

    delete [] pObjArr;
}

void SwUndoDrawUnGroup::Undo( SwUndoIter& rIter )
{
    bDelFmt = TRUE;

    // aus dem Array austragen
    SwDoc* pDoc = &rIter.GetDoc();
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pDoc->GetSpzFrmFmts();

    for( USHORT n = 1; n < nSize; ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );

        // --> OD 2006-11-01 #130889# - taken over by <SwUndoDrawUnGroupConnectToLayout>
//        SwDrawContact* pContact = (SwDrawContact*)rSave.pFmt->FindContactObj();

//        rSave.pObj = pContact->GetMaster();

//        //loescht sich selbst!
//        pContact->Changed( *rSave.pObj, SDRUSERCALL_DELETE,
//                           rSave.pObj->GetLastBoundRect() );
//        rSave.pObj->SetUserCall( 0 );
        // <--

        ::lcl_SaveAnchor( rSave.pFmt, rSave.nNodeIdx );

        // alle Uno-Objecte sollten sich jetzt abmelden
        ::lcl_SendRemoveToUno( *rSave.pFmt );

        rFlyFmts.Remove( rFlyFmts.GetPos( rSave.pFmt ));
    }

    // das Group-Object wieder einfuegen
    ::lcl_RestoreAnchor( pObjArr->pFmt, pObjArr->nNodeIdx );
    rFlyFmts.Insert( pObjArr->pFmt, rFlyFmts.Count() );

    SwDrawContact *pContact = new SwDrawContact( pObjArr->pFmt, pObjArr->pObj );
    pContact->ConnectToLayout();
    // --> OD 2005-03-22 #i45718# - follow-up of #i35635#
    // move object to visible layer
    pContact->MoveObjToVisibleLayer( pObjArr->pObj );
    // <--
    // --> OD 2005-05-10 #i45952# - notify that position attributes
    // are already set
    ASSERT( pObjArr->pFmt->ISA(SwDrawFrmFmt),
            "<SwUndoDrawGroup::Undo(..)> - wrong type of frame format for drawing object" );
    if ( pObjArr->pFmt->ISA(SwDrawFrmFmt) )
    {
        static_cast<SwDrawFrmFmt*>(pObjArr->pFmt)->PosAttrSet();
    }
    // <--
}

void SwUndoDrawUnGroup::Redo( SwUndoIter& )
{
    bDelFmt = FALSE;

    // das Group-Object sichern
    SwDrawFrmFmt* pFmt = pObjArr->pFmt;
    SwDrawContact* pContact = (SwDrawContact*)pFmt->FindContactObj();

    //loescht sich selbst!
    pContact->Changed( *pObjArr->pObj, SDRUSERCALL_DELETE,
        pObjArr->pObj->GetLastBoundRect() );
    pObjArr->pObj->SetUserCall( 0 );

    ::lcl_SaveAnchor( pFmt, pObjArr->nNodeIdx );

    // alle Uno-Objecte sollten sich jetzt abmelden
    ::lcl_SendRemoveToUno( *pFmt );

    // aus dem Array austragen
    SwDoc* pDoc = pFmt->GetDoc();
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pDoc->GetSpzFrmFmts();
    rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));

    for( USHORT n = 1; n < nSize; ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );

        ::lcl_RestoreAnchor( rSave.pFmt, rSave.nNodeIdx );
        rFlyFmts.Insert( rSave.pFmt, rFlyFmts.Count() );

        // --> OD 2006-11-01 #130889# - taken over by <SwUndoDrawUnGroupConnectToLayout>
//        SdrObject* pObj = rSave.pObj;

//        SwDrawContact *pContact = new SwDrawContact( rSave.pFmt, rSave.pObj );
//        pContact->ConnectToLayout();
//        // --> OD 2005-03-22 #i45718# - follow-up of #i35635#
//        // move object to visible layer
//        pContact->MoveObjToVisibleLayer( rSave.pObj );
//        // <--
        // <--
        // --> OD 2005-05-10 #i45952# - notify that position attributes
        // are already set
        ASSERT( rSave.pFmt->ISA(SwDrawFrmFmt),
                "<SwUndoDrawGroup::Undo(..)> - wrong type of frame format for drawing object" );
        if ( rSave.pFmt->ISA(SwDrawFrmFmt) )
        {
            static_cast<SwDrawFrmFmt*>(rSave.pFmt)->PosAttrSet();
        }
        // <--
    }
}

void SwUndoDrawUnGroup::AddObj( USHORT nPos, SwDrawFrmFmt* pFmt )
{
    SwUndoGroupObjImpl& rSave = *( pObjArr + nPos + 1 );
    rSave.pFmt = pFmt;
    rSave.pObj = 0;
}

//-------------------------------------
// --> OD 2006-11-01 #130889#
SwUndoDrawUnGroupConnectToLayout::SwUndoDrawUnGroupConnectToLayout()
    : SwUndo( UNDO_DRAWUNGROUP )
{
}

SwUndoDrawUnGroupConnectToLayout::~SwUndoDrawUnGroupConnectToLayout()
{
}

void SwUndoDrawUnGroupConnectToLayout::Undo( SwUndoIter& )
{
    for ( std::vector< SdrObject >::size_type i = 0;
          i < aDrawFmtsAndObjs.size(); ++i )
    {
        SdrObject* pObj( aDrawFmtsAndObjs[i].second );
        SwDrawContact* pDrawContact( dynamic_cast<SwDrawContact*>(pObj->GetUserCall()) );
        ASSERT( pDrawContact,
                "<SwUndoDrawUnGroupConnectToLayout::Undo(..)> -- missing SwDrawContact instance" );
        if ( pDrawContact )
        {
            // deletion of instance <pDrawContact> and thus disconnection from
            // the Writer layout.
            pDrawContact->Changed( *pObj, SDRUSERCALL_DELETE, pObj->GetLastBoundRect() );
            pObj->SetUserCall( 0 );
        }
    }
}

void SwUndoDrawUnGroupConnectToLayout::Redo( SwUndoIter& )
{
    for ( std::vector< std::pair< SwDrawFrmFmt*, SdrObject* > >::size_type i = 0;
          i < aDrawFmtsAndObjs.size(); ++i )
    {
        SwDrawFrmFmt* pFmt( aDrawFmtsAndObjs[i].first );
        SdrObject* pObj( aDrawFmtsAndObjs[i].second );
        SwDrawContact *pContact = new SwDrawContact( pFmt, pObj );
        pContact->ConnectToLayout();
        pContact->MoveObjToVisibleLayer( pObj );
    }
}

void SwUndoDrawUnGroupConnectToLayout::AddFmtAndObj( SwDrawFrmFmt* pDrawFrmFmt,
                                                     SdrObject* pDrawObject )
{
    aDrawFmtsAndObjs.push_back(
            std::pair< SwDrawFrmFmt*, SdrObject* >( pDrawFrmFmt, pDrawObject ) );
}
// <--

//-------------------------------------

SwUndoDrawDelete::SwUndoDrawDelete( USHORT nCnt )
    : SwUndo( UNDO_DRAWDELETE ), nSize( nCnt ), bDelFmt( TRUE )
{
    pObjArr = new SwUndoGroupObjImpl[ nSize ];
    pMarkLst = new SdrMarkList();
}

SwUndoDrawDelete::~SwUndoDrawDelete()
{
    if( bDelFmt )
    {
        SwUndoGroupObjImpl* pTmp = pObjArr;
        for( USHORT n = 0; n < pMarkLst->GetMarkCount(); ++n, ++pTmp )
            delete pTmp->pFmt;
    }
    delete [] pObjArr;
    delete pMarkLst;
}

void SwUndoDrawDelete::Undo( SwUndoIter &rIter )
{
    bDelFmt = FALSE;
    SwSpzFrmFmts& rFlyFmts = *rIter.GetDoc().GetSpzFrmFmts();
    for( USHORT n = 0; n < pMarkLst->GetMarkCount(); ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );
        ::lcl_RestoreAnchor( rSave.pFmt, rSave.nNodeIdx );
        rFlyFmts.Insert( rSave.pFmt, rFlyFmts.Count() );
        SdrObject *pObj = rSave.pObj;
        SwDrawContact *pContact = new SwDrawContact( rSave.pFmt, pObj );
        pContact->_Changed( *pObj, SDRUSERCALL_INSERTED, NULL );
        // --> OD 2005-03-22 #i45718# - follow-up of #i35635#
        // move object to visible layer
        pContact->MoveObjToVisibleLayer( pObj );
        // <--
        // --> OD 2005-05-10 #i45952# - notify that position attributes
        // are already set
        ASSERT( rSave.pFmt->ISA(SwDrawFrmFmt),
                "<SwUndoDrawGroup::Undo(..)> - wrong type of frame format for drawing object" );
        if ( rSave.pFmt->ISA(SwDrawFrmFmt) )
        {
            static_cast<SwDrawFrmFmt*>(rSave.pFmt)->PosAttrSet();
        }
        // <--
    }
    rIter.pMarkList = pMarkLst;
}

void SwUndoDrawDelete::Redo( SwUndoIter &rIter )
{
    bDelFmt = TRUE;
    SwSpzFrmFmts& rFlyFmts = *rIter.GetDoc().GetSpzFrmFmts();
    for( USHORT n = 0; n < pMarkLst->GetMarkCount(); ++n )
    {
        SwUndoGroupObjImpl& rSave = *( pObjArr + n );
        SdrObject *pObj = rSave.pObj;
        SwDrawContact *pContact = (SwDrawContact*)GetUserCall(pObj);
        SwDrawFrmFmt *pFmt = (SwDrawFrmFmt*)pContact->GetFmt();
        //loescht sich selbst!
        pContact->Changed( *pObj, SDRUSERCALL_DELETE, pObj->GetLastBoundRect() );
        pObj->SetUserCall( 0 );

        // alle Uno-Objecte sollten sich jetzt abmelden
        ::lcl_SendRemoveToUno( *pFmt );

        rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));
        ::lcl_SaveAnchor( pFmt, rSave.nNodeIdx );
    }
}

void SwUndoDrawDelete::AddObj( USHORT , SwDrawFrmFmt* pFmt,
                                const SdrMark& rMark )
{
    SwUndoGroupObjImpl& rSave = *( pObjArr + pMarkLst->GetMarkCount() );
    rSave.pObj = rMark.GetMarkedSdrObj();
    rSave.pFmt = pFmt;
    ::lcl_SaveAnchor( pFmt, rSave.nNodeIdx );

    // alle Uno-Objecte sollten sich jetzt abmelden
    ::lcl_SendRemoveToUno( *pFmt );

    // aus dem Array austragen
    SwDoc* pDoc = pFmt->GetDoc();
    SwSpzFrmFmts& rFlyFmts = *(SwSpzFrmFmts*)pDoc->GetSpzFrmFmts();
    rFlyFmts.Remove( rFlyFmts.GetPos( pFmt ));

    pMarkLst->InsertEntry( rMark );
}

