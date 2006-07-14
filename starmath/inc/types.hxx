/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: types.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-14 08:22:01 $
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
#ifndef TYPES_HXX
#define TYPES_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#define FONTNAME_MATH   "StarSymbol"
#define FONTNAME_MATH2  "OpenSymbol"

/////////////////////////////////////////////////////////////////

enum SmPrintSize { PRINT_SIZE_NORMAL, PRINT_SIZE_SCALED, PRINT_SIZE_ZOOMED };

/////////////////////////////////////////////////////////////////

#define FONT_ARIAL          "Arial"
#define FONT_SYMBOL         "Symbol"
#define FONT_MS_PMINCHO     "MS PMincho"
#define FONT_MS_PGOTHIC     "MS PGothic"
#define FONT_SUN_BATANG     "SunBatang"


inline sal_Bool IsInPrivateUseArea( sal_Unicode cChar ) { return 0xE000 <= cChar  &&  cChar <= 0xF8FF; }

// Greek char range as seen in svx/.../charmap.cxx RID_SUBSETSTR_BASIC_GREEK
inline sal_Bool IsGreekChar( sal_Unicode cChar ) { return 0x0370 <= cChar  &&  cChar <= 0x03FF; }

sal_Unicode ConvertMathPrivateUseAreaToUnicode( sal_Unicode cChar );
sal_Unicode ConvertUnicodeToMathPrivateUseArea( sal_Unicode cChar );

sal_Unicode ConvertMathToMathType( sal_Unicode cChar );
sal_Unicode ConvertMathTypeToMath( sal_Unicode cChar );

sal_Unicode ConvertMathToMathML( sal_Unicode cChar );
sal_Unicode ConvertMathMLToMath( sal_Unicode cChar );

sal_Unicode GetTokenChar( sal_Unicode cChar, sal_Bool bConvertForExport );

/////////////////////////////////////////////////////////////////
// enum definitions for characters from the 'StarSymbol' font
// (some chars have more than one alias!)
//! Note: not listed here does not(!) mean "not used"
//!		(see %alpha ... %gamma for example)
//
enum MathSymbol
{
    MS_FACT         = (sal_Unicode) 0x0021,
    MS_INFINITY     = (sal_Unicode) 0x221E,
    MS_SLASH        = (sal_Unicode) 0x002F,

    MS_NDIVIDES     = (sal_Unicode) 0x2224,
    MS_DRARROW      = (sal_Unicode) 0x21D2,
    MS_DLARROW      = (sal_Unicode) 0x21D0,
    MS_DLRARROW     = (sal_Unicode) 0x21D4,
    MS_UNDERBRACE   = (sal_Unicode) 0xE081,
    MS_OVERBRACE    = (sal_Unicode) 0xE082,
    MS_CIRC         = (sal_Unicode) 0x00B0,
    MS_ASSIGN       = (sal_Unicode) 0x003D,
    MS_ERROR        = (sal_Unicode) 0x00BF,

    MS_NEQ          = (sal_Unicode) 0x2260,
    MS_PLUS         = (sal_Unicode) 0xE083,
    MS_MINUS        = (sal_Unicode) 0x2212,
    MS_MULTIPLY     = (sal_Unicode) 0x2217,
    MS_TIMES        = (sal_Unicode) 0x00D7,
    MS_CDOT         = (sal_Unicode) 0x22C5,
    MS_DIV          = (sal_Unicode) 0x00F7,
    MS_PLUSMINUS    = (sal_Unicode) 0x00B1,
    MS_MINUSPLUS    = (sal_Unicode) 0x2213,
    MS_OPLUS        = (sal_Unicode) 0x2295,
    MS_OMINUS       = (sal_Unicode) 0x2296,
    MS_OTIMES       = (sal_Unicode) 0x2297,
    MS_ODIVIDE      = (sal_Unicode) 0x2298,
    MS_ODOT         = (sal_Unicode) 0x2299,
    MS_UNION        = (sal_Unicode) 0x222A,
    MS_INTERSECT    = (sal_Unicode) 0x2229,

    MS_LT           = (sal_Unicode) 0xE084,
    MS_GT           = (sal_Unicode) 0xE085,
    MS_LE           = (sal_Unicode) 0x2264,
    MS_GE           = (sal_Unicode) 0x2265,
    MS_LESLANT      = (sal_Unicode) 0xE086,
    MS_GESLANT      = (sal_Unicode) 0xE087,
    MS_LL           = (sal_Unicode) 0x226A,
    MS_GG           = (sal_Unicode) 0x226B,
    MS_SIM          = (sal_Unicode) 0x007E,
    MS_SIMEQ        = (sal_Unicode) 0x2243,
    MS_APPROX       = (sal_Unicode) 0x2248,
    MS_DEF          = (sal_Unicode) 0x225D,
    MS_EQUIV        = (sal_Unicode) 0x2261,
    MS_PROP         = (sal_Unicode) 0x221D,
    MS_PARTIAL      = (sal_Unicode) 0x2202,
    MS_SUBSET       = (sal_Unicode) 0x2282,

    MS_SUPSET       = (sal_Unicode) 0x2283,
    MS_SUBSETEQ     = (sal_Unicode) 0x2286,
    MS_SUPSETEQ     = (sal_Unicode) 0x2287,
    MS_NSUBSET      = (sal_Unicode) 0x2284,
    MS_NSUPSET      = (sal_Unicode) 0x2285,
    MS_NSUBSETEQ    = (sal_Unicode) 0x2288,
    MS_NSUPSETEQ    = (sal_Unicode) 0x2289,
    MS_IN           = (sal_Unicode) 0x2208,
    MS_NOTIN        = (sal_Unicode) 0x2209,
    MS_EXISTS       = (sal_Unicode) 0x2203,
    MS_BACKEPSILON  = (sal_Unicode) 0x220D,
    MS_ALEPH        = (sal_Unicode) 0x2135,
    MS_IM           = (sal_Unicode) 0x2111,
    MS_RE           = (sal_Unicode) 0x211C,
    MS_WP           = (sal_Unicode) 0x2118,

    MS_LINE         = (sal_Unicode) 0x2223,
    MS_DLINE        = (sal_Unicode) 0x2225,
    MS_ORTHO        = (sal_Unicode) 0x22A5,
    MS_DOTSLOW      = (sal_Unicode) 0xE08B,
    MS_DOTSAXIS     = (sal_Unicode) 0x22EF,
    MS_DOTSVERT     = (sal_Unicode) 0x22EE,
    MS_DOTSUP       = (sal_Unicode) 0x22F0,
    MS_DOTSDOWN     = (sal_Unicode) 0x22F1,
    MS_TRANSR       = (sal_Unicode) 0x22B6,
    MS_TRANSL       = (sal_Unicode) 0x22B7,
    MS_RIGHTARROW   = (sal_Unicode) 0xE08C,
    MS_BACKSLASH    = (sal_Unicode) 0x2216,
    MS_NEG          = (sal_Unicode) 0x00AC,

    MS_INT          = (sal_Unicode) 0x222B,
    MS_IINT         = (sal_Unicode) 0x222C,
    MS_IIINT        = (sal_Unicode) 0x222D,
    MS_LINT         = (sal_Unicode) 0x222E,
    MS_LLINT        = (sal_Unicode) 0x222F,
    MS_LLLINT       = (sal_Unicode) 0x2230,
    MS_SQRT         = (sal_Unicode) 0xE08D,
    MS_COPROD       = (sal_Unicode) 0x2210,
    MS_PROD         = (sal_Unicode) 0x220F,
    MS_SUM          = (sal_Unicode) 0x2211,
    MS_NABLA        = (sal_Unicode) 0x2207,
    MS_FORALL       = (sal_Unicode) 0x2200,

    MS_HAT          = (sal_Unicode) 0xE091,
    MS_CHECK        = (sal_Unicode) 0xE092,
    MS_BREVE        = (sal_Unicode) 0xE093,
    MS_ACUTE        = (sal_Unicode) 0xE094,
    MS_GRAVE        = (sal_Unicode) 0xE095,
    MS_TILDE        = (sal_Unicode) 0xE096,
    MS_BAR          = (sal_Unicode) 0xE097,
    MS_VEC          = (sal_Unicode) 0xE098,
    MS_DOT          = (sal_Unicode) 0x02D9,
    MS_DDOT         = (sal_Unicode) 0x00A8,
    MS_DDDOT        = (sal_Unicode) 0xE09B,
    MS_CIRCLE       = (sal_Unicode) 0x02DA,
    MS_AND          = (sal_Unicode) 0x2227,
    MS_OR           = (sal_Unicode) 0x2228,
    MS_NI           = (sal_Unicode) 0x220B,
    MS_EMPTYSET     = (sal_Unicode) 0x2205,

    MS_LBRACE       = (sal_Unicode) 0x007B,
    MS_RBRACE       = (sal_Unicode) 0x007D,
    MS_LPARENT      = (sal_Unicode) 0xE09E,
    MS_RPARENT      = (sal_Unicode) 0xE09F,
    MS_LANGLE       = (sal_Unicode) 0x2329,
    MS_RANGLE       = (sal_Unicode) 0x232A,
    MS_LBRACKET     = (sal_Unicode) 0x005B,
    MS_RBRACKET     = (sal_Unicode) 0x005D,

    MS_LDBRACKET    = (sal_Unicode) 0x301A,
    MS_RDBRACKET    = (sal_Unicode) 0x301B,
    MS_PLACE        = (sal_Unicode) 0xE0AA,

    MS_LCEIL        = (sal_Unicode) 0x2308,
    MS_LFLOOR       = (sal_Unicode) 0x230A,
    MS_RCEIL        = (sal_Unicode) 0x2309,
    MS_RFLOOR       = (sal_Unicode) 0x230B,

    MS_HBAR         = (sal_Unicode) 0x210F,
    MS_LAMBDABAR    = (sal_Unicode) 0x019B,
    MS_LEFTARROW    = (sal_Unicode) 0xE0DB,
    MS_UPARROW      = (sal_Unicode) 0xE0DC,
    MS_DOWNARROW    = (sal_Unicode) 0xE0DD,
    MS_SETN         = (sal_Unicode) 0x2115,
    MS_SETZ         = (sal_Unicode) 0x2124,
    MS_SETQ         = (sal_Unicode) 0x211A,
    MS_SETR         = (sal_Unicode) 0x211D,
    MS_SETC         = (sal_Unicode) 0x2102
};


#endif

