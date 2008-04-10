/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: chrdlg.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _SWCHARDLG_HXX
#define _SWCHARDLG_HXX
#include <sfx2/tabdlg.hxx>

#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

class SwView;
class SvxMacroItem;

/*--------------------------------------------------------------------
   Beschreibung:	Der Tabdialog Traeger der TabPages
 --------------------------------------------------------------------*/

class SwCharDlg: public SfxTabDialog
{
    SwView&   rView;
    BOOL      bIsDrwTxtMode;

public:
    SwCharDlg(Window* pParent, SwView& pVw, const SfxItemSet& rCoreSet,
              const String* pFmtStr = 0, BOOL bIsDrwTxtDlg = FALSE);

    ~SwCharDlg();

    virtual void PageCreated( USHORT nId, SfxTabPage &rPage );
};

/*-----------------14.08.96 11.03-------------------
    Beschreibung: Tabpage fuer URL-Attribut
--------------------------------------------------*/

class SwCharURLPage : public SfxTabPage
{
    FixedLine           aURLFL;

    FixedText			aURLFT;
    Edit                aURLED;
    FixedText			aTextFT;
    Edit                aTextED;
    FixedText			aNameFT;
    Edit                aNameED;
    FixedText			aTargetFrmFT;
    ComboBox 			aTargetFrmLB;
    PushButton			aURLPB;
    PushButton			aEventPB;
    FixedLine           aStyleFL;
    FixedText			aVisitedFT;
    ListBox             aVisitedLB;
    FixedText           aNotVisitedFT;
    ListBox             aNotVisitedLB;

    SvxMacroItem*		pINetItem;
    BOOL 				bModified;

    DECL_LINK( InsertFileHdl, PushButton * );
    DECL_LINK( EventHdl, PushButton * );

public:
                        SwCharURLPage( Window* pParent,
                                           const SfxItemSet& rSet );

                        ~SwCharURLPage();
    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
};

#endif

