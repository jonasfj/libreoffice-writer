/*************************************************************************
 *
 *  $RCSfile: srtdlg.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: jp $ $Date: 2001-04-24 18:09:30 $
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
#ifndef _SVX_STDDLG_HXX
#include <svx/stddlg.hxx>
#endif
#ifndef _SVX_LANGBOX_HXX
#include <svx/langbox.hxx>
#endif

class SwWrtShell;
class CollatorRessource;

class SwSortDlg : public SvxStandardDialog
{
    FixedText			aColLbl;
    FixedText			aTypLbl;
    FixedText 			aDirLbl;

    FixedLine			aDirGrp;

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

    FixedLine			aSortGrp;
    RadioButton			aColumnRB;
    RadioButton			aRowRB;

    FixedLine			aDelimGrp;
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

