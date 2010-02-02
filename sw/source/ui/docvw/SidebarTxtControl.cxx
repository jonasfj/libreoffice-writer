/************************************************************************* *
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile:  $
 * $Revision:  $
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

#include <SidebarTxtControl.hxx>

#include <SidebarTxtControlAcc.hxx>

#include <SidebarWin.hxx>
#include <PostItMgr.hxx>

#include <cmdid.h>
#include <docvw.hrc>

#include <svtools/securityoptions.hxx>

#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/mnumgr.hxx>

#include <vcl/svapp.hxx>
#include <vcl/help.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/gradient.hxx>
#include <vcl/scrbar.hxx>

#include <svx/outliner.hxx>
#include <svx/editeng.hxx>
#include <svx/editview.hxx>
#include <svx/flditem.hxx>

#include <uitool.hxx>
#include <view.hxx>
#include <wrtsh.hxx>
#include <shellres.hxx>
#include <SwRewriter.hxx>
#include <undobj.hxx>

namespace css = ::com::sun::star;

namespace sw { namespace sidebarwindows {

SidebarTxtControl::SidebarTxtControl( SwSidebarWin& rSidebarWin,
                                      WinBits nBits,
                                      SwView& rDocView,
                                      SwPostItMgr& rPostItMgr )
    : Control( &rSidebarWin, nBits )
    , mrSidebarWin( rSidebarWin )
    , mrDocView( rDocView )
    , mrPostItMgr( rPostItMgr )
    , mbMouseOver( false )
{
    AddEventListener( LINK( &mrSidebarWin, SwSidebarWin, WindowEventListener ) );
}

SidebarTxtControl::~SidebarTxtControl()
{
    RemoveEventListener( LINK( &mrSidebarWin, SwSidebarWin, WindowEventListener ) );
}

OutlinerView* SidebarTxtControl::GetTextView() const
{
    return mrSidebarWin.GetOutlinerView();
}

void SidebarTxtControl::GetFocus()
{
    Window::GetFocus();
    if ( !mrSidebarWin.IsMouseOver() )
    {
        Invalidate();
    }
}

void SidebarTxtControl::LoseFocus()
{
    // write the visible text back into the SwField
    mrSidebarWin.UpdateData();

    Window::LoseFocus();
    if ( !mrSidebarWin.IsMouseOver() )
    {
        Invalidate();
    }
}

void SidebarTxtControl::RequestHelp(const HelpEvent &rEvt)
{
    USHORT nResId = 0;
    switch( mrSidebarWin.GetLayoutStatus() )
    {
        case SwPostItHelper::INSERTED:  nResId = STR_REDLINE_INSERT; break;
        case SwPostItHelper::DELETED:   nResId = STR_REDLINE_DELETE; break;
        default: nResId = 0;
    }

    SwContentAtPos aCntntAtPos( SwContentAtPos::SW_REDLINE );
    if ( nResId &&
         mrDocView.GetWrtShell().GetContentAtPos( mrSidebarWin.GetAnchorPos(), aCntntAtPos ) )
    {
        String sTxt;
        sTxt = SW_RESSTR( nResId );
        sTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM(": " ));
        sTxt += aCntntAtPos.aFnd.pRedl->GetAuthorString();
        sTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " - " ));
        sTxt += GetAppLangDateTimeString( aCntntAtPos.aFnd.pRedl->GetTimeStamp() );
        Help::ShowQuickHelp( this,PixelToLogic(Rectangle(rEvt.GetMousePosPixel(),Size(50,10))),sTxt);
    }
}

void SidebarTxtControl::Paint( const Rectangle& rRect)
{
    if ( !Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
    {
        if ( mrSidebarWin.IsMouseOverSidebarWin() ||
             HasFocus() )
        {
            DrawGradient( Rectangle( Point(0,0), PixelToLogic(GetSizePixel()) ),
                          Gradient( GRADIENT_LINEAR,
                                    mrSidebarWin.ColorDark(),
                                    mrSidebarWin.ColorDark() ) );
        }
        else
        {
            DrawGradient( Rectangle( Point(0,0), PixelToLogic(GetSizePixel()) ),
                          Gradient( GRADIENT_LINEAR,
                                    mrSidebarWin.ColorLight(),
                                    mrSidebarWin.ColorDark()));
        }
    }

    if ( GetTextView() )
    {
        GetTextView()->Paint( rRect );
    }

    if ( mrSidebarWin.GetLayoutStatus()==SwPostItHelper::DELETED )
    {
        SetLineColor(mrSidebarWin.GetChangeColor());
        DrawLine( PixelToLogic( GetPosPixel() ),
                  PixelToLogic( GetPosPixel() +
                                Point( GetSizePixel().Width(),
                                       GetSizePixel().Height() ) ) );
        DrawLine( PixelToLogic( GetPosPixel() +
                                Point( GetSizePixel().Width(),0) ),
                  PixelToLogic( GetPosPixel() +
                                Point( 0, GetSizePixel().Height() ) ) );
    }
}

void SidebarTxtControl::KeyInput( const KeyEvent& rKeyEvt )
{
    const KeyCode& rKeyCode = rKeyEvt.GetKeyCode();
    USHORT nKey = rKeyCode.GetCode();
    if ( ( rKeyCode.IsMod1() && rKeyCode.IsMod2() ) &&
         ( (nKey == KEY_PAGEUP) || (nKey == KEY_PAGEDOWN) ) )
    {
        mrSidebarWin.SwitchToPostIt(nKey);
    }
    else if ( nKey == KEY_ESCAPE ||
              ( rKeyCode.IsMod1() &&
                ( nKey == KEY_PAGEUP ||
                  nKey == KEY_PAGEDOWN ) ) )
    {
        mrSidebarWin.SwitchToFieldPos();
    }
    else if ( nKey == KEY_INSERT )
    {
        if ( !rKeyCode.IsMod1() && !rKeyCode.IsMod2() )
        {
            mrSidebarWin.ToggleInsMode();
        }
    }
    else
    {
        //let's make sure we see our note
        mrPostItMgr.MakeVisible(&mrSidebarWin);

        long aOldHeight = mrSidebarWin.GetPostItTextHeight();
        bool bDone = false;

        /// HACK: need to switch off processing of Undo/Redo in Outliner
        if ( !( (nKey == KEY_Z || nKey == KEY_Y) && rKeyCode.IsMod1()) )
        {
            bool bIsProtected = mrSidebarWin.IsProtected();
            if ( !bIsProtected ||
                 ( bIsProtected &&
                   !mrSidebarWin.GetOutlinerView()->GetOutliner()->GetEditEngine().DoesKeyChangeText(rKeyEvt)) )
            {
                bDone = GetTextView() && GetTextView()->PostKeyEvent( rKeyEvt );
            }
            else
            {
                InfoBox( this, SW_RES( MSG_READONLY_CONTENT )).Execute();
            }
        }
        if (bDone)
            mrSidebarWin.ResizeIfNeccessary( aOldHeight, mrSidebarWin.GetPostItTextHeight() );
        else
        {
            // write back data first when showing navigator
            if ( nKey==KEY_F5 )
                mrSidebarWin.UpdateData();
            if (!mrDocView.KeyInput(rKeyEvt))
                Window::KeyInput(rKeyEvt);
        }
    }

    mrDocView.GetViewFrame()->GetBindings().InvalidateAll(FALSE);
}

void SidebarTxtControl::MouseMove( const MouseEvent& rMEvt )
{
    if ( GetTextView() )
    {
        OutlinerView* pOutlinerView( GetTextView() );
        pOutlinerView->MouseMove( rMEvt );
        // mba: why does OutlinerView not handle the modifier setting?!
        // this forces the postit to handle *all* pointer types
        SetPointer( pOutlinerView->GetPointer( rMEvt.GetPosPixel() ) );

        const EditView& aEV = pOutlinerView->GetEditView();
        const SvxFieldItem* pItem = aEV.GetFieldUnderMousePointer();
        if ( pItem )
        {
            const SvxFieldData* pFld = pItem->GetField();
            const SvxURLField* pURL = PTR_CAST( SvxURLField, pFld );
            if ( pURL )
            {
                String sURL( pURL->GetURL() );
                SvtSecurityOptions aSecOpts;
                if ( aSecOpts.IsOptionSet( SvtSecurityOptions::E_CTRLCLICK_HYPERLINK) )
                {
                    sURL.InsertAscii( ": ", 0 );
                    sURL.Insert( ViewShell::GetShellRes()->aHyperlinkClick, 0 );
                }
                Help::ShowQuickHelp( this,PixelToLogic(Rectangle(GetPosPixel(),Size(50,10))),sURL);
            }
        }
    }
}

void SidebarTxtControl::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( GetTextView() )
    {
        SvtSecurityOptions aSecOpts;
        bool bExecuteMod = aSecOpts.IsOptionSet( SvtSecurityOptions::E_CTRLCLICK_HYPERLINK);

        if ( !bExecuteMod || (bExecuteMod && rMEvt.GetModifier() == KEY_MOD1))
        {
            const EditView& aEV = GetTextView()->GetEditView();
            const SvxFieldItem* pItem = aEV.GetFieldUnderMousePointer();
            if ( pItem )
            {
                const SvxFieldData* pFld = pItem->GetField();
                const SvxURLField* pURL = PTR_CAST( SvxURLField, pFld );
                if ( pURL )
                {
                    GetTextView()->MouseButtonDown( rMEvt );
                    SwWrtShell &rSh = mrDocView.GetWrtShell();
                    String sURL( pURL->GetURL() );
                    String sTarget( pURL->GetTargetFrame() );
                    ::LoadURL( sURL, &rSh, URLLOAD_NOFILTER, &sTarget);
                    return;
                }
            }
        }
    }

    GrabFocus();
    if ( GetTextView() )
    {
        GetTextView()->MouseButtonDown( rMEvt );
    }
    mrDocView.GetViewFrame()->GetBindings().InvalidateAll(FALSE);
}

void SidebarTxtControl::MouseButtonUp( const MouseEvent& rMEvt )
{
    if ( GetTextView() )
        GetTextView()->MouseButtonUp( rMEvt );
}

IMPL_LINK( SidebarTxtControl, OnlineSpellCallback, SpellCallbackInfo*, pInfo )
{
    if ( pInfo->nCommand == SPELLCMD_STARTSPELLDLG )
    {
        mrDocView.GetViewFrame()->GetDispatcher()->Execute( FN_SPELL_GRAMMAR_DIALOG, SFX_CALLMODE_ASYNCHRON);
    }
    return 0;
}

IMPL_LINK( SidebarTxtControl, Select, Menu*, pSelMenu )
{
    mrSidebarWin.ExecuteCommand( pSelMenu->GetCurItemId() );
    return 0;
}

void SidebarTxtControl::Command( const CommandEvent& rCEvt )
{
    if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        if ( !mrSidebarWin.IsProtected() &&
             GetTextView() &&
             GetTextView()->IsWrongSpelledWordAtPos( rCEvt.GetMousePosPixel(),TRUE ))
        {
            Link aLink = LINK(this, SidebarTxtControl, OnlineSpellCallback);
            GetTextView()->ExecuteSpellPopup(rCEvt.GetMousePosPixel(),&aLink);
        }
        else
        {
            SfxPopupMenuManager* aMgr = mrDocView.GetViewFrame()->GetDispatcher()->Popup(0, this,&rCEvt.GetMousePosPixel());
            ((PopupMenu*)aMgr->GetSVMenu())->SetSelectHdl( LINK(this, SidebarTxtControl, Select) );

            {
                XubString aText = ((PopupMenu*)aMgr->GetSVMenu())->GetItemText( FN_DELETE_NOTE_AUTHOR );
                SwRewriter aRewriter;
                aRewriter.AddRule(UNDO_ARG1, mrSidebarWin.GetAuthor());
                aText = aRewriter.Apply(aText);
                ((PopupMenu*)aMgr->GetSVMenu())->SetItemText(FN_DELETE_NOTE_AUTHOR,aText);
            }

            if (rCEvt.IsMouseEvent())
                ((PopupMenu*)aMgr->GetSVMenu())->Execute(this,rCEvt.GetMousePosPixel());
            else
            {
                const Size aSize = GetSizePixel();
                const Point aPos = Point( aSize.getWidth()/2, aSize.getHeight()/2 );
                ((PopupMenu*)aMgr->GetSVMenu())->Execute(this,aPos);
            }
            delete aMgr;
        }
    }
    else
    if (rCEvt.GetCommand() == COMMAND_WHEEL)
    {
        if (mrSidebarWin.IsScrollbarVisible())
        {
            const CommandWheelData* pData = rCEvt.GetWheelData();
            if (pData->IsShift() || pData->IsMod1() || pData->IsMod2())
            {
                mrDocView.HandleWheelCommands(rCEvt);
            }
            else
            {
                HandleScrollCommand( rCEvt, 0 , mrSidebarWin.Scrollbar());
            }
        }
        else
        {
            mrDocView.HandleWheelCommands(rCEvt);
        }
    }
    else
    {
        if ( GetTextView() )
            GetTextView()->Command( rCEvt );
        else
            Window::Command(rCEvt);
    }
}

XubString SidebarTxtControl::GetSurroundingText() const
{
    if( GetTextView() )
        return GetTextView()->GetSurroundingText();
    else
        return XubString::EmptyString();
}

Selection SidebarTxtControl::GetSurroundingTextSelection() const
{
    if( GetTextView() )
        return GetTextView()->GetSurroundingTextSelection();
    else
        return Selection( 0, 0 );
}

css::uno::Reference< css::accessibility::XAccessible > SidebarTxtControl::CreateAccessible()
{

    SidebarTxtControlAccessible* pAcc( new SidebarTxtControlAccessible( *this ) );
    css::uno::Reference< css::awt::XWindowPeer > xWinPeer( pAcc );
    SetWindowPeer( xWinPeer, pAcc );

    css::uno::Reference< css::accessibility::XAccessible > xAcc( xWinPeer, css::uno::UNO_QUERY );
    return xAcc;
}

} } // end of namespace sw::sidebarwindows

