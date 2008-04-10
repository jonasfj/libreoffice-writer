/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: docstat.cxx,v $
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
#include "precompiled_sw.hxx"


#include <docstat.hxx>


/************************************************************************
 *						   SwDocStat::SwDocStat()
 ************************************************************************/

SwDocStat::SwDocStat() :
    nTbl(0),
    nGrf(0),
    nOLE(0),
    nPage(1),
    nPara(1),
    nWord(0),
    nChar(0),
    bModified(TRUE),
    pInternStat(0)
{}

/************************************************************************
 *						   void SwDocStat::Reset()
 ************************************************************************/

void SwDocStat::Reset()
{
    nTbl 	= 0;
    nGrf 	= 0;
    nOLE 	= 0;
    nPage   = 1;
    nPara   = 1;
    nWord 	= 0;
    nChar	= 0;
    bModified = TRUE;
    pInternStat = 0;
}

