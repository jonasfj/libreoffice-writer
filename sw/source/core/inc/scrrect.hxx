/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scrrect.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:55:34 $
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
#ifndef _SCRRECT_HXX
#define _SCRRECT_HXX


#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif
#include "swrect.hxx"
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif

class SwStripe
{
    SwTwips nY;
    SwTwips nHeight;
public:
    inline SwStripe( SwTwips nPos, SwTwips nHght )
        : nY( nPos ), nHeight( nHght ) {}
    inline SwTwips GetY() const { return nY; }
    inline SwTwips& Y() { return nY; }
    inline SwTwips GetHeight() const { return nHeight; }
    inline SwTwips& Height() { return nHeight; }
    inline SwTwips GetBottom() const { return nHeight ? nY + nHeight - 1 : nY; }
    inline BOOL operator<( const SwStripe &rTst ) const
        { return nY < rTst.nY || ( nY == rTst.nY && nHeight > rTst.nHeight ); }
    inline BOOL operator==( const SwStripe& rTst ) const
        { return nY == rTst.nY && nHeight == rTst.nHeight; }
};

SV_DECL_VARARR( SwStripeArr, SwStripe, 1, 4 );

class SwStripes : public SwStripeArr, public SwStripe
{
    SwTwips nMin;       // Left minimum
    SwTwips nMax;       // Right maximum
public:
#ifdef VERTICAL_LAYOUT
    SwStripes( SwTwips nPos, SwTwips nSize, SwTwips nMn, SwTwips nMx )
        : SwStripe( nPos, nSize ), nMin( nMn ), nMax( nMx ) {}
    SwStripes& Plus( const SwStripes& rOther, BOOL bVert );
    BOOL Recalc( BOOL bVert );
#else
    SwStripes( const SwRect& rRect ) : SwStripe( rRect.Top(), rRect.Height() ),
        nMin(rRect.Left()), nMax(rRect.Left() + rRect.Width()) {}
    SwStripes& operator+=( const SwStripes& rOther );
    BOOL Recalc();
#endif
    inline SwTwips GetMin() const { return nMin; }
    inline SwTwips GetMax() const { return nMax; }
    inline void SetMin( const SwTwips nNew ) { nMin = nNew; }
    inline void SetMax( const SwTwips nNew ) { nMax = nNew; }
    inline void ChkMin( const SwTwips nNew ) { if( nNew < nMin ) nMin = nNew; }
    inline void ChkMax( const SwTwips nNew ) { if( nNew > nMax ) nMax = nNew; }
    inline SwTwips GetRight() const { return nMax > nMin ? nMax - 1 : nMax; }
    inline SwTwips GetWidth() const { return nMax - nMin; }
};

class SwScrollColumn
{
    SwTwips nX;
    SwTwips nWidth;
    SwTwips nOffs;
#ifdef VERTICAL_LAYOUT
    BOOL bVertical;
public:
    inline SwScrollColumn( SwTwips nPos, SwTwips nSz, SwTwips nOff, BOOL bVert )
        : nX( nPos ), nWidth( nSz ), nOffs( nOff ), bVertical( bVert ) {}
    inline SwScrollColumn( const SwScrollColumn& rCol )
        : nX( rCol.nX ), nWidth( rCol.nWidth ),
          nOffs( rCol.nOffs ), bVertical( rCol.bVertical ) {}
    BOOL IsVertical() const { return bVertical; }
    inline BOOL operator<( const SwScrollColumn &rTst ) const
        { return bVertical < rTst.bVertical || ( bVertical == rTst.bVertical &&
          ( nX < rTst.nX || ( nX == rTst.nX && ( nWidth < rTst.nWidth ||
          ( nWidth == rTst.nWidth && nOffs < rTst.nOffs ) ) ) ) ); }
    inline BOOL operator==( const SwScrollColumn &rTst ) const
        { return bVertical == rTst.bVertical && nX == rTst.nX &&
                 nWidth == rTst.nWidth && nOffs == rTst.nOffs;}
#else
public:
    inline SwScrollColumn( const SwRect& rRect, SwTwips nOff )
        : nX( rRect.Left() ), nWidth( rRect.Width() ), nOffs( nOff ) {}
    inline SwScrollColumn( const SwScrollColumn& rCol )
        : nX( rCol.nX ), nWidth( rCol.nWidth ), nOffs( rCol.nOffs ) {}
    inline BOOL operator<( const SwScrollColumn &rTst ) const
        { return nX < rTst.nX || ( nX == rTst.nX && ( nWidth < rTst.nWidth ||
          ( nWidth == rTst.nWidth && nOffs < rTst.nOffs ) ) ); }
    inline BOOL operator==( const SwScrollColumn &rTst ) const
        { return nX == rTst.nX && nWidth == rTst.nWidth && nOffs == rTst.nOffs;}
#endif
    inline SwTwips GetX() const { return nX; }
    inline SwTwips GetWidth() const { return nWidth; }
    inline SwTwips GetOffs() const { return nOffs; }
    inline void ClrOffs() { nOffs = 0; }
    inline SwTwips GetRight() const { return nWidth ? nX + nWidth - 1 : nX; }
};

typedef SwStripes* SwStripesPtr;
SV_DECL_PTRARR_SORT(SwScrollStripes, SwStripesPtr, 1, 4)

class SwScrollArea : public SwScrollColumn, public SwScrollStripes
{
public:
    void SmartInsert( SwStripes* pStripes );
    void Add( SwScrollArea *pScroll );
    inline SwScrollArea( const SwScrollColumn &rCol, SwStripes* pStripes )
        : SwScrollColumn( rCol )
        { Insert( pStripes ); }
    BOOL Compress();
};

typedef SwScrollArea* SwScrollAreaPtr;
SV_DECL_PTRARR_SORT(SScrAreas,SwScrollAreaPtr,1,2);

class SwScrollAreas : public SScrAreas
{
public:
    void InsertCol( const SwScrollColumn &rCol, SwStripes *pStripes );
};


#endif //_SCRRECT_HXX
