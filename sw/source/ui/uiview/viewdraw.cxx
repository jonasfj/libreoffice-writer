/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: viewdraw.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:37:51 $
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
#ifndef _SVX_FONTWORK_BAR_HXX
#include <svx/fontworkbar.hxx>
#endif
#ifndef _SVX_FONTWORK_GALLERY_DIALOG_HXX
#include <svx/fontworkgallery.hxx>
#endif
#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
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
#include "conpoly.hxx"
#include "conarc.hxx"
#include "conform.hxx"
#include "concustomshape.hxx"
#include "dselect.hxx"
#include "edtwin.hxx"

// #108784#
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif

// #108784#
#ifndef _SVDPAGV_HXX
#include <svx/svdpagv.hxx>
#endif

#ifndef _SVX_EXTRUSION_BAR_HXX
#include <svx/extrusionbar.hxx>
#endif

#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

using namespace ::com::sun::star;
/*--------------------------------------------------------------------
    Beschreibung: 	Drawing-Ids ausfuehren
 --------------------------------------------------------------------*/

void SwView::ExecDraw(SfxRequest& rReq)
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem;
    const SfxAllEnumItem* pEItem = 0;
    const SfxStringItem* pStringItem = 0;
    SdrView *pSdrView = pWrtShell->GetDrawView();
    sal_Bool bDeselect = sal_False;

    sal_uInt16 nSlotId = rReq.GetSlot();
    if(pArgs && SFX_ITEM_SET == pArgs->GetItemState(GetPool().GetWhich(nSlotId), sal_False, &pItem))
    {
        pEItem = dynamic_cast< const SfxAllEnumItem*>(pItem);
        pStringItem = dynamic_cast< const SfxStringItem*>(pItem);
    }

    if (SID_INSERT_DRAW == nSlotId && pEItem)
        switch ( pEItem->GetValue() )
        {
            case SVX_SNAP_DRAW_SELECT:              nSlotId = SID_OBJECT_SELECT;            break;
            case SVX_SNAP_DRAW_LINE:                nSlotId = SID_DRAW_LINE;                break;
            case SVX_SNAP_DRAW_RECT:                nSlotId = SID_DRAW_RECT;                break;
            case SVX_SNAP_DRAW_ELLIPSE:             nSlotId = SID_DRAW_ELLIPSE;             break;
            case SVX_SNAP_DRAW_POLYGON_NOFILL:      nSlotId = SID_DRAW_POLYGON_NOFILL;      break;
            case SVX_SNAP_DRAW_BEZIER_NOFILL:       nSlotId = SID_DRAW_BEZIER_NOFILL;       break;
            case SVX_SNAP_DRAW_FREELINE_NOFILL:     nSlotId = SID_DRAW_FREELINE_NOFILL;     break;
            case SVX_SNAP_DRAW_ARC:                 nSlotId = SID_DRAW_ARC;                 break;
            case SVX_SNAP_DRAW_PIE:                 nSlotId = SID_DRAW_PIE;                 break;
            case SVX_SNAP_DRAW_CIRCLECUT:           nSlotId = SID_DRAW_CIRCLECUT;           break;
            case SVX_SNAP_DRAW_TEXT:                nSlotId = SID_DRAW_TEXT;                break;
            case SVX_SNAP_DRAW_TEXT_VERTICAL:       nSlotId = SID_DRAW_TEXT_VERTICAL;       break;
            case SVX_SNAP_DRAW_TEXT_MARQUEE:        nSlotId = SID_DRAW_TEXT_MARQUEE;        break;
            case SVX_SNAP_DRAW_CAPTION:             nSlotId = SID_DRAW_CAPTION;             break;
            case SVX_SNAP_DRAW_CAPTION_VERTICAL:    nSlotId = SID_DRAW_CAPTION_VERTICAL;    break;
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
                    Size aDocSize(pWrtShell->GetDocSize());
                    const SwRect& rVisArea = pWrtShell->VisArea();
                    Point aStartPos = rVisArea.Center();
                    if(rVisArea.Width() > aDocSize.Width())
                        aStartPos.X() = aDocSize.Width() / 2 + rVisArea.Left();
                    if(rVisArea.Height() > aDocSize.Height())
                        aStartPos.Y() = aDocSize.Height() / 2 + rVisArea.Top();

                    //determine the size of the object
                    if(pObj->IsGroupObject())
                    {
                        const Rectangle& rBoundRect = ((SdrObjGroup*)pObj)->GetCurrentBoundRect();
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
    else if ( nSlotId == SID_FONTWORK_GALLERY_FLOATER )
    {
        Window*	 pWin = &( pWrtShell->GetView().GetViewFrame()->GetWindow() );

        if ( pWin )
            pWin->EnterWait();

        if( !pWrtShell->HasDrawView() )
            pWrtShell->MakeDrawView();

        pSdrView = pWrtShell->GetDrawView();
        if ( pSdrView )
        {
            SdrObject* pObj = NULL;
            svx::FontWorkGalleryDialog aDlg( pSdrView, pWin, nSlotId );
            aDlg.SetSdrObjectRef( &pObj, pSdrView->GetModel() );
            aDlg.Execute();
            if ( pObj )
            {
                Size 			aDocSize( pWrtShell->GetDocSize() );
                const SwRect& 	rVisArea = pWrtShell->VisArea();
                Point 			aPos( rVisArea.Center() );
                Size			aSize;
                Size			aPrefSize( pObj->GetSnapRect().GetSize() );

                if( rVisArea.Width() > aDocSize.Width())
                    aPos.X() = aDocSize.Width() / 2 + rVisArea.Left();

                if(rVisArea.Height() > aDocSize.Height())
                    aPos.Y() = aDocSize.Height() / 2 + rVisArea.Top();

                if( aPrefSize.Width() && aPrefSize.Height() )
                {
                    if( pWin )
                        aSize = pWin->PixelToLogic( aPrefSize, MAP_TWIP );
                    else
                        aSize = Application::GetDefaultDevice()->PixelToLogic( aPrefSize, MAP_TWIP );
                }
                else
                    aSize = Size( 2835, 2835 );

                pWrtShell->EnterStdMode();
                pWrtShell->SwFEShell::Insert( *pObj, 0, 0, &aPos );
                rReq.Ignore ();
            }
        }
        if( pWin )
            pWin->LeaveWait();
    }

    if( nSlotId == SID_DRAW_CS_ID )
    {
        //deselect if same custom shape is selected again
        SwDrawBase* pFuncPtr = GetDrawFuncPtr();
        if( pFuncPtr && pFuncPtr->GetSlotId() == SID_DRAW_CS_ID )
        {
            ConstCustomShape* pConstCustomShape = (ConstCustomShape*)(pFuncPtr);
            rtl::OUString aNew = ConstCustomShape::GetShapeTypeFromRequest( rReq );
            rtl::OUString aOld = pConstCustomShape->GetShapeType();
            if( aNew == aOld )
            {
                bDeselect = true;
            }
        }
    }

    //deselect if same shape is selected again (but different custom shapes do have same slot id)
    if ( bDeselect || (nSlotId == nDrawSfxId && 
            (!pStringItem || (pStringItem->GetValue() == sDrawCustom)) 
                && (nSlotId != SID_DRAW_CS_ID) ) )
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
            sDrawCustom.Erase();
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
            sDrawCustom.Erase();
            break;

        case SID_DRAW_POLYGON_NOFILL:
        case SID_DRAW_BEZIER_NOFILL:
        case SID_DRAW_FREELINE_NOFILL:
            pFuncPtr = new ConstPolygon(pWrtShell, pEditWin, this);
            nDrawSfxId = nSlotId;
            sDrawCustom.Erase();
            break;

        case SID_DRAW_ARC:
        case SID_DRAW_PIE:
        case SID_DRAW_CIRCLECUT:
            pFuncPtr = new ConstArc(pWrtShell, pEditWin, this);
            nDrawSfxId = nSlotId;
            sDrawCustom.Erase();
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

        case SID_DRAWTBX_CS_BASIC :
        case SID_DRAWTBX_CS_SYMBOL :
        case SID_DRAWTBX_CS_ARROW :
        case SID_DRAWTBX_CS_FLOWCHART :
        case SID_DRAWTBX_CS_CALLOUT :
        case SID_DRAWTBX_CS_STAR :
        case SID_DRAW_CS_ID :
        {
            pFuncPtr = new ConstCustomShape(pWrtShell, pEditWin, this, rReq );
            nDrawSfxId = nSlotId;
            if ( nSlotId != SID_DRAW_CS_ID )
            {
                if ( pStringItem )
                {
                    sDrawCustom = pStringItem->GetValue();
                    aCurrShapeEnumCommand[ nSlotId - SID_DRAWTBX_CS_BASIC ] = sDrawCustom;
                    SfxBindings& rBind = GetViewFrame()->GetBindings();
                    rBind.Invalidate( nSlotId );
                    rBind.Update( nSlotId );
                }
            }
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
                SdrView *pTmpSdrView = pWrtShell->GetDrawView();
                const SdrMarkList& rMarkList = pTmpSdrView->GetMarkedObjectList();
                if(rMarkList.GetMarkCount() == 1 &&
                        (SID_DRAW_TEXT == nSlotId || SID_DRAW_TEXT_VERTICAL == nSlotId ||
                            SID_DRAW_TEXT_MARQUEE == nSlotId ))
                {
                    SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
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
        pSdrView->SdrEndTextEdit( sal_True );

    AttrChangedNotify(pWrtShell);
}

/*--------------------------------------------------------------------
    Beschreibung: 	Drawing beenden
 --------------------------------------------------------------------*/



void SwView::ExitDraw()
{
    NoRotate();

    if(pShell)
    {   
        //#126062 # the shell may be invalid at close/reload/SwitchToViewShell
        SfxDispatcher* pDispatch = GetViewFrame()->GetDispatcher();
        USHORT nIdx = 0;
        SfxShell* pTest = 0;
        do
        {
            pTest = pDispatch->GetShell(nIdx++);
        }
        while( pTest && pTest != this && pTest != pShell);
        if(pTest == pShell && 
            // don't call LeaveSelFrmMode() etc. for the below,
            // because objects may still be selected:
            !pShell->ISA(SwDrawBaseShell) &&
            !pShell->ISA(SwBezierShell) &&
            !pShell->ISA(svx::ExtrusionBar) &&
            !pShell->ISA(svx::FontworkBar))
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
        !pSdrView->PickHandle( aDocPos ) && IsTextTool() &&
        pSdrView->PickObj( aDocPos, pObj, pPV, SDRSEARCH_PICKTEXTEDIT ) &&

        // #108784#
        // To allow SwDrawVirtObj text objects to be activated, allow their type, too.
        //pObj->ISA( SdrTextObj ) &&
        ( pObj->ISA( SdrTextObj ) ||
          ( pObj->ISA(SwDrawVirtObj) &&
            ((SwDrawVirtObj*)pObj)->GetReferencedObj().ISA(SdrTextObj) ) ) &&

        !pWrtShell->IsSelObjProtected(FLYPROTECT_CONTENT))
    {
        bReturn = BeginTextEdit( pObj, pPV, pEditWin, sal_False );
    }

    pSdrView->SetHitTolerancePixel( nOld );

    return bReturn;
}

/******************************************************************************
 *	Beschreibung: DrawTextEditMode einschalten
 ******************************************************************************/
sal_Bool SwView::BeginTextEdit(SdrObject* pObj, SdrPageView* pPV, Window* pWin, sal_Bool bIsNewObj)
{
    SwWrtShell *pSh = &GetWrtShell();
    SdrView *pSdrView = pSh->GetDrawView();
    SdrOutliner* pOutliner = ::SdrMakeOutliner(OUTLINERMODE_TEXTOBJECT, pSdrView->GetModel());
    uno::Reference< linguistic2::XSpellChecker1 >  xSpell( ::GetSpellChecker() );
    if (pOutliner)
    {
        pOutliner->SetRefDevice(pSh->getIDocumentDeviceAccess()->getReferenceDevice(false));
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

    // #108784#
    // To allow editing the referenced object from a SwDrawVirtObj here
    // the original needs to be fetched evenually. This ATM activates the
    // text edit mode for the original object.
    SdrObject* pToBeActivated = pObj;

    // #108784#
    // Always the original object is edited. To allow the TextEdit to happen
    // where the VirtObj is positioned, on demand a occurring offset is set at
    // the TextEdit object. That offset is used for creating and managing the
    // OutlinerView.
    Point aNewTextEditOffset(0, 0);

    if(pObj->ISA(SwDrawVirtObj))
    {
        SwDrawVirtObj* pVirtObj = (SwDrawVirtObj*)pObj;
        pToBeActivated = &((SdrObject&)pVirtObj->GetReferencedObj());
        aNewTextEditOffset = pVirtObj->GetOffset();
    }

    // set in each case, thus it will be correct for all objects
    ((SdrTextObj*)pToBeActivated)->SetTextEditOffset(aNewTextEditOffset);

    sal_Bool bRet(pSdrView->SdrBeginTextEdit( pToBeActivated, pPV, pWin, sal_True, pOutliner, 0, sal_False, sal_False, sal_False ));

    // #i7672#
    // Since SdrBeginTextEdit actually creates the OutlinerView and thus also
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
        return (GetDrawFuncPtr()->IsInsertForm());
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
    sDrawCustom.Erase();
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

    const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
    sal_uInt32 nCount = rMarkList.GetMarkCount();

    if (nCount)
    {
        for (sal_uInt32 i = 0; i < nCount; i++)
        {
            // Sind ausser Controls noch normale Draw-Objekte selektiert?
            SdrObject *pSdrObj = rMarkList.GetMark(i)->GetMarkedSdrObj();

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



