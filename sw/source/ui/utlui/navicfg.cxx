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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <swtypes.hxx>	// fuer Pathfinder
#include <navicfg.hxx>
#include <swcont.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <unomid.h>

using namespace ::utl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;

/* -----------------------------08.09.00 16:30--------------------------------

 ---------------------------------------------------------------------------*/
Sequence<OUString> SwNavigationConfig::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "RootType",     		//0
        "SelectedPosition",     //1
        "OutlineLevel",         //2
        "InsertMode",           //3
        "ActiveBlock",          //4
        "ShowListBox",          //5
        "GlobalDocMode"         //6
    };
    const int nCount = 7;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
    {
        pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/*-----------------13.11.96 11.03-------------------

--------------------------------------------------*/

SwNavigationConfig::SwNavigationConfig() :
    utl::ConfigItem(C2U("Office.Writer/Navigator")),
    nRootType(0xffff),
    nSelectedPos(0),
    nOutlineLevel(MAXLEVEL),
    nRegionMode(REGION_MODE_NONE),
    nActiveBlock(0),
    bIsSmall(FALSE),
    bIsGlobalActive(TRUE)
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
//	EnableNotification(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case 0: pValues[nProp] >>= nRootType;	   break;
                    case 1: pValues[nProp] >>= nSelectedPos;   break;
                    case 2: pValues[nProp] >>= nOutlineLevel;  break;
                    case 3: pValues[nProp] >>= nRegionMode;    break;
                    case 4: pValues[nProp] >>= nActiveBlock;	break;
                    case 5: bIsSmall		= *(sal_Bool*)pValues[nProp].getValue();  break;
                    case 6: bIsGlobalActive = *(sal_Bool*)pValues[nProp].getValue();  break;
                }
            }
        }
    }
}
/* -----------------------------08.09.00 16:35--------------------------------

 ---------------------------------------------------------------------------*/
SwNavigationConfig::~SwNavigationConfig()
{}
/* -----------------------------08.09.00 16:35--------------------------------

 ---------------------------------------------------------------------------*/
void SwNavigationConfig::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();
    const Type& rType = ::getBooleanCppuType();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case 0: pValues[nProp] <<= nRootType;	  break;
            case 1: pValues[nProp] <<= nSelectedPos;  break;
            case 2: pValues[nProp] <<= nOutlineLevel; break;
            case 3: pValues[nProp] <<= nRegionMode;   break;
            case 4: pValues[nProp] <<= nActiveBlock;	break;
            case 5: pValues[nProp].setValue(&bIsSmall, rType);			break;
            case 6: pValues[nProp].setValue(&bIsGlobalActive, rType);	break;
        }
    }
    PutProperties(aNames, aValues);
}

void SwNavigationConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& ) {}


