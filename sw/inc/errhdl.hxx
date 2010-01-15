/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: errhdl.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _ERRHDL_HXX
#define _ERRHDL_HXX

#ifdef DBG_UTIL

#include <tools/solar.h>
#include <sal/types.h>
#include "swdllapi.h"

extern BOOL bAssert;				// TRUE, wenn eine ASSERT-Box hochkam


// -----------------------------------------------------------------------
// Ausgabe einer Fehlermeldung inkl. Dateiname und Zeilennummer
// wo der Fehler auftrat.
// Die Funktion darf nicht direkt benutzt werden!
// -----------------------------------------------------------------------
SW_DLLPUBLIC void AssertFail( const sal_Char*, const sal_Char*, USHORT );
SW_DLLPUBLIC void AssertFail( USHORT, const sal_Char*, USHORT );

#define ASSERT( cond, message ) \
    if( !(cond) ) { \
        const char	 *_pErrorText = #message; \
        const char	 *_pFileName  = __FILE__; \
       ::AssertFail( _pErrorText, _pFileName, __LINE__ ); \
    }

// -----------------------------------------------------------------------
// Prueft ob die angegebene Bedingung wahr ist, wenn nicht wird eine
// Fehlermeldung die ueber die ID Identifiziert wird, ausgegeben.
// -----------------------------------------------------------------------
#define ASSERT_ID( cond, id ) \
    if( !(cond) ) { \
        const char	 *_pFileName  = __FILE__; \
       ::AssertFail( (USHORT)id, _pFileName, __LINE__ ); \
    }


// -----------------------------------------------------------------------
// Beim Bilden der Produktversion werden alle Debug-Utilities automatisch
// ignoriert
// -----------------------------------------------------------------------
#else
#define ASSERT( cond, message ) 	;
#define ASSERT_ID( cond, id )		;
#endif // PRODUCT



#endif
