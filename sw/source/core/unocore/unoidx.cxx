/*************************************************************************
 *
 *  $RCSfile: unoidx.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-19 00:08:28 $
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
#include "core_pch.hxx"
#endif

#pragma hdrstop

#include <swtypes.hxx>
#include <cmdid.h>
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX //autogen
#include <docary.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#ifndef _PAGEDESC_HXX //autogen
#include <pagedesc.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _DOCTXM_HXX //autogen
#include <doctxm.hxx>
#endif
#ifndef _TXTTXMRK_HXX //autogen
#include <txttxmrk.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _NDTXT_HXX //autogen
#include <ndtxt.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_CHAPTERFORMAT_HPP_
#include <com/sun/star/text/ChapterFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PropertyAttribute_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _UNOIDX_HXX
#include <unoidx.hxx>
#endif
#ifndef _DOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _CHPFLD_HXX
#include <chpfld.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::rtl;

//-----------------------------------------------------------------------------
String lcl_AnyToString(uno::Any rVal) throw(IllegalArgumentException)
{
    if(rVal.getValueType() != ::getCppuType((OUString*)0))
        throw IllegalArgumentException();
    return String(*(OUString*)rVal.getValue());
}
//-----------------------------------------------------------------------------
sal_Int16 lcl_AnyToInt16(uno::Any rVal) throw(IllegalArgumentException)
{
    if(rVal.getValueType() != ::getCppuType((sal_Int16*)0))
        throw IllegalArgumentException();
    sal_Int16 nRet;
    rVal >>= nRet;
    return nRet;
}
//-----------------------------------------------------------------------------
sal_Bool lcl_AnyToBool(uno::Any rVal) throw(IllegalArgumentException)
{
    if(rVal.getValueType() != ::getCppuBooleanType())
        throw IllegalArgumentException();
    return *(sal_Bool*) rVal.getValue();
}
/******************************************************************************
 *
 ******************************************************************************/
SwTOXMark* lcl_GetMark(SwTOXType* pType, const SwTOXMark* pOwnMark)
{
    SwClientIter aIter(*pType);
    SwTOXMark* pMark = (SwTOXMark*)aIter.First(TYPE(SwTOXMark));
    while( pMark )
    {
        if(pMark == pOwnMark)
            return pMark;
        else
            pMark = (SwTOXMark*)aIter.Next();
    }
    return 0;
}

/******************************************************************
 * SwXDocumentIndex
 ******************************************************************/
/* -----------------20.06.98 11:06-------------------
 *
 * --------------------------------------------------*/
class SwDocIdxProperties_Impl
{
    SwTOXBase* 		pTOXBase;
public:
    SwTOXBase&		GetTOXBase() {return *pTOXBase;}
    SwDocIdxProperties_Impl(const SwTOXType* pType);
    ~SwDocIdxProperties_Impl(){delete pTOXBase;}
};
/* -----------------20.06.98 11:41-------------------
 *
 * --------------------------------------------------*/
SwDocIdxProperties_Impl::SwDocIdxProperties_Impl(const SwTOXType* pType)
{
    SwForm aForm(pType->GetType());
    pTOXBase = new SwTOXBase(pType, aForm,
                                TOX_MARK, pType->GetTypeName());
}
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXDocumentIndex::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXDocumentIndex::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}
/* -----------------------------06.04.00 15:01--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXDocumentIndex::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXDocumentIndex");
}
/* -----------------------------06.04.00 15:01--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXDocumentIndex::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.BaseIndex") == rServiceName ||
                ( TOX_INDEX == eTOXType && C2U("com.sun.star.text.DocumentIndex") == rServiceName) ||
                ( TOX_CONTENT == eTOXType && C2U("com.sun.star.text.ContentIndex") == rServiceName) ||
                ( TOX_USER == eTOXType && C2U("com.sun.star.text.UserDefinedIndex") == rServiceName) ||
                ( TOX_ILLUSTRATIONS == eTOXType && C2U("com.sun.star.text.IllustrationIndex") == rServiceName) ||
                ( TOX_TABLES == eTOXType && C2U("com.sun.star.text.TableIndex") == rServiceName) ||
                ( TOX_OBJECTS == eTOXType && C2U("com.sun.star.text.ObjectIndex") == rServiceName) ||
                ( TOX_AUTHORITIES == eTOXType && C2U("com.sun.star.text.Bibliography") == rServiceName);
}
/* -----------------------------06.04.00 15:01--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXDocumentIndex::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.BaseIndex");
    switch( eTOXType )
    {
        case TOX_INDEX:			pArray[1] = C2U("com.sun.star.text.DocumentIndex");break;
        case TOX_CONTENT:		pArray[1] = C2U("com.sun.star.text.ContentIndex");break;
        case TOX_TABLES:		pArray[1] = C2U("com.sun.star.text.TableIndex");break;
        case TOX_ILLUSTRATIONS:	pArray[1] = C2U("com.sun.star.text.IllustrationIndex");break;
        case TOX_OBJECTS:		pArray[1] = C2U("com.sun.star.text.ObjectIndex");break;
        case TOX_AUTHORITIES :	pArray[1] = C2U("com.sun.star.text.Bibliography");break;
        //case TOX_USER:
        default:
            pArray[1] = C2U("com.sun.star.text.UserDefinedIndex");
    }
    return aRet;
}
/*-- 14.12.98 09:35:03---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndex::SwXDocumentIndex(const SwTOXBaseSection* pB, SwDoc* pDc) :
    m_pDoc(pDc),
    aLstnrCntnr( (text::XTextContent*)this),
    pBase(pB),
    pProps(0),
    _pMap(0),
    pStyleAccess(0),
    pTokenAccess(0),
    bIsDescriptor(sal_False),
    eTOXType(TOX_USER)
{
    if(pBase && m_pDoc)
    {
        pBase->GetFmt()->Add(this);
        sal_uInt16 PropertyId;
        eTOXType = pBase->SwTOXBase::GetType();
        switch( eTOXType )
        {
            case TOX_INDEX:		PropertyId = PROPERTY_MAP_INDEX_IDX; break;
            case TOX_CONTENT:	PropertyId = PROPERTY_MAP_INDEX_CNTNT; break;
            case TOX_TABLES:		PropertyId = PROPERTY_MAP_INDEX_TABLES; break;
            case TOX_ILLUSTRATIONS:	PropertyId = PROPERTY_MAP_INDEX_ILLUSTRATIONS; break;
            case TOX_OBJECTS:		PropertyId = PROPERTY_MAP_INDEX_OBJECTS; break;
            case TOX_AUTHORITIES :	PropertyId = PROPERTY_MAP_BIBLIOGRAPHY; break;
            //case TOX_USER:
            default:
                PropertyId = PROPERTY_MAP_INDEX_USER;
        }
        _pMap = aSwMapProvider.GetPropertyMap(PropertyId);
    }
}
/* -----------------15.01.99 14:59-------------------
 *
 * --------------------------------------------------*/
SwXDocumentIndex::SwXDocumentIndex(TOXTypes eType, SwDoc& rDoc) :
    m_pDoc(0),
    aLstnrCntnr( (text::XTextContent*)this),
    pBase(0),
    pProps(new SwDocIdxProperties_Impl(rDoc.GetTOXType(eType, 0))),
    pStyleAccess(0),
    pTokenAccess(0),
    bIsDescriptor(sal_True),
    eTOXType(eType)
{
    sal_uInt16 PropertyId;
    switch(eType)
    {
        case TOX_INDEX:		PropertyId = PROPERTY_MAP_INDEX_IDX; break;
        case TOX_CONTENT:	PropertyId = PROPERTY_MAP_INDEX_CNTNT; break;
        case TOX_TABLES:		PropertyId = PROPERTY_MAP_INDEX_TABLES; break;
        case TOX_ILLUSTRATIONS:	PropertyId = PROPERTY_MAP_INDEX_ILLUSTRATIONS; break;
        case TOX_OBJECTS:		PropertyId = PROPERTY_MAP_INDEX_OBJECTS; break;
        case TOX_AUTHORITIES :	PropertyId = PROPERTY_MAP_BIBLIOGRAPHY; break;
        //case TOX_USER:
        default:
            PropertyId = PROPERTY_MAP_INDEX_USER;
    }
    _pMap = aSwMapProvider.GetPropertyMap(PropertyId);
}

/*-- 14.12.98 09:35:04---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndex::~SwXDocumentIndex()
{
    delete pProps;
}
/*-- 14.12.98 09:35:05---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXDocumentIndex::getServiceName(void) throw( RuntimeException )
{
    USHORT nObjectType = SW_SERVICE_TYPE_INDEX;
    switch(eTOXType)
    {
//		case TOX_INDEX:      		break;
        case TOX_USER:              nObjectType = SW_SERVICE_USER_INDEX;break;
        case TOX_CONTENT:           nObjectType = SW_SERVICE_CONTENT_INDEX;break;
        case TOX_ILLUSTRATIONS:     nObjectType = SW_SERVICE_INDEX_ILLUSTRATIONS;break;
        case TOX_OBJECTS:           nObjectType = SW_SERVICE_INDEX_OBJECTS;break;
        case TOX_TABLES:            nObjectType = SW_SERVICE_INDEX_TABLES;break;
        case TOX_AUTHORITIES:       nObjectType = SW_SERVICE_INDEX_BIBLIOGRAPHY;break;
    }
    return SwXServiceProvider::GetProviderName(nObjectType);
}
/*-- 14.12.98 09:35:05---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::update(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTOXBase* pTOXBase = (SwTOXBaseSection*)GetFmt()->GetSection();
    if(!pTOXBase)
        throw RuntimeException();
    ((SwTOXBaseSection*)pTOXBase)->Update();
    // Seitennummern eintragen
    ((SwTOXBaseSection*)pTOXBase)->UpdatePageNum();
}
/*-- 14.12.98 09:35:05---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XPropertySetInfo >  SwXDocumentIndex::getPropertySetInfo(void) throw( RuntimeException )
{
    Reference< XPropertySetInfo >  aRef = new SfxItemPropertySetInfo( _pMap );
    return aRef;
}
/*-- 14.12.98 09:35:05---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::setPropertyValue(const OUString& rPropertyName,
                                        const uno::Any& aValue)
        throw( UnknownPropertyException, PropertyVetoException,
                 IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                                        _pMap, rPropertyName);
    if(!pMap)
        throw UnknownPropertyException();
    SwTOXBase* pTOXBase;
    if(GetFmt())
        pTOXBase = (SwTOXBaseSection*)GetFmt()->GetSection();
    else if(bIsDescriptor)
        pTOXBase = &pProps->GetTOXBase();
    if(pTOXBase)
    {
        sal_uInt16 nCreate = pTOXBase->GetCreateType();
        sal_uInt16 nTOIOptions = 0;
        sal_uInt16 nOLEOptions = pTOXBase->GetOLEOptions();
        TOXTypes eTxBaseType = pTOXBase->GetTOXType()->GetType();
        if( eTxBaseType == TOX_INDEX )
            nTOIOptions = pTOXBase->GetOptions();
        SwForm	aForm(pTOXBase->GetTOXForm());
        sal_Bool bForm = sal_False;
        SfxItemSet* pAttrSet = 0;
        switch(pMap->nWID)
        {
            case WID_IDX_TITLE	:	 pTOXBase->SetTitle(lcl_AnyToString(aValue)); break;
            case WID_LEVEL		:
                pTOXBase->SetLevel(lcl_AnyToInt16(aValue));
            break;
            case WID_CREATE_FROM_MARKS                 :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_MARK: nCreate & ~TOX_MARK;
            break;
            case WID_CREATE_FROM_OUTLINE               :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_OUTLINELEVEL: nCreate & ~TOX_OUTLINELEVEL;
            break;
//			case WID_PARAGRAPH_STYLE_NAMES             :DBG_ERROR("not implemented")
//			break;
            case WID_CREATE_FROM_CHAPTER               :
                pTOXBase->SetFromChapter(lcl_AnyToBool(aValue));
            break;
            case WID_CREATE_FROM_LABELS                :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_SEQUENCE : nCreate & ~TOX_SEQUENCE;
            break;
            case WID_PROTECTED                         :
                pTOXBase->SetProtected(lcl_AnyToBool(aValue));
            break;
            case WID_USE_ALPHABETICAL_SEPARATORS:
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_ALPHA_DELIMITTER : nTOIOptions & ~TOI_ALPHA_DELIMITTER;
            break;
            case WID_USE_KEY_AS_ENTRY                  :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_KEY_AS_ENTRY : nTOIOptions & ~TOI_KEY_AS_ENTRY;
            break;
            case WID_USE_COMBINED_ENTRIES              :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_SAME_ENTRY : nTOIOptions & ~TOI_SAME_ENTRY;
            break;
            case WID_IS_CASE_SENSITIVE                 :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_CASE_SENSITIVE : nTOIOptions & ~TOI_CASE_SENSITIVE;
            break;
            case WID_USE_P_P                           :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_FF : nTOIOptions & ~TOI_FF;
            break;
            case WID_USE_DASH                          :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_DASH : nTOIOptions & ~TOI_DASH;
            break;
            case WID_USE_UPPER_CASE                    :
                nTOIOptions = lcl_AnyToBool(aValue) ?
                    nTOIOptions | TOI_INITIAL_CAPS : nTOIOptions & ~TOI_INITIAL_CAPS;
            break;
            case WID_IS_COMMA_SEPARATED :
                aForm.SetCommaSeparated(lcl_AnyToBool(aValue));
            break;
            case WID_LABEL_CATEGORY                    :
                pTOXBase->SetSequenceName(lcl_AnyToString(aValue));
            break;
            case WID_LABEL_DISPLAY_TYPE                :
            {
                sal_Int16 nVal = lcl_AnyToInt16(aValue);
                    sal_uInt16 nSet = CAPTION_COMPLETE;
                    switch (nVal)
                    {
                        case text::ChapterFormat::NAME_NUMBER: nSet = CAPTION_COMPLETE;
                        break;
                        case text::ChapterFormat::NUMBER	: nSet = CAPTION_NUMBER;
                        break;
                        case text::ChapterFormat::NAME : nSet = CAPTION_TEXT;
                        break;
                        default:
                            throw IllegalArgumentException();
                    }
                    pTOXBase->SetCaptionDisplay((SwCaptionDisplay)nVal);
            }
            break;
            case WID_USE_LEVEL_FROM_SOURCE             :
                pTOXBase->SetLevelFromChapter(lcl_AnyToBool(aValue));
            break;
//			case WID_LEVEL_FORMAT                      :
//				object readonly
//			break;
//			case WID_LEVEL_PARAGRAPH_STYLES            :DBG_ERROR("not implemented")
//				object readonly
//			break;
//			case WID_RECALC_TAB_STOPS                  :DBG_ERROR("not implemented")
//				lcl_AnyToBool(aValue) ?
//			break;
            //case WID_???                             :
            break;
            case WID_MAIN_ENTRY_CHARACTER_STYLE_NAME   :
                pTOXBase->SetMainEntryCharStyle(lcl_AnyToString(aValue));
            break;
            case WID_CREATE_FROM_TABLES                :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_TABLE : nCreate & ~TOX_TABLE;
            break;
            case WID_CREATE_FROM_TEXT_FRAMES           :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_FRAME : nCreate & ~TOX_FRAME;
            break;
            case WID_CREATE_FROM_GRAPHIC_OBJECTS       :
                nCreate = lcl_AnyToBool(aValue) ? nCreate | TOX_GRAPHIC : nCreate & ~TOX_GRAPHIC;
            break;
            case WID_CREATE_FROM_EMBEDDED_OBJECTS      :
                if(lcl_AnyToBool(aValue))
                    nCreate |= TOX_OLE;
                else
                    nCreate &= ~TOX_OLE;
            break;
            case WID_CREATE_FROM_STAR_MATH:
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_MATH : nOLEOptions & ~TOO_MATH;
            break;
            case WID_CREATE_FROM_STAR_IMAGE            :
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_IMAGE : nOLEOptions & ~TOO_IMAGE;
            break;
            case WID_CREATE_FROM_STAR_CHART            :
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_CHART : nOLEOptions & ~TOO_CHART;
            break;
            case WID_CREATE_FROM_STAR_CALC             :
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_CALC : nOLEOptions & ~TOO_CALC;
            break;
            case WID_CREATE_FROM_STAR_DRAW             :
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_DRAW_IMPRESS : nOLEOptions & ~TOO_DRAW_IMPRESS;
            break;
            case WID_CREATE_FROM_OTHER_EMBEDDED_OBJECTS:
                nOLEOptions = lcl_AnyToBool(aValue) ? nOLEOptions | TOO_OTHER : nOLEOptions & ~TOO_OTHER;
            break;
            case WID_PARA_HEAD             :
                bForm = sal_True;
                //Header steht an Pos 0
                aForm.SetTemplate( 0, lcl_AnyToString(aValue));
            break;
            case WID_PARA_SEP              :
                bForm = sal_True;
                aForm.SetTemplate( 1, lcl_AnyToString(aValue));
            break;

            case WID_PARA_LEV1             :
            case WID_PARA_LEV2             :
            case WID_PARA_LEV3             :
            case WID_PARA_LEV4             :
            case WID_PARA_LEV5             :
            case WID_PARA_LEV6             :
            case WID_PARA_LEV7             :
            case WID_PARA_LEV8             :
            case WID_PARA_LEV9             :
            case WID_PARA_LEV10            :
            {
                bForm = sal_True;
                // im sdbcx::Index beginnt Lebel 1 bei Pos 2 sonst bei Pos 1
                sal_uInt16 nLPos = pTOXBase->GetType() == TOX_INDEX ? 2 : 1;
                aForm.SetTemplate(nLPos + pMap->nWID - WID_PARA_LEV1, lcl_AnyToString(aValue));
            }
            break;
            /*
            case WID_FORM_SEP  :
                bForm = sal_True;
                //in Stichwort-Forms steht der Trenner an Pos 1
                aForm.SetPattern(1, lcl_AnyToString(aValue));
            break;
            case WID_FORM_LEV1 :
            case WID_FORM_LEV2 :
            case WID_FORM_LEV3 :
            case WID_FORM_LEV4 :
            case WID_FORM_LEV5 :
            case WID_FORM_LEV6 :
            case WID_FORM_LEV7 :
            case WID_FORM_LEV8 :
            case WID_FORM_LEV9 :
            case WID_FORM_LEV10:
            {
                bForm = sal_True;
                // im sdbcx::Index beginnt Lebel 1 bei Pos 2 sonst bei Pos 1
                sal_uInt16 nLPos = pTOXBase->GetType() == TOX_INDEX ? 2 : 1;
                aForm.SetPattern(nLPos + pMap->nWID - WID_FORM_LEV1, lcl_AnyToString(aValue));
            }
            break;
             */
            default:
                //this is for items only
                if(WID_PRIMARY_KEY > pMap->nWID)
                {
                    SfxItemPropertySet aPropSet(_pMap);
                    const SwAttrSet& rSet = m_pDoc->GetTOXBaseAttrSet(*pTOXBase);
                    pAttrSet = new SfxItemSet(rSet);
                    aPropSet.setPropertyValue(rPropertyName, aValue, *pAttrSet);

                    const SwSectionFmts& rSects = m_pDoc->GetSections();
                    const SwSectionFmt* pOwnFmt = GetFmt();
                    for(sal_uInt16 i = 0; i < rSects.Count(); i++)
                    {
                        const SwSectionFmt* pTmpFmt = rSects[ i ];
                        if(pTmpFmt == pOwnFmt)
                        {
                            m_pDoc->ChgSection( i, *(SwTOXBaseSection*)pTOXBase, pAttrSet );
                            break;
                        }
                    }
                }
        }
        pTOXBase->SetCreate(nCreate);
        pTOXBase->SetOLEOptions(nOLEOptions);
        if(pTOXBase->GetTOXType()->GetType() == TOX_INDEX)
            pTOXBase->SetOptions(nTOIOptions);
        if(bForm)
            pTOXBase->SetTOXForm(aForm);
        delete pAttrSet;
    }
    else
        throw RuntimeException();

}
/*-- 14.12.98 09:35:05---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXDocumentIndex::getPropertyValue(const OUString& rPropertyName)
    throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                                    _pMap, rPropertyName);
    if(!pMap)
        throw UnknownPropertyException();
    SwTOXBase* pTOXBase;
    if(GetFmt())
        pTOXBase = (SwTOXBaseSection*)GetFmt()->GetSection();
    else if(bIsDescriptor)
        pTOXBase = &pProps->GetTOXBase();
    if(pTOXBase)
    {
        sal_uInt16 nCreate = pTOXBase->GetCreateType();
        sal_uInt16 nTOIOptions = 0;
        sal_uInt16 nOLEOptions = pTOXBase->GetOLEOptions();
        if(pTOXBase->GetTOXType()->GetType() == TOX_INDEX)
            nTOIOptions = pTOXBase->GetOptions();
        const SwForm& rForm = pTOXBase->GetTOXForm();
        sal_Bool bBOOL = sal_True;
        sal_Bool bRet = sal_False;
        switch(pMap->nWID)
        {
            case WID_IDX_TITLE	:
                bBOOL = sal_False;
                aRet <<= OUString(pTOXBase->GetTitle());
            break;
            case WID_LEVEL		:
                bBOOL = sal_False;
                aRet <<= (sal_Int16)pTOXBase->GetLevel();
            break;
            case WID_CREATE_FROM_MARKS                 :
                bRet = 0 != (nCreate & TOX_MARK);
            break;
            case WID_CREATE_FROM_OUTLINE               :
                bRet = 0 != (nCreate & TOX_OUTLINELEVEL);
            break;
//			case WID_PARAGRAPH_STYLE_NAMES             :DBG_ERROR("not implemented")
//			break;
            case WID_CREATE_FROM_CHAPTER               :
                bRet = pTOXBase->IsFromChapter();
            break;
            case WID_CREATE_FROM_LABELS                :
                bRet = 0 != (nCreate & TOX_SEQUENCE);
            break;
            case WID_PROTECTED                         :
                bRet = pTOXBase->IsProtected();
            break;
            case WID_USE_ALPHABETICAL_SEPARATORS:
                bRet = 0 != (nTOIOptions & TOI_ALPHA_DELIMITTER);
            break;
            case WID_USE_KEY_AS_ENTRY                  :
                bRet = 0 != (nTOIOptions & TOI_KEY_AS_ENTRY);
            break;
            case WID_USE_COMBINED_ENTRIES              :
                bRet = 0 != (nTOIOptions & TOI_SAME_ENTRY);
            break;
            case WID_IS_CASE_SENSITIVE                 :
                bRet = 0 != (nTOIOptions & TOI_CASE_SENSITIVE);
            break;
            case WID_USE_P_P:
                bRet = 0 != (nTOIOptions & TOI_FF);
            break;
            case WID_USE_DASH                          :
                bRet = 0 != (nTOIOptions & TOI_DASH);
            break;
            case WID_USE_UPPER_CASE                    :
                bRet = 0 != (nTOIOptions & TOI_INITIAL_CAPS);
            break;
            case WID_IS_COMMA_SEPARATED :
                bRet = rForm.IsCommaSeparated();
            break;
            case WID_LABEL_CATEGORY                    :
                aRet <<= OUString(pTOXBase->GetSequenceName());
                bBOOL = sal_False;
            break;
            case WID_LABEL_DISPLAY_TYPE                :
            {
                bBOOL = sal_False;
                sal_Int16 nSet = text::ChapterFormat::NAME_NUMBER;
                switch (pTOXBase->GetCaptionDisplay())
                {
                    case CAPTION_COMPLETE:  nSet = text::ChapterFormat::NAME_NUMBER;break;
                    case CAPTION_NUMBER  :  nSet = text::ChapterFormat::NUMBER;	break;
                    case CAPTION_TEXT    :  nSet = text::ChapterFormat::NAME;      break;
                }
                aRet <<= nSet;
            }
            break;
            case WID_USE_LEVEL_FROM_SOURCE             :
                bRet = pTOXBase->IsLevelFromChapter();
            break;
            case WID_LEVEL_FORMAT                      :
            {
                Reference< container::XIndexReplace >  xTokenAcc =
                                    ((SwXDocumentIndex*)this)->GetTokenAccess();
                if(!xTokenAcc.is())
                    xTokenAcc = new SwXIndexTokenAccess_Impl(*
                                                (SwXDocumentIndex*)this);
                aRet.setValue(&xTokenAcc, ::getCppuType((const Reference<container::XIndexReplace>*)0));
                bBOOL = sal_False;
            }
            break;
            case WID_LEVEL_PARAGRAPH_STYLES            :
            {
                Reference< container::XIndexReplace >  xStyleAcc =
                                    ((SwXDocumentIndex*)this)->GetStyleAccess();
                if(!xStyleAcc.is())
                    xStyleAcc = new SwXIndexStyleAccess_Impl(*
                                                (SwXDocumentIndex*)this);
                aRet.setValue(&xStyleAcc, ::getCppuType((const Reference<container::XIndexReplace>*)0));
                bBOOL = sal_False;
            }
            break;
//			case WID_RECALC_TAB_STOPS                  :
//				tab stops are alway recalculated
//			break;
            //case WID_???                             :
            break;
            case WID_MAIN_ENTRY_CHARACTER_STYLE_NAME   :
                bBOOL = sal_False;
                aRet <<= OUString(pTOXBase->GetMainEntryCharStyle());
            break;
            case WID_CREATE_FROM_TABLES                :
                bRet = 0 != (nCreate & TOX_TABLE);
            break;
            case WID_CREATE_FROM_TEXT_FRAMES           :
                bRet = 0 != (nCreate & TOX_FRAME);
            break;
            case WID_CREATE_FROM_GRAPHIC_OBJECTS       :
                bRet = 0 != (nCreate & TOX_GRAPHIC);
            break;
            case WID_CREATE_FROM_EMBEDDED_OBJECTS      :
                bRet = 0 != (nCreate &= TOX_OLE);
            break;
            case WID_CREATE_FROM_STAR_MATH:
                bRet = 0 != (nOLEOptions & TOO_MATH);
            break;
            case WID_CREATE_FROM_STAR_IMAGE            :
                bRet = 0 != (nOLEOptions & TOO_IMAGE);
            break;
            case WID_CREATE_FROM_STAR_CHART            :
                bRet = 0 != (nOLEOptions & TOO_CHART);
            break;
            case WID_CREATE_FROM_STAR_CALC             :
                bRet = 0 != (nOLEOptions & TOO_CALC);
            break;
            case WID_CREATE_FROM_STAR_DRAW             :
                bRet = 0 != (nOLEOptions & TOO_DRAW_IMPRESS);
            break;
            case WID_CREATE_FROM_OTHER_EMBEDDED_OBJECTS:
                bRet = 0 != (nOLEOptions & TOO_OTHER);
            break;
            case WID_PARA_HEAD             :
                //Header steht an Pos 0
                aRet <<= OUString(rForm.GetTemplate( 0 ));
                bBOOL = sal_False;
            break;
            case WID_PARA_SEP              :
                aRet <<= OUString(rForm.GetTemplate( 1 ));
                bBOOL = sal_False;
            break;
            case WID_PARA_LEV1             :
            case WID_PARA_LEV2             :
            case WID_PARA_LEV3             :
            case WID_PARA_LEV4             :
            case WID_PARA_LEV5             :
            case WID_PARA_LEV6             :
            case WID_PARA_LEV7             :
            case WID_PARA_LEV8             :
            case WID_PARA_LEV9             :
            case WID_PARA_LEV10            :
            {
                // im sdbcx::Index beginnt Lebel 1 bei Pos 2 sonst bei Pos 1
                sal_uInt16 nLPos = pTOXBase->GetType() == TOX_INDEX ? 2 : 1;
                aRet <<= OUString(rForm.GetTemplate(nLPos + pMap->nWID - WID_PARA_LEV1));
                bBOOL = sal_False;
            }
            break;
            default:
                //this is for items only
                bBOOL = sal_False;
                if(WID_PRIMARY_KEY > pMap->nWID)
                {
                    SfxItemPropertySet aPropSet(_pMap);
                    const SwAttrSet& rSet = m_pDoc->GetTOXBaseAttrSet(*pTOXBase);
                    aRet = aPropSet.getPropertyValue(rPropertyName, rSet);
                }
        }
        if(bBOOL)
            aRet.setValue(&bRet, ::getCppuBooleanType());
    }
    return aRet;
}
/*-- 14.12.98 09:35:06---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::addPropertyChangeListener(const OUString& PropertyName, const Reference< XPropertyChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 09:35:06---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::removePropertyChangeListener(const OUString& PropertyName, const Reference< XPropertyChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 09:35:06---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::addVetoableChangeListener(const OUString& PropertyName, const Reference< XVetoableChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 09:35:07---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::removeVetoableChangeListener(const OUString& PropertyName, const Reference< XVetoableChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 09:35:07---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    if(pOld && pOld->Which() == RES_REMOVE_UNO_OBJECT &&
        (void*)GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            ((SwModify*)GetRegisteredIn())->Remove(this);
    else
        ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        aLstnrCntnr.Disposing();
}
/* -----------------18.02.99 13:39-------------------
 *
 * --------------------------------------------------*/
void SwXDocumentIndex::attachToRange(const Reference< text::XTextRange > & xTextRange)
    throw( IllegalArgumentException, RuntimeException )
{
    if(!bIsDescriptor)
        throw RuntimeException();
    Reference<XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    SwXTextCursor* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                SwXTextRange::getUnoTunnelId());
        pCursor = (SwXTextCursor*)xRangeTunnel->getSomething(
                                SwXTextCursor::getUnoTunnelId());
    }

    SwDoc* pDoc = pRange ? (SwDoc*)pRange->GetDoc() : pCursor ? (SwDoc*)pCursor->GetDoc() : 0;
    if(pDoc )
    {
        SwUnoInternalPaM aPam(*pDoc);
        //das muss jetzt sal_True liefern
        SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange);
        //TODO: Unterscheidung innerhalb von Benutzerverzeichnissen einbauen
//		const SwTOXType* pType = pDoc->GetTOXType(pProps->eToxType, 0);
//		DBG_ASSERT(pType, "Wieso gibt es diesen Typ nicht?" )

            const SwTOXBase* pOld = pDoc->GetCurTOX( *aPam.Start() );
            if(!pOld)
            {
                UnoActionContext aAction(pDoc);
                if(aPam.HasMark())
                    pDoc->DeleteAndJoin(aPam);

//				SwForm aForm(pType->GetType());
//				SwTOXBase* pTOXBase = new SwTOXBase(pType, aForm,
//							TOX_MARK, pType->GetTypeName());
                SwTOXBase& rTOXBase = pProps->GetTOXBase();
                //TODO: Descriptor- interface!
/*				sal_uInt16 nCreate = pTOXBase->GetCreateType();
                sal_uInt16 nOLEOptions = pTOXBase->GetOLEOptions();
                sal_uInt16 nTOIOptions = 0;
                const SfxItemPropertyMap* pMap = _pMap;
                while(pMap->nWID)
                {
                    // hier werden alle drei Typen verarbeitet, es sollten aber nie falsche Einstellungen m�glich sein
                    switch(pMap->nWID)
                    {
                        case WID_IDX_TITLE:
                            pTOXBase->SetTitle(pProps->sTitle);
                        break;
                        case WID_LEVEL                             :
                            pTOXBase->SetLevel(pProps->nLevels);
                        break;
                        case WID_CREATE_FROM_MARKS                 :
                            nCreate = pProps->bMarks ?
                                nCreate | TOX_MARK: nCreate & ~TOX_MARK;
                        break;
                        case WID_CREATE_FROM_OUTLINE               :
                            nCreate = pProps->bOutlines ?
                                nCreate | TOX_OUTLINELEVEL: nCreate & ~TOX_OUTLINELEVEL;
                        break;
//						case WID_PARAGRAPH_STYLE_NAMES             :
//							DBG_ERROR("not implemented")
//							//pProps->bParaStyles = lcl_AnyToBool(aValue);
//						break;
                        case WID_CREATE_FROM_CHAPTER               :
                            pTOXBase->SetFromChapter(pProps->bFromChapter);
                        break;
                        case WID_CREATE_FROM_LABELS                :
                            nCreate = pProps->bFromLabels ?
                                nCreate | TOX_SEQUENCE : nCreate & ~TOX_SEQUENCE;
                        break;
                        case WID_PROTECTED                         :
                            pTOXBase->SetProtected(pProps->bProtected);
                        break;
                        case WID_USE_ALPHABETICAL_SEPARATORS:
                            nTOIOptions = pProps->bUseAlphabeticalSeparators ?
                                nTOIOptions | TOI_ALPHA_DELIMITTER : nTOIOptions & ~TOI_ALPHA_DELIMITTER;
                        break;
                        case WID_USE_KEY_AS_ENTRY                  :
                            nTOIOptions = pProps->bKeyAsEntry ?
                                nTOIOptions | TOI_KEY_AS_ENTRY : nTOIOptions & ~TOI_KEY_AS_ENTRY;
                        break;
                        case WID_USE_COMBINED_ENTRIES              :
                            nTOIOptions = pProps->bCombineEntries ?
                                nTOIOptions | TOI_SAME_ENTRY : nTOIOptions & ~TOI_SAME_ENTRY;
                        break;
                        case WID_IS_CASE_SENSITIVE                 :
                            nTOIOptions = pProps->bCaseSensitive ?
                                nTOIOptions | TOI_CASE_SENSITIVE :
                                    nTOIOptions & ~TOI_CASE_SENSITIVE;
                        break;
                        case WID_USE_P_P                           :
                            nTOIOptions = pProps->bUsePP ?
                                nTOIOptions | TOI_FF : nTOIOptions & ~TOI_FF;
                        break;
                        case WID_USE_DASH                          :
                            nTOIOptions = pProps->bUseDash ?
                                nTOIOptions | TOI_DASH : nTOIOptions & ~TOI_DASH;
                        break;
                        case WID_USE_UPPER_CASE                    :
                            nTOIOptions = pProps->bUseUpperCase ?
                                nTOIOptions | TOI_INITIAL_CAPS : nTOIOptions & ~TOI_INITIAL_CAPS;
                        break;
                        case WID_IS_COMMA_SEPARATED :
                            aForm.SetCommaSeparated(pProps->bIsCommaSeparated);
                        break;
                        case WID_LABEL_CATEGORY                    :
                            pTOXBase->SetSequenceName(pProps->sLabelCategory);
                        break;
                        case WID_LABEL_DISPLAY_TYPE                :
                            pTOXBase->SetCaptionDisplay((SwCaptionDisplay)pProps->nDisplayType);
                        break;
                        case WID_USE_LEVEL_FROM_SOURCE             :
                            pTOXBase->SetFromChapter(pProps->bLevelFromChapter);
                        break;
                        case WID_LEVEL_PARAGRAPH_STYLES            :
                        {
                            for(sal_uInt16 i = 0; i < MAXLEVEL; i++)
                                pTOXBase->SetStyleNames(pProps->sStyleNames[i], i);
                        }
                        break;
//						case WID_RECALC_TAB_STOPS                  :
//							DBG_ERROR("not implemented")
// 							now always recalculated
//						break;
                        //case WID_???                             :
                        break;
                        case WID_MAIN_ENTRY_CHARACTER_STYLE_NAME   :
                            pTOXBase->SetMainEntryCharStyle(pProps->sMainEntryStyleName);
                        break;
                        case WID_CREATE_FROM_TABLES                :
                            nCreate = pProps->bTables ?
                                nCreate | TOX_TABLE : nCreate & ~TOX_TABLE;
                        break;
                        case WID_CREATE_FROM_TEXT_FRAMES           :
                            nCreate = pProps->bTextFrames ?
                                nCreate | TOX_FRAME : nCreate & ~TOX_FRAME;
                        break;
                        case WID_CREATE_FROM_GRAPHIC_OBJECTS       :
                            nCreate = pProps->bGraphics ?
                                nCreate | TOX_GRAPHIC : nCreate & ~TOX_GRAPHIC;
                        break;
                        case WID_CREATE_FROM_EMBEDDED_OBJECTS      :
                            nCreate = pProps->bEmbeddedObjects ?
                                nCreate | TOX_OLE : nCreate &~ TOX_OLE;
                        break;
                        case WID_CREATE_FROM_STAR_MATH             :
                            nOLEOptions = pProps->bFromStarMath ?
                                nOLEOptions | TOO_MATH : nOLEOptions & ~TOO_MATH;
                        break;
                        case WID_CREATE_FROM_STAR_IMAGE            :
                            nOLEOptions = pProps->bFromStarImage ?
                                nOLEOptions | TOO_IMAGE : nOLEOptions & ~TOO_IMAGE;
                        break;
                        case WID_CREATE_FROM_STAR_CHART            :
                            nOLEOptions = pProps->bFromStarChart ?
                                nOLEOptions | TOO_CHART : nOLEOptions & ~TOO_CHART;
                        break;
                        case WID_CREATE_FROM_STAR_CALC             :
                            nOLEOptions = pProps->bFromStarCalc ?
                                nOLEOptions | TOO_CALC : nOLEOptions & ~TOO_CALC;
                        break;
                        case WID_CREATE_FROM_STAR_DRAW             :
                            nOLEOptions = pProps->bFromStarDraw ?
                                nOLEOptions | TOO_DRAW_IMPRESS : nOLEOptions & ~TOO_DRAW_IMPRESS;
                        break;
                        case WID_CREATE_FROM_OTHER_EMBEDDED_OBJECTS:
                            nOLEOptions = pProps->bFromOtherEmbedded ?
                                nOLEOptions | TOO_OTHER : nOLEOptions & ~TOO_OTHER;
                        break;
                        case WID_PARA_HEAD:
                            //Header steht an Pos 0
                            aForm.SetTemplate( 0, pProps->sParaStyleHeading);
                        break;
                        case WID_PARA_SEP              :
                            //in Stichwort-Forms steht der Trenner an Pos 1
                            aForm.SetTemplate( 1, pProps->sParaStyleSeparator);
                        break;
                        case WID_PARA_LEV1:
                        case WID_PARA_LEV2:
                        case WID_PARA_LEV3:
                        case WID_PARA_LEV4:
                        case WID_PARA_LEV5:
                        case WID_PARA_LEV6:
                        case WID_PARA_LEV7:
                        case WID_PARA_LEV8:
                        case WID_PARA_LEV9:
                        case WID_PARA_LEV10:
                        {
                            // im sdbcx::Index beginnt Level 1 bei Pos 2 sonst bei Pos 0
                            sal_uInt16 nLPos = pType->GetType() == TOX_INDEX ? 2 : 1;
                            aForm.SetTemplate(nLPos + pMap->nWID - WID_PARA_LEV1,
                                                        pProps->sParaStyles[pMap->nWID - WID_PARA_LEV1]);
                        }
                        break;
                        case WID_LEVEL_FORMAT:
                        {
                            sal_uInt16 nStart = pType->GetType() == TOX_INDEX ? 2 : 1;
                            for(sal_uInt16 i = nStart; i < aForm.GetFormMax(); i++)
                            {
                                aForm.SetPattern(i,	pProps->sLevelPatterns[i - 1]);
                            }
                        }
                        break;
                    }
                    pMap++;
                }
                switch(pType->GetType())
                {
                    case TOX_INDEX:
                        pTOXBase->SetOptions(nTOIOptions);
                    break;
                    case TOX_CONTENT:
                    //break;
                    case TOX_USER:
                        if(!nCreate)
                            nCreate = TOX_MARK;
                    break;
                }
                pTOXBase->SetOLEOptions(nOLEOptions);
                pTOXBase->SetCreate(nCreate);
                pTOXBase->SetTOXForm(aForm);
                //TODO: apply Section attributes (columns and background)
*/				const SwTOXBaseSection* pTOX = pDoc->InsertTableOf(
                                    *aPam.GetPoint(), rTOXBase, 0, sal_True );

//				if(pProps->sName.Len())
//					pDoc->SetTOXBaseName( *pTOX, pProps->sName );
                    pDoc->SetTOXBaseName( *pTOX, pProps->GetTOXBase().GetTOXName() );


                // Seitennummern eintragen
                pBase = (const SwTOXBaseSection*)pTOX;
                pBase->GetFmt()->Add(this);
                ((SwTOXBaseSection*)pTOX)->UpdatePageNum();

//				delete pTOXBase;
            }
            else
                throw IllegalArgumentException();

        DELETEZ(pProps);
        m_pDoc = pDoc;
        bIsDescriptor = sal_False;
    }
}
/*-- 15.01.99 14:23:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::attach(const Reference< text::XTextRange > & xTextRange)
    throw( IllegalArgumentException, RuntimeException )
{
}
/*-- 15.01.99 14:23:56---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXDocumentIndex::getAnchor(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XTextRange >   xRet;
    if(GetRegisteredIn())
    {
        SwSectionFmt*  pSectFmt = GetFmt();
        const SwNodeIndex* pIdx;
        if( 0 != ( pIdx = pSectFmt->GetCntnt().GetCntntIdx() ) &&
            pIdx->GetNode().GetNodes().IsDocNodes() )
        {
            SwPaM aPaM(*pIdx);
            aPaM.Move( fnMoveForward, fnGoCntnt );
            aPaM.SetMark();
            aPaM.GetPoint()->nNode = *pIdx->GetNode().EndOfSectionNode();
            aPaM.Move( fnMoveBackward, fnGoCntnt );
            xRet = ::CreateTextRangeFromPosition(pSectFmt->GetDoc(),
                *aPaM.GetMark(), aPaM.GetPoint());
        }
    }
    else
        throw RuntimeException();
    return xRet;
}
/*-- 15.01.99 15:46:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_RemoveChildSections(SwSectionFmt& rParentFmt)
{
    SwSections aTmpArr;
    SwDoc* pDoc = rParentFmt.GetDoc();
    sal_uInt16 nCnt = rParentFmt.GetChildSections(aTmpArr,SORTSECT_POS);
    if( nCnt )
    {
        for( sal_uInt16 n = 0; n < nCnt; ++n )
            if( aTmpArr[n]->GetFmt()->IsInNodesArr() )
            {
                SwSectionFmt* pFmt = aTmpArr[n]->GetFmt();
                lcl_RemoveChildSections(*pFmt);
                pDoc->DelSectionFmt( pFmt );
            }
    }
}
void SwXDocumentIndex::dispose(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(GetRegisteredIn())
    {
        SwSectionFmt*  pSectFmt = GetFmt();
        pSectFmt->GetDoc()->DeleteTOX( *(SwTOXBaseSection*)pSectFmt->GetSection(), sal_True);
    }
    else
        throw RuntimeException();
}
/*-- 15.01.99 15:46:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::addEventListener(const Reference< XEventListener > & aListener) throw( RuntimeException )
{
    if(!GetRegisteredIn())
        throw RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 15.01.99 15:46:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndex::removeEventListener(const Reference< XEventListener > & aListener) throw( RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw RuntimeException();
}
/* -----------------30.07.99 11:28-------------------

 --------------------------------------------------*/
OUString SwXDocumentIndex::getName(void) throw( RuntimeException )
{
    SwSectionFmt* pSectionFmt = GetFmt();
    OUString uRet;
    if(bIsDescriptor)
    {
        uRet = OUString(pProps->GetTOXBase().GetTOXName());
    }
    else if(pSectionFmt)
    {
        uRet = OUString(pSectionFmt->GetSection()->GetName());
    }
    else
        throw RuntimeException();
    return uRet;
}
/* -----------------30.07.99 11:28-------------------

 --------------------------------------------------*/
void SwXDocumentIndex::setName(const OUString& rName) throw( RuntimeException )
{
    SwSectionFmt* pSectionFmt = GetFmt();
    String sNewName(rName);
    sal_Bool bExcept = sal_False;
    if(!sNewName.Len())
        bExcept = sal_True;
    else if(bIsDescriptor)
    {
        pProps->GetTOXBase().SetTOXName(sNewName);
    }
    else if(!pSectionFmt ||
        !pSectionFmt->GetDoc()->SetTOXBaseName(
                *(SwTOXBaseSection*)pSectionFmt->GetSection(), sNewName))
        bExcept = sal_True;

    if(bExcept)
        throw RuntimeException();
}

/******************************************************************
 * SwXDocumentIndexMark
 ******************************************************************/
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXDocumentIndexMark::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXDocumentIndexMark::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}

TYPEINIT1(SwXDocumentIndexMark, SwClient)
/* -----------------------------06.04.00 15:07--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXDocumentIndexMark::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXDocumentIndexMark");
}
/* -----------------------------06.04.00 15:07--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXDocumentIndexMark::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return !rServiceName.compareToAscii("com.sun.star.text.DocumentIndexMark")||
        !rServiceName.compareToAscii("com.sun.star.text.TextContent");
}
/* -----------------------------06.04.00 15:07--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXDocumentIndexMark::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.DocumentIndexMark");
    pArray[1] = C2U("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 14.12.98 10:25:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexMark::SwXDocumentIndexMark(TOXTypes eToxType) :
    m_pDoc(0),
    aLstnrCntnr( (text::XTextContent*)this),
    m_pTOXMark(0),
    nLevel(USHRT_MAX),
    eType(eToxType),
    bIsDescriptor(sal_True)
{
    InitMap(eToxType);
}
/*-- 14.12.98 10:25:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexMark::SwXDocumentIndexMark(const SwTOXType* pType,
                                    const SwTOXMark* pMark,
                                    SwDoc* pDc) :
    SwClient((SwTOXType*)pType),
    aLstnrCntnr( (text::XTextContent*)this),
    m_pDoc(pDc),
    m_pTOXMark(pMark),
    nLevel(USHRT_MAX),
    eType(pType->GetType()),
    bIsDescriptor(sal_False)
{
    InitMap(eType);
}
/*-- 14.12.98 10:25:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexMark::~SwXDocumentIndexMark()
{

}
/* -----------------21.04.99 09:36-------------------
 *
 * --------------------------------------------------*/
void SwXDocumentIndexMark::InitMap(TOXTypes eToxType)
{
    sal_uInt16 nMapId = PROPERTY_MAP_USER_MARK; //case TOX_USER:
    switch( eToxType )
    {
        case TOX_INDEX:
            nMapId = PROPERTY_MAP_INDEX_MARK ;
        break;
        case TOX_CONTENT:
            nMapId = PROPERTY_MAP_CNTIDX_MARK;
        break;
        //case TOX_USER:
    }
    _pMap = aSwMapProvider.GetPropertyMap(nMapId);
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXDocumentIndexMark::getMarkEntry(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    OUString sRet;
    if(pType)
    {
        SwTOXMark* pCurMark = lcl_GetMark(pType, GetTOXMark());
        SwTOXMark aMark(*pCurMark);
        sRet = OUString(aMark.GetAlternativeText());
    }
    else if(bIsDescriptor)
         sRet = sAltText;
    else
        throw RuntimeException();
    return sRet;
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::setMarkEntry(const OUString& rIndexEntry) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    if(pType)
    {
        SwTOXMark* pCurMark = lcl_GetMark(pType, GetTOXMark());
        SwTOXMark aMark(*pCurMark);
        aMark.SetAlternativeText(rIndexEntry);
        SwTxtTOXMark* pTxtMark = pCurMark->GetTxtTOXMark();
        SwPaM aPam(pTxtMark->GetTxtNode(), *pTxtMark->GetStart());
        aPam.SetMark();
        if(pTxtMark->GetEnd())
        {
            aPam.GetPoint()->nContent = *pTxtMark->GetEnd();
        }
        else
            aPam.GetPoint()->nContent++;

        //die alte Marke loeschen
        m_pDoc->Delete(pCurMark);
        m_pTOXMark = pCurMark = 0;

        sal_Bool bInsAtPos = aMark.IsAlternativeText();
        const SwPosition *pStt = aPam.Start(),
                            *pEnd = aPam.End();
        SwUnoCrsr* pCrsr = 0;
        if( bInsAtPos )
        {
            SwPaM aTmp( *pStt );
            m_pDoc->Insert( aTmp, aMark, 0 );
            pCrsr = m_pDoc->CreateUnoCrsr( *aTmp.Start() );
            pCrsr->Left(1);
        }
        else if( *pEnd != *pStt )
        {
            m_pDoc->Insert( aPam, aMark, SETATTR_DONTEXPAND );
            pCrsr = m_pDoc->CreateUnoCrsr( *aPam.Start() );
        }
        //und sonst - Marke geloescht?

        if(pCrsr)
        {
            SwTxtAttr* pTxtAttr = pCrsr->GetNode()->GetTxtNode()->GetTxtAttr(
                        pCrsr->GetPoint()->nContent, RES_TXTATR_TOXMARK);
            if(pTxtAttr)
                m_pTOXMark = &pTxtAttr->GetTOXMark();
        }
    }
    else if(bIsDescriptor)
    {
        sAltText = rIndexEntry;
    }
    else
        throw RuntimeException();
}
/* -----------------18.02.99 13:40-------------------
 *
 * --------------------------------------------------*/
void SwXDocumentIndexMark::attachToRange(const Reference< text::XTextRange > & xTextRange)
                throw( IllegalArgumentException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!bIsDescriptor)
        throw RuntimeException();

    Reference<XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    SwXTextCursor* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                SwXTextRange::getUnoTunnelId());
        pCursor = (SwXTextCursor*)xRangeTunnel->getSomething(
                                SwXTextCursor::getUnoTunnelId());
    }

    SwDoc* pDoc = pRange ? (SwDoc*)pRange->GetDoc() : pCursor ? (SwDoc*)pCursor->GetDoc() : 0;

    if(pDoc )
    {
        const SwTOXType* pTOXType = 0;
        switch(eType)
        {
            case TOX_INDEX:
            case TOX_CONTENT:
                pTOXType = pDoc->GetTOXType( eType, 0 );
            break;
            case TOX_USER:
            {
                if(!sUserIndexName.Len())
                    pTOXType = pDoc->GetTOXType( eType, 0 );
                else
                {
                    sal_uInt16 nCount = pDoc->GetTOXTypeCount( eType);
                    for(sal_uInt16 i = 0; i < nCount; i++)
                    {
                        const SwTOXType* pTemp = pDoc->GetTOXType( eType, i );
                        if(sUserIndexName == pTemp->GetTypeName())
                        {
                            pTOXType = pTemp;
                            break;
                        }
                    }
                }
            }
            break;
        }
        if(!pTOXType)
            throw IllegalArgumentException();
        ((SwTOXType*)pTOXType)->Add(this);

        SwUnoInternalPaM aPam(*pDoc);
        //das muss jetzt sal_True liefern
        SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange);
        SwTOXMark* pMark = new SwTOXMark(pTOXType);
        if(sAltText.Len())
            pMark->SetAlternativeText(sAltText);
        switch(eType)
        {
            case TOX_INDEX:
                if(sPrimaryKey.Len())
                    pMark->SetPrimaryKey(sPrimaryKey);
                if(sSecondaryKey.Len())
                    pMark->SetSecondaryKey(sSecondaryKey);
            break;
            case TOX_CONTENT:
                if(USHRT_MAX != nLevel)
                    pMark->SetLevel(nLevel);
            break;
        }
        UnoActionContext aAction(pDoc);
        sal_Bool bMark = *aPam.GetPoint() != *aPam.GetMark();
        // Marks ohne Alternativtext ohne selektierten Text koennen nicht eingefuegt werden,
        // deshalb hier ein Leerzeichen - ob das die ideale Loesung ist?
        if(!bMark && !pMark->GetAlternativeText().Len())
            pMark->SetAlternativeText(String::CreateFromAscii(" "));
        pDoc->Insert(aPam, *pMark, SETATTR_DONTEXPAND);
        if(bMark && *aPam.GetPoint() > *aPam.GetMark())
            aPam.Exchange();
        SwUnoCrsr* pCrsr = pDoc->CreateUnoCrsr( *aPam.Start() );
        if(!bMark)
        {
            pCrsr->SetMark();
            pCrsr->Left(1);
        }
        SwTxtAttr* pTxtAttr = pCrsr->GetNode()->GetTxtNode()->GetTxtAttr(
                        pCrsr->GetPoint()->nContent, RES_TXTATR_TOXMARK);
        delete pCrsr;
        if(pTxtAttr)
        {
            m_pTOXMark = &pTxtAttr->GetTOXMark();
            m_pDoc = pDoc;
            bIsDescriptor = sal_False;
        }
        else
            throw RuntimeException();
    }
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::attach(const Reference< text::XTextRange > & xTextRange)
                throw( IllegalArgumentException, RuntimeException )
{
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXDocumentIndexMark::getAnchor(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XTextRange >  aRet;
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    if(pType)
    {
        SwTOXMark* pCurMark = lcl_GetMark(pType, GetTOXMark());
        if(pCurMark && pCurMark->GetTxtTOXMark())
        {
            SwTxtTOXMark* pTxtMark = pCurMark->GetTxtTOXMark();
            SwPaM aPam(pTxtMark->GetTxtNode(), *pTxtMark->GetStart());
            aPam.SetMark();
            if(pTxtMark->GetEnd())
            {
                aPam.GetPoint()->nContent = *pTxtMark->GetEnd();
            }
            else
                aPam.GetPoint()->nContent++;
            Reference< frame::XModel >  xModel = m_pDoc->GetDocShell()->GetBaseModel();
            Reference< text::XTextDocument > xTDoc(xModel, uno::UNO_QUERY);
            aRet = new SwXTextRange(aPam, xTDoc->getText());
        }
    }
    if(!aRet.is())
        throw RuntimeException();
    return aRet;
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::dispose(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    if(pType)
    {
        SwTOXMark* pTMark = lcl_GetMark(pType, GetTOXMark());
        m_pDoc->Delete(pTMark);
    }
    else
        throw RuntimeException();
}
/*-- 14.12.98 10:25:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::addEventListener(const Reference< XEventListener > & aListener)
    throw( RuntimeException )
{
    if(!GetRegisteredIn())
        throw RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::removeEventListener(const Reference< XEventListener > & aListener)
    throw( RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw RuntimeException();
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XPropertySetInfo >  SwXDocumentIndexMark::getPropertySetInfo(void)
    throw( RuntimeException )
{
    uno::Reference< beans::XPropertySetInfo >  xInfo = new SfxItemPropertySetInfo(_pMap);
    // extend PropertySetInfo!
    const uno::Sequence<beans::Property> aPropSeq = xInfo->getProperties();
    Reference< XPropertySetInfo >  xRef = new SfxExtItemPropertySetInfo(
        aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARAGRAPH_EXTENSIONS),
        aPropSeq );
    return xRef;
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::setPropertyValue(const OUString& rPropertyName,
                                            const uno::Any& aValue)
    throw( UnknownPropertyException, PropertyVetoException,
        IllegalArgumentException, WrappedTargetException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                                        _pMap, rPropertyName);
    if(!pMap)
        throw UnknownPropertyException();
    if(pMap->nFlags & PropertyAttribute::READONLY)
        throw IllegalArgumentException();

    if(pType)
    {
        SwTOXMark* pCurMark = lcl_GetMark(pType, GetTOXMark());
        if(pCurMark)
        {
            SwTOXMark aMark(*pCurMark);
            switch(pMap->nWID)
            {
//					case WID_ALT_TEXT:
//						aMark.SetAlternativeText(lcl_AnyToString(aValue));
//					break;
                case WID_LEVEL:
                    aMark.SetLevel(min(MAXLEVEL, (sal_Int8)lcl_AnyToInt16(aValue)));
                break;
                case WID_PRIMARY_KEY  :
                    aMark.SetPrimaryKey(lcl_AnyToString(aValue));
                break;
                case WID_SECONDARY_KEY:
                    aMark.SetSecondaryKey(lcl_AnyToString(aValue));
                break;
            }

            SwTxtTOXMark* pTxtMark = pCurMark->GetTxtTOXMark();
            SwPaM aPam(pTxtMark->GetTxtNode(), *pTxtMark->GetStart());
            aPam.SetMark();
            if(pTxtMark->GetEnd())
            {
                aPam.GetPoint()->nContent = *pTxtMark->GetEnd();
            }
            else
                aPam.GetPoint()->nContent++;

            //die alte Marke loeschen
            m_pDoc->Delete(pCurMark);
            m_pTOXMark = pCurMark = 0;

            sal_Bool bInsAtPos = aMark.IsAlternativeText();
            const SwPosition *pStt = aPam.Start(),
                                *pEnd = aPam.End();
            SwUnoCrsr* pCrsr = 0;
            if( bInsAtPos )
            {
                SwPaM aTmp( *pStt );
                m_pDoc->Insert( aTmp, aMark, 0 );
                pCrsr = m_pDoc->CreateUnoCrsr( *aTmp.Start() );
                pCrsr->Left(1);
            }
            else if( *pEnd != *pStt )
            {
                m_pDoc->Insert( aPam, aMark, SETATTR_DONTEXPAND );
                pCrsr = m_pDoc->CreateUnoCrsr( *aPam.Start() );
            }
            //und sonst - Marke geloescht?

            if(pCrsr)
            {
                SwTxtAttr* pTxtAttr = pCrsr->GetNode()->GetTxtNode()->GetTxtAttr(
                            pCrsr->GetPoint()->nContent, RES_TXTATR_TOXMARK);
                if(pTxtAttr)
                    m_pTOXMark = &pTxtAttr->GetTOXMark();
            }
        }
    }
    else if(bIsDescriptor)
    {
        switch(pMap->nWID)
        {
            case WID_ALT_TEXT:
                sAltText = lcl_AnyToString(aValue);
            break;
            case WID_LEVEL:
            {
                sal_uInt16 nVal = lcl_AnyToInt16(aValue);
                if(nVal >= 0 && nVal < MAXLEVEL)
                    nLevel = nVal;
                else
                    throw IllegalArgumentException();
            }
            break;
            case WID_PRIMARY_KEY  :
                sPrimaryKey = lcl_AnyToString(aValue);
            break;
            case WID_SECONDARY_KEY:
                sSecondaryKey = lcl_AnyToString(aValue);
            break;
            case WID_USER_IDX_NAME :
                sUserIndexName = lcl_AnyToString(aValue);
            break;
        }
    }
    else
        throw RuntimeException();
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXDocumentIndexMark::getPropertyValue(const OUString& rPropertyName)
    throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    SwTOXType* pType = ((SwXDocumentIndexMark*)this)->GetTOXType();
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                                        _pMap, rPropertyName);

    if(!pMap)
        throw UnknownPropertyException();
     if(SwXParagraph::getDefaultTextContentValue(aRet, rPropertyName, pMap->nWID))
        return aRet;
    if(pType)
    {
        SwTOXMark* pCurMark = lcl_GetMark(pType, GetTOXMark());
        if(pCurMark)
        {
            switch(pMap->nWID)
            {
                case WID_ALT_TEXT:
                    aRet <<= OUString(pCurMark->GetAlternativeText());
                break;
                case WID_LEVEL:
                    aRet <<= (sal_Int16)pCurMark->GetLevel();
                break;
                case WID_PRIMARY_KEY  :
                    aRet <<= OUString(pCurMark->GetPrimaryKey());
                break;
                case WID_SECONDARY_KEY:
                    aRet <<= OUString(pCurMark->GetSecondaryKey());
                break;
                case WID_USER_IDX_NAME :
                    aRet <<= OUString(pType->GetTypeName());
                break;
            }
        }
    }
    else if(bIsDescriptor)
    {
        switch(pMap->nWID)
        {
            case WID_ALT_TEXT:
                aRet <<= OUString(sAltText);
            break;
            case WID_LEVEL:
                aRet <<= (sal_Int16)nLevel;
            break;
            case WID_PRIMARY_KEY  :
                aRet <<= OUString(sPrimaryKey);
            break;
            case WID_SECONDARY_KEY:
                aRet <<= OUString(sSecondaryKey);
            break;
            case WID_USER_IDX_NAME :
                aRet <<= OUString(sUserIndexName);
            break;
        }
    }
    else
        throw RuntimeException();
    return aRet;
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::addPropertyChangeListener(const OUString& PropertyName, const Reference< XPropertyChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 10:25:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::removePropertyChangeListener(const OUString& PropertyName, const Reference< XPropertyChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 10:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::addVetoableChangeListener(const OUString& PropertyName, const Reference< XVetoableChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 10:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::removeVetoableChangeListener(const OUString& PropertyName, const Reference< XVetoableChangeListener > & aListener) throw( UnknownPropertyException, WrappedTargetException, RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 10:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexMark*	SwXDocumentIndexMark::GetObject(SwTOXType* pType,
                                    const SwTOXMark* pMark,	SwDoc* pDoc)
{
    SwClientIter aIter(*pType);
    SwXDocumentIndexMark* pxMark = (SwXDocumentIndexMark*)
                                            aIter.First(TYPE(SwXDocumentIndexMark));
    while( pxMark )
    {
        if(pxMark->GetTOXMark() == pMark)
            return pxMark;
        pxMark = (SwXDocumentIndexMark*)aIter.Next();
    }
    return new SwXDocumentIndexMark(pType, pMark, pDoc);
}
/*-- 14.12.98 10:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXDocumentIndexMark::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        aLstnrCntnr.Disposing();
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXDocumentIndexes::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXDocumentIndexes");
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXDocumentIndexes::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.DocumentIndexes") == rServiceName;
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXDocumentIndexes::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.DocumentIndexes");
    return aRet;
}
/*-- 05.05.99 13:14:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexes::SwXDocumentIndexes(SwDoc* pDoc) :
    SwUnoCollection(pDoc)
{
}
/*-- 05.05.99 13:15:00---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXDocumentIndexes::~SwXDocumentIndexes()
{
}
/*-- 05.05.99 13:15:01---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXDocumentIndexes::getCount(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();

    sal_uInt32 nRet = 0;
    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    for( sal_uInt16 n = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() )
            ++nRet;
    }
    return nRet;
}
/*-- 05.05.99 13:15:01---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXDocumentIndexes::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();

    uno::Any aRet;
    sal_uInt32 nIdx = 0;

    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    for( sal_uInt16 n = 0, nCnt = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() &&
            nIdx++ == nIndex )
            {
               Reference< text::XDocumentIndex >  xTmp = new SwXDocumentIndex(
                                    (SwTOXBaseSection*)pSect, GetDoc() );
               aRet.setValue(&xTmp, ::getCppuType((Reference<text::XDocumentIndex>*)0));
               return aRet;
            }
    }

    throw IndexOutOfBoundsException();
    return aRet;
}

/*-- 31.01.00 10:12:31---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXDocumentIndexes::getByName(const OUString& rName)
    throw( container::NoSuchElementException, WrappedTargetException, RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();

    uno::Any aRet;
    sal_uInt32 nIdx = 0;

    String sToFind(rName);
    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    for( sal_uInt16 n = 0, nCnt = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() &&
                ((SwTOXBaseSection*)pSect)->GetTOXName() == sToFind)
            {
               Reference< text::XDocumentIndex >  xTmp = new SwXDocumentIndex(
                                    (SwTOXBaseSection*)pSect, GetDoc() );
               aRet.setValue(&xTmp, ::getCppuType((Reference<text::XDocumentIndex>*)0));
               return aRet;
            }
    }
    throw container::NoSuchElementException();
    return aRet;
}
/*-- 31.01.00 10:12:31---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXDocumentIndexes::getElementNames(void)
    throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();

    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    sal_Int32 nCount = 0;
    sal_uInt16 n;
    for( n = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() )
            ++nCount;
    }

    uno::Sequence< OUString > aRet(nCount);
    OUString* pArray = aRet.getArray();
    sal_uInt16 nCnt;
    for( n = 0, nCnt = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode())
        {
            pArray[nCnt++] = OUString(((SwTOXBaseSection*)pSect)->GetTOXName());
        }
    }
    return aRet;
}
/*-- 31.01.00 10:12:31---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXDocumentIndexes::hasByName(const OUString& rName)
    throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();

    String sToFind(rName);
    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    for( sal_uInt16 n = 0, nCnt = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode())
        {
            if(((SwTOXBaseSection*)pSect)->GetTOXName() == sToFind)
                return sal_True;
        }
    }
    return sal_False;
}
/*-- 05.05.99 13:15:01---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXDocumentIndexes::getElementType(void) throw( RuntimeException )
{
    return ::getCppuType((Reference< text::XDocumentIndex> *)0);
}
/*-- 05.05.99 13:15:02---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXDocumentIndexes::hasElements(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw RuntimeException();
    return 0 != getCount();
}

/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXIndexStyleAccess_Impl::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXIndexStyleAccess_Impl");
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXIndexStyleAccess_Impl::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.DocumentIndexParagraphStyles") == rServiceName;
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXIndexStyleAccess_Impl::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.DocumentIndexParagraphStyles");
    return aRet;
}
/*-- 13.09.99 16:52:28---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXIndexStyleAccess_Impl::SwXIndexStyleAccess_Impl(SwXDocumentIndex& rParentIdx) :
    rParent(rParentIdx),
    xParent(&rParentIdx)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    rParent.SetStyleAccess(this);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXIndexStyleAccess_Impl::~SwXIndexStyleAccess_Impl()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    rParent.SetStyleAccess(0);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXIndexStyleAccess_Impl::replaceByIndex(sal_Int32 nIndex, const uno::Any& rElement)
    throw( IllegalArgumentException, IndexOutOfBoundsException,
          WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const sal_Bool bDescriptor = rParent.IsDescriptor();
    SwSectionFmt* pSectFmt = rParent.GetFmt();
    if(!pSectFmt && !bDescriptor)
        throw RuntimeException();
    if(nIndex < 0 || nIndex > MAXLEVEL)
        throw IndexOutOfBoundsException();
    SwTOXBase* pTOXBase = bDescriptor ? &rParent.GetProperties_Impl()->GetTOXBase() :
            (SwTOXBaseSection*)pSectFmt->GetSection();
    if(rElement.getValueType() != ::getCppuType((uno::Sequence<OUString>*)0))
        throw IllegalArgumentException();
    const uno::Sequence<OUString>* pSeq = (const uno::Sequence<OUString>*)rElement.getValue();

    sal_uInt16 nStyles = pSeq->getLength();
    const OUString* pStyles = pSeq->getConstArray();
    String sSetStyles;
    for(sal_uInt16 i = 0; i < nStyles; i++)
    {
        if(i)
            sSetStyles += TOX_STYLE_DELIMITER;
        sSetStyles += String(pStyles[i]);
    }
    pTOXBase->SetStyleNames(sSetStyles, (sal_uInt16) nIndex);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXIndexStyleAccess_Impl::getCount(void) throw( RuntimeException )
{
    return MAXLEVEL;
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXIndexStyleAccess_Impl::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException,
                 RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const sal_Bool bDescriptor = rParent.IsDescriptor();
    SwSectionFmt* pSectFmt = rParent.GetFmt();
    if(!pSectFmt && !bDescriptor)
        throw RuntimeException();
    if(nIndex < 0 || nIndex > MAXLEVEL)
        throw IndexOutOfBoundsException();
    SwTOXBase* pTOXBase = bDescriptor ? &rParent.GetProperties_Impl()->GetTOXBase() :
                (SwTOXBaseSection*)pSectFmt->GetSection();

    const String& rStyles = pTOXBase->GetStyleNames((sal_uInt16) nIndex);
    sal_uInt16 nStyles = rStyles.GetTokenCount(TOX_STYLE_DELIMITER);
    uno::Sequence<OUString> aStyles(nStyles);
    OUString* pStyles = aStyles.getArray();
    for(sal_uInt16 i = 0; i < nStyles; i++)
    {
        pStyles[i] = OUString(rStyles.GetToken(i, TOX_STYLE_DELIMITER));
    }
    uno::Any aRet(&aStyles, ::getCppuType((uno::Sequence<OUString>*)0));
    return aRet;
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXIndexStyleAccess_Impl::getElementType(void)
    throw( RuntimeException )
{
    return ::getCppuType((uno::Sequence<OUString>*)0);
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXIndexStyleAccess_Impl::hasElements(void) throw( RuntimeException )
{
    return sal_True;
}

/* -----------------13.09.99 16:51-------------------

 --------------------------------------------------*/
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXIndexTokenAccess_Impl::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXIndexTokenAccess_Impl");
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXIndexTokenAccess_Impl::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.DocumentIndexLevelFormat") == rServiceName;
}
/* -----------------------------06.04.00 15:08--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXIndexTokenAccess_Impl::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.DocumentIndexLevelFormat");
    return aRet;
}
/*-- 13.09.99 16:52:28---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXIndexTokenAccess_Impl::SwXIndexTokenAccess_Impl(SwXDocumentIndex& rParentIdx) :
    rParent(rParentIdx),
    xParent(&rParentIdx),
    nCount(SwForm::GetFormMaxLevel(rParent.GetTOXType()))
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    rParent.SetTokenAccess(this);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXIndexTokenAccess_Impl::~SwXIndexTokenAccess_Impl()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    rParent.SetTokenAccess(0);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXIndexTokenAccess_Impl::replaceByIndex(sal_Int32 nIndex, const uno::Any& rElement)
    throw( IllegalArgumentException, IndexOutOfBoundsException,
            WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const sal_Bool bDescriptor = rParent.IsDescriptor();
    SwSectionFmt* pSectFmt = rParent.GetFmt();
    if(!pSectFmt && !bDescriptor)
        throw RuntimeException();

    SwTOXBase* pTOXBase = bDescriptor ? &rParent.GetProperties_Impl()->GetTOXBase() :
                            (SwTOXBaseSection*)pSectFmt->GetSection();
    if(nIndex < 0 ||
        (nIndex > pTOXBase->GetTOXForm().GetFormMax()))
            throw IndexOutOfBoundsException();
    if(rElement.getValueType() != ::getCppuType((uno::Sequence<PropertyValues>*)0))
        throw IllegalArgumentException();
    const uno::Sequence<PropertyValues>* pSeq = (uno::Sequence<PropertyValues>* )rElement.getValue();

    String sPattern;
    sal_uInt16 nTokens = pSeq->getLength();
    const PropertyValues* pTokens = pSeq->getConstArray();
    for(sal_uInt16 i = 0; i < nTokens; i++)
    {
        const PropertyValue* pProperties = pTokens[i].getConstArray();
        sal_uInt16 nProperties = pTokens[i].getLength();
        //create an invalid token
        SwFormToken aToken(TOKEN_END);
        for(sal_uInt16 j = 0; j < nProperties; j++)
        {
            if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("TokenType"))
            {
                const String sTokenType =
                        lcl_AnyToString(pProperties[j].Value);
                if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenEntryNumber"))
                    aToken.eTokenType = TOKEN_ENTRY_NO;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenEntryText" ))
                    aToken.eTokenType = TOKEN_ENTRY_TEXT;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenTabStop"   ))
                    aToken.eTokenType = TOKEN_TAB_STOP;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenText"      ))
                    aToken.eTokenType = TOKEN_TEXT;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenPageNumber"))
                    aToken.eTokenType = TOKEN_PAGE_NUMS;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenChapterInfo"      ))
                    aToken.eTokenType = TOKEN_CHAPTER_INFO;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenHyperlinkStart" ))
                    aToken.eTokenType = TOKEN_LINK_START;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenHyperlinkEnd"))
                    aToken.eTokenType = TOKEN_LINK_END;
                else if(COMPARE_EQUAL == sTokenType.CompareToAscii("TokenBibliographyDataField" ))
                    aToken.eTokenType = TOKEN_AUTHORITY;
            }
            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("CharacterStyleName"  )  )
            {
                const String sCharStyleName =
                    lcl_AnyToString(pProperties[j].Value);
                aToken.sCharStyleName = sCharStyleName;
                aToken.nPoolId = pSectFmt->GetDoc()->
                            GetPoolId( sCharStyleName, GET_POOLID_CHRFMT );
            }
            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("TabStopRightAligned") )
            {
                sal_Bool bRight = lcl_AnyToBool(pProperties[j].Value);
                aToken.eTabAlign = bRight ?
                                    SVX_TAB_ADJUST_END : SVX_TAB_ADJUST_LEFT;
            }
            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("TabStopPosition"  ))
            {
                sal_Int32 nPosition;
                if(pProperties[j].Value.getValueType() != ::getCppuType((sal_Int32*)0))
                    throw IllegalArgumentException();
                pProperties[j].Value >>= nPosition;
                nPosition = MM100_TO_TWIP(nPosition);
                if(nPosition < 0)
                    throw IllegalArgumentException();
                aToken.nTabStopPosition = nPosition;
            }
            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("TabStopFillCharacter" ))
            {
                const String sFillChar =
                    lcl_AnyToString(pProperties[j].Value);
                if(sFillChar.Len() > 1)
                    throw IllegalArgumentException();
                aToken.cTabFillChar = sFillChar.Len() ?
                                sFillChar.GetChar(0) : ' ';
            }

            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("Text" ))
               {
                const String sText =
                    lcl_AnyToString(pProperties[j].Value);
                aToken.sText = sText;
            }
            else if( COMPARE_EQUAL == pProperties[j].Name.compareToAscii("ChapterFormat"    ))
            {
                sal_Int16 nFormat = lcl_AnyToInt16(pProperties[j].Value);
                switch(nFormat)
                {
                    case text::ChapterFormat::NUMBER:		 	nFormat = CF_NUMBER;
                    break;
                    case text::ChapterFormat::NAME:  			nFormat = CF_TITLE;
                    break;
                    case text::ChapterFormat::NAME_NUMBER:		nFormat = CF_NUM_TITLE;
                    break;
                    case text::ChapterFormat::NO_PREFIX_SUFFIX:nFormat = CF_NUMBER_NOPREPST;
                    break;
                    case text::ChapterFormat::DIGIT:           nFormat = CF_NUM_NOPREPST_TITLE;
                    break;
                    default:
                        throw IllegalArgumentException();
                }
                aToken.nChapterFormat = nFormat;
            }
        }
        //exception if wrong TokenType
        if(TOKEN_END <= aToken.eTokenType )
            throw IllegalArgumentException();
        // set TokenType from TOKEN_ENTRY_TEXT to TOKEN_ENTRY if it is
        // not a content index
        if(TOKEN_ENTRY_TEXT == aToken.eTokenType &&
            ((TOX_CONTENT != pTOXBase->GetType())) ||
            TOKEN_ENTRY == aToken.eTokenType)
            sPattern += aToken.GetString();
    }
    SwForm aForm(pTOXBase->GetTOXForm());
    aForm.SetPattern((sal_uInt16) nIndex, sPattern);
    pTOXBase->SetTOXForm(aForm);
}
/*-- 13.09.99 16:52:29---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXIndexTokenAccess_Impl::getCount(void) throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const sal_Bool bDescriptor = rParent.IsDescriptor();
    SwSectionFmt* pSectFmt = rParent.GetFmt();
    if(!pSectFmt && !bDescriptor)
        throw RuntimeException();
    sal_Int32 nRet = bDescriptor ?
        nCount :
        ((SwTOXBaseSection*)pSectFmt->GetSection())->
                                            GetTOXForm().GetFormMax();
    return nRet;
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXIndexTokenAccess_Impl::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException,
         RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const sal_Bool bDescriptor = rParent.IsDescriptor();
    SwSectionFmt* pSectFmt = rParent.GetFmt();
    if(!pSectFmt && !bDescriptor)
        throw RuntimeException();
    SwTOXBase* pTOXBase = bDescriptor ? &rParent.GetProperties_Impl()->GetTOXBase() :
            (SwTOXBaseSection*)pSectFmt->GetSection();
    if(nIndex < 0 ||
    (nIndex > pTOXBase->GetTOXForm().GetFormMax()))
        throw IndexOutOfBoundsException();

    SwFormTokenEnumerator aEnumerator(pTOXBase->GetTOXForm().
                                        GetPattern((sal_uInt16) nIndex));
    sal_uInt16 nTokenCount = 0;
    uno::Sequence< PropertyValues > aRetSeq;
    while(aEnumerator.HasNextToken())
    {
        nTokenCount++;
        aRetSeq.realloc(nTokenCount);
        PropertyValues* pTokenProps = aRetSeq.getArray();
        SwFormToken  aToken = aEnumerator.GetNextToken();
        switch(aToken.eTokenType)
        {
            case TOKEN_ENTRY_NO     :
            {
                pTokenProps->realloc( 2 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenEntryNumber");
//				pArr[0].Value <<= C2U("TokenEntryNumber");

                pArr[1].Name = C2U("CharacterStyleName");
                pArr[1].Value <<= OUString(aToken.sCharStyleName);
            }
            break;
            case TOKEN_ENTRY        :	// no difference between Entry and Entry Text
            case TOKEN_ENTRY_TEXT   :
            {
                pTokenProps->realloc( 2 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenEntryText");

                pArr[1].Name = C2U("CharacterStyleName");
                pArr[1].Value <<= OUString(aToken.sCharStyleName);
            }
            break;
            case TOKEN_TAB_STOP     :
            {
                pTokenProps->realloc(3);
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenTabStop");


                if(SVX_TAB_ADJUST_END == aToken.eTabAlign)
                {
                    pArr[1].Name = C2U("TabStopRightAligned");
                    BOOL bTemp = sal_True;
                    pArr[1].Value.setValue(&bTemp, ::getCppuBooleanType());
                }
                else
                {
                    pArr[1].Name = C2U("TabStopPosition");
                    pArr[1].Value <<= (sal_Int32)(TWIP_TO_MM100(aToken.nTabStopPosition));
                }
                pArr[2].Name = C2U("TabStopFillCharacter");
                pArr[2].Value <<= OUString(aToken.cTabFillChar);
            }
            break;
            case TOKEN_TEXT         :
            {
                pTokenProps->realloc( 3 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenText");

                pArr[1].Name = C2U("CharacterStyleName");
                pArr[1].Value <<= OUString(aToken.sCharStyleName);

                pArr[2].Name = C2U("Text");
                pArr[2].Value <<= OUString(aToken.sText);
            }
            break;
            case TOKEN_PAGE_NUMS    :
            {
                pTokenProps->realloc( 2 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenPageNumber");

                pArr[1].Name = C2U("CharacterStyleName");
                pArr[1].Value <<= OUString(aToken.sCharStyleName);
            }
            break;
            case TOKEN_CHAPTER_INFO :
            {
                pTokenProps->realloc( 3 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenChapterInfo");

                pArr[1].Name = C2U("CharacterStyleName");
                pArr[1].Value <<= OUString(aToken.sCharStyleName);

                pArr[2].Name = C2U("ChapterFormat");
                sal_Int16 nVal = text::ChapterFormat::NUMBER;
                switch(aToken.nChapterFormat)
                {
                    case CF_NUMBER:				nVal = text::ChapterFormat::NUMBER; break;
                    case CF_TITLE:				nVal = text::ChapterFormat::NAME; break;
                    case CF_NUM_TITLE:			nVal = text::ChapterFormat::NAME_NUMBER; break;
                    case CF_NUMBER_NOPREPST:	nVal = text::ChapterFormat::NO_PREFIX_SUFFIX; break;
                    case CF_NUM_NOPREPST_TITLE: nVal = text::ChapterFormat::DIGIT; break;
                }
                pArr[2].Value <<= (sal_Int16)nVal;
            }
            break;
            case TOKEN_LINK_START   :
            {
                pTokenProps->realloc( 1 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenHyperlinkStart");
            }
            break;
            case TOKEN_LINK_END     :
            {
                pTokenProps->realloc( 1 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenHyperlinkEnd");
            }
            break;
            case TOKEN_AUTHORITY :
                pTokenProps->realloc( 1 );
                PropertyValue* pArr = pTokenProps->getArray();

                pArr[0].Name = C2U("TokenType");
                pArr[0].Value <<= OUString::createFromAscii("TokenBibliographyDataField");
                DBG_ERROR("bibliography not implemented")
            break;
        }
    }

    uno::Any aRet(&aRetSeq, ::getCppuType((uno::Sequence< PropertyValues >*)0));

    return aRet;
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type  SwXIndexTokenAccess_Impl::getElementType(void)
    throw( RuntimeException )
{
    return ::getCppuType((uno::Sequence< PropertyValues >*)0);
}
/*-- 13.09.99 16:52:30---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXIndexTokenAccess_Impl::hasElements(void) throw( RuntimeException )
{
    return sal_True;
}
/*------------------------------------------------------------------------

    $Log: not supported by cvs2svn $
    Revision 1.66  2000/09/18 16:04:32  willem.vandorp
    OpenOffice header added.
    
    Revision 1.65  2000/09/12 11:42:58  os
    #78682# support of service TextContent
    
    Revision 1.64  2000/09/07 07:59:10  os
    #78443# SetMarkEntry now works as desriptor too
    
    Revision 1.63  2000/08/17 10:08:04  os
    type of TabStopPosition corrected
    
    Revision 1.62  2000/08/09 16:05:26  os
    several new index services

    Revision 1.61  2000/06/29 14:59:05  os
    SwXDocumentIndex holds a SwTOXBase in its descriptor

    Revision 1.60  2000/05/16 09:14:54  os
    project usr removed

    Revision 1.59  2000/04/26 11:35:19  os
    GetName() returns String&

    Revision 1.58  2000/04/11 08:31:03  os
    UNICODE

    Revision 1.57  2000/03/31 10:26:36  os
    #74034# getServiceName: comply to documentation

    Revision 1.56  2000/03/27 10:21:10  os
    UNO III

    Revision 1.55  2000/03/21 15:42:24  os
    UNOIII

    Revision 1.54  2000/02/28 15:36:40  hjs
    #65293# include

    Revision 1.53  2000/02/25 14:08:38  os
    #72990# test script changes

    Revision 1.52  2000/02/23 16:13:05  os
    #72990# get/setPropertyValue and get/setName work as Descriptor, too

    Revision 1.51  2000/02/14 13:20:14  os
    #72990# service info added

    Revision 1.50  2000/02/11 14:35:28  hr
    #70473# changes for unicode ( patched by automated patchtool )

    Revision 1.49  2000/01/31 09:56:01  os
    #70889# SwXDocumentIndexes inherits container::XNameAccess

    Revision 1.48  2000/01/19 09:23:19  os
    #71955# setPropertyValues: template names corrected

    Revision 1.47  2000/01/17 15:13:51  os
    #71955# tab stop position as sal_Int32

    Revision 1.46  1999/12/20 11:07:04  os
    #70234# set pattern at the correct level

    Revision 1.45  1999/12/09 14:53:02  os
    #70234# index corrected

    Revision 1.44  1999/12/03 11:10:42  os
    #70234# wrong assignments corrected

    Revision 1.43  1999/12/01 14:44:23  os
    #70234# properties added

    Revision 1.42  1999/11/29 15:46:16  os
    #70234# some index errors corrected

    Revision 1.41  1999/11/25 15:43:26  os
    headers corrected

    Revision 1.40  1999/11/19 16:40:18  os
    modules renamed

    Revision 1.39  1999/11/10 14:48:20  os
    some index properties corrected

    Revision 1.38  1999/09/20 12:49:52  hr
    #65293#: .is()

    Revision 1.37  1999/09/14 13:29:05  os
    SwXDocumentIndex completed

    Revision 1.36  1999/09/01 14:18:38  OS
    new index properties


      Rev 1.35   01 Sep 1999 16:18:38   OS
   new index properties

      Rev 1.34   17 Aug 1999 13:57:54   OS
   extended indexes: get/set section attributes

      Rev 1.33   30 Jul 1999 14:41:40   OS
   container::XNamed in SwXDocumentIndex

      Rev 1.32   27 Jul 1999 20:23:42   JP
   replace class SwTOXBaseRange with SwTOXBaseSection - TOX use now SwSections

      Rev 1.31   05 May 1999 14:07:22   OS
   #64655# DocumentIndexes wiederbelebt

      Rev 1.30   22 Apr 1999 16:13:42   OS
   #65194# throw -> throw; #65124# not impl. nur noch warning; EventListener

      Rev 1.29   21 Apr 1999 14:35:52   OS
   #65098# Verzeichnisse und -eintraege

      Rev 1.28   15 Mar 1999 14:36:40   OS
   #62845# Makro fuer ServiceInfo jetzt auch fuer OS/2

      Rev 1.27   12 Mar 1999 09:41:12   OS
   #62845# XServiceInfo impl.

      Rev 1.26   09 Mar 1999 12:41:06   OS
   #62008# Solar-Mutex

      Rev 1.25   04 Mar 1999 15:03:04   OS
   #62191# UINT nicht mehr verwenden

      Rev 1.24   18 Feb 1999 14:25:48   OS
   #52654# insertTextContent statt attach

      Rev 1.23   28 Jan 1999 16:45:06   OS
   #56371# keine Objekte fuer DEBUG anlegen

      Rev 1.22   22 Jan 1999 15:09:10   OS
   #56371# Draw wieder verfuegbar

      Rev 1.21   19 Jan 1999 08:04:16   OS
    #56371# TF_ONE51: Descriptor-interfaces

      Rev 1.20   14 Jan 1999 16:21:50   OS
   #56371# TF_ONE51

      Rev 1.19   15 Dec 1998 10:10:02   OS
   #56371# TF_ONE51 Zwischenstand

      Rev 1.18   10 Dec 1998 15:53:14   OS
   #56371# TF_ONE51 Zwischenstand

      Rev 1.17   17 Sep 1998 09:24:54   OS
   #52654# IndexMark einfuegen auch ohne Markierung, Properties gerichtet

      Rev 1.16   10 Jul 1998 18:08:30   OS
   PropertySetInfo und IdlClass static

      Rev 1.15   03 Jul 1998 16:18:00   OS
   Verzeichnisse nicht verschachteln

      Rev 1.14   26 Jun 1998 18:16:46   OS
   Maps aus unomap

      Rev 1.13   25 Jun 1998 11:15:54   OS
   IndexMark::setPropertyValue : auch mit Alternativtext richtig umsetzen

      Rev 1.12   23 Jun 1998 10:24:48   OS
   Verzeichnisse benutzbar, mit Descriptor

      Rev 1.11   19 Jun 1998 14:46:20   TJ
   GetModel auskommentiert

      Rev 1.10   17 Jun 1998 16:10:32   MH
   chg: Syntax OS2

      Rev 1.9   12 Jun 1998 13:49:14   OS
   Package-Umstellung

      Rev 1.8   04 Jun 1998 12:40:56   OS
// automatisch auskommentiert - [getIdlClass(es) or queryInterface] - Bitte XTypeProvider benutzen!
//   getIdlClasses


      Rev 1.7   14 May 1998 17:49:40   OS
   div. Namensaenderungen

      Rev 1.6   13 May 1998 15:29:08   OS
   TextPosition-Auswertung verbessert

      Rev 1.5   06 May 1998 17:06:22   OS
   XTextPosition ohne Doc anlegen

      Rev 1.4   09 Apr 1998 15:10:52   OS
   Uno-Umstellung

      Rev 1.3   07 Apr 1998 14:09:42   OS
   Properties der Vezeichnisse lesen

      Rev 1.2   01 Apr 1998 07:28:48   OS
   Syntaxfehler

      Rev 1.1   31 Mar 1998 15:35:18   OS
   Properties angefangen

      Rev 1.0   30 Mar 1998 10:19:08   OS
   Initial revision.

------------------------------------------------------------------------*/
