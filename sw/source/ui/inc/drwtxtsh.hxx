/*************************************************************************
 *
 *  $RCSfile: drwtxtsh.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:39 $
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
#ifndef _SWDRWTXTSH_HXX
#define _SWDRWTXTSH_HXX


#ifndef _SFX_SHELL_HXX //autogen
#include <sfx2/shell.hxx>
#endif
#include "shellid.hxx"

class OutlinerView;
class SdrOutliner;
class SdrView;
class SwView;
class SfxModule;
class SwWrtShell;

class SwDrawTextShell: public SfxShell
{
    SwView		&rView;

    SdrView		*pSdrView;
    OutlinerView*pOLV;
    SdrOutliner	*pOutliner;

    BOOL 		bRotate : 1;
    BOOL 		bSelMove: 1;

    void 		SetAttrToMarked(const SfxItemSet& rAttr);
    void		InsertSymbol();
    BOOL		IsTextEdit();
public:
    SFX_DECL_INTERFACE(SW_DRWTXTSHELL);
    TYPEINFO();

    SwView	   &GetView() { return rView; }
    SwWrtShell &GetShell();

                SwDrawTextShell(SwView &rView);
    virtual		~SwDrawTextShell();

    void		StateDisableItems(SfxItemSet &);

    void		Execute(SfxRequest &);
    void		ExecDraw(SfxRequest &);
    void		GetState(SfxItemSet &);
    void		GetDrawTxtCtrlState(SfxItemSet&);

    void		ExecFontWork(SfxRequest& rReq);
    void		StateFontWork(SfxItemSet& rSet);
    void		ExecFormText(SfxRequest& rReq);
    void		GetFormTextState(SfxItemSet& rSet);
    void		ExecDrawLingu(SfxRequest &rReq);
    void		ExecUndo(SfxRequest &rReq);
    void		StateUndo(SfxItemSet &rSet);
    void		ExecClpbrd(SfxRequest &rReq);
    void		StateClpbrd(SfxItemSet &rSet);
    void		StateInsert(SfxItemSet &rSet);

    void		Init();
    void		StateStatusline(SfxItemSet &rSet);
};

#endif
