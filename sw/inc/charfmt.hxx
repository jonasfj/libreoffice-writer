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
#ifndef _CHARFMT_HXX
#define _CHARFMT_HXX

#include <format.hxx>

class SwCharFmt : public SwFmt
{
    friend class SwDoc;
    friend class SwTxtFmtColl;

    SwCharFmt( SwAttrPool& rPool, const sal_Char* pFmtName,
                SwCharFmt *pDerivedFrom )
          : SwFmt( rPool, pFmtName, aCharFmtSetRange, pDerivedFrom, RES_CHRFMT )
    {}
    SwCharFmt( SwAttrPool& rPool, const String &rFmtName,
                SwCharFmt *pDerivedFrom )
          : SwFmt( rPool, rFmtName, aCharFmtSetRange, pDerivedFrom, RES_CHRFMT )
    {}


public:
    TYPEINFO();    //Bereits in Basisklasse Client drin.
};

namespace CharFmt
{
    extern const SfxItemSet* GetItemSet( const SfxPoolItem& rAttr );
    extern const SfxPoolItem* GetItem( const SwTxtAttr& rAttr, USHORT nWhich );
    extern BOOL IsItemIncluded( const USHORT nWhich, const SwTxtAttr *pAttr );
}

#endif



