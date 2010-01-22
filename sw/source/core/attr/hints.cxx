/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: hints.cxx,v $
 * $Revision: 1.16 $
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
#include <hints.hxx>

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#include <svx/scripttypeitem.hxx>
#include <hintids.hxx>
#include <swtypes.hxx>
#include <ndtxt.hxx>
#include <errhdl.hxx>

SwFmtChg::SwFmtChg( SwFmt *pFmt )
    : SwMsgPoolItem( RES_FMT_CHG ),
    pChangedFmt( pFmt )
{}



SwInsTxt::SwInsTxt( xub_StrLen nP, xub_StrLen nL )
    : SwMsgPoolItem( RES_INS_TXT ),
    nPos( nP ),
    nLen( nL )
{}



SwDelChr::SwDelChr( xub_StrLen nP )
    : SwMsgPoolItem( RES_DEL_CHR ),
    nPos( nP )
{}



SwDelTxt::SwDelTxt( xub_StrLen nS, xub_StrLen nL )
    : SwMsgPoolItem( RES_DEL_TXT ),
    nStart( nS ),
    nLen( nL )
{}



SwUpdateAttr::SwUpdateAttr( xub_StrLen nS, xub_StrLen nE, USHORT nW )
    : SwMsgPoolItem( RES_UPDATE_ATTR ),
    nStart( nS ),
    nEnd( nE ),
    nWhichAttr( nW )
{}


// SwRefMarkFldUpdate wird verschickt, wenn sich die ReferenzMarkierungen
// Updaten sollen. Um Seiten-/KapitelNummer feststellen zu koennen, muss
// der akt. Frame befragt werden. Dafuer wird das akt. OutputDevice benoetigt.


SwRefMarkFldUpdate::SwRefMarkFldUpdate( const OutputDevice* pOutput )
    : SwMsgPoolItem( RES_REFMARKFLD_UPDATE ),
    pOut( pOutput )
{
    ASSERT( pOut, "es muss ein OutputDevice-Pointer gesetzt werden!" );
}


SwDocPosUpdate::SwDocPosUpdate( const SwTwips nDcPos )
    : SwMsgPoolItem( RES_DOCPOS_UPDATE ),
    nDocPos(nDcPos)
{}



// SwTableFmlUpdate wird verschickt, wenn sich die Tabelle neu berechnen soll
SwTableFmlUpdate::SwTableFmlUpdate( const SwTable* pNewTbl )
    : SwMsgPoolItem( RES_TABLEFML_UPDATE ),
    pTbl( pNewTbl ), pHistory( 0 ), nSplitLine( USHRT_MAX ),
    eFlags( TBL_CALC )
{
    DATA.pDelTbl = 0;
    bModified = bBehindSplitLine = FALSE;
    ASSERT( pTbl, "es muss ein Table-Pointer gesetzt werden!" );
}


SwAutoFmtGetDocNode::SwAutoFmtGetDocNode( const SwNodes* pNds )
    : SwMsgPoolItem( RES_AUTOFMT_DOCNODE ),
    pCntntNode( 0 ), pNodes( pNds )
{}


SwAttrSetChg::SwAttrSetChg( const SwAttrSet& rTheSet, SwAttrSet& rSet )
    : SwMsgPoolItem( RES_ATTRSET_CHG ),
    bDelSet( FALSE ),
    pChgSet( &rSet ),
    pTheChgdSet( &rTheSet )
{}


SwAttrSetChg::SwAttrSetChg( const SwAttrSetChg& rChgSet )
    : SwMsgPoolItem( RES_ATTRSET_CHG ),
    bDelSet( TRUE ),
    pTheChgdSet( rChgSet.pTheChgdSet )
{
    pChgSet = new SwAttrSet( *rChgSet.pChgSet );
}


SwAttrSetChg::~SwAttrSetChg()
{
    if( bDelSet )
        delete pChgSet;
}


#ifdef DBG_UTIL

void SwAttrSetChg::ClearItem( USHORT nWhch )
{
    ASSERT( bDelSet, "der Set darf nicht veraendert werden!" );
    pChgSet->ClearItem( nWhch );
}

#endif


SwMsgPoolItem::SwMsgPoolItem( USHORT nWhch )
    : SfxPoolItem( nWhch )
{}


// "Overhead" vom SfxPoolItem
int SwMsgPoolItem::operator==( const SfxPoolItem& ) const
{
    ASSERT( FALSE, "SwMsgPoolItem kennt kein ==" );
    return 0;
}


SfxPoolItem* SwMsgPoolItem::Clone( SfxItemPool* ) const
{
    ASSERT( FALSE, "SwMsgPoolItem kennt kein Clone" );
    return 0;
}

/******************************************************************************
 * hole aus der Default-Attribut Tabelle ueber den Which-Wert
 * das entsprechende default Attribut.
 * Ist keines vorhanden, returnt ein 0-Pointer !!!
 * Used to be inlined (hintids.hxx) in PRODUCT.
 ******************************************************************************/
#ifndef DBG_UTIL
const SfxPoolItem* GetDfltAttr( USHORT nWhich )
{
    return aAttrTab[ nWhich - POOLATTR_BEGIN ];
}
#else
const SfxPoolItem* GetDfltAttr( USHORT nWhich )
{
    ASSERT_ID( nWhich < POOLATTR_END && nWhich >= POOLATTR_BEGIN,
               ERR_OUTOFSCOPE );

    SfxPoolItem *pHt = aAttrTab[ nWhich - POOLATTR_BEGIN ];
    ASSERT( pHt, "GetDfltFmtAttr(): Dflt == 0" );
    return pHt;
}
#endif



SwCondCollCondChg::SwCondCollCondChg( SwFmt *pFmt )
    : SwMsgPoolItem( RES_CONDCOLL_CONDCHG ), pChangedFmt( pFmt )
{
}


SwVirtPageNumInfo::SwVirtPageNumInfo( const SwPageFrm *pPg ) :
    SwMsgPoolItem( RES_VIRTPAGENUM_INFO ),
    pPage( 0 ),
    pOrigPage( pPg ),
    pFrm( 0 )
{
}

// --> OD 2008-02-19 #refactorlists#
//SwNumRuleInfo::SwNumRuleInfo( const String& rRuleName )
//    : SwMsgPoolItem( RES_GETNUMNODES ), rName( rRuleName )
//{
//}

//void SwNumRuleInfo::AddNode( SwTxtNode& rNd )
//{
//    aList.Insert(rNd.GetIndex(), &rNd);
//}


SwFindNearestNode::SwFindNearestNode( const SwNode& rNd )
    : SwMsgPoolItem( RES_FINDNEARESTNODE ), pNd( &rNd ), pFnd( 0 )
{
}

void SwFindNearestNode::CheckNode( const SwNode& rNd )
{
    if( &pNd->GetNodes() == &rNd.GetNodes() )
    {
        ULONG nIdx = rNd.GetIndex();
        if( nIdx < pNd->GetIndex() &&
            ( !pFnd || nIdx > pFnd->GetIndex() ) &&
            nIdx > rNd.GetNodes().GetEndOfExtras().GetIndex() )
            pFnd = &rNd;
    }
}



USHORT GetWhichOfScript( USHORT nWhich, USHORT nScript )
{
    static const USHORT aLangMap[3] =
        { RES_CHRATR_LANGUAGE, RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CTL_LANGUAGE };
    static const USHORT aFontMap[3] =
        { RES_CHRATR_FONT, RES_CHRATR_CJK_FONT,  RES_CHRATR_CTL_FONT};
    static const USHORT aFontSizeMap[3] =
        { RES_CHRATR_FONTSIZE, RES_CHRATR_CJK_FONTSIZE,  RES_CHRATR_CTL_FONTSIZE };
    static const USHORT aWeightMap[3] =
        { RES_CHRATR_WEIGHT, RES_CHRATR_CJK_WEIGHT,  RES_CHRATR_CTL_WEIGHT};
    static const USHORT aPostureMap[3] =
        { RES_CHRATR_POSTURE, RES_CHRATR_CJK_POSTURE,  RES_CHRATR_CTL_POSTURE};

    const USHORT* pM;
    switch( nWhich )
    {
    case RES_CHRATR_LANGUAGE:
    case RES_CHRATR_CJK_LANGUAGE:
    case RES_CHRATR_CTL_LANGUAGE:
        pM = aLangMap;
        break;

    case RES_CHRATR_FONT:
    case RES_CHRATR_CJK_FONT:
    case RES_CHRATR_CTL_FONT:
        pM = aFontMap;
        break;

    case RES_CHRATR_FONTSIZE:
    case RES_CHRATR_CJK_FONTSIZE:
    case RES_CHRATR_CTL_FONTSIZE:
        pM = aFontSizeMap;
        break;

    case  RES_CHRATR_WEIGHT:
    case  RES_CHRATR_CJK_WEIGHT:
    case  RES_CHRATR_CTL_WEIGHT:
        pM = aWeightMap;
        break;
    case RES_CHRATR_POSTURE:
    case RES_CHRATR_CJK_POSTURE:
    case RES_CHRATR_CTL_POSTURE:
        pM = aPostureMap;
        break;

    default:
        pM = 0;
    }

    USHORT nRet;
    if( pM )
    {
        using namespace ::com::sun::star::i18n;
        {
            if( ScriptType::WEAK == nScript )
                nScript = GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() );
            switch( nScript)
            {
            case ScriptType::COMPLEX:	++pM;  // no break;
            case ScriptType::ASIAN:		++pM;  // no break;
            default:					nRet = *pM;
            }
        }
    }
    else
        nRet = nWhich;
    return nRet;
}
