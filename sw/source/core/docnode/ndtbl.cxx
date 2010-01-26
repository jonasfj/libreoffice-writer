/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ndtbl.cxx,v $
 * $Revision: 1.57 $
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

#include <com/sun/star/chart2/XChartDocument.hpp>

#ifdef WTC
#define private public
#endif
#include <hintids.hxx>

#include <svx/lrspitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/protitem.hxx>
#include <svx/boxitem.hxx>
// OD 06.08.2003 #i17174#
#include <svx/shaditem.hxx>
#include <fmtfsize.hxx>
#include <fmtornt.hxx>
#include <fmtfordr.hxx>
#include <fmtpdsc.hxx>
#include <fmtanchr.hxx>
#include <fmtlsplt.hxx>
#include <frmatr.hxx>
#include <charatr.hxx>
#include <cellfrm.hxx>
#include <pagefrm.hxx>
#include <tabcol.hxx>
#include <doc.hxx>
#include <cntfrm.hxx>
#include <pam.hxx>
#include <swcrsr.hxx>
#include <viscrs.hxx>
#include <swtable.hxx>
#include <ndtxt.hxx>
#include <swundo.hxx>
#include <tblsel.hxx>
#include <fldbas.hxx>
#include <poolfmt.hxx>
#include <tabfrm.hxx>
#include <undobj.hxx>
#include <tblafmt.hxx>
#include <swcache.hxx>
#include <ddefld.hxx>
#include <frminf.hxx>
#include <cellatr.hxx>
#include <swtblfmt.hxx>
#include <swddetbl.hxx>
#include <mvsave.hxx>
#include <docary.hxx>
#include <redline.hxx>
#include <rolbck.hxx>
#include <tblrwcl.hxx>
#include <editsh.hxx>
#include <txtfrm.hxx>
#include <ftnfrm.hxx>
#include <section.hxx>
#include <frmtool.hxx>
#include <node2lay.hxx>
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif
#include "docsh.hxx"
#include <tabcol.hxx>
#include <unochart.hxx>

#include <node.hxx>
#include <ndtxt.hxx>

#include <map>
#include <algorithm>
// --> OD 2005-12-05 #i27138#
#include <rootfrm.hxx>
// <--
#include <fldupde.hxx>


#ifndef DBG_UTIL
#define CHECK_TABLE(t)
#else
#ifdef DEBUG
#define CHECK_TABLE(t) (t).CheckConsistency();
#else
#define CHECK_TABLE(t)
#endif
#endif


using namespace ::com::sun::star;

// #i17764# delete table redlines when modifying the table structure?
// #define DEL_TABLE_REDLINES 1

const sal_Unicode T2T_PARA = 0x0a;

extern void ClearFEShellTabCols();

// steht im gctable.cxx
extern BOOL lcl_GC_Line_Border( const SwTableLine*& , void* pPara );

#ifdef DEL_TABLE_REDLINES
class lcl_DelRedlines
{
    SwDoc* pDoc;
public:
    lcl_DelRedlines( const SwTableNode& rNd, BOOL bCheckForOwnRedline );
    lcl_DelRedlines( SwPaM& rPam );

    ~lcl_DelRedlines() { pDoc->EndUndo(UNDO_EMPTY, NULL); }
};

lcl_DelRedlines::lcl_DelRedlines( SwPaM & rPam) : pDoc( rPam.GetDoc() )
{
    pDoc->StartUndo(UNDO_EMPTY, NULL);
    if( !pDoc->IsIgnoreRedline() && pDoc->GetRedlineTbl().Count() )
        pDoc->AcceptRedline( rPam, true );
}
#endif

void lcl_SetDfltBoxAttr( SwFrmFmt& rFmt, BYTE nId )
{
    BOOL bTop = FALSE, bBottom = FALSE, bLeft = FALSE, bRight = FALSE;
    switch ( nId )
    {
    case 0:	bTop = bBottom = bLeft = TRUE; 			break;
    case 1:	bTop = bBottom = bLeft = bRight = TRUE;	break;
    case 2:	bBottom = bLeft = TRUE; 				break;
    case 3: bBottom = bLeft = bRight = TRUE; 		break;
    }

    const BOOL bHTML = rFmt.getIDocumentSettingAccess()->get(IDocumentSettingAccess::HTML_MODE);
    Color aCol( bHTML ? COL_GRAY : COL_BLACK );
    SvxBorderLine aLine( &aCol, DEF_LINE_WIDTH_0 );
    if ( bHTML )
    {
        aLine.SetOutWidth( DEF_DOUBLE_LINE7_OUT );
        aLine.SetInWidth ( DEF_DOUBLE_LINE7_IN  );
        aLine.SetDistance( DEF_DOUBLE_LINE7_DIST);
    }
    SvxBoxItem aBox(RES_BOX); aBox.SetDistance( 55 );
    if ( bTop )
        aBox.SetLine( &aLine, BOX_LINE_TOP );
    if ( bBottom )
        aBox.SetLine( &aLine, BOX_LINE_BOTTOM );
    if ( bLeft )
        aBox.SetLine( &aLine, BOX_LINE_LEFT );
    if ( bRight )
        aBox.SetLine( &aLine, BOX_LINE_RIGHT );
    rFmt.SetFmtAttr( aBox );
}

void lcl_SetDfltBoxAttr( SwTableBox& rBox, SvPtrarr &rBoxFmtArr, BYTE nId,
                            const SwTableAutoFmt* pAutoFmt = 0 )
{
    SvPtrarr* pArr = (SvPtrarr*)rBoxFmtArr[ nId ];
    if( !pArr )
    {
        pArr = new SvPtrarr;
        rBoxFmtArr.Replace( pArr, nId );
    }

    SwTableBoxFmt* pNewBoxFmt = 0;
    SwFrmFmt* pBoxFmt = rBox.GetFrmFmt();
    for( USHORT n = 0; n < pArr->Count(); n += 2 )
        if( pArr->GetObject( n ) == pBoxFmt )
        {
            pNewBoxFmt = (SwTableBoxFmt*)pArr->GetObject( n + 1 );
            break;
        }

    if( !pNewBoxFmt )
    {
        SwDoc* pDoc = pBoxFmt->GetDoc();
        // das Format ist also nicht vorhanden, also neu erzeugen
        pNewBoxFmt = pDoc->MakeTableBoxFmt();
        pNewBoxFmt->SetFmtAttr( pBoxFmt->GetAttrSet().Get( RES_FRM_SIZE ) );

        if( pAutoFmt )
            pAutoFmt->UpdateToSet( nId, (SfxItemSet&)pNewBoxFmt->GetAttrSet(),
                                    SwTableAutoFmt::UPDATE_BOX,
                                    pDoc->GetNumberFormatter( TRUE ) );
        else
            ::lcl_SetDfltBoxAttr( *pNewBoxFmt, nId );

        void* p = pBoxFmt;
        pArr->Insert( p, pArr->Count() );
        p = pNewBoxFmt;
        pArr->Insert( p, pArr->Count() );
    }
    rBox.ChgFrmFmt( pNewBoxFmt );
}

SwTableBoxFmt *lcl_CreateDfltBoxFmt( SwDoc &rDoc, SvPtrarr &rBoxFmtArr,
                                    USHORT nCols, BYTE nId )
{
    if ( !rBoxFmtArr[nId] )
    {
        SwTableBoxFmt* pBoxFmt = rDoc.MakeTableBoxFmt();
        if( USHRT_MAX != nCols )
            pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE,
                                            USHRT_MAX / nCols, 0 ));
        ::lcl_SetDfltBoxAttr( *pBoxFmt, nId );
        rBoxFmtArr.Replace( pBoxFmt, nId );
    }
    return (SwTableBoxFmt*)rBoxFmtArr[nId];
}

SwTableBoxFmt *lcl_CreateAFmtBoxFmt( SwDoc &rDoc, SvPtrarr &rBoxFmtArr,
                                    const SwTableAutoFmt& rAutoFmt,
                                    USHORT nCols, BYTE nId )
{
    if( !rBoxFmtArr[nId] )
    {
        SwTableBoxFmt* pBoxFmt = rDoc.MakeTableBoxFmt();
        rAutoFmt.UpdateToSet( nId, (SfxItemSet&)pBoxFmt->GetAttrSet(),
                                SwTableAutoFmt::UPDATE_BOX,
                                rDoc.GetNumberFormatter( TRUE ) );
        if( USHRT_MAX != nCols )
            pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE,
                                            USHRT_MAX / nCols, 0 ));
        rBoxFmtArr.Replace( pBoxFmt, nId );
    }
    return (SwTableBoxFmt*)rBoxFmtArr[nId];
}

SwTableNode* SwDoc::IsIdxInTbl(const SwNodeIndex& rIdx)
{
    SwTableNode* pTableNd = 0;
    ULONG nIndex = rIdx.GetIndex();
    do {
        SwNode* pNd = (SwNode*)GetNodes()[ nIndex ]->StartOfSectionNode();
        if( 0 != ( pTableNd = pNd->GetTableNode() ) )
            break;

        nIndex = pNd->GetIndex();
    } while ( nIndex );
    return pTableNd;
}


// --------------- einfuegen einer neuen Box --------------

    // fuege in der Line, vor der InsPos eine neue Box ein.

BOOL SwNodes::InsBoxen( SwTableNode* pTblNd,
                        SwTableLine* pLine,
                        SwTableBoxFmt* pBoxFmt,
                        SwTxtFmtColl* pTxtColl,
                        const SfxItemSet* pAutoAttr,
                        USHORT nInsPos,
                        USHORT nCnt )
{
    if( !nCnt )
        return FALSE;
    ASSERT( pLine, "keine gueltige Zeile" );

    // Index hinter die letzte Box der Line
    ULONG nIdxPos = 0;
    SwTableBox *pPrvBox = 0, *pNxtBox = 0;
    if( pLine->GetTabBoxes().Count() )
    {
        if( nInsPos < pLine->GetTabBoxes().Count() )
        {
            if( 0 == (pPrvBox = pLine->FindPreviousBox( pTblNd->GetTable(),
                            pLine->GetTabBoxes()[ nInsPos ] )))
                pPrvBox = pLine->FindPreviousBox( pTblNd->GetTable() );
        }
        else if( 0 == ( pNxtBox = pLine->FindNextBox( pTblNd->GetTable(),
                            pLine->GetTabBoxes()[ nInsPos-1 ] )))
                pNxtBox = pLine->FindNextBox( pTblNd->GetTable() );
    }
    else if( 0 == ( pNxtBox = pLine->FindNextBox( pTblNd->GetTable() )))
        pPrvBox = pLine->FindPreviousBox( pTblNd->GetTable() );

    if( !pPrvBox && !pNxtBox )
    {
        BOOL bSetIdxPos = TRUE;
        if( pTblNd->GetTable().GetTabLines().Count() && !nInsPos )
        {
            const SwTableLine* pTblLn = pLine;
            while( pTblLn->GetUpper() )
                pTblLn = pTblLn->GetUpper()->GetUpper();

            if( pTblNd->GetTable().GetTabLines()[ 0 ] == pTblLn )
            {
                // also vor die erste Box der Tabelle
                while( ( pNxtBox = pLine->GetTabBoxes()[0])->GetTabLines().Count() )
                    pLine = pNxtBox->GetTabLines()[0];
                nIdxPos = pNxtBox->GetSttIdx();
                bSetIdxPos = FALSE;
            }
        }
        if( bSetIdxPos )
            // Tabelle ohne irgendeinen Inhalt oder am Ende, also vors Ende
            nIdxPos = pTblNd->EndOfSectionIndex();
    }
    else if( pNxtBox )			// es gibt einen Nachfolger
        nIdxPos = pNxtBox->GetSttIdx();
    else						// es gibt einen Vorgaenger
        nIdxPos = pPrvBox->GetSttNd()->EndOfSectionIndex() + 1;

    SwNodeIndex aEndIdx( *this, nIdxPos );
    for( USHORT n = 0; n < nCnt; ++n )
    {
        SwStartNode* pSttNd = new SwStartNode( aEndIdx, ND_STARTNODE,
                                                SwTableBoxStartNode );
        pSttNd->pStartOfSection = pTblNd;
        new SwEndNode( aEndIdx, *pSttNd );

        pPrvBox = new SwTableBox( pBoxFmt, *pSttNd, pLine );
        pLine->GetTabBoxes().C40_INSERT( SwTableBox, pPrvBox, nInsPos + n );

        //if( NO_NUMBERING == pTxtColl->GetOutlineLevel()//#outline level,zhaojianwei
        if( ! pTxtColl->IsAssignedToListLevelOfOutlineStyle()//<-end,zhaojianwei
//FEATURE::CONDCOLL
            && RES_CONDTXTFMTCOLL != pTxtColl->Which()
//FEATURE::CONDCOLL
        )
            new SwTxtNode( SwNodeIndex( *pSttNd->EndOfSectionNode() ),
                                pTxtColl, pAutoAttr );
        else
        {
            // Outline-Numerierung richtig behandeln !!!
            SwTxtNode* pTNd = new SwTxtNode(
                            SwNodeIndex( *pSttNd->EndOfSectionNode() ),
                            (SwTxtFmtColl*)GetDoc()->GetDfltTxtFmtColl(),
                            pAutoAttr );
            pTNd->ChgFmtColl( pTxtColl );
        }
    }
    return TRUE;
}

// --------------- einfuegen einer neuen Tabelle --------------

const SwTable* SwDoc::InsertTable( const SwInsertTableOptions& rInsTblOpts,
                                   const SwPosition& rPos, USHORT nRows,
                                   USHORT nCols, sal_Int16 eAdjust,
                                   const SwTableAutoFmt* pTAFmt,
                                   const SvUShorts* pColArr,
                                   BOOL bCalledFromShell,
                                   BOOL bNewModel )
{
    ASSERT( nRows, "Tabelle ohne Zeile?" );
    ASSERT( nCols, "Tabelle ohne Spalten?" );

    {
        // nicht in Fussnoten kopieren !!
        if( rPos.nNode < GetNodes().GetEndOfInserts().GetIndex() &&
            rPos.nNode >= GetNodes().GetEndOfInserts().StartOfSectionIndex() )
            return 0;

        // sollte das ColumnArray die falsche Anzahl haben wird es ignoriert!
        if( pColArr &&
            (nCols + ( text::HoriOrientation::NONE == eAdjust ? 2 : 1 )) != pColArr->Count() )
            pColArr = 0;
    }

    String aTblName = GetUniqueTblName();

    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoInsTbl( rPos, nCols, nRows, static_cast<USHORT>(eAdjust),
                                      rInsTblOpts, pTAFmt, pColArr,
                                      aTblName));
    }

    // fuege erstmal die Nodes ein
    // hole das Auto-Format fuer die Tabelle
    SwTxtFmtColl *pBodyColl = GetTxtCollFromPool( RES_POOLCOLL_TABLE ),
                 *pHeadColl = pBodyColl;

    BOOL bDfltBorders = 0 != ( rInsTblOpts.mnInsMode & tabopts::DEFAULT_BORDER );

    if( (rInsTblOpts.mnInsMode & tabopts::HEADLINE) && (1 != nRows || !bDfltBorders) )
        pHeadColl = GetTxtCollFromPool( RES_POOLCOLL_TABLE_HDLN );

    const USHORT nRowsToRepeat =
            tabopts::HEADLINE == (rInsTblOpts.mnInsMode & tabopts::HEADLINE) ?
            rInsTblOpts.mnRowsToRepeat :
            0;

    /* #106283# Save content node to extract FRAMEDIR from. */
    const SwCntntNode * pCntntNd = rPos.nNode.GetNode().GetCntntNode();

    /* #109161# If we are called from a shell pass the attrset from
        pCntntNd (aka the node the table is inserted at) thus causing
        SwNodes::InsertTable to propagate an adjust item if
        necessary. */
    SwTableNode *pTblNd = GetNodes().InsertTable(
        rPos.nNode,
        nCols,
        pBodyColl,
        nRows,
        nRowsToRepeat,
        pHeadColl,
        bCalledFromShell ? &pCntntNd->GetSwAttrSet() : 0 );

    // dann erstelle die Box/Line/Table-Struktur
    SwTableLineFmt* pLineFmt = MakeTableLineFmt();
    SwTableFmt* pTableFmt = MakeTblFrmFmt( aTblName, GetDfltFrmFmt() );

    /* #106283# If the node to insert the table at is a context node and has a
       non-default FRAMEDIR propagate it to the table. */
    if (pCntntNd)
    {
        const SwAttrSet & aNdSet = pCntntNd->GetSwAttrSet();
        const SfxPoolItem *pItem = NULL;

        if (SFX_ITEM_SET == aNdSet.GetItemState( RES_FRAMEDIR, TRUE, &pItem )
            && pItem != NULL)
        {
            pTableFmt->SetFmtAttr( *pItem );
        }
    }

    //Orientation am Fmt der Table setzen
    pTableFmt->SetFmtAttr( SwFmtHoriOrient( 0, eAdjust ) );
    // alle Zeilen haben die Fill-Order von links nach rechts !
    pLineFmt->SetFmtAttr( SwFmtFillOrder( ATT_LEFT_TO_RIGHT ));

    // die Tabelle bekommt USHRT_MAX als default SSize
    SwTwips nWidth = USHRT_MAX;
    if( pColArr )
    {
        USHORT nSttPos = (*pColArr)[ 0 ];
        USHORT nLastPos = (*pColArr)[ USHORT(pColArr->Count()-1)];
        if( text::HoriOrientation::NONE == eAdjust )
        {
            USHORT nFrmWidth = nLastPos;
            nLastPos = (*pColArr)[ USHORT(pColArr->Count()-2)];
            pTableFmt->SetFmtAttr( SvxLRSpaceItem( nSttPos, nFrmWidth - nLastPos, 0, 0, RES_LR_SPACE ) );
        }
        nWidth = nLastPos - nSttPos;
    }
    else if( nCols )
    {
        nWidth /= nCols;
        nWidth *= nCols; // to avoid rounding problems
    }
    pTableFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, nWidth ));
    if( !(rInsTblOpts.mnInsMode & tabopts::SPLIT_LAYOUT) )
        pTableFmt->SetFmtAttr( SwFmtLayoutSplit( FALSE ));

    // verschiebe ggfs. die harten PageDesc/PageBreak Attribute:
    SwCntntNode* pNextNd = GetNodes()[ pTblNd->EndOfSectionIndex()+1 ]
                            ->GetCntntNode();
    if( pNextNd && pNextNd->HasSwAttrSet() )
    {
        const SfxItemSet* pNdSet = pNextNd->GetpSwAttrSet();
        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == pNdSet->GetItemState( RES_PAGEDESC, FALSE,
            &pItem ) )
        {
            pTableFmt->SetFmtAttr( *pItem );
            pNextNd->ResetAttr( RES_PAGEDESC );
            pNdSet = pNextNd->GetpSwAttrSet();
        }
        if( pNdSet && SFX_ITEM_SET == pNdSet->GetItemState( RES_BREAK, FALSE,
             &pItem ) )
        {
            pTableFmt->SetFmtAttr( *pItem );
            pNextNd->ResetAttr( RES_BREAK );
        }
    }

    SwTable * pNdTbl = &pTblNd->GetTable();
    pTableFmt->Add( pNdTbl );       // das Frame-Format setzen

    pNdTbl->SetRowsToRepeat( nRowsToRepeat );
    pNdTbl->SetTableModel( bNewModel );

    SvPtrarr aBoxFmtArr( 0, 16 );
    SwTableBoxFmt* pBoxFmt = 0;
    if( !bDfltBorders && !pTAFmt )
    {
        pBoxFmt = MakeTableBoxFmt();
        pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, USHRT_MAX / nCols, 0 ));
    }
    else
    {
        const USHORT nBoxArrLen = pTAFmt ? 16 : 4;
        for( USHORT i = 0; i < nBoxArrLen; ++i )
            aBoxFmtArr.Insert( (void*)0, i );
    }
    // --> OD 2008-02-25 #refactorlists#
//    SfxItemSet aCharSet( GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_END-1 );
    SfxItemSet aCharSet( GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_LIST_END-1 );
    // <--

    SwNodeIndex aNdIdx( *pTblNd, 1 );   // auf den ersten Box-StartNode
    SwTableLines& rLines = pNdTbl->GetTabLines();
    for( USHORT n = 0; n < nRows; ++n )
    {
        SwTableLine* pLine = new SwTableLine( pLineFmt, nCols, 0 );
        rLines.C40_INSERT( SwTableLine, pLine, n );
        SwTableBoxes& rBoxes = pLine->GetTabBoxes();
        for( USHORT i = 0; i < nCols; ++i )
        {
            SwTableBoxFmt *pBoxF;
            if( pTAFmt )
            {
                BYTE nId = static_cast<BYTE>(!n ? 0 : (( n+1 == nRows )
                                        ? 12 : (4 * (1 + ((n-1) & 1 )))));
                nId = nId + static_cast<BYTE>( !i ? 0 :
                            ( i+1 == nCols ? 3 : (1 + ((i-1) & 1))));
                pBoxF = ::lcl_CreateAFmtBoxFmt( *this, aBoxFmtArr, *pTAFmt,
                                                nCols, nId );

                // ggfs. noch die Absatz/ZeichenAttribute setzen
                if( pTAFmt->IsFont() || pTAFmt->IsJustify() )
                {
                    aCharSet.ClearItem();
                    pTAFmt->UpdateToSet( nId, aCharSet,
                                        SwTableAutoFmt::UPDATE_CHAR, 0 );
                    if( aCharSet.Count() )
                        GetNodes()[ aNdIdx.GetIndex()+1 ]->GetCntntNode()->
                            SetAttr( aCharSet );
                }
            }
            else if( bDfltBorders )
            {
                BYTE nBoxId = (i < nCols - 1 ? 0 : 1) + (n ? 2 : 0 );
                pBoxF = ::lcl_CreateDfltBoxFmt( *this, aBoxFmtArr, nCols, nBoxId);
            }
            else
                pBoxF = pBoxFmt;

            // fuer AutoFormat bei der Eingabe: beim Einfuegen der Tabelle
            // werden gleich die Spalten gesetzt. Im Array stehen die
            // Positionen der Spalten!! (nicht deren Breite!)
            if( pColArr )
            {
                nWidth = (*pColArr)[ USHORT(i + 1) ] - (*pColArr)[ i ];
                if( pBoxF->GetFrmSize().GetWidth() != nWidth )
                {
                    if( pBoxF->GetDepends() )       // neues Format erzeugen!
                    {
                        SwTableBoxFmt *pNewFmt = MakeTableBoxFmt();
                        *pNewFmt = *pBoxF;
                        pBoxF = pNewFmt;
                    }
                    pBoxF->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, nWidth ));
                }
            }

            SwTableBox *pBox = new SwTableBox( pBoxF, aNdIdx, pLine);
            rBoxes.C40_INSERT( SwTableBox, pBox, i );
            aNdIdx += 3;        // StartNode, TextNode, EndNode  == 3 Nodes
        }
    }
    // und Frms einfuegen.
    GetNodes().GoNext( &aNdIdx );      // zum naechsten ContentNode
    pTblNd->MakeFrms( &aNdIdx );

    if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
    {
        SwPaM aPam( *pTblNd->EndOfSectionNode(), *pTblNd, 1 );
        if( IsRedlineOn() )
            AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_INSERT, aPam ), true);
        else
            SplitRedline( aPam );
    }

    SetModified();
    CHECK_TABLE( *pNdTbl );
    return pNdTbl;
}

SwTableNode* SwNodes::InsertTable( const SwNodeIndex& rNdIdx,
                                   USHORT nBoxes,
                                   SwTxtFmtColl* pCntntTxtColl,
                                   USHORT nLines,
                                   USHORT nRepeat,
                                   SwTxtFmtColl* pHeadlineTxtColl,
                                   const SwAttrSet * pAttrSet)
{
    if( !nBoxes )
        return 0;

    // wenn Lines angegeben, erzeuge die Matrix aus Lines & Boxen
    if( !pHeadlineTxtColl || !nLines )
        pHeadlineTxtColl = pCntntTxtColl;

    SwTableNode * pTblNd = new SwTableNode( rNdIdx );
    SwEndNode* pEndNd = new SwEndNode( rNdIdx, *pTblNd );

    if( !nLines )		// fuer die FOR-Schleife
        ++nLines;

    SwNodeIndex aIdx( *pEndNd );
    SwTxtFmtColl* pTxtColl = pHeadlineTxtColl;
    for( USHORT nL = 0; nL < nLines; ++nL )
    {
        for( USHORT nB = 0; nB < nBoxes; ++nB )
        {
            SwStartNode* pSttNd = new SwStartNode( aIdx, ND_STARTNODE,
                                                    SwTableBoxStartNode );
            pSttNd->pStartOfSection = pTblNd;

            SwTxtNode * pTmpNd = new SwTxtNode( aIdx, pTxtColl );

            // --> FME 2006-04-13 #i60422# Propagate some more attributes.
            // Adjustment was done for #109161#
            const SfxPoolItem* pItem = NULL;
            if ( NULL != pAttrSet )
            {
                static const USHORT aPropagateItems[] = {
                    RES_PARATR_ADJUST,
                    RES_CHRATR_FONT, RES_CHRATR_FONTSIZE,
                    RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONTSIZE,
                    RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONTSIZE, 0 };

                const USHORT* pIdx = aPropagateItems;
                while ( *pIdx != 0 )
                {
                    if ( SFX_ITEM_SET != pTmpNd->GetSwAttrSet().GetItemState( *pIdx ) &&
                         SFX_ITEM_SET == pAttrSet->GetItemState( *pIdx, TRUE, &pItem ) )
                        static_cast<SwCntntNode *>(pTmpNd)->SetAttr(*pItem);
                    ++pIdx;
                }
            }
            // <--

            new SwEndNode( aIdx, *pSttNd );
        }
        if ( nL + 1 >= nRepeat )
            pTxtColl = pCntntTxtColl;
    }
    return pTblNd;
}


//---------------- Text -> Tabelle -----------------------

const SwTable* SwDoc::TextToTable( const SwInsertTableOptions& rInsTblOpts,
                                   const SwPaM& rRange, sal_Unicode cCh,
                                   sal_Int16 eAdjust,
                                   const SwTableAutoFmt* pTAFmt )
{
    // pruefe ob in der Selection eine Tabelle liegt
    const SwPosition *pStt = rRange.Start(), *pEnd = rRange.End();
    {
        ULONG nCnt = pStt->nNode.GetIndex();
        for( ; nCnt <= pEnd->nNode.GetIndex(); ++nCnt )
            if( !GetNodes()[ nCnt ]->IsTxtNode() )
                return 0;
    }

    /* #106283# Save first node in the selection if it is a context node. */
    SwCntntNode * pSttCntntNd = pStt->nNode.GetNode().GetCntntNode();

    SwPaM aOriginal( *pStt, *pEnd );
    pStt = aOriginal.GetMark();
    pEnd = aOriginal.GetPoint();

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( aOriginal );
#endif

    SwUndoTxtToTbl* pUndo = 0;
    if( DoesUndo() )
    {
        StartUndo( UNDO_TEXTTOTABLE, NULL );
        pUndo = new SwUndoTxtToTbl( aOriginal, rInsTblOpts, cCh,
                    static_cast<USHORT>(eAdjust), pTAFmt );
        AppendUndo( pUndo );

        // das Splitten vom TextNode nicht in die Undohistory aufnehmen
        DoUndo( FALSE );
    }

    ::PaMCorrAbs( aOriginal, *pEnd );

    // sorge dafuer, das der Bereich auf Node-Grenzen liegt
    SwNodeRange aRg( pStt->nNode, pEnd->nNode );
    if( pStt->nContent.GetIndex() )
        SplitNode( *pStt, false );

    BOOL bEndCntnt = 0 != pEnd->nContent.GetIndex();
    // nicht splitten am Ende der Zeile (aber am Ende vom Doc!!)
    if( bEndCntnt )
    {
        if( pEnd->nNode.GetNode().GetCntntNode()->Len() != pEnd->nContent.GetIndex()
            || pEnd->nNode.GetIndex() >= GetNodes().GetEndOfContent().GetIndex()-1 )
        {
            SplitNode( *pEnd, false );
            ((SwNodeIndex&)pEnd->nNode)--;
            ((SwIndex&)pEnd->nContent).Assign(
                                pEnd->nNode.GetNode().GetCntntNode(), 0 );
            // ein Node und am Ende ??
            if( pStt->nNode.GetIndex() >= pEnd->nNode.GetIndex() )
                aRg.aStart--;
        }
        else
            aRg.aEnd++;
    }


    if( aRg.aEnd.GetIndex() == aRg.aStart.GetIndex() )
    {
        ASSERT( FALSE, "Kein Bereich" );
        aRg.aEnd++;
    }

    // Wir gehen jetzt immer ueber die Upper, um die Tabelle einzufuegen:
    SwNode2Layout aNode2Layout( aRg.aStart.GetNode() );

    DoUndo( 0 != pUndo );

    // dann erstelle die Box/Line/Table-Struktur
    SwTableBoxFmt* pBoxFmt = MakeTableBoxFmt();
    SwTableLineFmt* pLineFmt = MakeTableLineFmt();
    SwTableFmt* pTableFmt = MakeTblFrmFmt( GetUniqueTblName(), GetDfltFrmFmt() );

    // alle Zeilen haben die Fill-Order von links nach rechts !
    pLineFmt->SetFmtAttr( SwFmtFillOrder( ATT_LEFT_TO_RIGHT ));
    // die Tabelle bekommt USHRT_MAX als default SSize
    pTableFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, USHRT_MAX ));
    if( !(rInsTblOpts.mnInsMode & tabopts::SPLIT_LAYOUT) )
        pTableFmt->SetFmtAttr( SwFmtLayoutSplit( FALSE ));

    /* #106283# If the first node in the selection is a context node and if it
       has an item FRAMEDIR set (no default) propagate the item to the
       replacing table. */
    if (pSttCntntNd)
    {
        const SwAttrSet & aNdSet = pSttCntntNd->GetSwAttrSet();
        const SfxPoolItem *pItem = NULL;

        if (SFX_ITEM_SET == aNdSet.GetItemState( RES_FRAMEDIR, TRUE, &pItem )
            && pItem != NULL)
        {
            pTableFmt->SetFmtAttr( *pItem );
        }
    }

    SwTableNode* pTblNd = GetNodes().TextToTable(
            aRg, cCh, pTableFmt, pLineFmt, pBoxFmt,
            GetTxtCollFromPool( RES_POOLCOLL_STANDARD ), pUndo );

    SwTable * pNdTbl = &pTblNd->GetTable();
    ASSERT( pNdTbl, "kein Tabellen-Node angelegt."  )

    const USHORT nRowsToRepeat =
            tabopts::HEADLINE == (rInsTblOpts.mnInsMode & tabopts::HEADLINE) ?
            rInsTblOpts.mnRowsToRepeat :
            0;
    pNdTbl->SetRowsToRepeat( nRowsToRepeat );

    BOOL bUseBoxFmt = FALSE;
    if( !pBoxFmt->GetDepends() )
    {
        // die Formate an den Boxen haben schon die richtige Size, es darf
        // also nur noch die richtige Umrandung/AutoFmt gesetzt werden.
        bUseBoxFmt = TRUE;
        pTableFmt->SetFmtAttr( pBoxFmt->GetFrmSize() );
        delete pBoxFmt;
        eAdjust = text::HoriOrientation::NONE;
    }

    //Orientation am Fmt der Table setzen
    pTableFmt->SetFmtAttr( SwFmtHoriOrient( 0, eAdjust ) );
    pTableFmt->Add( pNdTbl );		// das Frame-Format setzen

    if( pTAFmt || ( rInsTblOpts.mnInsMode & tabopts::DEFAULT_BORDER) )
    {
        BYTE nBoxArrLen = pTAFmt ? 16 : 4;
        SvPtrarr aBoxFmtArr( nBoxArrLen, 0 );
        {
            for( BYTE i = 0; i < nBoxArrLen; ++i )
                aBoxFmtArr.Insert( (void*)0, i );
        }

        // --> OD 2008-02-25 #refactorlists#
//        SfxItemSet aCharSet( GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_END-1 );
        SfxItemSet aCharSet( GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_LIST_END-1 );
        // <--
        SwHistory* pHistory = pUndo ? &pUndo->GetHistory() : 0;

        SwTableBoxFmt *pBoxF = 0;
        SwTableLines& rLines = pNdTbl->GetTabLines();
        USHORT nRows = rLines.Count();
        for( USHORT n = 0; n < nRows; ++n )
        {
            SwTableBoxes& rBoxes = rLines[ n ]->GetTabBoxes();
            USHORT nCols = rBoxes.Count();
            for( USHORT i = 0; i < nCols; ++i )
            {
                SwTableBox* pBox = rBoxes[ i ];
                BOOL bChgSz = FALSE;

                if( pTAFmt )
                {
                    BYTE nId = static_cast<BYTE>(!n ? 0 : (( n+1 == nRows )
                                            ? 12 : (4 * (1 + ((n-1) & 1 )))));
                    nId = nId + static_cast<BYTE>(!i ? 0 :
                                ( i+1 == nCols ? 3 : (1 + ((i-1) & 1))));
                    if( bUseBoxFmt )
                        ::lcl_SetDfltBoxAttr( *pBox, aBoxFmtArr, nId, pTAFmt );
                    else
                    {
                        bChgSz = 0 == aBoxFmtArr[ nId ];
                        pBoxF = ::lcl_CreateAFmtBoxFmt( *this, aBoxFmtArr,
                                                *pTAFmt, USHRT_MAX, nId );
                    }

                    // ggfs. noch die Absatz/ZeichenAttribute setzen
                    if( pTAFmt->IsFont() || pTAFmt->IsJustify() )
                    {
                        aCharSet.ClearItem();
                        pTAFmt->UpdateToSet( nId, aCharSet,
                                            SwTableAutoFmt::UPDATE_CHAR, 0 );
                        if( aCharSet.Count() )
                        {
                            ULONG nSttNd = pBox->GetSttIdx()+1;
                            ULONG nEndNd = pBox->GetSttNd()->EndOfSectionIndex();
                            for( ; nSttNd < nEndNd; ++nSttNd )
                            {
                                SwCntntNode* pNd = GetNodes()[ nSttNd ]->GetCntntNode();
                                if( pNd )
                                {
                                    if( pHistory )
                                    {
                                        SwRegHistory aReg( pNd, *pNd, pHistory );
                                        pNd->SetAttr( aCharSet );
                                    }
                                    else
                                        pNd->SetAttr( aCharSet );
                                }
                            }
                        }
                    }
                }
                else
                {
                    BYTE nId = (i < nCols - 1 ? 0 : 1) + (n ? 2 : 0 );
                    if( bUseBoxFmt )
                        ::lcl_SetDfltBoxAttr( *pBox, aBoxFmtArr, nId );
                    else
                    {
                        bChgSz = 0 == aBoxFmtArr[ nId ];
                        pBoxF = ::lcl_CreateDfltBoxFmt( *this, aBoxFmtArr,
                                                        USHRT_MAX, nId );
                    }
                }

                if( !bUseBoxFmt )
                {
                    if( bChgSz )
                        pBoxF->SetFmtAttr( pBox->GetFrmFmt()->GetFrmSize() );
                    pBox->ChgFrmFmt( pBoxF );
                }
            }
        }

        if( bUseBoxFmt )
        {
            for( BYTE i = 0; i < nBoxArrLen; ++i )
            {
                SvPtrarr* pArr = (SvPtrarr*)aBoxFmtArr[ i ];
                delete pArr;
            }
        }
    }

    // JP 03.04.97: Inhalt der Boxen auf Zahlen abpruefen
    if( IsInsTblFormatNum() )
    {
        for( USHORT nBoxes = pNdTbl->GetTabSortBoxes().Count(); nBoxes; )
            ChkBoxNumFmt( *pNdTbl->GetTabSortBoxes()[ --nBoxes ], FALSE );
    }

    ULONG nIdx = pTblNd->GetIndex();
    aNode2Layout.RestoreUpperFrms( GetNodes(), nIdx, nIdx + 1 );

    {
        SwPaM& rTmp = (SwPaM&)rRange;	// Point immer an den Anfang
        rTmp.DeleteMark();
        rTmp.GetPoint()->nNode = *pTblNd;
        SwCntntNode* pCNd = GetNodes().GoNext( &rTmp.GetPoint()->nNode );
        rTmp.GetPoint()->nContent.Assign( pCNd, 0 );
    }

    if( pUndo )
        EndUndo( UNDO_TEXTTOTABLE, NULL );

    SetModified();
    SetFieldsDirty(true, NULL, 0);
    return pNdTbl;
}

SwTableNode* SwNodes::TextToTable( const SwNodeRange& rRange, sal_Unicode cCh,
                                    SwTableFmt* pTblFmt,
                                    SwTableLineFmt* pLineFmt,
                                    SwTableBoxFmt* pBoxFmt,
                                    SwTxtFmtColl* pTxtColl,
                                    SwUndoTxtToTbl* pUndo )
{
    if( rRange.aStart >= rRange.aEnd )
        return 0;

    SwTableNode * pTblNd = new SwTableNode( rRange.aStart );
    new SwEndNode( rRange.aEnd, *pTblNd );

    SwDoc* pDoc = GetDoc();
    SvUShorts aPosArr( 0, 16 );
    SwTable * pTable = &pTblNd->GetTable();
    SwTableLine* pLine;
    SwTableBox* pBox;
    USHORT nBoxes, nLines, nMaxBoxes = 0;

    SwNodeIndex aSttIdx( *pTblNd, 1 );
    SwNodeIndex aEndIdx( rRange.aEnd, -1 );
    for( nLines = 0, nBoxes = 0;
        aSttIdx.GetIndex() < aEndIdx.GetIndex();
        aSttIdx += 2, nLines++, nBoxes = 0 )
    {
        SwTxtNode* pTxtNd = aSttIdx.GetNode().GetTxtNode();
        ASSERT( pTxtNd, "nur TextNodes in der Tabelle aufnehmen" );

        if( !nLines && 0x0b == cCh )
        {
            cCh = 0x09;

            // JP 28.10.96: vom 1. Node die Positionen des Trenners besorgen,
            //				damit die Boxen entsprechend eingestellt werden
            SwTxtFrmInfo aFInfo( (SwTxtFrm*)pTxtNd->GetFrm() );
            if( aFInfo.IsOneLine() )		// nur dann sinnvoll!
            {
                const sal_Unicode* pTxt = pTxtNd->GetTxt().GetBuffer();
                for( xub_StrLen nChPos = 0; *pTxt; ++nChPos, ++pTxt )
                {
                    if( *pTxt == cCh )
                    {
                        aPosArr.Insert( static_cast<USHORT>(
                                        aFInfo.GetCharPos( nChPos+1, FALSE )),
                                        aPosArr.Count() );
                    }
                }

                aPosArr.Insert( /*aFInfo.GetFrm()->Frm().Left() +*/
                                static_cast<USHORT>(aFInfo.GetFrm()->IsVertical() ?
                                aFInfo.GetFrm()->Prt().Bottom() :
                                aFInfo.GetFrm()->Prt().Right()),
                                aPosArr.Count() );
            }
        }

        // die alten Frames loeschen, es werden neue erzeugt
        pTxtNd->DelFrms();

        // PageBreaks/PageDesc/ColBreak rausschmeissen.
        const SfxItemSet* pSet = pTxtNd->GetpSwAttrSet();
        if( pSet )
        {
// das entfernen der PageBreaks erst nach dem erzeugen der Tabelle
// erfolgen, denn sonst stehen sie falsch in der History !!!
//			SwRegHistory aRegH( pTxtNd, *pTxtNd, pHistory );
            const SfxPoolItem* pItem;
            if( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK, FALSE, &pItem ) )
            {
                if( !nLines )
                    pTblFmt->SetFmtAttr( *pItem );
                pTxtNd->ResetAttr( RES_BREAK );
                pSet = pTxtNd->GetpSwAttrSet();
            }

            if( pSet && SFX_ITEM_SET == pSet->GetItemState(
                RES_PAGEDESC, FALSE, &pItem ) &&
                ((SwFmtPageDesc*)pItem)->GetPageDesc() )
            {
                if( !nLines )
                    pTblFmt->SetFmtAttr( *pItem );
                pTxtNd->ResetAttr( RES_PAGEDESC );
            }
        }

        // setze den bei allen TextNode in der Tabelle den TableNode
        // als StartNode
        pTxtNd->pStartOfSection = pTblNd;

        pLine = new SwTableLine( pLineFmt, 1, 0 );
        pTable->GetTabLines().C40_INSERT( SwTableLine, pLine, nLines );

        SwStartNode* pSttNd;
        SwPosition aCntPos( aSttIdx, SwIndex( pTxtNd ));

        SvULongs aBkmkArr( 15, 15 );
        _SaveCntntIdx( pDoc, aSttIdx.GetIndex(), pTxtNd->GetTxt().Len(), aBkmkArr );

        const sal_Unicode* pTxt = pTxtNd->GetTxt().GetBuffer();

        if( T2T_PARA != cCh )
            for( xub_StrLen nChPos = 0; *pTxt; ++nChPos, ++pTxt )
                if( *pTxt == cCh )
                {
                    aCntPos.nContent = nChPos;
                    SwCntntNode* pNewNd = pTxtNd->SplitCntntNode( aCntPos );

                    if( aBkmkArr.Count() )
                        _RestoreCntntIdx( aBkmkArr, *pNewNd, nChPos,
                                            nChPos + 1 );

                    // Trennzeichen loeschen und SuchString korrigieren
                    pTxtNd->EraseText( aCntPos.nContent, 1 );
                    pTxt = pTxtNd->GetTxt().GetBuffer();
                    nChPos = 0;
                    --nChPos, --pTxt;           // for the ++ in the for loop !!!

                    // setze bei allen TextNodes in der Tabelle den TableNode
                    // als StartNode
                    const SwNodeIndex aTmpIdx( aCntPos.nNode, -1 );
                    pSttNd = new SwStartNode( aTmpIdx, ND_STARTNODE,
                                                SwTableBoxStartNode );
                    new SwEndNode( aCntPos.nNode, *pSttNd );
                    pNewNd->pStartOfSection = pSttNd;

                    // Section der Box zuweisen
                    pBox = new SwTableBox( pBoxFmt, *pSttNd, pLine );
                    pLine->GetTabBoxes().C40_INSERT( SwTableBox, pBox, nBoxes++ );
                }

        // und jetzt den letzten Teil-String
        if( aBkmkArr.Count() )
            _RestoreCntntIdx( aBkmkArr, *pTxtNd, pTxtNd->GetTxt().Len(),
                                pTxtNd->GetTxt().Len()+1 );

        pSttNd = new SwStartNode( aCntPos.nNode, ND_STARTNODE, SwTableBoxStartNode );
        const SwNodeIndex aTmpIdx( aCntPos.nNode, 1 );
        new SwEndNode( aTmpIdx, *pSttNd  );
        pTxtNd->pStartOfSection = pSttNd;

        pBox = new SwTableBox( pBoxFmt, *pSttNd, pLine );
        pLine->GetTabBoxes().C40_INSERT( SwTableBox, pBox, nBoxes++ );
        if( nMaxBoxes < nBoxes )
            nMaxBoxes = nBoxes;
    }

    // die Tabelle ausgleichen, leere Sections einfuegen
    USHORT n;

    for( n = 0; n < pTable->GetTabLines().Count(); ++n )
    {
        SwTableLine* pCurrLine = pTable->GetTabLines()[ n ];
        if( nMaxBoxes != ( nBoxes = pCurrLine->GetTabBoxes().Count() ))
        {
            InsBoxen( pTblNd, pCurrLine, pBoxFmt, pTxtColl, 0,
                        nBoxes, nMaxBoxes - nBoxes );

            if( pUndo )
                for( USHORT i = nBoxes; i < nMaxBoxes; ++i )
                    pUndo->AddFillBox( *pCurrLine->GetTabBoxes()[ i ] );

            // fehlen der 1. Line Boxen, dann kann man das Breiten Array
            // vergessen!
            if( !n )
                aPosArr.Remove( 0, aPosArr.Count() );
        }
    }

    if( aPosArr.Count() )
    {
        SwTableLines& rLns = pTable->GetTabLines();
        USHORT nLastPos = 0;
        for( n = 0; n < aPosArr.Count(); ++n )
        {
            SwTableBoxFmt *pNewFmt = pDoc->MakeTableBoxFmt();
            pNewFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE,
                                                aPosArr[ n ] - nLastPos ));
            for( USHORT nTmpLine = 0; nTmpLine < rLns.Count(); ++nTmpLine )
                //JP 24.06.98: hier muss ein Add erfolgen, da das BoxFormat
                //				von der rufenden Methode noch gebraucht wird!
                pNewFmt->Add( rLns[ nTmpLine ]->GetTabBoxes()[ n ] );

            nLastPos = aPosArr[ n ];
        }

        // damit die Tabelle die richtige Groesse bekommt, im BoxFormat die
        // Groesse nach "oben" transportieren.
        ASSERT( !pBoxFmt->GetDepends(), "wer ist in dem Format noch angemeldet" );
        pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, nLastPos ));
    }
    else
        pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, USHRT_MAX / nMaxBoxes ));

    // das wars doch wohl ??
    return pTblNd;
}
/*-- 18.05.2006 10:30:29---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwTable* SwDoc::TextToTable( const std::vector< std::vector<SwNodeRange> >& rTableNodes )
{
    /* #106283# Save first node in the selection if it is a content node. */
    SwCntntNode * pSttCntntNd = rTableNodes.begin()->begin()->aStart.GetNode().GetCntntNode();

    /**debug**/
#if OSL_DEBUG_LEVEL > 1
    const SwNodeRange& rStartRange = *rTableNodes.begin()->begin();
    const SwNodeRange& rEndRange = *rTableNodes.rbegin()->rbegin();
    (void) rStartRange;
    (void) rEndRange;
#endif
    /**debug**/

    //!!! not necessarily TextNodes !!!
    SwPaM aOriginal( rTableNodes.begin()->begin()->aStart, rTableNodes.rbegin()->rbegin()->aEnd );
    const SwPosition *pStt = aOriginal.GetMark();
    const SwPosition *pEnd = aOriginal.GetPoint();

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( aOriginal );
#endif

    SwUndoTxtToTbl* pUndo = 0;
    if( DoesUndo() )
    {
//        StartUndo( UNDO_TEXTTOTABLE );
//        pUndo = new SwUndoTxtToTbl( aOriginal, rInsTblOpts, cCh, eAdjust, pTAFmt );
//        AppendUndo( pUndo );

        // das Splitten vom TextNode nicht in die Undohistory aufnehmen
        DoUndo( FALSE );
    }

    ::PaMCorrAbs( aOriginal, *pEnd );

    // sorge dafuer, das der Bereich auf Node-Grenzen liegt
    SwNodeRange aRg( pStt->nNode, pEnd->nNode );
    if( pStt->nContent.GetIndex() )
        SplitNode( *pStt, false );

    BOOL bEndCntnt = 0 != pEnd->nContent.GetIndex();
    // nicht splitten am Ende der Zeile (aber am Ende vom Doc!!)
    if( bEndCntnt )
    {
        if( pEnd->nNode.GetNode().GetCntntNode()->Len() != pEnd->nContent.GetIndex()
            || pEnd->nNode.GetIndex() >= GetNodes().GetEndOfContent().GetIndex()-1 )
        {
            SplitNode( *pEnd, false );
            ((SwNodeIndex&)pEnd->nNode)--;
            ((SwIndex&)pEnd->nContent).Assign(
                                pEnd->nNode.GetNode().GetCntntNode(), 0 );
            // ein Node und am Ende ??
            if( pStt->nNode.GetIndex() >= pEnd->nNode.GetIndex() )
                aRg.aStart--;
        }
        else
            aRg.aEnd++;
    }


    if( aRg.aEnd.GetIndex() == aRg.aStart.GetIndex() )
    {
        ASSERT( FALSE, "Kein Bereich" );
        aRg.aEnd++;
    }

    // Wir gehen jetzt immer ueber die Upper, um die Tabelle einzufuegen:
    SwNode2Layout aNode2Layout( aRg.aStart.GetNode() );

    DoUndo( 0 != pUndo );

    // dann erstelle die Box/Line/Table-Struktur
    SwTableBoxFmt* pBoxFmt = MakeTableBoxFmt();
    SwTableLineFmt* pLineFmt = MakeTableLineFmt();
    SwTableFmt* pTableFmt = MakeTblFrmFmt( GetUniqueTblName(), GetDfltFrmFmt() );

    // alle Zeilen haben die Fill-Order von links nach rechts !
    pLineFmt->SetFmtAttr( SwFmtFillOrder( ATT_LEFT_TO_RIGHT ));
    // die Tabelle bekommt USHRT_MAX als default SSize
    pTableFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, USHRT_MAX ));
//    if( !(rInsTblOpts.mnInsMode & tabopts::SPLIT_LAYOUT) )
//        pTableFmt->SetAttr( SwFmtLayoutSplit( FALSE ));

    /* #106283# If the first node in the selection is a context node and if it
       has an item FRAMEDIR set (no default) propagate the item to the
       replacing table. */
    if (pSttCntntNd)
    {
        const SwAttrSet & aNdSet = pSttCntntNd->GetSwAttrSet();
        const SfxPoolItem *pItem = NULL;

        if (SFX_ITEM_SET == aNdSet.GetItemState( RES_FRAMEDIR, TRUE, &pItem )
            && pItem != NULL)
        {
            pTableFmt->SetFmtAttr( *pItem );
        }
    }

    SwTableNode* pTblNd = GetNodes().TextToTable(
            rTableNodes, pTableFmt, pLineFmt, pBoxFmt,
            GetTxtCollFromPool( RES_POOLCOLL_STANDARD )/*, pUndo*/ );

    SwTable * pNdTbl = &pTblNd->GetTable();
    ASSERT( pNdTbl, "kein Tabellen-Node angelegt."  )
   pTableFmt->Add( pNdTbl );       // das Frame-Format setzen

//    const USHORT nRowsToRepeat =
//            tabopts::HEADLINE == (rInsTblOpts.mnInsMode & tabopts::HEADLINE) ?
//            rInsTblOpts.mnRowsToRepeat :
//            0;
//    pNdTbl->SetRowsToRepeat( nRowsToRepeat );

    BOOL bUseBoxFmt = FALSE;
    if( !pBoxFmt->GetDepends() )
    {
        // die Formate an den Boxen haben schon die richtige Size, es darf
        // also nur noch die richtige Umrandung/AutoFmt gesetzt werden.
        bUseBoxFmt = TRUE;
        pTableFmt->SetFmtAttr( pBoxFmt->GetFrmSize() );
        delete pBoxFmt;
//        eAdjust = HORI_NONE;
    }

    //Orientation am Fmt der Table setzen
//    pTableFmt->SetAttr( SwFmtHoriOrient( 0, eAdjust ) );
//    pTableFmt->Add( pNdTbl );       // das Frame-Format setzen


    ULONG nIdx = pTblNd->GetIndex();
    aNode2Layout.RestoreUpperFrms( GetNodes(), nIdx, nIdx + 1 );

    {
//        SwPaM& rTmp = (SwPaM&)rRange;   // Point immer an den Anfang
//        rTmp.DeleteMark();
//        rTmp.GetPoint()->nNode = *pTblNd;
//        SwCntntNode* pCNd = GetNodes().GoNext( &rTmp.GetPoint()->nNode );
//        rTmp.GetPoint()->nContent.Assign( pCNd, 0 );
    }

//    if( pUndo )
//        EndUndo( UNDO_TEXTTOTABLE );

    SetModified();
    SetFieldsDirty( true, NULL, 0 );
    return pNdTbl;
}

/*-- 18.05.2006 08:23:28---------------------------------------------------

  -----------------------------------------------------------------------*/
SwTableNode* SwNodes::TextToTable( const std::vector< std::vector<SwNodeRange> >& rTableNodes,
                                    SwTableFmt* pTblFmt,
                                    SwTableLineFmt* pLineFmt,
                                    SwTableBoxFmt* pBoxFmt,
                                    SwTxtFmtColl* /*pTxtColl*/  /*, SwUndo... pUndo*/  )
{
    if( !rTableNodes.size() )
        return 0;

    SwTableNode * pTblNd = new SwTableNode( rTableNodes.begin()->begin()->aStart );
    //insert the end node after the last text node
   SwNodeIndex aInsertIndex( rTableNodes.rbegin()->rbegin()->aEnd );
   ++aInsertIndex;

   //!! owner ship will be transferred in c-tor to SwNodes array.
   //!! Thus no real problem here...
   new SwEndNode( aInsertIndex, *pTblNd );

#if OSL_DEBUG_LEVEL > 1
    /**debug**/
    const SwNodeRange& rStartRange = *rTableNodes.begin()->begin();
    const SwNodeRange& rEndRange = *rTableNodes.rbegin()->rbegin();
    (void) rStartRange;
    (void) rEndRange;
    /**debug**/
#endif

    SwDoc* pDoc = GetDoc();
    SvUShorts aPosArr( 0, 16 );
    SwTable * pTable = &pTblNd->GetTable();
    SwTableLine* pLine;
    SwTableBox* pBox;
    USHORT nBoxes, nLines, nMaxBoxes = 0;

//    SwHistory* pHistory = pUndo ? &pUndo->GetHistory() : 0;


    SwNodeIndex aNodeIndex = rTableNodes.begin()->begin()->aStart;
    // delete frames of all contained content nodes
    for( nLines = 0; aNodeIndex <= rTableNodes.rbegin()->rbegin()->aEnd; ++aNodeIndex,++nLines )
    {
        SwNode& rNode = aNodeIndex.GetNode();
        if( rNode.IsCntntNode() )
        {
            static_cast<SwCntntNode&>(rNode).DelFrms();
            if(rNode.IsTxtNode())
            {
                SwTxtNode& rTxtNode = static_cast<SwTxtNode&>(rNode);
                // setze den bei allen TextNode in der Tabelle den TableNode
                // als StartNode
// FIXME: this is setting wrong node StartOfSections in nested tables.
//                rTxtNode.pStartOfSection = pTblNd;
                // remove PageBreaks/PageDesc/ColBreak
                const SwAttrSet* pSet = rTxtNode.GetpSwAttrSet();
                if( pSet )
                {
        // das entfernen der PageBreaks erst nach dem erzeugen der Tabelle
        // erfolgen, denn sonst stehen sie falsch in der History !!!
        //          SwRegHistory aRegH( pTxtNd, *pTxtNd, pHistory );
                    const SfxPoolItem* pItem;
                    if( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK, FALSE, &pItem ) )
                    {
                        if( !nLines )
                            pTblFmt->SetFmtAttr( *pItem );
                        rTxtNode.ResetAttr( RES_BREAK );
                        pSet = rTxtNode.GetpSwAttrSet();
                    }

                    if( pSet && SFX_ITEM_SET == pSet->GetItemState(
                        RES_PAGEDESC, FALSE, &pItem ) &&
                        ((SwFmtPageDesc*)pItem)->GetPageDesc() )
                    {
                        if( !nLines )
                            pTblFmt->SetFmtAttr( *pItem );
                        rTxtNode.ResetAttr( RES_PAGEDESC );
                    }
                }
            }
        }
    }

//    SwNodeIndex aSttIdx( *pTblNd, 1 );
//    SwNodeIndex aEndIdx( rlNodes.rbegin()->aEnd, -1 );
    std::vector<std::vector < SwNodeRange > >::const_iterator aRowIter = rTableNodes.begin();
    for( nLines = 0, nBoxes = 0;
        aRowIter != rTableNodes.end();
        ++aRowIter, /*aSttIdx += 2, */nLines++, nBoxes = 0 )
    {
//        SwTxtNode* pTxtNd = aSttIdx.GetNode().GetTxtNode();
//        ASSERT( pTxtNd, "nur TextNodes in der Tabelle aufnehmen" );

        pLine = new SwTableLine( pLineFmt, 1, 0 );
        pTable->GetTabLines().C40_INSERT( SwTableLine, pLine, nLines );

//        SwStartNode* pSttNd;
//        SwPosition aCntPos( aSttIdx, SwIndex( pTxtNd ));

        std::vector< SwNodeRange >::const_iterator aCellIter = aRowIter->begin();
//        SvULongs aBkmkArr( 15, 15 );
//        _SaveCntntIdx( pDoc, aCellIter->aStart.GetIndex(), pTxtNd->GetTxt().Len(), aBkmkArr );
//        const sal_Unicode* pTxt = pTxtNd->GetTxt().GetBuffer();

        for( ; aCellIter != aRowIter->end(); ++aCellIter )
        {
//            aCellIter->aStart aCellIter->aEnd
//                aCntPos.nContent = nChPos;
//                SwCntntNode* pNewNd = pTxtNd->SplitNode( aCntPos );

//        auch f?rs undo?
//                if( aBkmkArr.Count() )
//                    _RestoreCntntIdx( aBkmkArr, *pNewNd, nChPos,
//                                        nChPos + 1 );

                const SwNodeIndex aTmpIdx( aCellIter->aStart, 0 );

               SwNodeIndex aCellEndIdx(aCellIter->aEnd);
               ++aCellEndIdx;
               SwStartNode* pSttNd = new SwStartNode( aTmpIdx, ND_STARTNODE,
                                            SwTableBoxStartNode );
                new SwEndNode( aCellEndIdx, *pSttNd );
                //set the start node on all node of the current cell
                SwNodeIndex aCellNodeIdx = aCellIter->aStart;
                for(;aCellNodeIdx <= aCellIter->aEnd; ++aCellNodeIdx )
                {
                    aCellNodeIdx.GetNode().pStartOfSection = pSttNd;
                    //skip start/end node pairs
                    if( aCellNodeIdx.GetNode().IsStartNode() )
                        aCellNodeIdx = SwNodeIndex( *aCellNodeIdx.GetNode().EndOfSectionNode() );
                }

                // Section der Box zuweisen
                pBox = new SwTableBox( pBoxFmt, *pSttNd, pLine );
                pLine->GetTabBoxes().C40_INSERT( SwTableBox, pBox, nBoxes++ );
        }
        if( nMaxBoxes < nBoxes )
            nMaxBoxes = nBoxes;
    }

    // die Tabelle ausgleichen, leere Sections einfuegen
    USHORT n;

    if( aPosArr.Count() )
    {
        SwTableLines& rLns = pTable->GetTabLines();
        USHORT nLastPos = 0;
        for( n = 0; n < aPosArr.Count(); ++n )
        {
            SwTableBoxFmt *pNewFmt = pDoc->MakeTableBoxFmt();
            pNewFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE,
                                                aPosArr[ n ] - nLastPos ));
            for( USHORT nLines2 = 0; nLines2 < rLns.Count(); ++nLines2 )
                //JP 24.06.98: hier muss ein Add erfolgen, da das BoxFormat
                //              von der rufenden Methode noch gebraucht wird!
                pNewFmt->Add( rLns[ nLines2 ]->GetTabBoxes()[ n ] );

            nLastPos = aPosArr[ n ];
        }

        // damit die Tabelle die richtige Groesse bekommt, im BoxFormat die
        // Groesse nach "oben" transportieren.
        ASSERT( !pBoxFmt->GetDepends(), "wer ist in dem Format noch angemeldet" );
        pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, nLastPos ));
    }
    else
        pBoxFmt->SetFmtAttr( SwFmtFrmSize( ATT_VAR_SIZE, USHRT_MAX / nMaxBoxes ));

    // das wars doch wohl ??
    return pTblNd;
}


//---------------- Tabelle -> Text -----------------------


BOOL SwDoc::TableToText( const SwTableNode* pTblNd, sal_Unicode cCh )
{
    if( !pTblNd )
        return FALSE;

    // --> FME 2004-09-28 #i34471#
    // If this is trigged by SwUndoTblToTxt::Repeat() nobody ever deleted
    // the table cursor.
    SwEditShell* pESh = GetEditShell();
    if( pESh && pESh->IsTableMode() )
        pESh->ClearMark();
    // <--

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( *pTblNd, FALSE );
#endif

    SwNodeRange aRg( *pTblNd, 0, *pTblNd->EndOfSectionNode() );
    SwUndoTblToTxt* pUndo = 0;
    SwNodeRange* pUndoRg = 0;
    if( DoesUndo() )
    {
        ClearRedo();
        pUndoRg = new SwNodeRange( aRg.aStart, -1, aRg.aEnd, +1 );
        pUndo = new SwUndoTblToTxt( pTblNd->GetTable(), cCh );
    }

    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.eFlags = TBL_BOXNAME;
    UpdateTblFlds( &aMsgHnt );

    BOOL bRet = GetNodes().TableToText( aRg, cCh, pUndo );
    if( pUndoRg )
    {
        pUndoRg->aStart++;
        pUndoRg->aEnd--;
        pUndo->SetRange( *pUndoRg );
        AppendUndo( pUndo );
        delete pUndoRg;
    }

    if( bRet )
        SetModified();

    return bRet;
}

// -- benutze die ForEach Methode vom PtrArray um aus einer Tabelle wieder
//		Text zuerzeugen. (Die Boxen koennen auch noch Lines enthalten !!)
struct _DelTabPara
{
    SwTxtNode* pLastNd;
    SwNodes& rNds;
    SwUndoTblToTxt* pUndo;
    sal_Unicode cCh;

    _DelTabPara( SwNodes& rNodes, sal_Unicode cChar, SwUndoTblToTxt* pU ) :
        pLastNd(0), rNds( rNodes ), pUndo( pU ), cCh( cChar ) {}
    _DelTabPara( const _DelTabPara& rPara ) :
        pLastNd(rPara.pLastNd), rNds( rPara.rNds ),
        pUndo( rPara.pUndo ), cCh( rPara.cCh ) {}
};

// forward deklarieren damit sich die Lines und Boxen rekursiv aufrufen
// koennen.
BOOL lcl_DelBox( const SwTableBox*&, void *pPara );

BOOL lcl_DelLine( const SwTableLine*& rpLine, void* pPara )
{
    ASSERT( pPara, "die Parameter fehlen" );
    _DelTabPara aPara( *(_DelTabPara*)pPara );
    ((SwTableLine*&)rpLine)->GetTabBoxes().ForEach( &lcl_DelBox, &aPara );
    if( rpLine->GetUpper() )		// gibt es noch eine uebergeordnete Box ??
        // dann gebe den letzten TextNode zurueck
        ((_DelTabPara*)pPara)->pLastNd = aPara.pLastNd;
    return TRUE;
}


BOOL lcl_DelBox( const SwTableBox*& rpBox, void* pPara )
{
    ASSERT( pPara, "die Parameter fehlen" );

    // loesche erstmal die Lines der Box
    _DelTabPara* pDelPara = (_DelTabPara*)pPara;
    if( rpBox->GetTabLines().Count() )
        ((SwTableBox*&)rpBox)->GetTabLines().ForEach( &lcl_DelLine, pDelPara );
    else
    {
        SwDoc* pDoc = pDelPara->rNds.GetDoc();
        SwNodeRange aDelRg( *rpBox->GetSttNd(), 0,
                            *rpBox->GetSttNd()->EndOfSectionNode() );
        // loesche die Section
        pDelPara->rNds.SectionUp( &aDelRg );
        const SwTxtNode* pCurTxtNd;
        if( T2T_PARA != pDelPara->cCh && pDelPara->pLastNd &&
            0 != ( pCurTxtNd = aDelRg.aStart.GetNode().GetTxtNode() ))
        {
            // Join the current text node with the last from the previous box if possible
            ULONG nNdIdx = aDelRg.aStart.GetIndex();
            aDelRg.aStart--;
            if( pDelPara->pLastNd == &aDelRg.aStart.GetNode() )
            {
                // Inserting the seperator
                SwIndex aCntIdx( pDelPara->pLastNd, pDelPara->pLastNd->GetTxt().Len());
                pDelPara->pLastNd->InsertText( pDelPara->cCh, aCntIdx,
                    IDocumentContentOperations::INS_EMPTYEXPAND );
                if( pDelPara->pUndo )
                    pDelPara->pUndo->AddBoxPos( *pDoc, nNdIdx, aDelRg.aEnd.GetIndex(),
                                                aCntIdx.GetIndex() );

                SvULongs aBkmkArr( 4, 4 );
                xub_StrLen nOldTxtLen = aCntIdx.GetIndex();
                _SaveCntntIdx( pDoc, nNdIdx, pCurTxtNd->GetTxt().Len(),
                                aBkmkArr );

                pDelPara->pLastNd->JoinNext();

                if( aBkmkArr.Count() )
                    _RestoreCntntIdx( pDoc, aBkmkArr,
                                        pDelPara->pLastNd->GetIndex(),
                                        nOldTxtLen );
            }
            else if( pDelPara->pUndo )
            {
                aDelRg.aStart++;
                pDelPara->pUndo->AddBoxPos( *pDoc, nNdIdx, aDelRg.aEnd.GetIndex() );
            }
        }
        else if( pDelPara->pUndo )
            pDelPara->pUndo->AddBoxPos( *pDoc, aDelRg.aStart.GetIndex(), aDelRg.aEnd.GetIndex() );
        aDelRg.aEnd--;
        pDelPara->pLastNd = aDelRg.aEnd.GetNode().GetTxtNode();

        //JP 03.04.97: die Ausrichtung der ZahlenFormatierung auf
        //				keinen Fall uebernehmen
        if( pDelPara->pLastNd && pDelPara->pLastNd->HasSwAttrSet() )
            pDelPara->pLastNd->ResetAttr( RES_PARATR_ADJUST );
    }
    return TRUE;
}


BOOL SwNodes::TableToText( const SwNodeRange& rRange, sal_Unicode cCh,
                            SwUndoTblToTxt* pUndo )
{
    // ist eine Tabelle selektiert ?
    SwTableNode* pTblNd;
    if( rRange.aStart.GetIndex() >= rRange.aEnd.GetIndex() ||
        0 == ( pTblNd = (*this)[ rRange.aStart ]->GetTableNode()) ||
        &rRange.aEnd.GetNode() != pTblNd->EndOfSectionNode() )
        return FALSE;

    // stand die Tabelle ganz alleine in einer Section ?
    // dann ueber den Upper der Tabelle die Frames anlegen
    SwNode2Layout* pNode2Layout = 0;
    SwNodeIndex aFrmIdx( rRange.aStart );
    SwNode* pFrmNd = FindPrvNxtFrmNode( aFrmIdx, &rRange.aEnd.GetNode() );
    if( !pFrmNd )
        // dann sammel mal alle Uppers ein
        pNode2Layout = new SwNode2Layout( *pTblNd );

    // loesche schon mal die Frames
    pTblNd->DelFrms();

    // dann "loeschen" die Tabellen und fasse alle Lines/Boxen zusammen
    _DelTabPara aDelPara( *this, cCh, pUndo );
    pTblNd->pTable->GetTabLines().ForEach( &lcl_DelLine, &aDelPara );

    // jetzt ist aus jeder TableLine ein TextNode mit dem entsprechenden
    // Trenner erzeugt worden. Es braucht nur noch die Table-Section
    // geloescht und fuer die neuen TextNode die Frames erzeugt werden.
    SwNodeRange aDelRg( rRange.aStart, rRange.aEnd );

    // JP 14.01.97: hat die Tabelle PageDesc-/Break-Attribute? Dann in den
    //				ersten TextNode uebernehmen
    {
// was ist mit UNDO???
        const SfxItemSet& rTblSet = pTblNd->pTable->GetFrmFmt()->GetAttrSet();
        const SfxPoolItem *pBreak, *pDesc;
        if( SFX_ITEM_SET != rTblSet.GetItemState( RES_PAGEDESC, FALSE, &pDesc ))
            pDesc = 0;
        if( SFX_ITEM_SET != rTblSet.GetItemState( RES_BREAK, FALSE, &pBreak ))
            pBreak = 0;

        if( pBreak || pDesc )
        {
            SwNodeIndex aIdx( *pTblNd  );
            SwCntntNode* pCNd = GoNext( &aIdx );
            if( pBreak )
                pCNd->SetAttr( *pBreak );
            if( pDesc )
                pCNd->SetAttr( *pDesc );
        }
    }

    SectionUp( &aDelRg );		// loesche die Section und damit die Tabelle
    // #i28006#
    ULONG nStt = aDelRg.aStart.GetIndex(), nEnd = aDelRg.aEnd.GetIndex();
    if( !pFrmNd )
    {
        pNode2Layout->RestoreUpperFrms( *this,
                        aDelRg.aStart.GetIndex(), aDelRg.aEnd.GetIndex() );
        delete pNode2Layout;
    }
    else
    {
        SwCntntNode *pCNd;
        SwSectionNode *pSNd;
        while( aDelRg.aStart.GetIndex() < nEnd )
        {
            if( 0 != ( pCNd = aDelRg.aStart.GetNode().GetCntntNode()))
            {
                if( pFrmNd->IsCntntNode() )
                    ((SwCntntNode*)pFrmNd)->MakeFrms( *pCNd );
                else if( pFrmNd->IsTableNode() )
                    ((SwTableNode*)pFrmNd)->MakeFrms( aDelRg.aStart );
                else if( pFrmNd->IsSectionNode() )
                    ((SwSectionNode*)pFrmNd)->MakeFrms( aDelRg.aStart );
                pFrmNd = pCNd;
            }
            else if( 0 != ( pSNd = aDelRg.aStart.GetNode().GetSectionNode()))
            {
                if( !pSNd->GetSection().IsHidden() && !pSNd->IsCntntHidden() )
                {
                    pSNd->MakeFrms( &aFrmIdx, &aDelRg.aEnd );
                    pFrmNd = pSNd;
                    break;
                }
                aDelRg.aStart = *pSNd->EndOfSectionNode();
            }
            aDelRg.aStart++;
        }
    }

    // #i28006# Fly frames have to be restored even if the table was
    // #alone in the section
    const SwSpzFrmFmts& rFlyArr = *GetDoc()->GetSpzFrmFmts();
    for( USHORT n = 0; n < rFlyArr.Count(); ++n )
    {
        SwFrmFmt *const pFmt = (SwFrmFmt*)rFlyArr[n];
        const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
        SwPosition const*const pAPos = rAnchor.GetCntntAnchor();
        if (pAPos &&
            ((FLY_AT_PARA == rAnchor.GetAnchorId()) ||
             (FLY_AT_CHAR == rAnchor.GetAnchorId())) &&
            nStt <= pAPos->nNode.GetIndex() &&
            pAPos->nNode.GetIndex() < nEnd )
        {
            pFmt->MakeFrms();
        }
    }

    return TRUE;
}


// ----- einfuegen von Spalten/Zeilen ------------------------

BOOL SwDoc::InsertCol( const SwCursor& rCursor, USHORT nCnt, BOOL bBehind )
{
    if( !::CheckSplitCells( rCursor, nCnt + 1, nsSwTblSearchType::TBLSEARCH_COL ) )
        return FALSE;

    // lasse ueber das Layout die Boxen suchen
    SwSelBoxes aBoxes;
    ::GetTblSel( rCursor, aBoxes, nsSwTblSearchType::TBLSEARCH_COL );

    BOOL bRet = FALSE;
    if( aBoxes.Count() )
        bRet = InsertCol( aBoxes, nCnt, bBehind );
    return bRet;
}

BOOL SwDoc::InsertCol( const SwSelBoxes& rBoxes, USHORT nCnt, BOOL bBehind )
{
    // uebers SwDoc fuer Undo !!
    ASSERT( rBoxes.Count(), "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    SwTable& rTbl = pTblNd->GetTable();
    if( rTbl.ISA( SwDDETable ))
        return FALSE;

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( *pTblNd, TRUE );
#endif

    SwTableSortBoxes aTmpLst( 0, 5 );
    SwUndoTblNdsChg* pUndo = 0;
    if( DoesUndo() )
    {
        DoUndo( FALSE );
        pUndo = new SwUndoTblNdsChg( UNDO_TABLE_INSCOL, rBoxes, *pTblNd,
                                     0, 0, nCnt, bBehind, FALSE );
        aTmpLst.Insert( &rTbl.GetTabSortBoxes(), 0, rTbl.GetTabSortBoxes().Count() );
    }

    SwTableFmlUpdate aMsgHnt( &rTbl );
    aMsgHnt.eFlags = TBL_BOXPTR;
    UpdateTblFlds( &aMsgHnt );

    BOOL bRet = rTbl.InsertCol( this, rBoxes, nCnt, bBehind );
    if( bRet )
    {
        SetModified();
        ::ClearFEShellTabCols();
        SetFieldsDirty( true, NULL, 0 );
    }

    if( pUndo )
    {
        DoUndo( TRUE );
        if( bRet )
        {
            ClearRedo();
            pUndo->SaveNewBoxes( *pTblNd, aTmpLst );
            AppendUndo( pUndo );
        }
        else
            delete pUndo;
    }
    return bRet;
}

BOOL SwDoc::InsertRow( const SwCursor& rCursor, USHORT nCnt, BOOL bBehind )
{
    // lasse ueber das Layout die Boxen suchen
    SwSelBoxes aBoxes;
    GetTblSel( rCursor, aBoxes, nsSwTblSearchType::TBLSEARCH_ROW );

    BOOL bRet = FALSE;
    if( aBoxes.Count() )
        bRet = InsertRow( aBoxes, nCnt, bBehind );
    return bRet;
}

BOOL SwDoc::InsertRow( const SwSelBoxes& rBoxes, USHORT nCnt, BOOL bBehind )
{
    // uebers SwDoc fuer Undo !!
    ASSERT( rBoxes.Count(), "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    SwTable& rTbl = pTblNd->GetTable();
    if( rTbl.ISA( SwDDETable ))
        return FALSE;

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( *pTblNd, TRUE );
#endif

    SwTableSortBoxes aTmpLst( 0, 5 );
    SwUndoTblNdsChg* pUndo = 0;
    if( DoesUndo() )
    {
        DoUndo( FALSE );
        pUndo = new SwUndoTblNdsChg( UNDO_TABLE_INSROW,rBoxes, *pTblNd,
                                     0, 0, nCnt, bBehind, FALSE );
        aTmpLst.Insert( &rTbl.GetTabSortBoxes(), 0, rTbl.GetTabSortBoxes().Count() );
    }

    SwTableFmlUpdate aMsgHnt( &rTbl );
    aMsgHnt.eFlags = TBL_BOXPTR;
    UpdateTblFlds( &aMsgHnt );

    BOOL bRet = rTbl.InsertRow( this, rBoxes, nCnt, bBehind );
    if( bRet )
    {
        SetModified();
        ::ClearFEShellTabCols();
        SetFieldsDirty( true, NULL, 0 );
    }

    if( pUndo )
    {
        DoUndo( TRUE );
        if( bRet )
        {
            ClearRedo();
            pUndo->SaveNewBoxes( *pTblNd, aTmpLst );
            AppendUndo( pUndo );
        }
        else
            delete pUndo;
    }
    return bRet;

}

// ----- loeschen von Spalten/Zeilen ------------------------

BOOL SwDoc::DeleteRow( const SwCursor& rCursor )
{
    // lasse ueber das Layout die Boxen suchen
    SwSelBoxes aBoxes;
    GetTblSel( rCursor, aBoxes, nsSwTblSearchType::TBLSEARCH_ROW );
    if( ::HasProtectedCells( aBoxes ))
        return FALSE;

    // die Crsr aus dem Loeschbereich entfernen.
    // Der Cursor steht danach:
    //	- es folgt noch eine Zeile, in dieser
    //	- vorher steht noch eine Zeile, in dieser
    //	- sonst immer dahinter
    {
        SwTableNode* pTblNd = rCursor.GetNode()->FindTableNode();

        if( pTblNd->GetTable().ISA( SwDDETable ))
            return FALSE;

        // suche alle Boxen / Lines
        _FndBox aFndBox( 0, 0 );
        {
            _FndPara aPara( aBoxes, &aFndBox );
            pTblNd->GetTable().GetTabLines().ForEach( &_FndLineCopyCol, &aPara );
        }

        if( !aFndBox.GetLines().Count() )
            return FALSE;

        SwEditShell* pESh = GetEditShell();
        if( pESh )
        {
            pESh->KillPams();
            // JP: eigentlich sollte man ueber alle Shells iterieren!!
        }

        _FndBox* pFndBox = &aFndBox;
        while( 1 == pFndBox->GetLines().Count() &&
                1 == pFndBox->GetLines()[0]->GetBoxes().Count() )
        {
            _FndBox* pTmp = pFndBox->GetLines()[0]->GetBoxes()[0];
            if( pTmp->GetBox()->GetSttNd() )
                break;		// das ist sonst zu weit
            pFndBox = pTmp;
        }

        SwTableLine* pDelLine = pFndBox->GetLines()[
                        pFndBox->GetLines().Count()-1 ]->GetLine();
        SwTableBox* pDelBox = pDelLine->GetTabBoxes()[
                            pDelLine->GetTabBoxes().Count() - 1 ];
        while( !pDelBox->GetSttNd() )
        {
            SwTableLine* pLn = pDelBox->GetTabLines()[
                        pDelBox->GetTabLines().Count()-1 ];
            pDelBox = pLn->GetTabBoxes()[ pLn->GetTabBoxes().Count() - 1 ];
        }
        SwTableBox* pNextBox = pDelLine->FindNextBox( pTblNd->GetTable(),
                                                        pDelBox, TRUE );
        while( pNextBox &&
                pNextBox->GetFrmFmt()->GetProtect().IsCntntProtected() )
            pNextBox = pNextBox->FindNextBox( pTblNd->GetTable(), pNextBox );

        if( !pNextBox )			// keine nachfolgende? dann die vorhergehende
        {
            pDelLine = pFndBox->GetLines()[ 0 ]->GetLine();
            pDelBox = pDelLine->GetTabBoxes()[ 0 ];
            while( !pDelBox->GetSttNd() )
                pDelBox = pDelBox->GetTabLines()[0]->GetTabBoxes()[0];
            pNextBox = pDelLine->FindPreviousBox( pTblNd->GetTable(),
                                                        pDelBox, TRUE );
            while( pNextBox &&
                    pNextBox->GetFrmFmt()->GetProtect().IsCntntProtected() )
                pNextBox = pNextBox->FindPreviousBox( pTblNd->GetTable(), pNextBox );
        }

        ULONG nIdx;
        if( pNextBox )		// dann den Cursor hier hinein
            nIdx = pNextBox->GetSttIdx() + 1;
        else				// ansonsten hinter die Tabelle
            nIdx = pTblNd->EndOfSectionIndex() + 1;

        SwNodeIndex aIdx( GetNodes(), nIdx );
        SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
        if( !pCNd )
            pCNd = GetNodes().GoNext( &aIdx );

        if( pCNd )
        {
            // die Cursor von der Shell oder den uebergebenen Cursor aendern?
            SwPaM* pPam = (SwPaM*)&rCursor;
            pPam->GetPoint()->nNode = aIdx;
            pPam->GetPoint()->nContent.Assign( pCNd, 0 );
            pPam->SetMark();			// beide wollen etwas davon haben
            pPam->DeleteMark();
        }
    }

    // dann loesche doch die Zeilen

    StartUndo(UNDO_ROW_DELETE, NULL);
    BOOL bResult = DeleteRowCol( aBoxes );
    EndUndo(UNDO_ROW_DELETE, NULL);

    return bResult;
}

BOOL SwDoc::DeleteCol( const SwCursor& rCursor )
{
    // lasse ueber das Layout die Boxen suchen
    SwSelBoxes aBoxes;
    GetTblSel( rCursor, aBoxes, nsSwTblSearchType::TBLSEARCH_COL );
    if( ::HasProtectedCells( aBoxes ))
        return FALSE;

    // die Crsr muessen noch aus dem Loesch Bereich entfernt
    // werden. Setze sie immer hinter/auf die Tabelle; ueber die
    // Dokument-Position werden sie dann immer an die alte Position gesetzt.
    SwEditShell* pESh = GetEditShell();
    if( pESh )
    {
        const SwNode* pNd = rCursor.GetNode()->FindTableBoxStartNode();
        pESh->ParkCrsr( SwNodeIndex( *pNd ) );
    }

    // dann loesche doch die Spalten
    StartUndo(UNDO_COL_DELETE, NULL);
    BOOL bResult = DeleteRowCol( aBoxes, true );
    EndUndo(UNDO_COL_DELETE, NULL);

    return bResult;
}

BOOL SwDoc::DeleteRowCol( const SwSelBoxes& rBoxes, bool bColumn )
{
    if( ::HasProtectedCells( rBoxes ))
        return FALSE;

    // uebers SwDoc fuer Undo !!
    ASSERT( rBoxes.Count(), "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    if( pTblNd->GetTable().ISA( SwDDETable ))
        return FALSE;

    ::ClearFEShellTabCols();
    SwSelBoxes aSelBoxes;
    aSelBoxes.Insert(rBoxes.GetData(), rBoxes.Count());
    SwTable &rTable = pTblNd->GetTable();
    long nMin = 0;
    long nMax = 0;
    if( rTable.IsNewModel() )
    {
        if( bColumn )
            rTable.ExpandColumnSelection( aSelBoxes, nMin, nMax );
        else
            rTable.FindSuperfluousRows( aSelBoxes );
    }

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( *pTblNd, TRUE );
#endif

    // soll die gesamte Tabelle geloescht werden ??
    const ULONG nTmpIdx1 = pTblNd->GetIndex();
    const ULONG nTmpIdx2 = aSelBoxes[ aSelBoxes.Count()-1 ]->GetSttNd()->
                                EndOfSectionIndex()+1;
    if( pTblNd->GetTable().GetTabSortBoxes().Count() == aSelBoxes.Count() &&
        aSelBoxes[0]->GetSttIdx()-1 == nTmpIdx1 &&
        nTmpIdx2 == pTblNd->EndOfSectionIndex() )
    {
        BOOL bNewTxtNd = FALSE;
        // steht diese auch noch alleine in einem FlyFrame ?
        SwNodeIndex aIdx( *pTblNd, -1 );
        const SwStartNode* pSttNd = aIdx.GetNode().GetStartNode();
        if( pSttNd )
        {
            const ULONG nTblEnd = pTblNd->EndOfSectionIndex() + 1;
            const ULONG nSectEnd = pSttNd->EndOfSectionIndex();
            if( nTblEnd == nSectEnd )
            {
                if( SwFlyStartNode == pSttNd->GetStartNodeType() )
                {
                    SwFrmFmt* pFmt = pSttNd->GetFlyFmt();
                    if( pFmt )
                    {
                        // Ok, das ist das gesuchte FlyFormat
                        DelLayoutFmt( pFmt );
                        return TRUE;
                    }
                }
                // kein Fly ?? also Kopf- oder Fusszeile: dann immer einen
                // TextNode ueberig lassen.
                // Undo koennen wir dann vergessen !!
                bNewTxtNd = TRUE;
            }
        }

        // kein Fly ?? also Kopf- oder Fusszeile: dann immer einen
        // TextNode ueberig lassen.
        aIdx++;
        if( DoesUndo() )
        {
            ClearRedo();
            SwPaM aPaM( *pTblNd->EndOfSectionNode(), aIdx.GetNode() );

            if( bNewTxtNd )
            {
                const SwNodeIndex aTmpIdx( *pTblNd->EndOfSectionNode(), 1 );
                GetNodes().MakeTxtNode( aTmpIdx,
                            GetTxtCollFromPool( RES_POOLCOLL_STANDARD ) );
            }

            // save the cursors (UNO and otherwise)
            SwPaM aSavePaM( SwNodeIndex( *pTblNd->EndOfSectionNode() ) );
            if( ! aSavePaM.Move( fnMoveForward, fnGoNode ) )
            {
                *aSavePaM.GetMark() = SwPosition( *pTblNd );
                aSavePaM.Move( fnMoveBackward, fnGoNode );
            }
            ::PaMCorrAbs( SwNodeIndex( *pTblNd ),
                          SwNodeIndex( *pTblNd->EndOfSectionNode() ),
                          *aSavePaM.GetMark() );

            // harte SeitenUmbrueche am nachfolgenden Node verschieben
            BOOL bSavePageBreak = FALSE, bSavePageDesc = FALSE;
            ULONG nNextNd = pTblNd->EndOfSectionIndex()+1;
            SwCntntNode* pNextNd = GetNodes()[ nNextNd ]->GetCntntNode();
            if( pNextNd )
            {
//JP 24.08.98: will man wirklich den PageDesc/Break vom
//				nachfolgen Absatz ueberbuegeln?
//				const SwAttrSet& rAttrSet = pNextNd->GetSwAttrSet();
//				if( SFX_ITEM_SET != rAttrSet.GetItemState( RES_PAGEDESC ) &&
//					SFX_ITEM_SET != rAttrSet.GetItemState( RES_BREAK ))
                {
                    SwFrmFmt* pTableFmt = pTblNd->GetTable().GetFrmFmt();
                    const SfxPoolItem *pItem;
                    if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_PAGEDESC,
                        FALSE, &pItem ) )
                    {
                        pNextNd->SetAttr( *pItem );
                        bSavePageDesc = TRUE;
                    }

                    if( SFX_ITEM_SET == pTableFmt->GetItemState( RES_BREAK,
                        FALSE, &pItem ) )
                    {
                        pNextNd->SetAttr( *pItem );
                        bSavePageBreak = TRUE;
                    }
                }
            }
            SwUndoDelete* pUndo = new SwUndoDelete( aPaM );
            if( bNewTxtNd )
                pUndo->SetTblDelLastNd();
            pUndo->SetPgBrkFlags( bSavePageBreak, bSavePageDesc );
            pUndo->SetTableName(pTblNd->GetTable().GetFrmFmt()->GetName());
            AppendUndo( pUndo );
        }
        else
        {
            if( bNewTxtNd )
            {
                const SwNodeIndex aTmpIdx( *pTblNd->EndOfSectionNode(), 1 );
                GetNodes().MakeTxtNode( aTmpIdx,
                            GetTxtCollFromPool( RES_POOLCOLL_STANDARD ) );
            }

            // save the cursors (UNO and otherwise)
            SwPaM aSavePaM( SwNodeIndex( *pTblNd->EndOfSectionNode() ) );
            if( ! aSavePaM.Move( fnMoveForward, fnGoNode ) )
            {
                *aSavePaM.GetMark() = SwPosition( *pTblNd );
                aSavePaM.Move( fnMoveBackward, fnGoNode );
            }
            ::PaMCorrAbs( SwNodeIndex( *pTblNd ),
                          SwNodeIndex( *pTblNd->EndOfSectionNode() ),
                          *aSavePaM.GetMark() );

            // harte SeitenUmbrueche am nachfolgenden Node verschieben
            SwCntntNode* pNextNd = GetNodes()[ pTblNd->EndOfSectionIndex()+1 ]->GetCntntNode();
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

            pTblNd->DelFrms();
            DeleteSection( pTblNd );
        }
        SetModified();
        SetFieldsDirty( true, NULL, 0 );
        return TRUE;
    }

    SwUndoTblNdsChg* pUndo = 0;
    if( DoesUndo() )
    {
        DoUndo( FALSE );
        pUndo = new SwUndoTblNdsChg( UNDO_TABLE_DELBOX, aSelBoxes, *pTblNd,
                                     nMin, nMax, 0, FALSE, FALSE );
    }

    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.eFlags = TBL_BOXPTR;
    UpdateTblFlds( &aMsgHnt );

    if( rTable.IsNewModel() )
    {
        if( bColumn )
            rTable.PrepareDeleteCol( nMin, nMax );
        rTable.FindSuperfluousRows( aSelBoxes );
        if( pUndo )
            pUndo->ReNewBoxes( aSelBoxes );
    }
    const BOOL bRet = rTable.DeleteSel( this, aSelBoxes, 0, pUndo, TRUE, TRUE );
    if( bRet )
    {
        SetModified();
        SetFieldsDirty( true, NULL, 0 );
    }

    if( pUndo )
    {
        DoUndo( TRUE );
        if( bRet )
        {
            ClearRedo();
            AppendUndo( pUndo );
        }
        else
            delete pUndo;
    }

    return bRet;
}


// ---------- teilen / zusammenfassen von Boxen in der Tabelle --------

BOOL SwDoc::SplitTbl( const SwSelBoxes& rBoxes, sal_Bool bVert, USHORT nCnt,
                      sal_Bool bSameHeight )
{
    // uebers SwDoc fuer Undo !!
    ASSERT( rBoxes.Count() && nCnt, "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    SwTable& rTbl = pTblNd->GetTable();
    if( rTbl.ISA( SwDDETable ))
        return FALSE;

#ifdef DEL_TABLE_REDLINES
    lcl_DelRedlines aDelRedl( *pTblNd, TRUE );
#endif

    SvULongs aNdsCnts;
    SwTableSortBoxes aTmpLst( 0, 5 );
    SwUndoTblNdsChg* pUndo = 0;
    BOOL bDoUndo = DoesUndo();
    if( bDoUndo )
    {
        DoUndo( FALSE );
        pUndo = new SwUndoTblNdsChg( UNDO_TABLE_SPLIT, rBoxes, *pTblNd, 0, 0,
                                     nCnt, bVert, bSameHeight );

        aTmpLst.Insert( &rTbl.GetTabSortBoxes(), 0, rTbl.GetTabSortBoxes().Count() );
        if( !bVert )
        {
            for( USHORT n = 0; n < rBoxes.Count(); ++n )
            {
                const SwStartNode* pSttNd = rBoxes[ n ]->GetSttNd();
                aNdsCnts.Insert( pSttNd->EndOfSectionIndex() -
                                 pSttNd->GetIndex(), n );
            }
        }
    }

    SwTableFmlUpdate aMsgHnt( &rTbl );
    aMsgHnt.eFlags = TBL_BOXPTR;
    UpdateTblFlds( &aMsgHnt );

    BOOL bRet;
    if( bVert )
        bRet = rTbl.SplitCol( this, rBoxes, nCnt );
    else
        bRet = rTbl.SplitRow( this, rBoxes, nCnt, bSameHeight );

    if( bRet )
    {
        SetModified();
        SetFieldsDirty( true, NULL, 0 );
    }

    DoUndo( bDoUndo );
    if( pUndo )
    {
        if( bRet )
        {
            ClearRedo();
            if( bVert )
                pUndo->SaveNewBoxes( *pTblNd, aTmpLst );
            else
                pUndo->SaveNewBoxes( *pTblNd, aTmpLst, rBoxes, aNdsCnts );
            AppendUndo( pUndo );
        }
        else
            delete pUndo;
    }

    return bRet;
}


USHORT SwDoc::MergeTbl( SwPaM& rPam )
{
    // pruefe ob vom aktuellen Crsr der SPoint/Mark in einer Tabelle stehen
    SwTableNode* pTblNd = rPam.GetNode()->FindTableNode();
    if( !pTblNd )
        return TBLMERGE_NOSELECTION;
    SwTable& rTable = pTblNd->GetTable();
    if( rTable.ISA(SwDDETable) )
        return TBLMERGE_NOSELECTION;
    USHORT nRet = TBLMERGE_NOSELECTION;
    if( !rTable.IsNewModel() )
    {
        nRet =::CheckMergeSel( rPam );
        if( TBLMERGE_OK != nRet )
            return nRet;
        nRet = TBLMERGE_NOSELECTION;
    }

    // --> FME 2004-10-08 #i33394#
    StartUndo( UNDO_TABLE_MERGE, NULL );
    // <--

#ifdef DEL_TABLE_REDLINES
    if( !IsIgnoreRedline() && GetRedlineTbl().Count() )
        DeleteRedline( *pTblNd, true, USHRT_MAX );
#endif
    RedlineMode_t eOld = GetRedlineMode();
    SetRedlineMode_intern((RedlineMode_t)(eOld | nsRedlineMode_t::REDLINE_IGNORE));

    SwUndoTblMerge* pUndo = 0;
    if( DoesUndo() )
        pUndo = new SwUndoTblMerge( rPam );

    // lasse ueber das Layout die Boxen suchen
    SwSelBoxes aBoxes;
    SwSelBoxes aMerged;
    SwTableBox* pMergeBox;

    if( !rTable.PrepareMerge( rPam, aBoxes, aMerged, &pMergeBox, pUndo ) )
    {   // no cells found to merge
        SetRedlineMode_intern( eOld );
        if( pUndo )
        {
            delete pUndo;
            if( UNDO_REDLINE == GetUndoIds(NULL, NULL) )
            {
                SwUndoRedline* pU = (SwUndoRedline*)RemoveLastUndo( UNDO_REDLINE );
                if( pU->GetRedlSaveCount() )
                {
                    SwUndoIter aUndoIter( &rPam, UNDO_REDLINE );
                    pU->Undo( aUndoIter );
                }
                delete pU;
            }
        }
    }
    else
    {
        // die PaMs muessen noch aus dem Loesch Bereich entfernt
        // werden. Setze sie immer hinter/auf die Tabelle; ueber die
        // Dokument-Position werden sie dann immer an die alte Position gesetzt.
        // Erstmal einen Index auf die Parkposition merken, denn nach GetMergeSel
        // komme ich nicht mehr dran.
        {
            rPam.DeleteMark();
            rPam.GetPoint()->nNode = *pMergeBox->GetSttNd();
            rPam.GetPoint()->nContent.Assign( 0, 0 );
            rPam.SetMark();
            rPam.DeleteMark();

            SwPaM* pTmp = &rPam;
            while( &rPam != ( pTmp = (SwPaM*)pTmp->GetNext() ))
                for( int i = 0; i < 2; ++i )
                    pTmp->GetBound( (BOOL)i ) = *rPam.GetPoint();
        }

        // dann fuege sie zusammen
        SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
        aMsgHnt.eFlags = TBL_BOXPTR;
        UpdateTblFlds( &aMsgHnt );

        if( pTblNd->GetTable().Merge( this, aBoxes, aMerged, pMergeBox, pUndo ))
        {
            nRet = TBLMERGE_OK;
            SetModified();
            SetFieldsDirty( true, NULL, 0 );
            if( pUndo )
                AppendUndo( pUndo );
        }
        else if( pUndo )
            delete pUndo;

        rPam.GetPoint()->nNode = *pMergeBox->GetSttNd();
        rPam.Move();

        ::ClearFEShellTabCols();
        SetRedlineMode_intern( eOld );
    }
    EndUndo( UNDO_TABLE_MERGE, NULL );
    return nRet;
}



// -------------------------------------------------------

//---------
// SwTableNode
//---------

SwTableNode::SwTableNode( const SwNodeIndex& rIdx )
    : SwStartNode( rIdx, ND_TABLENODE )
{
    pTable = new SwTable( 0 );
}

SwTableNode::~SwTableNode()
{
    //don't forget to notify uno wrappers
    SwFrmFmt* pTblFmt = GetTable().GetFrmFmt();
    SwPtrMsgPoolItem aMsgHint( RES_REMOVE_UNO_OBJECT,
                                pTblFmt );
    pTblFmt->Modify( &aMsgHint, &aMsgHint );
    DelFrms();
    delete pTable;
}

SwTabFrm *SwTableNode::MakeFrm()
{
    return new SwTabFrm( *pTable );
}

//Methode erzeugt fuer den vorhergehenden Node alle Ansichten vom
//Dokument. Die erzeugten Contentframes werden in das entsprechende
//Layout gehaengt.
void SwTableNode::MakeFrms(const SwNodeIndex & rIdx )
{
    if( !GetTable().GetFrmFmt()->GetDepends())//gibt es ueberhaupt Frames ??
        return;

    SwFrm *pFrm, *pNew;
    SwCntntNode * pNode = rIdx.GetNode().GetCntntNode();

    ASSERT( pNode, "Kein Contentnode oder Copy-Node und neuer Node identisch.");

    BOOL bBefore = rIdx < GetIndex();

    SwNode2Layout aNode2Layout( *this, rIdx.GetIndex() );

    while( 0 != (pFrm = aNode2Layout.NextFrm()) )
    {
        pNew = pNode->MakeFrm();
        // wird ein Node vorher oder nachher mit Frames versehen
        if ( bBefore )
            // der neue liegt vor mir
            pNew->Paste( pFrm->GetUpper(), pFrm );
        else
            // der neue liegt hinter mir
            pNew->Paste( pFrm->GetUpper(), pFrm->GetNext() );
    }
}

//Fuer jede Shell einen TblFrm anlegen und vor den entsprechenden
//CntntFrm pasten.

void SwTableNode::MakeFrms( SwNodeIndex* pIdxBehind )
{
    ASSERT( pIdxBehind, "kein Index" );
    *pIdxBehind = *this;
    SwNode *pNd = GetNodes().FindPrvNxtFrmNode( *pIdxBehind, EndOfSectionNode() );
    if( !pNd )
        return ;

    SwFrm *pFrm( 0L );
    SwLayoutFrm *pUpper( 0L );
    SwNode2Layout aNode2Layout( *pNd, GetIndex() );
    while( 0 != (pUpper = aNode2Layout.UpperFrm( pFrm, *this )) )
    {
        SwTabFrm* pNew = MakeFrm();
        pNew->Paste( pUpper, pFrm );
        // --> OD 2005-12-01 #i27138#
        // notify accessibility paragraphs objects about changed
        // CONTENT_FLOWS_FROM/_TO relation.
        // Relation CONTENT_FLOWS_FROM for next paragraph will change
        // and relation CONTENT_FLOWS_TO for previous paragraph will change.
        {
            ViewShell* pViewShell( pNew->GetShell() );
            if ( pViewShell && pViewShell->GetLayout() &&
                 pViewShell->GetLayout()->IsAnyShellAccessible() )
            {
                pViewShell->InvalidateAccessibleParaFlowRelation(
                            dynamic_cast<SwTxtFrm*>(pNew->FindNextCnt( true )),
                            dynamic_cast<SwTxtFrm*>(pNew->FindPrevCnt( true )) );
            }
        }
        // <--
        ((SwTabFrm*)pNew)->RegistFlys();
    }
}

void SwTableNode::DelFrms()
{
    //Erstmal die TabFrms ausschneiden und deleten, die Columns und Rows
    //nehmen sie mit in's Grab.
    //Die TabFrms haengen am FrmFmt des SwTable.
    //Sie muessen etwas umstaendlich zerstort werden, damit die Master
    //die Follows mit in's Grab nehmen.

    SwClientIter aIter( *(pTable->GetFrmFmt()) );
    SwClient *pLast = aIter.GoStart();
    while ( pLast )
    {
        BOOL bAgain = FALSE;
        if ( pLast->IsA( TYPE(SwFrm) ) )
        {
            SwTabFrm *pFrm = (SwTabFrm*)pLast;
            if ( !pFrm->IsFollow() )
            {
                while ( pFrm->HasFollow() )
                    pFrm->JoinAndDelFollows();
                // --> OD 2005-12-01 #i27138#
                // notify accessibility paragraphs objects about changed
                // CONTENT_FLOWS_FROM/_TO relation.
                // Relation CONTENT_FLOWS_FROM for current next paragraph will change
                // and relation CONTENT_FLOWS_TO for current previous paragraph will change.
                {
                    ViewShell* pViewShell( pFrm->GetShell() );
                    if ( pViewShell && pViewShell->GetLayout() &&
                         pViewShell->GetLayout()->IsAnyShellAccessible() )
                    {
                        pViewShell->InvalidateAccessibleParaFlowRelation(
                            dynamic_cast<SwTxtFrm*>(pFrm->FindNextCnt( true )),
                            dynamic_cast<SwTxtFrm*>(pFrm->FindPrevCnt( true )) );
                    }
                }
                // <--
                pFrm->Cut();
                delete pFrm;
                bAgain = TRUE;
            }
        }
        pLast = bAgain ? aIter.GoStart() : aIter++;
    }
}


void SwTableNode::SetNewTable( SwTable* pNewTable, BOOL bNewFrames )
{
    DelFrms();
    delete pTable;
    pTable = pNewTable;
    if( bNewFrames )
    {
        SwNodeIndex aIdx( *EndOfSectionNode());
        GetNodes().GoNext( &aIdx );
        MakeFrms( &aIdx );
    }
}

void SwDoc::GetTabCols( SwTabCols &rFill, const SwCursor* pCrsr,
                        const SwCellFrm* pBoxFrm ) const
{
    const SwTableBox* pBox = 0;
    SwTabFrm *pTab = 0;

    if( pBoxFrm )
    {
        pTab = ((SwFrm*)pBoxFrm)->ImplFindTabFrm();
        pBox = pBoxFrm->GetTabBox();
    }
    else if( pCrsr )
    {
        const SwCntntNode* pCNd = pCrsr->GetCntntNode();
        if( !pCNd )
            return ;

        Point aPt;
        const SwShellCrsr *pShCrsr = dynamic_cast<const SwShellCrsr*>(pCrsr);
        if( pShCrsr )
            aPt = pShCrsr->GetPtPos();

        const SwFrm* pTmpFrm = pCNd->GetFrm( &aPt, 0, FALSE );
        do {
            pTmpFrm = pTmpFrm->GetUpper();
        } while ( !pTmpFrm->IsCellFrm() );

        pBoxFrm = (SwCellFrm*)pTmpFrm;
        pTab = ((SwFrm*)pBoxFrm)->ImplFindTabFrm();
        pBox = pBoxFrm->GetTabBox();
    }
    else if( !pCrsr && !pBoxFrm )
    {
        ASSERT( !this, "einer von beiden muss angegeben werden!" );
        return ;
    }

    //Fix-Punkte setzen, LeftMin in Dokumentkoordinaten die anderen relativ.
    SWRECTFN( pTab )
    const SwPageFrm* pPage = pTab->FindPageFrm();
    const ULONG nLeftMin = (pTab->Frm().*fnRect->fnGetLeft)() -
                           (pPage->Frm().*fnRect->fnGetLeft)();
    const ULONG nRightMax = (pTab->Frm().*fnRect->fnGetRight)() -
                            (pPage->Frm().*fnRect->fnGetLeft)();

    rFill.SetLeftMin ( nLeftMin );
    rFill.SetLeft    ( (pTab->Prt().*fnRect->fnGetLeft)() );
    rFill.SetRight   ( (pTab->Prt().*fnRect->fnGetRight)());
    rFill.SetRightMax( nRightMax - nLeftMin );

    pTab->GetTable()->GetTabCols( rFill, pBox );
}

//
// Here are some little helpers used in SwDoc::GetTabRows
//

#define ROWFUZZY 25

struct FuzzyCompare
{
    bool operator() ( long s1, long s2 ) const;
};

bool FuzzyCompare::operator() ( long s1, long s2 ) const
{
    return ( s1 < s2 && abs( s1 - s2 ) > ROWFUZZY );
}

bool lcl_IsFrmInColumn( const SwCellFrm& rFrm, SwSelBoxes& rBoxes )
{
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {
        if ( rFrm.GetTabBox() == rBoxes[ i ] )
            return true;
    }

    return false;
}

//
// SwDoc::GetTabRows()
//

void SwDoc::GetTabRows( SwTabCols &rFill, const SwCursor* ,
                        const SwCellFrm* pBoxFrm ) const
{
    ASSERT( pBoxFrm, "GetTabRows called without pBoxFrm" )

    // --> FME 2005-09-12 #121591# Make code robust:
    if ( !pBoxFrm )
        return;
    // <--

    // --> FME 2005-01-06 #i39552# Collection of the boxes of the current
    // column has to be done at the beginning of this function, because
    // the table may be formatted in ::GetTblSel.
    SwDeletionChecker aDelCheck( pBoxFrm );

    SwSelBoxes aBoxes;
    const SwCntntFrm* pCntnt = ::GetCellCntnt( *pBoxFrm );
    if ( pCntnt && pCntnt->IsTxtFrm() )
    {
        const SwPosition aPos( *((SwTxtFrm*)pCntnt)->GetTxtNode() );
        const SwCursor aTmpCrsr( aPos, 0, false );
        ::GetTblSel( aTmpCrsr, aBoxes, nsSwTblSearchType::TBLSEARCH_COL );
    }
    // <--

    // --> FME 2005-09-12 #121591# Make code robust:
    if ( aDelCheck.HasBeenDeleted() )
    {
        ASSERT( false, "Current box has been deleted during GetTabRows()" )
        return;
    }
    // <--

    // --> FME 2005-09-12 #121591# Make code robust:
    const SwTabFrm* pTab = pBoxFrm->FindTabFrm();
    ASSERT( pTab, "GetTabRows called without a table" )
    if ( !pTab )
        return;
    // <--

    const SwFrm* pFrm = pTab->GetNextLayoutLeaf();

    //Fix-Punkte setzen, LeftMin in Dokumentkoordinaten die anderen relativ.
    SWRECTFN( pTab )
    const SwPageFrm* pPage = pTab->FindPageFrm();
    const long nLeftMin  = ( bVert ?
                             pTab->GetPrtLeft() - pPage->Frm().Left() :
                             pTab->GetPrtTop() - pPage->Frm().Top() );
    const long nLeft     = bVert ? LONG_MAX : 0;
    const long nRight    = (pTab->Prt().*fnRect->fnGetHeight)();
    const long nRightMax = bVert ? nRight : LONG_MAX;

    rFill.SetLeftMin( nLeftMin );
    rFill.SetLeft( nLeft );
    rFill.SetRight( nRight );
    rFill.SetRightMax( nRightMax );

    typedef std::map< long, std::pair< long, long >, FuzzyCompare > BoundaryMap;
    BoundaryMap aBoundaries;
    BoundaryMap::iterator aIter;
    std::pair< long, long > aPair;

    typedef std::map< long, bool > HiddenMap;
    HiddenMap aHidden;
    HiddenMap::iterator aHiddenIter;

    while ( pFrm && pTab->IsAnLower( pFrm ) )
    {
        if ( pFrm->IsCellFrm() && pFrm->FindTabFrm() == pTab )
        {
            // upper and lower borders of current cell frame:
            long nUpperBorder = (pFrm->Frm().*fnRect->fnGetTop)();
            long nLowerBorder = (pFrm->Frm().*fnRect->fnGetBottom)();

            // get boundaries for nUpperBorder:
            aIter = aBoundaries.find( nUpperBorder );
            if ( aIter == aBoundaries.end() )
            {
                aPair.first = nUpperBorder; aPair.second = LONG_MAX;
                aBoundaries[ nUpperBorder ] = aPair;
            }

            // get boundaries for nLowerBorder:
            aIter = aBoundaries.find( nLowerBorder );
            if ( aIter == aBoundaries.end() )
            {
                aPair.first = nUpperBorder; aPair.second = LONG_MAX;
            }
            else
            {
                nLowerBorder = (*aIter).first;
                long nNewLowerBorderUpperBoundary = Max( (*aIter).second.first, nUpperBorder );
                aPair.first = nNewLowerBorderUpperBoundary; aPair.second = LONG_MAX;
            }
            aBoundaries[ nLowerBorder ] = aPair;

            // calculate hidden flags for entry nUpperBorder/nLowerBorder:
            long nTmpVal = nUpperBorder;
            for ( BYTE i = 0; i < 2; ++i )
            {
                aHiddenIter = aHidden.find( nTmpVal );
                if ( aHiddenIter == aHidden.end() )
                    aHidden[ nTmpVal ] = !lcl_IsFrmInColumn( *((SwCellFrm*)pFrm), aBoxes );
                else
                {
                    if ( aHidden[ nTmpVal ] &&
                         lcl_IsFrmInColumn( *((SwCellFrm*)pFrm), aBoxes ) )
                        aHidden[ nTmpVal ] = false;
                }
                nTmpVal = nLowerBorder;
            }
        }

        pFrm = pFrm->GetNextLayoutLeaf();
    }

    // transfer calculated values from BoundaryMap and HiddenMap into rFill:
    USHORT nIdx = 0;
    for ( aIter = aBoundaries.begin(); aIter != aBoundaries.end(); ++aIter )
    {
        const long nTabTop = (pTab->*fnRect->fnGetPrtTop)();
        const long nKey = (*fnRect->fnYDiff)( (*aIter).first, nTabTop );
        const std::pair< long, long > aTmpPair = (*aIter).second;
        const long nFirst = (*fnRect->fnYDiff)( aTmpPair.first, nTabTop );
        const long nSecond = aTmpPair.second;

        aHiddenIter = aHidden.find( (*aIter).first );
        const bool bHidden = aHiddenIter != aHidden.end() && (*aHiddenIter).second;
        rFill.Insert( nKey, nFirst, nSecond, bHidden, nIdx++ );
    }

    // delete first and last entry
    ASSERT( rFill.Count(), "Deleting from empty vector. Fasten your seatbelts!" )
    // --> FME 2006-01-19 #i60818# There may be only one entry in rFill. Make
    // code robust by checking count of rFill.
    if ( rFill.Count() ) rFill.Remove( 0, 1 );
    if ( rFill.Count() ) rFill.Remove( rFill.Count() - 1 , 1 );
    // <--
    rFill.SetLastRowAllowedToChange( !pTab->HasFollowFlowLine() );
}

void SwDoc::SetTabCols( const SwTabCols &rNew, BOOL bCurRowOnly,
                        const SwCursor* pCrsr, const SwCellFrm* pBoxFrm )
{
    const SwTableBox* pBox = 0;
    SwTabFrm *pTab = 0;

    if( pBoxFrm )
    {
        pTab = ((SwFrm*)pBoxFrm)->ImplFindTabFrm();
        pBox = pBoxFrm->GetTabBox();
    }
    else if( pCrsr )
    {
        const SwCntntNode* pCNd = pCrsr->GetCntntNode();
        if( !pCNd )
            return ;

        Point aPt;
        const SwShellCrsr *pShCrsr = dynamic_cast<const SwShellCrsr*>(pCrsr);
        if( pShCrsr )
            aPt = pShCrsr->GetPtPos();

        const SwFrm* pTmpFrm = pCNd->GetFrm( &aPt, 0, FALSE );
        do {
            pTmpFrm = pTmpFrm->GetUpper();
        } while ( !pTmpFrm->IsCellFrm() );

        pBoxFrm = (SwCellFrm*)pTmpFrm;
        pTab = ((SwFrm*)pBoxFrm)->ImplFindTabFrm();
        pBox = pBoxFrm->GetTabBox();
    }
    else if( !pCrsr && !pBoxFrm )
    {
        ASSERT( !this, "einer von beiden muss angegeben werden!" );
        return ;
    }

    // sollte die Tabelle noch auf relativen Werten (USHRT_MAX) stehen
    // dann muss es jetzt auf absolute umgerechnet werden.
    SwTable& rTab = *pTab->GetTable();
    const SwFmtFrmSize& rTblFrmSz = rTab.GetFrmFmt()->GetFrmSize();
    SWRECTFN( pTab )
    // OD 06.08.2003 #i17174# - With fix for #i9040# the shadow size is taken
    // from the table width. Thus, add its left and right size to current table
    // printing area width in order to get the correct table size attribute.
    SwTwips nPrtWidth = (pTab->Prt().*fnRect->fnGetWidth)();
    {
        SvxShadowItem aShadow( rTab.GetFrmFmt()->GetShadow() );
        nPrtWidth += aShadow.CalcShadowSpace( SHADOW_LEFT ) +
                     aShadow.CalcShadowSpace( SHADOW_RIGHT );
    }
    if( nPrtWidth != rTblFrmSz.GetWidth() )
    {
        SwFmtFrmSize aSz( rTblFrmSz );
        aSz.SetWidth( nPrtWidth );
        rTab.GetFrmFmt()->SetFmtAttr( aSz );
    }

    SwTabCols aOld( rNew.Count() );

    const SwPageFrm* pPage = pTab->FindPageFrm();
    const ULONG nLeftMin = (pTab->Frm().*fnRect->fnGetLeft)() -
                           (pPage->Frm().*fnRect->fnGetLeft)();
    const ULONG nRightMax = (pTab->Frm().*fnRect->fnGetRight)() -
                            (pPage->Frm().*fnRect->fnGetLeft)();

    //Fix-Punkte setzen, LeftMin in Dokumentkoordinaten die anderen relativ.
    aOld.SetLeftMin ( nLeftMin );
    aOld.SetLeft    ( (pTab->Prt().*fnRect->fnGetLeft)() );
    aOld.SetRight   ( (pTab->Prt().*fnRect->fnGetRight)());
    aOld.SetRightMax( nRightMax - nLeftMin );

    rTab.GetTabCols( aOld, pBox );
    SetTabCols(rTab, rNew, aOld, pBox, bCurRowOnly );
}

void SwDoc::SetTabRows( const SwTabCols &rNew, BOOL bCurColOnly, const SwCursor*,
                        const SwCellFrm* pBoxFrm )
{
    const SwTableBox* pBox;
    SwTabFrm *pTab;

    ASSERT( pBoxFrm, "SetTabRows called without pBoxFrm" )

    pTab = ((SwFrm*)pBoxFrm)->ImplFindTabFrm();
    pBox = pBoxFrm->GetTabBox();

    // sollte die Tabelle noch auf relativen Werten (USHRT_MAX) stehen
    // dann muss es jetzt auf absolute umgerechnet werden.
    SWRECTFN( pTab )
    SwTabCols aOld( rNew.Count() );

    //Fix-Punkte setzen, LeftMin in Dokumentkoordinaten die anderen relativ.
    const SwPageFrm* pPage = pTab->FindPageFrm();

    aOld.SetRight( (pTab->Prt().*fnRect->fnGetHeight)() );
    long nLeftMin;
    if ( bVert )
    {
        nLeftMin = pTab->GetPrtLeft() - pPage->Frm().Left();
        aOld.SetLeft    ( LONG_MAX );
        aOld.SetRightMax( aOld.GetRight() );

    }
    else
    {
        nLeftMin = pTab->GetPrtTop() - pPage->Frm().Top();
        aOld.SetLeft    ( 0 );
        aOld.SetRightMax( LONG_MAX );
    }
    aOld.SetLeftMin ( nLeftMin );

    GetTabRows( aOld, 0, pBoxFrm );

       StartUndo( UNDO_TABLE_ATTR, NULL );

    // check for differences between aOld and rNew:
    const USHORT nCount = rNew.Count();
    const SwTable* pTable = pTab->GetTable();
    ASSERT( pTable, "My colleague told me, this couldn't happen" );

    for ( USHORT i = 0; i <= nCount; ++i )
    {
        const USHORT nIdxStt = bVert ? nCount - i : i - 1;
        const USHORT nIdxEnd = bVert ? nCount - i - 1 : i;

        const long nOldRowStart = i == 0  ? 0 : aOld[ nIdxStt ];
        const long nOldRowEnd =   i == nCount ? aOld.GetRight() : aOld[ nIdxEnd ];
        const long nOldRowHeight = nOldRowEnd - nOldRowStart;

        const long nNewRowStart = i == 0  ? 0 : rNew[ nIdxStt ];
        const long nNewRowEnd =   i == nCount ? rNew.GetRight() : rNew[ nIdxEnd ];
        const long nNewRowHeight = nNewRowEnd - nNewRowStart;

        const long nDiff = nNewRowHeight - nOldRowHeight;
        if ( abs( nDiff ) >= ROWFUZZY )
        {
            // For the old table model pTxtFrm and pLine will be set for every box.
            // For the new table model pTxtFrm will be set if the box is not covered,
            // but the pLine will be set if the box is not an overlapping box
            // In the new table model the row height can be adjusted,
            // when both variables are set.
            SwTxtFrm* pTxtFrm = 0;
            const SwTableLine* pLine = 0;

            // Iterate over all SwCellFrms with Bottom = nOldPos
            const SwFrm* pFrm = pTab->GetNextLayoutLeaf();
            while ( pFrm && pTab->IsAnLower( pFrm ) )
            {
                if ( pFrm->IsCellFrm() && pFrm->FindTabFrm() == pTab )
                {
                    const long nLowerBorder = (pFrm->Frm().*fnRect->fnGetBottom)();
                    const ULONG nTabTop = (pTab->*fnRect->fnGetPrtTop)();
                    if ( abs( (*fnRect->fnYInc)( nTabTop, nOldRowEnd ) - nLowerBorder ) <= ROWFUZZY )
                    {
                        if ( !bCurColOnly || pFrm == pBoxFrm )
                        {
                            const SwFrm* pCntnt = ::GetCellCntnt( static_cast<const SwCellFrm&>(*pFrm) );

                            if ( pCntnt && pCntnt->IsTxtFrm() )
                            {
                                pBox = ((SwCellFrm*)pFrm)->GetTabBox();
                                const long nRowSpan = pBox->getRowSpan();
                                if( nRowSpan > 0 ) // Not overlapped
                                    pTxtFrm = (SwTxtFrm*)pCntnt;
                                if( nRowSpan < 2 ) // Not overlapping for row height
                                    pLine = pBox->GetUpper();
                                if( pLine && pTxtFrm ) // always for old table model
                                {
                                    // The new row height must not to be calculated from a overlapping box
                                    SwFmtFrmSize aNew( pLine->GetFrmFmt()->GetFrmSize() );
                                    const long nNewSize = (pFrm->Frm().*fnRect->fnGetHeight)() + nDiff;
                                    if( nNewSize != aNew.GetHeight() )
                                    {
                                        aNew.SetHeight( nNewSize );
                                        if ( ATT_VAR_SIZE == aNew.GetHeightSizeType() )
                                            aNew.SetHeightSizeType( ATT_MIN_SIZE );
                                        // This position must not be in an overlapped box
                                        const SwPosition aPos( *((SwTxtFrm*)pCntnt)->GetTxtNode() );
                                        const SwCursor aTmpCrsr( aPos, 0, false );
                                        SetRowHeight( aTmpCrsr, aNew );
                                        // For the new table model we're done, for the old one
                                        // there might be another (sub)row to adjust...
                                        if( pTable->IsNewModel() )
                                            break;
                                    }
                                    pLine = 0;
                                }
                            }
                        }
                    }
                }
                pFrm = pFrm->GetNextLayoutLeaf();
            }
        }
    }

    EndUndo( UNDO_TABLE_ATTR, NULL );

    ::ClearFEShellTabCols();
}

/* -----------------18.07.98 11:45-------------------
 *  Direktzugriff fuer UNO
 * --------------------------------------------------*/
void SwDoc::SetTabCols(SwTable& rTab, const SwTabCols &rNew, const SwTabCols &rOld,
                                const SwTableBox *pStart, BOOL bCurRowOnly )
{
    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoAttrTbl( *rTab.GetTableNode(), TRUE ));
    }
    rTab.SetTabCols( rNew, rOld, pStart, bCurRowOnly );
      ::ClearFEShellTabCols();
    SetModified();
}

void SwDoc::SetRowsToRepeat( SwTable &rTable, USHORT nSet )
{
    if( nSet == rTable.GetRowsToRepeat() )
        return;

    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoTblHeadline( rTable, rTable.GetRowsToRepeat() , nSet) );
    }

    SwMsgPoolItem aChg( RES_TBLHEADLINECHG );
    rTable.SetRowsToRepeat( nSet );
    rTable.GetFrmFmt()->Modify( &aChg, &aChg );
    SetModified();
}




// Splittet eine Tabelle in der Grund-Zeile, in der der Index steht.
// Alle GrundZeilen dahinter wandern in eine neue Tabelle/-Node.
// Ist das Flag bCalcNewSize auf TRUE, wird fuer beide neuen Tabellen
// die neue Size aus dem Max der Boxen errechnet; vorrausgesetzt,
// die Size ist "absolut" gesetzt (USHRT_MAX)

void SwCollectTblLineBoxes::AddToUndoHistory( const SwCntntNode& rNd )
{
    if( pHst )
        pHst->Add( rNd.GetFmtColl(), rNd.GetIndex(), ND_TEXTNODE );
}

void SwCollectTblLineBoxes::AddBox( const SwTableBox& rBox )
{
    aPosArr.Insert( nWidth, aPosArr.Count() );
    SwTableBox* p = (SwTableBox*)&rBox;
    aBoxes.Insert( p, aBoxes.Count() );
    nWidth = nWidth + (USHORT)rBox.GetFrmFmt()->GetFrmSize().GetWidth();
}

const SwTableBox* SwCollectTblLineBoxes::GetBoxOfPos( const SwTableBox& rBox )
{
    const SwTableBox* pRet = 0;
    USHORT n;

    if( aPosArr.Count() )
    {
        for( n = 0; n < aPosArr.Count(); ++n )
            if( aPosArr[ n ] == nWidth )
                break;
            else if( aPosArr[ n ] > nWidth )
            {
                if( n )
                    --n;
                break;
            }

        if( n >= aPosArr.Count() )
            --n;

        nWidth = nWidth + (USHORT)rBox.GetFrmFmt()->GetFrmSize().GetWidth();
        pRet = aBoxes[ n ];
    }
    return pRet;
}

BOOL SwCollectTblLineBoxes::Resize( USHORT nOffset, USHORT nOldWidth )
{
    USHORT n;

    if( aPosArr.Count() )
    {
        for( n = 0; n < aPosArr.Count(); ++n )
            if( aPosArr[ n ] == nOffset )
                break;
            else if( aPosArr[ n ] > nOffset )
            {
                if( n )
                    --n;
                break;
            }

        aPosArr.Remove( 0, n );
        aBoxes.Remove( 0, n );

        // dann die Positionen der neuen Size anpassen
        for( n = 0; n < aPosArr.Count(); ++n )
        {
            ULONG nSize = nWidth;
            nSize *= ( aPosArr[ n ] - nOffset );
            nSize /= nOldWidth;
            aPosArr[ n ] = USHORT( nSize );
        }
    }
    return 0 != aPosArr.Count();
}

BOOL lcl_Line_CollectBox( const SwTableLine*& rpLine, void* pPara )
{
    SwCollectTblLineBoxes* pSplPara = (SwCollectTblLineBoxes*)pPara;
    if( pSplPara->IsGetValues() )
        ((SwTableLine*)rpLine)->GetTabBoxes().ForEach( &lcl_Box_CollectBox, pPara );
    else
        ((SwTableLine*)rpLine)->GetTabBoxes().ForEach( &lcl_BoxSetSplitBoxFmts, pPara );
    return TRUE;
}

BOOL lcl_Box_CollectBox( const SwTableBox*& rpBox, void* pPara )
{
    SwCollectTblLineBoxes* pSplPara = (SwCollectTblLineBoxes*)pPara;
    USHORT nLen = rpBox->GetTabLines().Count();
    if( nLen )
    {
        // dann mit der richtigen Line weitermachen
        if( pSplPara->IsGetFromTop() )
            nLen = 0;
        else
            --nLen;

        const SwTableLine* pLn = rpBox->GetTabLines()[ nLen ];
        lcl_Line_CollectBox( pLn, pPara );
    }
    else
        pSplPara->AddBox( *rpBox );
    return TRUE;
}

BOOL lcl_BoxSetSplitBoxFmts( const SwTableBox*& rpBox, void* pPara )
{
    SwCollectTblLineBoxes* pSplPara = (SwCollectTblLineBoxes*)pPara;
    USHORT nLen = rpBox->GetTabLines().Count();
    if( nLen )
    {
        // dann mit der richtigen Line weitermachen
        if( pSplPara->IsGetFromTop() )
            nLen = 0;
        else
            --nLen;

        const SwTableLine* pLn = rpBox->GetTabLines()[ nLen ];
        lcl_Line_CollectBox( pLn, pPara );
    }
    else
    {
        const SwTableBox* pSrcBox = pSplPara->GetBoxOfPos( *rpBox );
        SwFrmFmt* pFmt = pSrcBox->GetFrmFmt();
        SwTableBox* pBox = (SwTableBox*)rpBox;

        if( HEADLINE_BORDERCOPY == pSplPara->GetMode() )
        {
            const SvxBoxItem& rBoxItem = pBox->GetFrmFmt()->GetBox();
            if( !rBoxItem.GetTop() )
            {
                SvxBoxItem aNew( rBoxItem );
                aNew.SetLine( pFmt->GetBox().GetBottom(), BOX_LINE_TOP );
                if( aNew != rBoxItem )
                    pBox->ClaimFrmFmt()->SetFmtAttr( aNew );
            }
        }
        else
        {
USHORT __FAR_DATA aTableSplitBoxSetRange[] = {
    RES_LR_SPACE, 		RES_UL_SPACE,
    RES_BACKGROUND, 	RES_SHADOW,
    RES_PROTECT, 		RES_PROTECT,
    RES_VERT_ORIENT,	RES_VERT_ORIENT,
    0 };
            SfxItemSet aTmpSet( pFmt->GetDoc()->GetAttrPool(),
                                aTableSplitBoxSetRange );
            aTmpSet.Put( pFmt->GetAttrSet() );
            if( aTmpSet.Count() )
                pBox->ClaimFrmFmt()->SetFmtAttr( aTmpSet );

            if( HEADLINE_BOXATRCOLLCOPY == pSplPara->GetMode() )
            {
                SwNodeIndex aIdx( *pSrcBox->GetSttNd(), 1 );
                SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
                if( !pCNd )
                    pCNd = aIdx.GetNodes().GoNext( &aIdx );
                aIdx = *pBox->GetSttNd();
                SwCntntNode* pDNd = aIdx.GetNodes().GoNext( &aIdx );

                // nur wenn der Node alleine in der Section steht
                if( 2 == pDNd->EndOfSectionIndex() -
                        pDNd->StartOfSectionIndex() )
                {
                    pSplPara->AddToUndoHistory( *pDNd );
                    pDNd->ChgFmtColl( pCNd->GetFmtColl() );
                }
            }

            // bedingte Vorlage beachten
            pBox->GetSttNd()->CheckSectionCondColl();
        }
    }
    return TRUE;
}


BOOL SwDoc::SplitTable( const SwPosition& rPos, USHORT eHdlnMode,
                        BOOL bCalcNewSize )
{
    SwNode* pNd = &rPos.nNode.GetNode();
    SwTableNode* pTNd = pNd->FindTableNode();
    if( !pTNd || pNd->IsTableNode() )
        return 0;

    if( pTNd->GetTable().ISA( SwDDETable ))
        return FALSE;

    SwTable& rTbl = pTNd->GetTable();
    rTbl.SetHTMLTableLayout( 0 ); 	// MIB 9.7.97: HTML-Layout loeschen

    SwTableFmlUpdate aMsgHnt( &rTbl );

    SwHistory aHistory;
    if( DoesUndo() )
        aMsgHnt.pHistory = &aHistory;

    {
        ULONG nSttIdx = pNd->FindTableBoxStartNode()->GetIndex();

        // Suche die Grund-Line dieser Box:
        SwTableBox* pBox = rTbl.GetTblBox( nSttIdx );
        if( pBox )
        {
            SwTableLine* pLine = pBox->GetUpper();
            while( pLine->GetUpper() )
                pLine = pLine->GetUpper()->GetUpper();

            // in pLine steht jetzt die GrundLine.
            aMsgHnt.nSplitLine = rTbl.GetTabLines().C40_GETPOS( SwTableLine, pLine );
        }

        String sNewTblNm( GetUniqueTblName() );
        aMsgHnt.DATA.pNewTblNm = &sNewTblNm;
        aMsgHnt.eFlags = TBL_SPLITTBL;
        UpdateTblFlds( &aMsgHnt );
    }

    //Lines fuer das Layout-Update heraussuchen.
    _FndBox aFndBox( 0, 0 );
    aFndBox.SetTableLines( rTbl );
    aFndBox.DelFrms( rTbl );

    // TL_CHART2: need to inform chart of probably changed cell names
    //pDoc->UpdateCharts( rTbl.GetFrmFmt()->GetName() );

    SwTableNode* pNew = GetNodes().SplitTable( rPos.nNode, FALSE, bCalcNewSize );

    if( pNew )
    {
        SwSaveRowSpan* pSaveRowSp = pNew->GetTable().CleanUpTopRowSpan( rTbl.GetTabLines().Count() );
        SwUndoSplitTbl* pUndo = 0;
        if( DoesUndo() )
        {
            ClearRedo();
            AppendUndo( pUndo = new SwUndoSplitTbl( *pNew, pSaveRowSp, eHdlnMode, bCalcNewSize ));
            if( aHistory.Count() )
                pUndo->SaveFormula( aHistory );
        }

        switch( eHdlnMode )
        {
            // setze die untere Border der vorherige Line,
            // an der aktuellen als obere
        case HEADLINE_BORDERCOPY:
            {
                SwCollectTblLineBoxes aPara( FALSE, eHdlnMode );
                SwTableLine* pLn = rTbl.GetTabLines()[
                            rTbl.GetTabLines().Count() - 1 ];
                pLn->GetTabBoxes().ForEach( &lcl_Box_CollectBox, &aPara );

                aPara.SetValues( TRUE );
                pLn = pNew->GetTable().GetTabLines()[ 0 ];
                pLn->GetTabBoxes().ForEach( &lcl_BoxSetSplitBoxFmts, &aPara );

                // Kopfzeile wiederholen abschalten
                pNew->GetTable().SetRowsToRepeat( 0 );
            }
            break;

            // setze die Attributierung der ersten Line an der neuen ersten
        case HEADLINE_BOXATTRCOPY:
        case HEADLINE_BOXATRCOLLCOPY:
            {
                SwHistory* pHst = 0;
                if( HEADLINE_BOXATRCOLLCOPY == eHdlnMode && pUndo )
                    pHst = pUndo->GetHistory();

                SwCollectTblLineBoxes aPara( TRUE, eHdlnMode, pHst );
                SwTableLine* pLn = rTbl.GetTabLines()[ 0 ];
                pLn->GetTabBoxes().ForEach( &lcl_Box_CollectBox, &aPara );

                aPara.SetValues( TRUE );
                pLn = pNew->GetTable().GetTabLines()[ 0 ];
                pLn->GetTabBoxes().ForEach( &lcl_BoxSetSplitBoxFmts, &aPara );
            }
            break;

        case HEADLINE_CNTNTCOPY:
            rTbl.CopyHeadlineIntoTable( *pNew );
            if( pUndo )
                pUndo->SetTblNodeOffset( pNew->GetIndex() );
            break;

        case HEADLINE_NONE:
            // Kopfzeile wiederholen abschalten
            pNew->GetTable().SetRowsToRepeat( 0 );
            break;
        }

        // und Frms einfuegen.
        SwNodeIndex aNdIdx( *pNew->EndOfSectionNode() );
        GetNodes().GoNext( &aNdIdx );      // zum naechsten ContentNode
        pNew->MakeFrms( &aNdIdx );

        //Zwischen die Tabellen wird ein Absatz geschoben
        GetNodes().MakeTxtNode( SwNodeIndex( *pNew ),
                                GetTxtCollFromPool( RES_POOLCOLL_TEXT ) );
    }

    //Layout updaten
    aFndBox.MakeFrms( rTbl );

    // TL_CHART2: need to inform chart of probably changed cell names
    UpdateCharts( rTbl.GetFrmFmt()->GetName() );

    SetFieldsDirty( true, NULL, 0 );

    return 0 != pNew;
}

BOOL lcl_ChgTblSize( SwTable& rTbl )
{
    // das Attribut darf nicht ueber das Modify an der
    // Tabelle gesetzt werden, denn sonst werden alle
    // Boxen wieder auf 0 zurueck gesetzt. Also locke das Format
    SwFrmFmt* pFmt = rTbl.GetFrmFmt();
    SwFmtFrmSize aTblMaxSz( pFmt->GetFrmSize() );

    if( USHRT_MAX == aTblMaxSz.GetWidth() )
        return FALSE;

    BOOL bLocked = pFmt->IsModifyLocked();
    pFmt->LockModify();

    aTblMaxSz.SetWidth( 0 );

    SwTableLines& rLns = rTbl.GetTabLines();
    for( USHORT nLns = 0; nLns < rLns.Count(); ++nLns )
    {
        SwTwips nMaxLnWidth = 0;
        SwTableBoxes& rBoxes = rLns[ nLns ]->GetTabBoxes();
        for( USHORT nBox = 0; nBox < rBoxes.Count(); ++nBox )
            nMaxLnWidth += rBoxes[nBox]->GetFrmFmt()->GetFrmSize().GetWidth();

        if( nMaxLnWidth > aTblMaxSz.GetWidth() )
            aTblMaxSz.SetWidth( nMaxLnWidth );
    }
    pFmt->SetFmtAttr( aTblMaxSz );
    if( !bLocked )			// und gegebenenfalls Lock wieder freigeben
        pFmt->UnlockModify();

    return TRUE;
}

class _SplitTable_Para
{
    SvPtrarr aSrc, aDest;
    SwTableNode* pNewTblNd;
    SwTable& rOldTbl;

public:
    _SplitTable_Para( SwTableNode* pNew, SwTable& rOld )
        : aSrc( 16, 16 ), aDest( 16, 16 ), pNewTblNd( pNew ), rOldTbl( rOld )
    {}
    USHORT SrcFmt_GetPos( void* pFmt ) const
            { return aSrc.GetPos( pFmt ); }

    void DestFmt_Insert( void* pFmt )
            { aDest.Insert( pFmt, aDest.Count() ); }

    void SrcFmt_Insert( void* pFmt )
            { aSrc.Insert( pFmt, aSrc.Count() ); }

    SwFrmFmt* DestFmt_Get( USHORT nPos ) const
            { return (SwFrmFmt*)aDest[ nPos ]; }

    void ChgBox( SwTableBox* pBox )
    {
        rOldTbl.GetTabSortBoxes().Remove( pBox );
        pNewTblNd->GetTable().GetTabSortBoxes().Insert( pBox );
    }
};


BOOL lcl_SplitTable_CpyBox( const SwTableBox*& rpBox, void* pPara );

BOOL lcl_SplitTable_CpyLine( const SwTableLine*& rpLine, void* pPara )
{
    SwTableLine* pLn = (SwTableLine*)rpLine;
    _SplitTable_Para& rPara = *(_SplitTable_Para*)pPara;

    SwFrmFmt *pSrcFmt = pLn->GetFrmFmt();
    USHORT nPos = rPara.SrcFmt_GetPos( pSrcFmt );
    if( USHRT_MAX == nPos )
    {
        rPara.DestFmt_Insert( pLn->ClaimFrmFmt() );
        rPara.SrcFmt_Insert( pSrcFmt );
    }
    else
        pLn->ChgFrmFmt( (SwTableLineFmt*)rPara.DestFmt_Get( nPos ) );

    pLn->GetTabBoxes().ForEach( &lcl_SplitTable_CpyBox, pPara );
    return TRUE;
}

BOOL lcl_SplitTable_CpyBox( const SwTableBox*& rpBox, void* pPara )
{
    SwTableBox* pBox = (SwTableBox*)rpBox;
    _SplitTable_Para& rPara = *(_SplitTable_Para*)pPara;

    SwFrmFmt *pSrcFmt = pBox->GetFrmFmt();
    USHORT nPos = rPara.SrcFmt_GetPos( pSrcFmt );
    if( USHRT_MAX == nPos )
    {
        rPara.DestFmt_Insert( pBox->ClaimFrmFmt() );
        rPara.SrcFmt_Insert( pSrcFmt );
    }
    else
        pBox->ChgFrmFmt( (SwTableBoxFmt*)rPara.DestFmt_Get( nPos ) );

    if( pBox->GetSttNd() )
        rPara.ChgBox( pBox );
    else
        pBox->GetTabLines().ForEach( &lcl_SplitTable_CpyLine, pPara );
    return TRUE;
}

SwTableNode* SwNodes::SplitTable( const SwNodeIndex& rPos, BOOL bAfter,
                                    BOOL bCalcNewSize )
{
    SwNode* pNd = &rPos.GetNode();
    SwTableNode* pTNd = pNd->FindTableNode();
    if( !pTNd || pNd->IsTableNode() )
        return 0;

    ULONG nSttIdx = pNd->FindTableBoxStartNode()->GetIndex();

    // Suche die Grund-Line dieser Box:
    SwTable& rTbl = pTNd->GetTable();
    SwTableBox* pBox = rTbl.GetTblBox( nSttIdx );
    if( !pBox )
        return 0;

    SwTableLine* pLine = pBox->GetUpper();
    while( pLine->GetUpper() )
        pLine = pLine->GetUpper()->GetUpper();

    // in pLine steht jetzt die GrundLine.
    USHORT nLinePos = rTbl.GetTabLines().C40_GETPOS( SwTableLine, pLine );
    if( USHRT_MAX == nLinePos ||
        ( bAfter ? ++nLinePos >= rTbl.GetTabLines().Count() : !nLinePos ))
        return 0;		// nicht gefunden oder letze Line !!

    // Suche jetzt die 1. Box der nachfolgenden Line
    SwTableLine* pNextLine = rTbl.GetTabLines()[ nLinePos ];
    pBox = pNextLine->GetTabBoxes()[0];
    while( !pBox->GetSttNd() )
        pBox = pBox->GetTabLines()[0]->GetTabBoxes()[0];

    // dann fuege mal einen End- und TabelleNode ins Nodes-Array ein.
    SwTableNode * pNewTblNd;
    {
        SwEndNode* pOldTblEndNd = (SwEndNode*)pTNd->EndOfSectionNode()->GetEndNode();
        ASSERT( pOldTblEndNd, "wo ist der EndNode?" )

        SwNodeIndex aIdx( *pBox->GetSttNd() );
        new SwEndNode( aIdx, *pTNd );
        pNewTblNd = new SwTableNode( aIdx );
        pNewTblNd->GetTable().SetTableModel( rTbl.IsNewModel() );

        pOldTblEndNd->pStartOfSection = pNewTblNd;
        pNewTblNd->pEndOfSection = pOldTblEndNd;

        SwNode* pBoxNd = aIdx.GetNode().GetStartNode();
        do {
            ASSERT( pBoxNd->IsStartNode(), "das muss ein StartNode sein!" );
            pBoxNd->pStartOfSection = pNewTblNd;
            pBoxNd = (*this)[ pBoxNd->EndOfSectionIndex() + 1 ];
        } while( pBoxNd != pOldTblEndNd );
    }

    {
        // die Lines ruebermoven...
        SwTable& rNewTbl = pNewTblNd->GetTable();
        rNewTbl.GetTabLines().Insert( &rTbl.GetTabLines(), 0, nLinePos );
        //
        // von hinten (unten-rechts) nach vorn (oben-links) alle Boxen
        // beim chart data provider austragen (das modified event wird dann
        // in der aufrufenden Funktion getriggert.
        // TL_CHART2:
        SwChartDataProvider *pPCD = rTbl.GetFrmFmt()->getIDocumentChartDataProviderAccess()->GetChartDataProvider();
        if( pPCD )
        {
            for (USHORT k = nLinePos;  k < rTbl.GetTabLines().Count();  ++k)
            {
                USHORT nLineIdx = (rTbl.GetTabLines().Count() - 1) - k + nLinePos;
                USHORT nBoxCnt = rTbl.GetTabLines()[ nLineIdx ]->GetTabBoxes().Count();
                for (USHORT j = 0;  j < nBoxCnt;  ++j)
                {
                    USHORT nIdx = nBoxCnt - 1 - j;
                    pPCD->DeleteBox( &rTbl, *rTbl.GetTabLines()[ nLineIdx ]->GetTabBoxes()[nIdx] );
                }
            }
        }
        //
        // ...und loeschen
        USHORT nDeleted = rTbl.GetTabLines().Count() - nLinePos;
        rTbl.GetTabLines().Remove( nLinePos, nDeleted );

        // und die betr. Boxen verschieben. Dabei die Formate eindeutig
        // machen und die StartNodes korrigieren
        _SplitTable_Para aPara( pNewTblNd, rTbl );
        rNewTbl.GetTabLines().ForEach( &lcl_SplitTable_CpyLine, &aPara );
        rTbl.CleanUpBottomRowSpan( nDeleted );
    }

    {
        // Das Tabellen-FrmFormat kopieren
        SwFrmFmt* pOldTblFmt = rTbl.GetFrmFmt();
        SwFrmFmt* pNewTblFmt = pOldTblFmt->GetDoc()->MakeTblFrmFmt(
                                pOldTblFmt->GetDoc()->GetUniqueTblName(),
                                pOldTblFmt->GetDoc()->GetDfltFrmFmt() );

        *pNewTblFmt = *pOldTblFmt;
        pNewTblFmt->Add( &pNewTblNd->GetTable() );

        // neue Size errechnen ? (lcl_ChgTblSize nur das 2. aufrufen, wenn es
        // beim 1. schon geklappt hat; also absolute Groesse hat)
        if( bCalcNewSize && lcl_ChgTblSize( rTbl ) )
            lcl_ChgTblSize( pNewTblNd->GetTable() );
    }

    // TL_CHART2: need to inform chart of probably changed cell names
    rTbl.UpdateCharts();

    return pNewTblNd;		// das wars
}

// und die Umkehrung davon. rPos muss in der Tabelle stehen, die bestehen
// bleibt. Das Flag besagt ob die aktuelle mit der davor oder dahinter
// stehenden vereint wird.
BOOL SwDoc::MergeTable( const SwPosition& rPos, BOOL bWithPrev, USHORT nMode )
{
    SwTableNode* pTblNd = rPos.nNode.GetNode().FindTableNode(), *pDelTblNd;
    if( !pTblNd )
        return FALSE;

    SwNodes& rNds = GetNodes();
    if( bWithPrev )
        pDelTblNd = rNds[ pTblNd->GetIndex() - 1 ]->FindTableNode();
    else
        pDelTblNd = rNds[ pTblNd->EndOfSectionIndex() + 1 ]->GetTableNode();
    if( !pDelTblNd )
        return FALSE;

    if( pTblNd->GetTable().ISA( SwDDETable ) ||
        pDelTblNd->GetTable().ISA( SwDDETable ))
        return FALSE;

    // MIB 9.7.97: HTML-Layout loeschen
    pTblNd->GetTable().SetHTMLTableLayout( 0 );
    pDelTblNd->GetTable().SetHTMLTableLayout( 0 );

    // beide Tabellen vorhanden, also kanns losgehen
    SwUndoMergeTbl* pUndo = 0;
    SwHistory* pHistory = 0;
    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( pUndo = new SwUndoMergeTbl( *pTblNd, *pDelTblNd,
                                                bWithPrev, nMode ));
        pHistory = new SwHistory;
    }

    // alle "Tabellenformeln" anpassen
    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.DATA.pDelTbl = &pDelTblNd->GetTable();
    aMsgHnt.eFlags = TBL_MERGETBL;
    aMsgHnt.pHistory = pHistory;
    UpdateTblFlds( &aMsgHnt );

    // das eigentliche Mergen
    SwNodeIndex aIdx( bWithPrev ? *pTblNd : *pDelTblNd );
    BOOL bRet = rNds.MergeTable( aIdx, !bWithPrev, nMode, pHistory );

    if( pHistory )
    {
        if( pHistory->Count() )
            pUndo->SaveFormula( *pHistory );
        delete pHistory;
    }
    if( bRet )
    {
        SetModified();
        SetFieldsDirty( true, NULL, 0 );
    }
    return bRet;
}

BOOL SwNodes::MergeTable( const SwNodeIndex& rPos, BOOL bWithPrev,
                            USHORT nMode, SwHistory* )
{
    SwTableNode* pDelTblNd = rPos.GetNode().GetTableNode();
    ASSERT( pDelTblNd, "wo ist der TableNode geblieben?" );

    SwTableNode* pTblNd = (*this)[ rPos.GetIndex() - 1]->FindTableNode();
    ASSERT( pTblNd, "wo ist der TableNode geblieben?" );

    if( !pDelTblNd || !pTblNd )
        return FALSE;

    pDelTblNd->DelFrms();

    SwTable& rDelTbl = pDelTblNd->GetTable();
    SwTable& rTbl = pTblNd->GetTable();

    //Lines fuer das Layout-Update herausuchen.
    _FndBox aFndBox( 0, 0 );
    aFndBox.SetTableLines( rTbl );
    aFndBox.DelFrms( rTbl );

    // TL_CHART2: since chart currently does not want to get informed about
    // additional rows/cols there is no need for a modified event in the
    // remaining first table. Also, if it is required it  should be done
    // after the merging and not here...
    // pDoc->UpdateCharts( rTbl.GetFrmFmt()->GetName() );


    // TL_CHART2:
    // tell the charts about the table to be deleted and have them use their own data
    GetDoc()->CreateChartInternalDataProviders( &rDelTbl );

    // die Breite der TabellenFormate abgleichen:
    {
        const SwFmtFrmSize& rTblSz = rTbl.GetFrmFmt()->GetFrmSize();
        const SwFmtFrmSize& rDelTblSz = rDelTbl.GetFrmFmt()->GetFrmSize();
        if( rTblSz != rDelTblSz )
        {
            // dann sollten die mal schleunigst korrigiert werden
            if( bWithPrev )
                rDelTbl.GetFrmFmt()->SetFmtAttr( rTblSz );
            else
                rTbl.GetFrmFmt()->SetFmtAttr( rDelTblSz );
        }
    }

    if( !bWithPrev )
    {
        // dann mussen alle Attruibute der hinteren Tabelle auf die
        // vordere uebertragen werden, weil die hintere ueber das loeschen
        // des Node geloescht wird.
        rTbl.SetRowsToRepeat( rDelTbl.GetRowsToRepeat() );
        rTbl.SetTblChgMode( rDelTbl.GetTblChgMode() );

        rTbl.GetFrmFmt()->LockModify();
        *rTbl.GetFrmFmt() = *rDelTbl.GetFrmFmt();
        // auch den Namen umsetzen!
        rTbl.GetFrmFmt()->SetName( rDelTbl.GetFrmFmt()->GetName() );
        rTbl.GetFrmFmt()->UnlockModify();
    }

    // die Lines und Boxen ruebermoven
    USHORT nOldSize = rTbl.GetTabLines().Count();
    rTbl.GetTabLines().Insert( &rDelTbl.GetTabLines(), nOldSize );
    rDelTbl.GetTabLines().Remove( 0, rDelTbl.GetTabLines().Count() );

    rTbl.GetTabSortBoxes().Insert( &rDelTbl.GetTabSortBoxes() );
    rDelTbl.GetTabSortBoxes().Remove( (USHORT)0, rDelTbl.GetTabSortBoxes().Count() );

    // die vordere Tabelle bleibt immer stehen, die hintere wird geloescht
    SwEndNode* pTblEndNd = pDelTblNd->EndOfSectionNode();
    pTblNd->pEndOfSection = pTblEndNd;

    SwNodeIndex aIdx( *pDelTblNd, 1 );

    SwNode* pBoxNd = aIdx.GetNode().GetStartNode();
    do {
        ASSERT( pBoxNd->IsStartNode(), "das muss ein StartNode sein!" );
        pBoxNd->pStartOfSection = pTblNd;
        pBoxNd = (*this)[ pBoxNd->EndOfSectionIndex() + 1 ];
    } while( pBoxNd != pTblEndNd );
    pBoxNd->pStartOfSection = pTblNd;

    aIdx -= 2;
    DelNodes( aIdx, 2 );

    // jetzt an der 1. eingefuegten Line die bedingten Vorlagen umschubsen
    const SwTableLine* pFirstLn = rTbl.GetTabLines()[ nOldSize ];
    if( 1 == nMode )		//
    {
        // Header-Vorlagen in der Zeile setzen
        // und ggfs. in der History speichern fuers Undo!!!
    }
    lcl_LineSetHeadCondColl( pFirstLn, 0 );

    // und die Borders "aufrauemen"
    if( nOldSize )
    {
        _SwGCLineBorder aPara( rTbl );
        aPara.nLinePos = --nOldSize;
        pFirstLn = rTbl.GetTabLines()[ nOldSize ];
        lcl_GC_Line_Border( pFirstLn, &aPara );
    }

    //Layout updaten
    aFndBox.MakeFrms( rTbl );

    return TRUE;
}

// -------------------------------------------------------------------


// -- benutze die ForEach Methode vom PtrArray
struct _SetAFmtTabPara
{
    SwTableAutoFmt& rTblFmt;
    SwUndoTblAutoFmt* pUndo;
    USHORT nEndBox, nCurBox;
    BYTE nAFmtLine, nAFmtBox;

    _SetAFmtTabPara( const SwTableAutoFmt& rNew )
        : rTblFmt( (SwTableAutoFmt&)rNew ), pUndo( 0 ),
        nEndBox( 0 ), nCurBox( 0 ), nAFmtLine( 0 ), nAFmtBox( 0 )
    {}
};

// forward deklarieren damit sich die Lines und Boxen rekursiv aufrufen
// koennen.
BOOL lcl_SetAFmtBox( const _FndBox*&, void *pPara );
BOOL lcl_SetAFmtLine( const _FndLine*&, void *pPara );

BOOL lcl_SetAFmtLine( const _FndLine*& rpLine, void *pPara )
{
    ((_FndLine*&)rpLine)->GetBoxes().ForEach( &lcl_SetAFmtBox, pPara );
    return TRUE;
}

BOOL lcl_SetAFmtBox( const _FndBox*& rpBox, void *pPara )
{
    _SetAFmtTabPara* pSetPara = (_SetAFmtTabPara*)pPara;

    if( !rpBox->GetUpper()->GetUpper() )	// Box auf 1. Ebene ?
    {
        if( !pSetPara->nCurBox )
            pSetPara->nAFmtBox = 0;
        else if( pSetPara->nCurBox == pSetPara->nEndBox )
            pSetPara->nAFmtBox = 3;
        else
            pSetPara->nAFmtBox = (BYTE)(1 + ((pSetPara->nCurBox-1) & 1));
    }

    if( rpBox->GetBox()->GetSttNd() )
    {
        SwTableBox* pSetBox = (SwTableBox*)rpBox->GetBox();
        SwDoc* pDoc = pSetBox->GetFrmFmt()->GetDoc();
        // --> OD 2008-02-25 #refactorlists#
//        SfxItemSet aCharSet( pDoc->GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_END-1 );
        SfxItemSet aCharSet( pDoc->GetAttrPool(), RES_CHRATR_BEGIN, RES_PARATR_LIST_END-1 );
        // <--
        SfxItemSet aBoxSet( pDoc->GetAttrPool(), aTableBoxSetRange );
        BYTE nPos = pSetPara->nAFmtLine * 4 + pSetPara->nAFmtBox;
        pSetPara->rTblFmt.UpdateToSet( nPos, aCharSet,
                                        SwTableAutoFmt::UPDATE_CHAR, 0 );
        pSetPara->rTblFmt.UpdateToSet( nPos, aBoxSet,
                                        SwTableAutoFmt::UPDATE_BOX,
                                        pDoc->GetNumberFormatter( TRUE ) );
        if( aCharSet.Count() )
        {
            ULONG nSttNd = pSetBox->GetSttIdx()+1;
            ULONG nEndNd = pSetBox->GetSttNd()->EndOfSectionIndex();
            for( ; nSttNd < nEndNd; ++nSttNd )
            {
                SwCntntNode* pNd = pDoc->GetNodes()[ nSttNd ]->GetCntntNode();
                if( pNd )
                    pNd->SetAttr( aCharSet );
            }
        }

        if( aBoxSet.Count() )
        {
            if( pSetPara->pUndo &&
                SFX_ITEM_SET == aBoxSet.GetItemState( RES_BOXATR_FORMAT ))
                pSetPara->pUndo->SaveBoxCntnt( *pSetBox );

            pSetBox->ClaimFrmFmt()->SetFmtAttr( aBoxSet );
        }
    }
    else
        ((_FndBox*&)rpBox)->GetLines().ForEach( &lcl_SetAFmtLine, pPara );

    if( !rpBox->GetUpper()->GetUpper() )		// eine BaseLine
        ++pSetPara->nCurBox;
    return TRUE;
}


        // AutoFormat fuer die Tabelle/TabellenSelection
BOOL SwDoc::SetTableAutoFmt( const SwSelBoxes& rBoxes, const SwTableAutoFmt& rNew )
{
    ASSERT( rBoxes.Count(), "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    // suche alle Boxen / Lines
    _FndBox aFndBox( 0, 0 );
    {
        _FndPara aPara( rBoxes, &aFndBox );
        pTblNd->GetTable().GetTabLines().ForEach( &_FndLineCopyCol, &aPara );
    }
    if( !aFndBox.GetLines().Count() )
        return FALSE;

    pTblNd->GetTable().SetHTMLTableLayout( 0 );

    _FndBox* pFndBox = &aFndBox;
    while( 1 == pFndBox->GetLines().Count() &&
            1 == pFndBox->GetLines()[0]->GetBoxes().Count() )
        pFndBox = pFndBox->GetLines()[0]->GetBoxes()[0];

    if( !pFndBox->GetLines().Count() )		// eine zu weit? (nur 1 sel.Box)
        pFndBox = pFndBox->GetUpper()->GetUpper();


    // Undo abschalten, Attribute werden sich vorher gemerkt
    SwUndoTblAutoFmt* pUndo = 0;
    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( pUndo = new SwUndoTblAutoFmt( *pTblNd, rNew ) );
        DoUndo( FALSE );
    }

    _SetAFmtTabPara aPara( rNew );
    _FndLines& rFLns = pFndBox->GetLines();
    _FndLine* pLine;

    for( USHORT n = 0; n < rFLns.Count(); ++n )
    {
        pLine = rFLns[n];

        // Upper auf 0 setzen (Base-Line simulieren!)
        _FndBox* pSaveBox = pLine->GetUpper();
        pLine->SetUpper( 0 );

        if( !n )
            aPara.nAFmtLine = 0;
        else if( n+1 == rFLns.Count() )
            aPara.nAFmtLine = 3;
        else
            aPara.nAFmtLine = (BYTE)(1 + ((n-1) & 1 ));

        aPara.nAFmtBox = 0;
        aPara.nCurBox = 0;
        aPara.nEndBox = pLine->GetBoxes().Count()-1;
        aPara.pUndo = pUndo;
        pLine->GetBoxes().ForEach( &lcl_SetAFmtBox, &aPara );

        pLine->SetUpper( pSaveBox );
    }

    if( pUndo )
        DoUndo( TRUE );

    SetModified();
    SetFieldsDirty( true, NULL, 0 );

    return TRUE;
}


        // Erfrage wie attributiert ist
BOOL SwDoc::GetTableAutoFmt( const SwSelBoxes& rBoxes, SwTableAutoFmt& rGet )
{
    ASSERT( rBoxes.Count(), "keine gueltige Box-Liste" );
    SwTableNode* pTblNd = (SwTableNode*)rBoxes[0]->GetSttNd()->FindTableNode();
    if( !pTblNd )
        return FALSE;

    // suche alle Boxen / Lines
    _FndBox aFndBox( 0, 0 );
    {
        _FndPara aPara( rBoxes, &aFndBox );
        pTblNd->GetTable().GetTabLines().ForEach( &_FndLineCopyCol, &aPara );
    }
    if( !aFndBox.GetLines().Count() )
        return FALSE;

    _FndBox* pFndBox = &aFndBox;
    while( 1 == pFndBox->GetLines().Count() &&
            1 == pFndBox->GetLines()[0]->GetBoxes().Count() )
        pFndBox = pFndBox->GetLines()[0]->GetBoxes()[0];

    if( !pFndBox->GetLines().Count() )		// eine zu weit? (nur 1 sel.Box)
        pFndBox = pFndBox->GetUpper()->GetUpper();

    _FndLines& rFLns = pFndBox->GetLines();

    USHORT aLnArr[4];
    aLnArr[0] = 0;
    aLnArr[1] = 1 < rFLns.Count() ? 1 : 0;
    aLnArr[2] = 2 < rFLns.Count() ? 2 : aLnArr[1];
    aLnArr[3] = rFLns.Count() - 1;

    for( BYTE nLine = 0; nLine < 4; ++nLine )
    {
        _FndLine& rLine = *rFLns[ aLnArr[ nLine ] ];

        USHORT aBoxArr[4];
        aBoxArr[0] = 0;
        aBoxArr[1] = 1 < rLine.GetBoxes().Count() ? 1 : 0;
        aBoxArr[2] = 2 < rLine.GetBoxes().Count() ? 2 : aBoxArr[1];
        aBoxArr[3] = rLine.GetBoxes().Count() - 1;

        for( BYTE nBox = 0; nBox < 4; ++nBox )
        {
            SwTableBox* pFBox = rLine.GetBoxes()[ aBoxArr[ nBox ] ]->GetBox();
            // immer auf die 1. runterfallen
            while( !pFBox->GetSttNd() )
                pFBox = pFBox->GetTabLines()[0]->GetTabBoxes()[0];

            BYTE nPos = nLine * 4 + nBox;
            SwNodeIndex aIdx( *pFBox->GetSttNd(), 1 );
            SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
            if( !pCNd )
                pCNd = GetNodes().GoNext( &aIdx );

            if( pCNd )
                rGet.UpdateFromSet( nPos, pCNd->GetSwAttrSet(),
                                    SwTableAutoFmt::UPDATE_CHAR, 0 );
            rGet.UpdateFromSet( nPos, pFBox->GetFrmFmt()->GetAttrSet(),
                                SwTableAutoFmt::UPDATE_BOX,
                                GetNumberFormatter( TRUE ) );
        }
    }

    return TRUE;
}

String SwDoc::GetUniqueTblName() const
{
    ResId aId( STR_TABLE_DEFNAME, *pSwResMgr );
    String aName( aId );
    xub_StrLen nNmLen = aName.Len();

    USHORT nNum, nTmp, nFlagSize = ( pTblFrmFmtTbl->Count() / 8 ) +2;
    USHORT n;

    BYTE* pSetFlags = new BYTE[ nFlagSize ];
    memset( pSetFlags, 0, nFlagSize );

    for( n = 0; n < pTblFrmFmtTbl->Count(); ++n )
    {
        const SwFrmFmt* pFmt = (*pTblFrmFmtTbl)[ n ];
        if( !pFmt->IsDefault() && IsUsed( *pFmt )  &&
            pFmt->GetName().Match( aName ) == nNmLen )
        {
            // Nummer bestimmen und das Flag setzen
            nNum = static_cast<USHORT>(pFmt->GetName().Copy( nNmLen ).ToInt32());
            if( nNum-- && nNum < pTblFrmFmtTbl->Count() )
                pSetFlags[ nNum / 8 ] |= (0x01 << ( nNum & 0x07 ));
        }
    }

    // alle Nummern entsprechend geflag, also bestimme die richtige Nummer
    nNum = pTblFrmFmtTbl->Count();
    for( n = 0; n < nFlagSize; ++n )
        if( 0xff != ( nTmp = pSetFlags[ n ] ))
        {
            // also die Nummer bestimmen
            nNum = n * 8;
            while( nTmp & 1 )
                ++nNum, nTmp >>= 1;
            break;
        }

    delete [] pSetFlags;
    return aName += String::CreateFromInt32( ++nNum );
}

SwTableFmt* SwDoc::FindTblFmtByName( const String& rName, BOOL bAll ) const
{
    const SwFmt* pRet = 0;
    if( bAll )
        pRet = FindFmtByName( (SvPtrarr&)*pTblFrmFmtTbl, rName );
    else
    {
        // dann nur die, die im Doc gesetzt sind
        for( USHORT n = 0; n < pTblFrmFmtTbl->Count(); ++n )
        {
            const SwFrmFmt* pFmt = (*pTblFrmFmtTbl)[ n ];
            if( !pFmt->IsDefault() && IsUsed( *pFmt ) &&
                pFmt->GetName() == rName )
            {
                pRet = pFmt;
                break;
            }
        }
    }
    return (SwTableFmt*)pRet;
}

BOOL SwDoc::SetColRowWidthHeight( SwTableBox& rAktBox, USHORT eType,
                                    SwTwips nAbsDiff, SwTwips nRelDiff )
{
    SwTableNode* pTblNd = (SwTableNode*)rAktBox.GetSttNd()->FindTableNode();
    SwUndo* pUndo = 0;

    if( nsTblChgWidthHeightType::WH_FLAG_INSDEL & eType && pTblNd->GetTable().ISA( SwDDETable ))
        return FALSE;

    SwTableFmlUpdate aMsgHnt( &pTblNd->GetTable() );
    aMsgHnt.eFlags = TBL_BOXPTR;
    UpdateTblFlds( &aMsgHnt );

    BOOL bRet = FALSE;
    switch( eType & 0xff )
    {
    case nsTblChgWidthHeightType::WH_COL_LEFT:
    case nsTblChgWidthHeightType::WH_COL_RIGHT:
    case nsTblChgWidthHeightType::WH_CELL_LEFT:
    case nsTblChgWidthHeightType::WH_CELL_RIGHT:
        {
             bRet = pTblNd->GetTable().SetColWidth( rAktBox,
                                eType, nAbsDiff, nRelDiff,
                                DoesUndo() ? &pUndo : 0 );
        }
        break;
    case nsTblChgWidthHeightType::WH_ROW_TOP:
    case nsTblChgWidthHeightType::WH_ROW_BOTTOM:
    case nsTblChgWidthHeightType::WH_CELL_TOP:
    case nsTblChgWidthHeightType::WH_CELL_BOTTOM:
        bRet = pTblNd->GetTable().SetRowHeight( rAktBox,
                            eType, nAbsDiff, nRelDiff,
                            DoesUndo() ? &pUndo : 0 );
        break;
    }

    if( pUndo )
    {
        ClearRedo();
        AppendUndo( pUndo );
        DoUndo( TRUE );		// im SetColWidth kann es abgeschaltet werden!
    }

    if( bRet )
    {
        SetModified();
        if( nsTblChgWidthHeightType::WH_FLAG_INSDEL & eType )
            SetFieldsDirty( true, NULL, 0 );
    }
    return bRet;
}


void SwDoc::ChkBoxNumFmt( SwTableBox& rBox, BOOL bCallUpdate )
{
    //JP 09.07.97: Optimierung: wenn die Box schon sagt, das es Text
    //							sein soll, dann bleibt das auch Text!
    const SfxPoolItem* pNumFmtItem = 0;
    if( SFX_ITEM_SET == rBox.GetFrmFmt()->GetItemState( RES_BOXATR_FORMAT,
        FALSE, &pNumFmtItem ) && GetNumberFormatter()->IsTextFormat(
            ((SwTblBoxNumFormat*)pNumFmtItem)->GetValue() ))
        return ;

    SwUndoTblNumFmt* pUndo = 0;

    BOOL bIsEmptyTxtNd, bChgd = TRUE;
    sal_uInt32 nFmtIdx;
    double fNumber;
    if( rBox.HasNumCntnt( fNumber, nFmtIdx, bIsEmptyTxtNd ) )
    {
        if( !rBox.IsNumberChanged() )
            bChgd = FALSE;
        else
        {
            if( DoesUndo() )
            {
                StartUndo( UNDO_TABLE_AUTOFMT, NULL );
                pUndo = new SwUndoTblNumFmt( rBox );
                pUndo->SetNumFmt( nFmtIdx, fNumber );
            }

            SwTableBoxFmt* pBoxFmt = (SwTableBoxFmt*)rBox.GetFrmFmt();
            SfxItemSet aBoxSet( GetAttrPool(), RES_BOXATR_FORMAT, RES_BOXATR_VALUE );

            BOOL bSetNumFmt = IsInsTblFormatNum(), bLockModify = TRUE;
            if( bSetNumFmt )
            {
                if( !IsInsTblChangeNumFormat() )
                {
                    if( !pNumFmtItem )
                        bSetNumFmt = FALSE;
                    else
                    {
                        ULONG nOldNumFmt = ((SwTblBoxNumFormat*)pNumFmtItem)->
                                            GetValue();
                        SvNumberFormatter* pNumFmtr = GetNumberFormatter();

                        short nFmtType = pNumFmtr->GetType( nFmtIdx );
                        if( nFmtType == pNumFmtr->GetType( nOldNumFmt ) ||
                            NUMBERFORMAT_NUMBER == nFmtType )
                            // eingstelltes und vorgegebenes NumFormat
                            // stimmen ueberein -> altes Format beibehalten
                            nFmtIdx = nOldNumFmt;
                        else
                            // eingstelltes und vorgegebenes NumFormat
                            // stimmen nicht ueberein -> als Text einfuegen
                            bLockModify = bSetNumFmt = FALSE;
                    }
                }

                if( bSetNumFmt )
                {
                    pBoxFmt = (SwTableBoxFmt*)rBox.ClaimFrmFmt();

                    aBoxSet.Put( SwTblBoxValue( fNumber ));
                    aBoxSet.Put( SwTblBoxNumFormat( nFmtIdx ));
                }
            }

            // JP 28.04.98: Nur Formel zuruecksetzen reicht nicht.
            //				Sorge dafuer, das der Text auch entsprechend
            //				formatiert wird!

            if( !bSetNumFmt && !bIsEmptyTxtNd && pNumFmtItem )
            {
                // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
                //				Sorge dafuer, das der Text auch entsprechend
                //				formatiert wird!
                pBoxFmt->SetFmtAttr( *GetDfltAttr( RES_BOXATR_FORMAT ));
            }

            if( bLockModify ) pBoxFmt->LockModify();
            pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMAT, RES_BOXATR_VALUE );
            if( bLockModify ) pBoxFmt->UnlockModify();

            if( bSetNumFmt )
                pBoxFmt->SetFmtAttr( aBoxSet );
        }
    }
    else
    {
        // es ist keine Zahl
        const SfxPoolItem* pValueItem = 0, *pFmtItem = 0;
        SwTableBoxFmt* pBoxFmt = (SwTableBoxFmt*)rBox.GetFrmFmt();
        if( SFX_ITEM_SET == pBoxFmt->GetItemState( RES_BOXATR_FORMAT,
                FALSE, &pFmtItem ) ||
            SFX_ITEM_SET == pBoxFmt->GetItemState( RES_BOXATR_VALUE,
                FALSE, &pValueItem ))
        {
            if( DoesUndo() )
            {
                StartUndo( UNDO_TABLE_AUTOFMT, NULL );
                pUndo = new SwUndoTblNumFmt( rBox );
            }

            pBoxFmt = (SwTableBoxFmt*)rBox.ClaimFrmFmt();

            // alle Zahlenformate entfernen
            USHORT nWhich1 = RES_BOXATR_FORMULA;
            if( !bIsEmptyTxtNd )
                //JP 15.01.99: dieser Teil wurde doch schon oben abgeprueft!
                /* && pFmtItem && !GetNumberFormatter()->
                IsTextFormat( ((SwTblBoxNumFormat*)pFmtItem)->GetValue() ) )*/
            {
                nWhich1 = RES_BOXATR_FORMAT;

                // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
                //				Sorge dafuer, das der Text auch entsprechend
                //				formatiert wird!
                pBoxFmt->SetFmtAttr( *GetDfltAttr( nWhich1 ));
            }
            pBoxFmt->ResetFmtAttr( nWhich1, RES_BOXATR_VALUE );
        }
        else
            bChgd = FALSE;
    }

    if( bChgd )
    {
        if( pUndo )
        {
            pUndo->SetBox( rBox );
            AppendUndo( pUndo );
            EndUndo( UNDO_END, NULL );
        }

        const SwTableNode* pTblNd = rBox.GetSttNd()->FindTableNode();
        if( bCallUpdate )
        {
            SwTableFmlUpdate aTblUpdate( &pTblNd->GetTable() );
            UpdateTblFlds( &aTblUpdate );

            // TL_CHART2: update charts (when cursor leaves cell and
            // automatic update is enabled)
            if (AUTOUPD_FIELD_AND_CHARTS == getFieldUpdateFlags(true))
                pTblNd->GetTable().UpdateCharts();
        }
        SetModified();
    }
}

void SwDoc::SetTblBoxFormulaAttrs( SwTableBox& rBox, const SfxItemSet& rSet )
{
    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoTblNumFmt( rBox, &rSet ) );
    }

    SwFrmFmt* pBoxFmt = rBox.ClaimFrmFmt();
    if( SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_FORMULA ))
    {
        pBoxFmt->LockModify();
        pBoxFmt->ResetFmtAttr( RES_BOXATR_VALUE );
        pBoxFmt->UnlockModify();
    }
    else if( SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_VALUE ))
    {
        pBoxFmt->LockModify();
        pBoxFmt->ResetFmtAttr( RES_BOXATR_FORMULA );
        pBoxFmt->UnlockModify();
    }
    pBoxFmt->SetFmtAttr( rSet );
    SetModified();
}

void SwDoc::ClearBoxNumAttrs( const SwNodeIndex& rNode )
{
    SwStartNode* pSttNd;
    if( 0 != ( pSttNd = GetNodes()[ rNode ]->
                                FindSttNodeByType( SwTableBoxStartNode )) &&
        2 == pSttNd->EndOfSectionIndex() - pSttNd->GetIndex() )
    {
        SwTableBox* pBox = pSttNd->FindTableNode()->GetTable().
                            GetTblBox( pSttNd->GetIndex() );

        const SfxPoolItem* pFmtItem = 0;
        const SfxItemSet& rSet = pBox->GetFrmFmt()->GetAttrSet();
        if( SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_FORMAT, FALSE, &pFmtItem ) ||
            SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_FORMULA, FALSE ) ||
            SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_VALUE, FALSE ))
        {
            if( DoesUndo() )
            {
                ClearRedo();
                AppendUndo( new SwUndoTblNumFmt( *pBox ) );
            }

            SwFrmFmt* pBoxFmt = pBox->ClaimFrmFmt();

            //JP 01.09.97: TextFormate bleiben erhalten!
            USHORT nWhich1 = RES_BOXATR_FORMAT;
            if( pFmtItem && GetNumberFormatter()->IsTextFormat(
                    ((SwTblBoxNumFormat*)pFmtItem)->GetValue() ))
                nWhich1 = RES_BOXATR_FORMULA;
            else
                // JP 15.01.99: Nur Attribute zuruecksetzen reicht nicht.
                //				Sorge dafuer, das der Text auch entsprechend
                //				formatiert wird!
                pBoxFmt->SetFmtAttr( *GetDfltAttr( RES_BOXATR_FORMAT ));

            pBoxFmt->ResetFmtAttr( nWhich1, RES_BOXATR_VALUE );
            SetModified();
        }
    }
}

// kopiert eine Tabelle aus dem selben oder einem anderen Doc in sich
// selbst. Dabei wird eine neue Tabelle angelegt oder eine bestehende
// mit dem Inhalt gefuellt; wobei entweder der Inhalt ab einer Box oder
// in eine bestehende TblSelektion gefuellt wird.
// Gerufen wird es von: edglss.cxx/fecopy.cxx

BOOL SwDoc::InsCopyOfTbl( SwPosition& rInsPos, const SwSelBoxes& rBoxes,
                        const SwTable* pCpyTbl, BOOL bCpyName, BOOL bCorrPos )
{
    BOOL bRet;

    const SwTableNode* pSrcTblNd = pCpyTbl
            ? pCpyTbl->GetTableNode()
            : rBoxes[ 0 ]->GetSttNd()->FindTableNode();

    SwTableNode* pInsTblNd = GetNodes()[ rInsPos.nNode ]->FindTableNode();

    if( !pCpyTbl && !pInsTblNd )
    {
        SwUndoCpyTbl* pUndo = 0;
        if( DoesUndo() )
        {
            ClearRedo();
            pUndo = new SwUndoCpyTbl;
            DoUndo( FALSE );
        }

        bRet = pSrcTblNd->GetTable().MakeCopy( this, rInsPos, rBoxes,
                                                TRUE, bCpyName );
        if( pUndo )
        {
            if( !bRet )
                delete pUndo;
            else
            {
                pInsTblNd = GetNodes()[ rInsPos.nNode.GetIndex() - 1 ]->FindTableNode();

                pUndo->SetTableSttIdx( pInsTblNd->GetIndex() );
                AppendUndo( pUndo );
            }
            DoUndo( TRUE );
        }
    }
    else
    {
        RedlineMode_t eOld = GetRedlineMode();
        if( IsRedlineOn() )
      SetRedlineMode( (RedlineMode_t)(nsRedlineMode_t::REDLINE_ON |
                                  nsRedlineMode_t::REDLINE_SHOW_INSERT |
                                  nsRedlineMode_t::REDLINE_SHOW_DELETE));

        SwUndoTblCpyTbl* pUndo = 0;
        if( DoesUndo() )
        {
            ClearRedo();
            pUndo = new SwUndoTblCpyTbl;
            DoUndo( FALSE );
        }

        SwDoc* pCpyDoc = (SwDoc*)pSrcTblNd->GetDoc();
        SfxObjectShellRef* pRefForDocSh = 0;
        BOOL bDelCpyDoc = pCpyDoc == this;

        if( bDelCpyDoc )
        {
            // kopiere die Tabelle erstmal in ein temp. Doc
            pCpyDoc = new SwDoc;
            pCpyDoc->acquire();
            pRefForDocSh = new SfxObjectShellRef();
            pCpyDoc->SetRefForDocShell( pRefForDocSh );

            SwPosition aPos( SwNodeIndex( pCpyDoc->GetNodes().GetEndOfContent() ));
            if( !pSrcTblNd->GetTable().MakeCopy( pCpyDoc, aPos, rBoxes, TRUE, TRUE ))
            {
                delete pRefForDocSh;
                if( pCpyDoc->release() == 0 )
                    delete pCpyDoc;

                if( pUndo )
                {
                    DoUndo( TRUE );
                    delete pUndo;
                }
                return FALSE;
            }
            aPos.nNode -= 1;		// auf den EndNode der Tabelle
            pSrcTblNd = aPos.nNode.GetNode().FindTableNode();

            pCpyDoc->SetRefForDocShell( NULL );
        }

        const SwStartNode* pSttNd = rInsPos.nNode.GetNode().FindTableBoxStartNode();

        rInsPos.nContent.Assign( 0, 0 );

        // no complex into complex, but copy into or from new model is welcome
        if( ( !pSrcTblNd->GetTable().IsTblComplex() || pInsTblNd->GetTable().IsNewModel() )
            && ( bDelCpyDoc || rBoxes.Count() ) )
        {
            // dann die Tabelle "relativ" kopieren
            const SwSelBoxes* pBoxes;
            SwSelBoxes aBoxes;

            if( bDelCpyDoc )
            {
                SwTableBox* pBox = pInsTblNd->GetTable().GetTblBox(
                                        pSttNd->GetIndex() );
                ASSERT( pBox, "Box steht nicht in dieser Tabelle" );
                aBoxes.Insert( pBox );
                pBoxes = &aBoxes;
            }
            else
                pBoxes = &rBoxes;

            // kopiere die Tabelle in die selktierten Zellen.
            bRet = pInsTblNd->GetTable().InsTable( pSrcTblNd->GetTable(),
                                                        *pBoxes, pUndo );
        }
        else
        {
            SwNodeIndex aNdIdx( *pSttNd, 1 );
            bRet = pInsTblNd->GetTable().InsTable( pSrcTblNd->GetTable(),
                                                    aNdIdx, pUndo );
        }

        if( bDelCpyDoc )
        {
            delete pRefForDocSh;
            if( pCpyDoc->release() == 0 )
                delete pCpyDoc;
        }

        if( pUndo )
        {
            // falls die Tabelle nicht kopiert werden konnte, das Undo-Object
            // wieder loeschen
            if( !bRet && pUndo->IsEmpty() )
                delete pUndo;
            else
                AppendUndo( pUndo );
            DoUndo( TRUE );
        }

        if( bCorrPos )
        {
            rInsPos.nNode = *pSttNd;
            rInsPos.nContent.Assign( GetNodes().GoNext( &rInsPos.nNode ), 0 );
        }
        SetRedlineMode( eOld );
    }

    if( bRet )
    {
        SetModified();
        SetFieldsDirty( true, NULL, 0 );
    }
    return bRet;
}



BOOL SwDoc::_UnProtectTblCells( SwTable& rTbl )
{
    BOOL bChgd = FALSE;
    SwUndoAttrTbl* pUndo = DoesUndo() ? new SwUndoAttrTbl( *rTbl.GetTableNode() )
                                      : 0;

    SwTableSortBoxes& rSrtBox = rTbl.GetTabSortBoxes();
    for( USHORT i = rSrtBox.Count(); i; )
    {
        SwFrmFmt *pBoxFmt = rSrtBox[ --i ]->GetFrmFmt();
        if( pBoxFmt->GetProtect().IsCntntProtected() )
        {
            pBoxFmt->ResetFmtAttr( RES_PROTECT );
            bChgd = TRUE;
        }
    }

    if( pUndo )
    {
        if( bChgd )
        {
            ClearRedo();
            AppendUndo( pUndo );
        }
        else
            delete pUndo;
    }
    return bChgd;
}


BOOL SwDoc::UnProtectCells( const String& rName )
{
    BOOL bChgd = FALSE;
    SwTableFmt* pFmt = FindTblFmtByName( rName );
    if( pFmt )
    {
        bChgd = _UnProtectTblCells( *SwTable::FindTable( pFmt ) );
        if( bChgd )
            SetModified();
    }

    return bChgd;
}

BOOL SwDoc::UnProtectCells( const SwSelBoxes& rBoxes )
{
    BOOL bChgd = FALSE;
    if( rBoxes.Count() )
    {
        SwUndoAttrTbl* pUndo = DoesUndo()
                ? new SwUndoAttrTbl( *rBoxes[0]->GetSttNd()->FindTableNode() )
                : 0;

        SvPtrarr aFmts( 16 ), aNewFmts( 16 );
        for( USHORT i = rBoxes.Count(); i; )
        {
            SwTableBox* pBox = rBoxes[ --i ];
            SwFrmFmt* pBoxFmt = pBox->GetFrmFmt();
            if( pBoxFmt->GetProtect().IsCntntProtected() )
            {
                USHORT nFnd = aFmts.GetPos( pBoxFmt );
                if( USHRT_MAX != nFnd )
                    pBox->ChgFrmFmt( (SwTableBoxFmt*)aNewFmts[ nFnd ] );
                else
                {
                    aFmts.Insert( pBoxFmt, aFmts.Count() );
                    pBoxFmt = pBox->ClaimFrmFmt();
                    pBoxFmt->ResetFmtAttr( RES_PROTECT );
                    aNewFmts.Insert( pBoxFmt, aNewFmts.Count() );
                }
                bChgd = TRUE;
            }
        }

        if( pUndo )
        {
            if( bChgd )
            {
                ClearRedo();
                AppendUndo( pUndo );
            }
            else
                delete pUndo;
        }
    }
    return bChgd;
}

BOOL SwDoc::UnProtectTbls( const SwPaM& rPam )
{
    StartUndo(UNDO_EMPTY, NULL);

    BOOL bChgd = FALSE, bHasSel = rPam.HasMark() ||
                                    rPam.GetNext() != (SwPaM*)&rPam;
    SwFrmFmts& rFmts = *GetTblFrmFmts();
    SwTable* pTbl;
    const SwTableNode* pTblNd;
    for( USHORT n = rFmts.Count(); n ; )
        if( 0 != (pTbl = SwTable::FindTable( rFmts[ --n ] )) &&
            0 != (pTblNd = pTbl->GetTableNode() ) &&
            pTblNd->GetNodes().IsDocNodes() )
        {
            ULONG nTblIdx = pTblNd->GetIndex();

            // dann ueberpruefe ob Tabelle in der Selection liegt
            if( bHasSel )
            {
                int bFound = FALSE;
                SwPaM* pTmp = (SwPaM*)&rPam;
                do {
                    const SwPosition *pStt = pTmp->Start(),
                                    *pEnd = pTmp->End();
                    bFound = pStt->nNode.GetIndex() < nTblIdx &&
                            nTblIdx < pEnd->nNode.GetIndex();

                } while( !bFound && &rPam != ( pTmp = (SwPaM*)pTmp->GetNext() ) );
                if( !bFound )
                    continue;		// weitersuchen
            }

            // dann mal den Schutz aufheben
            bChgd |= _UnProtectTblCells( *pTbl );
        }

    EndUndo(UNDO_EMPTY, NULL);
    if( bChgd )
        SetModified();

    return bChgd;
}

BOOL SwDoc::HasTblAnyProtection( const SwPosition* pPos,
                                 const String* pTblName,
                                 BOOL* pFullTblProtection )
{
    BOOL bHasProtection = FALSE;
    SwTable* pTbl = 0;
    if( pTblName )
        pTbl = SwTable::FindTable( FindTblFmtByName( *pTblName ) );
    else if( pPos )
    {
        SwTableNode* pTblNd = pPos->nNode.GetNode().FindTableNode();
        if( pTblNd )
            pTbl = &pTblNd->GetTable();
    }

    if( pTbl )
    {
        SwTableSortBoxes& rSrtBox = pTbl->GetTabSortBoxes();
        for( USHORT i = rSrtBox.Count(); i; )
        {
            SwFrmFmt *pBoxFmt = rSrtBox[ --i ]->GetFrmFmt();
            if( pBoxFmt->GetProtect().IsCntntProtected() )
            {
                if( !bHasProtection )
                {
                    bHasProtection = TRUE;
                    if( !pFullTblProtection )
                        break;
                    *pFullTblProtection = TRUE;
                }
            }
            else if( bHasProtection && pFullTblProtection )
            {
                *pFullTblProtection = FALSE;
                break;
            }
        }
    }
    return bHasProtection;
}

#ifdef DEL_TABLE_REDLINES
lcl_DelRedlines::lcl_DelRedlines( const SwTableNode& rNd,
                                    BOOL bCheckForOwnRedline )
    : pDoc( (SwDoc*)rNd.GetNodes().GetDoc() )
{
    pDoc->StartUndo(UNDO_EMPTY, NULL);
    const SwRedlineTbl& rTbl = pDoc->GetRedlineTbl();
    if( !pDoc->IsIgnoreRedline() && rTbl.Count() )
    {
        BOOL bDelete = TRUE;
        if( bCheckForOwnRedline )
        {
            sal_uInt16 nRedlPos = pDoc->GetRedlinePos( rNd, USHRT_MAX );
            sal_uInt32 nSttNd = rNd.GetIndex(),
                       nEndNd = rNd.EndOfSectionIndex();

            for ( ; nRedlPos < rTbl.Count(); ++nRedlPos )
            {
                const SwRedline* pRedline = rTbl[ nRedlPos ];
                const SwPosition* pStt = pRedline->Start(),
                                  * pEnd = pStt == pRedline->GetPoint()
                                                      ? pRedline->GetMark()
                                                    : pRedline->GetPoint();
                if( pStt->nNode <= nSttNd )
                {
                    if( pEnd->nNode >= nEndNd &&
                        pRedline->GetAuthor() == pDoc->GetRedlineAuthor() )
                    {
                        bDelete = FALSE;
                        break;
                    }
                }
                else
                    break;
            }
        }
        if( bDelete )
        {
            SwPaM aPam(*rNd.EndOfSectionNode(), rNd);
            pDoc->AcceptRedline( aPam, true );
        }
    }
}
#endif


