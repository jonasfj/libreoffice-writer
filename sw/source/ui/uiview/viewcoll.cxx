/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewcoll.cxx,v $
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



#include "cmdid.h"
#include "uiitems.hxx"
#include <tools/list.hxx>
#include <vcl/window.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <svtools/stritem.hxx>
#include <rsc/rscsfx.hxx>


#include "errhdl.hxx"
#include "view.hxx"
#include "wrtsh.hxx"
#include "basesh.hxx"


void SwView::ExecColl(SfxRequest &rReq)
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    USHORT nWhich = rReq.GetSlot();
    switch( nWhich )
    {
        case FN_SET_PAGE:
        {
            DBG_ASSERT(!this, "Not implemented")
        }
        break;
        case FN_SET_PAGE_STYLE:
        {
            if( pArgs )
            {
                if (pArgs &&
                    SFX_ITEM_SET == pArgs->GetItemState( nWhich , TRUE, &pItem ))
                {
                    if( ((SfxStringItem*)pItem)->GetValue() !=
                                            GetWrtShell().GetCurPageStyle(FALSE) )
                    {
                        SfxStringItem aName(SID_STYLE_APPLY,
                                   ((SfxStringItem*)pItem)->GetValue());
                        SfxUInt16Item aFamItem( SID_STYLE_FAMILY,
                                            SFX_STYLE_FAMILY_PAGE);
                        SwPtrItem aShell(FN_PARAM_WRTSHELL, GetWrtShellPtr());
                        SfxRequest aReq(SID_STYLE_APPLY, 0, GetPool());
                        aReq.AppendItem(aName);
                        aReq.AppendItem(aFamItem);
                        aReq.AppendItem(aShell);
                        GetCurShell()->ExecuteSlot(aReq);
                    }
                }
            }
            else
            {
                SfxRequest aReq(FN_FORMAT_PAGE_DLG, 0, GetPool());
                GetCurShell()->ExecuteSlot(aReq);
            }
        }
        break;
        default:
            ASSERT(FALSE, falscher CommandProcessor fuer Dispatch);
            return;
    }
}



