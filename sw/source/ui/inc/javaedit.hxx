/*************************************************************************
 *
 *  $RCSfile: javaedit.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: mba $ $Date: 2002-07-01 09:11:23 $
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
#ifndef _SW_JAVAEDIT_HXX
#define _SW_JAVAEDIT_HXX

// include ---------------------------------------------------------------

#ifndef _SVX_STDDLG_HXX //autogen
#include <svx/stddlg.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_SVMEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif

class SwWrtShell;
class SwFldMgr;
class SwScriptField;

// class SwJavaEditDialog -------------------------------------------------

class SwJavaEditDialog : public SvxStandardDialog
{
private:
    FixedText           aTypeFT;
    Edit				aTypeED;
    RadioButton         aUrlRB;
    RadioButton         aEditRB;
    PushButton			aUrlPB;
    Edit				aUrlED;
    MultiLineEdit       aEditED;
    FixedLine           aPostItFL;

    OKButton            aOKBtn;
    CancelButton        aCancelBtn;
    ImageButton         aPrevBtn;
    ImageButton         aNextBtn;
    HelpButton          aHelpBtn;

    String              aText;
    String              aType;

    BOOL				bNew;
    BOOL                bIsUrl;

    SwScriptField*		pFld;
    SwFldMgr*			pMgr;
    SwWrtShell*			pSh;

    DECL_LINK( OKHdl, Button* );
    DECL_LINK( PrevHdl, Button* );
    DECL_LINK( NextHdl, Button* );
    DECL_LINK( RadioButtonHdl, RadioButton* pBtn );
    DECL_LINK( InsertFileHdl, PushButton * );

    virtual void 	Apply();

    void			CheckTravel();
    void			SetFld();

public:
    SwJavaEditDialog(Window* pParent, SwWrtShell* pWrtSh);
    ~SwJavaEditDialog();

    String              GetText() { return aText; }
    String              GetType() { return aType; }
    BOOL                IsUrl() { return bIsUrl; }
    BOOL                IsNew() { return bNew; }
    BOOL                IsUpdate();
};


#endif

