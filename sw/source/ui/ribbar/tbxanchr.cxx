/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tbxanchr.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:25:54 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include <string> // HACK: prevent conflict between STLPORT and Workshop headers

#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif

#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _SFXMNUMGR_HXX //autogen
#include <sfx2/mnumgr.hxx>
#endif


#include "cmdid.h"
#include "docsh.hxx"
#include "swtypes.hxx"
#include "swmodule.hxx"
#include "wrtsh.hxx"
#include "view.hxx"
#include "viewopt.hxx"
#include "errhdl.hxx"
#include "ribbar.hrc"
#include "tbxanchr.hxx"



SFX_IMPL_TOOLBOX_CONTROL(SwTbxAnchor, SfxUInt16Item);

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

SwTbxAnchor::SwTbxAnchor( USHORT nSlotId, USHORT nId, ToolBox& rTbx ) :
    SfxToolBoxControl( nSlotId, nId, rTbx ),
    nActAnchorId(0)
{
    rTbx.SetItemBits( nId, TIB_DROPDOWNONLY | rTbx.GetItemBits( nId ) );
}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

 SwTbxAnchor::~SwTbxAnchor()
{
}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

void  SwTbxAnchor::StateChanged( USHORT /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
    GetToolBox().EnableItem( GetId(), (GetItemState(pState) != SFX_ITEM_DISABLED) );

    if( eState == SFX_ITEM_AVAILABLE )
    {
        const SfxUInt16Item* pItem = PTR_CAST( SfxUInt16Item, pState );
        if(pItem)
            nActAnchorId = pItem->GetValue();
    }

}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

SfxPopupWindow* SwTbxAnchor::CreatePopupWindow()
{
    SwTbxAnchor::Click();
    return 0;
}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

void  SwTbxAnchor::Click()
{
    PopupMenu aPopMenu(SW_RES(MN_ANCHOR_POPUP));

    SfxViewFrame*   pViewFrame( 0 );
    SfxDispatcher*  pDispatch( 0 );
    SfxViewShell*   pCurSh( SfxViewShell::Current() );

    if ( pCurSh )
    {
        pViewFrame = pCurSh->GetViewFrame();
        if ( pViewFrame )
            pDispatch = pViewFrame->GetDispatcher();
    }

//    SfxDispatcher* pDispatch = GetBindings().GetDispatcher();
//    SfxViewFrame* pViewFrame = pDispatch ? pDispatch->GetFrame() : 0;
    SwView* pActiveView = 0;
    if(pViewFrame)
    {
        const TypeId aTypeId = TYPE(SwView);
        SwView* pView = (SwView*)SfxViewShell::GetFirst(&aTypeId);
        while( pView )
        {
            if(pView->GetViewFrame() == pViewFrame)
            {
                pActiveView = pView;
                break;
            }
            pView = (SwView*)SfxViewShell::GetNext(*pView, &aTypeId);
        }
    }
    if(!pActiveView)
    {
        DBG_ERROR("No active view could be found")
        return;
    }
    SwWrtShell* pWrtShell = pActiveView->GetWrtShellPtr();
    aPopMenu.EnableItem( FN_TOOL_ANKER_FRAME, 0 != pWrtShell->IsFlyInFly() );

    Rectangle aRect(GetToolBox().GetItemRect(GetId()));
    USHORT nHtmlMode = ::GetHtmlMode((SwDocShell*)SfxObjectShell::Current());
    BOOL bHtmlModeNoAnchor = ( nHtmlMode & HTMLMODE_ON) && 0 == (nHtmlMode & HTMLMODE_SOME_ABS_POS);

    if (bHtmlModeNoAnchor || pWrtShell->IsInHeaderFooter())
        aPopMenu.RemoveItem(aPopMenu.GetItemPos(FN_TOOL_ANKER_PAGE));

    if (nActAnchorId)
        aPopMenu.CheckItem(nActAnchorId);


    USHORT nSlotId = aPopMenu.Execute(&GetToolBox(), aRect);
    GetToolBox().EndSelection();

    if (nSlotId)
        pDispatch->Execute(nSlotId, SFX_CALLMODE_ASYNCHRON|SFX_CALLMODE_RECORD);
}
