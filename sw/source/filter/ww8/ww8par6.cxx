/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ww8par6.cxx,v $
 * $Revision: 1.188.8.1 $
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
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
#include <stdlib.h>
#include <svtools/itemiter.hxx>
#include <rtl/tencinfo.h>


#include <hintids.hxx>
#include <svx/lspcitem.hxx>
#include <svx/wrlmitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/kernitem.hxx>
#include <svx/langitem.hxx>
#include <svx/cmapitem.hxx>
#include <svx/shdditem.hxx>
#ifndef _SVX_CNTRITEM_HXX //autogen
#include <svx/cntritem.hxx>
#endif
#include <svx/crsditem.hxx>
#include <svx/postitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/colritem.hxx>
#include <svx/brshitem.hxx>
#include <svx/spltitem.hxx>
#include <svx/keepitem.hxx>
#include <svx/orphitem.hxx>
#include <svx/widwitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/escpitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/boxitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lrspitem.hxx>
#ifndef _SVX_TSTPITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#include <svx/akrnitem.hxx>
#include <svx/paperinf.hxx>
#ifndef _SVX_EMPHITEM_HXX //autogen
#include <svx/emphitem.hxx>
#endif
#include <svx/forbiddenruleitem.hxx>
#include <svx/twolinesitem.hxx>
#ifndef _SVX_SCRIPSPACEITEM_HXX
#include <svx/scriptspaceitem.hxx>
#endif
#include <svx/hngpnctitem.hxx>
#include <svx/pbinitem.hxx>
#include <svx/charscaleitem.hxx>
#include <svx/charrotateitem.hxx>
#include <svx/charreliefitem.hxx>
#include <svx/blnkitem.hxx>
#include <svx/hyznitem.hxx>
#include <svx/paravertalignitem.hxx>
#include <svx/pgrditem.hxx>
#include <svx/frmdiritem.hxx>
#include <svx/charhiddenitem.hxx>
#include <fmtpdsc.hxx>
#include <node.hxx>
#include <ndtxt.hxx> // SwTxtNode, siehe unten: JoinNode()
#include <pam.hxx>              // fuer SwPam
#include <doc.hxx>
#include <pagedesc.hxx>         // class SwPageDesc
#include <fmtanchr.hxx>
#include <fmtcntnt.hxx>
#include <fchrfmt.hxx>
#include <fmthdft.hxx>
#include <fmtclds.hxx>
#include <fmtftntx.hxx>
#include <frmatr.hxx>
#include <section.hxx>
#include <lineinfo.hxx>
#include <fmtline.hxx>
#include <txatbase.hxx>
#include <fmtflcnt.hxx>
#include <fmtclbl.hxx>
#include <tgrditem.hxx>
#include <hfspacingitem.hxx>
#include <swtable.hxx>
#include <fltini.hxx>   //For CalculateFlySize
#include "writerhelper.hxx"
#include "writerwordglue.hxx"
#include "ww8scan.hxx"
#include "ww8par2.hxx"          // class WW8RStyle, class WwAnchorPara
#include "ww8graf.hxx"

// OD 2004-05-18 #i27767#
#include <fmtwrapinfluenceonobjpos.hxx>

using namespace sw::util;
using namespace sw::types;
using namespace ::com::sun::star;
using namespace nsHdFtFlags;

//-----------------------------------------
//              diverses
//-----------------------------------------

#define MM_250 1417             // WW-Default fuer Hor. Seitenraender: 2.5 cm
#define MM_200 1134             // WW-Default fuer u.Seitenrand: 2.0 cm


BYTE lcl_ReadBorders(bool bVer67, WW8_BRC* brc, WW8PLCFx_Cp_FKP* pPap,
    const WW8RStyle* pSty = 0, const WW8PLCFx_SEPX* pSep = 0);


ColorData SwWW8ImplReader::GetCol(BYTE nIco)
{
    static const ColorData eSwWW8ColA[] =
    {
        COL_AUTO, COL_BLACK, COL_LIGHTBLUE, COL_LIGHTCYAN, COL_LIGHTGREEN,
        COL_LIGHTMAGENTA, COL_LIGHTRED, COL_YELLOW, COL_WHITE, COL_BLUE,
        COL_CYAN, COL_GREEN, COL_MAGENTA, COL_RED, COL_BROWN, COL_GRAY,
        COL_LIGHTGRAY
    };

    return eSwWW8ColA[nIco];
}

inline sal_uInt32 MSRoundTweak(sal_uInt32 x)
{
    return x;
}

/***************************************************************************
#  Seiten - Attribute, die nicht ueber die Attribut-Verwaltung, sondern
#   ueber ...->HasSprm abgearbeitet werden
#   ( ausser OLST, dass weiterhin ein normales Attribut ist )
#**************************************************************************/

static short ReadSprm( const WW8PLCFx_SEPX* pSep, USHORT nId, short nDefaultVal )
{
    const BYTE* pS = pSep->HasSprm( nId );          // sprm da ?
    short nVal = ( pS ) ? SVBT16ToShort( pS ) : nDefaultVal;
    return nVal;
}

static USHORT ReadUSprm( const WW8PLCFx_SEPX* pSep, USHORT nId, short nDefaultVal )
{
    const BYTE* pS = pSep->HasSprm( nId );          // sprm da ?
    USHORT nVal = ( pS ) ? SVBT16ToShort( pS ) : nDefaultVal;
    return nVal;
}

static BYTE ReadBSprm( const WW8PLCFx_SEPX* pSep, USHORT nId, BYTE nDefaultVal )
{
    const BYTE* pS = pSep->HasSprm( nId );          // sprm da ?
    BYTE nVal = ( pS ) ? SVBT8ToByte( pS ) : nDefaultVal;
    return nVal;
}

void wwSection::SetDirection()
{
    //sprmSTextFlow
    switch (maSep.wTextFlow)
    {
        default:
            ASSERT(!this, "Unknown layout type");
        case 0:
            meDir=FRMDIR_HORI_LEFT_TOP;
            break;
        case 1:
            meDir=FRMDIR_VERT_TOP_RIGHT;
            break;
        case 2:
            //asian letters are not rotated, western are. We can't import
            //bottom to top going left to right, we can't do this in
            //pages, (in drawboxes we could partly hack it with a rotated
            //drawing box, though not frame)
            meDir=FRMDIR_VERT_TOP_RIGHT;
            break;
        case 3:
            //asian letters are not rotated, western are. We can't import
            meDir=FRMDIR_VERT_TOP_RIGHT;
            break;
        case 4:
            //asian letters are rotated, western not. We can't import
            meDir=FRMDIR_HORI_LEFT_TOP;
            break;
    }

    sal_uInt8 nRTLPgn = maSep.fBiDi;
    if ((meDir == FRMDIR_HORI_LEFT_TOP) && nRTLPgn)
        meDir = FRMDIR_HORI_RIGHT_TOP;
}

bool wwSection::IsVertical() const
{
    if (meDir == FRMDIR_VERT_TOP_RIGHT || meDir == FRMDIR_VERT_TOP_LEFT)
        return true;
    return false;
}

/*
  #113694#
  This is something of festering mapping, I'm open to better ways of doing it,
  but primarily the grid in writer is different to that in word. In writer the
  grid elements are squares with ruby rows inbetween. While in word there is no
  ruby stuff, and the elements are rectangles. By misusing the ruby row I can
  handle distortions in one direction, but its all a bit of a mess:
*/
void SwWW8ImplReader::SetDocumentGrid(SwFrmFmt &rFmt, const wwSection &rSection)
{
    if (bVer67)
        return;

    rFmt.SetFmtAttr(SvxFrameDirectionItem(rSection.meDir, RES_FRAMEDIR));

    SwTwips nTextareaHeight = rFmt.GetFrmSize().GetHeight();
    const SvxULSpaceItem &rUL = ItemGet<SvxULSpaceItem>(rFmt, RES_UL_SPACE);
    nTextareaHeight -= rUL.GetUpper();
    nTextareaHeight -= rUL.GetLower();

    SwTwips nTextareaWidth = rFmt.GetFrmSize().GetWidth();
    const SvxLRSpaceItem &rLR = ItemGet<SvxLRSpaceItem>(rFmt, RES_LR_SPACE);
    nTextareaWidth -= rLR.GetLeft();
    nTextareaWidth -= rLR.GetRight();

    if (rSection.IsVertical())
        std::swap(nTextareaHeight, nTextareaWidth);

    SwTextGridItem aGrid;
    aGrid.SetDisplayGrid(false);
    aGrid.SetPrintGrid(false);
    SwTextGrid eType=GRID_NONE;

    switch (rSection.maSep.clm)
    {
        case 0:
            eType = GRID_NONE;
            break;
        default:
            ASSERT(!this, "Unknown grid type");
        case 3:
            eType = GRID_LINES_CHARS;
            aGrid.SetSnapToChars(sal_True);
            break;
        case 1:
            eType = GRID_LINES_CHARS;
            aGrid.SetSnapToChars(sal_False);
            break;
        case 2:
            eType = GRID_LINES_ONLY;
            break;
    }

    aGrid.SetGridType(eType);

    // seem to not add external leading in word, or the character would run across
    // two line in some cases.
    if (eType != GRID_NONE)
        rDoc.set(IDocumentSettingAccess::ADD_EXT_LEADING, false);

   //force to set document as standard page mode
    sal_Bool bSquaredMode = sal_False;
    rDoc.SetDefaultPageMode( bSquaredMode );
    aGrid.SetSquaredMode( bSquaredMode );

    //sep.dyaLinePitch
    sal_Int32 nLinePitch = rSection.maSep.dyaLinePitch;

    //Get the size of word's default styles font
    sal_uInt32 nCharWidth=240;
    for (USHORT nI = 0; nI < pStyles->GetCount(); ++nI)
    {
        if (pCollA[nI].bValid && pCollA[nI].pFmt &&
            pCollA[nI].GetWWStyleId() == 0)
        {
            nCharWidth = ItemGet<SvxFontHeightItem>(*(pCollA[nI].pFmt),
                RES_CHRATR_CJK_FONTSIZE).GetHeight();
            break;
        }
    }

    //dxtCharSpace
    if (rSection.maSep.dxtCharSpace)
    {
        UINT32 nCharSpace = rSection.maSep.dxtCharSpace;
        //main lives in top 20 bits, and is signed.
        INT32 nMain = (nCharSpace & 0xFFFFF000);
        nMain/=0x1000;
        nCharWidth += nMain*20;

        int nFraction = (nCharSpace & 0x00000FFF);
        nFraction = (nFraction*20)/0xFFF;
        nCharWidth += nFraction;
    }

    aGrid.SetBaseWidth( writer_cast<sal_uInt16>(nCharWidth));
    aGrid.SetLines(writer_cast<sal_uInt16>(nTextareaHeight/nLinePitch));
    aGrid.SetBaseHeight(writer_cast<sal_uInt16>(nLinePitch));

    // ruby height is not supported in ww8
    //sal_Int32 nRubyHeight = nLinePitch - nCharWidth;
    //if (nRubyHeight < 0)
    //    nRubyHeight = 0;
    sal_Int32 nRubyHeight = 0;
    aGrid.SetRubyHeight(writer_cast<sal_uInt16>(nRubyHeight));

    rFmt.SetFmtAttr(aGrid);
}

void SwWW8ImplReader::Read_ParaBiDi(USHORT, const BYTE* pData, short nLen)
{
    if( nLen < 0 )
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_FRAMEDIR);
    else
    {
        SvxFrameDirection eDir =
            *pData ? FRMDIR_HORI_RIGHT_TOP : FRMDIR_HORI_LEFT_TOP;
        NewAttr(SvxFrameDirectionItem(eDir, RES_FRAMEDIR));
    }
}

bool wwSectionManager::SetCols(SwFrmFmt &rFmt, const wwSection &rSection,
    sal_uInt32 nNettoWidth) const
{
    //sprmSCcolumns - Anzahl der Spalten - 1
    sal_Int16 nCols = rSection.NoCols();

    if (nCols < 2)
        return false;                   // keine oder bloedsinnige Spalten

    SwFmtCol aCol;                      // Erzeuge SwFmtCol

    //sprmSDxaColumns   - Default-Abstand 1.25 cm
    sal_Int32 nColSpace = rSection.StandardColSeperation();

    // sprmSLBetween
    if (rSection.maSep.fLBetween)
    {
        aCol.SetLineAdj(COLADJ_TOP);      // Line
        aCol.SetLineHeight(100);
        aCol.SetLineColor(Color(COL_BLACK));
        aCol.SetLineWidth(1);
    }

    aCol.Init(nCols, writer_cast<USHORT>(nColSpace),
        writer_cast<USHORT>(nNettoWidth));

    // sprmSFEvenlySpaced
    if (!rSection.maSep.fEvenlySpaced)
    {
        aCol._SetOrtho(false);
        int nIdx = 1;
        for (USHORT i = 0; i < nCols; i++ )
        {
            SwColumn* pCol = aCol.GetColumns()[i];
            sal_Int32 nLeft = rSection.maSep.rgdxaColumnWidthSpacing[nIdx-1]/2;
            sal_Int32 nRight = rSection.maSep.rgdxaColumnWidthSpacing[nIdx+1]/2;
            sal_Int32 nWishWidth = rSection.maSep.rgdxaColumnWidthSpacing[nIdx]
                + nLeft + nRight;
            pCol->SetWishWidth(writer_cast<USHORT>(nWishWidth));
            pCol->SetLeft(writer_cast<USHORT>(nLeft));
            pCol->SetRight(writer_cast<USHORT>(nRight));
            nIdx += 2;
        }
        aCol.SetWishWidth(writer_cast<USHORT>(nNettoWidth));
    }
    rFmt.SetFmtAttr(aCol);
    return true;
}

void wwSectionManager::SetLeftRight(wwSection &rSection)
{
    // 3. LR-Raender
    sal_uInt32 nWWLe = MSRoundTweak(rSection.maSep.dxaLeft);
    sal_uInt32 nWWRi = MSRoundTweak(rSection.maSep.dxaRight);
    sal_uInt32 nWWGu = rSection.maSep.dzaGutter;

    /*
    fRTLGutter is set if the gutter is on the right, the gutter is otherwise
    placed on the left unless the global dop options are to put it on top, that
    case is handled in GetPageULData.
    */
    if (rSection.maSep.fRTLGutter)
        nWWRi += nWWGu;
    else if (!mrReader.pWDop->iGutterPos)
        nWWLe += nWWGu;

    // Left / Right
    if ((rSection.nPgWidth - nWWLe - nWWRi) < MINLAY)
    {
        /*
        There are some label templates which are "broken", they specify
        margins which make no sense e.g. Left 16.10cm, Right 16.10cm. So the
        space left between the margins is less than 0 In word the left margin
        is honoured and if the right margin would be past the left margin is
        left at the left margin position.

        Now this will work fine for importing, layout and exporting, *but* the
        page layout dialog has a hardcoded minimum page width of 0.5cm so it
        will report a different value than what is actually being used. i.e.
        it will add up the values to give a wider page than is actually being
        used.
        */
        nWWRi = rSection.nPgWidth - nWWLe - MINLAY;
    }

    rSection.nPgLeft = nWWLe;
    rSection.nPgRight = nWWRi;
}

void wwSectionManager::SetPage(SwPageDesc &rInPageDesc, SwFrmFmt &rFmt,
    const wwSection &rSection, bool bIgnoreCols) const
{
    // 1. Orientierung
    rInPageDesc.SetLandscape(rSection.IsLandScape());

    // 2. Papiergroesse
    SwFmtFrmSize aSz( rFmt.GetFrmSize() );
    aSz.SetWidth(rSection.GetPageWidth());
    aSz.SetHeight(SvxPaperInfo::GetSloppyPaperDimension(rSection.GetPageHeight()));
    rFmt.SetFmtAttr(aSz);

    rFmt.SetFmtAttr(
        SvxLRSpaceItem(rSection.GetPageLeft(), rSection.GetPageRight(), 0, 0, RES_LR_SPACE));

    if (!bIgnoreCols)
        SetCols(rFmt, rSection, rSection.GetTextAreaWidth());
}

USHORT lcl_MakeSafeNegativeSpacing(USHORT nIn)
{
    if (nIn > SHRT_MAX)
        nIn = 0;
    return nIn;
}

void SwWW8ImplReader::SetPageBorder(SwFrmFmt &rFmt, const wwSection &rSection) const
{
    if (!IsBorder(rSection.brc))
        return;

    SfxItemSet aSet(rFmt.GetAttrSet());
    short aSizeArray[5]={0};
    SetFlyBordersShadow(aSet, rSection.brc, &aSizeArray[0]);
    SvxLRSpaceItem aLR(ItemGet<SvxLRSpaceItem>(aSet, RES_LR_SPACE));
    SvxULSpaceItem aUL(ItemGet<SvxULSpaceItem>(aSet, RES_UL_SPACE));

    SvxBoxItem aBox(ItemGet<SvxBoxItem>(aSet, RES_BOX));
    short aOriginalBottomMargin = aBox.GetDistance(BOX_LINE_BOTTOM);

    if (rSection.maSep.pgbOffsetFrom == 1)
    {
        USHORT nDist;
        if (aBox.GetLeft())
        {
            nDist = aBox.GetDistance(BOX_LINE_LEFT);
            aBox.SetDistance(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aLR.GetLeft() - nDist)), BOX_LINE_LEFT);
            aSizeArray[WW8_LEFT] =
                aSizeArray[WW8_LEFT] - nDist + aBox.GetDistance(BOX_LINE_LEFT);
        }

        if (aBox.GetRight())
        {
            nDist = aBox.GetDistance(BOX_LINE_RIGHT);
            aBox.SetDistance(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aLR.GetRight() - nDist)), BOX_LINE_RIGHT);
            aSizeArray[WW8_RIGHT] =
                aSizeArray[WW8_RIGHT] - nDist + aBox.GetDistance(BOX_LINE_RIGHT);
        }

        if (aBox.GetTop())
        {
            nDist = aBox.GetDistance(BOX_LINE_TOP);
            aBox.SetDistance(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aUL.GetUpper() - nDist)), BOX_LINE_TOP);
            aSizeArray[WW8_TOP] =
                aSizeArray[WW8_TOP] - nDist + aBox.GetDistance(BOX_LINE_TOP);
        }

        if (aBox.GetBottom())
        {
            nDist = aBox.GetDistance(BOX_LINE_BOTTOM);
            aBox.SetDistance(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aUL.GetLower() - nDist)), BOX_LINE_BOTTOM);
            aSizeArray[WW8_BOT] =
                aSizeArray[WW8_BOT] - nDist + aBox.GetDistance(BOX_LINE_BOTTOM);
        }

        aSet.Put(aBox);
    }

    if (aBox.GetLeft())
        aLR.SetLeft(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aLR.GetLeft() - aSizeArray[WW8_LEFT])));
    if (aBox.GetRight())
        aLR.SetRight(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aLR.GetRight() - aSizeArray[WW8_RIGHT])));
    if (aBox.GetTop())
        aUL.SetUpper(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aUL.GetUpper() - aSizeArray[WW8_TOP])));
    if (aBox.GetBottom())
    {
        //#i30088# and #i30074# - do a final sanity check on
        //bottom value. Do not allow a resulting zero if bottom
        //Border margin value was not originally zero.
        if(aUL.GetLower() != 0)
            aUL.SetLower(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aUL.GetLower() - aSizeArray[WW8_BOT])));
        else
            aUL.SetLower(lcl_MakeSafeNegativeSpacing(static_cast<USHORT>(aOriginalBottomMargin - aSizeArray[WW8_BOT])));
    }

    aSet.Put(aLR);
    aSet.Put(aUL);
    rFmt.SetFmtAttr(aSet);
}

void wwSectionManager::GetPageULData(const wwSection &rSection, bool bFirst,
    wwSectionManager::wwULSpaceData& rData) const
{
    sal_Int32 nWWUp = rSection.maSep.dyaTop;
    sal_Int32 nWWLo = rSection.maSep.dyaBottom;
    sal_uInt32 nWWHTop = rSection.maSep.dyaHdrTop;
    sal_uInt32 nWWFBot = rSection.maSep.dyaHdrBottom;

    /*
    If there is gutter in 97+ and the dop says put it on top then get the
    gutter distance and set it to the top margin. When we are "two pages
    in one" the gutter is put at the top of odd pages, and bottom of
    even pages, something we cannot do. So we will put it on top of all
    pages, that way the pages are at least the right size.
    */
    if (
         (!mrReader.bVer67 && mrReader.pWDop->iGutterPos &&
         rSection.maSep.fRTLGutter)
       )
    {
        nWWUp += rSection.maSep.dzaGutter;
    }

    if( bFirst )
        rData.bHasHeader = (rSection.maSep.grpfIhdt & WW8_HEADER_FIRST) !=0;
    else
    {
        rData.bHasHeader = (rSection.maSep.grpfIhdt &
            (WW8_HEADER_EVEN | WW8_HEADER_ODD)) != 0;
    }

    if( rData.bHasHeader )
    {
        rData.nSwUp  = nWWHTop;             // Header -> umrechnen
        // --> CMC, OD 2004-06-18 #i19922# - correction:
        // consider that <nWWUp> can be negative, compare only if it's positive
        if ( nWWUp > 0 &&
             static_cast<sal_uInt32>(abs(nWWUp)) >= nWWHTop )
            rData.nSwHLo = nWWUp - nWWHTop;
        else
            rData.nSwHLo = 0;

        // --> OD 2004-06-18 #i19922# - minimum page header height is now 1mm
        // use new constant <cMinHdFtHeight>
        if (rData.nSwHLo < sal::static_int_cast< sal_uInt32 >(cMinHdFtHeight))
            rData.nSwHLo = sal::static_int_cast< sal_uInt32 >(cMinHdFtHeight);
    }
    else // kein Header -> Up einfach uebernehmen
        rData.nSwUp = Abs(nWWUp);

    if( bFirst )
        rData.bHasFooter = (rSection.maSep.grpfIhdt &  WW8_FOOTER_FIRST) !=0;
    else
    {
        rData.bHasFooter = (rSection.maSep.grpfIhdt &
            (WW8_FOOTER_EVEN | WW8_FOOTER_ODD)) != 0;
    }

    if( rData.bHasFooter )
    {
        rData.nSwLo = nWWFBot;              // Footer -> Umrechnen
        // --> CMC, OD 2004-06-18 #i19922# - correction:
        // consider that <nWWLo> can be negative, compare only if it's positive
        if ( nWWLo > 0 &&
             static_cast<sal_uInt32>(abs(nWWLo)) >= nWWFBot )
            rData.nSwFUp = nWWLo - nWWFBot;
        else
            rData.nSwFUp = 0;

        // --> OD 2004-06-18 #i19922# - minimum page header height is now 1mm
        // use new constant <cMinHdFtHeight>
        if (rData.nSwFUp < sal::static_int_cast< sal_uInt32 >(cMinHdFtHeight))
            rData.nSwFUp = sal::static_int_cast< sal_uInt32 >(cMinHdFtHeight);
    }
    else // kein Footer -> Lo einfach uebernehmen
        rData.nSwLo = Abs(nWWLo);
}

void wwSectionManager::SetPageULSpaceItems(SwFrmFmt &rFmt,
    wwSectionManager::wwULSpaceData& rData, const wwSection &rSection) const
{
    if (rData.bHasHeader)               // ... und Header-Lower setzen
    {
        //Kopfzeilenhoehe minimal sezten
        if (SwFrmFmt* pHdFmt = (SwFrmFmt*)rFmt.GetHeader().GetHeaderFmt())
        {
            SvxULSpaceItem aHdUL(pHdFmt->GetULSpace());
            if (!rSection.IsFixedHeightHeader())    //normal
            {
                pHdFmt->SetFmtAttr(SwFmtFrmSize(ATT_MIN_SIZE, 0, rData.nSwHLo));
                // --> OD 2004-06-18 #i19922# - minimum page header height is now 1mm
                // use new constant <cMinHdFtHeight>
                aHdUL.SetLower( writer_cast<USHORT>(rData.nSwHLo - cMinHdFtHeight) );
                pHdFmt->SetFmtAttr(SwHeaderAndFooterEatSpacingItem(
                    RES_HEADER_FOOTER_EAT_SPACING, true));
            }
            else
            {
                // --> OD 2005-05-20 #i48832# - set correct spacing between
                // header and body.
                const SwTwips nHdLowerSpace( Abs(rSection.maSep.dyaTop) - rData.nSwUp - rData.nSwHLo );
                pHdFmt->SetFmtAttr(SwFmtFrmSize(ATT_FIX_SIZE, 0, rData.nSwHLo + nHdLowerSpace));
                aHdUL.SetLower( static_cast< USHORT >(nHdLowerSpace) );
                // <--
                pHdFmt->SetFmtAttr(SwHeaderAndFooterEatSpacingItem(
                    RES_HEADER_FOOTER_EAT_SPACING, false));
            }
            pHdFmt->SetFmtAttr(aHdUL);
        }
    }

    if (rData.bHasFooter)               // ... und Footer-Upper setzen
    {
        if (SwFrmFmt* pFtFmt = (SwFrmFmt*)rFmt.GetFooter().GetFooterFmt())
        {
            SvxULSpaceItem aFtUL(pFtFmt->GetULSpace());
            if (!rSection.IsFixedHeightFooter())    //normal
            {
                pFtFmt->SetFmtAttr(SwFmtFrmSize(ATT_MIN_SIZE, 0, rData.nSwFUp));
                // --> OD 2004-06-18 #i19922# - minimum page header height is now 1mm
                // use new constant <cMinHdFtHeight>
                aFtUL.SetUpper( writer_cast<USHORT>(rData.nSwFUp - cMinHdFtHeight) );
                pFtFmt->SetFmtAttr(SwHeaderAndFooterEatSpacingItem(
                    RES_HEADER_FOOTER_EAT_SPACING, true));
            }
            else
            {
                // --> OD 2005-05-20 #i48832# - set correct spacing between
                // footer and body.
                const SwTwips nFtUpperSpace( Abs(rSection.maSep.dyaBottom) - rData.nSwLo - rData.nSwFUp );
                pFtFmt->SetFmtAttr(SwFmtFrmSize(ATT_FIX_SIZE, 0, rData.nSwFUp + nFtUpperSpace));
                aFtUL.SetUpper( static_cast< USHORT >(nFtUpperSpace) );
                // <--
                pFtFmt->SetFmtAttr(SwHeaderAndFooterEatSpacingItem(
                    RES_HEADER_FOOTER_EAT_SPACING, false));
            }
            pFtFmt->SetFmtAttr(aFtUL);
        }
    }

    SvxULSpaceItem aUL(writer_cast<USHORT>(rData.nSwUp),
        writer_cast<USHORT>(rData.nSwLo), RES_UL_SPACE);
    rFmt.SetFmtAttr(aUL);
}

SwSectionFmt *wwSectionManager::InsertSection(
    SwPaM& rMyPaM, wwSection &rSection)
{
    SwSection aSection( CONTENT_SECTION, mrReader.rDoc.GetUniqueSectionName() );

    SfxItemSet aSet( mrReader.rDoc.GetAttrPool(), aFrmFmtSetRange );

    sal_uInt8 nRTLPgn = maSegments.empty() ? 0 : maSegments.back().IsBiDi();
    aSet.Put(SvxFrameDirectionItem(
        nRTLPgn ? FRMDIR_HORI_RIGHT_TOP : FRMDIR_HORI_LEFT_TOP, RES_FRAMEDIR));

    if (2 == mrReader.pWDop->fpc)
        aSet.Put( SwFmtFtnAtTxtEnd(FTNEND_ATTXTEND));
    if (0 == mrReader.pWDop->epc)
        aSet.Put( SwFmtEndAtTxtEnd(FTNEND_ATTXTEND));

    aSection.SetProtect(SectionIsProtected(rSection));

    rSection.mpSection =
        mrReader.rDoc.InsertSwSection( rMyPaM, aSection, &aSet );
    ASSERT(rSection.mpSection, "section not inserted!");
    if (!rSection.mpSection)
        return 0;

    SwPageDesc *pPage = 0;
    mySegrIter aEnd = maSegments.rend();
    for (mySegrIter aIter = maSegments.rbegin(); aIter != aEnd; ++aIter)
    {
        if (0 != (pPage = aIter->mpPage))
            break;
    }

    ASSERT(pPage, "no page outside this section!");

    if (!pPage)
        pPage = &mrReader.rDoc._GetPageDesc(0);

    if (!pPage)
        return 0;

    SwSectionFmt *pFmt = rSection.mpSection->GetFmt();
    ASSERT(pFmt, "impossible");
    if (!pFmt)
        return 0;

    SwFrmFmt& rFmt = pPage->GetMaster();
    const SvxLRSpaceItem& rLR = rFmt.GetLRSpace();
    long nPageLeft  = rLR.GetLeft();
    long nPageRight = rLR.GetRight();
    long nSectionLeft = rSection.GetPageLeft() - nPageLeft;
    long nSectionRight = rSection.GetPageRight() - nPageRight;
    if ((nSectionLeft != 0) || (nSectionRight != 0))
    {
        SvxLRSpaceItem aLR(nSectionLeft, nSectionRight, 0, 0, RES_LR_SPACE);
        pFmt->SetFmtAttr(aLR);
    }

    SetCols(*pFmt, rSection, rSection.GetTextAreaWidth());
    return pFmt;
}

void SwWW8ImplReader::HandleLineNumbering(const wwSection &rSection)
{
    // check if Line Numbering must be activated or resetted
    if (mbNewDoc && rSection.maSep.nLnnMod)
    {
        // restart-numbering-mode: 0 per page, 1 per section, 2 never restart
        bool bRestartLnNumPerSection = (1 == rSection.maSep.lnc);

        if (bNoLnNumYet)
        {
            SwLineNumberInfo aInfo( rDoc.GetLineNumberInfo() );

            aInfo.SetPaintLineNumbers(true);

            aInfo.SetRestartEachPage(rSection.maSep.lnc == 0);

            aInfo.SetPosFromLeft(writer_cast<USHORT>(rSection.maSep.dxaLnn));

            //Paint only for every n line
            aInfo.SetCountBy(rSection.maSep.nLnnMod);

            // to be defaulted features ( HARDCODED in MS Word 6,7,8,9 )
            aInfo.SetCountBlankLines(true);
            aInfo.SetCountInFlys(false);
            aInfo.SetPos( LINENUMBER_POS_LEFT );
            SvxNumberType aNumType; // this sets SVX_NUM_ARABIC per default
            aInfo.SetNumType( aNumType );

            rDoc.SetLineNumberInfo( aInfo );
            bNoLnNumYet = false;
        }

        if (
            (0 < rSection.maSep.lnnMin) ||
            (bRestartLnNumPerSection && !bNoLnNumYet)
           )
        {
            SwFmtLineNumber aLN;
            if (const SwFmtLineNumber* pLN
                = (const SwFmtLineNumber*)GetFmtAttr(RES_LINENUMBER))
            {
                aLN.SetCountLines( pLN->IsCount() );
            }
            aLN.SetStartValue(1 + rSection.maSep.lnnMin);
            NewAttr(aLN);
            pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_LINENUMBER);
        }
        bNoLnNumYet = false;
    }
}

wwSection::wwSection(const SwPosition &rPos) : maStart(rPos.nNode),
    mpSection(0), mpTitlePage(0), mpPage(0), meDir(FRMDIR_HORI_LEFT_TOP),
    nPgWidth(SvxPaperInfo::GetPaperSize(PAPER_A4).Width()),
    nPgLeft(MM_250), nPgRight(MM_250), mnBorders(0), mbHasFootnote(false)
{
}

void wwSectionManager::SetNumberingType(const wwSection &rNewSection,
    SwPageDesc &rPageDesc) const
{
    // Seitennummernformat speichern
    static const SvxExtNumType aNumTyp[5] =
    {
        SVX_NUM_ARABIC, SVX_NUM_ROMAN_UPPER, SVX_NUM_ROMAN_LOWER,
        SVX_NUM_CHARS_UPPER_LETTER_N, SVX_NUM_CHARS_LOWER_LETTER_N
    };

    SvxNumberType aType;
    aType.SetNumberingType( static_cast< sal_Int16 >(aNumTyp[rNewSection.maSep.nfcPgn]) );
    rPageDesc.SetNumType(aType);
}

// Bei jedem Abschnittswechsel ( auch am Anfang eines Dokuments ) wird
// CreateSep gerufen, dass dann den / die Pagedesc(s) erzeugt und
// mit Attributen un KF-Texten fuellt.
// Dieses Vorgehen ist noetig geworden, da die UEbersetzung der verschiedenen
// Seiten-Attribute zu stark verflochten ist.
void wwSectionManager::CreateSep(const long nTxtPos, bool /*bMustHaveBreak*/)
{
    /*
    #i1909# #100688# section/page breaks should not occur in tables or subpage
    elements like frames. Word itself ignores them in this case. The bug is
    more likely that this filter created such documents in the past!
    */
    if (mrReader.nInTable || mrReader.bTxbxFlySection || mrReader.InLocalApo())
        return;

    WW8PLCFx_SEPX* pSep = mrReader.pPlcxMan->GetSepPLCF();
    ASSERT(pSep, "impossible!");
    if (!pSep)
        return;

    ww::WordVersion eVer = mrReader.GetFib().GetFIBVersion();

    // M.M. Create a linked section if the WkbPLCF
    // has an entry for one at this cp
    WW8PLCFspecial* pWkb = mrReader.pPlcxMan->GetWkbPLCF();
    if (pWkb && pWkb->SeekPosExact(nTxtPos) &&
            pWkb->Where() == nTxtPos)
    {
        void* pData;
        WW8_CP nTest;
        pWkb->Get(nTest, pData);
        String sSectionName = mrReader.aLinkStringMap[SVBT16ToShort( ((WW8_WKB*)pData)->nLinkId) ];
        mrReader.ConvertFFileName(sSectionName, sSectionName);
        SwSection aSection(FILE_LINK_SECTION, sSectionName);
        aSection.SetLinkFileName( sSectionName );
        aSection.SetProtect(true);
        // --> CMC, OD 2004-06-18 #i19922# improvement:
        // return value of method <Insert> not used.
        mrReader.rDoc.InsertSwSection(*mrReader.pPaM, aSection, 0, false);
    }

    wwSection aLastSection(*mrReader.pPaM->GetPoint());
    if (!maSegments.empty())
        aLastSection = maSegments.back();

    //Here
    USHORT nLIdx = ( ( mrReader.pWwFib->lid & 0xff ) == 0x9 ) ? 1 : 0;

    //BEGIN read section values
    wwSection aNewSection(*mrReader.pPaM->GetPoint());

    static const USHORT aVer2Ids0[] =
    {
        /*sprmSBkc*/           117,
        /*sprmSFTitlePage*/    118,
        /*sprmSNfcPgn*/        122,
        /*sprmSCcolumns*/      119,
        /*sprmSDxaColumns*/    120,
        /*sprmSLBetween*/      133
    };

    static const USHORT aVer67Ids0[] =
    {
        /*sprmSBkc*/           142,
        /*sprmSFTitlePage*/    143,
        /*sprmSNfcPgn*/        147,
        /*sprmSCcolumns*/      144,
        /*sprmSDxaColumns*/    145,
        /*sprmSLBetween*/      158
    };

    static const USHORT aVer8Ids0[] =
    {
        /*sprmSBkc*/           0x3009,
        /*sprmSFTitlePage*/    0x300A,
        /*sprmSNfcPgn*/        0x300E,
        /*sprmSCcolumns*/      0x500B,
        /*sprmSDxaColumns*/    0x900C,
        /*sprmSLBetween*/      0x3019
    };

    const USHORT* pIds = eVer <= ww::eWW2 ? aVer2Ids0 : eVer <= ww::eWW7 ? aVer67Ids0 : aVer8Ids0;

    if (!maSegments.empty())
    {
        // Type of break: break codes are:
        // 0 No break
        // 1 New column
        // 2 New page
        // 3 Even page
        // 4 Odd page
        if (const BYTE* pSprmBkc = pSep->HasSprm(pIds[0]))
            aNewSection.maSep.bkc = *pSprmBkc;
    }

    // Has a table page
    aNewSection.maSep.fTitlePage =
        (0 != ReadBSprm( pSep, pIds[1], 0 ));

    // sprmSNfcPgn
    aNewSection.maSep.nfcPgn = ReadBSprm( pSep, pIds[2], 0 );
    if (aNewSection.maSep.nfcPgn > 4)
        aNewSection.maSep.nfcPgn = 0;

    aNewSection.maSep.fUnlocked = eVer > ww::eWW2 ? ReadBSprm(pSep, (eVer <= ww::eWW7 ? 139 : 0x3006), 0 ) : 0;

    // sprmSFBiDi
    aNewSection.maSep.fBiDi = eVer >= ww::eWW8 ? ReadBSprm(pSep, 0x3228, 0) : 0;

    aNewSection.maSep.ccolM1 = ReadSprm(pSep, pIds[3], 0 );

    //sprmSDxaColumns   - Default-Abstand 1.25 cm
    aNewSection.maSep.dxaColumns = ReadUSprm( pSep, pIds[4], 708 );

    // sprmSLBetween
    aNewSection.maSep.fLBetween = ReadBSprm(pSep, pIds[5], 0 );

    if (eVer >= ww::eWW6)
    {
        // sprmSFEvenlySpaced
        aNewSection.maSep.fEvenlySpaced =
            ReadBSprm(pSep, (eVer <= ww::eWW7 ? 138 : 0x3005), 1) ? true : false;

        if (aNewSection.maSep.ccolM1 > 0 && !aNewSection.maSep.fEvenlySpaced)
        {
            aNewSection.maSep.rgdxaColumnWidthSpacing[0] = 0;
            int nCols = aNewSection.maSep.ccolM1 + 1;
            int nIdx = 0;
            for (int i = 0; i < nCols; ++i)
            {
                //sprmSDxaColWidth
                const BYTE* pSW = pSep->HasSprm( (eVer <= ww::eWW7 ? 136 : 0xF203), BYTE( i ) );

                ASSERT( pSW, "+Sprm 136 (bzw. 0xF203) (ColWidth) fehlt" );
                sal_uInt16 nWidth = pSW ? SVBT16ToShort(pSW + 1) : 1440;

                aNewSection.maSep.rgdxaColumnWidthSpacing[++nIdx] = nWidth;

                if (i < nCols-1)
                {
                    //sprmSDxaColSpacing
                    const BYTE* pSD = pSep->HasSprm( (eVer <= ww::eWW7 ? 137 : 0xF204), BYTE( i ) );

                    ASSERT( pSD, "+Sprm 137 (bzw. 0xF204) (Colspacing) fehlt" );
                    if( pSD )
                    {
                        nWidth = SVBT16ToShort(pSD + 1);
                        aNewSection.maSep.rgdxaColumnWidthSpacing[++nIdx] = nWidth;
                    }
                }
            }
        }
    }

    static const USHORT aVer2Ids1[] =
    {
        /*sprmSBOrientation*/   137,
        /*sprmSXaPage*/         139,
        /*sprmSYaPage*/         140,
        /*sprmSDxaLeft*/        141,
        /*sprmSDxaRight*/       142,
        /*sprmSDzaGutter*/      145,
        /*sprmSFPgnRestart*/    125,
        /*sprmSPgnStart*/       136,
        /*sprmSDmBinFirst*/     115,
        /*sprmSDmBinOther*/     116
    };

    static const USHORT aVer67Ids1[] =
    {
        /*sprmSBOrientation*/   162,
        /*sprmSXaPage*/         164,
        /*sprmSYaPage*/         165,
        /*sprmSDxaLeft*/        166,
        /*sprmSDxaRight*/       167,
        /*sprmSDzaGutter*/      170,
        /*sprmSFPgnRestart*/    150,
        /*sprmSPgnStart*/       161,
        /*sprmSDmBinFirst*/     140,
        /*sprmSDmBinOther*/     141
    };

    static const USHORT aVer8Ids1[] =
    {
        /*sprmSBOrientation*/   0x301d,
        /*sprmSXaPage*/         0xB01F,
        /*sprmSYaPage*/         0xB020,
        /*sprmSDxaLeft*/        0xB021,
        /*sprmSDxaRight*/       0xB022,
        /*sprmSDzaGutter*/      0xB025,
        /*sprmSFPgnRestart*/    0x3011,
        /*sprmSPgnStart*/       0x501C,
           /*sprmSDmBinFirst*/     0x5007,
        /*sprmSDmBinOther*/     0x5008
    };

    pIds = eVer <= ww::eWW2 ? aVer2Ids1 : eVer <= ww::eWW7 ? aVer67Ids1 : aVer8Ids1;

                                            // 1. Orientierung
    aNewSection.maSep.dmOrientPage = ReadBSprm(pSep, pIds[0], 0);

    // 2. Papiergroesse
    aNewSection.maSep.xaPage = ReadUSprm(pSep, pIds[1], lLetterWidth);
    aNewSection.nPgWidth = SvxPaperInfo::GetSloppyPaperDimension(aNewSection.maSep.xaPage);

    aNewSection.maSep.yaPage = ReadUSprm(pSep, pIds[2], lLetterHeight);

    // 3. LR-Raender
    static const USHORT nLef[] = { MM_250, 1800 };
    static const USHORT nRig[] = { MM_250, 1800 };

    aNewSection.maSep.dxaLeft = ReadUSprm( pSep, pIds[3], nLef[nLIdx]);
    aNewSection.maSep.dxaRight = ReadUSprm( pSep, pIds[4], nRig[nLIdx]);

    //#110175# 2pages in 1sheet hackery ?
    //#i31806# but only swap if 2page in 1sheet is enabled.
    // its not clear if dmOrientPage is the correct member to
    // decide on this but I am not about to 2nd guess cmc.
    if(mrReader.pWDop->doptypography.f2on1 &&
            aNewSection.maSep.dmOrientPage == 2)
        std::swap(aNewSection.maSep.dxaLeft, aNewSection.maSep.dxaRight);

    aNewSection.maSep.dzaGutter = ReadUSprm( pSep, pIds[5], 0);

    aNewSection.maSep.fRTLGutter = static_cast< sal_uInt8 >(eVer >= ww::eWW8 ? ReadUSprm( pSep, 0x322A, 0 ) : 0);

    // Page Number Restarts - sprmSFPgnRestart
    aNewSection.maSep.fPgnRestart = ReadBSprm(pSep, pIds[6], 0);

    aNewSection.maSep.pgnStart = ReadBSprm( pSep, pIds[7], 0 );

    if (eVer >= ww::eWW6)
    {
        if (const BYTE* p = pSep->HasSprm( (eVer <= ww::eWW7 ? 132 : 0x3001) ))
            aNewSection.maSep.iHeadingPgn = *p;

        if (const BYTE* p = pSep->HasSprm( (eVer <= ww::eWW7 ? 131 : 0x3000) ))
            aNewSection.maSep.cnsPgn = *p;
    }

    if(const BYTE* pSprmSDmBinFirst = pSep->HasSprm( pIds[8] ))
        aNewSection.maSep.dmBinFirst = *pSprmSDmBinFirst;

    if (const BYTE* pSprmSDmBinOther = pSep->HasSprm( pIds[9] ))
        aNewSection.maSep.dmBinOther = *pSprmSDmBinOther;

    static const USHORT nTop[] = { MM_250, 1440 };
    static const USHORT nBot[] = { MM_200, 1440 };

    static const USHORT aVer2Ids2[] =
    {
        /*sprmSDyaTop*/         143,
        /*sprmSDyaBottom*/      144,
        /*sprmSDyaHdrTop*/      131,
        /*sprmSDyaHdrBottom*/   132,
        /*sprmSNLnnMod*/        129,
        /*sprmSLnc*/            127,
        /*sprmSDxaLnn*/         130,
        /*sprmSLnnMin*/         135
    };

    static const USHORT aVer67Ids2[] =
    {
        /*sprmSDyaTop*/         168,
        /*sprmSDyaBottom*/      169,
        /*sprmSDyaHdrTop*/      156,
        /*sprmSDyaHdrBottom*/   157,
        /*sprmSNLnnMod*/        154,
        /*sprmSLnc*/            152,
        /*sprmSDxaLnn*/         155,
        /*sprmSLnnMin*/         160
    };
    static const USHORT aVer8Ids2[] =
    {
        /*sprmSDyaTop*/         0x9023,
        /*sprmSDyaBottom*/      0x9024,
        /*sprmSDyaHdrTop*/      0xB017,
        /*sprmSDyaHdrBottom*/   0xB018,
        /*sprmSNLnnMod*/        0x5015,
        /*sprmSLnc*/            0x3013,
        /*sprmSDxaLnn*/         0x9016,
        /*sprmSLnnMin*/         0x501B
    };

    pIds = eVer <= ww::eWW2 ? aVer2Ids2 : eVer <= ww::eWW7 ? aVer67Ids2 : aVer8Ids2;

    aNewSection.maSep.dyaTop = ReadSprm( pSep, pIds[0], nTop[nLIdx] );
    aNewSection.maSep.dyaBottom = ReadSprm( pSep, pIds[1], nBot[nLIdx] );
    aNewSection.maSep.dyaHdrTop = ReadUSprm( pSep, pIds[2], 720 );
    aNewSection.maSep.dyaHdrBottom = ReadUSprm( pSep, pIds[3], 720 );

    if (eVer >= ww::eWW8)
    {
        aNewSection.maSep.wTextFlow = ReadUSprm(pSep, 0x5033, 0);
        aNewSection.maSep.clm = ReadUSprm( pSep, 0x5032, 0 );
        aNewSection.maSep.dyaLinePitch = ReadUSprm(pSep, 0x9031, 360);
        if (const BYTE* pS = pSep->HasSprm(0x7030))
            aNewSection.maSep.dxtCharSpace = SVBT32ToUInt32(pS);

        //sprmSPgbProp
        sal_uInt16 pgbProp = ReadSprm( pSep, 0x522F, 0 );
        aNewSection.maSep.pgbApplyTo = pgbProp & 0x0007;
        aNewSection.maSep.pgbPageDepth = (pgbProp & 0x0018) >> 3;
        aNewSection.maSep.pgbOffsetFrom = (pgbProp & 0x00E0) >> 5;

        aNewSection.mnBorders =
            ::lcl_ReadBorders(eVer <= ww::eWW7, aNewSection.brc, 0, 0, pSep);
    }

    // check if Line Numbering must be activated or resetted
    if (const BYTE* pSprmSNLnnMod = pSep->HasSprm( pIds[4] ))
        aNewSection.maSep.nLnnMod = *pSprmSNLnnMod;

    if (const BYTE* pSprmSLnc = pSep->HasSprm( pIds[5] ))
        aNewSection.maSep.lnc = *pSprmSLnc;

    if (const BYTE* pSprmSDxaLnn = pSep->HasSprm( pIds[6] ))
        aNewSection.maSep.dxaLnn = SVBT16ToShort( pSprmSDxaLnn );

    if (const BYTE* pSprmSLnnMin = pSep->HasSprm( pIds[7] ))
        aNewSection.maSep.lnnMin = *pSprmSLnnMin;

    if (eVer <= ww::eWW7)
        aNewSection.maSep.grpfIhdt = ReadBSprm(pSep, eVer <= ww::eWW2 ? 128 : 153, 0);
    else if (mrReader.pHdFt)
    {
        aNewSection.maSep.grpfIhdt = WW8_HEADER_ODD | WW8_FOOTER_ODD;

        if (aNewSection.HasTitlePage())
            aNewSection.maSep.grpfIhdt |= WW8_HEADER_FIRST | WW8_FOOTER_FIRST;

        if (mrReader.pWDop->fFacingPages)
            aNewSection.maSep.grpfIhdt |= WW8_HEADER_EVEN | WW8_FOOTER_EVEN;

        //See if we have a header or footer for each enabled possibility
        //if we do not then we inherit the previous sections header/footer,
        for (int nI = 0, nMask = 1; nI < 6; ++nI, nMask <<= 1)
        {
            if (aNewSection.maSep.grpfIhdt & nMask)
            {
                WW8_CP nStart;
                long nLen;
                mrReader.pHdFt->GetTextPosExact( static_cast< short >(nI + ( maSegments.size() + 1) * 6), nStart, nLen);
                //No header or footer, inherit pervious one, or set to zero
                //if no previous one
                if (!nLen)
                {
                    if (
                        maSegments.empty() ||
                        !(maSegments.back().maSep.grpfIhdt & nMask)
                       )
                    {
                        aNewSection.maSep.grpfIhdt &= ~nMask;
                    }
                }
            }
        }
    }

    SetLeftRight(aNewSection);
    //END read section values

    if (eVer >= ww::eWW8)
        aNewSection.SetDirection();

    mrReader.HandleLineNumbering(aNewSection);
    maSegments.push_back(aNewSection);
}

void SwWW8ImplReader::CopyPageDescHdFt(const SwPageDesc* pOrgPageDesc,
    SwPageDesc* pNewPageDesc, BYTE nCode )
{
    // copy first header content section
    if (nCode & WW8_HEADER_FIRST)
        rDoc.CopyHeader(pOrgPageDesc->GetMaster(), pNewPageDesc->GetMaster());

    // copy first footer content section
    if( nCode & WW8_FOOTER_FIRST )
        rDoc.CopyFooter(pOrgPageDesc->GetMaster(), pNewPageDesc->GetMaster());

    if( nCode & (   WW8_HEADER_ODD  | WW8_FOOTER_ODD
                  | WW8_HEADER_EVEN | WW8_FOOTER_EVEN ) )
    {
        // copy odd header content section
        if( nCode & WW8_HEADER_ODD )
        {
            rDoc.CopyHeader(pOrgPageDesc->GetMaster(),
                            pNewPageDesc->GetMaster() );
        }
        // copy odd footer content section
        if( nCode & WW8_FOOTER_ODD )
        {
            rDoc.CopyFooter(pOrgPageDesc->GetMaster(),
                            pNewPageDesc->GetMaster());
        }
        // copy even header content section
        if( nCode & WW8_HEADER_EVEN )
        {
            rDoc.CopyHeader(pOrgPageDesc->GetLeft(),
                            pNewPageDesc->GetLeft());
        }
        // copy even footer content section
        if( nCode & WW8_FOOTER_EVEN )
        {
            rDoc.CopyFooter(pOrgPageDesc->GetLeft(),
                            pNewPageDesc->GetLeft());
        }
    }
}

//------------------------------------------------------
//   Hilfsroutinen fuer Grafiken und Apos und Tabellen
//------------------------------------------------------

static bool _SetWW8_BRC(bool bVer67, WW8_BRC& rVar, const BYTE* pS)
{
    if( pS )
    {
        if( bVer67 )
            memcpy( rVar.aBits1, pS, sizeof( SVBT16 ) );
        else
            rVar = *((WW8_BRC*)pS);
    }

    return 0 != pS;
}

BYTE lcl_ReadBorders(bool bVer67, WW8_BRC* brc, WW8PLCFx_Cp_FKP* pPap,
    const WW8RStyle* pSty, const WW8PLCFx_SEPX* pSep)
{
// Ausgegend von diesen defines:
//      #define WW8_TOP 0
//      #define WW8_LEFT 1
//      #define WW8_BOT 2
//      #define WW8_RIGHT 3
//      #define WW8_BETW 4

//returns a BYTE filled with a bit for each position that had a sprm
//setting that border

    BYTE nBorder = false;
    if( pSep )
    {
        if( !bVer67 )
        {
             BYTE* pSprm[4];

            //  sprmSBrcTop, sprmSBrcLeft, sprmSBrcBottom, sprmSBrcRight
             if( pSep->Find4Sprms(  0x702B,   0x702C,   0x702D,   0x702E,
                                    pSprm[0], pSprm[1], pSprm[2], pSprm[3] ) )
             {
                for( int i = 0; i < 4; ++i )
                    nBorder |= (_SetWW8_BRC( bVer67, brc[ i ], pSprm[ i ] ))<<i;
             }
        }
    }
    else
    {

        static const USHORT aVer67Ids[5] = { 38, 39, 40, 41, 42 };

        static const USHORT aVer8Ids[5] =
                { 0x6424, 0x6425, 0x6426, 0x6427, 0x6428 };

        const USHORT* pIds = bVer67 ? aVer67Ids : aVer8Ids;

        if( pPap )
        {
            for( int i = 0; i < 5; ++i, ++pIds )
                nBorder |= (_SetWW8_BRC( bVer67, brc[ i ], pPap->HasSprm( *pIds )))<<i;
        }
        else if( pSty )
        {
            for( int i = 0; i < 5; ++i, ++pIds )
                nBorder |= (_SetWW8_BRC( bVer67, brc[ i ], pSty->HasParaSprm( *pIds )))<<i;
        }
        else {
            ASSERT( pSty || pPap, "WW8PLCFx_Cp_FKP and WW8RStyle "
                               "and WW8PLCFx_SEPX is 0" );
        }
    }

    return nBorder;
}

void GetLineIndex(SvxBoxItem &rBox, short nLineThickness, short nSpace, BYTE nCol, short nIdx,
    USHORT nOOIndex, USHORT nWWIndex, short *pSize=0)
{
    WW8_BordersSO::eBorderCode eCodeIdx;

    //Word mirrors some indexes inside outside depending on its position, we
    //don't do that, so flip them here
    if (nWWIndex == WW8_TOP || nWWIndex == WW8_LEFT)
    {
        switch (nIdx)
        {
            case 11:
            case 12:
                nIdx = (nIdx == 11) ? 12 : 11;
                break;
            case 14:
            case 15:
                nIdx = (nIdx == 14) ? 15 : 14;
                break;
            case 17:
            case 18:
                nIdx = (nIdx == 17) ? 18 : 17;
                break;
            case 24:
            case 25:
                nIdx = (nIdx == 24) ? 25 : 24;
                break;
        }
    }

    // Map to our border types, we should use of one equal line
    // thickness, or one of smaller thickness. If too small we
    // can make the defecit up in additional white space or
    // object size
    switch (nIdx)
    {
        // First the single lines
        case  1:
        case  2:
        case  5:
        // and the unsupported special cases which we map to a single line
        case  6:
        case  7:
        case  8:
        case  9:
        case 22:
        // or if in necessary by a double line
        case 24:
        case 25:
            if( nLineThickness < 10)
                eCodeIdx = WW8_BordersSO::single0;//   1 Twip for us
            else if( nLineThickness < 20)
                eCodeIdx = WW8_BordersSO::single5;//   10 Twips for us
            else if (nLineThickness < 50)
                eCodeIdx = WW8_BordersSO::single1;//  20 Twips
            else if (nLineThickness < 80)
                eCodeIdx = WW8_BordersSO::single2;//  50
            else if (nLineThickness < 100)
                eCodeIdx = WW8_BordersSO::single3;//  80
            else if (nLineThickness < 150)
                eCodeIdx = WW8_BordersSO::single4;// 100
            // Hack: for the quite thick lines we must paint double lines,
            // because our singles lines don't come thicker than 5 points.
            else if (nLineThickness < 180)
                eCodeIdx = WW8_BordersSO::double2;// 150
            else
                eCodeIdx = WW8_BordersSO::double5;// 180
        break;
        // then the shading beams which we represent by a double line
        case 23:
            eCodeIdx = WW8_BordersSO::double1;
        break;
        // then the double lines, for which we have good matches
        case  3:
        case 10: //Don't have tripple so use double
            if (nLineThickness < 60)
                eCodeIdx = WW8_BordersSO::double0;// 22 Twips for us
            else if (nLineThickness < 135)
                eCodeIdx = WW8_BordersSO::double7;// some more space
            else if (nLineThickness < 180)
                eCodeIdx = WW8_BordersSO::double1;// 60
            else
                eCodeIdx = WW8_BordersSO::double2;// 150
            break;
        case 11:
            eCodeIdx = WW8_BordersSO::double4;//  90 Twips for us
            break;
        case 12:
        case 13: //Don't have thin thick thin, so use thick thin
            if (nLineThickness < 87)
                eCodeIdx = WW8_BordersSO::double8;//  71 Twips for us
            else if (nLineThickness < 117)
                eCodeIdx = WW8_BordersSO::double9;// 101
            else if (nLineThickness < 166)
                eCodeIdx = WW8_BordersSO::double10;// 131
            else
                eCodeIdx = WW8_BordersSO::double5;// 180
            break;
        case 14:
            if (nLineThickness < 46)
                eCodeIdx = WW8_BordersSO::double0;//  22 Twips for us
            else if (nLineThickness < 76)
                eCodeIdx = WW8_BordersSO::double1;//  60
            else if (nLineThickness < 121)
                eCodeIdx = WW8_BordersSO::double4;//  90
            else if (nLineThickness < 166)
                eCodeIdx = WW8_BordersSO::double2;// 150
            else
                eCodeIdx = WW8_BordersSO::double6;// 180
            break;
        case 15:
        case 16: //Don't have thin thick thin, so use thick thin
            if (nLineThickness < 46)
                eCodeIdx = WW8_BordersSO::double0;//  22 Twips for us
            else if (nLineThickness < 76)
                eCodeIdx = WW8_BordersSO::double1;//  60
            else if (nLineThickness < 121)
                eCodeIdx = WW8_BordersSO::double3;//  90
            else if (nLineThickness < 166)
                eCodeIdx = WW8_BordersSO::double2;// 150
            else
                eCodeIdx = WW8_BordersSO::double5;// 180
            break;
        case 17:
            if (nLineThickness < 46)
                eCodeIdx = WW8_BordersSO::double0;//  22 Twips for us
            else if (nLineThickness < 72)
                eCodeIdx = WW8_BordersSO::double7;//  52
            else if (nLineThickness < 137)
                eCodeIdx = WW8_BordersSO::double4;//  90
            else
                eCodeIdx = WW8_BordersSO::double6;// 180
        break;
        case 18:
        case 19: //Don't have thin thick thin, so use thick thin
            if (nLineThickness < 46)
                eCodeIdx = WW8_BordersSO::double0;//  22 Twips for us
            else if (nLineThickness < 62)
                eCodeIdx = WW8_BordersSO::double7;//  52
            else if (nLineThickness < 87)
                eCodeIdx = WW8_BordersSO::double8;//  71
            else if (nLineThickness < 117)
                eCodeIdx = WW8_BordersSO::double9;// 101
            else if (nLineThickness < 156)
                eCodeIdx = WW8_BordersSO::double10;// 131
            else
                eCodeIdx = WW8_BordersSO::double5;// 180
            break;
        case 20:
            if (nLineThickness < 46)
                eCodeIdx = WW8_BordersSO::single1; //  20 Twips for us
            else
                eCodeIdx = WW8_BordersSO::double1;//  60
            break;
        case 21:
            eCodeIdx = WW8_BordersSO::double1;//  60 Twips for us
            break;
        default:
            eCodeIdx = WW8_BordersSO::single0;
            break;
    }

    const WW8_BordersSO& rBorders = WW8_BordersSO::Get0x01LineMatch(eCodeIdx);
    SvxBorderLine aLine;
    aLine.SetOutWidth(rBorders.mnOut);
    aLine.SetInWidth(rBorders.mnIn);
    aLine.SetDistance(rBorders.mnDist);

    //No AUTO for borders as yet, so if AUTO, use BLACK
    if (nCol == 0)
        nCol = 1;

    aLine.SetColor(SwWW8ImplReader::GetCol(nCol));

    if (pSize)
        pSize[nWWIndex] = nLineThickness+nSpace;

    rBox.SetLine(&aLine, nOOIndex);
    rBox.SetDistance(nSpace, nOOIndex);

}

void Set1Border(bool bVer67, SvxBoxItem &rBox, const WW8_BRC& rBor,
    USHORT nOOIndex, USHORT nWWIndex, short *pSize=0)
{
    BYTE nCol;
    short nSpace, nIdx;
    short nLineThickness = rBor.DetermineBorderProperties(bVer67,&nSpace,&nCol,
        &nIdx);

    GetLineIndex(rBox, nLineThickness, nSpace, nCol, nIdx, nOOIndex, nWWIndex, pSize );

}

bool lcl_IsBorder(bool bVer67, const WW8_BRC* pbrc, bool bChkBtwn = false)
{
    if( bVer67  )
        return ( pbrc[WW8_TOP  ].aBits1[0] & 0x18 ) ||  // brcType  != 0
               ( pbrc[WW8_LEFT ].aBits1[0] & 0x18 ) ||
               ( pbrc[WW8_BOT  ].aBits1[0] & 0x18 ) ||
               ( pbrc[WW8_RIGHT].aBits1[0] & 0x18 ) ||
               ( bChkBtwn && ( pbrc[WW8_BETW ].aBits1[0] )) ||
               //can have dotted and dashed with a brcType of 0
               ( (pbrc[WW8_TOP  ].aBits1[0] & 0x07)+1 > 6) ||
               ( (pbrc[WW8_LEFT ].aBits1[0] & 0x07)+1 > 6) ||
               ( (pbrc[WW8_BOT  ].aBits1[0] & 0x07)+1 > 6) ||
               ( (pbrc[WW8_RIGHT].aBits1[0] & 0x07)+1 > 6) ||
               ( bChkBtwn && ( (pbrc[WW8_BETW ].aBits1[0] & 0x07)+1 > 6))
               ;
                // Abfrage auf 0x1f statt 0x18 ist noetig, da zumindest einige
                // WW-Versionen ( 6.0 US ) bei dotted brcType auf 0 setzen
    else
        return pbrc[WW8_TOP  ].aBits1[1] ||         // brcType  != 0
               pbrc[WW8_LEFT ].aBits1[1] ||
               pbrc[WW8_BOT  ].aBits1[1] ||
               pbrc[WW8_RIGHT].aBits1[1] ||
               (bChkBtwn && pbrc[WW8_BETW ].aBits1[1]);
}

bool SwWW8ImplReader::IsBorder(const WW8_BRC* pbrc, bool bChkBtwn) const
{
    return lcl_IsBorder(bVer67, pbrc, bChkBtwn);
}

bool WW8_BRC::IsEmpty(bool bVer67) const
{
    return (IsBlank() || IsZeroed(bVer67));
}

bool WW8_BRC::IsBlank() const
{
    return (aBits1[0] == 0xff && aBits1[1] == 0xff);
}

bool WW8_BRC::IsZeroed(bool bVer67) const
{
    return (!(bVer67 ? (aBits1[0] & 0x001f) : aBits1[1]));
}

bool SwWW8ImplReader::SetBorder(SvxBoxItem& rBox, const WW8_BRC* pbrc,
    short *pSizeArray, BYTE nSetBorders) const
{
    bool bChange = false;
    static const USHORT aIdArr[ 10 ] =
    {
        WW8_TOP,    BOX_LINE_TOP,
        WW8_LEFT,   BOX_LINE_LEFT,
        WW8_RIGHT,  BOX_LINE_RIGHT,
        WW8_BOT,    BOX_LINE_BOTTOM,
        WW8_BETW,   BOX_LINE_BOTTOM
    };

    for( int i = 0, nEnd = 8; i < nEnd; i += 2 )
    {
        // ungueltige Borders ausfiltern
        const WW8_BRC& rB = pbrc[ aIdArr[ i ] ];
        if( !rB.IsEmpty(bVer67))
        {
            Set1Border(bVer67, rBox, rB, aIdArr[i+1], aIdArr[i], pSizeArray);
            bChange = true;
        }
        else if ( nSetBorders & (1 << aIdArr[i]) )
        {
            /*
            ##826##, ##653##

            If a style has borders set,and the para attributes attempt remove
            the borders, then this is perfectably acceptable, so we shouldn't
            ignore this blank entry

            nSetBorders has a bit set for each location that a sprm set a
            border, so with a sprm set, but no border, then disable the
            appropiate border
            */
            rBox.SetLine( 0, aIdArr[ i+1 ] );
        }
    }
    return bChange;
}


bool SwWW8ImplReader::SetShadow(SvxShadowItem& rShadow, const short *pSizeArray,
    const WW8_BRC *pbrc) const
{
    bool bRet = (
                ( bVer67 ? (pbrc[WW8_RIGHT].aBits1[ 1 ] & 0x20 )
                         : (pbrc[WW8_RIGHT].aBits2[ 1 ] & 0x20 ) )
                && (pSizeArray && pSizeArray[WW8_RIGHT])
                );
    if (bRet)
    {
        rShadow.SetColor(Color(COL_BLACK));
        short nVal = pSizeArray[WW8_RIGHT];
        if (nVal < 0x10)
            nVal = 0x10;
        rShadow.SetWidth(nVal);
        rShadow.SetLocation(SVX_SHADOW_BOTTOMRIGHT);
        bRet = true;
    }
    return bRet;
}

void SwWW8ImplReader::GetBorderDistance(const WW8_BRC* pbrc,
    Rectangle& rInnerDist) const
{
    // 'dptSpace' is stored in 3 bits of 'Border Code (BRC)'
    if (bVer67)
    {
        rInnerDist = Rectangle(((pbrc[ 1 ].aBits1[1] >> 3) & 0x1f) * 20,
                               ((pbrc[ 0 ].aBits1[1] >> 3) & 0x1f) * 20,
                               ((pbrc[ 3 ].aBits1[1] >> 3) & 0x1f) * 20,
                               ((pbrc[ 2 ].aBits1[1] >> 3) & 0x1f) * 20 );
    }
    else
    {
        rInnerDist = Rectangle( (pbrc[ 1 ].aBits2[1]       & 0x1f) * 20,
                                (pbrc[ 0 ].aBits2[1]       & 0x1f) * 20,
                                (pbrc[ 3 ].aBits2[1]       & 0x1f) * 20,
                                (pbrc[ 2 ].aBits2[1]       & 0x1f) * 20 );
    }
}


bool SwWW8ImplReader::SetFlyBordersShadow(SfxItemSet& rFlySet,
    const WW8_BRC *pbrc, short *pSizeArray) const
{
    bool bShadowed = false;
    if (IsBorder(pbrc))
    {
        SvxBoxItem aBox( RES_BOX );
        SetBorder(aBox, pbrc, pSizeArray);

        rFlySet.Put( aBox );

        // fShadow
        SvxShadowItem aShadow( RES_SHADOW );
        if( SetShadow( aShadow, pSizeArray, pbrc ))
        {
            bShadowed = true;
            rFlySet.Put( aShadow );
        }
    }
    return bShadowed;
}

//-----------------------------------------
//              APOs
//-----------------------------------------
                            // fuer Berechnung der minimalen FrameSize
#define MAX_BORDER_SIZE 210         // so breit ist max. der Border
#define MAX_EMPTY_BORDER 10         // fuer +-1-Fehler, mindestens 1

static void FlySecur1(short& rSize, const bool bBorder)
{
    short nMin = MINFLY +
        bBorder ? MAX_BORDER_SIZE : MAX_EMPTY_BORDER;

    if ( rSize < nMin )
        rSize = nMin;
}

inline bool SetValSprm( INT16* pVar, WW8PLCFx_Cp_FKP* pPap, USHORT nId )
{
    const BYTE* pS = pPap->HasSprm( nId );
    if( pS )
        *pVar = (INT16)SVBT16ToShort( pS );
    return ( pS != 0 );
}

inline bool SetValSprm( INT16* pVar, const WW8RStyle* pStyle, USHORT nId )
{
    const BYTE* pS = pStyle->HasParaSprm( nId );
    if( pS )
        *pVar = (INT16)SVBT16ToShort( pS );
    return ( pS != 0 );
}

/*
#i1930 revealed that sprm 0x360D as used in tables can affect the frame
around the table. Its full structure is not fully understood as yet.
*/
void WW8FlyPara::ApplyTabPos(const WW8_TablePos *pTabPos)
{
    if (pTabPos)
    {
        nSp26 = pTabPos->nSp26;
        nSp27 = pTabPos->nSp27;
        nSp29 = pTabPos->nSp29;
        nLeMgn = pTabPos->nLeMgn;
        nRiMgn = pTabPos->nRiMgn;
        nUpMgn = pTabPos->nUpMgn;
        nLoMgn = pTabPos->nLoMgn;
        nSp37 = pTabPos->nSp37;
    }
}

WW8FlyPara::WW8FlyPara(bool bIsVer67, const WW8FlyPara* pSrc /* = 0 */)
{
    if ( pSrc )
        memcpy( this, pSrc, sizeof( WW8FlyPara ) ); // Copy-Ctor
    else
    {
        memset( this, 0, sizeof( WW8FlyPara ) );    // Default-Ctor
        nSp37 = 2;                                  // Default: Umfluss
    }
    bVer67 = bIsVer67;
}

bool WW8FlyPara::operator==(const WW8FlyPara& rSrc) const
{
    /*
     Compare the parts that word seems to compare for equivalence.
     Interestingly being autoheight or absolute height (the & 0x7fff) doesn't
     matter to word e.g. #110507#
    */
    return
       (
         (nSp26 == rSrc.nSp26) &&
         (nSp27 == rSrc.nSp27) &&
         ((nSp45 & 0x7fff) == (rSrc.nSp45 & 0x7fff)) &&
         (nSp28 == rSrc.nSp28) &&
         (nLeMgn == rSrc.nLeMgn) &&
         (nRiMgn == rSrc.nRiMgn) &&
         (nUpMgn == rSrc.nUpMgn) &&
         (nLoMgn == rSrc.nLoMgn) &&
         (nSp29 == rSrc.nSp29) &&
         (nSp37 == rSrc.nSp37)
       );
}

// Read fuer normalen Text
void WW8FlyPara::Read(const BYTE* pSprm29, WW8PLCFx_Cp_FKP* pPap)
{
    if (pSprm29)
        nOrigSp29 = *pSprm29;                           // PPC ( Bindung )

    const BYTE* pS = 0;
    if( bVer67 )
    {
        SetValSprm( &nSp26, pPap, 26 ); // X-Position   //sprmPDxaAbs
        //set in me or in parent style
        mbVertSet |= SetValSprm( &nSp27, pPap, 27 );    // Y-Position   //sprmPDyaAbs
        SetValSprm( &nSp45, pPap, 45 ); // Hoehe        //sprmPWHeightAbs
        SetValSprm( &nSp28, pPap, 28 ); // Breite       //sprmPDxaWidth
        SetValSprm( &nLeMgn, pPap, 49 ); // L-Raender   //sprmPDxaFromText
        SetValSprm( &nRiMgn, pPap, 49 ); // R-Raender   //sprmPDxaFromText
        SetValSprm( &nUpMgn, pPap, 48 ); // U-Raender   //sprmPDyaFromText
        SetValSprm( &nLoMgn, pPap, 48 ); // D-Raender   //sprmPDyaFromText

        pS = pPap->HasSprm( 37 );                       //sprmPWr
        if( pS )
            nSp37 = *pS;
    }
    else
    {
        SetValSprm( &nSp26, pPap, 0x8418 ); // X-Position
        //set in me or in parent style
        mbVertSet |= SetValSprm( &nSp27, pPap, 0x8419 );    // Y-Position
        SetValSprm( &nSp45, pPap, 0x442B ); // Hoehe
        SetValSprm( &nSp28, pPap, 0x841A ); // Breite
        SetValSprm( &nLeMgn, pPap, 0x842F );    // L-Raender
        SetValSprm( &nRiMgn, pPap, 0x842F );    // R-Raender
        SetValSprm( &nUpMgn, pPap, 0x842E );    // U-Raender
        SetValSprm( &nLoMgn, pPap, 0x842E );    // D-Raender

        pS = pPap->HasSprm( 0x2423 );                               // Umfluss
        if( pS )
            nSp37 = *pS;
    }

    if( ::lcl_ReadBorders( bVer67, brc, pPap ))     // Umrandung
        bBorderLines = ::lcl_IsBorder( bVer67, brc );

    /*
     #i8798#
     Appears that with no dyaAbs set then the actual vert anchoring set is
     ignored and we remain relative to text, so if that is the case we are 0
     from para anchor, so we update the frame to have explicitly this type of
     anchoring
    */
    if (!mbVertSet)
        nSp29 = (nOrigSp29 & 0xCF) | 0x20;
    else
        nSp29 = nOrigSp29;
}

void WW8FlyPara::ReadFull(const BYTE* pSprm29, SwWW8ImplReader* pIo)
{
    WW8PLCFMan* pPlcxMan = pIo->pPlcxMan;
    WW8PLCFx_Cp_FKP* pPap = pPlcxMan->GetPapPLCF();

    Read(pSprm29, pPap);    // Lies Apo-Parameter

    do{             // Block zum rausspringen
        if( nSp45 != 0 /* || nSp28 != 0 */ )
            break;                      // bGrafApo nur bei Hoehe automatisch
        if( pIo->pWwFib->fComplex )
            break;                      // (*pPap)++ geht bei FastSave schief
                                        // -> bei FastSave kein Test auf Grafik-APO
        SvStream* pIoStrm = pIo->pStrm;
        ULONG nPos = pIoStrm->Tell();
        WW8PLCFxSave1 aSave;
        pPlcxMan->GetPap()->Save( aSave );
        bGrafApo = false;

        do{             // Block zum rausspringen

            BYTE nTxt[2];

            pIoStrm->Read( nTxt, 2 );                   // lies Text
            if( nTxt[0] != 0x01 || nTxt[1] != 0x0d )// nur Grafik + CR ?
                break;                              // Nein

            (*pPap)++;                              // Naechste Zeile

            // In APO ?
            //sprmPPc
            const BYTE* pS = pPap->HasSprm( bVer67 ? 29 : 0x261B );

            // Nein -> Grafik-Apo
            if( !pS ){
                bGrafApo = true;
                break;                              // Ende des APO
            }

            ww::WordVersion eVer = pIo->GetFib().GetFIBVersion();
            WW8FlyPara *pNowStyleApo=0;
            USHORT nColl = pPap->GetIstd();
            ww::sti eSti = eVer < ww::eWW6 ? ww::GetCanonicalStiFromStc( static_cast< sal_uInt8 >(nColl) ) : static_cast<ww::sti>(nColl);
            while (eSti != ww::stiNil && 0 == (pNowStyleApo = pIo->pCollA[nColl].pWWFly))
            {
                nColl = pIo->pCollA[nColl].nBase;
                eSti = eVer < ww::eWW6 ? ww::GetCanonicalStiFromStc( static_cast< sal_uInt8 >(nColl) ) : static_cast<ww::sti>(nColl);
            }

            WW8FlyPara aF(bVer67, pNowStyleApo);
                                                // Neuer FlaPara zum Vergleich
            aF.Read( pS, pPap );                // WWPara fuer neuen Para
            if( !( aF == *this ) )              // selber APO ? ( oder neuer ? )
                bGrafApo = true;                // nein -> 1-zeiliger APO
                                                //      -> Grafik-APO
        }
        while( 0 );                             // Block zum rausspringen

        pPlcxMan->GetPap()->Restore( aSave );
        pIoStrm->Seek( nPos );
    }while( 0 );                                    // Block zum rausspringen
}


// Read fuer Apo-Defs in Styledefs
void WW8FlyPara::Read(const BYTE* pSprm29, WW8RStyle* pStyle)
{
    if (pSprm29)
        nOrigSp29 = *pSprm29;                           // PPC ( Bindung )

    const BYTE* pS = 0;
    if (bVer67)
    {
        SetValSprm( &nSp26, pStyle, 26 );   // X-Position
        //set in me or in parent style
        mbVertSet |= SetValSprm(&nSp27, pStyle, 27);    // Y-Position
        SetValSprm( &nSp45, pStyle, 45 );   // Hoehe
        SetValSprm( &nSp28, pStyle, 28 );   // Breite
        SetValSprm( &nLeMgn,    pStyle, 49 );   // L-Raender
        SetValSprm( &nRiMgn,    pStyle, 49 );   // R-Raender
        SetValSprm( &nUpMgn,    pStyle, 48 );   // U-Raender
        SetValSprm( &nLoMgn,    pStyle, 48 );   // D-Raender

        pS = pStyle->HasParaSprm( 37 );             // Umfluss
        if( pS )
            nSp37 = *pS;
    }
    else
    {
        SetValSprm( &nSp26, pStyle, 0x8418 );   // X-Position
        //set in me or in parent style
        mbVertSet |= SetValSprm(&nSp27, pStyle, 0x8419);    // Y-Position
        SetValSprm( &nSp45, pStyle, 0x442B );   // Hoehe
        SetValSprm( &nSp28, pStyle, 0x841A );   // Breite
        SetValSprm( &nLeMgn, pStyle, 0x842F );  // L-Raender
        SetValSprm( &nRiMgn, pStyle, 0x842F );  // R-Raender
        SetValSprm( &nUpMgn, pStyle, 0x842E );  // U-Raender
        SetValSprm( &nLoMgn, pStyle, 0x842E );  // D-Raender

        pS = pStyle->HasParaSprm( 0x2423 );             // Umfluss
        if( pS )
            nSp37 = *pS;
    }

    if (::lcl_ReadBorders(bVer67, brc, 0, pStyle))      // Umrandung
        bBorderLines = ::lcl_IsBorder(bVer67, brc);

    /*
     #i8798#
     Appears that with no dyaAbs set then the actual vert anchoring set is
     ignored and we remain relative to text, so if that is the case we are 0
     from para anchor, so we update the frame to have explicitly this type of
     anchoring
    */
    if (!mbVertSet)
        nSp29 = (nOrigSp29 & 0xCF) | 0x20;
    else
        nSp29 = nOrigSp29;
}

bool WW8FlyPara::IsEmpty() const
{
    WW8FlyPara aEmpty(bVer67);
    /*
     wr of 0 like 2 appears to me to be equivalent for checking here. See
     #107103# if wrong, so given that the empty is 2, if we are 0 then set
     empty to 0 to make 0 equiv to 2 for empty checking
    */
    ASSERT(aEmpty.nSp37 == 2, "this is not what we expect for nSp37");
    if (this->nSp37 == 0)
        aEmpty.nSp37 = 0;
    if (aEmpty == *this)
        return true;
    return false;
}

// OD 14.10.2003 #i18732# - changes made on behalf of CMC
WW8SwFlyPara::WW8SwFlyPara( SwPaM& rPaM,
                            SwWW8ImplReader& rIo,
                            WW8FlyPara& rWW,
                            const sal_uInt32 nWWPgTop,
                            const sal_uInt32 nPgLeft,
                            const sal_uInt32 nPgWidth,
                            const INT32 nIniFlyDx,
                            const INT32 nIniFlyDy )
{
    (void) rPaM;
    (void) nPgLeft;

    memset( this, 0, sizeof( WW8SwFlyPara ) );  // Initialisieren
    nNewNettoWidth = MINFLY;                    // Minimum

    eSurround = ( rWW.nSp37 > 1 ) ? SURROUND_IDEAL : SURROUND_NONE;

    /*
     #95905#, #83307# seems to have gone away now, so reenable parallel
     wrapping support for frames in headers/footers. I don't know if we truly
     have an explictly specified behaviour for these circumstances.
    */

    nHeight = rWW.nSp45;
    if( nHeight & 0x8000 )
    {
        nHeight &= 0x7fff;
        eHeightFix = ATT_MIN_SIZE;
    }
    else
        eHeightFix = ATT_FIX_SIZE;

    if( nHeight <= MINFLY )
    {                           // keine Angabe oder Stuss
        eHeightFix = ATT_MIN_SIZE;
        nHeight = MINFLY;
    }

    nWidth = nNettoWidth = rWW.nSp28;
    if( nWidth <= 10 )                              // Auto-Breite
    {
        bAutoWidth = true;
        rIo.maTracer.Log(sw::log::eAutoWidthFrame);
        nWidth = nNettoWidth =
            msword_cast<sal_Int16>((nPgWidth ? nPgWidth : 2268)); // 4 cm
    }
    if( nWidth <= MINFLY )
        nWidth = nNettoWidth = MINFLY;              // Minimale Breite

    eVAlign = text::VertOrientation::NONE;                            // Defaults
    eHAlign = text::HoriOrientation::NONE;
    nYPos = 0;
    nXPos = 0;

    nRiMgn = rWW.nRiMgn;
    nLeMgn = rWW.nLeMgn;
    nLoMgn = rWW.nLoMgn;
    nUpMgn = rWW.nUpMgn;

    /*
    See issue #i9178# for the 9 anchoring options, and make sure they stay
    working if you modify the anchoring logic here.
    */

    // Wenn der Fly links, rechts, oben oder unten aligned ist,
    // wird der aeussere Textabstand ignoriert, da sonst
    // der Fly an falscher Position landen wuerde
    // JP 18.11.98: Problematisch wird es nur bei Innen/Aussen

    // Bindung
    nYBind = (( rWW.nSp29 & 0x30 ) >> 4);
    // --> OD 2005-08-24 #i53725# - absolute positioned objects have to be
    // anchored at-paragraph to assure its correct anchor position.
    eAnchor = FLY_AT_CNTNT;
    // <--
    switch (nYBind)
    {
        case 0:     //relative to margin
            eVRel = text::RelOrientation::PAGE_PRINT_AREA;
            break;
        case 1:     //relative to page
            eVRel = text::RelOrientation::PAGE_FRAME;
            break;
        default:    //relative to text
            eVRel = text::RelOrientation::FRAME;
            break;
    }

// OD 14.10.2003 #i18732#
    switch( rWW.nSp27 )             // besondere Y-Positionen ?
    {
        case -4:
            eVAlign = text::VertOrientation::TOP;
            if (nYBind < 2)
                nUpMgn = 0;
            break;  // oben
        case -8:
            eVAlign = text::VertOrientation::CENTER;
            break;  // zentriert
        case -12:
            eVAlign = text::VertOrientation::BOTTOM;
            if (nYBind < 2)
                nLoMgn = 0;
            break;  // unten
        default:
            nYPos = rWW.nSp27 + (short)nIniFlyDy;
            break;  // Korrekturen per Ini-Datei
    }

    switch( rWW.nSp26 )                 // besondere X-Positionen ?
    {
        case 0:
            eHAlign = text::HoriOrientation::LEFT;
            nLeMgn = 0;
            break;  // links
        case -4:
            eHAlign = text::HoriOrientation::CENTER;
            break;  // zentriert
        case -8:
            eHAlign = text::HoriOrientation::RIGHT;
            nRiMgn = 0;
            break;  // rechts
        case -12:
            eHAlign = text::HoriOrientation::LEFT;
            bToggelPos = true;
            break;  // innen
        case -16:
            eHAlign = text::HoriOrientation::RIGHT;
            bToggelPos = true;
            break;  // aussen
        default:
            nXPos = rWW.nSp26 + (short)nIniFlyDx;
            break;  // Korrekturen per Ini-Datei
    }

    nXBind = ( rWW.nSp29 & 0xc0 ) >> 6;
// OD 14.10.2003 #i18732#
    switch (nXBind)           // X - Bindung -> Koordinatentransformation
    {
        case 0:     //relative to column
            eHRel = text::RelOrientation::FRAME;
            break;
        case 1:     //relative to margin
            eHRel = text::RelOrientation::PAGE_PRINT_AREA;
            break;
        default:    //relative to page
            eHRel = text::RelOrientation::PAGE_FRAME;
            break;
    }

    // --> OD 2004-12-06 #i36649# - adjustments for certain horizontal alignments
    // Note: These special adjustments found by an investigation of documents
    //       containing frames with different left/right border distances and
    //       distances to text. The outcome is some how strange.
    // Note: These adjustments causes wrong horizontal positions for frames,
    //       which are aligned inside|outside to page|margin on even pages,
    //       the left and right border distances are different.
    // --> OD 2005-01-19 #119176# - no adjustments possible, if frame has
    // automatic width.
    // determine left border distance
    INT16 nLeBorderMgn( 0L );
    if ( !bAutoWidth )
    {
        INT16 nTemp = rWW.brc[WW8_LEFT].DetermineBorderProperties(rWW.bVer67,
            &nLeBorderMgn);
        nLeBorderMgn = nLeBorderMgn + nTemp;
    }
    // determine right border distance
    INT16 nRiBorderMgn( 0L );
    if ( !bAutoWidth )
    {
        INT16 nTemp = rWW.brc[WW8_RIGHT].DetermineBorderProperties(rWW.bVer67,
            &nRiBorderMgn);
        nRiBorderMgn = nRiBorderMgn + nTemp;
    }
    if ( !bAutoWidth && eHAlign == text::HoriOrientation::LEFT && eHRel == text::RelOrientation::PAGE_FRAME )
    {
        // convert 'left to page' to
        // 'from left -<width>-<2*left border distance>-<right wrap distance>
        // to page text area'
        eHAlign = text::HoriOrientation::NONE;
        eHRel = text::RelOrientation::PAGE_PRINT_AREA;
        nXPos = -nWidth - (2*nLeBorderMgn) - rWW.nRiMgn;
        // re-set left wrap distance
        nLeMgn = rWW.nLeMgn;
    }
    else if ( !bAutoWidth && eHAlign == text::HoriOrientation::RIGHT && eHRel == text::RelOrientation::PAGE_FRAME )
    {
        // convert 'right to page' to
        // 'from left <right border distance-left border distance>+<left wrap distance>
        // to right page border'
        eHAlign = text::HoriOrientation::NONE;
        eHRel = text::RelOrientation::PAGE_RIGHT;
        nXPos = ( nRiBorderMgn - nLeBorderMgn ) + rWW.nLeMgn;
        // re-set right wrap distance
        nRiMgn = rWW.nRiMgn;
    }
    else if ( !bAutoWidth && eHAlign == text::HoriOrientation::LEFT && eHRel == text::RelOrientation::PAGE_PRINT_AREA )
    {
        // convert 'left to margin' to
        // 'from left -<left border distance> to page text area'
        eHAlign = text::HoriOrientation::NONE;
        eHRel = text::RelOrientation::PAGE_PRINT_AREA;
        nXPos = -nLeBorderMgn;
        // re-set left wrap distance
        nLeMgn = rWW.nLeMgn;
    }
    else if ( !bAutoWidth && eHAlign == text::HoriOrientation::RIGHT && eHRel == text::RelOrientation::PAGE_PRINT_AREA )
    {
        // convert 'right to margin' to
        // 'from left -<width>-<left border distance> to right page border'
        eHAlign = text::HoriOrientation::NONE;
        eHRel = text::RelOrientation::PAGE_RIGHT;
        nXPos = -nWidth - nLeBorderMgn;
        // re-set right wrap distance
        nRiMgn = rWW.nRiMgn;
    }
    else if (rWW.bBorderLines)
    // <--
    {
        /*
        #i582#
        Word has a curious bug where the offset stored do not take into
        account the internal distance from the corner both
        */
        INT16 nLeLMgn = 0;
        INT16 nTemp = rWW.brc[WW8_LEFT].DetermineBorderProperties(rWW.bVer67,
            &nLeLMgn);
        nLeLMgn = nLeLMgn + nTemp;

        if (nLeLMgn)
        {
            if (eHAlign == text::HoriOrientation::LEFT)
                eHAlign = text::HoriOrientation::NONE;
            nXPos = nXPos - nLeLMgn;
        }
    }

    // --> OD 2007-07-03 #148498#
    // adjustments for certain vertical alignments
    if ( eVAlign == text::VertOrientation::NONE && eVRel == text::RelOrientation::PAGE_PRINT_AREA )
    {
        // convert "<X> from top page text area" to
        // "<X + page top margin> from page"
        eVRel = text::RelOrientation::PAGE_FRAME;
        nYPos = static_cast< INT16 >( nYPos + nWWPgTop );
    }
    // <--

    FlySecur1( nWidth, rWW.bBorderLines );          // passen Raender ?
    FlySecur1( nHeight, rWW.bBorderLines );

}

// hat ein Fly in WW eine automatische Breite, dann muss das durch
// nachtraegliches Anpassen der ( im SW festen ) Fly-Breite simuliert werden.
// Dabei kann die Fly-Breite groesser oder kleiner werden, da der Default-Wert
// ohne Wissen ueber den Inhalt eingesetzt wird.
void WW8SwFlyPara::BoxUpWidth( long nInWidth )
{
    if( bAutoWidth && nInWidth > nNewNettoWidth )
        nNewNettoWidth = nInWidth;
};

// Die Klasse WW8FlySet ist von SfxItemSet abgeleitet und stellt auch
// im Prizip nicht mehr zur Verfuegung, ist aber fuer mich besser
// zu handeln
// WW8FlySet-ctor fuer Apos und Graf-Apos
WW8FlySet::WW8FlySet(SwWW8ImplReader& rReader, const WW8FlyPara* pFW,
    const WW8SwFlyPara* pFS, bool bGraf)
    : SfxItemSet(rReader.rDoc.GetAttrPool(),RES_FRMATR_BEGIN,RES_FRMATR_END-1)
{
    if (!rReader.mbNewDoc)
        Reader::ResetFrmFmtAttrs(*this);    // Abstand/Umrandung raus
                                            // Position
    Put(SvxFrameDirectionItem(FRMDIR_HORI_LEFT_TOP, RES_FRAMEDIR));

/*Below can all go when we have from left in rtl mode*/
    SwTwips nXPos = pFS->nXPos;
    sal_Int16 eHRel = pFS->eHRel;
    rReader.MiserableRTLGraphicsHack(nXPos, pFS->nWidth, pFS->eHAlign, eHRel);
/*Above can all go when we have from left in rtl mode*/
    Put( SwFmtHoriOrient(nXPos, pFS->eHAlign, pFS->eHRel, pFS->bToggelPos ));
    Put( SwFmtVertOrient( pFS->nYPos, pFS->eVAlign, pFS->eVRel ) );

    if (pFS->nLeMgn || pFS->nRiMgn)     // Raender setzen
        Put(SvxLRSpaceItem(pFS->nLeMgn, pFS->nRiMgn, 0, 0, RES_LR_SPACE));

    if (pFS->nUpMgn || pFS->nLoMgn)
        Put(SvxULSpaceItem(pFS->nUpMgn, pFS->nLoMgn, RES_UL_SPACE));

    //we no longer need to hack around the header/footer problems
    Put(SwFmtSurround(pFS->eSurround));

    short aSizeArray[5]={0};
    rReader.SetFlyBordersShadow(*this,(const WW8_BRC*)pFW->brc,&aSizeArray[0]);

    // der 5. Parameter ist immer 0, daher geht beim Cast nix verloren

    // OD 2004-05-18 #i27767#
    // --> OD 2004-10-18 #i35017# - constant name has changed
    Put( SwFmtWrapInfluenceOnObjPos(
                text::WrapInfluenceOnPosition::ONCE_SUCCESSIVE ) );
    // <--

    if( !bGraf )
    {
        Put( SwFmtAnchor(pFS->eAnchor) );
        // Groesse einstellen

        //Ordinarily with frames, the border width and spacing is
        //placed outside the frame, making it larger. With these
        //types of frames, the left right thickness and space makes
        //it wider, but the top bottom spacing and border thickness
        //is placed inside.
        Put( SwFmtFrmSize( pFS->eHeightFix, pFS->nWidth +
            aSizeArray[WW8_LEFT] + aSizeArray[WW8_RIGHT],
            pFS->nHeight));
    }
}

// WW8FlySet-ctor fuer zeichengebundene Grafiken
WW8FlySet::WW8FlySet( SwWW8ImplReader& rReader, const SwPaM* pPaM,
    const WW8_PIC& rPic, long nWidth, long nHeight )
    : SfxItemSet(rReader.rDoc.GetAttrPool(),RES_FRMATR_BEGIN,RES_FRMATR_END-1)
{
    Init(rReader, pPaM);

    Put(SvxFrameDirectionItem(FRMDIR_HORI_LEFT_TOP, RES_FRAMEDIR));

    short aSizeArray[5]={0};
    /*
    If we have set borders then in word the graphic is displaced from the left
    and top the width of the borders of those sides, and then the shadow
    itself is drawn to the bottom and right of the displaced graphic.  In word
    the total size is that of the graphic plus the borders, plus the total
    shadow around all edges, for this translation the top and left shadow
    region is translated spacing around the graphic to those sides, and the
    bottom and right shadow size is added to the graphic size.
    */
    if (rReader.SetFlyBordersShadow( *this, rPic.rgbrc, &aSizeArray[0]))
    {
        Put(SvxLRSpaceItem( aSizeArray[WW8_LEFT], 0, 0, 0, RES_LR_SPACE ) );
        Put(SvxULSpaceItem( aSizeArray[WW8_TOP], 0, RES_UL_SPACE ));
        aSizeArray[WW8_RIGHT]*=2;
        aSizeArray[WW8_BOT]*=2;
    }

    Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth+aSizeArray[WW8_LEFT]+
        aSizeArray[WW8_RIGHT], nHeight+aSizeArray[WW8_TOP]
        + aSizeArray[WW8_BOT]) );
}

WW8FlySet::WW8FlySet(const SwWW8ImplReader& rReader, const SwPaM* pPaM)
    : SfxItemSet(rReader.rDoc.GetAttrPool(),RES_FRMATR_BEGIN,RES_FRMATR_END-1)
{
    Init(rReader, pPaM);
}

void WW8FlySet::Init(const SwWW8ImplReader& rReader, const SwPaM* pPaM)
{
    if (!rReader.mbNewDoc)
        Reader::ResetFrmFmtAttrs(*this);  // Abstand/Umrandung raus

    Put(SvxLRSpaceItem(RES_LR_SPACE)); //inline writer ole2 objects start with 0.2cm l/r
    SwFmtAnchor aAnchor(FLY_IN_CNTNT);

    aAnchor.SetAnchor(pPaM->GetPoint());
    Put(aAnchor);

    //The horizontal default is on the baseline, the vertical is centered
    //around the character center it appears
    if (rReader.maSectionManager.CurrentSectionIsVertical())
        Put(SwFmtVertOrient(0, text::VertOrientation::CHAR_CENTER,text::RelOrientation::CHAR));
    else
        Put(SwFmtVertOrient(0, text::VertOrientation::TOP, text::RelOrientation::FRAME));
}

WW8DupProperties::WW8DupProperties(SwDoc &rDoc, SwWW8FltControlStack *pStk)
    : pCtrlStck(pStk),
    aChrSet(rDoc.GetAttrPool(), RES_CHRATR_BEGIN, RES_CHRATR_END - 1 ),
    aParSet(rDoc.GetAttrPool(), RES_PARATR_BEGIN, RES_PARATR_END - 1 )
{
    //Close any open character properties and duplicate them inside the
    //first table cell
    USHORT nCnt = static_cast< USHORT >(pCtrlStck->Count());
    for (USHORT i=0; i < nCnt; i++)
    {
        const SwFltStackEntry* pEntry = (*pCtrlStck)[ i ];
        if(pEntry->bLocked)
        {
            if (isCHRATR(pEntry->pAttr->Which()))
            {
                aChrSet.Put( *pEntry->pAttr );

            }
            else if (isPARATR(pEntry->pAttr->Which()))
            {
                aParSet.Put( *pEntry->pAttr );
            }
        }
    }
}

void WW8DupProperties::Insert(const SwPosition &rPos)
{
    const SfxItemSet *pSet=&aChrSet;
    for(int i=0;i<2;i++)
    {
        if (i==1)
            pSet = &aParSet;

        if( pSet->Count() )
        {
            SfxItemIter aIter( *pSet );
            const SfxPoolItem* pItem = aIter.GetCurItem();
            do
            {
                pCtrlStck->NewAttr(rPos, *pItem);
            }while( !aIter.IsAtEnd() && 0 != ( pItem = aIter.NextItem() ) );
        }
    }
}

void SwWW8ImplReader::MoveInsideFly(const SwFrmFmt *pFlyFmt)
{
    WW8DupProperties aDup(rDoc,pCtrlStck);

    pCtrlStck->SetAttr(*pPaM->GetPoint(), 0, false);

    // Setze Pam in den FlyFrame
    const SwFmtCntnt& rCntnt = pFlyFmt->GetCntnt();
    ASSERT( rCntnt.GetCntntIdx(), "Kein Inhalt vorbereitet." );
    pPaM->GetPoint()->nNode = rCntnt.GetCntntIdx()->GetIndex() + 1;
    pPaM->GetPoint()->nContent.Assign( pPaM->GetCntntNode(), 0 );

    aDup.Insert(*pPaM->GetPoint());
}

SwTwips SwWW8ImplReader::MoveOutsideFly(SwFrmFmt *pFlyFmt,
    const SwPosition &rPos, bool bTableJoin)
{
    SwTwips nRetWidth = 0;
    // Alle Attribute schliessen, da sonst Attribute entstehen koennen,
    // die aus Flys rausragen
    WW8DupProperties aDup(rDoc,pCtrlStck);
    pCtrlStck->SetAttr(*pPaM->GetPoint(), 0, false);

    /*
    #i1291
    If this fly frame consists entirely of one table inside a frame
    followed by an empty paragraph then we want to delete the empty
    paragraph so as to get the frame to autoshrink to the size of the
    table to emulate words behaviour closer.
    */
    if (bTableJoin)
    {
        const SwNodeIndex* pNodeIndex = pFlyFmt->GetCntnt().
            GetCntntIdx();
        if (pNodeIndex)
        {
            SwNodeIndex aIdx( *pNodeIndex, 1 ),
            aEnd( *pNodeIndex->GetNode().EndOfSectionNode() );

            if (aIdx < aEnd)
            {
                if(aIdx.GetNode().IsTableNode())
                {
                    SwTableNode *pTable = aIdx.GetNode().GetTableNode();
                    aIdx = *aIdx.GetNode().EndOfSectionNode();
                    aIdx++;
                    if ( (aIdx < aEnd) && aIdx.GetNode().IsTxtNode() )
                    {
                        SwTxtNode *pNd = aIdx.GetNode().GetTxtNode();
                        aIdx++;
                        if (aIdx == aEnd && pNd && !pNd->GetTxt().Len())
                        {
                            rDoc.DelFullPara( *pPaM );

                            SwTable& rTable = pTable->GetTable();
                            SwFrmFmt* pTblFmt = rTable.GetFrmFmt();

                            if (pTblFmt)
                            {
                                SwFmtFrmSize aSize = pTblFmt->GetFrmSize();
                                aSize.SetHeightSizeType(ATT_MIN_SIZE);
                                aSize.SetHeight(MINLAY);
                                pFlyFmt->SetFmtAttr(aSize);
                                pTblFmt->SetFmtAttr(SwFmtHoriOrient(0,text::HoriOrientation::FULL));
                                nRetWidth = aSize.GetWidth();
                            }
                        }
                    }
                }
            }
        }
    }

    *pPaM->GetPoint() = rPos;
    aDup.Insert(*pPaM->GetPoint());
    return nRetWidth;
}

WW8FlyPara *SwWW8ImplReader::ConstructApo(const ApoTestResults &rApo,
    const WW8_TablePos *pTabPos)
{
    WW8FlyPara *pRet = 0;
    ASSERT(rApo.HasFrame() || pTabPos,
        "If no frame found, *MUST* be in a table");

    pRet = new WW8FlyPara(bVer67, rApo.mpStyleApo);

    // APO-Parameter ermitteln und Test auf bGrafApo
    if (rApo.HasFrame())
        pRet->ReadFull(rApo.mpSprm29, this);

    pRet->ApplyTabPos(pTabPos);

    if (pRet->IsEmpty())
        delete pRet, pRet = 0;
    return pRet;
}

bool SwWW8ImplReader::IsDropCap()
{
    // Find the DCS (Drop Cap Specifier) for the paragraph
    // if does not exist or if the first three bits are 0
    // then there is no dropcap on the paragraph
    WW8PLCFx_Cp_FKP *pPap = pPlcxMan ? pPlcxMan->GetPapPLCF() : 0;
    if (pPap)
    {
        const BYTE *pDCS;
        if (bVer67)
            pDCS = pPap->HasSprm(46);
        else
            pDCS = pPlcxMan->GetPapPLCF()->HasSprm(0x442C);
        if(pDCS)
        {
            short nDCS = SVBT16ToShort( pDCS );
            if((nDCS | 7) != 0)
                return true;
        }
    }
    return false;
}

bool SwWW8ImplReader::StartApo(const ApoTestResults &rApo,
    const WW8_TablePos *pTabPos)
{
    if (0 == (pWFlyPara = ConstructApo(rApo, pTabPos)))
        return false;

    // --> OD 2007-07-03 #148498#
    // <WW8SwFlyPara> constructor has changed - new 4th parameter
    // containing WW8 page top margin.
    pSFlyPara = new WW8SwFlyPara( *pPaM, *this, *pWFlyPara,
                                  maSectionManager.GetWWPageTopMargin(),
                                  maSectionManager.GetPageLeft(),
                                  maSectionManager.GetTextAreaWidth(),
                                  nIniFlyDx, nIniFlyDy);
    // <--

    // If this paragraph is a Dropcap set the flag and we will deal with it later
    if (IsDropCap())
    {
        bDropCap = true;
        pAktItemSet = new SfxItemSet( rDoc.GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_END - 1 );
        return false;
    }

    if( !pWFlyPara->bGrafApo )
    {

        // Innerhalb des GrafApo muessen Textattribute ignoriert werden, da
        // sie sonst auf den folgenden Zeilen landen.  Der Rahmen wird nur
        // eingefuegt, wenn er *nicht* nur zum Positionieren einer einzelnen
        // Grafik dient.  Ist es ein Grafik-Rahmen, dann werden pWFlyPara und
        // pSFlyPara behalten und die
        // daraus resultierenden Attribute beim Einfuegen der Grafik auf die
        // Grafik angewendet.

        WW8FlySet aFlySet(*this, pWFlyPara, pSFlyPara, false);

        pSFlyPara->pFlyFmt = rDoc.MakeFlySection( pSFlyPara->eAnchor,
            pPaM->GetPoint(), &aFlySet );
        ASSERT(pSFlyPara->pFlyFmt->GetAnchor().GetAnchorId() ==
            pSFlyPara->eAnchor, "Not the anchor type requested!");

        if (pSFlyPara->pFlyFmt)
        {
            if (!pDrawModel)
                GrafikCtor();

            SdrObject* pOurNewObject = CreateContactObject(pSFlyPara->pFlyFmt);
            pWWZOrder->InsertTextLayerObject(pOurNewObject);
        }

        if (FLY_IN_CNTNT != pSFlyPara->eAnchor)
            pAnchorStck->AddAnchor(*pPaM->GetPoint(),pSFlyPara->pFlyFmt);

        // merke Pos im Haupttext
        pSFlyPara->pMainTextPos = new SwPosition( *pPaM->GetPoint() );

        //remove fltanchors, otherwise they will be closed inside the
        //frame, which makes no sense, restore them after the frame is
        //closed
        pSFlyPara->pOldAnchorStck = pAnchorStck;
        pAnchorStck = new SwWW8FltAnchorStack(&rDoc, nFieldFlags);

        MoveInsideFly(pSFlyPara->pFlyFmt);

        // 1) ReadText() wird nicht wie beim W4W-Reader rekursiv aufgerufen,
        //    da die Laenge des Apo zu diesen Zeitpunkt noch nicht feststeht,
        //    ReadText() diese Angabe aber braucht.
        // 2) Der CtrlStck wird nicht neu erzeugt.
        //    die Char-Attribute laufen weiter ( AErger mit SW-Attributen )
        //    Paraattribute muessten am Ende jeden Absatzes zurueckgesetzt
        //    sein, d.h. es duerften am Absatzende keine Paraattribute
        //    auf dem Stack liegen
    }
    return true;
}

void wwSectionManager::JoinNode(const SwPosition &rPos, const SwNode &rNode)
{
    if ((!maSegments.empty()) && (maSegments.back().maStart == rPos.nNode))
        maSegments.back().maStart = SwNodeIndex(rNode);
}

bool SwWW8ImplReader::JoinNode(SwPaM &rPam, bool bStealAttr)
{
    bool bRet = false;
    rPam.GetPoint()->nContent = 0;          // an den Anfang der Zeile gehen

    SwNodeIndex aPref(rPam.GetPoint()->nNode, -1);

    if (SwTxtNode* pNode = aPref.GetNode().GetTxtNode())
    {
        maSectionManager.JoinNode(*rPam.GetPoint(), aPref.GetNode());
        rPam.GetPoint()->nNode = aPref;
        rPam.GetPoint()->nContent.Assign(pNode, pNode->GetTxt().Len());
        if (bStealAttr)
            pCtrlStck->StealAttr(rPam.GetPoint());

        pNode->JoinNext();

        bRet = true;
    }
    return bRet;
}

void SwWW8ImplReader::StopApo()
{
    ASSERT(pWFlyPara, "no pWFlyPara to close");
    if (!pWFlyPara)
        return;
    if (pWFlyPara->bGrafApo)
    {
        // Grafik-Rahmen, der *nicht* eingefuegt wurde leeren Absatz incl.
        // Attributen entfernen
        JoinNode(*pPaM, true);

    }
    else
    {
        if (!pSFlyPara->pMainTextPos || !pWFlyPara)
        {
            ASSERT( pSFlyPara->pMainTextPos, "StopApo: pMainTextPos ist 0" );
            ASSERT( pWFlyPara, "StopApo: pWFlyPara ist 0" );
            return;
        }

        /*
        #104920#
        What we are doing with this temporary nodeindex is as follows: The
        stack of attributes normally only places them into the document when
        the current insertion point has passed them by. Otherwise the end
        point of the attribute gets pushed along with the insertion point. The
        insertion point is moved and the properties commited during
        MoveOutsideFly. We also may want to remove the final paragraph in the
        frame, but we need to wait until the properties for that frame text
        have been commited otherwise they will be lost. So we first get a
        handle to the last the filter inserted. After the attributes are
        commited, if that paragraph exists we join it with the para after it
        that comes with the frame by default so that as normal we don't end up
        with one more paragraph than we wanted.
        */
        SwNodeIndex aPref(pPaM->GetPoint()->nNode, -1);

        SwTwips nNewWidth =
            MoveOutsideFly(pSFlyPara->pFlyFmt, *pSFlyPara->pMainTextPos);
        if (nNewWidth)
            pSFlyPara->BoxUpWidth(nNewWidth);

        Color aBg(0xFE, 0xFF, 0xFF, 0xFF);  //Transparent by default

        if (SwTxtNode* pNd = aPref.GetNode().GetTxtNode())
        {
            /*
            #i582#/#114238#
            Take the last paragraph background colour and fill the frame with
            it.  Otherwise, make it transparent, this appears to be how MSWord
            works
            */
            const SfxPoolItem &rItm = pNd->SwCntntNode::GetAttr(RES_BACKGROUND);
            const SvxBrushItem &rBrush = (const SvxBrushItem&)(rItm);
            if (rBrush.GetColor().GetColor() != COL_AUTO)
                aBg = rBrush.GetColor();

            //Get rid of extra empty paragraph
            pNd->JoinNext();
        }

        pSFlyPara->pFlyFmt->SetFmtAttr(SvxBrushItem(aBg, RES_BACKGROUND));

        DeleteAnchorStk();
        pAnchorStck = pSFlyPara->pOldAnchorStck;

        // Ist die Fly-Breite durch eine innenliegende Grafik vergroessert
        // worden ( bei automatischer Breite des Flys ), dann muss die Breite
        // des SW-Flys entsprechend umgesetzt werden, da der SW keine
        // automatische Breite kennt.
        if( pSFlyPara->nNewNettoWidth > MINFLY )    // BoxUpWidth ?
        {
            long nW = pSFlyPara->nNewNettoWidth;
            nW += pSFlyPara->nWidth - pSFlyPara->nNettoWidth;   // Rand dazu
            pSFlyPara->pFlyFmt->SetFmtAttr(
                SwFmtFrmSize( pSFlyPara->eHeightFix, nW, pSFlyPara->nHeight ) );
        }
        /*
        #83307# Word set *no* width meaning its an automatic width. The
        SwFlyPara reader will have already set a fallback width of the
        printable regions width, so we should reuse it. Despite the related
        problems with layout addressed with a hack in WW8FlyPara's constructor
        #i27204# Added AutoWidth setting. Left the old CalculateFlySize in place
        so that if the user unselects autowidth, the width doesn't max out
        */
        else if( !pWFlyPara->nSp28 )
        {
            using namespace sw::util;
            SfxItemSet aFlySet( pSFlyPara->pFlyFmt->GetAttrSet() );

            SwFmtFrmSize aSize(ItemGet<SwFmtFrmSize>(aFlySet, RES_FRM_SIZE));

            aFlySet.ClearItem(RES_FRM_SIZE);

            CalculateFlySize(aFlySet, pSFlyPara->pMainTextPos->nNode,
                pSFlyPara->nWidth);

            nNewWidth = ItemGet<SwFmtFrmSize>(aFlySet, RES_FRM_SIZE).GetWidth();

            aSize.SetWidth(nNewWidth);
            aSize.SetWidthSizeType(ATT_VAR_SIZE);

            pSFlyPara->pFlyFmt->SetFmtAttr(aSize);
        }

        delete pSFlyPara->pMainTextPos, pSFlyPara->pMainTextPos = 0;

// Damit die Frames bei Einfuegen in existierendes Doc erzeugt werden,
// wird in fltshell.cxx beim Setzen des FltAnchor-Attributes
// pFlyFrm->MakeFrms() gerufen

    }

    //#i8062#
    if (pSFlyPara && pSFlyPara->pFlyFmt)
        pFmtOfJustInsertedApo = pSFlyPara->pFlyFmt;

    DELETEZ( pSFlyPara );
    DELETEZ( pWFlyPara );
}

// TestSameApo() beantwortet die Frage, ob es dasselbe APO oder ein neues ist
bool SwWW8ImplReader::TestSameApo(const ApoTestResults &rApo,
    const WW8_TablePos *pTabPos)
{
    if( !pWFlyPara )
    {
        ASSERT( pWFlyPara, " Wo ist mein pWFlyPara ? " );
        return true;
    }

    // Es muss ein kompletter Vergleich ( ausser Borders ) stattfinden, um
    // alle Kombinationen Style / Hart richtig einzuordnen. Deshalb wird ein
    // temporaerer WW8FlyPara angelegt ( abh. ob Style oder nicht ), darauf
    // die harten Attrs angewendet, und dann verglichen

    // Zum Vergleich
    WW8FlyPara aF(bVer67, rApo.mpStyleApo);
    // WWPara fuer akt. Para
    if (rApo.HasFrame())
        aF.Read(rApo.mpSprm29, pPlcxMan->GetPapPLCF());
    aF.ApplyTabPos(pTabPos);

    return aF == *pWFlyPara;
}

/***************************************************************************
#       Attribut - Verwaltung
#**************************************************************************/

void SwWW8ImplReader::NewAttr( const SfxPoolItem& rAttr )
{
    if( !bNoAttrImport ) // zum Ignorieren von Styles beim Doc-Einfuegen
    {
        if (pAktColl)
        {
            ASSERT(rAttr.Which() != RES_FLTR_REDLINE, "redline in style!");
            pAktColl->SetFmtAttr(rAttr);
        }
        else if (pAktItemSet)
            pAktItemSet->Put(rAttr);
        else if (rAttr.Which() == RES_FLTR_REDLINE)
            mpRedlineStack->open(*pPaM->GetPoint(), rAttr);
        else
            pCtrlStck->NewAttr(*pPaM->GetPoint(), rAttr);

        if (mpPostProcessAttrsInfo && mpPostProcessAttrsInfo->mbCopy)
            mpPostProcessAttrsInfo->mItemSet.Put(rAttr);
    }
}

// holt Attribut aus der FmtColl / Stack / Doc
const SfxPoolItem* SwWW8ImplReader::GetFmtAttr( USHORT nWhich )
{
    const SfxPoolItem* pRet = 0;
    if (pAktColl)
        pRet = &(pAktColl->GetFmtAttr(nWhich));
    else if (pAktItemSet)
    {
        pRet = pAktItemSet->GetItem(nWhich);
        if (!pRet)
            pRet = pStandardFmtColl ? &(pStandardFmtColl->GetFmtAttr(nWhich)) : 0;
        if (!pRet)
            pRet = &rDoc.GetAttrPool().GetDefaultItem(nWhich);
    }
    else if (pPlcxMan && pPlcxMan->GetDoingDrawTextBox())
    {
        pRet = pCtrlStck->GetStackAttr(*pPaM->GetPoint(), nWhich);
        if (!pRet)
        {
            if (nAktColl < nColls && pCollA[nAktColl].pFmt &&
                pCollA[nAktColl].bColl)
            {
                pRet = &(pCollA[nAktColl].pFmt->GetFmtAttr(nWhich));
            }
        }
        if (!pRet)
            pRet = pStandardFmtColl ? &(pStandardFmtColl->GetFmtAttr(nWhich)) : 0;
        if (!pRet)
            pRet = &rDoc.GetAttrPool().GetDefaultItem(nWhich);
    }
    else
        pRet = pCtrlStck->GetFmtAttr(*pPaM->GetPoint(), nWhich);
    return pRet;
}

/***************************************************************************
#       eigentliche Attribute
#
# Die Methoden erhalten die Token-Id und die Laenge der noch folgenden
# Parameter gemaess Tabelle in WWScan.cxx als Parameter
#**************************************************************************/

/***************************************************************************
#  Spezial WW - Attribute
#**************************************************************************/

void SwWW8ImplReader::Read_Special(USHORT, const BYTE* pData, short nLen)
{
    if( nLen < 0 )
    {
        bSpec = false;
        return;
    }
    bSpec = ( *pData != 0 );
}

// Read_Obj wird fuer fObj und fuer fOle2 benutzt !
void SwWW8ImplReader::Read_Obj(USHORT , const BYTE* pData, short nLen)
{
    if( nLen < 0 )
        bObj = false;
    else
    {
        bObj = 0 != *pData;

        if( bObj && nPicLocFc && bEmbeddObj )
            nObjLocFc = nPicLocFc;
    }
}

void SwWW8ImplReader::Read_PicLoc(USHORT , const BYTE* pData, short nLen )
{
    if( nLen < 0 )
    {
        nPicLocFc = 0;
        bSpec = false;  // Stimmt das immer ?
    }
    else
    {
        nPicLocFc = SVBT32ToUInt32( pData );
        bSpec = true;

        if( bObj && nPicLocFc && bEmbeddObj )
            nObjLocFc = nPicLocFc;
    }
}

void SwWW8ImplReader::Read_POutLvl(USHORT, const BYTE* pData, short nLen )
{
    if (pAktColl && (0 < nLen))
    {
        if (SwWW8StyInf* pSI = GetStyle(nAktColl))
        {
            pSI->nOutlineLevel = static_cast< BYTE >(
                ( (1 <= pSI->GetWWStyleId()) && (9 >= pSI->GetWWStyleId()) )
            ? pSI->GetWWStyleId()-1
            : (pData ? *pData : 0) );
        }
    }
}

void SwWW8ImplReader::Read_Symbol(USHORT, const BYTE* pData, short nLen )
{
    if( !bIgnoreText )
    {
        if( nLen < 0 )
        {
            //otherwise disable after we print the char
            if (pPlcxMan && pPlcxMan->GetDoingDrawTextBox())
                pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_FONT );
            bSymbol = false;
        }
        else
        {
            // Make new Font-Atribut
            // (will be closed in SwWW8ImplReader::ReadChars() )

            //Will not be added to the charencoding stack, for styles the real
            //font setting will be put in as the styles charset, and for plain
            //text encoding for symbols is moot. Drawing boxes will check bSymbol
            //themselves so they don't need to add it to the stack either.
            if (SetNewFontAttr(SVBT16ToShort( pData ), false, RES_CHRATR_FONT))
            {
                if( bVer67 )
                {
                    cSymbol = ByteString::ConvertToUnicode(
                        *(sal_Char*)(pData+2), RTL_TEXTENCODING_MS_1252 );
                }
                else
                    cSymbol = SVBT16ToShort( pData+2 );
                bSymbol = true;
            }
        }
    }
}

SwWW8StyInf *SwWW8ImplReader::GetStyle(USHORT nColl) const
{
    return nColl < nColls ? &pCollA[nColl] : 0;
}

/***************************************************************************
#  Zeichen - Attribute
#**************************************************************************/

// Read_BoldUsw fuer Italic, Bold, Kapitaelchen, Versalien, durchgestrichen,
// Contour und Shadow
void SwWW8ImplReader::Read_BoldUsw( USHORT nId, const BYTE* pData, short nLen )
{
    const int nContigiousWestern = 8;
    const int nWestern = nContigiousWestern + 1;
    const int nEastern = 2;
    const int nIds = nWestern + nEastern;
    static const USHORT nEndIds[ nIds ] =
    {
        RES_CHRATR_WEIGHT,          RES_CHRATR_POSTURE,
        RES_CHRATR_CROSSEDOUT,      RES_CHRATR_CONTOUR,
        RES_CHRATR_SHADOWED,        RES_CHRATR_CASEMAP,
        RES_CHRATR_CASEMAP,         RES_CHRATR_HIDDEN,

        RES_CHRATR_CROSSEDOUT,

        RES_CHRATR_CJK_WEIGHT,      RES_CHRATR_CJK_POSTURE
    };

    ww::WordVersion eVersion = pWwFib->GetFIBVersion();

    BYTE nI;
    // die Attribut-Nr fuer "doppelt durchgestrichen" tanzt aus der Reihe
    if (0x2A53 == nId)
        nI = nContigiousWestern;               // The out of sequence western id
    else
    {
        // The contigious western ids
        if (eVersion <= ww::eWW2)
            nI = static_cast< BYTE >(nId - 60);
        else if (eVersion < ww::eWW8)
            nI = static_cast< BYTE >(nId - 85);
        else
            nI = static_cast< BYTE >(nId - 0x0835);
    }

    sal_uInt16 nMask = 1 << nI;

    if (nLen < 0)
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), nEndIds[ nI ] );
        // reset the CJK Weight and Posture, because they are the same as their
        // western equivalents in word
        if (nI < 2)
            pCtrlStck->SetAttr( *pPaM->GetPoint(), nEndIds[ nWestern + nI ] );
        pCtrlStck->SetToggleAttr(nI, false);
        return;
    }
    // Wert: 0 = Aus, 1 = An, 128 = Wie Style, 129 entgegen Style
    bool bOn = *pData & 1;
    SwWW8StyInf* pSI = GetStyle(nAktColl);
    if (pPlcxMan && eVersion > ww::eWW2)
    {
        const BYTE *pCharIstd =
            pPlcxMan->GetChpPLCF()->HasSprm(bVer67 ? 80 : 0x4A30);
        if (pCharIstd)
            pSI = GetStyle(SVBT16ToShort(pCharIstd));
    }

    if( pAktColl )                          // StyleDef -> Flags merken
    {
        if (pSI)
        {
            // The style based on has Bit 7 set ?
            if (
                pSI->nBase < nColls && (*pData & 0x80) &&
                (pCollA[pSI->nBase].n81Flags & nMask)
               )
            {
                bOn = !bOn;                     // umdrehen
            }

            if (bOn)
                pSI->n81Flags |= nMask;         // Flag setzen
            else
                pSI->n81Flags &= ~nMask;        // Flag loeschen
       }
    }
    else
    {

        // im Text -> Flags abfragen
        if( *pData & 0x80 )                 // Bit 7 gesetzt ?
        {
            if (pSI && pSI->n81Flags & nMask)       // und in StyleDef an ?
                bOn = !bOn;                 // dann invertieren
            // am Stack vermerken, das dieses ein Toggle-Attribut ist
            pCtrlStck->SetToggleAttr(nI, true);
        }
    }

    SetToggleAttr( nI, bOn );
}

void SwWW8ImplReader::Read_Bidi(USHORT, const BYTE*, short nLen)
{
    if (nLen > 0)
        bBidi = true;
    else
        bBidi = false;
}

// Read_BoldUsw for BiDi Italic, Bold
void SwWW8ImplReader::Read_BoldBiDiUsw(USHORT nId, const BYTE* pData,
    short nLen)
{
    static const USHORT nEndIds[2] =
    {
        RES_CHRATR_CTL_WEIGHT, RES_CHRATR_CTL_POSTURE,
    };

    BYTE nI;
    ww::WordVersion eVersion = pWwFib->GetFIBVersion();
    if (eVersion <= ww::eWW2)
        nI = static_cast< BYTE >(nId - 80);
    else if (eVersion < ww::eWW8)
        nI = static_cast< BYTE >(nId - 111);
    else
        nI = static_cast< BYTE >(nId - 0x085C);

    ASSERT(nI <= 1, "not happening");
    if (nI > 1)
        return;

    sal_uInt16 nMask = 1 << nI;

    if( nLen < 0 )
    {
        pCtrlStck->SetAttr(*pPaM->GetPoint(),nEndIds[nI]);
        pCtrlStck->SetToggleBiDiAttr(nI, false);
        return;
    }
    bool bOn = *pData & 1;
    SwWW8StyInf* pSI = GetStyle(nAktColl);
    if (pPlcxMan)
    {
        const BYTE *pCharIstd =
            pPlcxMan->GetChpPLCF()->HasSprm(bVer67 ? 80 : 0x4A30);
        if (pCharIstd)
            pSI = GetStyle(SVBT16ToShort(pCharIstd));
    }

    if (pAktColl && eVersion > ww::eWW2)        // StyleDef -> Flags merken
    {
        if (pSI)
        {
            if( pSI->nBase < nColls             // Style Based on
                && ( *pData & 0x80 )            // Bit 7 gesetzt ?
                && ( pCollA[pSI->nBase].n81BiDiFlags & nMask ) ) // BasisMaske ?
                    bOn = !bOn;                     // umdrehen

            if( bOn )
                pSI->n81BiDiFlags |= nMask;         // Flag setzen
            else
                pSI->n81BiDiFlags &= ~nMask;        // Flag loeschen
        }
    }
    else
    {

        // im Text -> Flags abfragen
        if (*pData & 0x80)                  // Bit 7 gesetzt ?
        {
            if (pSI && pSI->n81BiDiFlags & nMask) // und in StyleDef an ?
                bOn = !bOn;                     // dann invertieren
            // am Stack vermerken, das dieses ein Toggle-Attribut ist
            pCtrlStck->SetToggleBiDiAttr(nI, true);
        }
    }

    SetToggleBiDiAttr(nI, bOn);
}

void SwWW8ImplReader::SetToggleBiDiAttr(BYTE nAttrId, bool bOn)
{
    switch (nAttrId)
    {
        case 0:
            {
                SvxWeightItem aAttr( bOn ? WEIGHT_BOLD : WEIGHT_NORMAL, RES_CHRATR_WEIGHT );
                aAttr.SetWhich( RES_CHRATR_CTL_WEIGHT );
                NewAttr( aAttr );
            }
            break;
        case 1:
            {
                SvxPostureItem aAttr( bOn ? ITALIC_NORMAL : ITALIC_NONE, RES_CHRATR_POSTURE );
                aAttr.SetWhich( RES_CHRATR_CTL_POSTURE );
                NewAttr( aAttr );
            }
            break;
        default:
            ASSERT(!this, "Unhandled unknown bidi toggle attribute");
            break;

    }
}

void SwWW8ImplReader::SetToggleAttr(BYTE nAttrId, bool bOn)
{
    switch (nAttrId)
    {
        case 0:
            {
                SvxWeightItem aAttr( bOn ? WEIGHT_BOLD : WEIGHT_NORMAL, RES_CHRATR_WEIGHT );
                NewAttr( aAttr );
                aAttr.SetWhich( RES_CHRATR_CJK_WEIGHT );
                NewAttr( aAttr );
            }
            break;
        case 1:
            {
                SvxPostureItem aAttr( bOn ? ITALIC_NORMAL : ITALIC_NONE, RES_CHRATR_POSTURE );
                NewAttr( aAttr );
                aAttr.SetWhich( RES_CHRATR_CJK_POSTURE );
                NewAttr( aAttr );
            }
            break;
        case 2:
            NewAttr(SvxCrossedOutItem(bOn ? STRIKEOUT_SINGLE : STRIKEOUT_NONE, RES_CHRATR_CROSSEDOUT));
            break;
        case 3:
            NewAttr( SvxContourItem( bOn, RES_CHRATR_CONTOUR ) );
            break;
        case 4:
            NewAttr( SvxShadowedItem( bOn, RES_CHRATR_SHADOWED ) );
            break;
        case 5:
            NewAttr( SvxCaseMapItem( bOn ? SVX_CASEMAP_KAPITAELCHEN
                                              : SVX_CASEMAP_NOT_MAPPED, RES_CHRATR_CASEMAP ) );
            break;
        case 6:
            NewAttr( SvxCaseMapItem( bOn ? SVX_CASEMAP_VERSALIEN
                                             : SVX_CASEMAP_NOT_MAPPED, RES_CHRATR_CASEMAP ) );
            break;
        case 7:
            NewAttr(SvxCharHiddenItem(bOn, RES_CHRATR_HIDDEN));
            break;
        case 8:
            NewAttr( SvxCrossedOutItem( bOn ? STRIKEOUT_DOUBLE
                                                : STRIKEOUT_NONE, RES_CHRATR_CROSSEDOUT ) );
            break;
        default:
            ASSERT(!this, "Unhandled unknown toggle attribute");
            break;
    }
}

void SwWW8ImplReader::_ChkToggleAttr( USHORT nOldStyle81Mask,
                                        USHORT nNewStyle81Mask )
{
    USHORT i = 1, nToggleAttrFlags = pCtrlStck->GetToggleAttrFlags();
    for (BYTE n = 0; n < 7; ++n, i <<= 1)
    {
        if (
            (i & nToggleAttrFlags) &&
            ((i & nOldStyle81Mask) != (i & nNewStyle81Mask))
           )
        {
            SetToggleAttr(n, (i & nOldStyle81Mask));
        }
    }
}

void SwWW8ImplReader::_ChkToggleBiDiAttr( USHORT nOldStyle81Mask,
                                        USHORT nNewStyle81Mask )
{
    USHORT i = 1, nToggleAttrFlags = pCtrlStck->GetToggleBiDiAttrFlags();
    for (BYTE n = 0; n < 7; ++n, i <<= 1)
    {
        if (
            (i & nToggleAttrFlags) &&
            ((i & nOldStyle81Mask) != (i & nNewStyle81Mask))
           )
        {
            SetToggleBiDiAttr(n, (i & nOldStyle81Mask));
        }
    }
}

void SwWW8ImplReader::Read_SubSuper( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 ){
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_ESCAPEMENT );
        return;
    }

    short nEs;
    BYTE nProp;
    switch( *pData )
    {
        case 1:
            nEs = DFLT_ESC_AUTO_SUPER;
            nProp = DFLT_ESC_PROP;
            break;
        case 2:
            nEs = DFLT_ESC_AUTO_SUB;
            nProp = DFLT_ESC_PROP;
            break;
        default:
            nEs = 0;
            nProp = 100;
            break;
    }
    NewAttr( SvxEscapementItem( nEs, nProp, RES_CHRATR_ESCAPEMENT ) );
}

SwFrmFmt *SwWW8ImplReader::ContainsSingleInlineGraphic(const SwPaM &rRegion)
{
    /*
    #92489# & #92946#
    For inline graphics and objects word has a hacked in feature to use
    subscripting to force the graphic into a centered position on the line, so
    we must check when applying sub/super to see if it the subscript range
    contains only a single graphic, and if that graphic is anchored as
    FLY_IN_CNTNT and then we can change its anchoring to centered in the line.
    */
    SwFrmFmt *pRet=0;
    SwNodeIndex aBegin(rRegion.Start()->nNode);
    xub_StrLen nBegin(rRegion.Start()->nContent.GetIndex());
    SwNodeIndex aEnd(rRegion.End()->nNode);
    xub_StrLen nEnd(rRegion.End()->nContent.GetIndex());
    const SwTxtNode* pTNd;
    const SwTxtAttr* pTFlyAttr;
    if (
         aBegin == aEnd && nBegin == nEnd - 1 &&
         0 != (pTNd = aBegin.GetNode().GetTxtNode()) &&
         0 != (pTFlyAttr = pTNd->GetTxtAttrForCharAt(nBegin, RES_TXTATR_FLYCNT))
       )
    {
        const SwFmtFlyCnt& rFly = pTFlyAttr->GetFlyCnt();
        SwFrmFmt *pFlyFmt = rFly.GetFrmFmt();
        if( pFlyFmt &&
            FLY_IN_CNTNT == pFlyFmt->GetAnchor().GetAnchorId() )
        {
            pRet = pFlyFmt;
        }
    }
    return pRet;
}

bool SwWW8ImplReader::ConvertSubToGraphicPlacement()
{
    /*
    #92489# & #92946#
    For inline graphics and objects word has a hacked in feature to use
    subscripting to force the graphic into a centered position on the line, so
    we must check when applying sub/super to see if it the subscript range
    contains only a single graphic, and if that graphic is anchored as
    FLY_IN_CNTNT and then we can change its anchoring to centered in the line.
    */
    bool bIsGraphicPlacementHack = false;
    USHORT nPos;
    if (pCtrlStck->GetFmtStackAttr(RES_CHRATR_ESCAPEMENT, &nPos))
    {
        SwPaM aRegion(*pPaM->GetPoint());

        SwFltStackEntry aEntry = *((*pCtrlStck)[nPos]);
        aEntry.SetEndPos(*pPaM->GetPoint());

        SwFrmFmt *pFlyFmt = 0;
        if (
             aEntry.MakeRegion(&rDoc,aRegion,false) &&
             0 != (pFlyFmt = ContainsSingleInlineGraphic(aRegion))
           )
        {
            pCtrlStck->DeleteAndDestroy(nPos);
            pFlyFmt->SetFmtAttr(SwFmtVertOrient(0, text::VertOrientation::CHAR_CENTER, text::RelOrientation::CHAR));
            bIsGraphicPlacementHack = true;
        }
    }
    return bIsGraphicPlacementHack;
}

void SwWW8ImplReader::Read_SubSuperProp( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
    {
        if (!ConvertSubToGraphicPlacement())
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_ESCAPEMENT );
        return;
    }

    ww::WordVersion eVersion = pWwFib->GetFIBVersion();

    // Font-Position in HalfPoints
    short nPos = eVersion <= ww::eWW2 ? *pData : SVBT16ToShort( pData );
    INT32 nPos2 = nPos * ( 10 * 100 );      // HalfPoints in 100 * tw
    const SvxFontHeightItem* pF
        = (const SvxFontHeightItem*)GetFmtAttr(RES_CHRATR_FONTSIZE);
    ASSERT(pF, "Expected to have the fontheight available here");

    // #i59022: Check ensure nHeight != 0. Div by zero otherwise.
    INT32 nHeight = 240;
    if (pF != NULL && pF->GetHeight() != 0)
        nHeight = pF->GetHeight();
    nPos2 /= nHeight;                       // ... nun in % ( gerundet )
    if( nPos2 > 100 )                       // zur Sicherheit
        nPos2 = 100;
    if( nPos2 < -100 )
        nPos2 = -100;
    SvxEscapementItem aEs( (short)nPos2, 100, RES_CHRATR_ESCAPEMENT );
    NewAttr( aEs );
}

void SwWW8ImplReader::Read_Underline( USHORT, const BYTE* pData, short nLen )
{
    FontUnderline eUnderline = UNDERLINE_NONE;
    bool bWordLine = false;
    if( pData )
    {
        // Parameter:  0 = none,    1 = single,  2 = by Word,
                    // 3 = double,  4 = dotted,  5 = hidden
                    // 6 = thick,   7 = dash,    8 = dot(not used)
                    // 9 = dotdash 10 = dotdotdash 11 = wave


        // pruefe auf Sonderfall "fett+unterstrichen"
        bool bAlsoBold = /*( 6 == b )*/ false;
        // erst mal ggfs. *bold* einschalten!
        if( bAlsoBold )
        {
            BYTE nOn = 1;
            Read_BoldUsw( 0x0835, &nOn, nLen );
            eUnderline = UNDERLINE_SINGLE;
        }
        else
        {
            switch( *pData )
            {
            case 2: bWordLine = true;       // no break;
            case 1: eUnderline = (FontUnderline)UNDERLINE_SINGLE;       break;
            case 3: eUnderline = (FontUnderline)UNDERLINE_DOUBLE;       break;
            case 4: eUnderline = (FontUnderline)UNDERLINE_DOTTED;       break;
            case 7: eUnderline = (FontUnderline)UNDERLINE_DASH;         break;
            case 9: eUnderline = (FontUnderline)UNDERLINE_DASHDOT;      break;
            case 10:eUnderline = (FontUnderline)UNDERLINE_DASHDOTDOT;   break;
            case 6: eUnderline = (FontUnderline)UNDERLINE_BOLD;         break;
            case 11:eUnderline = (FontUnderline)UNDERLINE_WAVE;         break;
            case 20:eUnderline = (FontUnderline)UNDERLINE_BOLDDOTTED;   break;
            case 23:eUnderline = (FontUnderline)UNDERLINE_BOLDDASH;     break;
            case 39:eUnderline = (FontUnderline)UNDERLINE_LONGDASH;     break;
            case 55:eUnderline = (FontUnderline)UNDERLINE_BOLDLONGDASH; break;
            case 25:eUnderline = (FontUnderline)UNDERLINE_BOLDDASHDOT;  break;
            case 26:eUnderline = (FontUnderline)UNDERLINE_BOLDDASHDOTDOT;break;
            case 27:eUnderline = (FontUnderline)UNDERLINE_BOLDWAVE;     break;
            case 43:eUnderline = (FontUnderline)UNDERLINE_DOUBLEWAVE;   break;
            }
        }
    }

    // dann Stack ggfs. verwursteln und exit!
    if( nLen < 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_UNDERLINE );
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_WORDLINEMODE );
    }
    else
    {
        NewAttr( SvxUnderlineItem( eUnderline, RES_CHRATR_UNDERLINE ));
        if( bWordLine )
            NewAttr(SvxWordLineModeItem(true, RES_CHRATR_WORDLINEMODE));
    }
}

/*
//The last three vary, measurements, rotation ? ?
NoBracket   78 CA 06 -  02 00 00 02 34 52
()          78 CA 06 -  02 01 00 02 34 52
[]          78 CA 06 -  02 02 00 02 34 52
<>          78 CA 06 -  02 03 00 02 34 52
{}          78 CA 06 -  02 04 00 02 34 52
*/
void SwWW8ImplReader::Read_DoubleLine_Rotate( USHORT, const BYTE* pData,
    short nLen )
{
    if( nLen < 0 ) // close the tag
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_TWO_LINES );
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_ROTATE );
    }
    else if( pData && 6 == nLen )
    {
        switch( *pData )
        {
        case 2:                     // double line
            {
                sal_Unicode cStt = 0, cEnd = 0;
                switch( SVBT16ToShort( pData+1 ) )
                {
                case 1: cStt = '(', cEnd = ')'; break;
                case 2: cStt = '[', cEnd = ']'; break;
                case 3: cStt = '<', cEnd = '>'; break;
                case 4: cStt = '{', cEnd = '}'; break;
                }
                NewAttr( SvxTwoLinesItem( sal_True, cStt, cEnd, RES_CHRATR_TWO_LINES ));
            }
            break;

        case 1:                         // rotated characters
            {
                bool bFitToLine = 0 != *(pData+1);
                NewAttr( SvxCharRotateItem( 900, bFitToLine, RES_CHRATR_ROTATE ));
            }
            break;
        }
    }
}

void SwWW8ImplReader::Read_TxtColor( USHORT, const BYTE* pData, short nLen )
{
    //Has newer colour varient, ignore this old varient
    if (!bVer67 && pPlcxMan && pPlcxMan->GetChpPLCF()->HasSprm(0x6870))
        return;

    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
    else
    {
        BYTE b = *pData;            // Parameter: 0 = Auto, 1..16 Farben

        if( b > 16 )                // unbekannt -> Black
            b = 0;

        NewAttr( SvxColorItem(Color(GetCol(b)), RES_CHRATR_COLOR));
        if (pAktColl && pStyles)
            pStyles->bTxtColChanged = true;
    }
}

sal_uInt32 wwUtility::BGRToRGB(sal_uInt32 nColor)
{
    sal_uInt8
        r(static_cast<sal_uInt8>(nColor&0xFF)),
        g(static_cast<sal_uInt8>(((nColor)>>8)&0xFF)),
        b(static_cast<sal_uInt8>((nColor>>16)&0xFF)),
        t(static_cast<sal_uInt8>((nColor>>24)&0xFF));
    nColor = (t<<24) + (r<<16) + (g<<8) + b;
    return nColor;
}

void SwWW8ImplReader::Read_TxtForeColor(USHORT, const BYTE* pData, short nLen)
{
    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
    else
    {
        Color aColor(wwUtility::BGRToRGB(SVBT32ToUInt32(pData)));
        NewAttr(SvxColorItem(aColor, RES_CHRATR_COLOR));
        if (pAktColl && pStyles)
            pStyles->bTxtColChanged = true;
    }
}

bool SwWW8ImplReader::GetFontParams( USHORT nFCode, FontFamily& reFamily,
    String& rName, FontPitch& rePitch, CharSet& reCharSet )
{
    // Die Defines, aus denen diese Tabellen erzeugt werden, stehen in windows.h
    static const FontPitch ePitchA[] =
    {
        PITCH_DONTKNOW, PITCH_FIXED, PITCH_VARIABLE, PITCH_DONTKNOW
    };

    static const FontFamily eFamilyA[] =
    {
        FAMILY_DONTKNOW, FAMILY_ROMAN, FAMILY_SWISS, FAMILY_MODERN,
        FAMILY_SCRIPT, FAMILY_DECORATIVE
    };

    const WW8_FFN* pF = pFonts->GetFont( nFCode );  // Info dazu
    if( !pF )                                   // FontNummer unbekannt ?
        return false;                           // dann ignorieren

    rName = String( pF->sFontname );

    // pF->prg : Pitch
    rePitch = ePitchA[pF->prg];

    // pF->chs: Charset
    if( 77 == pF->chs )             // Mac-Font im Mac-Charset oder
        reCharSet = eTextCharSet;   // auf ANSI-Charset uebersetzt
    else
    { // patch from cmc for #i52786#
        // #i52786#, for word 67 we'll assume that ANSI is basically invalid,
        // might be true for (above) mac as well, but would need a mac example
        // that exercises this to be sure
        if (bVer67 && pF->chs == 0)
            reCharSet = RTL_TEXTENCODING_DONTKNOW;
        else
            reCharSet = rtl_getTextEncodingFromWindowsCharset( pF->chs );
    }

    // pF->ff : Family
    BYTE b = pF->ff;

    // make sure Font Family Code is set correctly
    // at least for the most important fonts
    // ( might be set wrong when Doc was not created by
    //   Winword but by third party program like Applixware... )
        /*
        0: FAMILY_DONTKNOW
        1: FAMILY_ROMAN
        2: FAMILY_SWISS
        3: FAMILY_MODERN
        4: FAMILY_SCRIPT
        5: FAMILY_DECORATIVE
    */
#define FONTNAMETAB_SZ    14
#define MAX_FONTNAME_ROMAN 6
    static const sal_Char
        // first comes ROMAN
        sFontName0[] = "\x07""Tms Rmn",
        sFontName1[] = "\x07""Timmons",
        sFontName2[] = "\x08""CG Times",
        sFontName3[] = "\x08""MS Serif",
        sFontName4[] = "\x08""Garamond",
        sFontName5[] = "\x11""Times Roman",
        sFontName6[] = "\x15""Times New Roman",
        // from here SWISS --> see above: #define MAX_FONTNAME_ROMAN 6
        sFontName7[] = "\x04""Helv",
        sFontName8[] = "\x05""Arial",
        sFontName9[] = "\x07""Univers",
        sFontName10[]= "\x11""LinePrinter",
        sFontName11[]= "\x11""Lucida Sans",
        sFontName12[]= "\x11""Small Fonts",
        sFontName13[]= "\x13""MS Sans Serif";
    static const sal_Char* const aFontNameTab[ FONTNAMETAB_SZ ] =
    {
        sFontName0,  sFontName1,  sFontName2,  sFontName3,
        sFontName4,  sFontName5,  sFontName6,  sFontName7,
        sFontName8,  sFontName9,  sFontName10, sFontName11,
        sFontName12, sFontName13
    };

    for( USHORT n = 0;  n < FONTNAMETAB_SZ; n++ )
    {
        const sal_Char* pCmp = aFontNameTab[ n ];
        xub_StrLen nLen = *pCmp++;
        if( rName.EqualsIgnoreCaseAscii(pCmp, 0, nLen) )
        {
            b = n <= MAX_FONTNAME_ROMAN ? 1 : 2;
            break;
        }
    }
    if( b < sizeof( eFamilyA ) )
        reFamily = eFamilyA[b];
    else
        reFamily = FAMILY_DONTKNOW;

    return true;
}

bool SwWW8ImplReader::SetNewFontAttr(USHORT nFCode, bool bSetEnums,
    USHORT nWhich)
{
    FontFamily eFamily;
    String aName;
    FontPitch ePitch;
    CharSet eSrcCharSet;

    if( !GetFontParams( nFCode, eFamily, aName, ePitch, eSrcCharSet ) )
    {
        //If we fail (and are not doing a style) then put something into the
        //character encodings stack anyway so that the property end that pops
        //off the stack will keep in sync
        if (!pAktColl && IsListOrDropcap())
        {
            if (nWhich == RES_CHRATR_CJK_FONT)
            {
                if (!maFontSrcCJKCharSets.empty())
                {
                    eSrcCharSet = maFontSrcCJKCharSets.top();
                }
                else
                {
                    eSrcCharSet = RTL_TEXTENCODING_DONTKNOW;
                }
                    
                maFontSrcCJKCharSets.push(eSrcCharSet);
            }
            else
            {
                if (!maFontSrcCharSets.empty())
                {
                    eSrcCharSet = maFontSrcCharSets.top();
                }
                else
                {
                    eSrcCharSet = RTL_TEXTENCODING_DONTKNOW;
                }
                    
                maFontSrcCharSets.push(eSrcCharSet);
            }
        }
        return false;
    }

    CharSet eDstCharSet = eSrcCharSet;

    SvxFontItem aFont( eFamily, aName, aEmptyStr, ePitch, eDstCharSet, nWhich);

    if( bSetEnums )
    {
        if( pAktColl ) // StyleDef
        {
            switch(nWhich)
            {
                default:
                case RES_CHRATR_FONT:
                    pCollA[nAktColl].eLTRFontSrcCharSet = eSrcCharSet;
                    break;
                case RES_CHRATR_CTL_FONT:
                    pCollA[nAktColl].eRTLFontSrcCharSet = eSrcCharSet;
                    break;
                case RES_CHRATR_CJK_FONT:
                    pCollA[nAktColl].eCJKFontSrcCharSet = eSrcCharSet;
                    break;
            }
        }
        else if (IsListOrDropcap())
        {
            //Add character text encoding to stack
            if (nWhich  == RES_CHRATR_CJK_FONT)
                maFontSrcCJKCharSets.push(eSrcCharSet);
            else
                maFontSrcCharSets.push(eSrcCharSet);
        }
    }

    NewAttr( aFont );                       // ...und 'reinsetzen

    return true;
}

void SwWW8ImplReader::ResetCharSetVars()
{
    ASSERT(!maFontSrcCharSets.empty(),"no charset to remove");
    if (!maFontSrcCharSets.empty())
        maFontSrcCharSets.pop();
}

void SwWW8ImplReader::ResetCJKCharSetVars()
{
    ASSERT(!maFontSrcCJKCharSets.empty(),"no charset to remove");
    if (!maFontSrcCJKCharSets.empty())
        maFontSrcCJKCharSets.pop();
}

/*
    Font ein oder ausschalten:
*/
void SwWW8ImplReader::Read_FontCode( USHORT nId, const BYTE* pData, short nLen )
{
    if (!bSymbol)           // falls bSymbol, gilt der am Symbol
    {                       // (siehe sprmCSymbol) gesetzte Font !
        switch( nId )
        {
    //      case 0x4a51:    //font to bias towards all else being equal ?
            case 113:
            case 0x4a5E:
                nId = RES_CHRATR_CTL_FONT;
                break;
            case 93:
            case 111:
            case 0x4a4f:
                nId = RES_CHRATR_FONT;
                break;
            case 112:
            case 0x4a50:
                nId = RES_CHRATR_CJK_FONT;
                break;
            default:
                return ;
        }

        if( nLen < 0 ) // Ende des Attributes
        {
            pCtrlStck->SetAttr( *pPaM->GetPoint(), nId );
            if (nId == RES_CHRATR_CJK_FONT)
                ResetCJKCharSetVars();
            else            
                ResetCharSetVars();
        }
        else
        {
            USHORT nFCode = SVBT16ToShort( pData );     // Font-Nummer
            if (SetNewFontAttr(nFCode, true, nId)   // Lies Inhalt
                && pAktColl && pStyles )                // Style-Def ?
            {
                // merken zur Simulation Default-Font
                if (RES_CHRATR_CJK_FONT == nId)
                    pStyles->bCJKFontChanged = true;
                else if (RES_CHRATR_CTL_FONT == nId)
                    pStyles->bCTLFontChanged = true;
                else
                    pStyles->bFontChanged = true;
            }
        }
    }
}

void SwWW8ImplReader::Read_FontSize( USHORT nId, const BYTE* pData, short nLen )
{
    switch( nId )
    {
        case 74:
        case 99:
        case 0x4a43:
            nId = RES_CHRATR_FONTSIZE;
            break;
        case 85:
        case 116:
        case 0x4a61:
            nId = RES_CHRATR_CTL_FONTSIZE;
            break;
        default:
            return ;
    }

    if( nLen < 0 )          // Ende des Attributes
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), nId  );
        if( RES_CHRATR_FONTSIZE == nId )  // reset additional the CJK size
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_CJK_FONTSIZE );
    }
    else
    {
        ww::WordVersion eVersion = pWwFib->GetFIBVersion();

        // Font-Size in half points e.g. 10 = 1440 / ( 72 * 2 )
        USHORT nFSize = eVersion <= ww::eWW2 ? *pData : SVBT16ToShort(pData);
        nFSize*= 10;

        SvxFontHeightItem aSz( nFSize, 100, nId );
        NewAttr( aSz );
        if( RES_CHRATR_FONTSIZE == nId )  // set additional the CJK size
        {
            aSz.SetWhich( RES_CHRATR_CJK_FONTSIZE );
            NewAttr( aSz );
        }
        if (pAktColl && pStyles)            // Style-Def ?
        {
            // merken zur Simulation Default-FontSize
            if (nId == RES_CHRATR_CTL_FONTSIZE)
                pStyles->bFCTLSizeChanged = true;
            else
                pStyles->bFSizeChanged = true;
        }
    }
}



void SwWW8ImplReader::Read_CharSet(USHORT , const BYTE* pData, short nLen)
{
    if( nLen < 0 )
    {                   // Ende des Attributes
        eHardCharSet = RTL_TEXTENCODING_DONTKNOW;
        return;
    }
    BYTE nfChsDiff = SVBT8ToByte( pData );

    if( nfChsDiff )
        eHardCharSet = rtl_getTextEncodingFromWindowsCharset( *(pData + 1) );
    else
        eHardCharSet = RTL_TEXTENCODING_DONTKNOW;
}

void SwWW8ImplReader::Read_Language( USHORT nId, const BYTE* pData, short nLen )
{
    switch( nId )
    {
        case 97:
        case 0x486D:
        case 0x4873: //Methinks, uncertain
            nId = RES_CHRATR_LANGUAGE;
            break;
        case 0x486E:
            nId = RES_CHRATR_CJK_LANGUAGE;
            break;
        case 83:
        case 114:
        case 0x485F:
            nId = RES_CHRATR_CTL_LANGUAGE;
            break;
        default:
            return;
    }

    if( nLen < 0 )                  // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), nId );
    else
    {
        USHORT nLang = SVBT16ToShort( pData );  // Language-Id
        NewAttr(SvxLanguageItem((const LanguageType)nLang, nId));
    }
}

/*
    Einschalten des Zeichen-Styles:
*/
void SwWW8ImplReader::Read_CColl( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 ){                 // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_TXTATR_CHARFMT );
        nCharFmt = -1;
        return;
    }
    USHORT nId = SVBT16ToShort( pData );    // Style-Id (NICHT Sprm-Id!)

    if( nId >= nColls || !pCollA[nId].pFmt  // ungueltige Id ?
        || pCollA[nId].bColl )              // oder Para-Style ?
        return;                             // dann ignorieren

    NewAttr( SwFmtCharFmt( (SwCharFmt*)pCollA[nId].pFmt ) );
    nCharFmt = (short) nId;
}


/*
    enger oder weiter als normal:
*/
void SwWW8ImplReader::Read_Kern( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 ){                 // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_KERNING );
        return;
    }
    INT16 nKern = SVBT16ToShort( pData );    // Kerning in Twips
    NewAttr( SvxKerningItem( nKern, RES_CHRATR_KERNING ) );
}

void SwWW8ImplReader::Read_FontKern( USHORT, const BYTE* , short nLen )
{
    if( nLen < 0 ) // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_AUTOKERN );
    else
        NewAttr(SvxAutoKernItem(true, RES_CHRATR_AUTOKERN));
}

void SwWW8ImplReader::Read_CharShadow(  USHORT, const BYTE* pData, short nLen )
{
    //Has newer colour varient, ignore this old varient
    if (!bVer67 && pPlcxMan && pPlcxMan->GetChpPLCF()->HasSprm(0xCA71))
        return;

    if( nLen <= 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_BACKGROUND );
        if( bCharShdTxtCol )
        {
            // Zeichenfarbe auch
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
            bCharShdTxtCol = false;
        }
    }
    else
    {
        WW8_SHD aSHD;
        aSHD.SetWWValue( *(SVBT16*)pData );
        SwWW8Shade aSh( bVer67, aSHD );

        NewAttr( SvxBrushItem( aSh.aColor, RES_CHRATR_BACKGROUND ));
    }
}

void SwWW8ImplReader::Read_TxtBackColor(USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_BACKGROUND );
        if( bCharShdTxtCol )
        {
            // Zeichenfarbe auch
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
            bCharShdTxtCol = false;
        }
    }
    else
    {
        ASSERT(nLen == 10, "Len of para back colour not 10!");
        if (nLen != 10)
            return;
        Color aColour(ExtractColour(pData, bVer67));
        NewAttr(SvxBrushItem(aColour, RES_CHRATR_BACKGROUND));
    }
}

void SwWW8ImplReader::Read_CharHighlight(USHORT, const BYTE* pData, short nLen)
{
    if( nLen <= 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_BACKGROUND );
        if( bCharShdTxtCol )
        {
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );  // Zeichenfarbe auch
            bCharShdTxtCol = false;
        }
    }
    else
    {
        BYTE b = *pData;            // Parameter: 0 = Auto, 1..16 Farben

        if( b > 16 )                // unbekannt -> Black
            b = 0;                  // Auto -> Black

        Color aCol(GetCol(b));
        NewAttr( SvxBrushItem( aCol , RES_CHRATR_BACKGROUND ));
    }
}


/***************************************************************************
#  Absatz - Attribute
#**************************************************************************/

void SwWW8ImplReader::Read_NoLineNumb(USHORT , const BYTE* pData, short nLen)
{
    if( nLen < 0 )  // Ende des Attributes
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_LINENUMBER );
        return;
    }
    SwFmtLineNumber aLN;
    if (const SwFmtLineNumber* pLN
        = (const SwFmtLineNumber*)GetFmtAttr(RES_LINENUMBER))
    {
        aLN.SetStartValue( pLN->GetStartValue() );
    }

    aLN.SetCountLines( pData && (0 == *pData) );
    NewAttr( aLN );
}

// Sprm 16, 17
void SwWW8ImplReader::Read_LR( USHORT nId, const BYTE* pData, short nLen )
{
    if (nLen < 0)  // End of the Attributes
    {
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_LR_SPACE);
        return;
    }

    short nPara = SVBT16ToShort( pData );

    SvxLRSpaceItem aLR( RES_LR_SPACE );
    const SfxPoolItem* pLR = GetFmtAttr(RES_LR_SPACE);
    if( pLR )
        aLR = *(const SvxLRSpaceItem*)pLR;

    /*
    The older word sprms mean left/right, while the new ones mean before/after.
    Writer now also works with before after, so when we see old left/right and
    we're RTL. We swap them
    */
    if (IsRightToLeft())
    {
        switch (nId)
        {
            //Left becomes after;
            case 17:
                nId = 16;
                break;
            case 0x840F:
                nId = 0x840E;
                break;
            //Right becomes before;
            case 16:
                nId = 17;
                break;
            case 0x840E:
                nId = 0x840F;
                break;
        }
    }

    switch (nId)
    {
        //sprmPDxaLeft
        case     17:
        case 0x840F:
        case 0x845E:
            aLR.SetTxtLeft( nPara );
            if (pAktColl)
                pCollA[nAktColl].bListReleventIndentSet = true;
            break;
        //sprmPDxaLeft1
        case     19:
        case 0x8411:
        case 0x8460:
            /*
            #94672# #99584#
            As part of an attempt to break my spirit ww 8+ formats can contain
            ww 7- lists. If they do and the list is part of the style, then
            when removing the list from a paragraph of that style there
            appears to be a bug where the hanging indent value which the list
            set is still factored into the left indent of the paragraph. Its
            not listed in the winword dialogs, but it is clearly there. So if
            our style has a broken ww 7- list and we know that the list has
            been removed then we will factor the original list applied hanging
            into our calculation.
            */
            if (pPlcxMan && pCollA[nAktColl].bHasBrokenWW6List)
            {
                const BYTE *pIsZeroed = pPlcxMan->GetPapPLCF()->HasSprm(0x460B);
                if (pIsZeroed && *pIsZeroed == 0)
                {
                    const SvxLRSpaceItem &rLR =
                        ItemGet<SvxLRSpaceItem>(*(pCollA[nAktColl].pFmt),
                        RES_LR_SPACE);
                    nPara = nPara - rLR.GetTxtFirstLineOfst();
                }
            }

            aLR.SetTxtFirstLineOfst(nPara);
            if (pAktColl)
                pCollA[nAktColl].bListReleventIndentSet = true;
            break;
        //sprmPDxaRight
        case     16:
        case 0x840E:
        case 0x845D:
            aLR.SetRight( nPara );
            break;
        default:
            return;
    }

    NewAttr(aLR);
}

// Sprm 20
void SwWW8ImplReader::Read_LineSpace( USHORT, const BYTE* pData, short nLen )
{
// Kommentear siehe Read_UL()
    if (bStyNormal && bWWBugNormal)
        return;

    if( nLen < 0 ){
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_LINESPACING );
        if( !( nIniFlags & WW8FL_NO_IMPLPASP ) )
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_UL_SPACE );
        return;
    }

    short nSpace = SVBT16ToShort( pData );
    ww::WordVersion eVersion = pWwFib->GetFIBVersion();
    short nMulti = (eVersion <= ww::eWW2) ? 1 : SVBT16ToShort( pData + 2 );

    SvxLineSpace eLnSpc;
    if( 0 > nSpace )
    {
        nSpace = -nSpace;
        eLnSpc = SVX_LINE_SPACE_FIX;
    }
    else
        eLnSpc = SVX_LINE_SPACE_MIN;

// WW hat einen impliziten zusaetzlichen Absatzabstand abhaengig vom
// Zeilenabstand. Er betraegt bei "genau", 0.8*Zeilenabstand "vor" und
// 0.2*Zeilenabstand "nach".
// Bei "Mindestens" sind es 1*Zeilenabstand "vor" und 0*Zeilenabstand "nach".
// Bei Mehrfach sind es 0 "vor" und min( 0cm, FontSize*(nFach-1) ) "nach".
//
// SW hat auch einen impliziten Zeilenabstand. er betraegt bei "mindestens"
// 1*Zeilenabstand "vor" und 0 "nach"
// bei proportional betraegt er min( 0cm, FontSize*(nFach-1) ) sowohl "vor"
// wie auch "nach"

    USHORT nWwPre = 0;
    USHORT nWwPost = 0;
    USHORT nSwPre = 0;
    USHORT nSwPost = 0;
    USHORT nSpaceTw = 0;

    SvxLineSpacingItem aLSpc( LINE_SPACE_DEFAULT_HEIGHT, RES_PARATR_LINESPACING );

    if( 1 == nMulti )               // MultilineSpace ( proportional )
    {
        long n = nSpace * 10 / 24;  // WW: 240 = 100%, SW: 100 = 100%

//JP 03.12.98: nach Absprache mit AMA ist die Begrenzung unsinnig
        if( n>200 ) n = 200;        // SW_UI-Maximum
        aLSpc.SetPropLineSpace( (const BYTE)n );
        const SvxFontHeightItem* pH = (const SvxFontHeightItem*)
            GetFmtAttr( RES_CHRATR_FONTSIZE );
        nSpaceTw = (USHORT)( n * pH->GetHeight() / 100 );

        if( n > 100 )
            nWwPost = nSwPre = nSwPost = (USHORT)( ( n - 100 )
                                                    * pH->GetHeight() / 100 );
    }
    else                            // Fixed / Minimum
    {
        // bei negativen Space ist der Abstand exakt, sonst minimum
        nSpaceTw = (USHORT)nSpace;
        aLSpc.SetLineHeight( nSpaceTw );
        aLSpc.GetLineSpaceRule() = eLnSpc;
        nSwPre = nSpace;

        if( SVX_LINE_SPACE_FIX == eLnSpc )                  // Genau
        {
            nWwPre = (USHORT)( 8L * nSpace / 10 );
            nWwPost = (USHORT)( 2L * nSpace / 10 );
            nSwPre = nSpace;
        }
        else                                                // Minimum
        {
            nWwPre = (USHORT)( 129L * nSpace / 100 - 95 );// erst bei groesseren
                                                          // Zeilenabstaenden
        }
    }
    NewAttr( aLSpc );
    if( pSFlyPara )
        pSFlyPara->nLineSpace = nSpaceTw;   // LineSpace fuer Graf-Apos
}

//#i18519# AutoSpace value depends on Dop fDontUseHTMLAutoSpacing setting
sal_uInt16 SwWW8ImplReader::GetParagraphAutoSpace(bool fDontUseHTMLAutoSpacing)
{
    if (fDontUseHTMLAutoSpacing)
        return 100;  //Seems to be always 5points in this case
    else
        return 280;  //Seems to be always 14points in this case
}

void SwWW8ImplReader::Read_DontAddEqual(USHORT, const BYTE *pData, short nLen)
{
    if (nLen < 0)
        return;

    if (*pData)
        maTracer.Log(sw::log::eDontAddSpaceForEqualStyles);
}

void SwWW8ImplReader::Read_ParaAutoBefore(USHORT, const BYTE *pData, short nLen)
{
    if (nLen < 0)
    {
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_UL_SPACE);
        return;
    }

    if (*pData)
    {
        SvxULSpaceItem aUL(*(const SvxULSpaceItem*)GetFmtAttr(RES_UL_SPACE));
        aUL.SetUpper(GetParagraphAutoSpace(pWDop->fDontUseHTMLAutoSpacing));
        NewAttr(aUL);
        if (pAktColl)
            pCollA[nAktColl].bParaAutoBefore = true;
        else
            bParaAutoBefore = true;
    }
    else
    {
        if (pAktColl)
            pCollA[nAktColl].bParaAutoBefore = false;
        else
            bParaAutoBefore = false;
    }
}

void SwWW8ImplReader::Read_ParaAutoAfter(USHORT, const BYTE *pData, short nLen)
{
    if (nLen < 0)
    {
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_UL_SPACE);
        return;
    }

    if (*pData)
    {
        SvxULSpaceItem aUL(*(const SvxULSpaceItem*)GetFmtAttr(RES_UL_SPACE));
        aUL.SetLower(GetParagraphAutoSpace(pWDop->fDontUseHTMLAutoSpacing));
        NewAttr(aUL);
        if (pAktColl)
            pCollA[nAktColl].bParaAutoAfter = true;
        else
            bParaAutoAfter = true;
    }
    else
    {
        if (pAktColl)
            pCollA[nAktColl].bParaAutoAfter = false;
        else
            bParaAutoAfter = false;
    }
}

// Sprm 21, 22
void SwWW8ImplReader::Read_UL( USHORT nId, const BYTE* pData, short nLen )
{
// Nun eine Umpopelung eines WW-Fehlers: Bei nProduct == 0c03d wird
// faelschlicherweise ein DyaAfter 240 ( delta y abstand after, amn.d.?b.)
// im Style "Normal" eingefuegt, der
// gar nicht da ist. Ueber das IniFlag WW8FL_NO_STY_DYA laesst sich dieses
// Verhalten auch fuer andere WW-Versionen erzwingen
//  ASSERT( !bStyNormal || bWWBugNormal, "+Dieses Doc deutet evtl. auf einen
// Fehler in der benutzten WW-Version hin. Wenn sich die Styles <Standard> bzw.
// <Normal> zwischen WW und SW im Absatz- oder Zeilenabstand unterscheiden,
// dann bitte dieses Doc SH zukommen lassen." );
// bWWBugNormal ist kein hinreichendes Kriterium dafuer, dass der
// angegebene Abstand falsch ist

    if( nLen < 0 )
    {
        // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_UL_SPACE );
        return;
    }
    short nPara = SVBT16ToShort( pData );
    if( nPara < 0 )
        nPara = -nPara;

    SvxULSpaceItem aUL( *(const SvxULSpaceItem*)GetFmtAttr( RES_UL_SPACE ));

    switch( nId )
    {
        //sprmPDyaBefore
        case     21:
        case 0xA413:
            aUL.SetUpper( nPara );
            break;
        //sprmPDyaAfter
        case     22:
        case 0xA414:
            aUL.SetLower( nPara );
            break;
        default:
            return;
    };

    NewAttr( aUL );
}

void SwWW8ImplReader::Read_IdctHint( USHORT, const BYTE* pData, short nLen )
{
    if (nLen < 0)
        nIdctHint = 0;
    else
        nIdctHint = *pData;
}

void SwWW8ImplReader::Read_Justify( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_ADJUST );
        return;
    }

    SvxAdjust eAdjust(SVX_ADJUST_LEFT);
    bool bDistributed = false;
    switch (*pData)
    {
        default:
        case 0:
            break;
        case 1:
            eAdjust = SVX_ADJUST_CENTER;
            break;
        case 2:
            eAdjust = SVX_ADJUST_RIGHT;
            break;
        case 3:
            eAdjust = SVX_ADJUST_BLOCK;
            break;
        case 4:
            eAdjust = SVX_ADJUST_BLOCK;
            bDistributed = true;
            break;
    }
    SvxAdjustItem aAdjust(eAdjust, RES_PARATR_ADJUST);
    if (bDistributed)
        aAdjust.SetLastBlock(SVX_ADJUST_BLOCK);

    NewAttr(aAdjust);
}

bool SwWW8ImplReader::IsRightToLeft()
{
    bool bRTL = false;
    const BYTE *pDir =
        pPlcxMan ? pPlcxMan->GetPapPLCF()->HasSprm(0x2441) : 0;
    if (pDir)
        bRTL = *pDir ? true : false;
    else
    {
        const SvxFrameDirectionItem* pItem=
            (const SvxFrameDirectionItem*)GetFmtAttr(RES_FRAMEDIR);
        if (pItem && (pItem->GetValue() == FRMDIR_HORI_RIGHT_TOP))
            bRTL = true;
    }
    return bRTL;
}

void SwWW8ImplReader::Read_RTLJustify( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_ADJUST );
        return;
    }

    //If we are in a ltr paragraph this is the same as normal Justify,
    //If we are in a rtl paragraph the meaning is reversed.
    if (!IsRightToLeft())
        Read_Justify(0x2403 /*dummy*/, pData, nLen);
    else
    {
        SvxAdjust eAdjust(SVX_ADJUST_RIGHT);
        bool bDistributed = false;
        switch (*pData)
        {
            default:
            case 0:
                break;
            case 1:
                eAdjust = SVX_ADJUST_CENTER;
                break;
            case 2:
                eAdjust = SVX_ADJUST_LEFT;
                break;
            case 3:
                eAdjust = SVX_ADJUST_BLOCK;
                break;
            case 4:
                eAdjust = SVX_ADJUST_BLOCK;
                bDistributed = true;
                break;
        }
        SvxAdjustItem aAdjust(eAdjust, RES_PARATR_ADJUST);
        if (bDistributed)
            aAdjust.SetLastBlock(SVX_ADJUST_BLOCK);

        NewAttr(aAdjust);
    }
}

void SwWW8ImplReader::Read_BoolItem( USHORT nId, const BYTE* pData, short nLen )
{
    switch( nId )
    {
        case 0x2433:
            nId = RES_PARATR_FORBIDDEN_RULES;
            break;
        case 0x2435:
            nId = RES_PARATR_HANGINGPUNCTUATION;
            break;
        case 0x2437:
            nId = RES_PARATR_SCRIPTSPACE;
            break;
        default:
            ASSERT( !this, "wrong Id" );
            return ;
    }

    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), nId );
    else
    {
        SfxBoolItem* pI = (SfxBoolItem*)GetDfltAttr( nId )->Clone();
        pI->SetValue( 0 != *pData );
        NewAttr( *pI );
        delete pI;
    }
}

void SwWW8ImplReader::Read_Emphasis( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_EMPHASIS_MARK );
    else
    {
        LanguageType nLang;
        //Check to see if there is an up and coming cjk language property. If
        //there is use it, if there is not fall back to the currently set one.
        //Only the cjk language setting seems to matter to word, the western
        //one is ignored
        const BYTE *pLang =
            pPlcxMan ? pPlcxMan->GetChpPLCF()->HasSprm(0x486E) : 0;

        if (pLang)
            nLang = SVBT16ToShort( pLang );
        else
        {
            nLang = ((const SvxLanguageItem *)
                GetFmtAttr(RES_CHRATR_CJK_LANGUAGE))->GetLanguage();
        }

        sal_uInt16 nVal;
        switch( *pData )
        {
        case 0:
            nVal = EMPHASISMARK_NONE;
            break;
        case 2:
            if ((nLang == LANGUAGE_CHINESE_HONGKONG) ||
                (nLang == LANGUAGE_CHINESE_MACAU) ||
                (nLang == LANGUAGE_CHINESE_TRADITIONAL) ||
                (nLang == LANGUAGE_KOREAN))
                nVal = EMPHASISMARK_CIRCLE_ABOVE;
            else if (nLang == LANGUAGE_JAPANESE)
                nVal = EMPHASISMARK_SIDE_DOTS;
            else
                nVal = EMPHASISMARK_DOTS_BELOW;
            break;
        case 3:
            nVal = EMPHASISMARK_CIRCLE_ABOVE;
            break;
        case 4:
            nVal = EMPHASISMARK_DOTS_BELOW;
            break;
        case 1:
            if ((nLang == LANGUAGE_CHINESE_SIMPLIFIED) ||
                (nLang == LANGUAGE_CHINESE_SINGAPORE))
                nVal = EMPHASISMARK_DOTS_BELOW;
            else
                nVal = EMPHASISMARK_DOTS_ABOVE;
            break;
        default:
            nVal = EMPHASISMARK_DOTS_ABOVE;
            break;
        }

        NewAttr( SvxEmphasisMarkItem( nVal, RES_CHRATR_EMPHASIS_MARK ) );
    }
}

void SwWW8ImplReader::Read_ScaleWidth( USHORT, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_SCALEW );
    else
    {
        sal_uInt16 nVal = SVBT16ToShort( pData );
        //The number must be between 1 and 600
        if (nVal < 1 || nVal > 600)
            nVal = 100;
        NewAttr( SvxCharScaleWidthItem( nVal, RES_CHRATR_SCALEW ) );
    }
}

void SwWW8ImplReader::Read_Relief( USHORT nId, const BYTE* pData, short nLen )
{
    if( nLen < 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_RELIEF );
    else
    {
        if( *pData )
        {
// JP 16.03.2001 - not so eays because this is also a toggle attribute!
//  2 x emboss on -> no emboss !!!
// the actual value must be searched over the stack / template

            const SvxCharReliefItem* pOld = (const SvxCharReliefItem*)
                                            GetFmtAttr( RES_CHRATR_RELIEF );
            FontRelief nNewValue = 0x854 == nId ? RELIEF_ENGRAVED
                                        : ( 0x858 == nId ? RELIEF_EMBOSSED
                                                         : RELIEF_NONE );
            if( pOld->GetValue() == nNewValue )
            {
                if( RELIEF_NONE != nNewValue )
                    nNewValue = RELIEF_NONE;
            }
            NewAttr( SvxCharReliefItem( nNewValue, RES_CHRATR_RELIEF ));
        }
    }
}

void SwWW8ImplReader::Read_TxtAnim(USHORT /*nId*/, const BYTE* pData, short nLen)
{
    if (nLen < 0)
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_CHRATR_BLINK);
    else
    {
        if (*pData)
        {
            bool bBlink;

            // #110851# The 7 animated text effects available in word all get
            // mapped to a blinking text effect in StarOffice
            // 0 no animation       1 Las Vegas lights
            // 2 background blink   3 sparkle text
            // 4 marching ants      5 marchine red ants
            // 6 shimmer
            if (*pData > 0 && *pData < 7 )
                bBlink = true;
            else
                bBlink = false;

            NewAttr(SvxBlinkItem(bBlink, RES_CHRATR_BLINK));
        }
    }
}

SwWW8Shade::SwWW8Shade(bool bVer67, const WW8_SHD& rSHD)
{
    BYTE b = rSHD.GetFore();
    ASSERT(b < 17, "ww8: colour out of range");
    if (b >= 17)
        b = 0;

    ColorData nFore(SwWW8ImplReader::GetCol(b));

    b = rSHD.GetBack();
    ASSERT(b < 17, "ww8: colour out of range");
    if( b >=  17 )
        b = 0;

    ColorData nBack(SwWW8ImplReader::GetCol(b));

    b = rSHD.GetStyle(bVer67);

    SetShade(nFore, nBack, b);
}

void SwWW8Shade::SetShade(ColorData nFore, ColorData nBack, sal_uInt16 nIndex)
{
    static const ULONG eMSGrayScale[] =
    {
        // Nul-Brush
           0,   // 0
        // Solid-Brush
        1000,   // 1
        // promillemaessig abgestufte Schattierungen
          50,   // 2
         100,   // 3
         200,   // 4
         250,   // 5
         300,   // 6
         400,   // 7
         500,   // 8
         600,   // 9
         700,   // 10
         750,   // 11
         800,   // 12
         900,   // 13
         333, // 14 Dark Horizontal
         333, // 15 Dark Vertical
         333, // 16 Dark Forward Diagonal
         333, // 17 Dark Backward Diagonal
         333, // 18 Dark Cross
         333, // 19 Dark Diagonal Cross
         333, // 20 Horizontal
         333, // 21 Vertical
         333, // 22 Forward Diagonal
         333, // 23 Backward Diagonal
         333, // 24 Cross
         333, // 25 Diagonal Cross
         // neun Nummern ohne Bedeutung in Ver8
         500, // 26
         500, // 27
         500, // 28
         500, // 29
         500, // 30
         500, // 31
         500, // 32
         500, // 33
         500, // 34
         // und weiter gehts mit tollen Schattierungen ;-)
          25,   // 35
          75,   // 36
         125,   // 37
         150,   // 38
         175,   // 39
         225,   // 40
         275,   // 41
         325,   // 42
         350,   // 43
         375,   // 44
         425,   // 45
         450,   // 46
         475,   // 47
         525,   // 48
         550,   // 49
         575,   // 50
         625,   // 51
         650,   // 52
         675,   // 53
         725,   // 54
         775,   // 55
         825,   // 56
         850,   // 57
         875,   // 58
         925,   // 59
         950,   // 60
         975,   // 61
         // und zu guter Letzt:
         970
    };// 62


    //NO auto for shading so Foreground: Auto = Black
    if (nFore == COL_AUTO)
        nFore = COL_BLACK;

    //NO auto for shading so background: Auto = Weiss
    ColorData nUseBack = nBack;
    if (nUseBack == COL_AUTO)
        nUseBack = COL_WHITE;


    if( nIndex >= sizeof( eMSGrayScale ) / sizeof ( eMSGrayScale[ 0 ] ) )
        nIndex = 0;

    ULONG nWW8BrushStyle = eMSGrayScale[nIndex];

    switch (nWW8BrushStyle)
    {
        case 0: // Null-Brush
            aColor.SetColor( nBack );
            break;
        default:
            {
                Color aForeColor(nFore);
                Color aBackColor(nUseBack);

                sal_uInt32 nRed = aForeColor.GetRed() * nWW8BrushStyle;
                sal_uInt32 nGreen = aForeColor.GetGreen() * nWW8BrushStyle;
                sal_uInt32 nBlue = aForeColor.GetBlue() * nWW8BrushStyle;
                nRed += aBackColor.GetRed()  * (1000L - nWW8BrushStyle);
                nGreen += aBackColor.GetGreen()* (1000L - nWW8BrushStyle);
                nBlue += aBackColor.GetBlue() * (1000L - nWW8BrushStyle);

                aColor.SetColor( RGB_COLORDATA( nRed/1000, nGreen/1000,
                    nBlue/1000 ) );
            }
            break;
    }
}

void SwWW8ImplReader::Read_Shade( USHORT, const BYTE* pData, short nLen )
{
    if (!bVer67 && pPlcxMan && pPlcxMan->GetPapPLCF()->HasSprm(0xC64D))
        return;

    if (nLen <= 0)
    {
        // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_BACKGROUND );
        if (bShdTxtCol)
        {
            // Zeichenfarbe auch
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
            bShdTxtCol = false;
        }
    }
    else
    {
        WW8_SHD aSHD;
        aSHD.SetWWValue( *(SVBT16*)pData );
        SwWW8Shade aSh( bVer67, aSHD );

        NewAttr(SvxBrushItem(aSh.aColor, RES_BACKGROUND));
    }
}

void SwWW8ImplReader::Read_ParaBackColor(USHORT, const BYTE* pData, short nLen)
{
    if (nLen <= 0)
    {
        // Ende des Attributes
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_BACKGROUND );
        if (bShdTxtCol)
        {
            // Zeichenfarbe auch
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_COLOR );
            bShdTxtCol = false;
        }
    }
    else
    {
        ASSERT(nLen == 10, "Len of para back colour not 10!");
        if (nLen != 10)
            return;
        NewAttr(SvxBrushItem(Color(ExtractColour(pData, bVer67)), RES_BACKGROUND));
    }
}

sal_uInt32 SwWW8ImplReader::ExtractColour(const BYTE* &rpData,
    bool
#ifdef DBG_UTIL
        bVer67
#endif
    )
{
    ASSERT(bVer67 == false, "Impossible");
    //ASSERT(SVBT32ToUInt32(rpData) == 0xFF000000, "Unknown 1 not 0xff000000");
    sal_uInt32 nFore = wwUtility::BGRToRGB(SVBT32ToUInt32(rpData));
    rpData+=4;
    sal_uInt32 nBack = wwUtility::BGRToRGB(SVBT32ToUInt32(rpData));
    rpData+=4;
    sal_uInt16 nIndex = SVBT16ToShort(rpData);
    rpData+=2;
    //Being a transparent background colour doesn't actually show the page
    //background through, it merely acts like white
    if (nBack == 0xFF000000)
        nBack = COL_AUTO;
    ASSERT(nBack == COL_AUTO || !(nBack & 0xFF000000),
        "ww8: don't know what to do with such a transparent bg colour, report");
    SwWW8Shade aShade(nFore, nBack, nIndex);
    return aShade.aColor.GetColor();
}

void SwWW8ImplReader::Read_Border(USHORT , const BYTE* , short nLen)
{
    if( nLen < 0 )
    {
        if( bHasBorder )
        {
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_BOX );
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_SHADOW );
            bHasBorder = false;
        }
    }
    else if( !bHasBorder )
    {
        // die Borders auf allen 4 Seiten werden gebuendelt.  dieses
        // vereinfacht die Verwaltung, d.h. die Box muss nicht 4 mal auf den
        // CtrlStack und wieder runter
        bHasBorder = true;

        WW8_BRC5 aBrcs;   // Top, Left, Bottom, Right, Between
        BYTE nBorder;

        if( pAktColl )
            nBorder = ::lcl_ReadBorders(bVer67, aBrcs, 0, pStyles);
        else
            nBorder = ::lcl_ReadBorders(bVer67, aBrcs, pPlcxMan->GetPapPLCF());

        if( nBorder )                                   // Border
        {
            bool bIsB = IsBorder(aBrcs, true);
            if (!InLocalApo() || !bIsB ||
                (pWFlyPara && !pWFlyPara->bBorderLines ))
            {
                // in Apo keine Umrandungen *ein*-schalten, da ich
                // sonst die Flyumrandungen doppelt bekomme
                // JP 04.12.98: aber nur wenn am Fly ein gesetzt ist, keine
                //              uebernehmen. Sonst wird gar keine gesetzt!
                //              Bug #59619#

                // auch wenn kein Rand gesetzt ist, muss das Attribut gesetzt
                // werden, sonst ist kein hartes Ausschalten von Style-Attrs
                // moeglich
                const SvxBoxItem* pBox
                    = (const SvxBoxItem*)GetFmtAttr( RES_BOX );
                SvxBoxItem aBox(RES_BOX);
                if (pBox)
                    aBox = *pBox;
                short aSizeArray[5]={0};

                SetBorder(aBox, aBrcs, &aSizeArray[0], nBorder);

                Rectangle aInnerDist;
                GetBorderDistance( aBrcs, aInnerDist );

                maTracer.Log(sw::log::eBorderDistOutside);

                aBox.SetDistance( (USHORT)aInnerDist.Left(), BOX_LINE_LEFT );
                aBox.SetDistance( (USHORT)aInnerDist.Top(), BOX_LINE_TOP );
                aBox.SetDistance( (USHORT)aInnerDist.Right(), BOX_LINE_RIGHT );
                aBox.SetDistance( (USHORT)aInnerDist.Bottom(), BOX_LINE_BOTTOM );

                NewAttr( aBox );

                SvxShadowItem aS(RES_SHADOW);
                if( SetShadow( aS, &aSizeArray[0], aBrcs ) )
                    NewAttr( aS );
            }
        }
    }
}

void SwWW8ImplReader::Read_Hyphenation( USHORT, const BYTE* pData, short nLen )
{
    // set Hyphenation flag
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_HYPHENZONE );
    else
    {
        SvxHyphenZoneItem aAttr(
            *(const SvxHyphenZoneItem*)GetFmtAttr( RES_PARATR_HYPHENZONE ) );

        aAttr.SetHyphen( 0 == *pData ); // sic !

        if( !*pData )
        {
            aAttr.GetMinLead()    = 2;
            aAttr.GetMinTrail()   = 2;
            aAttr.GetMaxHyphens() = 0;
        }

        NewAttr( aAttr );
    }
}

void SwWW8ImplReader::Read_WidowControl( USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
    {
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_WIDOWS );
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_ORPHANS );
    }
    else
    {
        BYTE nL = ( *pData & 1 ) ? 2 : 0;

        NewAttr( SvxWidowsItem( nL, RES_PARATR_WIDOWS ) );     // Aus -> nLines = 0
        NewAttr( SvxOrphansItem( nL, RES_PARATR_ORPHANS ) );

        if( pAktColl && pStyles )           // Style-Def ?
            pStyles->bWidowsChanged = true; // merken zur Simulation
                                            // Default-Widows
    }
}

void SwWW8ImplReader::Read_UsePgsuSettings(USHORT,const BYTE* pData,short nLen)
{
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_SNAPTOGRID);
    else
    {
        if(nInTable)
            NewAttr( SvxParaGridItem(false, RES_PARATR_SNAPTOGRID) );
        else
            NewAttr( SvxParaGridItem(*pData, RES_PARATR_SNAPTOGRID) );
    }
}

void SwWW8ImplReader::Read_AlignFont( USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_VERTALIGN);
    else
    {
        sal_uInt16 nVal = SVBT16ToShort( pData );
        switch (nVal)
        {
            case 0:
                nVal = SvxParaVertAlignItem::TOP;
                break;
            case 1:
                nVal = SvxParaVertAlignItem::CENTER;
                break;
            case 2:
                nVal = SvxParaVertAlignItem::BASELINE;
                break;
            case 3:
                nVal = SvxParaVertAlignItem::BOTTOM;
                break;
            case 4:
                nVal = SvxParaVertAlignItem::AUTOMATIC;
                break;
            default:
                nVal = SvxParaVertAlignItem::AUTOMATIC;
                ASSERT(!this,"Unknown paragraph vertical align");
                break;
        }
        NewAttr( SvxParaVertAlignItem( nVal, RES_PARATR_VERTALIGN ) );
    }
}

void SwWW8ImplReader::Read_KeepLines( USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_PARATR_SPLIT );
    else
        NewAttr( SvxFmtSplitItem( ( *pData & 1 ) == 0, RES_PARATR_SPLIT ) );
}

void SwWW8ImplReader::Read_KeepParas( USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_KEEP );
    else
        NewAttr( SvxFmtKeepItem( ( *pData & 1 ) != 0 , RES_KEEP) );
}

void SwWW8ImplReader::Read_BreakBefore( USHORT, const BYTE* pData, short nLen )
{
    if( nLen <= 0 )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_BREAK );
    else
        NewAttr( SvxFmtBreakItem(
                ( *pData & 1 ) ? SVX_BREAK_PAGE_BEFORE : SVX_BREAK_NONE, RES_BREAK ) );
}

void SwWW8ImplReader::Read_ApoPPC( USHORT, const BYTE* pData, short )
{
    if (pAktColl) // only for Styledef, sonst anders geloest
    {
        SwWW8StyInf& rSI = pCollA[nAktColl];
        WW8FlyPara* pFly = rSI.pWWFly ? rSI.pWWFly : new WW8FlyPara(bVer67);
        pCollA[nAktColl].pWWFly = pFly;
        pFly->Read(pData, pStyles);
        if (pFly->IsEmpty())
            delete pCollA[nAktColl].pWWFly, pCollA[nAktColl].pWWFly = 0;
    }
}

bool SwWW8ImplReader::ParseTabPos(WW8_TablePos *pTabPos, WW8PLCFx_Cp_FKP* pPap)
{
    bool bRet = false;
    const BYTE *pRes=0;
    memset(pTabPos, 0, sizeof(WW8_TablePos));
    if (0 != (pRes = pPap->HasSprm(0x360D)))
    {
        pTabPos->nSp29 = *pRes;
        pTabPos->nSp37 = 2;     //Possible fail area, always parallel wrap
        if (0 != (pRes = pPap->HasSprm(0x940E)))
            pTabPos->nSp26 = SVBT16ToShort(pRes);
        if (0 != (pRes = pPap->HasSprm(0x940F)))
            pTabPos->nSp27 = SVBT16ToShort(pRes);
        if (0 != (pRes = pPap->HasSprm(0x9410)))
            pTabPos->nLeMgn = SVBT16ToShort(pRes);
        if (0 != (pRes = pPap->HasSprm(0x941E)))
            pTabPos->nRiMgn = SVBT16ToShort(pRes);
        if (0 != (pRes = pPap->HasSprm(0x9411)))
            pTabPos->nUpMgn = SVBT16ToShort(pRes);
        if (0 != (pRes = pPap->HasSprm(0x941F)))
            pTabPos->nLoMgn = SVBT16ToShort(pRes);
        bRet = true;
    }
    return bRet;
}

/***************************************************************************
#  Seiten - Attribute werden nicht mehr als Attribute gehandhabt
#   ( ausser OLST )
#**************************************************************************/


long SwWW8ImplReader::ImportExtSprm(WW8PLCFManResult* pRes)
{
    /*************************************************************************
    #       Arrays zum Lesen der erweiterten ( selbstdefinierten ) SPRMs
    #*************************************************************************/
    typedef long (SwWW8ImplReader:: *FNReadRecordExt)(WW8PLCFManResult*);

    static const FNReadRecordExt aWwSprmTab[] =
    {
        /* 0 (256) */   &SwWW8ImplReader::Read_Ftn,     // FootNote
        /* 1 (257) */   &SwWW8ImplReader::Read_Ftn,     // EndNote
        /* 2 (258) */   &SwWW8ImplReader::Read_Field,  // Feld
        /* 3 (259) */   &SwWW8ImplReader::Read_Book,   // Bookmark
        /* 4 (260) */   &SwWW8ImplReader::Read_And     // Annotation
    };

    if( pRes->nSprmId < 280 )
    {
        BYTE nIdx = static_cast< BYTE >(pRes->nSprmId - eFTN);
        if( nIdx < sizeof( aWwSprmTab ) / sizeof( *aWwSprmTab )
            && aWwSprmTab[nIdx] )
            return (this->*aWwSprmTab[nIdx])(pRes);
        else
            return 0;
    }
    else
        return 0;
}

void SwWW8ImplReader::EndExtSprm(USHORT nSprmId)
{
    typedef sal_uInt16 (SwWW8ImplReader:: *FNReadRecordExt)();

    static const FNReadRecordExt aWwSprmTab[] =
    {
        /* 0 (256) */   &SwWW8ImplReader::End_Ftn,      // FootNote
        /* 1 (257) */   &SwWW8ImplReader::End_Ftn,      // EndNote
        /* 2 (258) */   &SwWW8ImplReader::End_Field,  // Feld
        /* 3 (259) */   0,   // Bookmark
        /* 4 (260) */   0     // Annotation
    };

    BYTE nIdx = static_cast< BYTE >(nSprmId - eFTN);
    if( nIdx < sizeof( aWwSprmTab ) / sizeof( *aWwSprmTab )
        && aWwSprmTab[nIdx] )
        (this->*aWwSprmTab[nIdx])();
}

/***************************************************************************
#       Arrays zum Lesen der SPRMs
#**************************************************************************/

// Funktion zum Einlesen von Sprms. Par1: SprmId
typedef void (SwWW8ImplReader:: *FNReadRecord)( USHORT, const BYTE*, short );

struct SprmReadInfo
{
    USHORT       nId;
    FNReadRecord pReadFnc;
};

bool operator==(const SprmReadInfo &rFirst, const SprmReadInfo &rSecond)
{
    return (rFirst.nId == rSecond.nId);
}

bool operator<(const SprmReadInfo &rFirst, const SprmReadInfo &rSecond)
{
    return (rFirst.nId < rSecond.nId);
}

typedef ww::SortedArray<SprmReadInfo> wwSprmDispatcher;

const wwSprmDispatcher *GetWW2SprmDispatcher()
{
    static SprmReadInfo aSprms[] =
    {
          {0, 0},                                    // "0" Default bzw. Error
                                                     //wird uebersprungen! ,
          {2, &SwWW8ImplReader::Read_StyleCode},     //"sprmPIstd",  pap.istd
                                                     //(style code)
          {3, 0},                                    //"sprmPIstdPermute", pap.istd
                                                     //permutation
          {4, 0},                                    //"sprmPIncLv1",
                                                     //pap.istddifference
          {5, &SwWW8ImplReader::Read_Justify},       //"sprmPJc", pap.jc
                                                     //(justification)
          {6, 0},                                    //"sprmPFSideBySide",
                                                     //pap.fSideBySide
          {7, &SwWW8ImplReader::Read_KeepLines},     //"sprmPFKeep", pap.fKeep
          {8, &SwWW8ImplReader::Read_KeepParas},     //"sprmPFKeepFollow ",
                                                     //pap.fKeepFollow
          {9, &SwWW8ImplReader::Read_BreakBefore},   //"sprmPPageBreakBefore",
                                                     //pap.fPageBreakBefore
         {10, 0},                                    //"sprmPBrcl", pap.brcl
         {11, 0},                                    //"sprmPBrcp ", pap.brcp
         {12, &SwWW8ImplReader::Read_ANLevelDesc},   //"sprmPAnld", pap.anld (ANLD
                                                     //structure)
         {13, &SwWW8ImplReader::Read_ANLevelNo},     //"sprmPNLvlAnm", pap.nLvlAnm
                                                     //nn
         {14, &SwWW8ImplReader::Read_NoLineNumb},    //"sprmPFNoLineNumb", ap.fNoLnn
         {15, &SwWW8ImplReader::Read_Tab},           //"?sprmPChgTabsPapx",
                                                     //pap.itbdMac, ...
         {16, &SwWW8ImplReader::Read_LR},            //"sprmPDxaRight", pap.dxaRight
         {17, &SwWW8ImplReader::Read_LR},            //"sprmPDxaLeft", pap.dxaLeft
         {18, 0},                                    //"sprmPNest", pap.dxaLeft
         {19, &SwWW8ImplReader::Read_LR},            //"sprmPDxaLeft1", pap.dxaLeft1
         {20, &SwWW8ImplReader::Read_LineSpace},     //"sprmPDyaLine", pap.lspd
                                                     //an LSPD
         {21, &SwWW8ImplReader::Read_UL},            //"sprmPDyaBefore",
                                                     //pap.dyaBefore
         {22, &SwWW8ImplReader::Read_UL},            //"sprmPDyaAfter", pap.dyaAfter
         {23, 0},                                    //"?sprmPChgTabs", pap.itbdMac,
                                                     //pap.rgdxaTab, ...
         {24, 0},                                    //"sprmPFInTable", pap.fInTable
         {25, &SwWW8ImplReader::Read_TabRowEnd},     //"sprmPTtp", pap.fTtp
         {26, 0},                                    //"sprmPDxaAbs", pap.dxaAbs
         {27, 0},                                    //"sprmPDyaAbs", pap.dyaAbs
         {28, 0},                                    //"sprmPDxaWidth", pap.dxaWidth
         {29, &SwWW8ImplReader::Read_ApoPPC},        //"sprmPPc", pap.pcHorz,
                                                     //pap.pcVert
         {30, 0},                                    //"sprmPBrcTop10", pap.brcTop
                                                     //BRC10
         {31, 0},                                    //"sprmPBrcLeft10",
                                                     //pap.brcLeft BRC10
         {32, 0},                                    //"sprmPBrcBottom10",
                                                     //pap.brcBottom BRC10
         {33, 0},                                    //"sprmPBrcRight10",
                                                     //pap.brcRight BRC10
         {34, 0},                                    //"sprmPBrcBetween10",
                                                     //pap.brcBetween BRC10
         {35, 0},                                    //"sprmPBrcBar10", pap.brcBar
                                                     //BRC10
         {36, 0},                                    //"sprmPFromText10",
                                                     //pap.dxaFromText dxa
         {37, 0},                                    //"sprmPWr", pap.wr wr
         {38, &SwWW8ImplReader::Read_Border},        //"sprmPBrcTop", pap.brcTop BRC
         {39, &SwWW8ImplReader::Read_Border},        //"sprmPBrcLeft",
                                                     //pap.brcLeft BRC
         {40, &SwWW8ImplReader::Read_Border},        //"sprmPBrcBottom",
                                                     //pap.brcBottom BRC
         {41, &SwWW8ImplReader::Read_Border},        //"sprmPBrcRight",
                                                     //pap.brcRight BRC
         {42, &SwWW8ImplReader::Read_Border},        //"sprmPBrcBetween",
                                                     //pap.brcBetween BRC
         {43, 0},                                    //"sprmPBrcBar", pap.brcBar
                                                     //BRC word
         {44, &SwWW8ImplReader::Read_Hyphenation},   //"sprmPFNoAutoHyph",
                                                     //pap.fNoAutoHyph
         {45, 0},                                    //"sprmPWHeightAbs",
                                                     //pap.wHeightAbs w
         {46, 0},                                    //"sprmPDcs", pap.dcs DCS
         {47, &SwWW8ImplReader::Read_Shade},         //"sprmPShd", pap.shd SHD
         {48, 0},                                    //"sprmPDyaFromText",
                                                     //pap.dyaFromText dya
         {49, 0},                                    //"sprmPDxaFromText",
                                                     //pap.dxaFromText dxa
         {50, 0},                                    //"sprmPFLocked", pap.fLocked
                                                     //0 or 1 byte
         {51, &SwWW8ImplReader::Read_WidowControl},  //"sprmPFWidowControl",
                                                     //pap.fWidowControl 0 or 1 byte
         {52, 0},                                    //"?sprmPRuler 52",
         {53, 0},                                    //"??53",
         {54, 0},                                    //"??54",
         {55, 0},                                    //"??55",
         {56, 0},                                    //"??56",
         {57, 0},                                    //"??57",
         {58, 0},                                    //"??58",
         {59, 0},                                    //"??59",

         {60, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFBold", chp.fBold 0,1,
                                                     //128, or 129 byte
         {61, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFItalic", chp.fItalic
                                                     //0,1, 128, or 129 byte
         {62, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFStrike", chp.fStrike
                                                     //0,1, 128, or 129 byte
         {63, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFOutline", chp.fOutline
                                                     //0,1, 128, or 129 byte
         {64, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFShadow", chp.fShadow
                                                     //0,1, 128, or 129 byte
         {65, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFSmallCaps",
                                                     //chp.fSmallCaps 0,1, 128, or
                                                     //129 byte
         {66, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFCaps", chp.fCaps 0,1,
                                                     //128, or 129 byte
         {67, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFVanish", chp.fVanish
                                                     //0,1, 128, or 129 byte
         {68, &SwWW8ImplReader::Read_FontCode},      //"sprmCFtc", chp.ftc ftc word
         {69, &SwWW8ImplReader::Read_Underline},     // "sprmCKul", chp.kul kul byte
         {70, 0},                                    //"sprmCSizePos", chp.hps,
                                                     //chp.hpsPos 3 bytes
         {71, &SwWW8ImplReader::Read_Kern},          //"sprmCDxaSpace",
                                                     //chp.dxaSpace dxa word
         {72, &SwWW8ImplReader::Read_Language},      //"sprmCLid", chp.lid LID word
         {73, &SwWW8ImplReader::Read_TxtColor},      //"sprmCIco", chp.ico ico byte
         {74, &SwWW8ImplReader::Read_FontSize},      //"sprmCHps", chp.hps hps word!
         {75, 0},                                    //"sprmCHpsInc", chp.hps byte
         {76, &SwWW8ImplReader::Read_SubSuperProp},  //"sprmCHpsPos", chp.hpsPos
                                                     //hps byte
         {77, 0},                                    //"sprmCHpsPosAdj", chp.hpsPos
                                                     //hps byte
         {78, &SwWW8ImplReader::Read_Majority},      //"?sprmCMajority", chp.fBold,
                                                     //chp.fItalic, chp.fSmallCaps
         {80, &SwWW8ImplReader::Read_BoldBiDiUsw},   //sprmCFBoldBi
         {81, &SwWW8ImplReader::Read_BoldBiDiUsw},   //sprmCFItalicBi
         {82, &SwWW8ImplReader::Read_FontCode},      //sprmCFtcBi
         {83, &SwWW8ImplReader::Read_Language},      //sprmClidBi
         {84, &SwWW8ImplReader::Read_TxtColor},      //sprmCIcoBi
         {85, &SwWW8ImplReader::Read_FontSize},      //sprmCHpsBi
         {86, 0},                                    //sprmCFBiDi
         {87, 0},                                    //sprmCFDiacColor
         {94, 0},                                    //"sprmPicBrcl", pic.brcl brcl
                                                     //(see PIC structure
                                                     //definition) byte
         {95, 0},                                    //"sprmPicScale", pic.mx,
                                                     //pic.my, pic.dxaCropleft,
         {96, 0},                                    //"sprmPicBrcTop", pic.brcTop
                                                     //BRC word
         {97, 0},                                    //"sprmPicBrcLeft",
                                                     //pic.brcLeft BRC word
         {98, 0},                                    //"sprmPicBrcBottom",
                                                     //pic.brcBottom BRC word
         {99, 0}                                     //"sprmPicBrcRight",
    };

    static wwSprmDispatcher aSprmSrch(aSprms, sizeof(aSprms) / sizeof(aSprms[0]));
    return &aSprmSrch;
}

const wwSprmDispatcher *GetWW6SprmDispatcher()
{
    static SprmReadInfo aSprms[] =
    {
          {0, 0},                                    // "0" Default bzw. Error
                                                     //wird uebersprungen! ,
          {2, &SwWW8ImplReader::Read_StyleCode},     //"sprmPIstd",  pap.istd
                                                     //(style code)
          {3, 0},                                    //"sprmPIstdPermute", pap.istd
                                                     //permutation
          {4, 0},                                    //"sprmPIncLv1",
                                                     //pap.istddifference
          {5, &SwWW8ImplReader::Read_Justify},       //"sprmPJc", pap.jc
                                                     //(justification)
          {6, 0},                                    //"sprmPFSideBySide",
                                                     //pap.fSideBySide
          {7, &SwWW8ImplReader::Read_KeepLines},     //"sprmPFKeep", pap.fKeep
          {8, &SwWW8ImplReader::Read_KeepParas},     //"sprmPFKeepFollow ",
                                                     //pap.fKeepFollow
          {9, &SwWW8ImplReader::Read_BreakBefore},   //"sprmPPageBreakBefore",
                                                     //pap.fPageBreakBefore
         {10, 0},                                    //"sprmPBrcl", pap.brcl
         {11, 0},                                    //"sprmPBrcp ", pap.brcp
         {12, &SwWW8ImplReader::Read_ANLevelDesc},   //"sprmPAnld", pap.anld (ANLD
                                                     //structure)
         {13, &SwWW8ImplReader::Read_ANLevelNo},     //"sprmPNLvlAnm", pap.nLvlAnm
                                                     //nn
         {14, &SwWW8ImplReader::Read_NoLineNumb},    //"sprmPFNoLineNumb", ap.fNoLnn
         {15, &SwWW8ImplReader::Read_Tab},           //"?sprmPChgTabsPapx",
                                                     //pap.itbdMac, ...
         {16, &SwWW8ImplReader::Read_LR},            //"sprmPDxaRight", pap.dxaRight
         {17, &SwWW8ImplReader::Read_LR},            //"sprmPDxaLeft", pap.dxaLeft
         {18, 0},                                    //"sprmPNest", pap.dxaLeft
         {19, &SwWW8ImplReader::Read_LR},            //"sprmPDxaLeft1", pap.dxaLeft1
         {20, &SwWW8ImplReader::Read_LineSpace},     //"sprmPDyaLine", pap.lspd
                                                     //an LSPD
         {21, &SwWW8ImplReader::Read_UL},            //"sprmPDyaBefore",
                                                     //pap.dyaBefore
         {22, &SwWW8ImplReader::Read_UL},            //"sprmPDyaAfter", pap.dyaAfter
         {23, 0},                                    //"?sprmPChgTabs", pap.itbdMac,
                                                     //pap.rgdxaTab, ...
         {24, 0},                                    //"sprmPFInTable", pap.fInTable
         {25, &SwWW8ImplReader::Read_TabRowEnd},     //"sprmPTtp", pap.fTtp
         {26, 0},                                    //"sprmPDxaAbs", pap.dxaAbs
         {27, 0},                                    //"sprmPDyaAbs", pap.dyaAbs
         {28, 0},                                    //"sprmPDxaWidth", pap.dxaWidth
         {29, &SwWW8ImplReader::Read_ApoPPC},        //"sprmPPc", pap.pcHorz,
                                                     //pap.pcVert
         {30, 0},                                    //"sprmPBrcTop10", pap.brcTop
                                                     //BRC10
         {31, 0},                                    //"sprmPBrcLeft10",
                                                     //pap.brcLeft BRC10
         {32, 0},                                    //"sprmPBrcBottom10",
                                                     //pap.brcBottom BRC10
         {33, 0},                                    //"sprmPBrcRight10",
                                                     //pap.brcRight BRC10
         {34, 0},                                    //"sprmPBrcBetween10",
                                                     //pap.brcBetween BRC10
         {35, 0},                                    //"sprmPBrcBar10", pap.brcBar
                                                     //BRC10
         {36, 0},                                    //"sprmPFromText10",
                                                     //pap.dxaFromText dxa
         {37, 0},                                    //"sprmPWr", pap.wr wr
         {38, &SwWW8ImplReader::Read_Border},        //"sprmPBrcTop", pap.brcTop BRC
         {39, &SwWW8ImplReader::Read_Border},        //"sprmPBrcLeft",
                                                     //pap.brcLeft BRC
         {40, &SwWW8ImplReader::Read_Border},        //"sprmPBrcBottom",
                                                     //pap.brcBottom BRC
         {41, &SwWW8ImplReader::Read_Border},        //"sprmPBrcRight",
                                                     //pap.brcRight BRC
         {42, &SwWW8ImplReader::Read_Border},        //"sprmPBrcBetween",
                                                     //pap.brcBetween BRC
         {43, 0},                                    //"sprmPBrcBar", pap.brcBar
                                                     //BRC word
         {44, &SwWW8ImplReader::Read_Hyphenation},   //"sprmPFNoAutoHyph",
                                                     //pap.fNoAutoHyph
         {45, 0},                                    //"sprmPWHeightAbs",
                                                     //pap.wHeightAbs w
         {46, 0},                                    //"sprmPDcs", pap.dcs DCS
         {47, &SwWW8ImplReader::Read_Shade},         //"sprmPShd", pap.shd SHD
         {48, 0},                                    //"sprmPDyaFromText",
                                                     //pap.dyaFromText dya
         {49, 0},                                    //"sprmPDxaFromText",
                                                     //pap.dxaFromText dxa
         {50, 0},                                    //"sprmPFLocked", pap.fLocked
                                                     //0 or 1 byte
         {51, &SwWW8ImplReader::Read_WidowControl},  //"sprmPFWidowControl",
                                                     //pap.fWidowControl 0 or 1 byte
         {52, 0},                                    //"?sprmPRuler 52",
         {53, 0},                                    //"??53",
         {54, 0},                                    //"??54",
         {55, 0},                                    //"??55",
         {56, 0},                                    //"??56",
         {57, 0},                                    //"??57",
         {58, 0},                                    //"??58",
         {59, 0},                                    //"??59",
         {60, 0},                                    //"??60",
         {61, 0},                                    //"??61",
         {62, 0},                                    //"??62",
         {63, 0},                                    //"??63",
         {64, &SwWW8ImplReader::Read_ParaBiDi},      //"rtl bidi ?
         {65, &SwWW8ImplReader::Read_CFRMarkDel},    //"sprmCFStrikeRM",
                                                     //chp.fRMarkDel 1 or 0 bit
         {66, &SwWW8ImplReader::Read_CFRMark},       //"sprmCFRMark", chp.fRMark
                                                     //1 or 0 bit
         {67, &SwWW8ImplReader::Read_FldVanish},     //"sprmCFFldVanish",
                                                     //chp.fFldVanish 1 or 0 bit
         {68, &SwWW8ImplReader::Read_PicLoc},        //"sprmCPicLocation",
                                                     //chp.fcPic and chp.fSpec
         {69, 0},                                    //"sprmCIbstRMark",
                                                     //chp.ibstRMark index into
                                                     //sttbRMark
         {70, 0},                                    //"sprmCDttmRMark", chp.dttm
                                                     //DTTM long
         {71, 0},                                    //"sprmCFData", chp.fData 1 or
                                                     //0 bit
         {72, 0},                                    //"sprmCRMReason",
                                                     //chp.idslRMReason an index to
                                                     //a table
         {73, &SwWW8ImplReader::Read_CharSet},       //"sprmCChse", chp.fChsDiff
                                                     //and chp.chse 3 bytes
         {74, &SwWW8ImplReader::Read_Symbol},        //"sprmCSymbol", chp.fSpec,
                                                     //chp.chSym and chp.ftcSym
         {75, &SwWW8ImplReader::Read_Obj},           //"sprmCFOle2", chp.fOle2 1
                                                     //or 0 bit
         {76, 0},                                    //"??76",
         {77, 0},                                    //"??77",
         {78, 0},                                    //"??78",
         {79, 0},                                    //"??79",
         {80, &SwWW8ImplReader::Read_CColl},         //"sprmCIstd", chp.istd istd,
                                                     //see stylesheet definition
                                                     //short
         {81, 0},                                    //"sprmCIstdPermute", chp.istd
                                                     //permutation vector
         {82, 0},                                    //"sprmCDefault", whole CHP
                                                     //none variable length
         {83, 0},                                    //"sprmCPlain", whole CHP
                                                     //none 0
         {84, 0},                                    //"??84",
         {85, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFBold", chp.fBold 0,1,
                                                     //128, or 129 byte
         {86, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFItalic", chp.fItalic
                                                     //0,1, 128, or 129 byte
         {87, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFStrike", chp.fStrike
                                                     //0,1, 128, or 129 byte
         {88, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFOutline", chp.fOutline
                                                     //0,1, 128, or 129 byte
         {89, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFShadow", chp.fShadow
                                                     //0,1, 128, or 129 byte
         {90, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFSmallCaps",
                                                     //chp.fSmallCaps 0,1, 128, or
                                                     //129 byte
         {91, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFCaps", chp.fCaps 0,1,
                                                     //128, or 129 byte
         {92, &SwWW8ImplReader::Read_BoldUsw},       //"sprmCFVanish", chp.fVanish
                                                     //0,1, 128, or 129 byte
         {93, &SwWW8ImplReader::Read_FontCode},      //"sprmCFtc", chp.ftc ftc word
         {94, &SwWW8ImplReader::Read_Underline},     // "sprmCKul", chp.kul kul byte
         {95, 0},                                    //"sprmCSizePos", chp.hps,
                                                     //chp.hpsPos 3 bytes
         {96, &SwWW8ImplReader::Read_Kern},          //"sprmCDxaSpace",
                                                     //chp.dxaSpace dxa word
         {97, &SwWW8ImplReader::Read_Language},      //"sprmCLid", chp.lid LID word
         {98, &SwWW8ImplReader::Read_TxtColor},      //"sprmCIco", chp.ico ico byte
         {99, &SwWW8ImplReader::Read_FontSize},      //"sprmCHps", chp.hps hps word!
        {100, 0},                                    //"sprmCHpsInc", chp.hps byte
        {101, &SwWW8ImplReader::Read_SubSuperProp},  //"sprmCHpsPos", chp.hpsPos
                                                     //hps byte
        {102, 0},                                    //"sprmCHpsPosAdj", chp.hpsPos
                                                     //hps byte
        {103, &SwWW8ImplReader::Read_Majority},      //"?sprmCMajority", chp.fBold,
                                                     //chp.fItalic, chp.fSmallCaps
        {104, &SwWW8ImplReader::Read_SubSuper},      //"sprmCIss", chp.iss iss byte
        {105, 0},                                    //"sprmCHpsNew50", chp.hps hps
                                                     //variable width, length
                                                     //always recorded as 2
        {106, 0},                                    //"sprmCHpsInc1", chp.hps
                                                     //complex variable width,
                                                     //length always recorded as 2
        {107, &SwWW8ImplReader::Read_FontKern},      //"sprmCHpsKern", chp.hpsKern
                                                     //hps short
        {108, &SwWW8ImplReader::Read_Majority},      //"sprmCMajority50", chp.fBold,
                                                     //chp.fItalic, chp.fSmallCaps,
                                                     // chp.fVanish, ...
        {109, 0},                                    //"sprmCHpsMul", chp.hps
                                                     //percentage to grow hps short
        {110, 0},                                    //"sprmCCondHyhen", chp.ysri
                                                     //ysri short
        {111, &SwWW8ImplReader::Read_FontCode},      //ww7 font
        {112, &SwWW8ImplReader::Read_FontCode},      //ww7 CJK font
        {113, &SwWW8ImplReader::Read_FontCode},      //ww7 rtl font
        {114, &SwWW8ImplReader::Read_Language},      //ww7 lid
        {115, &SwWW8ImplReader::Read_TxtColor},      //ww7 rtl colour ?
        {116, &SwWW8ImplReader::Read_FontSize},
        {117, &SwWW8ImplReader::Read_Special},       //"sprmCFSpec", chp.fSpec 1
                                                     //or 0 bit
        {118, &SwWW8ImplReader::Read_Obj},           //"sprmCFObj", chp.fObj 1 or 0
                                                     //bit
        {119, 0},                                    //"sprmPicBrcl", pic.brcl brcl
                                                     //(see PIC structure
                                                     //definition) byte
        {120, 0},                                    //"sprmPicScale", pic.mx,
                                                     //pic.my, pic.dxaCropleft,
        {121, 0},                                    //"sprmPicBrcTop", pic.brcTop
                                                     //BRC word
        {122, 0},                                    //"sprmPicBrcLeft",
                                                     //pic.brcLeft BRC word
        {123, 0},                                    //"sprmPicBrcBottom",
                                                     //pic.brcBottom BRC word
        {124, 0},                                    //"sprmPicBrcRight",
                                                     //pic.brcRight BRC word
        {125, 0},                                    //"??125",
        {126, 0},                                    //"??126",
        {127, 0},                                    //"??127",
        {128, 0},                                    //"??128",
        {129, 0},                                    //"??129",
        {130, 0},                                    //"??130",
        {131, 0},                                    //"sprmSScnsPgn", sep.cnsPgn
                                                     //cns byte
        {132, 0},                                    //"sprmSiHeadingPgn",
                                                     //sep.iHeadingPgn heading
                                                     //number level byte
        {133, &SwWW8ImplReader::Read_OLST},          //"sprmSOlstAnm", sep.olstAnm
                                                     //OLST variable length
        {134, 0},                                    //"??135",
        {135, 0},                                    //"??135",
        {136, 0},                                    //"sprmSDxaColWidth",
                                                     //sep.rgdxaColWidthSpacing
                                                     //complex 3 bytes
        {137, 0},                                    //"sprmSDxaColSpacing",
                                                     //sep.rgdxaColWidthSpacing
                                                     //complex 3 bytes
        {138, 0},                                    //"sprmSFEvenlySpaced",
                                                     //sep.fEvenlySpaced 1 or 0 byte
        {139, 0},                                    //"sprmSFProtected",
                                                     //sep.fUnlocked 1 or 0 byte
        {140, 0},                                    //"sprmSDmBinFirst",
                                                     //sep.dmBinFirst  word
        {141, 0},                                    //"sprmSDmBinOther",
                                                     //sep.dmBinOther  word
        {142, 0},                                    //"sprmSBkc", sep.bkc bkc
                                                     //byte BreakCode
        {143, 0},                                    //"sprmSFTitlePage",
                                                     //sep.fTitlePage 0 or 1 byte
        {144, 0},                                    //"sprmSCcolumns", sep.ccolM1
                                                     //# of cols - 1 word
        {145, 0},                                    //"sprmSDxaColumns",
                                                     //sep.dxaColumns dxa word
        {146, 0},                                    //"sprmSFAutoPgn",
                                                     //sep.fAutoPgn obsolete byte
        {147, 0},                                    //"sprmSNfcPgn", sep.nfcPgn
                                                     //nfc byte
        {148, 0},                                    //"sprmSDyaPgn", sep.dyaPgn
                                                     //dya short
        {149, 0},                                    //"sprmSDxaPgn", sep.dxaPgn
                                                     //dya short
        {150, 0},                                    //"sprmSFPgnRestart",
                                                     //sep.fPgnRestart 0 or 1 byte
        {151, 0},                                    //"sprmSFEndnote", sep.fEndnote
                                                     //0 or 1 byte
        {152, 0},                                    //"sprmSLnc", sep.lnc lnc byte
        {153, 0},                                    //"sprmSGprfIhdt", sep.grpfIhdt
                                                     //grpfihdt byte
        {154, 0},                                    //"sprmSNLnnMod", sep.nLnnMod
                                                     //non-neg int. word
        {155, 0},                                    //"sprmSDxaLnn", sep.dxaLnn
                                                     //dxa word
        {156, 0},                                    //"sprmSDyaHdrTop",
                                                     //sep.dyaHdrTop dya word
        {157, 0},                                    //"sprmSDyaHdrBottom",
                                                     //sep.dyaHdrBottom dya word
        {158, 0},                                    //"sprmSLBetween",
                                                     //sep.fLBetween 0 or 1 byte
        {159, 0},                                    //"sprmSVjc", sep.vjc vjc byte
        {160, 0},                                    //"sprmSLnnMin", sep.lnnMin
                                                     //lnn word
        {161, 0},                                    //"sprmSPgnStart", sep.pgnStart
                                                     //pgn word
        {162, 0},                                    //"sprmSBOrientation",
                                                     //sep.dmOrientPage dm byte
        {163, 0},                                    //"?SprmSBCustomize 163", ?
        {164, 0},                                    //"sprmSXaPage", sep.xaPage xa
                                                     //word
        {165, 0},                                    //"sprmSYaPage", sep.yaPage ya
                                                     //word
        {166, 0},                                    //"sprmSDxaLeft", sep.dxaLeft
                                                     //dxa word
        {167, 0},                                    //"sprmSDxaRight", sep.dxaRight
                                                     //dxa word
        {168, 0},                                    //"sprmSDyaTop", sep.dyaTop                                                     //dya word
        {169, 0},                                    //"sprmSDyaBottom",
                                                     //sep.dyaBottom dya word
        {170, 0},                                    //"sprmSDzaGutter",
                                                     //sep.dzaGutter dza word
        {171, 0},                                    //"sprmSDMPaperReq",
                                                     //sep.dmPaperReq dm word
        {172, 0},                                    //"??172",
        {173, 0},                                    //"??173",
        {174, 0},                                    //"??174",
        {175, 0},                                    //"??175",
        {176, 0},                                    //"??176",
        {177, 0},                                    //"??177",
        {178, 0},                                    //"??178",
        {179, 0},                                    //"??179",
        {180, 0},                                    //"??180",
        {181, 0},                                    //"??181",
        {182, 0},                                    //"sprmTJc", tap.jc jc word
                                                     //(low order byte is
                                                     //significant)
        {183, 0},                                    //"sprmTDxaLeft",
                                                     //tap.rgdxaCenter dxa word
        {184, 0},                                    //"sprmTDxaGapHalf",
                                                     //tap.dxaGapHalf,
                                                     //tap.rgdxaCenter dxa word
        {185, 0},                                    //"sprmTFCantSplit"
                                                     //tap.fCantSplit 1 or 0 byte
        {186, 0},                                    //"sprmTTableHeader",
                                                     //tap.fTableHeader 1 or 0 byte
        {187, 0},                                    //"sprmTTableBorders",
                                                     //tap.rgbrcTable complex
                                                     //12 bytes
        {188, 0},                                    //"sprmTDefTable10",
                                                     //tap.rgdxaCenter, tap.rgtc
                                                     //complex variable length
        {189, 0},                                    //"sprmTDyaRowHeight",
                                                     //tap.dyaRowHeight dya word
        {190, 0},                                    //"?sprmTDefTable", tap.rgtc
                                                     //complex
        {191, 0},                                    //"?sprmTDefTableShd",
                                                     //tap.rgshd complex
        {192, 0},                                    //"sprmTTlp", tap.tlp TLP
                                                     //4 bytes
        {193, 0},                                    //"sprmTSetBrc",
                                                     //tap.rgtc[].rgbrc complex
                                                     //5 bytes
        {194, 0},                                    //"sprmTInsert",
                                                     //tap.rgdxaCenter,
                                                     //tap.rgtc complex 4 bytes
        {195, 0},                                    //"sprmTDelete",
                                                     //tap.rgdxaCenter,
                                                     //tap.rgtc complex word
        {196, 0},                                    //"sprmTDxaCol",
                                                     //tap.rgdxaCenter complex
                                                     //4 bytes
        {197, 0},                                    //"sprmTMerge",
                                                     //tap.fFirstMerged,
                                                     //tap.fMerged complex word
        {198, 0},                                    //"sprmTSplit",
                                                     //tap.fFirstMerged,
                                                     //tap.fMerged complex word
        {199, 0},                                    //"sprmTSetBrc10",
                                                     //tap.rgtc[].rgbrc complex
                                                     //5 bytes
        {200, 0},                                    //"sprmTSetShd", tap.rgshd
                                                     //complex 4 bytes
        {207, 0},                                    //dunno
    };

    static wwSprmDispatcher aSprmSrch(aSprms, sizeof(aSprms) / sizeof(aSprms[0]));
    return &aSprmSrch;
}

const wwSprmDispatcher *GetWW8SprmDispatcher()
{
    static SprmReadInfo aSprms[] =
    {
        {0,      0},                                 // "0" Default bzw. Error

        {0x4600, &SwWW8ImplReader::Read_StyleCode},  //"sprmPIstd" pap.istd;istd
                                                     //(style code);short;
        {0xC601, 0},                                 //"sprmPIstdPermute" pap.istd;
                                                     //permutation vector;
                                                     //variable length;
        {0x2602, 0},                                 //"sprmPIncLvl" pap.istd,
                                                     //pap.lvl;difference between
                                                     //istd of base PAP and istd of
                                                     //PAP to be produced; byte;
        {0x2403, &SwWW8ImplReader::Read_Justify},    //"sprmPJc" pap.jc;jc
                                                     //(justification);byte;
        {0x2404, 0},                                 //"sprmPFSideBySide"
                                                     //pap.fSideBySide;0 or 1;byte;
        {0x2405, &SwWW8ImplReader::Read_KeepLines},  //"sprmPFKeep" pap.fKeep;0 or
                                                     //1;byte;
        {0x2406, &SwWW8ImplReader::Read_KeepParas},  //"sprmPFKeepFollow"
                                                     //pap.fKeepFollow;0 or 1;byte;
        {0x2407, &SwWW8ImplReader::Read_BreakBefore},//"sprmPFPageBreakBefore"
                                                     //pap.fPageBreakBefore;0 or 1;
                                                     //byte;
        {0x2408, 0},                                 //"sprmPBrcl" pap.brcl;brcl;
                                                     //byte;
        {0x2409, 0},                                 //"sprmPBrcp" pap.brcp;brcp;
                                                     //byte;
        {0x260A, &SwWW8ImplReader::Read_ListLevel},  //"sprmPIlvl" pap.ilvl;ilvl;
                                                     //byte;
        {0x460B, &SwWW8ImplReader::Read_LFOPosition},//"sprmPIlfo" pap.ilfo;ilfo
                                                     //(list index) ;short;
        {0x240C, &SwWW8ImplReader::Read_NoLineNumb}, //"sprmPFNoLineNumb"
                                                     //pap.fNoLnn;0 or 1;byte;
        {0xC60D, &SwWW8ImplReader::Read_Tab},        //"sprmPChgTabsPapx"
                                                     //pap.itbdMac, pap.rgdxaTab,
                                                     //pap.rgtbd;complex;variable
                                                     //length
        {0x840E, &SwWW8ImplReader::Read_LR},         //Word 97 version of "sprmPDxaRight" pap.dxaRight;
                                                     //dxa;word;
        {0x840F, &SwWW8ImplReader::Read_LR},         //Apparently Word 97 version of "sprmPDxaLeft" pap.dxaLeft;
                                                     //dxa;word;
        {0x4610, 0},                                 //"sprmPNest" pap.dxaLeft;
                                                     //dxa;word;
        {0x8411, &SwWW8ImplReader::Read_LR},         //Word 97 version of "sprmPDxaLeft1" pap.dxaLeft1;
                                                     //dxa;word;
        {0x6412, &SwWW8ImplReader::Read_LineSpace},  //"sprmPDyaLine" pap.lspd;
                                                     //an LSPD, a long word
                                                     //structure consisting of a
                                                     //short of dyaLine followed by
                                                     //a short of fMultLinespace;
                                                     //long;
        {0xA413, &SwWW8ImplReader::Read_UL},         //"sprmPDyaBefore"
                                                     //pap.dyaBefore;dya;word;
        {0xA414, &SwWW8ImplReader::Read_UL},         //"sprmPDyaAfter" pap.dyaAfter;
                                                     //dya;word;
        {0xC615, 0},                                 //"sprmPChgTabs" pap.itbdMac,
                                                     //pap.rgdxaTab, pap.rgtbd;
                                                     //complex;variable length;
        {0x2416, 0},                                 //"sprmPFInTable" pap.fInTable;
                                                     //0 or 1;byte;
        {0x2417, &SwWW8ImplReader::Read_TabRowEnd},  //"sprmPFTtp" pap.fTtp;0 or 1;
                                                     //byte;
        {0x8418, 0},                                 //"sprmPDxaAbs" pap.dxaAbs;dxa;
                                                     //word;
        {0x8419, 0},                                 //"sprmPDyaAbs" pap.dyaAbs;dya;
                                                     //word;
        {0x841A, 0},                                 //"sprmPDxaWidth" pap.dxaWidth;
                                                     //dxa;word;
        {0x261B, &SwWW8ImplReader::Read_ApoPPC},     //"sprmPPc" pap.pcHorz,
                                                     //pap.pcVert;complex;byte;
        {0x461C, 0},                                 //"sprmPBrcTop10" pap.brcTop;
                                                     //BRC10;word;
        {0x461D, 0},                                 //"sprmPBrcLeft10" pap.brcLeft;
                                                     //BRC10;word;
        {0x461E, 0},                                 //"sprmPBrcBottom10"
                                                     //pap.brcBottom;BRC10;word;
        {0x461F, 0},                                 //"sprmPBrcRight10"
                                                     //pap.brcRight;BRC10;word;
        {0x4620, 0},                                 //"sprmPBrcBetween10"
                                                     //pap.brcBetween;BRC10;word;
        {0x4621, 0},                                 //"sprmPBrcBar10" pap.brcBar;
                                                     //BRC10;word;
        {0x4622, 0},                                 //"sprmPDxaFromText10"
                                                     //pap.dxaFromText;dxa;word;
        {0x2423, 0},                                 //"sprmPWr" pap.wr;wr;byte;
        {0x6424, &SwWW8ImplReader::Read_Border},     //"sprmPBrcTop" pap.brcTop;BRC;
                                                     //long;
        {0x6425, &SwWW8ImplReader::Read_Border},     //"sprmPBrcLeft" pap.brcLeft;
                                                     //BRC;long;
        {0x6426, &SwWW8ImplReader::Read_Border},     //"sprmPBrcBottom"
                                                     //pap.brcBottom;BRC;long;
        {0x6427, &SwWW8ImplReader::Read_Border},     //"sprmPBrcRight" pap.brcRight;
                                                     //BRC;long;
        {0x6428, &SwWW8ImplReader::Read_Border},     //"sprmPBrcBetween"
                                                     //pap.brcBetween;BRC;long;
        {0x6629, 0},                                 //"sprmPBrcBar" pap.brcBar;BRC;
                                                     //long;
        {0x242A, &SwWW8ImplReader::Read_Hyphenation},//"sprmPFNoAutoHyph"
                                                     //pap.fNoAutoHyph;0 or 1;byte;
        {0x442B, 0},                                 //"sprmPWHeightAbs"
                                                     //pap.wHeightAbs;w;word;
        {0x442C, 0},                                 //"sprmPDcs" pap.dcs;DCS;short;
        {0x442D, &SwWW8ImplReader::Read_Shade},      //"sprmPShd" pap.shd;SHD;word;
        {0x842E, 0},                                 //"sprmPDyaFromText"
                                                     //pap.dyaFromText;dya;word;
        {0x842F, 0},                                 //"sprmPDxaFromText"
                                                     //pap.dxaFromText;dxa;word;
        {0x2430, 0},                                 //"sprmPFLocked" pap.fLocked;
                                                     //0 or 1;byte;
        {0x2431, &SwWW8ImplReader::Read_WidowControl},//"sprmPFWidowControl"
                                                     //pap.fWidowControl;0 or 1;byte
        {0xC632, 0},                                 //"sprmPRuler" variable length;
        {0x2433, &SwWW8ImplReader::Read_BoolItem},   //"sprmPFKinsoku" pap.fKinsoku;
                                                     //0 or 1;byte;
        {0x2434, 0},                                 //"sprmPFWordWrap"
                                                     //pap.fWordWrap;0 or 1;byte;
        {0x2435, &SwWW8ImplReader::Read_BoolItem},   //"sprmPFOverflowPunct"
                                                     //pap.fOverflowPunct; 0 or 1;
                                                     //byte;
        {0x2436, 0},                                 //"sprmPFTopLinePunct"
                                                     //pap.fTopLinePunct;0 or 1;byte
        {0x2437, &SwWW8ImplReader::Read_BoolItem},   //"sprmPFAutoSpaceDE"
                                                     //pap.fAutoSpaceDE;0 or 1;byte;
        {0x2438, 0},                                 //"sprmPFAutoSpaceDN"
                                                     //pap.fAutoSpaceDN;0 or 1;byte;
        {0x4439, &SwWW8ImplReader::Read_AlignFont},  //"sprmPWAlignFont"
                                                     //pap.wAlignFont;iFa; word;
        {0x443A, 0},                                 //"sprmPFrameTextFlow"
                                                     //pap.fVertical pap.fBackward
                                                     //pap.fRotateFont;complex; word
        {0x243B, 0},                                 //"sprmPISnapBaseLine" obsolete
                                                     //not applicable in Word97
                                                     //and later versions;;byte;
        {0xC63E, &SwWW8ImplReader::Read_ANLevelDesc},//"sprmPAnld" pap.anld;;
                                                     //variable length;
        {0xC63F, 0},                                 //"sprmPPropRMark"
                                                     //pap.fPropRMark;complex;
                                                     //variable length;
        {0x2640,  &SwWW8ImplReader::Read_POutLvl},   //"sprmPOutLvl" pap.lvl;has no
                                                     //effect if pap.istd is < 1 or
                                                     //is > 9;byte;
        {0x2441, &SwWW8ImplReader::Read_ParaBiDi},   //"sprmPFBiDi" ;;byte;
        {0x2443, 0},                                 //"sprmPFNumRMIns"
                                                     //pap.fNumRMIns;1 or 0;bit;
        {0x2444, 0},                                 //"sprmPCrLf" ;;byte;
        {0xC645, 0},                                 //"sprmPNumRM" pap.numrm;;
                                                     //variable length;
        {0x6645, 0},                                 //"sprmPHugePapx" ;fc in the
                                                     //data stream to locate the
                                                     //huge grpprl; long;
        {0x6646, 0},                                 //"sprmPHugePapx" ;fc in the
                                                     //data stream to locate the
                                                     //huge grpprl; long;
        {0x2447, &SwWW8ImplReader::Read_UsePgsuSettings},//"sprmPFUsePgsuSettings"
                                                     //pap.fUsePgsuSettings;1 or 0;
                                                     //byte;
        {0x2448, 0},                                 //"sprmPFAdjustRight"
                                                     //pap.fAdjustRight;1 or 0;byte;
        {0x0800, &SwWW8ImplReader::Read_CFRMarkDel}, //"sprmCFRMarkDel"
                                                     //chp.fRMarkDel;1 or 0;bit;
        {0x0801, &SwWW8ImplReader::Read_CFRMark},    //"sprmCFRMark" chp.fRMark;1
                                                     //or 0;bit;
        {0x0802, &SwWW8ImplReader::Read_FldVanish},  //"sprmCFFldVanish"
                                                     //chp.fFldVanish;1 or 0;bit;
        {0x6A03, &SwWW8ImplReader::Read_PicLoc},     //"sprmCPicLocation" chp.fcPic
                                                     //and chp.fSpec;variable
                                                     //length, length recorded is
                                                     //always 4;
        {0x4804, 0},                                 //"sprmCIbstRMark"
                                                     //chp.ibstRMark;index into
                                                     //sttbRMark;short;
        {0x6805, 0},                                 //"sprmCDttmRMark"
                                                     //chp.dttmRMark;DTTM;long;
        {0x0806, 0},                                 //"sprmCFData" chp.fData;1 or
                                                     //0;bit;
        {0x4807, 0},                                 //"sprmCIdslRMark"
                                                     //chp.idslRMReason;an index to
                                                     //a table of strings defined in
                                                     //Word 6.0 executables;short;
        {0xEA08, &SwWW8ImplReader::Read_CharSet},    //"sprmCChs" chp.fChsDiff and
                                                     //chp.chse;3 bytes;
        {0x6A09, &SwWW8ImplReader::Read_Symbol},     //"sprmCSymbol" chp.fSpec,
                                                     //chp.xchSym and chp.ftcSym;
                                                     //variable length, length
                                                     //recorded is always 4;
        {0x080A, &SwWW8ImplReader::Read_Obj},        //"sprmCFOle2" chp.fOle2;1 or
                                                     //0;bit;
      //0x480B,                                      //"sprmCIdCharType", obsolete:
                                                     //not applicable in Word97
                                                     //and later versions
        {0x2A0C, &SwWW8ImplReader::Read_CharHighlight},//"sprmCHighlight"
                                                     //chp.fHighlight,
                                                     //chp.icoHighlight;ico
                                                     //(fHighlight is set to 1 iff
                                                     //ico is not 0);byte;
        {0x680E, &SwWW8ImplReader::Read_PicLoc},     //"sprmCObjLocation" chp.fcObj;
                                                     //FC;long;
      //0x2A10, ? ? ?,                               //"sprmCFFtcAsciSymb"
        {0x4A30, &SwWW8ImplReader::Read_CColl},      //"sprmCIstd" chp.istd;istd,
                                                     //short;
        {0xCA31, 0},                                 //"sprmCIstdPermute" chp.istd;
                                                     //permutation vector; variable
                                                     //length;
        {0x2A32, 0},                                 //"sprmCDefault" whole CHP;none
                                                     //;variable length;
        {0x2A33, 0},                                 //"sprmCPlain" whole CHP;none;
                                                     //Laenge: 0;
        {0x2A34, &SwWW8ImplReader::Read_Emphasis},   //"sprmCKcd"
        {0x0835, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFBold" chp.fBold;0,1,
                                                     //128, or 129; byte;
        {0x0836, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFItalic" chp.fItalic;0,
                                                     //1, 128, or 129; byte;
        {0x0837, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFStrike" chp.fStrike;0,
                                                     //1, 128, or 129; byte;
        {0x0838, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFOutline" chp.fOutline;
                                                     //0,1, 128, or 129; byte;
        {0x0839, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFShadow" chp.fShadow;0,
                                                     //1, 128, or 129; byte;
        {0x083A, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFSmallCaps"
                                                     //chp.fSmallCaps;0,1, 128, or
                                                     //129;byte;
        {0x083B, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFCaps" chp.fCaps;0,1,
                                                     //128, or 129; byte;
        {0x083C, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFVanish" chp.fVanish;0,
                                                     //1, 128, or 129; byte;
      //0x4A3D, 0,                                   //"sprmCFtcDefault" ftc, only
                                                     //used internally, never
                                                     //stored in file;word;
        {0x2A3E, &SwWW8ImplReader::Read_Underline},  //"sprmCKul" chp.kul;kul;byte;
        {0xEA3F, 0},                                 //"sprmCSizePos" chp.hps,
                                                     //chp.hpsPos;3 bytes;
        {0x8840, &SwWW8ImplReader::Read_Kern},       //"sprmCDxaSpace" chp.dxaSpace;
                                                     //dxa;word;
        {0x4A41, &SwWW8ImplReader::Read_Language},   //"sprmCLid" ;only used
                                                     //internally never stored;word;
        {0x2A42, &SwWW8ImplReader::Read_TxtColor},   //"sprmCIco" chp.ico;ico;byte;
        {0x4A43, &SwWW8ImplReader::Read_FontSize},   //"sprmCHps" chp.hps;hps;byte;
        {0x2A44, 0},                                 //"sprmCHpsInc" chp.hps;byte;
        {0x4845, &SwWW8ImplReader::Read_SubSuperProp},//"sprmCHpsPos" chp.hpsPos;
                                                     //hps; byte;
        {0x2A46, 0},                                 //"sprmCHpsPosAdj" chp.hpsPos;
                                                     //hps; byte;
        {0xCA47, &SwWW8ImplReader::Read_Majority},   //"sprmCMajority" chp.fBold,
                                                     //chp.fItalic, chp.fSmallCaps,
                                                     //chp.fVanish, chp.fStrike,
                                                     //chp.fCaps, chp.rgftc,
                                                     //chp.hps, chp.hpsPos, chp.kul,
                                                     //chp.dxaSpace, chp.ico,
                                                     //chp.rglid;complex;variable
                                                     //length, length byte plus
                                                     //size of following grpprl;
        {0x2A48, &SwWW8ImplReader::Read_SubSuper},   //"sprmCIss" chp.iss;iss;byte;
        {0xCA49, 0},                                 //"sprmCHpsNew50" chp.hps;hps;
                                                     //variable width, length
                                                     //always recorded as 2;
        {0xCA4A, 0},                                 //"sprmCHpsInc1" chp.hps;
                                                     //complex; variable width,
                                                     //length always recorded as 2;
        {0x484B, &SwWW8ImplReader::Read_FontKern},   //"sprmCHpsKern" chp.hpsKern;
                                                     //hps;short;
        {0xCA4C, &SwWW8ImplReader::Read_Majority},   //"sprmCMajority50" chp.fBold,
                                                     //chp.fItalic, chp.fSmallCaps,
                                                     //chp.fVanish, chp.fStrike,
                                                     //chp.fCaps, chp.ftc, chp.hps,
                                                     //chp.hpsPos, chp.kul,
                                                     //chp.dxaSpace, chp.ico;
                                                     //complex; variable length;
        {0x4A4D, 0},                                 //"sprmCHpsMul" chp.hps;
                                                     //percentage to grow hps;short;
        {0x484E, 0},                                 //"sprmCYsri" chp.ysri;ysri;
                                                     //short;
        {0x4A4F, &SwWW8ImplReader::Read_FontCode},   //"sprmCRgFtc0" chp.rgftc[0];
                                                     //ftc for ASCII text; short;
        {0x4A50, &SwWW8ImplReader::Read_FontCode},   //"sprmCRgFtc1" chp.rgftc[1];
                                                     //ftc for Far East text;short;
        {0x4A51, &SwWW8ImplReader::Read_FontCode},   //"sprmCRgFtc2" chp.rgftc[2];
                                                     //ftc for non-Far East text;
                                                     //short;
        {0x4852, &SwWW8ImplReader::Read_ScaleWidth}, //"sprmCCharScale"
        {0x2A53, &SwWW8ImplReader::Read_BoldUsw},    //"sprmCFDStrike" chp.fDStrike;
                                                     //;byte;
        {0x0854, &SwWW8ImplReader::Read_Relief},     //"sprmCFImprint" chp.fImprint;
                                                     //1 or 0;bit;
        {0x0855, &SwWW8ImplReader::Read_Special},    //"sprmCFSpec" chp.fSpec;
                                                     //1 or 0;bit;
        {0x0856, &SwWW8ImplReader::Read_Obj},        //"sprmCFObj" chp.fObj;1 or 0;
                                                     //bit;
        {0xCA57, &SwWW8ImplReader::Read_CPropRMark}, //"sprmCPropRMark"
                                                     //chp.fPropRMark,
                                                     //chp.ibstPropRMark,
                                                     //chp.dttmPropRMark;Complex;
                                                     //variable length always
                                                     //recorded as 7 bytes;
        {0x0858, &SwWW8ImplReader::Read_Relief},     //"sprmCFEmboss" chp.fEmboss;
                                                     //1 or 0;bit;
        {0x2859, &SwWW8ImplReader::Read_TxtAnim},    //"sprmCSfxText" chp.sfxtText;
                                                     //text animation;byte;
        {0x085A, &SwWW8ImplReader::Read_Bidi},                                 //"sprmCFBiDi"
        {0x085B, 0},                                 //"sprmCFDiacColor"
        {0x085C, &SwWW8ImplReader::Read_BoldBiDiUsw},//"sprmCFBoldBi"
        {0x085D, &SwWW8ImplReader::Read_BoldBiDiUsw},//"sprmCFItalicBi"
        {0x4A5E, &SwWW8ImplReader::Read_FontCode},
        {0x485F, &SwWW8ImplReader::Read_Language},   // "sprmCLidBi"
      //0x4A60, ? ? ?,                               //"sprmCIcoBi",
        {0x4A61, &SwWW8ImplReader::Read_FontSize},   //"sprmCHpsBi"
        {0xCA62, 0},                                 //"sprmCDispFldRMark"
                                                     //chp.fDispFldRMark,
                                                     //chp.ibstDispFldRMark,
                                                     //chp.dttmDispFldRMark ;
                                                     //Complex;variable length
                                                     //always recorded as 39 bytes;
        {0x4863, 0},                                 //"sprmCIbstRMarkDel"
                                                     //chp.ibstRMarkDel;index into
                                                     //sttbRMark;short;
        {0x6864, 0},                                 //"sprmCDttmRMarkDel"
                                                     //chp.dttmRMarkDel;DTTM;long;
        {0x6865, 0},                                 //"sprmCBrc" chp.brc;BRC;long;
        {0x4866, &SwWW8ImplReader::Read_CharShadow}, //"sprmCShd" chp.shd;SHD;short;
        {0x4867, 0},                                 //"sprmCIdslRMarkDel"
                                                     //chp.idslRMReasonDel;an index
                                                     //to a table of strings
                                                     //defined in Word 6.0
                                                     //executables;short;
        {0x0868, 0},                                 //"sprmCFUsePgsuSettings"
                                                     //chp.fUsePgsuSettings; 1 or 0;
                                                     //bit;
        {0x486B, 0},                                 //"sprmCCpg" ;;word;
        {0x486D, &SwWW8ImplReader::Read_Language},   //"sprmCRgLid0" chp.rglid[0];
                                                     //LID: for non-Far East text;
                                                     //word;
        {0x486E, &SwWW8ImplReader::Read_Language},   //"sprmCRgLid1" chp.rglid[1];
                                                     //LID: for Far East text;word;
        {0x286F, &SwWW8ImplReader::Read_IdctHint},   //"sprmCIdctHint" chp.idctHint;
                                                     //IDCT: byte;
        {0x2E00, 0},                                 //"sprmPicBrcl" pic.brcl;brcl
                                                     //(see PIC structure
                                                     //definition);byte;
        {0xCE01, 0},                                 //"sprmPicScale" pic.mx,
                                                     //pic.my, pic.dxaCropleft,
                                                     //pic.dyaCropTop
                                                     //pic.dxaCropRight,
                                                     //pic.dyaCropBottom;Complex;
                                                     //length byte plus 12 bytes;
        {0x6C02, 0},                                 //"sprmPicBrcTop" pic.brcTop;
                                                     //BRC;long;
        {0x6C03, 0},                                 //"sprmPicBrcLeft" pic.brcLeft;
                                                     //BRC;long;
        {0x6C04, 0},                                 //"sprmPicBrcBottom"
                                                     //pic.brcBottom;BRC;long;
        {0x6C05, 0},                                 //"sprmPicBrcRight"
                                                     //pic.brcRight;BRC;long;
        {0x3000, 0},                                 //"sprmScnsPgn" sep.cnsPgn;cns;
                                                     //byte;
        {0x3001, 0},                                 //"sprmSiHeadingPgn"
                                                     //sep.iHeadingPgn;heading
                                                     //number level;byte;
        {0xD202, &SwWW8ImplReader::Read_OLST},       //"sprmSOlstAnm" sep.olstAnm;
                                                     //OLST;variable length;
        {0xF203, 0},                                 //"sprmSDxaColWidth"
                                                     //sep.rgdxaColWidthSpacing;
                                                     //complex; 3 bytes;
        {0xF204, 0},                                 //"sprmSDxaColSpacing"
                                                     //sep.rgdxaColWidthSpacing;
                                                     //complex; 3 bytes;
        {0x3005, 0},                                 //"sprmSFEvenlySpaced"
                                                     //sep.fEvenlySpaced; 1 or 0;
                                                     //byte;
        {0x3006, 0},                                 //"sprmSFProtected"
                                                     //sep.fUnlocked;1 or 0;byte;
        {0x5007, 0},                                 //"sprmSDmBinFirst"
                                                     //sep.dmBinFirst;;word;
        {0x5008, 0},                                 //"sprmSDmBinOther"
                                                     //sep.dmBinOther;;word;
        {0x3009, 0},                                 //"sprmSBkc" sep.bkc;bkc;byte;
        {0x300A, 0},                                 //"sprmSFTitlePage"
                                                     //sep.fTitlePage;0 or 1;byte;
        {0x500B, 0},                                 //"sprmSCcolumns" sep.ccolM1;
                                                     //# of cols - 1;word;
        {0x900C, 0},                                 //"sprmSDxaColumns"
                                                     //sep.dxaColumns;dxa;word;
        {0x300D, 0},                                 //"sprmSFAutoPgn" sep.fAutoPgn;
                                                     //obsolete;byte;
        {0x300E, 0},                                 //"sprmSNfcPgn" sep.nfcPgn;nfc;
                                                     //byte;
        {0xB00F, 0},                                 //"sprmSDyaPgn" sep.dyaPgn;dya;
                                                     //short;
        {0xB010, 0},                                 //"sprmSDxaPgn" sep.dxaPgn;dya;
                                                     //short;
        {0x3011, 0},                                 //"sprmSFPgnRestart"
                                                     //sep.fPgnRestart;0 or 1;byte;
        {0x3012, 0},                                 //"sprmSFEndnote" sep.fEndnote;
                                                     //0 or 1;byte;
        {0x3013, 0},                                 //"sprmSLnc" sep.lnc;lnc;byte;
        {0x3014, 0},                                 //"sprmSGprfIhdt" sep.grpfIhdt;
                                                     //grpfihdt; byte;
        {0x5015, 0},                                 //"sprmSNLnnMod" sep.nLnnMod;
                                                     //non-neg int.;word;
        {0x9016, 0},                                 //"sprmSDxaLnn" sep.dxaLnn;dxa;
                                                     //word;
        {0xB017, 0},                                 //"sprmSDyaHdrTop"
                                                     //sep.dyaHdrTop;dya;word;
        {0xB018, 0},                                 //"sprmSDyaHdrBottom"
                                                     //sep.dyaHdrBottom;dya;word;
        {0x3019, 0},                                 //"sprmSLBetween"
                                                     //sep.fLBetween;0 or 1;byte;
        {0x301A, 0},                                 //"sprmSVjc" sep.vjc;vjc;byte;
        {0x501B, 0},                                 //"sprmSLnnMin" sep.lnnMin;lnn;
                                                     //word;
        {0x501C, 0},                                 //"sprmSPgnStart" sep.pgnStart;
                                                     //pgn;word;
        {0x301D, 0},                                 //"sprmSBOrientation"
                                                     //sep.dmOrientPage;dm;byte;
      //0x301E, ? ? ?,                               //"sprmSBCustomize"
        {0xB01F, 0},                                 //"sprmSXaPage" sep.xaPage;xa;
                                                     //word;
        {0xB020, 0},                                 //"sprmSYaPage" sep.yaPage;ya;
                                                     //word;
        {0x2205, 0},                                 //"sprmSDxaLeft" sep.dxaLeft;
                                                     //dxa;word;
        {0xB022, 0},                                 //"sprmSDxaRight" sep.dxaRight;
                                                     //dxa;word;
        {0x9023, 0},                                 //"sprmSDyaTop" sep.dyaTop;dya;
                                                     //word;
        {0x9024, 0},                                 //"sprmSDyaBottom"
                                                     //sep.dyaBottom;dya;word;
        {0xB025, 0},                                 //"sprmSDzaGutter"
                                                     //sep.dzaGutter;dza;word;
        {0x5026, 0},                                 //"sprmSDmPaperReq"
                                                     //sep.dmPaperReq;dm;word;
        {0xD227, 0},                                 //"sprmSPropRMark"
                                                     //sep.fPropRMark,
                                                     //sep.ibstPropRMark,
                                                     //sep.dttmPropRMark ;complex;
                                                     //variable length always
                                                     //recorded as 7 bytes;
      //0x3228, ? ? ?,                               //"sprmSFBiDi",
      //0x3229, ? ? ?,                               //"sprmSFFacingCol"
        {0x322A, 0},                                 //"sprmSFRTLGutter", set to 1
                                                     //if gutter is on the right.
        {0x702B, 0},                                 //"sprmSBrcTop" sep.brcTop;BRC;
                                                     //long;
        {0x702C, 0},                                 //"sprmSBrcLeft" sep.brcLeft;
                                                     //BRC;long;
        {0x702D, 0},                                 //"sprmSBrcBottom"
                                                     //sep.brcBottom;BRC;long;
        {0x702E, 0},                                 //"sprmSBrcRight" sep.brcRight;
                                                     //BRC;long;
        {0x522F, 0},                                 //"sprmSPgbProp" sep.pgbProp;
                                                     //word;
        {0x7030, 0},                                 //"sprmSDxtCharSpace"
                                                     //sep.dxtCharSpace;dxt;long;
        {0x9031, 0},                                 //"sprmSDyaLinePitch"
                                                     //sep.dyaLinePitch;dya;
                                                     //WRONG:long; RIGHT:short; !
      //0x5032, ? ? ?,                               //"sprmSClm"
        {0x5033, 0},                                 //"sprmSTextFlow"
                                                     //sep.wTextFlow;complex ;short
        {0x5400, 0},                                 //"sprmTJc" tap.jc;jc;word (low
                                                     //order byte is significant);
        {0x9601, 0},                                 //"sprmTDxaLeft"
                                                     //tap.rgdxaCenter; dxa; word;
        {0x9602, 0},                                 //"sprmTDxaGapHalf"
                                                     //tap.dxaGapHalf,
                                                     //tap.rgdxaCenter; dxa; word;
        {0x3403, 0},                                 //"sprmTFCantSplit"
                                                     //tap.fCantSplit;1 or 0;byte;
        {0x3404, 0},                                 //"sprmTTableHeader"
                                                     //tap.fTableHeader;1 or 0;byte;
        {0x3466, 0},                                 //"sprmTFCantSplit90"
                                                     //tap.fCantSplit90;1 or 0;byte;
        {0xD605, 0},                                 //"sprmTTableBorders"
                                                     //tap.rgbrcTable;complex;
                                                     //24 bytes;
        {0xD606, 0},                                 //"sprmTDefTable10"
                                                     //tap.rgdxaCenter,
                                                     //tap.rgtc;complex; variable
                                                     //length;
        {0x9407, 0},                                 //"sprmTDyaRowHeight"
                                                     //tap.dyaRowHeight;dya;word;
        {0xD608, 0},                                 //"sprmTDefTable"
                                                     //tap.rgtc;complex
        {0xD609, 0},                                 //"sprmTDefTableShd"
                                                     //tap.rgshd;complex
        {0x740A, 0},                                 //"sprmTTlp" tap.tlp;TLP;
                                                     //4 bytes;
      //0x560B, ? ? ?,                               //"sprmTFBiDi"
      //0x740C, ? ? ?,                               //"sprmTHTMLProps"
        {0xD620, 0},                                 //"sprmTSetBrc"
                                                     //tap.rgtc[].rgbrc;complex;
                                                     //5 bytes;
        {0x7621, 0},                                 //"sprmTInsert"
                                                     //tap.rgdxaCenter,
                                                     //tap.rgtc;complex ;4 bytes;
        {0x5622, 0},                                 //"sprmTDelete"
                                                     //tap.rgdxaCenter,
                                                     //tap.rgtc;complex ;word;
        {0x7623, 0},                                 //"sprmTDxaCol"
                                                     //tap.rgdxaCenter;complex;
                                                     //4 bytes;
        {0x5624, 0},                                 //"sprmTMerge"
                                                     //tap.fFirstMerged,
                                                     //tap.fMerged;complex; word;
        {0x5625, 0},                                 //"sprmTSplit"
                                                     //tap.fFirstMerged,
                                                     //tap.fMerged;complex ;word;
        {0xD626, 0},                                 //"sprmTSetBrc10"
                                                     //tap.rgtc[].rgbrc;complex;
                                                     //5 bytes;
        {0x7627, 0},                                 //"sprmTSetShd" tap.rgshd;
                                                     //complex; 4 bytes;
        {0x7628, 0},                                 //"sprmTSetShdOdd"
                                                     //tap.rgshd;complex;4 bytes;
        {0x7629, 0},                                 //"sprmTTextFlow"
                                                     //tap.rgtc[].fVertical
                                                     //tap.rgtc[].fBackward
                                                     //tap.rgtc[].fRotateFont
                                                     //0 or 10 or 10 or 1;word;
      //0xD62A, ? ? ?  ,                             //"sprmTDiagLine"
        {0xD62B, 0},                                 //"sprmTVertMerge"
                                                     //tap.rgtc[].vertMerge;complex;
                                                     //variable length always
                                                     //recorded as 2 bytes;
        {0xD62C, 0},                                 //"sprmTVertAlign"
                                                     //tap.rgtc[].vertAlign;complex
                                                     //variable length always
                                                     //recorded as 3 byte;
        {0xCA78, &SwWW8ImplReader::Read_DoubleLine_Rotate},
        {0x6649, 0},                                 //undocumented
        {0xF614, 0},                                 //"sprmTTableWidth"
                                                     //recorded as 3 bytes;
        {0xD612, 0},                                 //undocumented
        {0xD613, 0},                                 //undocumented
        {0xD61A, 0},                                 //undocumented
        {0xD61B, 0},                                 //undocumented
        {0xD61C, 0},                                 //undocumented
        {0xD61D, 0},                                 //undocumented
        {0xD634, 0},                                 //undocumented
        {0xD632, 0},                                 //undocumented
        {0xD238, 0},                                 //undocumented sep
        {0xC64E, 0},                                 //undocumented
        {0xC64F, 0},                                 //undocumented
        {0xC650, 0},                                 //undocumented
        {0xC651, 0},                                 //undocumented
        {0xF661, 0},                                 //undocumented
        {0x4873, &SwWW8ImplReader::Read_Language},   //"sprmCRgLid3?" chp.rglid[0];
                                                     //LID: for non-Far East text
                                                     //(like a duplicate of 486D)
        {0x4874, 0},                                 //undocumented
        {0x6463, 0},                                 //undocumented
        {0x2461, &SwWW8ImplReader::Read_RTLJustify}, //undoc, must be asian version
                                                     //of "sprmPJc"
        {0x845E, &SwWW8ImplReader::Read_LR},         //Apparently post-Word 97 version of "sprmPDxaLeft"
        {0x8460, &SwWW8ImplReader::Read_LR},         //Post-Word 97 version of "sprmPDxaLeft1"
        {0x845D, &SwWW8ImplReader::Read_LR},         //Post-Word 97 version of "sprmPDxaRight"
        {0x3615, 0},                                 //undocumented
        {0x360D, 0},                                 //undocumented
        {0x940E, 0},                                 //undocumented
        {0x940F, 0},                                 //undocumented
        {0x9410, 0},                                 //undocumented
        {0x703A, 0},                                 //undocumented
        {0x303B, 0},                                 //undocumented
        {0x244B, &SwWW8ImplReader::Read_TabCellEnd}, //undocumented, must be
                                                     //subtable "sprmPFInTable"
        {0x244C, &SwWW8ImplReader::Read_TabRowEnd},  //undocumented, must be
                                                     // subtable "sprmPFTtp"
        {0x6815, 0},                                 //undocumented
        {0x6816, 0},                                 //undocumented
        {0x6870, &SwWW8ImplReader::Read_TxtForeColor},
        {0xC64D, &SwWW8ImplReader::Read_ParaBackColor},
        {0x6467, 0},                                 //undocumented
        {0xF617, 0},                                 //undocumented
        {0xD660, 0},                                 //undocumented
        {0xD670, 0},                                 //undocumented
        {0xCA71, &SwWW8ImplReader::Read_TxtBackColor},//undocumented
        {0x303C, 0},                                 //undocumented
        {0x245B, &SwWW8ImplReader::Read_ParaAutoBefore},//undocumented, para
        {0x245C, &SwWW8ImplReader::Read_ParaAutoAfter},//undocumented, para
        {0x246D, &SwWW8ImplReader::Read_DontAddEqual}//undocumented, para
    };

    static wwSprmDispatcher aSprmSrch(aSprms, sizeof(aSprms) / sizeof(aSprms[0]));
    return &aSprmSrch;
}

//-----------------------------------------
//      Hilfsroutinen : SPRM finden
//-----------------------------------------

const SprmReadInfo& SwWW8ImplReader::GetSprmReadInfo(USHORT nId) const
{
    ww::WordVersion eVersion = pWwFib->GetFIBVersion();
    const wwSprmDispatcher *pDispatcher;
    if (eVersion <= ww::eWW2)
        pDispatcher = GetWW2SprmDispatcher();
    else if (eVersion < ww::eWW8)
        pDispatcher = GetWW6SprmDispatcher();
    else
        pDispatcher = GetWW8SprmDispatcher();

    SprmReadInfo aSrch = {0, 0};
    aSrch.nId = nId;
    const SprmReadInfo* pFound = pDispatcher->search(aSrch);

    if (!pFound)
    {
        aSrch.nId = 0;
        pFound = pDispatcher->search(aSrch);
    }

    return *pFound;
}

//-----------------------------------------
//      Hilfsroutinen : SPRMs
//-----------------------------------------
void SwWW8ImplReader::EndSprm( USHORT nId )
{
    if( ( nId > 255 ) && ( nId < 0x0800 ) ) return;

    const SprmReadInfo& rSprm = GetSprmReadInfo( nId );

    if (rSprm.pReadFnc)
        (this->*rSprm.pReadFnc)( nId, 0, -1 );
}

short SwWW8ImplReader::ImportSprm(const BYTE* pPos,USHORT nId)
{
    if (!nId)
        nId = mpSprmParser->GetSprmId(pPos);

#if OSL_DEBUG_LEVEL > 1
    ASSERT( nId != 0xff, "Sprm FF !!!!" );
#endif

    const SprmReadInfo& rSprm = GetSprmReadInfo(nId);

    USHORT nFixedLen = mpSprmParser->DistanceToData(nId);
    USHORT nL = mpSprmParser->GetSprmSize(nId, pPos);

    if (rSprm.pReadFnc)
        (this->*rSprm.pReadFnc)(nId, pPos + nFixedLen, nL - nFixedLen);

    return nL;
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
