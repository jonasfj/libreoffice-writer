/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unocrsrhelper.cxx,v $
 *
 * $Revision: 1.35 $
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

#include <map>

#include <com/sun/star/text/XTextSection.hpp>

#include <cmdid.h>
#include <unocrsrhelper.hxx>
#include <unofootnote.hxx>
#include <unorefmark.hxx>
#include <unostyle.hxx>
#include <unoidx.hxx>
#include <unofield.hxx>
#include <unotbl.hxx>
#include <unosett.hxx>
#include <unoframe.hxx>
#include <unocrsr.hxx>
#include <doc.hxx>
#include <IDocumentRedlineAccess.hxx>
#include <fmtftn.hxx>
#include <fmtpdsc.hxx>
#include <charfmt.hxx>
#include <pagedesc.hxx>
#include <docstyle.hxx>
#include <ndtxt.hxx>
#include <txtrfmrk.hxx>
#include <fmtfld.hxx>
#include <docsh.hxx>
#include <section.hxx>
#include <shellio.hxx>
#include <edimp.hxx>
#include <swundo.hxx>
#include <cntfrm.hxx>
#include <pagefrm.hxx>
#include <svtools/eitem.hxx>
#include <tools/urlobj.hxx>
#include <docary.hxx>
#include <swtable.hxx>
#include <tox.hxx>
#include <fchrfmt.hxx>
#include <svx/flstitem.hxx>
#include <vcl/metric.hxx>
#include <svtools/ctrltool.hxx>
#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svtools/svstdarr.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/fcontnr.hxx>
#include <svtools/stritem.hxx>
#include <com/sun/star/beans/PropertyState.hpp>
#include <SwStyleNameMapper.hxx>
#include <redline.hxx>
#include <numrule.hxx>
#include <comphelper/storagehelper.hxx>
#include <comphelper/mediadescriptor.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
// --> OD 2008-11-26 #158694#
#include <SwNodeNum.hxx>
// <--

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::table;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using ::rtl::OUString;


namespace SwUnoCursorHelper
{

/* -----------------16.09.98 12:27-------------------
* 	Lesen spezieller Properties am Cursor
 * --------------------------------------------------*/
sal_Bool getCrsrPropertyValue(const SfxItemPropertySimpleEntry& rEntry
                                        , SwPaM& rPam
                                        , Any *pAny
                                        , PropertyState& eState
                                        , const SwTxtNode* pNode  )
{
    PropertyState eNewState = PropertyState_DIRECT_VALUE;
//    PropertyState_DEFAULT_VALUE
//    PropertyState_AMBIGUOUS_VALUE
    sal_Bool bDone = sal_True;
    switch(rEntry.nWID)
    {
        // --> OD 2008-11-26 #158694#
        case FN_UNO_PARA_CONT_PREV_SUBTREE:
            if (pAny)
            {
                const SwTxtNode * pTmpNode = pNode;

                if (!pTmpNode)
                    pTmpNode = rPam.GetNode()->GetTxtNode();

                bool bRet = false;

                if ( pTmpNode &&
                     pTmpNode->GetNum() &&
                     pTmpNode->GetNum()->IsContinueingPreviousSubTree() )
                {
                    bRet = true;
                }

                *pAny <<= bRet;
            }
        break;
        case FN_UNO_PARA_NUM_STRING:
            if (pAny)
            {
                const SwTxtNode * pTmpNode = pNode;

                if (!pTmpNode)
                    pTmpNode = rPam.GetNode()->GetTxtNode();

                String sRet;
                if ( pTmpNode && pTmpNode->GetNum() )
                {
                    sRet = pTmpNode->GetNumString();
                }

                *pAny <<= OUString(sRet);
            }
        break;
        // <--
        // --> OD 2008-05-20 #outlinelevel# - no longer needed
//        case FN_UNO_PARA_CHAPTER_NUMBERING_LEVEL:
//            if (pAny)
//            {
//                const SwTxtNode * pTmpNode = pNode;

//                if (!pTmpNode)
//                    pTmpNode = rPam.GetNode()->GetTxtNode();

//                sal_Int8 nRet = -1;
//                if (pTmpNode && pTmpNode->GetOutlineLevel() != NO_NUMBERING)
//                nRet = sal::static_int_cast< sal_Int8 >(pTmpNode->GetOutlineLevel());
//                *pAny <<= nRet;
//            }
//        break;
        // <--
        case RES_PARATR_OUTLINELEVEL: //#outlinelevel added by zhaojianwei
            if (pAny)
            {
                const SwTxtNode * pTmpNode = pNode;

                if (!pTmpNode)
                    pTmpNode = rPam.GetNode()->GetTxtNode();

                sal_Int16 nRet = -1;
                if ( pTmpNode )
                    nRet = sal::static_int_cast< sal_Int16 >( pTmpNode->GetAttrOutlineLevel() );

                *pAny <<= nRet;
            }
        break; //<-end,zhaojianwei
        case FN_UNO_PARA_CONDITIONAL_STYLE_NAME:
        case FN_UNO_PARA_STYLE :
        {
            SwFmtColl* pFmt = 0;
            if(pNode)
                pFmt = FN_UNO_PARA_CONDITIONAL_STYLE_NAME == rEntry.nWID
                            ? pNode->GetFmtColl() : &pNode->GetAnyFmtColl();
            else
            {
                pFmt = SwUnoCursorHelper::GetCurTxtFmtColl(rPam,
                        FN_UNO_PARA_CONDITIONAL_STYLE_NAME == rEntry.nWID);
            }
            if(pFmt)
            {
                if( pAny )
                {
                    String sVal;
                    SwStyleNameMapper::FillProgName(pFmt->GetName(), sVal, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
                    *pAny <<= OUString(sVal);
                }
            }
            else
                eNewState = PropertyState_AMBIGUOUS_VALUE;
        }
        break;
        case FN_UNO_PAGE_STYLE :
        {
            String sVal;
            GetCurPageStyle(rPam, sVal);
            if( pAny )
                *pAny <<= OUString(sVal);
            if(!sVal.Len())
                eNewState = PropertyState_AMBIGUOUS_VALUE;
        }
        break;
        case FN_UNO_NUM_START_VALUE  :
            if( pAny )
            {
                sal_Int16 nValue = IsNodeNumStart(rPam, eNewState);
                *pAny <<= nValue;
            }
        break;
        case FN_UNO_NUM_LEVEL  :
        case FN_UNO_IS_NUMBER  :
        // --> OD 2008-07-14 #i91601#
        case FN_UNO_LIST_ID:
        // <--
        case FN_NUMBER_NEWSTART:
        {
            const SwTxtNode* pTxtNd = rPam.GetNode()->GetTxtNode();
            // --> OD 2006-10-19 #134160# - make code robust:
            // consider case that PaM doesn't denote a text node
            const SwNumRule* pRule = pTxtNd ? pTxtNd->GetNumRule() : 0;
            // <--
            // hier wird Multiselektion nicht beruecksichtigt
            if( pRule )
            {
                if( pAny )
                {
                    if(rEntry.nWID == FN_UNO_NUM_LEVEL)
                        *pAny <<= (sal_Int16)(pTxtNd->GetActualListLevel());
                    else if(rEntry.nWID == FN_UNO_IS_NUMBER)
                    {
                        BOOL bIsNumber = pTxtNd->IsCountedInList();
                        pAny->setValue(&bIsNumber, ::getBooleanCppuType());
                    }
                    // --> OD 2008-07-14 #i91601#
                    else if ( rEntry.nWID == FN_UNO_LIST_ID )
                    {
                        const String sListId = pTxtNd->GetListId();
                        *pAny <<= OUString(sListId);
                    }
                    // <--
                    else /*if(rEntry.nWID == UNO_NAME_PARA_IS_NUMBERING_RESTART)*/
                    {
                        BOOL bIsRestart = pTxtNd->IsListRestart();
                        pAny->setValue(&bIsRestart, ::getBooleanCppuType());
                    }
                }
            }
            else
            {
                eNewState = PropertyState_DEFAULT_VALUE;

                if( pAny )
                {
                    // #i30838# set default values for default properties
                    if(rEntry.nWID == FN_UNO_NUM_LEVEL)
                        *pAny <<= static_cast<sal_Int16>( 0 );
                    else if(rEntry.nWID == FN_UNO_IS_NUMBER)
                        *pAny <<= false;
                    // --> OD 2008-07-14 #i91601#
                    else if ( rEntry.nWID == FN_UNO_LIST_ID )
                    {
                        *pAny <<= OUString();
                    }
                    // <--
                    else /*if(rEntry.nWID == UNO_NAME_PARA_IS_NUMBERING_RESTART)*/
                        *pAny <<= false;
                }
            }
            //PROPERTY_MAYBEVOID!
        }
        break;
        case FN_UNO_NUM_RULES  :
            if( pAny )
                getNumberingProperty(rPam, eNewState, pAny);
            else
            {
                if( !rPam.GetDoc()->GetCurrNumRule( *rPam.GetPoint() ) )
                    eNewState = PropertyState_DEFAULT_VALUE;
            }
            break;
        case FN_UNO_DOCUMENT_INDEX_MARK:
        {
            SwTxtAttr* pTxtAttr = rPam.GetNode()->GetTxtNode()->GetTxtAttr(
                                rPam.GetPoint()->nContent, RES_TXTATR_TOXMARK);
            if(pTxtAttr)
            {
                if( pAny )
                {
                    const SwTOXMark& rMark = pTxtAttr->GetTOXMark();
                    uno::Reference< XDocumentIndexMark >  xRef = SwXDocumentIndexMark::GetObject(
                            (SwTOXType*)rMark.GetTOXType(), &rMark, rPam.GetDoc());
                    pAny->setValue(&xRef, ::getCppuType((uno::Reference<XDocumentIndex>*)0));
                }
            }
            else
                //auch hier - nicht zu unterscheiden
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_DOCUMENT_INDEX:
        {
            const SwTOXBase* pBase = rPam.GetDoc()->GetCurTOX(
                                                    *rPam.Start() );
            if( pBase )
            {
                if( pAny )
                {
                    uno::Reference< XDocumentIndex > aRef =
                        SwXDocumentIndexes::GetObject((SwTOXBaseSection*)pBase);
                    pAny->setValue(&aRef, ::getCppuType((uno::Reference<XDocumentIndex>*)0));
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_TEXT_FIELD:
        {
            const SwPosition *pPos = rPam.Start();
            const SwTxtNode *pTxtNd =
                rPam.GetDoc()->GetNodes()[pPos->nNode.GetIndex()]->GetTxtNode();
            SwTxtAttr* pTxtAttr =
                pTxtNd ? pTxtNd->GetTxtAttr(pPos->nContent, RES_TXTATR_FIELD)
                       : 0;
            if(pTxtAttr)
            {
                if( pAny )
                {
                    SwXTextField* pField = CreateSwXTextField(*rPam.GetDoc(),
                           pTxtAttr->GetFld());
                    *pAny <<= uno::Reference< XTextField >( pField );
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
/*				laesst sich nicht feststellen
* 				case FN_UNO_BOOKMARK:
        {
            if()
            {
                uno::Reference< XBookmark >  xBkm = SwXBookmarks::GetObject(rBkm);
                rAny.set(&xBkm, ::getCppuType((const XBookmark*)0)());
            }
        }
        break;*/
        case FN_UNO_TEXT_TABLE:
        case FN_UNO_CELL:
        {
            SwStartNode* pSttNode = rPam.GetNode()->StartOfSectionNode();
            SwStartNodeType eType = pSttNode->GetStartNodeType();
            if(SwTableBoxStartNode == eType)
            {
                if( pAny )
                {
                    const SwTableNode* pTblNode = pSttNode->FindTableNode();
                    SwFrmFmt* pTableFmt = (SwFrmFmt*)pTblNode->GetTable().GetFrmFmt();
                    //SwTable& rTable = ((SwTableNode*)pSttNode)->GetTable();
                    if(FN_UNO_TEXT_TABLE == rEntry.nWID)
                    {
                        uno::Reference< XTextTable >  xTable = SwXTextTables::GetObject(*pTableFmt);
                        pAny->setValue(&xTable, ::getCppuType((uno::Reference<XTextTable>*)0));
                    }
                    else
                    {
                        SwTableBox* pBox = pSttNode->GetTblBox();
                        uno::Reference< XCell >  xCell = SwXCell::CreateXCell(pTableFmt, pBox);
                        pAny->setValue(&xCell, ::getCppuType((uno::Reference<XCell>*)0));
                    }
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_TEXT_FRAME:
        {
            SwStartNode* pSttNode = rPam.GetNode()->StartOfSectionNode();
            SwStartNodeType eType = pSttNode->GetStartNodeType();

            SwFrmFmt* pFmt;
            if(eType == SwFlyStartNode && 0 != (pFmt = pSttNode->GetFlyFmt()))
            {
                if( pAny )
                {
                    uno::Reference< XTextFrame >  xFrm = (SwXTextFrame*) SwXFrames::GetObject(*pFmt, FLYCNTTYPE_FRM);
                    pAny->setValue(&xFrm, ::getCppuType((uno::Reference<XTextFrame>*)0));
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_TEXT_SECTION:
        {
            SwSection* pSect = rPam.GetDoc()->GetCurrSection(*rPam.GetPoint());
            if(pSect)
            {
                if( pAny )
                {
                    uno::Reference< XTextSection >  xSect = SwXTextSections::GetObject( *pSect->GetFmt() );
                    pAny->setValue(&xSect, ::getCppuType((uno::Reference<XTextSection>*)0) );
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_ENDNOTE:
        case FN_UNO_FOOTNOTE:
        {
            SwTxtAttr* pTxtAttr = rPam.GetNode()->GetTxtNode()->
                                        GetTxtAttr(rPam.GetPoint()->nContent, RES_TXTATR_FTN);
            if(pTxtAttr)
            {
                const SwFmtFtn& rFtn = pTxtAttr->GetFtn();
                if(rFtn.IsEndNote() == (FN_UNO_ENDNOTE == rEntry.nWID))
                {
                    if( pAny )
                    {
                        uno::Reference< XFootnote >  xFoot = new SwXFootnote(rPam.GetDoc(), rFtn);
                        pAny->setValue(&xFoot, ::getCppuType((uno::Reference<XFootnote>*)0));
                    }
                }
                else
                    eNewState = PropertyState_DEFAULT_VALUE;
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_REFERENCE_MARK:
        {
            SwTxtAttr* pTxtAttr = rPam.GetNode()->GetTxtNode()->
                                        GetTxtAttr(rPam.GetPoint()->nContent, RES_TXTATR_REFMARK);
            if(pTxtAttr)
            {
                if( pAny )
                {
                    const SwFmtRefMark& rRef = pTxtAttr->GetRefMark();
                    uno::Reference< XTextContent >  xRef = SwXReferenceMarks::GetObject( rPam.GetDoc(), &rRef );
                    pAny->setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
                }
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case FN_UNO_CHARFMT_SEQUENCE:
        {

            SwTxtNode* pTxtNode;
            if((pTxtNode = (SwTxtNode*)rPam.GetNode( TRUE )) == rPam.GetNode(FALSE) &&
                    pTxtNode->GetpSwpHints())
            {
                USHORT nPaMStart = rPam.GetPoint()->nContent.GetIndex();
                USHORT nPaMEnd = rPam.GetMark() ? rPam.GetMark()->nContent.GetIndex() : nPaMStart;
                if(nPaMStart > nPaMEnd)
                {
                    USHORT nTmp = nPaMStart;
                    nPaMStart = nPaMEnd;
                    nPaMEnd = nTmp;
                }
                Sequence< ::rtl::OUString> aCharStyles;
                SwpHints* pHints = pTxtNode->GetpSwpHints();
                for(USHORT nAttr = 0; nAttr < pHints->GetStartCount(); nAttr++ )
                {
                    SwTxtAttr* pAttr = pHints->GetStart( nAttr );
                    if(pAttr->Which() != RES_TXTATR_CHARFMT)
                        continue;
                    USHORT nAttrStart = *pAttr->GetStart();
                    USHORT nAttrEnd = *pAttr->GetEnd();
                    //check if the attribute touches the selection
                    if( ( nAttrEnd > nPaMStart && nAttrStart < nPaMEnd ) ||
                        ( !nAttrStart && !nAttrEnd && !nPaMStart && !nPaMEnd ) )
                    {
                        //check for overlapping
                        if(nAttrStart > nPaMStart ||
                                    nAttrEnd < nPaMEnd)
                        {
                            aCharStyles.realloc(0);
                            eNewState = PropertyState_AMBIGUOUS_VALUE;
                            break;
                        }
                        else
                        {
                            //now the attribute should start before or at the selection
                            //and it should end at the end of the selection or behind
                            DBG_ASSERT(nAttrStart <= nPaMStart && nAttrEnd >=nPaMEnd,
                                    "attribute overlaps or is outside");
                            //now the name of the style has to be added to the sequence
                            aCharStyles.realloc(aCharStyles.getLength() + 1);
                            DBG_ASSERT(pAttr->GetCharFmt().GetCharFmt(), "no character format set");
                            aCharStyles.getArray()[aCharStyles.getLength() - 1] =
                                        SwStyleNameMapper::GetProgName(
                                            pAttr->GetCharFmt().GetCharFmt()->GetName(), nsSwGetPoolIdFromName::GET_POOLID_CHRFMT);
                        }
                    }

                }
                eNewState =
                    aCharStyles.getLength() ?
                        PropertyState_DIRECT_VALUE : PropertyState_DEFAULT_VALUE;;
                if(pAny)
                    (*pAny) <<= aCharStyles;
            }
            else
                eNewState = PropertyState_DEFAULT_VALUE;
        }
        break;
        case RES_TXTATR_CHARFMT:
        // kein break hier!
        default: bDone = sal_False;
    }
    if( bDone )
        eState = eNewState;
    return bDone;
};
/* -----------------30.06.98 10:30-------------------
 *
 * --------------------------------------------------*/
sal_Int16 IsNodeNumStart(SwPaM& rPam, PropertyState& eState)
{
    const SwTxtNode* pTxtNd = rPam.GetNode()->GetTxtNode();
    // --> OD 2008-02-28 #refactorlists#
    // correction: check, if restart value is set at the text node and use
    // new method <SwTxtNode::GetAttrListRestartValue()> to retrieve the value
    if ( pTxtNd && pTxtNd->GetNumRule() && pTxtNd->IsListRestart() &&
         pTxtNd->HasAttrListRestartValue() )
    {
        eState = PropertyState_DIRECT_VALUE;
        sal_Int16 nTmp = sal::static_int_cast< sal_Int16 >(pTxtNd->GetAttrListRestartValue());
        return nTmp;
    }
    // <--
    eState = PropertyState_DEFAULT_VALUE;
    return -1;
}

/* -----------------25.05.98 11:41-------------------
 *
 * --------------------------------------------------*/
void setNumberingProperty(const Any& rValue, SwPaM& rPam)
{
    uno::Reference<XIndexReplace> xIndexReplace;
    if(rValue >>= xIndexReplace)
    {
        SwXNumberingRules* pSwNum = 0;

        uno::Reference<XUnoTunnel> xNumTunnel(xIndexReplace, UNO_QUERY);
        if(xNumTunnel.is())
        {
            pSwNum = reinterpret_cast< SwXNumberingRules * >(
                sal::static_int_cast< sal_IntPtr >( xNumTunnel->getSomething( SwXNumberingRules::getUnoTunnelId() )));
        }

        if(pSwNum)
        {
            SwDoc* pDoc = rPam.GetDoc();
            if(pSwNum->GetNumRule())
            {
                SwNumRule aRule(*pSwNum->GetNumRule());
                const String* pNewCharStyles =  pSwNum->GetNewCharStyleNames();
                const String* pBulletFontNames = pSwNum->GetBulletFontNames();
                for(sal_uInt16 i = 0; i < MAXLEVEL; i++)
                {
                    SwNumFmt aFmt(aRule.Get( i ));
                    if( pNewCharStyles[i].Len() &&
                        pNewCharStyles[i] != SwXNumberingRules::GetInvalidStyle() &&
                        (!aFmt.GetCharFmt() || pNewCharStyles[i] != aFmt.GetCharFmt()->GetName()))
                    {
                        if(!pNewCharStyles[i].Len())
                            aFmt.SetCharFmt(0);
                        else
                        {

                            // CharStyle besorgen und an der Rule setzen
                            sal_uInt16 nChCount = pDoc->GetCharFmts()->Count();
                            SwCharFmt* pCharFmt = 0;
                            for(sal_uInt16 nCharFmt = 0; nCharFmt < nChCount; nCharFmt++)
                            {
                                SwCharFmt& rChFmt = *((*(pDoc->GetCharFmts()))[nCharFmt]);;
                                if(rChFmt.GetName() == pNewCharStyles[i])
                                {
                                    pCharFmt = &rChFmt;
                                    break;
                                }
                            }

                            if(!pCharFmt)
                            {
                                SfxStyleSheetBasePool* pPool = pDoc->GetDocShell()->GetStyleSheetPool();
                                SfxStyleSheetBase* pBase;
                                pBase = pPool->Find(pNewCharStyles[i], SFX_STYLE_FAMILY_CHAR);
                            // soll das wirklich erzeugt werden?
                                if(!pBase)
                                    pBase = &pPool->Make(pNewCharStyles[i], SFX_STYLE_FAMILY_PAGE);
                                pCharFmt = ((SwDocStyleSheet*)pBase)->GetCharFmt();
                            }
                            if(pCharFmt)
                                aFmt.SetCharFmt(pCharFmt);
                        }
                    }
                    //jetzt nochmal fuer Fonts
                    if(
                       pBulletFontNames[i] != SwXNumberingRules::GetInvalidStyle() &&
                       (
                        (pBulletFontNames[i].Len() && !aFmt.GetBulletFont()) ||
                        (pBulletFontNames[i].Len() &&
                            aFmt.GetBulletFont()->GetName() != pBulletFontNames[i])
                       )
                      )
                    {
                        const SvxFontListItem* pFontListItem =
                                (const SvxFontListItem* )pDoc->GetDocShell()
                                                    ->GetItem( SID_ATTR_CHAR_FONTLIST );
                        const FontList*	 pList = pFontListItem->GetFontList();

                        FontInfo aInfo = pList->Get(
                            pBulletFontNames[i],WEIGHT_NORMAL, ITALIC_NONE);
                        Font aFont(aInfo);
                        aFmt.SetBulletFont(&aFont);
                    }
                    aRule.Set( i, aFmt );
                }
                UnoActionContext aAction(pDoc);

                if( rPam.GetNext() != &rPam )           // Mehrfachselektion ?
                {
                    pDoc->StartUndo( UNDO_START, NULL );
                    SwPamRanges aRangeArr( rPam );
                    SwPaM aPam( *rPam.GetPoint() );
                    for( sal_uInt16 n = 0; n < aRangeArr.Count(); ++n )
                    {
                        // --> OD 2008-03-17 #refactorlists#
                        // no start of a new list
                        pDoc->SetNumRule( aRangeArr.SetPam( n, aPam ), aRule, false );
                        // <--
                    }
                    pDoc->EndUndo( UNDO_END, NULL );
                }
                else
                {
                    // --> OD 2008-03-17 #refactorlists#
                    // no start of a new list
                    pDoc->SetNumRule( rPam, aRule, false );
                    // <--
                }


            }
            else if(pSwNum->GetCreatedNumRuleName().Len())
            {
                UnoActionContext aAction(pDoc);
                SwNumRule* pRule = pDoc->FindNumRulePtr( pSwNum->GetCreatedNumRuleName() );
                if(!pRule)
                    throw RuntimeException();
                // --> OD 2008-03-17 #refactorlists#
                // no start of a new list
                pDoc->SetNumRule( rPam, *pRule, false );
                // <--
            }
            // --> OD 2009-08-18 #i103817#
            // outline numbering
            else
            {
                UnoActionContext aAction(pDoc);
                SwNumRule* pRule = pDoc->GetOutlineNumRule();
                if(!pRule)
                    throw RuntimeException();
                pDoc->SetNumRule( rPam, *pRule, false );
            }
            // <--
        }
    }
    else if(rValue.getValueType() == ::getVoidCppuType())
    {
        rPam.GetDoc()->DelNumRules(rPam);
    }
}


/* -----------------25.05.98 11:40-------------------
 *
 * --------------------------------------------------*/
void  getNumberingProperty(SwPaM& rPam, PropertyState& eState, Any * pAny )
{
    const SwNumRule* pNumRule = rPam.GetDoc()->GetCurrNumRule( *rPam.GetPoint() );
    if(pNumRule)
    {
        uno::Reference< XIndexReplace >  xNum = new SwXNumberingRules(*pNumRule);
        if ( pAny )
            pAny->setValue(&xNum, ::getCppuType((const uno::Reference<XIndexReplace>*)0));
        eState = PropertyState_DIRECT_VALUE;
    }
    else
        eState = PropertyState_DEFAULT_VALUE;
}
/* -----------------04.07.98 15:15-------------------
 *
 * --------------------------------------------------*/
void GetCurPageStyle(SwPaM& rPaM, String &rString)
{
    const SwPageFrm* pPage = rPaM.GetCntntNode()->GetFrm()->FindPageFrm();
    if(pPage)
        SwStyleNameMapper::FillProgName( pPage->GetPageDesc()->GetName(), rString, nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC, sal_True );
}
/* -----------------30.03.99 10:52-------------------
 * spezielle Properties am Cursor zuruecksetzen
 * --------------------------------------------------*/
void resetCrsrPropertyValue(const SfxItemPropertySimpleEntry& rEntry, SwPaM& rPam)
{
    SwDoc* pDoc = rPam.GetDoc();
    switch(rEntry.nWID)
    {
        case FN_UNO_PARA_STYLE :
//			lcl_SetTxtFmtColl(aValue, pUnoCrsr);
        break;
        case FN_UNO_PAGE_STYLE :
        break;
        case FN_UNO_NUM_START_VALUE  :
        {
            UnoActionContext aAction(pDoc);

            if( rPam.GetNext() != &rPam )			// Mehrfachselektion ?
            {
                pDoc->StartUndo( UNDO_START, NULL );
                SwPamRanges aRangeArr( rPam );
                SwPaM aPam( *rPam.GetPoint() );
                for( sal_uInt16 n = 0; n < aRangeArr.Count(); ++n )
                    pDoc->SetNodeNumStart( *aRangeArr.SetPam( n, aPam ).GetPoint(), 1 );
                pDoc->EndUndo( UNDO_END, NULL );
            }
            else
                pDoc->SetNodeNumStart( *rPam.GetPoint(), 0 );
        }

        break;
        case FN_UNO_NUM_LEVEL  :
        break;
        case FN_UNO_NUM_RULES:
//    		lcl_setNumberingProperty(aValue, pUnoCrsr);
        break;
        case FN_UNO_CHARFMT_SEQUENCE:
        {
            SvUShortsSort aWhichIds;
            aWhichIds.Insert(RES_TXTATR_CHARFMT);
            pDoc->ResetAttrs(rPam, sal_True, &aWhichIds);
        }
        break;
    }
}
/* -----------------21.07.98 11:36-------------------
 *
 * --------------------------------------------------*/
void InsertFile(SwUnoCrsr* pUnoCrsr,
    const String& rURL,
    const uno::Sequence< beans::PropertyValue >& rOptions
    ) throw( lang::IllegalArgumentException, io::IOException, uno::RuntimeException )
{
    SfxMedium* pMed = 0;
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    SwDocShell* pDocSh = pDoc->GetDocShell();
    comphelper::MediaDescriptor aMediaDescriptor( rOptions );
    ::rtl::OUString sFileName = rURL;
    ::rtl::OUString sFilterName, sFilterOptions, sPassword, sBaseURL;
    uno::Reference < io::XStream > xStream;
    uno::Reference < io::XInputStream > xInputStream;

    if( !sFileName.getLength() )
        aMediaDescriptor[comphelper::MediaDescriptor::PROP_URL()] >>= sFileName;
    if( !sFileName.getLength() )
        aMediaDescriptor[comphelper::MediaDescriptor::PROP_FILENAME()] >>= sFileName;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_INPUTSTREAM()] >>= xInputStream;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_STREAM()] >>= xStream;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_INPUTSTREAM()] >>= xInputStream;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_FILTERNAME()] >>= sFilterName;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_FILTEROPTIONS()] >>= sFilterOptions;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_PASSWORD()] >>= sPassword;
    aMediaDescriptor[comphelper::MediaDescriptor::PROP_DOCUMENTBASEURL() ] >>= sBaseURL;
    if ( !xInputStream.is() && xStream.is() )
        xInputStream = xStream->getInputStream();

    if(!pDocSh || (!sFileName.getLength() && !xInputStream.is()))
        return;

    SfxObjectFactory& rFact = pDocSh->GetFactory();
    const SfxFilter* pFilter = rFact.GetFilterContainer()->GetFilter4FilterName( sFilterName );
    uno::Reference < embed::XStorage > xReadStorage;
    if( xInputStream.is() )
    {
        uno::Sequence< uno::Any > aArgs( 2 );
        aArgs[0] <<= xInputStream;
        aArgs[1] <<= embed::ElementModes::READ;
        try
        {
            xReadStorage = uno::Reference< embed::XStorage >(
                            ::comphelper::OStorageHelper::GetStorageFactory()->createInstanceWithArguments( aArgs ),
                            uno::UNO_QUERY );
        }
        catch( const io::IOException& rEx)
        {
            (void)rEx;
        }
    }
    if ( !pFilter )
    {
        if( xInputStream.is() && !xReadStorage.is())
        {
            pMed = new SfxMedium;
            pMed->setStreamToLoadFrom(xInputStream, sal_True );
        }
        else
            pMed = xReadStorage.is() ?
                new SfxMedium(xReadStorage, sBaseURL, 0 ) :
                new SfxMedium(sFileName, STREAM_READ, sal_True, 0, 0 );
        if( sBaseURL.getLength() )
            pMed->GetItemSet()->Put( SfxStringItem( SID_DOC_BASEURL, sBaseURL ) );

        SfxFilterMatcher aMatcher( rFact.GetFilterContainer()->GetName() );
        ErrCode nErr = aMatcher.GuessFilter( *pMed, &pFilter, sal_False );
        if ( nErr || !pFilter)
            DELETEZ(pMed);
        else
            pMed->SetFilter( pFilter );
    }
    else
    {
        if(!pMed)
        {
            if( xInputStream.is() && !xReadStorage.is())
            {
                pMed = new SfxMedium;
                pMed->setStreamToLoadFrom(xInputStream, sal_True );
                pMed->SetFilter( pFilter );
            }
            else
            {
                if( xReadStorage.is() )
                {
                    pMed = new SfxMedium(xReadStorage, sBaseURL, 0 );
                    pMed->SetFilter( pFilter );
                }
                else
                    pMed = new SfxMedium(sFileName, STREAM_READ, sal_True, pFilter, 0);
            }
        }
        if(sFilterOptions.getLength())
            pMed->GetItemSet()->Put( SfxStringItem( SID_FILE_FILTEROPTIONS, sFilterOptions ) );
        if( sBaseURL.getLength())
            pMed->GetItemSet()->Put( SfxStringItem( SID_DOC_BASEURL, sBaseURL ) );
    }

    if( !pMed )
        return;

    SfxObjectShellRef aRef( pDocSh );

    pDocSh->RegisterTransfer( *pMed );
    pMed->DownLoad();	// ggfs. den DownLoad anstossen
    if( aRef.Is() && 1 < aRef->GetRefCount() )	// noch gueltige Ref?
    {
        SwReader* pRdr;
        SfxItemSet* pSet = 	pMed->GetItemSet();
        pSet->Put(SfxBoolItem(FN_API_CALL, sal_True));
        if(sPassword.getLength())
            pSet->Put(SfxStringItem(SID_PASSWORD, sPassword));
        Reader *pRead = pDocSh->StartConvertFrom( *pMed, &pRdr, 0, pUnoCrsr);
        if( pRead )
        {

            UnoActionContext aContext(pDoc);

            if(pUnoCrsr->HasMark())
                pDoc->DeleteAndJoin(*pUnoCrsr);

            SwNodeIndex aSave(  pUnoCrsr->GetPoint()->nNode, -1 );
            xub_StrLen nCntnt = pUnoCrsr->GetPoint()->nContent.GetIndex();

            sal_uInt32 nErrno = pRdr->Read( *pRead );   // und Dokument einfuegen

            if(!nErrno)
            {
                aSave++;
                pUnoCrsr->SetMark();
                pUnoCrsr->GetMark()->nNode = aSave;

                SwCntntNode* pCntNode = aSave.GetNode().GetCntntNode();
                if( !pCntNode )
                    nCntnt = 0;
                pUnoCrsr->GetMark()->nContent.Assign( pCntNode, nCntnt );
            }

            delete pRdr;

            // ggfs. alle Verzeichnisse updaten:
/*			if( pWrtShell->IsUpdateTOX() )
            {
                SfxRequest aReq( *this, FN_UPDATE_TOX );
                Execute( aReq );
                pWrtShell->SetUpdateTOX( sal_False );		// wieder zurueck setzen
            }*/

        }
    }
    delete pMed;
}

/* -----------------14.07.04 ------------------------
 *
 * --------------------------------------------------*/

// insert text and scan for CR characters in order to insert
// paragraph breaks at those positions by calling SplitNode
sal_Bool DocInsertStringSplitCR(
        SwDoc &rDoc,
        const SwPaM &rNewCursor, const String &rText,
        const bool bForceExpandHints )
{
    sal_Bool bOK = sal_True;

        const enum IDocumentContentOperations::InsertFlags nInsertFlags =
            (bForceExpandHints)
            ? static_cast<IDocumentContentOperations::InsertFlags>(
                    IDocumentContentOperations::INS_FORCEHINTEXPAND |
                    IDocumentContentOperations::INS_EMPTYEXPAND)
            : IDocumentContentOperations::INS_EMPTYEXPAND;

    OUString aTxt;
    xub_StrLen nStartIdx = 0;
    SwTxtNode* const pTxtNd =
        rNewCursor.GetPoint()->nNode.GetNode().GetTxtNode();
    const xub_StrLen nMaxLength = ( pTxtNd )
        ? STRING_LEN - pTxtNd->GetTxt().Len()
        : STRING_LEN;
    xub_StrLen nIdx = rText.Search( '\r', nStartIdx );
    if( ( nIdx == STRING_NOTFOUND && nMaxLength < rText.Len() ) ||
        ( nIdx != STRING_NOTFOUND && nMaxLength < nIdx ) )
    {
        nIdx = nMaxLength;
    }
    while (nIdx != STRING_NOTFOUND )
    {
        DBG_ASSERT( nIdx - nStartIdx >= 0, "index negative!" );
        aTxt = rText.Copy( nStartIdx, nIdx - nStartIdx );
        if (aTxt.getLength() &&
            !rDoc.InsertString( rNewCursor, aTxt, nInsertFlags ))
        {
            DBG_ERROR( "Doc->Insert(Str) failed." );
            bOK = sal_False;
        }
        if (!rDoc.SplitNode( *rNewCursor.GetPoint(), false ) )
        {
            DBG_ERROR( "SplitNode failed" );
            bOK = sal_False;
        }
        nStartIdx = nIdx + 1;
        nIdx = rText.Search( '\r', nStartIdx );
    }
    aTxt = rText.Copy( nStartIdx );
    if (aTxt.getLength() &&
        !rDoc.InsertString( rNewCursor, aTxt, nInsertFlags ))
    {
        DBG_ERROR( "Doc->Insert(Str) failed." );
        bOK = sal_False;
    }

    return bOK;
}
/*-- 10.03.2008 09:58:47---------------------------------------------------

  -----------------------------------------------------------------------*/
void makeRedline( SwPaM& rPaM,
    const ::rtl::OUString& rRedlineType,
    const uno::Sequence< beans::PropertyValue >& rRedlineProperties )
        throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    IDocumentRedlineAccess* pRedlineAccess = rPaM.GetDoc();

    RedlineType_t eType = nsRedlineType_t::REDLINE_INSERT;
    if( rRedlineType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "Delete" ) ))
        eType = nsRedlineType_t::REDLINE_DELETE;
    else if( rRedlineType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "Format" ) ))
        eType = nsRedlineType_t::REDLINE_FORMAT;
    else if( rRedlineType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "TextTable" ) ))
        eType = nsRedlineType_t::REDLINE_TABLE;
    else if( !rRedlineType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "Insert" ) ))
        throw lang::IllegalArgumentException();

    //todo: what about REDLINE_FMTCOLL?
    comphelper::SequenceAsHashMap aPropMap( rRedlineProperties );
    uno::Any aAuthorValue;
    aAuthorValue = aPropMap.getUnpackedValueOrDefault( ::rtl::OUString::createFromAscii("RedlineAuthor"), aAuthorValue);
    USHORT nAuthor = 0;
    ::rtl::OUString sAuthor;
    if( aAuthorValue >>= sAuthor )
        nAuthor = pRedlineAccess->InsertRedlineAuthor(sAuthor);

    ::rtl::OUString sComment;
    uno::Any aCommentValue;
    aCommentValue = aPropMap.getUnpackedValueOrDefault( ::rtl::OUString::createFromAscii("RedlineComment"), aCommentValue);

    SwRedlineData aRedlineData( eType, nAuthor );
    if( aCommentValue >>= sComment )
        aRedlineData.SetComment( sComment );

    ::util::DateTime aStamp;
    uno::Any aDateTimeValue;
    aDateTimeValue = aPropMap.getUnpackedValueOrDefault( ::rtl::OUString::createFromAscii("RedlineDateTime"), aDateTimeValue);
    if( aDateTimeValue >>= aStamp )
    {
       aRedlineData.SetTimeStamp(
        DateTime( Date( aStamp.Day, aStamp.Month, aStamp.Year ), Time( aStamp.Hours, aStamp.Minutes, aStamp.Seconds ) ) );
    }

    SwRedline* pRedline = new SwRedline( aRedlineData, rPaM );
    RedlineMode_t nPrevMode = pRedlineAccess->GetRedlineMode( );

    pRedlineAccess->SetRedlineMode_intern(nsRedlineMode_t::REDLINE_ON);
    bool bRet = pRedlineAccess->AppendRedline( pRedline, false );
    pRedlineAccess->SetRedlineMode_intern( nPrevMode );
    if( !bRet )
        throw lang::IllegalArgumentException();
}

/*-- 19.02.2009 09:27:26---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAnyMapHelper::~SwAnyMapHelper()
{
    AnyMapHelper_t::iterator aIt = begin();
    while( aIt != end() )
    {
        delete ( aIt->second );
        ++aIt;
    }
}
/*-- 19.02.2009 09:27:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAnyMapHelper::SetValue( USHORT nWhichId, USHORT nMemberId, const uno::Any& rAny )
{
    sal_uInt32 nKey = (nWhichId << 16) + nMemberId;
    AnyMapHelper_t::iterator aIt = find( nKey );
    if( aIt != end() )
    {
        *(aIt->second) = rAny;
    }
    else
        insert( value_type(nKey, new uno::Any( rAny )) );
}
/*-- 19.02.2009 09:27:26---------------------------------------------------

  -----------------------------------------------------------------------*/
bool    SwAnyMapHelper::FillValue( USHORT nWhichId, USHORT nMemberId, const uno::Any*& pAny )
{
    bool bRet = false;
    sal_uInt32 nKey = (nWhichId << 16) + nMemberId;
    AnyMapHelper_t::iterator aIt = find( nKey );
    if( aIt != end() )
    {
        pAny = aIt->second;
        bRet = true;
    }
    return bRet;
}

}//namespace SwUnoCursorHelper

