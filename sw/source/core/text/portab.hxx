/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: portab.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _PORTAB_HXX
#define _PORTAB_HXX

#include "porglue.hxx"

/*************************************************************************
 *                      class SwTabPortion
 *************************************************************************/

class SwTabPortion : public SwFixPortion
{
    const KSHORT nTabPos;
    const xub_Unicode cFill;

    // Das Format() verzweigt entweder in Pre- oder PostFormat()
    sal_Bool PreFormat( SwTxtFormatInfo &rInf );
public:
    SwTabPortion( const KSHORT nTabPos, const xub_Unicode cFill = '\0' );
    virtual void Paint( const SwTxtPaintInfo &rInf ) const;
    virtual sal_Bool Format( SwTxtFormatInfo &rInf );
    virtual void FormatEOL( SwTxtFormatInfo &rInf );
    sal_Bool PostFormat( SwTxtFormatInfo &rInf );
    inline  sal_Bool IsFilled() const { return 0 != cFill; }
    inline  KSHORT GetTabPos() const { return nTabPos; }

    // Accessibility: pass information about this portion to the PortionHandler
    virtual void HandlePortion( SwPortionHandler& rPH ) const;

    OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabLeftPortion
 *************************************************************************/

class SwTabLeftPortion : public SwTabPortion
{
public:
    inline SwTabLeftPortion( const KSHORT nTabPosVal, const xub_Unicode cFillChar='\0' )
         : SwTabPortion( nTabPosVal, cFillChar )
    { SetWhichPor( POR_TABLEFT ); }
    OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabRightPortion
 *************************************************************************/

class SwTabRightPortion : public SwTabPortion
{
public:
    inline SwTabRightPortion( const KSHORT nTabPosVal, const xub_Unicode cFillChar='\0' )
         : SwTabPortion( nTabPosVal, cFillChar )
    { SetWhichPor( POR_TABRIGHT ); }
    OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabCenterPortion
 *************************************************************************/

class SwTabCenterPortion : public SwTabPortion
{
public:
    inline SwTabCenterPortion( const KSHORT nTabPosVal, const xub_Unicode cFillChar='\0' )
         : SwTabPortion( nTabPosVal, cFillChar )
    { SetWhichPor( POR_TABCENTER );	}
    OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabDecimalPortion
 *************************************************************************/

class SwTabDecimalPortion : public SwTabPortion
{
    const xub_Unicode mcTab;

    /*
     * During text formatting, we already store the width of the portions
     * following the tab stop up to the decimal position. This value is
     * evaluated during pLastTab->FormatEOL. FME 2006-01-06 #127428#.
     */
    USHORT mnWidthOfPortionsUpTpDecimalPosition;

public:
    inline SwTabDecimalPortion( const KSHORT nTabPosVal, const xub_Unicode cTab,
                                const xub_Unicode cFillChar = '\0' )
         : SwTabPortion( nTabPosVal, cFillChar ),
           mcTab(cTab),
           mnWidthOfPortionsUpTpDecimalPosition( USHRT_MAX )
    { SetWhichPor( POR_TABDECIMAL ); }

    inline xub_Unicode GetTabDecimal() const { return mcTab; }

    inline void SetWidthOfPortionsUpToDecimalPosition( USHORT nNew )
    {
        mnWidthOfPortionsUpTpDecimalPosition = nNew;
    }
    inline USHORT GetWidthOfPortionsUpToDecimalPosition() const
    {
        return mnWidthOfPortionsUpTpDecimalPosition;
    }

    OUTPUT_OPERATOR
};


/*************************************************************************
 *                  class SwAutoTabDecimalPortion
 *************************************************************************/

class SwAutoTabDecimalPortion : public SwTabDecimalPortion
{
public:
    inline SwAutoTabDecimalPortion( const KSHORT nTabPosVal, const xub_Unicode cTab,
                                    const xub_Unicode cFillChar = '\0' )
         : SwTabDecimalPortion( nTabPosVal, cTab, cFillChar )
    { SetLen( 0 ); }
    virtual void Paint( const SwTxtPaintInfo &rInf ) const;
};


CLASSIO( SwTabPortion )
CLASSIO( SwTabLeftPortion )
CLASSIO( SwTabRightPortion )
CLASSIO( SwTabCenterPortion )
CLASSIO( SwTabDecimalPortion )


#endif
