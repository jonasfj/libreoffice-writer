/*************************************************************************
 *
 *  $RCSfile: edit.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: tl $ $Date: 2001-05-30 13:11:05 $
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

#pragma hdrstop

#include "starmath.hrc"
#define ITEMID_FONT 		1
#define ITEMID_FONTHEIGHT	2
#define ITEMID_LRSPACE		3
#define ITEMID_WEIGHT		4


#ifndef _SV_MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _MyEDITVIEW_HXX //autogen
#include <svx/editview.hxx>
#endif
#ifndef _MyEDITENG_HXX //autogen
#include <svx/editeng.hxx>
#endif
#ifndef _EDITSTAT_HXX //autogen
#include <svx/editstat.hxx>
#endif
#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif


#include "edit.hxx"
#include "view.hxx"
#include "document.hxx"
#include "config.hxx"

#define SCROLL_LINE 		24

#define MINWIDTH		200
#define MINHEIGHT		200
#define MINSPLIT		40
#define SPLITTERWIDTH	2


////////////////////////////////////////


void SmGetLeftSelectionPart(const ESelection aSel,
                            USHORT &nPara, USHORT &nPos)
    // returns paragraph number and position of the selections left part
{
    // compare start and end of selection and use the one that comes first
    if (	aSel.nStartPara <  aSel.nEndPara
        ||	aSel.nStartPara == aSel.nEndPara  &&  aSel.nStartPos < aSel.nEndPos)
    {	nPara = aSel.nStartPara;
        nPos  = aSel.nStartPos;
    }
    else
    {	nPara = aSel.nEndPara;
        nPos  = aSel.nEndPos;
    }
}

////////////////////////////////////////

SmEditWindow::SmEditWindow( Window* pParent ) :
    Window				(pParent),
    pEditEngine			(0),
    pEditEngineItemPool	(0),
    pEditView			(0),
    pHScrollBar			(0),
    pVScrollBar			(0),
    pScrollBox			(0)
{
    SetHelpId(HID_SMA_COMMAND_WIN_EDIT);
    EnableDrop();
    SetMapMode(MAP_PIXEL);

    SetBackground( GetSettings().GetStyleSettings().GetWindowColor() );

    aModifyTimer.SetTimeoutHdl(LINK(this, SmEditWindow, ModifyTimerHdl));
    aModifyTimer.SetTimeout(2000);
    aModifyTimer.Start();

    aCursorMoveTimer.SetTimeoutHdl(LINK(this, SmEditWindow, CursorMoveTimerHdl));
    aCursorMoveTimer.SetTimeout(500);

    SmViewShell *pViewSh = SmGetActiveView();
    DBG_ASSERT( pViewSh, "view shell missing" );
    SmDocShell * pDocSh = NULL;
    if (pViewSh)
        pDocSh = pViewSh->GetDoc();
    DBG_ASSERT( pDocSh, "doc shell missing" );
    if (pDocSh)
    {
        SetEditEngine( &pDocSh->GetEditEngine(), 
                       &pDocSh->GetEditEngineItemPool() );
    }
}

SmEditWindow::~SmEditWindow()
{
    aCursorMoveTimer.Stop();
    aModifyTimer.Stop();

    if (pEditEngine)	   
        pEditEngine->SetStatusEventHdl( Link() );
    if (pEditEngine && pEditView)
        pEditEngine->RemoveView( pEditView );
    delete pEditView;
    delete pHScrollBar;
    delete pVScrollBar;
    delete pScrollBox;
}


void SmEditWindow::SetEditEngine( EditEngine *pEng, SfxItemPool *pPool )
{
    DBG_ASSERT( pEng, "NULL pointer" );
    DBG_ASSERT( pPool, "NULL pointer" );
    pEditEngine			= pEng;
    pEditEngineItemPool	= pPool;
}


void SmEditWindow::ImplSetFont()
{
/*	
    SetPointFont( GetSettings().GetStyleSettings().GetAppFont() );

    if (pEditEngine && pEditEngineItemPool)
    {
        Font aFont = GetFont();
        pEditEngine->SetDefTab( USHORT( GetTextWidth( C2S("XXXX") ) ) );

        long nFntHeight = aFont.GetSize().Height();

        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontItem( aFont.GetFamily(), aFont.GetName(),
                    aFont.GetStyleName(), aFont.GetPitch(), aFont.GetCharSet(),
                    EE_CHAR_FONTINFO ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT_CJK ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT_CTL ) );
    
        // forces new settings to be used		   
        pEditEngine->Clear();	//#77957 incorrect font size
    }
*/
}

void SmEditWindow::DataChanged( const DataChangedEvent& )
{
    SetBackground( GetSettings().GetStyleSettings().GetWindowColor() );


//	ImplSetFont();
    SetPointFont( GetSettings().GetStyleSettings().GetAppFont() );

    if (pEditEngine && pEditEngineItemPool)
    {
        Font aFont = GetFont();
        pEditEngine->SetDefTab( USHORT( GetTextWidth( C2S("XXXX") ) ) );

        long nFntHeight = aFont.GetSize().Height();

        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontItem( aFont.GetFamily(), aFont.GetName(),
                    aFont.GetStyleName(), aFont.GetPitch(), aFont.GetCharSet(),
                    EE_CHAR_FONTINFO ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT_CJK ) );
        pEditEngineItemPool->SetPoolDefaultItem( 
                SvxFontHeightItem( nFntHeight, 100, EE_CHAR_FONTHEIGHT_CTL ) );
    
        // forces new settings to be used		   
        pEditEngine->Clear();	//#77957 incorrect font size
    }

    AdjustScrollBars();
    Resize();
}

IMPL_LINK( SmEditWindow, ModifyTimerHdl, Timer *, pTimer )
{
    SmModule *pp = SM_MOD1();
    if (pp->GetConfig()->IsAutoRedraw())
        Flush();
    aModifyTimer.Start();
    return 0;
}


IMPL_LINK(SmEditWindow, CursorMoveTimerHdl, Timer *, pTimer)
    // every once in a while check cursor position (selection) of edit
    // window and if it has changed (try to) set the formula-cursor
    // according to that.
{
    ESelection	aNewSelection   (GetSelection());

    if (!aNewSelection.IsEqual(aOldSelection))
    {	SmViewShell *pView = SmGetActiveView();
        //! woher wei� man, dass die Shell die wir hier kriegen die ist, die
        //! zu dem SmEditWindow gehoert fuer das er gestartet wurde?
        //! (Eine ViewShell vom Math ist es immer)
        //! Siehe auch Kommentar zu:
        //! SmEditWindow::SetText  und  SmViewShell::Activate

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
    if (rCEvt.GetCommand() == COMMAND_CONTEXTMENU)
    {
        GetParent()->ToTop();

        PopupMenu* pPopupMenu = new PopupMenu(SmResId(RID_COMMANDMENU));
        pPopupMenu->SetSelectHdl(LINK(this, SmEditWindow, MenuSelectHdl));

        pPopupMenu->Execute( this, OutputToScreenPixel(
                                        rCEvt.GetMousePosPixel() ) );
        delete pPopupMenu;
    }
    else if (pEditView)
        pEditView->Command( rCEvt );
    else
        Window::Command (rCEvt);
}

IMPL_LINK_INLINE_START( SmEditWindow, MenuSelectHdl, Menu *, pMenu )
{
    SmViewShell *pViewSh = SmGetActiveView();
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
        SfxViewShell* pViewShell = SfxViewShell::Current();
        if ( pViewShell && pViewShell->ISA(SmViewShell) )
        {
            SmDocShell* pDocSh = (SmDocShell*) pViewShell->GetViewFrame()->GetObjectShell();
            if (pDocSh)
            {
                    /* fuert zum (sofortigen) Zerstoeren von this! */
                pDocSh->DoInPlaceActivate( FALSE );
                bCallBase = FALSE;
            }
        }
        if ( bCallBase )
            Window::KeyInput( rKEvt );
    }
    else
    {
        SmViewShell *pView = SmGetActiveView();
        SmDocShell *pDocShell = pView ? pView->GetDoc() : 0;
        if (pDocShell)
            pDocShell->SetModified( TRUE );

        // Timer neu starten, um den Handler (auch bei l�ngeren Eingaben)
        // m�glichst nur einmal am Ende aufzurufen.
        aCursorMoveTimer.Start();

        if ( !pEditView->PostKeyEvent(rKEvt) )
        {
            if ( !SfxViewShell::Current()->KeyInput(rKEvt) )
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
                SfxViewShell* pVShell = SfxViewShell::Current();
                if ( pVShell && pVShell->ISA(SmViewShell) &&
                     ((SmViewShell*)pVShell)->GetGraphicWindow().HasFocus() )
                {
                    GrabFocus();
                }
            }
        }
        else
            aModifyTimer.Start();
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
    DBG_ASSERT( pEditEngine, "EditEngine missing" );
    if (!pEditView && pEditEngine)
    {
        //pEditEngineItemPool = EditEngine::CreatePool();
        //pEditEngine = new EditEngine( pEditEngineItemPool );
        pEditView = new EditView( pEditEngine, this );
//		pEditEngine->SetUpdateMode( FALSE );
        pEditEngine->InsertView( pEditView );

#ifdef NEVER		
        pEditEngine->SetControlWord(
                (pEditEngine->GetControlWord() | EE_CNTRL_AUTOINDENTING) &
                (~EE_CNTRL_UNDOATTRIBS) & 
                (~EE_CNTRL_PASTESPECIAL) );

        pEditEngine->SetWordDelimiters( C2S(" .=+-*/(){}[];\"" ) );
        pEditEngine->SetRefMapMode( MAP_PIXEL );
#endif

//		ImplSetFont();

//		pEditEngine->SetPaperSize( Size( 800, 0 ) );

        pVScrollBar = new ScrollBar(this, WinBits(WB_VSCROLL));
        pHScrollBar = new ScrollBar(this, WinBits(WB_HSCROLL));
        pVScrollBar->SetScrollHdl(LINK(this, SmEditWindow, ScrollHdl));
        pHScrollBar->SetScrollHdl(LINK(this, SmEditWindow, ScrollHdl));
        pScrollBox  = new ScrollBarBox(this);

        pEditView->SetOutputArea(AdjustScrollBars());

        ESelection eSelection;

        pEditView->SetSelection(eSelection);
//		pEditEngine->SetUpdateMode( TRUE );
        Update();
        pEditView->ShowCursor(TRUE, TRUE);

        pEditEngine->SetStatusEventHdl( LINK(this, SmEditWindow, EditStatusHdl) );
        SetPointer(pEditView->GetPointer());

//		pEditEngine->EraseVirtualDevice();
//		pEditEngine->ClearModifyFlag();

        SetScrollBarRanges();
    }
}


IMPL_LINK( SmEditWindow, EditStatusHdl, EditStatus *, pStat )
{
    if (! pEditView)
        return 1;
    else
    {
        SetScrollBarRanges();
        return 0;
    }
}

IMPL_LINK_INLINE_START( SmEditWindow, ScrollHdl, ScrollBar *, pScrollBar )
{
    pEditView->SetVisArea(Rectangle(Point(pHScrollBar->GetThumbPos(),
                                          pVScrollBar->GetThumbPos()),
                                    pEditView->GetVisArea().GetSize()));
    pEditView->Invalidate();
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
    if (pVScrollBar && pHScrollBar && pScrollBox)
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


XubString SmEditWindow::GetText()
{
    String aText;
    DBG_ASSERT( pEditEngine, "EditEngine missing" );
    if (pEditEngine)
        aText = pEditEngine->GetText( LINEEND_LF );
    return aText;
}


void SmEditWindow::SetText(const XubString& rText)
{
    DBG_ASSERT( pEditEngine, "EditEngine missing" );
    if (pEditEngine  &&  !pEditEngine->IsModified())
    {
        if (!pEditView)
            CreateEditView();

        ESelection eSelection = pEditView->GetSelection();

        pEditEngine->SetText(rText);
        pEditEngine->ClearModifyFlag();

        //! Hier die Timer neu zu starten verhindert, dass die Handler f�r andere
        //! (im Augenblick nicht mehr aktive) Math Tasks aufgerufen werden.
        aModifyTimer.Start();
        aCursorMoveTimer.Start();

        pEditView->SetSelection(eSelection);
    }
}
    
    
void SmEditWindow::GetFocus()
{
    Window::GetFocus();

    if (pEditView)
        pEditView->SetSelection( aActiveSelection );
    if (pEditEngine)
        pEditEngine->SetStatusEventHdl( LINK(this, SmEditWindow, EditStatusHdl) );
}


void SmEditWindow::LoseFocus()
{
    if (pEditView)
        aActiveSelection = pEditView->GetSelection();
    if (pEditEngine)
        pEditEngine->SetStatusEventHdl( Link() );
    
    Window::LoseFocus();
}


BOOL SmEditWindow::IsAllSelected() const
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    ESelection eSelection;
    if (pEditView)
        eSelection = pEditView->GetSelection();

    DBG_ASSERT( pEditEngine, "NULL pointer" );
    INT32 nParaCnt = pEditEngine->GetParagraphCount();
    if (!(nParaCnt - 1))
    {
        String Text( pEditEngine->GetText( LINEEND_LF ) );
        return !eSelection.nStartPos && (eSelection.nEndPos == Text.Len () - 1);
    }
    else
    {
        return !eSelection.nStartPara && (eSelection.nEndPara == nParaCnt - 1);
    }
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

void SmEditWindow::InsertCommand(USHORT Command)
{
    //Anfang der Selektion merken und hinterher den Cursor daraufsetzen. Nur so
    //macht das SelNextMark() Sinn.
    ESelection aSelection = pEditView->GetSelection();
    aSelection.nEndPos	= aSelection.nStartPos;
    aSelection.nEndPara = aSelection.nStartPara;

    DBG_ASSERT( pEditView, "NULL pointer" );
    String  aText = String(SmResId(Command));
    pEditView->InsertText(aText);

    if (HasMark(aText))
    {	// set selection to next mark
        pEditView->SetSelection(aSelection);
        SelNextMark();
    }
    else
    {	// set selection after inserted text
        aSelection.nStartPos  =
        aSelection.nEndPos   += aText.Len();
        pEditView->SetSelection(aSelection);
    }

    GrabFocus();
}

void SmEditWindow::MarkError(const Point &rPos)
{
    const int Col = rPos.X();
    const int Row = rPos.Y() - 1;

    pEditView->SetSelection(ESelection (Row, Col - 1, Row, Col));
    GrabFocus();
}

void SmEditWindow::SelNextMark()
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    ESelection eSelection = pEditView->GetSelection();
    USHORT     Pos        = eSelection.nEndPos;
    String     aMark (C2S("<?>"));
    String     aText;
    DBG_ASSERT( pEditEngine, "NULL pointer" );
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

void SmEditWindow::SelPrevMark()
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    ESelection eSelection = pEditView->GetSelection();
    USHORT     Pos        = STRING_NOTFOUND;
    xub_StrLen Max        = eSelection.nStartPos;
    DBG_ASSERT( pEditEngine, "NULL pointer" );
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

BOOL SmEditWindow::Drop(const DropEvent& rEvt)
{
    return pEditView ? pEditView->Drop( rEvt ) : FALSE;
}

BOOL SmEditWindow::QueryDrop(DropEvent& rEvt)
{
    return pEditView ? pEditView->QueryDrop( rEvt ) : FALSE;
}

ESelection SmEditWindow::GetSelection() const
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    ESelection eSel;
    if (pEditView)
        eSel = pEditView->GetSelection();
    return eSel;
}

void SmEditWindow::SetSelection(const ESelection &rSel)
{
    DBG_ASSERT( pEditView, "NULL pointer" );
    pEditView->SetSelection(rSel);
}

BOOL SmEditWindow::IsEmpty() const
{
    DBG_ASSERT( pEditEngine, "NULL pointer" );
    return pEditEngine->GetTextLen() == 0;
}

BOOL SmEditWindow::IsSelected() const
{
    return pEditView->HasSelection();
}

void SmEditWindow::Cut()
{
    pEditView->Cut();
}

void SmEditWindow::Copy()
{
    pEditView->Copy();
}

void SmEditWindow::Paste()
{
    pEditView->Paste();
}

void SmEditWindow::Delete()
{
    pEditView->DeleteSelected();
}

void SmEditWindow::InsertText(const String& Text)
{
    pEditView->InsertText(Text);
}

void SmEditWindow::Flush()
{
    if (pEditEngine  &&  pEditEngine->IsModified())
    {
        pEditEngine->ClearModifyFlag();
        SmViewShell *pViewSh = SmGetActiveView();
        if (pViewSh)
            pViewSh->GetViewFrame()->GetDispatcher()->Execute(
                    SID_TEXT, SFX_CALLMODE_STANDARD,
                    new SfxStringItem(SID_TEXT, GetText()), 0L);
    }

    if (aCursorMoveTimer.IsActive())
    {
        aCursorMoveTimer.Stop();
        // ggf noch die (neue) FormulaCursor Position setzen
        CursorMoveTimerHdl(&aCursorMoveTimer);
    }
}


