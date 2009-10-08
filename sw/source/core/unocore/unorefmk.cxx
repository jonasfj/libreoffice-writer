/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unorefmk.cxx,v $
 * $Revision: 1.17 $
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


#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <unoobj.hxx>
#include <unomap.hxx>
#include <unocrsr.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <fmtrfmrk.hxx>
#include <txtrfmrk.hxx>
#include <hints.hxx>

using namespace ::com::sun::star;
using ::rtl::OUString;

/******************************************************************
 * SwXReferenceMark
 ******************************************************************/

/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXReferenceMark::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXReferenceMark::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(this) );
    }
    return 0;
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXReferenceMark::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXReferenceMark");
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXReferenceMark::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return !rServiceName.compareToAscii("com.sun.star.text.ReferenceMark")||
                !rServiceName.compareToAscii("com.sun.star.text.TextContent");
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXReferenceMark::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.ReferenceMark");
    pArray[1] = C2U("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 11.12.98 10:28:32---------------------------------------------------

  -----------------------------------------------------------------------*/
TYPEINIT1(SwXReferenceMark, SwClient);

SwXReferenceMark::SwXReferenceMark(SwDoc* pDc, const SwFmtRefMark* pRefMark) :
    aLstnrCntnr( (text::XTextContent*)this),
    pDoc(pDc),
    pMark(pRefMark),
    m_bIsDescriptor(0 == pRefMark)
{
    if(pRefMark)
        sMarkName = pRefMark->GetRefName();
    if(pDoc)
        pDoc->GetUnoCallBack()->Add(this);
}
/*-- 11.12.98 10:28:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMark::~SwXReferenceMark()
{

}
/* -----------------03.11.99 14:14-------------------

 --------------------------------------------------*/
void SwXReferenceMark::InsertRefMark( SwPaM& rPam )
{
    //! in some cases when this function is called the pDoc pointer member may have become
    //! invalid/deleted thus we obtain the document pointer from rPaM where it should always
    //! be valid.
    SwDoc *pDoc2 = rPam.GetDoc();

    UnoActionContext aCont(pDoc2);
    SwTxtAttr* pTxtAttr = 0;
    SwFmtRefMark aRefMark(sMarkName);
    SfxItemSet  aSet(pDoc2->GetAttrPool(), RES_TXTATR_REFMARK, RES_TXTATR_REFMARK, 0L);
    aSet.Put(aRefMark);
    sal_Bool bMark = *rPam.GetPoint() != *rPam.GetMark();
    SwXTextCursor::SetCrsrAttr(rPam, aSet, 0);

    if( bMark && *rPam.GetPoint() > *rPam.GetMark())
        rPam.Exchange();

    if( bMark )
        pTxtAttr = rPam.GetNode()->GetTxtNode()->GetTxtAttr(
                rPam.GetPoint()->nContent, RES_TXTATR_REFMARK );
    else
        pTxtAttr = rPam.GetNode()->GetTxtNode()->GetTxtAttr(
                rPam.GetPoint()->nContent.GetIndex()-1, RES_TXTATR_REFMARK );

    if(pTxtAttr)
        pMark = &pTxtAttr->GetRefMark();

    pDoc2->GetUnoCallBack()->Add(this);
}

/* -----------------18.02.99 13:33-------------------
 *
 * --------------------------------------------------*/
void SwXReferenceMark::attachToRange(const uno::Reference< text::XTextRange > & xTextRange)
                throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    if(!m_bIsDescriptor)
        throw uno::RuntimeException();
    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange 	= reinterpret_cast< SwXTextRange * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXTextRange::getUnoTunnelId()) ));
        pCursor = reinterpret_cast< OTextCursorHelper * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( OTextCursorHelper::getUnoTunnelId()) ));
    }
    SwDoc* pDocument = pRange ? (SwDoc*)pRange->GetDoc() : pCursor ? (SwDoc*)pCursor->GetDoc() : 0;
    if(pDocument)
    {
        SwUnoInternalPaM aPam(*pDocument);
        //das muss jetzt sal_True liefern
        SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange);
        InsertRefMark(aPam);
        m_bIsDescriptor = sal_False;
        pDoc = pDocument;
        pDoc->GetUnoCallBack()->Add(this);
    }
    else
        throw lang::IllegalArgumentException();
}
/*-- 11.12.98 10:28:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::attach(const uno::Reference< text::XTextRange > & xTextRange)
                throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    attachToRange( xTextRange );
}
/*-- 11.12.98 10:28:34---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXReferenceMark::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  xRet;
    if(IsValid())
    {
        const SwFmtRefMark*	pNewMark = pDoc->GetRefMark(sMarkName);
        if(pNewMark && pNewMark == pMark)
        {
            const SwTxtRefMark* pTxtMark = pMark->GetTxtRefMark();
            if(pTxtMark &&
                &pTxtMark->GetTxtNode().GetNodes() == &pDoc->GetNodes())
            {
                SwTxtNode& rTxtNode = (SwTxtNode&)pTxtMark->GetTxtNode();
                SwPaM* pPam  = pTxtMark->GetEnd() ?
                                new SwPaM( rTxtNode, *pTxtMark->GetEnd(),
                                    rTxtNode, *pTxtMark->GetStart()) :
                                new SwPaM(	rTxtNode, *pTxtMark->GetStart());


                xRet = SwXTextRange::CreateTextRangeFromPosition(pDoc,
                        *pPam->Start(), pPam->End());
                delete pPam;
            }
        }
    }
    return xRet;
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::dispose(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(IsValid())
    {
        const SwFmtRefMark*	pNewMark = pDoc->GetRefMark(sMarkName);
        if(pNewMark && pNewMark == pMark)
        {
            const SwTxtRefMark* pTxtMark = pMark->GetTxtRefMark();
            if(pTxtMark &&
                &pTxtMark->GetTxtNode().GetNodes() == &pDoc->GetNodes())
            {
                SwTxtNode& rTxtNode = (SwTxtNode&)pTxtMark->GetTxtNode();
                xub_StrLen nStt = *pTxtMark->GetStart(),
                           nEnd = pTxtMark->GetEnd() ? *pTxtMark->GetEnd()
                                                     : nStt + 1;

                SwPaM aPam( rTxtNode, nStt, rTxtNode, nEnd );
                pDoc->DeleteAndJoin( aPam );
            }
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::addEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn())
        throw uno::RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::removeEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw uno::RuntimeException();
}
/*-- 11.12.98 10:28:36---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXReferenceMark::getName(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid() || !pDoc->GetRefMark(sMarkName))
    {
        throw uno::RuntimeException();
    }
    return sMarkName;
}
/*-- 11.12.98 10:28:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::setName(const OUString& Name_) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(m_bIsDescriptor)
        sMarkName = String(Name_);
    else
    {
        String sNewName(Name_);
        if(!IsValid() || !pDoc->GetRefMark(sMarkName) || pDoc->GetRefMark(sNewName))
        {
            throw uno::RuntimeException();
        }
        const SwFmtRefMark*	pCurMark = pDoc->GetRefMark(sMarkName);
        if(sNewName != sMarkName && pCurMark && pCurMark == pMark)
        {
            UnoActionContext aCont(pDoc);
            const SwTxtRefMark* pTxtMark = pMark->GetTxtRefMark();
            if(pTxtMark &&
                &pTxtMark->GetTxtNode().GetNodes() == &pDoc->GetNodes())
            {
                SwTxtNode& rTxtNode = (SwTxtNode&)pTxtMark->GetTxtNode();
                xub_StrLen nStt = *pTxtMark->GetStart(),
                           nEnd = pTxtMark->GetEnd() ? *pTxtMark->GetEnd()
                                                     : nStt + 1;

                SwPaM aPam( rTxtNode, nStt, rTxtNode, nEnd );
                pDoc->DeleteAndJoin( aPam );	//! deletes the pDoc member in the SwXReferenceMark
                                                //! The aPam will keep the correct and functional doc though

                sMarkName = sNewName;
                //create a new one
                InsertRefMark( aPam );
                pDoc = aPam.GetDoc();
            }
        }
    }
}
/* -----------------------------07.01.00 12:51--------------------------------

 ---------------------------------------------------------------------------*/
void 	SwXReferenceMark::Invalidate()
{
    if(GetRegisteredIn())
    {
        ((SwModify*)GetRegisteredIn())->Remove(this);
        aLstnrCntnr.Disposing();
        pDoc = 0;
        pMark = 0;
    }
}
/*-- 11.12.98 10:28:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    switch( pOld ? pOld->Which() : 0 )
    {
    case RES_REMOVE_UNO_OBJECT:
    case RES_OBJECTDYING:
        if( (void*)GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            Invalidate();
        break;
    case RES_FMT_CHG:
        // wurden wir an das neue umgehaengt und wird das alte geloscht?
        if( ((SwFmtChg*)pNew)->pChangedFmt == GetRegisteredIn() &&
            ((SwFmtChg*)pOld)->pChangedFmt->IsFmtInDTOR() )
            Invalidate();
        break;
    case RES_REFMARK_DELETED:
        if( (void*)pMark == ((SwPtrMsgPoolItem *)pOld)->pObject )
            Invalidate();
        break;
    }
}

/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SwXReferenceMark::getPropertySetInfo(  ) throw(uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo >  xRef =
        aSwMapProvider.GetPropertySet(PROPERTY_MAP_PARAGRAPH_EXTENSIONS)->getPropertySetInfo();
    return xRef;
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::setPropertyValue(
    const OUString& /*rPropertyName*/, const uno::Any& /*rValue*/ )
        throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    throw lang::IllegalArgumentException();
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXReferenceMark::getPropertyValue( const OUString& rPropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    uno::Any aRet;
    if(!SwXParagraph::getDefaultTextContentValue(aRet, rPropertyName))
        throw beans::UnknownPropertyException();
    return aRet;
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::addPropertyChangeListener(
    const OUString& /*rPropertyName*/, const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
            throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::removePropertyChangeListener(
    const OUString& /*rPropertyName*/, const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
            throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::addVetoableChangeListener( const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 12.09.00 12:58:21---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::removeVetoableChangeListener(
    const OUString& /*rPropertyName*/, const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}

