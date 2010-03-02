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
#define _ERRHDL_CXX


#include "stdlib.h"
#ifdef WIN
#endif
#include <tools/debug.hxx>
#include <vcl/svapp.hxx>
#include <vcl/sound.hxx>
#include <errhdl.hxx>
#include <error.h>				// fuer die defines von ERR_SW6MSG_ ...

// break into CodeView
#if defined(ZTC) && defined(WIN)
#define CVBREAK 	asm( 0xCC );
#endif
#if defined(MSC) && defined(WIN)
#define CVBREAK 	__asm int 3;
#endif
#ifndef CVBREAK
#define CVBREAK
#endif

BOOL bAssertFail = FALSE;			// ist gerade eine Assertbox oben ?
BOOL bAssert = FALSE;				// TRUE, wenn mal ein ASSERT kam.

/*------------------------------------------------------------------------
    Ausgabe einer Fehlermeldung inkl. Bedingung, Dateiname und Zeilennummer
    wo der Fehler auftrat.
    Die Funktion wird durch das ASSERT Makro gerufen!
    Parameter:
                char	*pError		Fehlermeldung
                char	*pFileName	Filename in dem der Fehler auftrat
                USHORT	nLine		Zeilennummer in dem der Fehler auftrat
------------------------------------------------------------------------*/

void AssertFail( const sal_Char* pError, const sal_Char* pFileName, USHORT nLine )
{
    CVBREAK;
    // NOTE4("ASSERT: %s at %d: %s\n", pFileName, nLine, pError);
    bAssert = TRUE;

    if( !bAssertFail && GetpApp() && GetpApp()->IsInMain() )
    {
        bAssertFail = TRUE;
        ByteString	aErr;
        aErr = "Assertion failed\n==================\nFILE      :  ";
        aErr += pFileName;
        aErr += " at line ";
        aErr += ByteString::CreateFromInt32( nLine );
        aErr += "\nERROR :  ";
        aErr += pError;

        ByteString aTmp( getenv( "SW_NOBEEP" ) );
        if ( aTmp != "TRUE" )
            Sound::Beep(SOUND_ERROR);

#if defined( UNX ) && !defined( DBG_UTIL )
        DBG_ERROR( aErr.GetBuffer() ); // DbgErr ist in UNIX-nicht Produkt-Versionen nicht definiert
#else
        DbgError( aErr.GetBuffer() );
#endif
        bAssertFail = FALSE;
    }
    else
    {
        Sound::Beep(SOUND_ERROR);
        Sound::Beep(SOUND_ERROR);
        Sound::Beep(SOUND_ERROR);
        if( !bAssertFail )
            *(short *)0 = 4711; 		// UAE ausloesen
    }
}

/*------------------------------------------------------------------------
    Ausgabe einer Fehlermeldung inkl. Bedingung, Dateiname und Zeilennummer
    wo der Fehler auftrat.
    Die Funktion wird durch das ASSERT Makro gerufen!
    Parameter:
                USHORT	nErrorId	Id fuer Fehlermeldung
                char	*pFileName	Filename in dem der Fehler auftrat
                USHORT	nLine		Zeilennummer in dem der Fehler auftrat
------------------------------------------------------------------------*/

void AssertFail( USHORT nErrorId, const sal_Char* pFileName, USHORT nLine )
{
    // Umsetzung der ErrorId in eine Fehlermeldung
    static const sal_Char
        /* Error Fehlermeldungen zugriffe ausserhalb eines Bereiches */
        sERR_VAR_IDX[]		= "Op[]",
        sERR_OUTOFSCOPE[]	= "Zugriff ausserhalb des Bereiches",
        /* Error Codes fuer Numerierungsregeln */
        sERR_NUMLEVEL[] 	= "Falscher Num-Level",
        /* Error Codes fuer TxtNode */
        sERR_NOHINTS[]		= "Zugriff auf ungueltiges HintsArray",
        sERR_UNKNOWN[]		= "???";

    static const sal_Char* aErrStrTab[ ERR_SWGMSG_END - ERR_SWGMSG_START +1 ] =
    {
        sERR_VAR_IDX, sERR_OUTOFSCOPE, sERR_NUMLEVEL, sERR_NOHINTS
    };

    const sal_Char* pMsg;
    if( nErrorId >= ERR_SWGMSG_START && nErrorId < ERR_SWGMSG_END )
        pMsg = aErrStrTab[ nErrorId - ERR_SWGMSG_START ];
    else
        pMsg = sERR_UNKNOWN;

    AssertFail( pMsg, pFileName, nLine );
}


