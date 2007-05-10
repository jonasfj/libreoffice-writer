/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tblafmt.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 15:53:53 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef _TBLAFMT_HXX
#define _TBLAFMT_HXX
/*************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

JP 20.07.95:

    Die akt. Struktur der Autoformatierung darf nicht mehr veraendert werden.
    Diese wird durch unterschiedlichen Code vom StartWriter und vom StarCalc
    eingelesen/geschrieben.
    Sollte sich doch mal eine Aenderung nicht vermeiden lassen, dann auf
    jedenfall in beiden Applikationen aendern.

    The structure of table auto formatting should not changed. It is used
    by different code of Writer and Calc. If a change is necessary, the
    source code of both applications must be changed!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**************************************************************************/

#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

#ifndef _HINTIDS_HXX
#include "hintids.hxx"			//_immmer_ vor den solar-items!
#endif

#ifndef _SVX_ALGITEM_HXX //autogen
#include <svx/algitem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX //autogen
#include <svx/postitem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX //autogen
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_CNTRTITEM_HXX //autogen
#include <svx/cntritem.hxx>
#endif
#ifndef _SVX_SHDDITEM_HXX //autogen
#include <svx/shdditem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_ADJITEM_HXX //autogen
#include <svx/adjitem.hxx>
#endif
#ifndef _SVX_ROTMODIT_HXX //autogen
#include <svx/rotmodit.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SVX_BOLNITEM_HXX
#include <svx/bolnitem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

struct SwAfVersions;

class SvNumberFormatter;

class SwBoxAutoFmt
{
    // common attributes of Calc and Writer
    // --- from 641 on: CJK and CTL font settings
    SvxFontItem         aFont;
    SvxFontHeightItem   aHeight;
    SvxWeightItem       aWeight;
    SvxPostureItem      aPosture;

    SvxFontItem         aCJKFont;
    SvxFontHeightItem   aCJKHeight;
    SvxWeightItem       aCJKWeight;
    SvxPostureItem      aCJKPosture;

    SvxFontItem         aCTLFont;
    SvxFontHeightItem   aCTLHeight;
    SvxWeightItem       aCTLWeight;
    SvxPostureItem      aCTLPosture;

    SvxUnderlineItem	aUnderline;
    SvxCrossedOutItem	aCrossedOut;
    SvxContourItem		aContour;
    SvxShadowedItem		aShadowed;
    SvxColorItem		aColor;
    SvxBoxItem			aBox;
    SvxLineItem         aTLBR;
    SvxLineItem         aBLTR;
    SvxBrushItem		aBackground;

    // Writer specific
    SvxAdjustItem		aAdjust;

    // Calc specific
    SvxHorJustifyItem	aHorJustify;
    SvxVerJustifyItem	aVerJustify;
    SfxBoolItem         aStacked;
    SvxMarginItem		aMargin;
    SfxBoolItem			aLinebreak;
    SfxInt32Item		aRotateAngle;
    SvxRotateModeItem	aRotateMode;

    // number format
    String				sNumFmtString;
    LanguageType		eSysLanguage, eNumFmtLanguage;

public:
    SwBoxAutoFmt();
    SwBoxAutoFmt( const SwBoxAutoFmt& rNew );
    ~SwBoxAutoFmt();

    int operator==( const SwBoxAutoFmt& rCmp ) const;
    SwBoxAutoFmt& operator=( const SwBoxAutoFmt& rNew );

    // die Get-Methoden
    const SvxFontItem		&GetFont() const		{ return aFont; }
    const SvxFontHeightItem	&GetHeight() const 		{ return aHeight; }
    const SvxWeightItem		&GetWeight() const 		{ return aWeight; }
    const SvxPostureItem	&GetPosture() const 	{ return aPosture; }
    const SvxFontItem       &GetCJKFont() const     { return aCJKFont; }
    const SvxFontHeightItem &GetCJKHeight() const   { return aCJKHeight; }
    const SvxWeightItem     &GetCJKWeight() const   { return aCJKWeight; }
    const SvxPostureItem    &GetCJKPosture() const  { return aCJKPosture; }
    const SvxFontItem       &GetCTLFont() const     { return aCTLFont; }
    const SvxFontHeightItem &GetCTLHeight() const   { return aCTLHeight; }
    const SvxWeightItem     &GetCTLWeight() const   { return aCTLWeight; }
    const SvxPostureItem    &GetCTLPosture() const  { return aCTLPosture; }
    const SvxUnderlineItem	&GetUnderline() const 	{ return aUnderline; }
    const SvxCrossedOutItem	&GetCrossedOut() const 	{ return aCrossedOut; }
    const SvxContourItem	&GetContour() const 	{ return aContour; }
    const SvxShadowedItem	&GetShadowed() const 	{ return aShadowed; }
    const SvxColorItem		&GetColor() const 		{ return aColor; }
    const SvxAdjustItem		&GetAdjust() const		{ return aAdjust; }
    const SvxBoxItem		&GetBox() const 		{ return aBox; }
    const SvxLineItem       &GetTLBR() const        { return aTLBR; }
    const SvxLineItem       &GetBLTR() const        { return aBLTR; }
    const SvxBrushItem		&GetBackground() const 	{ return aBackground; }
    void GetValueFormat( String& rFmt, LanguageType& rLng, LanguageType& rSys ) const
        { rFmt = sNumFmtString; rLng = eNumFmtLanguage; rSys = eSysLanguage; }

    // die SetMethoden
    void SetFont( const SvxFontItem& rNew ) 			{ aFont = rNew; }
    void SetHeight( const SvxFontHeightItem& rNew ) 	{ aHeight = rNew; }
    void SetWeight( const SvxWeightItem& rNew ) 		{ aWeight = rNew; }
    void SetPosture( const SvxPostureItem& rNew ) 		{ aPosture = rNew; }
    void SetCJKFont( const SvxFontItem& rNew )          { aCJKFont = rNew; }
    void SetCJKHeight( const SvxFontHeightItem& rNew )  { aCJKHeight = rNew; }
    void SetCJKWeight( const SvxWeightItem& rNew )      { aCJKWeight = rNew; }
    void SetCJKPosture( const SvxPostureItem& rNew )    { aCJKPosture = rNew; }
    void SetCTLFont( const SvxFontItem& rNew )          { aCTLFont = rNew; }
    void SetCTLHeight( const SvxFontHeightItem& rNew )  { aCTLHeight = rNew; }
    void SetCTLWeight( const SvxWeightItem& rNew )      { aCTLWeight = rNew; }
    void SetCTLPosture( const SvxPostureItem& rNew )    { aCTLPosture = rNew; }
    void SetUnderline( const SvxUnderlineItem& rNew ) 	{ aUnderline = rNew; }
    void SetCrossedOut( const SvxCrossedOutItem& rNew )	{ aCrossedOut = rNew; }
    void SetContour( const SvxContourItem& rNew ) 		{ aContour = rNew; }
    void SetShadowed( const SvxShadowedItem& rNew ) 	{ aShadowed = rNew; }
    void SetColor( const SvxColorItem& rNew ) 			{ aColor = rNew; }
    void SetAdjust( const SvxAdjustItem& rNew )
        {
            aAdjust.SetAdjust( rNew.GetAdjust() );
            aAdjust.SetOneWord( rNew.GetOneWord() );
            aAdjust.SetLastBlock( rNew.GetLastBlock() );
        }
    void SetBox( const SvxBoxItem& rNew ) 				{ aBox = rNew; }
    void SetBackground( const SvxBrushItem& rNew )      { aBackground = rNew; }
    void SetValueFormat( const String& rFmt, LanguageType eLng, LanguageType eSys )
        { sNumFmtString = rFmt; eNumFmtLanguage = eLng; eSysLanguage = eSys; }

    BOOL Load( SvStream& rStream, const SwAfVersions& rVersions, USHORT nVer );
    BOOL Save( SvStream& rStream ) const;
    BOOL SaveVerionNo( SvStream& rStream ) const;

#ifdef READ_OLDVERS
    // lade alte Version
    BOOL LoadOld( SvStream& rStream, USHORT aLoadVer[] );
#endif
};

class SW_DLLPUBLIC SwTableAutoFmt
{
    friend void _FinitCore();		// zum Zerstoeren des dflt. Pointers
    static SwBoxAutoFmt* pDfltBoxAutoFmt;

    String aName;
    USHORT nStrResId;

    // common flags of Calc and Writer
    BOOL bInclFont : 1;
    BOOL bInclJustify : 1;
    BOOL bInclFrame : 1;
    BOOL bInclBackground : 1;
    BOOL bInclValueFormat : 1;

    // Calc specific flags
    BOOL bInclWidthHeight : 1;

    SwBoxAutoFmt* aBoxAutoFmt[ 16 ];

public:
    SwTableAutoFmt( const String& rName );
    SwTableAutoFmt( const SwTableAutoFmt& rNew );
    ~SwTableAutoFmt();

    SwTableAutoFmt& operator=( const SwTableAutoFmt& rNew );

    void SetBoxFmt( const SwBoxAutoFmt& rNew, BYTE nPos );
    const SwBoxAutoFmt& GetBoxFmt( BYTE nPos ) const;

    void SetName( const String& rNew ) { aName = rNew; nStrResId = USHRT_MAX; }
    const String& GetName() const { return aName; }

    enum UpdateFlags { UPDATE_CHAR = 1, UPDATE_BOX = 2, UPDATE_ALL = 3 };
    SwBoxAutoFmt& UpdateFromSet( BYTE nPos, const SfxItemSet& rSet,
                                UpdateFlags eFlags, SvNumberFormatter* );
    void UpdateToSet( BYTE nPos, SfxItemSet& rSet, UpdateFlags eFlags,
                        SvNumberFormatter* ) const ;

    BOOL IsFont() const 		{ return bInclFont; }
    BOOL IsJustify() const 		{ return bInclJustify; }
    BOOL IsFrame() const 		{ return bInclFrame; }
    BOOL IsBackground() const	{ return bInclBackground; }
    BOOL IsValueFormat() const	{ return bInclValueFormat; }

    void SetFont( const BOOL bNew )			{ bInclFont = bNew; }
    void SetJustify( const  BOOL bNew )		{ bInclJustify = bNew; }
    void SetFrame( const BOOL bNew )		{ bInclFrame = bNew; }
    void SetBackground( const BOOL bNew )	{ bInclBackground = bNew; }
    void SetValueFormat( const BOOL bNew )	{ bInclValueFormat = bNew; }
    void SetWidthHeight( const BOOL bNew )	{ bInclWidthHeight = bNew; }

    BOOL Load( SvStream& rStream, const SwAfVersions& );
    BOOL Save( SvStream& rStream ) const;

#ifdef READ_OLDVERS
    // lade alte Version
    BOOL LoadOld( SvStream& rStream, USHORT aLoadVer[] );
#endif
};

typedef SwTableAutoFmt* SwTableAutoFmtPtr ;
SV_DECL_PTRARR_DEL( _SwTableAutoFmtTbl, SwTableAutoFmtPtr, 1, 5 )

class SW_DLLPUBLIC SwTableAutoFmtTbl : public _SwTableAutoFmtTbl
{
    SW_DLLPRIVATE BOOL Load( SvStream& rStream );
    SW_DLLPRIVATE BOOL Save( SvStream& rStream ) const;

public:
    SwTableAutoFmtTbl();

    BOOL Load();
    BOOL Save() const;
};

#endif
