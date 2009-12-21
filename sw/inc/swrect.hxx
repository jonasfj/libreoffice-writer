/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swrect.hxx,v $
 * $Revision: 1.14 $
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
#ifndef _SWRECT_HXX
#define _SWRECT_HXX
#include "errhdl.hxx"
#include <tools/gen.hxx>
class SvStream;

class SwRect
{
    Point m_Point;
    Size m_Size;


public:
    inline SwRect();
    inline SwRect( const SwRect &rRect );
    inline SwRect( const Point& rLT, const Size&  rSize );
    inline SwRect( const Point& rLT, const Point& rRB );
    inline SwRect( long X, long Y, long Width, long Height );

        //SV-SS z.B. SwRect( pWin->GetClipRect() );
    SwRect( const Rectangle &rRect );

    //Set-Methoden
    inline void Chg( const Point& rNP, const Size &rNS );
    inline void Pos(  const Point& rNew );
    inline void Pos( const long nNewX, const long nNewY );
    inline void SSize( const Size&  rNew  );
    inline void SSize( const long nHeight, const long nWidth );
    inline void Width(  long nNew );
    inline void Height( long nNew );
    inline void Left( const long nLeft );
    inline void Right( const long nRight );
    inline void Top( const long nTop );
    inline void Bottom( const long nBottom );

    //Get-Methoden
    inline const Point &Pos()  const;
    inline const Size  &SSize() const;
    inline long Width()  const;
    inline long Height() const;
    inline long Left()   const;
    inline long Right()  const;
    inline long Top()	 const;
    inline long Bottom() const;

    //Damit Layoutseitig per Memberpointer auf die Member von Pos und SSize
    //zugegriffen werden kann.
    inline Point &Pos();
    inline Size  &SSize();

    Point Center() const;

    void Justify();

           SwRect &Union( const SwRect& rRect );
           SwRect &Intersection( const SwRect& rRect );
   //Wie Intersection nur wird davon ausgegangen, dass die Rects ueberlappen!
           SwRect &_Intersection( const SwRect &rRect );
    inline SwRect  GetIntersection( const SwRect& rRect ) const;

           BOOL IsInside( const Point& rPOINT ) const;
           BOOL IsNear(const Point& rPoint, long nTolerance ) const;
           BOOL IsInside( const SwRect& rRect ) const;
           BOOL IsOver( const SwRect& rRect ) const;
    inline BOOL HasArea() const;
    inline BOOL IsEmpty() const;
    inline void Clear();

    inline SwRect &operator = ( const SwRect &rRect );

    inline BOOL operator == ( const SwRect& rRect ) const;
    inline BOOL operator != ( const SwRect& rRect ) const;

    inline SwRect &operator+=( const Point &rPt );
    inline SwRect &operator-=( const Point &rPt );

    inline SwRect &operator+=( const Size &rSz );
    inline SwRect &operator-=( const Size &rSz );

    //SV-SS z.B. pWin->DrawRect( aSwRect.SVRect() );
    inline Rectangle  SVRect() const;

    //Zortech wuerde hier fehlerhaften Code erzeugen.
//	inline operator SRectangle()  const;
//	inline operator Rectangle() const { return Rectangle( aPos, aSize ); }

    // Ausgabeoperator fuer die Debugging-Gemeinde
    friend SvStream &operator<<( SvStream &rStream, const SwRect &rRect );


    void _Top(      const long nTop );
    void _Bottom(   const long nBottom );
    void _Left(     const long nLeft );
    void _Right(    const long nRight );
    void _Width(    const long nNew );
    void _Height(   const long nNew );
    long _Top()     const;
    long _Bottom()  const;
    long _Left()    const;
    long _Right()   const;
    long _Width()   const;
    long _Height()  const;
    void SubTop(    const long nSub );
    void AddBottom( const long nAdd );
    void SubLeft(   const long nSub );
    void AddRight(  const long nAdd );
    void AddWidth(  const long nAdd );
    void AddHeight( const long nAdd );
    void SetPosX(   const long nNew );
    void SetPosY(   const long nNew );
    void SetLeftAndWidth( long nLeft, long nNew );
    void SetTopAndHeight( long nTop, long nNew );
    void SetRightAndWidth( long nRight, long nNew );
    void SetBottomAndHeight( long nBottom, long nNew );
    void SetUpperLeftCorner(  const Point& rNew );
    void SetUpperRightCorner(  const Point& rNew );
    void SetLowerLeftCorner(  const Point& rNew );
    const Size  _Size() const;
    const Point TopLeft()  const;
    const Point TopRight()  const;
    const Point BottomLeft()  const;
    const Point BottomRight()  const;
    const Size  SwappedSize() const;
    long GetLeftDistance( long ) const;
    long GetBottomDistance( long ) const;
    long GetRightDistance( long ) const;
    long GetTopDistance( long ) const;
    BOOL OverStepLeft( long ) const;
    BOOL OverStepBottom( long ) const;
    BOOL OverStepTop( long ) const;
    BOOL OverStepRight( long ) const;
};

// Implementation in in swrect.cxx
extern SvStream &operator<<( SvStream &rStream, const SwRect &rRect );

typedef void (SwRect:: *SwRectSet)( const long nNew );
typedef long (SwRect:: *SwRectGet)() const;
typedef const Point (SwRect:: *SwRectPoint)() const;
typedef const Size (SwRect:: *SwRectSize)() const;
typedef BOOL (SwRect:: *SwRectMax)( long ) const;
typedef long (SwRect:: *SwRectDist)( long ) const;
typedef void (SwRect:: *SwRectSetTwice)( long, long );
typedef void (SwRect:: *SwRectSetPos)( const Point& );

//---------------------------------- Set-Methoden
inline void SwRect::Chg( const Point& rNP, const Size &rNS )
{
    m_Point = rNP;
    m_Size = rNS;
}
inline void SwRect::Pos(  const Point& rNew )
{
    m_Point = rNew;
}
inline void SwRect::Pos( const long nNewX, const long nNewY )
{
    m_Point.setX(nNewX);
    m_Point.setY(nNewY);
}
inline void SwRect::SSize( const Size&  rNew  )
{
    m_Size = rNew;
}
inline void SwRect::SSize( const long nNewHeight, const long nNewWidth )
{
    m_Size.setWidth(nNewWidth);
    m_Size.setHeight(nNewHeight);
}
inline void SwRect::Width(  long nNew )
{
    m_Size.setWidth(nNew);
}
inline void SwRect::Height( long nNew )
{
    m_Size.setHeight(nNew);
}
inline void SwRect::Left( const long nLeft )
{
    m_Size.Width() += m_Point.getX() - nLeft;
    m_Point.setX(nLeft);
}
inline void SwRect::Right( const long nRight )
{
    m_Size.setWidth(nRight - m_Point.getX() + 1);
}
inline void SwRect::Top( const long nTop )
{
    m_Size.Height() += m_Point.getY() - nTop;
    m_Point.setY(nTop);
}
inline void SwRect::Bottom( const long nBottom )
{
    m_Size.setHeight(nBottom - m_Point.getY() + 1);
}

//----------------------------------- Get-Methoden
inline const Point &SwRect::Pos()  const
{
    return m_Point;
}
inline Point &SwRect::Pos()
{
    return m_Point;
}
inline const Size  &SwRect::SSize() const
{
    return m_Size;
}
inline Size  &SwRect::SSize()
{
    return m_Size;
}
inline long SwRect::Width()  const
{
    return m_Size.Width();
}
inline long SwRect::Height() const
{
    return m_Size.Height();
}
inline long SwRect::Left()   const
{
    return m_Point.X();
}
inline long SwRect::Right()  const
{
    return m_Size.getWidth() ? m_Point.getX() + m_Size.getWidth() - 1 : m_Point.getX();
}
inline long SwRect::Top()	 const
{
    return m_Point.Y();
}
inline long SwRect::Bottom() const
{
    return m_Size.getHeight() ? m_Point.getY() + m_Size.getHeight() - 1 : m_Point.getY();
}

//----------------------------------- operatoren
inline SwRect &SwRect::operator = ( const SwRect &rRect )
{
    m_Point = rRect.m_Point;
    m_Size = rRect.m_Size;
    return *this;
}
inline BOOL SwRect::operator == ( const SwRect& rRect ) const
{
    return (m_Point == rRect.m_Point && m_Size == rRect.m_Size);
}
inline BOOL SwRect::operator != ( const SwRect& rRect ) const
{
    return (m_Point != rRect.m_Point || m_Size != rRect.m_Size);
}

inline SwRect &SwRect::operator+=( const Point &rPt )
{
    m_Point += rPt;
    return *this;
}
inline SwRect &SwRect::operator-=( const Point &rPt )
{
    m_Point -= rPt;
    return *this;
}

inline SwRect &SwRect::operator+=( const Size &rSz )
{
    m_Size.Width() += rSz.Width();
    m_Size.Height() += rSz.Height();
    return *this;
}
inline SwRect &SwRect::operator-=( const Size &rSz )
{
    m_Size.Width() -= rSz.Width();
    m_Size.Height() -= rSz.Height();
    return *this;
}


//--------------------------- Sonstiges
inline Rectangle SwRect::SVRect() const
{
    ASSERT( !IsEmpty(), "SVRect() without Width or Height" );
    return Rectangle( m_Point.getX(), m_Point.getY(),
        m_Point.getX() + m_Size.getWidth() - 1,         //Right()
        m_Point.getY() + m_Size.getHeight() - 1 );      //Bottom()
}

inline SwRect SwRect::GetIntersection( const SwRect& rRect ) const
{
    return SwRect( *this ).Intersection( rRect );
}

inline BOOL SwRect::HasArea() const
{
    return !IsEmpty();
}
inline BOOL SwRect::IsEmpty() const
{
    return !(m_Size.getHeight() && m_Size.getWidth());
}
inline void SwRect::Clear()
{
    m_Point.setX(0);
    m_Point.setY(0);
    m_Size.setWidth(0);
    m_Size.setHeight(0);
}

//-------------------------- CToren
inline SwRect::SwRect() :
    m_Point( 0, 0 ),
    m_Size( 0, 0 )
{
}
inline SwRect::SwRect( const SwRect &rRect ) :
    m_Point( rRect.m_Point ),
    m_Size( rRect.m_Size )
{
}
inline SwRect::SwRect( const Point& rLT, const Size&  rSize ) :
    m_Point( rLT ),
    m_Size( rSize )
{
}
inline SwRect::SwRect( const Point& rLT, const Point& rRB ) :
    m_Point( rLT ),
    m_Size( rRB.X() - rLT.X() + 1, rRB.Y() - rLT.Y() + 1 )
{
}
inline SwRect::SwRect( long X, long Y, long W, long H ) :
    m_Point( X, Y ),
    m_Size( W, H )
{
}


#endif	//_SWRECT_HXX
