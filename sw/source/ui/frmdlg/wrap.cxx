/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: wrap.cxx,v $
 * $Revision: 1.17 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif



#include "hintids.hxx"
#include <vcl/graph.hxx>
#include <svx/htmlmode.hxx>
#include <sfx2/objsh.hxx>
#include <svl/intitem.hxx>
#include <editeng/opaqitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/lrspitem.hxx>
// OD 18.09.2003 #i18732#
#include <fmtfollowtextflow.hxx>
#include <svx/swframevalidation.hxx>


#include "cmdid.h"
#include "uitool.hxx"
#include "wrtsh.hxx"
#include "swmodule.hxx"
#include "viewopt.hxx"
#include "frmatr.hxx"
#include "frmmgr.hxx"
#include "globals.hrc"
#include "frmui.hrc"
#include "wrap.hrc"
#include "wrap.hxx"

using namespace ::com::sun::star;


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

static USHORT __FAR_DATA aWrapPageRg[] = {
    RES_LR_SPACE, RES_UL_SPACE,
    RES_PROTECT, RES_SURROUND,
    RES_PRINT, RES_PRINT,
    0
};


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



SwWrapDlg::SwWrapDlg(Window* pParent, SfxItemSet& rSet, SwWrtShell* pSh, BOOL bDrawMode) :
    SfxSingleTabDialog(pParent, rSet, 0),
    pWrtShell(pSh)

{
    // TabPage erzeugen
    SwWrapTabPage* pNewPage = (SwWrapTabPage*) SwWrapTabPage::Create(this, rSet);
    pNewPage->SetFormatUsed(FALSE, bDrawMode);
    pNewPage->SetShell(pWrtShell);
    SetTabPage(pNewPage);

    String sTitle(SW_RES(STR_FRMUI_WRAP));
    SetText(sTitle);
}



SwWrapDlg::~SwWrapDlg()
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



SwWrapTabPage::SwWrapTabPage(Window *pParent, const SfxItemSet &rSet) :
    SfxTabPage(pParent, SW_RES(TP_FRM_WRAP), rSet),

    aWrapFLC           (this, SW_RES(C_WRAP_FL)),
    aNoWrapRB			(this, SW_RES(RB_NO_WRAP)),
    aWrapLeftRB			(this, SW_RES(RB_WRAP_LEFT)),
    aWrapRightRB		(this, SW_RES(RB_WRAP_RIGHT)),
    aWrapParallelRB		(this, SW_RES(RB_WRAP_PARALLEL)),
    aWrapThroughRB		(this, SW_RES(RB_WRAP_THROUGH)),
    aIdealWrapRB		(this, SW_RES(RB_WRAP_IDEAL)),

    aMarginFL           (this, SW_RES(FL_MARGIN)),
    aLeftMarginFT       (this, SW_RES(FT_LEFT_MARGIN)),
    aLeftMarginED       (this, SW_RES(ED_LEFT_MARGIN)),
    aRightMarginFT      (this, SW_RES(FT_RIGHT_MARGIN)),
    aRightMarginED      (this, SW_RES(ED_RIGHT_MARGIN)),
    aTopMarginFT        (this, SW_RES(FT_TOP_MARGIN)),
    aTopMarginED        (this, SW_RES(ED_TOP_MARGIN)),
    aBottomMarginFT     (this, SW_RES(FT_BOTTOM_MARGIN)),
    aBottomMarginED     (this, SW_RES(ED_BOTTOM_MARGIN)),

    aOptionsSepFL       (this, SW_RES(FL_OPTION_SEP)),
    aOptionsFL          (this, SW_RES(FL_OPTION)),
    aWrapAnchorOnlyCB   (this, SW_RES(CB_ANCHOR_ONLY)),
    aWrapTransparentCB	(this, SW_RES(CB_TRANSPARENT)),
    aWrapOutlineCB		(this, SW_RES(CB_OUTLINE)),
    aWrapOutsideCB		(this, SW_RES(CB_ONLYOUTSIDE)),

    aWrapIL             (SW_RES(IL_WRAP)),
    aWrapILH            (SW_RES(ILH_WRAP)),

    nAnchorId(FLY_AT_CNTNT),
    nHtmlMode(0),

    pWrtSh(0),

    bFormat(FALSE),
    bNew(TRUE),
    bHtmlMode(FALSE),
    bContourImage(FALSE)

{
    FreeResource();
    SetExchangeSupport();

    Link aLk = LINK(this, SwWrapTabPage, RangeModifyHdl);
    aLeftMarginED.SetUpHdl(aLk);
    aLeftMarginED.SetDownHdl(aLk);
    aLeftMarginED.SetFirstHdl(aLk);
    aLeftMarginED.SetLastHdl(aLk);
    aLeftMarginED.SetLoseFocusHdl(aLk);

    aRightMarginED.SetUpHdl(aLk);
    aRightMarginED.SetDownHdl(aLk);
    aRightMarginED.SetFirstHdl(aLk);
    aRightMarginED.SetLastHdl(aLk);
    aRightMarginED.SetLoseFocusHdl(aLk);

    aTopMarginED.SetUpHdl(aLk);
    aTopMarginED.SetDownHdl(aLk);
    aTopMarginED.SetFirstHdl(aLk);
    aTopMarginED.SetLastHdl(aLk);
    aTopMarginED.SetLoseFocusHdl(aLk);

    aBottomMarginED.SetUpHdl(aLk);
    aBottomMarginED.SetDownHdl(aLk);
    aBottomMarginED.SetFirstHdl(aLk);
    aBottomMarginED.SetLastHdl(aLk);
    aBottomMarginED.SetLoseFocusHdl(aLk);

    aLk = LINK(this, SwWrapTabPage, WrapTypeHdl);
    aNoWrapRB.SetClickHdl(aLk);
    aWrapLeftRB.SetClickHdl(aLk);
    aWrapRightRB.SetClickHdl(aLk);
    aWrapParallelRB.SetClickHdl(aLk);
    aWrapThroughRB.SetClickHdl(aLk);
    aIdealWrapRB.SetClickHdl(aLk);
    ApplyImageList();
    aWrapOutlineCB.SetClickHdl(LINK(this, SwWrapTabPage, ContourHdl));
}



SwWrapTabPage::~SwWrapTabPage()
{
}



SfxTabPage* SwWrapTabPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwWrapTabPage(pParent, rSet);
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



void SwWrapTabPage::Reset(const SfxItemSet &rSet)
{
    //Contour fuer Draw, Grafik und OLE (Einfuegen/Grafik/Eigenschaften fehlt noch!)
    if( bDrawMode )
    {
        aWrapOutlineCB.Show();
        aWrapOutsideCB.Show();

        aWrapTransparentCB.Check( 0 == ((const SfxInt16Item&)rSet.Get(
                                        FN_DRAW_WRAP_DLG)).GetValue() );
        aWrapTransparentCB.SaveValue();
    }
    else
    {
        BOOL bShowCB = bFormat;
        if( !bFormat )
        {
            int nSelType = pWrtSh->GetSelectionType();
            if( ( nSelType & nsSelectionType::SEL_GRF ) ||
                ( nSelType & nsSelectionType::SEL_OLE && GRAPHIC_NONE !=
                            pWrtSh->GetIMapGraphic().GetType() ))
                bShowCB = TRUE;
        }
        if( bShowCB )
        {
            aWrapOutlineCB.Show();
            aWrapOutsideCB.Show();
        }
    }

    nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? TRUE : FALSE;

    FieldUnit aMetric = ::GetDfltMetric(bHtmlMode);
    SetMetric( aLeftMarginED  , aMetric );
    SetMetric( aRightMarginED , aMetric );
    SetMetric( aTopMarginED   , aMetric );
    SetMetric( aBottomMarginED, aMetric );

    const SwFmtSurround& rSurround = (const SwFmtSurround&)rSet.Get(RES_SURROUND);

    SwSurround nSur = rSurround.GetSurround();
    const SwFmtAnchor &rAnch = (const SwFmtAnchor&)rSet.Get(RES_ANCHOR);
    nAnchorId = rAnch.GetAnchorId();

    if ( (nAnchorId == FLY_AT_CNTNT || nAnchorId == FLY_AUTO_CNTNT) && nSur != SURROUND_NONE )
        aWrapAnchorOnlyCB.Check( rSurround.IsAnchorOnly() );
    else
        aWrapAnchorOnlyCB.Enable( FALSE );

    BOOL bContour = rSurround.IsContour();
    aWrapOutlineCB.Check( bContour );
    aWrapOutsideCB.Check( rSurround.IsOutside() );
    aWrapThroughRB.Enable(!aWrapOutlineCB.IsChecked());
    bContourImage = !bContour;

    ImageRadioButton* pBtn = NULL;

    switch (nSur)
    {
        case SURROUND_NONE:
        {
            pBtn = &aNoWrapRB;
            break;
        }

        case SURROUND_THROUGHT:
        {
            // transparent ?
            pBtn = &aWrapThroughRB;

            if (!bDrawMode)
            {
                const SvxOpaqueItem& rOpaque = (const SvxOpaqueItem&)rSet.Get(RES_OPAQUE);
                aWrapTransparentCB.Check(!rOpaque.GetValue());
            }
            break;
        }

        case SURROUND_PARALLEL:
        {
            pBtn = &aWrapParallelRB;
            break;
        }

        case SURROUND_IDEAL:
        {
            pBtn = &aIdealWrapRB;
            break;
        }

        default:
        {
            if (nSur == SURROUND_LEFT)
                pBtn = &aWrapLeftRB;
            else if (nSur == SURROUND_RIGHT)
                pBtn = &aWrapRightRB;
        }
    }
    if (pBtn)
    {
        pBtn->Check();
        WrapTypeHdl(pBtn);
        // Hier wird fuer Zeichenobjekte, die im Moment auf Durchlauf stehen,
        // schon mal der Default "Kontur an" vorbereitet, falls man spaeter auf
        // irgendeinen Umlauf umschaltet.
        if (bDrawMode && !aWrapOutlineCB.IsEnabled())
            aWrapOutlineCB.Check();
    }
    aWrapTransparentCB.Enable( pBtn == &aWrapThroughRB && !bHtmlMode );

    const SvxULSpaceItem& rUL = (const SvxULSpaceItem&)rSet.Get(RES_UL_SPACE);
    const SvxLRSpaceItem& rLR = (const SvxLRSpaceItem&)rSet.Get(RES_LR_SPACE);

    // Abstand zum Text
    aLeftMarginED.SetValue(aLeftMarginED.Normalize(rLR.GetLeft()), FUNIT_TWIP);
    aRightMarginED.SetValue(aRightMarginED.Normalize(rLR.GetRight()), FUNIT_TWIP);
    aTopMarginED.SetValue(aTopMarginED.Normalize(rUL.GetUpper()), FUNIT_TWIP);
    aBottomMarginED.SetValue(aBottomMarginED.Normalize(rUL.GetLower()), FUNIT_TWIP);

    ContourHdl(0);
    ActivatePage( rSet );
}

/*--------------------------------------------------------------------
    Beschreibung:	Attribute in den Set stopfen bei OK
 --------------------------------------------------------------------*/



BOOL SwWrapTabPage::FillItemSet(SfxItemSet &rSet)
{
    BOOL bModified = FALSE;
    const SfxPoolItem* pOldItem;

    const SwFmtSurround& rOldSur = (const SwFmtSurround&)GetItemSet().Get(RES_SURROUND);
    SwFmtSurround aSur( rOldSur );

    SvxOpaqueItem aOp( RES_OPAQUE);

    if (!bDrawMode)
    {
        const SvxOpaqueItem& rOpaque = (const SvxOpaqueItem&)GetItemSet().Get(RES_OPAQUE);
        aOp = rOpaque;
        aOp.SetValue(TRUE);
    }

    if (aNoWrapRB.IsChecked())
        aSur.SetSurround(SURROUND_NONE);
    else if (aWrapLeftRB.IsChecked())
        aSur.SetSurround(SURROUND_LEFT);
    else if (aWrapRightRB.IsChecked())
        aSur.SetSurround(SURROUND_RIGHT);
    else if (aWrapParallelRB.IsChecked())
        aSur.SetSurround(SURROUND_PARALLEL);
    else if (aWrapThroughRB.IsChecked())
    {
        aSur.SetSurround(SURROUND_THROUGHT);
        if (aWrapTransparentCB.IsChecked() && !bDrawMode)
            aOp.SetValue(FALSE);
    }
    else if (aIdealWrapRB.IsChecked())
        aSur.SetSurround(SURROUND_IDEAL);

    aSur.SetAnchorOnly( aWrapAnchorOnlyCB.IsChecked() );
    BOOL bContour = aWrapOutlineCB.IsChecked() && aWrapOutlineCB.IsEnabled();
    aSur.SetContour( bContour );

    if ( bContour )
        aSur.SetOutside(aWrapOutsideCB.IsChecked());

    if(0 == (pOldItem = GetOldItem( rSet, RES_SURROUND )) ||
                aSur != *pOldItem )
    {
        rSet.Put(aSur);
        bModified = TRUE;
    }

    if (!bDrawMode)
    {
        if(0 == (pOldItem = GetOldItem( rSet, FN_OPAQUE )) ||
                    aOp != *pOldItem )
        {
            rSet.Put(aOp);
            bModified = TRUE;
        }
    }

    BOOL bTopMod = aTopMarginED.IsValueModified();
    BOOL bBottomMod = aBottomMarginED.IsValueModified();

    SvxULSpaceItem aUL( RES_UL_SPACE );
    aUL.SetUpper((USHORT)aTopMarginED.Denormalize(aTopMarginED.GetValue(FUNIT_TWIP)));
    aUL.SetLower((USHORT)aBottomMarginED.Denormalize(aBottomMarginED.GetValue(FUNIT_TWIP)));

    if ( bTopMod || bBottomMod )
    {
        if(0 == (pOldItem = GetOldItem(rSet, RES_UL_SPACE)) ||
                aUL != *pOldItem )
        {
            rSet.Put( aUL, RES_UL_SPACE );
            bModified = TRUE;
        }
    }

    BOOL bLeftMod = aLeftMarginED.IsValueModified();
    BOOL bRightMod = aRightMarginED.IsValueModified();

    SvxLRSpaceItem aLR( RES_LR_SPACE );
    aLR.SetLeft((USHORT)aLeftMarginED.Denormalize(aLeftMarginED.GetValue(FUNIT_TWIP)));
    aLR.SetRight((USHORT)aRightMarginED.Denormalize(aRightMarginED.GetValue(FUNIT_TWIP)));

    if ( bLeftMod || bRightMod )
    {
        if( 0 == (pOldItem = GetOldItem(rSet, RES_LR_SPACE)) ||
                aLR != *pOldItem )
        {
            rSet.Put(aLR, RES_LR_SPACE);
            bModified = TRUE;
        }
    }

    if ( bDrawMode )
    {
        BOOL bChecked = aWrapTransparentCB.IsChecked() & aWrapTransparentCB.IsEnabled();
        if (aWrapTransparentCB.GetSavedValue() != bChecked)
            bModified |= 0 != rSet.Put(SfxInt16Item(FN_DRAW_WRAP_DLG, bChecked ? 0 : 1));
    }

    return bModified;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bsp - Update
 --------------------------------------------------------------------*/



void SwWrapTabPage::ActivatePage(const SfxItemSet& rSet)
{
    // Anchor
    const SwFmtAnchor &rAnch = (const SwFmtAnchor&)rSet.Get(RES_ANCHOR);
    nAnchorId = rAnch.GetAnchorId();
    BOOL bEnable = nAnchorId != FLY_IN_CNTNT;

    if (!bDrawMode)
    {
        SwWrtShell* pSh = bFormat ? ::GetActiveWrtShell() : pWrtSh;
        SwFlyFrmAttrMgr aMgr( bNew, pSh, (const SwAttrSet&)GetItemSet() );
        SvxSwFrameValidation aVal;

        // Size
        const SwFmtFrmSize& rFrmSize = (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE);
        Size aSize = rFrmSize.GetSize();

        // Rand
        const SvxULSpaceItem& rUL = (const SvxULSpaceItem&)rSet.Get(RES_UL_SPACE);
        const SvxLRSpaceItem& rLR = (const SvxLRSpaceItem&)rSet.Get(RES_LR_SPACE);
        nOldLeftMargin  = static_cast< USHORT >(rLR.GetLeft());
        nOldRightMargin = static_cast< USHORT >(rLR.GetRight());
        nOldUpperMargin = static_cast< USHORT >(rUL.GetUpper());
        nOldLowerMargin = static_cast< USHORT >(rUL.GetLower());

        // Position
        const SwFmtHoriOrient& rHori = (const SwFmtHoriOrient&)rSet.Get(RES_HORI_ORIENT);
        const SwFmtVertOrient& rVert = (const SwFmtVertOrient&)rSet.Get(RES_VERT_ORIENT);

        aVal.nAnchorType = static_cast< sal_Int16 >(nAnchorId);
        aVal.bAutoHeight = rFrmSize.GetHeightSizeType() == ATT_MIN_SIZE;
        aVal.bAutoWidth = rFrmSize.GetWidthSizeType() == ATT_MIN_SIZE;
        aVal.bMirror = rHori.IsPosToggle();
        // OD 18.09.2003 #i18732#
        aVal.bFollowTextFlow =
            static_cast<const SwFmtFollowTextFlow&>(rSet.Get(RES_FOLLOW_TEXT_FLOW)).GetValue();

        aVal.nHoriOrient = (short)rHori.GetHoriOrient();
        aVal.nVertOrient = (short)rVert.GetVertOrient();

        aVal.nHPos = rHori.GetPos();
        aVal.nHRelOrient = rHori.GetRelationOrient();
        aVal.nVPos = rVert.GetPos();
        aVal.nVRelOrient = rVert.GetRelationOrient();

        if (rFrmSize.GetWidthPercent() && rFrmSize.GetWidthPercent() != 0xff)
            aSize.Width() = aSize.Width() * rFrmSize.GetWidthPercent() / 100;

        if (rFrmSize.GetHeightPercent() && rFrmSize.GetHeightPercent() != 0xff)
            aSize.Height() = aSize.Height() * rFrmSize.GetHeightPercent() / 100;

        aVal.nWidth  = aSize.Width();
        aVal.nHeight = aSize.Height();
        aFrmSize = aSize;

        aMgr.ValidateMetrics(aVal, 0);

        SwTwips nLeft;
        SwTwips nRight;
        SwTwips nTop;
        SwTwips nBottom;

        nLeft	= aVal.nHPos - aVal.nMinHPos;
        nRight	= aVal.nMaxWidth - aVal.nWidth;
        nTop	= aVal.nVPos - aVal.nMinVPos;
        nBottom	= aVal.nMaxHeight - aVal.nHeight;

        if (bHtmlMode && 0 == (nHtmlMode & HTMLMODE_FULL_ABS_POS))
        {
            nLeft = nRight = ( nLeft + nRight ) / 2;
            nTop = nBottom = ( nTop + nBottom ) / 2;
        }
        else
        {
            if (aVal.nAnchorType == FLY_IN_CNTNT)
            {
                nLeft = nRight;

                if (aVal.nVPos < 0)
                {
                    if (aVal.nVPos <= aVal.nMaxHeight)
                        nTop = aVal.nMaxVPos - aVal.nHeight;
                    else
                        nTop = nBottom = 0;	// Kein Umlauf
                }
                else
                    nTop = aVal.nMaxVPos - aVal.nHeight - aVal.nVPos;
            }
            else
            {
                nLeft += nRight;
                nTop += nBottom;
            }

            nBottom	= nTop;
            nRight = nLeft;
        }

        aLeftMarginED.SetMax(aLeftMarginED.Normalize(nLeft), FUNIT_TWIP);
        aRightMarginED.SetMax(aRightMarginED.Normalize(nRight), FUNIT_TWIP);

        aTopMarginED.SetMax(aTopMarginED.Normalize(nTop), FUNIT_TWIP);
        aBottomMarginED.SetMax(aBottomMarginED.Normalize(nBottom), FUNIT_TWIP);

        RangeModifyHdl(&aLeftMarginED);
        RangeModifyHdl(&aTopMarginED);
    }

    const SwFmtSurround& rSurround = (const SwFmtSurround&)rSet.Get(RES_SURROUND);
    SwSurround nSur = rSurround.GetSurround();

    aWrapTransparentCB.Enable( bEnable && !bHtmlMode && nSur == SURROUND_THROUGHT );
    if(bHtmlMode)
    {
        BOOL bSomeAbsPos = 0 != (nHtmlMode & HTMLMODE_SOME_ABS_POS);
        const SwFmtHoriOrient& rHori = (const SwFmtHoriOrient&)rSet.Get(RES_HORI_ORIENT);
        sal_Int16 eHOrient = rHori.GetHoriOrient();
        sal_Int16 eHRelOrient = rHori.GetRelationOrient();
        aWrapOutlineCB.Hide();
        BOOL bAllHtmlModes = (nAnchorId == FLY_AT_CNTNT || nAnchorId == FLY_AUTO_CNTNT) &&
                            (eHOrient == text::HoriOrientation::RIGHT || eHOrient == text::HoriOrientation::LEFT);
        aWrapAnchorOnlyCB.Enable( bAllHtmlModes && nSur != SURROUND_NONE );
        aWrapOutsideCB.Hide();
        aIdealWrapRB.Enable( FALSE );


        aWrapTransparentCB.Enable( FALSE );
        aNoWrapRB.Enable( FLY_AT_CNTNT == nAnchorId );
        aWrapParallelRB.Enable( FALSE  );
        aWrapLeftRB		  .Enable( FLY_AT_CNTNT == nAnchorId ||
                            (FLY_AUTO_CNTNT == nAnchorId && eHOrient == text::HoriOrientation::RIGHT && eHRelOrient == text::RelOrientation::PRINT_AREA));
        aWrapRightRB	  .Enable( FLY_AT_CNTNT == nAnchorId ||
                            ( FLY_AUTO_CNTNT == nAnchorId && eHOrient == text::HoriOrientation::LEFT && eHRelOrient == text::RelOrientation::PRINT_AREA));

        aWrapThroughRB.Enable( (FLY_PAGE == nAnchorId ||
                                (FLY_AUTO_CNTNT == nAnchorId && eHRelOrient != text::RelOrientation::PRINT_AREA) || FLY_AT_CNTNT == nAnchorId )
                                    && bSomeAbsPos &&
                                        eHOrient != text::HoriOrientation::RIGHT);
        if(aNoWrapRB.IsChecked() && !aNoWrapRB.IsEnabled())
        {
            if(aWrapThroughRB.IsEnabled())
                aWrapThroughRB.Check(TRUE);
            else if(aWrapLeftRB.IsEnabled())
                aWrapLeftRB.Check();
            else if(aWrapRightRB.IsEnabled())
                aWrapRightRB.Check();

        }
        if(aWrapLeftRB.IsChecked() && !aWrapLeftRB.IsEnabled())
        {
            if(aWrapRightRB.IsEnabled())
                aWrapRightRB.Check();
            else if(aWrapThroughRB.IsEnabled())
                aWrapThroughRB.Check();
        }
        if(aWrapRightRB.IsChecked() && !aWrapRightRB.IsEnabled())
        {
            if(aWrapLeftRB.IsEnabled())
                aWrapLeftRB.Check();
            else if(aWrapThroughRB.IsEnabled())
                aWrapThroughRB.Check();
        }
        if(aWrapThroughRB.IsChecked() && !aWrapThroughRB.IsEnabled())
            if(aNoWrapRB.IsEnabled())
                aNoWrapRB.Check();

        if(aWrapParallelRB.IsChecked() && !aWrapParallelRB.IsEnabled())
            aWrapThroughRB.Check();
    }
    else
    {
        aNoWrapRB.Enable( bEnable );
        aWrapLeftRB.Enable( bEnable );
        aWrapRightRB.Enable( bEnable );
        aIdealWrapRB.Enable( bEnable );
        aWrapThroughRB.Enable( bEnable );
        aWrapParallelRB.Enable( bEnable );
        aWrapAnchorOnlyCB.Enable( (nAnchorId == FLY_AT_CNTNT || nAnchorId == FLY_AUTO_CNTNT)
                && nSur != SURROUND_NONE );
    }
    ContourHdl(0);
}



int SwWrapTabPage::DeactivatePage(SfxItemSet* _pSet)
{
    if(_pSet)
        FillItemSet(*_pSet);

    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bereichspruefung
 --------------------------------------------------------------------*/



IMPL_LINK( SwWrapTabPage, RangeModifyHdl, MetricField *, pEdit )
{
    if(bHtmlMode && 0 == (nHtmlMode & HTMLMODE_FULL_ABS_POS))
    {
        sal_Int64 nValue = pEdit->GetValue();
        if (pEdit == &aLeftMarginED)
            aRightMarginED.SetUserValue(nValue, FUNIT_NONE);
        else if (pEdit == &aRightMarginED)
            aLeftMarginED.SetUserValue(nValue, FUNIT_NONE);
        else if (pEdit == &aTopMarginED)
            aBottomMarginED.SetUserValue(nValue, FUNIT_NONE);
        else if (pEdit == &aBottomMarginED)
            aTopMarginED.SetUserValue(nValue, FUNIT_NONE);
    }
    else
    {
        sal_Int64 nValue = pEdit->GetValue();
        MetricField *pOpposite = 0;

        if (pEdit == &aLeftMarginED)
            pOpposite = &aRightMarginED;
        else if (pEdit == &aRightMarginED)
            pOpposite = &aLeftMarginED;
        else if (pEdit == &aTopMarginED)
            pOpposite = &aBottomMarginED;
        else if (pEdit == &aBottomMarginED)
            pOpposite = &aTopMarginED;

        sal_Int64 nOpposite = pOpposite->GetValue();

        if (nValue + nOpposite > Max(pEdit->GetMax(), pOpposite->GetMax()))
            pOpposite->SetValue(pOpposite->GetMax() - nValue);
    }

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



IMPL_LINK( SwWrapTabPage, WrapTypeHdl, ImageRadioButton *, pBtn )
{
    BOOL bWrapThrough = (pBtn == &aWrapThroughRB);
    aWrapTransparentCB.Enable( bWrapThrough && !bHtmlMode );
    bWrapThrough |= ( nAnchorId == FLY_IN_CNTNT );
    aWrapOutlineCB.Enable( !bWrapThrough && pBtn != &aNoWrapRB);
    aWrapOutsideCB.Enable( !bWrapThrough && aWrapOutlineCB.IsChecked() );
    aWrapAnchorOnlyCB.Enable( (nAnchorId == FLY_AT_CNTNT || nAnchorId == FLY_AUTO_CNTNT) &&
        (pBtn != &aNoWrapRB) );

    ContourHdl(0);
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



IMPL_LINK( SwWrapTabPage, ContourHdl, CheckBox *, EMPTYARG )
{
    BOOL bEnable = !(aWrapOutlineCB.IsChecked() && aWrapOutlineCB.IsEnabled());

    aWrapOutsideCB.Enable(!bEnable);

    bEnable =  !aWrapOutlineCB.IsChecked();
    if (bEnable == bContourImage) // damit es nicht immer flackert
    {
        bContourImage = !bEnable;
        ApplyImageList();
    }

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
USHORT* SwWrapTabPage::GetRanges()
{
    return aWrapPageRg;
}
/* -----------------------------08.05.2002 14:28------------------------------

 ---------------------------------------------------------------------------*/
void SwWrapTabPage::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyImageList();

    SfxTabPage::DataChanged( rDCEvt );
}
/* -----------------------------08.05.2002 14:28------------------------------

 ---------------------------------------------------------------------------*/
void SwWrapTabPage::ApplyImageList()
{
    ImageList& rImgLst = GetSettings().GetStyleSettings().GetHighContrastMode() ?
        aWrapILH : aWrapIL;

    aWrapThroughRB.SetModeRadioImage(rImgLst.GetImage(IMG_THROUGH));
    BOOL bWrapOutline =  !aWrapOutlineCB.IsChecked();
    if(bWrapOutline)
    {
        aNoWrapRB.SetModeRadioImage(rImgLst.GetImage(IMG_NONE));
        aWrapLeftRB.SetModeRadioImage(rImgLst.GetImage(IMG_LEFT));
        aWrapRightRB.SetModeRadioImage(rImgLst.GetImage(IMG_RIGHT));
        aWrapParallelRB.SetModeRadioImage(rImgLst.GetImage(IMG_PARALLEL));
        aIdealWrapRB.SetModeRadioImage(rImgLst.GetImage(IMG_IDEAL));
    }
    else
    {
        aNoWrapRB.SetModeRadioImage(rImgLst.GetImage( IMG_KON_NONE ));
        aWrapLeftRB.SetModeRadioImage(rImgLst.GetImage( IMG_KON_LEFT ));
        aWrapRightRB.SetModeRadioImage(rImgLst.GetImage( IMG_KON_RIGHT ));
        aWrapParallelRB.SetModeRadioImage(rImgLst.GetImage(IMG_KON_PARALLEL ));
        aIdealWrapRB.SetModeRadioImage(rImgLst.GetImage( IMG_KON_IDEAL ));
    }
}
