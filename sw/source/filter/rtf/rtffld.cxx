/*************************************************************************
 *
 *  $RCSfile: rtffld.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: hr $ $Date: 2003-04-29 15:09:47 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */


#pragma hdrstop

#include <ctype.h>

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _RTFTOKEN_H
#include <svtools/rtftoken.h>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif

#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _SWPARRTF_HXX
#include <swparrtf.hxx>
#endif
#ifndef _TXATBASE_HXX
#include <txatbase.hxx>
#endif
#ifndef _DBFLD_HXX
#include <dbfld.hxx>
#endif
#ifndef _USRFLD_HXX
#include <usrfld.hxx>
#endif
#ifndef _DOCUFLD_HXX
#include <docufld.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif
#ifndef _CHARFMT_HXX
#include <charfmt.hxx>
#endif
#ifndef _fmtruby_HXX
#include <fmtruby.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

// bestimme, ob es sich um ein IMPORT/TOC - Feld handelt.
// return:	0 - weder noch,
//			1 - TOC
//			2 - IMPORT
//			3 - INDEX
enum RTF_FLD_TYPES {
    RTFFLD_UNKNOWN = 0,
    RTFFLD_TOC,
    RTFFLD_IMPORT,
    RTFFLD_INDEX,
    RTFFLD_SYMBOL,
    RTFFLD_PAGE,
    RTFFLD_NUMPAGES,
    RTFFLD_DATE,
    RTFFLD_TIME,
    RTFFLD_DATA,
    RTFFLD_MERGEFLD,
    RTFFLD_HYPERLINK,
    RTFFLD_EQ
};

static RTF_FLD_TYPES _WhichFld( String& rName, String& rNext )
{
    // Strings sind PascalStrings; Laenge steht an 1. Stellen, dadurch wird
    // sich der Aufruf von strlen erspart!!!
    sal_Char __READONLY_DATA sTOC[]=		"\x03""toc";
    sal_Char __READONLY_DATA sIMPORT[]= 	"\x06""import";
    sal_Char __READONLY_DATA sINDEX[]=		"\x05""index";
    sal_Char __READONLY_DATA sSYMBOL[]= 	"\x06""symbol";
    sal_Char __READONLY_DATA sPAGE[]=		"\x04""page";
    sal_Char __READONLY_DATA sNUMPAGES[]=   "\x08""numpages";
    sal_Char __READONLY_DATA sDATE[]=		"\x04""date";
    sal_Char __READONLY_DATA sTIME[]=		"\x04""time";
    sal_Char __READONLY_DATA sDATA[]=		"\x04""data";
    sal_Char __READONLY_DATA sMERGEFLD[]=	"\x0A""mergefield";
    sal_Char __READONLY_DATA sIMPORT2[]=	"\x0E""includepicture";
    sal_Char __READONLY_DATA sHYPERLINK[]=	"\x09""hyperlink";
    sal_Char __READONLY_DATA sEQ[]=			"\x02""eq";

    struct _Dummy_RTF_FLD_TYPES
    {
        RTF_FLD_TYPES eFldType;
        const sal_Char* pFldNm;
    };
    __READONLY_DATA _Dummy_RTF_FLD_TYPES aFldNmArr[ RTFFLD_EQ+1 ] = {
            {RTFFLD_TOC,         sTOC},
            {RTFFLD_IMPORT,      sIMPORT},
            {RTFFLD_INDEX,       sINDEX},
            {RTFFLD_SYMBOL,      sSYMBOL},
            {RTFFLD_PAGE,        sPAGE},
            {RTFFLD_NUMPAGES,    sNUMPAGES},
            {RTFFLD_DATE,        sDATE},
            {RTFFLD_TIME,        sTIME},
            {RTFFLD_DATA,        sDATA},
            {RTFFLD_MERGEFLD,    sMERGEFLD},
            {RTFFLD_IMPORT,      sIMPORT2},
            {RTFFLD_HYPERLINK, 	 sHYPERLINK},
            {RTFFLD_EQ, 		 sEQ}
    };


    if( !rName.Len() )
        return RTFFLD_UNKNOWN;

    String sNm( rName );
    sNm = sNm.EraseLeadingChars().GetToken(0, ' ');
    ASSERT( sNm.Len(), "Feldname hat keine Laenge!" );
    if( !sNm.Len() )
        return RTFFLD_UNKNOWN;

    xub_StrLen nTokenStt = rName.Search( sNm );
    sNm.ToLowerAscii();

    for( int n = 0; n < sizeof( aFldNmArr ) / sizeof( aFldNmArr[0]); ++n )
    {
        const sal_Char* pCmp = aFldNmArr[n].pFldNm;
        int nLen = *pCmp++;
        xub_StrLen nFndPos = sNm.SearchAscii( pCmp );
        if( STRING_NOTFOUND != nFndPos &&
            ( !nFndPos || !isalpha(sNm.GetChar( nFndPos-1 )) ) &&
            ( nFndPos+nLen == sNm.Len() || !isalpha(sNm.GetChar(nFndPos+nLen) ) ) )
        {
//			rName = sNm.Copy( nFndPos, nLen );
            rName = rName.Copy( nFndPos, nLen );
            nFndPos += nTokenStt + nLen;
            while( rNext.GetChar( nFndPos ) == ' ' )    ++nFndPos;
            rNext.Erase( 0, nFndPos );
            rNext.EraseTrailingChars();
            return aFldNmArr[n].eFldType;
        }
    }
    return RTFFLD_UNKNOWN;		// nichts gefunden.
}

static USHORT CheckNumberFmtStr( const String& rNStr )
{
    const static sal_Char* aNumberTypeTab[] =
    {
        "\x0A""ALPHABETIC",       /* CHARS_UPPER_LETTER*/
        "\x0A""alphabetic",       /* CHARS_LOWER_LETTER*/
        "\x05""ROMAN",            /* ROMAN_UPPER       */
        "\x05""roman",            /* ROMAN_LOWER       */
        "\x06""ARABIC",           /* ARABIC            */
        "\x04""NONE",             /* NUMBER_NONE       */
        "\x04""CHAR",             /* CHAR_SPECIAL      */
        "\x04""PAGE"              /* PAGEDESC          */
    };

    ASSERT(sizeof(aNumberTypeTab) / sizeof(sal_Char *) 
           >= SVX_NUM_PAGEDESC - SVX_NUM_CHARS_UPPER_LETTER, "impossible");

    for (USHORT n = SVX_NUM_CHARS_UPPER_LETTER;  n <= SVX_NUM_PAGEDESC; ++n)
    {
        const sal_Char* pCmp = aNumberTypeTab[n - SVX_NUM_CHARS_UPPER_LETTER];
        int nLen = *pCmp++;
        if( rNStr.EqualsAscii( pCmp, 0, nLen ))
            return 2 <= n ? n : (n + SVX_NUM_CHARS_UPPER_LETTER_N);
    }
    return SVX_NUM_PAGEDESC;		// default-Wert
}

extern void sw3io_ConvertFromOldField( SwDoc& rDoc, USHORT& rWhich,
                                        USHORT& rSubType, ULONG &rFmt,
                                        USHORT nVersion );

class RtfFieldSwitch
{
    String sParam;
    xub_StrLen nCurPos;
public:
    RtfFieldSwitch( const String& rParam );
    sal_Unicode GetSwitch( String& rParam );

    BOOL IsAtEnd() const 				{ return nCurPos >= sParam.Len(); }
    xub_StrLen GetCurPos() const 		{ return nCurPos; }
    void Erase( xub_StrLen nEndPos ) 	{ sParam.Erase( 0, nEndPos ); }
    void Insert( const String& rIns ) 	{ sParam.Insert( rIns, 0 ); }
    const String& GetStr() const		{ return sParam; }
};

RtfFieldSwitch::RtfFieldSwitch( const String& rParam )
    : sParam( rParam ), nCurPos( 0  )
{
    sParam.EraseTrailingChars().EraseLeadingChars();
}

sal_Unicode RtfFieldSwitch::GetSwitch( String& rParam )
{
    // beginnt ein Schalter?
    sal_Unicode c, cKey = 0;
    if( '\\' == (c = sParam.GetChar( nCurPos )) )
    {
        if( '\\' == ( c = sParam.GetChar( ++nCurPos )) )
            c = sParam.GetChar( ++nCurPos );

        cKey = c;

        while( ++nCurPos < sParam.Len() &&
                ' ' == ( c = sParam.GetChar( nCurPos )) )
            ;
    }

    // dann alles in Hochkommatas oder bis zum naechsten // als
    // Param returnen
    USHORT nOffset;
    if( '"' != c && '\'' != c )
        c = '\\', nOffset = 0;
    else
        nOffset = 1;

    sParam.Erase( 0, nCurPos + nOffset );
    rParam = sParam.GetToken( 0, c );
    sParam.Erase( 0, rParam.Len() + nOffset ).EraseLeadingChars();
    if( '\\' == c )
        rParam.EraseTrailingChars();
    nCurPos = 0;

    return cKey;
}

struct RTF_EquationData
{
    String sFontName, sUp, sDown, sText;
    sal_Int32 nJustificationCode, nFontSize, nUp, nDown, nStyleNo;

    inline RTF_EquationData()
        : nJustificationCode(0), nFontSize(0), nUp(0), nDown(0),
        nStyleNo( -1 )
    {}
};

xub_StrLen lcl_FindEndBracket( const String& rStr )
{
    xub_StrLen nEnd = rStr.Len(), nRet = STRING_NOTFOUND, nPos = 0;
    int nOpenCnt = 1;
    sal_Unicode cCh;
    for( ; nPos < nEnd; ++nPos )
        if( ')' == (cCh = rStr.GetChar( nPos )) && !--nOpenCnt )
        {
            nRet = nPos;
            break;
        }
        else if( '(' == cCh )
            ++nOpenCnt;

    return nRet;
}

void lcl_ScanEquationField( const String& rStr, RTF_EquationData& rData,
                            sal_Unicode nSttKey )
{
    int nSubSupFlag(0);
    RtfFieldSwitch aRFS( rStr );
    while( !aRFS.IsAtEnd() )
    {
        String sParam;
        sal_Unicode cKey = aRFS.GetSwitch( sParam );
        if( 1 == nSubSupFlag )
            ++nSubSupFlag;
        else if( 1 < nSubSupFlag )
            nSubSupFlag = 0;

        BOOL bCheckBracket = FALSE;
        switch( cKey )
        {
        case 0:
            switch( nSttKey )
            {
            case 'u':	rData.sUp += sParam;	break;
            case 'd':	rData.sDown += sParam;	break;
            default:	rData.sText += sParam;	break;
            }
            break;

        case '*':
            if( sParam.Len() )
            {
                if( sParam.EqualsIgnoreCaseAscii( "jc", 0, 2 ) )
                    rData.nJustificationCode = sParam.Copy( 2 ).ToInt32();
                else if( sParam.EqualsIgnoreCaseAscii( "hps", 0, 3 ) )
                    rData.nFontSize= sParam.Copy( 3 ).ToInt32();
                else if( sParam.EqualsIgnoreCaseAscii( "Font:", 0, 5 ) )
                    rData.sFontName = sParam.Copy( 5 );
                else if( sParam.EqualsIgnoreCaseAscii( "cs", 0, 2 ) )
                    rData.nStyleNo = sParam.Copy( 2 ).ToInt32();
            }
            break;
        case 's' :
            ++nSubSupFlag;
            break;

        case 'u':
            if( sParam.Len() && 'p' == sParam.GetChar( 0 ) &&
                2 == nSubSupFlag )
            {
                rData.nUp = sParam.Copy( 1 ).ToInt32();
                bCheckBracket = TRUE;
            }
            break;

        case 'd':
            if( sParam.Len() && 'o' == sParam.GetChar( 0 ) &&
                2 == nSubSupFlag )
            {
                rData.nDown = sParam.Copy( 1 ).ToInt32();
                bCheckBracket = TRUE;
            }
            break;

        default:
            bCheckBracket = TRUE;
            cKey = 0;
            break;
        }

        if( bCheckBracket && sParam.Len() )
        {
            xub_StrLen nEnd, nStt = sParam.Search( '(' ),
                        nLen = sParam.Len();
            if( STRING_NOTFOUND != nStt )
            {
                sParam.Erase( 0, nStt + 1 ) += aRFS.GetStr();
                if( STRING_NOTFOUND !=
                        (nEnd = ::lcl_FindEndBracket( sParam )) )
                {
                    // end in the added string?
                    if( (nLen - nStt - 1 ) < nEnd )
                        aRFS.Erase( nEnd + 1 - (nLen - nStt - 1));
                    else
                    {
                        // not all handled here, so set new into the RFS
                        aRFS.Insert( sParam.Copy( nEnd + 1,
                                                nLen - nStt - nEnd - 2 ));
                        sal_Unicode cCh;
                        if( aRFS.GetStr().Len() &&
                            ( ',' == (cCh = aRFS.GetStr().GetChar(0)) ||
                              ';' == cCh ))
                            aRFS.Erase( 1 );
                    }

                    ::lcl_ScanEquationField( sParam.Copy( 0, nEnd ),
                                                rData, cKey );
                }
            }
        }
    }
}

int SwRTFParser::MakeFieldInst( String& rFieldStr )
{
    // sicher den Original-String fuer die FeldNamen (User/Datenbank)
    String aSaveStr( rFieldStr );
    SwFieldType * pFldType;
    USHORT nSubType;
    int nRet = _WhichFld(rFieldStr, aSaveStr);

    //Strip Mergeformat from fields
    xub_StrLen nPos=0;
    while (STRING_NOTFOUND != ( nPos = aSaveStr.SearchAscii("\\*", nPos)))
    {
        xub_StrLen nStartDel = nPos;
        nPos += 2;
        while (aSaveStr.GetChar(nPos) == ' ') 
            ++nPos;
        if (aSaveStr.EqualsIgnoreCaseAscii("MERGEFORMAT", nPos, 11))
        {
            xub_StrLen nNoDel = (nPos + 11 ) - nStartDel;
            aSaveStr.Erase(nStartDel, nNoDel);
            nPos -= (nStartDel - nPos);
        }
    }

    nPos = 0;
    switch (nRet)
    {
    case RTFFLD_IMPORT:
        {
//JP 11.03.96: vertraegt sich nicht so ganz mit Internet!
//			  if( STRING_NOTFOUND != ( nPos = aSaveStr.Search( '.' )))
//				  aSaveStr.Erase( nPos+4 );

            aSaveStr.EraseLeadingAndTrailingChars();
            if( aSaveStr.Len() )
            {
                sal_Unicode c = aSaveStr.GetChar( 0 );
                if( '"' == c || '\'' == c )
                {
                    aSaveStr.Erase( 0, 1 );
                    aSaveStr = aSaveStr.GetToken( 0, c );
                }

                rFieldStr = URIHelper::SmartRelToAbs( aSaveStr );
            }
//			SkipGroup();		// ueberlese den Rest
        }
        break;

    case RTFFLD_NUMPAGES:
        {
            SwDocStatField aFld( (SwDocStatFieldType*)pDoc->GetSysFldType( RES_DOCSTATFLD ), 
                                  DS_PAGE, SVX_NUM_ARABIC );
            if( STRING_NOTFOUND != ( nPos = aSaveStr.SearchAscii( "\\*" )) )
            {
                nPos += 2;
                while( aSaveStr.GetChar(nPos) == ' ' ) nPos++;
                aSaveStr.Erase( 0, nPos );

                // steht jetzt geanu auf dem Format-Namen
                aFld.ChangeFormat( CheckNumberFmtStr( aSaveStr ));
            }
            pDoc->Insert( *pPam, SwFmtFld( aFld ) );
            SkipGroup();
        }
        break;

    case RTFFLD_PAGE:
        {
            pFldType = pDoc->GetSysFldType( RES_PAGENUMBERFLD );
            SwPageNumberField aPF( (SwPageNumberFieldType*)pFldType,
                                    PG_RANDOM, SVX_NUM_ARABIC);
            if( STRING_NOTFOUND != ( nPos = aSaveStr.SearchAscii( "\\*" )) )
            {
                nPos += 2;
                while( aSaveStr.GetChar(nPos) == ' ' ) nPos++;
                aSaveStr.Erase( 0, nPos );

                // steht jetzt geanu auf dem Format-Namen
                aPF.ChangeFormat( CheckNumberFmtStr( aSaveStr ));
            }
            pDoc->Insert( *pPam, SwFmtFld( aPF ) );
            SkipGroup();		// ueberlese den Rest
        }
        break;
    case RTFFLD_DATE:
    case RTFFLD_TIME:
        {
            if( STRING_NOTFOUND == ( nPos = aSaveStr.SearchAscii( "\\@" )) )
            {
                // es fehlt die Format - Angabe: defaulten auf Datum
                pFldType = pDoc->GetSysFldType( RES_DATETIMEFLD );
                pDoc->Insert( *pPam, SwFmtFld( SwDateTimeField(
                                (SwDateTimeFieldType*)pFldType, DATEFLD )));
            }
            else
            {
                // versuche aus dem Formatstring zu erkennen, ob es ein
                // Datum oder Zeit oder Datum & Zeit Field ist
                // nur das Format interressiert
                aSaveStr.Erase( 0, aSaveStr.Search( '\"' )+1 );
                // alles hinter dem Format interressiert auch nicht mehr.
                aSaveStr.Erase( aSaveStr.Search( '\"' ) );
                aSaveStr.SearchAndReplaceAscii( "AM", aEmptyStr );
                aSaveStr.SearchAndReplaceAscii( "PM", aEmptyStr );

                xub_StrLen nDPos = aSaveStr.Search( 'M' ),	// M    -> Datum
                           nTPos = aSaveStr.Search( 'H' );	// H    -> 24h
                if( STRING_NOTFOUND == nTPos )
                    nTPos = aSaveStr.Search( 'h' );			// h    -> 12h

                SwField *pTFld = 0, *pDFld = 0;

                if( STRING_NOTFOUND != nTPos )
                {
                    ULONG nFormat = 0;
                    SvNumberFormatter* pFormatter = pDoc->GetNumberFormatter();
                    nFormat	= pFormatter->GetFormatIndex(
                        NF_TIME_START, LANGUAGE_SYSTEM );
                     pFldType = pDoc->GetSysFldType( RES_DATETIMEFLD );
                    pTFld = new SwDateTimeField( (SwDateTimeFieldType*)pFldType, TIMEFLD );
                }

                if( STRING_NOTFOUND != nDPos ) 		// Datum ?
                {
                    static SwDateFormat aDateA[16] = {
                        DF_SHORT, DF_LMON, DF_LDAYMON, DF_LDAYMON,
                        DF_SCENT, DF_LDAYMON, DF_LDAYMON, DF_LDAYMON,
                        DF_SHORT, DF_LMON, DF_LDAYMONTH, DF_LDAYMONTH,
                        DF_SCENT, DF_LDAYMONTH, DF_LDAYMONTH, DF_LDAYMONTH
                    };
                                //	t, tt, T, TT -> no day of week
                                //	ttt, tttt, TTT, TTTT -> day of week
                    BOOL bDayOfWeek = STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "ttt" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "TTT" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "ddd" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "DDD" );
                                //	M, MM -> numeric month
                                //	MMM, MMMM -> text. month
                    BOOL bLitMonth = STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "MMM" );
                                //	MMMM -> full month
                    BOOL bFullMonth = STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "MMMM" );
                                //	jj, JJ -> 2-col-year
                                //	jjjj, JJJJ -> 4-col-year
                    BOOL bFullYear = STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "jjj" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "JJJ" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "yyy" ) ||
                                      STRING_NOTFOUND !=
                                            aSaveStr.SearchAscii( "YYY" );

                    USHORT i =	( bLitMonth & 1 ) |
                                ( ( bDayOfWeek & 1 ) << 1 ) |
                                ( ( bFullYear & 1 ) << 2 ) |
                                ( ( bFullMonth & 1 ) << 3 );

                    pFldType = pDoc->GetSysFldType( RES_DATETIMEFLD );

                    nSubType = DATEFLD;
                    USHORT nWhich = RES_DATEFLD;
                    ULONG nFormat = aDateA[ i ];
                    sw3io_ConvertFromOldField( *pDoc, nWhich, nSubType, nFormat, 0x0110 );
                    pDFld = new SwDateTimeField( (SwDateTimeFieldType*)pFldType, DATEFLD, nFormat );
                }

                // Zeit kommt vor Date, alos Feld-Pointer swappen
                if( nTPos < nDPos )
                {
                    SwField* pTmp = pTFld;
                    pTFld = pDFld;
                    pDFld = pTmp;
                }

                if( pDFld )
                {
                    pDoc->Insert( *pPam, SwFmtFld( *pDFld ));
                    delete pDFld;
                }
                if( pTFld )
                {
                    pDoc->Insert( *pPam, SwFmtFld( *pTFld ));
                    delete pTFld;
                }
            }
            SkipGroup();		// ueberlese den Rest
        }
        break;

    case RTFFLD_DATA:
        {
            // Datenbank-FileName: nur der Filename interressiert
            // Zur Zeit werden nur SDF-Files verarbeitet, also suche nach
            // der Extension

            // im SWG geben die DATA Felder den Namen der Datenbank
            // an. Dieser kann als Field oder als DBInfo interpretiert
            // werden:
            //	\\data -> Datenbank-Name als Field
            //	DATA -> Datenbank-Info
            BOOL bField = rFieldStr.GetChar( 0 ) != 'D';

            // nur der Name interressiert
            if( STRING_NOTFOUND != (nPos = aSaveStr.Search( '.' )) )
                aSaveStr.Erase( nPos );
            SwDBData aData;
            aData.sDataSource = aSaveStr;
            if( bField )
            {
                pFldType = pDoc->GetSysFldType( RES_DBNAMEFLD );
                pDoc->Insert( *pPam, SwFmtFld( SwDBNameField(
                                (SwDBNameFieldType*)pFldType, SwDBData() ) ));
            }
            else
                pDoc->ChgDBData( aData );		// MS: Keine DBInfo verwenden
            SkipGroup();		// ueberlese den Rest
        }
        break;
    case RTFFLD_MERGEFLD:
        {
            // ein Datenbank - Feld: nur der Name interressiert
            // bis zum Ende vom String ist das der Feldname
            SwDBFieldType aTmp( pDoc, aSaveStr, SwDBData() );	//
            SwDBField aDBFld( (SwDBFieldType*)pDoc->InsertFldType( aTmp ));

            aDBFld.ChangeFormat( UF_STRING );
            pDoc->Insert( *pPam, SwFmtFld( aDBFld ));
            SkipGroup();		// ueberlese den Rest
        }
        break;

    case RTFFLD_SYMBOL:
        {
            // loesche fuehrende Blanks
            if( IsNewGroup() )
                GetAttrSet();
            SetNewGroup( TRUE );

            SfxItemSet& rSet = GetAttrSet();

            BOOL bCharIns = FALSE;
            RtfFieldSwitch aRFS( aSaveStr );
            while( !aRFS.IsAtEnd() )
            {
                String sParam;
                sal_Unicode cKey = aRFS.GetSwitch( sParam );
                if( sParam.Len() )
                    switch( cKey )
                    {
                    case 0:
                        if( !bCharIns )
                        {
                            sal_Unicode nChar = (sal_Unicode)sParam.ToInt32();
                            if( nChar )
                            {
                                pDoc->Insert( *pPam, nChar );
                                bCharIns = TRUE;
                            }
                        }
                        break;

                    case 'f': case 'F':
                        // Font setzen
                        {
                            SvxRTFFontTbl& rTbl = GetFontTbl();
                            for( Font* pFont = rTbl.First(); pFont;
                                    pFont = rTbl.Next() )
                                if( pFont->GetName() == sParam )
                                {
                                    rSet.Put( SvxFontItem(
                                            pFont->GetFamily(),
                                            sParam,
                                            pFont->GetStyleName(),
                                            pFont->GetPitch(),
                                            pFont->GetCharSet(),
                                            RES_CHRATR_FONT ));
                                    break;
                                }
                        }
                        break;
                    case 'h': case 'H':
                        //??
                        break;
                    case 's': case 'S':
                        // Fontsize setzen
                        {
                            const USHORT nVal = (USHORT)(sParam.ToInt32() * 20);
                            rSet.Put( SvxFontHeightItem( nVal,
                                                100, RES_CHRATR_FONTSIZE ));
                        }
                        break;
                    }
            }

            if( !IsNewGroup() ) AttrGroupEnd();
            SetNewGroup( FALSE );

            SkipGroup();		// ueberlese den Rest
        }
        break;

    case RTFFLD_HYPERLINK:
        rFieldStr.Erase();
        if( aSaveStr.Len() )
        {
            // return String ist URL, # Mark, \1 Frame
            String sMark, sFrame;
            RtfFieldSwitch aRFS( aSaveStr );
            while( !aRFS.IsAtEnd() )
            {
                String sParam;
                sal_Unicode cKey = aRFS.GetSwitch( sParam );
                if( sParam.Len() )
                    switch( cKey )
                    {
                    case 0:
                        if( !rFieldStr.Len() )
                            rFieldStr = URIHelper::SmartRelToAbs( sParam );
                        break;

                    case 'l':	case 'L':	sMark = sParam;		break;
                    case 't':	case 'T':	sFrame = sParam;	break;
                    }
            }

            if( sMark.Len() )
                ( rFieldStr += INET_MARK_TOKEN ) += sMark;
            if( sFrame.Len() )
                ( rFieldStr += '\1' ) += sFrame;
        }
        break;

    case RTFFLD_EQ:
        rFieldStr.Erase();
        if( aSaveStr.Len() )
        {
            RTF_EquationData aData;
            ::lcl_ScanEquationField( aSaveStr, aData, 0 );

            // is it a ruby attr?
            if( aData.sText.Len() && aData.sFontName.Len() &&
                aData.nFontSize && aData.sUp.Len() && !aData.sDown.Len() )
            {
                //Translate and apply
                switch( aData.nJustificationCode )
                {
                case 0:		aData.nJustificationCode = 1; 	break;
                case 1:		aData.nJustificationCode = 3;	break;
                case 2:		aData.nJustificationCode = 4;	break;
                case 4:		aData.nJustificationCode = 2;	break;
//				case 3:
                default:	aData.nJustificationCode = 0;	break;
                }

                SwFmtRuby aRuby( aData.sUp );
                SwCharFmt * pCharFmt = -1 != aData.nStyleNo
                                          ? aCharFmtTbl.Get( aData.nStyleNo )
                                          : 0;

                if( !pCharFmt )
                {
                    //Make a guess at which of asian of western we should be setting
                    USHORT nScript;
                    if (pBreakIt->xBreak.is())
                        nScript = pBreakIt->xBreak->getScriptType( aData.sUp, 0);
                    else
                        nScript = com::sun::star::i18n::ScriptType::ASIAN;

                    USHORT nFntHWhich = GetWhichOfScript( RES_CHRATR_FONTSIZE, nScript ),
                           nFntWhich = GetWhichOfScript( RES_CHRATR_FONT, nScript );

                    //Check to see if we already have a ruby charstyle that this fits
                    for(USHORT i=0; i < aRubyCharFmts.Count(); ++i )
                    {
                        SwCharFmt *pFmt = (SwCharFmt *)aRubyCharFmts[i];
                        const SvxFontHeightItem &rF = (const SvxFontHeightItem &)
                                                    pFmt->GetAttr( nFntHWhich );
                        if( rF.GetHeight() == USHORT(aData.nFontSize * 10 ))
                        {
                            const SvxFontItem &rF = (const SvxFontItem &)
                                                    pFmt->GetAttr( nFntWhich );
                            if( rF.GetFamilyName().Equals( aData.sFontName ))
                            {
                                pCharFmt = pFmt;
                                break;
                            }
                        }
                    }

                    //Create a new char style if necessary
                    if( !pCharFmt )
                    {
                        String sNm;
                        //Take this as the base name
                        SwStyleNameMapper::FillUIName( RES_POOLCHR_RUBYTEXT, sNm );
                        sNm += String::CreateFromInt32( aRubyCharFmts.Count() + 1 );
                        pCharFmt = pDoc->MakeCharFmt( sNm,
                                            ( SwCharFmt*)pDoc->GetDfltCharFmt() );

                        SvxFontHeightItem aHeightItem( aData.nFontSize * 10 );
                        aHeightItem.SetWhich( nFntHWhich );

                        SvxFontItem aFontItem( FAMILY_DONTKNOW, aData.sFontName,
                            aEmptyStr, PITCH_DONTKNOW, RTL_TEXTENCODING_DONTKNOW );
                        aFontItem.SetWhich( nFntWhich );

                        pCharFmt->SetAttr( aHeightItem );
                        pCharFmt->SetAttr( aFontItem );
                        void* p = pCharFmt;
                        aRubyCharFmts.Insert( p, aRubyCharFmts.Count() );
                    }
                }

                //Set the charstyle and justification
                aRuby.SetCharFmtName( pCharFmt->GetName() );
                aRuby.SetCharFmtId( pCharFmt->GetPoolFmtId() );
                aRuby.SetAdjustment( (USHORT)aData.nJustificationCode );

                // im FieldStr steht der anzuzeigenden Text, im
                pDoc->Insert( *pPam, aData.sText );
                pPam->SetMark();
                pPam->GetMark()->nContent -= aData.sText.Len();
                pDoc->Insert( *pPam, aRuby, SETATTR_DONTEXPAND );
                pPam->DeleteMark();
            }
            // or a combined character field?
            else if( aData.sUp.Len() && aData.sDown.Len() &&
                    !aData.sText.Len() && !aData.sFontName.Len() &&
                    !aData.nFontSize )
            {
                String sFld( aData.sUp );
                sFld += aData.sDown;
                SwCombinedCharField aFld((SwCombinedCharFieldType*)pDoc->
                                GetSysFldType( RES_COMBINED_CHARS ), sFld );
                pDoc->Insert( *pPam, SwFmtFld( aFld ));

            }
            SkipGroup();		// ueberlese den Rest
        }
        break;

    case RTFFLD_TOC:
    case RTFFLD_INDEX:
        break;

    default:
        {
            // keines von den bekannten Feldern, also eine neues UserField
            aSaveStr.EraseLeadingChars().EraseTrailingChars();
            SwUserFieldType aTmp( pDoc, aSaveStr );
            SwUserField aUFld( (SwUserFieldType*)pDoc->InsertFldType( aTmp ));
            aUFld.ChangeFormat( UF_STRING );
            pDoc->Insert( *pPam, SwFmtFld( aUFld ));
            nRet = RTFFLD_UNKNOWN;
        }
        break;
    }
    return nRet;
}

void SwRTFParser::ReadField()
{
    int nRet = 0;
    int nOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!
    int bFldInst = FALSE, bFldRslt = FALSE;
    String sFieldStr, sFieldNm;
    BYTE cCh;

    int nToken;
    while (nOpenBrakets && IsParserWorking())
    {
        switch (nToken = GetNextToken())
        {
        case '}':
            {
                --nOpenBrakets;
                if( 1 != nOpenBrakets || !bFldInst )
                    break;

                if( !bFldRslt )
                {
                    // FieldInst vollstaendig eingelesen, was ist es denn?
                    nRet = MakeFieldInst( sFieldStr );
                    switch ( nRet )
                    {
                    case RTFFLD_TOC:
                    case RTFFLD_INDEX:
                        // erstmal Index/Inhaltsverzeichniss ueberspringen
                        // und als normalen Text einfuegen. Spaeter mal auch dem
                        // SwPaM darum aufspannen.
                        return ;

                    case RTFFLD_IMPORT:
                    case RTFFLD_HYPERLINK:
                        sFieldNm = sFieldStr;
                        break;
                    }
                    sFieldStr.Erase();
                }
                else if (RTFFLD_UNKNOWN == nRet) 
                {
                    // FieldResult wurde eingelesen
                    SwTxtNode* pTxtNd = pPam->GetPoint()->nNode.GetNode().GetTxtNode();
                    SwTxtAttr* pFldAttr = pTxtNd->GetTxtAttr(
                            pPam->GetPoint()->nContent.GetIndex()-1 );

                    ((SwUserFieldType*)pFldAttr->GetFld().GetFld()->GetTyp())->
                                SetContent( sFieldStr );
                }
                else if( sFieldNm.Len() )
                {
                    switch ( nRet )
                    {
                    case RTFFLD_IMPORT:
                        // Grafik einfuegen
                        InsPicture( sFieldNm );
                        nRet = INT_MAX;
                        break;
                    case RTFFLD_HYPERLINK:
                        if( sFieldStr.Len() )
                        {
                            // im FieldStr steht der anzuzeigenden Text, im
                            pDoc->Insert( *pPam, sFieldStr );

                            String sTarget( sFieldNm.GetToken( 1, '\1' ));
                            if( sTarget.Len() )
                                sFieldNm.Erase( sFieldNm.Len() - sTarget.Len() -1 );

                            // oder ueber den Stack setzen??
                            pPam->SetMark();
                            pPam->GetMark()->nContent -= sFieldStr.Len();
                            pDoc->Insert( *pPam,
                                            SwFmtINetFmt( sFieldNm, sTarget ),
                                            SETATTR_DONTEXPAND );
                            pPam->DeleteMark();
                        }
                        break;
                    }
                }
            }
            break;

        case '{':
            if( RTF_IGNOREFLAG != GetNextToken() )
                SkipToken( -1 );
            // Unknown und alle bekannten nicht ausgewerteten Gruppen
            // sofort ueberspringen
            else if( RTF_UNKNOWNCONTROL != GetNextToken() )
                SkipToken( -2 );
            else
            {
                // gleich herausfiltern
                ReadUnknownData();
                if( '}' != GetNextToken() )
                    eState = SVPAR_ERROR;
                break;
            }
            ++nOpenBrakets;
            break;

        case RTF_DATAFIELD:
        case RTF_FIELD:
            SkipGroup();
            break;

        case RTF_FLDINST:
            bFldInst = TRUE;
            break;

        case RTF_FLDRSLT:
            bFldRslt = TRUE;
            break;

        case RTF_U:
            {
                if( nTokenValue )
                    sFieldStr += (sal_Unicode)nTokenValue;
                else
                    sFieldStr += aToken;
            }
            break;

        case RTF_LINE:			cCh = '\n';	goto INSINGLECHAR;
        case RTF_TAB:			cCh = '\t';	goto INSINGLECHAR;
        case RTF_SUBENTRYINDEX:	cCh = ':';	goto INSINGLECHAR;
        case RTF_EMDASH:		cCh = 151;	goto INSINGLECHAR;
        case RTF_ENDASH:		cCh = 150;	goto INSINGLECHAR;
        case RTF_BULLET:		cCh = 149;	goto INSINGLECHAR;
        case RTF_LQUOTE:		cCh = 145;	goto INSINGLECHAR;
        case RTF_RQUOTE:		cCh = 146;	goto INSINGLECHAR;
        case RTF_LDBLQUOTE:		cCh = 147;	goto INSINGLECHAR;
        case RTF_RDBLQUOTE:		cCh = 148;	goto INSINGLECHAR;
INSINGLECHAR:
            sFieldStr += ByteString::ConvertToUnicode( cCh,
                                               RTL_TEXTENCODING_MS_1252 );
            break;

        // kein Break, aToken wird als Text gesetzt
        case RTF_TEXTTOKEN:
            sFieldStr += aToken;
            break;

        case RTF_PICT:		// Pic-Daten einlesen!
            if( RTFFLD_IMPORT == nRet )
            {
                Graphic aGrf;
                SvxRTFPictureType aPicType;
                if( ReadBmpData( aGrf, aPicType ) )
                {
                    InsPicture( sFieldNm, &aGrf, &aPicType );
                    nRet = INT_MAX;
                }
                SkipGroup();
            }
            break;

        case RTF_BKMKSTART:
        case RTF_BKMKEND:
        case RTF_BKMK_KEY:
        case RTF_XE:
        case RTF_TC:
        case RTF_NEXTFILE:
        case RTF_TEMPLATE:
        case RTF_SHPRSLT:
            SkipGroup();
            break;

        case RTF_CS:
            // we write every time "EQ "
            if( bFldInst && 0 == sFieldStr.SearchAscii( "EQ " ))
            {
                // insert behind the EQ the "\*cs<NO> " string. This is utilize
                // in the MakeFieldInst
                String sTmp;
                (sTmp.AssignAscii( "\\* cs" )
                    += String::CreateFromInt32( nTokenValue )) += ' ';
                sFieldStr.Insert( sTmp, 3 );
            }
            break;
        default:
            SvxRTFParser::NextToken( nToken );
            break;
        }
    }

    // Grafik einfuegen
    if (RTFFLD_IMPORT == nRet && sFieldNm.Len())
        InsPicture( sFieldNm );

    SkipToken( -1 );		// die schliesende Klammer wird "oben" ausgewertet
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
