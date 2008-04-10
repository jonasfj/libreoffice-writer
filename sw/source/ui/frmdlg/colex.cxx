/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: colex.cxx,v $
 * $Revision: 1.19 $
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




#include "cmdid.h"
#include "hintids.hxx"
#include <algorithm>


#include <svtools/eitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/sizeitem.hxx>
#include <svx/pageitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/frmdiritem.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/graph.hxx>
#include <tgrditem.hxx>
#include <viewopt.hxx>
#include "colex.hxx"
#include "colmgr.hxx"

/*-----------------------------------------------------------------------
    Beschreibung: Uebernahme der aktualisierten Werte aus dem Set
 -----------------------------------------------------------------------*/
void SwPageExample::UpdateExample( const SfxItemSet& rSet )
{
    const SvxPageItem* pPage = 0;
    SfxItemPool* pPool = rSet.GetPool();
    USHORT nWhich = pPool->GetWhich( SID_ATTR_PAGE );

    if ( rSet.GetItemState( nWhich, FALSE ) == SFX_ITEM_SET )
    {
        // Ausrichtung
        pPage = (const SvxPageItem*)&rSet.Get( nWhich );

        if ( pPage )
            SetUsage( pPage->GetPageUsage() );
    }

    nWhich = pPool->GetWhich( SID_ATTR_PAGE_SIZE );

    if ( rSet.GetItemState( nWhich, FALSE ) == SFX_ITEM_SET )
    {
        // Orientation und Size aus dem PageItem
        const SvxSizeItem& rSize = (const SvxSizeItem&)rSet.Get( nWhich );
        SetSize( rSize.GetSize() );
    }
    nWhich = RES_LR_SPACE;
    if ( rSet.GetItemState( nWhich, FALSE ) == SFX_ITEM_SET )
    {
        // linken und rechten Rand einstellen
        const SvxLRSpaceItem& rLRSpace = (const SvxLRSpaceItem&)rSet.Get( nWhich );

        SetLeft( rLRSpace.GetLeft() );
        SetRight( rLRSpace.GetRight() );
    }
    else
    {
        SetLeft( 0 );
        SetRight( 0 );
    }

    nWhich = RES_UL_SPACE;

    if ( rSet.GetItemState( nWhich, FALSE ) == SFX_ITEM_SET )
    {
        // oberen und unteren Rand einstellen
        const SvxULSpaceItem& rULSpace = (const SvxULSpaceItem&)rSet.Get( nWhich );

        SetTop( rULSpace.GetUpper() );
        SetBottom( rULSpace.GetLower() );
    }
    else
    {
        SetTop( 0 );
        SetBottom( 0 );
    }


    // Kopfzeilen-Attribute auswerten
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rSet.GetItemState( pPool->GetWhich( SID_ATTR_PAGE_HEADERSET),
            FALSE, &pItem ) )
    {
        const SfxItemSet& rHeaderSet = ((SvxSetItem*)pItem)->GetItemSet();
        const SfxBoolItem& rHeaderOn =
            (const SfxBoolItem&)rHeaderSet.Get( pPool->GetWhich( SID_ATTR_PAGE_ON ) );

        if ( rHeaderOn.GetValue() )
        {
            const SvxSizeItem& rSize =
                (const SvxSizeItem&)rHeaderSet.Get(pPool->GetWhich(SID_ATTR_PAGE_SIZE));

            const SvxULSpaceItem& rUL = (const SvxULSpaceItem&)rHeaderSet.Get(
                                        pPool->GetWhich(SID_ATTR_ULSPACE));
            const SvxLRSpaceItem& rLR = (const SvxLRSpaceItem&)rHeaderSet.Get(
                                        pPool->GetWhich(SID_ATTR_LRSPACE));

            SetHdHeight( rSize.GetSize().Height() - rUL.GetLower());
            SetHdDist( rUL.GetLower() );
            SetHdLeft( rLR.GetLeft() );
            SetHdRight( rLR.GetRight() );
            SetHeader( TRUE );
            if ( rHeaderSet.GetItemState( RES_BACKGROUND ) == SFX_ITEM_SET )
            {
                const SvxBrushItem& rItem =
                    (const SvxBrushItem&)rHeaderSet.Get( RES_BACKGROUND );
                SetHdColor( rItem.GetColor() );
            }
            if ( rHeaderSet.GetItemState( RES_BOX ) == SFX_ITEM_SET )
            {
                const SvxBoxItem& rItem =
                    (const SvxBoxItem&)rHeaderSet.Get( RES_BOX );
                SetHdBorder( rItem );
            }
        }
        else
            SetHeader( FALSE );
    }

    if( SFX_ITEM_SET == rSet.GetItemState( pPool->GetWhich( SID_ATTR_PAGE_FOOTERSET),
            FALSE, &pItem ) )
    {
        const SfxItemSet& rFooterSet = ((SvxSetItem*)pItem)->GetItemSet();
        const SfxBoolItem& rFooterOn =
            (const SfxBoolItem&)rFooterSet.Get( SID_ATTR_PAGE_ON );

        if ( rFooterOn.GetValue() )
        {
            const SvxSizeItem& rSize =
                (const SvxSizeItem&)rFooterSet.Get( pPool->GetWhich( SID_ATTR_PAGE_SIZE ) );

            const SvxULSpaceItem& rUL = (const SvxULSpaceItem&)rFooterSet.Get(
                                        pPool->GetWhich( SID_ATTR_ULSPACE ) );
            const SvxLRSpaceItem& rLR = (const SvxLRSpaceItem&)rFooterSet.Get(
                                        pPool->GetWhich( SID_ATTR_LRSPACE ) );

            SetFtHeight( rSize.GetSize().Height() - rUL.GetUpper());
            SetFtDist( rUL.GetUpper() );
            SetFtLeft( rLR.GetLeft() );
            SetFtRight( rLR.GetRight() );
            SetFooter( TRUE );
            if( rFooterSet.GetItemState( RES_BACKGROUND ) == SFX_ITEM_SET )
            {
                const SvxBrushItem& rItem =
                    (const SvxBrushItem&)rFooterSet.Get( RES_BACKGROUND );
                SetFtColor( rItem.GetColor() );
            }
            if( rFooterSet.GetItemState( RES_BOX ) == SFX_ITEM_SET )
            {
                const SvxBoxItem& rItem =
                    (const SvxBoxItem&)rFooterSet.Get( RES_BOX );
                SetFtBorder( rItem );
            }
        }
        else
            SetFooter( FALSE );
    }
    if( SFX_ITEM_SET == rSet.GetItemState( RES_BACKGROUND,
            FALSE, &pItem ) )
    {
        SetColor( ( (const SvxBrushItem*)pItem )->GetColor() );
        const Graphic* pGrf = ( (const SvxBrushItem*)pItem )->GetGraphic();

        if ( pGrf )
        {
            Bitmap aBitmap = pGrf->GetBitmap();
            SetBitmap( &aBitmap );
        }
        else
            SetBitmap( NULL );
    }

    Invalidate();
}
/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/


void SwColExample::DrawPage( const Point& rOrg,
                            const BOOL bSecond,
                            const BOOL bEnabled )
{
    SwPageExample::DrawPage( rOrg, bSecond, bEnabled );
    USHORT nColumnCount;
    if( pColMgr && 0 != (nColumnCount = pColMgr->GetCount()))
    {
        long nL = GetLeft();
        long nR = GetRight();

        if ( GetUsage() == SVX_PAGE_MIRROR && !bSecond )
        {
            // fuer gespiegelt drehen
            nL = GetRight();
            nR = GetLeft();
        }

        SetFillColor( Color( COL_LIGHTGRAY ) );
        Rectangle aRect;
        aRect.Right() = rOrg.X() + GetSize().Width() - nR;
        aRect.Left()  = rOrg.X() + nL;
        aRect.Top()   = rOrg.Y() + GetTop()
                        + GetHdHeight() + GetHdDist();
        aRect.Bottom()= rOrg.Y() + GetSize().Height() - GetBottom()
                        - GetFtHeight() - GetFtDist();
        DrawRect(aRect);

        if(GetColor() == Color(COL_TRANSPARENT))
        {
            const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
            const Color& rFieldColor = rStyleSettings.GetFieldColor();
            SetFillColor( rFieldColor );
        }
        else
            SetFillColor( GetColor() );

        // #97495# make sure that the automatic column widht's are always equal
        BOOL bAutoWidth = pColMgr->IsAutoWidth();
        sal_Int32 nAutoColWidth = 0;
        if(bAutoWidth)
        {
            sal_Int32 nColumnWidthSum = 0;
            USHORT i;
            for(i = 0; i < nColumnCount; ++i)
                nColumnWidthSum += pColMgr->GetColWidth( i );
            nAutoColWidth = nColumnWidthSum / nColumnCount;
        }

        USHORT i;
        for( i = 0; i < nColumnCount; i++)
        {
            if(!bAutoWidth)
                nAutoColWidth = pColMgr->GetColWidth( i );
            aRect.Right() = aRect.Left() + nAutoColWidth;
            DrawRect(aRect);
            if(i < nColumnCount - 1)
                aRect.Left() = aRect.Right() + pColMgr->GetGutterWidth(i);
        }
        if(pColMgr->HasLine())
        {
            Point aUp( rOrg.X() + nL, rOrg.Y() + GetTop() );
            Point aDown( rOrg.X() + nL, rOrg.Y() + GetSize().Height()
                        - GetBottom() - GetFtHeight() - GetFtDist() );

            if( pColMgr->GetLineHeightPercent() != 100 )
            {
                long nLength = aDown.Y() - aUp.Y();
                nLength -= nLength * pColMgr->GetLineHeightPercent() / 100;
                switch(pColMgr->GetAdjust())
                {
                    case COLADJ_BOTTOM: aUp.Y() += nLength; break;
                    case COLADJ_TOP: aDown.Y() -= nLength; break;
                    case COLADJ_CENTER:
                        aUp.Y() += nLength / 2;
                        aDown.Y() -= nLength / 2;
                    break;
                    default:; // prevent warning
                }
            }

            int nDist;
            for( i = 0; i < nColumnCount -  1; i++)
            {
                int nGutter = pColMgr->GetGutterWidth(i);
                nDist = pColMgr->GetColWidth( i ) + nGutter;
                nDist -= (i == 0) ?
                    nGutter/2 :
                        0;
                aUp.X() += nDist;
                aDown.X() += nDist;
                DrawLine( aUp, aDown );

            }
        }
    }
}

/*-----------------25.10.96 09.15-------------------

--------------------------------------------------*/


SwColumnOnlyExample::SwColumnOnlyExample( Window* pParent, const ResId& rResId) :
    Window(pParent, rResId),
    m_aFrmSize(1,1)
{
    SetMapMode( MapMode( MAP_TWIP ) );
    m_aWinSize = GetOutputSizePixel();
    m_aWinSize.Height() -= 4;
    m_aWinSize.Width() -= 4;

    m_aWinSize = PixelToLogic( m_aWinSize );

    SetBorderStyle( WINDOW_BORDER_MONO );

    m_aFrmSize  = Size(11907, 16433);// DIN A4
    ::FitToActualSize(m_aCols, (USHORT)m_aFrmSize.Width());

    long nHeight = m_aFrmSize.Height();
    Fraction aScale( m_aWinSize.Height(), nHeight );
    MapMode aMapMode( GetMapMode() );
    aMapMode.SetScaleX( aScale );
    aMapMode.SetScaleY( aScale );
    SetMapMode( aMapMode );
}

/*-----------------25.10.96 09.16-------------------

--------------------------------------------------*/


void SwColumnOnlyExample::Paint( const Rectangle& /*rRect*/ )
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    const Color& rFieldColor = rStyleSettings.GetFieldColor();
    const Color& rDlgColor = rStyleSettings.GetDialogColor();
    const Color& rFieldTextColor = SwViewOption::GetFontColor();
    Color aGrayColor(COL_LIGHTGRAY);
    if(rFieldColor == aGrayColor)
        aGrayColor.Invert();

    Size aLogSize(PixelToLogic(GetOutputSizePixel()));
    Rectangle aCompleteRect(Point(0,0), aLogSize);
    SetLineColor(rDlgColor);
    SetFillColor(rDlgColor);
    DrawRect(aCompleteRect);

    SetLineColor( rFieldTextColor );
    Point aTL(  (aLogSize.Width() - m_aFrmSize.Width()) / 2,
                (aLogSize.Height() - m_aFrmSize.Height()) / 2);
    Rectangle aRect(aTL, m_aFrmSize);

    //draw a shadow rectangle
    SetFillColor( Color(COL_GRAY) );
    Rectangle aShadowRect(aRect);
    aShadowRect.Move(aTL.Y(), aTL.Y());
    DrawRect(aShadowRect);

    SetFillColor( rFieldColor );
    DrawRect(aRect);

    SetFillColor( aGrayColor );

    //Spaltentrenner?
    long nLength = aLogSize.Height() - 2 * aTL.Y();
    Point aUp( aTL );
    Point aDown( aTL.X(), nLength );
    BOOL bLines = FALSE;
    if(m_aCols.GetLineAdj() != COLADJ_NONE)
    {
        bLines = TRUE;

        USHORT nPercent = m_aCols.GetLineHeight();
        if( nPercent != 100 )
        {
            nLength -= nLength * nPercent / 100;
            switch(m_aCols.GetLineAdj())
            {
                case COLADJ_BOTTOM: aUp.Y() += nLength; break;
                case COLADJ_TOP: aDown.Y() -= nLength; break;
                case COLADJ_CENTER:
                        aUp.Y() += nLength / 2;
                        aDown.Y() -= nLength / 2;
                break;
                default:; //prevent warning
            }
        }

    }
    const SwColumns& rCols = m_aCols.GetColumns();
    USHORT nColCount = rCols.Count();
    if( nColCount )
    {
        DrawRect(aRect);
        SetFillColor( rFieldColor );
        Rectangle aFrmRect(aTL, m_aFrmSize);
        long nSum = aTL.X();
        for(USHORT i = 0; i < nColCount; i++)
        {
            SwColumn* pCol = rCols[i];
            aFrmRect.Left()    = nSum + pCol->GetLeft();//nSum + pCol->GetLeft() + aTL.X();
            nSum              += pCol->GetWishWidth();
            aFrmRect.Right()   = nSum - pCol->GetRight();
            DrawRect(aFrmRect);
        }
        if(bLines )
        {
            nSum = aTL.X();
            for(USHORT i = 0; i < nColCount - 1; i++)
            {
                nSum += rCols[i]->GetWishWidth();
                aUp.X() = nSum;
                aDown.X() = nSum;
                DrawLine(aUp, aDown);
            }
        }
    }
}

/*-----------------25.10.96 12.05-------------------

--------------------------------------------------*/


void  SwColumnOnlyExample::SetColumns(const SwFmtCol& rCol)
{
    m_aCols = rCol;
    USHORT nWishSum = m_aCols.GetWishWidth();
    long nFrmWidth = m_aFrmSize.Width();
    SwColumns& rCols = m_aCols.GetColumns();
    USHORT nColCount = rCols.Count();

    for(USHORT i = 0; i < nColCount; i++)
    {
        SwColumn* pCol = rCols[i];
        long nWish = pCol->GetWishWidth();
        nWish *= nFrmWidth;
        nWish /= nWishSum;
        pCol->SetWishWidth((USHORT)nWish);
        long nLeft = pCol->GetLeft();
        nLeft *= nFrmWidth;
        nLeft /= nWishSum;
        pCol->SetLeft((USHORT)nLeft);
        long nRight = pCol->GetRight();
        nRight *= nFrmWidth;
        nRight /= nWishSum;
        pCol->SetRight((USHORT)nRight);
    }
    // #97495# make sure that the automatic column width's are always equal
    if(nColCount && m_aCols.IsOrtho())
    {
        sal_Int32 nColumnWidthSum = 0;
        USHORT i;
        for(i = 0; i < nColCount; ++i)
        {
            SwColumn* pCol = rCols[i];
            nColumnWidthSum += pCol->GetWishWidth();
            nColumnWidthSum -= (pCol->GetRight() + pCol->GetLeft());
        }
        nColumnWidthSum /= nColCount;
        for(i = 0; i < nColCount; ++i)
        {
            SwColumn* pCol = rCols[i];
            pCol->SetWishWidth( static_cast< USHORT >(nColumnWidthSum + pCol->GetRight() + pCol->GetLeft()));
        }
    }
}
/* -----------------------------08.02.2002 11:44------------------------------

 ---------------------------------------------------------------------------*/
SwPageGridExample::~SwPageGridExample()
{
    delete pGridItem;
}
/* -----------------------------08.02.2002 11:48------------------------------

 ---------------------------------------------------------------------------*/
#define MAX_ROWS    10
#define MAX_LINES   15
void SwPageGridExample::DrawPage( const Point& rOrg,
                           const BOOL bSecond,
                           const BOOL bEnabled )
{
    SwPageExample::DrawPage(rOrg, bSecond, bEnabled);
    if(pGridItem && pGridItem->GetGridType())
    {
        //paint the grid now
        Color aLineColor = pGridItem->GetColor();
        if(aLineColor.GetColor() == COL_AUTO)
        {
            aLineColor = GetFillColor();
            aLineColor.Invert();
        }
        SetLineColor(aLineColor);
        long nL = GetLeft();
        long nR = GetRight();

        if ( GetUsage() == SVX_PAGE_MIRROR && !bSecond )
        {
            // fuer gespiegelt drehen
            nL = GetRight();
            nR = GetLeft();
        }

        Rectangle aRect;
        aRect.Right() = rOrg.X() + GetSize().Width() - nR;
        aRect.Left()  = rOrg.X() + nL;
        aRect.Top()	  = rOrg.Y() + GetTop()
                        + GetHdHeight() + GetHdDist();
        aRect.Bottom()= rOrg.Y() + GetSize().Height() - GetBottom()
                        - GetFtHeight() - GetFtDist();

        //increase the values to get a 'viewable' preview
        sal_Int32 nBaseHeight = pGridItem->GetBaseHeight() * 3;
        sal_Int32 nRubyHeight = pGridItem->GetRubyHeight() * 3;

        //detect height of rectangles
        Rectangle aRubyRect(aRect.TopLeft(),
                    m_bVertical ?
                    Size(nRubyHeight, aRect.GetHeight()) :
                    Size(aRect.GetWidth(), nRubyHeight));
        Rectangle aCharRect(aRect.TopLeft(),
                    m_bVertical ?
                    Size(nBaseHeight, aRect.GetHeight()) :
                    Size(aRect.GetWidth(), nBaseHeight));

        sal_Int32 nLineHeight = nBaseHeight + nRubyHeight;

        //detect count of rectangles
        sal_Int32 nLines = (m_bVertical ? aRect.GetWidth(): aRect.GetHeight()) / nLineHeight;
        if(nLines > pGridItem->GetLines())
            nLines = pGridItem->GetLines();

        // determine start position
        if(m_bVertical)
        {
            sal_Int16 nXStart = static_cast< sal_Int16 >(aRect.GetWidth() / 2 - nLineHeight * nLines /2);
            aRubyRect.Move(nXStart, 0);
            aCharRect.Move(nXStart, 0);
        }
        else
        {
            sal_Int16 nYStart = static_cast< sal_Int16 >(aRect.GetHeight() / 2 - nLineHeight * nLines /2);
            aRubyRect.Move(0, nYStart);
            aCharRect.Move(0, nYStart);
        }

        if(pGridItem->IsRubyTextBelow())
            m_bVertical ? aRubyRect.Move(nBaseHeight, 0) : aRubyRect.Move(0, nBaseHeight);
        else
            m_bVertical ? aCharRect.Move(nRubyHeight, 0) : aCharRect.Move(0, nRubyHeight);

        //vertical lines
        sal_Bool bBothLines = pGridItem->GetGridType() == GRID_LINES_CHARS;
        SetFillColor( Color( COL_TRANSPARENT ) );
        sal_Int32 nXMove = m_bVertical ? nLineHeight : 0;
        sal_Int32 nYMove = m_bVertical ? 0 : nLineHeight;
        for(sal_Int32 nLine = 0; nLine < nLines; nLine++)
        {
            DrawRect(aRubyRect);
            DrawRect(aCharRect);
            if(bBothLines)
            {
                Point aStart = aCharRect.TopLeft();
                Point aEnd = m_bVertical ? aCharRect.TopRight() : aCharRect.BottomLeft();
                while(m_bVertical ? aStart.Y() < aRect.Bottom(): aStart.X() < aRect.Right())
                {
                    DrawLine(aStart, aEnd);
                    if(m_bVertical)
                        aStart.Y() = aEnd.Y() += nBaseHeight;
                    else
                        aStart.X() = aEnd.X() += nBaseHeight;
                }
            }
            aRubyRect.Move(nXMove, nYMove);
            aCharRect.Move(nXMove, nYMove);
        }
    }
}
/* -----------------------------08.02.2002 11:48------------------------------

 ---------------------------------------------------------------------------*/
void SwPageGridExample::UpdateExample( const SfxItemSet& rSet )
{
    DELETEZ(pGridItem);
    //get the grid information
    if(SFX_ITEM_AVAILABLE <= rSet.GetItemState(RES_TEXTGRID, TRUE))
        pGridItem = (SwTextGridItem*)((const SwTextGridItem&)rSet.Get(RES_TEXTGRID)).Clone();
    if( SFX_ITEM_AVAILABLE <= rSet.GetItemState( RES_FRAMEDIR, TRUE ))
    {
        const SvxFrameDirectionItem& rDirItem =
                    (const SvxFrameDirectionItem&)rSet.Get(RES_FRAMEDIR);
        m_bVertical = rDirItem.GetValue() == FRMDIR_VERT_TOP_RIGHT||
                    rDirItem.GetValue() == FRMDIR_VERT_TOP_LEFT;
    }
    SwPageExample::UpdateExample(rSet);
}

