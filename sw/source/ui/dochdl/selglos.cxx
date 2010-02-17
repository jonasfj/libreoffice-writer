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



#include "swtypes.hxx"

#include "selglos.hxx"

#include "selglos.hrc"
#include "dochdl.hrc"
#include <vcl/msgbox.hxx>

// STATIC DATA -----------------------------------------------------------


// CTOR / DTOR -----------------------------------------------------------

SwSelGlossaryDlg::SwSelGlossaryDlg(Window * pParent, const String &rShortName)
    : ModalDialog(pParent, SW_RES(DLG_SEL_GLOS)),
    aGlosBox(this, SW_RES( LB_GLOS)),
    aGlosFL(this, SW_RES( FL_GLOS)),
    aOKBtn(this, SW_RES( BT_OK)),
    aCancelBtn(this, SW_RES( BT_CANCEL)),
    aHelpBtn(this, SW_RES(BT_HELP))
{
    String sText(aGlosFL.GetText());
    sText += rShortName;
    aGlosFL.SetText(sText);
    FreeResource();

    aGlosBox.SetDoubleClickHdl(LINK(this, SwSelGlossaryDlg, DoubleClickHdl));
}

/*-----------------25.02.94 20:50-------------------
 dtor ueberladen
--------------------------------------------------*/
SwSelGlossaryDlg::~SwSelGlossaryDlg() {}
/* -----------------25.10.99 08:33-------------------

 --------------------------------------------------*/
IMPL_LINK(SwSelGlossaryDlg, DoubleClickHdl, ListBox*, /*pBox*/)
{
    EndDialog(RET_OK);
    return 0;
}
