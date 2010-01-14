/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swregion.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _SWREGION_HXX
#define _SWREGION_HXX

#include <svl/svarray.hxx>

#include "swrect.hxx"

SV_DECL_VARARR( SwRects, SwRect, 20, 8 )

class SwRegionRects : public SwRects
{
    SwRect aOrigin;	// die Kopie des StartRects

    inline void InsertRect( const SwRect &rRect, const USHORT nPos, BOOL &rDel);

public:
    SwRegionRects( const SwRect& rStartRect, USHORT nInit = 20,
                                             USHORT nGrow = 8 );
    // Zum Ausstanzen aus aOrigin.
    void operator-=( const SwRect& rRect );

    // Aus Loechern werden Flaechen, aus Flaechen werden Loecher.
    void Invert();
    // Benachbarte Rechtecke zusammenfassen.
    void Compress( BOOL bFuzzy = TRUE );

    inline const SwRect &GetOrigin() const { return aOrigin; }
    inline void ChangeOrigin( const SwRect &rRect ) { aOrigin = rRect; }
};

#endif	//_SWREGION_HXX
