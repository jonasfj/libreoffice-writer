/*************************************************************************
 *
 *  $RCSfile: split.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: os $ $Date: 2002-11-22 07:27:31 $
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
#ifndef _TAB_SPLIT_HXX
#define _TAB_SPLIT_HXX

#ifndef _SVX_STDDLG_HXX //autogen
#include <svx/stddlg.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

class SwWrtShell;

class SwSplitTableDlg : public SvxStandardDialog
{
    FixedText 			aCountLbl;
    NumericField 		aCountEdit;
    FixedLine            aCountFL;
    ImageRadioButton 	aHorzBox;
    ImageRadioButton    aVertBox;
    CheckBox            aPropCB;
    FixedLine            aDirFL;
    OKButton 			aOKBtn;
    CancelButton 		aCancelBtn;
    HelpButton 			aHelpBtn;
    SwWrtShell& 		rSh;

protected:
    virtual void Apply();

public:
    SwSplitTableDlg(Window *pParent, SwWrtShell& rShell );
    DECL_LINK( ClickHdl, Button * );

    BOOL                IsHorizontal() const { return aHorzBox.IsChecked(); }
    BOOL                IsProportional() const { return aPropCB.IsChecked() && aHorzBox.IsChecked(); }
    long                GetCount() const { return aCountEdit.GetValue(); }
};

#endif
