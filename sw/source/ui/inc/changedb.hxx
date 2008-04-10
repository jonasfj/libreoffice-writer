/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: changedb.hxx,v $
 * $Revision: 1.7 $
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
#ifndef _CHANGEDB_HXX
#define _CHANGEDB_HXX


#ifndef _BITMAP_HXX //autogen
#include <vcl/bitmap.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#include <svtools/stdctrl.hxx>
#include <svtools/svtreebx.hxx>
#include <svx/stddlg.hxx>
#include "dbtree.hxx"
#include <vcl/fixed.hxx>

class SwFldMgr;
class SwView;
class SwWrtShell;
struct SwDBData;

/*--------------------------------------------------------------------
     Beschreibung: Datenbank an Feldern austauschen
 --------------------------------------------------------------------*/

class SwChangeDBDlg: public SvxStandardDialog
{
    FixedLine       aDBListFL;
    FixedText		aUsedDBFT;
    FixedText		aAvailDBFT;
    SvTreeListBox	aUsedDBTLB;
    SwDBTreeList	aAvailDBTLB;
    PushButton      aAddDBPB;
    FixedInfo		aDescFT;
    FixedText		aDocDBTextFT;
    FixedText		aDocDBNameFT;
    OKButton  		aOKBT;
    CancelButton 	aCancelBT;
    HelpButton 		aHelpBT;
//	PushButton		aChangeBT;

    ImageList       aImageList;
    ImageList       aImageListHC;

    SwWrtShell      *pSh;
    SwFldMgr		*pMgr;

    DECL_LINK(TreeSelectHdl, SvTreeListBox* pBox = 0);
    DECL_LINK(ButtonHdl, Button* pBtn);
    DECL_LINK(AddDBHdl, PushButton*);

    virtual void 	Apply();
    void			UpdateFlds();
    void			FillDBPopup();
    SvLBoxEntry*	Insert(const String& rDBName);
    void			ShowDBName(const SwDBData& rDBData);

public:
    SwChangeDBDlg(SwView& rVw);
    ~SwChangeDBDlg();
};

#endif
