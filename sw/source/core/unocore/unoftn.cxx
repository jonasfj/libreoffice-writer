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

#include <rtl/uuid.h>

#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <comphelper/sequence.hxx>

#include <unomid.h>
#include <unofootnote.hxx>
#include <unotextrange.hxx>
#include <unotextcursor.hxx>
#include <unoparagraph.hxx>
#include <unomap.hxx>
#include <unoprnms.hxx>
#include <unoevtlstnr.hxx>
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
 * SwXFootnote
 ******************************************************************/

class SwXFootnote::Impl
    : public SwClient
{

public:

    SwXFootnote &               m_rThis;
    const bool                  m_bIsEndnote;
    SwEventListenerContainer    m_ListenerContainer;
    bool                        m_bIsDescriptor;
    const SwFmtFtn *            m_pFmtFtn;
    ::rtl::OUString             m_sLabel;

    Impl(   SwXFootnote & rThis,
            SwDoc *const pDoc, SwFmtFtn const*const pFootnote,
            const bool bIsEndnote)
        : SwClient((pDoc) ? pDoc->GetUnoCallBack() : 0)
        , m_rThis(rThis)
        , m_bIsEndnote(bIsEndnote)
        , m_ListenerContainer(static_cast< ::cppu::OWeakObject* >(&rThis))
// #i111177#: unxsols4 (Sun C++ 5.9 SunOS_sparc) generates wrong code for this
//        , m_bIsDescriptor(0 == pFootnote)
        , m_bIsDescriptor((0 == pFootnote) ? true : false)
        , m_pFmtFtn(pFootnote)
    {
    }

    const SwFmtFtn* GetFootnoteFormat() const {
        return m_rThis.GetDoc() ? m_pFmtFtn : 0;
    }

    SwFmtFtn const& GetFootnoteFormatOrThrow() {
        SwFmtFtn const*const pFootnote( GetFootnoteFormat() );
        if (!pFootnote) {
            throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "SwXFootnote: disposed or invalid")), 0);
        }
        return *pFootnote;
    }

    void    Invalidate();

    // SwClient
    virtual void    Modify(SfxPoolItem *pOld, SfxPoolItem *pNew);

};

/* -----------------------------07.01.00 12:39--------------------------------

 ---------------------------------------------------------------------------*/
void SwXFootnote::Impl::Invalidate()
{
    if (GetRegisteredIn())
    {
        const_cast<SwModify*>(GetRegisteredIn())->Remove(this);
    }
    m_ListenerContainer.Disposing();
    m_pFmtFtn = 0;
    m_rThis.SetDoc(0);
}

/* -----------------18.01.99 09:12-------------------
 *
 * --------------------------------------------------*/
void SwXFootnote::Impl::Modify(SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);

    if (!GetRegisteredIn()) // removed => dispose
    {
        Invalidate();
    }
    else if (pOld)
    {
        switch (pOld->Which())
        {
            case RES_FOOTNOTE_DELETED:
                if (static_cast<const void*>(m_pFmtFtn) ==
                        static_cast<SwPtrMsgPoolItem *>(pOld)->pObject)
                {
                    Invalidate();
                }
                break;
        }
    }
}

/*-- 10.12.98 15:31:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnote::SwXFootnote(const bool bEndnote)
    : SwXText(0, CURSOR_FOOTNOTE)
    , m_pImpl( new SwXFootnote::Impl(*this, 0, 0, bEndnote) )
{
}
/*-- 10.12.98 15:31:45---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnote::SwXFootnote(SwDoc & rDoc, const SwFmtFtn& rFmt)
    : SwXText(& rDoc, CURSOR_FOOTNOTE)
    , m_pImpl( new SwXFootnote::Impl(*this, &rDoc, &rFmt, rFmt.IsEndNote()) )
{
}
/*-- 10.12.98 15:31:45---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnote::~SwXFootnote()
{
}

SwXFootnote *
SwXFootnote::GetXFootnote(
        SwModify const& /*rUnoCB*/, SwFmtFtn const& /*rFootnoteFmt*/)
{
    // re-use existing SwXFootnote
    // #i105557#: do not iterate over the registered clients: race condition
    // to do this properly requires the SwXFootnote to register at the
    // SwFmtFtn directly, not at the unocallback
    // also this function must return a uno Reference!
#if 0
    SwClientIter aIter( rUnoCB );
    SwXFootnote::Impl * pXFootnote = static_cast<SwXFootnote::Impl*>(
            aIter.First( TYPE( SwXFootnote::Impl )));
    while (pXFootnote)
    {
        SwDoc *const pDoc = pXFootnote->m_rThis.GetDoc();
        if (pDoc)
        {
            SwFmtFtn const*const pFtn = pXFootnote->GetFootnoteFormat();
            if (pFtn == &rFootnoteFmt)
            {
                return & pXFootnote->m_rThis;
            }
        }
        pXFootnote = static_cast<SwXFootnote::Impl*>(aIter.Next());
    }
#endif
    return 0;
}

SwXFootnote *
SwXFootnote::CreateXFootnote(SwDoc & rDoc, SwFmtFtn const& rFootnoteFmt)
{
    SwXFootnote *const pXFootnote(
        GetXFootnote(*rDoc.GetUnoCallBack(), rFootnoteFmt));
    return (pXFootnote)
        ?   pXFootnote
        :   new SwXFootnote(rDoc, rFootnoteFmt);
}

/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXFootnote::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL
SwXFootnote::getSomething(const uno::Sequence< sal_Int8 >& rId)
throw (uno::RuntimeException)
{
    const sal_Int64 nRet( ::sw::UnoTunnelImpl<SwXFootnote>(rId, this) );
    return (nRet) ? nRet : SwXText::getSomething(rId);
}

/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
OUString SAL_CALL
SwXFootnote::getImplementationName() throw (uno::RuntimeException)
{
    return C2U("SwXFootnote");
}

/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
static char const*const g_ServicesFootnote[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.Footnote",
    "com.sun.star.text.Text",
    "com.sun.star.text.Endnote", // NB: only supported for endnotes!
};
static const size_t g_nServicesEndnote(
    sizeof(g_ServicesFootnote)/sizeof(g_ServicesFootnote[0]));
static const size_t g_nServicesFootnote( g_nServicesEndnote - 1 ); // NB: omit!

sal_Bool SAL_CALL SwXFootnote::supportsService(const OUString& rServiceName)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return ::sw::SupportsServiceImpl(
            (m_pImpl->m_bIsEndnote) ? g_nServicesEndnote : g_nServicesFootnote,
            g_ServicesFootnote, rServiceName);
}

uno::Sequence< OUString > SAL_CALL
SwXFootnote::getSupportedServiceNames() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return ::sw::GetSupportedServiceNamesImpl(
            (m_pImpl->m_bIsEndnote) ? g_nServicesEndnote : g_nServicesFootnote,
            g_ServicesFootnote);
}

/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SAL_CALL
SwXFootnote::getTypes() throw (uno::RuntimeException)
{
    const uno::Sequence< uno::Type > aTypes = SwXFootnote_Base::getTypes();
    const uno::Sequence< uno::Type > aTextTypes = SwXText::getTypes();
    return ::comphelper::concatSequences(aTypes, aTextTypes);
}

/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SAL_CALL
SwXFootnote::getImplementationId() throw (uno::RuntimeException)
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
uno::Any SAL_CALL
SwXFootnote::queryInterface(const uno::Type& rType)
throw (uno::RuntimeException)
{
    const uno::Any ret = SwXFootnote_Base::queryInterface(rType);
    return (ret.getValueType() == ::getCppuVoidType())
        ?   SwXText::queryInterface(rType)
        :   ret;
}

/*-- 10.12.98 15:31:47---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SAL_CALL SwXFootnote::getLabel() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    ::rtl::OUString sRet;
    SwFmtFtn const*const pFmt = m_pImpl->GetFootnoteFormat();
    if(pFmt)
    {
        sRet = pFmt->GetNumStr();
    }
    else if (m_pImpl->m_bIsDescriptor)
    {
        sRet = m_pImpl->m_sLabel;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return sRet;
}

/*-- 10.12.98 15:31:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXFootnote::setLabel(const OUString& aLabel) throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const*const pFmt = m_pImpl->GetFootnoteFormat();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        DBG_ASSERT(pTxtFtn, "kein TextNode?");
        SwTxtNode& rTxtNode = (SwTxtNode&)pTxtFtn->GetTxtNode();

        SwPaM aPam(rTxtNode, *pTxtFtn->GetStart());
        GetDoc()->SetCurFtn(aPam, aLabel, pFmt->GetNumber(), pFmt->IsEndNote());
    }
    else if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->m_sLabel = String(aLabel);
    }
    else
    {
        throw uno::RuntimeException();
    }
}

/* -----------------18.02.99 13:32-------------------
 *
 * --------------------------------------------------*/
void SAL_CALL
SwXFootnote::attach(const uno::Reference< text::XTextRange > & xTextRange)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (!m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }
    const uno::Reference<lang::XUnoTunnel> xRangeTunnel(
            xTextRange, uno::UNO_QUERY);
    SwXTextRange *const pRange =
        ::sw::UnoTunnelGetImplementation<SwXTextRange>(xRangeTunnel);
    OTextCursorHelper *const pCursor =
        ::sw::UnoTunnelGetImplementation<OTextCursorHelper>(xRangeTunnel);
    SwDoc *const pNewDoc =
        (pRange) ? pRange->GetDoc() : ((pCursor) ? pCursor->GetDoc() : 0);
    if (!pNewDoc)
    {
        throw lang::IllegalArgumentException();
    }

    SwUnoInternalPaM aPam(*pNewDoc);
    //das muss jetzt sal_True liefern
    ::sw::XTextRangeToSwPaM(aPam, xTextRange);

    UnoActionContext aCont(pNewDoc);
    pNewDoc->DeleteAndJoin(aPam);
    aPam.DeleteMark();
    SwFmtFtn aFootNote(m_pImpl->m_bIsEndnote);
    if (m_pImpl->m_sLabel.getLength())
    {
        aFootNote.SetNumStr(m_pImpl->m_sLabel);
    }

    SwXTextCursor const*const pTextCursor(
            dynamic_cast<SwXTextCursor*>(pCursor));
    const bool bForceExpandHints( (pTextCursor)
            ? pTextCursor->IsAtEndOfMeta() : false );
    const SetAttrMode nInsertFlags = (bForceExpandHints)
        ? nsSetAttrMode::SETATTR_FORCEHINTEXPAND
        : nsSetAttrMode::SETATTR_DEFAULT;

    pNewDoc->InsertPoolItem(aPam, aFootNote, nInsertFlags);

    SwTxtFtn *const pTxtAttr = static_cast<SwTxtFtn*>(
        aPam.GetNode()->GetTxtNode()->GetTxtAttrForCharAt(
                aPam.GetPoint()->nContent.GetIndex()-1, RES_TXTATR_FTN ));

    if (pTxtAttr)
    {
        const SwFmtFtn& rFtn = pTxtAttr->GetFtn();
        m_pImpl->m_pFmtFtn = &rFtn;
        pNewDoc->GetUnoCallBack()->Add(m_pImpl.get());
        // force creation of sequence id - is used for references
        if (pNewDoc->IsInReading())
        {
            pTxtAttr->SetSeqNo(pNewDoc->GetFtnIdxs().Count());
        }
        else
        {
            pTxtAttr->SetSeqRefNo();
        }
    }
    m_pImpl->m_bIsDescriptor = sal_False;
    SetDoc(pNewDoc);
}

/*-- 10.12.98 15:31:48---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXFootnote::getAnchor() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const& rFmt( m_pImpl->GetFootnoteFormatOrThrow() );

    SwTxtFtn const*const pTxtFtn = rFmt.GetTxtFtn();
    SwPaM aPam( pTxtFtn->GetTxtNode(), *pTxtFtn->GetStart() );
    SwPosition aMark( *aPam.Start() );
    aPam.SetMark();
    aPam.GetMark()->nContent++;
    const uno::Reference< text::XTextRange > xRet =
        SwXTextRange::CreateXTextRange(*GetDoc(), *aPam.Start(), aPam.End());
    return xRet;
}
/*-- 10.12.98 15:31:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXFootnote::dispose() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const& rFmt( m_pImpl->GetFootnoteFormatOrThrow() );

    SwTxtFtn const*const pTxtFtn = rFmt.GetTxtFtn();
    DBG_ASSERT(pTxtFtn, "no TextNode?");
    SwTxtNode& rTxtNode = const_cast<SwTxtNode&>(pTxtFtn->GetTxtNode());
    const xub_StrLen nPos = *pTxtFtn->GetStart();
    SwPaM aPam(rTxtNode, nPos, rTxtNode, nPos+1);
    GetDoc()->DeleteAndJoin( aPam );
}

/*-- 10.12.98 15:31:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXFootnote::addEventListener(
    const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetFootnoteFormat())
    {
        throw uno::RuntimeException();
    }
    m_pImpl->m_ListenerContainer.AddListener(xListener);
}
/*-- 10.12.98 15:31:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXFootnote::removeEventListener(
    const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetFootnoteFormat() ||
        !m_pImpl->m_ListenerContainer.RemoveListener(xListener))
    {
        throw uno::RuntimeException();
    }
}

/* -----------------06.05.99 15:31-------------------
 *
 * --------------------------------------------------*/
const SwStartNode *SwXFootnote::GetStartNode() const
{
    SwFmtFtn const*const   pFmt = m_pImpl->GetFootnoteFormat();
    if(pFmt)
    {
        const SwTxtFtn* pTxtFtn = pFmt->GetTxtFtn();
        if( pTxtFtn )
        {
            return pTxtFtn->GetStartNode()->GetNode().GetStartNode();
        }
    }
    return 0;
}

uno::Reference< text::XTextCursor >
SwXFootnote::CreateCursor() throw (uno::RuntimeException)
{
    return createTextCursor();
}

/*-- 10.12.98 15:31:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextCursor > SAL_CALL
SwXFootnote::createTextCursor() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const& rFmt( m_pImpl->GetFootnoteFormatOrThrow() );

    SwTxtFtn const*const pTxtFtn = rFmt.GetTxtFtn();
    SwPosition aPos( *pTxtFtn->GetStartNode() );
    SwXTextCursor *const pXCursor =
        new SwXTextCursor(*GetDoc(), this, CURSOR_FOOTNOTE, aPos);
    SwUnoCrsr *const pUnoCrsr = pXCursor->GetCursor();
    pUnoCrsr->Move(fnMoveForward, fnGoNode);
    const uno::Reference< text::XTextCursor > xRet =
        static_cast<text::XWordCursor*>(pXCursor);
    return xRet;
}

/*-- 10.12.98 15:31:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextCursor > SAL_CALL
SwXFootnote::createTextCursorByRange(
    const uno::Reference< text::XTextRange > & xTextPosition)
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const& rFmt( m_pImpl->GetFootnoteFormatOrThrow() );

    SwUnoInternalPaM aPam(*GetDoc());
    if (!::sw::XTextRangeToSwPaM(aPam, xTextPosition))
    {
        throw uno::RuntimeException();
    }

    SwTxtFtn const*const pTxtFtn = rFmt.GetTxtFtn();
    SwNode const*const pFtnStartNode = &pTxtFtn->GetStartNode()->GetNode();

    const SwNode* pStart = aPam.GetNode()->FindFootnoteStartNode();
    if (pStart != pFtnStartNode)
    {
        throw uno::RuntimeException();
    }

    const uno::Reference< text::XTextCursor > xRet =
        static_cast<text::XWordCursor*>(
                new SwXTextCursor(*GetDoc(), this, CURSOR_FOOTNOTE,
                    *aPam.GetPoint(), aPam.GetMark()));
    return xRet;
}

/*-- 13.06.00 14:28:23---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration > SAL_CALL
SwXFootnote::createEnumeration() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwFmtFtn const& rFmt( m_pImpl->GetFootnoteFormatOrThrow() );

    SwTxtFtn const*const pTxtFtn = rFmt.GetTxtFtn();
    SwPosition aPos( *pTxtFtn->GetStartNode() );
    ::std::auto_ptr<SwUnoCrsr> pUnoCursor(
        GetDoc()->CreateUnoCrsr(aPos, sal_False));
    pUnoCursor->Move(fnMoveForward, fnGoNode);
    const uno::Reference< container::XEnumeration >  xRet =
        new SwXParagraphEnumeration(this, pUnoCursor, CURSOR_FOOTNOTE);
    return xRet;
}

/*-- 13.06.00 14:28:24---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXFootnote::getElementType() throw (uno::RuntimeException)
{
    return text::XTextRange::static_type();
}
/*-- 13.06.00 14:28:24---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SAL_CALL SwXFootnote::hasElements() throw (uno::RuntimeException)
{
    return sal_True;
}

/*-- 11.09.00 13:12:03---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SAL_CALL
SwXFootnote::getPropertySetInfo()
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    static uno::Reference< beans::XPropertySetInfo > xRet =
        aSwMapProvider.GetPropertySet(PROPERTY_MAP_FOOTNOTE)
            ->getPropertySetInfo();
    return xRet;
}

/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXFootnote::setPropertyValue(const ::rtl::OUString&, const uno::Any&)
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
        lang::IllegalArgumentException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    //no values to be set
    throw lang::IllegalArgumentException();
}
/*-- 11.09.00 13:12:04---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
SwXFootnote::getPropertyValue(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Any aRet;
    if (! ::sw::GetDefaultTextContentValue(aRet, rPropertyName))
    {
        if (rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_START_REDLINE)) ||
            rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_END_REDLINE)))
        {
            //redline can only be returned if it's a living object
            if (!m_pImpl->m_bIsDescriptor)
            {
                aRet = SwXText::getPropertyValue(rPropertyName);
            }
        }
        else if (rPropertyName.equalsAsciiL(
                    SW_PROP_NAME(UNO_NAME_REFERENCE_ID)))
        {
            SwFmtFtn const*const pFmt = m_pImpl->GetFootnoteFormat();
            if (pFmt)
            {
                SwTxtFtn const*const pTxtFtn = pFmt->GetTxtFtn();
                DBG_ASSERT(pTxtFtn, "no TextNode?");
                aRet <<= static_cast<sal_Int16>(pTxtFtn->GetSeqRefNo());
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
void SAL_CALL
SwXFootnote::addPropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXFootnote::addPropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXFootnote::removePropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXFootnote::removePropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXFootnote::addVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXFootnote::addVetoableChangeListener(): not implemented");
}

void SAL_CALL
SwXFootnote::removeVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXFootnote::removeVetoableChangeListener(): not implemented");
}

