 /*************************************************************************
 *
 *  $RCSfile: accmap.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: mib $ $Date: 2002-05-16 08:17:47 $
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

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif
#ifndef _SV_WINDOW_HXX 
#include <vcl/window.hxx>
#endif
#ifndef _SVDMODEL_HXX
#include <svx/svdmodel.hxx>
#endif
#ifndef SVX_UNOMOD_HXX
#include <svx/unomod.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif

#include <map>
#include <list>
#ifndef _ACCMAP_HXX
#include <accmap.hxx>
#endif
#ifndef _ACCCONTEXT_HXX
#include <acccontext.hxx>
#endif
#ifndef _ACCDOC_HXX
#include <accdoc.hxx>
#endif
#ifndef _ACCPARA_HXX
#include <accpara.hxx>
#endif
#ifndef _ACCHEADERFOOTER_HXX
#include <accheaderfooter.hxx>
#endif
#ifndef _ACCFOOTNOTE_HXX
#include <accfootnote.hxx>
#endif
#ifndef _ACCTEXTFRAME_HXX
#include <acctextframe.hxx>
#endif
#ifndef _ACCGRAPHIC_HXX
#include <accgraphic.hxx>
#endif
#ifndef _ACCEMBEDDED_HXX
#include <accembedded.hxx>
#endif
#ifndef _ACCCELL_HXX
#include <acccell.hxx>
#endif
#ifndef _ACCTABLE_HXX
#include <acctable.hxx>
#endif
#ifndef _FESH_HXX
#include "fesh.hxx"
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _HFFRM_HXX
#include <hffrm.hxx>
#endif
#ifndef _FTNFRM_HXX
#include <ftnfrm.hxx>
#endif
#ifndef _CELLFRM_HXX
#include <cellfrm.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _NDTYP_HXX
#include <ndtyp.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SVX_ACCESSIBILITY_SHAPE_TYPE_HANDLER_HXX
#include <svx/ShapeTypeHandler.hxx>
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_HXX
#include <svx/AccessibleShape.hxx>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLERELATIONTYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRelationType.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEEVENTID_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleEventId.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif
#ifndef  _COM_SUN_STAR_DOCUMENT_XEVENTBROADCASTER_HPP_
#include <com/sun/star/document/XEventBroadcaster.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif


using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::document;
using namespace ::rtl;

struct SwFrmFunc
{
    sal_Bool operator()( const SwFrm * p1,
                         const SwFrm * p2) const
    {
        return p1 < p2;
    }
};

typedef ::std::map < const SwFrm *, WeakReference < XAccessible >, SwFrmFunc > _SwAccessibleContextMap_Impl;

class SwAccessibleContextMap_Impl: public _SwAccessibleContextMap_Impl
{
public:
    
#ifndef PRODUCT
    sal_Bool mbLocked;
#endif

    SwAccessibleContextMap_Impl()
#ifndef PRODUCT
        : mbLocked( sal_False )
#endif
    {}

};

//------------------------------------------------------------------------------ 
class SwDrawModellListener_Impl : public SfxListener,
    public ::cppu::WeakImplHelper1< XEventBroadcaster >
{
    mutable ::osl::Mutex maListenerMutex;
    ::cppu::OInterfaceContainerHelper maEventListeners;
    SdrModel *mpDrawModel;

public:

    SwDrawModellListener_Impl( SdrModel *pDrawModel ); 
    virtual ~SwDrawModellListener_Impl(); 

    virtual void SAL_CALL addEventListener( const Reference< XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const Reference< XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);

    virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};

SwDrawModellListener_Impl::SwDrawModellListener_Impl( SdrModel *pDrawModel ) :
    maEventListeners( maListenerMutex ),
    mpDrawModel( pDrawModel )
{
    StartListening( *mpDrawModel );
}

SwDrawModellListener_Impl::~SwDrawModellListener_Impl() 
{
    EndListening( *mpDrawModel );
}

void SAL_CALL SwDrawModellListener_Impl::addEventListener( const Reference< XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    maEventListeners.addInterface( xListener );
}

void SAL_CALL SwDrawModellListener_Impl::removeEventListener( const Reference< XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    maEventListeners.removeInterface( xListener );
}

void SwDrawModellListener_Impl::Notify( SfxBroadcaster& rBC,
        const SfxHint& rHint )
{
    const SdrHint *pSdrHint = PTR_CAST( SdrHint, &rHint );
    if( !pSdrHint )
        return;
        
    EventObject aEvent;
    if( !SvxUnoDrawMSFactory::createEvent( mpDrawModel, pSdrHint, aEvent ) )
        return;

    ::cppu::OInterfaceIteratorHelper aIter( maEventListeners );
    while( aIter.hasMoreElements() )
    {
        Reference < XEventListener > xListener( aIter.next(),
                                                UNO_QUERY );
        try
        {
            xListener->notifyEvent( aEvent );
        }
        catch( ::com::sun::star::uno::RuntimeException& r )
        {
#ifdef DEBUG
            ByteString aError( "Runtime exception caught while notifying shape.:\n" );
            aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
            DBG_ERROR( aError.GetBuffer() );
#endif
        }
    }
}

//------------------------------------------------------------------------------ 
struct SwShapeFunc
{
    sal_Bool operator()( const SdrObject * p1,
                         const SdrObject * p2) const
    {
        return p1 < p2;
    }
};
typedef ::std::map < const SdrObject *, WeakReference < XAccessible >, SwShapeFunc > _SwAccessibleShapeMap_Impl;

class SwAccessibleShapeMap_Impl: public _SwAccessibleShapeMap_Impl
                                 
{
    accessibility::AccessibleShapeTreeInfo maInfo;

public:
    
#ifndef PRODUCT
    sal_Bool mbLocked;
#endif
    SwAccessibleShapeMap_Impl( SwAccessibleMap *pMap )
#ifndef PRODUCT
        : mbLocked( sal_False )
#endif
    {
        maInfo.SetSdrView( pMap->GetShell()->GetDrawView() );
        maInfo.SetWindow( pMap->GetShell()->GetWin() );
        maInfo.SetViewForwarder( pMap );
        Reference < XEventBroadcaster > xModelBroadcaster =
            new SwDrawModellListener_Impl( 
                    pMap->GetShell()->GetDoc()->MakeDrawModel() );
        maInfo.SetControllerBroadcaster( xModelBroadcaster );
    }

    const accessibility::AccessibleShapeTreeInfo& GetInfo() const { return maInfo; }

    ::vos::ORef < accessibility::AccessibleShape > *Copy( size_t& rSize, 
        const SwFEShell *pFESh = 0,
        ::vos::ORef < accessibility::AccessibleShape > **pSelShape = 0 ) const;
};

::vos::ORef < accessibility::AccessibleShape > 
    *SwAccessibleShapeMap_Impl::Copy( 
            size_t& rSize, const SwFEShell *pFESh,
            ::vos::ORef < accessibility::AccessibleShape > **pSelStart ) const
{
    ::vos::ORef < accessibility::AccessibleShape > *pShapes = 0;
    ::vos::ORef < accessibility::AccessibleShape > *pSelShape = 0;

    sal_uInt16 nSelShapes = pFESh ? pFESh->IsObjSelected() : 0;
    rSize = size();

    if( rSize > 0 )
    {
        pShapes = 
            new ::vos::ORef < accessibility::AccessibleShape >[rSize];

        const_iterator aIter = begin();
        const_iterator aEndIter = end();

        ::vos::ORef < accessibility::AccessibleShape > *pShape = pShapes;
        pSelShape = &(pShapes[rSize]);
        while( aIter != aEndIter )
        {
            const SdrObject *pObj = (*aIter).first;
            Reference < XAccessible > xAcc( (*aIter).second );
            if( nSelShapes && pFESh->IsObjSelected( *pObj ) )
            {
                // selected objects are inserted from the back
                --pSelShape;
                *pSelShape = 
                    static_cast < accessibility::AccessibleShape* >(
                                                    xAcc.get() );
                --nSelShapes;
            }
            else
            {
                *pShape = 
                    static_cast < accessibility::AccessibleShape* >(
                                                    xAcc.get() );
                ++pShape;
            }
            ++aIter;
        }
        ASSERT( pSelShape == pShape, "copying shapes went wrong!" );
    }

    if( pSelStart )
        *pSelStart = pSelShape;

    return pShapes;
}

//------------------------------------------------------------------------------ 
struct SwAccessibleEvent_Impl
{
public:
    enum EventType { CARET_OR_STATES, INVALID_CONTENT, POS_CHANGED,
                     CHILD_POS_CHANGED, SHAPE_SELECTION, DISPOSE };

private:
    SwRect		maOldBox;				// the old bounds for CHILD_POS_CHANGED
                                        // and POS_CHANGED
    WeakReference < XAccessible > mxAcc;	// The object that fires the event
    SwFrmOrObj 	maFrmOrObj;				// the child for CHILD_POS_CHANGED and
                                        // the same as xAcc for any other
                                        // event type
    EventType 	meType;					// The event type
    sal_uInt8	mnStates;				// check states or update caret pos

    SwAccessibleEvent_Impl& operator==( const SwAccessibleEvent_Impl& );

public:
    SwAccessibleEvent_Impl( EventType eT, SwAccessibleContext *pA,
                             const SwFrmOrObj& rFrmOrObj ) :
        meType( eT ), mxAcc( pA ), maFrmOrObj( rFrmOrObj ), mnStates( 0 )
    {}
    SwAccessibleEvent_Impl( EventType eT, const SwFrmOrObj& rFrmOrObj ) :
        meType( eT ), maFrmOrObj( rFrmOrObj ), mnStates( 0 ) 
    {
        ASSERT( SwAccessibleEvent_Impl::DISPOSE == meType, 
                "wrong event constructor, DISPOSE only" );
    }
    SwAccessibleEvent_Impl( EventType eT ) :
        meType( eT ), mnStates( 0 ) 
    {
        ASSERT( SwAccessibleEvent_Impl::SHAPE_SELECTION == meType, 
                "wrong event constructor, SHAPE_SELECTION only" );
    }
    SwAccessibleEvent_Impl( EventType eT, SwAccessibleContext *pA,
                            const SwFrmOrObj& rFrmOrObj, const SwRect& rR ) :
        meType( eT ), mxAcc( pA ), maFrmOrObj( rFrmOrObj ), maOldBox( rR ), 
        mnStates( 0 ) 
    {
        ASSERT( SwAccessibleEvent_Impl::CHILD_POS_CHANGED == meType ||
                SwAccessibleEvent_Impl::POS_CHANGED == meType, 
                "wrong event constructor, (CHILD_)POS_CHANGED only" );
    }
    SwAccessibleEvent_Impl( EventType eT, SwAccessibleContext *pA,
                            const SwFrmOrObj& rFrmOrObj, sal_uInt8 nSt  ) :
        meType( eT ), mxAcc( pA ), maFrmOrObj( rFrmOrObj ), mnStates( nSt )
    {
        ASSERT( SwAccessibleEvent_Impl::CARET_OR_STATES == meType, 
                "wrong event constructor, CARET_OR_STATES only" );
    }

    inline void SetType( EventType eT ){ meType = eT; }
    inline EventType 	GetType() const { return meType; }

    inline ::vos::ORef < SwAccessibleContext > GetContext() const;

    inline const SwRect& GetOldBox() const { return maOldBox; }
    inline void SetOldBox( const SwRect& rOldBox ) { maOldBox = rOldBox; }

    inline const SwFrmOrObj& GetFrmOrObj() const { return maFrmOrObj; }

    inline void SetStates( sal_uInt8 nSt ) { mnStates |= nSt; }
    inline sal_Bool IsUpdateCursorPos() const { return (mnStates & ACC_STATE_CARET) != 0; }
    inline sal_Bool IsInvalidateStates() const { return (mnStates & ACC_STATE_MASK) != 0; }
    inline sal_Bool IsInvalidateRelation() const { return (mnStates & ACC_STATE_RELATION_MASK) != 0; }
    inline sal_uInt8 GetStates() const { return mnStates & ACC_STATE_MASK; }
    inline sal_uInt8 GetAllStates() const { return mnStates; }
};

inline ::vos::ORef < SwAccessibleContext > 
    SwAccessibleEvent_Impl::GetContext() const
{
    Reference < XAccessible > xTmp( mxAcc );
    ::vos::ORef < SwAccessibleContext > xAccImpl(
         static_cast< SwAccessibleContext * >( xTmp.get() ) );

    return xAccImpl;
}

//------------------------------------------------------------------------------ 
typedef ::std::list < SwAccessibleEvent_Impl > _SwAccessibleEventList_Impl;

class SwAccessibleEventList_Impl: public _SwAccessibleEventList_Impl
{
    sal_Bool mbFiring;

public:

    SwAccessibleEventList_Impl() : mbFiring( sal_False ) {}

    inline void SetFiring() { mbFiring = sal_True; }
    inline sal_Bool IsFiring() const { return mbFiring; }
};

//------------------------------------------------------------------------------ 
// The shape list is filled if an accessible shape is destroyed. It
// simply keeps a reference to the accessible shape's XShape. These
// references are destroyed within the EndAction when firing events,
// There are twp reason for this. First of all, a new accessible shape
// for the XShape might be created soon. It's then cheaper if the XShape
// still exists. The other reason are situations where an accessible shape
// is destroyed within an SwFrmFmt::Modify. In this case, destryoing
// the XShape at the same time (indirectly by destroying the accessible
// shape) leads to an assert, because a client of the Modify is destroyed
// within a Modify call.

typedef ::std::list < Reference < XShape > > _SwShapeList_Impl;

class SwShapeList_Impl: public _SwShapeList_Impl
{
public:

    SwShapeList_Impl() {}
};


//------------------------------------------------------------------------------ 
struct SwFrmOrObjFunc
{
    sal_Bool operator()( const SwFrmOrObj& r1,
                         const SwFrmOrObj& r2 ) const
    {
        const void *p1 = r1.GetSwFrm()
                ? static_cast < const void * >( r1.GetSwFrm()) 
                : static_cast < const void * >( r1.GetSdrObject() );
        const void *p2 = r2.GetSwFrm()
                ? static_cast < const void * >( r2.GetSwFrm()) 
                : static_cast < const void * >( r2.GetSdrObject() );
        return p1 < p2;
    }
};
typedef ::std::map < SwFrmOrObj, SwAccessibleEventList_Impl::iterator,
                     SwFrmOrObjFunc > _SwAccessibleEventMap_Impl;

class SwAccessibleEventMap_Impl: public _SwAccessibleEventMap_Impl
{
};

//------------------------------------------------------------------------------ 
static sal_Bool AreInSameTable( const Reference< XAccessible >& rAcc,
                                 const SwFrm *pFrm )
{
    sal_Bool bRet = sal_False;

    if( pFrm && pFrm->IsCellFrm() && rAcc.is() )
    {
        // Is it in the same table? We check that
        // by comparing the last table frame in the
        // follow chain, because that's cheaper than
        // searching the first one.
        SwAccessibleContext *pAccImpl =
            static_cast< SwAccessibleContext *>( rAcc.get() );
        if( pAccImpl->GetFrm()->IsCellFrm() )
        {
            const SwTabFrm *pTabFrm1 = pAccImpl->GetFrm()->FindTabFrm();
            while( pTabFrm1->GetFollow() )
                   pTabFrm1 = pTabFrm1->GetFollow();

            const SwTabFrm *pTabFrm2 = pFrm->FindTabFrm();
            while( pTabFrm2->GetFollow() )
                   pTabFrm2 = pTabFrm2->GetFollow();

            bRet = (pTabFrm1 == pTabFrm2);
        }
    }

    return bRet;
}

void SwAccessibleMap::FireEvent( const SwAccessibleEvent_Impl& rEvent )
{
    ::vos::ORef < SwAccessibleContext > xAccImpl( rEvent.GetContext() );
    if( xAccImpl.isValid() )
    {
        switch( rEvent.GetType() )
        {
        case SwAccessibleEvent_Impl::INVALID_CONTENT:
            xAccImpl->InvalidateContent();
            break;
        case SwAccessibleEvent_Impl::POS_CHANGED:
            xAccImpl->InvalidatePosOrSize( rEvent.GetOldBox() );
            break;
        case SwAccessibleEvent_Impl::CHILD_POS_CHANGED:
            xAccImpl->InvalidateChildPosOrSize( rEvent.GetFrmOrObj(),
                                       rEvent.GetOldBox() );
            break;
        case SwAccessibleEvent_Impl::SHAPE_SELECTION:
            DoInvalidateShapeSelection();
            break;
        case SwAccessibleEvent_Impl::DISPOSE:
            ASSERT( xAccImpl.isValid(),
                    "dispose event has been stored" );
            break;
        }
        if( SwAccessibleEvent_Impl::DISPOSE != rEvent.GetType() )
        {
            if( rEvent.IsUpdateCursorPos() )
                xAccImpl->InvalidateCursorPos();
            if( rEvent.IsInvalidateStates() )
                xAccImpl->InvalidateStates( rEvent.GetStates() );
            if( rEvent.IsInvalidateRelation() )
                xAccImpl->InvalidateRelation( 
                    (rEvent.GetAllStates() & ACC_STATE_RELATION_FROM) != 0 ?
                    AccessibleEventId::CONTENT_FLOWS_FROM :
                    AccessibleEventId::CONTENT_FLOWS_TO );
        }
    }
}

void SwAccessibleMap::AppendEvent( const SwAccessibleEvent_Impl& rEvent )
{
    vos::OGuard aGuard( maEventMutex );

    if( !mpEvents )
        mpEvents = new SwAccessibleEventList_Impl;
    if( !mpEventMap )
        mpEventMap = new SwAccessibleEventMap_Impl;

    if( mpEvents->IsFiring() )
    {
        // While events are fired new ones are generated. They have to be fired
        // now. This does not work for DISPOSE events!
        ASSERT( rEvent.GetType() != SwAccessibleEvent_Impl::DISPOSE,
                "dispose event while firing events" );
        FireEvent( rEvent );
    }
    else
    {

        SwAccessibleEventMap_Impl::iterator aIter =
            mpEventMap->find( rEvent.GetFrmOrObj() );
        if( aIter != mpEventMap->end() )
        {
            SwAccessibleEvent_Impl aEvent( *(*aIter).second );
            ASSERT( aEvent.GetType() != SwAccessibleEvent_Impl::DISPOSE,
                    "dispose events should not be stored" );
            sal_Bool bAppendEvent = sal_True;
            switch( rEvent.GetType() )
            {
            case SwAccessibleEvent_Impl::CARET_OR_STATES:
                // A CARET_OR_STATES event is added to any other 
                // event only. It is broadcasted after any other event, so the
                // event should be put to the back.
                ASSERT( aEvent.GetType() !=
                            SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                        "invalid event combination" );
                aEvent.SetStates( rEvent.GetAllStates() );
                break;
            case SwAccessibleEvent_Impl::INVALID_CONTENT:
                // An INVALID_CONTENT event overwrites a CARET_OR_STATES
                // event (but keeps its flags) and it is contained in a
                // POS_CHANGED event.
                // Therefor, the event's type has to be adapted and the event
                // has to be put at the end.
                ASSERT( aEvent.GetType() !=
                               SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                        "invalid event combination" );
                if( aEvent.GetType() ==
                        SwAccessibleEvent_Impl::CARET_OR_STATES )
                    aEvent.SetType( SwAccessibleEvent_Impl::INVALID_CONTENT );
                break;
            case SwAccessibleEvent_Impl::POS_CHANGED:
                // A pos changed event overwrites CARET_STATES (keeping its
                // flags) as well as INVALID_CONTENT. The old box position
                // has to be stored however if the old event is not a
                // POS_CHANGED itself.
                ASSERT( aEvent.GetType() !=
                            SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                        "invalid event combination" );
                if( aEvent.GetType() != SwAccessibleEvent_Impl::POS_CHANGED )
                    aEvent.SetOldBox( rEvent.GetOldBox() );
                aEvent.SetType( SwAccessibleEvent_Impl::POS_CHANGED );
                break;
            case SwAccessibleEvent_Impl::CHILD_POS_CHANGED:
                // CHILD_POS_CHANGED events can only follow CHILD_POS_CHANGED
                // events. The only action that needs to be done again is
                // to put the old event to the back. The new one cannot be used,
                // because we are interested in the old frame bounds.
                ASSERT( aEvent.GetType() ==
                            SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                        "invalid event combination" );
                break;
            case SwAccessibleEvent_Impl::SHAPE_SELECTION:
                ASSERT( aEvent.GetType() ==
                            SwAccessibleEvent_Impl::SHAPE_SELECTION,
                        "invalid event combination" );
                break;
            case SwAccessibleEvent_Impl::DISPOSE:
                // DISPOSE events overwrite all others. They are not stored
                // but executed immediatly to avoid broadcasting of
                // defunctional objects. So what needs to be done here is to
                // remove all events for the frame in question.
                ASSERT( aEvent.GetType() !=
                            SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                        "invalid event combination" );
                bAppendEvent = sal_False;
                break;
            }
            if( bAppendEvent )
            {
                mpEvents->erase( (*aIter).second );
                (*aIter).second = mpEvents->insert( mpEvents->end(), aEvent );
            }
            else
            {
                mpEvents->erase( (*aIter).second );
                mpEventMap->erase( aIter );
            }
        }
        else if( SwAccessibleEvent_Impl::DISPOSE != rEvent.GetType() )
        {
            SwAccessibleEventMap_Impl::value_type aEntry( rEvent.GetFrmOrObj(),
                    mpEvents->insert( mpEvents->end(), rEvent ) );
            mpEventMap->insert( aEntry );
        }
    }
}

void SwAccessibleMap::InvalidateCursorPosition( 
        const Reference< XAccessible >& rAcc )
{
    SwAccessibleContext *pAccImpl =
        static_cast< SwAccessibleContext *>( rAcc.get() );
    ASSERT( pAccImpl, "no caret context" );
    ASSERT( pAccImpl->GetFrm(), "caret context is disposed" );
    if( GetShell()->ActionPend() )
    {
        SwAccessibleEvent_Impl aEvent(
            SwAccessibleEvent_Impl::CARET_OR_STATES, pAccImpl,
            pAccImpl->GetFrm(), ACC_STATE_CARET );
        AppendEvent( aEvent );
    }
    else
    {
        pAccImpl->InvalidateCursorPos();
    }
}

void SwAccessibleMap::InvalidateShapeSelection()
{
    if( GetShell()->ActionPend() )
    {
        SwAccessibleEvent_Impl aEvent(
            SwAccessibleEvent_Impl::SHAPE_SELECTION );
        AppendEvent( aEvent );
    }
    else
    {
        DoInvalidateShapeSelection();
    }
}

void SwAccessibleMap::DoInvalidateShapeSelection()
{
    ::vos::ORef < accessibility::AccessibleShape > *pShapes = 0;
    ::vos::ORef < accessibility::AccessibleShape > *pSelShape = 0;
    size_t nShapes = 0;

    const ViewShell *pVSh = GetShell();
    const SwFEShell *pFESh = pVSh->ISA( SwFEShell ) ?
                            static_cast< const SwFEShell * >( pVSh ) : 0;
    sal_uInt16 nSelShapes = pFESh ? pFESh->IsObjSelected() : 0;

    {
        vos::OGuard aGuard( maMutex );
        if( mpShapeMap )
            pShapes = mpShapeMap->Copy( nShapes, pFESh, &pSelShape );
    }
    
    if( pShapes )
    {
        Window *pWin = GetShell()->GetWin();
        sal_Bool bFocused = pWin && pWin->HasFocus();
        ::vos::ORef < accessibility::AccessibleShape > *pShape = pShapes;
        while( nShapes )
        {
            if( pShape->isValid() )
            {
                if( pShape >= pSelShape )
                {
                    (*pShape)->SetState( AccessibleStateType::SELECTED );
                    if( bFocused && 1 == nSelShapes )
                        (*pShape)->SetState( AccessibleStateType::FOCUSED );
                    else
                        (*pShape)->ResetState( AccessibleStateType::FOCUSED );
                }
                else
                {
                    (*pShape)->ResetState( AccessibleStateType::SELECTED );
                    (*pShape)->ResetState( AccessibleStateType::FOCUSED );
                }
            }

            --nShapes;
            ++pShape;
        }
        // TODO: Selection event

        delete[] pShapes;
    }
}

void SwAccessibleMap::DoInvalidateShapeFocus()
{
    const ViewShell *pVSh = GetShell();
    const SwFEShell *pFESh = pVSh->ISA( SwFEShell ) ?
                            static_cast< const SwFEShell * >( pVSh ) : 0;
    sal_uInt16 nSelShapes = pFESh ? pFESh->IsObjSelected() : 0;

    if( nSelShapes != 1 )
        return;

    ::vos::ORef < accessibility::AccessibleShape > *pShapes = 0;
    ::vos::ORef < accessibility::AccessibleShape > *pSelShape = 0;
    size_t nShapes = 0;


    {
        vos::OGuard aGuard( maMutex );
        if( mpShapeMap )
            pShapes = mpShapeMap->Copy( nShapes, pFESh, &pSelShape );
    }

    if( pShapes )
    {
        Window *pWin = GetShell()->GetWin();
        sal_Bool bFocused = pWin && pWin->HasFocus();
        ::vos::ORef < accessibility::AccessibleShape > *pShape = pShapes;
        while( nShapes )
        {
            if( pShape->isValid() )
            {
                if( bFocused && pShape >= pSelShape )
                    (*pShape)->SetState( AccessibleStateType::FOCUSED );
                else
                    (*pShape)->ResetState( AccessibleStateType::FOCUSED );
            }

            --nShapes;
            ++pShape;
        }

        delete[] pShapes;
    }
}


SwAccessibleMap::SwAccessibleMap( ViewShell *pSh ) :
    mpFrmMap( 0  ),
    mpShapeMap( 0  ),
    mpShapes( 0  ),
    mpEvents( 0  ),
    mpEventMap( 0  ),
    mpVSh( pSh ),
    mnPara( 1 ),
    mnFootnote( 1 ),
    mnEndnote( 1 ),
    mbShapeSelected( sal_False )
{
    pSh->GetLayout()->AddAccessibleShell();
}

SwAccessibleMap::~SwAccessibleMap()
{
    Reference < XAccessible > xAcc;
    {
        vos::OGuard aGuard( maMutex );
        if( mpFrmMap )
        {
            const SwRootFrm *pRootFrm = GetShell()->GetLayout();
            SwAccessibleContextMap_Impl::iterator aIter = mpFrmMap->find( pRootFrm );
            if( aIter != mpFrmMap->end() )
                xAcc = (*aIter).second;
            if( !xAcc.is() )
                xAcc = new SwAccessibleDocument( this );
        }
    }

    SwAccessibleDocument *pAcc =
        static_cast< SwAccessibleDocument * >( xAcc.get() );
    pAcc->Dispose( sal_True );

    {
        vos::OGuard aGuard( maMutex );
#ifndef PRODUCT
        ASSERT( !mpFrmMap || mpFrmMap->empty(),
                "Frame map should be empty after disposing the root frame" );
        if( mpFrmMap )
        {
            SwAccessibleContextMap_Impl::iterator aIter = mpFrmMap->begin();
            while( aIter != mpFrmMap->end() )
            {
                Reference < XAccessible > xTmp = (*aIter).second;
                if( xTmp.is() )
                {
                    SwAccessibleContext *pTmp =
                        static_cast< SwAccessibleContext * >( xTmp.get() );
                }
                ++aIter;
            }
        }
        ASSERT( !mpShapeMap || mpShapeMap->empty(),
                "Object map should be empty after disposing the root frame" );
        if( mpShapeMap )
        {
            SwAccessibleShapeMap_Impl::iterator aIter = mpShapeMap->begin();
            while( aIter != mpShapeMap->end() )
            {
                Reference < XAccessible > xTmp = (*aIter).second;
                if( xTmp.is() )
                {
                    accessibility::AccessibleShape *pTmp =
                        static_cast< accessibility::AccessibleShape* >( xTmp.get() );
                }
                ++aIter;
            }
        }
#endif
        delete mpFrmMap;
        mpFrmMap = 0;
        delete mpShapeMap;
        mpShapeMap = 0;
        delete mpShapes;
        mpShapes = 0;
    }

    {
        vos::OGuard aGuard( maEventMutex );
        ASSERT( !(mpEvents || mpEventMap), "pending events" );
        delete mpEventMap;
        mpEventMap = 0;
        delete mpEvents;
        mpEvents = 0;
    }
    mpVSh->GetLayout()->RemoveAccessibleShell();
}

Reference< XAccessible > SwAccessibleMap::GetDocumentView()
{
    Reference < XAccessible > xAcc;
    sal_Bool bSetVisArea = sal_False;

    {
        vos::OGuard aGuard( maMutex );

        if( !mpFrmMap )
        {
            mpFrmMap = new SwAccessibleContextMap_Impl;
#ifndef PRODUCT
            mpFrmMap->mbLocked = sal_False;
#endif
        }

#ifndef PRODUCT
        ASSERT( !mpFrmMap->mbLocked, "Map is locked" );
        mpFrmMap->mbLocked = sal_True;
#endif

        const SwRootFrm *pRootFrm = GetShell()->GetLayout();
        SwAccessibleContextMap_Impl::iterator aIter = mpFrmMap->find( pRootFrm );
        if( aIter != mpFrmMap->end() )
            xAcc = (*aIter).second;
        if( xAcc.is() )
        {
            bSetVisArea = sal_True; // Set VisArea when map mutex is not
                                    // locked
        }
        else
        {
            xAcc = new SwAccessibleDocument( this );
            if( aIter != mpFrmMap->end() )
            {
                (*aIter).second = xAcc;
            }
            else
            {
                SwAccessibleContextMap_Impl::value_type aEntry( pRootFrm, xAcc );
                mpFrmMap->insert( aEntry );
            }
        }

#ifndef PRODUCT
        mpFrmMap->mbLocked = sal_False;
#endif
    }

    if( bSetVisArea )
    {
        SwAccessibleDocument *pAcc =
            static_cast< SwAccessibleDocument * >( xAcc.get() );
        pAcc->SetVisArea();
    }

    return xAcc;
}

Reference< XAccessible> SwAccessibleMap::GetContext( const SwFrm *pFrm,
                                                     sal_Bool bCreate )
{
    Reference < XAccessible > xAcc;
    Reference < XAccessible > xOldCursorAcc;
    sal_Bool bOldShapeSelected = sal_False;

    {
        vos::OGuard aGuard( maMutex );

        if( !mpFrmMap && bCreate )
            mpFrmMap = new SwAccessibleContextMap_Impl;
        if( mpFrmMap )
        {
            SwAccessibleContextMap_Impl::iterator aIter = mpFrmMap->find( pFrm );
            if( aIter != mpFrmMap->end() )
                xAcc = (*aIter).second;

            if( !xAcc.is() && bCreate )
            {
                SwAccessibleContext *pAcc = 0;
                switch( pFrm->GetType() )
                {
                case FRM_TXT:
                    pAcc = new SwAccessibleParagraph( this, mnPara++, 
                                    static_cast< const SwTxtFrm * >( pFrm ) );
                    break;
                case FRM_HEADER:
                    pAcc = new SwAccessibleHeaderFooter( this, 
                                    static_cast< const SwHeaderFrm *>( pFrm ) );
                    break;
                case FRM_FOOTER:
                    pAcc = new SwAccessibleHeaderFooter( this, 
                                    static_cast< const SwFooterFrm *>( pFrm ) );
                    break;
                case FRM_FTN:
                    {
                        const SwFtnFrm *pFtnFrm =
                            static_cast < const SwFtnFrm * >( pFrm );
                        sal_Bool bIsEndnote =
                            SwAccessibleFootnote::IsEndnote( pFtnFrm );
                        pAcc = new SwAccessibleFootnote( this, bIsEndnote,
                                    (bIsEndnote ? mnEndnote++ : mnFootnote++),
                                    pFtnFrm );
                    }
                    break;
                case FRM_FLY:
                    {
                        const SwFlyFrm *pFlyFrm =
                            static_cast < const SwFlyFrm * >( pFrm );
                        switch( SwAccessibleFrameBase::GetNodeType( pFlyFrm ) )
                        {
                        case ND_GRFNODE:
                            pAcc = new SwAccessibleGraphic( this, pFlyFrm );
                            break;
                        case ND_OLENODE:
                            pAcc = new SwAccessibleEmbeddedObject( this, pFlyFrm );
                            break;
                        default:
                            pAcc = new SwAccessibleTextFrame( this, pFlyFrm );
                            break;
                        }
                    }
                    break;
                case FRM_CELL:
                    pAcc = new SwAccessibleCell( this, 
                                    static_cast< const SwCellFrm *>( pFrm ) );
                    break;
                case FRM_TAB:
                    pAcc = new SwAccessibleTable( this, 
                                    static_cast< const SwTabFrm *>( pFrm ) );
                    break;
                }
                xAcc = pAcc;

                ASSERT( xAcc.is(), "unknown frame type" );
                if( xAcc.is() )
                {
                    if( aIter != mpFrmMap->end() )
                    {
                        (*aIter).second = xAcc;
                    }
                    else
                    {
                        SwAccessibleContextMap_Impl::value_type aEntry( pFrm, xAcc );
                        mpFrmMap->insert( aEntry );
                    }

                    if( pAcc->HasCursor() && 
                        !AreInSameTable( mxCursorContext, pFrm ) )
                    {
                        // If the new context has the focus, and if we know
                        // another context that had the focus, then the focus
                        // just moves from the old context to the new one. We
                        // have to send a focus event and a caret event for 
                        // the old context then. We have to to that know, 
                        // because after we have left this method, anyone might
                        // call getStates for the new context and will get a
                        // focused state then. Sending the focus changes event
                        // after that seems to be strange. However, we cannot
                        // send a focus event fo the new context now, because
                        // noone except us knows it. In any case, we remeber 
                        // the new context as the one that has the focus 
                        // currently.

                        xOldCursorAcc = mxCursorContext;
                        mxCursorContext = xAcc;

                        bOldShapeSelected = mbShapeSelected;
                        mbShapeSelected = sal_False;
                    }
                }
            }
        }
    }

    // Invalidate focus for old object when map is not locked
    if( xOldCursorAcc.is() )
        InvalidateCursorPosition( xOldCursorAcc );
    if( bOldShapeSelected )
        InvalidateShapeSelection();

    return xAcc;
}

::vos::ORef < SwAccessibleContext > SwAccessibleMap::GetContextImpl(
            const SwFrm *pFrm,
            sal_Bool bCreate )
{
    Reference < XAccessible > xAcc( GetContext( pFrm, bCreate ) );

    ::vos::ORef < SwAccessibleContext > xAccImpl(
         static_cast< SwAccessibleContext * >( xAcc.get() ) );

    return xAccImpl;
}

Reference< XAccessible> SwAccessibleMap::GetContext(
        const SdrObject *pObj,
        SwAccessibleContext *pParentImpl,
        sal_Bool bCreate )
{
    Reference < XAccessible > xAcc;
    Reference < XAccessible > xOldCursorAcc;

    {
        vos::OGuard aGuard( maMutex );

        if( !mpShapeMap && bCreate )
            mpShapeMap = new SwAccessibleShapeMap_Impl( this );
        if( mpShapeMap )
        {
            SwAccessibleShapeMap_Impl::iterator aIter =
                   mpShapeMap->find( pObj );
            if( aIter != mpShapeMap->end() )
                xAcc = (*aIter).second;

            if( !xAcc.is() && bCreate )
            {
                accessibility::AccessibleShape *pAcc = 0;
                Reference < XShape > xShape( 
                    const_cast< SdrObject * >( pObj )->getUnoShape(),
                    UNO_QUERY );
                if( xShape.is() )
                {
                    accessibility::ShapeTypeHandler& rShapeTypeHandler =
                                accessibility::ShapeTypeHandler::Instance();
                    Reference < XAccessible > xParent( pParentImpl );
                    pAcc = rShapeTypeHandler.CreateAccessibleObject( xShape,
                                xParent, mpShapeMap->GetInfo() );
                }
                xAcc = pAcc;

                ASSERT( xAcc.is(), "unknown shape type" );
                if( xAcc.is() )
                {
                    if( aIter != mpShapeMap->end() )
                    {
                        (*aIter).second = xAcc;
                    }
                    else
                    {
                        SwAccessibleShapeMap_Impl::value_type aEntry( pObj,
                                                                      xAcc );
                        mpShapeMap->insert( aEntry );
                    }
                    // TODO: focus!!!
                }
            }
        }
    }

    // Invalidate focus for old object when map is not locked
    if( xOldCursorAcc.is() )
        InvalidateCursorPosition( xOldCursorAcc );

    return xAcc;
}

::vos::ORef < accessibility::AccessibleShape > SwAccessibleMap::GetContextImpl(
            const SdrObject *pObj,
            SwAccessibleContext *pParentImpl,
            sal_Bool bCreate )
{
    Reference < XAccessible > xAcc( GetContext( pObj, pParentImpl, bCreate ) );

    ::vos::ORef < accessibility::AccessibleShape > xAccImpl(
         static_cast< accessibility::AccessibleShape* >( xAcc.get() ) );

    return xAccImpl;
}


void SwAccessibleMap::RemoveContext( const SwFrm *pFrm )
{
    vos::OGuard aGuard( maMutex );

    if( mpFrmMap )
    {
        SwAccessibleContextMap_Impl::iterator aIter =
            mpFrmMap->find( pFrm );
        if( aIter != mpFrmMap->end() )
        {
            mpFrmMap->erase( aIter );

            // Remove reference to old caret object. Though mxCursorContext
            // is a weak reference and cleared automatically, clearing it
            // directly makes sure to not keep a defunctional object.
            Reference < XAccessible > xOldAcc( mxCursorContext );
            if( xOldAcc.is() )
            {
                SwAccessibleContext *pOldAccImpl =
                    static_cast< SwAccessibleContext *>( xOldAcc.get() );
                ASSERT( pOldAccImpl->GetFrm(), "old caret context is disposed" );
                if( pOldAccImpl->GetFrm() == pFrm )
                {
                    xOldAcc.clear();	// get an empty ref
                    mxCursorContext = xOldAcc;
                }
            }

            if( mpFrmMap->empty() )
            {
                delete mpFrmMap;
                mpFrmMap = 0;
            }
        }
    }
}

void SwAccessibleMap::RemoveContext( const SdrObject *pObj )
{
    vos::OGuard aGuard( maMutex );

    if( mpShapeMap )
    {
        SwAccessibleShapeMap_Impl::iterator aIter =
            mpShapeMap->find( pObj );
        if( aIter != mpShapeMap->end() )
        {
            mpShapeMap->erase( aIter );

            // The shape selection flag is not cleared, but one might do
            // so but has to make sure that the removed context is the one
            // that is selected.

            if( mpShapeMap->empty() )
            {
                delete mpShapeMap;
                mpShapeMap = 0;
            }
        }
    }
}


void SwAccessibleMap::Dispose( const SwFrm *pFrm, const SdrObject *pObj,
                               sal_Bool bRecursive )
{
    SwFrmOrObj aFrmOrObj( pFrm, pObj );

    // Indeed, the following assert checks the frame's accessible flag,
    // because that's the one that is evaluated in the layout. The frame
    // might not be accessible anyway. That's the case for cell frames that
    // contain further cells.
    ASSERT( !aFrmOrObj.GetSwFrm() || aFrmOrObj.GetSwFrm()->IsAccessibleFrm(),
            "non accessible frame should be disposed" );

    ::vos::ORef< SwAccessibleContext > xAccImpl;
    ::vos::ORef< SwAccessibleContext > xParentAccImpl;
    ::vos::ORef< ::accessibility::AccessibleShape > xShapeAccImpl;
    if( aFrmOrObj.IsAccessible() )
    {
        // get accessible context for frame
        {
            vos::OGuard aGuard( maMutex );

            // First of all look for an accessible context for a frame
            if( aFrmOrObj.GetSwFrm() && mpFrmMap )
            {
                SwAccessibleContextMap_Impl::iterator aIter =
                    mpFrmMap->find( aFrmOrObj.GetSwFrm() );
                if( aIter != mpFrmMap->end() )
                {
                    Reference < XAccessible > xAcc( (*aIter).second );
                    xAccImpl =
                        static_cast< SwAccessibleContext *>( xAcc.get() );
                }
            }
            if( !xAccImpl.isValid() && mpFrmMap )
            {
                // If there is none, look if the parent is accessible.
                const SwFrm *pParent =
                        SwAccessibleFrame::GetParent( aFrmOrObj );

                if( pParent )
                {
                    SwAccessibleContextMap_Impl::iterator aIter =
                        mpFrmMap->find( pParent );
                    if( aIter != mpFrmMap->end() )
                    {
                        Reference < XAccessible > xAcc( (*aIter).second );
                        xParentAccImpl =
                            static_cast< SwAccessibleContext *>( xAcc.get() );
                    }
                }
            }
            if( !xParentAccImpl.isValid() && !aFrmOrObj.GetSwFrm() && 
                mpShapeMap )
            {
                SwAccessibleShapeMap_Impl::iterator aIter =
                    mpShapeMap->find( aFrmOrObj.GetSdrObject() );
                if( aIter != mpShapeMap->end() )
                {
                    Reference < XAccessible > xAcc( (*aIter).second );
                    xShapeAccImpl =
                        static_cast< accessibility::AccessibleShape *>( xAcc.get() );
                }
            }
            if( pObj && GetShell()->ActionPend() &&
                (xParentAccImpl.isValid() || xShapeAccImpl.isValid()) )
            {
                // Keep a reference to the XShape to avoid that it
                // is deleted with a SwFrmFmt::Modify.
                Reference < XShape > xShape( 
                    const_cast< SdrObject * >( pObj )->getUnoShape(),
                    UNO_QUERY );
                if( xShape.is() )
                {
                    if( !mpShapes )
                        mpShapes = new SwShapeList_Impl;
                    mpShapes->push_back( xShape );
                }
            }
        }

        // remove events stored for the frame
        {
            vos::OGuard aGuard( maEventMutex );
            if( mpEvents )
            {
                SwAccessibleEventMap_Impl::iterator aIter =
                    mpEventMap->find( aFrmOrObj );
                if( aIter != mpEventMap->end() )
                {
                    SwAccessibleEvent_Impl aEvent(
                            SwAccessibleEvent_Impl::DISPOSE, aFrmOrObj );
                    AppendEvent( aEvent );
                }
            }
        }
    
        // If the frame is accessible and there is a context for it, dispose
        // the frame. If the frame is no context for it but disposing should
        // take place recursive, the frame's children have to be disposed
        // anyway, so we have to create the context then.
        if( xAccImpl.isValid() )
        {
            xAccImpl->Dispose( bRecursive );
        }
        else if( xParentAccImpl.isValid() )
        {
            // If the frame is a cell frame, the table must be notified.
            // If we are in an action, a table model change event will
            // be broadcasted at the end of the action to give the table
            // a chance to generate a single table change event.

            xParentAccImpl->DisposeChild( aFrmOrObj, bRecursive );
        }
        else if( xShapeAccImpl.isValid() )
        {
            RemoveContext( aFrmOrObj.GetSdrObject() );
            xShapeAccImpl->dispose();
        }
    }
}

void SwAccessibleMap::InvalidatePosOrSize( const SwFrm *pFrm,
                                           const SdrObject *pObj, 
                                           const SwRect& rOldBox )
{
    SwFrmOrObj aFrmOrObj( pFrm, pObj );
    if( aFrmOrObj.IsAccessible() )
    {
        ::vos::ORef< SwAccessibleContext > xAccImpl;
        ::vos::ORef< SwAccessibleContext > xParentAccImpl;
        {
            vos::OGuard aGuard( maMutex );

            if( mpFrmMap )
            {
                if( aFrmOrObj.GetSwFrm() )
                {
                    SwAccessibleContextMap_Impl::iterator aIter =
                        mpFrmMap->find( aFrmOrObj.GetSwFrm() );
                    if( aIter != mpFrmMap->end() )
                    {
                        // If there is an accesible object already it is
                        // notified directly.
                        Reference < XAccessible > xAcc( (*aIter).second );
                        xAccImpl =
                            static_cast< SwAccessibleContext *>( xAcc.get() );
                    }
                }
                if( !xAccImpl.isValid() )
                {
                    // Otherwise we look if the parent is accessible.
                    // If not, there is nothing to do.
                    const SwFrm *pParent =
                        SwAccessibleFrame::GetParent( aFrmOrObj );

                    if( pParent )
                    {
                        SwAccessibleContextMap_Impl::iterator aIter =
                            mpFrmMap->find( pParent );
                        if( aIter != mpFrmMap->end() )
                        {
                            Reference < XAccessible > xAcc( (*aIter).second );
                            xParentAccImpl =
                                static_cast< SwAccessibleContext *>( xAcc.get() );
                        }
                    }
                }
            }
        }

        if( xAccImpl.isValid() )
        {
            ASSERT( !rOldBox.IsEmpty(), "context should have a size" );
            if( GetShell()->ActionPend() )
            {
                SwAccessibleEvent_Impl aEvent(
                    SwAccessibleEvent_Impl::POS_CHANGED, xAccImpl.getBodyPtr(),
                    aFrmOrObj, rOldBox );
                AppendEvent( aEvent );
            }
            else
            {
                xAccImpl->InvalidatePosOrSize( rOldBox );
            }
        }
        else if( xParentAccImpl.isValid() )
        {
            if( GetShell()->ActionPend() )
            {
                SwAccessibleEvent_Impl aEvent(
                    SwAccessibleEvent_Impl::CHILD_POS_CHANGED,
                    xParentAccImpl.getBodyPtr(), aFrmOrObj, rOldBox );
                AppendEvent( aEvent );
            }
            else
            {
                xParentAccImpl->InvalidateChildPosOrSize( aFrmOrObj,
                                                          rOldBox );
            }
        }
    }
}

void SwAccessibleMap::InvalidateContent( const SwFrm *pFrm )
{
    SwFrmOrObj aFrmOrObj( pFrm );
    if( aFrmOrObj.IsAccessible() )
    {
        Reference < XAccessible > xAcc;
        {
            vos::OGuard aGuard( maMutex );

            if( mpFrmMap )
            {
                SwAccessibleContextMap_Impl::iterator aIter =
                    mpFrmMap->find( aFrmOrObj.GetSwFrm() );
                if( aIter != mpFrmMap->end() )
                    xAcc = (*aIter).second;
            }
        }

        if( xAcc.is() )
        {
            SwAccessibleContext *pAccImpl =
                static_cast< SwAccessibleContext *>( xAcc.get() );
            if( GetShell()->ActionPend() )
            {
                SwAccessibleEvent_Impl aEvent(
                    SwAccessibleEvent_Impl::INVALID_CONTENT, pAccImpl,
                    aFrmOrObj );
                AppendEvent( aEvent );
            }
            else
            {
                pAccImpl->InvalidateContent();
            }
        }
    }
}

void SwAccessibleMap::InvalidateCursorPosition( const SwFrm *pFrm )
{
    SwFrmOrObj aFrmOrObj( pFrm );
    sal_Bool bShapeSelected = sal_False;
    const ViewShell *pVSh = GetShell();
    if( pVSh->ISA( SwCrsrShell ) )
    {
        const SwCrsrShell *pCSh = static_cast< const SwCrsrShell * >( pVSh );
        if( pCSh->IsTableMode() )
        {
            while( aFrmOrObj.GetSwFrm() && !aFrmOrObj.GetSwFrm()->IsCellFrm() )
                aFrmOrObj = aFrmOrObj.GetSwFrm()->GetUpper();
        }
        else if( pVSh->ISA( SwFEShell ) )
        {
            sal_uInt16 nObjCount;
            const SwFEShell *pFESh = static_cast< const SwFEShell * >( pVSh );
            const SwFrm *pFlyFrm = pFESh->GetCurrFlyFrm();
            if( pFlyFrm )
            {
                ASSERT( !pFrm || pFrm->FindFlyFrm() == pFlyFrm,
                        "cursor is not contained in fly frame" );
                aFrmOrObj = pFlyFrm;
            }
            else if( (nObjCount = pFESh->IsObjSelected()) > 0 )
            {
                bShapeSelected = sal_True;
                aFrmOrObj = static_cast<const SwFrm *>( 0 );
            }
        }
    }

    ASSERT( bShapeSelected || aFrmOrObj.IsAccessible(), 
            "frame is not accessible" );

    Reference < XAccessible > xOldAcc;
    Reference < XAccessible > xAcc;
    sal_Bool bOldShapeSelected = sal_False;

    {
        vos::OGuard aGuard( maMutex );

        xOldAcc = mxCursorContext;
        mxCursorContext = xAcc;	// clear reference

        bOldShapeSelected = mbShapeSelected;
        mbShapeSelected = bShapeSelected;

        if( aFrmOrObj.GetSwFrm() && mpFrmMap )
        {
            SwAccessibleContextMap_Impl::iterator aIter =
                mpFrmMap->find( aFrmOrObj.GetSwFrm() );
            if( aIter != mpFrmMap->end() )
                xAcc = (*aIter).second;

            // For cells, some extra thoughts are necessary,
            // because invalidating the cursor for one cell
            // invalidates the cursor for all cells of the same
            // table. For this reason, we don't want to
            // invalidate the cursor for the old cursor object
            // and the new one if they are within the same table,
            // because this would result in doing the work twice.
            // Moreover, we have to make sure to invalidate the
            // cursor even if the current cell has no accessible object.
            // If the old cursor objects exists and is in the same
            // table, its the best choice, because using it avoids
            // an unnessarary cursor invalidation cycle when creating
            // a new object for the current cell.
            if( aFrmOrObj.GetSwFrm()->IsCellFrm() )
            {
                if( xOldAcc.is() &&
                    AreInSameTable( xOldAcc, aFrmOrObj.GetSwFrm() ) )
                {
                    if( xAcc.is() )
                        xOldAcc = xAcc;	// avoid extra invalidation
                    else 
                        xAcc = xOldAcc;	// make sure ate least one	
                }
                if( !xAcc.is() )
                    xAcc = GetContext( aFrmOrObj.GetSwFrm(), sal_True );
            }
        }
    }

    if( xOldAcc.is() && xOldAcc != xAcc )
        InvalidateCursorPosition( xOldAcc );
    if( bOldShapeSelected || bShapeSelected )
        InvalidateShapeSelection();
    if( xAcc.is() )
        InvalidateCursorPosition( xAcc );
}

void SwAccessibleMap::InvalidateFocus()
{
    Reference < XAccessible > xAcc;
    sal_Bool bShapeSelected;
    {
        vos::OGuard aGuard( maMutex );

        xAcc = mxCursorContext;
        bShapeSelected = mbShapeSelected;
    }

    if( xAcc.is() )
    {
        SwAccessibleContext *pAccImpl =
            static_cast< SwAccessibleContext *>( xAcc.get() );
        pAccImpl->InvalidateFocus();
    }
    else if( bShapeSelected  )
    {
        DoInvalidateShapeFocus();
    }
}

void SwAccessibleMap::SetCursorContext(
        const ::vos::ORef < SwAccessibleContext >& rCursorContext )
{
    vos::OGuard aGuard( maMutex );
    Reference < XAccessible > xAcc( rCursorContext.getBodyPtr() );
    mxCursorContext = xAcc;
}

void SwAccessibleMap::InvalidateStates( sal_uInt8 nStates )
{
    Reference< XAccessible > xAcc( GetDocumentView() );
    SwAccessibleContext *pAccImpl =
        static_cast< SwAccessibleContext *>( xAcc.get() );
    if( GetShell()->ActionPend() )
    {
        SwAccessibleEvent_Impl aEvent(
                SwAccessibleEvent_Impl::CARET_OR_STATES, pAccImpl,
                        pAccImpl->GetFrm(), nStates );
        AppendEvent( aEvent );
    }
    else
    {
        pAccImpl->InvalidateStates( nStates );
    }
}

void SwAccessibleMap::_InvalidateRelationSet( const SwFrm* pFrm, 
                                              sal_Bool bFrom )
{
    // first, see if this frame is accessible, and if so, get the respective 
    SwFrmOrObj aFrmOrObj( pFrm );
    if( aFrmOrObj.IsAccessible() )
    {
        Reference < XAccessible > xAcc;
        Reference < XAccessible > xParentAcc;
        {
            vos::OGuard aGuard( maMutex );

            if( mpFrmMap )
            {
                SwAccessibleContextMap_Impl::iterator aIter =
                    mpFrmMap->find( aFrmOrObj.GetSwFrm() );
                if( aIter != mpFrmMap->end() )
                {
                    xAcc = (*aIter).second;
                }
            }
        }

        // deliver event directly, or queue event
        if( xAcc.is() )
        {
            SwAccessibleContext *pAccImpl =
                static_cast< SwAccessibleContext *>( xAcc.get() );
            if( GetShell()->ActionPend() )
            {
                SwAccessibleEvent_Impl aEvent(
                    SwAccessibleEvent_Impl::CARET_OR_STATES, pAccImpl,
                        pFrm, bFrom ? ACC_STATE_RELATION_FROM 
                                    : ACC_STATE_RELATION_TO );
                AppendEvent( aEvent );
            }
            else
            {
                pAccImpl->InvalidateRelation( bFrom ?
                    AccessibleEventId::CONTENT_FLOWS_FROM :
                    AccessibleEventId::CONTENT_FLOWS_TO );
            }
        }
    }
}

void SwAccessibleMap::InvalidateRelationSet( const SwFrm* pMaster, 
                                             const SwFrm* pFollow )
{
    _InvalidateRelationSet( pMaster, sal_False );
    _InvalidateRelationSet( pFollow, sal_True );
}


void SwAccessibleMap::FireEvents()
{
    {
        vos::OGuard aGuard( maEventMutex );
        if( mpEvents )
        {
            mpEvents->SetFiring();
            SwAccessibleEventList_Impl::iterator aIter = mpEvents->begin();
            while( aIter != mpEvents->end() )
            {
                FireEvent( *aIter );
                ++aIter;
            }

            delete mpEventMap;
            mpEventMap = 0;

            delete mpEvents;
            mpEvents = 0;
        }
    }
    {
        vos::OGuard aGuard( maMutex );
        if( mpShapes )
        {
            delete mpShapes;
            mpShapes = 0;
        }
    }

}

sal_Bool SwAccessibleMap::IsValid() const
{
    return sal_True;
}

Rectangle SwAccessibleMap::GetVisibleArea() const
{
    MapMode aSrc( MAP_TWIP );
    MapMode aDest( MAP_100TH_MM );
    return OutputDevice::LogicToLogic( GetVisArea().SVRect(), aSrc, aDest );
}

Point SwAccessibleMap::LogicToPixel( const Point& rPoint ) const
{
    MapMode aSrc( MAP_100TH_MM );
    MapMode aDest( MAP_TWIP );
    Point aPoint( OutputDevice::LogicToLogic( rPoint, aSrc, aDest ) );
    if( GetShell()->GetWin() )
        aPoint = GetShell()->GetWin()->LogicToPixel( aPoint );

    return aPoint;
}

Size SwAccessibleMap::LogicToPixel( const Size& rSize ) const
{
    MapMode aSrc( MAP_100TH_MM );
    MapMode aDest( MAP_TWIP );
    Size aSize( OutputDevice::LogicToLogic( rSize, aSrc, aDest ) );
    if( GetShell()->GetWin() )
        aSize = GetShell()->GetWin()->LogicToPixel( aSize );

    return aSize;
}

Point SwAccessibleMap::PixelToLogic( const Point& rPoint ) const
{
    Point aPoint;
    if( GetShell()->GetWin() )
    {
        aPoint = GetShell()->GetWin()->PixelToLogic( rPoint );
        MapMode aSrc( MAP_TWIP );
        MapMode aDest( MAP_100TH_MM );
        aPoint = OutputDevice::LogicToLogic( aPoint, aSrc, aDest );
    }

    return aPoint;
}

Size SwAccessibleMap::PixelToLogic( const Size& rSize ) const
{
    Size aSize;
    if( GetShell()->GetWin() )
    {
        aSize = GetShell()->GetWin()->PixelToLogic( rSize );
        MapMode aSrc( MAP_TWIP );
        MapMode aDest( MAP_100TH_MM );
        aSize = OutputDevice::LogicToLogic( aSize, aSrc, aDest );
    }

    return aSize;
}
