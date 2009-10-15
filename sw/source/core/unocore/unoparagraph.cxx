/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoparagraph.cxx,v $
 * $Revision: 1.41 $
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


#include <cmdid.h>
#include <unomid.h>
#include <unoobj.hxx>
#include <unoport.hxx>
#include <unomap.hxx>
#include <unocrsr.hxx>
#include <unoprnms.hxx>
#include <unocrsrhelper.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <docsh.hxx>

#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svl/svstdarr.hxx>

#include <com/sun/star/beans/SetPropertyTolerantFailed.hpp>
#include <com/sun/star/beans/GetPropertyTolerantResult.hpp>
#include <com/sun/star/beans/TolerantPropertySetResultType.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>

using namespace ::com::sun::star;
using ::rtl::OUString;


/* -----------------------------01.12.00 18:09--------------------------------

 ---------------------------------------------------------------------------*/
class SwParaSelection
{
    SwCursor* m_pCursor;
public:
    SwParaSelection(SwCursor* pPam);
    ~SwParaSelection();
};

SwParaSelection::SwParaSelection(SwCursor* pCursor)
    : m_pCursor(pCursor)
{
    if (m_pCursor->HasMark())
    {
        m_pCursor->DeleteMark();
    }
    // is it at the start?
    if (m_pCursor->GetPoint()->nContent != 0)
    {
        m_pCursor->MovePara(fnParaCurr, fnParaStart);
    }
    // or at the end already?
    if (m_pCursor->GetPoint()->nContent != m_pCursor->GetCntntNode()->Len())
    {
        m_pCursor->SetMark();
        m_pCursor->MovePara(fnParaCurr, fnParaEnd);
    }
}

SwParaSelection::~SwParaSelection()
{
    if (m_pCursor->GetPoint()->nContent != 0)
    {
        m_pCursor->DeleteMark();
        m_pCursor->MovePara(fnParaCurr, fnParaStart);
    }
}


/******************************************************************
 * forward declarations
 ******************************************************************/

beans::PropertyState lcl_SwXParagraph_getPropertyState(
                            const SwTxtNode& rTxtNode,
                            const SwAttrSet** ppSet,
                            const SfxItemPropertySimpleEntry& rEntry,
                            sal_Bool &rAttrSetFetched )
                                throw( beans::UnknownPropertyException);

/******************************************************************
 * SwXParagraph
 ******************************************************************/

TYPEINIT1(SwXParagraph, SwClient);

/* -----------------------------11.07.00 12:10--------------------------------

 ---------------------------------------------------------------------------*/
SwXParagraph* SwXParagraph::GetImplementation(uno::Reference< XInterface> xRef )
{
    uno::Reference<lang::XUnoTunnel> xParaTunnel( xRef, uno::UNO_QUERY);
    if(xParaTunnel.is())
        return reinterpret_cast< SwXParagraph * >(
                sal::static_int_cast< sal_IntPtr >( xParaTunnel->getSomething(SwXParagraph::getUnoTunnelId()) ));
    return 0;
}
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXParagraph::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXParagraph::getSomething( const uno::Sequence< sal_Int8 >& rId )
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
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXParagraph::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXParagraph");
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParagraph::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    String sServiceName(rServiceName);
    return sServiceName.EqualsAscii("com.sun.star.text.TextContent") ||
        sServiceName.EqualsAscii("com.sun.star.text.Paragraph") ||
         sServiceName.EqualsAscii("com.sun.star.style.CharacterProperties")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesAsian")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesComplex")||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesAsian") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesComplex");
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXParagraph::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(8);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Paragraph");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
    pArray[2] = C2U("com.sun.star.style.CharacterPropertiesAsian");
    pArray[3] = C2U("com.sun.star.style.CharacterPropertiesComplex");
    pArray[4] = C2U("com.sun.star.style.ParagraphProperties");
    pArray[5] = C2U("com.sun.star.style.ParagraphPropertiesAsian");
    pArray[6] = C2U("com.sun.star.style.ParagraphPropertiesComplex");
    pArray[7] = C2U("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph() :
    xParentText(0),
    aLstnrCntnr( (text::XTextRange*)this),
    m_pPropSet(aSwMapProvider.GetPropertySet(PROPERTY_MAP_PARAGRAPH)),
    nSelectionStartPos(-1),
    nSelectionEndPos(-1),
    m_bIsDescriptor(TRUE)
{
}

/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph(uno::Reference< text::XText > const & i_xParent, SwTxtNode * i_pTxtNode, sal_Int32 nSelStart, sal_Int32 nSelEnd) :
    SwClient(i_pTxtNode),
    xParentText(i_xParent),
    aLstnrCntnr( (text::XTextRange*)this),
    m_pPropSet(aSwMapProvider.GetPropertySet(PROPERTY_MAP_PARAGRAPH)),
    nSelectionStartPos(nSelStart),
    nSelectionEndPos(nSelEnd),
    m_bIsDescriptor(FALSE)
{
}
/*-- 11.12.98 08:12:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::~SwXParagraph()
{
}
/* -----------------------------11.07.00 14:48--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::attachToText(SwXText* pParent, SwUnoCrsr* pCrsr)
{
    DBG_ASSERT(m_bIsDescriptor, "Paragraph is not a descriptor");
    if(m_bIsDescriptor)
    {
        m_bIsDescriptor = FALSE;
        pCrsr->Add(this);
        xParentText = pParent;
        if(m_sText.getLength())
        {
            try	{ setString(m_sText); }
            catch(...){}
            m_sText = OUString();
        }
    }
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo >  SwXParagraph::getPropertySetInfo(void)
                                            throw( uno::RuntimeException )
{
    static uno::Reference< beans::XPropertySetInfo >  xRef = m_pPropSet->getPropertySetInfo();
    return xRef;
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::setPropertyValue(const OUString& rPropertyName, const uno::Any& aValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
        lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence<OUString> aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    uno::Sequence<uno::Any> aValues(1);
    aValues.getArray()[0] = aValue;
    SetPropertyValues_Impl( aPropertyNames, aValues );
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParagraph::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence<OUString> aPropertyNames(1);
    aPropertyNames.getArray()[0] = rPropertyName;
    uno::Sequence< uno::Any > aRet = GetPropertyValues_Impl(aPropertyNames );
    return aRet.getConstArray()[0];
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXParagraph::SetPropertyValues_Impl(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
            lang::WrappedTargetException, uno::RuntimeException)
{
    if (GetTxtNode())
    {
        SwPosition aPos( *GetTxtNode() );
        SwCursor aCursor( aPos, 0, false );
        const OUString* pPropertyNames = rPropertyNames.getConstArray();
        const uno::Any* pValues = rValues.getConstArray();
        const SfxItemPropertyMap*   pMap = m_pPropSet->getPropertyMap();
        OUString sTmp;
        SwParaSelection aParaSel( & aCursor );
        for(sal_Int32 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
        {
            const SfxItemPropertySimpleEntry* pEntry = pMap->getByName( pPropertyNames[nProp] );
            if(!pEntry)
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pPropertyNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
            else
            {
                if ( pEntry->nFlags & beans::PropertyAttribute::READONLY)
                    throw beans::PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + pPropertyNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );

                SwXTextCursor::SetPropertyValue(aCursor, *m_pPropSet,
                                        sTmp, pValues[nProp]);
            }
        }
    }
    else
        throw uno::RuntimeException();
}

void SwXParagraph::setPropertyValues(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
        throw(beans::PropertyVetoException, lang::IllegalArgumentException,
                        lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // workaround for bad designed API
    try
    {
        SetPropertyValues_Impl( rPropertyNames, rValues );
    }
    catch (beans::UnknownPropertyException &rException)
    {
        // wrap the original (here not allowed) exception in
        // a lang::WrappedTargetException that gets thrown instead.
        lang::WrappedTargetException aWExc;
        aWExc.TargetException <<= rException;
        throw aWExc;
    }
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SAL_CALL SwXParagraph::GetPropertyValues_Impl(
        const uno::Sequence< OUString > & rPropertyNames )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    uno::Sequence< uno::Any > aValues(rPropertyNames.getLength());
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwPaM aPam( aPos );
        uno::Any* pValues = aValues.getArray();
        const OUString* pPropertyNames = rPropertyNames.getConstArray();
        const SfxItemPropertyMap*   pMap = m_pPropSet->getPropertyMap();
        const SwAttrSet& rAttrSet( pTxtNode->GetSwAttrSet() );
        for(sal_Int32 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
        {
            const SfxItemPropertySimpleEntry* pEntry = pMap->getByName( pPropertyNames[nProp] );
            if(pEntry)
            {
                if(!SwXParagraph::getDefaultTextContentValue(
                    pValues[nProp], pPropertyNames[nProp], pEntry->nWID))
                {
                    BOOL bDone = FALSE;
                    beans::PropertyState eTemp;
                    bDone = SwUnoCursorHelper::getCrsrPropertyValue(
                        *pEntry, aPam, &(pValues[nProp]), eTemp, pTxtNode );
                    if(!bDone)
                        m_pPropSet->getPropertyValue(*pEntry, rAttrSet, pValues[nProp]);
                }
            }
            else
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pPropertyNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
        }
    }
    else
        throw uno::RuntimeException();
    return aValues;
}
/* -----------------------------04.11.03 11:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXParagraph::getPropertyValues(
    const uno::Sequence< OUString >& rPropertyNames )
        throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< uno::Any > aValues;

    // workaround for bad designed API
    try
    {
        aValues = GetPropertyValues_Impl( rPropertyNames );
    }
    catch (beans::UnknownPropertyException &)
    {
        throw uno::RuntimeException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property exception caught" ) ), static_cast < cppu::OWeakObject * > ( this ) );
    }
    catch (lang::WrappedTargetException &)
    {
        throw uno::RuntimeException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "WrappedTargetException caught" ) ), static_cast < cppu::OWeakObject * > ( this ) );
    }

    return aValues;
}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::addPropertiesChangeListener(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::removePropertiesChangeListener(
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{}
/* -----------------------------02.04.01 11:43--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::firePropertiesChangeEvent(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{}
/* -----------------------------25.09.03 11:09--------------------------------

 ---------------------------------------------------------------------------*/

/* disabled for #i46921# */

uno::Sequence< beans::SetPropertyTolerantFailed > SAL_CALL SwXParagraph::setPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames,
        const uno::Sequence< uno::Any >& rValues )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    if (rPropertyNames.getLength() != rValues.getLength())
        throw lang::IllegalArgumentException();

    const SwTxtNode * const pTxtNode( GetTxtNode() );
    if (!pTxtNode)
        throw uno::RuntimeException();

    //SwNode& rTxtNode = pUnoCrsr->GetPoint()->nNode.GetNode();
    //const SwAttrSet& rAttrSet = ((SwTxtNode&)rTxtNode).GetSwAttrSet();
    //USHORT nAttrCount = rAttrSet.Count();

    sal_Int32 nProps = rPropertyNames.getLength();
    const OUString *pProp = rPropertyNames.getConstArray();

    //sal_Int32 nVals = rValues.getLength();
    const uno::Any *pValue = rValues.getConstArray();

    sal_Int32 nFailed = 0;
    uno::Sequence< beans::SetPropertyTolerantFailed > aFailed( nProps );
    beans::SetPropertyTolerantFailed *pFailed = aFailed.getArray();

    // get entry to start with
    const SfxItemPropertyMap*  pPropMap = m_pPropSet->getPropertyMap();

    OUString sTmp;
    SwPosition aPos( *pTxtNode );
    SwCursor aCursor( aPos, 0, false );
    SwParaSelection aParaSel( & aCursor );
    for (sal_Int32 i = 0;  i < nProps;  ++i)
    {
        try
        {
            pFailed[ nFailed ].Name = pProp[i];

            const SfxItemPropertySimpleEntry* pEntry = pPropMap->getByName( pProp[i] );
            if (!pEntry)
                pFailed[ nFailed++ ].Result  = beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
            else
            {
                // set property value
                // (compare to SwXParagraph::setPropertyValues)
                if (pEntry->nFlags & beans::PropertyAttribute::READONLY)
                    pFailed[ nFailed++ ].Result  = beans::TolerantPropertySetResultType::PROPERTY_VETO;
                else
                {
                    SwXTextCursor::SetPropertyValue(
                                aCursor, *m_pPropSet, pProp[i], pValue[i] );
                }
            }
        }
        catch (beans::UnknownPropertyException &)
        {
            // should not occur because property was searched for before
            DBG_ERROR( "unexpected exception catched" );
            pFailed[ nFailed++ ].Result = beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
        }
        catch (lang::IllegalArgumentException &)
        {
            pFailed[ nFailed++ ].Result = beans::TolerantPropertySetResultType::ILLEGAL_ARGUMENT;
        }
        catch (beans::PropertyVetoException &)
        {
            pFailed[ nFailed++ ].Result = beans::TolerantPropertySetResultType::PROPERTY_VETO;
        }
        catch (lang::WrappedTargetException &)
        {
            pFailed[ nFailed++ ].Result = beans::TolerantPropertySetResultType::WRAPPED_TARGET;
        }
    }

    aFailed.realloc( nFailed );
    return aFailed;
}


uno::Sequence< beans::GetPropertyTolerantResult > SAL_CALL SwXParagraph::getPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    uno::Sequence< beans::GetDirectPropertyTolerantResult > aTmpRes(
            GetPropertyValuesTolerant_Impl( rPropertyNames, sal_False ) );
    const beans::GetDirectPropertyTolerantResult *pTmpRes = aTmpRes.getConstArray();

    // copy temporary result to final result type
    sal_Int32 nLen = aTmpRes.getLength();
    uno::Sequence< beans::GetPropertyTolerantResult > aRes( nLen );
    beans::GetPropertyTolerantResult *pRes = aRes.getArray();
    for (sal_Int32 i = 0;  i < nLen;  i++)
        *pRes++ = *pTmpRes++;
    return aRes;
}


uno::Sequence< beans::GetDirectPropertyTolerantResult > SAL_CALL SwXParagraph::getDirectPropertyValuesTolerant(
        const uno::Sequence< OUString >& rPropertyNames )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return GetPropertyValuesTolerant_Impl( rPropertyNames, sal_True );
}


uno::Sequence< beans::GetDirectPropertyTolerantResult > SAL_CALL SwXParagraph::GetPropertyValuesTolerant_Impl(
        const uno::Sequence< OUString >& rPropertyNames,
        sal_Bool bDirectValuesOnly )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    const SwTxtNode * const pTxtNode( GetTxtNode() );
    DBG_ASSERT( pTxtNode != NULL, "need text node" );
    if (!pTxtNode)
        throw uno::RuntimeException();

    // #i46786# Use SwAttrSet pointer for determining the state.
    //          Use the value SwAttrSet (from the paragraph OR the style)
    //          for determining the actual value(s).
    const SwAttrSet* pAttrSet = pTxtNode->GetpSwAttrSet();
    const SwAttrSet& rValueAttrSet = pTxtNode->GetSwAttrSet();

    sal_Int32 nProps = rPropertyNames.getLength();
    const OUString *pProp = rPropertyNames.getConstArray();

    uno::Sequence< beans::GetDirectPropertyTolerantResult > aResult( nProps );
    beans::GetDirectPropertyTolerantResult *pResult = aResult.getArray();
    sal_Int32 nIdx = 0;

    // get entry to start with
    const SfxItemPropertyMap *pPropMap = m_pPropSet->getPropertyMap();

    for (sal_Int32 i = 0;  i < nProps;  ++i)
    {
        DBG_ASSERT( nIdx < nProps, "index out ouf bounds" );
        beans::GetDirectPropertyTolerantResult &rResult = pResult[nIdx];

        try
        {
            rResult.Name = pProp[i];

            const SfxItemPropertySimpleEntry* pEntry = pPropMap->getByName( pProp[i] );
            if (!pEntry)  // property available?
                rResult.Result = beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
            else
            {
                // get property state
                // (compare to SwXParagraph::getPropertyState)
                sal_Bool bAttrSetFetched = sal_True;
                beans::PropertyState eState = lcl_SwXParagraph_getPropertyState(
                            *pTxtNode, &pAttrSet, *pEntry, bAttrSetFetched );
                rResult.State  = eState;

//                if (bDirectValuesOnly  &&  PropertyState_DIRECT_VALUE != eState)
//                    rResult.Result = beans::TolerantPropertySetResultType::NO_DIRECT_VALUE;
//                else
                rResult.Result = beans::TolerantPropertySetResultType::UNKNOWN_FAILURE;
                if (!bDirectValuesOnly  ||  beans::PropertyState_DIRECT_VALUE == eState)
                {
                    // get property value
                    // (compare to SwXParagraph::getPropertyValue(s))
                    uno::Any aValue;
                    if (!SwXParagraph::getDefaultTextContentValue(
                                aValue, pProp[i], pEntry->nWID ) )
                    {
                        SwPosition aPos( *pTxtNode );
                        SwPaM aPam( aPos );
                        // handle properties that are not part of the attribute
                        // and thus only pretendend to be paragraph attributes
                        BOOL bDone = FALSE;
                        beans::PropertyState eTemp;
                        bDone = SwUnoCursorHelper::getCrsrPropertyValue(
                                    *pEntry, aPam, &aValue, eTemp, pTxtNode );

                        // if not found try the real paragraph attributes...
                        if (!bDone)
                            m_pPropSet->getPropertyValue( *pEntry, rValueAttrSet, aValue );
                    }

                    rResult.Value  = aValue;
                    rResult.Result = beans::TolerantPropertySetResultType::SUCCESS;

                    nIdx++;
                }
                // this assertion should never occur!
                DBG_ASSERT( nIdx < 1  ||  pResult[nIdx - 1].Result != beans::TolerantPropertySetResultType::UNKNOWN_FAILURE,
                        "unknown failure while retrieving property" );

            }
        }
        catch (beans::UnknownPropertyException &)
        {
            // should not occur because property was searched for before
            DBG_ERROR( "unexpected exception caught" );
            rResult.Result = beans::TolerantPropertySetResultType::UNKNOWN_PROPERTY;
        }
        catch (lang::IllegalArgumentException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::ILLEGAL_ARGUMENT;
        }
        catch (beans::PropertyVetoException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::PROPERTY_VETO;
        }
        catch (lang::WrappedTargetException &)
        {
            rResult.Result = beans::TolerantPropertySetResultType::WRAPPED_TARGET;
        }
    }

    // resize to actually used size
    aResult.realloc( nIdx );

    return aResult;
}

/* -----------------------------12.09.00 11:09--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParagraph::getDefaultTextContentValue(uno::Any& rAny, const OUString& rPropertyName, USHORT nWID)
{
    if(!nWID)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPE)))
            nWID = FN_UNO_ANCHOR_TYPE;
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPES)))
            nWID = FN_UNO_ANCHOR_TYPES;
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_TEXT_WRAP)))
            nWID = FN_UNO_TEXT_WRAP;
        else
            return FALSE;
    }

    switch(nWID)
    {
        case FN_UNO_TEXT_WRAP:  rAny <<= text::WrapTextMode_NONE; break;
        case FN_UNO_ANCHOR_TYPE: rAny <<= text::TextContentAnchorType_AT_PARAGRAPH; break;
        case FN_UNO_ANCHOR_TYPES:
        {	uno::Sequence<text::TextContentAnchorType> aTypes(1);
            text::TextContentAnchorType* pArray = aTypes.getArray();
            pArray[0] = text::TextContentAnchorType_AT_PARAGRAPH;
            rAny.setValue(&aTypes, ::getCppuType((uno::Sequence<text::TextContentAnchorType>*)0));
        }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::addPropertyChangeListener(
    const OUString& /*PropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::removePropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::addVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::removeVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}
//-----------------------------------------------------------------------------
beans::PropertyState lcl_SwXParagraph_getPropertyState(
//							SwUnoCrsr& rUnoCrsr,
                            const SwTxtNode& rTxtNode,
                            const SwAttrSet** ppSet,
                            const SfxItemPropertySimpleEntry& rEntry,
                            sal_Bool &rAttrSetFetched )
                                throw( beans::UnknownPropertyException)
{
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;

    if(!(*ppSet) && !rAttrSetFetched )
    {
        (*ppSet) = rTxtNode.GetpSwAttrSet();
        rAttrSetFetched = sal_True;
    }
    SwPosition aPos( rTxtNode );
    SwPaM aPam( aPos );
    switch( rEntry.nWID )
    {
    case FN_UNO_NUM_RULES:
        // if numbering is set, return it; else do nothing
        SwUnoCursorHelper::getNumberingProperty( aPam, eRet, NULL );
        break;
    case FN_UNO_ANCHOR_TYPES:
        break;
    case RES_ANCHOR:
        if ( MID_SURROUND_SURROUNDTYPE != rEntry.nMemberId )
            goto lcl_SwXParagraph_getPropertyStateDEFAULT;
        break;
    case RES_SURROUND:
        if ( MID_ANCHOR_ANCHORTYPE != rEntry.nMemberId )
            goto lcl_SwXParagraph_getPropertyStateDEFAULT;
        break;
    case FN_UNO_PARA_STYLE:
    case FN_UNO_PARA_CONDITIONAL_STYLE_NAME:
        {
            SwFmtColl* pFmt = SwXTextCursor::GetCurTxtFmtColl(
                aPam, rEntry.nWID == FN_UNO_PARA_CONDITIONAL_STYLE_NAME);
            eRet = pFmt ? beans::PropertyState_DIRECT_VALUE
                        : beans::PropertyState_AMBIGUOUS_VALUE;
        }
        break;
    case FN_UNO_PAGE_STYLE:
        {
            String sVal;
            SwUnoCursorHelper::GetCurPageStyle( aPam, sVal );
            eRet = sVal.Len() ? beans::PropertyState_DIRECT_VALUE
                              : beans::PropertyState_AMBIGUOUS_VALUE;
        }
        break;
    lcl_SwXParagraph_getPropertyStateDEFAULT:
    default:
        if((*ppSet) && SFX_ITEM_SET == (*ppSet)->GetItemState(rEntry.nWID, FALSE))
            eRet = beans::PropertyState_DIRECT_VALUE;
        break;
    }
    return eRet;
}

/*-- 05.03.99 11:37:30---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SwXParagraph::getPropertyState(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;
    const SwTxtNode * const pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        const SwAttrSet* pSet = 0;
        const SfxItemPropertySimpleEntry* pEntry = m_pPropSet->getPropertyMap()->getByName( rPropertyName );
        if(!pEntry)
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
        sal_Bool bDummy = sal_False;
        eRet = lcl_SwXParagraph_getPropertyState( *pTxtNode, &pSet, *pEntry,
                bDummy );
    }
    else
        throw uno::RuntimeException();
    return eRet;
}
/*-- 05.03.99 11:37:32---------------------------------------------------

  -----------------------------------------------------------------------*/

uno::Sequence< beans::PropertyState > SwXParagraph::getPropertyStates(
        const uno::Sequence< OUString >& PropertyNames)
        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const OUString* pNames = PropertyNames.getConstArray();
    uno::Sequence< beans::PropertyState > aRet(PropertyNames.getLength());
    beans::PropertyState* pStates = aRet.getArray();

    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        const SfxItemPropertyMap* pMap = m_pPropSet->getPropertyMap();
        const SwAttrSet* pSet = 0;
        sal_Bool bAttrSetFetched = sal_False;
        for(sal_Int32 i = 0, nEnd = PropertyNames.getLength(); i < nEnd; i++,++pStates,++pNames )
        {
            const SfxItemPropertySimpleEntry* pEntry = pMap->getByName( *pNames );
            if(!pEntry)
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + *pNames, static_cast < cppu::OWeakObject * > ( this ) );

            if (bAttrSetFetched && !pSet && isATR(pEntry->nWID))
            {
                *pStates = beans::PropertyState_DEFAULT_VALUE;
            }
            else
            {
                *pStates = lcl_SwXParagraph_getPropertyState(
                    *pTxtNode, &pSet, *pEntry, bAttrSetFetched );
            }
        }
    }
    else
        throw uno::RuntimeException();

    return aRet;
}
/*-- 05.03.99 11:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::setPropertyToDefault(const OUString& rPropertyName)
        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        if( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPE)) ||
            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_ANCHOR_TYPES)) ||
            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_TEXT_WRAP)))
                return;

        // select paragraph
        SwParaSelection aParaSel( & aCursor );
        const SfxItemPropertySimpleEntry* pEntry = m_pPropSet->getPropertyMap()->getByName( rPropertyName );
        if(pEntry)
        {
            if ( pEntry->nFlags & beans::PropertyAttribute::READONLY)
                throw uno::RuntimeException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only:" ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );

            if(pEntry->nWID < RES_FRMATR_END)
            {
                SvUShortsSort aWhichIds;
                aWhichIds.Insert(pEntry->nWID);
                if(pEntry->nWID < RES_PARATR_BEGIN)
                {
                    aCursor.GetDoc()->ResetAttrs(aCursor, sal_True, &aWhichIds);
                }
                else
                {
                    // for paragraph attributes the selection must be extended
                    // to paragraph boundaries
                    SwPosition aStart( *aCursor.Start() );
                    SwPosition aEnd  ( *aCursor.End()   );
                    std::auto_ptr<SwUnoCrsr> pTemp(
                        aCursor.GetDoc()->CreateUnoCrsr(aStart, sal_False) );
                    if(!SwUnoCursorHelper::IsStartOfPara(*pTemp))
                    {
                        pTemp->MovePara(fnParaCurr, fnParaStart);
                    }
                    pTemp->SetMark();
                    *pTemp->GetPoint() = aEnd;
                    //pTemp->Exchange();
                    SwXTextCursor::SelectPam(*pTemp, sal_True);
                    if(!SwUnoCursorHelper::IsEndOfPara(*pTemp))
                    {
                        pTemp->MovePara(fnParaCurr, fnParaEnd);
                    }
                    pTemp->GetDoc()->ResetAttrs(*pTemp, sal_True, &aWhichIds);
                }
            }
            else
            {
                SwUnoCursorHelper::resetCrsrPropertyValue(*pEntry, aCursor);
            }
        }
        else
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
    }
    else
        throw uno::RuntimeException();
}
/*-- 05.03.99 11:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParagraph::getPropertyDefault(const OUString& rPropertyName)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    uno::Any aRet;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        if(SwXParagraph::getDefaultTextContentValue(aRet, rPropertyName))
            return aRet;

        const SfxItemPropertySimpleEntry* pEntry = m_pPropSet->getPropertyMap()->getByName( rPropertyName );
        if(pEntry)
        {
            if(pEntry->nWID < RES_FRMATR_END)
            {
                const SfxPoolItem& rDefItem =
                    pTxtNode->GetDoc()->GetAttrPool().GetDefaultItem(
                        pEntry->nWID);
                rDefItem.QueryValue(aRet, pEntry->nMemberId);
            }
        }
        else
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
    }
    else
        throw uno::RuntimeException();

    return aRet;
}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::attach(const uno::Reference< text::XTextRange > & /*xTextRange*/)
                    throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    // SwXParagraph will only created in order to be inserteb by
    // 'insertTextContentBefore' or 'insertTextContentAfter' therefore
    // they cannot be attached
    throw uno::RuntimeException();
}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXParagraph::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  aRet;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        // select paragraph
        SwParaSelection aParaSel( & aCursor );
        aRet = new SwXTextRange(aCursor, xParentText);
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 11.12.98 08:12:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::dispose(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwCursor aCursor( SwPosition( *pTxtNode ), 0, false );
        // select paragraph
        {
            SwParaSelection aParaSel( & aCursor );
            pTxtNode->GetDoc()->DelFullPara(aCursor);
        }
        aLstnrCntnr.Disposing();
    }
    else
        throw uno::RuntimeException();
}
/*-- 11.12.98 08:12:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::addEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn())
        throw uno::RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 11.12.98 08:12:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::removeEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw uno::RuntimeException();
}
/*-- 11.12.98 08:12:53---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXParagraph::createEnumeration(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< container::XEnumeration >  aRef;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwPaM aPam ( aPos );
        aRef = new SwXTextPortionEnumeration(aPam, xParentText,
                    nSelectionStartPos, nSelectionEndPos);
    }
    else
        throw uno::RuntimeException();
    return aRef;

}
/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXParagraph::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((uno::Reference<text::XTextRange>*)0);
}
/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXParagraph::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return (GetTxtNode()) ? sal_True : sal_False;
}
/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XText >  SwXParagraph::getText(void) throw( uno::RuntimeException )
{
    return xParentText;
}
/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXParagraph::getStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  xRet;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        SwParaSelection aParaSel( &aCursor );
        SwPaM aPam( *aCursor.Start() );
        uno::Reference< text::XText >  xParent = getText();
        xRet = new SwXTextRange(aPam, xParent);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 11.12.98 08:12:56---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXParagraph::getEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >  xRet;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        SwParaSelection aParaSel( &aCursor );
        SwPaM aPam( *aCursor.End() );
        uno::Reference< text::XText >  xParent = getText();
        xRet = new SwXTextRange(aPam, xParent);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 11.12.98 08:12:56---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXParagraph::getString(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    OUString aRet;
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        SwParaSelection aParaSel( & aCursor );
        SwXTextCursor::getTextFromPam(aCursor, aRet);
    }
    else if(IsDescriptor())
        aRet = m_sText;
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 11.12.98 08:12:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::setString(const OUString& aString) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    const SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        SwPosition aPos( *pTxtNode );
        SwCursor aCursor( aPos, 0, false );
        if (!SwUnoCursorHelper::IsStartOfPara(aCursor)) {
            aCursor.MovePara(fnParaCurr, fnParaStart);
        }
        SwXTextCursor::SelectPam(aCursor, sal_True);
        if (pTxtNode->GetTxt().Len()) {
            aCursor.MovePara(fnParaCurr, fnParaEnd);
        }
        SwXTextCursor::SetString(aCursor, aString);
        SwXTextCursor::SelectPam(aCursor, sal_False);
    }
    else if(IsDescriptor())
        m_sText = aString;
    else
        throw uno::RuntimeException();

}
/* -----------------23.03.99 12:49-------------------
 *
 * --------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXParagraph::createContentEnumeration(const OUString& rServiceName)
    throw( uno::RuntimeException )
{
    const SwTxtNode * pTxtNode( GetTxtNode() );
    if(!pTxtNode || !rServiceName.equalsAscii("com.sun.star.text.TextContent"))
        throw uno::RuntimeException();

    SwPosition aPos( *pTxtNode );
    SwPaM aPam( aPos );
    uno::Reference< container::XEnumeration > xRet =
        new SwXParaFrameEnumeration(aPam, PARAFRAME_PORTION_PARAGRAPH);
    return xRet;
}
/* -----------------23.03.99 12:49-------------------
 *
 * --------------------------------------------------*/
uno::Sequence< OUString > SwXParagraph::getAvailableServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 11.12.98 08:12:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        aLstnrCntnr.Disposing();
}

const SwTxtNode * SwXParagraph::GetTxtNode() const
{
        return static_cast<const SwTxtNode*>(GetRegisteredIn());
}

SwTxtNode * SwXParagraph::GetTxtNode()
{
        return static_cast<SwTxtNode*>(pRegisteredIn);
}

::sfx2::Metadatable* SwXParagraph::GetCoreObject()
{
    SwTxtNode * pTxtNode( GetTxtNode() );
    return pTxtNode;
}

uno::Reference<frame::XModel> SwXParagraph::GetModel()
{
    SwTxtNode * pTxtNode( GetTxtNode() );
    if (pTxtNode)
    {
        const SwDocShell * pShell( pTxtNode->GetDoc()->GetDocShell() );
        return (pShell) ? pShell->GetModel() : 0;
    }
    else
    {
        return 0;
    }
}

