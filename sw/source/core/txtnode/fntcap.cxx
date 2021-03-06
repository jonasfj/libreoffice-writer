/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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


#include <hintids.hxx>
#include <editeng/cmapitem.hxx>

#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_CHARTYPE_HDL
#include <com/sun/star/i18n/CharType.hdl>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HDL
#include <com/sun/star/i18n/WordType.hdl>
#endif

#ifndef _PRINT_HXX //autogen
#include <vcl/print.hxx>
#endif
#include <errhdl.hxx>
#include <fntcache.hxx>
#include <swfont.hxx>
#include <breakit.hxx>
#include <txtfrm.hxx>       // SwTxtFrm
#include <scriptinfo.hxx>

using namespace ::com::sun::star::i18n;


#define KAPITAELCHENPROP 66

/*************************************************************************
 *						class SwCapitalInfo
 *
 * The information encapsulated in SwCapitalInfo is required
 * by the ::Do functions. They contain the information about
 * the original string, whereas rDo.GetInf() contains information
 * about the display string.
 *************************************************************************/

class SwCapitalInfo
{
public:
    explicit SwCapitalInfo( const XubString& rOrigText ) :
        rString( rOrigText ), nIdx( 0 ), nLen( 0 ) {};
    const XubString& rString;
    xub_StrLen nIdx;
    xub_StrLen nLen;
};

/*************************************************************************
 *						xub_StrLen lcl_CalcCaseMap()
 *
 * rFnt: required for CalcCaseMap
 * rOrigString: The original string
 * nOfst: Position of the substring in rOrigString
 * nLen: Length if the substring in rOrigString
 * nIdx: Referes to a position in the display string and should be mapped
 *       to a position in rOrigString
 *************************************************************************/

xub_StrLen lcl_CalcCaseMap( const SwFont& rFnt,
                            const XubString& rOrigString,
                            xub_StrLen nOfst,
                            xub_StrLen nLen,
                            xub_StrLen nIdx )
{
    int j = 0;
    const xub_StrLen nEnd = nOfst + nLen;
    ASSERT( nEnd <= rOrigString.Len(), "lcl_CalcCaseMap: Wrong parameters" )

    // special case for title case:
    const bool bTitle = SVX_CASEMAP_TITEL == rFnt.GetCaseMap() &&
                        pBreakIt->GetBreakIter().is();
    for ( xub_StrLen i = nOfst; i < nEnd; ++i )
    {
        XubString aTmp( rOrigString, i, 1 );

        if ( !bTitle ||
             pBreakIt->GetBreakIter()->isBeginWord(
                 rOrigString, i,
                 pBreakIt->GetLocale( rFnt.GetLanguage() ),
                 WordType::ANYWORD_IGNOREWHITESPACES ) )
            aTmp = rFnt.GetActualFont().CalcCaseMap( aTmp );

        j += aTmp.Len();

        if ( j > nIdx )
            return i;
    }

    return nOfst + nLen;
}

/*************************************************************************
 *						class SwDoCapitals
 *************************************************************************/

class SwDoCapitals
{
protected:
    SwDrawTextInfo &rInf;
    SwCapitalInfo* pCapInf; // referes to additional information
                           // required by the ::Do function
public:
    SwDoCapitals ( SwDrawTextInfo &rInfo ) : rInf( rInfo ), pCapInf( 0 ) { }
    virtual void Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont ) = 0;
    virtual void Do() = 0;
    inline OutputDevice& GetOut() { return rInf.GetOut(); }
    inline SwDrawTextInfo& GetInf() { return rInf; }
    inline SwCapitalInfo* GetCapInf() const { return pCapInf; }
    inline void SetCapInf( SwCapitalInfo& rNew ) { pCapInf = &rNew; }
};

/*************************************************************************
 *					  class SwDoGetCapitalSize
 *************************************************************************/

class SwDoGetCapitalSize : public SwDoCapitals
{
protected:
    Size aTxtSize;
public:
    SwDoGetCapitalSize( SwDrawTextInfo &rInfo ) : SwDoCapitals ( rInfo ) { }
    virtual void Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont );
    virtual void Do();
    const Size &GetSize() const { return aTxtSize; }
};

void SwDoGetCapitalSize::Init( SwFntObj *, SwFntObj * )
{
    aTxtSize.Height() = 0;
    aTxtSize.Width() = 0;
}

void SwDoGetCapitalSize::Do()
{
    aTxtSize.Width() += rInf.GetSize().Width();
    if( rInf.GetUpper() )
        aTxtSize.Height() = rInf.GetSize().Height();
}

/*************************************************************************
 *					  SwSubFont::GetCapitalSize()
 *************************************************************************/

Size SwSubFont::GetCapitalSize( SwDrawTextInfo& rInf )
{
    // Start:
    const long nOldKern = rInf.GetKern();
    rInf.SetKern( CheckKerning() );
    Point aPos;
    rInf.SetPos( aPos );
    rInf.SetSpace( 0 );
    rInf.SetDrawSpace( FALSE );
    SwDoGetCapitalSize aDo( rInf );
    DoOnCapitals( aDo );
    Size aTxtSize( aDo.GetSize() );

    // End:
    if( !aTxtSize.Height() )
    {
        SV_STAT( nGetTextSize );
        aTxtSize.Height() = short ( rInf.GetpOut()->GetTextHeight() );
    }
    rInf.SetKern( nOldKern );
    return aTxtSize;
}

/*************************************************************************
 *					  class SwDoGetCapitalBreak
 *************************************************************************/

class SwDoGetCapitalBreak : public SwDoCapitals
{
protected:
    xub_StrLen *pExtraPos;
    long nTxtWidth;
    xub_StrLen nBreak;
public:
    SwDoGetCapitalBreak( SwDrawTextInfo &rInfo, long nWidth, xub_StrLen *pExtra)
        :   SwDoCapitals ( rInfo ), pExtraPos( pExtra ), nTxtWidth( nWidth ),
            nBreak( STRING_LEN )
        { }
    virtual void Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont );
    virtual void Do();
    xub_StrLen GetBreak() const { return nBreak; }
};

void SwDoGetCapitalBreak::Init( SwFntObj *, SwFntObj * )
{
}

void SwDoGetCapitalBreak::Do()
{
    if ( nTxtWidth )
    {
        if ( rInf.GetSize().Width() < nTxtWidth )
            nTxtWidth -= rInf.GetSize().Width();
        else
        {
            xub_StrLen nEnd = rInf.GetEnd();
            if( pExtraPos )
            {
                nBreak = GetOut().GetTextBreak( rInf.GetText(), nTxtWidth, '-',
                     *pExtraPos, rInf.GetIdx(), rInf.GetLen(), rInf.GetKern() );
                if( *pExtraPos > nEnd )
                    *pExtraPos = nEnd;
            }
            else
                nBreak = GetOut().GetTextBreak( rInf.GetText(), nTxtWidth,
                               rInf.GetIdx(), rInf.GetLen(), rInf.GetKern() );

            if( nBreak > nEnd )
                nBreak = nEnd;

            // nBreak may be relative to the display string. It has to be
            // calculated relative to the original string:
            if ( GetCapInf()  )
            {
                if ( GetCapInf()->nLen != rInf.GetLen() )
                    nBreak = lcl_CalcCaseMap( *rInf.GetFont(),
                                              GetCapInf()->rString,
                                              GetCapInf()->nIdx,
                                              GetCapInf()->nLen, nBreak );
                else
                    nBreak = nBreak + GetCapInf()->nIdx;
            }

            nTxtWidth = 0;
        }
    }
}

/*************************************************************************
 *					  SwFont::GetCapitalBreak()
 *************************************************************************/

xub_StrLen SwFont::GetCapitalBreak( ViewShell* pSh, const OutputDevice* pOut,
    const SwScriptInfo* pScript, const XubString& rTxt, long nTextWidth,
    xub_StrLen *pExtra,	const xub_StrLen nIdx, const xub_StrLen nLen )
{
    // Start:
    Point aPos( 0, 0 );
    SwDrawTextInfo aInfo(pSh, *(OutputDevice*)pOut, pScript, rTxt, nIdx, nLen,
        0, FALSE);
    aInfo.SetPos( aPos );
    aInfo.SetSpace( 0 );
    aInfo.SetWrong( NULL );
    aInfo.SetGrammarCheck( NULL );
    aInfo.SetSmartTags( NULL ); // SMARTTAGS
    aInfo.SetDrawSpace( FALSE );
    aInfo.SetKern( CheckKerning() );
    aInfo.SetKanaComp( pScript ? 0 : 100 );
    aInfo.SetFont( this );

    SwDoGetCapitalBreak aDo( aInfo, nTextWidth, pExtra );
    DoOnCapitals( aDo );
    return aDo.GetBreak();
}

/*************************************************************************
 *					   class SwDoDrawCapital
 *************************************************************************/

class SwDoDrawCapital : public SwDoCapitals
{
protected:
    SwFntObj *pUpperFnt;
    SwFntObj *pLowerFnt;
public:
    SwDoDrawCapital( SwDrawTextInfo &rInfo ) :
        SwDoCapitals( rInfo )
        { }
    virtual void Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont );
    virtual void Do();
    void DrawSpace( Point &rPos );
};

void SwDoDrawCapital::Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont )
{
    pUpperFnt = pUpperFont;
    pLowerFnt = pLowerFont;
}

void SwDoDrawCapital::Do()
{
    SV_STAT( nDrawText );
    USHORT nOrgWidth = rInf.GetWidth();
    rInf.SetWidth( USHORT(rInf.GetSize().Width()) );
    if ( rInf.GetUpper() )
        pUpperFnt->DrawText( rInf );
    else
    {
        BOOL bOldBullet = rInf.GetBullet();
        rInf.SetBullet( FALSE );
        pLowerFnt->DrawText( rInf );
        rInf.SetBullet( bOldBullet );
    }

    ASSERT( pUpperFnt, "No upper font, dying soon!");
    rInf.Shift( pUpperFnt->GetFont()->GetOrientation() );
    rInf.SetWidth( nOrgWidth );
}

/*************************************************************************
 *					  SwDoDrawCapital::DrawSpace()
 *************************************************************************/

void SwDoDrawCapital::DrawSpace( Point &rPos )
{
    static sal_Char __READONLY_DATA sDoubleSpace[] = "  ";

    long nDiff = rInf.GetPos().X() - rPos.X();

    Point aPos( rPos );
    const BOOL bSwitchL2R = rInf.GetFrm()->IsRightToLeft() &&
                          ! rInf.IsIgnoreFrmRTL();


    if ( bSwitchL2R )
       rInf.GetFrm()->SwitchLTRtoRTL( aPos );

    const ULONG nMode = rInf.GetpOut()->GetLayoutMode();
    const BOOL bBidiPor = ( bSwitchL2R !=
                            ( 0 != ( TEXT_LAYOUT_BIDI_RTL & nMode ) ) );

    if ( bBidiPor )
        nDiff = -nDiff;

    if ( rInf.GetFrm()->IsVertical() )
        rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

    if ( nDiff )
    {
        rInf.ApplyAutoColor();
        GetOut().DrawStretchText( aPos, nDiff,
            XubString( sDoubleSpace, RTL_TEXTENCODING_MS_1252 ), 0, 2 );
    }
    rPos.X() = rInf.GetPos().X() + rInf.GetWidth();
}

/*************************************************************************
 *					  SwSubFont::DrawCapital()
 *************************************************************************/

void SwSubFont::DrawCapital( SwDrawTextInfo &rInf )
{
    // Es wird vorausgesetzt, dass rPos bereits kalkuliert ist!
    // hochgezogen in SwFont: const Point aPos( CalcPos(rPos) );
    rInf.SetDrawSpace( GetUnderline() != UNDERLINE_NONE ||
                       GetOverline()  != UNDERLINE_NONE ||
                       GetStrikeout() != STRIKEOUT_NONE );
    SwDoDrawCapital aDo( rInf );
    DoOnCapitals( aDo );
}

/*************************************************************************
 *					   class SwDoDrawCapital
 *************************************************************************/

class SwDoCapitalCrsrOfst : public SwDoCapitals
{
protected:
    SwFntObj *pUpperFnt;
    SwFntObj *pLowerFnt;
    xub_StrLen nCrsr;
    USHORT nOfst;
public:
    SwDoCapitalCrsrOfst( SwDrawTextInfo &rInfo, const USHORT nOfs ) :
        SwDoCapitals( rInfo ), nCrsr( 0 ), nOfst( nOfs )
        { }
    virtual void Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont );
    virtual void Do();

    void DrawSpace( const Point &rPos );
    inline xub_StrLen GetCrsr(){ return nCrsr; }
};

void SwDoCapitalCrsrOfst::Init( SwFntObj *pUpperFont, SwFntObj *pLowerFont )
{
    pUpperFnt = pUpperFont;
    pLowerFnt = pLowerFont;
}

void SwDoCapitalCrsrOfst::Do()
{
    if ( nOfst )
    {
        if ( nOfst > rInf.GetSize().Width() )
        {
            nOfst = nOfst - USHORT(rInf.GetSize().Width());
            nCrsr = nCrsr + rInf.GetLen();
        }
        else
        {
            SwDrawTextInfo aDrawInf( rInf.GetShell(), *rInf.GetpOut(),
                                     rInf.GetScriptInfo(),
                                     rInf.GetText(),
                                     rInf.GetIdx(),
                                     rInf.GetLen(), 0, FALSE );
            aDrawInf.SetOfst( nOfst );
            aDrawInf.SetKern( rInf.GetKern() );
            aDrawInf.SetKanaComp( rInf.GetKanaComp() );
            aDrawInf.SetFrm( rInf.GetFrm() );
            aDrawInf.SetFont( rInf.GetFont() );

            if ( rInf.GetUpper() )
            {
                aDrawInf.SetSpace( 0 );
                nCrsr = nCrsr + pUpperFnt->GetCrsrOfst( aDrawInf );
            }
            else
            {
                aDrawInf.SetSpace( rInf.GetSpace() );
                nCrsr = nCrsr + pLowerFnt->GetCrsrOfst( aDrawInf );
            }
            nOfst = 0;
        }
    }
}

/*************************************************************************
 *					  SwSubFont::GetCapitalCrsrOfst()
 *************************************************************************/

xub_StrLen SwSubFont::GetCapitalCrsrOfst( SwDrawTextInfo& rInf )
{
    const long nOldKern = rInf.GetKern();
    rInf.SetKern( CheckKerning() );
    SwDoCapitalCrsrOfst aDo( rInf, rInf.GetOfst() );
    Point aPos;
    rInf.SetPos( aPos );
    rInf.SetDrawSpace( FALSE );
    DoOnCapitals( aDo );
    rInf.SetKern( nOldKern );
    return aDo.GetCrsr();
}

/*************************************************************************
 *					  class SwDoDrawStretchCapital
 *************************************************************************/

class SwDoDrawStretchCapital : public SwDoDrawCapital
{
    const xub_StrLen nStrLen;
    const USHORT nCapWidth;
    const USHORT nOrgWidth;
public:
    virtual void Do();

    SwDoDrawStretchCapital( SwDrawTextInfo &rInfo, const USHORT nCapitalWidth )
            : SwDoDrawCapital( rInfo ),
              nStrLen( rInfo.GetLen() ),
              nCapWidth( nCapitalWidth ),
              nOrgWidth( rInfo.GetWidth() )
        { }
};

/*************************************************************************
 *					  SwDoDrawStretchCapital
 *************************************************************************/

void SwDoDrawStretchCapital::Do()
{
    SV_STAT( nDrawStretchText );
    USHORT nPartWidth = USHORT(rInf.GetSize().Width());

    if( rInf.GetLen() )
    {
        // 4023: Kapitaelchen und Kerning.
        long nDiff = long(nOrgWidth) - long(nCapWidth);
        if( nDiff )
        {
            nDiff *= rInf.GetLen();
            nDiff /= (long) nStrLen;
            nDiff += nPartWidth;
            if( 0 < nDiff )
                nPartWidth = USHORT(nDiff);
        }

        rInf.ApplyAutoColor();

        Point aPos( rInf.GetPos() );
        const BOOL bSwitchL2R = rInf.GetFrm()->IsRightToLeft() &&
                              ! rInf.IsIgnoreFrmRTL();

        if ( bSwitchL2R )
            rInf.GetFrm()->SwitchLTRtoRTL( aPos );

        if ( rInf.GetFrm()->IsVertical() )
            rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

        // Optimierung:
        if( 1 >= rInf.GetLen() )
            GetOut().DrawText( aPos, rInf.GetText(), rInf.GetIdx(),
                rInf.GetLen() );
        else
            GetOut().DrawStretchText( aPos, nPartWidth,
                                rInf.GetText(), rInf.GetIdx(), rInf.GetLen() );
    }
    ((Point&)rInf.GetPos()).X() += nPartWidth;
}

/*************************************************************************
 *					  SwSubFont::DrawStretchCapital()
 *************************************************************************/

void SwSubFont::DrawStretchCapital( SwDrawTextInfo &rInf )
{
    // Es wird vorausgesetzt, dass rPos bereits kalkuliert ist!
    // hochgezogen in SwFont: const Point aPos( CalcPos(rPos) );

    if( rInf.GetLen() == STRING_LEN )
        rInf.SetLen( rInf.GetText().Len() );

    const Point& rOldPos = rInf.GetPos();
    const USHORT nCapWidth = (USHORT)( GetCapitalSize( rInf ).Width() );
    rInf.SetPos( rOldPos );

    rInf.SetDrawSpace( GetUnderline() != UNDERLINE_NONE ||
                       GetOverline()  != UNDERLINE_NONE ||
                       GetStrikeout() != STRIKEOUT_NONE );
    SwDoDrawStretchCapital aDo( rInf, nCapWidth );
    DoOnCapitals( aDo );
}

/*************************************************************************
 *					SwSubFont::DoOnCapitals() const
 *************************************************************************/

// JP 22.8.2001 - global optimization off - Bug 91245 / 91223
#ifdef _MSC_VER
#pragma optimize("g",off)
#endif

void SwSubFont::DoOnCapitals( SwDoCapitals &rDo )
{
    ASSERT( pLastFont, "SwFont::DoOnCapitals: No LastFont?!" );

    Size aPartSize;
    long nKana = 0;
    const XubString aTxt( CalcCaseMap( rDo.GetInf().GetText() ) );
    xub_StrLen nMaxPos = Min( USHORT(rDo.GetInf().GetText().Len()
                            - rDo.GetInf().GetIdx()), rDo.GetInf().GetLen() );
    rDo.GetInf().SetLen( nMaxPos );

    const XubString& rOldText = rDo.GetInf().GetText();
    rDo.GetInf().SetText( aTxt );
    rDo.GetInf().SetSize( aPartSize );
    xub_StrLen nPos = rDo.GetInf().GetIdx();
    xub_StrLen nOldPos = nPos;
    nMaxPos = nMaxPos + nPos;

    // #107816#
    // Look if the length of the original text and the ToUpper-converted
    // text is different. If yes, do special handling.
    XubString aNewText;
    SwCapitalInfo aCapInf( rOldText );
    sal_Bool bCaseMapLengthDiffers( aTxt.Len() != rOldText.Len() );
    if ( bCaseMapLengthDiffers )
        rDo.SetCapInf( aCapInf );

    SwFntObj *pOldLast = pLastFont;
    SwFntAccess *pBigFontAccess = NULL;
    SwFntObj *pBigFont;
    SwFntAccess *pSpaceFontAccess = NULL;
    SwFntObj *pSpaceFont = NULL;

    const void *pMagic2 = NULL;
    USHORT nIndex2 = 0;
    SwSubFont aFont( *this );
    Point aStartPos( rDo.GetInf().GetPos() );

    const BOOL bTextLines = aFont.GetUnderline() != UNDERLINE_NONE
                         || aFont.GetOverline()  != UNDERLINE_NONE
                         || aFont.GetStrikeout() != STRIKEOUT_NONE;
    const BOOL bWordWise = bTextLines && aFont.IsWordLineMode() &&
                           rDo.GetInf().GetDrawSpace();
    const long nTmpKern = rDo.GetInf().GetKern();

    if ( bTextLines )
    {
        if ( bWordWise )
        {
            aFont.SetWordLineMode( FALSE );
            pSpaceFontAccess = new SwFntAccess( pMagic2, nIndex2, &aFont,
                                                rDo.GetInf().GetShell() );
            pSpaceFont = pSpaceFontAccess->Get();
        }
        else
            pSpaceFont = pLastFont;

        // Wir basteln uns einen Font fuer die Grossbuchstaben:
        aFont.SetUnderline( UNDERLINE_NONE );
        aFont.SetOverline( UNDERLINE_NONE );
        aFont.SetStrikeout( STRIKEOUT_NONE );
        pMagic2 = NULL;
        nIndex2 = 0;
        pBigFontAccess = new SwFntAccess( pMagic2, nIndex2, &aFont,
                                          rDo.GetInf().GetShell() );
        pBigFont = pBigFontAccess->Get();
    }
    else
        pBigFont = pLastFont;

    // Hier entsteht der Kleinbuchstabenfont:
    aFont.SetProportion( BYTE( (aFont.GetPropr()*KAPITAELCHENPROP) / 100L) );
    pMagic2 = NULL;
    nIndex2 = 0;
    SwFntAccess *pSmallFontAccess = new SwFntAccess( pMagic2, nIndex2, &aFont,
                                                     rDo.GetInf().GetShell() );
    SwFntObj *pSmallFont = pSmallFontAccess->Get();

    rDo.Init( pBigFont, pSmallFont );
    OutputDevice* pOutSize = pSmallFont->GetPrt();
    if( !pOutSize )
        pOutSize = &rDo.GetOut();
    OutputDevice* pOldOut = &rDo.GetOut();

    const LanguageType eLng = LANGUAGE_DONTKNOW == GetLanguage()
                            ? LANGUAGE_SYSTEM : GetLanguage();

    if( nPos < nMaxPos )
    {
        nPos = (xub_StrLen)pBreakIt->GetBreakIter()->endOfCharBlock( rOldText, nPos,
            pBreakIt->GetLocale( eLng ), CharType::LOWERCASE_LETTER);
        if( nPos == STRING_LEN )
            nPos = nOldPos;
        else if( nPos > nMaxPos )
            nPos = nMaxPos;
    }

    while( nOldPos < nMaxPos )
    {

        //  The lower ones...
        if( nOldPos != nPos )
        {
            SV_STAT( nGetTextSize );
            pLastFont = pSmallFont;
            pLastFont->SetDevFont( rDo.GetInf().GetShell(), rDo.GetOut() );

            // #107816#, #i14820#
            if( bCaseMapLengthDiffers )
            {
                // Build an own 'changed' string for the given part of the
                // source string and use it. That new string may differ in length
                // from the source string.
                const XubString aSnippet( rOldText, nOldPos, nPos - nOldPos);
                aNewText = CalcCaseMap( aSnippet );
                aCapInf.nIdx = nOldPos;
                aCapInf.nLen = nPos - nOldPos;
                rDo.GetInf().SetIdx( 0 );
                rDo.GetInf().SetLen( aNewText.Len() );
                rDo.GetInf().SetText( aNewText );
            }
            else
            {
                rDo.GetInf().SetIdx( nOldPos );
                rDo.GetInf().SetLen( nPos - nOldPos );
            }

            rDo.GetInf().SetUpper( FALSE );
            rDo.GetInf().SetOut( *pOutSize );
            aPartSize = pSmallFont->GetTextSize( rDo.GetInf() );
            nKana += rDo.GetInf().GetKanaDiff();
            rDo.GetInf().SetOut( *pOldOut );
            if( nTmpKern && nPos < nMaxPos )
                aPartSize.Width() += nTmpKern;
            rDo.Do();
            nOldPos = nPos;
        }
        nPos = (xub_StrLen)pBreakIt->GetBreakIter()->nextCharBlock( rOldText, nPos,
               pBreakIt->GetLocale( eLng ), CharType::LOWERCASE_LETTER);
        if( nPos == STRING_LEN || nPos > nMaxPos )
            nPos = nMaxPos;
        ASSERT( nPos, "nextCharBlock not implemented?" );
#ifdef DBG_UTIL
        if( !nPos )
            nPos = nMaxPos;
#endif
        // The upper ones...
        if( nOldPos != nPos )
        {
            const long nSpaceAdd = rDo.GetInf().GetSpace() / SPACING_PRECISION_FACTOR;

            do
            {
                rDo.GetInf().SetUpper( TRUE );
                pLastFont = pBigFont;
                pLastFont->SetDevFont( rDo.GetInf().GetShell(), rDo.GetOut() );
                xub_StrLen nTmp;
                if( bWordWise )
                {
                    nTmp = nOldPos;
                    while( nTmp < nPos && CH_BLANK == rOldText.GetChar( nTmp ) )
                        ++nTmp;
                    if( nOldPos < nTmp )
                    {
                        pLastFont = pSpaceFont;
                        pLastFont->SetDevFont( rDo.GetInf().GetShell(),
                                               rDo.GetOut() );
                        ((SwDoDrawCapital&)rDo).DrawSpace( aStartPos );
                        pLastFont = pBigFont;
                        pLastFont->SetDevFont( rDo.GetInf().GetShell(),
                                               rDo.GetOut() );

                        // #107816#, #i14820#
                        if( bCaseMapLengthDiffers )
                        {
                            // Build an own 'changed' string for the given part of the
                            // source string and use it. That new string may differ in length
                            // from the source string.
                            const XubString aSnippet( rOldText, nOldPos, nTmp - nOldPos);
                            aNewText = CalcCaseMap( aSnippet );
                            aCapInf.nIdx = nOldPos;
                            aCapInf.nLen = nTmp - nOldPos;
                            rDo.GetInf().SetIdx( 0 );
                            rDo.GetInf().SetLen( aNewText.Len() );
                            rDo.GetInf().SetText( aNewText );
                        }
                        else
                        {
                            rDo.GetInf().SetIdx( nOldPos );
                            rDo.GetInf().SetLen( nTmp - nOldPos );
                        }

                        rDo.GetInf().SetOut( *pOutSize );
                        aPartSize = pBigFont->GetTextSize( rDo.GetInf() );
                        nKana += rDo.GetInf().GetKanaDiff();
                        rDo.GetInf().SetOut( *pOldOut );
                        if( nSpaceAdd )
                            aPartSize.Width() += nSpaceAdd * ( nTmp - nOldPos );
                        if( nTmpKern && nPos < nMaxPos )
                            aPartSize.Width() += nTmpKern;
                        rDo.Do();
                        aStartPos = rDo.GetInf().GetPos();
                        nOldPos = nTmp;
                    }

                    while( nTmp < nPos && CH_BLANK != rOldText.GetChar( nTmp ) )
                        ++nTmp;
                }
                else
                    nTmp = nPos;
                if( nTmp > nOldPos )
                {
                      // #107816#, #i14820#
                    if( bCaseMapLengthDiffers )
                    {
                        // Build an own 'changed' string for the given part of the
                        // source string and use it. That new string may differ in length
                        // from the source string.
                        const XubString aSnippet( rOldText, nOldPos, nTmp - nOldPos);
                        aNewText = CalcCaseMap( aSnippet );
                        aCapInf.nIdx = nOldPos;
                        aCapInf.nLen = nTmp - nOldPos;
                        rDo.GetInf().SetIdx( 0 );
                        rDo.GetInf().SetLen( aNewText.Len() );
                        rDo.GetInf().SetText( aNewText );
                    }
                    else
                    {
                        rDo.GetInf().SetIdx( nOldPos );
                        rDo.GetInf().SetLen( nTmp - nOldPos );
                    }

                    rDo.GetInf().SetOut( *pOutSize );
                    aPartSize = pBigFont->GetTextSize( rDo.GetInf() );
                    nKana += rDo.GetInf().GetKanaDiff();
                    rDo.GetInf().SetOut( *pOldOut );
                    if( !bWordWise && rDo.GetInf().GetSpace() )
                    {
                        for( xub_StrLen nI = nOldPos; nI < nPos; ++nI )
                        {
                            if( CH_BLANK == rOldText.GetChar( nI ) )
                                aPartSize.Width() += nSpaceAdd;
                        }
                    }
                    if( nTmpKern && nPos < nMaxPos )
                        aPartSize.Width() += nTmpKern;
                    rDo.Do();
                    nOldPos = nTmp;
                }
            } while( nOldPos != nPos );
        }
        nPos = (xub_StrLen)pBreakIt->GetBreakIter()->endOfCharBlock( rOldText, nPos,
               pBreakIt->GetLocale( eLng ), CharType::LOWERCASE_LETTER);
        if( nPos == STRING_LEN || nPos > nMaxPos )
            nPos = nMaxPos;
        ASSERT( nPos, "endOfCharBlock not implemented?" );
#ifdef DBG_UTIL
        if( !nPos )
            nPos = nMaxPos;
#endif
    }

    // Aufraeumen:
    if( pBigFont != pOldLast )
        delete pBigFontAccess;

    if( bTextLines )
    {
        if( rDo.GetInf().GetDrawSpace() )
        {
            pLastFont = pSpaceFont;
            pLastFont->SetDevFont( rDo.GetInf().GetShell(), rDo.GetOut() );
            ( (SwDoDrawCapital&) rDo ).DrawSpace( aStartPos );
        }
        if ( bWordWise )
            delete pSpaceFontAccess;
    }
    pLastFont =	pOldLast;
    pLastFont->SetDevFont( rDo.GetInf().GetShell(), rDo.GetOut() );

    delete pSmallFontAccess;
    rDo.GetInf().SetText( rOldText );
    rDo.GetInf().SetKanaDiff( nKana );
}

// JP 22.8.2001 - global optimization off - Bug 91245 / 91223
#ifdef _MSC_VER
#pragma optimize("g",on)
#endif


