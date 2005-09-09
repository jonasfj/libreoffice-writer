/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dlelstnr.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 11:18:22 $
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


#pragma hdrstop


#ifndef _COM_SUN_STAR_LINGUISTIC2_DICTIONARYLISTEVENTFLAGS_HPP_
#include <com/sun/star/linguistic2/DictionaryListEventFlags.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC_XDICTIONARYLIST_HPP_
#include <com/sun/star/linguistic2/XDictionaryList.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_XLINGUSERVICEMANAGER_HPP_
#include <com/sun/star/linguistic2/XLinguServiceManager.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_LINGUSERVICEEVENTFLAGS_HPP_
#include <com/sun/star/linguistic2/LinguServiceEventFlags.hpp>
#endif

#include <com/sun/star/uno/Reference.h>

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _DLELSTNR_HXX_
#include "dlelstnr.hxx"
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWVIEW_HXX
#include <view.hxx>
#endif


using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::linguistic2;
using namespace ::com::sun::star::linguistic2::LinguServiceEventFlags;

#define A2OU(x)	OUString::createFromAscii(x)

/* -----------------------------17.03.00 09:07--------------------------------

 ---------------------------------------------------------------------------*/
SwLinguServiceEventListener::SwLinguServiceEventListener()
{
    Reference< XMultiServiceFactory > xMgr( comphelper::getProcessServiceFactory() );
    if (xMgr.is())
    {
        OUString aSvcName( A2OU( "com.sun.star.frame.Desktop" ) );
        xDesktop = Reference< frame::XDesktop >(
                xMgr->createInstance( aSvcName ), UNO_QUERY );
        if (xDesktop.is())
            xDesktop->addTerminateListener( this );
    
        aSvcName = A2OU( "com.sun.star.linguistic2.LinguServiceManager" );
        xLngSvcMgr = Reference< XLinguServiceManager >(
                xMgr->createInstance( aSvcName ), UNO_QUERY );
        if (xLngSvcMgr.is())
            xLngSvcMgr->addLinguServiceManagerListener( 
                (XLinguServiceEventListener *) this );
    }
}
/* -----------------------------17.03.00 09:07--------------------------------

 ---------------------------------------------------------------------------*/
SwLinguServiceEventListener::~SwLinguServiceEventListener()
{
}

/* -----------------------------17.03.00 09:06--------------------------------

 ---------------------------------------------------------------------------*/

void SwLinguServiceEventListener::processDictionaryListEvent(
            const DictionaryListEvent& rDicListEvent)
        throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    sal_Int16 nEvt = rDicListEvent.nCondensedEvent;

    sal_Int16 nSpellWrongFlags = 
            DictionaryListEventFlags::ADD_POS_ENTRY 	|
            DictionaryListEventFlags::DEL_NEG_ENTRY		|
            DictionaryListEventFlags::ACTIVATE_POS_DIC	|
            DictionaryListEventFlags::DEACTIVATE_NEG_DIC;
    sal_Bool bIsSpellWrong	=  0 != (nEvt & nSpellWrongFlags);
    sal_Int16 nSpellAllFlags = 
            DictionaryListEventFlags::ADD_NEG_ENTRY		|
            DictionaryListEventFlags::DEL_POS_ENTRY		|
            DictionaryListEventFlags::ACTIVATE_NEG_DIC	|
            DictionaryListEventFlags::DEACTIVATE_POS_DIC;
    sal_Bool bIsSpellAll	=  0 != (nEvt & nSpellAllFlags);
    
    if (bIsSpellWrong || bIsSpellAll)
        SW_MOD()->CheckSpellChanges( sal_False, bIsSpellWrong, bIsSpellAll );
}


void SAL_CALL SwLinguServiceEventListener::processLinguServiceEvent( 
            const LinguServiceEvent& rLngSvcEvent ) 
        throw(RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    
    if (rLngSvcEvent.Source == xLngSvcMgr)
    {
        sal_Bool bIsSpellWrong = 
                0 != (rLngSvcEvent.nEvent & SPELL_WRONG_WORDS_AGAIN);
        sal_Bool bIsSpellAll   = 
                0 != (rLngSvcEvent.nEvent & SPELL_CORRECT_WORDS_AGAIN);
        if (bIsSpellWrong || bIsSpellAll)
        {
            SW_MOD()->CheckSpellChanges( sal_False, bIsSpellWrong, bIsSpellAll );
        }
        if (rLngSvcEvent.nEvent & HYPHENATE_AGAIN)
        {
            SwView *pSwView = SW_MOD()->GetFirstView();
            
            //!! since this function may be called within the ctor of
            //!! SwView (during formatting) where the WrtShell is not yet 
            //!! created, we have to check for the WrtShellPtr to see
            //!! if it is already availbale
            while (pSwView && pSwView->GetWrtShellPtr())
            {
                pSwView->GetWrtShell().ChgHyphenation();
                pSwView = SW_MOD()->GetNextView( pSwView );
            }
        }
    }
}


void SAL_CALL SwLinguServiceEventListener::disposing(
            const EventObject& rEventObj )
        throw(RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (xLngSvcMgr.is()  &&  rEventObj.Source == xLngSvcMgr)
    {
        xLngSvcMgr = 0;
    }
}


void SAL_CALL SwLinguServiceEventListener::queryTermination(
            const EventObject& rEventObj )
        throw(TerminationVetoException, RuntimeException)
{
    //vos::OGuard aGuard(Application::GetSolarMutex());
}


void SAL_CALL SwLinguServiceEventListener::notifyTermination(
            const EventObject& rEventObj )
        throw(RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (xDesktop.is()  &&  rEventObj.Source == xDesktop)
    {
        if (xLngSvcMgr.is())
        {
            xLngSvcMgr->removeLinguServiceManagerListener( 
                    (XLinguServiceEventListener *) this );
            xLngSvcMgr = 0;
        }
        xDesktop = NULL;
    }
}

