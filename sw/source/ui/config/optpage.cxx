/*************************************************************************
 *
 *  $RCSfile: optpage.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:33 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _XTABLE_HXX //autogen
#include <svx/xtable.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_DLGUTIL_HXX //autogen
#include <svx/dlgutil.hxx>
#endif
#ifndef _SV_SYSTEM_HXX //autogen
#include <vcl/system.hxx>
#endif
#ifndef _SV_WALL_HXX
#include <vcl/wall.hxx>
#endif


#ifndef _FMTCOL_HXX //autogen
#include <fmtcol.hxx>
#endif
#ifndef _CHARATR_HXX
#include <charatr.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _SWATRSET_HXX
#include <swatrset.hxx>
#endif
#ifndef _OPTDLG_HXX
#include <optdlg.hxx>
#endif
#ifndef _CFGITEMS_HXX
#include <cfgitems.hxx> //Items fuer Sw-Seiten
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _UIITEMS_HXX
#include <uiitems.hxx>
#endif
#ifndef _INITUI_HXX
#include <initui.hxx>
#endif
#ifndef _OPTPAGE_HXX
#include <optpage.hxx>
#endif
#ifndef _SWPRTOPT_HXX
#include <swprtopt.hxx>
#endif
#ifndef _FONTCFG_HXX
#include <fontcfg.hxx>
#endif
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#ifndef _SRCVCFG_HXX
#include <srcvcfg.hxx>
#endif
#ifndef _SRCVIEW_HXX
#include <srcview.hxx>
#endif
#ifndef _SRCEDTW_HXX
#include <srcedtw.hxx>
#endif
#ifndef _CRSTATE_HXX
#include <crstate.hxx>
#endif

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

#define C2S(cChar) String::CreateFromAscii(cChar)
/*******************************************************
 ******************************************************/

/*-----------------31.08.96 10.16-------------------
    TabPage Anzeige/Inhalt
--------------------------------------------------*/

SwContentOptPage::SwContentOptPage( Window* pParent,
                                      const SfxItemSet& rCoreSet ) :
    SfxTabPage( pParent, SW_RES( TP_CONTENT_OPT ), rCoreSet ),
    aDispBox      ( this,	SW_RES( GB_DISP  	) ),
    aGrfCB        ( this,	SW_RES( CB_GRF  		) ),
    aTblCB        ( this,	SW_RES( CB_TBL		) ),
    aDrwCB        ( this,	SW_RES( CB_DRWFAST   ) ),
    aFldNameCB    ( this,	SW_RES( CB_FIELD 	) ),
    aPostItCB     ( this,	SW_RES( CB_POSTIT 	) ),

    aUnprintBox   ( this, 	SW_RES( GB_NOPRINT 	) ),
    aParaCB       ( this,	SW_RES( CB_PARA      ) ),
    aSHyphCB      ( this,	SW_RES( CB_SHYPH    	) ),
    aSpacesCB     ( this,	SW_RES( CB_SPACE 	) ),
    aHSpacesCB    ( this,	SW_RES( CB_HSPACE	) ),
    aTabCB        ( this,	SW_RES( CB_TAB    	) ),
    aBreakCB      ( this,	SW_RES( CB_BREAK    	) ),
    aHiddenCB     ( this,	SW_RES( CB_HIDDEN	) ),
    aHiddenParaCB ( this,   SW_RES( CB_HIDDEN_PARA ) ),

    aBackBox      ( this,	SW_RES( GB_BACK   	) ),
    aIdxEntryCB   ( this,	SW_RES( CB_IDX_ENTRY     ) ),
    aIdxBackCB   ( this,	SW_RES( CB_INDEX     ) ),
    aFootBackCB   ( this,	SW_RES( CB_FOOTBACK  ) ),
    aFldBackCB    ( this,	SW_RES( CB_FLDBACK  	) )
{
    FreeResource();
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aFootBackCB	.Hide();
        aIdxEntryCB	.Hide();
        aIdxBackCB.Hide();
        aTabCB		.Hide();
        aHiddenCB	.Hide();
        aHiddenParaCB.Hide();
        aFldBackCB.SetPosPixel(aIdxEntryCB.GetPosPixel());
        aBreakCB.SetPosPixel(aTabCB.GetPosPixel());
    }
}

#undef _INIT

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

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/


void SwContentOptPage::Reset(const SfxItemSet& rSet)
{
    const SwDocDisplayItem*	pDocDisplayAttr = 0;

    rSet.GetItemState( FN_PARAM_DOCDISP, FALSE,
                                    (const SfxPoolItem**)&pDocDisplayAttr );
    if(pDocDisplayAttr)
    {
        aParaCB		.Check	(pDocDisplayAttr->bParagraphEnd	  		);
        aTabCB		.Check	(pDocDisplayAttr->bTab 				    );
        aSpacesCB	.Check	(pDocDisplayAttr->bSpace				);
        aHSpacesCB	.Check	(pDocDisplayAttr->bNonbreakingSpace	    );
        aSHyphCB	.Check	(pDocDisplayAttr->bSoftHyphen		    );
        aHiddenCB	.Check	(pDocDisplayAttr->bHiddenText		    );
        aHiddenParaCB.Check	(pDocDisplayAttr->bShowHiddenPara		);
        aBreakCB	.Check	(pDocDisplayAttr->bManualBreak 		    );
        aIdxEntryCB	.Check	(pDocDisplayAttr->bIndexEntry		    );
        aIdxBackCB.Check	(pDocDisplayAttr->bIndexBackground		);
        aFootBackCB	.Check	(pDocDisplayAttr->bFootnoteBackground   );
        aFldBackCB	.Check	(pDocDisplayAttr->bField				);
        aTblCB		.Check	(pDocDisplayAttr->bTable				);
        aGrfCB		.Check	(pDocDisplayAttr->bGraphic 			    );
        aDrwCB		.Check  (pDocDisplayAttr->bDrawing 			    );
        aFldNameCB	.Check	(pDocDisplayAttr->bFieldName			);
        aPostItCB	.Check  (pDocDisplayAttr->bNotes				);
//											  bHtmlMode
    }

}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/


BOOL SwContentOptPage::FillItemSet(SfxItemSet& rSet)
{
    const SwDocDisplayItem*	pOldAttr = (const SwDocDisplayItem*)
                        GetOldItem(GetItemSet(), FN_PARAM_DOCDISP);

    SwDocDisplayItem aDisp;
    if(pOldAttr)
        aDisp = *pOldAttr;
    aDisp.bParagraphEnd	  		= aParaCB		.IsChecked();
    aDisp.bTab 				    = aTabCB		.IsChecked();
    aDisp.bSpace				= aSpacesCB		.IsChecked();
    aDisp.bNonbreakingSpace	    = aHSpacesCB	.IsChecked();
    aDisp.bSoftHyphen		    = aSHyphCB		.IsChecked();
    aDisp.bHiddenText		    = aHiddenCB		.IsChecked();
    aDisp.bShowHiddenPara		= aHiddenParaCB	.IsChecked();
    aDisp.bManualBreak 		    = aBreakCB		.IsChecked();
    aDisp.bIndexEntry		    = aIdxEntryCB	.IsChecked();
    aDisp.bIndexBackground		= aIdxBackCB	.IsChecked();
    aDisp.bFootnoteBackground   = aFootBackCB	.IsChecked();
    aDisp.bField				= aFldBackCB	.IsChecked();
    aDisp.bTable				= aTblCB		.IsChecked();
    aDisp.bGraphic 			    = aGrfCB		.IsChecked();
    aDisp.bDrawing 			    = aDrwCB		.IsChecked();
    aDisp.bFieldName			= aFldNameCB	.IsChecked();
    aDisp.bNotes 				= aPostItCB     .IsChecked();


    BOOL bRet = !pOldAttr || aDisp != *pOldAttr;
    if(bRet)
        bRet = 0 != rSet.Put(aDisp);
    return bRet;
}

/*-----------------31.08.96 10.30-------------------
 TabPage Anzeige/Layout
--------------------------------------------------*/


// -----------------------------------------------------------------------


SwLayoutOptPage::SwLayoutOptPage( Window* pParent,
                                      const SfxItemSet& rCoreSet ) :
    SfxTabPage( pParent, SW_RES( TP_LAYOUT_OPT ), rCoreSet ),
    aLineGB       ( this,	SW_RES( GB_LINE	    ) ),
    aTxtbegCB     ( this,	SW_RES( CB_TXTBEG	) ),
    aTblbegCB     ( this,	SW_RES( CB_TABBEG	) ),
    aSectBoundsCB( this,	SW_RES( CB_SECT_BOUNDS ) ),
    aCrossCB   	  ( this,	SW_RES( CB_CROSS     ) ),
    aSolidHandleCB( this, 	SW_RES( CB_HANDLE	) ),
    aBigHandleCB  ( this, 	SW_RES( CB_BIGHANDLE) ),
    aWindowGB     ( this,	SW_RES( GB_WINDOW	) ),
    aHScrollBox   ( this,	SW_RES( CB_HSCROLL   ) ),
    aVScrollBox   ( this,	SW_RES( CB_VSCROLL   ) ),
    aHRulerCBox   ( this, 	SW_RES( CB_HRULER  	) ),
    aVRulerCBox   ( this,	SW_RES( CB_VRULER    ) ),
    aSmoothCBox   ( this,	SW_RES( CB_SMOOTH_SCROLL    ) ),
    aMetricLB     ( this,	SW_RES( LB_METRIC 	) ),
    aMetricGB     ( this,	SW_RES( GB_METRIC 	) ),
    aTabGB		  ( this,	SW_RES( GB_TAB 		) ),
    aTabMF		  ( this,	SW_RES( MF_TAB 		) ),
    aMetricArr    ( SW_RES( ST_METRIC ) ),
    nLastTab(0)
{
    FreeResource();
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
            }
        }
    }
    aMetricLB.SetSelectHdl(LINK(this, SwLayoutOptPage, MetricHdl));
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aTabGB.Hide();
        aTabMF.Hide();
    }

}

#undef _INIT

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/

SwLayoutOptPage::~SwLayoutOptPage()
{
}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/


SfxTabPage*	SwLayoutOptPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet)
{
    return new SwLayoutOptPage(pParent, rAttrSet);
}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/


void SwLayoutOptPage::Reset(const SfxItemSet& rSet)
{
    const SwElemItem* pElemAttr = 0;

    rSet.GetItemState( FN_PARAM_ELEM , FALSE,
                                    (const SfxPoolItem**)&pElemAttr );
    if(pElemAttr)
    {
        aTxtbegCB  .Check( pElemAttr->bBounds	 		);
        aTblbegCB  .Check( pElemAttr->bTableBounds 	 	);
        aSectBoundsCB.Check( pElemAttr->bSectionBounds  );
        aCrossCB   .Check( pElemAttr->bCrosshair 		);
        aSolidHandleCB.Check( !pElemAttr->bHandles			);
        aBigHandleCB.Check(pElemAttr->bBigHandles 		);
        aHScrollBox.Check( pElemAttr->bHorzScrollbar	 );
        aVScrollBox.Check( pElemAttr->bVertScrollbar	 );
        aHRulerCBox.Check( pElemAttr->bHorzRuler		 );
        aVRulerCBox.Check( pElemAttr->bVertRuler		 );
        aSmoothCBox.Check( pElemAttr->bSmoothScroll		 );
    }


    aMetricLB.SetNoSelection();
    if ( rSet.GetItemState( SID_ATTR_METRIC ) >= SFX_ITEM_AVAILABLE )
    {
        const SfxUInt16Item& rItem = (SfxUInt16Item&)rSet.Get( SID_ATTR_METRIC );
        FieldUnit eFieldUnit = (FieldUnit)rItem.GetValue();

        for ( USHORT i = 0; i < aMetricLB.GetEntryCount(); ++i )
        {
            if ( (int)aMetricLB.GetEntryData( i ) == (int)eFieldUnit )
            {
                aMetricLB.SelectEntryPos( i );
                break;
            }
        }
        ::SetFieldUnit(aTabMF, eFieldUnit);
    }
    aMetricLB.SaveValue();

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_DEFTABSTOP, FALSE, &pItem))
    {
        nLastTab = ((SfxUInt16Item*)pItem)->GetValue();
        aTabMF.SetValue(aTabMF.Normalize(nLastTab), FUNIT_TWIP);
    }
    // in FrameDocs koennen Scrollbars nicht sinnvoll eingestellt werden
    if(SFX_ITEM_SET == rSet.GetItemState(FN_VIEW_IN_FRAME, FALSE, &pItem) &&
        ((const SfxBoolItem*)pItem)->GetValue())
    {
        aHScrollBox.Enable(FALSE);
        aVScrollBox.Enable(FALSE);
    }
    aTabMF.SaveValue();

}

/*-----------------31.08.96 13.58-------------------

--------------------------------------------------*/


BOOL SwLayoutOptPage::FillItemSet(SfxItemSet& rSet)
{
    const SwElemItem*	pOldAttr = (const SwElemItem*)
                        GetOldItem(GetItemSet(), FN_PARAM_ELEM);

    SwElemItem aElem;
    if(pOldAttr)
        aElem = *pOldAttr;

    aElem.bBounds	 	 = aTxtbegCB  .IsChecked();
    aElem.bTableBounds   = aTblbegCB  .IsChecked();
    aElem.bSectionBounds = aSectBoundsCB.IsChecked();
    aElem.bCrosshair 	 = aCrossCB   .IsChecked();
    aElem.bHandles		 = !aSolidHandleCB.IsChecked();
    aElem.bBigHandles	 = aBigHandleCB.IsChecked();
    aElem.bHorzScrollbar = aHScrollBox.IsChecked();
    aElem.bVertScrollbar = aVScrollBox.IsChecked();
    aElem.bHorzRuler	 = aHRulerCBox.IsChecked();
    aElem.bVertRuler	 = aVRulerCBox.IsChecked();
    aElem.bSmoothScroll	 = aSmoothCBox.IsChecked();

    BOOL bRet = !pOldAttr || aElem != *pOldAttr;
    if(bRet)
        bRet = 0 != rSet.Put(aElem);

    const USHORT nMPos = aMetricLB.GetSelectEntryPos();
    if ( nMPos != aMetricLB.GetSavedValue() )
    {
        // Doppel-Cast fuer VA3.0
        USHORT nFieldUnit = (USHORT)(long)aMetricLB.GetEntryData( nMPos );
        bRet |= 0 != rSet.Put( SfxUInt16Item( SID_ATTR_METRIC,
                                     (UINT16)nFieldUnit ) );
    }

    if(aTabMF.IsVisible() && aTabMF.GetText() != aTabMF.GetSavedValue())
        bRet |= 0 != rSet.Put(SfxUInt16Item(SID_ATTR_DEFTABSTOP,
                    (USHORT)aTabMF.Denormalize(aTabMF.GetValue(FUNIT_TWIP))));

    return bRet;
}

/*-----------------13.01.97 14.44-------------------
    Metric des Deftabstops umschalten
--------------------------------------------------*/

IMPL_LINK(SwLayoutOptPage, MetricHdl, ListBox*, EMPTYARG)
{
    const USHORT nMPos = aMetricLB.GetSelectEntryPos();
    if(nMPos != USHRT_MAX)
    {
        // Doppel-Cast fuer VA3.0
        FieldUnit eFieldUnit = (FieldUnit)(long)aMetricLB.GetEntryData( nMPos );
        BOOL bModified = aTabMF.IsModified();
        long nVal = bModified ?
            aTabMF.Denormalize( aTabMF.GetValue( FUNIT_TWIP ) ) :
                nLastTab;
        ::SetFieldUnit( aTabMF, eFieldUnit );
        aTabMF.SetValue( aTabMF.Normalize( nVal ), FUNIT_TWIP );
        if(!bModified)
            aTabMF.ClearModifyFlag();
    }

    return 0;
}

/*----------------- OS 27.01.95  -----------------------
 TabPage Drucker Zusatzeinstellungen
-------------------------------------------------------*/

// makro fuers init ------------------------------------------------------

#define _INIT \
    aGrfCB           (this, SW_RES(CB_PGRF)), \
    aTabCB           (this, SW_RES(CB_PTAB)), \
    aDrawCB          (this, SW_RES(CB_PDRAW)), \
    aCtrlFldCB       (this, SW_RES(CB_CTRLFLD)), \
    aBackgroundCB    (this, SW_RES(CB_BACKGROUND)),\
    aBlackFontCB 	 (this, SW_RES(CB_BLACK_FONT)),\
    aGroup1          (this, SW_RES(GRP_1)),  \
    aLeftPageCB      (this, SW_RES(CB_LEFTP)),\
    aRightPageCB     (this, SW_RES(CB_RIGHTP)),\
    aReverseCB       (this, SW_RES(CB_REVERSE)),\
    aProspectCB      (this, SW_RES(CB_PROSPECT)),\
    aGroup2          (this, SW_RES(GRP_2)),     \
    aNoRB            (this, SW_RES(RB_NO)),     \
    aOnlyRB          (this, SW_RES(RB_ONLY)),   \
    aEndRB           (this, SW_RES(RB_END)),    \
    aEndPageRB       (this, SW_RES(RB_PAGEEND)),\
    aGroup3          (this, SW_RES(GRP_3)),     \
    aPaperFromSetupCB(this, SW_RES(CB_PAPERFROMSETUP)),\
    aGroup5          (this, SW_RES(GRP_5)),     \
    aSingleJobsCB	 (this, SW_RES(CB_SINGLEJOBS)),\
    aGroup4          (this, SW_RES(GRP_4)),  \
    aFaxLB           (this, SW_RES(LB_FAX)), \
    aFaxGB           (this, SW_RES(GB_FAX)), \
    bAttrModified( FALSE )

// -----------------------------------------------------------------------


SwAddPrinterTabPage::SwAddPrinterTabPage( Window* pParent,
                                      const SfxItemSet& rCoreSet) :
    SfxTabPage( pParent, SW_RES( TP_OPTPRINT_PAGE ), rCoreSet),
    bPreview  ( FALSE ),
    _INIT
{
    Init();
    FreeResource();

    Link aLk = LINK( this, SwAddPrinterTabPage, AutoClickHdl);
    aGrfCB.SetClickHdl( aLk );
    aRightPageCB.SetClickHdl( aLk );
    aLeftPageCB.SetClickHdl( aLk );
    aTabCB.SetClickHdl( aLk );
    aDrawCB.SetClickHdl( aLk );
    aCtrlFldCB.SetClickHdl( aLk );
    aBackgroundCB.SetClickHdl( aLk );
    aBlackFontCB.SetClickHdl( aLk );
    aReverseCB.SetClickHdl( aLk );
    aProspectCB.SetClickHdl( aLk );
    aPaperFromSetupCB.SetClickHdl( aLk );
    aEndPageRB.SetClickHdl( aLk );
    aEndRB.SetClickHdl( aLk );
    aOnlyRB.SetClickHdl( aLk );
    aNoRB.SetClickHdl( aLk );
    aSingleJobsCB.SetClickHdl( aLk );
    aFaxLB.SetSelectHdl( LINK( this, SwAddPrinterTabPage, SelectHdl ) );

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aDrawCB      .Hide();
        aLeftPageCB  .Hide();
        aRightPageCB .Hide();
        aReverseCB.SetPosPixel(aLeftPageCB  .GetPosPixel());
        aProspectCB.SetPosPixel(aRightPageCB .GetPosPixel());
        aBlackFontCB.SetPosPixel(aBackgroundCB.GetPosPixel());
        aBackgroundCB.SetPosPixel(aCtrlFldCB.GetPosPixel());
        aCtrlFldCB.SetPosPixel(aDrawCB.GetPosPixel());
    }

}

#undef _INIT
//------------------------------------------------------------------------

void SwAddPrinterTabPage::SetPreview(BOOL bPrev)
{
    bPreview = bPrev;

    if (bPreview)
    {
        aLeftPageCB.Disable();
        aRightPageCB.Disable();
        aProspectCB.Disable();
        aGroup3.Disable();
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
        aAddPrinterAttr.bPrintTable 	= aTabCB.IsChecked();
        aAddPrinterAttr.bPrintDrawing 	= aDrawCB.IsChecked();
        aAddPrinterAttr.bPrintControl 	= aCtrlFldCB.IsChecked();
        aAddPrinterAttr.bPrintPageBackground = aBackgroundCB.IsChecked();
        aAddPrinterAttr.bPrintBlackFont = aBlackFontCB.IsChecked();

        aAddPrinterAttr.bPrintLeftPage	= aLeftPageCB.IsChecked();
        aAddPrinterAttr.bPrintRightPage = aRightPageCB.IsChecked();
        aAddPrinterAttr.bPrintReverse 	= aReverseCB.IsChecked();
        aAddPrinterAttr.bPrintProspect  = aProspectCB.IsChecked();
        aAddPrinterAttr.bPaperFromSetup = aPaperFromSetupCB.IsChecked();
        aAddPrinterAttr.bPrintSingleJobs = aSingleJobsCB.IsChecked();

        if (aNoRB.IsChecked()) 	aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_NONE;
        if (aOnlyRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ONLY;
        if (aEndRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ENDDOC;
        if (aEndPageRB.IsChecked()) aAddPrinterAttr.nPrintPostIts =
                                                        POSTITS_ENDPAGE;

        aAddPrinterAttr.sFaxName = aFaxLB.GetSelectEntry();
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
        aGrfCB.Check(			pAddPrinterAttr->bPrintGraphic);
        aTabCB.Check(			pAddPrinterAttr->bPrintTable);
        aDrawCB.Check(			pAddPrinterAttr->bPrintDrawing);
        aCtrlFldCB.Check(		pAddPrinterAttr->bPrintControl);
        aBackgroundCB.Check(    pAddPrinterAttr->bPrintPageBackground);
        aBlackFontCB.Check(		pAddPrinterAttr->bPrintBlackFont);
        aLeftPageCB.Check(		pAddPrinterAttr->bPrintLeftPage);
        aRightPageCB.Check(		pAddPrinterAttr->bPrintRightPage);
        aReverseCB.Check(		pAddPrinterAttr->bPrintReverse);
        aPaperFromSetupCB.Check(pAddPrinterAttr->bPaperFromSetup);
        aProspectCB.Check(      pAddPrinterAttr->bPrintProspect);
        aSingleJobsCB.Check(    pAddPrinterAttr->bPrintSingleJobs);

        aNoRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_NONE ) ;
        aOnlyRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ONLY ) ;
        aEndRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ENDDOC ) ;
        aEndPageRB.Check (pAddPrinterAttr->nPrintPostIts== POSTITS_ENDPAGE ) ;
        aFaxLB.SelectEntry( pAddPrinterAttr->sFaxName );
    }
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------


void 	SwAddPrinterTabPage::Init()
{

}
//------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwAddPrinterTabPage, AutoClickHdl, CheckBox *, EMPTYARG )
{
    bAttrModified = TRUE;
    return 0;
}
IMPL_LINK_INLINE_END( SwAddPrinterTabPage, AutoClickHdl, CheckBox *, EMPTYARG )

//------------------------------------------------------------------------


void  SwAddPrinterTabPage::SetFax( const SvStringsDtor& rFaxLst )
{
    for ( USHORT i = 0; i < rFaxLst.Count(); ++i )
        aFaxLB.InsertEntry( *rFaxLst.GetObject(i) );
    aFaxLB.SelectEntryPos( 0 );
}

//------------------------------------------------------------------------


IMPL_LINK_INLINE_START( SwAddPrinterTabPage, SelectHdl, ListBox *, EMPTYARG )
{
    bAttrModified=TRUE;
    return 0;
}
IMPL_LINK_INLINE_END( SwAddPrinterTabPage, SelectHdl, ListBox *, EMPTYARG )

/*-----------------03.09.96 11.53-------------------
    Tabpage Standardfonts
--------------------------------------------------*/


SwStdFontTabPage::SwStdFontTabPage( Window* pParent,
                                       const SfxItemSet& rSet ) :
    SfxTabPage( pParent, SW_RES( TP_STD_FONT ), rSet),
    aStandardLbl(this, SW_RES(FT_STANDARD)),
    aStandardBox(this, SW_RES(LB_STANDARD)),
    aTitleLbl   (this, SW_RES(FT_TITLE   )),
    aTitleBox   (this, SW_RES(LB_TITLE   )),
    aListLbl    (this, SW_RES(FT_LIST    )),
    aListBox    (this, SW_RES(LB_LIST    )),
    aLabelLbl   (this, SW_RES(FT_LABEL   )),
    aLabelBox   (this, SW_RES(LB_LABEL   )),
    aIdxLbl     (this, SW_RES(FT_IDX     )),
    aIdxBox     (this, SW_RES(LB_IDX     )),
    aStdChrFrm  (this, SW_RES(GB_STDCHR  )),
    aDocOnlyCB  (this, SW_RES(CB_DOCONLY )),
    aStandardPB (this, SW_RES(PB_STANDARD)),
    pPrt(0),
    pFontConfig(0),
    pWrtShell(0),
    bListDefault(FALSE),
    bLabelDefault(FALSE),
    bSetListDefault(TRUE),
    bSetLabelDefault(TRUE),
    bSetIdxDefault(TRUE),
    bIdxDefault(FALSE),
    bDeletePrinter(FALSE)
{
    FreeResource();
    aStandardPB.SetClickHdl(LINK(this, SwStdFontTabPage, StandardHdl));
    aStandardBox.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aListBox    .SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aLabelBox	.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));
    aIdxBox		.SetModifyHdl( LINK(this, SwStdFontTabPage, ModifyHdl));

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
                    SfxPrinter* pPrt, const String& rStyle)
{
    BOOL bDelete = FALSE;
    const SfxFont* pFnt = pPrt ? pPrt->GetFontByName(rStyle): 0;
    if(!pFnt)
    {
        pFnt = new SfxFont(FAMILY_DONTKNOW, rStyle);
        bDelete = TRUE;
    }
    SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(nType);
    pColl->SetAttr(SvxFontItem(pFnt->GetFamily(), pFnt->GetName(),
                aEmptyStr, pFnt->GetPitch(), pFnt->GetCharSet()));
    if(bDelete)
    {
        delete (SfxFont*) pFnt;
        bDelete = FALSE;
    }
}


/*-----------------03.09.96 11.53-------------------

--------------------------------------------------*/


BOOL SwStdFontTabPage::FillItemSet( SfxItemSet& rSet )
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


    if(bNotDocOnly)
    {
        if(sStandard != sStandardBak)
            pFontConfig->SetFontStandard(sStandard);
        if(sTitle != sTitleBak )
            pFontConfig->SetFontOutline(sTitle);
        if(sList != sListBak )
            pFontConfig->SetFontList(sList);
        if(sLabel != sLabelBak )
            pFontConfig->SetFontCaption(sLabel);
        if(sIdx != sIdxBak )
            pFontConfig->SetFontIndex(sIdx);
    }
    if(pWrtShell)
    {
        pWrtShell->StartAllAction();
        SfxPrinter* pPrt = pWrtShell->GetPrt();
        BOOL bMod = FALSE;
        if(sStandard != sShellStd)
        {
            BOOL bDelete = FALSE;
            const SfxFont* pFnt = pPrt ? pPrt->GetFontByName(sStandard): 0;
            if(!pFnt)
            {
                pFnt = new SfxFont(FAMILY_DONTKNOW, sStandard);
                bDelete = TRUE;
            }
            pWrtShell->SetDefault(SvxFontItem(pFnt->GetFamily(), pFnt->GetName(),
                                aEmptyStr, pFnt->GetPitch(), pFnt->GetCharSet()));
            SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_STANDARD);
            pColl->ResetAttr(RES_CHRATR_FONT);
            if(bDelete)
            {
                delete (SfxFont*) pFnt;
                bDelete = FALSE;
            }
//			lcl_SetColl(pWrtShell, RES_POOLCOLL_STANDARD, pPrt, sStandard);
            bMod = TRUE;
        }
        if(sTitle != sShellTitle )
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_HEADLINE_BASE, pPrt, sTitle);
            bMod = TRUE;
        }
        if(sList != sShellList && (!bListDefault || !bSetListDefault ))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_NUMBUL_BASE, pPrt, sList);
            bMod = TRUE;
        }
        if(sLabel != sShellLabel && (!bLabelDefault || !bSetLabelDefault))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_LABEL, pPrt, sLabel);
            bMod = TRUE;
        }
        if(sIdx != sShellIndex && (!bIdxDefault || !bSetIdxDefault))
        {
            lcl_SetColl(pWrtShell, RES_POOLCOLL_REGISTER_BASE, pPrt, sIdx);
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

void SwStdFontTabPage::Reset( const SfxItemSet& rSet )
{
    const SfxPoolItem* pItem;

    if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_PRINTER, FALSE, &pItem))
    {
        pPrt = (SfxPrinter*)((const SwPtrItem*)pItem)->GetValue();
    }
    else
    {
        SfxItemSet* pSet = new SfxItemSet( *rSet.GetPool(),
                    SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                    SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                    0 );
        pPrt = new SfxPrinter(pSet);
        bDeletePrinter = TRUE;
    }
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

    if(!pWrtShell)
    {
       sStdBackup = pFontConfig->GetFontStandard();
       sOutBackup = pFontConfig->GetFontOutline();
       sListBackup= pFontConfig->GetFontList();
       sCapBackup = pFontConfig->GetFontCaption();
       sIdxBackup = pFontConfig->GetFontIndex();
       aDocOnlyCB.Enable(FALSE);
    }
    else
    {
        SwTxtFmtColl *pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_STANDARD);
        sShellStd = sStdBackup =  pColl->GetFont().GetFamilyName();

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_HEADLINE_BASE);
        sShellTitle = sOutBackup = pColl->GetFont().GetFamilyName();

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_NUMBUL_BASE);
        bListDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(RES_CHRATR_FONT, FALSE);
        sShellList = sListBackup = pColl->GetFont().GetFamilyName();

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_LABEL);
        bLabelDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(RES_CHRATR_FONT, FALSE);
        sShellLabel = sCapBackup = pColl->GetFont().GetFamilyName();

        pColl = pWrtShell->GetTxtCollFromPool(RES_POOLCOLL_REGISTER_BASE);
        bIdxDefault = SFX_ITEM_DEFAULT == pColl->GetAttrSet().GetItemState(RES_CHRATR_FONT, FALSE);
        sShellIndex = sIdxBackup = pColl->GetFont().GetFamilyName();
    }
    aStandardBox.SetText(sStdBackup );
    aTitleBox   .SetText(sOutBackup );
    aListBox    .SetText(sListBackup);
    aLabelBox   .SetText(sCapBackup );
    aIdxBox     .SetText(sIdxBackup );

    aStandardBox.SaveValue();
    aTitleBox   .SaveValue();
    aListBox    .SaveValue();
    aLabelBox   .SaveValue();
    aIdxBox     .SaveValue();
}

/*-----------------07.09.96 12.28-------------------

--------------------------------------------------*/


IMPL_LINK( SwStdFontTabPage, StandardHdl, PushButton *, EMPTYARG )
{
    aStandardBox.SetText(SwStdFontConfig::GetDefaultFor(FONT_STANDARD));
    aTitleBox   .SetText(SwStdFontConfig::GetDefaultFor(FONT_OUTLINE));
    aListBox    .SetText(SwStdFontConfig::GetDefaultFor(FONT_LIST));
    aLabelBox   .SetText(SwStdFontConfig::GetDefaultFor(FONT_CAPTION));
    aIdxBox     .SetText(SwStdFontConfig::GetDefaultFor(FONT_INDEX));
    aStandardBox.SaveValue();
    aTitleBox   .SaveValue();
    aListBox    .SaveValue();
    aLabelBox   .SaveValue();
    aIdxBox     .SaveValue();

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

#undef _INIT

/*-----------------18.01.97 12.14-------------------
    Optionen Tabelle
--------------------------------------------------*/

SwTableOptionsTabPage::SwTableOptionsTabPage( Window* pParent, const SfxItemSet& rSet ) :
    SfxTabPage(pParent, SW_RES(TP_OPTTABLE_PAGE), rSet),
    aMoveGB(		this, ResId(GB_MOVE		)),
    aRowMoveFT(		this, ResId(FT_ROWMOVE	)),
    aRowMoveMF(		this, ResId(MF_ROWMOVE	)),
    aColMoveFT(		this, ResId(FT_COLMOVE	)),
    aColMoveMF(		this, ResId(MF_COLMOVE	)),
    aInsertGB(		this, ResId(GB_INSERT	)),
    aRowInsertFT(	this, ResId(FT_ROWINSERT)),
    aRowInsertMF(	this, ResId(MF_ROWINSERT)),
    aColInsertFT(	this, ResId(FT_COLINSERT)),
    aColInsertMF(	this, ResId(MF_COLINSERT)),
    aHandlingGB(	this, ResId(GB_HANDLING	)),
    aFixRB(			this, ResId(RB_FIX		)),
    aFixFT(			this, ResId(FT_FIX		)),
    aFixPropRB(		this, ResId(RB_FIXPROP	)),
    aFixPropFT(		this, ResId(FT_FIXPROP 	)),
    aVarRB(			this, ResId(RB_VAR		)),
    aVarFT(			this, ResId(FT_VAR		)),
    aDescFT(		this, ResId(FT_DESC		)),
    pWrtShell(0)
{
    FreeResource();
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

BOOL SwTableOptionsTabPage::FillItemSet( SfxItemSet& rSet )
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
        if(pWrtShell && SwWrtShell::SEL_TBL & pWrtShell->GetSelectionType())
        {
            pWrtShell->SetTblChgMode(eMode);
        }
        static USHORT __READONLY_DATA aInva[] =
                            {   FN_TABLE_MODE_FIX,
                                FN_TABLE_MODE_FIX_PROP,
                                FN_TABLE_MODE_VARIABLE,
                                0
                            };
        pWrtShell->GetView().GetViewFrame()->GetBindings().Invalidate( aInva );
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
}
/*-----------------19.04.97 13:20-------------------

--------------------------------------------------*/
SwSourceViewOptionsTabPage::SwSourceViewOptionsTabPage(
                    Window* pParent,   const SfxItemSet& rSet ) :
    SfxTabPage(pParent, SW_RES(TP_OPTSRCVIEW), rSet),
    aColorGB(this, 		ResId(GB_COLOR	)),
    aSGMLFT(this, 		ResId(FT_SGML	)),
    aSGMLLB(this, 		ResId(LB_SGML	)),
    aCommentFT(this, 	ResId(FT_COMMENT)),
    aCommentLB(this, 	ResId(LB_COMMENT)),
    aKeywdFT(this, 		ResId(FT_KEYWD	)),
    aKeywdLB(this, 		ResId(LB_KEYWD	)),
    aUnknownFT(this, 	ResId(FT_UNKNOWN)),
    aUnknownLB(this, 	ResId(LB_UNKNOWN))
{
    FreeResource();
    String sSGML	;
    String sComment	;
    String sKeywd	;
    String sUnknown	;

    XColorTable* pColorTbl = OFF_APP()->GetStdColorTable();
    SwSrcViewConfig* pSrcVwCfg = SW_MOD()->GetSourceViewConfig();
    USHORT nCount = (USHORT)pColorTbl->Count();
    aSGMLLB		.SetUpdateMode(FALSE);
    aCommentLB	.SetUpdateMode(FALSE);
    aKeywdLB	.SetUpdateMode(FALSE);
    aUnknownLB	.SetUpdateMode(FALSE);

    Color aTmpColSGML(pSrcVwCfg->GetSyntaxColor(SRC_SYN_SGML));
    Color aTmpColComment(pSrcVwCfg->GetSyntaxColor(SRC_SYN_COMMENT));
    Color aTmpColKey(pSrcVwCfg->GetSyntaxColor(SRC_SYN_KEYWRD));
    Color aTmpColUnknown(pSrcVwCfg->GetSyntaxColor(SRC_SYN_UNKNOWN));

    for(USHORT i = 0; i < nCount; i++)
    {
        XColorEntry* pEntry = pColorTbl->Get(i);
        Color aColor = pEntry->GetColor();
        String sName = pEntry->GetName();
        if(aColor == aTmpColSGML )
            sSGML = sName;
        if(aColor == aTmpColComment )
            sComment = sName;
        if(aColor == aTmpColKey )
            sKeywd = sName;
        if(aColor == aTmpColUnknown )
            sUnknown = sName;
        aSGMLLB		.InsertEntry(aColor, sName);
        aCommentLB	.InsertEntry(aColor, sName);
        aKeywdLB	.InsertEntry(aColor, sName);
        aUnknownLB	.InsertEntry(aColor, sName);
    }
    aSGMLLB		.SetUpdateMode(TRUE);
    aCommentLB	.SetUpdateMode(TRUE);
    aKeywdLB	.SetUpdateMode(TRUE);
    aUnknownLB	.SetUpdateMode(TRUE);
    aSGMLLB		.SelectEntry(sSGML		);
    aCommentLB	.SelectEntry(sComment	);
    aKeywdLB	.SelectEntry(sKeywd		);
    aUnknownLB	.SelectEntry(sUnknown	);

}

/*-----------------19.04.97 13:22-------------------

--------------------------------------------------*/
SwSourceViewOptionsTabPage::~SwSourceViewOptionsTabPage()
{
}

/*-----------------19.04.97 13:22-------------------

--------------------------------------------------*/
SfxTabPage*	SwSourceViewOptionsTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new SwSourceViewOptionsTabPage(pParent, rAttrSet);
}

/*-----------------19.04.97 13:23-------------------

--------------------------------------------------*/
BOOL SwSourceViewOptionsTabPage::FillItemSet( SfxItemSet& rSet )
{
    SwSrcViewConfig* pSrcVwCfg = SW_MOD()->GetSourceViewConfig();
    BOOL bModified = FALSE;
    if(aSGMLLB.GetSelectEntry().Len() &&
        !aSGMLLB.GetSelectEntryColor().IsRGBEqual(pSrcVwCfg->GetSyntaxColor(SRC_SYN_SGML)))
    {
        bModified = TRUE;
        pSrcVwCfg->SetSyntaxColor(SRC_SYN_SGML, aSGMLLB.GetSelectEntryColor());
    }
    if(aCommentLB.GetSelectEntry().Len() &&
        !aCommentLB.GetSelectEntryColor().IsRGBEqual(pSrcVwCfg->GetSyntaxColor(SRC_SYN_COMMENT)))
    {
        bModified = TRUE;
        pSrcVwCfg->SetSyntaxColor(SRC_SYN_COMMENT, aCommentLB.GetSelectEntryColor());
    }
    if(aKeywdLB.GetSelectEntry().Len() &&
        !aKeywdLB.GetSelectEntryColor().IsRGBEqual(pSrcVwCfg->GetSyntaxColor(SRC_SYN_KEYWRD)))
    {
        bModified = TRUE;
        pSrcVwCfg->SetSyntaxColor(SRC_SYN_KEYWRD, aKeywdLB.GetSelectEntryColor());
    }
    if(aUnknownLB.GetSelectEntry().Len() &&
        !aUnknownLB.GetSelectEntryColor().IsRGBEqual(pSrcVwCfg->GetSyntaxColor(SRC_SYN_UNKNOWN)))
    {
        bModified = TRUE;
        pSrcVwCfg->SetSyntaxColor(SRC_SYN_UNKNOWN, aUnknownLB.GetSelectEntryColor());
    }
    if(bModified)
    {
        TypeId aType(TYPE(SwSrcView));
        SfxViewShell* pShell = SfxViewShell::GetFirst(&aType);
        while( pShell )
        {
            ((SwSrcView*)pShell)->GetEditWin().SyntaxColorsChanged();
            pShell = SfxViewShell::GetNext(*pShell, &aType);
        }

    }
    return FALSE;
}

/*-----------------19.04.97 13:23-------------------

--------------------------------------------------*/
void SwSourceViewOptionsTabPage::Reset( const SfxItemSet& rSet )
{
    SwSrcViewConfig* pSrcVwCfg = SW_MOD()->GetSourceViewConfig();

    aSGMLLB		.SelectEntry( pSrcVwCfg->GetSyntaxColor(SRC_SYN_SGML) );
    aCommentLB	.SelectEntry( pSrcVwCfg->GetSyntaxColor(SRC_SYN_COMMENT) );
    aKeywdLB	.SelectEntry( pSrcVwCfg->GetSyntaxColor(SRC_SYN_KEYWRD) );
    aUnknownLB	.SelectEntry( pSrcVwCfg->GetSyntaxColor(SRC_SYN_UNKNOWN) );
}

/*  */

/*-----------------31.10.97 17:55-------------------
 TabPage fuer ShadowCrsr
--------------------------------------------------*/

SwShdwCrsrOptionsTabPage::SwShdwCrsrOptionsTabPage( Window* pParent,
                                                    const SfxItemSet& rSet )
    : SfxTabPage(pParent, SW_RES(TP_OPTSHDWCRSR), rSet),
    aFlagGB( this, SW_RES( GB_SHDWCRSFLAG )),
    aOnOffCB( this, SW_RES( CB_SHDWCRSONOFF )),
    aFillModeFT( this, SW_RES( FT_SHDWCRSFILLMODE )),
    aFillMarginRB( this, SW_RES( RB_SHDWCRSFILLMARGIN )),
    aFillIndentRB( this, SW_RES( RB_SHDWCRSFILLINDENT )),
    aFillTabRB( this, SW_RES( RB_SHDWCRSFILLTAB )),
    aFillSpaceRB( this, SW_RES( RB_SHDWCRSFILLSPACE )),
//	aColorGB( this, SW_RES( GB_SHDWCRSCOLOR )),
    aColorFT( this, SW_RES( FT_SHDWCRSCOLOR )),
    aColorLB( this, SW_RES( LB_SHDWCRSCOLOR )),
    aCrsrOptGB   ( this, SW_RES( GB_CRSR_OPT)),
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

    const Color& rShdwCol = aOpt.GetColor();

    String sColor;
    aColorLB.SetUpdateMode(FALSE);

    XColorTable* pColorTbl = OFF_APP()->GetStdColorTable();
    for( USHORT i = 0; i < pColorTbl->Count(); ++i )
    {
        XColorEntry* pEntry = pColorTbl->Get( i );
        Color aColor = pEntry->GetColor();
        String sName = pEntry->GetName();
        if( aColor == rShdwCol )
            sColor = sName;
        aColorLB.InsertEntry( aColor, sName );
    }
    aColorLB.SetUpdateMode( TRUE );
    aColorLB.SelectEntry( sColor );
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
    aOpt.SetColor( aColorLB.GetSelectEntryColor() );

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

    aColorLB.SelectEntry( aOpt.GetColor() );

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_CRSR_IN_PROTECTED, FALSE, &pItem ))
        aCrsrInProtCB.Check(((const SfxBoolItem*)pItem)->GetValue());
    aCrsrInProtCB.SaveValue();
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
static CharAttr __FAR_DATA aInsertAttr[] =
{
    SID_ATTR_CHAR_CASEMAP,		SVX_CASEMAP_NOT_MAPPED,
    SID_ATTR_CHAR_WEIGHT,		WEIGHT_BOLD,
    SID_ATTR_CHAR_POSTURE,  	ITALIC_NORMAL,
    SID_ATTR_CHAR_UNDERLINE,    UNDERLINE_SINGLE,
    SID_ATTR_CHAR_UNDERLINE,    UNDERLINE_DOUBLE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_VERSALIEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_GEMEINE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_KAPITAELCHEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_TITEL,
    SID_ATTR_BRUSH,				0,
};

static CharAttr __FAR_DATA aDeletedAttr[] =
{
    SID_ATTR_CHAR_CASEMAP,		SVX_CASEMAP_NOT_MAPPED,
    SID_ATTR_CHAR_WEIGHT,		WEIGHT_BOLD,
    SID_ATTR_CHAR_POSTURE,  	ITALIC_NORMAL,
    SID_ATTR_CHAR_STRIKEOUT,    STRIKEOUT_SINGLE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_VERSALIEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_GEMEINE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_KAPITAELCHEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_TITEL,
    SID_ATTR_BRUSH,				0,
};

static CharAttr __FAR_DATA aChangedAttr[] =
{
    SID_ATTR_CHAR_CASEMAP,		SVX_CASEMAP_NOT_MAPPED,
    SID_ATTR_CHAR_WEIGHT,		WEIGHT_BOLD,
    SID_ATTR_CHAR_POSTURE,  	ITALIC_NORMAL,
    SID_ATTR_CHAR_UNDERLINE,    UNDERLINE_SINGLE,
    SID_ATTR_CHAR_UNDERLINE,    UNDERLINE_DOUBLE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_VERSALIEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_GEMEINE,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_KAPITAELCHEN,
    SID_ATTR_CHAR_CASEMAP,      SVX_CASEMAP_TITEL,
    SID_ATTR_BRUSH,				0,
};

/*-----------------------------------------------------------------------
    Beschreibung: Markierungsvorschau
 -----------------------------------------------------------------------*/

SwMarkPreview::SwMarkPreview( Window *pParent, const ResId& rResID ) :

    Window(pParent, rResID),

    aTransColor	(COL_TRANSPARENT),
    aMarkColor	(COL_LIGHTRED),

    nMarkPos(0)

{
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

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

SwMarkPreview::~SwMarkPreview()
{
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

void SwMarkPreview::Paint(const Rectangle &rRect)
{
    // Schatten zeichnen
    Rectangle aShadow(aPage);
    aShadow += Point(3, 3);
    DrawRect(aShadow, Color(COL_GRAY), aTransColor);

    // Seite zeichnen
    DrawRect(aPage, Color(COL_WHITE), Color(COL_BLACK));

    // Separator zeichnen
    Rectangle aPageSeparator(aPage);
    aPageSeparator.SetSize(Size(2, aPageSeparator.GetHeight()));
    aPageSeparator.Move(aPage.GetWidth() / 2 - 1, 0);
    DrawRect(aPageSeparator, Color(COL_BLACK), aTransColor);

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
    DrawRect(aLeftMark, aMarkColor, aTransColor);
    DrawRect(aRightMark, aMarkColor, aTransColor);
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

void SwMarkPreview::PaintPage(const Rectangle &rRect)
{
    // PrintArea zeichnen
    DrawRect(rRect, aTransColor, Color(COL_GRAY));

    // Testabsatz zeichnen
    ULONG nLTxtBorder = 4;
    ULONG nRTxtBorder = 4;
    ULONG nTTxtBorder = 4;

    Rectangle aTextLine = rRect;
    aTextLine.SetSize(Size(aTextLine.GetWidth(), 2));
    aTextLine.Left()	+= nLTxtBorder;
    aTextLine.Right()	-= nRTxtBorder;
    aTextLine.Move(0, nTTxtBorder);

    USHORT nStep, nLines;
    const long nTxtLineHeight = aTextLine.GetHeight();

    nStep = aTextLine.GetHeight() + 2;
    nLines = (USHORT)(rRect.GetHeight() / (aTextLine.GetHeight() + 2)) - 1;

    // Text simulieren
    //
    for (USHORT i = 0; i < nLines; ++i)
    {
        if (i == (nLines - 1))
            aTextLine.SetSize(Size(aTextLine.GetWidth() / 2, aTextLine.GetHeight()));

        if (aPage.IsInside(aTextLine))
            DrawRect(aTextLine, Color(COL_GRAY), aTransColor);

        aTextLine.Move(0, nStep);
    }
    aTextLine.Move(0, -nStep);
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

void SwMarkPreview::DrawRect(const Rectangle &rRect, const Color &rFillColor, const Color &rLineColor)
{
    SetFillColor(rFillColor);
    SetLineColor(rLineColor);
    Window::DrawRect(rRect);
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

SwRedlineOptionsTabPage::SwRedlineOptionsTabPage( Window* pParent,
                                                    const SfxItemSet& rSet )
    : SfxTabPage(pParent, SW_RES(TP_REDLINE_OPT), rSet),

    aInsertRB			( this, SW_RES( RB_INSERT )),
    aDeletedRB			( this, SW_RES( RB_DELETED )),
    aChangedRB			( this, SW_RES( RB_CHANGED )),

    aInsertFT			( this, SW_RES( FT_TE_ATTRIB )),
    aInsertLB			( this, SW_RES( LB_TE_ATTRIB )),
    aDeletedLB			( this, SW_RES( LB_TL_ATTRIB )),
    aChangedLB			( this, SW_RES( LB_TC_ATTRIB )),

    aInsertColorFT		( this, SW_RES( FT_TE_COL )),
    aInsertColorLB		( this, SW_RES( LB_TE_COL )),
    aDeletedColorLB		( this, SW_RES( LB_TL_COL )),
    aChangedColorLB		( this, SW_RES( LB_TC_COL )),

    aAttribPreviewWN	( this, SW_RES( WIN_TE )),
    aInsertGB			( this, SW_RES( GB_TE )),

    aMarkPosFT			( this, SW_RES( FT_MARKPOS )),
    aMarkPosLB			( this, SW_RES( LB_MARKPOS )),
    aMarkColorFT		( this, SW_RES( FT_LC_COL )),
    aMarkColorLB		( this, SW_RES( LB_LC_COL )),
    aMarkPreviewWN		( this, SW_RES( WIN_MARK )),
    aChangedGB			( this, SW_RES( GB_LC )),
    sAuthor				( SW_RES( STR_AUTHOR )),
    sNone				( SW_RES( STR_NOTHING ))

{
    FreeResource();

    Link aLk = LINK(this, SwRedlineOptionsTabPage, RadioHdl);
    aInsertRB.SetClickHdl( aLk );
    aDeletedRB.SetClickHdl( aLk );
    aChangedRB.SetClickHdl( aLk );

    aLk = LINK(this, SwRedlineOptionsTabPage, AttribHdl);
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

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

SwRedlineOptionsTabPage::~SwRedlineOptionsTabPage()
{
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

SfxTabPage*	SwRedlineOptionsTabPage::Create( Window* pParent, const SfxItemSet& rSet )
{
    return new SwRedlineOptionsTabPage( pParent, rSet );
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

BOOL SwRedlineOptionsTabPage::FillItemSet( SfxItemSet& rSet )
{
    CharAttr *pAttr;
    SwModuleOptions *pOpt = SW_MOD()->GetModuleConfig();

    AuthorCharAttr aInsertAttr;
    AuthorCharAttr aDeletedAttr;
    AuthorCharAttr aChangedAttr;

    AuthorCharAttr aOldInsertAttr(pOpt->GetInsertAuthorAttr());
    AuthorCharAttr aOldDeletedAttr(pOpt->GetDeletedAuthorAttr());
    AuthorCharAttr aOldChangedAttr(pOpt->GetFormatAuthorAttr());

    ULONG nOldMarkColor = pOpt->GetMarkAlignColor().GetColor();
    USHORT nOldMarkMode = pOpt->GetMarkAlignMode();

    USHORT nPos = aInsertLB.GetSelectEntryPos();
    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        pAttr = (CharAttr *)aInsertLB.GetEntryData(nPos);
        aInsertAttr.nItemId = pAttr->nItemId;
        aInsertAttr.nAttr = pAttr->nAttr;

        nPos = aInsertColorLB.GetSelectEntryPos();

        switch (nPos)
        {
            case 0:
                aInsertAttr.nColor = COL_NONE;
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                aInsertAttr.nColor = COL_TRANSPARENT;
                break;
            default:
                aInsertAttr.nColor = aInsertColorLB.GetEntryColor(nPos).GetColor();
                break;
        }

        pOpt->SetInsertAuthorAttr(aInsertAttr);
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
        case 0:	nPos = HORI_NONE;		break;
        case 1:	nPos = HORI_LEFT;		break;
        case 2:	nPos = HORI_RIGHT; 		break;
        case 3:	nPos = HORI_OUTSIDE;	break;
        case 4:	nPos = HORI_INSIDE;		break;
    }
    pOpt->SetMarkAlignMode(nPos);

    pOpt->SetMarkAlignColor(aMarkColorLB.GetSelectEntryColor());

    if (!(aInsertAttr == aOldInsertAttr) ||
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

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

void SwRedlineOptionsTabPage::Reset( const SfxItemSet& rSet )
{
    const SwModuleOptions *pOpt = SW_MOD()->GetModuleConfig();

    const AuthorCharAttr &rInsertAttr = pOpt->GetInsertAuthorAttr();
    const AuthorCharAttr &rDeletedAttr = pOpt->GetDeletedAuthorAttr();
    const AuthorCharAttr &rChangedAttr = pOpt->GetFormatAuthorAttr();

    // Preview initialisieren
    InitFontStyle(aAttribPreviewWN);

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

    XColorTable* pColorTbl = OFF_APP()->GetStdColorTable();
    for( USHORT i = 0; i < pColorTbl->Count(); ++i )
    {
        XColorEntry* pEntry = pColorTbl->Get( i );
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

    ULONG nColor = rInsertAttr.nColor;

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

    // Attributlistboxen initialisieren
    USHORT nNum = sizeof(aInsertAttr) / sizeof(CharAttr);

    for (i = 0; i < nNum; i++)
    {
        aInsertLB.SetEntryData(i, &aInsertAttr[i]);
        if (aInsertAttr[i].nItemId == rInsertAttr.nItemId &&
            aInsertAttr[i].nAttr == rInsertAttr.nAttr)
            aInsertLB.SelectEntryPos(i);
    }

    nNum = sizeof(aDeletedAttr) / sizeof(CharAttr);
    for (i = 0; i < nNum; i++)
    {
        aDeletedLB.SetEntryData(i, &aDeletedAttr[i]);
        if (aDeletedAttr[i].nItemId == rDeletedAttr.nItemId &&
            aDeletedAttr[i].nAttr == rDeletedAttr.nAttr)
            aDeletedLB.SelectEntryPos(i);
    }

    nNum = sizeof(aChangedAttr) / sizeof(CharAttr);
    for (i = 0; i < nNum; i++)
    {
        aChangedLB.SetEntryData(i, &aChangedAttr[i]);
        if (aChangedAttr[i].nItemId == rChangedAttr.nItemId &&
            aChangedAttr[i].nAttr == rChangedAttr.nAttr)
            aChangedLB.SelectEntryPos(i);
    }

    USHORT nPos = 0;
    switch (pOpt->GetMarkAlignMode())
    {
        case HORI_NONE:		nPos = 0;	break;
        case HORI_LEFT:		nPos = 1;	break;
        case HORI_RIGHT:	nPos = 2; 	break;
        case HORI_OUTSIDE:	nPos = 3;	break;
        case HORI_INSIDE:	nPos = 4;	break;
    }
    aMarkPosLB.SelectEntryPos(nPos);

    // Einstellungen in Preview anzeigen
    AttribHdl(&aInsertLB);
    ColorHdl(&aInsertColorLB);

    ChangedMaskPrevHdl();
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

IMPL_LINK( SwRedlineOptionsTabPage, AttribHdl, ListBox *, pLB )
{
    SvxFontPrevWindow *pPrev = &aAttribPreviewWN;
    ColorListBox *pColorLB;

    if (pLB == &aInsertLB)
        pColorLB = &aInsertColorLB;
    else if (pLB == &aDeletedLB)
        pColorLB = &aDeletedColorLB;
    else
        pColorLB = &aChangedColorLB;

    SvxFont& rFont = pPrev->GetFont();
    rFont.SetWeight(WEIGHT_NORMAL);
    rFont.SetItalic(ITALIC_NONE);
    rFont.SetUnderline(UNDERLINE_NONE);
    rFont.SetStrikeout(STRIKEOUT_NONE);
    rFont.SetCaseMap(SVX_CASEMAP_NOT_MAPPED);
    pPrev->SetColor(Color(COL_WHITE));

    USHORT nPos = pColorLB->GetSelectEntryPos();

    switch (nPos)
    {
        case 0:
            rFont.SetColor(Color(COL_BLACK));
            break;
        case 1:
        case LISTBOX_ENTRY_NOTFOUND:
            rFont.SetColor(Color(COL_RED));
            break;
        default:
            rFont.SetColor(pColorLB->GetEntryColor(nPos));
            break;
    }

    nPos = pLB->GetSelectEntryPos();
    if (nPos == LISTBOX_ENTRY_NOTFOUND)
        nPos = 0;

    CharAttr *pAttr = (CharAttr *)pLB->GetEntryData(nPos);

    switch (pAttr->nItemId)
    {
        case SID_ATTR_CHAR_WEIGHT:
            rFont.SetWeight((FontWeight)pAttr->nAttr);
            break;

        case SID_ATTR_CHAR_POSTURE:
            rFont.SetItalic((FontItalic)pAttr->nAttr);
            break;

        case SID_ATTR_CHAR_UNDERLINE:
            rFont.SetUnderline((FontUnderline)pAttr->nAttr);
            break;

        case SID_ATTR_CHAR_STRIKEOUT:
            rFont.SetStrikeout((FontStrikeout)pAttr->nAttr);
            break;

        case SID_ATTR_CHAR_CASEMAP:
            rFont.SetCaseMap((SvxCaseMap)pAttr->nAttr);
            break;

        case SID_ATTR_BRUSH:
        {
            nPos = pColorLB->GetSelectEntryPos();
            if (nPos)
            {
                pPrev->SetColor(pColorLB->GetSelectEntryColor());
            }
            else
            {
                pPrev->SetColor(Color(COL_LIGHTGRAY));
            }
            rFont.SetColor(Color(COL_BLACK));
        }
        break;
    }

    pPrev->Invalidate();

    return 0;
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

IMPL_LINK( SwRedlineOptionsTabPage, ColorHdl, ColorListBox *, pColorLB )
{
    SvxFontPrevWindow *pPrev = &aAttribPreviewWN;
    ListBox *pLB;

    if (pColorLB == &aInsertColorLB)
        pLB = &aInsertLB;
    else if (pColorLB == &aDeletedColorLB)
        pLB = &aDeletedLB;
    else
        pLB = &aChangedLB;

    SvxFont& rFont = pPrev->GetFont();
    USHORT nPos = pLB->GetSelectEntryPos();
    if (nPos == LISTBOX_ENTRY_NOTFOUND)
        nPos = 0;

    CharAttr *pAttr = (CharAttr *)pLB->GetEntryData(nPos);

    if (pAttr->nItemId == SID_ATTR_BRUSH)
    {
        rFont.SetColor(Color(COL_BLACK));
        nPos = pColorLB->GetSelectEntryPos();
        if (nPos && nPos != LISTBOX_ENTRY_NOTFOUND)
        {
            pPrev->SetColor(pColorLB->GetSelectEntryColor());
        }
        else
        {
            pPrev->SetColor(Color(COL_LIGHTGRAY));
        }
    }
    else
    {
        nPos = pColorLB->GetSelectEntryPos();

        switch (nPos)
        {
            case 0:
                rFont.SetColor(Color(COL_BLACK));
                break;
            case 1:
            case LISTBOX_ENTRY_NOTFOUND:
                rFont.SetColor(Color(COL_RED));
                break;
            default:
                rFont.SetColor(pColorLB->GetEntryColor(nPos));
                break;
        }
    }

    pPrev->Invalidate();

    return 0;
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

IMPL_LINK( SwRedlineOptionsTabPage, ChangedMaskPrevHdl, ListBox *, pLB )
{
    aMarkPreviewWN.SetMarkPos(aMarkPosLB.GetSelectEntryPos());
    aMarkPreviewWN.SetColor(aMarkColorLB.GetSelectEntryColor().GetColor());

    aMarkPreviewWN.Invalidate();

    return 0;
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

IMPL_LINK( SwRedlineOptionsTabPage, RadioHdl, RadioButton *, pRB )
{
    BOOL bInsert = FALSE, bDeleted = FALSE, bChanged = FALSE;

    if (pRB == &aInsertRB)
    {
        AttribHdl(&aInsertLB);
        bInsert = TRUE;
    }
    else if (pRB == &aDeletedRB)
    {
        AttribHdl(&aDeletedLB);
        bDeleted = TRUE;
    }
    else if (pRB == &aChangedRB)
    {
        AttribHdl(&aChangedLB);
        bChanged = TRUE;
    }

    aInsertLB.Show(bInsert);
    aInsertColorLB.Show(bInsert);
    aDeletedLB.Show(bDeleted);
    aDeletedColorLB.Show(bDeleted);
    aChangedLB.Show(bChanged);
    aChangedColorLB.Show(bChanged);

    return 0;
}

/*-----------------------------------------------------------------------
    Beschreibung:
 -----------------------------------------------------------------------*/

void SwRedlineOptionsTabPage::InitFontStyle(SvxFontPrevWindow& rExampleWin)
{
    SvxFont& rFont = rExampleWin.GetFont();

    Font aFont;
    aFont.SetCharSet( gsl_getSystemTextEncoding() );
    aFont.SetSize( Size( 0, 12 ) );
    aFont.SetPitch( PITCH_VARIABLE );

    aFont.SetName( C2S("Times") );
    aFont.SetFamily( FAMILY_ROMAN );

//	Font aFont = System::GetStandardFont(STDFONT_ROMAN);
    aFont.SetFillColor(Color(COL_WHITE));
    aFont.SetWeight(WEIGHT_NORMAL);

    rFont = aFont;
    const Size aLogSize( rExampleWin.GetOutputSize() );
    rFont.SetSize(Size(0, aLogSize.Height() * 2 / 3));
    rExampleWin.SetFont(rFont);
    rExampleWin.UseResourceText();

    Color aWhiteColor = COL_WHITE;
    Wallpaper aWall(aWhiteColor);
//	Wallpaper aWall(Color( COL_WHITE ));
    rExampleWin.SetBackground(aWall);
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
    aTestGBox		( this,	WB_GROUP ),
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
    lcl_SetPosSize(aTestGBox, 	Point(6,2),	Size(209,126));
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

    aTestGBox.SetText( String(ByteString("Einstellungen nur f�r Testzwecke" ), RTL_TEXTENCODING_MS_1252));
    aTest1CBox .SetText( C2S("Layout not loading"));
    aTest2CBox .SetText( C2S("dynamic"));
    aTest3CBox .SetText( C2S("No calm"               ));
    aTest4CBox .SetText( C2S("WYSIWYG debug"         ));
    aTest5CBox .SetText( C2S("No idle format"        ));
    aTest6CBox .SetText( C2S("No screen adj"         ));
    aTest7CBox .SetText( C2S("win format"            ));
    aTest8CBox .SetText( C2S("No Scroll"             ));
    aTest9CBox .SetText( C2S("DrawingLayerNotLoading"));
    aTest10CBox.SetText( C2S("AutoFormat by Input"   ));
    aTestGBox.Show();
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


#undef _INIT
#endif

 /*--------------------------------------------------------------------
      Source Code Control System - Header

      $Header: /zpool/svn/migration/cvs_rep_09_09_08/code/sw/source/ui/config/optpage.cxx,v 1.1.1.1 2000-09-18 17:14:33 hr Exp $

    $Log: not supported by cvs2svn $
    Revision 1.123  2000/09/18 16:05:17  willem.vandorp
    OpenOffice header added.
    
    Revision 1.122  2000/09/08 15:11:56  os
    use configuration service
    
    Revision 1.121  2000/09/07 15:59:21  os
    change: SFX_DISPATCHER/SFX_BINDINGS removed

    Revision 1.120  2000/09/07 08:26:30  os
    FaxName now in SwPrintOptions

    Revision 1.119  2000/06/27 07:59:40  obo
    #65293

    Revision 1.118  2000/06/13 13:22:43  os
    #75770# Doc only flag in standard font TabPage now persistent

    Revision 1.117  2000/05/23 13:16:41  os
    wrong CreateFromAscii corrected

    Revision 1.116  2000/04/11 08:02:23  os
    UNICODE

    Revision 1.115  2000/03/03 15:16:58  os
    StarView remainders removed

    Revision 1.114  2000/02/24 17:26:00  hr
    43447#: gcc

    Revision 1.113  2000/02/11 14:44:06  hr
    #70473# changes for unicode ( patched by automated patchtool )

    Revision 1.112  2000/01/06 14:46:58  os
    #71448# no differences between PRODUCT and NONPRODUCT resources

    Revision 1.111  1999/12/14 14:31:36  jp
    Bug #69595#: print can create single Jobs

    Revision 1.110  1999/10/21 17:47:33  jp
    have to change - SearchFile with SfxIniManager, dont use SwFinder for this

    Revision 1.109  1999/09/22 12:37:43  os
    big handles

    Revision 1.108  1999/06/09 11:20:58  OS
    index background


      Rev 1.107   09 Jun 1999 13:20:58   OS
   index background

      Rev 1.106   10 May 1999 13:11:14   OS
   #64976# Tabulatorwert fuer Metricumstellung merken

      Rev 1.105   29 Mar 1999 09:17:00   OS
   #63971# kein TabStop putten, wenn Control unsichtbar

      Rev 1.104   19 Mar 1999 15:32:04   OS
   #63750# Tabellenmode auch wieder an der akt. Tabelle setzen

      Rev 1.103   15 Feb 1999 09:22:02   OS
   #61890# StdFontTabTage legt Drucker jetzt bei Bedarf selbst an

      Rev 1.102   27 Jan 1999 10:04:00   OS
   #58677# Cursor in Readonly-Bereichen

      Rev 1.101   11 Jan 1999 13:36:18   OS
   #60139# Set/UseDefault jetzt richtig

      Rev 1.100   16 Nov 1998 10:38:50   OS
   #59455# Grundschrifte auch nach Standard im Doc richtig setzen

      Rev 1.99   08 Sep 1998 16:49:54   OS
   #56134# Metric fuer Text und HTML getrennt

      Rev 1.98   06 Jul 1998 13:03:54   MH
   add: SfxBoolItem header

      Rev 1.97   03 Jul 1998 18:15:34   OS
   ScrollBar-CheckBoxen per Item disablen

      Rev 1.96   24 Jun 1998 16:16:56   OM
   #51575# Printoptionen auch in Seitenvorschau beachten

      Rev 1.95   24 Mar 1998 17:46:50   OM
   Formataenderung anzeigen

      Rev 1.94   10 Mar 1998 16:11:34   OM
   Gegen rottige Config abgesichert

      Rev 1.93   05 Mar 1998 14:34:08   OM
   Redline-Attribute in Module-Cfg speichern

      Rev 1.92   24 Feb 1998 15:32:24   OM
   Redline-Darstellungsoptionen setzen

      Rev 1.91   23 Feb 1998 12:40:44   OM
   Redlining-Optionen

      Rev 1.90   20 Feb 1998 14:45:52   OM
   Markierungs-Preview

      Rev 1.89   19 Feb 1998 15:26:24   OM
   Redlining-Optionen

      Rev 1.88   18 Feb 1998 09:46:54   OM
   Redlining Optionen

      Rev 1.87   18 Feb 1998 09:10:22   OM
   Redlining-Optionen

      Rev 1.86   28 Nov 1997 15:22:22   MA
   includes

      Rev 1.85   24 Nov 1997 17:58:50   MA
   include

      Rev 1.84   14 Nov 1997 17:13:34   OS
   TRUE returnen, wenn TblChgMode veraendert wurde #45521#

      Rev 1.83   03 Nov 1997 16:12:18   JP
   neu: Optionen/-Page/Basic-Schnittst. fuer ShadowCursor

      Rev 1.82   09 Oct 1997 14:40:40   OS
   UpdateMode fuer ColorListBoxen abschalten #44451#

      Rev 1.81   09 Sep 1997 11:38:50   OS
   includes

      Rev 1.80   15 Aug 1997 11:45:22   OS
   chartar/frmatr/txtatr aufgeteilt

      Rev 1.79   09 Aug 1997 12:50:22   OS
   paraitem/frmitems/textitem aufgeteilt

      Rev 1.78   08 Aug 1997 17:37:30   OM
   Headerfile-Umstellung

      Rev 1.77   08 Jul 1997 14:07:30   OS
   ConfigItems von der App ans Module

      Rev 1.76   12 Jun 1997 10:11:04   MA
   Option fuer Smooth

      Rev 1.75   06 Jun 1997 12:44:10   MA
   chg: versteckte Absaetze ausblenden

      Rev 1.74   05 May 1997 14:15:18   OS
   Grundschrift fuer Standard als Pool-Default setzen

      Rev 1.73   23 Apr 1997 14:20:36   OS
   GetEntryData von void* auf USHORT ueber long wg. IBM VA3.0

      Rev 1.72   21 Apr 1997 13:48:20   OS
   SEXPORT -> __EXPORT

      Rev 1.71   21 Apr 1997 11:01:42   OS
   Quelltextkonfiguration

      Rev 1.70   16 Apr 1997 12:30:38   OS
   SEXPORT

      Rev 1.69   25 Feb 1997 10:21:16   OS
   farbige Handles

      Rev 1.68   23 Feb 1997 10:34:12   OS
   zusaetzlicher Hilfstext fuer Tabellen

      Rev 1.67   03 Feb 1997 12:20:16   OS
   Hor. Scrollbar nicht mehr Browse-abhaengig

      Rev 1.66   03 Feb 1997 12:10:34   OS
   Pages fuer Html umgruppieren

      Rev 1.65   22 Jan 1997 11:36:30   MA
   Umstellung Put

      Rev 1.64   18 Jan 1997 15:12:16   OS
   Dialog Tabellenoptionen

      Rev 1.63   18 Jan 1997 11:14:46   OS
   Defaulteinstellungen fuer Grundschriften beruecksichtigen

      Rev 1.62   15 Jan 1997 16:21:54   MA
   #35010# Modified setzen

      Rev 1.61   14 Jan 1997 08:57:50   MA
   includes

      Rev 1.60   13 Jan 1997 16:54:54   OS
   TabStop auf der LayoutPage

      Rev 1.59   20 Dec 1996 13:38:28   OS
   auf SFX_ITEM_SET pruefen

      Rev 1.58   12 Dec 1996 08:44:16   OS
   keine Defaults suchen

      Rev 1.57   11 Dec 1996 14:12:48   OS
   Metric auf der Layout-Page

      Rev 1.56   10 Dec 1996 17:00:04   OS
   Warnungen von der OFA

      Rev 1.55   04 Dec 1996 23:02:42   MH
   chg: header

      Rev 1.54   03 Dec 1996 17:01:30   AMA
   Chg: Der Drucker wird nur im !Browsemodus angelegt.

      Rev 1.53   28 Nov 1996 15:20:26   OS
   neu: Schwarz drucken

      Rev 1.52   11 Nov 1996 09:22:10   MA
   ResMgr

      Rev 1.51   24 Oct 1996 13:14:58   OS
   Grundschriften nur nach Veraenderung im Doc auch in die Applikation eintragen

      Rev 1.50   23 Oct 1996 13:49:16   JP
   SVMEM -> SVSTDARR Umstellung

      Rev 1.49   02 Oct 1996 18:31:00   MA
   Umstellung Enable/Disable

      Rev 1.48   23 Sep 1996 19:28:28   HJS
   const nach nicht-const gecastet

      Rev 1.47   21 Sep 1996 15:56:42   OS
   Grundschriften in Comboboxen, damit nicht vorhandene Schriften gesetzt werden koennen; CheckBox fuer reine Dokumenteinstellung

      Rev 1.46   07 Sep 1996 14:04:28   OS
   Standard-Button fuer Grundschriften

      Rev 1.45   06 Sep 1996 14:32:06   OS
   Postits beruecksichtigen

      Rev 1.44   04 Sep 1996 08:09:12   OS
   neu: Grundschriften-Tabpage

      Rev 1.43   03 Sep 1996 16:56:24   OS
   Tabpage Standardfonts

      Rev 1.42   31 Aug 1996 17:14:00   OS
   neue Optionendialoge

      Rev 1.41   26 Aug 1996 16:57:54   MA
   chg: Lineale auch im BrowseMode

      Rev 1.40   17 Jul 1996 11:13:48   OS
   neu: Checkbox fuer Hintergrunddruck

      Rev 1.39   16 Jul 1996 20:19:50   MA
   includes

      Rev 1.38   24 May 1996 14:58:16   OS
    FN_STAT_MODIFIED -> SID_DOC_MODIFIED

      Rev 1.37   30 Apr 1996 14:03:10   OS
   Ansichts-Controls fuer Preview disablen

      Rev 1.36   25 Apr 1996 16:23:58   OS
   Im Browse-Mode Lineale und HScroll disablen

      Rev 1.35   15 Mar 1996 15:28:18   JP
   TestFlags erweitert

      Rev 1.34   07 Mar 1996 16:54:42   JP
   AddPrintItem um weiters Flag erweitert

      Rev 1.33   06 Feb 1996 15:18:42   JP
   Link Umstellung 305

      Rev 1.32   28 Nov 1995 23:37:36   JP
   uisys.hxx gibt es nicht mehr

 --------------------------------------------------------------------*/


