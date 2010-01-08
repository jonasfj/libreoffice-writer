/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unosect.cxx,v $
 * $Revision: 1.60 $
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

#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/text/SectionFileLink.hpp>

#include <cmdid.h>
#include <hintids.hxx>
#include <tools/link.hxx>
#include <svtools/urihelper.hxx>
#include <svx/brshitem.hxx>
#include <svx/xmlcnitm.hxx>
#include <sfx2/linkmgr.hxx>
#include <sfx2/lnkbase.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <fmtclds.hxx>
#include <unotextrange.hxx>
#include <unosection.hxx>
#include <unoparagraph.hxx>
#include <unotextcursor.hxx>
#include <unoredline.hxx>
#include <redline.hxx>
#include <unomap.hxx>
#include <unocrsr.hxx>
#include <section.hxx>
#include <doc.hxx>
#include <docsh.hxx>
#include <sfx2/docfile.hxx>
#include <docary.hxx>
#include <swundo.hxx>
#include <hints.hxx>
#include <tox.hxx>
#include <unoidx.hxx>
#include <doctxm.hxx>
#include <fmtftntx.hxx>
#include <fmtclbl.hxx>
#include <fmtcntnt.hxx>
#include <svx/frmdiritem.hxx>
/* #109700# */
#include <svx/lrspitem.hxx>


using namespace ::com::sun::star;
using ::rtl::OUString;


/******************************************************************
 *
 ******************************************************************/
struct SwTextSectionProperties_Impl
{
    uno::Sequence<sal_Int8> m_Password;
    ::rtl::OUString  m_sCondition;
    ::rtl::OUString  m_sLinkFileName;
    ::rtl::OUString  m_sSectionFilter;
    ::rtl::OUString  m_sSectionRegion;

    ::std::auto_ptr<SwFmtCol>               m_pColItem;
    ::std::auto_ptr<SvxBrushItem>           m_pBrushItem;
    ::std::auto_ptr<SwFmtFtnAtTxtEnd>       m_pFtnItem;
    ::std::auto_ptr<SwFmtEndAtTxtEnd>       m_pEndItem;
    ::std::auto_ptr<SvXMLAttrContainerItem> m_pXMLAttr;
    ::std::auto_ptr<SwFmtNoBalancedColumns> m_pNoBalanceItem;
    ::std::auto_ptr<SvxFrameDirectionItem>  m_pFrameDirItem;
    ::std::auto_ptr<SvxLRSpaceItem>         m_pLRSpaceItem; // #109700#

    bool m_bDDE;
    bool m_bHidden;
    bool m_bCondHidden;
    bool m_bProtect;
    // --> FME 2004-06-22 #114856# edit in readonly sections
    bool m_bEditInReadonly;
    // <--
    bool m_bUpdateType;

    SwTextSectionProperties_Impl()
        : m_bDDE(false)
        , m_bHidden(false)
        , m_bCondHidden(false)
        , m_bProtect(false)
        // --> FME 2004-06-22 #114856# edit in readonly sections
        , m_bEditInReadonly(false)
        // <--
        , m_bUpdateType(true)
    {
    }

};


class SwXTextSection::Impl
    : public SwClient
{

public:

    SwXTextSection &            m_rThis;
    const SfxItemPropertySet &  m_rPropSet;
    SwEventListenerContainer    m_ListenerContainer;
    const bool                  m_bIndexHeader;
    bool                        m_bIsDescriptor;
    ::rtl::OUString             m_sName;
    ::std::auto_ptr<SwTextSectionProperties_Impl> m_pProps;

    Impl(   SwXTextSection & rThis,
            SwSectionFmt *const pFmt, const bool bIndexHeader)
        : SwClient(pFmt)
        , m_rThis(rThis)
        , m_rPropSet(*aSwMapProvider.GetPropertySet(PROPERTY_MAP_SECTION))
        , m_ListenerContainer(static_cast< ::cppu::OWeakObject* >(&rThis))
        , m_bIndexHeader(bIndexHeader)
        , m_bIsDescriptor(0 == pFmt)
        , m_pProps((pFmt) ? 0 : new SwTextSectionProperties_Impl())
    {
    }

    SwSectionFmt * GetSectionFmt() const
    {
        return static_cast<SwSectionFmt*>(const_cast<SwModify*>(
                    GetRegisteredIn()));
    }

    SwSectionFmt & GetSectionFmtOrThrow() const {
        SwSectionFmt *const pFmt( GetSectionFmt() );
        if (!pFmt) {
            throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "SwXTextSection: disposed or invalid")), 0);
        }
        return *pFmt;
    }

    void SAL_CALL SetPropertyValues_Impl(
            const uno::Sequence< ::rtl::OUString >& rPropertyNames,
            const uno::Sequence< uno::Any >& aValues)
        throw (beans::UnknownPropertyException, beans::PropertyVetoException,
                lang::IllegalArgumentException, lang::WrappedTargetException,
                uno::RuntimeException);
    uno::Sequence< uno::Any > SAL_CALL
        GetPropertyValues_Impl(
            const uno::Sequence< ::rtl::OUString >& rPropertyNames)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException,
                uno::RuntimeException);

    // SwClient
    virtual void    Modify(SfxPoolItem *pOld, SfxPoolItem *pNew);

};

/*-- 10.12.98 14:42:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextSection::Impl::Modify(SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if (!GetRegisteredIn())
    {
        m_ListenerContainer.Disposing();
    }
}

/*-- 20.12.2005 10:27:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSectionFmt * SwXTextSection::GetFmt() const
{
    return m_pImpl->GetSectionFmt();
}

/*-- 20.12.2005 09:56:36---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextSection >
SwXTextSection::CreateXTextSection(
        SwSectionFmt *const pFmt, const bool bIndexHeader)
{
    // re-use existing SwXTextSection
    // #i105557#: do not iterate over the registered clients: race condition
    uno::Reference< text::XTextSection > xSection;
    if (pFmt)
    {
        xSection.set(pFmt->GetXTextSection());
    }
    if ( !xSection.is() )
    {
        SwXTextSection *const pNew = new SwXTextSection(pFmt, bIndexHeader);
        xSection.set(pNew);
        if (pFmt)
        {
            pFmt->SetXTextSection(xSection);
        }
    }
    return xSection;
}

/*-- 10.12.98 14:47:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextSection::SwXTextSection(
        SwSectionFmt *const pFmt, const bool bIndexHeader)
    : m_pImpl( new SwXTextSection::Impl(*this, pFmt, bIndexHeader) )
{
}

/*-- 10.12.98 14:47:07---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextSection::~SwXTextSection()
{
}

/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextSection::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL
SwXTextSection::getSomething(const uno::Sequence< sal_Int8 >& rId)
throw (uno::RuntimeException)
{
    return ::sw::UnoTunnelImpl<SwXTextSection>(rId, this);
}

/*-- 10.12.98 14:47:08---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextSection > SAL_CALL
SwXTextSection::getParentSection() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt & rSectionFmt( m_pImpl->GetSectionFmtOrThrow() );

    SwSectionFmt *const pParentFmt = rSectionFmt.GetParent();
    const uno::Reference< text::XTextSection > xRet =
        (pParentFmt) ? CreateXTextSection(pParentFmt) : 0;
    return xRet;
}

/*-- 10.12.98 14:47:08---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Reference< text::XTextSection > > SAL_CALL
SwXTextSection::getChildSections() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt & rSectionFmt( m_pImpl->GetSectionFmtOrThrow() );

    SwSections aChildren;
    rSectionFmt.GetChildSections(aChildren, SORTSECT_NOT, sal_False);
    uno::Sequence<uno::Reference<text::XTextSection> > aSeq(aChildren.Count());
    uno::Reference< text::XTextSection > * pArray = aSeq.getArray();
    for (sal_uInt16 i = 0; i < aChildren.Count(); i++)
    {
        SwSectionFmt *const pChild = aChildren.GetObject(i)->GetFmt();
        pArray[i] = CreateXTextSection(pChild);
    }
    return aSeq;
}

/* -----------------18.02.99 13:31-------------------
 *
 * --------------------------------------------------*/
void SAL_CALL
SwXTextSection::attach(const uno::Reference< text::XTextRange > & xTextRange)
throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }

    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange  = ::sw::UnoTunnelGetImplementation<SwXTextRange>(xRangeTunnel);
        pCursor =
            ::sw::UnoTunnelGetImplementation<OTextCursorHelper>(xRangeTunnel);
    }

    SwDoc *const pDoc =
        (pRange) ? pRange->GetDoc() : ((pCursor) ? pCursor->GetDoc() : 0);
    if (!pDoc)
    {
        throw lang::IllegalArgumentException();
    }

    SwUnoInternalPaM aPam(*pDoc);
    //das muss jetzt sal_True liefern
    ::sw::XTextRangeToSwPaM(aPam, xTextRange);
    UnoActionContext aCont(pDoc);
    pDoc->StartUndo( UNDO_INSSECTION, NULL );

    if (!m_pImpl->m_sName.getLength())
    {
        m_pImpl->m_sName = C2U("TextSection");
    }
    SectionType eType = (m_pImpl->m_pProps->m_bDDE)
        ? DDE_LINK_SECTION
        : ((m_pImpl->m_pProps->m_sLinkFileName.getLength() ||
            m_pImpl->m_pProps->m_sSectionRegion.getLength())
                ?  FILE_LINK_SECTION : CONTENT_SECTION);
    // index header section?
    if (m_pImpl->m_bIndexHeader)
    {
        // caller wants an index header section, but will only
        // give him one if a) we are inside an index, and b) said
        // index doesn't yet have a header section.
        const SwTOXBase* pBase = aPam.GetDoc()->GetCurTOX(*aPam.Start());

        // are we inside an index?
        if (pBase)
        {
            // get all child sections
            SwSections aSectionsArr;
            static_cast<const SwTOXBaseSection*>(pBase)->GetFmt()->
                GetChildSections(aSectionsArr);

            // and search for current header section
            const sal_uInt16 nCount = aSectionsArr.Count();
            sal_Bool bHeaderPresent = sal_False;
            for(sal_uInt16 i = 0; i < nCount; i++)
            {
                bHeaderPresent |=
                    (aSectionsArr[i]->GetType() == TOX_HEADER_SECTION);
            }
            if (! bHeaderPresent)
            {
                eType = TOX_HEADER_SECTION;
            }
        }
    }

    String tmp(m_pImpl->m_sName);
    SwSection aSect(eType, pDoc->GetUniqueSectionName(&tmp));
    aSect.SetCondition(m_pImpl->m_pProps->m_sCondition);
    ::rtl::OUStringBuffer sLinkNameBuf(m_pImpl->m_pProps->m_sLinkFileName);
    sLinkNameBuf.append(sfx2::cTokenSeperator);
    sLinkNameBuf.append(m_pImpl->m_pProps->m_sSectionFilter);
    sLinkNameBuf.append(sfx2::cTokenSeperator);
    sLinkNameBuf.append(m_pImpl->m_pProps->m_sSectionRegion);
    aSect.SetLinkFileName(sLinkNameBuf.makeStringAndClear());

    aSect.SetHidden(m_pImpl->m_pProps->m_bHidden);
    aSect.SetProtect(m_pImpl->m_pProps->m_bProtect);
    // --> FME 2004-06-22 #114856# edit in readonly sections
    aSect.SetEditInReadonly(m_pImpl->m_pProps->m_bEditInReadonly);
    // <--

    SfxItemSet aSet(pDoc->GetAttrPool(),
                RES_COL, RES_COL,
                RES_BACKGROUND, RES_BACKGROUND,
                RES_FTN_AT_TXTEND, RES_FRAMEDIR,
                RES_LR_SPACE, RES_LR_SPACE, // #109700#
                RES_UNKNOWNATR_CONTAINER,RES_UNKNOWNATR_CONTAINER,
                0);
    if (m_pImpl->m_pProps->m_pBrushItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pBrushItem);
    }
    if (m_pImpl->m_pProps->m_pColItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pColItem);
    }
    if (m_pImpl->m_pProps->m_pFtnItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pFtnItem);
    }
    if (m_pImpl->m_pProps->m_pEndItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pEndItem);
    }
    if (m_pImpl->m_pProps->m_pXMLAttr.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pXMLAttr);
    }
    if (m_pImpl->m_pProps->m_pNoBalanceItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pNoBalanceItem);
    }
    if (m_pImpl->m_pProps->m_pFrameDirItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pFrameDirItem);
    }
    /* #109700# */
    if (m_pImpl->m_pProps->m_pLRSpaceItem.get())
    {
        aSet.Put(*m_pImpl->m_pProps->m_pLRSpaceItem);
    }
    // section password
    if (m_pImpl->m_pProps->m_Password.getLength() > 0)
    {
        aSect.SetPasswd(m_pImpl->m_pProps->m_Password);
    }

    SwSection *const pRet =
        pDoc->InsertSwSection( aPam, aSect, aSet.Count() ? &aSet : 0 );
    pRet->GetFmt()->Add(m_pImpl.get());
    pRet->GetFmt()->SetXObject(static_cast< ::cppu::OWeakObject*>(this));

    // #97450# XML import must hide sections depending on their old
    //         condition status
    if (m_pImpl->m_pProps->m_sCondition.getLength() != 0)
    {
        pRet->SetCondHidden(m_pImpl->m_pProps->m_bCondHidden);
    }

    // set update type if DDE link (and connect, if necessary)
    if (m_pImpl->m_pProps->m_bDDE)
    {
        if (! pRet->IsConnected())
        {
            pRet->CreateLink(CREATE_CONNECT);
        }
        pRet->SetUpdateType( static_cast< USHORT >(
            (m_pImpl->m_pProps->m_bUpdateType) ?
                sfx2::LINKUPDATE_ALWAYS : sfx2::LINKUPDATE_ONCALL) );
    }

    // Undo-Klammerung hier beenden
    pDoc->EndUndo( UNDO_INSSECTION, NULL );
    m_pImpl->m_pProps.reset();
    m_pImpl->m_bIsDescriptor = false;
}

/*-- 10.12.98 14:47:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange > SAL_CALL
SwXTextSection::getAnchor() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Reference< text::XTextRange >  xRet;
    SwSectionFmt *const pSectFmt = m_pImpl->GetSectionFmt();
    if(pSectFmt)
    {
        const SwSection* pSect;
        const SwNodeIndex* pIdx;
        if( 0 != ( pSect = pSectFmt->GetSection() ) &&
            0 != ( pIdx = pSectFmt->GetCntnt().GetCntntIdx() ) &&
            pIdx->GetNode().GetNodes().IsDocNodes() )
        {
            SwPaM aPaM(*pIdx);
            aPaM.Move( fnMoveForward, fnGoCntnt );

            const SwEndNode* pEndNode = pIdx->GetNode().EndOfSectionNode();
            SwPaM aEnd(*pEndNode);
            aEnd.Move( fnMoveBackward, fnGoCntnt );
            xRet = SwXTextRange::CreateXTextRange(*pSectFmt->GetDoc(),
                *aPaM.Start(), aEnd.Start());
        }
    }
    return xRet;
}
/*-- 10.12.98 14:47:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::dispose() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt *const pSectFmt = m_pImpl->GetSectionFmt();
    if (pSectFmt)
    {
        pSectFmt->GetDoc()->DelSectionFmt( pSectFmt );
    }
}
/*-- 10.12.98 14:47:10---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::addEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetSectionFmt())
    {
        throw uno::RuntimeException();
    }
    m_pImpl->m_ListenerContainer.AddListener(xListener);
}
/*-- 10.12.98 14:47:10---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::removeEventListener(
        const uno::Reference< lang::XEventListener > & xListener)
throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    if (!m_pImpl->GetSectionFmt() ||
        !m_pImpl->m_ListenerContainer.RemoveListener(xListener))
    {
        throw uno::RuntimeException();
    }
}
/*-- 10.12.98 14:47:11---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SAL_CALL
SwXTextSection::getPropertySetInfo() throw (uno::RuntimeException)
{
    vos::OGuard g(Application::GetSolarMutex());

    static const uno::Reference< beans::XPropertySetInfo >  aRef =
        m_pImpl->m_rPropSet.getPropertySetInfo();
    return aRef;
}

/* -----------------------------12.02.01 10:45--------------------------------

 ---------------------------------------------------------------------------*/
void SwXTextSection::Impl::SetPropertyValues_Impl(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues)
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
        lang::IllegalArgumentException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    if(rPropertyNames.getLength() != rValues.getLength())
    {
        throw lang::IllegalArgumentException();
    }
    SwSectionFmt *const pFmt = GetSectionFmt();
    if (!pFmt && !m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }

    SwSection aSection(CONTENT_SECTION, aEmptyStr);
    SwSection *const pSect = (pFmt) ? pFmt->GetSection() : 0;
    if (pFmt)
    {
        aSection = *pSect;
    }
    OUString const*const pPropertyNames = rPropertyNames.getConstArray();
    uno::Any const*const pValues = rValues.getConstArray();
    ::std::auto_ptr<SfxItemSet> pItemSet;
    sal_Bool bLinkModeChanged = sal_False;
    sal_Bool bLinkMode = sal_False;

    for (sal_Int32 nProperty = 0; nProperty < rPropertyNames.getLength();
         nProperty++)
    {
        SfxItemPropertySimpleEntry const*const pEntry =
            m_rPropSet.getPropertyMap()->getByName(pPropertyNames[nProperty]);
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + pPropertyNames[nProperty],
                static_cast<cppu::OWeakObject *>(& m_rThis));
        }
        if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
        {
            throw beans::PropertyVetoException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Property is read-only: "))
                    + pPropertyNames[nProperty],
                static_cast<cppu::OWeakObject *>(& m_rThis));
        }
        switch (pEntry->nWID)
        {
            case WID_SECT_CONDITION:
            {
                OUString uTmp;
                pValues[nProperty] >>= uTmp;
                if (m_bIsDescriptor)
                {
                    m_pProps->m_sCondition = uTmp;
                }
                else
                {
                    aSection.SetCondition(uTmp);
                }
            }
            break;
            case WID_SECT_DDE_TYPE:
            case WID_SECT_DDE_FILE:
            case WID_SECT_DDE_ELEMENT:
            {
                OUString uTmp;
                pValues[nProperty] >>= uTmp;
                String sTmp(uTmp);
                if (m_bIsDescriptor)
                {
                    if (!m_pProps->m_bDDE)
                    {
                        ::rtl::OUStringBuffer buf;
                        buf.append(sfx2::cTokenSeperator);
                        buf.append(sfx2::cTokenSeperator);
                        m_pProps->m_sLinkFileName = buf.makeStringAndClear();
                        m_pProps->m_bDDE = true;
                    }
                    String sLinkFileName(m_pProps->m_sLinkFileName);
                    sLinkFileName.SetToken(pEntry->nWID - WID_SECT_DDE_TYPE,
                            sfx2::cTokenSeperator, sTmp);
                    m_pProps->m_sLinkFileName = sLinkFileName;
                }
                else
                {
                    String sLinkFileName(aSection.GetLinkFileName());
                    if (aSection.GetType() != DDE_LINK_SECTION)
                    {
                        sLinkFileName = sfx2::cTokenSeperator;
                        sLinkFileName += sfx2::cTokenSeperator;
                        aSection.SetType(DDE_LINK_SECTION);
                    }
                    sLinkFileName.SetToken(pEntry->nWID - WID_SECT_DDE_TYPE,
                            sfx2::cTokenSeperator, sTmp);
                    aSection.SetLinkFileName(sLinkFileName);
                }
            }
            break;
            case WID_SECT_DDE_AUTOUPDATE:
            {
                sal_Bool bVal(sal_False);
                if (!(pValues[nProperty] >>= bVal))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bUpdateType = bVal;
                }
                else
                {
                    bLinkModeChanged = sal_True;
                    bLinkMode = bVal;
                }
            }
            break;
            case WID_SECT_LINK:
            {
                text::SectionFileLink aLink;
                if (!(pValues[nProperty] >>= aLink))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bDDE = sal_False;
                    m_pProps->m_sLinkFileName = aLink.FileURL;
                    m_pProps->m_sSectionFilter = aLink.FilterName;
                }
                else
                {
                    if (aSection.GetType() != FILE_LINK_SECTION &&
                        aLink.FileURL.getLength())
                    {
                        aSection.SetType(FILE_LINK_SECTION);
                    }
                    ::rtl::OUStringBuffer sFileNameBuf;
                    if (aLink.FileURL.getLength())
                    {
                        sFileNameBuf.append( URIHelper::SmartRel2Abs(
                            pFmt->GetDoc()->GetDocShell()->GetMedium()
                                ->GetURLObject(),
                            aLink.FileURL, URIHelper::GetMaybeFileHdl()));
                    }
                    sFileNameBuf.append(sfx2::cTokenSeperator);
                    sFileNameBuf.append(aLink.FilterName);
                    sFileNameBuf.append(sfx2::cTokenSeperator);
                    sFileNameBuf.append(
                        aSection.GetLinkFileName().GetToken(2,
                            sfx2::cTokenSeperator));
                    const ::rtl::OUString sFileName(
                            sFileNameBuf.makeStringAndClear());
                    aSection.SetLinkFileName(sFileName);
                    if (sFileName.getLength() < 3)
                    {
                        aSection.SetType(CONTENT_SECTION);
                    }
                }
            }
            break;
            case WID_SECT_REGION:
            {
                OUString sLink;
                pValues[nProperty] >>= sLink;
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bDDE = sal_False;
                    m_pProps->m_sSectionRegion = sLink;
                }
                else
                {
                    if (aSection.GetType() != FILE_LINK_SECTION &&
                        sLink.getLength())
                    {
                        aSection.SetType(FILE_LINK_SECTION);
                    }
                    String sSectLink(aSection.GetLinkFileName());
                    while (3 < sSectLink.GetTokenCount(sfx2::cTokenSeperator))
                    {
                        sSectLink += sfx2::cTokenSeperator;
                    }
                    sSectLink.SetToken(2, sfx2::cTokenSeperator, sLink);
                    aSection.SetLinkFileName(sSectLink);
                    if (sSectLink.Len() < 3)
                    {
                        aSection.SetType(CONTENT_SECTION);
                    }
                }
            }
            break;
            case WID_SECT_VISIBLE:
            {
                sal_Bool bVal(sal_False);
                if (!(pValues[nProperty] >>= bVal))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bHidden = !bVal;
                }
                else
                {
                    aSection.SetHidden(!bVal);
                }
            }
            break;
            case WID_SECT_CURRENTLY_VISIBLE:
            {
                sal_Bool bVal(sal_False);
                if (!(pValues[nProperty] >>= bVal))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bCondHidden = !bVal;
                }
                else
                {
                    if (aSection.GetCondition().Len() != 0)
                    {
                        aSection.SetCondHidden(!bVal);
                    }
                }
            }
            break;
            case WID_SECT_PROTECTED:
            {
                sal_Bool bVal(sal_False);
                if (!(pValues[nProperty] >>= bVal))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bProtect = bVal;
                }
                else
                {
                    aSection.SetProtect(bVal);
                }
            }
            break;
            // --> FME 2004-06-22 #114856# edit in readonly sections
            case WID_SECT_EDIT_IN_READONLY:
            {
                sal_Bool bVal(sal_False);
                if (!(pValues[nProperty] >>= bVal))
                {
                    throw lang::IllegalArgumentException();
                }
                if (m_bIsDescriptor)
                {
                    m_pProps->m_bEditInReadonly = bVal;
                }
                else
                {
                    aSection.SetEditInReadonly(bVal);
                }
            }
            // <--
            break;
            case WID_SECT_PASSWORD:
            {
                uno::Sequence<sal_Int8> aSeq;
                pValues[nProperty] >>= aSeq;
                if (m_bIsDescriptor)
                {
                    m_pProps->m_Password = aSeq;
                }
                else
                {
                    aSection.SetPasswd(aSeq);
                }
            }
            break;
            default:
            {
                if (pFmt)
                {
                    const SfxItemSet& rOldAttrSet = pFmt->GetAttrSet();
                    pItemSet.reset( new SfxItemSet(*rOldAttrSet.GetPool(),
                                pEntry->nWID, pEntry->nWID, 0));
                    pItemSet->Put(rOldAttrSet);
                    m_rPropSet.setPropertyValue(*pEntry,
                            pValues[nProperty], *pItemSet);
                }
                else
                {
                    SfxPoolItem* pPutItem = 0;
                    if (RES_COL == pEntry->nWID)
                    {
                        if (!m_pProps->m_pColItem.get())
                        {
                            m_pProps->m_pColItem.reset(new SwFmtCol);
                        }
                        pPutItem = m_pProps->m_pColItem.get();
                    }
                    else if (RES_BACKGROUND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pBrushItem.get())
                        {
                            m_pProps->m_pBrushItem.reset(
                                new SvxBrushItem(RES_BACKGROUND));
                        }
                        pPutItem = m_pProps->m_pBrushItem.get();
                    }
                    else if (RES_FTN_AT_TXTEND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pFtnItem.get())
                        {
                            m_pProps->m_pFtnItem.reset(new SwFmtFtnAtTxtEnd);
                        }
                        pPutItem = m_pProps->m_pFtnItem.get();
                    }
                    else if (RES_END_AT_TXTEND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pEndItem.get())
                        {
                            m_pProps->m_pEndItem.reset(new SwFmtEndAtTxtEnd);
                        }
                        pPutItem = m_pProps->m_pEndItem.get();
                    }
                    else if (RES_UNKNOWNATR_CONTAINER== pEntry->nWID)
                    {
                        if (!m_pProps->m_pXMLAttr.get())
                        {
                            m_pProps->m_pXMLAttr.reset(
                                new SvXMLAttrContainerItem(
                                    RES_UNKNOWNATR_CONTAINER));
                        }
                        pPutItem = m_pProps->m_pXMLAttr.get();
                    }
                    else if (RES_COLUMNBALANCE== pEntry->nWID)
                    {
                        if (!m_pProps->m_pNoBalanceItem.get())
                        {
                            m_pProps->m_pNoBalanceItem.reset(
                                new SwFmtNoBalancedColumns(RES_COLUMNBALANCE));
                        }
                        pPutItem = m_pProps->m_pNoBalanceItem.get();
                    }
                    else if (RES_FRAMEDIR == pEntry->nWID)
                    {
                        if (!m_pProps->m_pFrameDirItem.get())
                        {
                            m_pProps->m_pFrameDirItem.reset(
                                new SvxFrameDirectionItem(
                                FRMDIR_HORI_LEFT_TOP, RES_FRAMEDIR));
                        }
                        pPutItem = m_pProps->m_pFrameDirItem.get();
                    }
                    else if (RES_LR_SPACE == pEntry->nWID)
                    {
                        // #109700#
                        if (!m_pProps->m_pLRSpaceItem.get())
                        {
                            m_pProps->m_pLRSpaceItem.reset(
                                new SvxLRSpaceItem( RES_LR_SPACE ));
                        }
                        pPutItem = m_pProps->m_pLRSpaceItem.get();
                    }
                    if (pPutItem)
                    {
                        pPutItem->PutValue(pValues[nProperty],
                                pEntry->nMemberId);
                    }
                }
            }
        }
    }

    if (pFmt)
    {
        SwDoc* pDoc = pFmt->GetDoc();
        const SwSectionFmts& rFmts = pDoc->GetSections();
        UnoActionContext aContext(pDoc);
        for (sal_uInt16 i = 0; i < rFmts.Count(); i++)
        {
            if (rFmts[i]->GetSection()->GetName() == pSect->GetName())
            {
                pDoc->ChgSection(i, aSection, pItemSet.get(),
                        pDoc->IsInReading());
                {
                    // temporarily remove actions to allow cursor update
                    UnoActionRemoveContext aRemoveContext( pDoc );
                }

                //SwSection* pSect = pFmt->GetSection();
                if (bLinkModeChanged && pSect->GetType() == DDE_LINK_SECTION)
                {
                    // set update type; needs an established link
                    if (!pSect->IsConnected())
                    {
                        pSect->CreateLink(CREATE_CONNECT);
                    }
                    pSect->SetUpdateType( static_cast< USHORT >((bLinkMode) ?
                        sfx2::LINKUPDATE_ALWAYS : sfx2::LINKUPDATE_ONCALL) );
                }
                // section found and processed: break from loop
                break;
            }
        }
    }
}

void SAL_CALL
SwXTextSection::setPropertyValues(
    const uno::Sequence< ::rtl::OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues)
throw (beans::PropertyVetoException, lang::IllegalArgumentException,
        lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // workaround for bad designed API
    try
    {
        m_pImpl->SetPropertyValues_Impl( rPropertyNames, rValues );
    }
    catch (beans::UnknownPropertyException &rException)
    {
        // wrap the original (here not allowed) exception in
        // a WrappedTargetException that gets thrown instead.
        lang::WrappedTargetException aWExc;
        aWExc.TargetException <<= rException;
        throw aWExc;
    }
}
/*-- 10.12.98 14:47:11---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextSection::setPropertyValue(
    const OUString& rPropertyName, const uno::Any& rValue)
throw (beans::UnknownPropertyException, beans::PropertyVetoException,
        lang::IllegalArgumentException, lang::WrappedTargetException,
        uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< ::rtl::OUString > aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    uno::Sequence< uno::Any > aValues(1);
    aValues.getArray()[0] = rValue;
    m_pImpl->SetPropertyValues_Impl( aPropertyNames, aValues );
}

/* -----------------------------12.02.01 10:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any >
SwXTextSection::Impl::GetPropertyValues_Impl(
        const uno::Sequence< OUString > & rPropertyNames )
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    SwSectionFmt *const pFmt = GetSectionFmt();
    if (!pFmt && !m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }

    uno::Sequence< uno::Any > aRet(rPropertyNames.getLength());
    uno::Any* pRet = aRet.getArray();
    SwSection *const pSect = (pFmt) ? pFmt->GetSection() : 0;
    const OUString* pPropertyNames = rPropertyNames.getConstArray();

    for (sal_Int32 nProperty = 0; nProperty < rPropertyNames.getLength();
        nProperty++)
    {
        SfxItemPropertySimpleEntry const*const pEntry =
            m_rPropSet.getPropertyMap()->getByName(pPropertyNames[nProperty]);
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + pPropertyNames[nProperty],
                static_cast<cppu::OWeakObject *>(& m_rThis));
        }
        switch(pEntry->nWID)
        {
            case WID_SECT_CONDITION:
            {
                OUString uTmp( (m_bIsDescriptor)
                    ? m_pProps->m_sCondition
                    : ::rtl::OUString(pSect->GetCondition()));
                pRet[nProperty] <<= uTmp;
            }
            break;
            case WID_SECT_DDE_TYPE:
            case WID_SECT_DDE_FILE:
            case WID_SECT_DDE_ELEMENT:
            {
                ::rtl::OUString sRet;
                if (m_bIsDescriptor)
                {
                    if (m_pProps->m_bDDE)
                    {
                        sRet = m_pProps->m_sLinkFileName;
                    }
                }
                else if (DDE_LINK_SECTION == pSect->GetType())
                {
                    sRet = pSect->GetLinkFileName();
                }
                sal_Int32 nDummy(0);
                sRet = sRet.getToken(pEntry->nWID - WID_SECT_DDE_TYPE,
                            sfx2::cTokenSeperator, nDummy);
                pRet[nProperty] <<= sRet;
            }
            break;
            case WID_SECT_DDE_AUTOUPDATE:
            {
                // GetUpdateType() returns .._ALWAYS or .._ONCALL
                if (pSect && pSect->IsLinkType() && pSect->IsConnected())  // lijian i73247
                {
                    const sal_Bool bTemp =
                        (pSect->GetUpdateType() == sfx2::LINKUPDATE_ALWAYS);
                    pRet[nProperty] <<= bTemp;
                }
            }
            break;
            case WID_SECT_LINK     :
            {
                text::SectionFileLink aLink;
                if (m_bIsDescriptor)
                {
                    if (!m_pProps->m_bDDE)
                    {
                        aLink.FileURL = m_pProps->m_sLinkFileName;
                        aLink.FilterName = m_pProps->m_sSectionFilter;
                    }
                }
                else if (FILE_LINK_SECTION == pSect->GetType())
                {
                    ::rtl::OUString sRet( pSect->GetLinkFileName() );
                    sal_Int32 nIndex(0);
                    aLink.FileURL =
                        sRet.getToken(0, sfx2::cTokenSeperator, nIndex);
                    aLink.FilterName =
                        sRet.getToken(0, sfx2::cTokenSeperator, nIndex);
                }
                pRet[nProperty] <<= aLink;
            }
            break;
            case WID_SECT_REGION :
            {
                ::rtl::OUString sRet;
                if (m_bIsDescriptor)
                {
                    sRet = m_pProps->m_sSectionRegion;
                }
                else if (FILE_LINK_SECTION == pSect->GetType())
                {
                    sRet = pSect->GetLinkFileName().GetToken(2,
                            sfx2::cTokenSeperator);
                }
                pRet[nProperty] <<= sRet;
            }
            break;
            case WID_SECT_VISIBLE   :
            {
                const sal_Bool bTemp = (m_bIsDescriptor)
                    ? !m_pProps->m_bHidden : !pSect->IsHidden();
                pRet[nProperty] <<= bTemp;
            }
            break;
            case WID_SECT_CURRENTLY_VISIBLE:
            {
                const sal_Bool bTemp = (m_bIsDescriptor)
                    ? !m_pProps->m_bCondHidden : !pSect->IsCondHidden();
                pRet[nProperty] <<= bTemp;
            }
            break;
            case WID_SECT_PROTECTED:
            {
                const sal_Bool bTemp = (m_bIsDescriptor)
                    ? m_pProps->m_bProtect : pSect->IsProtect();
                pRet[nProperty] <<= bTemp;
            }
            break;
            // --> FME 2004-06-22 #114856# edit in readonly sections
            case WID_SECT_EDIT_IN_READONLY:
            {
                const sal_Bool bTemp = (m_bIsDescriptor)
                    ? m_pProps->m_bEditInReadonly : pSect->IsEditInReadonly();
                pRet[nProperty] <<= bTemp;
            }
            break;
            // <--
            case  FN_PARAM_LINK_DISPLAY_NAME:
            {
                if (pFmt)
                {
                    pRet[nProperty] <<= OUString(pFmt->GetSection()->GetName());
                }
            }
            break;
            case WID_SECT_DOCUMENT_INDEX:
            {
                // search enclosing index
                SwSection* pEnclosingSection = pSect;
                while ((pEnclosingSection != NULL) &&
                       (TOX_CONTENT_SECTION != pEnclosingSection->GetType()))
                {
                    pEnclosingSection = pEnclosingSection->GetParent();
                }
                if (pEnclosingSection)
                {
                    // convert section to TOXBase and get SwXDocumentIndex
                    SwTOXBaseSection *const pTOXBaseSect =
                        PTR_CAST(SwTOXBaseSection, pEnclosingSection);
                    const uno::Reference<text::XDocumentIndex> xIndex =
                        SwXDocumentIndexes::GetObject(pTOXBaseSect);
                    pRet[nProperty] <<= xIndex;
                }
                // else: no enclosing index found -> empty return value
            }
            break;
            case WID_SECT_IS_GLOBAL_DOC_SECTION:
            {
                const sal_Bool bRet = (NULL == pFmt) ? sal_False :
                    static_cast<sal_Bool>(NULL != pFmt->GetGlobalDocSection());
                pRet[nProperty] <<= bRet;
            }
            break;
            case  FN_UNO_ANCHOR_TYPES:
            case  FN_UNO_TEXT_WRAP:
            case  FN_UNO_ANCHOR_TYPE:
                SwXParagraph::getDefaultTextContentValue(
                        pRet[nProperty], OUString(), pEntry->nWID);
            break;
            case FN_UNO_REDLINE_NODE_START:
            case FN_UNO_REDLINE_NODE_END:
            {
                if (!pFmt)
                    break;      // lijian i73247
                SwNode* pSectNode = pFmt->GetSectionNode();
                if (FN_UNO_REDLINE_NODE_END == pEntry->nWID)
                {
                    pSectNode = pSectNode->EndOfSectionNode();
                }
                const SwRedlineTbl& rRedTbl =
                    pFmt->GetDoc()->GetRedlineTbl();
                for (USHORT nRed = 0; nRed < rRedTbl.Count(); nRed++)
                {
                    const SwRedline* pRedline = rRedTbl[nRed];
                    SwNode const*const pRedPointNode = pRedline->GetNode(TRUE);
                    SwNode const*const pRedMarkNode = pRedline->GetNode(FALSE);
                    if ((pRedPointNode == pSectNode) ||
                        (pRedMarkNode == pSectNode))
                    {
                        SwNode const*const pStartOfRedline =
                            (SwNodeIndex(*pRedPointNode) <=
                             SwNodeIndex(*pRedMarkNode))
                                 ? pRedPointNode : pRedMarkNode;
                        const bool bIsStart = (pStartOfRedline == pSectNode);
                        pRet[nProperty] <<=
                            SwXRedlinePortion::CreateRedlineProperties(
                                    *pRedline, bIsStart);
                        break;
                    }
                }
            }
            break;
            case WID_SECT_PASSWORD:
            {
                pRet[nProperty] <<= (m_bIsDescriptor)
                    ? m_pProps->m_Password : pSect->GetPasswd();
            }
            break;
            default:
            {
                if (pFmt)
                {
                    m_rPropSet.getPropertyValue(*pEntry,
                            pFmt->GetAttrSet(), pRet[nProperty]);
                }
                else
                {
                    const SfxPoolItem* pQueryItem = 0;
                    if (RES_COL == pEntry->nWID)
                    {
                        if (!m_pProps->m_pColItem.get())
                        {
                            m_pProps->m_pColItem.reset(new SwFmtCol);
                        }
                        pQueryItem = m_pProps->m_pColItem.get();
                    }
                    else if (RES_BACKGROUND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pBrushItem.get())
                        {
                            m_pProps->m_pBrushItem.reset(
                                new SvxBrushItem(RES_BACKGROUND));
                        }
                        pQueryItem = m_pProps->m_pBrushItem.get();
                    }
                    else if (RES_FTN_AT_TXTEND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pFtnItem.get())
                        {
                            m_pProps->m_pFtnItem.reset(new SwFmtFtnAtTxtEnd);
                        }
                        pQueryItem = m_pProps->m_pFtnItem.get();
                    }
                    else if (RES_END_AT_TXTEND == pEntry->nWID)
                    {
                        if (!m_pProps->m_pEndItem.get())
                        {
                            m_pProps->m_pEndItem.reset(new SwFmtEndAtTxtEnd);
                        }
                        pQueryItem = m_pProps->m_pEndItem.get();
                    }
                    else if (RES_UNKNOWNATR_CONTAINER== pEntry->nWID)
                    {
                        if (!m_pProps->m_pXMLAttr.get())
                        {
                            m_pProps->m_pXMLAttr.reset(
                                new SvXMLAttrContainerItem);
                        }
                        pQueryItem = m_pProps->m_pXMLAttr.get();
                    }
                    else if (RES_COLUMNBALANCE== pEntry->nWID)
                    {
                        if (!m_pProps->m_pNoBalanceItem.get())
                        {
                            m_pProps->m_pNoBalanceItem.reset(
                                new SwFmtNoBalancedColumns);
                        }
                        pQueryItem = m_pProps->m_pNoBalanceItem.get();
                    }
                    else if (RES_FRAMEDIR == pEntry->nWID)
                    {
                        if (!m_pProps->m_pFrameDirItem.get())
                        {
                            m_pProps->m_pFrameDirItem.reset(
                                new SvxFrameDirectionItem(
                                    FRMDIR_ENVIRONMENT, RES_FRAMEDIR));
                        }
                        pQueryItem = m_pProps->m_pFrameDirItem.get();
                    }
                    /* -> #109700# */
                    else if (RES_LR_SPACE == pEntry->nWID)
                    {
                        if (!m_pProps->m_pLRSpaceItem.get())
                        {
                            m_pProps->m_pLRSpaceItem.reset(
                                new SvxLRSpaceItem( RES_LR_SPACE ));
                        }
                        pQueryItem = m_pProps->m_pLRSpaceItem.get();
                    }
                    /* <- #109700# */
                    if (pQueryItem)
                    {
                        pQueryItem->QueryValue(pRet[nProperty],
                                pEntry->nMemberId);
                    }
                }
            }
        }
    }
    return aRet;
}

/* -----------------------------04.11.03 10:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SAL_CALL
SwXTextSection::getPropertyValues(
    const uno::Sequence< ::rtl::OUString >& rPropertyNames)
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< uno::Any > aValues;

    // workaround for bad designed API
    try
    {
        aValues = m_pImpl->GetPropertyValues_Impl( rPropertyNames );
    }
    catch (beans::UnknownPropertyException &)
    {
        throw uno::RuntimeException(OUString(
            RTL_CONSTASCII_USTRINGPARAM("Unknown property exception caught")),
            static_cast<cppu::OWeakObject *>(this));
    }
    catch (lang::WrappedTargetException &)
    {
        throw uno::RuntimeException(OUString(
                RTL_CONSTASCII_USTRINGPARAM("WrappedTargetException caught")),
            static_cast<cppu::OWeakObject *>(this));
    }

    return aValues;
}
/*-- 10.12.98 14:47:12---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
SwXTextSection::getPropertyValue(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< ::rtl::OUString > aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    return m_pImpl->GetPropertyValues_Impl(aPropertyNames).getConstArray()[0];
}
/* -----------------------------12.02.01 10:30--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::addPropertiesChangeListener(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
throw (uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::addPropertiesChangeListener(): not implemented");
}

/* -----------------------------12.02.01 10:30--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::removePropertiesChangeListener(
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
throw (uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::removePropertiesChangeListener(): not implemented");
}

/* -----------------------------12.02.01 10:30--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::firePropertiesChangeEvent(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::firePropertiesChangeEvent(): not implemented");
}

/*-- 10.12.98 14:47:13---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXTextSection::addPropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::addPropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXTextSection::removePropertyChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::removePropertyChangeListener(): not implemented");
}

void SAL_CALL
SwXTextSection::addVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::addVetoableChangeListener(): not implemented");
}

void SAL_CALL
SwXTextSection::removeVetoableChangeListener(
        const ::rtl::OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    OSL_ENSURE(false,
        "SwXTextSection::removeVetoableChangeListener(): not implemented");
}

/*-- 08.11.00 10:47:55---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SAL_CALL
SwXTextSection::getPropertyState(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< OUString > aNames(1);
    aNames.getArray()[0] = rPropertyName;
    return getPropertyStates(aNames).getConstArray()[0];
}
/*-- 08.11.00 10:47:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyState > SAL_CALL
SwXTextSection::getPropertyStates(
        const uno::Sequence< OUString >& rPropertyNames)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt *const pFmt = m_pImpl->GetSectionFmt();
    if (!pFmt && !m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }

    uno::Sequence< beans::PropertyState > aStates(rPropertyNames.getLength());
    beans::PropertyState *const pStates = aStates.getArray();
    const OUString* pNames = rPropertyNames.getConstArray();
    for (sal_Int32 i = 0; i < rPropertyNames.getLength(); i++)
    {
        pStates[i] = beans::PropertyState_DEFAULT_VALUE;
        SfxItemPropertySimpleEntry const*const pEntry =
            m_pImpl->m_rPropSet.getPropertyMap()->getByName( pNames[i]);
        if (!pEntry)
        {
            throw beans::UnknownPropertyException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                    + pNames[i], static_cast< cppu::OWeakObject* >(this));
        }
        switch (pEntry->nWID)
        {
            case WID_SECT_CONDITION:
            case WID_SECT_DDE_TYPE:
            case WID_SECT_DDE_FILE:
            case WID_SECT_DDE_ELEMENT:
            case WID_SECT_DDE_AUTOUPDATE:
            case WID_SECT_LINK:
            case WID_SECT_REGION :
            case WID_SECT_VISIBLE:
            case WID_SECT_PROTECTED:
            // --> FME 2004-06-22 #114856# edit in readonly sections
            case WID_SECT_EDIT_IN_READONLY:
            // <--
            case  FN_PARAM_LINK_DISPLAY_NAME:
            case  FN_UNO_ANCHOR_TYPES:
            case  FN_UNO_TEXT_WRAP:
            case  FN_UNO_ANCHOR_TYPE:
                pStates[i] = beans::PropertyState_DIRECT_VALUE;
            break;
            default:
            {
                if (pFmt)
                {
                    pStates[i] = m_pImpl->m_rPropSet.getPropertyState(
                                    pNames[i], pFmt->GetAttrSet());
                }
                else
                {
                    if (RES_COL == pEntry->nWID)
                    {
                        if (!m_pImpl->m_pProps->m_pColItem.get())
                        {
                            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                        }
                        else
                        {
                            pStates[i] = beans::PropertyState_DIRECT_VALUE;
                        }
                    }
                    else //if(RES_BACKGROUND == pEntry->nWID)
                    {
                        if (!m_pImpl->m_pProps->m_pBrushItem.get())
                        {
                            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                        }
                        else
                        {
                            pStates[i] = beans::PropertyState_DIRECT_VALUE;
                        }
                    }
                }
            }
        }
    }
    return aStates;
}

/*-- 08.11.00 10:47:55---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL
SwXTextSection::setPropertyToDefault(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt *const pFmt = m_pImpl->GetSectionFmt();
    if (!pFmt && !m_pImpl->m_bIsDescriptor)
    {
        throw uno::RuntimeException();
    }

    SwSection aSection(CONTENT_SECTION, aEmptyStr);
    SwSection *const pSect = (pFmt) ? pFmt->GetSection() : 0;
    if (pFmt)
    {
        aSection = *pSect;
    }
    SfxItemPropertySimpleEntry const*const pEntry =
        m_pImpl->m_rPropSet.getPropertyMap()->getByName(rPropertyName);
    if (!pEntry)
    {
        throw beans::UnknownPropertyException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                + rPropertyName, static_cast< cppu::OWeakObject* >(this));
    }
    if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
    {
        throw uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "setPropertyToDefault: property is read-only: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }

    ::std::auto_ptr<SfxItemSet> pNewAttrSet;
    switch (pEntry->nWID)
    {
        case WID_SECT_CONDITION:
        {
            if (m_pImpl->m_bIsDescriptor)
            {
                m_pImpl->m_pProps->m_sCondition = aEmptyStr;
            }
            else
            {
                aSection.SetCondition(aEmptyStr);
            }
        }
        break;
        case WID_SECT_DDE_TYPE      :
        case WID_SECT_DDE_FILE      :
        case WID_SECT_DDE_ELEMENT   :
        case WID_SECT_LINK     :
        case WID_SECT_REGION :
            aSection.SetType(CONTENT_SECTION);
        break;
        case WID_SECT_DDE_AUTOUPDATE:
            aSection.SetUpdateType(sfx2::LINKUPDATE_ALWAYS);
        break;
        case WID_SECT_VISIBLE   :
        {
            if (m_pImpl->m_bIsDescriptor)
            {
                m_pImpl->m_pProps->m_bHidden = false;
            }
            else
            {
                aSection.SetHidden(FALSE);
            }
        }
        break;
        case WID_SECT_PROTECTED:
        {
            if (m_pImpl->m_bIsDescriptor)
            {
                m_pImpl->m_pProps->m_bProtect = false;
            }
            else
            {
                aSection.SetProtect(FALSE);
            }
        }
        break;
        // --> FME 2004-06-22 #114856# edit in readonly sections
        case WID_SECT_EDIT_IN_READONLY:
        {
            if (m_pImpl->m_bIsDescriptor)
            {
                m_pImpl->m_pProps->m_bEditInReadonly = false;
            }
            else
            {
                aSection.SetEditInReadonly(FALSE);
            }
        }
        break;
        // <--

        case  FN_UNO_ANCHOR_TYPES:
        case  FN_UNO_TEXT_WRAP:
        case  FN_UNO_ANCHOR_TYPE:
        break;
        default:
        {
            if (pEntry->nWID <= SFX_WHICH_MAX)
            {
                if (pFmt)
                {
                    const SfxItemSet& rOldAttrSet = pFmt->GetAttrSet();
                    pNewAttrSet.reset( new SfxItemSet(*rOldAttrSet.GetPool(),
                                        pEntry->nWID, pEntry->nWID, 0));
                    pNewAttrSet->ClearItem(pEntry->nWID);
                }
                else
                {
                    if (RES_COL == pEntry->nWID)
                    {
                        m_pImpl->m_pProps->m_pColItem.reset();
                    }
                    else if (RES_BACKGROUND == pEntry->nWID)
                    {
                        m_pImpl->m_pProps->m_pBrushItem.reset();
                    }
                }
            }
        }
    }

    if (pFmt)
    {
        SwDoc *const pDoc = pFmt->GetDoc();
        const SwSectionFmts& rFmts = pDoc->GetSections();
        UnoActionContext aContext(pDoc);
        for (sal_uInt16 i = 0; i < rFmts.Count(); i++)
        {
            if (rFmts[i]->GetSection()->GetName() == pSect->GetName())
            {
                pDoc->ChgSection(i, aSection, pNewAttrSet.get(),
                    pDoc->IsInReading());
                {
                    // temporarily remove actions to allow cursor update
                    UnoActionRemoveContext aRemoveContext( pDoc );
                }
                break;
            }
        }
    }
}

/*-- 08.11.00 10:47:56---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
SwXTextSection::getPropertyDefault(const OUString& rPropertyName)
throw (beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Any aRet;
    SwSectionFmt *const pFmt = m_pImpl->GetSectionFmt();
    SfxItemPropertySimpleEntry const*const pEntry =
        m_pImpl->m_rPropSet.getPropertyMap()->getByName(rPropertyName);
    if (!pEntry)
    {
        throw beans::UnknownPropertyException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown property: "))
                + rPropertyName,
            static_cast<cppu::OWeakObject *>(this));
    }

    switch(pEntry->nWID)
    {
        case WID_SECT_CONDITION:
        case WID_SECT_DDE_TYPE      :
        case WID_SECT_DDE_FILE      :
        case WID_SECT_DDE_ELEMENT   :
        case WID_SECT_REGION :
        case FN_PARAM_LINK_DISPLAY_NAME:
            aRet <<= OUString();
        break;
        case WID_SECT_LINK     :
            aRet <<= text::SectionFileLink();
        break;
        case WID_SECT_DDE_AUTOUPDATE:
        case WID_SECT_VISIBLE   :
        {
            sal_Bool bTemp = TRUE;
            aRet.setValue( &bTemp, ::getCppuBooleanType());
        }
        break;
        case WID_SECT_PROTECTED:
        // --> FME 2004-06-22 #114856# edit in readonly sections
        case WID_SECT_EDIT_IN_READONLY:
        // <--
        {
            sal_Bool bTemp = FALSE;
            aRet.setValue( &bTemp, ::getCppuBooleanType());
        }
        break;
        case  FN_UNO_ANCHOR_TYPES:
        case  FN_UNO_TEXT_WRAP:
        case  FN_UNO_ANCHOR_TYPE:
            SwXParagraph::getDefaultTextContentValue(aRet, OUString(), pEntry->nWID);
        break;
        default:
        if(pFmt && pEntry->nWID <= SFX_WHICH_MAX)
        {
            SwDoc *const pDoc = pFmt->GetDoc();
            const SfxPoolItem& rDefItem =
                pDoc->GetAttrPool().GetDefaultItem(pEntry->nWID);
            rDefItem.QueryValue(aRet, pEntry->nMemberId);
        }
    }
    return aRet;
}

/*-- 10.12.98 14:47:15---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SAL_CALL SwXTextSection::getName() throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    ::rtl::OUString sRet;
    SwSectionFmt const*const pFmt = m_pImpl->GetSectionFmt();
    if(pFmt)
    {
        sRet = pFmt->GetSection()->GetName();
    }
    else if (m_pImpl->m_bIsDescriptor)
    {
        sRet = m_pImpl->m_sName;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return sRet;
}
/*-- 10.12.98 14:47:16---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextSection::setName(const OUString& rName)
throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwSectionFmt *const pFmt = m_pImpl->GetSectionFmt();
    if(pFmt)
    {
        SwSection   aSection(CONTENT_SECTION, aEmptyStr);
        SwSection *const pSect = pFmt->GetSection();
        aSection = *pSect;
        String sNewName(rName);
        aSection.SetName(sNewName);

        const SwSectionFmts& rFmts = pFmt->GetDoc()->GetSections();
        sal_uInt16 nApplyPos = USHRT_MAX;
        for( sal_uInt16 i = 0; i < rFmts.Count(); i++ )
        {
            if(rFmts[i]->GetSection() == pSect)
            {
                nApplyPos = i;
            }
            else if(sNewName == rFmts[i]->GetSection()->GetName())
            {
                throw uno::RuntimeException();
            }
        }
        if(nApplyPos != USHRT_MAX)
        {
            {
                UnoActionContext aContext(pFmt->GetDoc());
                pFmt->GetDoc()->ChgSection( nApplyPos, aSection);
            }
            {
                // temporarily remove actions to allow cursor update
                UnoActionRemoveContext aRemoveContext( pFmt->GetDoc() );
            }
        }
    }
    else if (m_pImpl->m_bIsDescriptor)
    {
        m_pImpl->m_sName = rName;
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/* -----------------02.11.99 11:30-------------------

 --------------------------------------------------*/
OUString SAL_CALL
SwXTextSection::getImplementationName() throw (uno::RuntimeException)
{
    return C2U("SwXTextSection");
}

/* -----------------02.11.99 11:30-------------------

 --------------------------------------------------*/
static char const*const g_ServicesTextSection[] =
{
    "com.sun.star.text.TextContent",
    "com.sun.star.text.TextSection",
    "com.sun.star.document.LinkTarget",
};
static const size_t g_nServicesTextSection(
    sizeof(g_ServicesTextSection)/sizeof(g_ServicesTextSection[0]));

sal_Bool SAL_CALL SwXTextSection::supportsService(const OUString& rServiceName)
throw (uno::RuntimeException)
{
    return ::sw::SupportsServiceImpl(
            g_nServicesTextSection, g_ServicesTextSection, rServiceName);
}

/* -----------------02.11.99 11:30-------------------

 --------------------------------------------------*/
uno::Sequence< OUString > SAL_CALL
SwXTextSection::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return ::sw::GetSupportedServiceNamesImpl(
            g_nServicesTextSection, g_ServicesTextSection);
}

