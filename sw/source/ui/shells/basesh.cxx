/*************************************************************************
 *
 *  $RCSfile: basesh.cxx,v $
 *
 *  $Revision: 1.54 $
 *
 *  last change: $Author: hjs $ $Date: 2004-06-28 13:51:10 $
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

#pragma hdrstop
#include <sot/factory.hxx>
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif


#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#ifndef _SVXLINKMGR_HXX //autogen
#include <svx/linkmgr.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _IMAPDLG_HXX_ //autogen
#include <svx/imapdlg.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SFX_OBJITEM_HXX //autogen
#include <sfx2/objitem.hxx>
#endif
#ifndef _FILTER_HXX //autogen
#include <svtools/filter.hxx>
#endif
#ifndef _GALLERY_HXX_ //autogen
#include <svx/gallery.hxx>
#endif
#ifndef _SVX_CLIPFMTITEM_HXX
#include <svx/clipfmtitem.hxx>
#endif
#ifndef _CONTDLG_HXX_ //autogen
#include <svx/contdlg.hxx>
#endif
#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX //autogen
#include <svx/impgrf.hxx>
#endif
#ifndef _SFXSLSTITM_HXX //autogen
#include <svtools/slstitm.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXPTITEM_HXX //autogen
#include <svtools/ptitem.hxx>
#endif
#ifndef _SFXITEMITER_HXX
#include <svtools/itemiter.hxx>
#endif
#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX
#include <svx/srchitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_OPAQITEM_HXX //autogen
#include <svx/opaqitem.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX //autogen
#include <svx/sizeitem.hxx>
#endif
//CHINA001 #ifndef _SVX_BORDER_HXX
//CHINA001 #include <svx/border.hxx>
//CHINA001 #endif
#include <svx/flagsdef.hxx> //CHINA001
#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <svx/scripttypeitem.hxx>
#endif

#ifndef _FMTURL_HXX //autogen
#include <fmturl.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _BASESH_HXX
#include <basesh.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
//CHINA001 #ifndef _CHANGEDB_HXX
//CHINA001 #include <changedb.hxx>
//CHINA001 #endif
//CHINA001 #ifndef _BOOKMARK_HXX
//CHINA001 #include <bookmark.hxx>
//CHINA001 #endif
#ifndef _BOOKMRK_HXX
#include <bookmrk.hxx>
#endif
#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif
#ifndef _USRFLD_HXX
#include <usrfld.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _FRMMGR_HXX
#include <frmmgr.hxx>
#endif
#ifndef _TABLEMGR_HXX
#include <tablemgr.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>
#endif
#ifndef _SWDTFLVR_HXX
#include <swdtflvr.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _CONVERT_HXX
#include <convert.hxx>
#endif
//CHINA001 #ifndef _PATTERN_HXX
//CHINA001 #include <pattern.hxx>
//CHINA001 #endif
//CHINA001 #ifndef _UIBORDER_HXX
//CHINA001 #include <uiborder.hxx>
//CHINA001 #endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _TBLAFMT_HXX
#include <tblafmt.hxx>
#endif
#ifndef _CAPTION_HXX
#include <caption.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _SHELLS_HRC
#include <shells.hrc>
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif
#ifndef _GLOBALS_H
#include <globals.h>
#endif
#ifndef _UNOTXDOC_HXX
#include <unotxdoc.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _FMTINFMT_HXX
#include <fmtinfmt.hxx>
#endif

#include "swabstdlg.hxx" //CHINA001
#include "dialog.hrc" //CHINA001
#include "fldui.hrc" //CHINA001
#include "table.hrc" //CHINA001

// includes for InsertTable
#ifndef _SVX_HTMLMODE_HXX
#include <svx/htmlmode.hxx>
#endif
#ifndef _MODOPT_HXX //autogen
#include <modcfg.hxx>
#endif
#ifndef _TBLAFMT_HXX
#include <tblafmt.hxx>
#endif
#ifndef _INSTABLE_HXX
#include <instable.hxx>
#endif

USHORT SwBaseShell::nFrameMode = FLY_DRAG_END;
#define C2S(cChar) UniString::CreateFromAscii(cChar)

//Fuer die Erkennung der Id, die variable von Gallery mit SID_GALLERY_BG_BRUSH
//ankommt.
static BYTE nParagraphPos;
static BYTE nGraphicPos;
static BYTE nOlePos;
static BYTE nFramePos;
static BYTE nTablePos;
static BYTE nTableRowPos;
static BYTE nTableCellPos;
static BYTE nPagePos;
static BYTE nHeaderPos;
static BYTE nFooterPos;

#define Interior
#include <svx/svxslots.hxx>

#define SwBaseShell
#define Shadow
#include "itemdef.hxx"
#include "swslots.hxx"

#define SWCONTOURDLG(rView) ( (SvxContourDlg*) ( rView.GetViewFrame()->GetChildWindow( 	\
                          SvxContourDlgChildWindow::GetChildWindowId() )-> 	\
                          GetWindow() ) )

#define SWIMAPDLG(rView) ( (SvxIMapDlg*) ( rView.GetViewFrame()->GetChildWindow( 		\
                        SvxIMapDlgChildWindow::GetChildWindowId() )-> 	\
                        GetWindow() ) )


using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;


SFX_IMPL_INTERFACE(SwBaseShell, SfxShell, SW_RES(0))
{
    SFX_CHILDWINDOW_REGISTRATION(SvxIMapDlgChildWindow::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(SvxContourDlgChildWindow::GetChildWindowId());
}

TYPEINIT1(SwBaseShell,SfxShell)

/*--------------------------------------------------------------------
    Beschreibung:	statics
 --------------------------------------------------------------------*/


void lcl_UpdateIMapDlg( SwWrtShell& rSh )
{
    Graphic aGrf( rSh.GetIMapGraphic() );
    USHORT nGrfType = aGrf.GetType();
    void* pEditObj = GRAPHIC_NONE != nGrfType && GRAPHIC_DEFAULT != nGrfType
                        ? rSh.GetIMapInventor() : 0;
    TargetList* pList = new TargetList;
    rSh.GetView().GetViewFrame()->GetTopFrame()->GetTargetList(*pList);

    SfxItemSet aSet( rSh.GetAttrPool(), RES_URL, RES_URL );
    rSh.GetFlyFrmAttr( aSet );
    const SwFmtURL &rURL = (SwFmtURL&)aSet.Get( RES_URL );
    SvxIMapDlgChildWindow::UpdateIMapDlg(
            aGrf, rURL.GetMap(), pList, pEditObj );

    USHORT nCount = (USHORT)pList->Count();
    if(nCount)
        for( USHORT i = nCount; i; i--  )
        {
            delete pList->GetObject(i-1);
        }
    delete pList;
}


BOOL lcl_UpdateContourDlg( SwWrtShell &rSh, int nSel )
{
    Graphic aGraf( rSh.GetIMapGraphic() );
    USHORT nGrfType = aGraf.GetType();
    BOOL bRet = GRAPHIC_NONE != nGrfType && GRAPHIC_DEFAULT != nGrfType;
    if( bRet )
    {
        String aGrfName;
        if ( nSel & SwWrtShell::SEL_GRF )
            rSh.GetGrfNms( &aGrfName, 0 );

        SvxContourDlg *pDlg = SWCONTOURDLG(rSh.GetView());
        pDlg->Update( aGraf, aGrfName.Len() > 0,
                  rSh.GetGraphicPolygon(), rSh.GetIMapInventor() );
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:	Alle Items disablen, wird auch aus anderen Source gerufen
 --------------------------------------------------------------------*/

void DisableAllItems(SfxItemSet &rSet)
{
    SfxWhichIter aIter(rSet);

    USHORT nWhich = aIter.FirstWhich();
    ASSERT(nWhich, leeres Set);
    while(nWhich)
    {
        rSet.DisableItem(nWhich);
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	loeschen
 --------------------------------------------------------------------*/

void SwBaseShell::ExecDelete(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();
    switch(rReq.GetSlot())
    {
        case SID_DELETE:
            rSh.DelRight(TRUE);
            break;

        case FN_BACKSPACE:

            if( rSh.IsNoNum() )
            {
                rSh.SttCrsrMove();
                BOOL bLeft = rSh.Left( CRSR_SKIP_CHARS, TRUE, 1, FALSE  );
                if( bLeft )
                {
                    // JP 28.03.96: ein Backspace im Absatz ohne Nummer wird zum Delete
                    rSh.SwapPam();
                    rSh.DelRight( FALSE );
                }
                else
                    // JP 15.07.96: wenns nicht mehr nach vorne geht, die
                    //				Numerierung aufheben. Z.B. am Doc-/
                    //				Rahmen-/Tabellen-/Bereichs-Anfang
                    rSh.DelNumRules();

                rSh.EndCrsrMove();
                break;
            }

            // ansonsten DelLeft rufen
        case FN_SHIFT_BACKSPACE:
            rSh.DelLeft();
            break;
        default:
            DBG_ERROR("falscher Dispatcher");
            return;
    }
    rReq.Done();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwBaseShell::ExecClpbrd(SfxRequest &rReq)
{
    //Achtung: Suizid gefaehrdet! Nach Paste, Paste special kann der die
    //Shell zerstoert sein.


    SwWrtShell &rSh = GetShell();
    USHORT nId = rReq.GetSlot();
    sal_Bool bIgnore = sal_False;
    switch( nId )
    {
        case SID_CUT:
        case SID_COPY:
            rView.GetEditWin().FlushInBuffer();
            if ( rSh.HasSelection() )
            {
                SwTransferable* pTransfer = new SwTransferable( rSh );
/*??*/ 			::com::sun::star::uno::Reference<
                    ::com::sun::star::datatransfer::XTransferable > xRef(
                                                                pTransfer );

                if ( nId == SID_CUT )
                    pTransfer->Cut();
                else
                {
                    const BOOL bLockedView = rSh.IsViewLocked();
                    rSh.LockView( TRUE );    //lock visible section
                    pTransfer->Copy();
                    rSh.LockView( bLockedView );
                }
                break;
            }
            return;

        case SID_PASTE:
            {
                TransferableDataHelper aDataHelper(
                        TransferableDataHelper::CreateFromSystemClipboard(
                                        &rSh.GetView().GetEditWin() ) );

                if( aDataHelper.GetXTransferable().is() &&
                    SwTransferable::IsPaste( rSh, aDataHelper ))
                {
                    // temp. Variablen, da die Shell nach dem Paste schon
                    // zerstoert sein kann
                    SwView* pView = &rView;
                    SwTransferable::Paste( rSh, aDataHelper );
                    if( rSh.IsFrmSelected() || rSh.IsObjSelected() )
                        rSh.EnterSelFrmMode();
                    pView->AttrChangedNotify( &rSh );
                }
                else
                    return;
            }
            break;

        case SID_CLIPBOARD_FORMAT_ITEMS:
            {
                const SfxItemSet* pArgs = rReq.GetArgs();
                const SfxPoolItem* pFmt;
                if( pArgs && SFX_ITEM_SET == pArgs->GetItemState( nId, FALSE, &pFmt ) )
                {
                    TransferableDataHelper aDataHelper(
                        TransferableDataHelper::CreateFromSystemClipboard(
                                            &rSh.GetView().GetEditWin()) );
                    if( aDataHelper.GetXTransferable().is()
                        /*&& SwTransferable::IsPaste( rSh, aDataHelper )*/ )
                    {
                        // temp. Variablen, da die Shell nach dem Paste schon
                        // zerstoert sein kann
                        SwView* pView = &rView;

                        SwTransferable::PasteFormat( rSh, aDataHelper,
                                        ((SfxUInt32Item*)pFmt)->GetValue() );

                        //Done() has to be called before the shell has been removed
                        rReq.Done();
                        bIgnore = sal_True;
                        if( rSh.IsFrmSelected() || rSh.IsObjSelected())
                            rSh.EnterSelFrmMode();
                        pView->AttrChangedNotify( &rSh );
                    }
                }
            }
            break;

        case FN_PASTESPECIAL:
            {
                TransferableDataHelper aDataHelper(
                        TransferableDataHelper::CreateFromSystemClipboard(
                                        &rSh.GetView().GetEditWin()) );
                if( aDataHelper.GetXTransferable().is() &&
                    SwTransferable::IsPaste( rSh, aDataHelper ))
                {
                    // temp. Variablen, da die Shell nach dem Paste schon
                    // zerstoert sein kann
                    SwView* pView = &rView;
                    ULONG nFormatId = 0;
                    rReq.Ignore();
                    bIgnore = sal_True;
                    int nRet = SwTransferable::PasteSpecial( rSh, aDataHelper, nFormatId );
                    if ( nRet && rReq.IsRecording() )
                    {
                        SfxRequest aReq( pView->GetViewFrame(), SID_CLIPBOARD_FORMAT_ITEMS );
                        aReq.AppendItem( SfxUInt32Item( SID_CLIPBOARD_FORMAT_ITEMS, nFormatId ) );
                        aReq.Done();
                    }

                    if (rSh.IsFrmSelected() || rSh.IsObjSelected())
                        rSh.EnterSelFrmMode();
                    pView->AttrChangedNotify( &rSh );
                }
                else
                    return;
            }
            break;
        default:
            DBG_ERROR("falscher Dispatcher");
            return;
    }
    if(!bIgnore)
        rReq.Done();
}

/*--------------------------------------------------------------------
    Beschreibung:	ClipBoard-Status
 --------------------------------------------------------------------*/

void SwBaseShell::StateClpbrd(SfxItemSet &rSet)
{
    SwWrtShell &rSh = GetShell();
    SfxWhichIter aIter(rSet);

    const BOOL bCopy = rSh.HasSelection();

    USHORT nWhich = aIter.FirstWhich();

    while(nWhich)
    {
        switch(nWhich)
        {
        case SID_CUT:
            if( 0 != rSh.IsSelObjProtected( (FlyProtectType)
                            (FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) )
            {
                rSet.DisableItem( nWhich );
                break;
            }
        case SID_COPY:
            if( !bCopy )
                rSet.DisableItem( nWhich );
            break;

        case SID_PASTE:
            if( !GetView().IsPasteAllowed() )
                rSet.DisableItem( SID_PASTE );
            break;

        case FN_PASTESPECIAL:
            if( !GetView().IsPasteSpecialAllowed() )
                rSet.DisableItem( FN_PASTESPECIAL );
            break;

        case SID_CLIPBOARD_FORMAT_ITEMS:
            {
                TransferableDataHelper aDataHelper(
                    TransferableDataHelper::CreateFromSystemClipboard(
                                            &rSh.GetView().GetEditWin()) );

                SvxClipboardFmtItem aFmtItem( nWhich );
                SwTransferable::FillClipFmtItem( rSh, aDataHelper, aFmtItem );
                rSet.Put( aFmtItem );
            }
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Undo ausfuehren
 --------------------------------------------------------------------*/

void SwBaseShell::ExecUndo(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();

    USHORT nId = rReq.GetSlot(), nCnt = 1;
    const SfxItemSet* pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem;
    if( pArgs && SFX_ITEM_SET == pArgs->GetItemState( nId, FALSE, &pItem ))
        nCnt = ((SfxUInt16Item*)pItem)->GetValue();

    switch( nId )
    {
        case SID_UNDO:
            rSh.Do( SwWrtShell::UNDO, nCnt );
            break;

        case SID_REDO:
            rSh.Do( SwWrtShell::REDO, nCnt );
            break;

        case SID_REPEAT:
            rSh.Do( SwWrtShell::REPEAT );
            break;
        default:
            DBG_ERROR("falscher Dispatcher");
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Zustand Undo
 --------------------------------------------------------------------*/

void SwBaseShell::StateUndo(SfxItemSet &rSet)
{
    SwWrtShell &rSh = GetShell();
    SfxWhichIter aIter(rSet);
    USHORT nWhich = aIter.FirstWhich();
    while(nWhich)
    {
        switch(nWhich)
        {
            case SID_UNDO:
            {
                if( rSh.GetUndoIds() )
                    rSet.Put( SfxStringItem(nWhich,
                        rSh.GetDoString(SwWrtShell::UNDO)));
                else
                    rSet.DisableItem(nWhich);
                break;
            }
            case SID_REDO:
            {
                if(rSh.GetRedoIds())
                    rSet.Put(SfxStringItem(nWhich,
                        rSh.GetDoString(SwWrtShell::REDO)));
                else
                    rSet.DisableItem(nWhich);
                break;
            }
            case SID_REPEAT:
            {	// Repeat nur moeglich wenn kein REDO moeglich - UI-Restriktion
                if(rSh.GetRedoIds() == 0 &&
                    !rSh.IsSelFrmMode() &&
                    rSh.GetRepeatIds() )
                    rSet.Put(SfxStringItem(nWhich, rSh.GetRepeatString()));
                else
                    rSet.DisableItem(nWhich);
                break;
            }

            case SID_GETUNDOSTRINGS:
                if( rSh.GetUndoIds() )
                {
                    SfxStringListItem aStrLst( nWhich );
                    rSh.GetDoStrings( SwWrtShell::UNDO, aStrLst );
                    rSet.Put( aStrLst );
                }
                else
                    rSet.DisableItem( nWhich );
                break;

            case SID_GETREDOSTRINGS:
                if( rSh.GetRedoIds() )
                {
                    SfxStringListItem aStrLst( nWhich );
                    rSh.GetDoStrings( SwWrtShell::REDO, aStrLst );
                    rSet.Put( aStrLst );
                }
                else
                    rSet.DisableItem( nWhich );
                break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Slot-Id auswerten bzw. Dispatchen
 --------------------------------------------------------------------*/

void SwBaseShell::Execute(SfxRequest &rReq)
{
    const SfxPoolItem *pItem;
    SwWrtShell &rSh = GetShell();
    const SfxItemSet* pArgs = rReq.GetArgs();
    BOOL bMore = FALSE;

    USHORT nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case FN_REPAGINATE:
            {
                Reference < XModel > xModel = GetView().GetDocShell()->GetModel();
                Reference < XUnoTunnel > xDocTunnel ( xModel, UNO_QUERY );
                SwXTextDocument *pDoc = reinterpret_cast < SwXTextDocument * > ( xDocTunnel->getSomething ( SwXTextDocument::getUnoTunnelId() ) );
                pDoc->notifyRefreshListeners();
                rSh.CalcLayout();
            }
            break;
        case FN_UPDATE_FIELDS:
            {
                SwDocStat aDocStat;
                rSh.UpdateDocStat(aDocStat);
                rSh.EndAllTblBoxEdit();
                rSh.ViewShell::UpdateFlds(TRUE);

                if( rSh.IsCrsrInTbl() )
                {
                    if( !rSh.IsTblComplexForChart() )
                        SwTableFUNC( &rSh, FALSE).UpdateChart();
                    rSh.ClearTblBoxCntnt();
                    rSh.SaveTblBoxCntnt();
                }
            }
            break;
        case FN_UPDATE_CHARTS:
            {
                SwWait aWait( *rView.GetDocShell(), TRUE );
                rSh.UpdateAllCharts();
            }
            break;

        case FN_UPDATE_ALL:
            {
                rSh.EnterStdMode();
                SwView& rView = GetView();
                if( rSh.GetLinkManager().GetLinks().Count() )
                {
                    rSh.StartAllAction();
                    rSh.GetLinkManager().UpdateAllLinks( FALSE, TRUE, TRUE );
                    rSh.EndAllAction();
                }
                SfxDispatcher &rDis = *rView.GetViewFrame()->GetDispatcher();
                rDis.Execute( FN_UPDATE_FIELDS );
                rDis.Execute( FN_UPDATE_TOX );
                rDis.Execute( FN_UPDATE_CHARTS );
                rSh.CalcLayout();
            }
            break;

        case FN_UPDATE_INPUTFIELDS:
            rSh.UpdateInputFlds(NULL, FALSE);
            break;
        case FN_PREV_BOOKMARK:
            rReq.SetReturnValue(SfxBoolItem( nSlot, rSh.GoPrevBookmark()));
            break;
        case FN_NEXT_BOOKMARK:
            rReq.SetReturnValue(SfxBoolItem( nSlot, rSh.GoNextBookmark()));
            break;

        case FN_GOTO_NEXT_MARK:
        case FN_GOTO_PREV_MARK:
        {
            SwFldMgr aFldMgr;
            SwFieldType* pFldType = aFldMgr.GetFldType(RES_JUMPEDITFLD);

            if (pFldType)
            {
                if (rSh.IsSelFrmMode())
                {
                    rSh.UnSelectFrm();
                    rSh.LeaveSelFrmMode();
                }

                if (rSh.HasMark())
                {
                    MV_KONTEXT(&rSh);
                    if (rSh.IsCrsrPtAtEnd())
                        rSh.SwapPam();
                    rSh.ClearMark();
                    rSh.EndSelect();
                }
                BOOL bRet = rSh.MoveFldType(pFldType, nSlot == FN_GOTO_NEXT_MARK);
                if (bRet)
                    rSh.ClickToField(*rSh.GetCurFld());
                rReq.SetReturnValue(SfxBoolItem( nSlot, bRet));
            }
        }
        break;

        case FN_START_DOC_DIRECT:
        case FN_END_DOC_DIRECT:
        {
            if (rSh.IsSelFrmMode())
            {
                rSh.UnSelectFrm();
                rSh.LeaveSelFrmMode();
            }
            rSh.EnterStdMode();
            nSlot == FN_START_DOC_DIRECT ?
                rSh.SwCrsrShell::SttDoc() :
                    rSh.SwCrsrShell::EndDoc();
        }
        break;
        case FN_GOTO_PREV_OBJ:
        case FN_GOTO_NEXT_OBJ:
        {
                BOOL bSuccess = rSh.GotoObj(
                            nSlot == FN_GOTO_NEXT_OBJ ? TRUE : FALSE);
                rReq.SetReturnValue(SfxBoolItem(nSlot, bSuccess));
                if (bSuccess && !rSh.IsSelFrmMode())
                {
                    rSh.HideCrsr();
                    rSh.EnterSelFrmMode();
                    GetView().AttrChangedNotify( &rSh );
                }
        }
        break;
        case SID_GALLERY_FORMATS:
        {
            const int nSelType = rSh.GetSelectionType();
            if(SFX_ITEM_SET == pArgs->GetItemState( nSlot, TRUE, &pItem))
            {
                GalleryExplorer* pGal = 0;
                if ( (!rSh.IsSelFrmMode() || nSelType & SwWrtShell::SEL_GRF) &&
                    0!= (pGal = SVX_GALLERY())&&
                    0 != (SGA_FORMAT_GRAPHIC & ((SfxUInt32Item*)pItem)->GetValue()))
                {
                    SwWait aWait( *rView.GetDocShell(), TRUE );

                    String aGrfName, aFltName;
                    const Graphic aGrf( pGal->GetGraphic() );

                    if( pGal->IsLinkage() )
                    {
                        // Verknuepft
                        aGrfName = pGal->GetURL().PathToFileName();
                        aGrfName = URIHelper::SmartRelToAbs(aGrfName);
                        aFltName = pGal->GetFilterName();
                    }

                    if ( nSelType & SwWrtShell::SEL_GRF )
                        rSh.ReRead( aGrfName, aFltName, &aGrf );
                    else
                        rSh.Insert( aGrfName, aFltName, aGrf );

                    GetView().GetEditWin().GrabFocus();
                }
                else if(!rSh.IsSelFrmMode() && SGA_FORMAT_SOUND & ((SfxUInt32Item*)pItem)->GetValue())
                {
                    String sURL( pGal->GetURL().GetMainURL( INetURLObject::NO_DECODE ) );
                    String sLabel( pGal->GetURL().getBase() );
                    String sTarget; // empty string!

                    bool bIsHTMLMode =
                        0 == ( HTMLMODE_ON &
                               ::GetHtmlMode( GetView().GetDocShell() ) );

                    // in Writer, we insert a button which plays the
                    // sound. In Writer/Web, we just insert a (text) link.
                    if( bIsHTMLMode )
                        InsertURLButton( sURL, sTarget, sLabel );
                    else
                        rSh.InsertURL( SwFmtINetFmt( sURL, sTarget ), sLabel );
                }
            }
        }
        break;
        case FN_PAGE_STYLE_SET_COLS:
        {
            if (pArgs)
            {
                // aktuellen PageDescriptor ermitteln und damit den Set fuellen
                const USHORT nCurIdx = rSh.GetCurPageDesc();
                SwPageDesc aPageDesc(rSh.GetPageDesc(nCurIdx));

                SwFrmFmt &rFmt = aPageDesc.GetMaster();

                SwFmtCol aFmtCol = rFmt.GetCol();

                USHORT nCount;
                if(SFX_ITEM_SET == pArgs->GetItemState(nSlot))
                    nCount = ((SfxUInt16Item &)pArgs->Get(nSlot)).GetValue();
                else
                    nCount = ((SfxUInt16Item &)pArgs->Get(SID_ATTR_COLUMNS)).GetValue();
                USHORT nGutterWidth = DEF_GUTTER_WIDTH;

                aFmtCol.Init(nCount ? nCount : 1, nGutterWidth, USHRT_MAX);
                aFmtCol.SetWishWidth(USHRT_MAX);
                aFmtCol.SetGutterWidth(nGutterWidth, USHRT_MAX);

                rFmt.SetAttr(aFmtCol);

                rSh.ChgPageDesc(nCurIdx, aPageDesc);
            }
            else
                GetView().GetViewFrame()->GetDispatcher()->Execute(FN_FORMAT_PAGE_COLUMN_DLG, FALSE);
        }
        break;
        case FN_CONVERT_TEXT_TABLE:
        {
            sal_Unicode cDelim = 0;
            SwInsertTableOptions aInsTblOpts( tabopts::ALL_TBL_INS_ATTR, 1 );
            SwTableAutoFmt* pTAFmt = 0;
            //CHINA001 SwConvertTableDlg *pDlg = new SwConvertTableDlg( GetView() );
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
            DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

            AbstractSwConvertTableDlg* pDlg = pFact->CreateSwConvertTableDlg( GetView(),ResId( DLG_CONV_TEXT_TABLE ));
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
            if( RET_OK == pDlg->Execute() )
            {
                pDlg->GetValues( cDelim, aInsTblOpts, pTAFmt );

            }
            delete pDlg;

            if( cDelim )
            {
                //Shellwechsel!
                SwView& rSaveView = rView;
                BOOL bInserted = FALSE;
                if( rSh.GetTableFmt() )
                    rSh.TableToText( cDelim );
                else
                    bInserted = rSh.TextToTable( aInsTblOpts, cDelim, HORI_FULL, pTAFmt );
                rSh.EnterStdMode();

                if( bInserted )
                    rSaveView.AutoCaption( TABLE_CAP );
            }
            delete pTAFmt;
        }
        break;
        case SID_STYLE_WATERCAN:
        case SID_STYLE_UPDATE_BY_EXAMPLE:
        case SID_STYLE_NEW_BY_EXAMPLE:
        case SID_STYLE_APPLY:
        {
            ShellModes eMode = GetView().GetShellMode();
            if ( SEL_DRAW != eMode &&
                 SEL_DRAW_CTRL != eMode &&
                 SEL_DRAW_FORM != eMode &&
                 SEL_DRAWTEXT != eMode &&
                 SEL_BEZIER != eMode )
            {
                // oj #107754#
                if ( SID_STYLE_WATERCAN == nSlot )
                {
                    SwWrtShell &rSh = GetShell();
                    const BOOL bLockedView = rSh.IsViewLocked();
                    rSh.LockView( TRUE );    //lock visible section

                    GetView().GetDocShell()->ExecStyleSheet(rReq);

                    rSh.LockView( bLockedView );
                }
                else
                // wird von der DocShell aufgezeichnet
                    GetView().GetDocShell()->ExecStyleSheet(rReq);
            }
        }
        break;
        case FN_ESCAPE:
            GetView().ExecuteSlot(rReq);
        break;
        case SID_IMAP:
        {
            USHORT		nId = SvxIMapDlgChildWindow::GetChildWindowId();

            SfxViewFrame* pVFrame = GetView().GetViewFrame();
            pVFrame->ToggleChildWindow( nId );
            pVFrame->GetBindings().Invalidate( SID_IMAP );

            if ( pVFrame->HasChildWindow( nId ) && rSh.IsFrmSelected() )
                lcl_UpdateIMapDlg( rSh );
        }
        break;
        case SID_IMAP_EXEC:
        {
            SvxIMapDlg*	pDlg = SWIMAPDLG(GetView());

            // Kontrolle, ob Zuweisung ueberhaupt sinnvoll/erlaubt
            if ( rSh.IsFrmSelected() &&
                 pDlg->GetEditingObject() == rSh.GetIMapInventor() )
            {
                    SfxItemSet aSet( rSh.GetAttrPool(), RES_URL, RES_URL );
                    rSh.GetFlyFrmAttr( aSet );
                    SwFmtURL aURL( (SwFmtURL&)aSet.Get( RES_URL ) );
                    aURL.SetMap( &pDlg->GetImageMap() );
                    aSet.Put( aURL );
                    rSh.SetFlyFrmAttr( aSet );
            }
        }
        break;
        case SID_CONTOUR_DLG:
        {
            USHORT nId = SvxContourDlgChildWindow::GetChildWindowId();

            SfxViewFrame* pVFrame = GetView().GetViewFrame();
            pVFrame->ToggleChildWindow( nId );
            pVFrame->GetBindings().Invalidate( SID_CONTOUR_DLG );

            int nSel = rSh.GetSelectionType();
            if ( pVFrame->HasChildWindow( nId ) &&
                 (nSel & (SwWrtShell::SEL_GRF|SwWrtShell::SEL_OLE)) )
            {
                lcl_UpdateContourDlg( rSh, nSel );
            }
        }
        break;
        case SID_CONTOUR_EXEC:
        {
            SvxContourDlg *pDlg = SWCONTOURDLG(GetView());

            // Kontrolle, ob Zuweisung ueberhaupt sinnvoll/erlaubt
            int nSel = rSh.GetSelectionType();
            if ( nSel & (SwWrtShell::SEL_GRF|SwWrtShell::SEL_OLE) )
            {
                if ( pDlg->GetEditingObject() == rSh.GetIMapInventor() )
                {
                    rSh.StartAction();
                    SfxItemSet aSet( rSh.GetAttrPool(), RES_SURROUND, RES_SURROUND);
                    rSh.GetFlyFrmAttr( aSet );
                    SwFmtSurround aSur( (SwFmtSurround&)aSet.Get( RES_SURROUND ) );
                    if ( !aSur.IsContour() )
                    {
                        aSur.SetContour( TRUE );
                        if ( aSur.GetSurround() == SURROUND_NONE )
                            aSur.SetSurround( SURROUND_PARALLEL );
                        aSet.Put( aSur );
                        rSh.SetFlyFrmAttr( aSet );
                    }
                    const PolyPolygon aPoly( pDlg->GetPolyPolygon() );
                    rSh.SetGraphicPolygon( &aPoly );
                    if ( pDlg->IsGraphicChanged() )
                        rSh.ReRead( aEmptyStr, aEmptyStr, &pDlg->GetGraphic());
                    rSh.EndAction();
                }
            }
        }
        break;
        case FN_FRAME_TO_ANCHOR:
        {
            rSh.GotoFlyAnchor();
            rSh.EnterStdMode();
            rSh.CallChgLnk();
        }
        break;
        case FN_TOOL_ANKER:
            break;
        case FN_TOOL_ANKER_PAGE:
        case FN_TOOL_ANKER_PARAGRAPH:
        case FN_TOOL_ANKER_CHAR:
        case FN_TOOL_ANKER_AT_CHAR:
        case FN_TOOL_ANKER_FRAME:
        {
            RndStdIds eSet = nSlot == FN_TOOL_ANKER_PAGE
                                ? FLY_PAGE
                                : nSlot == FN_TOOL_ANKER_PARAGRAPH
                                    ? FLY_AT_CNTNT
                                    : nSlot == FN_TOOL_ANKER_FRAME
                                        ? FLY_AT_FLY
                                        : nSlot == FN_TOOL_ANKER_CHAR
                                            ? FLY_IN_CNTNT
                                            : FLY_AUTO_CNTNT;
            rSh.StartUndo();
            if( rSh.IsObjSelected() )
                rSh.ChgAnchor( eSet );
            else if( rSh.IsFrmSelected() )
            {
                // Der Set beinhaltet auch VERT/HORI_ORIENT, da in FEShell::
                // SetFlyFrmAttr/SetFlyFrmAnchor ggf. als Folge des Umankerns
                // die Ausrichtungen veraendert werden sollen.
                SfxItemSet aSet( GetPool(), RES_VERT_ORIENT, RES_ANCHOR );
                SwFmtAnchor aAnc( eSet, rSh.GetPhyPageNum() );
                aSet.Put( aAnc );
                rSh.SetFlyFrmAttr(aSet);
            }

            USHORT nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
            if( nHtmlMode )
            {
                SfxItemSet aSet(GetPool(), RES_SURROUND, RES_HORI_ORIENT);
                rSh.GetFlyFrmAttr(aSet);

                const SwFmtSurround& rSurround = (const SwFmtSurround&)aSet.Get(RES_SURROUND);
                const SwFmtVertOrient& rVert = (const SwFmtVertOrient&)aSet.Get(RES_VERT_ORIENT);
                const SwFmtHoriOrient& rHori = (const SwFmtHoriOrient&)aSet.Get(RES_HORI_ORIENT);
                SwVertOrient eVOrient = rVert.GetVertOrient();
                SwHoriOrient eHOrient = rHori.GetHoriOrient();
                SwSurround eSurround = rSurround.GetSurround();

                switch( eSet )
                {
                case FLY_AT_FLY:
                case FLY_PAGE:

                    //Durchlauf, links oder von links, oben, von oben
                    if(eSurround != SURROUND_THROUGHT)
                        aSet.Put(SwFmtSurround(SURROUND_THROUGHT));

                    if( eVOrient != VERT_TOP && eVOrient != VERT_NONE)
                        aSet.Put(SwFmtVertOrient(0, VERT_TOP));

                    if(eHOrient != HORI_NONE || eHOrient != HORI_LEFT)
                        aSet.Put(SwFmtHoriOrient(0, HORI_LEFT));
                    break;

                case FLY_AT_CNTNT:
                    //links, von links, rechts, oben, kein Uml, li+re Umlauf,
                    if(eSurround != SURROUND_LEFT || eSurround != SURROUND_RIGHT)
                        aSet.Put(SwFmtSurround(SURROUND_LEFT));

                    if( eVOrient != VERT_TOP)
                        aSet.Put(SwFmtVertOrient(0, VERT_TOP));

                    if(eHOrient != HORI_NONE || eHOrient != HORI_LEFT || eHOrient != HORI_RIGHT)
                        aSet.Put(SwFmtHoriOrient(0, HORI_LEFT));
                    break;

                case FLY_AUTO_CNTNT:
                    //links, von links, rechts, oben,  Durchlauf
                    if(eSurround != SURROUND_THROUGHT)
                        aSet.Put(SwFmtSurround(SURROUND_THROUGHT));

                    if( eVOrient != VERT_TOP)
                        aSet.Put(SwFmtVertOrient(0, VERT_TOP));

                    if(eHOrient != HORI_NONE || eHOrient != HORI_LEFT || eHOrient != HORI_RIGHT)
                        aSet.Put(SwFmtHoriOrient(0, HORI_LEFT));
                    break;
                }

                if( aSet.Count() )
                    rSh.SetFlyFrmAttr( aSet );
            }
            rSh.EndUndo();

            GetView().GetViewFrame()->GetBindings().Invalidate( FN_TOOL_ANKER );
        }
        break;

        case FN_FRAME_NOWRAP:
        case FN_FRAME_WRAP:
        case FN_FRAME_WRAP_IDEAL:
        case FN_FRAME_WRAPTHRU:
        case FN_FRAME_WRAPTHRU_TRANSP:
        case FN_FRAME_WRAP_CONTOUR:
        case FN_WRAP_ANCHOR_ONLY:
        case FN_FRAME_WRAP_LEFT:
        case FN_FRAME_WRAP_RIGHT:
            SetWrapMode( nSlot );
            break;

        case FN_UPDATE_ALL_LINKS:
            {
                if( rSh.GetLinkManager().GetLinks().Count() )
                {
                    BOOL bUpdateGrf = FALSE, bCallErrHdl = FALSE;
                    rSh.EnterStdMode();
                    rSh.StartAllAction();
                    rSh.GetLinkManager().UpdateAllLinks( FALSE, bCallErrHdl, bUpdateGrf );
                    rSh.EndAllAction();
                }
            }
            break;

        default:
            bMore = TRUE;
    }
    if(bMore && pArgs)
    {
        pItem = 0;
        pArgs->GetItemState(GetPool().GetWhich(nSlot), FALSE, &pItem);
        if(pItem)
        switch(nSlot)
        {
        case SID_ATTR_BRUSH:
        case SID_ATTR_BORDER_SHADOW:
        case RES_SHADOW:
        {
            rSh.StartAllAction();
            SfxItemSet	 aSet( rSh.GetAttrPool(),
                                RES_SHADOW, RES_SHADOW,
                                RES_BACKGROUND, RES_BACKGROUND, 0 );

            aSet.Put(*pItem);
            // Tabellenzelle(n) selektiert?
            if ( rSh.IsTableMode() )
            {
                SwFrmFmt *pFmt = rSh.GetTableFmt();
                pFmt->SetAttr( *pItem );
            }
            else if ( rSh.IsFrmSelected() )
            {
                // Umrandungsattribute ueber Frame-Manager setzen
                SwFlyFrmAttrMgr aMgr( FALSE, &rSh, FRMMGR_TYPE_NONE );
                aMgr.SetAttrSet( *pArgs );
                aMgr.UpdateFlyFrm();
            }
            else
            {
                rSh.SetAttr( *pArgs );
            }
            rSh.EndAllAction();
        }
        break;
        case FN_PAGE_STYLE_SET_LR_MARGIN:
        case FN_PAGE_STYLE_SET_UL_MARGIN:
        case FN_PAGE_STYLE_SET_NUMBER_FORMAT:
        case FN_PAGE_STYLE_SET_PAPER_SIZE:
        case FN_PAGE_STYLE_SET_PAPER_BIN:
        {
            DBG_ERROR("not implemented")
        }
        break;

        case SID_ATTR_BORDER_OUTER:
        {
            // Tabellenzelle(n) selektiert?
            if ( rSh.IsTableMode() )
            {
                // Umrandungattribute Get/SetTabBorders() setzen
                rSh.SetTabBorders(*pArgs);
            }
            else if ( rSh.IsFrmSelected() )
            {
                // Umrandungsattribute ueber Frame-Manager setzen
                SwFlyFrmAttrMgr aMgr( FALSE, &rSh, FRMMGR_TYPE_NONE );
                aMgr.SetAttrSet(*pArgs);
                aMgr.UpdateFlyFrm();
            }
            else
            {
                // Umrandungsattribute ganz normal ueber Shell setzen
                rSh.SetAttr( *pItem );
            }
        }
        break;
        default:
                DBG_ERROR("falscher Dispatcher");
        }

    }
}

/* -----------------14.04.99 15:10-------------------
 * Hier wird der State fuer SID_IMAP / SID_CONTOUR behandelt,
 * wenn die Grafik ausgeswappt ist
 * --------------------------------------------------*/
IMPL_LINK(SwBaseShell, GraphicArrivedHdl, SwCrsrShell* , pCrShell )
{
    USHORT nGrfType;
    SwWrtShell &rSh = GetShell();
    if( CNT_GRF == rSh.SwEditShell::GetCntType() &&
        GRAPHIC_NONE != ( nGrfType = rSh.GetGraphicType() ) &&
        aGrfUpdateSlots.Count() )
    {
        BOOL bProtect = 0 != rSh.IsSelObjProtected( (FlyProtectType)
                                    (FLYPROTECT_CONTENT|FLYPROTECT_PARENT) );
        SfxViewFrame* pVFrame = GetView().GetViewFrame();
        USHORT nSlot;
        for( USHORT n = 0; n < aGrfUpdateSlots.Count(); ++n )
        {
            BOOL bSetState = FALSE;
            BOOL bState = FALSE;
            switch( nSlot = aGrfUpdateSlots[ n ] )
            {
            case SID_IMAP:
            case SID_IMAP_EXEC:
                {
                    USHORT nId = SvxIMapDlgChildWindow::GetChildWindowId();
                    SvxIMapDlg *pDlg = pVFrame->HasChildWindow( nId ) ?
                        (SvxIMapDlg*) ( pVFrame->GetChildWindow( nId )
                                            ->GetWindow()) : 0;

                    if( pDlg && ( SID_IMAP_EXEC == nSlot ||
                                ( SID_IMAP == nSlot && !bProtect)) &&
                        pDlg->GetEditingObject() != rSh.GetIMapInventor())
                            lcl_UpdateIMapDlg( rSh );

                    if( !bProtect && SID_IMAP == nSlot )
                        bSetState = TRUE, bState = 0 != pDlg;
                }
                break;

            case SID_CONTOUR_DLG:
                if( !bProtect )
                {
                    USHORT nId = SvxContourDlgChildWindow::GetChildWindowId();
                    SvxIMapDlg *pDlg = pVFrame->HasChildWindow( nId ) ?
                        (SvxIMapDlg*) ( pVFrame->GetChildWindow( nId )
                                            ->GetWindow()) : 0;
                    if( pDlg && pDlg->GetEditingObject() !=
                                rSh.GetIMapInventor() )
                        lcl_UpdateContourDlg( rSh, SwWrtShell::SEL_GRF );

                    bSetState = TRUE;
                    bState = 0 != pDlg;
                }
                break;

            case FN_FRAME_WRAP_CONTOUR:
                if( !bProtect )
                {
                    SfxItemSet aSet(GetPool(), RES_SURROUND, RES_SURROUND);
                    rSh.GetFlyFrmAttr(aSet);
                    const SwFmtSurround& rWrap = (const SwFmtSurround&)aSet.Get(RES_SURROUND);
                    bSetState = TRUE;
                    bState = rWrap.IsContour();
                }
                break;

            case SID_GRFFILTER:
            case SID_GRFFILTER_INVERT:
            case SID_GRFFILTER_SMOOTH:
            case SID_GRFFILTER_SHARPEN:
            case SID_GRFFILTER_REMOVENOISE:
            case SID_GRFFILTER_SOBEL:
            case SID_GRFFILTER_MOSAIC:
            case SID_GRFFILTER_EMBOSS:
            case SID_GRFFILTER_POSTER:
            case SID_GRFFILTER_POPART:
            case SID_GRFFILTER_SEPIA:
            case SID_GRFFILTER_SOLARIZE:
                bSetState = bState = GRAPHIC_BITMAP == nGrfType;
                break;
            }

            if( bSetState )
            {
                SfxBoolItem aBool( nSlot, bState );
                if( pGetStateSet )
                    pGetStateSet->Put( aBool );
                else
                    pVFrame->GetBindings().SetState( aBool );
            }
        }
        aGrfUpdateSlots.RemoveAt( 0, aGrfUpdateSlots.Count() );
    }
    return 0;
}

void SwBaseShell::GetState( SfxItemSet &rSet )
{
    SwWrtShell &rSh = GetShell();
    SfxViewFrame* pVFrame = GetView().GetViewFrame();
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    pGetStateSet = &rSet;
    while ( nWhich )
    {
        switch ( nWhich )
        {
            case SID_GALLERY_FORMATS:
                if ( rSh.IsObjSelected() ||
                     (rSh.IsSelFrmMode() &&
                      !(rSh.GetSelectionType() & SwWrtShell::SEL_GRF)) )
                    rSet.DisableItem( nWhich );
                break;
            case SID_GALLERY_ENABLE_ADDCOPY:
                // #108230# allow copy from gallery in Writer AND Writer/Web!
                rSet.Put( SfxBoolItem( SID_GALLERY_ENABLE_ADDCOPY, TRUE ) );
                break;
            case FN_EDIT_REGION:
                if( !rSh.IsAnySectionInDoc() )
                    rSet.DisableItem(nWhich);
                break;

            case FN_INSERT_REGION:
                if( rSh.IsSelFrmMode() ||
                    !rSh.IsInsRegionAvailable() )
                    rSet.DisableItem( nWhich );
                break;
            case FN_CONVERT_TEXT_TABLE:
            {
                USHORT eFrmType = rSh.GetFrmType(0,TRUE);
                if( (eFrmType & FRMTYPE_FOOTNOTE) ||
                    (!rSh.GetTableFmt() && !rSh.IsTextToTableAvailable() ) )
                    rSet.DisableItem( nWhich );
            }
            break;
            case RES_SHADOW:
            {
                SfxItemSet	 aSet( rSh.GetAttrPool(),
                                    RES_SHADOW, RES_SHADOW );

                // Tabellenzelle(n) selektiert?
                if ( rSh.IsTableMode() )
                {
                    SwFrmFmt *pFmt = rSh.GetTableFmt();
                    aSet.Put(pFmt->GetAttr( nWhich, TRUE ));
                }
                else if( rSh.IsFrmSelected() )
                {
                    SwFlyFrmAttrMgr aMgr( FALSE, &rSh, FRMMGR_TYPE_NONE );
                    aSet.Put( aMgr.GetAttrSet() );
                }
                else
                    rSh.GetAttr( aSet );
#if OSL_DEBUG_LEVEL > 1

                const SvxShadowItem& rShItem = (const SvxShadowItem&)aSet.Get(nWhich);
                rSet.Put(rShItem);
#else
                rSet.Put((const SvxShadowItem&)aSet.Get(nWhich));
#endif
            }
            break;
            case SID_IMAP:
            {
                USHORT	nId = SvxIMapDlgChildWindow::GetChildWindowId();
                BOOL bHas = pVFrame->HasChildWindow( nId );
                BOOL bFrmSel = rSh.IsFrmSelected();
                BOOL bProtect = 0 != rSh.IsSelObjProtected( (FlyProtectType)
                                    (FLYPROTECT_CONTENT|FLYPROTECT_PARENT) );

                BOOL bIsGraphicSelection = rSh.GetSelectionType() == SwWrtShell::SEL_GRF;

                //wenn die Grafik ausgeswappt ist, dann muss der
                //Status asynchron ermittelt werden
                //bis dahin wird der Slot disabled
                if( bIsGraphicSelection && rSh.IsGrfSwapOut( TRUE ))
                {
                    if( AddGrfUpdateSlot( nWhich ))
                        rSh.GetGraphic(FALSE);  // start the loading
                }
                else
                {
                    if( bProtect || ( !bHas && ( !bFrmSel ||
                            (bIsGraphicSelection &&
                            rSh.GetIMapGraphic().GetType() == GRAPHIC_NONE )) ))
                        rSet.DisableItem( nWhich );
                    else
                    {
                        SfxBoolItem aBool(nWhich, bHas);
                        if ( bHas && bFrmSel )
                            lcl_UpdateIMapDlg( rSh );
                        rSet.Put(aBool);
                    }
                }
            }
            break;
            case SID_IMAP_EXEC:
            {
                BOOL bDisable = FALSE;
                if( !rSh.IsFrmSelected())
                    bDisable = TRUE;
                USHORT nId = SvxIMapDlgChildWindow::GetChildWindowId();
                if(!bDisable && pVFrame->HasChildWindow( nId ))
                {
                    if(rSh.GetSelectionType() == SwWrtShell::SEL_GRF
                                    && rSh.IsGrfSwapOut(TRUE))
                    {
                        if( AddGrfUpdateSlot( nWhich ))
                            rSh.GetGraphic(FALSE);  // start the loading
                    }
                    else
                    {
                        SvxIMapDlg *pDlg = SWIMAPDLG(GetView());
                        if( pDlg->GetEditingObject() != rSh.GetIMapInventor() )
                            lcl_UpdateIMapDlg( rSh );
                    }
                }
                rSet.Put(SfxBoolItem(nWhich, bDisable));
            }
            break;
            case FN_BACKSPACE:
            case SID_DELETE:
                if (rSh.IsSelObjProtected( (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) != 0)
                    rSet.DisableItem( nWhich );
                break;
            case SID_CONTOUR_DLG:
            {
                BOOL bParentCntProt = 0 != rSh.IsSelObjProtected(
                    (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) );

                if( bParentCntProt || 0 != (HTMLMODE_ON & ::GetHtmlMode(
                                            GetView().GetDocShell() )) )
                    rSet.DisableItem( nWhich );
                else
                {
                    USHORT nId = SvxContourDlgChildWindow::GetChildWindowId();
                    BOOL bHas = GetView().GetViewFrame()->HasChildWindow( nId );
                    int nSel = rSh.GetSelectionType();
                    BOOL bOk = 0 != (nSel & (SwWrtShell::SEL_GRF|SwWrtShell::SEL_OLE));

                    BOOL bDisable;
                    if( !bHas && !bOk )
                        bDisable = TRUE;
                    // wenn die Grafik ausgeswappt ist, dann muss der Status
                    // asynchron ermittelt werden bis dahin wird der Slot
                    // disabled
                    else if( (nSel & SwWrtShell::SEL_GRF) &&
                                rSh.IsGrfSwapOut(TRUE))
                    {
                        if( AddGrfUpdateSlot( nWhich ))
                            rSh.GetGraphic(FALSE);  // start the loading
                    }
                    else if( bHas && bOk )
                        bDisable = !lcl_UpdateContourDlg( rSh, nSel );
                    else if( bOk )
                        bDisable = GRAPHIC_NONE == rSh.GetIMapGraphic().GetType();

                    if( bDisable )
                        rSet.DisableItem( nWhich );
                    else
                        rSet.Put( SfxBoolItem(nWhich, bHas) );
                }
            }
            break;
            case SID_CONTOUR_EXEC:
            {
                BOOL bDisable = FALSE;
                int nSel = rSh.GetSelectionType();
                if( !(nSel & (SwWrtShell::SEL_GRF|SwWrtShell::SEL_OLE)) )
                    bDisable = TRUE;
                USHORT nId = SvxContourDlgChildWindow::GetChildWindowId();
                if( !bDisable && GetView().GetViewFrame()->HasChildWindow( nId ))
                {
                    SvxContourDlg *pDlg = SWCONTOURDLG(GetView());
                    if( pDlg->GetEditingObject() != rSh.GetIMapInventor() )
                        bDisable = TRUE;
                }
                rSet.Put(SfxBoolItem(nWhich, bDisable));
            }
            break;

            case FN_TOOL_ANKER:
            case FN_TOOL_ANKER_PAGE:
            case FN_TOOL_ANKER_PARAGRAPH:
            case FN_TOOL_ANKER_CHAR:
            case FN_TOOL_ANKER_AT_CHAR:
            case FN_TOOL_ANKER_FRAME:
            {
                BOOL bObj = 0 != rSh.IsObjSelected();
                BOOL bParentCntProt = rSh.IsSelObjProtected( (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) != 0;

                if( !bParentCntProt && (bObj || rSh.IsFrmSelected()))
                {
                    SfxItemSet aSet(GetPool(), RES_ANCHOR, RES_ANCHOR);
                    if(bObj)
                        rSh.GetObjAttr(aSet);
                    else
                        rSh.GetFlyFrmAttr(aSet);
                    RndStdIds eSet = ((SwFmtAnchor&)aSet.Get(RES_ANCHOR)).GetAnchorId();
                    BOOL bSet;
                    bSet = (nWhich == FN_TOOL_ANKER_PAGE && eSet == FLY_PAGE) ||
                            (nWhich == FN_TOOL_ANKER_PARAGRAPH && eSet == FLY_AT_CNTNT) ||
                            (nWhich == FN_TOOL_ANKER_FRAME && eSet == FLY_AT_FLY) ||
                            (nWhich == FN_TOOL_ANKER_AT_CHAR && eSet == FLY_AUTO_CNTNT) ||
                            (nWhich == FN_TOOL_ANKER_CHAR && eSet == FLY_IN_CNTNT);
                    if(nWhich != FN_TOOL_ANKER)
                    {
                        USHORT nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
                        if( ( nWhich == FN_TOOL_ANKER_PAGE &&
                              ((HTMLMODE_ON & nHtmlMode) && (0 == (nHtmlMode & HTMLMODE_SOME_ABS_POS)))) ||
                            ( nWhich == FN_TOOL_ANKER_FRAME && !rSh.IsFlyInFly() ) )
                            rSet.DisableItem(nWhich);
                        else
                            rSet.Put(SfxBoolItem(nWhich, bSet));
                    }
                    else
                    {
                        USHORT nSlotId = 0;

                        switch (eSet)
                        {
                            case FLY_PAGE:
                                nSlotId = FN_TOOL_ANKER_PAGE;
                            break;
                            case FLY_AT_CNTNT:
                                nSlotId = FN_TOOL_ANKER_PARAGRAPH;
                            break;
                            case FLY_IN_CNTNT:
                                nSlotId = FN_TOOL_ANKER_CHAR;
                            break;
                            case FLY_AUTO_CNTNT:
                                nSlotId = FN_TOOL_ANKER_AT_CHAR;
                            break;
                            case FLY_AT_FLY:
                                nSlotId = FN_TOOL_ANKER_FRAME;
                            break;
                        }
                        rSet.Put(SfxUInt16Item(nWhich, nSlotId));
                    }
                }
                else
                    rSet.DisableItem( nWhich );
            }
            break;
            case FN_FRAME_NOWRAP:
            case FN_FRAME_WRAP:
            case FN_FRAME_WRAP_IDEAL:
            case FN_FRAME_WRAPTHRU:
            case FN_FRAME_WRAPTHRU_TRANSP:
            case FN_FRAME_WRAP_CONTOUR:
            case FN_WRAP_ANCHOR_ONLY:
            case FN_FRAME_WRAP_LEFT:
            case FN_FRAME_WRAP_RIGHT:
            {
                BOOL bObj = 0 != rSh.IsObjSelected();
                BOOL bParentCntProt = rSh.IsSelObjProtected( (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) != 0;

                if( !bParentCntProt && (bObj || rSh.IsFrmSelected()))
                {
                    SfxItemSet aSet(GetPool(), RES_OPAQUE, RES_ANCHOR);
                    int nAnchorType;
                    if(bObj)
                    {
                        rSh.GetObjAttr(aSet);
                        nAnchorType = rSh.GetAnchorId();
                    }
                    else
                    {
                        rSh.GetFlyFrmAttr(aSet);
                        nAnchorType = ((SwFmtAnchor&)aSet.Get(RES_ANCHOR)).GetAnchorId();
                    }
                    const SwFmtSurround& rWrap = (const SwFmtSurround&)aSet.Get(RES_SURROUND);

                    const SvxOpaqueItem& rOpaque = (const SvxOpaqueItem&)aSet.Get(RES_OPAQUE);
                    BOOL bOpaque = rOpaque.GetValue();
                    SwSurround nSurround = rWrap.GetSurround();
                    BOOL bSet;

                    BOOL bDisable = nAnchorType == - 1 || nAnchorType == FLY_IN_CNTNT;
                    BOOL bHtmlMode = 0 != ::GetHtmlMode(GetView().GetDocShell());

                    switch( nWhich )
                    {
                        case FN_FRAME_NOWRAP:
                            bDisable |=
                                ( nAnchorType != FLY_AT_CNTNT &&
                                    nAnchorType != FLY_AUTO_CNTNT && nAnchorType != FLY_PAGE);
                            bSet = nSurround == SURROUND_NONE;
                        break;
                        case FN_FRAME_WRAP:
                            bDisable |= bHtmlMode;
                            bSet = nSurround == SURROUND_PARALLEL;
                        break;
                        case FN_FRAME_WRAP_IDEAL:
                            bDisable |= bHtmlMode;
                            bSet = nSurround == SURROUND_IDEAL;
                        break;
                        case FN_FRAME_WRAPTHRU:
                            bDisable |= (bHtmlMode ||
                                (nAnchorType != FLY_AT_CNTNT&& nAnchorType != FLY_AUTO_CNTNT && nAnchorType != FLY_PAGE));
                            if(bObj)
                                bSet = nSurround == SURROUND_THROUGHT && rSh.GetLayerId();
                            else
                                bSet = nSurround == SURROUND_THROUGHT && bOpaque;
                        break;
                        case FN_FRAME_WRAPTHRU_TRANSP:
                            bDisable |= bHtmlMode;
                            if(bObj)
                                bSet = nSurround == SURROUND_THROUGHT && !rSh.GetLayerId();
                            else
                                bSet = nSurround == SURROUND_THROUGHT && !bOpaque;
                        break;
                        case FN_FRAME_WRAP_CONTOUR:
                            bDisable |= bHtmlMode;
                            //no contour available whenn no wrap or wrap through is set
                            bDisable |= (nSurround == SURROUND_NONE || nSurround == SURROUND_THROUGHT);
                            bSet = rWrap.IsContour();
                            if( !bDisable )
                            {
                                int nSel = rSh.GetSelectionType();
                                if( (nSel & SwWrtShell::SEL_GRF) &&
                                            rSh.IsGrfSwapOut(TRUE))
                                {
                                    if( AddGrfUpdateSlot( nWhich ))
                                        rSh.GetGraphic(FALSE);  // start the loading
                                }
                                else if( rSh.IsFrmSelected() )
                                    bDisable = GRAPHIC_NONE ==
                                            rSh.GetIMapGraphic().GetType()||
                                            nSel & SwWrtShell::SEL_FRM;
                            }
                            bSet = bDisable ? FALSE : rWrap.IsContour();

                        break;
                        case FN_WRAP_ANCHOR_ONLY:
                            bDisable |= (bHtmlMode ||
                                (nAnchorType != FLY_AT_CNTNT));
                            bSet = rWrap.IsAnchorOnly();
                        break;
                        case FN_FRAME_WRAP_LEFT:
                            bSet = nSurround == SURROUND_LEFT;
                        break;
                        case FN_FRAME_WRAP_RIGHT:
                            bSet = nSurround == SURROUND_RIGHT;
                        break;
                    }

                    if(bDisable)
                        rSet.DisableItem(nWhich);
                    else
                        rSet.Put(SfxBoolItem(nWhich, bSet));
                }
                else
                    rSet.DisableItem(nWhich);
            }
            break;
            case FN_UPDATE_CHARTS:
                if( !rSh.HasCharts() )
                    rSet.DisableItem( nWhich );
                break;
            case FN_UPDATE_ALL_LINKS:
                if ( !rSh.GetLinkManager().GetLinks().Count() )
                    rSet.DisableItem(nWhich);
                break;
        }
        nWhich = aIter.NextWhich();
    }
    pGetStateSet = 0;
}

/*--------------------------------------------------------------------
    Beschreibung:	Slots mit dieser Statusmethode disablen
 --------------------------------------------------------------------*/


void SwBaseShell::StateDisableItems( SfxItemSet &rSet )
{
    SfxWhichIter aIter(rSet);
    USHORT nWhich = aIter.FirstWhich();

    while (nWhich)
    {
        rSet.DisableItem( nWhich );
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Slots mit dieser Statusmethode disablen
 --------------------------------------------------------------------*/


void SwBaseShell::StateStyle( SfxItemSet &rSet )
{
    BOOL bParentCntProt = GetShell().IsSelObjProtected( (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) != 0;
    ShellModes eMode = GetView().GetShellMode();

    if ( bParentCntProt ||
         SEL_DRAW == eMode ||
         SEL_DRAW_CTRL == eMode ||
         SEL_DRAW_FORM == eMode ||
         SEL_DRAWTEXT == eMode ||
         SEL_BEZIER == eMode )
    {
        SfxWhichIter aIter( rSet );
        USHORT nWhich = aIter.FirstWhich();
        while ( nWhich )
        {
            rSet.DisableItem( nWhich );
            nWhich = aIter.NextWhich();
        }
    }
    else
        GetView().GetDocShell()->StateStyleSheet(rSet, &GetShell());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwBaseShell::SetWrapMode( USHORT nSlot )
{
    SwWrtShell &rSh = GetShell();
    BOOL bObj = 0 != rSh.IsObjSelected();
    if( bObj || rSh.IsFrmSelected())
    {
        SfxItemSet aSet(GetPool(), RES_OPAQUE, RES_SURROUND);
        if(bObj)
            rSh.GetObjAttr(aSet);
        else
            rSh.GetFlyFrmAttr(aSet);
        SwFmtSurround aWrap( (SwFmtSurround&)aSet.Get(RES_SURROUND) );
        SwSurround nOldSurround(aWrap.GetSurround());
        SwSurround nSurround = SURROUND_PARALLEL;

        switch (nSlot)
        {
            case FN_FRAME_NOWRAP:
                nSurround = SURROUND_NONE;
                if (aWrap.IsContour())
                    aWrap.SetContour(FALSE);
                break;
            case FN_FRAME_WRAP_IDEAL:
                nSurround = SURROUND_IDEAL;
                break;
            case FN_WRAP_ANCHOR_ONLY:
                aWrap.SetAnchorOnly(!aWrap.IsAnchorOnly());
                break;
            case FN_FRAME_WRAP_CONTOUR:
                aWrap.SetContour(!aWrap.IsContour());
                if (nSurround == SURROUND_THROUGHT)
                    nSurround = SURROUND_PARALLEL;
                break;
            case FN_FRAME_WRAPTHRU_TRANSP:
                if (aWrap.IsContour())
                    aWrap.SetContour(FALSE);
                // kein break!!!
            case FN_FRAME_WRAPTHRU:
                nSurround = SURROUND_THROUGHT;
                break;

            case FN_FRAME_WRAP_LEFT:
                nSurround = SURROUND_LEFT;
                break;

            case FN_FRAME_WRAP_RIGHT:
                nSurround = SURROUND_RIGHT;
                break;

            default:
                break;
        }
        aWrap.SetSurround(nSurround);

        if (nSlot != FN_FRAME_WRAP_CONTOUR)
        {
            // Konturumfluss bei Draw-Objekten defaulten
            if (bObj && nOldSurround != nSurround &&
                (nOldSurround == SURROUND_NONE || nOldSurround == SURROUND_THROUGHT))
            {
                aWrap.SetContour(TRUE);
            }
        }

        aSet.Put( aWrap );
        aSet.Put(SvxOpaqueItem(RES_OPAQUE, nSlot != FN_FRAME_WRAPTHRU_TRANSP));
        if(bObj)
        {
            rSh.SetObjAttr(aSet);
            if (nSlot != FN_FRAME_WRAPTHRU_TRANSP)
                rSh.SelectionToHeaven();
            else
                rSh.SelectionToHell();
        }
        else
            rSh.SetFlyFrmAttr(aSet);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Update der Statuszeile erzwingen
 --------------------------------------------------------------------*/

void SwBaseShell::SetFrmMode(USHORT nMode, SwWrtShell *pSh )
{
    nFrameMode = nMode;
    SfxBindings &rBnd = pSh->GetView().GetViewFrame()->GetBindings();

    if( nMode == FLY_DRAG ||
        (pSh && (pSh->IsFrmSelected() || pSh->IsObjSelected())) )
    {
        const SfxPointItem aTmp1( SID_ATTR_POSITION, pSh->GetAnchorObjDiff());
        const SvxSizeItem  aTmp2( SID_ATTR_SIZE,	 pSh->GetObjSize());
        rBnd.SetState( aTmp1 );
        rBnd.SetState( aTmp2 );
    }
    else if( nMode == FLY_DRAG_END )
    {
        static USHORT __READONLY_DATA aInval[] =
        {
            SID_ATTR_POSITION, SID_ATTR_SIZE, 0
        };
        rBnd.Invalidate(aInval);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Ctor
 --------------------------------------------------------------------*/

SwBaseShell::SwBaseShell(SwView& rVw) :
    SfxShell( &rVw ),
    rView(rVw),
    pFrmMgr(0),
    pGetStateSet(0)
{
    SwWrtShell& rWrtSh = rView.GetWrtShell();

    SetPool(&rWrtSh.GetAttrPool());
    SetName(C2S("Base"));
    rWrtSh.SetGrfArrivedLnk( LINK( this, SwBaseShell, GraphicArrivedHdl));
}


SwBaseShell::~SwBaseShell()
{
    delete pFrmMgr;
    if( rView.GetCurShell() == this )
        rView.ResetSubShell();

    Link aTmp( LINK( this, SwBaseShell, GraphicArrivedHdl));
    if( aTmp == rView.GetWrtShell().GetGrfArrivedLnk() )
        rView.GetWrtShell().SetGrfArrivedLnk( Link() );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwBaseShell::ExecTxtCtrl( SfxRequest& rReq )
{
    const SfxItemSet *pArgs = rReq.GetArgs();

    if( pArgs)
    {
        SwWrtShell &rSh = GetShell();
        SvxScriptSetItem* pSSetItem = 0;
        USHORT nSlot = rReq.GetSlot();
        SfxItemPool& rPool = rSh.GetAttrPool();
        USHORT nWhich = rPool.GetWhich( nSlot );
        USHORT nScripts = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;

        switch( nSlot )
        {
        case SID_ATTR_CHAR_FONT:
            nScripts = rSh.GetScriptType();
        case SID_ATTR_CHAR_FONTHEIGHT:
        case SID_ATTR_CHAR_POSTURE:
        case SID_ATTR_CHAR_WEIGHT:
            {
                pSSetItem = new SvxScriptSetItem( nSlot, rPool );
                pSSetItem->PutItemForScriptType( nScripts, pArgs->Get( nWhich ));
                pArgs = &pSSetItem->GetItemSet();
            }
            break;
        }

        if( pArgs )
        {
            SwTxtFmtColl* pColl;
            if( (!(RES_CHRATR_BEGIN <= nWhich && nWhich < RES_CHRATR_END ) ||
                ( rSh.HasSelection() && rSh.IsSelFullPara() ) ) &&
                0 != (pColl = rSh.GetCurTxtFmtColl()) &&
                pColl->IsAutoUpdateFmt() )
                rSh.AutoUpdatePara( pColl, *pArgs );
            else
                rSh.SetAttr( *pArgs );
        }
        delete pSSetItem;
    }
    else
        GetView().GetViewFrame()->GetDispatcher()->Execute( SID_CHAR_DLG, FALSE);
    rReq.Done();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwBaseShell::GetTxtCtrlState( SfxItemSet& rSet )
{
    SwWrtShell &rSh = GetShell();
    rSh.GetAttr( rSet );
}

void SwBaseShell::GetTxtFontCtrlState( SfxItemSet& rSet )
{
    SwWrtShell &rSh = GetShell();
    BOOL bFirst = TRUE;
    SfxItemSet* pFntCoreSet = 0;
    USHORT nScriptType;
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    while( nWhich )
    {
        switch( nWhich )
        {
        case RES_CHRATR_FONT:
        case RES_CHRATR_FONTSIZE:
        case RES_CHRATR_WEIGHT:
        case RES_CHRATR_POSTURE:
            {
                if( !pFntCoreSet )
                {
                    pFntCoreSet = new SfxItemSet( *rSet.GetPool(),
                                    RES_CHRATR_BEGIN, RES_CHRATR_END-1 );
                    rSh.GetAttr( *pFntCoreSet );
                    nScriptType = rSh.GetScriptType();
                }
                SfxItemPool& rPool = *rSet.GetPool();
                SvxScriptSetItem aSetItem( rPool.GetSlotId( nWhich ), rPool );
                aSetItem.GetItemSet().Put( *pFntCoreSet, FALSE );
                const SfxPoolItem* pI = aSetItem.GetItemOfScript( nScriptType );
                if( pI )
                    rSet.Put( *pI, nWhich );
                else
                    rSet.InvalidateItem( nWhich );
                //set input context of the SwEditWin according to the selected font and script type
                if(RES_CHRATR_FONT == nWhich)
                {
                    Font aFont;
                    if(pI && pI->ISA(SvxFontItem))
                    {
                        aFont.SetName( ((const SvxFontItem*)pI)->GetFamilyName());
                        aFont.SetStyleName(((const SvxFontItem*)pI)->GetStyleName());
                        aFont.SetFamily(((const SvxFontItem*)pI)->GetFamily());
                        aFont.SetPitch(((const SvxFontItem*)pI)->GetPitch());
                        aFont.SetCharSet(((const SvxFontItem*)pI)->GetCharSet());
                    }

                    BOOL bVertical = rSh.IsInVerticalText();
                    aFont.SetOrientation(bVertical ? 2700 : 0);
                    aFont.SetVertical(bVertical);
                    GetView().GetEditWin().SetInputContext( InputContext( aFont, INPUTCONTEXT_TEXT |
                                                        INPUTCONTEXT_EXTTEXTINPUT ) );
                }
            }
            break;

        default:
            if( bFirst )
            {
                rSh.GetAttr( rSet );
                bFirst = FALSE;
            }
        }
        nWhich = aIter.NextWhich();
    }
    delete pFntCoreSet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwBaseShell::GetBckColState(SfxItemSet &rSet)
{
    SwWrtShell &rSh = GetShell();
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    int nSelType = rSh.GetSelectionType();

//	if ( nSelType & SwWrtShell::SEL_GRF ||
    if( nSelType & SwWrtShell::SEL_OLE )
    {
        rSet.DisableItem( SID_BACKGROUND_COLOR );
        return;
    }

    if ( nSelType & SwWrtShell::SEL_FRM )
    {
        BOOL bParentCntProt = rSh.IsSelObjProtected( (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) ) != 0;
        if (bParentCntProt)
        {
            rSet.DisableItem( SID_BACKGROUND_COLOR );
            return;
        }
    }

    SvxBrushItem aBrushItem;

    if( SwWrtShell::SEL_TBL_CELLS & nSelType )
        rSh.GetBoxBackground( aBrushItem );
    else
    {
        SfxItemSet aCoreSet(GetPool(), RES_BACKGROUND, RES_BACKGROUND);
        if( nSelType & SwWrtShell::SEL_GRF || SwWrtShell::SEL_FRM & nSelType )
            rSh.GetFlyFrmAttr( aCoreSet );
        else
            rSh.GetAttr( aCoreSet );
        aBrushItem = (const SvxBrushItem&)aCoreSet.Get(RES_BACKGROUND);
    }

    while ( nWhich )
    {
        switch(nWhich)
        {
            case SID_BACKGROUND_COLOR  :
            {
                SvxColorItem aColorItem(aBrushItem.GetColor());
                rSet.Put( aColorItem, SID_BACKGROUND_COLOR );
            }
            break;
            case SID_ATTR_BRUSH:
            case RES_BACKGROUND:
                rSet.Put( aBrushItem, GetPool().GetWhich(nWhich) );
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwBaseShell::ExecBckCol(SfxRequest& rReq)
{
    SwWrtShell &rSh = GetShell();
    int nSelType = rSh.GetSelectionType();
    if ( nSelType & SwWrtShell::SEL_OLE )
    {
        return;
    }

    const SfxItemSet* pArgs = rReq.GetArgs();
    USHORT nSlot = rReq.GetSlot();
    if( !pArgs  && nSlot != SID_BACKGROUND_COLOR)
        return ;

    SvxBrushItem aBrushItem;

    if( SwWrtShell::SEL_TBL_CELLS & nSelType )
    {
        rSh.GetBoxBackground( aBrushItem );
    }
    else
    {
        SfxItemSet aCoreSet(GetPool(), RES_BACKGROUND, RES_BACKGROUND);
        if( (SwWrtShell::SEL_FRM & nSelType) || (SwWrtShell::SEL_GRF & nSelType) )
            rSh.GetFlyFrmAttr( aCoreSet );
        else
            rSh.GetAttr( aCoreSet );
        aBrushItem = (const SvxBrushItem&)aCoreSet.Get(RES_BACKGROUND);
    }

//	BOOL bMsgOk = FALSE;

    switch (nSlot)
    {
        // RES_BACKGROUND (=SID_ATTR_BRUSH) muss ueber zwei IDs
        // gesetzt werden:
        case SID_BACKGROUND_COLOR:
            {
                aBrushItem.SetGraphicPos(GPOS_NONE);

                //Brush &rBrush = aBrushItem.GetBrush();
                if(pArgs)
                {
                    const SvxColorItem&	rNewColorItem = (const SvxColorItem&)
                                            pArgs->Get(SID_BACKGROUND_COLOR);
                    const Color& rNewColor = rNewColorItem.GetValue();
                    aBrushItem.SetColor( rNewColor );
                    GetView().GetViewFrame()->GetBindings().SetState(rNewColorItem);
                }
                else
                {
                    aBrushItem.SetColor( COL_TRANSPARENT );
                    rReq.AppendItem( SvxColorItem( Color( COL_TRANSPARENT ), nSlot ) );
                }
            }
            break;

        case SID_ATTR_BRUSH:
        case RES_BACKGROUND:
        {
            const SvxBrushItem& rNewBrushItem = (const SvxBrushItem&)
                                    pArgs->Get( GetPool().GetWhich(nSlot) );
            aBrushItem = rNewBrushItem;
        }
        break;
        default:
//			bMsgOk = FALSE;
            rReq.Ignore();
            DBG_ERROR( "Unbekannte Message bei ExecuteAttr!" );
            return;
    }

    if( SwWrtShell::SEL_TBL_CELLS & nSelType )
    {
        rSh.SetBoxBackground( aBrushItem );
    }
    else if( (SwWrtShell::SEL_FRM & nSelType) ||
        (SwWrtShell::SEL_GRF & nSelType)  )
    {
        SfxItemSet aCoreSet(GetPool(), RES_BACKGROUND, RES_BACKGROUND);
        aCoreSet.Put( aBrushItem );
        // Vorlagen-AutoUpdate
        SwFrmFmt* pFmt = rSh.GetCurFrmFmt();
        if(pFmt && pFmt->IsAutoUpdateFmt())
            rSh.AutoUpdateFrame( pFmt, aCoreSet);
        else
            rSh.SetFlyFrmAttr( aCoreSet );
    }
    else
    {
        SwTxtFmtColl* pColl = rSh.GetCurTxtFmtColl();
        if(	pColl && pColl->IsAutoUpdateFmt())
        {
            SfxItemSet aSet(GetPool(), RES_BACKGROUND, RES_BACKGROUND );
            aSet.Put(aBrushItem);
            rSh.AutoUpdatePara( pColl, aSet);
        }
        else
            rSh.SetAttr( aBrushItem );
    }

    rReq.Done();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwBaseShell::GetBorderState(SfxItemSet &rSet)
{
    SwWrtShell &rSh = GetShell();
    // Tabellenzelle(n) selektiert?
    BOOL bPrepare = TRUE;
    if ( rSh.IsTableMode() )
    {
        SfxItemSet aCoreSet( GetPool(),
                             RES_BOX, RES_BOX,
                             SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER, 0 );
        SvxBoxInfoItem aBoxInfo;
        aCoreSet.Put( aBoxInfo );
        rSh.GetTabBorders( aCoreSet );
        rSet.Put( aCoreSet );
    }
    else if ( rSh.IsFrmSelected() )
    {
        SwFlyFrmAttrMgr aMgr( FALSE, &rSh, FRMMGR_TYPE_NONE );
        rSet.Put( aMgr.GetAttrSet() );
        bPrepare = FALSE;
    }
    else
        // Umrandungsattribute ganz normal ueber Shell holen
        rSh.GetAttr( rSet );

    if ( bPrepare )
        ::PrepareBoxInfo( rSet, rSh );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwBaseShell::ExecDlg(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();
    Window *pMDI = &GetView().GetViewFrame()->GetWindow();
    //Damit aus dem Basic keine Dialoge fuer Hintergrund-Views aufgerufen werden:
    BOOL bBackground = (&GetView() != GetActiveView());
    const SfxPoolItem* pItem = 0;
    const SfxItemSet* pArgs = rReq.GetArgs();

    USHORT nSlot = rReq.GetSlot();
    const SfxItemSet* pOutSet = 0;
    if(pArgs)
        pArgs->GetItemState( GetPool().GetWhich(nSlot), FALSE, &pItem );

    switch ( nSlot )
    {
        case FN_FORMAT_PAGE_COLUMN_DLG:
        case FN_FORMAT_PAGE_DLG:
        {
            if( !bBackground )
            {
                const USHORT nCurIdx = rSh.GetCurPageDesc();
                const SwPageDesc& rPageDesc = rSh.GetPageDesc( nCurIdx );
                //temp. View, weil die Shell nach dem Dialog nicht mehr gueltig sein muss
                //z.B. Kopfzeile ausschalten
                SwView& rView = GetView();
                rView.GetDocShell()->FormatPage(rPageDesc.GetName(),
                                    nSlot == FN_FORMAT_PAGE_COLUMN_DLG,
                                    &rSh );
                rView.InvalidateRulerPos();
            }
        }
        break;
        case FN_FORMAT_BORDER_DLG:
        {
            SfxItemSet	 aSet( rSh.GetAttrPool(),
                               RES_BOX	  			, RES_SHADOW,
                               SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER,
                               0 );
            //CHINA001 SwBorderDlg* pDlg = 0;
            AbstractSfxSingleTabDialog * pDlg = 0;
            // Tabellenzelle(n) selektiert?
            if ( rSh.IsTableMode() )
            {
                // Umrandungattribute Get/SetTabBorders() setzen
                ::PrepareBoxInfo( aSet, rSh );
                rSh.GetTabBorders( aSet );
                //CHINA001 pDlg = new SwBorderDlg( pMDI, aSet, SW_BORDER_MODE_TABLE );
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
                DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

                pDlg = pFact->CreateSwBorderDlg( pMDI, aSet, SW_BORDER_MODE_TABLE,ResId( RC_DLG_SWBORDERDLG ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if ( pDlg->Execute() == RET_OK )
                {
                    rSh.SetTabBorders( *pDlg->GetOutputItemSet() );
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            else if ( rSh.IsFrmSelected() )
            {
                // Umrandungsattribute ueber Frame-Manager setzen
                SwFlyFrmAttrMgr aMgr( FALSE, &rSh, FRMMGR_TYPE_NONE );
                aSet.Put( aMgr.GetAttrSet() );

                //CHINA001 pDlg = new SwBorderDlg( pMDI, aSet, SW_BORDER_MODE_FRAME );
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
                DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

                pDlg = pFact->CreateSwBorderDlg( pMDI, aSet, SW_BORDER_MODE_FRAME,ResId( RC_DLG_SWBORDERDLG ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if ( pDlg->Execute() == RET_OK )
                {
                    aMgr.SetAttrSet( *pDlg->GetOutputItemSet() );
                    aMgr.UpdateFlyFrm();
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            else
            {
                // Umrandungsattribute ganz normal ueber Shell setzen
                rSh.GetAttr( aSet );
                ::PrepareBoxInfo( aSet, rSh );

                //CHINA001 pDlg = new SwBorderDlg( pMDI, aSet, SW_BORDER_MODE_PARA );
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
                DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

                pDlg = pFact->CreateSwBorderDlg( pMDI, aSet, SW_BORDER_MODE_PARA,ResId( RC_DLG_SWBORDERDLG ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if ( pDlg->Execute() == RET_OK )
                {
                    rSh.SetAttr( *pDlg->GetOutputItemSet() );
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            delete pDlg;
        }
        break;
        case FN_FORMAT_BACKGROUND_DLG:
        {
            SfxItemSet aSet( rSh.GetAttrPool(),
                             RES_BACKGROUND, RES_BACKGROUND );

            //CHINA001 SwBackgroundDlg* pDlg = 0;
            AbstractSfxSingleTabDialog * pDlg = 0;//CHINA001
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
            DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001


            // Tabellenzelle(n) selektiert?
            if ( rSh.IsTableMode() )
            {
                //Hintergrundattribute der Tabelle holen und in den Set packen
                SvxBrushItem aBrush(RES_BACKGROUND);
                rSh.GetBoxBackground( aBrush );
                //CHINA001 pDlg = new SwBackgroundDlg( pMDI, aSet );
                pDlg = pFact->CreateSfxSingleTabDialog( pMDI, aSet,ResId( RC_SWDLG_BACKGROUND ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                aSet.Put( aBrush );
                if ( pDlg->Execute() == RET_OK )
                {
                    //aBrush = (SvxBrushItem) pDlg->GetOutputItemSet()->Get( RES_BACKGROUND );

                    rSh.SetBoxBackground( (SvxBrushItem&)
                        pDlg->GetOutputItemSet()->Get( RES_BACKGROUND ));
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            else if ( rSh.IsFrmSelected() )
            {

                rSh.GetFlyFrmAttr( aSet );

                //CHINA001 pDlg = new SwBackgroundDlg( pMDI, aSet );
                pDlg = pFact->CreateSfxSingleTabDialog( pMDI, aSet,ResId( RC_SWDLG_BACKGROUND ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if ( pDlg->Execute() == RET_OK )
                {
                    rSh.SetFlyFrmAttr((SfxItemSet &) *pDlg->GetOutputItemSet() );
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            else
            {
                // Umrandungsattribute ganz normal ueber Shell setzen
                rSh.GetAttr( aSet );

                //CHINA001 pDlg = new SwBackgroundDlg( pMDI, aSet );
                pDlg = pFact->CreateSfxSingleTabDialog( pMDI, aSet,ResId( RC_SWDLG_BACKGROUND ));
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if ( pDlg->Execute() == RET_OK )
                {
                    rSh.SetAttr( *pDlg->GetOutputItemSet() );
                    pOutSet = pDlg->GetOutputItemSet();
                }
            }
            delete pDlg;

        }
        break;
        default:DBG_ERROR("falscher Dispatcher (basesh.cxx)");
    }
    if(pOutSet)
        rReq.Done(*pOutSet);
    else
        rReq.Done();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

int SwBaseShell::InsertGraphic( const String &rPath, const String &rFilter,
                                BOOL bLink, GraphicFilter *pFlt,
                                Graphic* pPreviewGrf, BOOL bRule )
{
    SwWait aWait( *rView.GetDocShell(), TRUE );

    Graphic aGrf;
    int nRes = GRFILTER_OK;
    if ( pPreviewGrf )
        aGrf = *pPreviewGrf;
    else
    {
        if( !pFlt )
            pFlt = ::GetGrfFilter();
        Link aOldLink = pFlt->GetUpdatePercentHdl();
        pFlt->SetUpdatePercentHdl( LINK( this, SwBaseShell, UpdatePercentHdl ));
        ::StartProgress( STR_STATSTR_IMPGRF, 0, 100, rView.GetDocShell() );
        nRes = ::LoadGraphic( rPath, rFilter, aGrf, pFlt /*, nFilter*/ );
        ::EndProgress( rView.GetDocShell() );
        pFlt->SetUpdatePercentHdl( aOldLink );
    }

    if( GRFILTER_OK == nRes )
    {
        SwWrtShell &rSh = GetShell();
        rSh.StartAction();
        if( bLink )
            rSh.Insert( URIHelper::SmartRelToAbs( rPath ),
                        rFilter, aGrf, pFrmMgr, bRule );
        else
            rSh.Insert( aEmptyStr, aEmptyStr, aGrf, pFrmMgr );
        // nach dem EndAction ist es zu spaet, weil die Shell dann schon zerstoert sein kann
        DELETEZ(pFrmMgr);
        rSh.EndAction();
    }
    return nRes;
}


IMPL_LINK_INLINE_START( SwBaseShell, UpdatePercentHdl, GraphicFilter *, pFilter )
{
    ::SetProgressState( pFilter->GetPercent(), rView.GetDocShell() );
    return 0;
}
IMPL_LINK_INLINE_END( SwBaseShell, UpdatePercentHdl, GraphicFilter *, pFilter )


// ----------------------------------------------------------------------------


SwWrtShell& SwBaseShell::GetShell()
{
    return rView.GetWrtShell();
}

// ----------------------------------------------------------------------------

SwWrtShell* SwBaseShell::GetShellPtr()
{
    return rView.GetWrtShellPtr();
}

// ----------------------------------------------------------------------------

void SwBaseShell::InsertTable( SfxRequest& _rRequest )
{
    const SfxItemSet* pArgs = _rRequest.GetArgs();
    SwWrtShell& rSh = GetShell();

    if ( !( rSh.GetFrmType( 0, TRUE ) & FRMTYPE_FOOTNOTE ) )
    {
        SwView &rView = GetView(); // Da GetView() nach Shellwechsel nicht mehr geht
        BOOL bHTMLMode = 0 != (::GetHtmlMode(rView.GetDocShell())&HTMLMODE_ON);
        BOOL bCallEndUndo = FALSE;

        if( !pArgs && rSh.IsSelection() && !rSh.IsInClickToEdit() &&
            !rSh.IsTableMode() )
        {
            const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
            SwInsertTableOptions aInsTblOpts = pModOpt->GetInsTblFlags(bHTMLMode);

            rSh.StartUndo(UNDO_INSTABLE);
            bCallEndUndo = TRUE;

            BOOL bInserted = rSh.TextToTable( aInsTblOpts, '\t', HORI_FULL );
            rSh.EnterStdMode();
            if (bInserted)
                rView.AutoCaption(TABLE_CAP);
            _rRequest.Done();
        }
        else
        {
            USHORT nCols = 0;
            USHORT nRows = 0;
            SwInsertTableOptions aInsTblOpts( tabopts::ALL_TBL_INS_ATTR, 1 );
            String aTableName, aAutoName;
            SwTableAutoFmt* pTAFmt = 0;

            if( pArgs && pArgs->Count() >= 2 )
            {
                SFX_REQUEST_ARG( _rRequest, pName, SfxStringItem, FN_INSERT_TABLE, sal_False );
                SFX_REQUEST_ARG( _rRequest, pCols, SfxUInt16Item, SID_ATTR_TABLE_COLUMN, sal_False );
                SFX_REQUEST_ARG( _rRequest, pRows, SfxUInt16Item, SID_ATTR_TABLE_ROW, sal_False );
                SFX_REQUEST_ARG( _rRequest, pFlags, SfxInt32Item, FN_PARAM_1, sal_False );
                SFX_REQUEST_ARG( _rRequest, pAuto, SfxStringItem, FN_PARAM_2, sal_False );

                if ( pName )
                    aTableName = pName->GetValue();
                if ( pCols )
                    nCols = pCols->GetValue();
                if ( pRows )
                    nRows = pRows->GetValue();
                if ( pAuto )
                {
                    aAutoName = pAuto->GetValue();
                    if ( aAutoName.Len() )
                    {
                        SwTableAutoFmtTbl aTableTbl;
                        aTableTbl.Load();
                        for ( USHORT n=0; n<aTableTbl.Count(); n++ )
                        {
                            if ( aTableTbl[n]->GetName() == aAutoName )
                            {
                                pTAFmt = new SwTableAutoFmt( *aTableTbl[n] );
                                break;
                            }
                        }
                    }
                }

                if ( pFlags )
                    aInsTblOpts.mnInsMode = (USHORT) pFlags->GetValue();
                else
                {
                    const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
                    aInsTblOpts = pModOpt->GetInsTblFlags(bHTMLMode);
                }
            }

            if( !nCols || !nRows )
            {
                //CHINA001 SwInsTableDlg *pDlg = new SwInsTableDlg(rView);
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
                AbstractInsTableDlg* pDlg = pFact->CreateInsTableDlg( ResId(DLG_INSERT_TABLE), rView );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                if( RET_OK == pDlg->Execute() )
                {
                    pDlg->GetValues( aTableName, nRows, nCols, aInsTblOpts, aAutoName, pTAFmt );
                }
                else
                    _rRequest.Ignore();
                delete pDlg;
            }

            if( nCols && nRows )
            {
                // record before shell change
                _rRequest.AppendItem( SfxStringItem( FN_INSERT_TABLE, aTableName ) );
                if ( aAutoName.Len() )
                    _rRequest.AppendItem( SfxStringItem( FN_PARAM_2, aAutoName ) );
                _rRequest.AppendItem( SfxUInt16Item( SID_ATTR_TABLE_COLUMN, nCols ) );
                _rRequest.AppendItem( SfxUInt16Item( SID_ATTR_TABLE_ROW, nRows ) );
                _rRequest.AppendItem( SfxInt32Item( FN_PARAM_1, (sal_Int32) aInsTblOpts.mnInsMode ) );
                _rRequest.Done();

                rSh.StartUndo(UNDO_INSTABLE);
                bCallEndUndo = TRUE;

                rSh.StartAllAction();
                if( rSh.HasSelection() )
                    rSh.DelRight();

                rSh.InsertTable( aInsTblOpts, nRows, nCols, HORI_FULL, pTAFmt );
                rSh.MoveTable( fnTablePrev, fnTableStart );

                if( aTableName.Len() && !rSh.GetTblStyle( aTableName ) )
                    rSh.GetTableFmt()->SetName( aTableName );

                rSh.EndAllAction();
                rView.AutoCaption(TABLE_CAP);
            }
            delete pTAFmt;
        }

        if( bCallEndUndo )
            rSh.EndUndo(UNDO_INSTABLE);	// wegen moegl. Shellwechsel
    }
}

// ----------------------------------------------------------------------------

void SwBaseShell::GetGalleryState( SfxItemSet &rSet )
{
    SwWrtShell &rSh = GetShell();
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    switch ( nWhich )
    {
        case SID_GALLERY_BG_BRUSH:
        {
            int nSel = rSh.GetSelectionType();
            SfxStringListItem aLst( nWhich );
            List *pLst = aLst.GetList();
            nParagraphPos = nGraphicPos = nOlePos = nFramePos = nTablePos =
            nTableRowPos  = nTableCellPos = nPagePos =
            nHeaderPos 	  = nFooterPos = 0;
            BYTE nPos = 1;
            pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_PAGE ), pLst->Count() );
            nPagePos = nPos++;
            USHORT nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
            BOOL bHtmlMode = 0 != (nHtmlMode & HTMLMODE_ON);

            if ( (!bHtmlMode || (nHtmlMode & HTMLMODE_FULL_STYLES)) &&
                 (nSel & SwWrtShell::SEL_TXT) )
            {
                pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_PARAGRAPH ), pLst->Count() );
                nParagraphPos = nPos++;
            }
            if ( (!bHtmlMode || (nHtmlMode & HTMLMODE_SOME_STYLES)) &&
                    nSel & (SwWrtShell::SEL_TBL|SwWrtShell::SEL_TBL_CELLS) )
            {
                pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_TABLE ), pLst->Count() );
                nTablePos = nPos++;

                if(!bHtmlMode)
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_TABLE_ROW ), pLst->Count() );
                    nTableRowPos = nPos++;
                }

                pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_TABLE_CELL), pLst->Count() );
                nTableCellPos = nPos++;
            }
            if(!bHtmlMode)
            {
                if ( nSel & SwWrtShell::SEL_FRM )
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_FRAME ), pLst->Count() );
                    nFramePos = nPos++;
                }
                if ( nSel & SwWrtShell::SEL_GRF )
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_GRAPHIC ), pLst->Count() );
                    nGraphicPos = nPos++;
                }
                if ( nSel & SwWrtShell::SEL_OLE )
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_OLE ), pLst->Count() );
                    nOlePos = nPos++;
                }
                const USHORT nType = rSh.GetFrmType(0,TRUE);
                if ( nType & FRMTYPE_HEADER )
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_HEADER ), pLst->Count() );
                    nHeaderPos = nPos++;
                }
                if ( nType & FRMTYPE_FOOTER )
                {
                    pLst->Insert( (void*) new SW_RESSTR( STR_SWBG_FOOTER ), pLst->Count() );
                    nFooterPos = nPos;
                }
            }
            if ( pLst->Count() )
                rSet.Put( aLst );
            else
                rSet.DisableItem( nWhich );
            break;
        }
    }
}


void SwBaseShell::ExecuteGallery(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();
    rSh.StartAction();
    const SfxItemSet* pArgs = rReq.GetArgs();
    USHORT nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case SID_GALLERY_BG_BRUSH:
        {
            int nSel = rSh.GetSelectionType();
            if ( nSel & SwWrtShell::SEL_DRW_TXT )
                break;

            BYTE nPos = (BYTE)((SfxUInt16Item &)pArgs->Get(SID_GALLERY_BG_POS)).GetValue();
            ++nPos;

            SvxBrushItem aBrush( (SvxBrushItem&)pArgs->Get(SID_GALLERY_BG_BRUSH));
            aBrush.SetWhich( RES_BACKGROUND );
            if ( nPos == nParagraphPos )
                rSh.SetAttr( aBrush );
            else if ( nPos == nTablePos )
                rSh.SetTabBackground( aBrush );
            else if ( nPos == nTableRowPos )
                rSh.SetRowBackground( aBrush );
            else if ( nPos == nTableCellPos )
                rSh.SetBoxBackground( aBrush );
            else if ( nPos == nFramePos || nPos == nGraphicPos || nPos == nOlePos )
            {
                SfxItemSet aCoreSet(GetPool(), RES_BACKGROUND, RES_BACKGROUND);
                aCoreSet.Put( aBrush );
                rSh.SetFlyFrmAttr( aCoreSet );
            }
            else if ( nPos == nPagePos || nPos == nHeaderPos || nPos == nFooterPos )
            {
                USHORT nDesc = rSh.GetCurPageDesc();
                SwPageDesc aDesc( rSh.GetPageDesc( nDesc ) );
                if ( nPos == nPagePos )
                    aDesc.GetMaster().SetAttr( aBrush );
                else if ( nPos == nHeaderPos )
                {
                    SwFmtHeader aHead( aDesc.GetMaster().GetHeader() );
                    aHead.GetHeaderFmt()->SetAttr( aBrush );
                    aDesc.GetMaster().SetAttr( aHead );
                }
                else if ( nPos == nFooterPos )
                {
                    SwFmtFooter aFoot( aDesc.GetMaster().GetFooter() );
                    aFoot.GetFooterFmt()->SetAttr( aBrush );
                    aDesc.GetMaster().SetAttr( aFoot );
                }
                rSh.ChgPageDesc( nDesc, aDesc );
            }
            break;
        }
    }
    rSh.EndAction();
    rReq.Done();
}

void SwBaseShell::ExecField( SfxRequest& rReq )
{
    USHORT nSlot = rReq.GetSlot();
    switch( nSlot )
    {
        case FN_CHANGE_DBFIELD:
        {
            //CHINA001 SwChangeDBDlg *pDlg = new SwChangeDBDlg(GetView());
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
            DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

            VclAbstractDialog* pDlg = pFact->CreateSwChangeDBDlg(GetView(),ResId( DLG_CHANGE_DB ));
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
            pDlg->Execute();
            delete pDlg;
        }
        break;
        default:
            ASSERT(FALSE, falscher Dispatcher);
    }
}

