/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swuiccoll.hxx,v $
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
#ifndef _SWUI_CCOLL_HXX
#define _SWUI_CCOLL_HXX

#include "ccoll.hxx"

class SwWrtShell;
class SwFmt;

class SwCondCollPage : public SfxTabPage
{
    FixedLine           aConditionFL;
    CheckBox			aConditionCB;

    FixedText			aContextFT;
    FixedText			aUsedFT;
    SvTabListBox 		aTbLinks;

    FixedText			aStyleFT;
    ListBox				aStyleLB;
    ListBox				aFilterLB;

    PushButton			aRemovePB;
    PushButton			aAssignPB;

    String				sNoTmpl;
    ResStringArray 		aStrArr;

    SwWrtShell 			&rSh;
    const CommandStruct*pCmds;
    SwFmt* 				pFmt;

    CollName* 			pNms;

    BOOL 				bNewTemplate;


    SwCondCollPage(Window *pParent, const SfxItemSet &rSet);
    virtual ~SwCondCollPage();

    virtual int     DeactivatePage(SfxItemSet *pSet);

    DECL_LINK( OnOffHdl, CheckBox* );
    DECL_LINK( AssignRemoveHdl, PushButton*);
    DECL_LINK( SelectHdl, ListBox* );

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

public:

    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    static USHORT* GetRanges();

    virtual BOOL FillItemSet(      SfxItemSet &rSet);
    virtual void Reset      (const SfxItemSet &rSet);

    void SetCollection( SwFmt* pFormat, BOOL bNew );
};



#endif

