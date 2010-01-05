/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: sortopt.hxx,v $
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
#ifndef _SORTOPT_HXX
#define _SORTOPT_HXX


#include <svl/svarray.hxx>
#include <tools/string.hxx>
#include "swdllapi.h"

enum SwSortOrder  		{ SRT_ASCENDING, SRT_DESCENDING	};
enum SwSortDirection	{ SRT_COLUMNS, SRT_ROWS			};

/*--------------------------------------------------------------------
    Beschreibung: SortierSchluessel
 --------------------------------------------------------------------*/
struct SW_DLLPUBLIC SwSortKey
{
    SwSortKey();
    SwSortKey( USHORT nId, const String& rSrtType, SwSortOrder eOrder );
    SwSortKey( const SwSortKey& rOld );

    String			sSortType;
    SwSortOrder		eSortOrder;
    USHORT 			nColumnId;
    BOOL			bIsNumeric;
};

SV_DECL_PTRARR(SwSortKeys, SwSortKey*, 3, 1)

struct SW_DLLPUBLIC SwSortOptions
{
    SwSortOptions();
    ~SwSortOptions();
    SwSortOptions(const SwSortOptions& rOpt);

    SwSortKeys		aKeys;
    SwSortDirection	eDirection;
    sal_Unicode		cDeli;
    USHORT			nLanguage;
    BOOL			bTable;
    BOOL 			bIgnoreCase;
};

#endif	// _SORTOPT_HXX
