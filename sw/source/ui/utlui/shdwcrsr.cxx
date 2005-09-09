/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shdwcrsr.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 11:30:29 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#pragma hdrstop

#ifndef _SV_WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#include "swtypes.hxx"
#include "shdwcrsr.hxx"


SwShadowCursor::~SwShadowCursor()
{
    if( USHRT_MAX != nOldMode )
        DrawCrsr( aOldPt, nOldHeight, nOldMode );
}

void SwShadowCursor::SetPos( const Point& rPt, long nHeight, USHORT nMode )
{
    Point aPt( pWin->LogicToPixel( rPt ));
    nHeight = pWin->LogicToPixel( Size( 0, nHeight )).Height();
    if( aOldPt != aPt || nOldHeight != nHeight || nOldMode != nMode )
    {
        if( USHRT_MAX != nOldMode )
            DrawCrsr( aOldPt, nOldHeight, nOldMode );

        DrawCrsr( aPt, nHeight, nMode );
        nOldMode = nMode;
        nOldHeight = nHeight;
        aOldPt = aPt;
    }
}

void SwShadowCursor::DrawTri( const Point& rPt, long nHeight, BOOL bLeft )
{
    USHORT nLineDiff = ( nHeight / 2 );
    USHORT nLineDiffHalf = nLineDiff / 2;

    // Punkt oben
    Point aPt1( (bLeft ? rPt.X() - 3 : rPt.X() + 3),
                rPt.Y() + nLineDiffHalf );
    // Punkt unten
    Point aPt2( aPt1.X(), aPt1.Y() + nHeight - nLineDiff - 1 );
    short nDiff = bLeft ? -1 : 1;
    while( aPt1.Y() <= aPt2.Y() )
    {
        pWin->DrawLine( aPt1, aPt2 );
        aPt1.Y()++, aPt2.Y()--;
        aPt2.X() = aPt1.X() += nDiff;
    }
}

void SwShadowCursor::DrawCrsr( const Point& rPt, long nHeight, USHORT nMode )
{
    nHeight = (((nHeight / 4)+1) * 4) + 1;

    pWin->Push();

    pWin->SetMapMode( MAP_PIXEL );
    pWin->SetRasterOp( ROP_XOR );

    pWin->SetLineColor( Color( aCol.GetColor() ^ COL_WHITE ) );

    // 1. der Strich:
    pWin->DrawLine( Point( rPt.X(), rPt.Y() + 1),
              Point( rPt.X(), rPt.Y() - 2 + nHeight ));

    // 2. das Dreieck
    if( HORI_LEFT == nMode || HORI_CENTER == nMode )	// Pfeil nach rechts
        DrawTri( rPt, nHeight, FALSE );
    if( HORI_RIGHT == nMode || HORI_CENTER == nMode )	// Pfeil nach links
        DrawTri( rPt, nHeight, TRUE );

    pWin->Pop();
}

void SwShadowCursor::Paint()
{
    if( USHRT_MAX != nOldMode )
        DrawCrsr( aOldPt, nOldHeight, nOldMode );
}

Rectangle SwShadowCursor::GetRect() const
{
    long nH = nOldHeight;
    Point aPt( aOldPt );

    nH = (((nH / 4)+1) * 4) + 1;
    USHORT nWidth = nH / 4 + 3 + 1;

    Size aSz( nWidth, nH );

    if( HORI_RIGHT == nOldMode )
        aPt.X() -= aSz.Width();
    else if( HORI_CENTER == nOldMode )
    {
        aPt.X() -= aSz.Width();
        aSz.Width() *= 2;
    }

    return pWin->PixelToLogic( Rectangle( aPt, aSz ) );
}




