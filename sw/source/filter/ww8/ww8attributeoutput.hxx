/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile$
 * $Revision$
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

#ifndef _WW8ATTRIBUTEOUTPUT_HXX_
#define _WW8ATTRIBUTEOUTPUT_HXX_

#include "attributeoutputbase.hxx"
#include "wrtww8.hxx"

class WW8AttributeOutput : public AttributeOutputBase
{
public:
    /// Export the state of RTL/CJK.
    virtual void RTLAndCJKState( bool bIsRTL, sal_uInt16 nScript );

    /// Start of the paragraph.
    virtual void StartParagraph( ww8::WW8TableNodeInfo::Pointer_t /*pTextNodeInfo*/ ) {}

    /// End of the paragraph.
    virtual void EndParagraph( ww8::WW8TableNodeInfoInner::Pointer_t pTextNodeInfoInner );

    /// Called before we start outputting the attributes.
    virtual void StartParagraphProperties( const SwTxtNode& /*rNode*/ ) {}

    /// Called after we end outputting the attributes.
    virtual void EndParagraphProperties() {}

    /// Empty paragraph.
    virtual void EmptyParagraph();

    /// Start of the text run.
    ///
    /// No-op for binary filters.
    virtual void StartRun( const SwRedlineData* /*pRedlineData*/ ) {}

    /// End of the text run.
    ///
    /// No-op for binary filters.
    virtual void EndRun() {}

    /// Before we start outputting the attributes.
    virtual void StartRunProperties();

    /// After we end outputting the attributes.
    virtual void EndRunProperties( const SwRedlineData* pRedlineData );

    /// Output text.
    virtual void RunText( const String& rText, rtl_TextEncoding eCharSet );
    
    /// Output text (without markup).
    virtual void RawText( const String& rText, bool bForceUnicode, rtl_TextEncoding eCharSet );

    /// Output ruby start.
    virtual void StartRuby( const SwTxtNode& rNode, const SwFmtRuby& rRuby );

    /// Output ruby end.
    virtual void EndRuby();

    /// Output URL start.
    virtual bool StartURL( const String &rUrl, const String &rTarget );

    /// Output URL end.
    virtual bool EndURL();

    virtual void FieldVanish( const String& rTxt, ww::eField eType );

    /// Output redlining.
    virtual void Redline( const SwRedlineData* pRedline );

    virtual void FormatDrop( const SwTxtNode& rNode, const SwFmtDrop &rSwFmtDrop, USHORT nStyle, ww8::WW8TableNodeInfo::Pointer_t pTextNodeInfo, ww8::WW8TableNodeInfoInner::Pointer_t pTextNodeInfoInner );

    /// Output FKP (Formatted disK Page) - necessary for binary formats only.
    /// FIXME having it in AttributeOutputBase is probably a hack, it
    /// should be in WW8AttributeOutput only...
    virtual void OutputFKP();

    /// Output style.
    virtual void ParagraphStyle( USHORT nStyle );

    virtual void TableInfoCell( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableInfoRow( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableDefinition( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableDefaultBorders( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableBackgrounds( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableHeight( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableCanSplit( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableBidi( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableVerticalCell( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfo );

    virtual void TableNodeInfo( ww8::WW8TableNodeInfo::Pointer_t pNodeInfo );

    virtual void TableNodeInfoInner( ww8::WW8TableNodeInfoInner::Pointer_t pNodeInfoInner );

    virtual void TableOrientation( ww8::WW8TableNodeInfoInner::Pointer_t pTableTextNodeInfoInner );

    virtual void TableRowEnd( sal_uInt32 nDepth = 1 );

    /// Start of the styles table.
    virtual void StartStyles();

    /// End of the styles table.
    virtual void EndStyles( USHORT nNumberOfStyles );

    /// Write default style.
    virtual void DefaultStyle( USHORT nStyle );

    /// Start of a style in the styles table.
    virtual void StartStyle( const String& rName, bool bPapFmt,
            USHORT nBase, USHORT nNext, USHORT nWwIdi, USHORT nId );

    /// End of a style in the styles table.
    virtual void EndStyle();

    /// Start of (paragraph or run) properties of a style.
    virtual void StartStyleProperties( bool bParProp, USHORT nStyle );

    /// End of (paragraph or run) properties of a style.
    virtual void EndStyleProperties( bool bParProp );

    /// Numbering rule and Id.
    virtual void OutlineNumbering( BYTE nLvl, const SwNumFmt &rNFmt, const SwFmt &rFmt );

    /// Page break
    /// As a paragraph property - the paragraph should be on the next page.
    virtual void PageBreakBefore( bool bBreak );

    /// Write a section break
    /// msword::ColumnBreak or msword::PageBreak
    virtual void SectionBreak( BYTE nC, const WW8_SepInfo* pSectionInfo = NULL );

    /// Start of the section properties.
    virtual void StartSection();

    /// End of the section properties.
    ///
    /// No-op for binary filters.
    virtual void EndSection() {}

    /// Protection of forms.
    virtual void SectionFormProtection( bool bProtected );

    /// Numbering of the lines in the document.
    virtual void SectionLineNumbering( ULONG nRestartNo, const SwLineNumberInfo& rLnNumInfo );

    /// Has different headers/footers for the title page.
    virtual void SectionTitlePage();

    /// Description of the page borders.
    virtual void SectionPageBorders( const SwFrmFmt* pFmt, const SwFrmFmt* pFirstPageFmt );

    /// Columns populated from right/numbers on the right side?
    virtual void SectionBiDi( bool bBiDi );

    /// The style of the page numbers.
    ///
    /// nPageRestartNumberr being 0 means no restart.
    virtual void SectionPageNumbering( USHORT nNumType, USHORT nPageRestartNumber );

    /// The type of breaking.
    virtual void SectionType( BYTE nBreakCode );

    /// Special header/footer flags, needed for old versions only.
    virtual void SectionWW6HeaderFooterFlags( BYTE nHeadFootFlags );

    /// Definition of a numbering instance.
    virtual void NumberingDefinition( USHORT nId, const SwNumRule &rRule );

    /// All the numbering level information.
    virtual void NumberingLevel( BYTE nLevel,
        USHORT nStart,
        USHORT nNumberingType,
        SvxAdjust eAdjust,
        const BYTE *pNumLvlPos,
        BYTE nFollow,
        const wwFont *pFont,
        const SfxItemSet *pOutSet,
        sal_Int16 nIndentAt,
        sal_Int16 nFirstLineIndex,
        sal_Int16 nListTabPos,
        const String &rNumberingString );

protected:
    /// Output frames - the implementation.
    void OutputFlyFrame_Impl( const sw::Frame& rFmt, const Point& rNdTopLeft );

    /// Sfx item Sfx item RES_CHRATR_CASEMAP
    virtual void CharCaseMap( const SvxCaseMapItem& );

    /// Sfx item Sfx item RES_CHRATR_COLOR
    virtual void CharColor( const SvxColorItem& );

    /// Sfx item Sfx item RES_CHRATR_CONTOUR
    virtual void CharContour( const SvxContourItem& );

    /// Sfx item RES_CHRATR_CROSSEDOUT
    virtual void CharCrossedOut( const SvxCrossedOutItem& rHt );

    /// Sfx item RES_CHRATR_ESCAPEMENT
    virtual void CharEscapement( const SvxEscapementItem& );

    /// Sfx item RES_CHRATR_FONT
    virtual void CharFont( const SvxFontItem& );

    /// Sfx item RES_CHRATR_FONTSIZE
    virtual void CharFontSize( const SvxFontHeightItem& );

    /// Sfx item RES_CHRATR_KERNING
    virtual void CharKerning( const SvxKerningItem& );

    /// Sfx item RES_CHRATR_LANGUAGE
    virtual void CharLanguage( const SvxLanguageItem& );

    /// Sfx item RES_CHRATR_POSTURE
    virtual void CharPosture( const SvxPostureItem& );

    /// Sfx item RES_CHRATR_SHADOWED
    virtual void CharShadow( const SvxShadowedItem& );

    /// Sfx item RES_CHRATR_UNDERLINE
    virtual void CharUnderline( const SvxUnderlineItem& );

    /// Sfx item RES_CHRATR_WEIGHT
    virtual void CharWeight( const SvxWeightItem& );

    /// Sfx item RES_CHRATR_AUTOKERN
    virtual void CharAutoKern( const SvxAutoKernItem& );

    /// Sfx item RES_CHRATR_BLINK
    virtual void CharAnimatedText( const SvxBlinkItem& );

    /// Sfx item RES_CHRATR_BACKGROUND
    virtual void CharBackground( const SvxBrushItem& );

    /// Sfx item RES_CHRATR_CJK_FONT
    virtual void CharFontCJK( const SvxFontItem& );

    /// Sfx item RES_CHRATR_CJK_FONTSIZE
    virtual void CharFontSizeCJK( const SvxFontHeightItem& rHt ) { CharFontSize( rHt ); }

    /// Sfx item RES_CHRATR_CJK_LANGUAGE
    virtual void CharLanguageCJK( const SvxLanguageItem& rHt ) { CharLanguage( rHt ); }

    /// Sfx item RES_CHRATR_CJK_POSTURE
    virtual void CharPostureCJK( const SvxPostureItem& rHt ) { CharPosture( rHt ); }

    /// Sfx item RES_CHRATR_CJK_WEIGHT
    virtual void CharWeightCJK( const SvxWeightItem& rHt ) { CharWeight( rHt ); }

    /// Sfx item RES_CHRATR_CTL_FONT
    virtual void CharFontCTL( const SvxFontItem& );

    /// Sfx item RES_CHRATR_CTL_FONTSIZE
    virtual void CharFontSizeCTL( const SvxFontHeightItem& rHt ) { CharFontSize( rHt ); }

    /// Sfx item RES_CHRATR_CTL_LANGUAGE
    virtual void CharLanguageCTL( const SvxLanguageItem& rHt ) { CharLanguage( rHt ); }

    /// Sfx item RES_CHRATR_CTL_POSTURE
    virtual void CharPostureCTL( const SvxPostureItem& );

    /// Sfx item RES_CHRATR_CTL_WEIGHT
    virtual void CharWeightCTL( const SvxWeightItem& );

    /// Sfx item RES_CHRATR_ROTATE
    virtual void CharRotate( const SvxCharRotateItem& );

    /// Sfx item RES_CHRATR_EMPHASIS_MARK
    virtual void CharEmphasisMark( const SvxEmphasisMarkItem& rHt );

    /// Sfx item RES_CHRATR_TWO_LINES
    virtual void CharTwoLines( const SvxTwoLinesItem& );

    /// Sfx item RES_CHRATR_SCALEW
    virtual void CharScaleWidth( const SvxCharScaleWidthItem& );

    /// Sfx item RES_CHRATR_RELIEF
    virtual void CharRelief( const SvxCharReliefItem& );

    /// Sfx item RES_CHRATR_HIDDEN
    virtual void CharHidden( const SvxCharHiddenItem& );

    /// Sfx item RES_TXTATR_INETFMT
    virtual void TextINetFormat( const SwFmtINetFmt& );

    /// Sfx item RES_TXTATR_CHARFMT
    virtual void TextCharFormat( const SwFmtCharFmt& );

    /// Sfx item RES_TXTATR_FTN
    virtual void TextFootnote_Impl( const SwFmtFtn& );

    /// Sfx item RES_PARATR_LINESPACING
    virtual void ParaLineSpacing_Impl( short nSpace, short nMulti );

    /// Sfx item RES_PARATR_ADJUST
    virtual void ParaAdjust( const SvxAdjustItem& rHt );

    /// Sfx item RES_PARATR_SPLIT
    virtual void ParaSplit( const SvxFmtSplitItem& );

    /// Sfx item RES_PARATR_WIDOWS
    virtual void ParaWidows( const SvxWidowsItem& rHt );

    /// Sfx item RES_PARATR_TABSTOP
    virtual void ParaTabStop( const SvxTabStopItem& rHt );

    /// Sfx item RES_PARATR_HYPHENZONE
    virtual void ParaHyphenZone( const SvxHyphenZoneItem& );

    /// Sfx item RES_PARATR_NUMRULE
    virtual void ParaNumRule_Impl( const SwTxtNode *pTxtNd, sal_Int32 nLvl, sal_Int32 nNumId );

    /// Sfx item RES_PARATR_SCRIPTSPACE
    virtual void ParaScriptSpace( const SfxBoolItem& );

    /// Sfx item RES_PARATR_VERTALIGN
    virtual void ParaVerticalAlign( const SvxParaVertAlignItem& );

    /// Sfx item RES_PARATR_SNAPTOGRID
    virtual void ParaSnapToGrid( const SvxParaGridItem& );

    /// Sfx item RES_FRM_SIZE
    virtual void FormatFrameSize( const SwFmtFrmSize& );

    /// Sfx item RES_PAPER_BIN
    virtual void FormatPaperBin( const SvxPaperBinItem& );

    /// Sfx item RES_LR_SPACE
    virtual void FormatLRSpace( const SvxLRSpaceItem& );

    /// Sfx item RES_UL_SPACE
    virtual void FormatULSpace( const SvxULSpaceItem& rHt );

    /// Sfx item RES_SURROUND
    virtual void FormatSurround( const SwFmtSurround& );

    /// Sfx item RES_VERT_ORIENT
    virtual void FormatVertOrientation( const SwFmtVertOrient& );

    /// Sfx item RES_HORI_ORIENT
    virtual void FormatHorizOrientation( const SwFmtHoriOrient& );

    /// Sfx item RES_ANCHOR
    virtual void FormatAnchor( const SwFmtAnchor& );

    /// Sfx item RES_BACKGROUND
    virtual void FormatBackground( const SvxBrushItem& );

    /// Sfx item RES_BOX
    virtual void FormatBox( const SvxBoxItem& );

    /// Sfx item RES_COL
    virtual void FormatColumns_Impl( USHORT nCols, const SwFmtCol & rCol, bool bEven, SwTwips nPageSize );

    /// Sfx item RES_KEEP
    virtual void FormatKeep( const SvxFmtKeepItem& );

    /// Sfx item RES_TEXTGRID
    virtual void FormatTextGrid( const SwTextGridItem& );

    /// Sfx item RES_LINENUMBER
    virtual void FormatLineNumbering( const SwFmtLineNumber& );

    /// Sfx item RES_FRAMEDIR
    virtual void FormatFrameDirection( const SvxFrameDirectionItem& );
    
    /// Write the expanded field
    virtual void WriteExpand( const SwField* pFld );
    
    virtual void RefField ( const SwField& rFld, const String& rRef );
    virtual void HiddenField( const SwField& rFld );
    virtual void SetField( const SwField& rFld, ww::eField eType, const String& rCmd ); 
    virtual void PostitField( const SwField* pFld );
    virtual bool DropdownField( const SwField* pFld );

    virtual bool AnalyzeURL( const String& rURL, const String& rTarget, String* pLinkURL, String* pMark );

    /// Reference to the export, where to get the data from
    WW8Export &m_rWW8Export;

    /// For output of styles.
    ///
    /// We have to remember these positions between the StartStyle() and
    /// EndStyle().
    USHORT nPOPosStdLen1, nPOPosStdLen2;

    /// For output of styles.
    ///
    /// We have to remember this position between StartStyleProperties() and
    /// EndStyleProperties().
    USHORT m_nStyleStartSize, m_nStyleLenPos;

    /// For output of styles.
    ///
    /// Used between StartStyles() and EndStyles().
    ULONG m_nStyAnzPos;

    /// For output of run properties.
    ///
    /// We have to remember the number of field results, and do not export end
    /// of the field results if we were forced to split text.
    USHORT m_nFieldResults;

public:
    WW8AttributeOutput( WW8Export &rWW8Export ) : AttributeOutputBase(), m_rWW8Export( rWW8Export ) {}
    virtual ~WW8AttributeOutput() {}

    /// Return the right export class.
    virtual MSWordExportBase& GetExport() { return m_rWW8Export; }

protected:
    /// Output the bold etc. attributes
    void OutputWW8Attribute( BYTE nId, bool bVal );
    
    /// Output the bold etc. attributes, the Complex Text Layout version
    void OutputWW8AttributeCTL( BYTE nId, bool bVal );

};

#endif // _WW8ATTRIBUTEOUTPUT_HXX_

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
