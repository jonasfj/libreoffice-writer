/*************************************************************************
 *
 *  $RCSfile: swfont.cxx,v $
 *
 *  $Revision: 1.34 $
 *
 *  last change: $Author: fme $ $Date: 2002-10-22 07:00:48 $
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
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifdef VERTICAL_LAYOUT
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#endif
#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_WRLMITEM_HXX //autogen
#include <svx/wrlmitem.hxx>
#endif
#ifndef _SVX_BLNKITEM_HXX //autogen
#include <svx/blnkitem.hxx>
#endif
#ifndef _SVX_NHYPITEM_HXX //autogen
#include <svx/nhypitem.hxx>
#endif
#ifndef _SVX_KERNITEM_HXX //autogen
#include <svx/kernitem.hxx>
#endif
#ifndef _SVX_CMAPITEM_HXX //autogen
#include <svx/cmapitem.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX //autogen
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_ESCPITEM_HXX //autogen
#include <svx/escpitem.hxx>
#endif
#ifndef _SVX_AKRNITEM_HXX //autogen
#include <svx/akrnitem.hxx>
#endif
#ifndef _SVX_SHDDITEM_HXX //autogen
#include <svx/shdditem.hxx>
#endif
#ifndef _SVX_CHARRELIEFITEM_HXX
#include <svx/charreliefitem.hxx>
#endif
#ifndef _SVX_CNTRITEM_HXX //autogen
#include <svx/cntritem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_CSCOITEM_HXX //autogen
#include <svx/cscoitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX //autogen
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX //autogen
#include <svx/postitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_EMPHITEM_HXX //autogen
#include <svx/emphitem.hxx>
#endif
#ifndef _SVX_CHARSCALEITEM_HXX
#include <svx/charscaleitem.hxx>
#endif
#ifndef _SVX_CHARROTATEITEM_HXX
#include <svx/charrotateitem.hxx>
#endif
#ifndef _SVX_TWOLINESITEM_HXX
#include <svx/twolinesitem.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#ifndef _CHARATR_HXX
#include <charatr.hxx>
#endif
#ifndef _SWATRSET_HXX //autogen
#include <swatrset.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>		// Bildschirmabgleich
#endif
#ifndef _SWFONT_HXX
#include <swfont.hxx>
#endif
#ifndef _FNTCACHE_HXX
#include <fntcache.hxx>     // FontCache
#endif
#ifndef _DRAWFONT_HXX
#include <drawfont.hxx>		// SwDrawTextInfo
#endif
#ifdef VERTICAL_LAYOUT
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>       // SwTxtFrm
#endif
#endif

#if defined(WIN) || defined(WNT) || defined(PM2)
#define FNT_LEADING_HACK
#endif

#if defined(WIN) || defined(WNT)
#define FNT_ATM_HACK
#endif

#ifndef PRODUCT
// globale Variable
SvStatistics aSvStat;
#endif

#ifdef VERTICAL_LAYOUT
using namespace ::com::sun::star::i18n::ScriptType;
#endif

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

/************************************************************************
 * Hintergrundbrush setzen,
 * die alte Brush wird _nicht_ destruiert, sondern ist der Rueckgabewert.
 ***********************************************************************/

Color* SwFont::XChgBackColor( Color* pNewColor )
{
    Color* pRet = pBackColor;
    pBackColor = pNewColor;
    bFntChg = TRUE;
    aSub[SW_LATIN].pMagic = aSub[SW_CJK].pMagic = aSub[SW_CTL].pMagic = 0;
    return pRet;
}

#ifdef VERTICAL_LAYOUT

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
#ifdef DEBUG
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
#ifdef DEBUG
        default :
            ASSERT( sal_False, "Unsupported direction" );
            break;
#endif
        }
    }
    return nDir;
}

void SwFont::SetVertical( USHORT nDir, const BOOL bVertFormat )
{
    // map direction if frame has vertical layout
    nDir = MapDirection( nDir, bVertFormat );

    if( nDir != aSub[0].GetOrientation() )
    {
        bFntChg = TRUE;
        aSub[0].SetVertical( nDir );
        aSub[1].SetVertical( nDir );
        aSub[2].SetVertical( nDir );
    }
}
#endif

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

void SwFont::SetDiffFnt( const SfxItemSet *pAttrSet, const SwDoc *pDoc )
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
            SetUnderline( ((SvxUnderlineItem*)pItem)->GetUnderline() );
            SetUnderColor( ((SvxUnderlineItem*)pItem)->GetColor() );
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
                SetAutoKern( ( !pDoc || !pDoc->IsKernAsianPunctuation() ) ?
                         KERNING_FONTSPECIFIC : KERNING_ASIAN );
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
    nToxCnt = nRefCnt = 0;
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

SwFont::SwFont( const SwAttrSet* pAttrSet, const SwDoc *pDoc )
{
    nActual = SW_LATIN;
    nToxCnt = nRefCnt = 0;
    bPaintBlank = FALSE;
    bPaintWrong = FALSE;
    bURL = FALSE;
    bGreyWave = FALSE;
    bNoColReplace = FALSE;
    bNoHyph = pAttrSet->GetNoHyphenHere().GetValue();
    bBlink = pAttrSet->GetBlink().GetValue();
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

    SetUnderline( pAttrSet->GetUnderline().GetUnderline() );
    SetUnderColor( pAttrSet->GetUnderline().GetColor() );
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
        SetAutoKern( ( !pDoc || !pDoc->IsKernAsianPunctuation() ) ?
                     KERNING_FONTSPECIFIC : KERNING_ASIAN );
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
    nToxCnt = nRefCnt = 0;
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

BOOL SwSubFont::ChgFnt( ViewShell *pSh, OutputDevice *pOut )
{
    if ( pLastFont )
        pLastFont->Unlock();
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh, TRUE );
    SV_STAT( nChangeFont );

    pLastFont = aFntAccess.Get();

    pLastFont->SetDevFont( pSh, pOut );

    pLastFont->Lock();
    return UNDERLINE_NONE != GetUnderline() || STRIKEOUT_NONE != GetStrikeout();
}

/*************************************************************************
 *					  SwFont::ChgPhysFnt()
 *************************************************************************/

void SwFont::ChgPhysFnt( ViewShell *pSh, OutputDevice *pOut )
{
    ASSERT( pOut, "SwFont:;ChgPhysFnt, not OutDev." );

    if( bOrgChg && aSub[nActual].IsEsc() )
    {
        const BYTE nOldProp = aSub[nActual].GetPropr();
        SetProportion( 100 );
        ChgFnt( pSh, pOut );
        SwFntAccess aFntAccess( aSub[nActual].pMagic, aSub[nActual].nFntIndex,
                                &aSub[nActual], pSh );
        aSub[nActual].nOrgHeight = aFntAccess.Get()->GetHeight( pSh, pOut );
        aSub[nActual].nOrgAscent = aFntAccess.Get()->GetAscent( pSh, pOut );
        SetProportion( nOldProp );
        bOrgChg = FALSE;
    }

    if( bFntChg )
    {
        ChgFnt( pSh, pOut );
        bFntChg = bOrgChg;
    }
    if( pOut->GetTextLineColor() != aUnderColor )
        pOut->SetTextLineColor( aUnderColor );
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
#ifdef DEBUG
    static nTst = 6;
    short nKernx = - short( Font::GetSize().Height() / nTst );
#else
    short nKernx = - short( Font::GetSize().Height() / 6 );
#endif
    if ( nKernx < GetFixKerning() )
        return GetFixKerning();
    return nKernx;
}

/*************************************************************************
 *                    SwFont::GetLeading()
 *************************************************************************/

USHORT SwFont::GetLeading( ViewShell *pSh, const OutputDevice *pOut )
{
    if( OUTDEV_PRINTER != pOut->GetOutDevType() )
        return 0;
    else
    {
        SwFntAccess aFntAccess( aSub[nActual].pMagic, aSub[nActual].nFntIndex,
                                &aSub[nActual], pSh );
        return aFntAccess.Get()->GetLeading();
    }
}

/*************************************************************************
 *                    SwSubFont::GetAscent()
 *************************************************************************/

USHORT SwSubFont::GetAscent( ViewShell *pSh, const OutputDevice *pOut )
{
    register USHORT nAscent;
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh );
    nAscent = aFntAccess.Get()->GetAscent( pSh, pOut );
    if( GetEscapement() )
        nAscent = CalcEscAscent( nAscent );
    return nAscent;
}

/*************************************************************************
 *					  SwSubFont::GetHeight()
 *************************************************************************/

USHORT SwSubFont::GetHeight( ViewShell *pSh, const OutputDevice *pOut )
{
    SV_STAT( nGetTextSize );
    SwFntAccess aFntAccess( pMagic, nFntIndex, this, pSh );
    const USHORT nHeight = aFntAccess.Get()->GetHeight( pSh, pOut );
    if ( GetEscapement() )
    {
        const USHORT nAscent = aFntAccess.Get()->GetAscent( pSh, pOut );
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
        ChgFnt( rInf.GetShell(), rInf.GetpOut() );

    Size aTxtSize;
    xub_StrLen nLn = ( rInf.GetLen() == STRING_LEN ? rInf.GetText().Len()
                                                   : rInf.GetLen() );
    rInf.SetLen( nLn );
    if( IsCapital() && nLn )
        aTxtSize = GetCapitalSize( rInf );
    else
    {
        SV_STAT( nGetTextSize );
        short nOldKern = rInf.GetKern();
        const XubString &rOldTxt = rInf.GetText();
        rInf.SetKern( CheckKerning() );
        if ( !IsCaseMap() )
            aTxtSize = pLastFont->GetTextSize( rInf );
        else
        {
            String aTmp = CalcCaseMap( rInf.GetText() );
            rInf.SetText( aTmp );
            aTxtSize = pLastFont->GetTextSize( rInf );
        }
        rInf.SetKern( nOldKern );
        rInf.SetText( rOldTxt );
        // 15142: Ein Wort laenger als eine Zeile, beim Zeilenumbruch
        //        hochgestellt, muss seine effektive Hoehe melden.
        if( GetEscapement() )
        {
            const USHORT nAscent = pLastFont->GetAscent( rInf.GetShell(),
                                                         rInf.GetpOut() );
            aTxtSize.Height() =
                (long)CalcEscHeight( (USHORT)aTxtSize.Height(), nAscent);
        }
    }

    return aTxtSize;
}

/*************************************************************************
 *					  SwFont::GetTxtBreak()
 *************************************************************************/

xub_StrLen SwFont::GetTxtBreak( ViewShell *pSh,	const OutputDevice *pOut,
    const SwScriptInfo* pScript, const XubString &rTxt, long nTextWidth,
    const xub_StrLen nIdx, const xub_StrLen nLen )
{
     ChgFnt( pSh, (OutputDevice *)pOut );

    USHORT nTxtBreak = 0;

    USHORT nLn = ( nLen == STRING_LEN ? rTxt.Len() : nLen );
    if( aSub[nActual].IsCapital() && nLn )
        nTxtBreak = GetCapitalBreak( pSh, pOut, pScript, rTxt, nTextWidth,
                                     0, nIdx, nLn );
    else
    {
        if ( !aSub[nActual].IsCaseMap() )
            nTxtBreak = pOut->GetTextBreak( rTxt, nTextWidth,
                                               nIdx, nLn, CheckKerning() );
        else
            nTxtBreak = pOut->GetTextBreak( aSub[nActual].CalcCaseMap( rTxt ),
                        nTextWidth, nIdx, nLn, CheckKerning() );
    }
    return nTxtBreak;
}

/*************************************************************************
 *					  SwFont::GetTxtBreak()
 *************************************************************************/

xub_StrLen SwFont::GetTxtBreak( ViewShell *pSh, const OutputDevice *pOut,
   const SwScriptInfo* pScript, const XubString &rTxt, long nTextWidth,
   xub_StrLen& rExtraCharPos, const xub_StrLen nIdx, const xub_StrLen nLen )
{
    // Robust ...
    if ( !pLastFont || pLastFont->GetOwner()!= aSub[nActual].pMagic )
        ChgFnt( pSh, (OutputDevice *)pOut );

    xub_StrLen nTxtBreak = 0;

    xub_StrLen nLn = ( nLen == STRING_LEN ? rTxt.Len() : nLen );
    if( aSub[nActual].IsCapital() && nLn )
            nTxtBreak = GetCapitalBreak( pSh, pOut, pScript, rTxt, nTextWidth,
                                &rExtraCharPos, nIdx, nLn );
    else
    {
        if ( !aSub[nActual].IsCaseMap() )
            nTxtBreak = pOut->GetTextBreak( rTxt, nTextWidth,
                            '-', rExtraCharPos, nIdx, nLn, CheckKerning() );
        else
            nTxtBreak = pOut->GetTextBreak( aSub[nActual].CalcCaseMap( rTxt ),
                nTextWidth,	'-', rExtraCharPos, nIdx, nLn, CheckKerning() );
    }
    return nTxtBreak;
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

    FontUnderline nOldUnder;
    SwFont* pUnderFnt = 0;

    if( rInf.GetUnderFnt() )
    {
        nOldUnder = GetUnderline();
        SetUnderline( UNDERLINE_NONE );
        pUnderFnt = rInf.GetUnderFnt();
    }

    if( !pLastFont || pLastFont->GetOwner()!=pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetpOut() );

    Point aPos( rInf.GetPos() );
    const Point &rOld = rInf.GetPos();
    rInf.SetPos( aPos );

    if( GetEscapement() )
        CalcEsc( rInf, aPos );

    rInf.SetKern( CheckKerning() + rInf.GetSperren() );

    if( IsCapital() )
        DrawCapital( rInf );
    else
    {
        SV_STAT( nDrawText );
        if ( !IsCaseMap() )
            pLastFont->DrawText( rInf );
        else
        {
            XubString aString( CalcCaseMap( rInf.GetText() ) );
            const XubString &rOldStr = rInf.GetText();
            rInf.SetText( aString );
            pLastFont->DrawText( rInf );
            rInf.SetText( rOldStr );
        }
    }
    rInf.SetPos( rOld );

    if( rInf.GetUnderFnt() && nOldUnder != UNDERLINE_NONE )
    {
static sal_Char __READONLY_DATA sDoubleSpace[] = "  ";
        Size aSize = _GetTxtSize( rInf );
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

#ifdef VERTICAL_LAYOUT
            const SwScriptInfo* pSI = rInf.GetScriptInfo();

            const sal_Bool bAsianFont =
                ( rInf.GetFont() && SW_CJK == rInf.GetFont()->GetActual() );
            for( xub_StrLen nTmp = nOldIdx; nTmp < nTmpEnd; ++nTmp )
                if( CH_BLANK == rOldStr.GetChar( nTmp ) || bAsianFont ||
                    ( nTmp + 1 < rOldStr.Len() && pSI &&
                      ASIAN == pSI->ScriptType( nTmp + 1 ) ) )

            // if next portion if a hole portion we do not consider any
            // extra space added because the last character was ASIAN
            if ( nSpace && rInf.IsSpaceStop() && bAsianFont )
                 --nSpace;

#else
            for( xub_StrLen nTmp = nOldIdx; nTmp < nTmpEnd; ++nTmp )
                if( CH_BLANK == rOldStr.GetChar( nTmp ) )
#endif
                    ++nSpace;

            nSpace *= rInf.GetSpace();
        }
        rInf.SetText( aStr );
        rInf.SetIdx( 0 );
        rInf.SetLen( 2 );
        SetUnderline( nOldUnder );
        rInf.SetWidth( USHORT(aSize.Width() + nSpace) );
        rInf.SetUnderFnt( 0 );

        pUnderFnt->_DrawStretchText( rInf );

        rInf.SetUnderFnt( pUnderFnt );
        rInf.SetText( rOldStr );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }
}

void SwSubFont::_DrawStretchText( SwDrawTextInfo &rInf )
{
    if( !rInf.GetLen() || !rInf.GetText().Len() )
        return;

    FontUnderline nOldUnder;
    SwFont* pUnderFnt = 0;

    if( rInf.GetUnderFnt() )
    {
        nOldUnder = GetUnderline();
        SetUnderline( UNDERLINE_NONE );
        pUnderFnt = rInf.GetUnderFnt();
    }

    if ( !pLastFont || pLastFont->GetOwner() != pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetpOut() );

    rInf.ApplyAutoColor();

    Point aPos( rInf.GetPos() );

    if( GetEscapement() )
        CalcEsc( rInf, aPos );

    rInf.SetKern( CheckKerning() + rInf.GetSperren() );

    if( IsCapital() )
    {
        const Point &rOld = rInf.GetPos();
        rInf.SetPos( aPos );
        DrawStretchCapital( rInf );
        rInf.SetPos( rOld );
    }
    else
    {
        SV_STAT( nDrawStretchText );

#ifdef VERTICAL_LAYOUT
        const Point &rOld = rInf.GetPos();
        rInf.SetPos( aPos );

        if ( rInf.GetFrm() )
        {
#ifdef BIDI
            if ( rInf.GetFrm()->IsRightToLeft() )
                rInf.GetFrm()->SwitchLTRtoRTL( aPos );
#endif

            if ( rInf.GetFrm()->IsVertical() )
                rInf.GetFrm()->SwitchHorizontalToVertical( aPos );
        }
#endif

        if ( !IsCaseMap() )
            rInf.GetOut().DrawStretchText( aPos, rInf.GetWidth(),
                            rInf.GetText(), rInf.GetIdx(), rInf.GetLen() );
        else
            rInf.GetOut().DrawStretchText( aPos, rInf.GetWidth(), CalcCaseMap(
                            rInf.GetText() ), rInf.GetIdx(), rInf.GetLen() );

#ifdef VERTICAL_LAYOUT
        rInf.SetPos( rOld );
#endif
    }

    if( rInf.GetUnderFnt() && nOldUnder != UNDERLINE_NONE )
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

        pUnderFnt->_DrawStretchText( rInf );

        rInf.SetUnderFnt( pUnderFnt );
        rInf.SetText( rOldStr );
        rInf.SetIdx( nOldIdx );
        rInf.SetLen( nOldLen );
    }
}

/*************************************************************************
 *					  SwSubFont::_GetCrsrOfst()
 *************************************************************************/

xub_StrLen SwSubFont::_GetCrsrOfst( SwDrawTextInfo& rInf )
{
    if ( !pLastFont || pLastFont->GetOwner()!=pMagic )
        ChgFnt( rInf.GetShell(), rInf.GetpOut() );

    xub_StrLen nLn = rInf.GetLen() == STRING_LEN ? rInf.GetText().Len()
                                                 : rInf.GetLen();
    rInf.SetLen( nLn );
    xub_StrLen nCrsr = 0;
    if( IsCapital() && nLn )
        nCrsr = GetCapitalCrsrOfst( rInf );
    else
    {
        const XubString &rOldTxt = rInf.GetText();
        short nOldKern = rInf.GetKern();
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

#ifdef VERTICAL_LAYOUT
    USHORT nDir = UnMapDirection(
                GetOrientation(), rInf.GetFrm() && rInf.GetFrm()->IsVertical() );
#endif

    switch ( GetEscapement() )
    {
    case DFLT_ESC_AUTO_SUB :
        nOfst = nOrgHeight - nOrgAscent -
            pLastFont->GetHeight( rInf.GetShell(), rInf.GetpOut() ) +
            pLastFont->GetAscent( rInf.GetShell(), rInf.GetpOut() );

#ifdef VERTICAL_LAYOUT
        switch ( nDir )
#else
        switch ( GetOrientation() )
#endif
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
        nOfst = pLastFont->GetAscent( rInf.GetShell(), rInf.GetpOut() ) -
                nOrgAscent;


#ifdef VERTICAL_LAYOUT
        switch ( nDir )
#else
        switch ( GetOrientation() )
#endif
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

#ifdef VERTICAL_LAYOUT
        switch ( nDir )
#else
        switch ( GetOrientation() )
#endif
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

