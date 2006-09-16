/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swuiidxmrk.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:04:33 $
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

#include "swuiidxmrk.hxx"

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif
#define _SVSTDARR_STRINGSSORT
#include <svtools/svstdarr.hxx>

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_SEARCHOPTIONS_HPP_
#include <com/sun/star/util/SearchOptions.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_SEARCHFLAGS_HPP_
#include <com/sun/star/util/SearchFlags.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_TRANSLITERATIONMODULES_HPP_
#include <com/sun/star/i18n/TransliterationModules.hpp>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _TXTCMP_HXX //autogen
#include <svtools/txtcmp.hxx>
#endif
#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <svx/scripttypeitem.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include "svx/unolingu.hxx"
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _IDXMRK_HXX
#include <idxmrk.hxx>
#endif
#ifndef _TXTTXMRK_HXX
#include <txttxmrk.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _MULTMRK_HXX
#include <multmrk.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>               	// fuer Undo-Ids
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _INDEX_HRC
#include <index.hrc>
#endif
#ifndef _IDXMRK_HRC
#include <idxmrk.hrc>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#include <utlui.hrc>
#ifndef _SWCONT_HXX
#include <swcont.hxx>
#endif
#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _SW_REWRITER_HXX
#include <SwRewriter.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif

#include "swuiidxmrk.hxx"

#define POS_CONTENT 0
#define POS_INDEX   1
#define POS_USER    2

static sal_uInt16 nTypePos = 1; // TOX_INDEX as standard
static sal_uInt16 nKey1Pos = USHRT_MAX;

static sal_uInt16 nKey2Pos = USHRT_MAX;

static const char*	pUserItemNamePosX = "POSX";
static const char*	pUserItemNamePosY = "POSY";

using namespace com::sun::star;
using namespace com::sun::star::i18n;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::i18n;
using namespace ::rtl;
using namespace ::comphelper;
using namespace ::com::sun::star;

#define C2U(cChar) OUString::createFromAscii(cChar)

/*--------------------------------------------------------------------
     Beschreibung:	Dialog zum Einfuegen einer Verzeichnismarkierung
 --------------------------------------------------------------------*/
SwIndexMarkDlg::SwIndexMarkDlg(Window *pParent,
                               sal_Bool bNewDlg,
                               const ResId& rResId,
                               sal_Int32 _nOptionsId ) :
      Window(pParent, rResId),
    nOptionsId( _nOptionsId ),
    bDel(sal_False),
    bNewMark(bNewDlg),
    pTOXMgr(0),
    pSh(0),
    aOKBT(this, 	SW_RES(BT_OK 	)),
    aCancelBT(this, SW_RES(BT_CANCEL )),
    aHelpBT(this, 	SW_RES(BT_HELP   )),
    aDelBT(this, 	SW_RES(BT_DEL 	)),
    aNewBT(this, 	SW_RES(BT_NEW 	)),
    aPrevBT(this, 	SW_RES(BT_PREV 	)),
    aPrevSameBT(this,SW_RES(BT_PREVSAME)),
    aNextBT(this, 	SW_RES(BT_NXT 	)),
    aNextSameBT(this,SW_RES(BT_NXTSAME)),
    aTypeFT	(this, 	SW_RES(LBL_INDEX	)),
    aTypeDCB(this, 	SW_RES(DCB_INDEX	)),
    aEntryFT(this, 	SW_RES(LBL_ENTRY	)),
    aEntryED(this, 	SW_RES(SL_ENTRY	)),
    aPhoneticFT0(this,  SW_RES(FT_PHONETIC_1 )),
    aPhoneticED0(this,  SW_RES(ED_PHONETIC_1 )),
    aKeyFT(this, 	SW_RES(LBL_KEY	)),
    aKeyDCB(this, 	SW_RES(DCB_KEY	)),
    aPhoneticFT1(this,  SW_RES(FT_PHONETIC_2 )),
    aPhoneticED1(this,  SW_RES(ED_PHONETIC_2 )),
    aKey2FT(this, 	SW_RES(LBL_KEY2	)),
    aKey2DCB(this, 	SW_RES(DCB_KEY2	)),
    aPhoneticFT2(this,  SW_RES(FT_PHONETIC_3 )),
    aPhoneticED2(this,  SW_RES(ED_PHONETIC_3 )),
    aLevelFT(this, 	SW_RES(LBL_LEVEL	)),
    aLevelED(this, 	SW_RES(SL_LEVEL	)),
    aMainEntryCB(this, SW_RES(CB_MAIN_ENTRY	)),
    aApplyToAllCB(this,SW_RES(CB_APPLY_TO_ALL)),
    aSearchCaseSensitiveCB(this,	SW_RES(CB_CASESENSITIVE	)),
    aSearchCaseWordOnlyCB(this,		SW_RES(CB_WORDONLY		)),
    aIndexFL(this,  SW_RES(FL_INDEX )),
    bSelected(sal_False),
    bPhoneticED0_ChangedByUser(FALSE),
    bPhoneticED1_ChangedByUser(FALSE),
    bPhoneticED2_ChangedByUser(FALSE),
    xExtendedIndexEntrySupplier(NULL),
    nLangForPhoneticReading(2052),
    bIsPhoneticReadingEnabled(FALSE)
{
    if( SvtCJKOptions().IsCJKFontEnabled() )
    {
        uno::Reference< lang::XMultiServiceFactory > xMSF = getProcessServiceFactory();

        xExtendedIndexEntrySupplier =
            uno::Reference< com::sun::star::i18n::XExtendedIndexEntrySupplier > (
                    xMSF->createInstance( C2U("com.sun.star.i18n.IndexEntrySupplier") ),
                                                                        uno::UNO_QUERY );
    }

    SetStyle(GetStyle()|WB_DIALOGCONTROL);
    FreeResource();
    aOKBT			.SetHelpId(HID_INSERT_IDX_MRK_OK   		);
    aCancelBT		.SetHelpId(HID_INSERT_IDX_MRK_CLOSE		);
    aDelBT      	.SetHelpId(HID_INSERT_IDX_MRK_DELETE	);
    aNewBT      	.SetHelpId(HID_INSERT_IDX_MRK_NEW		);
    aPrevBT     	.SetHelpId(HID_INSERT_IDX_MRK_PREV		);
    aPrevSameBT 	.SetHelpId(HID_INSERT_IDX_MRK_PREV_SAME	);
    aNextBT     	.SetHelpId(HID_INSERT_IDX_MRK_NEXT		);
    aNextSameBT 	.SetHelpId(HID_INSERT_IDX_MRK_NEXT_SAME	);
    aTypeDCB    	.SetHelpId(HID_INSERT_IDX_MRK_TYPE		);
    aEntryED    	.SetHelpId(HID_INSERT_IDX_MRK_ENTRY		);
    aKeyDCB     	.SetHelpId(HID_INSERT_IDX_MRK_PRIM_KEY	);
    aKey2DCB    	.SetHelpId(HID_INSERT_IDX_MRK_SEC_KEY	);
    aLevelED    	.SetHelpId(HID_INSERT_IDX_MRK_LEVEL		);
    aMainEntryCB	.SetHelpId(HID_INSERT_IDX_MRK_MAIN_ENTRY);
    aApplyToAllCB	.SetHelpId(HID_INSERT_IDX_MRK_APPLY_ALL	);
    aPhoneticED0	.SetHelpId(HID_INSERT_IDX_MRK_PHONETIC_READING );
    aPhoneticED1	.SetHelpId(HID_INSERT_IDX_MRK_PHONETIC_READING );
    aPhoneticED2	.SetHelpId(HID_INSERT_IDX_MRK_PHONETIC_READING );

    aSearchCaseSensitiveCB.SetHelpId(	HID_INSERT_IDX_MRK_SRCH_CASESENSITIVE	);
    aSearchCaseWordOnlyCB.SetHelpId(	HID_INSERT_IDX_MRK_SRCH_WORDONLY		);


    GetParent()->SetText( SW_RESSTR( bNewMark ? STR_IDXMRK_INSERT : STR_IDXMRK_EDIT));

    aDelBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 		DelHdl));
    aPrevBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 		PrevHdl));
    aPrevSameBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 	PrevSameHdl));
    aNextBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 		NextHdl));
    aNextSameBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 	NextSameHdl));
    //aTypeDCB.SetModifyHdl(LINK(this,SwIndexMarkDlg, 	ModifyHdl));
    aTypeDCB.SetSelectHdl(LINK(this,SwIndexMarkDlg, 	ModifyHdl));
    aKeyDCB.SetModifyHdl(LINK(this,SwIndexMarkDlg, 		KeyDCBModifyHdl));
    aKey2DCB.SetModifyHdl(LINK(this,SwIndexMarkDlg, 	KeyDCBModifyHdl));
    aOKBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 		InsertHdl));
    aCancelBT.SetClickHdl(LINK(this,SwIndexMarkDlg, 	CloseHdl));
    aEntryED.SetModifyHdl(LINK(this,SwIndexMarkDlg, 	ModifyHdl));
    aNewBT.SetClickHdl(LINK(this, SwIndexMarkDlg,       NewUserIdxHdl));
    aApplyToAllCB.SetClickHdl(LINK(this, SwIndexMarkDlg, SearchTypeHdl));
    aPhoneticED0.SetModifyHdl(LINK(this,SwIndexMarkDlg, PhoneticEDModifyHdl));
    aPhoneticED1.SetModifyHdl(LINK(this,SwIndexMarkDlg, PhoneticEDModifyHdl));
    aPhoneticED2.SetModifyHdl(LINK(this,SwIndexMarkDlg, PhoneticEDModifyHdl));

    if(bNewMark)
    {
        aDelBT.Hide();

        {
            ImageList aTempList( SW_RES( IMG_NAVI_ENTRYBMPH ) );
            aNewBT.SetModeImage( aTempList.GetImage( SID_SW_START + CONTENT_TYPE_INDEX ), BMP_COLOR_HIGHCONTRAST );
        }

        {
            ImageList aTempList( SW_RES( IMG_NAVI_ENTRYBMP ) );
            aNewBT.SetModeImage( aTempList.GetImage( SID_SW_START + CONTENT_TYPE_INDEX ), BMP_COLOR_NORMAL );
        }
    }
    else
    {
        aNewBT.Hide();
        OKButton aTmp(this, WB_HIDE);
        aOKBT.SetText( aTmp.GetText() );
    }

    aEntryED.GrabFocus();
}

/*--------------------------------------------------------------------
     Beschreibung:	Controls neu initialisieren mit der aktuellen
                    Markierung
 --------------------------------------------------------------------*/


void SwIndexMarkDlg::InitControls()
{
    DBG_ASSERT(pSh && pTOXMgr, "Shell nicht da?")
    // Inhalts-Verzeichnis
    const SwTOXType* pType = pTOXMgr->GetTOXType(TOX_CONTENT, 0);
    ASSERT(pType, "Kein Verzeichnistyp !!");
    String sTmpTypeSelection;
    if(aTypeDCB.GetSelectEntryCount())
        sTmpTypeSelection = aTypeDCB.GetSelectEntry();
    aTypeDCB.Clear();
    aTypeDCB.InsertEntry(pType->GetTypeName());

    // Stichwort-Verzeichnis
    pType = pTOXMgr->GetTOXType(TOX_INDEX, 0);
    ASSERT(pType, "Kein Verzeichnistyp !!");
    aTypeDCB.InsertEntry(pType->GetTypeName());

    // Benutzerverzeichnisse
    sal_uInt16 nCount = pSh->GetTOXTypeCount(TOX_USER);
    sal_uInt16 i;
    for( i = 0; i < nCount; ++i )
        aTypeDCB.InsertEntry( pSh->GetTOXType(TOX_USER, i)->GetTypeName() );

    // KeyWords Primary einlesen
    SvStringsSort aArr;
    nCount = pSh->GetTOIKeys( TOI_PRIMARY, aArr );
    for(i=0; i < nCount; ++i)
        aKeyDCB.InsertEntry( *aArr[ i ] );

    // KeyWords Secondary einlesen
    nCount = pSh->GetTOIKeys( TOI_SECONDARY, aArr );
    for(i=0; i < nCount; ++i)
        aKey2DCB.InsertEntry( *aArr[ i ] );

    UpdateLanguageDependenciesForPhoneticReading();

    // Aktueller Eintrag
    const SwTOXMark* pMark = pTOXMgr->GetCurTOXMark();
    if( pMark && !bNewMark)
    {
        // Controls-Handling

        // nur wenn mehr als eins vorhanden
        // wenn gleich landet es auf dem gleichen Eintrag
        pSh->SttCrsrMove();

        const SwTOXMark* pMoveMark;
        sal_Bool bShow = sal_False;

        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_PRV );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_NXT ), bShow = sal_True;
        aPrevBT.Enable( pMoveMark != pMark );
        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_NXT );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_PRV ), bShow = sal_True;
        aNextBT.Enable( pMoveMark != pMark );
        if( bShow )
        {
            aPrevBT.Show();
            aNextBT.Show();
            bShow = sal_False;
        }

        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_SAME_PRV );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_SAME_NXT ), bShow = sal_True;
        aPrevSameBT.Enable( pMoveMark != pMark );
        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_SAME_NXT );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_SAME_PRV ), bShow = sal_True;
        aNextSameBT.Enable( pMoveMark != pMark );
        if( bShow )
        {
            aNextSameBT.Show();
            aPrevSameBT.Show();
        }
        pSh->EndCrsrMove();

        aTypeFT.Show();

        aTypeDCB.Enable(sal_False);
        aTypeFT.Enable(sal_False);

        UpdateDialog();
    }
    else
    { 	// aktuelle Selektion (1.Element) anzeigen  ????
        sal_uInt16 nCnt = pSh->GetCrsrCnt();
        if (nCnt < 2)
        {
            bSelected = !pSh->HasSelection();
            aOrgStr = pSh->GetView().GetSelectionTextParam(sal_True, sal_False);
            aEntryED.SetText(aOrgStr);

            //alle gleichen Eintraege aufzunehmen darf nur im Body und auch da nur
            //bei vorhandener einfacher Selektion erlaubt werden
            const sal_uInt16 nFrmType = pSh->GetFrmType(0,sal_True);
            aApplyToAllCB.Show();
            aSearchCaseSensitiveCB.Show();
            aSearchCaseWordOnlyCB.Show();
            aApplyToAllCB.Enable(0 != aOrgStr.Len() &&
                0 == (nFrmType & ( FRMTYPE_HEADER | FRMTYPE_FOOTER | FRMTYPE_FLY_ANY )));
            SearchTypeHdl(&aApplyToAllCB);
        }

        // Verzeichnistyp ist default
        if(	sTmpTypeSelection.Len() &&
            LISTBOX_ENTRY_NOTFOUND != aTypeDCB.GetEntryPos( sTmpTypeSelection ) )
            aTypeDCB.SelectEntry(sTmpTypeSelection);
        else
            aTypeDCB.SelectEntry(aTypeDCB.GetEntry(nTypePos));
        ModifyHdl(&aTypeDCB);
    }
}

void	SwIndexMarkDlg::UpdateLanguageDependenciesForPhoneticReading()
{
    //no phonetic reading if no global cjk support
    if( !xExtendedIndexEntrySupplier.is() )
    {
        bIsPhoneticReadingEnabled = FALSE;
        return;
    }
    bIsPhoneticReadingEnabled = TRUE;

    //get the current language
    if(!bNewMark) //if dialog is opened to iterate existing marks
    {
        ASSERT(pTOXMgr, "need TOXMgr")
        if(!pTOXMgr)
            return;
        SwTOXMark* pMark = pTOXMgr->GetCurTOXMark();
        ASSERT(pMark, "need current SwTOXMark");
        if(!pMark)
            return;
        SwTxtTOXMark* pTxtTOXMark = pMark->GetTxtTOXMark();
        ASSERT(pTxtTOXMark, "need current SwTxtTOXMark");
        if(!pTxtTOXMark)
            return;
        const SwTxtNode* pTxtNode = pTxtTOXMark->GetpTxtNd();
        ASSERT(pTxtNode, "need current SwTxtNode");
        if(!pTxtNode)
            return;
        xub_StrLen nTextIndex = *pTxtTOXMark->GetStart();
        nLangForPhoneticReading = pTxtNode->GetLang( nTextIndex );
    }
    else //if dialog is opened to create a new mark
    {
        USHORT nScriptType = pSh->GetScriptType();
        USHORT nWhich;
        switch(nScriptType)
        {
            case SCRIPTTYPE_ASIAN: nWhich = RES_CHRATR_CJK_LANGUAGE; break;
            case SCRIPTTYPE_COMPLEX:nWhich = RES_CHRATR_CTL_LANGUAGE; break;
            //case SCRIPTTYPE_LATIN:
            default:nWhich = RES_CHRATR_LANGUAGE; break;
        }
        SfxItemSet aLangSet(pSh->GetAttrPool(), nWhich, nWhich);
        pSh->GetAttr(aLangSet);
        nLangForPhoneticReading = ((const SvxLanguageItem&)aLangSet.Get(nWhich)).GetLanguage();
    }

    /*
    //enable phonetic reading dependent on the current language
    {
        lang::Locale aLocale( SvxCreateLocale( LanguageType( nLangForPhoneticReading ) ) );
        bIsPhoneticReadingEnabled = xExtendedIndexEntrySupplier->usePhoneticEntry( aLocale );
    }
    */
}

String	SwIndexMarkDlg::GetDefaultPhoneticReading( const String& rText )
{
    if( !bIsPhoneticReadingEnabled )
        return aEmptyStr;

    lang::Locale aLocale( SvxCreateLocale( LanguageType( nLangForPhoneticReading ) ) );
    return xExtendedIndexEntrySupplier->getPhoneticCandidate(rText, aLocale);
}

/* -----------------07.09.99 10:43-------------------
    Change the content of aEntryED if text is selected
 --------------------------------------------------*/

void	SwIndexMarkDlg::Activate()
{
    // aktuelle Selektion (1.Element) anzeigen  ????
    if(bNewMark)
    {
        sal_uInt16 nCnt = pSh->GetCrsrCnt();
        if (nCnt < 2)
        {
            bSelected = !pSh->HasSelection();
            aOrgStr = pSh->GetView().GetSelectionTextParam(sal_True, sal_False);
            aEntryED.SetText(aOrgStr);

            //alle gleichen Eintraege aufzunehmen darf nur im Body und auch da nur
            //bei vorhandener einfacher Selektion erlaubt werden
            const sal_uInt16 nFrmType = pSh->GetFrmType(0,sal_True);
            aApplyToAllCB.Show();
            aSearchCaseSensitiveCB.Show();
            aSearchCaseWordOnlyCB.Show();
            aApplyToAllCB.Enable(0 != aOrgStr.Len() &&
                0 == (nFrmType & ( FRMTYPE_HEADER | FRMTYPE_FOOTER | FRMTYPE_FLY_ANY )));
            SearchTypeHdl(&aApplyToAllCB);
        }
        ModifyHdl(&aTypeDCB);
    }
}

/*--------------------------------------------------------------------
     Beschreibung:	Ok-Button auswerten
 --------------------------------------------------------------------*/
void SwIndexMarkDlg::Apply()
{
    InsertUpdate();
    if(bSelected)
        pSh->ResetSelect(0, sal_False);
}

/*--------------------------------------------------------------------
    Beschreibung: Aenderungen uebernehmen
 --------------------------------------------------------------------*/


void SwIndexMarkDlg::InsertUpdate()
{
    pSh->StartUndo(bDel ? UNDO_INDEX_ENTRY_DELETE : UNDO_INDEX_ENTRY_INSERT);
    pSh->StartAllAction();

    SwRewriter aRewriter;

    if( bNewMark )
    {
        InsertMark();

        if ( pTOXMgr->GetCurTOXMark())
            aRewriter.AddRule(UNDO_ARG1, pTOXMgr->GetCurTOXMark()->GetText());
    }
    else if( !pSh->HasReadonlySel() )
    {
        if ( pTOXMgr->GetCurTOXMark())
            aRewriter.AddRule(UNDO_ARG1,
                              pTOXMgr->GetCurTOXMark()->GetText());

        if( bDel )
            pTOXMgr->DeleteTOXMark();
        else if( pTOXMgr->GetCurTOXMark() )
            UpdateMark();
    }

    pSh->EndAllAction();
    pSh->EndUndo(bDel ? UNDO_INDEX_ENTRY_DELETE : UNDO_INDEX_ENTRY_INSERT);

    if((nTypePos = aTypeDCB.GetEntryPos(aTypeDCB.GetSelectEntry())) == LISTBOX_ENTRY_NOTFOUND)
        nTypePos = 0;

    nKey1Pos = aKeyDCB.GetEntryPos(aKeyDCB.GetText());
    nKey2Pos = aKey2DCB.GetEntryPos(aKey2DCB.GetText());
}

/*--------------------------------------------------------------------
     Beschreibung:	Marke einfuegen
 --------------------------------------------------------------------*/
static void lcl_SelectSameStrings(SwWrtShell& rSh, BOOL bWordOnly, BOOL bCaseSensitive)
{
    rSh.Push();

    SearchOptions aSearchOpt(
                        SearchAlgorithms_ABSOLUTE,
                        ( bWordOnly ? SearchFlags::NORM_WORD_ONLY : 0 ),
                        rSh.GetSelTxt(), OUString(),
                        SvxCreateLocale( GetAppLanguage() ),
                        0, 0, 0,
                        (bCaseSensitive
                            ? 0
                            : TransliterationModules_IGNORE_CASE) );

    rSh.ClearMark();
    BOOL bCancel;
    ULONG nRet = rSh.Find( aSearchOpt,  DOCPOS_START, DOCPOS_END, bCancel,
                        (FindRanges)(FND_IN_SELALL|FND_IN_BODYONLY), FALSE );
}


void SwIndexMarkDlg::InsertMark()
{
    sal_uInt16 nPos = aTypeDCB.GetEntryPos(aTypeDCB.GetSelectEntry());
    TOXTypes eType = nPos == POS_CONTENT ? TOX_CONTENT :
                        nPos == POS_INDEX ? TOX_INDEX : TOX_USER;

    SwTOXMarkDescription aDesc(eType);

    sal_uInt16 nLevel = (sal_uInt16)aLevelED.Denormalize(aLevelED.GetValue());
    switch(nPos)
    {
        case POS_CONTENT : break;
        case POS_INDEX:  	// Stichwortverzeichnismarke
        {
            UpdateKeyBoxes();
            String  aPrim(aKeyDCB.GetText());
            String  aSec(aKey2DCB.GetText());
            aDesc.SetPrimKey(aPrim);
            aDesc.SetSecKey(aSec);
            aDesc.SetMainEntry(aMainEntryCB.IsChecked());
            aDesc.SetPhoneticReadingOfAltStr(aPhoneticED0.GetText());
            aDesc.SetPhoneticReadingOfPrimKey(aPhoneticED1.GetText());
            aDesc.SetPhoneticReadingOfSecKey(aPhoneticED2.GetText());
        }
        break;
        default:            // Userdefinedverz.-Marke
        {
            String aName(aTypeDCB.GetSelectEntry());
            aDesc.SetTOUName(aName);
        }
    }
    if (aOrgStr != aEntryED.GetText())
        aDesc.SetAltStr(aEntryED.GetText());
    BOOL bApplyAll = aApplyToAllCB.IsChecked();
    BOOL bWordOnly = aSearchCaseWordOnlyCB.IsChecked();
    BOOL bCaseSensitive = aSearchCaseSensitiveCB.IsChecked();

    pSh->StartAllAction();
    // hier muessen alle gleichen Strings selektiert werden
    // damit der Eintrag auf alle gleichen Strings angewandt wird
    if(bApplyAll)
    {
        lcl_SelectSameStrings(*pSh, bWordOnly, bCaseSensitive);
    }
    aDesc.SetLevel(nLevel);
    SwTOXMgr aMgr(pSh);
    aMgr.InsertTOXMark(aDesc);
    if(bApplyAll)
        pSh->Pop(FALSE);

    pSh->EndAllAction();
}

/*--------------------------------------------------------------------
     Beschreibung:	Marke Updaten
 --------------------------------------------------------------------*/


void SwIndexMarkDlg::UpdateMark()
{
    String 	aAltText(aEntryED.GetText());
    String* pAltText = aOrgStr != aEntryED.GetText() ? &aAltText : 0;
    //empty alternative texts are not allowed
    if(pAltText && !pAltText->Len())
        return;

    UpdateKeyBoxes();

    sal_uInt16 nPos = aTypeDCB.GetEntryPos(aTypeDCB.GetSelectEntry());
    TOXTypes eType = TOX_USER;
    if(POS_CONTENT == nPos)
        eType = TOX_CONTENT;
    else if(POS_INDEX == nPos)
        eType = TOX_INDEX;

    SwTOXMarkDescription aDesc(eType);
    aDesc.SetLevel(aLevelED.GetValue());
    if(pAltText)
        aDesc.SetAltStr(*pAltText);

    String  aPrim(aKeyDCB.GetText());
    if(aPrim.Len())
        aDesc.SetPrimKey(aPrim);
    String  aSec(aKey2DCB.GetText());
    if(aSec.Len())
        aDesc.SetSecKey(aSec);

    if(eType == TOX_INDEX)
    {
        aDesc.SetPhoneticReadingOfAltStr(aPhoneticED0.GetText());
        aDesc.SetPhoneticReadingOfPrimKey(aPhoneticED1.GetText());
        aDesc.SetPhoneticReadingOfSecKey(aPhoneticED2.GetText());
    }
    aDesc.SetMainEntry(aMainEntryCB.IsVisible() && aMainEntryCB.IsChecked());
    pTOXMgr->UpdateTOXMark(aDesc);
}

/*--------------------------------------------------------------------
    Beschreibung: Neue Keys eintragen
 --------------------------------------------------------------------*/


void SwIndexMarkDlg::UpdateKeyBoxes()
{
    String aKey(aKeyDCB.GetText());
    sal_uInt16 nPos = aKeyDCB.GetEntryPos(aKey);

    if(nPos == LISTBOX_ENTRY_NOTFOUND && aKey.Len() > 0)
    {	// neuen Key erzeugen
        aKeyDCB.InsertEntry(aKey);
    }

    aKey = aKey2DCB.GetText();
    nPos = aKey2DCB.GetEntryPos(aKey);

    if(nPos == LISTBOX_ENTRY_NOTFOUND && aKey.Len() > 0)
    {	// neuen Key erzeugen
        aKey2DCB.InsertEntry(aKey);
    }
}
/* -----------------13.10.99 15:10-------------------

 --------------------------------------------------*/
class SwNewUserIdxDlg : public ModalDialog
{
    OKButton 		aOKPB;
    CancelButton 	aCancelPB;
    HelpButton 		aHelpPB;
    FixedLine       aNameFL;
    FixedText		aNameFT;
    Edit			aNameED;

    SwIndexMarkDlg*	pDlg;

    DECL_LINK( ModifyHdl, Edit*);

    public:
        SwNewUserIdxDlg(SwIndexMarkDlg* pParent) :
            ModalDialog(pParent, SW_RES(DLG_NEW_USER_IDX)),
            pDlg(pParent),
            aOKPB(this, ResId(		PB_OK		)),
            aCancelPB(this, ResId(	PB_CANCEL	)),
            aHelpPB(this, ResId(	PB_HELP		)),
            aNameFL(this, ResId(    FL_NAME     )),
            aNameFT(this, ResId(	FT_NAME		)),
            aNameED(this, ResId(	ED_NAME		))
            {
                FreeResource();
                aNameED.SetModifyHdl(LINK(this, SwNewUserIdxDlg, ModifyHdl));
                aOKPB.Enable(sal_False);
                aNameED.GrabFocus();
            }

    virtual void	Apply();
    String 	GetName(){return aNameED.GetText();}
};
void SwNewUserIdxDlg::Apply()
{
}

IMPL_LINK( SwNewUserIdxDlg, ModifyHdl, Edit*, pEdit)
{
    aOKPB.Enable(pEdit->GetText().Len() && !pDlg->IsTOXType(pEdit->GetText()));
    return 0;
}

IMPL_LINK( SwIndexMarkDlg, NewUserIdxHdl, Button*, pButton)
{
    SwNewUserIdxDlg* pDlg = new SwNewUserIdxDlg(this);
    if(RET_OK == pDlg->Execute())
    {
        String sNewName(pDlg->GetName());
        aTypeDCB.InsertEntry(sNewName);
        aTypeDCB.SelectEntry(sNewName);
    }
    delete pDlg;
    return 0;
}
/* -----------------------------17.01.00 12:18--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwIndexMarkDlg, SearchTypeHdl, CheckBox*, pBox)
{
    sal_Bool bEnable = pBox->IsChecked() && pBox->IsEnabled();
    aSearchCaseWordOnlyCB.Enable(bEnable);
    aSearchCaseSensitiveCB.Enable(bEnable);
    return 0;
}
/* -----------------07.09.99 10:30-------------------

 --------------------------------------------------*/
IMPL_LINK( SwIndexMarkDlg, InsertHdl, Button *, pButton )
{
    Apply();
    //close the dialog if only one entry is available
    if(!bNewMark && !aPrevBT.IsVisible() && !aNextBT.IsVisible())
        CloseHdl(pButton);
    return 0;
}
/* -----------------07.09.99 10:29-------------------

 --------------------------------------------------*/
IMPL_LINK( SwIndexMarkDlg, CloseHdl, Button *, pButton )
{
    if(bNewMark)
    {
        sal_uInt16 nSlot = FN_INSERT_IDX_ENTRY_DLG;
        SfxViewFrame::Current()->GetDispatcher()->Execute(nSlot,
                    SFX_CALLMODE_ASYNCHRON|SFX_CALLMODE_RECORD);
    }
    else
    {
        ((SwIndexMarkModalDlg*)GetParent())->EndDialog(RET_CANCEL);
    }
    return 0;
}
/*--------------------------------------------------------------------
     Beschreibung:	VerzeichnisTyp auswaehlen nur bei Einfuegen
 --------------------------------------------------------------------*/
IMPL_LINK( SwIndexMarkDlg, ModifyHdl, ListBox *, pBox )
{
    if(&aTypeDCB == pBox)
    {
        // Verzeichnistyp setzen
        sal_uInt16 nPos = aTypeDCB.GetEntryPos(aTypeDCB.GetSelectEntry());
        sal_Bool bLevelEnable = sal_False,
             bKeyEnable	  = sal_False,
             bSetKey2 	  = sal_False,
             bKey2Enable  = sal_False,
            bEntryHasText	= sal_False,
            bKey1HasText	= sal_False,
            bKey2HasText	= sal_False;
        if(nPos == POS_INDEX)
        {
            if(aEntryED.GetText().Len())
                bEntryHasText = sal_True;
            aPhoneticED0.SetText(GetDefaultPhoneticReading(aEntryED.GetText()));

            bKeyEnable = sal_True;
            aKeyDCB.SetText(aKeyDCB.GetEntry(nKey1Pos));
            aPhoneticED1.SetText(GetDefaultPhoneticReading(aKeyDCB.GetText()));
            if(aKeyDCB.GetText().Len() > 0)
            {
                bKey1HasText = bSetKey2 = bKey2Enable = sal_True;
                aKey2DCB.SetText(aKey2DCB.GetEntry(nKey2Pos));
                aPhoneticED2.SetText(GetDefaultPhoneticReading(aKey2DCB.GetText()));
                if(aKey2DCB.GetText().Len())
                    bKey2HasText = sal_True;
            }
        }
        else
        {
            bLevelEnable = sal_True;
            aLevelED.SetMax(MAXLEVEL);
            aLevelED.SetValue(aLevelED.Normalize(0));
            bSetKey2 = sal_True;
        }
        aLevelFT.Show(bLevelEnable);
        aLevelED.Show(bLevelEnable);
        aMainEntryCB.Show(nPos == POS_INDEX);

        aKeyFT.Enable(bKeyEnable);
        aKeyDCB.Enable(bKeyEnable);
        if ( bSetKey2 )
        {
            aKey2DCB.Enable(bKey2Enable);
            aKey2FT.Enable(bKey2Enable);
        }
        aPhoneticFT0.Enable(bKeyEnable&&bEntryHasText&&bIsPhoneticReadingEnabled);
        aPhoneticED0.Enable(bKeyEnable&&bEntryHasText&&bIsPhoneticReadingEnabled);
        aPhoneticFT1.Enable(bKeyEnable&&bKey1HasText&&bIsPhoneticReadingEnabled);
        aPhoneticED1.Enable(bKeyEnable&&bKey1HasText&&bIsPhoneticReadingEnabled);
        aPhoneticFT2.Enable(bKeyEnable&&bKey2HasText&&bIsPhoneticReadingEnabled);
        aPhoneticED2.Enable(bKeyEnable&&bKey2HasText&&bIsPhoneticReadingEnabled);
    }
    else //aEntryED  !!aEntryED is not a ListBox but a Edit
    {
        BOOL bHasText = (aEntryED.GetText().Len()>0);
        if(!bHasText)
        {
            aPhoneticED0.SetText(aEmptyStr);
            bPhoneticED0_ChangedByUser = FALSE;
        }
        else if(!bPhoneticED0_ChangedByUser)
            aPhoneticED0.SetText(GetDefaultPhoneticReading(aEntryED.GetText()));

        aPhoneticFT0.Enable(bHasText&&bIsPhoneticReadingEnabled);
        aPhoneticED0.Enable(bHasText&&bIsPhoneticReadingEnabled);
    }
    aOKBT.Enable(!pSh->HasReadonlySel() &&
        (aEntryED.GetText().Len() || pSh->GetCrsrCnt(sal_False)));
    return 0;
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/


IMPL_LINK_INLINE_START( SwIndexMarkDlg, NextHdl, Button *, EMPTYARG )
{
    InsertUpdate();
    pTOXMgr->NextTOXMark();
    UpdateDialog();
    return 0;
}
IMPL_LINK_INLINE_END( SwIndexMarkDlg, NextHdl, Button *, EMPTYARG )


IMPL_LINK_INLINE_START( SwIndexMarkDlg, NextSameHdl, Button *, EMPTYARG )
{
    InsertUpdate();
    pTOXMgr->NextTOXMark(sal_True);
    UpdateDialog();
    return 0;
}
IMPL_LINK_INLINE_END( SwIndexMarkDlg, NextSameHdl, Button *, EMPTYARG )


IMPL_LINK_INLINE_START( SwIndexMarkDlg, PrevHdl, Button *, EMPTYARG )
{
    InsertUpdate();
    pTOXMgr->PrevTOXMark();
    UpdateDialog();
    return 0;
}
IMPL_LINK_INLINE_END( SwIndexMarkDlg, PrevHdl, Button *, EMPTYARG )


IMPL_LINK_INLINE_START( SwIndexMarkDlg, PrevSameHdl, Button *, EMPTYARG )
{
    InsertUpdate();
    pTOXMgr->PrevTOXMark(sal_True);
    UpdateDialog();

    return 0;
}
IMPL_LINK_INLINE_END( SwIndexMarkDlg, PrevSameHdl, Button *, EMPTYARG )


IMPL_LINK( SwIndexMarkDlg, DelHdl, Button *, EMPTYARG )
{
    bDel = sal_True;
    InsertUpdate();
    bDel = sal_False;

    if(pTOXMgr->GetCurTOXMark())
        UpdateDialog();
    else
    {
        CloseHdl(&aCancelBT);
        SfxViewFrame::Current()->GetBindings().Invalidate(FN_EDIT_IDX_ENTRY_DLG);
    }
    return 0;
}

/*--------------------------------------------------------------------
     Beschreibung: Dialoganzeige erneuern
 --------------------------------------------------------------------*/


void SwIndexMarkDlg::UpdateDialog()
{
    DBG_ASSERT(pSh && pTOXMgr, "Shell nicht da?")
    SwTOXMark* pMark = pTOXMgr->GetCurTOXMark();
    ASSERT(pMark, "Keine aktuelle Markierung");
    if(!pMark)
        return;

    ViewShell::SetCareWin( GetParent() );

    aOrgStr = pMark->GetText();
    aEntryED.SetText(aOrgStr);

    // Verzeichnistyp setzen
    sal_Bool bLevelEnable = sal_True,
         bKeyEnable   = sal_False,
         bKey2Enable  = sal_False,
         bEntryHasText	= sal_False,
         bKey1HasText	= sal_False,
         bKey2HasText	= sal_False;

    TOXTypes eCurType = pMark->GetTOXType()->GetType();
    if(TOX_INDEX == eCurType)
    {
        bLevelEnable = sal_False;
        bKeyEnable = sal_True;
        bKey1HasText = bKey2Enable = 0 != pMark->GetPrimaryKey().Len();
        bKey2HasText = 0 != pMark->GetSecondaryKey().Len();
        bEntryHasText = 0 != pMark->GetText().Len();
        aKeyDCB.SetText( pMark->GetPrimaryKey() );
        aKey2DCB.SetText( pMark->GetSecondaryKey() );
        aPhoneticED0.SetText( pMark->GetTextReading() );
        aPhoneticED1.SetText( pMark->GetPrimaryKeyReading() );
        aPhoneticED2.SetText( pMark->GetSecondaryKeyReading() );
        aMainEntryCB.Check(pMark->IsMainEntry());
    }
    else if(TOX_CONTENT == eCurType || TOX_USER == eCurType)
    {
        aLevelED.SetValue(aLevelED.Normalize(pMark->GetLevel()));
    }
    aKeyFT.Enable(bKeyEnable);
    aKeyDCB.Enable(bKeyEnable);
    aLevelED.SetMax(MAXLEVEL);
    aLevelFT.Show(bLevelEnable);
    aLevelED.Show(bLevelEnable);
    aMainEntryCB.Show(!bLevelEnable);
    aKey2FT.Enable(bKey2Enable);
    aKey2DCB.Enable(bKey2Enable);

    UpdateLanguageDependenciesForPhoneticReading();
    aPhoneticFT0.Enable(bKeyEnable&&bEntryHasText&&bIsPhoneticReadingEnabled);
    aPhoneticED0.Enable(bKeyEnable&&bEntryHasText&&bIsPhoneticReadingEnabled);
    aPhoneticFT1.Enable(bKeyEnable&&bKey1HasText&&bIsPhoneticReadingEnabled);
    aPhoneticED1.Enable(bKeyEnable&&bKey1HasText&&bIsPhoneticReadingEnabled);
    aPhoneticFT2.Enable(bKeyEnable&&bKey2HasText&&bIsPhoneticReadingEnabled);
    aPhoneticED2.Enable(bKeyEnable&&bKey2HasText&&bIsPhoneticReadingEnabled);

    // Verzeichnis-Typ setzen
    aTypeDCB.SelectEntry(pMark->GetTOXType()->GetTypeName());

    // Next - Prev - Buttons setzen
    pSh->SttCrsrMove();
    if( aPrevBT.IsVisible() )
    {
        const SwTOXMark* pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_PRV );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_NXT );
        aPrevBT.Enable( pMoveMark != pMark );
        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_NXT );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_PRV );
        aNextBT.Enable( pMoveMark != pMark );
    }

    if( aPrevSameBT.IsVisible() )
    {
        const SwTOXMark* pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_SAME_PRV );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_SAME_NXT );
        aPrevSameBT.Enable( pMoveMark != pMark );
        pMoveMark = &pSh->GotoTOXMark( *pMark, TOX_SAME_NXT );
        if( pMoveMark != pMark )
            pSh->GotoTOXMark( *pMoveMark, TOX_SAME_PRV );
        aNextSameBT.Enable( pMoveMark != pMark );
    }

    sal_Bool bEnable = !pSh->HasReadonlySel();
    aOKBT.Enable( bEnable );
    aDelBT.Enable( bEnable );
    aEntryED.SetReadOnly( !bEnable );
    aLevelED.SetReadOnly( !bEnable );
    aKeyDCB.SetReadOnly( !bEnable );
    aKey2DCB.SetReadOnly( !bEnable );

    pSh->SelectTxtAttr( RES_TXTATR_TOXMARK, pMark->GetTxtTOXMark() );
    // we need the point at the start of the attribut
    pSh->SwapPam();

    pSh->EndCrsrMove();
}

/*--------------------------------------------------------------------
     Remind wether the edit boxes for Phonetic reading are changed manually
 --------------------------------------------------------------------*/

IMPL_LINK( SwIndexMarkDlg, PhoneticEDModifyHdl, Edit *, pEdit )
{
    if(&aPhoneticED0 == pEdit)
    {
        bPhoneticED0_ChangedByUser = pEdit->GetText().Len()>0;
    }
    else if(&aPhoneticED1 == pEdit)
    {
        bPhoneticED1_ChangedByUser = pEdit->GetText().Len()>0;
    }
    else if(&aPhoneticED2 == pEdit)
    {
        bPhoneticED2_ChangedByUser = pEdit->GetText().Len()>0;
    }
    return 0;
}

/*--------------------------------------------------------------------
     Beschreibung: Enable Disable des 2. Schluessels
 --------------------------------------------------------------------*/

IMPL_LINK( SwIndexMarkDlg, KeyDCBModifyHdl, ComboBox *, pBox )
{
    if(&aKeyDCB == pBox)
    {
        sal_Bool bEnable = pBox->GetText().Len() > 0;
        if(!bEnable)
        {
            aKey2DCB.SetText(aEmptyStr);
            aPhoneticED1.SetText(aEmptyStr);
            aPhoneticED2.SetText(aEmptyStr);
            bPhoneticED1_ChangedByUser = FALSE;
            bPhoneticED2_ChangedByUser = FALSE;
        }
        else
        {
            if(pBox->IsInDropDown())
            {
                //reset bPhoneticED1_ChangedByUser if a completly new string is selected
                bPhoneticED1_ChangedByUser = FALSE;
            }
            if(!bPhoneticED1_ChangedByUser)
                aPhoneticED1.SetText(GetDefaultPhoneticReading(pBox->GetText()));
        }
        aKey2DCB.Enable(bEnable);
        aKey2FT.Enable(bEnable);
    }
    else if(&aKey2DCB == pBox)
    {
        if(!(pBox->GetText().Len()>0))
        {
            aPhoneticED2.SetText(aEmptyStr);
            bPhoneticED2_ChangedByUser = FALSE;
        }
        else
        {
            if(pBox->IsInDropDown())
            {
                //reset bPhoneticED1_ChangedByUser if a completly new string is selected
                bPhoneticED2_ChangedByUser = FALSE;
            }
            if(!bPhoneticED2_ChangedByUser)
                aPhoneticED2.SetText(GetDefaultPhoneticReading(pBox->GetText()));
        }
    }
    sal_Bool	bKey1HasText	= (0 != aKeyDCB.GetText().Len());
    sal_Bool	bKey2HasText	= (0 != aKey2DCB.GetText().Len());

    aPhoneticFT1.Enable(bKey1HasText&&bIsPhoneticReadingEnabled);
    aPhoneticED1.Enable(bKey1HasText&bIsPhoneticReadingEnabled);
    aPhoneticFT2.Enable(bKey2HasText&bIsPhoneticReadingEnabled);
    aPhoneticED2.Enable(bKey2HasText&bIsPhoneticReadingEnabled);

    return 0;
}

/*-----------------25.02.94 21:19-------------------
 dtor ueberladen
--------------------------------------------------*/


SwIndexMarkDlg::~SwIndexMarkDlg()
{
    delete pTOXMgr;
    ViewShell::SetCareWin( 0 );
}
/* -----------------07.09.99 08:41-------------------

 --------------------------------------------------*/
void    SwIndexMarkDlg::ReInitDlg(SwWrtShell& rWrtShell, SwTOXMark* pCurTOXMark)
{
    pSh = &rWrtShell;
    delete pTOXMgr;
    pTOXMgr = new SwTOXMgr(pSh);
    if(pCurTOXMark)
    {
        for(USHORT i = 0; i < pTOXMgr->GetTOXMarkCount(); i++)
            if(pTOXMgr->GetTOXMark(i) == pCurTOXMark)
            {
                pTOXMgr->SetCurTOXMark(i);
                break;
            }
    }
    InitControls();
}
/* -----------------06.10.99 10:00-------------------

 --------------------------------------------------*/
SwIndexMarkFloatDlg::SwIndexMarkFloatDlg(SfxBindings* pBindings,
                                   SfxChildWindow* pChild,
                                   Window *pParent,
                                SfxChildWinInfo* pInfo,
                                   sal_Bool bNew) :
SfxModelessDialog(pBindings, pChild, pParent, SvtCJKOptions().IsCJKFontEnabled()?SW_RES(DLG_INSIDXMARK_CJK):SW_RES(DLG_INSIDXMARK)),
    aDlg(this, bNew, ResId(WIN_DLG), SvtCJKOptions().IsCJKFontEnabled()?DLG_INSIDXMARK_CJK:DLG_INSIDXMARK)
{
    FreeResource();
      SwWrtShell* pWrtShell = ::GetActiveWrtShell();
      DBG_ASSERT(pWrtShell, "No shell?")
      aDlg.ReInitDlg(*pWrtShell);
    Initialize(pInfo);
}
/* -----------------06.10.99 10:27-------------------

 --------------------------------------------------*/
void	SwIndexMarkFloatDlg::Activate()
{
    SfxModelessDialog::Activate();
    aDlg.Activate();
}

void SwIndexMarkFloatDlg::ReInitDlg(SwWrtShell& rWrtShell)
{
    aDlg.ReInitDlg( rWrtShell );
}

/* -----------------06.10.99 10:35-------------------

 --------------------------------------------------*/
SwIndexMarkModalDlg::SwIndexMarkModalDlg(Window *pParent, SwWrtShell& rSh, SwTOXMark* pCurTOXMark) :
SvxStandardDialog(pParent, SvtCJKOptions().IsCJKFontEnabled()?SW_RES(DLG_EDIT_IDXMARK_CJK):SW_RES(DLG_EDIT_IDXMARK)),
    aDlg(this, sal_False, ResId(WIN_DLG), SvtCJKOptions().IsCJKFontEnabled()?DLG_EDIT_IDXMARK_CJK:DLG_EDIT_IDXMARK)
{
    FreeResource();
    aDlg.ReInitDlg(rSh, pCurTOXMark);
}
/* -----------------06.10.99 10:46-------------------

 --------------------------------------------------*/
void	SwIndexMarkModalDlg::Apply()
{
    aDlg.Apply();
}
/* -----------------07.09.99 08:15-------------------

 --------------------------------------------------*/
//CHINA001 SFX_IMPL_CHILDWINDOW(SwInsertIdxMarkWrapper, FN_INSERT_IDX_ENTRY_DLG)

//CHINA001 SwInsertIdxMarkWrapper::SwInsertIdxMarkWrapper(	Window *pParentWindow,
//CHINA001 							sal_uInt16 nId,
//CHINA001 							SfxBindings* pBindings,
//CHINA001 							SfxChildWinInfo* pInfo ) :
//CHINA001 		SfxChildWindow(pParentWindow, nId)
//CHINA001 {
//CHINA001 
//CHINA001 pWindow = new SwIndexMarkFloatDlg(pBindings, this, pParentWindow, pInfo );
//CHINA001 pWindow->Show();	// at this point,because before pSh has to be initialized in ReInitDlg()
//CHINA001 // -> Show() will invoke StateChanged() and save pos
//CHINA001 eChildAlignment = SFX_ALIGN_NOALIGNMENT;
//CHINA001 }
/* -----------------07.09.99 09:14-------------------

 --------------------------------------------------*/
//CHINA001 SfxChildWinInfo SwInsertIdxMarkWrapper::GetInfo() const
//CHINA001 {
//CHINA001 SfxChildWinInfo aInfo = SfxChildWindow::GetInfo();
//CHINA001 
//CHINA001 return aInfo;
//CHINA001 }

//CHINA001 void	SwInsertIdxMarkWrapper::ReInitDlg(SwWrtShell& rWrtShell)
//CHINA001 {
//CHINA001 ((SwIndexMarkFloatDlg*)pWindow)->ReInitDlg(rWrtShell);
//CHINA001 }

/* -----------------16.09.99 14:19-------------------

 --------------------------------------------------*/
class SwCreateAuthEntryDlg_Impl : public ModalDialog
{

    FixedLine       aEntriesFL;

    FixedText*		pFixedTexts[AUTH_FIELD_END];
    ListBox*		pTypeListBox;
    ComboBox*		pIdentifierBox;
    Edit*			pEdits[AUTH_FIELD_END];

    OKButton        aOKBT;
    CancelButton 	aCancelBT;
    HelpButton 		aHelpBT;

    Link			aShortNameCheckLink;

    SwWrtShell& 	rWrtSh;

    sal_Bool			m_bNewEntryMode;
    sal_Bool			m_bNameAllowed;

    DECL_LINK(IdentifierHdl, ComboBox*);
    DECL_LINK(ShortNameHdl, Edit*);
    DECL_LINK(EnableHdl, ListBox* pBox);

public:
    SwCreateAuthEntryDlg_Impl(Window* pParent,
                            const String pFields[],
                            SwWrtShell& rSh,
                            sal_Bool bNewEntry,
                            sal_Bool bCreate);
    ~SwCreateAuthEntryDlg_Impl();

    String			GetEntryText(ToxAuthorityField eField) const;

    void			SetCheckNameHdl(const Link& rLink) {aShortNameCheckLink = rLink;}

};
/*-- 15.09.99 08:43:24---------------------------------------------------

  -----------------------------------------------------------------------*/
struct TextInfo
{
    sal_uInt16 nToxField;
    sal_uInt16 nHelpId;
};
static const TextInfo aTextInfoArr[] =
{
    {AUTH_FIELD_IDENTIFIER,      HID_AUTH_FIELD_IDENTIFIER      },
    {AUTH_FIELD_AUTHORITY_TYPE,  HID_AUTH_FIELD_AUTHORITY_TYPE  },
    {AUTH_FIELD_AUTHOR,          HID_AUTH_FIELD_AUTHOR          },
    {AUTH_FIELD_TITLE,           HID_AUTH_FIELD_TITLE           },
    {AUTH_FIELD_YEAR,            HID_AUTH_FIELD_YEAR            },
    {AUTH_FIELD_PUBLISHER,       HID_AUTH_FIELD_PUBLISHER       },
    {AUTH_FIELD_ADDRESS,         HID_AUTH_FIELD_ADDRESS         },
    {AUTH_FIELD_ISBN,            HID_AUTH_FIELD_ISBN            },
    {AUTH_FIELD_CHAPTER,         HID_AUTH_FIELD_CHAPTER         },
    {AUTH_FIELD_PAGES,           HID_AUTH_FIELD_PAGES           },
    {AUTH_FIELD_EDITOR,          HID_AUTH_FIELD_EDITOR          },
    {AUTH_FIELD_EDITION,         HID_AUTH_FIELD_EDITION         },
    {AUTH_FIELD_BOOKTITLE,       HID_AUTH_FIELD_BOOKTITLE       },
    {AUTH_FIELD_VOLUME,          HID_AUTH_FIELD_VOLUME          },
    {AUTH_FIELD_HOWPUBLISHED,    HID_AUTH_FIELD_HOWPUBLISHED    },
    {AUTH_FIELD_ORGANIZATIONS,   HID_AUTH_FIELD_ORGANIZATIONS   },
    {AUTH_FIELD_INSTITUTION,     HID_AUTH_FIELD_INSTITUTION     },
    {AUTH_FIELD_SCHOOL,          HID_AUTH_FIELD_SCHOOL          },
    {AUTH_FIELD_REPORT_TYPE,     HID_AUTH_FIELD_REPORT_TYPE     },
    {AUTH_FIELD_MONTH,           HID_AUTH_FIELD_MONTH           },
    {AUTH_FIELD_JOURNAL,         HID_AUTH_FIELD_JOURNAL         },
    {AUTH_FIELD_NUMBER,          HID_AUTH_FIELD_NUMBER          },
    {AUTH_FIELD_SERIES,          HID_AUTH_FIELD_SERIES          },
    {AUTH_FIELD_ANNOTE,          HID_AUTH_FIELD_ANNOTE          },
    {AUTH_FIELD_NOTE,            HID_AUTH_FIELD_NOTE            },
    {AUTH_FIELD_URL,             HID_AUTH_FIELD_URL             },
    {AUTH_FIELD_CUSTOM1,         HID_AUTH_FIELD_CUSTOM1         },
    {AUTH_FIELD_CUSTOM2,         HID_AUTH_FIELD_CUSTOM2         },
    {AUTH_FIELD_CUSTOM3,         HID_AUTH_FIELD_CUSTOM3         },
    {AUTH_FIELD_CUSTOM4,         HID_AUTH_FIELD_CUSTOM4         },
    {AUTH_FIELD_CUSTOM5,         HID_AUTH_FIELD_CUSTOM5         }
};

sal_Bool SwAuthMarkDlg::bIsFromComponent = sal_True;

SwAuthMarkDlg::SwAuthMarkDlg(  Window *pParent,
                               const ResId& rResId,
                               sal_Bool bNewDlg) :
    Window(pParent, rResId),
    aFromComponentRB(	this, ResId(RB_FROMCOMPONENT	)),
    aFromDocContentRB(	this, ResId(RB_FROMDOCCONTENT	)),
    aEntryFT(	this, ResId(FT_ENTRY	)),
    aEntryED(	this, ResId(ED_ENTRY	)),
    aEntryLB(	this, ResId(LB_ENTRY	)),
    aAuthorFT(	this, ResId(FT_AUTHOR   	)),
    aAuthorFI(	this, ResId(FI_AUTHOR  	)),
    aTitleFT(	this, ResId(FT_TITLE 	)),
    aTitleFI(	this, ResId(FI_TITLE 	)),
    aEntryFL(   this, ResId(FL_ENTRY    )),
    aOKBT(		this, ResId(PB_OK		)),
    aCancelBT(	this, ResId(PB_CANCEL	)),
    aHelpBT(	this, ResId(PB_HELP	)),
    sChangeST(	ResId(ST_CHANGE)),
    aCreateEntryPB(this,ResId(PB_CREATEENTRY)),
    aEditEntryPB(this, 	ResId(PB_EDITENTRY)),
    bNewEntry(bNewDlg),
    pSh(0),
    bBibAccessInitialized(sal_False)
{
    SetStyle(GetStyle()|WB_DIALOGCONTROL);
    FreeResource();

    aFromComponentRB.SetHelpId(HID_AUTH_MARK_DLG_FROM_COMP_RB);
    aFromDocContentRB.SetHelpId(HID_AUTH_MARK_DLG_FROM_DOC_RB );
    aEntryED.SetHelpId(HID_AUTH_MARK_DLG_ID_LISTBOX           );
    aEntryLB.SetHelpId(HID_AUTH_MARK_DLG_ID_LISTBOX           );

    aFromComponentRB.Show(bNewEntry);
    aFromDocContentRB.Show(bNewEntry);
    aFromComponentRB.Check(bIsFromComponent);
    aFromDocContentRB.Check(!bIsFromComponent);

    aOKBT		.SetHelpId(HID_INSERT_AUTH_MRK_OK   );
    aCancelBT	.SetHelpId(HID_INSERT_AUTH_MRK_CLOSE);
    aEntryED		.SetHelpId(HID_INSERT_AUTH_MRK_ENTRY		);
    aCreateEntryPB  .SetHelpId(HID_INSERT_AUTH_MRK_CREATE_ENTRY	);
    aEditEntryPB    .SetHelpId(HID_INSERT_AUTH_MRK_EDIT_ENTRY	);

    aOKBT.SetClickHdl(LINK(this,SwAuthMarkDlg, InsertHdl));
    aCancelBT.SetClickHdl(LINK(this,SwAuthMarkDlg, CloseHdl));
    aCreateEntryPB.SetClickHdl(LINK(this,SwAuthMarkDlg, CreateEntryHdl));
    aEditEntryPB.SetClickHdl(LINK(this,SwAuthMarkDlg, CreateEntryHdl));
    aFromComponentRB.SetClickHdl(LINK(this,SwAuthMarkDlg, ChangeSourceHdl));
    aFromDocContentRB.SetClickHdl(LINK(this,SwAuthMarkDlg, ChangeSourceHdl));
    aEntryED.SetModifyHdl(LINK(this,SwAuthMarkDlg, EditModifyHdl));

    GetParent()->SetText(String(SW_RES(
                    bNewEntry ? STR_AUTHMRK_INSERT : STR_AUTHMRK_EDIT)));
    aEntryED.Show(!bNewEntry);
    aEntryLB.Show(bNewEntry);
    if(!bNewEntry)
    {
        aOKBT.SetText(sChangeST);
    }
    else
    {
        aEntryLB.SetSelectHdl(LINK(this, SwAuthMarkDlg, CompEntryHdl));
    }
}
/*-- 15.09.99 08:43:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAuthMarkDlg::~SwAuthMarkDlg()
{
}
/*-- 15.09.99 08:43:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwAuthMarkDlg::ReInitDlg(SwWrtShell& rWrtShell)
{
    pSh = &rWrtShell;
    InitControls();
}
/* -----------------15.09.99 08:57-------------------

 --------------------------------------------------*/
IMPL_LINK( SwAuthMarkDlg, CloseHdl, PushButton *, EMPTYARG )
{
    if(bNewEntry)
    {
        sal_uInt16 nSlot = FN_INSERT_AUTH_ENTRY_DLG;
        SfxViewFrame::Current()->GetDispatcher()->Execute(nSlot,
                    SFX_CALLMODE_ASYNCHRON|SFX_CALLMODE_RECORD);
    }
    else
    {
        ((SwAuthMarkModalDlg*)GetParent())->EndDialog(RET_CANCEL);
    }
    return 0;
}
/* -----------------06.12.99 13:54-------------------

 --------------------------------------------------*/
static String lcl_FindColumnEntry(const beans::PropertyValue* pFields, sal_Int32 nLen, const String& rColumnTitle)
{
    String sRet;
    OUString uColumnTitle = rColumnTitle;
    for(sal_uInt16 i = 0; i < nLen; i++)
    {
        OUString uTmp;
        if(pFields[i].Name == uColumnTitle &&
            (pFields[i].Value >>= uTmp))
        {
            sRet = String(uTmp);
            break;
        }
    }
    return sRet;
}
/* -----------------------------07.12.99 15:39--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwAuthMarkDlg, CompEntryHdl, ListBox*, pBox)
{
    String sEntry(pBox->GetSelectEntry());
    if(bIsFromComponent)
    {
        if(xBibAccess.is() && sEntry.Len())
        {
            OUString uEntry(sEntry);
            if(xBibAccess->hasByName(uEntry))
            {
                uno::Any aEntry(xBibAccess->getByName(uEntry));
                uno::Sequence<beans::PropertyValue> aFieldProps;
                if(aEntry >>= aFieldProps)
                {
                    const beans::PropertyValue* pProps = aFieldProps.getConstArray();
                    for(sal_uInt16 i = 0; i < AUTH_FIELD_END && i < aFieldProps.getLength(); i++)
                    {
                        m_sFields[i] = lcl_FindColumnEntry(
                                pProps, aFieldProps.getLength(), m_sColumnTitles[i]);
                    }
                }
            }
        }
    }
    else
    {
        if(sEntry.Len())
        {
            const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                        pSh->GetFldType(RES_AUTHORITY, aEmptyStr);
            const SwAuthEntry* 	pEntry = pFType ? pFType->GetEntryByIdentifier(sEntry) : 0;
            for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
                m_sFields[i] = pEntry ?
                            pEntry->GetAuthorField((ToxAuthorityField)i) : aEmptyStr;
        }
    }
    if(!pBox->GetSelectEntry().Len())
    {
        for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
            m_sFields[i] = aEmptyStr;
    }
    aAuthorFI.SetText(m_sFields[AUTH_FIELD_AUTHOR]);
    aTitleFI.SetText(m_sFields[AUTH_FIELD_TITLE]);
    return 0;
}

/* -----------------15.09.99 08:57-------------------

 --------------------------------------------------*/
IMPL_LINK( SwAuthMarkDlg, InsertHdl, PushButton *, EMPTYARG )
{
    //insert or update the SwAuthorityField...
    if(pSh)
    {
        sal_Bool bDifferent = sal_False;
        DBG_ASSERT(m_sFields[AUTH_FIELD_IDENTIFIER].Len() , "No Id is set!")
        DBG_ASSERT(m_sFields[AUTH_FIELD_AUTHORITY_TYPE].Len() , "No authority type is set!")
        //check if the entry already exists with different content
        const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                        pSh->GetFldType(RES_AUTHORITY, aEmptyStr);
        const SwAuthEntry* 	pEntry = pFType ?
                pFType->GetEntryByIdentifier( m_sFields[AUTH_FIELD_IDENTIFIER])
                : 0;
        if(pEntry)
        {
            for(sal_uInt16 i = 0; i < AUTH_FIELD_END && !bDifferent; i++)
                bDifferent |= m_sFields[i] != pEntry->GetAuthorField((ToxAuthorityField)i);
            if(bDifferent)
            {
                QueryBox aQuery(this, SW_RES(DLG_CHANGE_AUTH_ENTRY));
                if(RET_YES != aQuery.Execute())
                    return 0;
            }
        }

        SwFldMgr aMgr(pSh);
        String sFields;
        for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
        {
            sFields += m_sFields[i];
            sFields += TOX_STYLE_DELIMITER;
        }
        if(bNewEntry)
        {
            if(bDifferent)
            {
                SwAuthEntry aNewData;
                for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
                    aNewData.SetAuthorField((ToxAuthorityField)i, m_sFields[i]);
                pSh->ChangeAuthorityData(&aNewData);
            }
            SwInsertFld_Data aData(TYP_AUTHORITY, 0, sFields, aEmptyStr, 0 );
            aMgr.InsertFld( aData );
        }
        else if(aMgr.GetCurFld())
        {
            aMgr.UpdateCurFld(0, sFields, aEmptyStr);
        }
    }
    if(!bNewEntry)
        CloseHdl(0);
    return 0;
}
/* -----------------17.09.99 13:06-------------------

 --------------------------------------------------*/
IMPL_LINK(SwAuthMarkDlg, CreateEntryHdl, PushButton*, pButton)
{
    sal_Bool bCreate = pButton == &aCreateEntryPB;
    String sOldId = m_sCreatedEntry[0];
    for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
        m_sCreatedEntry[i] = bCreate ? aEmptyStr : m_sFields[i];
    SwCreateAuthEntryDlg_Impl aDlg(pButton,
                bCreate ? m_sCreatedEntry : m_sFields,
                *pSh, bNewEntry, bCreate);
    if(bNewEntry)
    {
        aDlg.SetCheckNameHdl(LINK(this, SwAuthMarkDlg, IsEntryAllowedHdl));
    }
    if(RET_OK == aDlg.Execute())
    {
        if(bCreate && sOldId.Len())
        {
            aEntryLB.RemoveEntry(sOldId);
        }
        for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
        {
            m_sFields[i] = aDlg.GetEntryText((ToxAuthorityField)i);
            m_sCreatedEntry[i] = m_sFields[i];
        }
        if(bNewEntry && !aFromDocContentRB.IsChecked())
        {
            aFromDocContentRB.Check(sal_True);
            ChangeSourceHdl(&aFromDocContentRB);
        }
        if(bCreate)
        {
            DBG_ASSERT(LISTBOX_ENTRY_NOTFOUND ==
                        aEntryLB.GetEntryPos(m_sFields[AUTH_FIELD_IDENTIFIER]),
                        "entry exists!")
            aEntryLB.InsertEntry(m_sFields[AUTH_FIELD_IDENTIFIER]);
            aEntryLB.SelectEntry(m_sFields[AUTH_FIELD_IDENTIFIER]);
        }
        aEntryED.SetText(m_sFields[AUTH_FIELD_IDENTIFIER]);
        aAuthorFI.SetText(m_sFields[AUTH_FIELD_AUTHOR]);
        aTitleFI.SetText(m_sFields[AUTH_FIELD_TITLE]);
        aOKBT.Enable();
    }
    return 0;
}
/* -----------------------------20.12.99 14:26--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwAuthMarkDlg, ChangeSourceHdl, RadioButton*, pButton)
{
    sal_Bool bFromComp = (pButton == &aFromComponentRB);
    bIsFromComponent = bFromComp;
    aCreateEntryPB.Enable(!bIsFromComponent);
    aEntryLB.Clear();
    if(bIsFromComponent)
    {
        if(!bBibAccessInitialized)
        {
             uno::Reference< lang::XMultiServiceFactory > xMSF = getProcessServiceFactory();
            xBibAccess = uno::Reference< container::XNameAccess > (
                            xMSF->createInstance( C2U("com.sun.star.frame.Bibliography") ),
                                                                        uno::UNO_QUERY );
            uno::Reference< beans::XPropertySet >  xPropSet(xBibAccess, uno::UNO_QUERY);
            OUString uPropName(C2U("BibliographyDataFieldNames"));
            if(xPropSet.is() && xPropSet->getPropertySetInfo()->hasPropertyByName(uPropName))
            {
                uno::Any aNames = xPropSet->getPropertyValue(uPropName);
                uno::Sequence<beans::PropertyValue> aSeq;
                if( aNames >>= aSeq)
                {
                    const beans::PropertyValue* pArr = aSeq.getConstArray();
                    for(sal_uInt16 i = 0; i < aSeq.getLength(); i++)
                    {
                        String sTitle = pArr[i].Name;
                        sal_Int16 nField;
                        pArr[i].Value >>= nField;
                        if(nField >= 0 && nField < AUTH_FIELD_END)
                            m_sColumnTitles[nField] = sTitle;
                    }
                }
            }
            bBibAccessInitialized = sal_True;
        }
        if(xBibAccess.is())
        {
            uno::Sequence<OUString> aIdentifiers = xBibAccess->getElementNames();
            const OUString* pNames = aIdentifiers.getConstArray();
            for(sal_uInt16 i = 0; i < aIdentifiers.getLength(); i++)
            {
                aEntryLB.InsertEntry(pNames[i]);
            }
        }
    }
    else
    {
        const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                    pSh->GetFldType(RES_AUTHORITY, aEmptyStr);
        if(pFType)
        {
            SvStringsDtor aIds;
            pFType->GetAllEntryIdentifiers(	aIds );
            for(sal_uInt16 n = 0; n < aIds.Count(); n++)
                aEntryLB.InsertEntry(*aIds.GetObject(n));
        }
        if(m_sCreatedEntry[AUTH_FIELD_IDENTIFIER].Len())
            aEntryLB.InsertEntry(m_sCreatedEntry[AUTH_FIELD_IDENTIFIER]);
    }
    aEntryLB.SelectEntryPos(0);
    CompEntryHdl(&aEntryLB);
    return 0;
}
/* -----------------15.10.2002 13:16-----------------
 *
 * --------------------------------------------------*/
IMPL_LINK(SwAuthMarkDlg, EditModifyHdl, Edit*, pEdit)
{
    Link aAllowed = LINK(this, SwAuthMarkDlg, IsEntryAllowedHdl);
    long nResult = aAllowed.Call(pEdit);
    aOKBT.Enable(nResult > 0);
    if(nResult)
    {
        String sEntry(pEdit->GetText());
        m_sFields[AUTH_FIELD_IDENTIFIER] = sEntry;
        m_sCreatedEntry[AUTH_FIELD_IDENTIFIER] = sEntry;
    }
    return 0;
};
/* -----------------------------20.12.99 15:11--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwAuthMarkDlg, IsEntryAllowedHdl, Edit*, pEdit)
{
    String sEntry = pEdit->GetText();
    sal_Bool bAllowed = sal_False;
    if(sEntry.Len())
    {
        if(aEntryLB.GetEntryPos(sEntry) != LISTBOX_ENTRY_NOTFOUND)
            return 0;
        else if(bIsFromComponent)
        {
            const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                        pSh->GetFldType(RES_AUTHORITY, aEmptyStr);
            bAllowed = !pFType || !pFType->GetEntryByIdentifier(sEntry);
        }
        else
        {
            bAllowed = !xBibAccess.is() || !xBibAccess->hasByName(sEntry);
        }
    }
    return bAllowed;
}
/* -----------------21.09.99 14:19-------------------

 --------------------------------------------------*/
void SwAuthMarkDlg::InitControls()
{
    DBG_ASSERT(pSh, "Shell nicht da?")
    SwField* pField = pSh->GetCurFld();
    ASSERT(bNewEntry || pField, "Keine aktuelle Markierung");
    if(bNewEntry)
    {
        ChangeSourceHdl(aFromComponentRB.IsChecked() ? &aFromComponentRB : &aFromDocContentRB);
        aCreateEntryPB.Enable(!aFromComponentRB.IsChecked());
        if(!aFromComponentRB.IsChecked() && m_sCreatedEntry[0].Len())
            for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
                m_sFields[i] = m_sCreatedEntry[i];
    }
    if(bNewEntry || !pField || pField->GetTyp()->Which() != RES_AUTHORITY)
        return;

    const SwAuthEntry* pEntry = ((SwAuthorityFieldType*)pField->GetTyp())->
            GetEntryByHandle(((SwAuthorityField*)pField)->GetHandle());

    DBG_ASSERT(pEntry, "No authority entry found")
    if(!pEntry)
        return;
    for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
        m_sFields[i] = pEntry->GetAuthorField((ToxAuthorityField)i);

    aEntryED.SetText(pEntry->GetAuthorField(AUTH_FIELD_IDENTIFIER));
    aAuthorFI.SetText(pEntry->GetAuthorField(AUTH_FIELD_AUTHOR));
    aTitleFI.SetText(pEntry->GetAuthorField(AUTH_FIELD_TITLE));
}
/* -----------------------------05.09.2002 09:44------------------------------

 ---------------------------------------------------------------------------*/
void    SwAuthMarkDlg::Activate()
{
    aOKBT.Enable(!pSh->HasReadonlySel());
    Window::Activate();
}
/* -----------------07.09.99 08:15-------------------

 --------------------------------------------------*/
//CHINA001 SFX_IMPL_CHILDWINDOW(SwInsertAuthMarkWrapper, FN_INSERT_AUTH_ENTRY_DLG)

//CHINA001 SwInsertAuthMarkWrapper::SwInsertAuthMarkWrapper(	Window *pParentWindow,
//CHINA001 							sal_uInt16 nId,
//CHINA001 							SfxBindings* pBindings,
//CHINA001 							SfxChildWinInfo* pInfo ) :
//CHINA001 		SfxChildWindow(pParentWindow, nId)
//CHINA001 {
//CHINA001 
//CHINA001 pWindow = new SwAuthMarkFloatDlg(pBindings, this, pParentWindow, pInfo );
//CHINA001 
//CHINA001 
//CHINA001 eChildAlignment = SFX_ALIGN_NOALIGNMENT;
//CHINA001 }
/* -----------------07.09.99 09:14-------------------

 --------------------------------------------------*/
//CHINA001 SfxChildWinInfo SwInsertAuthMarkWrapper::GetInfo() const
//CHINA001 {
//CHINA001 SfxChildWinInfo aInfo = SfxChildWindow::GetInfo();
//CHINA001 return aInfo;
//CHINA001 }
/* -----------------19.10.99 11:16-------------------

 --------------------------------------------------*/
//CHINA001 void	SwInsertAuthMarkWrapper::ReInitDlg(SwWrtShell& rWrtShell)
//CHINA001 {
//CHINA001 ((SwAuthMarkFloatDlg*)pWindow)->ReInitDlg(rWrtShell);
//CHINA001 }

/* -----------------16.09.99 14:27-------------------

 --------------------------------------------------*/
SwCreateAuthEntryDlg_Impl::SwCreateAuthEntryDlg_Impl(Window* pParent,
        const String pFields[],
        SwWrtShell& rSh,
        sal_Bool bNewEntry,
        sal_Bool bCreate) :
    ModalDialog(pParent, SW_RES(DLG_CREATE_AUTH_ENTRY)),
    aOKBT(this, 		ResId(PB_OK			)),
    aCancelBT(this, 	ResId(PB_CANCEL		)),
    aHelpBT(this, 		ResId(PB_HELP		)),
    aEntriesFL(this,    ResId(FL_ENTRIES    )),
    pIdentifierBox(0),
    pTypeListBox(0),
    rWrtSh(rSh),
    m_bNewEntryMode(bNewEntry),
    m_bNameAllowed(sal_True)
{
    FreeResource();
    Point aFLPos(aEntriesFL.GetPosPixel());
    Point aTL1(aFLPos);
    Size aFLSz(aEntriesFL.GetSizePixel().Width(), GetSizePixel().Height());
    long nControlSpace = aFLSz.Width() / 4;
    long nControlWidth = nControlSpace - 2 * aTL1.X();
    aTL1.X() *= 2;
    aTL1.Y() *= 5;
    Point aTR1(aTL1);
    aTR1.X() += nControlSpace;
    Point aTL2(aTR1);
    aTL2.X() += nControlSpace;
    Point aTR2(aTL2);
    aTR2.X() += nControlSpace;
    Size aFixedTextSize(aFLSz);
    Size aTmpSz(8,10);
    aTmpSz = LogicToPixel(aTmpSz, MAP_APPFONT);
    aFixedTextSize.Height() = aTmpSz.Width();
    Size aEditSize(aFixedTextSize);
    aFixedTextSize.Width() = nControlWidth + aFLPos.X();
    aEditSize.Height() = aTmpSz.Height();
    aEditSize.Width() = nControlWidth;

    sal_uInt16 nOffset = aTmpSz.Width() * 3 / 2;
    sal_Bool bLeft = sal_True;
    Window* pRefWindow = 0;
    for(sal_uInt16 nIndex = 0; nIndex < AUTH_FIELD_END; nIndex++)
    {
        const TextInfo aCurInfo = aTextInfoArr[nIndex];

        pFixedTexts[nIndex] = new FixedText(this);
        if(nIndex)
            pFixedTexts[nIndex]->SetZOrder( pRefWindow, WINDOW_ZORDER_BEHIND );
        else
            pFixedTexts[nIndex]->SetZOrder( 0, WINDOW_ZORDER_FIRST );

        pRefWindow = pFixedTexts[nIndex];

        pFixedTexts[nIndex]->SetSizePixel(aFixedTextSize);
        pFixedTexts[nIndex]->SetPosPixel(bLeft ? aTL1 : aTL2);
        pFixedTexts[nIndex]->SetText(SW_RES(STR_AUTH_FIELD_START + aCurInfo.nToxField));
        pFixedTexts[nIndex]->Show();
        pEdits[nIndex] = 0;
        if( AUTH_FIELD_AUTHORITY_TYPE == aCurInfo.nToxField )
        {
            pTypeListBox = new ListBox(this, WB_DROPDOWN|WB_BORDER);
            pTypeListBox->SetZOrder( pRefWindow, WINDOW_ZORDER_BEHIND );
            pRefWindow = pTypeListBox;
            for(sal_uInt16 j = 0; j < AUTH_TYPE_END; j++)
                pTypeListBox->InsertEntry(String(SW_RES(STR_AUTH_TYPE_START + j)));
            if(pFields[aCurInfo.nToxField].Len())
            {
                sal_uInt16 nIndexPos = pFields[aCurInfo.nToxField].ToInt32();
                pTypeListBox->SelectEntryPos(nIndexPos);
            }
            Size aTmp(aEditSize);
            aTmp.Height() *= 4;
            pTypeListBox->SetSizePixel(aTmp);
            pTypeListBox->SetPosPixel(bLeft ? aTR1 : aTR2);
            pTypeListBox->Show();
            pTypeListBox->SetSelectHdl(LINK(this, SwCreateAuthEntryDlg_Impl, EnableHdl));
            pTypeListBox->SetHelpId(aCurInfo.nHelpId);

        }
        else if(AUTH_FIELD_IDENTIFIER == aCurInfo.nToxField && !m_bNewEntryMode)
        {
            pIdentifierBox = new ComboBox(this, WB_BORDER|WB_DROPDOWN);
            pIdentifierBox->SetZOrder( pRefWindow, WINDOW_ZORDER_BEHIND );
            pRefWindow = pIdentifierBox;

            pIdentifierBox->SetSelectHdl(LINK(this,
                                    SwCreateAuthEntryDlg_Impl, IdentifierHdl));


            const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                        rSh.GetFldType(RES_AUTHORITY, aEmptyStr);
            if(pFType)
            {
                SvStringsDtor aIds;
                pFType->GetAllEntryIdentifiers(	aIds );
                for(sal_uInt16 n = 0; n < aIds.Count(); n++)
                    pIdentifierBox->InsertEntry(*aIds.GetObject(n));
            }
            pIdentifierBox->SetText(pFields[aCurInfo.nToxField]);
            Size aTmp(aEditSize);
            aTmp.Height() *= 4;
            pIdentifierBox->SetSizePixel(aTmp);
            pIdentifierBox->SetPosPixel(bLeft ? aTR1 : aTR2);
            pIdentifierBox->Show();
            pIdentifierBox->SetHelpId(aCurInfo.nHelpId);
        }
        else
        {
            pEdits[nIndex] = new Edit(this, WB_BORDER);
            pEdits[nIndex]->SetZOrder( pRefWindow, WINDOW_ZORDER_BEHIND );
            pRefWindow = pEdits[nIndex];
            pEdits[nIndex]->SetSizePixel(aEditSize);
            pEdits[nIndex]->SetPosPixel(bLeft ? aTR1 : aTR2);
            pEdits[nIndex]->SetText(pFields[aCurInfo.nToxField]);
            pEdits[nIndex]->Show();
            pEdits[nIndex]->SetHelpId(aCurInfo.nHelpId);
            if(AUTH_FIELD_IDENTIFIER == aCurInfo.nToxField)
            {
                pEdits[nIndex]->SetModifyHdl(LINK(this, SwCreateAuthEntryDlg_Impl, ShortNameHdl));
                m_bNameAllowed = pFields[nIndex].Len() > 0;
                if(!bCreate)
                {
                    pFixedTexts[nIndex]->Enable(sal_False);
                    pEdits[nIndex]->Enable(sal_False);
                }
            }
        }
        if(bLeft)
        {
            aTL1.Y() += nOffset;
            aTR1.Y() += nOffset;
        }
        else
        {
            aTL2.Y() += nOffset;
            aTR2.Y() += nOffset;
        }
        bLeft = !bLeft;
    }
    EnableHdl(pTypeListBox);

    long nHeightDiff = - aFLSz.Height();
    aFLSz.Height() = aTL1.Y();
    nHeightDiff += aFLSz.Height();
//    aEntriesFL.SetSizePixel(aFLSz);
    Size aDlgSize(GetSizePixel());
    aDlgSize.Height() += nHeightDiff;
    SetSizePixel(aDlgSize);

}
/* -----------------16.09.99 14:47-------------------

 --------------------------------------------------*/
SwCreateAuthEntryDlg_Impl::~SwCreateAuthEntryDlg_Impl()
{
    for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
    {
        delete pFixedTexts[i];
        delete pEdits[i];
    }
    delete pTypeListBox;
    delete pIdentifierBox;
}
/* -----------------16.09.99 14:27-------------------

 --------------------------------------------------*/
String	SwCreateAuthEntryDlg_Impl::GetEntryText(ToxAuthorityField eField) const
{
    String sRet;
    if( AUTH_FIELD_AUTHORITY_TYPE == eField )
    {
        DBG_ASSERT(pTypeListBox, "No ListBox")
        sRet = String::CreateFromInt32(pTypeListBox->GetSelectEntryPos());
    }
    else if( AUTH_FIELD_IDENTIFIER == eField && !m_bNewEntryMode)
    {
        DBG_ASSERT(pIdentifierBox, "No ComboBox")
        sRet = pIdentifierBox->GetText();
    }
    else
    {
        for(sal_uInt16 nIndex = 0; nIndex < AUTH_FIELD_END; nIndex++)
        {
            const TextInfo aCurInfo = aTextInfoArr[nIndex];
            if(aCurInfo.nToxField == eField)
            {
                sRet = pEdits[nIndex]->GetText();
                break;
            }
        }
    }
    return sRet;
}
/* -----------------21.09.99 13:54-------------------

 --------------------------------------------------*/
IMPL_LINK(SwCreateAuthEntryDlg_Impl, IdentifierHdl, ComboBox*, pBox)
{
    const SwAuthorityFieldType* pFType = (const SwAuthorityFieldType*)
                                rWrtSh.GetFldType(RES_AUTHORITY, aEmptyStr);
    if(pFType)
    {
        const SwAuthEntry* pEntry = pFType->GetEntryByIdentifier(
                                                        pBox->GetText());
        if(pEntry)
        {
            for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
            {
                if(AUTH_FIELD_IDENTIFIER == i)
                    continue;
                if(AUTH_FIELD_AUTHORITY_TYPE == i)
                    pTypeListBox->SelectEntry(
                                pEntry->GetAuthorField((ToxAuthorityField)i));
                else
                    pEdits[i]->SetText(
                                pEntry->GetAuthorField((ToxAuthorityField)i));
            }
        }
    }
    return 0;
}
/* -----------------------------20.12.99 15:07--------------------------------

 ---------------------------------------------------------------------------*/

IMPL_LINK(SwCreateAuthEntryDlg_Impl, ShortNameHdl, Edit*, pEdit)
{
    if(aShortNameCheckLink.IsSet())
    {
        sal_Bool bEnable = 0 != aShortNameCheckLink.Call(pEdit);
        m_bNameAllowed |= bEnable;
        aOKBT.Enable(pTypeListBox->GetSelectEntryCount() && bEnable);
    }
    return 0;
}
/* -----------------------------20.12.99 15:54--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwCreateAuthEntryDlg_Impl, EnableHdl, ListBox*, pBox)
{
    aOKBT.Enable(m_bNameAllowed && pBox->GetSelectEntryCount());
    return 0;
};
/* -----------------06.10.99 10:00-------------------

 --------------------------------------------------*/
SwAuthMarkFloatDlg::SwAuthMarkFloatDlg(SfxBindings* pBindings,
                                   SfxChildWindow* pChild,
                                   Window *pParent,
                                SfxChildWinInfo* pInfo,
                                   sal_Bool bNew) :
    SfxModelessDialog(pBindings, pChild, pParent, SW_RES(DLG_INSAUTHMARK)),
    aDlg(this, ResId(WIN_DLG), bNew)
{
    FreeResource();
    Initialize(pInfo);
    SwWrtShell* pWrtShell = ::GetActiveWrtShell();
    DBG_ASSERT(pWrtShell, "No shell?")
    aDlg.ReInitDlg(*pWrtShell);
}
/* -----------------06.10.99 10:27-------------------

 --------------------------------------------------*/
void	SwAuthMarkFloatDlg::Activate()
{
    SfxModelessDialog::Activate();
    aDlg.Activate();
}

void SwAuthMarkFloatDlg::ReInitDlg(SwWrtShell& rWrtShell)
{
    aDlg.ReInitDlg( rWrtShell );
}

/* -----------------06.10.99 10:35-------------------

 --------------------------------------------------*/
SwAuthMarkModalDlg::SwAuthMarkModalDlg(Window *pParent, SwWrtShell& rSh) :
    SvxStandardDialog(pParent, SW_RES(DLG_EDIT_AUTHMARK)),
    aDlg(this, ResId(WIN_DLG), sal_False)
{
    FreeResource();
    aDlg.ReInitDlg(rSh);
}
/* -----------------06.10.99 10:46-------------------

 --------------------------------------------------*/
void	SwAuthMarkModalDlg::Apply()
{
    aDlg.InsertHdl(0);
}

