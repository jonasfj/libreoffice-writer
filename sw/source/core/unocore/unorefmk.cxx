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

#include <unomid.h>
#include <unotextrange.hxx>
#include <unorefmark.hxx>
#include <unotextcursor.hxx>
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

class SwXReferenceMark::Impl
    : public SwClient
{

public:
    SwEventListenerContainer    m_ListenerContainer;
    bool                        m_bIsDescriptor;
    SwDoc *                     m_pDoc;
    const SwFmtRefMark *        m_pMarkFmt;
    ::rtl::OUString             m_sMarkName;

    Impl(   SwXReferenceMark & rThis,
            SwDoc *const pDoc, SwFmtRefMark const*const pRefMark)
        : SwClient((pDoc) ? pDoc->GetUnoCallBack() : 0)
        , m_ListenerContainer(static_cast< ::cppu::OWeakObject* >(&rThis))
        , m_bIsDescriptor(0 == pRefMark)
        , m_pDoc(pDoc)
        , m_pMarkFmt(pRefMark)
    {
        if (pRefMark)
        {
            m_sMarkName = pRefMark->GetRefName();
        }
    }

    bool    IsValid() const { return 0 != GetRegisteredIn(); }
    void    InsertRefMark( SwPaM & rPam, SwXTextCursor const*const pCursor );
    void    Invalidate();

    // SwClient
    virtual void    Modify(SfxPoolItem *pOld, SfxPoolItem *pNew);

};

/* -----------------------------07.01.00 12:51--------------------------------

 ---------------------------------------------------------------------------*/
void SwXReferenceMark::Impl::Invalidate()
{
    if (IsValid())
    {
        const_cast<SwModify*>(GetRegisteredIn())->Remove(this);
    }
    m_ListenerContainer.Disposing();
    m_pDoc = 0;
    m_pMarkFmt = 0;
}

/*-- 11.12.98 10:28:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXReferenceMark::Impl::Modify(SfxPoolItem *pOld, SfxPoolItem *pNew)
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
            case RES_REFMARK_DELETED:
                if (static_cast<const void*>(m_pMarkFmt) ==
                        static_cast<SwPtrMsgPoolItem *>(pOld)->pObject)
                {
                    Invalidate();
                }
                break;
        }
    }
}


/*-- 11.12.98 10:28:32---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMark::SwXReferenceMark(
        SwDoc *const pDoc, SwFmtRefMark const*const pRefMark)
    : m_pImpl( new SwXReferenceMark::Impl(*this, pDoc, pRefMark) )
{
}

/*-- 11.12.98 10:28:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMark::~SwXReferenceMark()
{
}

SwXReferenceMark *
SwXReferenceMark::GetReferenceMark(
        SwModify const& /*rUnoCB*/, SwFmtRefMark const& /*rMarkFmt*/)
{
    // #i105557#: do not iterate over the registered clients: race condition
    // to do this properly requires the SwXReferenceMark to register at the
    // SwFmtRefMark directly, not at the unocallback
#if 0
    SwClientIter aIter( rUnoCB );
    SwXReferenceMark::Impl * pXMark =
        static_cast<SwXReferenceMark::Impl*>(
            aIter.First( TYPE( SwXReferenceMark::Impl ) ));
    while (pXMark)
    {
        if (pXMark->m_pMarkFmt == &rMarkFmt)
        {
            return &pXMark->m_rThis;
        }
        pXMark = static_cast<SwXReferenceMark::Impl*>(aIter.Next());
    }
#endif
    return 0;
}

SwXReferenceMark *
SwXReferenceMark::CreateXReferenceMark(
        SwDoc & rDoc, SwFmtRefMark const& rMarkFmt)
{
    SwXReferenceMark *const pXMark(
        GetReferenceMark(*rDoc.GetUnoCallBack(), rMarkFmt) );
    return (pXMark)
        ?   pXMark
        :   new SwXReferenceMark(&rDoc, &rMarkFmt);
}

/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXReferenceMark::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL
SwXReferenceMark::getSomething(const uno::Sequence< sal_Int8 >& rId)
throw (uno::RuntimeException)
{
    return ::sw::UnoTunnelImpl<SwXReferenceMark>(rId, this);
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
OUString SAL_CALL SwXReferenceMark::getImplementationName()
throw (uno::RuntimeException)
{
    return C2U("SwXReferenceMark");
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
static char const*const g_ServicesReferenceMark[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.ReferenceMark",
};
static const size_t g_nServicesReferenceMark(
    sizeof(g_ServicesReferenceMark)/sizeof(g_ServicesReferenceMark[0]));

sal_Bool SAL_CALL
SwXReferenceMark::supportsService(const OUString& rServiceName)
throw (uno::RuntimeException)
{
    return ::sw::SupportsServiceImpl(
            g_nServicesReferenceMark, g_ServicesReferenceMark, rServiceName);
}
/* -----------------------------06.04.00 16:41--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SAL_CALL
SwXReferenceMark::getSupportedServiceNames()
throw (uno::RuntimeException)
{
    return ::sw::GetSupportedServiceNamesImpl(
            g_nServicesReferenceMark, g_ServicesReferenceMark);
}

/* -----------------03.11.99 14:14-------------------

 --------------------------------------------------*/
void SwXReferenceMark::Impl::InsertRefMark(SwPaM& rPam,
        SwXTextCursor const*const pCursor)
{
    //! in some cases when this function is called the pDoc pointer member may have become
    //! invalid/deleted thus we obtain the document pointer from rPaM where it should always
    //! be valid.
    SwDoc *pDoc2 = rPam.GetDoc();

    UnoActionContext aCont(pDoc2);
    SwFmtRefMark aRefMark(m_sMarkName);
    sal_Bool bMark = *rPam.GetPoint() != *rPam.GetMark();

    const bool bForceExpandHints( (!bMark && pCursor)
            ? pCursor->IsAtEndOfMeta() : false );
    const SetAttrMode nInsertFlags = (bForceExpandHints)
        ?   ( nsSetAttrMode::SETATTR_FORCEHINTEXPAND
            | nsSetAttrMode::SETATTR_DONTEXPAND)
        : nsSetAttrMode::SETATTR_DONTEXPAND;

    pDoc2->InsertPoolItem( rPam, aRefMark, nInsertFlags );

    if( bMark && *rPam.GetPoint() > *rPam.GetMark())
    {
        rPam.Exchange();
    }

    SwTxtAttr *const pTxtAttr = (bMark)
        ? rPam.GetNode()->GetTxtNode()->GetTxtAttr(
                rPam.GetPoint()->nContent, RES_TXTATR_REFMARK)
        : rPam.GetNode()->GetTxtNode()->GetTxtAttrForCharAt(
                rPam.GetPoint()->nContent.GetIndex() - 1, RES_TXTATR_REFMARK);

    if(pTxtAttr)
    {
        m_pMarkFmt = &pTxtAttr->GetRefMark();
    }

    pDoc2->GetUnoCallBack()->Add(this);
}

/*-- 11.12.98 10:28:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXReferenceMark::attach(const uno::Reference< text::XTextRange > & xTextRange)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (!m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }
    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange = ::sw::UnoTunnelGetImplementation<SwXTextRange>(xRangeTunnel);
        pCursor =
            ::sw::UnoTunnelGetImplementation<OTextCursorHelper>(xRangeTunnel);
    }
    SwDoc *const pDocument =
        (pRange) ? pRange->GetDoc() : ((pCursor) ? pCursor->GetDoc() : 0);
    if (!pDocument)
    {
        throw lang::IllegalArgumentException();
    }

    SwUnoInternalPaM aPam(*pDocument);
    //das muss jetzt sal_True liefern
    ::sw::XTextRangeToSwPaM(aPam, xTextRange);
    m_pImpl->InsertRefMark(aPam, dynamic_cast<SwXTextCursor*>(pCursor));
    m_pImpl->m_bIsDescriptor = sal_False;
    m_pImpl->m_pDoc = pDocument;
    m_pImpl->m_pDoc->GetUnoCallBack()->Add(m_pImpl.get());
}

/*-- 11.12.98 10:28:34---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXReferenceMark::getAnchor() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (m_pImpl->IsValid())
    {
        SwFmtRefMark const*const pNewMark =
            m_pImpl->m_pDoc->GetRefMark(m_pImpl->m_sMarkName);
        if (pNewMark && (pNewMark == m_pImpl->m_pMarkFmt))
        {
            SwTxtRefMark const*const pTxtMark =
                m_pImpl->m_pMarkFmt->GetTxtRefMark();
            if (pTxtMark &&
                (&pTxtMark->GetTxtNode().GetNodes() ==
                    &m_pImpl->m_pDoc->GetNodes()))
            {
                SwTxtNode const& rTxtNode = pTxtMark->GetTxtNode();
                const ::std::auto_ptr<SwPaM> pPam( (pTxtMark->GetEnd())
                    ?   new SwPaM( rTxtNode, *pTxtMark->GetEnd(),
                                   rTxtNode, *pTxtMark->GetStart())
                    :   new SwPaM( rTxtNode, *pTxtMark->GetStart()) );

                return SwXTextRange::CreateXTextRange(
                            *m_pImpl->m_pDoc, *pPam->Start(), pPam->End());
            }
        }
    }
    return 0;
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::dispose() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if (m_pImpl->IsValid())
    {
        SwFmtRefMark const*const pNewMark =
            m_pImpl->m_pDoc->GetRefMark(m_pImpl->m_sMarkName);
        if (pNewMark && (pNewMark == m_pImpl->m_pMarkFmt))
        {
            SwTxtRefMark const*const pTxtMark =
                m_pImpl->m_pMarkFmt->GetTxtRefMark();
            if (pTxtMark &&
                (&pTxtMark->GetTxtNode().GetNodes() ==
                    &m_pImpl->m_pDoc->GetNodes()))
            {
                SwTxtNode const& rTxtNode = pTxtMark->GetTxtNode();
                xub_StrLen nStt = *pTxtMark->GetStart(),
                           nEnd = pTxtMark->GetEnd() ? *pTxtMark->GetEnd()
                                                     : nStt + 1;

                SwPaM aPam( rTxtNode, nStt, rTxtNode, nEnd );
                m_pImpl->m_pDoc->DeleteAndJoin( aPam );
            }
        }
    }
    else if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->Invalidate();
    }
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::addEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->IsValid())
    {
        throw uno::RuntimeException();
    }
    m_pImpl->m_ListenerContainer.AddListener(xListener);
}
/*-- 11.12.98 10:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::removeEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->IsValid() ||
        !m_pImpl->m_ListenerContainer.RemoveListener(xListener))
    {
        throw uno::RuntimeException();
    }
}
/*-- 11.12.98 10:28:36---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SAL_CALL SwXReferenceMark::getName()
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if (!m_pImpl->IsValid() ||
        !m_pImpl->m_pDoc->GetRefMark(m_pImpl->m_sMarkName))
    {
        throw uno::RuntimeException();
    }
    return m_pImpl->m_sMarkName;
}
/*-- 11.12.98 10:28:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::setName(const OUString& rName)
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->m_sMarkName = rName;
    }
    else
    {
        if (!m_pImpl->IsValid()
            || !m_pImpl->m_pDoc->GetRefMark(m_pImpl->m_sMarkName)
            || m_pImpl->m_pDoc->GetRefMark(rName))
        {
            throw uno::RuntimeException();
        }
        SwFmtRefMark const*const pCurMark =
            m_pImpl->m_pDoc->GetRefMark(m_pImpl->m_sMarkName);
        if ((rName != m_pImpl->m_sMarkName)
            && pCurMark && (pCurMark == m_pImpl->m_pMarkFmt))
        {
            const UnoActionContext aCont(m_pImpl->m_pDoc);
            SwTxtRefMark const*const pTxtMark =
                m_pImpl->m_pMarkFmt->GetTxtRefMark();
            if (pTxtMark &&
                (&pTxtMark->GetTxtNode().GetNodes() ==
                     &m_pImpl->m_pDoc->GetNodes()))
            {
                SwTxtNode const& rTxtNode = pTxtMark->GetTxtNode();
                xub_StrLen nStt = *pTxtMark->GetStart(),
                           nEnd = pTxtMark->GetEnd() ? *pTxtMark->GetEnd()
                                                     : nStt + 1;

                SwPaM aPam( rTxtNode, nStt, rTxtNode, nEnd );
                // deletes the m_pImpl->m_pDoc member in the SwXReferenceMark!
                m_pImpl->m_pDoc->DeleteAndJoin( aPam );
                // The aPam will keep the correct and functional doc though

                m_pImpl->m_sMarkName = rName;
                //create a new one
                m_pImpl->InsertRefMark( aPam, 0 );
                m_pImpl->m_pDoc = aPam.GetDoc();
            }
        }
    }
}

/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SAL_CALL
SwXReferenceMark::getPropertySetInfo() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    static uno::Reference< beans::XPropertySetInfo >  xRef =
        aSwMapProvider.GetPropertySet(PROPERTY_MAP_PARAGRAPH_EXTENSIONS)
            ->getPropertySetInfo();
    return xRef;
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::setPropertyValue(
    const OUString& /*rPropertyName*/, const uno::Any& /*rValue*/ )
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
    lang::IllegalArgumentException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    throw lang::IllegalArgumentException();
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
SwXReferenceMark::getPropertyValue(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    // does not seem to need SolarMutex
    uno::Any aRet;
    if (! ::sw::GetDefaultTextContentValue(aRet, rPropertyName))
    {
        throw beans::UnknownPropertyException();
    }
    return aRet;
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::addPropertyChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXReferenceMark::addPropertyChangeListener(): not implemented");
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::removePropertyChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXReferenceMark::removePropertyChangeListener(): not implemented");
}
/*-- 12.09.00 12:58:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::addVetoableChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXReferenceMark::addVetoableChangeListener(): not implemented");
}
/*-- 12.09.00 12:58:21---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXReferenceMark::removeVetoableChangeListener(
    const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXReferenceMark::removeVetoableChangeListener(): not implemented");
}

#include <com/sun/star/lang/DisposedException.hpp>
#include <unometa.hxx>
#include <unotext.hxx>
#include <unoport.hxx>
#include <txtatr.hxx>
#include <fmtmeta.hxx>
#include <docsh.hxx>

//=============================================================================

/******************************************************************
 * SwXMetaText
 ******************************************************************/

class SwXMetaText
    : public SwXText
{
private:
    SwXMeta & m_rMeta;

    virtual void PrepareForAttach(uno::Reference< text::XTextRange > & xRange,
            const SwPaM & rPam);

    virtual bool CheckForOwnMemberMeta(const SwPaM & rPam, const bool bAbsorb)
        throw (lang::IllegalArgumentException, uno::RuntimeException);

protected:
    virtual const SwStartNode *GetStartNode() const;
    virtual uno::Reference< text::XTextCursor >
        createCursor() throw (uno::RuntimeException);

public:
    SwXMetaText(SwDoc & rDoc, SwXMeta & rMeta);

    // XInterface
    virtual void SAL_CALL acquire() throw()
        { OSL_ENSURE(false, "ERROR: SwXMetaText::acquire"); }
    virtual void SAL_CALL release() throw()
        { OSL_ENSURE(false, "ERROR: SwXMetaText::release"); }

    // XTypeProvider
    virtual uno::Sequence< sal_Int8 > SAL_CALL
        getImplementationId() throw (uno::RuntimeException);

    // XText
    virtual uno::Reference< text::XTextCursor >  SAL_CALL
        createTextCursor() throw (uno::RuntimeException);
    virtual uno::Reference< text::XTextCursor >  SAL_CALL
        createTextCursorByRange(
            const uno::Reference< text::XTextRange > & xTextPosition)
        throw (uno::RuntimeException);

    SwXMeta & GetXMeta() { return m_rMeta; }

};

SwXMetaText::SwXMetaText(SwDoc & rDoc, SwXMeta & rMeta)
    : SwXText(&rDoc, CURSOR_META)
    , m_rMeta(rMeta)
{
}

const SwStartNode *SwXMetaText::GetStartNode() const
{
    SwXText const * const pParent(
            dynamic_cast<SwXText*>(m_rMeta.GetParentText().get()));
    return (pParent) ? pParent->GetStartNode() : 0;
}

void SwXMetaText::PrepareForAttach( uno::Reference<text::XTextRange> & xRange,
        const SwPaM & rPam)
{
    // create a new cursor to prevent modifying SwXTextRange
    xRange = static_cast<text::XWordCursor*>(
        new SwXTextCursor(&m_rMeta, *rPam.GetPoint(), CURSOR_META,
                GetDoc(), (rPam.HasMark()) ? rPam.GetMark() : 0));
}

bool SwXMetaText::CheckForOwnMemberMeta(const SwPaM & rPam, const bool bAbsorb)
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    return m_rMeta.CheckForOwnMemberMeta(rPam, bAbsorb);
}

uno::Reference< text::XTextCursor > SwXMetaText::createCursor()
throw (uno::RuntimeException)
{
    uno::Reference< text::XTextCursor > xRet;
    if (IsValid())
    {
        SwTxtNode * pTxtNode;
        xub_StrLen nMetaStart;
        xub_StrLen nMetaEnd;
        const bool bSuccess(
                m_rMeta.SetContentRange(pTxtNode, nMetaStart, nMetaEnd) );
        if (bSuccess)
        {
            SwPosition aPos(*pTxtNode, nMetaStart);
            xRet = static_cast<text::XWordCursor*>(
                    new SwXTextCursor(&m_rMeta, aPos, CURSOR_META, GetDoc()));
        }
    }
    return xRet;
}

uno::Sequence<sal_Int8> SAL_CALL
SwXMetaText::getImplementationId() throw (uno::RuntimeException)
{
    return m_rMeta.getImplementationId();
}

// XText
uno::Reference< text::XTextCursor > SAL_CALL
SwXMetaText::createTextCursor() throw (uno::RuntimeException)
{
    return createCursor();
}

uno::Reference< text::XTextCursor > SAL_CALL
SwXMetaText::createTextCursorByRange(
        const uno::Reference<text::XTextRange> & xTextPosition)
    throw (uno::RuntimeException)
{
    const uno::Reference<text::XTextCursor> xCursor( createCursor() );
    xCursor->gotoRange(xTextPosition, sal_False);
    return xCursor;
}

/******************************************************************
 * SwXMeta
 ******************************************************************/

// the Meta has a cached list of text portions for its contents
// this list is created by SwXTextPortionEnumeration
// the Meta listens at the SwTxtNode and throws away the cache when it changes

class SwXMeta::Impl
    : public SwClient
{

public:

    SwEventListenerContainer m_ListenerContainer;
    ::std::auto_ptr<const TextRangeList_t> m_pTextPortions;
    // 3 possible states: not attached, attached, disposed
    bool m_bIsDisposed;
    bool m_bIsDescriptor;
    uno::Reference<text::XText> m_xParentText;
    SwXMetaText m_Text;

    Impl(   SwXMeta & rThis, SwDoc & rDoc,
            ::sw::Meta * const pMeta,
            uno::Reference<text::XText> const& xParentText,
            TextRangeList_t const * const pPortions)
        : SwClient(pMeta)
        , m_ListenerContainer(static_cast< ::cppu::OWeakObject* >(&rThis))
        , m_pTextPortions( pPortions )
        , m_bIsDisposed( false )
        , m_bIsDescriptor(0 == pMeta)
        , m_xParentText(xParentText)
        , m_Text(rDoc, rThis)
    {
    }

    inline const ::sw::Meta * GetMeta() const;
    // only for SwXMetaField!
    inline const ::sw::MetaField * GetMetaField() const;

    // SwClient
    virtual void Modify(SfxPoolItem *pOld, SfxPoolItem *pNew);

};

inline const ::sw::Meta * SwXMeta::Impl::GetMeta() const
{
    return static_cast< const ::sw::Meta * >(GetRegisteredIn());
}

// SwModify
void SwXMeta::Impl::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew )
{
    m_pTextPortions.reset(); // throw away cache (SwTxtNode changed)

    ClientModify(this, pOld, pNew);

    if (!GetRegisteredIn()) // removed => dispose
    {
        m_ListenerContainer.Disposing();
        m_bIsDisposed = true;
        m_Text.Invalidate();
    }
}

uno::Reference<text::XText> SwXMeta::GetParentText() const
{
    return m_pImpl->m_xParentText;
}

SwXMeta::SwXMeta(SwDoc *const pDoc, ::sw::Meta *const pMeta,
        uno::Reference<text::XText> const& xParentText,
        TextRangeList_t const*const pPortions)
    : m_pImpl( new SwXMeta::Impl(*this, *pDoc, pMeta, xParentText, pPortions) )
{
}

SwXMeta::SwXMeta(SwDoc *const pDoc)
    : m_pImpl( new SwXMeta::Impl(*this, *pDoc, 0, 0, 0) )
{
}

SwXMeta::~SwXMeta()
{
}

uno::Reference<rdf::XMetadatable>
SwXMeta::CreateXMeta(::sw::Meta & rMeta,
            uno::Reference<text::XText> const& i_xParent,
            ::std::auto_ptr<TextRangeList_t const> pPortions)
{
    // re-use existing SwXMeta
    // #i105557#: do not iterate over the registered clients: race condition
    uno::Reference<rdf::XMetadatable> xMeta(rMeta.GetXMeta());
    if (xMeta.is())
    {
        if (pPortions.get()) // set cache in the XMeta to the given portions
        {
            const uno::Reference<lang::XUnoTunnel> xUT(xMeta, uno::UNO_QUERY);
            SwXMeta *const pXMeta(
                ::sw::UnoTunnelGetImplementation<SwXMeta>(xUT));
            OSL_ENSURE(pXMeta, "no pXMeta?");
            // NB: the meta must always be created with the complete content
            // if SwXTextPortionEnumeration is created for a selection,
            // it must be checked that the Meta is contained in the selection!
            pXMeta->m_pImpl->m_pTextPortions = pPortions;
            // ??? is this necessary?
            if (pXMeta->m_pImpl->m_xParentText.get() != i_xParent.get())
            {
                OSL_ENSURE(false, "SwXMeta with different parent?");
                pXMeta->m_pImpl->m_xParentText.set(i_xParent);
            }
        }
        return xMeta;
    }

    // create new SwXMeta
    SwTxtNode * const pTxtNode( rMeta.GetTxtNode() );
    OSL_ENSURE(pTxtNode, "CreateXMeta: no text node?");
    if (!pTxtNode) { return 0; }
    uno::Reference<text::XText> xParentText(i_xParent);
    if (!xParentText.is())
    {
        SwTxtMeta * const pTxtAttr( rMeta.GetTxtAttr() );
        OSL_ENSURE(pTxtAttr, "CreateXMeta: no text attr?");
        if (!pTxtAttr) { return 0; }
        const SwPosition aPos(*pTxtNode, *pTxtAttr->GetStart());
        xParentText.set( ::sw::CreateParentXText(*pTxtNode->GetDoc(), aPos) );
    }
    if (!xParentText.is()) { return 0; }
    SwXMeta *const pXMeta( (RES_TXTATR_META == rMeta.GetFmtMeta()->Which())
        ? new SwXMeta     (pTxtNode->GetDoc(), &rMeta, xParentText,
                            pPortions.release()) // temporarily un-auto_ptr :-(
        : new SwXMetaField(pTxtNode->GetDoc(), &rMeta, xParentText,
                            pPortions.release()));
    // this is why the constructor is private: need to acquire pXMeta here
    xMeta.set(pXMeta);
    // in order to initialize the weak pointer cache in the core object
    rMeta.SetXMeta(xMeta);
    return xMeta;
}


bool SwXMeta::SetContentRange(
        SwTxtNode *& rpNode, xub_StrLen & rStart, xub_StrLen & rEnd ) const
{
    ::sw::Meta const * const pMeta( m_pImpl->GetMeta() );
    if (pMeta)
    {
        SwTxtMeta const * const pTxtAttr( pMeta->GetTxtAttr() );
        if (pTxtAttr)
        {
            rpNode = pTxtAttr->GetTxtNode();
            if (rpNode)
            {
                // rStart points at the first position _within_ the meta!
                rStart = *pTxtAttr->GetStart() + 1;
                rEnd = *pTxtAttr->GetEnd();
                return true;
            }
        }
    }
    return false;
}

bool SwXMeta::CheckForOwnMemberMeta(const SwPaM & rPam, const bool bAbsorb)
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    SwTxtNode * pTxtNode;
    xub_StrLen nMetaStart;
    xub_StrLen nMetaEnd;
    const bool bSuccess( SetContentRange(pTxtNode, nMetaStart, nMetaEnd) );
    ASSERT(bSuccess, "no pam?");
    if (!bSuccess)
        throw lang::DisposedException();

    SwPosition const * const pStartPos( rPam.Start() );
    if (&pStartPos->nNode.GetNode() != pTxtNode)
    {
        throw lang::IllegalArgumentException(
            C2U("trying to insert into a nesting text content, but start "
                "of text range not in same paragraph as text content"),
                0, 0);
    }
    bool bForceExpandHints(false);
    const xub_StrLen nStartPos(pStartPos->nContent.GetIndex());
    // not <= but < because nMetaStart is behind dummy char!
    // not >= but > because == means insert at end!
    if ((nStartPos < nMetaStart) || (nStartPos > nMetaEnd))
    {
        throw lang::IllegalArgumentException(
            C2U("trying to insert into a nesting text content, but start "
                "of text range not inside text content"),
                0, 0);
    }
    else if (nStartPos == nMetaEnd)
    {
        bForceExpandHints = true;
    }
    if (rPam.HasMark() && bAbsorb)
    {
        SwPosition const * const pEndPos( rPam.End() );
        if (&pEndPos->nNode.GetNode() != pTxtNode)
        {
            throw lang::IllegalArgumentException(
                C2U("trying to insert into a nesting text content, but end "
                    "of text range not in same paragraph as text content"),
                    0, 0);
        }
        const xub_StrLen nEndPos(pEndPos->nContent.GetIndex());
        // not <= but < because nMetaStart is behind dummy char!
        // not >= but > because == means insert at end!
        if ((nEndPos < nMetaStart) || (nEndPos > nMetaEnd))
        {
            throw lang::IllegalArgumentException(
                C2U("trying to insert into a nesting text content, but end "
                    "of text range not inside text content"),
                    0, 0);
        }
        else if (nEndPos == nMetaEnd)
        {
            bForceExpandHints = true;
        }
    }
    return bForceExpandHints;
}

const uno::Sequence< sal_Int8 > & SwXMeta::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq( ::CreateUnoTunnelId() );
    return aSeq;
}

// XUnoTunnel
sal_Int64 SAL_CALL
SwXMeta::getSomething( const uno::Sequence< sal_Int8 > & i_rId )
throw (uno::RuntimeException)
{
    return ::sw::UnoTunnelImpl<SwXMeta>(i_rId, this);
}

// XServiceInfo
::rtl::OUString SAL_CALL
SwXMeta::getImplementationName() throw (uno::RuntimeException)
{
    return C2U("SwXMeta");
}

static char const*const g_ServicesMeta[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.InContentMetadata",
};
static const size_t g_nServicesMeta(
    sizeof(g_ServicesMeta)/sizeof(g_ServicesMeta[0]));

sal_Bool SAL_CALL
SwXMeta::supportsService(const ::rtl::OUString& rServiceName)
throw (uno::RuntimeException)
{
    return ::sw::SupportsServiceImpl(
            g_nServicesMeta, g_ServicesMeta, rServiceName);
}

uno::Sequence< ::rtl::OUString > SAL_CALL
SwXMeta::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return ::sw::GetSupportedServiceNamesImpl(g_nServicesMeta, g_ServicesMeta);
}


// XComponent
void SAL_CALL
SwXMeta::addEventListener(
        uno::Reference< lang::XEventListener> const & xListener )
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    m_pImpl->m_ListenerContainer.AddListener(xListener);
    if (m_pImpl->m_bIsDisposed)
    {
        m_pImpl->m_ListenerContainer.Disposing();
    }
}

void SAL_CALL
SwXMeta::removeEventListener(
        uno::Reference< lang::XEventListener> const & xListener )
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->m_bIsDisposed)
    {
        m_pImpl->m_ListenerContainer.RemoveListener(xListener);
    }
}

void SAL_CALL
SwXMeta::dispose() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->m_pTextPortions.reset();
        m_pImpl->m_ListenerContainer.Disposing();
        m_pImpl->m_bIsDisposed = true;
        m_pImpl->m_Text.Invalidate();
    }
    else if (!m_pImpl->m_bIsDisposed)
    {
        SwTxtNode * pTxtNode;
        xub_StrLen nMetaStart;
        xub_StrLen nMetaEnd;
        const bool bSuccess(SetContentRange(pTxtNode, nMetaStart, nMetaEnd));
        ASSERT(bSuccess, "no pam?");
        if (bSuccess)
        {
            // -1 because of CH_TXTATR
            SwPaM aPam( *pTxtNode, nMetaStart - 1, *pTxtNode, nMetaEnd );
            SwDoc * const pDoc( pTxtNode->GetDoc() );
            pDoc->DeleteAndJoin( aPam );

            // removal should call Modify and do the dispose
            OSL_ENSURE(m_pImpl->m_bIsDisposed, "zombie meta");
        }
    }
}


void SAL_CALL
SwXMeta::AttachImpl(const uno::Reference< text::XTextRange > & i_xTextRange,
        const USHORT i_nWhich)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (m_pImpl->m_bIsDisposed)
    {
        throw lang::DisposedException();
    }
    if (!m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException(
            C2S("SwXMeta::attach(): already attached"),
                static_cast< ::cppu::OWeakObject* >(this));
    }

    uno::Reference<lang::XUnoTunnel> xRangeTunnel(i_xTextRange, uno::UNO_QUERY);
    if (!xRangeTunnel.is())
    {
        throw lang::IllegalArgumentException(
            C2S("SwXMeta::attach(): argument is no XUnoTunnel"),
                static_cast< ::cppu::OWeakObject* >(this), 0);
    }
    SwXTextRange *const pRange(
            ::sw::UnoTunnelGetImplementation<SwXTextRange>(xRangeTunnel));
    OTextCursorHelper *const pCursor( (pRange) ? 0 :
            ::sw::UnoTunnelGetImplementation<OTextCursorHelper>(xRangeTunnel));
    if (!pRange && !pCursor)
    {
        throw lang::IllegalArgumentException(
            C2S("SwXMeta::attach(): argument not supported type"),
                static_cast< ::cppu::OWeakObject* >(this), 0);
    }

    SwDoc * const pDoc(
            pRange ? pRange->GetDoc() : pCursor ? pCursor->GetDoc() : 0 );
    if (!pDoc)
    {
        throw lang::IllegalArgumentException(
            C2S("SwXMeta::attach(): argument has no SwDoc"),
                static_cast< ::cppu::OWeakObject* >(this), 0);
    }

    SwUnoInternalPaM aPam(*pDoc);
    ::sw::XTextRangeToSwPaM(aPam, i_xTextRange);

    UnoActionContext aContext(pDoc);

    SwXTextCursor const*const pTextCursor(
            dynamic_cast<SwXTextCursor*>(pCursor));
    const bool bForceExpandHints((pTextCursor)
            ? pTextCursor->IsAtEndOfMeta() : false);
    const SetAttrMode nInsertFlags( (bForceExpandHints)
        ?   ( nsSetAttrMode::SETATTR_FORCEHINTEXPAND
            | nsSetAttrMode::SETATTR_DONTEXPAND)
        : nsSetAttrMode::SETATTR_DONTEXPAND );

    const ::boost::shared_ptr< ::sw::Meta> pMeta( (RES_TXTATR_META == i_nWhich)
        ? ::boost::shared_ptr< ::sw::Meta>( new ::sw::Meta() )
        : ::boost::shared_ptr< ::sw::Meta>(
            pDoc->GetMetaFieldManager().makeMetaField()) );
    SwFmtMeta meta(pMeta, i_nWhich); // this is cloned by Insert!
    const bool bSuccess( pDoc->InsertPoolItem( aPam, meta, nInsertFlags ) );
    SwTxtAttr * const pTxtAttr( pMeta->GetTxtAttr() );
    if (!bSuccess)
    {
        throw lang::IllegalArgumentException(
            C2S("SwXMeta::attach(): cannot create meta: range invalid?"),
                static_cast< ::cppu::OWeakObject* >(this), 1);
    }
    if (!pTxtAttr)
    {
        ASSERT(false, "meta inserted, but has no text attribute?");
        throw uno::RuntimeException(
            C2S("SwXMeta::attach(): cannot create meta"),
                static_cast< ::cppu::OWeakObject* >(this));
    }

    pMeta->Add(m_pImpl.get());
    pMeta->SetXMeta(uno::Reference<rdf::XMetadatable>(this));

    m_pImpl->m_xParentText = ::sw::CreateParentXText(*pDoc, *aPam.GetPoint());

    m_pImpl->m_bIsDescriptor = false;
}

// XTextContent
void SAL_CALL
SwXMeta::attach(const uno::Reference< text::XTextRange > & i_xTextRange)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    return SwXMeta::AttachImpl(i_xTextRange, RES_TXTATR_META);
}

uno::Reference< text::XTextRange > SAL_CALL
SwXMeta::getAnchor() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (m_pImpl->m_bIsDisposed)
    {
        throw lang::DisposedException();
    }
    if (m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException(
                C2S("SwXMeta::getAnchor(): not inserted"),
                static_cast< ::cppu::OWeakObject* >(this));
    }

    SwTxtNode * pTxtNode;
    xub_StrLen nMetaStart;
    xub_StrLen nMetaEnd;
    const bool bSuccess(SetContentRange(pTxtNode, nMetaStart, nMetaEnd));
    ASSERT(bSuccess, "no pam?");
    if (!bSuccess)
    {
        throw lang::DisposedException(
                C2S("SwXMeta::getAnchor(): not attached"),
                static_cast< ::cppu::OWeakObject* >(this));
    }

    const SwPosition start(*pTxtNode, nMetaStart - 1); // -1 due to CH_TXTATR
    const SwPosition end(*pTxtNode, nMetaEnd);
    return SwXTextRange::CreateXTextRange(*pTxtNode->GetDoc(), start, &end);
}

// XTextRange
uno::Reference< text::XText > SAL_CALL
SwXMeta::getText() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    //TODO probably this should return outer meta in case there is nesting,
    // but currently that is not done; would need to change at least
    // SwXTextPortionEnumeration and SwXMeta::attach and other places where
    // SwXMeta is constructed
    return GetParentText();
}

uno::Reference< text::XTextRange > SAL_CALL
SwXMeta::getStart() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.getStart();
}

uno::Reference< text::XTextRange > SAL_CALL
SwXMeta::getEnd() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.getEnd();
}

rtl::OUString SAL_CALL
SwXMeta::getString() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.getString();
}

void SAL_CALL
SwXMeta::setString(const rtl::OUString& rString) throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.setString(rString);
}

// XSimpleText
uno::Reference< text::XTextCursor > SAL_CALL
SwXMeta::createTextCursor() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.createTextCursor();
}

uno::Reference< text::XTextCursor > SAL_CALL
SwXMeta::createTextCursorByRange(
        const uno::Reference<text::XTextRange> & xTextPosition)
    throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.createTextCursorByRange(xTextPosition);
}

void SAL_CALL
SwXMeta::insertString(const uno::Reference<text::XTextRange> & xRange,
        const rtl::OUString& rString, sal_Bool bAbsorb)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.insertString(xRange, rString, bAbsorb);
}

void SAL_CALL
SwXMeta::insertControlCharacter(const uno::Reference<text::XTextRange> & xRange,
        sal_Int16 nControlCharacter, sal_Bool bAbsorb)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.insertControlCharacter(xRange, nControlCharacter,
                bAbsorb);
}

// XText
void SAL_CALL
SwXMeta::insertTextContent( const uno::Reference<text::XTextRange> & xRange,
        const uno::Reference<text::XTextContent> & xContent, sal_Bool bAbsorb)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.insertTextContent(xRange, xContent, bAbsorb);
}

void SAL_CALL
SwXMeta::removeTextContent(
        const uno::Reference< text::XTextContent > & xContent)
    throw (container::NoSuchElementException, uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());
    return m_pImpl->m_Text.removeTextContent(xContent);
}

// XElementAccess
uno::Type SAL_CALL
SwXMeta::getElementType() throw (uno::RuntimeException)
{
    return text::XTextRange::static_type();
}

sal_Bool SAL_CALL
SwXMeta::hasElements() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    return m_pImpl->GetRegisteredIn() ? sal_True : sal_False;
}

// XEnumerationAccess
uno::Reference< container::XEnumeration > SAL_CALL
SwXMeta::createEnumeration() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (m_pImpl->m_bIsDisposed)
    {
        throw lang::DisposedException();
    }
    if (m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException(
                C2S("createEnumeration(): not inserted"),
                static_cast< ::cppu::OWeakObject* >(this));
    }

    SwTxtNode * pTxtNode;
    xub_StrLen nMetaStart;
    xub_StrLen nMetaEnd;
    const bool bSuccess(SetContentRange(pTxtNode, nMetaStart, nMetaEnd));
    ASSERT(bSuccess, "no pam?");
    if (!bSuccess)
        throw lang::DisposedException();

    SwPaM aPam(*pTxtNode, nMetaStart);

    if (!m_pImpl->m_pTextPortions.get())
    {
        return new SwXTextPortionEnumeration(
                    aPam, GetParentText(), nMetaStart, nMetaEnd);
    }
    else // cached!
    {
        return new SwXTextPortionEnumeration(aPam, *m_pImpl->m_pTextPortions);
    }
}


// MetadatableMixin
::sfx2::Metadatable* SwXMeta::GetCoreObject()
{
    return const_cast< ::sw::Meta * >(m_pImpl->GetMeta());
}

uno::Reference<frame::XModel> SwXMeta::GetModel()
{
    ::sw::Meta const * const pMeta( m_pImpl->GetMeta() );
    if (pMeta)
    {
        SwTxtNode const * const pTxtNode( pMeta->GetTxtNode() );
        if (pTxtNode)
        {
            SwDocShell const * const pShell(pTxtNode->GetDoc()->GetDocShell());
            return (pShell) ? pShell->GetModel() : 0;
        }
    }
    return 0;
}


/******************************************************************
 * SwXMetaField
 ******************************************************************/

inline const ::sw::MetaField * SwXMeta::Impl::GetMetaField() const
{
    return static_cast< const ::sw::MetaField * >(GetRegisteredIn());
}

SwXMetaField::SwXMetaField(SwDoc *const pDoc, ::sw::Meta *const pMeta,
        uno::Reference<text::XText> const& xParentText,
        TextRangeList_t const*const pPortions)
    : SwXMetaField_Base(pDoc, pMeta, xParentText, pPortions)
{
    ASSERT(pMeta && dynamic_cast< ::sw::MetaField* >(pMeta),
        "SwXMetaField created for wrong hint!");
}

SwXMetaField::SwXMetaField(SwDoc *const pDoc)
    :  SwXMetaField_Base(pDoc)
{
}

SwXMetaField::~SwXMetaField()
{
}

// XServiceInfo
::rtl::OUString SAL_CALL
SwXMetaField::getImplementationName() throw (uno::RuntimeException)
{
    return C2U("SwXMetaField");
}

static char const*const g_ServicesMetaField[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.TextField",
    "com.sun.star.text.textfield.MetadataField",
};
static const size_t g_nServicesMetaField(
    sizeof(g_ServicesMetaField)/sizeof(g_ServicesMetaField[0]));

sal_Bool SAL_CALL
SwXMetaField::supportsService(const ::rtl::OUString& rServiceName)
throw (uno::RuntimeException)
{
    return ::sw::SupportsServiceImpl(
            g_nServicesMetaField, g_ServicesMetaField, rServiceName);
}

uno::Sequence< ::rtl::OUString > SAL_CALL
SwXMetaField::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return ::sw::GetSupportedServiceNamesImpl(
            g_nServicesMetaField, g_ServicesMetaField);
}

// XComponent
void SAL_CALL
SwXMetaField::addEventListener(
        uno::Reference< lang::XEventListener> const & xListener )
throw (uno::RuntimeException)
{
    return SwXMeta::addEventListener(xListener);
}

void SAL_CALL
SwXMetaField::removeEventListener(
        uno::Reference< lang::XEventListener> const & xListener )
throw (uno::RuntimeException)
{
    return SwXMeta::removeEventListener(xListener);
}

void SAL_CALL
SwXMetaField::dispose() throw (uno::RuntimeException)
{
    return SwXMeta::dispose();
}

// XTextContent
void SAL_CALL
SwXMetaField::attach(const uno::Reference< text::XTextRange > & i_xTextRange)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    return SwXMeta::AttachImpl(i_xTextRange, RES_TXTATR_METAFIELD);
}

uno::Reference< text::XTextRange > SAL_CALL
SwXMetaField::getAnchor() throw (uno::RuntimeException)
{
    return SwXMeta::getAnchor();
}

// XPropertySet
uno::Reference< beans::XPropertySetInfo > SAL_CALL
SwXMetaField::getPropertySetInfo() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    static uno::Reference< beans::XPropertySetInfo > xRef(
        aSwMapProvider.GetPropertySet(PROPERTY_MAP_METAFIELD)
            ->getPropertySetInfo() );
    return xRef;
}

void SAL_CALL
SwXMetaField::setPropertyValue(
        const ::rtl::OUString& rPropertyName, const uno::Any& rValue)
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
    lang::IllegalArgumentException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    ::sw::MetaField * const pMeta(
            const_cast< ::sw::MetaField * >(m_pImpl->GetMetaField()) );
    if (!pMeta)
        throw lang::DisposedException();

    if (rPropertyName.equalsAscii("NumberFormat"))
    {
        sal_Int32 nNumberFormat(0);
        if (rValue >>= nNumberFormat)
        {
            pMeta->SetNumberFormat(static_cast<sal_uInt32>(nNumberFormat));
        }
    }
    else if (rPropertyName.equalsAscii("IsFixedLanguage"))
    {
        bool b(false);
        if (rValue >>= b)
        {
            pMeta->SetIsFixedLanguage(b);
        }
    }
    else
    {
        throw beans::UnknownPropertyException();
    }
}

uno::Any SAL_CALL
SwXMetaField::getPropertyValue(const ::rtl::OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    ::sw::MetaField const * const pMeta( m_pImpl->GetMetaField() );
    if (!pMeta)
        throw lang::DisposedException();

    uno::Any any;

    if (rPropertyName.equalsAscii("NumberFormat"))
    {
        const ::rtl::OUString text( getPresentation(sal_False) );
        any <<= static_cast<sal_Int32>(pMeta->GetNumberFormat(text));
    }
    else if (rPropertyName.equalsAscii("IsFixedLanguage"))
    {
        any <<= pMeta->IsFixedLanguage();
    }
    else
    {
        throw beans::UnknownPropertyException();
    }

    return any;
}

void SAL_CALL
SwXMetaField::addPropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXMetaField::addPropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXMetaField::removePropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXMetaField::removePropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXMetaField::addVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXMetaField::addVetoableChangeListener(): not implemented");
}

void SAL_CALL
SwXMetaField::removeVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXMetaField::removeVetoableChangeListener(): not implemented");
}

#include <com/sun/star/lang/WrappedTargetRuntimeException.hpp>
#include <com/sun/star/rdf/Statement.hpp>
#include <com/sun/star/rdf/URI.hpp>
#include <com/sun/star/rdf/URIs.hpp>
#include <com/sun/star/rdf/XLiteral.hpp>
#include <com/sun/star/rdf/XRepositorySupplier.hpp>
#include <comphelper/processfactory.hxx>

static uno::Reference<rdf::XURI> const&
lcl_getURI(const bool bPrefix)
{
    static uno::Reference< uno::XComponentContext > xContext(
        ::comphelper::getProcessComponentContext());
    static uno::Reference< rdf::XURI > xOdfPrefix(
        rdf::URI::createKnown(xContext, rdf::URIs::ODF_PREFIX),
        uno::UNO_SET_THROW);
    static uno::Reference< rdf::XURI > xOdfSuffix(
        rdf::URI::createKnown(xContext, rdf::URIs::ODF_SUFFIX),
        uno::UNO_SET_THROW);
    return (bPrefix) ? xOdfPrefix : xOdfSuffix;
}

static ::rtl::OUString
lcl_getPrefixOrSuffix(
    uno::Reference<rdf::XRepository> const & xRepository,
    uno::Reference<rdf::XResource> const & xMetaField,
    uno::Reference<rdf::XURI> const & xPredicate)
{
    const uno::Reference<container::XEnumeration> xEnum(
        xRepository->getStatements(xMetaField, xPredicate, 0),
        uno::UNO_SET_THROW);
    while (xEnum->hasMoreElements()) {
        rdf::Statement stmt;
        if (!(xEnum->nextElement() >>= stmt)) {
            throw uno::RuntimeException();
        }
        const uno::Reference<rdf::XLiteral> xObject(stmt.Object,
            uno::UNO_QUERY);
        if (!xObject.is()) continue;
        if (xEnum->hasMoreElements()) {
            OSL_TRACE("ignoring other odf:Prefix/odf:Suffix statements");
        }
        return xObject->getValue();
    }
    return ::rtl::OUString();
}

void
getPrefixAndSuffix(
        const uno::Reference<frame::XModel>& xModel,
        const uno::Reference<rdf::XMetadatable>& xMetaField,
        ::rtl::OUString *const o_pPrefix, ::rtl::OUString *const o_pSuffix)
{
    try {
        const uno::Reference<rdf::XRepositorySupplier> xRS(
                xModel, uno::UNO_QUERY_THROW);
        const uno::Reference<rdf::XRepository> xRepo(
                xRS->getRDFRepository(), uno::UNO_SET_THROW);
        const uno::Reference<rdf::XResource> xMeta(
                xMetaField, uno::UNO_QUERY_THROW);
        if (o_pPrefix)
        {
            *o_pPrefix = lcl_getPrefixOrSuffix(xRepo, xMeta, lcl_getURI(true));
        }
        if (o_pSuffix)
        {
            *o_pSuffix = lcl_getPrefixOrSuffix(xRepo, xMeta, lcl_getURI(false));
        }
    } catch (uno::RuntimeException &) {
        throw;
    } catch (uno::Exception & e) {
        throw lang::WrappedTargetRuntimeException(
            ::rtl::OUString::createFromAscii("getPrefixAndSuffix: exception"),
            0, uno::makeAny(e));
    }
}

// XTextField
::rtl::OUString SAL_CALL
SwXMetaField::getPresentation(sal_Bool bShowCommand)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (bShowCommand)
    {
//FIXME ?
        return ::rtl::OUString();
    }
    else
    {
        // getString should check if this is invalid
        const ::rtl::OUString content( this->getString() );
        ::rtl::OUString prefix;
        ::rtl::OUString suffix;
        getPrefixAndSuffix(GetModel(), this, &prefix, &suffix);
        return prefix + content + suffix;
    }
}

