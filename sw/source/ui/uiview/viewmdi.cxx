/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewmdi.cxx,v $
 * $Revision: 1.24.140.1 $
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



#include <string> // HACK: prevent conflict between STLPORT and Workshop headers

#include "hintids.hxx"
#include <vcl/svapp.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/ruler.hxx>
#include <svx/lrspitem.hxx>
#include <svx/srchitem.hxx>
#include <sfx2/request.hxx>
#include <swmodule.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <wrtsh.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <viewopt.hxx>
#include <frmatr.hxx>
#ifndef _WDOCSH_HXX
#include <wdocsh.hxx>
#endif
#include <uitool.hxx>
#include <edtwin.hxx>
#include <pagedesc.hxx>
#include <bookmrk.hxx>
#include <fldbas.hxx>
#include <workctrl.hxx>
#include <usrpref.hxx>
#include <scroll.hxx>
#ifndef _WVIEW_HXX
#include <wview.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _VIEW_HRC
#include <view.hrc>
#endif
#ifndef _RIBBAR_HRC
#include <ribbar.hrc>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif

#include <IDocumentSettingAccess.hxx>
#include <PostItMgr.hxx>
#include <postit.hxx>

USHORT	SwView::nMoveType = NID_PGE;
USHORT  SwView::nActMark = 0;

#define VIEW_IMAGECOLOR COL_LIGHTBLUE

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;

void SwView::SetZoom( SvxZoomType eZoomType, short nFactor, BOOL bViewOnly )
{
    _SetZoom( GetEditWin().GetOutputSizePixel(), eZoomType, nFactor, bViewOnly );
}

void SwView::_SetZoom( const Size &rEditSize, SvxZoomType eZoomType,
                        short nFactor, BOOL bViewOnly )
{
    BOOL bUnLockView = !pWrtShell->IsViewLocked();
    pWrtShell->LockView( TRUE );
    pWrtShell->LockPaint();

    {
    ACT_KONTEXT(pWrtShell);

    long nFac = nFactor;

    BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
    SwMasterUsrPref *pUsrPref = (SwMasterUsrPref*)SW_MOD()->GetUsrPref(bWeb);

    const SwPageDesc &rDesc = pWrtShell->GetPageDesc( pWrtShell->GetCurPageDesc() );
    const SvxLRSpaceItem &rLRSpace = rDesc.GetMaster().GetLRSpace();
    const SwViewOption *pOpt = pWrtShell->GetViewOptions();
    long lLeftMargin = 0;

    if( eZoomType != SVX_ZOOM_PERCENT )
    {
        const bool bAutomaticViewLayout = 0 == pOpt->GetViewLayoutColumns();

        const SwRect aPageRect( pWrtShell->GetAnyCurRect( RECT_PAGE_CALC ) );
        const SwRect aRootRect( pWrtShell->GetAnyCurRect( RECT_PAGES_AREA ) );
        Size aPageSize( aPageRect.SSize() );
        Size aRootSize( aRootRect.SSize() );

        //mod #i6193# added sidebar width
        SwPostItMgr* pPostItMgr = GetPostItMgr();
        if (pPostItMgr->HasNotes() && pPostItMgr->ShowNotes())
            aPageSize.Width() += pPostItMgr->GetSidebarWidth() + pPostItMgr->GetSidebarBorderWidth();
        
        const MapMode aTmpMap( MAP_TWIP );
        const Size aWindowSize( GetEditWin().PixelToLogic( rEditSize, aTmpMap ) );

        if( nsUseOnPage::PD_MIRROR == rDesc.GetUseOn() )	// gespiegelte Seiten
        {
            const SvxLRSpaceItem &rLeftLRSpace = rDesc.GetLeft().GetLRSpace();
            aPageSize.Width() += Abs( long(rLeftLRSpace.GetLeft()) - long(rLRSpace.GetLeft()) );
        }

        if( SVX_ZOOM_OPTIMAL == eZoomType )
        {
            if (pPostItMgr->HasNotes() && pPostItMgr->ShowNotes())
            {
                lLeftMargin = long(rLRSpace.GetLeft()) + aPageRect.Left() + nLeftOfst;
            }
            else
            {
                aPageSize.Width() -= ( rLRSpace.GetLeft() + rLRSpace.GetRight() + nLeftOfst * 2 );
                lLeftMargin = long(rLRSpace.GetLeft()) + aPageRect.Left() + nLeftOfst;
            }
            nFac = aWindowSize.Width() * 100 / aPageSize.Width();
        }
        else if(SVX_ZOOM_WHOLEPAGE == eZoomType || SVX_ZOOM_PAGEWIDTH == eZoomType )
        {
            const long nOf = DOCUMENTBORDER * 2L;
            long nTmpWidth = bAutomaticViewLayout ? aPageSize.Width() : aRootSize.Width();
            nTmpWidth += nOf;
            aPageSize.Height() += nOf;
            nFac = aWindowSize.Width() * 100 / nTmpWidth;

            if ( SVX_ZOOM_WHOLEPAGE == eZoomType )
            {
                long nVisPercent = aWindowSize.Height() * 100 / aPageSize.Height();
                nFac = Min( nFac, nVisPercent );
            }	
        }
        else /*if( SVX_ZOOM_PAGEWIDTH_NOBORDER == eZoomType )*/
        {
            const long nTmpWidth = bAutomaticViewLayout ? aPageSize.Width() : aRootSize.Width();
            nFac = aWindowSize.Width() * 100 / nTmpWidth;
        }
    }

    nFac = Max( long( MINZOOM ), nFac );

    SwViewOption aOpt( *pOpt );
    if ( !GetViewFrame()->GetFrame()->IsInPlace() )
    {
        //MasterUsrPrefs updaten UND DANACH die ViewOptions der aktuellen
        //View updaten.
        if ( !bViewOnly &&
                (USHORT(nFac) 	   != pUsrPref->GetZoom() ||
                BYTE  (eZoomType) != pUsrPref->GetZoomType()) )
        {
            pUsrPref->SetZoom	 ( USHORT(nFac) );
            pUsrPref->SetZoomType( eZoomType );
            SW_MOD()->ApplyUsrPref( *pUsrPref,
                    bViewOnly ? this: 0,
                    bViewOnly ? VIEWOPT_DEST_VIEW_ONLY : 0 );
            pUsrPref->SetModified();
        }
        if ( pOpt->GetZoom() != (USHORT) nFac )
        {
            aOpt.SetZoom	( USHORT(nFac) );
            aOpt.SetReadonly(pOpt->IsReadonly());
            pWrtShell->ApplyViewOptions( aOpt );
        }
        if ( eZoomType != SVX_ZOOM_PERCENT )
        {
            Point aPos;

            if ( eZoomType == SVX_ZOOM_WHOLEPAGE )
                aPos.Y() = pWrtShell->GetAnyCurRect(RECT_PAGE).Top() - DOCUMENTBORDER;
            else
            {
                //sicherstellen, dass sich der Cursor im sichtbaren
                //Bereich befindet, damit nur 1x gescrollt wird
                aPos.X() = lLeftMargin;
                const SwRect &rCharRect = pWrtShell->GetCharRect();
                if ( rCharRect.Top() > GetVisArea().Bottom() ||
                    rCharRect.Bottom() < aPos.Y() )
                    aPos.Y() = rCharRect.Top() - rCharRect.Height();
                else
                    aPos.Y() = GetVisArea().Top();
            }
            SetVisArea( aPos );
        }
        // OS: Notloesung - in CalcVisArea wird u.U. wieder SetZoom gerufen und
        // dann werden falsche Werte eingestellt
        ((SwViewOption*)pWrtShell->GetViewOptions())->SetZoomType( eZoomType );
        CalcVisArea( rEditSize );	//fuer das Neuberechnen des sichtbaren Bereiches
    }
    else if ( USHORT(nFac) != pOpt->GetZoom() )
    {
        aOpt.SetZoom	( USHORT(nFac) );
        pWrtShell->ApplyViewOptions( aOpt );
    }

    const Fraction aFrac( nFac, 100 );
    pVRuler->SetZoom( aFrac );
    pVRuler->ForceUpdate();
    pHRuler->SetZoom( aFrac );
    pHRuler->ForceUpdate();
    ((SwViewOption*)pWrtShell->GetViewOptions())->SetZoomType( eZoomType );
    }
    pWrtShell->UnlockPaint();
    if( bUnLockView )
        pWrtShell->LockView( FALSE );
    
    if ( mpPostItMgr )
    {
        mpPostItMgr->Rescale();
        mpPostItMgr->CalcRects();
        mpPostItMgr->LayoutPostIts();
    }

//	eZoom = eZoomType;
}

void SwView::SetViewLayout( USHORT nColumns, bool bBookMode, BOOL bViewOnly )
{
    const BOOL bUnLockView = !pWrtShell->IsViewLocked();
    pWrtShell->LockView( TRUE );
    pWrtShell->LockPaint();

    {

    ACT_KONTEXT(pWrtShell);

    if ( !GetViewFrame()->GetFrame()->IsInPlace() && !bViewOnly )
    {
        const BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
        SwMasterUsrPref *pUsrPref = (SwMasterUsrPref*)SW_MOD()->GetUsrPref(bWeb);

        //MasterUsrPrefs updaten UND DANACH die ViewOptions der aktuellen
        //View updaten.
        if ( nColumns  != pUsrPref->GetViewLayoutColumns() ||
             bBookMode != pUsrPref->IsViewLayoutBookMode() )
        {
            pUsrPref->SetViewLayoutColumns( nColumns );
            pUsrPref->SetViewLayoutBookMode( bBookMode );
            SW_MOD()->ApplyUsrPref( *pUsrPref,
                    bViewOnly ? this: 0,
                    bViewOnly ? VIEWOPT_DEST_VIEW_ONLY : 0 );
            pUsrPref->SetModified();
        }
    }

    const SwViewOption *pOpt = pWrtShell->GetViewOptions();

    if ( nColumns  != pOpt->GetViewLayoutColumns() ||
         bBookMode != pOpt->IsViewLayoutBookMode() )
    {
        SwViewOption aOpt( *pOpt );
        aOpt.SetViewLayoutColumns( nColumns );
        aOpt.SetViewLayoutBookMode( bBookMode );
        pWrtShell->ApplyViewOptions( aOpt );
    }

    pVRuler->ForceUpdate();
    pHRuler->ForceUpdate();

    }

    pWrtShell->UnlockPaint();
    if( bUnLockView )
        pWrtShell->LockView( FALSE );

    SfxBindings& rBnd = GetViewFrame()->GetBindings();
    rBnd.Invalidate( SID_ATTR_VIEWLAYOUT );
    rBnd.Invalidate( SID_ATTR_ZOOMSLIDER);
}

/*
 * Scrollbar - Handler
 */

int SwView::_CreateScrollbar( BOOL bHori )
{
    Window *pMDI = &GetViewFrame()->GetWindow();
    SwScrollbar** ppScrollbar = bHori ? &pHScrollbar : &pVScrollbar;

    ASSERT( !*ppScrollbar, "vorher abpruefen!" )

    if( !bHori )
        CreatePageButtons( !bShowAtResize );

    *ppScrollbar = new SwScrollbar( pMDI, bHori );
    UpdateScrollbars();
    if(bHori)
        (*ppScrollbar)->SetScrollHdl( LINK( this, SwView, EndScrollHdl ));
    else
        (*ppScrollbar)->SetScrollHdl( LINK( this, SwView, ScrollHdl ));
    (*ppScrollbar)->SetEndScrollHdl( LINK( this, SwView, EndScrollHdl ));

    (*ppScrollbar)->EnableDrag( TRUE );

    if(GetWindow())
        InvalidateBorder();

    // Scrollbar muss nochmals getestet werden, da im InvalidateBorder u.U. der
    // Scrollbar wieder geloescht wurde
    if ( !bShowAtResize && (*ppScrollbar))
        (*ppScrollbar)->ExtendedShow();

    return 1;
}

void SwView::CreatePageButtons(BOOL bShow)
{
    Window *pMDI = &GetViewFrame()->GetWindow();
    pPageUpBtn		= new SwHlpImageButton(pMDI, SW_RES( BTN_PAGEUP ), TRUE );
    pPageUpBtn->SetHelpId(HID_SCRL_PAGEUP);
    pPageDownBtn	= new SwHlpImageButton(pMDI, SW_RES( BTN_PAGEDOWN ), FALSE );
    pPageDownBtn->SetHelpId(HID_SCRL_PAGEDOWN);
    Reference< XFrame > xFrame = GetViewFrame()->GetFrame()->GetFrameInterface();
    pNaviBtn = new SwNaviImageButton(pMDI, xFrame );
    pNaviBtn->SetHelpId(HID_SCRL_NAVI);
    Link aLk( LINK( this, SwView, BtnPage ) );
    pPageUpBtn->SetClickHdl( aLk );
    pPageDownBtn->SetClickHdl( aLk );
    if(nMoveType != NID_PGE)
    {
        Color aColor(VIEW_IMAGECOLOR);
        SetImageButtonColor(aColor);
    }

    if(bShow)
    {
        pPageUpBtn->Show();
        pPageDownBtn->Show();
        pNaviBtn->Show();
    }
};

/*
 * Button-Handler
 */
IMPL_LINK( SwView, BtnPage, Button *, pButton )
{
    // #i75416# move the execution of the search to an asynchronously called static link
    bool* pbNext = new bool( (pButton == pPageDownBtn) );
    Application::PostUserEvent( STATIC_LINK(this, SwView, MoveNavigationHdl), pbNext );
    return 0;
}
/*-----------------20.06.97 10:46-------------------

--------------------------------------------------*/
IMPL_STATIC_LINK( SwView, MoveNavigationHdl, bool *, pbNext )
{
    if ( !pbNext )
        return 0;
    bool bNext = *pbNext;
    SwWrtShell& rSh = pThis->GetWrtShell();
    switch( nMoveType )
    {
        case NID_PGE:
            bNext ? pThis->PhyPageDown() : pThis->PhyPageUp();
        break;
        case NID_TBL :
            rSh.EnterStdMode();
            if(bNext)
                rSh.MoveTable(fnTableNext, fnTableStart);
            else
                rSh.MoveTable(fnTablePrev, fnTableStart);
        break;
        case NID_FRM :
        case NID_GRF:
        case NID_OLE:
        {
            USHORT eType = GOTOOBJ_FLY_FRM;
            if(nMoveType == NID_GRF)
                eType = GOTOOBJ_FLY_GRF;
            else if(nMoveType == NID_OLE)
                eType = GOTOOBJ_FLY_OLE;
            BOOL bSuccess = bNext ?
                    rSh.GotoNextFly(eType) :
                        rSh.GotoPrevFly(eType);
            if(bSuccess)
            {
                rSh.HideCrsr();
                rSh.EnterSelFrmMode();
            }
        }
        break;
        case NID_DRW :
        case NID_CTRL:
            rSh.GotoObj(bNext,
                    nMoveType == NID_DRW ?
                        GOTOOBJ_DRAW_SIMPLE :
                        GOTOOBJ_DRAW_CONTROL);
        break;
        case NID_REG :
            rSh.EnterStdMode();
            if(bNext)
                rSh.MoveRegion(fnRegionNext, fnRegionStart);
            else
                rSh.MoveRegion(fnRegionPrev, fnRegionStart);

        break;
        case NID_BKM :
            rSh.EnterStdMode();
            pThis->GetViewFrame()->GetDispatcher()->Execute(bNext ?
                                        FN_NEXT_BOOKMARK :
                                            FN_PREV_BOOKMARK);
        break;
        case NID_OUTL:
            rSh.EnterStdMode();
            bNext ? rSh.GotoNextOutline() : rSh.GotoPrevOutline();
        break;
        case NID_SEL :
            bNext ? rSh.GoNextCrsr() : rSh.GoPrevCrsr();
        break;
        case NID_FTN:
            rSh.EnterStdMode();
            bNext ?
                rSh.GotoNextFtnAnchor() :
                    rSh.GotoPrevFtnAnchor();
        break;
        case NID_MARK:
        {
            // Selektionen aufheben
            rSh.MoveCrsr();
            rSh.EnterStdMode();
            const USHORT nBookCnt = rSh.GetBookmarkCnt();
            USHORT nMarkCount = 0;
            USHORT nRealIdx[MAX_MARKS];
            for( USHORT nCount = 0; nCount < nBookCnt; ++nCount )
            {
                if( rSh.GetBookmark( nCount ).IsMark() )
                {
                    nRealIdx[nMarkCount] = nCount;
                    ++nMarkCount;
                }
            }
            if (nMarkCount)
            {
                if(!bNext)
                {
                    if (nActMark > 1)
                        --nActMark;
                    else
                        nActMark = nMarkCount;
                }
                else
                {
                    ++nActMark;
                    if (nActMark > MAX_MARKS || nActMark > nMarkCount)
                        nActMark = 1;
                }

                rSh.GotoBookmark( nRealIdx[nActMark - 1] );
            }
        }
        break;
        case NID_POSTIT:
        {
            SwMarginWin* pPostIt = pThis->GetPostItMgr()->GetActivePostIt();
            if (pPostIt)
                pThis->GetPostItMgr()->SetActivePostIt(0);
            SwFieldType* pFldType = rSh.GetFldType(0, RES_POSTITFLD);
            if (rSh.MoveFldType(pFldType, bNext))
                pThis->GetViewFrame()->GetDispatcher()->Execute(FN_POSTIT);
            else
                //first/last item
                pThis->GetPostItMgr()->SetActivePostIt(pPostIt);
        }
        break;
        case NID_SRCH_REP:
        if(pSrchItem)
        {
            BOOL bBackward = pSrchItem->GetBackward();
            if(rSh.HasSelection() && !bNext == rSh.IsCrsrPtAtEnd())
                rSh.SwapPam();
            pSrchItem->SetBackward(!bNext);
            SfxRequest aReq(FN_REPEAT_SEARCH, SFX_CALLMODE_SLOT, pThis->GetPool());
            pThis->ExecSearch(aReq);
            pSrchItem->SetBackward(bBackward);
        }
        break;
        case NID_INDEX_ENTRY:
            rSh.GotoNxtPrvTOXMark(bNext);
        break;

        case NID_TABLE_FORMULA:
            rSh.GotoNxtPrvTblFormula( bNext );
            break;

        case NID_TABLE_FORMULA_ERROR:
            rSh.GotoNxtPrvTblFormula( bNext, TRUE );
            break;
    }
    pThis->pEditWin->GrabFocus();
    delete pbNext;
    return 0;
}

/*************************************************************************
|*
|*	SwView::CreateTab()
|*
|*	Beschreibung
|*	Ersterstellung		VB 29.05.91
|*	Letzte Aenderung	OS 09.05.96
|*
*************************************************************************/

int SwView::CreateTab()
{
    pHRuler->SetActive(GetFrame() && IsActive());

    pHRuler->Show();
    InvalidateBorder();
    return 1;
}

/*************************************************************************
|*
|*	SwView::KillTab()
|*
|*	Beschreibung
|*	Ersterstellung		VB 29.05.91
|*	Letzte Aenderung	OS 09.05.96
|*
*************************************************************************/

int SwView::KillTab()
{
    pHRuler->Hide();
    InvalidateBorder();
    return 1;
}

void SwView::ChangeTabMetric( FieldUnit eUnit )
{
    if(pHRuler->GetUnit() != eUnit )
    {
        pHRuler->SetUnit( eUnit );
        pHRuler->Invalidate();
    }
}

void SwView::ChangeVLinealMetric( FieldUnit eUnit )
{
    if(pVRuler->GetUnit() != eUnit)
    {
        pVRuler->SetUnit( eUnit );
        pVRuler->Invalidate();
    }
}
/* -----------------------------07.04.01 17:09--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwView::GetVLinealMetric(FieldUnit& eToFill) const
{
    eToFill = pVRuler->GetUnit();
    return pVRuler != 0;
}
/* -----------------------------07.04.01 17:09--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwView::GetHLinealMetric(FieldUnit& eToFill) const
{
    eToFill = pHRuler->GetUnit();
    return pHRuler != 0;
}
/*************************************************************************
|*
|*	SwView::CreateVLineal()
|*
|*	Beschreibung
|*	Ersterstellung	VB 29.05.91
|*
*************************************************************************/

int SwView::CreateVLineal()
{
    pHRuler->SetBorderPos( pVRuler->GetSizePixel().Width()-1 );

    pVRuler->SetActive(GetFrame() && IsActive());
    pVRuler->Show();
    InvalidateBorder();
    return 1;
}

/*************************************************************************
|*
|*	SwView::KillVLineal()
|*
|*	Beschreibung
|*	Ersterstellung	VB 29.05.91
|*
*************************************************************************/

int SwView::KillVLineal()
{
    pVRuler->Hide();
    pHRuler->SetBorderPos( 0 );
    InvalidateBorder();
    return 1;
}
/*************************************************************************
|*
|*	SwView::ExecRulerClick()
|*
|*	Beschreibung
|*	Ersterstellung	OS 15.06.95
|*	Letzte Aenderung
|*
*************************************************************************/

IMPL_LINK( SwView, ExecRulerClick, Ruler *, pRuler )
{
    USHORT nDefPage = 0;
    switch( pRuler->GetClickType() )
    {
        case RULER_TYPE_DONTKNOW:
        case RULER_TYPE_OUTSIDE:
        case RULER_TYPE_INDENT:
        case RULER_TYPE_MARGIN1:
        case RULER_TYPE_MARGIN2:
            nDefPage = TP_PARA_STD;
        break;
        default:
            nDefPage = TP_TABULATOR;

    }

    SfxUInt16Item aDefPage(SID_PARA_DLG, nDefPage);
    GetViewFrame()->GetDispatcher()->Execute( SID_PARA_DLG,
                                SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD,
                                &aDefPage, 0L );
    return 0;
}


/*-----------------20.02.97 09:11-------------------

--------------------------------------------------*/

USHORT SwView::GetMoveType()
{
    return nMoveType;
}
/*-----------------20.02.97 09:11-------------------

--------------------------------------------------*/

void SwView::SetMoveType(USHORT nSet)
{
    BOOL bLastPage = nMoveType == NID_PGE;
    nMoveType = nSet;
    BOOL bNewPage = nMoveType == NID_PGE;
    if(bNewPage != bLastPage)
    {
        Color aColor(bNewPage ? COL_BLACK : VIEW_IMAGECOLOR);
        const TypeId aTypeId = TYPE(SwView);
        SwView* pView = (SwView*)SfxViewShell::GetFirst(&aTypeId);
        while( pView )
        {
            pView->SetImageButtonColor(aColor);
            pView = (SwView*)SfxViewShell::GetNext(*pView, &aTypeId);
        }
    }
}

/*-----------------20.06.97 11:18-------------------

--------------------------------------------------*/
void SwView::SetActMark(BYTE nSet)
{
    nActMark = nSet;
}

/*-----------------15.03.97 10:53-------------------

--------------------------------------------------*/

void SwView::SetImageButtonColor(Color& rColor)
{
    if(pPageUpBtn)
    {
        pPageUpBtn->SetControlForeground(rColor);
        pPageDownBtn->SetControlForeground(rColor);
    }
}
/* -----------------------------2002/06/26 13:57------------------------------

 ---------------------------------------------------------------------------*/
void SwView::ShowHScrollbar(sal_Bool bShow)
{
    DBG_ASSERT(pHScrollbar, "Scrollbar invalid");
    pHScrollbar->ExtendedShow(bShow);
}
/* -----------------------------2002/06/26 13:57------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwView::IsHScrollbarVisible()const
{
    DBG_ASSERT(pHScrollbar, "Scrollbar invalid");
    return pHScrollbar->IsVisible( FALSE ) || pHScrollbar->IsAuto();
}
/* -----------------------------2002/06/26 13:57------------------------------

 ---------------------------------------------------------------------------*/
void SwView::ShowVScrollbar(sal_Bool bShow)
{
    DBG_ASSERT(pVScrollbar, "Scrollbar invalid");
    pVScrollbar->ExtendedShow(bShow);
    pPageUpBtn->Show(bShow);
    pPageDownBtn->Show(bShow);
    pNaviBtn->Show(bShow);
}
/* -----------------------------2002/06/26 13:57------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwView::IsVScrollbarVisible()const
{
    DBG_ASSERT(pVScrollbar, "Scrollbar invalid");
    return pVScrollbar->IsVisible( FALSE );
}


