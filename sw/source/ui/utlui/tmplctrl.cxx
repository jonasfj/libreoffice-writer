/*************************************************************************
 *
 *  $RCSfile: tmplctrl.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-17 15:57:04 $
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

// include ---------------------------------------------------------------

#pragma hdrstop

#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _STATUS_HXX //autogen
#include <vcl/status.hxx>
#endif

#include "wrtsh.hxx"
#include "view.hxx"
#include "swmodule.hxx"
#include "cmdid.h"
#include "docsh.hxx"
#include "tmplctrl.hxx"


// STATIC DATA -----------------------------------------------------------


SFX_IMPL_STATUSBAR_CONTROL( SwTemplateControl, SfxStringItem );

// class TemplatePopup_Impl --------------------------------------------------

class TemplatePopup_Impl : public PopupMenu
{
public:
    TemplatePopup_Impl();

    USHORT			GetCurId() const { return nCurId; }

private:
    USHORT			nCurId;

    virtual void    Select();
};

// -----------------------------------------------------------------------

TemplatePopup_Impl::TemplatePopup_Impl() :
    PopupMenu(),
    nCurId(USHRT_MAX)
{
}

// -----------------------------------------------------------------------

void TemplatePopup_Impl::Select()
{
    nCurId = GetCurItemId();
}

// class SvxZoomStatusBarControl ------------------------------------------

SwTemplateControl::SwTemplateControl( USHORT nId,
                                                  StatusBar& rStb,
                                                  SfxBindings& rBind ) :
    SfxStatusBarControl( nId, rStb, rBind )
{
}

// -----------------------------------------------------------------------

SwTemplateControl::~SwTemplateControl()
{
}

// -----------------------------------------------------------------------

void SwTemplateControl::StateChanged( const SfxPoolItem* pState )
{
    SfxItemState eState = GetItemState(pState);

    if( eState != SFX_ITEM_AVAILABLE || pState->ISA( SfxVoidItem ) )
        GetStatusBar().SetItemText( GetId(), String() );
    else if ( pState->ISA( SfxStringItem ) )
    {
        sTemplate = ((SfxStringItem*)pState)->GetValue();
        GetStatusBar().SetItemText( GetId(), sTemplate );
    }
}

// -----------------------------------------------------------------------

void SwTemplateControl::Paint( const UserDrawEvent&  )
{
    GetStatusBar().SetItemText( GetId(), sTemplate );
}

// -----------------------------------------------------------------------

void SwTemplateControl::Command( const CommandEvent& rCEvt )
{
    if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU &&
            GetStatusBar().GetItemText( GetId() ).Len() )
    {
        CaptureMouse();
        TemplatePopup_Impl aPop;
        {
            SwView* pView = ::GetActiveView();
            SwWrtShell* pWrtShell;
            if( pView && 0 != (pWrtShell = pView->GetWrtShellPtr()) &&
                !pWrtShell->SwCrsrShell::HasSelection()&&
                !pWrtShell->IsSelFrmMode() &&
                !pWrtShell->IsObjSelected())
            {
                SfxStyleSheetBasePool* pPool = pView->GetDocShell()->
                                                            GetStyleSheetPool();
                pPool->SetSearchMask(SFX_STYLE_FAMILY_PAGE, SFXSTYLEBIT_ALL);
                if( pPool->Count() > 1 )
                {
                    USHORT nCount = 0;
                    SfxStyleSheetBase* pStyle = pPool->First();
                    while( pStyle )
                    {
                        nCount++;
                        aPop.InsertItem( nCount, pStyle->GetName() );
                        pStyle = pPool->Next();
                    }

                    aPop.Execute( &GetStatusBar(), rCEvt.GetMousePosPixel());
                    USHORT nId = aPop.GetCurId();
                    if( nId != USHRT_MAX)
                    {
                        // sieht etwas umstaendlich aus, anders geht's aber nicht
                        pStyle = pPool->operator[]( nId - 1 );
                        SfxStringItem aStyle( FN_SET_PAGE_STYLE, pStyle->GetName() );
                        pWrtShell->GetView().GetViewFrame()->GetDispatcher()->Execute(
                                    FN_SET_PAGE_STYLE,
                                    SFX_CALLMODE_SLOT|SFX_CALLMODE_RECORD,
                                    &aStyle, 0L );
                    }
                }
            }
        }
        ReleaseMouse();
    }
}



