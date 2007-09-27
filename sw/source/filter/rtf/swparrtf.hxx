/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swparrtf.hxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 09:55:31 $
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
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef _SWPARRTF_HXX
#define _SWPARRTF_HXX

#ifndef __SGI_STL_MAP
#include <map>
#endif
#ifndef __SGI_STL_DEQUE
#include <deque>
#endif
#ifndef __SGI_STL_VECTOR
#include <vector>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_BOOLS
#include <svtools/svstdarr.hxx>
#endif
#ifndef _SVXRTF_HXX //autogen
#include <svx/svxrtf.hxx>
#endif
#ifndef _SVX_NUMITEM_HXX
#include <svx/numitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef __SGI_STL_ALGORITHM
#include <algorithm>
#endif

#ifndef _FLTSHELL_HXX
#include <fltshell.hxx>         // fuer den Attribut Stack
#endif

#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif

#ifndef SW_MS_MSFILTER_HXX
#include "../inc/msfilter.hxx"
#endif

#ifndef _SVDOBJ_HXX
#include <svx/svdobj.hxx>
#endif


extern void GetLineIndex(SvxBoxItem &rBox, short nLineThickness, short nSpace, BYTE nCol, short nIdx,
    USHORT nOOIndex, USHORT nWWIndex, short *pSize);

class Font;
class Graphic;

class SwNoTxtNode;
class Size;
class SwPaM;
class SwDoc;
class SwPageDesc;
class SwTxtFmtColl;
class SwTableNode;
class SwCharFmt;
class SwNumRule;
class SwRTFParser;
class SvxFontItem;
class SwRelNumRuleSpaces;
class SwNodeNum;
class SwTxtNode;
struct SvxRTFPictureType;

class SwNodeIdx : public SvxNodeIdx
{
    SwNodeIndex aIdx;
public:
    SwNodeIdx( const SwNodeIndex& rIdx ) : aIdx( rIdx ) {}
    virtual ULONG	GetIdx() const;
    virtual SvxNodeIdx* Clone() const;
};

class BookmarkPosition
{
public:
    SwNodeIndex maMkNode;
    xub_StrLen mnMkCntnt;
    BookmarkPosition(const SwPaM &rPaM);
    BookmarkPosition(const BookmarkPosition &rEntry);

    bool operator==(const BookmarkPosition);
private:
    //No assignment
    BookmarkPosition& operator=(const BookmarkPosition&);

};

class SwxPosition : public SvxPosition
{
    SwPaM* pPam;
public:
    SwxPosition( SwPaM* pCrsr )	: pPam( pCrsr ) {}

    virtual ULONG	GetNodeIdx() const;
    virtual xub_StrLen GetCntIdx() const;

    // erzeuge von sich selbst eine Kopie
    virtual SvxPosition* Clone() const;
    // erzeuge vom NodeIndex eine Kopie
    virtual SvxNodeIdx* MakeNodeIdx() const;
};


// zum zwischenspeichern der Flys:
struct SwFlySave
{
    SfxItemSet 	aFlySet;
    SwNodeIndex	nSttNd, nEndNd;
    xub_StrLen	nEndCnt;
    SwTwips		nPageWidth;
    USHORT 		nDropLines, nDropAnchor;

    SwFlySave( const SwPaM& rPam, SfxItemSet& rSet );
    int IsEqualFly( const SwPaM& rPos, SfxItemSet& rSet );
    void SetFlySize( const SwTableNode& rTblNd );
};

struct SwListEntry
{
    long nListId, nListTemplateId, nListNo;
    USHORT nListDocPos;
    BOOL bRuleUsed;

    SwListEntry()
        : nListId( 0 ), nListTemplateId( 0 ), nListNo( 0 ), nListDocPos( 0 ),
        bRuleUsed( FALSE )
    {}
    SwListEntry( long nLstId, long nLstTmplId, USHORT nLstDocPos )
        : nListId( nLstId ), nListTemplateId( nLstTmplId ), nListNo( 0 ),
        nListDocPos( nLstDocPos ), bRuleUsed( FALSE )
    {}

    void Clear() { nListId = nListTemplateId = nListNo = 0, nListDocPos = 0;
                    bRuleUsed = FALSE; }
};

DECLARE_TABLE( SwRTFStyleTbl, SwTxtFmtColl* )
DECLARE_TABLE( SwRTFCharStyleTbl, SwCharFmt* )
typedef SwFlySave* SwFlySavePtr;
SV_DECL_PTRARR_DEL( SwFlySaveArr, SwFlySavePtr, 0, 20 )
SV_DECL_VARARR( SwListArr, SwListEntry, 0, 20 )

struct DocPageInformation
{
    SvxBoxItem maBox;
    long mnPaperw;
    long mnPaperh;
    long mnMargl;
    long mnMargr;
    long mnMargt;
    long mnMargb;
    long mnGutter;
    long mnPgnStart;
    bool mbFacingp;
    bool mbLandscape;
    bool mbRTLdoc;
    DocPageInformation();
};

struct SectPageInformation
{
    std::vector<long> maColumns;
    SvxBoxItem maBox;
    SvxNumberType maNumType;
    SwPageDesc *mpTitlePageHdFt;
    SwPageDesc *mpPageHdFt;
    long mnPgwsxn;
    long mnPghsxn;
    long mnMarglsxn;
    long mnMargrsxn;
    long mnMargtsxn;
    long mnMargbsxn;
    long mnGutterxsn;
    long mnHeadery;
    long mnFootery;
    long mnPgnStarts;
    long mnCols;
    long mnColsx;
    long mnStextflow;
    int mnBkc;
    bool mbLndscpsxn;
    bool mbTitlepg;
    bool mbFacpgsxn;
    bool mbRTLsection;
    bool mbPgnrestart;
    bool mbTitlePageHdFtUsed;
    bool mbPageHdFtUsed;
    SectPageInformation(const DocPageInformation &rDoc);
    SectPageInformation(const SectPageInformation &rSect);
};

class rtfSection
{
public:
    rtfSection(const SwPosition &rPos,
        const SectPageInformation &rPageInfo);
    SwNodeIndex maStart;
    SectPageInformation maPageInfo;
    SwSection *mpSection;
    SwPageDesc *mpTitlePage;
    SwPageDesc *mpPage;

    bool IsContinous() const { return maPageInfo.mnBkc == 0; }
    long NoCols() const { return maPageInfo.mnCols; }
    long StandardColSeperation() const { return maPageInfo.mnColsx; }
    bool HasTitlePage() const { return maPageInfo.mbTitlepg; }
    long PageStartAt() const { return maPageInfo.mnPgnStarts; }
    bool PageRestartNo() const { return maPageInfo.mbPgnrestart; }
    bool IsBiDi() const { return maPageInfo.mbRTLsection; }
    long GetPageWidth() const { return maPageInfo.mnPgwsxn; }
    long GetPageHeight() const { return maPageInfo.mnPghsxn; }
    long GetPageLeft() const { return maPageInfo.mnMarglsxn; }
    long GetPageRight() const { return maPageInfo.mnMargrsxn; }
    bool IsLandScape() const { return maPageInfo.mbLndscpsxn; }
};

class rtfSections
{
private:
    SwRTFParser &mrReader;
    std::deque<rtfSection> maSegments;
    typedef std::deque<rtfSection>::iterator mySegIter;
    typedef std::deque<rtfSection>::reverse_iterator mySegrIter;

    struct wwULSpaceData
    {
        bool bHasHeader, bHasFooter;
        short nSwHLo, nHdUL, nSwFUp, nFtUL, nSwUp,  nSwLo;
        wwULSpaceData() : bHasHeader(false), bHasFooter(false) {}
    };

    void SetSegmentToPageDesc(const rtfSection &rSection, bool bTitlePage,
        bool bIgnoreCols);
    SwSectionFmt *InsertSection(SwPaM& rMyPaM, rtfSection &rSection);
    void SetPage(SwPageDesc &rInPageDesc, SwFrmFmt &rFmt,
        const rtfSection &rSection, bool bIgnoreCols);
    void GetPageULData(const rtfSection &rSection, bool bFirst,
        wwULSpaceData& rData);
    void SetPageULSpaceItems(SwFrmFmt &rFmt, wwULSpaceData& rData);
    bool SetCols(SwFrmFmt &rFmt, const rtfSection &rSection,
        USHORT nNettoWidth);
    void SetHdFt(rtfSection &rSection);
    void CopyFrom(const SwPageDesc &rFrom, SwPageDesc &rDest);
    void MoveFrom(SwPageDesc &rFrom, SwPageDesc &rDest);
public:
    bool empty() const { return maSegments.empty(); }
    int size() const { return maSegments.size(); }
    void push_back(const rtfSection &rSect);
    void pop_back() { maSegments.pop_back(); }
    rtfSection& back() { return maSegments.back(); }
    const rtfSection& back() const { return maSegments.back(); }
    void InsertSegments(bool bIsNewDoc);
    rtfSections(SwRTFParser &rReader) : mrReader(rReader) {}
    std::vector<sal_uInt16> maDummyPageNos;
    typedef std::vector<sal_uInt16>::reverse_iterator myrDummyIter;
    void PrependedInlineNode(const SwPosition &rPos,
        const SwNode &rNode);
};




class SwRTFParser : public SvxRTFParser
{
    /*
     Knows which writer style a given word style should be imported as.
    */
    sw::util::ParaStyleMapper maParaStyleMapper;
    sw::util::CharStyleMapper maCharStyleMapper;

    std::vector<String> aRevTbl;

    friend class rtfSections;
    DocPageInformation maPageDefaults;
    rtfSections maSegments;

    sw::util::InsertedTablesManager maInsertedTables;
    SwRTFStyleTbl aTxtCollTbl;
    SwRTFCharStyleTbl aCharFmtTbl;
    SwFlySaveArr aFlyArr;				// Flys als Letzes im Doc setzen
    SvBools aMergeBoxes;				// Flags fuer gemergte Zellen
    SwListArr aListArr;
    SvPtrarr aTblFmts;
    SvPtrarr aRubyCharFmts;
    BookmarkPosition* mpBookmarkStart;
    sw::util::RedlineStack *mpRedlineStack;
    sw::util::AuthorInfos* pAuthorInfos;

    SfxItemSet* pGrfAttrSet;
    SwTableNode* pTableNode, *pOldTblNd; // fuers Lesen von Tabellen: akt. Tab
    SwNodeIndex* pSttNdIdx;
    SwNodeIndex* pRegionEndIdx;
    SwDoc* 	pDoc;
    SwPaM*	pPam;				// SwPosition duerfte doch reichen, oder ??
    SwRelNumRuleSpaces* pRelNumRule;	// Liste aller benannten NumRules

    String sNestedFieldStr;
    SwFltRedline *pRedlineInsert;
    SwFltRedline *pRedlineDelete;

    String sBaseURL;

    USHORT nAktPageDesc, nAktFirstPageDesc;
    USHORT nAktBox;			// akt. Box
    USHORT nInsTblRow;		// beim nach \row kein \pard -> neue Line anlegen
    USHORT nNewNumSectDef;	// jeder SectionWechsel kann neue Rules definieren
    USHORT nRowsToRepeat;

    bool bSwPageDesc;
    bool bReadSwFly;		// lese Swg-Fly (wichtig fuer Bitmaps!)
    bool mbReadNoTbl;		// verhinder Tabelle in Tabelle/FootNote
    bool mbIsFootnote;
    bool bFootnoteAutoNum;	// automatische Numerierung ?
    bool bStyleTabValid;	// Styles schon erzeugt ?
    bool bInPgDscTbl;		// beim PageDescTbl lesen
    bool bNewNumList;		// Word 7.0 NumList gelesen, 6.0 ueberspringen
    bool bFirstContinue;	// 1.Call ins Continue
    bool bContainsPara;     // If there is no paragraph in the section
    bool bContainsTablePara;     // If there is an table in this section
    bool bForceNewTable;     // Forces a beginning of a new table
    bool bNestedField;
    bool bTrowdRead;         // True, iff an \trowd definition was read after the last \row

    int nZOrder;
    /*
     #i9243#
     In a footnote tables are not possible (for some obscure reason!)
    */
    bool CantUseTables() const { return mbReadNoTbl || mbIsFootnote; }

    virtual void InsertPara();
    virtual void InsertText();
    virtual void MovePos( int bForward = TRUE );
    virtual void SetEndPrevPara( SvxNodeIdx*& rpNodePos, xub_StrLen& rCntPos );
    void EnterEnvironment();
    void LeaveEnvironment();

    SwFmtPageDesc* GetCurrentPageDesc(SwPaM *pPam);
    void CheckInsNewTblLine();

    // setze RTF-Werte auf Swg-Werte
    void SetSwgValues( SfxItemSet& rSet );

    virtual void ReadInfo( const sal_Char* pChkForVerNo = 0 );

    void ReadUserProperties();

    void ReadListLevel( SwNumRule& rRule, BYTE nLvl );
    void SetBorderLine(SvxBoxItem& rBox, sal_uInt16 nLine);
    void ReadListTable();
    USHORT ReadRevTbl();
    void ReadShpRslt();
    void ReadShpTxt(String &shpTxt);
    void ReadDrawingObject();
    void InsertShpObject(SdrObject* pStroke, int nZOrder);
    void ReadShapeObject();
    void ReadListOverrideTable();
    SwNumRule *ReadNumSecLevel( int nToken );
    SwNumRule* GetNumRuleOfListNo( long nListNo,
                                    BOOL bRemoveFromList = FALSE );
    void RemoveUnusedNumRule( SwNumRule* );
    void RemoveUnusedNumRules();
    const Font* FindFontOfItem( const SvxFontItem& rItem ) const;

    // 3 Methoden zum Aufbauen der Styles
    SwTxtFmtColl* MakeColl( const String&, USHORT nPos, BYTE nOutlineLevel,
                            bool& rbCollExist );
    SwCharFmt* MakeCharFmt( const String& rName, USHORT nPos,
                            int& rbCollExist );
    void SetStyleAttr( SfxItemSet& rCollSet,
                        const SfxItemSet& rStyleSet,
                        const SfxItemSet& rDerivedSet );
    SwTxtFmtColl* MakeStyle( USHORT nNo, const SvxRTFStyleType& rStyle );
    SwCharFmt* MakeCharStyle( USHORT nNo, const SvxRTFStyleType& rStyle );
    void MakeStyleTab();

    int MakeFieldInst( String& rFieldStr );

    // einlesen/einfuegen von Bitmaps
    void InsPicture( const String& rNm,
                    const Graphic* = 0, const SvxRTFPictureType* = 0 );
    void _SetPictureSize( const SwNoTxtNode& rNd, const SwNodeIndex& rAnchor,
                            SfxItemSet& rSet, const SvxRTFPictureType* = 0  );

    void SetFlysInDoc();
    void GetPageSize( Size& rSize );		// Groesse der PagePrintArea

    // fuers Einlesen von Tabellen
    void GotoNextBox();
    void NewTblLine();

    void DelLastNode();			// loesche den letzten Node (Tabelle/Fly/Ftn/..)

    void AddNumRule( SwNumRule* pRule );
    void SetNumRelSpaces();
    void SetOultineRelSpaces( const SwNodeIndex& rStt,
                                const SwNodeIndex& rEnd );
    void SetNumLSpace( SwTxtNode& rNd, const SwNodeNum& rNum,
                                const SwNumRule& rRule );
    long GetSafePos(long nPos);

protected:
    // wird fuer jedes Token gerufen, das in CallParser erkannt wird
    virtual void NextToken( int nToken );

//	virtual void ReadUnknownData();
    virtual void ReadBitmapData();
#ifdef READ_OLE_OBJECT
    virtual void ReadOLEData();
#endif
    virtual	void SetAttrInDoc( SvxRTFItemStackType &rSet );
    virtual bool UncompressableStackEntry(const SvxRTFItemStackType &rSet) const;
    // fuer Tokens, die im ReadAttr nicht ausgewertet werden
    virtual void UnknownAttrToken( int nToken, SfxItemSet* pSet );

    void ReadPrtData();
    void ReadField();
    void ReadXEField();
    void ReadHeaderFooter( int nToken, SwPageDesc* pPageDesc = 0 );
    void ReadDocControls( int nToken );
    void SetPageInformationAsDefault(const DocPageInformation &rInfo);
    void ReadSectControls( int nToken );
    void DoHairyWriterPageDesc(int nToken);
    void ReadFly( int nToken, SfxItemSet* pFillSet = 0 );
    void ReadTable( int nToken );
    void ReadPageDescTbl();
    void SkipPageDescTbl();
    bool IsBorderToken(int nToken);

    const String& GetBaseURL() const { return sBaseURL;}

    virtual ~SwRTFParser();

public:
    SwRTFParser( SwDoc* pD, const SwPaM& rCrsr, SvStream& rIn,
                        const String& rBaseURL,
                        int bReadNewDoc = TRUE );

    virtual SvParserState CallParser();	// Aufruf des Parsers
    virtual int IsEndPara( SvxNodeIdx* pNd, xub_StrLen nCnt ) const;

    // fuers asynchrone lesen aus dem SvStream
//	virtual void SaveState( int nToken );
//	virtual void RestoreState();
    virtual void Continue( int nToken );
};


#endif

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
