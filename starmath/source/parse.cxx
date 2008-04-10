/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: parse.cxx,v $
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
#include "precompiled_starmath.hxx"


#include <stdio.h>

#define SMDLL 1

#include <com/sun/star/i18n/UnicodeType.hpp>
#include <i18npool/lang.h>
#include <unotools/charclass.hxx>
#include <svx/unolingu.hxx>
#include <svtools/syslocale.hxx>
#include "parse.hxx"
#ifndef _STARMATH_HRC
#include "starmath.hrc"
#endif
#ifndef _SMDLL_HXX
#include "smdll.hxx"
#endif
#include "smmod.hxx"
#include "config.hxx"

#include "node.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::i18n;

///////////////////////////////////////////////////////////////////////////

static inline BOOL strnccmp(const String &u1, xub_StrLen nIdx,
                              const sal_Char *s2, xub_StrLen nLen)
{
    return u1.EqualsIgnoreCaseAscii( s2, nIdx, nLen );
}

static const sal_Unicode aDelimiterTable[] =
{
    ' ',	'\t',	'\n',	'\r',	'+',	'-',	'*',	'/',	'=',	'#',
    '%',	'\\',	'"',	'~',	'`',	'>',	'<',	'&',	'|',	'(',
    ')',	'{',	'}',	'[',	']',	'^',	'_',
    '\0'	// end of list symbol
};


static inline BOOL IsDigit( sal_Unicode cChar )
{
    return '0' <= cChar && cChar <= '9';
}

///////////////////////////////////////////////////////////////////////////

SmToken::SmToken() :
    eType		(TUNKNOWN),
    cMathChar	('\0')
{
    nGroup = nCol = nRow = nLevel = 0;
}

///////////////////////////////////////////////////////////////////////////

struct SmTokenTableEntry
{
    const sal_Char*	pIdent;
    SmTokenType	 	eType;
    sal_Unicode	 	cMathChar;
    ULONG 		 	nGroup;
    USHORT		 	nLevel;
};

static const SmTokenTableEntry aTokenTable[] =
{
//	{ "#", TPOUND, '\0', 0, 0 },
//	{ "##", TDPOUND, '\0', 0, 0 },
//	{ "&", TAND, MS_AND, TGPRODUCT, 0 },
//	{ "(", TLPARENT, MS_LPARENT, TGLBRACES, 5 },	//! 5 to continue expression
//	{ ")", TRPARENT, MS_RPARENT, TGRBRACES, 0 },	//! 0 to terminate expression
//	{ "*", TMULTIPLY, MS_MULTIPLY, TGPRODUCT, 0 },
//	{ "+", TPLUS, MS_PLUS, TGUNOPER | TGSUM, 5 },
//	{ "+-", TPLUSMINUS, MS_PLUSMINUS, TGUNOPER | TGSUM, 5 },
//	{ "-", TMINUS, MS_MINUS, TGUNOPER | TGSUM, 5 },
//	{ "-+", TMINUSPLUS, MS_MINUSPLUS, TGUNOPER | TGSUM, 5 },
//	{ ".", TPOINT, '\0', 0, 0 },
//	{ "/", TDIVIDEBY, MS_SLASH, TGPRODUCT, 0 },
//	{ "<", TLT, MS_LT, TGRELATION, 0 },
//	{ "<<", TLL, MS_LL, TGRELATION, 0 },
//	{ "<=", TLE, MS_LE, TGRELATION, 0 },
//	{ "<>", TNEQ, MS_NEQ, TGRELATION, 0},
//	{ "<?>", TPLACE, MS_PLACE, 0, 5 },
//	{ "=", TASSIGN, MS_ASSIGN, TGRELATION, 0},
//	{ ">", TGT, MS_GT, TGRELATION, 0 },
//	{ ">=", TGE, MS_GE, TGRELATION, 0 },
//	{ ">>", TGG, MS_GG, TGRELATION, 0 },
    { "Im" , TIM, MS_IM, TGSTANDALONE, 5 },
    { "MZ23", TDEBUG, '\0', TGATTRIBUT, 0 },
    { "Re" , TRE, MS_RE, TGSTANDALONE, 5 },
    { "abs", TABS, '\0', TGUNOPER, 13 },
    { "arcosh", TACOSH, '\0', TGFUNCTION, 5 },
    { "arcoth", TACOTH, '\0', TGFUNCTION, 5 },
    { "acute", TACUTE, MS_ACUTE, TGATTRIBUT, 5 },
    { "aleph" , TALEPH, MS_ALEPH, TGSTANDALONE, 5 },
    { "alignb", TALIGNC, '\0', TGALIGN | TGDISCARDED, 0},
    { "alignc", TALIGNC, '\0', TGALIGN, 0},
    { "alignl", TALIGNL, '\0', TGALIGN, 0},
    { "alignm", TALIGNC, '\0', TGALIGN | TGDISCARDED, 0},
    { "alignr", TALIGNR, '\0', TGALIGN, 0},
    { "alignt", TALIGNC, '\0', TGALIGN | TGDISCARDED, 0},
    { "and", TAND, MS_AND, TGPRODUCT, 0},
    { "approx", TAPPROX, MS_APPROX, TGRELATION, 0},
    { "arccos", TACOS, '\0', TGFUNCTION, 5},
    { "arccot", TACOT, '\0', TGFUNCTION, 5},
    { "arcsin", TASIN, '\0', TGFUNCTION, 5},
    { "arctan", TATAN, '\0', TGFUNCTION, 5},
    { "arsinh", TASINH, '\0', TGFUNCTION, 5},
    { "artanh", TATANH, '\0', TGFUNCTION, 5},
    { "backepsilon" , TBACKEPSILON, MS_BACKEPSILON, TGSTANDALONE, 5},
    { "bar", TBAR, MS_BAR, TGATTRIBUT, 5},
    { "binom", TBINOM, '\0', 0, 5 },
    { "black", TBLACK, '\0', TGCOLOR, 0},
    { "blue", TBLUE, '\0', TGCOLOR, 0},
    { "bold", TBOLD, '\0', TGFONTATTR, 5},
    { "boper", TBOPER, '\0', TGPRODUCT, 0},
    { "breve", TBREVE, MS_BREVE, TGATTRIBUT, 5},
    { "bslash", TBACKSLASH, MS_BACKSLASH, TGPRODUCT, 0 },
    { "cdot", TCDOT, MS_CDOT, TGPRODUCT, 0},
    { "check", TCHECK, MS_CHECK, TGATTRIBUT, 5},
    { "circ" , TCIRC, MS_CIRC, TGSTANDALONE, 5},
    { "circle", TCIRCLE, MS_CIRCLE, TGATTRIBUT, 5},
    { "color", TCOLOR, '\0', TGFONTATTR, 5},
    { "coprod", TCOPROD, MS_COPROD, TGOPER, 5},
    { "cos", TCOS, '\0', TGFUNCTION, 5},
    { "cosh", TCOSH, '\0', TGFUNCTION, 5},
    { "cot", TCOT, '\0', TGFUNCTION, 5},
    { "coth", TCOTH, '\0', TGFUNCTION, 5},
    { "csub", TCSUB, '\0', TGPOWER, 0},
    { "csup", TCSUP, '\0', TGPOWER, 0},
    { "cyan", TCYAN, '\0', TGCOLOR, 0},
    { "dddot", TDDDOT, MS_DDDOT, TGATTRIBUT, 5},
    { "ddot", TDDOT, MS_DDOT, TGATTRIBUT, 5},
    { "def", TDEF, MS_DEF, TGRELATION, 0},
    { "div", TDIV, MS_DIV, TGPRODUCT, 0},
    { "divides", TDIVIDES, MS_LINE, TGRELATION, 0},
    { "dlarrow" , TDLARROW, MS_DLARROW, TGSTANDALONE, 5},
    { "dlrarrow" , TDLRARROW, MS_DLRARROW, TGSTANDALONE, 5},
    { "dot", TDOT, MS_DOT, TGATTRIBUT, 5},
    { "dotsaxis", TDOTSAXIS, MS_DOTSAXIS, TGSTANDALONE, 5},	// 5 to continue expression
    { "dotsdiag", TDOTSDIAG, MS_DOTSUP, TGSTANDALONE, 5},	//
    { "dotsdown", TDOTSDOWN, MS_DOTSDOWN, TGSTANDALONE, 5},  //
    { "dotslow", TDOTSLOW, MS_DOTSLOW, TGSTANDALONE, 5},    //
    { "dotsup", TDOTSUP, MS_DOTSUP, TGSTANDALONE, 5},      //
    { "dotsvert", TDOTSVERT, MS_DOTSVERT, TGSTANDALONE, 5},	//
    { "downarrow" , TDOWNARROW, MS_DOWNARROW, TGSTANDALONE, 5},
    { "drarrow" , TDRARROW, MS_DRARROW, TGSTANDALONE, 5},
    { "emptyset" , TEMPTYSET, MS_EMPTYSET, TGSTANDALONE, 5},
    { "equiv", TEQUIV, MS_EQUIV, TGRELATION, 0},
    { "exists", TEXISTS, MS_EXISTS, TGSTANDALONE, 5},
    { "exp", TEXP, '\0', TGFUNCTION, 5},
    { "fact", TFACT, MS_FACT, TGUNOPER, 5},
    { "fixed", TFIXED, '\0', TGFONT, 0},
    { "font", TFONT, '\0', TGFONTATTR, 5},
    { "forall", TFORALL, MS_FORALL, TGSTANDALONE, 5},
    { "from", TFROM, '\0', TGLIMIT, 0},
    { "func", TFUNC, '\0', TGFUNCTION, 5},
    { "ge", TGE, MS_GE, TGRELATION, 0},
    { "geslant", TGESLANT, MS_GESLANT, TGRELATION, 0 },
    { "gg", TGG, MS_GG, TGRELATION, 0},
    { "grave", TGRAVE, MS_GRAVE, TGATTRIBUT, 5},
    { "green", TGREEN, '\0', TGCOLOR, 0},
    { "gt", TGT, MS_GT, TGRELATION, 0},
    { "hat", THAT, MS_HAT, TGATTRIBUT, 5},
    { "hbar" , THBAR, MS_HBAR, TGSTANDALONE, 5},
    { "iiint", TIIINT, MS_IIINT, TGOPER, 5},
    { "iint", TIINT, MS_IINT, TGOPER, 5},
    { "in", TIN, MS_IN, TGRELATION, 0},
    { "infinity" , TINFINITY, MS_INFINITY, TGSTANDALONE, 5},
    { "infty" , TINFINITY, MS_INFINITY, TGSTANDALONE, 5},
    { "int", TINT, MS_INT, TGOPER, 5},
    { "intersection", TINTERSECT, MS_INTERSECT, TGPRODUCT, 0},
    { "ital", TITALIC, '\0', TGFONTATTR, 5},
    { "italic", TITALIC, '\0', TGFONTATTR, 5},
    { "lambdabar" , TLAMBDABAR, MS_LAMBDABAR, TGSTANDALONE, 5},
    { "langle", TLANGLE, MS_LANGLE, TGLBRACES, 5},
    { "lbrace", TLBRACE, MS_LBRACE, TGLBRACES, 5},
    { "lceil", TLCEIL, MS_LCEIL, TGLBRACES, 5},
    { "ldbracket", TLDBRACKET, MS_LDBRACKET, TGLBRACES, 5},
    { "ldline", TLDLINE, MS_DLINE, TGLBRACES, 5},
    { "le", TLE, MS_LE, TGRELATION, 0},
    { "left", TLEFT, '\0', 0, 5},
    { "leftarrow" , TLEFTARROW, MS_LEFTARROW, TGSTANDALONE, 5},
    { "leslant", TLESLANT, MS_LESLANT, TGRELATION, 0 },
    { "lfloor", TLFLOOR, MS_LFLOOR, TGLBRACES, 5},
    { "lim", TLIM, '\0', TGOPER, 5},
    { "liminf", TLIMINF, '\0', TGOPER, 5},
    { "limsup", TLIMSUP, '\0', TGOPER, 5},
    { "lint", TLINT, MS_LINT, TGOPER, 5},
    { "ll", TLL, MS_LL, TGRELATION, 0},
    { "lline", TLLINE, MS_LINE, TGLBRACES, 5},
    { "llint", TLLINT, MS_LLINT, TGOPER, 5},
    { "lllint", TLLLINT, MS_LLLINT, TGOPER, 5},
    { "ln", TLN, '\0', TGFUNCTION, 5},
    { "log", TLOG, '\0', TGFUNCTION, 5},
    { "lsub", TLSUB, '\0', TGPOWER, 0},
    { "lsup", TLSUP, '\0', TGPOWER, 0},
    { "lt", TLT, MS_LT, TGRELATION, 0},
    { "magenta", TMAGENTA, '\0', TGCOLOR, 0},
    { "matrix", TMATRIX, '\0', 0, 5},
    { "minusplus", TMINUSPLUS, MS_MINUSPLUS, TGUNOPER | TGSUM, 5},
    { "mline", TMLINE, MS_LINE, 0, 0},		//! nicht in TGRBRACES, Level 0
    { "nabla", TNABLA, MS_NABLA, TGSTANDALONE, 5},
    { "nbold", TNBOLD, '\0', TGFONTATTR, 5},
    { "ndivides", TNDIVIDES, MS_NDIVIDES, TGRELATION, 0},
    { "neg", TNEG, MS_NEG, TGUNOPER, 5 },
    { "neq", TNEQ, MS_NEQ, TGRELATION, 0},
    { "newline", TNEWLINE, '\0', 0, 0},
    { "ni", TNI, MS_NI, TGRELATION, 0},
    { "nitalic", TNITALIC, '\0', TGFONTATTR, 5},
    { "none", TNONE, '\0', TGLBRACES | TGRBRACES, 0},
    { "notin", TNOTIN, MS_NOTIN, TGRELATION, 0},
    { "nsubset", TNSUBSET, MS_NSUBSET, TGRELATION, 0 },
    { "nsupset", TNSUPSET, MS_NSUPSET, TGRELATION, 0 },
    { "nsubseteq", TNSUBSETEQ, MS_NSUBSETEQ, TGRELATION, 0 },
    { "nsupseteq", TNSUPSETEQ, MS_NSUPSETEQ, TGRELATION, 0 },
    { "nroot", TNROOT, MS_SQRT, TGUNOPER, 5},
    { "odivide", TODIVIDE, MS_ODIVIDE, TGPRODUCT, 0},
    { "odot", TODOT, MS_ODOT, TGPRODUCT, 0},
    { "ominus", TOMINUS, MS_OMINUS, TGSUM, 0},
    { "oper", TOPER, '\0', TGOPER, 5},
    { "oplus", TOPLUS, MS_OPLUS, TGSUM, 0},
    { "or", TOR, MS_OR, TGSUM, 0},
    { "ortho", TORTHO, MS_ORTHO, TGRELATION, 0},
    { "otimes", TOTIMES, MS_OTIMES, TGPRODUCT, 0},
    { "over", TOVER, '\0', TGPRODUCT, 0},
    { "overbrace", TOVERBRACE, MS_OVERBRACE, TGPRODUCT, 5},
    { "overline", TOVERLINE, '\0', TGATTRIBUT, 5},
    { "overstrike", TOVERSTRIKE, '\0', TGATTRIBUT, 5},
    { "owns", TNI, MS_NI, TGRELATION, 0},
    { "parallel", TPARALLEL, MS_DLINE, TGRELATION, 0},
    { "partial", TPARTIAL, MS_PARTIAL, TGSTANDALONE, 5 },
    { "phantom", TPHANTOM, '\0', TGFONTATTR, 5},
    { "plusminus", TPLUSMINUS, MS_PLUSMINUS, TGUNOPER | TGSUM, 5},
    { "prod", TPROD, MS_PROD, TGOPER, 5},
    { "prop", TPROP, MS_PROP, TGRELATION, 0},
    { "rangle", TRANGLE, MS_RANGLE, TGRBRACES, 0},	//! 0 to terminate expression
    { "rbrace", TRBRACE, MS_RBRACE, TGRBRACES, 0},	//
    { "rceil", TRCEIL, MS_RCEIL, TGRBRACES, 0},	//
    { "rdbracket", TRDBRACKET, MS_RDBRACKET, TGRBRACES, 0},	//
    { "rdline", TRDLINE, MS_DLINE, TGRBRACES, 0},	//
    { "red", TRED, '\0', TGCOLOR, 0},
    { "rfloor", TRFLOOR, MS_RFLOOR, TGRBRACES, 0},	//! 0 to terminate expression
    { "right", TRIGHT, '\0', 0, 0},
    { "rightarrow" , TRIGHTARROW, MS_RIGHTARROW, TGSTANDALONE, 5},
    { "rline", TRLINE, MS_LINE, TGRBRACES, 0},	//! 0 to terminate expression
    { "rsub", TRSUB, '\0', TGPOWER, 0},
    { "rsup", TRSUP, '\0', TGPOWER, 0},
    { "sans", TSANS, '\0', TGFONT, 0},
    { "serif", TSERIF, '\0', TGFONT, 0},
    { "setC" , TSETC, MS_SETC, TGSTANDALONE, 5},
    { "setN" , TSETN, MS_SETN, TGSTANDALONE, 5},
    { "setQ" , TSETQ, MS_SETQ, TGSTANDALONE, 5},
    { "setR" , TSETR, MS_SETR, TGSTANDALONE, 5},
    { "setZ" , TSETZ, MS_SETZ, TGSTANDALONE, 5},
    { "setminus", TBACKSLASH, MS_BACKSLASH, TGPRODUCT, 0 },
    { "sim", TSIM, MS_SIM, TGRELATION, 0},
    { "simeq", TSIMEQ, MS_SIMEQ, TGRELATION, 0},
    { "sin", TSIN, '\0', TGFUNCTION, 5},
    { "sinh", TSINH, '\0', TGFUNCTION, 5},
    { "size", TSIZE, '\0', TGFONTATTR, 5},
    { "slash", TSLASH, MS_SLASH, TGPRODUCT, 0 },
    { "sqrt", TSQRT, MS_SQRT, TGUNOPER, 5},
    { "stack", TSTACK, '\0', 0, 5},
    { "sub", TRSUB, '\0', TGPOWER, 0},
    { "subset", TSUBSET, MS_SUBSET, TGRELATION, 0},
    { "subseteq", TSUBSETEQ, MS_SUBSETEQ, TGRELATION, 0},
    { "sum", TSUM, MS_SUM, TGOPER, 5},
    { "sup", TRSUP, '\0', TGPOWER, 0},
    { "supset", TSUPSET, MS_SUPSET, TGRELATION, 0},
    { "supseteq", TSUPSETEQ, MS_SUPSETEQ, TGRELATION, 0},
    { "tan", TTAN, '\0', TGFUNCTION, 5},
    { "tanh", TTANH, '\0', TGFUNCTION, 5},
    { "tilde", TTILDE, MS_TILDE, TGATTRIBUT, 5},
    { "times", TTIMES, MS_TIMES, TGPRODUCT, 0},
    { "to", TTO, '\0', TGLIMIT, 0},
    { "toward", TTOWARD, MS_RIGHTARROW, TGRELATION, 0},
    { "transl", TTRANSL, MS_TRANSL, TGRELATION, 0},
    { "transr", TTRANSR, MS_TRANSR, TGRELATION, 0},
    { "underbrace", TUNDERBRACE, MS_UNDERBRACE, TGPRODUCT, 5},
    { "underline", TUNDERLINE, '\0', TGATTRIBUT, 5},
    { "union", TUNION, MS_UNION, TGSUM, 0},
    { "uoper", TUOPER, '\0', TGUNOPER, 5},
    { "uparrow" , TUPARROW, MS_UPARROW, TGSTANDALONE, 5},
    { "vec", TVEC, MS_VEC, TGATTRIBUT, 5},
    { "white", TWHITE, '\0', TGCOLOR, 0},
    { "widebslash", TWIDEBACKSLASH, MS_BACKSLASH, TGPRODUCT, 0 },
    { "widehat", TWIDEHAT, MS_HAT, TGATTRIBUT, 5},
    { "widetilde", TWIDETILDE, MS_TILDE, TGATTRIBUT, 5},
    { "wideslash", TWIDESLASH, MS_SLASH, TGPRODUCT, 0 },
    { "widevec", TWIDEVEC, MS_VEC, TGATTRIBUT, 5},
    { "wp" , TWP, MS_WP, TGSTANDALONE, 5},
    { "yellow", TYELLOW, '\0', TGCOLOR, 0},
//	{ "[", TLBRACKET, MS_LBRACKET, TGLBRACES, 5},	//! 5 to continue expression
//	{ "\\", TESCAPE, '\0', 0, 5},
//	{ "]", TRBRACKET, MS_RBRACKET, TGRBRACES, 0},	//! 0 to terminate expression
//	{ "^", TRSUP, '\0', TGPOWER, 0},
//	{ "_", TRSUB, '\0', TGPOWER, 0},
//	{ "`", TSBLANK, '\0', TGBLANK, 5},
//	{ "{", TLGROUP, MS_LBRACE, 0, 5},		//! 5 to continue expression
//	{ "|", TOR, MS_OR, TGSUM, 0},
//	{ "}", TRGROUP, MS_RBRACE, 0, 0},		//! 0 to terminate expression
//	{ "~", TBLANK, '\0', TGBLANK, 5},
    { "", TEND, '\0', 0, 0}
};


static const SmTokenTableEntry * GetTokenTableEntry( const String &rName )
{
    const SmTokenTableEntry * pRes = 0;
    if (rName.Len())
    {
        INT32 nEntries = sizeof( aTokenTable ) / sizeof( aTokenTable[0] );
        for (INT32 i = 0;  i < nEntries;  ++i)
        {
            if (rName.EqualsIgnoreCaseAscii( aTokenTable[i].pIdent ))
            {
                pRes = &aTokenTable[i];
                break;
            }
        }

    }

    return pRes;
}


///////////////////////////////////////////////////////////////////////////

#if OSL_DEBUG_LEVEL

BOOL SmParser::IsDelimiter( const String &rTxt, xub_StrLen nPos )
    // returns 'TRUE' iff cChar is '\0' or a delimeter
{
    DBG_ASSERT( nPos <= rTxt.Len(), "index out of range" );

    sal_Unicode cChar = rTxt.GetChar( nPos );
    if(!cChar)
        return TRUE;

    // check if 'cChar' is in the delimeter table
    const sal_Unicode *pDelim = &aDelimiterTable[0];
    for ( ;  *pDelim != 0;  pDelim++)
        if (*pDelim == cChar)
            break;

    BOOL bIsDelim = *pDelim != 0;

    INT16 nTypJp = SM_MOD1()->GetSysLocale().GetCharClass().getType( rTxt, nPos );
    bIsDelim |= nTypJp == com::sun::star::i18n::UnicodeType::SPACE_SEPARATOR ||
                nTypJp == com::sun::star::i18n::UnicodeType::CONTROL;

    return bIsDelim;
}

#endif

void SmParser::Insert(const String &rText, USHORT nPos)
{
    BufferString.Insert(rText, nPos);

    xub_StrLen  nLen = rText.Len();
    BufferIndex = BufferIndex + nLen;
    nTokenIndex = nTokenIndex + nLen;
}


void SmParser::Replace( USHORT nPos, USHORT nLen, const String &rText )
{
    DBG_ASSERT( nPos + nLen <= BufferString.Len(), "argument mismatch" );

    BufferString.Replace( nPos, nLen, rText );
    INT16  nChg = rText.Len() - nLen;
    BufferIndex = BufferIndex + nChg;
    nTokenIndex = nTokenIndex + nChg;
}


// First character may be any alphabetic
const sal_Int32 coStartFlags =
        KParseTokens::ANY_LETTER_OR_NUMBER |
        KParseTokens::IGNORE_LEADING_WS;

// Continuing characters may be any alphanumeric or dot.
const sal_Int32 coContFlags =
    ( coStartFlags | KParseTokens::ASC_DOT ) & ~KParseTokens::IGNORE_LEADING_WS
    | KParseTokens::TWO_DOUBLE_QUOTES_BREAK_STRING;

// First character for numbers, may be any numeric or dot
const sal_Int32 coNumStartFlags =
        KParseTokens::ASC_DIGIT |
        KParseTokens::ASC_DOT |
        KParseTokens::IGNORE_LEADING_WS;
// Continuing characters for numbers, may be any numeric or dot.
const sal_Int32 coNumContFlags =
    ( coNumStartFlags | KParseTokens::ASC_DOT ) & ~KParseTokens::IGNORE_LEADING_WS;

void SmParser::NextToken()
{
    static const String aEmptyStr;

    xub_StrLen	nBufLen = BufferString.Len();
    ParseResult	aRes;
    xub_StrLen	nRealStart;
    BOOL		bCont;
    BOOL        bNumStart = FALSE;
    CharClass   aCC(SM_MOD1()->GetSysLocale().GetCharClass().getLocale());
    do
    {
        // skip white spaces
        while (UnicodeType::SPACE_SEPARATOR ==
                        aCC.getType( BufferString, BufferIndex ))
           ++BufferIndex;

        sal_Int32 nStartFlags = coStartFlags;
        sal_Int32 nContFlags  = coContFlags;
        sal_Unicode cFirstChar = BufferString.GetChar( BufferIndex );
/*
        removed because of #i11752#
        bNumStart = cFirstChar == '.' || ('0' <= cFirstChar && cFirstChar <= '9');
        if (bNumStart)
        {
            nStartFlags = coNumStartFlags;
            nContFlags  = coNumContFlags;
        }
*/
        aRes = aCC.parseAnyToken( BufferString, BufferIndex,
                                            nStartFlags, aEmptyStr,
                                            nContFlags, aEmptyStr );

        // #i45779# parse numbers correctly
        // i.e. independent from the locale setting.
        // (note that #i11752# remains fixed)
        if ((aRes.TokenType & KParseType::IDENTNAME) && IsDigit( cFirstChar ))
        {
            //! locale where '.' is decimal seperator!
            static lang::Locale aDotLoc( SvxCreateLocale( LANGUAGE_ENGLISH_US ) );

            ParseResult aTmpRes;
            lang::Locale aOldLoc( aCC.getLocale() );
            aCC.setLocale( aDotLoc );
            aTmpRes = aCC.parsePredefinedToken( 
                            KParseType::ASC_NUMBER,
                            BufferString, BufferIndex,
                            KParseTokens::ASC_DIGIT, aEmptyStr,
                            KParseTokens::ASC_DIGIT | KParseTokens::ASC_DOT, aEmptyStr );
            aCC.setLocale( aOldLoc );
            if (aTmpRes.TokenType & KParseType::ASC_NUMBER)
                aRes.TokenType = aTmpRes.TokenType;
        }

        nRealStart = BufferIndex + sal::static_int_cast< xub_StrLen >(aRes.LeadingWhiteSpace);
        BufferIndex = nRealStart;

        bCont = FALSE;
        if ( aRes.TokenType == 0  &&
                nRealStart < nBufLen &&
                '\n' == BufferString.GetChar( nRealStart ) )
        {
            // keep data needed for tokens row and col entry up to date
            ++Row;
            BufferIndex = ColOff = nRealStart + 1;
            bCont = TRUE;
        }
        else if (aRes.TokenType & KParseType::ONE_SINGLE_CHAR)
        {
            String aName( BufferString.Copy( nRealStart, 2 ));
            if ( aName.EqualsAscii( "%%" ))
            {
                //SkipComment
                BufferIndex = nRealStart + 2;
                while (BufferIndex < nBufLen  &&
                    '\n' != BufferString.GetChar( BufferIndex ))
                    ++BufferIndex;
                bCont = TRUE;
            }
        }

    } while (bCont);

    // set index of current token
    nTokenIndex = BufferIndex;

    CurToken.nRow	= Row;
    CurToken.nCol	= nRealStart - ColOff + 1;

    BOOL bHandled = TRUE;
    if (nRealStart >= nBufLen)
    {
        CurToken.eType	   = TEND;
        CurToken.cMathChar = '\0';
        CurToken.nGroup	   = 0;
        CurToken.nLevel	   = 0;
        CurToken.aText.Erase();
    }
    else if ((aRes.TokenType & (KParseType::ASC_NUMBER | KParseType::UNI_NUMBER))
             || (bNumStart && (aRes.TokenType & KParseType::IDENTNAME)))
    {
        INT32 n = aRes.EndPos - nRealStart;
        DBG_ASSERT( n >= 0, "length < 0" );
        CurToken.eType      = TNUMBER;
        CurToken.cMathChar  = '\0';
        CurToken.nGroup     = 0;
        CurToken.nLevel     = 5;
        CurToken.aText      = BufferString.Copy( nRealStart, sal::static_int_cast< xub_StrLen >(n) );

#if OSL_DEBUG_LEVEL > 1
        if (!IsDelimiter( BufferString, static_cast< xub_StrLen >(aRes.EndPos) ))
        {
            DBG_WARNING( "identifier really finished? (compatibility!)" );
        }
#endif
    }
    else if (aRes.TokenType & KParseType::DOUBLE_QUOTE_STRING)
    {
        CurToken.eType      = TTEXT;
        CurToken.cMathChar  = '\0';
        CurToken.nGroup     = 0;
        CurToken.nLevel     = 5;
        CurToken.aText		= aRes.DequotedNameOrString;
        CurToken.nRow       = Row;
        CurToken.nCol       = nRealStart - ColOff + 2;
    }
    else if (aRes.TokenType & KParseType::IDENTNAME)
    {
        INT32 n = aRes.EndPos - nRealStart;
        DBG_ASSERT( n >= 0, "length < 0" );
        String aName( BufferString.Copy( nRealStart, sal::static_int_cast< xub_StrLen >(n) ) );
        const SmTokenTableEntry *pEntry = GetTokenTableEntry( aName );

        if (pEntry)
        {
            CurToken.eType      = pEntry->eType;
            CurToken.cMathChar  = pEntry->cMathChar;
            CurToken.nGroup     = pEntry->nGroup;
            CurToken.nLevel     = pEntry->nLevel;
            CurToken.aText.AssignAscii( pEntry->pIdent );
        }
        else
        {
            CurToken.eType      = TIDENT;
            CurToken.cMathChar  = '\0';
            CurToken.nGroup     = 0;
            CurToken.nLevel     = 5;
            CurToken.aText      = aName;

#if OSL_DEBUG_LEVEL > 1
            if (!IsDelimiter( BufferString, static_cast< xub_StrLen >(aRes.EndPos) ))
            {
                DBG_WARNING( "identifier really finished? (compatibility!)" );
            }
#endif
        }
    }
    else if (aRes.TokenType == 0  &&  '_' == BufferString.GetChar( nRealStart ))
    {
        CurToken.eType	   = TRSUB;
        CurToken.cMathChar = '\0';
        CurToken.nGroup	   = TGPOWER;
        CurToken.nLevel	   = 0;
        CurToken.aText.AssignAscii( "_" );

        aRes.EndPos = nRealStart + 1;
    }
    else if (aRes.TokenType & KParseType::BOOLEAN)
    {
        sal_Int32   &rnEndPos = aRes.EndPos;
        String  aName( BufferString.Copy( nRealStart, 
                        sal::static_int_cast< xub_StrLen >(rnEndPos - nRealStart) ));
        if (2 >= aName.Len())
        {
            sal_Unicode ch = aName.GetChar( 0 );
            switch (ch)
            {
                case '<':
                    {
                        if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "<<" ))
                        {
                            CurToken.eType	   = TLL;
                            CurToken.cMathChar = MS_LL;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "<<" );

                            rnEndPos = nRealStart + 2;
                        }
                        else if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "<=" ))
                        {
                            CurToken.eType	   = TLE;
                            CurToken.cMathChar = MS_LE;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "<=" );

                            rnEndPos = nRealStart + 2;
                        }
                        else if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "<>" ))
                        {
                            CurToken.eType	   = TNEQ;
                            CurToken.cMathChar = MS_NEQ;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "<>" );

                            rnEndPos = nRealStart + 2;
                        }
                        else if (BufferString.Copy( nRealStart, 3 ).
                                EqualsAscii( "<?>" ))
                        {
                            CurToken.eType	   = TPLACE;
                            CurToken.cMathChar = MS_PLACE;
                            CurToken.nGroup	   = 0;
                            CurToken.nLevel	   = 5;
                            CurToken.aText.AssignAscii( "<?>" );

                            rnEndPos = nRealStart + 3;
                        }
                        else
                        {
                            CurToken.eType	   = TLT;
                            CurToken.cMathChar = MS_LT;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "<" );
                        }
                    }
                    break;
                case '>':
                    {
                        if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( ">=" ))
                        {
                            CurToken.eType	   = TGE;
                            CurToken.cMathChar = MS_GE;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( ">=" );

                            rnEndPos = nRealStart + 2;
                        }
                        else if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( ">>" ))
                        {
                            CurToken.eType	   = TGG;
                            CurToken.cMathChar = MS_GG;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( ">>" );

                            rnEndPos = nRealStart + 2;
                        }
                        else
                        {
                            CurToken.eType	   = TGT;
                            CurToken.cMathChar = MS_GT;
                            CurToken.nGroup	   = TGRELATION;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( ">" );
                        }
                    }
                    break;
                default:
                    bHandled = FALSE;
            }
        }
    }
    else if (aRes.TokenType & KParseType::ONE_SINGLE_CHAR)
    {
        sal_Int32   &rnEndPos = aRes.EndPos;
        String  aName( BufferString.Copy( nRealStart, 
                            sal::static_int_cast< xub_StrLen >(rnEndPos - nRealStart) ) );

        if (1 == aName.Len())
        {
            sal_Unicode ch = aName.GetChar( 0 );
            switch (ch)
            {
                case '%':
                    {
                        //! modifies aRes.EndPos

                        DBG_ASSERT( rnEndPos >= nBufLen  ||
                                    '%' != BufferString.GetChar( sal::static_int_cast< xub_StrLen >(rnEndPos) ),
                                "unexpected comment start" );

                        // get identifier of user-defined character
                        ParseResult aTmpRes = aCC.parseAnyToken(
                                BufferString, rnEndPos,
                                KParseTokens::ANY_LETTER,
                                aEmptyStr,
                                coContFlags,
                                aEmptyStr );

                        xub_StrLen nTmpStart = sal::static_int_cast< xub_StrLen >(rnEndPos +
                                                    aTmpRes.LeadingWhiteSpace);

                        // default setting fo the case that no identifier
                        // i.e. a valid symbol-name is following the '%'
                        // character
                        CurToken.eType      = TTEXT;
                        CurToken.cMathChar  = '\0';
                        CurToken.nGroup     = 0;
                        CurToken.nLevel     = 5;
                        CurToken.aText      = String();
                        CurToken.nRow       = sal::static_int_cast< xub_StrLen >(Row);
                        CurToken.nCol       = nTmpStart - ColOff + 1;

                        if (aTmpRes.TokenType & KParseType::IDENTNAME)
                        {

                            xub_StrLen n = sal::static_int_cast< xub_StrLen >(aTmpRes.EndPos - nTmpStart);
                            CurToken.eType      = TSPECIAL;
                            CurToken.aText      = BufferString.Copy( sal::static_int_cast< xub_StrLen >(nTmpStart), n );

                            DBG_ASSERT( aTmpRes.EndPos > rnEndPos,
                                    "empty identifier" );
                            if (aTmpRes.EndPos > rnEndPos)
                                rnEndPos = aTmpRes.EndPos;
                            else
                                ++rnEndPos;
                        }

                        // if no symbol-name was found we start-over with
                        // finding the next token right afer the '%' sign.
                        // I.e. we leave rnEndPos unmodified.
                    }
                    break;
                case '[':
                    {
                        CurToken.eType	   = TLBRACKET;
                        CurToken.cMathChar = MS_LBRACKET;
                        CurToken.nGroup	   = TGLBRACES;
                        CurToken.nLevel	   = 5;
                        CurToken.aText.AssignAscii( "[" );
                    }
                    break;
                case '\\':
                    {
                        CurToken.eType	   = TESCAPE;
                        CurToken.cMathChar = '\0';
                        CurToken.nGroup	   = 0;
                        CurToken.nLevel	   = 5;
                        CurToken.aText.AssignAscii( "\\" );
                    }
                    break;
                case ']':
                    {
                        CurToken.eType	   = TRBRACKET;
                        CurToken.cMathChar = MS_RBRACKET;
                        CurToken.nGroup	   = TGRBRACES;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "]" );
                    }
                    break;
                case '^':
                    {
                        CurToken.eType	   = TRSUP;
                        CurToken.cMathChar = '\0';
                        CurToken.nGroup	   = TGPOWER;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "^" );
                    }
                    break;
                case '`':
                    {
                        CurToken.eType	   = TSBLANK;
                        CurToken.cMathChar = '\0';
                        CurToken.nGroup	   = TGBLANK;
                        CurToken.nLevel	   = 5;
                        CurToken.aText.AssignAscii( "`" );
                    }
                    break;
                case '{':
                    {
                        CurToken.eType	   = TLGROUP;
                        CurToken.cMathChar = MS_LBRACE;
                        CurToken.nGroup	   = 0;
                        CurToken.nLevel	   = 5;
                        CurToken.aText.AssignAscii( "{" );
                    }
                    break;
                case '|':
                    {
                        CurToken.eType	   = TOR;
                        CurToken.cMathChar = MS_OR;
                        CurToken.nGroup	   = TGSUM;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "|" );
                    }
                    break;
                case '}':
                    {
                        CurToken.eType	   = TRGROUP;
                        CurToken.cMathChar = MS_RBRACE;
                        CurToken.nGroup	   = 0;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "}" );
                    }
                    break;
                case '~':
                    {
                        CurToken.eType	   = TBLANK;
                        CurToken.cMathChar = '\0';
                        CurToken.nGroup	   = TGBLANK;
                        CurToken.nLevel	   = 5;
                        CurToken.aText.AssignAscii( "~" );
                    }
                    break;
                case '#':
                    {
                        if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "##" ))
                        {
                            CurToken.eType	   = TDPOUND;
                            CurToken.cMathChar = '\0';
                            CurToken.nGroup	   = 0;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "##" );

                            rnEndPos = nRealStart + 2;
                        }
                        else
                        {
                            CurToken.eType	   = TPOUND;
                            CurToken.cMathChar = '\0';
                            CurToken.nGroup	   = 0;
                            CurToken.nLevel	   = 0;
                            CurToken.aText.AssignAscii( "#" );
                        }
                    }
                    break;
                case '&':
                    {
                        CurToken.eType	   = TAND;
                        CurToken.cMathChar = MS_AND;
                        CurToken.nGroup	   = TGPRODUCT;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "&" );
                    }
                    break;
                case '(':
                    {
                        CurToken.eType	   = TLPARENT;
                        CurToken.cMathChar = MS_LPARENT;
                        CurToken.nGroup	   = TGLBRACES;
                        CurToken.nLevel	   = 5;		//! 0 to continue expression
                        CurToken.aText.AssignAscii( "(" );
                    }
                    break;
                case ')':
                    {
                        CurToken.eType	   = TRPARENT;
                        CurToken.cMathChar = MS_RPARENT;
                        CurToken.nGroup	   = TGRBRACES;
                        CurToken.nLevel	   = 0;		//! 0 to terminate expression
                        CurToken.aText.AssignAscii( ")" );
                    }
                    break;
                case '*':
                    {
                        CurToken.eType	   = TMULTIPLY;
                        CurToken.cMathChar = MS_MULTIPLY;
                        CurToken.nGroup	   = TGPRODUCT;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "*" );
                    }
                    break;
                case '+':
                    {
                        if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "+-" ))
                        {
                            CurToken.eType	   = TPLUSMINUS;
                            CurToken.cMathChar = MS_PLUSMINUS;
                            CurToken.nGroup	   = TGUNOPER | TGSUM;
                            CurToken.nLevel	   = 5;
                            CurToken.aText.AssignAscii( "+-" );

                            rnEndPos = nRealStart + 2;
                        }
                        else
                        {
                            CurToken.eType	   = TPLUS;
                            CurToken.cMathChar = MS_PLUS;
                            CurToken.nGroup	   = TGUNOPER | TGSUM;
                            CurToken.nLevel	   = 5;
                            CurToken.aText.AssignAscii( "+" );
                        }
                    }
                    break;
                case '-':
                    {
                        if (BufferString.Copy( nRealStart, 2 ).
                                EqualsAscii( "-+" ))
                        {
                            CurToken.eType	   = TMINUSPLUS;
                            CurToken.cMathChar = MS_MINUSPLUS;
                            CurToken.nGroup	   = TGUNOPER | TGSUM;
                            CurToken.nLevel	   = 5;
                            CurToken.aText.AssignAscii( "-+" );

                            rnEndPos = nRealStart + 2;
                        }
                        else
                        {
                            CurToken.eType	   = TMINUS;
                            CurToken.cMathChar = MS_MINUS;
                            CurToken.nGroup	   = TGUNOPER | TGSUM;
                            CurToken.nLevel	   = 5;
                            CurToken.aText.AssignAscii( "-" );
                        }
                    }
                    break;
                case '.':
                    {
                        // for compatibility with SO5.2
                        // texts like .34 ...56 ... h ...78..90
                        // will be treated as numbers
                        CurToken.eType     = TNUMBER;
                        CurToken.cMathChar = '\0';
                        CurToken.nGroup	   = 0;
                        CurToken.nLevel    = 5;

                        xub_StrLen nTxtStart = BufferIndex;
                        sal_Unicode cChar;
                        do
                        {
                            cChar = BufferString.GetChar( ++BufferIndex );
                        }
                        while ( cChar == '.' || IsDigit( cChar ) );

                        CurToken.aText = BufferString.Copy( sal::static_int_cast< xub_StrLen >(nTxtStart), 
                                                            sal::static_int_cast< xub_StrLen >(BufferIndex - nTxtStart) );
                        aRes.EndPos = BufferIndex;
                    }
                    break;
                case '/':
                    {
                        CurToken.eType	   = TDIVIDEBY;
                        CurToken.cMathChar = MS_SLASH;
                        CurToken.nGroup	   = TGPRODUCT;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "/" );
                    }
                    break;
                case '=':
                    {
                        CurToken.eType	   = TASSIGN;
                        CurToken.cMathChar = MS_ASSIGN;
                        CurToken.nGroup	   = TGRELATION;
                        CurToken.nLevel	   = 0;
                        CurToken.aText.AssignAscii( "=" );
                    }
                    break;
                default:
                    bHandled = FALSE;
            }
        }
    }
    else
        bHandled = FALSE;

    if (!bHandled)
    {
        CurToken.eType      = TCHARACTER;
        CurToken.cMathChar  = '\0';
        CurToken.nGroup     = 0;
        CurToken.nLevel     = 5;
        CurToken.aText      = BufferString.Copy( nRealStart, 1 );

        aRes.EndPos = nRealStart + 1;
    }

    if (TEND != CurToken.eType)
        BufferIndex = sal::static_int_cast< xub_StrLen >(aRes.EndPos);
}


////////////////////////////////////////
// grammar
//


void SmParser::Table()
{
    SmNodeArray  LineArray;

    Line();
    while (CurToken.eType == TNEWLINE)
    {
        NextToken();
        Line();
    }

    if (CurToken.eType != TEND)
        Error(PE_UNEXPECTED_CHAR);

    ULONG n = NodeStack.Count();

    LineArray.SetSize(n);

    for (ULONG i = 0; i < n; i++)
        LineArray.Put(n - (i + 1), NodeStack.Pop());

    SmStructureNode *pSNode = new SmTableNode(CurToken);
    pSNode->SetSubNodes(LineArray);
    NodeStack.Push(pSNode);
}


void SmParser::Align()
    // parse alignment info (if any), then go on with rest of expression
{
    SmStructureNode *pSNode = 0;
    BOOL    bNeedGroupClose = FALSE;

    if (TokenInGroup(TGALIGN))
    {
        if (CONVERT_40_TO_50 == GetConversion())
            // encapsulate expression to be aligned in group braces
            // (here group-open brace)
        {	Insert('{', GetTokenIndex());
            bNeedGroupClose = TRUE;

            // get first valid align statement in sequence
            // (the dominant one in 4.0) and erase all others (especially old
            // discarded tokens) from command string.
            while (TokenInGroup(TGALIGN))
            {	if (TokenInGroup(TGDISCARDED) || pSNode)
                {   BufferIndex = GetTokenIndex();
                    BufferString.Erase(BufferIndex, CurToken.aText.Len());
                }
                else
                    pSNode = new SmAlignNode(CurToken);

                NextToken();
            }
        }
        else
        {
            pSNode = new SmAlignNode(CurToken);

            NextToken();

            // allow for just one align statement in 5.0
            if (CONVERT_40_TO_50 != GetConversion() && TokenInGroup(TGALIGN))
            {	Error(PE_DOUBLE_ALIGN);
                return;
            }
        }
    }

    Expression();

    if (bNeedGroupClose)
        Insert('}', GetTokenIndex());

    if (pSNode)
    {	pSNode->SetSubNodes(NodeStack.Pop(), 0);
        NodeStack.Push(pSNode);
    }
}


void SmParser::Line()
{
    USHORT  n = 0;
    SmNodeArray  ExpressionArray;

    ExpressionArray.SetSize(n);

    // start with single expression that may have an alignment statement
    // (and go on with expressions that must not have alignment
    // statements in 'while' loop below. See also 'Expression()'.)
    if (CurToken.eType != TEND  &&  CurToken.eType != TNEWLINE)
    {	Align();
        ExpressionArray.SetSize(++n);
        ExpressionArray.Put(n - 1, NodeStack.Pop());
    }

    while (CurToken.eType != TEND  &&  CurToken.eType != TNEWLINE)
    {   if (CONVERT_40_TO_50 != GetConversion())
            Expression();
        else
            Align();
        ExpressionArray.SetSize(++n);
        ExpressionArray.Put(n - 1, NodeStack.Pop());
    }

    SmStructureNode *pSNode = new SmLineNode(CurToken);
    pSNode->SetSubNodes(ExpressionArray);
    NodeStack.Push(pSNode);
}


void SmParser::Expression()
{
    USHORT		 n = 0;
    SmNodeArray  RelationArray;

    RelationArray.SetSize(n);

    Relation();
    RelationArray.SetSize(++n);
    RelationArray.Put(n - 1, NodeStack.Pop());

    while (CurToken.nLevel >= 4)
    {	Relation();
        RelationArray.SetSize(++n);
        RelationArray.Put(n - 1, NodeStack.Pop());
    }

    SmStructureNode *pSNode = new SmExpressionNode(CurToken);
    pSNode->SetSubNodes(RelationArray);
    NodeStack.Push(pSNode);
}


void SmParser::Relation()
{
    Sum();
    while (TokenInGroup(TGRELATION))
    {
        SmStructureNode *pSNode  = new SmBinHorNode(CurToken);
        SmNode *pFirst = NodeStack.Pop();

        OpSubSup();
        SmNode *pSecond = NodeStack.Pop();

        Sum();

        pSNode->SetSubNodes(pFirst, pSecond, NodeStack.Pop());
        NodeStack.Push(pSNode);
    }
}


void SmParser::Sum()
{
    Product();
    while (TokenInGroup(TGSUM))
    {
        SmStructureNode *pSNode  = new SmBinHorNode(CurToken);
        SmNode *pFirst = NodeStack.Pop();

        OpSubSup();
        SmNode *pSecond = NodeStack.Pop();

        Product();

        pSNode->SetSubNodes(pFirst, pSecond, NodeStack.Pop());
        NodeStack.Push(pSNode);
    }
}


void SmParser::Product()
{
    Power();

    while (TokenInGroup(TGPRODUCT))
    {	SmStructureNode *pSNode;
        SmNode *pFirst = NodeStack.Pop(),
               *pOper;
        BOOL bSwitchArgs = FALSE;

        SmTokenType eType = CurToken.eType;
        switch (eType)
        {
            case TOVER:
                pSNode = new SmBinVerNode(CurToken);
                pOper = new SmRectangleNode(CurToken);
                NextToken();
                break;

            case TBOPER:
                pSNode = new SmBinHorNode(CurToken);

                NextToken();

                GlyphSpecial();
                pOper = NodeStack.Pop();
                break;

            case TOVERBRACE :
            case TUNDERBRACE :
                pSNode = new SmVerticalBraceNode(CurToken);
                pOper = new SmMathSymbolNode(CurToken);

                NextToken();
                break;

            case TWIDEBACKSLASH:
            case TWIDESLASH:
            {
                SmBinDiagonalNode *pSTmp = new SmBinDiagonalNode(CurToken);
                pSTmp->SetAscending(eType == TWIDESLASH);
                pSNode = pSTmp;

                pOper = new SmPolyLineNode(CurToken);
                NextToken();

                bSwitchArgs =TRUE;
                break;
            }

            default:
                pSNode = new SmBinHorNode(CurToken);

                OpSubSup();
                pOper = NodeStack.Pop();
        }

        Power();

        if (bSwitchArgs)
            //! vgl siehe SmBinDiagonalNode::Arrange
            pSNode->SetSubNodes(pFirst, NodeStack.Pop(), pOper);
        else
            pSNode->SetSubNodes(pFirst, pOper, NodeStack.Pop());
        NodeStack.Push(pSNode);
    }
}


void SmParser::SubSup(ULONG nActiveGroup)
{
    DBG_ASSERT(nActiveGroup == TGPOWER  ||  nActiveGroup == TGLIMIT,
               "Sm: falsche Tokengruppe");

    if (!TokenInGroup(nActiveGroup))
        // already finish
        return;

    SmSubSupNode *pNode = new SmSubSupNode(CurToken);
    //! Of course 'CurToken' ist just the first sub-/supscript token.
    //! It should be of no further interest. The positions of the
    //! sub-/supscripts will be identified by the corresponding subnodes
    //! index in the 'aSubNodes' array (enum value from 'SmSubSup').

    pNode->SetUseLimits(nActiveGroup == TGLIMIT);

    // initialize subnodes array
    SmNodeArray  aSubNodes;
    aSubNodes.SetSize(1 + SUBSUP_NUM_ENTRIES);
    aSubNodes.Put(0, NodeStack.Pop());
    for (USHORT i = 1;  i < aSubNodes.GetSize();  i++)
        aSubNodes.Put(i, NULL);

    // process all sub-/supscripts
    int  nIndex = 0;
    while (TokenInGroup(nActiveGroup))
    {	SmTokenType  eType (CurToken.eType);

        // skip sub-/supscript token
        NextToken();

        // get sub-/supscript node on top of stack
        if (eType == TFROM  ||  eType == TTO)
        {
            // parse limits in old 4.0 and 5.0 style
            Relation();
        }
        else
            Term();

        switch (eType)
        {	case TRSUB :	nIndex = (int) RSUB;	break;
            case TRSUP :	nIndex = (int) RSUP;	break;
            case TFROM :
            case TCSUB :	nIndex = (int) CSUB;	break;
            case TTO :
            case TCSUP :	nIndex = (int) CSUP;	break;
            case TLSUB :	nIndex = (int) LSUB;	break;
            case TLSUP :	nIndex = (int) LSUP;	break;
            default :
                DBG_ASSERT(FALSE, "Sm: unbekannter Fall");
        }
        nIndex++;
        DBG_ASSERT(1 <= nIndex	&&	nIndex <= 1 + SUBSUP_NUM_ENTRIES,
                   "SmParser::Power() : sub-/supscript index falsch")

        // set sub-/supscript if not already done
        if (aSubNodes.Get(nIndex) != NULL)
            Error(PE_DOUBLE_SUBSUPSCRIPT);
        aSubNodes.Put(nIndex, NodeStack.Pop());
    }

    pNode->SetSubNodes(aSubNodes);
    NodeStack.Push(pNode);
}


void SmParser::OpSubSup()
{
    // push operator symbol
    NodeStack.Push(new SmMathSymbolNode(CurToken));
    // skip operator token
    NextToken();
    // get sub- supscripts if any
    if (TokenInGroup(TGPOWER))
        SubSup(TGPOWER);
}


void SmParser::Power()
{
    // get body for sub- supscripts on top of stack
    Term();

    SubSup(TGPOWER);
}


void SmParser::Blank()
{
    DBG_ASSERT(TokenInGroup(TGBLANK), "Sm : falsches Token");
    SmBlankNode *pBlankNode = new SmBlankNode(CurToken);

    while (TokenInGroup(TGBLANK))
    {
        pBlankNode->IncreaseBy(CurToken);
        NextToken();
    }

    // Blanks am Zeilenende ignorieren wenn die entsprechende Option gesetzt ist
    if (CurToken.eType == TNEWLINE  ||  CurToken.eType == TEND
        &&  SM_MOD1()->GetConfig()->IsIgnoreSpacesRight())
        pBlankNode->Clear();

    NodeStack.Push(pBlankNode);
}


void SmParser::Term()
{
    switch (CurToken.eType)
    {	case TESCAPE :
            Escape();
            break;

        case TLGROUP :
            NextToken();

            // allow for empty group
            if (CurToken.eType == TRGROUP)
            {	SmStructureNode *pSNode = new SmExpressionNode(CurToken);
                pSNode->SetSubNodes(NULL, NULL);
                NodeStack.Push(pSNode);

                NextToken();
            }
            else	// go as usual
            {	Align();
                if (CurToken.eType != TRGROUP)
                    Error(PE_RGROUP_EXPECTED);
                else
                {	NextToken();
                }
            }
            break;

        case TLEFT :
            Brace();
            break;

        case TBLANK :
        case TSBLANK :
            Blank();
            break;

        case TTEXT :
            NodeStack.Push(new SmTextNode(CurToken, FNT_TEXT));
            NextToken();
            break;
        case TIDENT :
        case TCHARACTER :
            NodeStack.Push(new SmTextNode(CurToken, FNT_VARIABLE));
            NextToken();
            break;
        case TNUMBER :
            NodeStack.Push(new SmTextNode(CurToken, FNT_NUMBER));
            NextToken();
            break;

        case TLEFTARROW :
        case TRIGHTARROW :
        case TUPARROW :
        case TDOWNARROW :
        case TSETN :
        case TSETZ :
        case TSETQ :
        case TSETR :
        case TSETC :
        case THBAR :
        case TLAMBDABAR :
        case TCIRC :
        case TDRARROW :
        case TDLARROW :
        case TDLRARROW :
        case TBACKEPSILON :
        case TALEPH :
        case TIM :
        case TRE :
        case TWP :
        case TEMPTYSET :
        case TINFINITY :
        case TEXISTS :
        case TFORALL :
        case TPARTIAL :
        case TNABLA :
        case TTOWARD :
        case TDOTSAXIS :
        case TDOTSDIAG :
        case TDOTSDOWN :
        case TDOTSLOW :
        case TDOTSUP :
        case TDOTSVERT :
            NodeStack.Push(new SmMathSymbolNode(CurToken));
            NextToken();
            break;

        case TPLACE:
            NodeStack.Push(new SmPlaceNode(CurToken));
            NextToken();
            break;

        case TSPECIAL:
            Special();
            break;

        case TBINOM:
            Binom();
            break;

        case TSTACK:
            Stack();
            break;

        case TMATRIX:
            Matrix();
            break;

        default:
            if (TokenInGroup(TGLBRACES))
            {	Brace();
            }
            else if (TokenInGroup(TGOPER))
            {	Operator();
            }
            else if (TokenInGroup(TGUNOPER))
            {	UnOper();
            }
            else if (    TokenInGroup(TGATTRIBUT)
                     ||  TokenInGroup(TGFONTATTR))
            {	SmStructureNodeArray  aArray;

                BOOL    bIsAttr;
                USHORT  n = 0;
                while (TRUE == (bIsAttr = TokenInGroup(TGATTRIBUT))
                       ||  TokenInGroup(TGFONTATTR))
                {	aArray.SetSize(n + 1);

                    if (bIsAttr)
                        Attribut();
                    else
                        FontAttribut();

                    // check if casting in following line is ok
                    DBG_ASSERT(!NodeStack.Top()->IsVisible(), "Sm : Ooops...");

                    aArray.Put(n, (SmStructureNode *) NodeStack.Pop());
                    n++;
                }

                Power();

                SmNode *pFirstNode = NodeStack.Pop();
                while (n > 0)
                {	aArray.Get(n - 1)->SetSubNodes(0, pFirstNode);
                    pFirstNode = aArray.Get(n - 1);
                    n--;
                }
                NodeStack.Push(pFirstNode);
            }
            else if (TokenInGroup(TGFUNCTION))
            {   if (CONVERT_40_TO_50 != GetConversion())
                {	Function();
                }
                else	// encapsulate old 4.0 style parsing in braces
                {
                    // insert opening brace
                    Insert('{', GetTokenIndex());

                    //
                    // parse in 4.0 style
                    //
                    Function();

                    SmNode *pFunc = NodeStack.Pop();

                    if (CurToken.eType == TLPARENT)
                    {	Term();
                    }
                    else
                    {	Align();
                    }

                    // insert closing brace
                    Insert('}', GetTokenIndex());

                    SmStructureNode *pSNode = new SmExpressionNode(pFunc->GetToken());
                    pSNode->SetSubNodes(pFunc, NodeStack.Pop());
                    NodeStack.Push(pSNode);
                }
            }
            else
                Error(PE_UNEXPECTED_CHAR);
    }
}


void SmParser::Escape()
{
    NextToken();

    sal_Unicode	cChar;
    switch (CurToken.eType)
    {	case TLPARENT :		cChar = MS_LPARENT;		break;
        case TRPARENT :		cChar = MS_RPARENT;		break;
        case TLBRACKET :	cChar = MS_LBRACKET;	break;
        case TRBRACKET :	cChar = MS_RBRACKET;	break;
        case TLDBRACKET :   cChar = MS_LDBRACKET;   break;
        case TRDBRACKET :   cChar = MS_RDBRACKET;   break;
        case TLBRACE :
        case TLGROUP :		cChar = MS_LBRACE;		break;
        case TRBRACE :
        case TRGROUP :		cChar = MS_RBRACE;		break;
        case TLANGLE :		cChar = MS_LANGLE;		break;
        case TRANGLE :		cChar = MS_RANGLE;		break;
        case TLCEIL :		cChar = MS_LCEIL;		break;
        case TRCEIL :		cChar = MS_RCEIL;		break;
        case TLFLOOR :		cChar = MS_LFLOOR;		break;
        case TRFLOOR :		cChar = MS_RFLOOR;		break;
        case TLLINE :
        case TRLINE :		cChar = MS_LINE;		break;
        case TLDLINE :
        case TRDLINE :		cChar = MS_DLINE;		break;
        default:
            Error(PE_UNEXPECTED_TOKEN);
    }

    SmNode *pNode = new SmMathSymbolNode(CurToken);
    NodeStack.Push(pNode);

    NextToken();
}


void SmParser::Operator()
{
    if (TokenInGroup(TGOPER))
    {	SmStructureNode *pSNode = new SmOperNode(CurToken);

        // put operator on top of stack
        Oper();

        if (TokenInGroup(TGLIMIT) || TokenInGroup(TGPOWER))
            SubSup(CurToken.nGroup);
        SmNode *pOperator = NodeStack.Pop();

        // get argument
        Power();

        pSNode->SetSubNodes(pOperator, NodeStack.Pop());
        NodeStack.Push(pSNode);
    }
}


void SmParser::Oper()
{
    SmTokenType  eType (CurToken.eType);
    SmNode      *pNode = NULL;

    switch (eType)
    {
        case TSUM :
        case TPROD :
        case TCOPROD :
        case TINT :
        case TIINT :
        case TIIINT :
        case TLINT :
        case TLLINT :
        case TLLLINT :
            pNode = new SmMathSymbolNode(CurToken);
            break;

        case TLIM :
        case TLIMSUP :
        case TLIMINF :
            {
                const sal_Char* pLim = 0;
                switch (eType)
                {
                    case TLIM :		pLim = "lim";		break;
                    case TLIMSUP :  pLim = "lim sup";	break;
                    case TLIMINF :  pLim = "lim inf";	break;
                    default:
                        break;
                }
                if( pLim )
                    CurToken.aText.AssignAscii( pLim );
                pNode = new SmTextNode(CurToken, FNT_TEXT);
            }
            break;

        case TOVERBRACE :
        case TUNDERBRACE :
                pNode = new SmMathSymbolNode(CurToken);
            break;

        case TOPER :
            NextToken();

            DBG_ASSERT(CurToken.eType == TSPECIAL, "Sm: falsches Token");
            pNode = new SmGlyphSpecialNode(CurToken);
            break;

        default :
            DBG_ASSERT(0, "Sm: unbekannter Fall");
    }
    NodeStack.Push(pNode);

    NextToken();
}


void SmParser::UnOper()
{
    DBG_ASSERT(TokenInGroup(TGUNOPER), "Sm: falsches Token");

    SmToken		 aNodeToken = CurToken;
    SmTokenType  eType 		= CurToken.eType;
    BOOL		 bIsPostfix = eType == TFACT;

    SmStructureNode *pSNode;
    SmNode *pOper   = 0,
           *pExtra  = 0,
           *pArg;

    switch (eType)
    {
        case TABS :
        case TSQRT :
            NextToken();
            break;

        case TNROOT :
            NextToken();
            Power();
            pExtra = NodeStack.Pop();
            break;

        case TUOPER :
            NextToken();
            GlyphSpecial();
            pOper = NodeStack.Pop();
            break;

        case TPLUS :
        case TMINUS :
        case TPLUSMINUS :
        case TMINUSPLUS :
        case TNEG :
        case TFACT :
            OpSubSup();
            pOper = NodeStack.Pop();
            break;

        default :
            Error(PE_UNOPER_EXPECTED);
    }

    // get argument
    Power();
    pArg = NodeStack.Pop();

    if (eType == TABS)
    {	pSNode = new SmBraceNode(aNodeToken);
        pSNode->SetScaleMode(SCALE_HEIGHT);

        // build nodes for left & right lines
        // (text, group, level of the used token are of no interrest here)
        // we'll use row & column of the keyword for abs
        aNodeToken.eType = TABS;
        //
        aNodeToken.cMathChar = MS_LINE;
        SmNode* pLeft = new SmMathSymbolNode(aNodeToken);
        //
        aNodeToken.cMathChar = MS_LINE;
        SmNode* pRight = new SmMathSymbolNode(aNodeToken);

        pSNode->SetSubNodes(pLeft, pArg, pRight);
    }
    else if (eType == TSQRT  ||  eType == TNROOT)
    {	pSNode = new SmRootNode(aNodeToken);
        pOper = new SmRootSymbolNode(aNodeToken);
        pSNode->SetSubNodes(pExtra, pOper, pArg);
    }
    else
    {	pSNode = new SmUnHorNode(aNodeToken);

        if (bIsPostfix)
            pSNode->SetSubNodes(pArg, pOper);
        else
            // prefix operator
            pSNode->SetSubNodes(pOper, pArg);
    }

    NodeStack.Push(pSNode);
}


void SmParser::Attribut()
{
    DBG_ASSERT(TokenInGroup(TGATTRIBUT), "Sm: falsche Tokengruppe");

    SmStructureNode	*pSNode = new SmAttributNode(CurToken);
    SmNode		*pAttr;
    SmScaleMode  eScaleMode = SCALE_NONE;

    // get appropriate node for the attribut itself
    switch (CurToken.eType)
    {	case TUNDERLINE :
        case TOVERLINE :
        case TOVERSTRIKE :
            pAttr = new SmRectangleNode(CurToken);
            eScaleMode = SCALE_WIDTH;
            break;

        case TWIDEVEC :
        case TWIDEHAT :
        case TWIDETILDE :
            pAttr = new SmMathSymbolNode(CurToken);
            eScaleMode = SCALE_WIDTH;
            break;

        default :
            pAttr = new SmMathSymbolNode(CurToken);
    }

    NextToken();

    pSNode->SetSubNodes(pAttr, 0);
    pSNode->SetScaleMode(eScaleMode);
    NodeStack.Push(pSNode);
}


void SmParser::FontAttribut()
{
    DBG_ASSERT(TokenInGroup(TGFONTATTR), "Sm: falsche Tokengruppe");

    switch (CurToken.eType)
    {
        case TITALIC :
        case TNITALIC :
        case TBOLD :
        case TNBOLD :
        case TPHANTOM :
            NodeStack.Push(new SmFontNode(CurToken));
            NextToken();
            break;

        case TSIZE :
            FontSize();
            break;

        case TFONT :
            Font();
            break;

        case TCOLOR :
            Color();
            break;

        default :
            DBG_ASSERT(0, "Sm: unbekannter Fall");
    }
}


void SmParser::Color()
{
    DBG_ASSERT(CurToken.eType == TCOLOR, "Sm : Ooops...");

    // last color rules, get that one
    SmToken  aToken;
    do
    {	NextToken();

        if (TokenInGroup(TGCOLOR))
        {	aToken = CurToken;
            NextToken();
        }
        else
            Error(PE_COLOR_EXPECTED);
    } while (CurToken.eType == TCOLOR);

    NodeStack.Push(new SmFontNode(aToken));
}


void SmParser::Font()
{
    DBG_ASSERT(CurToken.eType == TFONT, "Sm : Ooops...");

    // last font rules, get that one
    SmToken  aToken;
    do
    {	NextToken();

        if (TokenInGroup(TGFONT))
        {	aToken = CurToken;
            NextToken();
        }
        else
            Error(PE_FONT_EXPECTED);
    } while (CurToken.eType == TFONT);

    NodeStack.Push(new SmFontNode(aToken));
}


// gets number used as arguments in Math formulas (e.g. 'size' command)
// Format: no negative numbers, must start with a digit, no exponent notation, ...
BOOL lcl_IsNumber(const UniString& rText)
{
    BOOL bPoint = FALSE;
    const sal_Unicode* pBuffer = rText.GetBuffer();
    for(xub_StrLen nPos = 0; nPos < rText.Len(); nPos++, pBuffer++)
    {
        const sal_Unicode cChar = *pBuffer;
        if(cChar == '.')
        {
            if(bPoint)
                return FALSE;
            else
                bPoint = TRUE;
        }
        else if ( !IsDigit( cChar ) )
            return FALSE;
    }
    return TRUE;
}

void SmParser::FontSize()
{
    DBG_ASSERT(CurToken.eType == TSIZE, "Sm : Ooops...");

    USHORT	   Type;
    SmFontNode *pFontNode = new SmFontNode(CurToken);

    NextToken();

    switch (CurToken.eType)
    {
        case TNUMBER:	Type = FNTSIZ_ABSOLUT;	break;
        case TPLUS:		Type = FNTSIZ_PLUS;		break;
        case TMINUS:	Type = FNTSIZ_MINUS;	break;
        case TMULTIPLY:	Type = FNTSIZ_MULTIPLY;	break;
        case TDIVIDEBY:	Type = FNTSIZ_DIVIDE;	break;

        default:
            delete pFontNode;
            Error(PE_SIZE_EXPECTED);
            return;
    }

    if (Type != FNTSIZ_ABSOLUT)
    {
        NextToken();
        if (CurToken.eType != TNUMBER)
        {
            delete pFontNode;
            Error(PE_SIZE_EXPECTED);
            return;
        }
    }

    // get number argument
    Fraction  aValue( 1L );
    if (lcl_IsNumber( CurToken.aText ))
    {
        double    fTmp;
        if ((fTmp = CurToken.aText.ToDouble()) != 0.0)
        {
            aValue = fTmp;

            //!! keep the numerator and denominator from being to large
            //!! otherwise ongoing multiplications may result in overflows
            //!! (for example in SmNode::SetFontSize the font size calculated
            //!! may become 0 because of this!!! Happens e.g. for ftmp = 2.9 with Linux
            //!! or ftmp = 1.11111111111111111... (11/9) on every platform.)
            if (aValue.GetDenominator() > 1000)
            {
                long nNum   = aValue.GetNumerator();
                long nDenom = aValue.GetDenominator();
                while (nDenom > 1000)
                {
                    nNum    /= 10;
                    nDenom  /= 10;
                }
                aValue = Fraction( nNum, nDenom );
            }
        }
    }

    NextToken();

    pFontNode->SetSizeParameter(aValue, Type);
    NodeStack.Push(pFontNode);
}


void SmParser::Brace()
{
    DBG_ASSERT(CurToken.eType == TLEFT  ||  TokenInGroup(TGLBRACES),
        "Sm: kein Klammer Ausdruck");

    SmStructureNode *pSNode  = new SmBraceNode(CurToken);
    SmNode *pBody   = 0,
           *pLeft   = 0,
           *pRight  = 0;
    SmScaleMode   eScaleMode = SCALE_NONE;
    SmParseError  eError     = PE_NONE;

    if (CurToken.eType == TLEFT)
    {	NextToken();

        eScaleMode = SCALE_HEIGHT;

        // check for left bracket
        if (TokenInGroup(TGLBRACES) || TokenInGroup(TGRBRACES))
        {
            pLeft = new SmMathSymbolNode(CurToken);

            NextToken();
            Bracebody(TRUE);
            pBody = NodeStack.Pop();

            if (CurToken.eType == TRIGHT)
            {	NextToken();

                // check for right bracket
                if (TokenInGroup(TGLBRACES) || TokenInGroup(TGRBRACES))
                {
                    pRight = new SmMathSymbolNode(CurToken);
                    NextToken();
                }
                else
                    eError = PE_RBRACE_EXPECTED;
            }
            else
                eError = PE_RIGHT_EXPECTED;
        }
        else
            eError = PE_LBRACE_EXPECTED;
    }
    else
    {
        if (TokenInGroup(TGLBRACES))
        {
            pLeft = new SmMathSymbolNode(CurToken);

            NextToken();
            Bracebody(FALSE);
            pBody = NodeStack.Pop();

            SmTokenType  eExpectedType = TUNKNOWN;
            switch (pLeft->GetToken().eType)
            {	case TLPARENT :	 	eExpectedType = TRPARENT;	break;
                case TLBRACKET : 	eExpectedType = TRBRACKET;	break;
                case TLBRACE : 	 	eExpectedType = TRBRACE;	break;
                case TLDBRACKET :	eExpectedType = TRDBRACKET;	break;
                case TLLINE : 	 	eExpectedType = TRLINE;		break;
                case TLDLINE :   	eExpectedType = TRDLINE;	break;
                case TLANGLE :   	eExpectedType = TRANGLE;	break;
                case TLFLOOR :   	eExpectedType = TRFLOOR;	break;
                case TLCEIL : 	 	eExpectedType = TRCEIL;		break;
                default :
                    DBG_ASSERT(0, "Sm: unbekannter Fall");
            }

            if (CurToken.eType == eExpectedType)
            {
                pRight = new SmMathSymbolNode(CurToken);
                NextToken();
            }
            else
                eError = PE_PARENT_MISMATCH;
        }
        else
            eError = PE_LBRACE_EXPECTED;
    }

    if (eError == PE_NONE)
    {   DBG_ASSERT(pLeft,  "Sm: NULL pointer");
        DBG_ASSERT(pRight, "Sm: NULL pointer");
        pSNode->SetSubNodes(pLeft, pBody, pRight);
        pSNode->SetScaleMode(eScaleMode);
        NodeStack.Push(pSNode);
    }
    else
    {	delete pSNode;
        delete pBody;
        delete pLeft;
        delete pRight;

        Error(eError);
    }
}


void SmParser::Bracebody(BOOL bIsLeftRight)
{
    SmStructureNode *pBody = new SmBracebodyNode(CurToken);
    SmNodeArray  	 aNodes;
    USHORT		 	 nNum = 0;

    // get body if any
    if (bIsLeftRight)
    {
        do
        {
            if (CurToken.eType == TMLINE)
            {
                NodeStack.Push(new SmMathSymbolNode(CurToken));
                NextToken();
                nNum++;
            }
            else if (CurToken.eType != TRIGHT)
            {	Align();
                nNum++;

                if (CurToken.eType != TMLINE  &&  CurToken.eType != TRIGHT)
                    Error(PE_RIGHT_EXPECTED);
            }
        } while (CurToken.eType != TEND  &&  CurToken.eType != TRIGHT);
    }
    else
    {
        do
        {
            if (CurToken.eType == TMLINE)
            {
                NodeStack.Push(new SmMathSymbolNode(CurToken));
                NextToken();
                nNum++;
            }
            else if (!TokenInGroup(TGRBRACES))
            {	Align();
                nNum++;

                if (CurToken.eType != TMLINE  &&  !TokenInGroup(TGRBRACES))
                    Error(PE_RBRACE_EXPECTED);
            }
        } while (CurToken.eType != TEND  &&  !TokenInGroup(TGRBRACES));
    }

    // build argument vector in parsing order
    aNodes.SetSize(nNum);
    for (USHORT i = 0;  i < nNum;  i++)
        aNodes.Put(nNum - 1 - i, NodeStack.Pop());

    pBody->SetSubNodes(aNodes);
    pBody->SetScaleMode(bIsLeftRight ? SCALE_HEIGHT : SCALE_NONE);
    NodeStack.Push(pBody);
}


void SmParser::Function()
{
    switch (CurToken.eType)
    {
        case TFUNC:
            NextToken();	// skip "FUNC"-statement
            // fall through

        case TSIN :
        case TCOS :
        case TTAN :
        case TCOT :
        case TASIN :
        case TACOS :
        case TATAN :
        case TACOT :
        case TSINH :
        case TCOSH :
        case TTANH :
        case TCOTH :
        case TASINH :
        case TACOSH :
        case TATANH :
        case TACOTH :
        case TLN :
        case TLOG :
        case TEXP :
            NodeStack.Push(new SmTextNode(CurToken, FNT_FUNCTION));
            NextToken();
            break;

        default:
            Error(PE_FUNC_EXPECTED);
    }
}


void SmParser::Binom()
{
    SmNodeArray  ExpressionArray;
    SmStructureNode	*pSNode = new SmTableNode(CurToken);

    NextToken();

    Sum();
    Sum();

    ExpressionArray.SetSize(2);

    for (int i = 0;  i < 2;  i++)
        ExpressionArray.Put(2 - (i + 1), NodeStack.Pop());

    pSNode->SetSubNodes(ExpressionArray);
    NodeStack.Push(pSNode);
}


void SmParser::Stack()
{
    SmNodeArray  ExpressionArray;
    NextToken();
    if (CurToken.eType == TLGROUP)
    {
        USHORT n = 0;

        do
        {
            NextToken();
            Align();
            n++;
        }
        while (CurToken.eType == TPOUND);

        ExpressionArray.SetSize(n);

        for (USHORT i = 0; i < n; i++)
            ExpressionArray.Put(n - (i + 1), NodeStack.Pop());

        if (CurToken.eType != TRGROUP)
            Error(PE_RGROUP_EXPECTED);

        NextToken();

        SmStructureNode *pSNode = new SmTableNode(CurToken);
        pSNode->SetSubNodes(ExpressionArray);
        NodeStack.Push(pSNode);
    }
    else
        Error(PE_LGROUP_EXPECTED);
}


void SmParser::Matrix()
{
    SmNodeArray  ExpressionArray;

    NextToken();
    if (CurToken.eType == TLGROUP)
    {
        USHORT c = 0;

        do
        {
            NextToken();
            Align();
            c++;
        }
        while (CurToken.eType == TPOUND);

        USHORT r = 1;

        while (CurToken.eType == TDPOUND)
        {
            NextToken();
            for (USHORT i = 0; i < c; i++)
            {
                Align();
                if (i < (c - 1))
                {
                    if (CurToken.eType == TPOUND)
                    {
                        NextToken();
                    }
                    else
                        Error(PE_POUND_EXPECTED);
                }
            }

            r++;
        }

        long nRC = r * c;

        ExpressionArray.SetSize(nRC);

        for (USHORT i = 0; i < (nRC); i++)
            ExpressionArray.Put((nRC) - (i + 1), NodeStack.Pop());

        if (CurToken.eType != TRGROUP)
            Error(PE_RGROUP_EXPECTED);

        NextToken();

        SmMatrixNode *pMNode = new SmMatrixNode(CurToken);
        pMNode->SetSubNodes(ExpressionArray);
        pMNode->SetRowCol(r, c);
        NodeStack.Push(pMNode);
    }
    else
        Error(PE_LGROUP_EXPECTED);
}


void SmParser::Special()
{
    BOOL bReplace = FALSE;
    String &rName = CurToken.aText;
    String aNewName;

    if (CONVERT_NONE == GetConversion())
    {
        // conversion of symbol names for 6.0 (XML) file format
        // (name change on import / export.
        // UI uses localized names XML file format does not.)
        if (IsImportSymbolNames())
        {
            const SmLocalizedSymbolData &rLSD = SM_MOD1()->GetLocSymbolData();
            aNewName = rLSD.GetUiSymbolName( rName );
            bReplace = TRUE;
        }
        else if (IsExportSymbolNames())
        {
            const SmLocalizedSymbolData &rLSD = SM_MOD1()->GetLocSymbolData();
            aNewName = rLSD.GetExportSymbolName( rName );
            bReplace = TRUE;
        }
    }
    else    // 5.0 <-> 6.0 formula text (symbol name) conversion
    {
        LanguageType nLanguage = GetLanguage();
        SmLocalizedSymbolData &rData = SM_MOD1()->GetLocSymbolData();
        const ResStringArray *pFrom = 0;
        const ResStringArray *pTo   = 0;
        if (CONVERT_50_TO_60 == GetConversion())
        {
            pFrom = rData.Get50NamesArray( nLanguage );
            pTo   = rData.Get60NamesArray( nLanguage );
        }
        else if (CONVERT_60_TO_50 == GetConversion())
        {
            pFrom = rData.Get60NamesArray( nLanguage );
            pTo   = rData.Get50NamesArray( nLanguage );
        }
        if (pFrom  &&  pTo)
        {
            DBG_ASSERT( pFrom->Count() == pTo->Count(),
                    "array length mismatch" );
            USHORT nCount = sal::static_int_cast< USHORT >(pFrom->Count());
            for (USHORT i = 0;  i < nCount;  ++i)
            {
                if (pFrom->GetString(i) == rName)
                {
                    aNewName = pTo->GetString(i);
                    bReplace = TRUE;
                }
            }
        }
        // else:
        // conversion arrays not found or (usually)
        // conversion not necessary
    }

    if (bReplace  &&  aNewName.Len()  &&  rName != aNewName)
    {
        Replace( GetTokenIndex() + 1, rName.Len(), aNewName );
        rName = aNewName;
    }

    NodeStack.Push(new SmSpecialNode(CurToken));
    NextToken();
}


void SmParser::GlyphSpecial()
{
    NodeStack.Push(new SmGlyphSpecialNode(CurToken));
    NextToken();
}


void SmParser::Error(SmParseError eError)
{
    SmStructureNode *pSNode = new SmExpressionNode(CurToken);
    SmErrorNode		*pErr   = new SmErrorNode(eError, CurToken);
    pSNode->SetSubNodes(pErr, 0);

    //! put a structure node on the stack (instead of the error node itself)
    //! because sometimes such a node is expected in order to attach some
    //! subnodes
    NodeStack.Push(pSNode);

    AddError(eError, pSNode);

    NextToken();
}


// end gramar


SmParser::SmParser()
{
    eConversion = CONVERT_NONE;
    bImportSymNames = bExportSymNames = FALSE;
    nLang = Application::GetSettings().GetUILanguage();
}


SmNode *SmParser::Parse(const String &rBuffer)
{
    BufferString = rBuffer;
    BufferString.ConvertLineEnd( LINEEND_LF );
    BufferIndex  =
    nTokenIndex  = 0;
    Row 		 = 1;
    ColOff		 = 0;
    CurError	 = -1;

    for (USHORT i = 0;  i < ErrDescList.Count();  i++)
        delete ErrDescList.Remove(i);

    ErrDescList.Clear();

    NodeStack.Clear();

    SetLanguage( Application::GetSettings().GetUILanguage() );
    NextToken();
    Table();

    return NodeStack.Pop();
}


USHORT SmParser::AddError(SmParseError Type, SmNode *pNode)
{
    SmErrorDesc *pErrDesc = new SmErrorDesc;

    pErrDesc->Type  = Type;
    pErrDesc->pNode = pNode;
    pErrDesc->Text  = String(SmResId(RID_ERR_IDENT));

    USHORT  nRID;
    switch (Type)
    {
        case PE_UNEXPECTED_CHAR:	 nRID = RID_ERR_UNEXPECTEDCHARACTER;	break;
        case PE_LGROUP_EXPECTED:	 nRID = RID_ERR_LGROUPEXPECTED;			break;
        case PE_RGROUP_EXPECTED:	 nRID = RID_ERR_RGROUPEXPECTED;			break;
        case PE_LBRACE_EXPECTED:	 nRID = RID_ERR_LBRACEEXPECTED;			break;
        case PE_RBRACE_EXPECTED:	 nRID = RID_ERR_RBRACEEXPECTED;			break;
        case PE_FUNC_EXPECTED:		 nRID = RID_ERR_FUNCEXPECTED;			break;
        case PE_UNOPER_EXPECTED:	 nRID = RID_ERR_UNOPEREXPECTED;			break;
        case PE_BINOPER_EXPECTED:	 nRID = RID_ERR_BINOPEREXPECTED;		break;
        case PE_SYMBOL_EXPECTED:	 nRID = RID_ERR_SYMBOLEXPECTED;			break;
        case PE_IDENTIFIER_EXPECTED: nRID = RID_ERR_IDENTEXPECTED;			break;
        case PE_POUND_EXPECTED:		 nRID = RID_ERR_POUNDEXPECTED;			break;
        case PE_COLOR_EXPECTED:		 nRID = RID_ERR_COLOREXPECTED;			break;
        case PE_RIGHT_EXPECTED:		 nRID = RID_ERR_RIGHTEXPECTED;			break;

        default:
            nRID = RID_ERR_UNKOWN;
    }
    pErrDesc->Text += SmResId(nRID);

    ErrDescList.Insert(pErrDesc);

    return (USHORT) ErrDescList.GetPos(pErrDesc);
}


const SmErrorDesc  *SmParser::NextError()
{
    if (ErrDescList.Count())
        if (CurError > 0) return ErrDescList.Seek(--CurError);
        else
        {
            CurError = 0;
            return ErrDescList.Seek(CurError);
        }
    else return 0;
}


const SmErrorDesc  *SmParser::PrevError()
{
    if (ErrDescList.Count())
        if (CurError < (int) (ErrDescList.Count() - 1)) return ErrDescList.Seek(++CurError);
        else
        {
            CurError = (int) (ErrDescList.Count() - 1);
            return ErrDescList.Seek(CurError);
        }
    else return 0;
}


const SmErrorDesc  *SmParser::GetError(USHORT i)
{
    return (/*i >= 0  &&*/  i < ErrDescList.Count())
               ? ErrDescList.Seek(i)
               : ErrDescList.Seek(CurError);
}


