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
#ifndef _LABFMT_HXX
#define _LABFMT_HXX

#include "swuilabimp.hxx"
#include "labimg.hxx"
#include <vcl/msgbox.hxx>
class SwLabFmtPage;

// class SwLabPreview -------------------------------------------------------

class SwLabPreview : public Window
{
    long lOutWPix;
    long lOutHPix;
    long lOutWPix23;
    long lOutHPix23;

    Color aGrayColor;

    String aHDistStr;
    String aVDistStr;
    String aWidthStr;
    String aHeightStr;
    String aLeftStr;
    String aUpperStr;
    String aColsStr;
    String aRowsStr;

    long lHDistWidth;
    long lVDistWidth;
    long lHeightWidth;
    long lLeftWidth;
    long lUpperWidth;
    long lColsWidth;

    long lXWidth;
    long lXHeight;

    SwLabItem aItem;

    void Paint(const Rectangle&);

    void DrawArrow(const Point& rP1, const Point& rP2, BOOL bArrow);

    using Window::GetParent;
    SwLabFmtPage* GetParent() {return (SwLabFmtPage*) Window::GetParent();}

    using Window::Update;

public:

     SwLabPreview(const SwLabFmtPage* pParent, const ResId& rResID);
    ~SwLabPreview();

    void Update(const SwLabItem& rItem);
};

// class SwLabFmtPage -------------------------------------------------------

class SwLabFmtPage : public SfxTabPage
{
    FixedInfo		aMakeFI;
    FixedInfo		aTypeFI;
    SwLabPreview aPreview;
    FixedText	 aHDistText;
    MetricField	 aHDistField;
    FixedText	 aVDistText;
    MetricField	 aVDistField;
    FixedText	 aWidthText;
    MetricField	 aWidthField;
    FixedText	 aHeightText;
    MetricField	 aHeightField;
    FixedText	 aLeftText;
    MetricField	 aLeftField;
    FixedText	 aUpperText;
    MetricField	 aUpperField;
    FixedText	 aColsText;
    NumericField aColsField;
    FixedText	 aRowsText;
    NumericField aRowsField;
    PushButton 	 aSavePB;

    Timer aPreviewTimer;
    BOOL  bModified;

    SwLabItem	 aItem;

     SwLabFmtPage(Window* pParent, const SfxItemSet& rSet);
    ~SwLabFmtPage();

    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( PreviewHdl, Timer * );
    DECL_LINK( LoseFocusHdl, Control * );
    DECL_LINK( SaveHdl, PushButton* );

    void ChangeMinMax();

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;
    using Window::GetParent;

public:

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rSet);

    virtual void ActivatePage(const SfxItemSet& rSet);
    virtual int  DeactivatePage(SfxItemSet* pSet = 0);
            void FillItem(SwLabItem& rItem);
    virtual BOOL FillItemSet(SfxItemSet& rSet);
    virtual void Reset(const SfxItemSet& rSet);

    SwLabDlg* GetParent() {return (SwLabDlg*) SfxTabPage::GetParent()->GetParent();}
};
/* -----------------------------23.01.01 10:26--------------------------------

 ---------------------------------------------------------------------------*/
class SwSaveLabelDlg : public ModalDialog
{
    FixedLine       aOptionsFL;
    FixedText		aMakeFT;
    ComboBox		aMakeCB;
    FixedText		aTypeFT;
    Edit			aTypeED;

    OKButton		aOKPB;
    CancelButton	aCancelPB;
    HelpButton		aHelpPB;

    QueryBox		aQueryMB;

    sal_Bool 		bSuccess;
    SwLabFmtPage* 	pLabPage;
    SwLabRec&		rLabRec;

    DECL_LINK(OkHdl, OKButton*);
    DECL_LINK(ModifyHdl, Edit*);

public:
    SwSaveLabelDlg(SwLabFmtPage* pParent, SwLabRec& rRec);

    void	SetLabel(const rtl::OUString& rMake, const rtl::OUString& rType)
        {
            aMakeCB.SetText(String(rMake));
            aTypeED.SetText(String(rType));
        }
    sal_Bool GetLabel(SwLabItem& rItem);
};
#endif

