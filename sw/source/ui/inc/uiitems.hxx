/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: uiitems.hxx,v $
 * $Revision: 1.7 $
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
#ifndef _UIITEMS_HXX
#define _UIITEMS_HXX

#include <svtools/intitem.hxx>
#include "swdllapi.h"
#include "cmdid.h"
#include "pagedesc.hxx"

class SwNumRule;
class IntlWrapper;

/*--------------------------------------------------------------------
    Beschreibung: Container fuer FootNote
 --------------------------------------------------------------------*/
class SW_DLLPUBLIC SwPageFtnInfoItem : public SfxPoolItem
{
    SwPageFtnInfo aFtnInfo;

public:

    SwPageFtnInfoItem(const USHORT nId, SwPageFtnInfo& rInfo);
    SwPageFtnInfoItem(const SwPageFtnInfoItem& rItem );
    ~SwPageFtnInfoItem();

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;

    virtual BOOL             QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    SwPageFtnInfo& GetPageFtnInfo()             { return aFtnInfo; }
    const SwPageFtnInfo& GetPageFtnInfo() const { return aFtnInfo; }
    void SetPageFtnInfo(SwPageFtnInfo& rInf) 	{ aFtnInfo = rInf; }
};

/*******************************************************************/

class SW_DLLPUBLIC SwPtrItem : public SfxPoolItem
{
    void* pMisc;

public:
    SwPtrItem( const USHORT nId = FN_PARAM_GRF_DIALOG, void* pPtr = 0);
    SwPtrItem( const SwPtrItem& rItem );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual int             operator==( const SfxPoolItem& ) const;

    void	SetValue(void * pPtr) 	{ pMisc= pPtr; }
    void*	GetValue() const 		{ return pMisc; }
};

/*******************************************************************/

class SW_DLLPUBLIC SwUINumRuleItem : public SfxPoolItem
{
    SwNumRule* pRule;

public:
    SwUINumRuleItem( const SwNumRule& rRule, const USHORT = FN_PARAM_ACT_NUMBER);
    SwUINumRuleItem( const SwUINumRuleItem& rItem );
    virtual ~SwUINumRuleItem();

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual int             operator==( const SfxPoolItem& ) const;

    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    const SwNumRule* GetNumRule() const 		{ return pRule; }
          SwNumRule* GetNumRule() 				{ return pRule; }
};

/* -----------------17.06.98 17:40-------------------
 *
 * --------------------------------------------------*/
class SwBackgroundDestinationItem : public SfxUInt16Item
{
public:
    SwBackgroundDestinationItem(USHORT  nWhich, USHORT nValue);

    virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
};

#endif // _UIITEMS_HXX
