/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: column.cxx,v $
 * $Revision: 1.36 $
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

#include "column.hxx"

#include "hintids.hxx"
#include <svx/htmlmode.hxx>
#include <svx/borderline.hxx>
#include <svx/boxitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/sizeitem.hxx>
#include "svx/frmdiritem.hxx"
#include <svtools/ctloptions.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/msgbox.hxx>
#include <swmodule.hxx>

#include <helpid.h>
#include "globals.hrc"
#include "swtypes.hxx"
#include "wrtsh.hxx"
#include "view.hxx"
#include "docsh.hxx"
#include "uitool.hxx"
#include "cmdid.h"
#include "viewopt.hxx"
#include "format.hxx"
#include "frmmgr.hxx"
#include "frmdlg.hxx"
#include "colmgr.hxx"
#include "prcntfld.hxx"
#include "paratr.hxx"
#include "frmui.hrc"
#include "poolfmt.hrc"
#include "column.hrc"
#include <section.hxx>
#include <docary.hxx>
#include <pagedesc.hxx>

#define FRAME_FORMAT_WIDTH 1000

// sw/inc/fmtclds.hxx
SV_IMPL_PTRARR( SwColumns, SwColumnPtr )

/*--------------------------------------------------------------------
    Beschreibung:  Statische Daten
 --------------------------------------------------------------------*/

static const USHORT __FAR_DATA nLines[] = {
    DEF_LINE_WIDTH_0,
    DEF_LINE_WIDTH_1,
    DEF_LINE_WIDTH_2,
    DEF_LINE_WIDTH_3,
    DEF_LINE_WIDTH_4
};

static const USHORT nLineCount = sizeof(nLines) / sizeof(nLines[0]);
static const USHORT nVisCols = 3;

inline BOOL IsMarkInSameSection( SwWrtShell& rWrtSh, const SwSection* pSect )
{
    rWrtSh.SwapPam();
    BOOL bRet = pSect == rWrtSh.GetCurrSection();
    rWrtSh.SwapPam();
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwColumnDlg::SwColumnDlg(Window* pParent, SwWrtShell& rSh) :
    SfxModalDialog(pParent, SW_RES(DLG_COLUMN)),
    aOK(this, 		SW_RES(PB_OK)),
    aCancel(this, 	SW_RES(PB_CANCEL)),
    aHelp(this, 	SW_RES(PB_HELP)),
    aApplyToFT(this, SW_RES(FT_APPLY_TO)),
    aApplyToLB(this, SW_RES(LB_APPLY_TO)),
    rWrtShell(rSh),
    pPageSet(0),
    pSectionSet(0),
    pSelectionSet(0),
    pFrameSet(0),
    nOldSelection(0),
    nSelectionWidth(0),
    bPageChanged(FALSE),
    bSectionChanged(FALSE),
    bSelSectionChanged(FALSE),
    bFrameChanged(FALSE)
{
    FreeResource();

    SwRect aRect;
    rWrtShell.CalcBoundRect(aRect, FLY_IN_CNTNT);

    nSelectionWidth = aRect.Width();

    SfxItemSet* pColPgSet = 0;
    static USHORT __READONLY_DATA aSectIds[] = { RES_COL, RES_COL,
                                                RES_FRM_SIZE, RES_FRM_SIZE,
                                                RES_COLUMNBALANCE, RES_FRAMEDIR,
                                                0 };

    const SwSection* pCurrSection = rWrtShell.GetCurrSection();
    USHORT nFullSectCnt = rWrtShell.GetFullSelectedSectionCount();
    if( pCurrSection && ( !rWrtShell.HasSelection() || 0 != nFullSectCnt ))
    {
        nSelectionWidth = rSh.GetSectionWidth(*pCurrSection->GetFmt());
        if ( !nSelectionWidth )
            nSelectionWidth = USHRT_MAX;
        pSectionSet = new SfxItemSet( rWrtShell.GetAttrPool(), aSectIds );
        pSectionSet->Put( pCurrSection->GetFmt()->GetAttrSet() );
        pColPgSet = pSectionSet;
        aApplyToLB.RemoveEntry( aApplyToLB.GetEntryPos(
                                        (void*)( 1 >= nFullSectCnt
                                                    ? LISTBOX_SECTIONS
                                                    : LISTBOX_SECTION )));
    }
    else
    {
        aApplyToLB.RemoveEntry(aApplyToLB.GetEntryPos( (void*) LISTBOX_SECTION ));
        aApplyToLB.RemoveEntry(aApplyToLB.GetEntryPos( (void*) LISTBOX_SECTIONS ));
    }

    if( rWrtShell.HasSelection() && rWrtShell.IsInsRegionAvailable() &&
        ( !pCurrSection || ( 1 != nFullSectCnt &&
            IsMarkInSameSection( rWrtShell, pCurrSection ) )))
    {
        pSelectionSet = new SfxItemSet( rWrtShell.GetAttrPool(), aSectIds );
        pColPgSet = pSelectionSet;
    }
    else
        aApplyToLB.RemoveEntry(aApplyToLB.GetEntryPos( (void*) LISTBOX_SELECTION ));

    if( rWrtShell.GetFlyFrmFmt() )
    {
        const SwFrmFmt* pFmt = rSh.GetFlyFrmFmt() ;
        pFrameSet = new SfxItemSet(rWrtShell.GetAttrPool(), aSectIds );
        pFrameSet->Put(pFmt->GetFrmSize());
        pFrameSet->Put(pFmt->GetCol());
        pColPgSet = pFrameSet;
    }
    else
        aApplyToLB.RemoveEntry(aApplyToLB.GetEntryPos( (void*) LISTBOX_FRAME ));


    USHORT nPagePos = aApplyToLB.GetEntryPos( (void*) LISTBOX_PAGE );
    const SwPageDesc* pPageDesc = rWrtShell.GetSelectedPageDescs();
    if( pPageDesc )
    {
        pPageSet = new SfxItemSet( rWrtShell.GetAttrPool(),
                                    RES_COL, RES_COL,
                                    RES_FRM_SIZE, RES_FRM_SIZE,
                                    RES_LR_SPACE, RES_LR_SPACE,
                                    0 );

        String sPageStr = aApplyToLB.GetEntry(nPagePos);
        aApplyToLB.RemoveEntry( nPagePos );
        sPageStr += pPageDesc->GetName();
        aApplyToLB.InsertEntry( sPageStr, nPagePos );
        aApplyToLB.SetEntryData( nPagePos, (void*) LISTBOX_PAGE);

        const SwFrmFmt &rFmt = pPageDesc->GetMaster();
        nPageWidth = rFmt.GetFrmSize().GetSize().Width();

        const SvxLRSpaceItem& rLRSpace = (const SvxLRSpaceItem&)rFmt.GetLRSpace();
        const SvxBoxItem& rBox = (const SvxBoxItem&) rFmt.GetBox();
        nPageWidth -= rLRSpace.GetLeft() + rLRSpace.GetRight() + rBox.GetDistance();

        pPageSet->Put(rFmt.GetCol());
        pPageSet->Put(rFmt.GetLRSpace());
        pColPgSet = pPageSet;
    }
    else
        aApplyToLB.RemoveEntry( nPagePos );


    ASSERT( pColPgSet, "" );
    // TabPage erzeugen
    SwColumnPage* pPage = (SwColumnPage*) SwColumnPage::Create( this,
                                                                *pColPgSet );
    pTabPage = pPage;

    //Groesse anpassen
    Size aPageSize(pTabPage->GetSizePixel());
    Size aDlgSize(GetOutputSizePixel());
    aDlgSize.Height() = aPageSize.Height();
    SetOutputSizePixel(aDlgSize);
    pTabPage->Show();

    aApplyToLB.SelectEntryPos(0);
    ObjectHdl(0);

    aApplyToLB.SetSelectHdl(LINK(this, SwColumnDlg, ObjectHdl));
    aOK.SetClickHdl(LINK(this, SwColumnDlg, OkHdl));
    //#i80458# if no columns can be set then disable OK
    if( !aApplyToLB.GetEntryCount() )
        aOK.Enable( sal_False );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwColumnDlg::~SwColumnDlg()
{
    delete pTabPage;
    delete pPageSet;
    delete pSectionSet;
    delete pSelectionSet;
}

/* -----------------26.05.99 11:40-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK(SwColumnDlg, ObjectHdl, ListBox*, pBox)
{
    SfxItemSet* pSet = 0;
    switch(nOldSelection)
    {
        case LISTBOX_SELECTION  :
            pSet = pSelectionSet;
        break;
        case LISTBOX_SECTION	:
            pSet = pSectionSet;
            bSectionChanged = TRUE;
        break;
        case LISTBOX_SECTIONS	:
            pSet = pSectionSet;
            bSelSectionChanged = TRUE;
        break;
        case LISTBOX_PAGE		:
            pSet = pPageSet;
            bPageChanged = TRUE;
        break;
        case LISTBOX_FRAME:
            pSet = pFrameSet;
            bFrameChanged = TRUE;
        break;
    }
    if(pBox)
    {
        pTabPage->FillItemSet(*pSet);
    }
    nOldSelection = (long)aApplyToLB.GetEntryData(aApplyToLB.GetSelectEntryPos());
    long nWidth = nSelectionWidth;
    switch(nOldSelection)
    {
        case LISTBOX_SELECTION  :
            pSet = pSelectionSet;
            if( pSelectionSet )
                pSet->Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth, nWidth));
        break;
        case LISTBOX_SECTION	:
        case LISTBOX_SECTIONS	:
            pSet = pSectionSet;
            pSet->Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth, nWidth));
        break;
        case LISTBOX_PAGE		:
            nWidth = nPageWidth;
            pSet = pPageSet;
            pSet->Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth, nWidth));
        break;
        case LISTBOX_FRAME:
            pSet = pFrameSet;
        break;
    }

    BOOL bIsSection = pSet == pSectionSet || pSet == pSelectionSet;
    pTabPage->ShowBalance(bIsSection);
    pTabPage->SetInSection(bIsSection);
    pTabPage->SetFrmMode(TRUE);
    pTabPage->SetPageWidth(nWidth);
    if( pSet )
        pTabPage->Reset(*pSet);
    return 0;
}
/* -----------------26.05.99 12:32-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK(SwColumnDlg, OkHdl, OKButton*, EMPTYARG)
{
    //aktuelle Selektion auswerten
    SfxItemSet* pSet = 0;
    switch(nOldSelection)
    {
        case LISTBOX_SELECTION  :
            pSet = pSelectionSet;
        break;
        case LISTBOX_SECTION	:
            pSet = pSectionSet;
            bSectionChanged = TRUE;
        break;
        case LISTBOX_SECTIONS	:
            pSet = pSectionSet;
            bSelSectionChanged = TRUE;
        break;
        case LISTBOX_PAGE		:
            pSet = pPageSet;
            bPageChanged = TRUE;
        break;
        case LISTBOX_FRAME:
            pSet = pFrameSet;
            bFrameChanged = TRUE;
        break;
    }
    pTabPage->FillItemSet(*pSet);

    if(pSelectionSet && SFX_ITEM_SET == pSelectionSet->GetItemState(RES_COL))
    {
        //Bereich mit Spalten einfuegen
        const SwFmtCol& rColItem = (const SwFmtCol&)pSelectionSet->Get(RES_COL);
        //nur, wenn es auch Spalten gibt!
        if(rColItem.GetNumCols() > 1)
            rWrtShell.GetView().GetViewFrame()->GetDispatcher()->Execute(
                FN_INSERT_REGION, SFX_CALLMODE_ASYNCHRON, *pSelectionSet );
    }

    if(pSectionSet && pSectionSet->Count() && bSectionChanged )
    {
        const SwSection* pCurrSection = rWrtShell.GetCurrSection();
        const SwSectionFmt* pFmt = pCurrSection->GetFmt();
        USHORT nNewPos = rWrtShell.GetSectionFmtPos( *pFmt );
        rWrtShell.ChgSection( nNewPos, *pCurrSection, pSectionSet );
    }

    if(pSectionSet && pSectionSet->Count() && bSelSectionChanged )
    {
        rWrtShell.SetSectionAttr( *pSectionSet );
    }

    if(pPageSet && SFX_ITEM_SET == pPageSet->GetItemState(RES_COL) && bPageChanged)
    {
        // aktuellen PageDescriptor ermitteln und damit den Set fuellen
        const USHORT nCurIdx = rWrtShell.GetCurPageDesc();
        SwPageDesc aPageDesc(rWrtShell.GetPageDesc(nCurIdx));
        SwFrmFmt &rFmt = aPageDesc.GetMaster();
        rFmt.SetAttr(pPageSet->Get(RES_COL));
        rWrtShell.ChgPageDesc(nCurIdx, aPageDesc);
    }
    if(pFrameSet && SFX_ITEM_SET == pFrameSet->GetItemState(RES_COL) && bFrameChanged)
    {
        SfxItemSet aTmp(*pFrameSet->GetPool(), RES_COL, RES_COL);
        aTmp.Put(*pFrameSet);
        rWrtShell.StartAction();
        rWrtShell.Push();
        rWrtShell.SetFlyFrmAttr( aTmp );
        //die Rahmenselektion wieder aufheben
        if(rWrtShell.IsFrmSelected())
        {
            rWrtShell.UnSelectFrm();
            rWrtShell.LeaveSelFrmMode();
        }
        rWrtShell.Pop();
        rWrtShell.EndAction();
    }
    EndDialog(RET_OK);
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

#if OSL_DEBUG_LEVEL < 2
inline
#endif
USHORT GetMaxWidth( SwColMgr* pColMgr, USHORT nCols )
{
    USHORT nMax = pColMgr->GetActualSize();
    if( --nCols )
        nMax -= pColMgr->GetGutterWidth() * nCols;
    return nMax;
}

static USHORT __FAR_DATA aPageRg[] = {
    RES_COL, RES_COL,
    0
};


DBG_NAME(columnhdl)



USHORT lcl_LineWidthToPos(ULONG nWidth)
{
    const USHORT nUShortWidth = (USHORT)nWidth;
    for(USHORT i = 0; i < nLineCount; ++i)
        if(nUShortWidth == nLines[i])
            return i;
    return 0;
}



void SwColumnPage::ResetColWidth()
{
    if( nCols )
    {
        USHORT nWidth = GetMaxWidth( pColMgr, nCols );
        nWidth = nWidth / nCols;

        for(USHORT i = 0; i < nCols; ++i)
            nColWidth[i] = (long) nWidth;
    }

}

/*--------------------------------------------------------------------
    Beschreibung:	Jetzt als TabPage
 --------------------------------------------------------------------*/



SwColumnPage::SwColumnPage(Window *pParent, const SfxItemSet &rSet)

    : SfxTabPage(pParent, SW_RES(TP_COLUMN), rSet),

    aClNrLbl(this,          SW_RES(FT_NUMBER  )),
    aCLNrEdt(this,          SW_RES(ED_NUMBER  )),
    aDefaultVS(this,        SW_RES(VS_DEFAULTS)),
    aBalanceColsCB(this,    SW_RES(CB_BALANCECOLS)),
    aFLGroup(this,          SW_RES(FL_COLUMNS )),

    aBtnUp(this,            SW_RES(BTN_DOWN)),
    aColumnFT(this,         SW_RES(FT_COLUMN)),
    aWidthFT(this,          SW_RES(FT_WIDTH)),
    aDistFT(this,           SW_RES(FT_DIST)),
    aLbl1(this,             SW_RES(FT_1)),
    aEd1(this,              SW_RES(ED_1)),
    aDistEd1(this,          SW_RES(ED_DIST1)),
    aLbl2(this,             SW_RES(FT_2)),
    aEd2(this,              SW_RES(ED_2)),
    aDistEd2(this,          SW_RES(ED_DIST2)),
    aLbl3(this,             SW_RES(FT_3)),
    aEd3(this,              SW_RES(ED_3)),
    aBtnDown(this,          SW_RES(BTN_UP)),
    aAutoWidthBox(this,     SW_RES(CB_AUTO_WIDTH)),

    aFLLayout(this,         SW_RES(FL_LAYOUT)),

    aLineTypeLbl(this,      SW_RES(FT_STYLE)),
    aLineTypeDLB(this,      SW_RES(LB_STYLE)),
    aLineHeightLbl(this,    SW_RES(FT_HEIGHT)),
    aLineHeightEdit(this,   SW_RES(ED_HEIGHT)),
    aLinePosLbl(this,       SW_RES(FT_POSITION)),
    aLinePosDLB(this,       SW_RES(LB_POSITION)),
    aFLLineType(this,       SW_RES(FL_LINETYPE)),

    aVertFL(this,         SW_RES(FL_VERT)),
    aPropertiesFL(  this,    SW_RES( FL_PROPERTIES    )),
    aTextDirectionFT( this,  SW_RES( FT_TEXTDIRECTION )),
    aTextDirectionLB( this,  SW_RES( LB_TEXTDIRECTION )),

    aPgeExampleWN(this,     SW_RES(WN_BSP)),
    aFrmExampleWN(this,     SW_RES(WN_BSP)),

    pColMgr(0),

    nFirstVis(0),
    nMinWidth(MINLAY),
    pModifiedField(0),
    bFormat(FALSE),
    bFrm(FALSE),
    bHtmlMode(FALSE),
    bLockUpdate(FALSE)
{
    USHORT i;

    FreeResource();
    SetExchangeSupport();

    aDefaultVS.SetHelpId(HID_COLUMN_VALUESET);
    aDefaultVS.SetColCount( 5 );
    aDefaultVS.SetStyle(  aDefaultVS.GetStyle()
                            | WB_ITEMBORDER
                            | WB_DOUBLEBORDER );

    for( i = 0; i < 5; i++)
        aDefaultVS.InsertItem( i + 1, i );

    aDefaultVS.SetSelectHdl(LINK(this, SwColumnPage, SetDefaultsHdl));

    // Controls fuer Zusaetzebereich beim MoreButton anmelden
    Link aCLNrLk = LINK(this, SwColumnPage, ColModify);
    aCLNrEdt.SetLoseFocusHdl(aCLNrLk);
    aCLNrEdt.SetUpHdl(aCLNrLk);
    aCLNrEdt.SetDownHdl(aCLNrLk);
    Link aLk = LINK(this, SwColumnPage, GapModify);
    aDistEd1.SetUpHdl(aLk);
    aDistEd1.SetDownHdl(aLk);
    aDistEd1.SetLoseFocusHdl(aLk);
    aDistEd2.SetUpHdl(aLk);
    aDistEd2.SetDownHdl(aLk);
    aDistEd2.SetLoseFocusHdl(aLk);

    aLk = LINK(this, SwColumnPage, EdModify);

    aEd1.SetUpHdl(aLk);
    aEd1.SetDownHdl(aLk);
    aEd1.SetLoseFocusHdl(aLk);

    aEd2.SetUpHdl(aLk);
    aEd2.SetDownHdl(aLk);
    aEd2.SetLoseFocusHdl(aLk);

    aEd3.SetUpHdl(aLk);
    aEd3.SetDownHdl(aLk);
    aEd3.SetLoseFocusHdl(aLk);

    aBtnUp.SetClickHdl(LINK(this, SwColumnPage, Up));
    aBtnDown.SetClickHdl(LINK(this, SwColumnPage, Down));
    aAutoWidthBox.SetClickHdl(LINK(this, SwColumnPage, AutoWidthHdl));

    aLk = LINK( this, SwColumnPage, UpdateColMgr );
    aLineTypeDLB.SetSelectHdl( aLk );
    aLineHeightEdit.SetModifyHdl( aLk );
    aLinePosDLB.SetSelectHdl( aLk );

        // Trennlinie
    aLineTypeDLB.SetUnit( FUNIT_POINT );
    aLineTypeDLB.SetSourceUnit( FUNIT_TWIP );
    for( i = 0; i < nLineCount; ++i )
        aLineTypeDLB.InsertEntry( 100 * nLines[ i ] );
}



SwColumnPage::~SwColumnPage()
{
    delete pColMgr;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwColumnPage::SetPageWidth(long nPageWidth)
{
    long nNewMaxWidth = static_cast< long >(aEd1.NormalizePercent(nPageWidth));

    aDistEd1.SetMax(nNewMaxWidth, FUNIT_TWIP);
    aDistEd2.SetMax(nNewMaxWidth, FUNIT_TWIP);
    aEd1.SetMax(nNewMaxWidth, FUNIT_TWIP);
    aEd2.SetMax(nNewMaxWidth, FUNIT_TWIP);
    aEd3.SetMax(nNewMaxWidth, FUNIT_TWIP);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



void SwColumnPage::Reset(const SfxItemSet &rSet)
{
    USHORT nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    if(nHtmlMode & HTMLMODE_ON)
    {
        bHtmlMode = TRUE;
        aAutoWidthBox.Enable(FALSE);
    }
    FieldUnit aMetric = ::GetDfltMetric(bHtmlMode);
    SetMetric(aEd1, aMetric);
    SetMetric(aEd2, aMetric);
    SetMetric(aEd3, aMetric);
    SetMetric(aDistEd1, aMetric);
    SetMetric(aDistEd2, aMetric);

    delete pColMgr;
    pColMgr = new SwColMgr(rSet);
    nCols 	= pColMgr->GetCount() ;
    aCLNrEdt.SetMax(Max((USHORT)aCLNrEdt.GetMax(), (USHORT)nCols));
    aCLNrEdt.SetLast(Max(nCols,(USHORT)aCLNrEdt.GetMax()));

    if(bFrm)
    {
        if(bFormat)                     // hier gibt es keine Size
            pColMgr->SetActualWidth(FRAME_FORMAT_WIDTH);
        else
        {
            const SwFmtFrmSize& rSize = (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE);
            const SvxBoxItem& rBox = (const SvxBoxItem&) rSet.Get(RES_BOX);
            pColMgr->SetActualWidth((USHORT)rSize.GetSize().Width() - rBox.GetDistance());
        }
    }
    if(aBalanceColsCB.IsVisible())
    {
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == rSet.GetItemState( RES_COLUMNBALANCE, FALSE, &pItem ))
            aBalanceColsCB.Check(!((const SwFmtNoBalancedColumns*)pItem)->GetValue());
        else
            aBalanceColsCB.Check( TRUE );
    }

    //text direction
    if( SFX_ITEM_AVAILABLE <= rSet.GetItemState( RES_FRAMEDIR ) )
    {
        const SvxFrameDirectionItem& rItem = (const SvxFrameDirectionItem&)rSet.Get(RES_FRAMEDIR);
        sal_uInt32 nVal  = rItem.GetValue();
        USHORT nPos = aTextDirectionLB.GetEntryPos( (void*) nVal );
        aTextDirectionLB.SelectEntryPos( nPos );
        aTextDirectionLB.SaveValue();
    }

    Init();
    ActivatePage( rSet );
}

/*--------------------------------------------------------------------
    Beschreibung:	TabPage erzeugen
 --------------------------------------------------------------------*/



SfxTabPage* SwColumnPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwColumnPage(pParent, rSet);
}

/*--------------------------------------------------------------------
    Beschreibung:	Attribute in den Set stopfen bei OK
 --------------------------------------------------------------------*/



BOOL SwColumnPage::FillItemSet(SfxItemSet &rSet)
{
    if(aCLNrEdt.HasChildPathFocus())
        aCLNrEdt.GetDownHdl().Call(&aCLNrEdt);
    // Im ItemSet setzen
    // Die aktuellen Einstellungen sind
    // schon vorhanden
    //
    const SfxPoolItem* pOldItem;
    const SwFmtCol& rCol = pColMgr->GetColumns();
    if(0 == (pOldItem = GetOldItem( rSet, RES_COL )) ||
                rCol != *pOldItem )
        rSet.Put(rCol);

    if(aBalanceColsCB.IsVisible() )
    {
        rSet.Put(SwFmtNoBalancedColumns(!aBalanceColsCB.IsChecked() ));
    }
    USHORT nPos;
    if( aTextDirectionLB.IsVisible() &&
        ( nPos = aTextDirectionLB.GetSelectEntryPos() ) !=
                                            aTextDirectionLB.GetSavedValue() )
    {
        sal_uInt32 nDirection = (sal_uInt32)(sal_IntPtr)aTextDirectionLB.GetEntryData( nPos );
        rSet.Put( SvxFrameDirectionItem( (SvxFrameDirection)nDirection, RES_FRAMEDIR));
    }
    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung:	ColumnManager updaten
 --------------------------------------------------------------------*/



IMPL_LINK( SwColumnPage, UpdateColMgr, void *, /*pField*/ )
{
    long nGutterWidth = pColMgr->GetGutterWidth();
    if(nCols > 1)
    {
            // Ermitteln, ob die schmalste Spalte zu schmal ist
            // fuer den eingestellten Spaltenabstand
        long nMin = nColWidth[0];
        USHORT i;

        for( i = 1; i < nCols; ++i)
            nMin = Min(nMin, nColWidth[i]);

        BOOL bAutoWidth = aAutoWidthBox.IsChecked();
        if(!bAutoWidth)
        {
            pColMgr->SetAutoWidth(FALSE);
                // falls der Benutzer nicht die Gesamtbreite vergeben hat,
                // den fehlenden Betrag auf die letzte Spalte addieren.
            long nSum = 0;
            for(i = 0; i < nCols; ++i)
                nSum += nColWidth[i];
            nGutterWidth = 0;
            for(i = 0; i < nCols - 1; ++i)
                nGutterWidth += nColDist[i];
            nSum += nGutterWidth;

            long nMaxW = pColMgr->GetActualSize();

            if( nSum < nMaxW  )
                nColWidth[nCols - 1] += nMaxW - nSum;

            pColMgr->SetColWidth( 0, static_cast< USHORT >(nColWidth[0] + (USHORT)nColDist[0]/2) );
            for( i = 1; i < nCols-1; ++i )
            {
                long nActDist = (nColDist[i] + nColDist[i - 1]) / 2;
                pColMgr->SetColWidth( i, (USHORT)nColWidth[i] + (USHORT)nActDist );
            }
            pColMgr->SetColWidth( nCols-1, static_cast< USHORT >(nColWidth[nCols-1] + nColDist[nCols -2]/2) );

        }

            // keins ist ausgeschaltet
        const USHORT nPos = aLineTypeDLB.GetSelectEntryPos();
        BOOL bEnable = 0 != nPos;
        aLineHeightEdit.Enable( bEnable );
        aLineHeightLbl.Enable( bEnable );
        if( !bEnable )
            pColMgr->SetNoLine();
        else if( LISTBOX_ENTRY_NOTFOUND != nPos )
        {
            pColMgr->SetLineWidthAndColor(nLines[nPos - 1], Color(COL_BLACK) );
            pColMgr->SetAdjust( SwColLineAdj(
                                    aLinePosDLB.GetSelectEntryPos() + 1) );
            pColMgr->SetLineHeightPercent((short)aLineHeightEdit.GetValue());
            bEnable = pColMgr->GetLineHeightPercent() != 100;
        }
        aLinePosLbl.Enable( bEnable );
        aLinePosDLB.Enable( bEnable );
    }
    else
    {
        pColMgr->NoCols();
        nCols = 0;
    }

    //Maximalwerte setzen
    aCLNrEdt.SetMax(Max(1L,
        Min(long(nMaxCols), long( pColMgr->GetActualSize() / (nGutterWidth + MINLAY)) )));
    aCLNrEdt.SetLast(aCLNrEdt.GetMax());
    aCLNrEdt.Reformat();

    //Beispielfenster anregen
    if(!bLockUpdate)
    {
        if(bFrm)
        {
            aFrmExampleWN.SetColumns( pColMgr->GetColumns() );
            aFrmExampleWN.Invalidate();
        }
        else
            aPgeExampleWN.Invalidate();
    }

    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:	Initialisierung
------------------------------------------------------------------------*/



void SwColumnPage::Init()
{
    aCLNrEdt.SetValue(nCols);

    BOOL bAutoWidth = pColMgr->IsAutoWidth() || bHtmlMode;
    aAutoWidthBox.Check( bAutoWidth );

    sal_Int32 nColumnWidthSum = 0;
    // Setzen der Breiten
    USHORT i;
    for(i = 0; i < nCols; ++i)
    {
        nColWidth[i] = pColMgr->GetColWidth(i);
        nColumnWidthSum += nColWidth[i];
        if(i < nCols - 1)
            nColDist[i] = pColMgr->GetGutterWidth(i);
    }

    if( 1 < nCols )
    {
        // #97495# make sure that the automatic column widht's are always equal
        if(bAutoWidth)
        {
            nColumnWidthSum /= nCols;
            for(i = 0; i < nCols; ++i)
                nColWidth[i] = nColumnWidthSum;
        }
        SwColLineAdj eAdj = pColMgr->GetAdjust();
        if( COLADJ_NONE == eAdj )		// der Dialog kennt kein NONE!
        {
            eAdj = COLADJ_TOP;
            //ohne Adjust auch kein Linientyp
            aLineTypeDLB.SelectEntryPos( 0 );
            aLineHeightEdit.SetValue( 100 );
        }
        else
        {
            aLineTypeDLB.SelectEntryPos( lcl_LineWidthToPos(( pColMgr->GetLineWidth() )) + 1);
            aLineHeightEdit.SetValue( pColMgr->GetLineHeightPercent() );
        }
        aLinePosDLB.SelectEntryPos( static_cast< USHORT >(eAdj - 1) );
    }
    else
    {
        aLinePosDLB.SelectEntryPos( 0 );
        aLineTypeDLB.SelectEntryPos( 0 );
        aLineHeightEdit.SetValue( 100 );
    }

    UpdateCols();
    Update();

        // Maximale Spaltenzahl setzen
        // Werte kleiner als 1 sind nicht erlaubt
    aCLNrEdt.SetMax(Max(1L,
        Min(long(nMaxCols), long( pColMgr->GetActualSize() / nMinWidth) )));
}

/*------------------------------------------------------------------------
 Beschreibung:	Die Anzahl der Spalten hat sich veraendert -- hier werden
                die Controls fuer die Bearbeitung der Spalten entsprechend
                der Spaltenzahl en- oder disabled.
                Falls es mehr als nVisCols (= 3) Spalten gibt, werden
                alle Edit enabled und die Buttons fuer das Scrollen
                ebenfalls.
                Andernfalls werden die Edits jeweils fuer die entsprechenden
                Spaltenzahl enabled; eine Spalte kann nicht bearbeitet werden.
------------------------------------------------------------------------*/



void SwColumnPage::UpdateCols()
{
    BOOL bEnableBtns= FALSE;
    BOOL bEnable12	= FALSE;
    BOOL bEnable3	= FALSE;
    const BOOL bEdit = !aAutoWidthBox.IsChecked();
    if ( nCols > nVisCols )
    {
        bEnableBtns = TRUE && !bHtmlMode;
        bEnable12 = bEnable3 = bEdit;
    }
    else if( bEdit )
    {
        // hier gibt es absichtlich kaum noch breaks
        switch(nCols)
        {
            case 3: bEnable3 = TRUE;
            case 2: bEnable12= TRUE; break;
            default: /* do nothing */;
        }
    }
    aEd1.Enable( bEnable12 );
    aDistEd1.Enable(nCols > 1);
    aEd2.Enable( bEnable12 );
    aDistEd2.Enable(bEnable3);
    aEd3.Enable( bEnable3  );
    aLbl1.Enable(bEnable12 );
    aLbl2.Enable(bEnable12 );
    aLbl3.Enable(bEnable3  );
    aBtnUp.Enable( bEnableBtns );
    aBtnDown.Enable( bEnableBtns );

    const BOOL bEnable = nCols > 1;
    if( !bEnable )
    {
        aLinePosDLB.Enable( FALSE );
        aLinePosLbl.Enable( FALSE );
    }
    aLineHeightEdit.Enable( bEnable );
    aLineHeightLbl.Enable( bEnable );
    aLineTypeDLB.Enable( bEnable );
    aLineTypeLbl.Enable( bEnable );
    aAutoWidthBox.Enable( bEnable && !bHtmlMode );
}

void SwColumnPage::SetLabels( USHORT nVis )
{
    String sLbl( '~' );

    String sLbl2( String::CreateFromInt32( nVis + 1 ));
    sLbl2.Insert(sLbl, sLbl2.Len() - 1);
    aLbl1.SetText(sLbl2);

    sLbl2 = String::CreateFromInt32( nVis + 2 );
    sLbl2.Insert(sLbl, sLbl2.Len() - 1);
    aLbl2.SetText(sLbl2);

    sLbl2 = String::CreateFromInt32( nVis + 3 );
    sLbl2.Insert(sLbl, sLbl2.Len() - 1);
    aLbl3.SetText(sLbl2);
}

/*------------------------------------------------------------------------
 Beschreibung:	Handler, der bei einer Veraenderung der Spaltenzahl
                gerufen wird.
                Eine Aenderung der Spaltenzahl ueberschreibt eventuelle
                Breiteneinstellungen des Benutzers; alle Spalten sind
                gleich breit.
------------------------------------------------------------------------*/

IMPL_LINK( SwColumnPage, ColModify, NumericField *, pNF )
{
    nCols = (USHORT)aCLNrEdt.GetValue();
    //#107890# the handler is also called from LoseFocus()
    //then no change has been made and thus no action should be taken
    // #i17816# changing the displayed types within the ValueSet
    //from two columns to two columns with different settings doesn't invalidate the
    // example windows in ::ColModify()
    if(!pNF ||(pColMgr->GetCount() != nCols))
    {
        if(pNF)
            aDefaultVS.SetNoSelection();
        long nDist = static_cast< long >(aDistEd1.DenormalizePercent(aDistEd1.GetValue(FUNIT_TWIP)));
        pColMgr->SetCount(nCols, (USHORT)nDist);
        for(USHORT i = 0; i < nCols; i++)
            nColDist[i] = nDist;
        nFirstVis = 0;
        SetLabels( nFirstVis );
        UpdateCols();
        ResetColWidth();
        Update();
    }

    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:	Modify- Handler fuer eine Aenderung der Spaltenbreite
                oder des Spaltenabstandes.
                Diese Aenderungen wirken sich erst zeitversetzt aus.
                Durch eine Aenderung der Spaltenbreite wird die automatische
                Berechnung der Spaltenbreite ausser Kraft gesetzt; erst
                eine Aenderung der Spaltenzahl kehrt wieder zu diesem
                Default zurueck.
------------------------------------------------------------------------*/



IMPL_LINK( SwColumnPage, GapModify, PercentField *, pFld )
{
    long nActValue = static_cast< long >(pFld->DenormalizePercent(pFld->GetValue(FUNIT_TWIP)));
    if(nCols < 2)
        return 0;
    if(aAutoWidthBox.IsChecked())
    {
        USHORT nMaxGap = pColMgr->GetActualSize() - nCols * MINLAY;
        DBG_ASSERT(nCols, "Abstand kann nicht ohne Spalten eingestellt werden")
        nMaxGap /= nCols - 1;
        if(nActValue > nMaxGap)
        {
            nActValue = nMaxGap;
            aDistEd1.SetPrcntValue(aDistEd1.NormalizePercent(nMaxGap), FUNIT_TWIP);
        }
        pColMgr->SetGutterWidth((USHORT)nActValue);
        for(USHORT i = 0; i < nCols; i++)
            nColDist[i] = nActValue;

        ResetColWidth();
        UpdateCols();
    }
    else

    {
        USHORT nOffset = 0;
        if(pFld == &aDistEd2)
        {
            nOffset = 1;
        }
        long nDiff = nActValue - nColDist[nFirstVis + nOffset];
        if(nDiff)
        {
            long nLeft = nColWidth[nFirstVis + nOffset];
            long nRight = nColWidth[nFirstVis + nOffset + 1];
            if(nLeft + nRight + 2 * MINLAY < nDiff)
                nDiff = nLeft + nRight - 2 * MINLAY;
            if(nDiff < nRight - MINLAY)
            {
                nRight -= nDiff;
            }
            else
            {
                long nTemp = nDiff - nRight + MINLAY;
                nRight = MINLAY;
                if(nLeft > nTemp - MINLAY)
                {
                    nLeft -= nTemp;
                    nTemp = 0;
                }
                else
                {
                    nTemp -= nLeft + MINLAY;
                    nLeft = MINLAY;
                }
                nDiff = nTemp;
            }
            nColWidth[nFirstVis + nOffset] = nLeft;
            nColWidth[nFirstVis + nOffset + 1] = nRight;
            nColDist[nFirstVis + nOffset] += nDiff;

            pColMgr->SetColWidth( nFirstVis + nOffset, USHORT(nLeft) );
            pColMgr->SetColWidth( nFirstVis + nOffset + 1, USHORT(nRight) );
            pColMgr->SetGutterWidth( USHORT(nColDist[nFirstVis + nOffset]), nFirstVis + nOffset );
        }

    }
    Update();
    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:
------------------------------------------------------------------------*/



IMPL_LINK( SwColumnPage, EdModify, PercentField *, pField )
{
    pModifiedField = pField;
    Timeout(0);
    return 0;
}
/*------------------------------------------------------------------------
 Beschreibung:	Handler hinter der Checkbox fuer automatische Breite.
                Ist die Box gecheckt, koennen keine expliziten Werte
                fuer die Spaltenbreite eingegeben werden.
------------------------------------------------------------------------*/



IMPL_LINK( SwColumnPage, AutoWidthHdl, CheckBox *, pBox )
{
    long nDist = static_cast< long >(aDistEd1.DenormalizePercent(aDistEd1.GetValue(FUNIT_TWIP)));
    pColMgr->SetCount(nCols, (USHORT)nDist);
    for(USHORT i = 0; i < nCols; i++)
        nColDist[i] = nDist;
    if(pBox->IsChecked())
    {
        pColMgr->SetGutterWidth(USHORT(nDist));
        ResetColWidth();
    }
    pColMgr->SetAutoWidth(pBox->IsChecked(), USHORT(nDist));
    UpdateCols();
    Update();
    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:	Raufscrollen der Inhalte der Edits.
------------------------------------------------------------------------*/

IMPL_LINK( SwColumnPage, Up, Button *, EMPTYARG )
{
    if( nFirstVis )
    {
        --nFirstVis;
        SetLabels( nFirstVis );
        Update();
    }
    return 0;
}
/*------------------------------------------------------------------------
 Beschreibung:	Runterscrollen der Inhalte der Edits.
------------------------------------------------------------------------*/

IMPL_LINK( SwColumnPage, Down, Button *, EMPTYARG )
{
    if( nFirstVis + nVisCols < nCols )
    {
        ++nFirstVis;
        SetLabels( nFirstVis );
        Update();
    }
    return 0;
}
/*------------------------------------------------------------------------
 Beschreibung:	Relikt aus alten Zeiten - jetzt direkt ohne time
 * 				Timer- Handler; angetriggert durch eine Aenderung der
                Spaltenbreite oder des Spaltenabstandes.
------------------------------------------------------------------------*/

IMPL_LINK( SwColumnPage, Timeout, Timer *, EMPTYARG )
{
    DBG_PROFSTART(columnhdl) ;
    if(pModifiedField)
    {
            // Finden der veraenderten Spalte
        USHORT nChanged = nFirstVis;
        if(pModifiedField == &aEd2)
            ++nChanged;
        else if(pModifiedField == &aEd3)
            nChanged += 2;
        /*else if(pModifiedField == &aEd4)
            nChanged += 3;*/

        long nNewWidth = (USHORT)
            pModifiedField->DenormalizePercent(pModifiedField->GetValue(FUNIT_TWIP));
        long nDiff = nNewWidth - nColWidth[nChanged];

        // wenn es die letzte Spalte ist
        if(nChanged == nCols - 1)
        {
            nColWidth[0] -= nDiff;
            if(nColWidth[0] < (long)nMinWidth)
            {
                nNewWidth -= nMinWidth - nColWidth[0];
                nColWidth[0] = nMinWidth;
            }

        }
        else if(nDiff)
        {
            nColWidth[nChanged + 1] -= nDiff;
            if(nColWidth[nChanged + 1] < (long) nMinWidth)
            {
                nNewWidth -= nMinWidth - nColWidth[nChanged + 1];
                nColWidth[nChanged + 1] = nMinWidth;
            }
        }
        nColWidth[nChanged] = nNewWidth;
        pModifiedField = 0;
    }
    Update();
    DBG_PROFSTOP(columnhdl) ;
    return 0;
}
/*------------------------------------------------------------------------
 Beschreibung:	Aktualisierung der Anzeige
------------------------------------------------------------------------*/



void SwColumnPage::Update()
{
    aBalanceColsCB.Enable(nCols > 1);
    if(nCols >= 2)
    {
        aEd1.SetPrcntValue(aEd1.NormalizePercent(nColWidth[nFirstVis]), FUNIT_TWIP);
        aDistEd1.SetPrcntValue(aDistEd1.NormalizePercent(nColDist[nFirstVis]), FUNIT_TWIP);
        aEd2.SetPrcntValue(aEd2.NormalizePercent(nColWidth[nFirstVis + 1]), FUNIT_TWIP);
        if(nCols >= 3)
        {
            aDistEd2.SetPrcntValue(aDistEd2.NormalizePercent(nColDist[nFirstVis + 1]), FUNIT_TWIP);
            aEd3.SetPrcntValue(aEd3.NormalizePercent(nColWidth[nFirstVis + 2]), FUNIT_TWIP);
        }
        else
        {
            aEd3.SetText(aEmptyStr);
            aDistEd2.SetText(aEmptyStr);
        }
    }
    else
    {
        aEd1.SetText(aEmptyStr);
        aEd2.SetText(aEmptyStr);
        aEd3.SetText(aEmptyStr);
        aDistEd1.SetText(aEmptyStr);
        aDistEd2.SetText(aEmptyStr);
    }
    UpdateColMgr(0);
}

/*--------------------------------------------------------------------
    Beschreibung:	Update Bsp
 --------------------------------------------------------------------*/



void SwColumnPage::ActivatePage(const SfxItemSet& rSet)
{
    if(!bFrm)
    {
        if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_PAGE_SIZE ))
        {
            const SvxSizeItem& rSize = (const SvxSizeItem&)rSet.Get(
                                                SID_ATTR_PAGE_SIZE);
            const SvxLRSpaceItem& rLRSpace = (const SvxLRSpaceItem&)rSet.Get(
                                                                RES_LR_SPACE );
            const SvxBoxItem& rBox = (const SvxBoxItem&) rSet.Get(RES_BOX);
            USHORT nActWidth = static_cast< USHORT >(rSize.GetSize().Width()
                            - rLRSpace.GetLeft() - rLRSpace.GetRight() - rBox.GetDistance());

            if( pColMgr->GetActualSize() != nActWidth)
            {
                pColMgr->SetActualWidth(nActWidth);
                ColModify( 0 );
                UpdateColMgr( 0 );
            }
        }
        aFrmExampleWN.Hide();
        aPgeExampleWN.UpdateExample( rSet, pColMgr );
        aPgeExampleWN.Show();

    }
    else
    {
        aPgeExampleWN.Hide();
        aFrmExampleWN.Show();

        // Size
        const SwFmtFrmSize& rSize = (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE);
        const SvxBoxItem& rBox = (const SvxBoxItem&) rSet.Get(RES_BOX);

        long nDistance = rBox.GetDistance();
        const USHORT nTotalWish = bFormat ? FRAME_FORMAT_WIDTH : USHORT(rSize.GetWidth() - 2 * nDistance);

        // Maximalwerte der Spaltenbreiten setzen
        SetPageWidth(nTotalWish);

        if(pColMgr->GetActualSize() != nTotalWish)
        {
            pColMgr->SetActualWidth(nTotalWish);
            Init();
        }
        BOOL bPercent;
        // im Rahmenformat nur relative Angaben
        if ( bFormat || rSize.GetWidthPercent() && rSize.GetWidthPercent() != 0xff)
        {
            // Wert fuer 100% setzen
            aEd1.SetRefValue(nTotalWish);
            aEd2.SetRefValue(nTotalWish);
            aEd3.SetRefValue(nTotalWish);
            aDistEd1.SetRefValue(nTotalWish);
            aDistEd2.SetRefValue(nTotalWish);

            // Auf %-Darstellung umschalten
            bPercent = TRUE;
        }
        else
            bPercent = FALSE;

        aEd1.ShowPercent(bPercent);
        aEd2.ShowPercent(bPercent);
        aEd3.ShowPercent(bPercent);
        aDistEd1.ShowPercent(bPercent);
        aDistEd2.ShowPercent(bPercent);
        aDistEd1.MetricField::SetMin(0);
        aDistEd2.MetricField::SetMin(0);
    }
    Update();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



int SwColumnPage::DeactivatePage(SfxItemSet *_pSet)
{
    if(_pSet)
        FillItemSet(*_pSet);

    return TRUE;
}



USHORT* SwColumnPage::GetRanges()
{
    return aPageRg;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



IMPL_LINK( SwColumnPage, SetDefaultsHdl, ValueSet *, pVS )
{
    USHORT nItem = pVS->GetSelectItemId();
    if( nItem < 4 )
    {
        aCLNrEdt.SetValue( nItem );
        aAutoWidthBox.Check();
        aDistEd1.SetPrcntValue(0);
        ColModify(0);
    }
    else
    {
        bLockUpdate = TRUE;
        aCLNrEdt.SetValue( 2 );
        aAutoWidthBox.Check(FALSE);
        aDistEd1.SetPrcntValue(0);
        ColModify(0);
        // jetzt noch das Breitenverhaeltnisse auf 2 : 1 bzw. 1 : 2 stellen
        USHORT nSmall = pColMgr->GetActualSize()  / 3;
        if(nItem == 4)
        {
            aEd2.SetPrcntValue(aEd2.NormalizePercent(long(nSmall)), FUNIT_TWIP);
            pModifiedField = &aEd2;
        }
        else
        {
            aEd1.SetPrcntValue(aEd1.NormalizePercent(long(nSmall)), FUNIT_TWIP);
            pModifiedField = &aEd1;
        }
        bLockUpdate = FALSE;
        Timeout(0);

    }
    return 0;
}

/*-----------------25.10.96 11.41-------------------

--------------------------------------------------*/


void SwColumnPage::SetFrmMode(BOOL bMod)
{
    bFrm = bMod;
}
/* -----------------------------2002/06/19 13:08------------------------------

 ---------------------------------------------------------------------------*/
void SwColumnPage::SetInSection(BOOL bSet)
{
    if(!SW_MOD()->GetCTLOptions().IsCTLFontEnabled())
        return;

    aVertFL.Show(bSet);
    aPropertiesFL.Show(bSet);
    aTextDirectionFT.Show(bSet);
    aTextDirectionLB.Show(bSet);
    if(bSet)
    {
        //resize line type FixedLine
        Point aLtPos = aFLLineType.GetPosPixel();
        Point aPropPos = aPropertiesFL.GetPosPixel();
        Size aSz = aFLLineType.GetSizePixel();
        aSz.Width() = aPropPos.X() - aLtPos.X() - LogicToPixel(Size(8, 8), MAP_APPFONT).Width();
        aFLLineType.SetSizePixel(aSz);
    }
    else
    {
        Size aSz = aFLLineType.GetSizePixel();
        aSz.Width() = LogicToPixel(Size(248, 248), MAP_APPFONT).Width();
        aFLLineType.SetSizePixel(aSz);
    }
}

/*-----------------07.03.97 08.33-------------------

--------------------------------------------------*/


void ColumnValueSet::UserDraw( const UserDrawEvent& rUDEvt )
{
    OutputDevice*  pDev = rUDEvt.GetDevice();
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

    Rectangle aRect = rUDEvt.GetRect();
    USHORT	nItemId = rUDEvt.GetItemId();
    long nRectWidth = aRect.GetWidth();
    long nRectHeight = aRect.GetHeight();

    Point aBLPos = aRect.TopLeft();
    Color aFillColor(pDev->GetFillColor());
    Color aLineColor(pDev->GetLineColor());
    pDev->SetFillColor(rStyleSettings.GetFieldColor());
    pDev->SetLineColor(SwViewOption::GetFontColor());

    long nStep = Abs(Abs(nRectHeight * 95 /100) / 11);
    long nTop = (nRectHeight - 11 * nStep ) / 2;
    USHORT nCols = 0;
    long nStarts[3];
    long nEnds[3];
    nStarts[0] = nRectWidth * 10 / 100;
    switch( nItemId )
    {
        case 1:
            nEnds[0] = nRectWidth * 9 / 10;
            nCols = 1;
        break;
        case 2: nCols = 2;
            nEnds[0] = nRectWidth * 45 / 100;
            nStarts[1] = nEnds[0] + nStep;
            nEnds[1] = nRectWidth * 9 / 10;
        break;
        case 3: nCols = 3;
            nEnds[0] 	= nRectWidth * 30 / 100;
            nStarts[1] 	= nEnds[0] + nStep;
            nEnds[1] 	= nRectWidth * 63 / 100;
            nStarts[2] 	= nEnds[1] + nStep;
            nEnds[2] 	= nRectWidth * 9 / 10;
        break;
        case 4: nCols = 2;
            nEnds[0] = nRectWidth * 63 / 100;
            nStarts[1] = nEnds[0] + nStep;
            nEnds[1] = nRectWidth * 9 / 10;
        break;
        case 5: nCols = 2;
            nEnds[0] = nRectWidth * 30 / 100;
            nStarts[1] = nEnds[0] + nStep;
            nEnds[1] = nRectWidth * 9 / 10;
        break;
    }
    for(USHORT j = 0; j < nCols; j++ )
    {
        Point aStart(aBLPos.X() + nStarts[j], 0);
        Point aEnd(aBLPos.X() + nEnds[j], 0);
        for( USHORT i = 0; i < 12; i ++)
        {
            aStart.Y() = aEnd.Y() = aBLPos.Y() + nTop + i * nStep;
            pDev->DrawLine(aStart, aEnd);
        }
    }
    pDev->SetFillColor(aFillColor);
    pDev->SetLineColor(aLineColor);
}

/*-----------------07.03.97 08.48-------------------

--------------------------------------------------*/

ColumnValueSet::~ColumnValueSet()
{
}
/* -----------------------------02.04.2002 16:01------------------------------

 ---------------------------------------------------------------------------*/
void ColumnValueSet::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        Format();
    }
    ValueSet::DataChanged( rDCEvt );
}

