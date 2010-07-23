/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "precompiled_starmath.hxx"


#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/AccessibleEventObject.hpp>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <toolkit/helper/vclunohelper.hxx>


#include "starmath.hrc"
#define ITEMID_FONT 		1
#define ITEMID_FONTHEIGHT	2
#define ITEMID_LRSPACE		3
#define ITEMID_WEIGHT		4


#include <vcl/menu.hxx>
#include <editeng/editview.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editstat.hxx>
#include <editeng/eeitem.hxx>
#include <sfx2/dispatch.hxx>
#include <svl/intitem.hxx>
#include <svl/itempool.hxx>
#include <svl/stritem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/lrspitem.hxx>
#include <svl/itemset.hxx>
#include <editeng/fontitem.hxx>
#include <sfx2/viewfrm.hxx>

#include "edit.hxx"
#include "view.hxx"
#include "document.hxx"
#include "config.hxx"

#define SCROLL_LINE 		24

#define MINWIDTH		200
#define MINHEIGHT		200
#define MINSPLIT		40
#define SPLITTERWIDTH	2


using namespace com::sun::star::accessibility;
using namespace com::sun::star;
using namespace com::sun::star::uno;

////////////////////////////////////////


void SmGetLeftSelectionPart(const ESelection aSel,
                            USHORT &nPara, USHORT &nPos)
    // returns paragraph number and position of the selections left part
{
    // compare start and end of selection and use the one that comes first
    if (	aSel.nStartPara <  aSel.nEndPara
        ||	(aSel.nStartPara == aSel.nEndPara  &&  aSel.nStartPos < aSel.nEndPos) )
    {	nPara = aSel.nStartPara;
        nPos  = aSel.nStartPos;
    }
    else
    {	nPara = aSel.nEndPara;
        nPos  = aSel.nEndPos;
    }
}

////////////////////////////////////////

SmEditWindow::SmEditWindow( SmCmdBoxWindow &rMyCmdBoxWin ) :
    Window              (&rMyCmdBoxWin),
    DropTargetHelper    ( this ),
    pAccessible         (0),
    rCmdBox             (rMyCmdBoxWin),
    pEditView           (0),
    pHScrollBar         (0),
    pVScrollBar         (0),
    pScrollBox          (0)
{
    SetHelpId(HID_SMA_COMMAND_WIN_EDIT);
    SetMapMode(MAP_PIXEL);

    // Even RTL languages don't use RTL for math
    rCmdBox.GetEditWindow()->EnableRTL( FALSE );

    ApplyColorConfigValues( SM_MOD()->GetColorConfig() );

    // compare DataChanged
    SetBackground( GetSettings().GetStyleSettings().GetWindowColor() );

    aModifyTimer.SetTimeoutHdl(LINK(this, SmEditWindow, ModifyTimerHdl));
    aModifyTimer.SetTimeout(500);

    aCursorMoveTimer.SetTimeoutHdl(LINK(this, SmEditWindow, CursorMoveTimerHdl));
    aCursorMoveTimer.SetTimeout(500);

    // if not called explicitly the this edit window within the
    // command window will just show an empty gray panel.
    Show();
}


SmEditWindow::~SmEditWindow()
{
    aCursorMoveTimer.Stop();
    aModifyTimer.Stop();


    // #112565# clean up of classes used for accessibility
    // must be done before EditView (and thus EditEngine) is no longer
    // available for those classes.
    if (pAccessible)
        pAccessible->ClearWin();    // make Accessible defunctional
    // Note: memory for pAccessible will be freed when the reference
    // xAccessible is released.

    if (pEditView)
    {
        EditEngine *pEditEngine = pEditView->GetEditEngine();
        if (pEditEngine)
        {
            pEditEngine->SetStatusEventHdl( Link() );
            pEditEngine->RemoveView( pEditView );
        }
    }
    delete pEditView;
    delete pHScrollBar;
    delete pVScrollBar;
    delete pScrollBox;
}

void SmEditWindow::InvalidateSlots()
{
    SfxBindings& rBind = GetView()->GetViewFrame()->GetBindings();
    rBind.Invalidate(SID_COPY);
    rBind.Invalidate(SID_CUT);
    rBind.Invalidate(SID_DELETE);
}

SmViewShell * SmEditWindow::GetView()
{
    return rCmdBox.GetView();
}


SmDocShell * SmEditWindow::GetDoc()
{
    SmViewShell *pView = rCmdBox.GetView();
    return pView ? pView->GetDoc() : 0;
}


EditEngine * SmEditWindow::GetEditEngine()
{
    EditEngine *pEditEng = 0;
    if (pEditView)
        pEditEng = pEditView->GetEditEngine();
    else
    {
        SmDocShell *pDoc = GetDoc();
        if (pDoc)
            pEditEng = &pDoc->GetEditEngine();
    }
    return pEditEng;
}


SfxItemPool * SmEditWindow::GetEditEngineItemPool()
{
    SmDocShell *pDoc = GetDoc();
    return pDoc ? &pDoc->GetEditEngineItemPool() : 0;
}

void SmEditWindow::ApplyColorConfigValues( const svtools::ColorConfig &rColorCfg )
{
    // Note: SetBackground still done in SmEditWindow::DataChanged
#if OSL_DEBUG_LEVEL > 1
//   ColorData nVal = rColorCfg.GetColorValue(svtools::FONTCOLOR).nColor;
#endif
    SetTextColor( rColorCfg.GetColorValue(svtools::FONTCOLOR).nColor );
    Invalidate();
}

void SmEditWindow::DataChanged( const DataChangedEvent& )
{
    const StyleSettings aSettings( GetSettings().GetStyleSettings() );

    ApplyColorConfigValues( SM_MOD()->GetColorConfig() );
    SetBackground( aSettings.GetWindowColor() );

    // edit fields in other Applications use this font instead of
    // the application font thus we use this one too
    SetPointFont( aSettings.GetFieldFont() /*aSettings.GetAppFont()*/ );

    EditEngine  *pEditEngine = GetEditEngine();
    SfxItemPool *pEditEngineItemPool = GetEditEngineItemPool();

    if (pEditEngine && pEditEngineItemPool)
    {
        //!
        //! see also SmDocShell::GetEditEngine() !
        //!

        pEditEngine->SetDefTab( USHORT( GetTextWidth( C2S("XXXX") ) ) );

        SetEditEngineDefaultFonts( *pEditEngine, *pEditEngineItemPool );

        // forces new settings to be used
        // unfortunately this resets the whole edit engine
        // thus we need to save at least the text
        String aTxt( pEditEngine->GetText( LINEEND_LF ) );
        pEditEngine->Clear();	//#77957 incorrect font size
        pEditEngine->SetText( aTxt );
    }

    AdjustScrollBars();
    Resize();
}

IMPL_LINK( SmEditWindow, ModifyTimerHdl, Timer *, EMPTYARG /*pTimer*/ )
{
    SmModule *pp = SM_MOD();
    if (pp->GetConfig()->IsAutoRedraw())
    {
        Flush();
        aModifyTimer.Stop();
    }
    return 0;
}


IMPL_LINK(SmEditWindow, CursorMoveTimerHdl, Timer *, EMPTYARG /*pTimer*/)
    // every once in a while check cursor position (selection) of edit
    // window and if it has changed (try to) set the formula-cursor
    // according to that.
{
    ESelection	aNewSelection   (GetSelection());

    if (!aNewSelection.IsEqual(aOldSelection))
    {   SmViewShell *pView = rCmdBox.GetView();

        if (pView)
        {
            // get row and column to look for
            USHORT	nRow, nCol;
            SmGetLeftSelectionPart(aNewSelection, nRow, nCol);
            nRow++;
            nCol++;

            pView->GetGraphicWindow().SetCursorPos(nRow, nCol);

            aOldSelection = aNewSelection;
        }
    }

    return 0;
}


void SmEditWindow::Resize()
{
    if (!pEditView)
        CreateEditView();

    if (pEditView)
    {
        pEditView->SetOutputArea(AdjustScrollBars());
        pEditView->ShowCursor();

        DBG_ASSERT( pEditView->GetEditEngine(), "EditEngine missing" );
        const long nMaxVisAreaStart = pEditView->GetEditEngine()->GetTextHeight() -
                                      pEditView->GetOutputArea().GetHeight();
        if (pEditView->GetVisArea().Top() > nMaxVisAreaStart)
        {
            Rectangle aVisArea(pEditView->GetVisArea() );
            aVisArea.Top() = (nMaxVisAreaStart > 0 ) ? nMaxVisAreaStart : 0;
            aVisArea.SetSize(pEditView->GetOutputArea().GetSize());
            pEditView->SetVisArea(aVisArea);
            pEditView->ShowCursor();
        }
        InitScrollBars();
    }
    Invalidate();
}

void SmEditWindow::MouseButtonUp(const MouseEvent &rEvt)
{
    if (pEditView)
        pEditView->MouseButtonUp(rEvt);
    else
        Window::MouseButtonUp (rEvt);

    // ggf FormulaCursor neu positionieren
    CursorMoveTimerHdl(&aCursorMoveTimer);
    InvalidateSlots();
}

void SmEditWindow::MouseButtonDown(const MouseEvent &rEvt)
{
    if (pEditView)
        pEditView->MouseButtonDown(rEvt);
    else
        Window::MouseButtonDown (rEvt);

    GrabFocus();
}

void SmEditWindow::Command(const CommandEvent& rCEvt)
{
    BOOL bForwardEvt = TRUE;
    if (rCEvt.GetCommand() == COMMAND_CONTEXTMENU)
    {
        GetParent()->ToTop();

        Point aPoint = rCEvt.GetMousePosPixel();
        PopupMenu* pPopupMenu = new PopupMenu(SmResId(RID_COMMANDMENU));

        // added for replaceability of context menus #96085, #93782
        Menu* pMenu = NULL;
        ::com::sun::star::ui::ContextMenuExecuteEvent aEvent;
        aEvent.SourceWindow = VCLUnoHelper::GetInterface( this );
        aEvent.ExecutePosition.X = aPoint.X();
        aEvent.ExecutePosition.Y = aPoint.Y();
        ::rtl::OUString sDummy;
        if ( GetView()->TryContextMenuInterception( *pPopupMenu, sDummy, pMenu, aEvent ) )
        {
            if ( pMenu )
            {
                delete pPopupMenu;
                pPopupMenu = (PopupMenu*) pMenu;
            }
        }

        pPopupMenu->SetSelectHdl(LINK(this, SmEditWindow, MenuSelectHdl));

        pPopupMenu->Execute( this, aPoint );
        delete pPopupMenu;
        bForwardEvt = FALSE;
    }
    else if (rCEvt.GetCommand() == COMMAND_WHEEL)
        bForwardEvt = !HandleWheelCommands( rCEvt );

    if (bForwardEvt)
    {
        if (pEditView)
            pEditView->Command( rCEvt );
        else
            Window::Command (rCEvt);
    }
}


BOOL SmEditWindow::HandleWheelCommands( const CommandEvent &rCEvt )
{
    BOOL bCommandHandled = FALSE;    // true if the CommandEvent needs not
                                    // to be passed on (because it has fully
                                    // been taken care of).

    const CommandWheelData* pWData = rCEvt.GetWheelData();
    if (pWData)
    {
        if (COMMAND_WHEEL_ZOOM == pWData->GetMode())
            bCommandHandled = TRUE;     // no zooming in Command window
        else
            bCommandHandled = HandleScrollCommand( rCEvt, pHScrollBar, pVScrollBar);
    }

    return bCommandHandled;
}


IMPL_LINK_INLINE_START( SmEditWindow, MenuSelectHdl, Menu *, pMenu )
{
    SmViewShell *pViewSh = rCmdBox.GetView();
    if (pViewSh)
        pViewSh->GetViewFrame()->GetDispatcher()->Execute(
                SID_INSERTCOMMAND, SFX_CALLMODE_STANDARD,
                new SfxInt16Item(SID_INSERTCOMMAND, pMenu->GetCurItemId()), 0L);
    return 0;
}
IMPL_LINK_INLINE_END( SmEditWindow, MenuSelectHdl, Menu *, pMenu )

void SmEditWindow::KeyInput(const KeyEvent& rKEvt)
{
    if (rKEvt.GetKeyCode().GetCode() == KEY_ESCAPE)
    {
        BOOL bCallBase = TRUE;
        SfxViewShell* pViewShell = GetView();
        if ( pViewShell && pViewShell->ISA(SmViewShell) )
        {
            // Terminate possible InPlace mode
            bCallBase = !pViewShell->Escape();
        }
        if ( bCallBase )
            Window::KeyInput( rKEvt );
    }
    else
    {
        // Timer neu starten, um den Handler (auch bei laengeren Eingaben)
        // moeglichst nur einmal am Ende aufzurufen.
        aCursorMoveTimer.Start();

        DBG_ASSERT( pEditView, "EditView missing (NULL pointer)" );
        if (!pEditView)
            CreateEditView();
        if ( !pEditView->PostKeyEvent(rKEvt) )
        {
            SmViewShell *pView = GetView();
            if ( pView && !pView->KeyInput(rKEvt) )
            {
                    /* fuert bei F1 (Hilfe) zum Zerstoeren von this! */
                Flush();
                if ( aModifyTimer.IsActive() )
                    aModifyTimer.Stop();
                Window::KeyInput(rKEvt);
            }
            else
            {
                //SFX hat evtl. Slot an der View gecallt und dabei (wg. Hack
                //im SFX) den Focus auf die View gesetzt
                SfxViewShell* pVShell = GetView();
                if ( pVShell && pVShell->ISA(SmViewShell) &&
                     ((SmViewShell*)pVShell)->GetGraphicWindow().HasFocus() )
                {
                    GrabFocus();
                }
            }
        }
        else
        {
            // have doc-shell modified only for formula input/change and not
            // cursor travelling and such things...
            SmDocShell *pDocShell = GetDoc();
            if (pDocShell)
                pDocShell->SetModified( GetEditEngine()->IsModified() );

            aModifyTimer.Start();
        }

        InvalidateSlots();
    }
}

void SmEditWindow::Paint(const Rectangle& rRect)
{
    if (!pEditView)
        CreateEditView();
    pEditView->Paint(rRect);
}

void SmEditWindow::CreateEditView()
{
    EditEngine *pEditEngine = GetEditEngine();

    //! pEditEngine and pEditView may be 0.
    //! For example when the program is used by the document-converter
    if (!pEditView && pEditEngine)
    {
        pEditView = new EditView( pEditEngine, this );
        pEditEngine->InsertView( pEditView );

        if (!pVScrollBar)
            pVScrollBar = new ScrollBar(this, WinBits(WB_VSCROLL));
        if (!pHScrollBar)
            pHScrollBar = new ScrollBar(this, WinBits(WB_HSCROLL));
        if (!pScrollBox)
            pScrollBox  = new ScrollBarBox(this);
        pVScrollBar->SetScrollHdl(LINK(this, SmEditWindow, ScrollHdl));
        pHScrollBar->SetScrollHdl(LINK(this, SmEditWindow, ScrollHdl));
        pVScrollBar->EnableDrag( TRUE );
        pHScrollBar->EnableDrag( TRUE );

        pEditView->SetOutputArea(AdjustScrollBars());

        ESelection eSelection;

        pEditView->SetSelection(eSelection);
        Update();
        pEditView->ShowCursor(TRUE, TRUE);

        pEditEngine->SetStatusEventHdl( LINK(this, SmEditWindow, EditStatusHdl) );
        SetPointer(pEditView->GetPointer());

        SetScrollBarRanges();
    }
}


IMPL_LINK( SmEditWindow, EditStatusHdl, EditStatus *, EMPTYARG /*pStat*/ )
{
    if (!pEditView)
        return 1;
    else
    {
        Resize();
        return 0;
    }
}

IMPL_LINK_INLINE_START( SmEditWindow, ScrollHdl, ScrollBar *, EMPTYARG /*pScrollBar*/ )
{
    DBG_ASSERT(pEditView, "EditView missing");
    if (pEditView)
    {
        pEditView->SetVisArea(Rectangle(Point(pHScrollBar->GetThumbPos(),
                                            pVScrollBar->GetThumbPos()),
                                        pEditView->GetVisArea().GetSize()));
        pEditView->Invalidate();
    }
    return 0;
}
IMPL_LINK_INLINE_END( SmEditWindow, ScrollHdl, ScrollBar *, pScrollBar )

Rectangle SmEditWindow::AdjustScrollBars()
{
    const Size aOut( GetOutputSizePixel() );
    Point aPoint;
    Rectangle aRect( aPoint, aOut );

    if (pVScrollBar && pHScrollBar && pScrollBox)
    {
        const long nTmp = GetSettings().GetStyleSettings().GetScrollBarSize();
        Point aPt( aRect.TopRight() ); aPt.X() -= nTmp -1L;
        pVScrollBar->SetPosSizePixel( aPt, Size(nTmp, aOut.Height() - nTmp));

        aPt = aRect.BottomLeft(); aPt.Y() -= nTmp - 1L;
        pHScrollBar->SetPosSizePixel( aPt, Size(aOut.Width() - nTmp, nTmp));

        aPt.X() = pHScrollBar->GetSizePixel().Width();
        aPt.Y() = pVScrollBar->GetSizePixel().Height();
        pScrollBox->SetPosSizePixel(aPt, Size(nTmp, nTmp ));

        aRect.Right()  = aPt.X() - 2;
        aRect.Bottom() = aPt.Y() - 2;
    }
    return aRect;
}

void SmEditWindow::SetScrollBarRanges()
{
    // Extra-Methode, nicht InitScrollBars, da auch fuer EditEngine-Events.
    EditEngine *pEditEngine = GetEditEngine();
    if (pVScrollBar && pHScrollBar && pEditEngine && pEditView)
    {
        long nTmp = pEditEngine->GetTextHeight();
        pVScrollBar->SetRange(Range(0, nTmp));
        pVScrollBar->SetThumbPos(pEditView->GetVisArea().Top());

        nTmp = pEditEngine->GetPaperSize().Width();
        pHScrollBar->SetRange(Range(0,nTmp));
        pHScrollBar->SetThumbPos(pEditView->GetVisArea().Left());
    }
}

void SmEditWindow::InitScrollBars()
{
    if (pVScrollBar && pHScrollBar && pScrollBox && pEditView)
    {
        const Size aOut( pEditView->GetOutputArea().GetSize() );
        pVScrollBar->SetVisibleSize(aOut.Height());
        pVScrollBar->SetPageSize(aOut.Height() * 8 / 10);
        pVScrollBar->SetLineSize(aOut.Height() * 2 / 10);

        pHScrollBar->SetVisibleSize(aOut.Width());
        pHScrollBar->SetPageSize(aOut.Width() * 8 / 10);
        pHScrollBar->SetLineSize(SCROLL_LINE );

        SetScrollBarRanges();

        pVScrollBar->Show();
        pHScrollBar->Show();
        pScrollBox->Show();
    }
}


String SmEditWindow::GetText() const
{
    String aText;
    EditEngine *pEditEngine = const_cast< SmEditWindow* >(this)->GetEditEngine();
    DBG_ASSERT( pEditEngine, "EditEngine missing" );
    if (pEditEngine)
        aText = pEditEngine->GetText( LINEEND_LF );
    return aText;
}


void SmEditWindow::SetText(const XubString& rText)
{
    EditEngine *pEditEngine = GetEditEngine();
    DBG_ASSERT( pEditEngine, "EditEngine missing" );
    if (pEditEngine  &&  !pEditEngine->IsModified())
    {
        if (!pEditView)
            CreateEditView();

        ESelection eSelection = pEditView->GetSelection();

        pEditEngine->SetText(rText);
        pEditEngine->ClearModifyFlag();

        //! Hier die Timer neu zu starten verhindert, dass die Handler fuer andere
        //! (im Augenblick nicht mehr aktive) Math Tasks aufgerufen werden.
        aModifyTimer.Start();
        aCursorMoveTimer.Start();

        pEditView->SetSelection(eSelection);
    }
}


void SmEditWindow::GetFocus()
{
    Window::GetFocus();

    if (xAccessible.is())
    {
        // Note: will implicitly send the AccessibleStateType::FOCUSED event
        ::accessibility::AccessibleTextHelper *pHelper = pAccessible->GetTextHelper();
        if (pHelper)
            pHelper->SetFocus( sal_True );
    }

    if (!pEditView)
         CreateEditView();
    EditEngine *pEditEngine = GetEditEngine();
    if (pEditEngine)
        pEditEngine->SetStatusEventHdl( LINK(this, SmEditWindow, EditStatusHdl) );
}


void SmEditWindow::LoseFocus()
{
    EditEngine *pEditEngine = GetEditEngine();
    if (pEditEngine)
        pEditEngine->SetStatusEventHdl( Link() );

    Window::LoseFocus();

    if (xAccessible.is())
    {
        // Note: will implicitly send the AccessibleStateType::FOCUSED event
        ::accessibility::AccessibleTextHelper *pHelper = pAccessible->GetTextHelper();
        if (pHelper)
            pHelper->SetFocus( sal_False );
    }
}


BOOL SmEditWindow::IsAllSelected() const
{
    BOOL bRes = FALSE;
    EditEngine *pEditEngine = ((SmEditWindow *) this)->GetEditEngine();
    DBG_ASSERT( pEditView, "NULL pointer" );
    DBG_ASSERT( pEditEngine, "NULL pointer" );
    if (pEditEngine  &&  pEditView)
    {
        ESelection eSelection( pEditView->GetSelection() );
        INT32 nParaCnt = pEditEngine->GetParagraphCount();
        if (!(nParaCnt - 1))
        {
            String Text( pEditEngine->GetText( LINEEND_LF ) );
            bRes = !eSelection.nStartPos && (eSelection.nEndPos == Text.Len () - 1);
        }
        else
        {
            bRes = !eSelection.nStartPara && (eSelection.nEndPara == nParaCnt - 1);
        }
    }
    return bRes;
}

void SmEditWindow::SelectAll()
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    if (pEditView)
    {
        // 0xFFFF as last two parameters refers to the end of the text
        pEditView->SetSelection( ESelection( 0, 0, 0xFFFF, 0xFFFF ) );
    }
}

void SmEditWindow::InsertCommand(USHORT nCommand)
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        //Anfang der Selektion merken und hinterher den Cursor daraufsetzen. Nur so
        //macht das SelNextMark() Sinn.
        ESelection aSelection = pEditView->GetSelection();
        aSelection.nEndPos  = aSelection.nStartPos;
        aSelection.nEndPara = aSelection.nStartPara;

        DBG_ASSERT( pEditView, "NULL pointer" );
        String  aText = String(SmResId(nCommand));
        pEditView->InsertText(aText);

        if (HasMark(aText))
        {   // set selection to next mark
            pEditView->SetSelection(aSelection);
            SelNextMark();
        }
        else
        {   // set selection after inserted text
            aSelection.nEndPos    = aSelection.nEndPos + sal::static_int_cast< xub_StrLen >(aText.Len());
            aSelection.nStartPos  = aSelection.nEndPos;
            pEditView->SetSelection(aSelection);
        }

        GrabFocus();
    }
}

void SmEditWindow::MarkError(const Point &rPos)
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        const xub_StrLen    nCol = sal::static_int_cast< xub_StrLen >(rPos.X());
        const USHORT        nRow = sal::static_int_cast< USHORT >(rPos.Y() - 1);

        pEditView->SetSelection(ESelection(nRow, nCol - 1, nRow, nCol));
        GrabFocus();
    }
}

void SmEditWindow::SelNextMark()
{
    EditEngine *pEditEngine = GetEditEngine();
    DBG_ASSERT( pEditView, "NULL pointer" );
    DBG_ASSERT( pEditEngine, "NULL pointer" );
    if (pEditEngine  &&  pEditView)
    {
        ESelection eSelection = pEditView->GetSelection();
        USHORT     Pos        = eSelection.nEndPos;
        String     aMark (C2S("<?>"));
        String     aText;
        USHORT     nCounts    = pEditEngine->GetParagraphCount();

        while (eSelection.nEndPara < nCounts)
        {
            aText = pEditEngine->GetText( eSelection.nEndPara );
            Pos   = aText.Search(aMark, Pos);

            if (Pos != STRING_NOTFOUND)
            {
                pEditView->SetSelection(ESelection (eSelection.nEndPara, Pos, eSelection.nEndPara, Pos + 3));
                break;
            }

            Pos = 0;
            eSelection.nEndPara++;
        }
    }
}

void SmEditWindow::SelPrevMark()
{
    EditEngine *pEditEngine = GetEditEngine();
    DBG_ASSERT( pEditEngine, "NULL pointer" );
    DBG_ASSERT( pEditView, "NULL pointer" );
    if (pEditEngine  &&  pEditView)
    {
        ESelection eSelection = pEditView->GetSelection();
        USHORT     Pos        = STRING_NOTFOUND;
        xub_StrLen Max        = eSelection.nStartPos;
        String     Text( pEditEngine->GetText( eSelection.nStartPara ) );
        String     aMark (C2S("<?>"));
        USHORT     nCounts    = pEditEngine->GetParagraphCount();

        do
        {
            USHORT Fnd = Text.Search(aMark, 0);

            while ((Fnd < Max) && (Fnd != STRING_NOTFOUND))
            {
                Pos = Fnd;
                Fnd = Text.Search(aMark, Fnd + 1);
            }

            if (Pos == STRING_NOTFOUND)
            {
                eSelection.nStartPara--;
                Text = pEditEngine->GetText( eSelection.nStartPara );
                Max = Text.Len();
            }
        }
        while ((eSelection.nStartPara < nCounts) &&
            (Pos == STRING_NOTFOUND));

        if (Pos != STRING_NOTFOUND)
        {
            pEditView->SetSelection(ESelection (eSelection.nStartPara, Pos, eSelection.nStartPara, Pos + 3));
        }
    }
}

BOOL SmEditWindow::HasMark(const String& rText) const
    // returns true iff 'rText' contains a mark
{
    return rText.SearchAscii("<?>", 0) != STRING_NOTFOUND;
}

void SmEditWindow::MouseMove(const MouseEvent &rEvt)
{
    if (pEditView)
        pEditView->MouseMove(rEvt);
}

sal_Int8 SmEditWindow::AcceptDrop( const AcceptDropEvent& /*rEvt*/ )
{
    return pEditView ? /*pEditView->QueryDrop( rEvt )*/DND_ACTION_NONE: DND_ACTION_NONE;
}

sal_Int8 SmEditWindow::ExecuteDrop( const ExecuteDropEvent& /*rEvt*/ )
{
    return pEditView ? /*pEditView->Drop( rEvt )*/DND_ACTION_NONE : DND_ACTION_NONE;
}

ESelection SmEditWindow::GetSelection() const
{
    // pointer may be 0 when reloading a document and the old view
    // was already destroyed
    //(DBG_ASSERT( pEditView, "NULL pointer" );
    ESelection eSel;
    if (pEditView)
        eSel = pEditView->GetSelection();
    return eSel;
}

void SmEditWindow::SetSelection(const ESelection &rSel)
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    if (pEditView)
        pEditView->SetSelection(rSel);
    InvalidateSlots();
}

BOOL SmEditWindow::IsEmpty() const
{
    EditEngine *pEditEngine = ((SmEditWindow *) this)->GetEditEngine();
    BOOL bEmpty = sal::static_int_cast< BOOL >(
                    pEditEngine ? pEditEngine->GetTextLen() == 0 : FALSE);
    return bEmpty;
}

BOOL SmEditWindow::IsSelected() const
{
    return pEditView ? pEditView->HasSelection() : FALSE;
}

void SmEditWindow::Cut()
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        pEditView->Cut();
        GetDoc()->SetModified( TRUE );
    }
}

void SmEditWindow::Copy()
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
        pEditView->Copy();
}

void SmEditWindow::Paste()
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        pEditView->Paste();
        GetDoc()->SetModified( TRUE );
    }
}

void SmEditWindow::Delete()
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        pEditView->DeleteSelected();
        GetDoc()->SetModified( TRUE );
    }
}

void SmEditWindow::InsertText(const String& Text)
{
    DBG_ASSERT( pEditView, "EditView missing" );
    if (pEditView)
    {
        pEditView->InsertText(Text);
        aModifyTimer.Start();
    }
}

void SmEditWindow::Flush()
{
    EditEngine *pEditEngine = GetEditEngine();
    if (pEditEngine  &&  pEditEngine->IsModified())
    {
        pEditEngine->ClearModifyFlag();
        SmViewShell *pViewSh = rCmdBox.GetView();
        if (pViewSh)
        {
            pViewSh->GetViewFrame()->GetDispatcher()->Execute(
                    SID_TEXT, SFX_CALLMODE_STANDARD,
                    new SfxStringItem(SID_TEXT, GetText()), 0L);
        }
    }

    if (aCursorMoveTimer.IsActive())
    {
        aCursorMoveTimer.Stop();
        // ggf noch die (neue) FormulaCursor Position setzen
        CursorMoveTimerHdl(&aCursorMoveTimer);
    }
}


void SmEditWindow::DeleteEditView( SmViewShell & /*rView*/ )
{
    if (pEditView)
    {
        EditEngine *pEditEngine = pEditView->GetEditEngine();
        if (pEditEngine)
        {
            pEditEngine->SetStatusEventHdl( Link() );
            pEditEngine->RemoveView( pEditView );
        }
        delete pEditView;
        pEditView = 0;
    }
}


uno::Reference< XAccessible > SmEditWindow::CreateAccessible()
{
    if (!pAccessible)
    {
        pAccessible = new SmEditAccessible( this );
        xAccessible = pAccessible;
        pAccessible->Init();
    }
    return xAccessible;
}

