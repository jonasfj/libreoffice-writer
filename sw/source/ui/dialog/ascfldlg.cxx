/*************************************************************************
 *
 *  $RCSfile: ascfldlg.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: os $ $Date: 2002-12-05 12:38:32 $
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
#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif


#ifndef _RTL_TEXTENC_H //autogen wg. rtl_TextEncoding
#include <rtl/textenc.h>
#endif
#ifndef _SVTOOLS_LINGUCFG_HXX_
#include <svtools/lingucfg.hxx>
#endif
#ifndef _FONTCFG_HXX
#include <fontcfg.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif

#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif
#ifndef _SVX_FLSTITEM_HXX //autogen wg. SvxFontListItem
#include <svx/flstitem.hxx>
#endif
#ifndef _SVX_DLGUTIL_HXX //autogen wg. GetLanguageString
#include <svx/dlgutil.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen wg. SvxFontItem
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX //autogen wg. SvxLanguageItem
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <svx/scripttypeitem.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _ASCFLDLG_HXX
#include <ascfldlg.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _SWDOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _DIALOG_HRC
#include <dialog.hrc>
#endif
#ifndef _ASCFLDLG_HRC
#include <ascfldlg.hrc>
#endif

using namespace ::com::sun::star;

const sal_Unicode cDialogExtraDataClose = '}';
const char __FAR_DATA sDialogImpExtraData[] = "EncImpDlg:{";
const char __FAR_DATA sDialogExpExtraData[] = "EncExpDlg:{";
const USHORT nDialogExtraDataLen = 11;	  	// 12345678901

SwAsciiFilterDlg::SwAsciiFilterDlg( Window* pParent, SwDocShell& rDocSh,
                                    SvStream* pStream )
    : SfxModalDialog( pParent, SW_RES( DLG_ASCII_FILTER )),
    aFL( this, ResId( FL_1 )),
    aCharSetFT( this, ResId( FT_CHARSET )),
    aCharSetLB( this, ResId( LB_CHARSET )),
    aFontFT( this, ResId( FT_FONT )),
    aFontLB( this, ResId( LB_FONT )),
    aLanguageFT( this, ResId( FT_LANGUAGE )),
    aLanguageLB( this, ResId( LB_LANGUAGE )),
    aCRLF_FT( this, ResId( FT_CRLF )),
    aCRLF_RB( this, ResId( RB_CRLF )),
    aCR_RB( this, ResId( RB_CR )),
    aLF_RB( this, ResId( RB_LF )),
    aOkPB( this, ResId( PB_OK )),
    aCancelPB( this, ResId( PB_CANCEL )),
    aHelpPB( this, ResId( PB_HELP )),
    sSystemCharSet( ResId( STR_SYS_CHARSET )),
    bSaveLineStatus( TRUE )
{
    FreeResource();

    SwAsciiOptions aOpt;
    {
        const String& rFindNm = String::CreateFromAscii(
                                    pStream ? sDialogImpExtraData
                                              : sDialogExpExtraData);
        USHORT nEnd, nStt = GetExtraData().Search( rFindNm );
        if( STRING_NOTFOUND != nStt )
        {
            nStt += nDialogExtraDataLen;
            nEnd = GetExtraData().Search( cDialogExtraDataClose, nStt );
            if( STRING_NOTFOUND != nEnd )
            {
                aOpt.ReadUserData( GetExtraData().Copy( nStt, nEnd - nStt ));
                nStt -= nDialogExtraDataLen;
                GetExtraData().Erase( nStt, nEnd - nStt + 1 );
            }
        }
    }

    // read the first chars and check the charset, (language - with L&H)
    if( pStream )
    {
        char aBuffer[ 4098 ];
        ULONG nOldPos = pStream->Tell();
        ULONG nBytesRead = pStream->Read( aBuffer, 4096 );
        pStream->Seek( nOldPos );

        if( nBytesRead <= 4096 )
        {
            aBuffer[ nBytesRead ] = '0';
            aBuffer[ nBytesRead+1 ] = '0';
            if( 0 != ( nBytesRead & 0x00000001 ) )
                aBuffer[ nBytesRead + 2 ] = '0';
        }

        BOOL bCR = FALSE, bLF = FALSE, bNoNormalChar = FALSE,
            bNullChar = FALSE;
        for( USHORT nCnt = 0; nCnt < nBytesRead; ++nCnt )
            switch( aBuffer[ nCnt ] )
            {
                case 0x0:	bNullChar = TRUE; break;
                case 0xA:	bLF = TRUE; break;
                case 0xD:	bCR = TRUE; break;
                case 0xC:
                case 0x1A:
                case 0x9:	break;
                default:
                    if( 0x20 > aBuffer[ nCnt ] )
                        bNoNormalChar = TRUE;
            }

        if( !bNullChar )
        {
            if( bCR )
            {
                if( bLF )
                {
                    aOpt.SetParaFlags( LINEEND_CRLF );
// have to check if of CharSet is type of ANSI
//					aOpt.SetCharSet( CHARSET_ANSI );
                }
                else
                {
                    aOpt.SetParaFlags( LINEEND_CR );
// have to check if CharSet is type of MAC
//					aOpt.SetCharSet( CHARSET_MAC );
                }
            }
            else if( bLF )
            {
                aOpt.SetParaFlags( LINEEND_LF );
// have to check if of CharSet is type of ANSI
//				aOpt.SetCharSet( CHARSET_ANSI );
            }
        }

        SwDoc* pDoc = rDocSh.GetDoc();
        {
            BOOL bDelPrinter = FALSE;
            SfxPrinter* pPrt = pDoc ? pDoc->GetPrt() : 0;
            if( !pPrt )
            {
                SfxItemSet* pSet = new SfxItemSet( rDocSh.GetPool(),
                            SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                            SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                            0 );
                pPrt = new SfxPrinter( pSet );
                bDelPrinter = TRUE;
            }

            const USHORT nCount = pPrt->GetFontCount();
            for (USHORT i = 0; i < nCount; ++i)
            {
                const String &rStr = pPrt->GetFont(i)->GetName();
                aFontLB.InsertEntry( rStr );
            }

            if( !aOpt.GetFontName().Len() )
            {
                if(pDoc)
                {
                    aOpt.SetFontName( ((SvxFontItem&)pDoc->GetDefault(
                                    RES_CHRATR_FONT )).GetFamilyName() );
                }
                else
                {
                    aOpt.SetFontName(SW_MOD()->GetStdFontConfig()->GetFontFor(FONT_STANDARD));
                }
            }
            aFontLB.SelectEntry( aOpt.GetFontName() );

            if( bDelPrinter )
                delete pPrt;
        }

        // initialisiere Sprache
        {
            if( !aOpt.GetLanguage() )
            {
                if(pDoc)
                {
                    USHORT nWhich = GetWhichOfScript( RES_CHRATR_LANGUAGE,
                                    GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() ));
                    aOpt.SetLanguage( ((SvxLanguageItem&)pDoc->
                                GetDefault( nWhich )).GetLanguage());
                }
                else
                {
                    SvtLinguOptions aLinguOpt;
                    SvtLinguConfig().GetOptions( aLinguOpt );
                    aOpt.SetLanguage(aLinguOpt.nDefaultLanguage);
                }
            }

            aLanguageLB.SetLanguageList( LANG_LIST_ALL, TRUE, FALSE );
            aLanguageLB.SelectLanguage( aOpt.GetLanguage() );
        }
    }
    else
    {
        // hide the used Control for the Export and move the
        // other behind the charset controls
        aFontFT.Hide();
        aFontLB.Hide();
        aLanguageFT.Hide();
        aLanguageLB.Hide();

        long nY = aFontFT.GetPosPixel().Y() + 1;
        Point aPos( aCRLF_FT.GetPosPixel() );	aPos.Y() = nY;
        aCRLF_FT.SetPosPixel( aPos );

        aPos = aCRLF_RB.GetPosPixel();	aPos.Y() = nY;
        aCRLF_RB.SetPosPixel( aPos );

        aPos = aCR_RB.GetPosPixel();	aPos.Y() = nY;
        aCR_RB.SetPosPixel( aPos );

        aPos = aLF_RB.GetPosPixel();	aPos.Y() = nY;
        aLF_RB.SetPosPixel( aPos );

        Size aSize = GetSizePixel();
        Size aTmpSz( 6, 6 );
        aTmpSz = LogicToPixel(aTmpSz, MAP_APPFONT);
        aSize.Height() = aHelpPB.GetPosPixel().Y() +
                         aHelpPB.GetSizePixel().Height() + aTmpSz.Height();
        SetSizePixel( aSize );
    }

    // initialisiere Zeichensatz
    aCharSetLB.FillFromTextEncodingTable( pStream != NULL );
    aCharSetLB.SelectTextEncoding( aOpt.GetCharSet()  );

    aCharSetLB.SetSelectHdl( LINK( this, SwAsciiFilterDlg, CharSetSelHdl ));
    aCRLF_RB.SetToggleHdl( LINK( this, SwAsciiFilterDlg, LineEndHdl ));
    aLF_RB.SetToggleHdl( LINK( this, SwAsciiFilterDlg, LineEndHdl ));
    aCR_RB.SetToggleHdl( LINK( this, SwAsciiFilterDlg, LineEndHdl ));

    SetCRLF( aOpt.GetParaFlags() );

    aCRLF_RB.SaveValue();
    aLF_RB.SaveValue();
    aCR_RB.SaveValue();
}


SwAsciiFilterDlg::~SwAsciiFilterDlg()
{
}


void SwAsciiFilterDlg::FillOptions( SwAsciiOptions& rOptions )
{
    ULONG nCCode = aCharSetLB.GetSelectTextEncoding();
    String sFont;
    ULONG nLng = 0;
    if( aFontLB.IsVisible() )
    {
        sFont = aFontLB.GetSelectEntry();
        nLng = (ULONG)aLanguageLB.GetSelectLanguage();
    }

    rOptions.SetFontName( sFont );
    rOptions.SetCharSet( rtl_TextEncoding( nCCode ) );
    rOptions.SetLanguage( USHORT( nLng ) );
    rOptions.SetParaFlags( GetCRLF() );

    // JP: Task #71802# save the user settings
    String sData;
    rOptions.WriteUserData( sData );
    if( sData.Len() )
    {
        const String& rFindNm = String::CreateFromAscii(
                                    aFontLB.IsVisible() ? sDialogImpExtraData
                                              : sDialogExpExtraData);
        USHORT nEnd, nStt = GetExtraData().Search( rFindNm );
        if( STRING_NOTFOUND != nStt )
        {
            // called twice, so remove "old" settings
            nEnd = GetExtraData().Search( cDialogExtraDataClose,
                                            nStt + nDialogExtraDataLen );
            if( STRING_NOTFOUND != nEnd )
                GetExtraData().Erase( nStt, nEnd - nStt + 1 );
        }
        String sTmp(GetExtraData());
        sTmp += rFindNm;
        sTmp += sData;
        sTmp += cDialogExtraDataClose;
        GetExtraData() = sTmp;
    }
}

void SwAsciiFilterDlg::SetCRLF( LineEnd eEnd )
{
    switch( eEnd )
    {
    case LINEEND_CR: 	aCR_RB.Check();		break;
    case LINEEND_CRLF: 	aCRLF_RB.Check();	break;
    case LINEEND_LF: 	aLF_RB.Check();		break;
    }
}

LineEnd SwAsciiFilterDlg::GetCRLF() const
{
    LineEnd eEnd;
    if( aCR_RB.IsChecked() )
        eEnd = LINEEND_CR;
    else if( aLF_RB.IsChecked() )
        eEnd = LINEEND_LF;
    else
        eEnd = LINEEND_CRLF;
    return eEnd;
}

IMPL_LINK( SwAsciiFilterDlg, CharSetSelHdl, SvxTextEncodingBox*, pBox )
{
    LineEnd eOldEnd = GetCRLF(), eEnd = (LineEnd)-1;
    ULONG nLng = aFontLB.IsVisible()
                    ? (ULONG)aLanguageLB.GetSelectLanguage()
                    : 0,
            nOldLng = nLng;

    rtl_TextEncoding nChrSet = pBox->GetSelectTextEncoding();
    if( nChrSet == gsl_getSystemTextEncoding() )
        eEnd = GetSystemLineEnd();
    else
    {
        switch( nChrSet )
        {
        case RTL_TEXTENCODING_MS_1252:
#ifdef UNX
            eEnd = LINEEND_LF;
#else
            eEnd = LINEEND_CRLF;				// ANSI
#endif
            break;

        case RTL_TEXTENCODING_APPLE_ROMAN:		// MAC
            eEnd = LINEEND_CR;
            break;

        case RTL_TEXTENCODING_IBM_850:			// DOS
            eEnd = LINEEND_CRLF;
            break;

        case RTL_TEXTENCODING_APPLE_ARABIC:
        case RTL_TEXTENCODING_APPLE_CENTEURO:
        case RTL_TEXTENCODING_APPLE_CROATIAN:
        case RTL_TEXTENCODING_APPLE_CYRILLIC:
        case RTL_TEXTENCODING_APPLE_DEVANAGARI:
        case RTL_TEXTENCODING_APPLE_FARSI:
        case RTL_TEXTENCODING_APPLE_GREEK:
        case RTL_TEXTENCODING_APPLE_GUJARATI:
        case RTL_TEXTENCODING_APPLE_GURMUKHI:
        case RTL_TEXTENCODING_APPLE_HEBREW:
        case RTL_TEXTENCODING_APPLE_ICELAND:
        case RTL_TEXTENCODING_APPLE_ROMANIAN:
        case RTL_TEXTENCODING_APPLE_THAI:
        case RTL_TEXTENCODING_APPLE_TURKISH:
        case RTL_TEXTENCODING_APPLE_UKRAINIAN:
        case RTL_TEXTENCODING_APPLE_CHINSIMP:
        case RTL_TEXTENCODING_APPLE_CHINTRAD:
        case RTL_TEXTENCODING_APPLE_JAPANESE:
        case RTL_TEXTENCODING_APPLE_KOREAN:
            eEnd = LINEEND_CR;
            break;

#if 0
which charset and language?
        case RTL_TEXTENCODING_IBM_437:
        case RTL_TEXTENCODING_IBM_860:
        case RTL_TEXTENCODING_IBM_861:
        case RTL_TEXTENCODING_IBM_863:
        case RTL_TEXTENCODING_IBM_865:
        case RTL_TEXTENCODING_ASCII_US:
        case RTL_TEXTENCODING_ISO_8859_1:
        case RTL_TEXTENCODING_ISO_8859_2:
        case RTL_TEXTENCODING_ISO_8859_3:
        case RTL_TEXTENCODING_ISO_8859_4:
        case RTL_TEXTENCODING_ISO_8859_5:
        case RTL_TEXTENCODING_ISO_8859_6:
        case RTL_TEXTENCODING_ISO_8859_7:
        case RTL_TEXTENCODING_ISO_8859_8:
        case RTL_TEXTENCODING_ISO_8859_9:
        case RTL_TEXTENCODING_ISO_8859_14:
        case RTL_TEXTENCODING_ISO_8859_15:
        case RTL_TEXTENCODING_IBM_737:
        case RTL_TEXTENCODING_IBM_775:
        case RTL_TEXTENCODING_IBM_852:
        case RTL_TEXTENCODING_IBM_855:
        case RTL_TEXTENCODING_IBM_857:
        case RTL_TEXTENCODING_IBM_862:
        case RTL_TEXTENCODING_IBM_864:
        case RTL_TEXTENCODING_IBM_866:
        case RTL_TEXTENCODING_IBM_869:
        case RTL_TEXTENCODING_MS_874:
        case RTL_TEXTENCODING_MS_1250:
        case RTL_TEXTENCODING_MS_1251:
        case RTL_TEXTENCODING_MS_1253:
        case RTL_TEXTENCODING_MS_1254:
        case RTL_TEXTENCODING_MS_1255:
        case RTL_TEXTENCODING_MS_1256:
        case RTL_TEXTENCODING_MS_1257:
        case RTL_TEXTENCODING_MS_1258:
        case RTL_TEXTENCODING_MS_932:
        case RTL_TEXTENCODING_MS_936:
        case RTL_TEXTENCODING_MS_949:
        case RTL_TEXTENCODING_MS_950:
        case RTL_TEXTENCODING_SHIFT_JIS:
        case RTL_TEXTENCODING_GB_2312:
        case RTL_TEXTENCODING_GBT_12345:
        case RTL_TEXTENCODING_GBK:
        case RTL_TEXTENCODING_BIG5:
        case RTL_TEXTENCODING_EUC_JP:
        case RTL_TEXTENCODING_EUC_CN:
        case RTL_TEXTENCODING_EUC_TW:
        case RTL_TEXTENCODING_ISO_2022_JP:
        case RTL_TEXTENCODING_ISO_2022_CN:
        case RTL_TEXTENCODING_KOI8_R:
#endif
        }
    }

    bSaveLineStatus = FALSE;
    if( eEnd != (LineEnd)-1 )		// changed?
    {
        if( eOldEnd != eEnd )
            SetCRLF( eEnd );
    }
    else
    {
        // restore old user choise (not the automatic!)
        aCRLF_RB.Check( aCRLF_RB.GetSavedValue() );
        aCR_RB.Check( aCR_RB.GetSavedValue() );
        aLF_RB.Check( aLF_RB.GetSavedValue() );
    }
    bSaveLineStatus = TRUE;

    if( nOldLng != nLng && aFontLB.IsVisible() )
        aLanguageLB.SelectLanguage( nLng );

    return 0;
}

IMPL_LINK( SwAsciiFilterDlg, LineEndHdl, RadioButton*, pBtn )
{
    if( bSaveLineStatus )
        pBtn->SaveValue();
    return 0;
}


