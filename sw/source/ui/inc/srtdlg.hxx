/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: srtdlg.hxx,v $
 * $Revision: 1.6 $
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
#ifndef _SRTDLG_HXX
#define _SRTDLG_HXX

#ifndef _FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _FIELD_HXX
#include <vcl/field.hxx>
#endif
#ifndef _LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#include <svx/stddlg.hxx>
#include <svx/langbox.hxx>

class SwWrtShell;
class CollatorRessource;

class SwSortDlg : public SvxStandardDialog
{
    FixedText			aColLbl;
    FixedText			aTypLbl;
    FixedText 			aDirLbl;

    FixedLine           aDirFL;

    CheckBox			aKeyCB1;
    NumericField		aColEdt1;
    ListBox				aTypDLB1;
    RadioButton			aSortUpRB;
    RadioButton			aSortDnRB;

    CheckBox			aKeyCB2;
    NumericField		aColEdt2;
    ListBox				aTypDLB2;
    RadioButton			aSortUp2RB;
    RadioButton			aSortDn2RB;

    CheckBox			aKeyCB3;
    NumericField		aColEdt3;
    ListBox				aTypDLB3;
    RadioButton			aSortUp3RB;
    RadioButton			aSortDn3RB;

    FixedLine           aSortFL;
    RadioButton			aColumnRB;
    RadioButton			aRowRB;

    FixedLine           aDelimFL;
    RadioButton			aDelimTabRB;
    RadioButton			aDelimFreeRB;
    Edit				aDelimEdt;
    PushButton          aDelimPB;

    FixedLine           aLangFL;
    SvxLanguageBox      aLangLB;

    FixedLine           aSortOptFL;
    CheckBox            aCaseCB;

    OKButton			aOkBtn;
    CancelButton		aCancelBtn;
    HelpButton			aHelpBtn;

    String aColTxt;
    String aRowTxt;
    String aNumericTxt;

    SwWrtShell			&rSh;
    CollatorRessource* pColRes;

    USHORT nX;
    USHORT nY;

    virtual void		Apply();
    sal_Unicode			GetDelimChar() const;

    DECL_LINK( CheckHdl, CheckBox * );
    DECL_LINK( DelimHdl, RadioButton* );
    DECL_LINK( LanguageHdl, ListBox* );
    DECL_LINK( DelimCharHdl, PushButton* );

public:
    SwSortDlg(Window * pParent, SwWrtShell &rSh);
    ~SwSortDlg();
};

#endif

