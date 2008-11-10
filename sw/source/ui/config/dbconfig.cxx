/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dbconfig.cxx,v $
 * $Revision: 1.9 $
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


#include <dbconfig.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <swdbdata.hxx>

#include <unomid.h>

using namespace utl;
using rtl::OUString;
using namespace com::sun::star::uno;

/*--------------------------------------------------------------------
     Beschreibung: Ctor
 --------------------------------------------------------------------*/

const Sequence<OUString>& SwDBConfig::GetPropertyNames()
{
    static Sequence<OUString> aNames;
    if(!aNames.getLength())
    {
        static const char* aPropNames[] =
        {
            "AddressBook/DataSourceName",        //  0
            "AddressBook/Command",              //  1
            "AddressBook/CommandType",          //  2
            "Bibliography/CurrentDataSource/DataSourceName",        //  4
            "Bibliography/CurrentDataSource/Command",              //  5
            "Bibliography/CurrentDataSource/CommandType"          //  6
        };
        const int nCount = sizeof(aPropNames)/sizeof(const char*);
        aNames.realloc(nCount);
        OUString* pNames = aNames.getArray();
        for(int i = 0; i < nCount; i++)
            pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/* -----------------------------06.09.00 16:44--------------------------------

 ---------------------------------------------------------------------------*/
SwDBConfig::SwDBConfig() :
    ConfigItem(C2U("Office.DataAccess"),
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    pAdrImpl(0),
    pBibImpl(0)
{
};
/* -----------------------------06.09.00 16:50--------------------------------

 ---------------------------------------------------------------------------*/
SwDBConfig::~SwDBConfig()
{
    delete pAdrImpl;
    delete pBibImpl;
}
/* -----------------------------20.02.01 12:32--------------------------------

 ---------------------------------------------------------------------------*/
void SwDBConfig::Load()
{
    const Sequence<OUString>& rNames = GetPropertyNames();
    if(!pAdrImpl)
    {

        pAdrImpl = new SwDBData;
        pAdrImpl->nCommandType = 0;
        pBibImpl = new SwDBData;
        pBibImpl->nCommandType = 0;
    }
    Sequence<Any> aValues = GetProperties(rNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == rNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == rNames.getLength())
    {
        for(int nProp = 0; nProp < rNames.getLength(); nProp++)
        {
            switch(nProp)
            {
                case  0: pValues[nProp] >>= pAdrImpl->sDataSource;  break;
                case  1: pValues[nProp] >>= pAdrImpl->sCommand;     break;
                case  2: pValues[nProp] >>= pAdrImpl->nCommandType; break;
                case  3: pValues[nProp] >>= pBibImpl->sDataSource;  break;
                case  4: pValues[nProp] >>= pBibImpl->sCommand;     break;
                case  5: pValues[nProp] >>= pBibImpl->nCommandType; break;
            }
        }
    }
}
/* -----------------------------20.02.01 12:36--------------------------------

 ---------------------------------------------------------------------------*/
const SwDBData& SwDBConfig::GetAddressSource()
{
    if(!pAdrImpl)
        Load();
    return *pAdrImpl;
}
/* -----------------29.11.2002 11:43-----------------
 *
 * --------------------------------------------------*/
const SwDBData& SwDBConfig::GetBibliographySource()
{
    if(!pBibImpl)
        Load();
    return *pBibImpl;
}



