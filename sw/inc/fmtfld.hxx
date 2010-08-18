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
#ifndef _FMTFLD_HXX
#define _FMTFLD_HXX

#include <svl/poolitem.hxx>
#include <svl/brdcst.hxx>
#include <svl/smplhint.hxx>

#include "swdllapi.h"
#include <calbck.hxx>

class SwField;
class SwTxtFld;
class SwView;

// ATT_FLD ***********************************
class SW_DLLPUBLIC SwFmtFld : public SfxPoolItem, public SwClient, public SfxBroadcaster
{
    friend class SwTxtFld;
    friend void _InitCore();

    SwField *pField;
    SwTxtFld* pTxtAttr;		// mein TextAttribut

    SwFmtFld();				// das default-Attibut

    // geschuetzter CopyCtor
    // @@@ copy construction allowed, but copy assignment is not? @@@
    SwFmtFld& operator=(const SwFmtFld& rFld);

public:
    TYPEINFO();

    // single argument constructors shall be explicit.
    explicit SwFmtFld( const SwField &rFld );

    // @@@ copy construction allowed, but copy assignment is not? @@@
    SwFmtFld( const SwFmtFld& rAttr );

    virtual ~SwFmtFld();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;

    virtual void Modify( SfxPoolItem* pOld, SfxPoolItem* pNew );
    virtual BOOL GetInfo( SfxPoolItem& rInfo ) const;

    const SwField *GetFld() const	{ return pField; }
    SwField *GetFld()				{ return pField; }

    // #111840#
    /**
       Sets current field.

       @param pField          new field

       @attention The current field will be destroyed before setting the new field.
     */
    void SetFld(SwField * pField);

    const SwTxtFld *GetTxtFld() const	{ return pTxtAttr; }
    SwTxtFld *GetTxtFld()				{ return pTxtAttr; }

    BOOL IsFldInDoc() const;
    BOOL IsProtect() const;
};

class SW_DLLPUBLIC SwFmtFldHint : public SfxHint
{
#define SWFMTFLD_INSERTED	1
#define SWFMTFLD_REMOVED	2
#define SWFMTFLD_FOCUS		3
#define SWFMTFLD_CHANGED	4
#define SWFMTFLD_LANGUAGE	5

    const SwFmtFld* pFld;
    sal_Int16 nWhich;
    const SwView* pView;

public:
    SwFmtFldHint( const SwFmtFld* p, sal_Int16 n, const SwView* pV = 0)
        : pFld(p)
        , nWhich(n)
        , pView(pV)
    {}

    TYPEINFO();
    const SwFmtFld* GetField() const { return pFld; }
    sal_Int16 Which() const { return nWhich; }
    const SwView* GetView() const { return pView; }
};

#endif
