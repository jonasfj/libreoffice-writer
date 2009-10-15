/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewprt.cxx,v $
 * $Revision: 1.42 $
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

#include <com/sun/star/text/NotePrintMode.hpp>

#if STLPORT_VERSION>=321
#include <cstdarg>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif
#include <sfx2/request.hxx>

#include <sfx2/app.hxx>
#include <svl/flagitem.hxx>
#include <vcl/msgbox.hxx>
#include <svtools/printdlg.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/prnmon.hxx>
#include <svx/paperinf.hxx>
#include <sfx2/dispatch.hxx>
#include <unotools/misccfg.hxx>
#include <svx/prtqry.hxx>
#include <svx/svdview.hxx>
#include <svl/eitem.hxx>
#include <svl/stritem.hxx>
#include <svl/intitem.hxx>
#include <svl/flagitem.hxx>
#include <svx/linkmgr.hxx>

#include <modcfg.hxx>
#include <edtwin.hxx>
#include <view.hxx>
#include <wrtsh.hxx>
#include <docsh.hxx>
#include <viewopt.hxx>
#include <prtopt.hxx>
#include <swprtopt.hxx>
#include <fontcfg.hxx>
#include <cfgitems.hxx>
#include <dbmgr.hxx>
#include <docstat.hxx>
#include <viewfunc.hxx>
#include <swmodule.hxx>
#include <wview.hxx>
#include <doc.hxx>
#include <fldbas.hxx>

#include <globals.hrc>
#include <view.hrc>
#include <app.hrc>
#include <svl/eitem.hxx>
#include <swwrtshitem.hxx>
#include "swabstdlg.hxx"
#include <svl/slstitm.hxx>

#include <unomid.h>

using namespace ::com::sun::star;


/*--------------------------------------------------------------------
    Beschreibung:	Drucker an Sfx uebergeben
 --------------------------------------------------------------------*/


SfxPrinter* __EXPORT SwView::GetPrinter( BOOL bCreate )
{
    const IDocumentDeviceAccess* pIDDA = GetWrtShell().getIDocumentDeviceAccess();
    SfxPrinter *pOld = pIDDA->getPrinter( false );
    SfxPrinter *pPrt = pIDDA->getPrinter( bCreate );
    if ( pOld != pPrt )
    {
        BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
        ::SetAppPrintOptions( &GetWrtShell(), bWeb );
    }
    return pPrt;
}

/*--------------------------------------------------------------------
    Beschreibung:	Druckerwechsel weitermelden
 --------------------------------------------------------------------*/

void SetPrinter( IDocumentDeviceAccess* pIDDA, SfxPrinter* pNew, BOOL bWeb )
{
    SwPrintOptions* pOpt = SW_MOD()->GetPrtOptions(bWeb);
    if( !pOpt)
        return;

    // Applikationseigene Druckoptionen aus SfxPrinter auslesen
    const SfxItemSet& rSet = pNew->GetOptions();

    const SwAddPrinterItem* pAddPrinterAttr;
    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_ADDPRINTER, FALSE,
        (const SfxPoolItem**)&pAddPrinterAttr ) )
    {
        if( pIDDA )
            pIDDA->setPrintData( *pAddPrinterAttr );
        if( pAddPrinterAttr->GetFax().getLength() )
            pOpt->SetFaxName(pAddPrinterAttr->GetFax());
    }
}


USHORT __EXPORT SwView::SetPrinter(SfxPrinter* pNew, USHORT nDiffFlags, bool  )
{
    SwWrtShell &rSh = GetWrtShell();
    SfxPrinter* pOld = rSh.getIDocumentDeviceAccess()->getPrinter( false );
    if ( pOld && pOld->IsPrinting() )
        return SFX_PRINTERROR_BUSY;

    if ( (SFX_PRINTER_JOBSETUP | SFX_PRINTER_PRINTER) & nDiffFlags )
    {
        rSh.getIDocumentDeviceAccess()->setPrinter( pNew, true, true );
        if ( nDiffFlags & SFX_PRINTER_PRINTER )
            rSh.SetModified();
    }
    BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
    if ( nDiffFlags & SFX_PRINTER_OPTIONS )
        ::SetPrinter( rSh.getIDocumentDeviceAccess(), pNew, bWeb );

    const BOOL bChgOri = nDiffFlags & SFX_PRINTER_CHG_ORIENTATION ? TRUE : FALSE;
    const BOOL bChgSize= nDiffFlags & SFX_PRINTER_CHG_SIZE ? TRUE : FALSE;
    if ( bChgOri || bChgSize )
    {
        rSh.StartAllAction();
        if ( bChgOri )
            rSh.ChgAllPageOrientation( USHORT(pNew->GetOrientation()) );
        if ( bChgSize )
        {
            Size aSz( SvxPaperInfo::GetPaperSize( pNew ) );
            rSh.ChgAllPageSize( aSz );
        }
        rSh.SetModified();
        rSh.EndAllAction();
        InvalidateRulerPos();
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

ErrCode SwView::DoPrint( SfxPrinter *pPrinter, PrintDialog *pDlg, BOOL bSilent, BOOL bIsAPI )
{
    // First test
    SwWrtShell* pSh = &GetWrtShell();
    SwNewDBMgr* pMgr = pSh->GetNewDBMgr();

    int bPrintSelection = -1;
    USHORT nMergeType = pMgr->GetMergeType();
    if( DBMGR_MERGE_MAILMERGE != nMergeType &&
        DBMGR_MERGE_DOCUMENTS != nMergeType &&
            !pDlg &&
            !bIsAPI && ( pSh->IsSelection() || pSh->IsFrmSelected() ||
        pSh->IsObjSelected() ) )
    {
        short nBtn = SvxPrtQryBox(&GetEditWin()).Execute();
        if( RET_CANCEL == nBtn )
            return ERRCODE_IO_ABORT;

        if( RET_OK == nBtn )
            bPrintSelection = 1;
        else
            bPrintSelection = 0;
    }

// 	Der PrintProgress stellt Handler am Printer der ViewShell ein.
//	Das kann natuerlich erste nach dem evtl. Umschalten des Druckers korrekt
//	funktionieren. #55210#
//	SfxPrintProgress *pProgress = new SfxPrintProgress( this, !bSilent );
    SfxPrintProgress *pProgress = 0;
    SfxPrinter *pDocPrinter = GetPrinter(TRUE);
    if ( !pPrinter )
        pPrinter = pDocPrinter;
    else if ( pDocPrinter != pPrinter )
    {
        //Da der Doc-Drucker beim SetPrinter geloescht wird,
        // muss man ihn vorher clonen
        SfxPrinter* pClone = pDocPrinter->Clone();
        SetPrinter( pPrinter, SFX_PRINTER_PRINTER );
        pProgress = new SfxPrintProgress( this, !bSilent );
        pProgress->RestoreOnEndPrint( pClone );
    }
    if(!pProgress)
        pProgress = new SfxPrintProgress( this, !bSilent );
    pProgress->SetWaitMode(FALSE);

    BOOL bStartJob = pPrinter->InitJob( &GetEditWin(),
            !bIsAPI && pSh->HasDrawView() && pSh->GetDrawView()->GetModel()->HasTransparentObjects());
    if( bStartJob )
    {
        // Drucker starten
        PreparePrint( pDlg );
        SfxObjectShell *pObjShell = GetViewFrame()->GetObjectShell();
        //#i82625# GetTitle( 0 ) doesn't work for 'embedded' documents like forms or reports
        String sDocumentTitle( pObjShell->GetTitle(SFX_TITLE_APINAME));
        SwPrtOptions aOpts( sDocumentTitle );
        BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
        nMergeType = pMgr->GetMergeType();

        BOOL bPrtPros;
        BOOL bPrtPros_RTL;
        SwView::MakeOptions( pDlg, aOpts, &bPrtPros, &bPrtPros_RTL, bWeb, GetPrinter(),
                        pSh->getIDocumentDeviceAccess()->getPrintData() );

        //set the appropriate view options to print
        //on silent mode the field commands have to be switched off always
        //on default print the user is asked what to do
        const SwViewOption* pCurrentViewOptions = pSh->GetViewOptions();
        bool bSwitchOff_IsFldName = pCurrentViewOptions->IsFldName() && pSh->IsAnyFieldInDoc();

        if(!bSilent && bSwitchOff_IsFldName)
        {
            QueryBox aBox( &GetEditWin(), SW_RES( DLG_PRT_FIELDNAME ) );
            USHORT nRet = aBox.Execute();
            if( RET_CANCEL == nRet)
                return ERRCODE_IO_ABORT;
            // disable field commands
            if( RET_NO != nRet )
            {
                bSwitchOff_IsFldName = false;
            }
        }
        bool bApplyViewOptions = bSwitchOff_IsFldName;
        //switch off display of hidden characters if on and hidden characters are in use
        const sal_Bool bOldShowHiddenChar = pCurrentViewOptions->IsShowHiddenChar();
        const sal_Bool bOldMetaChars = pCurrentViewOptions->IsViewMetaChars();
        if( bOldShowHiddenChar != aOpts.IsPrintHiddenText()
            && pSh->GetDoc()->ContainsHiddenChars())
            bApplyViewOptions |= true;
        //switch off display of hidden paragraphs if on and hidden paragraphs are in use
        const sal_Bool bOldShowHiddenPara = pCurrentViewOptions->IsShowHiddenPara();
        if( aOpts.IsPrintHiddenText() != bOldShowHiddenPara )
        {
            const SwFieldType* pFldType = pSh->GetDoc()->GetSysFldType(RES_HIDDENPARAFLD);
            if( pFldType && pFldType->GetDepends())
                bApplyViewOptions |= true;
        }
        const sal_Bool bOldShowHiddenField = pCurrentViewOptions->IsShowHiddenField();
        if( aOpts.IsPrintHiddenText() != bOldShowHiddenField )
        {
            const SwFieldType* pFldType = pSh->GetDoc()->GetSysFldType(RES_HIDDENTXTFLD);
            if( pFldType || pFldType->GetDepends())
                bApplyViewOptions |= true;
        }

        SwViewOption* pOrgViewOption = 0;
        bApplyViewOptions |= !aOpts.IsPrintTextPlaceholder();
        if(bApplyViewOptions)
        {
            pOrgViewOption = new SwViewOption(*pSh->GetViewOptions());
            if(bSwitchOff_IsFldName)
                pOrgViewOption->SetFldName(aOpts.IsPrintHiddenText());

            pOrgViewOption->SetShowHiddenChar(aOpts.IsPrintHiddenText());
            pOrgViewOption->SetViewMetaChars(sal_True);
            pOrgViewOption->SetShowHiddenPara(aOpts.IsPrintHiddenText());
            pOrgViewOption->SetShowHiddenField(aOpts.IsPrintHiddenText());
            pOrgViewOption->SetShowPlaceHolderFields( aOpts.bPrintTextPlaceholder );

            SW_MOD()->ApplyUsrPref(*pOrgViewOption, this, VIEWOPT_DEST_VIEW_ONLY );
        }

        if( nMergeType == DBMGR_MERGE_MAILMERGE ||
                DBMGR_MERGE_DOCUMENTS == nMergeType )
        {
            if(DBMGR_MERGE_DOCUMENTS == nMergeType)
                bStartJob = pMgr->MergePrintDocuments( *this, aOpts, *pProgress, bIsAPI );
            else
                bStartJob = pMgr->MergePrint( *this, aOpts, *pProgress, bIsAPI );
        }
        else
        {
            const BOOL bLockedView = pSh->IsViewLocked();
            pSh->LockView( TRUE );

            //BrowseView abschalten und die View gegen alle Paints locken.
            BOOL bBrowse = pSh->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE);
            SfxAllItemSet aSet( SFX_APP()->GetPool() );
            SfxBoolItem aBrowse( SID_BROWSER_MODE, FALSE );
            if ( bBrowse )
            {
                if ( pSh->GetWin() )
                    pSh->GetWin()->Update();
                pSh->LockPaint();
                aSet.Put( aBrowse, aBrowse.Which() );
                SfxRequest aReq( SID_BROWSER_MODE, 0, aSet );
                GetDocShell()->Execute( aReq );
            }

            // die Felder aktualisieren
            BOOL bIsModified = pSh->IsModified();
            pSh->StartAllAction();
            SwDocStat aDocStat;
            pSh->UpdateDocStat( aDocStat );
            pSh->EndAllTblBoxEdit();
            pSh->ViewShell::UpdateFlds(TRUE);
            if( pSh->IsCrsrInTbl() )
            {
                pSh->ClearTblBoxCntnt();
                pSh->SaveTblBoxCntnt();
            }
            pSh->EndAllAction();

            if( !bIsModified )
                pSh->ResetModified();

            if( -1 != bPrintSelection )
                aOpts.bPrintSelection = 0 != bPrintSelection;

            uno::Sequence< beans::PropertyValue> aViewProperties(18);
            beans::PropertyValue* pViewProperties =  aViewProperties.getArray();
            pViewProperties[1].Name = C2U("PrintGraphics");
            pViewProperties[1].Value <<= (sal_Bool)aOpts.IsPrintGraphic();
            pViewProperties[2].Name = C2U("PrintTables");
            pViewProperties[2].Value <<= (sal_Bool)aOpts.IsPrintTable();
            pViewProperties[3].Name = C2U("PrintDrawings");
            pViewProperties[3].Value <<= (sal_Bool)aOpts.IsPrintDraw();
            pViewProperties[4].Name = C2U("PrintLeftPages");
            pViewProperties[4].Value <<= (sal_Bool)aOpts.IsPrintLeftPage();
            pViewProperties[5].Name = C2U("PrintRightPages");
            pViewProperties[5].Value <<= (sal_Bool)aOpts.IsPrintRightPage();
            pViewProperties[6].Name = C2U("PrintControls");
            pViewProperties[6].Value <<= (sal_Bool)aOpts.IsPrintControl();
            pViewProperties[7].Name = C2U("PrintReversed");
            pViewProperties[7].Value <<= (sal_Bool)aOpts.IsPrintReverse();
            pViewProperties[8].Name = C2U("PrintPaperFromSetup");
            pViewProperties[8].Value <<= (sal_Bool)aOpts.IsPaperFromSetup();
            pViewProperties[9].Name = C2U("PrintFaxName");
            pViewProperties[9].Value <<= aOpts.GetFaxName();
            pViewProperties[10].Name = C2U("PrintAnnotationMode");
            pViewProperties[10].Value <<= (text::NotePrintMode) aOpts.GetPrintPostIts();
            pViewProperties[11].Name = C2U("PrintProspect");
            pViewProperties[11].Value <<= (sal_Bool)aOpts.IsPrintProspect();
            pViewProperties[12].Name = C2U("PrintPageBackground");
            pViewProperties[12].Value <<= (sal_Bool)aOpts.IsPrintPageBackground();
            pViewProperties[13].Name = C2U("PrintBlackFonts");
            pViewProperties[13].Value <<= (sal_Bool)aOpts.IsPrintBlackFont();
            pViewProperties[0].Name = C2U("IsSinglePrintJob");
            pViewProperties[0].Value <<= (sal_Bool)aOpts.IsPrintSingleJobs();
            pViewProperties[14].Name = C2U("Selection");
            pViewProperties[14].Value <<= (sal_Bool)aOpts.bPrintSelection;
            pViewProperties[15].Name = C2U("PrintEmptyPages");
            pViewProperties[15].Value <<= (sal_Bool)aOpts.bPrintEmptyPages;
            pViewProperties[16].Name = C2U("PrintHiddenText");
            pViewProperties[16].Value <<= (sal_Bool)aOpts.bPrintHiddenText;
            pViewProperties[17].Name = C2U("PrintTextPlaceholder");
            pViewProperties[17].Value <<= (sal_Bool)aOpts.bPrintTextPlaceholder;
            SetAdditionalPrintOptions(aViewProperties);

            SfxViewShell::Print(*pProgress, bIsAPI );
            if ( !pProgress->IsAborted() )
            {
                if( bPrtPros )
                {
                    bStartJob = pPrinter->StartJob( aOpts.GetJobName() );
                    if( bStartJob )
                        pSh->PrintProspect( aOpts, *pProgress, bPrtPros_RTL );
                }
                else
                    bStartJob = pSh->Prt( aOpts, pProgress );

                if ( bBrowse )
                {
                    aBrowse.SetValue( TRUE );
                    aSet.Put( aBrowse, aBrowse.Which() );
                    SfxRequest aReq( SID_BROWSER_MODE, 0, aSet );
                    GetDocShell()->Execute( aReq );
                    pSh->UnlockPaint();
                }
            }
            else
                bStartJob = FALSE;

            pSh->LockView( bLockedView );
        }
        if(pOrgViewOption)
        {
            if(bSwitchOff_IsFldName)
                pOrgViewOption->SetFldName(TRUE);
            pOrgViewOption->SetShowHiddenChar(bOldShowHiddenChar);
            pOrgViewOption->SetViewMetaChars(bOldMetaChars);
            pOrgViewOption->SetShowHiddenField(bOldShowHiddenField);
            pOrgViewOption->SetShowHiddenPara(bOldShowHiddenPara);
            //must to be set to sal_True anyway
            pOrgViewOption->SetShowPlaceHolderFields( sal_True );
            SW_MOD()->ApplyUsrPref(*pOrgViewOption, this, VIEWOPT_DEST_VIEW_ONLY );
            delete pOrgViewOption;
        }
    }

    if( !bStartJob )
    {
        // Printer konnte nicht gestartet werden
        delete pProgress;
        return pPrinter->GetError();
    }

    pProgress->Stop();
    if ( pPrinter->IsJobActive() )
    {
        pProgress->DeleteOnEndPrint();
        pPrinter->EndJob();
        return pPrinter->GetError();
    }
    else
    {
        // the next call might destroy pPrinter (in case it is not the usual document printer); so get the error before
        ULONG nError = pPrinter->GetError();
        pProgress->DeleteOnEndPrint();
        return nError;
    }
}



/*--------------------------------------------------------------------
    Beschreibung:	TabPage fuer applikationsspezifische Druckoptionen
 --------------------------------------------------------------------*/

SfxTabPage* __EXPORT SwView::CreatePrintOptionsPage(Window* pParent,
                                                    const SfxItemSet& rSet)
{
    return ::CreatePrintOptionsPage( pParent, rSet, FALSE );
}

/*--------------------------------------------------------------------
    Beschreibung:	Druckerdialog
 --------------------------------------------------------------------*/

PrintDialog* CreatePrintDialog( Window* pParent, USHORT nPg, SwWrtShell* pSh )
{
    PrintDialog *pDlg = new PrintDialog( pParent, false );
//	pDlg->ChangeFirstPage( 1 );

    if ( !nPg )
        nPg = 1;
//	pDlg->ChangeLastPage( nPg );
//	pDlg->ChangeMaxPage( 9999 );
    pDlg->EnableRange( PRINTDIALOG_FROMTO );

    if (pSh && (pSh->IsSelection() || pSh->IsFrmSelected() || pSh->IsObjSelected()))
        pDlg->EnableRange( PRINTDIALOG_SELECTION );

    pDlg->SetRangeText( String::CreateFromInt32(nPg) );
    pDlg->EnableRange( PRINTDIALOG_RANGE );
    pDlg->EnableCollate();
    return pDlg;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

PrintDialog* __EXPORT SwView::CreatePrintDialog( Window* pParent )
{
    // AMA: Hier sollte vielleicht die virtuelle Seitennummer angezeigt werden,
    // 		aber nur, wenn das Drucken virtuelle Seitennummern und nicht wie
    //		bisher (auch beim SWG 2.0) physikalische beachtet werden.
    return ::CreatePrintDialog( pParent, GetWrtShell().GetPhyPageNum(), &GetWrtShell() );
}

/*--------------------------------------------------------------------
    Beschreibung:	Print-Dispatcher
 --------------------------------------------------------------------*/

void __EXPORT SwView::ExecutePrint(SfxRequest& rReq)
{
    BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
    ::SetAppPrintOptions( &GetWrtShell(), bWeb );
    switch (rReq.GetSlot())
    {
        case FN_FAX:
        {
            SwPrintOptions* pPrintOptions = SW_MOD()->GetPrtOptions(bWeb);
            String sFaxName(pPrintOptions->GetFaxName());
            if (sFaxName.Len())
            {
                SfxStringItem aPrinterName(SID_PRINTER_NAME, sFaxName);
                SfxBoolItem aSilent( SID_SILENT, TRUE );
                GetViewFrame()->GetDispatcher()->Execute( SID_PRINTDOC,
                            SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD,
                            &aPrinterName, &aSilent, 0L );
            }
            else
            {
                InfoBox aInfoBox(&GetEditWin(), SW_RES(MSG_ERR_NO_FAX));
                String sMsg = aInfoBox.GetMessText();
                USHORT nResNo = bWeb ? STR_WEBOPTIONS : STR_TEXTOPTIONS;
                sMsg.SearchAndReplace(String::CreateFromAscii("%1"), String(SW_RES(nResNo)));
                aInfoBox.SetMessText(sMsg);
                aInfoBox.Execute();
                SfxUInt16Item aDefPage(SID_SW_EDITOPTIONS, TP_OPTPRINT_PAGE);
                GetViewFrame()->GetDispatcher()->Execute(SID_SW_EDITOPTIONS,
                            SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD,
                            &aDefPage, 0L );
            }
        }
        break;
        case SID_PRINTDOC:
        case SID_PRINTDOCDIRECT:
        {
            SwWrtShell* pSh = &GetWrtShell();
            SFX_REQUEST_ARG(rReq, pSilentItem, SfxBoolItem, SID_SILENT, FALSE);
            BOOL bSilent = pSilentItem ? pSilentItem->GetValue() : FALSE;
            SFX_REQUEST_ARG(rReq, pPrintFromMergeItem, SfxBoolItem, FN_QRY_MERGE, FALSE);
            if(pPrintFromMergeItem)
                rReq.RemoveItem(FN_QRY_MERGE);
            BOOL bFromMerge = pPrintFromMergeItem ? pPrintFromMergeItem->GetValue() : FALSE;
            SwMiscConfig aMiscConfig;
            if(!bSilent && !bFromMerge &&
                    SW_MOD()->GetModuleConfig()->IsAskForMailMerge() && pSh->IsAnyDatabaseFieldInDoc())
            {
                QueryBox aBox( &GetEditWin(), SW_RES( MSG_PRINT_AS_MERGE ));
                short nRet = aBox.Execute();
                if(RET_YES == nRet)
                {
                    SfxBoolItem aBool(FN_QRY_MERGE, TRUE);
                    GetViewFrame()->GetDispatcher()->Execute(
                                FN_QRY_MERGE, SFX_CALLMODE_ASYNCHRON, &aBool, 0L);
                    rReq.Ignore();
                    return;
                }
            }
            //#i61455# if master documentes are printed silently without loaded links then update the links now
            if( bSilent && pSh->IsGlobalDoc() && !pSh->IsGlblDocSaveLinks() )
            {
                pSh->GetLinkManager().UpdateAllLinks( sal_False, sal_False, sal_False, 0 );
            }
            SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
            return;
        }
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Page Drucker/Zusaetze erzeugen fuer SwView und
                    SwPagePreview
 --------------------------------------------------------------------*/

SfxTabPage* CreatePrintOptionsPage( Window *pParent,
                                const SfxItemSet &rOptions, BOOL bPreview )
{
    SfxTabPage* pPage = NULL;
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    if ( pFact )
    {
        ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( TP_OPTPRINT_PAGE );
        if ( fnCreatePage )
            pPage = (*fnCreatePage)( pParent, rOptions );
    }
    SfxAllItemSet aSet(*(rOptions.GetPool()));
    aSet.Put (SfxBoolItem(SID_PREVIEWFLAG_TYPE, bPreview));
    aSet.Put (SfxBoolItem(SID_FAX_LIST, sal_True));
    pPage->PageCreated(aSet);
    return pPage;
}


void SetAppPrintOptions( ViewShell* pSh, BOOL bWeb )
{
    SwPrintData aPrtData = *SW_MOD()->GetPrtOptions(bWeb);
    const IDocumentDeviceAccess* pIDDA = pSh->getIDocumentDeviceAccess();
    SwPrintData* pShellPrintData = pIDDA->getPrintData();

    if(pShellPrintData)
        aPrtData = *pShellPrintData;

    if( pIDDA->getPrinter( false ) )
    {
        // Applikationseigene Druckoptionen in SfxPrinter schiessen
        SwAddPrinterItem aAddPrinterItem (FN_PARAM_ADDPRINTER, aPrtData);
        SfxItemSet aSet( pSh->GetAttrPool(),
                    FN_PARAM_ADDPRINTER, 		FN_PARAM_ADDPRINTER,
                    SID_HTML_MODE,				SID_HTML_MODE,
                    SID_PRINTER_NOTFOUND_WARN, 	SID_PRINTER_NOTFOUND_WARN,
                    SID_PRINTER_CHANGESTODOC, 	SID_PRINTER_CHANGESTODOC,
                    0 );

        utl::MiscCfg aMisc;

        if(bWeb)
            aSet.Put(SfxUInt16Item(SID_HTML_MODE,
                    ::GetHtmlMode(((SwWrtShell*)pSh)->GetView().GetDocShell())));
        aSet.Put(SfxBoolItem(SID_PRINTER_NOTFOUND_WARN,
                        aMisc.IsNotFoundWarning() ));
        aSet.Put(aAddPrinterItem);
        aSet.Put( SfxFlagItem( SID_PRINTER_CHANGESTODOC,
            (aMisc.IsPaperSizeWarning() ? SFX_PRINTER_CHG_SIZE : 0)   |
            (aMisc.IsPaperOrientationWarning()  ? SFX_PRINTER_CHG_ORIENTATION : 0 )));

        pIDDA->getPrinter( true )->SetOptions( aSet );
    }

}

