/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoobj.cxx,v $
 * $Revision: 1.108 $
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


#include <osl/endian.h>
#include <rtl/ustrbuf.hxx>
#include <swtypes.hxx>
#include <hintids.hxx>
#include <cmdid.h>
#include <hints.hxx>
#include <bookmrk.hxx>
#include <frmfmt.hxx>
#include <doc.hxx>
#include <istyleaccess.hxx>
#include <ndtxt.hxx>
#include <ndnotxt.hxx>
#include <unocrsrhelper.hxx>
#include <swundo.hxx>
#include <rootfrm.hxx>
#include <flyfrm.hxx>
#include <ftnidx.hxx>
#include <sfx2/linkmgr.hxx>
#include <docary.hxx>
#include <paratr.hxx>
#include <tools/urlobj.hxx>
#include <pam.hxx>
#include <tools/cachestr.hxx>
#include <shellio.hxx>
#include <swerror.h>
#include <swtblfmt.hxx>
#include <fmtruby.hxx>
#include <fmthbsh.hxx>
#include <docsh.hxx>
#include <docstyle.hxx>
#include <charfmt.hxx>
#include <txtfld.hxx>
#include <fmtfld.hxx>
#include <fmtpdsc.hxx>
#include <pagedesc.hxx>
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#include <poolfmt.hxx>
#include <edimp.hxx>
#include <fchrfmt.hxx>
#include <fmtautofmt.hxx>
#include <cntfrm.hxx>
#include <pagefrm.hxx>
#include <doctxm.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/fcontnr.hxx>
#include <fmtrfmrk.hxx>
#include <txtrfmrk.hxx>
#include <unoclbck.hxx>
#include <unoobj.hxx>
#include <unomap.hxx>
#include <unosett.hxx>
#include <unoprnms.hxx>
#include <unotbl.hxx>
#include <unodraw.hxx>
#include <unocoll.hxx>
#include <unostyle.hxx>
#include <unofield.hxx>
#include <fmtanchr.hxx>
#include <svx/flstitem.hxx>
#include <svtools/ctrltool.hxx>
#include <flypos.hxx>
#include <txtftn.hxx>
#include <fmtftn.hxx>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>
#include <com/sun/star/style/PageStyleLayout.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <unoidx.hxx>
#include <unoframe.hxx>
#include <fmthdft.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <fmtflcnt.hxx>
#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svtools/svstdarr.hxx>
#include <svx/brshitem.hxx>
#include <fmtclds.hxx>
#include <dcontact.hxx>
#include <SwStyleNameMapper.hxx>
#include <crsskip.hxx>
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#include <memory>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;
using std::auto_ptr;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;

/****************************************************************************
    static methods
****************************************************************************/
uno::Sequence< sal_Int8 >  CreateUnoTunnelId()
{
    static osl::Mutex aCreateMutex;
    osl::Guard<osl::Mutex> aGuard( aCreateMutex );
    uno::Sequence< sal_Int8 > aSeq( 16 );
    rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0,	sal_True );
    return aSeq;
}
/****************************************************************************
    Hilfsklassen
****************************************************************************/

SwParaSelection::SwParaSelection(SwUnoCrsr* pCrsr) :
    pUnoCrsr(pCrsr)
{
    if(pUnoCrsr->HasMark())
        pUnoCrsr->DeleteMark();
    // steht er schon am Anfang?
    if(pUnoCrsr->GetPoint()->nContent != 0)
        pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    // oder gleichzeitig am Ende?
    if(pUnoCrsr->GetPoint()->nContent != pUnoCrsr->GetCntntNode()->Len())
    {
        pUnoCrsr->SetMark();
        pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
    }
}

SwParaSelection::~SwParaSelection()
{
    if(pUnoCrsr->GetPoint()->nContent != 0)
    {
        pUnoCrsr->DeleteMark();
        pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    }
}
/* -----------------13.05.98 12:15-------------------
 *
 * --------------------------------------------------*/
SwUnoInternalPaM::SwUnoInternalPaM(SwDoc& rDoc) :
    SwPaM(rDoc.GetNodes())
{
}
SwUnoInternalPaM::~SwUnoInternalPaM()
{
    while( GetNext() != this)
    {
        delete GetNext();
    }
}

SwUnoInternalPaM&	SwUnoInternalPaM::operator=(const SwPaM& rPaM)
{
    const SwPaM* pTmp = &rPaM;
    *GetPoint() = *rPaM.GetPoint();
    if(rPaM.HasMark())
    {
        SetMark();
        *GetMark() = *rPaM.GetMark();
    }
    else
        DeleteMark();
    while(&rPaM != (pTmp = (const SwPaM*)pTmp->GetNext()))
    {
        if(pTmp->HasMark())
            new SwPaM(*pTmp->GetMark(), *pTmp->GetPoint(), this);
        else
            new SwPaM(*pTmp->GetPoint(), this);
    }
    return *this;
}
/****************************************************************************
    ActionContext
****************************************************************************/
UnoActionContext::UnoActionContext(SwDoc* pDc) :
    pDoc(pDc)
{
    SwRootFrm* pRootFrm = pDoc->GetRootFrm();
    if(pRootFrm)
        pRootFrm->StartAllAction();
}
/*-----------------04.03.98 11:56-------------------

--------------------------------------------------*/
UnoActionContext::~UnoActionContext()
{
    //das Doc kann hier schon entfernt worden sein
    if(pDoc)
    {
        SwRootFrm* pRootFrm = pDoc->GetRootFrm();
        if(pRootFrm)
            pRootFrm->EndAllAction();
    }
}

/****************************************************************************
    ActionRemoveContext
****************************************************************************/
UnoActionRemoveContext::UnoActionRemoveContext(SwDoc* pDc) :
    pDoc(pDc)
{
    SwRootFrm* pRootFrm = pDoc->GetRootFrm();
    if(pRootFrm)
        pRootFrm->UnoRemoveAllActions();
}
/* -----------------07.07.98 12:05-------------------
 *
 * --------------------------------------------------*/
UnoActionRemoveContext::~UnoActionRemoveContext()
{
    SwRootFrm* pRootFrm = pDoc->GetRootFrm();
    if(pRootFrm)
        pRootFrm->UnoRestoreAllActions();

}

/*-----------------09.03.98 08:29-------------------

--------------------------------------------------*/
void SwXTextCursor::SelectPam(SwPaM& rCrsr, sal_Bool bExpand)
{
    if(bExpand)
    {
        if(!rCrsr.HasMark())
            rCrsr.SetMark();
    }
    else if(rCrsr.HasMark())
        rCrsr.DeleteMark();

}

/* -----------------20.05.98 14:59-------------------
 *
 * --------------------------------------------------*/
void SwXTextCursor::getTextFromPam(SwPaM& aCrsr, OUString& rBuffer)
{
    if(!aCrsr.HasMark())
        return;
    SvCacheStream aStream( 20480 );
#ifdef OSL_BIGENDIAN
    aStream.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
#else
    aStream.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
#endif
    WriterRef xWrt;
    // TODO/MBA: looks like a BaseURL doesn't make sense here
    SwIoSystem::GetWriter( C2S(FILTER_TEXT_DLG), String(), xWrt );
    if( xWrt.Is() )
    {
        SwWriter aWriter( aStream, aCrsr );
        xWrt->bASCII_NoLastLineEnd = sal_True;
        SwAsciiOptions aOpt = xWrt->GetAsciiOptions();
        aOpt.SetCharSet( RTL_TEXTENCODING_UNICODE );
        xWrt->SetAsciiOptions( aOpt );
        xWrt->bUCS2_WithStartChar = FALSE;
        // --> FME #i68522#
        const BOOL bOldShowProgress = xWrt->bShowProgress;
        xWrt->bShowProgress = FALSE;
        // <--

        long lLen;
        if( !IsError( aWriter.Write( xWrt ) ) &&
            0x7ffffff > (( lLen  = aStream.GetSize() )
                                    / sizeof( sal_Unicode )) + 1 )
        {
            aStream << (sal_Unicode)'\0';

            long lUniLen = (lLen / sizeof( sal_Unicode ));
            ::rtl::OUStringBuffer aStrBuffer( lUniLen );
            aStream.Seek( 0 );
            aStream.ResetError();
            while(lUniLen)
            {
                String sBuf;
                sal_Int32 nLocalLen = 0;
                if( lUniLen >= STRING_MAXLEN )
                    nLocalLen =  STRING_MAXLEN - 1;
                else
                    nLocalLen = lUniLen;
                sal_Unicode* pStrBuf = sBuf.AllocBuffer( xub_StrLen( nLocalLen + 1));
                aStream.Read( pStrBuf, 2 * nLocalLen );
                pStrBuf[ nLocalLen ] = '\0';
                aStrBuffer.append( pStrBuf, nLocalLen );
                lUniLen -= nLocalLen;
            }
            rBuffer = aStrBuffer.makeStringAndClear();
        }
        xWrt->bShowProgress = bOldShowProgress;
    }
}

/* -----------------06.07.98 07:33-------------------
 *
 * --------------------------------------------------*/
void lcl_setCharStyle(SwDoc* pDoc, const uno::Any aValue, SfxItemSet& rSet)
     throw (lang::IllegalArgumentException)
{
    SwDocShell* pDocSh = pDoc->GetDocShell();
    if(pDocSh)
    {
        OUString uStyle;
        aValue >>= uStyle;
        String sStyle;
        SwStyleNameMapper::FillUIName(uStyle, sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT, sal_True );
        SwDocStyleSheet* pStyle =
            (SwDocStyleSheet*)pDocSh->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_CHAR);
        if(pStyle)
        {
            SwFmtCharFmt aFmt(pStyle->GetCharFmt());
            rSet.Put(aFmt);
        }
        else
        {
             throw lang::IllegalArgumentException();
        }

    }
};
/* -----------------08.06.06 10:43-------------------
 *
 * --------------------------------------------------*/
void lcl_setAutoStyle(IStyleAccess& rStyleAccess, const uno::Any aValue, SfxItemSet& rSet, bool bPara )
     throw (lang::IllegalArgumentException)
{
    OUString uStyle;
    aValue >>= uStyle;
    String sStyle;
    StylePool::SfxItemSet_Pointer_t pStyle = bPara ?
        rStyleAccess.getByName(uStyle, IStyleAccess::AUTO_STYLE_PARA ):
        rStyleAccess.getByName(uStyle, IStyleAccess::AUTO_STYLE_CHAR );
    if(pStyle.get())
    {
        SwFmtAutoFmt aFmt( bPara ? sal::static_int_cast< USHORT >(RES_AUTO_STYLE)
                                 : sal::static_int_cast< USHORT >(RES_TXTATR_AUTOFMT) );
        aFmt.SetStyleHandle( pStyle );
        rSet.Put(aFmt);
    }
    else
    {
         throw lang::IllegalArgumentException();
    }
};
/* -----------------30.06.98 08:46-------------------
 *
 * --------------------------------------------------*/
void lcl_SetTxtFmtColl(const uno::Any& rAny, SwPaM& rPaM)
    throw (lang::IllegalArgumentException)
{
    SwDoc* pDoc = rPaM.GetDoc();
    SwDocShell* pDocSh = pDoc->GetDocShell();
    if(!pDocSh)
        return;
    OUString uStyle;
    rAny >>= uStyle;
    String sStyle;
    SwStyleNameMapper::FillUIName(uStyle, sStyle, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
    SwDocStyleSheet* pStyle =
                    (SwDocStyleSheet*)pDocSh->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_PARA);
    if(pStyle)
    {
        SwTxtFmtColl *pLocal = pStyle->GetCollection();
        UnoActionContext aAction(pDoc);
        pDoc->StartUndo( UNDO_START, NULL );
        FOREACHUNOPAM_START(&rPaM)
            pDoc->SetTxtFmtColl(*PUNOPAM, pLocal);
        FOREACHUNOPAM_END()
        pDoc->EndUndo( UNDO_END, NULL );
    }
    else
    {
        throw lang::IllegalArgumentException();
    }

}
/* -----------------06.07.98 07:38-------------------
 *
 * --------------------------------------------------*/
 void lcl_setPageDesc(SwDoc* pDoc, const uno::Any& aValue, SfxItemSet& rSet)
 {
    if(aValue.getValueType() != ::getCppuType((const OUString*)0))
        return;
    SwFmtPageDesc *pNewDesc = 0 ;
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState( RES_PAGEDESC, sal_True, &pItem ) )
    {
        pNewDesc = new SwFmtPageDesc(*((SwFmtPageDesc*)pItem));
    }
    if(!pNewDesc)
        pNewDesc = new SwFmtPageDesc();
    OUString uDescName;
    aValue >>= uDescName;
    String sDescName;
    SwStyleNameMapper::FillUIName(uDescName, sDescName, nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC, sal_True );
    if(!pNewDesc->GetPageDesc() || pNewDesc->GetPageDesc()->GetName() != sDescName)
    {
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
            rSet.ClearItem(RES_BREAK);
            rSet.Put(SwFmtPageDesc());
        }
        else
            rSet.Put(*pNewDesc);
    }
    delete pNewDesc;
}
/* -----------------30.06.98 10:29-------------------
 *
 * --------------------------------------------------*/
void lcl_SetNodeNumStart( SwPaM& rCrsr, uno::Any aValue )
{
    sal_Int16 nTmp = 1;
    aValue >>= nTmp;
    sal_uInt16 nStt = (nTmp < 0 ? USHRT_MAX : (sal_uInt16)nTmp);
    SwDoc* pDoc = rCrsr.GetDoc();
    UnoActionContext aAction(pDoc);

    if( rCrsr.GetNext() != &rCrsr )			// Mehrfachselektion ?
    {
        pDoc->StartUndo( UNDO_START, NULL );
        SwPamRanges aRangeArr( rCrsr );
        SwPaM aPam( *rCrsr.GetPoint() );
        for( sal_uInt16 n = 0; n < aRangeArr.Count(); ++n )
        {
          pDoc->SetNumRuleStart(*aRangeArr.SetPam( n, aPam ).GetPoint());
          pDoc->SetNodeNumStart(*aRangeArr.SetPam( n, aPam ).GetPoint(),
                    nStt );
        }
        pDoc->EndUndo( UNDO_END, NULL);
    }
    else
    {
        pDoc->SetNumRuleStart( *rCrsr.GetPoint());
        pDoc->SetNodeNumStart( *rCrsr.GetPoint(), nStt );
    }
}

/* -----------------17.09.98 09:44-------------------
 *
 * --------------------------------------------------*/
sal_Bool lcl_setCrsrPropertyValue(const SfxItemPropertyMap* pMap,
                                SwPaM& rPam,
                                SfxItemSet& rItemSet,
                                const uno::Any& aValue ) throw (lang::IllegalArgumentException)
{
    sal_Bool bRet = sal_True;
    if(0 ==(pMap->nFlags&PropertyAttribute::MAYBEVOID) &&
        aValue.getValueType() == ::getCppuVoidType())
        bRet = sal_False;
    else
    {
        switch(pMap->nWID)
        {
            case RES_TXTATR_CHARFMT:
                lcl_setCharStyle(rPam.GetDoc(), aValue, rItemSet );
            break;
            case RES_TXTATR_AUTOFMT:
                lcl_setAutoStyle(rPam.GetDoc()->GetIStyleAccess(), aValue, rItemSet, false );
            break;
            case FN_UNO_CHARFMT_SEQUENCE:
            {
                Sequence<OUString> aCharStyles;
                if(aValue >>= aCharStyles)
                {
                    for(sal_Int32 nStyle = 0; nStyle < aCharStyles.getLength(); nStyle++)
                    {
                        Any aStyle;
                        rPam.GetDoc()->StartUndo( UNDO_START, NULL);
                        aStyle <<= aCharStyles.getConstArray()[nStyle];
                        //create a local set and apply each format directly
                        SfxItemSet aSet(rPam.GetDoc()->GetAttrPool(), RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT );
                        lcl_setCharStyle(rPam.GetDoc(), aStyle, aSet );
                        //the first style should replace the current attributes, all other have to be added
                        SwXTextCursor::SetCrsrAttr(rPam, aSet, nStyle ? CRSR_ATTR_MODE_DONTREPLACE : 0);
                        rPam.GetDoc()->EndUndo( UNDO_START, NULL );
                    }
                }
                else
                    bRet = sal_False;
            }
            break;
            case FN_UNO_PARA_STYLE :
                lcl_SetTxtFmtColl(aValue, rPam);
            break;
            case RES_AUTO_STYLE:
                lcl_setAutoStyle(rPam.GetDoc()->GetIStyleAccess(), aValue, rItemSet, true );
            break;
            case FN_UNO_PAGE_STYLE :
            break;
            case FN_UNO_NUM_START_VALUE  :
                lcl_SetNodeNumStart( rPam, aValue );
            break;
            case FN_UNO_PARA_CHAPTER_NUMBERING_LEVEL:
                {
/*                  Will be used in OOo 3.0
                    SwTxtNode * pTmpNode = rPam.GetNode()->GetTxtNode();

                    BYTE nLevel;
                    aValue >>= nLevel;

                    if ( pTmpNode )
                    {
                        pTmpNode->SetOutlineLevel(nLevel);

                        // --> OD 2005-09-01 #i53198# - update outline nodes array
                        rPam.GetDoc()->GetNodes().UpdateOutlineNode( *pTmpNode );
                        // <--
                    }*/
                }
                break;
            case FN_UNO_NUM_LEVEL  :
            case FN_UNO_IS_NUMBER  :
            {
                SwTxtNode* pTxtNd = rPam.GetNode()->GetTxtNode();
                const SwNumRule* pRule = pTxtNd->GetNumRule();
                // hier wird Multiselektion nicht beruecksichtigt

                if( FN_UNO_NUM_LEVEL == pMap->nWID  &&  pRule != NULL )
                {
                    sal_Int16 nLevel = 0;
                    aValue >>= nLevel;

                    pTxtNd->SetLevel(nLevel);

                }
                else if( FN_UNO_IS_NUMBER == pMap->nWID )
                {
                    BOOL bIsNumber = *(sal_Bool*) aValue.getValue();
                    if(!bIsNumber)
                        pTxtNd->SetCounted(FALSE);
                }
                //PROPERTY_MAYBEVOID!
            }
            break;
            case FN_NUMBER_NEWSTART :
            {
                sal_Bool bVal = sal_False;
                if (aValue >>= bVal)
                    rPam.GetDoc()->SetNumRuleStart(*rPam.GetPoint(), bVal);
                else
                    throw lang::IllegalArgumentException();
            }
            break;
            case FN_UNO_NUM_RULES:
                SwUnoCursorHelper::setNumberingProperty(aValue, rPam);
            break;
            case RES_PARATR_DROP:
            {
                if( MID_DROPCAP_CHAR_STYLE_NAME == pMap->nMemberId)
                {
                    OUString uStyle;
                    if(aValue >>= uStyle)
                    {
                        String sStyle;
                        SwStyleNameMapper::FillUIName(uStyle, sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT, sal_True );
                        SwDoc* pDoc = rPam.GetDoc();
                        //default character style mustn't be set as default format
                        SwDocStyleSheet* pStyle =
                            (SwDocStyleSheet*)pDoc->GetDocShell()->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_CHAR);
                        SwFmtDrop* pDrop = 0;
                        if(pStyle &&
                                ((SwDocStyleSheet*)pStyle)->GetCharFmt() != pDoc->GetDfltCharFmt())
                        {
                            const SfxPoolItem* pItem;
                            if(SFX_ITEM_SET == rItemSet.GetItemState( RES_PARATR_DROP, sal_True, &pItem ) )
                                pDrop = new SwFmtDrop(*((SwFmtDrop*)pItem));
                            if(!pDrop)
                                pDrop = new SwFmtDrop();
                            rtl::Reference< SwDocStyleSheet > xStyle( new SwDocStyleSheet( *(SwDocStyleSheet*)pStyle ) );
                            pDrop->SetCharFmt(xStyle->GetCharFmt());
                        }
                        else
                             throw lang::IllegalArgumentException();
                        rItemSet.Put(*pDrop);
                        delete pDrop;
                    }
                    else
                        throw lang::IllegalArgumentException();
                }
                else
                    bRet = sal_False;
            }
            break;
            case RES_TXTATR_CJK_RUBY:
                if(MID_RUBY_CHARSTYLE == pMap->nMemberId )
                {
                    OUString sTmp;
                    if(aValue >>= sTmp)
                    {
                        SwFmtRuby* pRuby = 0;
                        const SfxPoolItem* pItem;
                        if(SFX_ITEM_SET == rItemSet.GetItemState( RES_TXTATR_CJK_RUBY, sal_True, &pItem ) )
                            pRuby = new SwFmtRuby(*((SwFmtRuby*)pItem));
                        if(!pRuby)
                            pRuby = new SwFmtRuby(aEmptyStr);
                        String sStyle;
                        SwStyleNameMapper::FillUIName(sTmp, sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT, sal_True );
                         pRuby->SetCharFmtName( sStyle );
                        pRuby->SetCharFmtId( 0 );
                        if(sStyle.Len() > 0)
                        {
                            sal_uInt16 nId = SwStyleNameMapper::GetPoolIdFromUIName( sStyle, nsSwGetPoolIdFromName::GET_POOLID_CHRFMT );
                            pRuby->SetCharFmtId(nId);
                        }
                        rItemSet.Put(*pRuby);
                        delete pRuby;
                    }
                    else
                        throw lang::IllegalArgumentException();
                    bRet = sal_True;
                }
                else
                    bRet = sal_False;
            break;
            case RES_PAGEDESC      :
            if(MID_PAGEDESC_PAGEDESCNAME == pMap->nMemberId )
            {
                lcl_setPageDesc(rPam.GetDoc(), aValue, rItemSet);
                break;
            }
            //hier kein break
            default: bRet = sal_False;
        }
    }
return bRet;
}

/* -----------------30.06.98 08:39-------------------
 *
 * --------------------------------------------------*/
SwFmtColl* SwXTextCursor::GetCurTxtFmtColl(SwPaM& rPam, BOOL bConditional)
{
    static const sal_uInt16 nMaxLookup = 1000;
    SwFmtColl *pFmt = 0;

//	if ( GetCrsrCnt() > nMaxLookup )
//		return 0;
    sal_Bool bError = sal_False;
    FOREACHUNOPAM_START(&rPam)

        ULONG nSttNd = PUNOPAM->GetMark()->nNode.GetIndex(),
              nEndNd = PUNOPAM->GetPoint()->nNode.GetIndex();
        xub_StrLen nSttCnt = PUNOPAM->GetMark()->nContent.GetIndex(),
                nEndCnt = PUNOPAM->GetPoint()->nContent.GetIndex();

        if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
        {
            sal_uInt32 nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
            nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (sal_uInt16)nTmp;
        }

        if( nEndNd - nSttNd >= nMaxLookup )
        {
            pFmt = 0;
            break;
        }

        const SwNodes& rNds = rPam.GetDoc()->GetNodes();
        for( ULONG n = nSttNd; n <= nEndNd; ++n )
        {
            const SwTxtNode* pNd = rNds[ n ]->GetTxtNode();
            if( pNd )
            {
                SwFmtColl* pNdFmt = bConditional ? pNd->GetFmtColl()
                                                    : &pNd->GetAnyFmtColl();
                if( !pFmt )
                    pFmt = pNdFmt;
                else if( pFmt != pNdFmt )
                {
                    bError = sal_True;
                    break;
                }
            }
        }
        if(bError)
            break;
    FOREACHUNOPAM_END()
    return bError ? 0 : pFmt;
}

/* -----------------26.06.98 16:20-------------------
 * 	Hilfsfunktion fuer PageDesc
 * --------------------------------------------------*/
 SwPageDesc*	GetPageDescByName_Impl(SwDoc& rDoc, const String& rName)
 {
    SwPageDesc* pRet = 0;
    sal_uInt16 nDCount = rDoc.GetPageDescCnt();
    sal_uInt16 i;

    for( i = 0; i < nDCount; i++ )
    {
        SwPageDesc* pDsc = &rDoc._GetPageDesc( i );
        if(pDsc->GetName() == rName)
        {
            pRet = pDsc;
            break;
        }
    }
    if(!pRet)
    {
        for(i = RC_POOLPAGEDESC_BEGIN; i <= STR_POOLPAGE_ENDNOTE; ++i)
        {
            const String aFmtName(SW_RES(i));
            if(aFmtName == rName)
            {
                pRet = rDoc.GetPageDescFromPool( static_cast< sal_uInt16 >(RES_POOLPAGE_BEGIN + i - RC_POOLPAGEDESC_BEGIN) );
                break;
            }
        }
    }

    return pRet;
 }
/******************************************************************************
 ******************************************************************************
 ******************************************************************************
 * Taskforce ONE51
 ******************************************************************************/

/******************************************************************
 * SwXTextCursor
 ******************************************************************/

/*-- 09.12.98 14:19:19---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXTextCursor::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextCursor");
}
/*-- 09.12.98 14:19:19---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwXTextCursor::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    String sServiceName(rServiceName);
    return sServiceName.EqualsAscii("com.sun.star.text.TextCursor") ||
         sServiceName.EqualsAscii("com.sun.star.style.CharacterProperties")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesAsian")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesComplex")||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesAsian") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesComplex") ||
        sServiceName.EqualsAscii("com.sun.star.text.TextSortable");
}
/*-- 09.12.98 14:19:19---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence< OUString > SwXTextCursor::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(8);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextCursor");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
    pArray[2] = C2U("com.sun.star.style.CharacterPropertiesAsian");
    pArray[3] = C2U("com.sun.star.style.CharacterPropertiesComplex");
    pArray[4] = C2U("com.sun.star.style.ParagraphProperties");
    pArray[5] = C2U("com.sun.star.style.ParagraphPropertiesAsian");
    pArray[6] = C2U("com.sun.star.style.ParagraphPropertiesComplex");
    pArray[7] = C2U("com.sun.star.text.TextSortable");
    return aRet;
}
/*-- 09.12.98 14:19:19---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextCursor::SwXTextCursor(uno::Reference< XText >  xParent, const SwPosition& rPos,
                    CursorType eSet, SwDoc* pDoc, const SwPosition* pMark) :
    aLstnrCntnr(( util::XSortable*)this),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    xParentText(xParent),
    pLastSortOptions(0),
    eType(eSet),
    mbRemoveUserEvent( false )
{
    SwUnoCrsr* pUnoCrsr = pDoc->CreateUnoCrsr(rPos, sal_False);
    if(pMark)
    {
        pUnoCrsr->SetMark();
        *pUnoCrsr->GetMark() = *pMark;
    }
    pUnoCrsr->Add(this);
}

/* -----------------04.03.99 09:02-------------------
 *
 * --------------------------------------------------*/
SwXTextCursor::SwXTextCursor(uno::Reference< XText >  xParent,
    SwUnoCrsr* pSourceCrsr, CursorType eSet) :
    aLstnrCntnr( (util::XSortable*)this),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    xParentText(xParent),
    pLastSortOptions(0),
    eType(eSet),
    mbRemoveUserEvent( false )
{
    SwUnoCrsr* pUnoCrsr = pSourceCrsr->GetDoc()->CreateUnoCrsr(*pSourceCrsr->GetPoint(), sal_False);
    if(pSourceCrsr->HasMark())
    {
        pUnoCrsr->SetMark();
        *pUnoCrsr->GetMark() = *pSourceCrsr->GetMark();
    }
    pUnoCrsr->Add(this);
}

/*-- 09.12.98 14:19:20---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextCursor::~SwXTextCursor()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // --> FME 2006-03-07 #126177# A user event has been posted in order
    // to delete the SwUnoCursor. Remove the user event if the SwXTextCursor
    // is destroyed before the user event could be handled.
    if ( mbRemoveUserEvent )
    {
        mbRemoveUserEvent = false;
        Application::RemoveUserEvent( mnUserEventId );
    }
    // <--

    SwUnoCrsr* pUnoCrsr = GetCrsr();
    delete pUnoCrsr;
    delete pLastSortOptions;
}
/*-- 09.12.98 14:19:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::DeleteAndInsert(const String& rText)
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        // Start/EndAction
        SwDoc* pDoc = pUnoCrsr->GetDoc();
        UnoActionContext aAction(pDoc);
        xub_StrLen nTxtLen = rText.Len();
        pDoc->StartUndo(UNDO_INSERT, NULL);
        SwCursor *_pStartCrsr = pUnoCrsr;
        do
        {
            if(_pStartCrsr->HasMark())
            {
                pDoc->DeleteAndJoin(*_pStartCrsr);
            }
            if(nTxtLen)
            {
                if( !SwUnoCursorHelper::DocInsertStringSplitCR( *pDoc, *_pStartCrsr, rText ) )
                {
                    DBG_ASSERT( sal_False, "Doc->Insert(Str) failed." )
                }
                SwXTextCursor::SelectPam(*pUnoCrsr, sal_True);
                _pStartCrsr->Left(rText.Len(), CRSR_SKIP_CHARS, FALSE, FALSE);
            }
        } while( (_pStartCrsr=(SwCursor*)_pStartCrsr->GetNext()) != pUnoCrsr );
        pDoc->EndUndo(UNDO_INSERT, NULL);
    }
}
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextCursor::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXTextCursor::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(this) );
    }
    return OTextCursorHelper::getSomething(rId);
}

/*-- 09.12.98 14:18:12---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::collapseToStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(pUnoCrsr->HasMark())
        {
            if(*pUnoCrsr->GetPoint() > *pUnoCrsr->GetMark())
                pUnoCrsr->Exchange();
            pUnoCrsr->DeleteMark();
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/*-- 09.12.98 14:18:14---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::collapseToEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(pUnoCrsr->HasMark())
        {
            if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                pUnoCrsr->Exchange();
            pUnoCrsr->DeleteMark();
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/*-- 09.12.98 14:18:41---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isCollapsed(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_True;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr && pUnoCrsr->GetMark())
    {
        bRet = (*pUnoCrsr->GetPoint() == *pUnoCrsr->GetMark());
    }
    return bRet;
}
/*-- 09.12.98 14:18:42---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::goLeft(sal_Int16 nCount, sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->Left( nCount, CRSR_SKIP_CHARS, FALSE, FALSE);
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}
/*-- 09.12.98 14:18:42---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::goRight(sal_Int16 nCount, sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->Right(nCount, CRSR_SKIP_CHARS, FALSE, FALSE);
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}
/*-- 09.12.98 14:18:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::gotoStart(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(eType == CURSOR_BODY)
        {
            pUnoCrsr->Move( fnMoveBackward, fnGoDoc );
            //check, that the cursor is not in a table
            SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
            SwCntntNode* pCont = 0;
            while( pTblNode )
            {
                pUnoCrsr->GetPoint()->nNode = *pTblNode->EndOfSectionNode();
                pCont = GetDoc()->GetNodes().GoNext(&pUnoCrsr->GetPoint()->nNode);
                pTblNode = pCont ? pCont->FindTableNode() : 0;
            }
            if(pCont)
                pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
            const SwStartNode* pTmp = pUnoCrsr->GetNode()->StartOfSectionNode();
            if(pTmp->IsSectionNode())
            {
                SwSectionNode* pSectionStartNode = (SwSectionNode*)pTmp;
                if(pSectionStartNode->GetSection().IsHiddenFlag())
                {
                    pCont = GetDoc()->GetNodes().GoNextSection(
                                &pUnoCrsr->GetPoint()->nNode, sal_True, sal_False);
                    if(pCont)
                        pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
                }
            }
        }
        else if(eType == CURSOR_FRAME ||
                eType == CURSOR_TBLTEXT ||
                eType == CURSOR_HEADER ||
                eType == CURSOR_FOOTER ||
                eType == CURSOR_FOOTNOTE||
                eType == CURSOR_REDLINE)
        {
            pUnoCrsr->MoveSection( fnSectionCurr, fnSectionStart);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/*-- 09.12.98 14:18:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::gotoEnd(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(eType == CURSOR_BODY)
            pUnoCrsr->Move( fnMoveForward, fnGoDoc );
        else if(eType == CURSOR_FRAME ||
                eType == CURSOR_TBLTEXT ||
                eType == CURSOR_HEADER ||
                eType == CURSOR_FOOTER ||
                eType == CURSOR_FOOTNOTE||
                eType == CURSOR_REDLINE)
        {
            pUnoCrsr->MoveSection( fnSectionCurr, fnSectionEnd);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/* -----------------05.03.99 07:27-------------------
 *
 * --------------------------------------------------*/
void SwXTextCursor::gotoRange(const uno::Reference< XTextRange > & xRange, sal_Bool bExpand )
                                throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pOwnCursor = GetCrsr();
    if(!pOwnCursor || !xRange.is())
    {
        throw uno::RuntimeException();
    }

    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange 	= reinterpret_cast< SwXTextRange * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXTextRange::getUnoTunnelId()) ));
        pCursor = reinterpret_cast< OTextCursorHelper * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( OTextCursorHelper::getUnoTunnelId()) ));
    }

    SwStartNodeType eSearchNodeType = SwNormalStartNode;
    switch(eType)
    {
        case CURSOR_FRAME:		eSearchNodeType = SwFlyStartNode;		break;
        case CURSOR_TBLTEXT: 	eSearchNodeType = SwTableBoxStartNode;	break;
        case CURSOR_FOOTNOTE: 	eSearchNodeType = SwFootnoteStartNode; 	break;
        case CURSOR_HEADER:		eSearchNodeType = SwHeaderStartNode;	break;
        case CURSOR_FOOTER:		eSearchNodeType = SwFooterStartNode;	break;
        //case CURSOR_INVALID:
        //case CURSOR_BODY:
        default:
            ;
    }
    const SwStartNode* pOwnStartNode = pOwnCursor->GetNode()->
                                            FindSttNodeByType(eSearchNodeType);

    const SwNode* pSrcNode = 0;
    if(pCursor && pCursor->GetPaM())
    {
        pSrcNode = pCursor->GetPaM()->GetNode();
    }
    else if(pRange && pRange->GetBookmark())
    {
        SwBookmark* pBkm = pRange->GetBookmark();
        pSrcNode = &pBkm->GetBookmarkPos().nNode.GetNode();
    }
    const SwStartNode* pTmp = pSrcNode ? pSrcNode->FindSttNodeByType(eSearchNodeType) : 0;

    //SectionNodes ueberspringen
    while(pTmp && pTmp->IsSectionNode())
    {
        pTmp = pTmp->StartOfSectionNode();
    }
    while(pOwnStartNode && pOwnStartNode->IsSectionNode())
    {
        pOwnStartNode = pOwnStartNode->StartOfSectionNode();
    }
    if(pOwnStartNode != pTmp)
    {
        throw uno::RuntimeException();
    }

    //jetzt muss die Selektion erweitert werden
    if(bExpand)
    {
        // der Cursor soll alles einschliessen, was bisher von ihm und dem uebergebenen
        // Range eingeschlossen wurde
        SwPosition aOwnLeft(*pOwnCursor->GetPoint());
        SwPosition aOwnRight(pOwnCursor->HasMark() ? *pOwnCursor->GetMark() : aOwnLeft);
        if(aOwnRight < aOwnLeft)
        {
            SwPosition aTmp = aOwnLeft;
            aOwnLeft = aOwnRight;
            aOwnRight = aTmp;
        }
        SwPosition* pParamLeft;
        SwPosition* pParamRight;
        if(pCursor)
        {
            const SwPaM* pTmp2 = pCursor->GetPaM();
            pParamLeft = new SwPosition(*pTmp2->GetPoint());
            pParamRight = new SwPosition(pTmp2->HasMark() ? *pTmp2->GetMark() : *pParamLeft);
        }
        else
        {
            SwBookmark* pBkm = pRange->GetBookmark();
            pParamLeft = new SwPosition(pBkm->GetBookmarkPos());
            pParamRight = new SwPosition(pBkm->GetOtherBookmarkPos() ? *pBkm->GetOtherBookmarkPos() : *pParamLeft);
        }
        if(*pParamRight < *pParamLeft)
        {
            SwPosition* pTmp2 = pParamLeft;
            pParamLeft = pParamRight;
            pParamRight = pTmp2;
        }
        // jetzt sind vier SwPositions da, zwei davon werden gebraucht, also welche?
        if(aOwnRight > *pParamRight)
            *pOwnCursor->GetPoint() = aOwnRight;
        else
            *pOwnCursor->GetPoint() = *pParamRight;
        pOwnCursor->SetMark();
        if(aOwnLeft < *pParamLeft)
            *pOwnCursor->GetMark() = aOwnLeft;
        else
            *pOwnCursor->GetMark() = *pParamLeft;
        delete pParamLeft;
        delete pParamRight;
    }
    else
    {
        //der Cursor soll dem uebergebenen Range entsprechen
        if(pCursor)
        {
            const SwPaM* pTmp2 = pCursor->GetPaM();
            *pOwnCursor->GetPoint() = *pTmp2->GetPoint();
            if(pTmp2->HasMark())
            {
                pOwnCursor->SetMark();
                *pOwnCursor->GetMark() = *pTmp2->GetMark();
            }
            else
                pOwnCursor->DeleteMark();
        }
        else
        {
            SwBookmark* pBkm = pRange->GetBookmark();
            *pOwnCursor->GetPoint() = pBkm->GetBookmarkPos();
            if(pBkm->GetOtherBookmarkPos())
            {
                pOwnCursor->SetMark();
                *pOwnCursor->GetMark() = *pBkm->GetOtherBookmarkPos();
            }
            else
                pOwnCursor->DeleteMark();
        }
    }
}

/*-- 09.12.98 14:18:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isStartOfWord(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        bRet = pUnoCrsr->IsStartWordWT( i18n::WordType::DICTIONARY_WORD );
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}
/*-- 09.12.98 14:18:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isEndOfWord(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        bRet = pUnoCrsr->IsEndWordWT( i18n::WordType::DICTIONARY_WORD );
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}
/*-- 09.12.98 14:18:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoNextWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    //Probleme gibt's noch mit einem Absatzanfang, an dem kein Wort beginnt.

    SwUnoCrsr* pUnoCrsr = GetCrsr();

    sal_Bool bRet = sal_False;
    if(pUnoCrsr)
    {
        // remember old position to check if cursor has moved
        // since the called functions are sometimes a bit unreliable
        // in specific cases...
        SwPosition  *pPoint		= pUnoCrsr->GetPoint();
        SwNode		*pOldNode	= &pPoint->nNode.GetNode();
        xub_StrLen	 nOldIndex	= pPoint->nContent.GetIndex();

        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        //Absatzende?
        if(pUnoCrsr->GetCntntNode() &&
                pPoint->nContent == pUnoCrsr->GetCntntNode()->Len())
            pUnoCrsr->Right(1, CRSR_SKIP_CHARS, FALSE, FALSE);
        else
        {
            sal_Bool bTmp = pUnoCrsr->GoNextWordWT( i18n::WordType::DICTIONARY_WORD );
            //if there is no next word within the current paragraph try to go to the start of the next paragraph
            if(!bTmp)
                pUnoCrsr->MovePara(fnParaNext, fnParaStart);
        }

        // return true if cursor has moved
        bRet = &pPoint->nNode.GetNode() != pOldNode  ||
                pPoint->nContent.GetIndex() != nOldIndex;
    }
    else
    {
        throw uno::RuntimeException();
    }

    return bRet;
}
/*-- 09.12.98 14:18:45---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoPreviousWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    // hier machen Leerzeichen am Absatzanfang Probleme
    SwUnoCrsr* pUnoCrsr = GetCrsr();

    sal_Bool bRet = sal_False;
    if(pUnoCrsr)
    {
        SwPosition  *pPoint		= pUnoCrsr->GetPoint();
        SwNode		*pOldNode	= &pPoint->nNode.GetNode();
        xub_StrLen	 nOldIndex	= pPoint->nContent.GetIndex();

        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        //Absatzanfang ?
        if(pPoint->nContent == 0)
            pUnoCrsr->Left(1, CRSR_SKIP_CHARS, FALSE, FALSE);
        else
        {
            pUnoCrsr->GoPrevWordWT( i18n::WordType::DICTIONARY_WORD );
            if(pPoint->nContent == 0)
                pUnoCrsr->Left(1, CRSR_SKIP_CHARS, FALSE, FALSE);
        }

        // return true if cursor has moved
        bRet = &pPoint->nNode.GetNode() != pOldNode  ||
                pPoint->nContent.GetIndex() != nOldIndex;
    }
    else
    {
        throw uno::RuntimeException();
    }

    return bRet;
}
/*-- 09.12.98 14:18:45---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoEndOfWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();

    sal_Bool bRet = sal_False;
    if(pUnoCrsr)
    {
        SwPosition  *pPoint		= pUnoCrsr->GetPoint();
        SwNode		&rOldNode	= pPoint->nNode.GetNode();
        xub_StrLen	 nOldIndex	= pPoint->nContent.GetIndex();

        sal_Int16 nWordType = i18n::WordType::DICTIONARY_WORD;
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(!pUnoCrsr->IsEndWordWT( nWordType ))
            pUnoCrsr->GoEndWordWT( nWordType );

        // restore old cursor if we are not at the end of a word by now
        // otherwise use current one
        bRet = pUnoCrsr->IsEndWordWT( nWordType );
        if (!bRet)
        {
            pPoint->nNode		= rOldNode;
            pPoint->nContent	= nOldIndex;
        }
    }
    else
    {
        throw uno::RuntimeException();
    }

    return bRet;
}
/*-- 09.12.98 14:18:46---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoStartOfWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();

    sal_Bool bRet = sal_False;
    if(pUnoCrsr)
    {
        SwPosition  *pPoint		= pUnoCrsr->GetPoint();
        SwNode		&rOldNode	= pPoint->nNode.GetNode();
        xub_StrLen	 nOldIndex	= pPoint->nContent.GetIndex();

        sal_Int16 nWordType = i18n::WordType::DICTIONARY_WORD;
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(!pUnoCrsr->IsStartWordWT( nWordType ))
            pUnoCrsr->GoStartWordWT( nWordType );

        // restore old cursor if we are not at the start of a word by now
        // otherwise use current one
        bRet = pUnoCrsr->IsStartWordWT( nWordType );
        if (!bRet)
        {
            pPoint->nNode		= rOldNode;
            pPoint->nContent	= nOldIndex;
        }
    }
    else
    {
        throw uno::RuntimeException();
    }

    return bRet;
}
/*-- 09.12.98 14:18:46---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isStartOfSentence(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        // start of paragraph?
        bRet = pUnoCrsr->GetPoint()->nContent == 0;
        // with mark ->no sentence start
        // (check if cursor is no selection, i.e. it does not have
        // a mark or else point and mark are identical)
        if(!bRet && (!pUnoCrsr->HasMark() ||
                        *pUnoCrsr->GetPoint() == *pUnoCrsr->GetMark()))
        {
            SwCursor aCrsr(*pUnoCrsr->GetPoint(),0,false);
            SwPosition aOrigPos = *aCrsr.GetPoint();
            aCrsr.GoSentence(SwCursor::START_SENT );
            bRet = aOrigPos == *aCrsr.GetPoint();
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:47---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isEndOfSentence(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        //end of paragraph?
        bRet = pUnoCrsr->GetCntntNode() &&
                pUnoCrsr->GetPoint()->nContent == pUnoCrsr->GetCntntNode()->Len();
        // with mark->no sentence end
        // (check if cursor is no selection, i.e. it does not have
        // a mark or else point and mark are identical)
        if(!bRet && (!pUnoCrsr->HasMark() ||
                        *pUnoCrsr->GetPoint() == *pUnoCrsr->GetMark()))
        {
            SwCursor aCrsr(*pUnoCrsr->GetPoint(),0,false);
            SwPosition aOrigPos = *aCrsr.GetPoint();
            aCrsr.GoSentence(SwCursor::END_SENT );
            bRet = aOrigPos == *aCrsr.GetPoint();
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:47---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoNextSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        BOOL bWasEOS = isEndOfSentence();
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->GoSentence(SwCursor::NEXT_SENT);
        if(!bRet)
            bRet = pUnoCrsr->MovePara(fnParaNext, fnParaStart);

        // if at the end of the sentence (i.e. at the space after the '.')
        // advance to next word in order for GoSentence to work properly
        // next time and have isStartOfSentence return true after this call
        if (!pUnoCrsr->IsStartWord())
        {
            BOOL bNextWord = pUnoCrsr->GoNextWord();
            if (bWasEOS && !bNextWord)
                bRet = sal_False;
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:47---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoPreviousSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->GoSentence(SwCursor::PREV_SENT);
        if(!bRet)
        {
            if(0 != (bRet = pUnoCrsr->MovePara(fnParaPrev, fnParaStart)))
            {
                pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
                //at the end of a paragraph move to the sentence end again
                //
                pUnoCrsr->GoSentence(SwCursor::PREV_SENT);
            }
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/* -----------------15.10.99 08:24-------------------

 --------------------------------------------------*/
sal_Bool SwXTextCursor::gotoStartOfSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        // if we're at the para start then we wont move
        // but bRet is also true if GoSentence failed but
        // the start of the sentence is reached
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr)
            || pUnoCrsr->GoSentence(SwCursor::START_SENT) ||
            SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/* -----------------15.10.99 08:24-------------------

 --------------------------------------------------*/
sal_Bool SwXTextCursor::gotoEndOfSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        // bRet is true if GoSentence() succeeded or if the
        // MovePara() succeeded while the end of the para is
        // not reached already
        sal_Bool bAlreadyParaEnd = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
        bRet = !bAlreadyParaEnd &&
                    (pUnoCrsr->GoSentence(SwCursor::END_SENT) ||
                        pUnoCrsr->MovePara(fnParaCurr, fnParaEnd));

    }
    else
        throw uno::RuntimeException();
    return bRet;
}

/*-- 09.12.98 14:18:48---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isStartOfParagraph(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:48---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::isEndOfParagraph(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        bRet = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoStartOfParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr )
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
        if(!bRet)
            bRet = pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    }
    else
        throw uno::RuntimeException();

    // since MovePara(fnParaCurr, fnParaStart) only returns false
    // if we were already at the start of the paragraph this function
    // should always complete successfully.
    DBG_ASSERT( bRet, "gotoStartOfParagraph failed" );
    return bRet;
}
/*-- 09.12.98 14:18:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoEndOfParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
        if(!bRet)
            bRet = pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
    }
    else
        throw uno::RuntimeException();

    // since MovePara(fnParaCurr, fnParaEnd) only returns false
    // if we were already at the end of the paragraph this function
    // should always complete successfully.
    DBG_ASSERT( bRet, "gotoEndOfParagraph failed" );
    return bRet;
}
/*-- 09.12.98 14:18:50---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoNextParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->MovePara(fnParaNext, fnParaStart);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:50---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextCursor::gotoPreviousParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->MovePara(fnParaPrev, fnParaStart);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 09.12.98 14:18:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XText >  SwXTextCursor::getText(void) throw( uno::RuntimeException )
{
    return xParentText;
}
/*-- 09.12.98 14:18:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXTextCursor::getStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
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
/*-- 09.12.98 14:18:51---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXTextCursor::getEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
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
/*-- 09.12.98 14:18:51---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXTextCursor::getString(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    OUString aTxt;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
    if( pUnoCrsr)
    {
/*		if( pUnoCrsr->GetPoint()->nNode.GetIndex() ==
            pUnoCrsr->GetMark()->nNode.GetIndex() )
        {
            SwTxtNode* pTxtNd = pUnoCrsr->GetNode()->GetTxtNode();
            if( pTxtNd )
            {
                sal_uInt16 nStt = pUnoCrsr->Start()->nContent.GetIndex();
                aTxt = pTxtNd->GetExpandTxt( nStt,
                        pUnoCrsr->End()->nContent.GetIndex() - nStt );
            }
        }
        else
*/		{
            //Text ueber mehrere Absaetze
            SwXTextCursor::getTextFromPam(*pUnoCrsr, aTxt);
        }
    }
    else
        throw uno::RuntimeException();
    return aTxt;
}
/*-- 09.12.98 14:18:52---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::setString(const OUString& aString) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(!pUnoCrsr)
        throw uno::RuntimeException();

    DeleteAndInsert(aString);
}
/* -----------------------------03.05.00 12:56--------------------------------

 ---------------------------------------------------------------------------*/
Any SwXTextCursor::GetPropertyValue(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
        throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aAny;
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        PropertyState eTemp;
        BOOL bDone = SwUnoCursorHelper::getCrsrPropertyValue( pMap, rPaM, &aAny, eTemp );
        if(!bDone)
        {
            SfxItemSet aSet(rPaM.GetDoc()->GetAttrPool(),
                RES_CHRATR_BEGIN, RES_FRMATR_END -1,
                RES_TXTATR_UNKNOWN_CONTAINER, RES_TXTATR_UNKNOWN_CONTAINER,
                RES_UNKNOWNATR_CONTAINER, RES_UNKNOWNATR_CONTAINER,
                0L);
            SwXTextCursor::GetCrsrAttr(rPaM, aSet);

            aAny = rPropSet.getPropertyValue(*pMap, aSet);
        }
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );

    return aAny;
}
/* -----------------------------03.05.00 12:57--------------------------------

 ---------------------------------------------------------------------------*/
void SwXTextCursor::SetPropertyValue(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet, const OUString& rPropertyName,
    const Any& aValue, const SfxItemPropertyMap* _pMap, USHORT nAttrMode)
        throw (UnknownPropertyException, PropertyVetoException,
            IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*   pMap = _pMap ? _pMap : SfxItemPropertyMap::GetByName(
                        rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if ( pMap->nFlags & PropertyAttribute::READONLY)
            throw PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );

        SfxItemSet aItemSet( pDoc->GetAttrPool(), pMap->nWID, pMap->nWID );
        SwXTextCursor::GetCrsrAttr( rPaM, aItemSet );

        if(!lcl_setCrsrPropertyValue( pMap, rPaM, aItemSet, aValue ))
            rPropSet.setPropertyValue(*pMap, aValue, aItemSet );
        SwXTextCursor::SetCrsrAttr(rPaM, aItemSet, nAttrMode );
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
}
/* -----------------------------03.05.00 13:16--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< PropertyState > SwXTextCursor::GetPropertyStates(
            SwPaM& rPaM, SfxItemPropertySet& rPropSet,
            const Sequence< OUString >& PropertyNames,
            SwGetPropertyStatesCaller eCaller )
            throw(UnknownPropertyException, RuntimeException)
{
    const OUString* pNames = PropertyNames.getConstArray();
    Sequence< PropertyState > aRet ( PropertyNames.getLength() );
    PropertyState* pStates = aRet.getArray();

    SfxItemSet *pSet = 0, *pSetParent = 0;
    const SfxItemPropertyMap* pSaveMap, *pMap = rPropSet.getPropertyMap();
    for( INT32 i = 0, nEnd = PropertyNames.getLength(); i < nEnd; i++ )
    {
        pSaveMap = pMap;
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i] );
        if(!pMap)
        {
            if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
               pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
            {
                pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                pMap = pSaveMap;
                continue;
            }
            else
                throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
        }
        if (eCaller == SW_PROPERTY_STATE_CALLER_SWX_TEXT_PORTION &&
            pMap->nWID < FN_UNO_RANGE_BEGIN &&
            pMap->nWID > FN_UNO_RANGE_END  &&
            pMap->nWID < RES_CHRATR_BEGIN &&
            pMap->nWID > RES_TXTATR_END )
            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
        else
        {
            if ( pMap->nWID >= FN_UNO_RANGE_BEGIN &&
                 pMap->nWID <= FN_UNO_RANGE_END )
                SwUnoCursorHelper::getCrsrPropertyValue(pMap, rPaM, 0, pStates[i] );
            else
            {
                if( !pSet )
                {
                    switch ( eCaller )
                    {
                        case SW_PROPERTY_STATE_CALLER_SWX_TEXT_PORTION:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                    RES_CHRATR_BEGIN,   RES_TXTATR_END );
                        break;
                        case SW_PROPERTY_STATE_CALLER_SINGLE_VALUE_ONLY:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                    pMap->nWID, pMap->nWID );
                        break;
                        default:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                RES_CHRATR_BEGIN, RES_FRMATR_END - 1,
                                RES_UNKNOWNATR_CONTAINER, RES_UNKNOWNATR_CONTAINER,
                                RES_TXTATR_UNKNOWN_CONTAINER, RES_TXTATR_UNKNOWN_CONTAINER,
                                0L );
                    }
                    // --> OD 2006-07-12 #i63870#
                    SwXTextCursor::GetCrsrAttr( rPaM, *pSet );
                    // <--
                }

                if( pSet->Count() )
                    pStates[i] = rPropSet.getPropertyState( *pMap,*pSet );
                else
                    pStates[i] = PropertyState_DEFAULT_VALUE;

                //try again to find out if a value has been inherited
                if( beans::PropertyState_DIRECT_VALUE == pStates[i] )
                {
                    if( !pSetParent )
                    {
                        pSetParent = pSet->Clone( FALSE );
                        // --> OD 2006-07-12 #i63870#
                        SwXTextCursor::GetCrsrAttr( rPaM, *pSetParent, TRUE, FALSE );
                        // <--
                    }

                    if( (pSetParent)->Count() )
                        pStates[i] = rPropSet.getPropertyState( *pMap, *pSetParent );
                    else
                        pStates[i] = PropertyState_DEFAULT_VALUE;
                }
            }
        }
        pMap++;
    }
    delete pSet;
    delete pSetParent;
    return aRet;
}
/* -----------------------------03.05.00 13:17--------------------------------

 ---------------------------------------------------------------------------*/
PropertyState SwXTextCursor::GetPropertyState(
    SwPaM& rPaM, SfxItemPropertySet& rPropSet, const OUString& rPropertyName)
                        throw(UnknownPropertyException, RuntimeException)
{
    Sequence < OUString > aStrings ( 1 );
    aStrings[0] = rPropertyName;
    Sequence < PropertyState > aSeq = GetPropertyStates( rPaM, rPropSet, aStrings, SW_PROPERTY_STATE_CALLER_SINGLE_VALUE_ONLY );
    return aSeq[0];
}
/* -----------------------------03.05.00 13:20--------------------------------

 ---------------------------------------------------------------------------*/
void lcl_SelectParaAndReset ( SwPaM &rPaM, SwDoc* pDoc, const SvUShortsSort* pWhichIds = 0 )
{
    // if we are reseting paragraph attributes, we need to select the full paragraph first
    SwPosition aStart = *rPaM.Start();
    SwPosition aEnd = *rPaM.End();
    auto_ptr < SwUnoCrsr > pTemp ( pDoc->CreateUnoCrsr(aStart, FALSE) );
    if(!SwUnoCursorHelper::IsStartOfPara(*pTemp))
        pTemp->MovePara(fnParaCurr, fnParaStart);
    pTemp->SetMark();
    *pTemp->GetPoint() = aEnd;
    SwXTextCursor::SelectPam(*pTemp, sal_True);
    if(!SwUnoCursorHelper::IsEndOfPara(*pTemp))
        pTemp->MovePara(fnParaCurr, fnParaEnd);
    pDoc->ResetAttrs(*pTemp, sal_True, pWhichIds);
}


void SwXTextCursor::SetPropertyToDefault(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
    throw(UnknownPropertyException, RuntimeException)
{
    NAMESPACE_VOS(OGuard) aGuard(Application::GetSolarMutex());
    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if ( pMap->nFlags & PropertyAttribute::READONLY)
            throw RuntimeException( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "setPropertyToDefault: property is read-only: " ) ) + rPropertyName, 0 );
        if(pMap->nWID < RES_FRMATR_END)
        {
            SvUShortsSort aWhichIds;
            aWhichIds.Insert(pMap->nWID);
            if(pMap->nWID < RES_PARATR_BEGIN)
                pDoc->ResetAttrs(rPaM, sal_True, &aWhichIds);
            else
                lcl_SelectParaAndReset ( rPaM, pDoc, &aWhichIds );
        }
        else
            SwUnoCursorHelper::resetCrsrPropertyValue(pMap, rPaM);
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
}
/* -----------------------------03.05.00 13:19--------------------------------

 ---------------------------------------------------------------------------*/
Any SwXTextCursor::GetPropertyDefault(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
    throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aRet;
    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if(pMap->nWID < RES_FRMATR_END)
        {
            const SfxPoolItem& rDefItem = pDoc->GetAttrPool().GetDefaultItem(pMap->nWID);
            rDefItem.QueryValue(aRet, pMap->nMemberId);
        }
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
    return aRet;
}
/*-- 09.12.98 14:18:54---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo >  SwXTextCursor::getPropertySetInfo(void) throw( uno::RuntimeException )
{
    static uno::Reference< beans::XPropertySetInfo >  xRef;
    if(!xRef.is())
    {
        static SfxItemPropertyMap aCrsrExtMap_Impl[] =
        {
            { SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT), FN_SKIP_HIDDEN_TEXT, &::getBooleanCppuType(), PROPERTY_NONE,     0},
            { SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT), FN_SKIP_PROTECTED_TEXT, &::getBooleanCppuType(), PROPERTY_NONE,     0},
            {0,0,0,0,0,0}
        };
        uno::Reference< beans::XPropertySetInfo >  xInfo = aPropSet.getPropertySetInfo();
        // PropertySetInfo verlaengern!
        const uno::Sequence<beans::Property> aPropSeq = xInfo->getProperties();
        xRef = new SfxExtItemPropertySetInfo(
            aCrsrExtMap_Impl,
            aPropSeq );
    }
    return xRef;
}
/*-- 09.12.98 14:18:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::setPropertyValue(const OUString& rPropertyName, const uno::Any& aValue)
        throw( beans::UnknownPropertyException, beans::PropertyVetoException,
             lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)))
        {
            sal_Bool bSet = *(sal_Bool*)aValue.getValue();
            pUnoCrsr->SetSkipOverHiddenSections(bSet);
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
        {
            sal_Bool bSet = *(sal_Bool*)aValue.getValue();
            pUnoCrsr->SetSkipOverProtectSections(bSet);
        }
        else
            SetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName, aValue);
    }
    else
        throw uno::RuntimeException();

}
/*-- 09.12.98 14:18:55---------------------------------------------------

  -----------------------------------------------------------------------*/
Any SwXTextCursor::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aAny;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)))
        {
            BOOL bSet = pUnoCrsr->IsSkipOverHiddenSections();
            aAny.setValue(&bSet, ::getBooleanCppuType());
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
        {
            BOOL bSet = pUnoCrsr->IsSkipOverProtectSections();
            aAny.setValue(&bSet, ::getBooleanCppuType());
        }
        else
            aAny = GetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName);
    }
    else
        throw uno::RuntimeException();
    return aAny;

}
/*-- 09.12.98 14:18:55---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::addPropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 09.12.98 14:18:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::removePropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 09.12.98 14:18:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::addVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 09.12.98 14:18:58---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::removeVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 05.03.99 11:36:11---------------------------------------------------

  -----------------------------------------------------------------------*/
beans::PropertyState SwXTextCursor::getPropertyState(const OUString& rPropertyName)
                        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        eRet = GetPropertyState(*pUnoCrsr, aPropSet, rPropertyName);
    }
    else
        throw RuntimeException();
    return eRet;
}
/*-- 05.03.99 11:36:11---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< beans::PropertyState > SwXTextCursor::getPropertyStates(
            const uno::Sequence< OUString >& PropertyNames)
            throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(!pUnoCrsr)
        throw RuntimeException();
    return GetPropertyStates(*pUnoCrsr, aPropSet, PropertyNames);
}
/*-- 05.03.99 11:36:12---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextCursor::setPropertyToDefault(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    Sequence < OUString > aSequence ( &rPropertyName, 1 );
    setPropertiesToDefault ( aSequence );
}
/*-- 05.03.99 11:36:12---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextCursor::getPropertyDefault(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    const Sequence < OUString > aSequence ( &rPropertyName, 1 );
    return getPropertyDefaults ( aSequence ).getConstArray()[0];
}

void SAL_CALL SwXTextCursor::setAllPropertiesToDefault()
    throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if (pUnoCrsr)
    {
        // para specific attribut ranges
        USHORT aParaResetableSetRange[] = {
            RES_FRMATR_BEGIN, RES_FRMATR_END-1,
            RES_PARATR_BEGIN, RES_PARATR_END-1,
            RES_UNKNOWNATR_BEGIN, RES_UNKNOWNATR_END-1,
            0
        };
        // selection specific attribut ranges
        USHORT aResetableSetRange[] = {
            RES_CHRATR_BEGIN, RES_CHRATR_END-1,
            RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT,
            RES_TXTATR_INETFMT, RES_TXTATR_INETFMT,
            RES_TXTATR_CJK_RUBY, RES_TXTATR_UNKNOWN_CONTAINER,
            0
        };
        SvUShortsSort	aParaWhichIds;
        SvUShortsSort	aWhichIds;
        for (USHORT k = 0;  k < 2;  ++k)
        {
            SvUShortsSort &rWhichIds	= k == 0? aParaWhichIds : aWhichIds;
            USHORT *pResetableSetRange	= k == 0? aParaResetableSetRange : aResetableSetRange;
            while (*pResetableSetRange)
            {
                USHORT nStart	= sal::static_int_cast< USHORT >(*pResetableSetRange++);
                USHORT nEnd		= sal::static_int_cast< USHORT >(*pResetableSetRange++);
                for (USHORT nId = nStart + 1;  nId <= nEnd;  ++nId)
                    rWhichIds.Insert( nId );
            }
        }
        if (aParaWhichIds.Count())
            lcl_SelectParaAndReset( *pUnoCrsr, pUnoCrsr->GetDoc(), &aParaWhichIds );
        if (aWhichIds.Count() )
            pUnoCrsr->GetDoc()->ResetAttrs( *pUnoCrsr, sal_True, &aWhichIds );
    }
    else
        throw uno::RuntimeException();
}

void SAL_CALL SwXTextCursor::setPropertiesToDefault( const Sequence< OUString >& aPropertyNames )
    throw (UnknownPropertyException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nCount = aPropertyNames.getLength();
    if ( nCount )
    {
        SwUnoCrsr* pUnoCrsr = GetCrsr();
        if(pUnoCrsr)
        {
            SwDoc* pDoc = pUnoCrsr->GetDoc();
            const SfxItemPropertyMap* pMap = aPropSet.getPropertyMap(), *pSaveMap;
            const OUString * pNames = aPropertyNames.getConstArray();
            SvUShortsSort aWhichIds, aParaWhichIds;
            for ( sal_Int32 i = 0; i < nCount; i++ )
            {
                pSaveMap = pMap;
                pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i]);
                if(!pMap)
                {
                    if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
                       pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
                    {
                        pMap = pSaveMap;
                        continue;
                    }
                    else
                        throw UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
                }
                if ( pMap->nFlags & PropertyAttribute::READONLY)
                    throw RuntimeException( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "setPropertiesToDefault: property is read-only: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( this ) );

                if( pMap->nWID < RES_FRMATR_END)
                {
                    if(pMap->nWID < RES_PARATR_BEGIN)
                        aWhichIds.Insert(pMap->nWID);
                    else
                        aParaWhichIds.Insert (pMap->nWID);
                }
                else if ( pMap->nWID == FN_UNO_NUM_START_VALUE )
                    SwUnoCursorHelper::resetCrsrPropertyValue(pMap, *pUnoCrsr);
            }

            if ( aParaWhichIds.Count() )
                lcl_SelectParaAndReset ( *pUnoCrsr, pDoc, &aParaWhichIds );
            if (aWhichIds.Count() )
                pDoc->ResetAttrs(*pUnoCrsr, sal_True, &aWhichIds);
        }
        else
            throw uno::RuntimeException();
    }
}
Sequence< Any > SAL_CALL SwXTextCursor::getPropertyDefaults( const Sequence< OUString >& aPropertyNames )
    throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nCount = aPropertyNames.getLength();
    Sequence < Any > aRet ( nCount );
    if ( nCount )
    {
        SwUnoCrsr* pUnoCrsr = GetCrsr();
        if (pUnoCrsr)
        {
            SwDoc* pDoc = pUnoCrsr->GetDoc();
            const SfxItemPropertyMap *pSaveMap, *pMap = aPropSet.getPropertyMap();
            const OUString *pNames = aPropertyNames.getConstArray();
            Any *pAny = aRet.getArray();
            for ( sal_Int32 i = 0; i < nCount; i++)
            {
                pSaveMap = pMap;
                pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i]);
                if(!pMap)
                {
                    if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
                       pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
                    {
                        pMap = pSaveMap;
                        continue;
                    }
                    else
                        throw UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
                }
                if(pMap->nWID < RES_FRMATR_END)
                {
                    const SfxPoolItem& rDefItem = pDoc->GetAttrPool().GetDefaultItem(pMap->nWID);
                    rDefItem.QueryValue(pAny[i], pMap->nMemberId);
                }
            }
        }
        else
            throw UnknownPropertyException();
    }
    return aRet;
}
