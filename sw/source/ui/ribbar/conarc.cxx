/*************************************************************************
 *
 *  $RCSfile: conarc.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: os $ $Date: 2002-12-05 12:47:04 $
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

#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif


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
            aStartPnt = pWin->PixelToLogic(rMEvt.GetPosPixel());
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

    if ((pSh->IsDrawCreate() || pWin->IsDrawAction()) && rMEvt.IsLeft())
    {
        Point aPnt(pWin->PixelToLogic(rMEvt.GetPosPixel()));
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
                pSh->EndCreate(SDRCREATE_NEXTPOINT);
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
            pWin->SetDrawMode(OBJ_CARC);
            break;
        case SID_DRAW_PIE:
            pWin->SetDrawMode(OBJ_SECT);
            break;
        case SID_DRAW_CIRCLECUT:
            pWin->SetDrawMode(OBJ_CCUT);
            break;
        default:
            pWin->SetDrawMode(OBJ_NONE);
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



