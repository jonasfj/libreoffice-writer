/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: optpage.cxx,v $
 * $Revision: 1.61 $
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


#include <hintids.hxx>
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <vcl/svapp.hxx>
#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif
#include <svtools/cjkoptions.hxx>
#include <svtools/ctrltool.hxx>
#include <svtools/eitem.hxx>
#include <svx/htmlmode.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/bindings.hxx>
#include <svx/xtable.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/langitem.hxx>
#include <svx/dlgutil.hxx>
#include <fmtcol.hxx>
#include <charatr.hxx>
#include <swtypes.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef IDOCUMENTDEVICEACCESS_HXX_INCLUDED
#include <IDocumentDeviceAccess.hxx>
#endif
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <uitool.hxx>
#include <cfgitems.hxx> //Items fuer Sw-Seiten
#include <poolfmt.hxx>
#include <uiitems.hxx>
#include <initui.hxx>
#include <optpage.hxx>
#include <swprtopt.hxx>
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#ifndef _SRCVIEW_HXX
#include <srcview.hxx>
#endif
#include <crstate.hxx>
#include <viewopt.hxx>
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _CONFIG_HRC
#include <config.hrc>
#endif
#ifndef _REDLOPT_HRC
#include <redlopt.hrc>
#endif
#ifndef _OPTDLG_HRC
#include <optdlg.hrc>
#endif
#include <svx/strarray.hxx>
#include <svtools/slstitm.hxx>
#include <sfx2/request.hxx>
#include <swwrtshitem.hxx>
#include <svtools/ctloptions.hxx>

#include <unomid.h>


using namespace ::com::sun::star;


/*******************************************************
 ******************************************************/

/*-----------------31.08.96 10.16-------------------
    TabPage Anzeige/Inhalt
--------------------------------------------------*/

SwContentOptPage::SwContentOptPage( Window* pParent,
                                      const SfxItemSet& rCoreSet ) :
    SfxTabPage( pParent, SW_RES( TP_CONTENT_OPT ), rCoreSet ),
    aLineFL       ( this,   SW_RES( FL_LINE     ) ),
    aCrossCB   	  ( this,	SW_RES( CB_CROSS     ) ),
    aSolidHandleCB( this, 	SW_RES( CB_HANDLE	) ),
    aBigHandleCB  ( this, 	SW_RES( CB_BIGHANDLE) ),

    aWindowFL     ( this,   SW_RES( FL_WINDOW   ) ),
    aHScrollBox   ( this,	SW_RES( CB_HSCROLL   ) ),
    aVScrollBox   ( this,	SW_RES( CB_VSCROLL   ) ),
    aAnyRulerCB   ( this,   SW_RES( CB_ANY_RULER ) ),
    aHRulerCBox   ( this,   SW_RES( CB_HRULER   ) ),
    aHMetric      ( this,   SW_RES( LB_HMETRIC    ) ),
    aVRulerCBox   ( this,	SW_RES( CB_VRULER    ) ),
    aVRulerRightCBox( this, SW_RES( CB_VRULER_RIGHT    ) ),
    aVMetric      ( this,   SW_RES( LB_VMETRIC    ) ),
    aSmoothCBox   ( this,   SW_RES( CB_SMOOTH_SCROLL    ) ),

    aDispFL      ( this,   SW_RES( FL_DISP     ) ),
    aGrfCB        ( this,	SW_RES( CB_GRF  		) ),
    aTblCB        ( this,	SW_RES( CB_TBL		) ),
    aDrwCB        ( this,	SW_RES( CB_DRWFAST   ) ),
    aFldNameCB    ( this,	SW_RES( CB_FIELD 	) ),
    aPostItCB     ( this,	SW_RES( CB_POSTIT 	) ),

    aSettingsFL   ( this,   SW_RES( FL_SETTINGS   ) ),
    aMetricFT     ( this,   SW_RES( FT_METRIC   ) ),
    aMetricLB     ( this,   SW_RES( LB_METRIC   ) )
{
    FreeResource();
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aMetricLB.Show();
        aSettingsFL.Show();
        aMetricFT.Show();
    }
    SvtCJKOptions aCJKOptions;
    if(aCJKOptions.IsVerticalTextEnabled() )
    {
        Point aSmoothPos(aSmoothCBox.GetPosPixel());
        aSmoothPos.Y() += aSmoothPos.Y() - aVRulerCBox.GetPosPixel().Y();
        aSmoothCBox.SetPosPixel(aSmoothPos);
    }
    else
        aVRulerRightCBox.Hide();
    aVRulerCBox.SetClickHdl(LINK(this, SwContentOptPage, VertRulerHdl ));
    aAnyRulerCB.SetClickHdl(LINK(this, SwContentOptPage, AnyRulerHdl));

    SvxStringArray aMetricArr( SW_RES( STR_ARR_METRIC ) );
    for ( USHORT i = 0; i < aMetricArr.Count(); ++i )
    {
        String sMetric = aMetricArr.GetStringByPos( i );
        FieldUnit eFUnit = (FieldUnit)aMetricArr.GetValue( i );

        switch ( eFUnit )
        {
            case FUNIT_MM:
            case FUNIT_CM:
            case FUNIT_POINT:
            case FUNIT_PICA:
            case FUNIT_INCH:
            {
                // nur diese Metriken benutzen
                USHORT nPos = aMetricLB.InsertEntry( sMetric );
                aMetricLB.SetEntryData( nPos, (void*)(long)eFUnit );
                aVMetric.InsertEntry( sMetric );
                aVMetric.SetEntryData( nPos, (void*)(long)eFUnit );
                aHMetric.InsertEntry( sMetric );
                aHMetric.SetEntryData( nPos, (void*)(long)eFUnit );
            }
            default:;//prevent warning
        }
    }
}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/
SwContentOptPage::~SwContentOptPage()
{
}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/
SfxTabPage*	SwContentOptPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet)
{
    return new SwContentOptPage(pParent, rAttrSet);
}
/* -----------------------------07.04.01 16:57--------------------------------

 ---------------------------------------------------------------------------*/
static void lcl_SelectMetricLB(ListBox& rMetric, USHORT nSID, const SfxItemSet& rSet)
{
    const SfxPoolItem* pItem;
    if( rSet.GetItemState( nSID, FALSE, &pItem ) >= SFX_ITEM_AVAILABLE )
    {
        FieldUnit eFieldUnit = (FieldUnit)((SfxUInt16Item*)pItem)->GetValue();
        for ( USHORT i = 0; i < rMetric.GetEntryCount(); ++i )
        {
            if ( (int)(sal_IntPtr)rMetric.GetEntryData( i ) == (int)eFieldUnit )
            {
                rMetric.SelectEntryPos( i );
                break;
            }
        }
    }
    rMetric.SaveValue();
}
/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/
void SwContentOptPage::Reset(const SfxItemSet& rSet)
{
    const SwElemItem* pElemAttr = 0;

    rSet.GetItemState( FN_PARAM_ELEM , FALSE,
                                    (const SfxPoolItem**)&pElemAttr );
    if(pElemAttr)
    {
        aTblCB      .Check  (pElemAttr->bTable                );
        aGrfCB      .Check  (pElemAttr->bGraphic              );
        aDrwCB      .Check  (pElemAttr->bDrawing              );
        aFldNameCB  .Check  (pElemAttr->bFieldName            );
        aPostItCB   .Check  (pElemAttr->bNotes                );
        aCrossCB   .Check( pElemAttr->bCrosshair        );
        aSolidHandleCB.Check( !pElemAttr->bHandles          );
        aBigHandleCB.Check(pElemAttr->bBigHandles       );
        aHScrollBox.Check( pElemAttr->bHorzScrollbar     );
        aVScrollBox.Check( pElemAttr->bVertScrollbar     );
        aAnyRulerCB.Check( pElemAttr->bAnyRuler );
        aHRulerCBox.Check( pElemAttr->bHorzRuler         );
        aVRulerCBox.Check( pElemAttr->bVertRuler         );
        aVRulerRightCBox.Check(pElemAttr->bVertRulerRight);
        aSmoothCBox.Check( pElemAttr->bSmoothScroll      );
    }
    aMetricLB.SetNoSelection();
    lcl_SelectMetricLB(aMetricLB, SID_ATTR_METRIC, rSet);
    lcl_SelectMetricLB(aHMetric, FN_HSCROLL_METRIC, rSet);
    lcl_SelectMetricLB(aVMetric, FN_VSCROLL_METRIC, rSet);
    AnyRulerHdl(&aAnyRulerCB);
}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/
BOOL SwContentOptPage::FillItemSet(SfxItemSet& rSet)
{
    const SwElemItem*   pOldAttr = (const SwElemItem*)
                        GetOldItem(GetItemSet(), FN_PARAM_ELEM);

    SwElemItem aElem;
    if(pOldAttr)
        aElem = *pOldAttr;
    aElem.bTable                = aTblCB        .IsChecked();
    aElem.bGraphic              = aGrfCB        .IsChecked();
    aElem.bDrawing              = aDrwCB        .IsChecked();
    aElem.bFieldName            = aFldNameCB    .IsChecked();
    aElem.bNotes                = aPostItCB     .IsChecked();
    aElem.bCrosshair     = aCrossCB   .IsChecked();
    aElem.bHandles       = !aSolidHandleCB.IsChecked();
    aElem.bBigHandles    = aBigHandleCB.IsChecked();
    aElem.bHorzScrollbar = aHScrollBox.IsChecked();
    aElem.bVertScrollbar = aVScrollBox.IsChecked();
    aElem.bAnyRuler = aAnyRulerCB.IsChecked();
    aElem.bHorzRuler     = aHRulerCBox.IsChecked();
    aElem.bVertRuler     = aVRulerCBox.IsChecked();
    aElem.bVertRulerRight= aVRulerRightCBox.IsChecked();
    aElem.bSmoothScroll  = aSmoothCBox.IsChecked();


    BOOL bRet = !pOldAttr || aElem != *pOldAttr;
    if(bRet)
        bRet = 0 != rSet.Put(aElem);
    USHORT nMPos = aMetricLB.GetSelectEntryPos();
    USHORT nGlobalMetricPos = nMPos;
    if ( nMPos != aMetricLB.GetSavedValue() )
    {
        // Doppel-Cast fuer VA3.0
        USHORT nFieldUnit = (USHORT)(long)aMetricLB.GetEntryData( nMPos );
        rSet.Put( SfxUInt16Item( SID_ATTR_METRIC, (UINT16)nFieldUnit ) );
        bRet = TRUE;
    }

    nMPos = aHMetric.GetSelectEntryPos();
    if ( nMPos != aHMetric.GetSavedValue() || nMPos != nGlobalMetricPos )
    {
        // Doppel-Cast fuer VA3.0
        USHORT nFieldUnit = (USHORT)(long)aHMetric.GetEntryData( nMPos );
        rSet.Put( SfxUInt16Item( FN_HSCROLL_METRIC, (UINT16)nFieldUnit ) );
        bRet = TRUE;
    }
    nMPos = aVMetric.GetSelectEntryPos();
    if ( nMPos != aVMetric.GetSavedValue() || nMPos != nGlobalMetricPos )
    {
        // Doppel-Cast fuer VA3.0
        USHORT nFieldUnit = (USHORT)(long)aVMetric.GetEntryData( nMPos );
        rSet.Put( SfxUInt16Item( FN_VSCROLL_METRIC, (UINT16)nFieldUnit ) );
        bRet = TRUE;
    }
    return bRet;
}
/* -----------------------------05.03.2002 15:07------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwContentOptPage, VertRulerHdl, CheckBox*, pBox)
{
    aVRulerRightCBox.Enable(pBox->IsEnabled() && pBox->IsChecked());
    return 0;
}
/* -----------------20.09.2002 11:30-----------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwContentOptPage, AnyRulerHdl, CheckBox*, pBox)
{
    BOOL bChecked = pBox->IsChecked();
    aHRulerCBox      .Enable(bChecked);
    aHMetric         .Enable(bChecked);
    aVRulerCBox      .Enable(bChecked);
    aVMetric         .Enable(bChecked);
    VertRulerHdl(&aVRulerCBox);
    return 0;
}
/*----------------- OS 27.01.95  -----------------------
 TabPage Drucker Zusatzeinstellungen
-------------------------------------------------------*/
SwAddPrinterTabPage::SwAddPrinterTabPage( Window* pParent,
                                      const SfxItemSet& rCoreSet) :
    SfxTabPage( pParent, SW_RES( TP_OPTPRINT_PAGE ), rCoreSet),
    aFL1          (this, SW_RES(FL_1)),
    aGrfCB           (this, SW_RES(CB_PGRF)),
//	aTabCB           (this, SW_RES(CB_PTAB)),
//	aDrawCB          (this, SW_RES(CB_PDRAW)),
    aCtrlFldCB       (this, SW_RES(CB_CTRLFLD)),
    aBackgroundCB    (this, SW_RES(CB_BACKGROUND)),
    aBlackFontCB 	 (this, SW_RES(CB_BLACK_FONT)),
    aPrintHiddenTextCB(this, SW_RES(CB_HIDDEN_TEXT)),
    aPrintTextPlaceholderCB(this, SW_RES(CB_TEXT_PLACEHOLDER)),
    aSeparatorLFL    (this, SW_RES(FL_SEP_PRT_LEFT )),
    aFL2          (this, SW_RES(FL_2)),
    aLeftPageCB      (this, SW_RES(CB_LEFTP)),
    aRightPageCB     (this, SW_RES(CB_RIGHTP)),
//    aReverseCB       (this, SW_RES(CB_REVERSE)),
    aProspectCB      (this, SW_RES(CB_PROSPECT)),
    aProspectCB_RTL      (this, SW_RES(CB_PROSPECT_RTL)),
    aSeparatorRFL    (this, SW_RES(FL_SEP_PRT_RIGHT)),
    aNoRB            (this, SW_RES(RB_NO)),
    aOnlyRB          (this, SW_RES(RB_ONLY)),
    aEndRB           (this, SW_RES(RB_END)),
    aEndPageRB       (this, SW_RES(RB_PAGEEND)),
    aFL3          (this, SW_RES(FL_3)),
    aFL4          (this, SW_RES(FL_4)),
    aPrintEmptyPagesCB(this, SW_RES(CB_PRINTEMPTYPAGES)),
//    aSingleJobsCB    (this, SW_RES(CB_SINGLEJOBS)),
    aPaperFromSetupCB(this, SW_RES(CB_PAPERFROMSETUP)),
    aFaxFT           (this, SW_RES(FT_FAX)),
    aFaxLB           (this, SW_RES(LB_FAX)),
    sNone(SW_RES(ST_NONE)),
    bAttrModified( FALSE ),
    bPreview  ( FALSE )
{
    Init();
    FreeResource();
    Link aLk = LINK( this, SwAddPrinterTabPage, AutoClickHdl);
    aGrfCB.SetClickHdl( aLk );
    aRightPageCB.SetClickHdl( aLk );
    aLeftPageCB.SetClickHdl( aLk );
//	aTabCB.SetClickHdl( aLk );
//	aDrawCB.SetClickHdl( aLk );
    aCtrlFldCB.SetClickHdl( aLk );
    aBackgroundCB.SetClickHdl( aLk );
    aBlackFontCB.SetClickHdl( aLk );
    aPrintHiddenTextCB.SetClickHdl( aLk );
    aPrintTextPlaceholderCB.SetClickHdl( aLk );
//	aReverseCB.SetClickHdl( aLk );
    aProspectCB.SetClickHdl( aLk );
    aProspectCB_RTL.SetClickHdl( aLk );
    aPaperFromSetupCB.SetClickHdl( aLk );
    aPrintEmptyPagesCB.SetClickHdl( aLk );
    aEndPageRB.SetClickHdl( aLk );
    aEndRB.SetClickHdl( aLk );
    aOnlyRB.SetClickHdl( aLk );
    aNoRB.SetClickHdl( aLk );
//	aSingleJobsCB.SetClickHdl( aLk );
    aFaxLB.SetSelectHdl( LINK( this, SwAddPrinterTabPage, SelectHdl ) );

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
//		aDrawCB      .Hide();
        aLeftPageCB  .Hide();
        aRightPageCB .Hide();
        aPrintHiddenTextCB.Hide();
        aPrintTextPlaceholderCB.Hide();
//		aReverseCB.SetPosPixel(aLeftPageCB.GetPosPixel());
        aProspectCB.SetPosPixel(aLeftPageCB.GetPosPixel());
        Point rPt( aRightPageCB.GetPosPixel() );
        rPt.setX(rPt.getX() + 15); // indent
        aProspectCB_RTL.SetPosPixel(rPt);
//		aBlackFontCB.SetPosPixel(aBackgroundCB.GetPosPixel());
//        aPrintHiddenTextCB.SetPosPixel(aBlackFontCB.GetPosPixel());
//		aBackgroundCB.SetPosPixel(aCtrlFldCB.GetPosPixel());
//		aCtrlFldCB.SetPosPixel(aDrawCB.GetPosPixel());
    }
    aProspectCB_RTL.Disable();
    SvtCTLOptions aCTLOptions;
    aProspectCB_RTL.Show(aCTLOptions.IsCTLFontEnabled());
}

//------------------------------------------------------------------------

void SwAddPrinterTabPage::SetPreview(BOOL bPrev)
{
    bPreview = bPrev;

    if (bPreview)
    {
        aLeftPageCB.Disable();
        aRightPageCB.Disable();
        aProspectCB.Disable();
        aProspectCB_RTL.Disable();
        aFL3.Disable();
        aNoRB.Disable();
        aOnlyRB.Disable();
        aEndRB.Disable();
        aEndPageRB.Disable();
    }
}

//------------------------------------------------------------------------

SfxTabPage*	SwAddPrinterTabPage::Create( Window* pParent,
                                       const SfxItemSet& rAttrSet )
{
    return ( new SwAddPrinterTabPage( pParent, rAttrSet ) );
}
//------------------------------------------------------------------------


BOOL 	SwAddPrinterTabPage::FillItemSet( SfxItemSet& rCoreSet )
{
    if ( bAttrModified )
    {
        SwAddPrinterItem aAddPrinterAttr (FN_PARAM_ADDPRINTER);
        aAddPrinterAttr.bPrintGraphic 	= aGrfCB.IsChecked();
        aAddPrinterAttr.bPrintTable 	= TRUE; // always enabled since CWS printerpullgpages /*aTabCB.IsChecked();*/
        aAddPrinterAttr.bPrintDraw      = aGrfCB.IsChecked(); // UI merged with aGrfCB in CWS printerpullgpages /*aDrawCB.IsChecked()*/;
        aAddPrinterAttr.bPrintControl 	= aCtrlFldCB.IsChecked();
        aAddPrinterAttr.bPrintPageBackground = aBackgroundCB.IsChecked();
        aAddPrinterAttr.bPrintBlackFont = aBlackFontCB.IsChecked();
        aAddPrinterAttr.bPrintHiddenText = aPrintHiddenTextCB.IsChecked();
        aAddPrinterAttr.bPrintTextPlaceholder = aPrintTextPlaceholderCB.IsChecked();

        aAddPrinterAttr.bPrintLeftPages     = aLeftPageCB.IsChecked();
        aAddPrinterAttr.bPrintRightPages    = aRightPageCB.IsChecked();
        aAddPrinterAttr.bPrintReverse       = FALSE; // handled by vcl itself since CWS printerpullpages /*aReverseCB.IsChecked()*/;
        aAddPrinterAttr.bPrintProspect      = aProspectCB.IsChecked();
        aAddPrinterAttr.bPrintProspectRTL   = aProspectCB_RTL.IsChecked();
        aAddPrinterAttr.bPaperFromSetup     = aPaperFromSetupCB.IsChecked();
        aAddPrinterAttr.bPrintEmptyPages    = aPrintEmptyPagesCB.IsChecked();
        aAddPrinterAttr.bPrintSingleJobs    = TRUE; // handled by vcl in new print dialog since CWS printerpullpages /*aSingleJobsCB.IsChecked()*/;

        if (aNoRB.IsChecked()) 	aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_NONE;
        if (aOnlyRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ONLY;
        if (aEndRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ENDDOC;
        if (aEndPageRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ENDPAGE;

        String sFax = aFaxLB.GetSelectEntry();
        aAddPrinterAttr.sFaxName = sNone == sFax ? aEmptyStr : sFax;
        rCoreSet.Put(aAddPrinterAttr);
    }
    return bAttrModified;
}
//------------------------------------------------------------------------


void 	SwAddPrinterTabPage::Reset( const SfxItemSet&  )
{
    const 	SfxItemSet&			rSet = GetItemSet();
    const 	SwAddPrinterItem*	pAddPrinterAttr = 0;

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_ADDPRINTER , FALSE,
                                    (const SfxPoolItem**)&pAddPrinterAttr ))
    {
        aGrfCB.Check(			pAddPrinterAttr->bPrintGraphic || pAddPrinterAttr->bPrintDraw );
//		aTabCB.Check(			pAddPrinterAttr->bPrintTable);
//        aDrawCB.Check(          pAddPrinterAttr->bPrintDraw);
        aCtrlFldCB.Check(		pAddPrinterAttr->bPrintControl);
        aBackgroundCB.Check(    pAddPrinterAttr->bPrintPageBackground);
        aBlackFontCB.Check(		pAddPrinterAttr->bPrintBlackFont);
        aPrintHiddenTextCB.Check( pAddPrinterAttr->bPrintHiddenText);
        aPrintTextPlaceholderCB.Check(pAddPrinterAttr->bPrintTextPlaceholder);
        aLeftPageCB.Check(      pAddPrinterAttr->bPrintLeftPages);
        aRightPageCB.Check(     pAddPrinterAttr->bPrintRightPages);
//		aReverseCB.Check(		pAddPrinterAttr->bPrintReverse);
        aPaperFromSetupCB.Check(pAddPrinterAttr->bPaperFromSetup);
        aPrintEmptyPagesCB.Check(pAddPrinterAttr->bPrintEmptyPages);
        aProspectCB.Check(      pAddPrinterAttr->bPrintProspect);
        aProspectCB_RTL.Check(      pAddPrinterAttr->bPrintProspectRTL);
//		aSingleJobsCB.Check(    pAddPrinterAttr->bPrintSingleJobs);

        aNoRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_NONE ) ;
        aOnlyRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ONLY ) ;
        aEndRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ENDDOC ) ;
        aEndPageRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ENDPAGE ) ;
        aFaxLB.SelectEntry( pAddPrinterAttr->sFaxName );
    }
    if (aProspectCB.IsChecked())
        aProspectCB_RTL.Enable(TRUE);
    else
        aProspectCB_RTL.Disable();

}
//-----------------------------------------------------------------------


void 	SwAddPrinterTabPage::Init()
{

}
//------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwAddPrinterTabPage, AutoClickHdl, CheckBox *, EMPTYARG )
{
    bAttrModified = TRUE;
    if (aProspectCB.IsChecked())
        aProspectCB_RTL.Enable(TRUE);
    else
    {
        aProspectCB_RTL.Check( FALSE );
        aProspectCB_RTL.Disable();
    }
    return 0;
}
IMPL_LINK_INLINE_END( SwAddPrinterTabPage, AutoClickHdl, CheckBox *, EMPTYARG )

//------------------------------------------------------------------------


void  SwAddPrinterTabPage::SetFax( const SvStringsDtor& rFaxLst )
{
    aFaxLB.InsertEntry(sNone);
    for ( USHORT i = 0; i < rFaxLst.Count(); ++i )
        aFaxLB.InsertEntry( *rFaxLst.GetObject(i) );
    aFaxLB.SelectEntryPos(0);
}

//------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwAddPrinterTabPage, SelectHdl, ListBox *, EMPTYARG )
{
    bAttrModified=TRUE;
    return 0;
}
IMPL_LINK_INLINE_END( SwAddPrinterTabPage, SelectHdl, ListBox *, EMPTYARG )

void SwAddPrinterTabPage::PageCreated (SfxAllItemSet aSet)
{
    //SFX_ITEMSET_ARG (&aSet,pListItem,SfxStringListItem,SID_FAX_LIST,sal_False);
    SFX_ITEMSET_ARG (&aSet,pListItem,SfxBoolItem,SID_FAX_LIST,sal_False);
    SFX_ITEMSET_ARG (&aSet,pPreviewItem,SfxBoolItem,SID_PREVIEWFLAG_TYPE,sal_False);
    if (pPreviewItem)
    {
        SetPreview(pPreviewItem->GetValue());
        Reset(aSet);
    }
    if (pListItem && pListItem->GetValue())
    {
        SvStringsDtor aFaxList;
        const std::vector<rtl::OUString>& rPrinters = Printer::GetPrinterQueues();
        for (unsigned int i = 0; i < rPrinters.size(); ++i)
        {
            String* pString = new String( rPrinters[i] );
            String* &rpString = pString;
            aFaxList.Insert(rpString, 0);
        }
        SetFax( aFaxList );
/*		SvStringsDtor aFaxList;
        const List *pList = (pListItem)->GetList();
        sal_uInt32 nCount = pList->Count();
        for(sal_uInt32 i = 0; i < nCount ; i++)
        {
            String* pString = (String*)(pList->GetObject(i));
            String* &rpString = pString;
            aFaxList.Insert(rpString, 0 );
        }
        SetFax(aFaxList);
*/
    }
}
/*-----------------03.09.96 11.53-------------------
    Tabpage Standardfonts
--------------------------------------------------*/


SwStdFontTabPage::SwStdFontTabPage( Window* pParent,
                                       const SfxItemSet& rSet ) :
    SfxTabPage( pParent, SW_RES( TP_STD_FONT ), rSet),
    aStdChrFL  (this, SW_RES(FL_STDCHR  )),
    aTypeFT(        this, SW_RES( FT_TYPE          )),

    aStandardLbl(this, SW_RES(FT_STANDARD)),
    aStandardBox(this, SW_RES(LB_STANDARD)),

    aHeightFT(        this, SW_RES( FT_SIZE          )),
    aStandardHeightLB(this, SW_RES( LB_STANDARD_SIZE )),

    aTitleLbl   (this, SW_RES(FT_TITLE   )),
    aTitleBox   (this, SW_RES(LB_TITLE   )),
    aTitleHeightLB(   this, SW_RES( LB_TITLE_SIZE    )),

    aListLbl    (this, SW_RES(FT_LIST    )),
    aListBox    (this, SW_RES(LB_LIST    )),
    aListHeightLB(    this, SW_RES( LB_LIST_SIZE     )),

    aLabelLbl   (this, SW_RES(FT_LABEL   )),
    aLabelBox   (this, SW_RES(LB_LABEL   )),
    aLabelHeightLB(   this, SW_RES( LB_LABEL_SIZE    )),

    aIdxLbl     (this, SW_RES(FT_IDX     )),
    aIdxBox     (this, SW_RES(LB_IDX     )),
    aIndexHeightLB(   this, SW_RES( LB_INDEX_SIZE    )),

    aDocOnlyCB  (this, SW_RES(CB_DOCONLY )),
    aStandardPB (this, SW_RES(PB_STANDARD)),
    pPrt(0),
    pFontList(0),
    pFontConfig(0),
    pWrtShell(0),
    eLanguage( GetAppLanguage() ),

    bListDefault(FALSE),
    bSetListDefault(TRUE),
    bLabelDefault(FALSE),
    bSetLabelDefault(TRUE),
    bIdxDefault(FALSE),
    bSetIdxDefault(TRUE),
    bDeletePrinter(FALSE),

    bListHeightDefault    (FALSE),
    bSetListHeightDefault (FALSE),
    bLabelHeightDefault   (FALSE),
    bSetLabelHeightDefault(FALSE),
    bIndexHeightDefault     (FALSE),
    bSetIndexHeightDefault  (FALSE),

    nFontGroup(FONT_GROUP_DEFAULT),

    sScriptWestern(SW_RES(ST_SCRIPT_WESTERN)),
    sScriptAsian(SW_RES(ST_SCRIPT_ASIAN)),
    sScriptComplex(SW_RES(ST_SCRIPT_CTL))
{
    FreeResource();
    aStandardPB.SetClickHdl(LINK(this, SwStdFontTabPage, StandardHdl));
    aStandardBox.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aListBox    .SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aLabelBox	.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aIdxBox		.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    Link aFocusLink = LINK( this, SwStdFontTabPage, LoseFocusHdl);
    aStandardBox.SetLoseFocusHdl( aFocusLink );
    aTitleBox   .SetLoseFocusHdl( aFocusLink );
    aListBox    .SetLoseFocusHdl( aFocusLink );
    aLabelBox   .SetLoseFocusHdl( aFocusLink );
    aIdxBox     .SetLoseFocusHdl( aFocusLink );

    Link aModifyHeightLink( LINK( this, SwStdFontTabPage, ModifyHeightHdl));
    aStandardHeightLB.SetModifyHdl( aModifyHeightLink );
    aTitleHeightLB.   SetModifyHdl( aModifyHeightLink );
    aListHeightLB.    SetModifyHdl( aModifyHeightLink );
    aLabelHeightLB.   SetModifyHdl( aModifyHeightLink );
    aIndexHeightLB.   SetModifyHdl( aModifyHeightLink );

    aDocOnlyCB.Check(SW_MOD()->GetModuleConfig()->IsDefaultFontInCurrDocOnly());
}

/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/

SwStdFontTabPage::~SwStdFontTabPage()
{
    if(bDeletePrinter)
        delete pPrt;
}

/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/
SfxTabPage*	SwStdFontTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new SwStdFontTabPage(pParent, rAttrSet);
}

/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/
void lcl_SetColl(SwWrtShell* pWrtShell, USHORT nType,
                    SfxPrinter* pPrt, const String& rStyle,
                    USHORT nFontWhich)
{
    BOOL bDelete = FALSE;
    const SfxFont* pFnt = pPrt ? pPrt->GetFontByName(rStyle): 0;
    if(!pFnt)
    {
        pFnt = new SfxFont(FAMILY_DONTKNOW, rStyle);
        bDelete = TRUE;
    }
    SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(nType);
    pColl->SetFmtAttr(SvxFontItem(pFnt->GetFamily(), pFnt->GetName(),
                aEmptyStr, pFnt->GetPitch(), pFnt->GetCharSet(), nFontWhich));
    if(bDelete)
    {
        delete (SfxFont*) pFnt;
    }
}
/*-- 11.10.2005 15:47:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_SetColl(SwWrtShell* pWrtShell, USHORT nType,
                    sal_Int32 nHeight, USHORT nFontHeightWhich)
{
    float fSize = (float)nHeight / 10;
    nHeight = CalcToUnit( fSize, SFX_MAPUNIT_TWIP );
    SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(nType);
    pColl->SetFmtAttr(SvxFontHeightItem(nHeight, 100, nFontHeightWhich));
}
/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/
BOOL SwStdFontTabPage::FillItemSet( SfxItemSet& )
{
    BOOL bNotDocOnly = !aDocOnlyCB.IsChecked();
    SW_MOD()->GetModuleConfig()->SetDefaultFontInCurrDocOnly(!bNotDocOnly);

    String sStandard 	= aStandardBox.GetText();
    String sTitle 		=  aTitleBox   .GetText();
    String sList 		=  aListBox    .GetText();
    String sLabel 		=  aLabelBox   .GetText();
    String sIdx 		=  aIdxBox     .GetText();
    String sStandardBak	= aStandardBox.GetSavedValue();
    String sTitleBak 	= aTitleBox   .GetSavedValue();
    String sListBak 	= aListBox    .GetSavedValue();
    String sLabelBak 	= aLabelBox   .GetSavedValue();
    String sIdxBak  	= aIdxBox     .GetSavedValue();

    bool bStandardHeightChanged = aStandardHeightLB.GetSavedValue() != aStandardHeightLB.GetText();
    bool bTitleHeightChanged = aTitleHeightLB.GetSavedValue() != aTitleHeightLB.GetText();
    bool bListHeightChanged = aListHeightLB.GetSavedValue() != aListHeightLB.GetText() && (!bListHeightDefault || !bSetListHeightDefault );
    bool bLabelHeightChanged = aLabelHeightLB.GetSavedValue() != aLabelHeightLB.GetText() && (!bLabelHeightDefault || !bSetLabelHeightDefault );
    bool bIndexHeightChanged = aIndexHeightLB.GetSavedValue() != aIndexHeightLB.GetText() && (!bIndexHeightDefault || !bSetIndexHeightDefault );
    if(bNotDocOnly)
    {
        pFontConfig->SetFontStandard(sStandard, nFontGroup);
        pFontConfig->SetFontOutline(sTitle, nFontGroup);
        pFontConfig->SetFontList(sList, nFontGroup);
        pFontConfig->SetFontCaption(sLabel, nFontGroup);
        pFontConfig->SetFontIndex(sIdx, nFontGroup);
        if(bStandardHeightChanged)
        {
            float fSize = (float)aStandardHeightLB.GetValue() / 10;
            pFontConfig->SetFontHeight( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), FONT_STANDARD, nFontGroup );
        }
        if(bTitleHeightChanged)
        {
            float fSize = (float)aTitleHeightLB.GetValue() / 10;
            pFontConfig->SetFontHeight( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), FONT_OUTLINE, nFontGroup );
        }
        if(bListHeightChanged)
        {
            float fSize = (float)aListHeightLB.GetValue() / 10;
            pFontConfig->SetFontHeight( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), FONT_LIST, nFontGroup );
        }
        if(bLabelHeightChanged)
        {
            float fSize = (float)aLabelHeightLB.GetValue() / 10;
            pFontConfig->SetFontHeight( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), FONT_CAPTION, nFontGroup );
        }
        if(bIndexHeightChanged)
        {
            float fSize = (float)aIndexHeightLB.GetValue() / 10;
            pFontConfig->SetFontHeight( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), FONT_INDEX, nFontGroup );
        }
    }
    if(pWrtShell)
    {
        pWrtShell->StartAllAction();
        SfxPrinter* pPrinter = pWrtShell->getIDocumentDeviceAccess()->getPrinter( false );
        BOOL bMod = FALSE;
        USHORT nFontWhich = sal::static_int_cast< sal_uInt16, RES_CHRATR >(
            nFontGroup == FONT_GROUP_DEFAULT  ? RES_CHRATR_FONT :
            FONT_GROUP_CJK == nFontGroup ? RES_CHRATR_CJK_FONT : RES_CHRATR_CTL_FONT);
        USHORT nFontHeightWhich = sal::static_int_cast< sal_uInt16, RES_CHRATR >(
            nFontGroup == FONT_GROUP_DEFAULT  ? RES_CHRATR_FONTSIZE :
            FONT_GROUP_CJK == nFontGroup ? RES_CHRATR_CJK_FONTSIZE : RES_CHRATR_CTL_FONTSIZE);
        if(sStandard != sShellStd)
        {
            BOOL bDelete = FALSE;
            const SfxFont* pFnt = pPrinter ? pPrinter->GetFontByName(sStandard): 0;
            if(!pFnt)
            {
                pFnt = new SfxFont(FAMILY_DONTKNOW, sStandard);
                bDelete = TRUE;
            }
            pWrtShell->SetDefault(SvxFontItem(pFnt->GetFamily(), pFnt->GetName(),
                                aEmptyStr, pFnt->GetPitch(), pFnt->GetCharSet(), nFontWhich));
            SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_STANDARD);
            pColl->ResetFmtAttr(nFontWhich);
            if(bDelete)
            {
                delete (SfxFont*) pFnt;
                bDelete = FALSE;
            }
//          lcl_SetColl(pWrtShell, RES_POOLCOLL_STANDARD, pPrinter, sStandard);
            bMod = TRUE;
        }
        if(bStandardHeightChanged)
        {
            float fSize = (float)aStandardHeightLB.GetValue() / 10;
            pWrtShell->SetDefault(SvxFontHeightItem( CalcToUnit( fSize, SFX_MAPUNIT_TWIP ), 100, nFontHeightWhich ) );
            SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_STANDARD);
            pColl->ResetFmtAttr(nFontHeightWhich);
            bMod = TRUE;
        }

        if(sTitle != sShellTitle )
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_HEADLINE_BASE, pPrinter, sTitle, nFontWhich);
            bMod = TRUE;
        }
        if(bTitleHeightChanged)
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_HEADLINE_BASE,
                sal::static_int_cast< sal_uInt16, sal_Int64 >(aTitleHeightLB.GetValue()), nFontHeightWhich);
            bMod = TRUE;
        }
        if(sList != sShellList && (!bListDefault || !bSetListDefault ))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_NUMBUL_BASE, pPrinter, sList, nFontWhich);
            bMod = TRUE;
        }
        if(bListHeightChanged)
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_NUMBUL_BASE,
                sal::static_int_cast< sal_uInt16, sal_Int64 >(aListHeightLB.GetValue()), nFontHeightWhich);
            bMod = TRUE;
        }
        if(sLabel != sShellLabel && (!bLabelDefault || !bSetLabelDefault))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_LABEL, pPrinter, sLabel, nFontWhich);
            bMod = TRUE;
        }
        if(bLabelHeightChanged)
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_LABEL,
                sal::static_int_cast< sal_uInt16, sal_Int64 >(aLabelHeightLB.GetValue()), nFontHeightWhich);
            bMod = TRUE;
        }
        if(sIdx != sShellIndex && (!bIdxDefault || !bSetIdxDefault))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_REGISTER_BASE, pPrinter, sIdx, nFontWhich);
            bMod = TRUE;
        }
        if(bIndexHeightChanged)
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_REGISTER_BASE,
                sal::static_int_cast< sal_uInt16, sal_Int64 >(aIndexHeightLB.GetValue()), nFontHeightWhich);
            bMod = TRUE;
        }
        if ( bMod )
            pWrtShell->SetModified();
        pWrtShell->EndAllAction();
    }

    return FALSE;
}

/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/
void SwStdFontTabPage::Reset( const SfxItemSet& rSet)
{
    const SfxPoolItem* pLang;
    USHORT nLangSlot = nFontGroup == FONT_GROUP_DEFAULT  ? SID_ATTR_LANGUAGE :
        FONT_GROUP_CJK == nFontGroup ? SID_ATTR_CHAR_CJK_LANGUAGE : SID_ATTR_CHAR_CTL_LANGUAGE;


    if( SFX_ITEM_SET == rSet.GetItemState(nLangSlot, FALSE, &pLang))
        eLanguage = ((const SvxLanguageItem*)pLang)->GetValue();

    String sTmp(aStdChrFL.GetText());
    String sToReplace = sScriptWestern;
    if(FONT_GROUP_CJK == nFontGroup )
        sToReplace = sScriptAsian;
    else if(FONT_GROUP_CTL == nFontGroup )
        sToReplace = sScriptComplex;

    sTmp.SearchAndReplaceAscii("%1", sToReplace);
    aStdChrFL.SetText(sTmp);
    const SfxPoolItem* pItem;

    if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_PRINTER, FALSE, &pItem))
    {
        pPrt = (SfxPrinter*)((const SwPtrItem*)pItem)->GetValue();
    }
    else
    {
        SfxItemSet* pPrinterSet = new SfxItemSet( *rSet.GetPool(),
                    SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                    SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                    0 );
        pPrt = new SfxPrinter(pPrinterSet);
        bDeletePrinter = TRUE;
    }
    pFontList = new FontList( pPrt );
    // #i94536# prevent duplication of font entries when 'reset' button is pressed
    if( !aStandardBox.GetEntryCount() )
    {
        const USHORT nCount = pPrt->GetFontCount();
        for (USHORT i = 0; i < nCount; ++i)
        {
            const String &rString = pPrt->GetFont(i)->GetName();
            aStandardBox.InsertEntry( rString );
            aTitleBox   .InsertEntry( rString );
            aListBox    .InsertEntry( rString );
            aLabelBox   .InsertEntry( rString );
            aIdxBox     .InsertEntry( rString );
        }
    }
    if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_STDFONTS, FALSE, &pItem))
    {
         pFontConfig = (SwStdFontConfig*)((const SwPtrItem*)pItem)->GetValue();
    }

    if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_WRTSHELL, FALSE, &pItem))
    {
        pWrtShell = (SwWrtShell*)((const SwPtrItem*)pItem)->GetValue();
    }
    String sStdBackup;
    String sOutBackup;
    String sListBackup;
    String sCapBackup;
    String sIdxBackup;
    sal_Int32 nStandardHeight = -1;
    sal_Int32 nTitleHeight = -1;
    sal_Int32 nListHeight = -1;
    sal_Int32 nLabelHeight = -1;
    sal_Int32 nIndexHeight = -1;

    if(!pWrtShell)
    {
        sStdBackup = pFontConfig->GetFontStandard(nFontGroup);
        sOutBackup = pFontConfig->GetFontOutline(nFontGroup);
        sListBackup= pFontConfig->GetFontList(nFontGroup);
        sCapBackup = pFontConfig->GetFontCaption(nFontGroup);
        sIdxBackup = pFontConfig->GetFontIndex(nFontGroup);
        nStandardHeight = pFontConfig->GetFontHeight( FONT_STANDARD, nFontGroup, eLanguage );
        nTitleHeight =    pFontConfig->GetFontHeight( FONT_OUTLINE , nFontGroup, eLanguage );
        nListHeight =     pFontConfig->GetFontHeight( FONT_LIST    , nFontGroup, eLanguage );
        nLabelHeight =    pFontConfig->GetFontHeight( FONT_CAPTION , nFontGroup, eLanguage );
        nIndexHeight =    pFontConfig->GetFontHeight( FONT_INDEX   , nFontGroup, eLanguage );
        if( nStandardHeight <= 0)
            nStandardHeight = pFontConfig->GetDefaultHeightFor( FONT_STANDARD + nFontGroup * FONT_PER_GROUP, eLanguage);
        if( nTitleHeight <= 0)
            nTitleHeight = pFontConfig->GetDefaultHeightFor( FONT_OUTLINE + nFontGroup * FONT_PER_GROUP, eLanguage);
        if( nListHeight <= 0)
            nListHeight = pFontConfig->GetDefaultHeightFor( FONT_LIST + nFontGroup * FONT_PER_GROUP, eLanguage);
        if( nLabelHeight <= 0)
            nLabelHeight = pFontConfig->GetDefaultHeightFor( FONT_CAPTION + nFontGroup * FONT_PER_GROUP, eLanguage);
        if( nIndexHeight <= 0)
            nIndexHeight = pFontConfig->GetDefaultHeightFor( FONT_INDEX + nFontGroup * FONT_PER_GROUP, eLanguage);

       aDocOnlyCB.Enable(FALSE);
    }
    else
    {
        SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_STANDARD);
        const SvxFontItem& rFont = !nFontGroup ? pColl->GetFont() :
                FONT_GROUP_CJK == nFontGroup ? pColl->GetCJKFont() : pColl->GetCTLFont();
        sShellStd = sStdBackup =  rFont.GetFamilyName();

        USHORT nFontHeightWhich = sal::static_int_cast< sal_uInt16, RES_CHRATR >(
            nFontGroup == FONT_GROUP_DEFAULT  ? RES_CHRATR_FONTSIZE :
            FONT_GROUP_CJK == nFontGroup ? RES_CHRATR_CJK_FONTSIZE : RES_CHRATR_CTL_FONTSIZE );
        const SvxFontHeightItem& rFontHeightStandard = (const SvxFontHeightItem& )pColl->GetFmtAttr(nFontHeightWhich);
        nStandardHeight = (sal_Int32)rFontHeightStandard.GetHeight();

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_HEADLINE_BASE);
        const SvxFontItem& rFontHL = !nFontGroup ? pColl->GetFont() :
                FONT_GROUP_CJK == nFontGroup ? pColl->GetCJKFont() : pColl->GetCTLFont();
        sShellTitle = sOutBackup = rFontHL.GetFamilyName();

        const SvxFontHeightItem& rFontHeightTitle = (const SvxFontHeightItem&)pColl->GetFmtAttr( nFontHeightWhich, sal_True );
        nTitleHeight = (sal_Int32)rFontHeightTitle.GetHeight();

        USHORT nFontWhich = sal::static_int_cast< sal_uInt16, RES_CHRATR >(
            nFontGroup == FONT_GROUP_DEFAULT  ? RES_CHRATR_FONT :
            FONT_GROUP_CJK == nFontGroup ? RES_CHRATR_CJK_FONT : RES_CHRATR_CTL_FONT);
        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_NUMBUL_BASE);
        const SvxFontItem& rFontLS = !nFontGroup ? pColl->GetFont() :
                FONT_GROUP_CJK == nFontGroup ? pColl->GetCJKFont() : pColl->GetCTLFont();
        bListDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);
        sShellList = sListBackup = rFontLS.GetFamilyName();

        const SvxFontHeightItem& rFontHeightList = (const SvxFontHeightItem&)pColl->GetFmtAttr(nFontHeightWhich, sal_True);
        nListHeight = (sal_Int32)rFontHeightList.GetHeight();
        bListHeightDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);


        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_LABEL);
        bLabelDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);
        const SvxFontItem& rFontCP = !nFontGroup ? pColl->GetFont() :
                FONT_GROUP_CJK == nFontGroup ? pColl->GetCJKFont() : pColl->GetCTLFont();
        sShellLabel = sCapBackup = rFontCP.GetFamilyName();
        const SvxFontHeightItem& rFontHeightLabel = (const SvxFontHeightItem&)pColl->GetFmtAttr(nFontHeightWhich, sal_True);
        nLabelHeight = (sal_Int32)rFontHeightLabel.GetHeight();
        bLabelHeightDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_REGISTER_BASE);
        bIdxDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);
        const SvxFontItem& rFontIDX = !nFontGroup ? pColl->GetFont() :
                FONT_GROUP_CJK == nFontGroup ? pColl->GetCJKFont() : pColl->GetCTLFont();
        sShellIndex = sIdxBackup = rFontIDX.GetFamilyName();
        const SvxFontHeightItem& rFontHeightIndex = (const SvxFontHeightItem&)pColl->GetFmtAttr(nFontHeightWhich, sal_True);
        nIndexHeight = (sal_Int32)rFontHeightIndex.GetHeight();
        bIndexHeightDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(nFontWhich, FALSE);
    }
    aStandardBox.SetText(sStdBackup );
    aTitleBox   .SetText(sOutBackup );
    aListBox    .SetText(sListBackup);
    aLabelBox   .SetText(sCapBackup );
    aIdxBox     .SetText(sIdxBackup );

    FontInfo aFontInfo( pFontList->Get(sStdBackup, sStdBackup) );
    aStandardHeightLB.Fill( &aFontInfo, pFontList );
    aFontInfo = pFontList->Get(sOutBackup, sOutBackup );
    aTitleHeightLB.Fill( &aFontInfo, pFontList );
    aFontInfo = pFontList->Get(sListBackup,sListBackup);
    aListHeightLB.Fill( &aFontInfo, pFontList );
    aFontInfo = pFontList->Get(sCapBackup, sCapBackup );
    aLabelHeightLB.Fill( &aFontInfo, pFontList );
    aFontInfo = pFontList->Get(sIdxBackup, sIdxBackup );
    aIndexHeightLB.Fill( &aFontInfo, pFontList );

    aStandardHeightLB.SetValue( CalcToPoint( nStandardHeight, SFX_MAPUNIT_TWIP, 10 ) );
    aTitleHeightLB.   SetValue( CalcToPoint( nTitleHeight   , SFX_MAPUNIT_TWIP, 10 ) );
    aListHeightLB.    SetValue( CalcToPoint( nListHeight    , SFX_MAPUNIT_TWIP, 10 ) );
    aLabelHeightLB.   SetValue( CalcToPoint( nLabelHeight   , SFX_MAPUNIT_TWIP, 10 ));
    aIndexHeightLB.   SetValue( CalcToPoint( nIndexHeight   , SFX_MAPUNIT_TWIP, 10 ));

    aStandardBox.SaveValue();
    aTitleBox   .SaveValue();
    aListBox    .SaveValue();
    aLabelBox   .SaveValue();
    aIdxBox     .SaveValue();

    aStandardHeightLB.SaveValue();
    aTitleHeightLB.   SaveValue();
    aListHeightLB.    SaveValue();
    aLabelHeightLB.   SaveValue();
    aIndexHeightLB.   SaveValue();
}

/*-----------------07.09.96 12.28-------------------

--------------------------------------------------*/


IMPL_LINK( SwStdFontTabPage, StandardHdl, PushButton *, EMPTYARG )
{
    sal_uInt8 nFontOffset = nFontGroup * FONT_PER_GROUP;
    aStandardBox.SetText(SwStdFontConfig::GetDefaultFor(FONT_STANDARD + nFontOffset, eLanguage));
    aTitleBox   .SetText(SwStdFontConfig::GetDefaultFor(FONT_OUTLINE  + nFontOffset, eLanguage));
    aListBox    .SetText(SwStdFontConfig::GetDefaultFor(FONT_LIST     + nFontOffset, eLanguage));
    aLabelBox   .SetText(SwStdFontConfig::GetDefaultFor(FONT_CAPTION  + nFontOffset, eLanguage));
    aIdxBox     .SetText(SwStdFontConfig::GetDefaultFor(FONT_INDEX    + nFontOffset, eLanguage));

    aStandardBox.SaveValue();
    aTitleBox   .SaveValue();
    aListBox    .SaveValue();
    aLabelBox   .SaveValue();
    aIdxBox     .SaveValue();

    aStandardHeightLB.SetValue( CalcToPoint(
        SwStdFontConfig::GetDefaultHeightFor(FONT_STANDARD + nFontOffset, eLanguage),
            SFX_MAPUNIT_TWIP, 10 ) );
    aTitleHeightLB   .SetValue(CalcToPoint(
        SwStdFontConfig::GetDefaultHeightFor(FONT_OUTLINE  +
            nFontOffset, eLanguage), SFX_MAPUNIT_TWIP, 10 ));
    aListHeightLB    .SetValue(CalcToPoint(
        SwStdFontConfig::GetDefaultHeightFor(FONT_LIST + nFontOffset, eLanguage),
            SFX_MAPUNIT_TWIP, 10 ));
    aLabelHeightLB   .SetValue(CalcToPoint(
        SwStdFontConfig::GetDefaultHeightFor(FONT_CAPTION  + nFontOffset, eLanguage),
            SFX_MAPUNIT_TWIP, 10 ));
    aIndexHeightLB   .SetValue(CalcToPoint(
        SwStdFontConfig::GetDefaultHeightFor(FONT_INDEX    + nFontOffset, eLanguage),
            SFX_MAPUNIT_TWIP, 10 ));

    return 0;
}
/*-----------------17.01.97 15.43-------------------

--------------------------------------------------*/

IMPL_LINK( SwStdFontTabPage, ModifyHdl, ComboBox*, pBox )
{
    if(pBox == &aStandardBox)
    {
        String sEntry = pBox->GetText();
        if(bSetListDefault && bListDefault)
            aListBox.SetText(sEntry);
        if(bSetLabelDefault && bLabelDefault)
            aLabelBox.SetText(sEntry);
        if(bSetIdxDefault && bIdxDefault)
            aIdxBox.SetText(sEntry);
    }
    else if(pBox == &aListBox)
    {
        bSetListDefault = FALSE;
    }
    else if(pBox == &aLabelBox)
    {
        bSetLabelDefault = FALSE;
    }
    else if(pBox == &aIdxBox)
    {
        bSetIdxDefault = FALSE;
    }
    return 0;
}

/*-- 11.10.2005 15:11:23---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwStdFontTabPage, ModifyHeightHdl, FontSizeBox*, pBox )
{
    if(pBox == &aStandardHeightLB)
    {
        sal_Int64 nValue = pBox->GetValue(FUNIT_TWIP);
        if(bSetListHeightDefault && bListHeightDefault)
            aListHeightLB.SetValue(nValue, FUNIT_TWIP);
        if(bSetLabelHeightDefault && bLabelHeightDefault)
            aLabelHeightLB.SetValue(nValue, FUNIT_TWIP);
        if(bSetIndexHeightDefault && bIndexHeightDefault)
            aIndexHeightLB.SetValue(nValue, FUNIT_TWIP);
    }
    else if(pBox == &aListHeightLB)
    {
        bSetListHeightDefault = FALSE;
    }
    else if(pBox == &aLabelHeightLB)
    {
        bSetLabelHeightDefault = FALSE;
    }
    else if(pBox == &aIndexHeightLB)
    {
        bSetIndexHeightDefault = FALSE;
    }
    return 0;
}

/*-- 28.09.2005 13:22:36---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwStdFontTabPage, LoseFocusHdl, ComboBox*, pBox )
{
    FontSizeBox* pHeightLB = 0;
    String sEntry = pBox->GetText();
    if(pBox == &aStandardBox)
    {
        pHeightLB = &aStandardHeightLB;
    }
    else if(pBox == &aTitleBox)
    {
        pHeightLB = &aTitleHeightLB;
    }
    else if(pBox == &aListBox)
    {
        pHeightLB = &aListHeightLB;
    }
    else if(pBox == &aLabelBox)
    {
        pHeightLB = &aLabelHeightLB;
    }
    else /*if(pBox == &aIdxBox)*/
    {
        pHeightLB = &aIndexHeightLB;
    }
    FontInfo aFontInfo( pFontList->Get(sEntry, sEntry) );
    pHeightLB->Fill( &aFontInfo, pFontList );

    return 0;
}


void SwStdFontTabPage::PageCreated (SfxAllItemSet aSet)
{
    SFX_ITEMSET_ARG (&aSet,pFlagItem,SfxUInt16Item, SID_FONTMODE_TYPE, sal_False);
    if (pFlagItem)
        SetFontMode(sal::static_int_cast< sal_uInt8, sal_uInt16>( pFlagItem->GetValue()));
}
/*-----------------18.01.97 12.14-------------------
    Optionen Tabelle
--------------------------------------------------*/

SwTableOptionsTabPage::SwTableOptionsTabPage( Window* pParent, const SfxItemSet& rSet ) :
    SfxTabPage(pParent, SW_RES(TP_OPTTABLE_PAGE), rSet),
    aTableFL        (this, SW_RES(FL_TABLE           )),
    aHeaderCB       (this, SW_RES(CB_HEADER          )),
    aRepeatHeaderCB (this, SW_RES(CB_REPEAT_HEADER    )),
    aDontSplitCB    (this, SW_RES(CB_DONT_SPLIT       )),
    aBorderCB       (this, SW_RES(CB_BORDER           )),

    aSeparatorFL     (this, SW_RES(FL_TABLE_SEPARATOR)),

    aTableInsertFL  (this, SW_RES(FL_TABLE_INSERT    )),
    aNumFormattingCB(this, SW_RES(CB_NUMFORMATTING   )),
    aNumFmtFormattingCB(this, SW_RES(CB_NUMFMT_FORMATTING )),
    aNumAlignmentCB (this, SW_RES(CB_NUMALIGNMENT )),

    aMoveFL(        this, SW_RES(FL_MOVE     )),
    aMoveFT(        this, SW_RES(FT_MOVE     )),
    aRowMoveFT(     this, SW_RES(FT_ROWMOVE  )),
    aRowMoveMF(     this, SW_RES(MF_ROWMOVE   )),
    aColMoveFT(     this, SW_RES(FT_COLMOVE   )),
    aColMoveMF(     this, SW_RES(MF_COLMOVE   )),

    aInsertFT(      this, SW_RES(FT_INSERT   )),
    aRowInsertFT(   this, SW_RES(FT_ROWINSERT)),
    aRowInsertMF(   this, SW_RES(MF_ROWINSERT)),
    aColInsertFT(   this, SW_RES(FT_COLINSERT)),
    aColInsertMF(   this, SW_RES(MF_COLINSERT)),

    aHandlingFT(    this, SW_RES(FT_HANDLING )),
    aFixRB(         this, SW_RES(RB_FIX       )),
    aFixPropRB(     this, SW_RES(RB_FIXPROP  )),
    aVarRB(         this, SW_RES(RB_VAR      )),
    aFixFT(         this, SW_RES(FT_FIX      )),
    aFixPropFT(     this, SW_RES(FT_FIXPROP   )),
    aVarFT(         this, SW_RES(FT_VAR       )),
    pWrtShell(0),
    bHTMLMode(FALSE)
{
    FreeResource();

    Link aLnk(LINK(this, SwTableOptionsTabPage, CheckBoxHdl));
    aNumFormattingCB.SetClickHdl(aLnk);
    aNumFmtFormattingCB.SetClickHdl(aLnk);
    aHeaderCB.SetClickHdl(aLnk);
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SwTableOptionsTabPage::~SwTableOptionsTabPage()
{
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SfxTabPage*	SwTableOptionsTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new SwTableOptionsTabPage(pParent, rAttrSet );
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

BOOL SwTableOptionsTabPage::FillItemSet( SfxItemSet& )
{
    BOOL bRet = FALSE;
    SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();

    if(aRowMoveMF.IsModified())
        pModOpt->SetTblHMove( (USHORT)aRowMoveMF.Denormalize( aRowMoveMF.GetValue(FUNIT_TWIP)));

    if(aColMoveMF.IsModified())
        pModOpt->SetTblVMove( (USHORT)aColMoveMF.Denormalize( aColMoveMF.GetValue(FUNIT_TWIP)));

    if(aRowInsertMF.IsModified())
        pModOpt->SetTblHInsert((USHORT)aRowInsertMF.Denormalize( aRowInsertMF.GetValue(FUNIT_TWIP)));

    if(aColInsertMF.IsModified())
        pModOpt->SetTblVInsert((USHORT)aColInsertMF.Denormalize( aColInsertMF.GetValue(FUNIT_TWIP)));

    TblChgMode eMode;
    if(aFixRB.IsChecked())
        eMode = TBLFIX_CHGABS;
    else if(aFixPropRB.IsChecked())
        eMode = TBLFIX_CHGPROP;
    else
        eMode = TBLVAR_CHGABS;
    if(eMode != pModOpt->GetTblMode())
    {
        pModOpt->SetTblMode(eMode);
        // der Tabellen-Tastatur-Modus hat sich geaendert, das soll
        // jetzt auch die aktuelle Tabelle erfahren
        if(pWrtShell && nsSelectionType::SEL_TBL & pWrtShell->GetSelectionType())
        {
            pWrtShell->SetTblChgMode(eMode);
            static USHORT __READONLY_DATA aInva[] =
                                {   FN_TABLE_MODE_FIX,
                                    FN_TABLE_MODE_FIX_PROP,
                                    FN_TABLE_MODE_VARIABLE,
                                    0
                                };
            pWrtShell->GetView().GetViewFrame()->GetBindings().Invalidate( aInva );
        }

        bRet = TRUE;
    }

    SwInsertTableOptions aInsOpts( 0, 0 );

    if (aHeaderCB.IsChecked())
        aInsOpts.mnInsMode |= tabopts::HEADLINE;

    if (aRepeatHeaderCB.IsEnabled() )
        aInsOpts.mnRowsToRepeat = aRepeatHeaderCB.IsChecked()? 1 : 0;

    if (!aDontSplitCB.IsChecked())
        aInsOpts.mnInsMode |= tabopts::SPLIT_LAYOUT;

    if (aBorderCB.IsChecked())
        aInsOpts.mnInsMode |= tabopts::DEFAULT_BORDER;

    if (aHeaderCB.GetSavedValue() != aHeaderCB.GetState() ||
        aRepeatHeaderCB.GetSavedValue() != aRepeatHeaderCB.GetState() ||
        aDontSplitCB.GetSavedValue() != aDontSplitCB.GetState() ||
        aBorderCB.GetSavedValue() != aBorderCB.GetState())
    {
        pModOpt->SetInsTblFlags(bHTMLMode, aInsOpts);
    }

    if (aNumFormattingCB.GetSavedValue() != aNumFormattingCB.GetState())
    {
        pModOpt->SetInsTblFormatNum(bHTMLMode, aNumFormattingCB.IsChecked());
        bRet = TRUE;
    }

    if (aNumFmtFormattingCB.GetSavedValue() != aNumFmtFormattingCB.GetState())
    {
        pModOpt->SetInsTblChangeNumFormat(bHTMLMode, aNumFmtFormattingCB.IsChecked());
        bRet = TRUE;
    }

    if (aNumAlignmentCB.GetSavedValue() != aNumAlignmentCB.GetState())
    {
        pModOpt->SetInsTblAlignNum(bHTMLMode, aNumAlignmentCB.IsChecked());
        bRet = TRUE;
    }

    return bRet;
}
/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/
void SwTableOptionsTabPage::Reset( const SfxItemSet& rSet)
{
    const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
    if ( rSet.GetItemState( SID_ATTR_METRIC ) >= SFX_ITEM_AVAILABLE )
    {
        const SfxUInt16Item& rItem = (SfxUInt16Item&)rSet.Get( SID_ATTR_METRIC );
        FieldUnit eFieldUnit = (FieldUnit)rItem.GetValue();
        ::SetFieldUnit( aRowMoveMF, eFieldUnit );
        ::SetFieldUnit( aColMoveMF, eFieldUnit );
        ::SetFieldUnit( aRowInsertMF, eFieldUnit );
        ::SetFieldUnit( aColInsertMF, eFieldUnit );
    }

    aRowMoveMF  .SetValue(aRowMoveMF.Normalize(pModOpt->GetTblHMove()), FUNIT_TWIP);
    aColMoveMF  .SetValue(aColMoveMF.Normalize(pModOpt->GetTblVMove()), FUNIT_TWIP);
    aRowInsertMF.SetValue(aRowInsertMF.Normalize(pModOpt->GetTblHInsert()), FUNIT_TWIP);
    aColInsertMF.SetValue(aColInsertMF.Normalize(pModOpt->GetTblVInsert()), FUNIT_TWIP);

    switch(pModOpt->GetTblMode())
    {
        case TBLFIX_CHGABS: 	aFixRB.Check();		break;
        case TBLFIX_CHGPROP: 	aFixPropRB.Check();	break;
        case TBLVAR_CHGABS: 	aVarRB.Check();	break;
    }
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState(SID_HTML_MODE, FALSE, &pItem))
    {
        bHTMLMode = 0 != (((const SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON);
    }

    //bestimmte Controls fuer HTML verstecken
    if(bHTMLMode)
    {
/*        Point aPos(aCaptionFL.GetPosPixel());
        long nYDiff = aTableFL.GetPosPixel().Y() - aPos.Y();
        aTableFL.SetPosPixel(aPos);

        aPos = aHeaderCB.GetPosPixel();
        aPos.Y() -= nYDiff;
        aHeaderCB.SetPosPixel(aPos);

        aPos = aRepeatHeaderCB.GetPosPixel();
        aPos.Y() -= nYDiff;
        aRepeatHeaderCB.SetPosPixel(aPos);

        aPos = aDontSplitCB.GetPosPixel(); // hier muss eine Luecke geschlossen werden
        aPos.Y() -= nYDiff;
        aBorderCB.SetPosPixel(aPos);

        aPos = aTableInsertFL.GetPosPixel();
        aPos.Y() -= nYDiff;
        aTableInsertFL.SetPosPixel(aPos);

        aPos = aNumFormattingCB.GetPosPixel();
        aPos.Y() -= nYDiff;
        aNumFormattingCB.SetPosPixel(aPos);

        aPos = aNumFmtFormattingCB.GetPosPixel();
        aPos.Y() -= nYDiff;
        aNumFmtFormattingCB.SetPosPixel(aPos);

        aPos = aNumAlignmentCB.GetPosPixel();
        aPos.Y() -= nYDiff;
        aNumAlignmentCB.SetPosPixel(aPos);

        aCaptionFL.Hide();
        aCaptionCB.Hide();
        aCaptionFT.Hide();
        aCaptionPB.Hide();
*/
        aDontSplitCB.Hide();
    }

    SwInsertTableOptions aInsOpts = pModOpt->GetInsTblFlags(bHTMLMode);
    USHORT nInsTblFlags = aInsOpts.mnInsMode;

    aHeaderCB.Check(0 != (nInsTblFlags & tabopts::HEADLINE));
    aRepeatHeaderCB.Check(aInsOpts.mnRowsToRepeat > 0);
    aDontSplitCB.Check(!(nInsTblFlags & tabopts::SPLIT_LAYOUT));
    aBorderCB.Check(0 != (nInsTblFlags & tabopts::DEFAULT_BORDER));

    aNumFormattingCB.Check(pModOpt->IsInsTblFormatNum(bHTMLMode));
    aNumFmtFormattingCB.Check(pModOpt->IsInsTblChangeNumFormat(bHTMLMode));
    aNumAlignmentCB.Check(pModOpt->IsInsTblAlignNum(bHTMLMode));

    aHeaderCB.SaveValue();
    aRepeatHeaderCB.SaveValue();
    aDontSplitCB.SaveValue();
    aBorderCB.SaveValue();
    aNumFormattingCB.SaveValue();
    aNumFmtFormattingCB.SaveValue();
    aNumAlignmentCB.SaveValue();

    CheckBoxHdl(0);
}
/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/
IMPL_LINK(SwTableOptionsTabPage, CheckBoxHdl, CheckBox*, EMPTYARG)
{
    aNumFmtFormattingCB.Enable(aNumFormattingCB.IsChecked());
    aNumAlignmentCB.Enable(aNumFormattingCB.IsChecked());
    aRepeatHeaderCB.Enable(aHeaderCB.IsChecked());
    return 0;
}
void SwTableOptionsTabPage::PageCreated (SfxAllItemSet aSet)
{
    SFX_ITEMSET_ARG (&aSet,pWrtSh,SwWrtShellItem,SID_WRT_SHELL,sal_False);
    if (pWrtSh)
        SetWrtShell(pWrtSh->GetValue());
}
/*  */

/*-----------------31.10.97 17:55-------------------
 TabPage fuer ShadowCrsr
--------------------------------------------------*/

SwShdwCrsrOptionsTabPage::SwShdwCrsrOptionsTabPage( Window* pParent,
                                                    const SfxItemSet& rSet )
    : SfxTabPage(pParent, SW_RES(TP_OPTSHDWCRSR), rSet),
    aUnprintFL   ( this,   SW_RES( FL_NOPRINT  ) ),
    aParaCB       ( this,   SW_RES( CB_PARA      ) ),
    aSHyphCB      ( this,   SW_RES( CB_SHYPH        ) ),
    aSpacesCB     ( this,   SW_RES( CB_SPACE    ) ),
    aHSpacesCB    ( this,   SW_RES( CB_HSPACE   ) ),
    aTabCB        ( this,   SW_RES( CB_TAB      ) ),
    aBreakCB      ( this,   SW_RES( CB_BREAK        ) ),
    aCharHiddenCB     ( this,   SW_RES( CB_CHAR_HIDDEN   ) ),
    aFldHiddenCB     ( this,   SW_RES( CB_FLD_HIDDEN   ) ),
    aFldHiddenParaCB ( this,   SW_RES( CB_FLD_HIDDEN_PARA ) ),

    aSeparatorFL(   this, SW_RES( FL_SEPARATOR_SHDW)),

    aFlagFL( this, SW_RES( FL_SHDWCRSFLAG )),
    aOnOffCB( this, SW_RES( CB_SHDWCRSONOFF )),

    aFillModeFT( this, SW_RES( FT_SHDWCRSFILLMODE )),
    aFillMarginRB( this, SW_RES( RB_SHDWCRSFILLMARGIN )),
    aFillIndentRB( this, SW_RES( RB_SHDWCRSFILLINDENT )),
    aFillTabRB( this, SW_RES( RB_SHDWCRSFILLTAB )),
    aFillSpaceRB( this, SW_RES( RB_SHDWCRSFILLSPACE )),
    aCrsrOptFL   ( this, SW_RES( FL_CRSR_OPT)),
    aCrsrInProtCB( this, SW_RES( CB_ALLOW_IN_PROT ))
{
    FreeResource();
    const SfxPoolItem* pItem = 0;
    SwShadowCursorItem aOpt;

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_SHADOWCURSOR, FALSE, &pItem ))
        aOpt = *(SwShadowCursorItem*)pItem;

    aOnOffCB.Check( aOpt.IsOn() );

    BYTE eMode = aOpt.GetMode();
    aFillIndentRB.Check( FILL_INDENT == eMode );
    aFillMarginRB.Check( FILL_MARGIN == eMode );
    aFillTabRB.Check( FILL_TAB == eMode );
    aFillSpaceRB.Check( FILL_SPACE == eMode );

    if(SFX_ITEM_SET == rSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aTabCB		.Hide();
        aCharHiddenCB.Hide();
        aFldHiddenCB    .Hide();
        aFldHiddenParaCB.Hide();
        aBreakCB.SetPosPixel(aTabCB.GetPosPixel());
        aFlagFL         .Hide();
        aOnOffCB        .Hide();
        aFillModeFT     .Hide();
        aFillMarginRB   .Hide();
        aFillIndentRB   .Hide();
        aFillTabRB      .Hide();
        aFillSpaceRB    .Hide();
        aCrsrOptFL      .Hide();
        aCrsrInProtCB   .Hide();
        aSeparatorFL.Hide();
        long nWidth = aFlagFL.GetSizePixel().Width() + aFlagFL.GetPosPixel().X()
                                                        - aUnprintFL.GetPosPixel().X();
        Size aSize(aUnprintFL.GetSizePixel());
        aSize.Width() = nWidth;
        aUnprintFL.SetSizePixel(aSize);
    }
}

SwShdwCrsrOptionsTabPage::~SwShdwCrsrOptionsTabPage()
{
}


SfxTabPage*	SwShdwCrsrOptionsTabPage::Create( Window* pParent, const SfxItemSet& rSet )
{
    return new SwShdwCrsrOptionsTabPage( pParent, rSet );
}

BOOL SwShdwCrsrOptionsTabPage::FillItemSet( SfxItemSet& rSet )
{
    SwShadowCursorItem aOpt;
    aOpt.SetOn( aOnOffCB.IsChecked() );

    BYTE eMode;
    if( aFillIndentRB.IsChecked() )
        eMode= FILL_INDENT;
    else if( aFillMarginRB.IsChecked() )
        eMode = FILL_MARGIN;
    else if( aFillTabRB.IsChecked() )
        eMode = FILL_TAB;
    else
        eMode = FILL_SPACE;
    aOpt.SetMode( eMode );

    BOOL bRet = FALSE;
    const SfxPoolItem* pItem = 0;
    if( SFX_ITEM_SET != rSet.GetItemState( FN_PARAM_SHADOWCURSOR, FALSE, &pItem )
        ||  ((SwShadowCursorItem&)*pItem) != aOpt )
    {
        rSet.Put( aOpt );
        bRet = TRUE;
    }

    if( aCrsrInProtCB.IsChecked() != aCrsrInProtCB.GetSavedValue())
    {
        rSet.Put(SfxBoolItem(FN_PARAM_CRSR_IN_PROTECTED, aCrsrInProtCB.IsChecked()));
        bRet |= TRUE;
    }

    const SwDocDisplayItem* pOldAttr = (const SwDocDisplayItem*)
                        GetOldItem(GetItemSet(), FN_PARAM_DOCDISP);

    SwDocDisplayItem aDisp;
    if(pOldAttr)
        aDisp = *pOldAttr;
    //
    aDisp.bParagraphEnd         = aParaCB       .IsChecked();
    aDisp.bTab                  = aTabCB        .IsChecked();
    aDisp.bSpace                = aSpacesCB     .IsChecked();
    aDisp.bNonbreakingSpace     = aHSpacesCB    .IsChecked();
    aDisp.bSoftHyphen           = aSHyphCB      .IsChecked();
    aDisp.bFldHiddenText        = aFldHiddenCB     .IsChecked();
    aDisp.bCharHiddenText       = aCharHiddenCB.IsChecked();
    aDisp.bShowHiddenPara       = aFldHiddenParaCB .IsChecked();
    aDisp.bManualBreak          = aBreakCB      .IsChecked();

    bRet |= (!pOldAttr || aDisp != *pOldAttr);
    if(bRet)
        bRet = 0 != rSet.Put(aDisp);

    return bRet;
}

void SwShdwCrsrOptionsTabPage::Reset( const SfxItemSet& rSet )
{
    const SfxPoolItem* pItem = 0;
    SwShadowCursorItem aOpt;

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_SHADOWCURSOR, FALSE, &pItem ))
        aOpt = *(SwShadowCursorItem*)pItem;

    aOnOffCB.Check( aOpt.IsOn() );

    BYTE eMode = aOpt.GetMode();
    aFillIndentRB.Check( FILL_INDENT == eMode );
    aFillMarginRB.Check( FILL_MARGIN == eMode );
    aFillTabRB.Check( FILL_TAB == eMode );
    aFillSpaceRB.Check( FILL_SPACE == eMode );

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_CRSR_IN_PROTECTED, FALSE, &pItem ))
        aCrsrInProtCB.Check(((const SfxBoolItem*)pItem)->GetValue());
    aCrsrInProtCB.SaveValue();

    const SwDocDisplayItem* pDocDisplayAttr = 0;

    rSet.GetItemState( FN_PARAM_DOCDISP, FALSE,
                                    (const SfxPoolItem**)&pDocDisplayAttr );
    if(pDocDisplayAttr)
    {
        aParaCB     .Check  (pDocDisplayAttr->bParagraphEnd         );
        aTabCB      .Check  (pDocDisplayAttr->bTab                  );
        aSpacesCB   .Check  (pDocDisplayAttr->bSpace                );
        aHSpacesCB  .Check  (pDocDisplayAttr->bNonbreakingSpace     );
        aSHyphCB    .Check  (pDocDisplayAttr->bSoftHyphen           );
        aCharHiddenCB.Check (pDocDisplayAttr->bCharHiddenText );
        aFldHiddenCB   .Check  (pDocDisplayAttr->bFldHiddenText );
        aFldHiddenParaCB.Check (pDocDisplayAttr->bShowHiddenPara       );
        aBreakCB    .Check  (pDocDisplayAttr->bManualBreak          );
    }
}

/*-----------------31.10.97 17:55-------------------
 TabPage fuer Redlining
--------------------------------------------------*/

struct CharAttr
{
    USHORT nItemId;
    USHORT nAttr;
};

// Editieren entspricht Einfuegen-Attributen
static CharAttr __FAR_DATA aRedlineAttr[] =
{
    { SID_ATTR_CHAR_CASEMAP,		SVX_CASEMAP_NOT_MAPPED },
    { SID_ATTR_CHAR_WEIGHT,			WEIGHT_BOLD },
    { SID_ATTR_CHAR_POSTURE,  		ITALIC_NORMAL },
    { SID_ATTR_CHAR_UNDERLINE,    	UNDERLINE_SINGLE },
    { SID_ATTR_CHAR_UNDERLINE,    	UNDERLINE_DOUBLE },
    { SID_ATTR_CHAR_STRIKEOUT,    	STRIKEOUT_SINGLE },
    { SID_ATTR_CHAR_CASEMAP,      	SVX_CASEMAP_VERSALIEN },
    { SID_ATTR_CHAR_CASEMAP,      	SVX_CASEMAP_GEMEINE },
    { SID_ATTR_CHAR_CASEMAP,      	SVX_CASEMAP_KAPITAELCHEN },
    { SID_ATTR_CHAR_CASEMAP,      	SVX_CASEMAP_TITEL },
    { SID_ATTR_BRUSH,				0 }
};
// Items from aRedlineAttr relevant for InsertAttr: strikethrough is
// not used
static USHORT aInsertAttrMap[] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };

// Items from aRedlineAttr relevant for DeleteAttr: underline and
// double underline is not used
static USHORT aDeletedAttrMap[] = { 0, 1, 2, 5, 6, 7, 8, 9, 10 };

// Items from aRedlineAttr relevant for ChangeAttr: strikethrough is
// not used
static USHORT aChangedAttrMap[] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };

/*-----------------------------------------------------------------------
    Beschreibung: Markierungsvorschau
 -----------------------------------------------------------------------*/

SwMarkPreview::SwMarkPreview( Window *pParent, const ResId& rResID ) :

    Window(pParent, rResID),
    m_aTransCol( COL_TRANSPARENT ),
    m_aMarkCol( COL_LIGHTRED ),
    nMarkPos(0)

{
    InitColors();
    SetMapMode(MAP_PIXEL);

    const Size aSz(GetOutputSizePixel());

    // Seite
    aPage.SetSize(Size(aSz.Width() - 3, aSz.Height() - 3));

    ULONG nOutWPix = aPage.GetWidth();
    ULONG nOutHPix = aPage.GetHeight();

    // PrintArea
    ULONG nLBorder = 8;
    ULONG nRBorder = 8;
    ULONG nTBorder = 4;
    ULONG nBBorder = 4;

    aLeftPagePrtArea = Rectangle(Point(nLBorder, nTBorder), Point((nOutWPix - 1) - nRBorder, (nOutHPix - 1) - nBBorder));
    USHORT nWidth = (USHORT)aLeftPagePrtArea.GetWidth();
    USHORT nKorr = (nWidth & 1) != 0 ? 0 : 1;
    aLeftPagePrtArea.SetSize(Size(nWidth / 2 - (nLBorder + nRBorder) / 2 + nKorr, aLeftPagePrtArea.GetHeight()));

    aRightPagePrtArea = aLeftPagePrtArea;
    aRightPagePrtArea.Move(aLeftPagePrtArea.GetWidth() + nLBorder + nRBorder + 1, 0);
}

SwMarkPreview::~SwMarkPreview()
{
}

void SwMarkPreview::InitColors( void )
{
    // m_aTransCol and m_aMarkCol are _not_ changed because they are set from outside!

    const StyleSettings& rSettings = GetSettings().GetStyleSettings();
    m_aBgCol = Color( rSettings.GetWindowColor() );

    BOOL bHC = m_aBgCol.IsDark();
    m_aLineCol = bHC? SwViewOption::GetFontColor() : Color( COL_BLACK );
    m_aShadowCol = bHC? m_aBgCol : rSettings.GetShadowColor();
    m_aTxtCol = bHC? SwViewOption::GetFontColor() : Color( COL_GRAY );
    m_aPrintAreaCol = m_aTxtCol;
}

void SwMarkPreview::DataChanged( const DataChangedEvent& rDCEvt )
{
    Window::DataChanged( rDCEvt );

    if( rDCEvt.GetType() == DATACHANGED_SETTINGS && ( rDCEvt.GetFlags() & SETTINGS_STYLE ) )
        InitColors();
}

void SwMarkPreview::Paint(const Rectangle &/*rRect*/)
{
    // Schatten zeichnen
    Rectangle aShadow(aPage);
    aShadow += Point(3, 3);
    DrawRect( aShadow, m_aShadowCol, m_aTransCol );

    // Seite zeichnen
    DrawRect( aPage, m_aBgCol, m_aLineCol );

    // Separator zeichnen
    Rectangle aPageSeparator(aPage);
    aPageSeparator.SetSize(Size(2, aPageSeparator.GetHeight()));
    aPageSeparator.Move(aPage.GetWidth() / 2 - 1, 0);
    DrawRect( aPageSeparator, m_aLineCol, m_aTransCol );

    PaintPage(aLeftPagePrtArea);
    PaintPage(aRightPagePrtArea);

    Rectangle aLeftMark(Point(aPage.Left() + 2, aLeftPagePrtArea.Top() + 4), Size(aLeftPagePrtArea.Left() - 4, 2));
    Rectangle aRightMark(Point(aRightPagePrtArea.Right() + 2, aRightPagePrtArea.Bottom() - 6), Size(aLeftPagePrtArea.Left() - 4, 2));

    switch (nMarkPos)
    {
        case 1:		// Links
            aRightMark.SetPos(Point(aRightPagePrtArea.Left() - 2 - aRightMark.GetWidth(), aRightMark.Top()));
            break;

        case 2:		// Rechts
            aLeftMark.SetPos(Point(aLeftPagePrtArea.Right() + 2, aLeftMark.Top()));
            break;

        case 3:		// Aussen
            break;

        case 4:		// Innen
            aLeftMark.SetPos(Point(aLeftPagePrtArea.Right() + 2, aLeftMark.Top()));
            aRightMark.SetPos(Point(aRightPagePrtArea.Left() - 2 - aRightMark.GetWidth(), aRightMark.Top()));
            break;

        case 0:		// Keine
        default:
            return;
    }
    DrawRect( aLeftMark, m_aMarkCol, m_aTransCol );
    DrawRect( aRightMark, m_aMarkCol, m_aTransCol );
}

void SwMarkPreview::PaintPage(const Rectangle &rRect)
{
    // PrintArea zeichnen
    DrawRect(rRect, m_aTransCol, m_aPrintAreaCol );

    // Testabsatz zeichnen
    ULONG nLTxtBorder = 4;
    ULONG nRTxtBorder = 4;
    ULONG nTTxtBorder = 4;

    Rectangle aTextLine = rRect;
    aTextLine.SetSize(Size(aTextLine.GetWidth(), 2));
    aTextLine.Left()	+= nLTxtBorder;
    aTextLine.Right()	-= nRTxtBorder;
    aTextLine.Move(0, nTTxtBorder);

    sal_Int32 nStep;
    USHORT nLines;

    nStep = aTextLine.GetHeight() + 2;
    nLines = (USHORT)(rRect.GetHeight() / (aTextLine.GetHeight() + 2)) - 1;

    // Text simulieren
    //
    for (USHORT i = 0; i < nLines; ++i)
    {
        if (i == (nLines - 1))
            aTextLine.SetSize(Size(aTextLine.GetWidth() / 2, aTextLine.GetHeight()));

        if (aPage.IsInside(aTextLine))
            DrawRect(aTextLine, m_aTxtCol, m_aTransCol );

        aTextLine.Move(0, nStep);
    }
    aTextLine.Move(0, -nStep);
}

void SwMarkPreview::DrawRect(const Rectangle &rRect, const Color &rFillColor, const Color &rLineColor)
{
    SetFillColor(rFillColor);
    SetLineColor(rLineColor);
    Window::DrawRect(rRect);
}

namespace
{
    void lcl_FillRedlineAttrListBox(
            ListBox& rLB, const AuthorCharAttr& rAttrToSelect,
            const USHORT* pAttrMap, const USHORT nAttrMapSize)
    {
        for (USHORT i = 0; i != nAttrMapSize; ++i)
        {
            CharAttr& rAttr(aRedlineAttr[pAttrMap[i]]);
            rLB.SetEntryData(i, &rAttr);
            if (rAttr.nItemId == rAttrToSelect.nItemId &&
                rAttr.nAttr == rAttrToSelect.nAttr)
                rLB.SelectEntryPos(i);
        }
    }
}

SwRedlineOptionsTabPage::SwRedlineOptionsTabPage( Window* pParent,
                                                    const SfxItemSet& rSet )
    : SfxTabPage(pParent, SW_RES(TP_REDLINE_OPT), rSet),

    aInsertFL(          this, SW_RES( FL_TE )),

    aInsertFT(          this, SW_RES( FT_CHG_INSERT     )),
    aInsertAttrFT(          this, SW_RES( FT_INS_ATTR     )),
    aInsertLB(          this, SW_RES( LB_INS_ATTR     )),
    aInsertColorFT(     this, SW_RES( FT_INS_COL      )),
    aInsertColorLB(     this, SW_RES( LB_INS_COL      )),
    aInsertedPreviewWN( this, SW_RES( WIN_INS         )),

    aDeletedFT(         this, SW_RES( FT_CHG_DELETE     )),
    aDeletedAttrFT(         this, SW_RES( FT_DEL_ATTR     )),
    aDeletedLB(         this, SW_RES( LB_DEL_ATTR     )),
    aDeletedColorFT(    this, SW_RES( FT_DEL_COL      )),
    aDeletedColorLB(    this, SW_RES( LB_DEL_COL      )),
    aDeletedPreviewWN(  this, SW_RES( WIN_DEL         )),

    aChangedFT(         this, SW_RES( FT_CHG_CHANGE     )),
    aChangedAttrFT(         this, SW_RES( FT_CHG_ATTR     )),
    aChangedLB(         this, SW_RES( LB_CHG_ATTR     )),
    aChangedColorFT(    this, SW_RES( FT_CHG_COL      )),
    aChangedColorLB(    this, SW_RES( LB_CHG_COL      )),
    aChangedPreviewWN(  this, SW_RES( WIN_CHG         )),

    aChangedFL          ( this, SW_RES( FL_LC )),

    aMarkPosFT			( this, SW_RES( FT_MARKPOS )),
    aMarkPosLB			( this, SW_RES( LB_MARKPOS )),
    aMarkColorFT		( this, SW_RES( FT_LC_COL )),
    aMarkColorLB		( this, SW_RES( LB_LC_COL )),
    aMarkPreviewWN		( this, SW_RES( WIN_MARK )),

    sAuthor             ( SW_RES( STR_AUTHOR )),
    sNone				( SW_RES( STR_NOTHING ))

{
    FreeResource();

    for(USHORT i = 0; i < aInsertLB.GetEntryCount(); i++)
    {
        String sEntry(aInsertLB.GetEntry(i));
        aDeletedLB.InsertEntry(sEntry);
        aChangedLB.InsertEntry(sEntry);
    };

    // remove strikethrough from insert and change and underline + double
    // underline from delete
    aInsertLB.RemoveEntry(5);
    aChangedLB.RemoveEntry(5);
    aDeletedLB.RemoveEntry(4);
    aDeletedLB.RemoveEntry(3);
    
    Link aLk = LINK(this, SwRedlineOptionsTabPage, AttribHdl);
    aInsertLB.SetSelectHdl( aLk );
    aDeletedLB.SetSelectHdl( aLk );
    aChangedLB.SetSelectHdl( aLk );

    aLk = LINK(this, SwRedlineOptionsTabPage, ColorHdl);
    aInsertColorLB.SetSelectHdl( aLk );
    aDeletedColorLB.SetSelectHdl( aLk );
    aChangedColorLB.SetSelectHdl( aLk );

    aLk = LINK(this, SwRedlineOptionsTabPage, ChangedMaskPrevHdl);
    aMarkPosLB.SetSelectHdl( aLk );
    aMarkColorLB.SetSelectHdl( aLk );
}

SwRedlineOptionsTabPage::~SwRedlineOptionsTabPage()
{
}

SfxTabPage* SwRedlineOptionsTabPage::Create( Window* pParent, const SfxItemSet& rSet)
{
    return new SwRedlineOptionsTabPage( pParent, rSet );
}

BOOL SwRedlineOptionsTabPage::FillItemSet( SfxItemSet& )
{
    CharAttr *pAttr;
    SwModuleOptions *pOpt = SW_MOD()->GetModuleConfig();

    AuthorCharAttr aInsertedAttr;
    AuthorCharAttr aDeletedAttr;
    AuthorCharAttr aChangedAttr;

    AuthorCharAttr aOldInsertAttr(pOpt->GetInsertAuthorAttr());
    AuthorCharAttr aOldDeletedAttr(pOpt->GetDeletedAuthorAttr());
    AuthorCharAttr aOldChangedAttr(pOpt->GetFormatAuthorAttr());

    ColorData nOldMarkColor = pOpt->GetMarkAlignColor().GetColor();
    USHORT nOldMarkMode = pOpt->GetMarkAlignMode();

    USHORT nPos = aInsertLB.GetSelectEntryPos();
    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        pAttr = (CharAttr *)aInsertLB.GetEntryData(nPos);
        aInsertedAttr.nItemId = pAttr->nItemId;
        aInsertedAttr.nAttr = pAttr->nAttr;

        nPos = aInsertColorLB.GetSelectEntryPos();

        switch (nPos)
        {
            case 0:
                aInsertedAttr.nColor = COL_NONE;
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                aInsertedAttr.nColor = COL_TRANSPARENT;
                break;
            default:
                aInsertedAttr.nColor = aInsertColorLB.GetEntryColor(nPos).GetColor();
                break;
        }

        pOpt->SetInsertAuthorAttr(aInsertedAttr);
    }

    nPos = aDeletedLB.GetSelectEntryPos();
    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        pAttr = (CharAttr *)aDeletedLB.GetEntryData(nPos);
        aDeletedAttr.nItemId = pAttr->nItemId;
        aDeletedAttr.nAttr = pAttr->nAttr;

        nPos = aDeletedColorLB.GetSelectEntryPos();

        switch (nPos)
        {
            case 0:
                aDeletedAttr.nColor = COL_NONE;
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                aDeletedAttr.nColor = COL_TRANSPARENT;
                break;
            default:
                aDeletedAttr.nColor = aDeletedColorLB.GetEntryColor(nPos).GetColor();
                break;
        }

        pOpt->SetDeletedAuthorAttr(aDeletedAttr);
    }

    nPos = aChangedLB.GetSelectEntryPos();
    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        pAttr = (CharAttr *)aChangedLB.GetEntryData(nPos);
        aChangedAttr.nItemId = pAttr->nItemId;
        aChangedAttr.nAttr = pAttr->nAttr;

        nPos = aChangedColorLB.GetSelectEntryPos();

        switch (nPos)
        {
            case 0:
                aChangedAttr.nColor = COL_NONE;
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                aChangedAttr.nColor = COL_TRANSPARENT;
                break;
            default:
                aChangedAttr.nColor = aChangedColorLB.GetEntryColor(nPos).GetColor();
                break;
        }

        pOpt->SetFormatAuthorAttr(aChangedAttr);
    }

    nPos = 0;
    switch (aMarkPosLB.GetSelectEntryPos())
    {
        case 0: nPos = text::HoriOrientation::NONE;       break;
        case 1: nPos = text::HoriOrientation::LEFT;       break;
        case 2: nPos = text::HoriOrientation::RIGHT;      break;
        case 3: nPos = text::HoriOrientation::OUTSIDE;    break;
        case 4: nPos = text::HoriOrientation::INSIDE;     break;
    }
    pOpt->SetMarkAlignMode(nPos);

    pOpt->SetMarkAlignColor(aMarkColorLB.GetSelectEntryColor());

    if (!(aInsertedAttr == aOldInsertAttr) ||
        !(aDeletedAttr == aOldDeletedAttr) ||
        !(aChangedAttr == aOldChangedAttr) ||
       nOldMarkColor != pOpt->GetMarkAlignColor().GetColor() ||
       nOldMarkMode != pOpt->GetMarkAlignMode())
    {
        // Alle Dokumente aktualisieren
        TypeId aType(TYPE(SwDocShell));
        SwDocShell* pDocShell = (SwDocShell*)SfxObjectShell::GetFirst(&aType);

        while( pDocShell )
        {
            pDocShell->GetWrtShell()->UpdateRedlineAttr();
            pDocShell = (SwDocShell*)SfxObjectShell::GetNext(*pDocShell, &aType);
        }
    }

    return FALSE;
}

void SwRedlineOptionsTabPage::Reset( const SfxItemSet&  )
{
    const SwModuleOptions *pOpt = SW_MOD()->GetModuleConfig();

    const AuthorCharAttr &rInsertAttr = pOpt->GetInsertAuthorAttr();
    const AuthorCharAttr &rDeletedAttr = pOpt->GetDeletedAuthorAttr();
    const AuthorCharAttr &rChangedAttr = pOpt->GetFormatAuthorAttr();

    // Preview initialisieren
    InitFontStyle(aInsertedPreviewWN);
    InitFontStyle(aDeletedPreviewWN);
    InitFontStyle(aChangedPreviewWN);

    // Farblistboxen initialisieren
    String sColor;
    aInsertColorLB.SetUpdateMode(FALSE);
    aDeletedColorLB.SetUpdateMode(FALSE);
    aChangedColorLB.SetUpdateMode(FALSE);
    aMarkColorLB.SetUpdateMode(FALSE);

    aInsertColorLB.InsertEntry(sNone);
    aDeletedColorLB.InsertEntry(sNone);
    aChangedColorLB.InsertEntry(sNone);

    aInsertColorLB.InsertEntry(sAuthor);
    aDeletedColorLB.InsertEntry(sAuthor);
    aChangedColorLB.InsertEntry(sAuthor);

    XColorTable* pColorTbl = XColorTable::GetStdColorTable();
    USHORT i;
    for( i = 0; i < pColorTbl->Count(); ++i )
    {
        XColorEntry* pEntry = pColorTbl->GetColor( i );
        Color aColor = pEntry->GetColor();
        String sName = pEntry->GetName();

        aInsertColorLB.InsertEntry( aColor, sName );
        aDeletedColorLB.InsertEntry( aColor, sName );
        aChangedColorLB.InsertEntry( aColor, sName );
        aMarkColorLB.InsertEntry( aColor, sName );
    }
    aInsertColorLB.SetUpdateMode( TRUE );
    aDeletedColorLB.SetUpdateMode( TRUE );
    aChangedColorLB.SetUpdateMode( TRUE );
    aMarkColorLB.SetUpdateMode( TRUE );

    ColorData nColor = rInsertAttr.nColor;

    switch (nColor)
    {
        case COL_TRANSPARENT:
            aInsertColorLB.SelectEntryPos(1);
            break;
        case COL_NONE:
            aInsertColorLB.SelectEntryPos(0);
            break;
        default:
            aInsertColorLB.SelectEntry(Color(nColor));
    }

    nColor = rDeletedAttr.nColor;

    switch (nColor)
    {
        case COL_TRANSPARENT:
            aDeletedColorLB.SelectEntryPos(1);
            break;
        case COL_NONE:
            aDeletedColorLB.SelectEntryPos(0);
            break;
        default:
            aDeletedColorLB.SelectEntry(Color(nColor));
    }

    nColor = rChangedAttr.nColor;

    switch (nColor)
    {
        case COL_TRANSPARENT:
            aChangedColorLB.SelectEntryPos(1);
            break;
        case COL_NONE:
            aChangedColorLB.SelectEntryPos(0);
            break;
        default:
            aChangedColorLB.SelectEntry(Color(nColor));
    }

    aMarkColorLB.SelectEntry(pOpt->GetMarkAlignColor());

    aInsertLB.SelectEntryPos(0);
    aDeletedLB.SelectEntryPos(0);
    aChangedLB.SelectEntryPos(0);
    
    lcl_FillRedlineAttrListBox(aInsertLB, rInsertAttr, aInsertAttrMap,
            sizeof(aInsertAttrMap) / sizeof(USHORT));
    lcl_FillRedlineAttrListBox(aDeletedLB, rDeletedAttr, aDeletedAttrMap,
            sizeof(aDeletedAttrMap) / sizeof(USHORT));
    lcl_FillRedlineAttrListBox(aChangedLB, rChangedAttr, aChangedAttrMap,
            sizeof(aChangedAttrMap) / sizeof(USHORT));
    
    USHORT nPos = 0;
    switch (pOpt->GetMarkAlignMode())
    {
        case text::HoriOrientation::NONE:     nPos = 0;   break;
        case text::HoriOrientation::LEFT:     nPos = 1;   break;
        case text::HoriOrientation::RIGHT:    nPos = 2;   break;
        case text::HoriOrientation::OUTSIDE:  nPos = 3;   break;
        case text::HoriOrientation::INSIDE:   nPos = 4;   break;
    }
    aMarkPosLB.SelectEntryPos(nPos);

    // Einstellungen in Preview anzeigen
    AttribHdl(&aInsertLB);
    ColorHdl(&aInsertColorLB);
    AttribHdl(&aDeletedLB);
    ColorHdl(&aInsertColorLB);
    AttribHdl(&aChangedLB);
    ColorHdl(&aChangedColorLB);

    ChangedMaskPrevHdl();
}

IMPL_LINK( SwRedlineOptionsTabPage, AttribHdl, ListBox *, pLB )
{
    SvxFontPrevWindow *pPrev = 0;
    ColorListBox *pColorLB;

    if (pLB == &aInsertLB)
    {
        pColorLB = &aInsertColorLB;
        pPrev = &aInsertedPreviewWN;
    }
    else if (pLB == &aDeletedLB)
    {
        pColorLB = &aDeletedColorLB;
        pPrev = &aDeletedPreviewWN;
    }
    else
    {
        pColorLB = &aChangedColorLB;
        pPrev = &aChangedPreviewWN;
    }

    SvxFont&	rFont = pPrev->GetFont();
    SvxFont&	rCJKFont = pPrev->GetCJKFont();

    rFont.SetWeight(WEIGHT_NORMAL);
    rCJKFont.SetWeight(WEIGHT_NORMAL);
    rFont.SetItalic(ITALIC_NONE);
    rCJKFont.SetItalic(ITALIC_NONE);
    rFont.SetUnderline(UNDERLINE_NONE);
    rCJKFont.SetUnderline(UNDERLINE_NONE);
    rFont.SetStrikeout(STRIKEOUT_NONE);
    rCJKFont.SetStrikeout(STRIKEOUT_NONE);
    rFont.SetCaseMap(SVX_CASEMAP_NOT_MAPPED);
    rCJKFont.SetCaseMap(SVX_CASEMAP_NOT_MAPPED);

    USHORT		nPos = pColorLB->GetSelectEntryPos();

    switch( nPos )
    {
        case 0:
            rFont.SetColor( Color( COL_BLACK ) );
            rCJKFont.SetColor( Color( COL_BLACK ) );
            break;
        case 1:
        case LISTBOX_ENTRY_NOTFOUND:
            rFont.SetColor( Color( COL_RED ) );
            rCJKFont.SetColor( Color( COL_RED ) );
            break;
        default:
            rFont.SetColor( pColorLB->GetEntryColor( nPos ) );
            rCJKFont.SetColor( pColorLB->GetEntryColor( nPos ) );
            break;
    }

    nPos = pLB->GetSelectEntryPos();
    if( nPos == LISTBOX_ENTRY_NOTFOUND )
        nPos = 0;

    CharAttr*	pAttr = ( CharAttr* ) pLB->GetEntryData( nPos );
    //switch off preview background color
    pPrev->ResetColor();
    switch (pAttr->nItemId)
    {
        case SID_ATTR_CHAR_WEIGHT:
            rFont.SetWeight( ( FontWeight ) pAttr->nAttr );
            rCJKFont.SetWeight( ( FontWeight ) pAttr->nAttr );
            break;

        case SID_ATTR_CHAR_POSTURE:
            rFont.SetItalic( ( FontItalic ) pAttr->nAttr );
            rCJKFont.SetItalic( ( FontItalic ) pAttr->nAttr );
            break;

        case SID_ATTR_CHAR_UNDERLINE:
            rFont.SetUnderline( ( FontUnderline ) pAttr->nAttr );
            rCJKFont.SetUnderline( ( FontUnderline ) pAttr->nAttr );
            break;

        case SID_ATTR_CHAR_STRIKEOUT:
            rFont.SetStrikeout( ( FontStrikeout ) pAttr->nAttr );
            rCJKFont.SetStrikeout( ( FontStrikeout ) pAttr->nAttr );
            break;

        case SID_ATTR_CHAR_CASEMAP:
            rFont.SetCaseMap( ( SvxCaseMap ) pAttr->nAttr );
            rCJKFont.SetCaseMap( ( SvxCaseMap ) pAttr->nAttr );
            break;

        case SID_ATTR_BRUSH:
        {
            nPos = pColorLB->GetSelectEntryPos();
            if( nPos )
                pPrev->SetColor( pColorLB->GetSelectEntryColor() );
            else
                pPrev->SetColor( Color( COL_LIGHTGRAY ) );

            rFont.SetColor( Color( COL_BLACK ) );
            rCJKFont.SetColor( Color( COL_BLACK ) );
        }
        break;
    }

    pPrev->Invalidate();

    return 0;
}

IMPL_LINK( SwRedlineOptionsTabPage, ColorHdl, ColorListBox *, pColorLB )
{
    SvxFontPrevWindow *pPrev = 0;
    ListBox* pLB;

    if (pColorLB == &aInsertColorLB)
    {
        pLB = &aInsertLB;
        pPrev = &aInsertedPreviewWN;
    }
    else if (pColorLB == &aDeletedColorLB)
    {
        pLB = &aDeletedLB;
        pPrev = &aDeletedPreviewWN;
    }
    else
    {
        pLB = &aChangedLB;
        pPrev = &aChangedPreviewWN;
    }

    SvxFont&	rFont = pPrev->GetFont();
    SvxFont&	rCJKFont = pPrev->GetCJKFont();
    USHORT		nPos = pLB->GetSelectEntryPos();
    if( nPos == LISTBOX_ENTRY_NOTFOUND )
        nPos = 0;

    CharAttr*	pAttr = ( CharAttr* ) pLB->GetEntryData( nPos );

    if( pAttr->nItemId == SID_ATTR_BRUSH )
    {
        rFont.SetColor( Color( COL_BLACK ) );
        rCJKFont.SetColor( Color( COL_BLACK ) );
        nPos = pColorLB->GetSelectEntryPos();
        if( nPos && nPos != LISTBOX_ENTRY_NOTFOUND )
            pPrev->SetColor( pColorLB->GetSelectEntryColor() );
        else
            pPrev->SetColor( Color( COL_LIGHTGRAY ) );
    }
    else
    {
        nPos = pColorLB->GetSelectEntryPos();

        switch( nPos )
        {
            case 0:
                rFont.SetColor( Color( COL_BLACK ) );
                rCJKFont.SetColor( Color( COL_BLACK ) );
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                rFont.SetColor( Color( COL_RED ) );
                rCJKFont.SetColor( Color( COL_RED ) );
                break;
            default:
                rFont.SetColor( pColorLB->GetEntryColor( nPos ) );
                rCJKFont.SetColor( pColorLB->GetEntryColor( nPos ) );
                break;
        }
    }

    pPrev->Invalidate();

    return 0;
}

IMPL_LINK( SwRedlineOptionsTabPage, ChangedMaskPrevHdl, ListBox *, EMPTYARG )
{
    aMarkPreviewWN.SetMarkPos(aMarkPosLB.GetSelectEntryPos());
    aMarkPreviewWN.SetColor(aMarkColorLB.GetSelectEntryColor().GetColor());

    aMarkPreviewWN.Invalidate();

    return 0;
}

void SwRedlineOptionsTabPage::InitFontStyle(SvxFontPrevWindow& rExampleWin)
{
    const AllSettings&	rAllSettings = Application::GetSettings();
    LanguageType		eLangType = rAllSettings.GetUILanguage();
    Color				aBackCol( rAllSettings.GetStyleSettings().GetWindowColor() );
    SvxFont&			rFont = rExampleWin.GetFont();
    SvxFont&			rCJKFont = rExampleWin.GetCJKFont();
    SvxFont&			rCTLFont = rExampleWin.GetCTLFont();

    Font				aFont( OutputDevice::GetDefaultFont( DEFAULTFONT_SERIF, eLangType,
                                                        DEFAULTFONT_FLAGS_ONLYONE, &rExampleWin ) );
    Font				aCJKFont( OutputDevice::GetDefaultFont( DEFAULTFONT_CJK_TEXT, eLangType,
                                                        DEFAULTFONT_FLAGS_ONLYONE, &rExampleWin ) );
    Font				aCTLFont( OutputDevice::GetDefaultFont( DEFAULTFONT_CTL_TEXT, eLangType,
                                                        DEFAULTFONT_FLAGS_ONLYONE, &rExampleWin ) );
    const Size			aDefSize( 0, 12 );
    aFont.SetSize( aDefSize );
    aCJKFont.SetSize( aDefSize );
    aCTLFont.SetSize( aDefSize );

    aFont.SetFillColor( aBackCol );
    aCJKFont.SetFillColor( aBackCol );
    aCTLFont.SetFillColor( aBackCol );

    aFont.SetWeight( WEIGHT_NORMAL );
    aCJKFont.SetWeight( WEIGHT_NORMAL );
    aCTLFont.SetWeight( WEIGHT_NORMAL );

    rFont = aFont;
    rCJKFont = aCJKFont;
    rCTLFont = aCTLFont;

    const Size			aNewSize( 0, rExampleWin.GetOutputSize().Height() * 2 / 3 );
    rFont.SetSize( aNewSize );
    rCJKFont.SetSize( aNewSize );

    rExampleWin.SetFont( rFont, rCJKFont,rCTLFont );

    rExampleWin.UseResourceText();

    Wallpaper			aWall( aBackCol );
    rExampleWin.SetBackground( aWall );
    rExampleWin.Invalidate();
}


#ifndef PRODUCT
/*******************************************************
 ******************************************************/
/*----------------- OS 11.01.95  -----------------------
 TabPage Testeinstellungen
-------------------------------------------------------*/

void lcl_SetPosSize(Window& rWin, Point aPos, Size aSize)
{
    aPos = rWin.LogicToPixel(aPos, MAP_APPFONT);
    aSize = rWin.OutputDevice::LogicToPixel(aSize, MAP_APPFONT);
    rWin.SetPosSizePixel(aPos, aSize);
}

SwTestTabPage::SwTestTabPage( Window* pParent,
                                      const SfxItemSet& rCoreSet) :
    SfxTabPage( pParent, WB_HIDE, rCoreSet),
    aTestFL        ( this, WB_GROUP ),
    aTest1CBox   	( this,	0 ),
    aTest2CBox    	( this,	0 ),
    aTest3CBox    	( this,	0 ),
    aTest4CBox    	( this,	0 ),
    aTest5CBox   	( this, 0 ),
    aTest6CBox   	( this,	0 ),
    aTest7CBox   	( this,	0 ),
    aTest8CBox		( this,	0 ),
    aTest9CBox		( this,	0 ),
    aTest10CBox		( this,	0 ),
    bAttrModified( FALSE )
{
    lcl_SetPosSize(*this, 		Point(0,0), Size(260 , 135));
    lcl_SetPosSize(aTestFL,   Point(6,2), Size(209,8));
    lcl_SetPosSize(aTest1CBox  , 	Point(12 , 14),	Size(74 , 10));
    lcl_SetPosSize(aTest2CBox  , 	Point(12 , 27),	Size(74 , 10));
    lcl_SetPosSize(aTest3CBox  , 	Point(12 , 40),	Size(74 , 10));
    lcl_SetPosSize(aTest4CBox  , 	Point(12 , 53),	Size(74 , 10));
    lcl_SetPosSize(aTest5CBox  , 	Point(12 , 66),	Size(74 , 10));
    lcl_SetPosSize(aTest6CBox  , 	Point(116, 14),	Size(74 , 10));
    lcl_SetPosSize(aTest7CBox  , 	Point(116, 27),	Size(74 , 10));
    lcl_SetPosSize(aTest8CBox  , 	Point(116, 40),	Size(74 , 10));
    lcl_SetPosSize(aTest9CBox  , 	Point(116, 53),	Size(74 , 10));
    lcl_SetPosSize(aTest10CBox	, 	Point(116, 66),	Size(74 , 10));

    aTestFL.SetText( String(ByteString("Einstellungen nur f\xFCr Testzwecke" ), RTL_TEXTENCODING_MS_1252));
    aTest1CBox .SetText( C2S("unused"));
    aTest2CBox .SetText( C2S("dynamic"));
    aTest3CBox .SetText( C2S("No calm"               ));
    aTest4CBox .SetText( C2S("WYSIWYG debug"         ));
    aTest5CBox .SetText( C2S("No idle format"        ));
    aTest6CBox .SetText( C2S("No screen adj"         ));
    aTest7CBox .SetText( C2S("win format"            ));
    aTest8CBox .SetText( C2S("No Scroll"             ));
    aTest9CBox .SetText( C2S("DrawingLayerNotLoading"));
    aTest10CBox.SetText( C2S("AutoFormat by Input"   ));
    aTestFL.Show();
    aTest1CBox .Show();
    aTest2CBox .Show();
    aTest3CBox .Show();
    aTest4CBox .Show();
    aTest5CBox .Show();
    aTest6CBox .Show();
    aTest7CBox .Show();
    aTest8CBox .Show();
    aTest9CBox .Show();
    aTest10CBox.Show();
    Init();

}


//------------------------------------------------------------------------


SfxTabPage*	SwTestTabPage::Create( Window* pParent,
                                       const SfxItemSet& rAttrSet )
{
    return ( new SwTestTabPage( pParent, rAttrSet ) );
}
//------------------------------------------------------------------------


BOOL 	SwTestTabPage::FillItemSet( SfxItemSet& rCoreSet )
{

    if ( bAttrModified )
    {
        SwTestItem aTestItem(FN_PARAM_SWTEST);
            aTestItem.bTest1=aTest1CBox.IsChecked();
            aTestItem.bTest2=aTest2CBox.IsChecked();
            aTestItem.bTest3=aTest3CBox.IsChecked();
            aTestItem.bTest4=aTest4CBox.IsChecked();
            aTestItem.bTest5=aTest5CBox.IsChecked();
            aTestItem.bTest6=aTest6CBox.IsChecked();
            aTestItem.bTest7=aTest7CBox.IsChecked();
            aTestItem.bTest8=aTest8CBox.IsChecked();
            aTestItem.bTest9=aTest9CBox.IsChecked();
            aTestItem.bTest10=aTest10CBox.IsChecked();
        rCoreSet.Put(aTestItem);
    }
    return bAttrModified;
}
//------------------------------------------------------------------------


void SwTestTabPage::Reset( const SfxItemSet& )
{
    const SfxItemSet& rSet = GetItemSet();
    const SwTestItem* pTestAttr = 0;

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_SWTEST , FALSE,
                                    (const SfxPoolItem**)&pTestAttr ))
    {
        aTest1CBox.Check(pTestAttr->bTest1);
        aTest2CBox.Check(pTestAttr->bTest2);
        aTest3CBox.Check(pTestAttr->bTest3);
        aTest4CBox.Check(pTestAttr->bTest4);
        aTest5CBox.Check(pTestAttr->bTest5);
        aTest6CBox.Check(pTestAttr->bTest6);
        aTest7CBox.Check(pTestAttr->bTest7);
        aTest8CBox.Check(pTestAttr->bTest8);
        aTest9CBox.Check(pTestAttr->bTest9);
        aTest10CBox.Check(pTestAttr->bTest10);
    }
}
//------------------------------------------------------------------------


void SwTestTabPage::Init()
{
    // handler
    Link aLk = LINK( this, SwTestTabPage, AutoClickHdl );
    aTest1CBox.SetClickHdl( aLk );
    aTest2CBox.SetClickHdl( aLk );
    aTest3CBox.SetClickHdl( aLk );
    aTest4CBox.SetClickHdl( aLk );
    aTest5CBox.SetClickHdl( aLk );
    aTest6CBox.SetClickHdl( aLk );
    aTest7CBox.SetClickHdl( aLk );
    aTest8CBox.SetClickHdl( aLk );
    aTest9CBox.SetClickHdl( aLk );
    aTest10CBox.SetClickHdl( aLk );
}
//------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwTestTabPage, AutoClickHdl, CheckBox *, EMPTYARG )
{
    bAttrModified = TRUE;
    return 0;
}
IMPL_LINK_INLINE_END( SwTestTabPage, AutoClickHdl, CheckBox *, EMPTYARG )
#endif



