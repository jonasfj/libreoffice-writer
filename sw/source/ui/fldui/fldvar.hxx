/*************************************************************************
 *
 *  $RCSfile: fldvar.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:37 $
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
#ifndef _SWFLDVAR_HXX
#define _SWFLDVAR_HXX

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _SV_EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

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

