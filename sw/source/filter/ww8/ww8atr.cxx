/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ww8atr.cxx,v $
 * $Revision: 1.113.40.3 $
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
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * This file contains methods for the WW8 output
 * (nodes, attributes, formats und chars).
 */

#include <hintids.hxx>

#include <vcl/svapp.hxx>
#include <vcl/salbtype.hxx>
#include <svtools/zformat.hxx>
#include <svtools/itemiter.hxx>
#include <svtools/whiter.hxx>
#include <svx/fontitem.hxx>
#include <svx/tstpitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/spltitem.hxx>
#include <svx/widwitem.hxx>
#include <svx/lspcitem.hxx>
#include <svx/keepitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/brshitem.hxx>
#include <svx/postitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/kernitem.hxx>
#include <svx/crsditem.hxx>
#include <svx/cmapitem.hxx>
#include <svx/wrlmitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/langitem.hxx>
#include <svx/escpitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/colritem.hxx>
#include <svx/hyznitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/cntritem.hxx>
#include <svx/shdditem.hxx>
#include <svx/akrnitem.hxx>
#include <svx/pbinitem.hxx>
#include <svx/emphitem.hxx>
#include <svx/twolinesitem.hxx>
#include <svx/charscaleitem.hxx>
#include <svx/charrotateitem.hxx>
#include <svx/charreliefitem.hxx>
#include <svx/paravertalignitem.hxx>
#include <svx/pgrditem.hxx>
#include <svx/frmdiritem.hxx>
#include <svx/blnkitem.hxx>
#include <svx/charhiddenitem.hxx>
#include <svx/paperinf.hxx>
#include <fmtfld.hxx>
#include <fchrfmt.hxx>
#include <fmtfsize.hxx>
#include <fmtpdsc.hxx>
#include <fmtornt.hxx>
#include <fmtanchr.hxx>
#include <fmtclds.hxx>
#include <fmtsrnd.hxx>
#include <fmtftn.hxx>
#include <fmtflcnt.hxx>
#include <frmatr.hxx>
#include <swtable.hxx>
#include <fmtinfmt.hxx>
#include <txtfld.hxx>
#include <txtftn.hxx>
#include <poolfmt.hxx>
#include <doc.hxx>          // Doc for footnotes
#include <pam.hxx>
#include <paratr.hxx>
#include <fldbas.hxx>       // for SwField
#include <docufld.hxx>      // for SwField
#include <expfld.hxx>
#include <pagedesc.hxx>     // for SwPageDesc
#include <flddat.hxx>       // for Date fields
#include <ndtxt.hxx>        // for Numrules
#include <fmthbsh.hxx>
#include <swrect.hxx>
#include <reffld.hxx>
#include <ftninfo.hxx>
#include <charfmt.hxx>
#include <section.hxx>
#include <lineinfo.hxx>
#include <fmtline.hxx>
#include <tox.hxx>
#include <fmtftntx.hxx>
#include <breakit.hxx>
#include <com/sun/star/i18n/ScriptType.hdl>
#include <unotools/localedatawrapper.hxx>
#include <tgrditem.hxx>
#include <flddropdown.hxx>
#include <chpfld.hxx>
#include <fmthdft.hxx>

#include <writerfilter/doctok/sprmids.hxx>

#if OSL_DEBUG_LEVEL > 1
#  include <fmtcntnt.hxx>
#endif
#include "writerhelper.hxx"
#include "writerwordglue.hxx"
#include "wrtww8.hxx"
#include "ww8par.hxx"
#include "ww8attributeoutput.hxx"
#include "fields.hxx"
#include <vcl/outdev.hxx>
#include <i18npool/mslangid.hxx>

using namespace ::com::sun::star;
using namespace nsFieldFlags;
using namespace nsSwDocInfoSubType;

/*
 * um nicht immer wieder nach einem Update festzustellen, das irgendwelche
 * Hint-Ids dazugekommen sind, wird hier definiert, die Groesse der Tabelle
 * definiert und mit der akt. verglichen. Bei unterschieden wird der
 * Compiler schon meckern.
 *
 * diese Section und die dazugeherigen Tabellen muessen in folgenden Files
 * gepflegt werden: rtf\rtfatr.cxx, sw6\sw6atr.cxx, w4w\w4watr.cxx
*/

#if !defined(MSC) && !defined(UNX) && !defined(PPC) && !defined(CSET) && !defined(__MWERKS__) && !defined(WTC) && !defined(__MINGW32__) && !defined(OS2)

#define ATTRFNTAB_SIZE 130
#if ATTRFNTAB_SIZE != POOLATTR_END - POOLATTR_BEGIN
#   error "Attribut-Tabelle ist ungueltigt. Wurden neue Hint-ID's zugefuegt ??"
#endif

#define NODETAB_SIZE 3
#if NODETAB_SIZE != RES_NODE_END - RES_NODE_BEGIN
#   error "Node-Tabelle ist ungueltigt. Wurden neue Hint-ID's zugefuegt ??"
#endif

#endif

using namespace sw::util;
using namespace sw::types;

bool WW8Export::CollapseScriptsforWordOk( USHORT nScript, USHORT nWhich )
{
    bool bRet = true;
    if ( nScript == i18n::ScriptType::ASIAN )
    {
        //for asian in ww8, there is only one fontsize
        //and one fontstyle (posture/weight) for ww6
        //there is the additional problem that there
        //is only one font setting for all three scripts
        switch ( nWhich )
        {
            case RES_CHRATR_FONTSIZE:
            case RES_CHRATR_POSTURE:
            case RES_CHRATR_WEIGHT:
                bRet = false;
                break;
            case RES_CHRATR_LANGUAGE:
            case RES_CHRATR_CTL_FONT:
            case RES_CHRATR_CTL_FONTSIZE:
            case RES_CHRATR_CTL_LANGUAGE:
            case RES_CHRATR_CTL_POSTURE:
            case RES_CHRATR_CTL_WEIGHT:
                if (bWrtWW8 == 0)
                    bRet = false;
            default:
                break;
        }
    }
    else if ( nScript == i18n::ScriptType::COMPLEX )
    {
        //Complex is ok in ww8, but for ww6 there is only
        //one font, one fontsize, one fontsize (weight/posture)
        //and only one language
        if ( bWrtWW8 == 0 )
        {
            switch ( nWhich )
            {
                case RES_CHRATR_CJK_FONT:
                case RES_CHRATR_CJK_FONTSIZE:
                case RES_CHRATR_CJK_POSTURE:
                case RES_CHRATR_CJK_WEIGHT:
                case RES_CHRATR_CJK_LANGUAGE:
                case RES_CHRATR_FONT:
                case RES_CHRATR_FONTSIZE:
                case RES_CHRATR_POSTURE:
                case RES_CHRATR_WEIGHT:
                case RES_CHRATR_LANGUAGE:
                    bRet = false;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        //for western in ww8, there is only one fontsize
        //and one fontstyle (posture/weight) for ww6
        //there is the additional problem that there
        //is only one font setting for all three scripts
        switch ( nWhich )
        {
            case RES_CHRATR_CJK_FONTSIZE:
            case RES_CHRATR_CJK_POSTURE:
            case RES_CHRATR_CJK_WEIGHT:
                bRet = false;
                break;
            case RES_CHRATR_CJK_LANGUAGE:
            case RES_CHRATR_CTL_FONT:
            case RES_CHRATR_CTL_FONTSIZE:
            case RES_CHRATR_CTL_LANGUAGE:
            case RES_CHRATR_CTL_POSTURE:
            case RES_CHRATR_CTL_WEIGHT:
                if ( bWrtWW8 == 0 )
                    bRet = false;
            default:
                break;
        }
    }
    return bRet;
}

//------------------------------------------------------------
//  Hilfsroutinen fuer Styles
//------------------------------------------------------------

void MSWordExportBase::ExportPoolItemsToCHP( sw::PoolItems &rItems, USHORT nScript )
{
    sw::cPoolItemIter aEnd = rItems.end();
    for ( sw::cPoolItemIter aI = rItems.begin(); aI != aEnd; ++aI )
    {
        const SfxPoolItem *pItem = aI->second;
        USHORT nWhich = pItem->Which();
        if ( ( isCHRATR( nWhich ) || isTXTATR( nWhich ) ) && CollapseScriptsforWordOk( nScript, nWhich ) )
        {
            AttrOutput().OutputItem( *pItem );
        }
    }
}

/*
 * Format wie folgt ausgeben:
 *      - gebe die Attribute aus; ohne Parents!
 */

void MSWordExportBase::OutputItemSet( const SfxItemSet& rSet, bool bPapFmt, bool bChpFmt, USHORT nScript )
{
    if ( rSet.Count() )
    {
        const SfxPoolItem* pItem;
        pISet = &rSet;                  // fuer Doppel-Attribute

        // If frame dir is set, but not adjust, then force adjust as well
        if ( bPapFmt && SFX_ITEM_SET == rSet.GetItemState( RES_FRAMEDIR, false ) )
        {
            // No explicit adjust set ?
            if ( SFX_ITEM_SET != rSet.GetItemState( RES_PARATR_ADJUST, false ) )
            {
                if ( 0 != ( pItem = rSet.GetItem( RES_PARATR_ADJUST ) ) )
                {
                    // then set the adjust used by the parent format
                    AttrOutput().OutputItem( *pItem );
                }
            }
        }

        if ( bPapFmt && SFX_ITEM_SET == rSet.GetItemState( RES_PARATR_NUMRULE, false, &pItem ) )
        {
            AttrOutput().OutputItem( *pItem );

            // switch off the numerbering?
            if ( !( (SwNumRuleItem*)pItem )->GetValue().Len() &&
                 SFX_ITEM_SET != rSet.GetItemState( RES_LR_SPACE, false) &&
                 SFX_ITEM_SET == rSet.GetItemState( RES_LR_SPACE, true, &pItem ) )
            {
                // the set the LR-Space of the parentformat!
                AttrOutput().OutputItem( *pItem );
            }
        }

        sw::PoolItems aItems;
        GetPoolItems( rSet, aItems );
        if ( bChpFmt )
            ExportPoolItemsToCHP(aItems, nScript);

        if ( bPapFmt )
        {
            sw::cPoolItemIter aEnd = aItems.end();
            for ( sw::cPoolItemIter aI = aItems.begin(); aI != aEnd; ++aI )
            {
                pItem = aI->second;
                USHORT nWhich = pItem->Which();
                if ( nWhich >= RES_PARATR_BEGIN && nWhich < RES_FRMATR_END && nWhich != RES_PARATR_NUMRULE)
                    AttrOutput().OutputItem( *pItem );
            }
        }
        pISet = 0;                      // fuer Doppel-Attribute
    }
}

void MSWordExportBase::GatherChapterFields()
{
    //If the header/footer contains a chapter field
    SwClientIter aIter(*pDoc->GetSysFldType(RES_CHAPTERFLD));
    const SwClient *pField = aIter.First(TYPE(SwFmtFld));
    while (pField)
    {
        const SwFmtFld* pFld = (const SwFmtFld*)(pField);
        if (const SwTxtFld *pTxtFld = pFld->GetTxtFld())
        {
            const SwTxtNode &rTxtNode = pTxtFld->GetTxtNode();
            maChapterFieldLocs.push_back(rTxtNode.GetIndex());
        }
       pField = aIter.Next();
    }
}

bool MSWordExportBase::CntntContainsChapterField(const SwFmtCntnt &rCntnt) const
{
    bool bRet = false;
    if ( const SwNodeIndex* pSttIdx = rCntnt.GetCntntIdx() )
    {
        SwNodeIndex aIdx( *pSttIdx, 1 );
        SwNodeIndex aEnd( *pSttIdx->GetNode().EndOfSectionNode() );
        ULONG nStart = aIdx.GetIndex();
        ULONG nEnd = aEnd.GetIndex();
        //If the header/footer contains a chapter field
        mycCFIter aIEnd = maChapterFieldLocs.end();
        for ( mycCFIter aI = maChapterFieldLocs.begin(); aI != aIEnd; ++aI )
        {
            if ( ( nStart <= *aI ) && ( *aI <= nEnd ) )
            {
                bRet = true;
                break;
            }
        }
    }
    return bRet;
}

bool MSWordExportBase::FmtHdFtContainsChapterField(const SwFrmFmt &rFmt) const
{
    if ( maChapterFieldLocs.empty() )
        return false;

    const SwFrmFmt *pFmt = 0;

    pFmt = rFmt.GetHeader().GetHeaderFmt();
    if ( pFmt && CntntContainsChapterField( pFmt->GetCntnt() ) )
        return true;

    pFmt = rFmt.GetFooter().GetFooterFmt();
    if ( pFmt && CntntContainsChapterField( pFmt->GetCntnt() ) )
        return true;

    return false;
}

bool MSWordExportBase::SetAktPageDescFromNode(const SwNode &rNd)
{
    bool bNewPageDesc = false;
    const SwPageDesc* pCurrent = SwPageDesc::GetPageDescOfNode(rNd);
    ASSERT(pCurrent && pAktPageDesc, "Not possible surely");
    if (pAktPageDesc && pCurrent)
    {
        if (pCurrent != pAktPageDesc)
        {
            if (pAktPageDesc->GetFollow() != pCurrent)
                bNewPageDesc = true;
            else
            {
                const SwFrmFmt& rTitleFmt = pAktPageDesc->GetMaster();
                const SwFrmFmt& rFollowFmt = pCurrent->GetMaster();

                bNewPageDesc = !IsPlausableSingleWordSection(rTitleFmt,
                    rFollowFmt);
            }
            pAktPageDesc = pCurrent;
        }
        else
        {
            const SwFrmFmt &rFmt = pCurrent->GetMaster();
            bNewPageDesc = FmtHdFtContainsChapterField(rFmt);
        }
    }
    return bNewPageDesc;
}

// Da WW nur Break-After ( Pagebreak und Sectionbreaks ) kennt, im SW aber
// Bagebreaks "vor" und "nach" und Pagedescs nur "vor" existieren, werden
// die Breaks 2* durchgeklimpert, naemlich vor und hinter jeder Zeile.
// Je nach BreakTyp werden sie vor oder nach der Zeile gesetzt.
// Es duerfen nur Funktionen gerufen werden, die nicht in den
// Ausgabebereich pO schreiben, da dieser nur einmal fuer CHP und PAP existiert
// und damit im falschen landen wuerden.
void MSWordExportBase::OutputSectionBreaks( const SfxItemSet *pSet, const SwNode& rNd )
{
    if ( bStyDef || bOutKF || bInWriteEscher || bOutPageDescs )
        return;

    bBreakBefore = true;

    bool bNewPageDesc = false;
    const SfxPoolItem* pItem=0;
    const SwFmtPageDesc *pPgDesc=0;

    //Output a sectionbreak if theres a new pagedesciptor.  otherwise output a
    //pagebreak if there is a pagebreak here, unless the new page (follow
    //style) is different to the current one, in which case plump for a
    //section.
    bool bBreakSet = false;

    if ( pSet && pSet->Count() )
    {
        if ( SFX_ITEM_SET == pSet->GetItemState( RES_PAGEDESC, false, &pItem )
             && ( (SwFmtPageDesc*)pItem )->GetRegisteredIn() )
        {
            bBreakSet = true;
            bNewPageDesc = true;
            pPgDesc = (const SwFmtPageDesc*)pItem;
            pAktPageDesc = pPgDesc->GetPageDesc();
        }
        else if ( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK, false, &pItem ) )
        {
            // --> FME 2007-05-30 #146867# Word does not like hard break attributes in some table cells
            bool bRemoveHardBreakInsideTable = false;
            if ( bOutTable )
            {
                const SwTableNode* pTableNode = rNd.FindTableNode();
                if ( pTableNode )
                {
                    const SwTableBox* pBox = rNd.GetTblBox();
                    const SwTableLine* pLine = pBox ? pBox->GetUpper() : 0;
                    // but only for non-complex tables
                    if ( pLine && !pLine->GetUpper() )
                    {
                        // check if box is not first in that line:
                        if ( 0 < pLine->GetTabBoxes().GetPos( pBox ) && pBox->GetSttNd() )
                        {
                            bRemoveHardBreakInsideTable = true;
                        }
                    }
                }
            }
            // <--

            bBreakSet = true;

            if ( !bRemoveHardBreakInsideTable )
            {
                ASSERT(pAktPageDesc, "should not be possible");
                /*
                 If because of this pagebreak the page desc following the page
                 break is the follow style of the current page desc then output a
                 section break using that style instead.  At least in those cases
                 we end up with the same style in word and writer, nothing can be
                 done when it happens when we get a new pagedesc because we
                 overflow from the first page style.
                */
                if ( pAktPageDesc )
                {
                    // --> OD 2007-05-30 #i76301#
                    // assure that there is a page break before set at the node.
                    const SvxFmtBreakItem* pBreak = dynamic_cast<const SvxFmtBreakItem*>(pItem);
                    if ( pBreak &&
                         pBreak->GetBreak() == SVX_BREAK_PAGE_BEFORE )
                    {
                        bNewPageDesc = SetAktPageDescFromNode( rNd );
                    }
                    // <--
                }
                if ( !bNewPageDesc )
                    AttrOutput().OutputItem( *pItem );
            }
        }
    }

    /*
    #i9301#
    No explicit page break, lets see if the style had one and we've moved to a
    new page style because of it, if we have to then we take the opportunity to
    set the equivalent word section here. We *could* do it for every paragraph
    that moves onto a new page because of layout, but that would be insane.
    */
    bool bHackInBreak = false;
    if ( !bBreakSet )
    {
        if ( const SwCntntNode *pNd = rNd.GetCntntNode() )
        {
            const SvxFmtBreakItem &rBreak =
                ItemGet<SvxFmtBreakItem>( *pNd, RES_BREAK );
            if ( rBreak.GetBreak() == SVX_BREAK_PAGE_BEFORE )
                bHackInBreak = true;
            else
            {   // Even a pagedesc item is set, the break item can be set 'NONE',
                // but a pagedesc item is an implicit page break before...
                const SwFmtPageDesc &rPageDesc =
                    ItemGet<SwFmtPageDesc>( *pNd, RES_PAGEDESC );
                if ( rPageDesc.GetRegisteredIn() )
                    bHackInBreak = true;
            }
        }
    }

    if ( bHackInBreak )
    {
        ASSERT( pAktPageDesc, "should not be possible" );
        if ( pAktPageDesc )
            bNewPageDesc = SetAktPageDescFromNode( rNd );
    }

    if ( bNewPageDesc && pAktPageDesc )
    {
        PrepareNewPageDesc( pSet, rNd, pPgDesc, pAktPageDesc );
    }
    bBreakBefore = false;
}

// --> OD 2007-05-29 #i76300#
bool MSWordExportBase::OutputFollowPageDesc( const SfxItemSet* pSet, const SwTxtNode* pNd )
{
    bool bRet = false;

    if ( pNd &&
         pAktPageDesc &&
         pAktPageDesc != pAktPageDesc->GetFollow() )
    {
        PrepareNewPageDesc( pSet, *pNd, 0, pAktPageDesc->GetFollow() );
        bRet = true;
    }

    return bRet;
}

const SwSectionFmt* MSWordExportBase::GetSectionFormat( const SwNode& rNd ) const
{
    const SwSectionFmt* pFmt = NULL;
    const SwSectionNode* pSect = rNd.FindSectionNode();
    if ( pSect &&
         CONTENT_SECTION == pSect->GetSection().GetType() )
    {
        pFmt = pSect->GetSection().GetFmt();
    }

    return pFmt;
}

ULONG MSWordExportBase::GetSectionLineNo( const SfxItemSet* pSet, const SwNode& rNd ) const
{
    const SwFmtLineNumber* pNItem = 0;
    if ( pSet )
    {
        pNItem = &( ItemGet<SwFmtLineNumber>( *pSet, RES_LINENUMBER ) );
    }
    else if ( const SwCntntNode *pNd = rNd.GetCntntNode() )
    {
        pNItem = &( ItemGet<SwFmtLineNumber>( *pNd, RES_LINENUMBER ) );
    }

    return pNItem? pNItem->GetStartValue() : 0;
}

void WW8Export::PrepareNewPageDesc( const SfxItemSet*pSet,
                                      const SwNode& rNd,
                                      const SwFmtPageDesc* pNewPgDescFmt,
                                      const SwPageDesc* pNewPgDesc )
{
    // Die PageDescs werden beim Auftreten von PageDesc-Attributen nur in
    // WW8Writer::pSepx mit der entsprechenden Position eingetragen.  Das
    // Aufbauen und die Ausgabe der am PageDesc haengenden Attribute und
    // Kopf/Fusszeilen passiert nach dem Haupttext und seinen Attributen.

    ULONG nFcPos = ReplaceCr( msword::PageBreak ); // Page/Section-Break

    // tatsaechlich wird hier NOCH NICHTS ausgegeben, sondern
    // nur die Merk-Arrays aCps, aSects entsprechend ergaenzt
    if ( !nFcPos )
        return;

    const SwSectionFmt* pFmt = GetSectionFormat( rNd );
    const ULONG nLnNm = GetSectionLineNo( pSet, rNd );

    ASSERT( pNewPgDescFmt || pNewPgDesc, "Neither page desc format nor page desc provided." );

    if ( pNewPgDescFmt )
    {
        pSepx->AppendSep( Fc2Cp( nFcPos ), *pNewPgDescFmt, rNd, pFmt, nLnNm );
    }
    else if ( pNewPgDesc )
    {
        pSepx->AppendSep( Fc2Cp( nFcPos ), pNewPgDesc, rNd, pFmt, nLnNm );
    }
}

void MSWordExportBase::CorrectTabStopInSet( SfxItemSet& rSet, USHORT nAbsLeft )
{
    const SvxTabStopItem *pItem =
        sw::util::HasItem<SvxTabStopItem>( rSet, RES_PARATR_TABSTOP );

    if ( pItem )
    {
        // dann muss das fuer die Ausgabe korrigiert werden
        SvxTabStopItem aTStop(*pItem);
        for ( USHORT nCnt = 0; nCnt < aTStop.Count(); ++nCnt )
        {
            SvxTabStop& rTab = (SvxTabStop&)aTStop[ nCnt ];
            if ( SVX_TAB_ADJUST_DEFAULT != rTab.GetAdjustment() &&
                rTab.GetTabPos() >= nAbsLeft )
            {
                rTab.GetTabPos() -= nAbsLeft;
            }
            else
            {
                aTStop.Remove( nCnt );
                --nCnt;
            }
        }
        rSet.Put( aTStop );
    }
}

BYTE WW8Export::GetNumId( USHORT eNumType )
{
    BYTE nRet = 0;
    switch( eNumType )
    {
    case SVX_NUM_CHARS_UPPER_LETTER:
    case SVX_NUM_CHARS_UPPER_LETTER_N:  nRet = 3;       break;
    case SVX_NUM_CHARS_LOWER_LETTER:
    case SVX_NUM_CHARS_LOWER_LETTER_N:  nRet = 4;       break;
    case SVX_NUM_ROMAN_UPPER:           nRet = 1;       break;
    case SVX_NUM_ROMAN_LOWER:           nRet = 2;       break;

    case SVX_NUM_BITMAP:
    case SVX_NUM_CHAR_SPECIAL:          nRet = 23;      break;

    // nix, macht WW undokumentiert auch so
    case SVX_NUM_NUMBER_NONE:           nRet = 0xff;    break;
    }
    return nRet;
}

void WW8AttributeOutput::OutlineNumbering( BYTE nLvl, const SwNumFmt &rNFmt, const SwFmt &rFmt )
{
    if ( nLvl >= WW8ListManager::nMaxLevel )
        nLvl = WW8ListManager::nMaxLevel-1;

    if ( m_rWW8Export.bWrtWW8 )
    {
        // write sprmPOutLvl sprmPIlvl and sprmPIlfo
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_POutLvl );
        m_rWW8Export.pO->Insert( nLvl, m_rWW8Export.pO->Count() );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_PIlvl );
        m_rWW8Export.pO->Insert( nLvl, m_rWW8Export.pO->Count() );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_PIlfo );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO,
                1 + m_rWW8Export.GetId( *m_rWW8Export.pDoc->GetOutlineNumRule() ) );
    }
    else
    {
        m_rWW8Export.Out_SwNumLvl( nLvl );
        // --> OD 2008-06-03 #i86652#
//        if (rNFmt.GetAbsLSpace())
        if ( rNFmt.GetPositionAndSpaceMode() ==
                                   SvxNumberFormat::LABEL_WIDTH_AND_POSITION  &&
             rNFmt.GetAbsLSpace() )
        // <--
        {
            SwNumFmt aNumFmt( rNFmt );
            const SvxLRSpaceItem& rLR =
                ItemGet<SvxLRSpaceItem>( rFmt, RES_LR_SPACE );

            aNumFmt.SetAbsLSpace( writer_cast<short>(
                    aNumFmt.GetAbsLSpace() + rLR.GetLeft() ) );
            m_rWW8Export.Out_NumRuleAnld(
                    *m_rWW8Export.pDoc->GetOutlineNumRule(),
                    aNumFmt, nLvl );
        }
        else
            m_rWW8Export.Out_NumRuleAnld(
                    *m_rWW8Export.pDoc->GetOutlineNumRule(),
                    rNFmt, nLvl );
    }
}

// --> OD 2007-06-04 #i77805#
bool WW8Export::DisallowInheritingOutlineNumbering(const SwFmt &rFmt)
{
    bool bRet( false );

    //If there is no numbering on this fmt, but its parent was outline
    //numbered, then in writer this is no inheritied, but in word it would
    //be, so we must export "no numbering" and "body level" to make word
    //behave like writer (see #i25755)
    if (SFX_ITEM_SET != rFmt.GetItemState(RES_PARATR_NUMRULE, false))
    {
        if (const SwFmt *pParent = rFmt.DerivedFrom())
        {
            if (((const SwTxtFmtColl*)pParent)->IsAssignedToListLevelOfOutlineStyle())
            {
                if (bWrtWW8)
                {
                    SwWW8Writer::InsUInt16(*pO, NS_sprm::LN_POutLvl);
                    pO->Insert(BYTE(9), pO->Count());
                    SwWW8Writer::InsUInt16(*pO, NS_sprm::LN_PIlfo);
                    SwWW8Writer::InsUInt16(*pO, 0);

                    bRet = true;
                }
                /*whats the winword 6 way to do this ?*/
            }
        }
    }

    return bRet;
}
// <--

void MSWordExportBase::OutputFormat( const SwFmt& rFmt, bool bPapFmt, bool bChpFmt, bool bFlyFmt )
{
    bool bCallOutSet = true;
    const SwModify* pOldMod = pOutFmtNode;
    pOutFmtNode = &rFmt;

    switch( rFmt.Which() )
    {
    case RES_CONDTXTFMTCOLL:
    case RES_TXTFMTCOLL:
        if( bPapFmt )
        {
            if (((const SwTxtFmtColl&)rFmt).IsAssignedToListLevelOfOutlineStyle())
            {
                int nLvl = ((const SwTxtFmtColl&)rFmt).GetAssignedOutlineStyleLevel();

                //if outline numbered
                // if Write StyleDefinition then write the OutlineRule
                const SwNumFmt& rNFmt = pDoc->GetOutlineNumRule()->Get( static_cast<USHORT>( nLvl ) );
                if ( bStyDef )
                    AttrOutput().OutlineNumbering( static_cast< BYTE >( nLvl ), rNFmt, rFmt );

                // --> OD 2008-06-03 #i86652#
//                if (rNFmt.GetAbsLSpace())
                if ( rNFmt.GetPositionAndSpaceMode() ==
                                           SvxNumberFormat::LABEL_WIDTH_AND_POSITION  &&
                     rNFmt.GetAbsLSpace() )
                // <--
                {
                    SfxItemSet aSet( rFmt.GetAttrSet() );
                    SvxLRSpaceItem aLR(
                        ItemGet<SvxLRSpaceItem>(aSet, RES_LR_SPACE));

                    aLR.SetTxtLeft( aLR.GetTxtLeft() + rNFmt.GetAbsLSpace() );
                    aLR.SetTxtFirstLineOfst( GetWordFirstLineOffset(rNFmt));

                    aSet.Put( aLR );
                    CorrectTabStopInSet( aSet, rNFmt.GetAbsLSpace() );
                    OutputItemSet( aSet, bPapFmt, bChpFmt,
                        i18n::ScriptType::LATIN);
                    bCallOutSet = false;
                }
            }
            else
            {
                //otherwise we might have to remove outline numbering from
                //what gets exported if the parent style was outline numbered
                // --> OD 2007-06-04 #i77805#
                // If inherited outline numbering is suppress, the left/right
                // margins has to be exported explicitly.
                if ( bStyDef && DisallowInheritingOutlineNumbering(rFmt) )
                {
                    SfxItemSet aSet( rFmt.GetAttrSet() );
                    SvxLRSpaceItem aLR(
                        ItemGet<SvxLRSpaceItem>(aSet, RES_LR_SPACE));
                    aSet.Put( aLR );
                    OutputItemSet( aSet, bPapFmt, bChpFmt,
                        com::sun::star::i18n::ScriptType::LATIN);
                    bCallOutSet = false;
                }
                // <--
            }
        }
        break;

    case RES_CHRFMT:
        break;
    case RES_FLYFRMFMT:
        if (bFlyFmt)
        {
            ASSERT(mpParentFrame, "No parent frame, all broken");

            if (mpParentFrame)
            {
                const SwFrmFmt &rFrmFmt = mpParentFrame->GetFrmFmt();

                SfxItemSet aSet(pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                    RES_FRMATR_END-1);
                aSet.Set(rFrmFmt.GetAttrSet());

                // Fly als Zeichen werden bei uns zu Absatz-gebundenen
                // jetzt den Abstand vom Absatz-Rand setzen
                if (pFlyOffset)
                {
                    aSet.Put(SwFmtHoriOrient(pFlyOffset->X()));
                    aSet.Put(SwFmtVertOrient(pFlyOffset->Y()));
                    SwFmtAnchor aAnchor(rFrmFmt.GetAnchor());
                    aAnchor.SetType(eNewAnchorType);
                    aSet.Put(aAnchor);
                }

                if (SFX_ITEM_SET != aSet.GetItemState(RES_SURROUND))
                    aSet.Put(SwFmtSurround(SURROUND_NONE));

                bOutFlyFrmAttrs = true;
                //script doesn't matter if not exporting chp
                OutputItemSet(aSet, true, false,
                    i18n::ScriptType::LATIN);
                bOutFlyFrmAttrs = false;

                bCallOutSet = false;
            }
        }
        break;
    default:
        ASSERT( !this, "Was wird hier ausgegeben ??? " );
        break;
    }

    if( bCallOutSet )
        OutputItemSet( rFmt.GetAttrSet(), bPapFmt, bChpFmt,
            i18n::ScriptType::LATIN);
    pOutFmtNode = pOldMod;
}

bool MSWordExportBase::HasRefToObject( USHORT nTyp, const String* pName, USHORT nSeqNo )
{
    const SwTxtNode* pNd;
    SwClientIter aIter( *pDoc->GetSysFldType( RES_GETREFFLD ) );
    for ( SwFmtFld* pFld = static_cast< SwFmtFld* >( aIter.First( TYPE( SwFmtFld ) ) );
            pFld;
            pFld = static_cast< SwFmtFld* >( aIter.Next() ) )
    {
        if ( pFld->GetTxtFld() && nTyp == pFld->GetFld()->GetSubType() &&
             0 != ( pNd = pFld->GetTxtFld()->GetpTxtNode() ) &&
             pNd->GetNodes().IsDocNodes() )
        {
            const SwGetRefField& rRFld = *static_cast< SwGetRefField* >( pFld->GetFld() );
            switch ( nTyp )
            {
                case REF_BOOKMARK:
                case REF_SETREFATTR:
                    if ( pName && *pName == rRFld.GetSetRefName() )
                        return true;
                    break;
                case REF_FOOTNOTE:
                case REF_ENDNOTE:
                    if ( nSeqNo == rRFld.GetSeqNo() )
                        return true;
                    break;
                case REF_SEQUENCEFLD:
                    break;      // ???
                case REF_OUTLINE:
                    break;      // ???
            }
        }
    }

    return false;
}

String MSWordExportBase::GetBookmarkName( USHORT nTyp, const String* pName, USHORT nSeqNo )
{
    String sRet;
    switch ( nTyp )
    {
        case REF_SETREFATTR:
            if ( pName )
            {
                sRet.APPEND_CONST_ASC( "Ref_" );
                sRet += *pName;
            }
            break;
        case REF_SEQUENCEFLD:
            break;      // ???
        case REF_BOOKMARK:
            if ( pName )
                sRet = *pName;
            break;
        case REF_OUTLINE:
            break;      // ???
        case REF_FOOTNOTE:
            sRet.APPEND_CONST_ASC( "_RefF" );
            sRet += String::CreateFromInt32( nSeqNo );
            break;
        case REF_ENDNOTE:
            sRet.APPEND_CONST_ASC( "_RefE" );
            sRet += String::CreateFromInt32( nSeqNo );
            break;
    }
    // --> OD 2005-06-08 #i43956# - encode bookmark accordingly
    return BookmarkToWord( sRet );
    // <--
}

//-----------------------------------------------------------------------
/*  */
/* File CHRATR.HXX: */
void WW8AttributeOutput::RTLAndCJKState( bool bIsRTL, sal_uInt16 nScript )
{
    if ( m_rWW8Export.bWrtWW8 && bIsRTL )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CFBiDi );
        m_rWW8Export.pO->Insert( (BYTE)1, m_rWW8Export.pO->Count() );
    }

    // #i46087# patch from james_clark; complex texts needs the undocumented SPRM CComplexScript with param 0x81.
    if ( m_rWW8Export.bWrtWW8 && nScript == i18n::ScriptType::COMPLEX && !bIsRTL )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CComplexScript );
        m_rWW8Export.pO->Insert( (BYTE)0x81, m_rWW8Export.pO->Count() );
        m_rWW8Export.pDop->bUseThaiLineBreakingRules = true;
    }
}

void WW8AttributeOutput::EndParagraph( ww8::WW8TableNodeInfoInner::Pointer_t pTextNodeInfoInner )
{
    m_rWW8Export.pPapPlc->AppendFkpEntry( m_rWW8Export.Strm().Tell(), m_rWW8Export.pO->Count(), m_rWW8Export.pO->GetData() );
    m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() ); // delete

    if ( pTextNodeInfoInner.get() != NULL )
    {         
        if ( pTextNodeInfoInner->isEndOfLine() )
        {
            TableRowEnd( pTextNodeInfoInner->getDepth() );
            
            SVBT16 nSty;
            ShortToSVBT16( m_rWW8Export.nStyleBeforeFly, nSty );
            m_rWW8Export.pO->Insert( (BYTE*)&nSty, 2, m_rWW8Export.pO->Count() );     // Style #
            TableInfoRow( pTextNodeInfoInner );
            m_rWW8Export.pPapPlc->AppendFkpEntry( m_rWW8Export.Strm().Tell(), m_rWW8Export.pO->Count(),
                    m_rWW8Export.pO->GetData() );
            m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() ); // delete
        }
    }
}

void WW8AttributeOutput::StartRunProperties()
{
    WW8_WrPlcFld* pCurrentFields = m_rWW8Export.CurrentFieldPlc();
    m_nFieldResults = pCurrentFields ? pCurrentFields->ResultCount() : 0;
}

void WW8AttributeOutput::EndRunProperties( const SwRedlineData* pRedlineData )
{
    Redline( pRedlineData );

    WW8_WrPlcFld* pCurrentFields = m_rWW8Export.CurrentFieldPlc();
    USHORT nNewFieldResults = pCurrentFields ? pCurrentFields->ResultCount() : 0;

    bool bExportedFieldResult = ( m_nFieldResults != nNewFieldResults );

    // If we have exported a field result, then we will have been forced to
    // split up the text into a 0x13, 0x14, <result> 0x15 sequence with the
    // properties forced out at the end of the result, so the 0x15 itself
    // should remain clean of all other attributes to avoid #iXXXXX#
    if ( !bExportedFieldResult )
    {
        m_rWW8Export.pChpPlc->AppendFkpEntry( m_rWW8Export.Strm().Tell(),
                m_rWW8Export.pO->Count(), m_rWW8Export.pO->GetData() );
    }
    m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() ); // delete
}

void WW8AttributeOutput::RunText( const String& rText, rtl_TextEncoding eCharSet )
{
    RawText( rText, m_rWW8Export.bWrtWW8, eCharSet );
}

void WW8AttributeOutput::RawText( const String& rText, bool bForceUnicode, rtl_TextEncoding eCharSet )
{
    m_rWW8Export.OutSwString( rText, 0, rText.Len(), bForceUnicode, eCharSet );
}

void WW8AttributeOutput::OutputFKP()
{
    if ( m_rWW8Export.pO->Count() )
    {
        m_rWW8Export.pChpPlc->AppendFkpEntry( m_rWW8Export.Strm().Tell(),
                m_rWW8Export.pO->Count(), m_rWW8Export.pO->GetData() );
        m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() ); // delete
    }
}

void WW8AttributeOutput::ParagraphStyle( USHORT nStyle )
{
    ASSERT( !m_rWW8Export.pO->Count(), " pO ist am ZeilenEnde nicht leer" );

    SVBT16 nSty;
    ShortToSVBT16( nStyle, nSty );
    m_rWW8Export.pO->Insert( (BYTE*)&nSty, 2, m_rWW8Export.pO->Count() );     // Style #
}

void WW8AttributeOutput::OutputWW8Attribute( BYTE nId, bool bVal )
{
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( 8 == nId ? NS_sprm::LN_CFDStrike : NS_sprm::LN_CFBold + nId );
    else if (8 == nId )
        return; // no such attribute in WW6
    else
        m_rWW8Export.pO->Insert( 85 + nId, m_rWW8Export.pO->Count() );

    m_rWW8Export.pO->Insert( bVal ? 1 : 0, m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::OutputWW8AttributeCTL( BYTE nId, bool bVal )
{
    ASSERT( nId <= 1, "out of range" );
    if ( !m_rWW8Export.bWrtWW8 || nId > 1 )
        return;

    m_rWW8Export.InsUInt16( NS_sprm::LN_CFBoldBi + nId );
    m_rWW8Export.pO->Insert( bVal ? 1 : 0, m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::CharFont( const SvxFontItem& rFont )
{
    USHORT nFontID = m_rWW8Export.GetId( rFont );

    if ( m_rWW8Export.bWrtWW8 )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CRgFtc0 );
        m_rWW8Export.InsUInt16( nFontID );
        m_rWW8Export.InsUInt16( NS_sprm::LN_CRgFtc2 );
    }
    else
        m_rWW8Export.pO->Insert( 93, m_rWW8Export.pO->Count() );

    m_rWW8Export.InsUInt16( nFontID );
}

void WW8AttributeOutput::CharFontCTL( const SvxFontItem& rFont )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CFtcBi );
        m_rWW8Export.InsUInt16( m_rWW8Export.GetId( rFont ) );
    }
}

void WW8AttributeOutput::CharFontCJK( const SvxFontItem& rFont )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CRgFtc1 );
        m_rWW8Export.InsUInt16( m_rWW8Export.GetId( rFont ) );
    }
}

void WW8AttributeOutput::CharWeightCTL( const SvxWeightItem& rWeight )
{
    //Can only export in 8+, in 7- export as normal varient and expect that
    //upperlevel code has blocked exporting clobbering attributes
    if (m_rWW8Export.bWrtWW8)
    {
        OutputWW8AttributeCTL( 0, WEIGHT_BOLD == rWeight.GetWeight());
    }
    else
    {
        OutputWW8Attribute( 0, WEIGHT_BOLD == rWeight.GetWeight());
    }
}

void WW8AttributeOutput::CharPostureCTL( const SvxPostureItem& rPosture )
{
    // Can only export in 8+, in 7- export as normal varient and expect that
    // upperlevel code has blocked exporting clobbering attributes
    if (m_rWW8Export.bWrtWW8)
    {
        OutputWW8AttributeCTL( 1, ITALIC_NONE != rPosture.GetPosture() );
    }
    else
    {
        OutputWW8Attribute( 1, ITALIC_NONE != rPosture.GetPosture() );
    }
}

void WW8AttributeOutput::CharPosture( const SvxPostureItem& rPosture )
{
    OutputWW8Attribute( 1, ITALIC_NONE != rPosture.GetPosture() );
}

void WW8AttributeOutput::CharWeight( const SvxWeightItem& rWeight )
{
    OutputWW8Attribute( 0, WEIGHT_BOLD == rWeight.GetWeight() );
}

// Shadowed und Contour are not in WW-UI. JP: ??
void WW8AttributeOutput::CharContour( const SvxContourItem& rContour )
{
    OutputWW8Attribute( 3, rContour.GetValue() ? true : false);
}

void WW8AttributeOutput::CharShadow( const SvxShadowedItem& rShadow )
{
    OutputWW8Attribute( 4, rShadow.GetValue() ? true : false);
}

void WW8AttributeOutput::CharKerning( const SvxKerningItem& rKerning )
{
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_CDxaSpace );
    else
        m_rWW8Export.pO->Insert( 96, m_rWW8Export.pO->Count() );

    m_rWW8Export.InsUInt16( rKerning.GetValue() );
}

void WW8AttributeOutput::CharAutoKern( const SvxAutoKernItem& rAutoKern )
{
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_CHpsKern );
    else
        m_rWW8Export.pO->Insert( 107, m_rWW8Export.pO->Count() );
    
    m_rWW8Export.InsUInt16( rAutoKern.GetValue() ? 1 : 0 );
}

void WW8AttributeOutput::CharAnimatedText( const SvxBlinkItem& rBlink )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CSfxText );
        // At the moment the only animated text effect we support is blinking
        m_rWW8Export.InsUInt16( rBlink.GetValue() ? 2 : 0 );
    }
}

void WW8AttributeOutput::CharCrossedOut( const SvxCrossedOutItem& rCrossed )
{
    FontStrikeout eSt = rCrossed.GetStrikeout();
    if ( STRIKEOUT_DOUBLE == eSt )
    {
        OutputWW8Attribute( 8, true );
        return;
    }
    if ( STRIKEOUT_NONE != eSt )
    {
        OutputWW8Attribute( 2, true );
        return;
    }

    // otherwise both off
    OutputWW8Attribute( 8, false );
    OutputWW8Attribute( 2, false );
}

void WW8AttributeOutput::CharCaseMap( const SvxCaseMapItem& rCaseMap )
{
    USHORT eSt = rCaseMap.GetValue();
    switch ( eSt )
    {
        case SVX_CASEMAP_KAPITAELCHEN:
            OutputWW8Attribute( 5, true );
            return;
        case SVX_CASEMAP_VERSALIEN:
            OutputWW8Attribute( 6, true );
            return;
        case SVX_CASEMAP_TITEL:
            // no such feature in word
            break;
        default:
            // otherwise both off
            OutputWW8Attribute( 5, false );
            OutputWW8Attribute( 6, false );
            return;
    }
}

void WW8AttributeOutput::CharHidden( const SvxCharHiddenItem& rHidden )
{
    OutputWW8Attribute( 7, rHidden.GetValue() );
}

void WW8AttributeOutput::CharUnderline( const SvxUnderlineItem& rUnderline )
{
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_CKul );
    else
        m_rWW8Export.pO->Insert( 94, m_rWW8Export.pO->Count() );

    const SfxPoolItem* pItem = m_rWW8Export.HasItem( RES_CHRATR_WORDLINEMODE );
    bool bWord = false;
    if (pItem)
        bWord = ((const SvxWordLineModeItem*)pItem)->GetValue() ? true : false;

    // WW95 - parameters:   0 = none,   1 = single, 2 = by Word,
    //                      3 = double, 4 = dotted, 5 = hidden
    // WW97 - additional parameters:
    //                      6 = thick,   7 = dash,       8 = dot(not used)
    //                      9 = dotdash 10 = dotdotdash, 11 = wave
    BYTE b = 0;
    switch ( rUnderline.GetLineStyle() )
    {
        case UNDERLINE_SINGLE:
            b = ( bWord ) ? 2 : 1;
            break;
        case UNDERLINE_BOLD:
            b = m_rWW8Export.bWrtWW8 ?  6 : 1;
            break;
        case UNDERLINE_DOUBLE:
            b = 3;
            break;
        case UNDERLINE_DOTTED:
            b = 4;
            break;
        case UNDERLINE_DASH:
            b = m_rWW8Export.bWrtWW8 ?  7 : 4;
            break;
        case UNDERLINE_DASHDOT:
            b = m_rWW8Export.bWrtWW8 ?  9 : 4;
            break;
        case UNDERLINE_DASHDOTDOT:
            b = m_rWW8Export.bWrtWW8 ? 10 : 4;
            break;
        case UNDERLINE_WAVE:
            b = m_rWW8Export.bWrtWW8 ? 11 : 3;
            break;
        // ------------  new in WW2000  -------------------------------------
        case UNDERLINE_BOLDDOTTED:
            b = m_rWW8Export.bWrtWW8 ? 20 : 4;
            break;
        case UNDERLINE_BOLDDASH:
            b = m_rWW8Export.bWrtWW8 ? 23 : 4;
            break;
        case UNDERLINE_LONGDASH:
            b = m_rWW8Export.bWrtWW8 ? 39 : 4;
            break;
        case UNDERLINE_BOLDLONGDASH:
            b = m_rWW8Export.bWrtWW8 ? 55 : 4;
            break;
        case UNDERLINE_BOLDDASHDOT:
            b = m_rWW8Export.bWrtWW8 ? 25 : 4;
            break;
        case UNDERLINE_BOLDDASHDOTDOT:
            b = m_rWW8Export.bWrtWW8 ? 26 : 4;
            break;
        case UNDERLINE_BOLDWAVE:
            b = m_rWW8Export.bWrtWW8 ? 27 : 3;
            break;
        case UNDERLINE_DOUBLEWAVE:
            b = m_rWW8Export.bWrtWW8 ? 43 : 3;
            break;
        case UNDERLINE_NONE:
            b = 0;
            break;
        default:
            ASSERT( rUnderline.GetLineStyle() == UNDERLINE_NONE, "Unhandled underline type" );
            break;
    }

    m_rWW8Export.pO->Insert( b, m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::CharLanguage( const SvxLanguageItem& rLanguage )
{
    USHORT nId = 0;
    if ( m_rWW8Export.bWrtWW8 )
    {
        switch ( rLanguage.Which() )
        {
            case RES_CHRATR_LANGUAGE:
                nId = NS_sprm::LN_CRgLid0;
                break;
            case RES_CHRATR_CJK_LANGUAGE:
                nId = NS_sprm::LN_CRgLid1;
                break;
            case RES_CHRATR_CTL_LANGUAGE:
                nId = NS_sprm::LN_CLidBi;
                break;
        }
    }
    else
        nId = 97;

    if ( nId )
    {
        if ( m_rWW8Export.bWrtWW8 ) // use sprmCRgLid0 rather than sprmCLid
            m_rWW8Export.InsUInt16( nId );
        else
            m_rWW8Export.pO->Insert( (BYTE)nId, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( rLanguage.GetLanguage() );

        // unknown as to exactly why, but this seems to shadow the other
        // paramater in word 2000 and without it spellchecking doesn't work
        if ( nId == NS_sprm::LN_CRgLid0 )
        {
            m_rWW8Export.InsUInt16( 0x4873 );
            m_rWW8Export.InsUInt16( rLanguage.GetLanguage() );
        }
        else if ( nId == NS_sprm::LN_CLidBi )
        {
            m_rWW8Export.InsUInt16( 0x4874 );
            m_rWW8Export.InsUInt16( rLanguage.GetLanguage() );
        }

    }
}

void WW8AttributeOutput::CharEscapement( const SvxEscapementItem& rEscapement )
{
    BYTE b = 0xFF;
    short nEsc = rEscapement.GetEsc(), nProp = rEscapement.GetProp();
    if ( !nEsc )
    {
        b = 0;
        nEsc = 0;
        nProp = 100;
    }
    else if ( DFLT_ESC_PROP == nProp )
    {
        if ( DFLT_ESC_SUB == nEsc || DFLT_ESC_AUTO_SUB == nEsc )
            b = 2;
        else if ( DFLT_ESC_SUPER == nEsc || DFLT_ESC_AUTO_SUPER == nEsc )
            b = 1;
    }

    if ( 0xFF != b )
    {
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_CIss );
        else
            m_rWW8Export.pO->Insert( 104, m_rWW8Export.pO->Count() );

        m_rWW8Export.pO->Insert( b, m_rWW8Export.pO->Count() );
    }

    if ( 0 == b || 0xFF == b )
    {
        long nHeight = ((SvxFontHeightItem&)m_rWW8Export.GetItem(
                                    RES_CHRATR_FONTSIZE )).GetHeight();
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_CHpsPos );
        else
            m_rWW8Export.pO->Insert( 101, m_rWW8Export.pO->Count() );

        m_rWW8Export.InsUInt16( (short)(( nHeight * nEsc + 500 ) / 1000 ));

        if( 100 != nProp || !b )
        {
            if( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_CHps );
            else
                m_rWW8Export.pO->Insert( 99, m_rWW8Export.pO->Count() );

            m_rWW8Export.InsUInt16(
                msword_cast<sal_uInt16>((nHeight * nProp + 500 ) / 1000));
        }
    }
}

void WW8AttributeOutput::CharFontSize( const SvxFontHeightItem& rHeight )
{
    USHORT nId = 0;
    if ( m_rWW8Export.bWrtWW8 )
    {
        switch ( rHeight.Which() )
        {
            case RES_CHRATR_FONTSIZE:
            case RES_CHRATR_CJK_FONTSIZE:
                nId = NS_sprm::LN_CHps;
                break;
            case RES_CHRATR_CTL_FONTSIZE:
                nId = NS_sprm::LN_CHpsBi;
                break;
        }
    }
    else
        nId = 99;

    if ( nId )
    {
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( nId );
        else
            m_rWW8Export.pO->Insert( (BYTE)nId, m_rWW8Export.pO->Count() );

        m_rWW8Export.InsUInt16( (UINT16)(( rHeight.GetHeight() + 5 ) / 10 ) );
    }
}

void WW8AttributeOutput::CharScaleWidth( const SvxCharScaleWidthItem& rScaleWidth )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_CCharScale );
        m_rWW8Export.InsUInt16( rScaleWidth.GetValue() );
    }
}

void WW8AttributeOutput::CharRelief( const SvxCharReliefItem& rRelief )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        USHORT nId;
        switch ( rRelief.GetValue() )
        {
            case RELIEF_EMBOSSED:   nId = NS_sprm::LN_CFEmboss;     break;
            case RELIEF_ENGRAVED:   nId = NS_sprm::LN_CFImprint;    break;
            default:                nId = 0;                        break;
        }

        if( nId )
        {
            m_rWW8Export.InsUInt16( nId );
            m_rWW8Export.pO->Insert( (BYTE)0x81, m_rWW8Export.pO->Count() );
        }
        else
        {
            // switch both flags off
            m_rWW8Export.InsUInt16( NS_sprm::LN_CFEmboss );
            m_rWW8Export.pO->Insert( (BYTE)0x0, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( NS_sprm::LN_CFImprint );
            m_rWW8Export.pO->Insert( (BYTE)0x0, m_rWW8Export.pO->Count() );
        }
    }
}

void WW8AttributeOutput::CharRotate( const SvxCharRotateItem& rRotate )
{
    // #i28331# - check that a Value is set
    if ( !rRotate.GetValue() )
        return;

    if ( m_rWW8Export.bWrtWW8 && !m_rWW8Export.bIsInTable )
    {
        // #i36867 In word the text in a table is rotated via the TC or NS_sprm::LN_TTextFlow
        // This means you can only rotate all or none of the text adding NS_sprm::LN_CEastAsianLayout
        // here corrupts the table, hence !m_rWW8Export.bIsInTable

        m_rWW8Export.InsUInt16( NS_sprm::LN_CEastAsianLayout );
        m_rWW8Export.pO->Insert( (BYTE)0x06, m_rWW8Export.pO->Count() ); //len 6
        m_rWW8Export.pO->Insert( (BYTE)0x01, m_rWW8Export.pO->Count() );

        m_rWW8Export.InsUInt16( rRotate.IsFitToLine() ? 1 : 0 );
        static const BYTE aZeroArr[ 3 ] = { 0, 0, 0 };
        m_rWW8Export.pO->Insert( aZeroArr, 3, m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::CharEmphasisMark( const SvxEmphasisMarkItem& rEmphasisMark )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        BYTE nVal;
        switch ( rEmphasisMark.GetValue() )
        {
            case EMPHASISMARK_NONE:             nVal = 0;   break;
            case EMPHASISMARK_SIDE_DOTS:        nVal = 2;   break;
            case EMPHASISMARK_CIRCLE_ABOVE:     nVal = 3;   break;
            case EMPHASISMARK_DOTS_BELOW:       nVal = 4;   break;
            // case 1:
            default:                            nVal = 1;   break;
        }

        m_rWW8Export.InsUInt16( NS_sprm::LN_CKcd );
        m_rWW8Export.pO->Insert( nVal, m_rWW8Export.pO->Count() );
    }
}

// TransCol uebersetzt SW-Farben in WW. Heraus kommt die bei WW fuer
// Text- und Hintergrundfarbe benutzte Codierung.
// Gibt es keine direkte Entsprechung, dann wird versucht, eine moeglichst
// aehnliche WW-Farbe zu finden.
// return: 5-Bit-Wert ( 0..16 )
BYTE WW8Export::TransCol( const Color& rCol )
{
    BYTE nCol = 0;      // ->Auto
    switch( rCol.GetColor() )
    {
    case COL_BLACK:         nCol = 1;   break;
    case COL_BLUE:          nCol = 9;   break;
    case COL_GREEN:         nCol = 11;  break;
    case COL_CYAN:          nCol = 10;  break;
    case COL_RED:           nCol = 13;  break;
    case COL_MAGENTA:       nCol = 12;  break;
    case COL_BROWN:         nCol = 14;  break;
    case COL_GRAY:          nCol = 15;  break;
    case COL_LIGHTGRAY:     nCol = 16;  break;
    case COL_LIGHTBLUE:     nCol = 2;   break;
    case COL_LIGHTGREEN:    nCol = 4;   break;
    case COL_LIGHTCYAN:     nCol = 3;   break;
    case COL_LIGHTRED:      nCol = 6;   break;
    case COL_LIGHTMAGENTA:  nCol = 5;   break;
    case COL_YELLOW:        nCol = 7;   break;
    case COL_WHITE:         nCol = 8;   break;
    case COL_AUTO:          nCol = 0;   break;

    default:
        if( !pBmpPal )
        {
            pBmpPal = new BitmapPalette( 16 );
            static const ColorData aColArr[ 16 ] = {
                COL_BLACK,      COL_LIGHTBLUE,  COL_LIGHTCYAN,  COL_LIGHTGREEN,
                COL_LIGHTMAGENTA,COL_LIGHTRED,  COL_YELLOW,     COL_WHITE,
                COL_BLUE,       COL_CYAN,       COL_GREEN,      COL_MAGENTA,
                COL_RED,        COL_BROWN,      COL_GRAY,       COL_LIGHTGRAY
            };

            for( USHORT i = 0; i < 16; ++i )
                pBmpPal->operator[]( i ) = Color( aColArr[ i ] );
        }
        nCol = static_cast< BYTE >(pBmpPal->GetBestIndex( rCol ) + 1);
        break;
    }
    return nCol;
}

// TransBrush uebersetzt SW-Brushes in WW. Heraus kommt WW8_SHD.
// Nicht-Standardfarben des SW werden noch nicht in die
// Misch-Werte ( 0 .. 95% ) vom WW uebersetzt.
// Return: Echte Brush ( nicht transparent )
// auch bei Transparent wird z.B. fuer Tabellen eine transparente Brush
// geliefert
bool WW8Export::TransBrush(const Color& rCol, WW8_SHD& rShd)
{
    if( rCol.GetTransparency() )
        rShd = WW8_SHD();               // alles Nullen : transparent
    else
    {
        rShd.SetFore( 8);
        rShd.SetBack( TransCol( rCol ) );
        rShd.SetStyle( bWrtWW8, 0 );
    }
    return !rCol.GetTransparency();
}

sal_uInt32 SuitableBGColor(sal_uInt32 nIn)
{
    if (nIn == COL_AUTO)
        return 0xFF000000;
    return wwUtility::RGBToBGR(nIn);
}

void WW8AttributeOutput::CharColor( const SvxColorItem& rColor )
{
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_CIco );
    else
        m_rWW8Export.pO->Insert( 98, m_rWW8Export.pO->Count() );

    BYTE nColor = m_rWW8Export.TransCol( rColor.GetValue() );
    m_rWW8Export.pO->Insert( nColor, m_rWW8Export.pO->Count() );

    if ( m_rWW8Export.bWrtWW8 && nColor )
    {
        m_rWW8Export.InsUInt16( 0x6870 );
        m_rWW8Export.InsUInt32( wwUtility::RGBToBGR( rColor.GetValue().GetColor() ) );
    }
}

void WW8AttributeOutput::CharBackground( const SvxBrushItem& rBrush )
{
    if( m_rWW8Export.bWrtWW8 )   // nur WW8 kann ZeichenHintergrund
    {
        WW8_SHD aSHD;

        m_rWW8Export.TransBrush( rBrush.GetColor(), aSHD );
        // sprmCShd
        m_rWW8Export.InsUInt16( NS_sprm::LN_CShd );
        m_rWW8Export.InsUInt16( aSHD.GetValue() );

        //Quite a few unknowns, some might be transparency or something
        //of that nature...
        m_rWW8Export.InsUInt16( 0xCA71 );
        m_rWW8Export.pO->Insert( 10, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt32( 0xFF000000 );
        m_rWW8Export.InsUInt32( SuitableBGColor( rBrush.GetColor().GetColor() ) );
        m_rWW8Export.InsUInt16( 0x0000);
    }
}

void WW8AttributeOutput::TextINetFormat( const SwFmtINetFmt& rINet )
{
    if ( rINet.GetValue().Len() )
    {
        USHORT nId;
        const String& rStr = rINet.GetINetFmt();
        if ( rStr.Len() )
            nId = rINet.GetINetFmtId();
        else
            nId = RES_POOLCHR_INET_NORMAL;

        const SwCharFmt* pFmt = IsPoolUserFmt( nId )
                        ? m_rWW8Export.pDoc->FindCharFmtByName( rStr )
                        : m_rWW8Export.pDoc->GetCharFmtFromPool( nId );

        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_CIstd );
        else
            m_rWW8Export.pO->Insert( 80, m_rWW8Export.pO->Count() );

        m_rWW8Export.InsUInt16( m_rWW8Export.GetId( *pFmt ) );
    }
}

// --> OD 2005-06-08 #i43956# - add optional parameter <pLinkStr>
// It's needed to write the hyperlink data for a certain cross-reference
// - it contains the name of the link target, which is a bookmark.
// --> OD 2008-08-14 #158418# - add optional parameter <bIncludeEmptyPicLocation>
// It is needed to write an empty picture location for page number field separators
static void InsertSpecialChar( WW8Export& rWrt, BYTE c,
                               String* pLinkStr = 0L,
                               bool bIncludeEmptyPicLocation = false )
{
    WW8Bytes aItems;
    rWrt.GetCurrentItems(aItems);

    if (c == 0x13)
        rWrt.pChpPlc->AppendFkpEntry(rWrt.Strm().Tell());
    else
        rWrt.pChpPlc->AppendFkpEntry(rWrt.Strm().Tell(), aItems.Count(),
            aItems.GetData());

    rWrt.WriteChar(c);

    // --> OD 2008-08-14 #158418#
    // store empty sprmCPicLocation for field separator
    if ( bIncludeEmptyPicLocation &&
         ( c == 0x13 || c == 0x14 || c == 0x15 ) )
    {
        SwWW8Writer::InsUInt16( aItems, NS_sprm::LN_CPicLocation );
        SwWW8Writer::InsUInt32( aItems, 0x00000000 );
    }
    // <--

    // --> OD 2005-06-08 #i43956# - write hyperlink data and attributes
    if ( rWrt.bWrtWW8 && c == 0x01 && pLinkStr )
    {
        // write hyperlink data to data stream
        SvStream& rStrm = *rWrt.pDataStrm;
        // position of hyperlink data
        const UINT32 nLinkPosInDataStrm = rStrm.Tell();
        // write empty header
        const UINT16 nEmptyHdrLen = 0x44;
        BYTE aEmptyHeader[ nEmptyHdrLen ] = { 0 };
        aEmptyHeader[ 4 ] = 0x44;
        rStrm.Write( aEmptyHeader, nEmptyHdrLen );
        // writer fixed header
        const UINT16 nFixHdrLen = 0x19;
        BYTE aFixHeader[ nFixHdrLen ] =
        {
            0x08, 0xD0, 0xC9, 0xEA, 0x79, 0xF9, 0xBA, 0xCE,
            0x11, 0x8C, 0x82, 0x00, 0xAA, 0x00, 0x4B, 0xA9,
            0x0B, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
            0x00,
        };
        rStrm.Write( aFixHeader, nFixHdrLen );
        // write reference string including length+1
        UINT32 nStrLen( pLinkStr->Len() + 1 );
        SwWW8Writer::WriteLong( rStrm, nStrLen );
        SwWW8Writer::WriteString16( rStrm, *(pLinkStr), false );
        // write additional two NULL Bytes
        SwWW8Writer::WriteLong( rStrm, 0 );
        // write length of hyperlink data
        const UINT32 nCurrPos = rStrm.Tell();
        rStrm.Seek( nLinkPosInDataStrm );
        SVBT32 nLen;
        UInt32ToSVBT32( nCurrPos - nLinkPosInDataStrm, nLen );
        rStrm.Write( nLen, 4 );
        rStrm.Seek( nCurrPos );

        // write attributes of hyperlink character 0x01
        SwWW8Writer::InsUInt16( aItems, NS_sprm::LN_CFFldVanish );
        aItems.Insert( (BYTE)0x81, aItems.Count() );
        SwWW8Writer::InsUInt16( aItems, NS_sprm::LN_CPicLocation );
        SwWW8Writer::InsUInt32( aItems, nLinkPosInDataStrm );
        SwWW8Writer::InsUInt16( aItems, NS_sprm::LN_CFData );
        aItems.Insert( (BYTE)0x01, aItems.Count() );
    }

    //Technically we should probably Remove all attribs
    //here for the 0x13, 0x14, 0x15, but our import
    //is slightly lacking
    //aItems.Remove(0, aItems.Count());
    // fSpec-Attribute true
    if( rWrt.bWrtWW8 )
    {
        SwWW8Writer::InsUInt16( aItems, NS_sprm::LN_CFSpec );
        aItems.Insert( 1, aItems.Count() );
    }
    else
    {
        aItems.Insert( 117, aItems.Count() ); //sprmCFSpec
        aItems.Insert( 1, aItems.Count() );
    }

    rWrt.pChpPlc->AppendFkpEntry(rWrt.Strm().Tell(), aItems.Count(),
        aItems.GetData());
}

String lcl_GetExpandedField(const SwField &rFld)
{
    String sRet(rFld.Expand());

    //replace LF 0x0A with VT 0x0B
    sRet.SearchAndReplaceAll(0x0A, 0x0B);

    return sRet;
}

WW8_WrPlcFld* WW8Export::CurrentFieldPlc() const
{
    WW8_WrPlcFld* pFldP = NULL;
    switch (nTxtTyp)
    {
        case TXT_MAINTEXT:
            pFldP = pFldMain;
            break;
        case TXT_HDFT:
            pFldP = pFldHdFt;
            break;
        case TXT_FTN:
            pFldP = pFldFtn;
            break;
        case TXT_EDN:
            pFldP = pFldEdn;
            break;
        case TXT_ATN:
            pFldP = pFldAtn;
            break;
        case TXT_TXTBOX:
            pFldP = pFldTxtBxs;
            break;
        case TXT_HFTXTBOX:
            pFldP = pFldHFTxtBxs;
            break;
        default:
            ASSERT( !this, "was ist das fuer ein SubDoc-Type?" );
    }
    return pFldP;
}

void WW8Export::OutputField( const SwField* pFld, ww::eField eFldType,
    const String& rFldCmd, BYTE nMode )
{
    bool bUnicode = IsUnicode();
    WW8_WrPlcFld* pFldP = CurrentFieldPlc();

    // --> OD 2008-08-14 #158418#
    const bool bIncludeEmptyPicLocation = ( eFldType == ww::ePAGE );
    // <--
    if (WRITEFIELD_START & nMode)
    {
        BYTE aFld13[2] = { 0x13, 0x00 };  // will change
        //#i3958#, Needed to make this field work correctly in Word 2000
        if (eFldType == ww::eSHAPE)
            aFld13[0] |= 0x80;
        aFld13[1] = static_cast< BYTE >(eFldType);  // Typ nachtragen
        pFldP->Append( Fc2Cp( Strm().Tell() ), aFld13 );
        // --> OD 2008-08-14 #158418#
        InsertSpecialChar( *this, 0x13, 0, bIncludeEmptyPicLocation );
        // <--
    }
    if (WRITEFIELD_CMD_START & nMode)
    {
        if (bUnicode)
            SwWW8Writer::WriteString16(Strm(), rFldCmd, false);
        else
        {
            SwWW8Writer::WriteString8(Strm(), rFldCmd, false,
                RTL_TEXTENCODING_MS_1252);
        }
        // --> OD 2005-06-08 #i43956# - write hyperlink character including
        // attributes and corresponding binary data for certain reference fields.
        bool bHandleBookmark = false;

        if (pFld)
        {
            if (pFld->GetTyp()->Which() == RES_GETREFFLD &&
                ( eFldType == ww::ePAGEREF || eFldType == ww::eREF ||
                  eFldType == ww::eNOTEREF || eFldType == ww::eFOOTREF ))
                bHandleBookmark = true;

#if 0
            if (pFld->GetTyp()->Which() == RES_INPUTFLD &&
                eFldType == ww::eFORMTEXT)
                bHandleBookmark = true;
#endif
        }

        if ( bHandleBookmark )
        {
            // retrieve reference destionation - the name of the bookmark
            String aLinkStr;
            const USHORT nSubType = pFld->GetSubType();
            const SwGetRefField& rRFld = *(static_cast<const SwGetRefField*>(pFld));
            if ( nSubType == REF_SETREFATTR ||
                 nSubType == REF_BOOKMARK )
            {
                aLinkStr = GetBookmarkName( nSubType, &rRFld.GetSetRefName(), 0 );
            }
            else if ( nSubType == REF_FOOTNOTE ||
                      nSubType == REF_ENDNOTE )
            {
                aLinkStr = GetBookmarkName( nSubType, 0, rRFld.GetSeqNo() );
            }
            else if ( nSubType == REF_SEQUENCEFLD )
            {
                aLinkStr = pFld->GetPar2();
            }
            // insert hyperlink character including attributes and data.
            InsertSpecialChar( *this, 0x01, &aLinkStr );
        }
        // <--
    }
    if (WRITEFIELD_CMD_END & nMode)
    {
        static const BYTE aFld14[2] = { 0x14, 0xff };
        pFldP->Append( Fc2Cp( Strm().Tell() ), aFld14 );
        pFldP->ResultAdded();
        // --> OD 2008-08-14 #158418#
        InsertSpecialChar( *this, 0x14, 0, bIncludeEmptyPicLocation );
        // <--
    }
    if (WRITEFIELD_END & nMode)
    {
        String sOut;
        if( pFld )
            sOut = lcl_GetExpandedField(*pFld);
        else
            sOut = rFldCmd;
        if( sOut.Len() )
        {
            if( bUnicode )
                SwWW8Writer::WriteString16(Strm(), sOut, false);
            else
            {
                SwWW8Writer::WriteString8(Strm(), sOut, false,
                    RTL_TEXTENCODING_MS_1252);
            }

            if (pFld)
            {
                if (pFld->GetTyp()->Which() == RES_INPUTFLD &&
                    eFldType == ww::eFORMTEXT)
                {
                    BYTE aArr[12];
                    BYTE *pArr = aArr;

                    if ( bWrtWW8 )
                    {
                        Set_UInt16( pArr, NS_sprm::LN_CPicLocation );
                        Set_UInt32( pArr, 0x0 );

                        Set_UInt16( pArr, NS_sprm::LN_CFSpec );
                        Set_UInt8( pArr, 1 );

                        Set_UInt16( pArr, NS_sprm::LN_CFNoProof );
                        Set_UInt8( pArr, 1 );
                    }
                    else
                    {
                        Set_UInt8(pArr, 0x68); //sprmCPicLocation
                        Set_UInt32(pArr, 0x0);

                        Set_UInt8( pArr, 117 ); //sprmCFSpec
                        Set_UInt8( pArr, 1 );
                    }
                    pChpPlc->AppendFkpEntry( Strm().Tell(), static_cast< short >(pArr - aArr), aArr );
                }
            }
        }
    }
    if (WRITEFIELD_CLOSE & nMode)
    {
        BYTE aFld15[2] = { 0x15, 0x80 };

        if (pFld)
        {
            if (pFld->GetTyp()->Which() == RES_INPUTFLD &&
                eFldType == ww::eFORMTEXT)
            {
                USHORT nSubType = pFld->GetSubType();

                if (nSubType == REF_SEQUENCEFLD)
                    aFld15[0] |= (0x4 << 5);
            }
        }

        pFldP->Append( Fc2Cp( Strm().Tell() ), aFld15 );
        // --> OD 2008-08-14 #158418#
        InsertSpecialChar( *this, 0x15, 0, bIncludeEmptyPicLocation );
        // <--
    }
}

void WW8Export::StartCommentOutput(const String& rName)
{
    String sStr(FieldString(ww::eQUOTE));
    sStr.APPEND_CONST_ASC("[");
    sStr += rName;
    sStr.APPEND_CONST_ASC("] ");
    OutputField(0, ww::eQUOTE, sStr, WRITEFIELD_START | WRITEFIELD_CMD_START);
}

void WW8Export::EndCommentOutput(const String& rName)
{
    String sStr(CREATE_CONST_ASC(" ["));
    sStr += rName;
    sStr.APPEND_CONST_ASC("] ");
    OutputField(0, ww::eQUOTE, sStr, WRITEFIELD_CMD_END | WRITEFIELD_END |
        WRITEFIELD_CLOSE);
}

USHORT MSWordExportBase::GetId( const SwTOXType& rTOXType )
{
    void* p = (void*)&rTOXType;
    USHORT nRet = aTOXArr.GetPos( p );
    if( USHRT_MAX == nRet )
        aTOXArr.Insert( p, nRet = aTOXArr.Count() );
    return nRet;
}

// return values:  1 - no PageNum,
//                 2 - TabStop before PageNum,
//                 3 - Text before PageNum - rTxt hold the text
//                 4 - no Text and no TabStop before PageNum
int lcl_CheckForm( const SwForm& rForm, BYTE nLvl, String& rText )
{
    int nRet = 4;
    rText.Erase();

    // #i21237#
    SwFormTokens aPattern = rForm.GetPattern(nLvl);
    SwFormTokens::iterator aIt = aPattern.begin();
    bool bPgNumFnd = false;
    FormTokenType eTType;

    // #i61362#
    if (! aPattern.empty())
    {
        // #i21237#
        while( ++aIt != aPattern.end() && !bPgNumFnd )
        {
            eTType = aIt->eTokenType;

            switch( eTType )
            {
            case TOKEN_PAGE_NUMS:
                bPgNumFnd = true;
                break;

            case TOKEN_TAB_STOP:
                nRet = 2;
                break;
            case TOKEN_TEXT:
                nRet = 3;
                rText = aIt->sText.Copy( 0, 5 ); // #i21237#
                break;

            case TOKEN_LINK_START:
            case TOKEN_LINK_END:
                break;

            default:
                nRet = 4;
                break;
            }
        }

        if( !bPgNumFnd )
            nRet = 1;
    }

    return nRet;
}

bool lcl_IsHyperlinked(const SwForm& rForm, USHORT nTOXLvl)
{
    bool bRes = false;
    for (USHORT nI = 1; nI < nTOXLvl; ++nI)
    {
        // #i21237#
        SwFormTokens aPattern = rForm.GetPattern(nI);

        if ( !aPattern.empty() )
        {
            SwFormTokens::iterator aIt = aPattern.begin();

            FormTokenType eTType;

            // #i21237#
            while ( ++aIt != aPattern.end() )
            {
                eTType = aIt->eTokenType;
                switch (eTType)
                {
                case TOKEN_LINK_START:
                case TOKEN_LINK_END:
                    bRes = true;
                break;
                default:
                    ;
                }
            }
        }
    }
    return bRes;
}

void AttributeOutputBase::StartTOX( const SwSection& rSect )
{
    if ( const SwTOXBase* pTOX = rSect.GetTOXBase() )
    {
        static const sal_Char sEntryEnd[] = "\" ";

        ww::eField eCode = ww::eTOC;
        String sStr;
        switch (pTOX->GetType())
        {
        case TOX_INDEX:
            eCode = ww::eINDEX;
            sStr = FieldString(eCode);

            if (pTOX->GetTOXForm().IsCommaSeparated())
                sStr.APPEND_CONST_ASC("\\r ");

            if (nsSwTOIOptions::TOI_ALPHA_DELIMITTER & pTOX->GetOptions())
                sStr.APPEND_CONST_ASC("\\h \"A\" ");

            {
                String aFillTxt;
                for (BYTE n = 1; n <= 3; ++n)
                {
                    String aTxt;
                    int nRet = ::lcl_CheckForm(pTOX->GetTOXForm(), n, aTxt);

                    if( 3 == nRet )
                        aFillTxt = aTxt;
                    else if ((4 == nRet) || (2 == nRet)) //#109414#
                        aFillTxt = '\t';
                    else
                        aFillTxt.Erase();
                }
                sStr.APPEND_CONST_ASC("\\e \"");
                sStr += aFillTxt;
                sStr.AppendAscii(sEntryEnd);
            }
            break;

//      case TOX_AUTHORITIES:   eCode = eTOA; sStr = ???; break;

        case TOX_ILLUSTRATIONS:
        case TOX_OBJECTS:
        case TOX_TABLES:
            if (!pTOX->IsFromObjectNames())
            {
                sStr = FieldString(eCode);

                sStr.APPEND_CONST_ASC("\\c \"");
                sStr += pTOX->GetSequenceName();
                sStr.AppendAscii(sEntryEnd);

                String aTxt;
                int nRet = ::lcl_CheckForm( pTOX->GetTOXForm(), 1, aTxt );
                if (1 == nRet)
                    sStr.APPEND_CONST_ASC("\\n ");
                else if( 3 == nRet || 4 == nRet )
                {
                    sStr.APPEND_CONST_ASC("\\p \"");
                    sStr += aTxt;
                    sStr.AppendAscii(sEntryEnd);
                }
            }
            break;

//      case TOX_USER:
//      case TOX_CONTENT:
        default:
            {
                sStr = FieldString(eCode);

                String sTOption;
                USHORT n, nTOXLvl = pTOX->GetLevel();
                if( !nTOXLvl )
                    ++nTOXLvl;

                if( nsSwTOXElement::TOX_MARK & pTOX->GetCreateType() )
                {
                    sStr.APPEND_CONST_ASC( "\\f " );

                    if( TOX_USER == pTOX->GetType() )
                    {
                         sStr += '\"';
                         sStr += (sal_Char)( 'A' + GetExport( ).GetId( *pTOX->GetTOXType() ) );
                         sStr.AppendAscii( sEntryEnd );
                    }

                if( nsSwTOXElement::TOX_OUTLINELEVEL & pTOX->GetCreateType() )
                {
                    // --> OD 2009-02-27 #i99641#
                    // The following code does not determine the minimum outline
                    // level for the TOC
//                    // Search over all the outline styles used and figure out
//                    // what is the minimum outline level we need to display
//                    // (ignoring headline styles 1-9)
//                    //BYTE nLvl = 0, nMinLvl = 0; //#outline level, removed by zhaojianwei
//                    int nLvl = 0, nMinLvl = 0;      //<-end,add by zhaojianwei
//                    const SwTxtFmtColls& rColls = *GetExport().pDoc->GetTxtFmtColls();
//                    const SwTxtFmtColl* pColl;
//                    for( n = rColls.Count(); n; )
//                    {
//                        pColl = rColls[ --n ];
//                        //nLvl = pColl->GetOutlineLevel();    //#outline level,zhaojianwei
//                        //USHORT nPoolId = pColl->GetPoolFmtId();
//                        //if( MAXLEVEL > nLvl && nMinLvl < nLvl &&        //<-end, ->add by zhaojianwei
//                        USHORT nPoolId = pColl->GetPoolFmtId();
//                        if( pColl->IsAssignedToListLevelOfOutlineStyle() &&
//                          nMinLvl < (nLvl = pColl->GetAssignedOutlineStyleLevel()) && //<-end,zhaojianwei
//                            ( RES_POOLCOLL_HEADLINE1 > nPoolId ||
//                              RES_POOLCOLL_HEADLINE9 < nPoolId ))
//                        {
//                            // If we are using the default heading styles then use nTOXLvl
//                            if(!nMinLvl)
//                                nLvl = nTOXLvl;
//                            else
//                                nLvl = nMinLvl < nTOXLvl ? nMinLvl : (BYTE)nTOXLvl;
//                            nMinLvl = nLvl;
//                        }
//                    }
                    const int nMinLvl = nTOXLvl;

//                    if( nLvl )
                    if ( nMinLvl > 0 )
                    {
                        int nTmpLvl = nMinLvl;
                        if (nTmpLvl > WW8ListManager::nMaxLevel)
                            nTmpLvl = WW8ListManager::nMaxLevel;

                        sStr.APPEND_CONST_ASC( "\\o \"1-" );
                        sStr += String::CreateFromInt32( nTmpLvl );
                        sStr.AppendAscii(sEntryEnd);

                    }
                    // <--

                    // --> OD 2009-02-27 #i99641#
                    // not needed to additional export paragraph style with
                    // an outline level to the /t option
//                    if( nMinLvl > 0 )
//                    // <--
//                    {
//                        // collect this templates into the \t otion
//                        const SwTxtFmtColls& rColls = *pDoc->GetTxtFmtColls();
//                        const SwTxtFmtColl* pColl;
//                        int nLvl = 0;
//                        for( n = rColls.Count(); n;)
//                        {
//                            pColl = rColls[--n];
//                            //nLvl =  pColl->GetOutlineLevel();         //#outline level, removed by zhaojianwei
//                            //if (MAXLEVEL > nLvl && nMinLvl <= nLvl)
//                            //{                                         //<-end, ->add by zhaojianwei
//                            if( pColl->IsAssignedToListLevelOfOutlineStyle() &&
//                                nMinLvl <= ( nLvl = pColl->GetAssignedOutlineStyleLevel()))
//                            {                                           //<-end,zhaojianwei
//                                if( sTOption.Len() )
//                                    sTOption += ';';
//                                (( sTOption += pColl->GetName() ) += ';' )
//                                        += String::CreateFromInt32( nLvl + 1 );
//                            }
//                        }
//                    }

                }
                

        if( nsSwTOXElement::TOX_OUTLINELEVEL & pTOX->GetCreateType() )
                  {
            // Take the TOC value of the max level to evaluate to as
            // the starting point for the \o flag, but reduce it to the 
            // value of the highest outline level filled by a *standard*
            // Heading 1 - 9 style because \o "Builds a table of
            // contents from paragraphs formatted with built-in heading
            // styles". And afterward fill in any outline styles left
            // uncovered by that range to the \t flag
            //
            // i.e. for
            // Heading 1
            // Heading 2
            // custom-style
            // Heading 4
            // output
            // \o 1-2 \tcustom-style,3,Heading 3,4
 
            // Search over all the outline styles used and figure out
            // what is the minimum outline level (if any) filled by a
            // non-standard style for that level, i.e. ignore headline
            // styles 1-9 and find the lowest valid outline level
            BYTE nPosOfLowestNonStandardLvl = MAXLEVEL;
            const SwTxtFmtColls& rColls = *GetExport().pDoc->GetTxtFmtColls();
            for( n = rColls.Count(); n; )
                      {
            const SwTxtFmtColl* pColl = rColls[ --n ];
            USHORT nPoolId = pColl->GetPoolFmtId();
            if (
                //Is a Non-Standard Outline Style
                (RES_POOLCOLL_HEADLINE1 > nPoolId || RES_POOLCOLL_HEADLINE9 < nPoolId) &&
                //Has a valid outline level
                (pColl->IsAssignedToListLevelOfOutlineStyle()) &&
                // Is less than the lowest known non-standard level
                (pColl->GetAssignedOutlineStyleLevel() < nPosOfLowestNonStandardLvl)
                            )
                          {
                              nPosOfLowestNonStandardLvl = ::sal::static_int_cast<BYTE>(pColl->GetAssignedOutlineStyleLevel());
                          }
                      }
  
            BYTE nMaxMSAutoEvaluate = nPosOfLowestNonStandardLvl < nTOXLvl ? nPosOfLowestNonStandardLvl : (BYTE)nTOXLvl;
  
            //output \o 1-X where X is the highest normal outline style to be included in the toc
            if ( nMaxMSAutoEvaluate )
              {
            if (nMaxMSAutoEvaluate > WW8ListManager::nMaxLevel)
              nMaxMSAutoEvaluate = WW8ListManager::nMaxLevel;
  
            sStr.APPEND_CONST_ASC( "\\o \"1-" );
            sStr += String::CreateFromInt32( nMaxMSAutoEvaluate );
            sStr.AppendAscii(sEntryEnd);
                      }
  
            //collect up any other styles in the writer TOC which will
            //not already appear in the MS TOC and place then into the
            //\t option
            if( nMaxMSAutoEvaluate < nTOXLvl )
                      {
            // collect this templates into the \t otion
            for( n = rColls.Count(); n;)
                          {
                const SwTxtFmtColl* pColl = rColls[ --n ];
                if (!pColl->IsAssignedToListLevelOfOutlineStyle())
                  continue;
                BYTE nTestLvl =  ::sal::static_int_cast<BYTE>(pColl->GetAssignedOutlineStyleLevel());
                if (nTestLvl < nTOXLvl && nTestLvl >= nMaxMSAutoEvaluate)
                  {
                if( sTOption.Len() )
                  sTOption += ',';
                (( sTOption += pColl->GetName() ) += ',' )
                  += String::CreateFromInt32( nTestLvl + 1 );
                  }
              }
              }
                  }

                if( nsSwTOXElement::TOX_TEMPLATE & pTOX->GetCreateType() )
                    // --> OD 2009-02-27 #i99641#
                    // Consider additional styles regardless of TOX-outlinelevel
                    for( n = 0; n < MAXLEVEL; ++n )
                    // <--
                    {
                        const String& rStyles = pTOX->GetStyleNames( n );
                        if( rStyles.Len() )
                        {
                            xub_StrLen nPos = 0;
                            String sLvl( ',' );
                            sLvl += String::CreateFromInt32( n + 1 );
                            do {
                                String sStyle( rStyles.GetToken( 0,
                                            TOX_STYLE_DELIMITER, nPos ));
                                if( sStyle.Len() )
                                {
                                    SwTxtFmtColl* pColl = GetExport().pDoc->FindTxtFmtCollByName(sStyle);
                                    if (!pColl->IsAssignedToListLevelOfOutlineStyle() || pColl->GetAssignedOutlineStyleLevel() < nTOXLvl)
                                    {
                                        if( sTOption.Len() )
                                            sTOption += ',';
                                        ( sTOption += sStyle ) += sLvl;
                                    }
                                }
                            } while( STRING_NOTFOUND != nPos );
                        }
                    }

                {
                    String aFillTxt;
                    BYTE nNoPgStt = MAXLEVEL, nNoPgEnd = MAXLEVEL;
                    bool bFirstFillTxt = true, bOnlyText = true;
                    for( n = 0; n < nTOXLvl; ++n )
                    {
                        String aTxt;
                        int nRet = ::lcl_CheckForm( pTOX->GetTOXForm(),
                                                    static_cast< BYTE >(n+1), aTxt );
                        if( 1 == nRet )
                        {
                            bOnlyText = false;
                            if( MAXLEVEL == nNoPgStt )
                                nNoPgStt = static_cast< BYTE >(n+1);
                        }
                        else
                        {
                            if( MAXLEVEL != nNoPgStt &&
                                MAXLEVEL == nNoPgEnd )
                                nNoPgEnd = BYTE(n);

                            bOnlyText = bOnlyText && 3 == nRet;
                            if( 3 == nRet || 4 == nRet )
                            {
                                if( bFirstFillTxt )
                                    aFillTxt = aTxt;
                                else if( aFillTxt != aTxt )
                                    aFillTxt.Erase();
                                bFirstFillTxt = false;
                            }
                        }
                    }
                    if( MAXLEVEL != nNoPgStt )
                    {
                        if (WW8ListManager::nMaxLevel < nNoPgEnd)
                            nNoPgEnd = WW8ListManager::nMaxLevel;
                        sStr.APPEND_CONST_ASC( "\\n " );
                        sStr += String::CreateFromInt32( nNoPgStt );
                        sStr += '-';
                        sStr += String::CreateFromInt32( nNoPgEnd  );
                        sStr += ' ';
                    }
                    if( bOnlyText )
                    {
                        sStr.APPEND_CONST_ASC( "\\p \"" );
                        sStr += aFillTxt;
                        sStr.AppendAscii(sEntryEnd);
                    }
                }

                if( sTOption.Len() )
                {
                    sStr.APPEND_CONST_ASC( "\\t \"" );
                    sStr += sTOption;
                    sStr.AppendAscii(sEntryEnd);
                }

                if (lcl_IsHyperlinked(pTOX->GetTOXForm(), nTOXLvl))
                    sStr.APPEND_CONST_ASC("\\h");
            }
            break;
            }
        }

        if( sStr.Len() )
        {
            GetExport( ).bInWriteTOX = true;
            GetExport( ).OutputField( 0, eCode, sStr, WRITEFIELD_START | WRITEFIELD_CMD_START |
                WRITEFIELD_CMD_END );
        }
        
        GetExport( ).bStartTOX = false;
    }
}

void AttributeOutputBase::EndTOX( const SwSection& rSect )
{
    const SwTOXBase* pTOX = rSect.GetTOXBase();
    if ( pTOX )
    {
        ww::eField eCode = TOX_INDEX == pTOX->GetType() ? ww::eINDEX : ww::eTOC;
        GetExport( ).OutputField( 0, eCode, aEmptyStr, WRITEFIELD_CLOSE );
    }
    GetExport( ).bInWriteTOX = false;
}

bool MSWordExportBase::GetNumberFmt(const SwField& rFld, String& rStr)
{
    // Returns a date or time format string by using the US NfKeywordTable
    bool bHasFmt = false;
    SvNumberFormatter* pNFmtr = pDoc->GetNumberFormatter();
    UINT32 nFmtIdx = rFld.GetFormat();
    const SvNumberformat* pNumFmt = pNFmtr->GetEntry( nFmtIdx );
    if( pNumFmt )
    {
        //USHORT nLng = rFld.GetLanguage();
        LocaleDataWrapper aLocDat( pNFmtr->GetServiceManager(),
            MsLangId::convertLanguageToLocale( LANGUAGE_ENGLISH_US ) );

        if( !pKeyMap )
        {
            pKeyMap = new NfKeywordTable;
            NfKeywordTable& rKeyMap = *(NfKeywordTable*)pKeyMap;
            pNFmtr->FillKeywordTable( rKeyMap, LANGUAGE_ENGLISH_US );
        }

        String sFmt(pNumFmt->GetMappedFormatstring(*(NfKeywordTable*)pKeyMap,
            aLocDat));
        if (sFmt.Len())
        {
            sw::ms::SwapQuotesInField(sFmt);

            rStr.APPEND_CONST_ASC( "\\@\"" );
            rStr += sFmt;
            rStr.APPEND_CONST_ASC( "\" " );
            bHasFmt = true;
        }
    }
    return bHasFmt;
}

void AttributeOutputBase::GetNumberPara( String& rStr, const SwField& rFld )
{
    switch(rFld.GetFormat())
    {
        case SVX_NUM_CHARS_UPPER_LETTER:
        case SVX_NUM_CHARS_UPPER_LETTER_N:
            rStr.APPEND_CONST_ASC( "\\*ALPHABETIC ");
            break;
        case SVX_NUM_CHARS_LOWER_LETTER:
        case SVX_NUM_CHARS_LOWER_LETTER_N:
            rStr.APPEND_CONST_ASC("\\*alphabetic ");
            break;
        case SVX_NUM_ROMAN_UPPER:
            rStr.APPEND_CONST_ASC("\\*ROMAN ");
            break;
        case SVX_NUM_ROMAN_LOWER:
            rStr.APPEND_CONST_ASC("\\*roman ");
            break;
        default:
            ASSERT(rFld.GetFormat() == SVX_NUM_ARABIC,
                "Unknown numbering type exported as default\n");
        case SVX_NUM_ARABIC:
            rStr.APPEND_CONST_ASC("\\*Arabic ");
            break;
        case SVX_NUM_PAGEDESC:
            //Nothing, use word's default
            break;
    }
}

void WW8Export::WritePostItBegin( WW8Bytes* pOut )
{
    BYTE aArr[ 3 ];
    BYTE* pArr = aArr;

    // sprmCFSpec true
    if( bWrtWW8 )
        Set_UInt16( pArr, NS_sprm::LN_CFSpec );
    else
        Set_UInt8( pArr, 117 ); //sprmCFSpec
    Set_UInt8( pArr, 1 );

    pChpPlc->AppendFkpEntry( Strm().Tell() );
    WriteChar( 0x05 );              // Annotation reference

    if( pOut )
        pOut->Insert( aArr, static_cast< USHORT >(pArr - aArr), pOut->Count() );
    else
        pChpPlc->AppendFkpEntry( Strm().Tell(), static_cast< short >(pArr - aArr), aArr );
}

String FieldString(ww::eField eIndex)
{
    String sRet(CREATE_CONST_ASC("  "));
    if (const char *pField = ww::GetEnglishFieldName(eIndex))
        sRet.InsertAscii(pField, 1);
    return sRet;
}

void WW8AttributeOutput::HiddenField( const SwField& rFld )
{
    String sExpand(rFld.GetPar2());

    //replace LF 0x0A with VT 0x0B
    sExpand.SearchAndReplaceAll(0x0A, 0x0B);
    m_rWW8Export.pChpPlc->AppendFkpEntry(m_rWW8Export.Strm().Tell());
    if (m_rWW8Export.IsUnicode())
    {
        SwWW8Writer::WriteString16(m_rWW8Export.Strm(), sExpand, false);
        static BYTE aArr[] =
        {
            0x3C, 0x08, 0x1
        };
        m_rWW8Export.pChpPlc->AppendFkpEntry(m_rWW8Export.Strm().Tell(), sizeof(aArr), aArr);
    }
    else
    {
        SwWW8Writer::WriteString8(m_rWW8Export.Strm(), sExpand, false,
            RTL_TEXTENCODING_MS_1252);
        static BYTE aArr[] =
        {
            92, 0x1
        };
        m_rWW8Export.pChpPlc->AppendFkpEntry(m_rWW8Export.Strm().Tell(), sizeof(aArr), aArr);
    }
}

void WW8AttributeOutput::SetField( const SwField& rFld, ww::eField eType, const String& rCmd )
{
    const SwSetExpField* pSet=(const SwSetExpField*)(&rFld);
    const String &rVar = pSet->GetPar2();

    ULONG nFrom = m_rWW8Export.Fc2Cp(m_rWW8Export.Strm().Tell());

    GetExport().OutputField(&rFld, eType, rCmd, WRITEFIELD_START |
        WRITEFIELD_CMD_START | WRITEFIELD_CMD_END);

    /*
    Is there a bookmark at the start position of this field, if so
    move it to the 0x14 of the result of the field.  This is what word
    does. MoveFieldMarks moves any bookmarks at this position to
    the beginning of the field result, and marks the bookmark as a
    fieldbookmark which is to be ended before the field end mark
    instead of after it like a normal bookmark.
    */
    m_rWW8Export.MoveFieldMarks(nFrom,m_rWW8Export.Fc2Cp(m_rWW8Export.Strm().Tell()));

    if (rVar.Len())
    {
        if (m_rWW8Export.IsUnicode())
            SwWW8Writer::WriteString16(m_rWW8Export.Strm(), rVar, false);
        else
        {
            SwWW8Writer::WriteString8(m_rWW8Export.Strm(), rVar, false,
                RTL_TEXTENCODING_MS_1252);
        }
    }
    GetExport().OutputField(&rFld, eType, rCmd, WRITEFIELD_CLOSE);
}

void WW8AttributeOutput::PostitField( const SwField* pFld ) 
{
    const SwPostItField& rPFld = *(SwPostItField*)pFld;
    m_rWW8Export.pAtn->Append( m_rWW8Export.Fc2Cp( m_rWW8Export.Strm().Tell() ), rPFld );
    m_rWW8Export.WritePostItBegin( m_rWW8Export.pO );
}

bool WW8AttributeOutput::DropdownField( const SwField* pFld )
{
    bool bExpand = true;
    if ( m_rWW8Export.bWrtWW8 )
    {
        const SwDropDownField& rFld2 = *(SwDropDownField*)pFld;
        uno::Sequence<rtl::OUString> aItems =
            rFld2.GetItemSequence();
        GetExport().DoComboBox(rFld2.GetName(),
                           rFld2.GetHelp(),
                           rFld2.GetToolTip(),
                           rFld2.GetSelectedItem(), aItems);
        bExpand = false;
    }
    return bExpand;
}

void WW8AttributeOutput::RefField( const SwField &rFld, const String &rRef)
{
    String sStr( FieldString( ww::eREF ) );
    sStr.APPEND_CONST_ASC( "\"" );
    sStr += rRef;
    sStr.APPEND_CONST_ASC( "\" " );
    m_rWW8Export.OutputField( &rFld, ww::eREF, sStr, WRITEFIELD_START |
        WRITEFIELD_CMD_START | WRITEFIELD_CMD_END );
    String sVar = lcl_GetExpandedField( rFld );
    if ( sVar.Len() )
    {
        if ( m_rWW8Export.IsUnicode() )
            SwWW8Writer::WriteString16( m_rWW8Export.Strm(), sVar, false );
        else
        {
            SwWW8Writer::WriteString8( m_rWW8Export.Strm(), sVar, false,
                RTL_TEXTENCODING_MS_1252 );
        }
    }
    m_rWW8Export.OutputField( &rFld, ww::eREF, sStr, WRITEFIELD_CLOSE );
}

void WW8AttributeOutput::WriteExpand( const SwField* pFld )
{
    String sExpand( lcl_GetExpandedField( *pFld ) );
    if ( m_rWW8Export.IsUnicode() )
        SwWW8Writer::WriteString16( m_rWW8Export.Strm(), sExpand, false );
    else
    {
        SwWW8Writer::WriteString8( m_rWW8Export.Strm(), sExpand, false,
            RTL_TEXTENCODING_MS_1252 );
    }
}

void AttributeOutputBase::TextField( const SwFmtFld& rField )
{
    const SwField* pFld = rField.GetFld();
    String sStr;        // fuer optionale Parameter
    bool bWriteExpand = false;
    USHORT nSubType = pFld->GetSubType();

    switch (pFld->GetTyp()->Which())
    {
    case RES_GETEXPFLD:
        if (nSubType == nsSwGetSetExpType::GSE_STRING)
        {
            const SwGetExpField *pGet=(const SwGetExpField*)(pFld);
            RefField( *pGet, pGet->GetFormula() );
        }
        else
            bWriteExpand = true;
        break;
    case RES_SETEXPFLD:
        if (nsSwGetSetExpType::GSE_SEQ == nSubType)
        {
            sStr = FieldString(ww::eSEQ);
            sStr.APPEND_CONST_ASC("\"");
            sStr += pFld->GetTyp()->GetName();
            sStr.APPEND_CONST_ASC( "\" " );

            GetNumberPara( sStr, *pFld );
            GetExport().OutputField(pFld, ww::eSEQ, sStr);
        }
        else if (nSubType & nsSwGetSetExpType::GSE_STRING)
        {
            bool bShowAsWell = false;
            ww::eField eFieldNo;
            const SwSetExpField *pSet=(const SwSetExpField*)(pFld);
            const String &rVar = pSet->GetPar2();
            if (pSet->GetInputFlag())
            {
                sStr = FieldString(ww::eASK);
                sStr.APPEND_CONST_ASC("\"");
                sStr += pSet->GetPar1();
                sStr.APPEND_CONST_ASC( "\" " );
                sStr += pSet->GetPromptText();
                sStr.APPEND_CONST_ASC( " \\d " );
                sStr += rVar;
                eFieldNo = ww::eASK;
            }
            else
            {
                sStr = FieldString(ww::eSET);
                sStr += pSet->GetPar1();
                sStr.APPEND_CONST_ASC(" \"");
                sStr += rVar;
                sStr.APPEND_CONST_ASC("\" ");
                eFieldNo = ww::eSET;
                bShowAsWell = (nSubType & nsSwExtendedSubType::SUB_INVISIBLE) ? false : true;
            }

            SetField( *pFld, eFieldNo, sStr );

            if (bShowAsWell)
                RefField( *pSet, pSet->GetPar1() );
        }
        else
            bWriteExpand = true;
        break;
    case RES_PAGENUMBERFLD:
        sStr = FieldString(ww::ePAGE);
        GetNumberPara(sStr, *pFld);
        GetExport().OutputField(pFld, ww::ePAGE, sStr);
        break;
    case RES_FILENAMEFLD:
        sStr = FieldString(ww::eFILENAME);
        if (pFld->GetFormat() == FF_PATHNAME)
            sStr.APPEND_CONST_ASC("\\p ");
        GetExport().OutputField(pFld, ww::eFILENAME, sStr);
        break;
    case RES_DBNAMEFLD:
        {
            sStr = FieldString(ww::eDATABASE);
            SwDBData aData = GetExport().pDoc->GetDBData();
            sStr += String(aData.sDataSource);
            sStr += DB_DELIM;
            sStr += String(aData.sCommand);
            GetExport().OutputField(pFld, ww::eDATABASE, sStr);
        }
        break;
    case RES_AUTHORFLD:
        {
            ww::eField eFld =
                (AF_SHORTCUT & nSubType ? ww::eUSERINITIALS : ww::eUSERNAME);
            GetExport().OutputField(pFld, eFld, FieldString(eFld));
        }
        break;
    case RES_TEMPLNAMEFLD:
        GetExport().OutputField(pFld, ww::eTEMPLATE, FieldString(ww::eTEMPLATE));
        break;
    case RES_DOCINFOFLD:    // Last printed, last edited,...
        if( DI_SUB_FIXED & nSubType )
            bWriteExpand = true;
        else
        {
            ww::eField eFld(ww::eNONE);
            switch (0xff & nSubType)
            {
                case DI_TITEL:
                    eFld = ww::eTITLE;
                    break;
                case DI_THEMA:
                    eFld = ww::eSUBJECT;
                    break;
                case DI_KEYS:
                    eFld = ww::eKEYWORDS;
                    break;
                case DI_COMMENT:
                    eFld = ww::eCOMMENTS;
                    break;
                case DI_DOCNO:
                    eFld = ww::eREVNUM;
                    break;
                case DI_CREATE:
                    if (DI_SUB_AUTHOR == (nSubType & DI_SUB_MASK))
                        eFld = ww::eAUTHOR;
                    else if (GetExport().GetNumberFmt(*pFld, sStr))
                        eFld = ww::eCREATEDATE;
                    break;

                case DI_CHANGE:
                    if (DI_SUB_AUTHOR == (nSubType & DI_SUB_MASK))
                        eFld = ww::eLASTSAVEDBY;
                    else if (GetExport().GetNumberFmt(*pFld, sStr))
                        eFld = ww::eSAVEDATE;
                    break;

                case DI_PRINT:
                    if (DI_SUB_AUTHOR != (nSubType & DI_SUB_MASK) &&
                        GetExport().GetNumberFmt(*pFld, sStr))
                        eFld = ww::ePRINTDATE;
                    break;
                case DI_EDIT:
                    if( DI_SUB_AUTHOR != (nSubType & DI_SUB_MASK ) &&
                        GetExport().GetNumberFmt( *pFld, sStr ))
                        eFld = ww::eSAVEDATE;
                    break;
                case DI_CUSTOM:
                    eFld = ww::eDOCPROPERTY;
                    {
                        static String sQuotes('\"');
                        const SwDocInfoField * pDocInfoField =
                        dynamic_cast<const SwDocInfoField *> (pFld);
                        
                        if (pDocInfoField != NULL)
                        {
                            String sFieldname = pDocInfoField->GetCntnt(TRUE);
                            xub_StrLen nIndex = sFieldname.Search(':');
                            
                            if (nIndex != sFieldname.Len())
                                sFieldname = sFieldname.Copy(nIndex + 1);

                            sStr.Insert(sQuotes);
                            sStr.Insert(sFieldname);
                            sStr.Insert(sQuotes);
                        }
                    }
                    break;
                default:
                    break;
            }

            if (eFld != ww::eNONE)
            {
                sStr.Insert(FieldString(eFld), 0);
                GetExport().OutputField(pFld, eFld, sStr);
            }
            else
                bWriteExpand = true;
        }
        break;
    case RES_DATETIMEFLD:
        if (FIXEDFLD & nSubType || !GetExport().GetNumberFmt(*pFld, sStr))
            bWriteExpand = true;
        else
        {
            ww::eField eFld = (DATEFLD & nSubType) ? ww::eDATE : ww::eTIME;
            sStr.Insert(FieldString(eFld), 0);
            GetExport().OutputField(pFld, eFld, sStr);
        }
        break;
    case RES_DOCSTATFLD:
        {
            ww::eField eFld = ww::eNONE;

            switch (nSubType)
            {
                case DS_PAGE:
                    eFld = ww::eNUMPAGE;
                    break;
                case DS_WORD:
                    eFld = ww::eNUMWORDS;
                    break;
                case DS_CHAR:
                    eFld = ww::eNUMCHARS;
                    break;
            }

            if (eFld != ww::eNONE)
            {
                sStr = FieldString(eFld);
                GetNumberPara(sStr, *pFld);
                GetExport().OutputField(pFld, eFld, sStr);
            }
            else
                bWriteExpand = true;
        }
        break;
    case RES_EXTUSERFLD:
        {
            ww::eField eFld = ww::eNONE;
            switch (0xFF & nSubType)
            {
                case EU_FIRSTNAME:
                case EU_NAME:
                    eFld = ww::eUSERNAME;
                    break;
                case EU_SHORTCUT:
                    eFld = ww::eUSERINITIALS;
                    break;
                case EU_STREET:
                case EU_COUNTRY:
                case EU_ZIP:
                case EU_CITY:
                    eFld = ww::eUSERADDRESS;
                    break;
            }

            if (eFld != ww::eNONE)
            {
                sStr = FieldString(eFld);
                GetExport().OutputField(pFld, eFld, sStr);
            }
            else
                bWriteExpand = true;
        }
        break;
    case RES_POSTITFLD:
        //Sadly only possible for word in main document text
        if (GetExport().nTxtTyp == TXT_MAINTEXT)
        {
            PostitField( pFld );
        }
        break;
    case RES_INPUTFLD:
        {
            const SwInputField * pInputField =
                dynamic_cast<const SwInputField *>(pFld);

            if (pInputField->isFormField())
                GetExport().DoFormText(pInputField);
            else
            {
                sStr = FieldString(ww::eFILLIN);

                sStr.APPEND_CONST_ASC("\"");
                sStr += pFld->GetPar2();
                sStr += '\"';

                GetExport().OutputField(pFld, ww::eFILLIN, sStr);
            }
        }
        break;
    case RES_GETREFFLD:
        {
            ww::eField eFld = ww::eNONE;
            const SwGetRefField& rRFld = *(SwGetRefField*)pFld;
            switch (nSubType)
            {
                case REF_SETREFATTR:
                case REF_BOOKMARK:
                    switch (pFld->GetFormat())
                    {
                        case REF_PAGE_PGDESC:
                        case REF_PAGE:
                            eFld = ww::ePAGEREF;
                            break;
                        default:
                            eFld = ww::eREF;
                            break;
                    }
                    sStr = FieldString(eFld);
                    sStr += GetExport().GetBookmarkName(nSubType,
                        &rRFld.GetSetRefName(), 0);
                    break;
                case REF_FOOTNOTE:
                case REF_ENDNOTE:
                    switch (pFld->GetFormat())
                    {
                        case REF_PAGE_PGDESC:
                        case REF_PAGE:
                            eFld = ww::ePAGEREF;
                            break;
                        case REF_UPDOWN:
                            eFld = ww::eREF;
                            break;
                        default:
                            eFld =
                                REF_ENDNOTE == nSubType ? ww::eNOTEREF : ww::eFOOTREF;
                            break;
                    }
                    sStr = FieldString(eFld);
                    sStr += GetExport().GetBookmarkName(nSubType, 0,
                        rRFld.GetSeqNo());
                    break;
            }

            if (eFld != ww::eNONE)
            {
                switch (pFld->GetFormat())
                {
                    case REF_UPDOWN:
                        sStr.APPEND_CONST_ASC(" \\p");
                        break;
                    case REF_CHAPTER:
                        sStr.APPEND_CONST_ASC(" \\n");
                        break;
                    default:
                        break;
                }
                sStr.APPEND_CONST_ASC(" \\h ");       // insert hyperlink
                GetExport().OutputField(pFld, eFld, sStr);
            }
            else
                bWriteExpand = true;
        }
        break;
    case RES_COMBINED_CHARS:
        {
        /*
        We need a font size to fill in the defaults, if these are overridden
        (as they generally are) by character properties then those properties
        win.

        The fontsize that is used in MS for determing the defaults is always
        the CJK fontsize even if the text is not in that language, in OOo the
        largest fontsize used in the field is the one we should take, but
        whatever we do, word will actually render using the fontsize set for
        CJK text. Nevertheless we attempt to guess whether the script is in
        asian or western text based up on the first character and use the
        font size of that script as our default.
        */
        USHORT nScript;
        if( pBreakIt->xBreak.is() )
            nScript = pBreakIt->xBreak->getScriptType( pFld->GetPar1(), 0);
        else
            nScript = i18n::ScriptType::ASIAN;

        long nHeight = ((SvxFontHeightItem&)(GetExport().GetItem(
            GetWhichOfScript(RES_CHRATR_FONTSIZE,nScript)))).GetHeight();;

        nHeight = (nHeight + 10) / 20; //Font Size in points;

        /*
        Divide the combined char string into its up and down part. Get the
        font size and fill in the defaults as up == half the font size and
        down == a fifth the font size
        */
        xub_StrLen nAbove = (pFld->GetPar1().Len()+1)/2;
        sStr = FieldString(ww::eEQ);
        sStr.APPEND_CONST_ASC("\\o (\\s\\up ");
        sStr += String::CreateFromInt32(nHeight/2);

        sStr.Append('(');
        sStr += String(pFld->GetPar1(),0,nAbove);
        sStr.APPEND_CONST_ASC("), \\s\\do ");
        sStr += String::CreateFromInt32(nHeight/5);

        sStr.Append('(');
        sStr += String(pFld->GetPar1(),nAbove,pFld->GetPar1().Len()-nAbove);
        sStr.APPEND_CONST_ASC("))");
        GetExport().OutputField(pFld, ww::eEQ, sStr);
        }
        break;
    case RES_DROPDOWN:
        bWriteExpand = DropdownField( pFld );
        break;
    case RES_CHAPTERFLD:
        bWriteExpand = true;
        if (GetExport().bOutKF && rField.GetTxtFld())
        {
            const SwTxtNode *pTxtNd = GetExport().GetHdFtPageRoot();
            if (!pTxtNd)
            {
                if (const SwNode *pNd = GetExport().pCurPam->GetNode())
                    pTxtNd = pNd->GetTxtNode();
            }

            if (pTxtNd)
            {
                SwChapterField aCopy(*(const SwChapterField*)pFld);
                aCopy.ChangeExpansion(*pTxtNd, false);
                WriteExpand( &aCopy );
                bWriteExpand = false;
            }
        }
        break;
        case RES_HIDDENTXTFLD:
        {
            String sExpand(pFld->GetPar2());
            if (sExpand.Len())
            {
                HiddenField( *pFld );
            }
        }
        break;
    default:
        bWriteExpand = true;
        break;
    }

    if (bWriteExpand)
        WriteExpand( pFld );
}

void AttributeOutputBase::TextFlyContent( const SwFmtFlyCnt& rFlyContent )
{
    if ( GetExport().pOutFmtNode && GetExport().pOutFmtNode->ISA( SwCntntNode ) )
    {
        SwTxtNode* pTxtNd = (SwTxtNode*)GetExport().pOutFmtNode;

        Point aLayPos;
        aLayPos = pTxtNd->FindLayoutRect( false, &aLayPos ).Pos();

        SwPosition aPos( *pTxtNd );
        sw::Frame aFrm( *rFlyContent.GetFrmFmt(), aPos );

        OutputFlyFrame_Impl( aFrm, aLayPos );
    }
}

// TOXMarks fehlen noch

// Detaillierte Einstellungen zur Trennung erlaubt WW nur dokumentenweise.
// Man koennte folgende Mimik einbauen: Die Werte des Style "Standard" werden,
// falls vorhanden, in die Document Properties ( DOP ) gesetzt.
// ---
// ACK.  Dieser Vorschlag passt exakt zu unserer Implementierung des Import,
// daher setze ich das gleich mal um. (KHZ, 07/15/2000)
void WW8AttributeOutput::ParaHyphenZone( const SvxHyphenZoneItem& rHyphenZone )
{
    // sprmPFNoAutoHyph
    if( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFNoAutoHyph );
    else
        m_rWW8Export.pO->Insert( 44, m_rWW8Export.pO->Count() );

    m_rWW8Export.pO->Insert( rHyphenZone.IsHyphen() ? 0 : 1, m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::ParaScriptSpace( const SfxBoolItem& rScriptSpace )
{
    USHORT nId = 0;
    if ( m_rWW8Export.bWrtWW8 )
        switch ( rScriptSpace.Which() )
        {
            case RES_PARATR_SCRIPTSPACE:            nId = NS_sprm::LN_PFAutoSpaceDE;   break;
            case RES_PARATR_HANGINGPUNCTUATION:     nId = NS_sprm::LN_PFOverflowPunct;   break;
            case RES_PARATR_FORBIDDEN_RULES:        nId = NS_sprm::LN_PFKinsoku;   break;
        }

    if ( nId )
    {
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( nId );
        else
            m_rWW8Export.pO->Insert( (BYTE)nId, m_rWW8Export.pO->Count() );

        m_rWW8Export.pO->Insert( rScriptSpace.GetValue() ? 1 : 0,
                            m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::ParaSnapToGrid( const SvxParaGridItem& rGrid )
{
    // sprmPFUsePgsuSettings
    // 97+ only
    if ( !m_rWW8Export.bWrtWW8 )
        return;

    m_rWW8Export.InsUInt16( NS_sprm::LN_PFUsePgsuSettings );
    m_rWW8Export.pO->Insert( rGrid.GetValue(), m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::ParaVerticalAlign( const SvxParaVertAlignItem& rAlign )
{
    // sprmPWAlignFont
    // 97+ only
    if( !m_rWW8Export.bWrtWW8 )
        return;

    m_rWW8Export.InsUInt16( NS_sprm::LN_PWAlignFont );

    INT16 nVal = rAlign.GetValue();
    switch ( nVal )
    {
        case SvxParaVertAlignItem::BASELINE:
            nVal = 2;
            break;
        case SvxParaVertAlignItem::TOP:
            nVal = 0;
            break;
        case SvxParaVertAlignItem::CENTER:
            nVal = 1;
            break;
        case SvxParaVertAlignItem::BOTTOM:
            nVal = 3;
            break;
        case SvxParaVertAlignItem::AUTOMATIC:
            nVal = 4;
            break;
        default:
            nVal = 4;
            ASSERT( false, "Unknown vert alignment" );
            break;
    }
    m_rWW8Export.InsUInt16( nVal );
}


// NoHyphen: ich habe keine Entsprechung in der SW-UI und WW-UI gefunden

void WW8AttributeOutput::TextHardBlank( const SwFmtHardBlank& rHardBlank )
{
    m_rWW8Export.WriteChar( rHardBlank.GetChar() );
}

// RefMark, NoLineBreakHere  fehlen noch

void WW8Export::WriteFtnBegin( const SwFmtFtn& rFtn, WW8Bytes* pOutArr )
{
    WW8Bytes aAttrArr;
    bool bAutoNum = !rFtn.GetNumStr().Len();    // Auto-Nummer
    if( bAutoNum )
    {
        if( bWrtWW8 )
        {
            static const BYTE aSpec[] =
            {
                0x03, 0x6a, 0, 0, 0, 0, // sprmCObjLocation
                0x55, 0x08, 1           // sprmCFSpec
            };

            aAttrArr.Insert(aSpec, sizeof(aSpec), aAttrArr.Count());
        }
        else
        {
            static BYTE const aSpec[] =
            {
                117, 1,                         // sprmCFSpec
                68, 4, 0, 0, 0, 0               // sprmCObjLocation
            };

            aAttrArr.Insert(aSpec, sizeof(aSpec), aAttrArr.Count());
        }
    }

    // sprmCIstd
    const SwEndNoteInfo* pInfo;
    if( rFtn.IsEndNote() )
        pInfo = &pDoc->GetEndNoteInfo();
    else
        pInfo = &pDoc->GetFtnInfo();
    const SwCharFmt* pCFmt = pOutArr
                                ? pInfo->GetAnchorCharFmt( *pDoc )
                                : pInfo->GetCharFmt( *pDoc );
    if( bWrtWW8 )
        SwWW8Writer::InsUInt16( aAttrArr, NS_sprm::LN_CIstd );
    else
        aAttrArr.Insert( 80, aAttrArr.Count() );
    SwWW8Writer::InsUInt16( aAttrArr, GetId( *pCFmt ) );

                                                // fSpec-Attribut true
                            // Fuer Auto-Nummer muss ein Spezial-Zeichen
                            // in den Text und darum ein fSpec-Attribut
    pChpPlc->AppendFkpEntry( Strm().Tell() );
    if( bAutoNum )
        WriteChar( 0x02 );              // Auto-Nummer-Zeichen
    else
        // User-Nummerierung
        OutSwString( rFtn.GetNumStr(), 0, rFtn.GetNumStr().Len(),
                        IsUnicode(), RTL_TEXTENCODING_MS_1252 );

    if( pOutArr )
    {
        // insert at start of array, so the "hard" attribute overrule the
        // attributes of the character template
        pOutArr->Insert( &aAttrArr, 0 );
    }
    else
    {
        WW8Bytes aOutArr;

        // insert at start of array, so the "hard" attribute overrule the
        // attributes of the character template
        aOutArr.Insert( &aAttrArr, 0 );

        // write for the ftn number in the content, the font of the anchor
        const SwTxtFtn* pTxtFtn = rFtn.GetTxtFtn();
        if( pTxtFtn )
        {
            WW8Bytes* pOld = pO;
            pO = &aOutArr;
            SfxItemSet aSet( pDoc->GetAttrPool(), RES_CHRATR_FONT,
                                                  RES_CHRATR_FONT );

            pCFmt = pInfo->GetCharFmt( *pDoc );
            aSet.Set( pCFmt->GetAttrSet() );

            pTxtFtn->GetTxtNode().GetAttr( aSet, *pTxtFtn->GetStart(),
                                            (*pTxtFtn->GetStart()) + 1 );
            m_pAttrOutput->OutputItem( aSet.Get( RES_CHRATR_FONT ) );
            pO = pOld;
        }
        pChpPlc->AppendFkpEntry( Strm().Tell(), aOutArr.Count(),
                                                aOutArr.GetData() );
    }
}

static bool lcl_IsAtTxtEnd(const SwFmtFtn& rFtn)
{
    bool bRet = true;
    if( rFtn.GetTxtFtn() )
    {
        USHORT nWh = static_cast< USHORT >(rFtn.IsEndNote() ? RES_END_AT_TXTEND
                                      : RES_FTN_AT_TXTEND);
        const SwSectionNode* pSectNd = rFtn.GetTxtFtn()->GetTxtNode().
                                                        FindSectionNode();
        while( pSectNd && FTNEND_ATPGORDOCEND ==
                ((const SwFmtFtnAtTxtEnd&)pSectNd->GetSection().GetFmt()->
                GetFmtAttr( nWh, true)).GetValue() )
            pSectNd = pSectNd->StartOfSectionNode()->FindSectionNode();

        if (!pSectNd)
            bRet = false;   // the is ftn/end collected at Page- or Doc-End
    }
    return bRet;
}


void AttributeOutputBase::TextFootnote( const SwFmtFtn& rFtn )
{
    USHORT nTyp;
    if ( rFtn.IsEndNote() )
    {
        nTyp = REF_ENDNOTE;
        if ( GetExport().bEndAtTxtEnd )
            GetExport().bEndAtTxtEnd = lcl_IsAtTxtEnd( rFtn );
    }
    else
    {
        nTyp = REF_FOOTNOTE;
        if ( GetExport().bFtnAtTxtEnd )
            GetExport().bFtnAtTxtEnd = lcl_IsAtTxtEnd( rFtn );
    }

    // if any reference to this footnote/endnote then insert an internal
    // Bookmark.
    String sBkmkNm;
    if ( GetExport().HasRefToObject( nTyp, 0, rFtn.GetTxtFtn()->GetSeqRefNo() ))
    {
        sBkmkNm = GetExport().GetBookmarkName( nTyp, 0,
                                    rFtn.GetTxtFtn()->GetSeqRefNo() );
        GetExport().AppendBookmark( sBkmkNm );
    }

    TextFootnote_Impl( rFtn );

    if ( sBkmkNm.Len() )
        GetExport().AppendBookmark( sBkmkNm ); // FIXME: Why is it added twice?  Shouldn't this one go to WW8AttributeOuput::TextFootnote_Impl()?
}

void WW8AttributeOutput::TextFootnote_Impl( const SwFmtFtn& rFtn )
{
    WW8_WrPlcFtnEdn* pFtnEnd;
    if ( rFtn.IsEndNote() )
        pFtnEnd = m_rWW8Export.pEdn;
    else
        pFtnEnd = m_rWW8Export.pFtn;

    pFtnEnd->Append( m_rWW8Export.Fc2Cp( m_rWW8Export.Strm().Tell() ), rFtn );
    m_rWW8Export.WriteFtnBegin( rFtn, m_rWW8Export.pO );
}

void WW8AttributeOutput::TextCharFormat( const SwFmtCharFmt& rCharFmt )
{
    if( rCharFmt.GetCharFmt() )
    {
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_CIstd );
        else
            m_rWW8Export.pO->Insert( 80, m_rWW8Export.pO->Count() );

        m_rWW8Export.InsUInt16( m_rWW8Export.GetId( *rCharFmt.GetCharFmt() ) );
    }
}

/*
 See ww8par6.cxx Read_DoubleLine for some more info
 */
void WW8AttributeOutput::CharTwoLines( const SvxTwoLinesItem& rTwoLines )
{
    // #i28331# - check that bOn is set
    if ( rTwoLines.GetValue() )
    {
        //97+ only
        if( !m_rWW8Export.bWrtWW8 )
            return;

        m_rWW8Export.InsUInt16( NS_sprm::LN_CEastAsianLayout );
        m_rWW8Export.pO->Insert( (BYTE)0x06, m_rWW8Export.pO->Count() ); //len 6
        m_rWW8Export.pO->Insert( (BYTE)0x02, m_rWW8Export.pO->Count() );

        sal_Unicode cStart = rTwoLines.GetStartBracket();
        sal_Unicode cEnd = rTwoLines.GetStartBracket();

        /*
        As per usual we have problems. We can have seperate left and right brackets
        in OOo, it doesn't appear that you can in word. Also in word there appear
        to only be a limited number of possibilities, we can use pretty much
        anything.

        So if we have none, we export none, if either bracket is set to a known
        word type we export both as that type (with the bracket winning out in
        the case of a conflict simply being the order of test here.

        Upshot being a documented created in word will be reexported with no
        ill effects.
        */

        USHORT nType;
        if (!cStart && !cEnd)
            nType = 0;
        else if ((cStart == '{') || (cEnd == '}'))
            nType = 4;
        else if ((cStart == '<') || (cEnd == '>'))
            nType = 3;
        else if ((cStart == '[') || (cEnd == ']'))
            nType = 2;
        else
            nType = 1;
        m_rWW8Export.InsUInt16( nType );
        static const BYTE aZeroArr[ 3 ] = { 0, 0, 0 };
        m_rWW8Export.pO->Insert( aZeroArr, 3, m_rWW8Export.pO->Count() );
    }
}

void AttributeOutputBase::ParaNumRule( const SwNumRuleItem& rNumRule )
{
    const SwTxtNode* pTxtNd = 0;
    USHORT nNumId;
    BYTE nLvl = 0;
    if ( rNumRule.GetValue().Len() )
    {
        const SwNumRule* pRule = GetExport().pDoc->FindNumRulePtr(
                                        rNumRule.GetValue() );
        if ( pRule && USHRT_MAX != ( nNumId = GetExport().GetId( *pRule ) ) )
        {
            ++nNumId;
            if ( GetExport().pOutFmtNode )
            {
                if ( GetExport().pOutFmtNode->ISA( SwCntntNode ) )
                {
                    pTxtNd = (SwTxtNode*)GetExport().pOutFmtNode;

                    if( pTxtNd->IsCountedInList())
                    {
                        nLvl = static_cast< BYTE >(pTxtNd->GetActualListLevel());

                        if ( pTxtNd->IsListRestart() )
                        {
                            USHORT nStartWith = static_cast< USHORT >( pTxtNd->GetActualListStartValue() );
                            nNumId = GetExport().DuplicateNumRule( pRule, nLvl, nStartWith );
                            if ( USHRT_MAX != nNumId )
                                ++nNumId;
                        }
                    }
                    else
                    {
                        // #i44815# adjust numbering for numbered paragraphs
                        // without number (NO_NUMLEVEL). These paragaphs
                        // will receive a list id 0, which WW interprets as
                        // 'no number'.
                        nNumId = 0;
                    }
                }
                else if ( GetExport().pOutFmtNode->ISA( SwTxtFmtColl ) )
                {
                    const SwTxtFmtColl* pC = (SwTxtFmtColl*)GetExport().pOutFmtNode;
                    if ( pC && pC->IsAssignedToListLevelOfOutlineStyle() )
                        nLvl = static_cast< BYTE >( pC->GetAssignedOutlineStyleLevel() ); //<-end,zhaojianwei
                }
            }
        }
        else
            nNumId = USHRT_MAX;
    }
    else
        nNumId = 0;

    if ( USHRT_MAX != nNumId )
    {
        if ( nLvl >= WW8ListManager::nMaxLevel )
            nLvl = WW8ListManager::nMaxLevel - 1;

        ParaNumRule_Impl( pTxtNd, nLvl, nNumId );
    }
}

void WW8AttributeOutput::ParaNumRule_Impl( const SwTxtNode* pTxtNd, sal_Int32 nLvl, sal_Int32 nNumId )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        // write sprmPIlvl and sprmPIlfo
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_PIlvl );
        m_rWW8Export.pO->Insert( ::sal::static_int_cast<BYTE>(nLvl), m_rWW8Export.pO->Count() );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_PIlfo );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, ::sal::static_int_cast<UINT16>(nNumId) );
    }
    else if ( pTxtNd && m_rWW8Export.Out_SwNum( pTxtNd ) )                // NumRules
        m_rWW8Export.pSepx->SetNum( pTxtNd );
}

/* File FRMATR.HXX */

void WW8AttributeOutput::FormatFrameSize( const SwFmtFrmSize& rSize )
{
    if( m_rWW8Export.bOutFlyFrmAttrs )                   // Flys
    {
        if( m_rWW8Export.bOutGrf )
            return;                // Fly um Grafik -> Auto-Groesse

//???? was ist bei Prozentangaben ???
        if ( rSize.GetWidth() && rSize.GetWidthSizeType() == ATT_FIX_SIZE)
        {
            //"sprmPDxaWidth"
            if( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_PDxaWidth );
            else
                m_rWW8Export.pO->Insert( 28, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( (USHORT)rSize.GetWidth() );
        }

        if ( rSize.GetHeight() )
        {
            // sprmPWHeightAbs
            if( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_PWHeightAbs );
            else
                m_rWW8Export.pO->Insert( 45, m_rWW8Export.pO->Count() );

            USHORT nH = 0;
            switch ( rSize.GetHeightSizeType() )
            {
                case ATT_VAR_SIZE: break;
                case ATT_FIX_SIZE: nH = (USHORT)rSize.GetHeight() & 0x7fff; break;
                default:           nH = (USHORT)rSize.GetHeight() | 0x8000; break;
            }
            m_rWW8Export.InsUInt16( nH );
        }
    }
    else if( m_rWW8Export.bOutPageDescs )            // PageDesc : Breite + Hoehe
    {
        if( m_rWW8Export.pAktPageDesc->GetLandscape() )
        {
            /*sprmSBOrientation*/
            if( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_SBOrientation );
            else
                m_rWW8Export.pO->Insert( 162, m_rWW8Export.pO->Count() );
            m_rWW8Export.pO->Insert( 2, m_rWW8Export.pO->Count() );
        }

        /*sprmSXaPage*/
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SXaPage );
        else
            m_rWW8Export.pO->Insert( 164, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16(
            msword_cast<sal_uInt16>(SvxPaperInfo::GetSloppyPaperDimension(rSize.GetWidth())));

        /*sprmSYaPage*/
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SYaPage );
        else
            m_rWW8Export.pO->Insert( 165, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16(
            msword_cast<sal_uInt16>(SvxPaperInfo::GetSloppyPaperDimension(rSize.GetHeight())));
    }
}

// FillOrder fehlt noch

// ReplaceCr() wird fuer Pagebreaks und Pagedescs gebraucht. Es wird ein
// bereits geschriebenes CR durch ein Break-Zeichen ersetzt. Replace muss
// direkt nach Schreiben des CR gerufen werden.
// Rueckgabe: FilePos des ersetzten CRs + 1 oder 0 fuer nicht ersetzt

ULONG WW8Export::ReplaceCr( BYTE nChar )
{
    // Bug #49917#
    ASSERT( nChar, "gegen 0 ersetzt bringt WW97/95 zum Absturz" );

    bool bReplaced = false;
    SvStream& rStrm = Strm();
    ULONG nRetPos = 0, nPos = rStrm.Tell();
    BYTE nBCode=0;
    UINT16 nUCode=0;
    //If there is at least two characters already output
    if (nPos - (IsUnicode() ? 2 : 1) >= ULONG(pFib->fcMin))
    {
        rStrm.SeekRel(IsUnicode() ? -2 : -1);
        if (IsUnicode())
            rStrm >> nUCode;
        else
        {
            rStrm >> nBCode;
            nUCode = nBCode;
        }
        //If the last char was a cr
        if (nUCode == 0x0d)             // CR ?
        {
            if ((nChar == 0x0c) &&
                (nPos - (IsUnicode() ? 4 : 2) >= ULONG(pFib->fcMin)))
            {
                rStrm.SeekRel( IsUnicode() ? -4 : -2 );
                if (IsUnicode())
                    rStrm >> nUCode;
                else
                {
                    rStrm >> nUCode;
                    nUCode = nBCode;
                }
            }
            else
            {
                rStrm.SeekRel( IsUnicode() ? -2 : -1 );
                nUCode = 0x0;
            }
            //And the para is not of len 0, then replace this cr with the mark
            if( nChar == 0x0e || nUCode == 0x0d )
                bReplaced = false;
            else
            {
                bReplaced = true;
                WriteChar(nChar);
                nRetPos = nPos;
            }
        }
        else if ((nUCode == 0x0c) && (nChar == 0x0e))
        {
            //#108854# a column break after a section has
            //no effect in writer
            bReplaced = true;
        }
        rStrm.Seek( nPos );
    }
    else
        bReplaced = true;

    if (!bReplaced)
    {
        // then write as normal char
        WriteChar(nChar);
        pPiece->SetParaBreak();
        pPapPlc->AppendFkpEntry(rStrm.Tell());
        pChpPlc->AppendFkpEntry(rStrm.Tell());
        nRetPos = rStrm.Tell();
    }
#ifdef PRODUCT
    else
    {
        ASSERT( nRetPos || nPos == (ULONG)pFib->fcMin,
                "WW8_ReplaceCr an falscher FilePos gerufen" );
    }
#endif
    return nRetPos;
}

void WW8AttributeOutput::TableRowEnd(sal_uInt32 nDepth)
{
    if ( nDepth == 1 )
        m_rWW8Export.WriteChar( (BYTE)0x07 );
    else if ( nDepth > 1 )
        m_rWW8Export.WriteChar( (BYTE)0x0d );

    //Technically in a word document this is a different value for a row ends
    //that are not row ends directly after a cell with a graphic. But it
    //doesn't seem to make a difference
    //pMagicTable->Append(Fc2Cp(Strm().Tell()),0x1B6);
}

void AttributeOutputBase::FormatPageDescription( const SwFmtPageDesc& rPageDesc )
{
    if ( GetExport().bStyDef && GetExport().pOutFmtNode && GetExport().pOutFmtNode->ISA( SwTxtFmtColl ) )
    {
        const SwTxtFmtColl* pC = (SwTxtFmtColl*)GetExport().pOutFmtNode;
        if ( (SFX_ITEM_SET != pC->GetItemState( RES_BREAK, false ) ) && rPageDesc.GetRegisteredIn() )
            FormatBreak( SvxFmtBreakItem( SVX_BREAK_PAGE_BEFORE, RES_BREAK ) );
    }
}

void WW8AttributeOutput::PageBreakBefore( bool bBreak )
{
    // sprmPPageBreakBefore/sprmPFPageBreakBefore
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFPageBreakBefore );
    else
        m_rWW8Export.pO->Insert( 9, m_rWW8Export.pO->Count() );

    m_rWW8Export.pO->Insert( bBreak ? 1 : 0, m_rWW8Export.pO->Count() );
}

// Breaks schreiben nichts in das Ausgabe-Feld rWrt.pO,
// sondern nur in den Text-Stream ( Bedingung dafuer, dass sie von Out_Break...
// gerufen werden duerfen )
void AttributeOutputBase::FormatBreak( const SvxFmtBreakItem& rBreak )
{
    if ( GetExport().bStyDef )
    {
        switch ( rBreak.GetBreak() )
        {
            // JP 21.06.99: column breaks never change to pagebreaks
            // case SVX_BREAK_COLUMN_BEFORE:
            // case SVX_BREAK_COLUMN_BOTH:
            case SVX_BREAK_NONE:
            case SVX_BREAK_PAGE_BEFORE:
            case SVX_BREAK_PAGE_BOTH:
                PageBreakBefore( rBreak.GetValue() );
                break;
            default:
                break;
        }
    }
    else if ( !GetExport().mpParentFrame )
    {
        BYTE nC = 0;
        bool bBefore = false;
        // --> OD 2007-05-29 #i76300#
        // Note: Can only be <true>, if <bBefore> equals <false>.
        bool bCheckForFollowPageDesc = false;
        // <--

        switch ( rBreak.GetBreak() )
        {
            case SVX_BREAK_NONE:                                // Ausgeschaltet
                if ( !GetExport().bBreakBefore )
                    PageBreakBefore( false );
                return;

            case SVX_BREAK_COLUMN_BEFORE:                       // ColumnBreak
                bBefore = true;
                // no break;
            case SVX_BREAK_COLUMN_AFTER:
            case SVX_BREAK_COLUMN_BOTH:
                if ( GetExport().Sections().CurrentNumberOfColumns( *GetExport().pDoc ) > 1 )
                {
                    nC = msword::ColumnBreak;
                }
                break;

            case SVX_BREAK_PAGE_BEFORE:                         // PageBreak
                // From now on(fix for #i77900#) we prefer to save a page break as
                // paragraph attribute, this has to be done after the export of the
                // paragraph ( => !GetExport().bBreakBefore )
                if ( !GetExport().bBreakBefore )
                    PageBreakBefore( true );
                break;

            case SVX_BREAK_PAGE_AFTER:
            case SVX_BREAK_PAGE_BOTH:
                nC = msword::PageBreak;
                // --> OD 2007-05-29 #i76300#
                // check for follow page description, if current writing attributes
                // of a paragraph.
                if ( dynamic_cast< const SwTxtNode* >( GetExport().pOutFmtNode ) &&
                     GetExport().GetCurItemSet() )
                {
                    bCheckForFollowPageDesc = true;
                }
                // <--
                break;

            default:
                break;
        }

        if ( ( bBefore == GetExport().bBreakBefore ) && nC )  // #49917#
        {
            // --> OD 2007-05-29 #i76300#
            bool bFollowPageDescWritten = false;
            if ( bCheckForFollowPageDesc && !bBefore )
            {
                bFollowPageDescWritten =
                    GetExport().OutputFollowPageDesc( GetExport().GetCurItemSet(),
                            dynamic_cast<const SwTxtNode*>( GetExport().pOutFmtNode ) );
            }
            if ( !bFollowPageDescWritten )
            {
                SectionBreak( nC );
            }
            // <--
        }
    }
}

void WW8AttributeOutput::SectionBreak( BYTE nC, const WW8_SepInfo* /*pSectionInfo*/ )
{
    m_rWW8Export.ReplaceCr( nC );
}

void WW8AttributeOutput::FormatTextGrid( const SwTextGridItem& rGrid )
{
    if ( m_rWW8Export.bOutPageDescs && m_rWW8Export.bWrtWW8 )
    {
        UINT16 nGridType = 0;
        switch ( rGrid.GetGridType() )
        {
            default:
                ASSERT(false, "Unknown grid type");
            case GRID_NONE:
                nGridType = 0;
                break;
            case GRID_LINES_ONLY:
                nGridType = 2;
                break;
            case GRID_LINES_CHARS:
                if ( rGrid.IsSnapToChars() )
                    nGridType = 3;
                else
                    nGridType = 1; 
                break;
        }
        m_rWW8Export.InsUInt16( NS_sprm::LN_SClm );
        m_rWW8Export.InsUInt16( nGridType );

        UINT16 nHeight = rGrid.GetBaseHeight() + rGrid.GetRubyHeight();
        m_rWW8Export.InsUInt16( NS_sprm::LN_SDyaLinePitch );
        m_rWW8Export.InsUInt16( nHeight );
        sal_uInt32 nPageCharSize = ItemGet<SvxFontHeightItem>(*(m_rWW8Export.pStyles->GetSwFmt()),
                RES_CHRATR_CJK_FONTSIZE).GetHeight();

        INT32 nCharWidth = rGrid.GetBaseWidth() - nPageCharSize;
        INT32 nFraction = 0;
        nFraction = nCharWidth % 20;
        if ( nCharWidth < 0 )
            nFraction = 20 + nFraction;
        nFraction = ( nFraction * 0xFFF ) / 20;
        nFraction = ( nFraction & 0x00000FFF );

        INT32 nMain = 0;
        nMain = nCharWidth / 20;
        if ( nCharWidth < 0 )
            nMain -= 1;
        nMain = nMain * 0x1000;
        nMain = ( nMain & 0xFFFFF000 );

        UINT32 nCharSpace = nFraction + nMain;
        m_rWW8Export.InsUInt16( NS_sprm::LN_SDxtCharSpace );
        m_rWW8Export.InsUInt32( nCharSpace );
    }
}

void WW8AttributeOutput::FormatPaperBin( const SvxPaperBinItem& rPaperBin )
{
    if ( m_rWW8Export.bOutPageDescs )
    {
        USHORT nVal;
        switch ( rPaperBin.GetValue() )
        {
            case 0: nVal = 15;  break;      // Automatically select
            case 1: nVal = 1;   break;      // Upper paper tray
            case 2: nVal = 4;   break;      // Manual paper feed
            default: nVal = 0;  break;
        }

        if ( nVal )
        {
            if( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( m_rWW8Export.bOutFirstPage? NS_sprm::LN_SDmBinFirst: NS_sprm::LN_SDmBinOther );
            else
                m_rWW8Export.pO->Insert( m_rWW8Export.bOutFirstPage? 140: 141, m_rWW8Export.pO->Count() );

            m_rWW8Export.InsUInt16( nVal );
        }
    }
}

void WW8AttributeOutput::FormatLRSpace( const SvxLRSpaceItem& rLR )
{
    // Flys fehlen noch ( siehe RTF )

    if ( m_rWW8Export.bOutFlyFrmAttrs )                   // Flys
    {
        // sprmPDxaFromText10
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDxaFromText10 );
        else
            m_rWW8Export.pO->Insert( 49, m_rWW8Export.pO->Count() );
        // Mittelwert nehmen, da WW nur 1 Wert kennt
        m_rWW8Export.InsUInt16( (USHORT) ( ( rLR.GetLeft() + rLR.GetRight() ) / 2 ) );
    }
    else if ( m_rWW8Export.bOutPageDescs )                // PageDescs
    {
        USHORT nLDist, nRDist;
        const SfxPoolItem* pItem = m_rWW8Export.HasItem( RES_BOX );
        if ( pItem )
        {
            nRDist = ((SvxBoxItem*)pItem)->CalcLineSpace( BOX_LINE_LEFT );
            nLDist = ((SvxBoxItem*)pItem)->CalcLineSpace( BOX_LINE_RIGHT );
        }
        else
            nLDist = nRDist = 0;
        nLDist = nLDist + (USHORT)rLR.GetLeft();
        nRDist = nRDist + (USHORT)rLR.GetRight();

        // sprmSDxaLeft
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SDxaLeft );
        else
            m_rWW8Export.pO->Insert( 166, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( nLDist );

        // sprmSDxaRight
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SDxaRight );
        else
            m_rWW8Export.pO->Insert( 167, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( nRDist );
    }
    else
    {                                          // normale Absaetze
        // sprmPDxaLeft
        if( m_rWW8Export.bWrtWW8 )
        {
            m_rWW8Export.InsUInt16( 0x845E );        //asian version ?
            m_rWW8Export.InsUInt16( (USHORT)rLR.GetTxtLeft() );
        }
        else
        {
            m_rWW8Export.pO->Insert( 17, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( (USHORT)rLR.GetTxtLeft() );
        }

        // sprmPDxaRight
        if( m_rWW8Export.bWrtWW8 )
        {
            m_rWW8Export.InsUInt16( 0x845D );        //asian version ?
            m_rWW8Export.InsUInt16( (USHORT)rLR.GetRight() );
        }
        else
        {
            m_rWW8Export.pO->Insert( 16, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( (USHORT)rLR.GetRight() );
        }

        // sprmPDxaLeft1
        if( m_rWW8Export.bWrtWW8 )
        {
            m_rWW8Export.InsUInt16( 0x8460 );        //asian version ?
            m_rWW8Export.InsUInt16( rLR.GetTxtFirstLineOfst() );
        }
        else
        {
            m_rWW8Export.pO->Insert( 19, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( rLR.GetTxtFirstLineOfst() );
        }
    }
}

void WW8AttributeOutput::FormatULSpace( const SvxULSpaceItem& rUL )
{
    // Flys fehlen noch ( siehe RTF )

    if ( m_rWW8Export.bOutFlyFrmAttrs )                   // Flys
    {
        // sprmPDyaFromText
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDyaFromText );
        else
            m_rWW8Export.pO->Insert( 48, m_rWW8Export.pO->Count() );
        // Mittelwert nehmen, da WW nur 1 Wert kennt
        m_rWW8Export.InsUInt16( (USHORT) ( ( rUL.GetUpper() + rUL.GetLower() ) / 2 ) );
    }
    else if ( m_rWW8Export.bOutPageDescs )            // Page-UL
    {
        ASSERT( m_rWW8Export.GetCurItemSet(), "Impossible" );
        if ( !m_rWW8Export.GetCurItemSet() )
            return;

        HdFtDistanceGlue aDistances( *m_rWW8Export.GetCurItemSet() );

        if ( aDistances.HasHeader() )
        {
            //sprmSDyaHdrTop
            if ( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_SDyaHdrTop );
            else
                m_rWW8Export.pO->Insert( 156, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( aDistances.dyaHdrTop );
        }

        // sprmSDyaTop
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SDyaTop );
        else
            m_rWW8Export.pO->Insert( 168, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( aDistances.dyaTop );

        if ( aDistances.HasFooter() )
        {
            //sprmSDyaHdrBottom
            if ( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_SDyaHdrBottom );
            else
                m_rWW8Export.pO->Insert( 157, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt16( aDistances.dyaHdrBottom );
        }

        //sprmSDyaBottom
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_SDyaBottom );
        else
            m_rWW8Export.pO->Insert( 169, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( aDistances.dyaBottom );
    }
    else
    {
        // sprmPDyaBefore
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDyaBefore );
        else
            m_rWW8Export.pO->Insert( 21, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( rUL.GetUpper() );
        // sprmPDyaAfter
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDyaAfter );
        else
            m_rWW8Export.pO->Insert( 22, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( rUL.GetLower() );
    }
}

// Print, Opaque, Protect fehlen noch

void WW8AttributeOutput::FormatSurround( const SwFmtSurround& rSurround )
{
    if ( m_rWW8Export.bOutFlyFrmAttrs )
    {
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PWr );
        else
            m_rWW8Export.pO->Insert( 37, m_rWW8Export.pO->Count() );

        m_rWW8Export.pO->Insert(
                ( SURROUND_NONE != rSurround.GetSurround() ) ? 2 : 1,
                m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::FormatVertOrientation( const SwFmtVertOrient& rFlyVert )
{
//!!!! Ankertyp und entsprechende Umrechnung fehlt noch

    if ( m_rWW8Export.bOutFlyFrmAttrs )
    {
        short nPos;
        switch( rFlyVert.GetVertOrient() )
        {
            case text::VertOrientation::NONE:
                nPos = (short)rFlyVert.GetPos();
                break;
            case text::VertOrientation::CENTER:
            case text::VertOrientation::LINE_CENTER:
                nPos = -8;
                break;
            case text::VertOrientation::BOTTOM:
            case text::VertOrientation::LINE_BOTTOM:
                nPos = -12;
                break;
            case text::VertOrientation::TOP:
            case text::VertOrientation::LINE_TOP:
            default:
                nPos = -4;
                break;
        }

        // sprmPDyaAbs
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDyaAbs );
        else
            m_rWW8Export.pO->Insert( 27, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( nPos );
    }
}


void WW8AttributeOutput::FormatHorizOrientation( const SwFmtHoriOrient& rFlyHori )
{
    if ( !m_rWW8Export.mpParentFrame )
    {
        ASSERT( m_rWW8Export.mpParentFrame, "HoriOrient without mpParentFrame !!" );
        return;
    }

//!!!! Ankertyp und entsprechende Umrechnung fehlt noch
    if ( m_rWW8Export.bOutFlyFrmAttrs )
    {
        short nPos;
        switch( rFlyHori.GetHoriOrient() )
        {
            case text::HoriOrientation::NONE:
                nPos = (short)rFlyHori.GetPos();
                if( !nPos )
                    nPos = 1;   // WW: 0 ist reserviert
                break;
            case text::HoriOrientation::LEFT:
                nPos = rFlyHori.IsPosToggle() ? -12 : 0;
                break;
            case text::HoriOrientation::RIGHT:
                nPos = rFlyHori.IsPosToggle() ? -16 : -8;
                break;
            case text::HoriOrientation::CENTER:
            case text::HoriOrientation::FULL:                         // FULL nur fuer Tabellen
            default:
                nPos = -4;
                break;
        }

        // sprmPDxaAbs
        if( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PDxaAbs );
        else
            m_rWW8Export.pO->Insert( 26, m_rWW8Export.pO->Count() );
        m_rWW8Export.InsUInt16( nPos );
    }
}

void WW8AttributeOutput::FormatAnchor( const SwFmtAnchor& rAnchor )
{
    ASSERT( m_rWW8Export.mpParentFrame, "Anchor without mpParentFrame !!" );

    if ( m_rWW8Export.bOutFlyFrmAttrs )
    {
        BYTE nP = 0;
        switch ( rAnchor.GetAnchorId() )
        {
            case FLY_PAGE:
                // Vert: Page | Horz: Page
                nP |= (1 << 4) | (2 << 6);
                break;
            // Im Fall eine Flys als Zeichen: Absatz-gebunden setzen!!!
            case FLY_AT_FLY:
            case FLY_AUTO_CNTNT:
            case FLY_AT_CNTNT:
            case FLY_IN_CNTNT:
                // Vert: Page | Horz: Page
                nP |= (2 << 4) | (0 << 6);
                break;
            default:
                break;
        }

        // sprmPPc
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PPc );
        else
            m_rWW8Export.pO->Insert( 29, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( nP, m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::FormatBackground( const SvxBrushItem& rBrush )
{
    // WW cannot have background in a section
    if ( !m_rWW8Export.bOutPageDescs )
    {
        WW8_SHD aSHD;

        m_rWW8Export.TransBrush( rBrush.GetColor(), aSHD );
        // sprmPShd
        if ( m_rWW8Export.bWrtWW8 )
            m_rWW8Export.InsUInt16( NS_sprm::LN_PShd );
        else
            m_rWW8Export.pO->Insert(47, m_rWW8Export.pO->Count());
        m_rWW8Export.InsUInt16( aSHD.GetValue() );

        // Quite a few unknowns, some might be transparency or something
        // of that nature...
        if ( m_rWW8Export.bWrtWW8 )
        {
            m_rWW8Export.InsUInt16( 0xC64D );
            m_rWW8Export.pO->Insert( 10, m_rWW8Export.pO->Count() );
            m_rWW8Export.InsUInt32( 0xFF000000 );
            m_rWW8Export.InsUInt32( SuitableBGColor( rBrush.GetColor().GetColor() ) );
            m_rWW8Export.InsUInt16( 0x0000 );
        }
    }
}

WW8_BRC WW8Export::TranslateBorderLine(const SvxBorderLine& rLine,
    USHORT nDist, bool bShadow)
{
    // M.M. This function writes out border lines to the word format similar to
    // what SwRTFWriter::OutRTFBorder does in the RTF filter Eventually it
    // would be nice if all this functionality was in the one place
    WW8_BRC aBrc;
    UINT16 nWidth = rLine.GetInWidth() + rLine.GetOutWidth();
    BYTE brcType = 0, nColCode = 0;

    if( nWidth )                                // Linie ?
    {
        // BRC.brcType
        bool bDouble = 0 != rLine.GetInWidth() && 0 != rLine.GetOutWidth();
        bool bThick = !bDouble && !bWrtWW8 && nWidth > 75;
        if( bDouble )
            brcType = 3;
        else if( bThick )
            brcType = 2;
        else
            brcType = 1;

        // BRC.dxpLineWidth
        if( bThick )
            nWidth /= 2;

        if( bWrtWW8 )
        {
            // Angabe in 8tel Punkten, also durch 2.5, da 1 Punkt = 20 Twips
            nWidth = (( nWidth * 8 ) + 10 ) / 20;
            if( 0xff < nWidth )
                nWidth = 0xff;
        }
        else
        {
            // Angabe in 0.75 pt
            nWidth = ( nWidth + 7 ) / 15;
            if( nWidth > 5 )
                nWidth = 5;
        }

        if( 0 == nWidth )                       // ganz duenne Linie
            nWidth = 1;                         //       nicht weglassen

        // BRC.ico
        nColCode = TransCol( rLine.GetColor() );
    }

    // BRC.dxpSpace
    USHORT nLDist = nDist;
    nLDist /= 20;               // Masseinheit : pt
    if( nLDist > 0x1f )
        nLDist = 0x1f;

    if( bWrtWW8 )
    {
        aBrc.aBits1[0] = BYTE(nWidth);
        aBrc.aBits1[1] = brcType;
        aBrc.aBits2[0] = nColCode;
        aBrc.aBits2[1] = BYTE(nLDist);

        // fShadow, keine weiteren Einstellungen im WW moeglich
        if( bShadow )
            aBrc.aBits2[1] |= 0x20;
    }
    else
    {
        USHORT aBits = nWidth + ( brcType << 3 );
        aBits |= (nColCode & 0x1f) << 6;
        aBits |= nLDist << 11;
        // fShadow, keine weiteren Einstellungen im WW moeglich
        if( bShadow )
            aBits |= 0x20;
        ShortToSVBT16( aBits, aBrc.aBits1);
    }

    return aBrc;
}

// MakeBorderLine() bekommt einen WW8Bytes* uebergeben, um die Funktion
// auch fuer die Tabellen-Umrandungen zu benutzen.
// Wenn nSprmNo == 0, dann wird der Opcode nicht ausgegeben.
// bShadow darf bei Tabellenzellen *nicht* gesetzt sein !
void WW8Export::Out_BorderLine(WW8Bytes& rO, const SvxBorderLine* pLine,
    USHORT nDist, sal_uInt16 nSprmNo, bool bShadow)
{
    ASSERT( ( nSprmNo == 0 ) ||
            ( nSprmNo >= 38 && nSprmNo <= 41 ) ||
            ( nSprmNo >= NS_sprm::LN_PBrcTop && nSprmNo <= NS_sprm::LN_PBrcRight ) ||
            ( nSprmNo >= NS_sprm::LN_SBrcTop && nSprmNo <= NS_sprm::LN_SBrcRight ),
            "Sprm for border out is of range" );

    WW8_BRC aBrc;

    if (pLine)
        aBrc = TranslateBorderLine( *pLine, nDist, bShadow );

    if( bWrtWW8 )
    {
        // WW97-SprmIds
        if ( nSprmNo != 0 )
            SwWW8Writer::InsUInt16( rO, nSprmNo );

        rO.Insert( aBrc.aBits1, 2, rO.Count() );
        rO.Insert( aBrc.aBits2, 2, rO.Count() );
    }
    else
    {
        // WW95-SprmIds
        if ( nSprmNo != 0 )
            rO.Insert( (BYTE)( nSprmNo ), rO.Count() );
        rO.Insert( aBrc.aBits1, 2, rO.Count() );
    }
}

// FormatBox1() ist fuer alle Boxen ausser in Tabellen.
// es wird pO des WW8Writers genommen
void WW8Export::Out_SwFmtBox(const SvxBoxItem& rBox, bool bShadow)
{
    if ( bOutPageDescs && !bWrtWW8 )
        return; // no page ouline in WW6

    static const USHORT aBorders[] =
    {
        BOX_LINE_TOP, BOX_LINE_LEFT, BOX_LINE_BOTTOM, BOX_LINE_RIGHT
    };
    static const sal_uInt16 aPBrc[] =
    {
        NS_sprm::LN_PBrcTop, NS_sprm::LN_PBrcLeft, NS_sprm::LN_PBrcBottom, NS_sprm::LN_PBrcRight
    };
    static const sal_uInt16 aSBrc[] =
    {
        NS_sprm::LN_SBrcTop, NS_sprm::LN_SBrcLeft, NS_sprm::LN_SBrcBottom, NS_sprm::LN_SBrcRight
    };
    static const sal_uInt16 aWW6PBrc[] =
    {
        38, 39, 40, 41
    };

    const USHORT* pBrd = aBorders;
    for( USHORT i = 0; i < 4; ++i, ++pBrd )
    {
        const SvxBorderLine* pLn = rBox.GetLine( *pBrd );

        sal_uInt16 nSprmNo = 0;
        if ( !bWrtWW8 )
            nSprmNo = aWW6PBrc[i];
        else if ( bOutPageDescs )
            nSprmNo = aSBrc[i];
        else
            nSprmNo = aPBrc[i];

        Out_BorderLine( *pO, pLn, rBox.GetDistance( *pBrd ), nSprmNo, bShadow );
    }
}

// FormatBox2() ist fuer TC-Strukturen in Tabellen. Der Sprm-Opcode
// wird nicht geschrieben, da es in der TC-Structur ohne Opcode gepackt ist.
// dxpSpace wird immer 0, da WW das in Tabellen so verlangt
// ( Tabellenumrandungen fransen sonst aus )
// Ein WW8Bytes-Ptr wird als Ausgabe-Parameter uebergeben

void WW8Export::Out_SwFmtTableBox( WW8Bytes& rO, const SvxBoxItem& rBox )
{
    // moeglich und vielleicht besser waere 0xffff
    static const USHORT aBorders[] =
    {
        BOX_LINE_TOP, BOX_LINE_LEFT, BOX_LINE_BOTTOM, BOX_LINE_RIGHT
    };
    const USHORT* pBrd = aBorders;
    for( int i = 0; i < 4; ++i, ++pBrd )
    {
        const SvxBorderLine* pLn = rBox.GetLine( *pBrd );
        Out_BorderLine(rO, pLn, 0, 0, false);
    }
}

void WW8AttributeOutput::FormatBox( const SvxBoxItem& rBox )
{
    // Fly um Grafik-> keine Umrandung hier, da
    // der GrafikHeader bereits die Umrandung hat
    if ( !m_rWW8Export.bOutGrf )
    {
        bool bShadow = false;
        const SfxPoolItem* pItem = m_rWW8Export.HasItem( RES_SHADOW );
        if ( pItem )
        {
            const SvxShadowItem* p = (const SvxShadowItem*)pItem;
            bShadow = ( p->GetLocation() != SVX_SHADOW_NONE )
                      && ( p->GetWidth() != 0 );
        }

        m_rWW8Export.Out_SwFmtBox( rBox, bShadow );
    }
}

SwTwips WW8Export::CurrentPageWidth(SwTwips &rLeft, SwTwips &rRight) const
{
    const SwFrmFmt* pFmt = pAktPageDesc ? &pAktPageDesc->GetMaster()
        : &const_cast<const SwDoc *>(pDoc)->GetPageDesc(0).GetMaster();

    const SvxLRSpaceItem& rLR = pFmt->GetLRSpace();
    SwTwips nPageSize = pFmt->GetFrmSize().GetWidth();
    rLeft = rLR.GetLeft();
    rRight = rLR.GetRight();
    return nPageSize;
}

void WW8AttributeOutput::FormatColumns_Impl( USHORT nCols, const SwFmtCol & rCol, bool bEven, SwTwips nPageSize )
{
    // CColumns
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_SCcolumns );
    else
        m_rWW8Export.pO->Insert( 144, m_rWW8Export.pO->Count(  ) );
    m_rWW8Export.InsUInt16( nCols - 1 );

    // DxaColumns
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_SDxaColumns );
    else
        m_rWW8Export.pO->Insert( 145, m_rWW8Export.pO->Count(  ) );
    m_rWW8Export.InsUInt16( rCol.GetGutterWidth( true ) );

    // LBetween
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_SLBetween );
    else
        m_rWW8Export.pO->Insert( 158, m_rWW8Export.pO->Count(  ) );
    m_rWW8Export.pO->Insert( COLADJ_NONE == rCol.GetLineAdj(  )? 0 : 1,
                             m_rWW8Export.pO->Count(  ) );

    const SwColumns & rColumns = rCol.GetColumns(  );

    // FEvenlySpaced
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_SFEvenlySpaced );
    else
        m_rWW8Export.pO->Insert( 138, m_rWW8Export.pO->Count(  ) );
    m_rWW8Export.pO->Insert( bEven ? 1 : 0, m_rWW8Export.pO->Count(  ) );
    
#if 0 
    // FIXME what's the use of this code
    if ( bEven )
    {
        USHORT nSpace = rColumns[0]->GetRight() + rColumns[1]->GetLeft();
        for ( n = 2; n < nCols; n++ )
        {
            short nDiff = nSpace - ( rColumns[n - 1]->GetRight()
                                         + rColumns[n]->GetLeft() );
            if (nDiff > 10 || nDiff < -10)
            {
                // Toleranz: 10 tw
                bEven = false;
                break;
            }
        }
    }
#endif

    if ( !bEven )
    {
        for ( USHORT n = 0; n < nCols; ++n )
        {
            //sprmSDxaColWidth
            if ( m_rWW8Export.bWrtWW8 )
                m_rWW8Export.InsUInt16( NS_sprm::LN_SDxaColWidth );
            else
                m_rWW8Export.pO->Insert( 136, m_rWW8Export.pO->Count(  ) );
            m_rWW8Export.pO->Insert( ( BYTE ) n, m_rWW8Export.pO->Count(  ) );
            m_rWW8Export.InsUInt16( rCol.
                                    CalcPrtColWidth( n,
                                                     ( USHORT ) nPageSize ) );

            if ( n + 1 != nCols )
            {
                //sprmSDxaColSpacing
                if ( m_rWW8Export.bWrtWW8 )
                    m_rWW8Export.InsUInt16( NS_sprm::LN_SDxaColSpacing );
                else
                    m_rWW8Export.pO->Insert( 137,
                                             m_rWW8Export.pO->Count(  ) );
                m_rWW8Export.pO->Insert( ( BYTE ) n,
                                         m_rWW8Export.pO->Count(  ) );
                m_rWW8Export.InsUInt16( rColumns[n]->GetRight(  ) +
                                        rColumns[n + 1]->GetLeft(  ) );
            }
        }
    }
}

void AttributeOutputBase::FormatColumns( const SwFmtCol& rCol )
{
    const SwColumns& rColumns = rCol.GetColumns();

    USHORT nCols = rColumns.Count();
    if ( 1 < nCols && !GetExport( ).bOutFlyFrmAttrs )
    {
        // dann besorge mal die Seitenbreite ohne Raender !!

        const SwFrmFmt* pFmt = GetExport( ).pAktPageDesc ? &GetExport( ).pAktPageDesc->GetMaster() : &const_cast<const SwDoc *>(GetExport( ).pDoc)->GetPageDesc(0).GetMaster();
        const SvxFrameDirectionItem &frameDirection = pFmt->GetFrmDir();
        SwTwips nPageSize;
        if ( frameDirection.GetValue() == FRMDIR_VERT_TOP_RIGHT || frameDirection.GetValue() == FRMDIR_VERT_TOP_LEFT )
        {
            const SvxULSpaceItem &rUL = pFmt->GetULSpace();
            nPageSize = pFmt->GetFrmSize().GetHeight();
            nPageSize -= rUL.GetUpper() + rUL.GetLower();

            const SwFmtHeader *header = dynamic_cast<const SwFmtHeader *>(pFmt->GetAttrSet().GetItem(RES_HEADER));
            if ( header )
            {
                const SwFrmFmt *headerFmt = header->GetHeaderFmt();
                if (headerFmt)
                {
                    nPageSize -= headerFmt->GetFrmSize().GetHeight();
                }
            }
            const SwFmtFooter *footer = dynamic_cast<const SwFmtFooter *>(pFmt->GetAttrSet().GetItem(RES_FOOTER));
            if ( footer )
            {
                const SwFrmFmt *footerFmt = footer->GetFooterFmt();
                if ( footerFmt )
                {
                    nPageSize -= footerFmt->GetFrmSize().GetHeight();
                }
            }
        }
        else
        {
            const SvxLRSpaceItem &rLR = pFmt->GetLRSpace();
            nPageSize = pFmt->GetFrmSize().GetWidth();
            nPageSize -= rLR.GetLeft() + rLR.GetRight();
        }

        // Nachsehen, ob alle Spalten gleich sind
        bool bEven = true;
        USHORT n;
        USHORT nColWidth = rCol.CalcPrtColWidth( 0, (USHORT)nPageSize );
        for ( n = 1; n < nCols; n++ )
        {
            short nDiff = nColWidth -
                rCol.CalcPrtColWidth( n, (USHORT)nPageSize );

            if ( nDiff > 10 || nDiff < -10 )      // Toleranz: 10 tw
            {
                bEven = false;
                break;
            }
        }
        
        FormatColumns_Impl( nCols, rCol, bEven, nPageSize );
    }
}

// "Paragraphs together"
void WW8AttributeOutput::FormatKeep( const SvxFmtKeepItem& rKeep )
{
    // sprmFKeepFollow
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFKeepFollow );
    else
        m_rWW8Export.pO->Insert( 8, m_rWW8Export.pO->Count() );

    m_rWW8Export.pO->Insert( rKeep.GetValue() ? 1 : 0, m_rWW8Export.pO->Count() );
}

// exclude a paragraph from Line Numbering
void WW8AttributeOutput::FormatLineNumbering( const SwFmtLineNumber& rNumbering )
{
    // sprmPFNoLineNumb
    if( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFNoLineNumb );
    else
        m_rWW8Export.pO->Insert( 14, m_rWW8Export.pO->Count() );

    m_rWW8Export.pO->Insert( rNumbering.IsCount() ? 0 : 1, m_rWW8Export.pO->Count() );
}


/* File PARATR.HXX  */

void WW8AttributeOutput::ParaLineSpacing_Impl( short nSpace, short nMulti )
{
    // sprmPDyaLine
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PDyaLine );
    else
        m_rWW8Export.pO->Insert( 20, m_rWW8Export.pO->Count() );

    m_rWW8Export.InsUInt16( nSpace );
    m_rWW8Export.InsUInt16( nMulti );
}

void AttributeOutputBase::ParaLineSpacing( const SvxLineSpacingItem& rSpacing )
{
    short nSpace = 240, nMulti = 0;

    switch ( rSpacing.GetLineSpaceRule() )
    {
        default:
            break;
        case SVX_LINE_SPACE_AUTO:
        case SVX_LINE_SPACE_FIX:
        case SVX_LINE_SPACE_MIN:
        {
            switch ( rSpacing.GetInterLineSpaceRule() )
            {
                case SVX_INTER_LINE_SPACE_FIX:      // unser Durchschuss
                {
                    // gibt es aber nicht in WW - also wie kommt man an
                    // die MaxLineHeight heran?
                    nSpace = (short)rSpacing.GetInterLineSpace();
                    sal_uInt16 nScript =
                        i18n::ScriptType::LATIN;
                    const SwAttrSet *pSet = 0;
                    if ( GetExport().pOutFmtNode && GetExport().pOutFmtNode->ISA( SwFmt ) )
                    {
                        const SwFmt *pFmt = (const SwFmt*)( GetExport().pOutFmtNode );
                        pSet = &pFmt->GetAttrSet();
                    }
                    else if ( GetExport().pOutFmtNode && GetExport().pOutFmtNode->ISA( SwTxtNode ) )
                    {
                        const SwTxtNode* pNd = (const SwTxtNode*)GetExport().pOutFmtNode;
                        pSet = &pNd->GetSwAttrSet();
                        if ( pBreakIt->xBreak.is() )
                        {
                            nScript = pBreakIt->xBreak->
                                getScriptType(pNd->GetTxt(), 0);
                        }
                    }
                    ASSERT( pSet, "No attrset for lineheight :-(" );
                    if ( pSet )
                    {
                        nSpace = nSpace + (short)( AttrSetToLineHeight( *GetExport().pDoc,
                            *pSet, *Application::GetDefaultDevice(), nScript ) );
                    }
                }
                break;
            case SVX_INTER_LINE_SPACE_PROP:
                nSpace = (short)( ( 240L * rSpacing.GetPropLineSpace() ) / 100L );
                nMulti = 1;
                break;
            default:                    // z.B. Minimum oder FIX?
                if ( SVX_LINE_SPACE_FIX == rSpacing.GetLineSpaceRule() )
                    nSpace = -(short)rSpacing.GetLineHeight();
                else
                    nSpace = (short)rSpacing.GetLineHeight();
                break;
            }
        }
        break;
    }

    ParaLineSpacing_Impl( nSpace, nMulti );
}

void WW8AttributeOutput::ParaAdjust( const SvxAdjustItem& rAdjust )
{
    // sprmPJc
    BYTE nAdj = 255;
    BYTE nAdjBiDi = 255;
    switch ( rAdjust.GetAdjust() )
    {
        case SVX_ADJUST_LEFT:
            nAdj = 0;
            nAdjBiDi = 2;
            break;
        case SVX_ADJUST_RIGHT:
            nAdj = 2;
            nAdjBiDi = 0;
            break;
        case SVX_ADJUST_BLOCKLINE:
        case SVX_ADJUST_BLOCK:
            nAdj = nAdjBiDi = 3;
            break;
        case SVX_ADJUST_CENTER:
            nAdj = nAdjBiDi = 1;
            break;
        default:
            return;    // not a supported Attribut
    }

    if ( 255 != nAdj )        // supported Attribut?
    {
        if ( m_rWW8Export.bWrtWW8 )
        {
            m_rWW8Export.InsUInt16( NS_sprm::LN_PJc );
            m_rWW8Export.pO->Insert( nAdj, m_rWW8Export.pO->Count() );

            /*
            Sadly for left to right paragraphs both these values are the same,
            for right to left paragraphs the bidi one is the reverse of the
            normal one.
            */
            m_rWW8Export.InsUInt16( NS_sprm::LN_PJcExtra ); //bidi version ?
            bool bBiDiSwap = false;
            if ( m_rWW8Export.pOutFmtNode )
            {
                short nDirection = FRMDIR_HORI_LEFT_TOP;
                if ( m_rWW8Export.pOutFmtNode->ISA( SwTxtNode ) )
                {
                    SwPosition aPos(*(const SwCntntNode*)m_rWW8Export.pOutFmtNode);
                    nDirection = m_rWW8Export.pDoc->GetTextDirection(aPos);
                }
                else if ( m_rWW8Export.pOutFmtNode->ISA( SwTxtFmtColl ) )
                {
                    const SwTxtFmtColl* pC =
                        (const SwTxtFmtColl*)m_rWW8Export.pOutFmtNode;
                    const SvxFrameDirectionItem &rItem =
                        ItemGet<SvxFrameDirectionItem>(*pC, RES_FRAMEDIR);
                    nDirection = rItem.GetValue();
                }
                if ( ( nDirection == FRMDIR_HORI_RIGHT_TOP ) ||
                     ( nDirection == FRMDIR_ENVIRONMENT && Application::GetSettings().GetLayoutRTL() ) )
                {
                    bBiDiSwap = true;
                }
            }

            if ( bBiDiSwap )
                m_rWW8Export.pO->Insert( nAdjBiDi, m_rWW8Export.pO->Count() );
            else
                m_rWW8Export.pO->Insert( nAdj, m_rWW8Export.pO->Count() );
        }
        else
        {
            m_rWW8Export.pO->Insert( 5, m_rWW8Export.pO->Count() );
            m_rWW8Export.pO->Insert( nAdj, m_rWW8Export.pO->Count() );
        }
    }
}

void WW8AttributeOutput::FormatFrameDirection( const SvxFrameDirectionItem& rDirection )
{
    if ( !m_rWW8Export.bWrtWW8 )   //8+ only
        return;

    UINT16 nTextFlow=0;
    bool bBiDi = false;
    short nDir = rDirection.GetValue();

    if ( nDir == FRMDIR_ENVIRONMENT )
    {
        if ( m_rWW8Export.bOutPageDescs )
            nDir = m_rWW8Export.GetCurrentPageDirection();
        else if ( m_rWW8Export.pOutFmtNode )
        {
            if ( m_rWW8Export.bOutFlyFrmAttrs )  //frame
            {
                nDir = m_rWW8Export.TrueFrameDirection(
                    *(const SwFrmFmt*)m_rWW8Export.pOutFmtNode );
            }
            else if ( m_rWW8Export.pOutFmtNode->ISA( SwCntntNode ) )   //pagagraph
            {
                const SwCntntNode* pNd =
                    (const SwCntntNode*)m_rWW8Export.pOutFmtNode;
                SwPosition aPos( *pNd );
                nDir = m_rWW8Export.pDoc->GetTextDirection( aPos );
            }
            else if ( m_rWW8Export.pOutFmtNode->ISA( SwTxtFmtColl ) )
                nDir = FRMDIR_HORI_LEFT_TOP;    //what else can we do :-(
        }

        if ( nDir == FRMDIR_ENVIRONMENT )
            nDir = FRMDIR_HORI_LEFT_TOP;    //Set something
    }

    switch ( nDir )
    {
        default:
            //Can't get an unknown type here
            ASSERT(false, "Unknown frame direction");
        case FRMDIR_HORI_LEFT_TOP:
            nTextFlow = 0;
            break;
        case FRMDIR_HORI_RIGHT_TOP:
            nTextFlow = 0;
            bBiDi = true;
            break;
        case FRMDIR_VERT_TOP_LEFT:  //word doesn't have this
        case FRMDIR_VERT_TOP_RIGHT:
            nTextFlow = 1;
            break;
    }

    if ( m_rWW8Export.bOutPageDescs )
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_STextFlow );
        m_rWW8Export.InsUInt16( nTextFlow );
        m_rWW8Export.InsUInt16( NS_sprm::LN_SFBiDi );
        m_rWW8Export.pO->Insert( bBiDi, m_rWW8Export.pO->Count() );
    }
    else if ( !m_rWW8Export.bOutFlyFrmAttrs )  //paragraph/style
    {
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFBiDi );
        m_rWW8Export.pO->Insert( bBiDi, m_rWW8Export.pO->Count() );
    }
}

// "Separate paragraphs"
void WW8AttributeOutput::ParaSplit( const SvxFmtSplitItem& rSplit )
{
    // sprmPFKeep
    if ( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFKeep );
    else
        m_rWW8Export.pO->Insert( 7, m_rWW8Export.pO->Count() );
    m_rWW8Export.pO->Insert( rSplit.GetValue() ? 0 : 1, m_rWW8Export.pO->Count() );
}

//  Es wird nur das Item "SvxWidowItem" und nicht die Orphans uebersetzt,
//  da es fuer beides im WW nur ein Attribut "Absatzkontrolle" gibt und
//  im SW wahrscheinlich vom Anwender immer Beide oder keiner gesetzt werden.
void WW8AttributeOutput::ParaWidows( const SvxWidowsItem& rWidows )
{
// sprmPFWidowControl
    if( m_rWW8Export.bWrtWW8 )
        m_rWW8Export.InsUInt16( NS_sprm::LN_PFWidowControl );
    else
        m_rWW8Export.pO->Insert( 51, m_rWW8Export.pO->Count() );
    m_rWW8Export.pO->Insert( rWidows.GetValue() ? 1 : 0, m_rWW8Export.pO->Count() );
}


class SwWW8WrTabu
{
    BYTE* pDel;                 // DelArray
    BYTE* pAddPos;              // AddPos-Array
    BYTE* pAddTyp;              // AddTyp-Array
    sal_uInt16 nAdd;            // so viele Tabs kommen hinzu
    sal_uInt16 nDel;            // so viele Tabs fallen weg
public:
    SwWW8WrTabu(sal_uInt16 nDelMax, sal_uInt16 nAddMax);
    ~SwWW8WrTabu();

    void Add(const SvxTabStop &rTS, long nAdjustment);
    void Del(const SvxTabStop &rTS, long nAdjustment);
    void PutAll(WW8Export& rWW8Wrt);
};

SwWW8WrTabu::SwWW8WrTabu(sal_uInt16 nDelMax, sal_uInt16 nAddMax)
    : nAdd(0), nDel(0)
{
    pDel = nDelMax ? new BYTE[nDelMax * 2] : 0;
    pAddPos = new BYTE[nAddMax * 2];
    pAddTyp = new BYTE[nAddMax];
}

SwWW8WrTabu::~SwWW8WrTabu()
{
    delete[] pAddTyp;
    delete[] pAddPos;
    delete[] pDel;
}

// Add( const SvxTabStop & rTS ) fuegt einen Tab in die WW-Struktur ein
void SwWW8WrTabu::Add(const SvxTabStop & rTS, long nAdjustment)
{
    // Tab-Position eintragen
    ShortToSVBT16(msword_cast<sal_Int16>(rTS.GetTabPos() + nAdjustment),
        pAddPos + (nAdd * 2));

    // Tab-Typ eintragen
    BYTE nPara = 0;
    switch (rTS.GetAdjustment())
    {
        case SVX_TAB_ADJUST_RIGHT:
            nPara = 2;
            break;
        case SVX_TAB_ADJUST_CENTER:
            nPara = 1;
            break;
        case SVX_TAB_ADJUST_DECIMAL:
            /*
            Theres nothing we can do btw the the decimal seperator has been
            customized, but if you think different remember that different
            locales have different seperators, i.e. german is a , while english
            is a .
            */
            nPara = 3;
            break;
        default:
            break;
    }

    switch( rTS.GetFill() )
    {
        case '.':   // dotted leader
            nPara |= 1 << 3;
            break;
        case '_':   // Single line leader
            nPara |= 3 << 3;
            break;
        case '-':   // hyphenated leader
            nPara |= 2 << 3;
            break;
        case '=':   // heavy line leader
            nPara |= 4 << 3;
            break;
    }

    ByteToSVBT8(nPara, pAddTyp + nAdd);
    ++nAdd;
}

// Del( const SvxTabStop & rTS ) fuegt einen zu loeschenden Tab
// in die WW-Struktur ein
void SwWW8WrTabu::Del(const SvxTabStop &rTS, long nAdjustment)
{
    // Tab-Position eintragen
    ShortToSVBT16(msword_cast<sal_Int16>(rTS.GetTabPos() + nAdjustment),
        pDel + (nDel * 2));
    ++nDel;
}

//  PutAll( WW8Export& rWW8Wrt ) schreibt das Attribut nach rWrt.pO
void SwWW8WrTabu::PutAll(WW8Export& rWrt)
{
    if (!nAdd && !nDel) //It its a no-op
        return;
    ASSERT(nAdd <= 255, "more than 255 added tabstops ?");
    ASSERT(nDel <= 255, "more than 244 removed tabstops ?");
    if (nAdd > 255)
        nAdd = 255;
    if (nDel > 255)
        nDel = 255;

    sal_uInt16 nSiz = 2 * nDel + 3 * nAdd + 2;
    if (nSiz > 255)
        nSiz = 255;

    if (rWrt.bWrtWW8)
        rWrt.InsUInt16(NS_sprm::LN_PChgTabsPapx);
    else
        rWrt.pO->Insert(15, rWrt.pO->Count());
    // cch eintragen
    rWrt.pO->Insert(msword_cast<sal_uInt8>(nSiz), rWrt.pO->Count());
    // DelArr schreiben
    rWrt.pO->Insert(msword_cast<sal_uInt8>(nDel), rWrt.pO->Count());
    rWrt.OutSprmBytes(pDel, nDel * 2);
    // InsArr schreiben
    rWrt.pO->Insert(msword_cast<sal_uInt8>(nAdd), rWrt.pO->Count());
    rWrt.OutSprmBytes(pAddPos, 2 * nAdd);         // AddPosArray
    rWrt.OutSprmBytes(pAddTyp, nAdd);             // AddTypArray
}


static void ParaTabStopAdd( WW8Export& rWrt, const SvxTabStopItem& rTStops,
    long nLParaMgn )
{
    SwWW8WrTabu aTab( 0, rTStops.Count());

    for( USHORT n = 0; n < rTStops.Count(); n++ )
    {
        const SvxTabStop& rTS = rTStops[n];
        // Def-Tabs ignorieren
        if (SVX_TAB_ADJUST_DEFAULT != rTS.GetAdjustment())
            aTab.Add(rTS, nLParaMgn);
    }
    aTab.PutAll( rWrt );
}

bool lcl_IsEqual(long nOneLeft, const SvxTabStop &rOne,
    long nTwoLeft, const SvxTabStop &rTwo)
{
    return(
            nOneLeft == nTwoLeft &&
            rOne.GetAdjustment() == rTwo.GetAdjustment() &&
            rOne.GetDecimal() == rTwo.GetDecimal() &&
            rOne.GetFill() == rTwo.GetFill()
          );
}

static void ParaTabStopDelAdd( WW8Export& rWrt, const SvxTabStopItem& rTStyle,
    long nLStypeMgn, const SvxTabStopItem& rTNew, long nLParaMgn )
{
    SwWW8WrTabu aTab(rTStyle.Count(), rTNew.Count());

    USHORT nO = 0;      // rTStyle Index
    USHORT nN = 0;      // rTNew Index

    do {
        const SvxTabStop* pTO;
        long nOP;
        if( nO < rTStyle.Count() )                  // alt noch nicht am Ende ?
        {
            pTO = &rTStyle[ nO ];
            nOP = pTO->GetTabPos() + nLStypeMgn;
            if( SVX_TAB_ADJUST_DEFAULT == pTO->GetAdjustment() )
            {
                nO++;                                // Default-Tab ignorieren
                continue;
            }
        }
        else
        {
            pTO = 0;
            nOP = LONG_MAX;
        }

        const SvxTabStop* pTN;
        long nNP;
        if( nN < rTNew.Count() )                    // neu noch nicht am Ende
        {
            pTN = &rTNew[ nN ];
            nNP = pTN->GetTabPos() + nLParaMgn;
            if( SVX_TAB_ADJUST_DEFAULT == pTN->GetAdjustment() )
            {
                nN++;                               // Default-Tab ignorieren
                continue;
            }
        }
        else
        {
            pTN = 0;
            nNP = LONG_MAX;
        }

        if( nOP == LONG_MAX && nNP == LONG_MAX )
            break;                                  // alles fertig

        if( nOP < nNP )                             // naechster Tab ist alt
        {
            aTab.Del(*pTO, nLStypeMgn);             // muss geloescht werden
            nO++;
        }
        else if( nNP < nOP )                        // naechster Tab ist neu
        {
            aTab.Add(*pTN, nLParaMgn);              // muss eigefuegt werden
            nN++;
        }
        else if (lcl_IsEqual(nOP, *pTO, nNP, *pTN)) // Tabs sind gleich:
        {
            nO++;                                   // nichts zu tun
            nN++;
        }
        else                                        // Tabs selbe Pos, diff Typ
        {
            aTab.Del(*pTO, nLStypeMgn);             // alten loeschen
            aTab.Add(*pTN, nLParaMgn);              // neuen einfuegen
            nO++;
            nN++;
        }
    } while( 1 );

    aTab.PutAll( rWrt );
}

void WW8AttributeOutput::ParaTabStop( const SvxTabStopItem& rTabStops )
{
    bool bTabsRelativeToIndex = m_rWW8Export.pCurPam->GetDoc()->get( IDocumentSettingAccess::TABS_RELATIVE_TO_INDENT );
    long nCurrentLeft = 0;
    
    if ( bTabsRelativeToIndex )
    {
        const SfxPoolItem* pLR = m_rWW8Export.HasItem( RES_LR_SPACE );
        
        if ( pLR != NULL )
            nCurrentLeft = ((const SvxLRSpaceItem*)pLR)->GetTxtLeft();
    }

    // --> FLR 2009-03-17 #i100264#
    if ( m_rWW8Export.bStyDef &&
         m_rWW8Export.pCurrentStyle != NULL &&
         m_rWW8Export.pCurrentStyle->DerivedFrom() != NULL ) 
    {
        SvxTabStopItem aTabs( 0, 0, SVX_TAB_ADJUST_DEFAULT, RES_PARATR_TABSTOP );
        const SwFmt *pParentStyle = m_rWW8Export.pCurrentStyle->DerivedFrom();
        const SvxTabStopItem* pParentTabs = HasItem<SvxTabStopItem>( pParentStyle->GetAttrSet(), RES_PARATR_TABSTOP );
        if ( pParentTabs )
        {
            aTabs.Insert( pParentTabs );
        }
        
        ParaTabStopDelAdd( m_rWW8Export, aTabs, 0, rTabStops, 0 );
        return;
    }
    // <--

    // StyleDef -> "einfach" eintragen || keine Style-Attrs -> dito
    const SvxTabStopItem* pStyleTabs = 0;
    if ( !m_rWW8Export.bStyDef && m_rWW8Export.pStyAttr )
    {
        pStyleTabs =
            HasItem<SvxTabStopItem>( *m_rWW8Export.pStyAttr, RES_PARATR_TABSTOP );
    }

    if ( !pStyleTabs )
        ParaTabStopAdd(m_rWW8Export, rTabStops, nCurrentLeft);
    else
    {
        long nStyleLeft = 0;
        
        if (bTabsRelativeToIndex)
        {
            const SvxLRSpaceItem &rStyleLR =
            ItemGet<SvxLRSpaceItem>(*m_rWW8Export.pStyAttr, RES_LR_SPACE);
            nStyleLeft = rStyleLR.GetTxtLeft();
        }

        ParaTabStopDelAdd(m_rWW8Export, *pStyleTabs, nStyleLeft, rTabStops,
            nCurrentLeft);
    }
}

//-----------------------------------------------------------------------

void AttributeOutputBase::OutputItem( const SfxPoolItem& rHt )
{
    // FIXME maybe use 'item_cast', like 'item_cast<SvxCharHiddenItem>( rHt )'?
    switch ( rHt.Which() )
    {
        case RES_CHRATR_CASEMAP:
            CharCaseMap( static_cast< const SvxCaseMapItem& >( rHt ) );
            break;
        case RES_CHRATR_COLOR:
            CharColor( static_cast< const SvxColorItem& >( rHt ) );
            break;
        case RES_CHRATR_CONTOUR:
            CharContour( static_cast< const SvxContourItem& >( rHt ) );
            break;
        case RES_CHRATR_CROSSEDOUT:
            CharCrossedOut( static_cast< const SvxCrossedOutItem& >( rHt ) );
            break;
        case RES_CHRATR_ESCAPEMENT:
            CharEscapement( static_cast< const SvxEscapementItem& >( rHt ) );
            break;
        case RES_CHRATR_FONT:
            CharFont( static_cast< const SvxFontItem& >( rHt ) );
            break;
        case RES_CHRATR_FONTSIZE:
            CharFontSize( static_cast< const SvxFontHeightItem& >( rHt ) );
            break;
        case RES_CHRATR_KERNING:
            CharKerning( static_cast< const SvxKerningItem& >( rHt ) );
            break;
        case RES_CHRATR_LANGUAGE:
            CharLanguage( static_cast< const SvxLanguageItem& >( rHt ) );
            break;
        case RES_CHRATR_POSTURE:
            CharPosture( static_cast< const SvxPostureItem& >( rHt ) );
            break;
        case RES_CHRATR_SHADOWED:
            CharShadow( static_cast< const SvxShadowedItem& >( rHt ) );
            break;
        case RES_CHRATR_UNDERLINE:
            CharUnderline( static_cast< const SvxUnderlineItem& >( rHt ) );
            break;
        case RES_CHRATR_WEIGHT:
            CharWeight( static_cast< const SvxWeightItem& >( rHt ) );
            break;
        case RES_CHRATR_AUTOKERN:
            CharAutoKern( static_cast< const SvxAutoKernItem& >( rHt ) );
            break;
        case RES_CHRATR_BLINK:
            CharAnimatedText( static_cast< const SvxBlinkItem& >( rHt ) );
            break;
        case RES_CHRATR_BACKGROUND:
            CharBackground( static_cast< const SvxBrushItem& >( rHt ) );
            break;

        case RES_CHRATR_CJK_FONT:
            CharFontCJK( static_cast< const SvxFontItem& >( rHt ) );
            break;
        case RES_CHRATR_CJK_FONTSIZE:
            CharFontSizeCJK( static_cast< const SvxFontHeightItem& >( rHt ) );
            break;
        case RES_CHRATR_CJK_LANGUAGE:
            CharLanguageCJK( static_cast< const SvxLanguageItem& >( rHt ) );
            break;
        case RES_CHRATR_CJK_POSTURE:
            CharPostureCJK( static_cast< const SvxPostureItem& >( rHt ) );
            break;
        case RES_CHRATR_CJK_WEIGHT:
            CharWeightCJK( static_cast< const SvxWeightItem& >( rHt ) );
            break;

        case RES_CHRATR_CTL_FONT:
            CharFontCTL( static_cast< const SvxFontItem& >( rHt ) );
            break;
        case RES_CHRATR_CTL_FONTSIZE:
            CharFontSizeCTL( static_cast< const SvxFontHeightItem& >( rHt ) );
            break;
        case RES_CHRATR_CTL_LANGUAGE:
            CharLanguageCTL( static_cast< const SvxLanguageItem& >( rHt ) );
            break;
        case RES_CHRATR_CTL_POSTURE:
            CharPostureCTL( static_cast< const SvxPostureItem& >( rHt ) );
            break;
        case RES_CHRATR_CTL_WEIGHT:
            CharWeightCTL( static_cast< const SvxWeightItem& >( rHt ) );
            break;

        case RES_CHRATR_ROTATE:
            CharRotate( static_cast< const SvxCharRotateItem& >( rHt ) );
            break;
        case RES_CHRATR_EMPHASIS_MARK:
            CharEmphasisMark( static_cast< const SvxEmphasisMarkItem& >( rHt ) );
            break;
        case RES_CHRATR_TWO_LINES:
            CharTwoLines( static_cast< const SvxTwoLinesItem& >( rHt ) );
            break;
        case RES_CHRATR_SCALEW:
            CharScaleWidth( static_cast< const SvxCharScaleWidthItem& >( rHt ) );
            break;
        case RES_CHRATR_RELIEF:
            CharRelief( static_cast< const SvxCharReliefItem& >( rHt ) );
            break;
        case RES_CHRATR_HIDDEN:
            CharHidden( static_cast< const SvxCharHiddenItem& >( rHt ) );
            break;

        case RES_TXTATR_INETFMT:
            TextINetFormat( static_cast< const SwFmtINetFmt& >( rHt ) );
            break;
        case RES_TXTATR_CHARFMT:
            TextCharFormat( static_cast< const SwFmtCharFmt& >( rHt ) );
            break;
        case RES_TXTATR_FIELD:
            TextField( static_cast< const SwFmtFld& >( rHt ) );
            break;
        case RES_TXTATR_FLYCNT:
            TextFlyContent( static_cast< const SwFmtFlyCnt& >( rHt ) );
            break;
        case RES_TXTATR_FTN:
            TextFootnote( static_cast< const SwFmtFtn& >( rHt ) );
            break;
        case RES_TXTATR_HARDBLANK:
            TextHardBlank( static_cast< const SwFmtHardBlank& >( rHt ) );
            break;

        case RES_PARATR_LINESPACING:
            ParaLineSpacing( static_cast< const SvxLineSpacingItem& >( rHt ) );
            break;
        case RES_PARATR_ADJUST:
            ParaAdjust( static_cast< const SvxAdjustItem& >( rHt ) );
            break;
        case RES_PARATR_SPLIT:
            ParaSplit( static_cast< const SvxFmtSplitItem& >( rHt ) );
            break;
        case RES_PARATR_WIDOWS:
            ParaWidows( static_cast< const SvxWidowsItem& >( rHt ) );
            break;
        case RES_PARATR_TABSTOP:
            ParaTabStop( static_cast< const SvxTabStopItem& >( rHt ) );
            break;
        case RES_PARATR_HYPHENZONE:
            ParaHyphenZone( static_cast< const SvxHyphenZoneItem& >( rHt ) );
            break;
        case RES_PARATR_NUMRULE:
            ParaNumRule( static_cast< const SwNumRuleItem& >( rHt ) );
            break;
        case RES_PARATR_SCRIPTSPACE:
            ParaScriptSpace( static_cast< const SfxBoolItem& >( rHt ) );
            break;
        case RES_PARATR_HANGINGPUNCTUATION:
            ParaHangingPunctuation( static_cast< const SfxBoolItem& >( rHt ) );
            break;
        case RES_PARATR_FORBIDDEN_RULES:
            ParaForbiddenRules( static_cast< const SfxBoolItem& >( rHt ) );
            break;
        case RES_PARATR_VERTALIGN:
            ParaVerticalAlign( static_cast< const SvxParaVertAlignItem& >( rHt ) );
            break;
        case RES_PARATR_SNAPTOGRID:
            ParaSnapToGrid( static_cast< const SvxParaGridItem& >( rHt ) );
            break;

        case RES_FRM_SIZE:
            FormatFrameSize( static_cast< const SwFmtFrmSize& >( rHt ) );
            break;
        case RES_PAPER_BIN:
            FormatPaperBin( static_cast< const SvxPaperBinItem& >( rHt ) );
            break;
        case RES_LR_SPACE:
            FormatLRSpace( static_cast< const SvxLRSpaceItem& >( rHt ) );
            break;
        case RES_UL_SPACE:
            FormatULSpace( static_cast< const SvxULSpaceItem& >( rHt ) );
            break;
        case RES_PAGEDESC:
            FormatPageDescription( static_cast< const SwFmtPageDesc& >( rHt ) );
            break;
        case RES_BREAK:
            FormatBreak( static_cast< const SvxFmtBreakItem& >( rHt ) );
            break;
        case RES_SURROUND:
            FormatSurround( static_cast< const SwFmtSurround& >( rHt ) );
            break;
        case RES_VERT_ORIENT:
            FormatVertOrientation( static_cast< const SwFmtVertOrient& >( rHt ) );
            break;
        case RES_HORI_ORIENT:
            FormatHorizOrientation( static_cast< const SwFmtHoriOrient& >( rHt ) );
            break;
        case RES_ANCHOR:
            FormatAnchor( static_cast< const SwFmtAnchor& >( rHt ) );
            break;
        case RES_BACKGROUND:
            FormatBackground( static_cast< const SvxBrushItem& >( rHt ) );
            break;
        case RES_BOX:
            FormatBox( static_cast< const SvxBoxItem& >( rHt ) );
            break;
        case RES_COL:
            FormatColumns( static_cast< const SwFmtCol& >( rHt ) );
            break;
        case RES_KEEP:
            FormatKeep( static_cast< const SvxFmtKeepItem& >( rHt ) );
            break;
        case RES_TEXTGRID:
            FormatTextGrid( static_cast< const SwTextGridItem& >( rHt ) );
            break;
        case RES_LINENUMBER:
            FormatLineNumbering( static_cast< const SwFmtLineNumber& >( rHt ) );
            break;
        case RES_FRAMEDIR:
            FormatFrameDirection( static_cast< const SvxFrameDirectionItem& >( rHt ) );
            break;

        default:
#if OSL_DEBUG_LEVEL > 0
            fprintf( stderr, "Unhandled SfxPoolItem with id %d.\n", rHt.Which() );
#endif
            break;
    }
}

void AttributeOutputBase::OutputStyleItemSet( const SfxItemSet& rSet, BOOL bDeep, BOOL bTestForDefault )
{
    // based on OutputItemSet() from wrt_fn.cxx

    const SfxItemPool& rPool = *rSet.GetPool();
    const SfxItemSet* pSet = &rSet;
    if ( !pSet->Count() )
    {
        if ( !bDeep )
            return;

        while ( 0 != ( pSet = pSet->GetParent() ) && !pSet->Count() )
            ;

        if ( !pSet )
            return;
    }

    const SfxPoolItem* pItem;
    if ( !bDeep || !pSet->GetParent() )
    {
        ASSERT( rSet.Count(), "Wurde doch schon behandelt oder?" );
        SfxItemIter aIter( *pSet );
        pItem = aIter.GetCurItem();
        do {
            OutputItem( *pItem );
        } while ( !aIter.IsAtEnd() && 0 != ( pItem = aIter.NextItem() ) );
    }
    else
    {
        SfxWhichIter aIter( *pSet );
        USHORT nWhich = aIter.FirstWhich();
        while ( nWhich )
        {
            if ( SFX_ITEM_SET == pSet->GetItemState( nWhich, bDeep, &pItem ) &&
                 ( !bTestForDefault ||
                   *pItem != rPool.GetDefaultItem( nWhich ) ||
                   ( pSet->GetParent() && *pItem != pSet->GetParent()->Get( nWhich ) ) ) )
            {
                OutputItem( *pItem );
            }
            nWhich = aIter.NextWhich();
        }
    }
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
