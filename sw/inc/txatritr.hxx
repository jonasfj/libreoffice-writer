/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txatritr.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _TXATRITR_HXX
#define _TXATRITR_HXX

#include <tools/solar.h>
#include <sal/types.h>
#include <svtools/svarray.hxx>
#include <svx/langitem.hxx>
#include <hintids.hxx>

class String;
class SwTxtNode;
class SwTxtAttr;
class SfxPoolItem;


class SwScriptIterator
{
    const String& rText;
    xub_StrLen nChgPos;
    sal_uInt16 nCurScript;
    sal_Bool bForward;

public:
    SwScriptIterator( const String& rStr, xub_StrLen nStart = 0,
                      sal_Bool bFrwrd = sal_True );

    sal_Bool Next();

    sal_uInt16 GetCurrScript() const 		{ return nCurScript; }
    xub_StrLen GetScriptChgPos() const		{ return nChgPos; }
    const String& GetText() const			{ return rText;	}
};


class SwTxtAttrIterator
{
    SwScriptIterator aSIter;
    SvPtrarr aStack;
    const SwTxtNode& rTxtNd;
    const SfxPoolItem *pParaItem, *pCurItem;
    xub_StrLen nChgPos;
    sal_uInt16 nAttrPos, nWhichId;
    sal_Bool bIsUseGetWhichOfScript;

    void AddToStack( const SwTxtAttr& rAttr );
    void SearchNextChg();

public:
    SwTxtAttrIterator( const SwTxtNode& rTxtNd, USHORT nWhichId,
                        xub_StrLen nStart = 0, sal_Bool bUseGetWhichOfScript = sal_True );

    sal_Bool Next();

    const SfxPoolItem& GetAttr() const	{ return *pCurItem; }
    xub_StrLen GetChgPos() const		{ return nChgPos; }
};


class SwLanguageIterator : public SwTxtAttrIterator
{
public:
    SwLanguageIterator( const SwTxtNode& rTxtNode, xub_StrLen nStart = 0,
                        USHORT nWhich = RES_CHRATR_LANGUAGE,
                        sal_Bool bUseGetWhichOfScript = sal_True )
        : SwTxtAttrIterator( rTxtNode, nWhich, nStart, bUseGetWhichOfScript )
    {}

    sal_uInt16 GetLanguage() const
        { return ((SvxLanguageItem&)GetAttr()).GetValue(); }
};


#endif
