/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: wrap.hxx,v $
 * $Revision: 1.9 $
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
#ifndef _WRAP_HXX
#define _WRAP_HXX

#include <sfx2/tabdlg.hxx>
#include <sfx2/basedlgs.hxx>

#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

class Window;
class SfxItemSet;
class SwWrtShell;

class SwWrapDlg : public SfxSingleTabDialog
{
    SwWrtShell* 		pWrtShell;

public:
     SwWrapDlg(Window* pParent, SfxItemSet& rSet, SwWrtShell* pSh, BOOL bDrawMode);
    ~SwWrapDlg();

    inline SwWrtShell* 	GetWrtShell()	{ return pWrtShell; }
};


/*--------------------------------------------------------------------
    Beschreibung:	Umlauf-TabPage
 --------------------------------------------------------------------*/

class SwWrapTabPage: public SfxTabPage
{
    // WRAPPING
    FixedLine           aWrapFLC;
    ImageRadioButton	aNoWrapRB;
    ImageRadioButton	aWrapLeftRB;
    ImageRadioButton	aWrapRightRB;
    ImageRadioButton	aWrapParallelRB;
    ImageRadioButton	aWrapThroughRB;
    ImageRadioButton	aIdealWrapRB;

    // MARGIN
    FixedLine           aMarginFL;
    FixedText			aLeftMarginFT;
    MetricField 		aLeftMarginED;
    FixedText			aRightMarginFT;
    MetricField 		aRightMarginED;
    FixedText			aTopMarginFT;
    MetricField 		aTopMarginED;
    FixedText			aBottomMarginFT;
    MetricField 		aBottomMarginED;

    // OPTIONS
    FixedLine           aOptionsSepFL;
    FixedLine           aOptionsFL;
    CheckBox            aWrapAnchorOnlyCB;
    CheckBox			aWrapTransparentCB;
    CheckBox			aWrapOutlineCB;
    CheckBox			aWrapOutsideCB;

    ImageList			aWrapIL;
    ImageList           aWrapILH;

    USHORT				nOldLeftMargin;
    USHORT				nOldRightMargin;
    USHORT				nOldUpperMargin;
    USHORT				nOldLowerMargin;

    RndStdIds           nAnchorId;
    USHORT 				nHtmlMode;

    Size aFrmSize;
    SwWrtShell*			pWrtSh;

    BOOL bFormat;
    BOOL bNew;
    BOOL bHtmlMode;
    BOOL bDrawMode;
    BOOL bContourImage;

    SwWrapTabPage(Window *pParent, const SfxItemSet &rSet);
    ~SwWrapTabPage();

    void            ApplyImageList();
    void            EnableModes(const SfxItemSet& rSet);
    virtual void 	ActivatePage(const SfxItemSet& rSet);
    virtual int 	DeactivatePage(SfxItemSet *pSet);
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );

    DECL_LINK( RangeModifyHdl, MetricField * );
    DECL_LINK( WrapTypeHdl, ImageRadioButton * );
    DECL_LINK( ContourHdl, CheckBox * );

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

public:

    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);

    virtual BOOL 	FillItemSet(SfxItemSet &rSet);
    virtual void 	Reset(const SfxItemSet &rSet);

    static USHORT*	GetRanges();
    inline void		SetNewFrame(BOOL bNewFrame) { bNew = bNewFrame; }
    inline void		SetFormatUsed(BOOL bFmt, BOOL bDrw) { bFormat = bFmt;
                                                            bDrawMode = bDrw; }
    inline void		SetShell(SwWrtShell* pSh) { pWrtSh = pSh; }
};

#endif


