/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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


#if STLPORT_VERSION>=321
#include <cstdarg>
#endif


#include <hintids.hxx>

#include <sfx2/request.hxx>
#include <svx/svxids.hrc>

#include <svtools/svmedit.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <sfx2/app.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/printer.hxx>
#include <vcl/msgbox.hxx>
#include <sfx2/dispatch.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/pbinitem.hxx>
#include <editeng/paperinf.hxx>
#include <editeng/brkitem.hxx>
#include <fmthdft.hxx>
#include <swwait.hxx>
#include <paratr.hxx>
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <docsh.hxx>
#include <frmatr.hxx>
#include <fldbas.hxx>
#include <swundo.hxx>
#include <IDocumentDeviceAccess.hxx>
#include <dbmgr.hxx>
#include <fmtcol.hxx>
#include <frmmgr.hxx>
#include <fldmgr.hxx>
#include <pagedesc.hxx>
#include <poolfmt.hxx>
#include <expfld.hxx>
#include <SwStyleNameMapper.hxx>
#include <crsskip.hxx>

#include <cmdid.h>
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _APP_HRC
#include <app.hrc>
#endif
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#include "swabstdlg.hxx"
#include "envelp.hrc"
#include "envimg.hxx"

#define ENV_NEWDOC		RET_OK
#define ENV_INSERT		RET_USER
#define ENV_CANCEL		SHRT_MAX


// --------------------------------------------------------------------------


// Funktion wird fuer Etiketten und Briefumschlaege benutzt!
//	im applab.cxx und appenv.cxx
String InsertLabEnvText( SwWrtShell& rSh, SwFldMgr& rFldMgr, const String& rText )
{
    String sRet;
    String aText(rText);
    aText.EraseAllChars( '\r' );


    USHORT nTokenPos = 0;
    while( STRING_NOTFOUND != nTokenPos )
    {
        String aLine = aText.GetToken( 0, '\n', nTokenPos );
        while ( aLine.Len() )
        {
            String sTmpText;
            BOOL bField = FALSE;

            USHORT nPos = aLine.Search( '<' );
            if ( nPos )
            {
                sTmpText = aLine.Copy( 0, nPos );
                aLine.Erase( 0, nPos );
//				sTmpText = aLine.Cut( 0, nPos );
            }
            else
            {
                nPos = aLine.Search( '>' );
                if ( nPos == STRING_NOTFOUND )
                {
                    sTmpText = aLine;
                    aLine.Erase();
//					sTmpText = aLine.Cut();
                }
                else
                {
                    sTmpText = aLine.Copy( 0, nPos + 1);
                    aLine.Erase( 0, nPos + 1);
//					sTmpText = aLine.Cut( 0, nPos + 1 );

                    // Datenbankfelder muesen mind. 3 Punkte beinhalten!
                    String sDBName( sTmpText.Copy( 1, sTmpText.Len() - 2));
                    USHORT nCnt = sDBName.GetTokenCount('.');
                    if (nCnt >= 3)
                    {
                        ::ReplacePoint(sDBName, TRUE);
                        SwInsertFld_Data aData(TYP_DBFLD, 0, sDBName, aEmptyStr, 0, &rSh );
                        rFldMgr.InsertFld( aData );
                        sRet = sDBName;
                        bField = TRUE;
                    }
                }
            }
            if ( !bField )
                rSh.Insert( sTmpText );
        }
        rSh.InsertLineBreak();
    }
    rSh.DelLeft();	// Letzten Linebreak wieder l???schen

    return sRet;
}

// ----------------------------------------------------------------------------


void lcl_CopyCollAttr(SwWrtShell* pOldSh, SwWrtShell* pNewSh, USHORT nCollId)
{
    USHORT nCollCnt = pOldSh->GetTxtFmtCollCount();
    SwTxtFmtColl* pColl;
    for( USHORT nCnt = 0; nCnt < nCollCnt; ++nCnt )
        if(nCollId == (pColl = &pOldSh->GetTxtFmtColl(nCnt))->GetPoolFmtId())
            pNewSh->GetTxtCollFromPool(nCollId)->SetFmtAttr(pColl->GetAttrSet());
}

// ----------------------------------------------------------------------------


void SwModule::InsertEnv( SfxRequest& rReq )
{
static USHORT nTitleNo = 0;

    SwDocShell		*pMyDocSh;
    SfxViewFrame	*pFrame;
    SwView          *pNewView;
    SwWrtShell		*pOldSh,
                    *pSh;

    //aktuelle Shell besorgen
    pMyDocSh = (SwDocShell*) SfxObjectShell::Current();
    pOldSh	 = pMyDocSh ? pMyDocSh->GetWrtShell() : 0;

    // Neues Dokument erzeugen (kein Show!)
    SfxObjectShellRef xDocSh( new SwDocShell( SFX_CREATE_MODE_STANDARD ) );
    xDocSh->DoInitNew( 0 );
    pFrame = SfxViewFrame::CreateViewFrame( *xDocSh, 0, TRUE );
    pNewView = (SwView*) pFrame->GetViewShell();
    pNewView->AttrChangedNotify( &pNewView->GetWrtShell() );//Damit SelectShell gerufen wird.
    pSh = pNewView->GetWrtShellPtr();

    String aTmp( SW_RES(STR_ENV_TITLE) );
    aTmp += String::CreateFromInt32( ++nTitleNo );
    xDocSh->SetTitle( aTmp );

    // Ggf. alte Collections "Absender" und "Empfaenger" in neues
    // Dokument kopieren
    if ( pOldSh )
    {
        ::lcl_CopyCollAttr(pOldSh, pSh, RES_POOLCOLL_JAKETADRESS);
        ::lcl_CopyCollAttr(pOldSh, pSh, RES_POOLCOLL_SENDADRESS);
    }

    // SwEnvItem aus Config lesen
    SwEnvCfgItem aEnvCfg;

    //Haben wir schon einen Briefumschlag.
    BOOL bEnvChange = FALSE;

    SfxItemSet aSet(GetPool(), FN_ENVELOP, FN_ENVELOP, 0);
    aSet.Put(aEnvCfg.GetItem());

    SfxPrinter* pTempPrinter = pSh->getIDocumentDeviceAccess()->getPrinter( true );
    if(pOldSh )
    {
        const SwPageDesc& rCurPageDesc = pOldSh->GetPageDesc(pOldSh->GetCurPageDesc());
        String sJacket;
        SwStyleNameMapper::FillUIName( RES_POOLPAGE_JAKET, sJacket );
        bEnvChange = rCurPageDesc.GetName() == sJacket;

        IDocumentDeviceAccess* pIDDA_old = pOldSh->getIDocumentDeviceAccess();
        if( pIDDA_old->getPrinter( false ) )
        {
            IDocumentDeviceAccess* pIDDA = pSh->getIDocumentDeviceAccess();
            pIDDA->setJobsetup( *pIDDA_old->getJobsetup() );
            //#69563# if it isn't the same printer then the pointer has been invalidated!
            pTempPrinter = pIDDA->getPrinter( true );
        }
        pTempPrinter->SetPaperBin(rCurPageDesc.GetMaster().GetPaperBin().GetValue());

    }

    Window *pParent = pOldSh ? pOldSh->GetWin() : 0;
    SfxAbstractTabDialog * pDlg=NULL;
    short nMode = ENV_INSERT;

    SFX_REQUEST_ARG( rReq, pItem, SwEnvItem, FN_ENVELOP, sal_False );
    if ( !pItem )
    {
        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

        pDlg = pFact->CreateSwEnvDlg( pParent, aSet, pOldSh, pTempPrinter, !bEnvChange, DLG_ENV );
        DBG_ASSERT(pDlg, "Dialogdiet fail!");
        nMode = pDlg->Execute();
    }
    else
    {
        SFX_REQUEST_ARG( rReq, pBoolItem, SfxBoolItem, FN_PARAM_1, sal_False );
        if ( pBoolItem && pBoolItem->GetValue() )
            nMode = ENV_NEWDOC;
    }

    if (nMode == ENV_NEWDOC || nMode == ENV_INSERT)
    {
        SwWait aWait( (SwDocShell&)*xDocSh, TRUE );

        // Dialog auslesen, Item in Config speichern
        const SwEnvItem& rItem = pItem ? *pItem : (const SwEnvItem&) pDlg->GetOutputItemSet()->Get(FN_ENVELOP);
        aEnvCfg.GetItem() = rItem;
        aEnvCfg.Commit();

        //Wenn wir Drucken uebernehmen wir den eingestellten Jobsetup aus
        //dem Dialog. Die Informationen muessen hier vor dem evtl. zerstoeren
        //der neuen Shell gesetzt werden, weil deren Drucker an den Dialog
        //gereicht wurde.
        if ( nMode != ENV_NEWDOC )
        {
            ASSERT(pOldSh, "Kein Dokument - war 'Einfuegen' nicht disabled???");
            SvxPaperBinItem aItem( RES_PAPER_BIN );
            aItem.SetValue((BYTE)pSh->getIDocumentDeviceAccess()->getPrinter(true)->GetPaperBin());
            pOldSh->GetPageDescFromPool(RES_POOLPAGE_JAKET)->GetMaster().SetFmtAttr(aItem);
        }

        SwWrtShell *pTmp = nMode == ENV_INSERT ? pOldSh : pSh;
        const SwPageDesc* pFollow = 0;
        SwTxtFmtColl *pSend = pTmp->GetTxtCollFromPool( RES_POOLCOLL_SENDADRESS ),
                     *pAddr = pTmp->GetTxtCollFromPool( RES_POOLCOLL_JAKETADRESS);
        const String &rSendMark = pSend->GetName();
        const String &rAddrMark = pAddr->GetName();

        if (nMode == ENV_INSERT)
        {

            SetView(&pOldSh->GetView()); // Pointer auf oberste View restaurieren

            //Neues Dok wieder loeschen
            xDocSh->DoClose();
            pSh = pOldSh;
            //#i4251# selected text or objects in the document should
            //not be deleted on inserting envelopes
            pSh->EnterStdMode();
            // Los geht's (Einfuegen)
            pSh->StartUndo(UNDO_UI_INSERT_ENVELOPE, NULL);
            pSh->StartAllAction();
            pSh->SttEndDoc(TRUE);

            if (bEnvChange)
            {
                // Folgevorlage: Seite 2
                pFollow = pSh->GetPageDesc(pSh->GetCurPageDesc()).GetFollow();

                // Text der ersten Seite loeschen
                if ( !pSh->SttNxtPg(TRUE) )
                    pSh->EndPg(TRUE);
                pSh->DelRight();
                // Rahmen der ersten Seite loeschen
                if( pSh->GotoFly( rSendMark ) )
                {
                    pSh->EnterSelFrmMode();
                    pSh->DelRight();
                }
                if ( pSh->GotoFly( rAddrMark ) )
                {
                    pSh->EnterSelFrmMode();
                    pSh->DelRight();
                }
                pSh->SttEndDoc(TRUE);
            }
            else
                // Folgevorlage: Seite 1
                pFollow = &pSh->GetPageDesc(pSh->GetCurPageDesc());

            // Seitenumbruch einfuegen
            if ( pSh->IsCrsrInTbl() )
            {
                pSh->SplitNode();
                pSh->Right( CRSR_SKIP_CHARS, FALSE, 1, FALSE );
                SfxItemSet aBreakSet( pSh->GetAttrPool(), RES_BREAK, RES_BREAK, 0 );
                aBreakSet.Put( SvxFmtBreakItem(SVX_BREAK_PAGE_BEFORE, RES_BREAK) );
                pSh->SetTblAttr( aBreakSet );
            }
            else
                pSh->InsertPageBreak(0, FALSE);
            pSh->SttEndDoc(TRUE);
        }
        else
        {
            pFollow = &pSh->GetPageDesc(pSh->GetCurPageDesc());
            // Los geht's (Drucken)
            pSh->StartAllAction();
            pSh->DoUndo(FALSE);

            // Neue Collections "Absender" und "Empfaenger" wieder in neues
            // Dokument kopieren
            if ( pOldSh )
            {
                ::lcl_CopyCollAttr(pOldSh, pSh, RES_POOLCOLL_JAKETADRESS);
                ::lcl_CopyCollAttr(pOldSh, pSh, RES_POOLCOLL_SENDADRESS);
            }
        }

        SET_CURR_SHELL(pSh);
        pSh->SetNewDoc();		// Performanceprobleme vermeiden

        // Flys dieser Seite merken
        SvPtrarr aFlyArr(0, 5);
        if( ENV_NEWDOC != nMode && !bEnvChange )
            pSh->GetPageObjs( aFlyArr );

        // Page-Desc ermitteln
        SwPageDesc* pDesc = pSh->GetPageDescFromPool(RES_POOLPAGE_JAKET);
        SwFrmFmt&   rFmt  = pDesc->GetMaster();

        Printer *pPrt = pSh->getIDocumentDeviceAccess()->getPrinter( true );

        // Raender (setzen sich zusammen aus Shift-Offset und
        // Ausrichtung)
        Size aPaperSize = pPrt->PixelToLogic( pPrt->GetPaperSizePixel(),
                                              MAP_TWIP);
        if ( !aPaperSize.Width() && !aPaperSize.Height() )
                    aPaperSize = SvxPaperInfo::GetPaperSize(PAPER_A4);
        if ( aPaperSize.Width() > aPaperSize.Height() )
            Swap( aPaperSize );

        long lLeft  = rItem.lShiftRight,
             lUpper = rItem.lShiftDown;

        USHORT nPageW = (USHORT) Max(rItem.lWidth, rItem.lHeight),
               nPageH = (USHORT) Min(rItem.lWidth, rItem.lHeight);

        switch (rItem.eAlign)
        {
            case ENV_HOR_LEFT: break;
            case ENV_HOR_CNTR: lLeft  += Max(0L, long(aPaperSize.Width() - nPageW)) / 2;
                               break;
            case ENV_HOR_RGHT: lLeft  += Max(0L, long(aPaperSize.Width() - nPageW));
                               break;
            case ENV_VER_LEFT: lUpper += Max(0L, long(aPaperSize.Width() - nPageH));
                               break;
            case ENV_VER_CNTR: lUpper += Max(0L, long(aPaperSize.Width() - nPageH)) / 2;
                               break;
            case ENV_VER_RGHT: break;
        }
        SvxLRSpaceItem aLRMargin( RES_LR_SPACE );
        SvxULSpaceItem aULMargin( RES_UL_SPACE );
        aLRMargin.SetLeft ((USHORT) lLeft );
        aULMargin.SetUpper((USHORT) lUpper);
        aLRMargin.SetRight(0);
        aULMargin.SetLower(0);
        rFmt.SetFmtAttr(aLRMargin);
        rFmt.SetFmtAttr(aULMargin);

        // Kopf-, Fusszeilen
        rFmt.SetFmtAttr(SwFmtHeader(BOOL(FALSE)));
        pDesc->ChgHeaderShare(FALSE);
        rFmt.SetFmtAttr(SwFmtFooter(BOOL(FALSE)));
        pDesc->ChgFooterShare(FALSE);

        // Seitennumerierung
        pDesc->SetUseOn(nsUseOnPage::PD_ALL);

        // Einstellen der Seitengroesse
        rFmt.SetFmtAttr(SwFmtFrmSize(ATT_FIX_SIZE,
                                            nPageW + lLeft, nPageH + lUpper));

        // Einstellen der Numerierungsart der Seite
        SvxNumberType aType;
        aType.SetNumberingType(SVX_NUM_NUMBER_NONE);
        pDesc->SetNumType(aType);

        // Folgevorlage
        if (pFollow)
            pDesc->SetFollow(pFollow);

        // Landscape
        pDesc->SetLandscape( rItem.eAlign >= ENV_VER_LEFT &&
                             rItem.eAlign <= ENV_VER_RGHT);

        // Page-Desc anwenden

        USHORT nPos;
        pSh->FindPageDescByName( pDesc->GetName(),
                                    FALSE,
                                    &nPos );


        pSh->ChgPageDesc( nPos, *pDesc);
        pSh->ChgCurPageDesc(*pDesc);

        // Rahmen einfuegen
        SwFlyFrmAttrMgr aMgr(FALSE, pSh, FRMMGR_TYPE_ENVELP);
        SwFldMgr aFldMgr;
        aMgr.SetHeightSizeType(ATT_VAR_SIZE);

        //Defaults ueberschreiben!
        aMgr.GetAttrSet().Put( SvxBoxItem(RES_BOX) );
        aMgr.SetULSpace( 0L, 0L );
        aMgr.SetLRSpace( 0L, 0L );

        // Absender
        if (rItem.bSend)
        {
            pSh->SttEndDoc(TRUE);
            aMgr.InsertFlyFrm(FLY_AT_PAGE,
                Point(rItem.lSendFromLeft + lLeft, rItem.lSendFromTop  + lUpper),
                Size (rItem.lAddrFromLeft - rItem.lSendFromLeft, 0));

            pSh->EnterSelFrmMode();
            pSh->SetFlyName( rSendMark );
            pSh->UnSelectFrm();
            pSh->LeaveSelFrmMode();
            pSh->SetTxtFmtColl( pSend );
            InsertLabEnvText( *pSh, aFldMgr, rItem.aSendText );
            aMgr.UpdateAttrMgr();
        }

        // Empfaenger
        pSh->SttEndDoc(TRUE);

        aMgr.InsertFlyFrm(FLY_AT_PAGE,
            Point(rItem.lAddrFromLeft + lLeft, rItem.lAddrFromTop  + lUpper),
            Size (nPageW - rItem.lAddrFromLeft - 566, 0));
        pSh->EnterSelFrmMode();
        pSh->SetFlyName( rAddrMark );
        pSh->UnSelectFrm();
        pSh->LeaveSelFrmMode();
        pSh->SetTxtFmtColl( pAddr );
        InsertLabEnvText(*pSh, aFldMgr, rItem.aAddrText);

        // Flys auf die "alten" Seiten verschieben
        if (aFlyArr.Count())
            pSh->SetPageObjsNewPage(aFlyArr, 1);

        // Fertig
        pSh->SttEndDoc(TRUE);

        pSh->EndAllAction();

        if (nMode == ENV_NEWDOC)
            pSh->DoUndo(TRUE);
        else
            pSh->EndUndo(UNDO_UI_INSERT_ENVELOPE);

        if (nMode == ENV_NEWDOC)
        {
            pFrame->GetFrame()->Appear();

            if ( rItem.aAddrText.indexOf('<') >= 0 )
            {
                static USHORT __READONLY_DATA aInva[] =
                                    {
                                        SID_SBA_BRW_UPDATE,
                                        SID_SBA_BRW_INSERT,
                                        SID_SBA_BRW_MERGE,
                                        0
                                    };
                pFrame->GetBindings().Invalidate( aInva );

                // Datenbankbeamer oeffnen
                ShowDBObj(*pNewView, pSh->GetDBData());
            }
        }

        if ( !pItem )
        {
            rReq.AppendItem( rItem );
            if ( nMode == ENV_NEWDOC )
                rReq.AppendItem( SfxBoolItem( FN_PARAM_1, TRUE ) );
        }

        rReq.Done();
    }
    else	//Abbruch
    {
        rReq.Ignore();

        xDocSh->DoClose();
        --nTitleNo;

        // Pointer auf oberste View restaurieren
        if (pOldSh)
            SetView(&pOldSh->GetView());
    }
    delete pDlg;
}


