/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: itabenum.hxx,v $
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
#ifndef _ITABENUM_HXX
#define _ITABENUM_HXX

#include <tools/solar.h>

namespace tabopts
{
    const USHORT DEFAULT_BORDER     = 0x01;
    const USHORT HEADLINE           = 0x02;
//    const USHORT REPEAT             = 0x04;
//    const USHORT HEADLINE_REPEAT    = 0x06;  // Headline + Repeat
    const USHORT SPLIT_LAYOUT       = 0x08;
    const USHORT HEADLINE_NO_BORDER = HEADLINE | SPLIT_LAYOUT;
    const USHORT ALL_TBL_INS_ATTR   = DEFAULT_BORDER | HEADLINE | SPLIT_LAYOUT;
}

struct SwInsertTableOptions
{
    USHORT mnInsMode;
    USHORT mnRowsToRepeat;

    SwInsertTableOptions( USHORT nInsMode, USHORT nRowsToRepeat ) :
        mnInsMode( nInsMode ), mnRowsToRepeat( nRowsToRepeat ) {};
};


#endif

