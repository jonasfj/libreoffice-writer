/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: DropDownFieldDialog.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _SW_DROPDOWNFIELDDIALOG_HXX
#define _SW_DROPDOWNFIELDDIALOG_HXX

#include <svx/stddlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

class SwDropDownField;
class SwField;
class SwWrtShell;

/*--------------------------------------------------------------------
     Dialog to edit drop down field selection
 --------------------------------------------------------------------*/
namespace sw
{        
class DropDownFieldDialog : public SvxStandardDialog
{
    FixedLine           aItemsFL;
    ListBox             aListItemsLB;

    OKButton            aOKPB;
    CancelButton        aCancelPB;
    PushButton          aNextPB;
    HelpButton          aHelpPB;

    PushButton          aEditPB;

    SwWrtShell          &rSh;
    SwDropDownField*    pDropField;
    
    DECL_LINK(ButtonHdl, PushButton*);
    virtual void    Apply();
public:
    DropDownFieldDialog(   Window *pParent, SwWrtShell &rSh,
                                SwField* pField, BOOL bNextButton = FALSE );
    ~DropDownFieldDialog();
};
} //namespace sw


#endif
