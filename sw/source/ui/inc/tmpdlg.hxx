/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tmpdlg.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:10:08 $
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

#ifndef _SWTMPDLG_HXX  //CHINA001 _SWCHARDLG_HXX
#define _SWTMPDLG_HXX //CHINA001 _SWCHARDLG_HXX

#ifndef _SFX_STYLEDLG_HXX //autogen
#include <sfx2/styledlg.hxx>
#endif

class SfxItemSet;
class FontList;
class SwWrtShell;

/*--------------------------------------------------------------------
   Beschreibung:    Der Tabdialog Traeger der TabPages
 --------------------------------------------------------------------*/

class SwTemplateDlg: public SfxStyleDialog
{

    USHORT      nType;
    USHORT      nHtmlMode;
    SwWrtShell*	pWrtShell;
    BOOL		bNewStyle;

    DECL_LINK( NumOptionsHdl, PushButton* );

public:
    SwTemplateDlg(  Window*             pParent,
                    SfxStyleSheetBase&  rBase,
                    USHORT              nRegion,
                    BOOL 				bColumn = FALSE,
                    SwWrtShell* 		pActShell = 0,
                    BOOL 				bNew = FALSE );

    ~SwTemplateDlg();
    const SfxItemSet* GetRefreshedSet();

    virtual void PageCreated( USHORT nId, SfxTabPage &rPage );
    virtual short Ok();
};


#endif

