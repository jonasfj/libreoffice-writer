/*************************************************************************
 *
 *  $RCSfile: unomodule.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2004-01-28 19:39:21 $
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

// System - Includes -----------------------------------------------------

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTSTATE_HPP_
#include <com/sun/star/frame/DispatchResultState.hpp>
#endif

#include "swmodule.hxx"
#include "swdll.hxx"
#include "unomodule.hxx"
#include <sfx2/objface.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/request.hxx>

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

using namespace ::com::sun::star;

::rtl::OUString SAL_CALL SwUnoModule_getImplementationName() throw( uno::RuntimeException )
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Writer.WriterModule" ) );
}

uno::Sequence< rtl::OUString > SAL_CALL SwUnoModule_getSupportedServiceNames() throw( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aSeq( 1 );
    aSeq[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.ModuleDispatcher"));
    return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SwUnoModule_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    return uno::Reference< frame::XDispatch >( new SwUnoModule( rSMgr ) );
}

    // XNotifyingDispatch
void SAL_CALL SwUnoModule::dispatchWithNotification( const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    // there is no guarantee, that we are holded alive during this method!
    // May the outside dispatch container will be updated by a CONTEXT_CHANGED
    // asynchronous ...
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xThis(static_cast< ::com::sun::star::frame::XNotifyingDispatch* >(this));

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    SwDLL::Init();
    const SfxSlot* pSlot = SW_MOD()->GetInterface()->GetSlot( aURL.Complete );

    sal_Int16 aState = ::com::sun::star::frame::DispatchResultState::DONTKNOW;
    if ( !pSlot )
        aState = ::com::sun::star::frame::DispatchResultState::FAILURE;
    else
    {
        SfxRequest aReq( pSlot, aArgs, SFX_CALLMODE_SYNCHRON, SW_MOD()->GetPool() );
        const SfxPoolItem* pResult = SW_MOD()->ExecuteSlot( aReq );
        if ( pResult )
            aState = ::com::sun::star::frame::DispatchResultState::SUCCESS;
        else
            aState = ::com::sun::star::frame::DispatchResultState::FAILURE;
    }

    if ( xListener.is() )
    {
        xListener->dispatchFinished(
            ::com::sun::star::frame::DispatchResultEvent(
                    xThis, aState, ::com::sun::star::uno::Any()));
    }
}

    // XDispatch
void SAL_CALL SwUnoModule::dispatch( const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs ) throw( ::com::sun::star::uno::RuntimeException )
{
    dispatchWithNotification(aURL, aArgs, ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >());
}

void SAL_CALL SwUnoModule::addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL SwUnoModule::removeStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException )
{
}

SEQUENCE< REFERENCE< XDISPATCH > > SAL_CALL SwUnoModule::queryDispatches( const SEQUENCE< DISPATCHDESCRIPTOR >& seqDescripts ) throw( ::com::sun::star::uno::RuntimeException )
{
    sal_Int32 nCount = seqDescripts.getLength();
    SEQUENCE< REFERENCE< XDISPATCH > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
    {
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL  ,
                                        seqDescripts[i].FrameName   ,
                                        seqDescripts[i].SearchFlags );
    }

    return lDispatcher;
}

// XDispatchProvider
REFERENCE< XDISPATCH > SAL_CALL SwUnoModule::queryDispatch( const UNOURL& aURL, const OUSTRING& sTargetFrameName, sal_Int32 eSearchFlags	) throw( RUNTIMEEXCEPTION )
{
    REFERENCE< XDISPATCH > xReturn;

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    SwDLL::Init();
    const SfxSlot* pSlot = SW_MOD()->GetInterface()->GetSlot( aURL.Complete );
    if ( pSlot )
        xReturn = REFERENCE< XDISPATCH >(static_cast< XDISPATCH* >(this), ::com::sun::star::uno::UNO_QUERY);

    return xReturn;
}

// XServiceInfo
::rtl::OUString SAL_CALL SwUnoModule::getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return SwUnoModule_getImplementationName();
}

sal_Bool SAL_CALL SwUnoModule::supportsService( const ::rtl::OUString& sServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
    UNOSEQUENCE< UNOOUSTRING >  seqServiceNames =   getSupportedServiceNames();
    const UNOOUSTRING*          pArray          =   seqServiceNames.getConstArray();
    for ( sal_Int32 nCounter=0; nCounter<seqServiceNames.getLength(); nCounter++ )
    {
        if ( pArray[nCounter] == sServiceName )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL SwUnoModule::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return SwUnoModule_getSupportedServiceNames();
}

