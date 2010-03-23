/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#include <wrtsh.hxx>
#include <fldbas.hxx>
#include <fldmgr.hxx>
#ifndef _MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#include <DropDownFieldDialog.hxx>
#include <flddropdown.hxx>
#ifndef _FLDUI_HRC
#include <fldui.hrc>
#endif
#ifndef _SW_DROPDOWNFIELDDIALOG_HRC
#include <DropDownFieldDialog.hrc>
#endif

using namespace ::com::sun::star;


/*--------------------------------------------------------------------
    Beschreibung: Feldeinfuegen bearbeiten
 --------------------------------------------------------------------*/

sw::DropDownFieldDialog::DropDownFieldDialog( Window *pParent, SwWrtShell &rS,
                              SwField* pField, BOOL bNextButton ) :

    SvxStandardDialog(pParent,  SW_RES(DLG_FLD_DROPDOWN)),
    aItemsFL(       this, SW_RES( FL_ITEMS       )),
    aListItemsLB(   this, SW_RES( LB_LISTITEMS   )),

    aOKPB(          this, SW_RES( PB_OK          )),
    aCancelPB(      this, SW_RES( PB_CANCEL      )),
    aNextPB(        this, SW_RES( PB_NEXT        )),
    aHelpPB(        this, SW_RES( PB_HELP        )),

    aEditPB(        this, SW_RES( PB_EDIT        )),

    rSh( rS ),
    pDropField(0)
{
    Link aButtonLk = LINK(this, DropDownFieldDialog, ButtonHdl);
    aEditPB.SetClickHdl(aButtonLk);
    if( bNextButton )
    {
        aNextPB.Show();
        aNextPB.SetClickHdl(aButtonLk);
    }
    else
    {
        long nDiff = aCancelPB.GetPosPixel().Y() - aOKPB.GetPosPixel().Y();
        Point aPos = aHelpPB.GetPosPixel();
        aPos.Y() -= nDiff;
        aHelpPB.SetPosPixel(aPos);
    }
    if( RES_DROPDOWN == pField->GetTyp()->Which() )
    {
        //
        pDropField = (SwDropDownField*)pField;
        String sTitle = GetText();
        sTitle += pDropField->GetPar2();
        SetText(sTitle);
        uno::Sequence< rtl::OUString > aItems = pDropField->GetItemSequence();
        const rtl::OUString* pArray = aItems.getConstArray();
        for(sal_Int32 i = 0; i < aItems.getLength(); i++)
            aListItemsLB.InsertEntry(pArray[i]);
        aListItemsLB.SelectEntry(pDropField->GetSelectedItem());
    }

    BOOL bEnable = !rSh.IsCrsrReadonly();
    aOKPB.Enable( bEnable );

    aListItemsLB.GrabFocus();
    FreeResource();
}

sw::DropDownFieldDialog::~DropDownFieldDialog()
{
}

/*--------------------------------------------------------------------

 --------------------------------------------------------------------*/

void sw::DropDownFieldDialog::Apply()
{
    if(pDropField)
    {
        String sSelect = aListItemsLB.GetSelectEntry();
        if(pDropField->GetPar1() != sSelect)
        {
            rSh.StartAllAction();

            SwDropDownField * pCopy = (SwDropDownField *) pDropField->Copy();

            pCopy->SetPar1(sSelect);
            rSh.SwEditShell::UpdateFlds(*pCopy);

            delete pCopy;

            rSh.SetUndoNoResetModified();
            rSh.EndAllAction();
        }
    }
}
/* -----------------17.06.2003 10:50-----------------

 --------------------------------------------------*/
IMPL_LINK(sw::DropDownFieldDialog, ButtonHdl, PushButton*, pButton)
{
    EndDialog(&aNextPB == pButton ? RET_OK : RET_YES );
    return 0;
}

