 /*************************************************************************
 *
 *  $RCSfile: acccontext.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: mib $ $Date: 2002-04-17 14:07:39 $
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
#if defined DEBUG && defined TEST_MIB
#ifndef _STRING_HXX 
#include <tools/string.hxx>
#endif
#ifndef _STREAM_HXX 
#include <tools/stream.hxx>
#endif
#endif
#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#ifndef _ERRHDL_HXX
#include "errhdl.hxx"
#endif
#ifndef _SWTYPES_HXX
#include "swtypes.hxx"
#endif

#pragma hdrstop

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLESTATESET_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleStateSet.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEEVENTID_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleEventId.hpp>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#include <unotools/accessiblestatesethelper.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>
#endif
#ifndef _ACCMAP_HXX
#include <accmap.hxx>
#endif
#ifndef _ACCFRMOBJSLIST_HXX
#include <accfrmobjslist.hxx>
#endif
#ifndef _ACCCONTEXT_HXX
#include <acccontext.hxx>
#endif

#if defined DEBUG && defined TEST_MIB
#define DBG_MSG( _msg ) \
    lcl_SwAccessibleContext_DbgMsg( this, _msg, 0, sal_False );
#define DBG_MSG_CD( _msg ) \
    lcl_SwAccessibleContext_DbgMsg( this, _msg, 0, sal_True );
#define DBG_MSG_PARAM( _msg, _param ) \
    lcl_SwAccessibleContext_DbgMsg( this, _msg, _param, sal_False );
#define DBG_MSG_THIS_PARAM( _msg, _this, _param ) \
    lcl_SwAccessibleContext_DbgMsg( _this, _msg, _param, sal_False );

void lcl_SwAccessibleContext_DbgMsg( SwAccessibleContext *pThisAcc,
                                     const char *pMsg,
                                     SwAccessibleContext *pChildAcc,
                                       sal_Bool bConstrDestr );
#else
#define DBG_MSG( _msg )
#define DBG_MSG_PARAM( _msg, _param )
#define DBG_MSG_THIS_PARAM( _msg, _this, _param )
#define DBG_MSG_CD( _msg )
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::rtl;

void SwAccessibleContext::InitStates()
{
    bIsShowingState = IsShowing();

    ViewShell *pVSh = GetMap()->GetShell();
    bIsEditableState = pVSh && 	IsEditable( pVSh );
    bIsOpaqueState = pVSh && IsOpaque( pVSh );
    bIsDefuncState = sal_False;
}

void SwAccessibleContext::SetParent( SwAccessibleContext *pParent ) 
{
    vos::OGuard aGuard( aMutex );

    ::com::sun::star::uno::Reference < ::drafts::com::sun::star::accessibility::XAccessible > xParent( pParent );
    xWeakParent = xParent;
}

Reference< XAccessible > SwAccessibleContext::GetWeakParent() const
{
    vos::OGuard aGuard( aMutex );

    Reference< XAccessible > xParent( xWeakParent );
    return xParent;
}

Window *SwAccessibleContext::GetWindow()
{
    Window *pWin = 0;

    if( GetMap() )
    {
        const ViewShell *pVSh = GetMap()->GetShell();
        ASSERT( pVSh, "no view shell" );
        if( pVSh )
            pWin = pVSh->GetWin();

        ASSERT( pWin, "no window" );
    }

    return pWin;
}

enum Action { NONE, SCROLLED, SCROLLED_WITHIN, 
                          SCROLLED_IN, SCROLLED_OUT };

void SwAccessibleContext::ChildrenScrolled( const SwFrm *pFrm,
                                        const SwRect& rOldVisArea )
{
    const SwRect& rNewVisArea = GetVisArea();
    SwFrmOrObj aFrm( pFrm );
    sal_Bool bVisibleOnly = aFrm.IsVisibleChildrenOnly();

    Reference < XAccessible > xAcc;

    const SwFrmOrObjSList aList( pFrm );
    SwFrmOrObjSList::const_iterator aIter( aList.begin() );
    while( aIter != aList.end() )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        SwRect aBox( rLower.GetBox() );
        if( rLower.IsAccessible() )
        {
            Action eAction = NONE;
            if( pLower  )
            {
                if( aBox.IsOver( rNewVisArea ) )
                {
                    if( aBox.IsOver( rOldVisArea ) )
                    {
                        eAction = SCROLLED_WITHIN;
                    }
                    else
                    {
                        if( bVisibleOnly )
                            eAction = SCROLLED_IN;
                        else
                            eAction = SCROLLED;
                    }
                }
                else if( aBox.IsOver( rOldVisArea ) )
                {
                    if( bVisibleOnly )
                        eAction = SCROLLED_OUT;
                    else
                        eAction = SCROLLED;
                }
                else if( !bVisibleOnly )
                {
                    // This wouldn't be required if the SwAccessibleFrame,
                    // wouldn't know about the vis area.
                    eAction = SCROLLED;
                }
                if( NONE != eAction )
                {
                    ::vos::ORef< SwAccessibleContext > xAccImpl = 
                        GetMap()->GetContextImpl( pLower, SCROLLED_OUT == eAction ||
                                                SCROLLED_IN == eAction );
                    if( xAccImpl.isValid() )
                    {
                        switch( eAction )
                        {
                        case SCROLLED:
                            xAccImpl->Scrolled( rOldVisArea );
                            break;
                        case SCROLLED_WITHIN:
                            xAccImpl->ScrolledWithin( rOldVisArea );
                            break;
                        case SCROLLED_IN:
                            xAccImpl->ScrolledIn();
                            break;
                        case SCROLLED_OUT:
                            xAccImpl->ScrolledOut( rOldVisArea );
                            break;
                        }
                    }
                    else
                    {
                        ChildrenScrolled( pLower, rOldVisArea );
                    }
                }
            }
            else
            {
                // TODO: SdrObjects
            }
        }
        else if( pLower && (!bVisibleOnly ||
                            aBox.IsOver( rOldVisArea ) ||
                              aBox.IsOver( rNewVisArea )) )
        {
            // There are no unaccessible SdrObjects that need to be notified
            ChildrenScrolled( pLower, rOldVisArea );
        }
        ++aIter;
    }
}

void SwAccessibleContext::Scrolled( const SwRect& rOldVisArea )
{
    SetVisArea( GetMap()->GetVisArea() );

    ChildrenScrolled( GetFrm(), rOldVisArea );

    sal_Bool bIsOldShowingState;
    sal_Bool bIsNewShowingState = IsShowing();
    {
        vos::OGuard aGuard( aMutex );
        bIsOldShowingState = bIsShowingState;
        bIsShowingState = bIsNewShowingState;
    }

    if( bIsOldShowingState != bIsNewShowingState )
        FireStateChangedEvent( AccessibleStateType::SHOWING, 
                               bIsNewShowingState  );
}

void SwAccessibleContext::ScrolledWithin( const SwRect& rOldVisArea )
{
    SetVisArea( GetMap()->GetVisArea() );

    ChildrenScrolled( GetFrm(), rOldVisArea );

    FireVisibleDataEvent();
}

void SwAccessibleContext::ScrolledIn()
{
    // This accessible should be freshly created, because it
    // was not visisble before. Therefor, its vis area must already
    // reflect the scrolling.
    ASSERT( GetVisArea() == GetMap()->GetVisArea(),
            "Vis area of child is wrong. Did it exist already?" );

    // Send child event at parent. That's all we have to do here.
    const SwFrm *pParent = GetParent();
    ::vos::ORef< SwAccessibleContext > xParentImpl(
         GetMap()->GetContextImpl( pParent, sal_False ) );
    Reference < XAccessibleContext > xThis( this );
    if( xParentImpl.isValid() )
    {
        SetParent( xParentImpl.getBodyPtr() );

        AccessibleEventObject aEvent;
        aEvent.EventId = AccessibleEventId::ACCESSIBLE_CHILD_EVENT;
        aEvent.NewValue <<= xThis;

        xParentImpl->FireAccessibleEvent( aEvent );
        DBG_MSG_PARAM( "AccessibleChild (added)", xChildImpl.getBodyPtr() )
    }
}

void SwAccessibleContext::ScrolledOut( const SwRect& rOldVisArea )
{
    SetVisArea( GetMap()->GetVisArea() );

    // First of all, update the children. That's required to dispose
    // all children that are existing only if they are visible. They
    // are not disposed by the recusive Dispose call that follows later on,
    // because this call will only dispose children that are in the
    // new vis area. The children we want to dispode however are in the
    // old vis area all.
    ChildrenScrolled( GetFrm(), rOldVisArea );

    // Broadcast a state changed event for the showing state. 
    // It might be that the child is freshly created just to send
    // the child event. In this case no listener will exist.
    FireStateChangedEvent( AccessibleStateType::SHOWING, sal_False );

    // We now dispose the frame
    Dispose( sal_True );
}


void SwAccessibleContext::InvalidateChildrenStates( const SwFrm *pFrm,
                                                sal_uInt8 nStates )
{
    const SwFrmOrObjSList aVisList( GetVisArea(), pFrm );

    SwFrmOrObjSList::const_iterator aIter( aVisList.begin() );
    while( aIter != aVisList.end() )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        if( pLower )
        {
            ::vos::ORef< SwAccessibleContext > xAccImpl;
            if( rLower.IsAccessible() )
                xAccImpl = GetMap()->GetContextImpl( pLower, sal_False );
            if( xAccImpl.isValid() )
                xAccImpl->InvalidateStates( nStates );
            else
                InvalidateChildrenStates( pLower, nStates );
        }
        else
        {
            // TODO: SdrObjects
        }
        ++aIter;
    }
}

void SwAccessibleContext::DisposeChildren( const SwFrm *pFrm,
                                       sal_Bool bRecursive )
{
    const SwFrmOrObjSList aVisList( GetVisArea(), pFrm );
    SwFrmOrObjSList::const_iterator aIter( aVisList.begin() );
    while( aIter != aVisList.end() )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        if( pLower )
        {
            ::vos::ORef< SwAccessibleContext > xAccImpl;
            if( rLower.IsAccessible() )
                xAccImpl = GetMap()->GetContextImpl( pLower, sal_False );
            if( xAccImpl.isValid() )
                xAccImpl->Dispose( bRecursive );
            else if( bRecursive )
                DisposeChildren( pLower, bRecursive );
        }
        else
        {
            // TODO: SdrObjects
        }
        ++aIter;
    }
}

void SwAccessibleContext::_InvalidateContent( sal_Bool )
{
}

void SwAccessibleContext::_InvalidateCursorPos()
{
}

void SwAccessibleContext::FireAccessibleEvent( AccessibleEventObject& rEvent )
{
    ASSERT( GetFrm(), "fire event for diposed frame?" );
    if( !GetFrm() )
        return;

    Reference < XAccessibleContext > xThis( this );
    rEvent.Source = xThis;

    ::cppu::OInterfaceIteratorHelper aIter( aAccessibleEventListeners );
    while( aIter.hasMoreElements() )
    {
        Reference < XAccessibleEventListener > xListener( aIter.next(),
                                                         UNO_QUERY );
        if( xListener.is() ) // TODO: test is unneccessary soon
        {
            try
            {
                xListener->notifyEvent( rEvent );
            }
            catch( ::com::sun::star::uno::RuntimeException& r )
            {
#ifdef DEBUG
                ByteString aError( "Runtime exception caught for event" );
                aError += ByteString::CreateFromInt32( rEvent.EventId );
                aError += ".:\n";
                aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
                DBG_ERROR( aError.GetBuffer() );
#endif
//				aIter.remove(); 
            }
        }
    }

}

void SwAccessibleContext::FireVisibleDataEvent()
{
    AccessibleEventObject aEvent;
    aEvent.EventId = AccessibleEventId::ACCESSIBLE_VISIBLE_DATA_EVENT;

    FireAccessibleEvent( aEvent );
    DBG_MSG( "AccessibleVisibleData" )
}

void SwAccessibleContext::FireStateChangedEvent( sal_Int16 nState,
                                                 sal_Bool bNewState )
{
    AccessibleEventObject aEvent;

    aEvent.EventId = AccessibleEventId::ACCESSIBLE_STATE_EVENT;
    if( bNewState )
        aEvent.NewValue <<= nState;
    else
        aEvent.OldValue <<= nState;

    FireAccessibleEvent( aEvent );
    DBG_MSG( "StateChanged" )
}

void SwAccessibleContext::GetStates(
        ::utl::AccessibleStateSetHelper& rStateSet )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // SHOWING
    if( bIsShowingState )
        rStateSet.AddState( AccessibleStateType::SHOWING );

    // EDITABLE
    if( bIsEditableState )
        rStateSet.AddState( AccessibleStateType::EDITABLE );

    // ENABLED
    rStateSet.AddState( AccessibleStateType::ENABLED );

    // OPAQUE
    if( bIsOpaqueState )
        rStateSet.AddState( AccessibleStateType::OPAQUE );

    // VISIBLE
    rStateSet.AddState( AccessibleStateType::VISIBLE );

    if( bIsDefuncState )
        rStateSet.AddState( AccessibleStateType::DEFUNC );
}

SwAccessibleContext::SwAccessibleContext( SwAccessibleMap *pM,
                                          sal_Int16 nR,
                                          const SwFrm *pF ) :
    SwAccessibleFrame( pM->GetShell()->VisArea().SVRect(), pF ),
    aAccessibleEventListeners( aListenerMutex ),
    aFocusListeners( aListenerMutex ),
    pMap( pM ),
    nRole( nR ),
    bDisposing( sal_False )
{
    InitStates();
    DBG_MSG_CD( "constructed" )
}

SwAccessibleContext::SwAccessibleContext( SwAccessibleMap *pM,
                                          const OUString& rName,
                                          sal_Int16 nR,
                                          const SwFrm *pF ) :
    SwAccessibleFrame( pM->GetShell()->VisArea().SVRect(), pF ),
    sName( rName ),
    aAccessibleEventListeners( aListenerMutex ),
    aFocusListeners( aListenerMutex ),
    pMap( pM ),
    nRole( nR ),
    bDisposing( sal_False )
{
    InitStates();
    DBG_MSG_CD( "constructed" )
}

SwAccessibleContext::~SwAccessibleContext()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    DBG_MSG_CD( "destructed" )
    if( GetFrm() && GetMap() )
        GetMap()->RemoveContext( GetFrm() );
}

Reference< XAccessibleContext > SAL_CALL
    SwAccessibleContext::getAccessibleContext( void )
        throw (::com::sun::star::uno::RuntimeException)
{
    Reference < XAccessibleContext > xRet( this );
    return xRet;
}

sal_Int32 SAL_CALL SwAccessibleContext::getAccessibleChildCount( void )
        throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    return bDisposing ? 0 : GetChildCount();
}

Reference< XAccessible> SAL_CALL 
    SwAccessibleContext::getAccessibleChild( long nIndex )
        throw (::com::sun::star::uno::RuntimeException,
                ::com::sun::star::lang::IndexOutOfBoundsException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    
    CHECK_FOR_DEFUNC( XAccessibleContext )

    const SwFrmOrObj aChild( GetChild( nIndex ) );
    if( !aChild.IsValid() )
    {
        Reference < XAccessibleContext > xThis( this );
        IndexOutOfBoundsException aExcept(
                OUString( RTL_CONSTASCII_USTRINGPARAM("index out of bounds") ),
                xThis );										\
        throw aExcept;
    }

    Reference< XAccessible > xChild;
    if( aChild.GetSwFrm() )
    {
        ::vos::ORef < SwAccessibleContext > xChildImpl(
                GetMap()->GetContextImpl( aChild.GetSwFrm(), !bDisposing )  );
        if( xChildImpl.isValid() )
        {
            xChildImpl->SetParent( this );
            xChild = xChildImpl.getBodyPtr();
        }
    }
    else
    {
        // TODO: SdrObjects
    }

    return xChild;
}

Reference< XAccessible> SAL_CALL SwAccessibleContext::getAccessibleParent (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    const SwFrm *pUpper = GetParent();
    ASSERT( pUpper != 0 || bDisposing, "no upper found" );

    Reference< XAccessible > xAcc;
    if( pUpper )
        xAcc = GetMap()->GetContext( pUpper, !bDisposing );

    ASSERT( xAcc.is() || bDisposing, "no parent found" );

    // Remember the parent as weak ref.
    {
        vos::OGuard aGuard( aMutex );
        xWeakParent = xAcc;
    }

    return xAcc;
}

sal_Int32 SAL_CALL SwAccessibleContext::getAccessibleIndexInParent (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    const SwFrm *pUpper = GetParent();
    ASSERT( pUpper != 0 || bDisposing, "no upper found" );

    sal_Int32 nIndex = -1;
    if( pUpper )
    {
        ::vos::ORef < SwAccessibleContext > xAccImpl(
            GetMap()->GetContextImpl( pUpper, !bDisposing )  );
        ASSERT( xAccImpl.isValid() || bDisposing, "no parent found" );
        if( xAccImpl.isValid() )
            nIndex = xAccImpl->GetChildIndex( GetFrm() );
    }

    return nIndex;
}

sal_Int16 SAL_CALL SwAccessibleContext::getAccessibleRole (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    return nRole;
}

OUString SAL_CALL SwAccessibleContext::getAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    ASSERT( !this, "description needs to be overloaded" );
    THROW_RUNTIME_EXCEPTION( XAccessibleContext, "internal error (method must be overloaded)" );
}

OUString SAL_CALL SwAccessibleContext::getAccessibleName (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    return sName;
}

Reference< XAccessibleRelationSet> SAL_CALL
    SwAccessibleContext::getAccessibleRelationSet (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    // by default there are no relations
    Reference< XAccessibleRelationSet> xRet;
    return xRet;
}

Reference<XAccessibleStateSet> SAL_CALL
    SwAccessibleContext::getAccessibleStateSet (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    ::utl::AccessibleStateSetHelper *pStateSet = 
        new ::utl::AccessibleStateSetHelper;

    Reference<XAccessibleStateSet> xStateSet( pStateSet );
    GetStates( *pStateSet );

    return xStateSet;
}

Locale SAL_CALL SwAccessibleContext::getLocale (void)
        throw (::drafts::com::sun::star::accessibility::IllegalAccessibleComponentStateException, ::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    Locale aLoc( Application::GetSettings().GetUILocale() );
    return aLoc;
}

void SAL_CALL SwAccessibleContext::addEventListener(
            const Reference< XAccessibleEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException)
{
    DBG_MSG( "accessible event listener added" )
    aAccessibleEventListeners.addInterface( xListener );
}

void SAL_CALL SwAccessibleContext::removeEventListener(
            const Reference< XAccessibleEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException)
{
    DBG_MSG( "accessible event listener removed" )
    aAccessibleEventListeners.removeInterface( xListener );
}

sal_Bool SAL_CALL SwAccessibleContext::contains(
            const ::com::sun::star::awt::Point& aPoint )
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    Window *pWin = GetWindow();
    CHECK_FOR_WINDOW( XAccessibleComponent, pWin )

    SwRect aLogBounds( GetBounds( GetFrm() ) ); // twip rel to doc root

    Point aPixPoint( aPoint.X, aPoint.Y ); // px rel to window
    Point aLogPoint( pWin->PixelToLogic( aPixPoint ) ); // twip rel to doc root

    return aLogBounds.IsInside( aLogPoint );
}


Reference< XAccessible > SAL_CALL SwAccessibleContext::getAccessibleAt(
                const awt::Point& aPoint )
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    Reference< XAccessible > xAcc;

    Window *pWin = GetWindow();
    CHECK_FOR_WINDOW( XAccessibleComponent, pWin )

    Point aPixPoint( aPoint.X, aPoint.Y ); // px rel to window
    Point aLogPoint( pWin->PixelToLogic( aPixPoint ) ); // twip rel to doc root

    const SwFrmOrObj aChild( GetChildAt( aLogPoint ) );
    if( aChild.GetSwFrm() )
    {
        xAcc = GetMap()->GetContext( aChild.GetSwFrm() );
    }
    else if( aChild.GetSdrObject() )
    {
        // TODO: SdrObjects
    }

    return xAcc;
}


awt::Rectangle SAL_CALL SwAccessibleContext::getBounds()
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    const SwFrm *pParent = GetParent();
    ASSERT( pParent, "no Parent found" );
    Window *pWin = GetWindow();

    CHECK_FOR_WINDOW( XAccessibleComponent, pWin && pParent )

    SwRect aLogBounds( GetBounds( GetFrm() ) ); // twip rel to doc root
    Rectangle aPixBounds( 0, 0, 0, 0 );
    if( !aLogBounds.IsEmpty() )
    {
        aPixBounds = pWin->LogicToPixel( aLogBounds.SVRect() );
        if( !pParent->IsRootFrm() )
        {
            Point aParentLogPos( GetBounds( pParent ).Pos() ); // twip rel to doc root
            Point aParentPixPos( pWin->LogicToPixel( aParentLogPos ) );
            aPixBounds.Left() -= aParentPixPos.X();
            aPixBounds.Top() -= aParentPixPos.Y();
        }
    }

    awt::Rectangle aBox( aPixBounds.Left(), aPixBounds.Top(),
                         aPixBounds.GetWidth(), aPixBounds.GetHeight() );

    return aBox;
}


awt::Point SAL_CALL SwAccessibleContext::getLocation()
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    const SwFrm *pParent = GetParent();
    ASSERT( pParent, "no parent found" );
    Window *pWin = GetWindow();

    CHECK_FOR_WINDOW( XAccessibleComponent, pWin && pParent )

    SwRect aLogBounds( GetBounds( GetFrm() ) ); // twip rel to doc root
    Point aPixPos( 0, 0 );
    if( !aLogBounds.IsEmpty() )
    {
        aPixPos = pWin->LogicToPixel( aLogBounds.Pos() );
        if( pParent->IsRootFrm() )
        {
            Point aParentLogPos( GetBounds( pParent ).Pos() ); // twip rel to doc root
            Point aParentPixPos( pWin->LogicToPixel( aParentLogPos ) );
            aPixPos.X() -= aParentPixPos.X();
            aPixPos.Y() -= aParentPixPos.Y();
        }
    }
    awt::Point aLoc( aPixPos.X(), aPixPos.Y() );
                         
    return aLoc;
}


::com::sun::star::awt::Point SAL_CALL SwAccessibleContext::getLocationOnScreen()
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    Window *pWin = GetWindow();
    CHECK_FOR_WINDOW( XAccessibleComponent, pWin )

    SwRect aLogBounds( GetBounds( GetFrm() ) ); // twip rel to doc root
    Point aPixPos( 0, 0 );
    if( !aLogBounds.IsEmpty() )
    {
        aPixPos = pWin->LogicToPixel( aLogBounds.Pos() );
        aPixPos = pWin->OutputToAbsoluteScreenPixel( aPixPos );
    }
    awt::Point aLoc( aPixPos.X(), aPixPos.Y() );
                         
    return aLoc;
}


::com::sun::star::awt::Size SAL_CALL SwAccessibleContext::getSize()
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleComponent )

    Window *pWin = GetWindow();
    CHECK_FOR_WINDOW( XAccessibleComponent, pWin )

    SwRect aLogBounds( GetBounds( GetFrm() ) ); // twip rel to doc root
    Size aPixSize( 0, 0 );
    // The size may differ by one pixel, dependent on the position of
    // the rectangle. For that reason we first have to do the conversion
    // into pixel and then have to get the size.
    if( !aLogBounds.IsEmpty() )
        aPixSize = pWin->LogicToPixel( aLogBounds.SVRect() ).GetSize();
    awt::Size aSize( aPixSize.Width(), aPixSize.Height() );

    return aSize;
}


sal_Bool SAL_CALL SwAccessibleContext::isShowing()
        throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return IsShowing();
}


sal_Bool SAL_CALL SwAccessibleContext::isVisible()
        throw (RuntimeException)
{
    return sal_True;
}


sal_Bool SAL_CALL SwAccessibleContext::isFocusTraversable()
        throw (RuntimeException)
{
    return sal_False;
}

    
void SAL_CALL SwAccessibleContext::addFocusListener(
            const Reference<
                ::com::sun::star::awt::XFocusListener >& xListener )
        throw (RuntimeException)
{
    DBG_MSG( "focus listener added" )
    aFocusListeners.addInterface( xListener );
}


void SAL_CALL SwAccessibleContext::removeFocusListener(
            const Reference<
                ::com::sun::star::awt::XFocusListener >& xListener )
        throw (RuntimeException)
{
    DBG_MSG( "focus listener removed" )
    aFocusListeners.removeInterface( xListener );
}

void SAL_CALL SwAccessibleContext::grabFocus()
        throw (RuntimeException)
{
    // impossible
}


Any SAL_CALL SwAccessibleContext::getAccessibleKeyBinding()
        throw (RuntimeException)
{
    // There are no key bindings
    Any aAny;
    return aAny;
}


OUString SAL_CALL SwAccessibleContext::getImplementationName()
        throw( RuntimeException )
{
    ASSERT( !this, "implementation name needs to be overloaded" );

    THROW_RUNTIME_EXCEPTION( XServiceInfo, "implementation name needs to be overloaded" )
}

sal_Bool SAL_CALL
    SwAccessibleContext::supportsService (const ::rtl::OUString& sServiceName)
        throw (::com::sun::star::uno::RuntimeException)
{
    ASSERT( !this, "supports service needs to be overloaded" );
    THROW_RUNTIME_EXCEPTION( XServiceInfo, "supports service needs to be overloaded" )
}

Sequence< OUString > SAL_CALL SwAccessibleContext::getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException )
{
    ASSERT( !this, "supported services names needs to be overloaded" );
    THROW_RUNTIME_EXCEPTION( XServiceInfo, "supported services needs to be overloaded" )
}

void SwAccessibleContext::Dispose( sal_Bool bRecursive )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    ASSERT( GetFrm() && GetMap(), "already disposed" );
    ASSERT( GetMap()->GetVisArea() == GetVisArea(),
                "invalid vis area for dispose" );

    bDisposing = sal_True;

    // dispose children
    if( bRecursive )
        DisposeChildren( GetFrm(), bRecursive );

    // get parent
    Reference< XAccessible > xParent( GetWeakParent() );
    Reference < XAccessibleContext > xThis( this );

    // send child event at parent
    if( xParent.is() )
    {
        SwAccessibleContext *pAcc = (SwAccessibleContext *)xParent.get();

        AccessibleEventObject aEvent;
        aEvent.EventId = AccessibleEventId::ACCESSIBLE_CHILD_EVENT;
        aEvent.OldValue <<= xThis;
        pAcc->FireAccessibleEvent( aEvent );
        DBG_MSG_THIS_PARAM( "AccessibleChild (removed)", pAcc, this )
    }

    // set defunc state
    {
        vos::OGuard aGuard( aMutex );
        bIsDefuncState = sal_True;
    }

    // broadcast state change
    FireStateChangedEvent( AccessibleStateType::DEFUNC, sal_True );

    // broadcast dispose event
    {
        EventObject aEvent;
        aEvent.Source = xThis;
        aAccessibleEventListeners.disposeAndClear( aEvent );
        aFocusListeners.disposeAndClear( aEvent );
        DBG_MSG_CD( "dispose" )
    }

    if( GetMap() && GetFrm() )
        GetMap()->RemoveContext( GetFrm() );	
    ClearFrm();
    pMap = 0;

    bDisposing = sal_False;
}

void SwAccessibleContext::DisposeChild( const SwFrm *pFrm, sal_Bool bRecursive )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFrmOrObj aFrm( GetFrm() );
    if( IsShowing( pFrm ) || !aFrm.IsVisibleChildrenOnly() )
    {
        // If the object could have existed before, than there is nothing to do,
        // because no wrapper exists now and therefor no one is interested to
        // get notified of the movement.
        ::vos::ORef< SwAccessibleContext > xAccImpl =
                GetMap()->GetContextImpl( pFrm, sal_True );
        xAccImpl->Dispose( bRecursive );
    }
    else if( bRecursive )
        DisposeChildren( GetFrm(), bRecursive );
}

void SwAccessibleContext::InvalidatePosOrSize( const SwRect& rOldPos )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    sal_Bool bIsOldShowingState;
    sal_Bool bIsNewShowingState = IsShowing();
    {
        vos::OGuard aGuard( aMutex );
        bIsOldShowingState = bIsShowingState;
        bIsShowingState = bIsNewShowingState;
    }

    if( bIsOldShowingState != bIsNewShowingState )
    {
        FireStateChangedEvent( AccessibleStateType::SHOWING, 
                               bIsNewShowingState  );
    }
    else if( bIsNewShowingState )
    {
        // The frame stays visible -> broadcast event
        FireVisibleDataEvent();
    }

    SwFrmOrObj aParent( GetParent() );
    if( !bIsNewShowingState && aParent.IsVisibleChildrenOnly() )
    {
        // The frame is now invisible -> dispose it
        Dispose( sal_True );
    }
    else
    {
        _InvalidateContent( sal_True );
    }
}

void SwAccessibleContext::InvalidateChildPosOrSize( const SwFrm *pFrm, 
                                           const SwRect& rOldFrm )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFrmOrObj aFrm( GetFrm() );
    sal_Bool bNew = rOldFrm.IsEmpty() || (rOldFrm.Left() == 0 && rOldFrm.Top());
    if( IsShowing( pFrm ) )
    {
        // If the object could have existed before, than there is nothing to do,
        // because no wrapper exists now and therefor no one is interested to
        // get notified of the movement.
        if( bNew || (aFrm.IsVisibleChildrenOnly() && !IsShowing( rOldFrm )) )
        {
            // The frame becomes visible. A child event must be send.
            ::vos::ORef< SwAccessibleContext > xAccImpl =
                GetMap()->GetContextImpl( pFrm, sal_True );
            xAccImpl->ScrolledIn();
        }
    }
    else
    {
        // If the frame was visible before, than a child event for the parent
        // needs to be send. However, there is no wrapper existing, and so
        // no notifications for grandchildren are required. If the are
        // grandgrandchildren, they would be notified by the layout.
        if( aFrm.IsVisibleChildrenOnly() &&
            !bNew && IsShowing( rOldFrm ) )
        {
            ::vos::ORef< SwAccessibleContext > xAccImpl =
                GetMap()->GetContextImpl( pFrm, sal_True );
            xAccImpl->SetParent( this );
            xAccImpl->Dispose( sal_True );
        }
    }
}

void SwAccessibleContext::InvalidateContent()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    _InvalidateContent( sal_False );
}

void SwAccessibleContext::InvalidateCursorPos()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    _InvalidateCursorPos();
}

void SwAccessibleContext::InvalidateStates( sal_uInt8 nStates )
{
    if( GetMap() )
    {
        ViewShell *pVSh = GetMap()->GetShell();
        if( pVSh )
        {
            if( (nStates & ACC_STATE_EDITABLE) != 0 )
            {
                sal_Bool bIsOldEditableState;
                sal_Bool bIsNewEditableState = IsEditable( pVSh );
                {
                    vos::OGuard aGuard( aMutex );
                    bIsOldEditableState = bIsEditableState;
                    bIsEditableState = bIsNewEditableState;
                }

                if( bIsOldEditableState != bIsNewEditableState )
                    FireStateChangedEvent( AccessibleStateType::EDITABLE, 
                                           bIsNewEditableState  );
            }
            if( (nStates & ACC_STATE_OPAQUE) != 0 )
            {
                sal_Bool bIsOldOpaqueState;
                sal_Bool bIsNewOpaqueState = IsOpaque( pVSh );
                {
                    vos::OGuard aGuard( aMutex );
                    bIsOldOpaqueState = bIsOpaqueState;
                    bIsOpaqueState = bIsNewOpaqueState;
                }

                if( bIsOldOpaqueState != bIsNewOpaqueState )
                    FireStateChangedEvent( AccessibleStateType::OPAQUE, 
                                           bIsNewOpaqueState  );
            }
        }

        InvalidateChildrenStates( GetFrm(), nStates );
    }
}

sal_Bool SwAccessibleContext::HasCursor()
{
    return sal_False;
}

OUString SwAccessibleContext::GetResource( sal_uInt16 nResId,
                                           const OUString *pArg1,
                                           const OUString *pArg2 )
{
    String sStr;
    {
        vos::OGuard aGuard(Application::GetSolarMutex());

        sStr = SW_RES( nResId );
    }

    if( pArg1 )
    {
        sStr.SearchAndReplace( String::CreateFromAscii(
                                    RTL_CONSTASCII_STRINGPARAM( "$(ARG1)" )),
                               String( *pArg1 ) );
    }
    if( pArg2 )
    {
        sStr.SearchAndReplace( String::CreateFromAscii(
                                    RTL_CONSTASCII_STRINGPARAM( "$(ARG2)" )),
                               String( *pArg2 ) );
    }

    return OUString( sStr );
}



#if defined DEBUG && defined TEST_MIB
void lcl_SwAccessibleContext_DbgMsg( SwAccessibleContext *pThisAcc,
                                     const char *pMsg,
                                     SwAccessibleContext *pChildAcc,
                                       sal_Bool bConstrDestr )
{
    static SvFileStream aStrm( String::CreateFromAscii("j:\\acc.log"),
                    STREAM_WRITE|STREAM_TRUNC|STREAM_SHARE_DENYNONE	);
    ByteString aName( String(pThisAcc->GetName()), 
                      RTL_TEXTENCODING_ISO_8859_1 );
    if( aName.Len() )
    {
        aStrm << aName.GetBuffer()
              << ": ";
    }
    aStrm << pMsg;
    if( pChildAcc )
    {
        ByteString aChild( String(pChildAcc->GetName()), 
                           RTL_TEXTENCODING_ISO_8859_1 );
        aStrm << ": "
              << aChild.GetBuffer();
    }
    aStrm << "\r\n    (";

    if( !bConstrDestr )
    {
        ByteString aDesc( String(pThisAcc->getAccessibleDescription()), 
                           RTL_TEXTENCODING_ISO_8859_1 );
        aStrm << aDesc.GetBuffer()
              << ", ";
    }

    Rectangle aVisArea( pThisAcc->GetVisArea() );
    aStrm << "VA: "
          << ByteString::CreateFromInt32( aVisArea.Left() ).GetBuffer()
          << ","
          << ByteString::CreateFromInt32( aVisArea.Top() ).GetBuffer()
          << ","
          << ByteString::CreateFromInt32( aVisArea.GetWidth() ).GetBuffer()
          << ","
          << ByteString::CreateFromInt32( aVisArea.GetHeight() ).GetBuffer();

    if( pThisAcc->GetFrm() )
    {
        Rectangle aBounds( pThisAcc->GetBounds( pThisAcc->GetFrm() ) );
        aStrm << ", BB: "
              << ByteString::CreateFromInt32( aBounds.Left() ).GetBuffer()
              << ","
              << ByteString::CreateFromInt32( aBounds.Top() ).GetBuffer()
              << ","
              << ByteString::CreateFromInt32( aBounds.GetWidth() ).GetBuffer()
              << ","
              << ByteString::CreateFromInt32( aBounds.GetHeight() ).GetBuffer()
              << ")\r\n";
    }

    aStrm.Flush();
}
#endif
