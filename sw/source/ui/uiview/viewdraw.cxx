/*************************************************************************
 *
 *  $RCSfile: viewdraw.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-24 14:54:43 $
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

#include "hintids.hxx"

#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif
#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif
#ifndef _MyEDITVIEW_HXX
#include <svx/editview.hxx>
#endif
#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _OUTLINER_HXX //autogen
#include <svx/outliner.hxx>
#endif
#ifndef _SVX_FMVIEW_HXX
#include <svx/fmview.hxx>
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#include <svx/dataaccessdescriptor.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif

#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif

#ifndef _LINGUISTIC_LNGPROPS_HHX_
#include <linguistic/lngprops.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif


#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif
#ifndef _SVDOTEXT_HXX //autogen
#include <svx/svdotext.hxx>
#endif
#ifndef _SVDOGRP_HXX
#include <svx/svdogrp.hxx>
#endif
#ifndef _SVDETC_HXX
#include <svx/svdetc.hxx>
#endif
#ifndef _EDITSTAT_HXX //autogen
#include <svx/editstat.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _SVX_FMGLOB_HXX //autogen
#include <svx/fmglob.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDOUTL_HXX
#include <svx/svdoutl.hxx>
#endif

#include "view.hxx"
#include "wrtsh.hxx"
#include "viewopt.hxx"
#include "cmdid.h"
#include "drawsh.hxx"
#include "drwbassh.hxx"
#include "beziersh.hxx"
#include "conrect.hxx"
#include "conctrl.hxx"
#include "conpoly.hxx"
#include "conarc.hxx"
#include "conform.hxx"
#include "dselect.hxx"
#include "edtwin.hxx"

using namespace ::com::sun::star;
/*--------------------------------------------------------------------
    Beschreibung: 	Drawing-Ids ausfuehren
 --------------------------------------------------------------------*/



void SwView::ExecDraw(SfxRequest& rReq)
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem;
    const SfxAllEnumItem* pEItem = 0;
    SdrView *pSdrView = pWrtShell->GetDrawView();
    sal_Bool bDeselect = sal_False;

    sal_uInt16 nSlotId = rReq.GetSlot();
    if(pArgs && SFX_ITEM_SET == pArgs->GetItemState(GetPool().GetWhich(nSlotId), sal_False, &pItem))
    {
        pEItem = (const SfxAllEnumItem*)pItem;
    }

    if( SID_INSERT_DRAW == nSlotId && pEItem)
        switch( pEItem->GetValue() )
        {
        case SVX_SNAP_DRAW_SELECT:			nSlotId = SID_OBJECT_SELECT; 		break;
        case SVX_SNAP_DRAW_LINE:			nSlotId = SID_DRAW_LINE; 			break;
        case SVX_SNAP_DRAW_RECT:			nSlotId = SID_DRAW_RECT; 			break;
        case SVX_SNAP_DRAW_ELLIPSE: 		nSlotId = SID_DRAW_ELLIPSE;			break;
        case SVX_SNAP_DRAW_POLYGON_NOFILL:	nSlotId = SID_DRAW_POLYGON_NOFILL;	break;
        case SVX_SNAP_DRAW_BEZIER_NOFILL:	nSlotId = SID_DRAW_BEZIER_NOFILL;	break;
        case SVX_SNAP_DRAW_FREELINE_NOFILL:	nSlotId = SID_DRAW_FREELINE_NOFILL;	break;
        case SVX_SNAP_DRAW_ARC: 			nSlotId = SID_DRAW_ARC;				break;
        case SVX_SNAP_DRAW_PIE: 			nSlotId = SID_DRAW_PIE;				break;
        case SVX_SNAP_DRAW_CIRCLECUT:		nSlotId = SID_DRAW_CIRCLECUT;		break;
        case SVX_SNAP_DRAW_TEXT:			nSlotId = SID_DRAW_TEXT; 			break;
        case SVX_SNAP_DRAW_TEXT_VERTICAL:	nSlotId = SID_DRAW_TEXT_VERTICAL;	break;
        case SVX_SNAP_DRAW_TEXT_MARQUEE:	nSlotId = SID_DRAW_TEXT_MARQUEE;		break;
        case SVX_SNAP_DRAW_CAPTION: 		nSlotId = SID_DRAW_CAPTION;			break;
        case SVX_SNAP_DRAW_CAPTION_VERTICAL: nSlotId = SID_DRAW_CAPTION_VERTICAL; break;
        }

    if (nSlotId == SID_OBJECT_SELECT && nFormSfxId == nSlotId)
    {
        bDeselect = sal_True;
    }
    else if (nSlotId == SID_FM_CREATE_CONTROL)
    {
        SFX_REQUEST_ARG( rReq, pIdentifierItem, SfxUInt16Item, SID_FM_CONTROL_IDENTIFIER, sal_False );
        if( pIdentifierItem )
        {
            sal_uInt16 nNewId = pIdentifierItem->GetValue();
            if (nNewId == nFormSfxId)
            {
                bDeselect = sal_True;
                GetViewFrame()->GetDispatcher()->Execute(SID_FM_LEAVE_CREATE);	// Button soll rauspoppen
            }
        }
    }
    else if( nSlotId == SID_FM_CREATE_FIELDCONTROL)
    {
        FmFormView* pFormView = PTR_CAST( FmFormView, pSdrView );
        if ( pFormView )
        {
            SFX_REQUEST_ARG( rReq, pDescriptorItem, SfxUnoAnyItem, SID_FM_DATACCESS_DESCRIPTOR, sal_False );
            DBG_ASSERT( pDescriptorItem, "SwView::ExecDraw(SID_FM_CREATE_FIELDCONTROL): invalid request args!" );
            if( pDescriptorItem )
            {
                ::svx::ODataAccessDescriptor aDescriptor( pDescriptorItem->GetValue() );
                SdrObject* pObj = pFormView->CreateFieldControl( aDescriptor );

                if ( pObj )
                {
                    Size aDocSz(pWrtShell->GetDocSize());
                    const SwRect& rVisArea = pWrtShell->VisArea();
                    Point aStartPos = rVisArea.Center();
                    if(rVisArea.Width() > aDocSz.Width())
                        aStartPos.X() = aDocSz.Width() / 2 + rVisArea.Left();
                    if(rVisArea.Height() > aDocSz.Height())
                        aStartPos.Y() = aDocSz.Height() / 2 + rVisArea.Top();

                    //determine the size of the object
                    if(pObj->IsGroupObject())
                    {
                        const Rectangle& rBoundRect = ((SdrObjGroup*)pObj)->GetBoundRect();
                        aStartPos.X() -= rBoundRect.GetWidth()/2;
                        aStartPos.Y() -= rBoundRect.GetHeight()/2;
                    }

                    // TODO: unmark all other
                    pWrtShell->EnterStdMode();
                    pWrtShell->SwFEShell::Insert( *pObj, 0, 0, &aStartPos );
                }
            }
        }
    }

    if (nSlotId == nDrawSfxId || bDeselect)
    {
        if (GetDrawFuncPtr())
        {
            GetDrawFuncPtr()->Deactivate();
            SetDrawFuncPtr(NULL);
        }

        if (pWrtShell->IsObjSelected() && !pWrtShell->IsSelFrmMode())
            pWrtShell->EnterSelFrmMode(NULL);
        LeaveDrawCreate();

        GetViewFrame()->GetBindings().Invalidate(SID_INSERT_DRAW);

        AttrChangedNotify(pWrtShell);
        return;
    }

    LeaveDrawCreate();

    if (pWrtShell->IsFrmSelected())
        pWrtShell->EnterStdMode();	// wegen Bug #45639

    SwDrawBase* pFuncPtr = NULL;

    switch (nSlotId)
    {
        case SID_OBJECT_SELECT:
        case SID_DRAW_SELECT:
            pFuncPtr = new DrawSelection(pWrtShell, pEditWin, this);
            nDrawSfxId = nFormSfxId = SID_OBJECT_SELECT;
            break;

        case SID_DRAW_LINE:
        case SID_DRAW_RECT:
        case SID_DRAW_ELLIPSE:
        case SID_DRAW_TEXT:
        case SID_DRAW_TEXT_VERTICAL:
        case SID_DRAW_TEXT_MARQUEE:
        case SID_DRAW_CAPTION:
        case SID_DRAW_CAPTION_VERTICAL:
            pFuncPtr = new ConstRectangle(pWrtShell, pEditWin, this);
            nDrawSfxId = nSlotId;
            break;

        case SID_DRAW_POLYGON_NOFILL:
        case SID_DRAW_BEZIER_NOFILL:
        case SID_DRAW_FREELINE_NOFILL:
            pFuncPtr = new ConstPolygon(pWrtShell, pEditWin, this);
            nDrawSfxId = nSlotId;
            break;

        case SID_DRAW_ARC:
        case SID_DRAW_PIE:
        case SID_DRAW_CIRCLECUT:
            pFuncPtr = new ConstArc(pWrtShell, pEditWin, this);
            nDrawSfxId = nSlotId;
            break;

        case SID_FM_CREATE_CONTROL:
        {
            SFX_REQUEST_ARG( rReq, pIdentifierItem, SfxUInt16Item, SID_FM_CONTROL_IDENTIFIER, sal_False );
            if( pIdentifierItem )
                nSlotId = pIdentifierItem->GetValue();
            pFuncPtr = new ConstFormControl(pWrtShell, pEditWin, this);
            nFormSfxId = nSlotId;
        }
        break;

        default:
            break;
    }

    static sal_uInt16 __READONLY_DATA aInval[] =
    {
        // Slot-Ids muessen beim Aufruf von Invalidate sortiert sein!
        SID_ATTRIBUTES_AREA,
        SID_INSERT_DRAW,
        0
    };
    GetViewFrame()->GetBindings().Invalidate(aInval);

    BOOL bEndTextEdit = TRUE;
    if (pFuncPtr)
    {
        if (GetDrawFuncPtr())
        {
            GetDrawFuncPtr()->Deactivate();
            SetDrawFuncPtr(NULL);
        }

        SetDrawFuncPtr(pFuncPtr);
        AttrChangedNotify(pWrtShell);

        pFuncPtr->Activate(nSlotId);
        NoRotate();
        if(rReq.GetModifier() == KEY_MOD1)
        {
            if(SID_OBJECT_SELECT == nDrawSfxId )
            {
                pWrtShell->GotoObj(TRUE);
            }
            else
            {
                pFuncPtr->CreateDefaultObject();
                pFuncPtr->Deactivate();
                SetDrawFuncPtr(NULL);
                LeaveDrawCreate();
                pWrtShell->EnterStdMode();
                SdrView *pSdrView = pWrtShell->GetDrawView();
                const SdrMarkList& rMarkList = pSdrView->GetMarkList();
                sal_uInt32 nCount = rMarkList.GetMarkCount();
                if(rMarkList.GetMarkCount() == 1 &&
                        (SID_DRAW_TEXT == nSlotId || SID_DRAW_TEXT_VERTICAL == nSlotId ||
                            SID_DRAW_TEXT_MARQUEE == nSlotId ))
                {
                    SdrObject* pObj = rMarkList.GetMark(0)->GetObj();
                    BeginTextEdit(pObj);
                    bEndTextEdit = FALSE;
                }
            }
        }
    }
    else
    {
        if (pWrtShell->IsObjSelected() && !pWrtShell->IsSelFrmMode())
            pWrtShell->EnterSelFrmMode(NULL);
    }

    if(bEndTextEdit && pSdrView && pSdrView->IsTextEdit())
        pSdrView->EndTextEdit( sal_True );

    AttrChangedNotify(pWrtShell);
}

/*--------------------------------------------------------------------
    Beschreibung: 	Drawing beenden
 --------------------------------------------------------------------*/



void SwView::ExitDraw()
{
    NoRotate();

    if (pShell && !pShell->ISA(SwDrawBaseShell) && !pShell->ISA(SwBezierShell))
    {
        SdrView *pSdrView = pWrtShell->GetDrawView();

        if (pSdrView && pSdrView->IsGroupEntered())
        {
            pSdrView->LeaveOneGroup();
            pSdrView->UnmarkAll();
            GetViewFrame()->GetBindings().Invalidate(SID_ENTER_GROUP);
        }

        if (GetDrawFuncPtr())
        {
            if (pWrtShell->IsSelFrmMode())
                pWrtShell->LeaveSelFrmMode();
            GetDrawFuncPtr()->Deactivate();

            SetDrawFuncPtr(NULL);
            LeaveDrawCreate();

            GetViewFrame()->GetBindings().Invalidate(SID_INSERT_DRAW);
        }
        GetEditWin().SetPointer(Pointer(POINTER_TEXT));
    }
}

/*--------------------------------------------------------------------
    Beschreibung: 	Rotate-Mode abschalten
 --------------------------------------------------------------------*/



void SwView::NoRotate()
{
    if (IsDrawRotate())
    {
        pWrtShell->SetDragMode(SDRDRAG_MOVE);
        FlipDrawRotate();

        const SfxBoolItem aTmp( SID_OBJECT_ROTATE, sal_False );
        GetViewFrame()->GetBindings().SetState( aTmp );
    }
}

/******************************************************************************
 *	Beschreibung: DrawTextEditMode einschalten
 ******************************************************************************/

sal_Bool SwView::EnterDrawTextMode(const Point& aDocPos)
{
    SdrObject* pObj;
    SdrPageView* pPV;
    SwWrtShell *pSh = &GetWrtShell();
    SdrView *pSdrView = pSh->GetDrawView();
    ASSERT( pSdrView, "EnterDrawTextMode without DrawView?" );

    sal_Bool bReturn = sal_False;

    sal_uInt16 nOld = pSdrView->GetHitTolerancePixel();
    pSdrView->SetHitTolerancePixel( 2 );

    if( pSdrView->IsMarkedHit( aDocPos ) &&
        !pSdrView->HitHandle( aDocPos, *pSh->GetOut() ) && IsTextTool() &&
        pSdrView->PickObj( aDocPos, pObj, pPV, SDRSEARCH_PICKTEXTEDIT ) &&
        pObj->ISA( SdrTextObj ) &&
        !pWrtShell->IsSelObjProtected(FLYPROTECT_CONTENT))
        bReturn = BeginTextEdit(pObj, pPV, pEditWin, FALSE );

    pSdrView->SetHitTolerancePixel( nOld );

    return bReturn;
}

/******************************************************************************
 *	Beschreibung: DrawTextEditMode einschalten
 ******************************************************************************/



sal_Bool SwView::BeginTextEdit( SdrObject* pObj, SdrPageView* pPV,
                                Window* pWin, sal_Bool bIsNewObj )
{
    SwWrtShell *pSh = &GetWrtShell();
    SdrView *pSdrView = pSh->GetDrawView();
    SdrOutliner* pOutliner = ::SdrMakeOutliner(OUTLINERMODE_TEXTOBJECT, pSdrView->GetModel());
    uno::Reference< linguistic2::XSpellChecker1 >  xSpell( ::GetSpellChecker() );
    if (pOutliner)
    {
        pOutliner->SetRefDevice(pSh->GetPrt());
        pOutliner->SetSpeller(xSpell);
        uno::Reference<linguistic2::XHyphenator> xHyphenator( ::GetHyphenator() );
        pOutliner->SetHyphenator( xHyphenator );
        pSh->SetCalcFieldValueHdl(pOutliner);

        sal_uInt32 nCntrl = pOutliner->GetControlWord();
        nCntrl |= EE_CNTRL_ALLOWBIGOBJS;
        nCntrl |= EE_CNTRL_URLSFXEXECUTE;

        const SwViewOption *pOpt = pSh->GetViewOptions();

        if (SwViewOption::IsFieldShadings())
            nCntrl |= EE_CNTRL_MARKFIELDS;
        else
            nCntrl &= ~EE_CNTRL_MARKFIELDS;

        if (pOpt->IsHideSpell())
            nCntrl |= EE_CNTRL_NOREDLINES;
        else
            nCntrl &= ~EE_CNTRL_NOREDLINES;

        if (pOpt->IsOnlineSpell())
            nCntrl |= EE_CNTRL_ONLINESPELLING;
        else
            nCntrl &= ~EE_CNTRL_ONLINESPELLING;

        pOutliner->SetControlWord(nCntrl);
        const SfxPoolItem& rItem = pSh->GetDoc()->GetDefault(RES_CHRATR_LANGUAGE);
        pOutliner->SetDefaultLanguage(((const SvxLanguageItem&)rItem).GetLanguage());

        if( bIsNewObj )
            pOutliner->SetVertical( SID_DRAW_TEXT_VERTICAL == nDrawSfxId ||
                                    SID_DRAW_CAPTION_VERTICAL == nDrawSfxId );
        // #i7672#
        // No longer necessary, see text below
        // Color aBackground(pSh->GetShapeBackgrd());
        // pOutliner->SetBackgroundColor(aBackground);

        // OD 09.12.2002 #103045# - set default horizontal text direction at outliner
        EEHorizontalTextDirection aDefHoriTextDir =
            pSh->IsShapeDefaultHoriTextDirR2L() ? EE_HTEXTDIR_R2L : EE_HTEXTDIR_L2R;
        pOutliner->SetDefaultHorizontalTextDirection( aDefHoriTextDir );
    }
    sal_Bool bRet =	pSdrView->BegTextEdit( pObj, pPV, pWin, TRUE, pOutliner );

    // #i7672#
    // Since BegTextEdit actually creates the OutlinerView and thus also
    // sets the background color, an own background color needs to be set
    // after TextEditing was started. This is now done here.
    if(bRet)
    {
        OutlinerView* pView = pSdrView->GetTextEditOutlinerView();

        if(pView)
        {
            Color aBackground(pSh->GetShapeBackgrd());
            pView->SetBackgroundColor(aBackground);
        }
    }

    return bRet;
}

/******************************************************************************
 *	Beschreibung: Ist ein DrawTextObjekt selektiert?
 ******************************************************************************/



sal_Bool SwView::IsTextTool() const
{
    sal_uInt16 nId;
    sal_uInt32 nInvent;
    SdrView *pSdrView = GetWrtShell().GetDrawView();
    ASSERT( pSdrView, "IsTextTool without DrawView?" );

    if (pSdrView->IsCreateMode())
        pSdrView->SetCreateMode(sal_False);

    pSdrView->TakeCurrentObj(nId,nInvent);
    return (nInvent==SdrInventor);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



SdrView* SwView::GetDrawView() const
{
    return GetWrtShell().GetDrawView();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



sal_Bool SwView::IsBezierEditMode()
{
    return (!IsDrawSelMode() && GetWrtShell().GetDrawView()->HasMarkablePoints());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwView::IsFormMode() const
{
    if (GetDrawFuncPtr() && GetDrawFuncPtr()->IsCreateObj())
    {
        if (GetDrawFuncPtr()->IsInsertForm())
            return sal_True;
        else
            return sal_False;
    }

    return AreOnlyFormsSelected();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



void SwView::SetDrawFuncPtr(SwDrawBase* pFuncPtr)
{
    if (pDrawActual)
        delete pDrawActual;
    pDrawActual = pFuncPtr;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwView::SetSelDrawSlot()
{
    nDrawSfxId = SID_OBJECT_SELECT;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwView::AreOnlyFormsSelected() const
{
    if ( GetWrtShell().IsFrmSelected() )
        return sal_False;

    sal_Bool bForm = sal_True;

    SdrView* pSdrView = GetWrtShell().GetDrawView();

    const SdrMarkList& rMarkList = pSdrView->GetMarkList();
    sal_uInt32 nCount = rMarkList.GetMarkCount();

    if (nCount)
    {
        for (sal_uInt32 i = 0; i < nCount; i++)
        {
            // Sind ausser Controls noch normale Draw-Objekte selektiert?
            SdrObject *pSdrObj = rMarkList.GetMark(i)->GetObj();

            if (!HasOnlyObj(pSdrObj, FmFormInventor))
            {
                bForm = sal_False;
                break;
            }
        }
    }
    else
        bForm = sal_False;

    return bForm;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwView::HasDrwObj(SdrObject *pSdrObj) const
{
    sal_Bool bRet = sal_False;

    if (pSdrObj->IsGroupObject())
    {
        SdrObjList*	pList = pSdrObj->GetSubList();
        sal_uInt32 nCnt = pList->GetObjCount();

        for (sal_uInt32 i = 0; i < nCnt; i++)
            if ((bRet = HasDrwObj(pList->GetObj(i))) == sal_True)
                break;
    }
    else if (SdrInventor == pSdrObj->GetObjInventor() || pSdrObj->Is3DObj())
        return sal_True;

    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwView::HasOnlyObj(SdrObject *pSdrObj, sal_uInt32 eObjInventor) const
{
    sal_Bool bRet = sal_False;

    if (pSdrObj->IsGroupObject())
    {
        SdrObjList*	pList = pSdrObj->GetSubList();
        sal_uInt32 nCnt = pList->GetObjCount();

        for (sal_uInt32 i = 0; i < nCnt; i++)
            if ((bRet = HasOnlyObj(pList->GetObj(i), eObjInventor)) == sal_False)
                break;
    }
    else if (eObjInventor == pSdrObj->GetObjInventor())
        return sal_True;

    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



sal_Bool SwView::ExecDrwTxtSpellPopup(const Point& rPt)
{
    sal_Bool bRet = sal_False;
    SdrView *pSdrView = pWrtShell->GetDrawView();
    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    Point aPos( GetEditWin().LogicToPixel( rPt ) );

    if (pOLV->IsWrongSpelledWordAtPos( aPos ))
    {
        bRet = sal_True;
        pOLV->ExecuteSpellPopup( aPos );
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



sal_Bool SwView::IsDrawTextHyphenate()
{
    SdrView *pSdrView = pWrtShell->GetDrawView();
    sal_Bool bHyphenate = sal_False;

    SfxItemSet aNewAttr( pSdrView->GetModel()->GetItemPool(),
                            EE_PARA_HYPHENATE, EE_PARA_HYPHENATE );
    if( pSdrView->GetAttributes( aNewAttr ) &&
        aNewAttr.GetItemState( EE_PARA_HYPHENATE ) >= SFX_ITEM_AVAILABLE )
        bHyphenate = ((const SfxBoolItem&)aNewAttr.Get( EE_PARA_HYPHENATE )).
                        GetValue();

    return bHyphenate;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



void SwView::HyphenateDrawText()
{
    SdrView *pSdrView = pWrtShell->GetDrawView();
    sal_Bool bHyphenate = IsDrawTextHyphenate();

    SfxItemSet aSet( GetPool(), EE_PARA_HYPHENATE, EE_PARA_HYPHENATE );
    aSet.Put( SfxBoolItem( EE_PARA_HYPHENATE, !bHyphenate ) );
    pSdrView->SetAttributes( aSet );
    GetViewFrame()->GetBindings().Invalidate(FN_HYPHENATE_OPT_DLG);
}



