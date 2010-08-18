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
#ifndef PARSE_HXX
#define PARSE_HXX


#include <vcl/svapp.hxx>
#include <tools/stack.hxx>
#include <tools/list.hxx>
#include <tools/string.hxx>

#include "types.hxx"

class SmNode;
class SmDocShell;


// TokenGroups
#define TGOPER			0x00000001
#define TGRELATION		0x00000002
#define TGSUM			0x00000004
#define TGPRODUCT		0x00000008
#define TGUNOPER		0x00000010
#define TGPOWER 		0x00000020
#define TGATTRIBUT		0x00000040
#define TGALIGN 		0x00000080
#define TGFUNCTION		0x00000100
#define TGBLANK 		0x00000200
#define TGLBRACES		0x00000400
#define TGRBRACES		0x00000800
#define TGCOLOR 		0x00001000
#define TGFONT			0x00002000
#define TGSTANDALONE	0x00004000
#define TGDISCARDED		0x00008000
#define TGLIMIT			0x00010000
#define TGFONTATTR		0x00020000


enum SmTokenType
{
    TEND,			TLGROUP,		TRGROUP,		TLPARENT,		TRPARENT,
    TLBRACKET,		TRBRACKET,		TPLUS,			TMINUS,			TMULTIPLY,
    TDIVIDEBY,		TASSIGN,		TPOUND,			TSPECIAL,		TSLASH,
    TBACKSLASH,		TBLANK,			TSBLANK,		TRSUB,			TRSUP,
    TCSUB,			TCSUP,			TLSUB,			TLSUP,			TGT,
    TLT,			TAND,			TOR,			TINTERSECT,		TUNION,
    TNEWLINE,		TBINOM,			TFROM,			TTO,			TINT,
    TSUM,			TOPER,			TABS,			TSQRT,			TFACT,
    TNROOT,			TOVER,			TTIMES,			TGE,			TLE,
    TGG,			TLL,			TDOTSAXIS,		TDOTSLOW,		TDOTSVERT,
    TDOTSDIAG,		TDOTSUP,		TDOTSDOWN,		TACUTE,			TBAR,
    TBREVE,			TCHECK,			TCIRCLE,		TDOT,			TDDOT,
    TDDDOT,			TGRAVE,			THAT,			TTILDE,			TVEC,
    TUNDERLINE,		TOVERLINE,		TOVERSTRIKE,	TITALIC,		TNITALIC,
    TBOLD,			TNBOLD,			TPHANTOM,		TFONT,			TSIZE,
    TCOLOR,			TALIGNL,		TALIGNC,		TALIGNR,		TLEFT,
    TRIGHT,			TLANGLE,		TLBRACE,		TLLINE,			TLDLINE,
    TLCEIL,			TLFLOOR,		TNONE,			TMLINE,			TRANGLE,
    TRBRACE,		TRLINE,			TRDLINE,		TRCEIL,			TRFLOOR,
    TSIN,			TCOS,			TTAN,			TCOT,			TFUNC,
    TSTACK,			TMATRIX,		TMATFORM,		TDPOUND,		TPLACE,
    TTEXT,			TNUMBER,		TCHARACTER,		TIDENT,			TNEQ,
    TEQUIV,			TDEF,			TPROP,			TSIM,			TSIMEQ,
    TAPPROX,		TPARALLEL,		TORTHO,			TIN,			TNOTIN,
    TSUBSET,		TSUBSETEQ,		TSUPSET,		TSUPSETEQ,		TPLUSMINUS,
    TMINUSPLUS,		TOPLUS,			TOMINUS,		TDIV,			TOTIMES,
    TODIVIDE,		TTRANSL,		TTRANSR,		TIINT,			TIIINT,
    TLINT,			TLLINT,			TLLLINT,		TPROD,			TCOPROD,
    TFORALL,		TEXISTS,		TLIM,			TNABLA,			TTOWARD,
    TSINH,			TCOSH,			TTANH,			TCOTH,			TASIN,
    TACOS,			TATAN,			TLN,			TLOG,			TUOPER,
    TBOPER,			TBLACK,			TWHITE,			TRED,			TGREEN,
    TBLUE,			TCYAN,			TMAGENTA,		TYELLOW,		TFIXED,
    TSANS,			TSERIF,			TPOINT,			TASINH,			TACOSH,
    TATANH,			TACOTH,			TACOT,			TEXP,			TCDOT,
    TODOT,			TLESLANT,		TGESLANT,		TNSUBSET,		TNSUPSET,
    TNSUBSETEQ,		TNSUPSETEQ,		TPARTIAL,		TNEG,			TNI,
    TBACKEPSILON,	TALEPH,			TIM,			TRE,			TWP,
    TEMPTYSET,		TINFINITY,		TESCAPE,		TLIMSUP,		TLIMINF,
    TNDIVIDES,		TDRARROW,		TDLARROW,		TDLRARROW,		TUNDERBRACE,
    TOVERBRACE,		TCIRC,			TTOP,			THBAR,			TLAMBDABAR,
    TLEFTARROW,		TRIGHTARROW,	TUPARROW,		TDOWNARROW,		TDIVIDES,
    TNDIBVIDES,		TSETN,			TSETZ,			TSETQ,			TSETR,
    TSETC,			TWIDEVEC,		TWIDETILDE,		TWIDEHAT,		TWIDESLASH,
    TWIDEBACKSLASH, TLDBRACKET,		TRDBRACKET,     TNOSPACE,
    TUNKNOWN,		TDEBUG
};


struct SmToken
{
    // token text
    String	aText;
    // token info
    SmTokenType eType;
    sal_Unicode		cMathChar;
    // parse-help info
    ULONG		nGroup;
    USHORT		nLevel;
    // token position
    USHORT      nRow;
    xub_StrLen  nCol;

    SmToken();
};


enum SmParseError
{
    PE_NONE,					PE_UNEXPECTED_END_OF_INPUT,
    PE_UNEXPECTED_CHAR,			PE_UNEXPECTED_TOKEN,
    PE_FUNC_EXPECTED,			PE_UNOPER_EXPECTED,
    PE_BINOPER_EXPECTED,		PE_SYMBOL_EXPECTED,
    PE_IDENTIFIER_EXPECTED,		PE_POUND_EXPECTED,
    PE_COLOR_EXPECTED,			PE_LGROUP_EXPECTED,
    PE_RGROUP_EXPECTED,			PE_LBRACE_EXPECTED,
    PE_RBRACE_EXPECTED,			PE_PARENT_MISMATCH,
    PE_RIGHT_EXPECTED,			PE_FONT_EXPECTED,
    PE_SIZE_EXPECTED,			PE_DOUBLE_ALIGN,
    PE_DOUBLE_SUBSUPSCRIPT
};


struct SmErrorDesc
{
    SmParseError  Type;
    SmNode		 *pNode;
    String		  Text;
};

DECLARE_STACK(SmNodeStack,  SmNode *)
DECLARE_LIST(SmErrDescList, SmErrorDesc *)

/**************************************************************************/

// defines possible conversions of the formula text from the format of
// one release to the one of another.
enum SmConvert
{
    CONVERT_NONE,
    CONVERT_40_TO_50,
    CONVERT_50_TO_60,
    CONVERT_60_TO_50
};


class SmParser
{
    String          BufferString;
    SmToken         CurToken;
    SmNodeStack     NodeStack;
    SmErrDescList   ErrDescList;
    int             CurError;
    LanguageType    nLang;
    xub_StrLen      BufferIndex,
                    nTokenIndex;
    USHORT          Row,
                    ColOff;
    SmConvert       eConversion;
    BOOL            bImportSymNames,
                    bExportSymNames;

    // declare copy-constructor and assignment-operator private
    SmParser(const SmParser &);
    SmParser & operator = (const SmParser &);

protected:
#if OSL_DEBUG_LEVEL
    BOOL			IsDelimiter( const String &rTxt, xub_StrLen nPos );
#endif
    void			NextToken();
    xub_StrLen		GetTokenIndex() const	{ return nTokenIndex; }
    void 			Insert(const String &rText, USHORT nPos);
    void            Replace( USHORT nPos, USHORT nLen, const String &rText );

    inline BOOL		TokenInGroup(ULONG nGroup);

    // grammar
    void	Table();
    void	Line();
    void	Expression();
    void	Relation();
    void	Sum();
    void	Product();
    void	SubSup(ULONG nActiveGroup);
    void	OpSubSup();
    void 	Power();
    void	Blank();
    void	Term();
    void	Escape();
    void	Operator();
    void	Oper();
    void	UnOper();
    void	Align();
    void	FontAttribut();
    void	Attribut();
    void	Font();
    void	FontSize();
    void	Color();
    void	Brace();
    void	Bracebody(BOOL bIsLeftRight);
    void	Function();
    void	Binom();
    void	Stack();
    void	Matrix();
    void	Special();
    void	GlyphSpecial();
    // end of grammar
    
    LanguageType    GetLanguage() const { return nLang; }
    void            SetLanguage( LanguageType nNewLang ) { nLang = nNewLang; }

    void	Error(SmParseError Error);

public:
                 SmParser();

    SmNode		*Parse(const String &rBuffer);

    const String & GetText() const { return BufferString; };

    SmConvert    GetConversion() const              { return eConversion; }
    void         SetConversion(SmConvert eConv)     { eConversion = eConv; }

    BOOL         IsImportSymbolNames() const        { return bImportSymNames; }
    void         SetImportSymbolNames(BOOL bVal)    { bImportSymNames = bVal; }
    BOOL         IsExportSymbolNames() const        { return bExportSymNames; }
    void         SetExportSymbolNames(BOOL bVal)    { bExportSymNames = bVal; }

    USHORT		 AddError(SmParseError Type, SmNode *pNode);

    const SmErrorDesc *	NextError();
    const SmErrorDesc *	PrevError();
    const SmErrorDesc *	GetError(USHORT i = 0xFFFF);
};


inline BOOL SmParser::TokenInGroup(ULONG nGroup)
{
    return (CurToken.nGroup & nGroup) ? TRUE : FALSE;
}


#endif

