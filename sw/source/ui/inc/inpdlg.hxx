/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: inpdlg.hxx,v $
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
#ifndef _INPDLG_HXX
#define _INPDLG_HXX

#include <svx/stddlg.hxx>

#ifndef _SV_SVMEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif
#include <vcl/fixed.hxx>
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

class SwInputField;
class SwSetExpField;
class SwUserFieldType;
class SwField;
class SwWrtShell;

/*--------------------------------------------------------------------
     Beschreibung: Einfuegen Felder
 --------------------------------------------------------------------*/

class SwFldInputDlg: public SvxStandardDialog
{
    virtual void 	Apply();
    virtual void    StateChanged( StateChangedType );

    SwWrtShell		   &rSh;
    SwInputField*		pInpFld;
    SwSetExpField*		pSetFld;
    SwUserFieldType*	pUsrType;

    Edit 				aLabelED;

    MultiLineEdit 		aEditED;
    FixedLine           aEditFL;

    OKButton  			aOKBT;
    CancelButton 		aCancelBT;
    PushButton			aNextBT;
    HelpButton			aHelpBT;

    DECL_LINK(NextHdl, PushButton*);
public:
    SwFldInputDlg( 	Window *pParent, SwWrtShell &rSh,
                    SwField* pField, BOOL bNextButton = FALSE );
    ~SwFldInputDlg();
};


#endif
