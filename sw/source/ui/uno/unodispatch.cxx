/*************************************************************************
 *
 *  $RCSfile: unodispatch.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: os $ $Date: 2000-11-07 14:42:59 $
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

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _UNODISPATCH_HXX
#include <unodispatch.hxx>
#endif
#ifndef _SWVIEW_HXX
#include <view.hxx>
#endif
#include <cmdid.h>


using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::beans;
using namespace rtl;
using namespace vos;

const char* cURLStart 			= ".uno:DataSourceBrowser/";
const char* cURLFormLetter 		= ".uno:DataSourceBrowser/FormLetter";
const char* cURLInsertContent 	= ".uno:DataSourceBrowser/InsertContent";//data into fields
const char* cURLInsertColumns 	= ".uno:DataSourceBrowser/InsertColumns";//data into text

/*-- 07.11.00 13:25:51---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDispatchProviderInterceptor::SwXDispatchProviderInterceptor(SwView& rVw) :
    m_rView(rVw)
{
    SfxFrame* pFrame = m_rView.GetViewFrame()->GetFrame();
    Reference< XFrame> xUnoFrame = pFrame->GetFrameInterface();
    m_xIntercepted = Reference< XDispatchProviderInterception>(xUnoFrame, UNO_QUERY);
    if(m_xIntercepted.is())
    {
        m_refCount++;
        m_xIntercepted->registerDispatchProviderInterceptor((XDispatchProviderInterceptor*)this);
        // this should make us the top-level dispatch-provider for the component, via a call to our
        // setDispatchProvider we should have got an fallback for requests we (i.e. our master) cannot fullfill
        Reference< XComponent> xInterceptedComponent(m_xIntercepted, UNO_QUERY);
        if (xInterceptedComponent.is())
            xInterceptedComponent->addEventListener((XEventListener*)this);
        m_refCount--;
    }
}
/*-- 07.11.00 13:25:51---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDispatchProviderInterceptor::~SwXDispatchProviderInterceptor()
{
}
/*-- 07.11.00 13:25:51---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XDispatch > SwXDispatchProviderInterceptor::queryDispatch(
    const URL& aURL, const OUString& aTargetFrameName, sal_Int32 nSearchFlags )
        throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    Reference< XDispatch> xResult;
    // create some dispatch ...
    if(!aURL.Complete.compareToAscii(cURLStart, 23))
    {
        if(!aURL.Complete.compareToAscii(cURLFormLetter))
        {
            if(!m_xFormLetterDispatch.is())
                m_xFormLetterDispatch = new SwXDispatch(m_rView, FN_DB_FORM_LETTER	  );
            xResult = m_xFormLetterDispatch;
        }
        else if(!aURL.Complete.compareToAscii(cURLInsertContent))
        {
            if(!m_xInsertContentDispatch.is())
                m_xInsertContentDispatch = new SwXDispatch(m_rView, FN_DB_INSERT_CONTENT);
            xResult = m_xInsertContentDispatch;
        }
        else if(!aURL.Complete.compareToAscii(cURLInsertColumns))
        {
            if(!m_xInsertColumnsDispatch.is())
                m_xInsertColumnsDispatch = new SwXDispatch(m_rView, FN_DB_INSERT_COLUMNS);
            xResult = m_xInsertColumnsDispatch;
        }
    }

    // ask our slave provider
    if (!xResult.is() && m_xSlaveDispatcher.is())
        xResult = m_xSlaveDispatcher->queryDispatch(aURL, aTargetFrameName, nSearchFlags);

    return xResult;
}
/*-- 07.11.00 13:25:52---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence< Reference< XDispatch > > SwXDispatchProviderInterceptor::queryDispatches(
    const Sequence< DispatchDescriptor >& aDescripts ) throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    Sequence< Reference< XDispatch> > aReturn(aDescripts.getLength());
    Reference< XDispatch>* pReturn = aReturn.getArray();
    const DispatchDescriptor* pDescripts = aDescripts.getConstArray();
    for (sal_Int16 i=0; i<aDescripts.getLength(); ++i, ++pReturn, ++pDescripts)
    {
        *pReturn = queryDispatch(pDescripts->FeatureURL,
                pDescripts->FrameName, pDescripts->SearchFlags);
    }
    return aReturn;
}
/*-- 07.11.00 13:25:52---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XDispatchProvider > SwXDispatchProviderInterceptor::getSlaveDispatchProvider(  )
        throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    return m_xSlaveDispatcher;
}
/*-- 07.11.00 13:25:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatchProviderInterceptor::setSlaveDispatchProvider(
    const Reference< XDispatchProvider >& xNewDispatchProvider ) throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    m_xSlaveDispatcher = xNewDispatchProvider;
}
/*-- 07.11.00 13:25:52---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XDispatchProvider > SwXDispatchProviderInterceptor::getMasterDispatchProvider(  )
        throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    return m_xMasterDispatcher;
}
/*-- 07.11.00 13:25:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatchProviderInterceptor::setMasterDispatchProvider(
    const Reference< XDispatchProvider >& xNewSupplier ) throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    m_xMasterDispatcher = xNewSupplier;
}
/*-- 07.11.00 13:25:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatchProviderInterceptor::disposing( const EventObject& Source )
    throw(RuntimeException)
{
    OGuard aGuard(Application::GetSolarMutex());
    if (m_xIntercepted.is())
    {
        m_xIntercepted->releaseDispatchProviderInterceptor((XDispatchProviderInterceptor*)this);
        Reference< XComponent> xInterceptedComponent(m_xIntercepted, UNO_QUERY);
        if (xInterceptedComponent.is())
            xInterceptedComponent->removeEventListener((XEventListener*)this);
        m_xFormLetterDispatch		= 0;
        m_xInsertContentDispatch	= 0;
        m_xInsertColumnsDispatch	= 0;
    }
    m_xIntercepted = NULL;
}
/* -----------------------------07.11.00 14:26--------------------------------

 ---------------------------------------------------------------------------*/
SwXDispatch::SwXDispatch(SwView& rVw, sal_uInt16 nSlotId) :
    m_rView(rVw),
    m_nSlot(nSlotId)
{
}
/*-- 07.11.00 14:26:13---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDispatch::~SwXDispatch()
{
}
/*-- 07.11.00 14:26:13---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatch::dispatch(
    const URL& aURL, const Sequence< PropertyValue >& aArgs ) throw(RuntimeException)
{
    DBG_ERROR("not implemented")
}
/*-- 07.11.00 14:26:13---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatch::addStatusListener(
    const Reference< XStatusListener >& xControl, const URL& aURL ) throw(RuntimeException)
{
    FeatureStateEvent aEvent;
    aEvent.IsEnabled = sal_True;
    xControl->statusChanged( aEvent );
    DBG_ERROR("not implemented")
}
/*-- 07.11.00 14:26:15---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDispatch::removeStatusListener(
    const Reference< XStatusListener >& xControl, const URL& aURL ) throw(RuntimeException)
{
    DBG_ERROR("not implemented")
}
