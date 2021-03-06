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
#ifndef SW_FMTLINE_HXX
#define SW_FMTLINE_HXX


#include <svl/poolitem.hxx>
#include <hintids.hxx>
#include <format.hxx>
#include "swdllapi.h"

class IntlWrapper;

class SW_DLLPUBLIC SwFmtLineNumber: public SfxPoolItem
{
    ULONG nStartValue	:24; //Startwert fuer den Absatz, 0 == kein Startwert
    ULONG bCountLines	:1;	 //Zeilen des Absatzes sollen mitgezaehlt werden.

public:
    SwFmtLineNumber();
    ~SwFmtLineNumber();

    TYPEINFO();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    ULONG GetStartValue() const { return nStartValue; }
    BOOL  IsCount()		  const { return bCountLines != 0; }

    void SetStartValue( ULONG nNew ) { nStartValue = nNew; }
    void SetCountLines( BOOL b )     { bCountLines = b;	   }
};

inline const SwFmtLineNumber &SwAttrSet::GetLineNumber(BOOL bInP) const
    { return (const SwFmtLineNumber&)Get( RES_LINENUMBER,bInP); }

#endif

