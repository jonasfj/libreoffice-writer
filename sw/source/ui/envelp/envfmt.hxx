/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: envfmt.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 11:41:50 $
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
#ifndef _ENVFMT_HXX
#define _ENVFMT_HXX


#ifndef _SVSTDARR_HXX
#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
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

public:

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rSet);

    virtual void ActivatePage(const SfxItemSet& rSet);
    virtual int  DeactivatePage(SfxItemSet* pSet = 0);
            void FillItem(SwEnvItem& rItem);
    virtual BOOL FillItemSet(SfxItemSet& rSet);
    virtual void Reset(const SfxItemSet& rSet);
};

#endif


