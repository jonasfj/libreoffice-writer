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
#ifndef _SWFLDDOK_HXX
#define _SWFLDDOK_HXX

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#include <vcl/group.hxx>
#include <vcl/edit.hxx>
#include <vcl/field.hxx>

#include "numfmtlb.hxx"
#include "fldpage.hxx"

/*--------------------------------------------------------------------
   Beschreibung:
 --------------------------------------------------------------------*/

class SwFldDokPage : public SwFldPage
{
    FixedText 			aTypeFT;
    ListBox				aTypeLB;
    FixedText			aSelectionFT;
    ListBox				aSelectionLB;
    FixedText			aValueFT;
    Edit				aValueED;
    NumericField		aLevelED;
    NumericField		aDateOffsetED;
    FixedText			aFormatFT;
    ListBox				aFormatLB;
    NumFormatListBox	aNumFormatLB;
    CheckBox			aFixedCB;

    String				sDateOffset;
    String				sTimeOffset;
    Bitmap				aRootOpened;
    Bitmap 				aRootClosed;

    USHORT				nOldSel;
    ULONG				nOldFormat;

    DECL_LINK( TypeHdl, ListBox* pLB = 0 );
    DECL_LINK( FormatHdl, ListBox* pLB = 0 );
    DECL_LINK( SubTypeHdl, ListBox* pLB = 0 );

    void				AddSubType(USHORT nTypeId);
    USHORT				FillFormatLB(USHORT nTypeId);

protected:
    virtual USHORT		GetGroup();

public:
                        SwFldDokPage(Window* pWindow, const SfxItemSet& rSet);

                        ~SwFldDokPage();

    static SfxTabPage*  Create(Window* pParent, const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    virtual void		FillUserData();
};


#endif

