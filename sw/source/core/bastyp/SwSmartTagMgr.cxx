/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SwSmartTagMgr.cxx,v $
 * $Revision: 1.3 $
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
#include "SwSmartTagMgr.hxx"

#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <swmodule.hxx>

#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif

using namespace com::sun::star;
using namespace com::sun::star::uno;

SwSmartTagMgr* SwSmartTagMgr::mpTheSwSmartTagMgr = 0;

SwSmartTagMgr& SwSmartTagMgr::Get()
{
    if ( !mpTheSwSmartTagMgr )
    {
        mpTheSwSmartTagMgr = new SwSmartTagMgr( SwDocShell::Factory().GetModuleName() );
        mpTheSwSmartTagMgr->Init( rtl::OUString::createFromAscii("Writer") );
    }
    return *mpTheSwSmartTagMgr;
}

SwSmartTagMgr::SwSmartTagMgr( const rtl::OUString& rModuleName ) :
    SmartTagMgr( rModuleName )
{
}

SwSmartTagMgr::~SwSmartTagMgr()
{
}

// ::com::sun::star::util::XModifyListener
void SwSmartTagMgr::modified( const lang::EventObject& rEO ) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    
    // Installed recognizers have changed. We remove all existing smart tags:
    SW_MOD()->CheckSpellChanges( sal_False, sal_True, sal_True, sal_True );
    
    SmartTagMgr::modified( rEO );
}

// ::com::sun::star::util::XChangesListener
void SwSmartTagMgr::changesOccurred( const util::ChangesEvent& rEvent ) throw( RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // Configuration has changed. We remove all existing smart tags:
    SW_MOD()->CheckSpellChanges( sal_False, sal_True, sal_True, sal_True );

    SmartTagMgr::changesOccurred( rEvent );
}

/*
SmartTagMgr& SwSmartTagMgr::Get()
{
    if ( !pSmartTagMgr )
        pSmartTagMgr = new SmartTagMgr( SwDocShell::Factory().GetModuleName() );
                                         
     return *pSmartTagMgr;
}
*/

