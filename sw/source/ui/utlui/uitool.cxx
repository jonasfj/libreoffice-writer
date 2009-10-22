/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: uitool.cxx,v $
 * $Revision: 1.26 $
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


#include <hintids.hxx>

#include <tools/datetime.hxx>
#include <vcl/svapp.hxx>
#include <unotools/collatorwrapper.hxx>
#include <svtools/urihelper.hxx>
#include <svtools/stritem.hxx>
#include <svtools/syslocale.hxx>
#include <sfx2/app.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <svx/pmdlitem.hxx>
#ifndef _SVX_TSTPITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#include <svx/boxitem.hxx>
#include <svx/sizeitem.hxx>
#include <svx/pageitem.hxx>
#include <svx/lrspitem.hxx>
#include <svtools/style.hxx>
#ifndef _VCL_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#include <unotools/localedatawrapper.hxx>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <comphelper/processfactory.hxx>
#include <sfx2/viewfrm.hxx>
#include <fmtornt.hxx>
#include <tabcol.hxx>
#include <edtwin.hxx>
#include <fmtfsize.hxx>
#include <fmthdft.hxx>
#include <fmtpdsc.hxx>
#include <wview.hxx>
#include <uiitems.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <wrtsh.hxx>
#include <swmodule.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <uitool.hxx>
#include <frmatr.hxx>
#include <paratr.hxx>
#include <fmtcol.hxx>
#include <poolfmt.hxx>
#include "usrpref.hxx"

#ifndef _ERROR_H
#include <error.h>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _UTLUI_HRC
#include <utlui.hrc>
#endif
#include <doc.hxx>
#include <docary.hxx>
#include <charfmt.hxx>
#include <SwStyleNameMapper.hxx>
// 50 cm 28350
//
#define MAXHEIGHT 28350
#define MAXWIDTH  28350

using namespace ::com::sun::star;
/*--------------------------------------------------------------------
    Beschreibung: Allgemeine List von StringPointern
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
    Beschreibung: Metric umschalten
 --------------------------------------------------------------------*/


void SetMetric(MetricFormatter& rCtrl, FieldUnit eUnit)
{
    SwTwips nMin = static_cast< SwTwips >(rCtrl.GetMin(FUNIT_TWIP));
    SwTwips nMax = static_cast< SwTwips >(rCtrl.GetMax(FUNIT_TWIP));

    rCtrl.SetUnit(eUnit);

    rCtrl.SetMin(nMin, FUNIT_TWIP);
    rCtrl.SetMax(nMax, FUNIT_TWIP);
}

/*--------------------------------------------------------------------
    Beschreibung:	Boxinfo-Attribut setzen
 --------------------------------------------------------------------*/


void PrepareBoxInfo(SfxItemSet& rSet, const SwWrtShell& rSh)
{
    SvxBoxInfoItem aBoxInfo( SID_ATTR_BORDER_INNER );
    const SfxPoolItem *pBoxInfo;
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BORDER_INNER,
                                        TRUE, &pBoxInfo))
        aBoxInfo = *(SvxBoxInfoItem*)pBoxInfo;

        // Tabellenvariante, wenn mehrere Tabellenzellen selektiert
    rSh.GetCrsr();					//Damit GetCrsrCnt() auch das Richtige liefert
    aBoxInfo.SetTable          (rSh.IsTableMode() && rSh.GetCrsrCnt() > 1);
        // Abstandsfeld immer anzeigen
    aBoxInfo.SetDist           ((BOOL) TRUE);
        // Minimalgroesse in Tabellen und Absaetzen setzen
    aBoxInfo.SetMinDist        (rSh.IsTableMode() || rSh.GetSelectionType() & (nsSelectionType::SEL_TXT | nsSelectionType::SEL_TBL));
        // Default-Abstand immer setzen
    aBoxInfo.SetDefDist        (MIN_BORDER_DIST);
        // Einzelne Linien koennen nur in Tabellen DontCare-Status haben
    aBoxInfo.SetValid(VALID_DISABLE, !rSh.IsTableMode());

    rSet.Put(aBoxInfo);
}

/*--------------------------------------------------------------------
    Beschreibung:	Header Footer fuellen
 --------------------------------------------------------------------*/


void FillHdFt(SwFrmFmt* pFmt, const  SfxItemSet& rSet)
{
    SwAttrSet aSet(pFmt->GetAttrSet());
    aSet.Put(rSet);

    const SvxSizeItem& rSize = (const SvxSizeItem&)rSet.Get(SID_ATTR_PAGE_SIZE);
    const SfxBoolItem& rDynamic = (const SfxBoolItem&)rSet.Get(SID_ATTR_PAGE_DYNAMIC);

    // Groesse umsetzen
    //
    SwFmtFrmSize aFrmSize(rDynamic.GetValue() ? ATT_MIN_SIZE : ATT_FIX_SIZE,
                            rSize.GetSize().Width(),
                            rSize.GetSize().Height());
    aSet.Put(aFrmSize);
    pFmt->SetFmtAttr(aSet);
}

/*--------------------------------------------------------------------
    Beschreibung:	PageDesc <-> in Sets wandeln und zurueck
 --------------------------------------------------------------------*/


void ItemSetToPageDesc( const SfxItemSet& rSet, SwPageDesc& rPageDesc )
{
    SwFrmFmt& rMaster = rPageDesc.GetMaster();

    // alle allgemeinen Rahmen-Attribute uebertragen
    //
    rMaster.SetFmtAttr(rSet);

    // PageData
    //
    if(rSet.GetItemState(SID_ATTR_PAGE) == SFX_ITEM_SET)
    {
        const SvxPageItem& rPageItem = (const SvxPageItem&)rSet.Get(SID_ATTR_PAGE);

        USHORT nUse = (USHORT)rPageItem.GetPageUsage();
        if(nUse & 0x04)
            nUse |= 0x03;
        if(nUse)
            rPageDesc.SetUseOn( (UseOnPage) nUse );
        rPageDesc.SetLandscape(rPageItem.IsLandscape());
        SvxNumberType aNumType;
        aNumType.SetNumberingType( static_cast< sal_Int16 >(rPageItem.GetNumType()) );
        rPageDesc.SetNumType(aNumType);
    }
    // Groesse
    //
    if(rSet.GetItemState(SID_ATTR_PAGE_SIZE) == SFX_ITEM_SET)
    {
        const SvxSizeItem& rSizeItem = (const SvxSizeItem&)rSet.Get(SID_ATTR_PAGE_SIZE);
        SwFmtFrmSize aSize(ATT_FIX_SIZE);
        aSize.SetSize(rSizeItem.GetSize());
        rMaster.SetFmtAttr(aSize);
    }
    // Kopzeilen-Attribute auswerten
    //
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_PAGE_HEADERSET,
            FALSE, &pItem ) )
    {
        const SfxItemSet& rHeaderSet = ((SvxSetItem*)pItem)->GetItemSet();
        const SfxBoolItem& rHeaderOn = (const SfxBoolItem&)rHeaderSet.Get(SID_ATTR_PAGE_ON);

        if(rHeaderOn.GetValue())
        {
            // Werte uebernehmen
            if(!rMaster.GetHeader().IsActive())
                rMaster.SetFmtAttr(SwFmtHeader(TRUE));

            // Das Headerformat rausholen und anpassen
            //
            SwFmtHeader aHeaderFmt(rMaster.GetHeader());
            SwFrmFmt *pHeaderFmt = aHeaderFmt.GetHeaderFmt();
            ASSERT(pHeaderFmt != 0, "kein HeaderFormat");

            ::FillHdFt(pHeaderFmt, rHeaderSet);

            rPageDesc.ChgHeaderShare(((const SfxBoolItem&)
                        rHeaderSet.Get(SID_ATTR_PAGE_SHARED)).GetValue());
        }
        else
        {	// Header ausschalten
            //
            if(rMaster.GetHeader().IsActive())
            {
                rMaster.SetFmtAttr(SwFmtHeader(BOOL(FALSE)));
                rPageDesc.ChgHeaderShare(FALSE);
            }
        }
    }

    // Fusszeilen-Attribute auswerten
    //
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_PAGE_FOOTERSET,
            FALSE, &pItem ) )
    {
        const SfxItemSet& rFooterSet = ((SvxSetItem*)pItem)->GetItemSet();
        const SfxBoolItem& rFooterOn = (const SfxBoolItem&)rFooterSet.Get(SID_ATTR_PAGE_ON);

        if(rFooterOn.GetValue())
        {
            // Werte uebernehmen
            if(!rMaster.GetFooter().IsActive())
                rMaster.SetFmtAttr(SwFmtFooter(TRUE));

            // Das Footerformat rausholen und anpassen
            //
            SwFmtFooter aFooterFmt(rMaster.GetFooter());
            SwFrmFmt *pFooterFmt = aFooterFmt.GetFooterFmt();
            ASSERT(pFooterFmt != 0, "kein FooterFormat");

            ::FillHdFt(pFooterFmt, rFooterSet);

            rPageDesc.ChgFooterShare(((const SfxBoolItem&)
                        rFooterSet.Get(SID_ATTR_PAGE_SHARED)).GetValue());
        }
        else
        {	// Footer ausschalten
            //
            if(rMaster.GetFooter().IsActive())
            {
                rMaster.SetFmtAttr(SwFmtFooter(BOOL(FALSE)));
                rPageDesc.ChgFooterShare(FALSE);
            }
        }
    }

    // Fussnoten
    //
    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_FTN_INFO,
            FALSE, &pItem ) )
        rPageDesc.SetFtnInfo( ((SwPageFtnInfoItem*)pItem)->GetPageFtnInfo() );


    //
    // Columns
    //

    // Registerhaltigkeit

    if(SFX_ITEM_SET == rSet.GetItemState(
                            SID_SWREGISTER_MODE, FALSE, &pItem))
    {
        BOOL bSet = ((const SfxBoolItem*)pItem)->GetValue();
        if(!bSet)
            rPageDesc.SetRegisterFmtColl(0);
        else if(SFX_ITEM_SET == rSet.GetItemState(
                                SID_SWREGISTER_COLLECTION, FALSE, &pItem))
        {
            const String& rColl = ((const SfxStringItem*)pItem)->GetValue();
            SwDoc& rDoc = *rMaster.GetDoc();
            SwTxtFmtColl* pColl = rDoc.FindTxtFmtCollByName( rColl );
            if( !pColl )
            {
                USHORT nId = SwStyleNameMapper::GetPoolIdFromUIName( rColl, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL );
                if( USHRT_MAX != nId )
                    pColl = rDoc.GetTxtCollFromPool( nId );
                else
                    pColl = rDoc.MakeTxtFmtColl( rColl,
                                (SwTxtFmtColl*)rDoc.GetDfltTxtFmtColl() );
            }
            if( pColl )
                pColl->SetFmtAttr( SwRegisterItem ( TRUE ));
            rPageDesc.SetRegisterFmtColl( pColl );
        }
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void PageDescToItemSet(	const SwPageDesc& rPageDesc, SfxItemSet& rSet)
{
    const SwFrmFmt& rMaster = rPageDesc.GetMaster();

    // Seitendaten
    //
    SvxPageItem aPageItem(SID_ATTR_PAGE);
    aPageItem.SetDescName(rPageDesc.GetName());
    aPageItem.SetPageUsage(rPageDesc.GetUseOn());
    aPageItem.SetLandscape(rPageDesc.GetLandscape());
    aPageItem.SetNumType((SvxNumType)rPageDesc.GetNumType().GetNumberingType());
    rSet.Put(aPageItem);

    // Groesse
    SvxSizeItem aSizeItem(SID_ATTR_PAGE_SIZE, rMaster.GetFrmSize().GetSize());
    rSet.Put(aSizeItem);

    // Maximale Groesse
    SvxSizeItem aMaxSizeItem(SID_ATTR_PAGE_MAXSIZE, Size(MAXWIDTH, MAXHEIGHT));
    rSet.Put(aMaxSizeItem);

    // Raender, Umrandung und das andere Zeug
    //
    rSet.Put(rMaster.GetAttrSet());

    SvxBoxInfoItem aBoxInfo( SID_ATTR_BORDER_INNER );
    const SfxPoolItem *pBoxInfo;
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BORDER_INNER,
                                            TRUE, &pBoxInfo) )
        aBoxInfo = *(SvxBoxInfoItem*)pBoxInfo;

    aBoxInfo.SetTable( FALSE );
        // Abstandsfeld immer anzeigen
    aBoxInfo.SetDist( TRUE);
        // Minimalgroesse in Tabellen und Absaetzen setzen
    aBoxInfo.SetMinDist( FALSE );
    // Default-Abstand immer setzen
    aBoxInfo.SetDefDist( MIN_BORDER_DIST );
        // Einzelne Linien koennen nur in Tabellen DontCare-Status haben
    aBoxInfo.SetValid( VALID_DISABLE );
    rSet.Put( aBoxInfo );


    SfxStringItem aFollow(SID_ATTR_PAGE_EXT1, aEmptyStr);
    if(rPageDesc.GetFollow())
        aFollow.SetValue(rPageDesc.GetFollow()->GetName());
    rSet.Put(aFollow);

    // Header
    //
    if(rMaster.GetHeader().IsActive())
    {
        const SwFmtHeader &rHeaderFmt = rMaster.GetHeader();
        const SwFrmFmt *pHeaderFmt = rHeaderFmt.GetHeaderFmt();
        ASSERT(pHeaderFmt != 0, kein HeaderFormat.);

        // HeaderInfo, Raender, Hintergrund, Umrandung
        //
        SfxItemSet aHeaderSet( *rSet.GetPool(),
                    SID_ATTR_PAGE_ON, 		SID_ATTR_PAGE_SHARED,
                    SID_ATTR_PAGE_SIZE, 	SID_ATTR_PAGE_SIZE,
                    SID_ATTR_BORDER_INNER,	SID_ATTR_BORDER_INNER,
                    RES_FRMATR_BEGIN,		RES_FRMATR_END-1,
                    0);

        // dynamische oder feste Hoehe
        //
        SfxBoolItem aOn(SID_ATTR_PAGE_ON, TRUE);
        aHeaderSet.Put(aOn);

        const SwFmtFrmSize &rFrmSize = pHeaderFmt->GetFrmSize();
        const SwFrmSize eSizeType = rFrmSize.GetHeightSizeType();
        SfxBoolItem aDynamic(SID_ATTR_PAGE_DYNAMIC, eSizeType != ATT_FIX_SIZE);
        aHeaderSet.Put(aDynamic);

        // Links gleich rechts
        //
        SfxBoolItem aShared(SID_ATTR_PAGE_SHARED, rPageDesc.IsHeaderShared());
        aHeaderSet.Put(aShared);

        // Groesse
        SvxSizeItem aSize(SID_ATTR_PAGE_SIZE, Size(rFrmSize.GetSize()));
        aHeaderSet.Put(aSize);

        // Rahmen-Attribute umschaufeln
        //
        aHeaderSet.Put(pHeaderFmt->GetAttrSet());
        aHeaderSet.Put( aBoxInfo );

        // SetItem erzeugen
        //
        SvxSetItem aSetItem(SID_ATTR_PAGE_HEADERSET, aHeaderSet);
        rSet.Put(aSetItem);
    }

    // Footer
    if(rMaster.GetFooter().IsActive())
    {
        const SwFmtFooter &rFooterFmt = rMaster.GetFooter();
        const SwFrmFmt *pFooterFmt = rFooterFmt.GetFooterFmt();
        ASSERT(pFooterFmt != 0, kein FooterFormat.);

        // FooterInfo, Raender, Hintergrund, Umrandung
        //
        SfxItemSet aFooterSet( *rSet.GetPool(),
                    SID_ATTR_PAGE_ON, 		SID_ATTR_PAGE_SHARED,
                    SID_ATTR_PAGE_SIZE, 	SID_ATTR_PAGE_SIZE,
                    SID_ATTR_BORDER_INNER,	SID_ATTR_BORDER_INNER,
                    RES_FRMATR_BEGIN, 		RES_FRMATR_END-1,
                    0);

        // dynamische oder feste Hoehe
        //
        SfxBoolItem aOn(SID_ATTR_PAGE_ON, TRUE);
        aFooterSet.Put(aOn);

        const SwFmtFrmSize &rFrmSize = pFooterFmt->GetFrmSize();
        const SwFrmSize eSizeType = rFrmSize.GetHeightSizeType();
        SfxBoolItem aDynamic(SID_ATTR_PAGE_DYNAMIC, eSizeType != ATT_FIX_SIZE);
        aFooterSet.Put(aDynamic);

        // Links gleich rechts
        //
        SfxBoolItem aShared(SID_ATTR_PAGE_SHARED, rPageDesc.IsFooterShared());
        aFooterSet.Put(aShared);

        // Groesse
        SvxSizeItem aSize(SID_ATTR_PAGE_SIZE, Size(rFrmSize.GetSize()));
        aFooterSet.Put(aSize);

        // Rahmen-Attribute umschaufeln
        //
        aFooterSet.Put(pFooterFmt->GetAttrSet());
        aFooterSet.Put( aBoxInfo );

        // SetItem erzeugen
        //
        SvxSetItem aSetItem(SID_ATTR_PAGE_FOOTERSET, aFooterSet);
        rSet.Put(aSetItem);
    }

    // Fussnoten einbauen
    //
    SwPageFtnInfo& rInfo = (SwPageFtnInfo&)rPageDesc.GetFtnInfo();
    SwPageFtnInfoItem aFtnItem(FN_PARAM_FTN_INFO, rInfo);
    rSet.Put(aFtnItem);

    // Registerhaltigkeit

    const SwTxtFmtColl* pCol = rPageDesc.GetRegisterFmtColl();
    SwRegisterItem aReg(pCol != 0);
    aReg.SetWhich(SID_SWREGISTER_MODE);
    rSet.Put(aReg);
    if(pCol)
        rSet.Put(SfxStringItem(SID_SWREGISTER_COLLECTION, pCol->GetName()));

}


/*--------------------------------------------------------------------
    Beschreibung:	DefaultTabs setzen
 --------------------------------------------------------------------*/


void MakeDefTabs(SwTwips nDefDist, SvxTabStopItem& rTabs)
{
    if( rTabs.Count() )
        return;
    {
        SvxTabStop aSwTabStop( nDefDist, SVX_TAB_ADJUST_DEFAULT );
        rTabs.Insert( aSwTabStop );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Abstand zwischen zwei Tabs
 --------------------------------------------------------------------*/


USHORT GetTabDist(const SvxTabStopItem& rTabs)
{
    USHORT nDefDist;
    if( rTabs.Count() )
        nDefDist = (USHORT)( rTabs[0].GetTabPos() );
    else
        nDefDist = 1134;     // 2cm
    return nDefDist;
}


// erfrage ob im Set eine Sfx-PageDesc-Kombination vorliegt und returne diese
void SfxToSwPageDescAttr( const SwWrtShell& rShell, SfxItemSet& rSet )
{
    const SfxPoolItem* pItem;
    SwFmtPageDesc aPgDesc;

    BOOL bChanged = FALSE;
    // Seitennummer
    if(SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_PARA_PAGENUM, FALSE, &pItem))
    {
        aPgDesc.SetNumOffset(((SfxUInt16Item*)pItem)->GetValue());
        bChanged = TRUE;
    }
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_PARA_MODEL, FALSE, &pItem ))
    {
        const String& rDescName = ((SvxPageModelItem*)pItem)->GetValue();
        if( rDescName.Len() )	// kein Name -> PageDesc ausschalten!
        {
            // nur loeschen, wenn PageDesc eingschaltet wird!
            rSet.ClearItem( RES_BREAK );
            SwPageDesc* pDesc = ((SwWrtShell&)rShell).FindPageDescByName(
                                                    rDescName, TRUE );
            if( pDesc )
                pDesc->Add( &aPgDesc );
        }
        rSet.ClearItem( SID_ATTR_PARA_MODEL );
        bChanged = TRUE;
    }
    else
    {
        SfxItemSet aCoreSet(rShell.GetView().GetPool(), RES_PAGEDESC, RES_PAGEDESC );
        rShell.GetCurAttr( aCoreSet );
        if(SFX_ITEM_SET == aCoreSet.GetItemState( RES_PAGEDESC, TRUE, &pItem ) )
        {
            if( ((SwFmtPageDesc*)pItem)->GetPageDesc() )
            {
                ((SwFmtPageDesc*)pItem)->GetPageDesc()->Add(&aPgDesc);
            }
        }
    }


    if(bChanged)
        rSet.Put( aPgDesc );
}


// erfrage ob im Set eine Sfx-PageDesc-Kombination vorliegt und returne diese
void SwToSfxPageDescAttr( SfxItemSet& rCoreSet )
{
    const SfxPoolItem* pItem = 0;
    String aName;
    USHORT nPageNum = 0;
    BOOL bPut = TRUE;
    switch( rCoreSet.GetItemState( RES_PAGEDESC, TRUE, &pItem ) )
    {
    case SFX_ITEM_SET:
        {
            if( ((SwFmtPageDesc*)pItem)->GetPageDesc() )
            {
                aName = ((SwFmtPageDesc*)pItem)->GetPageDesc()->GetName();
                nPageNum = ((SwFmtPageDesc*)pItem)->GetNumOffset();
            }
            rCoreSet.ClearItem( RES_PAGEDESC );
            // Seitennummer
        }
        break;

    case SFX_ITEM_AVAILABLE:
        break;

    default:
        bPut = FALSE;
    }
    SfxUInt16Item aPageNum( SID_ATTR_PARA_PAGENUM, nPageNum );
    rCoreSet.Put( aPageNum );

    if(bPut)
        rCoreSet.Put( SvxPageModelItem( aName, TRUE, SID_ATTR_PARA_MODEL ) );
}

/*--------------------------------------------------------------------
    Beschreibung:	Metric ermitteln
 --------------------------------------------------------------------*/


FieldUnit	GetDfltMetric(BOOL bWeb)
{
    return SW_MOD()->GetUsrPref(bWeb)->GetMetric();
}

/*--------------------------------------------------------------------
    Beschreibung:	Metric ermitteln
 --------------------------------------------------------------------*/


void	SetDfltMetric( FieldUnit eMetric, BOOL bWeb )
{
    SW_MOD()->ApplyUserMetric(eMetric, bWeb);
}

/*-----------------09.04.98 16:58-------------------

--------------------------------------------------*/
USHORT InsertStringSorted(const String& rEntry, ListBox& rToFill, USHORT nOffset )
{
    USHORT i = nOffset;
    CollatorWrapper& rCaseColl = ::GetAppCaseCollator();

    for( ; i < rToFill.GetEntryCount(); i++ )
    {
        if( 0 < rCaseColl.compareString( rToFill.GetEntry(i), rEntry ))
            break;
    }
    return rToFill.InsertEntry(rEntry, i);
}
void FillCharStyleListBox(ListBox& rToFill, SwDocShell* pDocSh, BOOL bSorted, BOOL bWithDefault)
{
    BOOL bHasOffset = rToFill.GetEntryCount() > 0;
    SfxStyleSheetBasePool* pPool = pDocSh->GetStyleSheetPool();
    pPool->SetSearchMask(SFX_STYLE_FAMILY_CHAR, SFXSTYLEBIT_ALL);
    SwDoc* pDoc = pDocSh->GetDoc();
    const SfxStyleSheetBase* pBase = pPool->First();
    String sStandard;
    SwStyleNameMapper::FillUIName( RES_POOLCOLL_STANDARD, sStandard );
    while(pBase)
    {
        if(bWithDefault || pBase->GetName() !=  sStandard)
        {
            USHORT nPos;
            if(bSorted)
                nPos = InsertStringSorted(pBase->GetName(), rToFill, bHasOffset );
            else
                nPos = rToFill.InsertEntry(pBase->GetName());
            long nPoolId = SwStyleNameMapper::GetPoolIdFromUIName( pBase->GetName(), nsSwGetPoolIdFromName::GET_POOLID_CHRFMT );
            rToFill.SetEntryData( nPos, (void*) (nPoolId));
        }
        pBase = pPool->Next();
    }
    // non-pool styles
    const SwCharFmts* pFmts = pDoc->GetCharFmts();
    for(USHORT i = 0; i < pFmts->Count(); i++)
    {
        const SwCharFmt* pFmt = (*pFmts)[i];
        if(pFmt->IsDefault())
            continue;
        const String& rName = pFmt->GetName();
        if(rToFill.GetEntryPos(rName) == LISTBOX_ENTRY_NOTFOUND)
        {
            USHORT nPos;
            if(bSorted)
                nPos = InsertStringSorted(rName, rToFill, bHasOffset );
            else
                nPos = rToFill.InsertEntry(rName);
            long nPoolId = USHRT_MAX;
            rToFill.SetEntryData( nPos, (void*) (nPoolId));
        }
    }
};

/* -----------------27.04.98 08:26-------------------
 *
 * --------------------------------------------------*/
SwTwips GetTableWidth( SwFrmFmt* pFmt, SwTabCols& rCols, USHORT *pPercent,
            SwWrtShell* pSh )
{
    //Die Breite zu besorgen ist etwas komplizierter.
    SwTwips nWidth = 0;
    const sal_Int16 eOri = pFmt->GetHoriOrient().GetHoriOrient();
    switch(eOri)
    {
        case text::HoriOrientation::FULL: nWidth = rCols.GetRight(); break;
        case text::HoriOrientation::LEFT_AND_WIDTH:
        case text::HoriOrientation::LEFT:
        case text::HoriOrientation::RIGHT:
        case text::HoriOrientation::CENTER:
            nWidth = pFmt->GetFrmSize().GetWidth();
        break;
        default:
        {
            if(pSh)
            {
                const SwFrmFmt *pFlyFmt;
                if ( 0 == (pFlyFmt = pSh->GetFlyFrmFmt()) )
                {
                    nWidth = pSh->GetAnyCurRect(RECT_PAGE_PRT).Width();
                }
                else
                {
                    nWidth = pSh->GetAnyCurRect(RECT_FLY_PRT_EMBEDDED).Width();
                }
            }
            else
            {
                DBG_ERROR("wo soll die Breite denn herkommen?");
            }
            const SvxLRSpaceItem& rLRSpace = pFmt->GetLRSpace();
            nWidth -= (rLRSpace.GetRight() + rLRSpace.GetLeft());
        }
    }
    if (pPercent)
        *pPercent = pFmt->GetFrmSize().GetWidthPercent();
    return nWidth;
}

/*------------------------------------------------------------------------*/

String GetAppLangDateTimeString( const DateTime& rDT )
{
    const LocaleDataWrapper& rAppLclData = SvtSysLocale().GetLocaleData();
    String sRet( rAppLclData.getDate( rDT ));
    ( sRet += ' ' ) += rAppLclData.getTime( rDT, FALSE, FALSE );
    return sRet;
}

/*-- 26.01.2006 08:06:33---------------------------------------------------

  -----------------------------------------------------------------------*/
bool ExecuteMenuCommand( PopupMenu& rMenu, SfxViewFrame& rViewFrame, USHORT nId )
{
    bool bRet = false;
    USHORT nItemCount = rMenu.GetItemCount();
    String sCommand;
    for( USHORT nItem = 0; nItem < nItemCount; ++nItem)
    {
        PopupMenu* pPopup = rMenu.GetPopupMenu( rMenu.GetItemId( nItem ) );
        if(pPopup)
        {
            sCommand = pPopup->GetItemCommand(nId);
            if(sCommand.Len())
                break;
        }
    }
    if(sCommand.Len())
    {
        uno::Reference< frame::XFrame >  xFrame = rViewFrame.GetFrame()->GetFrameInterface();
        uno::Reference < frame::XDispatchProvider > xProv( xFrame, uno::UNO_QUERY );
        util::URL aURL;
        aURL.Complete = sCommand;
        uno::Reference < util::XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer" )), uno::UNO_QUERY );
        xTrans->parseStrict( aURL );
        uno::Reference< frame::XDispatch >  xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );
        if( xDisp.is() )
        {
            uno::Sequence< beans::PropertyValue > aSeq;
            xDisp->dispatch( aURL, aSeq );
            bRet = true;
        }
    }
    return bRet;
}
