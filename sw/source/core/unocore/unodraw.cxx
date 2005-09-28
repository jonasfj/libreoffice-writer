/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unodraw.cxx,v $
 *
 *  $Revision: 1.68 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-28 11:22:07 $
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

#include <swtypes.hxx>
#include <cmdid.h>

#include <unomid.h>
#ifndef _UNODRAW_HXX
#include <unodraw.hxx>
#endif
#ifndef _UNOCOLL_HXX
#include <unocoll.hxx>
#endif
#ifndef _UNOFRAME_HXX
#include <unoframe.hxx>
#endif
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif
#ifndef _SVX_UNOPRNMS_HXX
#include <svx/unoprnms.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _SWDOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _UNOPORT_HXX
#include <unoport.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _SWUNDO_HXX //autogen
#include <swundo.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
#ifndef _NDTXT_HXX //autogen
#include <ndtxt.hxx>
#endif
#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif
#ifndef _SVX_UNOSHAPE_HXX
#include <svx/unoshape.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
// OD 2004-04-21 #i26791#
#ifndef _FMTFOLLOWTEXTFLOW_HXX
#include <fmtfollowtextflow.hxx>
#endif
#ifndef _ROOTFRM_HXX //autogen
#include <rootfrm.hxx>
#endif

#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _CRSTATE_HXX //autogen
#include <crstate.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef _E3D_SCENE3D_HXX
#include <svx/scene3d.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
// --> OD 2004-07-23 #i31698#
#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX3_HPP_
#include <com/sun/star/drawing/HomogenMatrix3.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_HORIORIENTATION_HPP_
#include <com/sun/star/text/HoriOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_VERTORIENTATION_HPP_
#include <com/sun/star/text/VertOrientation.hpp>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif
// <--
// --> OD 2004-08-06 #i28749#
#ifndef _COM_SUN_STAR_TEXT_POSITIONLAYOUTDIR_HPP_
#include <com/sun/star/text/PositionLayoutDir.hpp>
#endif
// <--
// OD 2004-05-05 #i28701#
#ifndef _FMTWRAPINFLUENCEONOBJPOS_HXX
#include <fmtwrapinfluenceonobjpos.hxx>
#endif
// --> OD 2004-11-10 #i35007#
#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif
// <--
// --> OD 2005-03-10 #i44334#, #i44681#
#ifndef _B2D_MATRIX3D_HXX
#include <goodies/matrix3d.hxx>
#endif
// <--

using namespace ::com::sun::star;

DECLARE_STL_USTRINGACCESS_MAP( uno::Sequence< sal_Int8 > *,  SwShapeImplementationIdMap );

static SwShapeImplementationIdMap aImplementationIdMap;

/* -----------------22.01.99 13:19-------------------
 *
 * --------------------------------------------------*/
class SwShapeDescriptor_Impl
{
    SwFmtHoriOrient* 	pHOrient;
    SwFmtVertOrient* 	pVOrient;
    SwFmtAnchor*		pAnchor;
    SwFmtSurround*		pSurround;
    SvxULSpaceItem*		pULSpace;
    SvxLRSpaceItem*		pLRSpace;
    sal_Bool            bOpaque;
    uno::Reference< text::XTextRange > xTextRange;
    // OD 2004-04-21 #i26791#
    SwFmtFollowTextFlow* mpFollowTextFlow;
    // OD 2004-05-05 #i28701# - add property 'WrapInfluenceOnObjPos'
    SwFmtWrapInfluenceOnObjPos* pWrapInfluenceOnObjPos;
    // --> OD 2004-08-06 #i28749#
    sal_Int16 mnPositionLayoutDir;
    // <--

public:
    SwShapeDescriptor_Impl() :
     // --> OD 2004-08-18 #i32349# - no defaults, in order to determine on
     // adding a shape, if positioning attributes are set or not.
     pHOrient( 0L ),
     pVOrient( 0L ),
     // <--
     pAnchor(0),
     pSurround(0),
     pULSpace(0),
     pLRSpace(0),
     bOpaque(sal_False),
     // OD 2004-04-21 #i26791#
     mpFollowTextFlow( new SwFmtFollowTextFlow( FALSE ) ),
     // OD 2004-05-05 #i28701#
     // --> OD 2004-10-18 #i35017# - constant name has changed
     pWrapInfluenceOnObjPos( new SwFmtWrapInfluenceOnObjPos(
                            text::WrapInfluenceOnPosition::ONCE_CONCURRENT ) ),
     // <--
     // --> OD 2004-08-06 #i28749#
     mnPositionLayoutDir( text::PositionLayoutDir::PositionInLayoutDirOfAnchor )
     {}

    ~SwShapeDescriptor_Impl()
    {
        delete pHOrient;
        delete pVOrient;
        delete pAnchor;
        delete pSurround;
        delete pULSpace;
        delete pLRSpace;
        // OD 2004-04-22 #i26791#
        delete mpFollowTextFlow;
        // OD 2004-05-05 #i28701#
        delete pWrapInfluenceOnObjPos;
    }
    SwFmtAnchor* 	GetAnchor(sal_Bool bCreate = sal_False)
        {
            if(bCreate && !pAnchor)
                pAnchor = new SwFmtAnchor(FLY_IN_CNTNT);
            return pAnchor;
        }
    SwFmtHoriOrient* GetHOrient(sal_Bool bCreate = sal_False)
        {
            if (bCreate && !pHOrient)
            {
                // OD 2004-06-03 #i26791# - change default
                pHOrient = new SwFmtHoriOrient( 0, HORI_NONE, FRAME );
            }
            return pHOrient;
        }
    SwFmtVertOrient* GetVOrient(sal_Bool bCreate = sal_False)
        {
            if(bCreate && !pVOrient)
            {
                // OD 2004-04-21 #i26791# - change default
                pVOrient = new SwFmtVertOrient( 0, VERT_NONE, FRAME );
            }
            return pVOrient;
        }

    SwFmtSurround*	GetSurround(sal_Bool bCreate = sal_False)
        {
            if(bCreate && !pSurround)
                pSurround = new SwFmtSurround();
            return pSurround;
        }
    SvxLRSpaceItem*	GetLRSpace(sal_Bool bCreate = sal_False)
        {
            if(bCreate && !pLRSpace)
                pLRSpace = new SvxLRSpaceItem();
            return pLRSpace;
        }
    SvxULSpaceItem*	GetULSpace(sal_Bool bCreate = sal_False)
        {
            if(bCreate && !pULSpace)
                pULSpace = new SvxULSpaceItem();
            return pULSpace;
        }
    uno::Reference< text::XTextRange > &    GetTextRange()
    {
        return xTextRange;
    }
    sal_Bool    IsOpaque()
        {
            return bOpaque;
        }
    const sal_Bool&    GetOpaque()
        {
            return bOpaque;
        }
    void RemoveHOrient(){DELETEZ(pHOrient);}
    void RemoveVOrient(){DELETEZ(pVOrient);}
    void RemoveAnchor(){DELETEZ(pAnchor);}
    void RemoveSurround(){DELETEZ(pSurround);}
    void RemoveULSpace(){DELETEZ(pULSpace);}
    void RemoveLRSpace(){DELETEZ(pLRSpace);}
    void SetOpaque(sal_Bool bSet){bOpaque = bSet;}

    // OD 2004-04-21 #i26791#
    SwFmtFollowTextFlow* GetFollowTextFlow( sal_Bool _bCreate = sal_False )
    {
        if ( _bCreate && !mpFollowTextFlow )
            mpFollowTextFlow = new SwFmtFollowTextFlow( FALSE );
        return mpFollowTextFlow;
    }
    void RemoveFollowTextFlow()
    {
        DELETEZ(mpFollowTextFlow);
    }

    // --> OD 2004-08-06 #i28749#
    sal_Int16 GetPositionLayoutDir() const
    {
        return mnPositionLayoutDir;
    }
    void SetPositionLayoutDir( sal_Int16 _nPositionLayoutDir )
    {
        switch ( _nPositionLayoutDir )
        {
            case text::PositionLayoutDir::PositionInHoriL2R:
            case text::PositionLayoutDir::PositionInLayoutDirOfAnchor:
            {
                mnPositionLayoutDir = _nPositionLayoutDir;
            }
            break;
            default:
            {
                ASSERT( false,
                        "<SwShapeDescriptor_Impl::SetPositionLayoutDir(..)> - invalid attribute value." );
            }
        }
    }
    void RemovePositionLayoutDir()
    {
        mnPositionLayoutDir = text::PositionLayoutDir::PositionInLayoutDirOfAnchor;
    }
    // <--

    // OD 2004-05-05 #i28701#
    inline SwFmtWrapInfluenceOnObjPos* GetWrapInfluenceOnObjPos(
                                        const sal_Bool _bCreate = sal_False )
    {
        if ( _bCreate && !pWrapInfluenceOnObjPos )
        {
            pWrapInfluenceOnObjPos = new SwFmtWrapInfluenceOnObjPos(
                        // --> OD 2004-10-18 #i35017# - constant name has changed
                        text::WrapInfluenceOnPosition::ONCE_CONCURRENT );
                        // <--
        }
        return pWrapInfluenceOnObjPos;
    }
    inline void RemoveWrapInfluenceOnObjPos()
    {
        DELETEZ(pWrapInfluenceOnObjPos);
    }
};
/****************************************************************************
    class SwFmDrawPage
****************************************************************************/

/* -----------------28.01.99 12:03-------------------
 *
 * --------------------------------------------------*/
SwFmDrawPage::SwFmDrawPage( SdrPage* pPage ) :
    SvxFmDrawPage( pPage ), pPageView(0)
{
}

/*-- 22.01.99 11:13:07---------------------------------------------------

  -----------------------------------------------------------------------*/
SwFmDrawPage::~SwFmDrawPage() throw ()
{
    RemovePageView();
}
/*-- 22.01.99 11:13:07---------------------------------------------------

  -----------------------------------------------------------------------*/
const SdrMarkList& 	SwFmDrawPage::PreGroup(const uno::Reference< drawing::XShapes > & xShapes)
{
    _SelectObjectsInView( xShapes, GetPageView() );
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    return rMarkList;
}
/*-- 22.01.99 11:13:08---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwFmDrawPage::PreUnGroup(const uno::Reference< drawing::XShapeGroup >  xShapeGroup)
{
    uno::Reference< drawing::XShape >  xShape( xShapeGroup, uno::UNO_QUERY);
    _SelectObjectInView( xShape, GetPageView() );
}
/*-- 22.01.99 11:13:08---------------------------------------------------

  -----------------------------------------------------------------------*/
/*void 	SwFmDrawPage::PostGroup()
{
    pView->GroupMarked();
}
/*-- 22.01.99 11:13:08---------------------------------------------------

  -----------------------------------------------------------------------*/
SdrPageView*	SwFmDrawPage::GetPageView()
{
    if(!pPageView)
        pPageView = pView->ShowPage( pPage, Point() );
    return pPageView;
}
/*-- 22.01.99 11:13:08---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwFmDrawPage::RemovePageView()
{
    if(pPageView && pView)
        pView->HidePage( pPageView );
    pPageView = 0;
}
/*-- 22.01.99 11:13:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< uno::XInterface >  	SwFmDrawPage::GetInterface( SdrObject* pObj )
{
    uno::Reference< XInterface >  xShape;
    if( pObj )
    {
        SwFrmFmt* pFmt = ::FindFrmFmt( pObj );
        SwXShape* pxShape = (SwXShape*)SwClientIter( *pFmt ).
                                                First( TYPE( SwXShape ));
        if(pxShape)
        {
            xShape =  *(cppu::OWeakObject*)pxShape;
        }
        else
            xShape = pObj->getUnoShape();
    }
    return xShape;
}
/*-- 22.01.99 11:13:09---------------------------------------------------

  -----------------------------------------------------------------------*/
SdrObject* SwFmDrawPage::_CreateSdrObject( const uno::Reference< drawing::XShape > & xShape ) throw ()
{
    //TODO: stimmt das so - kann die Methode weg?
    return SvxFmDrawPage::_CreateSdrObject( xShape );
}
/*-- 22.01.99 11:13:09---------------------------------------------------

  -----------------------------------------------------------------------*/

uno::Reference< drawing::XShape >  SwFmDrawPage::_CreateShape( SdrObject *pObj ) const throw ()
{
    uno::Reference< drawing::XShape >  xRet;
    if(pObj->ISA(SwVirtFlyDrawObj) || pObj->GetObjInventor() == SWGInventor)
    {
        SwFlyDrawContact* pFlyContact = (SwFlyDrawContact*)pObj->GetUserCall();
        if(pFlyContact)
        {
            FlyCntType eType;
            SwFrmFmt* pFlyFmt = pFlyContact->GetFmt();
            SwDoc* pDoc = pFlyFmt->GetDoc();
            const SwNodeIndex* pIdx;
            if( RES_FLYFRMFMT == pFlyFmt->Which()
                && 0 != ( pIdx = pFlyFmt->GetCntnt().GetCntntIdx() )
                && pIdx->GetNodes().IsDocNodes()
                )
            {
                const SwNode* pNd = pDoc->GetNodes()[ pIdx->GetIndex() + 1 ];
                if(!pNd->IsNoTxtNode())
                    eType = FLYCNTTYPE_FRM;
                else if( pNd->IsGrfNode() )
                    eType = FLYCNTTYPE_GRF;
                else if( pNd->IsOLENode() )
                    eType = FLYCNTTYPE_OLE;
            }
            else
            {
                ASSERT( false,
                        "<SwFmDrawPage::_CreateShape(..)> - could not retrieve type. Thus, no shape created." );
                return xRet;
            }
            xRet = SwXFrames::GetObject( *pFlyFmt, eType );
        }
     }
    else
    {
        // own block - temporary object has to be destroyed before
        // the delegator is set #81670#
        {
            xRet = SvxFmDrawPage::_CreateShape( pObj );
        }
        uno::Reference< XUnoTunnel > xShapeTunnel(xRet, uno::UNO_QUERY);
        //don't create an SwXShape if it already exists
        SwXShape* pShape = 0;
        if(xShapeTunnel.is())
            pShape = (SwXShape*)xShapeTunnel->getSomething(SwXShape::getUnoTunnelId());
        if(!pShape)
        {
            xShapeTunnel = 0;
            uno::Reference< uno::XInterface > xCreate(xRet, uno::UNO_QUERY);
            xRet = 0;
            uno::Reference< beans::XPropertySet >  xPrSet;
            if ( pObj->IsGroupObject() && (!pObj->Is3DObj() || ( PTR_CAST(E3dScene,pObj ) != NULL ) ) )
                xPrSet = new SwXGroupShape( xCreate );
            else
                xPrSet = new SwXShape( xCreate );
            xRet = uno::Reference< drawing::XShape >(xPrSet, uno::UNO_QUERY);
        }
    }
    return xRet;
}

/****************************************************************************
    class SwXDrawPage
****************************************************************************/
/* -----------------------------06.04.00 13:14--------------------------------

 ---------------------------------------------------------------------------*/
rtl::OUString SwXDrawPage::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXDrawPage");
}
/* -----------------------------06.04.00 13:14--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXDrawPage::supportsService(const rtl::OUString& rServiceName) throw( uno::RuntimeException )
{
    return C2U("com.sun.star.drawing.GenericDrawPage") == rServiceName;
}
/* -----------------------------06.04.00 13:14--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< rtl::OUString > SwXDrawPage::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aRet(1);
    rtl::OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.drawing.GenericDrawPage");
    return aRet;
}
/*-- 22.01.99 11:22:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDrawPage::SwXDrawPage(SwDoc* pDc) :
    pDoc(pDc),
    pDrawPage(0)
{
}
/*-- 22.01.99 11:22:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDrawPage::~SwXDrawPage()
{
    if(xPageAgg.is())
    {
        uno::Reference< uno::XInterface >  xInt;
        xPageAgg->setDelegator(xInt);
    }
}
/* -----------------------------15.06.00 15:00--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXDrawPage::queryInterface( const uno::Type& aType )
                                                throw( uno::RuntimeException )
{
    uno::Any aRet = SwXDrawPageBaseClass::queryInterface(aType);
    if(!aRet.hasValue())
    {
        aRet = GetSvxPage()->queryAggregation(aType);
    }
    return aRet;
}
/* -----------------------------15.06.00 15:01--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SwXDrawPage::getTypes() throw( uno::RuntimeException )
{
    uno::Sequence< uno::Type > aPageTypes = SwXDrawPageBaseClass::getTypes();
    uno::Sequence< uno::Type > aSvxTypes = GetSvxPage()->getTypes();

    long nIndex = aPageTypes.getLength();
    aPageTypes.realloc(aPageTypes.getLength() + aSvxTypes.getLength() + 1);

    uno::Type* pPageTypes = aPageTypes.getArray();
    const uno::Type* pSvxTypes = aSvxTypes.getConstArray();
    long nPos;
    for(nPos = 0; nPos < aSvxTypes.getLength(); nPos++)
    {
        pPageTypes[nIndex++] = pSvxTypes[nPos];
    }
    pPageTypes[nIndex] = ::getCppuType((uno::Reference<form::XFormsSupplier2>*)0);
    return aPageTypes;
}
/*-- 22.01.99 11:33:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXDrawPage::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    if(!pDoc->GetDrawModel())
        return 0;
    else
    {
        ((SwXDrawPage*)this)->GetSvxPage();
        return pDrawPage->getCount();
    }
}
/*-- 22.01.99 11:33:46---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXDrawPage::getByIndex(sal_Int32 nIndex)
        throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException,
               uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    if(!pDoc->GetDrawModel())
    {
        throw lang::IndexOutOfBoundsException();
    }
    else
    {
         ((SwXDrawPage*)this)->GetSvxPage();
         return pDrawPage->getByIndex( nIndex);
    }
    return uno::Any();
}
/* -----------------22.01.99 13:13-------------------
 *
 * --------------------------------------------------*/
uno::Type  SwXDrawPage::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((const uno::Reference<drawing::XShape>*)0);
}
/* -----------------22.01.99 13:13-------------------
 *
 * --------------------------------------------------*/
sal_Bool SwXDrawPage::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    if(!pDoc->GetDrawModel())
        return sal_False;
    else
        return ((SwXDrawPage*)this)->GetSvxPage()->hasElements();
}

/* -----------------22.01.99 12:42-------------------
 *
 * --------------------------------------------------*/
void SwXDrawPage::add(const uno::Reference< drawing::XShape > & xShape)
    throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShape, uno::UNO_QUERY);
    SwXShape* pShape = 0;
    SvxShape* pSvxShape = 0;
    if(xShapeTunnel.is())
    {
        pShape = (SwXShape*)xShapeTunnel->getSomething(SwXShape::getUnoTunnelId());
        pSvxShape = (SvxShape*)xShapeTunnel->getSomething(SvxShape::getUnoTunnelId());
    }

    if(!pShape || pShape->GetRegisteredIn() || !pShape->m_bDescriptor )
    {
        uno::RuntimeException aExcept;
        if(pShape)
            aExcept.Message = C2U("object already inserted");
        else
            aExcept.Message = C2U("illegal object");
        throw aExcept;
    }
    GetSvxPage()->add(xShape);

    uno::Reference< uno::XAggregation > 	xAgg = pShape->GetAggregationInterface();

    DBG_ASSERT(pSvxShape, "warum gibt es hier kein SvxShape?")
    //diese Position ist auf jeden Fall in 1/100 mm
    awt::Point aMM100Pos(pSvxShape->getPosition());

    //jetzt noch die Properties aus dem SwShapeDescriptor_Impl auswerten
    SwShapeDescriptor_Impl* pDesc = pShape->GetDescImpl();

    SfxItemSet aSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                                        RES_FRMATR_END-1 );
    SwFmtAnchor aAnchor( FLY_IN_CNTNT );
    sal_Bool bOpaque = sal_False;
    if( pDesc )
    {
        if(pDesc->GetSurround())
            aSet.Put( *pDesc->GetSurround());
        //die Items sind schon in Twip gesetzt
        if(pDesc->GetLRSpace())
        {
            aSet.Put(*pDesc->GetLRSpace());
        }
        if(pDesc->GetULSpace())
        {
            aSet.Put(*pDesc->GetULSpace());
        }
        if(pDesc->GetAnchor())
            aAnchor = *pDesc->GetAnchor();

        // --> OD 2004-08-18 #i32349# - if no horizontal position exists, create one
        if ( !pDesc->GetHOrient() )
        {
            SwFmtHoriOrient* pHori = pDesc->GetHOrient( sal_True );
            SwTwips nHoriPos = MM100_TO_TWIP(aMM100Pos.X);
            pHori->SetPos( nHoriPos );
        }
        // <--
        {
            if(pDesc->GetHOrient()->GetHoriOrient() == HORI_NONE)
                aMM100Pos.X = TWIP_TO_MM100(pDesc->GetHOrient()->GetPos());
            aSet.Put( *pDesc->GetHOrient() );
        }
        // --> OD 2004-08-18 #i32349# - if no vertical position exists, create one
        if ( !pDesc->GetVOrient() )
        {
            SwFmtVertOrient* pVert = pDesc->GetVOrient( sal_True );
            SwTwips nVertPos = MM100_TO_TWIP(aMM100Pos.Y);
            pVert->SetPos( nVertPos );
        }
        // <--
        {
            if(pDesc->GetVOrient()->GetVertOrient() == VERT_NONE)
                aMM100Pos.Y = TWIP_TO_MM100(pDesc->GetVOrient()->GetPos());
            aSet.Put( *pDesc->GetVOrient() );
        }

        if(pDesc->GetSurround())
            aSet.Put( *pDesc->GetSurround());
        bOpaque = pDesc->IsOpaque();

        // OD 2004-04-22 #i26791#
        if ( pDesc->GetFollowTextFlow() )
        {
            aSet.Put( *pDesc->GetFollowTextFlow() );
        }

        // OD 2004-05-05 #i28701#
        if ( pDesc->GetWrapInfluenceOnObjPos() )
        {
            aSet.Put( *pDesc->GetWrapInfluenceOnObjPos() );
        }
    }

    pSvxShape->setPosition(aMM100Pos);
    SdrObject* pObj = pSvxShape->GetSdrObject();
    // OD 25.06.2003 #108784# - set layer of new drawing object to corresponding
    // invisible layer.
    if(FmFormInventor != pObj->GetObjInventor())
        pObj->SetLayer( bOpaque ? pDoc->GetInvisibleHeavenId() : pDoc->GetInvisibleHellId() );
    else
        pObj->SetLayer(pDoc->GetInvisibleControlsId());

    SwPaM* pPam = new SwPaM(pDoc->GetNodes().GetEndOfContent());
    SwUnoInternalPaM* pInternalPam = 0;
    uno::Reference< text::XTextRange >  xRg;
    if( pDesc && (xRg = pDesc->GetTextRange()).is() )
    {
        pInternalPam = new SwUnoInternalPaM(*pDoc);
        if(SwXTextRange::XTextRangeToSwPaM(*pInternalPam, xRg))
        {
            if(FLY_AT_FLY == aAnchor.GetAnchorId() &&
                                !pInternalPam->GetNode()->FindFlyStartNode())
                        aAnchor.SetType(FLY_IN_CNTNT);
            else if(FLY_PAGE == aAnchor.GetAnchorId())
                aAnchor.SetAnchor(pInternalPam->Start());
        }
        else
            throw uno::RuntimeException();
    }
    else if( aAnchor.GetAnchorId() != FLY_PAGE && pDoc->GetRootFrm() )
    {
        SwCrsrMoveState aState( MV_SETONLYTEXT );
        Point aTmp(MM100_TO_TWIP(aMM100Pos.X), MM100_TO_TWIP(aMM100Pos.Y));
        pDoc->GetRootFrm()->GetCrsrOfst( pPam->GetPoint(), aTmp, &aState );
        aAnchor.SetAnchor( pPam->GetPoint() );

        // --> OD 2004-08-18 #i32349# - adjustment of vertical positioning
        // attributes no longer needed, because its already got a default.
    }
    else
    {
        aAnchor.SetType(FLY_PAGE);

        // --> OD 2004-08-18 #i32349# - adjustment of vertical positioning
        // attributes no longer needed, because its already got a default.
    }
    aSet.Put(aAnchor);
    SwPaM* pTemp = pInternalPam;
    if ( !pTemp )
        pTemp = pPam;
    UnoActionContext aAction(pDoc);
    pDoc->Insert( *pTemp, *pObj, &aSet );
    SwFrmFmt* pFmt = ::FindFrmFmt( pObj );
    if(pFmt)
        pFmt->Add(pShape);
    pShape->m_bDescriptor = sal_False;

    delete pPam;
    delete pInternalPam;
}
/* -----------------22.01.99 12:42-------------------
 *
 * --------------------------------------------------*/
void SwXDrawPage::remove(const uno::Reference< drawing::XShape > & xShape) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    uno::Reference<lang::XComponent> xComp(xShape, uno::UNO_QUERY);
    xComp->dispose();
}
/* -----------------17.02.99 10:38-------------------
 *
 * --------------------------------------------------*/
uno::Reference< drawing::XShapeGroup >  SwXDrawPage::group(const uno::Reference< drawing::XShapes > & xShapes) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc || !xShapes.is())
        throw uno::RuntimeException();
    uno::Reference< drawing::XShapeGroup >  xRet;
    if(xPageAgg.is())
    {

        SwFmDrawPage* pPage = GetSvxPage();
        if(pPage)//kann das auch Null sein?
        {
            //markieren und MarkList zurueckgeben
            const SdrMarkList& rMarkList = pPage->PreGroup(xShapes);
            if ( rMarkList.GetMarkCount() > 1 )
            {
                sal_Bool bFlyInCnt = sal_False;
                for ( sal_uInt16 i = 0; !bFlyInCnt && i < rMarkList.GetMarkCount(); ++i )
                {
                    const SdrObject *pObj = rMarkList.GetMark( i )->GetObj();
                    if ( FLY_IN_CNTNT == ::FindFrmFmt( (SdrObject*)pObj )->GetAnchor().GetAnchorId() )
                        bFlyInCnt = sal_True;
                }
                if( bFlyInCnt )
                    throw uno::RuntimeException();
                if( !bFlyInCnt )
                {
                    UnoActionContext aContext(pDoc);
                    pDoc->StartUndo( UNDO_START );

                    SwDrawContact* pContact = pDoc->GroupSelection( *pPage->GetDrawView() );
                    pDoc->ChgAnchor( pPage->GetDrawView()->GetMarkedObjectList(), FLY_AT_CNTNT/*int eAnchorId*/,
                        sal_True, sal_False );

                    pPage->GetDrawView()->UnmarkAll();
                    if(pContact)
                    {
                        uno::Reference< uno::XInterface >  xInt = pPage->GetInterface( pContact->GetMaster() );
                        xRet = uno::Reference< drawing::XShapeGroup >(xInt, uno::UNO_QUERY);
                    }
                    pDoc->EndUndo( UNDO_END );
                }
            }
            pPage->RemovePageView();
        }
    }
    return xRet;
}
/* -----------------17.02.99 10:38-------------------
 *
 * --------------------------------------------------*/
void SwXDrawPage::ungroup(const uno::Reference< drawing::XShapeGroup > & xShapeGroup) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    if(xPageAgg.is())
    {
        SwFmDrawPage* pPage = GetSvxPage();
        if(pPage)//kann das auch Null sein?
        {
            pPage->PreUnGroup(xShapeGroup);
            UnoActionContext aContext(pDoc);
            pDoc->StartUndo( UNDO_START );

            pDoc->UnGroupSelection( *pPage->GetDrawView() );
            pDoc->ChgAnchor( pPage->GetDrawView()->GetMarkedObjectList(), FLY_AT_CNTNT/*int eAnchorId*/,
                        sal_True, sal_False );
            pDoc->EndUndo( UNDO_END );
        }
        pPage->RemovePageView();
    }
}
/* -----------------05.05.98 17:05-------------------
 *
 * --------------------------------------------------*/
SwFmDrawPage* 	SwXDrawPage::GetSvxPage()
{
    if(!xPageAgg.is() && pDoc)
    {
        vos::OGuard  aGuard(Application::GetSolarMutex());
        // --> OD 2005-08-08 #i52858# - method name changed
        SdrModel* pModel = pDoc->GetOrCreateDrawModel();
        // <--
        SdrPage* pPage = pModel->GetPage( 0 );

        {
            // waehrend des queryInterface braucht man ein Ref auf das
            // Objekt, sonst wird es geloescht.
            pDrawPage = new SwFmDrawPage(pPage);
            uno::Reference< drawing::XDrawPage >  xPage = pDrawPage;
            uno::Any aAgg = xPage->queryInterface(::getCppuType((uno::Reference< uno::XAggregation >*)0));
            if(aAgg.getValueType() == ::getCppuType((uno::Reference< uno::XAggregation >*)0))
                xPageAgg = *(uno::Reference< uno::XAggregation >*)aAgg.getValue();
        }
        if( xPageAgg.is() )
            xPageAgg->setDelegator( (cppu::OWeakObject*)this );
    }
    return pDrawPage;
}
/****************************************************************************

****************************************************************************/
TYPEINIT1(SwXShape, SwClient);
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXShape::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXShape::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }

    if( xShapeAgg.is() )
    {
        const uno::Type& rTunnelType = ::getCppuType((uno::Reference<lang::XUnoTunnel>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rTunnelType );
        if(aAgg.getValueType() == rTunnelType)
        {
            uno::Reference<lang::XUnoTunnel> xAggTunnel =
                    *(uno::Reference<lang::XUnoTunnel>*)aAgg.getValue();
            if(xAggTunnel.is())
                return xAggTunnel->getSomething(rId);
        }
    }
    return 0;
}
/* -----------------01.02.99 11:38-------------------
 *
 * --------------------------------------------------*/
SwXShape::SwXShape(uno::Reference< uno::XInterface > & xShape) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_SHAPE)),
    _pMap(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_SHAPE)),
    pImpl(new SwShapeDescriptor_Impl),
    pImplementationId(0),
    m_bDescriptor(sal_True)
{
    if(xShape.is())  // default Ctor
    {
        const uno::Type& rAggType = ::getCppuType((const uno::Reference< uno::XAggregation >*)0);
        //aAgg contains a reference of the SvxShape!
        {
            uno::Any aAgg = xShape->queryInterface(rAggType);
            if(aAgg.getValueType() == rAggType)
                xShapeAgg = *(uno::Reference< uno::XAggregation >*)aAgg.getValue();
            // --> OD 2004-07-23 #i31698#
            if ( xShapeAgg.is() )
            {
                xShapeAgg->queryAggregation( ::getCppuType((uno::Reference< drawing::XShape >*)0) ) >>= mxShape;
                ASSERT( mxShape.is(),
                        "<SwXShape::SwXShape(..)> - no XShape found at <xShapeAgg>" );
            }
            // <--
        }
        xShape = 0;
        m_refCount++;
        if( xShapeAgg.is() )
            xShapeAgg->setDelegator( (cppu::OWeakObject*)this );
        m_refCount--;

        uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShapeAgg, uno::UNO_QUERY);
        SvxShape* pShape = 0;
        if(xShapeTunnel.is())
            pShape = (SvxShape*)xShapeTunnel->getSomething(SvxShape::getUnoTunnelId());

        SdrObject* pObj = pShape ? pShape->GetSdrObject() : 0;
        if(pObj)
        {
            SwFrmFmt* pFmt = ::FindFrmFmt( pObj );
            if(pFmt)
                pFmt->Add(this);
        }
    }
}

/*-- 22.01.99 11:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXShape::~SwXShape()
{
    if (xShapeAgg.is())
    {
        uno::Reference< uno::XInterface >  xRef;
        xShapeAgg->setDelegator(xRef);
    }
    delete pImpl;
}
/* -----------------------------16.06.00 12:21--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXShape::queryInterface( const uno::Type& aType ) throw( uno::RuntimeException )
{
    uno::Any aRet = SwXShapeBaseClass::queryInterface(aType);
    if(!aRet.hasValue() && xShapeAgg.is())
        aRet = xShapeAgg->queryAggregation(aType);
    return aRet;
}
/* -----------------------------16.06.00 12:21--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SwXShape::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aRet = SwXShapeBaseClass::getTypes();
    if(xShapeAgg.is())
    {
        uno::Any aProv = xShapeAgg->queryAggregation(::getCppuType((uno::Reference< XTypeProvider >*)0));
        if(aProv.hasValue())
        {
            uno::Reference< XTypeProvider > xAggProv;
            aProv >>= xAggProv;
            uno::Sequence< uno::Type > aAggTypes = xAggProv->getTypes();
            const uno::Type* pAggTypes = aAggTypes.getConstArray();
            long nIndex = aRet.getLength();

            aRet.realloc(nIndex + aAggTypes.getLength());
            uno::Type* pBaseTypes = aRet.getArray();

            for(long i = 0; i < aAggTypes.getLength(); i++)
                pBaseTypes[nIndex++] = pAggTypes[i];
        }
    }
    return aRet;
}
/* -----------------------------04.04.01 07:37--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SwXShape::getImplementationId(  ) throw(uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    // do we need to compute the implementation id for this instance?
    if( !pImplementationId && xShapeAgg.is())
    {
        uno::Reference< XShape > xAggShape;
        xShapeAgg->queryAggregation( ::getCppuType((uno::Reference< XShape >*)0) ) >>= xAggShape;

        if( xAggShape.is() )
        {
            const rtl::OUString aShapeType( xAggShape->getShapeType() );
            // did we already compute an implementation id for the agregated shape type?
            SwShapeImplementationIdMap::iterator aIter( aImplementationIdMap.find(aShapeType ) );
            if( aIter == aImplementationIdMap.end() )
            {
                // we need to create a new implementation id for this
                // note: this memory is not free'd until application exists
                //		 but since we have a fixed set of shapetypes and the
                //		 memory will be reused this is ok.
                pImplementationId = new uno::Sequence< sal_Int8 >( 16 );
                rtl_createUuid( (sal_uInt8 *) pImplementationId->getArray(), 0, sal_True );
                aImplementationIdMap[ aShapeType ] = pImplementationId;
            }
            else
            {
                // use the already computed implementation id
                pImplementationId = (*aIter).second;
            }
        }
    }
    if( NULL == pImplementationId )
    {
        DBG_ERROR( "Could not create an implementation id for a SwXShape!" );
        return uno::Sequence< sal_Int8 > ();
    }
    else
    {
        return *pImplementationId;
    }
}
/*-- 22.01.99 11:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo >  SwXShape::getPropertySetInfo(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference< beans::XPropertySetInfo >  aRet;
    if(xShapeAgg.is())
    {
        const uno::Type& rPropSetType = ::getCppuType((const uno::Reference< beans::XPropertySet >*)0);
        uno::Any aPSet = xShapeAgg->queryAggregation( rPropSetType );
        if(aPSet.getValueType() == rPropSetType && aPSet.getValue())
        {
            uno::Reference< beans::XPropertySet >  xPrSet =
                    *(uno::Reference< beans::XPropertySet >*)aPSet.getValue();
            uno::Reference< beans::XPropertySetInfo >  xInfo = xPrSet->getPropertySetInfo();
            // PropertySetInfo verlaengern!
            const uno::Sequence<beans::Property> aPropSeq = xInfo->getProperties();
            aRet = new SfxExtItemPropertySetInfo( _pMap, aPropSeq );
        }
    }
    if(!aRet.is())
        aRet = new SfxItemPropertySetInfo( _pMap );
    return aRet;
}
/*-- 22.01.99 11:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::setPropertyValue(const rtl::OUString& rPropertyName, const uno::Any& aValue)
     throw( beans::UnknownPropertyException, beans::PropertyVetoException,
            lang::IllegalArgumentException, lang::WrappedTargetException,
            uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFrmFmt* 	pFmt = GetFrmFmt();
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                                _pMap, rPropertyName);
    if(xShapeAgg.is())
    {
        if(pMap)
        {
            if ( pMap->nFlags & beans::PropertyAttribute::READONLY)
                throw beans::PropertyVetoException ( rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
            //mit Layout kann der Anker umgesetzt werden, ohne dass sich die Position aendert
            if(pFmt)
            {
                SwAttrSet aSet(pFmt->GetAttrSet());
                SwDoc* pDoc = pFmt->GetDoc();
                if(RES_ANCHOR == pMap->nWID && MID_ANCHOR_ANCHORFRAME == pMap->nMemberId)
                {
                    sal_Bool bDone = sal_True;
                    uno::Reference<text::XTextFrame> xFrame;
                    if(aValue >>= xFrame)
                    {
                        uno::Reference<lang::XUnoTunnel> xTunnel(xFrame, uno::UNO_QUERY);
                        SwXFrame* pFrame = xTunnel.is() ?
                                (SwXFrame*)xTunnel->getSomething(SwXFrame::getUnoTunnelId()) : 0;
                        if(pFrame && pFrame->GetFrmFmt() &&
                            pFrame->GetFrmFmt()->GetDoc() == pDoc)
                        {
                            UnoActionContext aCtx(pDoc);
                            SfxItemSet aSet( pDoc->GetAttrPool(),
                                        RES_FRMATR_BEGIN, RES_FRMATR_END - 1 );
                            aSet.SetParent(&pFmt->GetAttrSet());
                            SwFmtAnchor aAnchor = (const SwFmtAnchor&)aSet.Get(pMap->nWID);
                            SwPosition aPos(*pFrame->GetFrmFmt()->GetCntnt().GetCntntIdx());
                            aAnchor.SetAnchor(&aPos);
                            aAnchor.SetType(FLY_AT_FLY);
                            aSet.Put(aAnchor);
                            pFmt->SetAttr(aSet);
                            bDone = sal_True;
                        }
                    }
                    if(!bDone)
                        throw lang::IllegalArgumentException();
                }
                else if(RES_OPAQUE == pMap->nWID)
                {
                    SvxShape* pSvxShape = GetSvxShape();
                    DBG_ASSERT(pSvxShape, "No SvxShape found!")
                    if(pSvxShape)
                    {
                        SdrObject* pObj = pSvxShape->GetSdrObject();
                        // OD 25.06.2003 #108784# - set layer of new drawing
                        // object to corresponding invisible layer.
                        bool bIsVisible = pDoc->IsVisibleLayerId( pObj->GetLayer() );
                        if(FmFormInventor != pObj->GetObjInventor())
                        {
                            pObj->SetLayer( *(sal_Bool*)aValue.getValue()
                                            ? ( bIsVisible ? pDoc->GetHeavenId() : pDoc->GetInvisibleHeavenId() )
                                            : ( bIsVisible ? pDoc->GetHellId() : pDoc->GetInvisibleHellId() ));
                        }
                        else
                        {
                            pObj->SetLayer( bIsVisible ? pDoc->GetControlsId() : pDoc->GetInvisibleControlsId());
                        }

                    }

                }
                // OD 2004-04-22 #i26791# - special handling for property FN_TEXT_RANGE
                else if ( FN_TEXT_RANGE == pMap->nWID )
                {
                    SwFmtAnchor aAnchor( static_cast<const SwFmtAnchor&>(aSet.Get( RES_ANCHOR )) );
                    if ( aAnchor.GetAnchorId() == FLY_PAGE )
                    {
                        // set property <TextRange> not valid for to-page anchored shapes
                        throw lang::IllegalArgumentException();
                    }
                    else
                    {
                        SwUnoInternalPaM* pInternalPam =
                                        new SwUnoInternalPaM( *(pFmt->GetDoc()) );
                        uno::Reference< text::XTextRange > xRg;
                        aValue >>= xRg;
                        if ( SwXTextRange::XTextRangeToSwPaM(*pInternalPam, xRg) )
                        {
                            aAnchor.SetAnchor( pInternalPam->GetPoint() );
                            aSet.Put(aAnchor);
                            pFmt->SetAttr(aSet);
                        }
                        else
                        {
                            throw uno::RuntimeException();
                        }
                        delete pInternalPam;
                    }
                }
                // --> OD 2004-08-06 #i28749#
                else if ( FN_SHAPE_POSITION_LAYOUT_DIR == pMap->nWID )
                {
                    sal_Int16 nPositionLayoutDir;
                    aValue >>= nPositionLayoutDir;
                    pFmt->SetPositionLayoutDir( nPositionLayoutDir );
                }
                // <--
                else if( pDoc->GetRootFrm() )
                {
                    UnoActionContext aCtx(pDoc);
                    if(RES_ANCHOR == pMap->nWID && MID_ANCHOR_ANCHORTYPE == pMap->nMemberId)
                    {
                        SdrObject* pObj = pFmt->FindSdrObject();
                        SdrMarkList aList;
                        SdrMark aMark(pObj);
                        aList.InsertEntry(aMark);
                        sal_Int32 nAnchor;
                        cppu::enum2int( nAnchor, aValue );
                        pDoc->ChgAnchor( aList, (RndStdIds)nAnchor,
                                                sal_False, sal_True );
                    }
                    else
                    {
                        aPropSet.setPropertyValue(*pMap, aValue, aSet);
                        pFmt->SetAttr(aSet);
                    }
                }
                else
                {
                    aPropSet.setPropertyValue(*pMap, aValue, aSet);
                    pFmt->SetAttr(aSet);
                }
            }
            else
            {
                SfxPoolItem* pItem = 0;
                switch(pMap->nWID)
                {
                    case RES_ANCHOR:
                        pItem = pImpl->GetAnchor(sal_True);
                    break;
                    case RES_HORI_ORIENT:
                        pItem = pImpl->GetHOrient(sal_True);
                    break;
                    case RES_VERT_ORIENT:
                        pItem = pImpl->GetVOrient(sal_True);
                    break;
                    case  RES_LR_SPACE:
                        pItem = pImpl->GetLRSpace(sal_True);
                    break;
                    case  RES_UL_SPACE:
                        pItem = pImpl->GetULSpace(sal_True);
                    break;
                    case  RES_SURROUND:
                        pItem = pImpl->GetSurround(sal_True);
                    break;
                    case  FN_TEXT_RANGE:
                    {
                        const uno::Type rTextRangeType =
                            ::getCppuType((uno::Reference< text::XTextRange>*)0);
                        if(aValue.getValueType() == rTextRangeType)
                        {
                            uno::Reference< text::XTextRange > & rRange = pImpl->GetTextRange();
                            rRange = *(uno::Reference< text::XTextRange > *)aValue.getValue();
                        }
                    }
                    break;
                    case RES_OPAQUE :
                        pImpl->SetOpaque(*(sal_Bool*)aValue.getValue());
                    break;
                    // OD 2004-04-22 #i26791#
                    case RES_FOLLOW_TEXT_FLOW:
                    {
                        pItem = pImpl->GetFollowTextFlow( sal_True );
                    }
                    break;
                    // OD 2004-05-05 #i28701#
                    case RES_WRAP_INFLUENCE_ON_OBJPOS:
                    {
                        pItem = pImpl->GetWrapInfluenceOnObjPos( sal_True );
                    }
                    break;
                    // --> OD 2004-08-06 #i28749#
                    case FN_SHAPE_POSITION_LAYOUT_DIR :
                    {
                        sal_Int16 nPositionLayoutDir;
                        aValue >>= nPositionLayoutDir;
                        pImpl->SetPositionLayoutDir( nPositionLayoutDir );
                    }
                    break;
                    // <--
                }
                if(pItem)
                    ((SfxPoolItem*)pItem)->PutValue(aValue, pMap->nMemberId);
            }
        }
        else
        {
            uno::Reference< beans::XPropertySet >  xPrSet;
            const uno::Type& rPSetType =
                ::getCppuType((const uno::Reference< beans::XPropertySet >*)0);
            uno::Any aPSet = xShapeAgg->queryAggregation(rPSetType);
            if(aPSet.getValueType() != rPSetType || !aPSet.getValue())
                throw uno::RuntimeException();
            xPrSet = *(uno::Reference< beans::XPropertySet >*)aPSet.getValue();
            // --> OD 2004-08-05 #i31698# - setting the caption point of a
            // caption object doesn't have to change the object position.
            // Thus, keep the position, before the caption point is set and
            // restore it afterwards.
            awt::Point aKeepedPosition( 0, 0 );
            if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CaptionPoint"))) &&
                 getShapeType().equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.CaptionShape"))) )
            {
                    aKeepedPosition = getPosition();
            }
            // <--
            if( pFmt && pFmt->GetDoc()->GetRootFrm() )
            {
                UnoActionContext aCtx(pFmt->GetDoc());
                xPrSet->setPropertyValue(rPropertyName, aValue);
            }
            else
                xPrSet->setPropertyValue(rPropertyName, aValue);
            // --> OD 2004-11-11 #i35007# - adjustment of the position
            // attributes, if the transformation is set, causes wrong alignments
            // and is no longer needed.
            // The position attributes are set, if the drawing object is added
            // to the draw page - see <SwXDrawPage::add(..)> -  and on its first
            // positioning - see <SwAnchoredDrawObject::MakeObjPos().
//            // --> OD 2004-07-28 #i31698# - additionally adjust the position
//            // properties of the shape, if the transformation is set and
//            // the shape isn't a group member.
//            if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Transformation"))) &&
//                 !_GetTopGroupObj() )
//            {
//                drawing::HomogenMatrix3 aMatrix;
//                aValue >>= aMatrix;
//                awt::Point aNewPos( basegfx::fround( aMatrix.Line1.Column3 ),
//                                    basegfx::fround( aMatrix.Line2.Column3 ) );
//                _AdjustPositionProperties( aNewPos );
//            }
            // --> OD 2004-08-05 #i31698# - restore object position, if caption
            // point is set.
            if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CaptionPoint"))) &&
                 getShapeType().equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.CaptionShape"))) )
            {
                setPosition( aKeepedPosition );
            }
        }
    }
}
/*-- 22.01.99 11:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXShape::getPropertyValue(const rtl::OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    SwFrmFmt* 	pFmt = GetFrmFmt();
    if(xShapeAgg.is())
    {
        const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                    _pMap, rPropertyName);
        if(pMap)
        {
            if(pFmt)
            {
                if(RES_OPAQUE == pMap->nWID)
                {
                    SvxShape* pSvxShape = GetSvxShape();
                    DBG_ASSERT(pSvxShape, "No SvxShape found!")
                    if(pSvxShape)
                    {
                        SdrObject* pObj = pSvxShape->GetSdrObject();
                        // OD 02.07.2003 #108784# - consider invisible layers
                        sal_Bool bOpaque =
                            ( pObj->GetLayer() != pFmt->GetDoc()->GetHellId() &&
                              pObj->GetLayer() != pFmt->GetDoc()->GetInvisibleHellId() );
                        aRet.setValue(&bOpaque, ::getBooleanCppuType());
                    }
                }
                else if(FN_ANCHOR_POSITION == pMap->nWID)
                {
                    SvxShape* pSvxShape = GetSvxShape();
                    DBG_ASSERT(pSvxShape, "No SvxShape found!")
                    if(pSvxShape)
                    {
                        SdrObject* pObj = pSvxShape->GetSdrObject();
                        Point aPt = pObj->GetAnchorPos();
                        awt::Point aPoint( TWIP_TO_MM100( aPt.X() ),
                                           TWIP_TO_MM100( aPt.Y() ) );
                        aRet.setValue(&aPoint, ::getCppuType( (::com::sun::star::awt::Point*)0 ));
                    }
                }
                // OD 2004-04-22 #i26791# - special handling for FN_TEXT_RANGE
                else if ( FN_TEXT_RANGE == pMap->nWID )
                {
                    const SwFmtAnchor aAnchor = pFmt->GetAnchor();
                    if ( aAnchor.GetAnchorId() == FLY_PAGE )
                    {
                        // return nothing, because property <TextRange> isn't
                        // valid for to-page anchored shapes
                        uno::Any aAny;
                        aRet = aAny;
                    }
                    else
                    {
                        if ( aAnchor.GetCntntAnchor() )
                        {
                            uno::Reference< text::XTextRange > xTextRange =
                                SwXTextRange::CreateTextRangeFromPosition(
                                                    pFmt->GetDoc(),
                                                    *aAnchor.GetCntntAnchor(),
                                                    0L );
                            aRet.setValue(&xTextRange, ::getCppuType((uno::Reference<text::XTextRange>*)0));
                        }
                        else
                        {
                            // return nothing
                            uno::Any aAny;
                            aRet = aAny;
                        }
                    }
                }
                // --> OD 2004-08-06 #i28749#
                else if ( FN_SHAPE_TRANSFORMATION_IN_HORI_L2R == pMap->nWID )
                {
                    // get property <::drawing::Shape::Transformation>
                    // without conversion to layout direction as below
                    aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Transformation")) );
                }
                else if ( FN_SHAPE_POSITION_LAYOUT_DIR == pMap->nWID )
                {
                    aRet <<= pFmt->GetPositionLayoutDir();
                }
                // <--
                // --> OD 2004-10-28 #i36248#
                else if ( FN_SHAPE_STARTPOSITION_IN_HORI_L2R == pMap->nWID )
                {
                    // get property <::drawing::Shape::StartPosition>
                    // without conversion to layout direction as below
                    aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition")) );
                }
                else if ( FN_SHAPE_ENDPOSITION_IN_HORI_L2R == pMap->nWID )
                {
                    // get property <::drawing::Shape::EndPosition>
                    // without conversion to layout direction as below
                    aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition")) );
                }
                // <--
                else
                {
                    const SwAttrSet& rSet = pFmt->GetAttrSet();
                    aRet = aPropSet.getPropertyValue(*pMap, rSet);
                }
            }
            else
            {
                SfxPoolItem* pItem = 0;
                switch(pMap->nWID)
                {
                    case RES_ANCHOR:
                        pItem = pImpl->GetAnchor();
                    break;
                    case RES_HORI_ORIENT:
                        pItem = pImpl->GetHOrient();
                    break;
                    case RES_VERT_ORIENT:
                        pItem = pImpl->GetVOrient();
                    break;
                    case  RES_LR_SPACE:
                        pItem = pImpl->GetLRSpace();
                    break;
                    case  RES_UL_SPACE:
                        pItem = pImpl->GetULSpace();
                    break;
                    case  RES_SURROUND:
                        pItem = pImpl->GetSurround();
                    break;
                    case FN_TEXT_RANGE :
                        aRet.setValue(&pImpl->GetTextRange(), ::getCppuType((uno::Reference<text::XTextRange>*)0));
                    break;
                    case RES_OPAQUE :
                        aRet.setValue(&pImpl->GetOpaque(), ::getBooleanCppuType());
                    break;
                    case FN_ANCHOR_POSITION :
                    {
                        awt::Point aPoint;
                        aRet.setValue(&aPoint, ::getCppuType( (::com::sun::star::awt::Point*)0 ));
                    }
                    break;
                    // OD 2004-04-22 #i26791#
                    case RES_FOLLOW_TEXT_FLOW :
                    {
                        pItem = pImpl->GetFollowTextFlow();
                    }
                    break;
                    // OD 2004-05-05 #i28701#
                    case RES_WRAP_INFLUENCE_ON_OBJPOS:
                    {
                        pItem = pImpl->GetWrapInfluenceOnObjPos();
                    }
                    break;
                    // --> OD 2004-08-06 #i28749#
                    case FN_SHAPE_TRANSFORMATION_IN_HORI_L2R:
                    {
                        // get property <::drawing::Shape::Transformation>
                        // without conversion to layout direction as below
                        aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Transformation")) );
                    }
                    break;
                    case FN_SHAPE_POSITION_LAYOUT_DIR:
                    {
                        aRet <<= pImpl->GetPositionLayoutDir();
                    }
                    break;
                    // <--
                    // --> OD 2004-08-06 #i36248#
                    case FN_SHAPE_STARTPOSITION_IN_HORI_L2R:
                    {
                        // get property <::drawing::Shape::StartPosition>
                        // without conversion to layout direction as below
                        aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition")) );
                    }
                    break;
                    case FN_SHAPE_ENDPOSITION_IN_HORI_L2R:
                    {
                        // get property <::drawing::Shape::StartPosition>
                        // without conversion to layout direction as below
                        aRet = _getPropAtAggrObj( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition")) );
                    }
                    break;
                    // <--
                }
                if(pItem)
                    pItem->QueryValue(aRet, pMap->nMemberId);
            }
        }
        else
        {
            aRet = _getPropAtAggrObj( rPropertyName );

            // --> OD 2004-07-28 #i31698# - convert the position (translation)
            // of the drawing object in the transformation
            if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Transformation"))) )
            {
                drawing::HomogenMatrix3 aMatrix;
                aRet >>= aMatrix;
                aRet <<= _ConvertTransformationToLayoutDir( aMatrix );
            }
            // <--
            // --> OD 2004-10-28 #i36248#
            else if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition"))) )
            {
                awt::Point aStartPos;
                aRet >>= aStartPos;
                aRet <<= _ConvertStartPosToLayoutDir( aStartPos );
            }
            else if ( rPropertyName.equals(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition"))) )
            {
                awt::Point aEndPos;
                aRet >>= aEndPos;
                aRet <<= _ConvertEndPosToLayoutDir( aEndPos );
            }
            // <--
        }
    }
    return aRet;
}

/** method to get property from aggregation object

    OD 2004-10-28 #i36248#

    @author OD
*/
uno::Any SwXShape::_getPropAtAggrObj( const ::rtl::OUString& _rPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    uno::Any aRet;

    uno::Reference< beans::XPropertySet >  xPrSet;
    const uno::Type& rPSetType =
                ::getCppuType((const uno::Reference< beans::XPropertySet >*)0);
    uno::Any aPSet = xShapeAgg->queryAggregation(rPSetType);
    if ( aPSet.getValueType() != rPSetType || !aPSet.getValue() )
    {
        throw uno::RuntimeException();
    }
    xPrSet = *(uno::Reference< beans::XPropertySet >*)aPSet.getValue();
    aRet = xPrSet->getPropertyValue( _rPropertyName );

    return aRet;
}


/* -----------------------------02.11.00 09:41--------------------------------

 ---------------------------------------------------------------------------*/
beans::PropertyState SwXShape::getPropertyState( const rtl::OUString& rPropertyName )
    throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Sequence< rtl::OUString > aNames(1);
    rtl::OUString* pStrings = aNames.getArray();
    pStrings[0] = rPropertyName;
    uno::Sequence< beans::PropertyState > aStates = getPropertyStates(aNames);
    return aStates.getConstArray()[0];
}
/* -----------------------------02.11.00 09:41--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< beans::PropertyState > SwXShape::getPropertyStates(
    const uno::Sequence< rtl::OUString >& aPropertyNames )
        throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFrmFmt* 	pFmt = GetFrmFmt();
    uno::Sequence< beans::PropertyState > aRet(aPropertyNames.getLength());
    if(xShapeAgg.is())
    {
        SvxShape* pSvxShape = GetSvxShape();
        sal_Bool bGroupMember = sal_False;
        sal_Bool bFormControl = sal_False;
        SdrObject* pObject = pSvxShape->GetSdrObject();
        if(pObject)
        {
            bGroupMember = pObject->GetUpGroup() != 0;
            bFormControl = pObject->GetObjInventor() == FmFormInventor;
        }
        const rtl::OUString* pNames = aPropertyNames.getConstArray();
        beans::PropertyState* pRet = aRet.getArray();
        uno::Reference< XPropertyState >  xShapePrState;
        for(sal_Int32 nProperty = 0; nProperty < aPropertyNames.getLength(); nProperty++)
        {
            const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                        _pMap, pNames[nProperty]);
            if(pMap)
            {
                if(RES_OPAQUE == pMap->nWID)
                    pRet[nProperty] = bFormControl ?
                        beans::PropertyState_DEFAULT_VALUE : beans::PropertyState_DIRECT_VALUE;
                else if(FN_ANCHOR_POSITION == pMap->nWID)
                    pRet[nProperty] = beans::PropertyState_DIRECT_VALUE;
                else if(FN_TEXT_RANGE == pMap->nWID)
                    pRet[nProperty] = beans::PropertyState_DIRECT_VALUE;
                else if(bGroupMember)
                    pRet[nProperty] = beans::PropertyState_DEFAULT_VALUE;
                else if(pFmt)
                {
                    const SwAttrSet& rSet = pFmt->GetAttrSet();
                    SfxItemState eItemState = rSet.GetItemState(pMap->nWID, FALSE);

                    if(SFX_ITEM_SET == eItemState)
                        pRet[nProperty] = beans::PropertyState_DIRECT_VALUE;
                    else if(SFX_ITEM_DEFAULT == eItemState)
                        pRet[nProperty] = beans::PropertyState_DEFAULT_VALUE;
                    else
                        pRet[nProperty] = beans::PropertyState_AMBIGUOUS_VALUE;
                }
                else
                {
                    SfxPoolItem* pItem = 0;
                    switch(pMap->nWID)
                    {
                        case RES_ANCHOR:
                            pItem = pImpl->GetAnchor();
                        break;
                        case RES_HORI_ORIENT:
                            pItem = pImpl->GetHOrient();
                        break;
                        case RES_VERT_ORIENT:
                            pItem = pImpl->GetVOrient();
                        break;
                        case  RES_LR_SPACE:
                            pItem = pImpl->GetLRSpace();
                        break;
                        case  RES_UL_SPACE:
                            pItem = pImpl->GetULSpace();
                        break;
                        case  RES_SURROUND:
                            pItem = pImpl->GetSurround();
                        break;
                        // OD 2004-05-05 #i28701#
                        case RES_WRAP_INFLUENCE_ON_OBJPOS:
                        {
                            pItem = pImpl->GetWrapInfluenceOnObjPos();
                        }
                        break;
                    }
                    if(pItem)
                        pRet[nProperty] = beans::PropertyState_DIRECT_VALUE;
                    else
                        pRet[nProperty] = beans::PropertyState_DEFAULT_VALUE;
                }
            }
            else
            {
                if(!xShapePrState.is())
                {
                    const uno::Type& rPStateType = ::getCppuType((uno::Reference< XPropertyState >*)0);
                    uno::Any aPState = xShapeAgg->queryAggregation(rPStateType);
                    if(aPState.getValueType() != rPStateType || !aPState.getValue())
                        throw uno::RuntimeException();
                    xShapePrState = *(uno::Reference< XPropertyState >*)aPState.getValue();
                }
                pRet[nProperty] = xShapePrState->getPropertyState(pNames[nProperty]);
            }
        }
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/* -----------------------------02.11.00 09:41--------------------------------

 ---------------------------------------------------------------------------*/
void SwXShape::setPropertyToDefault( const rtl::OUString& rPropertyName )
    throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFrmFmt* 	pFmt = GetFrmFmt();
    if(xShapeAgg.is())
    {
        const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                        _pMap, rPropertyName);
        if(pMap)
        {
            if ( pMap->nFlags & beans::PropertyAttribute::READONLY)
                throw uno::RuntimeException( rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
            if(pFmt)
            {
                const SfxItemSet& rSet = pFmt->GetAttrSet();
                SfxItemSet aSet(pFmt->GetDoc()->GetAttrPool(), pMap->nWID, pMap->nWID);
                aSet.SetParent(&rSet);
                aSet.ClearItem(pMap->nWID);
                pFmt->GetDoc()->SetAttr(aSet, *pFmt);
            }
            else
            {
                switch(pMap->nWID)
                {
                    case RES_ANCHOR:		pImpl->RemoveAnchor(); 	break;
                    case RES_HORI_ORIENT: 	pImpl->RemoveHOrient(); break;
                    case RES_VERT_ORIENT:	pImpl->RemoveVOrient();	break;
                    case  RES_LR_SPACE:		pImpl->RemoveLRSpace();	break;
                    case  RES_UL_SPACE:		pImpl->RemoveULSpace();	break;
                    case  RES_SURROUND:		pImpl->RemoveSurround();break;
                    case RES_OPAQUE :       pImpl->SetOpaque(sal_False);  break;
                    case FN_TEXT_RANGE :
                    break;
                    // OD 2004-04-22 #i26791#
                    case RES_FOLLOW_TEXT_FLOW:
                    {
                        pImpl->RemoveFollowTextFlow();
                    }
                    break;
                    // OD 2004-05-05 #i28701#
                    case RES_WRAP_INFLUENCE_ON_OBJPOS:
                    {
                        pImpl->RemoveWrapInfluenceOnObjPos();
                    }
                    break;
                }
            }
        }
        else
        {
            const uno::Type& rPStateType = ::getCppuType((uno::Reference< XPropertyState >*)0);
            uno::Any aPState = xShapeAgg->queryAggregation(rPStateType);
            if(aPState.getValueType() != rPStateType || !aPState.getValue())
                throw uno::RuntimeException();
            uno::Reference< XPropertyState > xShapePrState = *(uno::Reference< XPropertyState >*)aPState.getValue();
            xShapePrState->setPropertyToDefault( rPropertyName );
        }
    }
    else
        throw uno::RuntimeException();
}
/* -----------------------------02.11.00 09:41--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXShape::getPropertyDefault( const rtl::OUString& rPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFrmFmt* 	pFmt = GetFrmFmt();
    uno::Any aRet;
    if(xShapeAgg.is())
    {
        const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                        _pMap, rPropertyName);
        if(pMap)
        {
            if(pMap->nWID < RES_FRMATR_END && pFmt)
            {
                const SfxPoolItem& rDefItem =
                    pFmt->GetDoc()->GetAttrPool().GetDefaultItem(pMap->nWID);
                rDefItem.QueryValue(aRet, pMap->nMemberId);
            }
            else
                throw uno::RuntimeException();
        }
        else
        {
            const uno::Type& rPStateType = ::getCppuType((uno::Reference< XPropertyState >*)0);
            uno::Any aPState = xShapeAgg->queryAggregation(rPStateType);
            if(aPState.getValueType() != rPStateType || !aPState.getValue())
                throw uno::RuntimeException();
            uno::Reference< XPropertyState > xShapePrState = *(uno::Reference< XPropertyState >*)aPState.getValue();
            xShapePrState->getPropertyDefault( rPropertyName );
        }
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 22.01.99 11:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::addPropertyChangeListener(
    const rtl::OUString& PropertyName,
    const uno::Reference< beans::XPropertyChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 22.01.99 11:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::removePropertyChangeListener(
    const rtl::OUString& PropertyName,
    const uno::Reference< beans::XPropertyChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 22.01.99 11:42:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::addVetoableChangeListener(
    const rtl::OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener > & aListener )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 22.01.99 11:42:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::removeVetoableChangeListener(
    const rtl::OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException,
           uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 22.01.99 11:42:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXShape::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}
/* -----------------14.04.99 13:02-------------------
 *
 * --------------------------------------------------*/
void SwXShape::attach(const uno::Reference< text::XTextRange > & xTextRange)
    throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());

    // get access to SwDoc
    // (see also SwXTextRange::XTextRangeToSwPaM)
    SwDoc*      pDoc = 0;
    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    if(xRangeTunnel.is())
    {
        SwXTextRange* pRange = 0;
        OTextCursorHelper* pCursor = 0;
        SwXTextPortion* pPortion = 0;
        SwXText* pText = 0;

        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                SwXTextRange::getUnoTunnelId());
        pText = (SwXText*)xRangeTunnel->getSomething(
                                SwXText::getUnoTunnelId());
        pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                OTextCursorHelper::getUnoTunnelId());
        pPortion = (SwXTextPortion*)xRangeTunnel->getSomething(
                                SwXTextPortion::getUnoTunnelId());

        if (pRange)
            pDoc = pRange->GetDoc();
        else if (!pDoc && pText)
            pDoc = pText->GetDoc();
        else if (!pDoc && pCursor)
            pDoc = pCursor->GetDoc();
        else if ( !pDoc && pPortion && pPortion->GetCrsr() )
            pDoc = pPortion->GetCrsr()->GetDoc();

    }

    if(!pDoc)
        throw uno::RuntimeException();
    SwDocShell *pDocSh = pDoc->GetDocShell();
    if (pDocSh)
    {
        uno::Reference< frame::XModel > xModel;
        xModel = pDocSh->GetModel();
        uno::Reference< drawing::XDrawPageSupplier > xDPS(xModel, uno::UNO_QUERY);
        if (xDPS.is())
        {
            uno::Reference< drawing::XDrawPage > xDP( xDPS->getDrawPage() );
            if (xDP.is())
            {
                uno::Any aPos;
                aPos <<= xTextRange;
                setPropertyValue(S2U("TextRange"), aPos);
                uno::Reference< drawing::XShape > xTemp( (cppu::OWeakObject*) this, uno::UNO_QUERY );
                xDP->add( xTemp );
            }
        }
    }
}
/* -----------------14.04.99 13:02-------------------
 *
 * --------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXShape::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  aRef;
    SwFrmFmt* pFmt = GetFrmFmt();
    if(pFmt)
    {
        const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
        // return an anchor for non-page bound frames
        // and for page bound frames that have a page no == NULL and a content position
        if( rAnchor.GetAnchorId() != FLY_PAGE ||
            (rAnchor.GetCntntAnchor() && !rAnchor.GetPageNum()))
        {
            const SwPosition &rPos = *(pFmt->GetAnchor().GetCntntAnchor());
            aRef = SwXTextRange::CreateTextRangeFromPosition(pFmt->GetDoc(), rPos, 0);
        }
    }
    else
        aRef = pImpl->GetTextRange();
    return aRef;
}
/* -----------------14.04.99 13:02-------------------
 *
 * --------------------------------------------------*/
void SwXShape::dispose(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFrmFmt* pFmt = GetFrmFmt();
    if(pFmt)
    {
        // OD 10.07.2003 #110742# - determine correct <SdrObject>
        //SdrObject* pObj = pFmt->FindSdrObject();
        SdrObject* pObj = GetSvxShape()->GetSdrObject();
        // OD 10.07.2003 #110742# - safety assertion:
        // <pObj> must be the same as <pFmt->FindSdrObject()>, if <pObj> isn't
        // a 'virtual' drawing object.
        // OD 25.08.2003 #111713# - refine assertion for safety reason.
        // --> OD 2005-02-02 #119236# - correct assertion and refine it.
        ASSERT( !pObj ||
                pObj->ISA(SwDrawVirtObj) ||
                pObj->GetUpGroup() ||
                pObj == pFmt->FindSdrObject(),
                "<SwXShape::dispose(..) - different 'master' drawing objects!!" );
        // <--
        // OD 10.07.2003 #110742# - perform delete of draw frame format *not*
        // for 'virtual' drawing objects.
        // --> OD 2005-02-02 #119236# - no delete of draw format for members
        // of a group
        if ( pObj &&
             !pObj->ISA(SwDrawVirtObj) &&
             !pObj->GetUpGroup() &&
             pObj->IsInserted() )
        // <--
        {
            if( pFmt->GetAnchor().GetAnchorId() == FLY_IN_CNTNT )
            {
                const SwPosition &rPos = *(pFmt->GetAnchor().GetCntntAnchor());
                SwTxtNode *pTxtNode = rPos.nNode.GetNode().GetTxtNode();
                const xub_StrLen nIdx = rPos.nContent.GetIndex();
                pTxtNode->Delete( RES_TXTATR_FLYCNT, nIdx, nIdx );
            }
            else
                pFmt->GetDoc()->DelLayoutFmt( pFmt );
        }
    }
    if(xShapeAgg.is())
    {
        uno::Any aAgg(xShapeAgg->queryAggregation( ::getCppuType((uno::Reference<XComponent>*)0)));
        uno::Reference<XComponent> xComp;
        aAgg >>= xComp;
        if(xComp.is())
            xComp->dispose();
    }
}
/* -----------------14.04.99 13:02-------------------
 *
 * --------------------------------------------------*/
void SwXShape::addEventListener(
                    const uno::Reference< lang::XEventListener > & aListener)
                    throw( uno::RuntimeException )
{
    uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShapeAgg, uno::UNO_QUERY);
    SvxShape* pSvxShape = GetSvxShape();
    if(pSvxShape)
         pSvxShape->addEventListener(aListener);
}
/* -----------------14.04.99 13:02-------------------
 *
 * --------------------------------------------------*/
void SwXShape::removeEventListener(
                    const uno::Reference< lang::XEventListener > & aListener)
                    throw( uno::RuntimeException )
{
    SvxShape* pSvxShape = GetSvxShape();
    if(pSvxShape)
        pSvxShape->removeEventListener(aListener);
}
/* -----------------03.06.99 08:53-------------------
 *
 * --------------------------------------------------*/
rtl::OUString SwXShape::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXShape");
}
/* -----------------03.06.99 08:53-------------------
 *
 * --------------------------------------------------*/
sal_Bool SwXShape::supportsService(const rtl::OUString& rServiceName) throw( uno::RuntimeException )
{
    sal_Bool bRet = sal_False;
    if(COMPARE_EQUAL == rServiceName.compareToAscii("com.sun.star.drawing.Shape"))
        bRet = sal_True;
    else if(xShapeAgg.is())
    {
        uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShapeAgg, uno::UNO_QUERY);
        SvxShape* pSvxShape = GetSvxShape();
        bRet = pSvxShape->supportsService(rServiceName);
    }
    return bRet;
}
/* -----------------03.06.99 08:53-------------------
 *
 * --------------------------------------------------*/
uno::Sequence< rtl::OUString > SwXShape::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aSeq;
    if(xShapeAgg.is())
    {
        uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShapeAgg, uno::UNO_QUERY);
        SvxShape* pSvxShape = GetSvxShape();
        if(pSvxShape)
            aSeq = pSvxShape->getSupportedServiceNames();
    }
    else
    {
        aSeq.realloc(1);
        aSeq.getArray()[0] = C2U("com.sun.star.drawing.Shape");
    }
    return aSeq;
}
/* -----------------------------15.03.00 14:54--------------------------------

 ---------------------------------------------------------------------------*/
SvxShape*	SwXShape::GetSvxShape()
{
    SvxShape* pSvxShape = 0;
    if(xShapeAgg.is())
    {
        uno::Reference< lang::XUnoTunnel > xShapeTunnel(xShapeAgg, uno::UNO_QUERY);
        if(xShapeTunnel.is())
            pSvxShape = (SvxShape*)xShapeTunnel->getSomething(SvxShape::getUnoTunnelId());
    }
    return pSvxShape;
}

// --> OD 2004-07-22 #i31698# -
// implementation of virtual methods from com::sun::star::drawing::XShape
awt::Point SAL_CALL SwXShape::getPosition() throw ( uno::RuntimeException )
{
    awt::Point aPos( _GetAttrPosition() );

    // handle group members
    SvxShape* pSvxShape = GetSvxShape();
    if ( pSvxShape )
    {
        SdrObject* pTopGroupObj = _GetTopGroupObj( pSvxShape );
        if ( pTopGroupObj )
        {
            // --> OD 2004-10-01 #i34750# - get attribute position of top group
            // shape and add offset between top group object and group member
            uno::Reference< drawing::XShape > xGroupShape =
                    uno::Reference< drawing::XShape >( pTopGroupObj->getUnoShape(),
                                                       uno::UNO_QUERY );
            aPos = xGroupShape->getPosition();
            // add offset between top group object and group member
            // to the determined attribute position
            // --> OD 2004-10-01 #i34750# - correction:
            // consider the layout direction
            const Rectangle aMemberObjRect = GetSvxShape()->GetSdrObject()->GetSnapRect();
            const Rectangle aGroupObjRect = pTopGroupObj->GetSnapRect();
            const SwFrmFmt::tLayoutDir eLayoutDir = GetFrmFmt()
                                                    ? GetFrmFmt()->GetLayoutDir()
                                                    : SwFrmFmt::HORI_L2R;
            awt::Point aOffset( 0, 0 );
            switch ( eLayoutDir )
            {
                case SwFrmFmt::HORI_L2R:
                {
                    aOffset.X = ( aMemberObjRect.Left() - aGroupObjRect.Left() );
                    aOffset.Y = ( aMemberObjRect.Top() - aGroupObjRect.Top() );
                }
                break;
                case SwFrmFmt::HORI_R2L:
                {
                    aOffset.X = ( aGroupObjRect.Right() - aMemberObjRect.Right() );
                    aOffset.Y = ( aMemberObjRect.Top() - aGroupObjRect.Top() );
                }
                break;
                case SwFrmFmt::VERT_R2L:
                {
                    aOffset.X = ( aMemberObjRect.Top() - aGroupObjRect.Top() );
                    aOffset.Y = ( aGroupObjRect.Right() - aMemberObjRect.Right() );
                }
                break;
                default:
                {
                    ASSERT( false,
                            "<SwXShape::getPosition()> - unsupported layout direction" );
                }
            }
            aOffset.X = TWIP_TO_MM100(aOffset.X);
            aOffset.Y = TWIP_TO_MM100(aOffset.Y);
            aPos.X += aOffset.X;
            aPos.Y += aOffset.Y;
            // <--
        }
    }

    return aPos;
}
void SAL_CALL SwXShape::setPosition( const awt::Point& aPosition )
                                                throw ( uno::RuntimeException )
{
    SdrObject* pTopGroupObj = _GetTopGroupObj();
    if ( !pTopGroupObj )
    {
        // --> OD 2005-02-10 #i37877# - no adjustment of position attributes,
        // if the position also has to be applied at the drawing object and
        // a contact object is already registered at the drawing object.
        bool bApplyPosAtDrawObj(false);
        bool bNoAdjustOfPosProp(false);
        // --> OD 2004-10-19 #i35798# - apply position also to drawing object,
        // if drawing object has no anchor position set.
        if ( mxShape.is() )
        {
            SvxShape* pSvxShape = GetSvxShape();
            if ( pSvxShape )
            {
                const SdrObject* pObj = pSvxShape->GetSdrObject();
                if ( pObj &&
                     pObj->GetAnchorPos().X() == 0 &&
                     pObj->GetAnchorPos().Y() == 0 )
                {
                    bApplyPosAtDrawObj = true;
                    if ( pObj->GetUserCall() &&
                         pObj->GetUserCall()->ISA(SwDrawContact) )
                    {
                        bNoAdjustOfPosProp = true;
                    }
                }
            }
        }
        // <--
        // shape isn't a group member. Thus, set positioning attributes
        if ( !bNoAdjustOfPosProp )
        {
            _AdjustPositionProperties( aPosition );
        }
        if ( bApplyPosAtDrawObj )
        {
            mxShape->setPosition( aPosition );
        }
        // <--
    }
    else if ( mxShape.is() )
    {
        // shape is a member of a group. Thus, set its position.
        awt::Point aNewPos( aPosition );
        // The given position is given in the according layout direction. Thus,
        // it has to be converted to a position in horizontal left-to-right
        // layout.
        // convert given absolute attribute position in layout direction into
        // position in horizontal left-to-right layout.
        {
            aNewPos = _ConvertPositionToHoriL2R( aNewPos, getSize() );
        }
        // Convert given absolute position in horizontal left-to-right
        // layout into relative position in horizontal left-to-right layout.
        uno::Reference< drawing::XShape > xGroupShape =
                uno::Reference< drawing::XShape >( pTopGroupObj->getUnoShape(),
                                                   uno::UNO_QUERY );
        {
            // --> OD 2004-09-29 #i34750# - correction:
            // use method <xGroupShape->getPosition()> to get the correct
            // position of the top group object.
            awt::Point aAttrPosInHoriL2R(
                    _ConvertPositionToHoriL2R( xGroupShape->getPosition(),
                                               xGroupShape->getSize() ) );
            // <--
            aNewPos.X -= aAttrPosInHoriL2R.X;
            aNewPos.Y -= aAttrPosInHoriL2R.Y;
        }
        // convert relative position in horizontal left-to-right layout into
        // absolute position in horizontal left-to-right layout
        {
            // --> OD 2004-10-01 #i34750# - correction:
            // use method <SvxShape->getPosition()> to get the correct
            // 'Drawing layer' position of the top group shape.
            uno::Reference< lang::XUnoTunnel > xGrpShapeTunnel(
                                                    pTopGroupObj->getUnoShape(),
                                                    uno::UNO_QUERY );
            SvxShape* pSvxGroupShape =
                (SvxShape*)xGrpShapeTunnel->getSomething(SvxShape::getUnoTunnelId());
            const awt::Point aGroupPos = pSvxGroupShape->getPosition();
            aNewPos.X += aGroupPos.X;
            aNewPos.Y += aGroupPos.Y;
            // <--
        }
        // set position
        mxShape->setPosition( aNewPos );
    }
}
awt::Size SAL_CALL SwXShape::getSize() throw ( uno::RuntimeException )
{
    awt::Size aSize;
    if ( mxShape.is() )
    {
        aSize = mxShape->getSize();
    }
    return aSize;
}
void SAL_CALL SwXShape::setSize( const awt::Size& aSize )
                                        throw ( beans::PropertyVetoException,
                                                uno::RuntimeException )
{
    if ( mxShape.is() )
    {
        mxShape->setSize( aSize );
    }
}
// <--
// --> OD 2004-07-22 #i31698# -
// implementation of virtual methods from com::sun::star::drawing::XShapeDescriptor
::rtl::OUString SAL_CALL SwXShape::getShapeType() throw ( uno::RuntimeException )
{
    ::rtl::OUString aType;
    if ( mxShape.is() )
    {
        aType = mxShape->getShapeType();
    }
    return aType;
}
// <--
/** method to determine top group object

    OD 2004-08-03 #i31698#

    @author OD
*/
SdrObject* SwXShape::_GetTopGroupObj( SvxShape* _pSvxShape )
{
    SdrObject* pTopGroupObj( 0L );

    SvxShape* pSvxShape = _pSvxShape ? _pSvxShape : GetSvxShape();
    if ( pSvxShape )
    {
        SdrObject* pSdrObj = pSvxShape->GetSdrObject();
        if ( pSdrObj && pSdrObj->GetUpGroup() )
        {
            pTopGroupObj = pSdrObj->GetUpGroup();
            while ( pTopGroupObj->GetUpGroup() )
            {
                pTopGroupObj = pTopGroupObj->GetUpGroup();
            }
        }
    }

    return pTopGroupObj;
}

/** method to determine position according to the positioning attributes

    OD 2004-08-03 #i31698#

    @author OD
*/
awt::Point SwXShape::_GetAttrPosition()
{
    awt::Point aAttrPos;

    uno::Any aHoriPos( getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HoriOrientPosition")) ) );
    aHoriPos >>= aAttrPos.X;
    uno::Any aVertPos( getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("VertOrientPosition")) ) );
    aVertPos >>= aAttrPos.Y;
    // --> OD 2004-10-19 #i35798# - fallback, if attribute position is (0,0)
    // and no anchor position is applied to the drawing object
    SvxShape* pSvxShape = GetSvxShape();
    if ( pSvxShape )
    {
        const SdrObject* pObj = pSvxShape->GetSdrObject();
        if ( pObj &&
             pObj->GetAnchorPos().X() == 0 &&
             pObj->GetAnchorPos().Y() == 0 &&
             aAttrPos.X == 0 && aAttrPos.Y == 0 )
        {
            const Rectangle aObjRect = pObj->GetSnapRect();
            aAttrPos.X = TWIP_TO_MM100(aObjRect.Left());
            aAttrPos.Y = TWIP_TO_MM100(aObjRect.Top());
        }
    }
    // <--
    // --> OD 2004-11-10 #i35007# - If drawing object is anchored as-character,
    // it's x-position isn't sensible. Thus, return the x-position as zero in this case.
    text::TextContentAnchorType eAnchorType =
                            text::TextContentAnchorType_AT_PARAGRAPH;
    {
        rtl::OUString sAnchorType( RTL_CONSTASCII_USTRINGPARAM( "AnchorType" ) );
        uno::Any aAny = getPropertyValue( sAnchorType );
        aAny >>= eAnchorType;
    }
    if ( eAnchorType == text::TextContentAnchorType_AS_CHARACTER )
    {
        aAttrPos.X = 0;
    }
    // <--

    return aAttrPos;
}

/** method to convert the position (translation) of the drawing object to
    the layout direction horizontal left-to-right.

    OD 2004-07-27 #i31698#

    @author OD
*/
awt::Point SwXShape::_ConvertPositionToHoriL2R( const awt::Point _aObjPos,
                                                const awt::Size _aObjSize )
{
    awt::Point aObjPosInHoriL2R( _aObjPos );

    SwFrmFmt* pFrmFmt = GetFrmFmt();
    if ( pFrmFmt )
    {
        SwFrmFmt::tLayoutDir eLayoutDir = pFrmFmt->GetLayoutDir();
        switch ( eLayoutDir )
        {
            case SwFrmFmt::HORI_L2R:
            {
                // nothing to do
            }
            break;
            case SwFrmFmt::HORI_R2L:
            {
                aObjPosInHoriL2R.X = -_aObjPos.X - _aObjSize.Width;
            }
            break;
            case SwFrmFmt::VERT_R2L:
            {
                aObjPosInHoriL2R.X = -_aObjPos.Y - _aObjSize.Width;
                aObjPosInHoriL2R.Y = _aObjPos.X;
            }
            break;
            default:
            {
                ASSERT( false,
                        "<SwXShape::_ConvertPositionToHoriL2R(..)> - unsupported layout direction" );
            }
        }
    }

    return aObjPosInHoriL2R;
}
/** method to convert the transformation of the drawing object to the layout
    direction, the drawing object is in

    OD 2004-07-27 #i31698#

    @author OD
*/
drawing::HomogenMatrix3 SwXShape::_ConvertTransformationToLayoutDir(
                                    drawing::HomogenMatrix3 _aMatrixInHoriL2R )
{
    drawing::HomogenMatrix3 aMatrix( _aMatrixInHoriL2R );

    // --> OD 2005-03-10 #i44334#, #i44681# - direct manipulation of the
    // tranformation structure isn't valid, if it contains rotation.
    SvxShape* pSvxShape = GetSvxShape();
    ASSERT( pSvxShape,
            "<SwXShape::_ConvertTransformationToLayoutDir(..)> - no SvxShape found!")
    if ( pSvxShape )
    {
        const SdrObject* pObj = pSvxShape->GetSdrObject();
        ASSERT( pObj,
                "<SwXShape::_ConvertTransformationToLayoutDir(..)> - no SdrObject found!")
        if ( pObj )
        {
            // get position of object in Writer coordinate system.
            awt::Point aPos( getPosition() );
            // get position of object in Drawing layer coordinate system
            const Point aTmpObjPos( pObj->GetSnapRect().TopLeft() );
            const awt::Point aObjPos(
                    TWIP_TO_MM100( aTmpObjPos.X() - pObj->GetAnchorPos().X() ),
                    TWIP_TO_MM100( aTmpObjPos.Y() - pObj->GetAnchorPos().Y() ) );
            // determine difference between these positions according to the
            // Writer coordinate system
            const awt::Point aTranslateDiff( aPos.X - aObjPos.X,
                                             aPos.Y - aObjPos.Y );
            // apply translation difference to transformation matrix.
            if ( aTranslateDiff.X != 0 || aTranslateDiff.Y != 0 )
            {
                Matrix3D aMatrix3D;
                aMatrix3D[0] = Point3D( aMatrix.Line1.Column1, aMatrix.Line1.Column2, aMatrix.Line1.Column3 );
                aMatrix3D[1] = Point3D( aMatrix.Line2.Column1, aMatrix.Line2.Column2, aMatrix.Line2.Column3 );
                aMatrix3D[2] = Point3D( aMatrix.Line3.Column1, aMatrix.Line3.Column2, aMatrix.Line3.Column3 );

                aMatrix3D.Translate( aTranslateDiff.X, aTranslateDiff.Y );

                aMatrix.Line1.Column1 = aMatrix3D[0].X();
                aMatrix.Line1.Column2 = aMatrix3D[0].Y();
                aMatrix.Line1.Column3 = aMatrix3D[0].W();
                aMatrix.Line2.Column1 = aMatrix3D[1].X();
                aMatrix.Line2.Column2 = aMatrix3D[1].Y();
                aMatrix.Line2.Column3 = aMatrix3D[1].W();
                aMatrix.Line3.Column1 = aMatrix3D[2].X();
                aMatrix.Line3.Column2 = aMatrix3D[2].Y();
                aMatrix.Line3.Column3 = aMatrix3D[2].W();
            }
        }
    }
    // <--

    return aMatrix;
}

/** method to adjust the positioning properties

    OD 2004-08-02 #i31698#

    @author OD
*/
void SwXShape::_AdjustPositionProperties( const awt::Point _aPosition )
{
    // handle x-position
    // --> OD 2004-11-10 #i35007# - no handling of x-position, if drawing
    // object is anchored as-character, because it doesn't make sense.
    text::TextContentAnchorType eAnchorType =
                            text::TextContentAnchorType_AT_PARAGRAPH;
    {
        rtl::OUString sAnchorType( RTL_CONSTASCII_USTRINGPARAM( "AnchorType" ) );
        uno::Any aAny = getPropertyValue( sAnchorType );
        aAny >>= eAnchorType;
    }
    if ( eAnchorType != text::TextContentAnchorType_AS_CHARACTER )
    // <--
    {
        // determine current x-postion
        rtl::OUString aHoriPosPropStr( RTL_CONSTASCII_USTRINGPARAM("HoriOrientPosition") );
        uno::Any aHoriPos( getPropertyValue( aHoriPosPropStr ) );
        sal_Int32 dCurrX;
        aHoriPos >>= dCurrX;
        // change x-position attribute, if needed
        if ( dCurrX != _aPosition.X )
        {
            // adjust x-position orientation to text::HoriOrientation::NONE, if needed
            // Note: has to be done before setting x-position attribute
            rtl::OUString aHoriOrientPropStr( RTL_CONSTASCII_USTRINGPARAM("HoriOrient") );
            uno::Any aHoriOrient( getPropertyValue( aHoriOrientPropStr ) );
            sal_Int16 eHoriOrient;
            aHoriOrient >>= eHoriOrient;
            if ( eHoriOrient != text::HoriOrientation::NONE )
            {
                eHoriOrient = text::HoriOrientation::NONE;
                aHoriOrient <<= eHoriOrient;
                setPropertyValue( aHoriOrientPropStr, aHoriOrient );
            }
            // set x-position attribute
            aHoriPos <<= _aPosition.X;
            setPropertyValue( aHoriPosPropStr, aHoriPos );
        }
    }

    // handle y-position
    {
        // determine current y-postion
        rtl::OUString aVertPosPropStr( RTL_CONSTASCII_USTRINGPARAM("VertOrientPosition") );
        uno::Any aVertPos( getPropertyValue( aVertPosPropStr ) );
        sal_Int32 dCurrY;
        aVertPos >>= dCurrY;
        // change y-position attribute, if needed
        if ( dCurrY != _aPosition.Y )
        {
            // adjust y-position orientation to text::VertOrientation::NONE, if needed
            // Note: has to be done before setting y-position attribute
            rtl::OUString aVertOrientPropStr( RTL_CONSTASCII_USTRINGPARAM("VertOrient") );
            uno::Any aVertOrient( getPropertyValue( aVertOrientPropStr ) );
            sal_Int16 eVertOrient;
            aVertOrient >>= eVertOrient;
            if ( eVertOrient != text::VertOrientation::NONE )
            {
                eVertOrient = text::VertOrientation::NONE;
                aVertOrient <<= eVertOrient;
                setPropertyValue( aVertOrientPropStr, aVertOrient );
            }
            // set y-position attribute
            aVertPos <<= _aPosition.Y;
            setPropertyValue( aVertPosPropStr, aVertPos );
        }
    }
}

/** method to convert start and end position of the drawing object to the
    Writer specific position, which is the attribute position in layout direction

    OD 2004-10-28 #i36248#

    @author OD
*/
void SwXShape::__ConvertStartEndPosToLayoutDir( awt::Point& _rioStartPos,
                                                awt::Point& _rioEndPos )
{
    awt::Point aPos( getPosition() );
    awt::Size aSize( getSize() );

    if ( _rioStartPos.X == _rioEndPos.X )
    {
        _rioStartPos.X = aPos.X;
        _rioEndPos.X = aPos.X;
    }
    else if ( _rioStartPos.X < _rioEndPos.X )
    {
        _rioStartPos.X = aPos.X;
        _rioEndPos.X = aPos.X + aSize.Width;
    }
    else
    {
        _rioEndPos.X = aPos.X;
        _rioStartPos.X = aPos.X + aSize.Width;
    }

    if ( _rioStartPos.Y == _rioEndPos.Y )
    {
        _rioStartPos.Y = aPos.Y;
        _rioEndPos.Y = aPos.Y;
    }
    else if ( _rioStartPos.Y < _rioEndPos.Y )
    {
        _rioStartPos.Y = aPos.Y;
        _rioEndPos.Y = aPos.Y + aSize.Height;
    }
    else
    {
        _rioEndPos.Y = aPos.Y;
        _rioStartPos.Y = aPos.Y + aSize.Height;
    }
}

/** method to convert start position of the drawing object to the
    Writer specific position, which is the attribute position in layout direction

    OD 2004-10-28 #i36248#

    @author OD
*/
awt::Point SwXShape::_ConvertStartPosToLayoutDir( const awt::Point& _aStartPos )
{
    awt::Point aStartPos( _aStartPos );
    awt::Point aEndPos( 0, 0 );
    SvxShape* pSvxShape( GetSvxShape() );
    if ( pSvxShape )
    {
        pSvxShape->_getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition")) )
                                                                >>= aEndPos;

        __ConvertStartEndPosToLayoutDir( aStartPos, aEndPos );
    }

    return aStartPos;
}

/** method to convert end position of the drawing object to the
    Writer specific position, which is the attribute position in layout direction

    OD 2004-10-28 #i36248#

    @author OD
*/
awt::Point SwXShape::_ConvertEndPosToLayoutDir( const awt::Point& _aEndPos )
{
    awt::Point aEndPos( _aEndPos );
    awt::Point aStartPos( 0, 0 );
    SvxShape* pSvxShape( GetSvxShape() );
    if ( pSvxShape )
    {
        pSvxShape->_getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition")) )
                                                                >>= aStartPos;

        __ConvertStartEndPosToLayoutDir( aStartPos, aEndPos );
    }

    return aEndPos;
}

/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXGroupShape::SwXGroupShape(uno::Reference< XInterface > & xShape) :
        SwXShape(xShape)
{
#ifdef DBG_UTIL
    uno::Reference<XShapes> xShapes(xShapeAgg, uno::UNO_QUERY);
    DBG_ASSERT(xShapes.is(), "no SvxShape found or shape is not a group shape")
#endif
}

/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXGroupShape::~SwXGroupShape()
{
}
/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXGroupShape::queryInterface( const uno::Type& rType ) throw(uno::RuntimeException)
{
    uno::Any aRet;
    if(rType == ::getCppuType((uno::Reference<XShapes>*)0))
        aRet <<= uno::Reference<XShapes>(this);
    else
        aRet = SwXShape::queryInterface(rType);
    return aRet;
}
/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXGroupShape::acquire(  ) throw()
{
    SwXShape::acquire();
}
/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXGroupShape::release(  ) throw()
{
    SwXShape::release();
}
/*-- 31.05.01 09:59:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXGroupShape::add( const uno::Reference< XShape >& xShape ) throw (uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SvxShape* pSvxShape = GetSvxShape();
    SwFrmFmt* pFmt = GetFrmFmt();
    if(pSvxShape && pFmt)
    {
        sal_Bool bOk = FALSE;
        uno::Reference<XShapes> xShapes;
        if( xShapeAgg.is() )
        {
            const uno::Type& rType = ::getCppuType((uno::Reference<XShapes>*)0 );
            uno::Any aAgg = xShapeAgg->queryAggregation( rType );
            aAgg >>= xShapes;
        }
        if(xShapes.is())
            xShapes->add(xShape);
        else
            throw uno::RuntimeException();

        uno::Reference<lang::XUnoTunnel> xTunnel(xShape, uno::UNO_QUERY);
        SwXShape* pSwShape = 0;
        if(xShape.is())
            pSwShape = (SwXShape*)xTunnel->getSomething(SwXShape::getUnoTunnelId());
        if(pSwShape && pSwShape->m_bDescriptor)
        {
            SvxShape* pAddShape = (SvxShape*)xTunnel->getSomething(SvxShape::getUnoTunnelId());
            if(pAddShape)
            {
                SdrObject* pObj = pAddShape->GetSdrObject();
                if(pObj)
                {
                    SwDoc* pDoc = pFmt->GetDoc();
                    // OD 25.06.2003 #108784# - set layer of new drawing
                    // object to corresponding invisible layer.
                    if( FmFormInventor != pObj->GetObjInventor())
                    {
                        pObj->SetLayer( pSwShape->pImpl->GetOpaque()
                                        ? pDoc->GetInvisibleHeavenId()
                                        : pDoc->GetInvisibleHellId() );
                    }
                    else
                    {
                        pObj->SetLayer(pDoc->GetInvisibleControlsId());
                    }
                }
            }
            pSwShape->m_bDescriptor = sal_False;
            //add the group member to the format of the group
            SwFrmFmt* pFmt = ::FindFrmFmt( pSvxShape->GetSdrObject() );
            if(pFmt)
                pFmt->Add(pSwShape);
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 31.05.01 09:59:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXGroupShape::remove( const uno::Reference< XShape >& xShape ) throw (uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    sal_Bool bOk = FALSE;
    uno::Reference<XShapes> xShapes;
    if( xShapeAgg.is() )
    {
        const uno::Type& rType = ::getCppuType((uno::Reference<XShapes>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rType );
        aAgg >>= xShapes;
    }
    if(!xShapes.is())
        throw uno::RuntimeException();
    xShapes->remove(xShape);
}
/*-- 31.05.01 09:59:20---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXGroupShape::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    sal_Int32 nRet = 0;
    uno::Reference<XIndexAccess> xAcc;
    if( xShapeAgg.is() )
    {
        const uno::Type& rType = ::getCppuType((uno::Reference<XIndexAccess>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rType );
        aAgg >>= xAcc;
    }
    if(!xAcc.is())
        throw uno::RuntimeException();
    return xAcc->getCount();
}
/*-- 31.05.01 09:59:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXGroupShape::getByIndex(sal_Int32 nIndex)
        throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException,
               uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference<XIndexAccess> xAcc;
    if( xShapeAgg.is() )
    {
        const uno::Type& rType = ::getCppuType((uno::Reference<XIndexAccess>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rType );
        aAgg >>= xAcc;
    }
    if(!xAcc.is())
        throw uno::RuntimeException();
    return xAcc->getByIndex(nIndex);
}
/*-- 31.05.01 09:59:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXGroupShape::getElementType(  ) throw(uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference<XIndexAccess> xAcc;
    if( xShapeAgg.is() )
    {
        const uno::Type& rType = ::getCppuType((uno::Reference<XIndexAccess>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rType );
        aAgg >>= xAcc;
    }
    if(!xAcc.is())
        throw uno::RuntimeException();
    return xAcc->getElementType();
}
/*-- 31.05.01 09:59:22---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXGroupShape::hasElements(  ) throw(uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference<XIndexAccess> xAcc;
    if( xShapeAgg.is() )
    {
        const uno::Type& rType = ::getCppuType((uno::Reference<XIndexAccess>*)0 );
        uno::Any aAgg = xShapeAgg->queryAggregation( rType );
        aAgg >>= xAcc;
    }
    if(!xAcc.is())
        throw uno::RuntimeException();
    return xAcc->hasElements();
}

