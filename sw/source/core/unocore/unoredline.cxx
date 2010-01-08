/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoredline.cxx,v $
 * $Revision: 1.34 $
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

#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/text/XTextTable.hpp>

#include <rtl/ustrbuf.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>

#include <pagedesc.hxx>
#include "poolfmt.hxx"
#include <redline.hxx>
#include <section.hxx>
#include <unoprnms.hxx>
#include <unomid.h>
#include <unotextrange.hxx>
#include <unotextcursor.hxx>
#include <unoparagraph.hxx>
#include <unocoll.hxx>
#include <unomap.hxx>
#include <unocrsr.hxx>
#include <unoredline.hxx>
#include <doc.hxx>
#include <docary.hxx>


using namespace ::com::sun::star;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;


/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwXRedlineText::SwXRedlineText(SwDoc* _pDoc, SwNodeIndex aIndex) :
    SwXText(_pDoc, CURSOR_REDLINE),
    aNodeIndex(aIndex)
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
const SwStartNode* SwXRedlineText::GetStartNode() const
{
    return aNodeIndex.GetNode().GetStartNode();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXRedlineText::queryInterface( const uno::Type& rType )
    throw(uno::RuntimeException)
{
    uno::Any aRet;

    if (::getCppuType((uno::Reference<container::XEnumerationAccess> *)0) == rType)
    {
        uno::Reference<container::XEnumerationAccess> aAccess = this;
        aRet <<= aAccess;
    }
    else
    {
        // delegate to SwXText and OWeakObject
        aRet = SwXText::queryInterface(rType);
        if(!aRet.hasValue())
        {
            aRet = OWeakObject::queryInterface(rType);
        }
    }

    return aRet;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence<uno::Type> SwXRedlineText::getTypes()
    throw(uno::RuntimeException)
{
    // SwXText::getTypes()
    uno::Sequence<uno::Type> aTypes = SwXText::getTypes();

    // add container::XEnumerationAccess
    sal_Int32 nLength = aTypes.getLength();
    aTypes.realloc(nLength + 1);
    aTypes[nLength] = ::getCppuType((uno::Reference<container::XEnumerationAccess> *)0);

    return aTypes;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence<sal_Int8> SwXRedlineText::getImplementationId()
    throw(uno::RuntimeException)
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
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference<text::XTextCursor> SwXRedlineText::createTextCursor(void)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwPosition aPos(aNodeIndex);
    SwXTextCursor* pCrsr = new SwXTextCursor(this, aPos, CURSOR_REDLINE,
                                             GetDoc());
    SwUnoCrsr* pUnoCursor = pCrsr->GetCrsr();
    pUnoCursor->Move(fnMoveForward, fnGoNode);

    // #101929# prevent a newly created text cursor from running inside a table
    // because table cells have their own XText.
    // Patterned after SwXTextFrame::createTextCursor(void).

    // skip all tables at the beginning
    SwTableNode* pTableNode = pUnoCursor->GetNode()->FindTableNode();
    SwCntntNode* pContentNode = NULL;
    bool bTable = pTableNode != NULL;
    while( pTableNode != NULL )
    {
        pUnoCursor->GetPoint()->nNode = *(pTableNode->EndOfSectionNode());
        pContentNode = GetDoc()->GetNodes().GoNext(&pUnoCursor->GetPoint()->nNode);
        pTableNode = pContentNode->FindTableNode();
    }
    if( pContentNode != NULL )
        pUnoCursor->GetPoint()->nContent.Assign( pContentNode, 0 );
    if( bTable && pUnoCursor->GetNode()->FindSttNodeByType( SwNormalStartNode )
                                                            != GetStartNode() )
    {
        // We have gone too far and have left our own redline. This means that
        // no content node outside of a table could be found, and therefore we
        // except.
        uno::RuntimeException aExcept;
        aExcept.Message = OUString( RTL_CONSTASCII_USTRINGPARAM(
            "No content node found that is inside this change section "
            "but outside of a table" ) );
        throw aExcept;
    }

    return (text::XWordCursor*)pCrsr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference<text::XTextCursor> SwXRedlineText::createTextCursorByRange(
    const uno::Reference<text::XTextRange> & aTextRange)
        throw( uno::RuntimeException )
{
    uno::Reference<text::XTextCursor> xCursor = createTextCursor();
    xCursor->gotoRange(aTextRange->getStart(), sal_False);
    xCursor->gotoRange(aTextRange->getEnd(), sal_True);
    return xCursor;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference<container::XEnumeration> SwXRedlineText::createEnumeration(void)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwPaM aPam(aNodeIndex);
    aPam.Move(fnMoveForward, fnGoNode);
    ::std::auto_ptr<SwUnoCrsr> pUnoCursor(
        GetDoc()->CreateUnoCrsr(*aPam.Start(), sal_False));
    return new SwXParagraphEnumeration(this, pUnoCursor, CURSOR_REDLINE);
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Type SwXRedlineText::getElementType(  ) throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<text::XTextRange>*)0);
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwXRedlineText::hasElements(  ) throw(uno::RuntimeException)
{
    return sal_True; 	// we always have a content index
}
/* -----------------------------19.12.00 11:36--------------------------------

 ---------------------------------------------------------------------------*/
SwXRedlinePortion::SwXRedlinePortion(	const SwRedline* pRed,
                        const SwUnoCrsr* pPortionCrsr,
                        uno::Reference< text::XText >  xParent, BOOL bStart) :
    SwXTextPortion(pPortionCrsr, xParent, bStart ? PORTION_REDLINE_START : PORTION_REDLINE_END),
//	SwXText(pPortionCrsr->GetDoc(), CURSOR_REDLINE),
//	SwXRedlineText(pPortionCrsr->GetDoc(), *pRed->GetContentIdx()),
    pRedline(pRed)
{
    SetCollapsed(!pRedline->HasMark());
}
/*-- 19.12.00 11:37:24---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXRedlinePortion::~SwXRedlinePortion()
{
}
/* -----------------------------19.12.00 11:46--------------------------------

 ---------------------------------------------------------------------------*/
static util::DateTime lcl_DateTimeToUno(const DateTime& rDT)
{
    util::DateTime aRetDT;
    aRetDT.Year = rDT.GetYear();
    aRetDT.Month= rDT.GetMonth();
    aRetDT.Day  	= rDT.GetDay();
    aRetDT.Hours	= rDT.GetHour();
    aRetDT.Minutes = rDT.GetMin();
    aRetDT.Seconds = rDT.GetSec();
    aRetDT.HundredthSeconds = rDT.Get100Sec();
    return aRetDT;
}

// ---------------------------------------------------------------------------
static OUString lcl_RedlineTypeToOUString(RedlineType_t eType)
{
    OUString sRet;
    switch(eType & nsRedlineType_t::REDLINE_NO_FLAG_MASK)
    {
        case nsRedlineType_t::REDLINE_INSERT: sRet = C2U("Insert"); break;
        case nsRedlineType_t::REDLINE_DELETE: sRet = C2U("Delete"); break;
        case nsRedlineType_t::REDLINE_FORMAT: sRet = C2U("Format"); break;
        case nsRedlineType_t::REDLINE_TABLE:  sRet = C2U("TextTable"); break;
        case nsRedlineType_t::REDLINE_FMTCOLL:sRet = C2U("Style"); break;
    }
    return sRet;
}

// ---------------------------------------------------------------------------
static uno::Sequence<beans::PropertyValue> lcl_GetSuccessorProperties(const SwRedline& rRedline)
{
    uno::Sequence<beans::PropertyValue> aValues(4);

    const SwRedlineData* pNext = rRedline.GetRedlineData().Next();
    if(pNext)
    {
        beans::PropertyValue* pValues = aValues.getArray();
        pValues[0].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_AUTHOR));
        // GetAuthorString(n) walks the SwRedlineData* chain;
        // here we always need element 1
        pValues[0].Value <<= OUString(rRedline.GetAuthorString(1));
        pValues[1].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_DATE_TIME));
        pValues[1].Value <<= lcl_DateTimeToUno(pNext->GetTimeStamp());
        pValues[2].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_COMMENT));
        pValues[2].Value <<= OUString(pNext->GetComment());
        pValues[3].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_TYPE));
        pValues[3].Value <<= lcl_RedlineTypeToOUString(pNext->GetType());
    }
    return aValues;
}
// ---------------------------------------------------------------------------
uno::Any SwXRedlinePortion::getPropertyValue( const OUString& rPropertyName )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    Validate();
    uno::Any aRet;
    if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_TEXT)))
    {
        SwNodeIndex* pNodeIdx = pRedline->GetContentIdx();
        if(pNodeIdx )
        {
            if ( 1 < ( pNodeIdx->GetNode().EndOfSectionIndex() - pNodeIdx->GetNode().GetIndex() ) )
            {
                SwUnoCrsr* pUnoCrsr = GetCursor();
                uno::Reference<text::XText> xRet = new SwXRedlineText(pUnoCrsr->GetDoc(), *pNodeIdx);
                aRet <<= xRet;
            }
            else {
                DBG_ASSERT(0, "Empty section in redline portion! (end node immediately follows start node)");
            }
        }
    }
    else
    {
        aRet = GetPropertyValue( rPropertyName, *pRedline);
        if(!aRet.hasValue() &&
           ! rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_SUCCESSOR_DATA)))
            aRet = SwXTextPortion::getPropertyValue(rPropertyName);
    }
    return aRet;
}
/* -----------------------------19.12.00 15:16--------------------------------

 ---------------------------------------------------------------------------*/
void SwXRedlinePortion::Validate() throw( uno::RuntimeException )
{
    SwUnoCrsr* pUnoCrsr = GetCursor();
    if(!pUnoCrsr)
        throw uno::RuntimeException();
    //search for the redline
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    const SwRedlineTbl& rRedTbl = pDoc->GetRedlineTbl();
    sal_Bool bFound = FALSE;
    for(USHORT nRed = 0; nRed < rRedTbl.Count() && !bFound; nRed++)
        bFound = pRedline == rRedTbl[nRed];
    if(!bFound)
        throw uno::RuntimeException();
}
/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SAL_CALL SwXRedlinePortion::getImplementationId(  ) throw(uno::RuntimeException)
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
/* -----------------------------11.01.01 16:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any  SwXRedlinePortion::GetPropertyValue( const OUString& rPropertyName, const SwRedline& rRedline ) throw()
{
    uno::Any aRet;
    if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_AUTHOR)))
        aRet <<= OUString(rRedline.GetAuthorString());
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_DATE_TIME)))
    {
        aRet <<= lcl_DateTimeToUno(rRedline.GetTimeStamp());
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_COMMENT)))
        aRet <<= OUString(rRedline.GetComment());
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_TYPE)))
    {
        aRet <<= lcl_RedlineTypeToOUString(rRedline.GetType());
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_SUCCESSOR_DATA)))
    {
        if(rRedline.GetRedlineData().Next())
            aRet <<= lcl_GetSuccessorProperties(rRedline);
    }
    else if (rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_IDENTIFIER)))
    {
        OUStringBuffer sBuf;
        sBuf.append( sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(&rRedline) ) );
        aRet <<= sBuf.makeStringAndClear();
    }
    else if (rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_IS_IN_HEADER_FOOTER)))
    {
        sal_Bool bRet =
            rRedline.GetDoc()->IsInHeaderFooter( rRedline.GetPoint()->nNode );
        aRet.setValue(&bRet, ::getBooleanCppuType());
    }
    else if (rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_MERGE_LAST_PARA)))
    {
        sal_Bool bRet = !rRedline.IsDelLastPara();
        aRet.setValue( &bRet, ::getBooleanCppuType() );
    }
    return aRet;
}
/* -----------------------------11.01.01 11:22--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXRedlinePortion::CreateRedlineProperties(
    const SwRedline& rRedline, sal_Bool bIsStart ) throw()
{
    uno::Sequence< beans::PropertyValue > aRet(11);
    const SwRedlineData* pNext = rRedline.GetRedlineData().Next();
    beans::PropertyValue* pRet = aRet.getArray();

    OUStringBuffer sRedlineIdBuf;
    sRedlineIdBuf.append( sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(&rRedline) ) );

    sal_Int32 nPropIdx  = 0;
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_AUTHOR));
    pRet[nPropIdx++].Value <<= OUString(rRedline.GetAuthorString());
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_DATE_TIME));
    pRet[nPropIdx++].Value <<= lcl_DateTimeToUno(rRedline.GetTimeStamp());
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_COMMENT));
    pRet[nPropIdx++].Value <<= OUString(rRedline.GetComment());
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_TYPE));
    pRet[nPropIdx++].Value <<= lcl_RedlineTypeToOUString(rRedline.GetType());
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_IDENTIFIER));
    pRet[nPropIdx++].Value <<= sRedlineIdBuf.makeStringAndClear();
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_IS_COLLAPSED));
    sal_Bool bTmp = !rRedline.HasMark();
    pRet[nPropIdx++].Value.setValue(&bTmp, ::getBooleanCppuType()) ;

    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_IS_START));
    pRet[nPropIdx++].Value.setValue(&bIsStart, ::getBooleanCppuType()) ;

    bTmp = !rRedline.IsDelLastPara();
    pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_MERGE_LAST_PARA));
    pRet[nPropIdx++].Value.setValue(&bTmp, ::getBooleanCppuType()) ;

    SwNodeIndex* pNodeIdx = rRedline.GetContentIdx();
    if(pNodeIdx )
    {
        if ( 1 < ( pNodeIdx->GetNode().EndOfSectionIndex() - pNodeIdx->GetNode().GetIndex() ) )
        {
            uno::Reference<text::XText> xRet = new SwXRedlineText(rRedline.GetDoc(), *pNodeIdx);
            pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_TEXT));
            pRet[nPropIdx++].Value <<= xRet;
        }
        else {
            DBG_ASSERT(0, "Empty section in redline portion! (end node immediately follows start node)");
        }
    }
    if(pNext)
    {
        pRet[nPropIdx].Name = C2U(SW_PROP_NAME_STR(UNO_NAME_REDLINE_SUCCESSOR_DATA));
        pRet[nPropIdx++].Value <<= lcl_GetSuccessorProperties(rRedline);
    }
    aRet.realloc(nPropIdx);
    return aRet;
}
/*-- 11.01.01 17:06:07---------------------------------------------------

  -----------------------------------------------------------------------*/
TYPEINIT1(SwXRedline, SwClient);
SwXRedline::SwXRedline(SwRedline& rRedline, SwDoc& rDoc) :
    SwXText(&rDoc, CURSOR_REDLINE),
    pDoc(&rDoc),
    pRedline(&rRedline)
{
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
}
/*-- 11.01.01 17:06:08---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXRedline::~SwXRedline()
{
}
/*-- 11.01.01 17:06:08---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SwXRedline::getPropertySetInfo(  ) throw(uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo >  xRef =
        aSwMapProvider.GetPropertySet(PROPERTY_MAP_REDLINE)->getPropertySetInfo();
    return xRef;
}
/*-- 11.01.01 17:06:08---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::setPropertyValue( const OUString& rPropertyName, const uno::Any& aValue )
    throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
        lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_AUTHOR)))
    {
        DBG_ERROR("currently not available");
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_DATE_TIME)))
    {
        DBG_ERROR("currently not available");
//		util::DateTime aDT;
//		if(aValue >>= aDT)
//				pRedline->SetTimeStamp(lcl_DateTimeFromUno(aDT));
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_COMMENT)))
    {
        OUString sTmp; aValue >>= sTmp;
        pRedline->SetComment(sTmp);
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_TYPE)))
    {
        DBG_ERROR("currently not available");
        OUString sTmp; aValue >>= sTmp;
        if(!sTmp.getLength())
            throw lang::IllegalArgumentException();
//		pRedline->SetType(lcl_OUStringToRedlineType(sTmp));
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_SUCCESSOR_DATA)))
    {
        DBG_ERROR("currently not available");
/*		SwRedlineData* pNext = pRedline->GetRedlineData().Next();
        uno::Sequence<beans::PropertyValue> aValues;
        if(!(aValue =>> aValues) || !pNext)
            throw lang::IllegalArgumentException();

        const beans::PropertyValue* pValues = aValues.getConstArray();
        for(sal_Int32 nValue = 0; nValue < aValues.getLength(); nValue++)
        {
            if(pValues[nValue].Name.equalsAscii(UNO_NAME_REDLINE_AUTHOR.pName)
            {
                DBG_ERROR("currently not available");
            }
            else if(pValues[nValue].Name.equalsAscii(UNO_NAME_REDLINE_DATE_TIME.pName))
            {
                util::DateTime aDT;
                if(pValues[nValue].Value >>= aDT)
                    pNext->SetTimeStamp(lcl_DateTimeFromUno(aDT));
            }
            else if(pValues[nValue].Name.equalsAscii(UNO_NAME_REDLINE_COMMENT.pName))
            {
                OUString sTmp; pValues[nValue].Value >>= sTmp;
                pNext->SetComment(sTmp);
            }
            else if(pValues[nValue].Name.equalsAscii(UNO_NAME_REDLINE_TYPE.pName))
            {
                OUString sTmp; pValues[nValue].Value >>= sTmp;
                pNext->SetType(lcl_OUStringToRedlineType(sTmp);
            }
        }
*/	}
    else
    {
        throw lang::IllegalArgumentException();
    }
}
/*-- 11.01.01 17:06:08---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXRedline::getPropertyValue( const OUString& rPropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();
    uno::Any aRet;
    BOOL bStart = rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_START));
    if(bStart ||
        rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_END)))
    {
        uno::Reference<XInterface> xRet;
        SwNode* pNode = pRedline->GetNode();
        if(!bStart && pRedline->HasMark())
            pNode = pRedline->GetNode(FALSE);
        switch(pNode->GetNodeType())
        {
            case ND_SECTIONNODE:
            {
                SwSectionNode* pSectNode = pNode->GetSectionNode();
                DBG_ASSERT(pSectNode, "No section node!");
                xRet = SwXTextSections::GetObject( *pSectNode->GetSection().GetFmt() );
            }
            break;
            case ND_TABLENODE :
            {
                SwTableNode* pTblNode = pNode->GetTableNode();
                DBG_ASSERT(pTblNode, "No table node!");
                SwTable& rTbl = pTblNode->GetTable();
                SwFrmFmt* pTblFmt = rTbl.GetFrmFmt();
                xRet = SwXTextTables::GetObject( *pTblFmt );
            }
            break;
            case ND_TEXTNODE :
            {
                SwPosition* pPoint = 0;
                if(bStart || !pRedline->HasMark())
                    pPoint = pRedline->GetPoint();
                else
                    pPoint = pRedline->GetMark();
                const uno::Reference<text::XTextRange> xRange =
                    SwXTextRange::CreateXTextRange(*pDoc, *pPoint, 0);
                xRet = xRange.get();
            }
            break;
            default:
                DBG_ERROR("illegal node type");
        }
        aRet <<= xRet;
    }
    else if(rPropertyName.equalsAsciiL(SW_PROP_NAME(UNO_NAME_REDLINE_TEXT)))
    {
        SwNodeIndex* pNodeIdx = pRedline->GetContentIdx();
        if( pNodeIdx )
        {
            if ( 1 < ( pNodeIdx->GetNode().EndOfSectionIndex() - pNodeIdx->GetNode().GetIndex() ) )
            {
                uno::Reference<text::XText> xRet = new SwXRedlineText(pDoc, *pNodeIdx);
                aRet <<= xRet;
            }
            else {
                DBG_ASSERT(0, "Empty section in redline portion! (end node immediately follows start node)");
            }
        }
    }
    else
        aRet = SwXRedlinePortion::GetPropertyValue(rPropertyName, *pRedline);
    return aRet;
}
/*-- 11.01.01 17:06:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::addPropertyChangeListener(
    const OUString& /*aPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.01.01 17:06:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::removePropertyChangeListener(
    const OUString& /*aPropertyName*/, const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.01.01 17:06:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::addVetoableChangeListener(
    const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.01.01 17:06:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::removeVetoableChangeListener(
    const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
}
/*-- 11.01.01 17:06:10---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXRedline::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
      {
        pDoc = 0;
        pRedline = 0;
    }
}
/*-- 19.12.00 11:37:25---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXRedline::createEnumeration(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< container::XEnumeration > xRet;
    if(!pDoc)
        throw uno::RuntimeException();

    SwNodeIndex* pNodeIndex = pRedline->GetContentIdx();
    if(pNodeIndex)
    {
        SwPaM aPam(*pNodeIndex);
        aPam.Move(fnMoveForward, fnGoNode);
        ::std::auto_ptr<SwUnoCrsr> pUnoCursor(
            GetDoc()->CreateUnoCrsr(*aPam.Start(), sal_False));
        xRet = new SwXParagraphEnumeration(this, pUnoCursor, CURSOR_REDLINE);
    }
    return xRet;
}
/* -----------------------------19.12.00 12:34--------------------------------

 ---------------------------------------------------------------------------*/
uno::Type SwXRedline::getElementType(  ) throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<text::XTextRange>*)0);
}
/* -----------------------------19.12.00 12:34--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwXRedline::hasElements(  ) throw(uno::RuntimeException)
{
    if(!pDoc)
        throw uno::RuntimeException();
    return 0 != pRedline->GetContentIdx();
}
/* -----------------------------19.12.00 15:11--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< text::XTextCursor >  SwXRedline::createTextCursor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!pDoc)
        throw uno::RuntimeException();

    uno::Reference< text::XTextCursor >  	xRet;
    SwNodeIndex* pNodeIndex = pRedline->GetContentIdx();
    if(pNodeIndex)
    {
        SwPosition aPos(*pNodeIndex);
        SwXTextCursor* pCrsr = new SwXTextCursor(this, aPos, CURSOR_REDLINE, pDoc);
        SwUnoCrsr* pUnoCrsr = pCrsr->GetCrsr();
        pUnoCrsr->Move(fnMoveForward, fnGoNode);

        //steht hier eine Tabelle?
        SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
        SwCntntNode* pCont = 0;
        while( pTblNode )
        {
            pUnoCrsr->GetPoint()->nNode = *pTblNode->EndOfSectionNode();
            pCont = GetDoc()->GetNodes().GoNext(&pUnoCrsr->GetPoint()->nNode);
            pTblNode = pCont->FindTableNode();
        }
        if(pCont)
            pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
        xRet =  (text::XWordCursor*)pCrsr;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return xRet;
}
/* -----------------------------19.12.00 15:11--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< text::XTextCursor >  SwXRedline::createTextCursorByRange(
    const uno::Reference< text::XTextRange > & /*aTextPosition*/)
        throw( uno::RuntimeException )
{
    throw uno::RuntimeException();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXRedline::queryInterface( const uno::Type& rType )
    throw(uno::RuntimeException)
{
    uno::Any aRet = SwXText::queryInterface(rType);
    if(!aRet.hasValue())
    {
        aRet = SwXRedlineBaseClass::queryInterface(rType);
    }
    return aRet;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence<uno::Type> SwXRedline::getTypes()
    throw(uno::RuntimeException)
{
    uno::Sequence<uno::Type> aTypes = SwXText::getTypes();
    uno::Sequence<uno::Type> aBaseTypes = SwXRedlineBaseClass::getTypes();
    const uno::Type* pBaseTypes = aBaseTypes.getConstArray();
    sal_Int32 nCurType = aTypes.getLength();
    aTypes.realloc(aTypes.getLength() + aBaseTypes.getLength());
    uno::Type* pTypes = aTypes.getArray();
    for(sal_Int32 nType = 0; nType < aBaseTypes.getLength(); nType++)
        pTypes[nCurType++] = pBaseTypes[nType];
    return aTypes;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence<sal_Int8> SwXRedline::getImplementationId()
    throw(uno::RuntimeException)
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

