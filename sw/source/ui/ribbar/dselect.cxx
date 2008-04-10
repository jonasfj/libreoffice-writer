/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dselect.cxx,v $
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


#include <sfx2/bindings.hxx>
#include "view.hxx"
#include "edtwin.hxx"
#include "wrtsh.hxx"
#include "cmdid.h"
#include "drawbase.hxx"
#include "dselect.hxx"

extern BOOL bNoInterrupt;		// in mainwn.cxx

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

DrawSelection::DrawSelection(SwWrtShell* pWrtShell, SwEditWin* pEditWin, SwView* pSwView) :
                SwDrawBase(pWrtShell, pEditWin, pSwView)
{
    m_bCreateObj = FALSE;
}

/*************************************************************************
|*
|* Tastaturereignisse bearbeiten
|*
|* Wird ein KeyEvent bearbeitet, so ist der Return-Wert TRUE, andernfalls
|* FALSE.
|*
\************************************************************************/

BOOL DrawSelection::KeyInput(const KeyEvent& rKEvt)
{
    BOOL bReturn = FALSE;

    switch (rKEvt.GetKeyCode().GetCode())
    {
        case KEY_ESCAPE:
        {
            if (m_pWin->IsDrawAction())
            {
                m_pSh->BreakMark();
                m_pWin->ReleaseMouse();
            }
            bReturn = TRUE;
        }
        break;
    }

    if (!bReturn)
        bReturn = SwDrawBase::KeyInput(rKEvt);

    return (bReturn);
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/

void DrawSelection::Activate(const USHORT nSlotId)
{
    m_pWin->SetSdrDrawMode(OBJ_NONE);
    m_pWin->SetObjectSelect( TRUE );
    SwDrawBase::Activate(nSlotId);

    m_pSh->GetView().GetViewFrame()->GetBindings().Invalidate(SID_INSERT_DRAW);
}


