/*************************************************************************
 *
 *  $RCSfile: txatbase.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 15:24:31 $
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
#ifndef _TXATBASE_HXX
#define _TXATBASE_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <svtools/poolitem.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

class SwFont;
class SfxItemPool;
class SvxBrushItem;
class SvxFontItem;
class SvxPostureItem;
class SvxWeightItem;
class SvxUnderlineItem;
class SvxFontHeightItem;
class SvxPropSizeItem;
class SvxShadowedItem;
class SvxAutoKernItem;
class SvxWordLineModeItem;
class SvxContourItem;
class SvxCrossedOutItem;
class SvxColorItem;
class SvxCharSetColorItem;
class SvXMLAttrContainerItem;
class SwFmtRuby;
class SvxTwoLinesItem;
class SvxEmphasisMarkItem;
class SvxCharScaleWidthItem;
class SvxCharRotateItem;
class SvxCharReliefItem;
class SvxCharHiddenItem;

class SwFmtCharFmt;
class SwFmtINetFmt;
class SvxKerningItem;
class SvxCaseMapItem;
class SvxLanguageItem;
class SvxEscapementItem;
class SvxBlinkItem;
class SvxNoHyphenItem;
class SwFmtSoftHyph;
class SwFmtHardBlank;
class SwFmtFld;
class SwFmtFtn;
class SwFmtFlyCnt;
class SwTOXMark;
class SwFmtRefMark;

class SwTxtAttr
{
    const SfxPoolItem* pAttr;
    xub_StrLen nStart;
    BOOL bDontExpand : 1;
    BOOL bLockExpandFlag : 1;

    BOOL bDontMergeAttr : 1;		    // refmarks, toxmarks, ruby
    BOOL bDontMoveAttr : 1;				// refmarks, toxmarks
    BOOL bCharFmtAttr : 1;				// charfmt, inet
    BOOL bOverlapAllowedAttr : 1;		// refmarks, toxmarks
    BOOL bPriorityAttr : 1;             // attribute has priority (redlining)
    BOOL bDontExpandStart : 1;			// don't expand start at paragraph start (ruby)
protected:
    SwTxtAttr( const SfxPoolItem& rAttr, xub_StrLen nStart );

    void SetLockExpandFlag( BOOL bFlag )	{ bLockExpandFlag = bFlag; }
    void SetDontMergeAttr( BOOL bFlag )		{ bDontMergeAttr = bFlag; }
    void SetDontMoveAttr( BOOL bFlag )		{ bDontMoveAttr = bFlag; }
    void SetCharFmtAttr( BOOL bFlag )		{ bCharFmtAttr = bFlag; }
    void SetOverlapAllowedAttr( BOOL bFlag ){ bOverlapAllowedAttr = bFlag; }
    void SetDontExpandStartAttr(BOOL bFlag)	{ bDontExpandStart = bFlag; }

public:
    virtual ~SwTxtAttr();

    // RemoveFromPool muss immer vorm DTOR Aufruf erfolgen!!
    void RemoveFromPool( SfxItemPool& rPool );

    // Start Position erfragen
                  xub_StrLen* GetStart() 		{ return &nStart; }
            const xub_StrLen* GetStart() const	{ return &nStart; }

    // End Position erfragen
    virtual 	 xub_StrLen* GetEnd();
    inline const xub_StrLen* GetEnd() const;
    inline const xub_StrLen* GetAnyEnd() const;

    inline void SetDontExpand( BOOL bNew );
    BOOL DontExpand() const					{ return bDontExpand; }
    BOOL IsLockExpandFlag() const 			{ return bLockExpandFlag; }
    BOOL IsDontMergeAttr() const			{ return bDontMergeAttr; }
    BOOL IsDontMoveAttr() const				{ return bDontMoveAttr; }
    BOOL IsCharFmtAttr() const				{ return bCharFmtAttr; }
    BOOL IsOverlapAllowedAttr() const		{ return bOverlapAllowedAttr; }
    BOOL IsPriorityAttr() const             { return bPriorityAttr; }
    void SetPriorityAttr( BOOL bFlag )		{ bPriorityAttr = bFlag; }
    BOOL IsDontExpandStartAttr() const		{ return bDontExpandStart; }

    inline const SfxPoolItem& GetAttr() const;
    inline USHORT Which() const { return GetAttr().Which(); }

    virtual	int         operator==( const SwTxtAttr& ) const;

    inline const SvxFontItem		 	&GetFont() const;
    inline const SvxPostureItem			&GetPosture() const;
    inline const SvxWeightItem			&GetWeight() const;
    inline const SvxUnderlineItem 		&GetUnderline() const;
    inline const SvxFontHeightItem		&GetFontSize() const;
    inline const SvxPropSizeItem		&GetPropSize() const;
    inline const SvxShadowedItem		&GetShadowed() const;
    inline const SvxAutoKernItem		&GetAutoKern() const;
    inline const SvxWordLineModeItem	&GetWordLineMode() const;
    inline const SvxContourItem			&GetContour() const;
    inline const SvxCrossedOutItem		&GetCrossedOut() const;
    inline const SvxColorItem 			&GetColor() const;
    inline const SvxCharSetColorItem	&GetCharSetColor() const;
    inline const SwFmtCharFmt			&GetCharFmt() const;
    inline const SvxKerningItem			&GetKerning() const;
    inline const SvxCaseMapItem			&GetCaseMap() const;
    inline const SvxLanguageItem		&GetLanguage() const;
    inline const SvxEscapementItem		&GetEscapement() const;
    inline const SvxBlinkItem			&GetBlink() const;
    inline const SvxBrushItem			&GetChrBackground() const;
    inline const SvxNoHyphenItem		&GetNoHyphenHere() const;
    inline const SwFmtSoftHyph			&GetSoftHyph() const;
    inline const SwFmtHardBlank 		&GetHardBlank() const;
    inline const SwFmtFld				&GetFld() const;
    inline const SwFmtFtn				&GetFtn() const;
    inline const SwFmtFlyCnt			&GetFlyCnt() const;
    inline const SwTOXMark				&GetTOXMark() const;
    inline const SwFmtRefMark			&GetRefMark() const;
    inline const SwFmtINetFmt			&GetINetFmt() const;
    inline const SvXMLAttrContainerItem	&GetXMLAttrContainer() const;
    inline const SwFmtRuby				&GetRuby() const;
    inline const SvxTwoLinesItem		&Get2Lines() const;
    inline const SvxEmphasisMarkItem	&GetEmphasisMark() const;
    inline const SvxCharScaleWidthItem	&GetCharScaleW() const;
    inline const SvxCharRotateItem		&GetCharRotate() const;
    inline const SvxCharReliefItem		&GetCharRelief() const;
    inline const SvxCharHiddenItem      &GetCharHidden() const;

private:
    SwTxtAttr( const SwTxtAttr& );
    SwTxtAttr& operator=( const SwTxtAttr& );
};

class SwTxtAttrEnd : public SwTxtAttr
{
protected:
    xub_StrLen nEnd;

public:
    SwTxtAttrEnd( const SfxPoolItem& rAttr, USHORT nStart, USHORT nEnd );
    virtual xub_StrLen* GetEnd();
};


// --------------- Inline Implementierungen ------------------------

inline const xub_StrLen* SwTxtAttr::GetEnd() const
{
    return ((SwTxtAttr*)this)->GetEnd();
}

inline const xub_StrLen* SwTxtAttr::GetAnyEnd() const
{
    const xub_StrLen* pEnd = GetEnd();
    return pEnd ? pEnd : &nStart;
}

inline const SfxPoolItem& SwTxtAttr::GetAttr() const
{
    ASSERT( pAttr, "wo ist mein Attribut?" );
    return *pAttr;
}

inline void SwTxtAttr::SetDontExpand( BOOL bNew )
{
    if( !bLockExpandFlag )
        bDontExpand = bNew;
}

inline const SvxFontItem& SwTxtAttr::GetFont() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_FONT, "Falsche Abfrage" );
    return (const SvxFontItem&)*pAttr;
}

inline const SvxPostureItem& SwTxtAttr::GetPosture() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_POSTURE, "Falsche Abfrage" );
    return (const SvxPostureItem&)*pAttr;
}

inline const SvxWeightItem& SwTxtAttr::GetWeight() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_WEIGHT, "Falsche Abfrage" );
    return (const SvxWeightItem&)*pAttr;
}

inline const SvxUnderlineItem& SwTxtAttr::GetUnderline() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_UNDERLINE, "Falsche Abfrage" );
    return (const SvxUnderlineItem&)*pAttr;
}

inline const SvxFontHeightItem& SwTxtAttr::GetFontSize() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_FONTSIZE, "Falsche Abfrage" );
    return (const SvxFontHeightItem&)*pAttr;
}

inline const SvxPropSizeItem& SwTxtAttr::GetPropSize() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_PROPORTIONALFONTSIZE, "Falsche Abfrage" );
    return (const SvxPropSizeItem&)*pAttr;
}

inline const SvxShadowedItem& SwTxtAttr::GetShadowed() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_SHADOWED, "Falsche Abfrage" );
    return (const SvxShadowedItem&)*pAttr;
}

inline const SvxAutoKernItem& SwTxtAttr::GetAutoKern() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_AUTOKERN, "Falsche Abfrage" );
    return (const SvxAutoKernItem&)*pAttr;
}

inline const SvxWordLineModeItem& SwTxtAttr::GetWordLineMode() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_WORDLINEMODE, "Falsche Abfrage" );
    return (const SvxWordLineModeItem&)*pAttr;
}

inline const SvxContourItem& SwTxtAttr::GetContour() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_CONTOUR, "Falsche Abfrage" );
    return (const SvxContourItem&)*pAttr;
}

inline const SvxCrossedOutItem& SwTxtAttr::GetCrossedOut() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_CROSSEDOUT, "Falsche Abfrage" );
    return (const SvxCrossedOutItem&)*pAttr;
}

inline const SvxColorItem& SwTxtAttr::GetColor() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_COLOR, "Falsche Abfrage" );
    return (const SvxColorItem&)*pAttr;
}

inline const SvxCharSetColorItem& SwTxtAttr::GetCharSetColor() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_CHARSETCOLOR, "Falsche Abfrage" );
    return (const SvxCharSetColorItem&)*pAttr;
}

inline const SwFmtCharFmt& SwTxtAttr::GetCharFmt() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_CHARFMT, "Falsche Abfrage" );
    return (const SwFmtCharFmt&)*pAttr;
}

inline const SvxKerningItem& SwTxtAttr::GetKerning() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_KERNING, "Falsche Abfrage" );
    return (const SvxKerningItem&)*pAttr;
}

inline const SvxCaseMapItem& SwTxtAttr::GetCaseMap() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_CASEMAP, "Falsche Abfrage" );
    return (const SvxCaseMapItem&)*pAttr;
}

inline const SvxLanguageItem& SwTxtAttr::GetLanguage() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_LANGUAGE, "Falsche Abfrage" );
    return (const SvxLanguageItem&)*pAttr;
}

inline const SvxEscapementItem& SwTxtAttr::GetEscapement() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_ESCAPEMENT, "Falsche Abfrage" );
    return (const SvxEscapementItem&)*pAttr;
}

inline const SvxBlinkItem& SwTxtAttr::GetBlink() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_BLINK, "Falsche Abfrage" );
    return (const SvxBlinkItem&)*pAttr;
}

inline const SvxBrushItem& SwTxtAttr::GetChrBackground() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_BACKGROUND, "Falsche Abfrage" );
    return (const SvxBrushItem&)*pAttr;
}

inline const SvxNoHyphenItem& SwTxtAttr::GetNoHyphenHere() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_NOHYPHEN, "Falsche Abfrage" );
    return (const SvxNoHyphenItem&)*pAttr;
}

inline const SwFmtSoftHyph& SwTxtAttr::GetSoftHyph() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_SOFTHYPH, "Falsche Abfrage" );
    return (const SwFmtSoftHyph&)*pAttr;
}

inline const SwFmtHardBlank& SwTxtAttr::GetHardBlank() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_HARDBLANK, "Falsche Abfrage" );
    return (const SwFmtHardBlank&)*pAttr;
}

inline const SwFmtFld& SwTxtAttr::GetFld() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_FIELD, "Falsche Abfrage" );
    return (const SwFmtFld&)*pAttr;
}

inline const SwFmtFtn& SwTxtAttr::GetFtn() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_FTN, "Falsche Abfrage" );
    return (const SwFmtFtn&)*pAttr;
}

inline const SwFmtFlyCnt& SwTxtAttr::GetFlyCnt() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_FLYCNT, "Falsche Abfrage" );
    return (const SwFmtFlyCnt&)*pAttr;
}

inline const SwTOXMark& SwTxtAttr::GetTOXMark() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_TOXMARK, "Falsche Abfrage" );
    return (const SwTOXMark&)*pAttr;
}

inline const SwFmtRefMark& SwTxtAttr::GetRefMark() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_REFMARK, "Falsche Abfrage" );
    return (const SwFmtRefMark&)*pAttr;
}

inline const SwFmtINetFmt& SwTxtAttr::GetINetFmt() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_INETFMT, "Falsche Abfrage" );
    return (const SwFmtINetFmt&)*pAttr;
}

inline const SvXMLAttrContainerItem& SwTxtAttr::GetXMLAttrContainer() const
{
    ASSERT( pAttr && pAttr->Which() == RES_UNKNOWNATR_CONTAINER,
            "Falsche Abfrage" );
    return (const SvXMLAttrContainerItem&)*pAttr;
}

inline const SwFmtRuby& SwTxtAttr::GetRuby() const
{
    ASSERT( pAttr && pAttr->Which() == RES_TXTATR_CJK_RUBY,
            "Falsche Abfrage" );
    return (const SwFmtRuby&)*pAttr;
}
inline const SvxTwoLinesItem& SwTxtAttr::Get2Lines() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_TWO_LINES,
            "Falsche Abfrage" );
    return (const SvxTwoLinesItem&)*pAttr;
}

inline const SvxEmphasisMarkItem& SwTxtAttr::GetEmphasisMark() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_EMPHASIS_MARK, "Falsche Abfrage" );
    return (const SvxEmphasisMarkItem&)*pAttr;
}

inline const SvxCharScaleWidthItem&	SwTxtAttr::GetCharScaleW() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_SCALEW, "Falsche Abfrage" );
    return (const SvxCharScaleWidthItem&)*pAttr;
}

inline const SvxCharRotateItem&	SwTxtAttr::GetCharRotate() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_ROTATE, "Falsche Abfrage" );
    return (const SvxCharRotateItem&)*pAttr;
}

inline const SvxCharReliefItem&	SwTxtAttr::GetCharRelief() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_RELIEF, "Falsche Abfrage" );
    return (const SvxCharReliefItem&)*pAttr;
}

inline const SvxCharHiddenItem& SwTxtAttr::GetCharHidden() const
{
    ASSERT( pAttr && pAttr->Which() == RES_CHRATR_HIDDEN, "Falsche Abfrage" );
    return (const SvxCharHiddenItem&)*pAttr;
}

    // #ifndef _TXATBASE_HXX
#endif
