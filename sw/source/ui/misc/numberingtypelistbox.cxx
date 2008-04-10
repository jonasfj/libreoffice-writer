/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: numberingtypelistbox.cxx,v $
 * $Revision: 1.14 $
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
#include <numberingtypelistbox.hxx>
#ifndef _MISC_HRC
#include <misc.hrc>
#endif
#include <cnttab.hxx>
#include <com/sun/star/style/NumberingType.hpp>
#include <com/sun/star/text/XDefaultNumberingProvider.hpp>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/text/XNumberingTypeInfo.hpp>

#include <unomid.h>

using namespace com::sun::star;
using namespace rtl;

/* -----------------------------01.03.01 16:04--------------------------------

 ---------------------------------------------------------------------------*/
struct SwNumberingTypeListBox_Impl
{
    uno::Reference<text::XNumberingTypeInfo> xInfo;
};
/* -----------------------------01.03.01 14:46--------------------------------

 ---------------------------------------------------------------------------*/
SwNumberingTypeListBox::SwNumberingTypeListBox( Window* pWin, const ResId& rResId,
        USHORT nTypeFlags ) :
    ListBox(pWin, rResId),
    pImpl(new SwNumberingTypeListBox_Impl)
{
    uno::Reference< lang::XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
    uno::Reference < uno::XInterface > xI = xMSF->createInstance(
        ::rtl::OUString::createFromAscii( "com.sun.star.text.DefaultNumberingProvider" ) );
    uno::Reference<text::XDefaultNumberingProvider> xDefNum(xI, uno::UNO_QUERY);
    DBG_ASSERT(xDefNum.is(), "service missing: \"com.sun.star.text.DefaultNumberingProvider\"")

    pImpl->xInfo = uno::Reference<text::XNumberingTypeInfo>(xDefNum, uno::UNO_QUERY);
    Reload(nTypeFlags);
}
/* -----------------------------01.03.01 14:46--------------------------------

 ---------------------------------------------------------------------------*/
SwNumberingTypeListBox::~SwNumberingTypeListBox()
{
    delete pImpl;
}
/* -----------------------------01.03.01 16:02--------------------------------

 ---------------------------------------------------------------------------*/
void SwNumberingTypeListBox::Reload(USHORT nTypeFlags)
{
    Clear();
    uno::Sequence<sal_Int16> aTypes;
    const sal_Int16* pTypes = NULL;
    if(0 != (nTypeFlags&INSERT_NUM_EXTENDED_TYPES) )
    {
        if(pImpl->xInfo.is())
        {
            aTypes = pImpl->xInfo->getSupportedNumberingTypes();
            pTypes = aTypes.getConstArray();
        }
    }
    SwOLENames aNames(SW_RES(STRRES_NUMTYPES));
    ResStringArray& rNames = aNames.GetNames();
    for(sal_uInt16 i = 0; i < rNames.Count(); i++)
    {
        long nValue = rNames.GetValue(i);
        sal_Bool bInsert = sal_True;
        USHORT nPos = LISTBOX_APPEND;
        switch(nValue)
        {
            case  style::NumberingType::NUMBER_NONE:
                bInsert = 0 != (nTypeFlags&INSERT_NUM_TYPE_NO_NUMBERING);
                nPos = 0;
             break;
            case  style::NumberingType::CHAR_SPECIAL:	bInsert = 0 != (nTypeFlags&INSERT_NUM_TYPE_BULLET); break;
            case  style::NumberingType::PAGE_DESCRIPTOR:bInsert = 0 != (nTypeFlags&INSERT_NUM_TYPE_PAGE_STYLE_NUMBERING); break;
            case  style::NumberingType::BITMAP:bInsert = 0 != (nTypeFlags&INSERT_NUM_TYPE_BITMAP ); break;
            default:
                if (nValue >  style::NumberingType::CHARS_LOWER_LETTER_N)
                {
                    // Insert only if offered by i18n framework per configuration.
                    bInsert = sal_False;
                    if (pTypes)
                    {
                        for(sal_Int32 nType = 0; nType < aTypes.getLength(); nType++)
                        {
                            if (pTypes[nType] == nValue)
                            {
                                bInsert = sal_True;
                                break;  // for
                            }
                        }
                    }
                }
        }
        if(bInsert)
        {
            USHORT nEntry = InsertEntry(rNames.GetString(i), nPos);
            SetEntryData( nEntry, (void*)nValue );
        }
    }
    if(0 != (nTypeFlags&INSERT_NUM_EXTENDED_TYPES) )
    {
        if(pTypes)
        {
            for(sal_Int32 nType = 0; nType < aTypes.getLength(); nType++)
            {
                sal_Int16 nCurrent = pTypes[nType];
                if(nCurrent > style::NumberingType::CHARS_LOWER_LETTER_N)
                {
                    if(LISTBOX_ENTRY_NOTFOUND == GetEntryPos((void*)(ULONG)nCurrent))
                    {
                        OUString aIdent = pImpl->xInfo->getNumberingIdentifier( nCurrent );
                        USHORT nPos = InsertEntry(aIdent);
                        SetEntryData(nPos,(void*)(ULONG)nCurrent);
                    }
                }
            }
        }
        SelectEntryPos(0);
    }
}
/* -----------------------------01.03.01 14:46--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int16 	SwNumberingTypeListBox::GetSelectedNumberingType()
{
    sal_Int16 nRet = 0;
    USHORT nSelPos = GetSelectEntryPos();
    if(LISTBOX_ENTRY_NOTFOUND != nSelPos)
        nRet = (sal_Int16)(ULONG)GetEntryData(nSelPos);
#ifdef DBG_UTIL
    else
        DBG_ERROR("SwNumberingTypeListBox not selected")
#endif
    return nRet;
}
/* -----------------------------01.03.01 14:46--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool 	SwNumberingTypeListBox::SelectNumberingType(sal_Int16 nType)
{
    USHORT nPos = GetEntryPos((void*)(ULONG)nType);
    SelectEntryPos( nPos );
    return LISTBOX_ENTRY_NOTFOUND != nPos;
}

