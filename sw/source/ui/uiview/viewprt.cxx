/*************************************************************************
 *
 *  $RCSfile: viewprt.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:49 $
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

#if STLPORT_VERSION>=321
#include <cstdarg>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif
#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif


#ifndef _SFXFLAGITEM_HXX //autogen
#include <svtools/flagitem.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef NOOLDSV //autogen
#include <vcl/system.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_PRINTDLG_HXX_ //autogen
#include <svtools/printdlg.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _SFX_PRNMON_HXX //autogen
#include <sfx2/prnmon.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _OFA_MISCCFG_HXX //autogen
#include <sfx2/misccfg.hxx>
#endif
#ifndef _SVX_PRTQRY_HXX
#include <svx/prtqry.hxx>
#endif

#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _FINDER_HXX
#include <finder.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _PRTOPT_HXX
#include <prtopt.hxx>
#endif
#ifndef _SWPRTOPT_HXX
#include <swprtopt.hxx>
#endif
#ifndef _OPTPAGE_HXX
#include <optpage.hxx>
#endif
#ifndef _CFGITEMS_HXX
#include <cfgitems.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif
#ifndef _VIEWFUNC_HXX
#include <viewfunc.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _WVIEW_HXX
#include <wview.hxx>
#endif

#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _VIEW_HRC
#include <view.hrc>
#endif


/*--------------------------------------------------------------------
    Beschreibung:	Drucker an Sfx uebergeben
 --------------------------------------------------------------------*/


SfxPrinter* __EXPORT SwView::GetPrinter( BOOL bCreate )
{
    SfxPrinter *pOld = GetWrtShell().GetPrt( FALSE );
    SfxPrinter *pPrt = GetWrtShell().GetPrt( bCreate );
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

void SetPrinter( SfxPrinter* pNew, BOOL bWeb )
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
//		Wir lassen die Druckoptionen so, wie sie sind.
//		pAddPrinterAttr->SetPrintOptions( pOpt );
        if( pAddPrinterAttr->GetFax().Len() )
            pOpt->SetFaxName(pAddPrinterAttr->GetFax());
    }
}


USHORT __EXPORT SwView::SetPrinter(SfxPrinter* pNew, USHORT nDiffFlags )
{
    SwWrtShell &rSh = GetWrtShell();
    if ( (SFX_PRINTER_JOBSETUP | SFX_PRINTER_PRINTER) & nDiffFlags )
    {
        rSh.SetPrt( pNew );
        if ( nDiffFlags & SFX_PRINTER_PRINTER )
            rSh.SetModified();
    }
    BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
    if ( nDiffFlags & SFX_PRINTER_OPTIONS )
        ::SetPrinter( pNew, bWeb );

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
    Beschreibung:	Drucken
 --------------------------------------------------------------------*/

void MakeOptions( PrintDialog* pDlg, SwPrtOptions& rOpts, BOOL* pPrtProspect,
                  BOOL bWeb, SfxPrinter* pPrt )
{
    const SwAddPrinterItem* pAddPrinterAttr;
    if( pPrt && SFX_ITEM_SET == pPrt->GetOptions().GetItemState(
        FN_PARAM_ADDPRINTER, FALSE, (const SfxPoolItem**)&pAddPrinterAttr ))
    {
        rOpts.bPrintGraph		= pAddPrinterAttr->IsPrintGraphic  ();
        rOpts.bPrintTable		= pAddPrinterAttr->IsPrintTable    ();
        rOpts.bPrintDraw		= pAddPrinterAttr->IsPrintDraw     ();
        rOpts.bPrintControl		= pAddPrinterAttr->IsPrintControl  ();
        rOpts.bPrintLeftPage	= pAddPrinterAttr->IsPrintLeftPage ();
        rOpts.bPrintRightPage	= pAddPrinterAttr->IsPrintRightPage();
        rOpts.bPrintReverse		= pAddPrinterAttr->IsPrintReverse  ();
        rOpts.bPaperFromSetup	= pAddPrinterAttr->IsPaperFromSetup();
        rOpts.bPrintPageBackground = pAddPrinterAttr->IsPrintPageBackground();
        rOpts.bPrintBlackFont	= pAddPrinterAttr->IsPrintBlackFont();
        rOpts.bSinglePrtJobs	= pAddPrinterAttr->IsPrintSingleJobs();
        rOpts.nPrintPostIts		= pAddPrinterAttr->GetPrintPostIts ();

        if( pPrtProspect )
            *pPrtProspect = pAddPrinterAttr->IsPrintProspect();
    }
    else
    {
        SwPrintOptions* pOpts = SW_MOD()->GetPrtOptions(bWeb);

        rOpts.bPrintGraph     = pOpts->IsPrintGraphic  ();
        rOpts.bPrintTable     = pOpts->IsPrintTable    ();
        rOpts.bPrintDraw      = pOpts->IsPrintDraw     ();
        rOpts.bPrintControl   = pOpts->IsPrintControl  ();
        rOpts.bPrintLeftPage  = pOpts->IsPrintLeftPage ();
        rOpts.bPrintRightPage = pOpts->IsPrintRightPage();
        rOpts.bPrintReverse   = pOpts->IsPrintReverse  ();
        rOpts.bPaperFromSetup = pOpts->IsPaperFromSetup();
        rOpts.bPrintPageBackground = pOpts->IsPrintPageBackground();
        rOpts.bPrintBlackFont 	= pOpts->IsPrintBlackFont();
        rOpts.bSinglePrtJobs	= pOpts->IsPrintSingleJobs();
        rOpts.nPrintPostIts		= pOpts->GetPrintPostIts ();

        if( pPrtProspect )
            *pPrtProspect = pOpts->IsPrintProspect();
    }
    rOpts.aMulti.SetTotalRange( Range( 0, RANGE_MAX ) );
    rOpts.aMulti.SelectAll( FALSE );
    rOpts.nCopyCount = 1;
    rOpts.bCollate = FALSE;
    rOpts.bPrintSelection = FALSE;
    rOpts.bJobStartet = FALSE;

    if ( pDlg )
    {
        rOpts.nCopyCount = pDlg->GetCopyCount();
        rOpts.bCollate = pDlg->IsCollateChecked();
        if ( pDlg->GetCheckedRange() == PRINTDIALOG_SELECTION )
        {
            rOpts.aMulti.SelectAll();
            rOpts.bPrintSelection = TRUE;
        }
        else if ( PRINTDIALOG_ALL == pDlg->GetCheckedRange() )
            rOpts.aMulti.SelectAll();
        else
        {
            rOpts.aMulti = MultiSelection( pDlg->GetRangeText() );
            rOpts.aMulti.SetTotalRange( Range( 0, RANGE_MAX ) );
        }
    }
    else
        rOpts.aMulti.SelectAll();
    rOpts.aMulti.Select( 0, FALSE );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

ErrCode	SwView::DoPrint( SfxPrinter *pPrinter, PrintDialog *pDlg,
                                     BOOL bSilent )
{
    // First test
    SwWrtShell* pSh = &GetWrtShell();
    SwNewDBMgr* pMgr = pSh->GetNewDBMgr();

    int bPrintSelection = -1;
    if( DBMGR_MERGE_MAILMERGE != pMgr->GetMergeType() && !pDlg && !bSilent
        && !bIsApi && ( pSh->IsSelection() || pSh->IsFrmSelected() ||
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

    // Drucker starten
    PreparePrint( pDlg );
    SfxObjectShell *pObjShell = GetViewFrame()->GetObjectShell();
    SwPrtOptions aOpts( pObjShell->GetTitle(0) );
    BOOL bStartJob;
    BOOL bWeb = 0 != PTR_CAST(SwWebView, this);
    if( pMgr->GetMergeType() == DBMGR_MERGE_MAILMERGE )
    {
        ::MakeOptions( pDlg, aOpts, 0, bWeb, GetPrinter() );
#ifdef REPLACE_OFADBMGR
        bStartJob = pMgr->MergePrint( *this, aOpts, *pProgress );
#else
        bStartJob = pMgr->MergePrint( *this, aOpts, *pProgress );
#endif
    }
    else
    {
        //BrowseView abschalten und die View gegen alle Paints locken.
        FASTBOOL bBrowse = pSh->IsBrowseMode();
        SfxAllItemSet aSet( SFX_APP()->GetPool() );
        SfxBoolItem aBrowse( SID_BROWSER_MODE, FALSE );
        if ( bBrowse )
        {
            if ( pSh->GetWin() )
                pSh->GetWin()->Update();
            pSh->LockPaint();
            pSh->LockView( TRUE );
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

        BOOL bPrtPros;
        ::MakeOptions( pDlg, aOpts, &bPrtPros, bWeb, GetPrinter() );
        if( -1 != bPrintSelection )
            aOpts.bPrintSelection = 0 != bPrintSelection;

        SfxViewShell::Print(*pProgress);
        if( bPrtPros )
        {
            bStartJob = pPrinter->StartJob( aOpts.GetJobName() );
            if( bStartJob )
                pSh->PrintProspect( aOpts, *pProgress );
        }
        else
            bStartJob = pSh->Prt( aOpts, *pProgress );

        if ( bBrowse )
        {
            aBrowse.SetValue( TRUE );
            aSet.Put( aBrowse, aBrowse.Which() );
            SfxRequest aReq( SID_BROWSER_MODE, 0, aSet );
            GetDocShell()->Execute( aReq );
            pSh->LockView( FALSE );
            pSh->UnlockPaint();
        }
    }

    bIsApi = FALSE;
    if ( !bStartJob )
    {
        // Printer konnte nicht gestartet werden
        delete pProgress;
        return ERRCODE_IO_ABORT;
    }

    pProgress->Stop();
    pProgress->DeleteOnEndPrint();
    pPrinter->EndJob();
    return pPrinter->GetError();
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
    PrintDialog *pDlg =
        new PrintDialog( pParent );
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
            SwPrintOptions* pOpt = SW_MOD()->GetPrtOptions(bWeb);
            String sFaxName(pOpt->GetFaxName());
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
            SwViewOption* pOrgViewOption = 0;
            if (pSh->GetViewOptions()->IsFldName() && pSh->IsAnyFieldInDoc())
            {
                QueryBox aBox( &GetEditWin(), SW_RES( DLG_PRT_FIELDNAME ) );
                USHORT nRet = aBox.Execute();
                if( RET_CANCEL == nRet)
                    return;
                // disable field commands
                if( RET_NO == nRet )
                {
                    pOrgViewOption = new SwViewOption(*pSh->GetViewOptions());
                    pOrgViewOption->SetFldName(FALSE);
                    SW_MOD()->ApplyUsrPref(*pOrgViewOption, this, VIEWOPT_DEST_VIEW_ONLY );
                }
            }
            bIsApi = rReq.IsAPI();
            SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
            if(pOrgViewOption)
            {
                pOrgViewOption->SetFldName(TRUE);
                SW_MOD()->ApplyUsrPref(*pOrgViewOption, this, VIEWOPT_DEST_VIEW_ONLY );
                delete pOrgViewOption;
            }
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
    SwAddPrinterTabPage* pPage = ( SwAddPrinterTabPage* )
                            SwAddPrinterTabPage::Create(pParent, rOptions);
    pPage->SetPreview(bPreview);

    SvStringsDtor aFaxList;
    const USHORT nCount = Printer::GetQueueCount();
    pPage->Reset(rOptions);
    for (USHORT i = 0; i < nCount; ++i)
    {
        String* pString = new String( Printer::GetQueueInfo( i ).GetPrinterName() );
        aFaxList.Insert(pString, 0);
    }
    pPage->SetFax( aFaxList );
    return pPage;
}


void SetAppPrintOptions( ViewShell* pSh, BOOL bWeb )
{
    SwPrintOptions* pOpt = SW_MOD()->GetPrtOptions(bWeb);
    if( pSh && pSh->GetPrt() && pOpt)
    {
        // Applikationseigene Druckoptionen in SfxPrinter schiessen
        SwAddPrinterItem aAddPrinterItem (FN_PARAM_ADDPRINTER, pOpt);
        SfxItemSet aSet( pSh->GetAttrPool(),
                    FN_PARAM_ADDPRINTER, 		FN_PARAM_ADDPRINTER,
                    SID_HTML_MODE,				SID_HTML_MODE,
                    SID_PRINTER_NOTFOUND_WARN, 	SID_PRINTER_NOTFOUND_WARN,
                    SID_PRINTER_CHANGESTODOC, 	SID_PRINTER_CHANGESTODOC,
                    0 );

            OfaMiscCfg* pMisc = SFX_APP()->GetMiscConfig();

        if(bWeb)
            aSet.Put(SfxUInt16Item(SID_HTML_MODE,
                    ::GetHtmlMode(((SwWrtShell*)pSh)->GetView().GetDocShell())));
        aSet.Put(SfxBoolItem(SID_PRINTER_NOTFOUND_WARN,
                        pMisc->IsNotFoundWarning() ));
        aSet.Put(aAddPrinterItem);
        aSet.Put( SfxFlagItem( SID_PRINTER_CHANGESTODOC,
            (pMisc->IsPaperSizeWarning() ? SFX_PRINTER_CHG_SIZE : 0)   |
            (pMisc->IsPaperBinWarning()  ? SFX_PRINTER_CHG_ORIENTATION : 0 )));

        pSh->GetPrt()->SetOptions( aSet );
    }

}

/*--------------------------------------------------------------------

$Log: not supported by cvs2svn $
Revision 1.143  2000/09/18 16:06:14  willem.vandorp
OpenOffice header added.

Revision 1.142  2000/09/07 15:59:33  os
change: SFX_DISPATCHER/SFX_BINDINGS removed

Revision 1.141  2000/09/07 08:24:35  os
FaxName now in SwPrintOptions

Revision 1.140  2000/08/10 07:36:58  os
#77466# CreateFromInt32

Revision 1.139  2000/07/18 12:50:09  os
replace ofadbmgr

Revision 1.138  2000/07/10 07:02:56  os
replace ofadbmgr

Revision 1.137  2000/07/07 15:25:44  os
replace ofadbmgr

Revision 1.136  2000/05/26 07:21:35  os
old SW Basic API Slots removed

Revision 1.135  2000/03/03 15:17:04  os
StarView remainders removed

Revision 1.134  2000/02/11 14:59:27  hr
#70473# changes for unicode ( patched by automated patchtool )

Revision 1.133  2000/02/01 14:11:31  os
#72527# print selection query box

Revision 1.132  1999/12/14 14:32:05  jp
Bug #69595#: print can create single Jobs

Revision 1.131  1999/11/11 15:09:57  hr
#65293#: STLPORT 3.2.1

Revision 1.130  1999/09/10 08:35:03  os
restore ViewOption after printing

Revision 1.129  1999/09/09 07:13:27  os
Query changed: printing documents with field commands

Revision 1.128  1999/08/11 14:12:24  JP
Bug #68171#: DoPrint - donst change selektionflag if not execute the Sel.Dlg


      Rev 1.127   11 Aug 1999 16:12:24   JP
   Bug #68171#: DoPrint - donst change selektionflag if not execute the Sel.Dlg

      Rev 1.126   09 Jul 1999 13:54:30   JP
   Bug #67535#: DoPrint - PrintSelection Dialog before any action started

      Rev 1.125   01 Mar 1999 16:22:16   MA
   #62490# Altlast entfernt (Drucken und Briefumschlaege/Etiketten und Datenbank)

      Rev 1.124   04 Feb 1999 11:22:16   ER
   #61415# OfaMiscCfg nach SFX verschoben

      Rev 1.123   13 Jan 1999 11:53:12   JP
   Bug #60794#: Fehlererkennung beim Tabellenrechnen

      Rev 1.122   29 Oct 1998 11:52:44   OM
   #58593# Selektion nur anbieten, wenn auch vorhanden

      Rev 1.121   19 Oct 1998 11:34:48   OM
   #58017# Falschen Hilfetext entfernt

      Rev 1.120   29 Sep 1998 16:58:30   OM
   #57214# Fokus von Controls nicht als Selektion interpretieren

      Rev 1.119   08 Sep 1998 09:59:24   MI
   #55602# DocInfo vor Ausdruck setzen und ggf. hinterher resetten (ErrorCode bei Abort)

      Rev 1.118   31 Aug 1998 17:48:56   OM
   #55613# Selektion drucken

      Rev 1.117   25 Aug 1998 08:25:28   OS
   SfxPrintProgress ermittelt den aktuellen Drucker der ViewShell -> fuer Fax spaeter erzeugen #55210#

      Rev 1.116   13 Jul 1998 14:42:30   TJ
   include

      Rev 1.115   13 Jul 1998 08:48:42   OS
   Dialog-Parent richtig setzen #51814#

      Rev 1.114   07 Jul 1998 14:25:54   AMA
   Chg: DoPrint uebernimmt das Drucken

      Rev 1.113   03 Jul 1998 15:04:32   AMA
   Chg: DoPrint uebernimmt das Drucken

      Rev 1.112   24 Jun 1998 16:17:42   OM
   #51575# Printoptionen auch in Seitenvorschau beachten

      Rev 1.111   21 Nov 1997 15:00:26   MA
   includes

      Rev 1.110   02 Oct 1997 16:29:14   TJ
   include

      Rev 1.109   30 Sep 1997 16:57:50   TJ
   include

      Rev 1.108   03 Sep 1997 15:55:34   OS
   DLL-Umbau

      Rev 1.107   02 Sep 1997 09:56:28   OM
   SDB-Headeranpassung

      Rev 1.106   08 Aug 1997 17:26:22   OM
   Headerfile-Umstellung

      Rev 1.105   21 Jul 1997 17:16:20   AMA
   Fix #38434#: Das FontListen-Update wird jetzt von SetPrt angestossen

      Rev 1.104   11 Jul 1997 14:47:00   OM
   #41525# Nur durch SQL-Statement eingeschraenkte Datensaetze verwenden

      Rev 1.103   08 Jul 1997 14:06:12   OS
   ConfigItems von der App ans Module

      Rev 1.102   13 Jun 1997 12:11:30   MA
   #40665# sid_broiwse ist an der DocShell

      Rev 1.101   11 Jun 1997 09:18:22   OS
   FN_CONFIG_DLG ->SID_SW_EDITOPTIONS fuer Fax #40584#

      Rev 1.100   09 Jun 1997 14:27:58   MA
   chg: Browse-Flag nur noch am Doc

*/

