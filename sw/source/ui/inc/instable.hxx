/*************************************************************************
 *
 *  $RCSfile: instable.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-05-03 13:53:48 $
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
#ifndef _INSTABLE_HXX
#define _INSTABLE_HXX


#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _BASEDLGS_HXX //autogen
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _ACTCTRL_HXX
#include <actctrl.hxx>
#endif

#include "textcontrolcombo.hxx"

class SwWrtShell;
class SwTableAutoFmt;


class SwInsTableDlg : public SfxModalDialog
{
    TableNameEdit   aNameEdit;
    FixedText       aNameFT;
    FixedText   	aColLbl;
    NumericField 	aColEdit;
    FixedText   	aRowLbl;
    NumericField 	aRowEdit;

    CheckBox		aHeaderCB;
    CheckBox		aRepeatHeaderCB;
    FixedText		aRepeatHeaderFT;	// "dummy" to build before and after FT
    FixedText		aRepeatHeaderBeforeFT;
    NumericField	aRepeatHeaderNF;
    FixedText		aRepeatHeaderAfterFT;
    TextControlCombo	aRepeatHeaderCombo;

    CheckBox		aDontSplitCB;
    CheckBox		aBorderCB;
    FixedLine       aOptionsFL;

    FixedLine       aFL;
    OKButton     	aOkBtn;
    CancelButton 	aCancelBtn;
    HelpButton 		aHelpBtn;
    PushButton 		aAutoFmtBtn;

    SwWrtShell*		pShell;
    SwTableAutoFmt*	pTAutoFmt;
    long			nEnteredValRepeatHeaderNF;

    DECL_LINK( ModifyName, Edit * );
    DECL_LINK( ModifyRowCol, NumericField * );
    DECL_LINK( AutoFmtHdl, PushButton* );
    DECL_LINK( CheckBoxHdl, CheckBox *pCB = 0 );
    DECL_LINK( ReapeatHeaderCheckBoxHdl, void* p = 0 );
    DECL_LINK( ModifyRepeatHeaderNF_Hdl, void* p = 0 );

public:
    SwInsTableDlg( SwView& rView );
    ~SwInsTableDlg();

    void GetValues( String& rName, USHORT& rRow, USHORT& rCol,
                    SwInsertTableOptions& rInsTblOpts, String& rTableAutoFmtName,
                    SwTableAutoFmt *& prTAFmt );
};

#endif
