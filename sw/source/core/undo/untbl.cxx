/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: untbl.cxx,v $
 * $Revision: 1.41 $
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
#include <svx/brkitem.hxx>
#include <fmtornt.hxx>
#include <fmtpdsc.hxx>
#include <doc.hxx>
#include <editsh.hxx>
#include <docary.hxx>
#include <ndtxt.hxx>
#include <swtable.hxx>
#include <pam.hxx>
#include <cntfrm.hxx>
#include <tblsel.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <undobj.hxx>
#include <rolbck.hxx>
#include <ddefld.hxx>
#include <tabcol.hxx>
#include <tabfrm.hxx>
#include <rowfrm.hxx>
#include <cellfrm.hxx>
#include <swcache.hxx>
#include <tblafmt.hxx>
#include <poolfmt.hxx>
#include <mvsave.hxx>
#include <cellatr.hxx>
#include <swtblfmt.hxx>
#include <swddetbl.hxx>
#include <redline.hxx>
#include <node2lay.hxx>
#include <tblrwcl.hxx>
#include <fmtanchr.hxx>
#include <comcore.hrc>
#include <unochart.hxx>

#ifndef DBG_UTIL
#define CHECK_TABLE(t)
#else
#ifdef DEBUG
#define CHECK_TABLE(t) (t).CheckConsistency();
#else
#define CHECK_TABLE(t)
#endif
#endif

#ifndef DBG_UTIL
    #define _DEBUG_REDLINE( pDoc )
#else
    void lcl_DebugRedline( const SwDoc* pDoc );
    #define _DEBUG_REDLINE( pDoc ) lcl_DebugRedline( pDoc );
#endif

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }
extern void ClearFEShellTabCols();

typedef SfxItemSet* SfxItemSetPtr;
SV_DECL_PTRARR_DEL( SfxItemSets, SfxItemSetPtr, 10, 5 )

typedef SwUndoSaveSection* SwUndoSaveSectionPtr;
SV_DECL_PTRARR_DEL( SwUndoSaveSections, SwUndoSaveSectionPtr, 0, 10 )

typedef SwUndoMove* SwUndoMovePtr;
SV_DECL_PTRARR_DEL( SwUndoMoves, SwUndoMovePtr, 0, 10 )

struct SwTblToTxtSave;
typedef SwTblToTxtSave* SwTblToTxtSavePtr;
SV_DECL_PTRARR_DEL( SwTblToTxtSaves, SwTblToTxtSavePtr, 0, 10 )

struct _UndoTblCpyTbl_Entry
{
    ULONG nBoxIdx, nOffset;
    SfxItemSet* pBoxNumAttr;
    SwUndo* pUndo;

    // Was the last paragraph of the new and the first paragraph of the old content joined?
    bool bJoin; // For redlining only

    _UndoTblCpyTbl_Entry( const SwTableBox& rBox );
    ~_UndoTblCpyTbl_Entry();
};
typedef _UndoTblCpyTbl_Entry* _UndoTblCpyTbl_EntryPtr;
SV_DECL_PTRARR_DEL( _UndoTblCpyTbl_Entries, _UndoTblCpyTbl_EntryPtr, 0, 10 )

class _SaveBox;
class _SaveLine;

class _SaveTable
{
    friend class _SaveBox;
    friend class _SaveLine;
    SfxItemSet aTblSet;
    _SaveLine* pLine;
    const SwTable* pSwTable;
    SfxItemSets aSets;
    SwFrmFmts aFrmFmts;
    USHORT nLineCount;
    BOOL bModifyBox : 1;
    BOOL bSaveFormula : 1;
    BOOL bNewModel : 1;

public:
    _SaveTable( const SwTable& rTbl, USHORT nLnCnt = USHRT_MAX,
                BOOL bSaveFml = TRUE );
    ~_SaveTable();

    USHORT AddFmt( SwFrmFmt* pFmt, bool bIsLine );
    void NewFrmFmt( const SwClient* pLnBx, BOOL bIsLine, USHORT nFmtPos,
                    SwFrmFmt* pOldFmt );

    void RestoreAttr( SwTable& rTbl, BOOL bModifyBox = FALSE );
    void SaveCntntAttrs( SwDoc* pDoc );
    void CreateNew( SwTable& rTbl, BOOL bCreateFrms = TRUE,
                    BOOL bRestoreChart = TRUE );
    BOOL IsNewModel() const { return bNewModel; }
};

class _SaveLine
{
    friend class _SaveTable;
    friend class _SaveBox;

    _SaveLine* pNext;
    _SaveBox* pBox;
    USHORT nItemSet;

public:

    _SaveLine( _SaveLine* pPrev, const SwTableLine& rLine, _SaveTable& rSTbl );
    ~_SaveLine();

    void RestoreAttr( SwTableLine& rLine, _SaveTable& rSTbl );
    void SaveCntntAttrs( SwDoc* pDoc );

    void CreateNew( SwTable& rTbl, SwTableBox& rParent, _SaveTable& rSTbl  );
};

class _SaveBox
{
    friend class _SaveLine;

    _SaveBox* pNext;
    ULONG nSttNode;
    long nRowSpan;
    USHORT nItemSet;
    union
    {
        SfxItemSets* pCntntAttrs;
        _SaveLine* pLine;
    } Ptrs;

public:
    _SaveBox( _SaveBox* pPrev, const SwTableBox& rBox, _SaveTable& rSTbl );
    ~_SaveBox();

    void RestoreAttr( SwTableBox& rBox, _SaveTable& rSTbl );
    void SaveCntntAttrs( SwDoc* pDoc );

    void CreateNew( SwTable& rTbl, SwTableLine& rParent, _SaveTable& rSTbl );
};

void InsertSort( SvUShorts& rArr, USHORT nIdx, USHORT* pInsPos = 0 );
void InsertSort( SvULongs& rArr, ULONG nIdx, USHORT* pInsPos = 0 );

#if defined( JP_DEBUG ) && defined(DBG_UTIL)
#include "shellio.hxx"
void DumpDoc( SwDoc* pDoc, const String& rFileNm );
void CheckTable( const SwTable& );
#define DUMPDOC(p,s)	DumpDoc( p, s);
#define CHECKTABLE(t) CheckTable( t );
#else
#define DUMPDOC(p,s)
#define CHECKTABLE(t)
#endif

/* #130880: Crash in undo of table to text when the table has (freshly) merged cells
The order of cell content nodes in the nodes array is not given by the recursive table structure.
The algorithmn must not rely on this even it holds for a fresh loaded table in odt file format.
So we need to remember not only the start node position but the end node position as well.
*/

struct SwTblToTxtSave
{
    ULONG m_nSttNd;
    ULONG m_nEndNd;
    xub_StrLen m_nCntnt;
    SwHistory* m_pHstry;
    // metadata references for first and last paragraph in cell
    ::boost::shared_ptr< ::sfx2::MetadatableUndo > m_pMetadataUndoStart;
    ::boost::shared_ptr< ::sfx2::MetadatableUndo > m_pMetadataUndoEnd;

    SwTblToTxtSave( SwDoc& rDoc, ULONG nNd, ULONG nEndIdx, xub_StrLen nCntnt );
    ~SwTblToTxtSave() { delete m_pHstry; }
};

SV_IMPL_PTRARR( SfxItemSets, SfxItemSetPtr )
SV_IMPL_PTRARR( SwUndoSaveSections, SwUndoSaveSectionPtr )
SV_IMPL_PTRARR( SwUndoMoves, SwUndoMovePtr )
SV_IMPL_PTRARR( SwTblToTxtSaves, SwTblToTxtSavePtr )
SV_IMPL_PTRARR( _UndoTblCpyTbl_Entries, _UndoTblCpyTbl_EntryPtr )

USHORT __FAR_DATA aSave_BoxCntntSet[] = {
    RES_CHRATR_COLOR, RES_CHRATR_CROSSEDOUT,
    RES_CHRATR_FONT, RES_CHRATR_FONTSIZE,
    RES_CHRATR_POSTURE,	RES_CHRATR_POSTURE,
    RES_CHRATR_SHADOWED, RES_CHRATR_WEIGHT,
    RES_PARATR_ADJUST, RES_PARATR_ADJUST,
    0 };



SwUndoInsTbl::SwUndoInsTbl( const SwPosition& rPos, USHORT nCl, USHORT nRw,
                            USHORT nAdj, const SwInsertTableOptions& rInsTblOpts,
                            const SwTableAutoFmt* pTAFmt,
                            const SvUShorts* pColArr,
                            const String & rName)
    : SwUndo( UNDO_INSTABLE ),
    aInsTblOpts( rInsTblOpts ), pDDEFldType( 0 ), pColWidth( 0 ), pRedlData( 0 ), pAutoFmt( 0 ),
    nSttNode( rPos.nNode.GetIndex() ), nRows( nRw ), nCols( nCl ), nAdjust( nAdj )
{
    if( pColArr )
    {
        pColWidth = new SvUShorts( 0, 1 );
        pColWidth->Insert( pColArr, 0 );
    }
    if( pTAFmt )
        pAutoFmt = new SwTableAutoFmt( *pTAFmt );

    // Redline beachten
    SwDoc& rDoc = *rPos.nNode.GetNode().GetDoc();
    if( rDoc.IsRedlineOn() )
    {
        pRedlData = new SwRedlineData( nsRedlineType_t::REDLINE_INSERT, rDoc.GetRedlineAuthor() );
        SetRedlineMode( rDoc.GetRedlineMode() );
    }

    sTblNm = rName;
}


SwUndoInsTbl::~SwUndoInsTbl()
{
    delete pDDEFldType;
    delete pColWidth;
    delete pRedlData;
    delete pAutoFmt;
}

void SwUndoInsTbl::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwNodeIndex aIdx( rDoc.GetNodes(), nSttNode );

    SwTableNode* pTblNd = aIdx.GetNode().GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );
    pTblNd->DelFrms();

    if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        rDoc.DeleteRedline( *pTblNd, true, USHRT_MAX );
    RemoveIdxFromSection( rDoc, nSttNode );

    // harte SeitenUmbrueche am nachfolgenden Node verschieben
    SwCntntNode* pNextNd = rDoc.GetNodes()[ pTblNd->EndOfSectionIndex()+1 ]->GetCntntNode();
    if( pNextNd )
    {
        SwFrmFmt* pTableFmt = pTblNd->GetTable().GetFrmFmt();
        const SfxPoolItem *pItem;

        if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_PAGEDESC,
            FALSE, &pItem ) )
            pNextNd->SetAttr( *pItem );

        if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_BREAK,
            FALSE, &pItem ) )
            pNextNd->SetAttr( *pItem );
    }


    sTblNm = pTblNd->GetTable().GetFrmFmt()->GetName();
    if( pTblNd->GetTable().IsA( TYPE( SwDDETable )) )
        pDDEFldType = (SwDDEFieldType*)((SwDDETable&)pTblNd->GetTable()).
                                        GetDDEFldType()->Copy();

    rDoc.GetNodes().Delete( aIdx, pTblNd->EndOfSectionIndex() -
                                aIdx.GetIndex() + 1 );

    rUndoIter.pAktPam->DeleteMark();
    rUndoIter.pAktPam->GetPoint()->nNode = aIdx;
    rUndoIter.pAktPam->GetPoint()->nContent.Assign(
                            rUndoIter.pAktPam->GetCntntNode(), 0 );
}


void SwUndoInsTbl::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    SwPosition aPos( *rUndoIter.pAktPam->GetPoint() );
    aPos.nNode = nSttNode;
    const SwTable* pTbl = rDoc.InsertTable( aInsTblOpts, aPos, nRows, nCols,
                                            nAdjust,
                                            pAutoFmt, pColWidth );
    ((SwFrmFmt*)pTbl->GetFrmFmt())->SetName( sTblNm );
    SwTableNode* pTblNode = (SwTableNode*)rDoc.GetNodes()[nSttNode]->GetTableNode();

    if( pDDEFldType )
    {
        SwDDEFieldType* pNewType = (SwDDEFieldType*)rDoc.InsertFldType(
                                                            *pDDEFldType);
        SwDDETable* pDDETbl = new SwDDETable( pTblNode->GetTable(), pNewType );
        pTblNode->SetNewTable( pDDETbl );		// setze die DDE-Tabelle
        delete pDDEFldType, pDDEFldType = 0;
    }

    if( (pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() )) ||
        ( !( nsRedlineMode_t::REDLINE_IGNORE & GetRedlineMode() ) &&
            rDoc.GetRedlineTbl().Count() ))
    {
        SwPaM aPam( *pTblNode->EndOfSectionNode(), *pTblNode, 1 );
        SwCntntNode* pCNd = aPam.GetCntntNode( FALSE );
        if( pCNd )
            aPam.GetMark()->nContent.Assign( pCNd, 0 );

        if( pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
        {
            RedlineMode_t eOld = rDoc.GetRedlineMode();
            rDoc.SetRedlineMode_intern((RedlineMode_t)(eOld & ~nsRedlineMode_t::REDLINE_IGNORE));

            rDoc.AppendRedline( new SwRedline( *pRedlData, aPam ), true);
            rDoc.SetRedlineMode_intern( eOld );
        }
        else
            rDoc.SplitRedline( aPam );
    }
}


void SwUndoInsTbl::Repeat( SwUndoIter& rUndoIter )
{
    rUndoIter.GetDoc().InsertTable( aInsTblOpts, *rUndoIter.pAktPam->GetPoint(),
                                        nRows, nCols, nAdjust,
                                        pAutoFmt, pColWidth );
}

SwRewriter SwUndoInsTbl::GetRewriter() const
{
    SwRewriter aRewriter;

    aRewriter.AddRule(UNDO_ARG1, SW_RES(STR_START_QUOTE));
    aRewriter.AddRule(UNDO_ARG2, sTblNm);
    aRewriter.AddRule(UNDO_ARG3, SW_RES(STR_END_QUOTE));

    return aRewriter;
}

// -----------------------------------------------------

SwTblToTxtSave::SwTblToTxtSave( SwDoc& rDoc, ULONG nNd, ULONG nEndIdx, xub_StrLen nCnt )
    : m_nSttNd( nNd ), m_nEndNd( nEndIdx), m_nCntnt( nCnt ), m_pHstry( 0 )
{
    // Attributierung des gejointen Node merken.
    SwTxtNode* pNd = rDoc.GetNodes()[ nNd ]->GetTxtNode();
    if( pNd )
    {
        m_pHstry = new SwHistory;

        m_pHstry->Add( pNd->GetTxtColl(), nNd, ND_TEXTNODE );
        if ( pNd->GetpSwpHints() )
        {
            m_pHstry->CopyAttr( pNd->GetpSwpHints(), nNd, 0,
                        pNd->GetTxt().Len(), false );
        }
        if( pNd->HasSwAttrSet() )
            m_pHstry->CopyFmtAttr( *pNd->GetpSwAttrSet(), nNd );

        if( !m_pHstry->Count() )
            delete m_pHstry, m_pHstry = 0;

        // METADATA: store
        m_pMetadataUndoStart = pNd->CreateUndo();
    }

    // we also need to store the metadata reference of the _last_ paragraph
    // we subtract 1 to account for the removed cell start/end node pair
    // (after SectionUp, the end of the range points to the node after the cell)
    if ( nEndIdx - 1 > nNd )
    {
        SwTxtNode* pLastNode( rDoc.GetNodes()[ nEndIdx - 1 ]->GetTxtNode() );
        if( pLastNode )
        {
            // METADATA: store
            m_pMetadataUndoEnd = pLastNode->CreateUndo();
        }
    }
}

SwUndoTblToTxt::SwUndoTblToTxt( const SwTable& rTbl, sal_Unicode cCh )
    : SwUndo( UNDO_TABLETOTEXT ),
    sTblNm( rTbl.GetFrmFmt()->GetName() ), pDDEFldType( 0 ), pHistory( 0 ),
    nSttNd( 0 ), nEndNd( 0 ),
    nAdjust( static_cast<USHORT>(rTbl.GetFrmFmt()->GetHoriOrient().GetHoriOrient()) ),
    cTrenner( cCh ), nHdlnRpt( rTbl.GetRowsToRepeat() )
{
    pTblSave = new _SaveTable( rTbl );
    pBoxSaves = new SwTblToTxtSaves( (BYTE)rTbl.GetTabSortBoxes().Count() );

    if( rTbl.IsA( TYPE( SwDDETable ) ) )
        pDDEFldType = (SwDDEFieldType*)((SwDDETable&)rTbl).GetDDEFldType()->Copy();

    bCheckNumFmt = rTbl.GetFrmFmt()->GetDoc()->IsInsTblFormatNum();

    pHistory = new SwHistory;
    const SwTableNode* pTblNd = rTbl.GetTableNode();
    ULONG nTblStt = pTblNd->GetIndex(), nTblEnd = pTblNd->EndOfSectionIndex();

    const SwSpzFrmFmts& rFrmFmtTbl = *pTblNd->GetDoc()->GetSpzFrmFmts();
    for( USHORT n = 0; n < rFrmFmtTbl.Count(); ++n )
    {
        const SwPosition* pAPos;
        SwFrmFmt* pFmt = rFrmFmtTbl[ n ];
        const SwFmtAnchor* pAnchor = &pFmt->GetAnchor();
        if( 0 != ( pAPos = pAnchor->GetCntntAnchor()) &&
            ( FLY_AUTO_CNTNT == pAnchor->GetAnchorId() ||
              FLY_AT_CNTNT == pAnchor->GetAnchorId() ) &&
            nTblStt <= pAPos->nNode.GetIndex() &&
            pAPos->nNode.GetIndex() < nTblEnd )
        {
            pHistory->Add( *pFmt );
        }
    }

    if( !pHistory->Count() )
        delete pHistory, pHistory = 0;
}


SwUndoTblToTxt::~SwUndoTblToTxt()
{
    delete pDDEFldType;
    delete pTblSave;
    delete pBoxSaves;
    delete pHistory;
}



void SwUndoTblToTxt::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwPaM* pPam = rUndoIter.pAktPam;

    SwNodeIndex aFrmIdx( rDoc.GetNodes(), nSttNd );
    SwNodeIndex aEndIdx( rDoc.GetNodes(), nEndNd );

    pPam->GetPoint()->nNode = aFrmIdx;
    pPam->SetMark();
    pPam->GetPoint()->nNode = aEndIdx;
    rDoc.DelNumRules( *pPam );
    pPam->DeleteMark();

    // dann sammel mal alle Uppers ein
    SwNode2Layout aNode2Layout( aFrmIdx.GetNode() );

    // erzeuge die TabelleNode Structur
    SwTableNode* pTblNd = rDoc.GetNodes().UndoTableToText( nSttNd, nEndNd, *pBoxSaves );
    pTblNd->GetTable().SetTableModel( pTblSave->IsNewModel() );
    SwTableFmt* pTableFmt = rDoc.MakeTblFrmFmt( sTblNm, rDoc.GetDfltFrmFmt() );
    pTableFmt->Add( &pTblNd->GetTable() );		// das Frame-Format setzen
    pTblNd->GetTable().SetRowsToRepeat( nHdlnRpt );

    // erzeuge die alte Tabellen Struktur
    pTblSave->CreateNew( pTblNd->GetTable() );

    if( pDDEFldType )
    {
        SwDDEFieldType* pNewType = (SwDDEFieldType*)rDoc.InsertFldType(
                                                            *pDDEFldType);
        SwDDETable* pDDETbl = new SwDDETable( pTblNd->GetTable(), pNewType );
        pTblNd->SetNewTable( pDDETbl, FALSE );		// setze die DDE-Tabelle
        delete pDDEFldType, pDDEFldType = 0;
    }

    if( bCheckNumFmt )
    {
        SwTableSortBoxes& rBxs = pTblNd->GetTable().GetTabSortBoxes();
        for( USHORT nBoxes = rBxs.Count(); nBoxes; )
            rDoc.ChkBoxNumFmt( *rBxs[ --nBoxes ], FALSE );
    }

    if( pHistory )
    {
        USHORT nTmpEnd = pHistory->GetTmpEnd();
        pHistory->TmpRollback( &rDoc, 0 );
        pHistory->SetTmpEnd( nTmpEnd );
    }

    aNode2Layout.RestoreUpperFrms( rDoc.GetNodes(),
                                   pTblNd->GetIndex(), pTblNd->GetIndex()+1 );

    // will man eine TabellenSelektion ??
    pPam->DeleteMark();
    pPam->GetPoint()->nNode = *pTblNd->EndOfSectionNode();
    pPam->SetMark();
    pPam->GetPoint()->nNode = *pPam->GetNode()->StartOfSectionNode();
    pPam->Move( fnMoveForward, fnGoCntnt );
    pPam->Exchange();
    pPam->Move( fnMoveBackward, fnGoCntnt );

    ClearFEShellTabCols();
}

    // steht im untbl.cxx und darf nur vom Undoobject gerufen werden
SwTableNode* SwNodes::UndoTableToText( ULONG nSttNd, ULONG nEndNd,
                                const SwTblToTxtSaves& rSavedData )
{
    SwNodeIndex aSttIdx( *this, nSttNd );
    SwNodeIndex aEndIdx( *this, nEndNd+1 );

    SwTableNode * pTblNd = new SwTableNode( aSttIdx );
    SwEndNode* pEndNd = new SwEndNode( aEndIdx, *pTblNd  );

    aEndIdx = *pEndNd;

    /* Set pTblNd as start of section for all nodes in [nSttNd, nEndNd].
       Delete all Frames attached to the nodes in that range. */
    SwNode* pNd;
    {
        ULONG n, nTmpEnd = aEndIdx.GetIndex();
        for( n = pTblNd->GetIndex() + 1; n < nTmpEnd; ++n )
        {
            if( ( pNd = (*this)[ n ] )->IsCntntNode() )
                ((SwCntntNode*)pNd)->DelFrms();
            pNd->pStartOfSection = pTblNd;
        }
    }

    // dann die Tabellen Struktur teilweise aufbauen. Erstmal eine Line
    // in der alle Boxen stehen! Die korrekte Struktur kommt dann aus der
    // SaveStruct
    SwTableBoxFmt* pBoxFmt = GetDoc()->MakeTableBoxFmt();
    SwTableLineFmt* pLineFmt = GetDoc()->MakeTableLineFmt();
    SwTableLine* pLine = new SwTableLine( pLineFmt, rSavedData.Count(), 0 );
    pTblNd->GetTable().GetTabLines().C40_INSERT( SwTableLine, pLine, 0 );

    SvULongs aBkmkArr( 0, 4 );
    for( USHORT n = rSavedData.Count(); n; )
    {
        SwTblToTxtSave* pSave = rSavedData[ --n ];
        // if the start node was merged with last from prev. cell,
        // subtract 1 from index to get the merged paragraph, and split that
        aSttIdx = pSave->m_nSttNd - ( ( USHRT_MAX != pSave->m_nCntnt ) ? 1 : 0);
        SwTxtNode* pTxtNd = aSttIdx.GetNode().GetTxtNode();

        if( USHRT_MAX != pSave->m_nCntnt )
        {
            // an der ContentPosition splitten, das vorherige Zeichen
            // loeschen (ist der Trenner!)
            ASSERT( pTxtNd, "Wo ist der TextNode geblieben?" );
            SwIndex aCntPos( pTxtNd, pSave->m_nCntnt - 1 );

            pTxtNd->EraseText( aCntPos, 1 );
            SwCntntNode* pNewNd = pTxtNd->SplitCntntNode(
                                        SwPosition( aSttIdx, aCntPos ));
            if( aBkmkArr.Count() )
                _RestoreCntntIdx( aBkmkArr, *pNewNd, pSave->m_nCntnt,
                                                     pSave->m_nCntnt + 1 );
        }
        else
        {
            if( aBkmkArr.Count() )
                aBkmkArr.Remove( 0, aBkmkArr.Count() );
            if( pTxtNd )
                _SaveCntntIdx( GetDoc(), aSttIdx.GetIndex(),
                                pTxtNd->GetTxt().Len(), aBkmkArr );
        }

        if( pTxtNd )
        {
            // METADATA: restore
            pTxtNd->GetTxtNode()->RestoreMetadata(pSave->m_pMetadataUndoStart);
            if( pTxtNd->HasSwAttrSet() )
                pTxtNd->ResetAllAttr();

            if( pTxtNd->GetpSwpHints() )
                pTxtNd->ClearSwpHintsArr( false );
        }

        if( pSave->m_pHstry )
        {
            USHORT nTmpEnd = pSave->m_pHstry->GetTmpEnd();
            pSave->m_pHstry->TmpRollback( GetDoc(), 0 );
            pSave->m_pHstry->SetTmpEnd( nTmpEnd );
        }

        // METADATA: restore
        // end points to node after cell
        if ( pSave->m_nEndNd - 1 > pSave->m_nSttNd )
        {
            SwTxtNode* pLastNode = (*this)[ pSave->m_nEndNd - 1 ]->GetTxtNode();
            if (pLastNode)
            {
                pLastNode->RestoreMetadata(pSave->m_pMetadataUndoEnd);
            }
        }

        aEndIdx = pSave->m_nEndNd;
        SwStartNode* pSttNd = new SwStartNode( aSttIdx, ND_STARTNODE,
                                                SwTableBoxStartNode );
        pSttNd->pStartOfSection = pTblNd;
        new SwEndNode( aEndIdx, *pSttNd );

        for( ULONG i = aSttIdx.GetIndex(); i < aEndIdx.GetIndex()-1; ++i )
        {
            pNd = (*this)[ i ];
            pNd->pStartOfSection = pSttNd;
            if( pNd->IsStartNode() )
                i = pNd->EndOfSectionIndex();
        }

        SwTableBox* pBox = new SwTableBox( pBoxFmt, *pSttNd, pLine );
        pLine->GetTabBoxes().C40_INSERT( SwTableBox, pBox, 0 );
    }
    return pTblNd;
}


void SwUndoTblToTxt::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwPaM* pPam = rUndoIter.pAktPam;


    pPam->GetPoint()->nNode = nSttNd;
    pPam->GetPoint()->nContent.Assign( 0, 0 );
    SwNodeIndex aSaveIdx( pPam->GetPoint()->nNode, -1 );

    pPam->SetMark();            // alle Indizies abmelden
    pPam->DeleteMark();

    SwTableNode* pTblNd = pPam->GetNode()->GetTableNode();
    ASSERT( pTblNd, "keinen TableNode gefunden" );

    if( pTblNd->GetTable().IsA( TYPE( SwDDETable )) )
        pDDEFldType = (SwDDEFieldType*)((SwDDETable&)pTblNd->GetTable()).
                                                GetDDEFldType()->Copy();

    rDoc.TableToText( pTblNd, cTrenner );

    aSaveIdx++;
    SwCntntNode* pCNd = aSaveIdx.GetNode().GetCntntNode();
    if( !pCNd && 0 == ( pCNd = rDoc.GetNodes().GoNext( &aSaveIdx ) ) &&
        0 == ( pCNd = rDoc.GetNodes().GoPrevious( &aSaveIdx )) )
    {
        ASSERT( FALSE, "wo steht denn nun der TextNode" );
    }

    pPam->GetPoint()->nNode = aSaveIdx;
    pPam->GetPoint()->nContent.Assign( pCNd, 0 );

    pPam->SetMark();            // alle Indizies abmelden
    pPam->DeleteMark();
}


void SwUndoTblToTxt::Repeat( SwUndoIter& rUndoIter )
{
    SwTableNode* pTblNd = rUndoIter.pAktPam->GetNode()->FindTableNode();
    if( pTblNd )
    {
        // bewege den Cursor aus der Tabelle
        SwPaM* pPam = rUndoIter.pAktPam;
        pPam->GetPoint()->nNode = *pTblNd->EndOfSectionNode();
        pPam->Move( fnMoveForward, fnGoCntnt );
        pPam->SetMark();
        pPam->DeleteMark();

        rUndoIter.GetDoc().TableToText( pTblNd, cTrenner );
    }
}

void SwUndoTblToTxt::SetRange( const SwNodeRange& rRg )
{
    nSttNd = rRg.aStart.GetIndex();
    nEndNd = rRg.aEnd.GetIndex();
}

void SwUndoTblToTxt::AddBoxPos( SwDoc& rDoc, ULONG nNdIdx, ULONG nEndIdx, xub_StrLen nCntntIdx )
{
    SwTblToTxtSave* pNew = new SwTblToTxtSave( rDoc, nNdIdx, nEndIdx, nCntntIdx );
    pBoxSaves->Insert( pNew, pBoxSaves->Count() );
}

// -----------------------------------------------------

SwUndoTxtToTbl::SwUndoTxtToTbl( const SwPaM& rRg,
                                const SwInsertTableOptions& rInsTblOpts,
                                sal_Unicode cCh, USHORT nAdj,
                                const SwTableAutoFmt* pAFmt )
    : SwUndo( UNDO_TEXTTOTABLE ), SwUndRng( rRg ), aInsTblOpts( rInsTblOpts ),
      pDelBoxes( 0 ), pAutoFmt( 0 ),
      pHistory( 0 ), cTrenner( cCh ), nAdjust( nAdj )
{
    if( pAFmt )
        pAutoFmt = new SwTableAutoFmt( *pAFmt );

    const SwPosition* pEnd = rRg.End();
    SwNodes& rNds = rRg.GetDoc()->GetNodes();
    bSplitEnd = pEnd->nContent.GetIndex() && ( pEnd->nContent.GetIndex()
                        != pEnd->nNode.GetNode().GetCntntNode()->Len() ||
                pEnd->nNode.GetIndex() >= rNds.GetEndOfContent().GetIndex()-1 );
}

SwUndoTxtToTbl::~SwUndoTxtToTbl()
{
    delete pDelBoxes;
    delete pAutoFmt;
}

void SwUndoTxtToTbl::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    ULONG nTblNd = nSttNode;
    if( nSttCntnt )
        ++nTblNd;		// Node wurde vorher gesplittet
    SwNodeIndex aIdx( rDoc.GetNodes(), nTblNd );
    SwTableNode* pTNd = rDoc.GetNodes()[ aIdx ]->GetTableNode();
    ASSERT( pTNd, "keinen Tabellen-Node gefunden" );

    RemoveIdxFromSection( rDoc, nTblNd );

    sTblNm = pTNd->GetTable().GetFrmFmt()->GetName();

    if( pHistory )
    {
        pHistory->TmpRollback( &rDoc, 0 );
        pHistory->SetTmpEnd( pHistory->Count() );
    }

    if( pDelBoxes )
    {
        SwTable& rTbl = pTNd->GetTable();
        for( USHORT n = pDelBoxes->Count(); n; )
        {
            SwTableBox* pBox = rTbl.GetTblBox( (*pDelBoxes)[ --n ] );
            if( pBox )
                ::_DeleteBox( rTbl, pBox, 0, FALSE, FALSE );
            else {
                ASSERT( !this, "Wo ist die Box geblieben?" );
            }
        }
    }

    SwNodeIndex aEndIdx( *pTNd->EndOfSectionNode() );
    rDoc.TableToText( pTNd, 0x0b == cTrenner ? 0x09 : cTrenner );

    // am Start wieder zusammenfuegen ?
    SwPosition* pPos = rUndoIter.pAktPam->GetPoint();
    if( nSttCntnt )
    {
        pPos->nNode = nTblNd;
        pPos->nContent.Assign( rDoc.GetNodes()[ pPos->nNode ]->GetCntntNode(), 0 );
        if( rUndoIter.pAktPam->Move( fnMoveBackward, fnGoCntnt))
        {
            SwNodeIndex& rIdx = rUndoIter.pAktPam->GetPoint()->nNode;

            // dann die Crsr/etc. nochmal relativ verschieben
            RemoveIdxRel( rIdx.GetIndex()+1, *pPos );

            rIdx.GetNode().GetCntntNode()->JoinNext();
        }
    }

    // am Ende wieder zusammenfuegen ?
    if( bSplitEnd )
    {
        SwNodeIndex& rIdx = pPos->nNode;
        rIdx = nEndNode;
        SwTxtNode* pTxtNd = rIdx.GetNode().GetTxtNode();
        if( pTxtNd && pTxtNd->CanJoinNext() )
        {
            rUndoIter.pAktPam->GetMark()->nContent.Assign( 0, 0 );
            rUndoIter.pAktPam->GetPoint()->nContent.Assign( 0, 0 );

            // dann die Crsr/etc. nochmal relativ verschieben
            pPos->nContent.Assign( pTxtNd, pTxtNd->GetTxt().Len() );
            RemoveIdxRel( nEndNode + 1, *pPos );

            pTxtNd->JoinNext();
        }
    }

    SetPaM( rUndoIter );		// manipulierten Bereich selectieren
}


void SwUndoTxtToTbl::Redo( SwUndoIter& rUndoIter )
{
    SetPaM( rUndoIter );
    RemoveIdxFromRange( *rUndoIter.pAktPam, FALSE );
    SetPaM( rUndoIter );

    const SwTable* pTable = rUndoIter.GetDoc().TextToTable(
                aInsTblOpts, *rUndoIter.pAktPam, cTrenner,
                nAdjust, pAutoFmt );
    ((SwFrmFmt*)pTable->GetFrmFmt())->SetName( sTblNm );
}


void SwUndoTxtToTbl::Repeat( SwUndoIter& rUndoIter )
{
    // keine TABLE IN TABLE
    if( !rUndoIter.pAktPam->GetNode()->FindTableNode() )
        rUndoIter.GetDoc().TextToTable( aInsTblOpts, *rUndoIter.pAktPam,
                                        cTrenner, nAdjust,
                                        pAutoFmt );
}

void SwUndoTxtToTbl::AddFillBox( const SwTableBox& rBox )
{
    if( !pDelBoxes )
        pDelBoxes = new SvULongs;
    pDelBoxes->Insert( rBox.GetSttIdx(), pDelBoxes->Count() );
}

SwHistory& SwUndoTxtToTbl::GetHistory()
{
    if( !pHistory )
        pHistory = new SwHistory;
    return *pHistory;
}

// -----------------------------------------------------

SwUndoTblHeadline::SwUndoTblHeadline( const SwTable& rTbl, USHORT nOldHdl,
                                      USHORT nNewHdl )
    : SwUndo( UNDO_TABLEHEADLINE ),
    nOldHeadline( nOldHdl ),
    nNewHeadline( nNewHdl )
{
    ASSERT( rTbl.GetTabSortBoxes().Count(), "Tabelle ohne Inhalt" );
    const SwStartNode *pSttNd = rTbl.GetTabSortBoxes()[ 0 ]->GetSttNd();
    ASSERT( pSttNd, "Box ohne Inhalt" );

    nTblNd = pSttNd->StartOfSectionIndex();
}


void SwUndoTblHeadline::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwTableNode* pTNd = rDoc.GetNodes()[ nTblNd ]->GetTableNode();
    ASSERT( pTNd, "keinen Tabellen-Node gefunden" );

    rDoc.SetRowsToRepeat( pTNd->GetTable(), nOldHeadline );
}


void SwUndoTblHeadline::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    SwTableNode* pTNd = rDoc.GetNodes()[ nTblNd ]->GetTableNode();
    ASSERT( pTNd, "keinen Tabellen-Node gefunden" );

    rDoc.SetRowsToRepeat( pTNd->GetTable(), nNewHeadline );
}


void SwUndoTblHeadline::Repeat( SwUndoIter& rUndoIter )
{
    SwTableNode* pTblNd = rUndoIter.pAktPam->GetNode()->FindTableNode();
    if( pTblNd )
        rUndoIter.GetDoc().SetRowsToRepeat( pTblNd->GetTable(), nNewHeadline );
}


/*  */



_SaveTable::_SaveTable( const SwTable& rTbl, USHORT nLnCnt, BOOL bSaveFml )
    : aTblSet( *rTbl.GetFrmFmt()->GetAttrSet().GetPool(), aTableSetRange ),
    pSwTable( &rTbl ), nLineCount( nLnCnt ), bSaveFormula( bSaveFml )
{
    bModifyBox = FALSE;
    bNewModel = rTbl.IsNewModel();
    aTblSet.Put( rTbl.GetFrmFmt()->GetAttrSet() );
    pLine = new _SaveLine( 0, *rTbl.GetTabLines()[ 0 ], *this );

    _SaveLine* pLn = pLine;
    if( USHRT_MAX == nLnCnt )
        nLnCnt = rTbl.GetTabLines().Count();
    for( USHORT n = 1; n < nLnCnt; ++n )
        pLn = new _SaveLine( pLn, *rTbl.GetTabLines()[ n ], *this );

    aFrmFmts.Remove( 0, aFrmFmts.Count() );
    pSwTable = 0;
}


_SaveTable::~_SaveTable()
{
    delete pLine;
}


USHORT _SaveTable::AddFmt( SwFrmFmt* pFmt, bool bIsLine )
{
    USHORT nRet = aFrmFmts.GetPos( pFmt );
    if( USHRT_MAX == nRet )
    {
        // Kopie vom ItemSet anlegen
        SfxItemSet* pSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
            bIsLine ? aTableLineSetRange : aTableBoxSetRange );
        pSet->Put( pFmt->GetAttrSet() );
        //JP 20.04.98: Bug 49502 - wenn eine Formel gesetzt ist, nie den
        //				Value mit sichern. Der muss gegebenfalls neu
        //				errechnet werden!
        //JP 30.07.98: Bug 54295 - Formeln immer im Klartext speichern
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pSet->GetItemState( RES_BOXATR_FORMULA, TRUE, &pItem ))
        {
            pSet->ClearItem( RES_BOXATR_VALUE );
            if( pSwTable && bSaveFormula )
            {
                SwTableFmlUpdate aMsgHnt( pSwTable );
                aMsgHnt.eFlags = TBL_BOXNAME;
                ((SwTblBoxFormula*)pItem)->ChgDefinedIn( pFmt );
                ((SwTblBoxFormula*)pItem)->ChangeState( &aMsgHnt );
                ((SwTblBoxFormula*)pItem)->ChgDefinedIn( 0 );
            }
        }
        aSets.Insert( pSet, (nRet = aSets.Count() ) );
        aFrmFmts.Insert( pFmt, nRet );
    }
    return nRet;
}


void _SaveTable::RestoreAttr( SwTable& rTbl, BOOL bMdfyBox )
{
    USHORT n;

    bModifyBox = bMdfyBox;

    // zuerst die Attribute des TabellenFrmFormates zurueck holen
    SwFrmFmt* pFmt = rTbl.GetFrmFmt();
    SfxItemSet& rFmtSet  = (SfxItemSet&)pFmt->GetAttrSet();
    rFmtSet.ClearItem();
    rFmtSet.Put( aTblSet );

    if( pFmt->IsInCache() )
    {
        SwFrm::GetCache().Delete( pFmt );
        pFmt->SetInCache( FALSE );
    }

    // zur Sicherheit alle Tableframes invalidieren
    SwClientIter aIter( *pFmt );
    for( SwClient* pLast = aIter.First( TYPE( SwFrm ) ); pLast; pLast = aIter.Next() )
        if( ((SwTabFrm*)pLast)->GetTable() == &rTbl )
        {
            ((SwTabFrm*)pLast)->InvalidateAll();
            ((SwTabFrm*)pLast)->SetCompletePaint();
        }

    // FrmFmts mit Defaults (0) fuellen
    pFmt = 0;
    for( n = aSets.Count(); n; --n )
        aFrmFmts.Insert( pFmt, aFrmFmts.Count() );

    USHORT nLnCnt = nLineCount;
    if( USHRT_MAX == nLnCnt )
        nLnCnt = rTbl.GetTabLines().Count();

    _SaveLine* pLn = pLine;
    for( n = 0; n < nLnCnt; ++n, pLn = pLn->pNext )
    {
        if( !pLn )
        {
            ASSERT( !this, "Anzahl der Lines hat sich veraendert" );
            break;
        }

        pLn->RestoreAttr( *rTbl.GetTabLines()[ n ], *this );
    }

    aFrmFmts.Remove( 0, aFrmFmts.Count() );
    bModifyBox = FALSE;
}


void _SaveTable::SaveCntntAttrs( SwDoc* pDoc )
{
    pLine->SaveCntntAttrs( pDoc );
}


void _SaveTable::CreateNew( SwTable& rTbl, BOOL bCreateFrms,
                            BOOL bRestoreChart )
{
    USHORT n;

    _FndBox aTmpBox( 0, 0 );
    //if( bRestoreChart )
    //    // ? TL_CHART2: notification or locking of controller required ?
    aTmpBox.DelFrms( rTbl );

    // zuerst die Attribute des TabellenFrmFormates zurueck holen
    SwFrmFmt* pFmt = rTbl.GetFrmFmt();
    SfxItemSet& rFmtSet  = (SfxItemSet&)pFmt->GetAttrSet();
    rFmtSet.ClearItem();
    rFmtSet.Put( aTblSet );

    if( pFmt->IsInCache() )
    {
        SwFrm::GetCache().Delete( pFmt );
        pFmt->SetInCache( FALSE );
    }

    // SwTableBox muss ein Format haben!!
    SwTableBox aParent( (SwTableBoxFmt*)pFmt, rTbl.GetTabLines().Count(), 0 );

    // FrmFmts mit Defaults (0) fuellen
    pFmt = 0;
    for( n = aSets.Count(); n; --n )
        aFrmFmts.Insert( pFmt, aFrmFmts.Count() );

    pLine->CreateNew( rTbl, aParent, *this );
    aFrmFmts.Remove( 0, aFrmFmts.Count() );

    // die neuen Lines eintragen, die alten loeschen
    USHORT nOldLines = nLineCount;
    if( USHRT_MAX == nLineCount )
        nOldLines = rTbl.GetTabLines().Count();

    SwDoc *pDoc = rTbl.GetFrmFmt()->GetDoc();
    SwChartDataProvider *pPCD = pDoc->GetChartDataProvider();
    for( n = 0; n < aParent.GetTabLines().Count(); ++n )
    {
        SwTableLine* pLn = aParent.GetTabLines()[ n ];
        pLn->SetUpper( 0 );
        if( n < nOldLines )
        {
            SwTableLine* pOld = rTbl.GetTabLines()[ n ];

            // TL_CHART2: notify chart about boxes to be removed
            const SwTableBoxes &rBoxes = pOld->GetTabBoxes();
            USHORT nBoxes = rBoxes.Count();
            for (USHORT k = 0;  k < nBoxes;  ++k)
            {
                SwTableBox *pBox = rBoxes[k];
                if (pPCD)
                    pPCD->DeleteBox( &rTbl, *pBox );
            }

            rTbl.GetTabLines().C40_REPLACE( SwTableLine, pLn, n );
            delete pOld;
        }
        else
            rTbl.GetTabLines().C40_INSERT( SwTableLine, pLn, n );
    }

    if( n < nOldLines )
    {
        // remove remaining lines...

        for (USHORT k1 = 0; k1 < nOldLines - n;  ++k1)
        {
            const SwTableBoxes &rBoxes = rTbl.GetTabLines()[n + k1]->GetTabBoxes();
            USHORT nBoxes = rBoxes.Count();
            for (USHORT k2 = 0;  k2 < nBoxes;  ++k2)
            {
                SwTableBox *pBox = rBoxes[k2];
                // TL_CHART2: notify chart about boxes to be removed
                if (pPCD)
                    pPCD->DeleteBox( &rTbl, *pBox );
            }
        }

        rTbl.GetTabLines().DeleteAndDestroy( n, nOldLines - n );
    }

    aParent.GetTabLines().Remove( 0, n );

    if( bCreateFrms )
        aTmpBox.MakeFrms( rTbl );
    if( bRestoreChart )
    {
        // TL_CHART2: need to inform chart of probably changed cell names
        pDoc->UpdateCharts( rTbl.GetFrmFmt()->GetName() );
    }
}


void _SaveTable::NewFrmFmt( const SwClient* pLnBx, BOOL bIsLine,
                            USHORT nFmtPos, SwFrmFmt* pOldFmt )
{
    SwDoc* pDoc = pOldFmt->GetDoc();

    SwFrmFmt* pFmt = aFrmFmts[ nFmtPos ];
    if( !pFmt )
    {
        if( bIsLine )
            pFmt = pDoc->MakeTableLineFmt();
        else
            pFmt = pDoc->MakeTableBoxFmt();
        pFmt->SetFmtAttr( *aSets[ nFmtPos ] );
        aFrmFmts.Replace( pFmt, nFmtPos );
    }

    //Erstmal die Frms ummelden.
    SwClientIter aIter( *pOldFmt );
    for( SwClient* pLast = aIter.First( TYPE( SwFrm ) ); pLast; pLast = aIter.Next() )
    {
        if( bIsLine ? pLnBx == ((SwRowFrm*)pLast)->GetTabLine()
                    : pLnBx == ((SwCellFrm*)pLast)->GetTabBox() )
        {
            pFmt->Add( pLast );
            ((SwFrm*)pLast)->InvalidateAll();
            ((SwFrm*)pLast)->ReinitializeFrmSizeAttrFlags();
            if ( !bIsLine )
            {
                ((SwCellFrm*)pLast)->SetDerivedVert( FALSE );
                ((SwCellFrm*)pLast)->CheckDirChange();
            }
        }
    }

    //Jetzt noch mich selbst ummelden.
    pFmt->Add( (SwClient*)pLnBx );

    if( bModifyBox && !bIsLine )
    {
        const SfxPoolItem& rOld = pOldFmt->GetFmtAttr( RES_BOXATR_FORMAT ),
                         & rNew = pFmt->GetFmtAttr( RES_BOXATR_FORMAT );
        if( rOld != rNew )
            pFmt->Modify( (SfxPoolItem*)&rOld, (SfxPoolItem*)&rNew );
    }

    if( !pOldFmt->GetDepends() )
        delete pOldFmt;

}


_SaveLine::_SaveLine( _SaveLine* pPrev, const SwTableLine& rLine, _SaveTable& rSTbl )
    : pNext( 0 )
{
    if( pPrev )
        pPrev->pNext = this;

    nItemSet = rSTbl.AddFmt( rLine.GetFrmFmt(), true );

    pBox = new _SaveBox( 0, *rLine.GetTabBoxes()[ 0 ], rSTbl );
    _SaveBox* pBx = pBox;
    for( USHORT n = 1; n < rLine.GetTabBoxes().Count(); ++n )
        pBx = new _SaveBox( pBx, *rLine.GetTabBoxes()[ n ], rSTbl );
}


_SaveLine::~_SaveLine()
{
    delete pBox;
    delete pNext;
}


void _SaveLine::RestoreAttr( SwTableLine& rLine, _SaveTable& rSTbl )
{
    rSTbl.NewFrmFmt( &rLine, TRUE, nItemSet, rLine.GetFrmFmt() );

    _SaveBox* pBx = pBox;
    for( USHORT n = 0; n < rLine.GetTabBoxes().Count(); ++n, pBx = pBx->pNext )
    {
        if( !pBx )
        {
            ASSERT( !this, "Anzahl der Boxen hat sich veraendert" );
            break;
        }
        pBx->RestoreAttr( *rLine.GetTabBoxes()[ n ], rSTbl );
    }
}


void _SaveLine::SaveCntntAttrs( SwDoc* pDoc )
{
    pBox->SaveCntntAttrs( pDoc );
    if( pNext )
        pNext->SaveCntntAttrs( pDoc );
}


void _SaveLine::CreateNew( SwTable& rTbl, SwTableBox& rParent, _SaveTable& rSTbl )
{
    SwTableLineFmt* pFmt = (SwTableLineFmt*)rSTbl.aFrmFmts[ nItemSet ];
    if( !pFmt )
    {
        SwDoc* pDoc = rTbl.GetFrmFmt()->GetDoc();
        pFmt = pDoc->MakeTableLineFmt();
        pFmt->SetFmtAttr( *rSTbl.aSets[ nItemSet ] );
        rSTbl.aFrmFmts.Replace( pFmt, nItemSet );
    }
    SwTableLine* pNew = new SwTableLine( pFmt, 1, &rParent );

    rParent.GetTabLines().C40_INSERT( SwTableLine, pNew, rParent.GetTabLines().Count() );

    // HB, #127868# robustness: in some cases - which I
    // cannot reproduce nor see from the code - pNew seems
    // to be set to NULL in C40_INSERT.
    ASSERT(pNew, "Table line just created set to NULL in C40_INSERT");

    if (pNew)
    {
        pBox->CreateNew( rTbl, *pNew, rSTbl );
    }

    if( pNext )
        pNext->CreateNew( rTbl, rParent, rSTbl );
}


_SaveBox::_SaveBox( _SaveBox* pPrev, const SwTableBox& rBox, _SaveTable& rSTbl )
    : pNext( 0 ), nSttNode( ULONG_MAX ), nRowSpan(0)
{
    Ptrs.pLine = 0;

    if( pPrev )
        pPrev->pNext = this;

    nItemSet = rSTbl.AddFmt( rBox.GetFrmFmt(), false );

    if( rBox.GetSttNd() )
    {
        nSttNode = rBox.GetSttIdx();
        nRowSpan = rBox.getRowSpan();
    }
    else
    {
        Ptrs.pLine = new _SaveLine( 0, *rBox.GetTabLines()[ 0 ], rSTbl );

        _SaveLine* pLn = Ptrs.pLine;
        for( USHORT n = 1; n < rBox.GetTabLines().Count(); ++n )
            pLn = new _SaveLine( pLn, *rBox.GetTabLines()[ n ], rSTbl );
    }
}


_SaveBox::~_SaveBox()
{
    if( ULONG_MAX == nSttNode )		// keine EndBox
        delete Ptrs.pLine;
    else
        delete Ptrs.pCntntAttrs;
    delete pNext;
}


void _SaveBox::RestoreAttr( SwTableBox& rBox, _SaveTable& rSTbl )
{
    rSTbl.NewFrmFmt( &rBox, FALSE, nItemSet, rBox.GetFrmFmt() );

    if( ULONG_MAX == nSttNode )		// keine EndBox
    {
        if( !rBox.GetTabLines().Count() )
        {
            ASSERT( !this, "Anzahl der Lines hat sich veraendert" );
        }
        else
        {
            _SaveLine* pLn = Ptrs.pLine;
            for( USHORT n = 0; n < rBox.GetTabLines().Count(); ++n, pLn = pLn->pNext )
            {
                if( !pLn )
                {
                    ASSERT( !this, "Anzahl der Lines hat sich veraendert" );
                    break;
                }

                pLn->RestoreAttr( *rBox.GetTabLines()[ n ], rSTbl );
            }
        }
    }
    else if( rBox.GetSttNd() && rBox.GetSttIdx() == nSttNode )
    {
        if( Ptrs.pCntntAttrs )
        {
            SwNodes& rNds = rBox.GetFrmFmt()->GetDoc()->GetNodes();
            USHORT nSet = 0;
            ULONG nEnd = rBox.GetSttNd()->EndOfSectionIndex();
            for( ULONG n = nSttNode + 1; n < nEnd; ++n )
            {
                SwCntntNode* pCNd = rNds[ n ]->GetCntntNode();
                if( pCNd )
                {
                    SfxItemSet* pSet = (*Ptrs.pCntntAttrs)[ nSet++ ];
                    if( pSet )
                    {
                        USHORT *pRstAttr = aSave_BoxCntntSet;
                        while( *pRstAttr )
                        {
                            pCNd->ResetAttr( *pRstAttr, *(pRstAttr+1) );
                            pRstAttr += 2;
                        }
                        pCNd->SetAttr( *pSet );
                    }
                    else
                        pCNd->ResetAllAttr();
                }
            }
        }
    }
    else
    {
        ASSERT( !this, "Box nicht mehr am gleichen Node" );
    }
}


void _SaveBox::SaveCntntAttrs( SwDoc* pDoc )
{
    if( ULONG_MAX == nSttNode )		// keine EndBox
    {
        // weiter in der Line
        Ptrs.pLine->SaveCntntAttrs( pDoc );
    }
    else
    {
        ULONG nEnd = pDoc->GetNodes()[ nSttNode ]->EndOfSectionIndex();
        Ptrs.pCntntAttrs = new SfxItemSets( (BYTE)(nEnd - nSttNode - 1 ), 5 );
        for( ULONG n = nSttNode + 1; n < nEnd; ++n )
        {
            SwCntntNode* pCNd = pDoc->GetNodes()[ n ]->GetCntntNode();
            if( pCNd )
            {
                SfxItemSet* pSet = 0;
                if( pCNd->HasSwAttrSet() )
                {
                    pSet = new SfxItemSet( pDoc->GetAttrPool(),
                                            aSave_BoxCntntSet );
                    pSet->Put( *pCNd->GetpSwAttrSet() );
                }

                Ptrs.pCntntAttrs->Insert( pSet, Ptrs.pCntntAttrs->Count() );
            }
        }
    }
    if( pNext )
        pNext->SaveCntntAttrs( pDoc );
}


void _SaveBox::CreateNew( SwTable& rTbl, SwTableLine& rParent, _SaveTable& rSTbl )
{
    SwTableBoxFmt* pFmt = (SwTableBoxFmt*)rSTbl.aFrmFmts[ nItemSet ];
    if( !pFmt )
    {
        SwDoc* pDoc = rTbl.GetFrmFmt()->GetDoc();
        pFmt = pDoc->MakeTableBoxFmt();
        pFmt->SetFmtAttr( *rSTbl.aSets[ nItemSet ] );
        rSTbl.aFrmFmts.Replace( pFmt, nItemSet );
    }

    if( ULONG_MAX == nSttNode )		// keine EndBox
    {
        SwTableBox* pNew = new SwTableBox( pFmt, 1, &rParent );
        rParent.GetTabBoxes().C40_INSERT( SwTableBox, pNew, rParent.GetTabBoxes().Count() );

        Ptrs.pLine->CreateNew( rTbl, *pNew, rSTbl );
    }
    else
    {
        // Box zum StartNode in der alten Tabelle suchen
        SwTableBox* pBox = rTbl.GetTblBox( nSttNode );
        ASSERT( pBox, "Wo ist meine TabellenBox geblieben?" );

        SwFrmFmt* pOld = pBox->GetFrmFmt();
        pFmt->Add( pBox );
        if( !pOld->GetDepends() )
            delete pOld;

        pBox->setRowSpan( nRowSpan );

        SwTableBoxes* pTBoxes = &pBox->GetUpper()->GetTabBoxes();
        pTBoxes->Remove( pTBoxes->C40_GETPOS( SwTableBox, pBox ) );

        pBox->SetUpper( &rParent );
        pTBoxes = &rParent.GetTabBoxes();
        pTBoxes->C40_INSERT( SwTableBox, pBox, pTBoxes->Count() );
    }

    if( pNext )
        pNext->CreateNew( rTbl, rParent, rSTbl );
}


/*  */

// UndoObject fuer Attribut Aenderung an der Tabelle


SwUndoAttrTbl::SwUndoAttrTbl( const SwTableNode& rTblNd, BOOL bClearTabCols )
    : SwUndo( UNDO_TABLE_ATTR ),
    nSttNode( rTblNd.GetIndex() )
{
    bClearTabCol = bClearTabCols;
    pSaveTbl = new _SaveTable( rTblNd.GetTable() );
}


SwUndoAttrTbl::~SwUndoAttrTbl()
{
    delete pSaveTbl;
}



void SwUndoAttrTbl::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwTableNode* pTblNd = rDoc.GetNodes()[ nSttNode ]->GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );

    if (pTblNd)
    {
        _SaveTable* pOrig = new _SaveTable( pTblNd->GetTable() );
        pSaveTbl->RestoreAttr( pTblNd->GetTable() );
        delete pSaveTbl;
        pSaveTbl = pOrig;
    }

    if( bClearTabCol )
        ClearFEShellTabCols();
}


void SwUndoAttrTbl::Redo( SwUndoIter& rUndoIter )
{
    Undo( rUndoIter );
}


/*  */

// UndoObject fuer AutoFormat an der Tabelle


SwUndoTblAutoFmt::SwUndoTblAutoFmt( const SwTableNode& rTblNd,
                                    const SwTableAutoFmt& rAFmt )
    : SwUndo( UNDO_TABLE_AUTOFMT ),
    nSttNode( rTblNd.GetIndex() ), pUndos( 0 ),
    bSaveCntntAttr( FALSE )
{
    pSaveTbl = new _SaveTable( rTblNd.GetTable() );

    if( rAFmt.IsFont() || rAFmt.IsJustify() )
    {
        // dann auch noch ueber die ContentNodes der EndBoxen und
        // und alle Absatz-Attribute zusammen sammeln
        pSaveTbl->SaveCntntAttrs( (SwDoc*)rTblNd.GetDoc() );
        bSaveCntntAttr = TRUE;
    }
}


SwUndoTblAutoFmt::~SwUndoTblAutoFmt()
{
    delete pUndos;
    delete pSaveTbl;
}

void SwUndoTblAutoFmt::SaveBoxCntnt( const SwTableBox& rBox )
{
    SwUndoTblNumFmt* p = new SwUndoTblNumFmt( rBox );
    if( !pUndos )
        pUndos = new SwUndos( 8, 8 );
    pUndos->Insert( p, pUndos->Count() );
}


void SwUndoTblAutoFmt::UndoRedo( BOOL bUndo, SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwTableNode* pTblNd = rDoc.GetNodes()[ nSttNode ]->GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );

    _SaveTable* pOrig = new _SaveTable( pTblNd->GetTable() );
        // dann auch noch ueber die ContentNodes der EndBoxen und
        // und alle Absatz-Attribute zusammen sammeln
    if( bSaveCntntAttr )
        pOrig->SaveCntntAttrs( &rDoc );

    if( pUndos && bUndo )
        for( USHORT n = pUndos->Count(); n; )
            pUndos->GetObject( --n )->Undo( rUndoIter );

    pSaveTbl->RestoreAttr( pTblNd->GetTable(), !bUndo );
    delete pSaveTbl;
    pSaveTbl = pOrig;
}

void SwUndoTblAutoFmt::Undo( SwUndoIter& rUndoIter )
{
    UndoRedo( TRUE, rUndoIter );
}


void SwUndoTblAutoFmt::Redo( SwUndoIter& rUndoIter )
{
    UndoRedo( FALSE, rUndoIter );
}


/*  */


SwUndoTblNdsChg::SwUndoTblNdsChg( SwUndoId nAction,
                                    const SwSelBoxes& rBoxes,
                                    const SwTableNode& rTblNd,
                                    long nMn, long nMx,
                                    USHORT nCnt, BOOL bFlg, BOOL bSmHght )
    : SwUndo( nAction ),
    aBoxes( rBoxes.Count() < 255 ? (BYTE)rBoxes.Count() : 255, 10 ),
    nMin( nMn ), nMax( nMx ),
    nSttNode( rTblNd.GetIndex() ), nCurrBox( 0 ),
    nCount( nCnt ), nRelDiff( 0 ), nAbsDiff( 0 ),
    nSetColType( USHRT_MAX ),
    bFlag( bFlg ),
    bSameHeight( bSmHght )
{
    Ptrs.pNewSttNds = 0;

    const SwTable& rTbl = rTblNd.GetTable();
    pSaveTbl = new _SaveTable( rTbl );

    // und die Selektion merken
    for( USHORT n = 0; n < rBoxes.Count(); ++n )
        aBoxes.Insert( rBoxes[n]->GetSttIdx(), n );
}


SwUndoTblNdsChg::SwUndoTblNdsChg( SwUndoId nAction,
                                    const SwSelBoxes& rBoxes,
                                    const SwTableNode& rTblNd )
    : SwUndo( nAction ),
    aBoxes( rBoxes.Count() < 255 ? (BYTE)rBoxes.Count() : 255, 10 ),
    nMin( 0 ), nMax( 0 ),
    nSttNode( rTblNd.GetIndex() ), nCurrBox( 0 ),
    nCount( 0 ), nRelDiff( 0 ), nAbsDiff( 0 ),
    nSetColType( USHRT_MAX ),
    bFlag( FALSE ),
    bSameHeight( FALSE )
{
    Ptrs.pNewSttNds = 0;

    const SwTable& rTbl = rTblNd.GetTable();
    pSaveTbl = new _SaveTable( rTbl );

    // und die Selektion merken
    for( USHORT n = 0; n < rBoxes.Count(); ++n )
        aBoxes.Insert( rBoxes[n]->GetSttIdx(), n );
}

void SwUndoTblNdsChg::ReNewBoxes( const SwSelBoxes& rBoxes )
{
    if( rBoxes.Count() != aBoxes.Count() )
    {
        aBoxes.Remove( 0, aBoxes.Count() );
        for( USHORT n = 0; n < rBoxes.Count(); ++n )
            aBoxes.Insert( rBoxes[n]->GetSttIdx(), n );
    }
}

SwUndoTblNdsChg::~SwUndoTblNdsChg()
{
    delete pSaveTbl;

    if( IsDelBox() )
        delete Ptrs.pDelSects;
    else
        delete Ptrs.pNewSttNds;
}

void SwUndoTblNdsChg::SaveNewBoxes( const SwTableNode& rTblNd,
                                    const SwTableSortBoxes& rOld )
{
    const SwTable& rTbl = rTblNd.GetTable();
    const SwTableSortBoxes& rTblBoxes = rTbl.GetTabSortBoxes();
    USHORT n;
    USHORT i;

    ASSERT( ! IsDelBox(), "falsche Action" );
    Ptrs.pNewSttNds = new SvULongs( (BYTE)(rTblBoxes.Count() - rOld.Count()), 5 );

    for( n = 0, i = 0; n < rOld.Count(); ++i )
    {
        if( rOld[ n ] == rTblBoxes[ i ] )
            ++n;
        else
            // neue Box: sortiert einfuegen!!
            InsertSort( *Ptrs.pNewSttNds, rTblBoxes[ i ]->GetSttIdx() );
    }

    for( ; i < rTblBoxes.Count(); ++i )
        // neue Box: sortiert einfuegen!!
        InsertSort( *Ptrs.pNewSttNds, rTblBoxes[ i ]->GetSttIdx() );
}


SwTableLine* lcl_FindTableLine( const SwTable& rTable,
                                const SwTableBox& rBox )
{
    SwTableLine* pRet = NULL;
    // i63949: For nested cells we have to take nLineNo - 1, too, not 0!
    const SwTableLines &rTableLines = ( rBox.GetUpper()->GetUpper() != NULL ) ?
                                  rBox.GetUpper()->GetUpper()->GetTabLines()
                                : rTable.GetTabLines();
    const SwTableLine* pLine = rBox.GetUpper();
    USHORT nLineNo = rTableLines.C40_GETPOS( SwTableLine, pLine );
    pRet = rTableLines[nLineNo - 1];

    return pRet;
}

const SwTableLines& lcl_FindParentLines( const SwTable& rTable,
                                       const SwTableBox& rBox )
{
    const SwTableLines& rRet =
        ( rBox.GetUpper()->GetUpper() != NULL ) ?
            rBox.GetUpper()->GetUpper()->GetTabLines() :
            rTable.GetTabLines();

    return rRet;
}


void SwUndoTblNdsChg::SaveNewBoxes( const SwTableNode& rTblNd,
                                    const SwTableSortBoxes& rOld,
                                    const SwSelBoxes& rBoxes,
                                    const SvULongs& rNodeCnts )
{
    const SwTable& rTbl = rTblNd.GetTable();
    const SwTableSortBoxes& rTblBoxes = rTbl.GetTabSortBoxes();

    ASSERT( ! IsDelBox(), "falsche Action" );
    Ptrs.pNewSttNds = new SvULongs( (BYTE)(rTblBoxes.Count() - rOld.Count()), 5 );

    ASSERT( rTbl.IsNewModel() || rOld.Count() + nCount * rBoxes.Count() == rTblBoxes.Count(),
        "unexpected boxes" );
    ASSERT( rOld.Count() <= rTblBoxes.Count(), "more unexpected boxes" );
    for( USHORT n = 0, i = 0; i < rTblBoxes.Count(); ++i )
    {
        if( ( n < rOld.Count() ) &&
            ( rOld[ n ] == rTblBoxes[ i ] ) )
        {
            // box already known? Then nothing to be done.
            ++n;
        }
        else
        {
            // new box found: insert (obey sort order)
            USHORT nInsPos;
            const SwTableBox* pBox = rTblBoxes[ i ];
            InsertSort( *Ptrs.pNewSttNds, pBox->GetSttIdx(), &nInsPos );

            // find the source box. It must be one in rBoxes.
            // We found the right one if it's in the same column as pBox.
            // No, if more than one selected cell in the same column has been splitted,
            // we have to look for the nearest one (i65201)!
            const SwTableBox* pSourceBox = NULL;
            const SwTableBox* pCheckBox = NULL;
            const SwTableLine* pBoxLine = pBox->GetUpper();
            USHORT nLineDiff = lcl_FindParentLines(rTbl,*pBox).C40_GETPOS(SwTableLine,pBoxLine);
            USHORT nLineNo = 0;
            for( USHORT j = 0; j < rBoxes.Count(); ++j )
            {
                pCheckBox = rBoxes[j];
                if( pCheckBox->GetUpper()->GetUpper() == pBox->GetUpper()->GetUpper() )
                {
                    const SwTableLine* pCheckLine = pCheckBox->GetUpper();
                    USHORT nCheckLine = lcl_FindParentLines( rTbl, *pCheckBox ).
                    C40_GETPOS( SwTableLine, pCheckLine );
                    if( ( !pSourceBox || nCheckLine > nLineNo ) && nCheckLine < nLineDiff )
                    {
                        nLineNo = nCheckLine;
                        pSourceBox = pCheckBox;
                    }
                }
            }

            // find the line number difference
            // (to help determine bNodesMoved flag below)
            nLineDiff = nLineDiff - nLineNo;
            ASSERT( pSourceBox, "Splitted source box not found!" );
            // find out how many nodes the source box used to have
            // (to help determine bNodesMoved flag below)
            USHORT nNdsPos = 0;
            while( rBoxes[ nNdsPos ] != pSourceBox )
                ++nNdsPos;
            ULONG nNodes = rNodeCnts[ nNdsPos ];

            // When a new table cell is created, it either gets a new
            // node, or it gets node(s) from elsewhere. The undo must
            // know, of course, and thus we must determine here just
            // where pBox's nodes are from:
            // If 1) the source box has lost nodes, and
            //    2) we're in the node range that got nodes
            // then pBox received nodes from elsewhere.
            // If bNodesMoved is set for pBox the undo must move the
            // boxes back, otherwise it must delete them.
            // The bNodesMoved flag is stored in a seperate array
            // which mirrors Ptrs.pNewSttNds, i.e. Ptrs.pNewSttNds[i]
            // and aMvBoxes[i] belong together.
            BOOL bNodesMoved =
                ( nNodes != ( pSourceBox->GetSttNd()->EndOfSectionIndex() -
                              pSourceBox->GetSttIdx() ) )
                && ( nNodes - 1 > nLineDiff );
            aMvBoxes.Insert( bNodesMoved, nInsPos );
        }
    }
}


void SwUndoTblNdsChg::SaveSection( SwStartNode* pSttNd )
{
    ASSERT( IsDelBox(), "falsche Action" );
    if( !Ptrs.pDelSects )
        Ptrs.pDelSects = new SwUndoSaveSections( 10, 5 );

    SwTableNode* pTblNd = pSttNd->FindTableNode();
    SwUndoSaveSection* pSave = new SwUndoSaveSection;
    pSave->SaveSection( pSttNd->GetDoc(), SwNodeIndex( *pSttNd ));

    Ptrs.pDelSects->Insert( pSave, Ptrs.pDelSects->Count() );
    nSttNode = pTblNd->GetIndex();
}


void SwUndoTblNdsChg::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwNodeIndex aIdx( rDoc.GetNodes(), nSttNode );

    SwTableNode* pTblNd = rDoc.GetNodes()[ aIdx ]->GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );

    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.eFlags = TBL_BOXPTR;
    rDoc.UpdateTblFlds( &aMsgHnt );

    CHECK_TABLE( pTblNd->GetTable() )

    _FndBox aTmpBox( 0, 0 );
    // ? TL_CHART2: notification or locking of controller required ?

    SwChartDataProvider *pPCD = rDoc.GetChartDataProvider();
    std::vector< SwTableBox* > aDelBoxes;
    if( IsDelBox() )
    {
        // Trick: die fehlenden Boxen in irgendeine Line einfuegen, beim
        // CreateNew werden sie korrekt verbunden.
        SwTableBox* pCpyBox = pTblNd->GetTable().GetTabSortBoxes()[0];
        SwTableBoxes& rLnBoxes = pCpyBox->GetUpper()->GetTabBoxes();

        // die Sections wieder herstellen
        for( USHORT n = Ptrs.pDelSects->Count(); n; )
        {
            SwUndoSaveSection* pSave = (*Ptrs.pDelSects)[ --n ];
            pSave->RestoreSection( &rDoc, &aIdx, SwTableBoxStartNode );
            if( pSave->GetHistory() )
                pSave->GetHistory()->Rollback( &rDoc );
            SwTableBox* pBox = new SwTableBox( (SwTableBoxFmt*)pCpyBox->GetFrmFmt(), aIdx,
                                                pCpyBox->GetUpper() );
            rLnBoxes.C40_INSERT( SwTableBox, pBox, rLnBoxes.Count() );
        }
        Ptrs.pDelSects->DeleteAndDestroy( 0, Ptrs.pDelSects->Count() );
    }
    else if( aMvBoxes.Count() )
    {
        // dann muessen Nodes verschoben und nicht geloescht werden!
        // Dafuer brauchen wir aber ein temp Array
        SvULongs aTmp( 0, 5);
        aTmp.Insert( Ptrs.pNewSttNds, 0 );

        // von hinten anfangen
        for( USHORT n = aTmp.Count(); n; )
        {
            // Box aus der Tabellen-Struktur entfernen
            ULONG nIdx = aTmp[ --n ];
            SwTableBox* pBox = pTblNd->GetTable().GetTblBox( nIdx );
            ASSERT( pBox, "Wo ist meine TabellenBox geblieben?" );

            // TL_CHART2: notify chart about box to be removed
            if (pPCD)
                pPCD->DeleteBox( &pTblNd->GetTable(), *pBox );

            if( aMvBoxes[ n ] )
            {
                SwNodeRange aRg( *pBox->GetSttNd(), 1,
                            *pBox->GetSttNd()->EndOfSectionNode() );

                SwTableLine* pLine = lcl_FindTableLine( pTblNd->GetTable(), *pBox );
                SwNodeIndex aInsPos( *(pLine->GetTabBoxes()[0]->GetSttNd()), 2 );

                // alle StartNode Indizies anpassen
                USHORT i = n;
                ULONG nSttIdx = aInsPos.GetIndex() - 2,
                       nNdCnt = aRg.aEnd.GetIndex() - aRg.aStart.GetIndex();
                while( i && aTmp[ --i ] > nSttIdx )
                    aTmp[ i ] += nNdCnt;

                // erst die Box loeschen
                delete pBox;
                // dann die Nodes verschieben,
                rDoc.GetNodes()._MoveNodes( aRg, rDoc.GetNodes(), aInsPos, FALSE );
            }
            else
                rDoc.DeleteSection( rDoc.GetNodes()[ nIdx ] );
            aDelBoxes.insert( aDelBoxes.end(), pBox );
        }
    }
    else
    {
        // Remove nodes from nodes array (backwards!)
        for( USHORT n = Ptrs.pNewSttNds->Count(); n; )
        {
            ULONG nIdx = (*Ptrs.pNewSttNds)[ --n ];
            SwTableBox* pBox = pTblNd->GetTable().GetTblBox( nIdx );
            ASSERT( pBox, "Where's my table box?" );
            // TL_CHART2: notify chart about box to be removed
            if (pPCD)
                pPCD->DeleteBox( &pTblNd->GetTable(), *pBox );
            aDelBoxes.insert( aDelBoxes.end(), pBox );
            rDoc.DeleteSection( rDoc.GetNodes()[ nIdx ] );
        }
    }
    // Remove boxes from table structure
    for( USHORT n = 0; n < aDelBoxes.size(); ++n )
    {
        SwTableBox* pCurrBox = aDelBoxes[n];
        SwTableBoxes* pTBoxes = &pCurrBox->GetUpper()->GetTabBoxes();
        pTBoxes->Remove( pTBoxes->C40_GETPOS( SwTableBox, pCurrBox ) );
        delete pCurrBox;
    }

    pSaveTbl->CreateNew( pTblNd->GetTable(), TRUE, FALSE );

    // TL_CHART2: need to inform chart of probably changed cell names
    rDoc.UpdateCharts( pTblNd->GetTable().GetFrmFmt()->GetName() );

    if( IsDelBox() )
        nSttNode = pTblNd->GetIndex();
    ClearFEShellTabCols();
    CHECK_TABLE( pTblNd->GetTable() )
}


void SwUndoTblNdsChg::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    SwTableNode* pTblNd = rDoc.GetNodes()[ nSttNode ]->GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );
    CHECK_TABLE( pTblNd->GetTable() )

    SwSelBoxes aSelBoxes;
    for( USHORT n = 0; n < aBoxes.Count(); ++n )
    {
        SwTableBox* pBox = pTblNd->GetTable().GetTblBox( aBoxes[ n ] );
        aSelBoxes.Insert( pBox );
    }

    // SelBoxes erzeugen und InsertCell/-Row/SplitTbl aufrufen
    switch( GetId() )
    {
    case UNDO_TABLE_INSCOL:
        if( USHRT_MAX == nSetColType )
            rDoc.InsertCol( aSelBoxes, nCount, bFlag );
        else
        {
            SwTableBox* pBox = pTblNd->GetTable().GetTblBox( nCurrBox );
            rDoc.SetColRowWidthHeight( *pBox, nSetColType, nAbsDiff,
                                        nRelDiff );
        }
        break;

    case UNDO_TABLE_INSROW:
        if( USHRT_MAX == nSetColType )
            rDoc.InsertRow( aSelBoxes, nCount, bFlag );
        else
        {
            SwTable& rTbl = pTblNd->GetTable();
            SwTableBox* pBox = rTbl.GetTblBox( nCurrBox );
            TblChgMode eOldMode = rTbl.GetTblChgMode();
            rTbl.SetTblChgMode( (TblChgMode)nCount );
            rDoc.SetColRowWidthHeight( *pBox, nSetColType, nAbsDiff, nRelDiff );
            rTbl.SetTblChgMode( eOldMode );
        }
        break;

    case UNDO_TABLE_SPLIT:
        rDoc.SplitTbl( aSelBoxes, bFlag, nCount, bSameHeight );
        break;
    case UNDO_TABLE_DELBOX:
    case UNDO_ROW_DELETE:
    case UNDO_COL_DELETE:
        if( USHRT_MAX == nSetColType )
        {
            SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
            aMsgHnt.eFlags = TBL_BOXPTR;
            rDoc.UpdateTblFlds( &aMsgHnt );
            SwTable &rTable = pTblNd->GetTable();
            if( nMax > nMin && rTable.IsNewModel() )
                rTable.PrepareDeleteCol( nMin, nMax );
            rTable.DeleteSel( &rDoc, aSelBoxes, 0, this, TRUE, TRUE );
        }
        else
        {
            SwTable& rTbl = pTblNd->GetTable();

            SwTableFmlUpdate aMsgHnt( &rTbl );
            aMsgHnt.eFlags = TBL_BOXPTR;
            rDoc.UpdateTblFlds( &aMsgHnt );

            SwTableBox* pBox = rTbl.GetTblBox( nCurrBox );
            TblChgMode eOldMode = rTbl.GetTblChgMode();
            rTbl.SetTblChgMode( (TblChgMode)nCount );

            rDoc.DoUndo( TRUE );		// wir brauchen die SaveSections!
            SwUndoTblNdsChg* pUndo = 0;

            switch( nSetColType & 0xff )
            {
            case nsTblChgWidthHeightType::WH_COL_LEFT:
            case nsTblChgWidthHeightType::WH_COL_RIGHT:
            case nsTblChgWidthHeightType::WH_CELL_LEFT:
            case nsTblChgWidthHeightType::WH_CELL_RIGHT:
                 rTbl.SetColWidth( *pBox, nSetColType, nAbsDiff,
                                    nRelDiff, (SwUndo**)&pUndo );
                break;
            case nsTblChgWidthHeightType::WH_ROW_TOP:
            case nsTblChgWidthHeightType::WH_ROW_BOTTOM:
            case nsTblChgWidthHeightType::WH_CELL_TOP:
            case nsTblChgWidthHeightType::WH_CELL_BOTTOM:
                rTbl.SetRowHeight( *pBox, nSetColType, nAbsDiff,
                                    nRelDiff, (SwUndo**)&pUndo );
                break;
            }

            if( pUndo )
            {
                Ptrs.pDelSects->Insert( pUndo->Ptrs.pDelSects, 0 );
                pUndo->Ptrs.pDelSects->Remove( 0, pUndo->Ptrs.pDelSects->Count() );

                delete pUndo;
            }
            rDoc.DoUndo( FALSE );

            rTbl.SetTblChgMode( eOldMode );
        }
        nSttNode = pTblNd->GetIndex();
        break;
    default:
        ;
    }
    ClearFEShellTabCols();
    CHECK_TABLE( pTblNd->GetTable() )
}


/*  */


SwUndoTblMerge::SwUndoTblMerge( const SwPaM& rTblSel )
    : SwUndo( UNDO_TABLE_MERGE ), SwUndRng( rTblSel ), pHistory( 0 )
{
    const SwTableNode* pTblNd = rTblSel.GetNode()->FindTableNode();
    ASSERT( pTblNd, "Wo ist TabllenNode" )
    pSaveTbl = new _SaveTable( pTblNd->GetTable() );
    pMoves = new SwUndoMoves;
    nTblNode = pTblNd->GetIndex();
}


SwUndoTblMerge::~SwUndoTblMerge()
{
    delete pSaveTbl;
    delete pMoves;
    delete pHistory;
}


void SwUndoTblMerge::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwNodeIndex aIdx( rDoc.GetNodes(), nTblNode );

    SwTableNode* pTblNd = rDoc.GetNodes()[ aIdx ]->GetTableNode();
    ASSERT( pTblNd, "kein TabellenNode" );

    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.eFlags = TBL_BOXPTR;
    rDoc.UpdateTblFlds( &aMsgHnt );

    _FndBox aTmpBox( 0, 0 );
    // ? TL_CHART2: notification or locking of controller required ?


    // 1. die geloeschten Boxen wiederherstellen:

    // Trick: die fehlenden Boxen in irgendeine Line einfuegen, beim
    // CreateNew werden sie korrekt verbunden.
    SwTableBox *pBox, *pCpyBox = pTblNd->GetTable().GetTabSortBoxes()[0];
    SwTableBoxes& rLnBoxes = pCpyBox->GetUpper()->GetTabBoxes();

DUMPDOC( &rDoc, "d:\\tmp\\tab_a.db" )
CHECKTABLE(pTblNd->GetTable())

    SwSelBoxes aSelBoxes;
    SwTxtFmtColl* pColl = rDoc.GetTxtCollFromPool( RES_POOLCOLL_STANDARD );
    USHORT n;

    for( n = 0; n < aBoxes.Count(); ++n )
    {
        aIdx = aBoxes[ n ];
        SwStartNode* pSttNd = rDoc.GetNodes().MakeTextSection( aIdx,
                                            SwTableBoxStartNode, pColl );
        pBox = new SwTableBox( (SwTableBoxFmt*)pCpyBox->GetFrmFmt(), *pSttNd,
                                pCpyBox->GetUpper() );
        rLnBoxes.C40_INSERT( SwTableBox, pBox, rLnBoxes.Count() );

        aSelBoxes.Insert( pBox );
    }

DUMPDOC( &rDoc, "d:\\tmp\\tab_b.db" )
CHECKTABLE(pTblNd->GetTable())

    SwChartDataProvider *pPCD = rDoc.GetChartDataProvider();
    // 2. die eingefuegten Boxen loeschen
    // die Nodes loeschen (von Hinten!!)
    for( n = aNewSttNds.Count(); n; )
    {
        // Box aus der Tabellen-Struktur entfernen
        ULONG nIdx = aNewSttNds[ --n ];

        if( !nIdx && n )
        {
            nIdx = aNewSttNds[ --n ];
            pBox = pTblNd->GetTable().GetTblBox( nIdx );
            ASSERT( pBox, "Wo ist meine TabellenBox geblieben?" );

            if( !pSaveTbl->IsNewModel() )
                rDoc.GetNodes().MakeTxtNode( SwNodeIndex(
                    *pBox->GetSttNd()->EndOfSectionNode() ), pColl );

            // das war der Trenner, -> die verschobenen herstellen
            for( USHORT i = pMoves->Count(); i; )
            {
                SwTxtNode* pTxtNd = 0;
                USHORT nDelPos = 0;
                SwUndoMove* pUndo = (*pMoves)[ --i ];
                if( !pUndo->IsMoveRange() )
                {
                    pTxtNd = rDoc.GetNodes()[ pUndo->GetDestSttNode() ]->GetTxtNode();
                    nDelPos = pUndo->GetDestSttCntnt() - 1;
                }
                pUndo->Undo( rUndoIter );
                if( pUndo->IsMoveRange() )
                {
                    // den ueberfluessigen Node loeschen
                    aIdx = pUndo->GetEndNode();
                    SwCntntNode *pCNd = aIdx.GetNode().GetCntntNode();
                    if( pCNd )
                    {
                        SwNodeIndex aTmp( aIdx, -1 );
                        SwCntntNode *pMove = aTmp.GetNode().GetCntntNode();
                        if( pMove )
                            pCNd->MoveTo( *pMove );
                    }
                    rDoc.GetNodes().Delete( aIdx, 1 );
                }
                else if( pTxtNd )
                {
                    // evt. noch ueberflussige Attribute loeschen
                    SwIndex aTmpIdx( pTxtNd, nDelPos );
                    if( pTxtNd->GetpSwpHints() && pTxtNd->GetpSwpHints()->Count() )
                        pTxtNd->RstAttr( aTmpIdx, pTxtNd->GetTxt().Len() -
                                                            nDelPos + 1 );
                    // das Trennzeichen loeschen
                    pTxtNd->EraseText( aTmpIdx, 1 );
                }
//				delete pUndo;
DUMPDOC( &rDoc, String( "d:\\tmp\\tab_") + String( aNewSttNds.Count() - i ) +
                String(".db") )
            }
//			pMoves->Remove( 0, pMoves->Count() );
            nIdx = pBox->GetSttIdx();
        }
        else
            pBox = pTblNd->GetTable().GetTblBox( nIdx );

        if( !pSaveTbl->IsNewModel() )
        {
            // TL_CHART2: notify chart about box to be removed
            if (pPCD)
                pPCD->DeleteBox( &pTblNd->GetTable(), *pBox );

            SwTableBoxes* pTBoxes = &pBox->GetUpper()->GetTabBoxes();
            pTBoxes->Remove( pTBoxes->C40_GETPOS( SwTableBox, pBox ) );


            // Indizies aus dem Bereich loeschen
            {
                SwNodeIndex aTmpIdx( *pBox->GetSttNd() );
                rDoc.CorrAbs( SwNodeIndex( aTmpIdx, 1 ),
                            SwNodeIndex( *aTmpIdx.GetNode().EndOfSectionNode() ),
                            SwPosition( aTmpIdx, SwIndex( 0, 0 )), TRUE );
            }

            delete pBox;
            rDoc.DeleteSection( rDoc.GetNodes()[ nIdx ] );
        }
    }
DUMPDOC( &rDoc, "d:\\tmp\\tab_z.db" )
CHECKTABLE(pTblNd->GetTable())


    pSaveTbl->CreateNew( pTblNd->GetTable(), TRUE, FALSE );

    // TL_CHART2: need to inform chart of probably changed cell names
    rDoc.UpdateCharts( pTblNd->GetTable().GetFrmFmt()->GetName() );

    if( pHistory )
    {
        pHistory->TmpRollback( &rDoc, 0 );
        pHistory->SetTmpEnd( pHistory->Count() );
    }
//	nTblNode = pTblNd->GetIndex();

    SwPaM* pPam = rUndoIter.pAktPam;
    pPam->DeleteMark();
    pPam->GetPoint()->nNode = nSttNode;
    pPam->GetPoint()->nContent.Assign( pPam->GetCntntNode(), nSttCntnt );
    pPam->SetMark();
    pPam->DeleteMark();

CHECKTABLE(pTblNd->GetTable())
    ClearFEShellTabCols();
}


void SwUndoTblMerge::Redo( SwUndoIter& rUndoIter )
{
    SwPaM* pPam = rUndoIter.pAktPam;
    SwDoc& rDoc = *pPam->GetDoc();

    SetPaM( *pPam );
    rDoc.MergeTbl( *pPam );
}

void SwUndoTblMerge::MoveBoxCntnt( SwDoc* pDoc, SwNodeRange& rRg, SwNodeIndex& rPos )
{
    SwNodeIndex aTmp( rRg.aStart, -1 ), aTmp2( rPos, -1 );
    SwUndoMove* pUndo = new SwUndoMove( pDoc, rRg, rPos );
    sal_Bool bDoesUndo = pDoc->DoesUndo();
    pDoc->DoUndo( sal_False );
    pDoc->MoveNodeRange( rRg, rPos, (pSaveTbl->IsNewModel()) ?
        IDocumentContentOperations::DOC_NO_DELFRMS :
        IDocumentContentOperations::DOC_MOVEDEFAULT );
    if( bDoesUndo )
        pDoc->DoUndo( sal_True );
    aTmp++;
    aTmp2++;
    pUndo->SetDestRange( aTmp2, rPos, aTmp );

    pMoves->Insert( pUndo, pMoves->Count() );
}


void SwUndoTblMerge::SetSelBoxes( const SwSelBoxes& rBoxes )
{
    // die Selektion merken
    for( USHORT n = 0; n < rBoxes.Count(); ++n )
        InsertSort( aBoxes, rBoxes[n]->GetSttIdx() );

    // als Trennung fuers einfuegen neuer Boxen nach dem Verschieben!
    aNewSttNds.Insert( (ULONG)0, aNewSttNds.Count() );

     // The new table model does not delete overlapped cells (by row span),
     // so the rBoxes array might be empty even some cells have been merged.
    if( rBoxes.Count() )
        nTblNode = rBoxes[ 0 ]->GetSttNd()->FindTableNode()->GetIndex();
}

void SwUndoTblMerge::SaveCollection( const SwTableBox& rBox )
{
    if( !pHistory )
        pHistory = new SwHistory;

    SwNodeIndex aIdx( *rBox.GetSttNd(), 1 );
    SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
    if( !pCNd )
        pCNd = aIdx.GetNodes().GoNext( &aIdx );

    pHistory->Add( pCNd->GetFmtColl(), aIdx.GetIndex(), pCNd->GetNodeType());
    if( pCNd->HasSwAttrSet() )
        pHistory->CopyFmtAttr( *pCNd->GetpSwAttrSet(), aIdx.GetIndex() );
}

/*  */


SwUndoTblNumFmt::SwUndoTblNumFmt( const SwTableBox& rBox,
                                    const SfxItemSet* pNewSet )
    : SwUndo( UNDO_TBLNUMFMT ),
    pBoxSet( 0 ), pHistory( 0 ), nFmtIdx( NUMBERFORMAT_TEXT )
{
    bNewFmt = bNewFml = bNewValue = FALSE;
    nNode = rBox.GetSttIdx();

    nNdPos = rBox.IsValidNumTxtNd( 0 == pNewSet );
    SwDoc* pDoc = rBox.GetFrmFmt()->GetDoc();

    if( ULONG_MAX != nNdPos )
    {
        SwTxtNode* pTNd = pDoc->GetNodes()[ nNdPos ]->GetTxtNode();

        pHistory = new SwHistory;
        SwRegHistory aRHst( *rBox.GetSttNd(), pHistory );
        // always save all text atttibutes because of possibly overlapping
        // areas of on/off
        pHistory->CopyAttr( pTNd->GetpSwpHints(), nNdPos, 0,
                            pTNd->GetTxt().Len(), true );

        if( pTNd->HasSwAttrSet() )
            pHistory->CopyFmtAttr( *pTNd->GetpSwAttrSet(), nNdPos );

        aStr = pTNd->GetTxt();
        if( pTNd->GetpSwpHints() )
            pTNd->GetpSwpHints()->DeRegister();
    }

    pBoxSet = new SfxItemSet( pDoc->GetAttrPool(), aTableBoxSetRange );
    pBoxSet->Put( rBox.GetFrmFmt()->GetAttrSet() );

    if( pNewSet )
    {
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pNewSet->GetItemState( RES_BOXATR_FORMAT,
                FALSE, &pItem ))
        {
            bNewFmt = TRUE;
            nNewFmtIdx = ((SwTblBoxNumFormat*)pItem)->GetValue();
        }
        if( SFX_ITEM_SET == pNewSet->GetItemState( RES_BOXATR_FORMULA,
                FALSE, &pItem ))
        {
            bNewFml = TRUE;
            aNewFml = ((SwTblBoxFormula*)pItem)->GetFormula();
        }
        if( SFX_ITEM_SET == pNewSet->GetItemState( RES_BOXATR_VALUE,
                FALSE, &pItem ))
        {
            bNewValue = TRUE;
            fNewNum = ((SwTblBoxValue*)pItem)->GetValue();
        }
    }

    // wird die History ueberhaupt benoetigt ??
    if( pHistory && !pHistory->Count() )
        DELETEZ( pHistory );
}


SwUndoTblNumFmt::~SwUndoTblNumFmt()
{
    delete pHistory;
    delete pBoxSet;
}

void SwUndoTblNumFmt::Undo( SwUndoIter& rIter )
{
    ASSERT( pBoxSet, "Where's the stored item set?" )

    SwDoc& rDoc = rIter.GetDoc();
    SwStartNode* pSttNd = rDoc.GetNodes()[ nNode ]->
                            FindSttNodeByType( SwTableBoxStartNode );
    ASSERT( pSttNd, "ohne StartNode kein TabellenBox" );
    SwTableBox* pBox = pSttNd->FindTableNode()->GetTable().GetTblBox(
                                    pSttNd->GetIndex() );
    ASSERT( pBox, "keine TabellenBox gefunden" );

    SwTableBoxFmt* pFmt = rDoc.MakeTableBoxFmt();
    pFmt->SetFmtAttr( *pBoxSet );
    pBox->ChgFrmFmt( pFmt );

    if( ULONG_MAX == nNdPos )
        return;

    SwTxtNode* pTxtNd = rDoc.GetNodes()[ nNdPos ]->GetTxtNode();
    // wenn mehr als ein Node geloescht wurde, dann wurden auch
    // alle "Node"-Attribute gespeichert
    if( pTxtNd->HasSwAttrSet() )
        pTxtNd->ResetAllAttr();

    if( pTxtNd->GetpSwpHints() && aStr.Len() )
        pTxtNd->ClearSwpHintsArr( true );

    // ChgTextToNum(..) only acts when the strings are different. We
    // need to do the same here.
    if( pTxtNd->GetTxt() != aStr )
    {
        rDoc.DeleteRedline( *( pBox->GetSttNd() ), false, USHRT_MAX );

        SwIndex aIdx( pTxtNd, 0 );
        if( aStr.Len() )
        {
            pTxtNd->EraseText( aIdx );
            pTxtNd->InsertText( aStr, aIdx,
                IDocumentContentOperations::INS_NOHINTEXPAND );
        }
    }

    if( pHistory )
    {
        USHORT nTmpEnd = pHistory->GetTmpEnd();
        pHistory->TmpRollback( &rDoc, 0 );
        pHistory->SetTmpEnd( nTmpEnd );
    }

    SwPaM* pPam = rIter.pAktPam;
    pPam->DeleteMark();
    pPam->GetPoint()->nNode = nNode + 1;
    pPam->GetPoint()->nContent.Assign( pTxtNd, 0 );
}

/** switch the RedlineMode on the given document, using
 * SetRedlineMode_intern. This class set the mode in the constructor,
 * and changes it back in the destructor, i.e. it uses the
 * initialization-is-resource-acquisition idiom.
 */
class RedlineModeInternGuard
{
    SwDoc& mrDoc;
    RedlineMode_t meOldRedlineMode;

public:
    RedlineModeInternGuard(
        SwDoc& rDoc,                      /// change mode of this document
        RedlineMode_t eNewRedlineMode,    /// new redline mode
        RedlineMode_t eRedlineModeMask  = (RedlineMode_t)(nsRedlineMode_t::REDLINE_ON | nsRedlineMode_t::REDLINE_IGNORE /*change only bits set in this mask*/));

    ~RedlineModeInternGuard();
};

RedlineModeInternGuard::RedlineModeInternGuard(
    SwDoc& rDoc,
    RedlineMode_t eNewRedlineMode,
    RedlineMode_t eRedlineModeMask )
    : mrDoc( rDoc ),
      meOldRedlineMode( rDoc.GetRedlineMode() )
{
    mrDoc.SetRedlineMode_intern((RedlineMode_t)( ( meOldRedlineMode & ~eRedlineModeMask ) |
                                     ( eNewRedlineMode & eRedlineModeMask ) ));
}

RedlineModeInternGuard::~RedlineModeInternGuard()
{
    mrDoc.SetRedlineMode_intern( meOldRedlineMode );
}



void SwUndoTblNumFmt::Redo( SwUndoIter& rIter )
{
    // konnte die Box veraendert werden ?
    if( !pBoxSet )
        return ;

    SwDoc& rDoc = rIter.GetDoc();

    SwPaM* pPam = rIter.pAktPam;
    pPam->DeleteMark();
    pPam->GetPoint()->nNode = nNode;

    SwNode* pNd = rDoc.GetNodes()[ pPam->GetPoint()->nNode ];
    SwStartNode* pSttNd = pNd->FindSttNodeByType( SwTableBoxStartNode );
    ASSERT( pSttNd, "ohne StartNode kein TabellenBox" );
    SwTableBox* pBox = pSttNd->FindTableNode()->GetTable().GetTblBox(
                                    pSttNd->GetIndex() );
    ASSERT( pBox, "keine TabellenBox gefunden" );

    SwFrmFmt* pBoxFmt = pBox->ClaimFrmFmt();
    if(	bNewFmt || bNewFml || bNewValue )
    {
        SfxItemSet aBoxSet( rDoc.GetAttrPool(),
                                RES_BOXATR_FORMAT, RES_BOXATR_VALUE );

        // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
        //				Sorge dafuer, das der Text auch entsprechend
        //				formatiert wird!
        pBoxFmt->LockModify();

        if( bNewFml )
            aBoxSet.Put( SwTblBoxFormula( aNewFml ));
        else
            pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMULA );
        if( bNewFmt )
            aBoxSet.Put( SwTblBoxNumFormat( nNewFmtIdx ));
        else
            pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMAT );
        if( bNewValue )
            aBoxSet.Put( SwTblBoxValue( fNewNum ));
        else
            pBoxFmt->ResetFmtAttr( RES_BOXATR_VALUE );
        pBoxFmt->UnlockModify();

        // dvo: When redlining is (was) enabled, setting the attribute
        // will also change the cell content. To allow this, the
        // REDLINE_IGNORE flag must be removed during Redo. #108450#
        RedlineModeInternGuard aGuard( rDoc, nsRedlineMode_t::REDLINE_NONE, nsRedlineMode_t::REDLINE_IGNORE );
        pBoxFmt->SetFmtAttr( aBoxSet );
    }
    else if( NUMBERFORMAT_TEXT != nFmtIdx )
    {
        SfxItemSet aBoxSet( rDoc.GetAttrPool(),
                            RES_BOXATR_FORMAT, RES_BOXATR_VALUE );

        aBoxSet.Put( SwTblBoxNumFormat( nFmtIdx ));
        aBoxSet.Put( SwTblBoxValue( fNum ));

        // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
        //				Sorge dafuer, das der Text auch entsprechend
        //				formatiert wird!
        pBoxFmt->LockModify();
        pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMULA );
        pBoxFmt->UnlockModify();

        // dvo: When redlining is (was) enabled, setting the attribute
        // will also change the cell content. To allow this, the
        // REDLINE_IGNORE flag must be removed during Redo. #108450#
        RedlineModeInternGuard aGuard( rDoc, nsRedlineMode_t::REDLINE_NONE, nsRedlineMode_t::REDLINE_IGNORE );
        pBoxFmt->SetFmtAttr( aBoxSet );
    }
    else
    {
        // es ist keine Zahl

        // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
        //				Sorge dafuer, das der Text auch entsprechend
        //				formatiert wird!
        pBoxFmt->SetFmtAttr( *GetDfltAttr( RES_BOXATR_FORMAT ));

        pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMAT, RES_BOXATR_VALUE );
    }

    if( bNewFml )
    {
        // egal was gesetzt wurde, ein Update der Tabelle macht sich immer gut
        SwTableFmlUpdate aTblUpdate( &pSttNd->FindTableNode()->GetTable() );
        rDoc.UpdateTblFlds( &aTblUpdate );
    }

    if( !pNd->IsCntntNode() )
        pNd = rDoc.GetNodes().GoNext( &pPam->GetPoint()->nNode );
    pPam->GetPoint()->nContent.Assign( (SwCntntNode*)pNd, 0 );
}

void SwUndoTblNumFmt::SetBox( const SwTableBox& rBox )
{
    nNode = rBox.GetSttIdx();
}

/*  */

_UndoTblCpyTbl_Entry::_UndoTblCpyTbl_Entry( const SwTableBox& rBox )
    : nBoxIdx( rBox.GetSttIdx() ), nOffset( 0 ),
    pBoxNumAttr( 0 ), pUndo( 0 ), bJoin( false )
{
}

_UndoTblCpyTbl_Entry::~_UndoTblCpyTbl_Entry()
{
    delete pUndo;
    delete pBoxNumAttr;
}


SwUndoTblCpyTbl::SwUndoTblCpyTbl()
    : SwUndo( UNDO_TBLCPYTBL ), pInsRowUndo( 0 )
{
    pArr = new _UndoTblCpyTbl_Entries;
}

SwUndoTblCpyTbl::~SwUndoTblCpyTbl()
{
    delete pArr;
    delete pInsRowUndo;
}

void SwUndoTblCpyTbl::Undo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    _DEBUG_REDLINE( &rDoc )

    SwTableNode* pTblNd = 0;
    for( USHORT n = pArr->Count(); n; )
    {
        _UndoTblCpyTbl_Entry* pEntry = (*pArr)[ --n ];
        ULONG nSttPos = pEntry->nBoxIdx + pEntry->nOffset;
        SwStartNode* pSNd = rDoc.GetNodes()[ nSttPos ]->StartOfSectionNode();
        if( !pTblNd )
            pTblNd = pSNd->FindTableNode();

        SwTableBox& rBox = *pTblNd->GetTable().GetTblBox( nSttPos );

        SwNodeIndex aInsIdx( *rBox.GetSttNd(), 1 );
        rDoc.GetNodes().MakeTxtNode( aInsIdx, (SwTxtFmtColl*)rDoc.GetDfltTxtFmtColl() );

        // b62341295: Redline for copying tables
        const SwNode *pEndNode = rBox.GetSttNd()->EndOfSectionNode();
        SwPaM aPam( aInsIdx.GetNode(), *pEndNode );
        SwUndoDelete* pUndo = 0;

        if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
        {
            bool bDeleteCompleteParagraph = false;
            bool bShiftPam = false;
            // There are a couple of different situations to consider during redlining
            if( pEntry->pUndo )
            {
                SwUndoDelete *pUnDel = (SwUndoDelete*)pEntry->pUndo;
                if( UNDO_REDLINE == pUnDel->GetId() )
                {
                    // The old content was not empty or he has been merged with the new content
                    bDeleteCompleteParagraph = !pEntry->bJoin; // bJoin is set when merged
                    // Set aTmpIdx to the beginning fo the old content
                    SwNodeIndex aTmpIdx( *pEndNode, pUnDel->NodeDiff()-1 );
                    SwTxtNode *pTxt = aTmpIdx.GetNode().GetTxtNode();
                    if( pTxt )
                    {
                        aPam.GetPoint()->nNode = *pTxt;
                        aPam.GetPoint()->nContent.Assign( pTxt, pUnDel->ContentStart() );
                    }
                    else
                        *aPam.GetPoint() = SwPosition( aTmpIdx );
                }
                else if( pUnDel->IsDelFullPara() )
                {
                    // When the old content was an empty paragraph, but could not be joined
                    // with the new content (e.g. because of a section or table)
                    // We "save" the aPam.Point, we go one step backwards (because later on the
                    // empty paragraph will be inserted by the undo) and set the "ShiftPam-flag
                    // for step forward later on.
                    bDeleteCompleteParagraph = true;
                    bShiftPam = true;
                    SwNodeIndex aTmpIdx( *pEndNode, -1 );
                    SwTxtNode *pTxt = aTmpIdx.GetNode().GetTxtNode();
                    if( pTxt )
                    {
                        aPam.GetPoint()->nNode = *pTxt;
                        aPam.GetPoint()->nContent.Assign( pTxt, 0 );
                    }
                    else
                        *aPam.GetPoint() = SwPosition( aTmpIdx );
                }
            }
            rDoc.DeleteRedline( aPam, true, USHRT_MAX );

            if( pEntry->pUndo )
            {
                pEntry->pUndo->Undo( rIter );
                delete pEntry->pUndo;
            }
            if( bShiftPam )
            {
                // The aPam.Point is at the moment at the last position of the new content and has to be
                // moved to the first postion of the old content for the SwUndoDelete operation
                SwNodeIndex aTmpIdx( aPam.GetPoint()->nNode, 1 );
                SwTxtNode *pTxt = aTmpIdx.GetNode().GetTxtNode();
                if( pTxt )
                {
                    aPam.GetPoint()->nNode = *pTxt;
                    aPam.GetPoint()->nContent.Assign( pTxt, 0 );
                }
                else
                    *aPam.GetPoint() = SwPosition( aTmpIdx );
            }
            pUndo = new SwUndoDelete( aPam, bDeleteCompleteParagraph, TRUE );
        }
        else
        {
            pUndo = new SwUndoDelete( aPam, true );
            if( pEntry->pUndo )
            {
                pEntry->pUndo->Undo( rIter );
                delete pEntry->pUndo;
            }
        }
        pEntry->pUndo = pUndo;

        aInsIdx = rBox.GetSttIdx() + 1;
        rDoc.GetNodes().Delete( aInsIdx, 1 );

        SfxItemSet aTmpSet( rDoc.GetAttrPool(), RES_BOXATR_FORMAT, RES_BOXATR_VALUE,
                                                RES_VERT_ORIENT, RES_VERT_ORIENT, 0 );
        aTmpSet.Put( rBox.GetFrmFmt()->GetAttrSet() );
        if( aTmpSet.Count() )
        {
            SwFrmFmt* pBoxFmt = rBox.ClaimFrmFmt();
            pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMAT, RES_BOXATR_VALUE );
            pBoxFmt->ResetFmtAttr( RES_VERT_ORIENT );
        }

        if( pEntry->pBoxNumAttr )
        {
            rBox.ClaimFrmFmt()->SetFmtAttr( *pEntry->pBoxNumAttr );
            delete pEntry->pBoxNumAttr, pEntry->pBoxNumAttr = 0;
        }

        if( aTmpSet.Count() )
        {
            pEntry->pBoxNumAttr = new SfxItemSet( rDoc.GetAttrPool(),
                                    RES_BOXATR_FORMAT, RES_BOXATR_VALUE,
                                    RES_VERT_ORIENT, RES_VERT_ORIENT, 0 );
            pEntry->pBoxNumAttr->Put( aTmpSet );
        }

        pEntry->nOffset = rBox.GetSttIdx() - pEntry->nBoxIdx;
    }

    if( pInsRowUndo )
        pInsRowUndo->Undo( rIter );
    _DEBUG_REDLINE( &rDoc )
}

void SwUndoTblCpyTbl::Redo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    _DEBUG_REDLINE( &rDoc )

    if( pInsRowUndo )
        pInsRowUndo->Redo( rIter );

    SwTableNode* pTblNd = 0;
    for( USHORT n = 0; n < pArr->Count(); ++n )
    {
        _UndoTblCpyTbl_Entry* pEntry = (*pArr)[ n ];
        ULONG nSttPos = pEntry->nBoxIdx + pEntry->nOffset;
        SwStartNode* pSNd = rDoc.GetNodes()[ nSttPos ]->StartOfSectionNode();
        if( !pTblNd )
            pTblNd = pSNd->FindTableNode();

        SwTableBox& rBox = *pTblNd->GetTable().GetTblBox( nSttPos );

        SwNodeIndex aInsIdx( *rBox.GetSttNd(), 1 );

        // b62341295: Redline for copying tables - Start.
        rDoc.GetNodes().MakeTxtNode( aInsIdx, (SwTxtFmtColl*)rDoc.GetDfltTxtFmtColl() );
        SwPaM aPam( aInsIdx.GetNode(), *rBox.GetSttNd()->EndOfSectionNode());
        SwUndo* pUndo = IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) ? 0 : new SwUndoDelete( aPam, TRUE );
        if( pEntry->pUndo )
        {
            pEntry->pUndo->Undo( rIter );
            if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ) )
            {
                // PrepareRedline has to be called with the beginning of the old content
                // When new and old content has been joined, the rIter.pAktPam has been set
                // by the Undo operation to this point.
                // Otherwise aInsIdx has been moved during the Undo operation
                if( pEntry->bJoin )
                    pUndo = PrepareRedline( &rDoc, rBox, *rIter.pAktPam->GetPoint(),
                                            pEntry->bJoin, true );
                else
                {
                    SwPosition aTmpPos( aInsIdx );
                    pUndo = PrepareRedline( &rDoc, rBox, aTmpPos, pEntry->bJoin, true );
                }
            }
            delete pEntry->pUndo;
        }
        pEntry->pUndo = pUndo;
        // b62341295: Redline for copying tables - End.

        aInsIdx = rBox.GetSttIdx() + 1;
        rDoc.GetNodes().Delete( aInsIdx, 1 );

        SfxItemSet aTmpSet( rDoc.GetAttrPool(), RES_BOXATR_FORMAT, RES_BOXATR_VALUE,
                                                RES_VERT_ORIENT, RES_VERT_ORIENT, 0 );
        aTmpSet.Put( rBox.GetFrmFmt()->GetAttrSet() );
        if( aTmpSet.Count() )
        {
            SwFrmFmt* pBoxFmt = rBox.ClaimFrmFmt();
            pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMAT, RES_BOXATR_VALUE );
            pBoxFmt->ResetFmtAttr( RES_VERT_ORIENT );
        }
        if( pEntry->pBoxNumAttr )
        {
            rBox.ClaimFrmFmt()->SetFmtAttr( *pEntry->pBoxNumAttr );
            delete pEntry->pBoxNumAttr, pEntry->pBoxNumAttr = 0;
        }

        if( aTmpSet.Count() )
        {
            pEntry->pBoxNumAttr = new SfxItemSet( rDoc.GetAttrPool(),
                                    RES_BOXATR_FORMAT, RES_BOXATR_VALUE,
                                    RES_VERT_ORIENT, RES_VERT_ORIENT, 0 );
            pEntry->pBoxNumAttr->Put( aTmpSet );
        }

        pEntry->nOffset = rBox.GetSttIdx() - pEntry->nBoxIdx;
    }
    _DEBUG_REDLINE( &rDoc )
}

void SwUndoTblCpyTbl::AddBoxBefore( const SwTableBox& rBox, BOOL bDelCntnt )
{
    if( pArr->Count() && !bDelCntnt )
        return;

    _UndoTblCpyTbl_Entry* pEntry = new _UndoTblCpyTbl_Entry( rBox );
    pArr->Insert( pEntry, pArr->Count() );

    SwDoc* pDoc = rBox.GetFrmFmt()->GetDoc();
    _DEBUG_REDLINE( pDoc )
    if( bDelCntnt )
    {
        SwNodeIndex aInsIdx( *rBox.GetSttNd(), 1 );
        pDoc->GetNodes().MakeTxtNode( aInsIdx, (SwTxtFmtColl*)pDoc->GetDfltTxtFmtColl() );
        SwPaM aPam( aInsIdx.GetNode(), *rBox.GetSttNd()->EndOfSectionNode() );

        if( !pDoc->IsRedlineOn() )
            pEntry->pUndo = new SwUndoDelete( aPam, TRUE );
    }

    pEntry->pBoxNumAttr = new SfxItemSet( pDoc->GetAttrPool(),
                                    RES_BOXATR_FORMAT, RES_BOXATR_VALUE,
                                    RES_VERT_ORIENT, RES_VERT_ORIENT, 0 );
    pEntry->pBoxNumAttr->Put( rBox.GetFrmFmt()->GetAttrSet() );
    if( !pEntry->pBoxNumAttr->Count() )
        delete pEntry->pBoxNumAttr, pEntry->pBoxNumAttr = 0;
    _DEBUG_REDLINE( pDoc )
}

void SwUndoTblCpyTbl::AddBoxAfter( const SwTableBox& rBox, const SwNodeIndex& rIdx, BOOL bDelCntnt )
{
    _UndoTblCpyTbl_Entry* pEntry = (*pArr)[ pArr->Count() - 1 ];

    // wurde der Inhalt geloescht, so loesche jetzt auch noch den temp.
    // erzeugten Node
    if( bDelCntnt )
    {
        SwDoc* pDoc = rBox.GetFrmFmt()->GetDoc();
        _DEBUG_REDLINE( pDoc )

        if( pDoc->IsRedlineOn() )
        {
            SwPosition aTmpPos( rIdx );
            pEntry->pUndo = PrepareRedline( pDoc, rBox, aTmpPos, pEntry->bJoin, false );
        }
        SwNodeIndex aDelIdx( *rBox.GetSttNd(), 1 );
        rBox.GetFrmFmt()->GetDoc()->GetNodes().Delete( aDelIdx, 1 );
        _DEBUG_REDLINE( pDoc )
    }

    pEntry->nOffset = rBox.GetSttIdx() - pEntry->nBoxIdx;
}

// PrepareRedline is called from AddBoxAfter() and from Redo() in slightly different situations.
// bRedo is set by calling from Redo()
// rJoin is false by calling from AddBoxAfter() and will be set if the old and new content has
// been merged.
// rJoin is true if Redo() is calling and the content has already been merged

SwUndo* SwUndoTblCpyTbl::PrepareRedline( SwDoc* pDoc, const SwTableBox& rBox,
    const SwPosition& rPos, bool& rJoin, bool bRedo )
{
    SwUndo *pUndo = 0;
    // b62341295: Redline for copying tables
    // What's to do?
    // Mark the cell content before rIdx as insertion,
    // mark the cell content behind rIdx as deletion
    // merge text nodes at rIdx if possible
    RedlineMode_t eOld = pDoc->GetRedlineMode();
    pDoc->SetRedlineMode_intern((RedlineMode_t)( ( eOld | nsRedlineMode_t::REDLINE_DONTCOMBINE_REDLINES ) &
                                     ~nsRedlineMode_t::REDLINE_IGNORE ));
    SwPosition aInsertEnd( rPos );
    SwTxtNode* pTxt;
    if( !rJoin )
    {
        // If the content is not merged, the end of the insertion is at the end of the node
        // _before_ the given position rPos
        --aInsertEnd.nNode;
        pTxt = aInsertEnd.nNode.GetNode().GetTxtNode();
        if( pTxt )
        {
            aInsertEnd.nContent.Assign( pTxt, pTxt->GetTxt().Len() );
            if( !bRedo && rPos.nNode.GetNode().GetTxtNode() )
            {   // Try to merge, if not called by Redo()
                rJoin = true;
                pTxt->JoinNext();
            }
        }
        else
            aInsertEnd.nContent = SwIndex( 0 );
    }
    // For joined (merged) contents the start of deletionm and end of insertion are identical
    // otherwise adjacent nodes.
    SwPosition aDeleteStart( rJoin ? aInsertEnd : rPos );
    if( !rJoin )
    {
        pTxt = aDeleteStart.nNode.GetNode().GetTxtNode();
        if( pTxt )
            aDeleteStart.nContent.Assign( pTxt, 0 );
    }
    SwPosition aCellEnd( SwNodeIndex( *rBox.GetSttNd()->EndOfSectionNode(), -1 ) );
    pTxt = aCellEnd.nNode.GetNode().GetTxtNode();
    if( pTxt )
        aCellEnd.nContent.Assign( pTxt, pTxt->GetTxt().Len() );
    if( aDeleteStart != aCellEnd )
    {   // If the old (deleted) part is not empty, here we are...
        SwPaM aDeletePam( aDeleteStart, aCellEnd );
        pUndo = new SwUndoRedlineDelete( aDeletePam, UNDO_DELETE );
        pDoc->AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_DELETE, aDeletePam ), true );
    }
    else if( !rJoin ) // If the old part is empty and joined, we are finished
    {   // if it is not joined, we have to delete this empty paragraph
        aCellEnd = SwPosition(
            SwNodeIndex( *rBox.GetSttNd()->EndOfSectionNode() ));
        SwPaM aTmpPam( aDeleteStart, aCellEnd );
        pUndo = new SwUndoDelete( aTmpPam, TRUE );
    }
    SwPosition aCellStart( SwNodeIndex( *rBox.GetSttNd(), 2 ) );
    pTxt = aCellStart.nNode.GetNode().GetTxtNode();
    if( pTxt )
        aCellStart.nContent.Assign( pTxt, 0 );
    if( aCellStart != aInsertEnd ) // An empty insertion will not been marked
    {
        SwPaM aTmpPam( aCellStart, aInsertEnd );
        pDoc->AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_INSERT, aTmpPam ), true );
    }

    pDoc->SetRedlineMode_intern( eOld );
    return pUndo;
}


BOOL SwUndoTblCpyTbl::InsertRow( SwTable& rTbl, const SwSelBoxes& rBoxes,
                                USHORT nCnt )
{
    SwTableNode* pTblNd = (SwTableNode*)rTbl.GetTabSortBoxes()[0]->
                                GetSttNd()->FindTableNode();

    SwTableSortBoxes aTmpLst( 0, 5 );
    pInsRowUndo = new SwUndoTblNdsChg( UNDO_TABLE_INSROW, rBoxes, *pTblNd,
                                       0, 0, nCnt, TRUE, FALSE );
    aTmpLst.Insert( &rTbl.GetTabSortBoxes(), 0, rTbl.GetTabSortBoxes().Count() );

    BOOL bRet = rTbl.InsertRow( rTbl.GetFrmFmt()->GetDoc(), rBoxes, nCnt, TRUE );
    if( bRet )
        pInsRowUndo->SaveNewBoxes( *pTblNd, aTmpLst );
    else
        delete pInsRowUndo, pInsRowUndo = 0;
    return bRet;
}

BOOL SwUndoTblCpyTbl::IsEmpty() const
{
    return !pInsRowUndo && !pArr->Count();
}

/*  */

SwUndoCpyTbl::SwUndoCpyTbl()
    : SwUndo( UNDO_CPYTBL ), pDel( 0 ), nTblNode( 0 )
{
}

SwUndoCpyTbl::~SwUndoCpyTbl()
{
    delete pDel;
}

void SwUndoCpyTbl::Undo( SwUndoIter& rIter )
{
    SwDoc& rDoc = rIter.GetDoc();
    SwTableNode* pTNd = rDoc.GetNodes()[ nTblNode ]->GetTableNode();

    // harte SeitenUmbrueche am nachfolgenden Node verschieben
    SwCntntNode* pNextNd = rDoc.GetNodes()[ pTNd->EndOfSectionIndex()+1 ]->GetCntntNode();
    if( pNextNd )
    {
        SwFrmFmt* pTableFmt = pTNd->GetTable().GetFrmFmt();
        const SfxPoolItem *pItem;

        if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_PAGEDESC,
            FALSE, &pItem ) )
            pNextNd->SetAttr( *pItem );

        if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_BREAK,
            FALSE, &pItem ) )
            pNextNd->SetAttr( *pItem );
    }

    SwPaM aPam( *pTNd, *pTNd->EndOfSectionNode(), 0 , 1 );
    pDel = new SwUndoDelete( aPam, TRUE );
}

void SwUndoCpyTbl::Redo( SwUndoIter& rIter )
{
    pDel->Undo( rIter );
    delete pDel, pDel = 0;
}


/*  */

SwUndoSplitTbl::SwUndoSplitTbl( const SwTableNode& rTblNd,
    SwSaveRowSpan* pRowSp, USHORT eMode, BOOL bNewSize )
    : SwUndo( UNDO_SPLIT_TABLE ),
    nTblNode( rTblNd.GetIndex() ), nOffset( 0 ), mpSaveRowSpan( pRowSp ), pSavTbl( 0 ),
    pHistory( 0 ), nMode( eMode ), nFmlEnd( 0 ), bCalcNewSize( bNewSize )
{
    switch( nMode )
    {
    case HEADLINE_BOXATRCOLLCOPY:
            pHistory = new SwHistory;
            // kein break;
    case HEADLINE_BORDERCOPY:
    case HEADLINE_BOXATTRCOPY:
        pSavTbl = new _SaveTable( rTblNd.GetTable(), 1, FALSE );
        break;
    }
}

SwUndoSplitTbl::~SwUndoSplitTbl()
{
    delete pSavTbl;
    delete pHistory;
    delete mpSaveRowSpan;
}

void SwUndoSplitTbl::Undo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pPam->DeleteMark();
    SwNodeIndex& rIdx = pPam->GetPoint()->nNode;
    rIdx = nTblNode + nOffset;

    //Den implizit erzeugten Absatz wieder entfernen.
    pDoc->GetNodes().Delete( rIdx, 1 );

    rIdx = nTblNode + nOffset;
    SwTableNode* pTblNd = rIdx.GetNode().GetTableNode();
    SwTable& rTbl = pTblNd->GetTable();

    SwTableFmlUpdate aMsgHnt( &rTbl );
    aMsgHnt.eFlags = TBL_BOXPTR;
    pDoc->UpdateTblFlds( &aMsgHnt );

    switch( nMode )
    {
    case HEADLINE_BOXATRCOLLCOPY:
        if( pHistory )
            pHistory->TmpRollback( pDoc, nFmlEnd );

        // kein break
    case HEADLINE_BOXATTRCOPY:
    case HEADLINE_BORDERCOPY:
        {
            pSavTbl->CreateNew( rTbl, FALSE );
            pSavTbl->RestoreAttr( rTbl );
        }
        break;

    case HEADLINE_CNTNTCOPY:
        // die erzeugte 1. Line muss wieder entfernt werden
        {
            SwSelBoxes aSelBoxes;
            SwTableBox* pBox = rTbl.GetTblBox( nTblNode + nOffset + 1 );
            rTbl.SelLineFromBox( pBox, aSelBoxes, TRUE );
            _FndBox aTmpBox( 0, 0 );
            aTmpBox.SetTableLines( aSelBoxes, rTbl );
            aTmpBox.DelFrms( rTbl );
            rTbl.DeleteSel( pDoc, aSelBoxes, 0, 0, FALSE, FALSE );
        }
        break;
    }

    pDoc->GetNodes().MergeTable( rIdx );

    if( pHistory )
    {
        pHistory->TmpRollback( pDoc, 0 );
        pHistory->SetTmpEnd( pHistory->Count() );
    }
    if( mpSaveRowSpan )
    {
        pTblNd = rIdx.GetNode().FindTableNode();
        if( pTblNd )
            pTblNd->GetTable().RestoreRowSpan( *mpSaveRowSpan );
    }
    ClearFEShellTabCols();
}

void SwUndoSplitTbl::Redo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pPam->DeleteMark();
    pPam->GetPoint()->nNode = nTblNode;
    pDoc->SplitTable( *pPam->GetPoint(), nMode, bCalcNewSize );

    ClearFEShellTabCols();
}

void SwUndoSplitTbl::Repeat( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pDoc->SplitTable( *pPam->GetPoint(), nMode, bCalcNewSize );
    ClearFEShellTabCols();
}

void SwUndoSplitTbl::SaveFormula( SwHistory& rHistory )
{
    if( !pHistory )
        pHistory = new SwHistory;

    nFmlEnd = rHistory.Count();
    pHistory->Move( 0, &rHistory );
}

/*  */

SwUndoMergeTbl::SwUndoMergeTbl( const SwTableNode& rTblNd,
                                const SwTableNode& rDelTblNd,
                                BOOL bWithPrv, USHORT nMd )
    : SwUndo( UNDO_MERGE_TABLE ), pSavTbl( 0 ),
    pHistory( 0 ), nMode( nMd ), bWithPrev( bWithPrv )
{
    // Endnode der letzen Tabellenzelle merken, die auf der Position verbleibt
    if( bWithPrev )
        nTblNode = rDelTblNd.EndOfSectionIndex() - 1;
    else
        nTblNode = rTblNd.EndOfSectionIndex() - 1;

    aName = rDelTblNd.GetTable().GetFrmFmt()->GetName();
    pSavTbl = new _SaveTable( rDelTblNd.GetTable() );

    pSavHdl = bWithPrev ? new _SaveTable( rTblNd.GetTable(), 1 ) : 0;
}

SwUndoMergeTbl::~SwUndoMergeTbl()
{
    delete pSavTbl;
    delete pSavHdl;
    delete pHistory;
}

void SwUndoMergeTbl::Undo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pPam->DeleteMark();
    SwNodeIndex& rIdx = pPam->GetPoint()->nNode;
    rIdx = nTblNode;

    SwTableNode* pTblNd = rIdx.GetNode().FindTableNode();
    SwTable* pTbl = &pTblNd->GetTable();

    SwTableFmlUpdate aMsgHnt( pTbl );
    aMsgHnt.eFlags = TBL_BOXPTR;
    pDoc->UpdateTblFlds( &aMsgHnt );

    //Lines fuer das Layout-Update herausuchen.
    _FndBox aFndBox( 0, 0 );
    aFndBox.SetTableLines( *pTbl );
    aFndBox.DelFrms( *pTbl );
    // ? TL_CHART2: notification or locking of controller required ?

    SwTableNode* pNew = pDoc->GetNodes().SplitTable( rIdx, TRUE, FALSE );

    //Layout updaten
    aFndBox.MakeFrms( *pTbl );
    // ? TL_CHART2: notification or locking of controller required ?

    if( bWithPrev )
    {
        // den Namen umsetzen
        pNew->GetTable().GetFrmFmt()->SetName( pTbl->GetFrmFmt()->GetName() );
        pSavHdl->RestoreAttr( pNew->GetTable() );
    }
    else
        pTbl = &pNew->GetTable();
    pTbl->GetFrmFmt()->SetName( aName );

//	pSavTbl->CreateNew( *pTbl, FALSE );
    pSavTbl->RestoreAttr( *pTbl );


    if( pHistory )
    {
        pHistory->TmpRollback( pDoc, 0 );
        pHistory->SetTmpEnd( pHistory->Count() );
    }

    // fuer die neue Tabelle die Frames anlegen
    SwNodeIndex aTmpIdx( *pNew );
    pNew->MakeFrms( &aTmpIdx );

    // Cursor  irgendwo in den Content stellen
    SwCntntNode* pCNd = pDoc->GetNodes().GoNext( &rIdx );
    pPam->GetPoint()->nContent.Assign( pCNd, 0 );

    ClearFEShellTabCols();

    // TL_CHART2: need to inform chart of probably changed cell names
    SwChartDataProvider *pPCD = pDoc->GetChartDataProvider();
    if (pPCD)
    {
        pDoc->UpdateCharts( pTbl->GetFrmFmt()->GetName() );
        pDoc->UpdateCharts( pNew->GetTable().GetFrmFmt()->GetName() );
    }
}

void SwUndoMergeTbl::Redo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pPam->DeleteMark();
    pPam->GetPoint()->nNode = nTblNode;
    if( bWithPrev )
        pPam->GetPoint()->nNode = nTblNode + 3;
    else
        pPam->GetPoint()->nNode = nTblNode;

    pDoc->MergeTable( *pPam->GetPoint(), bWithPrev, nMode );

    ClearFEShellTabCols();
}

void SwUndoMergeTbl::Repeat( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    pDoc->MergeTable( *pPam->GetPoint(), bWithPrev, nMode );
    ClearFEShellTabCols();
}

void SwUndoMergeTbl::SaveFormula( SwHistory& rHistory )
{
    if( !pHistory )
        pHistory = new SwHistory;
    pHistory->Move( 0, &rHistory );
}

/*  */


void InsertSort( SvUShorts& rArr, USHORT nIdx, USHORT* pInsPos )
{
    USHORT nO	= rArr.Count(), nM, nU = 0;
    if( nO > 0 )
    {
        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;
            if( *(rArr.GetData() + nM) == nIdx )
            {
                ASSERT( FALSE, "Index ist schon vorhanden, darf nie sein!" );
                return;
            }
            if( *(rArr.GetData() + nM) < nIdx )
                nU = nM + 1;
            else if( nM == 0 )
                break;
            else
                nO = nM - 1;
        }
    }
    rArr.Insert( nIdx, nU );
    if( pInsPos )
        *pInsPos = nU;
}

void InsertSort( SvULongs& rArr, ULONG nIdx, USHORT* pInsPos )
{
    USHORT nO	= rArr.Count(), nM, nU = 0;
    if( nO > 0 )
    {
        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;
            if( *(rArr.GetData() + nM) == nIdx )
            {
                ASSERT( FALSE, "Index ist schon vorhanden, darf nie sein!" );
                return;
            }
            if( *(rArr.GetData() + nM) < nIdx )
                nU = nM + 1;
            else if( nM == 0 )
                break;
            else
                nO = nM - 1;
        }
    }
    rArr.Insert( nIdx, nU );
    if( pInsPos )
        *pInsPos = nU;
}

#if defined( JP_DEBUG ) && defined(DBG_UTIL)


void DumpDoc( SwDoc* pDoc, const String& rFileNm )
{
    Writer* pWrt = SwIoSystem::GetWriter( "DEBUG" );
    if( pWrt )
    {
        SvFileStream aStream( rFileNm, STREAM_STD_WRITE );
        SwPaM* pPam = new SwPaM( pDoc, SwPosition( pDoc->GetNodes().EndOfContent ,
                                                 pDoc->GetNodes().EndOfContent ));
        pPam->Move( fnMoveBackward, fnGoDoc );
        pPam->SetMark();
        pPam->Move( fnMoveForward, fnGoDoc );

        pWrt->Write( pPam, *pDoc, aStream, rFileNm.GetStr() );

        delete pPam;
    }
}
void CheckTable( const SwTable& rTbl )
{
    const SwNodes& rNds = rTbl.GetFrmFmt()->GetDoc()->GetNodes();
    const SwTableSortBoxes& rSrtArr = pTblNd->GetTable().GetTabSortBoxes();
    for( USHORT n = 0; n < rSrtArr.Count(); ++n )
    {
        const SwTableBox* pBox = rSrtArr[ n ];
        const SwNode* pNd = pBox->GetSttNd();
        ASSERT( rNds[ *pBox->GetSttIdx() ] == pNd, "Box mit falchem StartNode"  );
    }
}
#endif


