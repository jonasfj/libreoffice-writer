/*************************************************************************
 *
 *  $RCSfile: dcontact.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: obo $ $Date: 2003-12-05 11:27:15 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#pragma hdrstop

#include "hintids.hxx"

#ifndef _SVX_PROTITEM_HXX //autogen
#include <svx/protitem.hxx>
#endif
#ifndef _SVX_OPAQITEM_HXX //autogen
#include <svx/opaqitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif
#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif
#ifndef _SVDOGRP_HXX //autogen
#include <svx/svdogrp.hxx>
#endif
#ifndef _SVDOTEXT_HXX //autogen
#include <svx/svdotext.hxx>
#endif
#ifndef _SVDMODEL_HXX //autogen
#include <svx/svdmodel.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVDVITER_HXX
#include <svx/svdviter.hxx>
#endif
#ifndef _SVDVIEW_HXX
#include <svx/svdview.hxx>
#endif


#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _VIEWIMP_HXX //autogen
#include <viewimp.hxx>
#endif
#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#ifndef _FMTCNTNT_HXX //autogen
#include <fmtcntnt.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>	// Notify_Background
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif

// #108784#
#ifndef _XOUTX_HXX
#include <svx/xoutx.hxx>
#endif

// OD 18.06.2003 #108784#
#include <algorithm>


TYPEINIT1( SwContact, SwClient )
TYPEINIT1( SwFlyDrawContact, SwContact )
TYPEINIT1( SwDrawContact, SwContact )


//Der Umgekehrte Weg: Sucht das Format zum angegebenen Objekt.
//Wenn das Object ein SwVirtFlyDrawObj ist so wird das Format von
//selbigem besorgt.
//Anderfalls ist es eben ein einfaches Zeichenobjekt. Diese hat einen
//UserCall und der ist Client vom gesuchten Format.

SwFrmFmt *FindFrmFmt( SdrObject *pObj )
{
    SwFrmFmt* pRetval = 0L;

    if ( pObj->ISA(SwVirtFlyDrawObj) )
    {
        pRetval = ((SwVirtFlyDrawObj*)pObj)->GetFmt();
    }
    else
    {
        SwDrawContact* pContact = static_cast<SwDrawContact*>(GetUserCall( pObj ));
        if ( pContact )
        {
            pRetval = pContact->GetFmt();
        }
    }

    return pRetval;
}

sal_Bool HasWrap( const SdrObject* pObj )
{
    if ( pObj )
    {
        const SwFrmFmt* pFmt = ::FindFrmFmt( pObj );
        if ( pFmt )
        {
            return SURROUND_THROUGHT != pFmt->GetSurround().GetSurround();
        }
    }

    return sal_False;
}

/*****************************************************************************
 *
 * GetBoundRect liefert das BoundRect _inklusive_ Abstand des Objekts.
 *
 *****************************************************************************/

SwRect GetBoundRect( const SdrObject* pObj )
{
    SwRect aRet( pObj->GetCurrentBoundRect() );
    const SwFmt *pFmt = ((SwContact*)GetUserCall(pObj))->GetFmt();
    const SvxULSpaceItem &rUL = pFmt->GetULSpace();
    const SvxLRSpaceItem &rLR = pFmt->GetLRSpace();
    {
        aRet.Top ( Max( aRet.Top() - long(rUL.GetUpper()), 0L ));
        aRet.Left( Max( aRet.Left()- long(rLR.GetLeft()),  0L ));
        aRet.SSize().Height() += rUL.GetLower();
        aRet.SSize().Width()  += rLR.GetRight();
    }
    return aRet;
}

/*****************************************************************************
 *
 *  Moves a SdrObj so that it fits to a given frame
 *
 *****************************************************************************/

void CaptureDrawObj( SdrObject& rObj, const SwRect& rFrm )
{
    Rectangle aRect( rObj.GetCurrentBoundRect() );
    if ( aRect.Right() >= rFrm.Right() + 10 )
    {
        Size aSize( rFrm.Right() - aRect.Right(), 0 );
        rObj.Move( aSize );
        aRect = rObj.GetCurrentBoundRect();
    }

    if ( aRect.Left() + 10 <= rFrm.Left() )
    {
        Size aSize( rFrm.Left() - aRect.Left(), 0 );
        rObj.Move( aSize );
    }
}

//Liefert den UserCall ggf. vom Gruppenobjekt
SdrObjUserCall* GetUserCall( const SdrObject* pObj )
{
    SdrObject *pTmp;
    while ( !pObj->GetUserCall() && 0 != (pTmp = pObj->GetUpGroup()) )
        pObj = pTmp;
    return pObj->GetUserCall();
}

// liefert TRUE falls das SrdObject ein Marquee-Object (Lauftext) ist
FASTBOOL IsMarqueeTextObj( const SdrObject& rObj )
{
    SdrTextAniKind eTKind;
    return SdrInventor == rObj.GetObjInventor() &&
        OBJ_TEXT == rObj.GetObjIdentifier() &&
        ( SDRTEXTANI_SCROLL == ( eTKind = ((SdrTextObj&)rObj).GetTextAniKind())
         || SDRTEXTANI_ALTERNATE == eTKind || SDRTEXTANI_SLIDE == eTKind );
}

/*************************************************************************
|*
|*	SwContact, Ctor und Dtor
|*
|*	Ersterstellung		AMA 27.Sep.96 18:13
|*	Letzte Aenderung	AMA 27.Sep.96
|*
|*************************************************************************/

SwContact::SwContact( SwFrmFmt *pToRegisterIn, SdrObject *pObj ) :
    SwClient( pToRegisterIn ),
    pMasterObj( pObj ),
    // OD 05.09.2003 #112039# - init member <mbInDTOR>
    mbInDTOR( false )
{
    pObj->SetUserCall( this );
}

SwContact::~SwContact()
{
    // OD 05.09.2003 #112039# - set <mbInDTOR>
    SetInDTOR();

    if ( pMasterObj )
    {
        pMasterObj->SetUserCall( 0 );	//Soll mir nicht in den Ruecken fallen.
        if ( pMasterObj->GetPage() )	//Der SdrPage die Verantwortung entziehen.
            pMasterObj->GetPage()->RemoveObject( pMasterObj->GetOrdNum() );
        delete pMasterObj;
    }
}

// OD 05.09.2003 #112039# - accessor for member <mbInDTOR>
const bool SwContact::IsInDTOR() const
{
    return mbInDTOR;
}

// OD 05.09.2003 #112039# - accessor to set member <mbInDTOR>
void SwContact::SetInDTOR()
{
    mbInDTOR = true;
}

// OD 13.05.2003 #108784# - copied inline-implementation of <GetMaster()> and
// <SetMaster()>
const SdrObject* SwContact::GetMaster() const
{
    return pMasterObj;
}
SdrObject* SwContact::GetMaster()
{
    return pMasterObj;
}
void SwContact::SetMaster( SdrObject* pNew )
{
    pMasterObj = pNew;
}

/*************************************************************************
|*
|*	SwFlyDrawContact, Ctor und Dtor
|*
|*	Ersterstellung		OK 23.11.94 18:13
|*	Letzte Aenderung	MA 06. Apr. 95
|*
|*************************************************************************/

SwFlyDrawContact::SwFlyDrawContact( SwFlyFrmFmt *pToRegisterIn, SdrModel *pMod ) :
    SwContact( pToRegisterIn )
{
    SetMaster( new SwFlyDrawObj() );
    GetMaster()->SetOrdNum( 0xFFFFFFFE );
    GetMaster()->SetUserCall( this );
}

/*************************************************************************
|*
|*	SwFlyDrawContact::CreateNewRef()
|*
|*	Ersterstellung		MA 14. Dec. 94
|*	Letzte Aenderung	MA 24. Apr. 95
|*
|*************************************************************************/

SwVirtFlyDrawObj *SwFlyDrawContact::CreateNewRef( SwFlyFrm *pFly )
{
    SwVirtFlyDrawObj *pDrawObj = new SwVirtFlyDrawObj( *GetMaster(), pFly );
    pDrawObj->SetModel( GetMaster()->GetModel() );
    pDrawObj->SetUserCall( this );

    //Der Reader erzeugt die Master und setzt diese, um die Z-Order zu
    //transportieren, in die Page ein. Beim erzeugen der ersten Referenz werden
    //die Master aus der Liste entfernt und fuehren von da an ein
    //Schattendasein.
    SdrPage *pPg;
    if ( 0 != ( pPg = GetMaster()->GetPage() ) )
    {
        const UINT32 nOrdNum = GetMaster()->GetOrdNum();
        pPg->ReplaceObject( pDrawObj, nOrdNum );
    }
    return pDrawObj;
}

/*************************************************************************
|*
|*	SwFlyDrawContact::Modify()
|*
|*	Ersterstellung		OK 08.11.94 10:21
|*	Letzte Aenderung	MA 06. Dec. 94
|*
|*************************************************************************/

void SwFlyDrawContact::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew )
{
}

/*************************************************************************
|*
|*	SwDrawContact, Ctor+Dtor
|*
|*	Ersterstellung		MA 09. Jan. 95
|*	Letzte Aenderung	MA 22. Jul. 98
|*
|*************************************************************************/
bool CheckControlLayer( const SdrObject *pObj )
{
    if ( FmFormInventor == pObj->GetObjInventor() )
        return true;
    if ( pObj->ISA( SdrObjGroup ) )
    {
        const SdrObjList *pLst = ((SdrObjGroup*)pObj)->GetSubList();
        for ( USHORT i = 0; i < pLst->GetObjCount(); ++i )
        {
            if ( ::CheckControlLayer( pLst->GetObj( i ) ) )
            {
                // OD 21.08.2003 #i18447# - return correct value ;-)
                return true;
            }
        }
    }
    return false;
}

SwDrawContact::SwDrawContact( SwFrmFmt *pToRegisterIn, SdrObject *pObj ) :
    SwContact( pToRegisterIn, pObj ),
    pAnchor( 0 ),
    pPage( 0 ),
    // OD 10.10.2003 #112299#
    mbDisconnectInProgress( false )
{
    // clear vector containing 'virtual' drawing objects.
   maDrawVirtObjs.clear();

    //Controls muessen immer im Control-Layer liegen. Das gilt auch fuer
    //Gruppenobjekte, wenn diese Controls enthalten.
    if ( ::CheckControlLayer( pObj ) )
    {
        // OD 25.06.2003 #108784# - set layer of object to corresponding invisible layer.
        pObj->SetLayer( pToRegisterIn->GetDoc()->GetInvisibleControlsId() );
    }
}

SwDrawContact::~SwDrawContact()
{
    // OD 05.09.2003 #112039# - set <mbInDTOR>
    SetInDTOR();

    DisconnectFromLayout();

    // OD 25.06.2003 #108784# - remove 'master' from drawing page
    RemoveMasterFromDrawPage();

    // remove and destroy 'virtual' drawing objects.
    RemoveAllVirtObjs();
}

// OD 23.06.2003 #108784# - method to create a new 'virtual' drawing object.
SwDrawVirtObj* SwDrawContact::CreateVirtObj()
{
    // determine 'master'
    SdrObject* pOrgMasterSdrObj = GetMaster();

    // create 'virtual' drawing object
    SwDrawVirtObj* pNewDrawVirtObj = new SwDrawVirtObj ( *(pOrgMasterSdrObj), *(this) );

    // add new 'virtual' drawing object managing data structure
    maDrawVirtObjs.push_back( pNewDrawVirtObj );

    return pNewDrawVirtObj;
}

// OD 23.06.2003 #108784# - destroys a given 'virtual' drawing object.
// side effect: 'virtual' drawing object is removed from data structure
//              <maDrawVirtObjs>.
void SwDrawContact::DestroyVirtObj( SwDrawVirtObj* _pVirtObj )
{
    if ( _pVirtObj )
    {
        delete _pVirtObj;
        _pVirtObj = 0;
    }
}

// OD 16.05.2003 #108784# - add a 'virtual' drawing object to drawing page.
// Use an already created one, which isn't used, or create a new one.
SwDrawVirtObj* SwDrawContact::AddVirtObj()
{
    SwDrawVirtObj* pAddedDrawVirtObj = 0L;

    // check, if a disconnected 'virtual' drawing object exist and use it
    std::list<SwDrawVirtObj*>::const_iterator aFoundVirtObjIter =
            std::find_if( maDrawVirtObjs.begin(), maDrawVirtObjs.end(),
                          UsedOrUnusedVirtObjPred( false ) );

    if ( aFoundVirtObjIter != maDrawVirtObjs.end() )
    {
        // use already created, disconnected 'virtual' drawing object
        pAddedDrawVirtObj = (*aFoundVirtObjIter);
    }
    else
    {
        // create new 'virtual' drawing object.
        pAddedDrawVirtObj = CreateVirtObj();
    }
    pAddedDrawVirtObj->AddToDrawingPage();

    return pAddedDrawVirtObj;
}

// OD 16.05.2003 #108784# - remove 'virtual' drawing objects and destroy them.
void SwDrawContact::RemoveAllVirtObjs()
{
    for ( std::list<SwDrawVirtObj*>::iterator aDrawVirtObjsIter = maDrawVirtObjs.begin();
          aDrawVirtObjsIter != maDrawVirtObjs.end();
          ++aDrawVirtObjsIter )
    {
        // remove and destroy 'virtual object'
        SwDrawVirtObj* pDrawVirtObj = (*aDrawVirtObjsIter);
        pDrawVirtObj->RemoveFromWriterLayout();
        pDrawVirtObj->RemoveFromDrawingPage();
        DestroyVirtObj( pDrawVirtObj );
    }
    maDrawVirtObjs.clear();
}

// OD 16.05.2003 #108784# - overload <SwContact::SetMaster(..)> in order to
// assert, if the 'master' drawing object is replaced.
// OD 10.07.2003 #110742# - replace of master object correctly handled, if
// handled by method <SwDrawContact::ChangeMasterObject(..)>. Thus, assert
// only, if a debug level is given.
void SwDrawContact::SetMaster( SdrObject* pNew )
{
#if OSL_DEBUG_LEVEL > 1
    ASSERT( !pNew, "debug notification - master replaced!" );
#endif
    SwContact::SetMaster( pNew );
}

// OD 19.06.2003 #108784# - get drawing object ('master' or 'virtual') by frame.
SdrObject* SwDrawContact::GetDrawObjectByAnchorFrm( const SwFrm& _rAnchorFrm )
{
    SdrObject* pRetDrawObj = 0L;

    if ( GetAnchor() && GetAnchor() == &_rAnchorFrm )
    {
        pRetDrawObj = GetMaster();
    }
    else
    {
        std::list<SwDrawVirtObj*>::const_iterator aFoundVirtObjIter =
                std::find_if( maDrawVirtObjs.begin(), maDrawVirtObjs.end(),
                              VirtObjAnchoredAtFrmPred( _rAnchorFrm ) );

        if ( aFoundVirtObjIter != maDrawVirtObjs.end() )
        {
            pRetDrawObj = (*aFoundVirtObjIter);
        }
    }

    return pRetDrawObj;
}

/*************************************************************************
|*
|*	SwDrawContact::Changed
|*
|*	Ersterstellung		MA 09. Jan. 95
|*	Letzte Aenderung	MA 29. May. 96
|*
|*************************************************************************/

void lcl_Notify( SwDrawContact* pThis, const Rectangle* pOldBoundRect )
{
    SwFrm *pAnch = pThis->GetAnchor();
    if( pAnch )
    {
        SwPageFrm *pPage = pAnch->FindPageFrm();
        ASSERT( GetUserCall(pThis->GetMaster()) == pThis, "Wrong Master" );
        if( pOldBoundRect && pPage )
        {
            SwRect aOldRect( *pOldBoundRect );
            if( aOldRect.HasArea() )
                Notify_Background( pThis->GetMaster(),pPage, aOldRect,
                                    PREP_FLY_LEAVE,TRUE);
        }
        SwRect aRect( pThis->GetMaster()->GetCurrentBoundRect() );
        if( aRect.HasArea() )
        {
            SwPageFrm *pPg = pThis->FindPage( aRect );
            if( pPg )
                Notify_Background( pThis->GetMaster(), pPg, aRect,
                                    PREP_FLY_ARRIVE, TRUE );
        }
        ClrContourCache( pThis->GetMaster() );

        // OD 03.07.2003 #108784# - notify background of all 'virtual' drawing objects
        pThis->NotifyBackgrdOfAllVirtObjs( pOldBoundRect );
    }
}

// OD 03.07.2003 #108784#
void SwDrawContact::NotifyBackgrdOfAllVirtObjs( const Rectangle* pOldBoundRect )
{
    for ( std::list<SwDrawVirtObj*>::iterator aDrawVirtObjIter = maDrawVirtObjs.begin();
          aDrawVirtObjIter != maDrawVirtObjs.end();
          ++aDrawVirtObjIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDrawVirtObjIter);
        if ( pDrawVirtObj->GetAnchorFrm() )
        {
            SwPageFrm* pPage = pDrawVirtObj->GetAnchorFrm()->FindPageFrm();
            if( pOldBoundRect && pPage )
            {
                SwRect aOldRect( *pOldBoundRect );
                aOldRect.Pos() += pDrawVirtObj->GetOffset();
                if( aOldRect.HasArea() )
                    Notify_Background( pDrawVirtObj ,pPage,
                                       aOldRect, PREP_FLY_LEAVE,TRUE);
            }
            SwRect aRect( pDrawVirtObj->GetCurrentBoundRect() );
            if( aRect.HasArea() )
            {
                SwPageFrm *pPg = pDrawVirtObj->GetPageFrm();
                if ( !pPg )
                    pPg = pDrawVirtObj->GetAnchorFrm()->FindPageFrm();
                if ( pPg )
                    pPg = (SwPageFrm*)::FindPage( aRect, pPg );
                if( pPg )
                    Notify_Background( pDrawVirtObj, pPg, aRect,
                                        PREP_FLY_ARRIVE, TRUE );
            }
            ClrContourCache( pDrawVirtObj );
        }
    }
}

void SwDrawContact::Changed( const SdrObject& rObj,
                             SdrUserCallType eType,
                             const Rectangle& rOldBoundRect )
{
    //Action aufsetzen, aber nicht wenn gerade irgendwo eine Action laeuft.
    ViewShell *pSh = 0, *pOrg;
    SwDoc *pDoc = GetFmt()->GetDoc();
    if ( pDoc->GetRootFrm() && pDoc->GetRootFrm()->IsCallbackActionEnabled() )
    {
        pDoc->GetEditShell( &pOrg );
        pSh = pOrg;
        if ( pSh )
            do
            {   if ( pSh->Imp()->IsAction() || pSh->Imp()->IsIdleAction() )
                    pSh = 0;
                else
                    pSh = (ViewShell*)pSh->GetNext();

            } while ( pSh && pSh != pOrg );

        if ( pSh )
            pDoc->GetRootFrm()->StartAllAction();
    }

    SdrObjUserCall::Changed( rObj, eType, rOldBoundRect );
    _Changed( rObj, eType, &rOldBoundRect );	//Achtung, ggf. Suizid!

    if ( pSh )
        pDoc->GetRootFrm()->EndAllAction();
}

void SwDrawContact::_Changed( const SdrObject& rObj,
                              SdrUserCallType eType,
                              const Rectangle* pOldBoundRect )
{
    BOOL bInCntnt = FLY_IN_CNTNT == GetFmt()->GetAnchor().GetAnchorId();
    /// OD 05.08.2002 #100843# - do *not* notify, if document is destructing
    BOOL bNotify = !bInCntnt &&
                   !(GetFmt()->GetDoc()->IsInDtor()) &&
                   ( SURROUND_THROUGHT != GetFmt()->GetSurround().GetSurround() );
    switch( eType )
    {
        case SDRUSERCALL_DELETE:
            {
                if( bNotify )
                    lcl_Notify( this, pOldBoundRect );
                DisconnectFromLayout( false );
                SetMaster( NULL );
                delete this;
                break;
            }
        case SDRUSERCALL_INSERTED:
            {
                // OD 10.10.2003 #112299#
                if ( mbDisconnectInProgress )
                {
                    ASSERT( false,
                            "<SwDrawContact::_Changed(..)> - Insert event during disconnection from layout is invalid." );
                }
                else
                {
                    ConnectToLayout();
                    if( bNotify )
                        lcl_Notify( this, pOldBoundRect );
                }
                break;
            }
        case SDRUSERCALL_REMOVED:
            {
                if( bNotify )
                    lcl_Notify( this, pOldBoundRect );
                DisconnectFromLayout( false );
                break;
            }
        case SDRUSERCALL_MOVEONLY:
        case SDRUSERCALL_RESIZE:
        case SDRUSERCALL_CHILD_MOVEONLY :
        case SDRUSERCALL_CHILD_RESIZE :
        case SDRUSERCALL_CHILD_CHGATTR :
        case SDRUSERCALL_CHILD_DELETE :
        case SDRUSERCALL_CHILD_COPY :
        case SDRUSERCALL_CHILD_INSERTED :
        case SDRUSERCALL_CHILD_REMOVED :
            if( bInCntnt )
            {
                SwFrm *pAnch = GetAnchor();
                // OD 10.10.2003 #112299# - do not try to connect to layout,
                // if disconnection from layout is in progress.
                // Note: Such a usercall can be initiated during disconnection
                //       of the group object, if a member 'connector' object is
                //       formatted, because the object, its 'connected' to is
                //       disconnected from layout.
                if ( !pAnch && !mbDisconnectInProgress )
                {
                    ConnectToLayout();
                    pAnch = GetAnchor();
                }
                if( pAnch && !((SwTxtFrm*)pAnch)->IsLocked() &&
                    &rObj == GetMaster() )
                {
                    SwFrmFmt *pFmt = GetFmt();
                    const SwFmtVertOrient &rVert = pFmt->GetVertOrient();
                    SwTwips nRel;
                    if( pAnch->IsVertical() )
                    {
                        nRel = rObj.GetRelativePos().X();
                        if( !pAnch->IsReverse() )
                            nRel = - nRel - rObj.GetSnapRect().GetWidth();
                    }
                    else
                    {
                        nRel = rObj.GetRelativePos().Y();
                    }
                    if( rVert.GetPos() != nRel )
                    {
                        SwFmtVertOrient aVert( rVert );
                        aVert.SetVertOrient( VERT_NONE );
                        aVert.SetPos( nRel );
                        pFmt->SetAttr( aVert );
                    }
                    ((SwTxtFrm*)pAnch)->Prepare();
                    InvalidateAnchorOfVirtObjs();
                }
            }
            else if( bNotify )
                lcl_Notify( this, pOldBoundRect );
            break;
        case SDRUSERCALL_CHGATTR:
            if( bNotify )
                lcl_Notify( this, pOldBoundRect );
            break;
    }
}

/*************************************************************************
|*
|*	SwDrawContact::Modify()
|*
|*	Ersterstellung		MA 09. Jan. 95
|*	Letzte Aenderung	MA 03. Dec. 95
|*
|*************************************************************************/

void SwDrawContact::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew )
{
    // OD 10.10.2003 #112299#
    ASSERT( !mbDisconnectInProgress,
            "<SwDrawContact::Modify(..)> called during disconnection.");

    //Es kommen immer Sets herein.
    //MA 03. Dec. 95: Falsch es kommen nicht immer Sets herein
    //(siehe SwRootFrm::AssertPageFlys()
    USHORT nWhich = pNew ? pNew->Which() : 0;
    SwFmtAnchor *pAnch = 0;
    if( RES_ATTRSET_CHG == nWhich )
    {
        if(	SFX_ITEM_SET ==	((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                            RES_ANCHOR, FALSE, (const SfxPoolItem**)&pAnch ))
        ;		// Beim GetItemState wird der AnkerPointer gesetzt !
        else if( SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->
                                 GetItemState( RES_VERT_ORIENT, FALSE ))
        {
            SwFrm* pFrm = GetAnchor();
            // OD 10.10.2003 #112299# - do not try to connect to layout during
            // disconnection from layout.
            if ( !pFrm && !mbDisconnectInProgress )
            {
                ConnectToLayout();
                pFrm = GetAnchor();
            }
            if( pFrm && pFrm->IsTxtFrm() )
                ((SwTxtFrm*)pFrm)->Prepare();
        }
    }
    else if( RES_ANCHOR == nWhich )
        pAnch = (SwFmtAnchor*)pNew;

    if ( pAnch )
    {
        // JP 10.04.95: nicht auf ein Reset Anchor reagieren !!!!!
        if ( SFX_ITEM_SET == ((SwFrmFmt*)pRegisteredIn)->GetAttrSet().
            GetItemState( RES_ANCHOR, FALSE ) )
        {
            // OD 10.10.2003 #112299# - no connect to layout during disconnection
            if ( !mbDisconnectInProgress )
            {
                if( !FLY_IN_CNTNT == pAnch->GetAnchorId() )
                    ((SwFrmFmt*)pRegisteredIn)->ResetAttr( RES_VERT_ORIENT );

                SwFrm *pOldAnch = GetAnchor();
                SwPageFrm *pPg = NULL;
                SwRect aOldRect;
                if( pOldAnch )
                {
                    pPg = pOldAnch->FindPageFrm();
                    aOldRect = SwRect( GetMaster()->GetCurrentBoundRect() );
                }
                ConnectToLayout( pAnch );
                if( pPg && aOldRect.HasArea() )
                    Notify_Background(GetMaster(),pPg,aOldRect,PREP_FLY_LEAVE,TRUE);
                lcl_Notify( this, NULL );

                // #86973#
                if(GetMaster())
                {
                    GetMaster()->SetChanged();
                    GetMaster()->BroadcastObjectChange();
                }
            }
        }
        else
            DisconnectFromLayout();
    }
    else if( RES_SURROUND == nWhich || RES_UL_SPACE == nWhich ||
             RES_LR_SPACE == nWhich ||
             ( RES_ATTRSET_CHG == nWhich &&
               ( SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                            RES_SURROUND, FALSE ) ||
                 SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                            RES_LR_SPACE, FALSE ) ||
                 SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                            RES_UL_SPACE, FALSE ) ) ) )
        lcl_Notify( this, NULL );
}


/*************************************************************************
|*
|*	SwDrawContact::DisconnectFromLayout()
|*
|*	Ersterstellung		MA 09. Jan. 95
|*	Letzte Aenderung	MA 25. Mar. 99
|*
|*************************************************************************/

void SwDrawContact::DisconnectFromLayout( bool _bMoveMasterToInvisibleLayer )
{
    // OD 10.10.2003 #112299#
    mbDisconnectInProgress = true;

    // OD 16.05.2003 #108784# - remove 'virtual' drawing objects from writer
    // layout and from drawing page
    for ( std::list<SwDrawVirtObj*>::iterator aDisconnectIter = maDrawVirtObjs.begin();
          aDisconnectIter != maDrawVirtObjs.end();
          ++aDisconnectIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDisconnectIter);
        pDrawVirtObj->RemoveFromWriterLayout();
        pDrawVirtObj->RemoveFromDrawingPage();
    }

    if ( pAnchor )
    {
        pAnchor->RemoveDrawObj( this );
    }

    if ( _bMoveMasterToInvisibleLayer && GetMaster() && GetMaster()->IsInserted() )
    {
        SdrViewIter aIter( GetMaster() );
        for( SdrView* pView = aIter.FirstView(); pView;
                    pView = aIter.NextView() )
        {
            pView->MarkObj( GetMaster(), pView->GetPageViewPvNum(0), TRUE );
        }

        // OD 25.06.2003 #108784# - Instead of removing 'master' object from
        // drawing page, move the 'master' drawing object into the corresponding
        // invisible layer.
        {
            //((SwFrmFmt*)pRegisteredIn)->GetDoc()->GetDrawModel()->GetPage(0)->
            //                            RemoveObject( GetMaster()->GetOrdNum() );
            // OD 21.08.2003 #i18447# - in order to consider group object correct
            // use new method <SwDrawContact::MoveObjToInvisibleLayer(..)>
            MoveObjToInvisibleLayer( GetMaster() );
        }
    }

    // OD 10.10.2003 #112299#
    mbDisconnectInProgress = false;
}

// OD 26.06.2003 #108784# - method to remove 'master' drawing object
// from drawing page.
void SwDrawContact::RemoveMasterFromDrawPage()
{
    if ( GetMaster() )
    {
        GetMaster()->SetUserCall( 0 );
        if ( GetMaster()->IsInserted() )
        {
            ((SwFrmFmt*)pRegisteredIn)->GetDoc()->GetDrawModel()->GetPage(0)->
                                        RemoveObject( GetMaster()->GetOrdNum() );
        }
    }
}

// OD 19.06.2003 #108784# - disconnect for a dedicated drawing object -
// could be 'master' or 'virtual'.
// a 'master' drawing object will disconnect a 'virtual' drawing object
// in order to take its place.
// OD 13.10.2003 #i19919# - no special case, if drawing object isn't in
// page header/footer, in order to get drawing objects in repeating table headers
// also working.
void SwDrawContact::DisconnectObjFromLayout( SdrObject* _pDrawObj )
{
    if ( _pDrawObj->ISA(SwDrawVirtObj) )
    {
        SwDrawVirtObj* pDrawVirtObj = static_cast<SwDrawVirtObj*>(_pDrawObj);
        pDrawVirtObj->RemoveFromWriterLayout();
        pDrawVirtObj->RemoveFromDrawingPage();
    }
    else
    {
        std::list<SwDrawVirtObj*>::const_iterator aFoundVirtObjIter =
                std::find_if( maDrawVirtObjs.begin(), maDrawVirtObjs.end(),
                              UsedOrUnusedVirtObjPred( true ) );
        if ( aFoundVirtObjIter != maDrawVirtObjs.end() )
        {
            // replace found 'virtual' drawing object by 'master' drawing
            // object and disconnect the 'virtual' one
            SwDrawVirtObj* pDrawVirtObj = (*aFoundVirtObjIter);
            SwFrm* pNewAnchorFrmOfMaster = pDrawVirtObj->GetAnchorFrm();
            Point aNewAnchorPos = pDrawVirtObj->GetAnchorPos();
            // disconnect 'virtual' drawing object
            pDrawVirtObj->RemoveFromWriterLayout();
            pDrawVirtObj->RemoveFromDrawingPage();
            // disconnect 'master' drawing object from current frame
            pAnchor->RemoveDrawObj( this );
            // re-connect 'master' drawing object to frame of found 'virtual'
            // drawing object.
            pNewAnchorFrmOfMaster->AppendDrawObj( this );
            // set anchor position at 'master' virtual drawing object to the
            // one of the found 'virtual' drawing object.
            GetMaster()->SetAnchorPos( aNewAnchorPos );
            // restore positions of remaining 'virtual' drawing objects
            CorrectRelativePosOfVirtObjs();
        }
        else
        {
            // no connected 'virtual' drawing object found. Thus, disconnect
            // completely from layout.
            DisconnectFromLayout();
        }
    }
}

/*************************************************************************
|*
|*	SwDrawContact::ConnectToLayout()
|*
|*	Ersterstellung		MA 09. Jan. 95
|*	Letzte Aenderung	MA 25. Mar. 99
|*
|*************************************************************************/

void SwDrawContact::ConnectToLayout( const SwFmtAnchor* pAnch )
{
    // OD 10.10.2003 #112299# - *no* connect to layout during disconnection from
    // layout.
    if ( mbDisconnectInProgress )
    {
        ASSERT( false,
                "<SwDrawContact::ConnectToLayout(..)> called during disconnection.");
        return;
    }

    SwFrmFmt* pDrawFrmFmt = (SwFrmFmt*)pRegisteredIn;

    SwRootFrm* pRoot = pDrawFrmFmt->GetDoc()->GetRootFrm();
    if ( !pRoot )
    {
        return;
    }

    // OD 16.05.2003 #108784# - remove 'virtual' drawing objects from writer
    // layout and from drawing page, and remove 'master' drawing object from
    // writer layout - 'master' object will remain in drawing page.
    DisconnectFromLayout( false );

    if ( !pAnch )
    {
        pAnch = &(pDrawFrmFmt->GetAnchor());
    }

    bool bSetAnchorPos = true;

    switch ( pAnch->GetAnchorId() )
    {
        case FLY_PAGE:
            {
                USHORT nPgNum = pAnch->GetPageNum();
                SwPageFrm *pPage = static_cast<SwPageFrm*>(pRoot->Lower());

                for ( USHORT i = 1; i < nPgNum && pPage; ++i )
                {
                    pPage = static_cast<SwPageFrm*>(pPage->GetNext());
                }

                if ( pPage )
                {
                    bSetAnchorPos = false;
                    pPage->SwFrm::AppendDrawObj( this );
                }
                else
                    //Sieht doof aus, ist aber erlaubt (vlg. SwFEShell::SetPageObjsNewPage)
                    pRoot->SetAssertFlyPages();
            }
            break;

        case FLY_AUTO_CNTNT:
        case FLY_AT_CNTNT:
        case FLY_AT_FLY:
        case FLY_IN_CNTNT:
            {
                if ( pAnch->GetAnchorId() == FLY_IN_CNTNT )
                {
                    ClrContourCache( GetMaster() );
                }
                // OD 16.05.2003 #108784# - support drawing objects in header/footer,
                // but not control objects:
                // anchor at first found frame the 'master' object and
                // at the following frames 'virtual' drawing objects.
                // Note: method is similar to <SwFlyFrmFmt::MakeFrms(..)>
                SwModify *pModify = 0;
                if( pAnch->GetCntntAnchor() )
                {
                    if ( pAnch->GetAnchorId() == FLY_AT_FLY )
                    {
                        SwNodeIndex aIdx( pAnch->GetCntntAnchor()->nNode );
                        SwCntntNode* pCNd = pDrawFrmFmt->GetDoc()->GetNodes().GoNext( &aIdx );
                        SwClientIter aIter( *pCNd );
                        if ( aIter.First( TYPE(SwFrm) ) )
                            pModify = pCNd;
                        else
                        {
                            const SwNodeIndex& rIdx = pAnch->GetCntntAnchor()->nNode;
                            SwSpzFrmFmts& rFmts = *(pDrawFrmFmt->GetDoc()->GetSpzFrmFmts());
                            for( sal_uInt16 i = 0; i < rFmts.Count(); ++i )
                            {
                                SwFrmFmt* pFlyFmt = rFmts[i];
                                if( pFlyFmt->GetCntnt().GetCntntIdx() &&
                                    rIdx == *(pFlyFmt->GetCntnt().GetCntntIdx()) )
                                {
                                    pModify = pFlyFmt;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        pModify = pAnch->GetCntntAnchor()->nNode.GetNode().GetCntntNode();
                    }
                }
                SwClientIter aIter( *pModify );
                SwFrm* pAnchorFrmOfMaster = 0;
                for( SwFrm *pFrm = (SwFrm*)aIter.First( TYPE(SwFrm) );
                     pFrm;
                     pFrm = (SwFrm*)aIter.Next() )
                {
                    // append drawing object, if
                    // (1) proposed anchor frame isn't a follow and
                    // (2) drawing object isn't a control object to be anchored
                    //     in header/footer.
                    const bool bAdd = ( !pFrm->IsCntntFrm() ||
                                        !((SwCntntFrm*)pFrm)->IsFollow() ) &&
                                      ( !::CheckControlLayer( GetMaster() ) ||
                                        !pFrm->FindFooterOrHeader() );

                    if( bAdd )
                    {
                        if ( FLY_AT_FLY == pAnch->GetAnchorId() && !pFrm->IsFlyFrm() )
                        {
                            pFrm = pFrm->FindFlyFrm();
                        }

                        if ( !pAnchorFrmOfMaster )
                        {
                            // OD 02.07.2003 #108784# - check, if 'master' drawing
                            // objects is inserted into drawing page. If not,
                            // assert and insert 'master' drawing object.
                            if ( !GetMaster()->IsInserted() )
                            {
                                // OD 25.06.2003 #108784# - debug assert
                                ASSERT( false, "<SwDrawContact::ConnectToLayout(..)> - master drawing object not inserted!?" );

                                pDrawFrmFmt->GetDoc()->GetDrawModel()->GetPage(0)->
                                    InsertObject( GetMaster(), GetMaster()->GetOrdNumDirect() );
                            }
                            // append 'master' drawing object
                            pAnchorFrmOfMaster = pFrm;
                            pFrm->AppendDrawObj( this );
                        }
                        else
                        {
                            // append 'virtual' drawing object
                            SwDrawVirtObj* pDrawVirtObj = AddVirtObj();
                            if ( pAnch->GetAnchorId() == FLY_IN_CNTNT )
                            {
                                ClrContourCache( pDrawVirtObj );
                            }
                            pFrm->AppendVirtDrawObj( this, pDrawVirtObj );

                            // for repaint, use new ActionChanged()
                            // pDrawVirtObj->SendRepaintBroadcast();
                            pDrawVirtObj->ActionChanged();
                        }

                        if ( pAnch->GetAnchorId() == FLY_IN_CNTNT )
                        {
                            pFrm->InvalidatePrt();
                        }

                        bSetAnchorPos = false;
                    }
                }
            }
            break;
/*
        case FLY_AT_FLY:
            {
                if( pAnch->GetCntntAnchor() ) // LAYER_IMPL
                {
                    SwFrm *pAnchor = 0;
                    //Erst einmal ueber den Inhalt suchen, weil konstant schnell. Kann
                    //Bei verketteten Rahmen aber auch schief gehen, weil dann evtl.
                    //niemals ein Frame zu dem Inhalt existiert. Dann muss leider noch
                    //die Suche vom StartNode zum FrameFormat sein.
                    SwNodeIndex aIdx( pAnch->GetCntntAnchor()->nNode );
                    SwCntntNode *pCNd = pFmt->GetDoc()->GetNodes().GoNext( &aIdx );
                    if ( pCNd && 0 != (pAnchor = pCNd->GetFrm( 0, 0, FALSE ) ) )
                        pAnchor = pAnchor->FindFlyFrm();
                    else
                    {
                        const SwNodeIndex &rIdx = pAnch->GetCntntAnchor()->nNode;
                        SwSpzFrmFmts& rFmts = *pFmt->GetDoc()->GetSpzFrmFmts();
                        for( USHORT i = 0; i < rFmts.Count(); ++i )
                        {
                            SwFrmFmt *pFmt = rFmts[i];
                            SwFlyFrmFmt* pFlyFmt;
                            if( 0 != (pFlyFmt = PTR_CAST( SwFlyFrmFmt, pFmt )) &&
                                pFlyFmt->GetCntnt().GetCntntIdx() && //#57390#, Reader
                                rIdx == *pFlyFmt->GetCntnt().GetCntntIdx() )
                            {
                                pAnchor = pFlyFmt->GetFrm( 0, FALSE );
                                break;
                            }
                        }
                    }
                    if ( pAnchor )	//Kann sein, dass der Anker noch nicht existiert
                    {
                        pAnchor->FindFlyFrm()->AppendDrawObj( this );
                        bSetAnchorPos = false;
                    }
                }
            }
            break;
*/
/*
        case FLY_IN_CNTNT:
            {
                ClrContourCache( GetMaster() );
                SwCntntNode *pNode = GetFmt()->GetDoc()->
                        GetNodes()[pAnch->GetCntntAnchor()->nNode]->GetCntntNode();
                SwCntntFrm *pCntnt = pNode->GetFrm( 0, 0, FALSE );
                if ( pCntnt )
                {
                    //Kann sein, dass der Anker noch nicht existiert
                    pCntnt->AppendDrawObj( this );
                    pCntnt->InvalidatePrt();
                }
                bSetAnchorPos = false;
            }
            break;
*/
#ifndef PRODUCT
        default:	ASSERT( FALSE, "Unknown Anchor." );
#endif
    }
    if ( GetAnchor() )
    {
        if( bSetAnchorPos )
        {
            GetMaster()->SetAnchorPos( GetAnchor()->GetFrmAnchorPos( ::HasWrap( GetMaster() ) ) );
    }
        // OD 21.08.2003 #i18447# - in order to consider group object correct
        // use new method <SwDrawContact::MoveObjToVisibleLayer(..)>
        MoveObjToVisibleLayer( GetMaster() );
    }
}

// OD 27.06.2003 #108784# - insert 'master' drawing object into drawing page
void SwDrawContact::InsertMasterIntoDrawPage()
{
    if ( !GetMaster()->IsInserted() )
    {
        GetFmt()->GetDoc()->GetDrawModel()->GetPage(0)
                ->InsertObject( GetMaster(), GetMaster()->GetOrdNumDirect() );
    }
    GetMaster()->SetUserCall( this );
}

// OD 20.06.2003 #108784# - method in order to correct relative positioning of
// 'virtual' drawing objects, e.g. caused by a <NbcSetAnchorPos(..)> at the
// 'master' drawing object.
void SwDrawContact::CorrectRelativePosOfVirtObjs()
{
    for ( std::list<SwDrawVirtObj*>::iterator aDrawVirtObjIter = maDrawVirtObjs.begin();
          aDrawVirtObjIter != maDrawVirtObjs.end();
          ++aDrawVirtObjIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDrawVirtObjIter);
        pDrawVirtObj->AdjustRelativePosToReference();
    }
}

// OD 20.06.2003 #108784# - correct in-direct movements of the 'virtual'
// drawing objects, e.g. caused by a <NbcMove(..)> at the 'master' drawing object
void SwDrawContact::MoveOffsetOfVirtObjs( const Size& _rMoveSize )
{
    for ( std::list<SwDrawVirtObj*>::iterator aDrawVirtObjIter = maDrawVirtObjs.begin();
          aDrawVirtObjIter != maDrawVirtObjs.end();
          ++aDrawVirtObjIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDrawVirtObjIter);
        pDrawVirtObj->SetOffset( pDrawVirtObj->GetOffset() - Point(_rMoveSize.Width(), _rMoveSize.Height()) );
    }
}

// OD 26.06.2003 #108784# - invalidate anchor frames of 'virtual' drawing objects.
// Needed for, e.g., 'as character' anchored drawing objects and a change of the
// 'master' drawing object.
void SwDrawContact::InvalidateAnchorOfVirtObjs()
{
    for ( std::list<SwDrawVirtObj*>::iterator aDrawVirtObjIter = maDrawVirtObjs.begin();
          aDrawVirtObjIter != maDrawVirtObjs.end();
          ++aDrawVirtObjIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDrawVirtObjIter);
        if ( pDrawVirtObj->GetAnchorFrm() )
        {
            pDrawVirtObj->GetAnchorFrm()->Prepare();
        }
    }
}

/*************************************************************************
|*
|*	SwDrawContact::FindPage(), ChkPage()
|*
|*	Ersterstellung		MA 21. Mar. 95
|*	Letzte Aenderung	MA 19. Jul. 96
|*
|*************************************************************************/

SwPageFrm* SwDrawContact::FindPage( const SwRect &rRect )
{
    SwPageFrm *pPg = pPage;
    if ( !pPg && pAnchor )
        pPg = pAnchor->FindPageFrm();
    if ( pPg )
        pPg = (SwPageFrm*)::FindPage( rRect, pPg );
    return pPg;
}

void SwDrawContact::ChkPage()
{
    // OD 10.10.2003 #112299#
    if ( mbDisconnectInProgress )
    {
        ASSERT( false,
                "<SwDrawContact::ChkPage()> called during disconnection." );
        return;
    }

    SwPageFrm* pPg = pAnchor && pAnchor->IsPageFrm() ?
        pPage : FindPage( GetMaster()->GetCurrentBoundRect() );
    if ( pPage != pPg )
    {
        // OD 27.06.2003 #108784# - if drawing object is anchor in header/footer
        // a change of the page is a dramatic change. Thus, completely re-connect
        // to the layout
        if ( pAnchor && pAnchor->FindFooterOrHeader() )
        {
            ConnectToLayout();
        }
        else
        {
            if ( pPage )
                pPage->SwPageFrm::RemoveDrawObj( this );
            pPg->SwPageFrm::AppendDrawObj( this );
            ChgPage( pPg );
        }
    }
}

/*************************************************************************
|*
|*	SwDrawContact::ChangeMasterObject()
|*
|*	Ersterstellung		MA 07. Aug. 95
|*	Letzte Aenderung	MA 20. Apr. 99
|*
|*************************************************************************/
// OD 10.07.2003 #110742# - Important note:
// method is called by method <SwDPage::ReplaceObject(..)>, which called its
// corresponding superclass method <FmFormPage::ReplaceObject(..)>.
// Note: 'master' drawing object *has* to be connected to layout (GetAnchor()
//       returns a frame), if method is called.
void SwDrawContact::ChangeMasterObject( SdrObject *pNewMaster )
{
    const SwFrm& rAnchorFrmOfMaster = *GetAnchor();

    DisconnectFromLayout( false );
    // OD 10.07.2003 #110742# - consider 'virtual' drawing objects
    RemoveAllVirtObjs();

    GetMaster()->SetUserCall( 0 );
    SetMaster( pNewMaster );
    GetMaster()->SetUserCall( this );

    Point aNewAnchor( rAnchorFrmOfMaster.GetFrmAnchorPos( ::HasWrap( GetMaster() ) ) );
    GetMaster()->NbcSetRelativePos( GetMaster()->GetSnapRect().TopLeft() -
                                    aNewAnchor );
    GetMaster()->NbcSetAnchorPos( aNewAnchor );

    // OD 10.07.2003 #110742# - connecting to layout not needed, because the
    // connection to the layout has to be done by the caller.
    // In current implementation this is <SwDPage::ReplaceObject(..)>, which
    // causes an <SDRUSERCALL_INSERTED>. This takes care of the connection to
    // layout.
    /*
    //Hier wird der neue Master ggf. in die Page eingefuegt, was das Drawing
    //aber gar nicht gut haben kann. Deshalb nehmen wir das Objekt hinterher
    //gleich wieder aus der Seite heraus.
    FASTBOOL bInserted = pNewMaster->IsInserted();
    ConnectToLayout();
    if ( !bInserted && pNewMaster->IsInserted() )
        ((SwFrmFmt*)pRegisteredIn)->GetDoc()->GetDrawModel()->GetPage(0)->
                                    RemoveObject( GetMaster()->GetOrdNum() );
    */
}

/** method to move drawing object to corresponding visible layer

    OD 21.08.2003 #i18447#

    @author OD
*/
void SwDrawContact::MoveObjToVisibleLayer( SdrObject* _pDrawObj )
{
    _MoveObjToLayer( true, _pDrawObj );
}

/** method to move drawing object to corresponding invisible layer

    OD 21.08.2003 #i18447#

    @author OD
*/
void SwDrawContact::MoveObjToInvisibleLayer( SdrObject* _pDrawObj )
{
    _MoveObjToLayer( false, _pDrawObj );
}

/** method to move object to visible/invisible layer

    OD 21.08.2003 #i18447#
    implementation for the public method <MoveObjToVisibleLayer(..)>
    and <MoveObjToInvisibleLayer(..)>

    @author OD
*/
void SwDrawContact::_MoveObjToLayer( const bool _bToVisible,
                                     SdrObject* _pDrawObj )
{
    if ( !_pDrawObj )
    {
        ASSERT( false, "SwDrawContact::_MoveObjToLayer(..) - no drawing object!" );
        return;
    }

    if ( !pRegisteredIn )
    {
        ASSERT( false, "SwDrawContact::_MoveObjToLayer(..) - no drawing frame format!" );
        return;
    }

    SwDoc* pWriterDoc = static_cast<SwFrmFmt*>(pRegisteredIn)->GetDoc();
    if ( !pWriterDoc )
    {
        ASSERT( false, "SwDrawContact::_MoveObjToLayer(..) - no writer document!" );
        return;
    }

    SdrLayerID nToHellLayerId =
        _bToVisible ? pWriterDoc->GetHellId() : pWriterDoc->GetInvisibleHellId();
    SdrLayerID nToHeavenLayerId =
        _bToVisible ? pWriterDoc->GetHeavenId() : pWriterDoc->GetInvisibleHeavenId();
    SdrLayerID nToControlLayerId =
        _bToVisible ? pWriterDoc->GetControlsId() : pWriterDoc->GetInvisibleControlsId();
    SdrLayerID nFromHellLayerId =
        _bToVisible ? pWriterDoc->GetInvisibleHellId() : pWriterDoc->GetHellId();
    SdrLayerID nFromHeavenLayerId =
        _bToVisible ? pWriterDoc->GetInvisibleHeavenId() : pWriterDoc->GetHeavenId();
    SdrLayerID nFromControlLayerId =
        _bToVisible ? pWriterDoc->GetInvisibleControlsId() : pWriterDoc->GetControlsId();

    if ( _pDrawObj->ISA( SdrObjGroup ) )
    {
        // determine layer for group object
        {
            // proposed layer of a group object is the hell layer
            SdrLayerID nNewLayerId = nToHellLayerId;
            if ( ::CheckControlLayer( _pDrawObj ) )
            {
                // it has to be the control layer, if one of the member
                // is a control
                nNewLayerId = nToControlLayerId;
            }
            else if ( _pDrawObj->GetLayer() == pWriterDoc->GetHeavenId() ||
                      _pDrawObj->GetLayer() == pWriterDoc->GetInvisibleHeavenId() )
            {
                // it has to be the heaven layer, if method <GetLayer()> reveals
                // a heaven layer
                nNewLayerId = nToHeavenLayerId;
            }
            // set layer at group object, but do *not* broadcast and
            // no propagation to the members.
            // Thus, call <NbcSetLayer(..)> at super class
            _pDrawObj->SdrObject::NbcSetLayer( nNewLayerId );
        }

        // call method recursively for group object members
        const SdrObjList* pLst =
                static_cast<SdrObjGroup*>(_pDrawObj)->GetSubList();
        if ( pLst )
        {
            for ( USHORT i = 0; i < pLst->GetObjCount(); ++i )
            {
                _MoveObjToLayer( _bToVisible, pLst->GetObj( i ) );
            }
        }
    }
    else
    {
        const SdrLayerID nLayerIdOfObj = _pDrawObj->GetLayer();
        if ( nLayerIdOfObj == nFromHellLayerId )
        {
            _pDrawObj->SetLayer( nToHellLayerId );
        }
        else if ( nLayerIdOfObj == nFromHeavenLayerId )
        {
            _pDrawObj->SetLayer( nToHeavenLayerId );
        }
        else if ( nLayerIdOfObj == nFromControlLayerId )
        {
            _pDrawObj->SetLayer( nToControlLayerId );
        }
    }
}

// =============================================================================
/** implementation of class <SwDrawVirtObj>

    OD 14.05.2003 #108784#

    @author OD
*/

TYPEINIT1(SwDrawVirtObj,SdrVirtObj);

// #108784#
// implemetation of SwDrawVirtObj
SwDrawVirtObj::SwDrawVirtObj( SdrObject&        _rNewObj,
                              SwDrawContact&    _rDrawContact )
    : SdrVirtObj( _rNewObj ),
      mpAnchorFrm( 0 ),
      mpPageFrm( 0 ),
      mrDrawContact( _rDrawContact )
{}

SwDrawVirtObj::SwDrawVirtObj( SdrObject&        _rNewObj,
                              const Point&      _rAnchorPos,
                              SwDrawContact&    _rDrawContact )
    : SdrVirtObj( _rNewObj, _rAnchorPos ),
      mpAnchorFrm( 0 ),
      mpPageFrm( 0 ),
      mrDrawContact( _rDrawContact )
{}

SwDrawVirtObj::~SwDrawVirtObj()
{}

void SwDrawVirtObj::SetOffset(const Point& rNewOffset)
{
    maOffset = rNewOffset;
    SetRectsDirty();
}

const Point SwDrawVirtObj::GetOffset() const
{
    return maOffset;
}

void SwDrawVirtObj::operator=( const SdrObject& rObj )
{

    SdrVirtObj::operator=(rObj);

    // copy local offset
    if ( rObj.ISA(SwDrawVirtObj) )
    {
        maOffset = static_cast<const SwDrawVirtObj&>(rObj).maOffset;
    }
}

SdrObject* SwDrawVirtObj::Clone() const
{
    SwDrawVirtObj* pObj = new SwDrawVirtObj( rRefObj, mrDrawContact );

    if ( pObj )
    {
        pObj->operator=(static_cast<const SdrObject&>(*this));
    //*pObj = *this;

        // members <mpAnchorFrm> and <mpPageFrm> not cloned.
    }

    return pObj;
}

// --------------------------------------------------------------------
// connection to writer layout: <SetAnchorFrm(..)>, <GetAnchorFrm()>,
// <SetPageFrm(..)>, <GetPageFrm()> and <RemoveFromWriterLayout()>
// --------------------------------------------------------------------
void SwDrawVirtObj::SetAnchorFrm( SwFrm* _pNewAnchorFrm )
{
    mpAnchorFrm = _pNewAnchorFrm;
}

SwFrm* SwDrawVirtObj::GetAnchorFrm() const
{
    return mpAnchorFrm;
}

void SwDrawVirtObj::SetPageFrm( SwPageFrm* _pNewPageFrm )
{
    mpPageFrm = _pNewPageFrm;
}

SwPageFrm* SwDrawVirtObj::GetPageFrm() const
{
    return mpPageFrm;
}

void SwDrawVirtObj::RemoveFromWriterLayout()
{
    // remove contact object from frame for 'virtual' drawing object
    if ( GetAnchorFrm() )
    {
        GetAnchorFrm()->RemoveVirtDrawObj( &mrDrawContact, this );
    }
}

// --------------------------------------------------------------------
// connection to writer layout: <AddToDrawingPage()>, <RemoveFromDrawingPage()>
// --------------------------------------------------------------------
void SwDrawVirtObj::AddToDrawingPage()
{
    // determine 'master'
    SdrObject* pOrgMasterSdrObj = mrDrawContact.GetMaster();

    // insert 'virtual' drawing object into page, set layer and user call.
    SdrPage *pPg;
    if ( 0 != ( pPg = pOrgMasterSdrObj->GetPage() ) )
    {
        pPg->InsertObject( this );
    }
    SetUserCall( &mrDrawContact );
}

void SwDrawVirtObj::RemoveFromDrawingPage()
{
    SetUserCall( 0 );
    if ( GetPage() )
    {
        GetPage()->RemoveObject( GetOrdNum() );
    }
}

// is 'virtual' drawing object connected to writer layout and  to drawing layer.
bool SwDrawVirtObj::IsConnected() const
{
    bool bRetVal = GetAnchorFrm() &&
                   ( GetPage() && GetUserCall() );

    return bRetVal;
}

// OD 19.06.2003 #108784# - adjust the relative postion of the 'virtual'
// drawing object to the reference object
void SwDrawVirtObj::AdjustRelativePosToReference()
{
    Point aVirtRelPos = GetRelativePos();
    Point aOrigialRelativePos = GetReferencedObj().GetRelativePos();
    Point aDiff = aVirtRelPos - aOrigialRelativePos;
    if( aDiff.X() || aDiff.Y() )
    {
        SetOffset( GetOffset() - aDiff );
    }
}

// #108784#
// All AnchorPos influencing methods which shall not change the
// original object positions.

void SwDrawVirtObj::NbcSetRelativePos(const Point& rPnt)
{
    ReferencedObj().NbcSetRelativePos( rPnt );
}

void SwDrawVirtObj::NbcSetAnchorPos(const Point& rPnt)
{
    Point aDelta(rPnt - aAnchor);

    aAnchor = rPnt;
    maOffset += aDelta;
    SetRectsDirty();
}

// #108784#
// All overloaded methods which need to use the offset to move
// the object position virtually.

const Rectangle& SwDrawVirtObj::GetCurrentBoundRect() const
{
    ((SwDrawVirtObj*)this)->aOutRect = rRefObj.GetCurrentBoundRect(); // Hier noch optimieren
    ((SwDrawVirtObj*)this)->aOutRect += maOffset;

    return aOutRect;
}

const Rectangle& SwDrawVirtObj::GetLastBoundRect() const
{
    ((SwDrawVirtObj*)this)->aOutRect = rRefObj.GetLastBoundRect(); // Hier noch optimieren
    ((SwDrawVirtObj*)this)->aOutRect += maOffset;

    return aOutRect;
}

void SwDrawVirtObj::RecalcBoundRect()
{
    aOutRect = rRefObj.GetCurrentBoundRect();
    aOutRect += maOffset;
}

sal_Bool SwDrawVirtObj::DoPaintObject(ExtOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const
{
    FASTBOOL bRet;

    Point aOfs(rOut.GetOffset());
    rOut.SetOffset(aOfs + maOffset);
    if ( GetSubList() && GetSubList()->GetObjCount() )
    {
        SdrPaintInfoRec aCopyInfoRec = rInfoRec;
        aCopyInfoRec.aCheckRect.Move( -maOffset.X(), -maOffset.Y() );
        aCopyInfoRec.aDirtyRect.Move( -maOffset.X(), -maOffset.Y() );
        bRet = rRefObj.DoPaintObject(rOut, aCopyInfoRec);
    }
    else
    {
        bRet = rRefObj.DoPaintObject(rOut, rInfoRec);
    }
    rOut.SetOffset(aOfs);

    return bRet;
}

SdrObject* SwDrawVirtObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    Point aPnt(rPnt - maOffset);
    FASTBOOL bRet = rRefObj.CheckHit(aPnt, nTol, pVisiLayer) != NULL;

    return bRet ? (SdrObject*)this : NULL;
}

void SwDrawVirtObj::TakeXorPoly(XPolyPolygon& rPoly, FASTBOOL bDetail) const
{
    rRefObj.TakeXorPoly(rPoly, bDetail);
    rPoly.Move(maOffset.X(), maOffset.Y());
}

void SwDrawVirtObj::TakeContour(XPolyPolygon& rPoly) const
{
    rRefObj.TakeContour(rPoly);
    rPoly.Move(maOffset.X(), maOffset.Y());
}

SdrHdl* SwDrawVirtObj::GetHdl(USHORT nHdlNum) const
{
    SdrHdl* pHdl = rRefObj.GetHdl(nHdlNum);
    Point aP(pHdl->GetPos() + maOffset);
    pHdl->SetPos(aP);

    return pHdl;
}

SdrHdl* SwDrawVirtObj::GetPlusHdl(const SdrHdl& rHdl, USHORT nPlNum) const
{
    SdrHdl* pHdl = rRefObj.GetPlusHdl(rHdl, nPlNum);
    pHdl->SetPos(pHdl->GetPos() + maOffset);

    return pHdl;
}

void SwDrawVirtObj::NbcMove(const Size& rSiz)
{
    rRefObj.NbcMove( rSiz );
    SetRectsDirty();
}

void SwDrawVirtObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    rRefObj.NbcResize(rRef - maOffset, xFact, yFact);
    SetRectsDirty();
}

void SwDrawVirtObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
    rRefObj.NbcRotate(rRef - maOffset, nWink, sn, cs);
    SetRectsDirty();
}

void SwDrawVirtObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    rRefObj.NbcMirror(rRef1 - maOffset, rRef2 - maOffset);
    SetRectsDirty();
}

void SwDrawVirtObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    rRefObj.NbcShear(rRef - maOffset, nWink, tn, bVShear);
    SetRectsDirty();
}

void SwDrawVirtObj::Move(const Size& rSiz)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    rRefObj.Move( rSiz );
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    if(xFact.GetNumerator() != xFact.GetDenominator() || yFact.GetNumerator() != yFact.GetDenominator())
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Resize(rRef - maOffset, xFact, yFact);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::Rotate(const Point& rRef, long nWink, double sn, double cs)
{
    if(nWink)
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Rotate(rRef - maOffset, nWink, sn, cs);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::Mirror(const Point& rRef1, const Point& rRef2)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    rRefObj.Mirror(rRef1 - maOffset, rRef2 - maOffset);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::Shear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    if(nWink)
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Shear(rRef - maOffset, nWink, tn, bVShear);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::RecalcSnapRect()
{
    aSnapRect = rRefObj.GetSnapRect();
    aSnapRect += maOffset;
}

const Rectangle& SwDrawVirtObj::GetSnapRect() const
{
    ((SwDrawVirtObj*)this)->aSnapRect = rRefObj.GetSnapRect();
    ((SwDrawVirtObj*)this)->aSnapRect += maOffset;

    return aSnapRect;
}

void SwDrawVirtObj::SetSnapRect(const Rectangle& rRect)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    Rectangle aR(rRect);
    aR -= maOffset;
    rRefObj.SetSnapRect(aR);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::NbcSetSnapRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR -= maOffset;
    SetRectsDirty();
    rRefObj.NbcSetSnapRect(aR);
}

const Rectangle& SwDrawVirtObj::GetLogicRect() const
{
    ((SwDrawVirtObj*)this)->aSnapRect = rRefObj.GetLogicRect();
    ((SwDrawVirtObj*)this)->aSnapRect += maOffset;

    return aSnapRect;
}

void SwDrawVirtObj::SetLogicRect(const Rectangle& rRect)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    Rectangle aR(rRect);
    aR -= maOffset;
    rRefObj.SetLogicRect(aR);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::NbcSetLogicRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR -= maOffset;
    rRefObj.NbcSetLogicRect(aR);
    SetRectsDirty();
}

Point SwDrawVirtObj::GetSnapPoint(USHORT i) const
{
    Point aP(rRefObj.GetSnapPoint(i));
    aP += maOffset;

    return aP;
}

const Point& SwDrawVirtObj::GetPoint(USHORT i) const
{
    ((SwDrawVirtObj*)this)->aHack = rRefObj.GetPoint(i);
    ((SwDrawVirtObj*)this)->aHack += maOffset;

    return aHack;
}

void SwDrawVirtObj::NbcSetPoint(const Point& rPnt, USHORT i)
{
    Point aP(rPnt);
    aP -= maOffset;
    rRefObj.SetPoint(aP, i);
    SetRectsDirty();
}

// #108784#
FASTBOOL SwDrawVirtObj::HasTextEdit() const
{
    return rRefObj.HasTextEdit();
}

// OD 18.06.2003 #108784# - overloaded 'layer' methods for 'virtual' drawing
// object to assure, that layer of 'virtual' object is the layer of the referenced
// object.
// NOTE: member <
SdrLayerID SwDrawVirtObj::GetLayer() const
{
    return GetReferencedObj().GetLayer();
}

void SwDrawVirtObj::NbcSetLayer(SdrLayerID nLayer)
{
    ReferencedObj().NbcSetLayer( nLayer );
    SdrVirtObj::NbcSetLayer( ReferencedObj().GetLayer() );
}

void SwDrawVirtObj::SetLayer(SdrLayerID nLayer)
{
    ReferencedObj().SetLayer( nLayer );
    SdrVirtObj::NbcSetLayer( ReferencedObj().GetLayer() );
}
// eof
