/*************************************************************************
 *
 *  $RCSfile: toolbox.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: tl $ $Date: 2002-10-09 09:50:52 $
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

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXIMGMGR_HXX //autogen
#include <sfx2/imgmgr.hxx>
#endif
#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif

#ifndef TOOLBOX_HXX
#include "toolbox.hxx"
#endif
#ifndef _STARMATH_HRC
#include "starmath.hrc"
#endif
#ifndef _TOOLBOX_HRC_
#include "toolbox.hrc"
#endif
#include "view.hxx"



SmToolBoxWindow::SmToolBoxWindow(SfxBindings *pBindings,
                                 SfxChildWindow *pChildWindow,
                                 Window *pParent) :
    SfxFloatingWindow(pBindings, pChildWindow, pParent, SmResId(RID_TOOLBOXWINDOW)),
    aToolBoxCat(this, ResId(NUM_TBX_CATEGORIES + 1)),
    bAdjustPosition(TRUE)
{
    // allow for cursor travelling between toolbox and sub-categories
    SetStyle( GetStyle() | WB_DIALOGCONTROL );

    nActiveCategory = -1;

    SfxImageManager *pImgMan = pBindings->GetImageManager();

    aToolBoxCat.SetClickHdl(LINK(this, SmToolBoxWindow, CategoryClickHdl));
    pImgMan->RegisterToolBox( &aToolBoxCat );

    int i;
    for (i = 0;  i < NUM_TBX_CATEGORIES;  i++)
    {
        ToolBox *pBox = new ToolBox(this, ResId (i+1));
        pImgMan->RegisterToolBox( pBox );
        vToolBoxCategories[i] = pBox;
        pBox->SetSelectHdl(LINK(this, SmToolBoxWindow, CmdSelectHdl));
    }
    pToolBoxCmd = vToolBoxCategories [0];

    // get ImageList
    for (i = 0;  i < NUM_TBX_CATEGORIES;  i++)
    {
        aImageLists [i] = new ImageList( SmResId(IL_UNOP  + 2*i) );
        aImageListsH[i] = new ImageList( SmResId(ILH_UNOP + 2*i) );
    }
    aImageLists [NUM_TBX_CATEGORIES] = new ImageList( SmResId(IL_IMG) );
    aImageListsH[NUM_TBX_CATEGORIES] = new ImageList( SmResId(ILH_IMG) );

    FreeResource();

    ApplyImageLists();
}

SmToolBoxWindow::~SmToolBoxWindow()
{
    SfxImageManager *pImgMan = GetBindings().GetImageManager();
    pImgMan->ReleaseToolBox( &aToolBoxCat );

    int i;
    for (i = 0;  i < NUM_TBX_CATEGORIES;  i++)
    {
        ToolBox *pBox = vToolBoxCategories[i];
        pImgMan->ReleaseToolBox( pBox );
        delete pBox;
    }
    for (i = 0;  i < NUM_TBX_CATEGORIES + 1;  ++i)
    {
        delete aImageLists[i];
        delete aImageListsH[i];
    }
}

void SmToolBoxWindow::ApplyImageLists()
{
    BOOL bHighContrast = GetDisplayBackground().GetColor().IsDark() != 0;
    ImageList ** const &rImgList = bHighContrast ? aImageListsH : aImageLists;
    for (int i = 0;  i < NUM_TBX_CATEGORIES;  ++i)
        vToolBoxCategories[i]->SetImageList( *rImgList[i] );
    aToolBoxCat.SetImageList( *rImgList[NUM_TBX_CATEGORIES] );
}

void SmToolBoxWindow::DataChanged( const DataChangedEvent &rEvt )
{
    if ( (rEvt.GetType() == DATACHANGED_SETTINGS) && (rEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyImageLists();

    SfxFloatingWindow::DataChanged( rEvt );
}

void SmToolBoxWindow::StateChanged( StateChangedType nStateChange )
{
    if (bAdjustPosition && STATE_CHANGE_INITSHOW == nStateChange)
    {
        // calculate initial position to be used after creation of the window...
        AdjustPosition( Point() );
        bAdjustPosition = FALSE;
    }
    //... otherwise the base class will remember the last position of the window
    SfxFloatingWindow::StateChanged( nStateChange );
}


void SmToolBoxWindow::AdjustPosition(const Point &rPoint)
{
    Size		CatSize (31 * 5, 31 * 2);
    Size		CmdSize (31 * 5, 31 * 5);
    Size		WndSize (31 * 5, CatSize.Height() + 10 + CmdSize.Height());
    Point		aPoint;

    aToolBoxCat.SetPosSizePixel(aPoint, CatSize);

    aPoint.Y() = 66;
    for (int i = 0;  i < NUM_TBX_CATEGORIES;  i++)
        vToolBoxCategories [i]->SetPosSizePixel(aPoint, CmdSize);

    SetOutputSizePixel(WndSize);
    SetPosPixel( rPoint );

    Point aPt;
    const Rectangle aRect( aPt, GetParent()->GetOutputSizePixel() );
    const Rectangle aSelf( rPoint, WndSize );
    if ( !rPoint.X() || !rPoint.Y() || !aRect.IsInside( aSelf ) )
    {
        Point aTopLeft( Point( aRect.Right() - WndSize.Width(), aRect.Top() ) );
        Point aPos( GetParent()->OutputToScreenPixel( aTopLeft ) );
        if (aPos.X() < 0)
            aPos.X() = 0;
        if (aPos.Y() < 0)
            aPos.Y() = 0;
        SetPosPixel( aPos );
    }

    SetCategory(RID_UNBINOPS_CAT);
}


BOOL SmToolBoxWindow::Close()
{
    SmViewShell *pViewSh = SmGetActiveView();
    if (pViewSh)
        pViewSh->GetViewFrame()->GetDispatcher()->Execute(
                SID_TOOLBOX, SFX_CALLMODE_STANDARD,
                new SfxBoolItem(SID_TOOLBOX, FALSE), 0L);
    return TRUE;
}

void SmToolBoxWindow::GetFocus()
{
    // give focus to category toolbox
    // (allow for cursor travelling when a category is selected with the mouse)
    aToolBoxCat.GrabFocus();
}

void SmToolBoxWindow::SetCategory(USHORT nCategory)
{
    if (nCategory != nActiveCategory)
    {
        USHORT nLines,
               nWhatBox;

        // check for valid resource id
        switch (nCategory)
        {
            case RID_UNBINOPS_CAT :		nWhatBox = 0; nLines = 4; break;
            case RID_RELATIONS_CAT:		nWhatBox = 1; nLines = 5; break;
            case RID_SETOPERATIONS_CAT: nWhatBox = 2; nLines = 5; break;
            case RID_FUNCTIONS_CAT:		nWhatBox = 3; nLines = 5; break;
            case RID_OPERATORS_CAT:		nWhatBox = 4; nLines = 3; break;
            case RID_ATTRIBUTES_CAT:	nWhatBox = 5; nLines = 5; break;
            case RID_MISC_CAT:			nWhatBox = 8; nLines = 4; break;
            case RID_BRACKETS_CAT:		nWhatBox = 6; nLines = 5; break;
            case RID_FORMAT_CAT:		nWhatBox = 7; nLines = 3; break;
            default:
                // nothing to be done
                return;
        }

        pToolBoxCmd->Hide();

        pToolBoxCmd = vToolBoxCategories [nWhatBox];

        Size  CatSize (31 * 5, 31 * 2);
        Size  CmdSize (31 * 5, 31 * nLines);
        Size  WndSize (31 * 5, CatSize.Height() + 10 + CmdSize.Height());

        aToolBoxCat.SetPosSizePixel(Point(0, 3), CatSize);
        vToolBoxCategories [nWhatBox]->SetPosSizePixel(Point(0, 70), CmdSize);
        SetOutputSizePixel(WndSize);

        if (nActiveCategory)
            aToolBoxCat.CheckItem(nActiveCategory, FALSE);
        nActiveCategory = nCategory;
        aToolBoxCat.CheckItem(nActiveCategory, TRUE);

        pToolBoxCmd->Show();
    }
}


IMPL_LINK_INLINE_START( SmToolBoxWindow, CategoryClickHdl, ToolBox*, pToolBox)
{
    SetCategory(pToolBox->GetCurItemId());
    return 0;
}
IMPL_LINK_INLINE_END( SmToolBoxWindow, CategoryClickHdl, ToolBox*, pToolBox)


IMPL_LINK_INLINE_START( SmToolBoxWindow, CmdSelectHdl, ToolBox*, pToolBox)
{
    SmViewShell *pViewSh = SmGetActiveView();
    if (pViewSh)
        pViewSh->GetViewFrame()->GetDispatcher()->Execute(
                SID_INSERTCOMMAND, SFX_CALLMODE_STANDARD,
                new SfxInt16Item(SID_INSERTCOMMAND, pToolBox->GetCurItemId()), 0L);
    return 0;
}
IMPL_LINK_INLINE_END( SmToolBoxWindow, CmdSelectHdl, ToolBox*, pToolBox)


/**************************************************************************/

SFX_IMPL_FLOATINGWINDOW(SmToolBoxWrapper, SID_TOOLBOXWINDOW);

SmToolBoxWrapper::SmToolBoxWrapper(Window *pParentWindow,
                                   USHORT nId, SfxBindings* pBindings,
                                   SfxChildWinInfo *pInfo) :
    SfxChildWindow(pParentWindow, nId)
{
    eChildAlignment = SFX_ALIGN_NOALIGNMENT;
    pWindow = new SmToolBoxWindow(pBindings, this, pParentWindow);
}


