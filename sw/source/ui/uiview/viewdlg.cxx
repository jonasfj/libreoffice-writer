/*************************************************************************
 *
 *  $RCSfile: viewdlg.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-17 15:50:37 $
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

#include "hintids.hxx"

#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SVX_TSPTITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif


#include "view.hxx"
#include "wrtsh.hxx"
#include "basesh.hxx"
#include "viewopt.hxx"
#include "uitool.hxx"
#include "cmdid.h"
#include "docstdlg.hxx"
#include "pagedesc.hxx"


void SwView::ExecDlg(SfxRequest &rReq)
{
    Window *pMDI = &GetViewFrame()->GetWindow();
    ModalDialog *pDialog = 0;
    //Damit aus dem Basic keine Dialoge fuer Hintergrund-Views aufgerufen werden:
    const SfxPoolItem* pItem = 0;
    const SfxItemSet* pArgs = rReq.GetArgs();

    USHORT nSlot = rReq.GetSlot();
    if(pArgs)
        pArgs->GetItemState( GetPool().GetWhich(nSlot), FALSE, &pItem );

    switch ( nSlot )
    {
        case FN_CHANGE_PAGENUM:
        {
            if ( pItem )
            {
                USHORT nValue = ((SfxUInt16Item *)pItem)->GetValue();
                USHORT nOldValue = pWrtShell->GetPageOffset();
                USHORT nPage, nLogPage;
                pWrtShell->GetPageNum( nPage, nLogPage,
                   pWrtShell->IsCrsrVisible(), FALSE);

                if(nValue != nOldValue || nValue != nLogPage)
                {
                    if(!nOldValue)
                        pWrtShell->SetNewPageOffset( nValue );
                    else
                        pWrtShell->SetPageOffset( nValue );
                }
            }
        }
        break;

        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }

    if( pDialog )
    {
        pDialog->Execute();
        delete pDialog;
    }
}



