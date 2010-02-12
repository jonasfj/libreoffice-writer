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


#include <hintids.hxx>

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#include <unotools/localedatawrapper.hxx>
#include <svx/unolingu.hxx>
#include <svx/brshitem.hxx>
#include <svx/wrlmitem.hxx>
#include <svx/blnkitem.hxx>
#include <svx/nhypitem.hxx>
#include <svx/kernitem.hxx>
#include <svx/cmapitem.hxx>
#include <svx/langitem.hxx>
#include <svx/escpitem.hxx>
#include <svx/akrnitem.hxx>
#include <svx/shdditem.hxx>
#include <svx/charreliefitem.hxx>
#ifndef _SVX_CNTRITEM_HXX //autogen
#include <svx/cntritem.hxx>
#endif
#include <svx/colritem.hxx>
#include <svx/cscoitem.hxx>
#include <svx/crsditem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/postitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#ifndef _SVX_EMPHITEM_HXX //autogen
#include <svx/emphitem.hxx>
#endif
#include <svx/charscaleitem.hxx>
#include <svx/charrotateitem.hxx>
#include <svx/twolinesitem.hxx>
#include <svx/charhiddenitem.hxx>
#include <IDocumentSettingAccess.hxx>
#include <vcl/window.hxx>
#include <charatr.hxx>
#include <viewsh.hxx>		// Bildschirmabgleich
#include <swfont.hxx>
#include <fntcache.hxx>     // FontCache
#include <txtfrm.hxx>       // SwTxtFrm
#include <scriptinfo.hxx>

#if defined(WIN) || defined(WNT) || defined(PM2)
#define FNT_LEADING_HACK
#endif

#if defined(WIN) || defined(WNT)
#define FNT_ATM_HACK
#endif

#ifdef DBG_UTIL
// globale Variable
SvStatistics aSvStat;
#endif

using namespace ::com::sun::star;

/************************************************************************
 * Hintergrundbrush setzen, z.B. bei Zeichenvorlagen
 ***********************************************************************/

void SwFont::SetBackColor( Color* pNewColor )
{
    delete pBackColor;
    pBackColor = pNewColor;
    bFntChg = TRUE;
    aSub[SW_LATIN].pMagic = aSub[SW_CJK].pMagic = aSub[SW_CTL].pMagic = 0;
}

// maps directions for vertical layout
USHORT MapDirection( USHORT nDir, const BOOL bVertFormat )
{
    if ( bVertFormat )
    {
        switch ( nDir )
        {
        case 0 :
            nDir = 2700;
            break;
        case 900 :
            nDir = 0;
            break;
        case 2700 :
            nDir = 1800;
            break;
#if OSL_DEBUG_LEVEL > 1
        default :
            ASSERT( sal_False, "Unsupported direction" );
            break;
#endif
        }
    }
    return nDir;
}

// maps the absolute direction set at the font to its logical conterpart
// in the rotated environment
USHORT UnMapDirection( USHORT nDir, const BOOL bVertFormat )
{
    if ( bVertFormat )
    {
        switch ( nDir )
        {
        case 0 :
            nDir = 900;
            break;
        case 1800 :
            nDir = 2700;
            break;
        case 2700 :
            nDir = 0;
            break;
#if OSL_DEBUG_LEVEL > 1
        default :
            ASSERT( sal_False, "Unsupported direction" );
            break;
#endif
        }
    }
    return nDir;
}

USHORT SwFont::GetOrientation( const BOOL bVertFormat ) const
{
    return UnMapDirection( aSub[nActual].GetOrientation(), bVertFormat );
}

void SwFont::SetVertical( USHORT nDir, const BOOL bVertFormat )
{
    // map direction if frame has vertical layout
    nDir = MapDirection( nDir, bVertFormat );

    if( nDir != aSub[0].GetOrientation() )
    {
        bFntChg = TRUE;
        aSub[0].SetVertical( nDir, bVertFormat );
        aSub[1].SetVertical( nDir, bVertFormat || nDir > 1000 );
        aSub[2].SetVertical( nDir, bVertFormat );
    }
}

/*************************************************************************
 Escapement:
    frEsc:  Fraction, Grad des Escapements
    Esc = resultierendes Escapement
    A1 = Original-Ascent			(nOrgAscent)
    A2 = verkleinerter Ascent		(nEscAscent)
    Ax = resultierender Ascent		(GetAscent())
    H1 = Original-Hoehe 			(nOrgHeight)
    H2 = verkleinerter Hoehe		(nEscHeight)
    Hx = resultierender Hoehe		(GetHeight())
    Bx = resultierende Baseline fuer die Textausgabe (CalcPos())
         (Vorsicht: Y - A1!)

    Escapement:
        Esc = H1 * frEsc;

    Hochstellung:
        Ax = A2 + Esc;
        Hx = H2 + Esc;
        Bx = A1 - Esc;

    Tiefstellung:
        Ax = A1;
        Hx = A1 + Esc + (H2 - A2);
        Bx = A1 + Esc;

*************************************************************************/

/*************************************************************************
 *					SwSubFont::CalcEscAscent( const USHORT nOldAscent )
 *************************************************************************/

// nEsc ist der Prozentwert
USHORT SwSubFont::CalcEscAscent( const USHORT nOldAscent ) const
{
    if( DFLT_ESC_AUTO_SUPER != GetEscapement() &&
        DFLT_ESC_AUTO_SUB != GetEscapement() )
    {
        const long nAscent = nOldAscent +
                             ( (long) nOrgHeight * GetEscapement() ) / 100L;
        if ( nAscent>0 )
            return ( Max( USHORT (nAscent), nOrgAscent ));
    }
    return nOrgAscent;
}

/*************************************************************************
 *                      SwFont::SetDiffFnt()
 *************************************************************************/

void SwFont::SetDiffFnt( const SfxItemSet *pAttrSet,
                         const IDocumentSettingAccess *pIDocumentSettingAccess )
{
    delete pBackColor;
    pBackColor = NULL;

    if( pAttrSet )
    {
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_FONT,
            TRUE, &pItem ))
        {
            const SvxFontItem *pFont = (const SvxFontItem *)pItem;
            aSub[SW_LATIN].SetFamily( pFont->GetFamily() );
            aSub[SW_LATIN].Font::SetName( pFont->GetFamilyName() );
            aSub[SW_LATIN].Font::SetStyleName( pFont->GetStyleName() );
            aSub[SW_LATIN].Font::SetPitch( pFont->GetPitch() );
            aSub[SW_LATIN].Font::SetCharSet( pFont->GetCharSet() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_FONTSIZE,
            TRUE, &pItem ))
        {
            const SvxFontHeightItem *pHeight = (const SvxFontHeightItem *)pItem;
            aSub[SW_LATIN].SvxFont::SetPropr( 100 );
            aSub[SW_LATIN].aSize = aSub[SW_LATIN].Font::GetSize();
            Size aTmpSize = aSub[SW_LATIN].aSize;
            aTmpSize.Height() = pHeight->GetHeight();
            aSub[SW_LATIN].SetSize( aTmpSize );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_POSTURE,
            TRUE, &pItem ))
            aSub[SW_LATIN].Font::SetItalic( ((SvxPostureItem*)pItem)->GetPosture() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_WEIGHT,
            TRUE, &pItem ))
            aSub[SW_LATIN].Font::SetWeight( ((SvxWeightItem*)pItem)->GetWeight() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_LANGUAGE,
            TRUE, &pItem ))
            aSub[SW_LATIN].SetLanguage( ((SvxLanguageItem*)pItem)->GetLanguage() );

        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CJK_FONT,
            TRUE, &pItem ))
        {
            const SvxFontItem *pFont = (const SvxFontItem *)pItem;
            aSub[SW_CJK].SetFamily( pFont->GetFamily() );
            aSub[SW_CJK].Font::SetName( pFont->GetFamilyName() );
            aSub[SW_CJK].Font::SetStyleName( pFont->GetStyleName() );
            aSub[SW_CJK].Font::SetPitch( pFont->GetPitch() );
            aSub[SW_CJK].Font::SetCharSet( pFont->GetCharSet() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CJK_FONTSIZE,
            TRUE, &pItem ))
        {
            const SvxFontHeightItem *pHeight = (const SvxFontHeightItem *)pItem;
            aSub[SW_CJK].SvxFont::SetPropr( 100 );
            aSub[SW_CJK].aSize = aSub[SW_CJK].Font::GetSize();
            Size aTmpSize = aSub[SW_CJK].aSize;
            aTmpSize.Height() = pHeight->GetHeight();
            aSub[SW_CJK].SetSize( aTmpSize );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CJK_POSTURE,
            TRUE, &pItem ))
            aSub[SW_CJK].Font::SetItalic( ((SvxPostureItem*)pItem)->GetPosture() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CJK_WEIGHT,
            TRUE, &pItem ))
            aSub[SW_CJK].Font::SetWeight( ((SvxWeightItem*)pItem)->GetWeight() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CJK_LANGUAGE,
            TRUE, &pItem ))
        {
            LanguageType eNewLang = ((SvxLanguageItem*)pItem)->GetLanguage();
            aSub[SW_CJK].SetLanguage( eNewLang );
            aSub[SW_LATIN].SetCJKContextLanguage( eNewLang );
            aSub[SW_CJK].SetCJKContextLanguage( eNewLang );
            aSub[SW_CTL].SetCJKContextLanguage( eNewLang );
        }

        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CTL_FONT,
            TRUE, &pItem ))
        {
            const SvxFontItem *pFont = (const SvxFontItem *)pItem;
            aSub[SW_CTL].SetFamily( pFont->GetFamily() );
            aSub[SW_CTL].Font::SetName( pFont->GetFamilyName() );
            aSub[SW_CTL].Font::SetStyleName( pFont->GetStyleName() );
            aSub[SW_CTL].Font::SetPitch( pFont->GetPitch() );
            aSub[SW_CTL].Font::SetCharSet( pFont->GetCharSet() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CTL_FONTSIZE,
            TRUE, &pItem ))
        {
            const SvxFontHeightItem *pHeight = (const SvxFontHeightItem *)pItem;
            aSub[SW_CTL].SvxFont::SetPropr( 100 );
            aSub[SW_CTL].aSize = aSub[SW_CTL].Font::GetSize();
            Size aTmpSize = aSub[SW_CTL].aSize;
            aTmpSize.Height() = pHeight->GetHeight();
            aSub[SW_CTL].SetSize( aTmpSize );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CTL_POSTURE,
            TRUE, &pItem ))
            aSub[SW_CTL].Font::SetItalic( ((SvxPostureItem*)pItem)->GetPosture() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CTL_WEIGHT,
            TRUE, &pItem ))
            aSub[SW_CTL].Font::SetWeight( ((SvxWeightItem*)pItem)->GetWeight() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CTL_LANGUAGE,
            TRUE, &pItem ))
            aSub[SW_CTL].SetLanguage( ((SvxLanguageItem*)pItem)->GetLanguage() );

        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_UNDERLINE,
            TRUE, &pItem ))
        {
            SetUnderline( ((SvxUnderlineItem*)pItem)->GetLineStyle() );
            SetUnderColor( ((SvxUnderlineItem*)pItem)->GetColor() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_OVERLINE,
            TRUE, &pItem ))
        {
            SetOverline( ((SvxOverlineItem*)pItem)->GetLineStyle() );
            SetOverColor( ((SvxOverlineItem*)pItem)->GetColor() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CROSSEDOUT,
            TRUE, &pItem ))
            SetStrikeout( ((SvxCrossedOutItem*)pItem)->GetStrikeout() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_COLOR,
            TRUE, &pItem ))
            SetColor( ((SvxColorItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_EMPHASIS_MARK,
            TRUE, &pItem ))
            SetEmphasisMark( ((SvxEmphasisMarkItem*)pItem)->GetEmphasisMark() );

        SetTransparent( TRUE );
        SetAlign( ALIGN_BASELINE );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CONTOUR,
            TRUE, &pItem ))
            SetOutline( ((SvxContourItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_SHADOWED,
            TRUE, &pItem ))
            SetShadow( ((SvxShadowedItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_RELIEF,
            TRUE, &pItem ))
            SetRelief( (FontRelief)((SvxCharReliefItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_SHADOWED,
            TRUE, &pItem ))
            SetPropWidth(((SvxShadowedItem*)pItem)->GetValue() ? 50 : 100 );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_AUTOKERN,
            TRUE, &pItem ))
        {
            if( ((SvxAutoKernItem*)pItem)->GetValue() )
            {
                SetAutoKern( ( !pIDocumentSettingAccess ||
                               !pIDocumentSettingAccess->get(IDocumentSettingAccess::KERN_ASIAN_PUNCTUATION) ) ?
                                KERNING_FONTSPECIFIC :
                                KERNING_ASIAN );
            }
            else
                SetAutoKern( 0 );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_WORDLINEMODE,
            TRUE, &pItem ))
            SetWordLineMode( ((SvxWordLineModeItem*)pItem)->GetValue() );

        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_ESCAPEMENT,
            TRUE, &pItem ))
        {
            const SvxEscapementItem *pEsc = (const SvxEscapementItem *)pItem;
            SetEscapement( pEsc->GetEsc() );
            if( aSub[SW_LATIN].IsEsc() )
                SetProportion( pEsc->GetProp() );
        }
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_CASEMAP,
            TRUE, &pItem ))
            SetCaseMap( ((SvxCaseMapItem*)pItem)->GetCaseMap() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_KERNING,
            TRUE, &pItem ))
            SetFixKerning( ((SvxKerningItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_NOHYPHEN,
            TRUE, &pItem ))
            SetNoHyph( ((SvxNoHyphenItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_BLINK,
            TRUE, &pItem ))
            SetBlink( ((SvxBlinkItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_ROTATE,
            TRUE, &pItem ))
            SetVertical( ((SvxCharRotateItem*)pItem)->GetValue() );
        if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_BACKGROUND,
            TRUE, &pItem ))
            pBackColor = new Color( ((SvxBrushItem*)pItem)->GetColor() );
        else
            pBackColor = NULL;
        const SfxPoolItem* pTwoLinesItem = 0;
        if( SFX_ITEM_SET ==
                pAttrSet->GetItemState( RES_CHRATR_TWO_LINES, TRUE, &pTwoLinesItem ))
            if ( ((SvxTwoLinesItem*)pTwoLinesItem)->GetValue() )
                SetVertical( 0 );
    }
    else
    {
        Invalidate();
        bNoHyph = FALSE;
        bBlink = FALSE;
    }
    bPaintBlank = FALSE;
    bPaintWrong = FALSE;
    ASSERT( aSub[SW_LATIN].IsTransparent(), "SwFont: Transparent revolution" );
}

/*************************************************************************
 *						class SwFont
 *************************************************************************/

SwFont::SwFont( const SwFont &rFont )
{
    aSub[SW_LATIN] = rFont.aSub[SW_LATIN];
    aSub[SW_CJK] = rFont.aSub[SW_CJK];
    aSub[SW_CTL] = rFont.aSub[SW_CTL];
    nActual = rFont.nActual;
    pBackColor = rFont.pBackColor ? new Color( *rFont.pBackColor ) : NULL;
    aUnderColor = rFont.GetUnderColor();
    aOverColor  = rFont.GetOverColor();
    nToxCnt = 0;
    nRefCnt = 0;
    m_nMetaCount = 0;
    bFntChg = rFont.bFntChg;
    bOrgChg = rFont.bOrgChg;
    bPaintBlank = rFont.bPaintBlank;
    bPaintWrong = FALSE;
    bURL = rFont.bURL;
    bGreyWave = rFont.bGreyWave;
    bNoColReplace = rFont.bNoColReplace;
    bNoHyph = rFont.bNoHyph;
    bBlink = rFont.bBlink;
}

SwFont::SwFont( const SwAttrSet* pAttrSet,
                const IDocumentSettingAccess* pIDocumentSettingAccess )
{
    nActual = SW_LATIN;
    nToxCnt = 0;
    nRefCnt = 0;
    m_nMetaCount = 0;
    bPaintBlank = FALSE;
    bPaintWrong = FALSE;
    bURL = FALSE;
    bGreyWave = FALSE;
    bNoColReplace = FALSE;
    bNoHyph = pAttrSet->GetNoHyphenHere().GetValue();
    bBlink = pAttrSet->GetBlink().GetValue();
    bOrgChg = TRUE;
    {
        const SvxFontItem& rFont = pAttrSet->GetFont();
        aSub[SW_LATIN].SetFamily( rFont.GetFamily() );
        aSub[SW_LATIN].SetName( rFont.GetFamilyName() );
        aSub[SW_LATIN].SetStyleName( rFont.GetStyleName() );
        aSub[SW_LATIN].SetPitch( rFont.GetPitch() );
        aSub[SW_LATIN].SetCharSet( rFont.GetCharSet() );
        aSub[SW_LATIN].SvxFont::SetPropr( 100 );   // 100% der FontSize
        Size aTmpSize = aSub[SW_LATIN].aSize;
        aTmpSize.Height() = pAttrSet->GetSize().GetHeight();
        aSub[SW_LATIN].SetSize( aTmpSize );
        aSub[SW_LATIN].SetItalic( pAttrSet->GetPosture().GetPosture() );
        aSub[SW_LATIN].SetWeight( pAttrSet->GetWeight().GetWeight() );
        aSub[SW_LATIN].SetLanguage( pAttrSet->GetLanguage().GetLanguage() );
    }

    {
        const SvxFontItem& rFont = pAttrSet->GetCJKFont();
        aSub[SW_CJK].SetFamily( rFont.GetFamily() );
        aSub[SW_CJK].SetName( rFont.GetFamilyName() );
        aSub[SW_CJK].SetStyleName( rFont.GetStyleName() );
        aSub[SW_CJK].SetPitch( rFont.GetPitch() );
        aSub[SW_CJK].SetCharSet( rFont.GetCharSet() );
        aSub[SW_CJK].SvxFont::SetPropr( 100 );   // 100% der FontSize
        Size aTmpSize = aSub[SW_CJK].aSize;
        aTmpSize.Height() = pAttrSet->GetCJKSize().GetHeight();
        aSub[SW_CJK].SetSize( aTmpSize );
        aSub[SW_CJK].SetItalic( pAttrSet->GetCJKPosture().GetPosture() );
        aSub[SW_CJK].SetWeight( pAttrSet->GetCJKWeight().GetWeight() );
        LanguageType eNewLang = pAttrSet->GetCJKLanguage().GetLanguage();
        aSub[SW_CJK].SetLanguage( eNewLang );
        aSub[SW_LATIN].SetCJKContextLanguage( eNewLang );
        aSub[SW_CJK].SetCJKContextLanguage( eNewLang );
        aSub[SW_CTL].SetCJKContextLanguage( eNewLang );
    }

    {
        const SvxFontItem& rFont = pAttrSet->GetCTLFont();
        aSub[SW_CTL].SetFamily( rFont.GetFamily() );
        aSub[SW_CTL].SetName( rFont.GetFamilyName() );
        aSub[SW_CTL].SetStyleName( rFont.GetStyleName() );
        aSub[SW_CTL].SetPitch( rFont.GetPitch() );
        aSub[SW_CTL].SetCharSet( rFont.GetCharSet() );
        aSub[SW_CTL].SvxFont::SetPropr( 100 );   // 100% der FontSize
        Size aTmpSize = aSub[SW_CTL].aSize;
        aTmpSize.Height() = pAttrSet->GetCTLSize().GetHeight();
        aSub[SW_CTL].SetSize( aTmpSize );
        aSub[SW_CTL].SetItalic( pAttrSet->GetCTLPosture().GetPosture() );
        aSub[SW_CTL].SetWeight( pAttrSet->GetCTLWeight().GetWeight() );
        aSub[SW_CTL].SetLanguage( pAttrSet->GetCTLLanguage().GetLanguage() );
    }

    const FontUnderline eUnderline = pAttrSet->GetUnderline().GetLineStyle();
    if ( pAttrSet->GetCharHidden().GetValue() )
        SetUnderline( UNDERLINE_DOTTED );
    else
        SetUnderline( eUnderline );
    SetUnderColor( pAttrSet->GetUnderline().GetColor() );
    SetOverline( pAttrSet->GetOverline().GetLineStyle() );
    SetOverColor( pAttrSet->GetOverline().GetColor() );
    SetEmphasisMark( pAttrSet->GetEmphasisMark().GetEmphasisMark() );
    SetStrikeout( pAttrSet->GetCrossedOut().GetStrikeout() );
    SetColor( pAttrSet->GetColor().GetValue() );
    SetTransparent( TRUE );
    SetAlign( ALIGN_BASELINE );
    SetOutline( pAttrSet->GetContour().GetValue() );
    SetShadow( pAttrSet->GetShadowed().GetValue() );
    SetPropWidth( pAttrSet->GetCharScaleW().GetValue() );
    SetRelief( (FontRelief)pAttrSet->GetCharRelief().GetValue() );
    if( pAttrSet->GetAutoKern().GetValue() )
    {
        SetAutoKern( ( !pIDocumentSettingAccess ||
                       !pIDocumentSettingAccess->get(IDocumentSettingAccess::KERN_ASIAN_PUNCTUATION) ) ?
                        KERNING_FONTSPECIFIC :
                        KERNING_ASIAN );
    }
    else
        SetAutoKern( 0 );
    SetWordLineMode( pAttrSet->GetWordLineMode().GetValue() );
    const SvxEscapementItem &rEsc = pAttrSet->GetEscapement();
    SetEscapement( rEsc.GetEsc() );
    if( aSub[SW_LATIN].IsEsc() )
        SetProportion( rEsc.GetProp() );
    SetCaseMap( pAttrSet->GetCaseMap().GetCaseMap() );
    SetFixKerning( pAttrSet->GetKerning().GetValue() );
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == pAttrSet->GetItemState( RES_CHRATR_BACKGROUND,
        TRUE, &pItem ))
        pBackColor = new Color( ((SvxBrushItem*)pItem)->GetColor() );
    else
        pBackColor = NULL;
    const SvxTwoLinesItem& rTwoLinesItem = pAttrSet->Get2Lines();
    if ( ! rTwoLinesItem.GetValue() )
        SetVertical( pAttrSet->GetCharRotate().GetValue() );
    else
        SetVertical( 0 );
}

SwSubFont& SwSubFont::operator=( const SwSubFont &rFont )
{
    SvxFont::operator=( rFont );
    pMagic = rFont.pMagic;
    nFntIndex = rFont.nFntIndex;
    nOrgHeight = rFont.nOrgHeight;
    nOrgAscent = rFont.nOrgAscent;
    nPropWidth = rFont.nPropWidth;
    aSize = rFont.aSize;
    return *this;
}

SwFont& SwFont::operator=( const SwFont &rFont )
{
    aSub[SW_LATIN] = rFont.aSub[SW_LATIN];
    aSub[SW_CJK] = rFont.aSub[SW_CJK];
    aSub[SW_CTL] = rFont.aSub[SW_CTL];
    nActual = rFont.nActual;
    delete pBackColor;
    pBackColor = rFont.pBackColor ? new Color( *rFont.pBackColor ) : NULL;
    aUnderColor = rFont.GetUnderColor();
    aOverColor  = rFont.GetOverColor();
    nToxCnt = 0;
    nRefCnt = 0;
    m_nMetaCount = 0;
    bFntChg = rFont.bFntChg;
    bOrgChg = rFont.bOrgChg;
    bPaintBlank = rFont.bPaintBlank;
    bPaintWrong = FALSE;
    bURL = rFont.bURL;
    bGreyWave = rFont.bGreyWave;
    bNoColReplace = rFont.bNoColReplace;
    bNoHyph = rFont.bNoHyph;
    bBlink = rFont.bBlink;
    return *this;
}

/*************************************************************************
 *						SwFont::GoMagic()
 *************************************************************************/

void SwFont::GoMagic( ViewShell *pSh, BYTE nWhich )
{
    SwFntAccess aFntAccess( aSub[nWhich].pMagic, aSub[nWhich].nFntIndex,
                            &aSub[nWhich], pSh, TRUE );
}

/*************************************************************************
 *						SwSubFont::IsSymbol()
 *************************************************************************/

BOOL SwSubFont::IsSymbol( ViewShell *pSh )
{
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh, FALSE );
    return aFntAccess.Get()->IsSymbol();
}

/*************************************************************************
 *						SwSubFont::ChgFnt()
 *************************************************************************/

BOOL SwSubFont::ChgFnt( ViewShell *pSh, OutputDevice& rOut )
{
    if ( pLastFont )
        pLastFont->Unlock();
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh, TRUE );
    SV_STAT( nChangeFont );

    pLastFont = aFntAccess.Get();

    pLastFont->SetDevFont( pSh, rOut );

    pLastFont->Lock();
    return UNDERLINE_NONE != GetUnderline() ||
           UNDERLINE_NONE != GetOverline()  ||
           STRIKEOUT_NONE != GetStrikeout();
}

/*************************************************************************
 *					  SwFont::ChgPhysFnt()
 *************************************************************************/

void SwFont::ChgPhysFnt( ViewShell *pSh, OutputDevice& rOut )
{
    if( bOrgChg && aSub[nActual].IsEsc() )
    {
        const BYTE nOldProp = aSub[nActual].GetPropr();
        SetProportion( 100 );
        ChgFnt( pSh, rOut );
        SwFntAccess aFntAccess( aSub[nActual].pMagic, aSub[nActual].nFntIndex,
                                &aSub[nActual], pSh );
        aSub[nActual].nOrgHeight = aFntAccess.Get()->GetFontHeight( pSh, rOut );
        aSub[nActual].nOrgAscent = aFntAccess.Get()->GetFontAscent( pSh, rOut );
        SetProportion( nOldProp );
        bOrgChg = FALSE;
    }

    if( bFntChg )
    {
        ChgFnt( pSh, rOut );
        bFntChg = bOrgChg;
    }
    if( rOut.GetTextLineColor() != aUnderColor )
        rOut.SetTextLineColor( aUnderColor );
    if( rOut.GetOverlineColor() != aOverColor )
        rOut.SetOverlineColor( aOverColor );
}

/*************************************************************************
 *						SwFont::CalcEscHeight()
 *         Height = MaxAscent + MaxDescent
 *      MaxAscent = Max (T1_ascent, T2_ascent + (Esc * T1_height) );
 *     MaxDescent = Max (T1_height-T1_ascent,
 * 						 T2_height-T2_ascent - (Esc * T1_height)
 *************************************************************************/

USHORT SwSubFont::CalcEscHeight( const USHORT nOldHeight,
                              const USHORT nOldAscent  ) const
{
    if( DFLT_ESC_AUTO_SUPER != GetEscapement() &&
        DFLT_ESC_AUTO_SUB != GetEscapement() )
    {
        long nDescent = nOldHeight - nOldAscent -
                             ( (long) nOrgHeight * GetEscapement() ) / 100L;
        const USHORT nDesc = ( nDescent>0 ) ? Max ( USHORT(nDescent),
                   USHORT(nOrgHeight - nOrgAscent) ) : nOrgHeight - nOrgAscent;
        return ( nDesc + CalcEscAscent( nOldAscent ) );
    }
    return nOrgHeight;
}

short SwSubFont::_CheckKerning( )
{
    short nKernx = - short( Font::GetSize().Height() / 6 );

    if ( nKernx < GetFixKerning() )
        return GetFixKerning();
    return nKernx;
}

/*************************************************************************
 *                    SwSubFont::GetAscent()
 *************************************************************************/

USHORT SwSubFont::GetAscent( ViewShell *pSh, const OutputDevice& rOut )
{
    USHORT nAscent;
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh );
    nAscent = aFntAccess.Get()->GetFontAscent( pSh, rOut );
    if( GetEscapement() )
        nAscent = CalcEscAscent( nAscent );
    return nAscent;
}

/*************************************************************************
 *					  SwSubFont::GetHeight()
 *************************************************************************/

USHORT SwSubFont::GetHeight( ViewShell *pSh, const OutputDevice& rOut )
{
    SV_STAT( nGetTextSize );
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh );
    const USHORT nHeight = aFntAccess.Get()->GetFontHeight( pSh, rOut );
    if ( GetEscapement() )
    {
        const USHORT nAscent = aFntAccess.Get()->GetFontAscent( pSh, rOut );
        return CalcEscHeight( nHeight, nAscent ); // + nLeading;
    }
    return nHeight; // + nLeading;
}

/*************************************************************************
 *					  SwSubFont::_GetTxtSize()
 *************************************************************************/
Size SwSubFont::_GetTxtSize( SwDrawTextInfo& rInf )
{
    // Robust: Eigentlich sollte der Font bereits eingestellt sein, aber
    // sicher ist sicher ...
    if ( !pLastFont || pLastFont->GetOwner()!=pMagic ||
         !IsSameInstance( rInf.GetpOut()->GetFont() ) )
        ChgFnt( rInf.GetShell(), rInf.GetOut() );

    SwDigitModeModifier aDigitModeModifier( rInf.GetOut(), rInf.GetFont()->GetLanguage() );

    Size aTxtSize;
    xub_StrLen nLn = ( rInf.GetLen() == STRING_LEN ? rInf.GetText().Len()
                                                   : rInf.GetLen() );
    rInf.SetLen( nLn );
    if( IsCapital() && nLn )
        aTxtSize = GetCapitalSize( rInf );
    else
    {
        SV_STAT( nGetTextSize );
        long nOldKern = rInf.GetKern();
        const XubString &rOldTxt = rInf.GetText();
        rInf.SetKern( CheckKerning() );
        if ( !IsCaseMap() )
            aTxtSize = pLastFont->GetTextSize( rInf );
        else
        {
            String aTmp = CalcCaseMap( rInf.GetText() );
            const XubString &rOldStr = rInf.GetText();
            sal_Bool bCaseMapLengthDiffers(aTmp.Len() != rOldStr.Len());

            if(bCaseMapLengthDiffers && rInf.GetLen())
            {
                // #108203#
                // If the length of the original string and the CaseMapped one
                // are different, it is necessary to handle the given text part as
                // a single snippet since it�s size may differ, too.
                xub_StrLen nOldIdx(rInf.GetIdx());
                xub_StrLen nOldLen(rInf.GetLen());
                const XubString aSnippet(rOldStr, nOldIdx, nOldLen);
                XubString aNewText(CalcCaseMap(aSnippet));

                rInf.SetText( aNewText );
                rInf.SetIdx( 0 );
                rInf.SetLen( aNewText.Len() );

                aTxtSize = pLastFont->GetTextSize( rInf );

                rInf.SetIdx( nOldIdx );
                rInf.SetLen( nOldLen );
            }
            else
            {
                rInf.SetText( aTmp );
                aTxtSize = pLastFont->GetTextSize( rInf );
            }

            rInf.SetText( rOldStr );
        }
        rInf.SetKern( nOldKern );
        rInf.SetText( rOldTxt );
        // 15142: Ein Wort laenger als eine Zeile, beim Zeilenumbruch
        //        hochgestellt, muss seine effektive Hoehe melden.
        if( GetEscapement() )
        {
            const USHORT nAscent = pLastFont->GetFontAscent( rInf.GetShell(),
                                                             rInf.GetOut() );
            aTxtSize.Height() =
                (long)CalcEscHeight( (USHORT)aTxtSize.Height(), nAscent);
        }
    }

    if (1==rInf.GetLen() && CH_TXT_ATR_FIELDSTART==rInf.GetText().GetChar(rInf.GetIdx()))
    {
        xub_StrLen nOldIdx(rInf.GetIdx());
        xub_StrLen nOldLen(rInf.GetLen());
        String aNewText=String::CreateFromAscii(CH_TXT_ATR_SUBST_FIELDSTART);
        rInf.SetText( aNewText );
        rInf.SetIdx( 0 );
        rInf.SetLen( aNewText.Len() );
        aTxtSize = pLastFont->GetTextSize( rInf );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }
    else if (1==rInf.GetLen() && CH_TXT_ATR_FIELDEND==rInf.GetText().GetChar(rInf.GetIdx()))
    {
        xub_StrLen nOldIdx(rInf.GetIdx());
        xub_StrLen nOldLen(rInf.GetLen());
        String aNewText=String::CreateFromAscii(CH_TXT_ATR_SUBST_FIELDEND);
        rInf.SetText( aNewText );
        rInf.SetIdx( 0 );
        rInf.SetLen( aNewText.Len() );
        aTxtSize = pLastFont->GetTextSize( rInf );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }

    return aTxtSize;
}

/*************************************************************************
 *					  SwSubFont::_DrawText()
 *************************************************************************/

void SwSubFont::_DrawText( SwDrawTextInfo &rInf, const BOOL bGrey )
{
    rInf.SetGreyWave( bGrey );
    xub_StrLen nLn = rInf.GetText().Len();
    if( !rInf.GetLen() || !nLn )
        return;
    if( STRING_LEN == rInf.GetLen() )
        rInf.SetLen( nLn );

    FontUnderline nOldUnder = UNDERLINE_NONE;
    SwUnderlineFont* pUnderFnt = 0;

    if( rInf.GetUnderFnt() )
    {
        nOldUnder = GetUnderline();
        SetUnderline( UNDERLINE_NONE );
        pUnderFnt = rInf.GetUnderFnt();
    }

    if( !pLastFont || pLastFont->GetOwner()!=pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetOut() );

    SwDigitModeModifier aDigitModeModifier( rInf.GetOut(), rInf.GetFont()->GetLanguage() );

    Point aPos( rInf.GetPos() );
    const Point &rOld = rInf.GetPos();
    rInf.SetPos( aPos );

    if( GetEscapement() )
        CalcEsc( rInf, aPos );

    rInf.SetKern( CheckKerning() + rInf.GetSperren() / SPACING_PRECISION_FACTOR );

    if( IsCapital() )
        DrawCapital( rInf );
    else
    {
        SV_STAT( nDrawText );
        if ( !IsCaseMap() )
            pLastFont->DrawText( rInf );
        else
        {
            const XubString &rOldStr = rInf.GetText();
            XubString aString( CalcCaseMap( rOldStr ) );
            sal_Bool bCaseMapLengthDiffers(aString.Len() != rOldStr.Len());

            if(bCaseMapLengthDiffers && rInf.GetLen())
            {
                // #108203#
                // If the length of the original string and the CaseMapped one
                // are different, it is necessary to handle the given text part as
                // a single snippet since it�s size may differ, too.
                xub_StrLen nOldIdx(rInf.GetIdx());
                xub_StrLen nOldLen(rInf.GetLen());
                const XubString aSnippet(rOldStr, nOldIdx, nOldLen);
                XubString aNewText = CalcCaseMap(aSnippet);

                rInf.SetText( aNewText );
                rInf.SetIdx( 0 );
                rInf.SetLen( aNewText.Len() );

                pLastFont->DrawText( rInf );

                rInf.SetIdx( nOldIdx );
                rInf.SetLen( nOldLen );
            }
            else
            {
                rInf.SetText( aString );
                pLastFont->DrawText( rInf );
            }

            rInf.SetText( rOldStr );
        }
    }

    if( pUnderFnt && nOldUnder != UNDERLINE_NONE )
    {
static sal_Char __READONLY_DATA sDoubleSpace[] = "  ";
        Size aFontSize = _GetTxtSize( rInf );
        const XubString &rOldStr = rInf.GetText();
        XubString aStr( sDoubleSpace, RTL_TEXTENCODING_MS_1252 );

        xub_StrLen nOldIdx = rInf.GetIdx();
        xub_StrLen nOldLen = rInf.GetLen();
        long nSpace = 0;
        if( rInf.GetSpace() )
        {
            xub_StrLen nTmpEnd = nOldIdx + nOldLen;
            if( nTmpEnd > rOldStr.Len() )
                nTmpEnd = rOldStr.Len();

            const SwScriptInfo* pSI = rInf.GetScriptInfo();

            const sal_Bool bAsianFont =
                ( rInf.GetFont() && SW_CJK == rInf.GetFont()->GetActual() );
            for( xub_StrLen nTmp = nOldIdx; nTmp < nTmpEnd; ++nTmp )
            {
                if( CH_BLANK == rOldStr.GetChar( nTmp ) || bAsianFont ||
                    ( nTmp + 1 < rOldStr.Len() && pSI &&
                      i18n::ScriptType::ASIAN == pSI->ScriptType( nTmp + 1 ) ) )
                    ++nSpace;
            }

            // if next portion if a hole portion we do not consider any
            // extra space added because the last character was ASIAN
            if ( nSpace && rInf.IsSpaceStop() && bAsianFont )
                 --nSpace;

            nSpace *= rInf.GetSpace() / SPACING_PRECISION_FACTOR;
        }

        rInf.SetWidth( USHORT(aFontSize.Width() + nSpace) );
        rInf.SetText( aStr );
        rInf.SetIdx( 0 );
        rInf.SetLen( 2 );
        SetUnderline( nOldUnder );
        rInf.SetUnderFnt( 0 );

        // set position for underline font
        rInf.SetPos( pUnderFnt->GetPos() );

        pUnderFnt->GetFont()._DrawStretchText( rInf );

        rInf.SetUnderFnt( pUnderFnt );
        rInf.SetText( rOldStr );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }

    rInf.SetPos( rOld );
}

void SwSubFont::_DrawStretchText( SwDrawTextInfo &rInf )
{
    if( !rInf.GetLen() || !rInf.GetText().Len() )
        return;

    FontUnderline nOldUnder = UNDERLINE_NONE;
    SwUnderlineFont* pUnderFnt = 0;

    if( rInf.GetUnderFnt() )
    {
        nOldUnder = GetUnderline();
        SetUnderline( UNDERLINE_NONE );
        pUnderFnt = rInf.GetUnderFnt();
    }

    if ( !pLastFont || pLastFont->GetOwner() != pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetOut() );

    SwDigitModeModifier aDigitModeModifier( rInf.GetOut(), rInf.GetFont()->GetLanguage() );

    rInf.ApplyAutoColor();

    Point aPos( rInf.GetPos() );

    if( GetEscapement() )
        CalcEsc( rInf, aPos );

    rInf.SetKern( CheckKerning() + rInf.GetSperren() / SPACING_PRECISION_FACTOR );
    const Point &rOld = rInf.GetPos();
    rInf.SetPos( aPos );

    if( IsCapital() )
        DrawStretchCapital( rInf );
    else
    {
        SV_STAT( nDrawStretchText );

        if ( rInf.GetFrm() )
        {
            if ( rInf.GetFrm()->IsRightToLeft() )
                rInf.GetFrm()->SwitchLTRtoRTL( aPos );

            if ( rInf.GetFrm()->IsVertical() )
                rInf.GetFrm()->SwitchHorizontalToVertical( aPos );
        }

        if ( !IsCaseMap() )
            rInf.GetOut().DrawStretchText( aPos, rInf.GetWidth(),
                            rInf.GetText(), rInf.GetIdx(), rInf.GetLen() );
        else
            rInf.GetOut().DrawStretchText( aPos, rInf.GetWidth(), CalcCaseMap(
                            rInf.GetText() ), rInf.GetIdx(), rInf.GetLen() );
    }

    if( pUnderFnt && nOldUnder != UNDERLINE_NONE )
    {
static sal_Char __READONLY_DATA sDoubleSpace[] = "  ";
        const XubString &rOldStr = rInf.GetText();
        XubString aStr( sDoubleSpace, RTL_TEXTENCODING_MS_1252 );
        xub_StrLen nOldIdx = rInf.GetIdx();
        xub_StrLen nOldLen = rInf.GetLen();
        rInf.SetText( aStr );
        rInf.SetIdx( 0 );
        rInf.SetLen( 2 );
        SetUnderline( nOldUnder );
        rInf.SetUnderFnt( 0 );

        // set position for underline font
        rInf.SetPos( pUnderFnt->GetPos() );

        pUnderFnt->GetFont()._DrawStretchText( rInf );

        rInf.SetUnderFnt( pUnderFnt );
        rInf.SetText( rOldStr );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }

    rInf.SetPos( rOld );
}

/*************************************************************************
 *					  SwSubFont::_GetCrsrOfst()
 *************************************************************************/

xub_StrLen SwSubFont::_GetCrsrOfst( SwDrawTextInfo& rInf )
{
    if ( !pLastFont || pLastFont->GetOwner()!=pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetOut() );

    SwDigitModeModifier aDigitModeModifier( rInf.GetOut(), rInf.GetFont()->GetLanguage() );

    xub_StrLen nLn = rInf.GetLen() == STRING_LEN ? rInf.GetText().Len()
                                                 : rInf.GetLen();
    rInf.SetLen( nLn );
    xub_StrLen nCrsr = 0;
    if( IsCapital() && nLn )
        nCrsr = GetCapitalCrsrOfst( rInf );
    else
    {
        const XubString &rOldTxt = rInf.GetText();
        long nOldKern = rInf.GetKern();
        rInf.SetKern( CheckKerning() );
        SV_STAT( nGetTextSize );
        if ( !IsCaseMap() )
            nCrsr = pLastFont->GetCrsrOfst( rInf );
        else
        {
            String aTmp = CalcCaseMap( rInf.GetText() );
            rInf.SetText( aTmp );
            nCrsr = pLastFont->GetCrsrOfst( rInf );
        }
        rInf.SetKern( nOldKern );
        rInf.SetText( rOldTxt );
    }
    return nCrsr;
}

/*************************************************************************
 *                    SwSubFont::CalcEsc()
 *************************************************************************/

void SwSubFont::CalcEsc( SwDrawTextInfo& rInf, Point& rPos )
{
    long nOfst;

    USHORT nDir = UnMapDirection(
                GetOrientation(), rInf.GetFrm() && rInf.GetFrm()->IsVertical() );

    switch ( GetEscapement() )
    {
    case DFLT_ESC_AUTO_SUB :
        nOfst = nOrgHeight - nOrgAscent -
            pLastFont->GetFontHeight( rInf.GetShell(), rInf.GetOut() ) +
            pLastFont->GetFontAscent( rInf.GetShell(), rInf.GetOut() );

        switch ( nDir )
        {
        case 0 :
            rPos.Y() += nOfst;
            break;
        case 900 :
            rPos.X() += nOfst;
            break;
        case 2700 :
            rPos.X() -= nOfst;
            break;
        }

        break;
    case DFLT_ESC_AUTO_SUPER :
        nOfst = pLastFont->GetFontAscent( rInf.GetShell(), rInf.GetOut() ) -
                nOrgAscent;


        switch ( nDir )
        {
        case 0 :
            rPos.Y() += nOfst;
            break;
        case 900 :
            rPos.X() += nOfst;
            break;
        case 2700 :
            rPos.X() -= nOfst;
            break;
        }

        break;
    default :
        nOfst = ((long)nOrgHeight * GetEscapement()) / 100L;

        switch ( nDir )
        {
        case 0 :
            rPos.Y() -= nOfst;
            break;
        case 900 :
            rPos.X() -= nOfst;
            break;
        case 2700 :
            rPos.X() += nOfst;
            break;
        }
    }
}

// used during painting of small capitals
void SwDrawTextInfo::Shift( USHORT nDir )
{
    ASSERT( bPos, "DrawTextInfo: Undefined Position" );
    ASSERT( bSize, "DrawTextInfo: Undefined Width" );

    const BOOL bBidiPor = ( GetFrm() && GetFrm()->IsRightToLeft() ) !=
                          ( 0 != ( TEXT_LAYOUT_BIDI_RTL & GetpOut()->GetLayoutMode() ) );

    nDir = bBidiPor ?
            1800 :
            UnMapDirection( nDir, GetFrm() && GetFrm()->IsVertical() );

    switch ( nDir )
    {
    case 0 :
        ((Point*)pPos)->X() += GetSize().Width();
        break;
    case 900 :
        ASSERT( ((Point*)pPos)->Y() >= GetSize().Width(), "Going underground" );
        ((Point*)pPos)->Y() -= GetSize().Width();
        break;
    case 1800 :
        ((Point*)pPos)->X() -= GetSize().Width();
        break;
    case 2700 :
        ((Point*)pPos)->Y() += GetSize().Width();
        break;
    }
}

/*************************************************************************
 *                      SwUnderlineFont::~SwUnderlineFont
 *
 * Used for the "continuous underline" feature.
 *************************************************************************/

SwUnderlineFont::SwUnderlineFont( SwFont& rFnt, const Point& rPoint )
        : aPos( rPoint ), pFnt( &rFnt )
{
};

SwUnderlineFont::~SwUnderlineFont()
{
     delete pFnt;
}

//Helper for filters to find true lineheight of a font
long AttrSetToLineHeight( const IDocumentSettingAccess& rIDocumentSettingAccess,
                          const SwAttrSet &rSet,
                          const OutputDevice &rOut, sal_Int16 nScript)
{
    SwFont aFont(&rSet, &rIDocumentSettingAccess);
    BYTE nActual;
    switch (nScript)
    {
        default:
        case i18n::ScriptType::LATIN:
            nActual = SW_LATIN;
            break;
        case i18n::ScriptType::ASIAN:
            nActual = SW_CJK;
            break;
        case i18n::ScriptType::COMPLEX:
            nActual = SW_CTL;
            break;
    }
    aFont.SetActual(nActual);

    OutputDevice &rMutableOut = const_cast<OutputDevice &>(rOut);
    const Font aOldFont(rMutableOut.GetFont());

    rMutableOut.SetFont(aFont.GetActualFont());
    long nHeight = rMutableOut.GetTextHeight();

    rMutableOut.SetFont(aOldFont);
    return nHeight;
}
