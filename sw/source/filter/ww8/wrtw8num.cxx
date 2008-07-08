/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: wrtw8num.cxx,v $
 * $Revision: 1.47 $
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


#include <hintids.hxx>
#include <vcl/font.hxx>
#include <svx/fontitem.hxx>
#include <svx/lrspitem.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <numrule.hxx>
#include <paratr.hxx>
#include <charfmt.hxx>
#include <ndtxt.hxx>
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#include "writerhelper.hxx"
#include "writerwordglue.hxx"
#include "wrtww8.hxx"
#include "ww8par.hxx"

//#define DUMPSYMBOLS
#ifdef DUMPSYMBOLS
#include <fstream>
#endif

using namespace ::com::sun::star;
using namespace sw::types;
using namespace sw::util;

USHORT SwWW8Writer::DupNumRuleWithLvlStart(const SwNumRule *pRule,BYTE nLvl,
    USHORT nVal)
{
    //List is set to restart at a particular value so for export make a
    //completely new list based on this one and export that instead,
    //which duplicates words behaviour in this respect.
    USHORT nNumId = USHRT_MAX;
    String sPrefix(CREATE_CONST_ASC("WW8TempExport"));
    sPrefix += String::CreateFromInt32( nUniqueList++ );
    // --> OD 2008-02-11 #newlistlevelattrs#
    SwNumRule* pMyNumRule =
            new SwNumRule( pDoc->GetUniqueNumRuleName(&sPrefix),
                           SvxNumberFormat::LABEL_WIDTH_AND_POSITION );
    // <--
    pUsedNumTbl->Insert( pMyNumRule, pUsedNumTbl->Count() );

    for (USHORT i=0;i<MAXLEVEL;i++)
    {
        const SwNumFmt& rSubRule = pRule->Get(i);
        pMyNumRule->Set( i, rSubRule );
    }

    SwNumFmt aNumFmt(pMyNumRule->Get(nLvl));
    aNumFmt.SetStart(nVal);
    pMyNumRule->Set(nLvl,aNumFmt);

    nNumId = GetId( *pMyNumRule );

    //Map the old list to our new list
    aRuleDuplicates[GetId(*pRule)] = nNumId;

    return nNumId;
}

USHORT SwWW8Writer::GetId( const SwNumRule& rNumRule ) const
{
    if( !pUsedNumTbl )
    {
        SwWW8Writer* pThis = (SwWW8Writer*)this;
        pThis->pUsedNumTbl = new SwNumRuleTbl;
        pThis->pUsedNumTbl->Insert( &pDoc->GetNumRuleTbl(), 0 );
        // --> OD 2005-10-17 #126238# - Check, if the outline rule is
        // already inserted into <pUsedNumTbl>. If yes, do not insert it again.
        bool bOutlineRuleAdded( false );
        for( USHORT n = pUsedNumTbl->Count(); n; )
        {
            const SwNumRule& rRule = *pUsedNumTbl->GetObject( --n );
            if ( !pDoc->IsUsed( rRule ) )
            {
                pThis->pUsedNumTbl->Remove( n );
            }
            else if ( &rRule == pDoc->GetOutlineNumRule() )
            {
                bOutlineRuleAdded = true;
            }
        }

        if ( !bOutlineRuleAdded )
        {
            // jetzt noch die OutlineRule einfuegen
            SwNumRule* pR = (SwNumRule*)pDoc->GetOutlineNumRule();
            pThis->pUsedNumTbl->Insert( pR, pUsedNumTbl->Count() );
        }
        // <--
    }
    SwNumRule* p = (SwNumRule*)&rNumRule;
    USHORT nRet = pUsedNumTbl->GetPos(p);

    //Is this list now duplicated into a new list which we should use
    // --> OD 2007-05-30 #i77812#
    // perform 'deep' search in duplication map
    ::std::map<USHORT,USHORT>::const_iterator aResult = aRuleDuplicates.end();
    do {
        aResult = aRuleDuplicates.find(nRet);
        if ( aResult != aRuleDuplicates.end() )
        {
            nRet = (*aResult).second;
        }
    } while ( aResult != aRuleDuplicates.end() );
    // <--

    return nRet;
}

//GetFirstLineOffset should problem never appear unadorned apart from
//here in the ww export filter
sal_Int16 GetWordFirstLineOffset(const SwNumFmt &rFmt)
{
    ASSERT( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION,
            "<GetWordFirstLineOffset> - misusage: position-and-space-mode does not equal LABEL_WIDTH_AND_POSITION" );

    short nFirstLineOffset;
    if (rFmt.GetNumAdjust() == SVX_ADJUST_RIGHT)
        nFirstLineOffset = -rFmt.GetCharTextDistance();
    else
        nFirstLineOffset = rFmt.GetFirstLineOffset();
    return nFirstLineOffset;
}

void SwWW8Writer::OutListTab()
{
    if( !pUsedNumTbl )
        return ;            // no numbering is used

    USHORT nCount = pUsedNumTbl->Count();
    USHORT n;

    pFib->fcPlcfLst = pTableStrm->Tell();
    SwWW8Writer::WriteShort( *pTableStrm, nCount );

    // First Loop - write static data of SwNumRule - LSTF
    for( n = 0; n < nCount; ++n )
    {
        const SwNumRule& rRule = *pUsedNumTbl->GetObject( n );

        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );
        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );

        // mit keinen Styles verbunden
        for( int i = 0; i < WW8ListManager::nMaxLevel; ++i )
            SwWW8Writer::WriteShort( *pTableStrm, 0xFFF );

        BYTE nFlags = 0, nDummy = 0;
        if( rRule.IsContinusNum() )
            nFlags |= 0x1;
        *pTableStrm << nFlags << nDummy;
    }

    // set len to FIB
    pFib->lcbPlcfLst = pTableStrm->Tell() - pFib->fcPlcfLst;

    // second Loop - write all Levels for all SwNumRules - LVLF

    // prepare the NodeNum to generate the NumString
    SwNumberTree::tNumberVector aNumVector;
    for (n = 0; n < WW8ListManager::nMaxLevel; ++n)
        aNumVector.push_back(n);

    BYTE aPapSprms [] = {
        0x0f, 0x84, 0, 0,               // sprmPDxaLeft
        0x11, 0x84, 0, 0,               // sprmPDxaLeft1
        0x15, 0xc6, 0x05, 0x00, 0x01, 0, 0, 0x06
    };

    StarSymbolToMSMultiFont *pConvert = 0;
    for( n = 0; n < nCount; ++n )
    {
        const SwNumRule& rRule = *pUsedNumTbl->GetObject( n );
        BYTE nLvl;
        BYTE nFlags = 0;
        BYTE nAlign;
        BYTE nLevels = static_cast< BYTE >(rRule.IsContinusNum() ?
            WW8ListManager::nMinLevel : WW8ListManager::nMaxLevel);
        for( nLvl = 0; nLvl < nLevels; ++nLvl )
        {
            // write the static data of the SwNumFmt of this level
            BYTE aNumLvlPos[WW8ListManager::nMaxLevel] = { 0,0,0,0,0,0,0,0,0 };

            const SwNumFmt& rFmt = rRule.Get( nLvl );
            SwWW8Writer::WriteLong( *pTableStrm, rFmt.GetStart() );
            *pTableStrm << SwWW8Writer::GetNumId( rFmt.GetNumberingType() );

            switch( rFmt.GetNumAdjust() )
            {
            case SVX_ADJUST_CENTER:
                nAlign = 1;
                break;
            case SVX_ADJUST_RIGHT:
                nAlign = 2;
                break;
            default:
                nAlign = 0;
                break;
            }
            *pTableStrm << nAlign;

            // --> OD 2008-06-03 #i86652#
            if ( rFmt.GetPositionAndSpaceMode() ==
                                    SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
            {
                nFlags = 2;     // ixchFollow: 0 - tab, 1 - blank, 2 - nothing
            }
            else if ( rFmt.GetPositionAndSpaceMode() ==
                                            SvxNumberFormat::LABEL_ALIGNMENT )
            {
                switch ( rFmt.GetLabelFollowedBy() )
                {
                    case SvxNumberFormat::LISTTAB:
                    {
                        nFlags = 0;
                    }
                    break;
                    case SvxNumberFormat::SPACE:
                    {
                        nFlags = 1;
                    }
                    break;
                    case SvxNumberFormat::NOTHING:
                    {
                        nFlags = 2;
                    }
                    break;
                    default:
                    {
                        nFlags = 0;
                        ASSERT( false,
                                "unknown GetLabelFollowedBy() return value" );
                    }
                }
            }
            // <--

            // Build the NumString for this Level
            String sNumStr;
            String sFontName;
            bool bWriteBullet = false;
            const Font* pBulletFont=0;
            rtl_TextEncoding eChrSet=0;
            FontFamily eFamily=FAMILY_DECORATIVE;
            if( SVX_NUM_CHAR_SPECIAL == rFmt.GetNumberingType() ||
                SVX_NUM_BITMAP == rFmt.GetNumberingType() )
            {
                sNumStr = rFmt.GetBulletChar();
                bWriteBullet = true;

                pBulletFont = rFmt.GetBulletFont();
                if (!pBulletFont)
                {
                    pBulletFont = &numfunc::GetDefBulletFont();
                }

                eChrSet = pBulletFont->GetCharSet();
                sFontName = pBulletFont->GetName();
                eFamily = pBulletFont->GetFamily();

                if (sw::util::IsStarSymbol(sFontName))
                    SubstituteBullet(sNumStr,eChrSet,sFontName);

                // --> OD 2008-06-03 #i86652#
                if ( rFmt.GetPositionAndSpaceMode() ==
                                        SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
                {
                    // --> OD 2007-07-23 #148661#
                    // <nFlags = 2>, if minimum label width equals 0 and
                    // minimum distance between label and text equals 0
                    nFlags = ( rFmt.GetFirstLineOffset() == 0 &&
                               rFmt.GetCharTextDistance() == 0 )
                             ? 2 : 0;     // ixchFollow: 0 - tab, 1 - blank, 2 - nothing
                    // <--
                }
                // <--
            }
            else
            {
                if (SVX_NUM_NUMBER_NONE != rFmt.GetNumberingType())
                {
                    BYTE* pLvlPos = aNumLvlPos;
                    // --> OD 2005-10-17 #126238# - the numbering string
                    // has to be restrict to the level currently working on.
                    sNumStr = rRule.MakeNumString(aNumVector, false, true, nLvl);
                    // <--

                    // now search the nums in the string
                    for( BYTE i = 0; i <= nLvl; ++i )
                    {
                        String sSrch( String::CreateFromInt32( i ));
                        xub_StrLen nFnd = sNumStr.Search( sSrch );
                        if( STRING_NOTFOUND != nFnd )
                        {
                            *pLvlPos = (BYTE)(nFnd + rFmt.GetPrefix().Len() + 1 );
                            ++pLvlPos;
                            sNumStr.SetChar( nFnd, (char)i );
                        }
                    }
                    // --> OD 2008-06-03 #i86652#
                    if ( rFmt.GetPositionAndSpaceMode() ==
                                            SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
                    {
                        // --> OD 2007-07-23 #148661#
                        // <nFlags = 2>, if minimum label width equals 0 and
                        // minimum distance between label and text equals 0
                        nFlags = ( rFmt.GetFirstLineOffset() == 0 &&
                                   rFmt.GetCharTextDistance() == 0 )
                                 ? 2 : 0;     // ixchFollow: 0 - tab, 1 - blank, 2 - nothing
                        // <--
                    }
                    // <--
                }

                if( rFmt.GetPrefix().Len() )
                    sNumStr.Insert( rFmt.GetPrefix(), 0 );
                sNumStr += rFmt.GetSuffix();
            }

            // write the rgbxchNums[9]
            pTableStrm->Write(aNumLvlPos, WW8ListManager::nMaxLevel);

            *pTableStrm << nFlags;
            // dxaSoace/dxaIndent (Word 6 compatibility)
            SwWW8Writer::WriteLong( *pTableStrm, 0 );
            SwWW8Writer::WriteLong( *pTableStrm, 0 );

            // cbGrpprlChpx
            WW8Bytes aCharAtrs;
            if( rFmt.GetCharFmt() || bWriteBullet )
            {
                WW8Bytes* pOldpO = pO;
                pO = &aCharAtrs;
                SfxItemSet aSet( pDoc->GetAttrPool(), RES_CHRATR_BEGIN,
                                                      RES_CHRATR_END );
                const SfxItemSet* pOutSet;
                if (bWriteBullet)
                {
                    pOutSet = &aSet;

                    if (rFmt.GetCharFmt())
                        aSet.Put( rFmt.GetCharFmt()->GetAttrSet() );
                    aSet.ClearItem(RES_CHRATR_CJK_FONT);
                    aSet.ClearItem(RES_CHRATR_FONT);

                    if (!sFontName.Len())
                        sFontName = pBulletFont->GetName();

                    wwFont aPseudoFont(sFontName, pBulletFont->GetPitch(),
                        eFamily, eChrSet, bWrtWW8);
                    USHORT nFontID = maFontHelper.GetId(aPseudoFont);

                    if (bWrtWW8)
                    {
                        InsUInt16(0x4a4f);
                        InsUInt16(nFontID);
                        InsUInt16(0x4a51);
                    }
                    else
                        pO->Insert(93, pO->Count());
                    InsUInt16(nFontID);
                }
                else
                    pOutSet = &rFmt.GetCharFmt()->GetAttrSet();

                Out_SfxItemSet(*pOutSet, false, true, i18n::ScriptType::LATIN);

                pO = pOldpO;
            }
            nFlags = (BYTE)aCharAtrs.Count();
            *pTableStrm << nFlags;

            // cbGrpprlPapx
            nFlags = sizeof( aPapSprms );
            *pTableStrm << nFlags;

            // reserved
            SwWW8Writer::WriteShort( *pTableStrm, 0 );

            // --> OD 2008-06-03 #i86652#
            if ( rFmt.GetPositionAndSpaceMode() ==
                                    SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
            {
                const sal_uInt16 nAbsLSpace = rFmt.GetAbsLSpace();
                const sal_Int16 nFirstLineOffset = GetWordFirstLineOffset(rFmt);

                // write Papx
                BYTE* pData = aPapSprms + 2;
                Set_UInt16( pData, nAbsLSpace );
                pData += 2;
                Set_UInt16( pData, nFirstLineOffset );
                pData += 5;
                Set_UInt16( pData, nAbsLSpace );
            }
            else if ( rFmt.GetPositionAndSpaceMode() ==
                                            SvxNumberFormat::LABEL_ALIGNMENT )
            {
                const sal_Int16 nIndentAt = static_cast<sal_Int16>(rFmt.GetIndentAt());
                const sal_Int16 nFirstLineIndenx = static_cast<sal_Int16>(rFmt.GetFirstLineIndent());
                const sal_Int16 nListTabPos =
                        rFmt.GetLabelFollowedBy() == SvxNumberFormat::LISTTAB
                        ? static_cast<sal_Int16>(rFmt.GetListtabPos())
                        : 0;
                // write Papx
                BYTE* pData = aPapSprms + 2;
                Set_UInt16( pData, nIndentAt );
                pData += 2;
                Set_UInt16( pData, nFirstLineIndenx );
                pData += 5;
                Set_UInt16( pData, nListTabPos );
            }

            pTableStrm->Write( aPapSprms, sizeof( aPapSprms ));
            // write Chpx
            if( aCharAtrs.Count() )
                pTableStrm->Write( aCharAtrs.GetData(), aCharAtrs.Count() );

            // write the num string
            SwWW8Writer::WriteShort(*pTableStrm, sNumStr.Len());
            SwWW8Writer::WriteString16(*pTableStrm, sNumStr, false);
        }
    }
    delete pConvert;
}

void SwWW8Writer::OutOverrideListTab()
{
    if( !pUsedNumTbl )
        return ;            // no numbering is used

    // write the "list format override" - LFO
    USHORT nCount = pUsedNumTbl->Count();
    USHORT n;

    pFib->fcPlfLfo = pTableStrm->Tell();
    SwWW8Writer::WriteLong( *pTableStrm, nCount );

    for( n = 0; n < nCount; ++n )
    {
        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );
        SwWW8Writer::FillCount( *pTableStrm, 12 );
    }
    for( n = 0; n < nCount; ++n )
        SwWW8Writer::WriteLong( *pTableStrm, -1 );  // no overwrite

    // set len to FIB
    pFib->lcbPlfLfo = pTableStrm->Tell() - pFib->fcPlfLfo;
}

void SwWW8Writer::OutListNamesTab()
{
    if( !pUsedNumTbl )
        return ;            // no numbering is used

    // write the "list format override" - LFO
    USHORT nNms = 0, nCount = pUsedNumTbl->Count();

    pFib->fcSttbListNames = pTableStrm->Tell();
    SwWW8Writer::WriteShort( *pTableStrm, -1 );
    SwWW8Writer::WriteLong( *pTableStrm, nCount );

    for( ; nNms < nCount; ++nNms )
    {
        const SwNumRule& rRule = *pUsedNumTbl->GetObject( nNms );
        String sNm;
        if( !rRule.IsAutoRule() )
            sNm = rRule.GetName();

        SwWW8Writer::WriteShort( *pTableStrm, sNm.Len() );
        if (sNm.Len())
            SwWW8Writer::WriteString16(*pTableStrm, sNm, false);
    }

    SwWW8Writer::WriteLong( *pTableStrm, pFib->fcSttbListNames + 2, nNms );
    // set len to FIB
    pFib->lcbSttbListNames = pTableStrm->Tell() - pFib->fcSttbListNames;
}

/*  */

// old WW95-Code

void SwWW8Writer::Out_Olst( const SwNumRule& rRule )
{
    static BYTE __READONLY_DATA aAnlvBase[] = { // Defaults
                                1,0,0,          // Upper Roman
                                0x0C,           // Hanging Indent, fPrev
                                0,0,1,0x80,0,0,1,0,0x1b,1,0,0 };

    static BYTE __READONLY_DATA aSprmOlstHdr[] = { 133, 212 };

    pO->Insert( aSprmOlstHdr, sizeof( aSprmOlstHdr ), pO->Count() );
    WW8_OLST aOlst;
    memset( &aOlst, 0, sizeof(aOlst) );
    BYTE* pC = aOlst.rgch;
    BYTE* pChars = (BYTE*)pC;
    USHORT nCharLen = 64;

    for (USHORT j = 0; j < WW8ListManager::nMaxLevel; j++ ) // 9 variable ANLVs
    {
        memcpy( &aOlst.rganlv[j], aAnlvBase, sizeof( WW8_ANLV ) );  // Defaults

        const SwNumFmt* pFmt = rRule.GetNumFmt( j );
        if( pFmt )
            BuildAnlvBase( aOlst.rganlv[j], pChars, nCharLen, rRule,
                            *pFmt, (BYTE)j );
    }

    pO->Insert( (BYTE*)&aOlst, sizeof( aOlst ), pO->Count() );
}


void SwWW8Writer::Out_WwNumLvl( BYTE nWwLevel )
{
    pO->Insert( 13, pO->Count() );
    pO->Insert( nWwLevel, pO->Count() );
}

void SwWW8Writer::Out_SwNumLvl( BYTE nSwLevel )
{
    // --> OD 2008-04-02 #refactorlists#
//    ASSERT(IsNum(nSwLevel), "numbered?");
    ASSERT( nSwLevel < MAXLEVEL, "numbered?");
    // <--
    Out_WwNumLvl( nSwLevel + 1 );
}

void SwWW8Writer::BuildAnlvBulletBase(WW8_ANLV& rAnlv, BYTE*& rpCh,
    USHORT& rCharLen, const SwNumFmt& rFmt)
{
    ByteToSVBT8(11, rAnlv.nfc);

    BYTE nb = 0;                                // Zahlentyp
    switch (rFmt.GetNumAdjust())
    {
        case SVX_ADJUST_RIGHT:
            nb = 2;
            break;
        case SVX_ADJUST_CENTER:
            nb = 1;
            break;
        case SVX_ADJUST_BLOCK:
        case SVX_ADJUST_BLOCKLINE:
            nb = 3;
            break;
        case SVX_ADJUST_LEFT:
        case SVX_ADJUST_END:
            break;
    }

    // --> OD 2008-06-03 #i86652#
    if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
    {
        if (GetWordFirstLineOffset(rFmt) < 0)
            nb |= 0x8;          // number will be displayed using a hanging indent
    }
    // <--
    ByteToSVBT8(nb, rAnlv.aBits1);

    if (1 < rCharLen)
    {
        // --> OD 2006-06-27 #b6440955#
//        const Font& rFont = rFmt.GetBulletFont() ? *rFmt.GetBulletFont()
//            : SwNumRule::GetDefBulletFont();
        const Font& rFont = rFmt.GetBulletFont()
                            ? *rFmt.GetBulletFont()
                            : numfunc::GetDefBulletFont();
        // <--
        String sNumStr = rFmt.GetBulletChar();
        rtl_TextEncoding eChrSet = rFont.GetCharSet();
        String sFontName = rFont.GetName();

        USHORT nFontId;
        if (sw::util::IsStarSymbol(sFontName))
        {
            /*
            If we are starsymbol then in ww7- mode we will always convert to a
            windows 8bit symbol font and an index into it, to conversion to
            8 bit is complete at this stage.
            */
            SubstituteBullet(sNumStr, eChrSet, sFontName);
            wwFont aPseudoFont(sFontName, rFont.GetPitch(), rFont.GetFamily(),
                eChrSet, bWrtWW8);
            nFontId = maFontHelper.GetId(aPseudoFont);
            *rpCh = static_cast<sal_uInt8>(sNumStr.GetChar(0));
        }
        else
        {
            /*
            Otherwise we are a unicode char and need to be converted back to
            an 8 bit format. We happen to know that if the font is already an
            8 bit windows font currently, staroffice promotes the char into
            the F000->F0FF range, so we can undo this, and we'll be back to
            the equivalent 8bit location, otherwise we have to convert from
            true unicode to an 8bit charset
            */
            nFontId = maFontHelper.GetId(rFont);
            sal_Unicode cChar = sNumStr.GetChar(0);
            if ( (eChrSet == RTL_TEXTENCODING_SYMBOL) && (cChar >= 0xF000) && (
                cChar <= 0xF0FF) )
            {
                *rpCh = static_cast< BYTE >(cChar - 0xF000);
            }
            else
                *rpCh =  ByteString::ConvertFromUnicode(cChar, eChrSet);
        }
        rpCh++;
        rCharLen--;
        ShortToSVBT16(nFontId, rAnlv.ftc);
        ByteToSVBT8( 1, rAnlv.cbTextBefore );
    }
    // --> OD 2008-06-03 #i86652#
    if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
    {
        ShortToSVBT16( -GetWordFirstLineOffset(rFmt), rAnlv.dxaIndent );
        ShortToSVBT16( rFmt.GetCharTextDistance(), rAnlv.dxaSpace );
    }
    else
    {
        ShortToSVBT16( 0, rAnlv.dxaIndent );
        ShortToSVBT16( 0, rAnlv.dxaSpace );
    }
    // <--
}

void SwWW8Writer::SubstituteBullet(String& rNumStr,
    rtl_TextEncoding& rChrSet, String& rFontName) const
{
    StarSymbolToMSMultiFont *pConvert = 0;
    FontFamily eFamily = FAMILY_DECORATIVE;

    if (!pConvert)
    {
        pConvert = CreateStarSymbolToMSMultiFont();
#ifdef DUMPSYMBOLS
        ::std::ofstream output("fontdebug");
        for (sal_Unicode i=0xE000;i<0xF8FF;i++)
        {
            String sFont = pConvert->ConvertChar(i);
            if (sFont.Len())
                 output << ::std::hex << i << std::endl;
        }
#endif
    }
    sal_Unicode cChar = rNumStr.GetChar(0);
    String sFont = pConvert->ConvertChar(cChar);

    if (sFont.Len())
    {
        rNumStr = static_cast< sal_Unicode >(cChar | 0xF000);
        rFontName = sFont;
        rChrSet = RTL_TEXTENCODING_SYMBOL;
    }
    else if ( bWrtWW8 &&
        (rNumStr.GetChar(0) < 0xE000 || rNumStr.GetChar(0) > 0xF8FF) )
    {
        /*
        Ok we can't fit into a known windows unicode font, but
        we are not in the private area, so we are a
        standardized symbol, so turn off the symbol bit and
        let words own font substitution kick in
        */
        rChrSet = RTL_TEXTENCODING_UNICODE;
        eFamily = FAMILY_SWISS;
        rFontName = ::GetFontToken(rFontName, 0);
     }
     else
     {
        /*
        Well we don't have an available substition, and we're
        in our private area, so give up and show a standard
        bullet symbol
        */
        rFontName.ASSIGN_CONST_ASC("Wingdings");
        rNumStr = static_cast< sal_Unicode >(0x6C);
     }
     delete pConvert;
}

static void SwWw8_InsertAnlText( const String& rStr, BYTE*& rpCh,
                                 USHORT& rCharLen, SVBT8& r8Len )
{
    BYTE nb = 0;
    WW8Bytes aO;
    SwWW8Writer::InsAsString8( aO, rStr, RTL_TEXTENCODING_MS_1252 );

    USHORT nCnt = aO.Count();
    if( nCnt && nCnt < rCharLen )
    {
        nb = (BYTE)nCnt;
        memcpy( rpCh, aO.GetData(), nCnt );
        rpCh += nCnt;
        rCharLen = rCharLen - nCnt;
    }
    ByteToSVBT8( nb, r8Len );
}

void SwWW8Writer::BuildAnlvBase(WW8_ANLV& rAnlv, BYTE*& rpCh,
    USHORT& rCharLen, const SwNumRule& rRul, const SwNumFmt& rFmt,
    BYTE nSwLevel)
{
    ByteToSVBT8(SwWW8Writer::GetNumId(rFmt.GetNumberingType()), rAnlv.nfc);

    BYTE nb = 0;
    switch (rFmt.GetNumAdjust())
    {
        case SVX_ADJUST_RIGHT:
            nb = 2;
            break;
        case SVX_ADJUST_CENTER:
            nb = 1;
            break;
        case SVX_ADJUST_BLOCK:
        case SVX_ADJUST_BLOCKLINE:
            nb = 3;
            break;
        case SVX_ADJUST_LEFT:
        case SVX_ADJUST_END:
            break;
    }

    bool bInclUpper = rFmt.GetIncludeUpperLevels() > 0;
    if( bInclUpper )
        nb |= 0x4;          // include previous levels

    if (GetWordFirstLineOffset(rFmt) < 0)
        nb |= 0x8;          // number will be displayed using a hanging indent
    ByteToSVBT8( nb, rAnlv.aBits1 );

    if( bInclUpper && !rRul.IsContinusNum() )
    {
        if( (nSwLevel >= WW8ListManager::nMinLevel )
            && (nSwLevel<= WW8ListManager::nMaxLevel )
            && (rFmt.GetNumberingType() != SVX_NUM_NUMBER_NONE ) )  // UEberhaupt Nummerierung ?
        {                                               // -> suche, ob noch Zahlen davor
            BYTE nUpper = rFmt.GetIncludeUpperLevels();
            if( (nUpper <= WW8ListManager::nMaxLevel) &&
                (rRul.Get(nUpper).GetNumberingType() != SVX_NUM_NUMBER_NONE ) ) // Nummerierung drueber ?
            {
                                                    // dann Punkt einfuegen
                SwWw8_InsertAnlText( aDotStr, rpCh, rCharLen,
                                    rAnlv.cbTextBefore );
            }

        }
    }
    else
    {
        SwWw8_InsertAnlText( rFmt.GetPrefix(), rpCh, rCharLen,
                             rAnlv.cbTextBefore );
        SwWw8_InsertAnlText( rFmt.GetSuffix(), rpCh, rCharLen,
                             rAnlv.cbTextAfter );
    }

    ShortToSVBT16( rFmt.GetStart(), rAnlv.iStartAt );
    // --> OD 2008-06-03 #i86652#
    if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
    {
        ShortToSVBT16( -GetWordFirstLineOffset(rFmt), rAnlv.dxaIndent );
        ShortToSVBT16( rFmt.GetCharTextDistance(), rAnlv.dxaSpace );
    }
    else
    {
        ShortToSVBT16( 0, rAnlv.dxaIndent );
        ShortToSVBT16( 0, rAnlv.dxaSpace );
    }
    // <--
}

void SwWW8Writer::Out_NumRuleAnld( const SwNumRule& rRul, const SwNumFmt& rFmt,
                                   BYTE nSwLevel )
{
    static BYTE __READONLY_DATA aSprmAnldDefault[54] = {
                         12, 52,
                         1,0,0,0x0c,0,0,1,0x80,0,0,1,0,0x1b,1,0,0,0x2e,
                         0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    BYTE aSprmAnld[54];

    memcpy( aSprmAnld, aSprmAnldDefault, sizeof( aSprmAnld ) );
    WW8_ANLD* pA = (WW8_ANLD*)(aSprmAnld + 2);  // handlicher Pointer

    BYTE* pChars = (BYTE*)(pA->rgchAnld);
    USHORT nCharLen = 31;

    if( nSwLevel == 11 )
        BuildAnlvBulletBase( pA->eAnlv, pChars, nCharLen, rFmt );
    else
        BuildAnlvBase( pA->eAnlv, pChars, nCharLen, rRul, rFmt, nSwLevel );

    // ... und raus damit
    OutSprmBytes( (BYTE*)&aSprmAnld, sizeof( aSprmAnld ) );
}


// Return: ist es eine Gliederung ?
bool SwWW8Writer::Out_SwNum(const SwTxtNode* pNd)
{
    int nLevel = pNd->GetActualListLevel();

    if (nLevel < 0 || nLevel >= MAXLEVEL)
    {
        ASSERT(FALSE, "Invalid level");

        return false;
    }

    BYTE nSwLevel = static_cast< BYTE >(nLevel);

    const SwNumRule* pRul = pNd->GetNumRule();
    if( !pRul || nSwLevel == WW8ListManager::nMaxLevel)
        return false;

    bool bRet = true;

    SwNumFmt aFmt(pRul->Get(nSwLevel));
    // --> OD 2008-06-03 #i86652#
    if ( aFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
    {
        const SvxLRSpaceItem& rLR = ItemGet<SvxLRSpaceItem>(*pNd, RES_LR_SPACE);
        aFmt.SetAbsLSpace(writer_cast<short>(aFmt.GetAbsLSpace() + rLR.GetLeft()));
    }

    if (
         aFmt.GetNumberingType() == SVX_NUM_NUMBER_NONE ||
         aFmt.GetNumberingType() == SVX_NUM_CHAR_SPECIAL ||
         aFmt.GetNumberingType() == SVX_NUM_BITMAP
       )
    {
        // Aufzaehlung
        // --> OD 2008-04-02 #refactorlists#
//        Out_WwNumLvl(bNoNum ? 12 : 11);
        Out_WwNumLvl(11);
        // <--
        Out_NumRuleAnld(*pRul, aFmt, 11);
        bRet = false;
    }
    else if (
              pRul->IsContinusNum() ||
              (pRul->Get(1).GetIncludeUpperLevels() <= 1)
            )
    {
        // Nummerierung
        // --> OD 2008-04-02 #refactorlists#
//        Out_WwNumLvl(bNoNum ? 12 : 10);
        Out_WwNumLvl(10);
        // <--
        Out_NumRuleAnld(*pRul, aFmt, 10);
        bRet = false;
    }
    else
    {
        // Gliederung
        // --> OD 2008-04-02 #refactorlists#
//        Out_SwNumLvl(bNoNum ? 12 : nSwLevel);
        Out_SwNumLvl(nSwLevel);
        // <--
        Out_NumRuleAnld(*pRul, aFmt, nSwLevel);
    }
    return bRet;
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
