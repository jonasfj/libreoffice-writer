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


#include <com/sun/star/i18n/ScriptType.hpp>
#include <tools/string.hxx>
#include <svtools/htmlkywd.hxx>
#include <svtools/htmlout.hxx>
#include <svtools/htmltokn.h>
#include <fmtfld.hxx>
#include <doc.hxx>
#include <breakit.hxx>
#include <ndtxt.hxx>
#include <txtfld.hxx>
#include "fldbas.hxx"
#include "docufld.hxx"
#include "flddat.hxx"
#include "htmlfld.hxx"
#include "wrthtml.hxx"

using namespace nsSwDocInfoSubType;

const sal_Char *SwHTMLWriter::GetNumFormat( USHORT nFmt )
{
    const sal_Char *pFmtStr = 0;

    switch( (SvxExtNumType)nFmt )
    {
    case SVX_NUM_CHARS_UPPER_LETTER:	pFmtStr = OOO_STRING_SW_HTML_FF_uletter; 	break;
    case SVX_NUM_CHARS_LOWER_LETTER:	pFmtStr = OOO_STRING_SW_HTML_FF_lletter; 	break;
    case SVX_NUM_ROMAN_UPPER:			pFmtStr = OOO_STRING_SW_HTML_FF_uroman; 		break;
    case SVX_NUM_ROMAN_LOWER:			pFmtStr = OOO_STRING_SW_HTML_FF_lroman; 		break;
    case SVX_NUM_ARABIC:				pFmtStr = OOO_STRING_SW_HTML_FF_arabic;	 	break;
    case SVX_NUM_NUMBER_NONE:			pFmtStr = OOO_STRING_SW_HTML_FF_none; 		break;
    case SVX_NUM_CHAR_SPECIAL:			pFmtStr = OOO_STRING_SW_HTML_FF_char; 		break;
    case SVX_NUM_PAGEDESC:				pFmtStr = OOO_STRING_SW_HTML_FF_page; 		break;
    case SVX_NUM_CHARS_UPPER_LETTER_N:	pFmtStr = OOO_STRING_SW_HTML_FF_ulettern; 	break;
    case SVX_NUM_CHARS_LOWER_LETTER_N:	pFmtStr = OOO_STRING_SW_HTML_FF_llettern; 	break;
    default:
        ;
    }

    return pFmtStr;
}

extern BOOL lcl_css1atr_equalFontItems( const SfxPoolItem& r1, const SfxPoolItem& r2 );
static Writer& OutHTML_SwField( Writer& rWrt, const SwField* pFld,
                                 const SwTxtNode& rTxtNd, xub_StrLen nFldPos )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    const SwFieldType* pFldTyp = pFld->GetTyp();
    USHORT nField = pFldTyp->Which();
    ULONG nFmt = pFld->GetFormat();

    const sal_Char *pTypeStr=0,	// TYPE
                      *pSubStr=0, 	// SUBTYPE
                   *pFmtStr=0;	// FORMAT (SW)
    String aValue;				// VALUE (SW)
    BOOL bNumFmt=FALSE;			// SDNUM (Number-Formatter-Format)
    BOOL bNumValue=FALSE;  		// SDVAL (Number-Formatter-Value)
    double dNumValue = 0.0;	 	// SDVAL (Number-Formatter-Value)
    BOOL bFixed=FALSE;			// SDFIXED
    String aName;               // NAME (CUSTOM)

    switch( nField )
    {
        case RES_EXTUSERFLD:
            pTypeStr = OOO_STRING_SW_HTML_FT_sender;
            switch( (SwExtUserSubType)pFld->GetSubType() )
            {
                case EU_COMPANY:  	pSubStr = OOO_STRING_SW_HTML_FS_company;		break;
                case EU_FIRSTNAME:	pSubStr = OOO_STRING_SW_HTML_FS_firstname;	break;
                case EU_NAME: 		pSubStr = OOO_STRING_SW_HTML_FS_name;		break;
                case EU_SHORTCUT:	pSubStr = OOO_STRING_SW_HTML_FS_shortcut;	break;
                case EU_STREET: 	pSubStr = OOO_STRING_SW_HTML_FS_street;		break;
                case EU_COUNTRY:	pSubStr = OOO_STRING_SW_HTML_FS_country;     break;
                case EU_ZIP: 		pSubStr = OOO_STRING_SW_HTML_FS_zip;         break;
                case EU_CITY: 		pSubStr = OOO_STRING_SW_HTML_FS_city;        break;
                case EU_TITLE: 		pSubStr = OOO_STRING_SW_HTML_FS_title;       break;
                case EU_POSITION:	pSubStr = OOO_STRING_SW_HTML_FS_position;    break;
                case EU_PHONE_PRIVATE:	pSubStr = OOO_STRING_SW_HTML_FS_pphone;      break;
                case EU_PHONE_COMPANY:	pSubStr = OOO_STRING_SW_HTML_FS_cphone;      break;
                case EU_FAX: 		pSubStr = OOO_STRING_SW_HTML_FS_fax;         break;
                case EU_EMAIL:  	pSubStr = OOO_STRING_SW_HTML_FS_email;       break;
                case EU_STATE:  	pSubStr = OOO_STRING_SW_HTML_FS_state;       break;
                default:
                    ;
            }
            ASSERT( pSubStr, "ubekannter Subtyp fuer SwExtUserField" );
            bFixed = ((const SwExtUserField*)pFld)->IsFixed();
            break;

        case RES_AUTHORFLD:
            pTypeStr = OOO_STRING_SW_HTML_FT_author;
            switch( (SwAuthorFormat)nFmt & 0xff)
            {
                case AF_NAME: 	  pFmtStr = OOO_STRING_SW_HTML_FF_name;		break;
                case AF_SHORTCUT:  pFmtStr = OOO_STRING_SW_HTML_FF_shortcut;	break;
            }
            ASSERT( pFmtStr, "ubekanntes Format fuer SwAuthorField" );
            bFixed = ((const SwAuthorField*)pFld)->IsFixed();
            break;

        case RES_DATETIMEFLD:
            pTypeStr = OOO_STRING_SW_HTML_FT_datetime;
            bNumFmt = TRUE;
            if( ((SwDateTimeField*)pFld)->IsFixed() )
            {
                bNumValue = TRUE;
                dNumValue = ((SwDateTimeField*)pFld)->GetValue();
            }
            break;

        case RES_PAGENUMBERFLD:
            {
                pTypeStr = OOO_STRING_SW_HTML_FT_page;
                SwPageNumSubType eSubType = (SwPageNumSubType)pFld->GetSubType();
                switch( eSubType )
                {
                    case PG_RANDOM:		pSubStr = OOO_STRING_SW_HTML_FS_random;		break;
                    case PG_NEXT:       pSubStr = OOO_STRING_SW_HTML_FS_next;		break;
                    case PG_PREV:       pSubStr = OOO_STRING_SW_HTML_FS_prev;		break;
                }
                ASSERT( pSubStr, "ubekannter Subtyp fuer SwPageNumberField" );
                pFmtStr = SwHTMLWriter::GetNumFormat( static_cast< sal_uInt16 >(nFmt) );

                if( (SvxExtNumType)nFmt==SVX_NUM_CHAR_SPECIAL )
                {
                    aValue = ((const SwPageNumberField *)pFld)->GetUserString();
                }
                else
                {
                    const String& rValue = pFld->GetPar2();
                    short nValue = (short)rValue.ToInt32();
                    if( (eSubType == PG_NEXT && nValue!=1) ||
                        (eSubType == PG_PREV && nValue!=-1) ||
                        (eSubType == PG_RANDOM && nValue!=0) )
                    {
                        aValue = rValue;
                    }
                }
            }
            break;
        case RES_DOCINFOFLD:
            {
                USHORT nSubType = pFld->GetSubType();
                pTypeStr = OOO_STRING_SW_HTML_FT_docinfo;
                USHORT nExtSubType = nSubType & 0x0f00;
                nSubType &= 0x00ff;

                switch( nSubType )
                {
                    case DI_TITEL:    	pSubStr = OOO_STRING_SW_HTML_FS_title; 	break;
                    case DI_THEMA:    	pSubStr = OOO_STRING_SW_HTML_FS_theme; 	break;
                    case DI_KEYS:    	pSubStr = OOO_STRING_SW_HTML_FS_keys; 	break;
                    case DI_COMMENT:    pSubStr = OOO_STRING_SW_HTML_FS_comment; break;
                    case DI_CREATE:    	pSubStr = OOO_STRING_SW_HTML_FS_create; 	break;
                    case DI_CHANGE:   	pSubStr = OOO_STRING_SW_HTML_FS_change; 	break;
                    case DI_CUSTOM:   	pSubStr = OOO_STRING_SW_HTML_FS_custom; 	break;
                    default:			pTypeStr = 0; 				break;
                }

                if( DI_CUSTOM == nSubType ) {
                    aName = static_cast<const SwDocInfoField*>(pFld)->GetName();
                }

                if( DI_CREATE == nSubType || DI_CHANGE == nSubType )
                {
                    switch( nExtSubType )
                    {
                        case DI_SUB_AUTHOR:
                            pFmtStr = OOO_STRING_SW_HTML_FF_author;
                            break;
                        case DI_SUB_TIME:
                            pFmtStr = OOO_STRING_SW_HTML_FF_time;
                            bNumFmt = TRUE;
                            break;
                        case DI_SUB_DATE:
                            pFmtStr = OOO_STRING_SW_HTML_FF_date;
                            bNumFmt = TRUE;
                            break;
                    }
                }
                bFixed = ((const SwDocInfoField*)pFld)->IsFixed();
                if( bNumFmt )
                {
                    if( bFixed )
                    {
                        // Fuer ein fixes Feld och den Num-Value ausgeben.
                        // Fixe Felder ohne Zahlenformate sollte es
                        // eigentlich nicht geben. ASSERT ist unten.
                        dNumValue = ((const SwDocInfoField*)pFld)->GetValue();
                        bNumValue = TRUE;
                    }
                    else if( !nFmt  )
                    {
                        // Nicht fixe Felder muessen kein Zahlenformat haben,
                        // wenn sie aus 4.0-Dokumenten stammen.
                        bNumFmt = FALSE;
                    }
                }
            }
            break;

        case RES_DOCSTATFLD:
            {
                pTypeStr = OOO_STRING_SW_HTML_FT_docstat;
                USHORT nSubType = pFld->GetSubType();
                switch( nSubType )
                {
                    case DS_PAGE:    	pSubStr = OOO_STRING_SW_HTML_FS_page; 	break;
                    case DS_PARA:    	pSubStr = OOO_STRING_SW_HTML_FS_para; 	break;
                    case DS_WORD:    	pSubStr = OOO_STRING_SW_HTML_FS_word; 	break;
                    case DS_CHAR:    	pSubStr = OOO_STRING_SW_HTML_FS_char; 	break;
                    case DS_TBL:    	pSubStr = OOO_STRING_SW_HTML_FS_tbl; 	break;
                    case DS_GRF:    	pSubStr = OOO_STRING_SW_HTML_FS_grf; 	break;
                    case DS_OLE:    	pSubStr = OOO_STRING_SW_HTML_FS_ole; 	break;
                    default:			pTypeStr = 0; 				break;
                }
                pFmtStr = SwHTMLWriter::GetNumFormat( static_cast< sal_uInt16 >(nFmt) );
            }
            break;

        case RES_FILENAMEFLD:
            pTypeStr = OOO_STRING_SW_HTML_FT_filename;
            switch( (SwFileNameFormat)(nFmt & ~FF_FIXED) )
            {
                case FF_NAME: 	  	pFmtStr = OOO_STRING_SW_HTML_FF_name;		break;
                case FF_PATHNAME: 	pFmtStr = OOO_STRING_SW_HTML_FF_pathname;	break;
                case FF_PATH:     	pFmtStr = OOO_STRING_SW_HTML_FF_path;		break;
                case FF_NAME_NOEXT:	pFmtStr = OOO_STRING_SW_HTML_FF_name_noext;	break;
                default:
                    ;
            }
            bFixed = ((const SwFileNameField*)pFld)->IsFixed();
            ASSERT( pFmtStr, "unbekanntes Format fuer SwFileNameField" );
            break;
    }

    // <SDFIELD>-Tag ausgeben
    if( pTypeStr )
    {
        ByteString sOut( '<' );
        ((((sOut += OOO_STRING_SVTOOLS_HTML_sdfield) += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += '=')
            += pTypeStr;
        if( pSubStr )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_subtype) += '=') += pSubStr;
        if( pFmtStr )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_format) += '=') += pFmtStr;
        if( aName.Len() )
        {
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"");
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), aName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }
        if( aValue.Len() )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_value) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), aValue, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }
        if( bNumFmt )
        {
            ASSERT( nFmt, "Zahlenformat ist 0" );
            sOut = HTMLOutFuncs::CreateTableDataOptionsValNum( sOut,
                        bNumValue, dNumValue, nFmt,
                        *rHTMLWrt.pDoc->GetNumberFormatter(),
                        rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );

        }
        if( bFixed )
            (sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_sdfixed;
        sOut += '>';
        rWrt.Strm() << sOut.GetBuffer();
    }

    // Inhalt des Feldes ausgeben
    String const sExpand( pFld->ExpandField(rWrt.pDoc->IsClipBoard()) );
    sal_Bool bNeedsCJKProcessing = sal_False;
    if( sExpand.Len() )
    {
        sal_uInt16 nScriptType = pBreakIt->GetBreakIter()->getScriptType( sExpand, 0 );
        xub_StrLen nPos = (xub_StrLen)pBreakIt->GetBreakIter()->endOfScript( sExpand, 0,
                                                          nScriptType );

        sal_uInt16 nScript =
            SwHTMLWriter::GetCSS1ScriptForScriptType( nScriptType );
        if( nPos < sExpand.Len() || nScript != rHTMLWrt.nCSS1Script )
        {
            bNeedsCJKProcessing = sal_True;
        }
    }

    if( bNeedsCJKProcessing )
    {
        SfxItemSet aScriptItemSet( rWrt.pDoc->GetAttrPool(),
                                   RES_CHRATR_FONT, RES_CHRATR_FONTSIZE,
                                   RES_CHRATR_POSTURE, RES_CHRATR_POSTURE,
                                   RES_CHRATR_WEIGHT, RES_CHRATR_WEIGHT,
                                   RES_CHRATR_CJK_FONT, RES_CHRATR_CTL_WEIGHT,
                                   0 );
        rTxtNd.GetAttr( aScriptItemSet, nFldPos, nFldPos+1 );

        sal_uInt16 aWesternWhichIds[4] =
            { RES_CHRATR_FONT, RES_CHRATR_FONTSIZE,
              RES_CHRATR_POSTURE, RES_CHRATR_WEIGHT };
        sal_uInt16 aCJKWhichIds[4] =
            { RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONTSIZE,
              RES_CHRATR_CJK_POSTURE, RES_CHRATR_CJK_WEIGHT };
        sal_uInt16 aCTLWhichIds[4] =
            { RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONTSIZE,
              RES_CHRATR_CTL_POSTURE, RES_CHRATR_CTL_WEIGHT };

        sal_uInt16 *pRefWhichIds = 0;
        switch( rHTMLWrt.nCSS1Script )
        {
        case CSS1_OUTMODE_WESTERN:
            pRefWhichIds = aWesternWhichIds;
            break;
        case CSS1_OUTMODE_CJK:
            pRefWhichIds = aCJKWhichIds;
            break;
        case CSS1_OUTMODE_CTL:
            pRefWhichIds = aCTLWhichIds;
            break;
        }

        xub_StrLen nPos = 0;
        do
        {
            sal_uInt16 nScriptType = pBreakIt->GetBreakIter()->getScriptType( sExpand, nPos );
            sal_uInt16 nScript =
                SwHTMLWriter::GetCSS1ScriptForScriptType( nScriptType );
            xub_StrLen nEndPos = (xub_StrLen)pBreakIt->GetBreakIter()->endOfScript(
                                    sExpand, nPos, nScriptType );
            if( nScript != CSS1_OUTMODE_ANY_SCRIPT &&
                /* #108791# */ nScript != rHTMLWrt.nCSS1Script )
            {
                sal_uInt16 *pWhichIds = 0;
                switch( nScript )
                {
                case CSS1_OUTMODE_WESTERN:	pWhichIds = aWesternWhichIds; break;
                case CSS1_OUTMODE_CJK:		pWhichIds = aCJKWhichIds; break;
                case CSS1_OUTMODE_CTL:		pWhichIds = aCTLWhichIds; break;
                }

                rHTMLWrt.bTagOn = TRUE;
                const SfxPoolItem *aItems[5];
                sal_uInt16 nItems = 0;
                for( sal_uInt16 i=0; i<4; i++ )
                {
                    const SfxPoolItem *pRefItem =
                        aScriptItemSet.GetItem( pRefWhichIds[i] );
                    const SfxPoolItem *pItem =
                        aScriptItemSet.GetItem( pWhichIds[i] );
                    if( pRefItem && pItem &&
                        !(0==i ? lcl_css1atr_equalFontItems( *pRefItem, *pItem )
                               : *pRefItem == *pItem) )
                    {
                        Out( aHTMLAttrFnTab, *pItem, rHTMLWrt );
                        aItems[nItems++] = pItem;
                    }
                }

                HTMLOutFuncs::Out_String( rWrt.Strm(), sExpand.Copy( nPos, nEndPos ),
                    rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );

                rHTMLWrt.bTagOn = FALSE;
                while( nItems )
                    Out( aHTMLAttrFnTab, *aItems[--nItems], rHTMLWrt );

            }
            else
            {
                HTMLOutFuncs::Out_String( rWrt.Strm(), sExpand.Copy( nPos, nEndPos ),
                    rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            }
            nPos = nEndPos;
        }
        while( nPos < sExpand.Len() );
    }
    else
    {
        HTMLOutFuncs::Out_String( rWrt.Strm(), sExpand,
              rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    }

    // Off-Tag ausgeben
    if( pTypeStr )
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_sdfield, FALSE );

    return rWrt;
}


Writer& OutHTML_SwFmtFld( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwFmtFld & rFld = (SwFmtFld&)rHt;
    const SwField* pFld = rFld.GetFld();
    const SwFieldType* pFldTyp = pFld->GetTyp();

    if( RES_SETEXPFLD == pFldTyp->Which() &&
        (nsSwGetSetExpType::GSE_STRING & pFld->GetSubType()) )
    {
        int bOn = FALSE;
        if( pFldTyp->GetName().EqualsAscii("HTML_ON" ) )
            bOn = TRUE;
        else if( !pFldTyp->GetName().EqualsAscii( "HTML_OFF" ) )
            return rWrt;

        String rTxt( pFld->GetPar2() );
        rTxt.EraseLeadingChars().EraseTrailingChars();
        rWrt.Strm() << '<';
        if( !bOn )
            rWrt.Strm() << '/';
        // TODO: HTML-Tags are written without entitities, that for, characters
        // not contained in the destination encoding are lost!
        ByteString sTmp( rTxt, ((SwHTMLWriter&)rWrt).eDestEnc );
        rWrt.Strm() << sTmp.GetBuffer() << '>';
    }
    else if( RES_POSTITFLD == pFldTyp->Which() )
    {
        // Kommentare werden im ANSI-Zeichensetz, aber mit System-Zeilen-
        // Umbruechen gesschrieben.
        const String& rComment = pFld->GetPar2();
        BOOL bWritten = FALSE;

        if( (rComment.Len() >= 6 && '<' == rComment.GetChar(0) &&
            '>' == rComment.GetChar(rComment.Len()-1) &&
            rComment.Copy( 1, 4 ).EqualsIgnoreCaseAscii(OOO_STRING_SVTOOLS_HTML_meta)) ||
              (rComment.Len() >= 7 &&
             rComment.Copy( 0, 4 ).EqualsAscii( "<!--" ) &&
             rComment.Copy( rComment.Len()-3, 3 ).EqualsAscii( "-->" )) )
        {
            // META-Tags direkt ausgeben
            String sComment( rComment );
            sComment.ConvertLineEnd( GetSystemLineEnd() );
            // TODO: HTML-Tags are written without entitities, that for,
            // characters not contained in the destination encoding are lost!
            ByteString sTmp( sComment, ((SwHTMLWriter&)rWrt).eDestEnc );
            rWrt.Strm() << sTmp.GetBuffer();
            bWritten = TRUE;
        }
        else if( rComment.Len() >= 7 &&
                 '>' == rComment.GetChar(rComment.Len()-1) &&
                 rComment.Copy(0,5).EqualsIgnoreCaseAscii("HTML:") )
        {
            String sComment( rComment.Copy(5) );
            sComment.EraseLeadingChars();
            if( '<' == sComment.GetChar(0) )
            {
                sComment.ConvertLineEnd( GetSystemLineEnd() );
                // TODO: HTML-Tags are written without entitities, that for,
                // characters not contained in the destination encoding are
                // lost!
                ByteString sTmp( sComment, ((SwHTMLWriter&)rWrt).eDestEnc );
                rWrt.Strm() << sTmp.GetBuffer();
                bWritten = TRUE;
            }

        }

        if( !bWritten )
        {
            ByteString sOut( '<' );

            String sComment( rComment );
            sComment.ConvertLineEnd( GetSystemLineEnd() );
            // TODO: ???
            (((sOut += OOO_STRING_SVTOOLS_HTML_comment) += ' ')
                += ByteString( sComment, ((SwHTMLWriter&)rWrt).eDestEnc ))
                += " -->";
            rWrt.Strm() << sOut.GetBuffer();
        }
    }
    else if( RES_SCRIPTFLD == pFldTyp->Which() )
    {
        SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
        if( rHTMLWrt.bLFPossible )
            rHTMLWrt.OutNewLine( TRUE );

        BOOL bURL = ((const SwScriptField *)pFld)->IsCodeURL();
        const String& rType = pFld->GetPar1();
        String aContents, aURL;
        if(bURL)
            aURL = pFld->GetPar2();
        else
            aContents = pFld->GetPar2();

        // sonst ist es der Script-Inhalt selbst. Da nur noh JavaScript
        // in Feldern landet, muss es sich um JavaSrript handeln ...:)
        HTMLOutFuncs::OutScript( rWrt.Strm(), rWrt.GetBaseURL(), aContents, rType, JAVASCRIPT,
                                 aURL, 0, 0, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );

        if( rHTMLWrt.bLFPossible )
            rHTMLWrt.OutNewLine( TRUE );
    }
    else
    {
        const SwTxtFld *pTxtFld = rFld.GetTxtFld();
        ASSERT( pTxtFld, "Where is the txt fld?" );
        if( pTxtFld )
            OutHTML_SwField( rWrt, pFld, pTxtFld->GetTxtNode(),
                             *pTxtFld->GetStart()  );
    }
    return rWrt;
}


