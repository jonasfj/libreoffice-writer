/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: eqnolefilehdr.cxx,v $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_starmath.hxx"
#include "eqnolefilehdr.hxx"
#include <sot/storage.hxx>

//////////////////////////////////////////////////////////////////////

void EQNOLEFILEHDR::Read(SvStorageStream *pS)
{
    *pS >> nCBHdr;
    *pS >> nVersion;
    *pS >> nCf;
    *pS >> nCBObject;
    *pS >> nReserved1;
    *pS >> nReserved2;
    *pS >> nReserved3;
    *pS >> nReserved4;
}


void EQNOLEFILEHDR::Write(SvStorageStream *pS)
{
    *pS << nCBHdr;
    *pS << nVersion;
    *pS << nCf;
    *pS << nCBObject;
    *pS << nReserved1;
    *pS << nReserved2;
    *pS << nReserved3;
    *pS << nReserved4;
}


sal_Bool GetMathTypeVersion( SotStorage* pStor, sal_uInt8 &nVersion )
{
    sal_uInt8 nVer = 0;
    sal_Bool bSuccess = sal_False;

    //
    // code sniplet copied from MathType::Parse
    //
    SvStorageStreamRef xSrc = pStor->OpenSotStream(
        String::CreateFromAscii("Equation Native"),
        STREAM_STD_READ | STREAM_NOCREATE);
    if ( (!xSrc.Is()) || (SVSTREAM_OK != xSrc->GetError()))
        return bSuccess;
    SvStorageStream *pS = &xSrc;
    pS->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    //
    EQNOLEFILEHDR aHdr;
    aHdr.Read(pS);
    *pS >> nVer;
    
    if (!pS->GetError())
    {
        nVersion = nVer;
        bSuccess = sal_True;
    }
    return bSuccess;
}

//////////////////////////////////////////////////////////////////////

