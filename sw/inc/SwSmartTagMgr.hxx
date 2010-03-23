/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef _SWSMARTTAGMGR_HXX
#define _SWSMARTTAGMGR_HXX

#include <svx/SmartTagMgr.hxx>


/*************************************************************************
 *                      class SwSmartTagMgr
 *
 * Wrapper for the svx SmartTagMgr
 *************************************************************************/

class SwSmartTagMgr : public SmartTagMgr
{
private:
    static SwSmartTagMgr* mpTheSwSmartTagMgr;

    SwSmartTagMgr( const rtl::OUString& rModuleName );
    virtual ~SwSmartTagMgr();
    
public:
    static SwSmartTagMgr& Get();
    
    // ::com::sun::star::util::XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);
    
    // ::com::sun::star::util::XChangesListener
      virtual void SAL_CALL changesOccurred( const ::com::sun::star::util::ChangesEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
};

/*
namespace SwSmartTagMgr
{
    SmartTagMgr& Get();
}
*/

#endif
