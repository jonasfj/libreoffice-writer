/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dcontact.cxx,v $
 * $Revision: 1.61.210.2 $
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
#include "hintids.hxx"
#include <svx/protitem.hxx>
#include <svx/opaqitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/svdpage.hxx>
#include <svx/fmglob.hxx>
#include <svx/svdogrp.hxx>
#include <svx/svdotext.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdpagv.hxx>
#include <svx/svdviter.hxx>
#include <svx/svdview.hxx>
#include <svx/shapepropertynotifier.hxx>
// AW, OD 2004-04-30 #i28501#
#include <svx/sdr/contact/objectcontactofobjlistpainter.hxx>
#include <svx/sdr/contact/displayinfo.hxx>
#include <fmtornt.hxx>
#include <viewimp.hxx>
#include <fmtsrnd.hxx>
#include <fmtanchr.hxx>
#include <node.hxx>
#include <fmtcntnt.hxx>
#include <pagefrm.hxx>
#include <rootfrm.hxx>
#include <frmtool.hxx>	// Notify_Background
#include <flyfrm.hxx>
#include <frmfmt.hxx>
#include <dflyobj.hxx>
#include <dcontact.hxx>
#include <unodraw.hxx>
#include <IDocumentDrawModelAccess.hxx>
#include <doc.hxx>
#include <hints.hxx>
#include <txtfrm.hxx>
#include <editsh.hxx>
#include <docary.hxx>

// OD 2004-02-11 #110582#-2
#include <flyfrms.hxx>

// OD 18.06.2003 #108784#
#include <algorithm>
// OD 2004-05-24 #i28701#
#include <sortedobjs.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

// AW: For VCOfDrawVirtObj and stuff
#include <svx/sdr/contact/viewcontactofvirtobj.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <svx/sdr/contact/viewobjectcontactofsdrobj.hxx>

#include <com/sun/star/text/WritingMode2.hpp>

using namespace ::com::sun::star;


TYPEINIT1( SwContact, SwClient )
TYPEINIT1( SwFlyDrawContact, SwContact )
TYPEINIT1( SwDrawContact, SwContact )

void setContextWritingMode( SdrObject* pObj, SwFrm* pAnchor )
{
    if( pObj && pAnchor )
    {
        short nWritingDirection = text::WritingMode2::LR_TB;
        if( pAnchor->IsVertical() )
        {
            nWritingDirection = text::WritingMode2::TB_RL;
        } else if( pAnchor->IsRightToLeft() )
        {
            nWritingDirection = text::WritingMode2::RL_TB;
        }
        pObj->SetContextWritingMode( nWritingDirection );
    }
}


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
/* SJ: after prior consultation with OD we decided to remove this Assertion
#if OSL_DEBUG_LEVEL > 1
    ASSERT( pRetval,
            "<::FindFrmFmt(..)> - no frame format found for given object. Please inform OD." );
#endif
*/
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

// --> OD 2006-08-15 #i68520# - change naming
SwRect GetBoundRectOfAnchoredObj( const SdrObject* pObj )
// <--
{
    SwRect aRet( pObj->GetCurrentBoundRect() );
    // --> OD 2006-08-10 #i68520# - call cache of <SwAnchoredObject>
    SwContact* pContact( GetUserCall( pObj ) );
    if ( pContact )
    {
        const SwAnchoredObject* pAnchoredObj( pContact->GetAnchoredObj( pObj ) );
        if ( pAnchoredObj )
        {
            aRet = pAnchoredObj->GetObjRectWithSpaces();
        }
    }
    // <--
    return aRet;
}

//Liefert den UserCall ggf. vom Gruppenobjekt
// OD 2004-03-31 #i26791# - change return type
SwContact* GetUserCall( const SdrObject* pObj )
{
    SdrObject *pTmp;
    while ( !pObj->GetUserCall() && 0 != (pTmp = pObj->GetUpGroup()) )
        pObj = pTmp;
    ASSERT( !pObj->GetUserCall() || pObj->GetUserCall()->ISA(SwContact),
            "<::GetUserCall(..)> - wrong type of found object user call." );
    return static_cast<SwContact*>(pObj->GetUserCall());
}

// liefert TRUE falls das SrdObject ein Marquee-Object (Lauftext) ist
BOOL IsMarqueeTextObj( const SdrObject& rObj )
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

SwContact::SwContact( SwFrmFmt *pToRegisterIn ) :
    SwClient( pToRegisterIn ),
    // OD 05.09.2003 #112039# - init member <mbInDTOR>
    mbInDTOR( false )
{}

SwContact::~SwContact()
{
    // OD 05.09.2003 #112039# - set <mbInDTOR>
    SetInDTOR();
}

// OD 05.09.2003 #112039# - accessor for member <mbInDTOR>
bool SwContact::IsInDTOR() const
{
    return mbInDTOR;
}

// OD 05.09.2003 #112039# - accessor to set member <mbInDTOR>
void SwContact::SetInDTOR()
{
    mbInDTOR = true;
}

/** method to move drawing object to corresponding visible layer

    OD 21.08.2003 #i18447#

    @author OD
*/
void SwContact::MoveObjToVisibleLayer( SdrObject* _pDrawObj )
{
    // --> OD 2005-06-08 #i46297# - notify background about the arriving of
    // the object and invalidate its position.
    const bool bNotify( !GetFmt()->getIDocumentDrawModelAccess()->IsVisibleLayerId( _pDrawObj->GetLayer() ) );
    // <--

    _MoveObjToLayer( true, _pDrawObj );

    // --> OD 2005-05-23 #i46297#
    if ( bNotify )
    {
        SwAnchoredObject* pAnchoredObj = GetAnchoredObj( _pDrawObj );
        ASSERT( pAnchoredObj,
                "<SwContact::MoveObjToInvisibleLayer(..)> - missing anchored object" );
        if ( pAnchoredObj )
        {
            ::setContextWritingMode( _pDrawObj, pAnchoredObj->GetAnchorFrmContainingAnchPos() );
            // Note: as-character anchored objects aren't registered at a page frame and
            //       a notification of its background isn't needed.
            if ( pAnchoredObj->GetPageFrm() )
            {
                ::Notify_Background( _pDrawObj, pAnchoredObj->GetPageFrm(),
                                     pAnchoredObj->GetObjRect(), PREP_FLY_ARRIVE, TRUE );
            }

            pAnchoredObj->InvalidateObjPos();
        }
    }
    // <--
}

/** method to move drawing object to corresponding invisible layer

    OD 21.08.2003 #i18447#

    @author OD
*/
void SwContact::MoveObjToInvisibleLayer( SdrObject* _pDrawObj )
{
    // --> OD 2005-06-08 #i46297# - notify background about the leaving of the object.
    const bool bNotify( GetFmt()->getIDocumentDrawModelAccess()->IsVisibleLayerId( _pDrawObj->GetLayer() ) );
    // <--

    _MoveObjToLayer( false, _pDrawObj );

    // --> OD 2005-05-19 #i46297#
    if ( bNotify )
    {
        SwAnchoredObject* pAnchoredObj = GetAnchoredObj( _pDrawObj );
        ASSERT( pAnchoredObj,
                "<SwContact::MoveObjToInvisibleLayer(..)> - missing anchored object" );
        // Note: as-character anchored objects aren't registered at a page frame and
        //       a notification of its background isn't needed.
        if ( pAnchoredObj && pAnchoredObj->GetPageFrm() )
        {
            ::Notify_Background( _pDrawObj, pAnchoredObj->GetPageFrm(),
                                 pAnchoredObj->GetObjRect(), PREP_FLY_LEAVE, TRUE );
        }
    }
    // <--
}

/** method to move object to visible/invisible layer

    OD 21.08.2003 #i18447#
    implementation for the public method <MoveObjToVisibleLayer(..)>
    and <MoveObjToInvisibleLayer(..)>

    @author OD
*/
void SwContact::_MoveObjToLayer( const bool _bToVisible,
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

    const IDocumentDrawModelAccess* pIDDMA = static_cast<SwFrmFmt*>(pRegisteredIn)->getIDocumentDrawModelAccess();
    if ( !pIDDMA )
    {
        ASSERT( false, "SwDrawContact::_MoveObjToLayer(..) - no writer document!" );
        return;
    }

    SdrLayerID nToHellLayerId =
        _bToVisible ? pIDDMA->GetHellId() : pIDDMA->GetInvisibleHellId();
    SdrLayerID nToHeavenLayerId =
        _bToVisible ? pIDDMA->GetHeavenId() : pIDDMA->GetInvisibleHeavenId();
    SdrLayerID nToControlLayerId =
        _bToVisible ? pIDDMA->GetControlsId() : pIDDMA->GetInvisibleControlsId();
    SdrLayerID nFromHellLayerId =
        _bToVisible ? pIDDMA->GetInvisibleHellId() : pIDDMA->GetHellId();
    SdrLayerID nFromHeavenLayerId =
        _bToVisible ? pIDDMA->GetInvisibleHeavenId() : pIDDMA->GetHeavenId();
    SdrLayerID nFromControlLayerId =
        _bToVisible ? pIDDMA->GetInvisibleControlsId() : pIDDMA->GetControlsId();

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
            else if ( _pDrawObj->GetLayer() == pIDDMA->GetHeavenId() ||
                      _pDrawObj->GetLayer() == pIDDMA->GetInvisibleHeavenId() )
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

// -------------------------------------------------------------------------
// OD 2004-01-16 #110582# - some virtual helper methods for information
// about the object (Writer fly frame resp. drawing object)

const SwIndex& SwContact::GetCntntAnchorIndex() const
{
    return GetCntntAnchor().nContent;
}

/** get minimum order number of anchored objects handled by with contact

    OD 2004-08-24 #110810#

    @author
*/
sal_uInt32 SwContact::GetMinOrdNum() const
{
    sal_uInt32 nMinOrdNum( SAL_MAX_UINT32 );

    std::vector< SwAnchoredObject* > aObjs;
    GetAnchoredObjs( aObjs );

    while ( !aObjs.empty() )
    {
        sal_uInt32 nTmpOrdNum = aObjs.back()->GetDrawObj()->GetOrdNum();

        if ( nTmpOrdNum < nMinOrdNum )
        {
            nMinOrdNum = nTmpOrdNum;
        }

        aObjs.pop_back();
    }

    ASSERT( nMinOrdNum != SAL_MAX_UINT32,
            "<SwContact::GetMinOrdNum()> - no order number found." );
    return nMinOrdNum;
}

/** get maximum order number of anchored objects handled by with contact

    OD 2004-08-24 #110810#

    @author
*/
sal_uInt32 SwContact::GetMaxOrdNum() const
{
    sal_uInt32 nMaxOrdNum( 0L );

    std::vector< SwAnchoredObject* > aObjs;
    GetAnchoredObjs( aObjs );

    while ( !aObjs.empty() )
    {
        sal_uInt32 nTmpOrdNum = aObjs.back()->GetDrawObj()->GetOrdNum();

        if ( nTmpOrdNum > nMaxOrdNum )
        {
            nMaxOrdNum = nTmpOrdNum;
        }

        aObjs.pop_back();
    }

    return nMaxOrdNum;
}
// -------------------------------------------------------------------------

/*************************************************************************
|*
|*	SwFlyDrawContact, Ctor und Dtor
|*
|*	Ersterstellung		OK 23.11.94 18:13
|*	Letzte Aenderung	MA 06. Apr. 95
|*
|*************************************************************************/

SwFlyDrawContact::SwFlyDrawContact( SwFlyFrmFmt *pToRegisterIn, SdrModel * ) :
    SwContact( pToRegisterIn )
{
    // OD 2004-04-01 #i26791# - class <SwFlyDrawContact> contains the 'master'
    // drawing object of type <SwFlyDrawObj> on its own.
    mpMasterObj = new SwFlyDrawObj;
    mpMasterObj->SetOrdNum( 0xFFFFFFFE );
    mpMasterObj->SetUserCall( this );
}

SwFlyDrawContact::~SwFlyDrawContact()
{
    if ( mpMasterObj )
    {
        mpMasterObj->SetUserCall( 0 );
        if ( mpMasterObj->GetPage() )
            mpMasterObj->GetPage()->RemoveObject( mpMasterObj->GetOrdNum() );
        delete mpMasterObj;
    }
}

// OD 2004-03-29 #i26791#
const SwAnchoredObject* SwFlyDrawContact::GetAnchoredObj( const SdrObject* _pSdrObj ) const
{
    ASSERT( _pSdrObj,
            "<SwFlyDrawContact::GetAnchoredObj(..)> - no object provided" );
    ASSERT( _pSdrObj->ISA(SwVirtFlyDrawObj),
            "<SwFlyDrawContact::GetAnchoredObj(..)> - wrong object type object provided" );
    ASSERT( GetUserCall( _pSdrObj ) == const_cast<SwFlyDrawContact*>(this),
            "<SwFlyDrawContact::GetAnchoredObj(..)> - provided object doesn't belongs to this contact" );

    const SwAnchoredObject* pRetAnchoredObj = 0L;

    if ( _pSdrObj && _pSdrObj->ISA(SwVirtFlyDrawObj) )
    {
        pRetAnchoredObj = static_cast<const SwVirtFlyDrawObj*>(_pSdrObj)->GetFlyFrm();
    }

    return pRetAnchoredObj;
}

SwAnchoredObject* SwFlyDrawContact::GetAnchoredObj( SdrObject* _pSdrObj )
{
    ASSERT( _pSdrObj,
            "<SwFlyDrawContact::GetAnchoredObj(..)> - no object provided" );
    ASSERT( _pSdrObj->ISA(SwVirtFlyDrawObj),
            "<SwFlyDrawContact::GetAnchoredObj(..)> - wrong object type provided" );
    ASSERT( GetUserCall( _pSdrObj ) == this,
            "<SwFlyDrawContact::GetAnchoredObj(..)> - provided object doesn't belongs to this contact" );

    SwAnchoredObject* pRetAnchoredObj = 0L;

    if ( _pSdrObj && _pSdrObj->ISA(SwVirtFlyDrawObj) )
    {
        pRetAnchoredObj = static_cast<SwVirtFlyDrawObj*>(_pSdrObj)->GetFlyFrm();
    }

    return pRetAnchoredObj;
}

const SdrObject* SwFlyDrawContact::GetMaster() const
{
    return mpMasterObj;
}

SdrObject* SwFlyDrawContact::GetMaster()
{
    return mpMasterObj;
}

void SwFlyDrawContact::SetMaster( SdrObject* _pNewMaster )
{
    ASSERT( _pNewMaster->ISA(SwFlyDrawObj),
            "<SwFlyDrawContact::SetMaster(..)> - wrong type of new master object" );
    mpMasterObj = static_cast<SwFlyDrawObj *>(_pNewMaster);
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
    SdrPage* pPg( 0L );
    if ( 0 != ( pPg = GetMaster()->GetPage() ) )
    {
        const UINT32 nOrdNum = GetMaster()->GetOrdNum();
        pPg->ReplaceObject( pDrawObj, nOrdNum );
    }
    // --> OD 2004-08-16 #i27030# - insert new <SwVirtFlyDrawObj> instance
    // into drawing page with correct order number
    else
    {
        GetFmt()->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage( 0 )->
                        InsertObject( pDrawObj, _GetOrdNumForNewRef( pFly ) );
    }
    // <--
    // --> OD 2004-12-13 #i38889# - assure, that new <SwVirtFlyDrawObj> instance
    // is in a visible layer.
    MoveObjToVisibleLayer( pDrawObj );
    // <--
    return pDrawObj;
}

/** method to determine new order number for new instance of <SwVirtFlyDrawObj>

    OD 2004-08-16 #i27030#
    Used in method <CreateNewRef(..)>

    @author OD
*/
sal_uInt32 SwFlyDrawContact::_GetOrdNumForNewRef( const SwFlyFrm* _pFlyFrm )
{
    sal_uInt32 nOrdNum( 0L );

    // search for another Writer fly frame registered at same frame format
    SwClientIter aIter( *GetFmt() );
    const SwFlyFrm* pFlyFrm( 0L );
    for ( pFlyFrm = (SwFlyFrm*)aIter.First( TYPE(SwFlyFrm) );
          pFlyFrm;
          pFlyFrm = (SwFlyFrm*)aIter.Next() )
    {
        if ( pFlyFrm != _pFlyFrm )
        {
            break;
        }
    }

    if ( pFlyFrm )
    {
        // another Writer fly frame found. Take its order number
        nOrdNum = pFlyFrm->GetVirtDrawObj()->GetOrdNum();
    }
    else
    {
        // no other Writer fly frame found. Take order number of 'master' object
        // --> OD 2004-11-11 #i35748# - use method <GetOrdNumDirect()> instead
        // of method <GetOrdNum()> to avoid a recalculation of the order number,
        // which isn't intended.
        nOrdNum = GetMaster()->GetOrdNumDirect();
        // <--
    }

    return nOrdNum;
}

/*************************************************************************
|*
|*	SwFlyDrawContact::Modify()
|*
|*	Ersterstellung		OK 08.11.94 10:21
|*	Letzte Aenderung	MA 06. Dec. 94
|*
|*************************************************************************/

void SwFlyDrawContact::Modify( SfxPoolItem *, SfxPoolItem * )
{
}

// OD 2004-01-16 #110582# - override method to control Writer fly frames,
// which are linked, and to assure that all objects anchored at/inside the
// Writer fly frame are also made visible.
void SwFlyDrawContact::MoveObjToVisibleLayer( SdrObject* _pDrawObj )
{
    ASSERT( _pDrawObj->ISA(SwVirtFlyDrawObj),
            "<SwFlyDrawContact::MoveObjToVisibleLayer(..)> - wrong SdrObject type -> crash" );

    if ( GetFmt()->getIDocumentDrawModelAccess()->IsVisibleLayerId( _pDrawObj->GetLayer() ) )
    {
        // nothing to do
        return;
    }

    SwFlyFrm* pFlyFrm = static_cast<SwVirtFlyDrawObj*>(_pDrawObj)->GetFlyFrm();

    // --> OD 2005-03-09 #i44464# - consider, that Writer fly frame content
    // already exists - (e.g. WW8 document is inserted into a existing document).
    if ( !pFlyFrm->Lower() )
    {
        pFlyFrm->InsertColumns();
        pFlyFrm->Chain( pFlyFrm->AnchorFrm() );
        pFlyFrm->InsertCnt();
    }
    if ( pFlyFrm->GetDrawObjs() )
    {
        for ( sal_uInt8 i = 0; i < pFlyFrm->GetDrawObjs()->Count(); ++i)
        {
            // --> OD 2004-07-01 #i28701# - consider type of objects in sorted object list.
            SdrObject* pObj = (*pFlyFrm->GetDrawObjs())[i]->DrawObj();
            SwContact* pContact = static_cast<SwContact*>(pObj->GetUserCall());
            pContact->MoveObjToVisibleLayer( pObj );
        }
    }

    // make fly frame visible
    SwContact::MoveObjToVisibleLayer( _pDrawObj );
}

// OD 2004-01-16 #110582# - override method to control Writer fly frames,
// which are linked, and to assure that all objects anchored at/inside the
// Writer fly frame are also made invisible.
void SwFlyDrawContact::MoveObjToInvisibleLayer( SdrObject* _pDrawObj )
{
    ASSERT( _pDrawObj->ISA(SwVirtFlyDrawObj),
            "<SwFlyDrawContact::MoveObjToInvisibleLayer(..)> - wrong SdrObject type -> crash" );

    if ( !GetFmt()->getIDocumentDrawModelAccess()->IsVisibleLayerId( _pDrawObj->GetLayer() ) )
    {
        // nothing to do
        return;
    }

    SwFlyFrm* pFlyFrm = static_cast<SwVirtFlyDrawObj*>(_pDrawObj)->GetFlyFrm();

    pFlyFrm->Unchain();
    pFlyFrm->DeleteCnt();
    if ( pFlyFrm->GetDrawObjs() )
    {
        for ( sal_uInt8 i = 0; i < pFlyFrm->GetDrawObjs()->Count(); ++i)
        {
            // --> OD 2004-07-01 #i28701# - consider type of objects in sorted object list.
            SdrObject* pObj = (*pFlyFrm->GetDrawObjs())[i]->DrawObj();
            SwContact* pContact = static_cast<SwContact*>(pObj->GetUserCall());
            pContact->MoveObjToInvisibleLayer( pObj );
        }
    }

    // make fly frame invisible
    SwContact::MoveObjToInvisibleLayer( _pDrawObj );
}

/** get data collection of anchored objects, handled by with contact

    OD 2004-08-23 #110810#

    @author
*/
void SwFlyDrawContact::GetAnchoredObjs( std::vector<SwAnchoredObject*>& _roAnchoredObjs ) const
{
    const SwFrmFmt* pFmt = GetFmt();

    SwClientIter aIter( *(const_cast<SwFrmFmt*>(pFmt)) );
    for( SwFlyFrm* pFlyFrm = (SwFlyFrm*)aIter.First( TYPE(SwFlyFrm) );
         pFlyFrm;
         pFlyFrm = (SwFlyFrm*)aIter.Next() )
    {
        _roAnchoredObjs.push_back( pFlyFrm );
    }
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

SwDrawContact::SwDrawContact( SwFrmFmt* pToRegisterIn, SdrObject* pObj ) :
    SwContact( pToRegisterIn ),
    maAnchoredDrawObj(),
    mbMasterObjCleared( false ),
    // OD 10.10.2003 #112299#
    mbDisconnectInProgress( false ),
    // --> OD 2006-01-18 #129959#
    mbUserCallActive( false ),
    // Note: value of <meEventTypeOfCurrentUserCall> isn't of relevance, because
    //       <mbUserCallActive> is FALSE.
    meEventTypeOfCurrentUserCall( SDRUSERCALL_MOVEONLY )
    // <--
{
    // clear vector containing 'virtual' drawing objects.
    maDrawVirtObjs.clear();

    // --> OD 2004-09-22 #i33909# - assure, that drawing object is inserted
    // in the drawing page.
    if ( !pObj->IsInserted() )
    {
        pToRegisterIn->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage(0)->
                                InsertObject( pObj, pObj->GetOrdNumDirect() );
    }
    // <--

    //Controls muessen immer im Control-Layer liegen. Das gilt auch fuer
    //Gruppenobjekte, wenn diese Controls enthalten.
    if ( ::CheckControlLayer( pObj ) )
    {
        // OD 25.06.2003 #108784# - set layer of object to corresponding invisible layer.
        pObj->SetLayer( pToRegisterIn->getIDocumentDrawModelAccess()->GetInvisibleControlsId() );
    }

    // OD 2004-03-29 #i26791#
    pObj->SetUserCall( this );
    maAnchoredDrawObj.SetDrawObj( *pObj );

    // if there already exists an SwXShape for the object, ensure it knows about us, and the SdrObject
    // FS 2009-04-07 #i99056#
    SwXShape::AddExistingShapeToFmt( *pObj );
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

    if ( !mbMasterObjCleared )
    {
        SdrObject* pObject = const_cast< SdrObject* >( maAnchoredDrawObj.GetDrawObj() );
        SdrObject::Free( pObject );
    }
}

// OD 2004-03-29 #i26791#
const SwAnchoredObject* SwDrawContact::GetAnchoredObj( const SdrObject* _pSdrObj ) const
{
    // handle default parameter value
    if ( !_pSdrObj )
    {
        _pSdrObj = GetMaster();
    }

    ASSERT( _pSdrObj,
            "<SwDrawContact::GetAnchoredObj(..)> - no object provided" );
    ASSERT( _pSdrObj->ISA(SwDrawVirtObj) ||
            ( !_pSdrObj->ISA(SdrVirtObj) && !_pSdrObj->ISA(SwDrawVirtObj) ),
            "<SwDrawContact::GetAnchoredObj(..)> - wrong object type object provided" );
    ASSERT( GetUserCall( _pSdrObj ) == const_cast<SwDrawContact*>(this) ||
            _pSdrObj == GetMaster(),
            "<SwDrawContact::GetAnchoredObj(..)> - provided object doesn't belongs to this contact" );

    const SwAnchoredObject* pRetAnchoredObj = 0L;

    if ( _pSdrObj )
    {
        if ( _pSdrObj->ISA(SwDrawVirtObj) )
        {
            pRetAnchoredObj = static_cast<const SwDrawVirtObj*>(_pSdrObj)->GetAnchoredObj();
        }
        else if ( !_pSdrObj->ISA(SdrVirtObj) && !_pSdrObj->ISA(SwDrawVirtObj) )
        {
            pRetAnchoredObj = &maAnchoredDrawObj;
        }
    }

    return pRetAnchoredObj;
}

SwAnchoredObject* SwDrawContact::GetAnchoredObj( SdrObject* _pSdrObj )
{
    // handle default parameter value
    if ( !_pSdrObj )
    {
        _pSdrObj = GetMaster();
    }

    ASSERT( _pSdrObj,
            "<SwDrawContact::GetAnchoredObj(..)> - no object provided" );
    ASSERT( _pSdrObj->ISA(SwDrawVirtObj) ||
            ( !_pSdrObj->ISA(SdrVirtObj) && !_pSdrObj->ISA(SwDrawVirtObj) ),
            "<SwDrawContact::GetAnchoredObj(..)> - wrong object type object provided" );
    ASSERT( GetUserCall( _pSdrObj ) == this || _pSdrObj == GetMaster(),
            "<SwDrawContact::GetAnchoredObj(..)> - provided object doesn't belongs to this contact" );

    SwAnchoredObject* pRetAnchoredObj = 0L;

    if ( _pSdrObj )
    {
        if ( _pSdrObj->ISA(SwDrawVirtObj) )
        {
            pRetAnchoredObj = static_cast<SwDrawVirtObj*>(_pSdrObj)->AnchoredObj();
        }
        else if ( !_pSdrObj->ISA(SdrVirtObj) && !_pSdrObj->ISA(SwDrawVirtObj) )
        {
            pRetAnchoredObj = &maAnchoredDrawObj;
        }
    }

    return pRetAnchoredObj;
}

const SdrObject* SwDrawContact::GetMaster() const
{
    return !mbMasterObjCleared
           ? maAnchoredDrawObj.GetDrawObj()
           : 0L;
}

SdrObject* SwDrawContact::GetMaster()
{
    return !mbMasterObjCleared
           ? maAnchoredDrawObj.DrawObj()
           : 0L;
}

// OD 16.05.2003 #108784# - overload <SwContact::SetMaster(..)> in order to
// assert, if the 'master' drawing object is replaced.
// OD 10.07.2003 #110742# - replace of master object correctly handled, if
// handled by method <SwDrawContact::ChangeMasterObject(..)>. Thus, assert
// only, if a debug level is given.
void SwDrawContact::SetMaster( SdrObject* _pNewMaster )
{
    if ( _pNewMaster )
    {
#if OSL_DEBUG_LEVEL > 1
        ASSERT( false, "debug notification - master replaced!" );
#endif
        maAnchoredDrawObj.SetDrawObj( *_pNewMaster );
    }
    else
    {
        mbMasterObjCleared = true;
    }
}

const SwFrm* SwDrawContact::GetAnchorFrm( const SdrObject* _pDrawObj ) const
{
    const SwFrm* pAnchorFrm = 0L;
    if ( !_pDrawObj ||
         _pDrawObj == GetMaster() ||
         ( !_pDrawObj->GetUserCall() &&
           GetUserCall( _pDrawObj ) == static_cast<const SwContact* const>(this) ) )
    {
        pAnchorFrm = maAnchoredDrawObj.GetAnchorFrm();
    }
    else if ( _pDrawObj->ISA(SwDrawVirtObj) )
    {
        pAnchorFrm = static_cast<const SwDrawVirtObj*>(_pDrawObj)->GetAnchorFrm();
    }
    else
    {
        ASSERT( false,
                "<SwDrawContact::GetAnchorFrm(..)> - unknown drawing object." )
    }

    return pAnchorFrm;
}
SwFrm* SwDrawContact::GetAnchorFrm( SdrObject* _pDrawObj )
{
    SwFrm* pAnchorFrm = 0L;
    if ( !_pDrawObj ||
         _pDrawObj == GetMaster() ||
         ( !_pDrawObj->GetUserCall() &&
           GetUserCall( _pDrawObj ) == this ) )
    {
        pAnchorFrm = maAnchoredDrawObj.AnchorFrm();
    }
    else
    {
        ASSERT( _pDrawObj->ISA(SwDrawVirtObj),
                "<SwDrawContact::GetAnchorFrm(..)> - unknown drawing object." )
        pAnchorFrm = static_cast<SwDrawVirtObj*>(_pDrawObj)->AnchorFrm();
    }

    return pAnchorFrm;
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

SwDrawContact::VirtObjAnchoredAtFrmPred::VirtObjAnchoredAtFrmPred(
                                                const SwFrm& _rAnchorFrm )
    : mpAnchorFrm( &_rAnchorFrm )
{
    if ( mpAnchorFrm->IsCntntFrm() )
    {
        const SwCntntFrm* pTmpFrm =
                            static_cast<const SwCntntFrm*>( mpAnchorFrm );
        while ( pTmpFrm->IsFollow() )
        {
            pTmpFrm = pTmpFrm->FindMaster();
        }
        mpAnchorFrm = pTmpFrm;
    }
}

// OD 2004-04-14 #i26791# - compare with master frame
bool SwDrawContact::VirtObjAnchoredAtFrmPred::operator() ( const SwDrawVirtObj* _pDrawVirtObj )
{
    const SwFrm* pObjAnchorFrm = _pDrawVirtObj->GetAnchorFrm();
    if ( pObjAnchorFrm && pObjAnchorFrm->IsCntntFrm() )
    {
        const SwCntntFrm* pTmpFrm =
                            static_cast<const SwCntntFrm*>( pObjAnchorFrm );
        while ( pTmpFrm->IsFollow() )
        {
            pTmpFrm = pTmpFrm->FindMaster();
        }
        pObjAnchorFrm = pTmpFrm;
    }

    return ( pObjAnchorFrm == mpAnchorFrm );
}

// OD 19.06.2003 #108784# - get drawing object ('master' or 'virtual') by frame.
SdrObject* SwDrawContact::GetDrawObjectByAnchorFrm( const SwFrm& _rAnchorFrm )
{
    SdrObject* pRetDrawObj = 0L;

    // OD 2004-04-14 #i26791# - compare master frames instead of direct frames
    const SwFrm* pProposedAnchorFrm = &_rAnchorFrm;
    if ( pProposedAnchorFrm->IsCntntFrm() )
    {
        const SwCntntFrm* pTmpFrm =
                            static_cast<const SwCntntFrm*>( pProposedAnchorFrm );
        while ( pTmpFrm->IsFollow() )
        {
            pTmpFrm = pTmpFrm->FindMaster();
        }
        pProposedAnchorFrm = pTmpFrm;
    }

    const SwFrm* pMasterObjAnchorFrm = GetAnchorFrm();
    if ( pMasterObjAnchorFrm && pMasterObjAnchorFrm->IsCntntFrm() )
    {
        const SwCntntFrm* pTmpFrm =
                            static_cast<const SwCntntFrm*>( pMasterObjAnchorFrm );
        while ( pTmpFrm->IsFollow() )
        {
            pTmpFrm = pTmpFrm->FindMaster();
        }
        pMasterObjAnchorFrm = pTmpFrm;
    }

    if ( pMasterObjAnchorFrm && pMasterObjAnchorFrm == pProposedAnchorFrm )
    {
        pRetDrawObj = GetMaster();
    }
    else
    {
        std::list<SwDrawVirtObj*>::const_iterator aFoundVirtObjIter =
                std::find_if( maDrawVirtObjs.begin(), maDrawVirtObjs.end(),
                              VirtObjAnchoredAtFrmPred( *pProposedAnchorFrm ) );

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
            // --> OD 2004-10-21 #i34640# - determine correct page frame
            SwPageFrm* pPage = pDrawVirtObj->AnchoredObj()->FindPageFrmOfAnchor();
            // <--
            if( pOldBoundRect && pPage )
            {
                SwRect aOldRect( *pOldBoundRect );
                aOldRect.Pos() += pDrawVirtObj->GetOffset();
                if( aOldRect.HasArea() )
                    ::Notify_Background( pDrawVirtObj, pPage,
                                         aOldRect, PREP_FLY_LEAVE,TRUE);
            }
            // --> OD 2004-10-21 #i34640# - include spacing for wrapping
            SwRect aRect( pDrawVirtObj->GetAnchoredObj()->GetObjRectWithSpaces() );
            // <--
            if( aRect.HasArea() )
            {
                // --> OD 2004-10-21 #i34640# - simplify
                SwPageFrm* pPg = (SwPageFrm*)::FindPage( aRect, pPage );
                // <--
                if ( pPg )
                    ::Notify_Background( pDrawVirtObj, pPg, aRect,
                                         PREP_FLY_ARRIVE, TRUE );
            }
            ::ClrContourCache( pDrawVirtObj );
        }
    }
}

// OD 2004-04-08 #i26791# - local method to notify the background for a drawing object
void lcl_NotifyBackgroundOfObj( SwDrawContact& _rDrawContact,
                                const SdrObject& _rObj,
                                const Rectangle* _pOldObjRect )
{
    // --> OD 2004-10-21 #i34640#
    SwAnchoredObject* pAnchoredObj =
        const_cast<SwAnchoredObject*>(_rDrawContact.GetAnchoredObj( &_rObj ));
    if ( pAnchoredObj && pAnchoredObj->GetAnchorFrm() )
    // <--
    {
        // --> OD 2004-10-21 #i34640# - determine correct page frame
        SwPageFrm* pPageFrm = pAnchoredObj->FindPageFrmOfAnchor();
        // <--
        if( _pOldObjRect && pPageFrm )
        {
            SwRect aOldRect( *_pOldObjRect );
            if( aOldRect.HasArea() )
            {
                // --> OD 2004-10-21 #i34640# - determine correct page frame
                SwPageFrm* pOldPageFrm = (SwPageFrm*)::FindPage( aOldRect, pPageFrm );
                // <--
                ::Notify_Background( &_rObj, pOldPageFrm, aOldRect,
                                     PREP_FLY_LEAVE, TRUE);
            }
        }
        // --> OD 2004-10-21 #i34640# - include spacing for wrapping
        SwRect aNewRect( pAnchoredObj->GetObjRectWithSpaces() );
        // <--
        if( aNewRect.HasArea() && pPageFrm )
        {
            pPageFrm = (SwPageFrm*)::FindPage( aNewRect, pPageFrm );
            ::Notify_Background( &_rObj, pPageFrm, aNewRect,
                                 PREP_FLY_ARRIVE, TRUE );
        }
        ClrContourCache( &_rObj );
    }
}

void SwDrawContact::Changed( const SdrObject& rObj,
                             SdrUserCallType eType,
                             const Rectangle& rOldBoundRect )
{
    // OD 2004-06-01 #i26791# - no event handling, if existing <ViewShell>
    // is in contruction
    SwDoc* pDoc = GetFmt()->GetDoc();
    if ( pDoc->GetRootFrm() &&
         pDoc->GetRootFrm()->GetCurrShell() &&
         pDoc->GetRootFrm()->GetCurrShell()->IsInConstructor() )
    {
        return;
    }

    // --> OD 2005-03-08 #i44339#
    // no event handling, if document is in destruction.
    // Exception: It's the SDRUSERCALL_DELETE event
    if ( pDoc->IsInDtor() && eType != SDRUSERCALL_DELETE )
    {
        return;
    }
    // <--

    //Action aufsetzen, aber nicht wenn gerade irgendwo eine Action laeuft.
    ViewShell *pSh = 0, *pOrg;
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
    _Changed( rObj, eType, &rOldBoundRect );    //Achtung, ggf. Suizid!

    if ( pSh )
        pDoc->GetRootFrm()->EndAllAction();
}

// --> OD 2006-01-18 #129959#
// helper class for method <SwDrawContact::_Changed(..)> for handling nested
// <SdrObjUserCall> events
class NestedUserCallHdl
{
    private:
        SwDrawContact* mpDrawContact;
        bool mbParentUserCallActive;
        SdrUserCallType meParentUserCallEventType;

    public:
        NestedUserCallHdl( SwDrawContact* _pDrawContact,
                           SdrUserCallType _eEventType )
            : mpDrawContact( _pDrawContact ),
              mbParentUserCallActive( _pDrawContact->mbUserCallActive ),
              meParentUserCallEventType( _pDrawContact->meEventTypeOfCurrentUserCall )
        {
            mpDrawContact->mbUserCallActive = true;
            mpDrawContact->meEventTypeOfCurrentUserCall = _eEventType;
        }

        ~NestedUserCallHdl()
        {
            if ( mpDrawContact )
            {
                mpDrawContact->mbUserCallActive = mbParentUserCallActive;
                mpDrawContact->meEventTypeOfCurrentUserCall = meParentUserCallEventType;
            }
        }

        void DrawContactDeleted()
        {
            mpDrawContact = 0;
        }

        bool IsNestedUserCall()
        {
            return mbParentUserCallActive;
        }

        void AssertNestedUserCall()
        {
            if ( IsNestedUserCall() )
            {
                bool bTmpAssert( true );
                // Currently its known, that a nested event SDRUSERCALL_RESIZE
                // could occur during parent user call SDRUSERCALL_INSERTED,
                // SDRUSERCALL_DELETE and SDRUSERCALL_RESIZE for edge objects.
                // Also possible are nested SDRUSERCALL_CHILD_RESIZE events for
                // edge objects
                // Thus, assert all other combinations
                if ( ( meParentUserCallEventType == SDRUSERCALL_INSERTED ||
                       meParentUserCallEventType == SDRUSERCALL_DELETE ||
                       meParentUserCallEventType == SDRUSERCALL_RESIZE ) &&
                     mpDrawContact->meEventTypeOfCurrentUserCall == SDRUSERCALL_RESIZE )
                {
                    bTmpAssert = false;
                }
                else if ( meParentUserCallEventType == SDRUSERCALL_CHILD_RESIZE &&
                          mpDrawContact->meEventTypeOfCurrentUserCall == SDRUSERCALL_CHILD_RESIZE )
                {
                    bTmpAssert = false;
                }

                if ( bTmpAssert )
                {
                    ASSERT( false,
                            "<SwDrawContact::_Changed(..)> - unknown nested <UserCall> event. This is serious, please inform OD." );
                }
            }
        }
};

// <--
//
// !!!ACHTUNG!!! The object may commit suicide!!!
//
void SwDrawContact::_Changed( const SdrObject& rObj,
                              SdrUserCallType eType,
                              const Rectangle* pOldBoundRect )
{
    // --> OD 2006-01-18 #129959#
    // suppress handling of nested <SdrObjUserCall> events
    NestedUserCallHdl aNestedUserCallHdl( this, eType );
    if ( aNestedUserCallHdl.IsNestedUserCall() )
    {
        aNestedUserCallHdl.AssertNestedUserCall();
        return;
    }
    // <--
    // OD 05.08.2002 #100843# - do *not* notify, if document is destructing
    // --> OD 2004-10-21 #i35912# - do *not* notify for as-character anchored
    // drawing objects.
    // --> OD 2004-11-11 #i35007#
    // improvement: determine as-character anchored object flag only once.
    const bool bAnchoredAsChar = ObjAnchoredAsChar();
    // <--
    const bool bNotify = !(GetFmt()->GetDoc()->IsInDtor()) &&
                         ( SURROUND_THROUGHT != GetFmt()->GetSurround().GetSurround() ) &&
                         !bAnchoredAsChar;
    // <--
    switch( eType )
    {
        case SDRUSERCALL_DELETE:
            {
                if ( bNotify )
                {
                    lcl_NotifyBackgroundOfObj( *this, rObj, pOldBoundRect );
                    // --> OD 2004-10-27 #i36181# - background of 'virtual'
                    // drawing objects have also been notified.
                    NotifyBackgrdOfAllVirtObjs( pOldBoundRect );
                    // <--
                }
                DisconnectFromLayout( false );
                SetMaster( NULL );
                delete this;
                // --> FME 2006-07-12 #i65784# Prevent memory corruption
                aNestedUserCallHdl.DrawContactDeleted();
                // <--
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
                    if ( bNotify )
                    {
                        lcl_NotifyBackgroundOfObj( *this, rObj, pOldBoundRect );
                    }
                }
                break;
            }
        case SDRUSERCALL_REMOVED:
            {
                if ( bNotify )
                {
                    lcl_NotifyBackgroundOfObj( *this, rObj, pOldBoundRect );
                }
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
        {
            // --> OD 2004-08-04 #i31698# - improvement:
            // get instance <SwAnchoredDrawObject> only once
            const SwAnchoredDrawObject* pAnchoredDrawObj =
                static_cast<const SwAnchoredDrawObject*>( GetAnchoredObj( &rObj ) );
            // <--
            // OD 2004-04-06 #i26791# - adjust positioning and alignment attributes,
            // if positioning of drawing object isn't in progress.
            // --> OD 2005-08-15 #i53320# - no adjust of positioning attributes,
            // if drawing object isn't positioned.
            if ( !pAnchoredDrawObj->IsPositioningInProgress() &&
                 !pAnchoredDrawObj->NotYetPositioned() )
            // <--
            {
                // --> OD 2004-09-29 #i34748# - If no last object rectangle is
                // provided by the anchored object, use parameter <pOldBoundRect>.
                const Rectangle& aOldObjRect = pAnchoredDrawObj->GetLastObjRect()
                                               ? *(pAnchoredDrawObj->GetLastObjRect())
                                               : *(pOldBoundRect);
                // <--
                // --> OD 2008-02-18 #i79400#
                // always invalidate object rectangle inclusive spaces
                pAnchoredDrawObj->InvalidateObjRectWithSpaces();
                // <--
                // --> OD 2005-01-28 #i41324# - notify background before
                // adjusting position
                if ( bNotify )
                {
                    // --> OD 2004-07-20 #i31573# - correction: Only invalidate
                    // background of given drawing object.
                    lcl_NotifyBackgroundOfObj( *this, rObj, &aOldObjRect );
                }
                // <--
                // --> OD 2004-08-04 #i31698# - determine layout direction
                // via draw frame format.
                SwFrmFmt::tLayoutDir eLayoutDir =
                                pAnchoredDrawObj->GetFrmFmt().GetLayoutDir();
                // <--
                // use geometry of drawing object
                SwRect aObjRect( rObj.GetSnapRect() );
                // If drawing object is a member of a group, the adjustment
                // of the positioning and the alignment attributes has to
                // be done for the top group object.
                if ( rObj.GetUpGroup() )
                {
                    const SdrObject* pGroupObj = rObj.GetUpGroup();
                    while ( pGroupObj->GetUpGroup() )
                    {
                        pGroupObj = pGroupObj->GetUpGroup();
                    }
                    // use geometry of drawing object
                    aObjRect = pGroupObj->GetSnapRect();
                }
                SwTwips nXPosDiff(0L);
                SwTwips nYPosDiff(0L);
                switch ( eLayoutDir )
                {
                    case SwFrmFmt::HORI_L2R:
                    {
                        nXPosDiff = aObjRect.Left() - aOldObjRect.Left();
                        nYPosDiff = aObjRect.Top() - aOldObjRect.Top();
                    }
                    break;
                    case SwFrmFmt::HORI_R2L:
                    {
                        nXPosDiff = aOldObjRect.Right() - aObjRect.Right();
                        nYPosDiff = aObjRect.Top() - aOldObjRect.Top();
                    }
                    break;
                    case SwFrmFmt::VERT_R2L:
                    {
                        nXPosDiff = aObjRect.Top() - aOldObjRect.Top();
                        nYPosDiff = aOldObjRect.Right() - aObjRect.Right();
                    }
                    break;
                    default:
                    {
                        ASSERT( false,
                                "<SwDrawContact::_Changed(..)> - unsupported layout direction" );
                    }
                }
                SfxItemSet aSet( GetFmt()->GetDoc()->GetAttrPool(),
                                 RES_VERT_ORIENT, RES_HORI_ORIENT, 0 );
                const SwFmtVertOrient& rVert = GetFmt()->GetVertOrient();
                if ( nYPosDiff != 0 )
                {

                    if ( rVert.GetRelationOrient() == text::RelOrientation::CHAR ||
                         rVert.GetRelationOrient() == text::RelOrientation::TEXT_LINE )
                    {
                        nYPosDiff = -nYPosDiff;
                    }
                    aSet.Put( SwFmtVertOrient( rVert.GetPos()+nYPosDiff,
                                               text::VertOrientation::NONE,
                                               rVert.GetRelationOrient() ) );
                }

                const SwFmtHoriOrient& rHori = GetFmt()->GetHoriOrient();
                if ( !bAnchoredAsChar && nXPosDiff != 0 )
                {
                    aSet.Put( SwFmtHoriOrient( rHori.GetPos()+nXPosDiff,
                                               text::HoriOrientation::NONE,
                                               rHori.GetRelationOrient() ) );
                }

                if ( nYPosDiff ||
                     ( !bAnchoredAsChar && nXPosDiff != 0 ) )
                {
                    GetFmt()->GetDoc()->SetFlyFrmAttr( *(GetFmt()), aSet );
                    // keep new object rectangle, to avoid multiple
                    // changes of the attributes by multiple event from
                    // the drawing layer - e.g. group objects and its members
                    // --> OD 2004-09-29 #i34748# - use new method
                    // <SwAnchoredDrawObject::SetLastObjRect(..)>.
                    const_cast<SwAnchoredDrawObject*>(pAnchoredDrawObj)
                                    ->SetLastObjRect( aObjRect.SVRect() );
                }
                else if ( aObjRect.SSize() != aOldObjRect.GetSize() )
                {
                    _InvalidateObjs();
                    // --> OD 2004-11-11 #i35007# - notify anchor frame
                    // of as-character anchored object
                    if ( bAnchoredAsChar )
                    {
                        const_cast<SwAnchoredDrawObject*>(pAnchoredDrawObj)
                            ->AnchorFrm()->Prepare( PREP_FLY_ATTR_CHG, GetFmt() );
                    }
                    // <--
                }
            }
            // --> OD 2006-01-18 #129959#
            // It reveals that the following code causes several defects -
            // on copying or on ungrouping a group shape containing edge objects.
            // Testing fix for #i53320# also reveal that the following code
            // isn't necessary.
//            // --> OD 2005-08-15 #i53320# - reset positioning attributes,
//            // if anchored drawing object isn't yet positioned.
//            else if ( pAnchoredDrawObj->NotYetPositioned() &&
//                      static_cast<const SwDrawFrmFmt&>(pAnchoredDrawObj->GetFrmFmt()).IsPosAttrSet() )
//            {
//                const_cast<SwDrawFrmFmt&>(
//                    static_cast<const SwDrawFrmFmt&>(pAnchoredDrawObj->GetFrmFmt()))
//                        .ResetPosAttr();
//            }
//            // <--
            // <--
        }
        break;
        case SDRUSERCALL_CHGATTR:
            if ( bNotify )
            {
                lcl_NotifyBackgroundOfObj( *this, rObj, pOldBoundRect );
            }
            break;
        default:
            break;
    }
}

namespace
{
    static const SwFmtAnchor* lcl_getAnchorFmt( const SfxPoolItem& _rItem )
    {
        USHORT nWhich = _rItem.Which();
        const SwFmtAnchor* pAnchorFmt = NULL;
        if ( RES_ATTRSET_CHG == nWhich )
        {
            static_cast<const SwAttrSetChg&>(_rItem).GetChgSet()->
                GetItemState( RES_ANCHOR, FALSE, (const SfxPoolItem**)&pAnchorFmt );
        }
        else if ( RES_ANCHOR == nWhich )
        {
            pAnchorFmt = &static_cast<const SwFmtAnchor&>(_rItem);
        }
        return pAnchorFmt;
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

    USHORT nWhich = pNew ? pNew->Which() : 0;
    const SwFmtAnchor* pNewAnchorFmt = pNew ? lcl_getAnchorFmt( *pNew ) : NULL;

    if ( pNewAnchorFmt )
    {
        // JP 10.04.95: nicht auf ein Reset Anchor reagieren !!!!!
        if ( SFX_ITEM_SET ==
                GetFmt()->GetAttrSet().GetItemState( RES_ANCHOR, FALSE ) )
        {
            // OD 10.10.2003 #112299# - no connect to layout during disconnection
            if ( !mbDisconnectInProgress )
            {
                // determine old object retangle of 'master' drawing object
                // for notification
                const Rectangle* pOldRect = 0L;
                Rectangle aOldRect;
                if ( GetAnchorFrm() )
                {
                    // --> OD 2004-10-27 #i36181# - include spacing in object
                    // rectangle for notification.
                    aOldRect = maAnchoredDrawObj.GetObjRectWithSpaces().SVRect();
                    pOldRect = &aOldRect;
                    // <--
                }
                // re-connect to layout due to anchor format change
                ConnectToLayout( pNewAnchorFmt );
                // notify background of drawing objects
                lcl_NotifyBackgroundOfObj( *this, *GetMaster(), pOldRect );
                NotifyBackgrdOfAllVirtObjs( pOldRect );

                const SwFmtAnchor* pOldAnchorFmt = pOld ? lcl_getAnchorFmt( *pOld ) : NULL;
                if ( !pOldAnchorFmt || ( pOldAnchorFmt->GetAnchorId() != pNewAnchorFmt->GetAnchorId() ) )
                {
                    ASSERT( maAnchoredDrawObj.DrawObj(), "SwDrawContact::Modify: no draw object here?" );
                    if ( maAnchoredDrawObj.DrawObj() )
                    {
                        // --> OD 2009-07-10 #i102752#
                        // assure that a ShapePropertyChangeNotifier exists
                        maAnchoredDrawObj.DrawObj()->notifyShapePropertyChange( ::svx::eTextShapeAnchorType );
                        // <--
                    }
                }
            }
        }
        else
            DisconnectFromLayout();
    }
    // --> OD 2006-03-17 #i62875# - revised fix for issue #124157#
    // no further notification, if not connected to Writer layout
    else if ( maAnchoredDrawObj.GetAnchorFrm() &&
              maAnchoredDrawObj.GetDrawObj()->GetUserCall() )
    {
        // --> OD 2004-07-01 #i28701# - on change of wrapping style, hell|heaven layer,
        // or wrapping style influence an update of the <SwSortedObjs> list,
        // the drawing object is registered in, has to be performed. This is triggered
        // by the 1st parameter of method call <_InvalidateObjs(..)>.
        if ( RES_SURROUND == nWhich ||
             RES_OPAQUE == nWhich ||
             RES_WRAP_INFLUENCE_ON_OBJPOS == nWhich ||
             ( RES_ATTRSET_CHG == nWhich &&
               ( SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                           RES_SURROUND, FALSE ) ||
                 SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                           RES_OPAQUE, FALSE ) ||
                 SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                           RES_WRAP_INFLUENCE_ON_OBJPOS, FALSE ) ) ) )
        {
            lcl_NotifyBackgroundOfObj( *this, *GetMaster(), 0L );
            NotifyBackgrdOfAllVirtObjs( 0L );
            _InvalidateObjs( true );
        }
        else if ( RES_UL_SPACE == nWhich || RES_LR_SPACE == nWhich ||
                  RES_HORI_ORIENT == nWhich || RES_VERT_ORIENT == nWhich ||
                  // --> OD 2004-07-01 #i28701# - add attribute 'Follow text flow'
                  RES_FOLLOW_TEXT_FLOW == nWhich ||
                  ( RES_ATTRSET_CHG == nWhich &&
                    ( SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                                RES_LR_SPACE, FALSE ) ||
                      SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                                RES_UL_SPACE, FALSE ) ||
                      SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                                RES_HORI_ORIENT, FALSE ) ||
                      SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                                RES_VERT_ORIENT, FALSE ) ||
                      SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState(
                                RES_FOLLOW_TEXT_FLOW, FALSE ) ) ) )
        {
            lcl_NotifyBackgroundOfObj( *this, *GetMaster(), 0L );
            NotifyBackgrdOfAllVirtObjs( 0L );
            _InvalidateObjs();
        }
        // --> OD 2004-10-26 #i35443#
        else if ( RES_ATTRSET_CHG == nWhich )
        {
            lcl_NotifyBackgroundOfObj( *this, *GetMaster(), 0L );
            NotifyBackgrdOfAllVirtObjs( 0L );
            _InvalidateObjs();
        }
        // <--
        else if ( RES_REMOVE_UNO_OBJECT == nWhich )
        {
            // nothing to do
        }
#if OSL_DEBUG_LEVEL > 1
        else
        {
            ASSERT( false,
                    "<SwDrawContact::Modify(..)> - unhandled attribute? - please inform od@openoffice.org" );
        }
#endif
    }

    // --> OD 2005-07-18 #i51474#
    GetAnchoredObj( 0L )->ResetLayoutProcessBools();
    // <--
}

// OD 2004-03-31 #i26791#
// --> OD 2004-07-01 #i28701# - added parameter <_bUpdateSortedObjsList>
void SwDrawContact::_InvalidateObjs( const bool _bUpdateSortedObjsList )
{
    // invalidate position of existing 'virtual' drawing objects
    for ( std::list<SwDrawVirtObj*>::iterator aDisconnectIter = maDrawVirtObjs.begin();
          aDisconnectIter != maDrawVirtObjs.end();
          ++aDisconnectIter )
    {
        SwDrawVirtObj* pDrawVirtObj = (*aDisconnectIter);
        // --> OD 2004-08-23 #i33313# - invalidation only for connected
        // 'virtual' drawing objects
        if ( pDrawVirtObj->IsConnected() )
        {
            pDrawVirtObj->AnchoredObj()->InvalidateObjPos();
            // --> OD 2004-07-01 #i28701#
            if ( _bUpdateSortedObjsList )
            {
                pDrawVirtObj->AnchoredObj()->UpdateObjInSortedList();
            }
            // <--
        }
        // <--
    }

    // invalidate position of 'master' drawing object
    SwAnchoredObject* pAnchoredObj = GetAnchoredObj( 0L );
    pAnchoredObj->InvalidateObjPos();
    // --> OD 2004-07-01 #i28701#
    if ( _bUpdateSortedObjsList )
    {
        pAnchoredObj->UpdateObjInSortedList();
    }
    // <--
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

    // --> OD 2004-10-27 #i36181# - notify background of drawing object
    if ( _bMoveMasterToInvisibleLayer &&
         !(GetFmt()->GetDoc()->IsInDtor()) &&
         GetAnchorFrm() )
    {
        const Rectangle aOldRect( maAnchoredDrawObj.GetObjRectWithSpaces().SVRect() );
        lcl_NotifyBackgroundOfObj( *this, *GetMaster(), &aOldRect );
        NotifyBackgrdOfAllVirtObjs( &aOldRect );
    }
    // <--

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

    if ( maAnchoredDrawObj.GetAnchorFrm() )
    {
        maAnchoredDrawObj.AnchorFrm()->RemoveDrawObj( maAnchoredDrawObj );
    }

    if ( _bMoveMasterToInvisibleLayer && GetMaster() && GetMaster()->IsInserted() )
    {
        SdrViewIter aIter( GetMaster() );
        for( SdrView* pView = aIter.FirstView(); pView;
                    pView = aIter.NextView() )
        {
            pView->MarkObj( GetMaster(), pView->GetSdrPageView(), TRUE );
        }

        // OD 25.06.2003 #108784# - Instead of removing 'master' object from
        // drawing page, move the 'master' drawing object into the corresponding
        // invisible layer.
        {
            //((SwFrmFmt*)pRegisteredIn)->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage(0)->
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
            ((SwFrmFmt*)pRegisteredIn)->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage(0)->
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
            SwFrm* pNewAnchorFrmOfMaster = pDrawVirtObj->AnchorFrm();
            // disconnect 'virtual' drawing object
            pDrawVirtObj->RemoveFromWriterLayout();
            pDrawVirtObj->RemoveFromDrawingPage();
            // disconnect 'master' drawing object from current frame
            GetAnchorFrm()->RemoveDrawObj( maAnchoredDrawObj );
            // re-connect 'master' drawing object to frame of found 'virtual'
            // drawing object.
            pNewAnchorFrmOfMaster->AppendDrawObj( maAnchoredDrawObj );
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
SwTxtFrm* lcl_GetFlyInCntntAnchor( SwTxtFrm* _pProposedAnchorFrm,
                                   const xub_StrLen _nTxtOfs )
{
    SwTxtFrm* pAct = _pProposedAnchorFrm;
    SwTxtFrm* pTmp;
    do
    {
        pTmp = pAct;
        pAct = pTmp->GetFollow();
    }
    while( pAct && _nTxtOfs >= pAct->GetOfst() );
    return pTmp;
}

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

    // --> OD 2004-09-22 #i33909# - *no* connect to layout, if 'master' drawing
    // object isn't inserted in the drawing page
    if ( !GetMaster()->IsInserted() )
    {
        ASSERT( false, "<SwDrawContact::ConnectToLayout(..)> - master drawing object not inserted -> no connect to layout. Please inform od@openoffice.org" );
        return;
    }
    // <--

    SwFrmFmt* pDrawFrmFmt = (SwFrmFmt*)pRegisteredIn;

    SwRootFrm* pRoot = pDrawFrmFmt->getIDocumentLayoutAccess()->GetRootFrm();
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

    switch ( pAnch->GetAnchorId() )
    {
        case FLY_AT_PAGE:
                {
                USHORT nPgNum = pAnch->GetPageNum();
                SwPageFrm *pPage = static_cast<SwPageFrm*>(pRoot->Lower());

                for ( USHORT i = 1; i < nPgNum && pPage; ++i )
                {
                    pPage = static_cast<SwPageFrm*>(pPage->GetNext());
                }

                if ( pPage )
                {
                    pPage->AppendDrawObj( maAnchoredDrawObj );
                }
                else
                    //Sieht doof aus, ist aber erlaubt (vlg. SwFEShell::SetPageObjsNewPage)
                    pRoot->SetAssertFlyPages();
                }
                break;

        case FLY_AT_CHAR:
        case FLY_AT_PARA:
        case FLY_AT_FLY:
        case FLY_AS_CHAR:
            {
                if ( pAnch->GetAnchorId() == FLY_AS_CHAR )
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
                        // --> OD 2004-06-15 #i29199# - It is possible, that
                        // the anchor doesn't exist - E.g., reordering the
                        // sub-documents in a master document.
                        // Note: The anchor will be inserted later.
                        if ( !pModify )
                        {
                            // break to end of the current switch case.
                            break;
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
                            ASSERT( pFrm,
                                    "<SwDrawContact::ConnectToLayout(..)> - missing fly frame -> crash." );
                        }

                        // OD 2004-01-20 #110582# - find correct follow for
                        // as character anchored objects.
                        if ((pAnch->GetAnchorId() == FLY_AS_CHAR) &&
                             pFrm->IsTxtFrm() )
                        {
                            pFrm = lcl_GetFlyInCntntAnchor(
                                        static_cast<SwTxtFrm*>(pFrm),
                                        pAnch->GetCntntAnchor()->nContent.GetIndex() );
                        }

                        if ( !pAnchorFrmOfMaster )
                        {
                            // append 'master' drawing object
                            pAnchorFrmOfMaster = pFrm;
                            pFrm->AppendDrawObj( maAnchoredDrawObj );
                        }
                        else
                        {
                            // append 'virtual' drawing object
                            SwDrawVirtObj* pDrawVirtObj = AddVirtObj();
                            if ( pAnch->GetAnchorId() == FLY_AS_CHAR )
                            {
                                ClrContourCache( pDrawVirtObj );
                            }
                            pFrm->AppendDrawObj( *(pDrawVirtObj->AnchoredObj()) );

                            // for repaint, use new ActionChanged()
                            // pDrawVirtObj->SendRepaintBroadcast();
                            pDrawVirtObj->ActionChanged();
                        }

                        if ( pAnch->GetAnchorId() == FLY_AS_CHAR )
                        {
                            pFrm->InvalidatePrt();
                        }
                    }
                }
            }
            break;
        default:
            ASSERT( FALSE, "Unknown Anchor." )
            break;
    }
    if ( GetAnchorFrm() )
    {
        ::setContextWritingMode( maAnchoredDrawObj.DrawObj(), GetAnchorFrm() );
        // OD 2004-04-01 #i26791# - invalidate objects instead of direct positioning
        _InvalidateObjs();
    }
}

// OD 27.06.2003 #108784# - insert 'master' drawing object into drawing page
void SwDrawContact::InsertMasterIntoDrawPage()
{
    if ( !GetMaster()->IsInserted() )
    {
        GetFmt()->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage(0)
                ->InsertObject( GetMaster(), GetMaster()->GetOrdNumDirect() );
    }
    GetMaster()->SetUserCall( this );
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
    // --> OD 2004-07-01 #i28701# - use method <GetPageFrm()>
    SwPageFrm* pPg = GetPageFrm();
    if ( !pPg && GetAnchorFrm() )
        pPg = GetAnchorFrm()->FindPageFrm();
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

    // --> OD 2004-07-01 #i28701#
    SwPageFrm* pPg = ( maAnchoredDrawObj.GetAnchorFrm() &&
                       maAnchoredDrawObj.GetAnchorFrm()->IsPageFrm() )
                     ? GetPageFrm()
                     : FindPage( GetMaster()->GetCurrentBoundRect() );
    if ( GetPageFrm() != pPg )
    {
        // OD 27.06.2003 #108784# - if drawing object is anchor in header/footer
        // a change of the page is a dramatic change. Thus, completely re-connect
        // to the layout
        if ( maAnchoredDrawObj.GetAnchorFrm() &&
             maAnchoredDrawObj.GetAnchorFrm()->FindFooterOrHeader() )
        {
            ConnectToLayout();
        }
        else
        {
            // --> OD 2004-07-01 #i28701# - use methods <GetPageFrm()> and <SetPageFrm>
            if ( GetPageFrm() )
                GetPageFrm()->RemoveDrawObjFromPage( maAnchoredDrawObj );
            pPg->AppendDrawObjToPage( maAnchoredDrawObj );
            SetPageFrm( pPg );
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
// Note: 'master' drawing object *has* to be connected to layout triggered
//       by the caller of this, if method is called.
void SwDrawContact::ChangeMasterObject( SdrObject *pNewMaster )
{
    DisconnectFromLayout( false );
    // OD 10.07.2003 #110742# - consider 'virtual' drawing objects
    RemoveAllVirtObjs();

    GetMaster()->SetUserCall( 0 );
    SetMaster( pNewMaster );
    GetMaster()->SetUserCall( this );

    _InvalidateObjs();
}

/** get data collection of anchored objects, handled by with contact

    OD 2004-08-23 #110810#

    @author
*/
void SwDrawContact::GetAnchoredObjs( std::vector<SwAnchoredObject*>& _roAnchoredObjs ) const
{
    _roAnchoredObjs.push_back( const_cast<SwAnchoredDrawObject*>(&maAnchoredDrawObj) );

    for ( std::list<SwDrawVirtObj*>::const_iterator aDrawVirtObjsIter = maDrawVirtObjs.begin();
          aDrawVirtObjsIter != maDrawVirtObjs.end();
          ++aDrawVirtObjsIter )
    {
        _roAnchoredObjs.push_back( (*aDrawVirtObjsIter)->AnchoredObj() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////
// AW: own sdr::contact::ViewContact (VC) sdr::contact::ViewObjectContact (VOC) needed 
// since offset is defined different from SdrVirtObj's sdr::contact::ViewContactOfVirtObj. 
// For paint, that offset is used by setting at the OutputDevice; for primitives this is 
// not possible since we have no OutputDevice, but define the geometry itself.

namespace sdr
{
    namespace contact
    {
        class VOCOfDrawVirtObj : public ViewObjectContactOfSdrObj
        {
        protected:
            // This method is responsible for creating the graphical visualisation data which is
            // stored/cached in the local primitive. Default gets view-independent Primitive
            // from the ViewContact using ViewContact::getViewIndependentPrimitive2DSequence(), takes care of
            // visibility, handles glue and ghosted.
            // This method will not handle included hierarchies and not check geometric visibility.
            virtual drawinglayer::primitive2d::Primitive2DSequence createPrimitive2DSequence(const DisplayInfo& rDisplayInfo) const;

        public:
            VOCOfDrawVirtObj(ObjectContact& rObjectContact, ViewContact& rViewContact)
            :   ViewObjectContactOfSdrObj(rObjectContact, rViewContact)
            {
            }

            virtual ~VOCOfDrawVirtObj();
        };

        class VCOfDrawVirtObj : public ViewContactOfVirtObj
        {
        protected:
            // Create a Object-Specific ViewObjectContact, set ViewContact and
            // ObjectContact. Always needs to return something. Default is to create
            // a standard ViewObjectContact containing the given ObjectContact and *this
            virtual ViewObjectContact& CreateObjectSpecificViewObjectContact(ObjectContact& rObjectContact);

        public:
            // basic constructor, used from SdrObject.
            VCOfDrawVirtObj(SwDrawVirtObj& rObj)
            :   ViewContactOfVirtObj(rObj)
            {
            }
            virtual ~VCOfDrawVirtObj();
            
            // access to SwDrawVirtObj
            SwDrawVirtObj& GetSwDrawVirtObj() const
            {
                return (SwDrawVirtObj&)mrObject;
            }
        };
    } // end of namespace contact
} // end of namespace sdr

namespace sdr
{
    namespace contact
    {
        // recursively collect primitive data from given VOC with given offset
        void impAddPrimitivesFromGroup(const ViewObjectContact& rVOC, const basegfx::B2DHomMatrix& rOffsetMatrix, const DisplayInfo& rDisplayInfo, drawinglayer::primitive2d::Primitive2DSequence& rxTarget)
        {
            const sal_uInt32 nSubHierarchyCount(rVOC.GetViewContact().GetObjectCount());

            for(sal_uInt32 a(0L); a < nSubHierarchyCount; a++)
            {
                const ViewObjectContact& rCandidate(rVOC.GetViewContact().GetViewContact(a).GetViewObjectContact(rVOC.GetObjectContact()));

                if(rCandidate.GetViewContact().GetObjectCount())
                {
                    // is a group object itself, call resursively
                    impAddPrimitivesFromGroup(rCandidate, rOffsetMatrix, rDisplayInfo, rxTarget);
                }
                else
                {
                    // single object, add primitives; check model-view visibility
                    if(rCandidate.isPrimitiveVisible(rDisplayInfo))
                    {
                        drawinglayer::primitive2d::Primitive2DSequence aNewSequence(rCandidate.getPrimitive2DSequence(rDisplayInfo));

                        if(aNewSequence.hasElements())
                        {
                            // get ranges
                            const drawinglayer::geometry::ViewInformation2D& rViewInformation2D(rCandidate.GetObjectContact().getViewInformation2D());
                            const basegfx::B2DRange aViewRange(rViewInformation2D.getViewport());
                            basegfx::B2DRange aObjectRange(rCandidate.getObjectRange());

                            // correct with virtual object's offset
                            aObjectRange.transform(rOffsetMatrix);

                            // check geometrical visibility (with offset)
                            if(!aViewRange.overlaps(aObjectRange))
                            {
                                // not visible, release
                                aNewSequence.realloc(0);
                            }
                        }

                        if(aNewSequence.hasElements())
                        {
                            drawinglayer::primitive2d::appendPrimitive2DSequenceToPrimitive2DSequence(rxTarget, aNewSequence);
                        }
                    }
                }
            }
        }

        drawinglayer::primitive2d::Primitive2DSequence VOCOfDrawVirtObj::createPrimitive2DSequence(const DisplayInfo& rDisplayInfo) const
        {
#ifdef DBG_UTIL
            // #i101734#
            static bool bCheckOtherThanTranslate(false);
            static double fShearX(0.0);
            static double fRotation(0.0);
            static double fScaleX(0.0);
            static double fScaleY(0.0);
#endif

            const VCOfDrawVirtObj& rVC = static_cast< const VCOfDrawVirtObj& >(GetViewContact());
            const SdrObject& rReferencedObject = rVC.GetSwDrawVirtObj().GetReferencedObj();
            drawinglayer::primitive2d::Primitive2DSequence xRetval;

            // create offset transformation
            basegfx::B2DHomMatrix aOffsetMatrix;
            const Point aLocalOffset(rVC.GetSwDrawVirtObj().GetOffset());

            if(aLocalOffset.X() || aLocalOffset.Y())
            {
#ifdef DBG_UTIL
                // #i101734# added debug code to check more complex transformations
                // than just a translation
                if(bCheckOtherThanTranslate)
                {
                    aOffsetMatrix.scale(fScaleX, fScaleY);
                    aOffsetMatrix.shearX(tan(fShearX * F_PI180));
                    aOffsetMatrix.rotate(fRotation * F_PI180);
                }
#endif

                aOffsetMatrix.set(0, 2, aLocalOffset.X());
                aOffsetMatrix.set(1, 2, aLocalOffset.Y());

            }

            if(rReferencedObject.ISA(SdrObjGroup))
            {
                // group object. Since the VOC/OC/VC hierarchy does not represent the
                // hierarchy virtual objects when they have group objects
                // (ViewContactOfVirtObj::GetObjectCount() returns null for that purpose)
                // to avoid multiple usages of VOCs (which would not work), the primitives
                // for the sub-hierarchy need to be collected here

                // Get the VOC of the referenced object (the Group) and fetch primitives from it
                const ViewObjectContact& rVOCOfRefObj = rReferencedObject.GetViewContact().GetViewObjectContact(GetObjectContact());
                impAddPrimitivesFromGroup(rVOCOfRefObj, aOffsetMatrix, rDisplayInfo, xRetval);
            }
            else
            {
                // single object, use method from referenced object to get the Primitive2DSequence
                xRetval = rReferencedObject.GetViewContact().getViewIndependentPrimitive2DSequence();
            }

            if(xRetval.hasElements())
            {
                // create transform primitive
                const drawinglayer::primitive2d::Primitive2DReference xReference(new drawinglayer::primitive2d::TransformPrimitive2D(aOffsetMatrix, xRetval));
                xRetval = drawinglayer::primitive2d::Primitive2DSequence(&xReference, 1);
            }
            
            return xRetval;
        }

        VOCOfDrawVirtObj::~VOCOfDrawVirtObj()
        {
        }

        ViewObjectContact& VCOfDrawVirtObj::CreateObjectSpecificViewObjectContact(ObjectContact& rObjectContact)
        {
            return *(new VOCOfDrawVirtObj(rObjectContact, *this));
        }

        VCOfDrawVirtObj::~VCOfDrawVirtObj()
        {
        }
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////////////

// =============================================================================
/** implementation of class <SwDrawVirtObj>

    OD 14.05.2003 #108784#

    @author OD
*/

TYPEINIT1(SwDrawVirtObj,SdrVirtObj);

sdr::contact::ViewContact* SwDrawVirtObj::CreateObjectSpecificViewContact()
{
    return new sdr::contact::VCOfDrawVirtObj(*this);
}

// #108784#
// implemetation of SwDrawVirtObj
SwDrawVirtObj::SwDrawVirtObj( SdrObject&        _rNewObj,
                              SwDrawContact&    _rDrawContact )
    : SdrVirtObj( _rNewObj ),
      // OD 2004-03-29 #i26791# - init new member <maAnchoredDrawObj>
      maAnchoredDrawObj(),
      mrDrawContact( _rDrawContact )
{
    // OD 2004-03-29 #i26791#
    maAnchoredDrawObj.SetDrawObj( *this );
    // --> OD 2004-11-17 #i35635# - set initial position out of sight
    NbcMove( Size( -RECT_EMPTY, -RECT_EMPTY ) );
    // <--
}

SwDrawVirtObj::~SwDrawVirtObj()
{}

void SwDrawVirtObj::operator=( const SdrObject& rObj )
{
    SdrVirtObj::operator=(rObj);
    // Note: Members <maAnchoredDrawObj> and <mrDrawContact>
    //       haven't to be considered.
}

SdrObject* SwDrawVirtObj::Clone() const
{
    SwDrawVirtObj* pObj = new SwDrawVirtObj( rRefObj, mrDrawContact );

    if ( pObj )
    {
        pObj->operator=(static_cast<const SdrObject&>(*this));
        // Note: Member <maAnchoredDrawObj> hasn't to be considered.
    }

    return pObj;
}

// --------------------------------------------------------------------
// connection to writer layout: <GetAnchoredObj()>, <SetAnchorFrm(..)>,
// <GetAnchorFrm()>, <SetPageFrm(..)>, <GetPageFrm()> and <RemoveFromWriterLayout()>
// --------------------------------------------------------------------
const SwAnchoredObject* SwDrawVirtObj::GetAnchoredObj() const
{
    return &maAnchoredDrawObj;
}

SwAnchoredObject* SwDrawVirtObj::AnchoredObj()
{
    return &maAnchoredDrawObj;
}

const SwFrm* SwDrawVirtObj::GetAnchorFrm() const
{
    // OD 2004-03-29 #i26791# - use new member <maAnchoredDrawObj>
    return maAnchoredDrawObj.GetAnchorFrm();
}

SwFrm* SwDrawVirtObj::AnchorFrm()
{
    // OD 2004-03-29 #i26791# - use new member <maAnchoredDrawObj>
    return maAnchoredDrawObj.AnchorFrm();
}

void SwDrawVirtObj::RemoveFromWriterLayout()
{
    // remove contact object from frame for 'virtual' drawing object
    // OD 2004-03-29 #i26791# - use new member <maAnchoredDrawObj>
    if ( maAnchoredDrawObj.GetAnchorFrm() )
    {
        maAnchoredDrawObj.AnchorFrm()->RemoveDrawObj( maAnchoredDrawObj );
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
    SdrPage* pDrawPg;
    // --> OD 2004-08-16 #i27030# - apply order number of referenced object
    if ( 0 != ( pDrawPg = pOrgMasterSdrObj->GetPage() ) )
    {
        // --> OD 2004-08-16 #i27030# - apply order number of referenced object
        pDrawPg->InsertObject( this, GetReferencedObj().GetOrdNum() );
    }
    else
    {
        pDrawPg = GetPage();
        if ( pDrawPg )
        {
            pDrawPg->SetObjectOrdNum( GetOrdNumDirect(),
                                      GetReferencedObj().GetOrdNum() );
        }
        else
        {
            SetOrdNum( GetReferencedObj().GetOrdNum() );
        }
    }
    // <--
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

// is 'virtual' drawing object connected to writer layout and to drawing layer.
bool SwDrawVirtObj::IsConnected() const
{
    bool bRetVal = GetAnchorFrm() &&
                   ( GetPage() && GetUserCall() );

    return bRetVal;
}

void SwDrawVirtObj::NbcSetAnchorPos(const Point& rPnt)
{
    SdrObject::NbcSetAnchorPos( rPnt );
}

//////////////////////////////////////////////////////////////////////////////
// #i97197#
// the methods relevant for positioning

const Rectangle& SwDrawVirtObj::GetCurrentBoundRect() const
{
    if(aOutRect.IsEmpty())
    {
        const_cast<SwDrawVirtObj*>(this)->RecalcBoundRect();
    }

    return aOutRect;
}

const Rectangle& SwDrawVirtObj::GetLastBoundRect() const
{
    return aOutRect;
}

const Point SwDrawVirtObj::GetOffset() const
{
    // do NOT use IsEmpty() here, there is already a useful offset
    // in the position
    if(aOutRect == Rectangle())
    {
        return Point();
    }
    else
    {
        return aOutRect.TopLeft() - GetReferencedObj().GetCurrentBoundRect().TopLeft();
    }
}

void SwDrawVirtObj::SetBoundRectDirty()
{
    // do nothing to not lose model information in aOutRect
}

void SwDrawVirtObj::RecalcBoundRect()
{
    // OD 2004-04-05 #i26791# - switch order of calling <GetOffset()> and
    // <ReferencedObj().GetCurrentBoundRect()>, because <GetOffset()> calculates
    // its value by the 'BoundRect' of the referenced object.
    //aOutRect = rRefObj.GetCurrentBoundRect();
    //aOutRect += GetOffset();

    const Point aOffset(GetOffset());
    aOutRect = ReferencedObj().GetCurrentBoundRect() + aOffset;
}

basegfx::B2DPolyPolygon SwDrawVirtObj::TakeXorPoly() const
{
    basegfx::B2DPolyPolygon aRetval(rRefObj.TakeXorPoly());
    basegfx::B2DHomMatrix aMatrix;
    aMatrix.translate(GetOffset().X(), GetOffset().Y());
    aRetval.transform(aMatrix);

    return aRetval;
}

basegfx::B2DPolyPolygon SwDrawVirtObj::TakeContour() const
{
    basegfx::B2DPolyPolygon aRetval(rRefObj.TakeContour());
    basegfx::B2DHomMatrix aMatrix;
    aMatrix.translate(GetOffset().X(), GetOffset().Y());
    aRetval.transform(aMatrix);

    return aRetval;
}

SdrHdl* SwDrawVirtObj::GetHdl(sal_uInt32 nHdlNum) const
{
    SdrHdl* pHdl = rRefObj.GetHdl(nHdlNum);
    Point aP(pHdl->GetPos() + GetOffset());
    pHdl->SetPos(aP);

    return pHdl;
}

SdrHdl* SwDrawVirtObj::GetPlusHdl(const SdrHdl& rHdl, USHORT nPlNum) const
{
    SdrHdl* pHdl = rRefObj.GetPlusHdl(rHdl, nPlNum);
    pHdl->SetPos(pHdl->GetPos() + GetOffset());

    return pHdl;
}

void SwDrawVirtObj::NbcMove(const Size& rSiz)
{
    SdrObject::NbcMove( rSiz );
}

void SwDrawVirtObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    rRefObj.NbcResize(rRef - GetOffset(), xFact, yFact);
    SetRectsDirty();
}

void SwDrawVirtObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
    rRefObj.NbcRotate(rRef - GetOffset(), nWink, sn, cs);
    SetRectsDirty();
}

void SwDrawVirtObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    rRefObj.NbcMirror(rRef1 - GetOffset(), rRef2 - GetOffset());
    SetRectsDirty();
}

void SwDrawVirtObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    rRefObj.NbcShear(rRef - GetOffset(), nWink, tn, bVShear);
    SetRectsDirty();
}

void SwDrawVirtObj::Move(const Size& rSiz)
{
    SdrObject::Move( rSiz );
//    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
//    rRefObj.Move( rSiz );
//    SetRectsDirty();
//    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    if(xFact.GetNumerator() != xFact.GetDenominator() || yFact.GetNumerator() != yFact.GetDenominator())
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Resize(rRef - GetOffset(), xFact, yFact);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::Rotate(const Point& rRef, long nWink, double sn, double cs)
{
    if(nWink)
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Rotate(rRef - GetOffset(), nWink, sn, cs);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::Mirror(const Point& rRef1, const Point& rRef2)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    rRefObj.Mirror(rRef1 - GetOffset(), rRef2 - GetOffset());
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::Shear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    if(nWink)
    {
        Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
        rRefObj.Shear(rRef - GetOffset(), nWink, tn, bVShear);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
    }
}

void SwDrawVirtObj::RecalcSnapRect()
{
    aSnapRect = rRefObj.GetSnapRect();
    aSnapRect += GetOffset();
}

const Rectangle& SwDrawVirtObj::GetSnapRect() const
{
    ((SwDrawVirtObj*)this)->aSnapRect = rRefObj.GetSnapRect();
    ((SwDrawVirtObj*)this)->aSnapRect += GetOffset();

    return aSnapRect;
}

void SwDrawVirtObj::SetSnapRect(const Rectangle& rRect)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    Rectangle aR(rRect);
    aR -= GetOffset();
    rRefObj.SetSnapRect(aR);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::NbcSetSnapRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR -= GetOffset();
    SetRectsDirty();
    rRefObj.NbcSetSnapRect(aR);
}

const Rectangle& SwDrawVirtObj::GetLogicRect() const
{
    ((SwDrawVirtObj*)this)->aSnapRect = rRefObj.GetLogicRect();
    ((SwDrawVirtObj*)this)->aSnapRect += GetOffset();

    return aSnapRect;
}

void SwDrawVirtObj::SetLogicRect(const Rectangle& rRect)
{
    Rectangle aBoundRect0; if(pUserCall) aBoundRect0 = GetLastBoundRect();
    Rectangle aR(rRect);
    aR -= GetOffset();
    rRefObj.SetLogicRect(aR);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE, aBoundRect0);
}

void SwDrawVirtObj::NbcSetLogicRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR -= GetOffset();
    rRefObj.NbcSetLogicRect(aR);
    SetRectsDirty();
}

Point SwDrawVirtObj::GetSnapPoint(sal_uInt32 i) const
{
    Point aP(rRefObj.GetSnapPoint(i));
    aP += GetOffset();

    return aP;
}

Point SwDrawVirtObj::GetPoint(sal_uInt32 i) const
{
    return Point(rRefObj.GetPoint(i) + GetOffset());
}

void SwDrawVirtObj::NbcSetPoint(const Point& rPnt, sal_uInt32 i)
{
    Point aP(rPnt);
    aP -= GetOffset();
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

bool SwDrawVirtObj::supportsFullDrag() const
{
    // call parent
    return SdrVirtObj::supportsFullDrag();
}

SdrObject* SwDrawVirtObj::getFullDragClone() const
{
    // call parent
    return SdrVirtObj::getFullDragClone();
}

// eof

