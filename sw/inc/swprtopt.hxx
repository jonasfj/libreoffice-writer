/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swprtopt.hxx,v $
 * $Revision: 1.6 $
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

#ifndef _SWPRTOPT_HXX
#define _SWPRTOPT_HXX

#include <tools/multisel.hxx>
#include <printdata.hxx>

#include <set>
#include <map>
#include <vector>
#include <utility>

#define POSTITS_NONE	0
#define POSTITS_ONLY	1
#define POSTITS_ENDDOC	2
#define POSTITS_ENDPAGE 3


////////////////////////////////////////////////////////////


class SwPrtOptions : public SwPrintData
{
    USHORT nJobNo;
    String sJobName;

public:
    MultiSelection  aMulti;
    Point  aOffset;
    ULONG  nMergeCnt;           // Anzahl der Serienbriefe
    ULONG  nMergeAct;           // Aktueller Serienbriefnr.
    USHORT nCopyCount;

    BOOL   bCollate,
           bPrintSelection,     // Markierung drucken
           bJobStartet;

    SwPrtOptions( const String& rJobName ) :
        nJobNo( 1 ),
        sJobName( rJobName ),
        aOffset( Point(0,0) ),
        nMergeCnt( 0 ),
        nMergeAct( 0 ),
        bCollate(FALSE),
        bPrintSelection (FALSE),
        bJobStartet(FALSE)
    {}

    const String& MakeNextJobName();		// steht in vprint.cxx
    const String& GetJobName() const 	{ return sJobName; }

#if defined(TCPP)
    // seit neuestem (SV 223) kann der keinen mehr generieren
    inline	SwPrtOptions(const SwPrtOptions& rNew) {*this = rNew;}
#endif


        SwPrtOptions& operator=(const SwPrintData& rData)
            {
                SwPrintData::operator=(rData);
                return *this;
            }
};


////////////////////////////////////////////////////////////

#endif //_SWPRTOPT_HXX

