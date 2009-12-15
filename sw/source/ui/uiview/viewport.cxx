/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewport.cxx,v $
 * $Revision: 1.47 $
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


#include "hintids.hxx"
#include <vcl/help.hxx>
#include <svx/ruler.hxx>
#include <svx/paperinf.hxx>
#include <svx/lrspitem.hxx>
#include <sfx2/bindings.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <wrtsh.hxx>
#include <swmodule.hxx>
#include <viewopt.hxx>
#include <frmatr.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <edtwin.hxx>
#include <scroll.hxx>
#ifndef _WVIEW_HXX
#include <wview.hxx>
#endif
#include <usrpref.hxx>
#include <pagedesc.hxx>
#include <workctrl.hxx>
#include <crsskip.hxx>

#include <PostItMgr.hxx>

#include <IDocumentSettingAccess.hxx>

//Das SetVisArea der DocShell darf nicht vom InnerResizePixel gerufen werden.
//Unsere Einstellungen muessen aber stattfinden.
#ifndef WB_RIGHT_ALIGNED
#define WB_RIGHT_ALIGNED    ((WinBits)0x00008000)
#endif

static BOOL bProtectDocShellVisArea = FALSE;

static USHORT nPgNum = 0;

BOOL SwView::IsDocumentBorder()
{
    return GetDocShell()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED ||
           pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) ||
           SVX_ZOOM_PAGEWIDTH_NOBORDER == (SvxZoomType)pWrtShell->GetViewOptions()->GetZoomType();
}

inline long GetLeftMargin( SwView &rView )
{
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
                BOOL bBorder)
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
        SID_RULER_BORDER_DISTANCE,
        SID_ATTR_PARA_LRSPACE_VERTICAL, SID_RULER_BORDERS_VERTICAL,
        SID_RULER_TEXT_RIGHT_TO_LEFT,
        SID_RULER_ROWS, SID_RULER_ROWS_VERTICAL, FN_STAT_PAGE,
        0
    };

    GetViewFrame()->GetBindings().Invalidate(aInval);

    DBG_ASSERT(pHRuler, "warum ist das Lineal nicht da?");
    pHRuler->ForceUpdate();
    pVRuler->ForceUpdate();
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
    BOOL bModified = false;
    SwTwips lGreenOffset = IsDocumentBorder() ? DOCUMENTBORDER : DOCUMENTBORDER * 2;
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

    if ( UpdateScrollbars() && !bInOuterResizePixel && !bInInnerResizePixel &&
            !GetViewFrame()->GetFrame().IsInPlace())
        OuterResizePixel( Point(),
                          GetViewFrame()->GetWindow().GetOutputSizePixel() );
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
    //sichergestellt, da? anliegende Paints korrekt in Dokumentkoordinaten
    //umgerechnet werden.
    //Vorsichtshalber tun wir das nur wenn an der Shell eine Action laeuft,
    //denn dann wir nicht wirklich gepaintet sondern die Rechtecke werden
    //lediglich (in Dokumentkoordinaten) vorgemerkt.
    if ( pWrtShell && pWrtShell->ActionPend() )
        pWrtShell->GetWin()->Update();

    aVisArea = aLR;

    const BOOL bOuterResize = bUpdateScrollbar && UpdateScrollbars();

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
            aVis.SetSize( GetDocShell()->SfxObjectShell::GetVisArea(ASPECT_CONTENT).GetSize() );
                    // TODO/LATER: why casting?!
                    //GetDocShell()->SfxInPlaceObject::GetVisArea().GetSize() );

        //Bei embedded immer mit Modify...
        // TODO/LATER: why casting?!
        GetDocShell()->SfxObjectShell::SetVisArea( aVis );
        /*
        if ( GetDocShell()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
            GetDocShell()->SfxInPlaceObject::SetVisArea( aVis );
        else
            GetDocShell()->SvEmbeddedObject::SetVisArea( aVis );*/
    }

    SfxViewShell::VisAreaChanged( aVisArea );

    InvalidateRulerPos();

    SwEditWin::ClearTip();

    if ( bOuterResize && !bInOuterResizePixel && !bInInnerResizePixel)
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
    pHScrollbar->SetAuto( pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) &&
                              !GetViewFrame()->GetFrame().IsInPlace() );
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
        pPt->X() = rRect.Left() - (nRangeX != USHRT_MAX ? nRangeX : nXScroll);
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
                pCareWn->GetWindowExtentsRelative( &GetEditWin() ) ) );
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
                static_cast< USHORT >((aVisSize.Width() - aSize.Width()) / 2),
                static_cast< USHORT >((aVisSize.Height()- aSize.Height())/ 2) );

        if( bTopCrsr )
        {
            const long nBorder = IsDocumentBorder() ? DOCUMENTBORDER : 0;
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
            const long nBorder = IsDocumentBorder() ? DOCUMENTBORDER : 0;
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
    long nYScrl = GetYScroll() / 2;
    rOff = -(aVisArea.GetHeight() - nYScrl);
    //nicht vor den Dokumentanfang scrollen
    if( aVisArea.Top() - rOff < 0 )
        rOff = rOff - aVisArea.Top();
    else if( GetWrtShell().GetCharRect().Top() < (aVisArea.Top() + nYScrl))
        rOff += nYScrl;
    return TRUE;
}


BOOL SwView::GetPageScrollDownOffset( SwTwips &rOff ) const
{
    if ( !aVisArea.GetHeight() ||
         (aVisArea.GetHeight() > aDocSz.Height()) )
        return FALSE;
    long nYScrl = GetYScroll() / 2;
    rOff = aVisArea.GetHeight() - nYScrl;
    //nicht hinter das Dokumentende scrollen
    if ( aVisArea.Top() + rOff > aDocSz.Height() )
        rOff = aDocSz.Height() - aVisArea.Bottom();
    else if( GetWrtShell().GetCharRect().Bottom() >
                                            ( aVisArea.Bottom() - nYScrl ))
        rOff -= nYScrl;
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
            aAlPt.Y() += 3 * GetEditWin().PixelToLogic( Size( 0, 1 ) ).Height();
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
            aAlPt.Y() += 3 * GetEditWin().PixelToLogic( Size( 0, 1 ) ).Height();
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
            pWrtShell->Right(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
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

    if(!pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) &&
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
//          if(!GetViewFrame()->GetFrame().IsInPlace())
//				S F X_BINDINGS().Update(FN_STAT_PAGE);

            //QuickHelp:
            if( pWrtShell->GetPageCnt() > 1 && Help::IsQuickHelpEnabled() )
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
                        sPageStr.SearchAndReplaceAll( 0x0a, ' ' );
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
        BOOL bBorder = IsDocumentBorder();
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
    GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOMSLIDER ); // for snapping points
}

/*--------------------------------------------------------------------
    Beschreibung:	Bedienelemente neu anordnen
 --------------------------------------------------------------------*/


void SwView::CalcAndSetBorderPixel( SvBorder &rToFill, BOOL /*bInner*/ )
{
    BOOL bRightVRuler = pWrtShell->GetViewOptions()->IsVRulerRight();
    if ( pVRuler->IsVisible() )
    {
        long nWidth = pVRuler->GetSizePixel().Width();
        if(bRightVRuler)
            rToFill.Right() = nWidth;
        else
            rToFill.Left() = nWidth;
    }

    DBG_ASSERT(pHRuler, "warum ist das Lineal nicht da?");
    if ( pHRuler->IsVisible() )
        rToFill.Top() = pHRuler->GetSizePixel().Height();

    const StyleSettings &rSet = GetEditWin().GetSettings().GetStyleSettings();
    const long nTmp = rSet.GetScrollBarSize();
    if( pVScrollbar->IsVisible(FALSE) )
    {
        if(bRightVRuler)
            rToFill.Left() = nTmp;
        else
            rToFill.Right()  = nTmp;
    }
    //#i32913# in browse mode the visibility of the horizontal scrollbar
    // depends on the content (fixed width tables may require a scrollbar)
    if ( pHScrollbar->IsVisible(pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE)) )
        rToFill.Bottom() = nTmp;

    SetBorderPixel( rToFill );
}


void ViewResizePixel( const Window &rRef,
                    const Point &rOfst,
                    const Size &rSize,
                    const Size &rEditSz,
                    const BOOL /*bInner*/,
                    SwScrollbar& rVScrollbar,
                    SwScrollbar& rHScrollbar,
                    ImageButton* pPageUpBtn,
                    ImageButton* pPageDownBtn,
                    ImageButton* pNaviBtn,
                    Window& rScrollBarBox,
                    SvxRuler* pVLineal,
                    SvxRuler* pHLineal,
                    BOOL bWebView,
                    BOOL bVRulerRight )
{
// ViewResizePixel wird auch von der PreView benutzt!!!

    const BOOL bHLineal = pHLineal && pHLineal->IsVisible();
    const long nHLinSzHeight = bHLineal ?
                        pHLineal->GetSizePixel().Height() : 0;
    const BOOL bVLineal = pVLineal && pVLineal->IsVisible();
    const long nVLinSzWidth = bVLineal ?
                        pVLineal->GetSizePixel().Width() : 0;
    long nHBSzHeight2= rHScrollbar.IsVisible( FALSE ) || !rHScrollbar.IsAuto() ?
                       rRef.GetSettings().GetStyleSettings().GetScrollBarSize() : 0;
    long nHBSzHeight =
                rHScrollbar.IsVisible(TRUE) ||  (rHScrollbar.IsVisible( FALSE ) && !rHScrollbar.IsAuto()) ?
                                nHBSzHeight2:0;
    long nVBSzWidth = rVScrollbar.IsVisible(TRUE) ||  (rVScrollbar.IsVisible( FALSE ) && !rVScrollbar.IsAuto()) ?
                         rRef.GetSettings().GetStyleSettings().GetScrollBarSize() : 0;

    if(pVLineal)
    {
        WinBits nStyle = pVLineal->GetStyle()&~WB_RIGHT_ALIGNED;
        Point aPos( rOfst.X(), rOfst.Y()+nHLinSzHeight );
        if(bVRulerRight)
        {
            aPos.X() += rSize.Width() - nVLinSzWidth;
            nStyle |= WB_RIGHT_ALIGNED;
        }
        Size  aSize( nVLinSzWidth, rEditSz.Height() );
        if(!aSize.Width())
            aSize.Width() = pVLineal->GetSizePixel().Width();
        pVLineal->SetStyle(nStyle);
        pVLineal->SetPosSizePixel( aPos, aSize );
        if(!pVLineal->IsVisible())
            pVLineal->Resize();
    }
//  Lineal braucht ein Resize, sonst funktioniert es nicht im unischtbaren Zustand
    if(pHLineal)
    {
        Size aSize( rSize.Width(), nHLinSzHeight );
        if ( nVBSzWidth && !bVRulerRight)
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
    {
        Point aPos( rOfst.X(),
                    rOfst.Y()+rSize.Height()-nHBSzHeight );
        if(bVRulerRight)
        {
            aPos.X() += nVBSzWidth;
        }

        Size  aSize( rSize.Width(), nHBSzHeight2 );
        if ( nVBSzWidth )
            aSize.Width() -= nVBSzWidth;
        rHScrollbar.SetPosSizePixel( aPos, aSize );
        aScrollFillPos.Y() = aPos.Y();
    }
    {
        Point aPos( rOfst.X()+rSize.Width()-nVBSzWidth,
                    rOfst.Y() );
        Size  aSize( nVBSzWidth, rSize.Height() );
        if(bVRulerRight)
        {
            aPos.X() = rOfst.X();
            if(bHLineal)
            {
                aPos.Y() += nHLinSzHeight;
                aSize.Height() -= nHLinSzHeight;
            }
        }

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
        rVScrollbar.SetPosSizePixel( aPos, aSize );

        aPos.Y() += aSize.Height();
        pPageUpBtn->SetPosSizePixel( aPos, aImgSz );
        if(pNaviBtn)
        {
            aPos.Y() += aImgSz.Height();
            pNaviBtn->SetPosSizePixel(aPos, aImgSz);
        }

        aPos.Y() += aImgSz.Height();
        pPageDownBtn->SetPosSizePixel( aPos, aImgSz );


        if( rHScrollbar.IsVisible( FALSE ) )
        {
            aScrollFillPos.X() = aPos.X();

            rScrollBarBox.SetPosSizePixel( aScrollFillPos,
                                         Size( nHBSzHeight,	nVBSzWidth) );
        }
    }
}


void SwView::ShowAtResize()
{
    bShowAtResize = FALSE;
    if ( pWrtShell->GetViewOptions()->IsViewHRuler() )
        pHRuler->Show();
}


void SwView::InnerResizePixel( const Point &rOfst, const Size &rSize )
{
    Size aObjSize = GetObjectShell()->GetVisArea().GetSize();
    if ( aObjSize.Width() > 0 && aObjSize.Height() > 0 )
    {
        SvBorder aBorder( GetBorderPixel() );
        Size aSize( rSize );
        aSize.Width() -= (aBorder.Left() + aBorder.Right());
        aSize.Height() -= (aBorder.Top() + aBorder.Bottom());
        Size aObjSizePixel = GetWindow()->LogicToPixel( aObjSize, MAP_TWIP );
        SfxViewShell::SetZoomFactor( Fraction( aSize.Width(), aObjSizePixel.Width() ),
                        Fraction( aSize.Height(), aObjSizePixel.Height() ) );
    }

    bInInnerResizePixel = TRUE;
    const BOOL bHScrollVisible = pHScrollbar->IsVisible(TRUE);
    const BOOL bVScrollVisible = pVScrollbar->IsVisible(TRUE);
    BOOL bRepeat = FALSE;
    do
    {
        Size aSz( rSize );
        SvBorder aBorder;
        CalcAndSetBorderPixel( aBorder, TRUE );
        if ( GetViewFrame()->GetFrame().IsInPlace() )
        {
            Size aViewSize( aSz );
            Point aViewPos( rOfst );
            aViewSize.Height() -= (aBorder.Top() + aBorder.Bottom());
            aViewSize.Width()  -= (aBorder.Left() + aBorder.Right());
            aViewPos.X() += aBorder.Left();
            aViewPos.Y() += aBorder.Top();
            GetEditWin().SetPosSizePixel( aViewPos, aViewSize );
        }
        else
        {
            aSz.Height() += aBorder.Top()  + aBorder.Bottom();
            aSz.Width()  += aBorder.Left() + aBorder.Right();
        }

        Size aEditSz( GetEditWin().GetOutputSizePixel() );
        ViewResizePixel( GetEditWin(), rOfst, aSz, aEditSz, TRUE, *pVScrollbar,
                            *pHScrollbar, pPageUpBtn, pPageDownBtn,
                            pNaviBtn,
                            *pScrollFill, pVRuler, pHRuler,
                            0 != PTR_CAST(SwWebView, this),
                            pWrtShell->GetViewOptions()->IsVRulerRight());
        if ( bShowAtResize )
            ShowAtResize();

        if( pHRuler->IsVisible() || pVRuler->IsVisible() )
        {
            const Fraction& rFrac = GetEditWin().GetMapMode().GetScaleX();
            USHORT nZoom = 100;
            if (0 != rFrac.GetDenominator())
                nZoom = USHORT(rFrac.GetNumerator() * 100L / rFrac.GetDenominator());

            const Fraction aFrac( nZoom, 100 );
            pVRuler->SetZoom( aFrac );
            pHRuler->SetZoom( aFrac );
            InvalidateRulerPos();	//Inhalt invalidieren.
        }
        //CursorStack zuruecksetzen, da die Cursorpositionen fuer PageUp/-Down
        //nicht mehr zum aktuell sichtbaren Bereich passen
        pWrtShell->ResetCursorStack();

        //EditWin niemals einstellen!

        //VisArea einstellen, aber dort nicht das SetVisArea der DocShell rufen!
        bProtectDocShellVisArea = TRUE;
        CalcVisArea( aEditSz );
        //visibility changes of the automatic horizontal scrollbar
        //require to repeat the ViewResizePixel() call - but only once!
        if(bRepeat)
            bRepeat = FALSE;
        else if(bHScrollVisible != pHScrollbar->IsVisible(TRUE) ||
                bVScrollVisible != pVScrollbar->IsVisible(TRUE))
            bRepeat = TRUE;
    }while( bRepeat );
    bProtectDocShellVisArea = FALSE;
    bInInnerResizePixel = FALSE;
}


void SwView::OuterResizePixel( const Point &rOfst, const Size &rSize )
{
    // FME 22.08.2003 #i16909# - return, if no size (caused by minimize window).
    if ( bInOuterResizePixel || ( !rSize.Width() && !rSize.Height() ) )
        return;
    bInOuterResizePixel	= TRUE;

// feststellen, ob Scrollbars angezeigt werden duerfen
    BOOL bBrowse = pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE);
    BOOL bShowH = FALSE,
         bShowV = FALSE,
         bAuto  = FALSE,
         bHAuto = bBrowse;
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
        bShowH = bShowV = bHAuto = bAuto = FALSE;
    }
    if(pHScrollbar->IsVisible(FALSE) != bShowH)
        ShowHScrollbar(bShowH);
    pHScrollbar->SetAuto( bHAuto );
    if(pVScrollbar->IsVisible(FALSE) != bShowV)
        ShowVScrollbar(bShowV);
    pVScrollbar->SetAuto(bAuto);

    SET_CURR_SHELL( pWrtShell );
    BOOL bRepeat = FALSE;
    long nCnt = 0;

    BOOL bUnLockView = !pWrtShell->IsViewLocked();
    pWrtShell->LockView( TRUE );
    pWrtShell->LockPaint();

    do {
        ++nCnt;
        const BOOL bScroll1 = pVScrollbar->IsVisible(TRUE);
        const BOOL bScroll2 = pHScrollbar->IsVisible(TRUE);
        SvBorder aBorder;
        CalcAndSetBorderPixel( aBorder, FALSE );
        const Size aEditSz( GetEditWin().GetOutputSizePixel() );
        ViewResizePixel( GetEditWin(), rOfst, rSize, aEditSz, FALSE, *pVScrollbar,
                                *pHScrollbar, pPageUpBtn, pPageDownBtn,
                                pNaviBtn,
                                *pScrollFill, pVRuler, pHRuler,
                                0 != PTR_CAST(SwWebView, this),
                                pWrtShell->GetViewOptions()->IsVRulerRight() );
        if ( bShowAtResize )
            ShowAtResize();

        if( pHRuler->IsVisible() || pVRuler->IsVisible() )
            InvalidateRulerPos();	//Inhalt invalidieren.

        //CursorStack zuruecksetzen, da die Cursorpositionen fuer PageUp/-Down
        //nicht mehr zum aktuell sichtbaren Bereich passen
        pWrtShell->ResetCursorStack();

        ASSERT( !GetEditWin().IsVisible() ||
                    (( aEditSz.Width() > 0 && aEditSz.Height() > 0 )
                        || !aVisArea.IsEmpty()), "Small world, isn't it?" );

        //EditWin niemals einstellen!

        //Die VisArea muss aber natuerlich eingestellt werden.
        //jetzt ist auch der richtige Zeitpunkt den Zoom neu zu berechnen wenn
        //es kein einfacher Faktor ist.
        pWrtShell->StartAction();
        CalcVisArea( aEditSz );

        //Damit auch beim outplace editing die Seitenbreite sofort
        //angepasst wird.
        //TODO/LATER: is that still necessary?!
        /*
        if ( pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
            pDocSh->SetVisArea(
                            pDocSh->SfxInPlaceObject::GetVisArea() );*/
        if ( pWrtShell->GetViewOptions()->GetZoomType() != SVX_ZOOM_PERCENT &&
             !pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) )
            _SetZoom( aEditSz, (SvxZoomType)pWrtShell->GetViewOptions()->GetZoomType(), 100, TRUE );
        pWrtShell->EndAction();

        bRepeat = bScroll1 != pVScrollbar->IsVisible(TRUE);
        if ( !bRepeat )
            bRepeat = bScroll2 != pHScrollbar->IsVisible(TRUE);

        //Nicht endlosschleifen. Moeglichst dann stoppen wenn die
        //(Auto-)Scrollbars sichtbar sind.
        if ( bRepeat && 
             ( nCnt > 10 || ( nCnt > 3 && bHAuto && bAuto ) )
           )
        {
            bRepeat = FALSE;
        }

    }while ( bRepeat );

    if( pVScrollbar->IsVisible(FALSE) || pVScrollbar->IsAuto())
    {
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

    bInOuterResizePixel = FALSE;

    if ( mpPostItMgr )
    {
        mpPostItMgr->CalcRects();
        mpPostItMgr->LayoutPostIts();
    }
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
    if ( pWrtShell->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) )
        aPgSize = SvxPaperInfo::GetPaperSize(PAPER_A4);
    else
    {
        aPgSize = GetWrtShell().GetAnyCurRect(RECT_PAGE).SSize();
        aPgSize.Width() += DOCUMENTBORDER * 2;

        const SwPageDesc &rDesc = pWrtShell->GetPageDesc( pWrtShell->GetCurPageDesc() );
        if( nsUseOnPage::PD_MIRROR == rDesc.GetUseOn() )
        {
            const SvxLRSpaceItem &rLRSpace = rDesc.GetMaster().GetLRSpace();
            const SvxLRSpaceItem &rLeftLRSpace = rDesc.GetLeft().GetLRSpace();
            aPgSize.Width() += Abs( long(rLeftLRSpace.GetLeft()) - long(rLRSpace.GetLeft()) );
        }
    }
    return GetEditWin().LogicToPixel( aPgSize );
}


BOOL SwView::UpdateScrollbars()
{
    BOOL bRet = FALSE;
    if ( !aVisArea.IsEmpty() )
    {
        const BOOL bBorder = IsDocumentBorder();
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

        {
            const BOOL bVScrollVisible = pVScrollbar->IsVisible(TRUE);
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

            if ( bVScrollVisible != pVScrollbar->IsVisible(TRUE) )
                bRet = TRUE;
        }
        {
            const BOOL bHScrollVisible = pHScrollbar->IsVisible(TRUE);
            pHScrollbar->DocSzChgd( aTmpSz );
            pHScrollbar->ViewPortChgd( aTmpRect );
            if ( bHScrollVisible != pHScrollbar->IsVisible(TRUE) )
                bRet = TRUE;
            pScrollFill->Show(pHScrollbar->IsVisible(TRUE) && pVScrollbar->IsVisible(TRUE) );
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
            nFact = static_cast< USHORT >(Max( 20, nFact - 10 ));
        else
            nFact = static_cast< USHORT >(Min( 600, nFact + 10 ));

        SetZoom( SVX_ZOOM_PERCENT, nFact );
        bOk = TRUE;
    }
    else
    { 
        if (pWData && (COMMAND_WHEEL_SCROLL==pWData->GetMode()) && (((ULONG)0xFFFFFFFF) == pWData->GetScrollLines()))
            {
                        if (pWData->GetDelta()<0)
                                PhyPageDown();
                        else
                                PhyPageUp();
                        bOk = TRUE;
                }
        else
            bOk = pEditWin->HandleScrollCommand( rCEvt,
                            pHScrollbar, pVScrollbar);
    }
    return bOk;
}


