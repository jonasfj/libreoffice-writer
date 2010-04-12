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
#ifndef _SWFLDVAR_HXX
#define _SWFLDVAR_HXX

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#include <vcl/group.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/edit.hxx>

#include "fldpage.hxx"
#include "condedit.hxx"
#include "numfmtlb.hxx"

class SwFldVarPage;

/*--------------------------------------------------------------------
   Beschreibung:
 --------------------------------------------------------------------*/

class SelectionListBox : public ListBox
{
    SwFldVarPage* 	pDlg;
    BOOL 			bCallAddSelection;

    virtual long		PreNotify( NotifyEvent& rNEvt );

public:
    SelectionListBox( SwFldVarPage* pDialog, const ResId& rResId );

    //	Selektieren per Ctrl oder Alt erkennen und mit SelectHdl auswerten
    BOOL			IsCallAddSelection() const {return bCallAddSelection;}
    void 			ResetCallAddSelection() {bCallAddSelection = FALSE;}
};

/*--------------------------------------------------------------------
   Beschreibung:
 --------------------------------------------------------------------*/

class SwFldVarPage : public SwFldPage
{
    friend class SelectionListBox;

    FixedText 			aTypeFT;
    ListBox 			aTypeLB;
    FixedText 			aSelectionFT;
    SelectionListBox	aSelectionLB;
    FixedText 			aNameFT;
    Edit				aNameED;
    FixedText 			aValueFT;
    ConditionEdit 		aValueED;
    FixedText			aFormatFT;
    NumFormatListBox	aNumFormatLB;
    ListBox				aFormatLB;
    FixedText 			aChapterHeaderFT;
    FixedText 			aChapterLevelFT;
    ListBox				aChapterLevelLB;
    CheckBox			aInvisibleCB;
    FixedText			aSeparatorFT;
    Edit				aSeparatorED;
    ToolBox				aNewDelTBX;

    String				sOldValueFT;
    String				sOldNameFT;

    ULONG				nOldFormat;
    BOOL				bInit;

    DECL_LINK( TypeHdl, ListBox* pLB = 0 );
    DECL_LINK( SubTypeHdl, ListBox* pLB = 0 );
    DECL_LINK( ModifyHdl, Edit *pED = 0 );
    DECL_LINK( TBClickHdl, ToolBox *pTB = 0);
    DECL_LINK( ChapterHdl, ListBox *pLB = 0);
    DECL_LINK( SeparatorHdl, Edit *pED = 0 );

    void				UpdateSubType();
    USHORT				FillFormatLB(USHORT nTypeId);

protected:
    virtual USHORT		GetGroup();

public:
                        SwFldVarPage(Window* pParent, const SfxItemSet& rSet);

                        ~SwFldVarPage();

    static SfxTabPage*  Create(Window* pParent, const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    virtual void		FillUserData();
};


#endif

