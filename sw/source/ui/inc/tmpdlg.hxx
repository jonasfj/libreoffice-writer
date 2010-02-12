/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifndef _SWTMPDLG_HXX
#define _SWTMPDLG_HXX

#include <sfx2/styledlg.hxx>

class SfxItemSet;
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

