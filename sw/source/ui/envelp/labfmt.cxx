/*************************************************************************
 *
 *  $RCSfile: labfmt.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: fme $ $Date: 2002-12-10 09:43:47 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SV_POLY_HXX //autogen
#include <vcl/poly.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif

#include "swtypes.hxx"
#include "cmdid.h"
#include "label.hxx"
#include "labimp.hxx"
#include "labimg.hxx"
#include "labfmt.hxx"
#include "uitool.hxx"

#include "label.hrc"
#include "labfmt.hrc"

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;

// #define ------------------------------------------------------------------

#define ROUND(x) ((USHORT) ((x) + .5))
#define C2S(cChar) String::CreateFromAscii(cChar)
#define C2U(cChar) OUString::createFromAscii(cChar)
// --------------------------------------------------------------------------
SwLabPreview::SwLabPreview( const SwLabFmtPage* pParent, const ResId& rResID ) :

    Window((Window*) pParent, rResID),

    aHDistStr (SW_RES(STR_HDIST )),
    aVDistStr (SW_RES(STR_VDIST )),
    aWidthStr (SW_RES(STR_WIDTH )),
    aHeightStr(SW_RES(STR_HEIGHT)),
    aLeftStr  (SW_RES(STR_LEFT  )),
    aUpperStr (SW_RES(STR_UPPER )),
    aColsStr  (SW_RES(STR_COLS  )),
    aRowsStr  (SW_RES(STR_ROWS  )),
    aGrayColor(COL_LIGHTGRAY)
{
    SetMapMode(MAP_PIXEL);

    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    const Color& rWinColor = rStyleSettings.GetWindowColor();
    SetBackground(Wallpaper(rWinColor));

    Font aFont = GetFont();
    aFont.SetTransparent(TRUE);
    aFont.SetWeight	 (WEIGHT_NORMAL);
    SetFont(aFont);

    const Size aSz(GetOutputSizePixel());

    lOutWPix   = aSz.Width ();
    lOutHPix   = aSz.Height();


    lHDistWidth  = GetTextWidth(aHDistStr );
    lVDistWidth  = GetTextWidth(aVDistStr );
    lHeightWidth = GetTextWidth(aHeightStr);
    lLeftWidth	 = GetTextWidth(aLeftStr  );
    lUpperWidth  = GetTextWidth(aUpperStr );
    lColsWidth	 = GetTextWidth(aColsStr  );

    lXHeight = GetTextHeight();
    lXWidth  = GetTextWidth('X');

    // Skalierungsfaktor
    float fx = (float)(lOutWPix - (2 * (lLeftWidth + 15))) / (float)lOutWPix;

    lOutWPix23 = (long)((float)lOutWPix * fx);
    lOutHPix23 = (long)((float)lOutHPix * fx);
}

// --------------------------------------------------------------------------
SwLabPreview::~SwLabPreview()
{
}
// --------------------------------------------------------------------------
void SwLabPreview::Paint(const Rectangle &rRect)
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    const Color& rWinColor = rStyleSettings.GetWindowColor();
    const Color& rFieldTextColor = SwViewOption::GetFontColor();

    Font aFont = GetFont();
    aFont.SetFillColor( rWinColor );
    aFont.SetColor(rFieldTextColor);
    SetFont(aFont);

    SetBackground(Wallpaper(rWinColor));

    SetLineColor(rWinColor);
    SetFillColor(aGrayColor);
    Font aPaintFont(GetFont());
    aPaintFont.SetTransparent(FALSE);
    SetFont(aPaintFont);

    // Groesse des darzustellenden Bereichs
    long lDispW = ROUND(aItem.lLeft  + aItem.lHDist);
    long lDispH = ROUND(aItem.lUpper + aItem.lVDist);
    if (aItem.nCols == 1)
        lDispW += ROUND(aItem.lLeft );
    else
        lDispW += ROUND(aItem.lHDist / 10);
    if (aItem.nRows == 1)
        lDispH += ROUND(aItem.lUpper);
    else
        lDispH += ROUND(aItem.lVDist / 10);

    // Skalierungsfaktor
    float fx = (float) lOutWPix23 / Max(1L, lDispW),
          fy = (float) lOutHPix23 / Max(1L, lDispH),
          f  = fx < fy ? fx : fy;

    // Nullpunkt
    long lOutlineW = ROUND(f * lDispW);
    long lOutlineH = ROUND(f * lDispH);

    long lX0 = (lOutWPix - lOutlineW) / 2;
    long lY0 = (lOutHPix - lOutlineH) / 2;
    long lX1 = ROUND(lX0 + f *	aItem.lLeft );
    long lY1 = ROUND(lY0 + f *	aItem.lUpper);
    long lX2 = ROUND(lX0 + f * (aItem.lLeft  + aItem.lWidth ));
    long lY2 = ROUND(lY0 + f * (aItem.lUpper + aItem.lHeight));
    long lX3 = ROUND(lX0 + f * (aItem.lLeft  + aItem.lHDist ));
    long lY3 = ROUND(lY0 + f * (aItem.lUpper + aItem.lVDist ));

    // Umriss zeichnen (Flaeche)
    DrawRect(Rectangle(Point(lX0, lY0), Size(lOutlineW, lOutlineH)));

    // Umriss zeichnen (Umrandung)
    SetLineColor(rFieldTextColor);
    DrawLine(Point(lX0, lY0), Point(lX0 + lOutlineW - 1, lY0)); // Oben
    DrawLine(Point(lX0, lY0), Point(lX0, lY0 + lOutlineH - 1)); // Links
    if (aItem.nCols == 1)
        DrawLine(Point(lX0 + lOutlineW - 1, lY0), Point(lX0 + lOutlineW - 1, lY0 + lOutlineH - 1)); // Rechts
    if (aItem.nRows == 1)
        DrawLine(Point(lX0, lY0 + lOutlineH - 1), Point(lX0 + lOutlineW - 1, lY0 + lOutlineH - 1)); // Unten

    // Etiketten
    SetClipRegion (Rectangle(Point(lX0, lY0), Size(lOutlineW, lOutlineH)));
    SetFillColor(rWinColor);
    for (USHORT nRow = 0; nRow < Min((USHORT) 2, (USHORT) aItem.nRows); nRow++)
        for (USHORT nCol = 0; nCol < Min((USHORT) 2, (USHORT) aItem.nCols); nCol++)
            DrawRect(Rectangle(
              Point(ROUND(lX0 + f * (aItem.lLeft  + nCol * aItem.lHDist)),
                    ROUND(lY0 + f * (aItem.lUpper + nRow * aItem.lVDist))),
              Size (ROUND(f * aItem.lWidth ),
                    ROUND(f * aItem.lHeight))));
    SetClipRegion();

    // Beschritung: Rand links
    if (aItem.lLeft)
    {
        long lX = (lX0 + lX1) / 2;
        DrawArrow(Point(lX0, lY0 - 5), Point(lX1, lY0 - 5), FALSE);
        DrawArrow(Point(lX, lY0 - 10), Point(lX, lY0 - 5), TRUE);
        DrawText(Point(lX1 - lLeftWidth, lY0 - 10 - lXHeight), aLeftStr);
    }

    // Beschriftung: Rand oben
    if (aItem.lUpper)
    {
        DrawArrow(Point(lX0 - 5, lY0), Point(lX0 - 5, lY1), FALSE);
        DrawText(Point(lX0 - 10 - lUpperWidth, ROUND(lY0 + f * aItem.lUpper / 2 - lXHeight / 2)), aUpperStr);
    }

    // Beschriftung: Breite und Hoehe
    {
        long lX = lX2 - lXWidth / 2 - lHeightWidth / 2;
        long lY = lY1 + lXHeight;

        DrawLine(Point(lX1, lY), Point(lX2 - 1, lY));
        DrawLine(Point(lX, lY1), Point(lX, lY2 - 1));

        DrawText(Point(lX1 + lXWidth / 2, lY - lXHeight / 2), aWidthStr);
        DrawText(Point(lX - lHeightWidth / 2, lY2 - lXHeight - lXHeight / 2), aHeightStr);
    }

    // Beschriftung: Horz. Abstand
    if (aItem.nCols > 1)
    {
        long lX = (lX1 + lX3) / 2;
        DrawArrow(Point(lX1, lY0 - 5), Point(lX3, lY0 - 5), FALSE);
        DrawArrow(Point(lX, lY0 - 10), Point(lX, lY0 - 5), TRUE);
        DrawText(Point(lX - lHDistWidth / 2, lY0 - 10 - lXHeight), aHDistStr);
    }

    // Beschriftung: Vertikaler Abstand
    if (aItem.nRows > 1)
    {
        DrawArrow(Point(lX0 - 5, lY1), Point(lX0 - 5, lY3), FALSE);
        DrawText(Point(lX0 - 10 - lVDistWidth, ROUND(lY1 + f * aItem.lVDist / 2 - lXHeight / 2)), aVDistStr);
    }

    // Beschriftung: Spalten
    {
        long lY = lY0 + lOutlineH + 4;
        DrawArrow(Point(lX0, lY), Point(lX0 + lOutlineW - 1, lY), TRUE);
        DrawText(Point((lX0 + lX0 + lOutlineW - 1) / 2 - lColsWidth / 2, lY + 5), aColsStr);
    }

    // Beschriftung: Zeilen
    {
        long lX = lX0 + lOutlineW + 4;
        DrawArrow(Point(lX, lY0), Point(lX, lY0 + lOutlineH - 1), TRUE);
        DrawText(Point(lX + 5, (lY0 + lY0 + lOutlineH - 1 - lXHeight / 2) / 2), aRowsStr);
    }
}

// Pfeil bzw. Intervall zeichnen --------------------------------------------



void SwLabPreview::DrawArrow(const Point &rP1, const Point &rP2, BOOL bArrow)
{
    DrawLine(rP1, rP2);

    if (bArrow)
    {
        Point aArr[3];

        // Pfeil zeichnen
        if (rP1.Y() == rP2.Y())
        {
            // Waagerecht
            aArr[0].X() = rP2.X() - 5;
            aArr[0].Y() = rP2.Y() - 2;
            aArr[1].X() = rP2.X();
            aArr[1].Y() = rP2.Y();
            aArr[2].X() = rP2.X() - 5;
            aArr[2].Y() = rP2.Y() + 2;
        }
        else
        {
            // Senkrecht
            aArr[0].X() = rP2.X() - 2;
            aArr[0].Y() = rP2.Y() - 5;
            aArr[1].X() = rP2.X() + 2;
            aArr[1].Y() = rP2.Y() - 5;
            aArr[2].X() = rP2.X();
            aArr[2].Y() = rP2.Y();
        }

        const Color& rFieldTextColor = SwViewOption::GetFontColor();
        SetFillColor(rFieldTextColor);
        DrawPolygon(Polygon(3, aArr));
    }
    else
    {
        // Intervall zeichnen
        if (rP1.Y() == rP2.Y())
        {
            // Waagerecht
            DrawLine(Point(rP1.X(), rP1.Y() - 2), Point(rP1.X(), rP1.Y() + 2));
            DrawLine(Point(rP2.X(), rP2.Y() - 2), Point(rP2.X(), rP2.Y() + 2));
        }
        else
        {
            // Senkrecht
            DrawLine(Point(rP1.X() - 2, rP1.Y()), Point(rP1.X() + 2, rP1.Y()));
            DrawLine(Point(rP2.X() - 2, rP2.Y()), Point(rP2.X() + 2, rP2.Y()));
        }
    }
}

// --------------------------------------------------------------------------

void SwLabPreview::Update(const SwLabItem& rItem)
{
    aItem = rItem;
    Invalidate();
}

// --------------------------------------------------------------------------



SwLabFmtPage::SwLabFmtPage(Window* pParent, const SfxItemSet& rSet) :

    SfxTabPage(pParent, SW_RES(TP_LAB_FMT), rSet),
    aItem		 ((const SwLabItem&) rSet.Get(FN_LABEL)),
    aMakeFI		 (this, ResId(FI_MAKE)),
    aTypeFI      (this, ResId(FI_TYPE)),
    aPreview	 (this, ResId(WIN_PREVIEW)),
    aHDistText	 (this, ResId(TXT_HDIST  )),
    aHDistField  (this, ResId(FLD_HDIST  )),
    aVDistText	 (this, ResId(TXT_VDIST  )),
    aVDistField  (this, ResId(FLD_VDIST  )),
    aWidthText	 (this, ResId(TXT_WIDTH  )),
    aWidthField  (this, ResId(FLD_WIDTH  )),
    aHeightText  (this, ResId(TXT_HEIGHT )),
    aHeightField (this, ResId(FLD_HEIGHT )),
    aLeftText	 (this, ResId(TXT_LEFT	 )),
    aLeftField	 (this, ResId(FLD_LEFT	 )),
    aUpperText	 (this, ResId(TXT_UPPER  )),
    aUpperField  (this, ResId(FLD_UPPER  )),
    aColsText	 (this, ResId(TXT_COLUMNS)),
    aColsField	 (this, ResId(FLD_COLUMNS)),
    aRowsText	 (this, ResId(TXT_ROWS	 )),
    aRowsField	 (this, ResId(FLD_ROWS	 )),
    aSavePB		 (this, ResId(PB_SAVE  )),
    bModified(FALSE)

{
    FreeResource();
    SetExchangeSupport();

    // Metriken
    FieldUnit aMetric = ::GetDfltMetric(FALSE);
    SetMetric(aHDistField , aMetric);
    SetMetric(aVDistField , aMetric);
    SetMetric(aWidthField , aMetric);
    SetMetric(aHeightField, aMetric);
    SetMetric(aLeftField  , aMetric);
    SetMetric(aUpperField , aMetric);

    // Handler installieren
    Link aLk = LINK(this, SwLabFmtPage, ModifyHdl);
    aHDistField .SetModifyHdl( aLk );
    aVDistField .SetModifyHdl( aLk );
    aWidthField .SetModifyHdl( aLk );
    aHeightField.SetModifyHdl( aLk );
    aLeftField	.SetModifyHdl( aLk );
    aUpperField .SetModifyHdl( aLk );
    aColsField	.SetModifyHdl( aLk );
    aRowsField	.SetModifyHdl( aLk );

    aLk = LINK(this, SwLabFmtPage, LoseFocusHdl);
    aHDistField .SetLoseFocusHdl( aLk );
    aVDistField .SetLoseFocusHdl( aLk );
    aWidthField .SetLoseFocusHdl( aLk );
    aHeightField.SetLoseFocusHdl( aLk );
    aLeftField	.SetLoseFocusHdl( aLk );
    aUpperField .SetLoseFocusHdl( aLk );
    aColsField	.SetLoseFocusHdl( aLk );
    aRowsField	.SetLoseFocusHdl( aLk );

    aSavePB.SetClickHdl( LINK (this, SwLabFmtPage, SaveHdl));
    // Timer einstellen
    aPreviewTimer.SetTimeout(1000);
    aPreviewTimer.SetTimeoutHdl(LINK(this, SwLabFmtPage, PreviewHdl));
}

// --------------------------------------------------------------------------



SwLabFmtPage::~SwLabFmtPage()
{
}

// Modify-Handler der MetricFields. Preview-Timer starten -------------------



IMPL_LINK_INLINE_START( SwLabFmtPage, ModifyHdl, Edit *, EMPTYARG )
{
    bModified = TRUE;
    aPreviewTimer.Start();
    return 0;
}
IMPL_LINK_INLINE_END( SwLabFmtPage, ModifyHdl, Edit *, EMPTYARG )

// Preview invalidaten ------------------------------------------------------



IMPL_LINK_INLINE_START( SwLabFmtPage, PreviewHdl, Timer *, EMPTYARG )
{
    aPreviewTimer.Stop();
    ChangeMinMax();
    FillItem( aItem );
    aPreview.Update( aItem );

    return 0;
}
IMPL_LINK_INLINE_END( SwLabFmtPage, PreviewHdl, Timer *, EMPTYARG )

// LoseFocus-Handler: Bei Aenderung sofort updaten --------------------------



IMPL_LINK_INLINE_START( SwLabFmtPage, LoseFocusHdl, Control *, pControl )
{
    if (((Edit*) pControl)->IsModified())
        PreviewHdl(0);
    return 0;
}
IMPL_LINK_INLINE_END( SwLabFmtPage, LoseFocusHdl, Control *, pControl )

// Minima und Maxima fuer Fields festlegen ----------------------------------


void SwLabFmtPage::ChangeMinMax()
{
    long lMax = 31748; // 56 cm

    // Min und Max

    long lLeft	= GETFLDVAL(aLeftField ),
         lUpper = GETFLDVAL(aUpperField),
         lHDist = GETFLDVAL(aHDistField),
         lVDist = GETFLDVAL(aVDistField);

    long nMinSize = 10;	// 0,1cm

    aHDistField .SetMin(nMinSize, FUNIT_CM);
    aVDistField .SetMin(nMinSize, FUNIT_CM);

    aHDistField .SetMax((long) 100 * ((lMax - lLeft ) / Max(1L, (long) aColsField.GetValue())), FUNIT_TWIP);
    aVDistField .SetMax((long) 100 * ((lMax - lUpper) / Max(1L, (long) aRowsField.GetValue())), FUNIT_TWIP);

    aWidthField .SetMin(nMinSize, FUNIT_CM);
    aHeightField.SetMin(nMinSize, FUNIT_CM);

    aWidthField .SetMax((long) 100 * (lHDist), FUNIT_TWIP);
    aHeightField.SetMax((long) 100 * (lVDist), FUNIT_TWIP);

    aLeftField	.SetMax((long) 100 * (lMax - (long) aColsField.GetValue() * GETFLDVAL(aHDistField)), FUNIT_TWIP);
    aUpperField .SetMax((long) 100 * (lMax - (long) aRowsField.GetValue() * GETFLDVAL(aVDistField)), FUNIT_TWIP);

    aColsField	.SetMax((lMax - lLeft ) / Max(1L, lHDist));
    aRowsField	.SetMax((lMax - lUpper) / Max(1L, lVDist));

    // First und Last

    aHDistField .SetFirst(aHDistField .GetMin());
    aVDistField .SetFirst(aVDistField .GetMin());

    aHDistField .SetLast (aHDistField .GetMax());
    aVDistField .SetLast (aVDistField .GetMax());

    aWidthField .SetFirst(aWidthField .GetMin());
    aHeightField.SetFirst(aHeightField.GetMin());

    aWidthField .SetLast (aWidthField .GetMax());
    aHeightField.SetLast (aHeightField.GetMax());

    aLeftField	.SetLast (aLeftField  .GetMax());
    aUpperField .SetLast (aUpperField .GetMax());

    aColsField	.SetLast (aColsField  .GetMax());
    aRowsField	.SetLast (aRowsField  .GetMax());

    aHDistField .Reformat();
    aVDistField .Reformat();
    aWidthField .Reformat();
    aHeightField.Reformat();
    aLeftField	.Reformat();
    aUpperField .Reformat();
    aColsField	.Reformat();
    aRowsField	.Reformat();
}

// --------------------------------------------------------------------------



SfxTabPage* SwLabFmtPage::Create(Window* pParent, const SfxItemSet& rSet)
{
    return new SwLabFmtPage(pParent, rSet);
}

// ----------------------------------------------------------------------------



void SwLabFmtPage::ActivatePage(const SfxItemSet& rSet)
{
    SfxItemSet aSet(rSet);
    Reset(aSet);
}

// ----------------------------------------------------------------------------



int SwLabFmtPage::DeactivatePage(SfxItemSet* pSet)
{
    if (pSet)
        FillItemSet(*pSet);

    return TRUE;
}

// --------------------------------------------------------------------------



void SwLabFmtPage::FillItem(SwLabItem& rItem)
{
    if (bModified)
    {
        rItem.aMake = rItem.aType = SW_RESSTR(STR_CUSTOM);

        SwLabRec& rRec = *GetParent()->Recs()[0];
        rItem.lHDist  = rRec.lHDist  = GETFLDVAL(aHDistField );
        rItem.lVDist  = rRec.lVDist  = GETFLDVAL(aVDistField );
        rItem.lWidth  = rRec.lWidth  = GETFLDVAL(aWidthField );
        rItem.lHeight = rRec.lHeight = GETFLDVAL(aHeightField);
        rItem.lLeft   = rRec.lLeft	 = GETFLDVAL(aLeftField  );
        rItem.lUpper  = rRec.lUpper  = GETFLDVAL(aUpperField );
        rItem.nCols   = rRec.nCols	 = (USHORT) aColsField.GetValue();
        rItem.nRows   = rRec.nRows	 = (USHORT) aRowsField.GetValue();
    }
}

// --------------------------------------------------------------------------



BOOL SwLabFmtPage::FillItemSet(SfxItemSet& rSet)
{
    FillItem(aItem);
    rSet.Put(aItem);

    return TRUE;
}

// --------------------------------------------------------------------------
void SwLabFmtPage::Reset(const SfxItemSet& rSet)
{
    // Fields initialisieren
    GetParent()->GetLabItem(aItem);

    aHDistField .SetMax(100 * aItem.lHDist , FUNIT_TWIP);
    aVDistField .SetMax(100 * aItem.lVDist , FUNIT_TWIP);
    aWidthField .SetMax(100 * aItem.lWidth , FUNIT_TWIP);
    aHeightField.SetMax(100 * aItem.lHeight, FUNIT_TWIP);
    aLeftField	.SetMax(100 * aItem.lLeft  , FUNIT_TWIP);
    aUpperField .SetMax(100 * aItem.lUpper , FUNIT_TWIP);

    SETFLDVAL(aHDistField , aItem.lHDist );
    SETFLDVAL(aVDistField , aItem.lVDist );
    SETFLDVAL(aWidthField , aItem.lWidth );
    SETFLDVAL(aHeightField, aItem.lHeight);
    SETFLDVAL(aLeftField  , aItem.lLeft  );
    SETFLDVAL(aUpperField , aItem.lUpper );

    aColsField.SetMax(aItem.nCols);
    aRowsField.SetMax(aItem.nRows);

    aColsField	.SetValue(aItem.nCols);
    aRowsField	.SetValue(aItem.nRows);
    aMakeFI.SetText(aItem.aMake);
    aTypeFI.SetText(aItem.aType);
    PreviewHdl(0);
}
/* -----------------------------22.01.01 15:11--------------------------------

 ---------------------------------------------------------------------------*/

IMPL_LINK( SwLabFmtPage, SaveHdl, PushButton *, EMPTYARG )
{
    SwLabRec aRec;
    aRec.lHDist  = GETFLDVAL(aHDistField );
    aRec.lVDist  = GETFLDVAL(aVDistField );
    aRec.lWidth  = GETFLDVAL(aWidthField );
    aRec.lHeight = GETFLDVAL(aHeightField);
    aRec.lLeft	 = GETFLDVAL(aLeftField  );
    aRec.lUpper  = GETFLDVAL(aUpperField );
    aRec.nCols	 = (USHORT) aColsField.GetValue();
    aRec.nRows	 = (USHORT) aRowsField.GetValue();
    aRec.bCont = aItem.bCont;
    SwSaveLabelDlg* pSaveDlg = new SwSaveLabelDlg(this, aRec);
    pSaveDlg->SetLabel(aItem.aLstMake, aItem.aLstType);
    pSaveDlg->Execute();
    if(pSaveDlg->GetLabel(aItem))
    {
        bModified = FALSE;
        const Sequence<OUString>& rMan = GetParent()->GetLabelsConfig().GetManufacturers();
        SvStringsDtor& rMakes = GetParent()->Makes();
        if(rMakes.Count() < (USHORT)rMan.getLength())
        {
            rMakes.DeleteAndDestroy(0, rMakes.Count());
            const OUString* pMan = rMan.getConstArray();
            for(sal_Int32 nMan = 0; nMan < rMan.getLength(); nMan++)
            {
                rMakes.Insert( new String(pMan[nMan]), rMakes.Count() );
            }
        }
        aMakeFI.SetText(aItem.aMake);
        aTypeFI.SetText(aItem.aType);
    }
    delete pSaveDlg;
    return 0;
}
/* -----------------------------23.01.01 10:41--------------------------------

 ---------------------------------------------------------------------------*/
SwSaveLabelDlg::SwSaveLabelDlg(SwLabFmtPage* pParent, SwLabRec& rRec) :
    ModalDialog(pParent, SW_RES(DLG_SAVE_LABEL)),
    rLabRec(rRec),
    pLabPage(pParent),
    aOptionsFL(this,ResId(FL_OPTIONS  )),
    aMakeFT(this, 	ResId(FT_MAKE     )),
    aMakeCB(this, 	ResId(CB_MAKE     )),
    aTypeFT(this, 	ResId(FT_TYPE     )),
    aTypeED(this, 	ResId(ED_TYPE     )),
    aOKPB(this, 	ResId(PB_OK     )),
    aCancelPB(this, ResId(PB_CANCEL )),
    aHelpPB(this, 	ResId(PB_HELP  	)),
    aQueryMB(this,  ResId(MB_QUERY )),
    bSuccess(sal_False)
{
    FreeResource();

    aOKPB.SetClickHdl(LINK(this, SwSaveLabelDlg, OkHdl));
    Link aLk(LINK(this, SwSaveLabelDlg, ModifyHdl));
    aMakeCB.SetModifyHdl(aLk);
    aTypeED.SetModifyHdl(aLk);

    SwLabelConfig& rCfg = pLabPage->GetParent()->GetLabelsConfig();
    const Sequence<OUString>& rMan = rCfg.GetManufacturers();
    const OUString* pMan = rMan.getConstArray();
    for(sal_Int32 i = 0; i < rMan.getLength(); i++)
        aMakeCB.InsertEntry(pMan[i]);
}
/* -----------------------------23.01.01 10:40--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwSaveLabelDlg, OkHdl, OKButton*, pButton)
{
    SwLabelConfig& rCfg = pLabPage->GetParent()->GetLabelsConfig();
    String sMake(aMakeCB.GetText());
    String sType(aTypeED.GetText());
    if(rCfg.HasLabel(sMake, sType))
    {
        String sTmp(aQueryMB.GetMessText());
        String sQuery(sTmp);
        sQuery.SearchAndReplace(C2S("%1"), sMake);
        sQuery.SearchAndReplace(C2S("%2"), sType);
        aQueryMB.SetMessText(sQuery);

        short eRet = aQueryMB.Execute();
        aQueryMB.SetMessText(sTmp);
        if(RET_YES != eRet)
            return 0;
    }
    rLabRec.aType = sType;
    rCfg.SaveLabel(sMake, sType, rLabRec);
    bSuccess = sal_True;
    EndDialog(RET_OK);
    return 0;
}
/* -----------------------------23.01.01 11:22--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwSaveLabelDlg, ModifyHdl, Edit*, EMPTYARG)
{
    aOKPB.Enable(aMakeCB.GetText().Len() && aTypeED.GetText().Len());
    return 0;
}
/* -----------------------------23.01.01 16:06--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwSaveLabelDlg::GetLabel(SwLabItem& rItem)
{
    if(bSuccess)
    {
        rItem.aMake = aMakeCB.GetText();
        rItem.aType = aTypeED.GetText();
        rItem.lHDist  = rLabRec.lHDist;
        rItem.lVDist  = rLabRec.lVDist;
        rItem.lWidth  = rLabRec.lWidth;
        rItem.lHeight = rLabRec.lHeight;
        rItem.lLeft   = rLabRec.lLeft;
        rItem.lUpper  = rLabRec.lUpper;
        rItem.nCols   = rLabRec.nCols;
        rItem.nRows   = rLabRec.nRows;
    }
    return bSuccess;
}

