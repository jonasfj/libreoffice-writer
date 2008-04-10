/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: zoomctrl.cxx,v $
 * $Revision: 1.8 $
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

#ifndef _STATUS_HXX //autogen
#include <vcl/status.hxx>
#endif
#include <sfx2/app.hxx>
#include <svtools/stritem.hxx>
#include <svx/zoomitem.hxx>

#include "swtypes.hxx"
#include "zoomctrl.hxx"


SFX_IMPL_STATUSBAR_CONTROL( SwZoomControl, SvxZoomItem );


SwZoomControl::SwZoomControl( USHORT _nSlotId,
                              USHORT _nId,
                              StatusBar& rStb ) :
    SvxZoomStatusBarControl( _nSlotId, _nId, rStb )
{
}

// -----------------------------------------------------------------------

SwZoomControl::~SwZoomControl()
{
}

// -----------------------------------------------------------------------

void SwZoomControl::StateChanged( USHORT nSID, SfxItemState eState,
                                  const SfxPoolItem* pState )
{
    if(SFX_ITEM_AVAILABLE == eState && pState->ISA( SfxStringItem ))
    {
        sPreviewZoom = ((const SfxStringItem*)pState)->GetValue();
        GetStatusBar().SetItemText( GetId(), sPreviewZoom );
    }
    else
    {
        sPreviewZoom = aEmptyStr;
        SvxZoomStatusBarControl::StateChanged(nSID, eState, pState);
    }
}

// -----------------------------------------------------------------------

void SwZoomControl::Paint( const UserDrawEvent& rUsrEvt )
{
    if(!sPreviewZoom.Len())
        SvxZoomStatusBarControl::Paint(rUsrEvt);
    else
        GetStatusBar().SetItemText( GetId(), sPreviewZoom );
}

// -----------------------------------------------------------------------

void SwZoomControl::Command( const CommandEvent& rCEvt )
{
    if(!sPreviewZoom.Len())
        SvxZoomStatusBarControl::Command(rCEvt);
}




