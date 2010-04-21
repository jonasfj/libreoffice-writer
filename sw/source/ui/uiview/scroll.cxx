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



#include "swtypes.hxx"
#include "swrect.hxx"
#include "scroll.hxx"

#define SCROLL_LINE_SIZE 250


SwScrollbar::SwScrollbar( Window *pWin, BOOL bHoriz ) :
    ScrollBar( pWin,
    WinBits( WB_3DLOOK | WB_HIDE | ( bHoriz ? WB_HSCROLL : WB_VSCROLL)  ) ),
    bHori( bHoriz ),
    bAuto( FALSE ),
    bThumbEnabled( TRUE ),
    bVisible(FALSE),
    bSizeSet(FALSE)
{
    // SSA: --- RTL --- no mirroring for horizontal scrollbars
    if( bHoriz )
        EnableRTL( FALSE );
}


 SwScrollbar::~SwScrollbar() {}

/*------------------------------------------------------------------------
 Beschreibung:	wird nach einer Aenderung der Dokumentgroesse gerufen, um den
                Range des Scrollbars neu einzustellen.
------------------------------------------------------------------------*/

void SwScrollbar::DocSzChgd( const Size &rSize )
{
    aDocSz = rSize;
    SetRange( Range( 0, bHori ? rSize.Width() : rSize.Height()) );
    const ULONG nVisSize = GetVisibleSize();
    SetLineSize( SCROLL_LINE_SIZE );
//    SetLineSize( nVisSize * 10 / 100 );
    SetPageSize( nVisSize * 77 / 100 );
}

/*------------------------------------------------------------------------
 Beschreibung:	wird nach einer Veraenderung des sichtbaren Ausschnittes
                gerufen.
------------------------------------------------------------------------*/


void SwScrollbar::ViewPortChgd( const Rectangle &rRect )
{
    long nThumb, nVisible;
    if( bHori )
    {
        nThumb = rRect.Left();
        nVisible = rRect.GetWidth();
    }
    else
    {
        nThumb = rRect.Top();
        nVisible = rRect.GetHeight();
    }

    SetVisibleSize( nVisible );
    DocSzChgd(aDocSz);
    if ( bThumbEnabled )
        SetThumbPos( nThumb );
    if(bAuto)
        AutoShow();
}

/*-----------------10/21/97 02:48pm-----------------

--------------------------------------------------*/
void SwScrollbar::ExtendedShow( BOOL bSet )
{
    bVisible = bSet;
    if( (!bSet ||  !bAuto) && IsUpdateMode() && bSizeSet)
        ScrollBar::Show(bSet);
}

/*-----------------10/21/97 03:23pm-----------------

--------------------------------------------------*/
void SwScrollbar::SetPosSizePixel( const Point& rNewPos, const Size& rNewSize )
{
    ScrollBar::SetPosSizePixel(rNewPos, rNewSize);
    bSizeSet = TRUE;
    if(bVisible)
        ExtendedShow();

}


/*-----------------14.04.98 11:38-------------------

--------------------------------------------------*/
void SwScrollbar::SetAuto(BOOL bSet)
{
    if(bAuto != bSet)
    {
        bAuto = bSet;

        // automatisch versteckt - dann anzeigen
        if(!bAuto && bVisible && !ScrollBar::IsVisible())
            ExtendedShow(TRUE);
        else if(bAuto)
            AutoShow();	// oder automatisch verstecken
    }
}
/*-----------------14.04.98 11:43-------------------

--------------------------------------------------*/
void SwScrollbar::AutoShow()
{
    long nVis = GetVisibleSize();
    long nLen = GetRange().Len();
    {
        if( nVis >= nLen - 1)
        {
            if(ScrollBar::IsVisible())
                ScrollBar::Show(FALSE);
        }
        else if ( !ScrollBar::IsVisible() &&
                  (!bHori || nVis) )		//Optimierung fuer Browser.
                                            //Horizontaler Scrollbar per
                                            //default aus.
        {
            ScrollBar::Show(TRUE);
        }
    }
}
