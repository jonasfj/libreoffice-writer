/************************************************************************* *
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: postit.cxx,v $
 * $Revision: 1.4 $
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


#include "precompiled_sw.hxx"
#include <postit.hxx>
#include <PostItMgr.hxx>

#ifndef _POPUP_HRC
#include <popup.hrc>
#endif
#ifndef _DOCVW_HRC
#include <docvw.hrc>
#endif

#include <hintids.hxx>
#include "viewopt.hxx"
#include "cmdid.h"

#include <vcl/scrbar.hxx>
#include <vcl/button.hxx>
#include <tools/poly.hxx>   // Polygon
#include <vcl/svapp.hxx>
#include <vcl/gradient.hxx>

#include <svx/eeitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/bulitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/shdditem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/colritem.hxx>
#include <svx/flditem.hxx>

#include <svtools/zforlist.hxx>

#include <svx/editview.hxx>
#include <svx/svdview.hxx>
#include <tools/poly.hxx>	// Polygon
#include <basegfx/matrix/b2dhommatrix.hxx>

#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/mnumgr.hxx>

#include <vcl/salbtype.hxx>	// FRound


#ifndef _VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif

#include <swrect.hxx>
#include <svx/sdrpaintwindow.hxx>
#include <svx/sdr/overlay/overlaymanager.hxx>
#include <svx/editstat.hxx> //EditEngine flags
#include <svx/outliner.hxx>
#include <svx/editeng.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/tuple/b2dtuple.hxx>

#include <docufld.hxx> // SwPostItField
#include <edtwin.hxx>
#include <view.hxx>
#include <docsh.hxx>
#include <fmtfld.hxx>
#include <wrtsh.hxx>
#include <doc.hxx>

#include <vcl/edit.hxx>
#include <svtools/svmedit.hxx>

#include <txtfld.hxx>
#include <SwUndoField.hxx>
#include "../../core/inc/docfld.hxx"   // for expression fields
#include <editsh.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>

#define ANKORLINE_WIDTH			1
#define METABUTTON_WIDTH		16
#define METABUTTON_HEIGHT		18
#define METABUTTON_AREA_WIDTH	30
#define POSTIT_META_HEIGHT	(sal_Int32)		30
#define POSTIT_MINIMUMSIZE_WITHOUT_META		50


Color ColorFromAlphaColor(UINT8 aTransparency, Color &aFront, Color &aBack )
{
    return Color((UINT8)(aFront.GetRed()	* aTransparency/(double)255	+ aBack.GetRed()	* (1-aTransparency/(double)255)),
                 (UINT8)(aFront.GetGreen()	* aTransparency/(double)255	+ aBack.GetGreen()	* (1-aTransparency/(double)255)),
                 (UINT8)(aFront.GetBlue()	* aTransparency/(double)255	+ aBack.GetBlue()	* (1-aTransparency/(double)255)));
}

/************ PostItTxt **************************************/
PostItTxt::PostItTxt(Window* pParent, WinBits nBits) : Window(pParent, nBits), mpOutlinerView(0),mMouseOver(false),mbShowPopup(FALSE)
{
    AddEventListener( LINK( this, PostItTxt, WindowEventListener ) );
    mpPostIt = static_cast<SwPostIt*>(GetParent());
}

PostItTxt::~PostItTxt()
{
    RemoveEventListener( LINK( this, PostItTxt, WindowEventListener ) );
}

void PostItTxt::GetFocus()
{
    if(mpPostIt)
         mpPostIt->Mgr()->SetActivePostIt(mpPostIt);
    Window::GetFocus();
    Invalidate();
}

void PostItTxt::LoseFocus()
{
    // write the visible text back into the SwField
    if ( mpPostIt )
    {
        mpPostIt->UpdateData();
        mpPostIt->Engine()->ClearModifyFlag();
        mpPostIt->Engine()->GetUndoManager().Clear();
    }

    Window::LoseFocus();
    Invalidate();
}

void PostItTxt::Paint( const Rectangle& rRect)
{
    if ( !Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
    {
        if (mMouseOver || HasFocus())
            DrawGradient(Rectangle(Point(0,0),PixelToLogic(GetSizePixel())),Gradient(GRADIENT_LINEAR,mColorDark,mColorDark));
        else
            DrawGradient(Rectangle(Point(0,0),PixelToLogic(GetSizePixel())),Gradient(GRADIENT_LINEAR,mColorLight,mColorDark));
     }
    mpOutlinerView->Paint( rRect );
}

void PostItTxt::SetColor(Color &aColorDark,Color &aColorLight)
{
        mColorDark = aColorDark;
        mColorLight = aColorLight;
}

void PostItTxt::KeyInput( const KeyEvent& rKeyEvt )
{
    const KeyCode& rKeyCode = rKeyEvt.GetKeyCode();
    USHORT nKey = rKeyCode.GetCode();
    SwView* pView = mpPostIt->DocView();

    if ( (nKey== KEY_N) && rKeyCode.IsMod1() && rKeyCode.IsMod2())
    {
        if ( mpPostIt->Mgr()->GetActivePostIt() == mpPostIt )
            mpPostIt->Mgr()->SetActivePostIt(0);
        mpPostIt->SwitchToFieldPos();
    }
    else
    if  (((nKey == KEY_PAGEUP) && rKeyCode.IsMod1() && rKeyCode.IsMod2()) ||
         ((nKey == KEY_PAGEDOWN) && rKeyCode.IsMod1() && rKeyCode.IsMod2() ))
    {
        mpPostIt->SwitchToPostIt(nKey);
    }
    else
    if ((nKey == KEY_ESCAPE))
    {
        if ( mpPostIt->Mgr()->GetActivePostIt() == mpPostIt )
            mpPostIt->Mgr()->SetActivePostIt(0);
        if (!mpPostIt->IsReadOnly() && mpPostIt->Engine()->GetEditEngine().GetText()==String(::rtl::OUString::createFromAscii("")))
            mpPostIt->Delete();
        else
            mpPostIt->SwitchToFieldPos();
    }
    else
    {
        //let's make sure we see our note
        mpPostIt->Mgr()->MakeVisible(mpPostIt);

        // we need EnableOutput, otherwise text might jump up and down
        // e.g ENTER->text jumps up, if we then resize based on the new hight, it jumps back down
        //EnableOutput(FALSE);
        //TODO: EnableOutput is slow as hell, we need sth like VirtualKeyInput
        long aOldHeight = mpPostIt->GetPostItTextHeight();
        bool bDone = false;
        if ( !( (nKey == KEY_Z || nKey == KEY_Y) && rKeyCode.IsMod1() ) )
            // HACK: need to switch off processing of Undo/Redo in Outliner
            bDone = mpOutlinerView->PostKeyEvent( rKeyEvt );
        if (bDone)
        {
            mpPostIt->ResizeIfNeccessary(aOldHeight,mpPostIt->GetPostItTextHeight());
        }
        else
        {
            if (!pView->KeyInput(rKeyEvt))
                Window::KeyInput(rKeyEvt);
        }
    }

    pView->GetViewFrame()->GetBindings().InvalidateAll(FALSE);
}

void PostItTxt::MouseMove( const MouseEvent& rMEvt )
{
    if ( mpOutlinerView )
        mpOutlinerView->MouseMove( rMEvt );

    // mba: why does OutlinerView not handle the modifier setting?!
    // this forces the postit to handle *all* pointer types
    if ( rMEvt.GetModifier() == KEY_MOD1 )
        SetPointer( mpOutlinerView->GetPointer( rMEvt.GetPosPixel() ) );
    else
        SetPointer( Pointer( mpOutlinerView->GetOutliner()->IsVertical() ? POINTER_TEXT_VERTICAL : POINTER_TEXT ) );
}

void PostItTxt::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( rMEvt.GetModifier() == KEY_MOD1 && mpOutlinerView )
    {
        const EditView& aEV = mpOutlinerView->GetEditView();
        const SvxFieldItem* pItem = aEV.GetFieldUnderMousePointer();
        if ( pItem )
        {
            const SvxFieldData* pFld = pItem->GetField();
            const SvxURLField* pURL = PTR_CAST( SvxURLField, pFld );
            if ( pURL )
            {
                mpOutlinerView->MouseButtonDown( rMEvt );
                SwWrtShell &rSh = mpPostIt->DocView()->GetWrtShell();
                String sURL( pURL->GetURL() );
                String sTarget( pURL->GetTargetFrame() );
                ::LoadURL( sURL, &rSh, URLLOAD_NOFILTER, &sTarget);
                return;
            }
        }
    }

    GrabFocus();
    if ( mpOutlinerView )
        mpOutlinerView->MouseButtonDown( rMEvt );
    mpPostIt->DocView()->GetViewFrame()->GetBindings().InvalidateAll(FALSE);
}

void PostItTxt::MouseButtonUp( const MouseEvent& rMEvt )
{
    if ( mpOutlinerView )
        mpOutlinerView->MouseButtonUp( rMEvt );
}

void PostItTxt::Command( const CommandEvent& rCEvt )
{
    if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        if (mpOutlinerView->IsWrongSpelledWordAtPos( rCEvt.GetMousePosPixel(),TRUE ))
            mpOutlinerView->ExecuteSpellPopup(rCEvt.GetMousePosPixel());
        else
        {
            SfxPopupMenuManager* aMgr = mpPostIt->DocView()->GetViewFrame()->GetDispatcher()->Popup(0, this,&rCEvt.GetMousePosPixel());
            XubString aText = ((PopupMenu*)aMgr->GetSVMenu())->GetItemText( FN_DELETE_NOTE_AUTHOR );
            aText += mpPostIt->GetAuthor();
            ((PopupMenu*)aMgr->GetSVMenu())->SetItemText(FN_DELETE_NOTE_AUTHOR,aText);

            XubString aText2 = ((PopupMenu*)aMgr->GetSVMenu())->GetItemText( FN_DELETE_NOTE_AUTHOR );

            if (rCEvt.IsMouseEvent())
            {
                //mpPostIt->DocView()->GetViewFrame()->GetDispatcher()->ExecutePopup( 0, this,&rCEvt.GetMousePosPixel());
                ((PopupMenu*)aMgr->GetSVMenu())->Execute(this,rCEvt.GetMousePosPixel());
            }
            else
            {
                const Size aSize = GetSizePixel();
                const Point aPos = Point( aSize.getWidth()/2, aSize.getHeight()/2 );
                //mpPostIt->DocView()->GetViewFrame()->GetDispatcher()->ExecutePopup( 0, this,&aPos);
                ((PopupMenu*)aMgr->GetSVMenu())->Execute(this,aPos);
            }
            delete aMgr;
        }
    }
    else
    if (rCEvt.GetCommand() == COMMAND_WHEEL)
    {
        if (mpPostIt->Scrollbar()->IsVisible())
        {
            const CommandWheelData* pData = rCEvt.GetWheelData();
            if (pData->IsShift() || pData->IsMod1() || pData->IsMod2())
            {
                mpPostIt->DocView()->HandleWheelCommands(rCEvt);
            }
            else
            {
                HandleScrollCommand( rCEvt, 0 , mpPostIt->Scrollbar());

                /*
                long nLines = pData->GetNotchDelta() * (long)pData->GetScrollLines();
                if ( ((mpPostIt->Scrollbar()->GetRange().Min() == mpPostIt->Scrollbar()->GetThumbPos()) && (nLines > 0)) ||
                    ( (mpPostIt->Scrollbar()->GetRange().Max() == mpPostIt->Scrollbar()->GetThumbPos()+mpPostIt->Scrollbar()->GetVisibleSize()) &&  (nLines < 0)) )
                {
                    mpPostIt->DocView()->HandleWheelCommands(rCEvt);
                }
                else
                {
                    HandleScrollCommand( rCEvt, 0 , mpPostIt->Scrollbar());
                }
                */
            }
        }
        else
        {
            mpPostIt->DocView()->HandleWheelCommands(rCEvt);
        }
    }
    else
    {
        if ( mpOutlinerView )
            mpOutlinerView->Command( rCEvt );
        else
            Window::Command(rCEvt);
    }
}

void PostItTxt::DataChanged( const DataChangedEvent& aData)
{
    Window::DataChanged( aData );
}

IMPL_LINK( PostItTxt, WindowEventListener, VclSimpleEvent*, pWinEvent )
{
    if ( pWinEvent && pWinEvent->ISA( VclWindowEvent ) )
    {
        VclWindowEvent *pEvent = (VclWindowEvent*)pWinEvent;
        if (pEvent->GetId() == VCLEVENT_WINDOW_MOUSEMOVE)
        {
            MouseEvent* pMouseEvt = (MouseEvent*)pEvent->GetData();
            if ( pMouseEvt->IsEnterWindow() )
            {
                mMouseOver = true;
                if (mpPostIt->Ankor())
                    mpPostIt->Ankor()->SetLineInfo(LineInfo(LINE_SOLID,ANKORLINE_WIDTH*15));
                Invalidate();
            }
            else if ( pMouseEvt->IsLeaveWindow())
            {
                mMouseOver = false;
                if (!mbShowPopup && !HasFocus())
                {
                    if (mpPostIt->Ankor())
                        mpPostIt->Ankor()->SetLineInfo(LineInfo(LINE_DASH,ANKORLINE_WIDTH*15));
                    Invalidate();
                }
            }
        }
    }
    return sal_True;
}


/******* SwPostIt **************************************/
SwPostIt::SwPostIt( Window* pParent, WinBits nBits, SwFmtFld* aField,SwPostItMgr* aMgr) : Window(pParent, nBits),
    mpOutlinerView(0),
    mpOutliner(0),
    mpPostItTxt(0),
    mpMeta(0),
    mpVScrollbar(0),
    mpFmtFld(aField),
    mpFld( static_cast<SwPostItField*>(aField->GetFld())),
    mpAnkor(0),
    mpMgr(aMgr),
    mbMeta(true),
    mpButtonPopup(new PopupMenu(SW_RES(MN_ANNOTATION_BUTTON)))
{
    SwEditWin* aWin = static_cast<SwEditWin*>(GetParent());
    mpView = &aWin->GetView();

    SdrPaintWindow* pPaintWindow = mpView->GetDrawView()->GetPaintWindow(0);
    if(pPaintWindow)
    {
        pOverlayManager = pPaintWindow->GetOverlayManager();
    }

    InitControls();
    SetPostItText();
}

void SwPostIt::SetPostItText()
{
    // get text from SwPostItField and insert into our textview
    mpOutliner->SetModifyHdl( Link() );
    mpOutliner->EnableUndo( FALSE );
    mpFld = static_cast<SwPostItField*>(mpFmtFld->GetFld());
    if( mpFld->GetTextObject() )
        mpOutliner->SetText( *mpFld->GetTextObject() );
    else
    {
        mpOutliner->Clear();
        SfxItemSet item( mpView->GetDocShell()->GetPool() );
        item.Put(SvxFontHeightItem(200,100,EE_CHAR_FONTHEIGHT));
        mpOutlinerView->SetAttribs(item);
        mpOutlinerView->InsertText(mpFld->GetPar2(),false);
    }

    mpOutliner->ClearModifyFlag();
    mpOutliner->GetUndoManager().Clear();
    mpOutliner->EnableUndo( TRUE );
    mpOutliner->SetModifyHdl( LINK( this, SwPostIt, ModifyHdl ) );
    Invalidate();
}

SwPostIt::~SwPostIt()
{
    if (mpOutlinerView)
    {
        delete mpOutlinerView;
    }

    if (mpOutliner)
    {
        delete mpOutliner;
    }

    if (mpMeta)
    {
        mpMeta->RemoveEventListener( LINK( mpPostItTxt, PostItTxt, WindowEventListener ) );
        delete mpMeta;
    }

    if (mpPostItTxt)
    {
        delete mpPostItTxt;
    }

    if (mpVScrollbar)
    {
        delete mpVScrollbar;
    }

    if (mpAnkor)
    {
        if (mpAnkor->getOverlayManager())
        {
            // remove this object from the chain
            mpAnkor->getOverlayManager()->remove(*mpAnkor);
        }
        delete mpAnkor;
    }

    if (mpButtonPopup)
    {
        delete mpButtonPopup;
    }
}

void SwPostIt::Paint( const Rectangle& rRect)
{
    Window::Paint(rRect);

    if (mpMeta->IsVisible() )
    {
        //draw left over space
        if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
            SetFillColor(COL_BLACK);
        else
            SetFillColor(mColorDark);
        SetLineColor();
        DrawRect(PixelToLogic(Rectangle(Point(mpMeta->GetPosPixel().X()+mpMeta->GetSizePixel().Width(),mpMeta->GetPosPixel().Y()),Size(GetMetaButtonAreaWidth(),mpMeta->GetSizePixel().Height()))));

        if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode())
        {
            //draw rect around button
            SetFillColor(COL_BLACK);
            SetLineColor(COL_WHITE);
        }
        else
        {
            //draw button
            Gradient aGradient(GRADIENT_LINEAR,ColorFromAlphaColor(15,mColorAnkor,mColorDark),ColorFromAlphaColor(80,mColorAnkor,mColorDark));
            DrawGradient(mRectMetaButton,aGradient);
            //draw �rect around button
            SetFillColor();
            SetLineColor(ColorFromAlphaColor(90,mColorAnkor,mColorDark));
        }
        DrawRect(mRectMetaButton);

        //draw arrow
        if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
            SetFillColor(COL_WHITE);
        else
            SetFillColor(COL_BLACK);
        SetLineColor();
        DrawPolygon(Polygon(aPopupTriangle));
    }
}

void SwPostIt::SetPosSizePixelRect(long nX, long nY,long nWidth, long nHeight,const SwRect &aRect, const long aPageBorder)
{
    mbMeta = true;
    mPosSize = Rectangle(Point(nX,nY),Size(nWidth,nHeight));
    mAnkorRect = aRect;
    mPageBorder = aPageBorder;
}

void SwPostIt::SetSize( const Size& rNewSize )
{
    mPosSize.SetSize(rNewSize);
}

void SwPostIt::SetVirtualPosSize( const Point& aPoint, const Size& aSize)
{
    mPosSize = Rectangle(aPoint,aSize);
}

void SwPostIt::TranslateTopPosition(const long aAmount)
{
    mPosSize.Move(0,aAmount);
}

void SwPostIt::ShowAnkorOnly(const Point &aPoint)
{
    HideNote();
    SetPosAndSize();
    if (mpAnkor)
    {
        mpAnkor->SetSixthPosition(basegfx::B2DPoint(aPoint.X(),aPoint.Y()));
        mpAnkor->SetSeventhPosition(basegfx::B2DPoint(aPoint.X(),aPoint.Y()));
        mpAnkor->setVisible(true);
    }
}

void SwPostIt::InitControls()
{
    // actual window which holds the user text
    mpPostItTxt = new PostItTxt(this, 0);
    mpPostItTxt->SetPointer(Pointer(POINTER_TEXT));


    // window control for author and date
    mpMeta = new MultiLineEdit(this,0);
    mpMeta->SetReadOnly();
    mpMeta->SetRightToLeft(Application::GetSettings().GetLayoutRTL());
    mpMeta->AlwaysDisableInput(true);
    mpMeta->SetCallHandlersOnInputDisabled(true);
    mpMeta->AddEventListener( LINK( mpPostItTxt, PostItTxt, WindowEventListener ) );
    AddEventListener( LINK( mpPostItTxt, PostItTxt, WindowEventListener ) );

    CheckMetaText();

    SwDocShell* aShell = mpView->GetDocShell();
    mpOutliner = new Outliner(&aShell->GetPool(),OUTLINERMODE_TEXTOBJECT);
    aShell->GetDoc()->SetCalcFieldValueHdl( mpOutliner );
    // mpOutliner->EnableUndo( FALSE );
    mpOutliner->SetUpdateMode( TRUE );
    Rescale();

    OutputDevice* pDev = aShell->GetDoc()->getReferenceDevice(TRUE);
    if ( pDev )
    {
        mpOutliner->SetRefDevice( pDev );
    }

    mpOutlinerView = new OutlinerView ( mpOutliner, mpPostItTxt );
    mpOutlinerView->SetBackgroundColor(COL_TRANSPARENT);
    mpOutliner->InsertView(mpOutlinerView );
    mpPostItTxt->SetTextView(mpOutlinerView);

    SfxItemSet item(aShell->GetPool());
    item.Put(SvxFontHeightItem(200,100,EE_CHAR_FONTHEIGHT));
    mpOutlinerView->SetAttribs(item);

    //create Scrollbars
    mpVScrollbar = new ScrollBar(this, WB_3DLOOK |WB_VSCROLL|WB_DRAG);
    mpVScrollbar->EnableNativeWidget(false);
    mpVScrollbar->EnableRTL( false );
    mpVScrollbar->SetScrollHdl(LINK(this, SwPostIt, ScrollHdl));
    mpVScrollbar->EnableDrag();
    mpVScrollbar->AddEventListener( LINK( mpPostItTxt, PostItTxt, WindowEventListener ) );

    mpButtonPopup->SetMenuFlags(MENU_FLAG_ALWAYSSHOWDISABLEDENTRIES);

    const SwViewOption* pVOpt = mpView->GetWrtShellPtr()->GetViewOptions();
    ULONG nCntrl = mpOutliner->GetControlWord();
    // TODO: crash when AUTOCOMPLETE enabled
    nCntrl |= EE_CNTRL_AUTOCORRECT  | EV_CNTRL_AUTOSCROLL | EE_CNTRL_URLSFXEXECUTE | EE_CNTRL_ONLINESPELLING;; // | EE_CNTRL_AUTOCOMPLETE;
    if (!pVOpt->IsOnlineSpell())
        nCntrl |= EE_CNTRL_NOREDLINES;
    mpOutliner->SetControlWord(nCntrl);

    XubString aText = mpButtonPopup->GetItemText( FN_DELETE_NOTE_AUTHOR );
    aText += GetAuthor();
    mpButtonPopup->SetItemText(FN_DELETE_NOTE_AUTHOR,aText);

    mpOutlinerView->StartSpeller();

    mpMeta->Show();
    mpVScrollbar->Show();
    mpPostItTxt->Show();
}

void SwPostIt::CheckMetaText()
{
    /*
    String sDateTime;
    SvNumberFormatter* pNumFormatter = mpView->GetDocShell()->GetDoc()->GetNumberFormatter();
    const ULONG nFormatDate = pNumFormatter->GetStandardFormat( NUMBERFORMAT_DATETIME , Application::GetSettings().GetLanguage());
    const DateTime aDateTime( mpFld->GetDate(), mpFld->GetTime());
    pNumFormatter->GetOutputString( aDateTime - DateTime( *pNumFormatter->GetNullDate()), nFormatDate, sDateTime, &pColor );
    */
    const LocaleDataWrapper& rLocalData = SvtSysLocale().GetLocaleData();
    String sMeta = mpFld->GetPar1();
    if (mpFld->GetDate()==Date())
        sMeta = sMeta + rtl::OUString::createFromAscii("\n") + String(SW_RES(STR_POSTIT_TODAY));
    else
    if (mpFld->GetDate()==Date(Date()-1))
        sMeta = sMeta + rtl::OUString::createFromAscii("\n") + String(SW_RES(STR_POSTIT_YESTERDAY));
    else
        sMeta = sMeta + rtl::OUString::createFromAscii("\n") + rLocalData.getDate( mpFld->GetDate() );
    sMeta = sMeta + rtl::OUString::createFromAscii(" ")  + rLocalData.getTime( mpFld->GetTime(),false );
    if (mpMeta->GetText() != sMeta)
        mpMeta->SetText(sMeta);
}

void SwPostIt::Rescale()
{
    MapMode aMode = GetParent()->GetMapMode();
    aMode.SetOrigin( Point() );
    //aMode.SetScaleX( aMode.GetScaleX() * Fraction( 8, 10 ) );
    //aMode.SetScaleY( aMode.GetScaleY() * Fraction( 8, 10 ) );
    mpOutliner->SetRefMapMode( aMode );
    SetMapMode( aMode );
    mpPostItTxt->SetMapMode( aMode );
    if ( mpMeta )
    {
        Font aFont( mpMeta->GetSettings().GetStyleSettings().GetFieldFont() );
        sal_Int32 nHeight = aFont.GetHeight();
        nHeight = nHeight * aMode.GetScaleY().GetNumerator() / aMode.GetScaleY().GetDenominator();
        aFont.SetHeight( nHeight );
        mpMeta->SetControlFont( aFont );
    }
}

void SwPostIt::MetaInfo(const bool bMeta )
{
    mbMeta = bMeta;
    DoResize();
}

void SwPostIt::SetPosAndSize()
{
    if (GetSizePixel() != mPosSize.GetSize())
    {
        SetSizePixel(mPosSize.GetSize());
        DoResize();
    }

    if (GetPosPixel().X() != mPosSize.TopLeft().X() || (abs(GetPosPixel().Y() - mPosSize.TopLeft().Y()) > 5) )
    {
        SetPosPixel(mPosSize.TopLeft());

        Point aLineStart;
        Point aLineEnd ;
        if (mbMarginSide)
        {
            aLineStart = EditWin()->PixelToLogic( Point(GetPosPixel().X()+GetSizePixel().Width(),GetPosPixel().Y()-1) );
            aLineEnd = EditWin()->PixelToLogic( Point(GetPosPixel().X(),GetPosPixel().Y()-1) );
        }
        else
        {
            aLineStart = EditWin()->PixelToLogic( Point(GetPosPixel().X(),GetPosPixel().Y()-1) );
            aLineEnd = EditWin()->PixelToLogic( Point(GetPosPixel().X()+GetSizePixel().Width(),GetPosPixel().Y()-1) );
        }

        if (mpAnkor)
        {
            mpAnkor->SetAllPosition(basegfx::B2DPoint( mAnkorRect.Left() , mAnkorRect.Bottom() - 5* 15),
                                    basegfx::B2DPoint( mAnkorRect.Left()-5*15 , mAnkorRect.Bottom()+5*15),
                                    basegfx::B2DPoint( mAnkorRect.Left()+5*15 , mAnkorRect.Bottom()+5*15),
                                    basegfx::B2DPoint( mAnkorRect.Left(), mAnkorRect.Bottom()+2*15),
                                    basegfx::B2DPoint( mPageBorder ,mAnkorRect.Bottom()+2*15),
                                    basegfx::B2DPoint( aLineStart.X(),aLineStart.Y()),
                                    basegfx::B2DPoint( aLineEnd.X(),aLineEnd.Y()));
            mpAnkor->SetHeight(mAnkorRect.Height());
        }
        else
        {
            mpAnkor	= new SwPostItAnkor(basegfx::B2DPoint( mAnkorRect.Left() , mAnkorRect.Bottom()-5*15),
                                        basegfx::B2DPoint( mAnkorRect.Left()-5*15 , mAnkorRect.Bottom()+5*15),
                                        basegfx::B2DPoint( mAnkorRect.Left()+5*15 , mAnkorRect.Bottom()+5*15),
                                        basegfx::B2DPoint( mAnkorRect.Left(), mAnkorRect.Bottom()+2*15),
                                        basegfx::B2DPoint( mPageBorder ,mAnkorRect.Bottom()+2*15),
                                        basegfx::B2DPoint( aLineStart.X(),aLineStart.Y()),
                                        basegfx::B2DPoint( aLineEnd.X(),aLineEnd.Y()) , mColorAnkor,LineInfo(LINE_DASH,ANKORLINE_WIDTH*15), false);
            mpAnkor->SetHeight(mAnkorRect.Height());
            mpAnkor->setVisible(false);
            pOverlayManager->add(*mpAnkor);
            if (HasChildPathFocus())
                mpAnkor->SetLineInfo(LineInfo(LINE_SOLID,ANKORLINE_WIDTH*15));
        }
    }
    else
    {
        if ( mpAnkor->getBasePosition() != basegfx::B2DPoint( mAnkorRect.Left() , mAnkorRect.Bottom()-5*15))
            mpAnkor->SetTriPosition(basegfx::B2DPoint( mAnkorRect.Left() , mAnkorRect.Bottom() - 5* 15),
                                    basegfx::B2DPoint( mAnkorRect.Left()-5*15 , mAnkorRect.Bottom()+5*15),
                                    basegfx::B2DPoint( mAnkorRect.Left()+5*15 , mAnkorRect.Bottom()+5*15),
                                    basegfx::B2DPoint( mAnkorRect.Left(), mAnkorRect.Bottom()+2*15),
                                    basegfx::B2DPoint( mPageBorder ,mAnkorRect.Bottom()+2*15));
    }
}

void SwPostIt::DoResize()
{
    long aTextHeight		=   LogicToPixel( mpOutliner->CalcTextSize()).Height();
    unsigned long aWidth	=	GetSizePixel().Width();
    long aHeight			=	GetSizePixel().Height();

    if (mbMeta)
    {
        aHeight -= GetMetaHeight();
        mpMeta->Show();
        mpPostItTxt->SetQuickHelpText(rtl::OUString::createFromAscii(""));
    }
    else
    {
        mpMeta->Hide();
        mpPostItTxt->SetQuickHelpText(mpMeta->GetText());
    }

    if (aTextHeight > aHeight)
    {	// we need vertical scrollbars and have to reduce the width
        aWidth -= GetScrollbarWidth();
        mpVScrollbar->Show();
    }
    else
    {
        mpVScrollbar->Hide();
    }

    mpPostItTxt->SetPosSizePixel(0, 0, aWidth, aHeight);
    mpMeta->SetPosSizePixel(0,aHeight,GetSizePixel().Width()-GetMetaButtonAreaWidth(),GetMetaHeight());
    mpOutliner->SetPaperSize( PixelToLogic( Size(aWidth,aHeight) ) ) ;
    mpOutlinerView->SetOutputArea( PixelToLogic( Rectangle(0,0,aWidth,aHeight) ) );
    if (!mpVScrollbar->IsVisible())
    {	// if we do not have a scrollbar anymore, we want to see the complete text
        mpOutlinerView->SetVisArea( PixelToLogic( Rectangle(0,0,aWidth,aHeight) ) );
    }
    mpVScrollbar->SetPosSizePixel( aWidth, 0, GetScrollbarWidth(), aHeight		);
    mpVScrollbar->SetVisibleSize( PixelToLogic(Size(0,aHeight)).Height() );
    mpVScrollbar->SetPageSize( PixelToLogic(Size(0,aHeight)).Height() * 8 / 10 );
    mpVScrollbar->SetLineSize( mpOutliner->GetTextHeight() / 10 );
    mpVScrollbar->SetThumbPos( mpOutlinerView->GetVisArea().Top()+ mpOutlinerView->GetEditView().GetCursor()->GetOffsetY());
    mpVScrollbar->SetRange( Range(0, mpOutliner->GetTextHeight()));

    //calculate rects for meta- button
    const Fraction& fx( GetMapMode().GetScaleX() );
    const Fraction& fy( GetMapMode().GetScaleY() );

    Point aPos( mpMeta->GetPosPixel());
    Point aBase( aPos.X() + aPos.X() + GetSizePixel().Width(), aPos.Y() );
    Point aLeft = PixelToLogic( Point( aBase.X() - (METABUTTON_WIDTH+5)*fx.GetNumerator()/fx.GetDenominator(), aBase.Y()+17*fy.GetNumerator()/fx.GetDenominator() ) );
    Point aRight = PixelToLogic( Point( aBase.X() - (METABUTTON_WIDTH-1)*fx.GetNumerator()/fx.GetDenominator(), aBase.Y()+17*fy.GetNumerator()/fy.GetDenominator() ) );
    Point aBottom = PixelToLogic( Point( aBase.X() - (METABUTTON_WIDTH+2)*fx.GetNumerator()/fx.GetDenominator(), aBase.Y()+20*fy.GetNumerator()/fy.GetDenominator() ) );

    //Point aLeft		= PixelToLogic(Point(mpMeta->GetPosPixel().X()+mpMeta->GetPosPixel().X()+GetSizePixel().Width()-(GetMetaButtonWidth()+10)+5,mpMeta->GetPosPixel().Y()+17));
    //Point aRight	= PixelToLogic(Point(mpMeta->GetPosPixel().X()+mpMeta->GetPosPixel().X()+GetSizePixel().Width()-(GetMetaButtonWidth()+10)+11,mpMeta->GetPosPixel().Y()+17));
    //Point aBottom	= PixelToLogic(Point(mpMeta->GetPosPixel().X()+mpMeta->GetPosPixel().X()+GetSizePixel().Width()-(GetMetaButtonWidth()+10)+8,mpMeta->GetPosPixel().Y()+20));

    aPopupTriangle.clear();
    aPopupTriangle.append(basegfx::B2DPoint(aLeft.X(),aLeft.Y()));
    aPopupTriangle.append(basegfx::B2DPoint(aRight.X(),aRight.Y()));
    aPopupTriangle.append(basegfx::B2DPoint(aBottom.X(),aBottom.Y()));
    aPopupTriangle.setClosed(true);
    mRectMetaButton = PixelToLogic( Rectangle( Point(
                aPos.X()+GetSizePixel().Width()-(METABUTTON_WIDTH+10)*fx.GetNumerator()/fx.GetDenominator(),
                aPos.Y()+5*fy.GetNumerator()/fy.GetDenominator() ),
                Size( METABUTTON_WIDTH*fx.GetNumerator()/fx.GetDenominator(), METABUTTON_HEIGHT*fy.GetNumerator()/fy.GetDenominator() ) ) );
}

void SwPostIt::ResizeIfNeccessary(long aOldHeight, long aNewHeight)
{
    if (aOldHeight != aNewHeight)
    {
        if (aNewHeight > GetMinimumSizeWithoutMeta())
        {
            //check for lower border or next note and resize if space left
            long aBorder = mpMgr->GetNextBorder();
            if (aBorder == -1)
            {
                // we have notes scrollbar on this page, do not set new size
                // TODO: seperate scrollbar pos stuff from real resizing
                DoResize();
            }
            else
            {
                long aNewLowerValue = GetPosPixel().Y() + aNewHeight + GetMetaHeight();
                if (aNewLowerValue < aBorder)
                    SetSizePixel(Size(GetSizePixel().Width(),aNewHeight+GetMetaHeight()));
                else
                    SetSizePixel(Size(GetSizePixel().Width(),aBorder - GetPosPixel().Y()));
                DoResize();
                Invalidate();
            }
        }
        else
        {
            if (GetSizePixel().Height() != GetMinimumSizeWithoutMeta() + GetMetaHeight())
            {
                SetSizePixel(Size(GetSizePixel().Width(),GetMinimumSizeWithoutMeta() + GetMetaHeight()));
                DoResize();
                Invalidate();
            }
        }
    }
    else
    {
        // TODO: seperate scrollbar pos stuff from real resizing
        DoResize();
    }
}

void SwPostIt::SetColor(Color &aColorDark,Color &aColorLight, Color &aColorAnkor)
{
    mColorDark =  aColorDark;
    mColorLight = aColorLight;
    mColorAnkor = aColorAnkor;

    mpPostItTxt->SetColor(aColorDark,aColorLight);

    if ( !Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
    {
        //Wallpaper aWall(Gradient(GRADIENT_LINEAR,mColorLight,mColorDark));
        //mpPostItTxt->SetBackground(aWall);

        mpMeta->SetControlBackground(mColorDark);
        AllSettings aSettings = mpMeta->GetSettings();
        StyleSettings aStyleSettings = aSettings.GetStyleSettings();
        aStyleSettings.SetFieldTextColor(aColorAnkor);
        aSettings.SetStyleSettings(aStyleSettings);
        mpMeta->SetSettings(aSettings);

        AllSettings aSettings2 = mpVScrollbar->GetSettings();
        StyleSettings aStyleSettings2 = aSettings2.GetStyleSettings();
        aStyleSettings2.SetButtonTextColor(Color(0,0,0));
        //aStyleSettings2.SetLightColor(mColorAnkor);
        aStyleSettings2.SetCheckedColor(mColorLight); //hintergund
        //aStyleSettings2.SetLightBorderColor(mColorAnkor);
        aStyleSettings2.SetShadowColor(mColorAnkor);
        aStyleSettings2.SetFaceColor(mColorDark);
        aSettings2.SetStyleSettings(aStyleSettings2);
        mpVScrollbar->SetSettings(aSettings2);
    }
}

void SwPostIt::SetMarginSide(bool aMarginSide)
{
    mbMarginSide = aMarginSide;
}

void SwPostIt::SetReadonly(BOOL bSet)
{
    mbReadonly = bSet;
    View()->SetReadOnly(bSet);
}

void SwPostIt::DataChanged( const DataChangedEvent& aEvent)
{
    Window::DataChanged( aEvent );
}

void SwPostIt::GetFocus()
{
    if (mpPostItTxt)
        mpPostItTxt->GrabFocus();
}

void SwPostIt::LoseFocus()
{
}

void SwPostIt::ShowNote()
{
    SetPosAndSize();
    if (!IsVisible())
        Window::Show();
    if (mpAnkor)
        mpAnkor->setVisible(true);
}

void SwPostIt::HideNote()
{
    if (IsVisible())
        Window::Hide();
    if (mpAnkor)
        mpAnkor->setVisible(false);
}

void SwPostIt::ActivatePostIt()
{
    mpOutliner->ClearModifyFlag();
    mpOutliner->GetUndoManager().Clear();

    /*
    // enable visible spell checking
    ULONG nCntrl = Engine()->GetControlWord();
    const SwViewOption* pVOpt = DocView()->GetWrtShell().GetViewOptions();

    if( pVOpt->IsOnlineSpell() )
    {
        nCntrl |= EE_CNTRL_ONLINESPELLING|EE_CNTRL_ALLOWBIGOBJS;
        nCntrl &= ~EE_CNTRL_NOREDLINES;
        if( pVOpt->IsHideSpell() )
            nCntrl |= EE_CNTRL_NOREDLINES;
    }
    else
    {
        nCntrl &= ~EE_CNTRL_ONLINESPELLING;
        nCntrl |= EE_CNTRL_NOREDLINES;
    }
    Engine()->SetControlWord(nCntrl);
    */

    CheckMetaText();
    if ( Ankor() )
        Ankor()->SetLineInfo(LineInfo(LINE_SOLID,ANKORLINE_WIDTH*15));
    View()->ShowCursor();

    if ( !Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
        View()->SetBackgroundColor(mColorDark);
    //	mpPostItTxt->SetBackground(Wallpaper(mColorDark));
}

void SwPostIt::DeactivatePostIt()
{
    // remove selection, #i87073#
    if (View()->GetEditView().HasSelection())
    {
        ESelection aSelection = View()->GetEditView().GetSelection();
        aSelection.nEndPara = aSelection.nStartPara;
        aSelection.nEndPos = aSelection.nStartPos;
        View()->GetEditView().SetSelection(aSelection);
    }

    if ( Ankor() )
        Ankor()->SetLineInfo(LineInfo(LINE_DASH,ANKORLINE_WIDTH*15));
    // write the visible text back into the SwField
    UpdateData();
    mpOutliner->ClearModifyFlag();
    mpOutliner->GetUndoManager().Clear();

    if ( !Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
        View()->SetBackgroundColor(COL_TRANSPARENT);
    //mpPostItTxt->SetBackground(Gradient(GRADIENT_LINEAR,mColorLight,mColorDark));
}

void SwPostIt::UpdateData()
{
    if ( mpOutliner->IsModified() )
    {
        SwPosition * pPos = mpFmtFld->GetTxtFld()->GetPosition();
        if ( pPos )
        {
            SwField* pOldField = mpFld->Copy();
              mpFld->SetPar2(mpOutliner->GetEditEngine().GetText());
            mpFld->SetTextObject(mpOutliner->CreateParaObject());
            mpView->GetDocShell()->GetDoc()->AppendUndo(new SwUndoFieldFromDoc(*pPos, *pOldField, *mpFld, 0, true));
            delete pOldField;
            delete pPos;
            // so we get a new layout of notes (ankor position is still the same and we would otherwise not get one)
            mpMgr->SetLayout();
            mpView->GetDocShell()->SetModified();
        }
    }
}

void SwPostIt::Delete()
{
    if ( Mgr()->GetActivePostIt() == this)
        Mgr()->SetActivePostIt(0);
    // we delete the field directly, the Mgr cleans up the PostIt by listening
    mpView->GetWrtShellPtr()->GotoField(*mpFmtFld);
    mpView->GetWrtShellPtr()->DelRight();
}

Color SwPostIt::ColorDark()
{
    return mColorDark;
}

Color SwPostIt::ColorLight()
{
    return mColorLight;
}

Color SwPostIt::ColorAnkor()
{
    return mColorAnkor;
}

SwEditWin*	SwPostIt::EditWin()
{
    return &mpView->GetEditWin();
}

String SwPostIt::GetAuthor() const
{
    return mpFld->GetPar1();
}

long SwPostIt::GetPostItTextHeight()
{
    return mpOutliner ? LogicToPixel(mpOutliner->CalcTextSize()).Height() : 0;
}

void SwPostIt::SwitchToPostIt(bool aDirection)
{
    if (aDirection)
        SwitchToPostIt(KEY_PAGEDOWN);
    else
        SwitchToPostIt(KEY_PAGEUP);
}

void SwPostIt::SwitchToPostIt(USHORT aDirection)
{
    if (mpMgr)
    {
        SwPostIt* pPostIt = mpMgr->GetNextPostIt(aDirection, this);
        if (pPostIt)
        {
            // the note we switch to should receive focus and be visible
            pPostIt->GrabFocus();
            mpMgr->MakeVisible(pPostIt);
        }
    }
}

void SwPostIt::MouseButtonDown( const MouseEvent& rMEvt )
{
    if (mRectMetaButton.IsInside(PixelToLogic(rMEvt.GetPosPixel())) && rMEvt.IsLeft())
    {
        if (mbReadonly)
        {
            mpButtonPopup->EnableItem(FN_DELETE_NOTE,false);
            mpButtonPopup->EnableItem(FN_DELETE_NOTE_AUTHOR,false);
            mpButtonPopup->EnableItem(FN_DELETE_ALL_NOTES,false);
        }
        else
        {
            mpButtonPopup->EnableItem(FN_DELETE_NOTE,true);
            mpButtonPopup->EnableItem(FN_DELETE_NOTE_AUTHOR,true);
            mpButtonPopup->EnableItem(FN_DELETE_ALL_NOTES,true);
        }

        ExecuteCommand( mpButtonPopup->Execute( this,Rectangle(LogicToPixel(mRectMetaButton.BottomLeft()),LogicToPixel(mRectMetaButton.BottomLeft())),POPUPMENU_EXECUTE_DOWN | POPUPMENU_NOMOUSEUPCLOSE) );
    }
}

void SwPostIt::ExecuteCommand(USHORT nSlot)
{
    switch (nSlot)
    {
        case FN_DELETE_NOTE:
            Delete();
            break;
        case FN_HIDE_NOTE:
            if ( mpMgr->GetActivePostIt() == this )
            {
                mpMgr->SetActivePostIt(0);
                // put the cursor back into the document
                SwitchToFieldPos();
            }
            mpMgr->Hide(mpFld);
            break;
        case FN_DELETE_ALL_NOTES:
        case FN_HIDE_ALL_NOTES:
            // not possible as slot as this would require that "this" is the active postit
            mpView->GetViewFrame()->GetBindings().Execute( nSlot, 0, 0, SFX_CALLMODE_ASYNCHRON );
            break;
        case FN_DELETE_NOTE_AUTHOR:
        case FN_HIDE_NOTE_AUTHOR:
        {
            // not possible as slot as this would require that "this" is the active postit
            SfxStringItem aItem( nSlot, mpFld->GetPar1() );
            const SfxPoolItem* aItems[2];
            aItems[0] = &aItem;
            aItems[1] = 0;
            mpView->GetViewFrame()->GetBindings().Execute( nSlot, aItems, 0, SFX_CALLMODE_ASYNCHRON );
        }
    }
}

void SwPostIt::SwitchToFieldPos(bool bAfter)
{
    mpView->GetDocShell()->GetWrtShell()->GotoField(*mpMgr->GetFmtFld(this));
    if (bAfter)
        mpView->GetDocShell()->GetWrtShell()->SwCrsrShell::Right(1, 0, FALSE);
    GrabFocusToDocument();
}

IMPL_LINK(SwPostIt, ScrollHdl, ScrollBar*, pScroll)
{
    long nDiff = View()->GetEditView().GetVisArea().Top() - pScroll->GetThumbPos();
    View()->Scroll( 0, nDiff );
    return 0;
}

IMPL_LINK(SwPostIt, ModifyHdl, void*, pVoid)
{
    // no warnings, please
    pVoid=0;
    mpView->GetDocShell()->SetModified(sal_True);
    return 0;
}

void SwPostIt::ResetAttributes()
{
    mpOutlinerView->RemoveAttribs(TRUE);
    mpOutliner->RemoveFields(TRUE);
    SfxItemSet aSet( mpView->GetDocShell()->GetPool() );
    aSet.Put(SvxFontHeightItem(200,100,EE_CHAR_FONTHEIGHT));
    mpOutlinerView->SetAttribs(aSet);
}

sal_Int32 SwPostIt::GetScrollbarWidth()
{
    return mpView->GetWrtShell().GetViewOptions()->GetZoom() / 10;
}

sal_Int32 SwPostIt::GetMetaButtonAreaWidth()
{
    const Fraction& f( GetMapMode().GetScaleX() );
    return METABUTTON_AREA_WIDTH * f.GetNumerator() / f.GetDenominator();
}

sal_Int32 SwPostIt::GetMetaHeight()
{
    const Fraction& f( GetMapMode().GetScaleY() );
    return POSTIT_META_HEIGHT * f.GetNumerator() / f.GetDenominator();
}

sal_Int32 SwPostIt::GetMinimumSizeWithMeta()
{
    return mpMgr->GetMinimumSizeWithMeta();
}

sal_Int32 SwPostIt::GetMinimumSizeWithoutMeta()
{
    const Fraction& f( GetMapMode().GetScaleY() );
    return POSTIT_MINIMUMSIZE_WITHOUT_META * f.GetNumerator() / f.GetDenominator();
}

void SwPostIt::SetSpellChecking(bool bEnable)
{
    ULONG nCntrl = mpOutliner->GetControlWord();
    if (bEnable)
        nCntrl &= ~EE_CNTRL_NOREDLINES;
    else
        nCntrl |= EE_CNTRL_NOREDLINES;
    mpOutliner->SetControlWord(nCntrl);
}

/******	SwPostItAnkor	***********************************************************/

void SwPostItAnkor::implEnsureGeometry()
{
    if(!maTriangle.count())
    {
        maTriangle.append(getBasePosition());
        maTriangle.append(GetSecondPosition());
        maTriangle.append(GetThirdPosition());
        maTriangle.setClosed(true);
    }

    if(!maLine.count())
    {
        maLine.append(GetFourthPosition());
        maLine.append(GetFifthPosition());
        maLine.append(GetSixthPosition());
    }

    if(!maLineTop.count())
    {
        maLineTop.append(GetSixthPosition());
        maLineTop.append(GetSeventhPosition());
    }
}

void SwPostItAnkor::implResetGeometry()
{
    maTriangle.clear();
    maLine.clear();
    maLineTop.clear();
}

void SwPostItAnkor::implDrawGeometry(OutputDevice& rOutputDevice, Color aColor, double fOffX, double fOffY)
{
    basegfx::B2DPolygon aTri(maTriangle);
    basegfx::B2DPolygon aLin(maLine);
    const Polygon aLinTop(maLineTop);

    if(0.0 != fOffX || 0.0 != fOffY)
    {
        // transform polygons
        basegfx::B2DHomMatrix aTranslate;
        aTranslate.set(0, 2, fOffX);
        aTranslate.set(1, 2, fOffY);

        aTri.transform(aTranslate);
        aLin.transform(aTranslate);
    }

    // draw triangle
    rOutputDevice.SetLineColor();
    rOutputDevice.SetFillColor(aColor);
    rOutputDevice.DrawPolygon(Polygon(aTri));

    /*
    basegfx::B2DHomMatrix aMatrix;
    aMatrix.translate(-aTri.getB2DPoint(0).getX(),-aTri.getB2DPoint(0).getY());
    aMatrix.scale(1.0, -1.0);
    aMatrix.translate(aTri.getB2DPoint(0).getX(),aTri.getB2DPoint(0).getY());
    aMatrix.translate(0,(mHeight*-1) + 13 * 15 );
    aTri.transform(aMatrix);
    rOutputDevice.DrawPolygon(Polygon(aTri));
    */

    // draw line
    rOutputDevice.SetLineColor(aColor);
    rOutputDevice.SetFillColor();
    rOutputDevice.DrawPolyLine(Polygon(aLin), mLineInfo);
    rOutputDevice.DrawPolyLine(aLinTop,LineInfo(LINE_SOLID,ANKORLINE_WIDTH*15));
}

Color SwPostItAnkor::implBlendColor(const Color aOriginal, sal_Int16 nChange)
{
    if(0 != nChange)
    {
        sal_Int16 nR(aOriginal.GetRed() + nChange);
        sal_Int16 nG(aOriginal.GetGreen() + nChange);
        sal_Int16 nB(aOriginal.GetBlue() + nChange);

        // truncate R, G and B
        if(nR > 0xff)
            nR = 0xff;
        else if(nR < 0)
            nR = 0;

        if(nG > 0xff)
            nG = 0xff;
        else if(nG < 0)
            nG = 0;

        if(nB > 0xff)
            nB = 0xff;
        else if(nB < 0)
            nB = 0;

        return Color((sal_uInt8)nR, (sal_uInt8)nG, (sal_uInt8)nB);
    }
    else
    {
        return aOriginal;
    }
}

SwPostItAnkor::SwPostItAnkor(const basegfx::B2DPoint& rBasePos,
                        const basegfx::B2DPoint& rSecondPos,
                        const basegfx::B2DPoint& rThirdPos,
                        const basegfx::B2DPoint& rFourthPos,
                        const basegfx::B2DPoint& rFifthPos,
                        const basegfx::B2DPoint& rSixthPos,
                        const basegfx::B2DPoint& rSeventhPos,
                        Color aBaseColor,
                        const LineInfo &aLineInfo,
                        bool bShadowedEffect)
        :	OverlayObjectWithBasePosition(rBasePos, aBaseColor),
            maSecondPosition(rSecondPos),
            maThirdPosition(rThirdPos),
            maFourthPosition(rFourthPos),
            maFifthPosition(rFifthPos),
            maSixthPosition(rSixthPos),
            maSeventhPosition(rSeventhPos),
            maTriangle(),
            maLine(),
            maLineTop(),
            mLineInfo(aLineInfo),
            mHeight(0),
            mbShadowedEffect(bShadowedEffect)
{
    if (mLineInfo.GetStyle()==LINE_DASH)
    {
        mLineInfo.SetDistance( 3 * 15);
        mLineInfo.SetDashLen(  5 * 15);
        mLineInfo.SetDashCount(100);
    }
    mbAllowsAnimation = sal_False;
}

SwPostItAnkor::~SwPostItAnkor()
{
}

void SwPostItAnkor::Trigger(sal_uInt32 /*nTime*/)
{
}

void SwPostItAnkor::drawGeometry(OutputDevice& rOutputDevice)
{
    implEnsureGeometry();

    if(getShadowedEffect())
    {
        // calculate one pixel offset
        const basegfx::B2DVector aOnePixelOffset(rOutputDevice.GetInverseViewTransformation() * basegfx::B2DVector(1.0, 1.0));
        const Color aLighterColor(implBlendColor(getBaseColor(), 20));
        const Color aDarkerColor(implBlendColor(getBaseColor(), -20));

        // draw top-left
        implDrawGeometry(rOutputDevice, aLighterColor, -aOnePixelOffset.getX(), -aOnePixelOffset.getY());

        // draw bottom-right
        implDrawGeometry(rOutputDevice, aDarkerColor, aOnePixelOffset.getX(), aOnePixelOffset.getY());
    }

    // draw original
    implDrawGeometry(rOutputDevice, getBaseColor(), 0.0, 0.0);
}

void SwPostItAnkor::createBaseRange(OutputDevice& rOutputDevice)
{
   // get range from geometry
    implEnsureGeometry();
    maBaseRange = basegfx::tools::getRange(maTriangle);
    maBaseRange.expand(basegfx::tools::getRange(maLine));
    maBaseRange.expand(basegfx::tools::getRange(maLineTop));

    /*
    basegfx::B2DHomMatrix aMatrix;
    aMatrix.translate(-maTriangle.getB2DPoint(0).getX(),-maTriangle.getB2DPoint(0).getY());
    aMatrix.scale(1.0,-1.0);
    aMatrix.translate(maTriangle.getB2DPoint(0).getX(),maTriangle.getB2DPoint(0).getY());
    aMatrix.translate(0,(mHeight*-1) + 13 * 15 );
    basegfx::B2DRange MyRange(basegfx::tools::getRange(maTriangle));
    MyRange.transform(aMatrix);
    maBaseRange.expand(MyRange);
    */

    // expand range for thick lines and shadowed geometry
    double fExpand(0.0);

    // take fat line into account
    if(0 != mLineInfo.GetWidth())
    {
        // expand range for logic half line width
        fExpand += static_cast< double >(mLineInfo.GetWidth()) / 2.0;
    }

    // take shadowed into account
    if(getShadowedEffect())
    {
        const basegfx::B2DVector aOnePixelOffset(rOutputDevice.GetInverseViewTransformation() * basegfx::B2DVector(1.0, 1.0));
        fExpand += ::std::max(aOnePixelOffset.getX(), aOnePixelOffset.getY());
    }

    if(0.0 != fExpand)
    {
        maBaseRange.grow(fExpand);
    }
}


void SwPostItAnkor::SetAllPosition(const basegfx::B2DPoint& rPoint1,
            const basegfx::B2DPoint& rPoint2,
            const basegfx::B2DPoint& rPoint3,
            const basegfx::B2DPoint& rPoint4,
            const basegfx::B2DPoint& rPoint5,
            const basegfx::B2DPoint& rPoint6,
            const basegfx::B2DPoint& rPoint7)
{
    maBasePosition = rPoint1;
    maSecondPosition = rPoint2;
    maThirdPosition = rPoint3;
    maFourthPosition = rPoint4;
    maFifthPosition = rPoint5;
    maSixthPosition = rPoint6;
    maSeventhPosition = rPoint7;

    implResetGeometry();
    objectChange();
}

void SwPostItAnkor::SetSecondPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maSecondPosition)
    {
        maSecondPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetThirdPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maThirdPosition)
    {
        maThirdPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetFourthPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maFourthPosition)
    {
        maFourthPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetFifthPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maFifthPosition)
    {
        maFifthPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetSixthPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maSixthPosition)
    {
        maSixthPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetSeventhPosition(const basegfx::B2DPoint& rNew)
{
    if(rNew != maSeventhPosition)
    {
        maSeventhPosition = rNew;
        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetTriPosition(const basegfx::B2DPoint& rPoint1,const basegfx::B2DPoint& rPoint2,const basegfx::B2DPoint& rPoint3,
                                    const basegfx::B2DPoint& rPoint4,const basegfx::B2DPoint& rPoint5)
{
    maBasePosition = rPoint1;
    maSecondPosition = rPoint2;
    maThirdPosition = rPoint3;
    maFourthPosition = rPoint4;
    maFifthPosition = rPoint5;

    implResetGeometry();
    objectChange();
}

void SwPostItAnkor::transform(const basegfx::B2DHomMatrix& rMatrix)
{
    if(!rMatrix.isIdentity())
    {
        // transform base position
        OverlayObjectWithBasePosition::transform(rMatrix);

        maSecondPosition = rMatrix * GetSecondPosition();
        maThirdPosition = rMatrix * GetThirdPosition();
        maFourthPosition = rMatrix * GetFourthPosition();
        maFifthPosition = rMatrix * GetFifthPosition();
        maSixthPosition = rMatrix * GetSixthPosition();
        maSeventhPosition = rMatrix * GetSeventhPosition();

        implResetGeometry();
        objectChange();
    }
}

void SwPostItAnkor::SetLineInfo(const LineInfo &aLineInfo)
{
    if (aLineInfo != mLineInfo)
    {
        mLineInfo = aLineInfo;
        if (mLineInfo.GetStyle()==LINE_DASH)
        {
            mLineInfo.SetDistance( 3 * 15);
            mLineInfo.SetDashLen(  5 * 15);
            mLineInfo.SetDashCount(100);
        }
        //remove and add overlayobject, so it is the last one inside the manager to draw
        //therefore this line is on top
        sdr::overlay::OverlayManager* pMgr = getOverlayManager();
        if (pMgr)
        {
            pMgr->remove(*this);
            pMgr->add(*this);
            objectChange();
        }
    }
}

void SwPostItAnkor::SetColorLineInfo(Color aBaseColor,const LineInfo& aLineInfo)
{
    if ( (maBaseColor != aBaseColor) || (mLineInfo != aLineInfo) )
    {
        maBaseColor = aBaseColor;
        mLineInfo = aLineInfo;
        if (mLineInfo.GetStyle()==LINE_DASH)
        {
            mLineInfo.SetDistance( 3 *15);
            mLineInfo.SetDashLen( 5 * 15);
            mLineInfo.SetDashCount(100);
        }

        objectChange();
    }
}

void SwPostItAnkor::setShadowedEffect(bool bNew)
{
    if(bNew != getShadowedEffect())
    {
        mbShadowedEffect = bNew;
        objectChange();
    }
}
