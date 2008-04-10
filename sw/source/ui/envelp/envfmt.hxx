/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: envfmt.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _ENVFMT_HXX
#define _ENVFMT_HXX


#ifndef _SVSTDARR_HXX
#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>
#endif
#include <svtools/stdctrl.hxx>
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _MENUBTN_HXX //autogen
#include <vcl/menubtn.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#include "envlop.hxx"

class SwTxtFmtColl;

// class SwEnvFmtPage ---------------------------------------------------------

class SwEnvFmtPage : public SfxTabPage
{

    FixedLine    aAddrFL;
    FixedInfo	 aAddrPosInfo;
    FixedText    aAddrLeftText;
    MetricField  aAddrLeftField;
    FixedText    aAddrTopText;
    MetricField  aAddrTopField;
    FixedInfo	 aAddrFormatInfo;
    MenuButton   aAddrEditButton;
    FixedLine    aSendFL;
    FixedInfo	 aSendPosInfo;
    FixedText    aSendLeftText;
    MetricField  aSendLeftField;
    FixedText    aSendTopText;
    MetricField  aSendTopField;
    FixedInfo	 aSendFormatInfo;
    MenuButton   aSendEditButton;
    FixedLine    aSizeFL;
    FixedText    aSizeFormatText;
    ListBox      aSizeFormatBox;
    FixedText    aSizeWidthText;
    MetricField  aSizeWidthField;
    FixedText    aSizeHeightText;
    MetricField  aSizeHeightField;
    SwEnvPreview aPreview;

    SvUShorts  aIDs;

     SwEnvFmtPage(Window* pParent, const SfxItemSet& rSet);
    ~SwEnvFmtPage();

    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( EditHdl, MenuButton * );
    DECL_LINK( FormatHdl, ListBox * );

    void SetMinMax();

    SfxItemSet	*GetCollItemSet(SwTxtFmtColl* pColl, BOOL bSender);
    
    using Window::GetParent;
    SwEnvDlg	*GetParent() {return (SwEnvDlg*) SfxTabPage::GetParent()->GetParent();}

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

public:

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rSet);

    virtual void ActivatePage(const SfxItemSet& rSet);
    virtual int  DeactivatePage(SfxItemSet* pSet = 0);
            void FillItem(SwEnvItem& rItem);
    virtual BOOL FillItemSet(SfxItemSet& rSet);
    virtual void Reset(const SfxItemSet& rSet);
};

#endif


