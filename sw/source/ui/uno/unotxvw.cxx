/*************************************************************************
 *
 *  $RCSfile: unotxvw.cxx,v $
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

#include "viscrs.hxx"
#include <sfx2/frame.hxx>
#include <cmdid.h>
#include <hintids.hxx>
#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _SWDOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _UNOTXVW_HXX
#include <unotxvw.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _SWVIEW_HXX
#include <view.hxx>
#endif
#ifndef _UNOMOD_HXX
#include <unomod.hxx>
#endif
#ifndef _UNOFRAME_HXX
#include <unoframe.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _WRTSH_HXX //autogen
#include <wrtsh.hxx>
#endif
#ifndef _UNOTBL_HXX
#include <unotbl.hxx>
#endif
#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif
#ifndef _SVDOUNO_HXX //autogen
#include <svx/svdouno.hxx>
#endif
#ifndef _SVDOGRP_HXX //autogen
#include <svx/svdogrp.hxx>
#endif
#ifndef _PAGEDESC_HXX //autogen
#include <pagedesc.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _BOOKMRK_HXX //autogen
#include <bookmrk.hxx>
#endif
#ifndef _UNOTXDOC_HXX //autogen
#include <unotxdoc.hxx>
#endif
#ifndef _UNODRAW_HXX //autogen
#include <unodraw.hxx>
#endif
#ifndef _SVX_UNOSHGRP_HXX //autogen
#include <svx/unoshcol.hxx>
#endif
#ifndef _SVX_UNOSHAPE_HXX //autogen wg. SvxShape
#include <svx/unoshape.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen wg. SdrPageView
#include <svx/svdpagv.hxx>
#endif
#include <swerror.h>
#ifndef _CACHESTR_HXX
#include <tools/cachestr.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::text;
using namespace rtl;

SV_IMPL_PTRARR( SelectionChangeListenerArr, XSelectionChangeListenerPtr );

/* -----------------22.05.98 12:20-------------------
 *
 * --------------------------------------------------*/
SwPaM* lcl_createPamCopy(const SwPaM& rPam)
{
    SwPaM* pRet = new SwPaM(*rPam.GetPoint());
    if(rPam.HasMark())
    {
        pRet->SetMark();
        *pRet->GetMark() = *rPam.GetMark();
    }
    if(rPam.GetNext() != (const Ring*)&rPam)
    {
        SwPaM *_pStartCrsr = (SwPaM *)rPam.GetNext();
        do
        {
            //neuen PaM erzeugen
            SwPaM* pPaM = new SwPaM(*_pStartCrsr);
            //und in den Ring einfuegen
            pPaM->MoveTo(pRet);

        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != rPam.GetNext() );
    }
    return pRet;
}
/* -----------------04.11.98 15:32-------------------
 *
 * --------------------------------------------------*/
sal_Bool lcl_FindObjInGroup(uno::Reference< awt::XControl > & xRet, SdrObjGroup* pGroup, const Reference< awt::XControlModel > & xModel, Window* pWin)
{
    SdrObjList* pList = pGroup->GetSubList();
    sal_uInt32 nCount = pList->GetObjCount();
    for( sal_uInt32 i=0; i< nCount; i++ )
    {
        SdrObject* pObj = pList->GetObj(i);
        SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, pObj );
        SdrObjGroup* pGroup;
        if( pFormObj )
        {
            Reference< awt::XControlModel >  xCM = pFormObj->GetUnoControlModel();
            if( xCM.is() && xModel == xCM )
            {
                xRet = pFormObj->GetUnoControl( pWin );
                break;
            }
        }
        else if(0 != (pGroup = PTR_CAST( SdrObjGroup, pObj )))
        {
            if(lcl_FindObjInGroup(xRet, pGroup, xModel, pWin))
                break;
        }
    }
    return xRet.is();
}

/******************************************************************
 * SwXTextView
 ******************************************************************/
/*-- 17.12.98 09:34:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextView::SwXTextView(SwView* pSwView) :
    SfxBaseController(pSwView),
    pView(pSwView),
    pxViewSettings(0),
    pxTextViewCursor(0)
{

}
/*-- 17.12.98 09:34:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextView::~SwXTextView()
{
    if(pxViewSettings)
    {
         beans::XPropertySet* pSettings = pxViewSettings->get();
        ((SwXViewSettings*)pSettings)->Invalidate();
        delete pxViewSettings;
    }
    if(pxTextViewCursor)
    {
     text::XTextViewCursor* pCrsr = pxTextViewCursor->get();
        ((SwXTextViewCursor*)pCrsr)->Invalidate();
        delete pxTextViewCursor;
    }
}
/* -----------------------------18.05.00 10:18--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< uno::Type > SAL_CALL SwXTextView::getTypes(  ) throw(::com::sun::star::uno::RuntimeException)
{
//	uno::Sequence< uno::Type > aViewTypes = SwXTextViewBaseClass::getTypes();
    uno::Sequence< uno::Type > aBaseTypes = SfxBaseController::getTypes();

    long nIndex = aBaseTypes.getLength();
    aBaseTypes.realloc(
        aBaseTypes.getLength() + 5 );

    uno::Type* pBaseTypes = aBaseTypes.getArray();
    pBaseTypes[nIndex++] = ::getCppuType((Reference<view::XSelectionSupplier	>*)0);
    pBaseTypes[nIndex++] = ::getCppuType((Reference<lang::XServiceInfo			>*)0);
    pBaseTypes[nIndex++] = ::getCppuType((Reference<view::XControlAccess		>*)0);
    pBaseTypes[nIndex++] = ::getCppuType((Reference<text::XTextViewCursorSupplier>*)0);
    pBaseTypes[nIndex++] = ::getCppuType((Reference<view::XViewSettingsSupplier	>*)0);
    return aBaseTypes;
}
/* -----------------------------18.05.00 10:18--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< sal_Int8 > SAL_CALL SwXTextView::getImplementationId(  ) throw(::com::sun::star::uno::RuntimeException)
{
    static uno::Sequence< sal_Int8 > aId( 16 );
    static BOOL bInit = FALSE;
    if(!bInit)
        rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
    return aId;
}
/* -----------------------------18.05.00 10:18--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXTextView::acquire(  )throw()
{
    SfxBaseController::acquire();
}
/* -----------------------------18.05.00 10:18--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXTextView::release(  )throw()
{
    SfxBaseController::release();
}
/* -----------------------------18.05.00 10:23--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SAL_CALL SwXTextView::queryInterface( const uno::Type& aType )
    throw (RuntimeException)
{
    uno::Any aRet;
    if(aType == ::getCppuType((Reference<view::XSelectionSupplier	>*)0))
    {
        Reference<view::XSelectionSupplier> xRet = this;
        aRet.setValue(&xRet, aType);
    }
    else if(aType == ::getCppuType((Reference<lang::XServiceInfo			>*)0))
    {
        Reference<lang::XServiceInfo> xRet = this;
        aRet.setValue(&xRet, aType);
    }
    else if(aType == ::getCppuType((Reference<view::XControlAccess		>*)0))
    {
        Reference<view::XControlAccess> xRet = this;
        aRet.setValue(&xRet, aType);
    }
    else if(aType == ::getCppuType((Reference<text::XTextViewCursorSupplier>*)0))
    {
        Reference<text::XTextViewCursorSupplier> xRet = this;
        aRet.setValue(&xRet, aType);
    }
    else if(aType == ::getCppuType((Reference<view::XViewSettingsSupplier	>*)0))
    {
        Reference<view::XViewSettingsSupplier> xRet = this;
        aRet.setValue(&xRet, aType);
    }
    else
        aRet = SfxBaseController::queryInterface(aType);
    return aRet;
}
/*-- 17.12.98 09:34:26---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextView::select(const uno::Any& aInterface) throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(GetView() && aInterface.getValueType().getTypeClass() == uno::TypeClass_INTERFACE)
    {
        Reference< uno::XInterface >  xInterface = *(Reference< uno::XInterface > *)aInterface.getValue();
        SwWrtShell& rSh = GetView()->GetWrtShell();
        SwDoc* pDoc = GetView()->GetDocShell()->GetDoc();
        Reference< lang::XUnoTunnel >  xIfcTunnel(xInterface, uno::UNO_QUERY);
        Reference< text::XTextCursor >  xCrsr(xInterface, uno::UNO_QUERY);
        Reference< container::XIndexAccess > 	xPosN(xInterface, uno::UNO_QUERY);
        Reference< text::XTextRange > 	xPos(xInterface, uno::UNO_QUERY);

        SwPaM * pPam = 0;
        SwXTextRanges* pPosN = 0;
        if(xCrsr.is())
        {
            //
            SwXTextCursor* pCursor =
                xIfcTunnel.is() ?
                    (SwXTextCursor*)xIfcTunnel->getSomething(SwXTextCursor::getUnoTunnelId())
                    : 0;

            if(pCursor && pCursor->GetDoc() == GetView()->GetDocShell()->GetDoc())
                pPam = lcl_createPamCopy(*((SwXTextCursor*)pCursor)->GetPaM());

        }
        else if(xPosN.is() &&
            xIfcTunnel.is() &&
                0 != (pPosN = (SwXTextRanges*)xIfcTunnel->getSomething(SwXTextRanges::getUnoTunnelId())))
        {
            const SwUnoCrsr* pUnoCrsr = pPosN->GetCursor();
            if(pUnoCrsr)
            {
                pPam = lcl_createPamCopy(*pUnoCrsr);
            }
        }
        else if(xPos.is())
        {
            SwUnoInternalPaM aPam(*pDoc);
            if(SwXTextRange::XTextRangeToSwPaM(aPam, xPos))
            {
                pPam = lcl_createPamCopy(aPam);
            }
        }
        if(pPam)
        {
            rSh.EnterStdMode();
            rSh.SetSelection(*pPam);
            delete pPam;
            return sal_True;
        }
        Reference< uno::XInterface >  xFrm(xInterface, uno::UNO_QUERY);;

        if(xFrm.is())
        {
            //
            SwXFrame* pFrame = (SwXFrame*)
                xIfcTunnel->getSomething(SwXFrame::getUnoTunnelId());

            SwFrmFmt* pFrmFmt = pFrame ? pFrame->GetFrmFmt() : 0;
            if(pFrmFmt && pFrmFmt->GetDoc() == pDoc)
            {
                sal_Bool bSuccess = rSh.GotoFly( pFrmFmt->GetName(), pFrame->GetFlyCntType());
                if (bSuccess)
                {
                    rSh.HideCrsr();
                    rSh.EnterSelFrmMode();
                }
            }
            return sal_True;
        }

        Reference< text::XTextTable >  xTbl(xInterface, uno::UNO_QUERY);;

        if(xTbl.is())
        {
            SwXTextTable* pTable = (SwXTextTable*)
                xIfcTunnel->getSomething(SwXTextTable::getUnoTunnelId());

            SwFrmFmt* pTblFrmFmt = pTable ? ((SwXTextTable*)pTable)->GetFrmFmt() : 0;
            if(pTblFrmFmt &&pTblFrmFmt->GetDoc() == pDoc)
            {
                rSh.EnterStdMode();
                rSh.GotoTable(pTblFrmFmt->GetName());
            }
            return sal_True;
        }

        Reference< text::XTextContent >  xBkm(xInterface, uno::UNO_QUERY);;

        if(xBkm.is())
        {
            SwXBookmark* pBkm = (SwXBookmark*)
                    xIfcTunnel->getSomething(SwXBookmark::getUnoTunnelId());
            if(pBkm && pBkm->GetDoc() == pDoc)
            {

                sal_uInt16 nFndPos = rSh.FindBookmark(pBkm->getName());

                if( USHRT_MAX != nFndPos )
                {
                    rSh.EnterStdMode();
                    rSh.GotoBookmark( nFndPos );
                }
                return sal_True;
            }
        }
        // IndexMark, sdbcx::Index, TextField, Draw, Section, text::Footnote, text::Paragraph
        //

        Reference< drawing::XShapes >  xShapeColl( xInterface, uno::UNO_QUERY );
        Reference< beans::XPropertySet >  xTmpProp(xInterface, uno::UNO_QUERY);
        SwXShape* pSwXShape = (SwXShape*)
            xIfcTunnel->getSomething(SwXShape::getUnoTunnelId());
        SvxShape* pSvxShape = 0;
        if(pSwXShape)
        {
            Reference< uno::XAggregation > 	xAgg = pSwXShape->GetAggregationInterface();
            if(xAgg.is())
            {
                pSvxShape = (SvxShape*)xIfcTunnel->getSomething(SvxShape::getUnoTunnelId());
            }
        }

        if ( pSvxShape || xShapeColl.is() )			// Drawing drawing::Layer
        {
            SdrView* pDrawView = rSh.GetDrawView();
            if (pDrawView)
            {
                pDrawView->EndTextEdit();
                pDrawView->UnmarkAll();

                if (pSvxShape)		// einzelnes Shape
                {
                    SdrObject *pObj = pSvxShape->GetSdrObject();
                    if (pObj)
                    {
//						lcl_ShowObject( *pViewSh, *pDrawView, pObj );
                        SdrPageView* pPV = pDrawView->GetPageViewPvNum(0);
                        if ( pPV && pObj->GetPage() == pPV->GetPage() )
                        {
                            pDrawView->MarkObj( pObj, pPV );
                        }
                    }
                }
                else	// Shape Collection
                {
                    SdrPageView* pPV = NULL;
                    long nCount = xShapeColl->getCount();
                    for ( long i = 0; i < nCount; i++ )
                    {
                        Reference< drawing::XShape >  xShapeInt;
                        uno::Any aAny = xShapeColl->getByIndex(i);
                        if ( aAny.getValueType() == ::getCppuType((Reference<drawing::XShape>*)0))
                            xShapeInt = *((Reference< drawing::XShape > *) aAny.getValue());
                        if (xShapeInt.is())
                        {
                            Reference< lang::XUnoTunnel> xShapeTunnel(xShapeInt, uno::UNO_QUERY);

                            SvxShape* pShape = xShapeTunnel.is() ?
                                (SvxShape*)xShapeTunnel->getSomething(SvxShape::getUnoTunnelId()) : 0;

                            if (pShape)
                            {
                                SdrObject *pObj = pShape->GetSdrObject();
                                if (pObj)
                                {
                                    if (!pPV)				// erstes Objekt
                                    {
//										lcl_ShowObject( *pViewSh, *pDrawView, pObj );
                                        pPV = pDrawView->GetPageViewPvNum(0);
                                    }
                                    if ( pPV && pObj->GetPage() == pPV->GetPage() )
                                    {
                                        pDrawView->MarkObj( pObj, pPV );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return sal_False;

}
/*-- 17.12.98 09:34:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextView::getSelection(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< uno::XInterface >  aRef;
    SwView* pView = ((SwXTextView*)this)->GetView();
    if(pView)
    {
        // ein interface aus der aktuellen Selektion erzeugen
        SwWrtShell& rSh = pView->GetWrtShell();
        ShellModes	eSelMode = pView->GetShellMode();
        switch(eSelMode)
        {
            case SEL_TABLE_TEXT      :
            {
                if(rSh.GetTableCrsr())
                {
                    DBG_ASSERT(rSh.GetTableFmt(), "kein Tabellenformat?");
                    Reference< text::XTextTableCursor >  xCrsr = new SwXTextTableCursor(*rSh.GetTableFmt(),
                                                    rSh.GetTableCrsr());
                    aRef = Reference< uno::XInterface >  (xCrsr, uno::UNO_QUERY);;
                    break;
                }

            }
            // ohne Tabellenselektion wird der Text geliefert
            //break;
            case SEL_LIST_TEXT       :
            case SEL_TABLE_LIST_TEXT:
            case SEL_TEXT            :
            {
                Reference< container::XIndexAccess >  xPos = new SwXTextRanges(rSh.GetCrsr());
                aRef = Reference< uno::XInterface >(xPos, uno::UNO_QUERY);
            }
            break;
            case SEL_FRAME           :
            case SEL_GRAPHIC         :
            case SEL_OBJECT          :
            {
                //Get FlyFrameFormat; fuer UI Macro Anbindung an Flys
                const SwFrmFmt* pFmt = rSh.GetFlyFrmFmt();
                SwXFrame* pxFrame = (SwXFrame*)SwClientIter((SwFrmFmt&)*pFmt).
                                                First(TYPE(SwXFrame));

                if(pxFrame)				   //das einzige gemeinsame interface fuer alle Frames
                {
                    aRef = Reference< uno::XInterface >((cppu::OWeakObject*)pxFrame, uno::UNO_QUERY);
                }
                else
                {
                    if(SEL_FRAME == eSelMode)
                    {
                        Reference< text::XTextFrame >  xFrm =  new SwXTextFrame((SwFrmFmt&)*pFmt);
                        aRef = Reference< uno::XInterface >(xFrm, uno::UNO_QUERY);
                    }
                    else if(SEL_GRAPHIC == eSelMode)
                    {
                        Reference< text::XTextContent >  xFrm = new SwXTextGraphicObject((SwFrmFmt&)*pFmt);
                        aRef = xFrm;
                    }
                    else
                    {
                        Reference< text::XTextContent >  xFrm =  new SwXTextEmbeddedObject((SwFrmFmt&)*pFmt);
                        aRef = xFrm;
                    }
                }
            }
            break;
            case SEL_DRAW            :
            case SEL_DRAW_CTRL       :
            case SEL_DRAW_FORM       :
            case SEL_DRAWTEXT        :
            case SEL_BEZIER          :
            {
                Reference< drawing::XDrawPageSupplier >  xPageSupp;
                Reference< frame::XModel > xModel = pView->GetDocShell()->GetBaseModel();
                Reference< lang::XUnoTunnel > xModelTunnel(xModel, uno::UNO_QUERY);
                SwXTextDocument* pTextDoc = (SwXTextDocument*)xModelTunnel->
                                getSomething(SwXTextDocument::getUnoTunnelId());

                SwFmDrawPage* pSvxDrawPage = 	pTextDoc->GetDrawPage()->GetSvxPage();
                Reference< drawing::XShapes >  xShCol = new SvxShapeCollection();

                const SdrMarkList& rMarkList = rSh.GetDrawView()->GetMarkList();
                for(sal_uInt16 i = 0; i < rMarkList.GetMarkCount(); i++)
                {
                    SdrObject* pObj = rMarkList.GetMark(i)->GetObj();
                    Reference< uno::XInterface >  xInt = pSvxDrawPage->GetInterface( pObj );
                    Reference< drawing::XShape >  xShape(xInt, uno::UNO_QUERY);;
                    xShCol->add(xShape);
                }
                aRef = Reference< uno::XInterface >(xShCol, uno::UNO_QUERY);
            }
            break;
        }
    }
    uno::Any aRet(&aRef, ::getCppuType((Reference<uno::XInterface>*)0));
    return aRet;
}
/*-- 17.12.98 09:34:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextView::addSelectionChangeListener(
                                    const Reference< view::XSelectionChangeListener > & rxListener)
                                    throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    DBG_WARNING("not implemented")

    Reference< view::XSelectionChangeListener > * pInsert = new Reference< view::XSelectionChangeListener > ;
    *pInsert = rxListener;
    aSelChangedListeners.Insert(pInsert, aSelChangedListeners.Count());
}
/*-- 17.12.98 09:34:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextView::removeSelectionChangeListener(
                                        const Reference< view::XSelectionChangeListener > & rxListener)
                                        throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRemoved = sal_False;
    view::XSelectionChangeListener* pLeft = rxListener.get();
    for(sal_uInt16 i = 0; i < aSelChangedListeners.Count(); i++)
    {
        Reference< view::XSelectionChangeListener > * pElem = aSelChangedListeners.GetObject(i);
         view::XSelectionChangeListener* pRight = pElem->get();
        if(pLeft == pRight)
        {
            aSelChangedListeners.Remove(i);
            bRemoved = sal_True;
            delete pElem;
        }
    }
    if(!bRemoved)
        throw uno::RuntimeException();
}
/*-- 17.12.98 09:34:27---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< awt::XControl >  SwXTextView::getControl(const Reference< awt::XControlModel > & xModel)
        throw( container::NoSuchElementException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< awt::XControl >  xRet;

    ViewShell *pVSh = 0;
    SwView* pView = ((SwXTextView*)this)->GetView();
    if(pView)
    {
        SdrModel* pModel = pView->GetDocShell()->GetDoc()->GetDrawModel();
        if(pModel)
        {
            SdrPage* pPage = pModel->GetPage( 0 );
            Window *pWin = pView->GetWrtShell().GetWin();
            sal_uInt32 nCount = pPage->GetObjCount();
            for( sal_uInt32 i=0; i< nCount; i++ )
            {
                SdrObject* pObj = pPage->GetObj(i);
                SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, pObj );
                SdrObjGroup* pGroup;
                if( pFormObj )
                {
                    Reference< awt::XControlModel >  xCM = pFormObj->GetUnoControlModel();
                    if( xCM.is() && xModel == xCM )
                    {
                        xRet = pFormObj->GetUnoControl( pWin );
                        break;
                    }
                }
                else if(0 != (pGroup = PTR_CAST( SdrObjGroup, pObj )))
                {
                    if(lcl_FindObjInGroup(xRet, pGroup, xModel, pWin))
                        break;
                }
            }
        }
    }
    return xRet;
}
/*-- 17.12.98 09:34:28---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextViewCursor >  SwXTextView::getViewCursor(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    SwView* pView = ((SwXTextView*)this)->GetView();
    if(pView)
    {
        if(!pxTextViewCursor)
        {
            ((SwXTextView*)this)->pxTextViewCursor = new Reference< text::XTextViewCursor > ;
            *pxTextViewCursor = new  SwXTextViewCursor(pView);
        }
        return *pxTextViewCursor;
    }
    else
        throw uno::RuntimeException();
    return Reference< text::XTextViewCursor > ();
}
/*-- 17.12.98 09:34:28---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< beans::XPropertySet >  SwXTextView::getViewSettings(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        if(!pxViewSettings)
        {
            ((SwXTextView*)this)->pxViewSettings = new Reference< beans::XPropertySet > ;
            *pxViewSettings = new SwXViewSettings(sal_False, pView);
        }
    }
    else
        throw uno::RuntimeException();
    return *pxViewSettings;
}

/*-- 17.12.98 09:34:29---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextView::NotifySelChanged()
{
    Reference< uno::XInterface >  xInt = (cppu::OWeakObject*)(SfxBaseController*)this;

     lang::EventObject aEvent(xInt);

    sal_uInt16 nCount = aSelChangedListeners.Count();
    for ( sal_uInt16 i = nCount; i--; )
    {
        Reference< view::XSelectionChangeListener >  *pObj = aSelChangedListeners[i];
        (*pObj)->selectionChanged(aEvent);
    }
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextView::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextView");
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextView::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextView") == rServiceName;
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextView::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextView");
    return aRet;
}

/******************************************************************
 * SwXTextViewCursor
 ******************************************************************/
/*-- 17.12.98 09:36:23---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextViewCursor::SwXTextViewCursor(SwView* pVw) :
    pView(pVw),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR))
{

}
/*-- 17.12.98 09:36:24---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextViewCursor::~SwXTextViewCursor()
{
}
/*-- 17.12.98 09:36:25---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::isVisible(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    DBG_WARNING("not implemented")
    return sal_True;
}
/*-- 17.12.98 09:36:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::setVisible(sal_Bool bVisible) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    DBG_WARNING("not implemented")
}
/*-- 17.12.98 09:36:26---------------------------------------------------

  -----------------------------------------------------------------------*/
awt::Point SwXTextViewCursor::getPosition(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    awt::Point aRet;
    if(pView)
    {
        const SwWrtShell& rSh = pView->GetWrtShell();
        const SvxULSpaceItem& rUL = rSh.GetPageDesc( rSh.GetCurPageDesc()
                                                    ).GetMaster().GetULSpace();
        SwRect aCharRect(rSh.GetCharRect());
        long nY = aCharRect.Top() -
                                (rUL.GetUpper() + DOCUMENTBORDER);
        aRet.Y = TWIP_TO_MM100(nY);
        const SvxLRSpaceItem& rLR = rSh.GetPageDesc( rSh.GetCurPageDesc()
                                                    ).GetMaster().GetLRSpace();
        long nX = aCharRect.Left() -
                                (rLR.GetLeft() + DOCUMENTBORDER);
        aRet.X = TWIP_TO_MM100(nX);
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 17.12.98 09:36:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::collapseToStart(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        if(rSh.HasSelection())
        {
            SwPaM* pShellCrsr = rSh.GetCrsr();
            if(*pShellCrsr->GetPoint() > *pShellCrsr->GetMark())
                pShellCrsr->Exchange();
            pShellCrsr->DeleteMark();
            rSh.EnterStdMode();
            rSh.SetSelection(*pShellCrsr);
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 17.12.98 09:36:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::collapseToEnd(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        if(rSh.HasSelection())
        {
            SwPaM* pShellCrsr = rSh.GetCrsr();
            if(*pShellCrsr->GetPoint() < *pShellCrsr->GetMark())
                pShellCrsr->Exchange();
            pShellCrsr->DeleteMark();
            rSh.EnterStdMode();
            rSh.SetSelection(*pShellCrsr);
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 17.12.98 09:36:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::isCollapsed(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        const SwWrtShell& rSh = pView->GetWrtShell();
        bRet = rSh.HasSelection();
    }
    else
        throw uno::RuntimeException();
    return bRet;

}
/*-- 17.12.98 09:36:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::goLeft(sal_Int16 nCount, sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        for( sal_uInt16 i = 0; i < nCount; i++ )
            bRet = pView->GetWrtShell().Left( bExpand, 1, sal_True );
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::goRight(sal_Int16 nCount, sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        for( sal_uInt16 i = 0; i < nCount; i++ )
            bRet = pView->GetWrtShell().Right( bExpand, 1, sal_True );
    }
    else
        throw uno::RuntimeException();
    return bRet;

}
/* -----------------08.03.99 11:18-------------------
 *
 * --------------------------------------------------*/
void SwXTextViewCursor::gotoRange(const Reference< text::XTextRange > & xRange, sal_Bool bExpand) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView && xRange.is())
    {
        ShellModes	eSelMode = pView->GetShellMode();
        // ein interface aus der aktuellen Selektion erzeugen
        SwWrtShell& rSh = pView->GetWrtShell();
        rSh.EnterStdMode();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwPaM aOwnPaM(*pShellCrsr->GetPoint());
        if(pShellCrsr->HasMark())
        {
            aOwnPaM.SetMark();
            *aOwnPaM.GetMark() = *pShellCrsr->GetMark();
        }

        Reference<lang::XUnoTunnel> xRangeTunnel( xRange, uno::UNO_QUERY);
        SwXTextRange* pRange = 0;
        SwXTextCursor* pCursor = 0;
        if(xRangeTunnel.is())
        {
            pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                    SwXTextRange::getUnoTunnelId());
            pCursor = (SwXTextCursor*)xRangeTunnel->getSomething(
                                    SwXTextCursor::getUnoTunnelId());
        }

        const sal_uInt16 nFrmType = rSh.GetFrmType(0,sal_True);

        SwStartNodeType eSearchNodeType = SwNormalStartNode;
        if(nFrmType & FRMTYPE_FLY_ANY)
            eSearchNodeType = SwFlyStartNode;
        else if(nFrmType &FRMTYPE_HEADER)
            eSearchNodeType = SwHeaderStartNode;
        else if(nFrmType & FRMTYPE_FOOTER)
            eSearchNodeType = SwFooterStartNode;
        else if(nFrmType & FRMTYPE_TABLE)
            eSearchNodeType = SwTableBoxStartNode;
        else if(nFrmType & FRMTYPE_FOOTNOTE)
            eSearchNodeType = SwFootnoteStartNode;

        const SwStartNode* pOwnStartNode = aOwnPaM.GetNode()->
                                                FindSttNodeByType(eSearchNodeType);

        const SwNode* pSrcNode = 0;
        if(pCursor && pCursor->GetCrsr())
        {
            pSrcNode = pCursor->GetCrsr()->GetNode();
        }
        else if(pRange && pRange->GetBookmark())
        {
            SwBookmark* pBkm = pRange->GetBookmark();
            pSrcNode = &pBkm->GetPos().nNode.GetNode();
        }
        const SwStartNode* pTmp = pSrcNode ? pSrcNode->FindSttNodeByType(eSearchNodeType) : 0;

        //SectionNodes ueberspringen
        while(pTmp && pTmp->IsSectionNode())
        {
            pTmp = pTmp->FindStartNode();
        }
        //ohne Expand darf mit dem ViewCursor ueberall hingesprungen werden
        //mit Expand nur in der gleichen Umgebung
        if(bExpand &&
            (pOwnStartNode != pTmp ||
            (eSelMode != SEL_LIST_TEXT &&
                eSelMode != SEL_TABLE_LIST_TEXT &&
                    eSelMode != SEL_TEXT)))
            throw uno::RuntimeException();

        //jetzt muss die Selektion erweitert werden
        if(bExpand)
        {
            // der Cursor soll alles einschliessen, was bisher von ihm und dem uebergebenen
            // Range eingeschlossen wurde
            SwPosition aOwnLeft(*aOwnPaM.GetPoint());
            SwPosition aOwnRight(aOwnPaM.HasMark() ? *aOwnPaM.GetMark() : aOwnLeft);
            if(aOwnRight < aOwnLeft)
            {
                SwPosition aTmp = aOwnLeft;
                aOwnLeft = aOwnRight;
                aOwnRight = aTmp;
            }
            SwPosition* pParamLeft;
            SwPosition* pParamRight;
            if(pCursor)
            {
                const SwUnoCrsr* pTmp = pCursor->GetCrsr();
                pParamLeft = new SwPosition(*pTmp->GetPoint());
                pParamRight = new SwPosition(pTmp->HasMark() ? *pTmp->GetMark() : *pParamLeft);
            }
            else
            {
                SwBookmark* pBkm = pRange->GetBookmark();
                pParamLeft = new SwPosition(pBkm->GetPos());
                pParamRight = new SwPosition(pBkm->GetOtherPos() ? *pBkm->GetOtherPos() : *pParamLeft);
            }
            if(*pParamRight < *pParamLeft)
            {
                SwPosition* pTmp = pParamLeft;
                pParamLeft = pParamRight;
                pParamRight = pTmp;
            }
            // jetzt sind vier SwPositions da, zwei davon werden gebraucht, also welche?
            if(aOwnRight < *pParamRight)
                *aOwnPaM.GetPoint() = aOwnRight;
            else
                *aOwnPaM.GetPoint() = *pParamRight;
            aOwnPaM.SetMark();
            if(aOwnLeft < *pParamLeft)
                *aOwnPaM.GetMark() = *pParamLeft;
            else
                *aOwnPaM.GetMark() = aOwnLeft;

            delete pParamLeft;
            delete pParamRight;
        }
        else
        {
            //der Cursor soll dem uebergebenen Range entsprechen
            if(pCursor)
            {
                const SwUnoCrsr* pTmp = pCursor->GetCrsr();
                *aOwnPaM.GetPoint() = *pTmp->GetPoint();
                if(pTmp->HasMark())
                {
                    aOwnPaM.SetMark();
                    *aOwnPaM.GetMark() = *pTmp->GetMark();
                }
                else
                    aOwnPaM.DeleteMark();
            }
            else
            {
                SwBookmark* pBkm = pRange->GetBookmark();
                *aOwnPaM.GetPoint() = pBkm->GetPos();
                if(pBkm->GetOtherPos())
                {
                    aOwnPaM.SetMark();
                    *aOwnPaM.GetMark() = *pBkm->GetOtherPos();
                }
                else
                    aOwnPaM.DeleteMark();
            }
        }
        rSh.SetSelection(aOwnPaM);
    }
    else
        throw uno::RuntimeException();

}
/*-- 17.12.98 09:36:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::gotoStart(sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
        pView->GetWrtShell().SttDoc( bExpand );
    else
        throw uno::RuntimeException();
}
/*-- 17.12.98 09:36:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::gotoEnd(sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
        pView->GetWrtShell().EndDoc( bExpand );
    else
        throw uno::RuntimeException();
}
/*-- 17.12.98 09:36:28---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToFirstPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        bRet = pView->GetWrtShell().SttDoc();
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:29---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToLastPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        bRet = rSh.EndDoc();
        rSh.SttPg();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:30---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToPage(sal_Int16 nPage) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        bRet = pView->GetWrtShell().GotoPage(nPage);
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:30---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToNextPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        pView->GetWrtShell().SttNxtPg();
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:31---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToPreviousPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        bRet = pView->GetWrtShell().EndPrvPg();
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:32---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToEndOfPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        bRet = pView->GetWrtShell().EndPg();
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:32---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::jumpToStartOfPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
        bRet = pView->GetWrtShell().SttPg();
    else
        throw uno::RuntimeException();
    return bRet;
}
/* -----------------04.10.99 14:21-------------------

 --------------------------------------------------*/
sal_Int16 SwXTextViewCursor::getPage(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    short nRet = 0;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        sal_uInt16 nPage, nLogPage;
        rSh.GetPageNum( nPage, nLogPage,
                           rSh.IsCrsrVisible(), sal_False);
        nRet = (short)nPage;
    }
    else
        throw uno::RuntimeException();
    return nRet;
}
/*-- 17.12.98 09:36:33---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::screenDown(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        SfxRequest aReq(FN_PAGEDOWN, SFX_CALLMODE_SLOT, pView->GetPool());
        pView->Execute(aReq);
        const SfxPoolItem* pRet = aReq.GetReturnValue();
        bRet = pRet && ((const SfxBoolItem*)pRet)->GetValue();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 09:36:33---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::screenUp(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        SfxRequest aReq(FN_PAGEUP, SFX_CALLMODE_SLOT, pView->GetPool());
        pView->Execute(aReq);
        const SfxPoolItem* pRet = aReq.GetReturnValue();
        bRet = pRet && ((const SfxBoolItem*)pRet)->GetValue();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 11:59:05---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XText >  SwXTextViewCursor::getText(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XText >  xRet;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwDoc* pDoc = pView->GetDocShell()->GetDoc();
        Reference< text::XTextRange >  xRg = ::CreateTextRangeFromPosition(pDoc,
                                    *pShellCrsr->Start(), 0);
        xRet = xRg->getText();
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 17.12.98 11:59:05---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXTextViewCursor::getStart(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XTextRange >  xRet;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwDoc* pDoc = pView->GetDocShell()->GetDoc();
        xRet = ::CreateTextRangeFromPosition(pDoc,
                                    *pShellCrsr->Start(), 0);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 17.12.98 11:59:06---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXTextViewCursor::getEnd(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XTextRange >  xRet;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwDoc* pDoc = pView->GetDocShell()->GetDoc();
        xRet = ::CreateTextRangeFromPosition(pDoc,
                                        *pShellCrsr->End(), 0);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/* -----------------12.10.99 09:03-------------------

 --------------------------------------------------*/
OUString SwXTextViewCursor::getString(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    OUString uRet;
    if(pView)
    {
        ShellModes	eSelMode = pView->GetShellMode();
        switch(eSelMode)

            case SEL_LIST_TEXT       :
            case SEL_TABLE_LIST_TEXT:
            case SEL_TEXT            :
            {
                SwWrtShell& rSh = pView->GetWrtShell();
                SwPaM* pShellCrsr = rSh.GetCrsr();
                SwXTextCursor::getTextFromPam(*pShellCrsr, uRet);
            }
    }
    return uRet;
}
/*-- 17.12.98 11:59:06---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::setString(const OUString& aString) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    DBG_WARNING("not implemented")
}

/*-- 29.06.00 17:33:38---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XPropertySetInfo >  SwXTextViewCursor::getPropertySetInfo(  ) throw(RuntimeException)
{
    static Reference< XPropertySetInfo >  xRef = aPropSet.getPropertySetInfo();
    return xRef;
}
/*-- 29.06.00 17:33:39---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::setPropertyValue( const OUString& rPropertyName, const Any& aValue )
                            throw(UnknownPropertyException, PropertyVetoException,
                                IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwXTextCursor::SetPropertyValue(*pShellCrsr, aPropSet, rPropertyName, aValue );
    }
    else
        throw RuntimeException();
}
/*-- 29.06.00 17:33:39---------------------------------------------------

  -----------------------------------------------------------------------*/
Any  SwXTextViewCursor::getPropertyValue( const OUString& rPropertyName )
                throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Any aRet;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        aRet = SwXTextCursor::GetPropertyValue(	 *pShellCrsr, aPropSet, rPropertyName);
    }
    else
        throw RuntimeException();
    return aRet;
}
/*-- 29.06.00 17:33:40---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::addPropertyChangeListener(
    const OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
/*-- 29.06.00 17:33:40---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::removePropertyChangeListener(
    const OUString& aPropertyName, const Reference< XPropertyChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
/*-- 29.06.00 17:33:41---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::addVetoableChangeListener(
    const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
/*-- 29.06.00 17:33:41---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::removeVetoableChangeListener( const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener ) throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
/*-- 29.06.00 17:33:41---------------------------------------------------

  -----------------------------------------------------------------------*/
PropertyState  SwXTextViewCursor::getPropertyState( const OUString& rPropertyName ) throw(UnknownPropertyException, RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    PropertyState eState;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        eState = SwXTextCursor::GetPropertyState( *pShellCrsr, aPropSet, rPropertyName);
    }
    else
        throw RuntimeException();
    return eState;
}
/*-- 29.06.00 17:33:42---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence< PropertyState >  SwXTextViewCursor::getPropertyStates(
    const Sequence< OUString >& rPropertyNames ) throw(UnknownPropertyException, RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Sequence< PropertyState >  aRet;
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        aRet = SwXTextCursor::GetPropertyStates(*pShellCrsr, aPropSet,	rPropertyNames);
    }
    return aRet;
}
/*-- 29.06.00 17:33:42---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwXTextViewCursor::setPropertyToDefault( const OUString& rPropertyName )
                                        throw(UnknownPropertyException, RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        SwXTextCursor::SetPropertyToDefault( *pShellCrsr, aPropSet,	rPropertyName);
    }
}
/*-- 29.06.00 17:33:43---------------------------------------------------

  -----------------------------------------------------------------------*/
Any  SwXTextViewCursor::getPropertyDefault( const OUString& rPropertyName )
                        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aRet;
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
    {
        SwWrtShell& rSh = pView->GetWrtShell();
        SwPaM* pShellCrsr = rSh.GetCrsr();
        aRet = SwXTextCursor::GetPropertyDefault( *pShellCrsr, aPropSet, rPropertyName);
    }
    return aRet;
}

/*------------------------------------------------------------------------
    $Log: not supported by cvs2svn $
    Revision 1.76  2000/09/18 16:06:16  willem.vandorp
    OpenOffice header added.
    
    Revision 1.75  2000/09/07 15:59:34  os
    change: SFX_DISPATCHER/SFX_BINDINGS removed
    
    Revision 1.74  2000/07/10 12:32:19  os
    chg: acquire/release don't throw exceptions

    Revision 1.73  2000/06/29 16:45:50  os
    #75928# XTextViewCursor implements XPropertySet/XPropertyState

    Revision 1.72  2000/06/27 07:57:29  obo
    #65293#

    Revision 1.71  2000/05/24 13:36:13  hr
    exception specifications

    Revision 1.70  2000/05/23 07:39:15  os
    SwXTextView : inheritance changed

    Revision 1.69  2000/05/18 08:35:56  os
    XInterface - mothods

    Revision 1.68  2000/05/16 09:15:14  os
    project usr removed

    Revision 1.67  2000/05/10 11:55:31  os
    Basic API removed

    Revision 1.66  2000/04/26 15:00:45  os
    GetName() returns const String&

    Revision 1.65  2000/04/11 08:05:00  os
    UNICODE

    Revision 1.64  2000/03/27 10:36:31  os
    UNO III

    Revision 1.63  2000/03/21 15:39:44  os
    UNOIII

    Revision 1.62  2000/02/11 14:59:57  hr
    #70473# changes for unicode ( patched by automated patchtool )

    Revision 1.61  2000/01/03 12:16:26  os
    #71284# ::select: query for SwXShape-base class before calling getImplementation()


    Revision 1.60  1999/11/25 15:37:50  os
    headers corrected

    Revision 1.59  1999/11/19 16:38:03  os
    modules renamed

    Revision 1.58  1999/11/03 14:39:20  os
    getString:: convert to UNICODE

    Revision 1.57  1999/10/12 07:24:19  os
    #69115# API parameter in GotoLeftRight/LRMargin; getText/getStart+End/collapse

    Revision 1.56  1999/10/05 10:22:04  os
    new method: getPage

    Revision 1.55  1999/09/28 09:12:43  os
    #67167# view::XLineCursor, view::XViewCursor inherited

    Revision 1.54  1999/09/07 05:59:43  os
    #67887# frame::XDispatchProvider

    Revision 1.53  1999/08/31 09:32:00  OS
    #68459# ::select: correct conversion from uno::Any


      Rev 1.52   31 Aug 1999 11:32:00   OS
   #68459# ::select: correct conversion from uno::Any

      Rev 1.51   26 Aug 1999 11:44:10   HR
   select/getSelection corrected

      Rev 1.50   25 Aug 1999 16:47:32   OS
   getSelection/select with uno::Any

      Rev 1.49   16 Aug 1999 17:21:14   OS
   #65953# select(): shapes implemented
=======
/*-- 28.09.99 08:31:19---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::goDown(sal_Int16 nCount, sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        for( sal_uInt16 i = 0; i < nCount; i++ )
            bRet = pView->GetWrtShell().Down( bExpand, 1, sal_True );
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 28.09.99 08:31:20---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::goUp(sal_Int16 nCount, sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        for( sal_uInt16 i = 0; i < nCount; i++ )
            bRet = pView->GetWrtShell().Up( bExpand, 1, sal_True );
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 28.09.99 08:31:20---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::isAtStartOfLine(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        bRet = pView->GetWrtShell().IsAtLeftMargin();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 28.09.99 08:31:21---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextViewCursor::isAtEndOfLine(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pView)
    {
        bRet = pView->GetWrtShell().IsAtRightMargin(sal_True);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 28.09.99 08:31:21---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::gotoEndOfLine(sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
        pView->GetWrtShell().RightMargin(bExpand, sal_True);
    else
        throw uno::RuntimeException();
}
/*-- 28.09.99 08:31:22---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextViewCursor::gotoStartOfLine(sal_Bool bExpand) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(pView)
        pView->GetWrtShell().LeftMargin(bExpand, sal_True);
    else
        throw uno::RuntimeException();
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextViewCursor::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextViewCursor");
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextViewCursor::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextViewCursor") == rServiceName;
}
/* -----------------------------06.04.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextViewCursor::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextViewCursor");
    return aRet;
}

/*------------------------------------------------------------------------
    $Log: not supported by cvs2svn $
    Revision 1.76  2000/09/18 16:06:16  willem.vandorp
    OpenOffice header added.
    
    Revision 1.75  2000/09/07 15:59:34  os
    change: SFX_DISPATCHER/SFX_BINDINGS removed
    
    Revision 1.74  2000/07/10 12:32:19  os
    chg: acquire/release don't throw exceptions

    Revision 1.73  2000/06/29 16:45:50  os
    #75928# XTextViewCursor implements XPropertySet/XPropertyState

    Revision 1.72  2000/06/27 07:57:29  obo
    #65293#

    Revision 1.71  2000/05/24 13:36:13  hr
    exception specifications

    Revision 1.70  2000/05/23 07:39:15  os
    SwXTextView : inheritance changed

    Revision 1.69  2000/05/18 08:35:56  os
    XInterface - mothods

    Revision 1.68  2000/05/16 09:15:14  os
    project usr removed

    Revision 1.67  2000/05/10 11:55:31  os
    Basic API removed

    Revision 1.66  2000/04/26 15:00:45  os
    GetName() returns const String&

    Revision 1.65  2000/04/11 08:05:00  os
    UNICODE

    Revision 1.64  2000/03/27 10:36:31  os
    UNO III

    Revision 1.63  2000/03/21 15:39:44  os
    UNOIII

    Revision 1.62  2000/02/11 14:59:57  hr
    #70473# changes for unicode ( patched by automated patchtool )

    Revision 1.61  2000/01/03 12:16:26  os
    #71284# ::select: query for SwXShape-base class before calling getImplementation()


    Revision 1.60  1999/11/25 15:37:50  os
    headers corrected

    Revision 1.59  1999/11/19 16:38:03  os
    modules renamed

    Revision 1.58  1999/11/03 14:39:20  os
    getString:: convert to UNICODE

    Revision 1.57  1999/10/12 07:24:19  os
    #69115# API parameter in GotoLeftRight/LRMargin; getText/getStart+End/collapse

    Revision 1.56  1999/10/05 10:22:04  os
    new method: getPage

    Revision 1.55  1999/09/28 09:12:43  os
    #67167# view::XLineCursor, view::XViewCursor inherited


      Rev 1.48   13 Jul 1999 09:34:00   OS
   getPosition etwas optimiert

      Rev 1.47   07 Jul 1999 08:52:10   OS
   #67471# view::XSelectionChangeListener now supported

      Rev 1.46   22 Apr 1999 16:09:06   OS
   #65194# throw -> throw

      Rev 1.45   22 Apr 1999 15:28:52   OS
   #65124# not implemented - nur noch DBG_WARNING

      Rev 1.44   07 Apr 1999 10:26:32   OS
   #60348# richtiges Shape liefern

      Rev 1.43   15 Mar 1999 14:38:26   OS
   #62845# Makro fuer ServiceInfo jetzt auch fuer OS/2

      Rev 1.42   12 Mar 1999 09:57:24   OS
   #62845# lang::XServiceInfo impl.

      Rev 1.41   10 Mar 1999 16:19:38   OS
   #63127# Controller-Methoden von der Basisklasse

      Rev 1.40   09 Mar 1999 12:38:08   OS
   #62393# gotoRange verbessert, #62008# Solar-Mutex

      Rev 1.39   08 Mar 1999 07:43:02   MH
   update 515

      Rev 1.38   05 Mar 1999 08:51:46   OS
   #62191# INT statt UINT

      Rev 1.37   10 Feb 1999 16:15:26   OS
// automatisch auskommentiert - [getIdlClass(es) or queryInterface] - Bitte XTypeProvider benutzen!
//   #52654# lang::XComponent im queryInterface


      Rev 1.36   28 Jan 1999 16:27:50   OS
   #56371# keine Objekte fuer DEBUG anlegen

      Rev 1.35   27 Jan 1999 12:06:08   OS
   #56371# TF_ONE51

      Rev 1.34   27 Jan 1999 09:59:16   OS
   #56371# TF_ONE51

      Rev 1.33   17 Dec 1998 12:09:10   OS
   #56371# TF_ONE51 Zwischenstand

      Rev 1.32   10 Dec 1998 15:54:04   OS
   #56371# TF_ONE51 Zwischenstand

      Rev 1.31   11 Nov 1998 10:58:06   AWO
   Include fehlte

      Rev 1.30   09 Nov 1998 14:44:10   OS
   #58785# getSelection an der XTextView vollstaendig

      Rev 1.29   04 Nov 1998 15:55:26   OS
   #58289# getControl geht auch in Gruppen

      Rev 1.28   05 Oct 1998 17:32:22   HR
   UIK() Makro

      Rev 1.27   01 Oct 1998 15:58:14   OM
   #52654# SelectionChangeListener

      Rev 1.26   01 Oct 1998 11:40:54   HR
   Typo beseitigt

      Rev 1.25   18 Sep 1998 11:40:56   OS
   #56651# GotoBookmark optimiert

      Rev 1.24   18 Sep 1998 09:31:34   OS
   52654# view::XScreenCursor

      Rev 1.23   17 Sep 1998 15:53:38   OS
   #52654# text::TextViewCursor(-Supplier)

      Rev 1.22   07 Sep 1998 09:07:06   OS
   #54862# add/removeSelectionChangeListener

      Rev 1.21   10 Jul 1998 18:09:40   OS
   PropertySetInfo und IdlClass static

      Rev 1.20   08 Jul 1998 10:00:00   TJ
   include

      Rev 1.19   03 Jul 1998 07:22:40   OS
   Ableitung von view::XControlAccess

      Rev 1.18   18 Jun 1998 13:23:34   OS
   include-Umstellung 396c

      Rev 1.17   18 Jun 1998 09:23:14   OS
   Ableitung von SfxOfficeDocController

      Rev 1.16   11 Jun 1998 11:06:40   TJ
   getActualViewData -> getViewData

      Rev 1.15   11 Jun 1998 08:15:48   OS
   Code-Kopie war nix#

      Rev 1.14   04 Jun 1998 09:40:28   OS
// automatisch auskommentiert - [getIdlClass(es) or queryInterface] - Bitte XTypeProvider benutzen!
//   getIdlClasses


      Rev 1.13   02 Jun 1998 16:18:10   OS
   391-Anpassung

      Rev 1.12   02 Jun 1998 15:50:34   OS
   SwXTextView von frame::XController abgeleitet

      Rev 1.11   26 May 1998 12:33:12   OS
   SetSelection fuer Cursor

      Rev 1.10   22 May 1998 15:15:24   OS
   get/setSelection erweitert

      Rev 1.9   14 May 1998 17:50:08   OS
   div. Namensaenderungen

      Rev 1.8   09 Apr 1998 15:10:34   OS
   Uno-Umstellung

      Rev 1.7   08 Apr 1998 12:52:24   OS
   text::ViewSettings auch fuer die TextView; select fuer Frames u. text::Bookmarks

      Rev 1.6   03 Apr 1998 14:37:52   OS
   wieder aktiviert

      Rev 1.5   30 Jan 1998 15:10:46   OS
   wieder uebersetzbar

      Rev 1.4   19 Jan 1998 14:58:04   OS
   wieder uebersetzbar

      Rev 1.3   08 Jan 1998 12:56:48   OS
   UsrUik -> uno::Uik

      Rev 1.2   19 Dec 1997 18:04:56   OS
   Jahressicherung

      Rev 1.1   17 Dec 1997 16:19:48   OS
   Tabellenanfang

      Rev 1.0   16 Dec 1997 11:52:34   OS
   Initial revision.

------------------------------------------------------------------------*/

