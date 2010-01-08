/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoftn.cxx,v $
 * $Revision: 1.31 $
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
#include <rtl/uuid.h>

#include <unomid.h>
#include <unofootnote.hxx>
#include <unotextrange.hxx>
#include <unotextcursor.hxx>
#include <unoparagraph.hxx>
#include <unomap.hxx>
#include <unoprnms.hxx>
#include <doc.hxx>
#include <ftnidx.hxx>
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <ndtxt.hxx>
#include <unocrsr.hxx>
#include <hints.hxx>


using namespace ::com::sun::star;
using ::rtl::OUString;

/******************************************************************
 *
 ******************************************************************/
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXFootnote::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXFootnote::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(this) );
    }
    return SwXText::getSomething( rId );
}
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFootnote::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXFootnote");
}
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXFootnote::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return  !rServiceName.compareToAscii("com.sun.star.text.Footnote") ||
            !rServiceName.compareToAscii("com.sun.star.text.TextContent") ||
            !rServiceName.compareToAscii("com.sun.star.text.Text") ||
            (m_bIsEndnote && !rServiceName.compareToAscii("com.sun.star.text.Endnote"));
;
}
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXFootnote::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(m_bIsEndnote ? 4 : 3);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Footnote");
    pArray[1] = C2U("com.sun.star.text.TextContent");
    pArray[2] = C2U("com.sun.star.text.Text");
    if(m_bIsEndnote)
        pArray[3] = C2U("com.sun.star.text.Endnote");
    return aRet;
}
/*-- 10.12.98 15:31:44---------------------------------------------------

  -----------------------------------------------------------------------*/
TYPEINIT1(SwXFootnote, SwClient);

SwXFootnote::SwXFootnote(sal_Bool bEndnote) :
    SwXText(0, CURSOR_FOOTNOTE),
    aLstnrCntnr( (text::XTextContent*)this),
    pFmtFtn(0),
    m_bIsDescriptor(sal_True),
    m_bIsEndnote(bEndnote)
{

}
/*-- 10.12.98 15:31:45---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnote::SwXFootnote(SwDoc* _pDoc, const SwFmtFtn& rFmt) :
    SwXText(_pDoc, CURSOR_FOOTNOTE),
    aLstnrCntnr( (text::XTextContent*)this),
    pFmtFtn(&rFmt),
    m_bIsDescriptor(sal_False),
    m_bIsEndnote(rFmt.IsEndNote())
{
    GetDoc()->GetUnoCallBack()->Add(this);
}
/*-- 10.12.98 15:31:45---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnote::~SwXFootnote()
{

}
/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SAL_CALL SwXFootnote::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aFtnTypes = SwXFootnote_Base::getTypes();
    uno::Sequence< uno::Type > aTextTypes = SwXText::getTypes();

    long nIndex = aFtnTypes.getLength();
    aFtnTypes.realloc(	aFtnTypes.getLength() + aTextTypes.getLength());

    uno::Type* pFtnTypes = aFtnTypes.getArray();
    const uno::Type* pTextTypes = aTextTypes.getConstArray();
    for(long nPos = 0; nPos < aTextTypes.getLength(); nPos++)
        pFtnTypes[nIndex++] = pTextTypes[nPos];

    return aFtnTypes;
}

/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SAL_CALL SwXFootnote::getImplementationId(  ) throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static uno::Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
    {
        rtl_createUuid( (sal_uInt8 *)(aId.getArray() ), 0, sal_True );
        bInit = sal_True;
    }
    return aId;
}
/* -----------------------------21.03.00 15:46--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SAL_CALL SwXFootnote::queryInterface(const uno::Type& rType)
throw (uno::RuntimeException)
{
    const uno::Any ret = SwXFootnote_Base::queryInterface(rType);
    return (ret.getValueType() == ::getCppuVoidType())
        ?   SwXText::queryInterface(rType)
        :   ret;
}

/*-- 10.12.98 15:31:47---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXFootnote::getLabel(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    String sRet;
    const SwFmtFtn*  pFmt = SwXFootnote::FindFmt();
    if(pFmt)
        sRet = pFmt->GetNumStr();
    else if(m_bIsDescriptor)
        return m_sLabel;
    else
        throw uno::RuntimeException();
    return sRet;
}
/*-- 10.12.98 15:31:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::setLabel(const OUString& aLabel) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        DBG_ASSERT(pTxtFtn, "kein TextNode?");
        SwTxtNode& rTxtNode = (SwTxtNode&)pTxtFtn->GetTxtNode();

        SwPaM aPam(rTxtNode, *pTxtFtn->GetStart());
        GetDoc()->SetCurFtn(aPam, aLabel, pFmt->GetNumber(), pFmt->IsEndNote());
    }
    else if(m_bIsDescriptor)
        m_sLabel = String(aLabel);
    else
        throw uno::RuntimeException();

}
/* -----------------18.02.99 13:32-------------------
 *
 * --------------------------------------------------*/
void SwXFootnote::attachToRange(const uno::Reference< text::XTextRange > & xTextRange)
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
    SwDoc* pNewDoc = pRange ? (SwDoc*)pRange->GetDoc() : pCursor ? (SwDoc*)pCursor->GetDoc() : 0;
    if(pNewDoc)
    {
        SwUnoInternalPaM aPam(*pNewDoc);
        //das muss jetzt sal_True liefern
        SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange);

        UnoActionContext aCont(pNewDoc);
        SwTxtAttr* pTxtAttr = 0;
        pNewDoc->DeleteAndJoin(aPam);
        aPam.DeleteMark();
        SwFmtFtn aFootNote(m_bIsEndnote);
        if(m_sLabel.Len())
            aFootNote.SetNumStr(m_sLabel);

        SwXTextCursor const*const pTextCursor(
                dynamic_cast<SwXTextCursor*>(pCursor));
        const bool bForceExpandHints( (pTextCursor)
                ? pTextCursor->IsAtEndOfMeta() : false );
        const SetAttrMode nInsertFlags = (bForceExpandHints)
            ? nsSetAttrMode::SETATTR_FORCEHINTEXPAND
            : nsSetAttrMode::SETATTR_DEFAULT;

        pNewDoc->InsertPoolItem(aPam, aFootNote, nInsertFlags);

        pTxtAttr = aPam.GetNode()->GetTxtNode()->GetTxtAttrForCharAt(
                    aPam.GetPoint()->nContent.GetIndex()-1, RES_TXTATR_FTN );

        if(pTxtAttr)
        {
            const SwFmtFtn& rFtn = pTxtAttr->GetFtn();
            pFmtFtn = &rFtn;
            pNewDoc->GetUnoCallBack()->Add(this);
            //force creation of sequence id - is used for references
            if(pNewDoc->IsInReading())
                ((SwTxtFtn*)pTxtAttr)->SetSeqNo(pNewDoc->GetFtnIdxs().Count());
            else
                ((SwTxtFtn*)pTxtAttr)->SetSeqRefNo();
        }
        m_bIsDescriptor = sal_False;
        SetDoc(pNewDoc);
    }
    else
        throw lang::IllegalArgumentException();
}
/*-- 10.12.98 15:31:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::attach(const uno::Reference< text::XTextRange > & xTextRange)
            throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    attachToRange( xTextRange );
}
/*-- 10.12.98 15:31:48---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXFootnote::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  aRef;
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        SwPaM aPam( pTxtFtn->GetTxtNode(), *pTxtFtn->GetStart() );
        SwPosition aMark( *aPam.Start() );
        aPam.SetMark();
        aPam.GetMark()->nContent++;
        aRef = SwXTextRange::CreateTextRangeFromPosition((SwDoc*)GetDoc(), *aPam.Start(), aPam.End());
    }
    else
        throw uno::RuntimeException();
    return aRef;
}
/*-- 10.12.98 15:31:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::dispose(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        DBG_ASSERT(pTxtFtn, "kein TextNode?");
        SwTxtNode& rTxtNode = (SwTxtNode&)pTxtFtn->GetTxtNode();
        xub_StrLen nPos = *pTxtFtn->GetStart();
        SwPaM aPam(rTxtNode, nPos, rTxtNode, nPos+1 );
        GetDoc()->DeleteAndJoin( aPam );
    }
    else
        throw uno::RuntimeException();

}
/*-- 10.12.98 15:31:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::addEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn())
        throw uno::RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 10.12.98 15:31:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::removeEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw uno::RuntimeException();
}
/* -----------------06.05.99 15:31-------------------
 *
 * --------------------------------------------------*/
const SwStartNode *SwXFootnote::GetStartNode() const
{
    const SwStartNode *pSttNd = 0;
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        if( pTxtFtn )
            pSttNd = pTxtFtn->GetStartNode()->GetNode().GetStartNode();
    }
    return pSttNd;
}

uno::Reference< text::XTextCursor >   SwXFootnote::createCursor() throw ( uno::RuntimeException)
{
    return createTextCursor();
}
/*-- 10.12.98 15:31:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextCursor >  SwXFootnote::createTextCursor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextCursor >  aRef;
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        SwPosition aPos( *pTxtFtn->GetStartNode() );
        SwXTextCursor* pXCrsr = new SwXTextCursor(this, aPos, CURSOR_FOOTNOTE, GetDoc());
        aRef =  (text::XWordCursor*)pXCrsr;
        SwUnoCrsr*	pUnoCrsr = pXCrsr->GetCrsr();
        pUnoCrsr->Move(fnMoveForward, fnGoNode);
    }
    else
        throw uno::RuntimeException();
    return aRef;
}
/*-- 10.12.98 15:31:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextCursor >  SwXFootnote::createTextCursorByRange(
    const uno::Reference< text::XTextRange > & aTextPosition)
            throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SwFmtFtn*  pFmt = FindFmt();
    if( !pFmt ) 
        throw uno::RuntimeException();
    uno::Reference< text::XTextCursor >  aRef;
    SwUnoInternalPaM aPam(*GetDoc());
    if(SwXTextRange::XTextRangeToSwPaM(aPam, aTextPosition))
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        const SwNode* pFtnStartNode = &pTxtFtn->GetStartNode()->GetNode();

        const SwNode* pStart = aPam.GetNode()->FindFootnoteStartNode();
        if( pStart != pFtnStartNode )
            throw uno::RuntimeException();
        
        aRef = (text::XWordCursor*)new SwXTextCursor(this , *aPam.GetPoint(), CURSOR_FOOTNOTE, GetDoc(), aPam.GetMark());
    }
    else
        throw uno::RuntimeException();
    return aRef;
}
/*-- 13.06.00 14:28:23---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXFootnote::createEnumeration() throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< container::XEnumeration >  aRef;
    const SwFmtFtn*  pFmt = FindFmt();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        SwPosition aPos( *pTxtFtn->GetStartNode() );
        ::std::auto_ptr<SwUnoCrsr> pUnoCursor(
            GetDoc()->CreateUnoCrsr(aPos, sal_False));
        pUnoCursor->Move(fnMoveForward, fnGoNode);
        aRef = new SwXParagraphEnumeration(this, pUnoCursor, CURSOR_FOOTNOTE);
    }
    return aRef;
}
/*-- 13.06.00 14:28:24---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXFootnote::getElementType(  ) throw(uno::RuntimeException)
{
    return ::getCppuType(static_cast<uno::Reference<XTextRange>*>(0));
}
/*-- 13.06.00 14:28:24---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXFootnote::hasElements(  ) throw(uno::RuntimeException)
{
    return sal_True;
}
/* -----------------------------07.01.00 12:39--------------------------------

 ---------------------------------------------------------------------------*/
void SwXFootnote::Invalidate()
{
    if(GetRegisteredIn())
    {
        ((SwModify*)GetRegisteredIn())->Remove(this);
        pFmtFtn = 0;
        aLstnrCntnr.Disposing();
        SetDoc(0);
    }
}
/* -----------------18.01.99 09:12-------------------
 *
 * --------------------------------------------------*/
void SwXFootnote::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
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
    case RES_FOOTNOTE_DELETED:
        if( (void*)pFmtFtn == ((SwPtrMsgPoolItem *)pOld)->pObject )
            Invalidate();
        break;
    }
}
/*-- 11.09.00 13:12:03---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SwXFootnote::getPropertySetInfo(  )
    throw(uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo >  xRef = aSwMapProvider.GetPropertySet(PROPERTY_MAP_FOOTNOTE)->getPropertySetInfo();
    return xRef;
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::setPropertyValue( const ::rtl::OUString&,
    const uno::Any& )
        throw(beans::UnknownPropertyException, beans::PropertyVetoException,
            lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    //no values to be set
    throw lang::IllegalArgumentException();
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXFootnote::getPropertyValue( const OUString& rPropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(!SwXParagraph::getDefaultTextContentValue(aRet, rPropertyName))
    {
        if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_START_REDLINE))||
                rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_END_REDLINE)))
        {
            //redline can only be returned if it's a living object
            if(!m_bIsDescriptor)
                aRet = SwXText::getPropertyValue(rPropertyName);
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_REFERENCE_ID)))
        {
            const SwFmtFtn*  pFmt = FindFmt();
            if(pFmt)
            {
                const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
                DBG_ASSERT(pTxtFtn, "no TextNode?");
                aRet <<= (sal_Int16)pTxtFtn->GetSeqRefNo();
            }
        }
        else
        {
            beans::UnknownPropertyException aExcept;
            aExcept.Message = rPropertyName;
            throw aExcept;
        }
    }
    return aRet;
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::addPropertyChangeListener( const OUString& /*aPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::removePropertyChangeListener( const OUString& /*aPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::addVetoableChangeListener( const OUString& /*PropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.09.00 13:12:05---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFootnote::removeVetoableChangeListener( const OUString& /*PropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}


