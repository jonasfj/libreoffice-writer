/*************************************************************************
 *
 *  $RCSfile: viewport.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: jp $ $Date: 2001-10-11 07:06:17 $
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

#include "hintids.hxx"

#ifndef _SV_HELP_HXX //autogen
#include <vcl/help.hxx>
#endif

#ifndef _SVX_RULER_HXX //autogen
#include <svx/ruler.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXSTBMGR_HXX //autogen
#include <sfx2/stbmgr.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif


#include "view.hxx"
#include "wrtsh.hxx"
#include "swmodule.hxx"
#include "viewopt.hxx"
#include "frmatr.hxx"
#include "docsh.hxx"
#include "cmdid.h"
#include "edtwin.hxx"
#include "scroll.hxx"
#include "wview.hxx"
#include "usrpref.hxx"
#include "pagedesc.hxx"
#include "workctrl.hxx"

//Das SetVisArea der DocShell darf nicht vom InnerResizePixel gerufen werden.
//Unsere Einstellungen muessen aber stattfinden.
#if SUPD<631
#define SVX_ZOOM_PAGEWIDTH_NOBORDER 4
#endif

static BOOL bProtectDocShellVisArea = FALSE;

static USHORT nPgNum = 0;

inline BOOL SwView::IsDocumentBorder()
{
    return GetDocShell()->GetProtocol().IsInPlaceActive() ||
           GetDocShell()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED ||
           pWrtShell->IsBrowseMode() ||
           SVX_ZOOM_PAGEWIDTH_NOBORDER == (SvxZoomType)pWrtShell->GetViewOptions()->GetZoomType();
}
inline long GetLeftMargin( SwView &rView )
{
    BOOL bWeb = 0 != PTR_CAST(SwWebView, &rView);
    SvxZoomType eType = (SvxZoomType)rView.GetWrtShell().GetViewOptions()->GetZoomType();
    long lRet = rView.GetWrtShell().GetAnyCurRect(RECT_PAGE_PRT).Left();
    return eType == SVX_ZOOM_PERCENT   ? lRet + DOCUMENTBORDER :
           eType == SVX_ZOOM_PAGEWIDTH || eType == SVX_ZOOM_PAGEWIDTH_NOBORDER ? 0 :
                                         lRet + DOCUMENTBORDER + nLeftOfst;
}

//-------------------------------------------------------------------------

void lcl_GetPos(SwView* pView,
                Point& rPos,
                SwScrollbar* pScrollbar,
                FASTBOOL bBorder)
{
    SwWrtShell &rSh = pView->GetWrtShell();
    const Size aDocSz( rSh.GetDocSize() );

    const long lBorder = bBorder ? DOCUMENTBORDER : DOCUMENTBORDER * 2;
    BOOL bHori = pScrollbar->IsHoriScroll();

    const long lPos = pScrollbar->GetThumbPos() + (bBorder ? DOCUMENTBORDER : 0);
    long Point:: *pPt = bHori ? &Point::nA : &Point::nB;
    long Size::  *pSz = bHori ? &Size::nA  : &Size::nB;

    long lDelta = lPos - rSh.VisArea().Pos().*pPt;
    const long lSize = aDocSz.*pSz + lBorder;
    // Bug 11693: sollte rechts oder unten zuviel Wiese sein, dann muss
    // 			  diese von der VisArea herausgerechnet werden!
    long nTmp = pView->GetVisArea().Right()+lDelta;
    if ( bHori && nTmp > lSize )
        lDelta -= nTmp - lSize;
    nTmp = pView->GetVisArea().Bottom()+lDelta;
    if ( !bHori && nTmp > lSize )
        lDelta -= nTmp - lSize;

    rPos.*pPt += lDelta;
    if ( bBorder && rPos.*pPt < DOCUMENTBORDER )
        rPos.*pPt = DOCUMENTBORDER;
}

/*--------------------------------------------------------------------
    Beschreibung:	Nullpunkt Lineal setzen
 --------------------------------------------------------------------*/

void SwView::InvalidateRulerPos()
{
    static USHORT __READONLY_DATA aInval[] =
    {
        SID_ATTR_PARA_LRSPACE, SID_RULER_BORDERS, SID_RULER_PAGE_POS,
        SID_RULER_LR_MIN_MAX, SID_ATTR_LONG_ULSPACE, SID_ATTR_LONG_LRSPACE,
        FN_STAT_PAGE, 0
    };

    GetViewFrame()->GetBindings().Invalidate(aInval);

    DBG_ASSERT(pHLineal, "warum ist das Lineal nicht da?")
    pHLineal->ForceUpdate();
    if( pVLineal )
        pVLineal->ForceUpdate();
}

/*--------------------------------------------------------------------
    Beschreibung:	begrenzt das Scrollen soweit, dass jeweils nur einen
                    viertel Bildschirm bis vor das Ende des Dokumentes
                    gescrollt werden kann.
 --------------------------------------------------------------------*/

long SwView::SetHScrollMax( long lMax )
{
    const long lBorder = IsDocumentBorder() ? DOCUMENTBORDER : DOCUMENTBORDER * 2;
    const long lSize = GetDocSz().Width() + lBorder - aVisArea.GetWidth();

    // bei negativen Werten ist das Dokument vollstaendig sichtbar;
    // in diesem Fall kein Scrollen
    return Max( Min( lMax, lSize ), 0L );
}


long SwView::SetVScrollMax( long lMax )
{
    const long lBorder = IsDocumentBorder() ? DOCUMENTBORDER : DOCUMENTBORDER * 2;
    long lSize = GetDocSz().Height() + lBorder - aVisArea.GetHeight();
    return Max( Min( lMax, lSize), 0L );		// siehe horz.
}


Point SwView::AlignToPixel(const Point &rPt) const
{
    return GetEditWin().PixelToLogic( GetEditWin().LogicToPixel( rPt ) );
}

/*--------------------------------------------------------------------
    Beschreibung:	Dokumentgroesse hat sich geaendert
 --------------------------------------------------------------------*/

void SwView::DocSzChgd(const Size &rSz)
{

extern int bDocSzUpdated;

    aDocSz = rSz;

    if( !pWrtShell || aVisArea.IsEmpty() )		// keine Shell -> keine Aenderung
    {
        bDocSzUpdated = FALSE;
        return;
    }

    //Wenn Text geloescht worden ist, kann es sein, dass die VisArea hinter
    //den sichtbaren Bereich verweist
    Rectangle aNewVisArea( aVisArea );
    BOOL bModified = FALSE;
    const SwTwips lGreenOffset = IsDocumentBorder() ? DOCUMENTBORDER : DOCUMENTBORDER * 2;
    SwTwips lTmp = aDocSz.Width() + lGreenOffset;
    if ( aNewVisArea.Right() >= lTmp  )
    {
        lTmp = aNewVisArea.Right() - lTmp;
        aNewVisArea.Right() -= lTmp;
        aNewVisArea.Left() -= lTmp;
        bModified = TRUE;
    }
    lTmp = aDocSz.Height() + lGreenOffset;
    if ( aNewVisArea.Bottom() >= lTmp )
    {
        lTmp = aNewVisArea.Bottom() - lTmp;
        aNewVisArea.Bottom() -= lTmp;
        aNewVisArea.Top() -= lTmp;
        bModified = TRUE;
    }

    if ( bModified )
        SetVisArea( aNewVisArea, FALSE );

//	FASTBOOL bHResize = pHScrollbar && pHScrollbar->IsAuto() && pHScrollbar->IsVisible();
//	FASTBOOL bVResize = pVScrollbar && pVScrollbar->IsAuto() && pVScrollbar->IsVisible();
    if ( UpdateScrollbars() && !bInOuterResizePixel )//&&
//		 ((bHResize != (pHScrollbar && pHScrollbar->IsAuto() && pHScrollbar->IsVisible())) ||
//		  (bVResize != (pVScrollbar && pVScrollbar->IsAuto() && pVScrollbar->IsVisible()))) )
    {
        OuterResizePixel( Point(),
                          GetViewFrame()->GetWindow().GetOutputSizePixel() );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Visarea neu setzen
 --------------------------------------------------------------------*/

void SwView::SetVisArea( const Rectangle &rRect, BOOL bUpdateScrollbar )
{
    const Size aOldSz( aVisArea.GetSize() );

    const Point aTopLeft(	  AlignToPixel( rRect.TopLeft() ));
    const Point aBottomRight( AlignToPixel( rRect.BottomRight() ));
    Rectangle aLR( aTopLeft, aBottomRight );

    if( aLR == aVisArea )
        return;

    const SwTwips lMin = IsDocumentBorder() ? DOCUMENTBORDER : 0;

    // keine negative Position, keine neg. Groesse
    if( aLR.Top() < lMin )
    {
        aLR.Bottom() += lMin - aLR.Top();
        aLR.Top() = lMin;
    }
    if( aLR.Left() < lMin )
    {
        aLR.Right() += lMin - aLR.Left();
        aLR.Left() = lMin;
    }
    if( aLR.Right() < 0 )
        aLR.Right() = 0;
    if( aLR.Bottom() < 0 )
        aLR.Bottom() = 0;

    if( aLR == aVisArea )
        return;

    const Size aSize( aLR.GetSize() );
    if( aSize.Width() < 0 || aSize.Height() < 0 )
        return;

    //Bevor die Daten veraendert werden ggf. ein Update rufen. Dadurch wird
    //sichergestellt, da� anliegende Paints korrekt in Dokumentkoordinaten
    //umgerechnet werden.
    //Vorsichtshalber tun wir das nur wenn an der Shell eine Action laeuft,
    //denn dann wir nicht wirklich gepaintet sondern die Rechtecke werden
    //lediglich (in Dokumentkoordinaten) vorgemerkt.
    if ( pWrtShell && pWrtShell->ActionPend() )
        pWrtShell->GetWin()->Update();

    aVisArea = aLR;

    const FASTBOOL bOuterResize = bUpdateScrollbar && UpdateScrollbars() && !bInOuterResizePixel;

    if ( pWrtShell )
    {
        pWrtShell->VisPortChgd( aVisArea );
        if ( aOldSz != pWrtShell->VisArea().SSize() &&
             ( Abs(aOldSz.Width() - pWrtShell->VisArea().Width()) > 2 ||
                Abs(aOldSz.Height() - pWrtShell->VisArea().Height()) > 2 ) )
            pWrtShell->CheckBrowseView( FALSE );
    }

    if ( !bProtectDocShellVisArea )
    {
        //Wenn die Groesse der VisArea unveraendert ist, reichen wir die
        //Groesse der VisArea vom InternalObject weiter. Damit soll der
        //Transport von Fehlern vermieden werden.
        Rectangle aVis( aVisArea );
        if ( aVis.GetSize() == aOldSz )
            aVis.SetSize(
                    GetDocShell()->SfxInPlaceObject::GetVisArea().GetSize() );

        //Bei embedded immer mit Modify...
        if ( GetDocShell()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
            GetDocShell()->SfxInPlaceObject::SetVisArea( aVis );
        else
            GetDocShell()->SvEmbeddedObject::SetVisArea( aVis );
    }

    SfxViewShell::VisAreaChanged( aVisArea );

    InvalidateRulerPos();

    SwEditWin::ClearTip();

    if ( bOuterResize )
        OuterResizePixel( Point(),
                          GetViewFrame()->GetWindow().GetOutputSizePixel() );
}

/*--------------------------------------------------------------------
    Beschreibung:	Pos VisArea setzen
 --------------------------------------------------------------------*/

void SwView::SetVisArea( const Point &rPt, BOOL bUpdateScrollbar )
{
    //einmal alignen, damit Brushes korrekt angesetzt werden.
    //MA 31. May. 96: Das geht in der BrowseView schief, weil evlt.
    //nicht das ganze Dokument sichtbar wird. Da der Inhalt in Frames
    //passgenau ist, kann nicht aligned werden (bessere Idee?!?!)
    //MA 29. Oct. 96 (fix: Bild.de, 200%) ganz ohne Alignment geht es nicht
    //mal sehen wie weit wir mit der halben BrushSize kommen.
    //TODO: why BRUSH_SIZE?
    Point aPt( rPt );
//	const long nTmp = GetWrtShell().IsFrameView() ? BRUSH_SIZE/2 : BRUSH_SIZE;
    const long nTmp = GetWrtShell().IsFrameView() ? 4 : 8;
    aPt = GetEditWin().LogicToPixel( aPt );
    aPt.X() -= aPt.X() % nTmp;
    aPt.Y() -= aPt.Y() % nTmp;
    aPt = GetEditWin().PixelToLogic( aPt );

    if ( aPt == aVisArea.TopLeft() )
        return;

    const long lXDiff = aVisArea.Left() - aPt.X();
    const long lYDiff = aVisArea.Top()	- aPt.Y();
    SetVisArea( Rectangle( aPt,
            Point( aVisArea.Right() - lXDiff, aVisArea.Bottom() - lYDiff ) ),
            bUpdateScrollbar);
}


void SwView::CheckVisArea()
{
    if ( pHScrollbar )
        pHScrollbar->SetAuto( pWrtShell->IsBrowseMode() &&
                              !GetDocShell()->GetProtocol().IsInPlaceActive() );
    if ( IsDocumentBorder() )
    {
        if ( aVisArea.Left() != DOCUMENTBORDER ||
             aVisArea.Top()  != DOCUMENTBORDER )
        {
            Rectangle aNewVisArea( aVisArea );
            aNewVisArea.Move( DOCUMENTBORDER - aVisArea.Left(),
                              DOCUMENTBORDER - aVisArea.Top() );
            SetVisArea( aNewVisArea, TRUE );
        }
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Sichtbaren Bereich berechnen

    OUT Point *pPt:				neue Position des sichtbaren
                                Bereiches
    IN	Rectangle &rRect:		Rechteck, das sich innerhalb des neuen
                                sichtbaren Bereiches befinden soll
        USHORT nRange			optional exakte Angabe des Bereiches,
                                um den ggfs. gescrollt werden soll
 --------------------------------------------------------------------*/

void SwView::CalcPt( Point *pPt, const Rectangle &rRect,
                     USHORT nRangeX, USHORT nRangeY)
{

    const SwTwips lMin = IsDocumentBorder() ? DOCUMENTBORDER : 0;

    long nYScroll = GetYScroll();
    long nDesHeight = rRect.GetHeight();
    long nCurHeight = aVisArea.GetHeight();
    nYScroll = Min(nYScroll, nCurHeight - nDesHeight); // wird es knapp, dann nicht zuviel scrollen
    if(nDesHeight > nCurHeight) // die Hoehe reicht nicht aus, dann interessiert nYScroll nicht mehr
    {
        pPt->Y() = rRect.Top();
        pPt->Y() = Max( lMin, pPt->Y() );
    }
    else if ( rRect.Top() < aVisArea.Top() )				//Verschiebung nach oben
    {
        pPt->Y() = rRect.Top() - (nRangeY != USHRT_MAX ? nRangeY : nYScroll);
        pPt->Y() = Max( lMin, pPt->Y() );
    }
    else if( rRect.Bottom() > aVisArea.Bottom() )	//Verschiebung nach unten
    {
        pPt->Y() = rRect.Bottom() -
                    (aVisArea.GetHeight()) + ( nRangeY != USHRT_MAX ?
            nRangeY : nYScroll );
        pPt->Y() = SetVScrollMax( pPt->Y() );
    }
    long nXScroll = GetXScroll();
    if ( rRect.Right() > aVisArea.Right() )			//Verschiebung nach rechts
    {
        pPt->X() = rRect.Right()  -
                    (aVisArea.GetWidth()) +
                    (nRangeX != USHRT_MAX ? nRangeX : nXScroll);
        pPt->X() = SetHScrollMax( pPt->X() );
    }
    else if ( rRect.Left() < aVisArea.Left() )		//Verschiebung nach links
    {
        pPt->X() = rRect.Left() -
                    (nRangeX != USHRT_MAX ? nRangeX : nXScroll);
        pPt->X() = Max( ::GetLeftMargin( *this ) + nLeftOfst, pPt->X() );
        pPt->X() = Min( rRect.Left() - nScrollX, pPt->X() );
        pPt->X() = Max( 0L, pPt->X() );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Scrolling
 --------------------------------------------------------------------*/

BOOL SwView::IsScroll( const Rectangle &rRect ) const
{
    return bCenterCrsr || bTopCrsr || !aVisArea.IsInside(rRect);
}


void SwView::Scroll( const Rectangle &rRect, USHORT nRangeX, USHORT nRangeY )
{
    if ( aVisArea.IsEmpty() )
        return;

    Rectangle aOldVisArea( aVisArea );
    long nDiffY = 0;

    Window* pCareWn = ViewShell::GetCareWin(GetWrtShell());
    if ( pCareWn )
    {
        Rectangle aDlgRect( GetEditWin().PixelToLogic(
                    GetEditWin().ScreenToOutputPixel( pCareWn->GetPosPixel() ) ),
                    GetEditWin().PixelToLogic( pCareWn->GetSizePixel() ) );
        // Nur, wenn der Dialog nicht rechts oder links der VisArea liegt:
        if ( aDlgRect.Left() < aVisArea.Right() &&
             aDlgRect.Right() > aVisArea.Left() )
        {
            // Falls wir nicht zentriert werden sollen, in der VisArea liegen
            // und nicht vom Dialog ueberdeckt werden ...
            if ( !bCenterCrsr && aOldVisArea.IsInside( rRect )
                 && ( rRect.Left() > aDlgRect.Right()
                      || rRect.Right() < aDlgRect.Left()
                      || rRect.Top() > aDlgRect.Bottom()
                      || rRect.Bottom() < aDlgRect.Top() ) )
                return;

            // Ist oberhalb oder unterhalb der Dialogs mehr Platz?
            long nTopDiff = aDlgRect.Top() - aVisArea.Top();
            long nBottomDiff = aVisArea.Bottom() - aDlgRect.Bottom();
            if ( nTopDiff < nBottomDiff )
            {
                if ( nBottomDiff > 0 ) // Ist unterhalb ueberhaupt Platz?
                {	// dann verschieben wir die Oberkante und merken uns dies
                    nDiffY = aDlgRect.Bottom() - aVisArea.Top();
                    aVisArea.Top() += nDiffY;
                }
            }
            else
            {
                if ( nTopDiff > 0 ) // Ist oberhalb ueberhaupt Platz?
                    aVisArea.Bottom() = aDlgRect.Top(); // Unterkante aendern
            }
        }
    }

    //s.o. !IsScroll()
    if( !(bCenterCrsr || bTopCrsr) && aVisArea.IsInside( rRect ) )
    {
        aVisArea = aOldVisArea;
        return;
    }
    //falls das Rechteck groesser als der sichtbare Bereich -->
    //obere linke Ecke
    Size aSize( rRect.GetSize() );
    const Size aVisSize( aVisArea.GetSize() );
    if( !aVisArea.IsEmpty() && (
        aSize.Width() + GetXScroll() > aVisSize.Width() ||
        aSize.Height()+ GetYScroll() > aVisSize.Height() ))
    {
        Point aPt( aVisArea.TopLeft() );
        aSize.Width() = Min( aSize.Width(), aVisSize.Width() );
        aSize.Height()= Min( aSize.Height(),aVisSize.Height());

        CalcPt( &aPt, Rectangle( rRect.TopLeft(), aSize ),
                (aVisSize.Width() - aSize.Width()) / 2,
                (aVisSize.Height()- aSize.Height())/ 2 );

        if( bTopCrsr )
        {
            long nBorder = IsDocumentBorder() ? DOCUMENTBORDER : 0;
            aPt.Y() = Min( Max( nBorder, rRect.Top() ),
                                aDocSz.Height() + nBorder -
                                    aVisArea.GetHeight() );
        }
        aPt.Y() -= nDiffY;
        aVisArea = aOldVisArea;
        SetVisArea( aPt );
        return;
    }
    if( !bCenterCrsr )
    {
        Point aPt( aVisArea.TopLeft() );
        CalcPt( &aPt, rRect, nRangeX, nRangeY );

        if( bTopCrsr )
        {
            long nBorder = IsDocumentBorder() ? DOCUMENTBORDER : 0;
            aPt.Y() = Min( Max( nBorder, rRect.Top() ),
                                aDocSz.Height() + nBorder -
                                    aVisArea.GetHeight() );
        }

        aPt.Y() -= nDiffY;
        aVisArea = aOldVisArea;
        SetVisArea( aPt );
        return;
    }

    //Cursor zentrieren
    Point aPnt( aVisArea.TopLeft() );
    // ... in Y-Richtung auf jeden Fall
    aPnt.Y() += ( rRect.Top() + rRect.Bottom()
                  - aVisArea.Top() - aVisArea.Bottom() ) / 2 - nDiffY;
    // ... in X-Richtung nur, wenn das Rechteck rechts oder links aus der
    //	   VisArea hinausragt.
    if ( rRect.Right() > aVisArea.Right() || rRect.Left() < aVisArea.Left() )
    {
        aPnt.X() += ( rRect.Left() + rRect.Right()
                  - aVisArea.Left() - aVisArea.Right() ) / 2;
        aPnt.X() = SetHScrollMax( aPnt.X() );
        const SwTwips lMin = IsDocumentBorder() ? DOCUMENTBORDER : 0;
        aPnt.X() = Max( (GetLeftMargin( *this ) - lMin) + nLeftOfst, aPnt.X() );
    }
    aVisArea = aOldVisArea;
    if( pCareWn )
    {   // Wenn wir nur einem Dialog ausweichen wollen, wollen wir nicht ueber
        // das Ende des Dokument hinausgehen.
        aPnt.Y() = SetVScrollMax( aPnt.Y() );
    }
    SetVisArea( aPnt );
}

/*--------------------------------------------------------------------
    Beschreibung:	Seitenweises Scrollen
    Liefern den Wert, um den bei PageUp / -Down gescrollt werden soll
 --------------------------------------------------------------------*/

BOOL SwView::GetPageScrollUpOffset( SwTwips &rOff ) const
{
    if ( !aVisArea.Top() || !aVisArea.GetHeight() )
        return FALSE;
    rOff = -(aVisArea.GetHeight() - (GetYScroll() / 2));
    //nicht vor den Dokumentanfang scrollen
    rOff = aVisArea.Top() - rOff < 0 ? rOff - aVisArea.Top() : rOff;
    return TRUE;
}


BOOL SwView::GetPageScrollDownOffset( SwTwips &rOff ) const
{
    if ( !aVisArea.GetHeight() ||
         (aVisArea.GetHeight() > aDocSz.Height()) )
        return FALSE;
    rOff = aVisArea.GetHeight() - (GetYScroll() / 2);
    //nicht hinter das Dokumentende scrollen
    if ( aVisArea.Top() + rOff > aDocSz.Height() )
        rOff = aDocSz.Height() - aVisArea.Bottom();
    return rOff > 0;
}

// Seitenweises Blaettern

long SwView::PageUp()
{
    if (!aVisArea.GetHeight())
        return 0;

    Point aPos(aVisArea.TopLeft());
    aPos.Y() -= aVisArea.GetHeight() - (GetYScroll() / 2);
    aPos.Y() = Max(0L, aPos.Y());
    SetVisArea( aPos );
    return 1;
}


long SwView::PageDown()
{
    if ( !aVisArea.GetHeight() )
        return 0;
    Point aPos( aVisArea.TopLeft() );
    aPos.Y() += aVisArea.GetHeight() - (GetYScroll() / 2);
    aPos.Y() = SetVScrollMax( aPos.Y() );
    SetVisArea( aPos );
    return 1;
}


long SwView::PhyPageUp()
{
    //aktuell sichtbare Seite erfragen, nicht formatieren
    USHORT nActPage = pWrtShell->GetNextPrevPageNum( FALSE );

    if( USHRT_MAX != nActPage )
    {
        const Point aPt( aVisArea.Left(),
                         pWrtShell->GetPagePos( nActPage ).Y() );
        Point aAlPt( AlignToPixel( aPt ) );
        // falls ein Unterschied besteht, wurde abgeschnitten --> dann
        // einen Pixel addieren, damit kein Rest der Vorgaengerseite
        // sichtbar ist
        if( aPt.Y() != aAlPt.Y() )
            aAlPt.Y() += GetEditWin().PixelToLogic( Size( 0, 1 ) ).Height();
        SetVisArea( aAlPt );
    }
    return 1;
}


long SwView::PhyPageDown()
{
    //aktuell sichtbare Seite erfragen, nicht formatieren
    USHORT nActPage = pWrtShell->GetNextPrevPageNum( TRUE );
    // falls die letzte Dokumentseite sichtbar ist, nichts tun
    if( USHRT_MAX != nActPage )
    {
        const Point aPt( aVisArea.Left(),
                         pWrtShell->GetPagePos( nActPage ).Y() );
        Point aAlPt( AlignToPixel( aPt ) );
        // falls ein Unterschied besteht, wurde abgeschnitten --> dann
        // einen Pixel addieren, damit kein Rest der Vorgaengerseite sichtbar ist
        if( aPt.Y() != aAlPt.Y() )
            aAlPt.Y() += GetEditWin().PixelToLogic( Size( 0, 1 ) ).Height();
        SetVisArea( aAlPt );
    }
    return 1;
}


long SwView::PageUpCrsr( BOOL bSelect )
{
    if ( !bSelect )
    {
        const USHORT eType = pWrtShell->GetFrmType(0,TRUE);
        if ( eType & FRMTYPE_FOOTNOTE )
        {
            pWrtShell->MoveCrsr();
            pWrtShell->GotoFtnAnchor();
            pWrtShell->Right();
            return 1;
        }
    }

    SwTwips lOff = 0;
    if ( GetPageScrollUpOffset( lOff ) &&
         (pWrtShell->IsCrsrReadonly() ||
          !pWrtShell->PageCrsr( lOff, bSelect )) &&
         PageUp() )
    {
        pWrtShell->ResetCursorStack();
        return TRUE;
    }
    return FALSE;
}


long SwView::PageDownCrsr(BOOL bSelect)
{
    SwTwips lOff = 0;
    if ( GetPageScrollDownOffset( lOff ) &&
         (pWrtShell->IsCrsrReadonly() ||
          !pWrtShell->PageCrsr( lOff, bSelect )) &&
         PageDown() )
    {
        pWrtShell->ResetCursorStack();
        return TRUE;
    }
    return FALSE;
}

/*------------------------------------------------------------------------
 Beschreibung:	Handler der Scrollbars
------------------------------------------------------------------------*/

IMPL_LINK( SwView, ScrollHdl, SwScrollbar *, pScrollbar )
{
    if ( GetWrtShell().ActionPend() )
        return 0;

    if ( pScrollbar->GetType() == SCROLL_DRAG )
        pWrtShell->EnableSmooth( FALSE );

    if(!pWrtShell->IsBrowseMode() &&
        pScrollbar->GetType() == SCROLL_DRAG)
    {
        //Hier wieder auskommentieren wenn das mitscrollen nicht gewuenscht ist.
        // JP 21.07.00: the end scrollhandler invalidate the FN_STAT_PAGE,
        // 				so we dont must do it agin.
        EndScrollHdl(pScrollbar);

        Point aPos( aVisArea.TopLeft() );
        lcl_GetPos(this, aPos, pScrollbar, IsDocumentBorder());

        USHORT nPhNum = 1;
        USHORT nVirtNum = 1;

        String sDisplay;
        if(pWrtShell->GetPageNumber( aPos.Y(), FALSE, nPhNum, nVirtNum, sDisplay ))
        {
            // JP 21.07.00: the end scrollhandler invalidate the FN_STAT_PAGE,
            // 				so we dont must do it agin.
//			if(!GetDocShell()->GetProtocol().IsInPlaceActive())
//				S F X_BINDINGS().Update(FN_STAT_PAGE);

            //QuickHelp:
            USHORT nPageCnt = pWrtShell->GetPageCnt();
            if( nPageCnt > 1 && Help::IsQuickHelpEnabled() )
            {
                if( !nPgNum || nPgNum != nPhNum )
                {
                    Rectangle aRect;
                    aRect.Left() = pScrollbar->GetParent()->OutputToScreenPixel(
                                        pScrollbar->GetPosPixel() ).X() -8;
                    aRect.Top() = pScrollbar->OutputToScreenPixel(
                                    pScrollbar->GetPointerPosPixel() ).Y();
                    aRect.Right() 	= aRect.Left();
                    aRect.Bottom()	= aRect.Top();

                    String sPageStr( GetPageStr( nPhNum, nVirtNum, sDisplay ));
                    SwContentAtPos aCnt( SwContentAtPos::SW_OUTLINE );
                    pWrtShell->GetContentAtPos( aPos, aCnt );
                    if( aCnt.sStr.Len() )
                    {
                        sPageStr += String::CreateFromAscii(
                                        RTL_CONSTASCII_STRINGPARAM( "  - " ));
                        sPageStr.Insert( aCnt.sStr, 0, 80 );
                        sPageStr.SearchAndReplaceAll( '\t', ' ' );
                    }

                    Help::ShowQuickHelp( pScrollbar, aRect, sPageStr,
                                    QUICKHELP_RIGHT|QUICKHELP_VCENTER);
                }
                nPgNum = nPhNum;
            }
        }
    }
    else
        EndScrollHdl(pScrollbar);

    if ( pScrollbar->GetType() == SCROLL_DRAG )
        pWrtShell->EnableSmooth( TRUE );

    return 0;
}
/*------------------------------------------------------------------------
 Beschreibung:	Handler der Scrollbars
------------------------------------------------------------------------*/

IMPL_LINK( SwView, EndScrollHdl, SwScrollbar *, pScrollbar )
{
    if ( !GetWrtShell().ActionPend() )
    {
        if(nPgNum)
        {
            nPgNum = 0;
            Help::ShowQuickHelp(pScrollbar, Rectangle(), aEmptyStr, 0);
        }
        Point aPos( aVisArea.TopLeft() );
        FASTBOOL bBorder = IsDocumentBorder();
        lcl_GetPos(this, aPos, pScrollbar, bBorder);
        if ( bBorder && aPos == aVisArea.TopLeft() )
            UpdateScrollbars();
        else
            SetVisArea( aPos, FALSE );

        GetViewFrame()->GetBindings().Update(FN_STAT_PAGE);
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:

        berechnet die Groesse von aVisArea abhaengig von der Groesse
        des EditWin auf dem Schirm.

 --------------------------------------------------------------------*/

void SwView::CalcVisArea( const Size &rOutPixel )
{
    Point aTopLeft;
    Rectangle aRect( aTopLeft, rOutPixel );
    aTopLeft = GetEditWin().PixelToLogic( aTopLeft );
    Point aBottomRight( GetEditWin().PixelToLogic( aRect.BottomRight() ) );

    aRect.Left() = aTopLeft.X();
    aRect.Top() = aTopLeft.Y();
    aRect.Right() = aBottomRight.X();
    aRect.Bottom() = aBottomRight.Y();

    //Die Verschiebungen nach rechts und/oder unten koennen jetzt falsch
    //sein (z.B. Zoom aendern, Viewgroesse aendern.
    const long lBorder = IsDocumentBorder() ? DOCUMENTBORDER : DOCUMENTBORDER*2;
    if ( aRect.Left() )
    {
        const long lWidth = GetWrtShell().GetDocSize().Width() + lBorder;
        if ( aRect.Right() > lWidth )
        {
            long lDelta    = aRect.Right() - lWidth;
            aRect.Left()  -= lDelta;
            aRect.Right() -= lDelta;
        }
    }
    if ( aRect.Top() )
    {
        const long lHeight = GetWrtShell().GetDocSize().Height() + lBorder;
        if ( aRect.Bottom() > lHeight )
        {
            long lDelta		= aRect.Bottom() - lHeight;
            aRect.Top()	   -= lDelta;
            aRect.Bottom() -= lDelta;
        }
    }
    SetVisArea( aRect );
    GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOM );
}

/*--------------------------------------------------------------------
    Beschreibung:	Bedienelemente neu anordnen
 --------------------------------------------------------------------*/


void SwView::CalcAndSetBorderPixel( SvBorder &rToFill, FASTBOOL bInner )
{
    if ( pVLineal )
        rToFill.Left() = pVLineal->GetSizePixel().Width();

    DBG_ASSERT(pHLineal, "warum ist das Lineal nicht da?")
    if ( pHLineal->IsVisible() )
        rToFill.Top() = pHLineal->GetSizePixel().Height();

    const StyleSettings &rSet = GetEditWin().GetSettings().GetStyleSettings();
    const long nTmp = rSet.GetScrollBarSize();
    if ( pVScrollbar && (pVScrollbar->IsVisible() || !pVScrollbar->IsAuto()) )
        rToFill.Right()  = nTmp;

    if ( pHScrollbar && (pHScrollbar->IsVisible() || !pHScrollbar->IsAuto()) )
        rToFill.Bottom() = nTmp;

    SetBorderPixel( rToFill );
}


void ViewResizePixel( const Window &rRef,
                    const Point &rOfst,
                    const Size &rSize,
                    const Size &rEditSz,
                    const BOOL bInner,
                    SwScrollbar* pVScrollbar,
                    SwScrollbar* pHScrollbar,
                    ImageButton* pPageUpBtn,
                    ImageButton* pPageDownBtn,
                    ImageButton* pNaviBtn,
                    Window* pScrollBarBox,
                    SvxRuler* pVLineal,
                    SvxRuler* pHLineal,
                    BOOL bIgnoreVisibility,
                    BOOL bWebView )
{
// ViewResizePixel wird auch von der PreView benutzt!!!

    const BOOL bHLineal = pHLineal && pHLineal->IsVisible();
    const long nHLinSzHeight = bHLineal ?
                        pHLineal->GetSizePixel().Height() : 0;
    const long nVLinSzWidth = pVLineal ?
                        pVLineal->GetSizePixel().Width() : 0;
    long nHBSzHeight2= pHScrollbar && (pHScrollbar->IsVisible() || !pHScrollbar->IsAuto()) ?
                       rRef.GetSettings().GetStyleSettings().GetScrollBarSize() : 0;
    long nHBSzHeight = pHScrollbar && (bIgnoreVisibility || pHScrollbar->IsVisible()) ?
                                nHBSzHeight2:0;
    long nVBSzWidth = pVScrollbar && (pVScrollbar->IsVisible() || !pVScrollbar->IsAuto()) ? rRef.GetSettings().GetStyleSettings().GetScrollBarSize() : 0;

    // Lineale anordnen
    if ( pVLineal )
    {
        Point aPos( rOfst.X(), rOfst.Y()+nHLinSzHeight );
        Size  aSize( nVLinSzWidth, rEditSz.Height() );
        pVLineal->SetPosSizePixel( aPos, aSize );
    }
//  Lineal braucht ein Resize, sonst funktioniert es nicht im unischtbaren Zustand
//	if ( bHLineal )
    if ( pHLineal ) 	//MA: In der Seitenansicht gibt es das Lineal nicht!
    {
        Size aSize( rSize.Width(), nHLinSzHeight );
        if ( nVBSzWidth )
            aSize.Width() -= nVBSzWidth;
        if(!aSize.Height())
            aSize.Height() = pHLineal->GetSizePixel().Height();
        pHLineal->SetPosSizePixel( rOfst, aSize );
//		#46802 VCL ruft an unsichtbaren Fenstern kein Resize
//      fuer das Lineal ist das aber keine gute Idee
        if(!pHLineal->IsVisible())
            pHLineal->Resize();
    }

    // Scrollbars und SizeBox anordnen
    Point aScrollFillPos;
    if ( pHScrollbar && (pHScrollbar->IsVisible() || !pHScrollbar->IsAuto()) )
    {
        Point aPos( rOfst.X(),
                    rOfst.Y()+rSize.Height()-nHBSzHeight );
        Size  aSize( rSize.Width(), nHBSzHeight2 );
        if ( nVBSzWidth )
            aSize.Width() -= nVBSzWidth;
        pHScrollbar->SetPosSizePixel( aPos, aSize );
        aScrollFillPos.Y() = aPos.Y();
    }
    if ( pVScrollbar && (pVScrollbar->IsVisible()|| !pVScrollbar->IsAuto()))
    {
        Point aPos( rOfst.X()+rSize.Width()-nVBSzWidth,
                    rOfst.Y() );
        Size  aSize( nVBSzWidth, rSize.Height() );
        Size  aImgSz( nVBSzWidth, nVBSzWidth );

        //#55949#  wenn der Platz fuer Scrollbar und Page-Buttons zu klein wird, dann
        // werden die Buttons versteckt
        USHORT nCnt = pNaviBtn ? 3 : 2;
        long nSubSize = (aImgSz.Width() * nCnt );
        //
        BOOL bHidePageButtons = aSize.Height() < ((bWebView ? 3 : 2) * nSubSize);
        if(!bHidePageButtons)
            aSize.Height() -= nSubSize;
        else
            aImgSz.Width() = 0;	// kein Hide, weil das im Update Scrollbar missverstanden wird

        if ( nHBSzHeight )
            aSize.Height() -= nHBSzHeight;
        pVScrollbar->SetPosSizePixel( aPos, aSize );

        aPos.Y() += aSize.Height();
        pPageUpBtn->SetPosSizePixel( aPos, aImgSz );
        if(pNaviBtn)
        {
            aPos.Y() += aImgSz.Height();
            pNaviBtn->SetPosSizePixel(aPos, aImgSz);
        }

        aPos.Y() += aImgSz.Height();
        pPageDownBtn->SetPosSizePixel( aPos, aImgSz );


        if( pHScrollbar )
        {
            aScrollFillPos.X() = aPos.X();
            pScrollBarBox->SetPosSizePixel( aScrollFillPos,
                                         Size( nHBSzHeight,	nVBSzWidth) );
        }
    }
}


void SwView::ShowAtResize()
{
    const FASTBOOL bBrowse = pWrtShell->IsBrowseMode();
    bShowAtResize = FALSE;
    if ( pVLineal )
        pVLineal->Show();
    DBG_ASSERT(pHLineal, "warum ist das Lineal nicht da?")
    if ( pWrtShell->GetViewOptions()->IsViewTabwin() )
        pHLineal->Show();
    if ( pHScrollbar && (!bBrowse ||
                         GetDocShell()->GetProtocol().IsInPlaceActive()) )
        pHScrollbar->Show();
    if ( pVScrollbar )
    {
        pVScrollbar->Show();
        if(pPageUpBtn)
        {
            pPageUpBtn->Show();
            pPageDownBtn->Show();
        }
        if(pNaviBtn)
            pNaviBtn->Show();
        if ( !bBrowse && pScrollFill )
            pScrollFill->Show();
    }
}


void SwView::InnerResizePixel( const Point &rOfst, const Size &rSize )
{
    SvBorder aBorder;
    CalcAndSetBorderPixel( aBorder, TRUE );
    Size aSz( rSize );
    aSz.Height() += aBorder.Top()  + aBorder.Bottom();
    aSz.Width()  += aBorder.Left() + aBorder.Right();
    Size aEditSz( GetEditWin().GetOutputSizePixel() );
    const BOOL bBrowse = pWrtShell->IsBrowseMode();
    ViewResizePixel( GetEditWin(), rOfst, aSz, aEditSz, TRUE, pVScrollbar,
                            pHScrollbar, pPageUpBtn, pPageDownBtn,
                            pNaviBtn,
                            pScrollFill, pVLineal, pHLineal,
                            (!bBrowse || GetDocShell()->GetProtocol().IsInPlaceActive()),
                            0 != PTR_CAST(SwWebView, this));
    if ( bShowAtResize )
        ShowAtResize();

    if( pHLineal->IsVisible() || pVLineal )
    {
        const Fraction& rFrac = GetEditWin().GetMapMode().GetScaleX();
        USHORT nZoom = USHORT(rFrac.GetNumerator() * 100L / rFrac.GetDenominator());

        const Fraction aFrac( nZoom, 100 );
        if ( pVLineal )
            pVLineal->SetZoom( aFrac );
        DBG_ASSERT(pHLineal, "warum ist das Lineal nicht da?")
        if ( pHLineal->IsVisible() )
            pHLineal->SetZoom( aFrac );
        InvalidateRulerPos();	//Inhalt invalidieren.
    }
    //CursorStack zuruecksetzen, da die Cursorpositionen fuer PageUp/-Down
    //nicht mehr zum aktuell sichtbaren Bereich passen
    pWrtShell->ResetCursorStack();

    //EditWin niemals einstellen!

    //VisArea einstellen, aber dort nicht das SetVisArea der DocShell rufen!
    bProtectDocShellVisArea = TRUE;
    CalcVisArea( aEditSz );
    bProtectDocShellVisArea = FALSE;
}


void SwView::OuterResizePixel( const Point &rOfst, const Size &rSize )
{
    if( bInOuterResizePixel )
        return;
    bInOuterResizePixel	= TRUE;

// feststellen, ob Scrollbars angezeigt werden duerfen
    FASTBOOL bBrowse = pWrtShell->IsBrowseMode();
    FASTBOOL bShowH,
             bShowV,
             bAuto = FALSE,
             bHAuto= bBrowse;
    switch( GetScrollingMode() )
    {
    case SCROLLING_DEFAULT:
        {
            const SwViewOption *pVOpt = pWrtShell->GetViewOptions();
            if ( !pVOpt->IsReadonly() || pVOpt->IsStarOneSetting() )
            {
                bShowH = pVOpt->IsViewHScrollBar();
                bShowV = pVOpt->IsViewVScrollBar();
                break;
            }
        }
        /* kein break hier */
    case SCROLLING_AUTO:
        bAuto = bHAuto = TRUE;
        bShowH = bShowV = TRUE;
        break;
    case SCROLLING_YES:
        bShowH = bShowV = TRUE;
        break;
    case SCROLLING_NO:
        bShowH = bShowV = bHAuto = FALSE;
        break;
    }
    SwDocShell* pDocSh = GetDocShell();
    BOOL bIsPreview = pDocSh->IsPreview();
    if( bIsPreview )
    {
        bShowH = bShowV = bHAuto = FALSE;
    }
    if ( bShowH != StatHScrollbar() )
        bShowH ? CreateHScrollbar() : KillHScrollbar();
    if ( pHScrollbar )
    {
        pHScrollbar->SetUpdateMode(FALSE);
        pHScrollbar->SetAuto( bHAuto );
    }
    if(bShowV != StatVScrollbar())
        bShowV ? CreateVScrollbar() : KillVScrollbar();
    if(pVScrollbar)
    {
        pVScrollbar->SetUpdateMode(FALSE);
        pVScrollbar->SetAuto(bAuto);
    }

    SET_CURR_SHELL( pWrtShell );
    FASTBOOL bRepeat = FALSE;
    long nCnt = 0;

    BOOL bUnLockView = !pWrtShell->IsViewLocked();
    pWrtShell->LockView( TRUE );
    pWrtShell->LockPaint();

    do {
        ++nCnt;
        const FASTBOOL bScroll1 = pVScrollbar ? pVScrollbar->IsVisible() : FALSE;
        const FASTBOOL bScroll2 = pHScrollbar ? pHScrollbar->IsVisible() : FALSE;
        SvBorder aBorder;
        CalcAndSetBorderPixel( aBorder, FALSE );
        const Size aEditSz( GetEditWin().GetOutputSizePixel() );
        ViewResizePixel( GetEditWin(), rOfst, rSize, aEditSz, FALSE, pVScrollbar,
                                pHScrollbar, pPageUpBtn, pPageDownBtn,
                                pNaviBtn,
                                pScrollFill, pVLineal, pHLineal, !bBrowse,
                                0 != PTR_CAST(SwWebView, this) );
        if ( bShowAtResize )
            ShowAtResize();

        if( pHLineal->IsVisible() || pVLineal )
            InvalidateRulerPos();	//Inhalt invalidieren.

        //CursorStack zuruecksetzen, da die Cursorpositionen fuer PageUp/-Down
        //nicht mehr zum aktuell sichtbaren Bereich passen
        pWrtShell->ResetCursorStack();

        ASSERT( ( aEditSz.Width() > 0 && aEditSz.Height() > 0 )
                || !aVisArea.IsEmpty(), "Small world, isn't it?" );

        //EditWin niemals einstellen!

        //Die VisArea muss aber natuerlich eingestellt werden.
        //jetzt ist auch der richtige Zeitpunkt den Zoom neu zu berechnen wenn
        //es kein einfacher Faktor ist.
        pWrtShell->StartAction();
        CalcVisArea( aEditSz );

        //Damit auch beim outplace editing die Seitenbreite sofort
        //angepasst wird.
        if ( pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
            pDocSh->SetVisArea(
                            pDocSh->SfxInPlaceObject::GetVisArea() );
        if ( pWrtShell->GetViewOptions()->GetZoomType() != SVX_ZOOM_PERCENT &&
             !pWrtShell->IsBrowseMode() )
            _SetZoom( aEditSz, (SvxZoomType)pWrtShell->GetViewOptions()->GetZoomType() );
        pWrtShell->EndAction();

        bRepeat = bScroll1 != (pVScrollbar ? pVScrollbar->IsVisible() : FALSE);
        if ( !bRepeat )
            bRepeat = bScroll2 != (pHScrollbar ? pHScrollbar->IsVisible() : FALSE);

        //Nicht endlosschleifen. Moeglichst dann stoppen wenn die
        //(Auto-)Scrollbars sichtbar sind.
        if ( bRepeat && nCnt > 10 ||
             (nCnt > 3 && bHAuto && bAuto &&
              (pVScrollbar ? pVScrollbar->IsVisible() == bAuto  : TRUE) &&
              (pHScrollbar ? pHScrollbar->IsVisible() == bHAuto : TRUE)) )
        {
            bRepeat = FALSE;
        }

    }while ( bRepeat );

    if( pHScrollbar )
        pHScrollbar->SetUpdateMode(TRUE);

    if( pVScrollbar )
    {
        pVScrollbar->SetUpdateMode(TRUE);
        BOOL bShowButtons = pVScrollbar->IsVisible(TRUE);
        if(pPageUpBtn && pPageUpBtn->IsVisible() != bShowButtons)
        {
            pPageUpBtn->Show(bShowButtons);
            if(pPageDownBtn)
                pPageDownBtn->Show(bShowButtons);
            if(pNaviBtn)
                pNaviBtn->Show(bShowButtons);
        }
    }

    pWrtShell->UnlockPaint();
    if( bUnLockView )
        pWrtShell->LockView( FALSE );

    bInOuterResizePixel	= FALSE;
}


void SwView::SetZoomFactor( const Fraction &rX, const Fraction &rY )
{
    const Fraction &rFrac = rX < rY ? rX : rY;
    SetZoom( SVX_ZOOM_PERCENT, (short) long(rFrac * Fraction( 100, 1 )) );

    //Um Rundungsfehler zu minimieren lassen wir von der Basisklasse ggf.
    //auch die krummen Werte einstellen
    SfxViewShell::SetZoomFactor( rX, rY );
}


Size SwView::GetOptimalSizePixel() const
{
    Size aPgSize;
    if ( pWrtShell->IsBrowseMode() )
    {
        aPgSize.Height() = lA4Height;
        aPgSize.Width()  = lA4Width;
    }
    else
    {
        const SwPageDesc &rDesc = pWrtShell->GetPageDesc(
                                                    pWrtShell->GetCurPageDesc() );
        const SvxLRSpaceItem &rLRSpace = rDesc.GetMaster().GetLRSpace();
        aPgSize = GetWrtShell().GetAnyCurRect(RECT_PAGE).SSize();
        if( PD_MIRROR == rDesc.GetUseOn() )
        {
            const SvxLRSpaceItem &rLeftLRSpace = rDesc.GetLeft().GetLRSpace();
            aPgSize.Width() +=
                Abs( long(rLeftLRSpace.GetLeft()) - long(rLRSpace.GetLeft()) );
        }
        aPgSize.Width() += DOCUMENTBORDER * 2;
    }
    return GetEditWin().LogicToPixel( aPgSize );
}


BOOL SwView::UpdateScrollbars()
{
    BOOL bRet = FALSE;
    if ( !aVisArea.IsEmpty() && (pHScrollbar || pVScrollbar) )
    {
        const FASTBOOL bBorder = IsDocumentBorder();
        Rectangle aTmpRect( aVisArea );
        if ( bBorder )
        {
            Point aPt( DOCUMENTBORDER, DOCUMENTBORDER );
            aPt = AlignToPixel( aPt );
            aTmpRect.Move( -aPt.X(), -aPt.Y() );
        }

        Size aTmpSz( aDocSz );
        const long lOfst = bBorder ? 0 : DOCUMENTBORDER * 2L;
        aTmpSz.Width() += lOfst; aTmpSz.Height() += lOfst;

        if ( pVScrollbar )
        {
            const BOOL bVis = pVScrollbar->IsVisible();
            pVScrollbar->DocSzChgd( aTmpSz );
            pVScrollbar->ViewPortChgd( aTmpRect );

            BOOL bShowButtons = pVScrollbar->IsVisible(TRUE);
            if(pPageUpBtn && pPageUpBtn->IsVisible() != bShowButtons)
            {
                pPageUpBtn->Show(bShowButtons);
                if(pPageDownBtn)
                    pPageDownBtn->Show(bShowButtons);
                if(pNaviBtn)
                    pNaviBtn->Show(bShowButtons);
            }

            if ( !bVis && !bShowAtResize && !pVScrollbar->IsAuto() )
                pVScrollbar->Show();
            if ( bVis != pVScrollbar->IsVisible() )
                bRet = TRUE;
        }
        if ( pHScrollbar )
        {
            const BOOL bVis = pHScrollbar->IsVisible();
            pHScrollbar->DocSzChgd( aTmpSz );
            pHScrollbar->ViewPortChgd( aTmpRect );
            if ( !bVis && !bShowAtResize && !pHScrollbar->IsAuto() )
                pHScrollbar->Show();
            if ( bVis != pHScrollbar->IsVisible() )
                bRet = TRUE;
        }
        if ( pScrollFill )
        {
            if ( pHScrollbar && pVScrollbar &&
                 pHScrollbar->IsVisible() && pVScrollbar->IsVisible() )
            {
                pScrollFill->Show();
            }
            else
                pScrollFill->Hide();
        }
    }
    return bRet;
}


void SwView::Move()
{
    if ( GetWrtShell().IsInSelect() )
        GetWrtShell().EndSelect();	//#32427#
    SfxViewShell::Move();
}

BOOL SwView::HandleWheelCommands( const CommandEvent& rCEvt )
{
    BOOL bOk = FALSE;
    const CommandWheelData* pWData = rCEvt.GetWheelData();
    if( pWData && COMMAND_WHEEL_ZOOM == pWData->GetMode() )
    {
        USHORT nFact = pWrtShell->GetViewOptions()->GetZoom();
        if( 0L > pWData->GetDelta() )
            nFact = Max( 20, nFact - 10 );
        else
            nFact = Min( 600, nFact + 10 );

        SetZoom( SVX_ZOOM_PERCENT, nFact );
        bOk = TRUE;
    }
    else
        bOk = pEditWin->HandleScrollCommand( rCEvt,
                    pHScrollbar && pHScrollbar->IsVisible(TRUE) ? pHScrollbar : 0,
                    pVScrollbar && pVScrollbar->IsVisible(TRUE) ? pVScrollbar : 0 );
    return bOk;
}


