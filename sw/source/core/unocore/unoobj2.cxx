/*************************************************************************
 *
 *  $RCSfile: unoobj2.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: os $ $Date: 2000-12-01 17:37:27 $
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


#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#include <swtypes.hxx>
#include <hintids.hxx>
#include <cmdid.h>
#ifndef _HINTS_HXX //autogen
#include <hints.hxx>
#endif
#ifndef _BOOKMRK_HXX //autogen
#include <bookmrk.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _NDTXT_HXX //autogen
#include <ndtxt.hxx>
#endif
#ifndef _NDNOTXT_HXX //autogen
#include <ndnotxt.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _SWUNDO_HXX //autogen
#include <swundo.hxx>
#endif
#ifndef _ROOTFRM_HXX //autogen
#include <rootfrm.hxx>
#endif
#ifndef _FLYFRM_HXX //autogen
#include <flyfrm.hxx>
#endif
#ifndef _FTNIDX_HXX //autogen
#include <ftnidx.hxx>
#endif
#ifndef _LINKMGR_HXX
#include <so3/linkmgr.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#include <pam.hxx>
#ifndef _CACHESTR_HXX //autogen
#include <tools/cachestr.hxx>
#endif
#ifndef _SHELLIO_HXX //autogen
#include <shellio.hxx>
#endif
#include <swerror.h>
#ifndef _SWTBLFMT_HXX //autogen
#include <swtblfmt.hxx>
#endif
#ifndef _FMTHBSH_HXX
#include <fmthbsh.hxx>
#endif
#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif
#ifndef _DOCSTYLE_HXX //autogen
#include <docstyle.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _PAGEDESC_HXX //autogen
#include <pagedesc.hxx>
#endif
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _EDIMP_HXX //autogen
#include <edimp.hxx>
#endif
#ifndef _FCHRFMT_HXX //autogen
#include <fchrfmt.hxx>
#endif
#ifndef _CNTFRM_HXX //autogen
#include <cntfrm.hxx>
#endif
#ifndef _PAGEFRM_HXX //autogen
#include <pagefrm.hxx>
#endif
#ifndef _DOCTXM_HXX
#include <doctxm.hxx>
#endif
#ifndef _TOX_HXX
#include <tox.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX //autogen
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _FMTRFMRK_HXX //autogen
#include <fmtrfmrk.hxx>
#endif
#ifndef _TXTRFMRK_HXX //autogen
#include <txtrfmrk.hxx>
#endif
#ifndef _UNOCLBCK_HXX
#include <unoclbck.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef SW_UNOMID_HXX
#include <unomid.h>
#endif
#ifndef _UNOSETT_HXX
#include <unosett.hxx>
#endif
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif
#ifndef _UNOTBL_HXX
#include <unotbl.hxx>
#endif
#ifndef _UNODRAW_HXX
#include <unodraw.hxx>
#endif
#ifndef _UNOCOLL_HXX
#include <unocoll.hxx>
#endif
#ifndef _UNOSTYLE_HXX
#include <unostyle.hxx>
#endif
#ifndef _UNOFIELD_HXX
#include <unofield.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _SVX_FLSTITEM_HXX //autogen
#include <svx/flstitem.hxx>
#endif
#ifndef _CTRLTOOL_HXX //autogen
#include <svtools/ctrltool.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _FLYPOS_HXX
#include <flypos.hxx>
#endif
#ifndef _TXTFTN_HXX //autogen
#include <txtftn.hxx>
#endif
#ifndef _SECTION_HXX //autogen
#include <section.hxx>
#endif
#ifndef _FMTFTN_HXX //autogen
#include <fmtftn.hxx>
#endif

#ifndef _COM_SUN_STAR_TEXT_WRAPTEXTMODE_HPP_
#include <com/sun/star/text/WrapTextMode.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP_
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_PAGESTYLELAYOUT_HPP_
#include <com/sun/star/style/PageStyleLayout.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _UNOIDX_HXX
#include <unoidx.hxx>
#endif
#ifndef _UNOFRAME_HXX
#include <unoframe.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_ //autogen
#include <osl/mutex.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _FMTFLCNT_HXX //autogen
#include <fmtflcnt.hxx>
#endif
#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svtools/svstdarr.hxx>
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif

//TODO: new Interface & new uno::Exception for protected content
#define EXCEPT_ON_PROTECTION(rUnoCrsr)	\
    if((rUnoCrsr).HasReadonlySel()) \
        throw uno::RuntimeException();

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;

using namespace ::rtl;

//collectn.cxx
BOOL lcl_IsNumeric(const String&);

String lcl_GetCurPageStyle(SwPaM& rPaM);

sal_Bool lcl_getCrsrPropertyValue(const SfxItemPropertyMap* pMap
                                        , SwPaM& rPam
                                        , const SfxItemSet& rSet
                                        , uno::Any& rAny
                                        , beans::PropertyState& eState );
sal_Bool lcl_IsStartOfPara(SwPaM& rUnoCrsr);

sal_Bool lcl_IsEndOfPara(SwPaM& rUnoCrsr);

void	lcl_resetCrsrPropertyValue(const SfxItemPropertyMap* pMap, SwPaM& rPam);
void lcl_InsertFile(SwUnoCrsr* pUnoCrsr,
    const String& rFileName,
    const String& rFilterName,
    const String& rFilterOption,
    const String& rPassword);

uno::Any  lcl_getNumberingProperty(SwPaM& rPam, beans::PropertyState& eState);

/*-- 09.12.98 14:18:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::insertDocumentFromURL(const OUString& rURL,
    const uno::Sequence< beans::PropertyValue >& aOptions)
    throw( lang::IllegalArgumentException, io::IOException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        String sFilterName;
        String sFilterOption;
        String sPassword;
        OUString uFilterName(C2U(	UNO_NAME_FILTER_NAME	));
        OUString uFilterOption(C2U(	UNO_NAME_FILTER_OPTION	));
        OUString uPassword(C2U( UNO_NAME_PASSWORD ));
        sal_Bool bIllegalArgument = sal_False;

        for ( int n = 0; n < aOptions.getLength(); ++n )
        {
            // get Property-Value from options
            const beans::PropertyValue &rProp = aOptions.getConstArray()[n];
            uno::Any aValue( rProp.Value );

            // FilterName-Property?
            if ( rProp.Name == uFilterName )
            {
                if ( rProp.Value.getValueType() == ::getCppuType((const OUString*)0))
                {
                    OUString uFilterName;
                    rProp.Value >>= uFilterName;
                    sFilterName = String(uFilterName);
                }
                else if ( rProp.Value.getValueType() != ::getVoidCppuType() )
                    bIllegalArgument = sal_True;
            }
            else if( rProp.Name == uFilterOption )
            {
                if ( rProp.Value.getValueType() == ::getCppuType((const OUString*)0))
                {
                    OUString uFilterOption;
                    rProp.Value >>= uFilterOption;
                    sFilterOption = String(uFilterOption) ;
                }
                else if ( rProp.Value.getValueType() != ::getVoidCppuType() )

                    bIllegalArgument = sal_True;
            }
            else if( rProp.Name == uPassword )
            {
                if ( rProp.Value.getValueType() == ::getCppuType((const OUString*)0))
                {
                    OUString uPassword;
                    rProp.Value >>= uPassword;
                    sPassword = String(uPassword );
                }
                else if ( rProp.Value.getValueType() != ::getVoidCppuType() )
                    bIllegalArgument = sal_True;
            }
            else if(rProp.Value.getValueType() != ::getVoidCppuType())
                bIllegalArgument = sal_True;
        }
        if(bIllegalArgument)
            throw lang::IllegalArgumentException();
        lcl_InsertFile(pUnoCrsr, rURL, sFilterName, sFilterOption, sPassword);
    }
    else
        throw uno::RuntimeException();
}
/*-- 09.12.98 14:18:58---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXTextCursor::createSortDescriptor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence< beans::PropertyValue > aRet(12);
    beans::PropertyValue* pArray = aRet.getArray();

    uno::Any aVal;
    sal_Bool bFalse = sal_False;
    sal_Bool bTrue = sal_True;
    aVal.setValue( &bFalse, ::getCppuBooleanType());
    pArray[0] = beans::PropertyValue(C2U("IsSortInTable"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    String sSpace(String::CreateFromAscii(" "));
    sal_Unicode uSpace = sSpace.GetChar(0);
    aVal.setValue( &uSpace, ::getCppuCharType());
    pArray[1] = beans::PropertyValue(C2U("Delimiter"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[2] = beans::PropertyValue(C2U("SortColumns"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal <<= (INT16)1;
    pArray[3] = beans::PropertyValue(C2U("SortRowOrColumnNo0"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[4] = beans::PropertyValue(C2U("IsSortNumeric0"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[5] = beans::PropertyValue(C2U("IsSortAscending0"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal <<= (INT16)1;
    pArray[6] = beans::PropertyValue(C2U("SortRowOrColumnNo1"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[7] = beans::PropertyValue(C2U("IsSortNumeric1"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[8] = beans::PropertyValue(C2U("IsSortAscending1"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal <<= (INT16)1;
    pArray[9] = beans::PropertyValue(C2U("SortRowOrColumnNo2"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[10] = beans::PropertyValue(C2U("IsSortNumeric2"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal.setValue( &bTrue, ::getCppuBooleanType());
    pArray[11] = beans::PropertyValue(C2U("IsSortAscending2"), -1, aVal, beans::PropertyState_DIRECT_VALUE);
    return aRet;
}
/*-- 09.12.98 14:19:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::sort(const uno::Sequence< beans::PropertyValue >& rDescriptor)
        throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    const beans::PropertyValue* pProperties = rDescriptor.getConstArray();
    SwSortOptions aSortOpt;

    aSortOpt.bTable = sal_False;
    aSortOpt.nDeli = ' ';
    aSortOpt.eDirection = SRT_COLUMNS;

    aSortOpt.aKeys;
    SwSortKey* pKey1 = new SwSortKey;
    pKey1->nColumnId = USHRT_MAX;
    pKey1->eSortKeyType = SRT_NUMERIC;
    pKey1->eSortOrder   = SRT_ASCENDING;

    SwSortKey* pKey2 = new SwSortKey;
    pKey2->nColumnId = USHRT_MAX;
    pKey2->eSortKeyType = SRT_NUMERIC;
    pKey2->eSortOrder   = SRT_ASCENDING;

    SwSortKey* pKey3 = new SwSortKey;
    pKey3->nColumnId = USHRT_MAX;
    pKey3->eSortKeyType = SRT_NUMERIC;
    pKey3->eSortOrder   = SRT_ASCENDING;
    SwSortKey* aKeys[3] = {pKey1, pKey2, pKey3};

    for( int n = 0; n < rDescriptor.getLength(); ++n )
    {
        uno::Any aValue( pProperties[n].Value );
//		String sPropName = pProperties[n].Name;
        const OUString& rPropName = pProperties[n].Name;
        if( COMPARE_EQUAL == rPropName.compareToAscii("IsSortInTable"))
        {
            if ( aValue.getValueType() == ::getBooleanCppuType() )
            {
                aValue >>= aSortOpt.bTable;
            }
            else
                throw uno::RuntimeException();
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("Delimiter"))
        {
            if ( aValue.getValueType() == ::getCppuType((sal_Unicode*)0))
            {
                sal_Unicode uChar;
                aValue >>= uChar;
                aSortOpt.nDeli = uChar;
            }
            else
                throw uno::RuntimeException();
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("SortColumns"))
        {
            if ( aValue.getValueType() == ::getBooleanCppuType() )
            {
                sal_Bool bTemp;
                aValue >>= bTemp;
                aSortOpt.eDirection = bTemp ? SRT_COLUMNS : SRT_ROWS;
            }
            else
                throw uno::RuntimeException();
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("SortRowOrColumnNo", 17) &&
            rPropName.getLength() == 18 &&
            (rPropName.getStr()[17] >= '0' && rPropName.getStr()[17] <= '9'))
        {
            sal_uInt16 nIndex = rPropName.getStr()[17];
            nIndex -= '0';
            sal_Int16 nCol = -1;
            if( aValue.getValueType() == ::getCppuType((const sal_Int16*)0) && nIndex < 3)
                aValue >>= nCol;
            if(	nCol >= 0 )
            {
                aKeys[nIndex]->nColumnId = nCol;
            }
            else
                throw uno::RuntimeException();
        }
        else if(0 == rPropName.search(C2U("IsSortNumeric")) &&
            rPropName.getLength() == 14 &&
            (rPropName.getStr()[13] >= '0' && rPropName.getStr()[13] <= '9'))
        {
            sal_uInt16 nIndex = rPropName.getStr()[13];
            nIndex = nIndex - '0';
            if ( aValue.getValueType() == ::getBooleanCppuType() && nIndex < 3 )
            {
                sal_Bool bTemp;
                aValue >>= bTemp;
                aKeys[nIndex]->eSortKeyType = bTemp ? SRT_NUMERIC : SRT_APLHANUM;
            }
            else
                throw uno::RuntimeException();
        }
        else if(0 == rPropName.search(C2U("IsSortAscending")) && rPropName.getLength() == 16 &&
            lcl_IsNumeric(String(rPropName[(sal_uInt16)15])))
        {
            sal_uInt16 nIndex = rPropName.getStr()[13];
            if ( aValue.getValueType() == ::getBooleanCppuType() && nIndex < 3 )
            {
                sal_Bool bTemp;
                aValue >>= bTemp;
                aKeys[nIndex]->eSortOrder = bTemp ? SRT_ASCENDING : SRT_DESCENDING;
            }
            else
                throw uno::RuntimeException();
        }
    }
    if(pKey1->nColumnId != USHRT_MAX)
        aSortOpt.aKeys.C40_INSERT(SwSortKey, pKey1, aSortOpt.aKeys.Count());
    if(pKey2->nColumnId != USHRT_MAX)
        aSortOpt.aKeys.C40_INSERT(SwSortKey, pKey2, aSortOpt.aKeys.Count());
    if(pKey3->nColumnId != USHRT_MAX)
        aSortOpt.aKeys.C40_INSERT(SwSortKey, pKey3, aSortOpt.aKeys.Count());

    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr && aSortOpt.aKeys.Count())
    {
        if(pUnoCrsr->HasMark())
        {
            UnoActionContext aContext( pUnoCrsr->GetDoc() );
            pUnoCrsr->GetDoc()->SortText(*pUnoCrsr, aSortOpt);
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 10.12.98 11:52:15---------------------------------------------------

  -----------------------------------------------------------------------*/
void ClientModify(SwClient* pClient, SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    switch( pOld ? pOld->Which() : 0 )
    {
    case RES_REMOVE_UNO_OBJECT:
    case RES_OBJECTDYING:
        if( (void*)pClient->GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            ((SwModify*)pClient->GetRegisteredIn())->Remove(pClient);
        break;

    case RES_FMT_CHG:
        // wurden wir an das neue umgehaengt und wird das alte geloscht?
        if( ((SwFmtChg*)pNew)->pChangedFmt == pClient->GetRegisteredIn() &&
            ((SwFmtChg*)pOld)->pChangedFmt->IsFmtInDTOR() )
            ((SwModify*)pClient->GetRegisteredIn())->Remove(pClient);
        break;
    }
}

/* -----------------------------03.04.00 09:11--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XEnumeration >  SAL_CALL SwXTextCursor::createContentEnumeration(const OUString& rServiceName) throw( RuntimeException )
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( !pUnoCrsr || 0 != rServiceName.compareToAscii("com.sun.star.text.TextContent") )
        throw RuntimeException();

    Reference< XEnumeration > xRet = new SwXParaFrameEnumeration(*pUnoCrsr, PARAFRAME_PORTION_TEXTRANGE);
    return xRet;
}
/* -----------------------------03.04.00 09:11--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SAL_CALL SwXTextCursor::getAvailableServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = OUString::createFromAscii("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 09.12.98 14:19:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextCursor::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        aLstnrCntnr.Disposing();

}
/*-- 09.12.98 14:19:01---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwPaM* 	SwXTextCursor::GetPaM() const
{
    return GetCrsr() ? GetCrsr() : 0;
}
/*-- 09.12.98 14:19:02---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwDoc* SwXTextCursor::GetDoc()const
{
    return	 GetCrsr() ? GetCrsr()->GetDoc() : 0;
}
/* -----------------22.07.99 13:52-------------------

 --------------------------------------------------*/
SwDoc* SwXTextCursor::GetDoc()
{
    return	 GetCrsr() ? GetCrsr()->GetDoc() : 0;
}

/*-- 09.12.98 14:19:03---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::SetCrsrAttr(SwPaM& rPam, const SfxItemSet& rSet, sal_Bool bTableMode)
{
    sal_uInt16 nFlags = 0;//???
    SwDoc* pDoc = rPam.GetDoc();
    //StartEndAction
    UnoActionContext aAction(pDoc);
    SwPaM* pCrsr = &rPam;
    if( pCrsr->GetNext() != pCrsr ) 	// Ring von Cursorn
    {
        pDoc->StartUndo(UNDO_INSATTR);

        SwPaM *_pStartCrsr = &rPam;
        do
        {
            if( _pStartCrsr->HasMark() && ( bTableMode ||
                *_pStartCrsr->GetPoint() != *_pStartCrsr->GetMark() ))
                pDoc->Insert(*_pStartCrsr, rSet, nFlags );
        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != &rPam );

        pDoc->EndUndo(UNDO_INSATTR);
    }
    else
    {
//			if( !HasSelection() )
//				UpdateAttr();
        pDoc->Insert( *pCrsr, rSet, nFlags );
    }
}
/*-- 09.12.98 14:19:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::GetCrsrAttr(SwPaM& rPam, SfxItemSet& rSet, BOOL bCurrentAttrOnly)
{
static const sal_uInt16 nMaxLookup = 255;
    SfxItemSet aSet( *rSet.GetPool(), rSet.GetRanges() );
    SfxItemSet *pSet = &rSet;
    SwPaM *_pStartCrsr = &rPam;
    do
    {
        sal_uInt32 nSttNd = _pStartCrsr->GetMark()->nNode.GetIndex(),
                    nEndNd = _pStartCrsr->GetPoint()->nNode.GetIndex();
            xub_StrLen nSttCnt = _pStartCrsr->GetMark()->nContent.GetIndex(),
                    nEndCnt = _pStartCrsr->GetPoint()->nContent.GetIndex();

            if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
            {
                sal_uInt32 nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
                nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (sal_uInt16)nTmp;
            }

            if( nEndNd - nSttNd >= nMaxLookup )
            {
                rSet.ClearItem();
                rSet.InvalidateAllItems();
                return;// uno::Any();
            }

            // beim 1.Node traegt der Node die Werte in den GetSet ein (Initial)
            // alle weiteren Nodes werden zum GetSet zu gemergt
            for( sal_uInt32 n = nSttNd; n <= nEndNd; ++n )
            {
                SwNode* pNd = rPam.GetDoc()->GetNodes()[ n ];
                switch( pNd->GetNodeType() )
                {
                case ND_TEXTNODE:
                    {
                        xub_StrLen nStt = n == nSttNd ? nSttCnt : 0,
                                nEnd = n == nEndNd ? nEndCnt
                                            : ((SwTxtNode*)pNd)->GetTxt().Len();
                        ((SwTxtNode*)pNd)->GetAttr( *pSet, nStt, nEnd, bCurrentAttrOnly, !bCurrentAttrOnly );
                    }
                    break;
                case ND_GRFNODE:
                case ND_OLENODE:
                    ((SwCntntNode*)pNd)->GetAttr( *pSet );
                    break;

                default:
                    pNd = 0;
                }

                if( pNd )
                {
                    if( pSet != &rSet )
                        rSet.MergeValues( aSet );

                    if( aSet.Count() )
                        aSet.ClearItem();

                }
                pSet = &aSet;
            }
    } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != &rPam );
}
/******************************************************************
 * SwXParagraphEnumeration
 ******************************************************************/
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXParagraphEnumeration::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXParagraphEnumeration");
}
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParagraphEnumeration::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.ParagraphEnumeration") == rServiceName;
}
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXParagraphEnumeration::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.ParagraphEnumeration");
    return aRet;
}
/*-- 10.12.98 11:52:12---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraphEnumeration::SwXParagraphEnumeration(SwXText* pParent,
                                                    SwPosition& rPos,
                                                    CursorType eType) :
        xParentText(pParent),
        bFirstParagraph(sal_True),
        eCursorType(eType)
{
    SwUnoCrsr* pUnoCrsr = pParent->GetDoc()->CreateUnoCrsr(rPos, sal_False);
    pUnoCrsr->Add(this);
}

/*-- 10.12.98 11:52:12---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraphEnumeration::SwXParagraphEnumeration(SwXText* pParent,
                                                SwUnoCrsr*	pCrsr,
                                                CursorType eType) :
        SwClient(pCrsr),
        xParentText(pParent),
        bFirstParagraph(sal_True),
        eCursorType(eType)
{
}
/*-- 10.12.98 11:52:12---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraphEnumeration::~SwXParagraphEnumeration()
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        delete pUnoCrsr;

}
/*-- 10.12.98 11:52:13---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXParagraphEnumeration::hasMoreElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(bFirstParagraph)
            bRet = sal_True;
        else
        {
            SwPosition* pStart = pUnoCrsr->GetPoint();
            SwUnoCrsr* pNewCrsr = pUnoCrsr->GetDoc()->CreateUnoCrsr(*pStart, sal_False);
            //man soll hier auch in Tabellen landen duerfen
            if(CURSOR_TBLTEXT != eCursorType)
                pNewCrsr->SetRemainInSection( sal_False );

            //was mache ich, wenn ich schon in einer Tabelle stehe?
            SwTableNode* pTblNode = pNewCrsr->GetNode()->FindTableNode();
            if(CURSOR_TBLTEXT != eCursorType && pTblNode)
            {
                pNewCrsr->GetPoint()->nNode = pTblNode->EndOfSectionIndex();
                bRet = pNewCrsr->Move(fnMoveForward, fnGoNode);
            }
            else
                bRet = pNewCrsr->MovePara(fnParaNext, fnParaStart);
            delete pNewCrsr;
        }
    }
    return bRet;
}
/*-- 10.12.98 11:52:14---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParagraphEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextContent >  aRef;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
         XText* pText = xParentText.get();
        sal_Bool bInTable = sal_False;
        if(!bFirstParagraph)
        {
            //man soll hier auch in Tabellen landen duerfen
            if(CURSOR_TBLTEXT != eCursorType)
            {
                pUnoCrsr->SetRemainInSection( sal_False );
                //was mache ich, wenn ich schon in einer Tabelle stehe?
                SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
                if(pTblNode)
                {
                    // wir haben es mit einer Tabelle zu tun - also ans Ende
                    pUnoCrsr->GetPoint()->nNode = pTblNode->EndOfSectionIndex();
                    if(!pUnoCrsr->Move(fnMoveForward, fnGoNode))
                    {
                        throw container::NoSuchElementException();
                    }
                    else
                        bInTable = sal_True;

                }
            }
        }

        if(	bFirstParagraph || bInTable || pUnoCrsr->MovePara(fnParaNext, fnParaStart))
        {
            bFirstParagraph = sal_False;
            SwPosition* pStart = pUnoCrsr->GetPoint();
            //steht man nun in einer Tabelle, oder in einem einfachen Absatz?

            SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
            if(CURSOR_TBLTEXT != eCursorType && pTblNode)
            {
                // wir haben es mit einer Tabelle zu tun
                SwFrmFmt* pTableFmt = (SwFrmFmt*)pTblNode->GetTable().GetFrmFmt();
             XTextTable* pTable = SwXTextTables::GetObject( *pTableFmt );
                aRef =  (XTextContent*)(SwXTextTable*)pTable;
            }
            else
            {
                SwUnoCrsr* pNewCrsr = pUnoCrsr->GetDoc()->CreateUnoCrsr(*pStart, sal_False);
                aRef =  (XTextContent*)new SwXParagraph((SwXText*)pText, pNewCrsr);
            }
        }
        else
            throw container::NoSuchElementException();

    }
    else
        throw uno::RuntimeException();

    uno::Any aRet(&aRef, ::getCppuType((uno::Reference<XTextContent>*)0));
    return aRet;
}
void SwXParagraphEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}

/******************************************************************
 * SwXTextRange
 ******************************************************************/
TYPEINIT1(SwXTextRange, SwClient);

/* -----------------10.12.98 13:19-------------------
 *
 * --------------------------------------------------*/
/*uno::Reference< uno::XInterface >  SwXTextRange_NewInstance_Impl()
{
    return *new SwXTextRange();
};
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextRange::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
    //XUnoTunnel
sal_Int64 SAL_CALL SwXTextRange::getSomething(
    const uno::Sequence< sal_Int8 >& rId )
        throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
        {
                return (sal_Int64)this;
        }
    return 0;
}
/* -----------------------------06.04.00 16:34--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextRange::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextRange");
}
/* -----------------------------06.04.00 16:34--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextRange::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    String sServiceName(rServiceName);
    return sServiceName.EqualsAscii("com.sun.star.text.TextRange") ||
         sServiceName.EqualsAscii("com.sun.star.style.CharacterProperties")||
         sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties");
}
/* -----------------------------06.04.00 16:34--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextRange::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextRange");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
     pArray[2] = C2U("com.sun.star.style.ParagraphProperties");
    return aRet;
}
/*-- 10.12.98 12:54:42---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange() :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    pDoc(0),
    aObjectDepend(this, 0),
    pBox(0),
    pBoxStartNode(0),
    eRangePosition(RANGE_INVALID)
{
}
/*-- 10.12.98 12:54:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwPaM& rPam, const uno::Reference< XText > & rxParent) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    xParentText(rxParent),
    aObjectDepend(this, 0),
    pDoc(rPam.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    eRangePosition(RANGE_IN_TEXT)
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/*-- 10.12.98 12:54:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rFmt, SwPaM& rPam) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    aObjectDepend(this, &rFmt),
    pDoc(rPam.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    eRangePosition(RANGE_IN_FRAME)
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt, SwTableBox& rTblBox, SwPaM& rPam) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    pDoc(rPam.GetDoc()),
    aObjectDepend(this, &rTblFmt),
    pBox(&rTblBox),
    pBoxStartNode(0),
    eRangePosition(RANGE_IN_CELL)
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/* -----------------------------09.08.00 16:07--------------------------------

 ---------------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt, const SwStartNode& rStartNode, SwPaM& rPam) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    pDoc(rPam.GetDoc()),
    aObjectDepend(this, &rTblFmt),
    pBox(0),
    pBoxStartNode(&rStartNode),
    eRangePosition(RANGE_IN_CELL)
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/* -----------------19.02.99 11:39-------------------
 *
 * --------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    aObjectDepend(this, &rTblFmt),
    pDoc(rTblFmt.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    eRangePosition(RANGE_IS_TABLE)
{
}

/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::~SwXTextRange()
{
    if(GetBookmark())
        pDoc->DelBookmark( GetBookmark()->GetName() );
}
/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwXTextRange::_CreateNewBookmark(SwPaM& rPam)
{
    SwBookmark* pBkm = GetBookmark();
    if(pBkm)
        pDoc->DelBookmark( pBkm->GetName() );
    KeyCode aCode;
    String sBookmarkName(C2S("SwXTextPosition"));
    String sShortName;
    rPam.GetDoc()->MakeUniqueBookmarkName(sBookmarkName);

    SwBookmark* pMark = pDoc->MakeBookmark(rPam, aCode,
                sBookmarkName, sShortName, UNO_BOOKMARK);
    pMark->Add(this);
}
/*-- 10.12.98 12:54:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextRange::DeleteAndInsert(const String& rText) throw( uno::RuntimeException )
{
    SwBookmark* pBkm = GetBookmark();
    if(pBkm )
    {
        const SwPosition& rPoint = pBkm->GetPos();
        const SwPosition* pMark = pBkm->GetOtherPos();
        SwCursor aNewCrsr( rPoint);
        SwDoc* pDoc = aNewCrsr.GetDoc();
        if(pMark)
        {
            aNewCrsr.SetMark();
            *aNewCrsr.GetMark() = *pMark;
        }
        EXCEPT_ON_PROTECTION(aNewCrsr)

        UnoActionContext aAction(aNewCrsr.GetDoc());
        pDoc->StartUndo(UNDO_INSERT);
        if(aNewCrsr.HasMark())
            pDoc->DeleteAndJoin(aNewCrsr);

        if(rText.Len())
        {
            if( !pDoc->Insert(aNewCrsr, rText) )
                ASSERT( sal_False, "Doc->Insert(Str) failed." );
            SwXTextCursor::SelectPam(aNewCrsr, sal_True);
            aNewCrsr.Left(rText.Len());
        }
        _CreateNewBookmark(aNewCrsr);
        pDoc->EndUndo(UNDO_INSERT);
    }

}

/*-- 10.12.98 12:54:46---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XText >  SwXTextRange::getText(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!xParentText.is())
    {
        if(eRangePosition == RANGE_IN_FRAME &&
            aObjectDepend.GetRegisteredIn())
        {
            SwFrmFmt* pFrmFmt = (SwFrmFmt*)aObjectDepend.GetRegisteredIn();
            SwXTextFrame* pxFrm = (SwXTextFrame*)SwClientIter( *pFrmFmt ).
                                                First( TYPE( SwXTextFrame ));
            if(pxFrm)
                ((SwXTextRange*)this)->xParentText = pxFrm;
            else
                ((SwXTextRange*)this)->xParentText = new SwXTextFrame(*pFrmFmt);
            ((SwModify*)aObjectDepend.GetRegisteredIn())->
                    Remove(&((SwXTextRange*)this)->aObjectDepend);

        }
        else if(eRangePosition == RANGE_IN_CELL &&
            aObjectDepend.GetRegisteredIn())
        {
            const SwStartNode* pSttNd = pBoxStartNode ? pBoxStartNode : pBox->GetSttNd();
            const SwTableNode* pTblNode = pSttNd->FindTableNode();
            const SwFrmFmt* pTableFmt = pTblNode->GetTable().GetFrmFmt();
            ((SwXTextRange*)this)->xParentText =
                    pBox ? SwXCell::CreateXCell((SwFrmFmt*)pTableFmt, pBox)
                         : new SwXCell( (SwFrmFmt*)pTableFmt, *pBoxStartNode );
            ((SwModify*)aObjectDepend.GetRegisteredIn())->
                    Remove(&((SwXTextRange*)this)->aObjectDepend);
        }
        else if(eRangePosition == RANGE_IS_TABLE &&
            aObjectDepend.GetRegisteredIn() )
        {
            SwFrmFmt* pTblFmt = (SwFrmFmt*)aObjectDepend.GetRegisteredIn();
            SwDoc* pDoc = pTblFmt->GetDoc();
            SwTable* pTable = SwTable::FindTable( pTblFmt );
            SwTableNode* pTblNode = pTable->GetTableNode();
            SwPosition aPosition( *pTblNode );
            Reference< XTextRange >  xRange = CreateTextRangeFromPosition(pDoc,
                        aPosition, 0);
            xParentText = xRange->getText();
        }
        else
        {
            // jetzt noch alle Faelle finden, die nicht abgedeckt sind
            // (Body, Kopf-/Fusszeilen, Fussnotentext )
            DBG_WARNING("not implemented")
        }
    }
    return xParentText;
}
/*-- 10.12.98 12:54:47---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXTextRange::getStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwBookmark* pBkm = GetBookmark();
    if(!xParentText.is())
        getText();
    if(pBkm)
    {
        SwPaM aPam(pBkm->GetPos());
        xRet = new SwXTextRange(aPam, xParentText);
    }
    else if(eRangePosition == RANGE_IS_TABLE)
    {
        //start und ende sind mit this identisch, wenn es eine Tabelle ist
        xRet = this;
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 10.12.98 12:54:47---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXTextRange::getEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwBookmark* pBkm = GetBookmark();
    if(!xParentText.is())
        getText();
    if(pBkm)
    {
        SwPaM aPam(pBkm->GetOtherPos()? *pBkm->GetOtherPos() : pBkm->GetPos());
        xRet = new SwXTextRange(aPam, xParentText);
    }
    else if(eRangePosition == RANGE_IS_TABLE)
    {
        //start und ende sind mit this identisch, wenn es eine Tabelle ist
        xRet = this;
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 10.12.98 12:54:47---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXTextRange::getString(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwBookmark* pBkm = GetBookmark();
    OUString sRet;
    //fuer Tabellen gibt es keine Bookmark, also auch keinen Text
    //evtl. koennte man hier die Tabelle als ASCII exportieren?
    if(pBkm && pBkm->GetOtherPos())
    {
        const SwPosition& rPoint = pBkm->GetPos();
        const SwPosition* pMark = pBkm->GetOtherPos();
        SwPaM aCrsr(*pMark, rPoint);
/*		if( rPoint.nNode.GetIndex() ==
            pMark->nNode.GetIndex() )
        {
            SwTxtNode* pTxtNd = aCrsr.GetNode()->GetTxtNode();
            if( pTxtNd )
            {
                sal_uInt16 nStt = aCrsr.Start()->nContent.GetIndex();
                sRet = pTxtNd->GetExpandTxt( nStt,
                        aCrsr.End()->nContent.GetIndex() - nStt );
            }
        }
        else
*/		{
            SwXTextCursor::getTextFromPam(aCrsr, sRet);
        }
    }
    return sRet;
}
/*-- 10.12.98 12:54:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextRange::setString(const OUString& aString)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(RANGE_IN_CELL == eRangePosition)
    {
        //setString in Tabellen kann nicht erlaubt werden
        throw uno::RuntimeException();
    }
    else
        DeleteAndInsert(aString);
}
/*-- 10.12.98 12:54:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextRange::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    sal_Bool bAlreadyRegisterred = 0 != GetRegisteredIn();
    ClientModify(this, pOld, pNew);
    if(aObjectDepend.GetRegisteredIn())
    {
        ClientModify(&aObjectDepend, pOld, pNew);
        // if the depend was removed then the range must be removed too
        if(!aObjectDepend.GetRegisteredIn() && GetRegisteredIn())
            ((SwModify*)GetRegisteredIn())->Remove(this);
        // or if the range has been removed but the depend ist still
        // connected then the depend must be removed
        else if(bAlreadyRegisterred && !GetRegisteredIn() &&
                aObjectDepend.GetRegisteredIn())
            ((SwModify*)aObjectDepend.GetRegisteredIn())->Remove(&aObjectDepend);
    }
}
/*-- 10.12.98 12:54:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool 	SwXTextRange::GetPositions(SwPaM& rToFill) const
{
    sal_Bool bRet = sal_False;
    SwBookmark* pBkm = GetBookmark();
    if(pBkm)
    {
        *rToFill.GetPoint() = pBkm->GetPos();
        if(pBkm->GetOtherPos())
        {
            rToFill.SetMark();
            *rToFill.GetMark() = *pBkm->GetOtherPos();
        }
        else
            rToFill.DeleteMark();
        bRet = sal_True;
    }
    return bRet;
}
/*-- 10.12.98 12:54:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool 		SwXTextRange::XTextRangeToSwPaM( SwUnoInternalPaM& rToFill,
                            const uno::Reference< XTextRange > & xTextRange)
{
    sal_Bool bRet = sal_False;

    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    SwXTextCursor* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                SwXTextRange::getUnoTunnelId());
        pCursor = (SwXTextCursor*)xRangeTunnel->getSomething(
                                SwXTextCursor::getUnoTunnelId());
    }

    if(pRange && pRange->GetDoc() == rToFill.GetDoc())
    {
        bRet = pRange->GetPositions(rToFill);
    }
    else
    {
        const SwPaM* pPam = 0;
        if(pCursor && pCursor->GetDoc() == rToFill.GetDoc() &&
            0 != (pPam = pCursor->GetPaM()))
            {
                DBG_ASSERT((SwPaM*)pPam->GetNext() == pPam, "was machen wir mit Ringen?" )
                bRet = sal_True;
                *rToFill.GetPoint() = *pPam->GetPoint();
                if(pPam->HasMark())
                {
                    rToFill.SetMark();
                    *rToFill.GetMark() = *pPam->GetMark();
                }
                else
                    rToFill.DeleteMark();
            }
    }
    return bRet;
}
/* -----------------------------03.04.00 09:11--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XEnumeration >  SAL_CALL SwXTextRange::createContentEnumeration(
        const OUString& rServiceName)
                throw( RuntimeException )
{
    SwBookmark* pBkm = GetBookmark();
    if( !pBkm || COMPARE_EQUAL != rServiceName.compareToAscii("com.sun.star.text.TextContent") )
        throw RuntimeException();

    const SwPosition& rPoint = pBkm->GetPos();
    const SwPosition* pMark = pBkm->GetOtherPos();
    SwUnoCrsr* pNewCrsr = pDoc->CreateUnoCrsr(rPoint, FALSE);
    if(pMark && *pMark != rPoint)
    {
        pNewCrsr->SetMark();
        *pNewCrsr->GetMark() = *pMark;
    }
    Reference< XEnumeration > xRet = new SwXParaFrameEnumeration(*pNewCrsr, PARAFRAME_PORTION_TEXTRANGE);
    delete pNewCrsr;
    return xRet;
}
/* -----------------------------03.04.00 09:11--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SAL_CALL SwXTextRange::getAvailableServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = OUString::createFromAscii("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 03.05.00 12:41:46---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< XPropertySetInfo > SAL_CALL SwXTextRange::getPropertySetInfo(  ) throw(RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static Reference< XPropertySetInfo > xRef =
        aPropSet.getPropertySetInfo();
    return xRef;
}
/*-- 03.05.00 12:41:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::setPropertyValue(
    const OUString& rPropertyName, const Any& rValue )
    throw(UnknownPropertyException, PropertyVetoException,
        IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(GetDoc()->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    SwXTextCursor::SetPropertyValue(aPaM, aPropSet, rPropertyName, rValue);
}
/*-- 03.05.00 12:41:47---------------------------------------------------

  -----------------------------------------------------------------------*/
Any SAL_CALL SwXTextRange::getPropertyValue( const OUString& rPropertyName )
    throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(((SwDoc*)GetDoc())->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    return SwXTextCursor::GetPropertyValue(aPaM, aPropSet, rPropertyName);
}
/*-- 03.05.00 12:41:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::addPropertyChangeListener(
    const OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener )
    throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::removePropertyChangeListener(
    const OUString& aPropertyName, const Reference< XPropertyChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::addVetoableChangeListener(
    const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener )
    throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::removeVetoableChangeListener(
    const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
PropertyState SAL_CALL SwXTextRange::getPropertyState( const OUString& rPropertyName )
    throw(UnknownPropertyException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(((SwDoc*)GetDoc())->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    return SwXTextCursor::GetPropertyState(aPaM, aPropSet, rPropertyName);
}
/*-- 03.05.00 12:41:49---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence< PropertyState > SAL_CALL SwXTextRange::getPropertyStates(
    const Sequence< OUString >& rPropertyName ) throw(UnknownPropertyException, RuntimeException)
{
    NAMESPACE_VOS(OGuard) aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(((SwDoc*)GetDoc())->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    return SwXTextCursor::GetPropertyStates(aPaM, aPropSet, rPropertyName);
}
/*-- 03.05.00 12:41:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::setPropertyToDefault( const OUString& rPropertyName )
    throw(UnknownPropertyException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(((SwDoc*)GetDoc())->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    SwXTextCursor::SetPropertyToDefault(aPaM, aPropSet, rPropertyName);
}
/*-- 03.05.00 12:41:50---------------------------------------------------

  -----------------------------------------------------------------------*/
Any SAL_CALL SwXTextRange::getPropertyDefault( const OUString& rPropertyName )
    throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc() || !GetBookmark())
        throw RuntimeException();
    SwPaM aPaM(((SwDoc*)GetDoc())->GetNodes());
    SwXTextRange::GetPositions(aPaM);
    return SwXTextCursor::GetPropertyDefault(aPaM, aPropSet, rPropertyName);
}

/******************************************************************
 * SwXTextRanges
 ******************************************************************/
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextRanges::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXTextRanges::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}
/****************************************************************************
 *	Text positions
 * 	Bis zum ersten Zugriff auf eine TextPosition wird ein SwCursor gehalten,
 * danach wird ein Array mit uno::Reference< XTextPosition >  angelegt
 *
****************************************************************************/
SV_IMPL_PTRARR(XTextRangeArr, XTextRangeRefPtr);
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextRanges::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextRanges");
}
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextRanges::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextRanges") == rServiceName;
}
/* -----------------------------06.04.00 16:36--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextRanges::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextRanges");
    return aRet;
}
/*-- 10.12.98 13:57:20---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRanges::SwXTextRanges() :
    pRangeArr(0)
{

}
/*-- 10.12.98 13:57:21---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRanges::SwXTextRanges( SwUnoCrsr* pCrsr, uno::Reference< XText >  xParent) :
    SwClient(pCrsr),
    xParentText(xParent),
    pRangeArr(0)
{

}
/*-- 10.12.98 13:57:22---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRanges::SwXTextRanges(SwPaM* pCrsr) :
    pRangeArr(0)
{
    SwUnoCrsr* pUnoCrsr = pCrsr->GetDoc()->CreateUnoCrsr(*pCrsr->GetPoint());
    if(pCrsr->HasMark())
    {
        pUnoCrsr->SetMark();
        *pUnoCrsr->GetMark() = *pCrsr->GetMark();
    }
    if(pCrsr->GetNext() != pCrsr)
    {
        SwPaM *_pStartCrsr = (SwPaM *)pCrsr->GetNext();
        do
        {
            //neuen PaM erzeugen
            SwPaM* pPaM = _pStartCrsr->HasMark() ?
                        new SwPaM(*_pStartCrsr->GetMark(), *_pStartCrsr->GetPoint()) :
                            new SwPaM(*_pStartCrsr->GetPoint());
            //und in den Ring einfuegen
            pPaM->MoveTo(pUnoCrsr);

        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != pCrsr );
    }

    pUnoCrsr->Add(this);
}
/*-- 10.12.98 13:57:22---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRanges::~SwXTextRanges()
{
    SwUnoCrsr* pCrsr = GetCrsr();
    delete pCrsr;
    if(pRangeArr)
    {
        pRangeArr->DeleteAndDestroy(0, pRangeArr->Count());
        delete pRangeArr;
    }
}
/*-- 10.12.98 13:57:24---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXTextRanges::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nRet = 0;
    SwUnoCrsr* pCrsr = GetCrsr();
    if(pCrsr)
    {
        FOREACHUNOPAM_START(pCrsr)
            nRet++;
        FOREACHUNOPAM_END()
    }
    else if(pRangeArr)
        nRet = pRangeArr->Count();
    return nRet;
}
/*-- 10.12.98 13:57:25---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextRanges::getByIndex(sal_Int32 nIndex)
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  aRef;
    XTextRangeArr* pArr = ((SwXTextRanges*)this)->GetRangesArray();
    if(pArr && pArr->Count() > nIndex)
    {
        XTextRangeRefPtr pRef = pArr->GetObject(nIndex);
        aRef = *pRef;
    }
    else
        throw lang::IndexOutOfBoundsException();
    uno::Any aRet(&aRef, ::getCppuType((uno::Reference<XTextRange>*)0));
    return aRet;
}
/*-- 10.12.98 13:57:25---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type  SwXTextRanges::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/*-- 10.12.98 13:57:26---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextRanges::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return getCount() > 0;
}
/* -----------------10.12.98 14:25-------------------
 *
 * --------------------------------------------------*/
XTextRangeArr*	SwXTextRanges::GetRangesArray()
{
    SwUnoCrsr* pCrsr = GetCrsr();
    if(!pRangeArr && pCrsr)
    {
        pRangeArr = new XTextRangeArr();
        FOREACHUNOPAM_START(pCrsr)
            uno::Reference< XTextRange > * pPtr = new uno::Reference< XTextRange > ;
            // in welcher Umgebung steht denn der PaM?
            SwStartNode* pSttNode = PUNOPAM->GetNode()->FindStartNode();
            SwStartNodeType eType = pSttNode->GetStartNodeType();
            uno::Reference< XText >  xCurParentRef;
            switch(eType)
            {
                case SwTableBoxStartNode:
                {
                    SwTableBox* pBox = pSttNode->GetTblBox();
                    *pPtr = new SwXTextRange(*pBox->GetFrmFmt(), *pBox, *PUNOPAM);
                }
                break;
                case SwFlyStartNode:
                {
                    SwFrmFmt* pFlyFmt = pSttNode->GetFlyFmt();
                    SwXTextFrame* pxFrm = (SwXTextFrame*)SwClientIter( *pFlyFmt ).
                                                First( TYPE( SwXTextFrame ));
                    if(pxFrm)
                        xCurParentRef = pxFrm;
                    else
                        *pPtr = new SwXTextRange(*pFlyFmt, *PUNOPAM);
                }
                break;
                case SwFootnoteStartNode:
                {
                    const SwFtnIdxs& rIdxs = pCrsr->GetDoc()->GetFtnIdxs();
                    sal_uInt16 n, nFtnCnt = rIdxs.Count();
                    for( n = 0; n < nFtnCnt; ++n )
                    {
                        const SwTxtFtn* pFtn = rIdxs[ n ];

                        const SwStartNode* pTemp = 0;
                        const SwNode& rNode = pFtn->GetStartNode()->GetNode();
                        if(rNode.GetNodeType() == ND_STARTNODE)
                            pTemp = (const SwStartNode*)&rNode;
                        if(pSttNode == pTemp)
                        {
                            const SwFmtFtn& rFtn = pFtn->GetFtn();
                            //TODO: schon existierendes Fussnotenobjekt wiederfinden!
                            xCurParentRef = new SwXFootnote(pCrsr->GetDoc(), rFtn);
                            break;
                        }
                    }
                }
                break;
                case SwHeaderStartNode:
                case SwFooterStartNode:
                {
                    //PageStyle besorgen, HeaderText anlegen/erfragen,
                    //und dann SwXTextPosition damit anlegen
                    String sPageStyleName = lcl_GetCurPageStyle(*pCrsr);
                    uno::Reference< style::XStyleFamiliesSupplier >  xStyleSupp(
                        pCrsr->GetDoc()->GetDocShell()->GetBaseModel(), uno::UNO_QUERY);

                    uno::Reference< container::XNameAccess >  xStyles = xStyleSupp->getStyleFamilies();
                    uno::Any aStyleFamily = xStyles->getByName(C2U("PageStyles"));

                    uno::Reference< container::XNameContainer > xFamily =
                        *(uno::Reference< container::XNameContainer > *)aStyleFamily.getValue();
                    uno::Any aStyle = xFamily->getByName(sPageStyleName);
                    uno::Reference< style::XStyle >  xStyle = *(uno::Reference< style::XStyle > *)aStyle.getValue();

                    uno::Reference< beans::XPropertySet >  xPropSet(xStyle, uno::UNO_QUERY);
                    uno::Any aLayout = xPropSet->getPropertyValue(C2U(UNO_NAME_PAGE_STYLE_LAYOUT));

                     style::PageStyleLayout eLayout = *(style::PageStyleLayout*)aLayout.getValue();
                    uno::Any aShare = xPropSet->getPropertyValue(C2U(UNO_NAME_HEADER_IS_SHARED));
                    sal_Bool bShare;
                    aShare >>= bShare;
                    sal_Bool bLeft = sal_False;
                    sal_Bool bRight = sal_False;
                    //jetzt evtl. noch zw. linker/rechter Kopf-/Fusszeile unterscheiden
                    if(!bShare && eLayout == style::PageStyleLayout_MIRRORED)
                    {
                        uno::Reference<lang::XUnoTunnel> xTunnel(xStyle, uno::UNO_QUERY);
                        SwXPageStyle* pStyle = (SwXPageStyle*)
                            xTunnel->getSomething( SwXPageStyle::getUnoTunnelId());

                        DBG_ASSERT(pStyle, "Was ist das fuer ein style::XStyle?")
                        bLeft = pSttNode == pStyle->GetStartNode(eType == SwHeaderStartNode, sal_True);
                        bRight = !bLeft;
                    }
                    uno::Any aParent;
                    sal_Bool bFooter = eType == SwFooterStartNode;
                    if(eLayout == style::PageStyleLayout_LEFT || bLeft)
                        aParent = xPropSet->getPropertyValue(C2U(bFooter ? UNO_NAME_FOOTER_TEXT_LEFT : UNO_NAME_HEADER_TEXT_LEFT));
                    else if(eLayout == style::PageStyleLayout_RIGHT)
                        aParent = xPropSet->getPropertyValue(C2U(bFooter ? UNO_NAME_FOOTER_TEXT_RIGHT : UNO_NAME_HEADER_TEXT_RIGHT));
                    else
                        aParent = xPropSet->getPropertyValue(C2U(bFooter ? UNO_NAME_FOOTER_TEXT : UNO_NAME_HEADER_TEXT));

                    if(aParent.getValueType() != ::getVoidCppuType())
                    {
                        uno::Reference< XText >  xText = *(uno::Reference< XText > *)aParent.getValue();
                        xCurParentRef = xText;
                    }
                }


                break;
//				case SwNormalStartNode:
                default:
                {
                    if(!xParentText.is())
                    {
                        uno::Reference< XTextDocument >  xDoc(
                            pCrsr->GetDoc()->GetDocShell()->GetBaseModel(), uno::UNO_QUERY);
                        xParentText = xDoc->getText();
                    }
                    xCurParentRef = xParentText;
                }
            }

            if(xCurParentRef.is())
                *pPtr = new SwXTextRange(*PUNOPAM, xCurParentRef);
            if(pPtr->is())
                pRangeArr->Insert(pPtr, pRangeArr->Count());
        FOREACHUNOPAM_END()
        pCrsr->Remove( this );
    }
    return pRangeArr;
}
/*-- 10.12.98 13:57:02---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextRanges::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}


/******************************************************************
 * SwXParagraph
 ******************************************************************/
/* -----------------------------11.07.00 12:10--------------------------------

 ---------------------------------------------------------------------------*/
SwXParagraph* SwXParagraph::GetImplementation(Reference< XInterface> xRef )
{
    uno::Reference<lang::XUnoTunnel> xParaTunnel( xRef, uno::UNO_QUERY);
    if(xParaTunnel.is())
        return (SwXParagraph*)xParaTunnel->getSomething(SwXParagraph::getUnoTunnelId());
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
            return (sal_Int64)this;
    }
    return 0;
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXParagraph::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXParagraph");
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParagraph::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    String sServiceName(rServiceName);
    return sServiceName.EqualsAscii("com.sun.star.text.TextContent") ||
        sServiceName.EqualsAscii("com.sun.star.text.Paragraph") ||
         sServiceName.EqualsAscii("com.sun.star.style.CharacterProperties")||
         sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties");
}
/* -----------------------------06.04.00 16:37--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXParagraph::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(4);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Paragraph");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
     pArray[2] = C2U("com.sun.star.style.ParagraphProperties");
     pArray[3] = C2U("com.sun.star.text.TextContent");
    return aRet;
}
/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph() :
    aLstnrCntnr( (XTextRange*)this),
    xParentText(0),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARAGRAPH)),
    m_bIsDescriptor(TRUE)
{
}

/*-- 11.12.98 08:12:47---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::SwXParagraph(SwXText* pParent, SwUnoCrsr* pCrsr) :
    SwClient(pCrsr),
    xParentText(pParent),
    aLstnrCntnr( (XTextRange*)this),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARAGRAPH)),
    m_bIsDescriptor(FALSE)
{
}
/*-- 11.12.98 08:12:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraph::~SwXParagraph()
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        delete pUnoCrsr;
}
/* -----------------------------11.07.00 14:48--------------------------------

 ---------------------------------------------------------------------------*/
void SwXParagraph::attachToText(SwXText* pParent, SwUnoCrsr* pCrsr)
{
    DBG_ASSERT(m_bIsDescriptor, "Paragraph is not a descriptor")
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
Reference< XPropertySetInfo >  SwXParagraph::getPropertySetInfo(void)
                                            throw( RuntimeException )
{
    static Reference< XPropertySetInfo >  xRef = aPropSet.getPropertySetInfo();
    return xRef;
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::setPropertyValue(const OUString& rPropertyName, const uno::Any& aValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
        lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aAny;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(rPropertyName.equals(C2U(UNO_NAME_TEXT_WRAP)) ||
            rPropertyName.equals(C2U(UNO_NAME_ANCHOR_TYPE)) ||
                rPropertyName.equals(C2U(UNO_NAME_ANCHOR_TYPES)))
        {
            throw IllegalArgumentException();
        }
        SwParaSelection aParaSel(pUnoCrsr);
        SwXTextCursor::SetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName, aValue);
    }
    else
        throw uno::RuntimeException();
}
/*-- 11.12.98 08:12:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParagraph::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aAny;
    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if(pUnoCrsr)
    {
        if(!SwXParagraph::getDefaultTextContentValue(aAny, rPropertyName))
        {
            SwParaSelection aParaSel(pUnoCrsr);
            SwNode& rTxtNode = pUnoCrsr->GetPoint()->nNode.GetNode();
            SwAttrSet& rAttrSet = ((SwTxtNode&)rTxtNode).GetSwAttrSet();

            String aPropertyName(rPropertyName);
            const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                        aPropSet.getPropertyMap(), rPropertyName);
            BOOL bDone = FALSE;
            if(pMap)
            {
                PropertyState eTemp;
                bDone = lcl_getCrsrPropertyValue(pMap, *pUnoCrsr, rAttrSet, aAny, eTemp );
                if(!bDone)
                    aAny = aPropSet.getPropertyValue(*pMap, rAttrSet);
            }
            else
                throw UnknownPropertyException();
//			aAny = SwXTextCursor::GetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName);
        }
    }
    else
        throw uno::RuntimeException();
    return aAny;
}
/* -----------------------------12.09.00 11:09--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParagraph::getDefaultTextContentValue(Any& rAny, const OUString& rPropertyName, USHORT nWID)
{
    if(!nWID)
    {
        if(0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPE))
            nWID = FN_UNO_ANCHOR_TYPE;
        else if(0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPES))
            nWID = FN_UNO_ANCHOR_TYPES;
        else if(0 == rPropertyName.compareToAscii(UNO_NAME_TEXT_WRAP))
            nWID = FN_UNO_TEXT_WRAP;
        else
            return FALSE;
    }

    switch(nWID)
    {
        case FN_UNO_TEXT_WRAP:  rAny <<= WrapTextMode_NONE; break;
        case FN_UNO_ANCHOR_TYPE: rAny <<= TextContentAnchorType_AT_PARAGRAPH; break;
        case FN_UNO_ANCHOR_TYPES:
        {	Sequence<TextContentAnchorType> aTypes(1);
            TextContentAnchorType* pArray = aTypes.getArray();
            pArray[0] = TextContentAnchorType_AT_PARAGRAPH;
            rAny.setValue(&aTypes, ::getCppuType((uno::Sequence<TextContentAnchorType>*)0));
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
    const OUString& PropertyName,
    const uno::Reference< beans::XPropertyChangeListener > & aListener)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::removePropertyChangeListener(const OUString& PropertyName, const uno::Reference< beans::XPropertyChangeListener > & aListener) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 11.12.98 08:12:50---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::addVetoableChangeListener(const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener > & aListener) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::removeVetoableChangeListener(const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
//-----------------------------------------------------------------------------
beans::PropertyState lcl_SwXParagraph_getPropertyState(
                            SwUnoCrsr& rUnoCrsr,
                            const SwAttrSet** ppSet,
                            SfxItemPropertySet& rPropSet,
                            const OUString& rPropertyName )
                                throw( beans::UnknownPropertyException)
{
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;
    if(0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPE) ||
         0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPES) ||
            0 == rPropertyName.compareToAscii(UNO_NAME_TEXT_WRAP))
        return eRet;

    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(!pMap)
    {
        UnknownPropertyException aExcept;
        aExcept.Message = rPropertyName;
        throw aExcept;
    }

    if(!(*ppSet))
    {
        SwNode& rTxtNode = rUnoCrsr.GetPoint()->nNode.GetNode();
        (*ppSet) = ((SwTxtNode&)rTxtNode).GetpSwAttrSet();
    }
    if(!rPropertyName.compareToAscii(UNO_NAME_NUMBERING_RULES))
    {
        //wenn eine Numerierung gesetzt ist, dann hier herausreichen, sonst nichts tun
        lcl_getNumberingProperty( rUnoCrsr, eRet );
    }
    else if(!rPropertyName.compareToAscii(UNO_NAME_PARA_STYLE_NAME) ||
            !rPropertyName.compareToAscii(UNO_NAME_PARA_CONDITIONAL_STYLE_NAME))
    {
        SwFmtColl* pFmt = SwXTextCursor::GetCurTxtFmtColl( rUnoCrsr,
            !rPropertyName.compareToAscii(UNO_NAME_PARA_CONDITIONAL_STYLE_NAME));
        if( !pFmt )
            eRet = beans::PropertyState_AMBIGUOUS_VALUE;
    }
    else if(!rPropertyName.compareToAscii(UNO_NAME_PAGE_STYLE_NAME))
    {
        String sVal = lcl_GetCurPageStyle( rUnoCrsr );
        if( !sVal.Len() )
            eRet = beans::PropertyState_AMBIGUOUS_VALUE;
    }
    else if((*ppSet) && SFX_ITEM_SET == (*ppSet)->GetItemState(pMap->nWID, FALSE))
        eRet = beans::PropertyState_DIRECT_VALUE;
    else
        eRet = beans::PropertyState_DEFAULT_VALUE;

/*	if( !*ppSet )
    {
        // Absatz selektieren
        SwParaSelection aParaSel( &rUnoCrsr );
        *ppSet = new SfxItemSet( rUnoCrsr.GetDoc()->GetAttrPool(),
                        RES_CHRATR_BEGIN, 	RES_PARATR_NUMRULE,
                        RES_FILL_ORDER, 	RES_FRMATR_END -1,
                        RES_UNKNOWNATR_CONTAINER, RES_UNKNOWNATR_CONTAINER,
                        RES_TXTATR_UNKNOWN_CONTAINER, RES_TXTATR_UNKNOWN_CONTAINER,
                        0L);
        SwXTextCursor::GetCrsrAttr( rUnoCrsr, **ppSet, TRUE );
    }
    if(rPropertyName.equals(C2U(UNO_NAME_PAGE_DESC_NAME)))
    {
        // Sonderbehandlung RES_PAGEDESC
        const SfxPoolItem* pItem;
        if(SFX_ITEM_SET == (*ppSet)->GetItemState( RES_PAGEDESC, sal_True, &pItem ) )
        {
            eRet = beans::PropertyState_DIRECT_VALUE;
        }
    }
    else if(rPropertyName.equals(C2U(UNO_NAME_NUMBERING_RULES)))
    {
        //wenn eine Numerierung gesetzt ist, dann hier herausreichen, sonst nichts tun
        lcl_getNumberingProperty( rUnoCrsr, eRet );
    }
    else if(rPropertyName.equals(C2U(UNO_NAME_PARA_STYLE_NAME)) ||
            rPropertyName.equals(C2U(UNO_NAME_PARA_CONDITIONAL_STYLE_NAME)))
    {
        SwFmtColl* pFmt = SwXTextCursor::GetCurTxtFmtColl( rUnoCrsr,
            rPropertyName.equals(C2U(UNO_NAME_PARA_CONDITIONAL_STYLE_NAME)));
        if( !pFmt )
            eRet = beans::PropertyState_AMBIGUOUS_VALUE;
    }
    else if(rPropertyName.equals(C2U(UNO_NAME_PAGE_STYLE_NAME)))
    {
        String sVal = lcl_GetCurPageStyle( rUnoCrsr );
        if( !sVal.Len() )
            eRet = beans::PropertyState_AMBIGUOUS_VALUE;
    }
    else
    {
        eRet = rPropSet.getPropertyState( rPropertyName, **ppSet );
        //special handling for character attributes that are set in the ItemSet of the text node
        if(beans::PropertyState_DEFAULT_VALUE == eRet)
        {
            const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                    rPropSet.getPropertyMap(), rPropertyName);
            if(pMap)
            {
                SwNode& rTxtNode = rUnoCrsr.GetPoint()->nNode.GetNode();
                SwAttrSet* pNodeAttrSet = ((SwTxtNode&)rTxtNode).GetpSwAttrSet();
                if(pNodeAttrSet && SFX_ITEM_SET == pNodeAttrSet->GetItemState(pMap->nWID, FALSE))
                    eRet = beans::PropertyState_DIRECT_VALUE;
            }
        }
    }
*/
    return eRet;
}

/*-- 05.03.99 11:37:30---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SwXParagraph::getPropertyState(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;
    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if( pUnoCrsr )
    {
        const SwAttrSet* pSet = 0;
        eRet = lcl_SwXParagraph_getPropertyState( *pUnoCrsr, &pSet,
                                                    aPropSet, rPropertyName );
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

    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if( pUnoCrsr )
    {
        const SwAttrSet* pSet = 0;
        for(sal_Int32 i = 0, nEnd = PropertyNames.getLength(); i < nEnd; i++ )
            pStates[i] = lcl_SwXParagraph_getPropertyState( *pUnoCrsr, &pSet,
                                                         aPropSet, pNames[i]);
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
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPE) ||
            0 == rPropertyName.compareToAscii(UNO_NAME_ANCHOR_TYPES)||
                0 == rPropertyName.compareToAscii(UNO_NAME_TEXT_WRAP))
            return;

        // Absatz selektieren
        SwParaSelection aParaSel(pUnoCrsr);
        SwDoc* pDoc = pUnoCrsr->GetDoc();
        const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                aPropSet.getPropertyMap(), rPropertyName);
        if(pMap)
        {
            if(pMap->nWID < RES_FRMATR_END)
            {
                SvUShortsSort aWhichIds;
                aWhichIds.Insert(pMap->nWID);
                if(pMap->nWID < RES_PARATR_BEGIN)
                    pUnoCrsr->GetDoc()->ResetAttr(*pUnoCrsr, sal_True, &aWhichIds);
                else
                {
                    //fuer Absatzattribute muss die Selektion jeweils auf
                    //Absatzgrenzen erweitert werden
                    SwPosition aStart = *pUnoCrsr->Start();
                    SwPosition aEnd = *pUnoCrsr->End();
                    SwUnoCrsr* pTemp = pUnoCrsr->GetDoc()->CreateUnoCrsr(aStart, sal_False);
                    if(!lcl_IsStartOfPara(*pTemp))
                    {
                        pTemp->MovePara(fnParaCurr, fnParaStart);
                    }
                    pTemp->SetMark();
                    *pTemp->GetPoint() = aEnd;
                    //pTemp->Exchange();
                    SwXTextCursor::SelectPam(*pTemp, sal_True);
                    if(!lcl_IsEndOfPara(*pTemp))
                    {
                        pTemp->MovePara(fnParaCurr, fnParaEnd);
                    }
                    pTemp->GetDoc()->ResetAttr(*pTemp, sal_True, &aWhichIds);
                    delete pTemp;
                }
            }
            else
                lcl_resetCrsrPropertyValue(pMap, *pUnoCrsr);
        }
        else
            throw beans::UnknownPropertyException();
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
    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if(pUnoCrsr)
    {
        if(SwXParagraph::getDefaultTextContentValue(aRet, rPropertyName))
            return aRet;

        SwDoc* pDoc = pUnoCrsr->GetDoc();
        const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                aPropSet.getPropertyMap(), rPropertyName);
        if(pMap)
        {
            if(pMap->nWID < RES_FRMATR_END)
            {
                const SfxPoolItem& rDefItem =
                    pUnoCrsr->GetDoc()->GetAttrPool().GetDefaultItem(pMap->nWID);
                rDefItem.QueryValue(aRet, pMap->nMemberId);
            }
        }
        else
            throw beans::UnknownPropertyException();
    }
    else
        throw uno::RuntimeException();

    return aRet;
}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::attach(const uno::Reference< XTextRange > & xTextRange)
                    throw( lang::IllegalArgumentException, uno::RuntimeException )
{

}
/*-- 11.12.98 08:12:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXParagraph::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  aRet;
    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if(pUnoCrsr)
    {
        // Absatz selektieren
        SwParaSelection aSelection(pUnoCrsr);
        aRet = new SwXTextRange(*pUnoCrsr, xParentText);
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
    SwUnoCrsr* pUnoCrsr = ((SwXParagraph*)this)->GetCrsr();
    if(pUnoCrsr)
    {
        // Absatz selektieren
        SwParaSelection aSelection(pUnoCrsr);
        pUnoCrsr->GetDoc()->DeleteAndJoin(*pUnoCrsr);
        pUnoCrsr->Remove(this);
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
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        aRef = new SwXTextPortionEnumeration(*pUnoCrsr, xParentText);
    else
        throw uno::RuntimeException();
    return aRef;

}
/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXParagraph::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/*-- 11.12.98 08:12:54---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXParagraph::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(((SwXParagraph*)this)->GetCrsr())
        return sal_True;
    else
        return sal_False;
}
/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XText >  SwXParagraph::getText(void) throw( uno::RuntimeException )
{
    return xParentText;
}
/*-- 11.12.98 08:12:55---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXParagraph::getStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( pUnoCrsr)
    {
        SwPaM aPam(*pUnoCrsr->Start());
        uno::Reference< XText >  xParent = getText();
        xRet = new SwXTextRange(aPam, xParent);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}
/*-- 11.12.98 08:12:56---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXParagraph::getEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( pUnoCrsr)
    {
        SwPaM aPam(*pUnoCrsr->End());
        uno::Reference< XText >  xParent = getText();
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
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( pUnoCrsr)
        aRet = pUnoCrsr->GetNode()->GetTxtNode()->GetTxt();
    else if(IsDescriptor())
        aRet = m_sText;
    else
        throw uno::RuntimeException();
    return aRet;
}
/* -----------------11.12.98 10:07-------------------
 *
 * --------------------------------------------------*/
void SwXTextCursor::SetString(SwUnoCrsr& rUnoCrsr, const OUString& rString)
{
    // Start/EndAction
    SwDoc* pDoc = rUnoCrsr.GetDoc();
    UnoActionContext aAction(pDoc);
    String aText(rString);
    xub_StrLen nTxtLen = aText.Len();
    pDoc->StartUndo(UNDO_INSERT);
    if(rUnoCrsr.HasMark())
        pDoc->DeleteAndJoin(rUnoCrsr);
    if(nTxtLen)
    {
        //OPT: GetSystemCharSet
        if( !pDoc->Insert(rUnoCrsr, aText) )
            ASSERT( sal_False, "Doc->Insert(Str) failed." );
        SwXTextCursor::SelectPam(rUnoCrsr, sal_True);
        rUnoCrsr.Left(nTxtLen);
    }
    pDoc->EndUndo(UNDO_INSERT);
}
/*-- 11.12.98 08:12:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXParagraph::setString(const OUString& aString) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();

    if(pUnoCrsr)
    {
        EXCEPT_ON_PROTECTION(*pUnoCrsr)
        if(!lcl_IsStartOfPara(*pUnoCrsr))
            pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
        SwXTextCursor::SelectPam(*pUnoCrsr, sal_True);
        if(pUnoCrsr->GetNode()->GetTxtNode()->GetTxt().Len())
            pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
        SwXTextCursor::SetString(*pUnoCrsr, aString);
        SwXTextCursor::SelectPam(*pUnoCrsr, sal_False);
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
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( !pUnoCrsr || COMPARE_EQUAL != rServiceName.compareToAscii("com.sun.star.text.TextContent") )
        throw uno::RuntimeException();

    uno::Reference< container::XEnumeration >  xRet = new SwXParaFrameEnumeration(*pUnoCrsr, PARAFRAME_PORTION_PARAGRAPH);
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
/******************************************************************
 * SwXParaFrameEnumeration
 ******************************************************************/
SV_IMPL_PTRARR(SwDependArr, SwDepend*);
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXParaFrameEnumeration::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXParaFrameEnumeration");
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXParaFrameEnumeration::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.util.ContentEnumeration") == rServiceName;
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXParaFrameEnumeration::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.util.ContentEnumeration");
    return aRet;
}
/*-- 23.03.99 13:22:29---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParaFrameEnumeration::SwXParaFrameEnumeration(const SwUnoCrsr& rUnoCrsr,
                                                sal_uInt8 nParaFrameMode,
                                                SwFrmFmt* pFmt)
{
    SwDoc* pDoc = rUnoCrsr.GetDoc();
    SwUnoCrsr* pUnoCrsr = pDoc->CreateUnoCrsr(*rUnoCrsr.GetPoint(), sal_False);
    if(rUnoCrsr.HasMark())
    {
        pUnoCrsr->SetMark();
        *pUnoCrsr->GetMark() = *rUnoCrsr.GetMark();
    }
    pUnoCrsr->Add(this);

    if(PARAFRAME_PORTION_PARAGRAPH == nParaFrameMode)
    {
        const SwNodeIndex& rOwnNode = rUnoCrsr.GetPoint()->nNode;
        //alle Rahmen, Grafiken und OLEs suchen, die an diesem Absatz gebunden sind
        const SwSpzFrmFmts& rFmts = *pDoc->GetSpzFrmFmts();
        USHORT nSize = rFmts.Count();
        for ( USHORT i = 0; i < nSize; i++)
        {
            const SwFrmFmt* pFmt = rFmts[ i ];
            const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
            const SwPosition* pAnchorPos;
            if( rAnchor.GetAnchorId() == FLY_AT_CNTNT &&
                0 != (pAnchorPos = rAnchor.GetCntntAnchor()) &&
                    pAnchorPos->nNode == rOwnNode)
            {
                //jetzt einen SwDepend anlegen und in das Array einfuegen
                SwDepend* pNewDepend = new SwDepend(this, (SwFrmFmt*)pFmt);
                aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
            }
        }
    }
    else if(pFmt)
    {
        //jetzt einen SwDepend anlegen und in das Array einfuegen
        SwDepend* pNewDepend = new SwDepend(this, pFmt);
        aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
    }
    else if((PARAFRAME_PORTION_CHAR == nParaFrameMode) || (PARAFRAME_PORTION_TEXTRANGE == nParaFrameMode))
    {
        SwPosFlyFrms aFlyFrms;
        //get all frames that are bound at paragraph or at character
        pDoc->GetAllFlyFmts(aFlyFrms, pUnoCrsr);//, bDraw);
        for(USHORT i = 0; i < aFlyFrms.Count(); i++)
        {
            SwPosFlyFrm* pPosFly = aFlyFrms[i];
            SwFrmFmt* pFrmFmt = (SwFrmFmt*)&pPosFly->GetFmt();
            //jetzt einen SwDepend anlegen und in das Array einfuegen
            SwDepend* pNewDepend = new SwDepend(this, pFrmFmt);
            aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
        }
        //created from any text range
        if(pUnoCrsr->HasMark())
        {
            if(pUnoCrsr->Start() != pUnoCrsr->GetPoint())
                pUnoCrsr->Exchange();
            do
            {
                FillFrame(*pUnoCrsr);
                pUnoCrsr->Right();
            }
            while(*pUnoCrsr->Start() < *pUnoCrsr->End());
        }
        FillFrame(*pUnoCrsr);
    }
}
/*-- 23.03.99 13:22:30---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParaFrameEnumeration::~SwXParaFrameEnumeration()
{
    aFrameArr.DeleteAndDestroy(0, aFrameArr.Count());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    delete pUnoCrsr;
}
/* -----------------------------03.04.00 10:15--------------------------------
    Description: Search for a FLYCNT text attribute at the cursor point
                and fill the frame into the array
 ---------------------------------------------------------------------------*/
void SwXParaFrameEnumeration::FillFrame(SwUnoCrsr& rUnoCrsr)
{
    // search for objects at the cursor - anchored at/as char
    SwTxtAttr* pTxtAttr = rUnoCrsr.GetNode()->GetTxtNode()->GetTxtAttr(
                    rUnoCrsr.GetPoint()->nContent, RES_TXTATR_FLYCNT);
    if(pTxtAttr)
    {
        const SwFmtFlyCnt& rFlyCnt = pTxtAttr->GetFlyCnt();
        SwFrmFmt* pFrmFmt = rFlyCnt.GetFrmFmt();
        //jetzt einen SwDepend anlegen und in das Array einfuegen
        SwDepend* pNewDepend = new SwDepend(this, pFrmFmt);
        aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
    }
}
/*-- 23.03.99 13:22:32---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXParaFrameEnumeration::hasMoreElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetCrsr())
        throw uno::RuntimeException();
    return xNextObject.is() ? sal_True : CreateNextObject();
}
/*-- 23.03.99 13:22:33---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParaFrameEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetCrsr())
        throw uno::RuntimeException();
    if(!xNextObject.is() && aFrameArr.Count())
    {
        CreateNextObject();
    }
    if(!xNextObject.is())
        throw container::NoSuchElementException();
    uno::Any aRet(&xNextObject, ::getCppuType((uno::Reference<XTextContent>*)0));
    xNextObject = 0;
    return aRet;
}
/* -----------------23.03.99 13:38-------------------
 *
 * --------------------------------------------------*/
sal_Bool SwXParaFrameEnumeration::CreateNextObject()
{
    if(!aFrameArr.Count())
        return sal_False;
    SwDepend* pDepend = aFrameArr.GetObject(0);
    aFrameArr.Remove(0);
    SwFrmFmt* pFormat = (SwFrmFmt*)pDepend->GetRegisteredIn();
    delete pDepend;
    // the format should be valid her otherwise the client
    // would have been removed in ::Modify
    // check for a shape first
    SwClientIter aIter(*pFormat);
    SwDrawContact* pContact = (SwDrawContact*)
                                            aIter.First(TYPE(SwDrawContact));
    if(pContact)
       {
          SdrObject* pSdr = pContact->GetMaster();
        if(pSdr)
        {
            Reference<frame::XModel> xModel =
                            pFormat->GetDoc()->GetDocShell()->GetBaseModel();
            Reference<drawing::XDrawPageSupplier> xPageSupp(
                        xModel, UNO_QUERY);

            Reference<drawing::XDrawPage> xPage = xPageSupp->getDrawPage();
            XDrawPage* pImpPage = xPage.get();
            Reference <XShape> xShape = ((SwXDrawPage*)pImpPage)->GetSvxPage()->_CreateShape( pSdr );
            xNextObject = uno::Reference< XTextContent >(xShape, uno::UNO_QUERY);
        }
       }
    else
    {
        const SwNodeIndex* pIdx = pFormat->GetCntnt().GetCntntIdx();
        DBG_ASSERT(pIdx, "where is the index?");
        const SwNode* pNd = GetCrsr()->GetDoc()->GetNodes()[ pIdx->GetIndex() + 1 ];

        FlyCntType eType;
        if(!pNd->IsNoTxtNode())
            eType = FLYCNTTYPE_FRM;
        else if(pNd->IsGrfNode())
            eType = FLYCNTTYPE_GRF;
        else
            eType = FLYCNTTYPE_OLE;

        uno::Reference< container::XNamed >  xFrame = SwXFrames::GetObject(*pFormat, eType);
        xNextObject = uno::Reference< XTextContent >(xFrame, uno::UNO_QUERY);
    }

    return xNextObject.is();
}

/*-- 23.03.99 13:22:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXParaFrameEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    switch( pOld ? pOld->Which() : 0 )
    {
    case RES_REMOVE_UNO_OBJECT:
    case RES_OBJECTDYING:
        if( (void*)GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            ((SwModify*)GetRegisteredIn())->Remove(this);
        break;

    case RES_FMT_CHG:
        // wurden wir an das neue umgehaengt und wird das alte geloscht?
        if( ((SwFmtChg*)pNew)->pChangedFmt == GetRegisteredIn() &&
            ((SwFmtChg*)pOld)->pChangedFmt->IsFmtInDTOR() )
            ((SwModify*)GetRegisteredIn())->Remove(this);
        break;
    }
    if(!GetRegisteredIn())
    {
        aFrameArr.DeleteAndDestroy(0, aFrameArr.Count());
        xNextObject = 0;
    }
    else
    {
        //dann war es vielleicht ein Frame am Absatz?
        for(sal_uInt16 i = aFrameArr.Count(); i; i--)
        {
            SwDepend* pDepend = aFrameArr.GetObject(i-1);
            if(!pDepend->GetRegisteredIn())
            {
                delete pDepend;
                aFrameArr.Remove(i-1);
            }
        }
    }
}
/******************************************************************
 *	SwXTextPortionEnumeration
 ******************************************************************/
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextPortionEnumeration::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXTextPortionEnumeration::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextPortionEnumeration::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextPortionEnumeration");
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextPortionEnumeration::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextPortionEnumeration") == rServiceName;
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextPortionEnumeration::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextPortionEnumeration");
    return aRet;
}
/*-- 27.01.99 10:44:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextPortionEnumeration::SwXTextPortionEnumeration(SwPaM& rParaCrsr, uno::Reference< XText >  xParentText) :
    xParent(xParentText),
    bAtEnd(sal_False),
    bFirstPortion(sal_True)
{
    SwUnoCrsr* pUnoCrsr = rParaCrsr.GetDoc()->CreateUnoCrsr(*rParaCrsr.GetPoint(), sal_False);
    pUnoCrsr->Add(this);

    //alle Rahmen, Grafiken und OLEs suchen, die an diesem Absatz AM ZEICHEN gebunden sind
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    sal_uInt16 nCount = pDoc->GetFlyCount();
    const SwNodeIndex nOwnNode = pUnoCrsr->GetPoint()->nNode;
    for( sal_uInt16 i = 0; i < nCount; i++)
    {
        SwFrmFmt* pFmt = pDoc->GetFlyNum(i);
        //steht der Anker in diesem Node und ist er absatzgebunden?
        if( pFmt->GetAnchor().GetAnchorId() == FLY_AUTO_CNTNT )
        {
            const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
            const SwPosition* pAnchorPos = rAnchor.GetCntntAnchor();
            if(pAnchorPos->nNode == nOwnNode)
            {
                //jetzt einen SwDepend anlegen und sortiert in das Array einfuegen
                SwDepend* pNewDepend = new SwDepend(this, pFmt);
                const SwFmtAnchor& rNewAnchor = pFmt->GetAnchor();
                const SwPosition* pNewAnchorPos = rNewAnchor.GetCntntAnchor();
                xub_StrLen nInsertIndex = pNewAnchorPos->nContent.GetIndex();

                for(sal_uInt16 nFrame = 0; nFrame < aFrameArr.Count(); nFrame++)
                {
                    SwDepend* pCurDepend = aFrameArr.GetObject(nFrame);
                    SwFrmFmt* pFormat = (SwFrmFmt*)pCurDepend->GetRegisteredIn();
                    const SwFmtAnchor& rAnchor = pFormat->GetAnchor();
                    const SwPosition* pAnchorPos = rAnchor.GetCntntAnchor();

                    if(pAnchorPos->nContent.GetIndex() > nInsertIndex)
                    {
                        aFrameArr.C40_INSERT(SwDepend, pNewDepend, nFrame);
                        pNewDepend = 0;
                        break;
                    }
                }
                if(pNewDepend)
                    aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
            }
        }
    }
    CreatePortions();
}
/*-- 27.01.99 10:44:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextPortionEnumeration::~SwXTextPortionEnumeration()
{
    for(sal_uInt16 nFrame = aFrameArr.Count(); nFrame; nFrame--)
    {
        SwDepend* pCurDepend = aFrameArr.GetObject(nFrame - 1);
        delete pCurDepend;
        aFrameArr.Remove(nFrame - 1);
    }

    if( aPortionArr.Count() )
        aPortionArr.DeleteAndDestroy(0, aPortionArr.Count() );

    SwUnoCrsr* pUnoCrsr = GetCrsr();
    delete pUnoCrsr;
}
/*-- 27.01.99 10:44:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextPortionEnumeration::hasMoreElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return aPortionArr.Count() > 0;
}
/*-- 27.01.99 10:44:45---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextPortionEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!aPortionArr.Count())
        throw container::NoSuchElementException();
    XTextRangeRefPtr pPortion = aPortionArr.GetObject(0);
    Any aRet(pPortion, ::getCppuType((uno::Reference<XTextRange>*)0));
    aPortionArr.Remove(0);
    delete pPortion;
    return aRet;
}
/* -----------------------------31.08.00 14:28--------------------------------

 ---------------------------------------------------------------------------*/
void lcl_InsertRefMarkPortion(
    XTextRangeArr& rArr, SwUnoCrsr* pUnoCrsr, Reference<XText> xParent, SwTxtAttr* pAttr, BOOL bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    SwFmtRefMark& rRefMark = ((SwFmtRefMark&)pAttr->GetAttr());
    Reference<XTextContent> xContent = ((SwUnoCallBack*)pDoc->GetUnoCallBack())->GetRefMark(rRefMark);
    if(!xContent.is())
        xContent = new SwXReferenceMark(pDoc, &rRefMark);

    SwXTextPortion* pPortion;
    if(!bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_REFMARK_START)),
            rArr.Count());
        pPortion->SetRefMark(xContent);
        pPortion->SetIsCollapsed(pAttr->GetEnd() ? FALSE : TRUE);
    }
    else
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_REFMARK_END)),
            rArr.Count());
        pPortion->SetRefMark(xContent);
    }
}
//-----------------------------------------------------------------------------
void lcl_InsertTOXMarkPortion(
    XTextRangeArr& rArr, SwUnoCrsr* pUnoCrsr, Reference<XText> xParent, SwTxtAttr* pAttr, BOOL bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    SwTOXMark& rTOXMark = ((SwTOXMark&)pAttr->GetAttr());

    Reference<XTextContent> xContent =
        ((SwUnoCallBack*)pDoc->GetUnoCallBack())->GetTOXMark(rTOXMark);
    if(!xContent.is())
        xContent = new SwXDocumentIndexMark(rTOXMark.GetTOXType(), &rTOXMark, pDoc);

    SwXTextPortion* pPortion;
    if(!bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_TOXMARK_START)),
            rArr.Count());
        pPortion->SetTOXMark(xContent);
        pPortion->SetIsCollapsed(pAttr->GetEnd() ? FALSE : TRUE);
    }
    if(bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_TOXMARK_END)),
            rArr.Count());
        pPortion->SetTOXMark(xContent);
    }
}
//-----------------------------------------------------------------------------
#define BKM_TYPE_START			0
#define BKM_TYPE_END            1
#define BKM_TYPE_START_END      2
struct SwXBookmarkPortion_Impl
{
    SwXBookmark* 				pBookmark;
    Reference<XTextContent>		xBookmark;
    BYTE						nBkmType;
    ULONG 						nIndex;

    SwXBookmarkPortion_Impl( SwXBookmark* pXMark, BYTE nType, ULONG nIdx) :
        pBookmark(pXMark),
        xBookmark(pXMark),
        nBkmType(nType),
        nIndex(nIdx){}

    // compare by nIndex
    BOOL operator < (const SwXBookmarkPortion_Impl &rCmp) const;
    BOOL operator ==(const SwXBookmarkPortion_Impl &rCmp) const;

};
BOOL SwXBookmarkPortion_Impl::operator ==(const SwXBookmarkPortion_Impl &rCmp) const
{
    return nIndex == rCmp.nIndex &&
        pBookmark == rCmp.pBookmark &&
            nBkmType == rCmp.nBkmType;
}

BOOL SwXBookmarkPortion_Impl::operator < (const SwXBookmarkPortion_Impl &rCmp) const
{
    return nIndex < rCmp.nIndex;
}

typedef SwXBookmarkPortion_Impl* SwXBookmarkPortion_ImplPtr;
SV_DECL_PTRARR_SORT(SwXBookmarkPortionArr, SwXBookmarkPortion_ImplPtr, 0, 2)
SV_IMPL_OP_PTRARR_SORT(SwXBookmarkPortionArr, SwXBookmarkPortion_ImplPtr)
//-----------------------------------------------------------------------------
void lcl_ExportBookmark(
    SwXBookmarkPortionArr& rBkmArr, ULONG nIndex,
    SwUnoCrsr* pUnoCrsr, Reference<XText> xParent, XTextRangeArr& rPortionArr)
{
    SwXBookmarkPortion_ImplPtr pPtr;
    while(rBkmArr.Count() && nIndex == (pPtr = rBkmArr.GetObject(0))->nIndex)
    {
        SwXTextPortion* pPortion;
        if(BKM_TYPE_START == pPtr->nBkmType || BKM_TYPE_START_END == pPtr->nBkmType)
        {
            rPortionArr.Insert(
                new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_BOOKMARK_START)),
                rPortionArr.Count());
            pPortion->SetBookmark(pPtr->xBookmark);
            pPortion->SetIsCollapsed(BKM_TYPE_START_END == pPtr->nBkmType ? TRUE : FALSE);

        }
        if(BKM_TYPE_END == pPtr->nBkmType)
        {
            rPortionArr.Insert(
                new Reference< XTextRange >(pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_BOOKMARK_END)),
                rPortionArr.Count());
            pPortion->SetBookmark(pPtr->xBookmark);
        }
        rBkmArr.Remove((USHORT)0);
        delete pPtr;
    }
}
//-----------------------------------------------------------------------------
Reference<XTextRange> lcl_ExportHints(SwpHints* pHints,
                                XTextRangeArr& rPortionArr,
                                SwUnoCrsr* pUnoCrsr,
                                Reference<XText> xParent,
                                const xub_StrLen nCurrentIndex,
                                SwTextPortionType& ePortionType,
                                const xub_StrLen& nFirstFrameIndex,
                                SwXBookmarkPortionArr& aBkmArr)
{
    Reference<XTextRange> xRef;
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    sal_Bool bAttrFound = sal_False;
    //search for special text attributes - first some ends
    sal_uInt16 nEndIndex = 0;
    sal_uInt16 nNextEnd = 0;
    while(nEndIndex < pHints->GetEndCount() &&
        (!pHints->GetEnd(nEndIndex)->GetEnd() ||
        nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetEnd()))))
    {
        if(pHints->GetEnd(nEndIndex)->GetEnd())
        {
            SwTxtAttr* pAttr = pHints->GetEnd(nEndIndex);
            USHORT nAttrWhich = pAttr->Which();
            if(nNextEnd == nCurrentIndex &&
                ( RES_TXTATR_TOXMARK == nAttrWhich ||
                        RES_TXTATR_REFMARK == nAttrWhich))
            {
                switch( nAttrWhich )
                {
                    case RES_TXTATR_TOXMARK:
                        lcl_InsertTOXMarkPortion(
                            rPortionArr, pUnoCrsr, xParent, pAttr, TRUE);
                        ePortionType = PORTION_TEXT;
                    break;
                    case RES_TXTATR_REFMARK:
                        lcl_InsertRefMarkPortion(
                            rPortionArr, pUnoCrsr, xParent, pAttr, TRUE);
                        ePortionType = PORTION_TEXT;
                    break;

                }
            }
        }
        nEndIndex++;
    }
    //then som starts
    sal_uInt16 nStartIndex = 0;
    sal_uInt16 nNextStart = 0;
    while(nStartIndex < pHints->GetStartCount() &&
        nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
    {
        SwTxtAttr* pAttr = pHints->GetStart(nStartIndex);
        USHORT nAttrWhich = pAttr->Which();
        if(nNextStart == nCurrentIndex &&
            (!pAttr->GetEnd() ||
                RES_TXTATR_TOXMARK == nAttrWhich ||
                    RES_TXTATR_REFMARK == nAttrWhich))
        {
            switch( nAttrWhich )
            {
                case RES_TXTATR_FIELD:
                    pUnoCrsr->Right(1);
                    bAttrFound = sal_True;
                    ePortionType = PORTION_FIELD;
                break;
                case RES_TXTATR_FLYCNT	 :
                    pUnoCrsr->Right(1);
                    pUnoCrsr->Exchange();
                    bAttrFound = sal_True;
                    ePortionType = PORTION_FRAME;
                break;
                case RES_TXTATR_FTN 	 :
                {
                    pUnoCrsr->Right(1);
                    SwXTextPortion* pPortion;
                    xRef = 	pPortion = new SwXTextPortion(*pUnoCrsr, xParent, PORTION_FOOTNOTE);
                    Reference<XTextContent> xContent =
                        Reference<XTextContent>(
                        SwXFootnotes::GetObject(*pDoc, pAttr->SwTxtAttr::GetFtn()),
                        UNO_QUERY);
                    pPortion->SetFootnote(xContent);
                    bAttrFound = sal_True;
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_SOFTHYPH :
                {
                    SwXTextPortion* pPortion;
                    rPortionArr.Insert(
                        new Reference< XTextRange >(
                            pPortion = new SwXTextPortion(
                                *pUnoCrsr, xParent, PORTION_CONTROL_CHAR)),
                        rPortionArr.Count());
                    pPortion->SetControlChar(3);
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_HARDBLANK:
                {
                    ePortionType = PORTION_CONTROL_CHAR;
                    SwXTextPortion* pPortion;
                    rPortionArr.Insert(
                        new Reference< XTextRange >(
                            pPortion = new SwXTextPortion(
                                *pUnoCrsr, xParent, PORTION_CONTROL_CHAR)),
                        rPortionArr.Count());
                    const SwFmtHardBlank& rFmt = pAttr->GetHardBlank();
                    if(rFmt.GetChar() == '-')
                        pPortion->SetControlChar(2);//HARD_HYPHEN
                    else
                        pPortion->SetControlChar(4);//HARD_SPACE
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_TOXMARK:
                    lcl_InsertTOXMarkPortion(
                        rPortionArr, pUnoCrsr, xParent, pAttr, FALSE);
                    ePortionType = PORTION_TEXT;
                break;
                case RES_TXTATR_REFMARK:
                    pUnoCrsr->Right(1);
                    bAttrFound = sal_True;
                    lcl_InsertRefMarkPortion(
                        rPortionArr, pUnoCrsr, xParent, pAttr, FALSE);
                    ePortionType = PORTION_TEXT;
                    if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                            pUnoCrsr->Exchange();
                    pUnoCrsr->DeleteMark();
                break;
                default:
                    DBG_ERROR("was fuer ein Attribut?");
            }

        }
        nStartIndex++;
    }

    if(!bAttrFound)
    {
        // hier wird nach Uebergaengen zwischen Attributen gesucht, die nach der
        // aktuellen Cursorposition liegen
        // wenn dabei ein Rahmen 'ueberholt' wird, dann muss auch in der TextPortion unterbrochen werden

        nStartIndex = 0;
        nNextStart = 0;
        while(nStartIndex < pHints->GetStartCount() &&
            nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
            nStartIndex++;

        sal_uInt16 nEndIndex = 0;
        sal_uInt16 nNextEnd = 0;
        while(nEndIndex < pHints->GetEndCount() &&
            nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetAnyEnd())))
            nEndIndex++;
        //nMovePos legt die neue EndPosition fest
        sal_uInt16 nMovePos = nNextStart > nCurrentIndex && nNextStart < nNextEnd ? nNextStart : nNextEnd;

        if(aBkmArr.Count() && aBkmArr.GetObject(0)->nIndex < nMovePos)
        {
            DBG_ASSERT(aBkmArr.GetObject(0)->nIndex > nCurrentIndex,
                "forgotten bookmark(s)")
            nMovePos = aBkmArr.GetObject(0)->nIndex;
        }
        // liegt die Endposition nach dem naechsten Rahmen, dann aufbrechen
        if(nFirstFrameIndex != STRING_MAXLEN && nMovePos > nFirstFrameIndex)
            nMovePos = nFirstFrameIndex;

        if(nMovePos > nCurrentIndex)
            pUnoCrsr->Right(nMovePos - nCurrentIndex);
        else
            pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
    }
    return xRef;
}
//-----------------------------------------------------------------------------
void SwXTextPortionEnumeration::CreatePortions()
{
    uno::Any aRet;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr /*&& !bAtEnd*/)
    {
        SwXBookmarkPortionArr aBkmArr;
        SwDoc* pDoc = pUnoCrsr->GetDoc();
        const SwNodeIndex nOwnNode = pUnoCrsr->GetPoint()->nNode;
        //search for all bookmarks that start or end in this paragraph
        const SwBookmarks& rMarks = pDoc->GetBookmarks();
        sal_uInt16 nArrLen = rMarks.Count();
        for( sal_uInt16 n = 0; n < nArrLen; ++n )
        {
            SwBookmark* pMark = rMarks.GetObject( n );
            if( !pMark->IsBookMark() )
                continue;

            const SwPosition& rPos1 = pMark->GetPos();
            const SwPosition* pPos2 = pMark->GetOtherPos();
            BOOL bBackward = pPos2 ? rPos1 > *pPos2: FALSE;
            if(rPos1.nNode == nOwnNode)
            {
                BYTE nType = bBackward ? BKM_TYPE_END : BKM_TYPE_START;
                if(!pPos2)
                {
                    nType = BKM_TYPE_START_END;
                }
                SwXBookmarkPortion_ImplPtr pBkmPtr = new SwXBookmarkPortion_Impl(
                    SwXBookmarks::GetObject( *pMark, pDoc ), nType, rPos1.nContent.GetIndex() );

                aBkmArr.Insert(pBkmPtr);

            }
            if(pPos2 && pPos2->nNode == nOwnNode)
            {
                BYTE nType = bBackward ? BKM_TYPE_START : BKM_TYPE_END;
                SwXBookmarkPortion_ImplPtr pBkmPtr = new SwXBookmarkPortion_Impl(
                        SwXBookmarks::GetObject( *pMark, pDoc ), nType, pPos2->nContent.GetIndex() );
                aBkmArr.Insert(pBkmPtr);
            }
        }
#ifdef DEBUG
        for(long i_debug = 0; i_debug <aBkmArr.Count(); i_debug++)
        {
            SwXBookmarkPortion_ImplPtr pPtr = aBkmArr.GetObject(i_debug);
        }

#endif
        while(!bAtEnd)
        {
            if(pUnoCrsr->HasMark())
            {
                if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                    pUnoCrsr->Exchange();
                pUnoCrsr->DeleteMark();
            }
            if(!bFirstPortion	&& pUnoCrsr->GetCntntNode() &&
                    pUnoCrsr->GetPoint()->nContent == pUnoCrsr->GetCntntNode()->Len())
            {
                //hier sollte man nie ankommen!
                bAtEnd = sal_True;
            }
            else
            {
                SwNode* pNode = pUnoCrsr->GetNode();
                if(ND_TEXTNODE == pNode->GetNodeType())
                {
                    SwTxtNode* pTxtNode = (SwTxtNode*)pNode;
                    SwpHints* pHints = pTxtNode->GetpSwpHints();
                    SwTextPortionType ePortionType = PORTION_TEXT;
                    xub_StrLen nCurrentIndex = pUnoCrsr->GetPoint()->nContent.GetIndex();
                    xub_StrLen nFirstFrameIndex = STRING_MAXLEN;
                    uno::Reference< XTextRange >  xRef;
                    if(!pUnoCrsr->GetCntntNode()->Len())
                    {
                        if(pHints)
                        {
                            DBG_ERROR("hints not exported")
                        }
                        if(aBkmArr.Count())
                        {
                            lcl_ExportBookmark(aBkmArr, 0, pUnoCrsr, xParent, aPortionArr);
                        }
                        // der Absatz ist leer, also nur Portion erzeugen und raus
                        xRef = new SwXTextPortion(*pUnoCrsr, xParent, ePortionType);
                    }
                    else
                    {
                        //falls schon Rahmen entsorgt wurden, dann raus hier
                        for(sal_uInt16 nFrame = aFrameArr.Count(); nFrame; nFrame--)
                        {
                            SwDepend* pCurDepend = aFrameArr.GetObject(nFrame - 1);
                            if(!pCurDepend->GetRegisteredIn())
                            {
                                delete pCurDepend;
                                aFrameArr.Remove(nFrame - 1);
                            }
                        }

                        //zunaechst den ersten Frame im aFrameArr finden (bezogen auf die Position im Absatz)
                        SwDepend* pFirstFrameDepend = 0;
                        //Eintraege im aFrameArr sind sortiert!
                        if(aFrameArr.Count())
                        {
                            SwDepend* pCurDepend = aFrameArr.GetObject(0);
                            SwFrmFmt* pFormat = (SwFrmFmt*)pCurDepend->GetRegisteredIn();
                            const SwFmtAnchor& rAnchor = pFormat->GetAnchor();
                            const SwPosition* pAnchorPos = rAnchor.GetCntntAnchor();
                            pFirstFrameDepend = pCurDepend;
                            nFirstFrameIndex = pAnchorPos->nContent.GetIndex();
                        }

                        SwXTextCursor::SelectPam(*pUnoCrsr, sal_True);

                        //ist hier schon ein Rahmen faellig?
                        if(nCurrentIndex == nFirstFrameIndex)
                        {
                            xRef = new SwXTextPortion(*pUnoCrsr, xParent,
                                *(SwFrmFmt*)pFirstFrameDepend->GetRegisteredIn());
                            SwDepend* pCurDepend = aFrameArr.GetObject(0);
                            delete pCurDepend;
                            aFrameArr.Remove(0);
                        }
                    }
                    if(!xRef.is())
                    {
                        lcl_ExportBookmark(aBkmArr, nCurrentIndex, pUnoCrsr, xParent, aPortionArr);
                        if(pHints)
                        {
                            xRef = lcl_ExportHints(pHints,
                                aPortionArr,
                                pUnoCrsr,
                                xParent,
                                nCurrentIndex,
                                ePortionType,
                                nFirstFrameIndex,
                                aBkmArr);

                        }
                        else if(USHRT_MAX != nFirstFrameIndex)
                        {
                            pUnoCrsr->Right(nFirstFrameIndex - nCurrentIndex);
                        }
                        else
                        {
                            lcl_ExportBookmark(aBkmArr, nCurrentIndex, pUnoCrsr, xParent, aPortionArr);
                            if(!aBkmArr.Count())
                                sal_Bool bMove = pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
                            else
                            {
                                SwXBookmarkPortion_ImplPtr pPtr = aBkmArr.GetObject(0);
                                sal_uInt16 nMovePos = pPtr->nIndex;
                                DBG_ASSERT(nMovePos > nCurrentIndex, "wrong move index")
                                pUnoCrsr->Right(nMovePos - nCurrentIndex);
                            }
                        }
                    }
                    if(!xRef.is() && pUnoCrsr->HasMark() )
                        xRef = new SwXTextPortion(*pUnoCrsr, xParent, ePortionType);
                    if(xRef.is())
                        aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                }
                else
                {
                    DBG_ERROR("kein TextNode - was nun?")
                }
            }
            if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                    pUnoCrsr->Exchange();

            // Absatzende ?
            if(pUnoCrsr->GetCntntNode() &&
                    pUnoCrsr->GetPoint()->nContent == pUnoCrsr->GetCntntNode()->Len())
            {
                bAtEnd = sal_True;
                lcl_ExportBookmark(aBkmArr, pUnoCrsr->GetCntntNode()->Len(),
                                            pUnoCrsr, xParent, aPortionArr);
                SwNode* pNode = pUnoCrsr->GetNode();
                if(ND_TEXTNODE == pNode->GetNodeType())
                {
                    SwTxtNode* pTxtNode = (SwTxtNode*)pNode;
                    SwpHints* pHints = pTxtNode->GetpSwpHints();
                    if(pHints)
                    {
                        SwTextPortionType ePortionType = PORTION_TEXT;
                        Reference<XTextRange> xRef = lcl_ExportHints(pHints,
                            aPortionArr,
                            pUnoCrsr,
                            xParent,
                            pUnoCrsr->GetCntntNode()->Len(),
                            ePortionType,
                            STRING_MAXLEN,
                            aBkmArr);
                        if(xRef.is())
                            aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                    }
                }
            }
        }
    }
}
/*-- 27.01.99 10:44:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextPortionEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}

/* -----------------22.04.99 11:24-------------------
 *
 * --------------------------------------------------*/
SV_IMPL_PTRARR(SwEvtLstnrArray, XEventListenerPtr);

/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::SwEventListenerContainer( uno::XInterface* pxParent) :
    pListenerArr(0),
    pxParent(pxParent)
{
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::~SwEventListenerContainer()
{
    if(pListenerArr && pListenerArr->Count())
    {
        pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
    }
    delete pListenerArr;
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwEventListenerContainer::AddListener(const uno::Reference< lang::XEventListener > & rxListener)
{
    if(!pListenerArr)
        pListenerArr = new SwEvtLstnrArray;
    uno::Reference< lang::XEventListener > * pInsert = new uno::Reference< lang::XEventListener > ;
    *pInsert = rxListener;
    pListenerArr->Insert(pInsert, pListenerArr->Count());
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool	SwEventListenerContainer::RemoveListener(const uno::Reference< lang::XEventListener > & rxListener)
{
    if(!pListenerArr)
        return sal_False;
    else
    {
         lang::XEventListener* pLeft = rxListener.get();
        for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
        {
            XEventListenerPtr pElem = pListenerArr->GetObject(i);
             lang::XEventListener* pRight = pElem->get();
            if(pLeft == pRight)
            {
                pListenerArr->Remove(i);
                delete pElem;
                return sal_True;
            }
        }
    }
    return sal_False;
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwEventListenerContainer::Disposing()
{
    if(!pListenerArr)
        return;

    lang::EventObject aObj(pxParent);
    for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
    {
        XEventListenerPtr pElem = pListenerArr->GetObject(i);
        (*pElem)->disposing(aObj);
    }
    pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
}

