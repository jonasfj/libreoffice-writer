/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unostyle.cxx,v $
 *
 *  $Revision: 1.77 $
 *
 *  last change: $Author: kz $ $Date: 2007-12-12 13:26:08 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _SFXSMPLHINT_HXX //autogen
#include <svtools/smplhint.hxx>
#endif
#ifndef _CTRLTOOL_HXX //autogen
#include <svtools/ctrltool.hxx>
#endif
#ifndef _SFXSTYLE_HXX
#include <svtools/style.hxx>
#endif
#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#ifndef _SVX_PAGEITEM_HXX //autogen
#include <svx/pageitem.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX //autogen
#include <svx/sizeitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_FLSTITEM_HXX //autogen
#include <svx/flstitem.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _PAGEDESC_HXX //autogen
#include <pagedesc.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#include <cmdid.h>
#ifndef _UNOSTYLE_HXX
#include <unostyle.hxx>
#endif
#ifndef _UNOSETT_HXX
#include <unosett.hxx>
#endif
#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _SWSTYLE_H
#include <swstyle.h>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif
#ifndef _SHELLIO_HXX //autogen
#include <shellio.hxx>
#endif
#ifndef _DOCSTYLE_HXX //autogen
#include <docstyle.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen wg. SwFmtPageDesc
#include <fmtpdsc.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _UNOEVENT_HXX
#include "unoevent.hxx"
#endif
#ifndef _FMTRUBY_HXX
#include <fmtruby.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif
#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHSTYLECATEGORY_HPP_
#include <com/sun/star/style/ParagraphStyleCategory.hpp>
#endif
/*
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
*/
#ifndef _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif
#include <istyleaccess.hxx>

#ifndef _GETMETRICVAL_HXX
#include <GetMetricVal.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif

#include <boost/shared_ptr.hpp>

#include "ccoll.hxx"

#define STYLE_FAMILY_COUNT 5            // we have 5 style families
#define TYPE_BOOL       0
#define TYPE_SIZE       1
#define TYPE_BRUSH      2
#define TYPE_ULSPACE    3
#define TYPE_SHADOW     4
#define TYPE_LRSPACE    5
#define TYPE_BOX        6

const unsigned short aStyleByIndex[] =
{
    SFX_STYLE_FAMILY_CHAR,
    SFX_STYLE_FAMILY_PARA,
    SFX_STYLE_FAMILY_PAGE     ,
    SFX_STYLE_FAMILY_FRAME    ,
    SFX_STYLE_FAMILY_PSEUDO
};

// Already implemented autostyle families: 3
#define AUTOSTYLE_FAMILY_COUNT 3
const IStyleAccess::SwAutoStyleFamily aAutoStyleByIndex[] =
{
    IStyleAccess::AUTO_STYLE_CHAR,
    IStyleAccess::AUTO_STYLE_RUBY,
    IStyleAccess::AUTO_STYLE_PARA
};

using namespace ::com::sun::star;
using namespace ::rtl;

/******************************************************************************
 *
 ******************************************************************************/

//convert FN_... to RES_ in header and footer itemset
sal_uInt16 lcl_ConvertFNToRES(sal_uInt16 nFNId)
{
    sal_uInt16 nRes = USHRT_MAX;
    switch(nFNId)
    {
        case FN_UNO_FOOTER_ON:
        case FN_UNO_HEADER_ON:
        break;
        case FN_UNO_FOOTER_BACKGROUND:
        case FN_UNO_HEADER_BACKGROUND:      nRes = RES_BACKGROUND;
        break;
        case FN_UNO_FOOTER_BOX:
        case FN_UNO_HEADER_BOX:             nRes = RES_BOX;
        break;
        case FN_UNO_FOOTER_LR_SPACE:
        case FN_UNO_HEADER_LR_SPACE:        nRes = RES_LR_SPACE;
        break;
        case FN_UNO_FOOTER_SHADOW:
        case FN_UNO_HEADER_SHADOW:          nRes = RES_SHADOW;
        break;
        case FN_UNO_FOOTER_BODY_DISTANCE:
        case FN_UNO_HEADER_BODY_DISTANCE:   nRes = RES_UL_SPACE;
        break;
        case FN_UNO_FOOTER_IS_DYNAMIC_DISTANCE:
        case FN_UNO_HEADER_IS_DYNAMIC_DISTANCE: nRes = SID_ATTR_PAGE_DYNAMIC;
        break;
        case FN_UNO_FOOTER_SHARE_CONTENT:
        case FN_UNO_HEADER_SHARE_CONTENT:   nRes = SID_ATTR_PAGE_SHARED;
        break;
        case FN_UNO_FOOTER_HEIGHT:
        case FN_UNO_HEADER_HEIGHT:          nRes = SID_ATTR_PAGE_SIZE;
        break;
        case FN_UNO_FOOTER_EAT_SPACING:
        case FN_UNO_HEADER_EAT_SPACING:   nRes = RES_HEADER_FOOTER_EAT_SPACING;
        break;
    }
    return nRes;

}

SwGetPoolIdFromName lcl_GetSwEnumFromSfxEnum ( SfxStyleFamily eFamily )
{
    switch ( eFamily )
    {
        case SFX_STYLE_FAMILY_CHAR:
            return nsSwGetPoolIdFromName::GET_POOLID_CHRFMT;
        case SFX_STYLE_FAMILY_PARA:
            return nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL;
        case SFX_STYLE_FAMILY_FRAME:
            return nsSwGetPoolIdFromName::GET_POOLID_FRMFMT;
        case SFX_STYLE_FAMILY_PAGE:
            return nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC;
        case SFX_STYLE_FAMILY_PSEUDO:
            return nsSwGetPoolIdFromName::GET_POOLID_NUMRULE;
        default:
            DBG_ASSERT(sal_False, "someone asking for all styles in unostyle.cxx!" );
            return nsSwGetPoolIdFromName::GET_POOLID_CHRFMT;
    }
}

class SwAutoStylesEnumImpl
{
    std::vector<SfxItemSet_Pointer_t> mAutoStyles;
    std::vector<SfxItemSet_Pointer_t>::iterator aIter;
    SwDoc* pDoc;
    IStyleAccess::SwAutoStyleFamily eFamily;
public:
    SwAutoStylesEnumImpl( SwDoc* pInitDoc, IStyleAccess::SwAutoStyleFamily eFam );
    ::sal_Bool hasMoreElements() { return aIter != mAutoStyles.end(); }
    SfxItemSet_Pointer_t nextElement() { return *(aIter++); }
    IStyleAccess::SwAutoStyleFamily getFamily() const { return eFamily; }
    SwDoc* getDoc() const { return pDoc; }
};


/******************************************************************
 * SwXStyleFamilies
 ******************************************************************/
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXStyleFamilies::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXStyleFamilies");
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXStyleFamilies::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return C2U("com.sun.star.style.StyleFamilies") == rServiceName;
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXStyleFamilies::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.style.StyleFamilies");
    return aRet;
}
/*-- 16.12.98 15:13:26---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyleFamilies::SwXStyleFamilies(SwDocShell& rDocShell) :
    SwUnoCollection(rDocShell.GetDoc()),
    pDocShell(&rDocShell),
    pxCharStyles(0),
    pxParaStyles(0),
    pxFrameStyles(0),
    pxPageStyles(0),
    pxNumberingStyles(0)
{

}
/*-- 16.12.98 15:13:26---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyleFamilies::~SwXStyleFamilies()
{
    delete pxCharStyles;
    delete pxParaStyles;
    delete pxFrameStyles;
    delete pxPageStyles;
    delete pxNumberingStyles;
}
/*-- 21.12.98 12:05:22---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL SwXStyleFamilies::getByName(const OUString& Name)
    throw(
        container::NoSuchElementException,
        lang::WrappedTargetException,
        uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
// der Index kommt aus const unsigned short aStyleByIndex[] =
    uno::Any aRet;
    if(!IsValid())
        throw uno::RuntimeException();
    if(Name.compareToAscii("CharacterStyles") == 0 )
        aRet = getByIndex(0);
    else if(Name.compareToAscii("ParagraphStyles") == 0)
        aRet = getByIndex(1);
    else if(Name.compareToAscii("FrameStyles") == 0 )
        aRet = getByIndex(3);
    else if(Name.compareToAscii("PageStyles") == 0 )
        aRet = getByIndex(2);
    else if(Name.compareToAscii("NumberingStyles") == 0 )
        aRet = getByIndex(4);
    else
        throw container::NoSuchElementException();
    return aRet;
}
/*-- 21.12.98 12:05:22---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXStyleFamilies::getElementNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aNames(STYLE_FAMILY_COUNT);
    OUString* pNames = aNames.getArray();
    pNames[0] = C2U("CharacterStyles");
    pNames[1] = C2U("ParagraphStyles");
    pNames[2] = C2U("FrameStyles");
    pNames[3] = C2U("PageStyles");
    pNames[4] = C2U("NumberingStyles");
    return aNames;
}
/*-- 21.12.98 12:05:22---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyleFamilies::hasByName(const OUString& Name) throw( uno::RuntimeException )
{
    if( Name.compareToAscii("CharacterStyles") == 0 ||
        Name.compareToAscii("ParagraphStyles") == 0 ||
        Name.compareToAscii("FrameStyles") == 0 ||
        Name.compareToAscii("PageStyles") == 0 ||
        Name.compareToAscii("NumberingStyles") == 0 )
        return sal_True;
    else
        return sal_False;
}
/*-- 16.12.98 15:13:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXStyleFamilies::getCount(void) throw( uno::RuntimeException )
{
    return STYLE_FAMILY_COUNT;
}
/*-- 16.12.98 15:13:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXStyleFamilies::getByIndex(sal_Int32 nIndex)
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(nIndex < 0 || nIndex >= STYLE_FAMILY_COUNT)
        throw lang::IndexOutOfBoundsException();
    if(IsValid())
    {
        uno::Reference< container::XNameContainer >  aRef;
        sal_uInt16 nType = aStyleByIndex[nIndex];
        switch( nType )
        {
            case SFX_STYLE_FAMILY_CHAR:
            {
                if(!pxCharStyles)
                {
                    ((SwXStyleFamilies*)this)->pxCharStyles = new uno::Reference< container::XNameContainer > ();
                    *pxCharStyles = new SwXStyleFamily(pDocShell, nType);
                }
                aRef = *pxCharStyles;
            }
            break;
            case SFX_STYLE_FAMILY_PARA:
            {
                if(!pxParaStyles)
                {
                    ((SwXStyleFamilies*)this)->pxParaStyles = new uno::Reference< container::XNameContainer > ();
                    *pxParaStyles = new SwXStyleFamily(pDocShell, nType);
                }
                aRef = *pxParaStyles;
            }
            break;
            case SFX_STYLE_FAMILY_PAGE     :
            {
                if(!pxPageStyles)
                {
                    ((SwXStyleFamilies*)this)->pxPageStyles = new uno::Reference< container::XNameContainer > ();
                    *pxPageStyles = new SwXStyleFamily(pDocShell, nType);
                }
                aRef = *pxPageStyles;
            }
            break;
            case SFX_STYLE_FAMILY_FRAME    :
            {
                if(!pxFrameStyles)
                {
                    ((SwXStyleFamilies*)this)->pxFrameStyles = new uno::Reference< container::XNameContainer > ();
                    *pxFrameStyles = new SwXStyleFamily(pDocShell, nType);
                }
                aRef = *pxFrameStyles;
            }
            break;
            case SFX_STYLE_FAMILY_PSEUDO:
            {
                if(!pxNumberingStyles)
                {
                    ((SwXStyleFamilies*)this)->pxNumberingStyles = new uno::Reference< container::XNameContainer > ();
                    *pxNumberingStyles = new SwXStyleFamily(pDocShell, nType);
                }
                aRef = *pxNumberingStyles;
            }
            break;
        }
        aRet.setValue(&aRef, ::getCppuType((const uno::Reference<container::XNameContainer>*)0));
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 16.12.98 15:13:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXStyleFamilies::getElementType(void)
    throw( uno::RuntimeException )
{
    return ::getCppuType((const uno::Reference<container::XNameContainer>*)0);

}
/*-- 16.12.98 15:13:28---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyleFamilies::hasElements(void) throw( uno::RuntimeException )
{
    return sal_True;
}
/*-- 16.12.98 15:13:28---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyleFamilies::loadStylesFromURL(const OUString& rURL,
    const uno::Sequence< beans::PropertyValue >& aOptions)
    throw( io::IOException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool    bLoadStyleText = sal_True;
    sal_Bool    bLoadStylePage = sal_True;
    sal_Bool    bLoadStyleOverwrite = sal_True;
    sal_Bool    bLoadStyleNumbering = sal_True;
    sal_Bool    bLoadStyleFrame = sal_True;
    if(IsValid() && rURL.getLength())
    {
        const uno::Any* pVal;
        int nCount = aOptions.getLength();
        const beans::PropertyValue* pArray = aOptions.getConstArray();
        for(int i = 0; i < nCount; i++)
            if( ( pVal = &pArray[i].Value)->getValueType() ==
                    ::getBooleanCppuType() )
            {
                String sName = pArray[i].Name;
                sal_Bool bVal = *(sal_Bool*)pVal->getValue();
                if( sName.EqualsAscii(SW_PROP_NAME_STR(UNO_NAME_OVERWRITE_STYLES     )))
                    bLoadStyleOverwrite = bVal;
                else if( sName.EqualsAscii(SW_PROP_NAME_STR(UNO_NAME_LOAD_NUMBERING_STYLES )))
                    bLoadStyleNumbering = bVal;
                else if( sName.EqualsAscii(SW_PROP_NAME_STR(UNO_NAME_LOAD_PAGE_STYLES   )))
                    bLoadStylePage = bVal;
                else if( sName.EqualsAscii(SW_PROP_NAME_STR(UNO_NAME_LOAD_FRAME_STYLES     )))
                    bLoadStyleFrame = bVal;
                else if( sName.EqualsAscii(SW_PROP_NAME_STR(UNO_NAME_LOAD_TEXT_STYLES      )))
                    bLoadStyleText = bVal;
            }

        SwgReaderOption aOpt;
        aOpt.SetFrmFmts( bLoadStyleFrame );
        aOpt.SetTxtFmts( bLoadStyleText );
        aOpt.SetPageDescs( bLoadStylePage );
        aOpt.SetNumRules( bLoadStyleNumbering );
        aOpt.SetMerge( !bLoadStyleOverwrite );

        ULONG nErr = pDocShell->LoadStylesFromFile( rURL, aOpt, TRUE );
        if( nErr )
            throw io::IOException();
    }
    else
        throw uno::RuntimeException();
}
/*-- 16.12.98 15:13:28---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXStyleFamilies::getStyleLoaderOptions(void)
        throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< beans::PropertyValue > aSeq(5);
    beans::PropertyValue* pArray = aSeq.getArray();
    uno::Any aVal;
    sal_Bool bTemp = sal_True;
    aVal.setValue(&bTemp, ::getCppuBooleanType());
    pArray[0] = beans::PropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_LOAD_TEXT_STYLES)), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    aVal.setValue(&bTemp, ::getCppuBooleanType());
    pArray[1] = beans::PropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_LOAD_FRAME_STYLES)), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    aVal.setValue(&bTemp, ::getCppuBooleanType());
    pArray[2] = beans::PropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_LOAD_PAGE_STYLES)), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    aVal.setValue(&bTemp, ::getCppuBooleanType());
    pArray[3] = beans::PropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_LOAD_NUMBERING_STYLES)), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    aVal.setValue(&bTemp, ::getCppuBooleanType());
    pArray[4] = beans::PropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_OVERWRITE_STYLES)), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    return aSeq;
}

/******************************************************************
 * SwXStyleFamily
 ******************************************************************/
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXStyleFamily::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXStyleFamily");
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXStyleFamily::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return C2U("com.sun.star.style.StyleFamily") == rServiceName;
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXStyleFamily::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.style.StyleFamily");
    return aRet;
}
/*-- 16.12.98 16:03:56---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyleFamily::SwXStyleFamily(SwDocShell* pDocSh, sal_uInt16 nFamily) :
        eFamily((SfxStyleFamily)nFamily),
        pBasePool(pDocSh->GetStyleSheetPool()),
        pDocShell(pDocSh)
{
/*  switch( nFamily )
    {
        case SFX_STYLE_FAMILY_CHAR:
            _pPropMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_CHAR_STYLE);
        break;
        case SFX_STYLE_FAMILY_PARA:
            _pPropMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARA_STYLE);
        break;
        case SFX_STYLE_FAMILY_PAGE:
            _pPropMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PAGE_STYLE);
        break;
        case SFX_STYLE_FAMILY_FRAME:
            _pPropMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_FRAME_STYLE);
        break;
        case SFX_STYLE_FAMILY_PSEUDO:
            _pPropMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_NUM_STYLE);
        break;
    }*/
    StartListening(*pBasePool);
}
/*-- 16.12.98 16:03:56---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyleFamily::~SwXStyleFamily()
{

}
sal_Int32 lcl_GetCountOrName ( const SwDoc &rDoc, SfxStyleFamily eFamily, String *pString, sal_uInt16 nIndex = USHRT_MAX )
{
    sal_Int32 nCount = 0;
    switch( eFamily )
    {
        case SFX_STYLE_FAMILY_CHAR:
        {
            sal_uInt16 nBaseCount =  RES_POOLCHR_HTML_END - RES_POOLCHR_HTML_BEGIN  +
                                     RES_POOLCHR_NORMAL_END - RES_POOLCHR_NORMAL_BEGIN;
            nIndex = nIndex - nBaseCount;
            const sal_uInt16 nArrLen = rDoc.GetCharFmts()->Count();
            for( sal_uInt16 i = 0; i < nArrLen; i++ )
            {
                SwCharFmt* pFmt = (*rDoc.GetCharFmts())[ i ];
                if( pFmt->IsDefault() && pFmt != rDoc.GetDfltCharFmt() )
                    continue;
                if ( IsPoolUserFmt ( pFmt->GetPoolFmtId() ) )
                {
                    if ( nIndex == nCount )
                    {
                        // the default character format needs to be set to "Default!"
                        if(rDoc.GetDfltCharFmt() == pFmt)
                            SwStyleNameMapper::FillUIName(
                                RES_POOLCOLL_STANDARD, *pString );
                        else
                            *pString = pFmt->GetName();
                        break;
                    }
                    nCount++;
                }
            }
            nCount += nBaseCount;
        }
        break;
        case SFX_STYLE_FAMILY_PARA:
        {
            sal_uInt16 nBaseCount = RES_POOLCOLL_HTML_END - RES_POOLCOLL_HTML_BEGIN +
                                    RES_POOLCOLL_DOC_END - RES_POOLCOLL_DOC_BEGIN +
                                    RES_POOLCOLL_REGISTER_END - RES_POOLCOLL_REGISTER_BEGIN +
                                    RES_POOLCOLL_EXTRA_END - RES_POOLCOLL_EXTRA_BEGIN +
                                    RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                    RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN;
            nIndex = nIndex - nBaseCount;
            const sal_uInt16 nArrLen = rDoc.GetTxtFmtColls()->Count();
            for ( sal_uInt16 i = 0; i < nArrLen; i++ )
            {
                SwTxtFmtColl * pColl = (*rDoc.GetTxtFmtColls())[i];
                if ( pColl->IsDefault() )
                    continue;
                if ( IsPoolUserFmt ( pColl->GetPoolFmtId() ) )
                {
                    if ( nIndex == nCount )
                    {
                        *pString = pColl->GetName();
                        break;
                    }
                    nCount++;
                }
            }
            nCount += nBaseCount;
        }
        break;
        case SFX_STYLE_FAMILY_FRAME:
        {
            sal_uInt16 nBaseCount = RES_POOLFRM_END - RES_POOLFRM_BEGIN;
            nIndex = nIndex - nBaseCount;
            const sal_uInt16 nArrLen = rDoc.GetFrmFmts()->Count();
            for( sal_uInt16 i = 0; i < nArrLen; i++ )
            {
                SwFrmFmt* pFmt = (*rDoc.GetFrmFmts())[ i ];
                if(pFmt->IsDefault() || pFmt->IsAuto())
                    continue;
                if ( IsPoolUserFmt ( pFmt->GetPoolFmtId() ) )
                {
                    if ( nIndex == nCount )
                    {
                        *pString = pFmt->GetName();
                        break;
                    }
                    nCount++;
                }
            }
            nCount += nBaseCount;
        }
        break;
        case SFX_STYLE_FAMILY_PAGE:
        {
            sal_uInt16 nBaseCount = RES_POOLPAGE_END - RES_POOLPAGE_BEGIN;
            nIndex = nIndex - nBaseCount;
            const sal_uInt16 nArrLen = rDoc.GetPageDescCnt();
            for(sal_uInt16 i = 0; i < nArrLen; ++i)
            {
                const SwPageDesc& rDesc = rDoc.GetPageDesc(i);

                /*if(rDesc.IsDefault() || rDesc.IsAuto())
                    continue;*/
                if ( IsPoolUserFmt ( rDesc.GetPoolFmtId() ) )
                {
                    if ( nIndex == nCount )
                    {
                        *pString = rDesc.GetName();
                        break;
                    }
                    nCount++;
                }
            }
            nCount += nBaseCount;
        }
        break;
        case SFX_STYLE_FAMILY_PSEUDO:
        {
            sal_uInt16 nBaseCount = RES_POOLNUMRULE_END - RES_POOLNUMRULE_BEGIN;
            nIndex = nIndex - nBaseCount;
            const SwNumRuleTbl& rNumTbl = rDoc.GetNumRuleTbl();
            for(USHORT i = 0; i < rNumTbl.Count(); ++i)
            {
                const SwNumRule& rRule = *rNumTbl[ i ];
                if( rRule.IsAutoRule() )
                    continue;
                if ( IsPoolUserFmt ( rRule.GetPoolFmtId() ) )
                {
                    if ( nIndex == nCount )
                    {
                        *pString = rRule.GetName();
                        break;
                    }
                    nCount++;
                }
            }
            nCount += nBaseCount;
        }
        break;

        default:
            ;
    }
    return nCount;
}
/*-- 16.12.98 16:03:57---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXStyleFamily::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return lcl_GetCountOrName ( *pDocShell->GetDoc(), eFamily, NULL );
}
/*-- 16.12.98 16:03:57---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXStyleFamily::getByIndex(sal_Int32 nTempIndex)
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if ( nTempIndex >= 0 && nTempIndex < USHRT_MAX )
    {
        sal_uInt16 nIndex = static_cast < sal_uInt16 > ( nTempIndex );
        if(pBasePool)
        {
            String sStyleName;
            switch( eFamily )
            {
                case SFX_STYLE_FAMILY_CHAR:
                {
                    if ( nIndex < ( RES_POOLCHR_NORMAL_END - RES_POOLCHR_NORMAL_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCHR_NORMAL_BEGIN + nIndex), sStyleName );
                    else if ( nIndex < ( RES_POOLCHR_HTML_END - RES_POOLCHR_HTML_BEGIN  +
                                         RES_POOLCHR_NORMAL_END - RES_POOLCHR_NORMAL_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( RES_POOLCHR_HTML_BEGIN
                                                        - RES_POOLCHR_NORMAL_END + RES_POOLCHR_NORMAL_BEGIN
                                                        + nIndex, sStyleName );
                }
                break;
                case SFX_STYLE_FAMILY_PARA:
                {
                    if ( nIndex < ( RES_POOLCOLL_TEXT_END - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCOLL_TEXT_BEGIN + nIndex), sStyleName );
                    else if ( nIndex < ( RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                         RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( RES_POOLCOLL_LISTS_BEGIN
                                                        - RES_POOLCOLL_TEXT_END + RES_POOLCOLL_TEXT_BEGIN
                                                        + nIndex, sStyleName );
                    else if ( nIndex < ( RES_POOLCOLL_EXTRA_END - RES_POOLCOLL_EXTRA_BEGIN +
                                         RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                         RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCOLL_EXTRA_BEGIN
                                                         - RES_POOLCOLL_LISTS_END + RES_POOLCOLL_LISTS_BEGIN
                                                         - RES_POOLCOLL_TEXT_END  + RES_POOLCOLL_TEXT_BEGIN
                                                         + nIndex), sStyleName );
                    else if ( nIndex < ( RES_POOLCOLL_REGISTER_END - RES_POOLCOLL_REGISTER_BEGIN +
                                         RES_POOLCOLL_EXTRA_END - RES_POOLCOLL_EXTRA_BEGIN +
                                         RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                         RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCOLL_REGISTER_BEGIN
                                                         - RES_POOLCOLL_EXTRA_END + RES_POOLCOLL_EXTRA_BEGIN
                                                         - RES_POOLCOLL_LISTS_END + RES_POOLCOLL_LISTS_BEGIN
                                                         - RES_POOLCOLL_TEXT_END  + RES_POOLCOLL_TEXT_BEGIN
                                                         + nIndex), sStyleName );
                    else if ( nIndex < ( RES_POOLCOLL_DOC_END - RES_POOLCOLL_DOC_BEGIN +
                                         RES_POOLCOLL_REGISTER_END - RES_POOLCOLL_REGISTER_BEGIN +
                                         RES_POOLCOLL_EXTRA_END - RES_POOLCOLL_EXTRA_BEGIN +
                                         RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                         RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCOLL_DOC_BEGIN
                                                         - RES_POOLCOLL_REGISTER_END + RES_POOLCOLL_REGISTER_BEGIN
                                                         - RES_POOLCOLL_EXTRA_END + RES_POOLCOLL_EXTRA_BEGIN
                                                         - RES_POOLCOLL_LISTS_END + RES_POOLCOLL_LISTS_BEGIN
                                                         - RES_POOLCOLL_TEXT_END  + RES_POOLCOLL_TEXT_BEGIN
                                                         + nIndex), sStyleName );
                    else if ( nIndex < ( RES_POOLCOLL_HTML_END - RES_POOLCOLL_HTML_BEGIN +
                                         RES_POOLCOLL_DOC_END - RES_POOLCOLL_DOC_BEGIN +
                                         RES_POOLCOLL_REGISTER_END - RES_POOLCOLL_REGISTER_BEGIN +
                                         RES_POOLCOLL_EXTRA_END - RES_POOLCOLL_EXTRA_BEGIN +
                                         RES_POOLCOLL_LISTS_END - RES_POOLCOLL_LISTS_BEGIN +
                                         RES_POOLCOLL_TEXT_END  - RES_POOLCOLL_TEXT_BEGIN ) )
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLCOLL_HTML_BEGIN
                                                         - RES_POOLCOLL_DOC_END + RES_POOLCOLL_DOC_BEGIN
                                                         - RES_POOLCOLL_REGISTER_END + RES_POOLCOLL_REGISTER_BEGIN
                                                         - RES_POOLCOLL_EXTRA_END + RES_POOLCOLL_EXTRA_BEGIN
                                                         - RES_POOLCOLL_LISTS_END + RES_POOLCOLL_LISTS_BEGIN
                                                         - RES_POOLCOLL_TEXT_END  + RES_POOLCOLL_TEXT_BEGIN
                                                         + nIndex), sStyleName );
                }
                break;
                case SFX_STYLE_FAMILY_FRAME:
                {
                    if ( nIndex < ( RES_POOLFRM_END - RES_POOLFRM_BEGIN ) )
                    {
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLFRM_BEGIN + nIndex), sStyleName );
                    }
                }
                break;
                case SFX_STYLE_FAMILY_PAGE:
                {
                    if ( nIndex < ( RES_POOLPAGE_END - RES_POOLPAGE_BEGIN ) )
                    {
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLPAGE_BEGIN + nIndex), sStyleName );
                    }
                }
                break;
                case SFX_STYLE_FAMILY_PSEUDO:
                {
                    if ( nIndex < ( RES_POOLNUMRULE_END - RES_POOLNUMRULE_BEGIN ) )
                    {
                        SwStyleNameMapper::FillUIName ( static_cast< sal_uInt16 >(RES_POOLNUMRULE_BEGIN + nIndex), sStyleName );
                    }
                }
                break;

                default:
                    ;
            }
            if ( !sStyleName.Len() )
                lcl_GetCountOrName ( *pDocShell->GetDoc(), eFamily, &sStyleName, nIndex );

            if ( sStyleName.Len() )
            {
                SfxStyleSheetBase* pBase = pBasePool->Find( sStyleName, eFamily );
                if(pBase)
                {
                    uno::Reference< style::XStyle >  xStyle = _FindStyle(sStyleName);
                    if(!xStyle.is())
                    {
                        xStyle = eFamily == SFX_STYLE_FAMILY_PAGE ?
                            new SwXPageStyle(*pBasePool, pDocShell, eFamily, sStyleName) :
                                eFamily == SFX_STYLE_FAMILY_FRAME ?
                                new SwXFrameStyle(*pBasePool, pDocShell->GetDoc(), pBase->GetName()):
                                    new SwXStyle(*pBasePool, eFamily, pDocShell->GetDoc(), sStyleName);
                    }
                    aRet.setValue(&xStyle, ::getCppuType((uno::Reference<style::XStyle>*)0));
                }
                else
                    throw container::NoSuchElementException();
            }
            else
                throw lang::IndexOutOfBoundsException();
        }
        else
            throw uno::RuntimeException();
    }
    else
        throw lang::IndexOutOfBoundsException();

    return aRet;
}
/*-- 16.12.98 16:03:57---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXStyleFamily::getByName(const OUString& rName)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    String sStyleName;
    SwStyleNameMapper::FillUIName(rName, sStyleName, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        if(pBase)
        {
            uno::Reference< style::XStyle >  xStyle = _FindStyle(sStyleName);
            if(!xStyle.is())
            {
                xStyle = eFamily == SFX_STYLE_FAMILY_PAGE ?
                    new SwXPageStyle(*pBasePool, pDocShell, eFamily, sStyleName) :
                        eFamily == SFX_STYLE_FAMILY_FRAME ?
                        new SwXFrameStyle(*pBasePool, pDocShell->GetDoc(), pBase->GetName()):
                            new SwXStyle(*pBasePool, eFamily, pDocShell->GetDoc(), sStyleName);
            }
            aRet.setValue(&xStyle, ::getCppuType((uno::Reference<style::XStyle>*)0));
        }
        else
            throw container::NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
    return aRet;

}
/*-- 16.12.98 16:03:57---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXStyleFamily::getElementNames(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< OUString > aRet;
    if(pBasePool)
    {
        SfxStyleSheetIterator* pIterator = pBasePool->CreateIterator(eFamily, 0xffff);
        sal_uInt16 nCount = pIterator->Count();
        aRet.realloc(nCount);
        OUString* pArray = aRet.getArray();
        String aString;
        for(sal_uInt16 i = 0; i < nCount; i++)
        {
            SwStyleNameMapper::FillProgName((*pIterator)[i]->GetName(), aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );
            pArray[i] = OUString ( aString );
        }
        delete pIterator;
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 16.12.98 16:03:57---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyleFamily::hasByName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pBasePool)
    {
        String sStyleName;
        SwStyleNameMapper::FillUIName(rName, sStyleName, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        bRet = 0 != pBase;
    }
    else
        throw uno::RuntimeException();
    return bRet;

}
/*-- 16.12.98 16:03:58---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXStyleFamily::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((const uno::Reference<style::XStyle>*)0);

}
/*-- 16.12.98 16:03:58---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyleFamily::hasElements(void) throw( uno::RuntimeException )
{
    if(!pBasePool)
        throw uno::RuntimeException();
    return sal_True;
}
/*-- 16.12.98 16:03:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyleFamily::insertByName(const OUString& rName, const uno::Any& rElement)
        throw( lang::IllegalArgumentException, container::ElementExistException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(pBasePool)
    {
        String sStyleName;
        SwStyleNameMapper::FillUIName(rName, sStyleName, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True);
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        SfxStyleSheetBase* pUINameBase = pBasePool->Find( sStyleName );
        if(pBase || pUINameBase)
            throw container::ElementExistException();
        else
        {
            if(rElement.getValueType().getTypeClass() ==
                                            uno::TypeClass_INTERFACE)
            {
                uno::Reference< uno::XInterface > * pxRef =
                    (uno::Reference< uno::XInterface > *)rElement.getValue();

                uno::Reference<lang::XUnoTunnel> xStyleTunnel( *pxRef, uno::UNO_QUERY);

                SwXStyle* pNewStyle = 0;
                if(xStyleTunnel.is())
                {
                    pNewStyle = reinterpret_cast< SwXStyle * >(
                            sal::static_int_cast< sal_IntPtr >( xStyleTunnel->getSomething( SwXStyle::getUnoTunnelId()) ));
                }
                if(!pNewStyle || !pNewStyle->IsDescriptor() ||
                    pNewStyle->GetFamily() != eFamily)
                        throw lang::IllegalArgumentException();
                if(pNewStyle)
                {
                    sal_uInt16 nMask = 0xffff;
                    if(eFamily == SFX_STYLE_FAMILY_PARA && !pNewStyle->IsConditional())
                        nMask &= ~SWSTYLEBIT_CONDCOLL;
#if OSL_DEBUG_LEVEL > 1
                    SfxStyleSheetBase& rNewBase =
#endif
                        pBasePool->Make(sStyleName, eFamily, nMask);
                    pNewStyle->SetDoc(pDocShell->GetDoc(), pBasePool);
                    pNewStyle->SetStyleName(sStyleName);
                    String sParentStyleName(pNewStyle->GetParentStyleName());
                    if(sParentStyleName.Len())
                    {
                        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
                        SfxStyleSheetBase* pParentBase = pBasePool->Find(sParentStyleName);
                        if(pParentBase && pParentBase->GetFamily() == eFamily &&
                            &pParentBase->GetPool() == pBasePool)
                            pBasePool->SetParent( eFamily, sStyleName,  sParentStyleName );

                    }
#if OSL_DEBUG_LEVEL > 1
                    (void)rNewBase;
#endif
                    //so, jetzt sollten noch die Properties des Descriptors angewandt werden
                    pNewStyle->ApplyDescriptorProperties();
                }
                else
                    throw lang::IllegalArgumentException();
            }
            else
                throw lang::IllegalArgumentException();
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 16.12.98 16:03:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyleFamily::replaceByName(const OUString& rName, const uno::Any& rElement)
    throw( lang::IllegalArgumentException, container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily);
        SfxStyleSheetBase* pBase = pBasePool->Find(rName);
        //Ersetzung geht nur fuer benutzerdefinierte Styles
        if(!pBase)
            throw container::NoSuchElementException();
        if(!pBase->IsUserDefined())
            throw lang::IllegalArgumentException();
        //if theres an object available to this style then it must be invalidated
        uno::Reference< style::XStyle >  xStyle = _FindStyle(pBase->GetName());
        if(xStyle.is())
        {
            uno::Reference<lang::XUnoTunnel> xTunnel( xStyle, uno::UNO_QUERY);
            if(xTunnel.is())
            {
                SwXStyle* pStyle = reinterpret_cast< SwXStyle * >(
                        sal::static_int_cast< sal_IntPtr >( xTunnel->getSomething( SwXStyle::getUnoTunnelId()) ));
                pStyle->Invalidate();
            }
        }

        pBasePool->Erase(pBase);
        insertByName(rName, rElement);
    }
    else
        throw uno::RuntimeException();
}
/*-- 16.12.98 16:03:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyleFamily::removeByName(const OUString& rName) throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        String aString;
        SwStyleNameMapper::FillUIName(rName, aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );

        SfxStyleSheetBase* pBase = pBasePool->Find( aString );
        if(pBase)
            pBasePool->Erase(pBase);
        else
            throw container::NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
}
/*-- 16.12.98 16:03:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyleFamily::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    SfxSimpleHint *pHint = PTR_CAST( SfxSimpleHint, &rHint );
    if( pHint && ( pHint->GetId() & SFX_HINT_DYING ) )
    {
        pBasePool = 0;
        pDocShell = 0;
        EndListening(rBC);
    }
}
/*-- 16.12.98 16:03:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyle*   SwXStyleFamily::_FindStyle(const String& rStyleName)const
{
    sal_uInt16  nLCount = pBasePool->GetListenerCount();
    SfxListener* pListener = 0;
    for( sal_uInt16 i = 0; i < nLCount; i++)
    {
        pListener = pBasePool->GetListener( i );
        if(PTR_CAST(SwXStyle, pListener) && ((SwXStyle*)pListener)->GetFamily() == eFamily && ((SwXStyle*)pListener)->GetStyleName() == rStyleName)
        {
            return (SwXStyle*)pListener;
        }
    }
    return 0;
}
/******************************************************************
 *
 ******************************************************************/
class SwStyleProperties_Impl
{
    const SfxItemPropertyMap*   _pMap;
    uno::Any**                  pAnyArr;
    sal_uInt16                  nArrLen;

public:
    SwStyleProperties_Impl(const SfxItemPropertyMap* _pMap);
    ~SwStyleProperties_Impl();

    sal_Bool    SetProperty(const String& rName, uno::Any aVal);
    sal_Bool    GetProperty(const String& rName, uno::Any*& rpAny);
    sal_Bool    ClearProperty( const OUString& rPropertyName );
    void    ClearAllProperties( );
    void       	GetProperty(const OUString &rPropertyName, const uno::Reference < beans::XPropertySet > &rxPropertySet, uno::Any& rAny );


    const SfxItemPropertyMap*   GetPropertyMap() const {return _pMap;}
};
//--------------------------------------------------------------------
//--------------------------------------------------------------------
SwStyleProperties_Impl::SwStyleProperties_Impl(const SfxItemPropertyMap* pMap) :
    _pMap(pMap),
    nArrLen(0)
{
    const SfxItemPropertyMap* pTmp = _pMap;
    while(pTmp[nArrLen].nWID)
        nArrLen++;
    pAnyArr = new uno::Any* [nArrLen];
    for ( sal_uInt16 i =0 ; i < nArrLen; i++ )
        pAnyArr[i] = 0;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
SwStyleProperties_Impl::~SwStyleProperties_Impl()
{
    for ( sal_uInt16 i =0 ; i < nArrLen; i++ )
        delete pAnyArr[i];
    delete[] pAnyArr;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
sal_Bool SwStyleProperties_Impl::SetProperty(const String& rName, uno::Any aVal)
{
    sal_uInt16 nPos = 0;
    const SfxItemPropertyMap* pTemp = _pMap;
    while( pTemp->pName )
    {
        if(rName.EqualsAscii(pTemp->pName))
            break;
        ++nPos;
        ++pTemp;
    }
    if(nPos < nArrLen)
    {
        delete pAnyArr[nPos];
        pAnyArr[nPos] = new uno::Any ( aVal );
    }
    return nPos < nArrLen;
}

sal_Bool SwStyleProperties_Impl::ClearProperty( const OUString& rName )
{
    sal_uInt16 nPos = 0;
    const SfxItemPropertyMap* pTemp = _pMap;
    while( pTemp->pName )
    {
        if( rName.equalsAsciiL ( pTemp->pName, pTemp->nNameLen ) )
            break;
        ++nPos;
        ++pTemp;
    }
    if( nPos < nArrLen )
    {
        delete pAnyArr[nPos];
        pAnyArr[ nPos ] = 0;
    }
    return nPos < nArrLen;
}
void SwStyleProperties_Impl::ClearAllProperties( )
{
    for ( sal_uInt16 i = 0; i < nArrLen; i++ )
    {
        delete pAnyArr[i];
        pAnyArr[ i ] = 0;
    }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
sal_Bool SwStyleProperties_Impl::GetProperty(const String& rName, uno::Any*& rpAny )
{
    sal_uInt16 nPos = 0;
    const SfxItemPropertyMap* pTemp = _pMap;
    while( pTemp->pName )
    {
        if(rName.EqualsAscii(pTemp->pName))
            break;
        ++nPos;
        ++pTemp;
    }
    if(nPos < nArrLen)
        rpAny = pAnyArr[nPos];
    return nPos < nArrLen;
}

void SwStyleProperties_Impl::GetProperty( const OUString &rPropertyName, const uno::Reference < beans::XPropertySet > &rxPropertySet, uno::Any & rAny )
{
    rAny = rxPropertySet->getPropertyValue( rPropertyName );
}

/******************************************************************
 *
 ******************************************************************/
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXStyle::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXStyle::getSomething( const uno::Sequence< sal_Int8 >& rId )
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

TYPEINIT1(SwXStyle, SfxListener);
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXStyle::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXStyle");
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXStyle::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    BOOL bRet = C2U("com.sun.star.style.Style") == rServiceName;
    if(!bRet && SFX_STYLE_FAMILY_CHAR == eFamily)
        bRet = !rServiceName.compareToAscii("com.sun.star.style.CharacterStyle")||
               !rServiceName.compareToAscii("com.sun.star.style.CharacterProperties")||
               !rServiceName.compareToAscii("com.sun.star.style.CharacterPropertiesAsian")||
               !rServiceName.compareToAscii("com.sun.star.style.CharacterPropertiesComplex");
    if(!bRet && SFX_STYLE_FAMILY_PARA == eFamily)
        bRet = (C2U("com.sun.star.style.ParagraphStyle") == rServiceName)||
               (C2U("com.sun.star.style.ParagraphProperties") == rServiceName) ||
               (C2U("com.sun.star.style.ParagraphPropertiesAsian") == rServiceName) ||
               (C2U("com.sun.star.style.ParagraphPropertiesComplex") == rServiceName);
    if(!bRet && SFX_STYLE_FAMILY_PAGE == eFamily)
        bRet = (C2U("com.sun.star.style.PageStyle") == rServiceName)||
               (C2U("com.sun.star.style.PageProperties") == rServiceName);

    return  bRet;
}
/* -----------------------------06.04.00 11:24--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXStyle::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    long nCount = 1;
    if(SFX_STYLE_FAMILY_PARA == eFamily)
    {
        nCount = 5;
        if(bIsConditional)
            nCount++;
    }
    else if(SFX_STYLE_FAMILY_CHAR == eFamily)
        nCount = 5;
    else if(SFX_STYLE_FAMILY_PAGE == eFamily)
        nCount = 3;
    uno::Sequence< OUString > aRet(nCount);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.style.Style");
    switch(eFamily)
    {
        case SFX_STYLE_FAMILY_CHAR:
            pArray[1] = C2U("com.sun.star.style.CharacterStyle");
            pArray[2] = C2U("com.sun.star.style.CharacterProperties");
            pArray[3] = C2U("com.sun.star.style.CharacterPropertiesAsian");
            pArray[4] = C2U("com.sun.star.style.CharacterPropertiesComplex");
        break;
        case SFX_STYLE_FAMILY_PAGE:
            pArray[1] = C2U("com.sun.star.style.PageStyle");
            pArray[2] = C2U("com.sun.star.style.PageProperties");
        break;
        case SFX_STYLE_FAMILY_PARA:
            pArray[1] = C2U("com.sun.star.style.ParagraphStyle");
            pArray[2] = C2U("com.sun.star.style.ParagraphProperties");
            pArray[3] = C2U("com.sun.star.style.ParagraphPropertiesAsian");
            pArray[4] = C2U("com.sun.star.style.ParagraphPropertiesComplex");
        if(bIsConditional)
            pArray[5] = C2U("com.sun.star.style.ConditionalParagraphStyle");
        break;

        default:
            ;
    }
    return aRet;
}
/*-- 17.12.98 08:26:49---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyle::SwXStyle( SwDoc *pDoc, SfxStyleFamily eFam, BOOL bConditional) :
    m_pDoc( pDoc ),
    pBasePool(0),
    eFamily(eFam),
    bIsDescriptor(sal_True),
    bIsConditional(bConditional)
{
    // Register ourselves as a listener to the document (via the page descriptor)
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
    // get the property set for the default style data
    // First get the model
    uno::Reference < frame::XModel > xModel = pDoc->GetDocShell()->GetBaseModel();
    // Ask the model for it's family supplier interface
    uno::Reference < style::XStyleFamiliesSupplier > xFamilySupplier ( xModel, uno::UNO_QUERY );
    // Get the style families
    uno::Reference < container::XNameAccess > xFamilies = xFamilySupplier->getStyleFamilies();

    uno::Any aAny;
    sal_uInt16 nMapId = PROPERTY_MAP_NUM_STYLE;
    switch( eFamily )
    {
        case SFX_STYLE_FAMILY_CHAR:
        {
            nMapId = PROPERTY_MAP_CHAR_STYLE;
            aAny = xFamilies->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "CharacterStyles" ) ) );
            // Get the Frame family (and keep it for later)
            aAny >>= mxStyleFamily;
            //aAny = mxStyleFamily->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Standard" ) ) );
            //aAny >>= mxStyleData;
        }
        break;
        case SFX_STYLE_FAMILY_PARA:
        {
            nMapId = bIsConditional ? PROPERTY_MAP_CONDITIONAL_PARA_STYLE : PROPERTY_MAP_PARA_STYLE;
            aAny = xFamilies->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "ParagraphStyles" ) ) );
            // Get the Frame family (and keep it for later)
            aAny >>= mxStyleFamily;
            aAny = mxStyleFamily->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Standard" ) ) );
            aAny >>= mxStyleData;
        }
        break;
        case SFX_STYLE_FAMILY_PAGE:
        {
            nMapId = PROPERTY_MAP_PAGE_STYLE;
            aAny = xFamilies->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "PageStyles" ) ) );
            // Get the Frame family (and keep it for later)
            aAny >>= mxStyleFamily;
            aAny = mxStyleFamily->getByName ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Standard" ) ) );
            aAny >>= mxStyleData;
        }
        break;
        case SFX_STYLE_FAMILY_FRAME :
        {
            nMapId = PROPERTY_MAP_FRAME_STYLE;
        }
        break;
        case SFX_STYLE_FAMILY_PSEUDO:
        {
            nMapId = PROPERTY_MAP_NUM_STYLE;
        }
        break;

        default:
            ;
    }
    pPropImpl = new SwStyleProperties_Impl(aSwMapProvider.GetPropertyMap(nMapId));
}


SwXStyle::SwXStyle(SfxStyleSheetBasePool& rPool, SfxStyleFamily eFam,
        SwDoc*  pDoc,   const String& rStyleName) :
    m_pDoc(pDoc),
    sStyleName(rStyleName),
    pBasePool(&rPool),
    eFamily(eFam),
    bIsDescriptor(sal_False),
    bIsConditional(sal_False),
    pPropImpl(0)
{
    StartListening(rPool);
    if(eFam == SFX_STYLE_FAMILY_PARA)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "where is the style?" );
        if(pBase)
        {
            const sal_uInt16 nId = SwStyleNameMapper::GetPoolIdFromUIName(sStyleName, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL);
            if(nId != USHRT_MAX)
                bIsConditional = ::IsConditionalByPoolId( nId );
            else
                bIsConditional = RES_CONDTXTFMTCOLL == ((SwDocStyleSheet*)pBase)->GetCollection()->Which();
        }
    }
}
/*-- 17.12.98 08:26:50---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXStyle::~SwXStyle()
{
    if(pBasePool)
        EndListening(*pBasePool);
    delete pPropImpl;
}
/*-- 17.12.98 08:26:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
    {
        m_pDoc = 0;
        mxStyleData.clear();
        mxStyleFamily.clear();
    }
}
OUString SwXStyle::getName(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    String aString;
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "where is the style?" );
        if(!pBase)
            throw uno::RuntimeException();
        SwStyleNameMapper::FillProgName(pBase->GetName(), aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True);
    }
    else
        aString = sStyleName;
    return OUString (aString);
}
/*-- 17.12.98 08:26:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::setName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "where is the style?" );
        sal_Bool bExcept = sal_True;
        if(pBase && pBase->IsUserDefined())
        {
            SwDocStyleSheet aTmp( *(SwDocStyleSheet*)pBase );
            bExcept = !aTmp.SetName(rName);
            if(!bExcept)
                sStyleName = String(rName);
        }
        if(bExcept)
            throw uno::RuntimeException();
    }
    else
        sStyleName = String(rName);
}
/*-- 17.12.98 08:26:51---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyle::isUserDefined(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        //if it is not found it must be non user defined
        if(pBase)
            bRet = pBase->IsUserDefined();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 08:26:51---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXStyle::isInUse(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_USED);
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        if(pBase)
            bRet = pBase->IsUsed();
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 17.12.98 08:26:52---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXStyle::getParentStyle(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    String aString;
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL);
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        if(pBase)
            aString = pBase->GetParent();
    }
    else if(bIsDescriptor)
        aString = sParentStyleName;
    else
        throw uno::RuntimeException();
    SwStyleNameMapper::FillProgName(aString, aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );
    return OUString ( aString );
}
/*-- 17.12.98 08:26:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::setParentStyle(const OUString& rParentStyle)
            throw( container::NoSuchElementException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    String sParentStyle;
    SwStyleNameMapper::FillUIName(rParentStyle, sParentStyle, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily);
        BOOL bExcept = FALSE;
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        if(pBase)
        {
            SwDocStyleSheet aBase(*(SwDocStyleSheet*)pBase);
            //make it a 'real' style - necessary for pooled styles
            aBase.GetItemSet();
            if(aBase.GetParent() != sParentStyle)
            {
                bExcept = !aBase.SetParent(sParentStyle);
            }
        }
        else
            bExcept = TRUE;
        if(bExcept)
            throw uno::RuntimeException();
    }
    else if(bIsDescriptor)
    {
        sParentStyleName = String(sParentStyle);
        try
        {
            uno::Any aAny = mxStyleFamily->getByName ( sParentStyle );
            aAny >>= mxStyleData;
        }
        catch ( container::NoSuchElementException& )
        {
        }
        catch ( lang::WrappedTargetException& )
        {
        }
        catch ( uno::RuntimeException& )
        {
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 17.12.98 08:26:52---------------------------------------------------

  -----------------------------------------------------------------------*/

uno::Reference< beans::XPropertySetInfo > lcl_getPropertySetInfo( SfxStyleFamily eFamily, sal_Bool bIsConditional )
{
    uno::Reference< beans::XPropertySetInfo >  xRet;
    switch( eFamily )
    {
        case SFX_STYLE_FAMILY_CHAR:
        {
            static uno::Reference< beans::XPropertySetInfo >  xCharRef;
            if(!xCharRef.is())
            {
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(PROPERTY_MAP_CHAR_STYLE));
                xCharRef = aPropSet.getPropertySetInfo();
            }
            xRet = xCharRef;
        }
        break;
        case SFX_STYLE_FAMILY_PARA:
        {
            static uno::Reference< beans::XPropertySetInfo >  xParaRef;
            if(!xParaRef.is())
            {
                sal_uInt16 nMapId = bIsConditional ? PROPERTY_MAP_CONDITIONAL_PARA_STYLE : PROPERTY_MAP_PARA_STYLE;
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(nMapId));
                xParaRef = aPropSet.getPropertySetInfo();
            }
            xRet = xParaRef;
        }
        break;
        case SFX_STYLE_FAMILY_PAGE     :
        {
            static uno::Reference< beans::XPropertySetInfo >  xPageRef;
            if(!xPageRef.is())
            {
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PAGE_STYLE) );
                xPageRef = aPropSet.getPropertySetInfo();
            }
            xRet = xPageRef;
        }
        break;
        case SFX_STYLE_FAMILY_FRAME    :
        {
            static uno::Reference< beans::XPropertySetInfo >  xFrameRef;
            if(!xFrameRef.is())
            {
                SfxItemPropertySet aPropSet(
                aSwMapProvider.GetPropertyMap(PROPERTY_MAP_FRAME_STYLE));
                xFrameRef = aPropSet.getPropertySetInfo();
            }
            xRet = xFrameRef;
        }
        break;
        case SFX_STYLE_FAMILY_PSEUDO:
        {
            static uno::Reference< beans::XPropertySetInfo >  xNumRef;
            if(!xNumRef.is())
            {
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(PROPERTY_MAP_NUM_STYLE));
                xNumRef = aPropSet.getPropertySetInfo();
            }
            xRet = xNumRef;
        }
        break;

        default:
            ;
    }
    return xRet;
}

uno::Reference< beans::XPropertySetInfo >  SwXStyle::getPropertySetInfo(void)
    throw( uno::RuntimeException )
{
    return lcl_getPropertySetInfo( eFamily, bIsConditional );
}
/* -----------------23.04.99 13:28-------------------
 *
 * --------------------------------------------------*/
void    SwXStyle::ApplyDescriptorProperties()
{
    bIsDescriptor = sal_False;
    mxStyleData.clear();
    mxStyleFamily.clear();

    const SfxItemPropertyMap* pTemp = pPropImpl->GetPropertyMap();
    while(pTemp->nWID)
    {
        uno::Any* pAny;
        String sPropName(C2S(pTemp->pName));
        OUString sUPropName(sPropName);
        pPropImpl->GetProperty(sPropName, pAny);
        if(pAny)
            setPropertyValue(sUPropName, *pAny);
        pTemp++;
    }
}

/*-- 18.04.01 13:07:27---------------------------------------------------

  -----------------------------------------------------------------------*/
struct SwStyleBase_Impl
{
    SwDoc&              rDoc;

    const SwPageDesc*   pOldPageDesc;

    SwDocStyleSheet*    pNewBase;
    SfxItemSet*         pItemSet;

    const String&       rStyleName;
    sal_uInt16              nPDescPos;

    SwStyleBase_Impl(SwDoc& rSwDoc, const String& rName) :
        rDoc(rSwDoc),
        pOldPageDesc(0),
        pNewBase(0),
        pItemSet(0),
        rStyleName(rName),
        nPDescPos(0xffff)
        {}
    ~SwStyleBase_Impl(){delete pNewBase; delete pItemSet; }

    sal_Bool HasItemSet() {return 0 != pNewBase;}
    SfxItemSet& GetItemSet()
        {
            DBG_ASSERT(pNewBase, "no SwDocStyleSheet available")
            if(!pItemSet)
                pItemSet = new SfxItemSet(pNewBase->GetItemSet());
            return *pItemSet;
        }

        const SwPageDesc& GetOldPageDesc();
};
/* -----------------------------25.04.01 12:44--------------------------------

 ---------------------------------------------------------------------------*/
const SwPageDesc& SwStyleBase_Impl::GetOldPageDesc()
{
    if(!pOldPageDesc)
    {
        sal_uInt16 i;
        sal_uInt16 nPDescCount = rDoc.GetPageDescCnt();
        for(i = 0; i < nPDescCount; i++)
        {
            const SwPageDesc& rDesc =
                const_cast<const SwDoc &>(rDoc).GetPageDesc( i );
            if(rDesc.GetName() == rStyleName)
            {
                pOldPageDesc = & rDesc;
                nPDescPos = i;
                break;
            }
        }
        if(!pOldPageDesc)
        {
            for(i = RC_POOLPAGEDESC_BEGIN; i <= STR_POOLPAGE_ENDNOTE; ++i)
            {
                const String aFmtName(SW_RES(i));
                if(aFmtName == rStyleName)
                {
                    pOldPageDesc = rDoc.GetPageDescFromPool( static_cast< sal_uInt16 >(RES_POOLPAGE_BEGIN + i - RC_POOLPAGEDESC_BEGIN) );
                    break;
                }
            }
            for(i = 0; i < nPDescCount + 1; i++)
            {
                const SwPageDesc& rDesc =
                    const_cast<const SwDoc &>(rDoc).GetPageDesc( i );
                if(rDesc.GetName() == rStyleName)
                {
                    nPDescPos = i;
                    break;
                }
            }
        }
    }
    return *pOldPageDesc;
}

/* -----------------------------19.04.01 09:44--------------------------------

 ---------------------------------------------------------------------------*/

void lcl_SetStyleProperty(const SfxItemPropertyMap* pMap,
                        SfxItemPropertySet& rPropSet,
                        const uno::Any& rValue,
                        SwStyleBase_Impl& rBase,
                        SfxStyleSheetBasePool* pBasePool,
                        SwDoc* pDoc,
                        SfxStyleFamily eFamily)
                            throw(beans::PropertyVetoException, lang::IllegalArgumentException,
                                lang::WrappedTargetException, uno::RuntimeException)

{
    switch(pMap->nWID)
    {
        case RES_PAPER_BIN:
        {
            SfxPrinter *pPrinter = pDoc->getPrinter( true );
            OUString sTmp;
            sal_uInt16 nBin = USHRT_MAX;
            if ( !( rValue >>= sTmp ) )
                throw lang::IllegalArgumentException();
            if ( sTmp.equalsAsciiL ( RTL_CONSTASCII_STRINGPARAM ( "[From printer settings]" ) ) )
                nBin = USHRT_MAX-1;
            else if ( pPrinter )
            {
                for (sal_uInt16 i=0, nEnd = pPrinter->GetPaperBinCount(); i < nEnd; i++ )
                {
                    if (sTmp == OUString ( pPrinter->GetPaperBinName ( i ) ) )
                    {
                        nBin = i;
                        break;
                    }
                }
            }
            if ( nBin == USHRT_MAX )
                throw lang::IllegalArgumentException();
            else
            {
                SfxItemSet& rStyleSet = rBase.GetItemSet();
                SfxItemSet aSet(*rStyleSet.GetPool(), pMap->nWID, pMap->nWID);
                aSet.SetParent(&rStyleSet);
                rPropSet.setPropertyValue(*pMap, uno::makeAny ( static_cast < sal_Int8 > ( nBin == USHRT_MAX-1 ? -1 : nBin ) ), aSet);
                rStyleSet.Put(aSet);
            }
        }
        break;
        case  FN_UNO_NUM_RULES: //Sonderbehandlung fuer das SvxNumRuleItem:
        {
            if(rValue.getValueType() == ::getCppuType((uno::Reference< container::XIndexReplace>*)0) )
            {
                uno::Reference< container::XIndexReplace > * pxRulesRef =
                        (uno::Reference< container::XIndexReplace > *)rValue.getValue();

                uno::Reference<lang::XUnoTunnel> xNumberTunnel( *pxRulesRef, uno::UNO_QUERY);

                SwXNumberingRules* pSwXRules = 0;
                if(xNumberTunnel.is())
                {
                    pSwXRules = reinterpret_cast< SwXNumberingRules * >(
                            sal::static_int_cast< sal_IntPtr >(xNumberTunnel->getSomething( SwXNumberingRules::getUnoTunnelId()) ));
                }
                if(pSwXRules)
                {
                    const String* pCharStyleNames = pSwXRules->GetNewCharStyleNames();
                    const String* pBulletFontNames = pSwXRules->GetBulletFontNames();

                    SwNumRule aSetRule(*pSwXRules->GetNumRule());
                    const SwCharFmts* pFmts = pDoc->GetCharFmts();
                    sal_uInt16 nChCount = pFmts->Count();
                    for(sal_uInt16 i = 0; i < MAXLEVEL; i++)
                    {

                        const SwNumFmt* pFmt = aSetRule.GetNumFmt( i );
                        if(pFmt)
                        {
                            SwNumFmt aFmt(*pFmt);
                            if(
                                pCharStyleNames[i] != SwXNumberingRules::GetInvalidStyle() &&
                                ((pCharStyleNames[i].Len() && !pFmt->GetCharFmt()) ||
                                pCharStyleNames[i].Len() &&
                                            pFmt->GetCharFmt()->GetName() != pCharStyleNames[i] ))
                            {

                                SwCharFmt* pCharFmt = 0;
                                if(pCharStyleNames[i].Len())
                                {
                                    for(sal_uInt16 j = 0; j< nChCount; j++)
                                    {
                                        SwCharFmt* pTmp = (*pFmts)[j];
                                        if(pTmp->GetName() == pCharStyleNames[i])
                                        {
                                            pCharFmt = pTmp;
                                            break;
                                        }
                                    }
                                    if(!pCharFmt)
                                    {

                                        SfxStyleSheetBase* pBase;
                                        pBase = ((SfxStyleSheetBasePool*)pBasePool)->Find(pCharStyleNames[i], SFX_STYLE_FAMILY_CHAR);
                                        if(!pBase)
                                            pBase = &pBasePool->Make(pCharStyleNames[i], SFX_STYLE_FAMILY_CHAR);
                                        pCharFmt = ((SwDocStyleSheet*)pBase)->GetCharFmt();

                                    }

                                    aFmt.SetCharFmt( pCharFmt );
                                }
                            }
                            //jetzt nochmal fuer Fonts
                            if(pBulletFontNames[i] != SwXNumberingRules::GetInvalidStyle() &&
                                ((pBulletFontNames[i].Len() && !pFmt->GetBulletFont()) ||
                                pBulletFontNames[i].Len() &&
                                        pFmt->GetBulletFont()->GetName() != pBulletFontNames[i] ))
                            {
                                const SvxFontListItem* pFontListItem =
                                        (const SvxFontListItem* )pDoc->GetDocShell()
                                                            ->GetItem( SID_ATTR_CHAR_FONTLIST );
                                const FontList*  pList = pFontListItem->GetFontList();
                                FontInfo aInfo = pList->Get(
                                    pBulletFontNames[i],WEIGHT_NORMAL, ITALIC_NONE);
                                Font aFont(aInfo);
                                aFmt.SetBulletFont(&aFont);
                            }
                            aSetRule.Set( i, &aFmt );
                        }
                    }
                    rBase.pNewBase->SetNumRule(aSetRule);
                }
            }
            else
                throw lang::IllegalArgumentException();
        }
        break;
        case FN_UNO_DEFAULT_OUTLINE_LEVEL:
        {
            sal_Int8 nLevel = 0;
            if( rValue >>= nLevel )
                rBase.pNewBase->GetCollection()->SetOutlineLevel( nLevel );
            else
                rBase.pNewBase->GetCollection()->SetOutlineLevel( NO_NUMBERING );
        }
        break;
        case FN_UNO_FOLLOW_STYLE:
        {
            OUString sTmp;
            rValue >>= sTmp;
            String aString;
            SwStyleNameMapper::FillUIName(sTmp, aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True ) ;
            rBase.pNewBase->SetFollow( aString );
        }
        break;
        case RES_PAGEDESC :
        if( MID_PAGEDESC_PAGEDESCNAME != pMap->nMemberId)
            goto put_itemset;
        {
            // Sonderbehandlung RES_PAGEDESC
            if(rValue.getValueType() != ::getCppuType((const OUString*)0))
                throw lang::IllegalArgumentException();
            SfxItemSet& rStyleSet = rBase.GetItemSet();

            SwFmtPageDesc* pNewDesc = 0;
            const SfxPoolItem* pItem;
            if(SFX_ITEM_SET == rStyleSet.GetItemState( RES_PAGEDESC, sal_True, &pItem ) )
            {
                pNewDesc = new SwFmtPageDesc(*((SwFmtPageDesc*)pItem));
            }
            if(!pNewDesc)
                pNewDesc = new SwFmtPageDesc();
            OUString uDescName;
            rValue >>= uDescName;
            String sDescName;
            SwStyleNameMapper::FillUIName(uDescName, sDescName, nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC, sal_True );
            if(!pNewDesc->GetPageDesc() || pNewDesc->GetPageDesc()->GetName() != sDescName)
            {
                //sal_uInt16 nCount = pDoc->GetPageDescCnt();
                sal_Bool bPut = sal_False;
                if(sDescName.Len())
                {
                    SwPageDesc* pPageDesc = ::GetPageDescByName_Impl(*pDoc, sDescName);
                    if(pPageDesc)
                    {
                        pPageDesc->Add( pNewDesc );
                        bPut = sal_True;
                    }
                    else
                    {
                        throw lang::IllegalArgumentException();
                    }
                }
                if(!bPut)
                {
                    rStyleSet.ClearItem(RES_BREAK);
                    rStyleSet.Put(SwFmtPageDesc());
                }
                else
                    rStyleSet.Put(*pNewDesc);
            }
            delete pNewDesc;
        }
        break;
        case FN_UNO_IS_AUTO_UPDATE:
        {
            BOOL bAuto = *(sal_Bool*)rValue.getValue();
            if(SFX_STYLE_FAMILY_PARA == eFamily)
                rBase.pNewBase->GetCollection()->SetAutoUpdateFmt(bAuto);
            else if(SFX_STYLE_FAMILY_FRAME == eFamily)
                rBase.pNewBase->GetFrmFmt()->SetAutoUpdateFmt(bAuto);
        }
        break;
        case FN_UNO_PARA_STYLE_CONDITIONS:
        {
            uno::Sequence< beans::NamedValue > aSeq;
            if (!(rValue >>= aSeq))
                throw lang::IllegalArgumentException();

            DBG_ASSERT(COND_COMMAND_COUNT == 28,
                    "invalid size of comman count?");
            const beans::NamedValue *pSeq = aSeq.getConstArray();
            sal_Int32 nLen = aSeq.getLength();

            sal_Bool bFailed = sal_False;
            SwCondCollItem aCondItem;
            for(USHORT i = 0; i < nLen; i++)
            {
                OUString aTmp;
                if ((pSeq[i].Value >>= aTmp))
                {
                    // get UI style name from programmatic style name
                    String aStyleName;
                       SwStyleNameMapper::FillUIName( aTmp, aStyleName, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True );

                    //
                    // check for correct context and style name
                    //
                    sal_Int16 nIdx = GetCommandContextIndex( pSeq[i].Name );
                    //
                    pBasePool->SetSearchMask( SFX_STYLE_FAMILY_PARA, SFXSTYLEBIT_ALL );
                    sal_Bool bStyleFound = sal_False;
                    const SfxStyleSheetBase* pBase = pBasePool->First();
                    while (pBase && !bStyleFound)
                    {
                        if(pBase->GetName() == aStyleName)
                            bStyleFound = sal_True;
                        pBase = pBasePool->Next();
                    }
                    //
                    if (nIdx == -1 || !bStyleFound)
                    {
                        bFailed = sal_True;
                        break;
                    }

                    aCondItem.SetStyle( &aStyleName, nIdx);
                }
                else
                    bFailed = sal_True;
            }
            if (bFailed)
                throw lang::IllegalArgumentException();
            rBase.GetItemSet().Put( aCondItem );
        }
        break;
        case FN_UNO_CATEGORY:
        {
            if(!rBase.pNewBase->IsUserDefined())
                throw lang::IllegalArgumentException();
            short nSet = 0;
            rValue >>= nSet;

            sal_uInt16 nId;
            switch( nSet )
            {
                case style::ParagraphStyleCategory::TEXT:
                    nId = SWSTYLEBIT_TEXT;
                    break;
                case style::ParagraphStyleCategory::CHAPTER:
                    nId = SWSTYLEBIT_CHAPTER;
                    break;
                case style::ParagraphStyleCategory::LIST:
                    nId = SWSTYLEBIT_LIST;
                    break;
                case style::ParagraphStyleCategory::INDEX:
                    nId = SWSTYLEBIT_IDX;
                    break;
                case style::ParagraphStyleCategory::EXTRA:
                    nId = SWSTYLEBIT_EXTRA;
                    break;
                case style::ParagraphStyleCategory::HTML:
                    nId = SWSTYLEBIT_HTML;
                    break;
                default: throw lang::IllegalArgumentException();
            }
            rBase.pNewBase->SetMask( nId|SFXSTYLEBIT_USERDEF );
        }
        break;
        case SID_SWREGISTER_COLLECTION:
        {
            OUString sName;
            rValue >>= sName;
            SwRegisterItem aReg( sName.getLength() != 0);
            aReg.SetWhich(SID_SWREGISTER_MODE);
            rBase.GetItemSet().Put(aReg);
            String aString;
            SwStyleNameMapper::FillUIName(sName, aString, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True);

            rBase.GetItemSet().Put(SfxStringItem(SID_SWREGISTER_COLLECTION, aString ) );
        }
        break;
        case RES_TXTATR_CJK_RUBY:
            if(MID_RUBY_CHARSTYLE == pMap->nMemberId )
            {
                OUString sTmp;
                if(rValue >>= sTmp)
                {
                    SfxItemSet& rStyleSet = rBase.GetItemSet();
                    SwFmtRuby* pRuby = 0;
                    const SfxPoolItem* pItem;
                    if(SFX_ITEM_SET == rStyleSet.GetItemState( RES_TXTATR_CJK_RUBY, sal_True, &pItem ) )
                        pRuby = new SwFmtRuby(*((SwFmtRuby*)pItem));
                    if(!pRuby)
                        pRuby = new SwFmtRuby(aEmptyStr);
                    String sStyle;
                    SwStyleNameMapper::FillUIName(sTmp, sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT, sal_True );
                    pRuby->SetCharFmtName( sTmp );
                    pRuby->SetCharFmtId( 0 );
                    if(sTmp.getLength())
                    {
                        sal_uInt16 nId = SwStyleNameMapper::GetPoolIdFromUIName( sTmp, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT );
                        pRuby->SetCharFmtId(nId);
                    }
                    rStyleSet.Put(*pRuby);
                    delete pRuby;
                }
                else
                    throw lang::IllegalArgumentException();
            }
        goto put_itemset;
        //break;
        case RES_PARATR_DROP:
        {
            if( MID_DROPCAP_CHAR_STYLE_NAME == pMap->nMemberId)
            {
                if(rValue.getValueType() == ::getCppuType((const OUString*)0))
                {
                    SfxItemSet& rStyleSet = rBase.GetItemSet();

                    SwFmtDrop* pDrop = 0;
                    const SfxPoolItem* pItem;
                    if(SFX_ITEM_SET == rStyleSet.GetItemState( RES_PARATR_DROP, sal_True, &pItem ) )
                        pDrop = new SwFmtDrop(*((SwFmtDrop*)pItem));
                    if(!pDrop)
                        pDrop = new SwFmtDrop();
                    OUString uStyle;
                    rValue >>= uStyle;
                    String sStyle;
                    SwStyleNameMapper::FillUIName(uStyle, sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT, sal_True );
                    SwDocStyleSheet* pStyle =
                        (SwDocStyleSheet*)pDoc->GetDocShell()->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_CHAR);
                    if(pStyle)
                        pDrop->SetCharFmt(pStyle->GetCharFmt());
                    else
                        throw lang::IllegalArgumentException();
                    rStyleSet.Put(*pDrop);
                    delete pDrop;
                }
                else
                    throw lang::IllegalArgumentException();
                break;
            }
        }
        //no break!
        default:
put_itemset:
        {
            SfxItemSet& rStyleSet = rBase.GetItemSet();
            SfxItemSet aSet(*rStyleSet.GetPool(), pMap->nWID, pMap->nWID);
            aSet.SetParent(&rStyleSet);
            rPropSet.setPropertyValue(*pMap, rValue, aSet);
            rStyleSet.Put(aSet);
            // --> OD 2006-10-18 #i70223#
            if ( SFX_STYLE_FAMILY_PARA == eFamily &&
                 pMap->nWID == RES_PARATR_NUMRULE &&
                 rBase.pNewBase && rBase.pNewBase->GetCollection() &&
                 rBase.pNewBase->GetCollection()->GetOutlineLevel() < MAXLEVEL /* assigned to list level of outline style */)
            {
                OUString sNewNumberingRuleName;
                rValue >>= sNewNumberingRuleName;
                String sTmp( sNewNumberingRuleName );
                if ( sNewNumberingRuleName.getLength() == 0 ||
                     sTmp != pDoc->GetOutlineNumRule()->GetName() )
                {
                    // delete assignment to list level of outline style.
                    rBase.pNewBase->GetCollection()->SetOutlineLevel( NO_NUMBERING );
                }
            }
        }
    }
}
/* -----------------------------18.04.01 13:29--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXStyle::SetPropertyValues_Impl(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
            lang::WrappedTargetException, uno::RuntimeException)
{
    if ( !m_pDoc )
        throw uno::RuntimeException();
    sal_Int8 nPropSetId = PROPERTY_SET_CHAR_STYLE;
    switch(eFamily)
    {
        case SFX_STYLE_FAMILY_PARA  : nPropSetId = bIsConditional ? PROPERTY_SET_CONDITIONAL_PARA_STYLE : PROPERTY_SET_PARA_STYLE; break;
        case SFX_STYLE_FAMILY_FRAME : nPropSetId = PROPERTY_SET_FRAME_STYLE ;break;
        case SFX_STYLE_FAMILY_PAGE  : nPropSetId = PROPERTY_SET_PAGE_STYLE  ;break;
        case SFX_STYLE_FAMILY_PSEUDO: nPropSetId = PROPERTY_SET_NUM_STYLE   ;break;
        default:
            ;
    }
    SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();

    if(rPropertyNames.getLength() != rValues.getLength())
        throw lang::IllegalArgumentException();

    const OUString* pNames = rPropertyNames.getConstArray();
    const uno::Any* pValues = rValues.getConstArray();

    SwStyleBase_Impl aBaseImpl(*m_pDoc, sStyleName);
    if(pBasePool)
    {
        sal_uInt16 nSaveMask = pBasePool->GetSearchMask();
        pBasePool->SetSearchMask(eFamily);
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        pBasePool->SetSearchMask(eFamily, nSaveMask );
        DBG_ASSERT(pBase, "where is the style?" );
        if(pBase)
            aBaseImpl.pNewBase = new SwDocStyleSheet(*(SwDocStyleSheet*)pBase);
        else
            throw uno::RuntimeException();
    }

    for(sal_Int16 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
    {
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[nProp]);

        if(!pMap ||
           (!bIsConditional && pNames[nProp].equalsAsciiL(SW_PROP_NAME(UNO_NAME_PARA_STYLE_CONDITIONS))))
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
        if ( pMap->nFlags & beans::PropertyAttribute::READONLY)
            throw beans::PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
        if(aBaseImpl.pNewBase)
        {
            lcl_SetStyleProperty(pMap, rPropSet, pValues[nProp], aBaseImpl,
                                 pBasePool, m_pDoc, eFamily);
        }
        else if(bIsDescriptor)
        {
            if(!pPropImpl->SetProperty(pNames[nProp], pValues[nProp]))
                throw lang::IllegalArgumentException();
        }
        else
            throw uno::RuntimeException();
    }
    if(aBaseImpl.HasItemSet())
        aBaseImpl.pNewBase->SetItemSet(aBaseImpl.GetItemSet());
}

void SwXStyle::setPropertyValues(
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


uno::Any lcl_GetStyleProperty(const SfxItemPropertyMap* pMap,
                        SfxItemPropertySet& rPropSet,
                        SwStyleBase_Impl& rBase,
                        SfxStyleSheetBase* pBase,
                        SfxStyleFamily eFamily,
                        SwDoc *pDoc) throw(uno::RuntimeException)
{
    uno::Any aRet;
    if(FN_UNO_IS_PHYSICAL == pMap->nWID)
    {
        BOOL bPhys = pBase != 0;
        if(pBase)
        {
            bPhys = ((SwDocStyleSheet*)pBase)->IsPhysical();
            // The standard character format is not existing physically
            if( bPhys && SFX_STYLE_FAMILY_CHAR == eFamily &&
                ((SwDocStyleSheet*)pBase)->GetCharFmt() &&
                ((SwDocStyleSheet*)pBase)->GetCharFmt()->IsDefault() )
                bPhys = FALSE;
        }
        aRet.setValue(&bPhys, ::getBooleanCppuType());
    }
    else if(pBase)
    {
        if(!rBase.pNewBase)
            rBase.pNewBase = new SwDocStyleSheet( *(SwDocStyleSheet*)pBase );
        switch(pMap->nWID)
        {
            case RES_PAPER_BIN:
            {
                SfxItemSet& rSet = rBase.GetItemSet();
                aRet = rPropSet.getPropertyValue(*pMap, rSet);
                sal_Int8 nBin = 0;
                aRet >>= nBin;
                if ( nBin == -1 )
                    aRet <<= OUString ( RTL_CONSTASCII_USTRINGPARAM ( "[From printer settings]" ) );
                else
                {
                    SfxPrinter *pPrinter = pDoc->getPrinter( false );
                    OUString sTmp;
                    if (pPrinter )
                        sTmp = pPrinter->GetPaperBinName ( nBin );
                    aRet <<= sTmp;
                }
            }
            break;
            case  FN_UNO_NUM_RULES: //Sonderbehandlung fuer das SvxNumRuleItem:
            {
                const SwNumRule* pRule = rBase.pNewBase->GetNumRule();
                DBG_ASSERT(pRule, "Wo ist die NumRule?")
                uno::Reference< container::XIndexReplace >  xRules = new SwXNumberingRules(*pRule);
                aRet.setValue(&xRules, ::getCppuType((uno::Reference<container::XIndexReplace>*)0));
            }
            break;
            case FN_UNO_DEFAULT_OUTLINE_LEVEL:
            {
                DBG_ASSERT( SFX_STYLE_FAMILY_PARA == eFamily, "only paras" );
                BYTE nLevel = rBase.pNewBase->GetCollection()->GetOutlineLevel();
                if( nLevel != NO_NUMBERING )
                    aRet <<= static_cast<sal_Int8>( nLevel );
            }
            break;
            case FN_UNO_FOLLOW_STYLE:
            {
                String aString;
                SwStyleNameMapper::FillProgName(rBase.pNewBase->GetFollow(), aString, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True);
                aRet <<= OUString( aString );
            }
            break;
            case RES_PAGEDESC :
            if( MID_PAGEDESC_PAGEDESCNAME != pMap->nMemberId)
                goto query_itemset;
            {
                // Sonderbehandlung RES_PAGEDESC
                const SfxPoolItem* pItem;
                if(SFX_ITEM_SET == rBase.GetItemSet().GetItemState( RES_PAGEDESC, sal_True, &pItem ) )
                {
                    const SwPageDesc* pDesc = ((const SwFmtPageDesc*)pItem)->GetPageDesc();
                    if(pDesc)
                    {
                        String aString;
                        SwStyleNameMapper::FillProgName(pDesc->GetName(), aString,  nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC, sal_True );
                        aRet <<= OUString( aString );
                    }
                }
            }
            break;
            case FN_UNO_IS_AUTO_UPDATE:
            {
                BOOL bAuto = FALSE;
                if(SFX_STYLE_FAMILY_PARA == eFamily)
                    bAuto = rBase.pNewBase->GetCollection()->IsAutoUpdateFmt();
                else if(SFX_STYLE_FAMILY_FRAME == eFamily)
                    bAuto = rBase.pNewBase->GetFrmFmt()->IsAutoUpdateFmt();
                aRet.setValue(&bAuto, ::getBooleanCppuType());
            }
            break;
            case FN_UNO_DISPLAY_NAME:
            {
                OUString sName(rBase.pNewBase->GetName());
                aRet <<= sName;
            }
            break;
            case FN_UNO_PARA_STYLE_CONDITIONS:
            {
                DBG_ASSERT(COND_COMMAND_COUNT == 28,
                        "invalid size of comman count?");
                //SfxItemSet& rStyleSet = rBase.GetItemSet();
                uno::Sequence< beans::NamedValue > aSeq(COND_COMMAND_COUNT);
                beans::NamedValue *pSeq = aSeq.getArray();

                SwFmt *pFmt = ((SwDocStyleSheet*)pBase)->GetCollection();
                const CommandStruct *pCmds = SwCondCollItem::GetCmds();
                for (USHORT n = 0;  n < COND_COMMAND_COUNT;  ++n)
                {
                    String aStyleName;

                    const SwCollCondition* pCond = 0;
                    if( pFmt && RES_CONDTXTFMTCOLL == pFmt->Which() &&
                        0 != ( pCond = ((SwConditionTxtFmtColl*)pFmt)->
                        HasCondition( SwCollCondition( 0, pCmds[n].nCnd, pCmds[n].nSubCond ) ) )
                        && pCond->GetTxtFmtColl() )
                    {
                        // get programmatic style name from UI style name
                        aStyleName = pCond->GetTxtFmtColl()->GetName();
                        SwStyleNameMapper::FillProgName(aStyleName, aStyleName, lcl_GetSwEnumFromSfxEnum ( eFamily ), sal_True);
                    }

                    pSeq[n].Name  = GetCommandContextByIndex(n);
                    pSeq[n].Value <<= rtl::OUString( aStyleName );
                }
                aRet <<= aSeq;
            }
            break;
            case FN_UNO_CATEGORY:
            {
                sal_uInt16 nPoolId = rBase.pNewBase->GetCollection()->GetPoolFmtId();
                short nRet = -1;
                switch ( COLL_GET_RANGE_BITS & nPoolId )
                {
                    case COLL_TEXT_BITS:
                        nRet = style::ParagraphStyleCategory::TEXT;
                        break;
                    case COLL_DOC_BITS:
                        nRet = style::ParagraphStyleCategory::CHAPTER;
                        break;
                    case COLL_LISTS_BITS:
                        nRet = style::ParagraphStyleCategory::LIST;
                        break;
                    case COLL_REGISTER_BITS:
                        nRet = style::ParagraphStyleCategory::INDEX;
                        break;
                    case COLL_EXTRA_BITS:
                        nRet = style::ParagraphStyleCategory::EXTRA;
                        break;
                    case COLL_HTML_BITS:
                        nRet = style::ParagraphStyleCategory::HTML;
                        break;
                }
                aRet <<= nRet;
            }
            break;
            case SID_SWREGISTER_COLLECTION:
            {
                const SwPageDesc *pPageDesc = rBase.pNewBase->GetPageDesc();
                const SwTxtFmtColl* pCol = 0;
                String aString;
                if( pPageDesc )
                    pCol = pPageDesc->GetRegisterFmtColl();
                if( pCol )
                    SwStyleNameMapper::FillProgName(
                                pCol->GetName(), aString, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
                aRet <<= OUString ( aString );
            }
            break;
            default:
query_itemset:
            {
                SfxItemSet& rSet = rBase.GetItemSet();
                aRet = rPropSet.getPropertyValue(*pMap, rSet);
            }
        }
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/* -----------------------------19.04.01 09:26--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SAL_CALL SwXStyle::GetPropertyValues_Impl(
        const uno::Sequence< OUString > & rPropertyNames )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    if ( !m_pDoc )
        throw uno::RuntimeException();
    sal_Int8 nPropSetId = PROPERTY_SET_CHAR_STYLE;
    switch(eFamily)
    {
        case SFX_STYLE_FAMILY_PARA  : nPropSetId = bIsConditional ? PROPERTY_SET_CONDITIONAL_PARA_STYLE : PROPERTY_SET_PARA_STYLE; break;
        case SFX_STYLE_FAMILY_FRAME : nPropSetId = PROPERTY_SET_FRAME_STYLE ;break;
        case SFX_STYLE_FAMILY_PAGE  : nPropSetId = PROPERTY_SET_PAGE_STYLE  ;break;
        case SFX_STYLE_FAMILY_PSEUDO: nPropSetId = PROPERTY_SET_NUM_STYLE   ;break;
        default:
            ;
    }
    SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();

    const OUString* pNames = rPropertyNames.getConstArray();
    uno::Sequence< uno::Any > aRet(rPropertyNames.getLength());
    uno::Any* pRet = aRet.getArray();
    SwStyleBase_Impl aBase(*m_pDoc, sStyleName);
    SfxStyleSheetBase* pBase = 0;
    for(sal_Int32 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
    {
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[nProp]);
        if(!pMap ||
           (!bIsConditional && pNames[nProp].equalsAsciiL(SW_PROP_NAME(UNO_NAME_PARA_STYLE_CONDITIONS))))
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
        if(pBasePool)
        {
            if(!pBase)
            {
                sal_uInt16 nSaveMask = pBasePool->GetSearchMask();
                pBasePool->SetSearchMask(eFamily, SFXSTYLEBIT_ALL );
                pBase = pBasePool->Find(sStyleName);
                pBasePool->SetSearchMask(eFamily, nSaveMask );
            }
            pRet[nProp] = lcl_GetStyleProperty(pMap, rPropSet, aBase, pBase, eFamily, GetDoc() );
        }
        else if(bIsDescriptor)
        {
            uno::Any *pAny = 0;
            pPropImpl->GetProperty ( pNames[nProp], pAny );
            if( !pAny )
            {
                sal_Bool bExcept = sal_False;
                switch( eFamily )
                {
                    case SFX_STYLE_FAMILY_PSEUDO:
                        bExcept = sal_True;
                    break;
                    case SFX_STYLE_FAMILY_PARA:
                    case SFX_STYLE_FAMILY_PAGE:
                        pPropImpl->GetProperty ( pNames[nProp], mxStyleData, pRet[ nProp ] );
                    break;
                    case SFX_STYLE_FAMILY_CHAR:
                    case SFX_STYLE_FAMILY_FRAME :
                    {
                        if (pMap->nWID >= POOLATTR_BEGIN && pMap->nWID < RES_UNKNOWNATR_END )
                        {
                            SwFmt * pFmt;
                            if ( eFamily == SFX_STYLE_FAMILY_CHAR )
                                pFmt = m_pDoc->GetDfltCharFmt();
                            else
                                pFmt = m_pDoc->GetDfltFrmFmt();
                            const SwAttrPool * pPool = pFmt->GetAttrSet().GetPool();
                            const SfxPoolItem & rItem = pPool->GetDefaultItem ( pMap->nWID );
                            rItem.QueryValue ( pRet[nProp], pMap->nMemberId );
                        }
                        else
                            bExcept = sal_True;
                    }
                    break;

                    default:
                        ;
                }
                if (bExcept )
                {
                    uno::RuntimeException aExcept;
                    aExcept.Message = OUString ( RTL_CONSTASCII_USTRINGPARAM ( "No default value for: " ) ) + pNames[nProp];
                    throw aExcept;
                }
            }
            else
                pRet [ nProp ] = *pAny;
        }
        else
            throw uno::RuntimeException();
    }
    return aRet;
}
/* -----------------------------04.11.03 09:26--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXStyle::getPropertyValues(
    const uno::Sequence< OUString >& rPropertyNames ) throw(uno::RuntimeException)
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
/*-- 18.04.01 13:07:29---------------------------------------------------
  -----------------------------------------------------------------------*/
void SwXStyle::addPropertiesChangeListener(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{
}
/*-- 18.04.01 13:07:30---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::removePropertiesChangeListener(
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{
}
/*-- 18.04.01 13:07:30---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::firePropertiesChangeEvent(
    const uno::Sequence< OUString >& /*aPropertyNames*/,
    const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
        throw(uno::RuntimeException)
{
}
/*-- 17.12.98 08:26:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::setPropertyValue(const OUString& rPropertyName, const uno::Any& rValue)
    throw( beans::UnknownPropertyException,
        beans::PropertyVetoException,
        lang::IllegalArgumentException,
        lang::WrappedTargetException,
        uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const uno::Sequence<OUString> aProperties(&rPropertyName, 1);
    const uno::Sequence<uno::Any> aValues(&rValue, 1);
    SetPropertyValues_Impl( aProperties, aValues );
}
/*-- 17.12.98 08:26:53---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXStyle::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const uno::Sequence<OUString> aProperties(&rPropertyName, 1);
    return GetPropertyValues_Impl(aProperties).getConstArray()[0];

}
/*-- 17.12.98 08:26:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::addPropertyChangeListener(const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener > & /*xListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 17.12.98 08:26:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::removePropertyChangeListener(const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XPropertyChangeListener > & /*xListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 17.12.98 08:26:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::addVetoableChangeListener(const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener > & /*xListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 17.12.98 08:26:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::removeVetoableChangeListener(const OUString& /*rPropertyName*/,
    const uno::Reference< beans::XVetoableChangeListener > & /*xListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}

/*-- 08.03.99 10:50:26---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SwXStyle::getPropertyState(const OUString& rPropertyName)
        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< OUString > aNames(1);
    OUString* pNames = aNames.getArray();
    pNames[0] = rPropertyName;
    uno::Sequence< beans::PropertyState > aStates = getPropertyStates(aNames);
    return aStates.getConstArray()[0];
}
/*-- 08.03.99 10:50:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyState > SwXStyle::getPropertyStates(
    const uno::Sequence< OUString >& rPropertyNames)
        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< beans::PropertyState > aRet(rPropertyNames.getLength());
    beans::PropertyState* pStates = aRet.getArray();
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily );
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "where is the style?" );

        if(pBase)
        {
            const OUString* pNames = rPropertyNames.getConstArray();
            SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase );
            sal_Int8 nPropSetId = PROPERTY_SET_CHAR_STYLE;
            switch(eFamily)
            {
                case SFX_STYLE_FAMILY_PARA  : nPropSetId = bIsConditional ? PROPERTY_SET_CONDITIONAL_PARA_STYLE : PROPERTY_SET_PARA_STYLE; break;
                case SFX_STYLE_FAMILY_FRAME : nPropSetId = PROPERTY_SET_FRAME_STYLE ;break;
                case SFX_STYLE_FAMILY_PAGE  : nPropSetId = PROPERTY_SET_PAGE_STYLE;   break;
                case SFX_STYLE_FAMILY_PSEUDO: nPropSetId = PROPERTY_SET_NUM_STYLE   ;break;
                default:
                    ;
            }
            SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
            const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();

            SfxItemSet aSet = aStyle.GetItemSet();
            for(sal_Int32 i = 0; i < rPropertyNames.getLength(); i++)
            {
                const String& rPropName = pNames[i];
                pMap = SfxItemPropertyMap::GetByName(pMap, rPropName);
                if(!pMap)
                    throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropName, static_cast < cppu::OWeakObject * > ( this ) );
                if( FN_UNO_NUM_RULES ==  pMap->nWID ||
                    FN_UNO_FOLLOW_STYLE == pMap->nWID )
                {
                    pStates[i] = beans::PropertyState_DIRECT_VALUE;
                }
                else if( FN_UNO_DEFAULT_OUTLINE_LEVEL == pMap->nWID )
                {
                    pStates[i] =
                        ( aStyle.GetCollection()->GetOutlineLevel()
                          == NO_NUMBERING )
                        ? beans::PropertyState_DEFAULT_VALUE
                        : beans::PropertyState_DIRECT_VALUE;
                }
                else if(SFX_STYLE_FAMILY_PAGE == eFamily &&
                        (rPropName.EqualsAscii("Header", 0, 6)
                            || rPropName.EqualsAscii("Footer", 0, 6)))
                {
                    sal_uInt16 nResId = lcl_ConvertFNToRES(pMap->nWID);
                    BOOL bFooter = rPropName.EqualsAscii("Footer", 0, 6);
                    const SvxSetItem* pSetItem;
                    if(SFX_ITEM_SET == aSet.GetItemState(
                            bFooter ? SID_ATTR_PAGE_FOOTERSET : SID_ATTR_PAGE_HEADERSET,
                            sal_False, (const SfxPoolItem**)&pSetItem))
                    {
                        const SfxItemSet& rSet = pSetItem->GetItemSet();
                        SfxItemState eState = rSet.GetItemState(nResId, sal_False);
                        if(SFX_ITEM_SET == eState)
                            pStates[i] = beans::PropertyState_DIRECT_VALUE;
                        else
                            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                    }
                    else
                        pStates[i] = beans::PropertyState_AMBIGUOUS_VALUE;
                }
                else
                {
                    pStates[i] = rPropSet.getPropertyState(*pMap, aSet);
                    if( SFX_STYLE_FAMILY_PAGE == eFamily &&
                        SID_ATTR_PAGE_SIZE == pMap->nWID &&
                        beans::PropertyState_DIRECT_VALUE == pStates[i] )
                    {
                        const SvxSizeItem& rSize =
                            static_cast < const SvxSizeItem& >(
                                    aSet.Get(SID_ATTR_PAGE_SIZE) );
                        sal_uInt8 nMemberId = pMap->nMemberId & 0x7f;
                        if( ( LONG_MAX == rSize.GetSize().Width() &&
                              (MID_SIZE_WIDTH == nMemberId ||
                               MID_SIZE_SIZE == nMemberId ) ) ||
                            ( LONG_MAX == rSize.GetSize().Height() &&
                              MID_SIZE_HEIGHT == nMemberId ) )
                        {
                            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                        }
                    }
                }
            }
        }
        else
            throw uno::RuntimeException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 08.03.99 10:50:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXStyle::setPropertyToDefault(const OUString& rPropertyName)
        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    const uno::Sequence < OUString > aSequence ( &rPropertyName, 1 );
    setPropertiesToDefault ( aSequence );
}

void SAL_CALL SwXStyle::setPropertiesToDefault( const uno::Sequence< OUString >& aPropertyNames )
    throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwFmt *pTargetFmt = 0;

    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily);
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "Where is the style?")

        if(pBase)
        {
            SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase );
            switch(eFamily)
            {
                case SFX_STYLE_FAMILY_CHAR: pTargetFmt = aStyle.GetCharFmt(); break;
                case SFX_STYLE_FAMILY_PARA: pTargetFmt = aStyle.GetCollection(); break;
                case SFX_STYLE_FAMILY_FRAME: pTargetFmt = aStyle.GetFrmFmt(); break;
                case SFX_STYLE_FAMILY_PAGE:
                    {
                        sal_uInt16 nPgDscPos = USHRT_MAX;
                        SwPageDesc *pDesc = m_pDoc->FindPageDescByName( aStyle.GetPageDesc()->GetName(), &nPgDscPos );
                        if( pDesc )
                            pTargetFmt = &pDesc->GetMaster();
                    }
                    break;
                case SFX_STYLE_FAMILY_PSEUDO:
                    break;
                default:
                    ;
            }
        }
    }
    sal_Int8 nPropSetId = PROPERTY_SET_CHAR_STYLE;
    switch(eFamily)
    {
        case SFX_STYLE_FAMILY_PARA  : nPropSetId = bIsConditional ? PROPERTY_SET_CONDITIONAL_PARA_STYLE : PROPERTY_SET_PARA_STYLE; break;
        case SFX_STYLE_FAMILY_FRAME : nPropSetId = PROPERTY_SET_FRAME_STYLE; break;
        case SFX_STYLE_FAMILY_PAGE  : nPropSetId = PROPERTY_SET_PAGE_STYLE; break;
        case SFX_STYLE_FAMILY_PSEUDO: nPropSetId = PROPERTY_SET_NUM_STYLE; break;
        default:
            ;
    }
    SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();

    const OUString* pNames = aPropertyNames.getConstArray();

    if ( pTargetFmt )
    {
        for( sal_Int32 nProp = 0, nEnd = aPropertyNames.getLength(); nProp < nEnd; nProp++ )
        {
            pMap = SfxItemPropertyMap::GetByName( pMap, pNames[nProp] );
            if( !pMap )
                throw beans::UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is unknown: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
            if ( pMap->nWID == FN_UNO_FOLLOW_STYLE || pMap->nWID == FN_UNO_NUM_RULES )
                throw uno::RuntimeException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Cannot reset: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
            if ( pMap->nFlags & beans::PropertyAttribute::READONLY )
                throw uno::RuntimeException( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "setPropertiesToDefault: property is read-only: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );

            if( pMap->nWID == FN_UNO_DEFAULT_OUTLINE_LEVEL )
                static_cast<SwTxtFmtColl*>(pTargetFmt)->SetOutlineLevel( NO_NUMBERING );
            else
                pTargetFmt->ResetAttr ( pMap->nWID );
        }
    }
    else if ( bIsDescriptor )
    {
        for( sal_Int32 nProp = 0, nEnd = aPropertyNames.getLength(); nProp < nEnd; nProp++ )
            pPropImpl->ClearProperty ( pNames[ nProp ] );
    }
}

void SAL_CALL SwXStyle::setAllPropertiesToDefault(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(pBasePool)
    {
        pBasePool->SetSearchMask(eFamily);
        SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
        DBG_ASSERT(pBase, "where is the style, you fiend!?")

        if(pBase)
        {
            SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase );

            SwFmt *pTargetFmt = 0;
            sal_uInt16 nPgDscPos = USHRT_MAX;
            switch( eFamily )
            {
            case SFX_STYLE_FAMILY_CHAR :
                pTargetFmt = aStyle.GetCharFmt();
                break;
            case SFX_STYLE_FAMILY_PARA :
                {
                    pTargetFmt = aStyle.GetCollection();
                    // --> OD 2007-07-25 #132402# - make code robust
                    if ( aStyle.GetCollection() )
                    {
                        aStyle.GetCollection()->SetOutlineLevel( NO_NUMBERING );
                    }
                    // <--
                }
                break;
            case SFX_STYLE_FAMILY_FRAME:
                pTargetFmt = aStyle.GetFrmFmt();
                break;
            case SFX_STYLE_FAMILY_PAGE:
                {
                    SwPageDesc *pDesc = m_pDoc->FindPageDescByName( aStyle.GetPageDesc()->GetName(), &nPgDscPos );
                    if( pDesc )
                    {
                        pTargetFmt = &pDesc->GetMaster();
                        pDesc->SetUseOn ( nsUseOnPage::PD_ALL );
                    }
                }
                break;
            case SFX_STYLE_FAMILY_PSEUDO:
                break;

            default:
                ;
            }
            if( pTargetFmt )
            {
                if( USHRT_MAX != nPgDscPos )
                {
                    SwPageDesc& rPageDesc = m_pDoc->_GetPageDesc(nPgDscPos);
                    rPageDesc.ResetAllMasterAttr();

                    SvxLRSpaceItem aLR(RES_LR_SPACE);
                    sal_Int32 nSize = GetMetricVal ( CM_1) * 2;
                    aLR.SetLeft ( nSize );
                    aLR.SetLeft ( nSize );
                    SvxULSpaceItem aUL( RES_UL_SPACE );
                    aUL.SetUpper ( static_cast < sal_uInt16 > ( nSize ) );
                    aUL.SetLower ( static_cast < sal_uInt16 > ( nSize ) );
                    pTargetFmt->SetAttr ( aLR );
                    pTargetFmt->SetAttr ( aUL );

                    SwPageDesc* pStdPgDsc = m_pDoc->GetPageDescFromPool( RES_POOLPAGE_STANDARD );
                    SwFmtFrmSize aFrmSz( ATT_FIX_SIZE );
                    if( RES_POOLPAGE_STANDARD == rPageDesc.GetPoolFmtId() )
                    {
                        if( m_pDoc->getPrinter( false ) )
                        {
                            const Size aPhysSize( SvxPaperInfo::GetPaperSize(
                                        static_cast<Printer*>( m_pDoc->getPrinter( false ) )) );
                            aFrmSz.SetSize( aPhysSize );
                        }
                        else
                        {
                            aFrmSz.SetWidth( LONG_MAX );
                            aFrmSz.SetHeight( LONG_MAX );
                        }

                    }
                    else
                    {
                        aFrmSz = pStdPgDsc->GetMaster().GetFrmSize();
                    }
                    if( pStdPgDsc->GetLandscape() )
                    {
                        SwTwips nTmp = aFrmSz.GetHeight();
                        aFrmSz.SetHeight( aFrmSz.GetWidth() );
                        aFrmSz.SetWidth( nTmp );
                    }
                    pTargetFmt->SetAttr( aFrmSz );
                }
                else
                {
                    // --> OD 2007-01-25 #i73790# - method renamed
                    pTargetFmt->ResetAllFmtAttr();
                    // <--
                }

                if( USHRT_MAX != nPgDscPos )
                    m_pDoc->ChgPageDesc( nPgDscPos,
                                         const_cast<const SwDoc *>(m_pDoc)
                                         ->GetPageDesc(nPgDscPos) );
            }

        }
        else
            throw uno::RuntimeException();
    }
    else if ( bIsDescriptor )
        pPropImpl->ClearAllProperties();
    else
        throw uno::RuntimeException();
}

uno::Sequence< uno::Any > SAL_CALL SwXStyle::getPropertyDefaults( const uno::Sequence< OUString >& aPropertyNames )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nCount = aPropertyNames.getLength();
    uno::Sequence < uno::Any > aRet ( nCount );
    if ( nCount )
    {
        if( pBasePool)
        {
            pBasePool->SetSearchMask(eFamily);
            SfxStyleSheetBase* pBase = pBasePool->Find(sStyleName);
            DBG_ASSERT(pBase, "Doesn't seem to be a style!")

            if(pBase)
            {
                SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase );
                sal_Int8 nPropSetId = PROPERTY_SET_CHAR_STYLE;
                switch(eFamily)
                {
                    case SFX_STYLE_FAMILY_PARA  : nPropSetId = bIsConditional ? PROPERTY_SET_CONDITIONAL_PARA_STYLE : PROPERTY_SET_PARA_STYLE; break;
                    case SFX_STYLE_FAMILY_FRAME : nPropSetId = PROPERTY_SET_FRAME_STYLE; break;
                    case SFX_STYLE_FAMILY_PAGE  : nPropSetId = PROPERTY_SET_PAGE_STYLE; break;
                    case SFX_STYLE_FAMILY_PSEUDO: nPropSetId = PROPERTY_SET_NUM_STYLE; break;
                    default:
                        ;
                }
                SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
                const SfxItemPropertyMap* pMap = rPropSet.getPropertyMap();

                const SfxItemSet &rSet = aStyle.GetItemSet(), *pParentSet = rSet.GetParent();
                const OUString *pNames = aPropertyNames.getConstArray();
                uno::Any *pRet = aRet.getArray();
                for ( sal_Int32 i = 0 ; i < nCount; i++)
                {
                    pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i]);

                    if ( !pMap )
                        throw beans::UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( this ) );

                    if( pParentSet )
                        pRet[i] = aSwMapProvider.GetPropertySet(nPropSetId).getPropertyValue(pNames[i], *pParentSet);
                    else if( pMap->nWID != rSet.GetPool()->GetSlotId(pMap->nWID) )
                    {
                        const SfxPoolItem& rItem = rSet.GetPool()->GetDefaultItem(pMap->nWID);
                        rItem.QueryValue(pRet[i], pMap->nMemberId);
                    }
                }
            }
            else
                throw uno::RuntimeException();
        }
        else
            throw uno::RuntimeException();
    }
    return aRet;
}
/*-- 08.03.99 10:50:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXStyle::getPropertyDefault(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    const uno::Sequence < OUString > aSequence ( &rPropertyName, 1 );
    return getPropertyDefaults ( aSequence ).getConstArray()[0];
}
/* -----------------21.01.99 13:08-------------------
 *
 * --------------------------------------------------*/
void SwXStyle::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    SfxSimpleHint *pHint = PTR_CAST( SfxSimpleHint, &rHint );
    if( pHint )
    {
        if(( pHint->GetId() & SFX_HINT_DYING ) || ( pHint->GetId() & SFX_STYLESHEET_ERASED))
        {
            pBasePool = 0;
            EndListening(rBC);
        }
        else if( pHint->GetId() &(SFX_STYLESHEET_CHANGED|SFX_STYLESHEET_ERASED) )
        {
            ((SfxStyleSheetPool&)rBC).SetSearchMask(eFamily);
            SfxStyleSheetBase* pOwnBase = ((SfxStyleSheetPool&)rBC).Find(sStyleName);
            if(!pOwnBase)
            {
                EndListening(rBC);
                Invalidate();
            }
        }
    }
}
/* -----------------------------15.08.00 11:35--------------------------------

 ---------------------------------------------------------------------------*/
void SwXStyle::Invalidate()
{
    sStyleName.Erase();
    pBasePool = 0;
    m_pDoc = 0;
    mxStyleData.clear();
    mxStyleFamily.clear();
}


/******************************************************************
 * SwXPageStyle
 ******************************************************************/
/*-- 17.12.98 08:43:35---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXPageStyle::SwXPageStyle(SfxStyleSheetBasePool& rPool,
        SwDocShell* pDocSh, SfxStyleFamily eFam,
        const String& rStyleName)://, const SfxItemPropertyMap* _pMap) :
    SwXStyle(rPool, eFam, pDocSh->GetDoc(), rStyleName),//, _pMap),
    pDocShell(pDocSh)
{

}
/* -----------------23.08.99 15:52-------------------

 --------------------------------------------------*/
SwXPageStyle::SwXPageStyle(SwDocShell* pDocSh) :
    SwXStyle(pDocSh->GetDoc(), SFX_STYLE_FAMILY_PAGE),
    pDocShell(pDocSh)
{
}

/*-- 17.12.98 08:43:35---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXPageStyle::~SwXPageStyle()
{

}
/* -----------------------------18.04.01 13:50--------------------------------

 ---------------------------------------------------------------------------*/
void SAL_CALL SwXPageStyle::SetPropertyValues_Impl(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
            lang::WrappedTargetException, uno::RuntimeException)
{
    if(!GetDoc())
        throw uno::RuntimeException();
    SfxItemPropertySet& aPropSet = aSwMapProvider.GetPropertySet(PROPERTY_SET_PAGE_STYLE);

    if(rPropertyNames.getLength() != rValues.getLength())
        throw lang::IllegalArgumentException();

    const OUString* pNames = rPropertyNames.getConstArray();
    const uno::Any* pValues = rValues.getConstArray();
    const SfxItemPropertyMap*   pMap = aPropSet.getPropertyMap();
    SwStyleBase_Impl aBaseImpl(*GetDoc(), GetStyleName());
    if(GetBasePool())
    {
        sal_uInt16 nSaveMask = GetBasePool()->GetSearchMask();
        GetBasePool()->SetSearchMask(GetFamily());
        SfxStyleSheetBase* pBase = GetBasePool()->Find(GetStyleName());
        GetBasePool()->SetSearchMask(GetFamily(), nSaveMask );
        DBG_ASSERT(pBase, "where is the style?" );
        if(pBase)
            aBaseImpl.pNewBase = new SwDocStyleSheet(*(SwDocStyleSheet*)pBase);
        else
            throw uno::RuntimeException();
    }

    for(sal_Int16 nProp = 0; nProp < rPropertyNames.getLength(); nProp++)
    {
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[nProp]);
        if (!pMap)
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );
        if ( pMap->nFlags & beans::PropertyAttribute::READONLY)
            throw beans::PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );

        if(GetBasePool())
        {
            switch(pMap->nWID)
            {
                case FN_UNO_HEADER_ON:
                case FN_UNO_HEADER_BACKGROUND:
                case FN_UNO_HEADER_BOX:
                case FN_UNO_HEADER_LR_SPACE:
                case FN_UNO_HEADER_SHADOW:
                case FN_UNO_HEADER_BODY_DISTANCE:
                case FN_UNO_HEADER_IS_DYNAMIC_DISTANCE:
                case FN_UNO_HEADER_SHARE_CONTENT:
                case FN_UNO_HEADER_HEIGHT:
                case FN_UNO_HEADER_EAT_SPACING:

                case FN_UNO_FOOTER_ON:
                case FN_UNO_FOOTER_BACKGROUND:
                case FN_UNO_FOOTER_BOX:
                case FN_UNO_FOOTER_LR_SPACE:
                case FN_UNO_FOOTER_SHADOW:
                case FN_UNO_FOOTER_BODY_DISTANCE:
                case FN_UNO_FOOTER_IS_DYNAMIC_DISTANCE:
                case FN_UNO_FOOTER_SHARE_CONTENT:
                case FN_UNO_FOOTER_HEIGHT:
                case FN_UNO_FOOTER_EAT_SPACING:
                {
                    sal_Bool bSetItem = sal_False;
                    sal_Bool bFooter = sal_False;
                    sal_uInt16 nItemType = TYPE_BOOL;
                    sal_uInt16 nRes = 0;
                    switch(pMap->nWID)
                    {
                        case FN_UNO_FOOTER_ON:                  bFooter = sal_True;
                        //kein break;
                        case FN_UNO_HEADER_ON:                  nRes = SID_ATTR_PAGE_ON;
                        break;
                        case FN_UNO_FOOTER_BACKGROUND:          bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_BACKGROUND:          nRes = RES_BACKGROUND; nItemType = TYPE_BRUSH;
                        break;
                        case FN_UNO_FOOTER_BOX:                 bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_BOX:                 nRes = RES_BOX; nItemType = TYPE_BOX;
                        break;
                        case FN_UNO_FOOTER_LR_SPACE:            bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_LR_SPACE:            nRes = RES_LR_SPACE;nItemType = TYPE_LRSPACE;
                        break;
                        case FN_UNO_FOOTER_SHADOW:              bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_SHADOW:              nRes = RES_SHADOW;nItemType = TYPE_SHADOW;
                        break;
                        case FN_UNO_FOOTER_BODY_DISTANCE:       bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_BODY_DISTANCE:       nRes = RES_UL_SPACE;nItemType = TYPE_ULSPACE;
                        break;
                        case FN_UNO_FOOTER_IS_DYNAMIC_DISTANCE: bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_IS_DYNAMIC_DISTANCE: nRes = SID_ATTR_PAGE_DYNAMIC;
                        break;
                        case FN_UNO_FOOTER_SHARE_CONTENT:       bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_SHARE_CONTENT:       nRes = SID_ATTR_PAGE_SHARED;
                        break;
                        case FN_UNO_FOOTER_HEIGHT:              bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_HEIGHT:              nRes = SID_ATTR_PAGE_SIZE;nItemType = TYPE_SIZE;
                        break;
                        case FN_UNO_FOOTER_EAT_SPACING:     bFooter = sal_True;
                        // kein break;
                        case FN_UNO_HEADER_EAT_SPACING:     nRes = RES_HEADER_FOOTER_EAT_SPACING;nItemType = TYPE_SIZE;
                        break;
                    }
                    const SvxSetItem* pSetItem;
                    if(SFX_ITEM_SET == aBaseImpl.GetItemSet().GetItemState(
                            bFooter ? SID_ATTR_PAGE_FOOTERSET : SID_ATTR_PAGE_HEADERSET,
                            sal_False, (const SfxPoolItem**)&pSetItem))
                    {
                        SvxSetItem* pNewSetItem = (SvxSetItem*)pSetItem->Clone();
                        SfxItemSet& rSetSet = pNewSetItem->GetItemSet();
                        const SfxPoolItem* pItem = 0;
                        SfxPoolItem* pNewItem = 0;
                        if(!pItem && nRes != rSetSet.GetPool()->GetSlotId(nRes))
                            pItem = &rSetSet.GetPool()->GetDefaultItem(nRes);
                        if(pItem)
                        {
                            pNewItem = pItem->Clone();
                        }
                        else
                        {
                            switch(nItemType)
                            {
                                case TYPE_BOOL: pNewItem = new SfxBoolItem(nRes);       break;
                                case TYPE_SIZE: pNewItem = new SvxSizeItem(nRes);       break;
                                case TYPE_BRUSH: pNewItem = new SvxBrushItem(nRes);     break;
                                case TYPE_ULSPACE: pNewItem = new SvxULSpaceItem(nRes); break;
                                case TYPE_SHADOW : pNewItem = new SvxShadowItem(nRes);  break;
                                case TYPE_LRSPACE: pNewItem = new SvxLRSpaceItem(nRes); break;
                                case TYPE_BOX: pNewItem = new SvxBoxItem(nRes);         break;
                            }
                        }
                        bSetItem = pNewItem->PutValue(pValues[nProp], pMap->nMemberId);
                        rSetSet.Put(*pNewItem);
                        aBaseImpl.GetItemSet().Put(*pNewSetItem);
                        delete pNewItem;
                        delete pNewSetItem;
                    }
                    else if(SID_ATTR_PAGE_ON == nRes )
                    {
                        sal_Bool bVal = *(sal_Bool*)pValues[nProp].getValue();
                        if(bVal)
                        {
                            SfxItemSet aTempSet(*aBaseImpl.GetItemSet().GetPool(),
                                RES_BACKGROUND, RES_SHADOW,
                                RES_LR_SPACE, RES_UL_SPACE,
                                nRes, nRes,
                                SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
                                SID_ATTR_PAGE_DYNAMIC, SID_ATTR_PAGE_DYNAMIC,
                                SID_ATTR_PAGE_SHARED, SID_ATTR_PAGE_SHARED,
                                0 );
                            aTempSet.Put(SfxBoolItem(nRes, sal_True));
                            aTempSet.Put(SvxSizeItem(SID_ATTR_PAGE_SIZE, Size(MM50, MM50)));
                            aTempSet.Put(SvxLRSpaceItem(RES_LR_SPACE));
                            aTempSet.Put(SvxULSpaceItem(RES_UL_SPACE));
                            aTempSet.Put(SfxBoolItem(SID_ATTR_PAGE_SHARED, sal_True));
                            aTempSet.Put(SfxBoolItem(SID_ATTR_PAGE_DYNAMIC, sal_True));

                            SvxSetItem aNewSetItem( bFooter ? SID_ATTR_PAGE_FOOTERSET : SID_ATTR_PAGE_HEADERSET,
                                    aTempSet);
                            aBaseImpl.GetItemSet().Put(aNewSetItem);
                        }
                    }
                }
                break;
                case FN_PARAM_FTN_INFO :
                {
                    const SfxPoolItem& rItem = aBaseImpl.GetItemSet().Get(FN_PARAM_FTN_INFO);
                    SfxPoolItem* pNewFtnItem = rItem.Clone();
                    sal_Bool bPut = pNewFtnItem->PutValue(pValues[nProp], pMap->nMemberId);
                    aBaseImpl.GetItemSet().Put(*pNewFtnItem);
                    delete pNewFtnItem;
                    if(!bPut)
                        throw lang::IllegalArgumentException();
                }
                break;
                case  FN_UNO_HEADER       :
                case  FN_UNO_HEADER_LEFT  :
                case  FN_UNO_HEADER_RIGHT :
                case  FN_UNO_FOOTER       :
                case  FN_UNO_FOOTER_LEFT  :
                case  FN_UNO_FOOTER_RIGHT :
                    throw lang::IllegalArgumentException();
                //break;
                default:
                    lcl_SetStyleProperty(pMap, aPropSet, pValues[nProp], aBaseImpl,
                                        GetBasePool(), GetDoc(), GetFamily());
            }
        }
        else if(IsDescriptor())
        {
            if(!GetPropImpl()->SetProperty(pNames[nProp], pValues[nProp]))
                throw lang::IllegalArgumentException();
        }
        else
            throw uno::RuntimeException();
    }
    if(aBaseImpl.HasItemSet())
    {
        BOOL bDoesUndo = GetDoc()->DoesUndo();
        if( bDoesUndo )
        {
            // Fix i64460: as long as Undo of page styles with header/footer causes trouble...
            GetDoc()->DelAllUndoObj();
            GetDoc()->DoUndo( FALSE );
        }
        aBaseImpl.pNewBase->SetItemSet(aBaseImpl.GetItemSet());
        GetDoc()->DoUndo( bDoesUndo );
    }
}

void SwXPageStyle::setPropertyValues(
    const uno::Sequence< OUString >& rPropertyNames,
    const uno::Sequence< uno::Any >& rValues )
        throw(beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
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
/* -----------------------------04.11.03 13:50--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SAL_CALL SwXPageStyle::GetPropertyValues_Impl(
        const uno::Sequence< OUString >& rPropertyNames )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    if(!GetDoc())
        throw uno::RuntimeException();

    sal_Int32 nLength = rPropertyNames.getLength();
    SfxItemPropertySet& aPropSet = aSwMapProvider.GetPropertySet(PROPERTY_SET_PAGE_STYLE);
    const OUString* pNames = rPropertyNames.getConstArray();
    uno::Sequence< uno::Any > aRet ( nLength );

    uno::Any* pRet = aRet.getArray();
    const SfxItemPropertyMap*   pMap = aPropSet.getPropertyMap();
    SwStyleBase_Impl aBase(*GetDoc(), GetStyleName());
    SfxStyleSheetBase* pBase = 0;
    for(sal_Int32 nProp = 0; nProp < nLength; nProp++)
    {
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[nProp]);
        if (!pMap)
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[nProp], static_cast < cppu::OWeakObject * > ( this ) );

        if(GetBasePool())
        {
            if(!pBase)
            {
                sal_uInt16 nSaveMask = GetBasePool()->GetSearchMask();
                GetBasePool()->SetSearchMask(GetFamily(), SFXSTYLEBIT_ALL );
                pBase = GetBasePool()->Find(GetStyleName());
                GetBasePool()->SetSearchMask(GetFamily(), nSaveMask );
            }
            sal_uInt16 nRes = 0;
            sal_Bool bHeader = sal_False, bAll = sal_False, bLeft = sal_False, bRight = sal_False;
            switch(pMap->nWID)
            {
                case FN_UNO_HEADER_ON:
                case FN_UNO_HEADER_BACKGROUND:
                case FN_UNO_HEADER_BOX:
                case FN_UNO_HEADER_LR_SPACE:
                case FN_UNO_HEADER_SHADOW:
                case FN_UNO_HEADER_BODY_DISTANCE:
                case FN_UNO_HEADER_IS_DYNAMIC_DISTANCE:
                case FN_UNO_HEADER_SHARE_CONTENT:
                case FN_UNO_HEADER_HEIGHT:
                case FN_UNO_HEADER_EAT_SPACING:

                case FN_UNO_FOOTER_ON:
                case FN_UNO_FOOTER_BACKGROUND:
                case FN_UNO_FOOTER_BOX:
                case FN_UNO_FOOTER_LR_SPACE:
                case FN_UNO_FOOTER_SHADOW:
                case FN_UNO_FOOTER_BODY_DISTANCE:
                case FN_UNO_FOOTER_IS_DYNAMIC_DISTANCE:
                case FN_UNO_FOOTER_SHARE_CONTENT:
                case FN_UNO_FOOTER_HEIGHT:
                case FN_UNO_FOOTER_EAT_SPACING:
                {
                    SfxStyleSheetBasePool* pBasePool2 = ((SwXPageStyle*)this)->GetBasePool();
                    pBasePool2->SetSearchMask(GetFamily());
                    SfxStyleSheetBase* pBase2 = pBasePool2->Find(GetStyleName());
                    if(pBase2)
                    {
                        SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase2 );
                        const SfxItemSet& rSet = aStyle.GetItemSet();
                        sal_Bool bFooter = sal_False;
                        switch(pMap->nWID)
                        {
                            case FN_UNO_FOOTER_ON:
                                bFooter = sal_True;
                            // kein break!
                            case FN_UNO_HEADER_ON:
                            {
                                //falls das SetItem nicht da ist, dann ist der Wert sal_False
                                BOOL bRet = sal_False;
                                pRet[nProp].setValue(&bRet, ::getCppuBooleanType());
                                nRes = SID_ATTR_PAGE_ON;
                            }
                            break;
                            case FN_UNO_FOOTER_BACKGROUND:      bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_BACKGROUND:      nRes = RES_BACKGROUND;
                            break;
                            case FN_UNO_FOOTER_BOX:             bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_BOX:             nRes = RES_BOX;
                            break;
                            case FN_UNO_FOOTER_LR_SPACE:        bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_LR_SPACE:        nRes = RES_LR_SPACE;
                            break;
                            case FN_UNO_FOOTER_SHADOW:          bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_SHADOW:          nRes = RES_SHADOW;
                            break;
                            case FN_UNO_FOOTER_BODY_DISTANCE:   bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_BODY_DISTANCE:   nRes = RES_UL_SPACE;
                            break;
                            case FN_UNO_FOOTER_IS_DYNAMIC_DISTANCE: bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_IS_DYNAMIC_DISTANCE: nRes = SID_ATTR_PAGE_DYNAMIC;
                            break;
                            case FN_UNO_FOOTER_SHARE_CONTENT:   bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_SHARE_CONTENT:   nRes = SID_ATTR_PAGE_SHARED;
                            break;
                            case FN_UNO_FOOTER_HEIGHT:          bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_HEIGHT:          nRes = SID_ATTR_PAGE_SIZE;
                            break;
                            case FN_UNO_FOOTER_EAT_SPACING: bFooter = sal_True;
                            // kein break;
                            case FN_UNO_HEADER_EAT_SPACING: nRes = RES_HEADER_FOOTER_EAT_SPACING;
                            break;
                        }
                        const SvxSetItem* pSetItem;
                        if(SFX_ITEM_SET == rSet.GetItemState(
                                bFooter ? SID_ATTR_PAGE_FOOTERSET : SID_ATTR_PAGE_HEADERSET,
                                sal_False, (const SfxPoolItem**)&pSetItem))
                        {
                            const SfxItemSet& rTmpSet = pSetItem->GetItemSet();
                            const SfxPoolItem* pItem = 0;
                            rTmpSet.GetItemState(nRes, sal_True, &pItem);
                            if(!pItem && nRes != rTmpSet.GetPool()->GetSlotId(nRes))
                                pItem = &rTmpSet.GetPool()->GetDefaultItem(nRes);
                            if(pItem)
                                pItem->QueryValue(pRet[nProp], pMap->nMemberId);
                        }
                    }
                }
                break;
                case  FN_UNO_HEADER       :
                    bAll = sal_True; goto Header;
                case  FN_UNO_HEADER_LEFT  :
                    bLeft = sal_True; goto Header;
                case  FN_UNO_HEADER_RIGHT :
                    bRight = sal_True; goto Header;
Header:
                    bHeader = sal_True;
                    nRes = RES_HEADER; goto MakeObject;
                case  FN_UNO_FOOTER       :
                    bAll = sal_True; goto Footer;
                case  FN_UNO_FOOTER_LEFT  :
                    bLeft = sal_True; goto Footer;
                case  FN_UNO_FOOTER_RIGHT :
                    bRight = sal_True;
Footer:
                    nRes = RES_FOOTER;
MakeObject:
                {
                    const SwPageDesc& rDesc = aBase.GetOldPageDesc();
                    const SwFrmFmt* pFrmFmt = 0;
                    sal_Bool bShare = bHeader && rDesc.IsHeaderShared()||
                                    !bHeader && rDesc.IsFooterShared();
                    // TextLeft returns the left content if there is one,
                    // Text and TextRight return the master content.
                    // TextRight does the same as Text and is for
                    // comptability only.
                    if( bLeft && !bShare )
                        pFrmFmt = &rDesc.GetLeft();
                    else
                        pFrmFmt = &rDesc.GetMaster();
                    if(pFrmFmt)
                    {
                        const SfxItemSet& rSet = pFrmFmt->GetAttrSet();
                        const SfxPoolItem* pItem;
                        SwFrmFmt* pHeadFootFmt;
                        if(SFX_ITEM_SET == rSet.GetItemState(nRes, sal_True, &pItem) &&
                        0 != (pHeadFootFmt = bHeader ?
                                    ((SwFmtHeader*)pItem)->GetHeaderFmt() :
                                        ((SwFmtFooter*)pItem)->GetFooterFmt()))
                        {
                            // gibt es schon ein Objekt dafuer?
                            SwXHeadFootText* pxHdFt = (SwXHeadFootText*)SwClientIter( *pHeadFootFmt ).
                                            First( TYPE( SwXHeadFootText ));
                            uno::Reference< text::XText >  xRet = pxHdFt;
                            if(!pxHdFt)
                                xRet = new SwXHeadFootText(*pHeadFootFmt, bHeader);
                            pRet[nProp].setValue(&xRet, ::getCppuType((uno::Reference<text::XText>*)0));
                        }
                    }
                }
                break;
                case FN_PARAM_FTN_INFO :
                {
                    SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pBase );
                    const SfxItemSet& rSet = aStyle.GetItemSet();
                    const SfxPoolItem& rItem = rSet.Get(FN_PARAM_FTN_INFO);
                    rItem.QueryValue(pRet[nProp], pMap->nMemberId);
                }
                break;
                default:
                pRet[nProp] = lcl_GetStyleProperty(pMap, aPropSet, aBase, pBase, GetFamily(), GetDoc() );
            }
        }
        else if(IsDescriptor())
        {
            uno::Any* pAny = 0;
            GetPropImpl()->GetProperty(pNames[nProp], pAny);
            if ( !pAny )
                GetPropImpl()->GetProperty ( pNames[nProp], mxStyleData, pRet[ nProp ] );
            else
                pRet[nProp] = *pAny;
        }
        else
            throw uno::RuntimeException();
    }
    return aRet;
}
/* -----------------------------18.04.01 13:50--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXPageStyle::getPropertyValues(
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
/*-- 17.12.98 08:43:36---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXPageStyle::getPropertyValue(const OUString& rPropertyName) throw(
    beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const uno::Sequence<OUString> aProperties(&rPropertyName, 1);
    return GetPropertyValues_Impl(aProperties).getConstArray()[0];
}
/*-- 17.12.98 08:43:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXPageStyle::setPropertyValue(const OUString& rPropertyName, const uno::Any& rValue)
    throw( beans::UnknownPropertyException,
        beans::PropertyVetoException,
        lang::IllegalArgumentException,
        lang::WrappedTargetException,
        uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const uno::Sequence<OUString> aProperties(&rPropertyName, 1);
    const uno::Sequence<uno::Any> aValues(&rValue, 1);
    SetPropertyValues_Impl( aProperties, aValues );
}

SwXFrameStyle::SwXFrameStyle ( SwDoc *pDoc )
: SwXStyle ( pDoc, SFX_STYLE_FAMILY_FRAME, FALSE)
{
}
/* -----------------------------15.12.00 15:45--------------------------------

 ---------------------------------------------------------------------------*/
SwXFrameStyle::~SwXFrameStyle()
{
}
/* -----------------------------15.12.00 14:30--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SwXFrameStyle::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aTypes = SwXStyle::getTypes();
    sal_Int32 nLen = aTypes.getLength();
    aTypes.realloc(nLen + 1);
    aTypes.getArray()[nLen] = ::getCppuType((uno::Reference<XEventsSupplier>*)0);
    return aTypes;
}
/* -----------------------------15.12.00 14:30--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXFrameStyle::queryInterface( const uno::Type& rType ) throw(uno::RuntimeException)
{
    uno::Any aRet;
    if(rType == ::getCppuType((uno::Reference<XEventsSupplier>*)0))
        aRet <<= uno::Reference<XEventsSupplier>(this);
    else
        aRet = SwXStyle::queryInterface(rType);
    return aRet;
}
/* -----------------------------15.12.00 14:30--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< container::XNameReplace > SwXFrameStyle::getEvents(  ) throw(uno::RuntimeException)
{
    return new SwFrameStyleEventDescriptor( *this );
}
/*-- 19.05.2006 11:23:55---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyles::SwXAutoStyles(SwDocShell& rDocShell) :
    SwUnoCollection(rDocShell.GetDoc()), pDocShell( &rDocShell )
{
}
/*-- 19.05.2006 11:23:56---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyles::~SwXAutoStyles()
{
}
/*-- 19.05.2006 11:23:57---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXAutoStyles::getCount(void) throw( uno::RuntimeException )
{
    return AUTOSTYLE_FAMILY_COUNT;
}
/*-- 19.05.2006 11:23:57---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoStyles::getByIndex(sal_Int32 nIndex)
        throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException,
                uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(nIndex < 0 || nIndex >= AUTOSTYLE_FAMILY_COUNT)
        throw lang::IndexOutOfBoundsException();
    if(IsValid())
    {
        uno::Reference< style::XAutoStyleFamily >  aRef;
        IStyleAccess::SwAutoStyleFamily nType = aAutoStyleByIndex[nIndex];
        switch( nType )
        {
            case IStyleAccess::AUTO_STYLE_CHAR:
            {
                if(!xAutoCharStyles.is())
                    xAutoCharStyles = new SwXAutoStyleFamily(pDocShell, nType);
                aRef = xAutoCharStyles;
            }
            break;
            case IStyleAccess::AUTO_STYLE_RUBY:
            {
                if(!xAutoRubyStyles.is())
                    xAutoRubyStyles = new SwXAutoStyleFamily(pDocShell, nType );
                aRef = xAutoRubyStyles;
            }
            break;
            case IStyleAccess::AUTO_STYLE_PARA:
            {
                if(!xAutoParaStyles.is())
                    xAutoParaStyles = new SwXAutoStyleFamily(pDocShell, nType );
                aRef = xAutoParaStyles;
            }
            break;

            default:
                ;
        }
        aRet.setValue(&aRef, ::getCppuType((const uno::Reference<style::XAutoStyleFamily>*)0));
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 19.05.2006 11:23:57---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXAutoStyles::getElementType(  ) throw(uno::RuntimeException)
{
    return ::getCppuType((const uno::Reference<style::XAutoStyleFamily>*)0);
}
/*-- 19.05.2006 11:23:58---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoStyles::hasElements(  ) throw(uno::RuntimeException)
{
    return sal_True;
}
/*-- 19.05.2006 11:23:58---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoStyles::getByName(const rtl::OUString& Name)
        throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    uno::Any aRet;
    if(Name.compareToAscii("CharacterStyles") == 0 )
        aRet = getByIndex(0);
    else if(Name.compareToAscii("RubyStyles") == 0 )
        aRet = getByIndex(1);
    else if(Name.compareToAscii("ParagraphStyles") == 0 )
        aRet = getByIndex(2);
    else
        throw container::NoSuchElementException();
    return aRet;
}
/*-- 19.05.2006 11:23:59---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< rtl::OUString > SwXAutoStyles::getElementNames(void)
            throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aNames(AUTOSTYLE_FAMILY_COUNT);
    OUString* pNames = aNames.getArray();
    pNames[0] = C2U("CharacterStyles");
    pNames[1] = C2U("RubyStyles");
    pNames[2] = C2U("ParagraphStyles");
    return aNames;
}
/*-- 19.05.2006 11:24:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoStyles::hasByName(const rtl::OUString& Name)
            throw( uno::RuntimeException )
{
    if( Name.compareToAscii("CharacterStyles") == 0 ||
        Name.compareToAscii("RubyStyles") == 0 ||
        Name.compareToAscii("ParagraphStyles") == 0 )
        return sal_True;
    else
        return sal_False;
}

/*-- 19.05.2006 11:24:02---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyleFamily::SwXAutoStyleFamily(SwDocShell* pDocSh, IStyleAccess::SwAutoStyleFamily nFamily) :
    pDocShell( pDocSh ), eFamily(nFamily)
{
    // Register ourselves as a listener to the document (via the page descriptor)
    pDocSh->GetDoc()->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
}
/*-- 19.05.2006 11:24:02---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyleFamily::~SwXAutoStyleFamily()
{
}

void SwXAutoStyleFamily::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        pDocShell = 0;
}

/*-- 31.05.2006 11:24:02---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< style::XAutoStyle > SwXAutoStyleFamily::insertStyle(
    const uno::Sequence< beans::PropertyValue >& Values )
        throw (uno::RuntimeException)
{
    if( !pDocShell )
        throw uno::RuntimeException();
    const USHORT* pRange = 0;
    const SfxItemPropertyMap* pMap = 0;
    switch( eFamily )
    {
        case IStyleAccess::AUTO_STYLE_CHAR:
        {
            pRange = aCharAutoFmtSetRange;
            pMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_CHAR_AUTO_STYLE);
        }
        break;
        case IStyleAccess::AUTO_STYLE_RUBY:
        {
            pRange = 0;//aTxtNodeSetRange;
            pMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_RUBY_AUTO_STYLE);
        }
        break;
        case IStyleAccess::AUTO_STYLE_PARA:
        {
            pRange = aTxtNodeSetRange;
            pMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARA_AUTO_STYLE);
        }
        break;

        default:
            ;
    }
    SwAttrSet aSet( pDocShell->GetDoc()->GetAttrPool(), pRange );
    SfxItemPropertySet aPropSet( pMap );
    const beans::PropertyValue* pSeq = Values.getConstArray();
    sal_Int32 nLen = Values.getLength();
    for( sal_Int32 i = 0; i < nLen; ++i )
    {
        try
        {
            aPropSet.setPropertyValue( pSeq[i].Name, pSeq[i].Value, aSet );
        }
        catch (beans::UnknownPropertyException &)
        {
            ASSERT( false, "Unknown property" );
        }
        catch (lang::IllegalArgumentException &)
        {
            ASSERT( false, "Illegal argument" );
        }
    }

    SfxItemSet_Pointer_t pSet = pDocShell->GetDoc()->GetIStyleAccess().cacheAutomaticStyle( aSet, eFamily );
    uno::Reference<style::XAutoStyle> xRet = new SwXAutoStyle(pDocShell->GetDoc(), pSet, eFamily);
    return xRet;
}
/*-- 31.05.2006 11:24:02---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration > SwXAutoStyleFamily::createEnumeration(  )
        throw (uno::RuntimeException)
{
    if( !pDocShell )
        throw uno::RuntimeException();
    return uno::Reference< container::XEnumeration >
        (new SwXAutoStylesEnumerator( pDocShell->GetDoc(), eFamily ));
}
/*-- 19.05.2006 11:24:03---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXAutoStyleFamily::getElementType(  ) throw(uno::RuntimeException)
{
    return ::getCppuType((const uno::Reference<style::XAutoStyle>*)0);
}
/*-- 19.05.2006 11:24:04---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoStyleFamily::hasElements(  ) throw(uno::RuntimeException)
{
    return sal_False;
}

/*-- 31.05.2006 11:24:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAutoStylesEnumImpl::SwAutoStylesEnumImpl( SwDoc* pInitDoc, IStyleAccess::SwAutoStyleFamily eFam )
: pDoc( pInitDoc ), eFamily( eFam )
{
    // special case for ruby auto styles:
    if ( IStyleAccess::AUTO_STYLE_RUBY == eFam )
    {
        std::set< std::pair< USHORT, USHORT > > aRubyMap;
        SwAttrPool& rAttrPool = pDoc->GetAttrPool();
        USHORT nCount = rAttrPool.GetItemCount( RES_TXTATR_CJK_RUBY );

        for ( USHORT nI = 0; nI < nCount; ++nI )
        {
            const SwFmtRuby* pItem = static_cast<const SwFmtRuby*>(rAttrPool.GetItem( RES_TXTATR_CJK_RUBY, nI ));
            if ( pItem && pItem->GetTxtRuby() )
            {
                std::pair< USHORT, USHORT > aPair( pItem->GetPosition(), pItem->GetAdjustment() );
                if ( aRubyMap.find( aPair ) == aRubyMap.end() )
                {
                    aRubyMap.insert( aPair );
                    SfxItemSet_Pointer_t pItemSet( new SfxItemSet( rAttrPool, RES_TXTATR_CJK_RUBY, RES_TXTATR_CJK_RUBY ) );
                    pItemSet->Put( *pItem );
                    mAutoStyles.push_back( pItemSet );
                }
            }
        }
    }
    else
    {
        pDoc->GetIStyleAccess().getAllStyles( mAutoStyles, eFamily );
    }

    aIter = mAutoStyles.begin();
}

/*-- 31.05.2006 11:24:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStylesEnumerator::SwXAutoStylesEnumerator( SwDoc* pDoc, IStyleAccess::SwAutoStyleFamily eFam )
: pImpl( new SwAutoStylesEnumImpl( pDoc, eFam ) )
{
    // Register ourselves as a listener to the document (via the page descriptor)
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
}
/*-- 31.05.2006 11:24:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStylesEnumerator::~SwXAutoStylesEnumerator()
{
    delete pImpl;
}

void SwXAutoStylesEnumerator::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
    {
        delete pImpl;
        pImpl = 0;
    }
}


/*-- 31.05.2006 11:24:05---------------------------------------------------

  -----------------------------------------------------------------------*/
::sal_Bool SwXAutoStylesEnumerator::hasMoreElements(  )
    throw (uno::RuntimeException)
{
    if( !pImpl )
        throw uno::RuntimeException();
    return pImpl->hasMoreElements();
}
/*-- 31.05.2006 11:24:05---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoStylesEnumerator::nextElement(  )
    throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
{
    if( !pImpl )
        throw uno::RuntimeException();
    uno::Any aRet;
    if( pImpl->hasMoreElements() )
    {
        SfxItemSet_Pointer_t pNextSet = pImpl->nextElement();
        uno::Reference< style::XAutoStyle > xAutoStyle = new SwXAutoStyle(pImpl->getDoc(),
                                                        pNextSet, pImpl->getFamily());
        aRet.setValue(&xAutoStyle, ::getCppuType((uno::Reference<style::XAutoStyle>*)0));
    }
    return aRet;
}
/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyle::SwXAutoStyle( SwDoc* pDoc, SfxItemSet_Pointer_t pInitSet, IStyleAccess::SwAutoStyleFamily eFam )
: pSet( pInitSet ), eFamily( eFam )
{
    // Register ourselves as a listener to the document (via the page descriptor)
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoStyle::~SwXAutoStyle()
{
}

void SwXAutoStyle::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        pSet.reset();
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SwXAutoStyle::getPropertySetInfo(  )
                throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySetInfo >  xRet;
    switch( eFamily )
    {
        case IStyleAccess::AUTO_STYLE_CHAR:
        {
            static uno::Reference< beans::XPropertySetInfo >  xCharRef;
            if(!xCharRef.is())
            {
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(PROPERTY_MAP_CHAR_AUTO_STYLE));
                xCharRef = aPropSet.getPropertySetInfo();
            }
            xRet = xCharRef;
        }
        break;
        case IStyleAccess::AUTO_STYLE_RUBY:
        {
            static uno::Reference< beans::XPropertySetInfo >  xRubyRef;
            if(!xRubyRef.is())
            {
                sal_uInt16 nMapId = PROPERTY_MAP_RUBY_AUTO_STYLE;
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(nMapId));
                xRubyRef = aPropSet.getPropertySetInfo();
            }
            xRet = xRubyRef;
        }
        break;
        case IStyleAccess::AUTO_STYLE_PARA:
        {
            static uno::Reference< beans::XPropertySetInfo >  xParaRef;
            if(!xParaRef.is())
            {
                sal_uInt16 nMapId = PROPERTY_MAP_PARA_AUTO_STYLE;
                SfxItemPropertySet aPropSet(
                    aSwMapProvider.GetPropertyMap(nMapId));
                xParaRef = aPropSet.getPropertySetInfo();
            }
            xRet = xParaRef;
        }
        break;

        default:
            ;
    }

    return xRet;
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::setPropertyValue( const OUString& /*rPropertyName*/, const uno::Any& /*rValue*/ )
     throw( beans::UnknownPropertyException,
            beans::PropertyVetoException,
            lang::IllegalArgumentException,
            lang::WrappedTargetException,
            uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoStyle::getPropertyValue( const OUString& rPropertyName )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const uno::Sequence<OUString> aProperties(&rPropertyName, 1);
    return GetPropertyValues_Impl(aProperties).getConstArray()[0];
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::addPropertyChangeListener( const OUString& /*aPropertyName*/,
                                              const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException )
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::removePropertyChangeListener( const OUString& /*aPropertyName*/,
                                                 const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException )
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::addVetoableChangeListener( const OUString& /*PropertyName*/,
                                              const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException )
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::removeVetoableChangeListener( const OUString& /*PropertyName*/,
                                                 const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException )
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::setPropertyValues(
        const uno::Sequence< ::rtl::OUString >& /*aPropertyNames*/,
        const uno::Sequence< uno::Any >& /*aValues*/ )
            throw (beans::PropertyVetoException, lang::IllegalArgumentException,
                lang::WrappedTargetException, uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXAutoStyle::GetPropertyValues_Impl(
        const uno::Sequence< OUString > & rPropertyNames )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    if( !pSet.get() )
        throw uno::RuntimeException();
    // query_item

    sal_Int8 nPropSetId = PROPERTY_SET_CHAR_AUTO_STYLE;
    switch(eFamily)
    {
        case IStyleAccess::AUTO_STYLE_CHAR  : nPropSetId = PROPERTY_SET_CHAR_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_RUBY  : nPropSetId = PROPERTY_SET_RUBY_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_PARA  : nPropSetId = PROPERTY_SET_PARA_AUTO_STYLE;  break;
        default:
            ;
    }

    SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();
    const OUString* pNames = rPropertyNames.getConstArray();

    sal_Int32 nLen = rPropertyNames.getLength();
    uno::Sequence< uno::Any > aRet( nLen );
    uno::Any* pValues = aRet.getArray();

    SfxItemSet& rSet = *pSet.get();

    for( sal_Int32 i = 0; i < nLen; ++i )
    {
        const String& rPropName = pNames[i];
        pMap = SfxItemPropertyMap::GetByName(pMap, rPropName);
        if(!pMap)
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropName, static_cast < cppu::OWeakObject * > ( this ) );
        else if ( RES_TXTATR_AUTOFMT == pMap->nWID || RES_AUTO_STYLE == pMap->nWID )
        {
            OUString sName(StylePool::nameOf( pSet ));
            pValues[i] <<= sName;
        }
        else
            pValues[i] = rPropSet.getPropertyValue( *pMap, rSet );
    }
    return aRet;
}

/*-- 19.05.2006 11:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXAutoStyle::getPropertyValues (
        const uno::Sequence< ::rtl::OUString >& rPropertyNames )
            throw (uno::RuntimeException)
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

/*-- 19.05.2006 11:24:10---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::addPropertiesChangeListener(
        const uno::Sequence< ::rtl::OUString >& /*aPropertyNames*/,
        const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
            throw (uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:10---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::removePropertiesChangeListener(
        const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
            throw (uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:11---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::firePropertiesChangeEvent(
        const uno::Sequence< ::rtl::OUString >& /*aPropertyNames*/,
        const uno::Reference< beans::XPropertiesChangeListener >& /*xListener*/ )
            throw (uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:11---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SwXAutoStyle::getPropertyState( const OUString& rPropertyName )
    throw( beans::UnknownPropertyException,
           uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< OUString > aNames(1);
    OUString* pNames = aNames.getArray();
    pNames[0] = rPropertyName;
    uno::Sequence< beans::PropertyState > aStates = getPropertyStates(aNames);
    return aStates.getConstArray()[0];
}

/*-- 19.05.2006 11:24:11---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::setPropertyToDefault( const OUString& /*PropertyName*/ )
    throw( beans::UnknownPropertyException,
           uno::RuntimeException )
{
}

/*-- 19.05.2006 11:24:11---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoStyle::getPropertyDefault( const OUString& rPropertyName )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           uno::RuntimeException)
{
    const uno::Sequence < OUString > aSequence ( &rPropertyName, 1 );
    return getPropertyDefaults ( aSequence ).getConstArray()[0];
}

/*-- 19.05.2006 11:24:12---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyState > SwXAutoStyle::getPropertyStates(
        const uno::Sequence< ::rtl::OUString >& rPropertyNames )
            throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    if( !pSet.get() )
        throw uno::RuntimeException();
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< beans::PropertyState > aRet(rPropertyNames.getLength());
    beans::PropertyState* pStates = aRet.getArray();
    const OUString* pNames = rPropertyNames.getConstArray();

    sal_Int8 nPropSetId = PROPERTY_SET_CHAR_AUTO_STYLE;
    switch(eFamily)
    {
        case IStyleAccess::AUTO_STYLE_CHAR  : nPropSetId = PROPERTY_SET_CHAR_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_RUBY  : nPropSetId = PROPERTY_SET_RUBY_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_PARA  : nPropSetId = PROPERTY_SET_PARA_AUTO_STYLE;  break;
        default:
            ;
    }

    SfxItemPropertySet &rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap *pMap = rPropSet.getPropertyMap();
    SfxItemSet& rSet = *pSet.get();
    for(sal_Int32 i = 0; i < rPropertyNames.getLength(); i++)
    {
        const String& rPropName = pNames[i];
        pMap = SfxItemPropertyMap::GetByName(pMap, rPropName);
        if(!pMap)
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropName, static_cast < cppu::OWeakObject * > ( this ) );
        pStates[i] = rPropSet.getPropertyState(*pMap, rSet);
    }
    return aRet;
}

/*-- 19.05.2006 11:24:12---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::setAllPropertiesToDefault(  )
            throw (uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:13---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoStyle::setPropertiesToDefault(
        const uno::Sequence< ::rtl::OUString >& /*aPropertyNames*/ )
            throw (beans::UnknownPropertyException, uno::RuntimeException)
{
}

/*-- 19.05.2006 11:24:14---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Any > SwXAutoStyle::getPropertyDefaults(
        const uno::Sequence< ::rtl::OUString >& /*aPropertyNames*/ )
            throw (beans::UnknownPropertyException, lang::WrappedTargetException,
                    uno::RuntimeException)
{
    uno::Sequence< uno::Any > aRet(0);
    return aRet;
}

/*-- 19.05.2006 11:24:14---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXAutoStyle::getProperties() throw (uno::RuntimeException)
{
    if( !pSet.get() )
        throw uno::RuntimeException();
    vos::OGuard aGuard(Application::GetSolarMutex());
    std::vector< beans::PropertyValue > aPropertyVector;

    sal_Int8 nPropSetId = 0;
    switch(eFamily)
    {
        case IStyleAccess::AUTO_STYLE_CHAR  : nPropSetId = PROPERTY_SET_CHAR_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_RUBY  : nPropSetId = PROPERTY_SET_RUBY_AUTO_STYLE;  break;
        case IStyleAccess::AUTO_STYLE_PARA  : nPropSetId = PROPERTY_SET_PARA_AUTO_STYLE;  break;
        default:
            ;
    }

    SfxItemPropertySet& rPropSet = aSwMapProvider.GetPropertySet(nPropSetId);
    const SfxItemPropertyMap* pMap = rPropSet.getPropertyMap();

    SfxItemSet& rSet = *pSet.get();
    SfxItemIter aIter(rSet);
    const SfxPoolItem* pItem = aIter.FirstItem();

    while ( pItem )
    {
        const USHORT nWID = pItem->Which();

        // TODO: Optimize
        int i = 0;
        while ( pMap[i].nWID != 0 )
        {
            if ( pMap[i].nWID == nWID )
            {
                beans::PropertyValue aPropertyValue;
                String sString( OUString::createFromAscii( pMap[i].pName ) );
                aPropertyValue.Name = sString;
                pItem->QueryValue( aPropertyValue.Value, pMap[i].nMemberId );
                aPropertyVector.push_back( aPropertyValue );
                break;
            }
            ++i;
        }
        pItem = aIter.NextItem();
    }

    const sal_Int32 nCount = aPropertyVector.size();
    uno::Sequence< beans::PropertyValue > aRet( nCount );
    beans::PropertyValue* pProps = aRet.getArray();

    for ( int i = 0; i < nCount; ++i, pProps++ )
    {
        *pProps = aPropertyVector[i];
    }

    return aRet;
}
