/*************************************************************************
 *
 *  $RCSfile: textsh.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: mba $ $Date: 2002-06-14 07:55:02 $
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

#ifndef _SWTEXTSH_HXX
#define _SWTEXTSH_HXX

#ifndef _SWBASESH_HXX
#include <basesh.hxx>
#endif

class Button;
class SwFldMgr;
class SvxHyperlinkItem;

class SwTextShell: public SwBaseShell
{
    SwFldMgr*	pPostItFldMgr;

    void InsertSymbol( String& rChars, String& rFont );
    void InsertHyperlink(const SvxHyperlinkItem& rHlnkItem);
    BOOL InsertGraphicDlg();

public:
    SFX_DECL_INTERFACE(SW_TEXTSHELL);
    TYPEINFO();

    DECL_LINK( PostItNextHdl, Button * );
    DECL_LINK( PostItPrevHdl, Button * );
    DECL_LINK( RedlineNextHdl, Button * );
    DECL_LINK( RedlinePrevHdl, Button * );

    void	Execute(SfxRequest &);
    void	GetState(SfxItemSet &);

    void	ExecInsert(SfxRequest &);
    void	StateInsert(SfxItemSet&);
    void	ExecDelete(SfxRequest &);
    void	ExecEnterNum(SfxRequest &);
    void	ExecBasicMove(SfxRequest &);
    void	ExecMove(SfxRequest &);
    void	ExecMovePage(SfxRequest &);
    void	ExecMoveCol(SfxRequest &);
    void	ExecMoveLingu(SfxRequest &);
    void	ExecMoveMisc(SfxRequest &);
    void	ExecField(SfxRequest &rReq);
    void	StateField(SfxItemSet &);
    void	ExecIdx(SfxRequest &);
    void	GetIdxState(SfxItemSet &);
    void	ExecGlossary(SfxRequest &);

    void	ExecCharAttr(SfxRequest &);
    void	ExecCharAttrArgs(SfxRequest &);
    void	ExecParaAttr(SfxRequest &);
    void	ExecParaAttrArgs(SfxRequest &);
    void	ExecAttr(SfxRequest &);
    void	ExecDB(SfxRequest &);
    void 	ExecTransliteration(SfxRequest &);

    void	GetAttrState(SfxItemSet &);

             SwTextShell(SwView &rView);
    virtual ~SwTextShell();
};

#endif
