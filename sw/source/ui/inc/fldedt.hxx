/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fldedt.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _FLDEDT_HXX
#define _FLDEDT_HXX
#include <sfx2/basedlgs.hxx>
#include <vcl/imagebtn.hxx>

class SwView;
class SwWrtShell;

/*--------------------------------------------------------------------
   Beschreibung:
 --------------------------------------------------------------------*/

class SwFldEditDlg : public SfxSingleTabDialog
{
    SwWrtShell* pSh;
    ImageButton aPrevBT;
    ImageButton aNextBT;
    PushButton	aAddressBT;

    DECL_LINK( AddressHdl, PushButton *pBt = 0 );
    DECL_LINK( NextPrevHdl, Button *pBt = 0 );

    void 			Init();
    SfxTabPage*		CreatePage(USHORT nGroup);

public:

     SwFldEditDlg(SwView& rVw);
    ~SwFldEditDlg();

    DECL_LINK( OKHdl, Button * );

    virtual short	Execute();

    void			EnableInsert(BOOL bEnable);
    void			InsertHdl();
};


#endif
