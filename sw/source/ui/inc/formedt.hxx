/*************************************************************************
 *
 *  $RCSfile: formedt.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fme $ $Date: 2001-06-01 10:20:44 $
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
#ifndef _FORMEDT_HXX
#define _FORMEDT_HXX

#ifndef _SVX_STDDLG_HXX //autogen
#include <svx/stddlg.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

#include "toxe.hxx"

class SwWrtShell;
class SwForm;

/*--------------------------------------------------------------------
     Beschreibung:	Markierung fuer Verzeichniseintrag einfuegen
 --------------------------------------------------------------------*/

class SwIdxFormDlg : public SvxStandardDialog
{
    DECL_LINK( EntryHdl, Button * );
    DECL_LINK( PageHdl, Button * );
    DECL_LINK( TabHdl, Button * );
    DECL_LINK( JumpHdl, Button * );
    DECL_LINK( StdHdl, Button * );
    DECL_LINK( SelectHdl, ListBox * );
    DECL_LINK( EnableSelectHdl, ListBox * );
    DECL_LINK( DoubleClickHdl, Button * );
    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( AssignHdl, Button * );
    void			UpdatePattern();
    void			Apply();

    ListBox 		aEntryLB;
    OKButton		aOKBtn;
    CancelButton	aCancelBT;
    FixedText		aLevelFT;
    Edit			aEntryED;
    PushButton		aEntryBT;
    PushButton		aTabBT;
    PushButton		aPageBT;
    PushButton		aJumpBT;
    FixedLine       aEntryFL;
    FixedText		aLevelFT2;
    ListBox 		aLevelLB;
    FixedText		aTemplateFT;
    ListBox 		aParaLayLB;
    PushButton		aStdBT;
    PushButton		aAssignBT;
    FixedLine       aFormatFL;

    SwWrtShell	   &rSh;
    SwForm		   *pForm;
    USHORT			nAktLevel;
    BOOL			bLastLinkIsEnd;

public:
    SwIdxFormDlg( Window* pParent, SwWrtShell &rShell, const SwForm& rForm );
    ~SwIdxFormDlg();

    static BOOL		IsNoNum(SwWrtShell& rSh, const String& rName);
    const SwForm&	GetTOXForm();
};

inline const SwForm& SwIdxFormDlg::GetTOXForm()
{
    return *pForm;
}

#endif
