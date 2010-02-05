/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fntcache.cxx,v $
 * $Revision: 1.98.56.1 $
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


#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#ifndef _PRINT_HXX //autogen
#include <vcl/print.hxx>
#endif
#include <vcl/lineinfo.hxx>
#ifndef _METRIC_HXX //autogen
#include <vcl/metric.hxx>
#endif
#include <vcl/window.hxx>
#include <vcl/svapp.hxx>
#ifndef _COM_SUN_STAR_I18N_CHARACTERITERATORMODE_HDL_
#include <com/sun/star/i18n/CharacterIteratorMode.hdl>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HDL
#include <com/sun/star/i18n/WordType.hdl>
#endif
#include <breakit.hxx>
#include <viewsh.hxx>		// Bildschirmabgleich
#include <viewopt.hxx>		// Bildschirmabgleich abschalten, ViewOption
#include <fntcache.hxx>
#include <IDocumentSettingAccess.hxx>
#include <swfont.hxx>       // CH_BLANK + CH_BULLET
#include <wrong.hxx>
#include "dbg_lay.hxx"
#include <txtfrm.hxx>       // SwTxtFrm
#include <pagefrm.hxx>
#include <pagedesc.hxx> // SwPageDesc
#include <tgrditem.hxx>
#include <scriptinfo.hxx>
#include <editeng/brshitem.hxx>
#include <tools/shl.hxx>
#include <swmodule.hxx>
#include <accessibilityoptions.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <doc.hxx>
#include <editeng/fhgtitem.hxx>
#include <docsh.hxx>
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif

using namespace ::com::sun::star;

// globale Variablen, werden in FntCache.Hxx bekanntgegeben
// Der FontCache wird in TxtInit.Cxx _TXTINIT erzeugt und in _TXTEXIT geloescht
SwFntCache *pFntCache = NULL;
// Letzter Font, der durch ChgFntCache eingestellt wurde.
SwFntObj *pLastFont = NULL;
// Die "MagicNumber", die den Fonts zur Identifizierung verpasst wird
BYTE* pMagicNo = NULL;

Color *pWaveCol = 0;

long SwFntObj::nPixWidth;
MapMode* SwFntObj::pPixMap = NULL;
OutputDevice* SwFntObj::pPixOut = NULL;

extern USHORT UnMapDirection( USHORT nDir, const BOOL bVertFormat );
USHORT GetDefaultFontHeight( SwDrawTextInfo &rInf )
{
    SwDocShell* pDocShell = rInf.GetShell()->GetDoc()->GetDocShell();
    SfxStyleSheetBasePool* pBasePool = pDocShell->GetStyleSheetPool();

    String aString(SW_RES(STR_POOLCOLL_STANDARD));

    SfxStyleSheetBase* pStyle = pBasePool->Find( aString, (SfxStyleFamily)SFX_STYLE_FAMILY_PARA );
    SfxItemSet& aTmpSet = pStyle->GetItemSet();
    SvxFontHeightItem &aDefaultFontItem = (SvxFontHeightItem&)aTmpSet.Get(RES_CHRATR_CJK_FONTSIZE);
    return (USHORT)aDefaultFontItem.GetHeight();
}



/*************************************************************************
|*
|*	SwFntCache::Flush()
|*
|*	Ersterstellung		AMA 16. Dez. 94
|*	Letzte Aenderung	AMA 16. Dez. 94
|*
|*************************************************************************/

void SwFntCache::Flush( )
{
    if ( pLastFont )
    {
        pLastFont->Unlock();
        pLastFont = NULL;
    }
    SwCache::Flush( );
}

/*************************************************************************
|*
|*	SwFntObj::SwFntObj(), ~SwFntObj()
|*
|*	Ersterstellung		AMA 7. Nov. 94
|*	Letzte Aenderung	AMA 7. Nov. 94
|*
|*************************************************************************/

SwFntObj::SwFntObj( const SwSubFont &rFont, const void *pOwn, ViewShell *pSh ) :
    SwCacheObj( (void*)pOwn ),
    aFont( rFont ),
    pScrFont( NULL ),
    pPrtFont( &aFont ),
    pPrinter( NULL ),
    nPropWidth( rFont.GetPropWidth() )
{
    nZoom = pSh ? pSh->GetViewOptions()->GetZoom() : USHRT_MAX;
    nGuessedLeading = USHRT_MAX;
    nExtLeading = USHRT_MAX;
    nPrtAscent = USHRT_MAX;
    nPrtHeight = USHRT_MAX;
    bPaintBlank = ( UNDERLINE_NONE != aFont.GetUnderline()
                 || UNDERLINE_NONE != aFont.GetOverline()
                 || STRIKEOUT_NONE != aFont.GetStrikeout() )
                 && !aFont.IsWordLineMode();
    aFont.SetLanguage(rFont.GetLanguage());
}

SwFntObj::~SwFntObj()
{
    if ( pScrFont != pPrtFont )
        delete pScrFont;
    if ( pPrtFont != &aFont )
        delete pPrtFont;
}

void SwFntObj::CreatePrtFont( const OutputDevice& rPrt )
{
    if ( nPropWidth != 100 && pPrinter != &rPrt )
    {
        if( pScrFont != pPrtFont )
            delete pScrFont;
        if( pPrtFont != &aFont )
            delete pPrtFont;

        const Font aOldFnt( rPrt.GetFont() );
        ((OutputDevice&)rPrt).SetFont( aFont );
        const FontMetric aWinMet( rPrt.GetFontMetric() );
        ((OutputDevice&)rPrt).SetFont( aOldFnt );
        long nWidth = ( aWinMet.GetSize().Width() * nPropWidth ) / 100;

        if( !nWidth )
            ++nWidth;
        pPrtFont = new Font( aFont );
        pPrtFont->SetSize( Size( nWidth, aFont.GetSize().Height() ) );
        pScrFont = NULL;
    }
}

/*************************************************************************
 *
 *  bool lcl_IsFontAdjustNecessary( rOutDev, rRefDev )
 *
 *  returns whether we have to adjust the output font to resemble
 *  the formatting font
 *
 *  _Not_ necessary if
 *
 *  1. RefDef == OutDev (text formatting, online layout...)
 *  2. PDF export from online layout
 *  3. Prospect/PagePreview pringing
 *
 *************************************************************************/

bool lcl_IsFontAdjustNecessary( const OutputDevice& rOutDev,
                                const OutputDevice& rRefDev )
{
    return &rRefDev != &rOutDev &&
           OUTDEV_WINDOW != rRefDev.GetOutDevType() &&
           ( OUTDEV_PRINTER != rRefDev.GetOutDevType() ||
             OUTDEV_PRINTER != rOutDev.GetOutDevType() );
}

struct CalcLinePosData
{
    SwDrawTextInfo& rInf;
    Font& rFont;
    xub_StrLen nCnt;
    const BOOL bSwitchH2V;
    const BOOL bSwitchL2R;
    long nHalfSpace;
    sal_Int32* pKernArray;
    const BOOL bBidiPor;

    CalcLinePosData( SwDrawTextInfo& _rInf, Font& _rFont,
                      xub_StrLen _nCnt, const BOOL _bSwitchH2V, const BOOL _bSwitchL2R,
                      long _nHalfSpace, sal_Int32* _pKernArray, const BOOL _bBidiPor) :
        rInf( _rInf ),
        rFont( _rFont ),
        nCnt( _nCnt ),
        bSwitchH2V( _bSwitchH2V ),
        bSwitchL2R( _bSwitchL2R ),
        nHalfSpace( _nHalfSpace ),
        pKernArray( _pKernArray ),
        bBidiPor( _bBidiPor )
    {
    }
};

/** Function: lcl_calcLinePos

   Computes the start and end position of an underline. This function is called
   from the DrawText-method (for underlining misspelled words or smarttag terms).
*/

void lcl_calcLinePos( const CalcLinePosData &rData,
    Point &rStart, Point &rEnd, xub_StrLen nStart, xub_StrLen nWrLen )
{
   long nBlank = 0;
   const xub_StrLen nEnd = nStart + nWrLen;
   const long nTmpSpaceAdd = rData.rInf.GetSpace() / SPACING_PRECISION_FACTOR;

   if ( nEnd < rData.nCnt
       && CH_BLANK == rData.rInf.GetText().GetChar( rData.rInf.GetIdx() + nEnd ) )
   {
       if( nEnd + 1 == rData.nCnt )
           nBlank -= nTmpSpaceAdd;
       else
           nBlank -= rData.nHalfSpace;
   }

   // determine start, end and length of wave line
   sal_Int32 nKernStart = nStart ? rData.pKernArray[ USHORT( nStart - 1 ) ] : 0;
   sal_Int32 nKernEnd = rData.pKernArray[ USHORT( nEnd - 1 ) ];

   USHORT nDir = rData.bBidiPor ? 1800 :
       UnMapDirection( rData.rFont.GetOrientation(), rData.bSwitchH2V );

   switch ( nDir )
   {
   case 0 :
       rStart.X() += nKernStart;
       rEnd.X() = nBlank + rData.rInf.GetPos().X() + nKernEnd;
       rEnd.Y() = rData.rInf.GetPos().Y();
       break;
   case 900 :
       rStart.Y() -= nKernStart;
       rEnd.X() = rData.rInf.GetPos().X();
       rEnd.Y() = nBlank + rData.rInf.GetPos().Y() - nKernEnd;
       break;
   case 1800 :
       rStart.X() -= nKernStart;
       rEnd.X() = rData.rInf.GetPos().X() - nKernEnd - nBlank;
       rEnd.Y() = rData.rInf.GetPos().Y();
       break;
   case 2700 :
       rStart.Y() += nKernStart;
       rEnd.X() = rData.rInf.GetPos().X();
       rEnd.Y() = nBlank + rData.rInf.GetPos().Y() + nKernEnd;
       break;
   }

   if ( rData.bSwitchL2R )
   {
       rData.rInf.GetFrm()->SwitchLTRtoRTL( rStart );
       rData.rInf.GetFrm()->SwitchLTRtoRTL( rEnd );
   }

   if ( rData.bSwitchH2V )
   {
       rData.rInf.GetFrm()->SwitchHorizontalToVertical( rStart );
       rData.rInf.GetFrm()->SwitchHorizontalToVertical( rEnd );
   }
}

/*************************************************************************
 *
 *  USHORT SwFntObj::GetFontAscent( const OutputDevice& rOut )
 *
 *	Ersterstellung		AMA 7. Nov. 94
 *	Letzte Aenderung	AMA 7. Nov. 94
 *
 *  Beschreibung: liefern den Ascent des Fonts auf dem
 * 	gewuenschten Outputdevice zurueck, ggf. muss der Bildschirmfont erst
 *  erzeugt werden.
 *************************************************************************/

USHORT SwFntObj::GetFontAscent( const ViewShell *pSh, const OutputDevice& rOut )
{
    USHORT nRet = 0;
    const OutputDevice& rRefDev = pSh ? pSh->GetRefDev() : rOut;

    if ( pSh && lcl_IsFontAdjustNecessary( rOut, rRefDev ) )
    {
        CreateScrFont( *pSh, rOut );
        ASSERT( USHRT_MAX != nScrAscent, "nScrAscent is going berzerk" )
        nRet = nScrAscent;
    }
    else
    {
        if ( nPrtAscent == USHRT_MAX ) // DruckerAscent noch nicht bekannt?
        {
            CreatePrtFont( rOut );
            const Font aOldFnt( rRefDev.GetFont() );
            ((OutputDevice&)rRefDev).SetFont( *pPrtFont );
            const FontMetric aOutMet( rRefDev.GetFontMetric() );
            nPrtAscent = (USHORT) aOutMet.GetAscent();
            ( (OutputDevice&)rRefDev).SetFont( aOldFnt );
        }

        nRet = nPrtAscent;
    }

#if !defined(MACOSX) // #i89844# extleading is below the line for Mac
    // TODO: move extleading below the line for all platforms too
    nRet += GetFontLeading( pSh, rRefDev );
#endif

    ASSERT( USHRT_MAX != nRet, "GetFontAscent returned USHRT_MAX" )
    return nRet;
}

/*************************************************************************
 *
 *  USHORT SwFntObj::GetFontHeight( const OutputDevice* pOut )
 *
 *  Ersterstellung      AMA 7. Nov. 94
 *  Letzte Aenderung    AMA 7. Nov. 94
 *
 *  Beschreibung: liefern die H?he des Fonts auf dem
 *  gewuenschten Outputdevice zurueck, ggf. muss der Bildschirmfont erst
 *  erzeugt werden.
 *************************************************************************/

USHORT SwFntObj::GetFontHeight( const ViewShell* pSh, const OutputDevice& rOut )
{
    USHORT nRet = 0;
    const OutputDevice& rRefDev = pSh ? pSh->GetRefDev() : rOut;

    if ( pSh && lcl_IsFontAdjustNecessary( rOut, rRefDev ) )
    {
        CreateScrFont( *pSh, rOut );
        ASSERT( USHRT_MAX != nScrHeight, "nScrHeight is going berzerk" )
        nRet = nScrHeight + GetFontLeading( pSh, rRefDev );
    }
    else
    {
        if ( nPrtHeight == USHRT_MAX ) // PrinterHeight noch nicht bekannt?
        {
            CreatePrtFont( rOut );
            const Font aOldFnt( rRefDev.GetFont() );
            ((OutputDevice&)rRefDev).SetFont( *pPrtFont );
            nPrtHeight = static_cast<USHORT>(rRefDev.GetTextHeight());

#if OSL_DEBUG_LEVEL > 1
            // Check if vcl did not change the meading of GetTextHeight
            const FontMetric aOutMet( rRefDev.GetFontMetric() );
            long nTmpPrtHeight = (USHORT)aOutMet.GetAscent() + aOutMet.GetDescent();
            (void) nTmpPrtHeight;
            // #i106098#: do not compare with == here due to rounding error
            ASSERT( abs(nTmpPrtHeight - nPrtHeight) < 3,
                    "GetTextHeight != Ascent + Descent" );
#endif

            ((OutputDevice&)rRefDev).SetFont( aOldFnt );
        }

        nRet = nPrtHeight + GetFontLeading( pSh, rRefDev );
    }

    ASSERT( USHRT_MAX != nRet, "GetFontHeight returned USHRT_MAX" )
    return nRet;
}

USHORT SwFntObj::GetFontLeading( const ViewShell *pSh, const OutputDevice& rOut )
{
    USHORT nRet = 0;

    if ( pSh )
    {
        if ( USHRT_MAX == nGuessedLeading || USHRT_MAX == nExtLeading )
        {
            const Font aOldFnt( rOut.GetFont() );
            ((OutputDevice&)rOut).SetFont( *pPrtFont );
            const FontMetric aMet( rOut.GetFontMetric() );
            ((OutputDevice&)rOut).SetFont( aOldFnt );
            bSymbol = RTL_TEXTENCODING_SYMBOL == aMet.GetCharSet();
            GuessLeading( *pSh, aMet );
            nExtLeading = static_cast<USHORT>(aMet.GetExtLeading());
        }

        const IDocumentSettingAccess& rIDSA = *pSh->getIDocumentSettingAccess();
        const bool bBrowse = ( pSh->GetWin() &&
                               rIDSA.get(IDocumentSettingAccess::BROWSE_MODE) &&
                              !pSh->GetViewOptions()->IsPrtFormat() );

        if ( !bBrowse && rIDSA.get(IDocumentSettingAccess::ADD_EXT_LEADING) )
            nRet = nExtLeading;
        else
            nRet = nGuessedLeading;
    }

    ASSERT( USHRT_MAX != nRet, "GetFontLeading returned USHRT_MAX" )
    return nRet;
}


/*************************************************************************
 *
 *  SwFntObj::CreateScrFont( const ViewShell& rSh, const OutputDevice& rOut )
 *
 *	Ersterstellung		AMA 7. Nov. 94
 *	Letzte Aenderung	AMA 7. Nov. 94
 *
 *  pOut is the output device, not the reference device
 *
 *************************************************************************/

void SwFntObj::CreateScrFont( const ViewShell& rSh, const OutputDevice& rOut )
{
static sal_Char __READONLY_DATA sStandardString[] = "Dies ist der Teststring";

    if ( pScrFont )
        return;

    // any changes to the output device are reset at the end of the function
    OutputDevice* pOut = (OutputDevice*)&rOut;

    // Save old font
    Font aOldOutFont( pOut->GetFont() );

    nScrHeight = USHRT_MAX;

    // Condition for output font / refdev font adjustment
    OutputDevice* pPrt = &rSh.GetRefDev();

    if( !rSh.GetWin() ||
        !rSh.getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) ||
         rSh.GetViewOptions()->IsPrtFormat() )
    {
        // After CreatePrtFont pPrtFont is the font which is actually used
        // by the reference device
        CreatePrtFont( *pPrt );
        pPrinter = pPrt;

        // save old reference device font
        Font aOldPrtFnt( pPrt->GetFont() );

        // set the font used at the reference device at the reference device
        // and the output device
        pPrt->SetFont( *pPrtFont );
        pOut->SetFont( *pPrtFont );

        // This should be the default for pScrFont.
        pScrFont = pPrtFont;

        FontMetric aMet = pPrt->GetFontMetric( );
        bSymbol = RTL_TEXTENCODING_SYMBOL == aMet.GetCharSet();

        if ( USHRT_MAX == nGuessedLeading )
            GuessLeading( rSh, aMet );

        if ( USHRT_MAX == nExtLeading )
            nExtLeading = static_cast<USHORT>(aMet.GetExtLeading());

#if OSL_DEBUG_LEVEL > 1
        const XubString aDbgTxt1( pPrtFont->GetName() );
        const XubString aDbgTxt2( aMet.GetName() );
#endif

        if ( aMet.IsDeviceFont( ) )
        {
            if ( (RTL_TEXTENCODING_DONTKNOW == pPrtFont->GetCharSet() ||
                  FAMILY_DONTKNOW  == pPrtFont->GetFamily()  ||
                  PITCH_DONTKNOW   == pPrtFont->GetPitch()     ) &&
                 (RTL_TEXTENCODING_DONTKNOW == aMet.GetCharSet()  ||
                  FAMILY_DONTKNOW  == aMet.GetFamily()   ||
                  PITCH_DONTKNOW   == aMet.GetPitch()      )    )
            {
                // Das folgende ist teuer, aber selten: ein unbekannter Font
                // kann vom Drucker nicht vernuenftig zugeordnet werden. Dann
                // nehmen wir eben das Mapping des Bildschirms in Anspruch und
                // setzen den Familyname, Charset und Pitch wie dort. Dieser
                // Font wird nun nochmals auf dem Drucker eingestellt.
                Font aFnt1 = pOut->GetFontMetric();
                Font aFnt2( *pPrtFont );

                if (RTL_TEXTENCODING_DONTKNOW == pPrtFont->GetCharSet())
                    aFnt2.SetCharSet( aFnt1.GetCharSet() );
                if (FAMILY_DONTKNOW  == pPrtFont->GetFamily())
                    aFnt2.SetFamily( aFnt1.GetFamily() );
                if (PITCH_DONTKNOW   == pPrtFont->GetPitch())
                    aFnt2.SetPitch( aFnt1.GetPitch() );

                pPrt->SetFont( aFnt2 );
                aMet = pPrt->GetFontMetric( );
            }

            const XubString aStandardStr( sStandardString,
                RTL_TEXTENCODING_MS_1252 );

            // This is the reference width
            const long nOWidth = pPrt->GetTextWidth( aStandardStr );

            // Let's have a look what's the difference to the width
            // calculated for the output device using the font set at the
            // reference device
            long nSWidth = nOWidth - pOut->GetTextWidth( aStandardStr );
            nScrHeight = (USHORT) pOut->GetTextHeight();

            // Um Aerger mit dem Generic Printer aus dem Wege zu gehen.
            if( aMet.GetSize().Height() )
            {
                BOOL bScrSymbol = FALSE;
                CharSet ePrtChSet = aMet.GetCharSet();
                // NoSymbol bedeutet, dass der Drucker sich fuer einen
                // Nicht-Symbol-Font entschieden hat.
                BOOL bNoSymbol = ( RTL_TEXTENCODING_DONTKNOW != ePrtChSet &&
                                   RTL_TEXTENCODING_SYMBOL != ePrtChSet );
                if ( bNoSymbol )
                    bScrSymbol = RTL_TEXTENCODING_SYMBOL ==
                                 pOut->GetFontMetric().GetCharSet();
                Size aTmp( aMet.GetSize() );

                if( aTmp.Width() && !pPrtFont->GetSize().Width() )
                {
                    aTmp.Width() = 0;
                    aMet.SetSize( aTmp );
                }

                // Now we set the metrics used at the reference device at the
                // output device
                pOut->SetFont( aMet );

                if( bNoSymbol && ( bScrSymbol != ( RTL_TEXTENCODING_SYMBOL ==
                                        pOut->GetFontMetric().GetCharSet() ) ) )
                {
                    // Hier landen wir, wenn der Drucker keinen Symbolfont waehlt,
                    // aber genau einer der beiden Screenfonts ein Symbolfont ist.
                    // Wir nehmen dann eben den anderen.
                    if ( bScrSymbol )
                        pScrFont = new Font( aMet ); // mit Abgleich
                    else
                        pOut->SetFont( *pPrtFont ); // ohne Abgleich
                }
                else
                {
                    // Let's have a look what's the difference to the width
                    // calculated for the output device using the metrics set at
                    // the reference device
                    long nPWidth = nOWidth - pOut->GetTextWidth( aStandardStr );

                    // We prefer smaller fonts
                    BYTE nNeg = 0;
                    if ( nSWidth<0 ) { nSWidth *= -2; nNeg = 1; }
                    if ( nPWidth<0 ) { nPWidth *= -2; nNeg |= 2; }

                    // nSWidth = Difference between string width on reference device
                    //           and string width on output device with user font set.
                    // nPWidth = Difference between string width on reference device
                    //           and string width on output device with metric obtained
                    //           from reference device.
                    // We prefer to take the font with the smaller deviation,
                    // exception: keep the original font unless the deviation
                    // is really bad (at least 3%)
            // Since the test string is neither localized nor has a high resemblance
            // of the "real text for this font" a higher deviation is reasonable

                    if ( (nSWidth <= nPWidth)
                    ||   (nSWidth * 32 <= nOWidth ) )
                    {
                        // No adjustment, we take the same font for the output
                        // device like for the reference device
                        pOut->SetFont( *pPrtFont );
                        pScrFont = pPrtFont;
                        nPWidth = nSWidth;
                        nNeg &= 1;
                    }
                    else
                    {
                        // The metrics give a better result. So we build
                        // a new font for the output device based on the
                        // metrics used at the reference device
                        pScrFont = new Font( aMet ); // mit Abgleich
                        nSWidth = nPWidth;
                        nNeg &= 2;
                    }

                    //
                    // now pScrFont is set to the better font and this should
                    // be set at the output device
                    //

                    // we still have to check if the choosed font is not to wide
                    if( nNeg && nOWidth )
                    {
                        nPWidth *= 100;
                        nPWidth /= nOWidth;

                        // if the screen font is too wide, we try to reduce
                        // the font height and get a smaller one
                        if( nPWidth > 25 )
                        {
                            if( nPWidth > 80 )
                                nPWidth = 80;
                            nPWidth = 100 - nPWidth/4;
                            Size aTmpSize = pScrFont->GetSize();
                            aTmpSize.Height() *= nPWidth;
                            aTmpSize.Height() /= 100;
                            if( aTmpSize.Width() )
                            {
                                aTmpSize.Width() *= nPWidth;
                                aTmpSize.Width() /= 100;
                            }
                            Font *pNew = new Font( *pScrFont );
                            pNew->SetSize( aTmpSize );
                            pOut->SetFont( *pNew );
                            nPWidth = nOWidth -
                                      pOut->GetTextWidth( aStandardStr );
                            if( nPWidth < 0 ) { nPWidth *= -2; }
                            if( nPWidth < nSWidth )
                            {
                                if( pScrFont != pPrtFont )
                                    delete pScrFont;
                                pScrFont = pNew;
                            }
                            else
                            {
                                delete pNew;
                                pOut->SetFont( *pScrFont );
                            }
                        }
                    }
                }
            }
        }

        // reset the original reference device font
        pPrt->SetFont( aOldPrtFnt );
    }
    else
    {
        bSymbol = RTL_TEXTENCODING_SYMBOL == aFont.GetCharSet();
        if ( nGuessedLeading == USHRT_MAX )
            nGuessedLeading = 0;

        // no external leading in browse mode
        if ( nExtLeading == USHRT_MAX )
            nExtLeading = 0;

        pScrFont = pPrtFont;
    }

    // Zoomfaktor ueberpruefen, z.B. wg. PrtOle2 beim Speichern
    {
        // Sollte der Zoomfaktor des OutputDevices nicht mit dem der View-
        // Options uebereinstimmen, so darf dieser Font nicht gecacht
        // werden, deshalb wird der Zoomfaktor auf einen "ungueltigen" Wert
        // gesetzt.
        long nTmp;
        if( pOut->GetMapMode().GetScaleX().IsValid() &&
            pOut->GetMapMode().GetScaleY().IsValid() &&
            pOut->GetMapMode().GetScaleX() == pOut->GetMapMode().GetScaleY() )
        {
            nTmp = ( 100 * pOut->GetMapMode().GetScaleX().GetNumerator() ) /
                     pOut->GetMapMode().GetScaleX().GetDenominator();
        }
        else
            nTmp = 0;
        if( nTmp != nZoom )
            nZoom = USHRT_MAX - 1;
    }

    nScrAscent = (USHORT)pOut->GetFontMetric().GetAscent();
    if ( USHRT_MAX == nScrHeight )
        nScrHeight = (USHORT)pOut->GetTextHeight();

    // reset original output device font
    pOut->SetFont( aOldOutFont );
}


void SwFntObj::GuessLeading( const ViewShell&
#if defined(WNT) || defined(WIN) || defined(PM2)
                             rSh
#endif
                             , const FontMetric& rMet )
{
    // If leading >= 5, this seems to be enough leading.
    // Nothing has to be done.
    if ( rMet.GetIntLeading() >= 5 )
    {
        nGuessedLeading = 0;
        return;
    }

#if defined(WNT) || defined(WIN) || defined(PM2)
    OutputDevice *pWin = rSh.GetWin() ?
                         rSh.GetWin() :
                         GetpApp()->GetDefaultDevice();
    if ( pWin )
    {
        MapMode aTmpMap( MAP_TWIP );
        MapMode aOldMap = pWin->GetMapMode( );
        pWin->SetMapMode( aTmpMap );
        const Font aOldFnt( pWin->GetFont() );
        pWin->SetFont( *pPrtFont );
        const FontMetric aWinMet( pWin->GetFontMetric() );
        const USHORT nWinHeight = USHORT( aWinMet.GetSize().Height() );
        if( pPrtFont->GetName().Search( aWinMet.GetName() ) < USHRT_MAX )
        {
            // Wenn das Leading auf dem Window auch 0 ist, dann
            // muss es auch so bleiben (vgl. StarMath!).
            long nTmpLeading = (long)aWinMet.GetIntLeading();
             // einen Versuch haben wir noch wg. 31003:
            if( nTmpLeading <= 0 )
            {
                pWin->SetFont( rMet );
                nTmpLeading = (long)pWin->GetFontMetric().GetIntLeading();
                if( nTmpLeading < 0 )
                    nGuessedLeading = 0;
                else
                    nGuessedLeading = USHORT(nTmpLeading);
            }
            else
            {
                nGuessedLeading = USHORT(nTmpLeading);
                // Manta-Hack #50153#:
                // Wer beim Leading luegt, luegt moeglicherweise auch beim
                // Ascent/Descent, deshalb wird hier ggf. der Font ein wenig
                // tiefergelegt, ohne dabei seine Hoehe zu aendern.
                long nDiff = Min( rMet.GetDescent() - aWinMet.GetDescent(),
                    aWinMet.GetAscent() - rMet.GetAscent() - nTmpLeading );
                if( nDiff > 0 )
                {
                    ASSERT( nPrtAscent < USHRT_MAX, "GuessLeading: PrtAscent-Fault" );
                    if ( nPrtAscent < USHRT_MAX )
                        nPrtAscent = nPrtAscent + (USHORT)(( 2 * nDiff ) / 5);
                }
            }
        }
        else
        {
            // Wenn alle Stricke reissen, nehmen wir 15% der
            // Hoehe, ein von CL empirisch ermittelter Wert.
            nGuessedLeading = (nWinHeight * 15) / 100;
        }
        pWin->SetFont( aOldFnt );
        pWin->SetMapMode( aOldMap );
    }
    else
#endif
        nGuessedLeading = 0;
}

/*************************************************************************
 *
 *	void SwFntObj::SetDeviceFont( const OutputDevice *pOut ),
 *
 *	Ersterstellung		AMA 7. Nov. 94
 *	Letzte Aenderung	AMA 7. Nov. 94
 *
 *  Beschreibung: stellt den Font am gewuenschten OutputDevice ein,
 *  am Bildschirm muss eventuell erst den Abgleich durchgefuehrt werden.
 *
 *************************************************************************/

void SwFntObj::SetDevFont( const ViewShell *pSh, OutputDevice& rOut )
{
    const OutputDevice& rRefDev = pSh ? pSh->GetRefDev() : rOut;

    if ( pSh && lcl_IsFontAdjustNecessary( rOut, rRefDev ) )
    {
        CreateScrFont( *pSh, rOut );
        if( !GetScrFont()->IsSameInstance( rOut.GetFont() ) )
            rOut.SetFont( *pScrFont );
        if( pPrinter && ( !pPrtFont->IsSameInstance( pPrinter->GetFont() ) ) )
            pPrinter->SetFont( *pPrtFont );
    }
    else
    {
        CreatePrtFont( rOut );
        if( !pPrtFont->IsSameInstance( rOut.GetFont() ) )
            rOut.SetFont( *pPrtFont );
    }

    // Here, we actually do not need the leading values, but by calling
    // GetFontLeading() we assure that the values are calculated for later use.
    GetFontLeading( pSh, rRefDev );
}

#define WRONG_SHOW_MIN 5
#define WRONG_SHOW_SMALL 11
#define WRONG_SHOW_MEDIUM 15

/*************************************************************************
 *
 * void SwFntObj::DrawText( ... )
 *
 *	Ersterstellung		AMA 16. Dez. 94
 *	Letzte Aenderung	AMA 16. Dez. 94
 *
 *  Beschreibung: Textausgabe
 * 					auf dem Bildschirm 			=> DrawTextArray
 * 					auf dem Drucker, !Kerning 	=> DrawText
 * 					auf dem Drucker + Kerning	=> DrawStretchText
 *
 *************************************************************************/

BYTE lcl_WhichPunctuation( xub_Unicode cChar )
{
    if ( ( cChar < 0x3001 || cChar > 0x3002 ) &&
            ( cChar < 0x3008 || cChar > 0x3011 ) &&
            ( cChar < 0x3014 || cChar > 0x301F ) &&
              0xFF62 != cChar && 0xFF63 != cChar )
        // no punctuation
        return SwScriptInfo::NONE;
    else if ( 0x3001 == cChar || 0x3002 == cChar ||
              0x3009 == cChar || 0x300B == cChar ||
              0x300D == cChar || 0x300F == cChar ||
              0x3011 == cChar || 0x3015 == cChar ||
              0x3017 == cChar || 0x3019 == cChar ||
              0x301B == cChar || 0x301E == cChar ||
              0x301F == cChar || 0xFF63 == cChar )
        // right punctuation
        return SwScriptInfo::SPECIAL_RIGHT;

    return SwScriptInfo::SPECIAL_LEFT;
}

static sal_Bool lcl_IsMonoSpaceFont( const OutputDevice& rOut )
{
    const String aStr1( xub_Unicode( 0x3008 ) );
    const String aStr2( xub_Unicode( 0x307C ) );
    const long nWidth1 = rOut.GetTextWidth( aStr1 );
    const long nWidth2 = rOut.GetTextWidth( aStr2 );
    return nWidth1 == nWidth2;
}

// ER 09.07.95 20:34
// mit -Ox Optimierung stuerzt's unter win95 ab
// JP 12.07.95: unter WNT auch (i386);       Alpha ??
// global optimization off
#ifdef _MSC_VER
#pragma optimize("g",off)
#endif

/* This helper structure (SwForbidden) contains the already marked parts of the string
    to avoid double lines (e.g grammar + spell check error) */

typedef std::vector< std::pair< xub_StrLen, xub_StrLen > > SwForbidden;

static void lcl_DrawLineForWrongListData(
    SwForbidden &rForbidden,
    const SwDrawTextInfo    &rInf,
    const SwWrongList       *pWList,
    const CalcLinePosData   &rCalcLinePosData,
    const Size              &rPrtFontSize )
{
    if (!pWList)
        return;

    xub_StrLen nStart = rInf.GetIdx();
    xub_StrLen nWrLen = rInf.GetLen();

    // check if respective data is available in the current text range
    if (pWList->Check( nStart, nWrLen ))
    {
        // get line color to use...
        Color aLineColor;
        if (pWList == rInf.GetWrong())  // ... for spell checking
            aLineColor = SwViewOption::GetSpellColor();
        else if (pWList == rInf.GetGrammarCheck())  // ... for grammar checking
            // currently there is no specific color for grammar check errors available in the configuration
            aLineColor = Color( COL_LIGHTBLUE );
        else if (pWList == rInf.GetSmartTags())  // ... for smart tags
            aLineColor = SwViewOption::GetSmarttagColor();

        long nHght = rInf.GetOut().LogicToPixel( rPrtFontSize ).Height();

        // Draw wavy lines for spell and grammar errors only if font is large enough.
        // Lines for smart tags will always be drawn.
        if (pWList == rInf.GetSmartTags() || WRONG_SHOW_MIN < nHght)
        {
            SwForbidden::iterator pIter = rForbidden.begin();
            if (rInf.GetOut().GetConnectMetaFile())
                rInf.GetOut().Push();

            const Color aCol( rInf.GetOut().GetLineColor() );
            const BOOL bColSave = aCol != aLineColor;
            if (bColSave)
                rInf.GetOut().SetLineColor( aLineColor );

            // iterate over all ranges stored in the respective SwWrongList
            do
            {
                nStart = nStart - rInf.GetIdx();

                const xub_StrLen nEnd = nStart + nWrLen;
                xub_StrLen nNext = nStart;
                while( nNext < nEnd )
                {
                    while( pIter != rForbidden.end() && pIter->second <= nNext )
                        ++pIter;
                    xub_StrLen nNextStart = nNext;
                    xub_StrLen nNextEnd = nEnd;
                    if( pIter == rForbidden.end() || nNextEnd <= pIter->first )
                    {
                        // No overlapping mark up found
                        std::pair< xub_StrLen, xub_StrLen > aNew;
                        aNew.first = nNextStart;
                        aNew.second = nNextEnd;
                        rForbidden.insert( pIter, aNew );
                        pIter = rForbidden.begin();
                        nNext = nEnd;
                    }
                    else
                    {
                        nNext = pIter->second;
                        if( nNextStart < pIter->first )
                        {
                            nNextEnd = pIter->first;
                            pIter->first = nNextStart;
                        }
                        else
                            continue;
                    }
                    // determine line pos
                    Point aStart( rInf.GetPos() );
                    Point aEnd;
                    lcl_calcLinePos( rCalcLinePosData, aStart, aEnd, nNextStart, nNextEnd - nNextStart );

                    // draw line for smart tags?
                    if (pWList == rInf.GetSmartTags())
                    {
                        aStart.Y() +=30;
                        aEnd.Y() +=30;

                        LineInfo aLineInfo( LINE_DASH );
                        aLineInfo.SetDistance( 40 );
                        aLineInfo.SetDashLen( 1 );
                        aLineInfo.SetDashCount(1);

                        rInf.GetOut().DrawLine( aStart, aEnd, aLineInfo );
                    }
                    else    // draw wavy lines for spell or grammar errors
                    {
                        // get wavy line type to use
                        USHORT nWave =
                            WRONG_SHOW_MEDIUM < nHght ? WAVE_NORMAL :
                            ( WRONG_SHOW_SMALL < nHght ? WAVE_SMALL : WAVE_FLAT );

                        rInf.GetOut().DrawWaveLine( aStart, aEnd, nWave );
                    }
                }

                nStart = nEnd + rInf.GetIdx();
                nWrLen = rInf.GetIdx() + rInf.GetLen() - nStart;
            }
            while (nWrLen && pWList->Check( nStart, nWrLen ));

            if (bColSave)
                rInf.GetOut().SetLineColor( aCol );

            if (rInf.GetOut().GetConnectMetaFile())
                rInf.GetOut().Pop();
        }
    }
}


void SwFntObj::DrawText( SwDrawTextInfo &rInf )
{
    ASSERT( rInf.GetShell(), "SwFntObj::DrawText without shell" )

    OutputDevice& rRefDev = rInf.GetShell()->GetRefDev();
    OutputDevice* pWin = rInf.GetShell()->GetWin();
    const IDocumentSettingAccess* pIDSA = rInf.GetShell()->getIDocumentSettingAccess();

    // true if pOut is the printer and the printer has been used for formatting
    const BOOL bPrt = OUTDEV_PRINTER == rInf.GetOut().GetOutDevType() &&
                      OUTDEV_PRINTER == rRefDev.GetOutDevType();
    const BOOL bBrowse = ( pWin &&
                           pIDSA->get(IDocumentSettingAccess::BROWSE_MODE) &&
                          !rInf.GetShell()->GetViewOptions()->IsPrtFormat() &&
                          !rInf.GetBullet() &&
                           ( rInf.GetSpace() || !rInf.GetKern() ) &&
                          !rInf.GetWrong() &&
                          !rInf.GetGrammarCheck() &&
                          !rInf.GetSmartTags() &&
                          !rInf.GetGreyWave() );

    // bDirectPrint indicates that we can enter the branch which calls
    // the DrawText functions instead of calling the DrawTextArray functions
    const BOOL bDirectPrint = bPrt || bBrowse;

    // Condition for output font / refdev font adjustment
    const BOOL bUseScrFont =
        lcl_IsFontAdjustNecessary( rInf.GetOut(), rRefDev );

    Font* pTmpFont = bUseScrFont ? pScrFont : pPrtFont;

    //
    //  bDirectPrint and bUseScrFont should have these values:
    //
    //  Outdev / RefDef  | Printer | VirtPrinter | Window
    // ----------------------------------------------------
    //  Printer          | 1 - 0   | 0 - 1       | -
    // ----------------------------------------------------
    //  VirtPrinter/PDF  | 0 - 1   | 0 - 1       | -
    // ----------------------------------------------------
    //  Window/VirtWindow| 0 - 1   | 0 - 1       | 1 - 0
    //
    // Exception: During painting of a Writer OLE object, we do not have
    // a window. Therefore bUseSrcFont is always 0 in this case.
    //

#ifdef DBG_UTIL

    const BOOL bNoAdjust = bPrt ||
            (  pWin &&
               pIDSA->get(IDocumentSettingAccess::BROWSE_MODE) &&
              !rInf.GetShell()->GetViewOptions()->IsPrtFormat() );

    if ( OUTDEV_PRINTER == rInf.GetOut().GetOutDevType() )
    {
        // Printer output
        if ( OUTDEV_PRINTER == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 1 && bUseScrFont == 0, "Outdev Check failed" )
        }
        else if ( OUTDEV_VIRDEV == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 0 && bUseScrFont == 1, "Outdev Check failed" )
        }
        else
        {
            ASSERT( sal_False, "Outdev Check failed" )
        }
    }
    else if ( OUTDEV_VIRDEV == rInf.GetOut().GetOutDevType() && ! pWin )
    {
        // PDF export
        if ( OUTDEV_PRINTER == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 0 && bUseScrFont == 1, "Outdev Check failed" )
        }
        else if ( OUTDEV_VIRDEV == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 0 && bUseScrFont == 1, "Outdev Check failed" )
        }
        else
        {
            ASSERT( sal_False, "Outdev Check failed" )
        }
    }
    else if ( OUTDEV_WINDOW == rInf.GetOut().GetOutDevType() ||
               ( OUTDEV_VIRDEV == rInf.GetOut().GetOutDevType() && pWin ) )
    {
        // Window or virtual window
        if ( OUTDEV_PRINTER == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 0 && bUseScrFont == 1, "Outdev Check failed" )
        }
        else if ( OUTDEV_VIRDEV == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 0 && bUseScrFont == 1, "Outdev Check failed" )
        }
        else if ( OUTDEV_WINDOW == rRefDev.GetOutDevType() )
        {
            ASSERT( bNoAdjust == 1 && bUseScrFont == 0, "Outdev Check failed" )
        }
        else
        {
            ASSERT( sal_False, "Outdev Check failed" )
        }
    }
    else
    {
            ASSERT( sal_False, "Outdev Check failed" )
    }

#endif

    // robust: better use the printer font instead of using no font at all
    ASSERT( pTmpFont, "No screen or printer font?" );
    if ( ! pTmpFont )
        pTmpFont = pPrtFont;

    // HACK: UNDERLINE_WAVE darf nicht mehr missbraucht werden, daher
    // wird die graue Wellenlinie des ExtendedAttributSets zunaechst
    // in der Fontfarbe erscheinen.

    const BOOL bSwitchH2V = rInf.GetFrm() && rInf.GetFrm()->IsVertical();
    const BOOL bSwitchL2R = rInf.GetFrm() && rInf.GetFrm()->IsRightToLeft() &&
                            ! rInf.IsIgnoreFrmRTL();
    const ULONG nMode = rInf.GetOut().GetLayoutMode();
    const BOOL bBidiPor = ( bSwitchL2R !=
                            ( 0 != ( TEXT_LAYOUT_BIDI_RTL & nMode ) ) );

    // be sure to have the correct layout mode at the printer
    if ( pPrinter )
    {
        pPrinter->SetLayoutMode( rInf.GetOut().GetLayoutMode() );
        pPrinter->SetDigitLanguage( rInf.GetOut().GetDigitLanguage() );
    }

    Point aPos( rInf.GetPos() );
    if( !bPrt )
    {
        if( rInf.GetpOut() != pPixOut || rInf.GetOut().GetMapMode() != *pPixMap )
        {
            *pPixMap = rInf.GetOut().GetMapMode();
            pPixOut = rInf.GetpOut();
            Size aTmp( 1, 1 );
            nPixWidth = rInf.GetOut().PixelToLogic( aTmp ).Width();
        }

        aPos.X() += rInf.GetFrm()->IsRightToLeft() ? 0 : nPixWidth;
    }

    Color aOldColor( pTmpFont->GetColor() );
    sal_Bool bChgColor = rInf.ApplyAutoColor( pTmpFont );
    if( !pTmpFont->IsSameInstance( rInf.GetOut().GetFont() ) )
        rInf.GetOut().SetFont( *pTmpFont );
    if ( bChgColor )
        pTmpFont->SetColor( aOldColor );

    if ( STRING_LEN == rInf.GetLen() )
        rInf.SetLen( rInf.GetText().Len() );


    //
    // ASIAN LINE AND CHARACTER GRID MODE START: snap to characters
    //

    if ( rInf.GetFrm() && rInf.SnapToGrid() && rInf.GetFont() &&
         SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && pGrid->IsSnapToChars())
        {
            //for textgrid refactor
            //const USHORT nGridWidth = pGrid->GetBaseHeight();
            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            const USHORT nGridWidth = GETGRIDWIDTH(pGrid, pDoc);
            sal_Int32* pKernArray = new sal_Int32[rInf.GetLen()];

            if ( pPrinter )
                pPrinter->GetTextArray( rInf.GetText(), pKernArray,
                                        rInf.GetIdx(), rInf.GetLen() );
            else
                rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                            rInf.GetIdx(), rInf.GetLen() );

            long nWidthPerChar = pKernArray[ rInf.GetLen() - 1 ] / rInf.GetLen();

            const ULONG i = nWidthPerChar ?
                                ( nWidthPerChar - 1 ) / nGridWidth + 1:
                                1;

            nWidthPerChar = i * nGridWidth;

            // position of first character, we take the printer position
            long nCharWidth = pKernArray[ 0 ];
            ULONG nHalfWidth = nWidthPerChar / 2;

            long nNextFix;

            // punctuation characters are not centered
            xub_Unicode cChar = rInf.GetText().GetChar( rInf.GetIdx() );
            BYTE nType = lcl_WhichPunctuation( cChar );
            switch ( nType )
            {
            case SwScriptInfo::NONE :
                aPos.X() += ( nWidthPerChar - nCharWidth ) / 2;
                nNextFix = nCharWidth / 2;
                break;
            case SwScriptInfo::SPECIAL_RIGHT :
                nNextFix = nHalfWidth;
                break;
            default:
                aPos.X() += nWidthPerChar - nCharWidth;
                nNextFix = nCharWidth - nHalfWidth;
            }

            // calculate offsets
            for ( xub_StrLen j = 1; j < rInf.GetLen(); ++j )
            {
                long nScr = pKernArray[ j ] - pKernArray[ j - 1 ];
                nNextFix += nWidthPerChar;

                // punctuation characters are not centered
                cChar = rInf.GetText().GetChar( rInf.GetIdx() + j );
                nType = lcl_WhichPunctuation( cChar );
                switch ( nType )
                {
                case SwScriptInfo::NONE :
                    pKernArray[ j - 1 ] = nNextFix - ( nScr / 2 );
                    break;
                case SwScriptInfo::SPECIAL_RIGHT :
                    pKernArray[ j - 1 ] = nNextFix - nHalfWidth;
                    break;
                default:
                    pKernArray[ j - 1 ] = nNextFix + nHalfWidth - nScr;
                }
            }

            // the layout engine requires the total width of the output
            pKernArray[ rInf.GetLen() - 1 ] = rInf.GetWidth() -
                                              aPos.X() + rInf.GetPos().X() ;

            if ( bSwitchH2V )
                rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

            rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                pKernArray, rInf.GetIdx(), rInf.GetLen() );

            delete[] pKernArray;
            return;
        }
    }

    // For text grid refactor
    // ASIAN LINE AND CHARACTER GRID MODE START: not snap to characters
    //
    if ( rInf.GetFrm() && rInf.SnapToGrid() && rInf.GetFont() &&
         SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )

        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && !pGrid->IsSnapToChars() )
        {
            const USHORT  nDefaultFontHeight = GetDefaultFontHeight( rInf );

            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            long nGridWidthAdd = GETGRIDWIDTH(pGrid, pDoc);
            if( SW_LATIN == rInf.GetFont()->GetActual() )
                nGridWidthAdd = ( nGridWidthAdd - nDefaultFontHeight ) / 2;
            else
                nGridWidthAdd = nGridWidthAdd - nDefaultFontHeight;

            sal_Int32*  pKernArray = new sal_Int32[rInf.GetLen()];

            if ( pPrinter )
                pPrinter->GetTextArray( rInf.GetText(), pKernArray,
                rInf.GetIdx(), rInf.GetLen() );
            else
                rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                rInf.GetIdx(), rInf.GetLen() );
            if ( bSwitchH2V )
                rInf.GetFrm()->SwitchHorizontalToVertical( aPos );
            if ( rInf.GetSpace() || rInf.GetKanaComp())
            {
                long nSpaceAdd = rInf.GetSpace() / SPACING_PRECISION_FACTOR;
                sal_Bool bSpecialJust = sal_False;
                if ( rInf.GetFont() && rInf.GetLen() )
                {
                    const SwScriptInfo* pSI = rInf.GetScriptInfo();
                    const BYTE nActual = rInf.GetFont()->GetActual();
                    ///Kana Compression
                    if( SW_CJK == nActual && rInf.GetKanaComp() &&
                        pSI && pSI->CountCompChg() &&
                        lcl_IsMonoSpaceFont( *(rInf.GetpOut()) ) )
                    {
                        pSI->Compress( pKernArray,rInf.GetIdx(), rInf.GetLen(),
                            rInf.GetKanaComp(), (USHORT)aFont.GetSize().Height(),&aPos );
                        bSpecialJust = sal_True;
                    }
                    ///Asian Justification
                    if ( ( SW_CJK == nActual || SW_LATIN == nActual ) && nSpaceAdd )
                    {
                        LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CJK );
                        if ( LANGUAGE_KOREAN != aLang && LANGUAGE_KOREAN_JOHAB != aLang)
                        {
                            long nSpaceSum = nSpaceAdd;
                            for ( USHORT nI = 0; nI < rInf.GetLen(); ++nI )
                            {
                                pKernArray[ nI ] += nSpaceSum;
                                nSpaceSum += nSpaceAdd;
                            }
                            bSpecialJust = sal_True;
                            nSpaceAdd = 0;
                        }
                    }
                    long nGridAddSum = nGridWidthAdd;
                    for(xub_StrLen i = 0; i < rInf.GetLen(); i++,nGridAddSum += nGridWidthAdd )
                    {
                        pKernArray[i] += nGridAddSum;
                    }
                    long nKernSum = rInf.GetKern();
                    if ( bSpecialJust || rInf.GetKern() )
                    {
                        for( xub_StrLen i = 0; i < rInf.GetLen(); i++, nKernSum += rInf.GetKern() )
                        {
                            if ( CH_BLANK == rInf.GetText().GetChar(rInf.GetIdx()+i) )
                                nKernSum += nSpaceAdd;
                            pKernArray[i] += nKernSum;
                        }
                        ///With through/uderstr. Grouped style requires a blank at the end
                        ///of a text edition special measures:
                        if( bPaintBlank && rInf.GetLen() && (CH_BLANK ==
                            rInf.GetText().GetChar( rInf.GetIdx() + rInf.GetLen() - 1) ) )
                        {
                            ///If it concerns a singular, underlined space acts,
                            ///we must spend two:
                            if( 1 == rInf.GetLen() )
                            {
                                pKernArray[0] = rInf.GetWidth() + nSpaceAdd;
                                rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                    pKernArray, rInf.GetIdx(), 1 );
                            }
                            else
                            {
                                pKernArray[ rInf.GetLen() - 2] += nSpaceAdd;
                                rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                    pKernArray, rInf.GetIdx(), rInf.GetLen() );
                            }
                        }
                        else
                        {
                            rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                pKernArray, rInf.GetIdx(), rInf.GetLen() );
                        }
                    }
                    else
                    {
                        Point aTmpPos( aPos );
                        xub_StrLen i;
                        xub_StrLen j = 0;
                        long nSpaceSum = 0;
                        for( i = 0; i < rInf.GetLen(); i++ )
                        {
                            if( CH_BLANK == rInf.GetText().GetChar( rInf.GetIdx() + i) )
                            {
                                nSpaceSum += nSpaceAdd;
                                if( j < i)
                                    rInf.GetOut().DrawText( aTmpPos, rInf.GetText(),
                                    rInf.GetIdx() + j, i - j );
                                j = i + 1;
                                pKernArray[i] = pKernArray[i] + nSpaceSum;
                                aTmpPos.X() = aPos.X() + pKernArray[ i ] + nKernSum;
                            }
                        }
                        if( j < i )
                            rInf.GetOut().DrawText( aTmpPos, rInf.GetText(),
                            rInf.GetIdx() +j , i - j );
                    }
                }
            }
            else
            {
                //long nKernAdd = rInf.GetKern();
        long nKernAdd = 0;
                long nGridAddSum = nGridWidthAdd + nKernAdd;
                for(xub_StrLen i = 0; i < rInf.GetLen(); i++,nGridAddSum += nGridWidthAdd + nKernAdd )
                {
                    pKernArray[i] += nGridAddSum;
                }
                rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                    pKernArray, rInf.GetIdx(), rInf.GetLen() );
            }
            delete[] pKernArray;
            return;
        }
    }

    //
    // DIRECT PAINTING WITHOUT SCREEN ADJUSTMENT
    //

    if ( bDirectPrint )
    {
        const Fraction aTmp( 1, 1 );
        BOOL bStretch = rInf.GetWidth() && ( rInf.GetLen() > 1 ) && bPrt
                        && ( aTmp != rInf.GetOut().GetMapMode().GetScaleX() );

        if ( bSwitchL2R )
            rInf.GetFrm()->SwitchLTRtoRTL( aPos );

        if ( bSwitchH2V )
            rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

        // In the good old days we used to have a simple DrawText if the
        // output device is the printer. Now we need a DrawTextArray if
        // 1. KanaCompression is enabled
        // 2. Justified alignment
        // Simple kerning is handled by DrawStretchText
        if( rInf.GetSpace() || rInf.GetKanaComp() )
        {
            sal_Int32 *pKernArray = new sal_Int32[ rInf.GetLen() ];
            rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                       rInf.GetIdx(), rInf.GetLen() );

            if( bStretch )
            {
                xub_StrLen nZwi = rInf.GetLen() - 1;
                long nDiff = rInf.GetWidth() - pKernArray[ nZwi ]
                             - rInf.GetLen() * rInf.GetKern();
                long nRest = nDiff % nZwi;
                long nAdd;
                if( nRest < 0 )
                {
                    nAdd = -1;
                    nRest += nZwi;
                }
                else
                {
                    nAdd = +1;
                    nRest = nZwi - nRest;
                }
                nDiff /= nZwi;
                long nSum = nDiff;
                for( xub_StrLen i = 0; i < nZwi; )
                {
                    pKernArray[ i ] += nSum;
                    if( ++i == nRest )
                        nDiff += nAdd;
                    nSum += nDiff;
                }
            }

            //
            // Modify Array for special justifications
            //
            long nSpaceAdd = rInf.GetSpace() / SPACING_PRECISION_FACTOR;
            sal_Bool bSpecialJust = sal_False;

            if ( rInf.GetFont() && rInf.GetLen() )
            {
                const SwScriptInfo* pSI = rInf.GetScriptInfo();
                const BYTE nActual = rInf.GetFont()->GetActual();

                // Kana Compression
                if ( SW_CJK == nActual && rInf.GetKanaComp() &&
                     pSI && pSI->CountCompChg() &&
                     lcl_IsMonoSpaceFont( rInf.GetOut() ) )
                {
                    pSI->Compress( pKernArray, rInf.GetIdx(), rInf.GetLen(),
                                   rInf.GetKanaComp(),
                                   (USHORT)aFont.GetSize().Height(), &aPos );
                    bSpecialJust = sal_True;
                }

                // Asian Justification
                if ( SW_CJK == nActual && nSpaceAdd )
                {
                    LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CJK );

                    if ( LANGUAGE_KOREAN != aLang && LANGUAGE_KOREAN_JOHAB != aLang )
                    {
                        long nSpaceSum = nSpaceAdd;
                        for ( USHORT nI = 0; nI < rInf.GetLen(); ++nI )
                        {
                            pKernArray[ nI ] += nSpaceSum;
                            nSpaceSum += nSpaceAdd;
                        }

                        bSpecialJust = sal_True;
                        nSpaceAdd = 0;
                    }
                }

                // Kashida Justification
                if ( SW_CTL == nActual && nSpaceAdd )
                {
                    if ( SwScriptInfo::IsArabicText( rInf.GetText(), rInf.GetIdx(), rInf.GetLen() ) )
                    {
                        if ( pSI && pSI->CountKashida() &&
                            pSI->KashidaJustify( pKernArray, 0, rInf.GetIdx(),
                                                 rInf.GetLen(), nSpaceAdd ) != STRING_LEN )
                        {
                            bSpecialJust = sal_True;
                            nSpaceAdd = 0;
                        }
                    }
                }

                // Thai Justification
                if ( SW_CTL == nActual && nSpaceAdd )
                {
                    LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CTL );

                    if ( LANGUAGE_THAI == aLang )
                    {
                        // Use rInf.GetSpace() because it has more precision than
                        // nSpaceAdd:
                        SwScriptInfo::ThaiJustify( rInf.GetText(), pKernArray, 0,
                                                   rInf.GetIdx(), rInf.GetLen(),
                                                   rInf.GetNumberOfBlanks(),
                                                   rInf.GetSpace() );

                        // adding space to blanks is already done
                        bSpecialJust = sal_True;
                        nSpaceAdd = 0;
                    }
                }
            }

            long nKernSum = rInf.GetKern();

            if ( bStretch || bPaintBlank || rInf.GetKern() || bSpecialJust )
            {
                for( xub_StrLen i = 0; i < rInf.GetLen(); i++,
                     nKernSum += rInf.GetKern() )
                {
                    if ( CH_BLANK == rInf.GetText().GetChar(rInf.GetIdx()+i) )
                        nKernSum += nSpaceAdd;
                    pKernArray[i] += nKernSum;
                }

                // Bei durch/unterstr. Blocksatz erfordert ein Blank am Ende
                // einer Textausgabe besondere Massnahmen:
                if( bPaintBlank && rInf.GetLen() && ( CH_BLANK ==
                    rInf.GetText().GetChar( rInf.GetIdx()+rInf.GetLen()-1 ) ) )
                {
                    // Wenn es sich um ein singulaeres, unterstrichenes Space
                    // handelt, muessen wir zwei ausgeben:
                    if( 1 == rInf.GetLen() )
                    {
                           pKernArray[0] = rInf.GetWidth() + nSpaceAdd;

                        rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                                     pKernArray, rInf.GetIdx(), 1 );
                    }
                    else
                    {
                        pKernArray[ rInf.GetLen() - 2 ] += nSpaceAdd;
                        rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                            pKernArray, rInf.GetIdx(), rInf.GetLen() );
                    }
                }
                else
                    rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                                 pKernArray, rInf.GetIdx(), rInf.GetLen() );
            }
            else
            {
                Point aTmpPos( aPos );
                xub_StrLen j = 0;
                xub_StrLen i;
                for( i = 0; i < rInf.GetLen(); i++ )
                {
                    if( CH_BLANK == rInf.GetText().GetChar( rInf.GetIdx()+i ) )
                    {
                        nKernSum += nSpaceAdd;
                        if( j < i )
                        rInf.GetOut().DrawText( aTmpPos, rInf.GetText(),
                                                rInf.GetIdx() + j, i - j );
                        j = i + 1;
                        SwTwips nAdd = pKernArray[ i ] + nKernSum;
                        if ( ( TEXT_LAYOUT_BIDI_STRONG | TEXT_LAYOUT_BIDI_RTL ) == nMode )
                            nAdd *= -1;
                        aTmpPos.X() = aPos.X() + nAdd;
                    }
                }
                if( j < i )
                    rInf.GetOut().DrawText( aTmpPos, rInf.GetText(),
                                            rInf.GetIdx() + j, i - j );
            }
            delete[] pKernArray;
        }
        else if( bStretch )
        {
            long nTmpWidth = rInf.GetWidth();
            if( rInf.GetKern() && rInf.GetLen() && nTmpWidth > rInf.GetKern() )
                nTmpWidth -= rInf.GetKern();
            rInf.GetOut().DrawStretchText( aPos, nTmpWidth,
                                           rInf.GetText(), rInf.GetIdx(), rInf.GetLen() );
        }
        else if( rInf.GetKern() )
        {
            const long nTmpWidth = GetTextSize( rInf ).Width();

            const Color aSaveColor( pTmpFont->GetColor() );
            const sal_Bool bColorChanged = rInf.ApplyAutoColor( pTmpFont );

            if( bColorChanged )
            {
                if( !pTmpFont->IsSameInstance( rInf.GetOut().GetFont() ) )
                    rInf.GetOut().SetFont( *pTmpFont );
                pTmpFont->SetColor( aSaveColor );
            }

            rInf.GetOut().DrawStretchText( aPos, (USHORT)nTmpWidth,
                                           rInf.GetText(), rInf.GetIdx(), rInf.GetLen() );
        }
        else
            rInf.GetOut().DrawText( aPos, rInf.GetText(),
                                    rInf.GetIdx(), rInf.GetLen() );
    }

    //
    // PAINTING WITH FORMATTING DEVICE/SCREEN ADJUSTMENT
    //

    else
    {
        const String* pStr = &rInf.GetText();
        String aStr( aEmptyStr );
        BOOL bBullet = rInf.GetBullet();
        if( bSymbol )
            bBullet = FALSE;
        sal_Int32 *pKernArray = new sal_Int32[ rInf.GetLen() ];
        CreateScrFont( *rInf.GetShell(), rInf.GetOut() );
        long nScrPos;

        // get screen array
        sal_Int32* pScrArray = new sal_Int32[ rInf.GetLen() ];
        rInf.GetOut().GetTextArray( rInf.GetText(), pScrArray,
                                    rInf.GetIdx(), rInf.GetLen() );

        // OLE: no printer available
        // ASSERT( pPrinter, "DrawText needs pPrinter" )
        if ( pPrinter )
        {
            // pTmpFont has already been set as current font for rInf.GetOut()
            if ( pPrinter != rInf.GetpOut() || pTmpFont != pPrtFont )
            {
                if( !pPrtFont->IsSameInstance( pPrinter->GetFont() ) )
                    pPrinter->SetFont( *pPrtFont );
            }
            pPrinter->GetTextArray( rInf.GetText(), pKernArray, rInf.GetIdx(),
                                    rInf.GetLen() );
        }
        else
        {
//            BOOL bRestore = FALSE;
//            MapMode aOld( rInf.GetOut().GetMapMode() );
//                if( rInf.GetZoom().GetNumerator() &&
//                        rInf.GetZoom() != aOld.GetScaleX() )
//                {
//                        MapMode aNew( aOld );
//                        aNew.SetScaleX( rInf.GetZoom() );
//                        aNew.SetScaleY( rInf.GetZoom() );
//                        rInf.GetOut().SetMapMode( aNew );
//                        bRestore = TRUE;
//                }
            rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                        rInf.GetIdx(), rInf.GetLen() );
//            if( bRestore )
//                rInf.GetOut().SetMapMode( aOld );
        }

        //
        // Modify Printer and ScreenArrays for special justifications
        //
        long nSpaceAdd = rInf.GetSpace() / SPACING_PRECISION_FACTOR;
        bool bNoHalfSpace = false;

        if ( rInf.GetFont() && rInf.GetLen() )
        {
            const BYTE nActual = rInf.GetFont()->GetActual();
            const SwScriptInfo* pSI = rInf.GetScriptInfo();

            // Kana Compression
            if ( SW_CJK == nActual && rInf.GetKanaComp() &&
                 pSI && pSI->CountCompChg() &&
                 lcl_IsMonoSpaceFont( rInf.GetOut() ) )
            {
                Point aTmpPos( aPos );
                pSI->Compress( pScrArray, rInf.GetIdx(), rInf.GetLen(),
                               rInf.GetKanaComp(),
                               (USHORT)aFont.GetSize().Height(), &aTmpPos );
                pSI->Compress( pKernArray, rInf.GetIdx(), rInf.GetLen(),
                               rInf.GetKanaComp(),
                               (USHORT)aFont.GetSize().Height(), &aPos );
            }

            // Asian Justification
            if ( SW_CJK == nActual && nSpaceAdd )
            {
                LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CJK );

                if ( LANGUAGE_KOREAN != aLang && LANGUAGE_KOREAN_JOHAB != aLang )
                {
                    long nSpaceSum = nSpaceAdd;
                    for ( USHORT nI = 0; nI < rInf.GetLen(); ++nI )
                    {
                        pKernArray[ nI ] += nSpaceSum;
                        pScrArray[ nI ] += nSpaceSum;
                        nSpaceSum += nSpaceAdd;
                    }

                    nSpaceAdd = 0;
                }
            }

            // Kashida Justification
            if ( SW_CTL == nActual && nSpaceAdd )
            {
                if ( SwScriptInfo::IsArabicText( rInf.GetText(), rInf.GetIdx(), rInf.GetLen() ) )
                {
                    if ( pSI && pSI->CountKashida() &&
                         pSI->KashidaJustify( pKernArray, pScrArray, rInf.GetIdx(),
                                              rInf.GetLen(), nSpaceAdd ) != STRING_LEN )
                        nSpaceAdd = 0;
                    else
                        bNoHalfSpace = true;
                }
            }

            // Thai Justification
            if ( SW_CTL == nActual && nSpaceAdd )
            {
                LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CTL );

                if ( LANGUAGE_THAI == aLang )
                {
                    SwScriptInfo::ThaiJustify( rInf.GetText(), pKernArray,
                                               pScrArray, rInf.GetIdx(),
                                               rInf.GetLen(),
                                               rInf.GetNumberOfBlanks(),
                                               rInf.GetSpace() );

                    // adding space to blanks is already done
                    nSpaceAdd = 0;
                }
            }
        }

        nScrPos = pScrArray[ 0 ];

        if( bBullet )
        {
            // !!! HACK !!!
            // The Arabic layout engine requires some context of the string
            // which should be painted.
            xub_StrLen nCopyStart = rInf.GetIdx();
            if ( nCopyStart )
                --nCopyStart;

            xub_StrLen nCopyLen = rInf.GetLen();
            if ( nCopyStart + nCopyLen < rInf.GetText().Len() )
                ++nCopyLen;

            aStr = rInf.GetText().Copy( nCopyStart, nCopyLen );
            pStr = &aStr;

            for( xub_StrLen i = 0; i < aStr.Len(); ++i )
                if( CH_BLANK == aStr.GetChar( i ) )
                    aStr.SetChar( i, CH_BULLET );
        }

        xub_StrLen nCnt = rInf.GetText().Len();
        if ( nCnt < rInf.GetIdx() )
            nCnt = 0;
        else
            nCnt = nCnt - rInf.GetIdx();
        nCnt = Min( nCnt, rInf.GetLen() );
        long nKernSum = rInf.GetKern();
        xub_Unicode cChPrev = rInf.GetText().GetChar( rInf.GetIdx() );

        // Wenn es sich um ein singulaeres, unterstrichenes Space
        // im Blocksatz handelt, muessen wir zwei ausgeben:
        if ( ( nCnt == 1 ) && rInf.GetSpace() && ( cChPrev == CH_BLANK ) )
        {
            pKernArray[0] = rInf.GetWidth() +
                            rInf.GetKern() +
                          ( rInf.GetSpace() / SPACING_PRECISION_FACTOR );

            if ( bSwitchL2R )
                rInf.GetFrm()->SwitchLTRtoRTL( aPos );

            if ( bSwitchH2V )
                rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

            rInf.GetOut().DrawTextArray( aPos, rInf.GetText(),
                                         pKernArray, rInf.GetIdx(), 1 );
            if( bBullet )
                rInf.GetOut().DrawTextArray( aPos, *pStr, pKernArray,
                                             rInf.GetIdx() ? 1 : 0, 1 );
        }
        else
        {
            xub_Unicode nCh;

            // Bei Pairkerning waechst der Printereinfluss auf die Positionierung
            USHORT nMul = 3;

            if ( pPrtFont->GetKerning() )
                nMul = 1;

            const USHORT nDiv = nMul+1;

            // In nSpaceSum wird der durch Blocksatz auf die Spaces verteilte
            // Zwischenraum aufsummiert.
            // Die Spaces selbst werden im Normalfall in der Mitte des
            // Zwischenraums positioniert, deshalb die nSpace/2-Mimik.
            // Bei wortweiser Unterstreichung muessen sie am Anfang des
            // Zwischenraums stehen, damit dieser nicht unterstrichen wird.
            // Ein Space am Anfang oder am Ende des Textes muss allerdings
            // vor bzw. hinter den kompletten Zwischenraum gesetzt werden,
            // sonst wuerde das Durch-/Unterstreichen Luecken aufweisen.
            long nSpaceSum = 0;
            // in word line mode and for Arabic, we disable the half space trick:
            const long nHalfSpace = pPrtFont->IsWordLineMode() || bNoHalfSpace ? 0 : nSpaceAdd / 2;
            const long nOtherHalf = nSpaceAdd - nHalfSpace;
            if ( nSpaceAdd && ( cChPrev == CH_BLANK ) )
                nSpaceSum = nHalfSpace;
            for ( xub_StrLen i=1; i<nCnt; ++i,nKernSum += rInf.GetKern() )
            {
                nCh = rInf.GetText().GetChar( rInf.GetIdx() + i );

                ASSERT( pScrArray, "Where is the screen array?" )
                long nScr;
                nScr = pScrArray[ i ] - pScrArray[ i - 1 ];

                // Wenn vor uns ein (Ex-)SPACE ist, positionieren wir uns optimal,
                // d.h. unseren rechten Rand auf die 100% Druckerposition,
                // sind wir sogar selbst ein Ex-SPACE, so positionieren wir uns
                // linksbuendig zur Druckerposition.
                if ( nCh == CH_BLANK )
                {
#ifdef FONT_TEST_DEBUG
                    lcl_Pos( 3, nScrPos, nScr, pKernArray[i-1], pKernArray[i] );
#else
                    nScrPos = pKernArray[i-1] + nScr;
#endif
                    if ( cChPrev == CH_BLANK )
                        nSpaceSum += nOtherHalf;
                    if ( i + 1 == nCnt )
                        nSpaceSum += nSpaceAdd;
                    else
                        nSpaceSum += nHalfSpace;
                }
                else
                {
                    if ( cChPrev == CH_BLANK )
                    {
#ifdef FONT_TEST_DEBUG
                        lcl_Pos( 6, nScrPos, nScr, pKernArray[i-1], pKernArray[i] );
#else
                        nScrPos = pKernArray[i-1] + nScr;
#endif
                        // kein Pixel geht verloren:
                        nSpaceSum += nOtherHalf;
                    }
                    else if ( cChPrev == '-' )
#ifdef FONT_TEST_DEBUG
                        lcl_Pos( 6, nScrPos, nScr, pKernArray[i-1], pKernArray[i] );
#else
                        nScrPos = pKernArray[i-1] + nScr;
#endif
                    else
                    {
#ifdef FONT_TEST_DEBUG
                        lcl_Pos( 0, nScrPos, nScr, pKernArray[i-1], pKernArray[i] );
#else
                        nScrPos += nScr;
                        nScrPos = ( nMul * nScrPos + pKernArray[i] ) / nDiv;
#endif
                    }
                }
                cChPrev = nCh;
                pKernArray[i-1] = nScrPos - nScr + nKernSum + nSpaceSum;
                // In word line mode and for Arabic, we disabled the half space trick. If a portion
                // ends with a blank, the full nSpaceAdd value has been added to the character in
                // front of the blank. This leads to painting artifacts, therefore we remove the
                // nSpaceAdd value again:
                if ( (bNoHalfSpace || pPrtFont->IsWordLineMode()) && i+1 == nCnt && nCh == CH_BLANK )
                    pKernArray[i-1] = pKernArray[i-1] - nSpaceAdd;
            }

            // the layout engine requires the total width of the output
            pKernArray[ rInf.GetLen() - 1 ] += nKernSum + nSpaceSum;

            if( rInf.GetGreyWave() )
            {
                if( rInf.GetLen() )
                {
                    long nHght = rInf.GetOut().LogicToPixel(
                                    pPrtFont->GetSize() ).Height();
                    if( WRONG_SHOW_MIN < nHght )
                    {
                        if ( rInf.GetOut().GetConnectMetaFile() )
                            rInf.GetOut().Push();

                        USHORT nWave =
                            WRONG_SHOW_MEDIUM < nHght ? WAVE_NORMAL :
                            ( WRONG_SHOW_SMALL < nHght ? WAVE_SMALL :
                            WAVE_FLAT );
                        Color aCol( rInf.GetOut().GetLineColor() );
                        BOOL bColSave = aCol != *pWaveCol;
                        if ( bColSave )
                            rInf.GetOut().SetLineColor( *pWaveCol );

                        Point aEnd;
                        long nKernVal = pKernArray[ USHORT( rInf.GetLen() - 1 ) ];

                        USHORT nDir = bBidiPor ?
                                        1800 :
                                        UnMapDirection(
                                            GetFont()->GetOrientation(),
                                            bSwitchH2V );

                        switch ( nDir )
                        {
                        case 0 :
                            aEnd.X() = rInf.GetPos().X() + nKernVal;
                            aEnd.Y() = rInf.GetPos().Y();
                            break;
                        case 900 :
                            aEnd.X() = rInf.GetPos().X();
                            aEnd.Y() = rInf.GetPos().Y() - nKernVal;
                            break;
                        case 1800 :
                            aEnd.X() = rInf.GetPos().X() - nKernVal;
                            aEnd.Y() = rInf.GetPos().Y();
                            break;
                        case 2700 :
                            aEnd.X() = rInf.GetPos().X();
                            aEnd.Y() = rInf.GetPos().Y() + nKernVal;
                            break;
                        }

                        Point aCurrPos( rInf.GetPos() );

                        if ( bSwitchL2R )
                        {
                            rInf.GetFrm()->SwitchLTRtoRTL( aCurrPos );
                            rInf.GetFrm()->SwitchLTRtoRTL( aEnd );
                        }

                        if ( bSwitchH2V )
                        {
                            rInf.GetFrm()->SwitchHorizontalToVertical( aCurrPos );
                            rInf.GetFrm()->SwitchHorizontalToVertical( aEnd );
                        }
                        rInf.GetOut().DrawWaveLine( aCurrPos, aEnd, nWave );

                        if ( bColSave )
                            rInf.GetOut().SetLineColor( aCol );

                        if ( rInf.GetOut().GetConnectMetaFile() )
                            rInf.GetOut().Pop();
                    }
                }
            }
            else if( !bSymbol && rInf.GetLen() )
            {
                // anything to do?
                if (rInf.GetWrong() || rInf.GetGrammarCheck() || rInf.GetSmartTags())
                {
                    CalcLinePosData aCalcLinePosData(rInf, *GetFont(),
                            nCnt, bSwitchH2V, bSwitchL2R,
                            nHalfSpace, pKernArray, bBidiPor);

                    SwForbidden aForbidden;
                    // draw line for smart tag data
                    lcl_DrawLineForWrongListData( aForbidden, rInf, rInf.GetSmartTags(), aCalcLinePosData, Size() );
                    // draw wave line for spell check errors
                    // draw them BEFORE the grammar check lines to 'override' the latter in case of conflict.
                    // reason: some grammar errors can only be found if spelling errors are fixed,
                    // therefore we don't want the user to miss a spelling error.
                    lcl_DrawLineForWrongListData( aForbidden, rInf, rInf.GetWrong(), aCalcLinePosData, pPrtFont->GetSize() );
                    // draw wave line for grammar check errors
                    lcl_DrawLineForWrongListData( aForbidden, rInf, rInf.GetGrammarCheck(), aCalcLinePosData, pPrtFont->GetSize() );
                }
            }

            xub_StrLen nOffs = 0;
            xub_StrLen nLen = rInf.GetLen();
#ifdef COMING_SOON
            if( aPos.X() < rInf.GetLeft() )
            {
                while( nOffs < nLen &&
                    aPos.X() + pKernArray[ nOffs ] < rInf.GetLeft() )
                    ++nOffs;
                if( nOffs < nLen )
                {
                    --nLen;
                    while( nLen > nOffs &&
                        aPos.X() + pKernArray[ nLen ] > rInf.GetRight() )
                        --nLen;
                    ++nLen;
                    if( nOffs )
                        --nOffs;
                }
                if( nOffs )
                {
                    long nDiff = pKernArray[ nOffs - 1 ];
                    aPos.X() += nDiff;
                    for( xub_StrLen nX = nOffs; nX < nLen; ++nX )
                        pKernArray[ nX ] -= nDiff;
                }
            }
#endif
            if( nOffs < nLen )
            {
                // If we paint bullets instead of spaces, we use a copy of
                // the paragraph string. For the layout engine, the copy
                // of the string has to be an environment of the range which
                // is painted
                xub_StrLen nTmpIdx = bBullet ?
                                              ( rInf.GetIdx() ? 1 : 0 ) :
                                              rInf.GetIdx();

                if ( bSwitchL2R )
                    rInf.GetFrm()->SwitchLTRtoRTL( aPos );

                if ( bSwitchH2V )
                    rInf.GetFrm()->SwitchHorizontalToVertical( aPos );

                rInf.GetOut().DrawTextArray( aPos, *pStr, pKernArray + nOffs,
                                             nTmpIdx + nOffs , nLen - nOffs );
            }
        }
        delete[] pScrArray;
        delete[] pKernArray;
    }
}


// Optimierung war fuer DrawText() ausgeschaltet
#ifdef _MSC_VER
#pragma optimize("",on)
#endif


/*************************************************************************
 *
 *	Size SwFntObj::GetTextSize( const OutputDevice *pOut, const String &rTxt,
 *			 const USHORT nIdx, const USHORT nLen, const short nKern = 0 );
 *
 *	Ersterstellung		AMA 16. Dez. 94
 *	Letzte Aenderung	AMA 16. Dez. 94
 *
 *  Beschreibung: ermittelt die TextSize (des Druckers)
 *
 *************************************************************************/

Size SwFntObj::GetTextSize( SwDrawTextInfo& rInf )
{
    Size aTxtSize;
    const xub_StrLen nLn = ( STRING_LEN != rInf.GetLen() ) ? rInf.GetLen() :
                           rInf.GetText().Len();

    // be sure to have the correct layout mode at the printer
    if ( pPrinter )
    {
        pPrinter->SetLayoutMode( rInf.GetOut().GetLayoutMode() );
        pPrinter->SetDigitLanguage( rInf.GetOut().GetDigitLanguage() );
    }

    if ( rInf.GetFrm() && nLn && rInf.SnapToGrid() && rInf.GetFont() &&
         SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && pGrid->IsSnapToChars() )
        {
            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            const USHORT nGridWidth = GETGRIDWIDTH(pGrid, pDoc);

            OutputDevice* pOutDev;

            if ( pPrinter )
            {
                if( !pPrtFont->IsSameInstance( pPrinter->GetFont() ) )
                    pPrinter->SetFont(*pPrtFont);
                pOutDev = pPrinter;
            }
            else
                pOutDev = rInf.GetpOut();

            aTxtSize.Width() =
                    pOutDev->GetTextWidth( rInf.GetText(), rInf.GetIdx(), nLn );

            ASSERT( !rInf.GetShell() ||
                    ( USHRT_MAX != GetGuessedLeading() && USHRT_MAX != GetExtLeading() ),
                "Leading values should be already calculated" )
            aTxtSize.Height() = pOutDev->GetTextHeight() +
                                GetFontLeading( rInf.GetShell(), rInf.GetOut() );

            long nWidthPerChar = aTxtSize.Width() / nLn;

            const ULONG i = nWidthPerChar ?
                            ( nWidthPerChar - 1 ) / nGridWidth + 1:
                            1;

            aTxtSize.Width() = i * nGridWidth * nLn;
            rInf.SetKanaDiff( 0 );
            return aTxtSize;
        }
    }

    //for textgrid refactor
    if ( rInf.GetFrm() && nLn && rInf.SnapToGrid() && rInf.GetFont() &&
         SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && !pGrid->IsSnapToChars() )
        {
            const USHORT nDefaultFontHeight = GetDefaultFontHeight( rInf );

            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            long nGridWidthAdd = GETGRIDWIDTH(pGrid, pDoc);
            if( SW_LATIN == rInf.GetFont()->GetActual() )
                nGridWidthAdd = ( nGridWidthAdd - nDefaultFontHeight ) / 2;
            else
                nGridWidthAdd = nGridWidthAdd - nDefaultFontHeight;
            OutputDevice* pOutDev;
            if ( pPrinter )
            {
                if( !pPrtFont->IsSameInstance( pPrinter->GetFont() ) )
                    pPrinter->SetFont(*pPrtFont);
                pOutDev = pPrinter;
            }
            else
                pOutDev = rInf.GetpOut();
            aTxtSize.Width() = pOutDev->GetTextWidth( rInf.GetText(), rInf.GetIdx(), nLn );
            aTxtSize.Height() = pOutDev->GetTextHeight() +
                                GetFontLeading( rInf.GetShell(), rInf.GetOut() );
            aTxtSize.Width() += (nLn) * long( nGridWidthAdd );
            //if ( rInf.GetKern() && nLn )
            //    aTxtSize.Width() += ( nLn ) * long( rInf.GetKern() );

            rInf.SetKanaDiff( 0 );
            return aTxtSize;
        }
    }

    const BOOL bCompress = rInf.GetKanaComp() && nLn &&
                           rInf.GetFont() &&
                           SW_CJK == rInf.GetFont()->GetActual() &&
                           rInf.GetScriptInfo() &&
                           rInf.GetScriptInfo()->CountCompChg() &&
                           lcl_IsMonoSpaceFont( rInf.GetOut() );

    ASSERT(	!bCompress || ( rInf.GetScriptInfo() && rInf.GetScriptInfo()->
            CountCompChg()), "Compression without info" );

    // This is the part used e.g., for cursor travelling
    // See condition for DrawText or DrawTextArray (bDirectPrint)
    if ( pPrinter && pPrinter != rInf.GetpOut() )
    {
        if( !pPrtFont->IsSameInstance( pPrinter->GetFont() ) )
            pPrinter->SetFont(*pPrtFont);
        aTxtSize.Width() = pPrinter->GetTextWidth( rInf.GetText(),
                                                   rInf.GetIdx(), nLn );
        aTxtSize.Height() = pPrinter->GetTextHeight();
        sal_Int32 *pKernArray = new sal_Int32[nLn];
        CreateScrFont( *rInf.GetShell(), rInf.GetOut() );
        if( !GetScrFont()->IsSameInstance( rInf.GetOut().GetFont() ) )
            rInf.GetOut().SetFont( *pScrFont );
        long nScrPos;

        pPrinter->GetTextArray( rInf.GetText(), pKernArray, rInf.GetIdx(),nLn );
        if( bCompress )
            rInf.SetKanaDiff( rInf.GetScriptInfo()->Compress( pKernArray,
                rInf.GetIdx(), nLn, rInf.GetKanaComp(),
                (USHORT)aFont.GetSize().Height() ) );
        else
            rInf.SetKanaDiff( 0 );

        if ( rInf.GetKanaDiff() )
            nScrPos = pKernArray[ nLn - 1 ];
        else
        {
            sal_Int32* pScrArray = new sal_Int32[ rInf.GetLen() ];
            rInf.GetOut().GetTextArray( rInf.GetText(), pScrArray,
                                        rInf.GetIdx(), rInf.GetLen() );
            nScrPos = pScrArray[ 0 ];
            xub_StrLen nCnt = rInf.GetText().Len();
            if ( nCnt < rInf.GetIdx() )
                nCnt=0;
            else
                nCnt = nCnt - rInf.GetIdx();
            nCnt = Min (nCnt, nLn);
            xub_Unicode nChPrev = rInf.GetText().GetChar( rInf.GetIdx() );

            xub_Unicode nCh;

            // Bei Pairkerning waechst der Printereinfluss auf die Positionierung
            USHORT nMul = 3;
            if ( pPrtFont->GetKerning() )
                nMul = 1;
            const USHORT nDiv = nMul+1;
            for( xub_StrLen i=1; i<nCnt; i++ )
            {
                nCh = rInf.GetText().GetChar( rInf.GetIdx() + i );
                long nScr;
                nScr = pScrArray[ i ] - pScrArray[ i - 1 ];
                if ( nCh == CH_BLANK )
                    nScrPos = pKernArray[i-1]+nScr;
                else
                {
                    if ( nChPrev == CH_BLANK || nChPrev == '-' )
                        nScrPos = pKernArray[i-1]+nScr;
                    else
                    {
                        nScrPos += nScr;
                        nScrPos = ( nMul * nScrPos + pKernArray[i] ) / nDiv;
                    }
                }
                nChPrev = nCh;
                pKernArray[i-1] = nScrPos - nScr;
            }
            delete[] pScrArray;
        }

        delete[] pKernArray;
        aTxtSize.Width() = nScrPos;
    }
    else
    {
        if( !pPrtFont->IsSameInstance( rInf.GetOut().GetFont() ) )
            rInf.GetOut().SetFont( *pPrtFont );
        if( bCompress )
        {
            sal_Int32 *pKernArray = new sal_Int32[nLn];
            rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                        rInf.GetIdx(), nLn );
            rInf.SetKanaDiff( rInf.GetScriptInfo()->Compress( pKernArray,
                rInf.GetIdx(), nLn, rInf.GetKanaComp(),
                (USHORT) aFont.GetSize().Height() ) );
            aTxtSize.Width() = pKernArray[ nLn - 1 ];
            delete[] pKernArray;
        }
        else
        {
            aTxtSize.Width() = rInf.GetOut().GetTextWidth( rInf.GetText(),
                                                           rInf.GetIdx(), nLn );
            rInf.SetKanaDiff( 0 );
        }

        aTxtSize.Height() = rInf.GetOut().GetTextHeight();
    }

    if ( rInf.GetKern() && nLn )
        aTxtSize.Width() += ( nLn - 1 ) * long( rInf.GetKern() );

    ASSERT( !rInf.GetShell() ||
            ( USHRT_MAX != GetGuessedLeading() && USHRT_MAX != GetExtLeading() ),
              "Leading values should be already calculated" )
    aTxtSize.Height() += GetFontLeading( rInf.GetShell(), rInf.GetOut() );
    return aTxtSize;
}


xub_StrLen SwFntObj::GetCrsrOfst( SwDrawTextInfo &rInf )
{
    long nSpaceAdd =       rInf.GetSpace() / SPACING_PRECISION_FACTOR;
    const long nSperren = -rInf.GetSperren() / SPACING_PRECISION_FACTOR;
    long nKern = rInf.GetKern();

    if( 0 != nSperren )
        nKern -= nSperren;

    sal_Int32 *pKernArray = new sal_Int32[ rInf.GetLen() ];

    // be sure to have the correct layout mode at the printer
    if ( pPrinter )
    {
        pPrinter->SetLayoutMode( rInf.GetOut().GetLayoutMode() );
        pPrinter->SetDigitLanguage( rInf.GetOut().GetDigitLanguage() );
        pPrinter->GetTextArray( rInf.GetText(), pKernArray,
                                rInf.GetIdx(), rInf.GetLen() );
    }
    else
        rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                    rInf.GetIdx(), rInf.GetLen() );

    const SwScriptInfo* pSI = rInf.GetScriptInfo();
    if ( rInf.GetFont() && rInf.GetLen() )
    {
        const BYTE nActual = rInf.GetFont()->GetActual();

        // Kana Compression
        if ( SW_CJK == nActual && rInf.GetKanaComp() &&
             pSI && pSI->CountCompChg() &&
             lcl_IsMonoSpaceFont( rInf.GetOut() ) )
        {
            pSI->Compress( pKernArray, rInf.GetIdx(), rInf.GetLen(),
                           rInf.GetKanaComp(),
                           (USHORT) aFont.GetSize().Height() );
        }

        // Asian Justification
        if ( SW_CJK == rInf.GetFont()->GetActual() )
        {
            LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CJK );

            if ( LANGUAGE_KOREAN != aLang && LANGUAGE_KOREAN_JOHAB != aLang )
            {
                long nSpaceSum = nSpaceAdd;
                for ( USHORT nI = 0; nI < rInf.GetLen(); ++nI )
                {
                    pKernArray[ nI ] += nSpaceSum;
                    nSpaceSum += nSpaceAdd;
                }

                nSpaceAdd = 0;
            }

        }

        // Kashida Justification
        if ( SW_CTL == nActual && rInf.GetSpace() )
        {
            if ( SwScriptInfo::IsArabicText( rInf.GetText(), rInf.GetIdx(), rInf.GetLen() ) )
            {
                if ( pSI && pSI->CountKashida() &&
                    pSI->KashidaJustify( pKernArray, 0, rInf.GetIdx(), rInf.GetLen(),
                                         nSpaceAdd ) != STRING_LEN )
                    nSpaceAdd = 0;
            }
        }

        // Thai Justification
        if ( SW_CTL == nActual && nSpaceAdd )
        {
            LanguageType aLang = rInf.GetFont()->GetLanguage( SW_CTL );

            if ( LANGUAGE_THAI == aLang )
            {
                SwScriptInfo::ThaiJustify( rInf.GetText(), pKernArray, 0,
                                           rInf.GetIdx(), rInf.GetLen(),
                                           rInf.GetNumberOfBlanks(),
                                           rInf.GetSpace() );

                // adding space to blanks is already done
                nSpaceAdd = 0;
            }
        }
    }

    long nLeft = 0;
    long nRight = 0;
    xub_StrLen nCnt = 0;
    long nSpaceSum = 0;
    long nKernSum = 0;

    if ( rInf.GetFrm() && rInf.GetLen() && rInf.SnapToGrid() &&
         rInf.GetFont() && SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && pGrid->IsSnapToChars() )
        {
            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            const USHORT nGridWidth = GETGRIDWIDTH(pGrid, pDoc);

            long nWidthPerChar = pKernArray[ rInf.GetLen() - 1 ] / rInf.GetLen();

            ULONG i = nWidthPerChar ?
                      ( nWidthPerChar - 1 ) / nGridWidth + 1:
                      1;

            nWidthPerChar = i * nGridWidth;

            nCnt = (USHORT)(rInf.GetOfst() / nWidthPerChar);
            if ( 2 * ( rInf.GetOfst() - nCnt * nWidthPerChar ) > nWidthPerChar )
                ++nCnt;

            delete[] pKernArray;
            return nCnt;
        }
    }

    //for textgrid refactor
    if ( rInf.GetFrm() && rInf.GetLen() && rInf.SnapToGrid() &&
         rInf.GetFont() && SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && !pGrid->IsSnapToChars() )
        {

            const USHORT nDefaultFontHeight = GetDefaultFontHeight( rInf );

            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            long nGridWidthAdd = GETGRIDWIDTH(pGrid, pDoc);
            if( SW_LATIN == rInf.GetFont()->GetActual() )
                nGridWidthAdd = ( nGridWidthAdd - nDefaultFontHeight ) / 2;
            else
                nGridWidthAdd = nGridWidthAdd - nDefaultFontHeight;

            for(xub_StrLen j = 0; j < rInf.GetLen(); j++)
            {
                long nScr = pKernArray[ j ] + ( nSpaceAdd + nGridWidthAdd  ) * ( j + 1 );
                if( nScr >= rInf.GetOfst())
                {
                    nCnt = j;
                    break;
                }
            }
            delete[] pKernArray;
            return nCnt;
        }
    }

    sal_uInt16 nItrMode = i18n::CharacterIteratorMode::SKIPCELL;
    sal_Int32 nDone = 0;
    LanguageType aLang = LANGUAGE_NONE;
    sal_Bool bSkipCell = sal_False;
    xub_StrLen nIdx = rInf.GetIdx();
    xub_StrLen nLastIdx = nIdx;
    const xub_StrLen nEnd = rInf.GetIdx() + rInf.GetLen();

    // skip character cells for complex scripts
    // --> OD 2009-10-14 #i105571# - skip also character cells for CJK
    if ( rInf.GetFont() && 
         ( SW_CTL == rInf.GetFont()->GetActual() ||
           SW_CJK == rInf.GetFont()->GetActual() ) &&
         pBreakIt->GetBreakIter().is() )
    // <--
    {
        aLang = rInf.GetFont()->GetLanguage();
        bSkipCell = sal_True;
    }

    while ( ( nRight < long( rInf.GetOfst() ) ) && ( nIdx < nEnd ) )
    {
        if ( nSpaceAdd && CH_BLANK == rInf.GetText().GetChar( nIdx ) )
            nSpaceSum += nSpaceAdd;

        // go to next character (cell).
        nLastIdx = nIdx;

        if ( bSkipCell )
        {
            nIdx = (xub_StrLen)pBreakIt->GetBreakIter()->nextCharacters( rInf.GetText(),
                        nIdx, pBreakIt->GetLocale( aLang ), nItrMode, 1, nDone );
            if ( nIdx <= nLastIdx )
                break;
        }
        else
            ++nIdx;

        nLeft = nRight;
        nRight = pKernArray[ nIdx - rInf.GetIdx() - 1 ] + nKernSum + nSpaceSum;

        nKernSum += nKern;
    }

    // step back if position is before the middle of the character
    // or if we do not want to go to the next character
    if ( nIdx > rInf.GetIdx() &&
         ( rInf.IsPosMatchesBounds() ||
           ( ( nRight > long( rInf.GetOfst() ) ) &&
             ( nRight - rInf.GetOfst() > rInf.GetOfst() - nLeft ) ) ) )
        nCnt = nLastIdx - rInf.GetIdx(); // first half
    else
        nCnt = nIdx - rInf.GetIdx(); // second half

    if ( pSI )
        rInf.SetCursorBidiLevel( pSI->DirType( nLastIdx ) );

    delete[] pKernArray;
    return nCnt;
}


/*************************************************************************
|*
|*	SwFntAccess::SwFntAccess()
|*
|*	Ersterstellung		AMA 9. Nov. 94
|*	Letzte Aenderung	AMA 9. Nov. 94
|*
|*************************************************************************/

SwFntAccess::SwFntAccess( const void* &rMagic,
                USHORT &rIndex, const void *pOwn, ViewShell *pSh,
                BOOL bCheck ) :
  SwCacheAccess( *pFntCache, rMagic, rIndex ),
  pShell( pSh )
{
    // Der benutzte CTor von SwCacheAccess sucht anhand rMagic+rIndex im Cache
    if ( IsAvail() )
    {
        // Der schnellste Fall: ein bekannter Font ( rMagic ),
        // bei dem Drucker und Zoom nicht ueberprueft werden brauchen.
        if ( !bCheck )
            return;

        // Hier ist zwar der Font bekannt, muss aber noch ueberprueft werden.

    }
    else
        // Hier ist der Font nicht bekannt, muss also gesucht werden.
        bCheck = FALSE;


    {
        OutputDevice* pOut = 0;
        USHORT nZoom = USHRT_MAX;

        // Get the reference device
        if ( pSh )
        {
            pOut = &pSh->GetRefDev();
            nZoom = pSh->GetViewOptions()->GetZoom();
        }

        SwFntObj *pFntObj;
        if ( bCheck )
        {
            pFntObj = Get();
            if ( ( pFntObj->GetZoom( ) == nZoom ) &&
                 ( pFntObj->pPrinter == pOut ) &&
                   pFntObj->GetPropWidth() ==
                        ((SwSubFont*)pOwn)->GetPropWidth() )
                return; // Die Ueberpruefung ergab: Drucker+Zoom okay.
            pFntObj->Unlock( ); // Vergiss dies Objekt, es wurde leider
            pObj = NULL;	 	// eine Drucker/Zoomaenderung festgestellt.
        }

        // Search by font comparison, quite expensive!
        // Look for same font and same printer
        pFntObj = pFntCache->First();
        while ( pFntObj && !( pFntObj->aFont == *(Font *)pOwn &&
                              pFntObj->GetZoom() == nZoom &&
                              pFntObj->GetPropWidth() ==
                              ((SwSubFont*)pOwn)->GetPropWidth() &&
                              ( !pFntObj->pPrinter || pFntObj->pPrinter == pOut ) ) )
            pFntObj = pFntCache->Next( pFntObj );

        if( pFntObj && pFntObj->pPrinter != pOut )
        {
            // Wir haben zwar einen ohne Drucker gefunden, mal sehen, ob es
            // auch noch einen mit identischem Drucker gibt.
            SwFntObj *pTmpObj = pFntObj;
            while( pTmpObj && !( pTmpObj->aFont == *(Font *)pOwn &&
                   pTmpObj->GetZoom()==nZoom && pTmpObj->pPrinter==pOut &&
                   pTmpObj->GetPropWidth() ==
                        ((SwSubFont*)pOwn)->GetPropWidth() ) )
                pTmpObj = pFntCache->Next( pTmpObj );
            if( pTmpObj )
                pFntObj = pTmpObj;
        }

        if ( !pFntObj ) // Font has not been found, create one
        {
            // Das Objekt muss neu angelegt werden, deshalb muss der Owner ein
            // SwFont sein, spaeter wird als Owner die "MagicNumber" gehalten.
            SwCacheAccess::pOwner = pOwn;
            pFntObj = Get(); // hier wird via NewObj() angelegt und gelockt.
            ASSERT(pFntObj, "No Font, no Fun.");
        }
        else  // Font has been found, so we lock it.
        {
            pFntObj->Lock();
            if( pFntObj->pPrinter != pOut ) // Falls bis dato kein Drucker bekannt
            {
                ASSERT( !pFntObj->pPrinter, "SwFntAccess: Printer Changed" );
                pFntObj->CreatePrtFont( *pOut );
                pFntObj->pPrinter = pOut;
                pFntObj->pScrFont = NULL;
                pFntObj->nGuessedLeading = USHRT_MAX;
                pFntObj->nExtLeading = USHRT_MAX;
                pFntObj->nPrtAscent = USHRT_MAX;
                pFntObj->nPrtHeight = USHRT_MAX;
            }
            pObj = pFntObj;
        }

        // egal, ob neu oder gefunden, ab jetzt ist der Owner vom Objekt eine
        // MagicNumber und wird auch dem aufrufenden SwFont bekanntgegeben,
        // ebenso der Index fuer spaetere direkte Zugriffe
        rMagic = pFntObj->GetOwner();
        SwCacheAccess::pOwner = rMagic;
        rIndex = pFntObj->GetCachePos();
    }
}

SwCacheObj *SwFntAccess::NewObj( )
{
    // Ein neuer Font, eine neue "MagicNumber".
    return new SwFntObj( *(SwSubFont *)pOwner, ++pMagicNo, pShell );
}

extern xub_StrLen lcl_CalcCaseMap( const SwFont& rFnt,
                                   const XubString& rOrigString,
                                   xub_StrLen nOfst,
                                   xub_StrLen nLen,
                                   xub_StrLen nIdx );

xub_StrLen SwFont::GetTxtBreak( SwDrawTextInfo& rInf, long nTextWidth )
{
    ChgFnt( rInf.GetShell(), rInf.GetOut() );

    const BOOL bCompress = rInf.GetKanaComp() && rInf.GetLen() &&
                           SW_CJK == GetActual() &&
                           rInf.GetScriptInfo() &&
                           rInf.GetScriptInfo()->CountCompChg() &&
                           lcl_IsMonoSpaceFont( rInf.GetOut() );

    ASSERT( !bCompress || ( rInf.GetScriptInfo() && rInf.GetScriptInfo()->
            CountCompChg()), "Compression without info" );

    USHORT nTxtBreak = 0;
    long nKern = 0;

    USHORT nLn = ( rInf.GetLen() == STRING_LEN ? rInf.GetText().Len()
                                               : rInf.GetLen() );

    if ( rInf.GetFrm() && nLn && rInf.SnapToGrid() &&
         rInf.GetFont() && SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && pGrid->IsSnapToChars() )
        {
            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            const USHORT nGridWidth = GETGRIDWIDTH(pGrid, pDoc);

            sal_Int32* pKernArray = new sal_Int32[rInf.GetLen()];
            rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                        rInf.GetIdx(), rInf.GetLen() );

            long nWidthPerChar = pKernArray[ rInf.GetLen() - 1 ] / rInf.GetLen();

            const ULONG i = nWidthPerChar ?
                            ( nWidthPerChar - 1 ) / nGridWidth + 1:
                            1;

            nWidthPerChar = i * nGridWidth;
            long nCurrPos = nWidthPerChar;

            while( nTxtBreak < rInf.GetLen() && nTextWidth >= nCurrPos )
            {
                nCurrPos += nWidthPerChar;
                ++nTxtBreak;
            }

            delete[] pKernArray;
            return nTxtBreak + rInf.GetIdx();
        }
    }

    //for text grid enhancement
    if ( rInf.GetFrm() && nLn && rInf.SnapToGrid() && rInf.GetFont() &&
         SW_CJK == rInf.GetFont()->GetActual() )
    {
        GETGRID( rInf.GetFrm()->FindPageFrm() )
        if ( pGrid && GRID_LINES_CHARS == pGrid->GetGridType() && !pGrid->IsSnapToChars() )
        {
            const USHORT nDefaultFontHeight = GetDefaultFontHeight( rInf );

            const SwDoc* pDoc = rInf.GetShell()->GetDoc();
            long nGridWidthAdd = GETGRIDWIDTH(pGrid, pDoc);
            if( SW_LATIN == rInf.GetFont()->GetActual() )
                nGridWidthAdd = ( nGridWidthAdd - nDefaultFontHeight ) / 2 ;
            else
                nGridWidthAdd = nGridWidthAdd - nDefaultFontHeight;

            sal_Int32* pKernArray = new sal_Int32[rInf.GetLen()];
            rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                            rInf.GetIdx(), rInf.GetLen() );
            long nCurrPos = pKernArray[nTxtBreak] + nGridWidthAdd;
            while( nTxtBreak < rInf.GetLen() && nTextWidth >= nCurrPos)
            {
                nTxtBreak++;
                nCurrPos = pKernArray[nTxtBreak] + nGridWidthAdd * ( nTxtBreak + 1 );
            }
            delete[] pKernArray;
            return nTxtBreak + rInf.GetIdx();
        }
    }

    if( aSub[nActual].IsCapital() && nLn )
        nTxtBreak = GetCapitalBreak( rInf.GetShell(), rInf.GetpOut(),
        rInf.GetScriptInfo(), rInf.GetText(), nTextWidth,0, rInf.GetIdx(),nLn );
    else
    {
        nKern = CheckKerning();

        const XubString* pTmpText;
        XubString aTmpText;
        xub_StrLen nTmpIdx;
        xub_StrLen nTmpLen;
        bool bTextReplaced = false;

        if ( !aSub[nActual].IsCaseMap() )
        {
            pTmpText = &rInf.GetText();
            nTmpIdx = rInf.GetIdx();
            nTmpLen = nLn;
        }
        else
        {
            const XubString aSnippet( rInf.GetText(), rInf.GetIdx(), nLn );
            aTmpText = aSub[nActual].CalcCaseMap( aSnippet );
            const bool bTitle = SVX_CASEMAP_TITEL == aSub[nActual].GetCaseMap() &&
                                pBreakIt->GetBreakIter().is();

            // Uaaaaahhhh!!! In title case mode, we would get wrong results
            if ( bTitle && nLn )
            {
                // check if rInf.GetIdx() is begin of word
                if ( !pBreakIt->GetBreakIter()->isBeginWord(
                     rInf.GetText(), rInf.GetIdx(),
                     pBreakIt->GetLocale( aSub[nActual].GetLanguage() ),
                     i18n::WordType::ANYWORD_IGNOREWHITESPACES ) )
                {
                    // In this case, the beginning of aTmpText is wrong.
                    XubString aSnippetTmp( aSnippet, 0, 1 );
                    aSnippetTmp = aSub[nActual].CalcCaseMap( aSnippetTmp );
                    aTmpText.Erase( 0, aSnippetTmp.Len() );
                    aTmpText.Insert( aSnippet.GetChar( 0 ), 0 );
                }
            }

            pTmpText = &aTmpText;
            nTmpIdx = 0;
            nTmpLen = aTmpText.Len();
            bTextReplaced = true;
        }

           if( rInf.GetHyphPos() )
            nTxtBreak = rInf.GetOut().GetTextBreak( *pTmpText, nTextWidth,
                                                    '-', *rInf.GetHyphPos(),
                                                     nTmpIdx, nTmpLen, nKern );
        else
            nTxtBreak = rInf.GetOut().GetTextBreak( *pTmpText, nTextWidth,
                                                    nTmpIdx, nTmpLen, nKern );

        if ( bTextReplaced && STRING_LEN != nTxtBreak )
        {
            if ( nTmpLen != nLn )
                nTxtBreak = lcl_CalcCaseMap( *this, rInf.GetText(),
                                             rInf.GetIdx(), nLn, nTxtBreak );
            else
                nTxtBreak = nTxtBreak + rInf.GetIdx();
        }
    }

    if ( ! bCompress )
        return nTxtBreak;

    nTxtBreak = nTxtBreak - rInf.GetIdx();

    if( nTxtBreak < nLn )
    {
        if( !nTxtBreak && nLn )
            nLn = 1;
        else if( nLn > 2 * nTxtBreak )
            nLn = 2 * nTxtBreak;
        sal_Int32 *pKernArray = new sal_Int32[ nLn ];
        rInf.GetOut().GetTextArray( rInf.GetText(), pKernArray,
                                    rInf.GetIdx(), nLn );
        if( rInf.GetScriptInfo()->Compress( pKernArray, rInf.GetIdx(), nLn,
                            rInf.GetKanaComp(), (USHORT)GetHeight( nActual ) ) )
        {
            long nKernAdd = nKern;
            xub_StrLen nTmpBreak = nTxtBreak;
            if( nKern && nTxtBreak )
                nKern *= nTxtBreak - 1;
            while( nTxtBreak<nLn && nTextWidth >= pKernArray[nTxtBreak] +nKern )
            {
                nKern += nKernAdd;
                ++nTxtBreak;
            }
            if( rInf.GetHyphPos() )
                *rInf.GetHyphPos() += nTxtBreak - nTmpBreak; // It's not perfect
        }
        delete[] pKernArray;
    }
    nTxtBreak = nTxtBreak + rInf.GetIdx();

    return nTxtBreak;
}

extern Color aGlobalRetoucheColor;

sal_Bool SwDrawTextInfo::ApplyAutoColor( Font* pFont )
{
    const Font& rFnt = pFont ? *pFont : GetOut().GetFont();
    sal_Bool bPrt = GetShell() && ! GetShell()->GetWin();
    ColorData nNewColor = COL_BLACK;
    sal_Bool bChgFntColor = sal_False;
    sal_Bool bChgLineColor = sal_False;

    if( bPrt && GetShell() && GetShell()->GetViewOptions()->IsBlackFont() )
    {
        if ( COL_BLACK != rFnt.GetColor().GetColor() )
            bChgFntColor = sal_True;

        if ( (COL_BLACK != GetOut().GetLineColor().GetColor()) ||
             (COL_BLACK != GetOut().GetOverlineColor().GetColor()) )
            bChgLineColor = sal_True;
    }
    else
    {
        // FontColor has to be changed if:
        // 1. FontColor = AUTO or 2. IsAlwaysAutoColor is set
        // LineColor has to be changed if:
        // 1. IsAlwaysAutoColor is set

        bChgLineColor = ! bPrt && GetShell() &&
                GetShell()->GetAccessibilityOptions()->IsAlwaysAutoColor();

        bChgFntColor = COL_AUTO == rFnt.GetColor().GetColor() || bChgLineColor;

        if ( bChgFntColor )
        {
            // check if current background has a user defined setting
            const Color* pCol = GetFont() ? GetFont()->GetBackColor() : NULL;
            if( ! pCol || COL_TRANSPARENT == pCol->GetColor() )
            {
                const SvxBrushItem* pItem;
                SwRect aOrigBackRect;

                /// OD 21.08.2002
                ///     consider, that [GetBackgroundBrush(...)] can set <pCol>
                ///     - see implementation in /core/layout/paintfrm.cxx
                /// OD 21.08.2002 #99657#
                ///     There is a user defined setting for the background, if there
                ///     is a background brush and its color is *not* "no fill"/"auto fill".
                if( GetFrm()->GetBackgroundBrush( pItem, pCol, aOrigBackRect, FALSE ) )
                {
                    if ( !pCol )
                    {
                        pCol = &pItem->GetColor();
                    }

                    /// OD 30.08.2002 #99657#
                    /// determined color <pCol> can be <COL_TRANSPARENT>. Thus, check it.
                    if ( pCol->GetColor() == COL_TRANSPARENT)
                        pCol = NULL;
                }
                else
                    pCol = NULL;
            }

            // no user defined color at paragraph or font background
            if ( ! pCol )
                pCol = &aGlobalRetoucheColor;

            if( GetShell() && GetShell()->GetWin() )
            {
                // here we determine the prefered window text color for painting
                const SwViewOption* pViewOption = GetShell()->GetViewOptions();
                if(pViewOption->IsPagePreview() &&
                        !SW_MOD()->GetAccessibilityOptions().GetIsForPagePreviews())
                    nNewColor = COL_BLACK;
                else
                    // we take the font color from the appearence page
                    nNewColor = SwViewOption::GetFontColor().GetColor();
            }

            // change painting color depending of dark/bright background
            Color aTmpColor( nNewColor );
            if ( pCol->IsDark() && aTmpColor.IsDark() )
                nNewColor = COL_WHITE;
            else if ( pCol->IsBright() && aTmpColor.IsBright() )
                nNewColor = COL_BLACK;
        }
    }

    if ( bChgFntColor || bChgLineColor )
    {
        Color aNewColor( nNewColor );

        if ( bChgFntColor )
        {
            if ( pFont && aNewColor != pFont->GetColor() )
            {
                // only set the new color at the font passed as argument
                pFont->SetColor( aNewColor );
            }
            else if ( aNewColor != GetOut().GetFont().GetColor() )
            {
                // set new font with new color at output device
                Font aFont( rFnt );
                aFont.SetColor( aNewColor );
                GetOut().SetFont( aFont );
            }
        }

        // the underline and overline colors have to be set separately
        if ( bChgLineColor )
        {
            // get current font color or color set at output device
            aNewColor = pFont ? pFont->GetColor() : GetOut().GetFont().GetColor();
            if ( aNewColor != GetOut().GetLineColor() )
                GetOut().SetLineColor( aNewColor );
            if ( aNewColor != GetOut().GetOverlineColor() )
                GetOut().SetOverlineColor( aNewColor );
        }

        return sal_True;
    }

    return sal_False;
}

