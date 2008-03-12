/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: guess.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: rt $ $Date: 2008-03-12 12:24:10 $
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


#include <ctype.h>

#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _SHL_HXX
#include <tools/shl.hxx>    // needed for SW_MOD() macro
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>   // ASSERTs
#endif
#ifndef _DLELSTNR_HXX_
#include <dlelstnr.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef IDOCUMENTSETTINGACCESS_HXX_INCLUDED
#include <IDocumentSettingAccess.hxx>
#endif
#ifndef _TXTCFG_HXX
#include <txtcfg.hxx>
#endif
#ifndef _GUESS_HXX
#include <guess.hxx>
#endif
#ifndef _INFTXT_HXX
#include <inftxt.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx> // SwPageDesc
#endif
#ifndef SW_TGRDITEM_HXX
#include <tgrditem.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_BREAKTYPE_HPP_
#include <com/sun/star/i18n/BreakType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HPP_
#include <com/sun/star/i18n/WordType.hpp>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _PORFLD_HXX
#include <porfld.hxx>
#endif

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::i18n;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::linguistic2;

#define CH_FULL_BLANK 0x3000

/*************************************************************************
 *						SwTxtGuess::Guess
 *
 * provides information for line break calculation
 * returns true if no line break has to be performed
 * otherwise possible break or hyphenation position is determined
 *************************************************************************/

sal_Bool SwTxtGuess::Guess( const SwTxtPortion& rPor, SwTxtFormatInfo &rInf,
                            const KSHORT nPorHeight )
{
    nCutPos = rInf.GetIdx();

    // Leere Strings sind immer 0
    if( !rInf.GetLen() || !rInf.GetTxt().Len() )
        return sal_False;

    ASSERT( rInf.GetIdx() < rInf.GetTxt().Len(),
            "+SwTxtGuess::Guess: invalid SwTxtFormatInfo" );

    ASSERT( nPorHeight, "+SwTxtGuess::Guess: no height" );

    USHORT nMinSize;
    USHORT nMaxSizeDiff;

    const SwScriptInfo& rSI =
            ((SwParaPortion*)rInf.GetParaPortion())->GetScriptInfo();

    USHORT nMaxComp = ( SW_CJK == rInf.GetFont()->GetActual() ) &&
                        rSI.CountCompChg() &&
                        ! rInf.IsMulti() &&
                        ! rPor.InFldGrp() &&
                        ! rPor.IsDropPortion() ?
                        10000 :
                            0 ;

    SwTwips nLineWidth = rInf.Width() - rInf.X();
    xub_StrLen nMaxLen = rInf.GetTxt().Len() - rInf.GetIdx();

    if ( rInf.GetLen() < nMaxLen )
        nMaxLen = rInf.GetLen();

    if( !nMaxLen )
        return sal_False;

    KSHORT nItalic = 0;
    if( ITALIC_NONE != rInf.GetFont()->GetItalic() && !rInf.NotEOL() )
    {
        sal_Bool bAddItalic = sal_True;

        // do not add extra italic value if we have an active character grid
        if ( rInf.SnapToGrid() )
        {
            GETGRID( rInf.GetTxtFrm()->FindPageFrm() )
            bAddItalic = !pGrid || GRID_LINES_CHARS != pGrid->GetGridType();
        }

        // do not add extra italic value for an isolated blank:
        if ( 1 == rInf.GetLen() &&
             CH_BLANK == rInf.GetTxt().GetChar( rInf.GetIdx() ) )
            bAddItalic = sal_False;

        nItalic = bAddItalic ? nPorHeight / 12 : 0;

        nLineWidth -= nItalic;

        // --> FME 2005-05-13 #i46524# LineBreak bug with italics
        if ( nLineWidth < 0 ) nLineWidth = 0;
        // <--
    }

    // first check if everything fits to line
    if ( long ( nLineWidth ) * 2 > long ( nMaxLen ) * nPorHeight )
    {
        // call GetTxtSize with maximum compression (for kanas)
        rInf.GetTxtSize( &rSI, rInf.GetIdx(), nMaxLen,
                         nMaxComp, nMinSize, nMaxSizeDiff );

        nBreakWidth = nMinSize;

        if ( nBreakWidth <= nLineWidth )
        {
            // portion fits to line
            nCutPos = rInf.GetIdx() + nMaxLen;
            if( nItalic &&
                ( nCutPos >= rInf.GetTxt().Len() ||
                  // --> FME 2005-05-13 #i48035# Needed for CalcFitToContent
                  // if first line ends with a manual line break
                  rInf.GetTxt().GetChar( nCutPos ) == CH_BREAK ) )
                  // <--
                nBreakWidth = nBreakWidth + nItalic;

            // save maximum width for later use
            if ( nMaxSizeDiff )
                rInf.SetMaxWidthDiff( (ULONG)&rPor, nMaxSizeDiff );

            return sal_True;
        }
    }

    sal_Bool bHyph = rInf.IsHyphenate() && !rInf.IsHyphForbud();
    xub_StrLen nHyphPos = 0;

    // nCutPos is the first character not fitting to the current line
    // nHyphPos is the first character not fitting to the current line,
    // considering an additional "-" for hyphenation
    if( bHyph )
    {
        nCutPos = rInf.GetTxtBreak( nLineWidth, nMaxLen, nMaxComp, nHyphPos );

        if ( !nHyphPos && rInf.GetIdx() )
            nHyphPos = rInf.GetIdx() - 1;
    }
    else
    {
        nCutPos = rInf.GetTxtBreak( nLineWidth, nMaxLen, nMaxComp );

#ifndef PRODUCT
        if ( STRING_LEN != nCutPos )
        {
            rInf.GetTxtSize( &rSI, rInf.GetIdx(), nCutPos - rInf.GetIdx(),
                             nMaxComp, nMinSize, nMaxSizeDiff );
            ASSERT( nMinSize <= nLineWidth, "What a Guess!!!" );
        }
#endif
    }

    if( nCutPos > rInf.GetIdx() + nMaxLen )
    {
        // second check if everything fits to line
        nCutPos = nBreakPos = rInf.GetIdx() + nMaxLen - 1;
        rInf.GetTxtSize( &rSI, rInf.GetIdx(), nMaxLen, nMaxComp,
                         nMinSize, nMaxSizeDiff );

        nBreakWidth = nMinSize;

        // Der folgende Vergleich sollte eigenlich immer sal_True ergeben, sonst
        // hat es wohl bei GetTxtBreak einen Pixel-Rundungsfehler gegeben...
        if ( nBreakWidth <= nLineWidth )
        {
            if( nItalic && ( nBreakPos + 1 ) >= rInf.GetTxt().Len() )
                nBreakWidth = nBreakWidth + nItalic;

            // save maximum width for later use
            if ( nMaxSizeDiff )
                rInf.SetMaxWidthDiff( (ULONG)&rPor, nMaxSizeDiff );

            return sal_True;
        }
    }

    // we have to trigger an underflow for a footnote portion
    // which does not fit to the current line
    if ( rPor.IsFtnPortion() )
    {
        nBreakPos = rInf.GetIdx();
        nCutPos = rInf.GetLen();
        return sal_False;
    }

    xub_StrLen nPorLen = 0;
    // do not call the break iterator nCutPos is a blank
    xub_Unicode cCutChar = rInf.GetTxt().GetChar( nCutPos );
    if( CH_BLANK == cCutChar || CH_FULL_BLANK == cCutChar )
    {
        nBreakPos = nCutPos;
        xub_StrLen nX = nBreakPos;

        // we step back until a non blank character has been found
        // or there is only one more character left
        while( nX && nBreakPos > rInf.GetLineStart() + 1 &&
               ( CH_BLANK == ( cCutChar = rInf.GetChar( --nX ) ) ||
                 CH_FULL_BLANK == cCutChar ) )
            --nBreakPos;

        if( nBreakPos > rInf.GetIdx() )
            nPorLen = nBreakPos - rInf.GetIdx();
        while( ++nCutPos < rInf.GetTxt().Len() &&
               ( CH_BLANK == ( cCutChar = rInf.GetChar( nCutPos ) ) ||
                 CH_FULL_BLANK == cCutChar ) )
            ; // nothing

        nBreakStart = nCutPos;
    }
    else if( pBreakIt->xBreak.is() )
    {
        // New: We should have a look into the last portion, if it was a
        // field portion. For this, we expand the text of the field portion
        // into our string. If the line break position is inside of before
        // the field portion, we trigger an underflow.

        xub_StrLen nOldIdx = rInf.GetIdx();
        xub_Unicode cFldChr = 0;

#if OSL_DEBUG_LEVEL > 1
        XubString aDebugString;
#endif

        // be careful: a field portion can be both: 0x01 (common field)
        // or 0x02 (the follow of a footnode)
        if ( rInf.GetLast() && rInf.GetLast()->InFldGrp() &&
             ! rInf.GetLast()->IsFtnPortion() &&
             rInf.GetIdx() > rInf.GetLineStart() &&
             CH_TXTATR_BREAKWORD ==
                ( cFldChr = rInf.GetTxt().GetChar( rInf.GetIdx() - 1 ) ) )
        {
            SwFldPortion* pFld = (SwFldPortion*)rInf.GetLast();
            XubString aTxt;
            pFld->GetExpTxt( rInf, aTxt );

            if ( aTxt.Len() )
            {
                nFieldDiff = aTxt.Len() - 1;
                nCutPos = nCutPos + nFieldDiff;
                nHyphPos = nHyphPos + nFieldDiff;

#if OSL_DEBUG_LEVEL > 1
                aDebugString = rInf.GetTxt();
#endif

                XubString& rOldTxt = (XubString&)rInf.GetTxt();
                rOldTxt.Erase( rInf.GetIdx() - 1, 1 );
                rOldTxt.Insert( aTxt, rInf.GetIdx() - 1 );
                rInf.SetIdx( rInf.GetIdx() + nFieldDiff );
            }
            else
                cFldChr = 0;
        }

        LineBreakHyphenationOptions aHyphOpt;
        Reference< XHyphenator >  xHyph;
        if( bHyph )
        {
            xHyph = ::GetHyphenator();
            aHyphOpt = LineBreakHyphenationOptions( xHyph,
                                rInf.GetHyphValues(), nHyphPos );
        }

        // Get Language for break iterator.
        // We have to switch the current language if we have a script
        // change at nCutPos. Otherwise LATIN punctuation would never
        // be allowed to be hanging punctuation.
        // NEVER call GetLang if the string has been modified!!!
        LanguageType aLang = rInf.GetFont()->GetLanguage();

        // If we are inside a field portion, we use a temporar string which
        // differs from the string at the textnode. Therefore we are not allowed
        // to call the GetLang function.
        if ( nCutPos && ! rPor.InFldGrp() )
        {
            const CharClass& rCC = GetAppCharClass();

            // step back until a non-punctuation character is reached
            xub_StrLen nLangIndex = nCutPos;

            // If a field has been expanded right in front of us we do not
            // step further than the beginning of the expanded field
            // (which is the position of the field placeholder in our
            // original string).
            const xub_StrLen nDoNotStepOver = CH_TXTATR_BREAKWORD == cFldChr ?
                                              rInf.GetIdx() - nFieldDiff - 1:
                                              0;

            while ( nLangIndex > nDoNotStepOver &&
                    ! rCC.isLetterNumeric( rInf.GetTxt(), nLangIndex ) )
                --nLangIndex;

            // last "real" character is not inside our current portion
            // we have to check the script type of the last "real" character
            if ( nLangIndex < rInf.GetIdx() )
            {
                USHORT nScript = pBreakIt->GetRealScriptOfText( rInf.GetTxt(),
                                                                nLangIndex );
                ASSERT( nScript, "Script is not between 1 and 4" );

                // compare current script with script from last "real" character
                if ( nScript - 1 != rInf.GetFont()->GetActual() )
                    aLang = rInf.GetTxtFrm()->GetTxtNode()->GetLang(
                        CH_TXTATR_BREAKWORD == cFldChr ?
                        nDoNotStepOver :
                        nLangIndex, 0, nScript );
            }
        }

        const ForbiddenCharacters aForbidden(
                *rInf.GetTxtFrm()->GetNode()->getIDocumentSettingAccess()->getForbiddenCharacters( aLang, true ) );

        const sal_Bool bAllowHanging = rInf.IsHanging() && ! rInf.IsMulti() &&
                                      ! rPor.InFldGrp();

        LineBreakUserOptions aUserOpt(
                aForbidden.beginLine, aForbidden.endLine,
                rInf.HasForbiddenChars(), bAllowHanging, sal_False );

        //! register listener to LinguServiceEvents now in order to get
        //! notified about relevant changes in the future
        SwModule *pModule = SW_MOD();
        if (!pModule->GetLngSvcEvtListener().is())
            pModule->CreateLngSvcEvtListener();

        // !!! We must have a local copy of the locale, because inside
        // getLineBreak the LinguEventListener can trigger a new formatting,
        // which can corrupt the locale pointer inside pBreakIt.
        const lang::Locale aLocale = pBreakIt->GetLocale( aLang );

        // determines first possible line break from nRightPos to
        // start index of current line
        LineBreakResults aResult = pBreakIt->xBreak->getLineBreak(
            rInf.GetTxt(), nCutPos, aLocale,
            rInf.GetLineStart(), aHyphOpt, aUserOpt );

        nBreakPos = (xub_StrLen)aResult.breakIndex;

        // if we are formatting multi portions we want to allow line breaks
        // at the border between single line and multi line portion
        // we have to be carefull with footnote portions, they always come in
        // with an index 0
        if ( nBreakPos < rInf.GetLineStart() && rInf.IsFirstMulti() &&
             ! rInf.IsFtnInside() )
            nBreakPos = rInf.GetLineStart();

        nBreakStart = nBreakPos;

        bHyph = BreakType::HYPHENATION == aResult.breakType;

        if ( bHyph && nBreakPos != STRING_LEN)
        {
            // found hyphenation position within line
            // nBreakPos is set to the hyphenation position
            xHyphWord = aResult.rHyphenatedWord;
            nBreakPos += xHyphWord->getHyphenationPos() + 1;

#if OSL_DEBUG_LEVEL > 1
            // e.g., Schif-fahrt, referes to our string
            const String aWord = xHyphWord->getWord();
            // e.g., Schiff-fahrt, referes to the word after hyphenation
            const String aHyphenatedWord = xHyphWord->getHyphenatedWord();
            // e.g., Schif-fahrt: 5, referes to our string
            const USHORT nHyphenationPos = xHyphWord->getHyphenationPos();
            (void)nHyphenationPos;
            // e.g., Schiff-fahrt: 6, referes to the word after hyphenation
            const USHORT nHyphenPos = xHyphWord->getHyphenPos();
            (void)nHyphenPos;
#endif

            // if not in interactive mode, we have to break behind a soft hyphen
            if ( ! rInf.IsInterHyph() && rInf.GetIdx() )
            {
                const long nSoftHyphPos =
                        xHyphWord->getWord().indexOf( CHAR_SOFTHYPHEN );

                if ( nSoftHyphPos >= 0 &&
                     nBreakStart + nSoftHyphPos <= nBreakPos &&
                     nBreakPos > rInf.GetLineStart() )
                    nBreakPos = rInf.GetIdx() - 1;
            }

            if( nBreakPos >= rInf.GetIdx() )
            {
                nPorLen = nBreakPos - rInf.GetIdx();
                if( '-' == rInf.GetTxt().GetChar( nBreakPos - 1 ) )
                    xHyphWord = NULL;
            }
        }
        else if ( !bHyph && nBreakPos >= rInf.GetLineStart() )
        {
            ASSERT( nBreakPos != STRING_LEN, "we should have found a break pos" );

            // found break position within line
            xHyphWord = NULL;

            // check, if break position is soft hyphen and an underflow
            // has to be triggered
            if( nBreakPos > rInf.GetLineStart() && rInf.GetIdx() &&
                CHAR_SOFTHYPHEN == rInf.GetTxt().GetChar( nBreakPos - 1 ) )
                nBreakPos = rInf.GetIdx() - 1;

            // Delete any blanks at the end of a line, but be careful:
            // If a field has been expanded, we do not want to delete any
            // blanks inside the field portion. This would cause an unwanted
            // underflow
            xub_StrLen nX = nBreakPos;
            while( nX > rInf.GetLineStart() &&
                   ( CH_TXTATR_BREAKWORD != cFldChr || nX > rInf.GetIdx() ) &&
                   ( CH_BLANK == rInf.GetChar( --nX ) ||
                     CH_FULL_BLANK == rInf.GetChar( nX ) ) )
                nBreakPos = nX;
            if( nBreakPos > rInf.GetIdx() )
                nPorLen = nBreakPos - rInf.GetIdx();
        }
        else
        {
            // no line break found, setting nBreakPos to STRING_LEN
            // causes a break cut
            nBreakPos = STRING_LEN;
            ASSERT( nCutPos >= rInf.GetIdx(), "Deep cut" );
            nPorLen = nCutPos - rInf.GetIdx();
        }

        if( nBreakPos > nCutPos && nBreakPos != STRING_LEN )
        {
            const xub_StrLen nHangingLen = nBreakPos - nCutPos;
            SwPosSize aTmpSize = rInf.GetTxtSize( &rSI, nCutPos,
                                                  nHangingLen, 0 );
            ASSERT( !pHanging, "A hanging portion is hanging around" );
            pHanging = new SwHangingPortion( aTmpSize );
            pHanging->SetLen( nHangingLen );
            nPorLen = nCutPos - rInf.GetIdx();
        }

        // If we expanded a field, we must repair the original string.
        // In case we do not trigger an underflow, we correct the nBreakPos
        // value, but we cannot correct the nBreakStart value:
        // If we have found a hyphenation position, nBreakStart can lie before
        // the field.
        if ( CH_TXTATR_BREAKWORD == cFldChr )
        {
            if ( nBreakPos < rInf.GetIdx() )
                nBreakPos = nOldIdx - 1;
            else if ( STRING_LEN != nBreakPos )
            {
                ASSERT( nBreakPos >= nFieldDiff, "I've got field trouble!" );
                nBreakPos = nBreakPos - nFieldDiff;
            }

            ASSERT( nCutPos >= rInf.GetIdx() && nCutPos >= nFieldDiff,
                    "I've got field trouble, part2!" );
            nCutPos = nCutPos - nFieldDiff;

            XubString& rOldTxt = (XubString&)rInf.GetTxt();
            rOldTxt.Erase( nOldIdx - 1, nFieldDiff + 1 );
            rOldTxt.Insert( cFldChr, nOldIdx - 1 );
            rInf.SetIdx( nOldIdx );

#if OSL_DEBUG_LEVEL > 1
            ASSERT( aDebugString == rInf.GetTxt(),
                    "Somebody, somebody, somebody put something in my string" );
#endif
        }
    }

    if( nPorLen )
    {
        rInf.GetTxtSize( &rSI, rInf.GetIdx(), nPorLen,
                         nMaxComp, nMinSize, nMaxSizeDiff );

        // save maximum width for later use
        if ( nMaxSizeDiff )
            rInf.SetMaxWidthDiff( (ULONG)&rPor, nMaxSizeDiff );

        nBreakWidth = nItalic + nMinSize;
    }
    else
        nBreakWidth = 0;

    if( pHanging )
        nBreakPos = nCutPos;

    return sal_False;
}

/*************************************************************************
 *						SwTxtGuess::AlternativeSpelling
 *************************************************************************/

// returns true if word at position nPos has a diffenrent spelling
// if hyphenated at this position (old german spelling)

sal_Bool SwTxtGuess::AlternativeSpelling( const SwTxtFormatInfo &rInf,
    const xub_StrLen nPos )
{
    // get word boundaries
    xub_StrLen nWordLen;

    Boundary aBound =
        pBreakIt->xBreak->getWordBoundary( rInf.GetTxt(), nPos,
        pBreakIt->GetLocale( rInf.GetFont()->GetLanguage() ),
        WordType::DICTIONARY_WORD, sal_True );
    nBreakStart = (xub_StrLen)aBound.startPos;
    nWordLen = static_cast<xub_StrLen>(aBound.endPos - nBreakStart);

    // if everything else fails, we want to cut at nPos
    nCutPos = nPos;

    XubString aTxt( rInf.GetTxt().Copy( nBreakStart, nWordLen ) );

    // check, if word has alternative spelling
    Reference< XHyphenator >  xHyph( ::GetHyphenator() );
    ASSERT( xHyph.is(), "Hyphenator is missing");
    //! subtract 1 since the UNO-interface is 0 based
    xHyphWord =	xHyph->queryAlternativeSpelling( OUString(aTxt),
                        pBreakIt->GetLocale( rInf.GetFont()->GetLanguage() ),
                        nPos - nBreakStart, rInf.GetHyphValues() );
    return xHyphWord.is() && xHyphWord->isAlternativeSpelling();
}

