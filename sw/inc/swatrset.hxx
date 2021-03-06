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
#ifndef _SWATRSET_HXX
#define _SWATRSET_HXX
#include <tools/solar.h>
#include <tools/mempool.hxx>
#include <svl/itemset.hxx>
#include <svl/itempool.hxx>
#include <swdllapi.h>

class SwModify;
class SwDoc;
class OutputDevice;
class IDocumentSettingAccess;
class SfxBoolItem;
class SvxPostureItem;
class SvxWeightItem;
class SvxShadowedItem;
class SvxAutoKernItem;
class SvxWordLineModeItem;
class SvxContourItem;
class SvxKerningItem;
class SvxUnderlineItem;
class SvxOverlineItem;
class SvxCrossedOutItem;
class SvxFontHeightItem;
class SvxPropSizeItem;
class SvxFontItem;
class SvxColorItem;
class SvxCharSetColorItem;
class SvxLanguageItem;
class SvxEscapementItem;
class SvxCaseMapItem;
class SvxNoHyphenItem;
class SvxBlinkItem;
class SvxEmphasisMarkItem;
class SvxTwoLinesItem;
class SvxCharScaleWidthItem;
class SvxCharRotateItem;
class SvxCharReliefItem;
class SvxCharHiddenItem;

// Frame-Attribute
class SwFmtFillOrder;
class SwFmtFrmSize;
class SvxPaperBinItem;
class SvxLRSpaceItem;
class SvxULSpaceItem;
class SwFmtCntnt;
class SwFmtHeader;
class SwFmtFooter;
class SvxPrintItem;
class SvxOpaqueItem;
class SvxProtectItem;
class SwFmtSurround;
class SwFmtVertOrient;
class SwFmtHoriOrient;
class SwFmtAnchor;
class SvxBoxItem;
class SvxBrushItem;
class SvxShadowItem;
class SwFmtPageDesc;
class SvxFmtBreakItem;
class SwFmtCol;
class SvxMacroItem;
class SvxFmtKeepItem;
class SwFmtURL;
class SwFmtLineNumber;
class SwFmtEditInReadonly;
class SwFmtLayoutSplit;
class SwFmtRowSplit;
class SwFmtChain;
class SwFmtFtnAtTxtEnd;
class SwFmtEndAtTxtEnd;
class SwFmtNoBalancedColumns;
class SvxFrameDirectionItem;
class SwTextGridItem;
class SwHeaderAndFooterEatSpacingItem;
// OD 18.09.2003 #i18732#
class SwFmtFollowTextFlow;
// OD 2004-05-05 #i28701#
class SwFmtWrapInfluenceOnObjPos;

// Grafik-Attribute
class SwMirrorGrf;
class SwCropGrf;
class SwRotationGrf;
class SwLuminanceGrf;
class SwContrastGrf;
class SwChannelRGrf;
class SwChannelGGrf;
class SwChannelBGrf;
class SwGammaGrf;
class SwInvertGrf;
class SwTransparencyGrf;
class SwDrawModeGrf;

// Paragraph-Attribute
class SvxLineSpacingItem;
class SvxAdjustItem;
class SvxFmtSplitItem;
class SwRegisterItem;
class SwNumRuleItem;
class SvxWidowsItem;
class SvxOrphansItem;
class SvxTabStopItem;
class SvxHyphenZoneItem;
class SwFmtDrop;
class SvxScriptSpaceItem;
class SvxHangingPunctuationItem;
class SvxForbiddenRuleItem;
class SvxParaVertAlignItem;
class SvxParaGridItem;
class SwParaConnectBorderItem;

// TabellenBox-Attribute
class SwTblBoxNumFormat;
class SwTblBoxFormula;
class SwTblBoxValue;

class SwAttrPool : public SfxItemPool
{
    friend void _InitCore();			// fuers anlegen/zerstoeren der
    friend void _FinitCore();           // Versionsmaps
    static USHORT* pVersionMap1;
    static USHORT* pVersionMap2;
    static USHORT* pVersionMap3;
    static USHORT* pVersionMap4;
    // OD 2004-01-21 #i18732# - due to extension of attribute set a new version
    // map for binary filter is necessary (version map 5).
    static USHORT* pVersionMap5;
    static USHORT* pVersionMap6;

    SwDoc* pDoc;

public:
    SwAttrPool( SwDoc* pDoc );
protected:
    virtual ~SwAttrPool();
public:

          SwDoc* GetDoc() 			{ return pDoc; }
    const SwDoc* GetDoc() const		{ return pDoc; }

    static USHORT* GetVersionMap1() { return pVersionMap1; }
    static USHORT* GetVersionMap2() { return pVersionMap2; }
    static USHORT* GetVersionMap3() { return pVersionMap3; }
    static USHORT* GetVersionMap6() { return pVersionMap4; }
};


class SW_DLLPUBLIC SwAttrSet : public SfxItemSet
{
    // Pointer fuers Modify-System
    SwAttrSet *pOldSet, *pNewSet;

    // Notification-Callback
    virtual void Changed( const SfxPoolItem& rOld, const SfxPoolItem& rNew );

    void PutChgd( const SfxPoolItem& rI ) { SfxItemSet::PutDirect( rI ); }
public:
    SwAttrSet( SwAttrPool&, USHORT nWhich1, USHORT nWhich2 );
    SwAttrSet( SwAttrPool&, const USHORT* nWhichPairTable );
    SwAttrSet( const SwAttrSet& );

    virtual SfxItemSet* Clone(BOOL bItems = TRUE, SfxItemPool *pToPool = 0) const;

    int Put_BC( const SfxPoolItem& rAttr, SwAttrSet* pOld, SwAttrSet* pNew );
    int Put_BC( const SfxItemSet& rSet, SwAttrSet* pOld, SwAttrSet* pNew );

    // ein Item oder einen Bereich loeschen
    USHORT ClearItem_BC( USHORT nWhich, SwAttrSet* pOld, SwAttrSet* pNew );
    USHORT ClearItem_BC( USHORT nWhich1, USHORT nWhich2,
                        SwAttrSet* pOld = 0, SwAttrSet* pNew = 0 );

    int Intersect_BC( const SfxItemSet& rSet, SwAttrSet* pOld, SwAttrSet* pNew );

    void GetPresentation( SfxItemPresentation ePres,
        SfxMapUnit eCoreMetric,	SfxMapUnit ePresMetric,	String &rText ) const;

    SwAttrPool*	GetPool() const { return (SwAttrPool*)SfxItemSet::GetPool(); }

    // kopiere die Attribute ggfs. ueber Dokumentgrenzen
    void CopyToModify( SwModify& rMod ) const;

    // ----------------------------------------------------------------
    // Sonderbehandlung fuer einige Attribute
    // Setze den Modify-Pointer (alten pDefinedIn) bei folgenden Attributen:
    //	- SwFmtDropCaps
    //	- SwFmtPageDesc
    // (Wird beim Einfuegen in Formate/Nodes gerufen)
    // Second version is for the SwAttrSet handles of SwCntntNode.
    bool SetModifyAtAttr( const SwModify* pModify );

    // Das Doc wird jetzt am SwAttrPool gesetzt. Dadurch hat man es immer
    // im Zugriff.
    const SwDoc *GetDoc() const { return GetPool()->GetDoc(); }
          SwDoc *GetDoc() 		{ return GetPool()->GetDoc(); }

    // GetMethoden: das Bool gibt an, ob nur im Set (FALSE) oder auch in
    //				den Parents gesucht werden soll. Wird nichts gefunden,
    //				wird das deflt. Attribut returnt.
    // Charakter-Attribute	- impl. steht im charatr.hxx
    // AMA 12.10.94: Umstellung von SwFmt... auf Svx...
    inline const SvxPostureItem           &GetPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem          &GetWeight( BOOL = TRUE ) const;
    inline const SvxShadowedItem        &GetShadowed( BOOL = TRUE ) const;
    inline const SvxAutoKernItem        &GetAutoKern( BOOL = TRUE ) const;
    inline const SvxWordLineModeItem    &GetWordLineMode( BOOL = TRUE ) const;
    inline const SvxContourItem         &GetContour( BOOL = TRUE ) const;
    inline const SvxKerningItem         &GetKerning( BOOL = TRUE ) const;
    inline const SvxUnderlineItem       &GetUnderline( BOOL = TRUE ) const;
    inline const SvxOverlineItem        &GetOverline( BOOL = TRUE ) const;
    inline const SvxCrossedOutItem      &GetCrossedOut( BOOL = TRUE ) const;
    inline const SvxFontHeightItem        &GetSize( BOOL = TRUE ) const;
    inline const SvxPropSizeItem        &GetPropSize( BOOL = TRUE ) const;
    inline const SvxFontItem            &GetFont( BOOL = TRUE ) const;
    inline const SvxColorItem           &GetColor( BOOL = TRUE ) const;
    inline const SvxCharSetColorItem    &GetCharSetColor( BOOL = TRUE ) const;
    inline const SvxLanguageItem        &GetLanguage( BOOL = TRUE ) const;
    inline const SvxEscapementItem      &GetEscapement( BOOL = TRUE ) const;
    inline const SvxCaseMapItem         &GetCaseMap( BOOL = TRUE ) const;
    inline const SvxNoHyphenItem      &GetNoHyphenHere( BOOL = TRUE ) const;
    inline const SvxBlinkItem         &GetBlink( BOOL = TRUE ) const;
    inline const SvxBrushItem         &GetChrBackground( BOOL = TRUE ) const;
    inline const SvxFontItem          &GetCJKFont( BOOL = TRUE ) const;
    inline const SvxFontHeightItem    &GetCJKSize( BOOL = TRUE ) const;
    inline const SvxLanguageItem      &GetCJKLanguage( BOOL = TRUE ) const;
    inline const SvxPostureItem       &GetCJKPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem        &GetCJKWeight( BOOL = TRUE ) const;
    inline const SvxFontItem          &GetCTLFont( BOOL = TRUE ) const;
    inline const SvxFontHeightItem    &GetCTLSize( BOOL = TRUE ) const;
    inline const SvxLanguageItem      &GetCTLLanguage( BOOL = TRUE ) const;
    inline const SvxPostureItem       &GetCTLPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem        &GetCTLWeight( BOOL = TRUE ) const;
    inline const SfxBoolItem              &GetWritingDirection( BOOL = TRUE ) const;
    inline const SvxEmphasisMarkItem  &GetEmphasisMark( BOOL = TRUE ) const;
    inline const SvxTwoLinesItem      &Get2Lines( BOOL = TRUE ) const;
    inline const SvxCharScaleWidthItem    &GetCharScaleW( BOOL = TRUE ) const;
    inline const SvxCharRotateItem        &GetCharRotate( BOOL = TRUE ) const;
    inline const SvxCharReliefItem        &GetCharRelief( BOOL = TRUE ) const;
    inline const SvxCharHiddenItem      &GetCharHidden( BOOL = TRUE ) const;

    // Frame-Attribute	- impl. steht im frmatr.hxx
    inline const SwFmtFillOrder       &GetFillOrder( BOOL = TRUE ) const;
    inline const SwFmtFrmSize             &GetFrmSize( BOOL = TRUE ) const;
    inline const SvxPaperBinItem      &GetPaperBin( BOOL = TRUE ) const;
    inline const SvxLRSpaceItem           &GetLRSpace( BOOL = TRUE ) const;
    inline const SvxULSpaceItem           &GetULSpace( BOOL = TRUE ) const;
    inline const SwFmtCntnt           &GetCntnt( BOOL = TRUE ) const;
    inline const SwFmtHeader          &GetHeader( BOOL = TRUE ) const;
    inline const SwFmtFooter          &GetFooter( BOOL = TRUE ) const;
    inline const SvxPrintItem             &GetPrint( BOOL = TRUE ) const;
    inline const SvxOpaqueItem            &GetOpaque( BOOL = TRUE ) const;
    inline const SvxProtectItem           &GetProtect( BOOL = TRUE ) const;
    inline const SwFmtSurround            &GetSurround( BOOL = TRUE ) const;
    inline const SwFmtVertOrient      &GetVertOrient( BOOL = TRUE ) const;
    inline const SwFmtHoriOrient      &GetHoriOrient( BOOL = TRUE ) const;
    inline const SwFmtAnchor          &GetAnchor( BOOL = TRUE ) const;
    inline const SvxBoxItem               &GetBox( BOOL = TRUE ) const;
    inline const SvxFmtKeepItem         &GetKeep( BOOL = TRUE ) const;
    inline const SvxBrushItem           &GetBackground( BOOL = TRUE ) const;
    inline const SvxShadowItem            &GetShadow( BOOL = TRUE ) const;
    inline const SwFmtPageDesc            &GetPageDesc( BOOL = TRUE ) const;
    inline const SvxFmtBreakItem      &GetBreak( BOOL = TRUE ) const;
    inline const SwFmtCol                 &GetCol( BOOL = TRUE ) const;
    inline const SvxMacroItem             &GetMacro( BOOL = TRUE ) const;
    inline const SwFmtURL             &GetURL( BOOL = TRUE ) const;
    inline const SwFmtEditInReadonly  &GetEditInReadonly( BOOL = TRUE ) const;
    inline const SwFmtLayoutSplit     &GetLayoutSplit( BOOL = TRUE ) const;
    inline const SwFmtRowSplit          &GetRowSplit( BOOL = TRUE ) const;
    inline const SwFmtChain             &GetChain( BOOL = TRUE ) const;
    inline const SwFmtLineNumber      &GetLineNumber( BOOL = TRUE ) const;
    inline const SwFmtFtnAtTxtEnd     &GetFtnAtTxtEnd( BOOL = TRUE ) const;
    inline const SwFmtEndAtTxtEnd     &GetEndAtTxtEnd( BOOL = TRUE ) const;
    inline const SwFmtNoBalancedColumns &GetBalancedColumns( BOOL = TRUE ) const;
    inline const SvxFrameDirectionItem    &GetFrmDir( BOOL = TRUE ) const;
    inline const SwTextGridItem         &GetTextGrid( BOOL = TRUE ) const;
    inline const SwHeaderAndFooterEatSpacingItem &GetHeaderAndFooterEatSpacing( BOOL = TRUE ) const;
    // OD 18.09.2003 #i18732#
    inline const SwFmtFollowTextFlow    &GetFollowTextFlow(BOOL = TRUE) const;
    // OD 2004-05-05 #i28701#
    inline const SwFmtWrapInfluenceOnObjPos& GetWrapInfluenceOnObjPos(BOOL = TRUE) const;

    // Grafik-Attribute	- impl. steht im grfatr.hxx
    inline const SwMirrorGrf            &GetMirrorGrf( BOOL = TRUE ) const;
    inline const SwCropGrf            &GetCropGrf( BOOL = TRUE ) const;
    inline const SwRotationGrf            &GetRotationGrf(BOOL = TRUE ) const;
    inline const SwLuminanceGrf       &GetLuminanceGrf(BOOL = TRUE ) const;
    inline const SwContrastGrf            &GetContrastGrf(BOOL = TRUE ) const;
    inline const SwChannelRGrf            &GetChannelRGrf(BOOL = TRUE ) const;
    inline const SwChannelGGrf            &GetChannelGGrf(BOOL = TRUE ) const;
    inline const SwChannelBGrf            &GetChannelBGrf(BOOL = TRUE ) const;
    inline const SwGammaGrf           &GetGammaGrf(BOOL = TRUE ) const;
    inline const SwInvertGrf          &GetInvertGrf(BOOL = TRUE ) const;
    inline const SwTransparencyGrf        &GetTransparencyGrf(BOOL = TRUE ) const;
    inline const SwDrawModeGrf            &GetDrawModeGrf(BOOL = TRUE ) const;

    // Paragraph-Attribute	- impl. steht im paratr.hxx
    inline const SvxLineSpacingItem       &GetLineSpacing( BOOL = TRUE ) const;
    inline const SvxAdjustItem            &GetAdjust( BOOL = TRUE ) const;
    inline const SvxFmtSplitItem      &GetSplit( BOOL = TRUE ) const;
    inline const SwRegisterItem           &GetRegister( BOOL = TRUE ) const;
    inline const SwNumRuleItem            &GetNumRule( BOOL = TRUE ) const;
    inline const SvxWidowsItem            &GetWidows( BOOL = TRUE ) const;
    inline const SvxOrphansItem           &GetOrphans( BOOL = TRUE ) const;
    inline const SvxTabStopItem           &GetTabStops( BOOL = TRUE ) const;
    inline const SvxHyphenZoneItem        &GetHyphenZone( BOOL = TRUE ) const;
    inline const SwFmtDrop                &GetDrop( BOOL = TRUE ) const;
    inline const SvxScriptSpaceItem       &GetScriptSpace(BOOL = TRUE) const;
    inline const SvxHangingPunctuationItem &GetHangingPunctuation(BOOL = TRUE) const;
    inline const SvxForbiddenRuleItem     &GetForbiddenRule(BOOL = TRUE) const;
    inline const SvxParaVertAlignItem &GetParaVertAlign(BOOL = TRUE) const;
    inline const SvxParaGridItem        &GetParaGrid(BOOL = TRUE) const;
    inline const SwParaConnectBorderItem &GetParaConnectBorder(BOOL = TRUE ) const;

    // TabellenBox-Attribute	- impl. steht im cellatr.hxx
    inline  const SwTblBoxNumFormat       &GetTblBoxNumFmt( BOOL = TRUE ) const;
    inline  const SwTblBoxFormula     &GetTblBoxFormula( BOOL = TRUE ) const;
    inline  const SwTblBoxValue           &GetTblBoxValue( BOOL = TRUE ) const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwAttrSet)
};

//Helper for filters to find true lineheight of a font
SW_DLLPUBLIC long AttrSetToLineHeight( const IDocumentSettingAccess& rIDocumentSettingAccess,
                          const SwAttrSet &rSet,
                          const OutputDevice &rOut, sal_Int16 nScript);
#endif
