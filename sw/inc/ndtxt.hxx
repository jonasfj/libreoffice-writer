/*************************************************************************
 *
 *  $RCSfile: ndtxt.hxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: rt $ $Date: 2004-10-22 08:09:31 $
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
#ifndef _NDTXT_HXX
#define _NDTXT_HXX

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _NDHINTS_HXX
#include <ndhints.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

namespace utl {
    class TransliterationWrapper;
};

class SwNumRules;
class SwTxtFmtColl;
class SwCntntFrm;
class SwTxtFld; 		 // Fuer GetTxtFld()
class SwAttrSet;
class SwUndoTransliterate;


struct SwSpellArgs;             // for Spell(), splargs.hxx
struct SwConversionArgs;        // for Convert(), splargs.hxx
class SwInterHyphInfo;          // for Hyphenate(), splargs.hxx
class SwWrongList;      // fuer OnlineSpelling
class SwNodeNum;
class OutputDevice;
class SwScriptInfo;
struct SwDocStat;

// Konstanten fuer das Text-Insert:
#define INS_DEFAULT         0x0000 // keine Extras
#define INS_EMPTYEXPAND     0x0001 // leere Hints beim Einfuegen aufspannen
#define INS_NOHINTEXPAND    0x0002 // Hints an der InsPos nicht aufspannen

namespace com { namespace sun { namespace star { namespace uno {
    template < class > class Sequence;
}}}}

// --------------------
// SwTxtNode
// --------------------
class SW_DLLPUBLIC SwTxtNode: public SwCntntNode
{
    // fuer das Erzeugen des ersten TextNode
    friend class SwDoc; 		// CTOR und AppendTxtNode()
    friend class SwNodes;
    friend class SwTxtFrm;
    friend class SwScriptInfo;
    friend void SwpHints::Insert( SwTxtAttr*, SwTxtNode&, USHORT );

    //Kann 0 sein, nur dann nicht 0 wenn harte Attribute drin stehen.
    //Also niemals direkt zugreifen!
    SwpHints	*pSwpHints;
    SwWrongList	*pWrong;	// Rechtschreibfehler nach Online-Spelling
    SwNodeNum	*pNdNum;	// Numerierung fuer diesen Absatz
    XubString	aText;

    // Some of the chars this para are hidden. Paragraph has to be reformatted
    // on changing the view to print preview.
    mutable BOOL bContainsHiddenChars : 1;
    // The whole paragraph is hidden because of the hidden text attribute
    mutable BOOL bHiddenCharsHidePara : 1;
    // The last two flags have to be recalculated if this flag is set:
    mutable BOOL bRecalcHiddenCharFlags : 1;

    SW_DLLPRIVATE SwTxtNode( const SwNodeIndex &rWhere, SwTxtFmtColl *pTxtColl,
                SwAttrSet* pAutoAttr = 0 );

    // Kopiert die Attribute an nStart nach pDest.
    SW_DLLPRIVATE void CopyAttr( SwTxtNode *pDest, const xub_StrLen nStart, const xub_StrLen nOldPos);

    SW_DLLPRIVATE SwTxtNode* _MakeNewTxtNode( const SwNodeIndex&, BOOL bNext = TRUE,
                                BOOL bChgFollow = TRUE );

    SW_DLLPRIVATE void	_Cut( SwTxtNode *pDest, const SwIndex &rDestStart,
                  const SwIndex &rStart, xub_StrLen nLen, BOOL bUpdate = TRUE );

    SW_DLLPRIVATE SwTxtAttr* MakeTxtAttr( const SfxPoolItem& rNew, xub_StrLen nStt, xub_StrLen nEnd,
                            BOOL bPool = TRUE );

    // Verlagere alles umfassende harte Attribute in den AttrSet des Absatzes
    SW_DLLPRIVATE void MoveTxtAttr_To_AttrSet();  // wird von SplitNode gerufen.

    // lege den spz. AttrSet an
    SW_DLLPRIVATE virtual void NewAttrSet( SwAttrPool& );

    SW_DLLPRIVATE void Replace0xFF( XubString& rTxt, xub_StrLen& rTxtStt,
                        xub_StrLen nEndPos, BOOL bExpandFlds ) const;

    // Optimization: Asking for information about hidden characters at SwScriptInfo
    // updates these flags.
    inline bool IsCalcHiddenCharFlags() const { return bRecalcHiddenCharFlags; }
    inline void SetHiddenCharAttribute( bool bNewHiddenCharsHidePara, bool bNewContainsHiddenChars ) const
    {
        bHiddenCharsHidePara = bNewHiddenCharsHidePara;
        bContainsHiddenChars = bNewContainsHiddenChars;
        bRecalcHiddenCharFlags = false;
    }

    SW_DLLPRIVATE void CalcHiddenCharFlags() const;

public:
    const String& GetTxt() const { return aText; }

    // Zugriff auf SwpHints
    inline 		 SwpHints &GetSwpHints();
    inline const SwpHints &GetSwpHints() const;
    inline 		 SwpHints *GetpSwpHints() { return pSwpHints; }
    inline const SwpHints *GetpSwpHints() const { return pSwpHints; }
    void 		 SetWrong( SwWrongList *pNew );
    inline 		 SwWrongList *GetWrong() { return pWrong; }
    inline const SwWrongList *GetWrong() const { return pWrong; }
    inline BOOL  HasHints() const { return pSwpHints ? TRUE : FALSE; }
    inline 		 SwpHints &GetOrCreateSwpHints();

    virtual ~SwTxtNode();

    virtual xub_StrLen Len() const;

    // steht in itratr
    void GetMinMaxSize( ULONG nIndex, ULONG& rMin, ULONG &rMax, ULONG &rAbs,
                        OutputDevice* pOut = 0 ) const;

    /*
     * Einfuegen anderer Datentypen durch Erzeugen eines
     * temporaeren Strings.
     */
    SwTxtNode&	Insert( xub_Unicode c, const SwIndex &rIdx );
    SwTxtNode&	Insert( const XubString &rStr, const SwIndex &rIdx,
                        const USHORT nMode = INS_DEFAULT );

    SwTxtNode&	Erase( const SwIndex &rIdx, xub_StrLen nCount = STRING_LEN,
                       const USHORT nMode = INS_DEFAULT );

    // Aktionen auf Attributen
    // loesche alle TextAttribute die als Attribut im Set vorhanden sind
    // (Set-Pointer != 0 ) oder alle deren Which-Wert mit nWhich mappen
    // oder wenn Which = 0, alle.
    void	RstAttr( const SwIndex &rIdx, xub_StrLen nLen, USHORT nWhich = 0,
                    const SfxItemSet* pSet = 0, BOOL bInclRefToxMark = FALSE );
    void	GCAttr();

    // loesche das Text-Attribut (muss beim Pool abgemeldet werden!)
    void 	DestroyAttr( SwTxtAttr* pAttr );

    // loesche alle Attribute aus dem SwpHintsArray.
    void 	ClearSwpHintsArr( int bDelAll = TRUE, int bDelFields = TRUE );

    // uebernehme den Pointer auf das Text-Attribut
    BOOL	Insert( SwTxtAttr *pAttr, USHORT nMode = 0 );
    // lege ein neues TextAttribut an und fuege es ins SwpHints-Array ein
    // returne den neuen Pointer (oder 0 bei Fehlern)!
    SwTxtAttr* Insert( const SfxPoolItem& rAttr,
                        xub_StrLen nStt, xub_StrLen nEnd, USHORT nMode = 0 );

    // setze diese Attribute am TextNode. Wird der gesamte Bereich umspannt,
    // dann setze sie nur im AutoAttrSet (SwCntntNode:: SetAttr)
    BOOL SetAttr( const SfxItemSet& rSet,
                    xub_StrLen nStt, xub_StrLen nEnd, USHORT nMode = 0 );
    // erfrage die Attribute vom TextNode ueber den Bereich
    BOOL GetAttr( SfxItemSet& rSet, xub_StrLen nStt, xub_StrLen nEnd,
                    BOOL bOnlyTxtAttr  = FALSE,
                    BOOL bGetFromChrFmt = TRUE ) const;

    // uebertrage Attribute eines AttrSets ( AutoFmt ) in das SwpHintsArray
    void FmtToTxtAttr( SwTxtNode* pNd );

    // loeschen eines einzelnen Attributes (fuer SwUndoAttr)
    // ( nur das Attribut loeschen, dass mit Which,Start und End oder
    //   mit pTxtHint identisch ist (es gibt nur ein gueltiges))
    // 	AUSNAHME: ToxMarks !!!
    void 	Delete( USHORT nTxtWhich, xub_StrLen nStart, xub_StrLen nEnd = 0 );
    void 	Delete( SwTxtAttr * pTxtAttr, BOOL bThisOnly = FALSE );

    // Aktionen auf Text und Attributen
    void	Copy(SwTxtNode *pDest, const SwIndex &rStart, USHORT nLen);
    void	Copy(SwTxtNode *pDest, const SwIndex &rDestStart,
                const SwIndex &rStart, xub_StrLen nLen);
    void	Cut(SwTxtNode *pDest, const SwIndex &rStart, xub_StrLen nLen);
    inline void	Cut(SwTxtNode *pDest, const SwIndex &rDestStart,
                    const SwIndex &rStart, xub_StrLen nLen);
    // ersetze im String an Position nIdx das Zeichen
    void Replace( const SwIndex& rStart, xub_Unicode cCh );
    void Replace( const SwIndex& rStart, xub_StrLen nLen, const XubString& rText );
    void ReplaceTextOnly( xub_StrLen nPos, xub_StrLen nLen, const XubString& rText,
                    const ::com::sun::star::uno::Sequence<long>& rOffsets );

    // virtuelle Methoden aus dem CntntNode
    virtual SwCntntFrm *MakeFrm();
    virtual SwCntntNode *SplitNode( const SwPosition & );
    virtual SwCntntNode *JoinNext();
    virtual SwCntntNode *JoinPrev();

    SwCntntNode *AppendNode( const SwPosition & );

    // setze ggf. das DontExpand-Flag an INet bzw. Zeichenvorlagen
    BOOL DontExpandFmt( const SwIndex& rIdx, BOOL bFlag = TRUE,
                        BOOL bFmtToTxtAttributes = TRUE );

    // gebe das vorgegebene Attribut, welches an der TextPosition (rIdx)
    // gesetzt ist zurueck. Gibt es keines, returne 0-Pointer
    // gesetzt heisst: Start <= rIdx < End
    SwTxtAttr *GetTxtAttr( const SwIndex& rIdx, USHORT nWhichHt,
                           BOOL bExpand = FALSE ) const;

    // Diese Methode liefert nur Textattribute auf der Position nIdx
    // zurueck, die kein EndIdx besitzen und denselben Which besitzen.
    // Ueblicherweise steht an dieser Position ein CH_TXTATR.
    // Bei RES_TXTATR_END entfaellt die Pruefung auf den Which-Wert.
    SwTxtAttr *GetTxtAttr( const xub_StrLen nIdx,
                           const USHORT nWhichHt = RES_TXTATR_END ) const;

    SwTxtFld  *GetTxtFld( const SwIndex& rIdx )
        { return (SwTxtFld *)GetTxtAttr( rIdx, RES_TXTATR_FIELD ); }

    // Aktuelles Wort zurueckliefern
    XubString GetCurWord(xub_StrLen);
    USHORT Spell(SwSpellArgs*);
    USHORT Convert( SwConversionArgs & );

    inline SwTxtFmtColl *GetTxtColl() const;
    virtual SwFmtColl *ChgFmtColl( SwFmtColl* );
    void _ChgTxtCollUpdateNum( const SwTxtFmtColl* pOld,
                                const SwTxtFmtColl* pNew );

    // kopiere die Collection mit allen Autoformaten zum Dest-Node
    // dieser kann auch in einem anderen Dokument stehen!
    // (Methode steht im ndcopy.cxx!!)
    void CopyCollFmt( SwTxtNode& rDestNd );

    // Numerierung
    const SwNodeNum* UpdateNum( const SwNodeNum& );
    SwNumRule *GetNumRule() const;
    const SwNodeNum* GetNum() const; // #i29363#
    const SwNodeNum* GetOutlineNum() const;
    BOOL MayBeNumbered() const; // #i23730#
    void NumRuleChgd();					// Frames benachrichtigen
    XubString GetNumString() const;		// returnt Outline oder Num - String
    long GetLeftMarginWithNum( BOOL bTxtLeft = FALSE ) const;
    BOOL GetFirstLineOfsWithNum( short& rFirstOffset ) const;

#ifdef VERTICAL_LAYOUT
    USHORT GetLang( const xub_StrLen nBegin, const xub_StrLen nLen = 0,
                    USHORT nScript = 0 ) const;
#else
    USHORT GetLang( const xub_StrLen nBegin, const xub_StrLen nLen = 0) const;
#endif
        // steht in ndcopy.cxx
    BOOL IsSymbol( const xub_StrLen nBegin ) const;	// steht in itratr.cxx
    virtual SwCntntNode* MakeCopy( SwDoc*, const SwNodeIndex& ) const;

    // interaktive Trennung: wir finden den TxtFrm und rufen sein CalcHyph
    BOOL Hyphenate( SwInterHyphInfo &rHyphInf );
    void DelSoftHyph( const xub_StrLen nStart, const xub_StrLen nEnd );

    // Liefert einen String mit expandierten Feldern zurueck
    //   opt. die Kapitel/Nummer-String zurueck
    XubString GetExpandTxt( const xub_StrLen nIdx = 0,
                         const xub_StrLen nLen = STRING_LEN,
                         const BOOL bWithNum = FALSE ) const;
    BOOL GetExpandTxt( SwTxtNode& rDestNd, const SwIndex* pDestIdx = 0,
                        xub_StrLen nIdx = 0, xub_StrLen nLen = STRING_LEN,
                       BOOL bWithNum = FALSE, BOOL bWithFtn = TRUE,
                       BOOL bReplaceTabsWithSpaces = FALSE ) const;
    XubString GetRedlineTxt( xub_StrLen nIdx = 0,
                          xub_StrLen nLen = STRING_LEN,
                          BOOL bExpandFlds = FALSE,
                          BOOL bWithNum = FALSE ) const;
    //Liefert fuer die Initalfunktion tatsaechliche Anzahl der Initialzeichen
    //bei nWishLen == 0 die des ersten Wortes
    USHORT GetDropLen( USHORT nWishLen) const;

    // Passes back info needed on the dropcap dimensions
    bool GetDropSize(int& rFontHeight, int& rDropHeight, int& rDropDescent) const;


    //
    // Hidden Paragraph Field:
    //
    inline BOOL CalcHiddenParaField()
        { if(pSwpHints) return pSwpHints->CalcHiddenParaField(); return FALSE; }
    // Setzen des CalcVisible-Flags
    inline void SetCalcHiddenParaField(){ if(pSwpHints) pSwpHints->SetCalcHiddenParaField(); }

    // Ist der Absatz sichtbar
    inline BOOL HasHiddenParaField() const
        { return pSwpHints ? pSwpHints->HasHiddenParaField() : FALSE; }

    //
    // Hidden Paragraph Field:
    //
    inline bool HasHiddenCharAttribute( bool bWholePara ) const
    {
        if ( bRecalcHiddenCharFlags )
            CalcHiddenCharFlags();
        return bWholePara ? bHiddenCharsHidePara : bContainsHiddenChars;
    }
    inline void SetCalcHiddenCharFlags() const { bRecalcHiddenCharFlags = TRUE; }

// --> FME 2004-06-08 #i12836# enhanced pdf
    //
    // Returns if the node is hidden due to
    // 1. HiddenParaField
    // 2. HiddenCharAttribute
    // 3. HiddenSection
    //
    bool IsHidden() const;
// <--

    //
    // Footnotes:
    //
    inline BOOL HasFtn() const {return pSwpHints ? pSwpHints->HasFtn() : FALSE;}

    inline SwTxtAttr* MakeTmpTxtAttr( const SfxPoolItem& rNew )
        { return MakeTxtAttr( rNew, 0, 0, FALSE ); }

    TYPEINFO();	// fuer rtti

    // fuers Umhaengen der TxtFmtCollections (Outline-Nummerierung!!)
    virtual void Modify( SfxPoolItem*, SfxPoolItem* );
//	virtual BOOL GetInfo( SfxPoolItem& ) const;

    // aus SwIndexReg
    virtual void Update( const SwIndex & aPos, USHORT xub_StrLen,
                         BOOL bNegativ = FALSE, BOOL bDelete = FALSE );

    // change text to Upper/Lower/Hiragana/Katagana/...
    void TransliterateText( utl::TransliterationWrapper& rTrans,
                            xub_StrLen nStart, xub_StrLen nEnd,
                            SwUndoTransliterate* pUndo = 0 );

    // count words in given range
    void CountWords( SwDocStat& rStat, xub_StrLen nStart, xub_StrLen nEnd ) const;

    // #111840#
    /**
       Returns position of certain text attribute.

       @param pAttr     text attribute to search

       @return position of given attribute or NULL in case of failure
     */
    SwPosition * GetPosition(const SwTxtAttr * pAttr);

    // -> #i29560
    BOOL HasNumber() const;
    BOOL HasBullet() const;
    // <- #i29560

    USHORT GetScalingOfSelectedText( xub_StrLen nStt, xub_StrLen nEnd ) const;

    // -> #i27615#
    BOOL IsNumbered() const;
    BOOL HasMarkedLabel() const;
    // <- #i27615#

    USHORT GetWidthOfLeadingTabs() const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwTxtNode)
};

//-----------------------------------------------------------------------------

inline SwpHints	&SwTxtNode::GetSwpHints()
{
    ASSERT_ID( pSwpHints, ERR_NOHINTS);
    return *pSwpHints;
}
inline const SwpHints &SwTxtNode::GetSwpHints() const
{
    ASSERT_ID( pSwpHints, ERR_NOHINTS);
    return *pSwpHints;
}

inline SwpHints& SwTxtNode::GetOrCreateSwpHints()
{
    if( !pSwpHints )
        pSwpHints = new SwpHints;
    return *pSwpHints;
}

inline SwTxtFmtColl* SwTxtNode::GetTxtColl() const
{
    return (SwTxtFmtColl*)GetRegisteredIn();
}

// fuer den IBM-Compiler nicht inlinen wg. 42876
#ifndef ICC
// Inline Metoden aus Node.hxx - erst hier ist der TxtNode bekannt !!
inline       SwTxtNode   *SwNode::GetTxtNode()
{
     return ND_TEXTNODE == nNodeType ? (SwTxtNode*)this : 0;
}
inline const SwTxtNode   *SwNode::GetTxtNode() const
{
     return ND_TEXTNODE == nNodeType ? (const SwTxtNode*)this : 0;
}
#endif

inline void	SwTxtNode::Cut(SwTxtNode *pDest, const SwIndex &rDestStart,
                            const SwIndex &rStart, xub_StrLen nLen)
{
    _Cut( pDest, rDestStart, rStart, nLen, TRUE );
}


#endif
