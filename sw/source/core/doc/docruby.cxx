/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: docruby.cxx,v $
 * $Revision: 1.13 $
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


#include <string.h>			// fuer strchr()
#include <hintids.hxx>

#ifndef _COM_SUN_STAR_I18N_UNICODETYPE_HDL
#include <com/sun/star/i18n/UnicodeType.hdl>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HDL
#include <com/sun/star/i18n/WordType.hdl>
#endif
#include <unotools/charclass.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <mvsave.hxx>		// Strukturen zum Sichern beim Move/Delete
#include <ndtxt.hxx>
#include <txatbase.hxx>
#include <rubylist.hxx>
#include <pam.hxx>
#include <swundo.hxx>		// fuer die UndoIds
#include <undobj.hxx>
#include <breakit.hxx>
#include <crsskip.hxx>

SV_IMPL_PTRARR( SwRubyList, SwRubyListEntryPtr )

using namespace ::com::sun::star::i18n;


/*
 * Members in the list:
 *   - String - the orig text
 *   - SwFmtRuby - the ruby attribut
 *
 *
 */
USHORT SwDoc::FillRubyList( const SwPaM& rPam, SwRubyList& rList,
                            USHORT nMode )
{
    const SwPaM *_pStartCrsr = (SwPaM*)rPam.GetNext(),
                *__pStartCrsr = _pStartCrsr;
    BOOL bCheckEmpty = &rPam != _pStartCrsr;
    do {
        const SwPosition* pStt = _pStartCrsr->Start(),
                        * pEnd = pStt == _pStartCrsr->GetPoint()
                                                ? _pStartCrsr->GetMark()
                                                : _pStartCrsr->GetPoint();
        if( !bCheckEmpty || ( pStt != pEnd && *pStt != *pEnd ))
        {
            SwPaM aPam( *pStt );
            do {
                SwRubyListEntry* pNew = new SwRubyListEntry;
                if( pEnd != pStt )
                {
                    aPam.SetMark();
                    *aPam.GetMark() = *pEnd;
                }
                if( _SelectNextRubyChars( aPam, *pNew, nMode ))
                {
                    rList.Insert( pNew, rList.Count() );
                    aPam.DeleteMark();
                }
                else
                {
                    delete pNew;
                     if( *aPam.GetPoint() < *pEnd )
                     {
                        // goto next paragraph
                        aPam.DeleteMark();
                        aPam.Move( fnMoveForward, fnGoNode );
                     }
                     else
                        break;
                }
            } while( 30 > rList.Count() && *aPam.GetPoint() < *pEnd );
        }
    } while( 30 > rList.Count() &&
        (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != __pStartCrsr );

    return rList.Count();
}

USHORT SwDoc::SetRubyList( const SwPaM& rPam, const SwRubyList& rList,
                            USHORT nMode )
{
    StartUndo( UNDO_SETRUBYATTR, NULL );
    SvUShortsSort aDelArr;
    aDelArr.Insert( RES_TXTATR_CJK_RUBY );

    USHORT nListEntry = 0;

    const SwPaM *_pStartCrsr = (SwPaM*)rPam.GetNext(),
                *__pStartCrsr = _pStartCrsr;
    BOOL bCheckEmpty = &rPam != _pStartCrsr;
    do {
        const SwPosition* pStt = _pStartCrsr->Start(),
                        * pEnd = pStt == _pStartCrsr->GetPoint()
                                                ? _pStartCrsr->GetMark()
                                                : _pStartCrsr->GetPoint();
        if( !bCheckEmpty || ( pStt != pEnd && *pStt != *pEnd ))
        {

            SwPaM aPam( *pStt );
            do {
                SwRubyListEntry aCheckEntry;
                if( pEnd != pStt )
                {
                    aPam.SetMark();
                    *aPam.GetMark() = *pEnd;
                }
                if( _SelectNextRubyChars( aPam, aCheckEntry, nMode ))
                {
                    const SwRubyListEntry* pEntry = rList[ nListEntry++ ];
                    if( aCheckEntry.GetRubyAttr() != pEntry->GetRubyAttr() )
                    {
                        // set/reset the attribut
                        if( pEntry->GetRubyAttr().GetText().Len() )
                            Insert( aPam, pEntry->GetRubyAttr(), 0 );
                        else
                            ResetAttrs( aPam, TRUE, &aDelArr );
                    }

                    if( aCheckEntry.GetText() != pEntry->GetText() &&
                        pEntry->GetText().Len() )
                    {
                        // text is changed, so replace the original
                        Replace( aPam, pEntry->GetText(), FALSE );
                    }
                    aPam.DeleteMark();
                }
                else
                {
                     if( *aPam.GetPoint() < *pEnd )
                     {
                        // goto next paragraph
                        aPam.DeleteMark();
                        aPam.Move( fnMoveForward, fnGoNode );
                     }
                     else
                    {
                        const SwRubyListEntry* pEntry = rList[ nListEntry++ ];

                        // set/reset the attribut
                        if( pEntry->GetRubyAttr().GetText().Len() &&
                            pEntry->GetText().Len() )
                        {
                            Insert( aPam, pEntry->GetText(), true );
                            aPam.SetMark();
                            aPam.GetMark()->nContent -= pEntry->GetText().Len();
                            Insert( aPam, pEntry->GetRubyAttr(), nsSetAttrMode::SETATTR_DONTEXPAND );
                        }
                        else
                            break;
                        aPam.DeleteMark();
                    }
                }
            } while( nListEntry < rList.Count() && *aPam.GetPoint() < *pEnd );
        }
    } while( 30 > rList.Count() &&
        (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != __pStartCrsr );

    EndUndo( UNDO_SETRUBYATTR, NULL );

    return nListEntry;
}

BOOL SwDoc::_SelectNextRubyChars( SwPaM& rPam, SwRubyListEntry& rEntry, USHORT )
{
    // Point must be the startposition, Mark is optional the end position
    SwPosition* pPos = rPam.GetPoint();
       const SwTxtNode* pTNd = pPos->nNode.GetNode().GetTxtNode();
    const String* pTxt = &pTNd->GetTxt();
    xub_StrLen nStart = pPos->nContent.GetIndex(), nEnd = pTxt->Len();

    BOOL bHasMark = rPam.HasMark();
    if( bHasMark )
    {
        // in the same node?
        if( rPam.GetMark()->nNode == pPos->nNode )
        {
            // then use that end
            xub_StrLen nTEnd = rPam.GetMark()->nContent.GetIndex();
            if( nTEnd < nEnd )
                nEnd = nTEnd;
        }
        rPam.DeleteMark();
    }

    // ----- search the start
    // --- look where a ruby attribut starts
    USHORT nHtIdx = USHRT_MAX;
    const SwpHints* pHts = pTNd->GetpSwpHints();
    const SwTxtAttr* pAttr = 0;
    if( pHts )
    {
        const SwTxtAttr* pHt;
        for( nHtIdx = 0; nHtIdx < pHts->Count(); ++nHtIdx )
            if( RES_TXTATR_CJK_RUBY == ( pHt = (*pHts)[ nHtIdx ])->Which() &&
                *pHt->GetAnyEnd() > nStart )
            {
                if( *pHt->GetStart() < nEnd )
                {
                    pAttr = pHt;
                    if( !bHasMark && nStart > *pAttr->GetStart() )
                    {
                        nStart = *pAttr->GetStart();
                        pPos->nContent = nStart;
                    }
                }
                break;
            }
    }

    if( !bHasMark && nStart && ( !pAttr || nStart != *pAttr->GetStart()) )
    {
        // skip to the word begin!
        long nWordStt = pBreakIt->GetBreakIter()->getWordBoundary(
                            *pTxt, nStart,
                            pBreakIt->GetLocale( pTNd->GetLang( nStart )),
                            WordType::ANYWORD_IGNOREWHITESPACES,
                            TRUE ).startPos;
        if( nWordStt < nStart && -1 != nWordStt )
        {
            nStart = (xub_StrLen)nWordStt;
            pPos->nContent = nStart;
        }
    }

    BOOL bAlphaNum = FALSE;
    long nWordEnd = nEnd;
    CharClass& rCC = GetAppCharClass();
    while(  nStart < nEnd )
    {
        if( pAttr && nStart == *pAttr->GetStart() )
        {
            pPos->nContent = nStart;
            if( !rPam.HasMark() )
            {
                rPam.SetMark();
                pPos->nContent = *pAttr->GetAnyEnd();
                if( pPos->nContent.GetIndex() > nEnd )
                    pPos->nContent = nEnd;
                rEntry.SetRubyAttr( pAttr->GetRuby() );
            }
            break;
        }

        sal_Int32 nChType = rCC.getType( *pTxt, nStart );
        BOOL bIgnoreChar = FALSE, bIsAlphaNum = FALSE, bChkNxtWrd = FALSE;
        switch( nChType )
        {
        case UnicodeType::UPPERCASE_LETTER:
        case UnicodeType::LOWERCASE_LETTER:
        case UnicodeType::TITLECASE_LETTER:
        case UnicodeType::DECIMAL_DIGIT_NUMBER:
                bChkNxtWrd = bIsAlphaNum = TRUE;
                break;

        case UnicodeType::SPACE_SEPARATOR:
        case UnicodeType::CONTROL:
/*??*/	case UnicodeType::PRIVATE_USE:
        case UnicodeType::START_PUNCTUATION:
        case UnicodeType::END_PUNCTUATION:
            bIgnoreChar = TRUE;
            break;


        case UnicodeType::OTHER_LETTER:
            bChkNxtWrd = TRUE;
            // no break!
//		case UnicodeType::UNASSIGNED:
//		case UnicodeType::MODIFIER_LETTER:
//		case UnicodeType::NON_SPACING_MARK:
//		case UnicodeType::ENCLOSING_MARK:
//		case UnicodeType::COMBINING_SPACING_MARK:
//		case UnicodeType::LETTER_NUMBER:
//		case UnicodeType::OTHER_NUMBER:
//		case UnicodeType::LINE_SEPARATOR:
//		case UnicodeType::PARAGRAPH_SEPARATOR:
//		case UnicodeType::FORMAT:
//		case UnicodeType::SURROGATE:
//		case UnicodeType::DASH_PUNCTUATION:
//		case UnicodeType::CONNECTOR_PUNCTUATION:
///*?? */case UnicodeType::OTHER_PUNCTUATION:
//--> char '!' is to ignore!
//		case UnicodeType::MATH_SYMBOL:
//		case UnicodeType::CURRENCY_SYMBOL:
//		case UnicodeType::MODIFIER_SYMBOL:
//		case UnicodeType::OTHER_SYMBOL:
//		case UnicodeType::INITIAL_PUNCTUATION:
//		case UnicodeType::FINAL_PUNCTUATION:
        default:
                bIsAlphaNum = FALSE;
                break;
        }

        if( rPam.HasMark() )
        {
            if( bIgnoreChar || bIsAlphaNum != bAlphaNum || nStart >= nWordEnd )
                break;
        }
        else if( !bIgnoreChar )
        {
            rPam.SetMark();
            bAlphaNum = bIsAlphaNum;
            if( bChkNxtWrd && pBreakIt->GetBreakIter().is() )
            {
                // search the end of this word
                nWordEnd = pBreakIt->GetBreakIter()->getWordBoundary(
                            *pTxt, nStart,
                            pBreakIt->GetLocale( pTNd->GetLang( nStart )),
                            WordType::ANYWORD_IGNOREWHITESPACES,
                            TRUE ).endPos;
                if( 0 > nWordEnd || nWordEnd > nEnd || nWordEnd == nStart )
                    nWordEnd = nEnd;
            }
        }
        pTNd->GoNext( &pPos->nContent, CRSR_SKIP_CHARS );
        nStart = pPos->nContent.GetIndex();
    }

    nStart = rPam.GetMark()->nContent.GetIndex();
    rEntry.SetText( pTxt->Copy( nStart,
                           rPam.GetPoint()->nContent.GetIndex() - nStart ));
    return rPam.HasMark();
}

SwRubyListEntry::~SwRubyListEntry()
{
}
