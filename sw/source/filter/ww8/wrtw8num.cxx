/*************************************************************************
 *
 *  $RCSfile: wrtw8num.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: cmc $ $Date: 2002-07-26 12:29:09 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SV_FONT_HXX //autogen
#include <vcl/font.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen wg. SvxLRSpaceItem
#include <svx/lrspitem.hxx>
#endif

#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX //autogen
#include <docary.hxx>
#endif
#ifndef _NUMRULE_HXX //autogen
#include <numrule.hxx>
#endif
#ifndef _PARATR_HXX //autogen
#include <paratr.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _NDTXT_HXX //autogen wg. SwTxtNode
#include <ndtxt.hxx>
#endif
#ifndef _WRTWW8_HXX
#include <wrtww8.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif

//#define DUMPSYMBOLS
#ifdef DUMPSYMBOLS
#include <fstream>
#endif

using namespace ::com::sun::star::i18n;

USHORT SwWW8Writer::DupNumRuleWithLvlStart(const SwNumRule *pRule,BYTE nLvl, 
    USHORT nVal)
{
    //List is set to restart at a particular value so for export make a 
    //completely new list based on this one and export that instead, 
    //which duplicates words behaviour in this respect.
    USHORT nNumId = USHRT_MAX;
    String sPrefix(CREATE_CONST_ASC("WW8TempExport"));
    sPrefix += String::CreateFromInt32( nUniqueList++ );
    SwNumRule* pMyNumRule = new SwNumRule(pDoc->GetUniqueNumRuleName(&sPrefix));
    pUsedNumTbl->Insert( pMyNumRule, pUsedNumTbl->Count() );

    for (int i=0;i<MAXLEVEL;i++)
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
        for( USHORT n = pUsedNumTbl->Count(); n; )
            if( !pDoc->IsUsed( *pUsedNumTbl->GetObject( --n )) )
                pThis->pUsedNumTbl->Remove( n );

        // jetzt noch die OutlineRule einfuegen
        SwNumRule* pR = (SwNumRule*)pDoc->GetOutlineNumRule();
        pThis->pUsedNumTbl->Insert( pR, pUsedNumTbl->Count() );
    }
    SwNumRule* p = (SwNumRule*)&rNumRule;
    USHORT nRet = pUsedNumTbl->GetPos(p);

    //Is this list now duplicated into a new list which we should use
    ::std::map<USHORT,USHORT>::const_iterator aResult =
        aRuleDuplicates.find(nRet);
    if (aResult != aRuleDuplicates.end())
        nRet = (*aResult).second;

    return nRet;
}

void SwWW8Writer::OutListTab()
{
    if( !pUsedNumTbl )
        return ;			// no numbering is used

    USHORT nCount = pUsedNumTbl->Count();

    pFib->fcPlcfLst = pTableStrm->Tell();
    SwWW8Writer::WriteShort( *pTableStrm, nCount );

    // First Loop - write static data of SwNumRule - LSTF
    for( USHORT n = 0; n < nCount; ++n )
    {
        const SwNumRule& rRule = *pUsedNumTbl->GetObject( n );

        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );
        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );

        // mit keinen Styles verbunden
        for( int i = 0; i < nWW8MaxListLevel; ++i )
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
    SwNodeNum aNdNum( 0 );
    for( n = 0; n < nWW8MaxListLevel; ++n )
        aNdNum.GetLevelVal()[ n ] = n;

    BYTE aPapSprms [] = {
        0x0f, 0x84, 0, 0, 				// sprmPDxaLeft
        0x11, 0x84, 0, 0				// sprmPDxaLeft1
    };

    StarSymbolToMSMultiFont *pConvert = 0;
    for( n = 0; n < nCount; ++n )
    {
        const SwNumRule& rRule = *pUsedNumTbl->GetObject( n );
        BYTE nLvl, nFlags,
            nLevels = rRule.IsContinusNum() ? 1 : nWW8MaxListLevel;
        for( nLvl = 0; nLvl < nLevels; ++nLvl )
        {
            // write the static data of the SwNumFmt of this level
            BYTE aNumLvlPos[ nWW8MaxListLevel ] = { 0,0,0,0,0,0,0,0,0 };

            const SwNumFmt& rFmt = rRule.Get( nLvl );
            SwWW8Writer::WriteLong( *pTableStrm, rFmt.GetStart() );
            *pTableStrm << SwWW8Writer::GetNumId( rFmt.GetNumberingType() );

            switch( rFmt.GetNumAdjust() )
            {
            case SVX_ADJUST_CENTER:	nFlags = 1;	break;
            case SVX_ADJUST_RIGHT:	nFlags = 2;	break;
            default:				nFlags = 0;	break;
            }
            *pTableStrm << nFlags;

            // Build the NumString for this Level
            String sNumStr;
            String sFontName;
            BOOL bWriteBullet = FALSE;
            const Font* pBulletFont=0;
            rtl_TextEncoding eChrSet=0;
            FontFamily eFamily=FAMILY_DECORATIVE;
            if( SVX_NUM_CHAR_SPECIAL == rFmt.GetNumberingType() ||
                SVX_NUM_BITMAP == rFmt.GetNumberingType() )
            {
                sNumStr = rFmt.GetBulletChar();
                bWriteBullet = TRUE;

                pBulletFont = rFmt.GetBulletFont();
                if( !pBulletFont )
                    pBulletFont = &SwNumRule::GetDefBulletFont();

                eChrSet = pBulletFont->GetCharSet();
                sFontName = pBulletFont->GetName();
                eFamily = pBulletFont->GetFamily();

                if (wwFont::IsStarSymbol(sFontName))
                {
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
                    const char *pName=0;
                    sal_Unicode cChar = sNumStr.GetChar(0);
                    String sFont = pConvert->ConvertChar(cChar);
                    if (sFont.Len())
                    {
                        sNumStr = cChar;
                        sFontName = sFont;
                        eChrSet = RTL_TEXTENCODING_SYMBOL;
                    }
                    else if (sNumStr.GetChar(0) < 0xE000 || 
                        sNumStr.GetChar(0) > 0xF8FF)
                    {
                        /*
                        Ok we can't fit into a known windows unicode font, but
                        we are not in the private area, so we are a
                        standardized symbol, so turn off the symbol bit and
                        let words own font substitution kick in
                        */
                        eChrSet = RTL_TEXTENCODING_UNICODE;
                        eFamily = FAMILY_SWISS;
                        sFontName = ::GetFontToken(sFontName, 0);
                    }
                    else
                    {
                        /*
                        Well we don't have an available substition, and we're
                        in our private area, so give up and show a standard
                        bullet symbol
                        */
                        sFontName.ASSIGN_CONST_ASC("Wingdings");
                        sNumStr = 0x6C;
                    }
                }
                else if (eChrSet == RTL_TEXTENCODING_SYMBOL)
                {
                    sal_Unicode cChar = sNumStr.GetChar(0);
                    if (cChar>= 0xF000 && cChar<=0xF0FF)
                        sNumStr.SetChar(0, cChar-0xF000);
                }
            }
            else
            {
                if (SVX_NUM_NUMBER_NONE != rFmt.GetNumberingType())
                {
                    BYTE* pLvlPos = aNumLvlPos;
                    aNdNum.SetLevel( nLvl );
                    sNumStr = rRule.MakeNumString( aNdNum, FALSE, TRUE );

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
                }

                if( rFmt.GetPrefix().Len() )
                    sNumStr.Insert( rFmt.GetPrefix(), 0 );
                sNumStr += rFmt.GetSuffix();
            }

            // write the rgbxchNums[9]
            pTableStrm->Write( aNumLvlPos, nWW8MaxListLevel );

            nFlags = 2;		// ixchFollow: 0 - tab, 1 - blank, 2 - nothing
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
#if 0
                    if (bWrtWW8)
                    {
                        InsUInt16(0x286F);
                        pO->Insert(01, pO->Count());
                    }
#endif
                }
                else
                    pOutSet = &rFmt.GetCharFmt()->GetAttrSet();

                Out_SfxItemSet( *pOutSet, FALSE, TRUE, ScriptType::LATIN );

                pO = pOldpO;
            }
            nFlags = (BYTE)aCharAtrs.Count();
            *pTableStrm << nFlags;

            // cbGrpprlPapx
            nFlags = sizeof( aPapSprms );
            *pTableStrm << nFlags;

            // reserved
            SwWW8Writer::WriteShort( *pTableStrm, 0 );

            // write Papx
            BYTE* pData = aPapSprms + 2;
            Set_UInt16( pData, rFmt.GetAbsLSpace() );
            pData += 2;
            Set_UInt16( pData, rFmt.GetFirstLineOffset() );
            pTableStrm->Write( aPapSprms, sizeof( aPapSprms ));

            // write Chpx
            if( aCharAtrs.Count() )
                pTableStrm->Write( aCharAtrs.GetData(), aCharAtrs.Count() );

            // write the num string
            SwWW8Writer::WriteShort( *pTableStrm, sNumStr.Len() );
            SwWW8Writer::WriteString16( *pTableStrm, sNumStr, FALSE );
        }
    }
    delete pConvert;
}

void SwWW8Writer::OutOverrideListTab()
{
    if( !pUsedNumTbl )
        return ;			// no numbering is used

    // write the "list format override" - LFO
    USHORT nCount = pUsedNumTbl->Count();

    pFib->fcPlfLfo = pTableStrm->Tell();
    SwWW8Writer::WriteLong( *pTableStrm, nCount );

    for( USHORT n = 0; n < nCount; ++n )
    {
        SwWW8Writer::WriteLong( *pTableStrm, n + 1 );
        SwWW8Writer::FillCount( *pTableStrm, 12 );
    }
    for( n = 0; n < nCount; ++n )
        SwWW8Writer::WriteLong( *pTableStrm, -1 );	// no overwrite

    // set len to FIB
    pFib->lcbPlfLfo = pTableStrm->Tell() - pFib->fcPlfLfo;
}

void SwWW8Writer::OutListNamesTab()
{
    if( !pUsedNumTbl )
        return ;			// no numbering is used

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
        if( sNm.Len() )
            SwWW8Writer::WriteString16( *pTableStrm, sNm, FALSE );
    }

    SwWW8Writer::WriteLong( *pTableStrm, pFib->fcSttbListNames + 2, nNms );
    // set len to FIB
    pFib->lcbSttbListNames = pTableStrm->Tell() - pFib->fcSttbListNames;
}

/*  */

// old WW95-Code

void SwWW8Writer::Out_Olst( const SwNumRule& rRule )
{
    static BYTE __READONLY_DATA aAnlvBase[] = {	// Defaults
                                1,0,0,     		// Upper Roman
                                0x0C,			// Hanging Indent, fPrev
                                0,0,1,0x80,0,0,1,0,0x1b,1,0,0 };

    static BYTE __READONLY_DATA aSprmOlstHdr[] = { 133, 212 };

    pO->Insert( aSprmOlstHdr, sizeof( aSprmOlstHdr ), pO->Count() );
    WW8_OLST aOlst;
    memset( &aOlst, 0, sizeof(aOlst) );
    BYTE* pC = aOlst.rgch;
    BYTE* pChars = (BYTE*)pC;
    USHORT nCharLen = 64;

    USHORT j;
    for( j = 0; j < nWW8MaxListLevel; j++ )	// 9 variable ANLVs
    {
        memcpy( &aOlst.rganlv[j], aAnlvBase, sizeof( WW8_ANLV ) );	// Defaults

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
    Out_WwNumLvl( ( nSwLevel == NO_NUM ) ? 0 : nSwLevel + 1 );
}

void SwWW8Writer::BuildAnlvBulletBase( WW8_ANLV& rAnlv, BYTE*& rpCh,
                                    USHORT& rCharLen, const SwNumFmt& rFmt )
{
    BYTE nb = 0;								// Zahlentyp
    ByteToSVBT8( 11, rAnlv.nfc );

    switch( rFmt.GetNumAdjust() )
    {
    case SVX_ADJUST_RIGHT: 		nb = 2; break;
    case SVX_ADJUST_CENTER: 	nb = 1; break;
    case SVX_ADJUST_BLOCK:
    case SVX_ADJUST_BLOCKLINE: 	nb = 3; break;
    }
    if( rFmt.GetFirstLineOffset() < 0 )
        nb |= 0x8;			// number will be displayed using a hanging indent
    ByteToSVBT8( nb, rAnlv.aBits1 );

    if( 1 < rCharLen )
    {
        const Font& rFont = rFmt.GetBulletFont() ? *rFmt.GetBulletFont()
            : SwNumRule::GetDefBulletFont();

        USHORT nFontId = maFontHelper.GetId(rFont);
        ShortToSVBT16( nFontId, rAnlv.ftc );
        *rpCh = ByteString::ConvertFromUnicode( rFmt.GetBulletChar(),
            rFont.GetCharSet() );

        rpCh++;
        rCharLen--;
        ByteToSVBT8( 1, rAnlv.cbTextBefore );
    }
    ShortToSVBT16( -rFmt.GetFirstLineOffset(), rAnlv.dxaIndent );
    ShortToSVBT16( rFmt.GetCharTextDistance(), rAnlv.dxaSpace );
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
        rCharLen -= nCnt;
    }
    ByteToSVBT8( nb, r8Len );
}

void SwWW8Writer::BuildAnlvBase( WW8_ANLV& rAnlv, BYTE*& rpCh,
                                USHORT& rCharLen, const SwNumRule& rRul,
                                const SwNumFmt& rFmt, BYTE nSwLevel )
{
    ByteToSVBT8( SwWW8Writer::GetNumId( rFmt.GetNumberingType() ), rAnlv.nfc );

    BYTE nb = 0;
    switch( rFmt.GetNumAdjust() )
    {
    case SVX_ADJUST_RIGHT: nb = 2; break;
    case SVX_ADJUST_CENTER: nb = 1; break;
    case SVX_ADJUST_BLOCK:
    case SVX_ADJUST_BLOCKLINE: nb = 3; break;
    }

    BOOL bInclUpper = rFmt.GetIncludeUpperLevels() > 0;
    if( bInclUpper )
        nb |= 0x4;			// include previous levels

    if( rFmt.GetFirstLineOffset() < 0 )
        nb |= 0x8;			// number will be displayed using a hanging indent
    ByteToSVBT8( nb, rAnlv.aBits1 );

    if( bInclUpper && !rRul.IsContinusNum() )
    {
        if( (nSwLevel >= 1 )
            && (nSwLevel<= nWW8MaxListLevel )
            && (rFmt.GetNumberingType() != SVX_NUM_NUMBER_NONE ) )	// UEberhaupt Nummerierung ?
        {												// -> suche, ob noch Zahlen davor
            BYTE nUpper = rFmt.GetIncludeUpperLevels();
            if( (nUpper >= 0 )
                && (nUpper <= nWW8MaxListLevel )
                && (rRul.Get(nUpper).GetNumberingType() != SVX_NUM_NUMBER_NONE ) )	// Nummerierung drueber ?
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
    ShortToSVBT16( -rFmt.GetFirstLineOffset(), rAnlv.dxaIndent );
    ShortToSVBT16( rFmt.GetCharTextDistance(), rAnlv.dxaSpace );
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
    WW8_ANLD* pA = (WW8_ANLD*)(aSprmAnld + 2);	// handlicher Pointer

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
BOOL SwWW8Writer::Out_SwNum( const SwTxtNode* pNd )
{
    BYTE nSwLevel = pNd->GetNum()->GetLevel();
    const SwNumRule* pRul = pNd->GetNumRule();
    if( !pRul || nSwLevel == nWW8MaxListLevel )
        return FALSE;

    BOOL bNoNum = FALSE;
    if( nSwLevel == NO_NUM )
        nSwLevel = NO_NUMLEVEL | 0 ;	// alte Codierung...
    if( ( nSwLevel & NO_NUMLEVEL ) != 0 )
    {
        nSwLevel &= ~NO_NUMLEVEL;		// 0..nWW8MaxListLevel
        bNoNum = TRUE;
    }

    BOOL bRet = TRUE;
    const SwNumFmt* pFmt = &pRul->Get( nSwLevel );// interessierendes Format

#ifdef NUM_RELSPACE
    SwNumFmt aFmt( *pFmt );
    const SvxLRSpaceItem& rLR = (SvxLRSpaceItem&)pNd->SwCntntNode::GetAttr( 
        RES_LR_SPACE );
    aFmt.SetAbsLSpace( aFmt.GetAbsLSpace() + rLR.GetLeft() );
    pFmt = &aFmt;
#endif

    if( pFmt->GetNumberingType() == SVX_NUM_NUMBER_NONE
        || pFmt->GetNumberingType() == SVX_NUM_CHAR_SPECIAL
        || pFmt->GetNumberingType() == SVX_NUM_BITMAP )
    {
                            // Aufzaehlung
        Out_WwNumLvl( (bNoNum) ? 12 : 11 );
        Out_NumRuleAnld( *pRul, *pFmt, 11 );
        bRet = FALSE;
    }
    else if( pRul->IsContinusNum()
              || ( pRul->Get(1).GetIncludeUpperLevels() <= 1 ) )
    {
                            // Nummerierung
        Out_WwNumLvl( (bNoNum) ? 12 : 10 );
        Out_NumRuleAnld( *pRul, *pFmt, 10 );
        bRet = FALSE;
    }
    else
    {
                            // Gliederung
        Out_SwNumLvl( (bNoNum) ? 12 :nSwLevel );
        Out_NumRuleAnld( *pRul, *pFmt, nSwLevel );
    }
    return bRet;
}


