/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: envfmt.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 11:41:38 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif




#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#include <tools/pstm.hxx>

#define _SVSTDARR_LONGSSORT
#define _SVSTDARR_LONGS
#include <svtools/svstdarr.hxx>

#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _SVX_TSTPITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _BASESH_HXX
#include <basesh.hxx>
#endif
#ifndef _DRPCPS_HXX
#include <drpcps.hxx>
#endif
#ifndef _ENVFMT_HXX
#include <envfmt.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _PARDLG_HXX
#include "swuipardlg.hxx"
#endif
#ifndef _PATTERN_HXX
#include <pattern.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _UIBORDER_HXX
#include <uiborder.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif

#ifndef _ENVFMT_HRC
#include <envfmt.hrc>
#endif

#include "swabstdlg.hxx"
#include "chrdlg.hrc"

namespace swui
{
    SwAbstractDialogFactory * GetFactory();
}

static PopupMenu *pMenu;
static long lUserW = 5669; // 10 cm
static long lUserH = 5669; // 10 cm

// --------------------------------------------------------------------------


SwEnvFmtPage::SwEnvFmtPage(Window* pParent, const SfxItemSet& rSet) :

    SfxTabPage(pParent, SW_RES(TP_ENV_FMT), rSet),

    aAddrFL             (this, SW_RES( FL_ADDRESSEE )),
    aAddrPosInfo   		(this, SW_RES( TXT_ADDR_POS )),
    aAddrLeftText  		(this, SW_RES( TXT_ADDR_LEFT )),
    aAddrLeftField 		(this, SW_RES( FLD_ADDR_LEFT )),
    aAddrTopText   		(this, SW_RES( TXT_ADDR_TOP )),
    aAddrTopField  		(this, SW_RES( FLD_ADDR_TOP )),
    aAddrFormatInfo		(this, SW_RES( TXT_ADDR_FORMAT )),
    aAddrEditButton		(this, SW_RES( BTN_ADDR_EDIT )),
    aSendFL             (this, SW_RES( FL_SENDER )),
    aSendPosInfo		(this, SW_RES( TXT_SEND_POS )),
    aSendLeftText  		(this, SW_RES( TXT_SEND_LEFT )),
    aSendLeftField 		(this, SW_RES( FLD_SEND_LEFT )),
    aSendTopText   		(this, SW_RES( TXT_SEND_TOP )),
    aSendTopField  		(this, SW_RES( FLD_SEND_TOP )),
    aSendFormatInfo		(this, SW_RES( TXT_SEND_FORMAT )),
    aSendEditButton		(this, SW_RES( BTN_SEND_EDIT )),
    aSizeFL             (this, SW_RES( FL_SIZE )),
    aSizeFormatText		(this, SW_RES( TXT_SIZE_FORMAT )),
    aSizeFormatBox 		(this, SW_RES( BOX_SIZE_FORMAT )),
    aSizeWidthText 		(this, SW_RES( TXT_SIZE_WIDTH )),
    aSizeWidthField		(this, SW_RES( FLD_SIZE_WIDTH )),
    aSizeHeightText		(this, SW_RES( TXT_SIZE_HEIGHT )),
    aSizeHeightField	(this, SW_RES( FLD_SIZE_HEIGHT )),
    aPreview       		(this, SW_RES( WIN_PREVIEW ))

{
    FreeResource();
    SetExchangeSupport();

    // Metriken
    FieldUnit aMetric = ::GetDfltMetric(FALSE);
    SetMetric(aAddrLeftField,   aMetric);
    SetMetric(aAddrTopField,    aMetric);
    SetMetric(aSendLeftField,   aMetric);
    SetMetric(aSendTopField,    aMetric);
    SetMetric(aSizeWidthField,  aMetric);
    SetMetric(aSizeHeightField, aMetric);

    // Menues einhaengen
    ::pMenu = new PopupMenu(SW_RES(MNU_EDIT));
    aAddrEditButton.SetPopupMenu(::pMenu);
    aSendEditButton.SetPopupMenu(::pMenu);

    // Handler installieren
    Link aLk = LINK(this, SwEnvFmtPage, ModifyHdl);
    aAddrLeftField  .SetUpHdl( aLk );
    aAddrTopField   .SetUpHdl( aLk );
    aSendLeftField  .SetUpHdl( aLk );
    aSendTopField   .SetUpHdl( aLk );
    aSizeWidthField .SetUpHdl( aLk );
    aSizeHeightField.SetUpHdl( aLk );

    aAddrLeftField  .SetDownHdl( aLk );
    aAddrTopField   .SetDownHdl( aLk );
    aSendLeftField  .SetDownHdl( aLk );
    aSendTopField   .SetDownHdl( aLk );
    aSizeWidthField .SetDownHdl( aLk );
    aSizeHeightField.SetDownHdl( aLk );

    aAddrLeftField  .SetLoseFocusHdl( aLk );
    aAddrTopField   .SetLoseFocusHdl( aLk );
    aSendLeftField  .SetLoseFocusHdl( aLk );
    aSendTopField   .SetLoseFocusHdl( aLk );
    aSizeWidthField .SetLoseFocusHdl( aLk );
    aSizeHeightField.SetLoseFocusHdl( aLk );

    aLk = LINK(this, SwEnvFmtPage, EditHdl );
    aAddrEditButton.SetSelectHdl( aLk );
    aSendEditButton.SetSelectHdl( aLk );

    aPreview.SetBorderStyle( WINDOW_BORDER_MONO );

    aSizeFormatBox     .SetSelectHdl(LINK(this, SwEnvFmtPage, FormatHdl));

    // aSizeFormatBox
    for (USHORT i = SVX_PAPER_A3; i <= SVX_PAPER_KAI32BIG; i++)
    {
        if (i != SVX_PAPER_USER)
        {
            String aPaperName = SvxPaperInfo::GetName((SvxPaper) i),
                   aEntryName;

            USHORT nPos   = 0;
            BOOL   bFound = FALSE;
            while (nPos < aSizeFormatBox.GetEntryCount() && !bFound)
            {
                aEntryName = aSizeFormatBox.GetEntry(i);
                if (aEntryName < aPaperName)
                    nPos++;
                else
                    bFound = TRUE;
            }
            aSizeFormatBox.InsertEntry(aPaperName, nPos);
            aIDs.Insert((USHORT) i, nPos);
        }
    }
    aSizeFormatBox.InsertEntry(SvxPaperInfo::GetName(SVX_PAPER_USER));
    aIDs.Insert((USHORT) SVX_PAPER_USER, aIDs.Count());

}

// --------------------------------------------------------------------------


__EXPORT SwEnvFmtPage::~SwEnvFmtPage()
{
    aAddrEditButton.SetPopupMenu(0);
    aSendEditButton.SetPopupMenu(0);
    delete ::pMenu;
}

// --------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwEnvFmtPage, ModifyHdl, Edit *, pEdit )
{
    long lWVal = static_cast< long >(GetFldVal(aSizeWidthField ));
    long lHVal = static_cast< long >(GetFldVal(aSizeHeightField));

    long lWidth  = Max(lWVal, lHVal);
    long lHeight = Min(lWVal, lHVal);

    if (pEdit == &aSizeWidthField || pEdit == &aSizeHeightField)
    {
        SvxPaper ePaper = SvxPaperInfo::GetPaper(
            Size(lHeight, lWidth), MAP_TWIP, TRUE);
        for (USHORT i = 0; i < aIDs.Count(); i++)
            if (aIDs[i] == (USHORT)ePaper)
                aSizeFormatBox.SelectEntryPos(i);

        // Benutzergroesse merken
        if (aIDs[aSizeFormatBox.GetSelectEntryPos()] == (USHORT)SVX_PAPER_USER)
        {
            lUserW = lWidth ;
            lUserH = lHeight;
        }

        aSizeFormatBox.GetSelectHdl().Call(&aSizeFormatBox);
    }
    else
    {
        FillItem(GetParent()->aEnvItem);
        SetMinMax();
        aPreview.Invalidate();
    }
    return 0;
}
IMPL_LINK_INLINE_END( SwEnvFmtPage, ModifyHdl, Edit *, pEdit )

// --------------------------------------------------------------------------


IMPL_LINK( SwEnvFmtPage, EditHdl, MenuButton *, pButton )
{
    SwWrtShell* pSh = GetParent()->pSh;
    ASSERT(pSh, "Shell fehlt");

    // Collection-Ptr ermitteln
    BOOL bSender = pButton != &aAddrEditButton;

    SwTxtFmtColl* pColl = pSh->GetTxtCollFromPool( static_cast< USHORT >(
        bSender ? RES_POOLCOLL_SENDADRESS : RES_POOLCOLL_JAKETADRESS));
    ASSERT(pColl, "Text-Collection fehlt");

    switch (pButton->GetCurItemId())
    {
        case MID_CHAR:
        {
            SfxItemSet *pCollSet = GetCollItemSet(pColl, bSender);

            // Damit die Hintergrundfarbe nicht uebergebuegelt wird:
            SfxAllItemSet aTmpSet(*pCollSet);

            // Das CHRATR_BACKGROUND-Attribut wird fuer den Dialog in
            // ein RES_BACKGROUND verwandelt und wieder zurueck ...
            const SfxPoolItem *pTmpBrush;

            if( SFX_ITEM_SET == aTmpSet.GetItemState( RES_CHRATR_BACKGROUND,
                TRUE, &pTmpBrush ) )
            {
                SvxBrushItem aTmpBrush( *((SvxBrushItem*)pTmpBrush) );
                aTmpBrush.SetWhich( RES_BACKGROUND );
                aTmpSet.Put( aTmpBrush );
            }
            else
                aTmpSet.ClearItem( RES_BACKGROUND );

            SwAbstractDialogFactory* pFact = swui::GetFactory();
            DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

            SfxAbstractTabDialog* pDlg = pFact->CreateSwCharDlg( GetParent(), pSh->GetView(), aTmpSet, DLG_CHAR ,&pColl->GetName() );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");
            if (pDlg->Execute() == RET_OK)
            {
                SfxItemSet aOutputSet( *pDlg->GetOutputItemSet() );
                if( SFX_ITEM_SET == aOutputSet.GetItemState( RES_BACKGROUND,
                    FALSE, &pTmpBrush ) )
                {
                    SvxBrushItem aTmpBrush( *((SvxBrushItem*)pTmpBrush) );
                    aTmpBrush.SetWhich( RES_CHRATR_BACKGROUND );
                    pCollSet->Put( aTmpBrush );
                }
                aOutputSet.ClearItem( RES_BACKGROUND );
                //pColl->SetAttr( aTmpSet );
                pCollSet->Put(aOutputSet);
            }
            delete pDlg;
        }
        break;

        case MID_PARA:
        {
            SfxItemSet *pCollSet = GetCollItemSet(pColl, bSender);

            // Damit die Tabulatoren nicht uebergebuegelt werden:
            SfxAllItemSet aTmpSet(*pCollSet);

            // Tabulatoren, DefaultTabs ins ItemSet Stecken
            const SvxTabStopItem& rDefTabs = (const SvxTabStopItem&)
                pSh->GetView().GetCurShell()->GetPool().GetDefaultItem(RES_PARATR_TABSTOP);

            USHORT nDefDist = ::GetTabDist( rDefTabs );
            SfxUInt16Item aDefDistItem( SID_ATTR_TABSTOP_DEFAULTS, nDefDist );
            aTmpSet.Put( aDefDistItem );

            // Aktueller Tab
            SfxUInt16Item aTabPos( SID_ATTR_TABSTOP_POS, 0 );
            aTmpSet.Put( aTabPos );

            // linker Rand als Offset
            const long nOff = ((SvxLRSpaceItem&)aTmpSet.Get( RES_LR_SPACE )).
                                                                GetTxtLeft();
            SfxInt32Item aOff( SID_ATTR_TABSTOP_OFFSET, nOff );
            aTmpSet.Put( aOff );

            // BoxInfo setzen
            ::PrepareBoxInfo( aTmpSet, *pSh );

            SwParaDlg *pDlg = new SwParaDlg(GetParent(), pSh->GetView(), aTmpSet, DLG_ENVELOP, &pColl->GetName());

            if ( pDlg->Execute() == RET_OK )
            {
                // Defaults evtl umsetzen
                const SfxPoolItem* pItem = 0;
                SfxItemSet* pOutputSet = (SfxItemSet*)pDlg->GetOutputItemSet();
                USHORT nNewDist;
                if( SFX_ITEM_SET == pOutputSet->GetItemState( SID_ATTR_TABSTOP_DEFAULTS,
                    FALSE, &pItem ) &&
                    nDefDist != (nNewDist = ((SfxUInt16Item*)pItem)->GetValue()) )
                {
                    SvxTabStopItem aDefTabs( 0, 0, SVX_TAB_ADJUST_DEFAULT, RES_PARATR_TABSTOP );
                    MakeDefTabs( nNewDist, aDefTabs );
                    pSh->SetDefault( aDefTabs );
                    pOutputSet->ClearItem( SID_ATTR_TABSTOP_DEFAULTS );
                }
                if( pOutputSet->Count() )
                {
                    //pColl->SetAttr( *pOutputSet );
                    pCollSet->Put(*pOutputSet);
                }
            }
            delete pDlg;
        }
        break;
    }
    return 0;
}

/*------------------------------------------------------------------------
  Beschreibung: Ein temporaeres Itemset, das bei Abbruch verworfen wird
------------------------------------------------------------------------*/

SfxItemSet *SwEnvFmtPage::GetCollItemSet(SwTxtFmtColl* pColl, BOOL bSender)
{
    SfxItemSet *&pAddrSet = bSender ? GetParent()->pSenderSet : GetParent()->pAddresseeSet;

    if (!pAddrSet)
    {
        // Range ermitteln (Ranges beider Itemsets mergen)
        const USHORT *pRanges = pColl->GetAttrSet().GetRanges();

        static USHORT __READONLY_DATA aRanges[] =
        {
            RES_PARATR_BEGIN, RES_PARATR_ADJUST,
            RES_PARATR_TABSTOP, RES_PARATR_END-1,
            RES_LR_SPACE, RES_UL_SPACE,
            RES_BACKGROUND, RES_SHADOW,
            SID_ATTR_TABSTOP_POS, SID_ATTR_TABSTOP_POS,
            SID_ATTR_TABSTOP_DEFAULTS, SID_ATTR_TABSTOP_DEFAULTS,
            SID_ATTR_TABSTOP_OFFSET, SID_ATTR_TABSTOP_OFFSET,
            SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER,
            0, 0
        };

        // BruteForce-Merge, weil MergeRange in SvTools buggy ist:
        USHORT i = 0;
        SvLongsSort aMergedRanges( 0, 10 );

        while (pRanges[i])
        {
            for (USHORT nPos = pRanges[i]; nPos <= pRanges[i+1]; nPos++)
                aMergedRanges.Insert(nPos);
            i += 2;
        }

        i = 0;

        while (aRanges[i])
        {
            for (USHORT nPos = aRanges[i]; nPos <= aRanges[i+1]; nPos++)
                aMergedRanges.Insert(nPos);
            i += 2;
        }

        // Ranges kompaktieren
        SvLongs aCompactedRanges( 0, 10 );

        aCompactedRanges.Insert(aMergedRanges[0], aCompactedRanges.Count());

        for (i = 0; i < aMergedRanges.Count(); i++)
        {
            while (i + 1 < aMergedRanges.Count() &&
                aMergedRanges[i+1] - aMergedRanges[i] == 1)
            {
                i++;
            }
            long nEnd = aMergedRanges[i];
            aCompactedRanges.Insert(nEnd, aCompactedRanges.Count());

            if (i + 1 < aMergedRanges.Count())
            {
                long nStart = aMergedRanges[i+1];
                aCompactedRanges.Insert(nStart, aCompactedRanges.Count());
            }
        }

        // Neue Ranges erzeugen
        USHORT *pNewRanges = new USHORT[aCompactedRanges.Count() + 1];
        for (i = 0; i < aCompactedRanges.Count(); i++)
            pNewRanges[i] = (USHORT)aCompactedRanges[i];

        pNewRanges[i] = 0;

        pAddrSet = new SfxItemSet(GetParent()->pSh->GetView().GetCurShell()->GetPool(),
                                pNewRanges);

        pAddrSet->Put(pColl->GetAttrSet());
        delete[] pNewRanges;
    }

    return pAddrSet;
}

// --------------------------------------------------------------------------


IMPL_LINK( SwEnvFmtPage, FormatHdl, ListBox *, EMPTYARG )
{
    long lWidth;
    long lHeight;
    long lSendFromLeft;
    long lSendFromTop;
    long lAddrFromLeft;
    long lAddrFromTop;

    USHORT nPaper = aIDs[aSizeFormatBox.GetSelectEntryPos()];
    if (nPaper != (USHORT)SVX_PAPER_USER)
    {
        Size aSz = SvxPaperInfo::GetPaperSize((SvxPaper)nPaper);
        lWidth  = Max(aSz.Width(), aSz.Height());
        lHeight = Min(aSz.Width(), aSz.Height());
    }
    else
    {
        lWidth  = lUserW;
        lHeight = lUserH;
    }

    lSendFromLeft = 566;            // 1cm
    lSendFromTop  = 566;            // 1cm
    lAddrFromLeft = lWidth  / 2;
    lAddrFromTop  = lHeight / 2;

    SetFldVal(aAddrLeftField, lAddrFromLeft);
    SetFldVal(aAddrTopField , lAddrFromTop );
    SetFldVal(aSendLeftField, lSendFromLeft);
    SetFldVal(aSendTopField , lSendFromTop );

    SetFldVal(aSizeWidthField , lWidth );
    SetFldVal(aSizeHeightField, lHeight);

    SetMinMax();

    FillItem(GetParent()->aEnvItem);
    aPreview.Invalidate();
    return 0;
}

// --------------------------------------------------------------------------


void SwEnvFmtPage::SetMinMax()
{
    long lWVal = static_cast< long >(GetFldVal(aSizeWidthField ));
    long lHVal = static_cast< long >(GetFldVal(aSizeHeightField));

    long lWidth  = Max(lWVal, lHVal),
         lHeight = Min(lWVal, lHVal);

    // Min und Max
    aAddrLeftField.SetMin((long) 100 * (GetFldVal(aSendLeftField) + 566), FUNIT_TWIP);
    aAddrLeftField.SetMax((long) 100 * (lWidth  - 2 * 566), FUNIT_TWIP);
    aAddrTopField .SetMin((long) 100 * (GetFldVal(aSendTopField ) + 2 * 566), FUNIT_TWIP);
    aAddrTopField .SetMax((long) 100 * (lHeight - 2 * 566), FUNIT_TWIP);
    aSendLeftField.SetMin((long) 100 * (566), FUNIT_TWIP);
    aSendLeftField.SetMax((long) 100 * (GetFldVal(aAddrLeftField) - 566), FUNIT_TWIP);
    aSendTopField .SetMin((long) 100 * (566), FUNIT_TWIP);
    aSendTopField .SetMax((long) 100 * (GetFldVal(aAddrTopField ) - 2 * 566), FUNIT_TWIP);

    // First und Last
    aAddrLeftField.SetFirst(aAddrLeftField.GetMin());
    aAddrLeftField.SetLast (aAddrLeftField.GetMax());
    aAddrTopField .SetFirst(aAddrTopField .GetMin());
    aAddrTopField .SetLast (aAddrTopField .GetMax());
    aSendLeftField.SetFirst(aSendLeftField.GetMin());
    aSendLeftField.SetLast (aSendLeftField.GetMax());
    aSendTopField .SetFirst(aSendTopField .GetMin());
    aSendTopField .SetLast (aSendTopField .GetMax());

    // Fields neu formatieren
    aAddrLeftField  .Reformat();
    aAddrTopField   .Reformat();
    aSendLeftField  .Reformat();
    aSendTopField   .Reformat();
    aSizeWidthField .Reformat();
    aSizeHeightField.Reformat();
}

// --------------------------------------------------------------------------


SfxTabPage* __EXPORT SwEnvFmtPage::Create(Window* pParent, const SfxItemSet& rSet)
{
    return new SwEnvFmtPage(pParent, rSet);
}

// ----------------------------------------------------------------------------


void __EXPORT SwEnvFmtPage::ActivatePage(const SfxItemSet& rSet)
{
    SfxItemSet aSet(rSet);
    aSet.Put(GetParent()->aEnvItem);
    Reset(aSet);
}

// ----------------------------------------------------------------------------


int __EXPORT SwEnvFmtPage::DeactivatePage(SfxItemSet* _pSet)
{
    if( _pSet )
        FillItemSet(*_pSet);
    return SfxTabPage::LEAVE_PAGE;
}

// --------------------------------------------------------------------------


void SwEnvFmtPage::FillItem(SwEnvItem& rItem)
{
    rItem.lAddrFromLeft = static_cast< sal_Int32 >(GetFldVal(aAddrLeftField));
    rItem.lAddrFromTop  = static_cast< sal_Int32 >(GetFldVal(aAddrTopField ));
    rItem.lSendFromLeft = static_cast< sal_Int32 >(GetFldVal(aSendLeftField));
    rItem.lSendFromTop  = static_cast< sal_Int32 >(GetFldVal(aSendTopField ));

    USHORT nPaper = aIDs[aSizeFormatBox.GetSelectEntryPos()];
    if (nPaper == (USHORT)SVX_PAPER_USER)
    {
        long lWVal = static_cast< long >(GetFldVal(aSizeWidthField ));
        long lHVal = static_cast< long >(GetFldVal(aSizeHeightField));
        rItem.lWidth  = Max(lWVal, lHVal);
        rItem.lHeight = Min(lWVal, lHVal);
    }
    else
    {
        long lWVal = SvxPaperInfo::GetPaperSize((SvxPaper)nPaper).Width ();
        long lHVal = SvxPaperInfo::GetPaperSize((SvxPaper)nPaper).Height();
        rItem.lWidth  = Max(lWVal, lHVal);
        rItem.lHeight = Min(lWVal, lHVal);
    }
}

// --------------------------------------------------------------------------


BOOL __EXPORT SwEnvFmtPage::FillItemSet(SfxItemSet& rSet)
{
    FillItem(GetParent()->aEnvItem);
    rSet.Put(GetParent()->aEnvItem);
    return TRUE;
}

// --------------------------------------------------------------------------


void __EXPORT SwEnvFmtPage::Reset(const SfxItemSet& rSet)
{
    const SwEnvItem& rItem = (const SwEnvItem&) rSet.Get(FN_ENVELOP);

    SvxPaper ePaper = SvxPaperInfo::GetPaper(
        Size( Min(rItem.lWidth, rItem.lHeight),
        Max(rItem.lWidth, rItem.lHeight)), MAP_TWIP, TRUE);
    for (USHORT i = 0; i < (USHORT) aIDs.Count(); i++)
        if (aIDs[i] == (USHORT)ePaper)
            aSizeFormatBox.SelectEntryPos(i);

    // Die MetricFields
    SetFldVal(aAddrLeftField, rItem.lAddrFromLeft);
    SetFldVal(aAddrTopField , rItem.lAddrFromTop );
    SetFldVal(aSendLeftField, rItem.lSendFromLeft);
    SetFldVal(aSendTopField , rItem.lSendFromTop );
    SetFldVal(aSizeWidthField  , Max(rItem.lWidth, rItem.lHeight));
    SetFldVal(aSizeHeightField , Min(rItem.lWidth, rItem.lHeight));
    SetMinMax();

    DELETEZ(GetParent()->pSenderSet);
    DELETEZ(GetParent()->pAddresseeSet);
}



