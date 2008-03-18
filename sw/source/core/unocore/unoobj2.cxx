/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unoobj2.cxx,v $
 *
 *  $Revision: 1.68 $
 *
 *  last change: $Author: vg $ $Date: 2008-03-18 16:00:16 $
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
#include <sfx2/linkmgr.hxx>
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
#ifndef _UNOPORT_HXX
#include <unoport.hxx>
#endif
#ifndef _UNOCRSRHELPER_HXX
#include <unocrsrhelper.hxx>
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
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _CTRLTOOL_HXX //autogen
#include <svtools/ctrltool.hxx>
#endif
#ifndef _FLYPOS_HXX
#include <flypos.hxx>
#endif
#ifndef _TXTFTN_HXX //autogen
#include <txtftn.hxx>
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
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _UNOTOOLS_COLLATORWRAPPER_HXX
#include <unotools/collatorwrapper.hxx>
#endif
#ifndef _COM_SUN_STAR_TABLE_TABLESORTFIELD_HPP_
#include <com/sun/star/table/TableSortField.hpp>
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
#define _SVSTDARR_XUB_STRLEN
#include <svtools/svstdarr.hxx>
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
// OD 2004-05-07 #i28701#
#include <vector>
// OD 2004-05-24 #i28701#
#ifndef _SORTEDOBJS_HXX
#include <sortedobjs.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;

using ::rtl::OUString;

// OD 2004-05-07 #i28701# - adjust 4th parameter
void CollectFrameAtNode( SwClient& rClnt, const SwNodeIndex& rIdx,
                         SwDependArr& rFrameArr,
                         const bool _bAtCharAnchoredObjs )
{
    // _bAtCharAnchoredObjs:
    // <TRUE>: at-character anchored objects are collected
    // <FALSE>: at-paragraph anchored objects are collected

    // alle Rahmen, Grafiken und OLEs suchen, die an diesem Absatz
    // gebunden sind
    SwDoc* pDoc = rIdx.GetNode().GetDoc();

    USHORT nChkType = static_cast< USHORT >(_bAtCharAnchoredObjs ? FLY_AUTO_CNTNT : FLY_AT_CNTNT);
    const SwCntntFrm* pCFrm;
    const SwCntntNode* pCNd;
    if( pDoc->GetRootFrm() &&
        0 != (pCNd = rIdx.GetNode().GetCntntNode()) &&
        0 != (pCFrm = pCNd->GetFrm()) )
    {
        const SwSortedObjs *pObjs = pCFrm->GetDrawObjs();
        if( pObjs )
            for( USHORT i = 0; i < pObjs->Count(); ++i )
            {
                SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
                SwFrmFmt& rFmt = pAnchoredObj->GetFrmFmt();
                if ( rFmt.GetAnchor().GetAnchorId() == nChkType )
                {
                    //jetzt einen SwDepend anlegen und in das Array einfuegen
                    SwDepend* pNewDepend = new SwDepend( &rClnt, &rFmt );

                    // OD 2004-05-07 #i28701# - sorting no longer needed,
                    // because list <SwSortedObjs> is already sorted.
                    rFrameArr.C40_INSERT( SwDepend, pNewDepend, rFrameArr.Count() );
                }
            }
    }
    else
    {
        // OD 2004-05-07 #i28701# - helper list to get <rFrameArr> sorted
        std::vector< std::pair< xub_StrLen, sal_uInt32 > > aSortLst;
        typedef std::vector< std::pair< xub_StrLen, sal_uInt32 > >::iterator tSortLstIter;

        const SwSpzFrmFmts& rFmts = *pDoc->GetSpzFrmFmts();
        USHORT nSize = rFmts.Count();
        for ( USHORT i = 0; i < nSize; i++)
        {
            const SwFrmFmt* pFmt = rFmts[ i ];
            const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
            const SwPosition* pAnchorPos;
            if( rAnchor.GetAnchorId() == nChkType &&
                0 != (pAnchorPos = rAnchor.GetCntntAnchor()) &&
                    pAnchorPos->nNode == rIdx )
            {
                //jetzt einen SwDepend anlegen und in das Array einfuegen
                SwDepend* pNewDepend = new SwDepend( &rClnt, (SwFrmFmt*)pFmt);

                // OD 2004-05-07 #i28701# - determine insert position for
                // sorted <rFrameArr>
                USHORT nInsPos = rFrameArr.Count();
                {
                    xub_StrLen nCntIndex = pAnchorPos->nContent.GetIndex();
                    sal_uInt32 nAnchorOrder = rAnchor.GetOrder();

                    tSortLstIter aInsIter = aSortLst.end();
                    for ( tSortLstIter aIter = aSortLst.begin();
                          aIter != aSortLst.end();
                          ++aIter )
                    {
                        if ( (*aIter).first > nCntIndex ||
                             ( (*aIter).first == nCntIndex &&
                               (*aIter).second > nAnchorOrder ) )
                        {
                            nInsPos = sal::static_int_cast< USHORT >(aIter - aSortLst.begin());
                            aInsIter = aIter;
                            break;
                        }
                    }
                    std::pair< xub_StrLen, sal_uInt32 > aEntry( nCntIndex,
                                                                nAnchorOrder );
                    aSortLst.insert( aInsIter, aEntry );
                }
                rFrameArr.C40_INSERT( SwDepend, pNewDepend, nInsPos );
            }
        }
    }
}


/*-- 09.12.98 14:18:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::insertDocumentFromURL(const OUString& rURL,
    const uno::Sequence< beans::PropertyValue >& rOptions)
    throw( lang::IllegalArgumentException, io::IOException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwUnoCursorHelper::InsertFile(pUnoCrsr, rURL, rOptions);
    }
    else
        throw uno::RuntimeException();
}
/* -----------------------------15.12.00 14:01--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXTextCursor::createSortDescriptor(sal_Bool bFromTable)
{
    uno::Sequence< beans::PropertyValue > aRet(5);
    beans::PropertyValue* pArray = aRet.getArray();

    uno::Any aVal;
    aVal.setValue( &bFromTable, ::getCppuBooleanType());
    pArray[0] = beans::PropertyValue(C2U("IsSortInTable"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    String sSpace(String::CreateFromAscii(" "));
    sal_Unicode uSpace = sSpace.GetChar(0);

    aVal <<= uSpace;
    pArray[1] = beans::PropertyValue(C2U("Delimiter"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal <<= (sal_Bool) sal_False;
    pArray[2] = beans::PropertyValue(C2U("IsSortColumns"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    aVal <<= (sal_Int32) 3;
    pArray[3] = beans::PropertyValue(C2U("MaxSortFieldsCount"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    uno::Sequence< table::TableSortField > aFields(3);
    table::TableSortField* pFields = aFields.getArray();

    Locale aLang( SvxCreateLocale( LANGUAGE_SYSTEM ) );
    // get collator algorithm to be used for the locale
    Sequence < OUString > aSeq( GetAppCollator().listCollatorAlgorithms( aLang ) );
    INT32 nLen = aSeq.getLength();
    DBG_ASSERT( nLen > 0, "list of collator algorithms is empty!");
    OUString aCollAlg;
    if (nLen > 0)
        aCollAlg = aSeq.getConstArray()[0];

#if OSL_DEBUG_LEVEL > 1
    const OUString *pTxt = aSeq.getConstArray();
    (void)pTxt;
#endif

    pFields[0].Field = 1;
    pFields[0].IsAscending = sal_True;
    pFields[0].IsCaseSensitive = sal_False;
    pFields[0].FieldType = table::TableSortFieldType_ALPHANUMERIC;
    pFields[0].CollatorLocale = aLang;
    pFields[0].CollatorAlgorithm = aCollAlg;

    pFields[1].Field = 1;
    pFields[1].IsAscending = sal_True;
    pFields[1].IsCaseSensitive = sal_False;
    pFields[1].FieldType = table::TableSortFieldType_ALPHANUMERIC;
    pFields[1].CollatorLocale = aLang;
    pFields[1].CollatorAlgorithm = aCollAlg;

    pFields[2].Field = 1;
    pFields[2].IsAscending = sal_True;
    pFields[2].IsCaseSensitive = sal_False;
    pFields[2].FieldType = table::TableSortFieldType_ALPHANUMERIC;
    pFields[2].CollatorLocale = aLang;
    pFields[2].CollatorAlgorithm = aCollAlg;

    aVal <<= aFields;
    pArray[4] = beans::PropertyValue(C2U("SortFields"), -1, aVal, beans::PropertyState_DIRECT_VALUE);

    return aRet;
}

/*-- 09.12.98 14:18:58---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyValue > SwXTextCursor::createSortDescriptor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return SwXTextCursor::createSortDescriptor(sal_False);
}
/* -----------------------------15.12.00 14:06--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwXTextCursor::convertSortProperties(
    const uno::Sequence< beans::PropertyValue >& rDescriptor, SwSortOptions& rSortOpt)
{
    sal_Bool bRet = sal_True;
    const beans::PropertyValue* pProperties = rDescriptor.getConstArray();

    rSortOpt.bTable = sal_False;
    rSortOpt.cDeli = ' ';
    rSortOpt.eDirection = SRT_COLUMNS;  //!! UI text may be contrary though !!

    SwSortKey* pKey1 = new SwSortKey;
    pKey1->nColumnId = USHRT_MAX;
    pKey1->bIsNumeric = TRUE;
    pKey1->eSortOrder   = SRT_ASCENDING;

    SwSortKey* pKey2 = new SwSortKey;
    pKey2->nColumnId = USHRT_MAX;
    pKey2->bIsNumeric = TRUE;
    pKey2->eSortOrder   = SRT_ASCENDING;

    SwSortKey* pKey3 = new SwSortKey;
    pKey3->nColumnId = USHRT_MAX;
    pKey3->bIsNumeric = TRUE;
    pKey3->eSortOrder   = SRT_ASCENDING;
    SwSortKey* aKeys[3] = {pKey1, pKey2, pKey3};

    sal_Bool bOldSortdescriptor(sal_False);
    sal_Bool bNewSortdescriptor(sal_False);

    for( int n = 0; n < rDescriptor.getLength(); ++n )
    {
        uno::Any aValue( pProperties[n].Value );
//		String sPropName = pProperties[n].Name;
        const OUString& rPropName = pProperties[n].Name;

        // old and new sortdescriptor
        if( COMPARE_EQUAL == rPropName.compareToAscii("IsSortInTable"))
        {
            if ( aValue.getValueType() == ::getBooleanCppuType() )
                rSortOpt.bTable = *(sal_Bool*)aValue.getValue();
            else
                bRet = sal_False;
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("Delimiter"))
        {
            sal_Unicode uChar = sal_Unicode();
            if( aValue >>= uChar )
                rSortOpt.cDeli = uChar;
            else
                bRet = sal_False;
        }
        // old sortdescriptor
        else if(COMPARE_EQUAL == rPropName.compareToAscii("SortColumns"))
        {
            bOldSortdescriptor = sal_True;
            if ( aValue.getValueType() == ::getBooleanCppuType() )
            {
                sal_Bool bTemp = *(sal_Bool*)aValue.getValue();
                rSortOpt.eDirection = bTemp ? SRT_COLUMNS : SRT_ROWS;
            }
            else
                bRet = sal_False;
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("IsCaseSensitive"))
        {
            bOldSortdescriptor = sal_True;
            if ( aValue.getValueType() == ::getBooleanCppuType() )
            {
                sal_Bool bTemp = *(sal_Bool*)aValue.getValue();
                rSortOpt.bIgnoreCase = !bTemp;
            }
            else
                bRet = sal_False;
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("CollatorLocale"))
        {
            bOldSortdescriptor = sal_True;
            Locale aLocale;
            if (aValue >>= aLocale)
                rSortOpt.nLanguage = SvxLocaleToLanguage( aLocale );
            else
                bRet = sal_False;
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("CollatorAlgorithm", 17) &&
            rPropName.getLength() == 18 &&
            (rPropName.getStr()[17] >= '0' && rPropName.getStr()[17] <= '9'))
        {
            bOldSortdescriptor = sal_True;
            sal_uInt16 nIndex = rPropName.getStr()[17];
            nIndex -= '0';
            OUString aTxt;
            if ((aValue >>= aTxt) && nIndex < 3)
                aKeys[nIndex]->sSortType = aTxt;
            else
                bRet = sal_False;
        }
        else if(COMPARE_EQUAL == rPropName.compareToAscii("SortRowOrColumnNo", 17) &&
            rPropName.getLength() == 18 &&
            (rPropName.getStr()[17] >= '0' && rPropName.getStr()[17] <= '9'))
        {
            bOldSortdescriptor = sal_True;
            sal_uInt16 nIndex = rPropName.getStr()[17];
            nIndex -= '0';
            sal_Int16 nCol = -1;
            if( aValue.getValueType() == ::getCppuType((const sal_Int16*)0) && nIndex < 3)
                aValue >>= nCol;
            if(	nCol >= 0 )
                aKeys[nIndex]->nColumnId = nCol;
            else
                bRet = sal_False;
        }
        else if(0 == rPropName.indexOf(C2U("IsSortNumeric")) &&
            rPropName.getLength() == 14 &&
            (rPropName.getStr()[13] >= '0' && rPropName.getStr()[13] <= '9'))
        {
            bOldSortdescriptor = sal_True;
            sal_uInt16 nIndex = rPropName.getStr()[13];
            nIndex = nIndex - '0';
            if ( aValue.getValueType() == ::getBooleanCppuType() && nIndex < 3 )
            {
                sal_Bool bTemp = *(sal_Bool*)aValue.getValue();
                aKeys[nIndex]->bIsNumeric = bTemp;
            }
            else
                bRet = sal_False;
        }
        else if(0 == rPropName.indexOf(C2U("IsSortAscending")) &&
            rPropName.getLength() == 16 &&
            (rPropName.getStr()[15] >= '0' && rPropName.getStr()[15] <= '9'))
        {
            bOldSortdescriptor = sal_True;
            sal_uInt16 nIndex = rPropName.getStr()[15];
            nIndex -= '0';
            if ( aValue.getValueType() == ::getBooleanCppuType() && nIndex < 3 )
            {
                sal_Bool bTemp = *(sal_Bool*)aValue.getValue();
                aKeys[nIndex]->eSortOrder = bTemp ? SRT_ASCENDING : SRT_DESCENDING;
            }
            else
                bRet = sal_False;
        }
        // new sortdescriptor
        else if(COMPARE_EQUAL == rPropName.compareToAscii("IsSortColumns"))
        {
            bNewSortdescriptor = sal_True;
            if ( aValue.getValueType() == ::getBooleanCppuType() )
            {
                sal_Bool bTemp = *(sal_Bool*)aValue.getValue();
                rSortOpt.eDirection = bTemp ? SRT_COLUMNS : SRT_ROWS;
            }
            else
                bRet = sal_False;
        }
        else if (COMPARE_EQUAL == rPropName.compareToAscii("SortFields"))
        {
            bNewSortdescriptor = sal_True;
            uno::Sequence < table::TableSortField > aFields;
            if ( aValue >>= aFields )
            {
                sal_Int32 nCount(aFields.getLength());
                if (nCount <= 3)
                {
                    table::TableSortField* pFields = aFields.getArray();
                    for (sal_Int32 i = 0; i < nCount; ++i)
                    {
                        rSortOpt.bIgnoreCase = !pFields[i].IsCaseSensitive;
                        rSortOpt.nLanguage = SvxLocaleToLanguage( pFields[i].CollatorLocale );
                        aKeys[i]->sSortType = pFields[i].CollatorAlgorithm;
                        aKeys[i]->nColumnId = static_cast< USHORT >(pFields[i].Field);
                        aKeys[i]->bIsNumeric = (pFields[i].FieldType == table::TableSortFieldType_NUMERIC);
                        aKeys[i]->eSortOrder = pFields[i].IsAscending ? SRT_ASCENDING : SRT_DESCENDING;
                    }
                }
                else
                    bRet = sal_False;
            }
            else
                bRet = sal_False;
        }
    }

    if (bNewSortdescriptor && bOldSortdescriptor)
    {
        DBG_ERROR("someone tried to set the old deprecated and the new sortdescriptor");
        bRet = sal_False;
    }

    if(pKey1->nColumnId != USHRT_MAX)
        rSortOpt.aKeys.C40_INSERT(SwSortKey, pKey1, rSortOpt.aKeys.Count());
    if(pKey2->nColumnId != USHRT_MAX)
        rSortOpt.aKeys.C40_INSERT(SwSortKey, pKey2, rSortOpt.aKeys.Count());
    if(pKey3->nColumnId != USHRT_MAX)
        rSortOpt.aKeys.C40_INSERT(SwSortKey, pKey3, rSortOpt.aKeys.Count());

    return bRet && rSortOpt.aKeys.Count() > 0;
}
/*-- 09.12.98 14:19:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::sort(const uno::Sequence< beans::PropertyValue >& rDescriptor)
        throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwSortOptions aSortOpt;
    SwUnoCrsr* pUnoCrsr = GetCrsr();

    if(pUnoCrsr)
    {
        if(pUnoCrsr->HasMark())
        {
            if(!SwXTextCursor::convertSortProperties(rDescriptor, aSortOpt))
                    throw uno::RuntimeException();
            UnoActionContext aContext( pUnoCrsr->GetDoc() );

            SwPosition* pStart = pUnoCrsr->Start();
            SwPosition* pEnd   = pUnoCrsr->End();

            SwNodeIndex aPrevIdx( pStart->nNode, -1 );
            ULONG nOffset = pEnd->nNode.GetIndex() - pStart->nNode.GetIndex();
            xub_StrLen nCntStt  = pStart->nContent.GetIndex();

            pUnoCrsr->GetDoc()->SortText(*pUnoCrsr, aSortOpt);

            // Selektion wieder setzen
            pUnoCrsr->DeleteMark();
            pUnoCrsr->GetPoint()->nNode.Assign( aPrevIdx.GetNode(), +1 );
            SwCntntNode* pCNd = pUnoCrsr->GetCntntNode();
            xub_StrLen nLen = pCNd->Len();
            if( nLen > nCntStt )
                nLen = nCntStt;
            pUnoCrsr->GetPoint()->nContent.Assign(pCNd, nLen );
            pUnoCrsr->SetMark();

            pUnoCrsr->GetPoint()->nNode += nOffset;
            pCNd = pUnoCrsr->GetCntntNode();
            pUnoCrsr->GetPoint()->nContent.Assign( pCNd, pCNd->Len() );
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
uno::Reference< XEnumeration >  SAL_CALL SwXTextCursor::createContentEnumeration(const OUString& rServiceName) throw( RuntimeException )
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( !pUnoCrsr || 0 != rServiceName.compareToAscii("com.sun.star.text.TextContent") )
        throw RuntimeException();

    uno::Reference< XEnumeration > xRet = new SwXParaFrameEnumeration(*pUnoCrsr, PARAFRAME_PORTION_TEXTRANGE);
    return xRet;
}
/* -----------------------------07.03.01 14:53--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< XEnumeration >  SwXTextCursor::createEnumeration(void) throw( RuntimeException )
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if( !pUnoCrsr  )
        throw RuntimeException();
    uno::Reference<XUnoTunnel> xTunnel(xParentText, UNO_QUERY);
    SwXText* pParentText = 0;
    if(xTunnel.is())
    {
        pParentText = reinterpret_cast< SwXText *>(
                sal::static_int_cast< sal_IntPtr >( xTunnel->getSomething(SwXText::getUnoTunnelId()) ));
    }
    DBG_ASSERT(pParentText, "parent is not a SwXText")

    SwUnoCrsr* pNewCrsr = pUnoCrsr->GetDoc()->CreateUnoCrsr(*pUnoCrsr->GetPoint());
    if(pUnoCrsr->HasMark())
    {
        pNewCrsr->SetMark();
        *pNewCrsr->GetMark() = *pUnoCrsr->GetMark();
    }
    CursorType eSetType = eType == CURSOR_TBLTEXT ? CURSOR_SELECTION_IN_TABLE : CURSOR_SELECTION;
    SwXParagraphEnumeration *pEnum = new SwXParagraphEnumeration(pParentText, *pNewCrsr, eSetType);
    uno::Reference< XEnumeration > xRet = pEnum;
    if (eType == CURSOR_TBLTEXT)
    {
        // for import of tables in tables we have to remember the actual
        // table and start node of the current position in the enumeration.
        SwTableNode *pStartN = pUnoCrsr->GetPoint()->nNode.GetNode().FindTableNode();
        if (pStartN)
        {
            pEnum->SetOwnTable( &pStartN->GetTable() );
            pEnum->SetOwnStartNode( pStartN );
        }
    }

    return xRet;
}
/* -----------------------------07.03.01 15:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Type  SwXTextCursor::getElementType(void) throw( RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/* -----------------------------07.03.01 15:43--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwXTextCursor::hasElements(void) throw( RuntimeException )
{
    return sal_True;
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

IMPL_STATIC_LINK( SwXTextCursor, RemoveCursor_Impl,
                  uno::Reference< XInterface >*, EMPTYARG )
{
    ASSERT( pThis != NULL, "no reference?" );
    //ASSERT( pArg != NULL, "no reference?" );

    // --> FME 2006-03-07 #126177# Tell the SwXTextCursor that the user event
    // has been executed. It is not necessary to remove the user event in
    // ~SwXTextCursor
    pThis->DoNotRemoveUserEvent();
    // <--

    SwUnoCrsr* pCursor = pThis->GetCrsr();
    if( pCursor != NULL )
    {
        pCursor->Remove( pThis );
        delete pCursor;
    }

    // --> FME 2006-03-07 #126177#
    //delete pArg;
    // <--

    return 0;
}

void 	SwXTextCursor::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);

    // if the cursor leaves its designated section, it becomes invalid
    if( !mbRemoveUserEvent && ( pOld != NULL ) && ( pOld->Which() == RES_UNOCURSOR_LEAVES_SECTION ) )
    {
        // --> FME 2006-03-07 #126177# We don't need to create a reference
        // to the SwXTextCursor to prevent its deletion. If the destructor
        // of the SwXTextCursor is called before the user event is executed,
        // the user event will be removed. This is necessary, because an other
        // thread might be currently waiting in ~SwXTextCursor. In this case
        // the pRef = new ... stuff did not work!

        // create reference to this object to prevent deletion before
        // the STATIC_LINK is executed. The link will delete the
        // reference.
        //uno::Reference<XInterface>* pRef =
            //new uno::Reference<XInterface>( static_cast<XServiceInfo*>( this ) );

        mbRemoveUserEvent = true;
        // <--

        mnUserEventId = Application::PostUserEvent(
                        STATIC_LINK( this, SwXTextCursor, RemoveCursor_Impl ), this );
    }

    if(!GetRegisteredIn())
        aLstnrCntnr.Disposing();

}
/*-- 09.12.98 14:19:01---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwPaM* 	SwXTextCursor::GetPaM() const
{
    return GetCrsr() ? GetCrsr() : 0;
}
// -----------------------------------------------------------------------------
SwPaM* 	SwXTextCursor::GetPaM()
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
void SwXTextCursor::SetCrsrAttr(SwPaM& rPam, const SfxItemSet& rSet, USHORT nAttrMode)
{
    sal_uInt16 nFlags = nsSetAttrMode::SETATTR_APICALL | (nAttrMode & nsSetAttrMode::SETATTR_NOFORMATATTR);
    if(nAttrMode & CRSR_ATTR_MODE_DONTREPLACE)
        nFlags |= nsSetAttrMode::SETATTR_DONTREPLACE;
    SwDoc* pDoc = rPam.GetDoc();
    //StartEndAction
    UnoActionContext aAction(pDoc);
    SwPaM* pCrsr = &rPam;
    if( pCrsr->GetNext() != pCrsr ) 	// Ring von Cursorn
    {
        pDoc->StartUndo(UNDO_INSATTR, NULL);

        SwPaM *_pStartCrsr = &rPam;
        do
        {
            if( _pStartCrsr->HasMark() && ( (CRSR_ATTR_MODE_TABLE & nAttrMode) ||
                *_pStartCrsr->GetPoint() != *_pStartCrsr->GetMark() ))
                pDoc->Insert(*_pStartCrsr, rSet, nFlags );
        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != &rPam );

        pDoc->EndUndo(UNDO_INSATTR, NULL);
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
// --> OD 2006-07-12 #i63870#
// split third parameter <bCurrentAttrOnly> into new parameters <bOnlyTxtAttr>
// and <bGetFromChrFmt> to get better control about resulting <SfxItemSet>
void SwXTextCursor::GetCrsrAttr( SwPaM& rPam,
                                 SfxItemSet& rSet,
                                 BOOL bOnlyTxtAttr,
                                 BOOL bGetFromChrFmt )
{
    static const sal_uInt16 nMaxLookup = 1000;
    SfxItemSet aSet( *rSet.GetPool(), rSet.GetRanges() );
    SfxItemSet *pSet = &rSet;
    SwPaM *_pStartCrsr = &rPam;
    do
    {
        ULONG nSttNd = _pStartCrsr->GetMark()->nNode.GetIndex(),
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
            for( ULONG n = nSttNd; n <= nEndNd; ++n )
            {
                SwNode* pNd = rPam.GetDoc()->GetNodes()[ n ];
                switch( pNd->GetNodeType() )
                {
                case ND_TEXTNODE:
                    {
                        xub_StrLen nStt = n == nSttNd ? nSttCnt : 0,
                                nEnd = n == nEndNd ? nEndCnt
                                            : ((SwTxtNode*)pNd)->GetTxt().Len();
                        ((SwTxtNode*)pNd)->GetAttr( *pSet, nStt, nEnd, bOnlyTxtAttr, bGetFromChrFmt );
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
        nFirstParaStart(-1),
        nLastParaEnd(-1),
        nEndIndex(rPos.nNode.GetIndex()),
        eCursorType(eType),
        bFirstParagraph(sal_True)
{
    pOwnTable = 0;
    pOwnStartNode = 0;
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
        nFirstParaStart(-1),
        nLastParaEnd(-1),
        nEndIndex(pCrsr->End()->nNode.GetIndex()),
        eCursorType(eType),
        bFirstParagraph(sal_True)
{
    pOwnTable = 0;
    pOwnStartNode = 0;
    if(CURSOR_SELECTION == eCursorType || CURSOR_SELECTION_IN_TABLE == eCursorType)
    {
        if(*pCrsr->GetPoint() > *pCrsr->GetMark())
            pCrsr->Exchange();
        nFirstParaStart = pCrsr->GetPoint()->nContent.GetIndex();
        nLastParaEnd = pCrsr->GetMark()->nContent.GetIndex();
        if(pCrsr->HasMark())
            pCrsr->DeleteMark();
    }
}
/*-- 10.12.98 11:52:12---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParagraphEnumeration::~SwXParagraphEnumeration()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        delete pUnoCrsr;

}
/*-- 10.12.98 11:52:13---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXParagraphEnumeration::hasMoreElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return bFirstParagraph ? sal_True : xNextPara.is();
}
/*-- 14.08.03 13:10:14---------------------------------------------------

  -----------------------------------------------------------------------*/

//!! compare to SwShellTableCrsr::FillRects() in viscrs.cxx
SwTableNode * lcl_FindTopLevelTable(
        /*SwUnoCrsr* pUnoCrsr ,*/
        SwTableNode *pTblNode,
        const SwTable *pOwnTable )
{
    // find top-most table in current context (section) level

    SwTableNode * pLast = pTblNode;
    for (SwTableNode* pTmp = pLast;
         pTmp != NULL  &&  &pTmp->GetTable() != pOwnTable;	/* we must not go up higher than the own table! */
         pTmp = pTmp->StartOfSectionNode()->FindTableNode() )
    {
        pLast = pTmp;
    }
    return pLast;
}


BOOL lcl_CursorIsInSection(
        const SwUnoCrsr *pUnoCrsr,
        const SwStartNode *pOwnStartNode )
{
    // returns true if the cursor is in the section (or in a sub section!)
    // represented by pOwnStartNode

    BOOL bRes = TRUE;
    if (pUnoCrsr && pOwnStartNode)
    {
        const SwEndNode * pOwnEndNode = pOwnStartNode->EndOfSectionNode();
        bRes = pOwnStartNode->GetIndex() <= pUnoCrsr->Start()->nNode.GetIndex() &&
               pUnoCrsr->End()->nNode.GetIndex() <= pOwnEndNode->GetIndex();
    }
    return bRes;
}


uno::Reference< XTextContent > SAL_CALL SwXParagraphEnumeration::NextElement_Impl(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    uno::Reference< XTextContent >  aRef;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        // check for exceeding selections
        if(!bFirstParagraph &&
            (CURSOR_SELECTION == eCursorType || CURSOR_SELECTION_IN_TABLE == eCursorType))
        {
            SwPosition* pStart = pUnoCrsr->Start();
            ::std::auto_ptr<SwUnoCrsr> aNewCrsr( pUnoCrsr->GetDoc()->CreateUnoCrsr(*pStart, sal_False) );
            //man soll hier auch in Tabellen landen duerfen
            if(CURSOR_TBLTEXT != eCursorType && CURSOR_SELECTION_IN_TABLE != eCursorType)
                aNewCrsr->SetRemainInSection( sal_False );

            // os 2005-01-14: This part is only necessary to detect movements out of a selection
            // if there is no selection we don't have to care
            SwTableNode* pTblNode = aNewCrsr->GetNode()->FindTableNode();
            if((CURSOR_TBLTEXT != eCursorType && CURSOR_SELECTION_IN_TABLE != eCursorType) && pTblNode)
            {
                aNewCrsr->GetPoint()->nNode = pTblNode->EndOfSectionIndex();
                aNewCrsr->Move(fnMoveForward, fnGoNode);
            }
            else
                aNewCrsr->MovePara(fnParaNext, fnParaStart);
            if(nEndIndex < aNewCrsr->Start()->nNode.GetIndex())
                return aRef;	// empty reference
        }

        XText* pText = xParentText.get();
        sal_Bool bInTable = sal_False;
        if(!bFirstParagraph)
        {
            //man soll hier auch in Tabellen landen duerfen
            //if(CURSOR_TBLTEXT != eCursorType && CURSOR_SELECTION_IN_TABLE != eCursorType)
            {
                //BOOL bRemain = sal_False;
                //pUnoCrsr->SetRemainInSection( bRemain );
                pUnoCrsr->SetRemainInSection( sal_False );
                //was mache ich, wenn ich schon in einer Tabelle stehe?
                SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
                pTblNode = lcl_FindTopLevelTable( /*pUnoCrsr,*/ pTblNode, pOwnTable );
                if(pTblNode  &&  &pTblNode->GetTable() != pOwnTable)
                {
                    // wir haben es mit einer fremden Tabelle zu tun - also ans Ende
                    pUnoCrsr->GetPoint()->nNode = pTblNode->EndOfSectionIndex();
                    if(!pUnoCrsr->Move(fnMoveForward, fnGoNode))
                        return aRef;
                    else
                        bInTable = sal_True;

                }
            }
        }

        // the cursor must remain in the current section or a subsection
        // before AND after the movement...
        if( lcl_CursorIsInSection( pUnoCrsr, pOwnStartNode ) &&
            (bFirstParagraph || bInTable ||
            (pUnoCrsr->MovePara(fnParaNext, fnParaStart) &&
                lcl_CursorIsInSection( pUnoCrsr, pOwnStartNode ) ) ) )
        {
            SwPosition* pStart = pUnoCrsr->Start();
            sal_Int32 nFirstContent = bFirstParagraph ? nFirstParaStart : -1;
            sal_Int32 nLastContent = nEndIndex ==  pStart->nNode.GetIndex() ? nLastParaEnd : -1;
            //steht man nun in einer Tabelle, oder in einem einfachen Absatz?

            SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
            pTblNode = lcl_FindTopLevelTable( /*pUnoCrsr,*/ pTblNode, pOwnTable );
            if(/*CURSOR_TBLTEXT != eCursorType && CURSOR_SELECTION_IN_TABLE != eCursorType && */
                pTblNode  &&  &pTblNode->GetTable() != pOwnTable)
            {
                // wir haben es mit einer fremden Tabelle zu tun
                SwFrmFmt* pTableFmt = (SwFrmFmt*)pTblNode->GetTable().GetFrmFmt();
                XTextTable* pTable = SwXTextTables::GetObject( *pTableFmt );
                aRef =  (XTextContent*)(SwXTextTable*)pTable;
            }
            else
            {
                SwUnoCrsr* pNewCrsr = pUnoCrsr->GetDoc()->CreateUnoCrsr(*pStart, sal_False);
                aRef =  (XTextContent*)new SwXParagraph((SwXText*)pText, pNewCrsr, nFirstContent, nLastContent);
            }
        }
    }
    else
        throw uno::RuntimeException();

    return aRef;
}
/*-- 10.12.98 11:52:14---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXParagraphEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextContent >  aRef;

    if (bFirstParagraph)
    {
        xNextPara = NextElement_Impl();
        bFirstParagraph = sal_False;
    }
    aRef = xNextPara;
    if (!aRef.is())
        throw container::NoSuchElementException();
    xNextPara = NextElement_Impl();

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
            return sal::static_int_cast< sal_Int64 >( reinterpret_cast <sal_IntPtr >(this) );
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
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesAsian")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesComplex")||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesAsian") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesComplex");
}
/* -----------------------------06.04.00 16:34--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextRange::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(7);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextRange");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
    pArray[2] = C2U("com.sun.star.style.CharacterPropertiesAsian");
    pArray[3] = C2U("com.sun.star.style.CharacterPropertiesComplex");
    pArray[4] = C2U("com.sun.star.style.ParagraphProperties");
    pArray[5] = C2U("com.sun.star.style.ParagraphPropertiesAsian");
    pArray[6] = C2U("com.sun.star.style.ParagraphPropertiesComplex");
    return aRet;
}

/*-- 10.12.98 12:54:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwPaM& rPam, const uno::Reference< XText > & rxParent) :
    eRangePosition(RANGE_IN_TEXT),
    pDoc(rPam.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    aObjectDepend(this, 0),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    xParentText(rxParent)
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/*-- 10.12.98 12:54:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rFmt, SwPaM& rPam) :
    eRangePosition(RANGE_IN_FRAME),
    pDoc(rPam.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    aObjectDepend(this, &rFmt),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR))
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt, SwTableBox& rTblBox, SwPaM& rPam) :
    eRangePosition(RANGE_IN_CELL),
    pDoc(rPam.GetDoc()),
    pBox(&rTblBox),
    pBoxStartNode(0),
    aObjectDepend(this, &rTblFmt),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR))
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/* -----------------------------09.08.00 16:07--------------------------------

 ---------------------------------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt, const SwStartNode& rStartNode, SwPaM& rPam) :
    eRangePosition(RANGE_IN_CELL),
    pDoc(rPam.GetDoc()),
    pBox(0),
    pBoxStartNode(&rStartNode),
    aObjectDepend(this, &rTblFmt),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR))
{
    //Bookmark an der anlegen
    _CreateNewBookmark(rPam);
}
/* -----------------19.02.99 11:39-------------------
 *
 * --------------------------------------------------*/
SwXTextRange::SwXTextRange(SwFrmFmt& rTblFmt) :
    eRangePosition(RANGE_IS_TABLE),
    pDoc(rTblFmt.GetDoc()),
    pBox(0),
    pBoxStartNode(0),
    aObjectDepend(this, &rTblFmt),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR))
{
}

/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextRange::~SwXTextRange()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(GetBookmark())
        pDoc->deleteBookmark( GetBookmark()->GetName() );
}
/*-- 10.12.98 12:54:44---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwXTextRange::_CreateNewBookmark(SwPaM& rPam)
{
    static sal_Int32 nBookmark = 0;
    String sBookmarkName;

    SwBookmark* pBkm = GetBookmark();
    if(pBkm)
    {
        // If a bookmark exists already its name can be resused
        sBookmarkName = pBkm->GetName();
        pDoc->deleteBookmark( sBookmarkName );
    }
    else
    {
        // Otherwise we have to create a new name. This is not done
        // using SwDoc::MakeUniqueBookmarkName, because this method
        // starts counting bookmarks beginning with 1. That's required
        // for real bookmarks, but very slow in thsi case there lots
        // of bookmarks requiere any unique name only.
        String sPrefix(C2S("SwXTextPosition"));
        const SwBookmarks& rBookmarks = pDoc->getBookmarks();
        sal_uInt16 nBookmarks = rBookmarks.Count(), i;
        do
        {
            nBookmark++;
            if( nBookmark < 1 )	// on overwflow restart with 1
                nBookmark = 1;

            sBookmarkName = sPrefix;
            sBookmarkName += String::CreateFromInt32( nBookmark );
            for( i = 0; i < nBookmarks; i++ )
                if( rBookmarks[i]->GetName().Equals( sBookmarkName ) )
                    break;
        }
        while( i < nBookmarks );
    }

    KeyCode aCode;
    String sShortName;
    SwBookmark* pMark = pDoc->makeBookmark(rPam, aCode, sBookmarkName, sShortName, IDocumentBookmarkAccess::UNO_BOOKMARK);
    pMark->Add(this);
}
/*-- 10.12.98 12:54:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextRange::DeleteAndInsert(const String& rText) throw( uno::RuntimeException )
{
    SwBookmark* pBkm = GetBookmark();
    if(pBkm )
    {
        const SwPosition& rPoint = *pBkm->BookmarkStart();
        const SwPosition* pMark = pBkm->BookmarkEnd();
        SwCursor aNewCrsr( rPoint, 0, false );
        if(pMark)
        {
            aNewCrsr.SetMark();
            *aNewCrsr.GetMark() = *pMark;
        }

        UnoActionContext aAction( pDoc );
        pDoc->StartUndo(UNDO_INSERT, NULL);
        if(aNewCrsr.HasMark())
            pDoc->DeleteAndJoin(aNewCrsr);

        if(rText.Len())
        {
            SwUnoCursorHelper::DocInsertStringSplitCR( *pDoc, aNewCrsr, rText );

            SwXTextCursor::SelectPam(aNewCrsr, sal_True);
            aNewCrsr.Left(rText.Len(), CRSR_SKIP_CHARS, FALSE, FALSE);
        }
        _CreateNewBookmark(aNewCrsr);
        pDoc->EndUndo(UNDO_INSERT, NULL);
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
            SwTable* pTable = SwTable::FindTable( pTblFmt );
            SwTableNode* pTblNode = pTable->GetTableNode();
            SwPosition aPosition( *pTblNode );
            uno::Reference< XTextRange >  xRange = SwXTextRange::CreateTextRangeFromPosition(pDoc,
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
        SwPaM aPam(*pBkm->BookmarkStart());
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
//		SwPaM aPam(pBkm->GetOtherPos()? *pBkm->GetOtherPos() : pBkm->GetPos());
        SwPaM aPam(*pBkm->BookmarkEnd());
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
    if(pBkm && pBkm->GetOtherBookmarkPos())
    {
        const SwPosition& rPoint = pBkm->GetBookmarkPos();
        const SwPosition* pMark = pBkm->GetOtherBookmarkPos();
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
    if(RANGE_IS_TABLE == eRangePosition)
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
        *rToFill.GetPoint() = pBkm->GetBookmarkPos();
        if(pBkm->GetOtherBookmarkPos())
        {
            rToFill.SetMark();
            *rToFill.GetMark() = *pBkm->GetOtherBookmarkPos();
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
    OTextCursorHelper* pCursor = 0;
    SwXTextPortion* pPortion = 0;
    SwXText* pText = 0;
    SwXParagraph* pPara = 0;
    if(xRangeTunnel.is())
    {
        pRange 	= reinterpret_cast< SwXTextRange * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXTextRange::getUnoTunnelId()) ));
        pCursor = reinterpret_cast< OTextCursorHelper * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( OTextCursorHelper::getUnoTunnelId()) ));
        pPortion = reinterpret_cast< SwXTextPortion * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXTextPortion::getUnoTunnelId()) ));
        pText 	= reinterpret_cast< SwXText * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXText::getUnoTunnelId()) ));
        pPara 	= reinterpret_cast< SwXParagraph * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXParagraph::getUnoTunnelId()) ));
    }

    //if it's a text cursor then create a temporary cursor there and re-use the pCursor variable
    uno::Reference< XTextCursor > xTextCursor;
    if(pText)
    {
        xTextCursor = pText->createCursor();
        xTextCursor->gotoEnd(sal_True);
        uno::Reference<XUnoTunnel> xCrsrTunnel( xTextCursor, UNO_QUERY);
        pCursor = reinterpret_cast<  OTextCursorHelper * >(
                sal::static_int_cast< sal_IntPtr >( xCrsrTunnel->getSomething( OTextCursorHelper::getUnoTunnelId()) ));
    }
    if(pRange && pRange->GetDoc() == rToFill.GetDoc())
    {
        bRet = pRange->GetPositions(rToFill);
    }
    else
    {
        SwDoc* pDoc = 0;
        const SwPaM* pUnoCrsr = 0;
        if (pPara)
        {
            pUnoCrsr = pPara->GetCrsr();
            if (pUnoCrsr)
                pDoc = pUnoCrsr->GetDoc();
        }
        else
        {
            pDoc = pCursor ? pCursor->GetDoc() : NULL;
            if ( !pDoc )
                pDoc = pPortion ? pPortion->GetCrsr()->GetDoc() : NULL;
            pUnoCrsr = pCursor ? pCursor->GetPaM() : pPortion ? pPortion->GetCrsr() : 0;
        }
        if(pUnoCrsr && pDoc == rToFill.GetDoc())
            {
                DBG_ASSERT((SwPaM*)pUnoCrsr->GetNext() == pUnoCrsr, "was machen wir mit Ringen?" )
                bRet = sal_True;
                *rToFill.GetPoint() = *pUnoCrsr->GetPoint();
                if(pPara)
                {
                    // set selection to the whole paragraph
                    rToFill.SetMark();
                    rToFill.GetMark()->nContent =
                        pUnoCrsr->GetNode()->GetTxtNode()->GetTxt().Len();
                }
                else if(pUnoCrsr->HasMark())
                {
                    rToFill.SetMark();
                    *rToFill.GetMark() = *pUnoCrsr->GetMark();
                }
                else
                    rToFill.DeleteMark();
            }
    }
    return bRet;
}
/* -----------------24.02.99 14:18-------------------
 * Der StartNode muss in einem existierenden Header/Footen liegen
 * --------------------------------------------------*/
sal_Bool lcl_IsStartNodeInFormat(sal_Bool bHeader, SwStartNode* pSttNode,
    const SwFrmFmt* pFrmFmt, SwFrmFmt*& rpFormat)
{
    sal_Bool bRet = sal_False;
    const SfxItemSet& rSet = pFrmFmt->GetAttrSet();
    const SfxPoolItem* pItem;
    SwFrmFmt* pHeadFootFmt;
    if(SFX_ITEM_SET == rSet.GetItemState( static_cast< USHORT >(bHeader ? RES_HEADER : RES_FOOTER), sal_True, &pItem) &&
            0 != (pHeadFootFmt = bHeader ?
                    ((SwFmtHeader*)pItem)->GetHeaderFmt() :
                                ((SwFmtFooter*)pItem)->GetFooterFmt()))
    {
        const SwFmtCntnt& rFlyCntnt = pHeadFootFmt->GetCntnt();
        const SwNode& rNode = rFlyCntnt.GetCntntIdx()->GetNode();
        const SwStartNode* pCurSttNode = rNode.FindSttNodeByType(
            bHeader ? SwHeaderStartNode : SwFooterStartNode);
        if(pCurSttNode && pCurSttNode == pSttNode)
        {
            bRet = sal_True;
            rpFormat = pHeadFootFmt;
        }
    }
    return bRet;
}
/* -----------------03.11.98 15:58-------------------
 *
 * --------------------------------------------------*/
uno::Reference< XTextRange >  SwXTextRange::CreateTextRangeFromPosition(SwDoc* pDoc,
                        const SwPosition& rPos, const SwPosition* pMark)
{
    uno::Reference< XTextRange >  aRet;
    SwUnoCrsr* pNewCrsr = pDoc->CreateUnoCrsr(rPos, sal_False);
    if(pMark)
    {
        pNewCrsr->SetMark();
        *pNewCrsr->GetMark() = *pMark;
    }
    uno::Reference< XText >  xParentText;
    //jetzt besorgen wir uns mal den Parent:
    SwStartNode* pSttNode = rPos.nNode.GetNode().StartOfSectionNode();
    while(pSttNode && pSttNode->IsSectionNode())
    {
        pSttNode = pSttNode->StartOfSectionNode();
    }
    SwStartNodeType eType = pSttNode->GetStartNodeType();
    switch(eType)
    {
        case SwTableBoxStartNode:
        {
            const SwTableNode* pTblNode = pSttNode->FindTableNode();
            SwFrmFmt* pTableFmt = (SwFrmFmt*)pTblNode->GetTable().GetFrmFmt();
            SwTableBox* pBox = pSttNode->GetTblBox();

            if( pBox )
                aRet = new SwXTextRange(*pTableFmt, *pBox, *pNewCrsr);
            else
                aRet = new SwXTextRange(*pTableFmt, *pSttNode, *pNewCrsr);
        }
        break;
        case SwFlyStartNode:
        {
            SwFrmFmt* pFmt;
            if(0 != (pFmt = pSttNode->GetFlyFmt()))
            {
                aRet = new SwXTextRange(*pFmt, *pNewCrsr);

            }
        }
        break;
        case SwHeaderStartNode:
        case SwFooterStartNode:
        {
            sal_Bool bHeader = SwHeaderStartNode == eType;
            sal_uInt16 nPDescCount = pDoc->GetPageDescCnt();
            for(sal_uInt16 i = 0; i < nPDescCount; i++)
            {
                const SwPageDesc& rDesc = const_cast<const SwDoc*>(pDoc)
                    ->GetPageDesc( i );
                const SwFrmFmt* pFrmFmtMaster = &rDesc.GetMaster();
                const SwFrmFmt* pFrmFmtLeft = &rDesc.GetLeft();

                SwFrmFmt* pHeadFootFmt = 0;
                if(!lcl_IsStartNodeInFormat(bHeader, pSttNode, pFrmFmtMaster, pHeadFootFmt))
                    lcl_IsStartNodeInFormat(bHeader, pSttNode, pFrmFmtLeft, pHeadFootFmt);

                if(pHeadFootFmt)
                {
                    SwXHeadFootText* pxHdFt = (SwXHeadFootText*)SwClientIter( *pHeadFootFmt ).
                                    First( TYPE( SwXHeadFootText ));
                    xParentText = pxHdFt;
                    if(!pxHdFt)
                        xParentText = new SwXHeadFootText(*pHeadFootFmt, bHeader);
                    break;
                }
            }
        }
        break;
        case SwFootnoteStartNode:
        {
            sal_uInt16 n, nFtnCnt = pDoc->GetFtnIdxs().Count();
            uno::Reference< XFootnote >  xRef;
            for( n = 0; n < nFtnCnt; ++n )
            {
                const SwTxtFtn* pTxtFtn = pDoc->GetFtnIdxs()[ n ];
                const SwFmtFtn& rFtn = pTxtFtn->GetFtn();
                pTxtFtn = rFtn.GetTxtFtn();
#if OSL_DEBUG_LEVEL > 1
                const SwStartNode* pTmpSttNode = pTxtFtn->GetStartNode()->GetNode().
                                FindSttNodeByType(SwFootnoteStartNode);
                (void)pTmpSttNode;
#endif

                if(pSttNode == pTxtFtn->GetStartNode()->GetNode().
                                    FindSttNodeByType(SwFootnoteStartNode))
                {
                    xParentText = ((SwUnoCallBack*)pDoc->GetUnoCallBack())->
                                                            GetFootnote(rFtn);
                    if(!xParentText.is())
                        xParentText = new SwXFootnote(pDoc, rFtn);
                    break;
                }
            }
        }
        break;
        default:
        {
            // dann ist es der Body-Text
            uno::Reference<frame::XModel> xModel = pDoc->GetDocShell()->GetBaseModel();
            uno::Reference< XTextDocument > xDoc(
                xModel, uno::UNO_QUERY);
            xParentText = xDoc->getText();
        }
    }
    if(!aRet.is())
        aRet = new SwXTextRange(*pNewCrsr, xParentText);
    delete pNewCrsr;
    return aRet;
}
/* -----------------------------03.04.00 09:11--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< XEnumeration >  SAL_CALL SwXTextRange::createContentEnumeration(
        const OUString& rServiceName)
                throw( RuntimeException )
{
    SwBookmark* pBkm = GetBookmark();
    if( !pBkm || COMPARE_EQUAL != rServiceName.compareToAscii("com.sun.star.text.TextContent") )
        throw RuntimeException();

    const SwPosition& rPoint = pBkm->GetBookmarkPos();
    const SwPosition* pMark = pBkm->GetOtherBookmarkPos();
    SwUnoCrsr* pNewCrsr = pDoc->CreateUnoCrsr(rPoint, FALSE);
    if(pMark && *pMark != rPoint)
    {
        pNewCrsr->SetMark();
        *pNewCrsr->GetMark() = *pMark;
    }
    uno::Reference< XEnumeration > xRet = new SwXParaFrameEnumeration(*pNewCrsr, PARAFRAME_PORTION_TEXTRANGE);
    delete pNewCrsr;
    return xRet;
}
/* -----------------------------07.03.01 14:55--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< XEnumeration >  SwXTextRange::createEnumeration(void) throw( RuntimeException )
{
    SwBookmark* pBkm = GetBookmark();
    if( !pBkm  )
        throw RuntimeException();
    const SwPosition& rPoint = pBkm->GetBookmarkPos();
    const SwPosition* pMark = pBkm->GetOtherBookmarkPos();
    SwUnoCrsr* pNewCrsr = pDoc->CreateUnoCrsr(rPoint, FALSE);
    if(pMark && *pMark != rPoint)
    {
        pNewCrsr->SetMark();
        *pNewCrsr->GetMark() = *pMark;
    }
    uno::Reference<XUnoTunnel> xTunnel(xParentText, UNO_QUERY);
    SwXText* pParentText = 0;
    if(xTunnel.is())
    {
        pParentText = reinterpret_cast< SwXText * >(
                sal::static_int_cast< sal_IntPtr >( xTunnel->getSomething(SwXText::getUnoTunnelId()) ));
    }
    DBG_ASSERT(pParentText, "parent is not a SwXText")
    CursorType eSetType = RANGE_IN_CELL == eRangePosition ? CURSOR_SELECTION_IN_TABLE : CURSOR_SELECTION;
    uno::Reference< XEnumeration > xRet = new SwXParagraphEnumeration(pParentText, *pNewCrsr, eSetType);
    return xRet;
}
/* -----------------------------07.03.01 15:43--------------------------------

 ---------------------------------------------------------------------------*/
uno::Type  SwXTextRange::getElementType(void) throw( RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/* -----------------------------07.03.01 15:43--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwXTextRange::hasElements(void) throw( RuntimeException )
{
    return sal_True;
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
uno::Reference< XPropertySetInfo > SAL_CALL SwXTextRange::getPropertySetInfo(  ) throw(RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static uno::Reference< XPropertySetInfo > xRef =
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
    const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::removePropertyChangeListener(
    const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::addVetoableChangeListener(
    const OUString& /*PropertyName*/, const uno::Reference< XVetoableChangeListener >& /*aListener*/ )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    DBG_WARNING("not implemented")
}
/*-- 03.05.00 12:41:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SwXTextRange::removeVetoableChangeListener(
    const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener >& /*aListener*/ )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
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
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast <sal_IntPtr >(this) );
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
    vos::OGuard aGuard(Application::GetSolarMutex());
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
    if(pArr && 0 <= nIndex && nIndex < pArr->Count())
    {
        XTextRangeRefPtr pRef = pArr->GetObject( USHORT( nIndex ));
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

            uno::Reference< XTextRange >* pPtr =
                new uno::Reference<XTextRange>( SwXTextRange::CreateTextRangeFromPosition(PUNOPAM->GetDoc(),
                        *PUNOPAM->GetPoint(), PUNOPAM->GetMark()));
//				new uno::Reference<XTextRange>( SwXTextRange::createTextRangeFromPaM(*PUNOPAM, xParentText));
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

/* -----------------11.12.98 10:07-------------------
 *
 * --------------------------------------------------*/
void SwXTextCursor::SetString(SwCursor& rCrsr, const OUString& rString)
{
    // Start/EndAction
    SwDoc* pDoc = rCrsr.GetDoc();
    UnoActionContext aAction(pDoc);
    String aText(rString);
    xub_StrLen nTxtLen = aText.Len();
    pDoc->StartUndo(UNDO_INSERT, NULL);
    if(rCrsr.HasMark())
        pDoc->DeleteAndJoin(rCrsr);
    if(nTxtLen)
    {
        if( !SwUnoCursorHelper::DocInsertStringSplitCR( *pDoc, rCrsr, aText ) )
        {
            DBG_ASSERT( sal_False, "DocInsertStringSplitCR" )
        }
        SwXTextCursor::SelectPam(rCrsr, sal_True);
        rCrsr.Left(nTxtLen, CRSR_SKIP_CHARS, FALSE, FALSE);
    }
    pDoc->EndUndo(UNDO_INSERT, NULL);
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
        ::CollectFrameAtNode( *this, rUnoCrsr.GetPoint()->nNode,
                                aFrameArr, FALSE );
    else if(pFmt)
    {
        //jetzt einen SwDepend anlegen und in das Array einfuegen
        SwDepend* pNewDepend = new SwDepend(this, pFmt);
        aFrameArr.C40_INSERT(SwDepend, pNewDepend, aFrameArr.Count());
    }
    else if((PARAFRAME_PORTION_CHAR == nParaFrameMode) ||
            (PARAFRAME_PORTION_TEXTRANGE == nParaFrameMode))
    {
        if(PARAFRAME_PORTION_TEXTRANGE == nParaFrameMode)
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
                    pUnoCrsr->Right(1, CRSR_SKIP_CHARS, FALSE, FALSE);
                }
                while(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark());
            }
        }
        FillFrame(*pUnoCrsr);
    }
}
/*-- 23.03.99 13:22:30---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXParaFrameEnumeration::~SwXParaFrameEnumeration()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
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
            xNextObject = uno::Reference< XTextContent >(pSdr->getUnoShape(), uno::UNO_QUERY);
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
// -----------------------------------------------------------------------------
IMPLEMENT_FORWARD_REFCOUNT( SwXTextCursor,SwXTextCursor_Base )

uno::Any SAL_CALL SwXTextCursor::queryInterface( const uno::Type& _rType ) throw (uno::RuntimeException)
{
    if( _rType == ::getCppuType( (uno::Reference<XUnoTunnel>*)0)  )
        return OTextCursorHelper::queryInterface( _rType );
    else
        return SwXTextCursor_Base::queryInterface( _rType );
}


