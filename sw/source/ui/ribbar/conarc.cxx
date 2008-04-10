/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: conarc.cxx,v $
 * $Revision: 1.7 $
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


#include <svx/svdobj.hxx>


#include "view.hxx"
#include "edtwin.hxx"
#include "wrtsh.hxx"
#include "drawbase.hxx"
#include "conarc.hxx"



/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/



ConstArc::ConstArc(SwWrtShell* pWrtShell, SwEditWin* pEditWin, SwView* pSwView)
    : SwDrawBase(pWrtShell, pEditWin, pSwView),	nAnzButUp(0)
{
}

/*************************************************************************
|*
|* MouseButtonDown-event
|*
\************************************************************************/



BOOL ConstArc::MouseButtonDown( const MouseEvent& rMEvt )
{
    BOOL bReturn;

    if ((bReturn = SwDrawBase::MouseButtonDown(rMEvt)) == TRUE)
    {
        if (!nAnzButUp)
            aStartPnt = m_pWin->PixelToLogic(rMEvt.GetPosPixel());
    }
    return (bReturn);
}

/*************************************************************************
|*
|* MouseButtonUp-event
|*
\************************************************************************/



BOOL ConstArc::MouseButtonUp( const MouseEvent& rMEvt )
{
    BOOL bReturn = FALSE;

    if ((m_pSh->IsDrawCreate() || m_pWin->IsDrawAction()) && rMEvt.IsLeft())
    {
        Point aPnt(m_pWin->PixelToLogic(rMEvt.GetPosPixel()));
        if (!nAnzButUp && aPnt == aStartPnt)
        {
            SwDrawBase::MouseButtonUp(rMEvt);
            bReturn = TRUE;
        }
        else
        {	nAnzButUp++;

            if (nAnzButUp == 3)		// Kreisbogenerzeugung beendet
            {
                SwDrawBase::MouseButtonUp(rMEvt);
                nAnzButUp = 0;
                bReturn = TRUE;
            }
            else
                m_pSh->EndCreate(SDRCREATE_NEXTPOINT);
        }
    }
/*	else if ( pView->IsCreateObj() && rMEvt.IsRight() )
    {
        pView->EndCreateObj( SDRCREATE_FORCEEND );
        bReturn = TRUE;
    }*/

    return (bReturn);
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/



void ConstArc::Activate(const USHORT nSlotId)
{
    switch (nSlotId)
    {
        case SID_DRAW_ARC:
            m_pWin->SetSdrDrawMode(OBJ_CARC);
            break;
        case SID_DRAW_PIE:
            m_pWin->SetSdrDrawMode(OBJ_SECT);
            break;
        case SID_DRAW_CIRCLECUT:
            m_pWin->SetSdrDrawMode(OBJ_CCUT);
            break;
        default:
            m_pWin->SetSdrDrawMode(OBJ_NONE);
            break;
    }

    SwDrawBase::Activate(nSlotId);
}

/*************************************************************************
|*
|* Funktion deaktivieren
|*
\************************************************************************/

void ConstArc::Deactivate()
{
    nAnzButUp = 0;

    SwDrawBase::Deactivate();
}



