/*************************************************************************
 *
 *  $RCSfile: ww8par5.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: cmc $ $Date: 2002-08-19 15:12:02 $
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

#include <ctype.h>				// tolower
#include <stdio.h>				// sscanf()

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif
#ifndef _ZFORMAT_HXX
#include <svtools/zformat.hxx>
#endif

#ifndef _LINKMGR_HXX //autogen
#include <so3/linkmgr.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX //autogen
#include <svx/langitem.hxx>
#endif

#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _FMTHBSH_HXX //autogen
#include <fmthbsh.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>              // fuer SwPam
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _CHARATR_HXX
#include <charatr.hxx>			// class SwFmtFld
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>			// class SwDateTimeField
#endif
#ifndef _DOCUFLD_HXX
#include <docufld.hxx>			// class SwPageNumberField
#endif
#ifndef _REFFLD_HXX
#include <reffld.hxx>			// class SwGetRefField
#endif
#ifndef _BOOKMRK_HXX			// class SwBookmark
#include <bookmrk.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>			// class SwSetExpField
#endif
#ifndef _DBFLD_HXX
#include <dbfld.hxx>			// class SwDBField
#endif
#ifndef _TOX_HXX
#include <tox.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>			// class SwSection
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FMTINFMT_HXX
#include <fmtinfmt.hxx>
#endif
#ifndef _CHPFLD_HXX
#include <chpfld.hxx>
#endif
#ifndef _FTNIDX_HXX
#include <ftnidx.hxx>
#endif
#ifndef _TXTFTN_HXX
#include <txtftn.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _SHELLRES_HXX
#include <shellres.hxx>
#endif
#ifndef _SWDBDATA_HXX
#include <swdbdata.hxx>
#endif
#ifndef _FMTRUBY_HXX
#include <fmtruby.hxx>
#endif
#ifndef _CHARFMT_HXX
#include <charfmt.hxx>
#endif
#ifndef _TXTATR_HXX
#include <txtatr.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _FMTCLDS_HXX
#include <fmtclds.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

#ifndef _WW8SCAN_HXX
#include "ww8scan.hxx"			// WW8FieldDesc
#endif
#ifndef _WW8PAR_HXX
#include "ww8par.hxx"
#endif

#define WWF_INVISIBLE 86			// Bit-Nummer fuer Invisible ( IniFlags )
#define MAX_FIELDLEN 64000

#define WW8_TOX_LEVEL_DELIM 	':'

class _ReadFieldParams
{
private:
    String aData;
    xub_StrLen nLen, nFnd, nNext, nSavPtr;
public:
    _ReadFieldParams( const String& rData );
    ~_ReadFieldParams();

    xub_StrLen GoToTokenParam();
    long SkipToNextToken();
    xub_StrLen GetTokenSttPtr() const 	{ return nFnd;  }
    void SetNextPtr( xub_StrLen _nNext ) { nNext = _nNext; }

    xub_StrLen FindNextStringPiece( xub_StrLen _nStart = STRING_NOTFOUND );
    bool GetTokenSttFromTo(xub_StrLen* _pFrom, xub_StrLen* _pTo,
        xub_StrLen _nMax);

    String GetResult() const;
};


_ReadFieldParams::_ReadFieldParams( const String& _rData )
    : aData( _rData ), nLen( _rData.Len() ), nNext( 0 )
{
    /*
        erstmal nach einer oeffnenden Klammer oder einer Leerstelle oder einem
        Anfuehrungszeichen oder einem Backslash suchen, damit der Feldbefehl
        (also INCLUDEPICTURE bzw EINFUeGENGRAFIK bzw ...) ueberlesen wird
    */
    while( (nLen > nNext) && (aData.GetChar( nNext ) == ' ') )
        ++nNext;

    sal_Unicode c;
    while(     nLen > nNext
            && (c = aData.GetChar( nNext )) != ' '
            && c != '"'
            && c != '\\'
            && c != 132  )
        ++nNext;

    nFnd      = nNext;
    nSavPtr   = nNext;
//	cLastChar = aData.GetChar( nSavPtr );
}


_ReadFieldParams::~_ReadFieldParams()
{
//	aData.SetChar( nSavPtr, cLastChar );
}


String _ReadFieldParams::GetResult() const
{
    return    (STRING_NOTFOUND == nFnd)
            ? aEmptyStr
            : aData.Copy( nFnd, (nSavPtr - nFnd) );
}


xub_StrLen _ReadFieldParams::GoToTokenParam()
{
    xub_StrLen nOld = nNext;
    if( -2 == SkipToNextToken() )
        return GetTokenSttPtr();
    nNext = nOld;
    return STRING_NOTFOUND;
}

long _ReadFieldParams::SkipToNextToken() // ret: -2: NOT a '\' parameter but normal Text
{
    long nRet = -1;		// Ende
//	aData.SetChar( nSavPtr, cLastChar );
    if(    (STRING_NOTFOUND != nNext)
        && (nLen > nNext)
        && STRING_NOTFOUND != ( nFnd = FindNextStringPiece( nNext ) ))
    {
        nSavPtr = nNext;
//		cLastChar = aData.GetChar( nSavPtr );

        if(    '\\' == aData.GetChar( nFnd )
            && '\\' != aData.GetChar( nFnd + 1 ) ) // Options-Parameter gefunden
        {
            nRet = aData.GetChar( ++nFnd );
            nNext = ++nFnd; 			// und dahinter setzen
        }
        else
        {
            nRet = -2;
            if(    (STRING_NOTFOUND != nSavPtr )
                && ('"' == aData.GetChar( nSavPtr - 1 )) )
            {
                nSavPtr--;
//				cLastChar = aData.GetChar( nSavPtr );
            }
//			aData.SetChar( nSavPtr, 0 );
        }
    }
    return nRet;
}

// FindNextPara sucht naechsten Backslash-Parameter oder naechste Zeichenkette
// bis zum Blank oder naechsten "\" oder zum schliessenden Anfuehrungszeichen
// oder zum String-Ende von pStr.
//
// Ausgabe ppNext (falls ppNext != 0) Suchbeginn fuer naechsten Parameter bzw. 0
//
// Returnwert: 0 falls String-Ende erreicht,
//             ansonsten Anfang des Paramters bzw. der Zeichenkette
//
xub_StrLen _ReadFieldParams::FindNextStringPiece(const xub_StrLen nStart)
{
    xub_StrLen  n = ( STRING_NOTFOUND == nStart ) ? nFnd : nStart;	// Anfang
    xub_StrLen n2;			// Ende

    nNext = STRING_NOTFOUND;		// Default fuer nicht gefunden

    while( (nLen > n) && (aData.GetChar( n ) == ' ') )
        ++n;

    if( nLen == n )
        return STRING_NOTFOUND;		// String End reached!

    if(		(aData.GetChar( n ) == '"')		// Anfuehrungszeichen vor Para?
        ||  (aData.GetChar( n ) == 132) )
    {
        n++;						// Anfuehrungszeichen ueberlesen
        n2 = n;						// ab hier nach Ende suchen
        while(     (nLen > n2)
                && (aData.GetChar( n2 ) != '"')
                && (aData.GetChar( n2 ) != 147) )
            n2++;					// Ende d. Paras suchen
    }
    else						// keine Anfuehrungszeichen
    {
        n2 = n;						// ab hier nach Ende suchen
        while( (nLen > n2) && (aData.GetChar( n2 ) != ' ') ) // Ende d. Paras suchen
        {
            if( aData.GetChar( n2 ) == '\\' )
            {
                if( aData.GetChar( n2+1 ) == '\\' )
                    n2 += 2;		// Doppel-Backslash -> OK
                else
                {
                    if( n2 > n )
                        n2--;
                    break;			// einfach-Backslash -> Ende
                }
            }
            else
                n2++;				// kein Backslash -> OK
        }
    }
    if( nLen > n2 )
    {
        if(aData.GetChar( n2 ) != ' ') n2++;
        nNext = n2;
    }
    return n;
}



// read parameters "1-3" or 1-3 with both values between 1 and nMax
bool _ReadFieldParams::GetTokenSttFromTo(USHORT* pFrom, USHORT* pTo, USHORT nMax)
{
    USHORT nStart = 0;
    USHORT nEnd   = 0;
    xub_StrLen n = GoToTokenParam();
    if( STRING_NOTFOUND != n )
    {

        String sParams( GetResult() );

        xub_StrLen nIndex = 0;
        String sStart( sParams.GetToken(0, '-', nIndex) );
        if( STRING_NOTFOUND != nIndex )
        {
            nStart = static_cast<USHORT>(sStart.ToInt32());
            nEnd   = static_cast<USHORT>(sParams.Copy(nIndex).ToInt32());
        }
    }
    if( pFrom ) *pFrom = nStart;
    if( pTo )   *pTo   = nEnd;

    return nStart && nEnd && (nMax >= nStart) && (nMax >= nEnd);
}

//----------------------------------------
//    			Bookmarks
//----------------------------------------

long SwWW8ImplReader::Read_Book(WW8PLCFManResult*)
{
    // muesste auch ueber pRes.nCo2OrIdx gehen
    WW8PLCFx_Book* pB = pPlcxMan->GetBook();
    if( !pB )
    {
        ASSERT( pB, "WW8PLCFx_Book - Pointer nicht da" );
        return 0;
    }

    eBookStatus eB = pB->GetStatus();
    if (eB & BOOK_IGNORE)
        return 0;								// Bookmark zu ignorieren

    if (pB->GetIsEnd())
    {
        pRefStck->SetAttr(*pPaM->GetPoint(), RES_FLTR_BOOKMARK, true,
            pB->GetHandle());
        return 0;
    }

    //"_Toc*" and "_Hlt*" are unnecessary
    const String* pName = pB->GetName();
    if(    !pName || pName->EqualsIgnoreCaseAscii( "_Toc", 0, 4 )
        || pName->EqualsIgnoreCaseAscii( "_Hlt", 0, 4 ) )
        return 0;

    //JP 16.11.98: ToUpper darf auf keinen Fall gemacht werden, weil der
    //Bookmark- name ein Hyperlink-Ziel sein kann!

    String aVal;
    if( SwFltGetFlag( nFieldFlags, SwFltControlStack::BOOK_TO_VAR_REF ) )
    {
        // Fuer UEbersetzung Bookmark -> Variable setzen
        long nLen = pB->GetLen();
        if( nLen > MAX_FIELDLEN )
            nLen = MAX_FIELDLEN;

        long nOldPos = pStrm->Tell();
        nLen = pSBase->WW8ReadString( *pStrm, aVal, pB->GetStartPos(), nLen,
                                        eStructCharSet );
        pStrm->Seek( nOldPos );

        // JP 19.03.2001 - now here the implementation of the old
        // 				"QuoteString" and I hope with a better performance
        //				as before. It's also only needed if the filterflags
        //				say we will convert bookmarks to SetExpFields! And
        //				this the exception!

        String sHex(CREATE_CONST_ASC( "\\x" ));
        bool bSetAsHex;
        bool bAllowCr = SwFltGetFlag(nFieldFlags, 
            SwFltControlStack::ALLOW_FLD_CR) ? true : false;

        sal_Unicode cChar;

        for( xub_StrLen nI = 0;
                nI < aVal.Len() && aVal.Len() < (MAX_FIELDLEN - 4); ++nI )
        {
            switch( cChar = aVal.GetChar( nI ) )
            {
            case 0x0b:
            case 0x0c:
            case 0x0d:
                if( bAllowCr )
                    aVal.SetChar( nI, '\n' ), bSetAsHex = false;
                else
                    bSetAsHex = true;
                break;

            case 0xFE:
            case 0xFF:
                bSetAsHex = true;
                break;

            default:
                bSetAsHex = 0x20 > cChar;
                break;
            }

            if( bSetAsHex )
            {
                //all Hex-Numbers with \x before
                String sTmp( sHex );
                if( cChar < 0x10 )
                    sTmp += '0';
                sTmp += String::CreateFromInt32( cChar, 16 );
                aVal.Replace( nI, 1 , sTmp );
                nI += sTmp.Len() - 1;
            }
        }

        if( aVal.Len() > (MAX_FIELDLEN - 4))
            aVal.Erase( MAX_FIELDLEN - 4 );
    }
    pRefStck->NewAttr( *pPaM->GetPoint(), SwFltBookmark( *pName, aVal,
        pB->GetHandle(), 0 ) );
    return 0;
}

//----------------------------------------------------------------------
//    allgemeine Hilfsroutinen zum Auseinanderdroeseln der Parameter
//----------------------------------------------------------------------

// ConvertFFileName uebersetzt FeldParameter-Namen u. ae. in den
// System-Zeichensatz.
// Gleichzeitig werden doppelte Backslashes in einzelne uebersetzt.
void SwWW8ImplReader::ConvertFFileName( String& rName, const String& rOrg )
{
    rName = rOrg;
    rName.SearchAndReplaceAllAscii( "\\\\", String( '\\' ));

    // ggfs. anhaengende Anfuehrungszeichen entfernen
    if( rName.Len() && 	'"' == rName.GetChar( rName.Len()-1 ))
        rName.Erase( rName.Len()-1, 1);

    //#82900# Need the more sophisticated url converter. cmc
    rName = URIHelper::SmartRelToAbs(rName);
}

// ConvertUFNneme uebersetzt FeldParameter-Namen u. ae. in den
// System-Zeichensatz und Upcased sie ( z.B. fuer Ref-Felder )
void SwWW8ImplReader::ConvertUFName( String& rName )
{
    GetAppCharClass().toUpper( rName );
}

void lcl_ConvertSequenceName( SwWW8ImplReader& rReader, String& rSequenceName )
{
    rReader.ConvertUFName( rSequenceName );
    if(    '0' <= rSequenceName.GetChar( 0 )
        && '9' >= rSequenceName.GetChar( 0 ) )
        rSequenceName.Insert('_', 0);
}

// FindParaStart() finds 1st Parameter that follows '\' and cToken
// and returns start of this parameter or STRING_NOT_FOUND.
xub_StrLen FindParaStart( const String& rStr, sal_Unicode cToken, sal_Unicode cToken2 )
{
    bool bStr = false;			// innerhalb String ignorieren

    for( xub_StrLen nBuf=0; nBuf+1 < rStr.Len(); nBuf++ )
    {
        if( rStr.GetChar( nBuf ) == '"' )
            bStr = !bStr;

        if(    !bStr
            && rStr.GetChar( nBuf ) == '\\'
            && (    rStr.GetChar( nBuf + 1 ) == cToken
                 || rStr.GetChar( nBuf + 1 ) == cToken2 ) )
        {
            nBuf += 2;
            // skip spaces between cToken and it's parameters
            while(    nBuf < rStr.Len()
                   && rStr.GetChar( nBuf ) == ' ' )
                nBuf++;
            // return start of parameters
            return nBuf < rStr.Len() ? nBuf : STRING_NOTFOUND;
        }
    }
    return STRING_NOTFOUND;
}

// FindPara() findet den ersten Parameter mit '\' und cToken. Es wird
// ein neuer String allokiert ( der vom Aufrufer deallokiert werden muss )
// und alles, was zum Parameter gehoert, wird in ihm zurueckgeliefert.
String FindPara( const String& rStr, sal_Unicode cToken, sal_Unicode cToken2 )
{
    xub_StrLen n2;											// Ende
    xub_StrLen n = FindParaStart( rStr, cToken, cToken2 );	// Anfang
    if( STRING_NOTFOUND == n )
        return aEmptyStr;

    if(    rStr.GetChar( n ) == '"'
        || rStr.GetChar( n ) == 132 )
    {								// Anfuehrungszeichen vor Para
        n++;						// Anfuehrungszeichen ueberlesen
        n2 = n;						// ab hier nach Ende suchen
        while(     n2 < rStr.Len()
                && rStr.GetChar( n2 ) != 147
                && rStr.GetChar( n2 ) != '"' )
            n2++;					// Ende d. Paras suchen
    }
    else
    {							// keine Anfuehrungszeichen
        n2 = n;						// ab hier nach Ende suchen
        while(     n2 < rStr.Len()
                && rStr.GetChar( n2 ) != ' ' )
            n2++;					// Ende d. Paras suchen
    }
    return rStr.Copy( n, n2-n );
}


static SvxExtNumType GetNumTypeFromName(const String& rStr, 
    bool bAllowPageDesc = false)
{
    SvxExtNumType eTyp = bAllowPageDesc ? SVX_NUM_PAGEDESC : SVX_NUM_ARABIC;
    if( rStr.EqualsIgnoreCaseAscii( "Arabi", 0, 5 ) )  // Arabisch, Arabic
        eTyp = SVX_NUM_ARABIC;
    else if( rStr.EqualsAscii( "misch", 2, 5 ) ) 	// r"omisch
        eTyp = SVX_NUM_ROMAN_LOWER;
    else if( rStr.EqualsAscii( "MISCH", 2, 5 ) )	// R"OMISCH
        eTyp = SVX_NUM_ROMAN_UPPER;
    else if( rStr.EqualsIgnoreCaseAscii( "alphabeti", 0, 9 ) )// alphabetisch, alphabetic
        eTyp =  ( rStr.GetChar( 0 ) == 'A' )
                ? SVX_NUM_CHARS_UPPER_LETTER_N
                : SVX_NUM_CHARS_LOWER_LETTER_N;
    else if( rStr.EqualsIgnoreCaseAscii( "roman", 0, 5 ) )	// us
        eTyp =  ( rStr.GetChar( 0 ) == 'R' )
                ? SVX_NUM_ROMAN_UPPER
                : SVX_NUM_ROMAN_LOWER;
    return eTyp;
}

static SvxExtNumType GetNumberPara(String& rStr, bool bAllowPageDesc = false)
{
    String s( FindPara( rStr, '*', '*' ) );		// Ziffernart
    SvxExtNumType aType = GetNumTypeFromName( s, bAllowPageDesc );
    return aType;
}

static ULONG MSDateTimeFormatToSwFormat(String& rParams, 
    SvNumberFormatter *pFormatter, USHORT nLang)
{
    // tell the Formatter about the new entry
    UINT16 nCheckPos = 0;
    INT16  nType = NUMBERFORMAT_DEFINED;
    ULONG  nKey = 0;

    pFormatter->PutEntry(rParams, nCheckPos, nType, nKey, nLang);

    return nKey;
}

short SwWW8ImplReader::GetTimeDatePara(String& rStr, ULONG& rFormat)
{
    SvNumberFormatter* pFormatter = rDoc.GetNumberFormatter();
    String sParams( FindPara( rStr, '@', '@' ) );// Date/Time
    if (!sParams.Len())
    {
        //Get the system date in the correct final language layout, convert to
        //a known language and modify the 2 digit year part to be 4 digit, and
        //convert back to the correct language layout.
        LanguageType nLang = LANGUAGE_ENGLISH_US;
        if (const SvxLanguageItem *pLang = 
            (const SvxLanguageItem*)GetFmtAttr(RES_CHRATR_LANGUAGE))
        {
            nLang = pLang->GetValue();
        }

        ULONG nIndex = pFormatter->GetFormatIndex(NF_DATE_SYSTEM_SHORT, nLang);

        SvNumberformat aFormat = const_cast<SvNumberformat &>
            (*(pFormatter->GetEntry(nIndex)));
        aFormat.ConvertLanguage(*pFormatter, nLang, LANGUAGE_ENGLISH_US);

        sParams = aFormat.GetFormatstring();
        sParams.SearchAndReplace(CREATE_CONST_ASC("YY"),
            CREATE_CONST_ASC("YYYY"));

        UINT16 nCheckPos = 0;
        INT16 nType = NUMBERFORMAT_DEFINED;
        rFormat = 0;

        pFormatter->PutandConvertEntry(sParams, nCheckPos, nType, rFormat,
            LANGUAGE_ENGLISH_US, nLang);

        return NUMBERFORMAT_DATE;
    }

    const SvxLanguageItem& rLang = (const SvxLanguageItem&)(rDoc.GetAttrPool().
        GetDefaultItem( RES_CHRATR_LANGUAGE ));

    ULONG nFmtIdx = MSDateTimeFormatToSwFormat(sParams, pFormatter, 
        rLang.GetValue()); 
    short nNumFmtType = NUMBERFORMAT_UNDEFINED;
    if (nFmtIdx)
        nNumFmtType = pFormatter->GetType(nFmtIdx);
    rFormat = nFmtIdx;

    return nNumFmtType;
}

//-----------------------------------------
//				Felder
//-----------------------------------------
// Am Ende des Einlesens entsprechende Felder updaten ( z.Zt. die Referenzen )
void SwWW8ImplReader::UpdateFields()
{
//	rDoc.GetSysFldType( RES_GETREFFLD )->UpdateFlds();	// Referenzen
//	rDoc.UpdateFlds();									// SetExp-Fields
//	rDoc.UpdateFlds();				// alles ???
//	rDoc.UpdateExpFlds();								// SetExp-Fields
    rDoc.SetUpdateExpFldStat();					// JP: neu fuer alles wichtige
    rDoc.SetInitDBFields(true);				// Datenbank-Felder auch
}

// Read_Field liest ein Feld ein oder, wenn es nicht gelesen werden kann,
// wird 0 zurueckgegeben, so dass das Feld vom Aufrufer textuell gelesen wird.
// Returnwert: Gesamtlaenge des Feldes ( zum UEberlesen )
long SwWW8ImplReader::Read_Field(WW8PLCFManResult* pRes)
{
    typedef eF_ResT (SwWW8ImplReader:: *FNReadField)( WW8FieldDesc*, String& );
    static FNReadField aWW8FieldTab[93] = 
    {
        0,
        0,
        0,
        &SwWW8ImplReader::Read_F_Ref,				// 3
        0,
        0,
        &SwWW8ImplReader::Read_F_Set,				// 6
        0,
        &SwWW8ImplReader::Read_F_Tox,				// 8
        0,
        0,
        0,
        &SwWW8ImplReader::Read_F_Seq,				// 12
        &SwWW8ImplReader::Read_F_Tox,				// 13
        &SwWW8ImplReader::Read_F_DocInfo,			// 14
        &SwWW8ImplReader::Read_F_DocInfo,			// 15
        &SwWW8ImplReader::Read_F_DocInfo,			// 16
        &SwWW8ImplReader::Read_F_Author,			// 17
        &SwWW8ImplReader::Read_F_DocInfo,			// 18
        &SwWW8ImplReader::Read_F_DocInfo,			// 19
        &SwWW8ImplReader::Read_F_DocInfo,			// 20
        &SwWW8ImplReader::Read_F_DocInfo,			// 21
        &SwWW8ImplReader::Read_F_DocInfo,			// 22
        &SwWW8ImplReader::Read_F_DocInfo,			// 23
        &SwWW8ImplReader::Read_F_DocInfo,			// 24
        &SwWW8ImplReader::Read_F_DocInfo,			// 25
        &SwWW8ImplReader::Read_F_Anz,				// 26
        &SwWW8ImplReader::Read_F_Anz,				// 27
        &SwWW8ImplReader::Read_F_Anz,				// 28
        &SwWW8ImplReader::Read_F_FileName,			// 29
        &SwWW8ImplReader::Read_F_TemplName,			// 30
        &SwWW8ImplReader::Read_F_DateTime,			// 31
        &SwWW8ImplReader::Read_F_DateTime,			// 32
        &SwWW8ImplReader::Read_F_CurPage,			// 33
        0,
        0,
        &SwWW8ImplReader::Read_F_IncludeText,		// 36
        &SwWW8ImplReader::Read_F_PgRef,				// 37
        &SwWW8ImplReader::Read_F_InputVar,			// 38
        &SwWW8ImplReader::Read_F_Input,				// 39
        0,
        &SwWW8ImplReader::Read_F_DBNext,			// 41
        0,
        0,
        &SwWW8ImplReader::Read_F_DBNum,				// 44
        0,
        0,
        0,
        0,
        &SwWW8ImplReader::Read_F_Equation,			// 49
        0,
        &SwWW8ImplReader::Read_F_Macro,				// 51
        &SwWW8ImplReader::Read_F_ANumber,			// 52
        &SwWW8ImplReader::Read_F_ANumber,			// 53
        &SwWW8ImplReader::Read_F_ANumber,			// 54
        0,


        0,		// 56: VERKNUePFUNG		// fehlt noch !!!!!!!!!!!!!!!!!!!!!!!


        &SwWW8ImplReader::Read_F_Symbol,			// 57
        &SwWW8ImplReader::Read_F_Embedd,			// 58
        &SwWW8ImplReader::Read_F_DBField,			// 59
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        &SwWW8ImplReader::Read_F_IncludePicture,	// 67
        &SwWW8ImplReader::Read_F_IncludeText,		// 68
        0,
        &SwWW8ImplReader::Read_F_FormTextBox,		// 70
        &SwWW8ImplReader::Read_F_FormCheckBox,		// 71
        &SwWW8ImplReader::Read_F_NoteReference,		// 72
        0, /*&SwWW8ImplReader::Read_F_Tox*/
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        &SwWW8ImplReader::Read_F_FormListBox,		// 83
        0,											// 84
        &SwWW8ImplReader::Read_F_DocInfo,			// 85
        0,											// 86
        &SwWW8ImplReader::Read_F_OCX,				// 87
        &SwWW8ImplReader::Read_F_Hyperlink,			// 88
        0,											// 89
        0,											// 90
        0,											// 91
        0 											// 92 - Dummy leer Methode
    };											// 92   == alle ueber 91

    ASSERT( ( sizeof( aWW8FieldTab ) / sizeof( *aWW8FieldTab ) == 93 ),
            "FeldFunc-Tabelle stimmt nicht" );
    if( nIniFlags & WW8FL_NO_FLD )
        return 0;

    WW8FieldDesc aF;

    WW8PLCFx_FLD* pF = pPlcxMan->GetFld();
    ASSERT(pF, "WW8PLCFx_FLD - Pointer nicht da");
    if( !pF )
        return 0;

    bool bOk = pF->GetPara( pRes->nCp2OrIdx, aF );

    ASSERT(bOk, "WW8: Bad Field!\n");

    maFieldStack.push(aF.nId);

    USHORT n = ( aF.nId <= 91 ) ? aF.nId : 92; // alle > 91 werden 92
    USHORT nI = n / 32;						// # des UINT32
    ULONG nMask = 1 << ( n % 32 );			// Maske fuer Bits

    if( nFieldTagAlways[nI] & nMask )		// Flag: Tag it
        return Read_F_Tag( &aF );			// Resultat nicht als Text

    if( !bOk || !aF.nId )                   // Feld kaputt
        return aF.nLen;						// -> ignorieren

    if( aF.nId > 91
//		|| ( aF.nOpt & 0x40 )				// 0x40-Result Nest -> OK
                             )				// WW: Nested Field
        if( nFieldTagBad[nI] & nMask )		// Flag: Tag it when bad
            return Read_F_Tag( &aF );		// Resultat nicht als Text
        else
            return aF.nLen;

    if( aWW8FieldTab[aF.nId] == 0			// keine Routine vorhanden
        || aF.bCodeNest )
    {
        if( nFieldTagBad[nI] & nMask )		// Flag: Tag it when bad
            return Read_F_Tag( &aF );		// Resultat nicht als Text
                                            // Lese nur Resultat
        if( aF.bResNest )
            return aF.nLen;					// Result nested -> nicht brauchbar

        return aF.nLen - aF.nLRes - 1;  // so viele ueberlesen, das Resultfeld
                                        // wird wie Haupttext eingelesen

    }
    else
    {									// Lies Feld
        long nOldPos = pStrm->Tell();
        String aStr;
        aF.nLCode = pSBase->WW8ReadString( *pStrm, aStr, pPlcxMan->GetCpOfs()+
            aF.nSCode, aF.nLCode, eTextCharSet );

        eF_ResT eRes = (this->*aWW8FieldTab[aF.nId])( &aF, aStr );
        pStrm->Seek( nOldPos );

        switch ( eRes )
        {
            case FLD_OK:
                return aF.nLen;						// alles OK
            case FLD_TEXT:
                // so viele ueberlesen, das Resultfeld wird wie Haupttext
                // eingelesen
                // JP 15.07.99: attributes can start at char 0x14 so skip one
                // char more back == "-2"
                return aF.nLen - aF.nLRes - 2;		
            case FLD_TAGTXT:
                if(  ( nFieldTagBad[nI] & nMask ) )	// Flag: Tag bad
                    return Read_F_Tag( &aF );		// Taggen
                return aF.nLen - aF.nLRes - 2;	// oder Text-Resultat
            case FLD_TAGIGN:
                if(  ( nFieldTagBad[nI] & nMask ) )	// Flag: Tag bad
                    return Read_F_Tag( &aF );		// Taggen
                return aF.nLen;					// oder ignorieren
            case FLD_READ_FSPA:
                return aF.nLen - aF.nLRes - 2; // auf Char 1 positionieren
            default:	
                return aF.nLen;						// ignorieren
        }
    }
}

//-----------------------------------------
//		  Felder Taggen
//-----------------------------------------

// MakeTagString() gibt als Returnwert die Position des ersten
// CR / Zeilenende / Seitenumbruch in pText und wandelt auch nur bis dort
// Wenn keins dieser Sonderzeichen enthalten ist, wird 0 zurueckgeliefert.
void SwWW8ImplReader::MakeTagString( String& rStr, const String& rOrg )
{
    String sHex( CREATE_CONST_ASC( "\\x" ));
    bool bAllowCr = SwFltGetFlag( nFieldFlags, SwFltControlStack::TAGS_IN_TEXT )
                || SwFltGetFlag( nFieldFlags, SwFltControlStack::ALLOW_FLD_CR );
    sal_Unicode cChar;
    rStr = rOrg;

    for( xub_StrLen nI = 0;
            nI < rStr.Len() && rStr.Len() < (MAX_FIELDLEN - 4); ++nI )
    {
        bool bSetAsHex = false;
        switch( cChar = rStr.GetChar( nI ) )
        {
            case 132:                 		// Typographische Anfuehrungszeichen
            case 148:						// gegen normale tauschen
            case 147: 
                rStr.SetChar( nI, '"' ); 
                break;
            case 19:  
                rStr.SetChar( nI, '{' ); 
                break;	// 19..21 zu {|}
            case 20:  
                rStr.SetChar( nI, '|' ); 
                break;
            case 21:  
                rStr.SetChar( nI, '}' ); 
                break;
            case '\\':						// \{|} per \ Taggen
            case '{':
            case '|':
            case '}': 
                rStr.Insert( nI, '\\' ); 
                ++nI; 
                break;
            case 0x0b:
            case 0x0c:
            case 0x0d:
                if( bAllowCr )
                    rStr.SetChar( nI, '\n' );
                else
                    bSetAsHex = true;
                break;
            case 0xFE:
            case 0xFF:
                bSetAsHex = true;
                break;
            default:
                bSetAsHex = 0x20 > cChar;
                break;
        }

        if( bSetAsHex )
        {
            //all Hex-Numbers with \x before
            String sTmp( sHex );
            if( cChar < 0x10 )
                sTmp += '0';
            sTmp += String::CreateFromInt32( cChar, 16 );
            rStr.Replace( nI, 1 , sTmp );
            nI += sTmp.Len() - 1;
        }
    }

    if( rStr.Len() > (MAX_FIELDLEN - 4))
        rStr.Erase( MAX_FIELDLEN - 4 );
}

void SwWW8ImplReader::InsertTagField( const USHORT nId, const String& rTagText )
{
    String aName( CREATE_CONST_ASC( "WwFieldTag" ) );
    if( SwFltGetFlag( nFieldFlags, SwFltControlStack::TAGS_DO_ID ) ) // Nummer?
        aName += String::CreateFromInt32( nId );					// ausgeben ?

    if( SwFltGetFlag(nFieldFlags, SwFltControlStack::TAGS_IN_TEXT))
    {
        aName += rTagText;		// als Txt taggen
        rDoc.Insert(*pPaM, aName, false);
    }
    else
    {													// normal tagggen

        SwFieldType* pFT = rDoc.InsertFldType(
                                SwSetExpFieldType( &rDoc, aName, GSE_STRING ) );
        SwSetExpField aFld( (SwSetExpFieldType*)pFT, rTagText );							// SUB_INVISIBLE
        USHORT nSubType = ( SwFltGetFlag( nFieldFlags, SwFltControlStack::TAGS_VISIBLE ) ) ? 0 : SUB_INVISIBLE;
        aFld.SetSubType(nSubType|GSE_STRING);

        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    }
}

long SwWW8ImplReader::Read_F_Tag( WW8FieldDesc* pF )
{
    long nOldPos = pStrm->Tell();

    WW8_CP nStart = pF->nSCode - 1;			// mit 0x19 am Anfang
    long nL = pF->nLen;						// Gesamtlaenge mit Resultat u. Nest
    if( nL > MAX_FIELDLEN )
        nL = MAX_FIELDLEN;					// MaxLaenge, durch Quoten
                                            // max. 4* so gross
    String sFTxt;
    nL = pSBase->WW8ReadString( *pStrm, sFTxt,
                                pPlcxMan->GetCpOfs() + nStart, nL, eStructCharSet);


    String aTagText;
    MakeTagString( aTagText, sFTxt );
    InsertTagField( pF->nId, aTagText );

    pStrm->Seek( nOldPos );
    return pF->nLen;
}


//-----------------------------------------
//		  normale Felder
//-----------------------------------------

// Read_F_Nul() dient dazu, Felder, die ein belegtes ResultatFeld haben, dieses
// aber trotzdem keinen Text darstellt, sondern z.B. Variablen-Werte, komplett
// zu ignorieren.
// Noetig z.B. fuer Feld 6 "Set" == "Bestimmen".
eF_ResT SwWW8ImplReader::Read_F_Nul( WW8FieldDesc*, String& )
{
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_Input( WW8FieldDesc* pF, String& rStr )
{
    String aDef;
    String aQ;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aQ.Len() )
                aQ = aReadParam.GetResult();
            break;
        case 'd':
        case 'D':
            {
                xub_StrLen n = aReadParam.GoToTokenParam();
                if( STRING_NOTFOUND != n )
                    aDef = aReadParam.GetResult();
            }
            break;
        }
    }
    if( !aDef.Len() )
        aDef = GetFieldResult( pF );

    SwInputField aFld( (SwInputFieldType*)rDoc.GetSysFldType( RES_INPUTFLD ),
                        aDef, aQ, INP_TXT, 0 );	// sichtbar ( geht z.Zt. nicht anders )
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );

    return FLD_OK;
}

// GetFieldResult alloziert einen String und liest das Feld-Resultat ein
String SwWW8ImplReader::GetFieldResult( WW8FieldDesc* pF )
{
    long nOldPos = pStrm->Tell();

    WW8_CP nStart = pF->nSRes;				// Start Resultat
    long nL = pF->nLRes;					// Laenge Resultat
    if( !nL )
        return aEmptyStr;							// kein Resultat

    if( nL > MAX_FIELDLEN )
        nL = MAX_FIELDLEN;					// MaxLaenge, durch Quoten
                                            // max. 4* so gross

    String sRes;
    nL = pSBase->WW8ReadString( *pStrm, sRes, pPlcxMan->GetCpOfs() + nStart,
                                nL, eStructCharSet );

    pStrm->Seek( nOldPos );
    return sRes;
}

/*
Bookmarks can be set with fields SET and ASK, and they can be referenced with
REF. When set, they behave like variables in writer, otherwise they behave
like normal bookmarks. We can check whether we should use a show variable
instead of a normal bookmark ref by converting to "show variable" at the end
of the document those refs which look for the content of a bookmark but whose
bookmarks were set with SET or ASK. (See SwWW8FltRefStack)

The other piece of the puzzle is that refs that point to the "location" of the
bookmark will in word actually point to the last location where the bookmark
was set with SET or ASK, not the actual bookmark. This is only noticable when
a document sets the bookmark more than once. This is because word places the
true bookmark at the location of the last set, but the refs will display the
position of the first set before the ref. 

So what we will do is 

1) keep a list of all bookmarks that were set, any bookmark names mentioned
here that are refed by content will be converted to show variables.

2) create pseudo bookmarks for every position that a bookmark is set with SET
or ASK but has no existing bookmark. We can then keep a map from the original
bookmark name to the new one. As we parse the document new pseudo names will
replace the older ones, so the map always contains the bookmark of the
location that msword itself would use.

3) word's bookmarks are case insensitive, writers are not. So we need to
map case different versions together, regardless of whether they are
variables or not.

4) when a reference is (first) SET or ASK, the bookmark associated with it
is placed around the 0x14 0x15 result part of the field. We will fiddle
the placement to be the writer equivalent of directly before and after
the field, which gives the same effect and meaning, to do so we must
get any bookmarks in the field range, and begin them immediately before
the set/ask field, and end them directly afterwards. MapBookmarkVariables
returns an identifier of the bookmark attribute to close after inserting
the appropiate set/ask field.
*/
long SwWW8ImplReader::MapBookmarkVariables(const WW8FieldDesc* pF,
    String &rOrigName, const String &rData)
{
    ASSERT(pPlcxMan,"No pPlcxMan");
    long nNo;
    /*
    If there was no bookmark associated with this set field, then we create a
    pseudo one and insert it in the document.
    */
    USHORT nIndex;
    pPlcxMan->GetBook()->MapName(rOrigName);
    String sName = pPlcxMan->GetBook()->GetBookmark(
        pF->nSCode, pF->nSCode + pF->nLen, nIndex);
    if (sName.Len())
    {
        pPlcxMan->GetBook()->SetStatus(nIndex,BOOK_IGNORE);
        nNo = nIndex;
    }
    else
    {
        sName = CREATE_CONST_ASC("WWSetBkmk");
        nNo = pRefStck->aFieldVarNames.size()+1;
        sName += String::CreateFromInt32(nNo);
        nNo += pPlcxMan->GetBook()->GetIMax();
    }
    pRefStck->NewAttr(*pPaM->GetPoint(),SwFltBookmark(sName,rData,nNo,0));
    pRefStck->aFieldVarNames[rOrigName] = sName;
    return nNo;
}

/*
Word can set a bookmark with set or with ask, such a bookmark is equivalent to
our variables, but until the end of a document we cannot be sure if a bookmark
is a variable or not, at the end we will have a list of reference names which
were set or asked, all bookmarks using the content of those bookmarks are
converted to show variables, those that reference the position of the field
can be left as references, because a bookmark is also inserted at the position
of a set or ask field, either by word, or in some special cases by the import
filter itself.
*/
SwFltStackEntry *SwWW8FltRefStack::RefToVar(const SwField* pFld,
    SwFltStackEntry *pEntry)
{
    SwFltStackEntry *pRet=0;
    if (RES_GETREFFLD == pFld->Which())
    {
        //Get the name of the ref field, and see if actually a variable
        const String &rName = pFld->GetPar1();
        ::std::map<String,String,SwWW8FltRefStack::ltstr>::const_iterator 
            aResult = aFieldVarNames.find(rName);

        if (aResult != aFieldVarNames.end())
        {
            SwGetExpField aFld( (SwGetExpFieldType*)
                pDoc->GetSysFldType(RES_GETEXPFLD), rName, GSE_STRING, 0);
            delete pEntry->pAttr;
            SwFmtFld aTmp(aFld);
            pEntry->pAttr = aTmp.Clone();
            pRet = pEntry;
        }
    }
    return pRet;
}

const String &SwWW8ImplReader::GetMappedBookmark(String &rOrigName)
{
    ASSERT(pPlcxMan,"no pPlcxMan");
    pPlcxMan->GetBook()->MapName(rOrigName);

    //See if there has been a variable set with this name, if so get 
    //the pseudo bookmark name that was set with it.
    ::std::map<String,String,SwWW8FltRefStack::ltstr>::const_iterator aResult =
            pRefStck->aFieldVarNames.find(rOrigName);

    const String &rBkmName = (aResult == pRefStck->aFieldVarNames.end()) 
        ? rOrigName : (*aResult).second;

    return rBkmName;
}

// "ASK"
eF_ResT SwWW8ImplReader::Read_F_InputVar( WW8FieldDesc* pF, String& rStr )
{
    String sOrigName;
    String aQ;
    String aDef;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !sOrigName.Len() )
                sOrigName = aReadParam.GetResult();
            else if( !aQ.Len() )
                aQ = aReadParam.GetResult();
            break;
        case 'd':
        case 'D':
            if (STRING_NOTFOUND != aReadParam.GoToTokenParam())
                aDef = aReadParam.GetResult();
            break;
        }
    }

    if( !sOrigName.Len() )
        return FLD_TAGIGN;  // macht ohne Textmarke keinen Sinn
    if( !aDef.Len() )
        aDef = GetFieldResult( pF );

    long nNo = MapBookmarkVariables(pF,sOrigName,aDef);

    SwSetExpFieldType* pFT = (SwSetExpFieldType*)rDoc.InsertFldType(
        SwSetExpFieldType(&rDoc, sOrigName, GSE_STRING));
    SwSetExpField aFld( pFT, aDef );
    aFld.SetSubType(SUB_INVISIBLE|GSE_STRING);
    aFld.SetInputFlag(true);
    aFld.SetPromptText( aQ );

    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );

    pRefStck->SetAttr(*pPaM->GetPoint(), RES_FLTR_BOOKMARK, true, nNo);
    return FLD_OK;
}

// "AUTONR"
eF_ResT SwWW8ImplReader::Read_F_ANumber( WW8FieldDesc*, String& rStr )
{
    if( !pNumFldType ){		// 1. Mal
        SwSetExpFieldType aT( &rDoc, CREATE_CONST_ASC("AutoNr"), GSE_SEQ );
        pNumFldType = rDoc.InsertFldType( aT );
    }
    SwSetExpField aFld( (SwSetExpFieldType*)pNumFldType, aEmptyStr,
                        GetNumberPara( rStr ) );
    aFld.SetValue( ++nFldNum );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

// "SEQ"
eF_ResT SwWW8ImplReader::Read_F_Seq( WW8FieldDesc*, String& rStr )
{
    String aSequenceName;
    String aBook;
    bool bHidden	= false;
    bool bFormat	= false;
    bool bShowLast	= false;
    bool bCountOn	= true;
    String sStart, sLevel;
    SvxExtNumType eNumFormat = SVX_NUM_ARABIC;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aSequenceName.Len() )
                aSequenceName = aReadParam.GetResult();
            else if( !aBook.Len() )
                aBook = aReadParam.GetResult();
            break;

        case 'h':
            if( !bFormat )
                bHidden = true;				// Hidden-Flag aktivieren
            break;

        case '*':
            bFormat = true;					// Format-Flag aktivieren
            bHidden = false;				// Hidden-Flag deaktivieren
            nRet = aReadParam.SkipToNextToken();
            if( -2 == nRet )
                eNumFormat = GetNumTypeFromName( aReadParam.GetResult() );
            break;

        case 'r':
            bShowLast = false;				// Zaehler neu setzen
            bCountOn  = false;
            nRet = aReadParam.SkipToNextToken();
            if( -2 == nRet )
                sStart = aReadParam.GetResult();
            break;

        case 'c':
            bShowLast = true;			// zuletzt verwendete Nummer anzeigen
            bCountOn  = false;
            break;

        case 'n':
            bCountOn  = true;				// Nummer um eins erhoehen (default)
            bShowLast = false;
            break;

        case 's':						// Outline Level
            if( -2 == aReadParam.SkipToNextToken() )
                sLevel = aReadParam.GetResult();
            break;
        }
    }
    if (!aSequenceName.Len() && !aBook.Len())
        return FLD_TAGIGN;

    SwSetExpFieldType* pFT = (SwSetExpFieldType*)rDoc.InsertFldType(
                        SwSetExpFieldType( &rDoc, aSequenceName, GSE_SEQ ) );
    SwSetExpField aFld( pFT, aEmptyStr,	eNumFormat );

    if( sStart.Len() )
        aFld.SetFormula( ( aSequenceName += '=' ) += sStart );

    if( sLevel.Len() )
    {
        BYTE nLvl = (BYTE)sLevel.ToInt32();
        if( nLvl )
        {
            if( MAXLEVEL <= --nLvl )
                nLvl = MAXLEVEL - 1;
            pFT->SetOutlineLvl( nLvl );
        }
    }

    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}


eF_ResT SwWW8ImplReader::Read_F_DocInfo( WW8FieldDesc* pF, String& rStr )
{
    USHORT nSub=0;
    // RegInfoFormat, DefaultFormat fuer DocInfoFelder
    USHORT nReg = DI_SUB_AUTHOR;
    bool bDateTime = false;

    if( 85 == pF->nId )
    {
        /*
        There are up to 26 fields that may be meant by 'DocumentProperty'.
        Which of them is to be inserted here ?
        This Problem can only be solved by implementing a name matching
        method that compares the given Parameter String with the four
        possible name sets (english, german, french, spanish)
        */
        String aStr( rStr );
        xub_StrLen nPos1 = aStr.Search( '"' );
        if( (STRING_NOTFOUND != nPos1) && (nPos1+1 < aStr.Len()) )
        {
            xub_StrLen nPos2 = aStr.SearchAndReplace( '"', '\0', nPos1+1 );
            if (STRING_NOTFOUND != nPos2)
            {
                aStr.Erase(0, nPos1+1);
                static const sal_Char* aName10 = "\x0F"; // SW field code
                static const sal_Char* aName11 // German
                    = "TITEL";
                static const sal_Char* aName12 // French
                    = "TITRE";
                static const sal_Char* aName13 // English
                    = "TITLE";
                static const sal_Char* aName14 // Spanish
                    = "TITRO";
                static const sal_Char* aName20 = "\x15"; // SW filed code
                static const sal_Char* aName21 // German
                    = "ERSTELLDATUM";
                static const sal_Char* aName22 // French
                    = "CR��";
                static const sal_Char* aName23 // English
                    = "CREATED";
                static const sal_Char* aName24 // Spanish
                    = "CREADO";
                static const sal_Char* aName30 = "\x16"; // SW filed code
                static const sal_Char* aName31 // German
                    = "ZULETZTGESPEICHERTZEIT";
                static const sal_Char* aName32 // French
                    = "DERNIERENREGISTREMENT";
                static const sal_Char* aName33 // English
                    = "SAVED";
                static const sal_Char* aName34 // Spanish
                    = "MODIFICADO";
                static const sal_Char* aName40 = "\x17"; // SW filed code
                static const sal_Char* aName41 // German
                    = "ZULETZTGEDRUCKT";
                static const sal_Char* aName42 // French
                    = "DERNI�REIMPRESSION";
                static const sal_Char* aName43 // English
                    = "LASTPRINTED";
                static const sal_Char* aName44 // Spanish
                    = "HUPS PUPS";
                static const sal_Char* aName50 = "\x18"; // SW filed code
                static const sal_Char* aName51 // German
                    = "�BERARBEITUNGSNUMMER";
                static const sal_Char* aName52 // French
                    = "NUM�RODEREVISION";
                static const sal_Char* aName53 // English
                    = "REVISIONNUMBER";
                static const sal_Char* aName54 // Spanish
                    = "SNUBBEL BUBBEL";
                static const USHORT nFldCnt  = 5;

                // additional fields are to be coded soon!   :-)

                static const USHORT nLangCnt = 4;
                static const sal_Char *aNameSet_26[nFldCnt][nLangCnt+1] =
                {
                    {aName10, aName11, aName12, aName13, aName14},
                    {aName20, aName21, aName22, aName23, aName24},
                    {aName30, aName31, aName32, aName33, aName34},
                    {aName40, aName41, aName42, aName43, aName44},
                    {aName50, aName51, aName52, aName53, aName54}
                };
                bool bFldFound= false;
                USHORT nFIdx;
                for(USHORT nLIdx=1; !bFldFound && (nLangCnt > nLIdx); ++nLIdx)
                {
                    for(nFIdx = 0;  !bFldFound && (nFldCnt  > nFIdx); ++nFIdx)
                    {
                        if( aStr.Equals( String( aNameSet_26[nFIdx][nLIdx],
                            RTL_TEXTENCODING_MS_1252 ) ) )
                        {
                            bFldFound = true;
                            pF->nId   = aNameSet_26[nFIdx][0][0];
                        }
                    }
                }
                if( !bFldFound )
                    return FLD_TAGTXT; // Error: show field as string
            }
        }
    }

    switch( pF->nId )
    {
        case 14:
            /* kann alle INFO-Vars!! */
            nSub = DI_KEYS;
            break;
        case 15:
            nSub = DI_TITEL;
            break;
        case 16:
            nSub = DI_THEMA;
            break;
        case 18:
            nSub = DI_KEYS;
            break;
        case 19:
            nSub = DI_COMMENT;
            break;
        case 20:
            nSub = DI_CHANGE;
            nReg = DI_SUB_AUTHOR;
            break;
        case 21:
            nSub = DI_CREATE;
            nReg = DI_SUB_DATE;
            bDateTime = true;
            break;
        case 23:
            nSub = DI_PRINT;
            nReg = DI_SUB_DATE;
            bDateTime = true;
            break;
        case 24:
            nSub = DI_DOCNO;
            break;
        case 22:
            nSub = DI_CHANGE;
            nReg = DI_SUB_DATE;
            bDateTime = true;
            break;
        case 25:
            nSub = DI_CHANGE;
            nReg = DI_SUB_TIME;
            bDateTime = true;
            break;
    }

    ULONG nFormat = 0;

    if( bDateTime )
    {
        short nDT = GetTimeDatePara(rStr, nFormat);
        switch( nDT )
        {
            case NUMBERFORMAT_DATE:
                nReg = DI_SUB_DATE;
                break;
            case NUMBERFORMAT_TIME:
                nReg = DI_SUB_TIME;
                break;
            case NUMBERFORMAT_DATETIME:
                nReg = DI_SUB_DATE;
                break;
            default:
                nReg = DI_SUB_DATE;
                break;
        }
    }

    SwDocInfoField aFld( (SwDocInfoFieldType*)
        rDoc.GetSysFldType( RES_DOCINFOFLD ), nSub|nReg, nFormat );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_Author( WW8FieldDesc*, String& )
{
        // SH: Das SwAuthorField bezeichnet nicht den urspruenglichen
        // Autor, sondern den aktuellen Benutzer, also besser ueber DocInfo
        // (#56149)
    SwDocInfoField aFld( (SwDocInfoFieldType*)
                     rDoc.GetSysFldType( RES_DOCINFOFLD ),
                     DI_CREATE|DI_SUB_AUTHOR );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_TemplName( WW8FieldDesc*, String& )
{
    SwTemplNameField aFld( (SwTemplNameFieldType*)
                     rDoc.GetSysFldType( RES_TEMPLNAMEFLD ), FF_NAME );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

// Sowohl das Datum- wie auch das Uhrzeit-Feld kann fuer Datum, fuer Uhrzeit
// oder fuer beides benutzt werden.
eF_ResT SwWW8ImplReader::Read_F_DateTime( WW8FieldDesc*pF, String& rStr )
{												// Datum/Uhrzeit - Feld
    ULONG nFormat = 0;

    short nDT = GetTimeDatePara(rStr, nFormat);

    if( NUMBERFORMAT_UNDEFINED == nDT )				// no D/T-Formatstring
    {
        if( 32 == pF->nId )
        {
            nDT		= NUMBERFORMAT_TIME;
            nFormat	= rDoc.GetNumberFormatter()->GetFormatIndex(
                        NF_TIME_START, LANGUAGE_SYSTEM );
        }
        else
        {
            nDT		= NUMBERFORMAT_DATE;
            nFormat	= rDoc.GetNumberFormatter()->GetFormatIndex(
                        NF_DATE_START, LANGUAGE_SYSTEM );
        }
    }

    if( nDT & NUMBERFORMAT_DATE )
    {
        SwDateTimeField aFld( (SwDateTimeFieldType*)
                        rDoc.GetSysFldType( RES_DATETIMEFLD ), DATEFLD, nFormat );
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    }
    else if( nDT == NUMBERFORMAT_TIME )
    {
        SwDateTimeField aFld( (SwDateTimeFieldType*)
                        rDoc.GetSysFldType( RES_DATETIMEFLD ), TIMEFLD, nFormat );
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    }

    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_FileName( WW8FieldDesc*, String& )
{
    SwFileNameField aFld( (SwFileNameFieldType*)
                          rDoc.GetSysFldType( RES_FILENAMEFLD ) );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_Anz( WW8FieldDesc* pF, String& rStr )
{												// SeitenZahl - Feld
    USHORT nSub = DS_PAGE;
    switch ( pF->nId ){
    case 27: nSub = DS_WORD; break;				// Wordzahl
    case 28: nSub = DS_CHAR; break;				// Zeichenzahl
    }
    SwDocStatField aFld( (SwDocStatFieldType*)
                         rDoc.GetSysFldType( RES_DOCSTATFLD ), nSub,
                         GetNumberPara( rStr ) );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_CurPage( WW8FieldDesc*, String& rStr )
{
    // zusaetzlich mit Kapitelnummer?
    if( bPgChpLevel )
    {
        SwChapterField aFld( (SwChapterFieldType*)
                    rDoc.GetSysFldType( RES_CHAPTERFLD ), CF_NUMBER );
        aFld.SetLevel( nPgChpLevel );
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );

        static const sal_Char aDelim[] = "-.:\x97\x96";
        BYTE nDelim = nPgChpDelim;
        if( nDelim > 4 )
            nDelim = 0;

        sal_Unicode c = ByteString::ConvertToUnicode( aDelim[ nDelim ],
                                        RTL_TEXTENCODING_MS_1252 );
        if( '-' == c )
            rDoc.Insert( *pPaM, CHAR_HARDHYPHEN );
        else
            rDoc.Insert( *pPaM, SwFmtHardBlank( c ));
    }

    // Seitennummer
    SwPageNumberField aFld( (SwPageNumberFieldType*)
        rDoc.GetSysFldType( RES_PAGENUMBERFLD ), PG_RANDOM, 
        GetNumberPara(rStr, true));

    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_Symbol( WW8FieldDesc*, String& rStr )
{                                               // Symbol-Zeichensatz
/*
    char* p;
    char* pQ = Find1stPara( pStr, &p );			// 1. Para ist Ascii-Wert
    USHORT nC = String( pQ );
    if( !nC )									// ungueltige Syntax
        return FLD_TAGIGN;						// -> kein 0-Zeichen in Text
    char* pCSet = FindPara( p, 'f', 'F' );			// \f -> Zeichensatz
    char* pSiz = FindPara( p, 's', 'S' );			// \s -> FontSize
    if( pCSet ){								// Charset angegeben ?
        String aName( pCSet );
        if( eStructCharSet != eSysCharSet )				// OS2, WIN mit Mac-Doc,...
            aName.Convert( eStructCharSet, eSysCharSet );
        SvxFontItem aFont(	FAMILY_DONTKNOW, aName, // "WingDings",
                            aEmptyStr,
                            PITCH_DONTKNOW, CHARSET_SYMBOL);
        NewAttr( aFont );              // neuer Font
    }
    if( pSiz ){									// Size angegeben ?
        ULONG nSiz = String( pSiz );			// in pt
        SvxFontHeightItem aSz( nSiz * 20 );		// pT -> twip
        NewAttr( aSz );
    }
*/
    String aQ;
    String aName;
    String aSiz;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aQ.Len() )
                aQ = aReadParam.GetResult();
            break;
        case 'f':
        case 'F':
            {
                xub_StrLen n = aReadParam.GoToTokenParam();
                if( STRING_NOTFOUND != n )
                    aName = aReadParam.GetResult();
            }
            break;
        case 's':
        case 'S':
            {
                xub_StrLen n = aReadParam.GoToTokenParam();
                if( STRING_NOTFOUND != n )
                    aSiz = aReadParam.GetResult();
            }
            break;
        }
    }
    if( !aQ.Len() )
        return FLD_TAGIGN;						// -> kein 0-Zeichen in Text

    if( aName.Len() )							// Font Name angegeben ?
    {
        SvxFontItem aFont(	FAMILY_DONTKNOW, aName, // "WingDings",
                            aEmptyStr,
                            PITCH_DONTKNOW, RTL_TEXTENCODING_SYMBOL );
        NewAttr( aFont );						// neuer Font
    }

    if( aSiz.Len() )							// Size angegeben ?
    {
        SvxFontHeightItem aSz( aSiz.ToInt32() * 20 ); // pT -> twip
        NewAttr( aSz );
    }

    rDoc.Insert( *pPaM, aQ.GetChar( 0 ) );

    if( aSiz.Len() )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_FONTSIZE );
    if( aName.Len() )
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_FONT );

    return FLD_OK;
}

// "EINBETTEN"
eF_ResT SwWW8ImplReader::Read_F_Embedd( WW8FieldDesc*, String& rStr )
{
    String sHost;

    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            sHost = aReadParam.GetResult();
            break;

        case 's':
            // use ObjectSize
            break;
        }
    }

    if( bObj && nPicLocFc )
        nObjLocFc = nPicLocFc;
    bEmbeddObj = true;
    return FLD_TEXT;
}


// "SET"
eF_ResT SwWW8ImplReader::Read_F_Set( WW8FieldDesc* pF, String& rStr )
{
    String sOrigName;
    String sVal;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !sOrigName.Len() )
                sOrigName = aReadParam.GetResult();
            else if( !sVal.Len() )
                sVal = aReadParam.GetResult();
            break;
        }
    }

    long nNo = MapBookmarkVariables(pF,sOrigName,sVal);

    SwFieldType* pFT = rDoc.InsertFldType( SwSetExpFieldType( &rDoc, sOrigName,
        GSE_STRING ) );
    SwSetExpField aFld( (SwSetExpFieldType*)pFT, sVal );
    aFld.SetSubType(SUB_INVISIBLE|GSE_STRING);

    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );

    pRefStck->SetAttr(*pPaM->GetPoint(), RES_FLTR_BOOKMARK, true, nNo);

    return FLD_OK;
}

// "REF"
eF_ResT SwWW8ImplReader::Read_F_Ref( WW8FieldDesc*, String& rStr )
{														// Reference - Field
    String sOrigBkmName;
    bool bChapterNr = false;
    bool bAboveBelow = false;

    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !sOrigBkmName.Len() ) // get name of bookmark
                sOrigBkmName = aReadParam.GetResult();
            break;
        case 'n':
        case 'r':
        case 'w':
            bChapterNr = true; // activate flag 'Chapter Number'
            break;

        case 'p':
            bAboveBelow = true;
            break;
        case 'h':
            break;
        default:
            // unimplemented switch: just do 'nix nought nothing'  :-)
            break;
        }
    }

    if ( SwFltGetFlag( nFieldFlags, SwFltControlStack::HYPO ) )
    {
        SwGetExpField aFld( (SwGetExpFieldType*)
            rDoc.GetSysFldType( RES_GETEXPFLD ),sOrigBkmName, GSE_STRING, 
            VVF_SYS );
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
        pRefStck->SetBookRef(sOrigBkmName, false);
    }
    else
    {
        const String &rBkmName = GetMappedBookmark(sOrigBkmName);

        if (!bAboveBelow || bChapterNr)
        {
            if (bChapterNr)
            {
                SwGetRefField aFld( 
                    (SwGetRefFieldType*)rDoc.GetSysFldType( RES_GETREFFLD ),
                    rBkmName,REF_BOOKMARK,0,REF_CHAPTER);
                rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
            }
            else
            {
                /*
                If we are just inserting the contents of the bookmark, then it
                is possible that the bookmark is actually a variable, so we
                must store it until the end of the document to see if it was,
                in which case we'll turn it into a show variable
                */    
                SwGetRefField aFld( 
                    (SwGetRefFieldType*)rDoc.GetSysFldType( RES_GETREFFLD ),
                    sOrigBkmName,REF_BOOKMARK,0,REF_CONTENT);
                pRefStck->NewAttr( *pPaM->GetPoint(), SwFmtFld(aFld) );
                pRefStck->SetAttr( *pPaM->GetPoint(), RES_TXTATR_FIELD);
            }
        }

        if( bAboveBelow )
        {
            SwGetRefField aFld( (SwGetRefFieldType*)
                rDoc.GetSysFldType( RES_GETREFFLD ), rBkmName, REF_BOOKMARK, 0,
                REF_UPDOWN );
            rDoc.Insert(*pPaM, SwFmtFld(aFld));
        }
    }
    return FLD_OK;
}

// Note Reference - Field
eF_ResT SwWW8ImplReader::Read_F_NoteReference( WW8FieldDesc*, String& rStr )
{
    String aBkmName;
    bool bChapterNr = false;
    bool bAboveBelow = false;

    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aBkmName.Len() ) // get name of foot/endnote
                aBkmName = aReadParam.GetResult();
            break;
        case 'r':
            bChapterNr = true; // activate flag 'Chapter Number'
            break;
        case 'p':
            bAboveBelow = true;
            break;
        case 'h':
            break;
        default:
            // unimplemented switch: just do 'nix nought nothing'  :-)
            break;
        }
    }

    if ( SwFltGetFlag( nFieldFlags, SwFltControlStack::HYPO ) )
    {
        SwGetExpField aFld( (SwGetExpFieldType*)
            rDoc.GetSysFldType( RES_GETEXPFLD ), aBkmName, GSE_STRING, VVF_SYS);
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
        pRefStck->SetBookRef(aBkmName, false);
    }
    else
    {	// set Sequence No of corresponding Foot-/Endnote to Zero
        // (will be corrected in
        SwGetRefField aFld( (SwGetRefFieldType*)
            rDoc.GetSysFldType( RES_GETREFFLD ), aBkmName, REF_FOOTNOTE, 0,
            REF_ONLYNUMBER );
        pRefStck->NewAttr(*pPaM->GetPoint(), SwFmtFld( aFld ));
        if( bAboveBelow )
        {
            SwGetRefField aFld2( (SwGetRefFieldType*)
                rDoc.GetSysFldType( RES_GETREFFLD ),aBkmName, REF_FOOTNOTE, 0,
                REF_UPDOWN );
            pRefStck->NewAttr(*pPaM->GetPoint(), SwFmtFld( aFld2 ));
        }
    }
    return FLD_OK;
}

// "SEITENREF"
eF_ResT SwWW8ImplReader::Read_F_PgRef( WW8FieldDesc*, String& rStr )
{
    String sOrigName;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !sOrigName.Len() )
                sOrigName = aReadParam.GetResult();
            break;
        }
    }
    if ( SwFltGetFlag( nFieldFlags, SwFltControlStack::HYPO ) )
    {
        SwGetRefField aFld( (SwGetRefFieldType*)
                        rDoc.GetSysFldType( RES_GETREFFLD ), sOrigName, 0, 0,
                        REF_PAGE );
        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
        pRefStck->SetBookRef(sOrigName, true);
    }
    else
    {
        const String &rName = GetMappedBookmark(sOrigName);
        
        SwGetRefField aFld( 
            (SwGetRefFieldType*)rDoc.GetSysFldType( RES_GETREFFLD ), rName,
            REF_BOOKMARK, 0, REF_PAGE );

        rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    }
    return FLD_OK;
}

// "MACROSCHALTFL"ACHE"
eF_ResT SwWW8ImplReader::Read_F_Macro( WW8FieldDesc*, String& rStr )
{
    String aName;
    String aVText;
    long nRet;
    bool bNewVText = true;
    bool bBracket  = false;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aName.Len() )
                aName = aReadParam.GetResult();
            else if( !aVText.Len() || bBracket )
            {
                if( bBracket )
                    aVText += ' ';
                aVText += aReadParam.GetResult();
                if (bNewVText)
                {
                    bBracket = aVText.EqualsIgnoreCaseAscii('[', 1, 0) 
                        ? true : false;
                    bNewVText = false;
                }
                else if( aVText.GetChar( aVText.Len()-1 ) == ']' )
                    bBracket  = false;
            }
            break;
        }
    }
    if( !aName.Len() )
        return FLD_TAGIGN;  // makes no sense without Makro-Name

    aName.InsertAscii( "StarOffice.Standard.Modul1.", 0 );

    SwMacroField aFld( (SwMacroFieldType*)
                    rDoc.GetSysFldType( RES_MACROFLD ), aName, aVText );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

// "EINF"UGENGRAFIK"
eF_ResT SwWW8ImplReader::Read_F_IncludePicture( WW8FieldDesc*, String& rStr )
{
    String aGrfName;
    bool bEmbedded = true;

    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if (!aGrfName.Len())
                ConvertFFileName(aGrfName, aReadParam.GetResult());
            break;

        case 'd':
            bEmbedded = false;			// Embedded-Flag deaktivieren
            break;

        case 'c':// den Converter-Namen ueberlesen
            aReadParam.FindNextStringPiece();
            break;
        }
    }

    if( !bEmbedded )
    {
        /*
            Besonderheit:

            Wir setzen jetzt den Link ins Doc und merken uns den SwFlyFrmFmt.
            Da wir ja unten auf jjeden Fall mit Return-Wert FLD_READ_FSPA enden,
            wird der Skip-Wert so bemessen, dass das folgende Char-1 eingelesen
            wird.
            Wenn wir dann in SwWW8ImplReader::ImportGraf() reinlaufen, wird
            erkannt, dass wir soeben einen Grafik-Link inserted haben und
            das passende SwAttrSet wird ins Frame-Format eingesetzt.
        */
        SfxItemSet aFlySet( rDoc.GetAttrPool(), RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
        aFlySet.Put( SwFmtAnchor( FLY_IN_CNTNT ) );
        aFlySet.Put( SwFmtVertOrient( 0, VERT_TOP, FRAME ));
        pFlyFmtOfJustInsertedGraphic = rDoc.Insert( *pPaM,
                                                    aGrfName,
                                                    aEmptyStr,
                                                    0,			// Graphic*
                                                    &aFlySet,
                                                    0);			// SwFrmFmt*
        aGrfNameGenerator.SetUniqueGraphName(pFlyFmtOfJustInsertedGraphic,
            INetURLObject(aGrfName).GetBase());
    }
    return FLD_READ_FSPA;
}

// "EINFUEGENTEXT"
eF_ResT SwWW8ImplReader::Read_F_IncludeText( WW8FieldDesc* pF, String& rStr )
{
    String aPara;
    String aBook;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aPara.Len() )
                aPara = aReadParam.GetResult();
            else if( !aBook.Len() )
                aBook = aReadParam.GetResult();
            break;
        case '*':
            //Skip over MERGEFORMAT
            aReadParam.SkipToNextToken();
            break;
        }
    }
    ConvertFFileName(aPara, aPara);

    if( aBook.Len() && aBook.GetChar( 0 ) != '\\' )
    {
        // Bereich aus Quelle ( kein Switch ) ?
        ConvertUFName( aBook );
        aPara += so3::cTokenSeperator;
        aPara += so3::cTokenSeperator;
        aPara += aBook;
    }
    String aStr(CREATE_CONST_ASC( "WW" ));

    /*
    ##509##
    What we will do is insert a section to be linked to a file, but just in
    case the file is not available we will fill in the section with the stored
    content of this winword field as a fallback.
    */
    WW8ReaderSave aSave( this );

    SwSection aSection( FILE_LINK_SECTION, rDoc.GetUniqueSectionName( &aStr ) );
    aSection.SetLinkFileName( aPara );
    aSection.SetProtect(true);

    pNewSection = rDoc.Insert(*pPaM, aSection, 0 ,false);

    const SwSectionNode* pSectionNode =	pNewSection->GetFmt()->GetSectionNode();
    ASSERT(!pAfterSection, "recursive sections!");
    pAfterSection = new SwNodeIndex( *pSectionNode->EndOfSectionNode(), 1 );

    pPaM->GetPoint()->nNode = pSectionNode->GetIndex()+1;
    pPaM->GetPoint()->nContent.Assign(pPaM->GetCntntNode(), 0 );

    bTxbxFlySection = true;
    ReadText(pF->nSRes, pF->nLRes, pPlcxMan->GetManType());

    aSave.Restore( this );

    return FLD_OK;
}

// "SERIENDRUCKFELD"
eF_ResT SwWW8ImplReader::Read_F_DBField( WW8FieldDesc*, String& rStr )
{
    String aName;
    long nRet;
    _ReadFieldParams aReadParam( rStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            if( !aName.Len() )
                aName = aReadParam.GetResult();
            break;
        }
    }
    SwDBFieldType aD( &rDoc, aName, SwDBData() );	// Datenbank: Nichts
    SwFieldType* pFT = rDoc.InsertFldType( aD );
    SwDBField aFld( (SwDBFieldType*)pFT );
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

// "N"ACHSTER"
eF_ResT SwWW8ImplReader::Read_F_DBNext( WW8FieldDesc*, String& )
{
    SwDBNextSetFieldType aN;
    SwFieldType* pFT = rDoc.InsertFldType( aN );
    SwDBNextSetField aFld( (SwDBNextSetFieldType*)pFT, aEmptyStr, aEmptyStr,
                            SwDBData() );		// Datenbank: Nichts
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

// "DATENSATZ"
eF_ResT SwWW8ImplReader::Read_F_DBNum( WW8FieldDesc*, String& )
{
    SwDBSetNumberFieldType aN;
    SwFieldType* pFT = rDoc.InsertFldType( aN );
    SwDBSetNumberField aFld( (SwDBSetNumberFieldType*)pFT,
                           SwDBData() );			// Datenbank: Nichts
    rDoc.Insert( *pPaM, SwFmtFld( aFld ) );
    return FLD_OK;
}

/*
    EQ , only the usage for
    a. Combined Characters supported, must be exactly in the form that word
    only accepts as combined charactersm, i.e.
    eq \o(\s\up Y(XXX),\s\do Y(XXX))
    b. Ruby Text supported, must be in the form that word recognizes as being
    ruby text
    ...
*/
eF_ResT SwWW8ImplReader::Read_F_Equation( WW8FieldDesc*, String& rStr )
{
    _ReadFieldParams aReadParam( rStr );
    long cChar = aReadParam.SkipToNextToken();
    if ('o' == cChar)
        Read_SubF_Combined(aReadParam);
    else if ('*' == cChar)
        Read_SubF_Ruby(aReadParam);
    return FLD_OK;
}

void SwWW8ImplReader::Read_SubF_Combined( _ReadFieldParams& rReadParam)
{
    String sCombinedCharacters;
    if ((-2 == rReadParam.SkipToNextToken()) &&
            rReadParam.GetResult().EqualsIgnoreCaseAscii('(', 1, 0))
    {
        for (int i=0;i<2;i++)
        {
            if ('s' == rReadParam.SkipToNextToken())
            {
                long cChar = rReadParam.SkipToNextToken();
                if (-2 != rReadParam.SkipToNextToken())
                    break;
                String sF = rReadParam.GetResult();
                if ((('u' == cChar) && sF.EqualsIgnoreCaseAscii('p', 1, 0))
                || (('d' == cChar) && sF.EqualsIgnoreCaseAscii('o', 1, 0)))
                {
                    if (-2 == rReadParam.SkipToNextToken())
                    {
                        String sPart = rReadParam.GetResult();
                        xub_StrLen nBegin = sPart.Search('(');

                        //Word disallows brackets in this field, which
                        //aids figuring out the case of an end of )) vs )
                        xub_StrLen nEnd = sPart.Search(')');

                        if ((nBegin != STRING_NOTFOUND) &&
                            (nEnd != STRING_NOTFOUND))
                        {
                            sCombinedCharacters +=
                                sPart.Copy(nBegin+1,nEnd-nBegin-1);
                        }
                    }
                }
            }
        }
    }
    if (sCombinedCharacters.Len())
    {
        SwCombinedCharField aFld((SwCombinedCharFieldType*)
            rDoc.GetSysFldType(RES_COMBINED_CHARS),sCombinedCharacters);
        rDoc.Insert(*pPaM,SwFmtFld(aFld));
    }
}

void SwWW8ImplReader::Read_SubF_Ruby( _ReadFieldParams& rReadParam)
{
    USHORT nJustificationCode=0;
    String sFontName;
    UINT32 nFontSize=0;
    String sRuby;
    String sText;
    long nRet;
    while( -1 != ( nRet = rReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
        case -2:
            {
                String sTemp = rReadParam.GetResult();
                if( sTemp.EqualsIgnoreCaseAscii( "jc", 0, 2 ) )
                {
                    sTemp.Erase(0,2);
                    nJustificationCode = static_cast<USHORT>(sTemp.ToInt32());
                }
                else if( sTemp.EqualsIgnoreCaseAscii( "hps", 0, 3 ) )
                {
                    sTemp.Erase(0,3);
                    nFontSize= static_cast<UINT32>(sTemp.ToInt32());
                }
                else if( sTemp.EqualsIgnoreCaseAscii( "Font:", 0, 5 ) )
                {
                    sTemp.Erase(0,5);
                    sFontName = sTemp;
                }
            }
            break;
        case '*':
            break;
        case 'o':
            while( -1 != ( nRet = rReadParam.SkipToNextToken() ))
            {
                if ('u' == nRet)
                {
                    if (-2 == rReadParam.SkipToNextToken() &&
                      (rReadParam.GetResult().EqualsIgnoreCaseAscii('p', 1, 0)))
                    {
                        if (-2 == rReadParam.SkipToNextToken())
                        {
                            String sPart = rReadParam.GetResult();
                            xub_StrLen nBegin = sPart.Search('(');

                            //Word disallows brackets in this field,
                            xub_StrLen nEnd = sPart.Search(')');

                            if ((nBegin != STRING_NOTFOUND) &&
                                (nEnd != STRING_NOTFOUND))
                            {
                                sRuby = sPart.Copy(nBegin+1,nEnd-nBegin-1);
                            }
                            if (STRING_NOTFOUND ==
                                (nBegin = sPart.Search(',',nEnd)))
                            {
                                nBegin = sPart.Search(';',nEnd);
                            }
                            nEnd = sPart.SearchBackward(')');
                            if ((nBegin != STRING_NOTFOUND) &&
                                (nEnd != STRING_NOTFOUND))
                            {
                                sText = sPart.Copy(nBegin+1,nEnd-nBegin-1);
                            }
                        }
                    }
                }

            }
            break;
        }
    }

    //Translate and apply
    if (sRuby.Len() && sText.Len() && sFontName.Len() && nFontSize)
    {
        switch (nJustificationCode)
        {
            case 0:
                nJustificationCode=1;
                break;
            case 1:
                nJustificationCode=3;
                break;
            case 2:
                nJustificationCode=4;
                break;
            default:
            case 3:
                nJustificationCode=0;
                break;
            case 4:
                nJustificationCode=2;
                break;
        }

        SwFmtRuby aRuby(sRuby);
        const SwCharFmt *pCharFmt=0;
        //Make a guess at which of asian of western we should be setting
        USHORT nScript;
        if (pBreakIt->xBreak.is())
            nScript = pBreakIt->xBreak->getScriptType(sRuby, 0);
        else
            nScript = com::sun::star::i18n::ScriptType::ASIAN;

        //Check to see if we already have a ruby charstyle that this fits
        for(std::vector<const SwCharFmt*>::const_iterator aIter
            = aRubyCharFmts.begin(); aIter != aRubyCharFmts.end(); ++aIter)
        {
            const SvxFontHeightItem &rFH =
                (const SvxFontHeightItem &)((*aIter)->GetAttr(
                GetWhichOfScript(RES_CHRATR_FONTSIZE,nScript)));
            if (rFH.GetHeight() == nFontSize*10)
            {
                const SvxFontItem &rF =
                    (const SvxFontItem &)((*aIter)->GetAttr(
                    GetWhichOfScript(RES_CHRATR_FONT,nScript)));
                if (rF.GetFamilyName().Equals(sFontName))
                {
                    pCharFmt=*aIter;
                    break;
                }
            }
        }

        //Create a new char style if necessary
        if (!pCharFmt)
        {
            SwCharFmt *pFmt=0;
            String aNm;
            //Take this as the base name
            SwStyleNameMapper::FillUIName(RES_POOLCHR_RUBYTEXT,aNm);
            aNm+=String::CreateFromInt32(aRubyCharFmts.size()+1);
            pFmt = rDoc.MakeCharFmt(aNm,(SwCharFmt*)rDoc.GetDfltCharFmt());
            SvxFontHeightItem aHeightItem(nFontSize*10);
            SvxFontItem aFontItem(FAMILY_DONTKNOW,sFontName,
                aEmptyStr,PITCH_DONTKNOW,RTL_TEXTENCODING_DONTKNOW);
            aHeightItem.SetWhich(GetWhichOfScript(RES_CHRATR_FONTSIZE,nScript));
            aFontItem.SetWhich(GetWhichOfScript(RES_CHRATR_FONT,nScript));
            pFmt->SetAttr(aHeightItem);
            pFmt->SetAttr(aFontItem);
            aRubyCharFmts.push_back(pFmt);
            pCharFmt = pFmt;
        }

        //Set the charstyle and justification
        aRuby.SetCharFmtName(pCharFmt->GetName());
        aRuby.SetCharFmtId(pCharFmt->GetPoolFmtId());
        aRuby.SetAdjustment(nJustificationCode);

        NewAttr(aRuby);
        rDoc.Insert( *pPaM, sText );
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_TXTATR_CJK_RUBY );
    }
}

//-----------------------------------------
//		  Verzeichnis-Felder
//-----------------------------------------

void lcl_toxMatchACSwitch(	SwWW8ImplReader& rReader,
                            SwDoc& rDoc,
                            SwTOXBase& rBase,
                            _ReadFieldParams& rParam,
                            SwCaptionDisplay eCaptionType)
{
    xub_StrLen n = rParam.GoToTokenParam();
    if( STRING_NOTFOUND != n )
    {
        SwTOXType* pType = (SwTOXType*)rDoc.GetTOXType( TOX_ILLUSTRATIONS, 0);
        pType->Add( &rBase );
        rBase.SetCaptionDisplay( eCaptionType );
        // Read Sequence Name and store in TOXBase
        String sSeqName( rParam.GetResult() );
        lcl_ConvertSequenceName( rReader, sSeqName );
        rBase.SetSequenceName( sSeqName );
    }
}


void lcl_toxMatchTSwitch(SwWW8ImplReader& rReader, SwTOXBase& rBase,
    _ReadFieldParams& rParam)
{
    xub_StrLen n = rParam.GoToTokenParam();
    if( STRING_NOTFOUND != n )
    {
        String sParams( rParam.GetResult() );
        if( sParams.Len() )
        {
            xub_StrLen nIndex = 0;

            //#92940# Delimiters between styles and style levels appears to
            //allow both ; and ,

            String sTemplate( sParams.GetToken(0, ';', nIndex) );
            if( STRING_NOTFOUND == nIndex )
            {
                nIndex=0;
                sTemplate = sParams.GetToken(0, ',', nIndex);
            }
            if( STRING_NOTFOUND == nIndex )
            {
                const SwFmt* pStyle = rReader.GetStyleWithOrgWWName(sTemplate);
                if( pStyle )
                    sTemplate = pStyle->GetName();
                // Store Style for Level 0 into TOXBase
                rBase.SetStyleNames( sTemplate, 0 );
            }
            else while( STRING_NOTFOUND != nIndex )
            {
                xub_StrLen nOldIndex=nIndex;
                USHORT nLevel = static_cast<USHORT>(
                    sParams.GetToken(0, ';', nIndex).ToInt32());
                if( STRING_NOTFOUND == nIndex )
                {
                    nIndex = nOldIndex;
                    nLevel = static_cast<USHORT>(
                        sParams.GetToken(0, ',', nIndex).ToInt32());
                }

                if( (0 < nLevel) && (MAXLEVEL >= nLevel) )
                {
                    nLevel--;
                    // Store Style and Level into TOXBase
                    const SwFmt* pStyle
                            = rReader.GetStyleWithOrgWWName( sTemplate );

                    if( pStyle )
                        sTemplate = pStyle->GetName();

                    String sStyles( rBase.GetStyleNames( nLevel ) );
                    if( sStyles.Len() )
                        sStyles += TOX_STYLE_DELIMITER;
                    sStyles += sTemplate;
                    rBase.SetStyleNames( sStyles, nLevel );
                }
                // read next style name...
                nOldIndex = nIndex;
                sTemplate = sParams.GetToken(0, ';', nIndex);
                if( STRING_NOTFOUND == nIndex )
                {
                    nIndex=nOldIndex;
                    sTemplate = sParams.GetToken(0, ',', nIndex);
                }
            }
        }
    }
}

eF_ResT SwWW8ImplReader::Read_F_Tox( WW8FieldDesc* pF, String& rStr )
{
    if( nIniFlags & WW8FL_NO_TOX )
        return FLD_OK;			// abgeschaltet -> ignorieren
    if( ( pF->nLRes < 3 ) )
        return FLD_TAGIGN;		// Nur Stuss -> ignorieren

    TOXTypes eTox;								// Baue ToxBase zusammen
    switch( pF->nId )
    {
        case  8: 
            eTox = TOX_INDEX; 
            break;
        case 13: 
            eTox = TOX_CONTENT; 
            break;
        default: 
            eTox = TOX_USER; 
            break;
    }

    USHORT nCreateOf = (eTox == TOX_CONTENT) ? TOX_OUTLINELEVEL : TOX_MARK;

    USHORT nIndexCols = 0;
    SwSection *pTest = rDoc.GetCurrSection(*pPaM->GetPoint());
    if (pTest)  //section is open, set to its no of section cols
    {
        const SwSectionFmt *pFmt = pTest->GetFmt();
        if (pFmt)
            nIndexCols = pFmt->GetCol().GetNumCols();
    }
    else if (pPageDesc)  //set to current number of page cols
        nIndexCols = pPageDesc->GetMaster().GetCol().GetNumCols();

    const SwTOXType* pType = rDoc.GetTOXType( eTox, 0 );
    SwForm aOrigForm(eTox);
    SwTOXBase* pBase = new SwTOXBase( pType, aOrigForm, nCreateOf, aEmptyStr );
                                // Name des Verzeichnisses
    switch( eTox ){
    case TOX_INDEX:
        {
            USHORT eOptions = TOI_SAME_ENTRY | TOI_CASE_SENSITIVE;


            // TOX_OUTLINELEVEL setzen wir genau dann, wenn
            // die Parameter \o in 1 bis 9 liegen
            // oder der Parameter \f existiert
            // oder GARKEINE Switches Parameter angegeben sind.
            long nRet;
            _ReadFieldParams aReadParam( rStr );
            while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
            {
                switch( nRet )
                {
                case 'c':
                    {
                        xub_StrLen n = aReadParam.GoToTokenParam();
                        if( STRING_NOTFOUND != n )
                        {
                            String sParams( aReadParam.GetResult() );
                            // if NO String just ignore the \c
                            if( sParams.Len() )
                            {
                                nIndexCols =
                                    static_cast<USHORT>(sParams.ToInt32());
                            }
                        }
                    }
                    break;
                case 'e':
                    {
                        xub_StrLen n = aReadParam.GoToTokenParam();
                        if( STRING_NOTFOUND != n )  // if NO String just ignore the \e
                        {
                            String sDelimiter( aReadParam.GetResult() );
                            SwForm aForm( pBase->GetTOXForm() );

                            // Attention: if TOX_CONTENT brave
                            //            GetFormMax() returns MAXLEVEL + 1  !!
                            USHORT nEnd = aForm.GetFormMax()-1;

                            for(USHORT nLevel = 1;
                                   nLevel <= nEnd;
                                   ++nLevel)
                            {
                                // Levels count from 1
                                // Level 0 is reserved for CAPTION

                                // Delimiter statt Tabstop vor der Seitenzahl einsetzen,
                                // falls es eine Seitenzahl gibt:
                                FormTokenType ePrevType = TOKEN_END;
                                FormTokenType eType;
                                SwFormTokenEnumerator aEnumer =
                                    aForm.CreateTokenEnumerator( nLevel );
                                do
                                {
                                    eType = aEnumer.GetNextTokenType();
                                    switch( eType )
                                    {
                                        case TOKEN_PAGE_NUMS:
                                        {
                                            if( TOKEN_TAB_STOP == ePrevType )
                                            {
                                                // remove Tab
                                                aEnumer.GetPrevTokenType();
                                                aEnumer.RemoveCurToken();
                                                // insert new Token
                                                if( 0x09 == sDelimiter.GetChar( 0 ) )
                                                {
                                                    SwFormToken aToken( TOKEN_TAB_STOP );
                                                    aToken.eTabAlign = SVX_TAB_ADJUST_END;
                                                    aEnumer.InsertToken( aToken );
                                                }
                                                else
                                                {
                                                    SwFormToken aToken( TOKEN_TEXT );
                                                    aToken.sText = sDelimiter;
                                                    aEnumer.InsertToken( aToken );
                                                }
                                                aForm.SetPattern( nLevel,
                                                        aEnumer.GetPattern() );
                                            }
                                            eType = TOKEN_END;
                                            break;
                                        }
                                    }
                                    ePrevType = eType;
                                }
                                while( TOKEN_END != eType );
                            }
                            pBase->SetTOXForm( aForm );
                        }
                    }
                    break;
                case 'h':
                    {
                        eOptions |= TOI_ALPHA_DELIMITTER;
                    }
                    break;
                case 'r':
                    {
                        SwForm aForm( pBase->GetTOXForm() );
                        aForm.SetCommaSeparated(true);
                        pBase->SetTOXForm( aForm );
                    }
                    break;
                /*
                // the following switches are not (yet) supported
                // by good old StarWriter:
                case 'b':
                case 'd':
                case 'f':
                case 'g':
                case 'I':
                case 'p':
                case 's':
                    break;
                */
                }
            }
            /*
            const TOXTypes eType = pBase->GetTOXType()->GetType();
            switch( eType )
            {
                case TOX_CONTENT:
                    if( eCreateFrom )
                        pBase->SetCreate( eCreateFrom );
                    break;
                case TOX_ILLUSTRATIONS:
                    if( !eCreateFrom )
                        eCreateFrom = TOX_SEQUENCE;
                    pBase->SetCreate( eCreateFrom );
                    break;
            }
            */
            pBase->SetOptions( eOptions );
        }
        break;





    case TOX_CONTENT:
        {
            // TOX_OUTLINELEVEL setzen wir genau dann, wenn
            // die Parameter \o in 1 bis 9 liegen
            // oder der Parameter \f existiert
            // oder GARKEINE Switches Parameter angegeben sind.
            USHORT eCreateFrom = 0;
            USHORT nMaxLevel = 0;
            long nRet;
            _ReadFieldParams aReadParam( rStr );
            while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
            {
                switch( nRet )
                {
                case 'a':
                case 'c':
                    lcl_toxMatchACSwitch(*this, rDoc, *pBase, aReadParam,
                                           ('c' == nRet)
                                         ? CAPTION_COMPLETE
                                         : CAPTION_TEXT );
                    break;
                case 'o':
                    {
                        USHORT nVal;
                        if( !aReadParam.GetTokenSttFromTo(0, &nVal, MAXLEVEL) )
                            nVal = aOrigForm.GetFormMax()-1;
                        if( nMaxLevel < nVal )
                            nMaxLevel = nVal;
                        eCreateFrom |= TOX_OUTLINELEVEL;
                    }
                    break;
                case 'f':
                    eCreateFrom |= TOX_MARK;
                    break;
                case 'l':
                    {
                        USHORT nVal;
                        if( aReadParam.GetTokenSttFromTo(0, &nVal, MAXLEVEL) )
                        {
                            if( nMaxLevel < nVal )
                                nMaxLevel = nVal;
                            eCreateFrom |= TOX_MARK;
                        }
                    }
                    break;
                case 't': // paragraphs using special styles shall
                          // provide the TOX's content
                    lcl_toxMatchTSwitch(*this, *pBase, aReadParam);
                    eCreateFrom |= TOX_TEMPLATE;
                    break;
                case 'p':
                    {
                        xub_StrLen n = aReadParam.GoToTokenParam();
                        if( STRING_NOTFOUND != n )  // if NO String just ignore the \p
                        {
                            String sDelimiter( aReadParam.GetResult() );
                            SwForm aForm( pBase->GetTOXForm() );

                            // Attention: if TOX_CONTENT brave
                            //            GetFormMax() returns MAXLEVEL + 1  !!
                            USHORT nEnd = aForm.GetFormMax()-1;

                            for(USHORT nLevel = 1;
                                   nLevel <= nEnd;
                                   ++nLevel)
                            {
                                // Levels count from 1
                                // Level 0 is reserved for CAPTION

                                // Delimiter statt Tabstop vor der Seitenzahl einsetzen,
                                // falls es eine Seitenzahl gibt:
                                FormTokenType ePrevType = TOKEN_END;
                                FormTokenType eType;
                                SwFormTokenEnumerator aEnumer =
                                    aForm.CreateTokenEnumerator( nLevel );
                                do
                                {
                                    eType = aEnumer.GetNextTokenType();
                                    switch( eType )
                                    {
                                        case TOKEN_PAGE_NUMS:
                                        {
                                            if( TOKEN_TAB_STOP == ePrevType )
                                            {
                                                // remove Tab
                                                aEnumer.GetPrevTokenType();
                                                aEnumer.RemoveCurToken();
                                                // insert new Token
                                                SwFormToken aToken( TOKEN_TEXT );
                                                aToken.sText = sDelimiter;

                                                aEnumer.InsertToken( aToken );
                                                aForm.SetPattern( nLevel,
                                                        aEnumer.GetPattern() );
                                            }
                                            eType = TOKEN_END;
                                            break;
                                        }
                                    }
                                    ePrevType = eType;
                                }
                                while( TOKEN_END != eType );
                            }
                            pBase->SetTOXForm( aForm );
                        }
                    }
                    break;
                case 'n': // don't print page numbers
                    {
                        // read START and END param
                        USHORT nStart, nEnd;
                        if( !aReadParam.GetTokenSttFromTo(	&nStart, &nEnd,
                            MAXLEVEL ) )
                        {
                            nStart = 1;
                            nEnd = aOrigForm.GetFormMax()-1;
                        }
                        // remove page numbers from this levels
                        SwForm aForm( pBase->GetTOXForm() );
                        if( aForm.GetFormMax() <= nEnd)
                            nEnd = aForm.GetFormMax()-1;
                        for(USHORT nLevel = nStart;
                               nLevel <= nEnd;
                               ++nLevel)
                        {
                            // Levels count from 1
                            // Level 0 is reserved for CAPTION

                            // Seitenzahl und ggfs. davorstehenden Tabstop entfernen:
                            FormTokenType eType;
                            SwFormTokenEnumerator aEnumer =
                                aForm.CreateTokenEnumerator( nLevel );
                            do
                            {
                                eType = aEnumer.GetNextTokenType();
                                switch( eType )
                                {
                                    case TOKEN_PAGE_NUMS:
                                    {
                                        aEnumer.RemoveCurToken();
                                        if( TOKEN_TAB_STOP
                                            == aEnumer.GetPrevTokenType() )
                                        {
                                            aEnumer.RemoveCurToken();
                                            aForm.SetPattern( nLevel,
                                                    aEnumer.GetPattern() );
                                        }
                                        eType = TOKEN_END;
                                        break;
                                    }
                                }
                            }
                            while( TOKEN_END != eType );
                        }
                        pBase->SetTOXForm( aForm );
                    }
                    break;

                /*
                // the following switches are not (yet) supported
                // by good old StarWriter:
                case 'b':
                case 's':
                case 'd':
                    break;
                */
                }
            }
            if( !nMaxLevel )
                nMaxLevel = MAXLEVEL;
            pBase->SetLevel( nMaxLevel );

            const TOXTypes eType = pBase->GetTOXType()->GetType();
            switch( eType )
            {
                case TOX_CONTENT:
                    if( eCreateFrom )
                        pBase->SetCreate( eCreateFrom );
                    break;
                case TOX_ILLUSTRATIONS:
                    {
                        if( !eCreateFrom )
                            eCreateFrom = TOX_SEQUENCE;
                        pBase->SetCreate( eCreateFrom );

                        /*
                        #91214#
                        We don't know until here if we are an illustration
                        or not, and so have being used a TOX_CONTENT so far
                        which has 10 levels, while TOX has only two, this
                        level is set only in the constructor of SwForm, so
                        create a new one and copy over anything that could
                        be set in the old one, and remove entries from the
                        pattern which do not apply to illustration indices
                        */
                        SwForm aOldForm( pBase->GetTOXForm() );
                        SwForm aForm( eType );
                        USHORT nEnd = aForm.GetFormMax()-1;

                        for(USHORT nLevel = 1; nLevel <= nEnd; ++nLevel)
                        {

                            SwFormTokenEnumerator aTokenEnum(aOldForm.
                                GetPattern(nLevel));
                            while (aTokenEnum.HasNextToken())
                            {
                                if (aTokenEnum.GetNextTokenType() ==
                                        TOKEN_ENTRY_NO)
                                {
                                    aTokenEnum.RemoveCurToken();
                                }
                            }
                            aForm.SetPattern(nLevel, aTokenEnum.GetPattern());

                            aForm.SetTemplate( nLevel,
                                aOldForm.GetTemplate(nLevel));
                        }

                        pBase->SetTOXForm( aForm );
                    }
                    break;
            }
        }
        break;
    case TOX_USER:
        break;
    } // ToxBase fertig

    // Update fuer TOX anstossen
    rDoc.SetUpdateTOX(true);

    const SwPosition* pPos = pPaM->GetPoint();

    SwFltTOX aFltTOX( pBase, nIndexCols );

    // test if there is already a break item on this node
    SwCntntNode* pNd = pPos->nNode.GetNode().GetCntntNode();
    if( pNd )
    {
        const SfxItemSet* pSet = pNd->GetpSwAttrSet();
        if( pSet )
        {
            if (SFX_ITEM_SET == pSet->GetItemState(RES_BREAK, false))
                aFltTOX.SetHadBreakItem(true);
            if (SFX_ITEM_SET == pSet->GetItemState(RES_PAGEDESC, false))
                aFltTOX.SetHadPageDescItem(true);
        }
    }

    // Setze Anfang in Stack
    pRefStck->NewAttr( *pPos, aFltTOX );
    if( 1 < nIndexCols )
        bDontCreateSep = true;

    rDoc.InsertTableOf(*pPaM->GetPoint(), *aFltTOX.GetBase(), false);

    // Setze Ende in Stack
    pRefStck->SetAttr( *pPos, RES_FLTR_TOX );

    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_Hyperlink( WW8FieldDesc* pF, String& rStr )
{
    eF_ResT eRet = FLD_OK;
    String sURL, sTarget, sMark;
    bool bDataImport = false;

    rStr.EraseTrailingChars( 1 );

    if (!bDataImport)
    {
        long nRet;
        _ReadFieldParams aReadParam( rStr );
        while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
            switch( nRet )
            {
            case -2:
                if (!sURL.Len())
                    ConvertFFileName(sURL, aReadParam.GetResult());
                break;

            case 'n':
                sTarget.ASSIGN_CONST_ASC( "_blank" );
                break;

            case 'l':
                nRet = aReadParam.SkipToNextToken();
                if( -2 == nRet )
                {
                    sMark = aReadParam.GetResult();
                    if( sMark.Len() && '"' == sMark.GetChar( sMark.Len()-1 ))
                        sMark.Erase( sMark.Len() - 1 );

                }
                break;

            case 'h':
            case 'm':
            case 's':
            case 't':
                ASSERT( !this, "Auswertung fehlt noch - Daten unbekannt" );
                break;
            }
    }

    // das Resultat uebernehmen
    String sDef(GetFieldResult(pF));
    ASSERT( (sURL.Len() || sMark.Len()) && sDef.Len(), "WW8: Empty URL" )

    if( sMark.Len() )
        ( sURL += INET_MARK_TOKEN ) += sMark;

    SwFmtINetFmt aURL( sURL, sTarget );

    //As an attribute this needs to be closed, and that'll happen from
    //EndExtSprm in conjunction with the maFieldStack If there are are flyfrms
    //between the start and begin, their hyperlinks will be set at that time
    //as well.
    pCtrlStck->NewAttr( *pPaM->GetPoint(), aURL );
    return FLD_TEXT;
}


void SwWW8ImplReader::ImportTox( int nFldId, String aStr )
{
    bool bIdx = ( nFldId != 9 );
    TOXTypes eTox = ( !bIdx ) ? TOX_CONTENT : TOX_INDEX;	// Default

    USHORT nLevel = 1;

    xub_StrLen n;
    String sFldTxt;
    long nRet;
    _ReadFieldParams aReadParam( aStr );
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
        switch( nRet )
        {
        case -2:
            if( !sFldTxt.Len() )
            {
                // PrimaryKey ohne ":", 2nd dahinter
                sFldTxt = aReadParam.GetResult();
            }
            break;

        case 'f':
            n = aReadParam.GoToTokenParam();
            if( STRING_NOTFOUND != n )
            {
                String sParams( aReadParam.GetResult() );
                if( 'C' != sParams.GetChar(0) && 'c' != sParams.GetChar(0) )
                    eTox = TOX_USER;
            }
            break;

        case 'l':
            n = aReadParam.GoToTokenParam();
            if( STRING_NOTFOUND != n )
            {
                String sParams( aReadParam.GetResult() );
                if(    sParams.Len() // if NO String just ignore the \l
                    && sParams.GetChar( 0 ) > '0'
                    && sParams.GetChar( 0 ) <= '9' )
                {
                    nLevel = (USHORT)sParams.ToInt32();
                }
            }
            break;
        }

    ASSERT( rDoc.GetTOXTypeCount( eTox ), "Doc.GetTOXTypeCount() == 0  :-(" );

    const SwTOXType* pT = rDoc.GetTOXType( eTox, 0 );
    SwTOXMark aM( pT );

    if( eTox != TOX_INDEX )
        aM.SetLevel( nLevel );
    else
    {
        xub_StrLen nFnd = sFldTxt.Search( WW8_TOX_LEVEL_DELIM );
        if( STRING_NOTFOUND != nFnd )  // it exist levels
        {
            aM.SetPrimaryKey( sFldTxt.Copy( 0, nFnd ) );
            xub_StrLen nScndFnd =
                sFldTxt.Search( WW8_TOX_LEVEL_DELIM, nFnd+1 );
            if( STRING_NOTFOUND != nScndFnd )
            {
                aM.SetSecondaryKey(  sFldTxt.Copy( nFnd+1, nScndFnd - nFnd - 1 ));
                nFnd = nScndFnd;
            }
            sFldTxt.Erase( 0, nFnd+1 );
        }
    }

    aM.SetAlternativeText( sFldTxt );

    if( !aM.IsAlternativeText() )
    {
        pPaM->SetMark();
        pPaM->GetMark()->nContent += aStr.Len();
    }
    rDoc.Insert( *pPaM, aM );
    if( !aM.IsAlternativeText() )
        pPaM->DeleteMark();
}

void SwWW8ImplReader::Read_FldVanish( USHORT, const BYTE*, short nLen )
{
    const int nChunk = 64;  //number of characters to read at one time
    //Meaningless in a style
    if (pAktColl)
        return;

    // Vorsicht: Bei Feldnamen mit Umlauten geht das MEMICMP nicht!
    const static sal_Char *aFldNames[] = {	"\x06""INHALT", "\x02""XE",	// dt.
                                            "\x02""TC"  };				// us
    const static BYTE  aFldId[] = { 9, 4, 9 };

    if( nIniFlags & WW8FL_NO_FLD )
        return;

    if( nLen < 0 )
    {
        bIgnoreText = false;
        return;
    }

    // our methode was called from
    // ''Skip attributes of field contents'' loop within ReadTextAttr()
    if( bIgnoreText )
        return;

    bIgnoreText = true;
    long nOldPos = pStrm->Tell();

    WW8_CP nStartCp = pSBase->WW8Fc2Cp( nOldPos );
    String sFieldName;
    USHORT nFieldLen = pSBase->WW8ReadString( *pStrm, sFieldName, nStartCp, 
        nChunk, eStructCharSet );
    nStartCp+=nFieldLen;

    xub_StrLen nC = 0;
    //If the first chunk did not start with a field start then
    //reset the stream position and give up
    if( !nFieldLen || (0x13 != sFieldName.GetChar( nC ))) // Field Start Mark
    {
        // If Field End Mark found
        if( nFieldLen && (0x15 == sFieldName.GetChar( nC )))
            bIgnoreText = false;
        pStrm->Seek( nOldPos );
        return;					// kein Feld zu finden
    }

    xub_StrLen nFnd;
    //If this chunk does not contain a field end, keep reading chunks
    //until we find one, or we run out of text,
    while (STRING_NOTFOUND == (nFnd = sFieldName.Search(0x15)))
    {
        String sTemp;
        nFieldLen = pSBase->WW8ReadString( *pStrm, sTemp,
        nStartCp, nChunk, eStructCharSet );
        sFieldName+=sTemp;
        nStartCp+=nFieldLen;
        if (!nFieldLen)
            break;
    }

    pStrm->Seek( nOldPos );

    //if we have no 0x15 give up, otherwise erase everything from the 0x15
    //onwards
    if (STRING_NOTFOUND == nFnd)
        return;
    else
        sFieldName.Erase(nFnd);

    nC++;
    while( ' '  == sFieldName.GetChar( nC ))
        nC++;

    for( int i = 0; i < 3; i++ )
    {
        const sal_Char* pName = aFldNames[i];
        USHORT nNameLen = *pName++;
        if( sFieldName.EqualsIgnoreCaseAscii( pName, nC, nNameLen ) )
        {
            ImportTox( aFldId[i], sFieldName.Copy( nC + nNameLen ) );
            break;					// keine Mehrfachnennungen moeglich
        }
    }
    bIgnoreText = true;
    pStrm->Seek( nOldPos );
}

// Read_Invisible ist fuer das "Unsichtbar"-Zeichenattribut. Mangels
// entsprechender Funktion im SW evtl. als Tag.
//
// ACHTUNG: Methode gelegentlich umstellen: unsichtbaren Text als
//					*Feld* integrieren...
//
void SwWW8ImplReader::Read_Invisible( USHORT, const BYTE*, short nLen )
{
    USHORT n = WWF_INVISIBLE;				// Bit-Nummer fuer Invisible
    USHORT nI = n / 32;					  	// # des UINT32
    ULONG nMask = 1 << ( n % 32 );	// Maske fuer Bits

    if ( (nFieldTagBad[nI] & nMask) || (nFieldTagAlways[nI] & nMask) )
    {
        String aTag(CREATE_CONST_ASC("{INVISIBLE "));

        if( nLen < 0 )
            aTag.APPEND_CONST_ASC("END}");
        else
            aTag.APPEND_CONST_ASC("START}");

        InsertTagField( n, aTag );
    }
}
