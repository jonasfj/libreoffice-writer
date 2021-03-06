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


#include <cctype>
#if defined(MACOSX)
#include <stdlib.h>
#endif
#include <cstdlib>
#include <climits>
// #include <cmath>
#include <cfloat>
#include <hintids.hxx>
#include <rtl/math.hxx>
#include <editeng/langitem.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <unotools/localedatawrapper.hxx>
#include <unotools/charclass.hxx>
#include <editeng/unolingu.hxx>
#include <editeng/scripttypeitem.hxx>
#include <unotools/useroptions.hxx>
#include <tools/datetime.hxx>
#include <svl/zforlist.hxx>
#include <swmodule.hxx>
#include <doc.hxx>
#include <viewsh.hxx>
#include <docstat.hxx>
#include <calc.hxx>
#include <shellres.hxx>
#include <dbfld.hxx>
#include <expfld.hxx>
#include <usrfld.hxx>
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#include <docfld.hxx>
#include <swunodef.hxx>
#include <swtypes.hxx>

using namespace ::com::sun::star;

// tippt sich schneller
#define RESOURCE ViewShell::GetShellRes()

const sal_Char __FAR_DATA sCalc_Add[] 	= 	"add";
const sal_Char __FAR_DATA sCalc_Sub[]	=	"sub";
const sal_Char __FAR_DATA sCalc_Mul[]	=	"mul";
const sal_Char __FAR_DATA sCalc_Div[]	=	"div";
const sal_Char __FAR_DATA sCalc_Phd[]	=	"phd";
const sal_Char __FAR_DATA sCalc_Sqrt[]	=	"sqrt";
const sal_Char __FAR_DATA sCalc_Pow[]	=	"pow";
const sal_Char __FAR_DATA sCalc_Or[]	=	"or";
const sal_Char __FAR_DATA sCalc_Xor[]	=	"xor";
const sal_Char __FAR_DATA sCalc_And[]	=	"and";
const sal_Char __FAR_DATA sCalc_Not[]	=	"not";
const sal_Char __FAR_DATA sCalc_Eq[]	=	"eq";
const sal_Char __FAR_DATA sCalc_Neq[]	=	"neq";
const sal_Char __FAR_DATA sCalc_Leq[]	=	"leq";
const sal_Char __FAR_DATA sCalc_Geq[]	=	"geq";
const sal_Char __FAR_DATA sCalc_L[]		=	"l";
const sal_Char __FAR_DATA sCalc_G[]		=	"g";
const sal_Char __FAR_DATA sCalc_Sum[]	=	"sum";
const sal_Char __FAR_DATA sCalc_Mean[]	=	"mean";
const sal_Char __FAR_DATA sCalc_Min[]	=	"min";
const sal_Char __FAR_DATA sCalc_Max[]	=	"max";
const sal_Char __FAR_DATA sCalc_Sin[]	=	"sin";
const sal_Char __FAR_DATA sCalc_Cos[]	=	"cos";
const sal_Char __FAR_DATA sCalc_Tan[]	=	"tan";
const sal_Char __FAR_DATA sCalc_Asin[]	=	"asin";
const sal_Char __FAR_DATA sCalc_Acos[]	=	"acos";
const sal_Char __FAR_DATA sCalc_Atan[]	=	"atan";
const sal_Char __FAR_DATA sCalc_Round[]	=	"round";
const sal_Char __FAR_DATA sCalc_Date[]	=	"date";



//!!!!! ACHTUNG - Sortierte Liste aller Operatoren !!!!!
struct _CalcOp
{
    union{
        const sal_Char* pName;
        const String* pUName;
    };
    SwCalcOper eOp;
};

_CalcOp	__READONLY_DATA aOpTable[] = {
/* ACOS */    {{sCalc_Acos},       CALC_ACOS},  // Arcuscosinus
/* ADD */     {{sCalc_Add},        CALC_PLUS},  // Addition
/* AND */     {{sCalc_And},        CALC_AND},  	// log. und
/* ASIN */    {{sCalc_Asin},       CALC_ASIN},  // Arcussinus
/* ATAN */    {{sCalc_Atan},       CALC_ATAN},  // Arcustangens
/* COS */     {{sCalc_Cos},        CALC_COS},  	// Cosinus
/* DATE */    {{sCalc_Date},       CALC_DATE},	// Date
/* DIV */     {{sCalc_Div},        CALC_DIV},   // Dividieren
/* EQ */      {{sCalc_Eq},         CALC_EQ},   	// gleich
/* G */       {{sCalc_G},          CALC_GRE},  	// groesser
/* GEQ */     {{sCalc_Geq},        CALC_GEQ},  	// groesser gleich
/* L */       {{sCalc_L},          CALC_LES},  	// kleiner
/* LEQ */     {{sCalc_Leq},        CALC_LEQ},  	// kleiner gleich
/* MAX */     {{sCalc_Max},        CALC_MAX},  	// Maximalwert
/* MEAN */    {{sCalc_Mean},       CALC_MEAN},  // Mittelwert
/* MIN */     {{sCalc_Min},        CALC_MIN},  	// Minimalwert
/* MUL */     {{sCalc_Mul},        CALC_MUL},  	// Multiplizieren
/* NEQ */     {{sCalc_Neq},        CALC_NEQ},  	// nicht gleich
/* NOT */     {{sCalc_Not},        CALC_NOT},  	// log. nicht
/* OR */      {{sCalc_Or},         CALC_OR},   	// log. oder
/* PHD */     {{sCalc_Phd},        CALC_PHD},   // Prozent
/* POW */     {{sCalc_Pow},        CALC_POW},	// Potenzieren
/* ROUND */   {{sCalc_Round},      CALC_ROUND},	// Runden
/* SIN */     {{sCalc_Sin},        CALC_SIN},  	// Sinus
/* SQRT */    {{sCalc_Sqrt},       CALC_SQRT},	// Wurzel
/* SUB */     {{sCalc_Sub},        CALC_MINUS},	// Subtraktion
/* SUM */     {{sCalc_Sum},        CALC_SUM},  	// Summe
/* TAN */     {{sCalc_Tan},        CALC_TAN},  	// Tangens
/* XOR */     {{sCalc_Xor},        CALC_XOR}  	// log. xoder
};

double __READONLY_DATA nRoundVal[] = {
    5.0e+0,	0.5e+0,	0.5e-1,	0.5e-2,	0.5e-3,	0.5e-4,	0.5e-5,	0.5e-6,
    0.5e-7,	0.5e-8,	0.5e-9,	0.5e-10,0.5e-11,0.5e-12,0.5e-13,0.5e-14,
    0.5e-15,0.5e-16
};

double __READONLY_DATA nKorrVal[] = {
    9, 9e-1, 9e-2, 9e-3, 9e-4, 9e-5, 9e-6, 9e-7, 9e-8,
    9e-9, 9e-10, 9e-11, 9e-12, 9e-13, 9e-14
};

    // First character may be any alphabetic or underscore.
const sal_Int32 coStartFlags =
        i18n::KParseTokens::ANY_LETTER_OR_NUMBER |
        i18n::KParseTokens::ASC_UNDERSCORE |
        i18n::KParseTokens::IGNORE_LEADING_WS;

    // Continuing characters may be any alphanumeric or underscore or dot.
const sal_Int32 coContFlags =
    ( coStartFlags | i18n::KParseTokens::ASC_DOT )
        & ~i18n::KParseTokens::IGNORE_LEADING_WS;


extern "C" {
static int
#if defined( WNT )
 __cdecl
#endif
#if defined( ICC )
 _Optlink
#endif
    OperatorCompare( const void *pFirst, const void *pSecond)
{
    int nRet = 0;
    if( CALC_NAME == ((_CalcOp*)pFirst)->eOp )
    {
        if( CALC_NAME == ((_CalcOp*)pSecond)->eOp )
            nRet = ((_CalcOp*)pFirst)->pUName->CompareTo(
                            *((_CalcOp*)pSecond)->pUName );
        else
            nRet = ((_CalcOp*)pFirst)->pUName->CompareToAscii(
                            ((_CalcOp*)pSecond)->pName );
    }
    else
    {
        if( CALC_NAME == ((_CalcOp*)pSecond)->eOp )
            nRet = -1 * ((_CalcOp*)pSecond)->pUName->CompareToAscii(
                            ((_CalcOp*)pFirst)->pName );
        else
            nRet = strcmp( ((_CalcOp*)pFirst)->pName,
                            ((_CalcOp*)pSecond)->pName );
    }
    return nRet;
}

}// extern "C"

_CalcOp* FindOperator( const String& rSrch )
{
    _CalcOp aSrch;
    aSrch.pUName = &rSrch;
    aSrch.eOp = CALC_NAME;

    return (_CalcOp*)bsearch(   (void*) &aSrch,
                                (void*) aOpTable,
                                sizeof( aOpTable ) / sizeof( _CalcOp ),
                                sizeof( _CalcOp ),
                                OperatorCompare );
}


//-----------------------------------------------------------------------------

SwHash* Find( const String& rStr, SwHash** ppTable, USHORT nTblSize,
                USHORT* pPos )
{
    ULONG ii = 0;
    for( xub_StrLen n = 0; n < rStr.Len(); ++n )
        ii = ii << 1 ^ rStr.GetChar( n );
    ii %= nTblSize;

    if( pPos )
        *pPos = (USHORT)ii;

    for( SwHash* pEntry = *(ppTable+ii); pEntry; pEntry = pEntry->pNext )
        if( rStr == pEntry->aStr )
            return pEntry;
    return 0;
}

inline LanguageType GetDocAppScriptLang( SwDoc& rDoc )
{
    return ((SvxLanguageItem&)rDoc.GetDefault(
                            GetWhichOfScript( RES_CHRATR_LANGUAGE,
                                GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() ))
            )).GetLanguage();
}

double lcl_ConvertToDateValue( SwDoc& rDoc, sal_Int32 nDate )
{
    double nRet = 0;
    SvNumberFormatter* pFormatter = rDoc.GetNumberFormatter();
    if( pFormatter )
    {
        Date* pNull = pFormatter->GetNullDate();
        Date aDate( nDate >> 24, (nDate & 0x00FF0000) >> 16, nDate & 0x0000FFFF );
        nRet = aDate - *pNull;
    }
    return nRet;
}

//-----------------------------------------------------------------------------

/******************************************************************************
|*
|*	SwCalc::SwCalc( SwDoc* pD ) :
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	JP 03.11.95
|*
|******************************************************************************/

SwCalc::SwCalc( SwDoc& rD )
    :
    aErrExpr( aEmptyStr, SwSbxValue(), 0 ),
    rDoc( rD ),
    pLclData( &SvtSysLocale().GetLocaleData() ),
    pCharClass( &GetAppCharClass() ),
    nListPor( 0 ),
    eError( CALC_NOERR )
{
    aErrExpr.aStr.AssignAscii( "~C_ERR~" );
    memset( VarTable, 0, sizeof(VarTable) );
    LanguageType eLang = GetDocAppScriptLang( rDoc );

    if( eLang != SvxLocaleToLanguage( pLclData->getLocale() ) ||
        eLang != SvxLocaleToLanguage( pCharClass->getLocale() ) )
    {
        STAR_NMSPC::lang::Locale aLocale( SvxCreateLocale( eLang ));
        STAR_REFERENCE( lang::XMultiServiceFactory ) xMSF(
                            ::comphelper::getProcessServiceFactory() );
        pCharClass = new CharClass( xMSF, aLocale );
        pLclData = new LocaleDataWrapper( xMSF, aLocale );
    }

    sCurrSym = pLclData->getCurrSymbol();
    sCurrSym.EraseLeadingChars().EraseTrailingChars();
    pCharClass->toLower( sCurrSym );

static sal_Char __READONLY_DATA
    sNType0[] = "false",
    sNType1[] = "true",
    sNType2[] = "pi",
    sNType3[] = "e",
    sNType4[] = "tables",
    sNType5[] = "graf",
    sNType6[] = "ole",
    sNType7[] = "page",
    sNType8[] = "para",
    sNType9[] = "word",
    sNType10[]= "char",

    sNType11[] = "user_firstname" ,
    sNType12[] = "user_lastname" ,
    sNType13[] = "user_initials" ,
    sNType14[] = "user_company" ,
    sNType15[] = "user_street" ,
    sNType16[] = "user_country" ,
    sNType17[] = "user_zipcode" ,
    sNType18[] = "user_city" ,
    sNType19[] = "user_title" ,
    sNType20[] = "user_position" ,
    sNType21[] = "user_tel_work" ,
    sNType22[] = "user_tel_home" ,
    sNType23[] = "user_fax" ,
    sNType24[] = "user_email" ,
    sNType25[] = "user_state" ,
    sNType26[] = "graph"
    ;

static const sal_Char* __READONLY_DATA sNTypeTab[ 27 ] =
{
    sNType0, sNType1, sNType2, sNType3, sNType4, sNType5,
    sNType6, sNType7, sNType8, sNType9, sNType10, sNType11,
    sNType12, sNType13, sNType14, sNType15, sNType16, sNType17,
    sNType18, sNType19, sNType20, sNType21, sNType22, sNType23,
    sNType24,

    // diese sind mit doppelten HashIds
    sNType25, sNType26
};
static USHORT __READONLY_DATA aHashValue[ 27 ] =
{
    34, 38, 43,  7, 18, 32, 22, 29, 30, 33,  3,
    28, 24, 40,  9, 11, 26, 45,  4, 23, 36, 44, 19,  5,  1,
    // diese sind mit doppelten HashIds
    11, 38
};
static USHORT __READONLY_DATA aAdrToken[ 12 ] =
{
    USER_OPT_COMPANY, USER_OPT_STREET, USER_OPT_COUNTRY, USER_OPT_ZIP,
    USER_OPT_CITY, USER_OPT_TITLE, USER_OPT_POSITION, USER_OPT_TELEPHONEWORK,
    USER_OPT_TELEPHONEHOME, USER_OPT_FAX, USER_OPT_EMAIL, USER_OPT_STATE
};

static USHORT SwDocStat::* __READONLY_DATA aDocStat1[ 3 ] =
{
    &SwDocStat::nTbl, &SwDocStat::nGrf, &SwDocStat::nOLE
};
static ULONG SwDocStat::* __READONLY_DATA aDocStat2[ 4 ] =
{
    &SwDocStat::nPage, &SwDocStat::nPara,
    &SwDocStat::nWord, &SwDocStat::nChar
};

#if TBLSZ != 47
#error Alle Hashwerte angepasst?
#endif

    const SwDocStat& rDocStat = rDoc.GetDocStat();

    SwSbxValue nVal;
    String sTmpStr;
    USHORT n;

    for( n = 0; n < 25; ++n )
    {
        sTmpStr.AssignAscii( sNTypeTab[ n ] );
        VarTable[ aHashValue[ n ] ] = new SwCalcExp( sTmpStr, nVal, 0 );
    }

    ((SwCalcExp*)VarTable[ aHashValue[ 0 ] ])->nValue.PutBool( FALSE );
    ((SwCalcExp*)VarTable[ aHashValue[ 1 ] ])->nValue.PutBool( TRUE );
    ((SwCalcExp*)VarTable[ aHashValue[ 2 ] ])->nValue.PutDouble( F_PI );
    ((SwCalcExp*)VarTable[ aHashValue[ 3 ] ])->nValue.PutDouble( 2.7182818284590452354 );

    for( n = 0; n < 3; ++n )
        ((SwCalcExp*)VarTable[ aHashValue[ n + 4 ] ])->nValue.PutLong( rDocStat.*aDocStat1[ n ]  );
    for( n = 0; n < 4; ++n )
        ((SwCalcExp*)VarTable[ aHashValue[ n + 7 ] ])->nValue.PutLong( rDocStat.*aDocStat2[ n ]  );

    SvtUserOptions& rUserOptions = SW_MOD()->GetUserOptions();

    ((SwCalcExp*)VarTable[ aHashValue[ 11 ] ])->nValue.PutString( (String)rUserOptions.GetFirstName() );
    ((SwCalcExp*)VarTable[ aHashValue[ 12 ] ])->nValue.PutString( (String)rUserOptions.GetLastName() );
    ((SwCalcExp*)VarTable[ aHashValue[ 13 ] ])->nValue.PutString( (String)rUserOptions.GetID() );

    for( n = 0; n < 11; ++n )
        ((SwCalcExp*)VarTable[ aHashValue[ n + 14 ] ])->nValue.PutString(
                                        (String)rUserOptions.GetToken( aAdrToken[ n ] ));

    nVal.PutString( (String)rUserOptions.GetToken( aAdrToken[ 11 ] ));
    sTmpStr.AssignAscii( sNTypeTab[ 25 ] );
    VarTable[ aHashValue[ 25 ] ]->pNext = new SwCalcExp( sTmpStr, nVal, 0 );

// at time its better not to use "graph", because then the im-/export have
// to change in all formulas this name.
//	nVal.PutLong( rDocStat.*aDocStat1[ 1 ]  );
//	VarTable[ aHashValue[ 26 ] ]->pNext = new SwCalcExp(
//												sNTypeTab[ 26 ], nVal, 0 );
}

/******************************************************************************
|*
|*	SwCalc::~SwCalc()
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	OK 12-02-93 11:04am
|*
|******************************************************************************/

SwCalc::~SwCalc()
{
    for( USHORT n = 0; n < TBLSZ; ++n )
        delete VarTable[n];
    if( pLclData != &SvtSysLocale().GetLocaleData() )
        delete pLclData;
    if( pCharClass != &GetAppCharClass() )
        delete pCharClass;
}

/******************************************************************************
|*
|*	SwSbxValue SwCalc::Calculate( const String& rStr )
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	OK 12-02-93 11:04am
|*
|******************************************************************************/

SwSbxValue SwCalc::Calculate( const String& rStr )
{
    eError = CALC_NOERR;
    SwSbxValue nResult;

    if( !rStr.Len() )
        return nResult;

    nListPor = 0;
    eCurrListOper = CALC_PLUS;			// defaulten auf Summe

    sCommand = rStr;
    nCommandPos = 0;

    while( (eCurrOper = GetToken()) != CALC_ENDCALC && eError == CALC_NOERR )
        nResult = Expr();

    if( eError )
        nResult.PutDouble( DBL_MAX );

    return nResult;
}

/******************************************************************************
|*
|*	String SwCalc::GetStrResult( SwSbxValue nValue, BOOL bRound = TRUE )
|*	Beschreibung		Der Parameter bRound ist auf TRUE defaultet und darf
|*						nur beim errechnen von Tabellenzellen auf FALSE gesetzt
|*						werden, damit keine Rundungsfehler beim zusammenstellen
|*						der Formel entstehen.
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	JP 19.02.98
|*
|******************************************************************************/

String SwCalc::GetStrResult( const SwSbxValue& rVal, BOOL bRound )
{
    if( !rVal.IsDouble() )
        return rVal.GetString();

    return GetStrResult( rVal.GetDouble(), bRound );
}


String SwCalc::GetStrResult( double nValue, BOOL )
{
    if( nValue >= DBL_MAX )
        switch( eError )
        {
            case CALC_SYNTAX	:	return RESOURCE->aCalc_Syntax;
            case CALC_ZERODIV	:	return RESOURCE->aCalc_ZeroDiv;
            case CALC_BRACK		:	return RESOURCE->aCalc_Brack;
            case CALC_POWERR	:	return RESOURCE->aCalc_Pow;
            case CALC_VARNFND	:	return RESOURCE->aCalc_VarNFnd;
            case CALC_OVERFLOW	:	return RESOURCE->aCalc_Overflow;
            case CALC_WRONGTIME :	return RESOURCE->aCalc_WrongTime;
            default				:	return RESOURCE->aCalc_Default;
        }

    USHORT	nDec = 15; //pLclData->getNumDigits();
    String	aRetStr( ::rtl::math::doubleToUString( nValue,
                rtl_math_StringFormat_Automatic,
                nDec,
                pLclData->getNumDecimalSep().GetChar(0),
                true ));

    return aRetStr;
}

/******************************************************************************
|*
|*	SwCalcExp* SwCalc::VarLook( const String& )
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	JP 15.11.99
|*
|******************************************************************************/

SwCalcExp* SwCalc::VarInsert( const String &rStr )
{
    String aStr( rStr );
    pCharClass->toLower( aStr );
    return VarLook( aStr, 1 );
}

/******************************************************************************
|*
|*	SwCalcExp* SwCalc::VarLook( const String& , USHORT ins )
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	JP 15.11.99
|*
|******************************************************************************/
SwCalcExp* SwCalc::VarLook( const String& rStr, USHORT ins )
{
    aErrExpr.nValue.SetVoidValue(false);

    USHORT ii = 0;
    String aStr( rStr );
    pCharClass->toLower( aStr );

    SwHash* pFnd = Find( aStr, VarTable, TBLSZ, &ii );

    if( !pFnd )
    {
        // dann sehen wir mal im Doc nach:
        SwHash** ppDocTbl = rDoc.GetUpdtFlds().GetFldTypeTable();
        for( SwHash* pEntry = *(ppDocTbl+ii); pEntry; pEntry = pEntry->pNext )
            if( aStr == pEntry->aStr )
            {
                // dann hier zufuegen
                pFnd = new SwCalcExp( aStr, SwSbxValue(),
                                    ((SwCalcFldType*)pEntry)->pFldType );
                pFnd->pNext = *(VarTable+ii);
                *(VarTable+ii) = pFnd;
                break;
            }
    }

    if( pFnd )
    {
        SwCalcExp* pFndExp = (SwCalcExp*)pFnd;

        if( pFndExp->pFldType && pFndExp->pFldType->Which() == RES_USERFLD )
        {
            SwUserFieldType* pUFld = (SwUserFieldType*)pFndExp->pFldType;
            if( nsSwGetSetExpType::GSE_STRING & pUFld->GetType() )
                pFndExp->nValue.PutString( pUFld->GetContent() );
            else if( !pUFld->IsValid() )
            {
                // Die aktuellen Werte sichern . . .
                USHORT			nOld_ListPor		= nListPor;
                SwSbxValue		nOld_LastLeft		= nLastLeft;
                SwSbxValue		nOld_NumberValue	= nNumberValue;
                xub_StrLen		nOld_CommandPos		= nCommandPos;
                SwCalcOper		eOld_CurrOper	  	= eCurrOper;
                SwCalcOper		eOld_CurrListOper	= eCurrListOper;

                pFndExp->nValue.PutDouble( pUFld->GetValue( *this ) );

                // . . . und zurueck damit.
                nListPor		= nOld_ListPor;
                nLastLeft		= nOld_LastLeft;
                nNumberValue	= nOld_NumberValue;
                nCommandPos		= nOld_CommandPos;
                eCurrOper		= eOld_CurrOper;
                eCurrListOper	= eOld_CurrListOper;
            }
            else
                pFndExp->nValue.PutDouble( pUFld->GetValue() );
        }
        return pFndExp;
    }

    // Name(p)=Adress.PLZ oder Adress.DATENSATZNUMMER
    // DBSETNUMBERFLD = DatenSATZ-nummernfeld (NICHT "setze Datensatznummer!!!")
    // #101436#: At this point the "real" case variable has to be used
    String sTmpName( rStr );
    ::ReplacePoint( sTmpName );

    if( !ins )
    {
        SwNewDBMgr *pMgr = rDoc.GetNewDBMgr();

        // Name(p)=Adress.PLZ oder Adress.DATENSATZNUMMER
        // DBSETNUMBERFLD = DatenSATZ-nummernfeld (NICHT "setze Datensatznummer!!!")
        String sDBName(GetDBName( sTmpName ));
        String sSourceName(sDBName.GetToken(0, DB_DELIM));
        String sTableName(sDBName.GetToken(0).GetToken(1, DB_DELIM));
        if( pMgr && sSourceName.Len() && sTableName.Len() &&
            pMgr->OpenDataSource(sSourceName, sTableName, -1, false))
        {
            String sColumnName( GetColumnName( sTmpName ));
            ASSERT (sColumnName.Len(), "DB-Spaltenname fehlt!");

            String sDBNum( SwFieldType::GetTypeStr(TYP_DBSETNUMBERFLD) );
            pCharClass->toLower(sDBNum);

            // Hier nochmal initialisieren, da das nicht mehr in docfld
            // fuer Felder != RES_DBFLD geschieht. Z.B. wenn ein Expressionfield
            // vor einem DB_Field in einem Dok vorkommt.
            VarChange( sDBNum, pMgr->GetSelectedRecordId(sSourceName, sTableName));

            if( sDBNum.EqualsIgnoreCaseAscii(sColumnName) )
            {
                aErrExpr.nValue.PutLong(long(pMgr->GetSelectedRecordId(sSourceName, sTableName)));
                return &aErrExpr;
            }

            ULONG nTmpRec = 0;
            if( 0 != ( pFnd = Find( sDBNum, VarTable, TBLSZ ) ) )
                nTmpRec = ((SwCalcExp*)pFnd)->nValue.GetULong();

            String sResult;
            double nNumber = DBL_MAX;

            long nLang = SvxLocaleToLanguage( pLclData->getLocale() );
            if(pMgr->GetColumnCnt( sSourceName, sTableName, sColumnName,
                                    nTmpRec, nLang, sResult, &nNumber ))
            {
                if (nNumber != DBL_MAX)
                    aErrExpr.nValue.PutDouble( nNumber );
                else
                    aErrExpr.nValue.PutString( sResult );

                return &aErrExpr;
            }
        }
        else
        {
            //data source was not available - set return to "NoValue"
            aErrExpr.nValue.SetVoidValue(true);
        }
        // auf keinen fall eintragen!!
        return &aErrExpr;
    }


    SwCalcExp* pNewExp = new SwCalcExp( aStr, SwSbxValue(), 0 );
    pNewExp->pNext = VarTable[ ii ];
    VarTable[ ii ] = pNewExp;

    String sColumnName( GetColumnName( sTmpName ));
    ASSERT( sColumnName.Len(), "DB-Spaltenname fehlt!" );
    if( sColumnName.EqualsIgnoreCaseAscii(
                            SwFieldType::GetTypeStr( TYP_DBSETNUMBERFLD ) ))
    {
        SwNewDBMgr *pMgr = rDoc.GetNewDBMgr();
        String sDBName(GetDBName( sTmpName ));
        String sSourceName(sDBName.GetToken(0, DB_DELIM));
        String sTableName(sDBName.GetToken(0).GetToken(1, DB_DELIM));
        if( pMgr && sSourceName.Len() && sTableName.Len() &&
            pMgr->OpenDataSource(sSourceName, sTableName, -1, false) &&
            !pMgr->IsInMerge())
            pNewExp->nValue.PutULong( pMgr->GetSelectedRecordId(sSourceName, sTableName));
        else
            pNewExp->nValue.SetVoidValue(true);
    }

    return pNewExp;
}

/******************************************************************************
|*
|*	BOOL SwCalc::VarChange( const String& rStr, const SwSbxValue nValue )
|*
|*	Erstellung			OK 12-02-93 11:04am
|*	Letzte Aenderung	OK 12-02-93 11:04am
|*
|******************************************************************************/

void SwCalc::VarChange( const String& rStr, double nValue )
{
    SwSbxValue aVal( nValue );
    VarChange( rStr, aVal );
}

void SwCalc::VarChange( const String& rStr, const SwSbxValue& rValue )
{
    String aStr( rStr );
    pCharClass->toLower( aStr );

    USHORT nPos = 0;
    SwCalcExp* pFnd = (SwCalcExp*)Find( aStr, VarTable, TBLSZ, &nPos );

    if( !pFnd )
    {
        pFnd = new SwCalcExp( aStr, SwSbxValue( rValue ), 0 );
        pFnd->pNext = VarTable[ nPos ];
        VarTable[ nPos ] = pFnd;
    }
    else
        pFnd->nValue = rValue;
}

/******************************************************************************
|*
|*	BOOL SwCalc::Push( const void* pPtr )
|*
|*	Erstellung			OK 12-02-93 11:05am
|*	Letzte Aenderung	OK 12-02-93 11:05am
|*
|******************************************************************************/

BOOL SwCalc::Push( const VoidPtr pPtr )
{
    if( USHRT_MAX != aRekurStk.GetPos( pPtr ) )
        return FALSE;

    aRekurStk.Insert( pPtr, aRekurStk.Count() );
    return TRUE;
}

/******************************************************************************
|*
|*	void SwCalc::Pop( const void* pPtr )
|*
|*	Erstellung			OK 12-02-93 11:05am
|*	Letzte Aenderung	OK 12-02-93 11:05am
|*
|******************************************************************************/

void SwCalc::Pop( const VoidPtr )
{
    ASSERT( aRekurStk.Count(), "SwCalc: Pop auf ungueltigen Ptr" );

    aRekurStk.Remove( aRekurStk.Count() - 1 );
}


/******************************************************************************
|*
|*	SwCalcOper SwCalc::GetToken()
|*
|*	Erstellung			OK 12-02-93 11:05am
|*	Letzte Aenderung	JP 03.11.95
|*
|******************************************************************************/

SwCalcOper SwCalc::GetToken()
{
#if OSL_DEBUG_LEVEL > 1
//JP 25.01.2001: static for switch back to the "old" implementation of the
//				calculator, which don't use the I18N routines.
static int nUseOld = 0;
if( !nUseOld )
{
#endif

    if( nCommandPos >= sCommand.Len() )
        return eCurrOper = CALC_ENDCALC;

    using namespace ::com::sun::star::i18n;
    {
        // Parse any token.
        ParseResult aRes = pCharClass->parseAnyToken( sCommand, nCommandPos,
                                                    coStartFlags, aEmptyStr,
                                                    coContFlags, aEmptyStr );

        BOOL bSetError = TRUE;
        xub_StrLen nRealStt = nCommandPos + (xub_StrLen)aRes.LeadingWhiteSpace;
        if( aRes.TokenType & (KParseType::ASC_NUMBER | KParseType::UNI_NUMBER) )
        {
            nNumberValue.PutDouble( aRes.Value );
            eCurrOper = CALC_NUMBER;
            bSetError = FALSE;
        }
        else if( aRes.TokenType & KParseType::IDENTNAME )
        {
            String aName( sCommand.Copy( nRealStt, static_cast<xub_StrLen>(aRes.EndPos) - nRealStt ));
            //#101436#: the variable may contain a database name it must not be converted to lower case
            // instead all further comparisons must be done case-insensitive
            //pCharClass->toLower( aName );
            String sLowerCaseName(aName);
            pCharClass->toLower( sLowerCaseName );
            // Currency-Symbol abfangen
            if( sLowerCaseName == sCurrSym )
            {
                nCommandPos = (xub_StrLen)aRes.EndPos;
                return GetToken();	// also nochmal aufrufen
            }

            // Operations abfangen
            _CalcOp* pFnd = ::FindOperator( sLowerCaseName );
            if( pFnd )
            {
                switch( ( eCurrOper = ((_CalcOp*)pFnd)->eOp ) )
                {
                    case CALC_SUM:
                    case CALC_MEAN:
                        eCurrListOper = CALC_PLUS;
                        break;
                    case CALC_MIN:
                        eCurrListOper = CALC_MIN_IN;
                        break;
                    case CALC_MAX:
                        eCurrListOper = CALC_MAX_IN;
                        break;
                    case CALC_DATE:
                        eCurrListOper = CALC_MONTH;
                        break;
                    default:
                        break;
                }
                nCommandPos = (xub_StrLen)aRes.EndPos;
                return eCurrOper;
            }
            aVarName = aName;
            eCurrOper = CALC_NAME;
            bSetError = FALSE;
        }
        else if ( aRes.TokenType & KParseType::DOUBLE_QUOTE_STRING )
        {
            nNumberValue.PutString( String( aRes.DequotedNameOrString ));
            eCurrOper = CALC_NUMBER;
            bSetError = FALSE;
        }
        else if( aRes.TokenType & KParseType::ONE_SINGLE_CHAR )
        {
            String aName( sCommand.Copy( nRealStt, static_cast<xub_StrLen>(aRes.EndPos) - nRealStt ));
            if( 1 == aName.Len() )
            {
                bSetError = FALSE;
                sal_Unicode ch = aName.GetChar( 0 );
                switch( ch )
                {
                case ';': if( CALC_MONTH == eCurrListOper ||
                              CALC_DAY == eCurrListOper )
                          {
                              eCurrOper = eCurrListOper;
                              break;
                          }
                case '\n':
                            eCurrOper = CALC_PRINT;
                            break;
                case '%':
                case '^':
                case '*':
                case '/':
                case '+':
                case '-':
                case '(':
                case ')':	eCurrOper = SwCalcOper(ch);
                            break;

                case '=':
                case '!':
                        {
                            SwCalcOper eTmp2;
                            if( '=' == ch )
                                eCurrOper = SwCalcOper('='), eTmp2 = CALC_EQ;
                            else
                                eCurrOper = CALC_NOT, eTmp2 = CALC_NEQ;

                            if( aRes.EndPos < sCommand.Len() &&
                                '=' == sCommand.GetChar( (xub_StrLen)aRes.EndPos ) )
                            {
                                eCurrOper = eTmp2;
                                ++aRes.EndPos;
                            }
                        }
                        break;

                case cListDelim :
                        eCurrOper = eCurrListOper;
                        break;

                case '[':
                        if( aRes.EndPos < sCommand.Len() )
                        {
                            aVarName.Erase();
                            xub_StrLen nFndPos = (xub_StrLen)aRes.EndPos,
                                        nSttPos = nFndPos;

                            do{
                                if( STRING_NOTFOUND != ( nFndPos =
                                    sCommand.Search( ']', nFndPos )) )
                                {
                                    // ignore the ]
                                    if( '\\' == sCommand.GetChar(nFndPos-1))
                                    {
                                        aVarName += sCommand.Copy( nSttPos,
                                                    nFndPos - nSttPos - 1 );
                                        nSttPos = ++nFndPos;
                                    }
                                    else
                                        break;
                                }
                            } while( STRING_NOTFOUND != nFndPos );

                            if( STRING_NOTFOUND != nFndPos )
                            {
                                if( nSttPos != nFndPos )
                                    aVarName += sCommand.Copy( nSttPos,
                                                    nFndPos - nSttPos );
                                aRes.EndPos = nFndPos + 1;
                                eCurrOper = CALC_NAME;
                            }
                            else
                                bSetError = TRUE;
                        }
                        else
                            bSetError = TRUE;
                        break;

                default:
                    bSetError = TRUE;
                    break;
                }
            }
        }
        else if( aRes.TokenType & KParseType::BOOLEAN )
        {
            String aName( sCommand.Copy( nRealStt, static_cast<xub_StrLen>(aRes.EndPos) - nRealStt ));
            if( aName.Len() )
            {
                sal_Unicode ch = aName.GetChar(0);

                bSetError = TRUE;
                if ('<' == ch || '>' == ch)
                {
                    bSetError = FALSE;

                    SwCalcOper eTmp2 = ('<' == ch) ? CALC_LEQ : CALC_GEQ;
                    eCurrOper = ('<' == ch) ? CALC_LES : CALC_GRE;

                    if( 2 == aName.Len() && '=' == aName.GetChar(1) )
                        eCurrOper = eTmp2;
                    else if( 1 != aName.Len() )
                        bSetError = TRUE;
                }
            }
        }
        else if( nRealStt == sCommand.Len() )
        {
            eCurrOper = CALC_ENDCALC;
            bSetError = FALSE;
        }

        if( bSetError )
        {
            eError = CALC_SYNTAX;
            eCurrOper = CALC_PRINT;
        }
        nCommandPos = (xub_StrLen)aRes.EndPos;
    };

#if OSL_DEBUG_LEVEL > 1

#define NextCh( s, n )	(nCommandPos < sCommand.Len() ? sCommand.GetChar( nCommandPos++ ) : 0)

}
else
{
    sal_Unicode ch;
    sal_Unicode cTSep = pLclData->getNumThousandSep().GetChar(0),
                cDSep = pLclData->getNumDecimalSep().GetChar(0);

    do {
        if( 0 == ( ch = NextCh( sCommand, nCommandPos ) ) )
            return eCurrOper = CALC_ENDCALC;
    } while ( ch == '\t' || ch == ' ' || ch == cTSep );

    if( ch == cDSep )
        ch = '.';

    switch( ch )
    {
        case ';': if( CALC_MONTH == eCurrListOper || CALC_DAY == eCurrListOper )
                  {
                      eCurrOper = eCurrListOper;
                      break;
                  } // else .. no break
        case '\n':
                    {
                        sal_Unicode c;
                        while( nCommandPos < sCommand.Len() && ( ( c =
                                sCommand.GetChar( nCommandPos ) ) == ' ' ||
                                c == '\t' || c == '\x0a' || c == '\x0d' ))
                            ++nCommandPos;
                        eCurrOper = CALC_PRINT;
                    }
                    break;
        case '%':
        case '^':
        case '*':
        case '/':
        case '+':
        case '-':
        case '(':
        case ')':	eCurrOper = SwCalcOper(ch);
                    break;

        case '=':   if( '=' == sCommand.GetChar( nCommandPos ) )
                    {
                        ++nCommandPos;
                        eCurrOper = CALC_EQ;
                    }
                    else
                        eCurrOper = SwCalcOper(ch);
                    break;

        case '!':   if( '=' == sCommand.GetChar( nCommandPos ) )
                    {
                        ++nCommandPos;
                        eCurrOper = CALC_NEQ;
                    }
                    else
                        eCurrOper = CALC_NOT;
                    break;

        case '>':
        case '<':   eCurrOper = '>' == ch  ? CALC_GRE : CALC_LES;
                    if( '=' == (ch = sCommand.GetChar( nCommandPos ) ) )
                    {
                        ++nCommandPos;
                        eCurrOper = CALC_GRE == eCurrOper ? CALC_GEQ : CALC_LEQ;
                    }
                    else if( ' ' != ch )
                    {
                        eError = CALC_SYNTAX;
                        eCurrOper = CALC_PRINT;
                    }
                    break;

        case cListDelim :
                    eCurrOper = eCurrListOper;
                    break;

        case '0':	case '1':	case '2':	case '3':	case '4':
        case '5':	case '6':	case '7':	case '8':	case '9':
        case ',':
        case '.':	{
                        double nVal;
                        --nCommandPos; 		//  auf das 1. Zeichen zurueck
                        if( Str2Double( sCommand, nCommandPos, nVal, pLclData ))
                        {
                            nNumberValue.PutDouble( nVal );
                            eCurrOper = CALC_NUMBER;
                        }
                        else
                        {
                            // fehlerhafte Zahl
                            eError = CALC_SYNTAX;
                            eCurrOper = CALC_PRINT;
                        }
                    }
                    break;

        case '[':	{
                        String aStr;
                        BOOL bIgnore = FALSE;
                        do {
                            while( 0 != ( ch = NextCh( sCommand, nCommandPos  ))
                                    && ch != ']' )
                            {
                                if( !bIgnore && '\\' == ch )
                                    bIgnore = TRUE;
                                else if( bIgnore )
                                    bIgnore = FALSE;
                                aStr += ch;
                            }

                            if( !bIgnore )
                                break;

                            aStr.SetChar( aStr.Len() - 1, ch );
                        } while( ch );

                        aVarName = aStr;
                        eCurrOper = CALC_NAME;
                    }
                    break;

        case '"':	{
                        xub_StrLen nStt = nCommandPos;
                        while( 0 != ( ch = NextCh( sCommand, nCommandPos ) )
                                && '"' != ch )
                            ;

                        xub_StrLen nLen = nCommandPos - nStt;
                        if( '"' == ch )
                            --nLen;
                        nNumberValue.PutString( sCommand.Copy( nStt, nLen ));
                        eCurrOper = CALC_NUMBER;
                    }
                    break;

        default:	if( ch && pCharClass->isLetter( sCommand, nCommandPos - 1)
                            || '_' == ch )
                    {
                        xub_StrLen nStt = nCommandPos-1;
                        while( 0 != (ch = NextCh( sCommand, nCommandPos )) &&
                               (pCharClass->isLetterNumeric(
                                                   sCommand, nCommandPos - 1) ||
                                ch == '_' || ch == '.' ) )
                            ;

                        if( ch )
                            --nCommandPos;

                        String aStr( sCommand.Copy( nStt, nCommandPos-nStt ));
                        pCharClass->toLower( aStr );


                        // Currency-Symbol abfangen
                        if( aStr == sCurrSym )
                            return GetToken();	// also nochmal aufrufen

                        // Operations abfangen
                        _CalcOp* pFnd = ::FindOperator( aStr );
                        if( pFnd )
                        {
                            switch( ( eCurrOper = ((_CalcOp*)pFnd)->eOp ) )
                            {
                                case CALC_SUM  :
                                case CALC_MEAN : eCurrListOper = CALC_PLUS;
                                                    break;
                                case CALC_MIN  : eCurrListOper = CALC_MIN_IN;
                                                    break;
                                case CALC_MAX  : eCurrListOper = CALC_MAX_IN;
                                                    break;
                                case CALC_DATE  : eCurrListOper = CALC_MONTH;
                                                    break;
                                default :
                                    break;
                            }
                            return eCurrOper;
                        }
                        aVarName = aStr;
                        eCurrOper = CALC_NAME;
                    }
                    else
                    {
                        eError = CALC_SYNTAX;
                        eCurrOper = CALC_PRINT;
                    }
                    break;
    }

}
#endif
    return eCurrOper;
}

/******************************************************************************
|*
|*	SwSbxValue SwCalc::Term()
|*
|*	Erstellung			OK 12-02-93 11:05am
|*	Letzte Aenderung	JP 16.01.96
|*
|******************************************************************************/

SwSbxValue SwCalc::Term()
{
    SwSbxValue left( Prim() );
    nLastLeft = left;
    for(;;)
    {
        USHORT nSbxOper = USHRT_MAX;

        switch( eCurrOper )
        {
// wir haben kein Bitweises verodern, oder ?
//			case CALC_AND:	eSbxOper = SbxAND;	break;
//			case CALC_OR:	eSbxOper = SbxOR;	break;
//			case CALC_XOR:	eSbxOper = SbxXOR;	break;
            case CALC_AND:	{
                                GetToken();
                                BOOL bB = Prim().GetBool();
                                left.PutBool( left.GetBool() && bB );
                            }
                            break;
            case CALC_OR:	{
                                GetToken();
                                BOOL bB = Prim().GetBool();
                                left.PutBool( left.GetBool() || bB );
                            }
                            break;
            case CALC_XOR:	{
                                GetToken();
                                BOOL bR = Prim().GetBool();
                                BOOL bL = left.GetBool();
                                left.PutBool( (bL && !bR) || (!bL && bR) );
                            }
                            break;

            case CALC_EQ:	nSbxOper = SbxEQ;	break;
            case CALC_NEQ:	nSbxOper = SbxNE;	break;
            case CALC_LEQ:	nSbxOper = SbxLE;	break;
            case CALC_GEQ:	nSbxOper = SbxGE;	break;
            case CALC_GRE:	nSbxOper = SbxGT;	break;
            case CALC_LES:	nSbxOper = SbxLT;	break;

            case CALC_MUL:	nSbxOper = SbxMUL; 	break;
            case CALC_DIV:	nSbxOper = SbxDIV;  break;

            case CALC_MIN_IN:
                            {
                                GetToken();
                                SwSbxValue e = Prim();
                                left = left.GetDouble() < e.GetDouble()
                                            ? left : e;
                            }
                            break;
            case CALC_MAX_IN:
                            {
                                GetToken();
                                SwSbxValue e = Prim();
                                left = left.GetDouble() > e.GetDouble()
                                            ? left : e;
                            }
                            break;
            case CALC_MONTH:
                            {
                                GetToken();
                                SwSbxValue e = Prim();
                                sal_Int32 nYear = (sal_Int32) floor( left.GetDouble() );
                                nYear = nYear & 0x0000FFFF;
                                sal_Int32 nMonth = (INT32) floor( e.GetDouble() );
                                nMonth = ( nMonth & 0x000000FF ) << 16;
                                left.PutLong( nMonth + nYear );
                                eCurrOper = CALC_DAY;
                            }
                            break;
            case CALC_DAY:
                            {
                                GetToken();
                                SwSbxValue e = Prim();
                                sal_Int32 nYearMonth = (sal_Int32) floor( left.GetDouble() );
                                nYearMonth = nYearMonth & 0x00FFFFFF;
                                sal_Int32 nDay = (sal_Int32) floor( e.GetDouble() );
                                nDay = ( nDay & 0x000000FF ) << 24;
                                left = lcl_ConvertToDateValue( rDoc, nDay + nYearMonth );
                            }
                            break;
            case CALC_ROUND:
                            {
                                GetToken();
                                SwSbxValue e = Prim();

                                double fVal = 0;
                                double fFac = 1;
                                INT32 nDec = (INT32) floor( e.GetDouble() );
                                if( nDec < -20 || nDec > 20 )
                                {
                                    eError = CALC_OVERFLOW;
                                    left.Clear();
                                    return left;
                                }
                                fVal = left.GetDouble();
                                USHORT i;
                                if( nDec >= 0)
                                    for (i = 0; i < (USHORT) nDec; ++i )
                                        fFac *= 10.0;
                                else
                                    for (i = 0; i < (USHORT) -nDec; ++i )
                                        fFac /= 10.0;

                                fVal *= fFac;

                                BOOL bSign;
                                if (fVal < 0.0)
                                {
                                    fVal *= -1.0;
                                    bSign = TRUE;
                                }
                                else
                                    bSign = FALSE;

                                // runden
                                double fNum = fVal;				// find the exponent
                                int nExp = 0;
                                if( fNum > 0 )
                                {
                                    while( fNum < 1.0 ) fNum *= 10.0, --nExp;
                                    while( fNum >= 10.0 ) fNum /= 10.0, ++nExp;
                                }
                                nExp = 15 - nExp;
                                if( nExp > 15 )
                                    nExp = 15;
                                else if( nExp <= 1 )
                                    nExp = 0;
                                fVal = floor( fVal+ 0.5 + nRoundVal[ nExp ] );

                                if (bSign)
                                    fVal *= -1.0;

                                fVal /= fFac;

                                left.PutDouble( fVal );
                            }
                            break;

/*
// removed here because of #77448# (=2*3^2 != 18)
            case CALC_POW:  {
                                GetToken();
                                double fraction, integer;
                                double right = Prim().GetDouble(),
                                       dleft = left.GetDouble();

                                fraction = modf( right, &integer );
                                if( ( dleft < 0.0 && 0.0 != fraction ) ||
                                    ( 0.0 == dleft && right < 0.0 ) )
                                {
                                    eError = CALC_OVERFLOW;
                                    left.Clear();
                                    return left;
                                }
                                dleft = pow(dleft, right );
                                if( dleft == HUGE_VAL )
                                {
                                    eError = CALC_POWERR;
                                    left.Clear();
                                    return left;
                                }
                                left.PutDouble( dleft );
                            }
                            break;
*/
            default:		return left;
        }

        if( USHRT_MAX != nSbxOper )
        {
            // #i47706: cast to SbxOperator AFTER comparing to USHRT_MAX
            SbxOperator eSbxOper = (SbxOperator)nSbxOper;

            GetToken();
            if( SbxEQ <= eSbxOper && eSbxOper <= SbxGE )
                left.PutBool( left.Compare( eSbxOper, Prim() ));
            else
            {
                SwSbxValue aRight( Prim() );
                aRight.MakeDouble();
                left.MakeDouble();

                if( SbxDIV == eSbxOper && !aRight.GetDouble() )
                    eError = CALC_ZERODIV;
                else
                    left.Compute( eSbxOper, aRight );
            }
        }
    }
}

/******************************************************************************
|*
|*	SwSbxValue SwCalc::Prim()
|*
|*	Erstellung			OK 12-02-93 11:05am
|*	Letzte Aenderung	JP 03.11.95
|*
|******************************************************************************/

extern "C" typedef double (*pfCalc)( double );

SwSbxValue SwCalc::Prim()
{
    SwSbxValue nErg;

    pfCalc pFnc = 0;

    BOOL bChkTrig = FALSE, bChkPow = FALSE;

    switch( eCurrOper )
    {
        case CALC_SIN:		pFnc = &sin;						break;
        case CALC_COS:		pFnc = &cos;						break;
        case CALC_TAN:		pFnc = &tan;						break;
        case CALC_ATAN:		pFnc = &atan;						break;
        case CALC_ASIN:		pFnc = &asin; 	bChkTrig = TRUE;	break;
        case CALC_ACOS:		pFnc = &acos; 	bChkTrig = TRUE;	break;

        case CALC_NOT:		{
                                GetToken();
                                nErg = Prim();
                                if( SbxSTRING == nErg.GetType() )
                                    nErg.PutBool( 0 == nErg.GetString().Len() );
                                else if(SbxBOOL == nErg.GetType() )
                                    nErg.PutBool(!nErg.GetBool());
                                // evaluate arguments manually so that the binary NOT below
                                // does not get called.
                                // We want a BOOLEAN NOT.
                                else if (nErg.IsNumeric())
                                    nErg.PutLong( nErg.GetDouble() == 0.0 ? 1 : 0 );
                                else
                                {
                                    DBG_ERROR( "unexpected case. computing binary NOT" );
                                    //!! computes a binary NOT
                                    nErg.Compute( SbxNOT, nErg );
                                }
                            }
                            break;

        case CALC_NUMBER:	if( GetToken() == CALC_PHD )
                            {
                                double aTmp = nNumberValue.GetDouble();
                                aTmp *= 0.01;
                                nErg.PutDouble( aTmp );
                                GetToken();
                            }
                            else if( eCurrOper == CALC_NAME )
                                eError = CALC_SYNTAX;
                            else
                            {
                                nErg = nNumberValue;
                                bChkPow = TRUE;
                            }
                            break;

        case CALC_NAME: 	if( GetToken() == CALC_ASSIGN )
                            {
                                SwCalcExp* n = VarInsert( aVarName );
                                GetToken();
                                nErg = n->nValue = Expr();
                            }
                            else
                            {
                                nErg = VarLook( aVarName )->nValue;
                                bChkPow = TRUE;
                            }
                            break;

        case CALC_MINUS:	GetToken();
                            nErg.PutDouble( -(Prim().GetDouble()) );
                            break;

        case CALC_LP:		{
                                GetToken();
                                nErg = Expr();
                                if( eCurrOper != CALC_RP )
                                    eError = CALC_BRACK;
                                else
                                {
                                    GetToken();
                                    bChkPow = TRUE; // in order for =(7)^2 to work
                                }
                            }
                            break;

        case CALC_MEAN:		{
                                nListPor = 1;
                                GetToken();
                                nErg = Expr();
                                double aTmp = nErg.GetDouble();
                                aTmp /= nListPor;
                                nErg.PutDouble( aTmp );
                            }
                            break;

        case CALC_SQRT:		{
                                GetToken();
                                nErg = Prim();
                                if( nErg.GetDouble() < 0 )
                                    eError = CALC_OVERFLOW;
                                else
                                    nErg.PutDouble( sqrt( nErg.GetDouble() ));
                            }
                            break;

        case CALC_SUM:
        case CALC_DATE:
        case CALC_MIN:
        case CALC_MAX:		GetToken();
                            nErg = Expr();
                            break;

        case CALC_ENDCALC:	nErg.Clear();
                            break;

        default:			eError = CALC_SYNTAX;
                            break;
    }

    if( pFnc )
    {
        GetToken();
        double nVal = Prim().GetDouble();
        if( !bChkTrig || ( nVal > -1 && nVal < 1 ) )
            nErg.PutDouble( (*pFnc)( nVal ) );
        else
            eError = CALC_OVERFLOW;
    }

    // added here because of #77448# (=2*3^2 should be 18)
    if( bChkPow && eCurrOper == CALC_POW )
    {
        double dleft = nErg.GetDouble();
        GetToken();
        double right = Prim().GetDouble();

        double fraction, integer;
        fraction = modf( right, &integer );
        if( ( dleft < 0.0 && 0.0 != fraction ) ||
            ( 0.0 == dleft && right < 0.0 ) )
        {
            eError = CALC_OVERFLOW;
            nErg.Clear();
        }
        else
        {
            dleft = pow(dleft, right );
            if( dleft == HUGE_VAL )
            {
                eError = CALC_POWERR;
                nErg.Clear();
            }
            else
            {
                nErg.PutDouble( dleft );
//				GetToken();
            }
        }
    }

    return nErg;
}

/******************************************************************************
|*
|*	SwSbxValue	SwCalc::Expr()
|*
|*	Erstellung			OK 12-02-93 11:06am
|*	Letzte Aenderung	JP 03.11.95
|*
|******************************************************************************/

SwSbxValue	SwCalc::Expr()
{
    SwSbxValue left = Term(), right;
    nLastLeft = left;
    for(;;)
        switch(eCurrOper)
        {
            case CALC_PLUS:		GetToken();
                                // erzeuge zum addieren auf jedenfall einen
                                // Double-Wert
                                left.MakeDouble();
                                ( right = Term() ).MakeDouble();
                                left.Compute( SbxPLUS, right );
                                nListPor++;
                                break;

            case CALC_MINUS:	GetToken();
                                // erzeuge zum addieren auf jedenfall einen
                                // Double-Wert
                                left.MakeDouble();
                                ( right = Term() ).MakeDouble();
                                left.Compute( SbxMINUS, right );
                                break;

            default:			return left;
        }
}

//------------------------------------------------------------------------------

String SwCalc::GetColumnName(const String& rName)
{
    xub_StrLen nPos = rName.Search(DB_DELIM);
    if( STRING_NOTFOUND != nPos )
    {
        nPos = rName.Search(DB_DELIM, nPos + 1);

        if( STRING_NOTFOUND != nPos )
            return rName.Copy(nPos + 1);
    }
    return rName;
}

//------------------------------------------------------------------------------

String SwCalc::GetDBName(const String& rName)
{
    xub_StrLen nPos = rName.Search(DB_DELIM);
    if( STRING_NOTFOUND != nPos )
    {
        nPos = rName.Search(DB_DELIM, nPos + 1);

        if( STRING_NOTFOUND != nPos )
            return rName.Copy( 0, nPos );
    }
    SwDBData aData = rDoc.GetDBData();
    String sRet = aData.sDataSource;
    sRet += DB_DELIM;
    sRet += String(aData.sCommand);
    return sRet;
}

//------------------------------------------------------------------------------

/******************************************************************************
 *	Methode		:	BOOL SwCalc::Str2Double( double& )
 *	Beschreibung:
 *	Erstellt	:	OK 07.06.94 12:56
 *	Aenderung	: 	JP 27.10.98
 ******************************************************************************/
BOOL SwCalc::Str2Double( const String& rCommand, xub_StrLen& rCommandPos,
                            double& rVal, const LocaleDataWrapper* pLclData )
{
    const LocaleDataWrapper* pLclD = pLclData;
    if( !pLclD )
        pLclD = &SvtSysLocale().GetLocaleData();

    const xub_Unicode nCurrCmdPos = rCommandPos;
    rtl_math_ConversionStatus eStatus;
    const sal_Unicode* pEnd;
    rVal = rtl_math_uStringToDouble( rCommand.GetBuffer() + rCommandPos,
            rCommand.GetBuffer() + rCommand.Len(),
            pLclD->getNumDecimalSep().GetChar(0),
            pLclD->getNumThousandSep().GetChar(0),
            &eStatus, &pEnd );
    rCommandPos = static_cast<xub_StrLen>(pEnd - rCommand.GetBuffer());

    if( !pLclData && pLclD != &SvtSysLocale().GetLocaleData() )
        delete (LocaleDataWrapper*)pLclD;

    return rtl_math_ConversionStatus_Ok == eStatus && nCurrCmdPos != rCommandPos;
}

BOOL SwCalc::Str2Double( const String& rCommand, xub_StrLen& rCommandPos,
                            double& rVal, SwDoc* pDoc )
{
    const LocaleDataWrapper* pLclD = &SvtSysLocale().GetLocaleData();
    if( pDoc )
    {

        LanguageType eLang = GetDocAppScriptLang( *pDoc );
        if( eLang != SvxLocaleToLanguage( pLclD->getLocale() ) )
            pLclD = new LocaleDataWrapper(
                            ::comphelper::getProcessServiceFactory(),
                            SvxCreateLocale( eLang ) );
    }

    const xub_Unicode nCurrCmdPos = rCommandPos;
    rtl_math_ConversionStatus eStatus;
    const sal_Unicode* pEnd;
    rVal = rtl_math_uStringToDouble( rCommand.GetBuffer() + rCommandPos,
            rCommand.GetBuffer() + rCommand.Len(),
            pLclD->getNumDecimalSep().GetChar(0),
            pLclD->getNumThousandSep().GetChar(0),
            &eStatus, &pEnd );
    rCommandPos = static_cast<xub_StrLen>(pEnd - rCommand.GetBuffer());

    if( pLclD != &SvtSysLocale().GetLocaleData() )
        delete (LocaleDataWrapper*)pLclD;

    return rtl_math_ConversionStatus_Ok == eStatus && nCurrCmdPos != rCommandPos;
}

//------------------------------------------------------------------------------

BOOL SwCalc::IsValidVarName( const String& rStr,
                                    String* pValidName )
{
    BOOL bRet = FALSE;
    using namespace ::com::sun::star::i18n;
    {
        // Parse any token.
        ParseResult aRes = GetAppCharClass().parseAnyToken( rStr, 0,
                                                    coStartFlags, aEmptyStr,
                                                     coContFlags, aEmptyStr );

        if( aRes.TokenType & KParseType::IDENTNAME )
        {
            bRet = aRes.EndPos == rStr.Len();
            if( pValidName )
            {
                xub_StrLen nRealStt = (xub_StrLen)aRes.LeadingWhiteSpace;
                *pValidName = rStr.Copy( nRealStt, static_cast<xub_StrLen>(aRes.EndPos) - nRealStt );
            }
        }
        else if( pValidName )
            pValidName->Erase();
    }
    return bRet;
}

//------------------------------------------------------------------------------

/******************************************************************************
|*
|*	CTOR DTOR der SwHash classes
|*
|*	Ersterstellung		OK 25.06.93 12:20
|*	Letzte Aenderung	OK 25.06.93 12:20
|*
******************************************************************************/

SwHash::SwHash( const String& rStr ) :
    aStr( rStr ),
    pNext( 0 )
{}

SwHash::~SwHash()
{
    if( pNext )
        delete pNext;
}

void DeleteHashTable( SwHash **ppHashTable, USHORT nCount )
{
    for ( USHORT i = 0; i < nCount; ++i )
        delete *(ppHashTable+i);
    delete [] ppHashTable;
}

SwCalcExp::SwCalcExp( const String& rStr, const SwSbxValue& rVal,
                    const SwFieldType* pType )
    : SwHash( rStr ),
    nValue( rVal ),
    pFldType( pType )
{
}


SwSbxValue::~SwSbxValue()
{
}

BOOL SwSbxValue::GetBool() const
{
    return SbxSTRING == GetType() ? 0 != GetString().Len()
                                  : 0 != SbxValue::GetBool();
}

double SwSbxValue::GetDouble() const
{
    double nRet;
    if( SbxSTRING == GetType() )
    {
        xub_StrLen nStt = 0;
        SwCalc::Str2Double( GetString(), nStt, nRet );
    }
    else if (IsBool())
    {
        nRet = 0 != GetBool() ? 1.0 : 0.0;
    }
    else
        nRet = SbxValue::GetDouble();
    return nRet;
}

SwSbxValue& SwSbxValue::MakeDouble()
{
    if( SbxSTRING == GetType() )
        PutDouble( GetDouble() );
    return *this;
}

#ifdef STANDALONE_HASHCALC

// dies ist der Beispielcode zu erzeugen der HashValues im CTOR:

#include <stdio.h>

void main()
{
static sal_Char
    sNType0[] = "false", 	sNType1[] = "true",		sNType2[] = "pi",
    sNType3[] = "e",		sNType4[] = "tables",	sNType5[] = "graf",
    sNType6[] = "ole",		sNType7[] = "page",		sNType8[] = "para",
    sNType9[] = "word",		sNType10[]= "char",
    sNType11[] = "user_company" ,		sNType12[] = "user_firstname" ,
    sNType13[] = "user_lastname" ,		sNType14[] = "user_initials",
    sNType15[] = "user_street" ,        sNType16[] = "user_country" ,
    sNType17[] = "user_zipcode" ,       sNType18[] = "user_city" ,
    sNType19[] = "user_title" ,         sNType20[] = "user_position" ,
    sNType21[] = "user_tel_home",	    sNType22[] = "user_tel_work",
    sNType23[] = "user_fax" ,           sNType24[] = "user_email" ,
    sNType25[] = "user_state",			sNType26[] = "graph"
    ;

static const sal_Char* sNTypeTab[ 27 ] =
{
    sNType0, sNType1, sNType2, sNType3, sNType4, sNType5,
    sNType6, sNType7, sNType8, sNType9, sNType10, sNType11,
    sNType12, sNType13, sNType14, sNType15, sNType16, sNType17,
    sNType18, sNType19, sNType20, sNType21, sNType22, sNType23,
    sNType24, sNType25, sNType26
};

    const unsigned short nTblSize = 47;
    int aArr[ nTblSize ] = { 0 };
    sal_Char ch;

    for( int n = 0; n < 27; ++n )
    {
        unsigned long ii = 0;
        const sal_Char* pp = sNTypeTab[ n ];

        while( *pp )
            ii = ii << 1 ^ *pp++;
        ii %= nTblSize;

        ch = aArr[ ii ] ? 'X' : ' ';
        aArr[ ii ] = 1;
        printf( "%-20s -> %3d [%c]\n", sNTypeTab[ n ], ii, ch );
    }
}

#endif



