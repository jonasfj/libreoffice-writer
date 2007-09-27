/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textsh.hxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:12:45 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _SWTEXTSH_HXX
#define _SWTEXTSH_HXX

#ifndef _SWBASESH_HXX
#include <basesh.hxx>
#endif

class AbstractSvxPostItDialog;
class SwFldMgr;
class SvxHyperlinkItem;

class SwTextShell: public SwBaseShell
{
    SwFldMgr*	pPostItFldMgr;

    void InsertSymbol( SfxRequest& );
    void InsertHyperlink(const SvxHyperlinkItem& rHlnkItem);
    bool InsertMediaDlg( SfxRequest& );
    void ChangeHeaderOrFooter(const String& rStyleName, BOOL bHeader, BOOL bOn, BOOL bShowWarning);

public:
    SFX_DECL_INTERFACE(SW_TEXTSHELL)
    TYPEINFO();

    DECL_LINK( PostItNextHdl, AbstractSvxPostItDialog * );
    DECL_LINK( PostItPrevHdl, AbstractSvxPostItDialog * );
    DECL_LINK( RedlineNextHdl, AbstractSvxPostItDialog * );
    DECL_LINK( RedlinePrevHdl, AbstractSvxPostItDialog * );

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
