/*************************************************************************
 *
 *  $RCSfile: viewpg.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: od $ $Date: 2002-12-03 15:41:50 $
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
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SV_WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _SFX_PROGRESS_HXX //autogen
#include <sfx2/progress.hxx>
#endif

#ifndef _PVPRTDAT_HXX
#include <pvprtdat.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _SWPRTOPT_HXX
#include <swprtopt.hxx> // SwPrtOptions
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _PTQUEUE_HXX
#include <ptqueue.hxx>
#endif
#ifndef _SWREGION_HXX
#include <swregion.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _FNTCACHE_HXX
#include <fntcache.hxx>
#endif

#ifndef _STATSTR_HRC
#include <statstr.hrc>	// Text fuer SfxProgress
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif

const SwTwips nXFree = 4*142;        // == 0.25 cm * 4
const SwTwips nYFree = 4*142;
static USHORT nPrevViewXFreePix = 0;
static USHORT nPrevViewYFreePix = 0;

SwPageFrm* lcl_GetSttPage( BYTE& rRow, BYTE& rCol, USHORT& rSttPage,
                                const SwRootFrm* pRoot )
{
    USHORT nCalcSttPg = rSttPage;
    if( nCalcSttPg )
        --nCalcSttPg;
    if( !rCol )
        ++rCol;
    if( !rRow )
        ++rRow;

    // suche die Start-Seite
    SwPageFrm *pSttPage = (SwPageFrm*)pRoot->Lower(), *pPrevPage = pSttPage;
    while( pSttPage && nCalcSttPg )
    {
        pPrevPage = pSttPage;
        pSttPage = (SwPageFrm*)pSttPage->GetNext();
        --nCalcSttPg;
    }

    // keine Seiten mehr ??
    if( !pSttPage )
    {
        pSttPage = pPrevPage;
        rSttPage -= nCalcSttPg;		// das sind die tatsaechlichen
        if( 1 == (rSttPage & 1 ))	// eine rechte Seite?
            ++rSttPage;				// dann sorge dafuer das sie rechts steht

        // dann dem Wunsch entsprechend Rueckwaerts
        for( USHORT n = rRow * rCol; n && pSttPage; --n )
        {
            pPrevPage = pSttPage;
            pSttPage = (SwPageFrm*)pSttPage->GetPrev();
            --rSttPage;
        }
        if( !pSttPage )
        {
            pSttPage = pPrevPage;
            rSttPage = 1;
        }
    }
    return pSttPage;
}

USHORT ViewShell::CalcPreViewPage(
        const Size& rWinSize, 		// auf diese Size die Scalierung errechnen
        USHORT& rRowCol, 			// Zeilen/Spalten (Row im Hi, Col im LowByte!)
        USHORT nSttPage,			// Start ab dieser Seite, eine gueltige ??
        Size& rMaxSize, 			// MaxSize einer Seite
        USHORT& rVirtPageNo, 		// virtuelle SeitenNummer
        USHORT nAccSelPage )		// selected page for accessibility
{
    const SwRootFrm* pRoot = GetLayout();
    ASSERT( pRoot, "Wo ist mein Layout?" );

    // ohne Win-Size ist nichts los
    if( !rWinSize.Width() || !rWinSize.Height() )
        return 0;

    // von allen Seiten die Size holen. Das Max entscheidet
    // ueber die Scalierung.  (So macht es WW)
    register SwTwips nMaxRowSz = 0, nMaxColSz = 0;
    register USHORT nCalcSttPg = nSttPage;
    if( nCalcSttPg )
        --nCalcSttPg;

    BYTE nRow = BYTE(rRowCol >> 8), nCol = BYTE(rRowCol & 0xff);
    if( !nCol )
        ++nCol;
    if( !nRow )
        ++nRow;

    // suche bestimme die Max-Breite
    register SwPageFrm *pSttPage = (SwPageFrm*)pRoot->Lower(),
                        *pPrevPage = pSttPage;
    while( pSttPage && nCalcSttPg )
    {
        pSttPage->Calc();
        register Size& rSize = pSttPage->Frm().SSize();
        if( nMaxColSz < rSize.Width() )
            nMaxColSz = rSize.Width();
        if( nMaxRowSz < rSize.Height() )
            nMaxRowSz = rSize.Height();
        pPrevPage = pSttPage;
        pSttPage = (SwPageFrm*)pSttPage->GetNext();
        --nCalcSttPg;
    }

    // keine Seiten mehr ??
    if( !pSttPage )
    {
        pSttPage = pPrevPage;
        nSttPage -= nCalcSttPg;		// das sind die tatsaechlichen
#if OLD
// hiermit stimmt die Errechnung der vir. PageNo nicht !!
        if( 1 == (nSttPage & 1 ))	// eine rechte Seite?
            ++nSttPage;				// dann sorge dafuer das sie rechts steht

        // dann dem Wunsch entsprechend Rueckwaerts
        for( USHORT n = nRow * nCol; n && pSttPage; --n )
        {
            pPrevPage = pSttPage;
            pSttPage = (SwPageFrm*)pSttPage->GetPrev();
            --nSttPage;
        }
#else
        USHORT n = nRow * nCol;
        if( 1 == (nSttPage & 1 ))	// eine rechte Seite?
            --n;					// dann sorge dafuer das sie rechts steht

        // dann dem Wunsch entsprechend Rueckwaerts
        for( ; n && pSttPage; --n )
        {
            pPrevPage = pSttPage;
            pSttPage = (SwPageFrm*)pSttPage->GetPrev();
            --nSttPage;
        }
#endif
        if( !pSttPage )
        {
            pSttPage = pPrevPage;
            nSttPage = 0;
        }
        rVirtPageNo = pPrevPage->GetVirtPageNum();
    }
    else 	// dann haben wir unsere StartSeite,
            // also weiter und das Max bestimmen
    {
        rVirtPageNo = pSttPage->GetVirtPageNum();
        while( pSttPage )
        {
            pSttPage->Calc();
            register Size& rSize = pSttPage->Frm().SSize();
            if( nMaxColSz < rSize.Width() )
                nMaxColSz = rSize.Width();
            if( nMaxRowSz < rSize.Height() )
                nMaxRowSz = rSize.Height();
            pSttPage = (SwPageFrm*)pSttPage->GetNext();
        }
    }

    rMaxSize.Height() = nMaxRowSz;
    rMaxSize.Width() = nMaxColSz;
    rRowCol = (nRow << 8) + nCol;		// und wieder returnen

    // Jetzt die Scalierung incl. der gewuenschten Zwischenraueme errechnen
    nMaxColSz = nCol * nMaxColSz;
    nMaxRowSz = nRow * nMaxRowSz;

    MapMode aMapMode( MAP_TWIP );
    // falls die Abstaende zwischen den Seiten noch nicht berechnet wurden
    if( !nPrevViewXFreePix || !nPrevViewYFreePix )
    {
        Size aTmpSz( nXFree, nYFree );
        aTmpSz = GetOut()->LogicToPixel( aTmpSz, aMapMode );
        nPrevViewXFreePix = USHORT( aTmpSz.Width() );
        nPrevViewYFreePix = USHORT( aTmpSz.Height() );
    }
    Size aSz( rWinSize.Width() - ((nCol+1) * nPrevViewXFreePix),
              rWinSize.Height() - ( (nRow+1) * nPrevViewYFreePix ) );
    aSz = GetOut()->PixelToLogic( aSz, aMapMode );
    Fraction aScX( aSz.Width(), nMaxColSz );
    Fraction aScY( aSz.Height(), nMaxRowSz );
    if( aScX < aScY )
        aScY = aScX;

    {
        // fuer Drawing, damit diese ihre Objecte vernuenftig Painten
        // koennen, auf "glatte" Prozentwerte setzen
        aScY *= Fraction( 1000, 1 );
        long nTmp = (long)aScY;
        if( 1 < nTmp )
            --nTmp;
        else
            nTmp = 1;
        aScY = Fraction( nTmp, 1000 );
        if( pOpt )								// fuer den Font-Cache die
            pOpt->SetZoom( USHORT(nTmp / 10));	// Options mitpflegen!
    }
    aMapMode.SetScaleY( aScY );
    aMapMode.SetScaleX( aScY );

#ifdef ACCESSIBLE_LAYOUT
    if( USHRT_MAX == nAccSelPage )
        nAccSelPage = nSttPage;
    else if(nAccSelPage < nSttPage || nAccSelPage > nSttPage + (nRow * nCol) )
        nAccSelPage = nSttPage ? nSttPage : 1;

    Imp()->UpdateAccessiblePreview( nRow, nCol, nSttPage, rMaxSize,
                                    GetPreviewFreePix(), aScY, nAccSelPage );
#endif

    // was machen wir, wenn der MapMode unter einer
    // gewissen Schwelle (z.B. 10% (WW6)) ist ???

    GetOut()->SetMapMode( aMapMode );
    return nSttPage;
}
#ifdef ACCESSIBLE_LAYOUT
void ViewShell::ShowPreViewSelection( sal_uInt16 nSelPage )
{
    Imp()->InvalidateAccessiblePreViewSelection( nSelPage );
}
#endif

void ViewShell::PreViewPage(
        const Rectangle& rRect, 	// Paint-Rect von Windows
        USHORT nRowCol,				// Anzahl Zeilen/Spalten
        USHORT nSttPage,			// Start ab dieser Seite, eine gueltige ??
        const Size& rPageSize,      // MaxSize einer Seite
        sal_uInt16 nSelectedPage )  // Page to be highlighted
{
    if( !GetWin() && !GetOut()->GetConnectMetaFile() )
        return;						//Fuer den Drucker tun wir hier nix

    SET_CURR_SHELL( this );

    const SwRootFrm* pRoot = GetLayout();
    ASSERT( pRoot, "Wo ist mein Layout?" );

    BYTE nRow = BYTE(nRowCol >> 8), nCol = BYTE(nRowCol & 0xff);
    SwPageFrm* pSttPage = ::lcl_GetSttPage( nRow, nCol, nSttPage, pRoot );

    // damit auch die Seiten im Paint angezeigt werden, gleich die richtigen
    // Werte setzen! (siehe auch Bug 24575)
    Imp()->bFirstPageInvalid = FALSE;
    Imp()->pFirstVisPage = pSttPage;

    const Rectangle aPixRect( GetOut()->LogicToPixel( rRect ) );

    MapMode aMapMode( GetOut()->GetMapMode() );
    MapMode aCalcMapMode( aMapMode );
    aCalcMapMode.SetOrigin( Point() );

    SwRegionRects aRegion( rRect );//fuer die Wiese

    Point aFreePt( nPrevViewXFreePix, nPrevViewYFreePix );
    aFreePt = GetOut()->PixelToLogic( aFreePt, aCalcMapMode );
    Point aCalcPt( aFreePt );
    long nPageHeight = aFreePt.Y() + rPageSize.Height()+1;
    SwPageFrm* pPage = pSttPage;

    // erstmal die Wiese
    // Sonderbehandlung fuer die erste Seite
    // JP 19.08.98: aber nur wenn sie mehrspaltig angezeigt wird
    int bFirstPg = !nSttPage && 1 != nCol;

    for( BYTE nCntRow = 0; pPage && nCntRow < nRow; ++nCntRow )
    {
        aCalcPt.X() = aFreePt.X();
        for( BYTE nCntCol = 0; pPage && nCntCol < nCol; ++nCntCol )
        {
            pPage->Calc();

            // Sonderbehandlung fuer 1. Seite, ist immer eine rechte
            if( bFirstPg )
            {
                bFirstPg = FALSE;
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
                continue;
            }

            if( pPage->IsEmptyPage() )
            {
                aRegion -= SwRect( aCalcPt, rPageSize );
                aCalcPt.X() += rPageSize.Width()+1 + aFreePt.X();
            }
            else
            {
                aRegion -= SwRect( aCalcPt, pPage->Frm().SSize() );
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        aCalcPt.Y() += nPageHeight;
    }
    GetOut()->SetMapMode( aCalcMapMode );
    _PaintDesktop( aRegion );

    // und dann das Dokument
    bFirstPg = !nSttPage && 1 != nCol;
    aCalcPt = aFreePt;
    pPage = pSttPage;
    Font* pEmptyPgFont = 0;
    nSelectedPage -= nSttPage;
    Color aLineColor(COL_LIGHTBLUE);
    DBG_ASSERT(pWin, "no window available")
    const StyleSettings& rSettings = GetWin()->GetSettings().GetStyleSettings();
    if(rSettings.GetHighContrastMode())
        aLineColor = rSettings.GetHighlightTextColor();
    const Color& rColor = rSettings.GetFieldTextColor();
    for( nCntRow = 0; pPage && nCntRow < nRow; ++nCntRow )
    {
        aCalcPt.X() = aFreePt.X();
        for( BYTE nCntCol = 0; pPage && nCntCol < nCol; ++nCntCol )
        {
            // Sonderbehandlung fuer 1. Seite, ist immer eine rechte
            if( bFirstPg )
            {
                bFirstPg = FALSE;
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
                nSelectedPage--;
                continue;
            }

            Rectangle aPageRect;

            if( pPage->IsEmptyPage() )
            {
                Rectangle aRect( aCalcPt, rPageSize );

                aMapMode.SetOrigin( Point(0,0) );
                GetOut()->SetMapMode( aMapMode );

                const Color aRetouche( Imp()->GetRetoucheColor() );

                if( GetOut()->GetFillColor() != aRetouche )
                    GetOut()->SetFillColor( aRetouche );

                GetOut()->DrawRect( aRect );

                if( !pEmptyPgFont )
                {
                    pEmptyPgFont = new Font;
                    pEmptyPgFont->SetSize( Size( 0, 80 * 20 )); // == 80 pt
                    pEmptyPgFont->SetWeight( WEIGHT_BOLD );
                    pEmptyPgFont->SetStyleName( aEmptyStr );
                    pEmptyPgFont->SetName( String::CreateFromAscii(
                            RTL_CONSTASCII_STRINGPARAM( "Helvetica" )) );
                    pEmptyPgFont->SetFamily( FAMILY_SWISS );
                    pEmptyPgFont->SetTransparent( TRUE );
                    pEmptyPgFont->SetColor( COL_GRAY );
                }
                Font aOldFont( GetOut()->GetFont() );
                GetOut()->SetFont( *pEmptyPgFont );
                GetOut()->DrawText( aRect, SW_RESSTR( STR_EMPTYPAGE ),
                                    TEXT_DRAW_VCENTER |
                                    TEXT_DRAW_CENTER |
                                    TEXT_DRAW_CLIP );
                GetOut()->SetFont( aOldFont );
                aPageRect = GetOut()->LogicToPixel( aRect );

                aCalcPt.X() += rPageSize.Width()+1 + aFreePt.X();
            }
            else
            {
                aVisArea = pPage->Frm();

                Point aPos( aCalcPt );
                aPos -= aVisArea.Pos();

                aMapMode.SetOrigin( aPos );
                GetOut()->SetMapMode( aMapMode );

                Rectangle aSVRect( GetOut()->LogicToPixel( aVisArea.SVRect() ) );
                aPageRect = aSVRect;
                if( aPixRect.IsOver( aSVRect ) )
                {
                    aSVRect.Intersection( aPixRect );
                    aSVRect = GetOut()->PixelToLogic( aSVRect );

                    Paint( aSVRect );
                }
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
            }

            Rectangle aShadow( aPageRect.Left()+2, aPageRect.Bottom()+1,
                                aPageRect.Right()+2, aPageRect.Bottom()+2 );
            Color aFill( GetOut()->GetFillColor() );
            Color aLine( GetOut()->GetLineColor() );
            GetOut()->SetFillColor( rColor );
            GetOut()->SetLineColor( rColor );
            aShadow = GetOut()->PixelToLogic( aShadow );
            GetOut()->DrawRect( aShadow );
            aShadow.Left() = aPageRect.Right() + 1;
            aShadow.Right() = aShadow.Left() + 1;
            aShadow.Top() = aPageRect.Top() + 2;
            aShadow.Bottom() = aPageRect.Bottom();
            aShadow = GetOut()->PixelToLogic( aShadow );
            GetOut()->DrawRect( aShadow );
            aShadow = GetOut()->PixelToLogic( aPageRect );
            GetOut()->SetFillColor( Color( COL_TRANSPARENT ) );
            GetOut()->DrawRect( aShadow );
            if( !nSelectedPage )
            {
                aShadow.Right() = aPageRect.Right() - 1;
                aShadow.Left() = aPageRect.Left() + 1;
                aShadow.Top() = aPageRect.Top() + 1;
                aShadow.Bottom() = aPageRect.Bottom() - 1;
                aShadow = GetOut()->PixelToLogic( aShadow );
                GetOut()->SetFillColor( Color( COL_TRANSPARENT ) );
                GetOut()->SetLineColor( aLineColor );
                GetOut()->DrawRect( aShadow );
                aShadow.Right() = aPageRect.Right() - 2;
                aShadow.Left() = aPageRect.Left() + 2;
                aShadow.Top() = aPageRect.Top() + 2;
                aShadow.Bottom() = aPageRect.Bottom() - 2;
                aShadow = GetOut()->PixelToLogic( aShadow );
                GetOut()->DrawRect( aShadow );
            }
            GetOut()->SetFillColor( aFill );
            GetOut()->SetLineColor( aLine );

            pPage = (SwPageFrm*)pPage->GetNext();
            nSelectedPage--;
        }
        aCalcPt.Y() += nPageHeight;
    }

    delete pEmptyPgFont;
    GetOut()->SetMapMode( aCalcMapMode );
    aVisArea.Clear();		// verhinder internes Painten !!
}

/** init page preview layout - new method, replacing <CalcPreViewPage>

    OD 27.11.2002 #103492#
    initialize the page preview settings for a given layout.
    side effects:
    (1) data struture for current preview settings are initialized and set.
    (2) if parameter <_bCalcScale> is true, mapping mode with calculated
    scaling is set at the output device and the zoom at the view options is
    set with the calculated scaling.

    @author OD

    @param _nCols
    input parameter - initial number of page columns in the preview.

    @param _nRows
    input parameter - initial number of page rows in the preview.

    @param _orMaxPageSize
    output parameter - maximal size in width and height of all pages

    @param _orPreviewDocSize
    output parameter - size of the document in the proposed preview layout
    included the spacing between the pages.

    @param _bCalcScale
    input parameter - control, if method should calculate the needed
    scaling for the proposed preview layout for the given window size
    and sets the scaling at the output device and the view options.

    @param _pPxWinSize
    input parameter - window size in which the preview will be displayed and
    for which the scaling will be calculated.

    @return boolean, indicating, if preview layout is successful initialized.
*/
bool ViewShell::InitPreviewLayout( const sal_uInt16 _nCols,
                                   const sal_uInt16 _nRows,
                                   Size&            _orMaxPageSize,
                                   Size&            _orPreviewDocSize,
                                   const bool       _bCalcScale,
                                   const Size*      _pPxWinSize
                                 )
{
    const SwRootFrm* pRootFrm = GetLayout();
    // check environment and parameters
    {
        ASSERT( pRootFrm, "No layout - preview layout can *not* be initialized" );
        if ( !pRootFrm )
            return false;

        bool bColsRowsValid = (_nCols != 0) && (_nRows != 0);
        ASSERT( bColsRowsValid, "preview layout parameters not correct - preview layout can *not* be initialized" );
        if ( !bColsRowsValid )
            return false;

        bool bPxWinSizeValid = _pPxWinSize &&
                (_pPxWinSize->Width() != 0) && (_pPxWinSize->Height() != 0);
        ASSERT( bPxWinSizeValid, "no window size - preview layout can *not* be initialized" );
        if ( !bPxWinSizeValid )
            return false;
    }

    // environment and parameters ok

    // clear existing preview settings
    Imp()->CurrPrevwSet().Clear();

    // set layout information at preview settings
    Imp()->CurrPrevwSet().nCols = _nCols;
    Imp()->CurrPrevwSet().nRows = _nRows;

    // calculate maximal page size; calculate also number of pages
    const SwPageFrm* pPage = static_cast<const SwPageFrm*>(pRootFrm->Lower());
    register Size aMaxPageSize( 0,0 );
    register sal_uInt16 nPages = 0;
    while ( pPage )
    {
        ++nPages;
        pPage->Calc();
        register const Size& rPageSize = pPage->Frm().SSize();
        if ( rPageSize.Width() > aMaxPageSize.Width() )
            aMaxPageSize.Width() = rPageSize.Width();
        if ( rPageSize.Height() > aMaxPageSize.Height() )
            aMaxPageSize.Height() = rPageSize.Height();
        pPage = static_cast<const SwPageFrm*>(pPage->GetNext());
    }
    // set maximal page size, column width and row height at preview settings
    Imp()->CurrPrevwSet().aMaxPageSize = aMaxPageSize;
    Imp()->CurrPrevwSet().nColWidth = aMaxPageSize.Width() + nXFree;
    Imp()->CurrPrevwSet().nRowHeight = aMaxPageSize.Height() + nYFree;

    // set number of pages at preview settings
    Imp()->CurrPrevwSet().nPages = nPages;
    // validate layout information
    Imp()->CurrPrevwSet().bLayoutInfoValid = true;

    // return calculated maximal size in width and height of all pages
    _orMaxPageSize = Imp()->CurrPrevwSet().aMaxPageSize;

    // calculate document size in preview layout and set it at preview settings
    {
        // document width
        Imp()->CurrPrevwSet().aPreviewDocSize.Width() =
                _nCols * aMaxPageSize.Width() + (_nCols+1) * nXFree;

        // document height
        // determine number of rows needed for <nPages> in preview layout
        // <nPages + 1>, because top-left-corner of preview layout is left free.
        sal_uInt16 nDocRows = (nPages + 1) / _nCols;
        if ( ( (nPages + 1) % _nCols ) > 0 )
            ++nDocRows;
        Imp()->CurrPrevwSet().aPreviewDocSize.Height() =
                nDocRows * aMaxPageSize.Height() + (nDocRows+1) * nYFree;
    }
    // return calculated document size in preview layout
    _orPreviewDocSize = Imp()->CurrPrevwSet().aPreviewDocSize;

    if ( _bCalcScale )
    {
        // calculate scaling
        SwTwips nInitLayoutWidth = _nCols * aMaxPageSize.Width() +
                                   (_nCols+1) * nXFree;
        SwTwips nInitLayoutHeight = _nRows * aMaxPageSize.Height() +
                                    (_nRows+1) * nYFree;
        MapMode aMapMode( MAP_TWIP );
        Size aWinSize = GetOut()->PixelToLogic( *(_pPxWinSize), aMapMode );
        Fraction aXScale( aWinSize.Width(), nInitLayoutWidth );
        Fraction aYScale( aWinSize.Height(), nInitLayoutHeight );
        if( aXScale < aYScale )
            aYScale = aXScale;
        {
            // adjust scaling for Drawing layer.
            aYScale *= Fraction( 1000, 1 );
            long nNewNuminator = aYScale.operator long();
            if( nNewNuminator < 1 )
                nNewNuminator = 1;
            aYScale = Fraction( nNewNuminator, 1000 );
            // propagate scaling as zoom percentation to view option for font cache
            if( pOpt )
                pOpt->SetZoom( USHORT(nNewNuminator / 10));
        }
        aMapMode.SetScaleY( aYScale );
        aMapMode.SetScaleX( aYScale );
        // set created mapping mode with calculated scaling at output device.
        GetOut()->SetMapMode( aMapMode );
    }

    // set window size in twips at preview settings
    Imp()->CurrPrevwSet().aWinSize =
                    GetOut()->PixelToLogic( *(_pPxWinSize) );
    // validate layout sizes
    Imp()->CurrPrevwSet().bLayoutSizesValid = true;

    return true;
}

/** prepare paint of page preview

    With the valid preview layout settings - calculated and set by method
    <InitPreviewLayout> - the paint of a specific part of the virtual
    preview document is prepared. The corresponding part is given by either
    a start page (parameter <_nProposedStartPageNum>) or a absolute position
    (parameter <_aProposedStartPoint>).
    The accessibility preview will also be updated via a corresponding
    method call.

    @author OD

    @param _nProposedStartPageNum [0..<number of document pages>]
    input parameter - proposed number of page, which should be painted in
    the left-top-corner in the current output device.

    @param _nProposedStartPos [(0,0)..<PreviewDocumentSize>]
    input parameter - proposed absolute position in the virtual preview
    document, which should be painted in the left-top-corner in the current
    output device.

    @param _onStartPageNum
    output parameter - physical number of page, which will be painted in the
    left-top-corner in the current output device.

    @param _onStartPageVirtNum
    output parameter - virtual number of page, which will be painted in the
    left-top-corner in the current output device.

    @param _orDocPreviewPaintRect
    output parameter - rectangle of preview document, which will be painted.

    @return boolean, indicating, if prepare of preview paint was successful.
*/
extern void lcl_CalcAdditionalPaintOffset( CurrentPreviewSettings& _rPrevwSettings );
extern void lcl_CalcDocPrevwPaintRect( const CurrentPreviewSettings& _rPrevwSettings,
                                       Rectangle& _orDocPrevwPaintRect );

bool ViewShell::PreparePreviewPaint( const sal_uInt16 _nProposedStartPageNum,
                                     const Point      _aProposedStartPos,
                                     const Size*      _pPxWinSize,
                                     sal_uInt16&      _onStartPageNum,
                                     sal_uInt16&      _onStartPageVirtNum,
                                     Rectangle&       _orDocPreviewPaintRect
                                   )
{
    sal_uInt16 nProposedStartPageNum = _nProposedStartPageNum;
    const SwRootFrm* pRootFrm = GetLayout();
    // check environment and parameters
    {
        ASSERT( pRootFrm, "No layout - no prepare of preview paint");
        if ( !pRootFrm )
            return false;

        bool bLayoutSettingsValid = Imp()->CurrPrevwSet().bLayoutInfoValid &&
                                    Imp()->CurrPrevwSet().bLayoutSizesValid;
        ASSERT( bLayoutSettingsValid,
                "no valid preview layout info/sizes - no prepare of preview paint");
        if ( !bLayoutSettingsValid )
            return false;

        bool bStartPageRangeValid =
                _nProposedStartPageNum <= Imp()->CurrPrevwSet().nPages;
        ASSERT( bStartPageRangeValid,
                "proposed start page not existing - no prepare of preview paint");
        if ( !bStartPageRangeValid )
            return false;

        bool bStartPosRangeValid =
                _aProposedStartPos.X() <= Imp()->CurrPrevwSet().aPreviewDocSize.Width() &&
                _aProposedStartPos.Y() <= Imp()->CurrPrevwSet().aPreviewDocSize.Height();
        ASSERT( bStartPosRangeValid,
                "proposed start position out of range - no prepare of preview paint");
        if ( !bStartPosRangeValid )
            return false;

        bool bWinSizeValid = _pPxWinSize->Width() != 0 && _pPxWinSize->Height() != 0;
        ASSERT ( bWinSizeValid, "no window size - no prepare of preview paint");
        if ( !bWinSizeValid )
            return false;

        bool bStartInfoValid = _nProposedStartPageNum > 0 ||
                               _aProposedStartPos != Point(0,0);
        if ( !bStartInfoValid )
            nProposedStartPageNum = 1;
    }

    // environment and parameter ok

    // update window size at preview setting data
    Imp()->CurrPrevwSet().aWinSize = GetOut()->PixelToLogic( *(_pPxWinSize) );

    // get preview setting data
    sal_uInt16 nCols = Imp()->CurrPrevwSet().nCols;
    sal_uInt16 nRows = Imp()->CurrPrevwSet().nRows;
    sal_uInt16 nPages = Imp()->CurrPrevwSet().nPages;
    sal_uInt16 nColWidth = static_cast<sal_uInt16>(Imp()->CurrPrevwSet().nColWidth);
    sal_uInt16 nRowHeight = static_cast<sal_uInt16>(Imp()->CurrPrevwSet().nRowHeight);

    sal_uInt16 nStartPageNum, nColOfProposed, nStartCol, nRowOfProposed;
    Point aStartPageOffset;
    Point aPreviewDocOffset;
    if ( nProposedStartPageNum > 0 )
    {
        // determine column and row of proposed start page in virtual preview layout
        sal_uInt16 nTmpCol = (nProposedStartPageNum+1) % nCols;
        nColOfProposed = nTmpCol ? nTmpCol : nCols;
        sal_uInt16 nTmpRow = (nProposedStartPageNum+1) / nCols;
        nRowOfProposed = nTmpCol ? nTmpRow+1 : nTmpRow;
        // determine start page == first page in the row of proposed start page
        if ( nProposedStartPageNum == 1 || nRowOfProposed == 1 )
            nStartPageNum = 1;
        else
            nStartPageNum = nProposedStartPageNum - (nColOfProposed-1);
        // set starting column
        nStartCol = 1;
        // page offset == (-1,-1), indicating no offset and paint of free space.
        aStartPageOffset.X() = -1;
        aStartPageOffset.Y() = -1;
        // virtual preview document offset.
        aPreviewDocOffset.X() = 0;
        if ( nRowOfProposed == 1)
            aPreviewDocOffset.Y() = 0;
        else
            aPreviewDocOffset.Y() = (nRowOfProposed-1) * nRowHeight + 1;
    }
    else
    {
        // determine column and row of proposed start position
        if ( _aProposedStartPos.X() == 0 )
            nColOfProposed = 1;
        else
        {
            sal_uInt16 nTmpCol = _aProposedStartPos.X() / nColWidth;
            nColOfProposed =
                    (_aProposedStartPos.X() % nColWidth) ? nTmpCol+1 : nTmpCol;
        }
        if ( _aProposedStartPos.Y() == 0 )
            nRowOfProposed = 1;
        else
        {
            sal_uInt16 nTmpRow = _aProposedStartPos.Y() / nRowHeight;
            nRowOfProposed =
                    (_aProposedStartPos.Y() % nRowHeight) ? nTmpRow+1 : nTmpRow;
        }
        // determine start page == page at proposed start position
        if ( nRowOfProposed == 1 && nColOfProposed == 1 )
            nStartPageNum = 1;
        else
            nStartPageNum = (nRowOfProposed-1) * nCols + (nColOfProposed-1);
        // set starting column
        nStartCol = nColOfProposed;
// NOTE: <nStartPageNum> can be greater than <nPages> - consider case later
        // page offset
        SwTwips nTmpXOffset = (_aProposedStartPos.X() % nColWidth) - nXFree;
        SwTwips nTmpYOffset = (_aProposedStartPos.Y() % nRowHeight) - nYFree;
        if ( nTmpXOffset < 0 && nTmpYOffset < 0 )
            aStartPageOffset = Point( -1,-1 );
        else
        {
            aStartPageOffset.X() = nTmpXOffset < 0 ? 0 : nTmpXOffset;
            aStartPageOffset.Y() = nTmpYOffset < 0 ? 0 : nTmpYOffset;
        }
        // virtual preview document offset.
        aPreviewDocOffset = _aProposedStartPos;
    }

    // set paint data at preview settings
    Imp()->CurrPrevwSet().nPaintPhyStartPageNum = nStartPageNum;
    Imp()->CurrPrevwSet().nPaintStartCol = nStartCol;
    Imp()->CurrPrevwSet().nPaintStartRow = nRowOfProposed;
    Imp()->CurrPrevwSet().aPaintStartPageOffset = aStartPageOffset;
    Imp()->CurrPrevwSet().aPaintPreviewDocOffset = aPreviewDocOffset;
    // validate paint date
    Imp()->CurrPrevwSet().bPaintInfoValid = true;

    // return start page
    _onStartPageNum = nStartPageNum;
    // return virtual page number of start page
    _onStartPageVirtNum = 0;
    if ( nStartPageNum <= nPages )
    {
        const SwPageFrm* pPage = static_cast<const SwPageFrm*>( pRootFrm->Lower() );
        while ( pPage && pPage->GetPhyPageNum() < nStartPageNum )
        {
            pPage = static_cast<const SwPageFrm*>( pPage->GetNext() );
        }
        if ( pPage )
            _onStartPageVirtNum = pPage->GetVirtPageNum();
    }

    // determine additional paint offset, if preview layout fits into window.
    lcl_CalcAdditionalPaintOffset( Imp()->CurrPrevwSet() );

    // determine rectangle to be painted from document preview
    lcl_CalcDocPrevwPaintRect( Imp()->CurrPrevwSet(), _orDocPreviewPaintRect );

    return true;
};

void lcl_CalcAdditionalPaintOffset( CurrentPreviewSettings& _rPrevwSettings )
{
    Size aWinSize = _rPrevwSettings.aWinSize;

    SwTwips nPrevwLayoutHeight =
            _rPrevwSettings.nRows * _rPrevwSettings.nRowHeight + nYFree;
    if ( nPrevwLayoutHeight <= aWinSize.Height() &&
         _rPrevwSettings.aPaintStartPageOffset.Y() <= 0 )
    {
        _rPrevwSettings.bDoesLayoutRowsFitIntoWindow = true;
        _rPrevwSettings.aAdditionalPaintOffset.Y() =
            (aWinSize.Height() - nPrevwLayoutHeight) / 2;

    }
    else
    {
        _rPrevwSettings.bDoesLayoutRowsFitIntoWindow = false;
        _rPrevwSettings.aAdditionalPaintOffset.Y() = 0;
    }

    SwTwips nPrevwLayoutWidth =
            _rPrevwSettings.nCols * _rPrevwSettings.nColWidth + nXFree;
    if ( nPrevwLayoutWidth <= aWinSize.Width() &&
         _rPrevwSettings.aPaintStartPageOffset.X() <= 0 )
        _rPrevwSettings.aAdditionalPaintOffset.X() =
                (aWinSize.Width() - nPrevwLayoutWidth) / 2;
    else
        _rPrevwSettings.aAdditionalPaintOffset.X() = 0;

}

void lcl_CalcDocPrevwPaintRect( const CurrentPreviewSettings& _rPrevwSettings,
                                Rectangle& _orDocPrevwPaintRect )
{
    Point aTopLeftPos = _rPrevwSettings.aPaintPreviewDocOffset;
    _orDocPrevwPaintRect.SetPos( aTopLeftPos );

    Size aSize;
    aSize.Width() = Min( _rPrevwSettings.aPreviewDocSize.Width() -
                            aTopLeftPos.X(),
                        _rPrevwSettings.aWinSize.Width() -
                            _rPrevwSettings.aAdditionalPaintOffset.X() );
    if ( _rPrevwSettings.bDoesLayoutRowsFitIntoWindow )
        aSize.Height() = _rPrevwSettings.nRows * _rPrevwSettings.nRowHeight + nYFree;
    else
        aSize.Height() = Min( _rPrevwSettings.aPreviewDocSize.Height() -
                                aTopLeftPos.Y(),
                              _rPrevwSettings.aWinSize.Height() -
                                _rPrevwSettings.aAdditionalPaintOffset.Y() );
    _orDocPrevwPaintRect.SetSize( aSize );
}

/** property <DoesPreviewLayoutRowsFitIntoWin> of current preview layout

    OD 03.12.2002 #103492#

    @author OD

    @return boolean, indicating that the rows of the current preview layout
    fit into the current window size.
*/
bool ViewShell::DoesPreviewLayoutRowsFitIntoWindow()
{
    return Imp()->CurrPrevwSet().bDoesLayoutRowsFitIntoWindow;
}

/** helper method for <ViewShell::PaintPreview(..)>

    OD 03.12.2002 #103492#

    @author OD
*/
void lcl_CalcPreviewBackgrd( const Rectangle  _aOutRect,
                             const sal_uInt16 _nStartCol,
                             const sal_uInt16 _nCols,
                             const sal_uInt16 _nRows,
                             const bool       _bDoesLayoutFits,
                             const SwTwips    _nColWidth,
                             const SwTwips    _nRowHeight,
                             const Size       _aMaxPageSize,
                             const SwPageFrm* _pStartPage,
                             const Point&     _rPaintOffset,
                             SwRegionRects&   _orPreviewBackgrdRegion )
{
    const SwPageFrm* pPage = _pStartPage;
    sal_uInt16 nCurrCol = _nStartCol;
    sal_uInt16 nConsideredRows = 0;
    Point aCurrPaintOffset = _rPaintOffset;
    // loop on pages to determine preview background retangles
    while ( pPage &&
            aCurrPaintOffset.X() < _aOutRect.Right() &&
            aCurrPaintOffset.Y() < _aOutRect.Bottom() &&
            (!_bDoesLayoutFits || nConsideredRows < _nRows) )
    {
        pPage->Calc();  // don't know, if necessary.

        // consider only pages, which have to be painted.
        if ( aCurrPaintOffset.X() < _aOutRect.Right() ||
             nCurrCol < _nStartCol )
        {
            if ( pPage->GetPhyPageNum() == 1 && _nCols != 1 && nCurrCol == 1)
            {
                // first page always in 2nd column
                // --> prepare date for next and start next loop
                aCurrPaintOffset.X() += _nColWidth;
                ++nCurrCol;
                continue;
            }

            Size aPageSize;
            if ( pPage->IsEmptyPage() )
            {
                if ( pPage->GetPhyPageNum() % 2 == 0 )
                    aPageSize = pPage->GetPrev()->Frm().SSize();
                else
                    aPageSize = pPage->GetNext()->Frm().SSize();
            }
            else
                aPageSize = pPage->Frm().SSize();

            Point aOffset( aCurrPaintOffset );
            if ( aPageSize.Width() < _aMaxPageSize.Width() )
                aOffset.X() += ( _aMaxPageSize.Width() - aPageSize.Width() ) / 2;
            if ( aPageSize.Height() < _aMaxPageSize.Height() )
                aOffset.Y() += ( _aMaxPageSize.Height() - aPageSize.Height() ) / 2;
            SwRect aPageRect( aOffset, aPageSize );
            _orPreviewBackgrdRegion -= aPageRect;
        }

        // prepare data for next loop
        pPage = static_cast<const SwPageFrm*>(pPage->GetNext());
        aCurrPaintOffset.X() += _nColWidth;
        ++nCurrCol;
        if ( nCurrCol > _nCols )
        {
            ++nConsideredRows;
            aCurrPaintOffset.X() = _rPaintOffset.X();
            nCurrCol = 1;
            aCurrPaintOffset.Y() += _nRowHeight;
        }
    }
}

/** paint prepared preview

    OD 28.11.2002 #103492#

    @author OD

    @param _nSelectedPageNum
    input parameter - physical number of page, which should be painted as
    selected by am extra border in color COL_LIGHTBLUE.

    @param _aOutRect
    input parameter - Twip rectangle of window, which should be painted.

    @return boolean, indicating, if paint of preview was performed
*/
bool ViewShell::PaintPreview( const sal_uInt16 _nSelectedPageNum,
                              const Rectangle  _aOutRect )
{
    const SwRootFrm* pRootFrm = GetLayout();
    // check environment and parameters
    {
        ASSERT( pRootFrm, "No layout - no paint of preview" );
        if ( !pRootFrm )
            return false;

        ASSERT( Imp()->CurrPrevwSet().bPaintInfoValid,
                "invalid preview settings - no paint of preview" );
        if ( !Imp()->CurrPrevwSet().bPaintInfoValid )
            return false;
    }

    // environment and parameter ok

    if ( !GetWin() && !GetOut()->GetConnectMetaFile() )
        return false;;

    SET_CURR_SHELL( this );

    // determine start page frame
    sal_uInt16 nStartPageNum = Imp()->CurrPrevwSet().nPaintPhyStartPageNum;
    const SwPageFrm* pStartPage = static_cast<const SwPageFrm*>( pRootFrm->Lower() );
    while ( pStartPage && pStartPage->GetPhyPageNum() < nStartPageNum )
    {
        pStartPage = static_cast<const SwPageFrm*>( pStartPage->GetNext() );
    }
    // prepare paint
    if ( pStartPage )
    {
        Imp()->bFirstPageInvalid = FALSE;
        Imp()->pFirstVisPage = const_cast<SwPageFrm*>(pStartPage);
    }
    // get necessary preview layout data
    sal_uInt16 nCols = Imp()->CurrPrevwSet().nCols;
    sal_uInt16 nRows = Imp()->CurrPrevwSet().nRows;
    bool bDoesLayoutFits = Imp()->CurrPrevwSet().bDoesLayoutRowsFitIntoWindow;
    SwTwips nColWidth = Imp()->CurrPrevwSet().nColWidth;
    SwTwips nRowHeight = Imp()->CurrPrevwSet().nRowHeight;
    Size aMaxPageSize = Imp()->CurrPrevwSet().aMaxPageSize;

    // calculate initial paint offset
    Point aInitialPaintOffset;
    if ( Imp()->CurrPrevwSet().aPaintStartPageOffset != Point( -1, -1 ) )
    {
        Point aPageOffset( Imp()->CurrPrevwSet().aPaintStartPageOffset );
        if ( aPageOffset.X() > 0 )
            aInitialPaintOffset.X() = -aPageOffset.X();
        else
            aInitialPaintOffset.X() = nXFree;
        if ( aPageOffset.Y() > 0 )
            aInitialPaintOffset.Y() = -aPageOffset.Y();
        else
            aInitialPaintOffset.Y() = nYFree;
    }
    else
        aInitialPaintOffset = Point( nXFree, nYFree );
    aInitialPaintOffset += Imp()->CurrPrevwSet().aAdditionalPaintOffset;

    // init data structure for preview background rectangles
    SwRegionRects aPreviewBackgrdRegion( _aOutRect );
    // calculate preview background rectangles
    lcl_CalcPreviewBackgrd( _aOutRect,
                            Imp()->CurrPrevwSet().nPaintStartCol,
                            nCols, nRows, bDoesLayoutFits,
                            nColWidth, nRowHeight, aMaxPageSize,
                            pStartPage, aInitialPaintOffset,
                            aPreviewBackgrdRegion );
    // paint preview background rectangles
    _PaintDesktop( aPreviewBackgrdRegion );

    // prepare data for paint of pages
    const Rectangle aPxOutRect( GetOut()->LogicToPixel( _aOutRect ) );
    MapMode aMapMode( GetOut()->GetMapMode() );
    MapMode aSavedMapMode = aMapMode;

    Font* pEmptyPgFont = 0;

    Color aSelPgLineColor(COL_LIGHTBLUE);
    const StyleSettings& rSettings = GetWin()->GetSettings().GetStyleSettings();
    if ( rSettings.GetHighContrastMode() )
        aSelPgLineColor = rSettings.GetHighlightTextColor();

    Color aEmptyPgShadowBorderColor = rSettings.GetFieldTextColor();

    // prepare loop data
    const SwPageFrm* pPage = pStartPage;
    sal_uInt16 nCurrCol = Imp()->CurrPrevwSet().nPaintStartCol;
    sal_uInt16 nPaintedRows = 0;
    Point aCurrPaintOffset = aInitialPaintOffset;
    // loop on pages to determine preview background retangles
    while ( pPage &&
            aCurrPaintOffset.X() < _aOutRect.Right() &&
            aCurrPaintOffset.Y() < _aOutRect.Bottom() &&
            ( !bDoesLayoutFits || nPaintedRows < nRows )
          )
    {
        // consider only pages, which have to be painted.
        if ( aCurrPaintOffset.X() < _aOutRect.Right() ||
             nCurrCol < Imp()->CurrPrevwSet().nPaintStartCol
           )
        {
            if ( pPage->GetPhyPageNum() == 1 && nCols != 1 && nCurrCol == 1)
            {
                // first page always in 2nd column
                // --> prepare date for next and start next loop
                aCurrPaintOffset.X() += nColWidth;
                ++nCurrCol;
                continue;
            }

            Rectangle aPxPageRect;
            if ( pPage->IsEmptyPage() )
            {
                // determine empty page rectangle
                Size aPageSize;
                if ( pPage->GetPhyPageNum() % 2 == 0 )
                    aPageSize = pPage->GetPrev()->Frm().SSize();
                else
                    aPageSize = pPage->GetNext()->Frm().SSize();
                Point aOffset( aCurrPaintOffset );
                if ( aPageSize.Width() < aMaxPageSize.Width() )
                    aOffset.X() += (aMaxPageSize.Width() - aPageSize.Width()) / 2;
                if ( aPageSize.Height() < aMaxPageSize.Height() )
                    aOffset.Y() += (aMaxPageSize.Height() - aPageSize.Height()) / 2;
                Rectangle aPageRect( aOffset, aPageSize );
                // paint empty page rectangle
                aMapMode.SetOrigin( Point(0,0) );
                GetOut()->SetMapMode( aMapMode );
                const Color aRetouche( Imp()->GetRetoucheColor() );
                if( GetOut()->GetFillColor() != aRetouche )
                    GetOut()->SetFillColor( aRetouche );
                GetOut()->DrawRect( aPageRect );
                // paint empty page text
                if( !pEmptyPgFont )
                {
                    pEmptyPgFont = new Font;
                    pEmptyPgFont->SetSize( Size( 0, 80 * 20 )); // == 80 pt
                    pEmptyPgFont->SetWeight( WEIGHT_BOLD );
                    pEmptyPgFont->SetStyleName( aEmptyStr );
                    pEmptyPgFont->SetName( String::CreateFromAscii(
                            RTL_CONSTASCII_STRINGPARAM( "Helvetica" )) );
                    pEmptyPgFont->SetFamily( FAMILY_SWISS );
                    pEmptyPgFont->SetTransparent( TRUE );
                    pEmptyPgFont->SetColor( COL_GRAY );
                }
                Font aOldFont( GetOut()->GetFont() );
                GetOut()->SetFont( *pEmptyPgFont );
                GetOut()->DrawText( aPageRect, SW_RESSTR( STR_EMPTYPAGE ),
                                    TEXT_DRAW_VCENTER |
                                    TEXT_DRAW_CENTER |
                                    TEXT_DRAW_CLIP );
                GetOut()->SetFont( aOldFont );
                // paint shadow and border for empty page
                aPxPageRect = GetOut()->LogicToPixel( aPageRect );
                Color aFill( GetOut()->GetFillColor() );
                Color aLine( GetOut()->GetLineColor() );
                GetOut()->SetFillColor( aEmptyPgShadowBorderColor );
                GetOut()->SetLineColor( aEmptyPgShadowBorderColor );
                // bottom shadow
                Rectangle aRect( aPxPageRect.Left()+2, aPxPageRect.Bottom()+1,
                                   aPxPageRect.Right()+2, aPxPageRect.Bottom()+2 );
                aRect = GetOut()->PixelToLogic( aRect );
                GetOut()->DrawRect( aRect );
                // right shadow
                aRect.Left() = aPxPageRect.Right() + 1;
                aRect.Right() = aRect.Left() + 1;
                aRect.Top() = aPxPageRect.Top() + 2;
                aRect.Bottom() = aPxPageRect.Bottom();
                aRect = GetOut()->PixelToLogic( aRect );
                GetOut()->DrawRect( aRect );
                // border
                aRect = GetOut()->PixelToLogic( aPxPageRect );
                GetOut()->SetFillColor( Color( COL_TRANSPARENT ) );
                GetOut()->DrawRect( aRect );
                GetOut()->SetFillColor( aFill );
                GetOut()->SetLineColor( aLine );
            }
            else
            {
                Point aMapOffset( aCurrPaintOffset );
                if ( pPage->Frm().Width() < aMaxPageSize.Width() )
                    aMapOffset.X() += (aMaxPageSize.Width() - pPage->Frm().Width()) / 2;
                if ( pPage->Frm().Height() < aMaxPageSize.Height() )
                    aMapOffset.Y() += (aMaxPageSize.Height() - pPage->Frm().Height()) / 2;
                aMapOffset -= pPage->Frm().Pos();
                aMapMode.SetOrigin( aMapOffset );
                GetOut()->SetMapMode( aMapMode );

                aVisArea = pPage->Frm();
                Rectangle aPxPaintRect( GetOut()->LogicToPixel( aVisArea.SVRect() ) );
                aPxPageRect = aPxPaintRect;
                if ( aPxOutRect.IsOver( aPxPaintRect) )
                {
                    aPxPaintRect.Intersection( aPxOutRect );
                    Rectangle aPaintRect = GetOut()->PixelToLogic( aPxPaintRect );
                    Paint( aPaintRect );
                }
            }
            if ( pPage->GetPhyPageNum() == _nSelectedPageNum )
            {
                Color aFill( GetOut()->GetFillColor() );
                Color aLine( GetOut()->GetLineColor() );
                Rectangle aRect( aPxPageRect.Left()+1, aPxPageRect.Top()+1,
                                   aPxPageRect.Right()-1, aPxPageRect.Bottom()-1 );
                aRect = GetOut()->PixelToLogic( aRect );
                GetOut()->SetFillColor( Color( COL_TRANSPARENT ) );
                GetOut()->SetLineColor( aSelPgLineColor );
                GetOut()->DrawRect( aRect );
                aRect = Rectangle( aPxPageRect.Left()+2, aPxPageRect.Top()+2,
                                   aPxPageRect.Right()-2, aPxPageRect.Bottom()-2 );
                aRect = GetOut()->PixelToLogic( aRect );
                GetOut()->DrawRect( aRect );
                GetOut()->SetFillColor( aFill );
                GetOut()->SetLineColor( aLine );
            }
        }

        // prepare data for next loop
        pPage = static_cast<const SwPageFrm*>(pPage->GetNext());
        aCurrPaintOffset.X() += nColWidth;
        ++nCurrCol;
        if ( nCurrCol > nCols )
        {
            ++nPaintedRows;
            aCurrPaintOffset.X() = aInitialPaintOffset.X();
            nCurrCol = 1;
            aCurrPaintOffset.Y() += nRowHeight;
        }
    }

    delete pEmptyPgFont;
    GetOut()->SetMapMode( aSavedMapMode );
    aVisArea.Clear();

    return true;
}

void ViewShell::RepaintCoreRect(
        const SwRect& rRect, 		// Paint-Rect der Core
        USHORT nRowCol,				// Anzahl Zeilen/Spalten
        USHORT nSttPage,			// Start ab dieser Seite, eine gueltige ??
        const Size& rPageSize )		// MaxSize einer Seite
{
    if( !GetWin() && !GetOut()->GetConnectMetaFile() )
        return;						//Fuer den Drucker tun wir hier nix

    SET_CURR_SHELL( this );

    const SwRootFrm* pRoot = GetLayout();
    ASSERT( pRoot, "Wo ist mein Layout?" );

    BYTE nRow = BYTE(nRowCol >> 8), nCol = BYTE(nRowCol & 0xff);
    SwPageFrm* pSttPage = ::lcl_GetSttPage( nRow, nCol, nSttPage, pRoot );

    // damit auch die Seiten im Paint angezeigt werden, gleich die richtigen
    // Werte setzen! (siehe auch Bug 24575)
    Imp()->bFirstPageInvalid = FALSE;
    Imp()->pFirstVisPage = pSttPage;

//    const Rectangle aPixRect( GetOut()->LogicToPixel( rRect ) );

    MapMode aMapMode( GetOut()->GetMapMode() );
    MapMode aCalcMapMode( aMapMode );
    aCalcMapMode.SetOrigin( Point() );

    Point aFreePt( nPrevViewXFreePix, nPrevViewYFreePix );
    aFreePt = GetOut()->PixelToLogic( aFreePt, aCalcMapMode );
    Point aCalcPt( aFreePt );
    long nPageHeight = aFreePt.Y() + rPageSize.Height()+1;
    SwPageFrm* pPage = pSttPage;

    // Sonderbehandlung fuer die erste Seite
    // JP 19.08.98: aber nur wenn sie mehrspaltig angezeigt wird
    int bFirstPg = !nSttPage && 1 != nCol;
    for( BYTE nCntRow = 0; pPage && nCntRow < nRow; ++nCntRow )
    {
        aCalcPt.X() = aFreePt.X();
        for( BYTE nCntCol = 0; pPage && nCntCol < nCol; ++nCntCol )
        {
            // Sonderbehandlung fuer 1. Seite, ist immer eine rechte
            if( bFirstPg )
            {
                bFirstPg = FALSE;
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
                continue;
            }

            SwRect aTmpRect( pPage->Frm() );
            if( rRect.IsOver( aTmpRect ))
            {
                Point aPos( aCalcPt );
                aPos -= aTmpRect.Pos();

                aMapMode.SetOrigin( aPos );

                aTmpRect._Intersection( rRect );
                Rectangle aSVRect( GetOut()->
                    LogicToLogic( aTmpRect.SVRect(), &aMapMode, &aCalcMapMode ) );
                GetWin()->Invalidate( aSVRect );
            }
            aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        aCalcPt.Y() += nPageHeight;
    }

    aVisArea.Clear();		// verhinder internes Painten !!
}

    // und jetzt mal raus auf den Drucker
void ViewShell::PrintPreViewPage( SwPrtOptions& rOptions,
                                  USHORT nRowCol, SfxProgress& rProgress,
                                  const SwPagePreViewPrtData* pPrtData )
{
    if( !rOptions.aMulti.GetSelectCount() )
        return;

    // wenn kein Drucker vorhanden ist, wird nicht gedruckt
    SfxPrinter* pPrt = GetPrt();
    if(	!pPrt || !pPrt->GetName().Len() )
        return;

// schoen waers gewesen !!!	const MultiSelection& rMulti = rOptions.aMulti;
    MultiSelection aMulti( rOptions.aMulti );
    Range aPages( aMulti.FirstSelected(), aMulti.LastSelected() );
    if ( aPages.Max() > USHRT_MAX )
        aPages.Max() = USHRT_MAX;

    ASSERT( aPages.Min() > 0,
            "Seite 0 Drucken?" );
    ASSERT( aPages.Min() <= aPages.Max(),
            "MinSeite groesser MaxSeite." );

    // eine neue Shell fuer den Printer erzeugen
    ViewShell aShell( *this, 0 );

    aShell.pRef = new SfxPrinter( *pPrt );

    SET_CURR_SHELL( &aShell );

    aShell.PrepareForPrint( rOptions );

    // gibt es versteckte Absatzfelder, unnoetig wenn die Absaetze bereits
    // ausgeblendet sind.
    int bHiddenFlds = FALSE;
    SwFieldType* pFldType = 0;
    if ( GetViewOptions()->IsShowHiddenPara() )
    {
        pFldType = GetDoc()->GetSysFldType( RES_HIDDENPARAFLD );
        bHiddenFlds = 0 != pFldType->GetDepends();
        if( bHiddenFlds )
        {
            SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
            pFldType->Modify( &aHnt, 0);
        }
    }

    // Seiten fuers Drucken formatieren
    aShell.CalcPagesForPrint( (USHORT)aPages.Max(), &rProgress );

    USHORT nCopyCnt = rOptions.bCollate ? rOptions.nCopyCount : 1;
    BOOL bStartJob = FALSE;

    for ( USHORT nCnt = 0; nCnt < nCopyCnt; nCnt++ )
    {
        if( rOptions.IsPrintSingleJobs() && rOptions.GetJobName().Len() &&
            ( bStartJob || rOptions.bJobStartet ) )
        {
            pPrt->EndJob();
            rOptions.bJobStartet = TRUE;

            // Reschedule statt Yield, da Yield keine Events abarbeitet
            // und es sonst eine Endlosschleife gibt.
            while( pPrt->IsPrinting() )
                    rProgress.Reschedule();

            rOptions.MakeNextJobName();
            bStartJob = pPrt->StartJob( rOptions.GetJobName() );
        }

        const SwPageFrm *pStPage  = (SwPageFrm*)GetLayout()->Lower();
        const SwFrm		*pEndPage = pStPage;

        for( USHORT i = 1; pStPage && i < (USHORT)aPages.Min(); ++i )
            pStPage = (SwPageFrm*)pStPage->GetNext();

        if( !pStPage )			// dann wars das
        {
            if( bHiddenFlds )
            {
                SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
                pFldType->Modify( &aHnt, 0);
                CalcPagesForPrint( (USHORT)aPages.Max() );
            }
            return;
        }

        // unsere SttPage haben wir, dann die EndPage suchen
        USHORT nFirstPageNo = i;
        USHORT nLastPageNo  = i;
        USHORT nPageNo      = 1;

        pEndPage = pStPage;

        if( pStPage->GetNext() && (i != (USHORT)aPages.Max()) )
        {
            pEndPage = pEndPage->GetNext();
            for( ++i,++nLastPageNo;
                 pEndPage->GetNext() && i < (USHORT)aPages.Max(); ++i )
            {
                pEndPage = pEndPage->GetNext();
                ++nLastPageNo;
            }
        }

        if( rOptions.bPrintReverse )
        {
            const SwFrm *pTmp = pStPage;
            pStPage  = (SwPageFrm*)pEndPage;
            pEndPage = pTmp;
            nPageNo  = nLastPageNo;
        }
        else
            nPageNo = nFirstPageNo;


    /*-----------------01.03.95 13:06-------------------

    // Diese Options nur per Dialog einstellbar

        if( !rOptions.bPaperFromSetup )		// Schacht einstellen.
            pPrt->SetPaperBin( pStPage->GetFmt()->GetPaperBin().
                                    GetBin() );

        // Orientation einstellen: Breiter als Hoch -> Landscape,
        // sonst -> Portrait.
        if( pStPage->GetPageDesc()->GetLandscape() )
            pPrt->SetOrientation( ORIENTATION_LANDSCAPE );
        else
            pPrt->SetOrientation( ORIENTATION_PORTRAIT );

    --------------------------------------------------*/

        // ein Array fuer die Seiten anlegen, die auf eine Drucker-Seite
        // gedruckt werden sollen.
        BYTE nRow = BYTE(nRowCol >> 8), nCol = BYTE(nRowCol & 0xff);
        USHORT nPages = nRow * nCol;
        SwPageFrm** aPageArr = new SwPageFrm* [ nPages ];
        memset( aPageArr, 0, sizeof( SwPageFrm* ) * nPages );
        USHORT nCntPage = 0;

        SwTwips nCalcW = 0, nCalcH = 0, nMaxRowSz = 0, nMaxColSz = 0;

        MapMode aOld( pPrt->GetMapMode() );
        MapMode aMapMode( MAP_TWIP );
        Size aPrtSize( pPrt->PixelToLogic( pPrt->GetPaperSizePixel(), aMapMode ));
        if( pPrtData )
        {
            aPrtSize.Width() -= pPrtData->GetLeftSpace() +
                                pPrtData->GetRightSpace() +
                                ( pPrtData->GetHorzSpace() * (nCol - 1));
            aPrtSize.Height() -= pPrtData->GetTopSpace() +
                                pPrtData->GetBottomSpace() +
                                ( pPrtData->GetVertSpace() * (nRow - 1));
        }

        aMulti.Select( Range( nLastPageNo+1, USHRT_MAX ), FALSE );
        USHORT nSelCount = USHORT((aMulti.GetSelectCount()+nPages-1) / nPages);
        nSelCount *= nCopyCnt;
        USHORT nPrintCount = 1;

        const XubString aTmp( SW_RES( STR_STATSTR_PRINT ) );
        rProgress.SetText( aTmp );
        //HACK, damit die Anzeige nicht durcheinander kommt:
        rProgress.SetState( 1, nSelCount );
        rProgress.SetText( aTmp );
        bStartJob = TRUE;

        while( pStPage )
        {
            // Mag der Anwender noch ?
            rProgress.Reschedule();

            if ( !pPrt->IsJobActive() )
                break;

            if( aMulti.IsSelected( nPageNo ) )
            {
                if( rOptions.bPrintReverse )
                    aPageArr[ nPages - ++nCntPage ] = (SwPageFrm*)pStPage;
                else
                    aPageArr[ nCntPage++ ] = (SwPageFrm*)pStPage;

                const Size& rSz = pStPage->Frm().SSize();
                nCalcW += rSz.Width();
                if( nCalcH < rSz.Height() )
                    nCalcH = rSz.Height();

                if( 0 == (nCntPage % nCol ) || 			// neue Zeile
                    nCntPage == nPages || pStPage == pEndPage )
                {
                    // sollte die Seite nicht gefuellt sein, so erweiter
                    // anhand der letzen Seite. Dadurch hat auch die
                    // letze Seite die richtigen Spalten/Reihen.
                    // BUG: 17695
                    if( pStPage == pEndPage && nCntPage != nPages )
                    {
                        // dann Werte mit der letzen Seite auffuellen
                        if( nCntPage < nCol )
                            nCalcW += rSz.Width() * (nCol - nCntPage);

                        BYTE nRows = (BYTE) ( nCntPage / nCol + 1 );
                        if( nRows < nRow )
                            nCalcH += ( nRow - nRows ) * nCalcH;
                    }

                    if( nMaxColSz < nCalcW )
                        nMaxColSz = nCalcW;
                    nCalcW = 0;
                    nMaxRowSz += nCalcH;
                }

                if( nCntPage == nPages || pStPage == pEndPage )
                {
                    // den MapMode einstellen
                    aMapMode.SetOrigin( Point() );
                    {
                        Fraction aScX( aPrtSize.Width(), nMaxColSz );
                        Fraction aScY( aPrtSize.Height(), nMaxRowSz );

// JP 19.08.98: wird zur Zeit nicht ausgewertet, weil
//				der Font sich nicht enstprechend
//				stretch laesst.
#if 0
                        if( pPrtData && pPrtData->GetStretch() )
                        {
                            // fuer Drawing, damit diese ihre Objecte vernuenftig Painten
                            // koennen, auf "glatte" Prozentwerte setzen
                            long nTmp = (long)(aScY *= Fraction( 1000, 1 ));
                            if( 1 < nTmp ) --nTmp; else nTmp = 1;
                            aScY = Fraction( nTmp, 1000 );

                            nTmp = (long)(aScX *= Fraction( 1000, 1 ));
                            if( 1 < nTmp ) --nTmp; else nTmp = 1;
                            aScX = Fraction( nTmp, 1000 );
                        }
                        else
#endif
                        {
                            if( aScX < aScY )
                                aScY = aScX;

                            // fuer Drawing, damit diese ihre Objecte vernuenftig Painten
                            // koennen, auf "glatte" Prozentwerte setzen
                            aScY *= Fraction( 1000, 1 );
                            long nTmp = (long)aScY;
                            if( 1 < nTmp )
                                --nTmp;
                            else
                                nTmp = 1;
                            aScY = Fraction( nTmp, 1000 );
                            aScX = aScY;
                        }

                        aMapMode.SetScaleY( aScY );
                        aMapMode.SetScaleX( aScX );
                    }

                    Point aPrtOff( pPrt->PixelToLogic(
                                        pPrt->GetPageOffsetPixel(), aMapMode ) );
                    long nPageHeight = (nMaxRowSz / nRow);

                    // dann kann das drucken losgehen
                    rProgress.SetState( nPrintCount++, nSelCount );

                    pPrt->StartPage();
                    Point aCalcPt;
                    SwPageFrm** ppTmpPg = aPageArr;

                    // ist das Array nicht vollsteandig gefuellt ?
                    if( rOptions.bPrintReverse && nCntPage != nPages )
                    {
                        // beim Rueckwaertsdruck alle Seiten nach vorne
                        // verschieben!
                        memmove( ppTmpPg, ppTmpPg + (nPages - nCntPage),
                                    nCntPage * sizeof( SwPageFrm*) );
                        aPageArr[ nCntPage ] = 0;		// Endekennung
                    }

                    long nHOffs = 0, nVOffs = 0, nXStt = 0;
                    if( pPrtData )
                    {
                        const Fraction& rScaleX = aMapMode.GetScaleX();
                        const Fraction& rScaleY = aMapMode.GetScaleY();
                        Fraction aF( pPrtData->GetTopSpace(), 1 );
                        aCalcPt.Y() = aF /= rScaleY;
                        aF = Fraction( pPrtData->GetLeftSpace(), 1 );
                        nXStt = aF /= rScaleX;
                        aF = Fraction( pPrtData->GetVertSpace(), 1 );
                        nVOffs = aF /= rScaleY;
                        aF = Fraction( pPrtData->GetHorzSpace(), 1 );
                        nHOffs = aF /= rScaleX;
                    }

                    for( BYTE nR = 0; *ppTmpPg && nR < nRow; ++nR )
                    {
                        aCalcPt.X() = nXStt;
                        for( BYTE nC = 0; *ppTmpPg && nC < nCol; ++nC )
                        {
                            aShell.Imp()->SetFirstVisPageInvalid();
                            aShell.aVisArea = (*ppTmpPg)->Frm();

                            Point aPos( aCalcPt );
                            aPos -= aShell.aVisArea.Pos();
                            aPos -= aPrtOff;
                            aMapMode.SetOrigin( aPos );
                            pPrt->SetMapMode( aMapMode );
                            (*ppTmpPg)->GetUpper()->Paint( (*ppTmpPg)->Frm() );
                            aCalcPt.X() += nHOffs + (*ppTmpPg)->Frm().Width();
                            ++ppTmpPg;
                        }
                        aCalcPt.Y() += nVOffs + nPageHeight;
                    }
                    pPrt->EndPage();
                    SwPaintQueue::Repaint();

                    memset( aPageArr, 0, sizeof( SwPageFrm* ) * nPages );
                   nCntPage = 0;
                    nMaxRowSz = nMaxColSz = nCalcH = nCalcW = 0;
                }
            }

            if( pStPage == pEndPage )
                pStPage = 0;
            else if( rOptions.bPrintReverse )
            {
                --nPageNo;
                pStPage = (SwPageFrm*)pStPage->GetPrev();
            }
            else
            {
                ++nPageNo;
                pStPage = (SwPageFrm*)pStPage->GetNext();
            }
        }
        pPrt->SetMapMode( aOld );

        if( bHiddenFlds )
        {
            SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
            pFldType->Modify( &aHnt, 0);
            CalcPagesForPrint( (USHORT)aPages.Max() );
        }
        delete[] aPageArr;

        if( bStartJob )
            rOptions.bJobStartet = TRUE;
    }
    pFntCache->Flush();
}

    // Prospektdruck
void ViewShell::PrintProspect( SwPrtOptions& rOptions,
                               SfxProgress& rProgress )
{
    if( !rOptions.aMulti.GetSelectCount() )
        return;

    // wenn kein Drucker vorhanden ist, wird nicht gedruckt
    SfxPrinter* pPrt = GetPrt();
    if( !pPrt || !pPrt->GetName().Len() ||
        ( !rOptions.bPrintLeftPage && !rOptions.bPrintRightPage ))
        return;

    MultiSelection aMulti( rOptions.aMulti );
    Range aPages( aMulti.FirstSelected(), aMulti.LastSelected() );
    if ( aPages.Max() > USHRT_MAX )
        aPages.Max() = USHRT_MAX;

    ASSERT( aPages.Min() > 0,
            "Seite 0 Drucken?" );
    ASSERT( aPages.Min() <= aPages.Max(),
            "MinSeite groesser MaxSeite." );

    // eine neue Shell fuer den Printer erzeugen
    ViewShell aShell( *this, 0 );
    aShell.pRef = new SfxPrinter( *pPrt );

    SET_CURR_SHELL( &aShell );

    aShell.PrepareForPrint( rOptions );

    // gibt es versteckte Absatzfelder, unnoetig wenn die Absaetze bereits
    // ausgeblendet sind.
    int bHiddenFlds = FALSE;
    SwFieldType* pFldType = 0;
    if ( GetViewOptions()->IsShowHiddenPara() )
    {
        pFldType = GetDoc()->GetSysFldType( RES_HIDDENPARAFLD );
        bHiddenFlds = 0 != pFldType->GetDepends();
        if( bHiddenFlds )
        {
            SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
            pFldType->Modify( &aHnt, 0);
        }
    }

    // Seiten fuers Drucken formatieren
    aShell.CalcPagesForPrint( (USHORT)aPages.Max(), &rProgress );

    USHORT nCopyCnt = rOptions.bCollate ? rOptions.nCopyCount : 1;

    const SwPageFrm *pStPage  = (SwPageFrm*)GetLayout()->Lower();

    for( USHORT i = 1; pStPage && i < (USHORT)aPages.Min(); ++i )
        pStPage = (SwPageFrm*)pStPage->GetNext();

    if( !pStPage )          // dann wars das
    {
        if( bHiddenFlds )
        {
            SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
            pFldType->Modify( &aHnt, 0);
            CalcPagesForPrint( (USHORT)aPages.Max() );
        }
        return;
    }

    // unsere SttPage haben wir, dann die EndPage suchen
    SvPtrarr aArr( 255, 255 );
    aArr.Insert( (void*)pStPage, aArr.Count() );

    while( pStPage->GetNext() && i < (USHORT)aPages.Max() )
    {
        pStPage = (SwPageFrm*)pStPage->GetNext();
        if( aMulti.IsSelected( ++i ) )
            aArr.Insert( (void*)pStPage, aArr.Count() );
    }

    // auf Doppelseiten auffuellen
    if( 1 == aArr.Count() )		// eine Seite ist ein Sonderfall
        aArr.Insert( (void*)0, 1 );
    else
    {
        while( aArr.Count() & 3 )
            aArr.Insert( (void*)0, aArr.Count() );

        if( rOptions.bPrintReverse && 4 < aArr.Count() )
        {
            // das Array umsortieren
            // Array:    1 2 3 4 5 6 7 8
            // soll:	 3 4 1 2 7 8 5 6
            // Algorhytmus:
            // vordere Haelfte: Austausch von 2 Pointer von Vorne vor die Haelfte
            // hintere Haelfte: Austausch von 2 Pointer von der Haelfte nach hinten

            USHORT nHalf = aArr.Count() / 2;
            USHORT nSwapCount = nHalf / 4;

            VoidPtr* ppArrStt = (VoidPtr*)aArr.GetData();
            VoidPtr* ppArrHalf = (VoidPtr*)aArr.GetData() + nHalf;

            for( int nLoop = 0; nLoop < 2; ++nLoop )
            {
                for( USHORT n = 0; n < nSwapCount; ++n )
                {
                    void* pTmp = *ppArrStt;
                    *ppArrStt++ = *(ppArrHalf-2);
                    *(ppArrHalf-2) = pTmp;

                    pTmp = *ppArrStt;
                    *ppArrStt++ = *--ppArrHalf;
                    *ppArrHalf-- = pTmp;
                }
                ppArrStt = (VoidPtr*)aArr.GetData() + nHalf;
                ppArrHalf = (VoidPtr*)aArr.GetData() + aArr.Count();
            }
        }
    }

    BOOL bStartJob = FALSE;

    for( USHORT nCnt = 0; nCnt < nCopyCnt; nCnt++ )
    {
        if( rOptions.IsPrintSingleJobs() && rOptions.GetJobName().Len() &&
            ( bStartJob || rOptions.bJobStartet ) )
        {
            pPrt->EndJob();
            rOptions.bJobStartet = TRUE;

            // Reschedule statt Yield, da Yield keine Events abarbeitet
            // und es sonst eine Endlosschleife gibt.
            while( pPrt->IsPrinting() )
                    rProgress.Reschedule();

            rOptions.MakeNextJobName();
            bStartJob = pPrt->StartJob( rOptions.GetJobName() );
        }

        // dann sorge mal dafuer, das alle Seiten in der richtigen
        // Reihenfolge stehen:
        USHORT nSPg = 0, nEPg = aArr.Count(), nStep = 1;
        if( 0 == (nEPg & 1 ))      // ungerade gibt es nicht!
            --nEPg;

        if( !rOptions.bPrintLeftPage )
            ++nStep;
        else if( !rOptions.bPrintRightPage )
        {
            ++nStep;
            ++nSPg, --nEPg;
        }

        USHORT nCntPage = (( nEPg - nSPg ) / ( 2 * nStep )) + 1;

        MapMode aOld( pPrt->GetMapMode() );
        MapMode aMapMode( MAP_TWIP );
        Size aPrtSize( pPrt->PixelToLogic( pPrt->GetPaperSizePixel(), aMapMode ) );

        const XubString aTmp( SW_RES( STR_STATSTR_PRINT ) );
        rProgress.SetText( aTmp );
        //HACK, damit die Anzeige nicht durcheinander kommt:
        rProgress.SetState( 1, nCntPage );
        rProgress.SetText( aTmp );

        for( USHORT nPrintCount = 0; nSPg < nEPg &&
                nPrintCount < nCntPage; ++nPrintCount )
        {
            // Mag der Anwender noch ?
            rProgress.Reschedule();

            if ( !pPrt->IsJobActive() )
                break;

            SwTwips nMaxRowSz, nMaxColSz;
            pStPage = (SwPageFrm*)aArr[ nSPg ];
            const SwPageFrm* pNxtPage = nEPg < aArr.Count()
                                ? pNxtPage = (SwPageFrm*)aArr[ nEPg ]
                                : 0;

            if( !pStPage )
            {
                nMaxColSz = 2 * pNxtPage->Frm().SSize().Width();
                nMaxRowSz = pNxtPage->Frm().SSize().Height();
            }
            else if( !pNxtPage )
            {
                nMaxColSz = 2 * pStPage->Frm().SSize().Width();
                nMaxRowSz = pStPage->Frm().SSize().Height();
            }
            else
            {
                nMaxColSz = pNxtPage->Frm().SSize().Width() +
                            pStPage->Frm().SSize().Width();
                nMaxRowSz = Max( pNxtPage->Frm().SSize().Height(),
                                 pStPage->Frm().SSize().Height() );
            }

            if( 0 == ( nSPg & 1 ) )		// diese muss gespiegel werden
            {
                const SwPageFrm* pTmp = pStPage;
                pStPage = pNxtPage;
                pNxtPage = pTmp;
            }

            // den MapMode einstellen
            aMapMode.SetOrigin( Point() );
            {
                Fraction aScX( aPrtSize.Width(), nMaxColSz );
                Fraction aScY( aPrtSize.Height(), nMaxRowSz );
                if( aScX < aScY )
                    aScY = aScX;

                {
                    // fuer Drawing, damit diese ihre Objecte vernuenftig Painten
                    // koennen, auf "glatte" Prozentwerte setzen
                    aScY *= Fraction( 1000, 1 );
                    long nTmp = (long)aScY;
                    if( 1 < nTmp )
                        --nTmp;
                    else
                        nTmp = 1;
                    aScY = Fraction( nTmp, 1000 );
                }

                aMapMode.SetScaleY( aScY );
                aMapMode.SetScaleX( aScY );
            }

            Point aPrtOff( pPrt->PixelToLogic(
                                pPrt->GetPageOffsetPixel(), aMapMode ) );
            Size aTmpPrtSize( pPrt->PixelToLogic(
                                pPrt->GetPaperSizePixel(), aMapMode ) );

            // dann kann das drucken losgehen
            bStartJob = TRUE;
            rProgress.SetState( nPrintCount, nCntPage );

            pPrt->StartPage();

            Point aSttPt;
            for( int nC = 0; nC < 2; ++nC )
            {
                if( pStPage )
                {
                    aShell.Imp()->SetFirstVisPageInvalid();
                    aShell.aVisArea = pStPage->Frm();

                    Point aPos( aSttPt );
                    aPos -= aShell.aVisArea.Pos();
                    aPos -= aPrtOff;
                    aMapMode.SetOrigin( aPos );
                    pPrt->SetMapMode( aMapMode );
                    pStPage->GetUpper()->Paint( pStPage->Frm() );
                }

                pStPage = pNxtPage;
                aSttPt.X() += aTmpPrtSize.Width() / 2;
            }

            pPrt->EndPage();
            SwPaintQueue::Repaint();
            nSPg += nStep;
            nEPg -= nStep;
        }
        pPrt->SetMapMode( aOld );

        if( bHiddenFlds )
        {
            SwMsgPoolItem aHnt( RES_HIDDENPARA_PRINT );
            pFldType->Modify( &aHnt, 0);
            CalcPagesForPrint( (USHORT)aPages.Max() );
        }

        if( bStartJob )
            rOptions.bJobStartet = TRUE;
    }
    pFntCache->Flush();
}


BOOL ViewShell::IsPreViewDocPos(
        Point& rDocPt,				// DocPos die bestimmt und auf
                                    // Layout Korrdination einer View umge-
                                    // rechnet werden soll
        USHORT nRowCol, 			// Zeilen/Spalten (Row im Hi, Col im LowByte!)
        USHORT nSttPage,			// Start ab dieser Seite, eine gueltige ??
        const Size& rPageSize 		// (Max)Size einer Seite
        )
{
    BOOL bRet = FALSE;
    SET_CURR_SHELL( this );

    const SwRootFrm* pRoot = GetLayout();
    ASSERT( pRoot, "Wo ist mein Layout?" );

    BYTE nRow = BYTE(nRowCol >> 8), nCol = BYTE(nRowCol & 0xff);
    SwPageFrm* pSttPage = ::lcl_GetSttPage( nRow, nCol, nSttPage, pRoot );

    MapMode aMapMode( GetOut()->GetMapMode() );
    MapMode aCalcMapMode( aMapMode );
    aCalcMapMode.SetOrigin( Point() );

    Point aFreePt( nPrevViewXFreePix, nPrevViewYFreePix );
    aFreePt = GetOut()->PixelToLogic( aFreePt, aCalcMapMode );
    Point aCalcPt( aFreePt );
    long nPageHeight = aFreePt.Y() + rPageSize.Height()+1;
    SwPageFrm* pPage = pSttPage;

    // Sonderbehandlung fuer die erste Seite
    // JP 19.08.98: aber nur wenn sie mehrspaltig angezeigt wird
    int bFirstPg = !nSttPage && 1 != nCol;

    for( BYTE nCntRow = 0; pPage && nCntRow < nRow; ++nCntRow )
    {
        aCalcPt.X() = aFreePt.X();
        for( BYTE nCntCol = 0; pPage && nCntCol < nCol; ++nCntCol )
        {
            // Sonderbehandlung fuer 1. Seite, ist immer eine rechte
            if( bFirstPg )
            {
                bFirstPg = FALSE;
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
                continue;
            }

            if( pPage->IsEmptyPage() )
                aCalcPt.X() += rPageSize.Width()+1 + aFreePt.X();
            else
            {
                SwRect aPgRect( aCalcPt, pPage->Frm().SSize() );
                if( aPgRect.IsInside( rDocPt ))
                {
                    // dann den richtigen Point errechnen:
                    Point aOffset( rDocPt );
                    aOffset -= aCalcPt;
                    aOffset += pPage->Frm().Pos();
                    rDocPt = aOffset;
                    bRet = TRUE;
                    break;
                }
                aCalcPt.X() += pPage->Frm().Width()+1 + aFreePt.X();
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        if( bRet )
            break;

        aCalcPt.Y() += nPageHeight;
    }
    return bRet;
}

Size ViewShell::GetPagePreViewPrtMaxSize() const
{
    Size aMaxSize;
    register const SwFrm *pSttPage = GetLayout()->Lower();
    while( pSttPage )
    {
        register const Size& rSize = pSttPage->Frm().SSize();
        if( aMaxSize.Width() < rSize.Width() )
            aMaxSize.Width() = rSize.Width();
        if( aMaxSize.Height() < rSize.Height() )
            aMaxSize.Height() = rSize.Height();
        pSttPage = pSttPage->GetNext();
    }
    return aMaxSize;
}

Point ViewShell::GetPreviewFreePix() const
{
    return Point( nPrevViewXFreePix, nPrevViewYFreePix );
}

/*************************************************************************

      $Log: not supported by cvs2svn $
      Revision 1.12  2002/12/02 07:55:21  od
      #103492# implement new methods for new preview functionality

      Revision 1.11  2002/06/17 10:19:33  ama
      Fix #99298#: Light blue as page preview selection border

      Revision 1.10  2002/05/29 15:05:09  mib
      #99641#: focus in page preview

      Revision 1.9  2002/05/29 14:28:45  os
      #99649# 'single print job' corrected

      Revision 1.8  2002/05/22 11:46:20  dvo
      #95586# made page preview accessible

      Revision 1.6  2002/05/13 12:13:27  os
      #90353# new[]-delete[] mix fixed

      Revision 1.5  2002/04/08 14:32:45  ama
      Fix #98404#: Border and shadow for pages

      Revision 1.4  2002/03/15 07:33:34  os
      #97978# page preview accessiblity implemented

      Revision 1.3  2001/05/10 08:45:09  os
      store print options at the document

      Revision 1.2  2000/10/25 12:03:41  jp
      Spellchecker/Hyphenator are not longer member of the shells

      Revision 1.1.1.1  2000/09/19 00:08:29  hr
      initial import

      Revision 1.74  2000/09/18 16:04:37  willem.vandorp
      OpenOffice header added.

      Revision 1.73  2000/05/09 11:45:55  ama
      Unicode changes

      Revision 1.72  2000/04/27 07:37:23  os
      UNICODE

      Revision 1.71  2000/02/11 14:36:11  hr
      #70473# changes for unicode ( patched by automated patchtool )

      Revision 1.70  1999/12/14 14:28:30  jp
      Bug #69595#: print can create single Jobs

      Revision 1.69  1999/02/22 07:35:12  MA
      1949globale Shell entsorgt, Shells am RootFrm


      Rev 1.68   22 Feb 1999 08:35:12   MA
   1949globale Shell entsorgt, Shells am RootFrm

      Rev 1.67   14 Sep 1998 13:06:50   JP
   Bug #56069#: keine 0 oder negative Scalierung erzeugen

      Rev 1.66   19 Aug 1998 14:10:16   JP
   Task #55252#: Sonderfall der Einspaltigkeit beachten

      Rev 1.65   19 Aug 1998 11:16:30   JP
   Task #55252#: optionale PagePreViewPrint-Einstellungen beachten

      Rev 1.64   24 Jun 1998 18:45:24   MA
   DataChanged fuer ScrollBar und Retouche, Retouche ganz umgestellt

      Rev 1.63   21 Jun 1998 12:43:16   JP
   Bug #51189#: ScrollHandling umgebaut; ab Seite 0 starten um Seite 1 & 2 nebeneinander zu sehen

      Rev 1.62   19 Jun 1998 16:01:56   JP
   neu: IsPreViewDocPos - stelle fest, ob der Point innerhalb einer Seite liegt

      Rev 1.61   29 Apr 1998 09:31:28   MA
   RetoucheBrush -> RetoucheColor

      Rev 1.60   27 Apr 1998 12:28:12   AMA
   Fix: Prospektdruck einer Selektion: Richtige Seiten drucken!

      Rev 1.59   27 Jan 1998 22:35:42   JP
   GetNumDepend durch GetDepends ersetzt

      Rev 1.58   20 Nov 1997 12:39:22   MA
   includes

      Rev 1.57   03 Nov 1997 13:07:30   MA
   precomp entfernt

      Rev 1.56   29 Oct 1997 15:27:56   JP
   PreViewPage: Leere Seiten mit anzeigen

      Rev 1.55   13 Oct 1997 10:30:44   MA
   Umbau/Vereinfachung Paint

      Rev 1.54   01 Jul 1997 15:42:52   JP
   Bug #41162#: beim drucken die Raender des Drukers herausrechen

      Rev 1.53   18 Jun 1997 16:39:52   AMA
   Fix #40745#: Formatieren fuer einen Referenzdrucker mit Zoomfaktor 1

      Rev 1.52   06 Jun 1997 12:45:46   MA
   chg: versteckte Absaetze ausblenden

      Rev 1.51   14 Apr 1997 18:21:24   MA
   #38806# Options auch fuer Prospect, jetzt mit eigener Methode

      Rev 1.50   09 Feb 1997 20:58:44   JP
   Bug #35760#: beim Core-Repaint die Bereiche der PagePreView mitteilen

      Rev 1.49   19 Dec 1996 14:32:06   MA
   #34691# BlackFont-Option uebernehmen

      Rev 1.48   11 Nov 1996 09:57:56   MA
   ResMgr

      Rev 1.47   24 Aug 1996 17:10:36   JP
   svdraw.hxx entfernt

      Rev 1.46   26 Jul 1996 16:58:58   JP
   Bug #29897#: PropectDruck - invers Druck korrekt behandeln

      Rev 1.45   18 Jul 1996 07:18:18   SWG
   ein include zu wenig

      Rev 1.44   17 Jul 1996 10:53:04   OS
   svdraw unter OS/2 ausserhalb der PCH

      Rev 1.43   16 Jul 1996 15:52:36   MA
   new: PrintPageBackground

      Rev 1.42   02 Jul 1996 12:08:52   JP
   Bug #28965#: Positionierung korrigiert/optimiert

      Rev 1.41   28 Jun 1996 10:59:26   AMA
   Fix: Kein ASSERT mehr beim Komplettausdruck der Seitenvorschau.

      Rev 1.40   27 Jun 1996 19:12:14   HJS
   includes

      Rev 1.39   27 Jun 1996 19:08:36   HJS
   includes

      Rev 1.38   27 Jun 1996 16:24:28   MA
   includes

      Rev 1.37   24 Jun 1996 20:17:54   HJS
   include hintids.hxx

      Rev 1.36   11 Jun 1996 19:44:40   JP
   Bug #27584#: kein ULONG_MAX als Item verschicken -> eigene MessageId definieren

      Rev 1.35   21 Mar 1996 14:04:30   MA
   svhxx entfernt

      Rev 1.34   20 Mar 1996 11:50:46   MA
   Warnings

      Rev 1.33   08 Mar 1996 14:19:00   JP
   kleinere Bugs beim PrintProspect behoben

      Rev 1.32   07 Mar 1996 16:48:24   JP
   neu: PrintPropect

      Rev 1.31   23 Jan 1996 14:14:46   JP
   Bug #24575#: an der Imp-Struktur die 1. sichtbare Seite setzen

      Rev 1.30   30 Nov 1995 13:26:20   MA
   opt: Desktop nicht mehr per Polygon

      Rev 1.29   24 Nov 1995 17:11:46   OM
   PCH->PRECOMPILED

      Rev 1.28   14 Nov 1995 11:01:48   MA
   Options nachgepflegt

      Rev 1.27   13 Nov 1995 12:18:36   MA
   chg: static -> lcl_

      Rev 1.26   08 Nov 1995 12:17:34   AMA
   Set statt Change (301)

      Rev 1.25   29 Aug 1995 09:59:12   JP
   Bug 18260: PrintPreView - nicht ueber den Rand hinausdrucken

      Rev 1.24   22 Aug 1995 17:59:08   JP
   Bug 17695: Print - auf der letzten Seite das Raster beibehalten

      Rev 1.23   11 Aug 1995 18:10:12   JP
   Print: reverse drucken - nicht den Speicher ueberschreiben, PageFrame-Pointer verschieben

      Rev 1.22   11 Aug 1995 14:48:34   AMA
   Fix: Kopienanzahl beim Drucken quadriert

      Rev 1.21   09 Aug 1995 22:04:16   ER
   ! global/static data _vor_ seg_eofglobals

      Rev 1.20   06 Aug 1995 20:28:58   JP
   Bug17235: erst die Wiese malen; Bug17090: nicht immer alles Painten;  Bundsteg abgeklemmt

      Rev 1.19   19 Jun 1995 09:51:52   KH
   Anpassung Metrowerks

      Rev 1.18   10 May 1995 14:01:18   MA
   fix: Painten waehrend des Druckens.

      Rev 1.17   06 May 1995 14:22:32   JP
   USHORTs als static

      Rev 1.16   06 May 1995 10:08:28   JP
   ClacPreView: WinSize in Pixel, Abstaende richtig berechnen

      Rev 1.15   05 May 1995 19:35:22   AMA
   Umbau pProgress; Fix: FontCache+PreView

      Rev 1.14   03 May 1995 20:34:14   AMA
   Umbau: SfxProgress etc.

      Rev 1.13   20 Apr 1995 18:21:26   JP
   MapMode: auf glatte Prozentwerte runden, Bugfix fuers Drucken

      Rev 1.12   27 Mar 1995 20:21:28   JP
   CalcPreView: neuer Parameter - virtuelle Seitennummer returnen (fuer StatusLeiste)

      Rev 1.11   13 Mar 1995 11:38:22   KH
   Anpassung CFRONT

      Rev 1.10   09 Mar 1995 16:58:26   JP
   PreViewPage: Visarea am Ende loeschen, dadurch Ausgabe verhindern, wenn in anderer Shell eine Eingabe erfolgt

      Rev 1.9   02 Mar 1995 11:23:34   JP
   PrintPreView: Seitenhoehe richtig errechnen

      Rev 1.8   01 Mar 1995 22:35:30   JP
   Drucken noch ein wenig verbessert

      Rev 1.7   01 Mar 1995 21:05:20   JP
   neu: Drucken (ungetestet)

      Rev 1.6   28 Feb 1995 21:33:10   JP
   PreViewPage: bei der richtigen Seite den Bundsteg malen

      Rev 1.5   27 Feb 1995 11:11:26   JP
   kleinere Bugfixes

      Rev 1.4   27 Feb 1995 00:05:22   JP
   Paint: Current Shell stezen

      Rev 1.3   26 Feb 1995 15:05:18   JP
   nochmals umgestellt, Max aller Seiten errechnen

      Rev 1.2   24 Feb 1995 21:43:04   JP
   weitere Teile implementiert

      Rev 1.1   24 Feb 1995 15:46:04   JP
   1.Seite beachten, Doppelseiten sonderbehandeln

      Rev 1.0   24 Feb 1995 00:13:12   JP
   Initial revision.

*************************************************************************/
