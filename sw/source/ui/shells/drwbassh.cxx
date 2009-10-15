/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: drwbassh.cxx,v $
 * $Revision: 1.29 $
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


#include <tools/shl.hxx>
#include <hintids.hxx>
#include <helpid.h>
#include <swtypes.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/request.hxx>
#include <sfx2/bindings.hxx>
#include <svl/aeitem.hxx>
#include <svx/svdview.hxx>
#include <vcl/msgbox.hxx>
#include <svx/srchitem.hxx>
#include <svl/whiter.hxx>
#include <svx/swframevalidation.hxx>
#include <svx/anchorid.hxx>
#include <svx/htmlmode.hxx>
#include <uitool.hxx>
#include <fmtornt.hxx>
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <wview.hxx>
#include <edtwin.hxx>
#include <viewopt.hxx>
#include <dcontact.hxx>
#include <frmfmt.hxx>
#include <wrap.hxx>
#include <drawbase.hxx>
#include <drwbassh.hxx>
#include <swdtflvr.hxx>
#include <svx/svdogrp.hxx>
#include <svx/svdpage.hxx>
#include <svx/svditer.hxx>

#include <shells.hrc>
#define SwDrawBaseShell
#include <sfx2/msg.hxx>
#include <swslots.hxx>
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include "swabstdlg.hxx"
#include "dialog.hrc"
#include <swundo.hxx>
#include <com/sun/star/text/HoriOrientation.hpp>
#include <com/sun/star/text/VertOrientation.hpp>
#include <com/sun/star/text/RelOrientation.hpp>

#include <IDocumentDrawModelAccess.hxx>

using namespace ::com::sun::star;

SFX_IMPL_INTERFACE(SwDrawBaseShell, SwBaseShell, SW_RES(0))
{
}

TYPEINIT1(SwDrawBaseShell,SwBaseShell)

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwDrawBaseShell::SwDrawBaseShell(SwView &_rView):
    SwBaseShell( _rView )
{
    GetShell().NoEdit(TRUE);

    SwEditWin& rWin = GetView().GetEditWin();

    rWin.SetBezierMode(SID_BEZIER_MOVE);

    if ( !_rView.GetDrawFuncPtr() )
        _rView.GetEditWin().StdDrawMode( OBJ_NONE, TRUE );

    SwTransferable::CreateSelection( GetShell() );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwDrawBaseShell::~SwDrawBaseShell()
{
    GetView().ExitDraw();
    GetShell().Edit();
    SwTransferable::ClearSelection( GetShell() );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwDrawBaseShell::Execute(SfxRequest &rReq)
{
    SwWrtShell *pSh = &GetShell();
    SdrView*	pSdrView = pSh->GetDrawView();
    const SfxItemSet *pArgs = rReq.GetArgs();
    USHORT		nSlotId = rReq.GetSlot();
    BOOL		bChanged = pSdrView->GetModel()->IsChanged();
    pSdrView->GetModel()->SetChanged(FALSE);
    const SfxPoolItem* pItem = 0;
    if(pArgs)
        pArgs->GetItemState(nSlotId, FALSE, &pItem);

    //Sonderfall Align per Menue
    if(pItem && nSlotId == SID_OBJECT_ALIGN)
    {
        DBG_ASSERT(PTR_CAST(SfxEnumItem, pItem),"SfxEnumItem erwartet");
        nSlotId = nSlotId + ((const SfxEnumItem*)pItem)->GetValue();
        nSlotId++;
    }

    BOOL bAlignPossible = pSh->IsAlignPossible();

    BOOL bTopParam = TRUE, bBottomParam = TRUE;
    BOOL bNotify = FALSE;
    BOOL bDone = FALSE;
    SfxBindings& rBind = GetView().GetViewFrame()->GetBindings();

    switch (nSlotId)
    {
        case FN_DRAW_WRAP_DLG:
        {
            if(pSdrView->AreObjectsMarked())
            {
                if(!pArgs)
                {
                    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
                    if( rMarkList.GetMark(0) != 0 )
                    {
                        SfxItemSet aSet(GetPool(),	RES_SURROUND, RES_SURROUND,
                                                    RES_ANCHOR, RES_ANCHOR,
                                                    RES_LR_SPACE, RES_UL_SPACE,
                                                    SID_HTML_MODE, SID_HTML_MODE,
                                                    FN_DRAW_WRAP_DLG, FN_DRAW_WRAP_DLG,
                                                    0);

                        aSet.Put(SfxBoolItem(SID_HTML_MODE,
                            0 != ::GetHtmlMode(pSh->GetView().GetDocShell())));

                        aSet.Put(SfxInt16Item(FN_DRAW_WRAP_DLG, pSh->GetLayerId()));

                        pSh->GetObjAttr(aSet);
                        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                        DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

                        SfxAbstractDialog* pDlg = pFact->CreateSwWrapDlg( GetView().GetWindow(), aSet, pSh, TRUE, RC_DLG_SWWRAPDLG );
                        DBG_ASSERT(pDlg, "Dialogdiet fail!");

                        if (pDlg->Execute() == RET_OK)
                        {
                            const SfxPoolItem* pWrapItem;
                            const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                            if(SFX_ITEM_SET == pOutSet->GetItemState(FN_DRAW_WRAP_DLG, FALSE, &pWrapItem))
                            {
                                short nLayer = ((const SfxInt16Item*)pWrapItem)->GetValue();
                                if (nLayer == 1)
                                    pSh->SelectionToHeaven();
                                else
                                    pSh->SelectionToHell();
                            }

                            pSh->SetObjAttr(*pOutSet);
                        }
                    delete pDlg;
                    }
                }
            }
        }
        break;

        case SID_ATTR_TRANSFORM:
        {
            if(pSdrView->AreObjectsMarked())
            {
                if(!pArgs)
                {
                    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
                    if( rMarkList.GetMark(0) != 0 )
                    {
                        SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
                        SfxAbstractTabDialog *pDlg=NULL;
                        BOOL bCaption = FALSE;

                        // Erlaubte Verankerungen:
                        short nAnchor = pSh->GetAnchorId();
                        USHORT nAllowedAnchors = SVX_OBJ_AT_CNTNT|SVX_OBJ_IN_CNTNT;
                        USHORT nHtmlMode = ::GetHtmlMode(pSh->GetView().GetDocShell());

                        if( !((HTMLMODE_ON & nHtmlMode) && (0 == (nHtmlMode & HTMLMODE_SOME_ABS_POS))) )
                            nAllowedAnchors |= SVX_OBJ_PAGE;
                        if ( pSh->IsFlyInFly() )
                            nAllowedAnchors |= SVX_OBJ_AT_FLY;

                        if (pObj->GetObjIdentifier() == OBJ_CAPTION )
                            bCaption = TRUE;

                        if (bCaption)
                        {
                            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                            if ( pFact )
                            {
                                AbstractSvxCaptionDialog* pCaptionDlg =
                                        pFact->CreateCaptionDialog( NULL, pSdrView, RID_SVXDLG_CAPTION, nAllowedAnchors );
                                pCaptionDlg->SetValidateFramePosLink( LINK(this, SwDrawBaseShell, ValidatePosition) );
                                pDlg = pCaptionDlg;
                                DBG_ASSERT(pDlg, "Dialogdiet fail!");
                            }
                        }
                        else
                        {
                            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                            if ( pFact )
                            {

                                AbstractSvxTransformTabDialog* pTransform =
                                            pFact->CreateSvxTransformTabDialog( NULL, NULL, pSdrView, RID_SVXDLG_TRANSFORM, nAllowedAnchors );
                                pTransform->SetValidateFramePosLink( LINK(this, SwDrawBaseShell, ValidatePosition) );
                                pDlg = pTransform;
                                DBG_ASSERT(pDlg, "Dialogdiet fail!");
                            }
                        }
                        SfxItemSet aNewAttr(pSdrView->GetGeoAttrFromMarked());

                        const USHORT* pRange = pDlg->GetInputRanges( *aNewAttr.GetPool() );
                        SfxItemSet aSet( *aNewAttr.GetPool(), pRange );
                        FieldUnit eMetric = ::GetDfltMetric(0 != dynamic_cast<SwWebView*>(&GetView()));
                        SW_MOD()->PutItem(SfxUInt16Item(SID_ATTR_METRIC, static_cast< UINT16 >(eMetric)) );

                        aSet.Put( aNewAttr, FALSE );

                        if (bCaption)
                            pSdrView->GetAttributes( aSet );

                        aSet.Put(SfxInt16Item(SID_ATTR_TRANSFORM_ANCHOR, nAnchor));
                        BOOL bRTL;
                        aSet.Put(SfxBoolItem(SID_ATTR_TRANSFORM_IN_VERTICAL_TEXT, pSh->IsFrmVertical(TRUE, bRTL)));
                        aSet.Put(SfxBoolItem(SID_ATTR_TRANSFORM_IN_RTL_TEXT, bRTL));

                        SwFrmFmt* pFrmFmt = FindFrmFmt( pObj );

                        aSet.Put( pFrmFmt->GetFmtAttr(RES_FOLLOW_TEXT_FLOW) );

                        SwFmtVertOrient aVOrient((const SwFmtVertOrient&)pFrmFmt->GetFmtAttr(RES_VERT_ORIENT));
                        aSet.Put(SfxInt16Item(SID_ATTR_TRANSFORM_VERT_ORIENT, aVOrient.GetVertOrient()));
                        aSet.Put(SfxInt16Item(SID_ATTR_TRANSFORM_VERT_RELATION, aVOrient.GetRelationOrient() ));
                        aSet.Put(SfxInt32Item(SID_ATTR_TRANSFORM_VERT_POSITION, aVOrient.GetPos()));

                        SwFmtHoriOrient aHOrient((const SwFmtHoriOrient&)pFrmFmt->GetFmtAttr(RES_HORI_ORIENT));
                        aSet.Put(SfxInt16Item(SID_ATTR_TRANSFORM_HORI_ORIENT, aHOrient.GetHoriOrient()));
                        aSet.Put(SfxInt16Item(SID_ATTR_TRANSFORM_HORI_RELATION, aHOrient.GetRelationOrient() ));
                        aSet.Put(SfxBoolItem(SID_ATTR_TRANSFORM_HORI_MIRROR, aHOrient.IsPosToggle()));
                        aSet.Put(SfxInt32Item(SID_ATTR_TRANSFORM_HORI_POSITION, aHOrient.GetPos()));

                        aSet.Put(SfxUInt16Item(SID_HTML_MODE, nHtmlMode));

                        pDlg->SetInputSet( &aSet );

                        if (pDlg->Execute() == RET_OK)
                        {
                            const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                            pSh->StartAllAction();

                            // --> OD 2004-07-14 #i30451#
                            pSh->StartUndo();

                            pSdrView->SetGeoAttrToMarked(*pOutSet);

                            if (bCaption)
                                pSdrView->SetAttributes(*pOutSet);

                            BOOL bPosCorr =
                                SFX_ITEM_SET != pOutSet->GetItemState(
                                    SID_ATTR_TRANSFORM_POS_X, FALSE ) &&
                                SFX_ITEM_SET != pOutSet->GetItemState(
                                    SID_ATTR_TRANSFORM_POS_Y, FALSE );

                            SfxItemSet aFrmAttrSet(GetPool(), RES_FRMATR_BEGIN, RES_FRMATR_END - 1);

                            bool bSingleSelection = rMarkList.GetMarkCount() == 1;

                            const SfxPoolItem* pAnchorItem;
                            if(SFX_ITEM_SET == pOutSet->GetItemState(
                                SID_ATTR_TRANSFORM_ANCHOR, FALSE, &pAnchorItem))
                            {
                                if(!bSingleSelection)
                                    pSh->ChgAnchor(((const SfxInt16Item*)pAnchorItem)
                                            ->GetValue(), FALSE, bPosCorr );
                                else
                                {
                                    SwFmtAnchor aAnchor(pFrmFmt->GetAnchor());
                                    aAnchor.SetType((RndStdIds)((const SfxInt16Item*)pAnchorItem)->GetValue());
                                    aFrmAttrSet.Put( aAnchor );
                                }
                            }
                            const SfxPoolItem* pHoriOrient = 0;
                            const SfxPoolItem* pHoriRelation = 0;
                            const SfxPoolItem* pHoriPosition = 0;
                            const SfxPoolItem* pHoriMirror = 0;
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_HORI_ORIENT, FALSE, &pHoriOrient);
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_HORI_RELATION, FALSE, &pHoriRelation);
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_HORI_POSITION, FALSE, &pHoriPosition);
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_HORI_MIRROR, FALSE, &pHoriMirror);
                            if(pHoriOrient || pHoriRelation || pHoriPosition || pHoriMirror)
                            {
                                if(pHoriOrient)
                                    aHOrient.SetHoriOrient(
                                          static_cast<const SfxInt16Item*>(pHoriOrient)->GetValue());
                                if(pHoriRelation)
                                    aHOrient.SetRelationOrient(
                                              static_cast<const SfxInt16Item*>(pHoriRelation)->GetValue());
                                if(pHoriPosition)
                                    aHOrient.SetPos( static_cast<const SfxInt32Item*>(pHoriPosition)->GetValue());
                                if(pHoriMirror)
                                    aHOrient.SetPosToggle( static_cast<const SfxBoolItem*>(pHoriMirror)->GetValue());
                                aFrmAttrSet.Put(aHOrient);
                            }

                            const SfxPoolItem* pVertOrient = 0;
                            const SfxPoolItem* pVertRelation = 0;
                            const SfxPoolItem* pVertPosition = 0;
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_VERT_ORIENT, FALSE, &pVertOrient);
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_VERT_RELATION, FALSE, &pVertRelation);
                            pOutSet->GetItemState(SID_ATTR_TRANSFORM_VERT_POSITION, FALSE, &pVertPosition);
                            if(pVertOrient || pVertRelation || pVertPosition )
                            {
                                if(pVertOrient)
                                    aVOrient.SetVertOrient(
                                        static_cast<const SfxInt16Item*>(pVertOrient)->GetValue());
                                if(pVertRelation)
                                    aVOrient.SetRelationOrient(
                                        static_cast<const SfxInt16Item*>(pVertRelation)->GetValue());
                                if(pVertPosition)
                                    aVOrient.SetPos( static_cast<const SfxInt32Item*>(pVertPosition)->GetValue());
                                aFrmAttrSet.Put( aVOrient );
                            }
                            const SfxPoolItem* pFollowItem = 0;
                            pOutSet->GetItemState(RES_FOLLOW_TEXT_FLOW, FALSE, &pFollowItem);
                            if(pFollowItem)
                                aFrmAttrSet.Put(*pFollowItem);

                            if(aFrmAttrSet.Count())
                                pSh->SetDrawingAttr(aFrmAttrSet);

                            rBind.InvalidateAll(FALSE);

                            // --> OD 2004-07-14 #i30451#
                            pSh->EndUndo( UNDO_INSFMTATTR );

                            pSh->EndAllAction();
                        }
                        delete pDlg;

                    }
                }
            }
        }
        break;

        case SID_DELETE:
        case FN_BACKSPACE:
            if (pSh->IsObjSelected() && !pSdrView->IsTextEdit())
            {
                bDone = TRUE;

                if( GetView().IsDrawRotate() )
                {
                    pSh->SetDragMode( SDRDRAG_MOVE );
                    GetView().FlipDrawRotate();
                }

                pSh->SetModified();
                pSh->DelSelectedObj();

                if (rReq.IsAPI() ||
                    GetView().GetEditWin().IsObjectSelect() )
                {
                    // Wenn Basic-Aufruf, dann zurueck in die Textshell, da das
                    // Basic sonst keine Rueckkehrmoeglichkeit hat.
                    if (GetView().GetDrawFuncPtr())
                    {
                        GetView().GetDrawFuncPtr()->Deactivate();
                        GetView().SetDrawFuncPtr(NULL);
                    }
                    GetView().LeaveDrawCreate();	// In Selektionsmode wechseln
                }

                if (pSh->IsSelFrmMode())
                {
                    pSh->LeaveSelFrmMode();
                    // #105852# FME
//                   pSh->NoEdit();
                }
                bNotify = TRUE;
            }
            break;

        case SID_GROUP:
            if (pSh->IsObjSelected() > 1 && pSh->IsGroupAllowed())
            {
                pSh->GroupSelection();	// Objekt gruppieren
                rBind.Invalidate(SID_UNGROUP);
            }
            break;

        case SID_UNGROUP:
            if (pSh->IsGroupSelected())
            {
                pSh->UnGroupSelection();	// Objektgruppierung aufheben
                rBind.Invalidate(SID_GROUP);
            }
            break;

        case SID_ENTER_GROUP:
            if (pSh->IsGroupSelected())
            {
                pSdrView->EnterMarkedGroup();
                rBind.InvalidateAll(FALSE);
            }
            break;

        case SID_LEAVE_GROUP:
            if (pSdrView->IsGroupEntered())
            {
                pSdrView->LeaveOneGroup();
                rBind.Invalidate(SID_ENTER_GROUP);
                rBind.Invalidate(SID_UNGROUP);
            }
            break;

        case SID_OBJECT_ALIGN_LEFT:
        case SID_OBJECT_ALIGN_CENTER:
        case SID_OBJECT_ALIGN_RIGHT:
        case SID_OBJECT_ALIGN_UP:
        case SID_OBJECT_ALIGN_MIDDLE:
        case SID_OBJECT_ALIGN_DOWN:
        {
            if ( bAlignPossible )
            {
                const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
                if( rMarkList.GetMarkCount() == 1 && bAlignPossible )
                {	// Objekte nicht aneinander ausrichten

                    USHORT nAnchor = pSh->GetAnchorId();
                    if (nAnchor == FLY_IN_CNTNT)
                    {
                        sal_Int16 nVertOrient = -1;

                        switch (nSlotId)
                        {
                            case SID_OBJECT_ALIGN_UP:
                                nVertOrient = text::VertOrientation::TOP;
                                break;
                            case SID_OBJECT_ALIGN_MIDDLE:
                                nVertOrient = text::VertOrientation::CENTER;
                                break;
                            case SID_OBJECT_ALIGN_DOWN:
                                nVertOrient = text::VertOrientation::BOTTOM;
                                break;
                            default:
                                break;
                        }
                        if (nVertOrient != -1)
                        {
                            pSh->StartAction();
                            SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
                            SwFrmFmt* pFrmFmt = FindFrmFmt( pObj );
                            SwFmtVertOrient aVOrient((SwFmtVertOrient&)pFrmFmt->GetFmtAttr(RES_VERT_ORIENT));
                            aVOrient.SetVertOrient( nVertOrient );
                            pFrmFmt->SetFmtAttr(aVOrient);
                            pSh->EndAction();
                        }
                        break;
                    }
                    if (nAnchor == FLY_AT_CNTNT)
                        break;	// Absatzverankerte Rahmen nicht ausrichten
                }

                pSh->StartAction();
                switch (nSlotId)
                {
                    case SID_OBJECT_ALIGN_LEFT:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_LEFT, SDRVALIGN_NONE);
                        break;
                    case SID_OBJECT_ALIGN_CENTER:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_CENTER, SDRVALIGN_NONE);
                        break;
                    case SID_OBJECT_ALIGN_RIGHT:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_RIGHT, SDRVALIGN_NONE);
                        break;
                    case SID_OBJECT_ALIGN_UP:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_NONE, SDRVALIGN_TOP);
                        break;
                    case SID_OBJECT_ALIGN_MIDDLE:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_NONE, SDRVALIGN_CENTER);
                        break;
                    case SID_OBJECT_ALIGN_DOWN:
                        pSdrView->AlignMarkedObjects(SDRHALIGN_NONE, SDRVALIGN_BOTTOM);
                        break;
                }
                pSh->EndAction();
            }
        }
        break;

        case FN_FRAME_UP:
            bTopParam = FALSE;
            /* no break */
        case SID_FRAME_TO_TOP:
            pSh->SelectionToTop( bTopParam );
            break;

        case FN_FRAME_DOWN:
            bBottomParam = FALSE;
            /* no break */
        case SID_FRAME_TO_BOTTOM:
            pSh->SelectionToBottom( bBottomParam );
            break;

        case FN_NAME_SHAPE:
        {
            bDone = TRUE;

            if(1L == pSdrView->GetMarkedObjectCount())
            {
                // #i68101#
                SdrObject* pSelected = pSdrView->GetMarkedObjectByIndex(0L);
                OSL_ENSURE(pSelected, "DrawViewShell::FuTemp03: nMarkCount, but no object (!)");
                String aName(pSelected->GetName());

                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "Dialogdiet fail!");
                AbstractSvxObjectNameDialog* pDlg = pFact->CreateSvxObjectNameDialog(NULL, aName, RID_SVXDLG_OBJECT_NAME);
                OSL_ENSURE(pDlg, "Dialogdiet fail!");

                pDlg->SetCheckNameHdl(LINK(this, SwDrawBaseShell, CheckGroupShapeNameHdl));

                if(RET_OK == pDlg->Execute())
                {
                    pDlg->GetName(aName);
                    pSelected->SetName(aName);
                    pSh->SetModified();
                }

                delete pDlg;
            }

            break;
        }

        // #i68101#
        case FN_TITLE_DESCRIPTION_SHAPE:
        {
            bDone = TRUE;

            if(1L == pSdrView->GetMarkedObjectCount())
            {
                SdrObject* pSelected = pSdrView->GetMarkedObjectByIndex(0L);
                OSL_ENSURE(pSelected, "DrawViewShell::FuTemp03: nMarkCount, but no object (!)");
                String aTitle(pSelected->GetTitle());
                String aDescription(pSelected->GetDescription());

                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "Dialogdiet fail!");
                AbstractSvxObjectTitleDescDialog* pDlg = pFact->CreateSvxObjectTitleDescDialog(NULL, aTitle, aDescription, RID_SVXDLG_OBJECT_TITLE_DESC);
                OSL_ENSURE(pDlg, "Dialogdiet fail!");

                if(RET_OK == pDlg->Execute())
                {
                    pDlg->GetTitle(aTitle);
                    pDlg->GetDescription(aDescription);

                    pSelected->SetTitle(aTitle);
                    pSelected->SetDescription(aDescription);

                    pSh->SetModified();
                }

                delete pDlg;
            }

            break;
        }

        default:
            DBG_ASSERT(!this, "falscher Dispatcher");
            return;
    }
    if(!bDone)
    {
        if(nSlotId >= SID_OBJECT_ALIGN_LEFT && nSlotId <= SID_OBJECT_ALIGN_DOWN)
            rBind.Invalidate(SID_ATTR_LONG_LRSPACE);
        if (pSdrView->GetModel()->IsChanged())
            pSh->SetModified();
        else if (bChanged)
            pSdrView->GetModel()->SetChanged(TRUE);
        // 40220: Nach dem Loeschen von DrawObjekten ueber die API GPF durch Selbstzerstoerung
        if(bNotify)
            GetView().AttrChangedNotify(pSh); // ggf Shellwechsel...
    }
}
/* -----------------------------27.02.2002 15:27------------------------------
    Checks whether a given name is allowed for a group shape
 ---------------------------------------------------------------------------*/
IMPL_LINK( SwDrawBaseShell, CheckGroupShapeNameHdl, AbstractSvxNameDialog*, pNameDialog )
{
    SwWrtShell          &rSh = GetShell();
    SdrView *pSdrView = rSh.GetDrawView();
    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
    DBG_ASSERT(rMarkList.GetMarkCount() == 1, "wrong draw selection");
    SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
    const String sCurrentName = pObj->GetName();
    String sNewName;
    pNameDialog->GetName(sNewName);
    long nRet = 0;
    if(!sNewName.Len() || sCurrentName == sNewName)
        nRet = 1;
    else
    {
        nRet = 1;
        SdrModel* pModel = rSh.getIDocumentDrawModelAccess()->GetDrawModel();
        // --> OD 2006-03-09 #i51726# - all drawing objects can be named now.
        // consider also drawing objects inside group objects
//        SdrPage* pPage = pModel->GetPage(0);
//        sal_uInt32 nCount = pPage->GetObjCount();
//        for( sal_uInt32 i=0; i< nCount; i++ )
//        {
//            SdrObject* pTemp = pPage->GetObj(i);
//            if(pObj != pTemp && pTemp->ISA(SdrObjGroup) && pTemp->GetName() == sNewName)
//            {
//                nRet = 0;
//                break;
//            }
//        }
        SdrObjListIter aIter( *(pModel->GetPage(0)), IM_DEEPWITHGROUPS );
        while( aIter.IsMore() )
        {
            SdrObject* pTempObj = aIter.Next();
            if ( pObj != pTempObj && pTempObj->GetName() == sNewName )
            {
                nRet = 0;
                break;
            }
        }
        // <--
    }
    return nRet;
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwDrawBaseShell::GetState(SfxItemSet& rSet)
{
    SwWrtShell &rSh = GetShell();
    SdrView* pSdrView = rSh.GetDrawViewWithValidMarkList();
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    BOOL bProtected = rSh.IsSelObjProtected(FLYPROTECT_CONTENT);

    if (!bProtected)	// Im Parent nachsehen
        bProtected |= rSh.IsSelObjProtected( FLYPROTECT_CONTENT|FLYPROTECT_PARENT ) != 0;

    while( nWhich )
    {
        switch( nWhich )
        {
            case FN_DRAW_WRAP_DLG:
            case SID_ATTR_TRANSFORM:
            case SID_FRAME_TO_TOP:
            case SID_FRAME_TO_BOTTOM:
            case FN_FRAME_UP:
            case FN_FRAME_DOWN:
            case SID_DELETE:
            case FN_BACKSPACE:
                if( bProtected || !rSh.IsObjSelected() )
                    rSet.DisableItem( nWhich );
                break;
            case SID_GROUP:
                if ( rSh.IsObjSelected() < 2 || bProtected || !rSh.IsGroupAllowed() )
                    rSet.DisableItem( nWhich );
                break;
            case SID_UNGROUP:
                if ( !rSh.IsGroupSelected() || bProtected )
                    rSet.DisableItem( nWhich );
                break;
            case SID_ENTER_GROUP:
                if ( !rSh.IsGroupSelected() )
                    rSet.DisableItem( nWhich );
                break;
            case SID_LEAVE_GROUP:
                if ( !pSdrView->IsGroupEntered() )
                    rSet.DisableItem( nWhich );
                break;
            case SID_OBJECT_ALIGN_LEFT:
            case SID_OBJECT_ALIGN_CENTER:
            case SID_OBJECT_ALIGN_RIGHT:
            case SID_OBJECT_ALIGN_UP:
            case SID_OBJECT_ALIGN_MIDDLE:
            case SID_OBJECT_ALIGN_DOWN:
            case SID_OBJECT_ALIGN:
                if ( !rSh.IsAlignPossible() || bProtected )
                    rSet.DisableItem( nWhich );
                else
                {
                    SfxAllEnumItem aEnumItem(nWhich, USHRT_MAX);
                    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
                    //if only one object is selected it can only be vertically
                    // aligned because it is character bound
                    if( rMarkList.GetMarkCount() == 1 )
                    {
                        aEnumItem.DisableValue(SID_OBJECT_ALIGN_LEFT);
                        aEnumItem.DisableValue(SID_OBJECT_ALIGN_CENTER);
                        aEnumItem.DisableValue(SID_OBJECT_ALIGN_RIGHT);
                    }
                    rSet.Put(aEnumItem);
                }
                break;

            case FN_NAME_SHAPE :
                {
                    if(1L != pSdrView->GetMarkedObjectCount())
                    {
                        rSet.DisableItem( nWhich );
                    }
                }
                break;

            // #i68101#
            case FN_TITLE_DESCRIPTION_SHAPE:
                {
                    const bool bIsWebView(NULL != dynamic_cast<SwWebView*>(&GetView()));

                    if(!bIsWebView && 1L != pSdrView->GetMarkedObjectCount())
                    {
                        rSet.DisableItem( nWhich );
                    }
                }
                break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


BOOL SwDrawBaseShell::Disable(SfxItemSet& rSet, USHORT nWhich)
{
    BOOL bDisable = GetShell().IsSelObjProtected(FLYPROTECT_CONTENT);

    if (bDisable)
    {
        if (nWhich)
            rSet.DisableItem( nWhich );
        else
        {
            SfxWhichIter aIter( rSet );
            nWhich = aIter.FirstWhich();
            while (nWhich)
            {
                rSet.DisableItem( nWhich );
                nWhich = aIter.NextWhich();
            }
        }
    }

    return bDisable;
}

/*-- 09.03.2004 13:15:03---------------------------------------------------
    Validate of drawing positions
  -----------------------------------------------------------------------*/
IMPL_LINK(SwDrawBaseShell, ValidatePosition, SvxSwFrameValidation*, pValidation )
{
    SwWrtShell *pSh = &GetShell();
    pValidation->nMinHeight = MINFLY;
    pValidation->nMinWidth =  MINFLY;

    SwRect aBoundRect;

    // OD 18.09.2003 #i18732# - adjustment for allowing vertical position
    //      aligned to page for fly frame anchored to paragraph or to character.
    const RndStdIds eAnchorType = static_cast<RndStdIds >(pValidation->nAnchorType);
    const SwPosition* pCntntPos = 0;
    SdrView*  pSdrView = pSh->GetDrawView();
    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
        SwFrmFmt* pFrmFmt = FindFrmFmt( pObj );
        pCntntPos = pFrmFmt->GetAnchor().GetCntntAnchor();
    }

    pSh->CalcBoundRect( aBoundRect, eAnchorType,
                           pValidation->nHRelOrient,
                           pValidation->nVRelOrient,
                           pCntntPos,
                           pValidation->bFollowTextFlow,
                           pValidation->bMirror, NULL, &pValidation->aPercentSize);

    BOOL bRTL;
    BOOL bIsInVertical = pSh->IsFrmVertical(TRUE, bRTL);
    if(bIsInVertical)
    {
        Point aPos(aBoundRect.Pos());
        long nTmp = aPos.X();
        aPos.X() = aPos.Y();
        aPos.Y() = nTmp;
        Size aSize(aBoundRect.SSize());
        nTmp = aSize.Width();
        aSize.Width() = aSize.Height();
        aSize.Height() = nTmp;
        aBoundRect.Chg( aPos, aSize );
        //exchange width/height to enable correct values
        nTmp = pValidation->nWidth;
        pValidation->nWidth = pValidation->nHeight;
        pValidation->nHeight = nTmp;
    }
    if ( eAnchorType == FLY_PAGE || eAnchorType == FLY_AT_FLY )
    {
        // MinimalPosition
        pValidation->nMinHPos = aBoundRect.Left();
        pValidation->nMinVPos = aBoundRect.Top();
        SwTwips nH = pValidation->nHPos;
        SwTwips nV = pValidation->nVPos;

        if (pValidation->nHPos + pValidation->nWidth > aBoundRect.Right())
        {
            if (pValidation->nHoriOrient == text::HoriOrientation::NONE)
            {
                pValidation->nHPos -= ((pValidation->nHPos + pValidation->nWidth) - aBoundRect.Right());
                nH = pValidation->nHPos;
            }
            else
                pValidation->nWidth = aBoundRect.Right() - pValidation->nHPos;
        }

        if (pValidation->nHPos + pValidation->nWidth > aBoundRect.Right())
            pValidation->nWidth = aBoundRect.Right() - pValidation->nHPos;

        if (pValidation->nVPos + pValidation->nHeight > aBoundRect.Bottom())
        {
            if (pValidation->nVertOrient == text::VertOrientation::NONE)
            {
                pValidation->nVPos -= ((pValidation->nVPos + pValidation->nHeight) - aBoundRect.Bottom());
                nV = pValidation->nVPos;
            }
            else
                pValidation->nHeight = aBoundRect.Bottom() - pValidation->nVPos;
        }

        if (pValidation->nVPos + pValidation->nHeight > aBoundRect.Bottom())
            pValidation->nHeight = aBoundRect.Bottom() - pValidation->nVPos;

        if ( pValidation->nVertOrient != text::VertOrientation::NONE )
            nV = aBoundRect.Top();

        if ( pValidation->nHoriOrient != text::HoriOrientation::NONE )
            nH = aBoundRect.Left();

        pValidation->nMaxHPos   = aBoundRect.Right()  - pValidation->nWidth;
        pValidation->nMaxHeight = aBoundRect.Bottom() - nV;

        pValidation->nMaxVPos   = aBoundRect.Bottom() - pValidation->nHeight;
        pValidation->nMaxWidth  = aBoundRect.Right()  - nH;
    }
    else if ( eAnchorType == FLY_AT_CNTNT || eAnchorType == FLY_AUTO_CNTNT )
    {
        if (pValidation->nHPos + pValidation->nWidth > aBoundRect.Right())
        {
            if (pValidation->nHoriOrient == text::HoriOrientation::NONE)
            {
                pValidation->nHPos -= ((pValidation->nHPos + pValidation->nWidth) - aBoundRect.Right());
            }
            else
                pValidation->nWidth = aBoundRect.Right() - pValidation->nHPos;
        }

        // OD 29.09.2003 #i17567#, #i18732# - consider following the text flow
        // and alignment at page areas.
        const bool bMaxVPosAtBottom = !pValidation->bFollowTextFlow ||
                                      pValidation->nVRelOrient == text::RelOrientation::PAGE_FRAME ||
                                      pValidation->nVRelOrient == text::RelOrientation::PAGE_PRINT_AREA;
        {
            SwTwips nTmpMaxVPos = ( bMaxVPosAtBottom
                                    ? aBoundRect.Bottom()
                                    : aBoundRect.Height() ) -
                                  pValidation->nHeight;
            if ( pValidation->nVPos > nTmpMaxVPos )
            {
                if (pValidation->nVertOrient == text::VertOrientation::NONE)
                {
                    pValidation->nVPos = nTmpMaxVPos;
                }
                else
                {
                    pValidation->nHeight = ( bMaxVPosAtBottom
                                     ? aBoundRect.Bottom()
                                     : aBoundRect.Height() ) - pValidation->nVPos;
                }
            }
        }

        pValidation->nMinHPos  = aBoundRect.Left();
        pValidation->nMaxHPos  = aBoundRect.Right() - pValidation->nWidth;

        pValidation->nMinVPos  = aBoundRect.Top();
        // OD 26.09.2003 #i17567#, #i18732# - determine maximum vertical position
        if ( bMaxVPosAtBottom )
        {
            pValidation->nMaxVPos  = aBoundRect.Bottom() - pValidation->nHeight;
        }
        else
        {
            pValidation->nMaxVPos  = aBoundRect.Height() - pValidation->nHeight;
        }

        // Maximale Breite Hoehe
        const SwTwips nH = ( pValidation->nHoriOrient != text::HoriOrientation::NONE )
                           ? aBoundRect.Left()
                           : pValidation->nHPos;
        const SwTwips nV = ( pValidation->nVertOrient != text::VertOrientation::NONE )
                           ? aBoundRect.Top()
                           : pValidation->nVPos;
        pValidation->nMaxHeight  = pValidation->nMaxVPos + pValidation->nHeight - nV;
        pValidation->nMaxWidth   = pValidation->nMaxHPos + pValidation->nWidth - nH;
    }
    else if ( eAnchorType == FLY_IN_CNTNT )
    {
        pValidation->nMinHPos = 0;
        pValidation->nMaxHPos = 0;

        pValidation->nMaxHeight = aBoundRect.Height();
        pValidation->nMaxWidth  = aBoundRect.Width();

        pValidation->nMaxVPos   = aBoundRect.Height();
        pValidation->nMinVPos   = -aBoundRect.Height() + pValidation->nHeight;
        if (pValidation->nMaxVPos < pValidation->nMinVPos)
        {
            pValidation->nMinVPos = pValidation->nMaxVPos;
            pValidation->nMaxVPos = -aBoundRect.Height();
        }
    }
    if(bIsInVertical)
    {
        //restore width/height exchange
        long nTmp = pValidation->nWidth;
        pValidation->nWidth = pValidation->nHeight;
        pValidation->nHeight = nTmp;
    }

    if (pValidation->nMaxWidth < pValidation->nWidth)
        pValidation->nWidth = pValidation->nMaxWidth;
    if (pValidation->nMaxHeight < pValidation->nHeight)
        pValidation->nHeight = pValidation->nMaxHeight;
    return 0;
}




