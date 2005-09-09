/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textidx.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:54:10 $
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


#pragma hdrstop
#include <hintids.hxx>
#include <uiparam.hxx>
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX //autogen
#include <svx/sizeitem.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _UIITEMS_HXX
#include <uiitems.hxx>
#endif
#include "viewopt.hxx"
#include "cmdid.h"
#include "view.hxx"
#include "wrtsh.hxx"
#include "swundo.hxx"               	// fuer Undo-Ids
#include "textsh.hxx"
#include "idxmrk.hxx"
//CHINA001 #include "multmrk.hxx"
#include "cnttab.hxx"
#include "toxmgr.hxx"
#include "swabstdlg.hxx" //CHINA001 
#include <index.hrc> //CHINA001 
#include <globals.hrc> //CHINA001
// STATIC DATA -----------------------------------------------------------

void SwTextShell::ExecIdx(SfxRequest &rReq)
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    USHORT nSlot = rReq.GetSlot();
    if(pArgs)
       pArgs->GetItemState(nSlot, FALSE, &pItem );

    SfxViewFrame* pVFrame = GetView().GetViewFrame();
    Window *pMDI = &pVFrame->GetWindow();

    switch( nSlot )
    {
        case FN_EDIT_AUTH_ENTRY_DLG :
        {
            //CHINA001 SwAuthMarkModalDlg* pDlg = new SwAuthMarkModalDlg(pMDI, GetShell());
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
            VclAbstractDialog* pDlg = pFact->CreateVclAbstractDialog( pMDI, GetShell(), ResId(DLG_EDIT_AUTHMARK) );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
            pDlg->Execute();
            delete pDlg;
        }
        break;
        case FN_INSERT_AUTH_ENTRY_DLG:
        {
            // no BASIC support
            pVFrame->ToggleChildWindow(FN_INSERT_AUTH_ENTRY_DLG);

            SwInsertAuthMarkWrapper *pAuthMrk = (SwInsertAuthMarkWrapper*)
                                pVFrame->GetChildWindow(FN_INSERT_AUTH_ENTRY_DLG);

            Invalidate(rReq.GetSlot());
        }
        break;
        case FN_INSERT_IDX_ENTRY_DLG:
        {
            pVFrame->ToggleChildWindow(FN_INSERT_IDX_ENTRY_DLG);
            Invalidate(rReq.GetSlot());
        }
        break;
        case FN_EDIT_IDX_ENTRY_DLG:
        {
            SwTOXMgr aMgr(GetShellPtr());
            USHORT nRet = RET_OK;
            if(aMgr.GetTOXMarkCount() > 1)
            {	// Mehrere Marken, welche solls denn sein ?
                //
                //CHINA001 SwMultiTOXMarkDlg* pMultDlg = new SwMultiTOXMarkDlg(pMDI, aMgr);
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
                VclAbstractDialog* pMultDlg = pFact->CreateMultiTOXMarkDlg( ResId(DLG_MULTMRK),
                                                        pMDI, aMgr);
                DBG_ASSERT(pMultDlg, "Dialogdiet fail!");//CHINA001
                nRet = pMultDlg->Execute();
                delete pMultDlg;
            }
            if(	nRet == RET_OK)
            {
                //CHINA001 SwIndexMarkModalDlg* pDlg = new SwIndexMarkModalDlg(pMDI, GetShell(), aMgr.GetCurTOXMark());
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
                VclAbstractDialog* pDlg = pFact->CreateIndexMarkModalDlg( ResId(DLG_EDIT_IDXMARK), pMDI, GetShell(), aMgr.GetCurTOXMark() );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                pDlg->Execute();
                delete pDlg;
            }
            break;
        }
        case FN_IDX_MARK_TO_IDX:
        {
            GetShell().GotoTOXMarkBase();
            break;
        }
        case FN_INSERT_MULTI_TOX:
        {
            SfxItemSet aSet(GetPool(),
                            RES_COL, RES_COL,
                            RES_BACKGROUND, RES_BACKGROUND,
                            RES_FRM_SIZE, RES_FRM_SIZE,
                            SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
                            RES_LR_SPACE, RES_LR_SPACE,
                            FN_PARAM_TOX_TYPE, FN_PARAM_TOX_TYPE,
                            0	);
            SwWrtShell& rSh = GetShell();
            SwRect aRect;
            rSh.CalcBoundRect(aRect, FLY_IN_CNTNT);

            long nWidth = aRect.Width();
            aSet.Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth));
            // Hoehe=Breite fuer konsistentere Vorschau (analog zu Bereich bearbeiten)
            aSet.Put(SvxSizeItem(SID_ATTR_PAGE_SIZE, Size(nWidth, nWidth)));
            const SwTOXBase* pCurTOX = 0;
            BOOL bGlobal = FALSE;
            if(pItem)
            {
                pCurTOX = (const SwTOXBase* )((SwPtrItem*)pItem)->GetValue();
                bGlobal = TRUE;
            }
            else
                pCurTOX = rSh.GetCurTOX();
            if(pCurTOX)
            {
                const SfxItemSet* pSet = pCurTOX->GetAttrSet();
                if(pSet)
                    aSet.Put(*pSet);
            }

//CHINA001			SwMultiTOXTabDialog* pDlg = new SwMultiTOXTabDialog(pMDI, aSet, rSh, (SwTOXBase* )pCurTOX,
//CHINA001			USHRT_MAX, bGlobal);
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
            AbstractMultiTOXTabDialog* pDlg = pFact->CreateMultiTOXTabDialog( ResId(DLG_MULTI_TOX),
                                                        pMDI, aSet, rSh, (SwTOXBase* )pCurTOX,
                                                        USHRT_MAX, bGlobal);
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001			
            pDlg->Execute();
            delete pDlg;
        }
        break;
        case FN_REMOVE_CUR_TOX:
        {
            SwWrtShell& rSh = GetShell();
            const SwTOXBase* pBase = rSh.GetCurTOX();
            DBG_ASSERT(pBase, "no TOXBase to remove")
            if( pBase )
                rSh.DeleteTOX(*pBase, TRUE);
        }
        break;
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}


void SwTextShell::GetIdxState(SfxItemSet &rSet)
{
    SwWrtShell& rSh = GetShell();
    SfxViewFrame* pVFrame = GetView().GetViewFrame();
    SwInsertIdxMarkWrapper *pIdxMrk = (SwInsertIdxMarkWrapper*)
                        pVFrame->GetChildWindow(FN_INSERT_IDX_ENTRY_DLG);

    SfxChildWindow* pAuthMark = pVFrame->GetChildWindow(FN_INSERT_AUTH_ENTRY_DLG);

    const BOOL bHtmlMode = 0 != ::GetHtmlMode( GetView().GetDocShell() );
    const SwTOXBase* pBase = 0;
    if( bHtmlMode || 0 != ( pBase = rSh.GetCurTOX()) )
    {
        USHORT nBase = 0;
        if( pBase )
        {
            if(pBase->IsTOXBaseInReadonly())
            {
                rSet.DisableItem( FN_INSERT_MULTI_TOX );
            }
        }

        rSet.DisableItem( FN_EDIT_IDX_ENTRY_DLG );
        rSet.DisableItem( FN_EDIT_AUTH_ENTRY_DLG );

        if(!pIdxMrk)
            rSet.DisableItem( FN_INSERT_IDX_ENTRY_DLG );
        else
            rSet.Put(SfxBoolItem(FN_INSERT_IDX_ENTRY_DLG, TRUE));

        if(!pAuthMark)
            rSet.DisableItem( FN_INSERT_AUTH_ENTRY_DLG );
        else
            rSet.Put(SfxBoolItem(FN_INSERT_AUTH_ENTRY_DLG, TRUE));

    }
    else //if( SFX_ITEM_UNKNOWN != rSet.GetItemState( FN_EDIT_IDX_ENTRY_DLG ))
    {

        BOOL bEnableEdit = TRUE;
        BOOL bInReadonly = rSh.HasReadonlySel();
        if( rSh.HasSelection() || bInReadonly)
            bEnableEdit = FALSE;
        else
        {
            SwTOXMarks aArr;
            rSh.GetCurTOXMarks( aArr );
            if( !aArr.Count())
                bEnableEdit = FALSE;
        }

        if(!bEnableEdit)
            rSet.DisableItem( FN_EDIT_IDX_ENTRY_DLG );

        if(bInReadonly)
        {
            rSet.DisableItem(FN_INSERT_IDX_ENTRY_DLG);
            rSet.DisableItem( FN_INSERT_MULTI_TOX );
        }
        else
            rSet.Put(SfxBoolItem(FN_INSERT_IDX_ENTRY_DLG,
                                    0 != pIdxMrk));


        SwField* pField = rSh.GetCurFld();

        if(bInReadonly)
            rSet.DisableItem(FN_INSERT_AUTH_ENTRY_DLG);
        else
            rSet.Put(SfxBoolItem(FN_INSERT_AUTH_ENTRY_DLG, 0 != pAuthMark));

        if(	bInReadonly || !pField ||
            pField->GetTyp()->Which() != RES_AUTHORITY)
            rSet.DisableItem(FN_EDIT_AUTH_ENTRY_DLG);
        rSet.DisableItem(FN_REMOVE_CUR_TOX);
    }
}

// -----------------------------------------------------------------------

