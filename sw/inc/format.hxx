/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: format.hxx,v $
 * $Revision: 1.32.144.1 $
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
#ifndef _FORMAT_HXX
#define _FORMAT_HXX

#include <tools/solar.h>
#include "swdllapi.h"
#include <errhdl.hxx>		// fuer ASSERT
#include <swatrset.hxx>		// fuer SfxItemPool/-Set, Attr forward decl.
#include <calbck.hxx>		// fuer SwModify
#include <hintids.hxx>

class IDocumentSettingAccess;
class IDocumentDrawModelAccess;
class IDocumentLayoutAccess;
class IDocumentTimerAccess;
class IDocumentFieldsAccess;
class IDocumentChartDataProviderAccess;
class SwDoc;

class SwFmt : public SwModify
{
//  friend class SwSwgReader;
//  friend class SwSwgWriter;

    String aFmtName;
    SwAttrSet aSet;

    USHORT nWhichId;
    USHORT nFmtId;			// Format-ID fuer Lesen/Schreiben
    USHORT nPoolFmtId;		// Id-fuer "automatich" erzeugte Formate
                            // (ist keine harte Attributierung !!)
    USHORT nPoolHelpId;		// HelpId fuer diese Pool-Vorlage
    BYTE nPoolHlpFileId; 	// FilePos ans Doc auf die Vorlagen-Hilfen
    BOOL   bWritten : 1;	// TRUE: bereits geschrieben
    BOOL   bAutoFmt : 1;	// FALSE: es handelt sich um eine Vorlage
                            // ist dflt immer auf TRUE !
    BOOL   bFmtInDTOR : 1;	// TRUE: das Format wird geloscht. Damit man in
                            // der FmtChg-Message das erkennen kann!!!
    BOOL   bAutoUpdateFmt : 1;	// TRUE: am Format werden die Attribute
                            // eines kompletten Absatzes gesetzt (UI-seitig!)

protected:
    SwFmt( SwAttrPool& rPool, const sal_Char* pFmtNm,
            const USHORT* pWhichRanges, SwFmt *pDrvdFrm, USHORT nFmtWhich );
    SwFmt( SwAttrPool& rPool, const String &rFmtNm, const USHORT* pWhichRanges,
            SwFmt *pDrvdFrm, USHORT nFmtWhich );
    SwFmt( const SwFmt& rFmt );

public:
    TYPEINFO();		//Bereits in Basisklasse Client drin.

    virtual ~SwFmt();
    SwFmt &operator=(const SwFmt&);

    // fuer die Abfrage der Writer-Funktionen
    USHORT Which() const { return nWhichId; }

    virtual void Modify( SfxPoolItem* pOldValue, SfxPoolItem* pNewValue );
        // erfrage vom Format Informationen
    virtual BOOL GetInfo( SfxPoolItem& ) const;

    // kopiere Attribute; auch ueber Dokumentgrenzen hinweg
    void CopyAttrs( const SwFmt&, BOOL bReplace=TRUE );

    // loesche alle Attribute, die nicht in rFmt stehen
    void DelDiffs( const SfxItemSet& rSet );
    void DelDiffs( const SwFmt& rFmt ) { DelDiffs( rFmt.GetAttrSet() ); }

    // Umhaengen des Formats (0 = Default)
    BOOL SetDerivedFrom(SwFmt *pDerivedFrom = 0);

    // Ist bInParents FALSE,
    // wird nur in diesem Format nach dem Attribut gesucht.
    inline const SfxPoolItem& GetFmtAttr( USHORT nWhich,
                                          BOOL bInParents = TRUE ) const;
    inline SfxItemState GetItemState( USHORT nWhich, BOOL bSrchInParent = TRUE,
                                    const SfxPoolItem **ppItem = 0 ) const;
    // --> OD 2008-03-03 #refactorlists#
    // methods renamed and made virtual
    SW_DLLPUBLIC virtual BOOL SetFmtAttr( const SfxPoolItem& rAttr );
    SW_DLLPUBLIC virtual BOOL SetFmtAttr( const SfxItemSet& rSet );
    SW_DLLPUBLIC virtual BOOL ResetFmtAttr( USHORT nWhich1, USHORT nWhich2 = 0 );
    // <--

    // --> OD 2007-01-24 #i73790#
    // Method renamed and made virtual
    // Nimmt alle Hints aus dem Delta-Array,
    // liefert die Anzahl der geloeschten Hints
    virtual USHORT ResetAllFmtAttr();
    // <--

    inline SwFmt* DerivedFrom() const { return (SwFmt*)pRegisteredIn; }
    inline BOOL IsDefault() const { return DerivedFrom() == 0; }

    inline const String& GetName() const 		{ return aFmtName; }
    void SetName( const String& rNewName, sal_Bool bBroadcast=sal_False );
    inline void SetName( const sal_Char* pNewName,
                         sal_Bool bBroadcast=sal_False);

    // zur Abfrage des Attribute Arrays
    inline const SwAttrSet& GetAttrSet() const { return aSet; }

    // Das Doc wird jetzt am SwAttrPool gesetzt. Dadurch hat man es immer
    // im Zugriff.
    const SwDoc *GetDoc() const			{ return aSet.GetDoc(); }
          SwDoc *GetDoc() 				{ return aSet.GetDoc(); }

    /** Provides access to the document settings interface
     */
    const IDocumentSettingAccess* getIDocumentSettingAccess() const;

    /** Provides access to the document draw model interface
     */
    const IDocumentDrawModelAccess* getIDocumentDrawModelAccess() const;
          IDocumentDrawModelAccess* getIDocumentDrawModelAccess();

    /** Provides access to the document layout interface
     */
    const IDocumentLayoutAccess* getIDocumentLayoutAccess() const;
          IDocumentLayoutAccess* getIDocumentLayoutAccess();

     /** Provides access to the document idle timer interface
     */
    IDocumentTimerAccess* getIDocumentTimerAccess();

     /** Provides access to the document idle timer interface
     */
    IDocumentFieldsAccess* getIDocumentFieldsAccess();

     /** gives access to the chart data-provider
     */
    IDocumentChartDataProviderAccess* getIDocumentChartDataProviderAccess();

    // erfragen und setzen der Poolvorlagen-Id's
    USHORT GetPoolFmtId() const { return nPoolFmtId; }
    void SetPoolFmtId( USHORT nId ) { nPoolFmtId = nId; }

    // erfragen und setzen der Hilfe-Id's fuer die Document-Vorlagen
    USHORT GetPoolHelpId() const { return nPoolHelpId; }
    void SetPoolHelpId( USHORT nId ) { nPoolHelpId = nId; }
    BYTE GetPoolHlpFileId() const { return nPoolHlpFileId; }
    void SetPoolHlpFileId( BYTE nId ) { nPoolHlpFileId = nId; }
    // erfrage die Attribut-Beschreibung, returnt den reingereichten String
    void GetPresentation( SfxItemPresentation ePres,
        SfxMapUnit eCoreMetric,	SfxMapUnit ePresMetric,	String &rText ) const
        { aSet.GetPresentation( ePres, eCoreMetric,	ePresMetric, rText ); }
    // Das Format-ID fuer Lesen/Schreiben:
    void   ResetWritten()    { bWritten = FALSE; }

    // Abfragen/Setzen vom AutoFmt-Flag
    BOOL IsAuto() const 				{ return bAutoFmt; }
    void SetAuto( BOOL bNew = FALSE )	{ bAutoFmt = bNew; }

    // Abfragen/Setzen vom bAutoUpdateFmt-Flag
    BOOL IsAutoUpdateFmt() const 				{ return bAutoUpdateFmt; }
    void SetAutoUpdateFmt( BOOL bNew = TRUE )	{ bAutoUpdateFmt = bNew; }

    BOOL IsFmtInDTOR() const { return bFmtInDTOR; }

    // GetMethoden: das Bool gibt an, ob nur im Set (FALSE) oder auch in
    //				den Parents gesucht werden soll. Wird nichts gefunden,
    //				wird das deflt. Attribut returnt.
    // Charakter-Attribute	- impl. steht im charatr.hxx
    // AMA 12.10.94: Umstellung von SwFmt... auf Svx...
    inline const SvxPostureItem      &GetPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem       &GetWeight( BOOL = TRUE ) const;
    inline const SvxShadowedItem     &GetShadowed( BOOL = TRUE ) const;
    inline const SvxAutoKernItem     &GetAutoKern( BOOL = TRUE ) const;
    inline const SvxWordLineModeItem &GetWordLineMode( BOOL = TRUE ) const;
    inline const SvxContourItem      &GetContour( BOOL = TRUE ) const;
    inline const SvxKerningItem      &GetKerning( BOOL = TRUE ) const;
    inline const SvxUnderlineItem    &GetUnderline( BOOL = TRUE ) const;
    inline const SvxCrossedOutItem   &GetCrossedOut( BOOL = TRUE ) const;
    inline const SvxFontHeightItem   &GetSize( BOOL = TRUE ) const;
    inline const SvxPropSizeItem     &GetPropSize( BOOL = TRUE ) const;
    inline const SvxFontItem         &GetFont( BOOL = TRUE ) const;
    inline const SvxColorItem        &GetColor( BOOL = TRUE ) const;
    inline const SvxCharSetColorItem &GetCharSetColor( BOOL = TRUE ) const;
    inline const SvxLanguageItem     &GetLanguage( BOOL = TRUE ) const;
    inline const SvxEscapementItem   &GetEscapement( BOOL = TRUE ) const;
    inline const SvxCaseMapItem      &GetCaseMap( BOOL = TRUE ) const;
    inline const SvxNoHyphenItem     &GetNoHyphenHere( BOOL = TRUE ) const;
    inline const SvxBlinkItem		 &GetBlink( BOOL = TRUE ) const;
    inline const SvxBrushItem	 	 &GetChrBackground( BOOL = TRUE ) const;

    inline const SvxFontItem         &GetCJKFont( BOOL = TRUE ) const;
    inline const SvxFontHeightItem   &GetCJKSize( BOOL = TRUE ) const;
    inline const SvxLanguageItem     &GetCJKLanguage( BOOL = TRUE ) const;
    inline const SvxPostureItem      &GetCJKPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem       &GetCJKWeight( BOOL = TRUE ) const;
    inline const SvxFontItem         &GetCTLFont( BOOL = TRUE ) const;
    inline const SvxFontHeightItem   &GetCTLSize( BOOL = TRUE ) const;
    inline const SvxLanguageItem     &GetCTLLanguage( BOOL = TRUE ) const;
    inline const SvxPostureItem      &GetCTLPosture( BOOL = TRUE ) const;
    inline const SvxWeightItem       &GetCTLWeight( BOOL = TRUE ) const;
    inline const SfxBoolItem           &GetWritingDirection( BOOL = TRUE ) const;
    inline const SvxEmphasisMarkItem &GetEmphasisMark( BOOL = TRUE ) const;
    inline const SvxTwoLinesItem   &Get2Lines( BOOL = TRUE ) const;
    inline const SvxCharScaleWidthItem &GetCharScaleW( BOOL = TRUE ) const;
    inline const SvxCharRotateItem     &GetCharRotate( BOOL = TRUE ) const;
    inline const SvxCharReliefItem     &GetCharRelief( BOOL = TRUE ) const;
    inline const SvxCharHiddenItem   &GetCharHidden( BOOL = TRUE ) const;

    // Frame-Attribute	- impl. steht im frmatr.hxx,
    inline const SwFmtFillOrder           &GetFillOrder( BOOL = TRUE ) const;
    inline const SwFmtFrmSize             &GetFrmSize( BOOL = TRUE ) const;
    inline const SwFmtHeader          &GetHeader( BOOL = TRUE ) const;
    inline const SwFmtFooter          &GetFooter( BOOL = TRUE ) const;
    inline const SwFmtSurround            &GetSurround( BOOL = TRUE ) const;
    inline const SwFmtHoriOrient      &GetHoriOrient( BOOL = TRUE ) const;
    inline const SwFmtAnchor          &GetAnchor( BOOL = TRUE ) const;
    inline const SwFmtCol                 &GetCol( BOOL = TRUE ) const;
    inline const SvxPaperBinItem      &GetPaperBin( BOOL = TRUE ) const;
    inline const SvxLRSpaceItem           &GetLRSpace( BOOL = TRUE ) const;
    inline const SvxULSpaceItem           &GetULSpace( BOOL = TRUE ) const;
    inline const SwFmtCntnt           &GetCntnt( BOOL = TRUE ) const;
    inline const SvxPrintItem             &GetPrint( BOOL = TRUE ) const;
    inline const SvxOpaqueItem            &GetOpaque( BOOL = TRUE ) const;
    inline const SvxProtectItem           &GetProtect( BOOL = TRUE ) const;
    inline const SwFmtVertOrient      &GetVertOrient( BOOL = TRUE ) const;
    inline const SvxBoxItem               &GetBox( BOOL = TRUE ) const;
    inline const SvxFmtKeepItem         &GetKeep( BOOL = TRUE ) const;
    inline const SvxBrushItem           &GetBackground( BOOL = TRUE ) const;
    inline const SvxShadowItem            &GetShadow( BOOL = TRUE ) const;
    inline const SwFmtPageDesc            &GetPageDesc( BOOL = TRUE ) const;
    inline const SvxFmtBreakItem      &GetBreak( BOOL = TRUE ) const;
    inline const SvxMacroItem             &GetMacro( BOOL = TRUE ) const;
    inline const SwFmtURL             &GetURL( BOOL = TRUE ) const;
    inline const SwFmtEditInReadonly  &GetEditInReadonly( BOOL = TRUE ) const;
    inline const SwFmtLayoutSplit     &GetLayoutSplit( BOOL = TRUE ) const;
    inline const SwFmtRowSplit          &GetRowSplit( BOOL = TRUE ) const;
    inline const SwFmtChain               &GetChain( BOOL = TRUE ) const;
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
    inline const SwMirrorGrf          &GetMirrorGrf( BOOL = TRUE ) const;
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
    inline  const SwTblBoxNumFormat     &GetTblBoxNumFmt( BOOL = TRUE ) const;
    inline  const SwTblBoxFormula       &GetTblBoxFormula( BOOL = TRUE ) const;
    inline  const SwTblBoxValue         &GetTblBoxValue( BOOL = TRUE ) const;

    /** SwFmt::IsBackgroundTransparent - for feature #99657#

        OD 22.08.2002
        Virtual method to determine, if background of format is transparent.
        Default implementation returns false. Thus, subclasses have to overload
        method, if the specific subclass can have a transparent background.

        @author OD

        @return false, default implementation
    */
    virtual sal_Bool IsBackgroundTransparent() const;

    /** SwFmt::IsShadowTransparent - for feature #99657#

        OD 22.08.2002
        Virtual method to determine, if shadow of format is transparent.
        Default implementation returns false. Thus, subclasses have to overload
        method, if the specific subclass can have a transparent shadow.

        @author OD

        @return false, default implementation
    */
    virtual sal_Bool IsShadowTransparent() const;
};

// --------------- inline Implementierungen ------------------------

inline const SfxPoolItem& SwFmt::GetFmtAttr( USHORT nWhich,
                                             BOOL bInParents ) const
{
    return aSet.Get( nWhich, bInParents );
}

inline void SwFmt::SetName( const sal_Char* pNewName,
                             sal_Bool bBroadcast )
{
    String aTmp( String::CreateFromAscii( pNewName ) );
    SetName( aTmp, bBroadcast );
}

inline SfxItemState SwFmt::GetItemState( USHORT nWhich, BOOL bSrchInParent,
                                        const SfxPoolItem **ppItem ) const
{
    return aSet.GetItemState( nWhich, bSrchInParent, ppItem );
}

#undef inline

#endif // _FORMAT_HXX
