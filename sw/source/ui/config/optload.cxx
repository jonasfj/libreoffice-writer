/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optload.cxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-07 16:32:30 $
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



#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _UIITEMS_HXX
#include <uiitems.hxx>
#endif
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#include "swmodule.hxx"
#include "usrpref.hxx"
#include "wrtsh.hxx"
#include "linkenum.hxx"
#include <uitool.hxx>
#include <view.hxx>

#include "globals.hrc"
#include "cmdid.h"

#include "optload.hrc"
#include "optload.hxx"
#ifndef _SVX_DLGUTIL_HXX
#include <svx/dlgutil.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX
#include <svx/htmlmode.hxx>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _EXPFLD_HXX //autogen
#include <expfld.hxx>
#endif
#include <caption.hxx>
#ifndef _COM_SUN_STAR_DOCUMENT_PRINTERINDEPENDENTLAYOUT_HPP_
#include <com/sun/star/document/PrinterIndependentLayout.hpp>
#endif

#include <svtools/insdlg.hxx>
#include <sot/clsids.hxx>

#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif

#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _CONFIG_HRC
#include <config.hrc>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#include <SwNodeNum.hxx>

#include <doc.hxx>
#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif

using namespace ::com::sun::star;

/* -----------------22.10.98 15:12-------------------
 *
 * --------------------------------------------------*/
SwLoadOptPage::SwLoadOptPage( Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, SW_RES( TP_OPTLOAD_PAGE ), rSet ),

    aUpdateFL           ( this, SW_RES( FL_UPDATE ) ),
    aLinkFT             ( this, SW_RES( FT_LINK ) ),
    aAlwaysRB           ( this, SW_RES( RB_ALWAYS ) ),
    aRequestRB          ( this, SW_RES( RB_REQUEST ) ),
    aNeverRB            ( this, SW_RES( RB_NEVER  ) ),

    aFieldFT            ( this, SW_RES( FT_FIELD ) ),
    aAutoUpdateFields   ( this, SW_RES( CB_AUTO_UPDATE_FIELDS ) ),
    aAutoUpdateCharts   ( this, SW_RES( CB_AUTO_UPDATE_CHARTS ) ),

    aSettingsFL         ( this, SW_RES( FL_SETTINGS ) ),
    aMetricFT           ( this, SW_RES( FT_METRIC ) ),
    aMetricLB           ( this, SW_RES( LB_METRIC ) ),
    aTabFT              ( this, SW_RES( FT_TAB ) ),
    aTabMF              ( this, SW_RES( MF_TAB ) ),
    aUseSquaredPageMode ( this, SW_RES( CB_USE_SQUARE_PAGE_MODE ) ),

    pWrtShell	( NULL ),
    bHTMLMode   ( FALSE ),
    nLastTab	( 0 ),
    nOldLinkMode( MANUAL )

{
    FreeResource();

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
            }
            default:; //prevent warning
        }
    }
    aMetricLB.SetSelectHdl(LINK(this, SwLoadOptPage, MetricHdl));

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState(SID_HTML_MODE, FALSE, &pItem )
        && ((SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON)
    {
        aTabFT.Hide();
        aTabMF.Hide();
    }
    
    SvtCJKOptions aCJKOptions;
    if(!aCJKOptions.IsAsianTypographyEnabled())
        aUseSquaredPageMode.Hide();
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SwLoadOptPage::~SwLoadOptPage()
{
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SfxTabPage*	__EXPORT SwLoadOptPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new SwLoadOptPage(pParent, rAttrSet );
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

BOOL __EXPORT SwLoadOptPage::FillItemSet( SfxItemSet& rSet )
{
    BOOL bRet = FALSE;
    SwModule* pMod = SW_MOD();

    USHORT nNewLinkMode = AUTOMATIC;
    if (aNeverRB.IsChecked())
        nNewLinkMode = NEVER;
    else if (aRequestRB.IsChecked())
        nNewLinkMode = MANUAL;

    SwFldUpdateFlags eFldFlags = aAutoUpdateFields.IsChecked() ?
        aAutoUpdateCharts.IsChecked() ? AUTOUPD_FIELD_AND_CHARTS : AUTOUPD_FIELD_ONLY : AUTOUPD_OFF;

    if(aAutoUpdateFields.IsChecked() != aAutoUpdateFields.GetSavedValue() ||
            aAutoUpdateCharts.IsChecked() != aAutoUpdateCharts.GetSavedValue())
    {
        pMod->ApplyFldUpdateFlags(eFldFlags);
        if(pWrtShell)
        {
            pWrtShell->SetFldUpdateFlags(eFldFlags);
            pWrtShell->SetModified();
        }
    }

    if (nNewLinkMode != nOldLinkMode)
    {
        pMod->ApplyLinkMode(nNewLinkMode);
        if (pWrtShell)
        {
            pWrtShell->SetLinkUpdMode( nNewLinkMode );
            pWrtShell->SetModified();
        }

        bRet = TRUE;
    }

    const USHORT nMPos = aMetricLB.GetSelectEntryPos();
    if ( nMPos != aMetricLB.GetSavedValue() )
    {
        // Doppel-Cast fuer VA3.0
        USHORT nFieldUnit = (USHORT)(long)aMetricLB.GetEntryData( nMPos );
        rSet.Put( SfxUInt16Item( SID_ATTR_METRIC, (UINT16)nFieldUnit ) );
        bRet = TRUE;
    }

    if(aTabMF.IsVisible() && aTabMF.GetText() != aTabMF.GetSavedValue())
    {
        rSet.Put(SfxUInt16Item(SID_ATTR_DEFTABSTOP,
                    (USHORT)aTabMF.Denormalize(aTabMF.GetValue(FUNIT_TWIP))));
        bRet = TRUE;
    }
    
    sal_Bool bIsSquaredPageModeFlag = aUseSquaredPageMode.IsChecked();
    if ( bIsSquaredPageModeFlag != aUseSquaredPageMode.GetSavedValue() )
    {
        pMod->ApplyDefaultPageMode( bIsSquaredPageModeFlag );
        if ( pWrtShell )
        {
            SwDoc* pDoc = pWrtShell->GetDoc();
            pDoc->SetDefaultPageMode( bIsSquaredPageModeFlag );
            pWrtShell->SetModified();
        }
        bRet = TRUE;
    }

    return bRet;
}
/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/
void __EXPORT SwLoadOptPage::Reset( const SfxItemSet& rSet)
{
    const SwMasterUsrPref* pUsrPref = SW_MOD()->GetUsrPref(FALSE);
    const SfxPoolItem* pItem;

    if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_WRTSHELL, FALSE, &pItem))
        pWrtShell = (SwWrtShell*)((const SwPtrItem*)pItem)->GetValue();

    SwFldUpdateFlags eFldFlags = AUTOUPD_GLOBALSETTING;
    nOldLinkMode = GLOBALSETTING;
    if (pWrtShell)
    {
        eFldFlags = pWrtShell->GetFldUpdateFlags(TRUE);
        nOldLinkMode = pWrtShell->GetLinkUpdMode(TRUE);
    }
    if(GLOBALSETTING == nOldLinkMode)
        nOldLinkMode = pUsrPref->GetUpdateLinkMode();
    if(AUTOUPD_GLOBALSETTING == eFldFlags)
        eFldFlags = pUsrPref->GetFldUpdateFlags();

    aAutoUpdateFields.Check(eFldFlags != AUTOUPD_OFF);
    aAutoUpdateCharts.Check(eFldFlags == AUTOUPD_FIELD_AND_CHARTS);

    switch (nOldLinkMode)
    {
        case NEVER:		aNeverRB.Check();	break;
        case MANUAL:	aRequestRB.Check();	break;
        case AUTOMATIC:	aAlwaysRB.Check();	break;
    }

    aAutoUpdateFields.SaveValue();
    aAutoUpdateCharts.SaveValue();
    aMetricLB.SetNoSelection();
    if ( rSet.GetItemState( SID_ATTR_METRIC ) >= SFX_ITEM_AVAILABLE )
    {
        const SfxUInt16Item& rItem = (SfxUInt16Item&)rSet.Get( SID_ATTR_METRIC );
        FieldUnit eFieldUnit = (FieldUnit)rItem.GetValue();

        for ( USHORT i = 0; i < aMetricLB.GetEntryCount(); ++i )
        {
            if ( (int)(sal_IntPtr)aMetricLB.GetEntryData( i ) == (int)eFieldUnit )
            {
                aMetricLB.SelectEntryPos( i );
                break;
            }
        }
        ::SetFieldUnit(aTabMF, eFieldUnit);
    }
    aMetricLB.SaveValue();
    if(SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_DEFTABSTOP, FALSE, &pItem))
    {
        nLastTab = ((SfxUInt16Item*)pItem)->GetValue();
        aTabMF.SetValue(aTabMF.Normalize(nLastTab), FUNIT_TWIP);
    }
    aTabMF.SaveValue();

    if(SFX_ITEM_SET == rSet.GetItemState(SID_HTML_MODE, FALSE, &pItem))
    {
        bHTMLMode = 0 != (((const SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON);
    }
    
    //default page mode loading
    if(pWrtShell)
    {
        sal_Bool bSquaredPageMode = pWrtShell->GetDoc()->IsSquaredPageMode();
        aUseSquaredPageMode.Check( bSquaredPageMode );
            aUseSquaredPageMode.SaveValue();
    }
}
/*-----------------13.01.97 14.44-------------------
    Metric des Deftabstops umschalten
--------------------------------------------------*/

IMPL_LINK(SwLoadOptPage, MetricHdl, ListBox*, EMPTYARG)
{
    const USHORT nMPos = aMetricLB.GetSelectEntryPos();
    if(nMPos != USHRT_MAX)
    {
        // Doppel-Cast fuer VA3.0
        FieldUnit eFieldUnit = (FieldUnit)(long)aMetricLB.GetEntryData( nMPos );
        BOOL bModified = aTabMF.IsModified();
        long nVal = bModified ?
            sal::static_int_cast<sal_Int32, sal_Int64 >( aTabMF.Denormalize( aTabMF.GetValue( FUNIT_TWIP ) )) :
                nLastTab;
        ::SetFieldUnit( aTabMF, eFieldUnit );
        aTabMF.SetValue( aTabMF.Normalize( nVal ), FUNIT_TWIP );
        if(!bModified)
            aTabMF.ClearModifyFlag();
    }

    return 0;
}
/*********************************************************************/
/*                                                                   */
/*********************************************************************/

IMPL_LINK(SwLoadOptPage, CaptionHdl, PushButton*, EMPTYARG)
{
    SwCaptionOptDlg aDlg(this, GetItemSet());
    aDlg.Execute();

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwCaptionOptDlg::SwCaptionOptDlg(Window* pParent, const SfxItemSet& rSet) :
    SfxSingleTabDialog(pParent, rSet, 0)
{
    // TabPage erzeugen
    SetTabPage((SwCaptionOptPage*) SwCaptionOptPage::Create(this, rSet));
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwCaptionOptDlg::~SwCaptionOptDlg()
{
}

/* -----------------22.10.98 15:12-------------------
 *
 * --------------------------------------------------*/

SwCaptionPreview::SwCaptionPreview( Window* pParent, const ResId& rResId )
    : Window( pParent, rResId )
{
    maDrawPos = Point( 4, 6 );

    Wallpaper	aBack( GetSettings().GetStyleSettings().GetWindowColor() );
    SetBackground( aBack );
    SetFillColor( aBack.GetColor() );
    SetLineColor( aBack.GetColor() );
    SetBorderStyle( WINDOW_BORDER_MONO );
    Font aFont(GetFont());
    aFont.SetHeight(aFont.GetHeight() * 120 / 100 );
    SetFont(aFont);
}

void SwCaptionPreview::SetPreviewText( const String& rText )
{
    if( rText != maText )
    {
        maText = rText;
        Invalidate();
    }
}

void SwCaptionPreview::Paint( const Rectangle& rRect )
{
    Window::Paint( rRect );

    DrawRect( Rectangle( Point( 0, 0 ), GetSizePixel() ) );
    DrawText( Point( 4, 6 ), maText );
}


SwCaptionOptPage::SwCaptionOptPage( Window* pParent, const SfxItemSet& rSet )
    : SfxTabPage(pParent, SW_RES(TP_OPTCAPTION_PAGE), rSet),
    aCheckFT		(this, SW_RES(FT_OBJECTS	)),
    aCheckLB		(this, SW_RES(CLB_OBJECTS	)),
    aFtCaptionOrder(this, SW_RES( FT_ORDER )),
    aLbCaptionOrder(this, SW_RES( LB_ORDER )),
    aPreview        (this, SW_RES(WIN_PREVIEW   )),
    aSettingsGroupFL(this, SW_RES(FL_SETTINGS_2	)),
    aCategoryText	(this, SW_RES(TXT_CATEGORY	)),
    aCategoryBox 	(this, SW_RES(BOX_CATEGORY	)),
    aFormatText		(this, SW_RES(TXT_FORMAT	)),
    aFormatBox		(this, SW_RES(BOX_FORMAT	)),
    aNumberingSeparatorFT(this, SW_RES(FT_NUM_SEP  )),
    aNumberingSeparatorED(this, SW_RES(ED_NUM_SEP  )),
    aTextText		(this, SW_RES(TXT_TEXT		)),
    aTextEdit		(this, SW_RES(EDT_TEXT		)),
    aPosText		(this, SW_RES(TXT_POS		)),
    aPosBox			(this, SW_RES(BOX_POS		)),
    aNumCaptFL		(this, SW_RES(FL_NUMCAPT	)),
    aFtLevel		(this, SW_RES(FT_LEVEL		)),
    aLbLevel		(this, SW_RES(LB_LEVEL		)),
    aFtDelim		(this, SW_RES(FT_SEPARATOR	)),
    aEdDelim		(this, SW_RES(ED_SEPARATOR	)),
    aCategoryFL		(this, SW_RES(FL_CATEGORY	)),
    aCharStyleFT	(this, SW_RES(FT_CHARSTYLE	)),
    aCharStyleLB	(this, SW_RES(LB_CHARSTYLE	)),
    aApplyBorderCB	(this, SW_RES(CB_APPLYBORDER)),

    sSWTable    	(SW_RES(STR_TABLE			)),
    sSWFrame    	(SW_RES(STR_FRAME			)),
    sSWGraphic   	(SW_RES(STR_GRAPHIC			)),
    sOLE    		(SW_RES(STR_OLE				)),

    sBegin			(SW_RESSTR(STR_BEGINNING			)),
    sEnd			(SW_RESSTR(STR_END					)),
    sAbove			(SW_RESSTR(STR_ABOVE				)),
    sBelow			(SW_RESSTR(STR_CP_BELOW				)),
    sNone           (SW_RESSTR( STR_CATEGORY_NONE )),

    pMgr			(new SwFldMgr()),
    bHTMLMode(FALSE)
{
    Wallpaper	aBack( GetSettings().GetStyleSettings().GetWindowColor() );
    aPreview.SetBackground( aBack );

    SwStyleNameMapper::FillUIName( RES_POOLCOLL_LABEL_ABB, sIllustration );
    SwStyleNameMapper::FillUIName( RES_POOLCOLL_LABEL_TABLE, sTable );
    SwStyleNameMapper::FillUIName( RES_POOLCOLL_LABEL_FRAME, sText );
    SwStyleNameMapper::FillUIName( RES_POOLCOLL_LABEL_DRAWING, sDrawing );

    USHORT i, nCount;
    SwWrtShell *pSh = ::GetActiveWrtShell();

    // aFormatBox
    USHORT nSelFmt = SVX_NUM_ARABIC;
    if (pSh)
    {
        nCount = pMgr->GetFldTypeCount();
        SwFieldType* pFldType;
        for ( i = nCount; i; )
            if( ( pFldType = pMgr->GetFldType(USHRT_MAX, --i))->GetName() ==
                aCategoryBox.GetText() )
            {
                nSelFmt = (USHORT)((SwSetExpFieldType*)pFldType)->GetSeqFormat();
                break;
            }

        ::FillCharStyleListBox( aCharStyleLB, pSh->GetView().GetDocShell(), TRUE, TRUE );
    }


    nCount = pMgr->GetFormatCount(TYP_SEQFLD, FALSE);
    for ( i = 0; i < nCount; ++i )
    {
        aFormatBox.InsertEntry( pMgr->GetFormatStr(TYP_SEQFLD, i) );
        USHORT nFmtId = pMgr->GetFormatId(TYP_SEQFLD, i);
        aFormatBox.SetEntryData( i, reinterpret_cast<void*>(nFmtId) );
        if( nFmtId == nSelFmt )
            aFormatBox.SelectEntryPos( i );
    }

    for (i = 0; i < MAXLEVEL; i++)
        aLbLevel.InsertEntry(String::CreateFromInt32(i + 1));

    sal_Unicode nLvl = MAXLEVEL;
    String	sDelim( String::CreateFromAscii( ": " ) );

    if (pSh)
    {
        SwSetExpFieldType* pFldType = (SwSetExpFieldType*)pMgr->GetFldType(
                                            RES_SETEXPFLD, aCategoryBox.GetText() );
        if( pFldType )
        {
            sDelim = pFldType->GetDelimiter();
            nLvl = pFldType->GetOutlineLvl();
        }
    }

    aLbLevel.SelectEntryPos( nLvl < MAXLEVEL ? nLvl + 1 : 0 );
    aEdDelim.SetText( sDelim );

    aCheckLB.SetHelpId(HID_OPTCAPTION_CLB);

    FreeResource();

    Link aLk = LINK( this, SwCaptionOptPage, ModifyHdl );
    aCategoryBox.SetModifyHdl( aLk );
    aNumberingSeparatorED.SetModifyHdl( aLk );
    aTextEdit	.SetModifyHdl( aLk );

    aLk = LINK(this, SwCaptionOptPage, SelectHdl);
    aCategoryBox.SetSelectHdl( aLk );
    aFormatBox	.SetSelectHdl( aLk );

    aLbCaptionOrder.SetSelectHdl( LINK(this, SwCaptionOptPage, OrderHdl));

    aCheckLB.SetSelectHdl( LINK(this, SwCaptionOptPage, ShowEntryHdl) );
    aCheckLB.SetCheckButtonHdl( LINK(this, SwCaptionOptPage, ShowEntryHdl) );
    aCheckLB.SetDeselectHdl( LINK(this, SwCaptionOptPage, SaveEntryHdl) );
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SwCaptionOptPage::~SwCaptionOptPage()
{
    DelUserData();
    delete pMgr;
}

/*-----------------18.01.97 12.43-------------------

--------------------------------------------------*/

SfxTabPage*	SwCaptionOptPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new SwCaptionOptPage(pParent, rAttrSet );
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

BOOL SwCaptionOptPage::FillItemSet( SfxItemSet&  )
{
    BOOL bRet = FALSE;
    SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();

    SaveEntry(aCheckLB.FirstSelected());	// Aktuellen Eintrag uebernehmen

    SvLBoxEntry* pEntry = aCheckLB.First();

    while (pEntry)
    {
        InsCaptionOpt* pData = (InsCaptionOpt*)pEntry->GetUserData();
        bRet |= pModOpt->SetCapOption(bHTMLMode, pData);
        pEntry = aCheckLB.Next(pEntry);
    }

    USHORT nCheckCount = aCheckLB.GetCheckedEntryCount();
    pModOpt->SetInsWithCaption( bHTMLMode, nCheckCount > 0 );

    sal_Int32 nPos = aLbCaptionOrder.GetSelectEntryPos();
    pModOpt->SetCaptionOrderNumberingFirst(nPos == 1 ? sal_True : sal_False );

    return bRet;
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

void SwCaptionOptPage::Reset( const SfxItemSet& rSet)
{
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState(SID_HTML_MODE, FALSE, &pItem))
    {
        bHTMLMode = 0 != (((const SfxUInt16Item*)pItem)->GetValue() & HTMLMODE_ON);
    }

    DelUserData();
    aCheckLB.GetModel()->Clear();   // remove all entries

    // Writer objects
    USHORT nPos = 0;
    aCheckLB.InsertEntry(sSWTable);
    SetOptions(nPos++, TABLE_CAP);
    aCheckLB.InsertEntry(sSWFrame);
    SetOptions(nPos++, FRAME_CAP);
    aCheckLB.InsertEntry(sSWGraphic);
    SetOptions(nPos++, GRAPHIC_CAP);

    // get Productname and -version
    String sComplete, sWithoutVersion;
    ::rtl::OUString sTemp;
    uno::Any aAny =
        ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
    if ( aAny >>= sTemp )
    {
        sComplete = sTemp;
        sWithoutVersion = sTemp;
        aAny = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTVERSION );
        if ( !( aAny >>= sTemp ) )
        {
            DBG_ERRORFILE( "Couldn't get PRODUCTVERSION variable" );
        }
        else
        {
            sComplete += ' ';
            sComplete += String( sTemp );
        }
    }
    else
    {
        DBG_ERRORFILE( "Couldn't get PRODUCTNAME variable" );
    }

    SvObjectServerList aObjS;
    aObjS.FillInsertObjects();
    aObjS.Remove( SvGlobalName( SO3_SW_CLASSID ) ); // remove Writer-ID

    for ( ULONG i = 0; i < aObjS.Count(); ++i )
    {
        const SvGlobalName &rOleId = aObjS[i].GetClassName();
        const String* pClassName = &aObjS[i].GetHumanName();
        if ( rOleId == SvGlobalName( SO3_OUT_CLASSID ) )
            pClassName = &sOLE;
        String sClass( *pClassName );
        // don't show product version
        sClass.SearchAndReplace( sComplete, sWithoutVersion );
        aCheckLB.InsertEntry( sClass );
        SetOptions( nPos++, OLE_CAP, &rOleId );
    }
    aLbCaptionOrder.SelectEntryPos(
        SW_MOD()->GetModuleConfig()->IsCaptionOrderNumberingFirst() ? 1 : 0);
    ModifyHdl();
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

void SwCaptionOptPage::SetOptions(const USHORT nPos,
        const SwCapObjType eObjType, const SvGlobalName *pOleId)
{
    SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
    const InsCaptionOpt* pOpt = pModOpt->GetCapOption(bHTMLMode, eObjType, pOleId);

    if (pOpt)
    {
        aCheckLB.SetEntryData(nPos, new InsCaptionOpt(*pOpt));
        aCheckLB.CheckEntryPos(nPos, pOpt->UseCaption());
    }
    else
        aCheckLB.SetEntryData(nPos, new InsCaptionOpt(eObjType, pOleId));
}

/*-----------------18.01.97 12.42-------------------

--------------------------------------------------*/

void SwCaptionOptPage::DelUserData()
{
    SvLBoxEntry* pEntry = aCheckLB.First();

    while (pEntry)
    {
        delete (InsCaptionOpt*)pEntry->GetUserData();
        pEntry->SetUserData(0);
        pEntry = aCheckLB.Next(pEntry);
    }
}

/* -----------------26.10.98 11:06-------------------
 *
 * --------------------------------------------------*/

IMPL_LINK( SwCaptionOptPage, ShowEntryHdl, SvxCheckListBox *, EMPTYARG )
{
    SvLBoxEntry* pSelEntry = aCheckLB.FirstSelected();

    if (pSelEntry)
    {
        sal_Bool bChecked = aCheckLB.IsChecked((USHORT)aCheckLB.GetModel()->GetAbsPos(pSelEntry));

        aSettingsGroupFL.Enable( bChecked );
        aCategoryText.Enable( bChecked );
        aCategoryBox.Enable( bChecked );
        aFormatText.Enable( bChecked );
        aFormatBox.Enable( bChecked );
        sal_Bool bNumSep = bChecked && aLbCaptionOrder.GetSelectEntryPos() == 1;
        aNumberingSeparatorED.Enable( bNumSep );
        aNumberingSeparatorFT.Enable( bNumSep );
        aTextText.Enable( bChecked );
        aTextEdit.Enable( bChecked );
        aPosText.Enable( bChecked );
        aPosBox.Enable( bChecked );
        aNumCaptFL.Enable( bChecked );
        aFtLevel.Enable( bChecked );
        aLbLevel.Enable( bChecked );
        aFtDelim.Enable( bChecked );
        aEdDelim.Enable( bChecked );
        aCategoryFL.Enable( bChecked );
        aCharStyleFT.Enable( bChecked );
        aCharStyleLB.Enable( bChecked );
        aApplyBorderCB.Enable( bChecked );
        aPreview.Enable( bChecked );

        SwWrtShell *pSh = ::GetActiveWrtShell();

        InsCaptionOpt* pOpt = (InsCaptionOpt*)pSelEntry->GetUserData();

        aCategoryBox.Clear();
        aCategoryBox.InsertEntry( sNone );
        if (pSh)
        {
            USHORT nCount = pMgr->GetFldTypeCount();

            for (USHORT i = 0; i < nCount; i++)
            {
                SwFieldType *pType = pMgr->GetFldType( USHRT_MAX, i );
                if( pType->Which() == RES_SETEXPFLD &&
                    ((SwSetExpFieldType *) pType)->GetType() & nsSwGetSetExpType::GSE_SEQ )
                    aCategoryBox.InsertEntry(SwBoxEntry(pType->GetName()));
            }
        }
        else
        {
            aCategoryBox.InsertEntry(SwBoxEntry(sIllustration));
            aCategoryBox.InsertEntry(SwBoxEntry(sTable));
            aCategoryBox.InsertEntry(SwBoxEntry(sText));
            aCategoryBox.InsertEntry(SwBoxEntry(sDrawing));
        }

        if(pOpt->GetCategory().Len())
            aCategoryBox.SetText(pOpt->GetCategory());
        else
            aCategoryBox.SetText( sNone );
        if (pOpt->GetCategory().Len() &&
            aCategoryBox.GetEntryPos(pOpt->GetCategory()) == COMBOBOX_ENTRY_NOTFOUND)
            aCategoryBox.InsertEntry(pOpt->GetCategory());
        if (!aCategoryBox.GetText().Len())
        {
            USHORT nPos = 0;
            switch(pOpt->GetObjType())
            {
                case OLE_CAP:
                case GRAPHIC_CAP:       nPos = 1;   break;
                case TABLE_CAP:         nPos = 2;   break;
                case FRAME_CAP:         nPos = 3;   break;
            }
            aCategoryBox.SetText(aCategoryBox.GetEntry(nPos).GetName());
        }

        for (USHORT i = 0; i < aFormatBox.GetEntryCount(); i++)
        {
            if (pOpt->GetNumType() == (USHORT)(ULONG)aFormatBox.GetEntryData(i))
            {
                aFormatBox.SelectEntryPos(i);
                break;
            }
        }
        aTextEdit.SetText(pOpt->GetCaption());

        aPosBox.Clear();
        switch (pOpt->GetObjType())
        {
            case GRAPHIC_CAP:
            case TABLE_CAP:
            case OLE_CAP:
                aPosBox.InsertEntry(sAbove);
                aPosBox.InsertEntry(sBelow);
                break;
            case FRAME_CAP:
                aPosBox.InsertEntry(sBegin);
                aPosBox.InsertEntry(sEnd);
                break;
        }
        aPosBox.SelectEntryPos(pOpt->GetPos());
        aPosBox.Enable( pOpt->GetObjType() != GRAPHIC_CAP && aPosText.IsEnabled() );
        aPosBox.SelectEntryPos(pOpt->GetPos());

        USHORT nLevelPos = ( pOpt->GetLevel() < MAXLEVEL ) ? pOpt->GetLevel() + 1 : 0;
        aLbLevel.SelectEntryPos( nLevelPos );
        aEdDelim.SetText(pOpt->GetSeparator());
        aNumberingSeparatorED.SetText( pOpt->GetNumSeparator() );
        if(pOpt->GetCharacterStyle().Len())
            aCharStyleLB.SelectEntry( pOpt->GetCharacterStyle() );
        else
            aCharStyleLB.SelectEntryPos( 0 );
        aApplyBorderCB.Enable( aCategoryBox.IsEnabled() &&
                pOpt->GetObjType() != TABLE_CAP && pOpt->GetObjType() != FRAME_CAP );
        aApplyBorderCB.Check( pOpt->CopyAttributes() );
    }

    ModifyHdl();

    return 0;
}

/* -----------------26.10.98 11:06-------------------
 *
 * --------------------------------------------------*/

IMPL_LINK( SwCaptionOptPage, SaveEntryHdl, SvxCheckListBox *, EMPTYARG )
{
    SvLBoxEntry* pEntry = aCheckLB.GetHdlEntry();

    if (pEntry)		// Alles speichern
        SaveEntry(pEntry);

    return 0;
}

/* -----------------05.11.98 16:23-------------------
 *
 * --------------------------------------------------*/

void SwCaptionOptPage::SaveEntry(SvLBoxEntry* pEntry)
{
    if (pEntry)
    {
        InsCaptionOpt* pOpt = (InsCaptionOpt*)pEntry->GetUserData();

        pOpt->UseCaption() = aCheckLB.IsChecked((USHORT)aCheckLB.GetModel()->GetAbsPos(pEntry));
        String aName( aCategoryBox.GetText() );
        if(aName == sNone)
            pOpt->SetCategory(aEmptyStr);
        else
        {
            aName.EraseLeadingChars (' ');
            aName.EraseTrailingChars(' ');
            pOpt->SetCategory(aName);
        }
        pOpt->SetNumType((USHORT)(ULONG)aFormatBox.GetEntryData(aFormatBox.GetSelectEntryPos()));
        pOpt->SetCaption(aTextEdit.IsEnabled() ? aTextEdit.GetText() : aEmptyStr );
        pOpt->SetPos(aPosBox.GetSelectEntryPos());
        USHORT nPos = aLbLevel.GetSelectEntryPos();
        USHORT nLevel = ( nPos > 0 && nPos != LISTBOX_ENTRY_NOTFOUND ) ? nPos - 1 : MAXLEVEL;
        pOpt->SetLevel(nLevel);
        pOpt->SetSeparator(aEdDelim.GetText());
        pOpt->SetNumSeparator( aNumberingSeparatorED.GetText());
        if(!aCharStyleLB.GetSelectEntryPos())
            pOpt->SetCharacterStyle(aEmptyStr);
        else
            pOpt->SetCharacterStyle(aCharStyleLB.GetSelectEntry());
        pOpt->CopyAttributes() = aApplyBorderCB.IsChecked();
    }
}

/* -----------------26.10.98 11:06-------------------
 *
 * --------------------------------------------------*/

IMPL_LINK( SwCaptionOptPage, ModifyHdl, Edit *, EMPTYARG )
{
    String sFldTypeName = aCategoryBox.GetText();
/*
    SwFieldType* pType = sFldTypeName.Len() ? pMgr->GetFldType(RES_SETEXPFLD, sFldTypeName) : 0;

    ((SfxSingleTabDialog*)GetParent())->GetOKButton()->Enable( (!pType || ((SwSetExpFieldType*)pType)->GetType() == GSE_SEQ)
                                    && sFldTypeName.Len() != 0 );
*/
    SfxSingleTabDialog *pDlg = (SfxSingleTabDialog *)GetParent();
    PushButton *pBtn = pDlg->GetOKButton();
    if (pBtn)
        pBtn->Enable(sFldTypeName.Len() != 0);
    sal_Bool bEnable = aCategoryBox.IsEnabled() && sFldTypeName != sNone;

    aFormatText.Enable(bEnable);
    aFormatBox.Enable(bEnable);
    aTextText.Enable(bEnable);
    aTextEdit.Enable(bEnable);

    DrawSample();
    return 0;
}

/* -----------------26.10.98 10:58-------------------
 *
 * --------------------------------------------------*/

IMPL_LINK_INLINE_START( SwCaptionOptPage, SelectHdl, ListBox *, EMPTYARG )
{
    DrawSample();
    return 0;
}
IMPL_LINK_INLINE_END( SwCaptionOptPage, SelectHdl, ListBox *, EMPTYARG )
/*-- 02.11.2007 10:00:36---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwCaptionOptPage, OrderHdl, ListBox*, pBox )
{
    DrawSample();
    sal_Int32 nPos = pBox->GetSelectEntryPos();
    aNumberingSeparatorFT.Enable( nPos == 1 );
    aNumberingSeparatorED.Enable( nPos == 1 );
    return 0;
}
/* -----------------26.10.98 10:58-------------------
 *
 * --------------------------------------------------*/

void SwCaptionOptPage::DrawSample()
{
    String aStr;

    if( aCategoryBox.GetText() != sNone)
    {
        //#i61007# order of captions
        bool bOrderNumberingFirst = aLbCaptionOrder.GetSelectEntryPos() == 1;
        // Nummer
        USHORT nNumFmt = (USHORT)(ULONG)aFormatBox.GetEntryData(
                                        aFormatBox.GetSelectEntryPos() );
        if( SVX_NUM_NUMBER_NONE != nNumFmt )
        {
            //#i61007# order of captions
            if( !bOrderNumberingFirst )
            {
                // Kategorie
                aStr += aCategoryBox.GetText();
                aStr += ' ';
            }

            SwWrtShell *pSh = ::GetActiveWrtShell();
            String sFldTypeName( aCategoryBox.GetText() );
            if (pSh)
            {
                SwSetExpFieldType* pFldType = (SwSetExpFieldType*)pMgr->GetFldType(
                                                RES_SETEXPFLD, sFldTypeName );
                if( pFldType && pFldType->GetOutlineLvl() < MAXLEVEL )
                {
                    BYTE nLvl = pFldType->GetOutlineLvl();
                    SwNodeNum::tNumberVector aNumVector;
                    for( BYTE i = 0; i <= nLvl; ++i )
                        aNumVector.push_back(1);

                    String sNumber( pSh->GetOutlineNumRule()->MakeNumString(
                                                            aNumVector, FALSE ));
                    if( sNumber.Len() )
                        (aStr += sNumber) += pFldType->GetDelimiter();
                }
            }

            switch( nNumFmt )
            {
                case SVX_NUM_CHARS_UPPER_LETTER:    aStr += 'A'; break;
                case SVX_NUM_CHARS_UPPER_LETTER_N:  aStr += 'A'; break;
                case SVX_NUM_CHARS_LOWER_LETTER:    aStr += 'a'; break;
                case SVX_NUM_CHARS_LOWER_LETTER_N:  aStr += 'a'; break;
                case SVX_NUM_ROMAN_UPPER:           aStr += 'I'; break;
                case SVX_NUM_ROMAN_LOWER:           aStr += 'i'; break;
                //case ARABIC:
                default:                    aStr += '1'; break;
            }
        }
        //#i61007# order of captions
        if( bOrderNumberingFirst )
        {
            aStr += aNumberingSeparatorED.GetText();
            aStr += aCategoryBox.GetText();
        }
        aStr += aTextEdit.GetText();
    }
    aPreview.SetPreviewText( aStr );
}

/*------------------------------------------------------------------------
 Beschreibung:	ComboBox ohne Spaces
------------------------------------------------------------------------*/

void CaptionComboBox::KeyInput(const KeyEvent& rEvt)
{
    if( rEvt.GetKeyCode().GetCode() != KEY_SPACE )
        SwComboBox::KeyInput(rEvt);
}



