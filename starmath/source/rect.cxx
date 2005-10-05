/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rect.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: kz $ $Date: 2005-10-05 15:03:08 $
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

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#ifndef _SV_VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif


#include "rect.hxx"
#include "types.hxx"
#include "utility.hxx"
#include "smmod.hxx"


////////////////////////////////////////////////////////////////////////////////


// '\0' terminiertes Array mit Zeichen, die im StarMath Font als Buchstaben
// betrachtet werden sollen, (um im Gegensatz zu den anderen Operatoren
// und Symbolen ein "normales"(ungecliptes) SmRect zu erhalten).
static xub_Unicode __READONLY_DATA aMathAlpha[] =
{
    MS_ALEPH,               MS_IM,                  MS_RE,
    MS_WP,                  xub_Unicode(0xE070),    MS_EMPTYSET,
    xub_Unicode(0x2113),    xub_Unicode(0xE0D6),    xub_Unicode(0x2107),
    xub_Unicode(0x2127),    xub_Unicode(0x210A),    MS_HBAR,
    MS_LAMBDABAR,           MS_SETN,                MS_SETZ,
    MS_SETQ,                MS_SETR,                MS_SETC,
    xub_Unicode(0x2373),    xub_Unicode(0xE0A5),    xub_Unicode(0x2112),
    xub_Unicode(0x2130),    xub_Unicode(0x2131),
    xub_Unicode('\0')
};

BOOL SmIsMathAlpha(const XubString &rText)
    // ergibt genau dann TRUE, wenn das Zeichen (aus dem StarMath Font) wie ein
    // Buchstabe behandelt werden soll.
{
    if (rText.Len() == 0)
        return FALSE;

    DBG_ASSERT(rText.Len() == 1, "Sm : String enthaelt nicht genau ein Zeichen");
    xub_Unicode cChar = rText.GetChar(0);

    // ist es ein griechisches Zeichen ?
    if (xub_Unicode(0xE0AC) <= cChar  &&  cChar <= xub_Unicode(0xE0D4))
        return TRUE;
    else
    {
        // kommt es in 'aMathAlpha' vor ?
        const xub_Unicode *pChar = aMathAlpha;
        while (*pChar  &&  *pChar != cChar)
            pChar++;
        return *pChar != xub_Unicode('\0');
    }
}


////////////////////////////////////////
//
// SmRect members
//


SmRect::SmRect()
    // constructs empty rectangle at (0, 0) with width and height 0.
{
    DBG_ASSERT(aTopLeft == Point(0, 0), "Sm: ooops...");
    DBG_ASSERT(aSize == Size(0, 0), "Sm: ooops...");

    bHasBaseline = bHasAlignInfo = FALSE;
    nBaseline = nAlignT = nAlignM = nAlignB =
    nGlyphTop = nGlyphBottom =
    nItalicLeftSpace = nItalicRightSpace =
    nLoAttrFence = nHiAttrFence = 0;
    nBorderWidth = 0;
}


SmRect::SmRect(const SmRect &rRect)
:	aTopLeft(rRect.aTopLeft),
    aSize(rRect.aSize)
{
    bHasBaseline  = rRect.bHasBaseline;
    nBaseline	  = rRect.nBaseline;
    nAlignT		  = rRect.nAlignT;
    nAlignM		  = rRect.nAlignM;
    nAlignB		  = rRect.nAlignB;
    nGlyphTop	  = rRect.nGlyphTop;
    nGlyphBottom  = rRect.nGlyphBottom;
    nHiAttrFence  = rRect.nHiAttrFence;
    nLoAttrFence  = rRect.nLoAttrFence;
    bHasAlignInfo = rRect.bHasAlignInfo;
    nItalicLeftSpace  = rRect.nItalicLeftSpace;
    nItalicRightSpace = rRect.nItalicRightSpace;
    nBorderWidth  = rRect.nBorderWidth;
}


void SmRect::CopyAlignInfo(const SmRect &rRect)
{
    nBaseline	  = rRect.nBaseline;
    bHasBaseline  =	rRect.bHasBaseline;
    nAlignT		  =	rRect.nAlignT;
    nAlignM		  =	rRect.nAlignM;
    nAlignB		  =	rRect.nAlignB;
    bHasAlignInfo = rRect.bHasAlignInfo;
    nLoAttrFence  =	rRect.nLoAttrFence;
    nHiAttrFence  =	rRect.nHiAttrFence;
}


void SmRect::BuildRect(const OutputDevice &rDev, const SmFormat *pFormat,
                       const XubString &rText, USHORT nBorder)
{
#ifndef PRODUCT
    if (rDev.GetOutDevType() != OUTDEV_PRINTER)
        DBG_WARNING("Sm :  Referenz-Device ist kein Drucker");
#endif

    DBG_ASSERT(aTopLeft == Point(0, 0), "Sm: Ooops...");

    aSize = Size(rDev.GetTextWidth(rText), rDev.GetTextHeight());

    const FontMetric  aFM (rDev.GetFontMetric());
    BOOL              bIsMath  = aFM.GetName().EqualsIgnoreCaseAscii( FONTNAME_MATH ) ||
                                 aFM.GetName().EqualsIgnoreCaseAscii( FONTNAME_MATH2 );
    BOOL			  bAllowSmaller = bIsMath && !SmIsMathAlpha(rText);
    const long		  nFontHeight = rDev.GetFont().GetSize().Height();

    nBorderWidth  = nBorder;
    bHasAlignInfo = TRUE;
    bHasBaseline  = TRUE;
    nBaseline	  = aFM.GetAscent();
    nAlignT		  = nBaseline - nFontHeight * 750L / 1000L;
    nAlignM 	  = nBaseline - nFontHeight * 121L / 422L;
        // that's where the horizontal bars of '+', '-', ... are
        // (1/3 of ascent over baseline)
        // (121 = 1/3 of 12pt ascent, 422 = 12pt fontheight)
    nAlignB		  = nBaseline;

    // workaround for printer fonts with very small (possible 0 or even
    // negative(!)) leading
    if (aFM.GetIntLeading() < 5  &&  rDev.GetOutDevType() == OUTDEV_PRINTER)
    {
        OutputDevice	*pWindow = Application::GetDefaultDevice();

        pWindow->Push(PUSH_MAPMODE | PUSH_FONT);

        pWindow->SetMapMode(rDev.GetMapMode());
        pWindow->SetFont(rDev.GetFontMetric());

        long  nDelta = pWindow->GetFontMetric().GetIntLeading();
        if (nDelta == 0)
        { 	// dieser Wert entspricht etwa einem Leading von 80 bei einer
            // Fonthoehe von 422 (12pt)
            nDelta = nFontHeight * 8L / 43;
        }
        SetTop(GetTop() - nDelta);

        pWindow->Pop();
    }

    // get GlyphBoundRect
    Rectangle  aGlyphRect;
    BOOL       bSuccess = SmGetGlyphBoundRect(rDev, rText, aGlyphRect);
    DBG_ASSERT(bSuccess, "Sm : Ooops... (fehlt evtl. der Font?)");

    nItalicLeftSpace  = GetLeft() - aGlyphRect.Left() + nBorderWidth;
    nItalicRightSpace = aGlyphRect.Right() - GetRight() + nBorderWidth;
    if (nItalicLeftSpace  < 0  &&  !bAllowSmaller)
        nItalicLeftSpace  = 0;
    if (nItalicRightSpace < 0  &&  !bAllowSmaller)
        nItalicRightSpace = 0;

    long  nDist = 0;
    if (pFormat)
        nDist = (rDev.GetFont().GetSize().Height()
                * pFormat->GetDistance(DIS_ORNAMENTSIZE)) / 100L;

    nHiAttrFence = aGlyphRect.TopLeft().Y() - 1 - nBorderWidth - nDist;
    nLoAttrFence = SmFromTo(GetAlignB(), GetBottom(), 0.0);

    nGlyphTop    = aGlyphRect.Top() - nBorderWidth;
    nGlyphBottom = aGlyphRect.Bottom() + nBorderWidth;

    if (bAllowSmaller)
    {
        // fuer Symbole und Operatoren aus dem StarMath Font passen wir den
        // oberen und unteren Rand dem Zeichen an.
        SetTop(nGlyphTop);
        SetBottom(nGlyphBottom);
    }

    if (nHiAttrFence < GetTop())
        nHiAttrFence = GetTop();

    if (nLoAttrFence > GetBottom())
        nLoAttrFence = GetBottom();

    DBG_ASSERT(rText.Len() == 0  ||  !IsEmpty(),
               "Sm: leeres Rechteck erzeugt");
}


void SmRect::Init(const OutputDevice &rDev, const SmFormat *pFormat,
                  const XubString &rText, USHORT nBorderWidth)
    // get rectangle fitting for drawing 'rText' on OutputDevice 'rDev'
{
    BuildRect(rDev, pFormat, rText, nBorderWidth);
}


SmRect::SmRect(const OutputDevice &rDev, const SmFormat *pFormat,
               const XubString &rText, long nBorderWidth)
{
    DBG_ASSERT( nBorderWidth >= 0, "BorderWidth negativ" );
    if (nBorderWidth < 0)
        nBorderWidth = 0;
    Init(rDev, pFormat, rText, (USHORT) nBorderWidth);
}


SmRect::SmRect(long nWidth, long nHeight)
    // this constructor should never be used for anything textlike because
    // it will not provide useful values for baseline, AlignT and AlignB!
    // It's purpose is to get a 'SmRect' for the horizontal line in fractions
    // as used in 'SmBinVerNode'.
:	aSize(nWidth, nHeight)
{
    DBG_ASSERT(aTopLeft == Point(0, 0), "Sm: ooops...");

    bHasBaseline  = FALSE;
    bHasAlignInfo = TRUE;
    nBaseline	  = 0;
    nAlignT		  = GetTop();
    nAlignB		  = GetBottom();
    nAlignM		  = (nAlignT + nAlignB) / 2;		// this is the default
    nItalicLeftSpace = nItalicRightSpace = 0;
    nGlyphTop    = nHiAttrFence  = GetTop();
    nGlyphBottom = nLoAttrFence  = GetBottom();
    nBorderWidth  = 0;
}


void SmRect::SetLeft(long nLeft)
{
    if (nLeft <= GetRight())
    {	aSize.Width() = GetRight() - nLeft + 1;
        aTopLeft.X()  = nLeft;
    }
}


void SmRect::SetRight(long nRight)
{
    if (nRight >= GetLeft())
        aSize.Width() = nRight - GetLeft() + 1;
}


void SmRect::SetBottom(long nBottom)
{
    if (nBottom >= GetTop())
        aSize.Height() = nBottom - GetTop() + 1;
}


void SmRect::SetTop(long nTop)
{
    if (nTop <= GetBottom())
    {	aSize.Height()	 = GetBottom() - nTop + 1;
        aTopLeft.Y() = nTop;
    }
}


void SmRect::Move(const Point &rPosition)
    // move rectangle by position 'rPosition'.
{
    aTopLeft  += rPosition;

    long  nDelta = rPosition.Y();
    nBaseline += nDelta;
    nAlignT   += nDelta;
    nAlignM	  += nDelta;
    nAlignB   += nDelta;
    nGlyphTop    += nDelta;
    nGlyphBottom += nDelta;
    nHiAttrFence += nDelta;
    nLoAttrFence += nDelta;
}


const Point SmRect::AlignTo(const SmRect &rRect, RectPos ePos,
                            RectHorAlign eHor, RectVerAlign eVer) const
{	Point  aPos (GetTopLeft());
        // will become the topleft point of the new rectangle position

    // set horizontal or vertical new rectangle position depending on
    // 'ePos' is one of 'RP_LEFT', 'RP_RIGHT' or 'RP_TOP', 'RP_BOTTOM'
    switch (ePos)
    {	case RP_LEFT :
            aPos.X() = rRect.GetItalicLeft() - GetItalicRightSpace()
                       - GetWidth();
            break;
        case RP_RIGHT :
            aPos.X() = rRect.GetItalicRight() + 1 + GetItalicLeftSpace();
            break;
        case RP_TOP :
            aPos.Y() = rRect.GetTop() - GetHeight();
            break;
        case RP_BOTTOM :
            aPos.Y() = rRect.GetBottom() + 1;
            break;
        case RP_ATTRIBUT :
            aPos.X() = rRect.GetItalicCenterX() - GetItalicWidth() / 2
                       + GetItalicLeftSpace();
            break;
        default :
            DBG_ASSERT(FALSE, "Sm: unbekannter Fall");
    }

    // check if horizontal position is already set
    if (ePos == RP_LEFT  ||  ePos == RP_RIGHT  ||  ePos == RP_ATTRIBUT)
        // correct error in current vertical position
        switch (eVer)
        {	case RVA_TOP :
                aPos.Y() += rRect.GetAlignT() - GetAlignT();
                break;
            case RVA_MID :
                aPos.Y() += rRect.GetAlignM() - GetAlignM();
                break;
            case RVA_BASELINE :
                // align baselines if possible else align mid's
                if (HasBaseline() && rRect.HasBaseline())
                    aPos.Y() += rRect.GetBaseline() - GetBaseline();
                else
                    aPos.Y() += rRect.GetAlignM() - GetAlignM();
                break;
            case RVA_BOTTOM :
                aPos.Y() += rRect.GetAlignB() - GetAlignB();
                break;
            case RVA_CENTERY :
                aPos.Y() += rRect.GetCenterY() - GetCenterY();
                break;
            case RVA_ATTRIBUT_HI:
                aPos.Y() += rRect.GetHiAttrFence() - GetBottom();
                break;
            case RVA_ATTRIBUT_MID :
                aPos.Y() += SmFromTo(rRect.GetAlignB(), rRect.GetAlignT(), 0.4)
                            - GetCenterY();
                break;
            case RVA_ATTRIBUT_LO :
                aPos.Y() += rRect.GetLoAttrFence() - GetTop();
                break;
        default :
                DBG_ASSERT(FALSE, "Sm: unbekannter Fall");
        }

    // check if vertical position is already set
    if (ePos == RP_TOP	||	ePos == RP_BOTTOM)
        // correct error in current horizontal position
        switch (eHor)
        {	case RHA_LEFT :
                aPos.X() += rRect.GetItalicLeft() - GetItalicLeft();
                break;
            case RHA_CENTER :
                aPos.X() += rRect.GetItalicCenterX() - GetItalicCenterX();
                break;
            case RHA_RIGHT :
                aPos.X() += rRect.GetItalicRight() - GetItalicRight();
                break;
            default :
                DBG_ASSERT(FALSE, "Sm: unbekannter Fall");
        }

    return aPos;
}


SmRect & SmRect::Union(const SmRect &rRect)
    // rectangle union of current one with 'rRect'. The result is to be the
    // smallest rectangles that covers the space of both rectangles.
    // (empty rectangles cover no space)
    //! Italic correction is NOT taken into account here!
{
    if (rRect.IsEmpty())
        return *this;

    long  nL  = rRect.GetLeft(),
          nR  = rRect.GetRight(),
          nT  = rRect.GetTop(),
          nB  = rRect.GetBottom(),
          nGT = rRect.nGlyphTop,
          nGB = rRect.nGlyphBottom;
    if (!IsEmpty())
    {	long  nTmp;

        if ((nTmp = GetLeft()) < nL)
            nL = nTmp;
        if ((nTmp = GetRight()) > nR)
            nR = nTmp;
        if ((nTmp = GetTop()) < nT)
            nT = nTmp;
        if ((nTmp = GetBottom()) > nB)
            nB = nTmp;
        if ((nTmp = nGlyphTop) < nGT)
            nGT = nTmp;
        if ((nTmp = nGlyphBottom) > nGB)
            nGB = nTmp;
    }

    SetLeft(nL);
    SetRight(nR);
    SetTop(nT);
    SetBottom(nB);
    nGlyphTop    = nGT;
    nGlyphBottom = nGB;

    return *this;
}


SmRect & SmRect::ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode)
    // let current rectangle be the union of itself and 'rRect'
    // (the smallest rectangle surrounding both). Also adapt values for
    // 'AlignT', 'AlignM', 'AlignB', baseline and italic-spaces.
    // The baseline is set according to 'eCopyMode'.
    // If one of the rectangles has no relevant info the other one is copied.
{
    // get some values used for (italic) spaces adaption
    // ! (need to be done before changing current SmRect) !
    long  nL = Min(GetItalicLeft(),  rRect.GetItalicLeft()),
          nR = Max(GetItalicRight(), rRect.GetItalicRight());

    Union(rRect);

    SetItalicSpaces(GetLeft() - nL, nR - GetRight());

    if (!HasAlignInfo())
        CopyAlignInfo(rRect);
    else if (rRect.HasAlignInfo())
    {	nAlignT = Min(GetAlignT(), rRect.GetAlignT());
        nAlignB = Max(GetAlignB(), rRect.GetAlignB());
        nHiAttrFence = Min(GetHiAttrFence(), rRect.GetHiAttrFence());
        nLoAttrFence = Max(GetLoAttrFence(), rRect.GetLoAttrFence());
        DBG_ASSERT(HasAlignInfo(), "Sm: ooops...");

        switch (eCopyMode)
        {	case RCP_THIS:
                // already done
                break;
            case RCP_ARG:
                CopyMBL(rRect);
                break;
            case RCP_NONE:
                ClearBaseline();
                nAlignM = (nAlignT + nAlignB) / 2;
                break;
            case RCP_XOR:
                if (!HasBaseline())
                    CopyMBL(rRect);
                break;
            default :
                DBG_ASSERT(FALSE, "Sm: unbekannter Fall");
        }
    }

    return *this;
}


SmRect & SmRect::ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode,
                          long nNewAlignM)
    // as 'ExtendBy' but sets AlignM value to 'nNewAlignM'.
    // (this version will be used in 'SmBinVerNode' to provide means to
    // align eg "{a over b} over c" correctly where AlignM should not
    // be (AlignT + AlignB) / 2)
{
    DBG_ASSERT(HasAlignInfo(), "Sm: keine Align Info");

    ExtendBy(rRect, eCopyMode);
    nAlignM = nNewAlignM;

    return *this;
}


SmRect & SmRect::ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode,
                          BOOL bKeepVerAlignParams)
    // as 'ExtendBy' but keeps original values for AlignT, -M and -B and
    // baseline.
    // (this is used in 'SmSupSubNode' where the sub-/supscripts shouldn't
    // be allowed to modify these values.)
{
    long  nOldAlignT   = GetAlignT(),
          nOldAlignM   = GetAlignM(),
          nOldAlignB   = GetAlignB(),
          nOldBaseline = nBaseline;		//! depends not on 'HasBaseline'
    BOOL  bOldHasAlignInfo = HasAlignInfo();

    ExtendBy(rRect, eCopyMode);

    if (bKeepVerAlignParams)
    {	nAlignT	  = nOldAlignT;
        nAlignM	  = nOldAlignM;
        nAlignB	  = nOldAlignB;
        nBaseline = nOldBaseline;
        bHasAlignInfo = bOldHasAlignInfo;
    }

    return *this;
}


long SmRect::OrientedDist(const Point &rPoint) const
    // return oriented distance of rPoint to the current rectangle,
    // especially the return value is <= 0 iff the point is inside the
    // rectangle.
    // For simplicity the maximum-norm is used.
{
    BOOL  bIsInside = IsInsideItalicRect(rPoint);

    // build reference point to define the distance
    Point  aRef;
    if (bIsInside)
    {	Point  aIC (GetItalicCenterX(), GetCenterY());

        aRef.X() = rPoint.X() >= aIC.X() ? GetItalicRight() : GetItalicLeft();
        aRef.Y() = rPoint.Y() >= aIC.Y() ? GetBottom() : GetTop();
    }
    else
    {
        // x-coordinate
        if (rPoint.X() > GetItalicRight())
            aRef.X() = GetItalicRight();
        else if (rPoint.X() < GetItalicLeft())
            aRef.X() = GetItalicLeft();
        else
            aRef.X() = rPoint.X();
        // y-coordinate
        if (rPoint.Y() > GetBottom())
            aRef.Y() = GetBottom();
        else if (rPoint.Y() < GetTop())
            aRef.Y() = GetTop();
        else
            aRef.Y() = rPoint.Y();
    }

    // build distance vector
    Point  aDist (aRef - rPoint);

    long nAbsX = labs(aDist.X()),
         nAbsY = labs(aDist.Y());

    return bIsInside ? - Min(nAbsX, nAbsY) : Max (nAbsX, nAbsY);
}


BOOL SmRect::IsInsideRect(const Point &rPoint) const
{
    return	   rPoint.Y() >= GetTop()
           &&  rPoint.Y() <= GetBottom()
           &&  rPoint.X() >= GetLeft()
           &&  rPoint.X() <= GetRight();
}


BOOL SmRect::IsInsideItalicRect(const Point &rPoint) const
{
    return	   rPoint.Y() >= GetTop()
           &&  rPoint.Y() <= GetBottom()
           &&  rPoint.X() >= GetItalicLeft()
           &&  rPoint.X() <= GetItalicRight();
}

SmRect SmRect::AsGlyphRect() const
{
    SmRect aRect (*this);
    aRect.SetTop(nGlyphTop);
    aRect.SetBottom(nGlyphBottom);
    return aRect;
}


// forward declaration
void SmDrawFrame(OutputDevice &rDev, const Rectangle &rRec,
                 const Color aCol = COL_BLACK);

void SmRect::Draw(OutputDevice &rDev, const Point &rPosition, int nFlags) const
{
    if (IsEmpty())
        return;

    rDev.Push(PUSH_LINECOLOR);

    if (nFlags & SM_RECT_LINES)
    {	long   nLeftSpace  = 0,
               nRightSpace = 0;

        if (nFlags & SM_RECT_ITALIC)
        {	nLeftSpace	= GetItalicLeftSpace();
            nRightSpace = GetItalicRightSpace();
        }

        long  nLeft  = GetLeft()  - nLeftSpace,
              nRight = GetRight() + nRightSpace;

        Point aOffset (rPosition - GetTopLeft());

        rDev.SetLineColor(COL_LIGHTBLUE);
        rDev.DrawLine(Point(nLeft,	GetAlignB()) += aOffset,
                      Point(nRight, GetAlignB()) += aOffset);
        rDev.DrawLine(Point(nLeft,	GetAlignT()) += aOffset,
                      Point(nRight, GetAlignT()) += aOffset);
        if (HasBaseline())
            rDev.DrawLine(Point(nLeft,	GetBaseline()) += aOffset,
                          Point(nRight, GetBaseline()) += aOffset);

        rDev.SetLineColor(COL_GRAY);
        rDev.DrawLine(Point(nLeft,	GetHiAttrFence()) += aOffset,
                      Point(nRight, GetHiAttrFence()) += aOffset);
    }

    if (nFlags & SM_RECT_MID)
    {	Point	aCenter = rPosition
                          + (Point(GetItalicCenterX(), GetAlignM()) -= GetTopLeft()),
                aLenX	  (GetWidth() / 5, 0),
                aLenY	  (0, GetHeight() / 16);

        rDev.SetLineColor(COL_LIGHTGREEN);
        rDev.DrawLine(aCenter - aLenX, aCenter + aLenX);
        rDev.DrawLine(aCenter - aLenY, aCenter + aLenY);
    }

    if (nFlags & SM_RECT_ITALIC)
        SmDrawFrame(rDev, Rectangle(rPosition - Point(GetItalicLeftSpace(), 0),
                GetItalicSize()));

    if (nFlags & SM_RECT_CORE)
        SmDrawFrame(rDev, Rectangle(rPosition, GetSize()), COL_LIGHTRED);

    rDev.Pop();
}



////////////////////////////////////////
// misc functions
//


void SmDrawFrame(OutputDevice &rDev, const Rectangle &rRec,
                 const Color aCol)
{
    rDev.Push(PUSH_LINECOLOR);

    rDev.SetLineColor(aCol);

    rDev.DrawLine(rRec.TopLeft(),	  rRec.BottomLeft());
    rDev.DrawLine(rRec.BottomLeft(),  rRec.BottomRight());
    rDev.DrawLine(rRec.BottomRight(), rRec.TopRight());
    rDev.DrawLine(rRec.TopRight(),	  rRec.TopLeft());

    rDev.Pop();
}


BOOL SmGetGlyphBoundRect(const OutputDevice &rDev,
                         const XubString &rText, Rectangle &rRect)
    // basically the same as 'GetTextBoundRect' (in class 'OutputDevice')
    // but with a string as argument.
{
    // handle special case first
    xub_StrLen nLen = rText.Len();
    if (nLen == 0)
    {	rRect.SetEmpty();
        return TRUE;
    }

    // get a device where 'OutputDevice::GetTextBoundRect' will be successful
    OutputDevice *pGlyphDev;
    if (rDev.GetOutDevType() != OUTDEV_PRINTER)
        pGlyphDev = (OutputDevice *) &rDev;
    else
    {
        // since we format for the printer (where GetTextBoundRect will fail)
        // we need a virtual device here.
        pGlyphDev = &SM_MOD1()->GetDefaultVirtualDev();
    }

    const FontMetric  aDevFM (rDev.GetFontMetric());

    pGlyphDev->Push(PUSH_FONT | PUSH_MAPMODE);
    Font aFnt(rDev.GetFont());
    aFnt.SetAlign(ALIGN_TOP);

    // use scale factor when calling GetTextBoundRect to counter
    // negative effects from antialiasing which may otherwise result
    // in significant incorrect bounding rectangles for some charcters.
    Size aFntSize = aFnt.GetSize();

    // HDU: workaround to avoid HUGE font sizes and resulting problems (#112783#)
    long nScaleFactor = 1;
    while( aFntSize.Height() > 2000 * nScaleFactor )
        nScaleFactor *= 2;
    
    aFnt.SetSize( Size( aFntSize.Width() / nScaleFactor, aFntSize.Height() / nScaleFactor ) );
    pGlyphDev->SetFont(aFnt);

    long nTextWidth = rDev.GetTextWidth(rText);
    Point aPoint;
    Rectangle   aResult (aPoint, Size(nTextWidth, rDev.GetTextHeight())),
                aTmp;

    BOOL bSuccess = pGlyphDev->GetTextBoundRect(aTmp, rText, 0, 0);
    DBG_ASSERT( bSuccess, "GetTextBoundRect failed" );


    if (!aTmp.IsEmpty())
    {
        aResult = Rectangle(aTmp.Left() * nScaleFactor, aTmp.Top() * nScaleFactor,
                            aTmp.Right() * nScaleFactor, aTmp.Bottom() * nScaleFactor);
        if (&rDev != pGlyphDev) /* only when rDev is a printer... */
        {
            long nGDTextWidth  = pGlyphDev->GetTextWidth(rText);
            if (nGDTextWidth != 0  &&
                nTextWidth != nGDTextWidth)
            {
                aResult.Right() *= nTextWidth;
                aResult.Right() /= nGDTextWidth * nScaleFactor;
            }
        }
    }

    // move rectangle to match possibly different baselines
    // (because of different devices)
    long nDelta = aDevFM.GetAscent() - pGlyphDev->GetFontMetric().GetAscent() * nScaleFactor;
    aResult.Move(0, nDelta);

    pGlyphDev->Pop();

    rRect = aResult;
    return bSuccess;
}


