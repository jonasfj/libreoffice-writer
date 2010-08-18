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
#ifndef _SW_CONVERT_HXX
#define _SW_CONVERT_HXX

#include <vcl/fixed.hxx>
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#include <vcl/edit.hxx>
#include <sfx2/basedlgs.hxx>
#include <textcontrolcombo.hxx>

class SwTableAutoFmt;
class SwView;
class SwWrtShell;
class NumericField;
struct SwInsertTableOptions;

class SwConvertTableDlg: public SfxModalDialog
{
    RadioButton		aTabBtn;
    RadioButton		aSemiBtn;
    RadioButton		aParaBtn;
    RadioButton		aOtherBtn;
    Edit 			aOtherEd;
    CheckBox		aKeepColumn;
    FixedLine       aDelimFL;

    CheckBox		aHeaderCB;
    CheckBox		aRepeatHeaderCB;
    
    FixedText       aRepeatHeaderFT;    // "dummy" to build before and after FT
    FixedText           aRepeatHeaderBeforeFT;
    NumericField        aRepeatHeaderNF;
    FixedText           aRepeatHeaderAfterFT;
    TextControlCombo    aRepeatHeaderCombo;
    
    FixedLine       aOptionsFL;
    CheckBox        aDontSplitCB;
    CheckBox		aBorderCB;
    PushButton      aAutoFmtBtn;

    OKButton		aOkBtn;
    CancelButton	aCancelBtn;
    HelpButton		aHelpBtn;

    String			sConvertTextTable;
    SwTableAutoFmt*	pTAutoFmt;
    SwWrtShell*		pShell;

    DECL_LINK( AutoFmtHdl, PushButton* );
    DECL_LINK( BtnHdl, Button* );
    DECL_LINK( CheckBoxHdl, CheckBox *pCB = 0 );
    DECL_LINK( ReapeatHeaderCheckBoxHdl, void* p = 0 );

public:
    SwConvertTableDlg( SwView& rView, bool bToTable );
    ~SwConvertTableDlg();

    void GetValues( sal_Unicode& rDelim,
                    SwInsertTableOptions& rInsTblOpts,
                    SwTableAutoFmt *& prTAFmt );
};


#endif
