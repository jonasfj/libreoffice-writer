/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swnewtable.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-28 15:50:29 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#include <swtable.hxx>
#include <tblsel.hxx>
#include <tblrwcl.hxx>
#include <node.hxx>
#include <undobj.hxx>
#include <pam.hxx>
#include <frmfmt.hxx>
#include <frmatr.hxx>
#include <cellfrm.hxx>
#include <fmtfsize.hxx>
#include <doc.hxx>
#include <vector>
#include <set>
#include <list>
#include <memory>
#include <svx/boxitem.hxx>
#include <svx/protitem.hxx>
#include <swtblfmt.hxx>

#ifdef PRODUCT
#define CHECK_TABLE(t)
#else
#ifdef DEBUG
#define CHECK_TABLE(t) (t).CheckConsistency();
#else
#define CHECK_TABLE(t)
#endif
#endif

// ---------------------------------------------------------------

/*
SwBoxSelection is a small helperclass (structure),
it contains an "array" of table boxes, a rectangulare selection of table boxes.
To be more specific, it contains
a vector of box selections, every box selection (SwSelBoxes) contains the
selected boxes in on row.
The member mnMergeWidth contains the width of the selected boxes
*/

class SwBoxSelection
{
public:
    std::vector<const SwSelBoxes*> aBoxes;
    long mnMergeWidth;
    SwBoxSelection::SwBoxSelection() : mnMergeWidth(0) {}
    bool isEmpty() const { return aBoxes.size() == 0; }
    void insertBoxes( const SwSelBoxes* pNew ){ aBoxes.insert( aBoxes.end(), pNew ); }
};

/** NewMerge(..) removes the superfluous cells after cell merge

SwTable::NewMerge(..) does some cleaning up,
it simply deletes the superfluous cells ("cell span")
ands notifies the Undo about it.
The main work has been done by SwTable::PrepareMerge(..) already.

@param rBoxes
the boxes to remove

@param pUndo
the undo object to notify, maybe empty

@return TRUE for compatibility reasons with OldMerge(..)
*/

BOOL SwTable::NewMerge( SwDoc* pDoc, const SwSelBoxes& rBoxes,
     const SwSelBoxes& rMerged, SwTableBox*, SwUndoTblMerge* pUndo )
{
    if( pUndo )
        pUndo->SetSelBoxes( rBoxes );
    DeleteSel( pDoc, rBoxes, &rMerged, 0, TRUE, TRUE );

    CHECK_TABLE( *this )
    return TRUE;
}

/** lcl_ExpandMinMax(..) examines the given table line.

All boxes will be checked if they overlap to the given (horizontal) range (rMin, rMax)
If a box overlaps only partial the range will be expanded.

@param rMin
if a box starts before rMin but ends behind, rMin will be decremented
to the left border of this box

@param rMax
if a box starts before rMax but ends behind, rMax will be incremented
to the right border of this box

@param rLine
the row (table line) to examine

@return true if the range has been expanded, false otherwise

*/

bool lcl_ExpandMinMax( long& rMin, long& rMax, const SwTableLine& rLine )
{
    // nNewMin will become the left border of the first box which overlaps
    // bLookForMin will be set to false, if it has been found
    bool bLookForMin = true;
    long nNewMin = 0;
    // nNewMax will become the right border of the last overlapping box
    long nNewMax = 0;

    USHORT nCount = rLine.GetTabBoxes().Count();
    for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
    {
        SwTableBox* pBox = rLine.GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        long nWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth();
        if( bLookForMin && nNewMin + nWidth <= rMin )
            nNewMin += nWidth; // boxes with right border before rMin
        else
            bLookForMin = false; // boxes with right border behind rMin
        if( nNewMax < rMax ) // left border before rMax
            nNewMax += nWidth;
        else
            break; // right border behind rMax => Done.
    }
    ASSERT( nNewMax >= rMax, "Funny widths" );
    bool bRet = nNewMin < rMin; // Left border has to be expanded
    rMin = nNewMin;
    if( rMax < nNewMax ) // right border has to be expanded
    {
        rMax = nNewMax;
        bRet = true;
    }
    return bRet;
}

/** lcl_CheckMinMax helps evaluating (horizontal) min/max of boxes

lcl_CheckMinMax(..) compares the left border and the right border
of a given cell with the given range and sets it accordingly.

@param rMin
will be decremented if necessary to the left border of the cell

@param rMax
will be incremented if necessary to the right border of the cell

@param rLine
the row (table line) of the interesting box

@param nCheck
the index of the box in the table box array of the given row

@param bSet
if bSet is false, rMin and rMax will be manipulated if necessary
if bSet is true, rMin and rMax will be set to the left and right border of the box

*/

void lcl_CheckMinMax( long& rMin, long& rMax, const SwTableLine& rLine, USHORT nCheck, bool bSet )
{
    ++nCheck;
    if( rLine.GetTabBoxes().Count() < nCheck )
    {   // robust
        ASSERT( false, "Box out of table line" );
        nCheck = rLine.GetTabBoxes().Count();
    }

    long nNew = 0; // will be the right border of the current box
    long nWidth = 0; // the width of the current box
    for( USHORT nCurrBox = 0; nCurrBox < nCheck; ++nCurrBox )
    {
        SwTableBox* pBox = rLine.GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        nWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth();
        nNew += nWidth;
    }
    // nNew is the right border of the wished box
    if( bSet || nNew > rMax )
        rMax = nNew;
    nNew -= nWidth; // nNew becomes the left border of the wished box
    if( bSet || nNew < rMin )
        rMin = nNew;
}

/** lcl_CheckRowSpan(..) looks for row span in a given range

given the range (logical x-values), the boxes in the given line were examined,
if they overlap with the range and if they have a row span < 0

@param nMin
the left border of the interesting area

@param
the right border

@param rLine
the row (table line) to check

@return the most negativ row span in the line which overlaps completely within the range

*/

long lcl_CheckRowSpan( long nMin, long nMax, const SwTableLine& rLine )
{
    long nSpan = 0;
    long nLeft = 0; // the left border of the current box
    long nRight = 0; // the right border of the current box
    USHORT nCount = rLine.GetTabBoxes().Count();
    for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
    {
        nLeft = nRight;
        SwTableBox* pBox = rLine.GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        nRight += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
        if( nLeft >= nMin && nRight <= nMax && pBox->getRowSpan() < 1 &&
            nSpan > pBox->getRowSpan() )
            nSpan = pBox->getRowSpan(); // overlapping box with row span found
        if( nRight > nMax )
            return nSpan; // the next boxes will start behind nMax => done.
    }
    return nSpan;
}

/** lcl_Box2LeftBorder(..) delivers the left (logical) border of a table box

The left logical border of a table box is the sum of the cell width before this
box.

@param rBox
is the requested table box

@return is the left logical border (long, even it cannot be negative)

*/

long lcl_Box2LeftBorder( const SwTableBox& rBox )
{
    if( !rBox.GetUpper() )
        return 0;
    long nLeft = 0;
    const SwTableLine &rLine = *rBox.GetUpper();
    USHORT nCount = rLine.GetTabBoxes().Count();
    for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
    {
        SwTableBox* pBox = rLine.GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        if( pBox == &rBox )
            return nLeft;
        nLeft += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
    }
    ASSERT( false, "Box not found in own upper?" );
    return nLeft;
}

/** lcl_LeftBorder2Box delivers the box to a given left border

It's used to find the master/follow table boxes in previous/next rows.
Don't call this function to check if there is such a box,
call it if you know there has to be such box.

@param nLeft
the left border (logical x-value) of the demanded box

@param rLine
the row (table line) to be scanned

@return a pointer to the table box inside the given row with the wished left border

*/

SwTableBox* lcl_LeftBorder2Box( long nLeft, const SwTableLine* pLine )
{
    if( !pLine )
        return 0;
    long nCurrLeft = 0;
    USHORT nCount = pLine->GetTabBoxes().Count();
    for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
    {
        SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        if( nCurrLeft >= nLeft && pBox->GetFrmFmt()->GetFrmSize().GetWidth() )
        {
            ASSERT( nCurrLeft == nLeft, "Wrong box found" );
            return pBox;
        }
        nCurrLeft += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
    }
    ASSERT( false, "Didn't found wished box" );
    return 0;
}

/** lcl_ChangeRowSpan corrects row span after insertion/deletion of rows

lcl_ChangeRowSpan(..) has to be called after an insertion or deletion of rows
to adjust the row spans of previous rows accordingly.
If rows are deleted, the previous rows with row spans into the deleted area
have to be decremented by the number of _overlapped_ inserted rows.
If rows are inserted, the previous rows with row span into the inserted area
have to be incremented by the number of inserted rows.
For those row spans which ends exactly above the inserted area it has to be
decided by the parameter bSingle if they have to be expanded or not.

@param rTable
the table to manipulate (has to be a new model table)

@param nDiff
the number of rows which has been inserted (nDiff > 0) or deleted (nDiff < 0)

@param nRowIdx
the index of the first row which has to be checked

@param bSingle
true if the new inserted row should not extend row spans which ends in the row above
this is for rows inserted by UI "insert row"
false if all cells of an inserted row has to be overlapped by the previous row
this is for rows inserted by "split row"
false is also needed for deleted rows

*/

void lcl_ChangeRowSpan( const SwTable& rTable, const long nDiff,
                        USHORT nRowIdx, const bool bSingle )
{
    if( !nDiff || nRowIdx >= rTable.GetTabLines().Count() )
        return;
    ASSERT( !bSingle || nDiff > 0, "Don't set bSingle when deleting lines!" );
    bool bGoOn;
    // nDistance is the distance between the current row and the critical row,
    // e.g. the deleted rows or the inserted rows.
    // If the row span is lower than the distance there is nothing to do
    // because the row span ends before the critical area.
    // When the inserted rows should not be overlapped by row spans which ends
    // exactly in the row above, the trick is to start with a distance of 1.
    long nDistance = bSingle ? 1 : 0;
    do
    {
        bGoOn = false; // will be set to true if we found a non-master cell
        // which has to be manipulated => we have to chekc the previous row, too.
        const SwTableLine* pLine = rTable.GetTabLines()[ nRowIdx ];
        USHORT nBoxCount = pLine->GetTabBoxes().Count();
        for( USHORT nCurrBox = 0; nCurrBox < nBoxCount; ++nCurrBox )
        {
            long nRowSpan = pLine->GetTabBoxes()[nCurrBox]->getRowSpan();
            long nAbsSpan = nRowSpan > 0 ? nRowSpan : -nRowSpan;
            // Check if the last overlapped cell is above or below
            // the critical area
            if( nAbsSpan > nDistance )
            {
                if( nDiff > 0 )
                {
                    if( nRowSpan > 0 )
                        nRowSpan += nDiff; // increment row span of master cell
                    else
                    {
                        nRowSpan -= nDiff; // increment row span of non-master cell
                        bGoOn = true;
                    }
                }
                else
                {
                    if( nRowSpan > 0 )
                    {   // A master cell
                         // end of row span behind the deleted area ..
                        if( nRowSpan - nDistance > -nDiff )
                            nRowSpan += nDiff;
                        else // .. or inside the deleted area
                            nRowSpan = nDistance + 1;
                    }
                    else
                    {   // Same for a non-master cell
                        if( nRowSpan + nDistance < nDiff )
                            nRowSpan -= nDiff;
                        else
                            nRowSpan = -nDistance - 1;
                        bGoOn = true; // We have to continue
                    }
                }
                pLine->GetTabBoxes()[ nCurrBox ]->setRowSpan( nRowSpan );
            }
        }
        ++nDistance;
        if( nRowIdx )
            --nRowIdx;
        else
            bGoOn = false; //robust
    } while( bGoOn );
}

SwBoxSelection* SwTable::CollectBoxSelection( const SwPaM& rPam ) const
{
    ASSERT( bNewModel, "Don't call me for old tables" );
    if( !aLines.Count() )
        return 0;
    const SwNode* pStartNd = rPam.Start()->nNode.GetNode().FindTableBoxStartNode();
    const SwNode* pEndNd = rPam.End()->nNode.GetNode().FindTableBoxStartNode();
    if( !pStartNd || !pEndNd || pStartNd == pEndNd )
        return 0;

    USHORT nLines = aLines.Count();
    USHORT nTop = 0, nBottom = 0;
    long nMin = 0, nMax = 0;
    int nFound = 0;
    for( USHORT nRow = 0; nFound < 2 && nRow < nLines; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        ASSERT( pLine, "Missing table line" );
        USHORT nCols = pLine->GetTabBoxes().Count();
        for( USHORT nCol = 0; nCol < nCols; ++nCol )
        {
            SwTableBox* pBox = pLine->GetTabBoxes()[nCol];
            ASSERT( pBox, "Missing table box" );
            if( nFound )
            {
                if( pBox->GetSttNd() == pEndNd )
                {
                    nBottom = nRow;
                    lcl_CheckMinMax( nMin, nMax, *pLine, nCol, false );
                    ++nFound;
                    break;
                }
            }
            else if( pBox->GetSttNd() == pStartNd )
            {
                nTop = nRow;
                lcl_CheckMinMax( nMin, nMax, *pLine, nCol, true );
                ++nFound;
            }
        }
    }
    if( nFound < 2 )
        return 0;

    bool bOkay = true;
    long nMid = ( nMin + nMax ) / 2;

    SwBoxSelection* pRet = new SwBoxSelection();
    std::list< std::pair< SwTableBox*, long > > aNewWidthList;
    USHORT nCheckBottom = nBottom;
    long nLeftSpan = 0;
    long nRightSpan = 0;
    long nLeftSpanCnt = 0;
    long nRightSpanCnt = 0;
    for( USHORT nRow = nTop; nRow <= nBottom && bOkay; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        ASSERT( pLine, "Missing table line" );
        SwSelBoxes *pBoxes = new SwSelBoxes();
        long nLeft = 0;
        long nRight = 0;
        long nRowSpan = 1;
        USHORT nCount = pLine->GetTabBoxes().Count();
        for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
        {
            SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
            ASSERT( pBox, "Missing table box" );
            nLeft = nRight;
            nRight += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
            nRowSpan = pBox->getRowSpan();
            if( nRight <= nMin )
            {
                if( nRight == nMin && nLeftSpanCnt )
                    bOkay = false;
                continue;
            }
            SwTableBox* pInnerBox = 0;
            SwTableBox* pLeftBox = 0;
            SwTableBox* pRightBox = 0;
            long nDiff = 0;
            long nDiff2 = 0;
            if( nLeft < nMin )
            {
                if( nRight >= nMid || nRight + nLeft >= nMin + nMin )
                {
                    if( nCurrBox )
                    {
                        pBoxes->Insert( pBox );
                        pInnerBox = pBox;
                        pLeftBox = pLine->GetTabBoxes()[nCurrBox-1];
                        nDiff = nMin - nLeft;
                        if( nRight > nMax )
                        {
                            if( nCurrBox+1 < nCount )
                            {
                                pRightBox = pLine->GetTabBoxes()[nCurrBox+1];
                                nDiff2 = nRight - nMax;
                            }
                            else
                                bOkay = false;
                        }
                        else if( nRightSpanCnt && nRight == nMax )
                            bOkay = false;
                    }
                    else
                        bOkay = false;
                }
                else if( nCurrBox+1 < nCount )
                {
                    pLeftBox = pBox;
                    pInnerBox = pLine->GetTabBoxes()[nCurrBox+1];
                    nDiff = nMin - nRight;
                }
                else
                    bOkay = false;
            }
            else if( nRight <= nMax ) 
            {
                pBoxes->Insert( pBox );
                if( nRow == nTop && nRowSpan < 0 )
                {
                    bOkay = false;
                    break;
                }
                if( nRowSpan > 1 && nRow + nRowSpan - 1 > nBottom )
                    nBottom = nRow + (USHORT)nRowSpan - 1;
                if( nRowSpan < -1 && nRow - nRowSpan - 1 > nBottom )
                    nBottom = (USHORT)(nRow - nRowSpan - 1);
                if( nRightSpanCnt && nRight == nMax )
                    bOkay = false;
            }
            else if( nLeft < nMax )
            {
                if( nLeft <= nMid || nRight + nLeft <= nMax )
                {
                    if( nCurrBox+1 < nCount )
                    {
                        pBoxes->Insert( pBox );
                        pInnerBox = pBox;
                        pRightBox = pLine->GetTabBoxes()[nCurrBox+1];
                        nDiff = nRight - nMax;
                    }
                    else
                        bOkay = false;
                }
                else if( nCurrBox )
                {
                    pRightBox = pBox;
                    pInnerBox = pLine->GetTabBoxes()[nCurrBox-1];
                    nDiff = nLeft - nMax;
                }
                else
                    bOkay = false;
            }
            else
                break;
            if( pInnerBox )
            {
                if( nRow == nBottom )
                {
                    long nTmpSpan = pInnerBox->getRowSpan();
                    if( nTmpSpan > 1 )
                        nBottom += (USHORT)nTmpSpan - 1;
                    else if( nTmpSpan < -1 )
                        nBottom = (USHORT)( nBottom - nTmpSpan - 1 );
                }
                SwTableBox* pOuterBox = pLeftBox;
                do
                {
                    if( pOuterBox )
                    {
                        long nOutSpan = pOuterBox->getRowSpan();
                        if( nOutSpan != 1 )
                        {
                            USHORT nCheck = nRow;
                            if( nOutSpan < 0 )
                            {
                                const SwTableBox& rBox = 
                                    pOuterBox->FindStartOfRowSpan( *this, USHRT_MAX );
                                nOutSpan = rBox.getRowSpan();
                                const SwTableLine* pTmpL = rBox.GetUpper();
                                nCheck = GetTabLines().C40_GETPOS( SwTableLine, pTmpL );
                                if( nCheck < nTop )
                                    bOkay = false;
                                if( pOuterBox == pLeftBox )
                                {
                                    if( !nLeftSpanCnt || nMin - nDiff != nLeftSpan )
                                        bOkay = false;
                                }
                                else
                                {
                                    if( !nRightSpanCnt || nMax + nDiff != nRightSpan )
                                        bOkay = false;
                                }
                            }
                            else
                            {
                                if( pOuterBox == pLeftBox )
                                {
                                    if( nLeftSpanCnt )
                                        bOkay = false;
                                    nLeftSpan = nMin - nDiff;
                                    nLeftSpanCnt = nOutSpan;
                                }
                                else
                                {
                                    if( nRightSpanCnt )
                                        bOkay = false;
                                    nRightSpan = nMax + nDiff;
                                    nRightSpanCnt = nOutSpan;
                                }
                            }
                            nCheck += (USHORT)nOutSpan - 1;
                            if( nCheck > nCheckBottom )
                                nCheckBottom = nCheck;
                        }
                        else if( ( nLeftSpanCnt && pLeftBox == pOuterBox ) ||
                            ( nRightSpanCnt && pRightBox == pOuterBox ) )
                            bOkay = false;
                        std::pair< SwTableBox*, long > aTmp;
                        aTmp.first = pInnerBox;
                        aTmp.second = -nDiff;
                        aNewWidthList.push_back( aTmp );
                        aTmp.first = pOuterBox;
                        aTmp.second = nDiff;
                        aNewWidthList.push_back( aTmp );
                    }
                    pOuterBox = pOuterBox == pRightBox ? 0 : pRightBox;
                    if( nDiff2 )
                        nDiff = nDiff2;
                } while( pOuterBox );
            }
        }
        if( nLeftSpanCnt )
            --nLeftSpanCnt;
        if( nRightSpanCnt )
            --nRightSpanCnt;
        pRet->insertBoxes( pBoxes );
    }
    pRet->mnMergeWidth = nMax - nMin;
    if( nCheckBottom > nBottom )
        bOkay = false;
    if( bOkay )
    {
        std::list< std::pair< SwTableBox*, long > >::iterator 
            pCurr = aNewWidthList.begin();
        while( pCurr != aNewWidthList.end() )
        {
            SwFrmFmt* pFmt = pCurr->first->ClaimFrmFmt();
            long nNewWidth = pFmt->GetFrmSize().GetWidth() + pCurr->second;
            pFmt->SetAttr( SwFmtFrmSize( ATT_VAR_SIZE, nNewWidth, 0 ) );
            ++pCurr;
        }
    }
    else
    {
        delete pRet;
        pRet = 0;
    }
    return pRet;
}

void lcl_InvalidateCellFrm( const SwTableBox& rBox )
{
    SwClientIter aIter( *rBox.GetFrmFmt() );
    SwClient* pLast;
    for( pLast = aIter.First( TYPE( SwFrm ) ); pLast; pLast = aIter.Next() )
    {
        SwCellFrm *pCell = (SwCellFrm*)pLast;
        if( pCell->GetTabBox() == &rBox )
        {
            pCell->InvalidateSize();
            SwFrm* pLower = pCell->GetLower();
            if( pLower )
                pLower->_InvalidateSize();
        }
    }
}

long lcl_SumBoxWidth( const SwTableBoxes& rBoxes )
{
    long nSum = 0;
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
        nSum += rBoxes[i]->GetFrmFmt()->GetFrmSize().GetWidth();
    return nSum;
}

long lcl_InsertPosition( SwTable &rTable, std::vector<USHORT>& rInsPos,
    const SwSelBoxes& rBoxes, BOOL bBehind )
{
    long nAddWidth = 0;
    long nCount = 0;
    for( USHORT j = 0; j < rBoxes.Count(); ++j )
    {
        SwTableBox *pBox = rBoxes[j];
        SwTableLine* pLine = pBox->GetUpper();
        long nWidth = rBoxes[j]->GetFrmFmt()->GetFrmSize().GetWidth();
        nAddWidth += nWidth;
        USHORT nCurrBox = pLine->GetTabBoxes().C40_GETPOS(SwTableBox, pBox );
        USHORT nCurrLine = rTable.GetTabLines().C40_GETPOS(SwTableLine, pLine );
        ASSERT( nCurrLine != USHRT_MAX, "Time to say Good-Bye.." );
        if( rInsPos[ nCurrLine ] == USHRT_MAX )
        {
            rInsPos[ nCurrLine ] = nCurrBox;
            ++nCount;
        }
        else if( ( rInsPos[ nCurrLine ] > nCurrBox ) == !bBehind )
            rInsPos[ nCurrLine ] = nCurrBox;
    }
    if( nCount )
        nAddWidth /= nCount;
    return nAddWidth;
}

/** SwTable::NewInsertCol(..) insert new column(s) into a table


@param pDoc
the document

@param rBoxes
the selected boxes

@param nCnt
the number of columns to insert

@param bBehind
insertion behind (true) or before (false) the selected boxes

@return true, if any insertion has been done successfully

*/

BOOL SwTable::NewInsertCol( SwDoc* pDoc, const SwSelBoxes& rBoxes,
    USHORT nCnt, BOOL bBehind )
{
    if( !aLines.Count() || !nCnt )
        return FALSE;

    CHECK_TABLE( *this )
    long nOld = 0;
    for( USHORT i = 0; i < aLines[0]->GetTabBoxes().Count(); ++i )
        nOld += aLines[0]->GetTabBoxes()[i]->GetFrmFmt()->GetFrmSize().GetWidth();
    
    std::vector< USHORT > aInsPos( aLines.Count(), USHRT_MAX );
    sal_uInt64 nAddWidth = lcl_InsertPosition( *this, aInsPos, rBoxes, bBehind );
    nAddWidth *= nCnt;
    sal_uInt64 nNew = nAddWidth + nOld;
    if( !nNew )
        return FALSE;
    nAddWidth *= nOld;
    nAddWidth /= nNew;
    nAddWidth /= nCnt;
    nAddWidth *= nCnt;
    if( !nAddWidth || nAddWidth >= nOld )
        return FALSE;

    _FndBox aFndBox( 0, 0 );
    aFndBox.SetTableLines( rBoxes, *this );
    aFndBox.DelFrms( *this );
    aFndBox.SaveChartData( *this );

    nNew = nOld - nAddWidth;
    nAddWidth /= nCnt;

    AdjustWidths( nOld, long(nNew) );
    SwTableNode* pTblNd = GetTableNode();
    std::vector<SwTableBoxFmt*> aInsFormat( nCnt, 0 );
    USHORT nLastLine = USHRT_MAX;
    long nLastRowSpan = 1;

    for( USHORT i = 0; i < aLines.Count(); ++i )
    {
        SwTableLine* pLine = aLines[ i ];
        USHORT nInsPos = aInsPos[i];
        ASSERT( nInsPos != USHRT_MAX, "Didn't found insert position" );
        SwTableBox* pBox = pLine->GetTabBoxes()[ nInsPos ];
        if( bBehind )
            ++nInsPos;
        SwTableBoxFmt* pBoxFrmFmt = (SwTableBoxFmt*)pBox->GetFrmFmt();
        ::_InsTblBox( pDoc, pTblNd, pLine, pBoxFrmFmt, pBox, nInsPos, nCnt );
        long nRowSpan = pBox->getRowSpan();
        long nDiff = i - nLastLine;
        bool bNewSpan = false;
        if( nLastLine != USHRT_MAX && nDiff < nLastRowSpan &&
            nRowSpan != nDiff - nLastRowSpan )
        {
            bNewSpan = true;
            while( nLastLine < i )
            {
                SwTableLine* pTmpLine = aLines[ nLastLine ];
                USHORT nTmpPos = aInsPos[nLastLine];
                if( bBehind )
                    ++nTmpPos;
                for( USHORT j = 0; j < nCnt; ++j )
                    pTmpLine->GetTabBoxes()[nTmpPos+j]->setRowSpan( nDiff );
                if( nDiff > 0 )
                    nDiff = -nDiff;
                ++nDiff;
                ++nLastLine;
            }
        }
        if( nRowSpan > 0 )
            bNewSpan = true;
        if( bNewSpan )
        {
            nLastLine = i;
            if( nRowSpan < 0 )
                nLastRowSpan = -nRowSpan;
            else
                nLastRowSpan = nRowSpan;
        }
        const SvxBoxItem& aSelBoxItem = pBoxFrmFmt->GetBox();
        SvxBoxItem* pNoRightBorder = 0;
        if( aSelBoxItem.GetRight() )
        {
            pNoRightBorder = new SvxBoxItem( aSelBoxItem );
            pNoRightBorder->SetLine( 0, BOX_LINE_RIGHT );
        }
        for( USHORT j = 0; j < nCnt; ++j )
        {
            SwTableBox *pCurrBox = pLine->GetTabBoxes()[nInsPos+j];
            if( bNewSpan )
            {
                pCurrBox->setRowSpan( nLastRowSpan );
                SwFrmFmt* pFrmFmt = pCurrBox->ClaimFrmFmt();
                SwFmtFrmSize aFrmSz( pFrmFmt->GetFrmSize() );
                aFrmSz.SetWidth( long(nAddWidth) );
                pFrmFmt->SetAttr( aFrmSz );
                if( pNoRightBorder && ( !bBehind || j+1 < nCnt ) )
                    pFrmFmt->SetAttr( *pNoRightBorder );
                aInsFormat[j] = (SwTableBoxFmt*)pFrmFmt;
            }
            else
                pCurrBox->ChgFrmFmt( aInsFormat[j] );
        }
        if( bBehind && pNoRightBorder )
        {
            SwFrmFmt* pFrmFmt = pBox->ClaimFrmFmt();
            pFrmFmt->SetAttr( *pNoRightBorder );
        }
        delete pNoRightBorder;
    }

    aFndBox.MakeFrms( *this );
    aFndBox.RestoreChartData( *this );
#ifndef PRODUCT
    {
        const SwTableBoxes &rTabBoxes = aLines[0]->GetTabBoxes();
        long nNewWidth = 0;
        for( USHORT i = 0; i < rTabBoxes.Count(); ++i )
            nNewWidth += rTabBoxes[i]->GetFrmFmt()->GetFrmSize().GetWidth();
        ASSERT( nNewWidth > 0, "Very small" );
    }
#endif
    CHECK_TABLE( *this )

    return TRUE;
}

/** SwTable::PrepareMerge(..) some preparation for the coming Merge(..)

For the old table model, ::GetMergeSel(..) is called only,
for the new table model, PrepareMerge does the main work.
It modifices all cells to merge (width, border, rowspan etc.) and collects
the cells which have to be deleted by Merge(..) afterwards.
If there are superfluous rows, these cells are put into the deletion list as well.

@param rPam
the selection to merge

@param rBoxes
should be empty at the beginning, at the end it is filled with boxes to delete.

@param ppMergeBox
will be set to the master cell box

@param pUndo
the undo object to record all changes
can be Null, e.g. when called by Redo(..)

@return

*/

bool SwTable::PrepareMerge( const SwPaM& rPam, SwSelBoxes& rBoxes,
   SwSelBoxes& rMerged, SwTableBox** ppMergeBox, SwUndoTblMerge* pUndo )
{
    if( !bNewModel )
    {
        ::GetMergeSel( rPam, rBoxes, ppMergeBox, pUndo );
        return rBoxes.Count() > 1;
    }
    CHECK_TABLE( *this )
    // We have to assert a "rectangular" box selection before we start to merge
    std::auto_ptr< SwBoxSelection > pSel( CollectBoxSelection( rPam ) );
    if( !pSel.get() || pSel->isEmpty() )
        return false;
    // Now we should have a rectangle of boxes,
    // i.e. contiguous cells in contiguous rows
    bool bMerge = false; // will be set if any content is transferred from
    // a "not already overlapped" cell into the new master cell.
    SwTableBox *pMergeBox = (*pSel->aBoxes[0])[0]; // the master cell box
    if( !pMergeBox )
        return false;
    (*ppMergeBox) = pMergeBox;
    // The new master box will get the left and the top border of the top-left
    // box of the selection and because the new master cell _is_ the top-left
    // box, the left and right border does not need to be changed.
    // The right and bottom border instead has to be derived from the right-
    // bottom box of the selection. If this is a overlapped cell,
    // the appropiate master box.
    SwTableBox* pLastBox = 0; // the right-bottom (master) cell
    SwDoc* pDoc = GetFrmFmt()->GetDoc();
    SwPosition aInsPos( *pMergeBox->GetSttNd()->EndOfSectionNode() );
    SwPaM aChkPam( aInsPos );
    // The number of lines in the selection rectangle: nLineCount
    const USHORT nLineCount = USHORT(pSel->aBoxes.size());
    // BTW: nLineCount is the rowspan of the new master cell
    long nRowSpan = nLineCount;
    // We will need the first and last line of the selection
    // to check if there any superfluous row after merging
    SwTableLine* pFirstLn = 0;
    SwTableLine* pLastLn = 0;
    // Iteration over the lines of the selection...
    for( USHORT nCurrLine = 0; nCurrLine < nLineCount; ++nCurrLine )
    {
        // The selected boxes in the current line
        const SwSelBoxes* pBoxes = pSel->aBoxes[ nCurrLine ];
        USHORT nColCount = pBoxes->Count();
        // Iteration over the selected cell in the current row
        for( USHORT nCurrCol = 0; nCurrCol < nColCount; ++nCurrCol )
        {
            SwTableBox* pBox = (*pBoxes)[nCurrCol];
            rMerged.Insert( pBox );
            // Only the first selected cell in every row will be alive,
            // the other will be deleted => put into rBoxes
            if( nCurrCol )
                rBoxes.Insert( pBox );
            else
            {
                if( nCurrLine == 1 )
                    pFirstLn = pBox->GetUpper(); // we need this line later on
                if( nCurrLine + 1 == nLineCount )
                    pLastLn = pBox->GetUpper(); // and this one, too.
            }
            // A box has to be merged if it's not the master box itself,
            // but an already overlapped cell must not be merged as well.
            bool bDoMerge = pBox != pMergeBox && pBox->getRowSpan() > 0;
            // The last box has to be in the last "column" of the selection
            // and it has to be a master cell
            if( nCurrCol+1 == nColCount && pBox->getRowSpan() > 0 )
                pLastBox = pBox;
            if( bDoMerge )
            {
                bMerge = true;
                // If the cell to merge contains only one empty paragraph,
                // we do not transfer this paragraph.
                if( !IsEmptyBox( *pBox, aChkPam ) )
                {
                    SwNodeIndex& rInsPosNd = aInsPos.nNode;
                    SwPaM aPam( aInsPos );
                    aPam.GetPoint()->nNode.Assign( *pBox->GetSttNd()->EndOfSectionNode(), -1 );
                    SwCntntNode* pCNd = aPam.GetCntntNode();
                    USHORT nL = pCNd ? pCNd->Len() : 0;
                    aPam.GetPoint()->nContent.Assign( pCNd, nL );
                    SwNodeIndex aSttNdIdx( *pBox->GetSttNd(), 1 );
                    if( pUndo )
                        pDoc->DoUndo( FALSE );
                    pDoc->AppendTxtNode( *aPam.GetPoint() );
                    if( pUndo )
                        pDoc->DoUndo( TRUE );
                    SwNodeRange aRg( aSttNdIdx, aPam.GetPoint()->nNode );
                    if( pUndo )
                        pUndo->MoveBoxCntnt( pDoc, aRg, rInsPosNd );
                    else
                        pDoc->Move( aRg, rInsPosNd, IDocumentContentOperations::DOC_NO_DELFRMS );
                }
            }
            // Only the cell of the first selected column will stay alive
            // and got a new row span
            if( !nCurrCol )
                pBox->setRowSpan( nRowSpan );
        }
        if( nRowSpan > 0 ) // the master cell is done, from now on we set
            nRowSpan = -nRowSpan; // negative row spans
        ++nRowSpan; // ... -3, -2, -1
    }
    if( bMerge )
    {
        // A row containing overlapped cells is superfluous,
        // these cells can be put into rBoxes for deletion
        _FindSuperfluousRows( rBoxes, pFirstLn, pLastLn );
        // pNewFmt will be set to the new master box and the overlapped cells
        SwFrmFmt* pNewFmt = pMergeBox->ClaimFrmFmt();
        pNewFmt->SetAttr( SwFmtFrmSize( ATT_VAR_SIZE, pSel->mnMergeWidth, 0 ) );
        for( USHORT nCurrLine = 0; nCurrLine < nLineCount; ++nCurrLine )
        {
            const SwSelBoxes* pBoxes = pSel->aBoxes[ nCurrLine ];
            USHORT nColCount = pBoxes->Count();
            for( USHORT nCurrCol = 0; nCurrCol < nColCount; ++nCurrCol )
            {
                SwTableBox* pBox = (*pBoxes)[nCurrCol];
                if( nCurrCol )
                {
                    // Even this box will be deleted soon,
                    // we have to correct the width to avoid side effects
                    SwFrmFmt* pFmt = pBox->ClaimFrmFmt();
                    pFmt->SetAttr( SwFmtFrmSize( ATT_VAR_SIZE, 0, 0 ) );
                }
                else
                    pBox->ChgFrmFmt( (SwTableBoxFmt*)pNewFmt );
            }
        }
        if( pLastBox ) // Robust
        {
            // The new borders of the master cell...
            SvxBoxItem aBox( pMergeBox->GetFrmFmt()->GetBox() );
            bool bOld = aBox.GetRight() || aBox.GetBottom();
            const SvxBoxItem& rBox = pLastBox->GetFrmFmt()->GetBox();
            aBox.SetLine( rBox.GetRight(), BOX_LINE_RIGHT );
            aBox.SetLine( rBox.GetBottom(), BOX_LINE_BOTTOM );
            if( bOld || aBox.GetLeft() || aBox.GetTop() || aBox.GetRight() || aBox.GetBottom() )
                (*ppMergeBox)->GetFrmFmt()->SetAttr( aBox );
        }

        if( pUndo )
            pUndo->AddNewBox( pMergeBox->GetSttIdx() );
    }
    return bMerge;
}

void SwTable::_FindSuperfluousRows( SwSelBoxes& rBoxes,
    SwTableLine* pFirstLn, SwTableLine* pLastLn )
{
    if( !pFirstLn || !pLastLn )
    {
        if( !rBoxes.Count() )
            return;
        pFirstLn = rBoxes[0]->GetUpper();
        pLastLn = rBoxes[ rBoxes.Count() - 1 ]->GetUpper();
    }
    USHORT nFirstLn = GetTabLines().C40_GETPOS(SwTableLine, pFirstLn );
    USHORT nLastLn = GetTabLines().C40_GETPOS(SwTableLine, pLastLn );
    for( USHORT nRow = nFirstLn; nRow <= nLastLn; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        ASSERT( pLine, "Missing table line" );
        USHORT nCols = pLine->GetTabBoxes().Count();
        bool bSuperfl = true;
        for( USHORT nCol = 0; nCol < nCols; ++nCol )
        {
            SwTableBox *pBox = pLine->GetTabBoxes()[nCol];
            if( pBox->getRowSpan() > 0 &&
                USHRT_MAX == rBoxes.GetPos( pBox ) )
            {
                bSuperfl = false;
                break;
            }
        }
        if( bSuperfl )
        {
            for( USHORT nCol = 0; nCol < nCols; ++nCol )
            {
                SwTableBox* pBox = pLine->GetTabBoxes()[nCol];
                rBoxes.Insert( pBox );
            }
        }
    }
}

SwTableBox& SwTableBox::FindStartOfRowSpan( const SwTable& rTable, USHORT nMaxStep )
{
    if( getRowSpan() > 0 || !nMaxStep )
        return *this;

    const USHORT nSteps = nMaxStep;

    long nLeftBorder = lcl_Box2LeftBorder( *this );
    SwTableBox* pBox = this;
    const SwTableLine* pMyUpper = GetUpper();
    USHORT nLine = rTable.GetTabLines().C40_GETPOS(SwTableLine, pMyUpper );
    if( nLine && nLine < rTable.GetTabLines().Count() )
    {
        SwTableBox* pNext;
        do
        {
            pNext = lcl_LeftBorder2Box( nLeftBorder, rTable.GetTabLines()[--nLine] );
            if( pNext )
                pBox = pNext;
        } while( nLine && --nMaxStep && pNext && pBox->getRowSpan() < 1 );
    }

    return *pBox;
}

SwTableBox& SwTableBox::FindEndOfRowSpan( const SwTable& rTable, USHORT nMaxStep )
{
    long nAbsSpan = getRowSpan();
    if( nAbsSpan < 0 )
        nAbsSpan = -nAbsSpan;
    if( nAbsSpan == 1 || !nMaxStep )
        return *this;

    const USHORT nSteps = nMaxStep;

    if( nMaxStep > --nAbsSpan )
        nMaxStep = (USHORT)nAbsSpan;
    const SwTableLine* pMyUpper = GetUpper();
    USHORT nLine = rTable.GetTabLines().C40_GETPOS(SwTableLine, pMyUpper );
    nMaxStep = nLine + nMaxStep;
    if( nMaxStep >= rTable.GetTabLines().Count() )
        nMaxStep = rTable.GetTabLines().Count() - 1;
    long nLeftBorder = lcl_Box2LeftBorder( *this );
    SwTableBox* pBox =
        lcl_LeftBorder2Box( nLeftBorder, rTable.GetTabLines()[ nMaxStep ] );
    if ( !pBox )
        pBox = this;

    return *pBox;
}

void lcl_getAllMergedBoxes( const SwTable& rTable, SwSelBoxes& rBoxes, SwTableBox& rBox )
{
    SwTableBox* pBox = &rBox;
    ASSERT( pBox == &rBox.FindStartOfRowSpan( rTable, USHRT_MAX ), "Not a master box" );
    rBoxes.Insert( pBox );
    if( pBox->getRowSpan() == 1 )
        return;
    const SwTableLine* pMyUpper = pBox->GetUpper();
    USHORT nLine = rTable.GetTabLines().C40_GETPOS(SwTableLine, pMyUpper );
    long nLeftBorder = lcl_Box2LeftBorder( *pBox );
    USHORT nCount = rTable.GetTabLines().Count();
    while( ++nLine < nCount && pBox && pBox->getRowSpan() != -1 )
    {
        pBox = lcl_LeftBorder2Box( nLeftBorder, rTable.GetTabLines()[nLine] );
        if( pBox )
            rBoxes.Insert( pBox );
    };
}

void lcl_UnMerge( const SwTable& rTable, SwTableBox& rBox, USHORT nCnt,
    BOOL bSameHeight )
{
    SwSelBoxes aBoxes;
    lcl_getAllMergedBoxes( rTable, aBoxes, rBox );
    USHORT nCount = aBoxes.Count();
    if( nCount < 2 )
        return;
    if( nCnt > nCount )
        nCnt = nCount;
    USHORT nIdx = 0;
    USHORT *pSplitIdx = new USHORT[ nCnt ];
    if( bSameHeight )
    {
        SwTwips *pHeights = new SwTwips[ nCount ];
        SwTwips nHeight = 0;
        for( USHORT i = 0; i < nCount; ++i )
        {
            SwTableLine* pLine = aBoxes[ i ]->GetUpper();
            SwFrmFmt *pRowFmt = pLine->GetFrmFmt();
            pHeights[ i ] = pRowFmt->GetFrmSize().GetHeight();
            nHeight += pHeights[ i ];
        }
        SwTwips nSumH = 0;
        USHORT nIdx = 0;
        for( USHORT i = 1; i <= nCnt; ++i )
        {
            SwTwips nSplit = ( i * nHeight ) / nCnt;
            while( nSumH < nSplit && nIdx < nCount )
                nSumH += pHeights[ nIdx++ ];
            pSplitIdx[ i - 1 ] = nIdx;
        }
        delete[] pHeights;
    }
    else
    {
        for( long i = 1; i <= nCnt; ++i )
            pSplitIdx[ i - 1 ] = (USHORT)( ( i * nCount ) / nCnt );
    }
    for( long i = 0; i < nCnt; ++i )
    {
        USHORT nNextIdx = pSplitIdx[ i ];
        aBoxes[ nIdx ]->setRowSpan( nNextIdx - nIdx );
        lcl_InvalidateCellFrm( *aBoxes[ nIdx ] );
        while( ++nIdx < nNextIdx )
            aBoxes[ nIdx ]->setRowSpan( nIdx - nNextIdx );
    }
    delete[] pSplitIdx;
}

void lcl_FillSelBoxes( SwSelBoxes &rBoxes, SwTableLine &rLine )
{
    USHORT nBoxCount = rLine.GetTabBoxes().Count();
    USHORT nCurrBox;
    for( nCurrBox = 0; nCurrBox < nBoxCount; ++nCurrBox )
        rBoxes.Insert( rLine.GetTabBoxes()[nCurrBox] );
}

void SwTable::InsertSpannedRow( SwDoc* pDoc, USHORT nRowIdx, USHORT nCnt )
{
    CHECK_TABLE( *this )
    ASSERT( nCnt && nRowIdx < GetTabLines().Count(), "Wrong call of InsertSpannedRow" );
    SwSelBoxes aBoxes;
    SwTableLine& rLine = *GetTabLines()[ nRowIdx ];
    lcl_FillSelBoxes( aBoxes, rLine );
    SwFmtFrmSize aFSz( rLine.GetFrmFmt()->GetFrmSize() );
    if( ATT_VAR_SIZE != aFSz.GetHeightSizeType() )
    {
        SwFrmFmt* pFrmFmt = rLine.ClaimFrmFmt();
        long nNewHeight = aFSz.GetHeight() / ( nCnt + 1 );
        if( !nNewHeight )
            ++nNewHeight;
        aFSz.SetHeight( nNewHeight );
        pFrmFmt->SetAttr( aFSz );
    }
    _InsertRow( pDoc, aBoxes, nCnt, TRUE );
    USHORT nBoxCount = rLine.GetTabBoxes().Count();
    for( USHORT n = 0; n < nCnt; ++n )
    {
        SwTableLine *pNewLine = GetTabLines()[ nRowIdx + nCnt - n ];
        for( USHORT nCurrBox = 0; nCurrBox < nBoxCount; ++nCurrBox )
        {
            long nRowSpan = rLine.GetTabBoxes()[nCurrBox]->getRowSpan();
            if( nRowSpan > 0 )
                nRowSpan = - nRowSpan;
            pNewLine->GetTabBoxes()[ nCurrBox ]->setRowSpan( nRowSpan - n );
        }
    }
    lcl_ChangeRowSpan( *this, nCnt, nRowIdx, false );
    CHECK_TABLE( *this )
}

typedef std::pair< USHORT, USHORT > SwLineOffset;
typedef std::list< SwLineOffset > SwLineOffsetArray;

/******************************************************************************
When a couple of table boxes has to be split,
lcl_SophisticatedFillLineIndices delivers the information where and how many
rows have to be inserted.
Input
    rTable: the table to manipulate
    rBoxes: an array of boxes to split
    nCnt:   how many parts are wanted
Output
    rArr:   a list of pairs ( line index, number of lines to insert )

******************************************************************************/

void lcl_SophisticatedFillLineIndices( SwLineOffsetArray &rArr,
    const SwTable& rTable, const SwSelBoxes& rBoxes, USHORT nCnt )
{
    std::list< SwLineOffset > aBoxes;
    SwLineOffset aLnOfs( USHRT_MAX, USHRT_MAX );
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {   // Collect all end line indices and the row spans
        const SwTableBox &rBox = rBoxes[ i ]->FindStartOfRowSpan( rTable );
        ASSERT( rBox.getRowSpan() > 0, "Didn't I say 'StartOfRowSpan' ??" )
        if( nCnt > rBox.getRowSpan() )
        {
            const SwTableLine *pLine = rBox.GetUpper();
            const USHORT nEnd = USHORT( rBox.getRowSpan() +
                rTable.GetTabLines().C40_GETPOS( SwTableLine, pLine ) );
            // The next if statement is a small optimization
            if( aLnOfs.first != nEnd || aLnOfs.second != rBox.getRowSpan() )
            {
                aLnOfs.first = nEnd; // ok, this is the line behind the box
                aLnOfs.second = USHORT( rBox.getRowSpan() ); // the row span
                aBoxes.insert( aBoxes.end(), aLnOfs );
            }
        }
    }
    // As I said, I noted the line index _behind_ the last line of the boxes
    // in the resulting array the index has to be _on_ the line
    // nSum is to evaluate the wished value
    USHORT nSum = 1;
    while( aBoxes.size() )
    {
        // I. step:
        // Looking for the "smallest" line end with the smallest row span
        std::list< SwLineOffset >::iterator pCurr = aBoxes.begin();
        aLnOfs = *pCurr; // the line end and row span of the first box
        while( ++pCurr != aBoxes.end() )
        {
            if( aLnOfs.first > pCurr->first )
            {   // Found a smaller line end
                aLnOfs.first = pCurr->first;
                aLnOfs.second = pCurr->second; // row span
            }
            else if( aLnOfs.first == pCurr->first &&
                     aLnOfs.second < pCurr->second )
                aLnOfs.second = pCurr->second; // Found a smaller row span
        }
        ASSERT( aLnOfs.second < nCnt, "Clean-up failed" )
        aLnOfs.second = nCnt - aLnOfs.second; // the number of rows to insert
        rArr.insert( rArr.end(),
            SwLineOffset( aLnOfs.first - nSum, aLnOfs.second ) );
        // the correction has to be incremented because in the following
        // loops the line ends were manipulated
        nSum = nSum + aLnOfs.second;

        pCurr = aBoxes.begin();
        while( pCurr != aBoxes.end() )
        {
            if( pCurr->first == aLnOfs.first )
            {   // These boxes can be removed because the last insertion
                // of rows will expand their row span above the needed value
                std::list< SwLineOffset >::iterator pDel = pCurr;
                ++pCurr;
                aBoxes.erase( pDel );
            }
            else
            {
                bool bBefore = ( pCurr->first - pCurr->second < aLnOfs.first );
                // Manipulation of the end line indices as if the rows are
                // already inserted
                pCurr->first = pCurr->first + aLnOfs.second;
                if( bBefore )
                {   // If the insertion is inside the box,
                    // its row span has to be incremented
                    pCurr->second = pCurr->second + aLnOfs.second;
                    if( pCurr->second >= nCnt )
                    {   // if the row span is bigger than the split factor
                        // this box is done
                        std::list< SwLineOffset >::iterator pDel = pCurr;
                        ++pCurr;
                        aBoxes.erase( pDel );
                    }
                    else
                        ++pCurr;
                }
                else
                    ++pCurr;
            }
        }
    }
}

typedef std::set< SwTwips > SwSplitLines;

USHORT lcl_CalculateSplitLineHeights( SwSplitLines &rCurr, SwSplitLines &rNew,
    const SwTable& rTable, const SwSelBoxes& rBoxes, USHORT nCnt )
{
    if( nCnt < 2 )
        return 0;
    std::list< SwLineOffset > aBoxes;
    SwLineOffset aLnOfs( USHRT_MAX, USHRT_MAX );
    USHORT nFirst = USHRT_MAX; // becomes the index of the first line
    USHORT nLast = 0; // becomes the index of the last line of the splitting
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {   // Collect all pairs (start+end) of line indices to split
        const SwTableBox &rBox = rBoxes[ i ]->FindStartOfRowSpan( rTable );
        ASSERT( rBox.getRowSpan() > 0, "Didn't I say 'StartOfRowSpan' ??" )
        const SwTableLine *pLine = rBox.GetUpper();
        const USHORT nStart = rTable.GetTabLines().C40_GETPOS( SwTableLine, pLine );
        const USHORT nEnd = USHORT( rBox.getRowSpan() + nStart - 1 );
        // The next if statement is a small optimization
        if( aLnOfs.first != nStart || aLnOfs.second != nEnd )
        {
            aLnOfs.first = nStart;
            aLnOfs.second = nEnd;
            aBoxes.insert( aBoxes.end(), aLnOfs );
            if( nStart < nFirst )
                nFirst = nStart;
            if( nEnd > nLast )
                nLast = nEnd;
        }
    }

    if( aBoxes.empty() )
        return 0;
    SwTwips nHeight = 0;
    SwTwips* pLines = new SwTwips[ nLast + 1 - nFirst ];
    for( USHORT i = nFirst; i <= nLast; ++i )
    {
        nHeight += rTable.GetTabLines()[ i ]->GetTableLineHeight();
        rCurr.insert( rCurr.end(), nHeight );
        pLines[ i - nFirst ] = nHeight;
    }
    std::list< SwLineOffset >::iterator pSplit = aBoxes.begin();
    while( pSplit != aBoxes.end() )
    {
        SwTwips nBase = pSplit->first <= nFirst ? 0 :
                        pLines[ pSplit->first - nFirst - 1 ];
        SwTwips nDiff = pLines[ pSplit->second - nFirst ] - nBase;
        for( USHORT i = 1; i < nCnt; ++i )
        {
            SwTwips nSplit = nBase + ( i * nDiff ) / nCnt;
            rNew.insert( nSplit );
        }
        ++pSplit;
    }
    return nFirst;
}

USHORT lcl_LineIndex( const SwTable& rTable, const SwSelBoxes& rBoxes,
                      bool bBehind )
{
    USHORT nDirect = USHRT_MAX;
    USHORT nSpan = USHRT_MAX;
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {
        SwTableBox *pBox = rBoxes[i];
        const SwTableLine* pLine = rBoxes[i]->GetUpper();
        USHORT nPos = rTable.GetTabLines().C40_GETPOS( SwTableLine, pLine );
        if( USHRT_MAX != nPos )
        {
            if( bBehind )
            {
                if( nPos > nDirect || nDirect == USHRT_MAX )
                    nDirect = nPos;
                long nRowSpan = pBox->getRowSpan();
                if( nRowSpan < 2 )
                    nSpan = 0;
                else if( nSpan )
                {
                    USHORT nEndOfRowSpan = (USHORT)(nPos + nRowSpan - 1);
                    if( nEndOfRowSpan > nSpan || nSpan == USHRT_MAX )
                        nSpan = nEndOfRowSpan;
                }
            }
            else if( nPos < nDirect )
                nDirect = nPos;
        }
    }
    if( nSpan && nSpan < USHRT_MAX )
        return nSpan;
    return nDirect;
}

BOOL SwTable::NewSplitRow( SwDoc* pDoc, const SwSelBoxes& rBoxes, USHORT nCnt,
    BOOL bSameHeight )
{
    CHECK_TABLE( *this )
    ++nCnt;
    _FndBox aFndBox( 0, 0 );
    aFndBox.SetTableLines( rBoxes, *this );
    aFndBox.DelFrms( *this );
    aFndBox.SaveChartData( *this );

    if( bSameHeight && pDoc->GetRootFrm() )
    {
        SwSplitLines aRowLines;
        SwSplitLines aSplitLines;
        USHORT nFirst = lcl_CalculateSplitLineHeights( aRowLines, aSplitLines,
            *this, rBoxes, nCnt );
        SwTwips nLast = 0;
        SwSplitLines::iterator pSplit = aSplitLines.begin();
        SwSplitLines::iterator pCurr = aRowLines.begin();
        while( pCurr != aRowLines.end() )
        {
            while( pSplit != aSplitLines.end() && *pSplit < *pCurr )
            {
                InsertSpannedRow( pDoc, nFirst, 1 );
                SwTableLine* pRow = GetTabLines()[ nFirst ];
                SwFrmFmt* pRowFmt = pRow->ClaimFrmFmt();
                SwFmtFrmSize aFSz( pRowFmt->GetFrmSize() );
                aFSz.SetHeightSizeType( ATT_MIN_SIZE );
                aFSz.SetHeight( *pSplit - nLast );
                pRowFmt->SetAttr( aFSz );
                nLast = *pSplit;
                ++pSplit;
                ++nFirst;
            }
            if( pSplit != aSplitLines.end() && nLast == *pSplit )
                ++pSplit;
            SwTableLine* pRow = GetTabLines()[ nFirst ];
            SwFrmFmt* pRowFmt = pRow->ClaimFrmFmt();
            SwFmtFrmSize aFSz( pRowFmt->GetFrmSize() );
            aFSz.SetHeightSizeType( ATT_MIN_SIZE );
            aFSz.SetHeight( *pCurr - nLast );
            pRowFmt->SetAttr( aFSz );
            nLast = *pCurr;
            ++pCurr;
            ++nFirst;
        }
    }
    else
        bSameHeight = FALSE;
    if( !bSameHeight )
    {
        SwLineOffsetArray aLineOffs;
        lcl_SophisticatedFillLineIndices( aLineOffs, *this, rBoxes, nCnt );
        SwLineOffsetArray::reverse_iterator pCurr( aLineOffs.rbegin() );
        while( pCurr != aLineOffs.rend() )
        {
            InsertSpannedRow( pDoc, pCurr->first, pCurr->second );
            ++pCurr;
        }
    }

    std::set< USHORT> aIndices;
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {
        ASSERT( rBoxes[i]->getRowSpan() != 1, "Forgot to split?" )
        if( rBoxes[i]->getRowSpan() > 1 )
            aIndices.insert( i );
    }

    std::set< USHORT >::iterator pCurrBox = aIndices.begin();
    while( pCurrBox != aIndices.end() )
        lcl_UnMerge( *this, *rBoxes[*pCurrBox++], nCnt, bSameHeight );

    CHECK_TABLE( *this )
    //Layout updaten
    aFndBox.MakeFrms( *this );
    aFndBox.RestoreChartData( *this );

    return TRUE;
}

BOOL SwTable::InsertRow( SwDoc* pDoc, const SwSelBoxes& rBoxes,
                        USHORT nCnt, BOOL bBehind )
{
    bool bRet = false;
    if( IsNewModel() )
    {
        CHECK_TABLE( *this )
        USHORT nRowIdx = lcl_LineIndex( *this, rBoxes, bBehind );
        if( nRowIdx < USHRT_MAX )
        {
            _FndBox aFndBox( 0, 0 );
            aFndBox.SetTableLines( rBoxes, *this );
            aFndBox.DelFrms( *this );
            aFndBox.SaveChartData( *this );

            bRet = true;
            SwTableLine *pLine = GetTabLines()[ nRowIdx ];
            SwSelBoxes aLineBoxes;
            lcl_FillSelBoxes( aLineBoxes, *pLine );
            _InsertRow( pDoc, aLineBoxes, nCnt, bBehind );
            USHORT nBoxCount = pLine->GetTabBoxes().Count();
            USHORT nOfs = bBehind ? 0 : 1;
            for( USHORT n = 0; n < nCnt; ++n )
            {
                SwTableLine *pNewLine = GetTabLines()[ nRowIdx+nCnt-n-nOfs];
                for( USHORT nCurrBox = 0; nCurrBox < nBoxCount; ++nCurrBox )
                {
                    long nRowSpan = pLine->GetTabBoxes()[nCurrBox]->getRowSpan();
                    if( bBehind )
                    {
                        if( nRowSpan == 1 || nRowSpan == -1 )
                            nRowSpan = n + 1;
                        else if( nRowSpan > 1 )
                            nRowSpan = - nRowSpan;
                    }
                    else
                    {
                        if( nRowSpan > 0 )
                            nRowSpan = n + 1;
                        else
                            --nRowSpan;
                    }
                    pNewLine->GetTabBoxes()[ nCurrBox ]->setRowSpan( nRowSpan - n );
                }
            }
            if( bBehind )
                ++nRowIdx;
            if( nRowIdx )
                lcl_ChangeRowSpan( *this, nCnt, --nRowIdx, true );
            //Layout update
            aFndBox.MakeFrms( *this );
            aFndBox.RestoreChartData( *this );
        }
        CHECK_TABLE( *this )
    }
    else
        bRet = _InsertRow( pDoc, rBoxes, nCnt, bBehind );
    return bRet;
}

void SwTable::PrepareDelBoxes( const SwSelBoxes& rBoxes )
{
    if( IsNewModel() )
    {
        for( USHORT i = 0; i < rBoxes.Count(); ++i )
        {
            SwTableBox* pBox = rBoxes[i];
            long nRowSpan = pBox->getRowSpan();
            if( nRowSpan != 1 && pBox->GetFrmFmt()->GetFrmSize().GetWidth() )
            {
                long nLeft = lcl_Box2LeftBorder( *pBox );
                SwTableLine *pLine = pBox->GetUpper();
                USHORT nLinePos = GetTabLines().C40_GETPOS(SwTableLine, pLine);
                ASSERT( nLinePos < USHRT_MAX, "Box/table mismatch" )
                if( nRowSpan > 1 )
                {
                    if( ++nLinePos < GetTabLines().Count() )
                    {
                        pLine = GetTabLines()[ nLinePos ];
                        pBox = lcl_LeftBorder2Box( nLeft, pLine );
                        ASSERT( pBox, "RowSpan irritation I" )
                        if( pBox )
                            pBox->setRowSpan( --nRowSpan );
                    }
                }
                else if( nLinePos > 0 )
                {
                    do
                    {
                        pLine = GetTabLines()[ --nLinePos ];
                        pBox = lcl_LeftBorder2Box( nLeft, pLine );
                        ASSERT( pBox, "RowSpan irritation II" )
                        if( pBox )
                        {
                            nRowSpan = pBox->getRowSpan();
                            if( nRowSpan > 1 )
                            {
                                lcl_InvalidateCellFrm( *pBox );
                                --nRowSpan;
                            }
                            else
                                ++nRowSpan;
                            pBox->setRowSpan( nRowSpan );
                        }
                        else
                            nRowSpan = 1;
                    }
                    while( nRowSpan < 0 && nLinePos > 0 );
                }
            }
        }
    }
}

void lcl_SearchSelBox( const SwTable &rTable, SwSelBoxes& rBoxes, long nMin, long nMax,
                       SwTableLine& rLine, bool bChkProtected, bool bColumn )
{
    long nLeft = 0;
    long nRight = 0;
    long nMid = ( nMax + nMin )/ 2;
    USHORT nCount = rLine.GetTabBoxes().Count();
    for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
    {
        SwTableBox* pBox = rLine.GetTabBoxes()[nCurrBox];
        ASSERT( pBox, "Missing table box" );
        long nWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth();
        nRight += nWidth;
        if( nRight > nMin )
        {
            bool bAdd = false;
            if( nRight <= nMax )
                bAdd = nLeft >= nMin || nRight >= nMid ||
                       nRight - nMin > nMin - nLeft;
            else
                bAdd = nLeft <= nMid || nRight - nMax < nMax - nLeft;
            long nRowSpan = pBox->getRowSpan();
            if( bAdd && 
                //( bColumn || nRowSpan > 0 ) && 
                ( !bChkProtected ||
                !pBox->GetFrmFmt()->GetProtect().IsCntntProtected() ) )
            {
                USHORT nOldCnt = rBoxes.Count();
                rBoxes.Insert( pBox );
                if( bColumn && nRowSpan != 1 && nOldCnt < rBoxes.Count() )
                {
                    SwTableBox *pMasterBox = pBox->getRowSpan() > 0 ? pBox
                        : &pBox->FindStartOfRowSpan( rTable, USHRT_MAX );
                    lcl_getAllMergedBoxes( rTable, rBoxes, *pMasterBox );
                }
            }
        }
        if( nRight >= nMax )
            break;
        nLeft = nRight;
    }
}


void SwTable::CreateSelection(  const SwPaM& rPam, SwSelBoxes& rBoxes,
    const SearchType eSearch, bool bChkProtected ) const
{
    ASSERT( bNewModel, "Don't call me for old tables" );
    if( !aLines.Count() )
        return;
    const SwNode* pStartNd = rPam.GetPoint()->nNode.GetNode().FindTableBoxStartNode();
    const SwNode* pEndNd = rPam.GetMark()->nNode.GetNode().FindTableBoxStartNode();
    if( !pStartNd || !pEndNd )
        return;
    CreateSelection( pStartNd, pEndNd, rBoxes, eSearch, bChkProtected );
}

void SwTable::CreateSelection( const SwNode* pStartNd, const SwNode* pEndNd,
    SwSelBoxes& rBoxes, const SearchType eSearch, bool bChkProtected ) const
{
    // SwSelBoxes aKeepBoxes;
    if( rBoxes.Count() )
    {
        // aKeepBoxes.Insert( &rBoxes );
        rBoxes.Remove( USHORT(0), rBoxes.Count() );
    }
    // Looking for start and end of the selection given by SwNode-pointer
    USHORT nLines = aLines.Count();
    // nTop becomes the line number of the upper box
    // nBottom becomes the line number of the lower box
    USHORT nTop = 0, nBottom = 0;
    // nUpperMin becomes the left border value of the upper box
    // nUpperMax becomes the right border of the upper box
    // nLowerMin and nLowerMax the borders of the lower box
    long nUpperMin = 0, nUpperMax = 0;
    long nLowerMin = 0, nLowerMax = 0;
    // nFound will incremented if a box is found
    // 0 => no box found; 1 => the upper box has been found; 2 => both found
    int nFound = 0;
    for( USHORT nRow = 0; nFound < 2 && nRow < nLines; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        ASSERT( pLine, "Missing table line" );
        USHORT nCols = pLine->GetTabBoxes().Count();
        for( USHORT nCol = 0; nCol < nCols; ++nCol )
        {
            SwTableBox* pBox = pLine->GetTabBoxes()[nCol];
            ASSERT( pBox, "Missing table box" );
            if( pBox->GetSttNd() == pEndNd || pBox->GetSttNd() == pStartNd )
            {
                if( !bChkProtected ||
                    !pBox->GetFrmFmt()->GetProtect().IsCntntProtected() )
                    rBoxes.Insert( pBox );
                if( nFound )
                {
                    nBottom = nRow;
                    lcl_CheckMinMax( nLowerMin, nLowerMax, *pLine, nCol, true );
                    ++nFound;
                    break;
                }
                else
                {
                    nTop = nRow;
                    lcl_CheckMinMax( nUpperMin, nUpperMax, *pLine, nCol, true );
                    ++nFound;
                     // If start and end node are identical, we're nearly done..
                    if( pEndNd == pStartNd )
                    {
                        nBottom = nTop;
                        nLowerMin = nUpperMin;
                        nLowerMax = nUpperMax;
                        ++nFound;
                    }
                }
            }
        }
    }
    if( nFound < 2 )
        return; // At least one node was not a part of the given table
    if( eSearch == SEARCH_ROW )
    {
        // Selection of a row is quiet easy:
        // every (unprotected) box between start and end line
        // with a positive row span will be collected
        for( USHORT nRow = nTop; nRow <= nBottom; ++nRow )
        {
            SwTableLine* pLine = aLines[nRow];
            ASSERT( pLine, "Missing table line" );
            USHORT nCount = pLine->GetTabBoxes().Count();
            for( USHORT nCurrBox = 0; nCurrBox < nCount; ++nCurrBox )
            {
                SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
                ASSERT( pBox, "Missing table box" );
                if( pBox->getRowSpan() > 0 && ( !bChkProtected ||
                    !pBox->GetFrmFmt()->GetProtect().IsCntntProtected() ) )
                    rBoxes.Insert( pBox );
            }
        }
        return;
    }
    bool bCombine = nTop == nBottom;
    if( !bCombine )
    {
        long nMinWidth = nUpperMax - nUpperMin;
        long nTmp = nLowerMax - nLowerMin;
        if( nMinWidth > nTmp )
            nMinWidth = nTmp;
        nTmp = nLowerMax < nUpperMax ? nLowerMax : nUpperMax;
        nTmp -= ( nLowerMin < nUpperMin ) ? nUpperMin : nLowerMin;
        // If the overlapping between upper and lower box is less than half
        // of the width (of the smaller cell), bCombine is set,
        // e.g. if upper and lower cell are in different columns
        bCombine = ( nTmp + nTmp < nMinWidth );
    }
    if( bCombine )
    {
        if( nUpperMin < nLowerMin )
            nLowerMin = nUpperMin;
        else
            nUpperMin = nLowerMin;
        if( nUpperMax > nLowerMax )
            nLowerMax = nUpperMax;
        else
            nUpperMax = nLowerMax;
    }
    const bool bColumn = eSearch == SEARCH_COL;
    if( bColumn )
    {
        for( USHORT i = 0; i < nTop; ++i )
            lcl_SearchSelBox( *this, rBoxes, nUpperMin, nUpperMax,
                              *aLines[i], bChkProtected, bColumn );
    }
    
    {
        long nMin = nUpperMin < nLowerMin ? nUpperMin : nLowerMin;
        long nMax = nUpperMax < nLowerMax ? nLowerMax : nUpperMax;
        for( USHORT i = nTop; i <= nBottom; ++i )
            lcl_SearchSelBox( *this, rBoxes, nMin, nMax, *aLines[i],
                              bChkProtected, bColumn );
    }
    /* if( nTop + 1 < nBottom )
    {
        long nInnerMin = nUpperMin < nLowerMin ? nLowerMin : nUpperMin;
        long nInnerMax = nUpperMax < nLowerMax ? nUpperMax : nLowerMax;
        for( USHORT i = nTop + 1; i < nBottom; ++i )
            lcl_SearchSelBox( *this, rBoxes, nInnerMin, nInnerMax, *aLines[i],
                              bChkProtected, bColumn );
    }
    if( bCombine ) // => nUpperMin == nLowerMin, nUpperMax == nLowerMax
    {
        if( nBottom > nTop )
            lcl_SearchSelBox( *this, rBoxes, nUpperMin, nUpperMax, *aLines[nTop],
                              bChkProtected, bColumn );
        lcl_SearchSelBox( *this, rBoxes, nLowerMin, nLowerMax, *aLines[nBottom],
                          bChkProtected, bColumn );
    }
    else if( aKeepBoxes.Count() )
    {
        long nMin = nUpperMin < nLowerMin ? nUpperMin : nLowerMin;
        long nMax = nUpperMax < nLowerMax ? nLowerMax : nUpperMax;
        SwSelBoxes aCandidates;
        for( USHORT i = nTop; i <= nBottom; ++i )
            lcl_SearchSelBox( *this, aCandidates, nMin, nMax, *aLines[i],
                              bChkProtected, bColumn );
        USHORT nOld = 0, nNew = 0;
        while ( nOld < aKeepBoxes.Count() && nNew < aCandidates.Count() )
        {
            const SwTableBox* pPOld = *( aKeepBoxes.GetData() + nOld );
            SwTableBox* pPNew = *( aCandidates.GetData() + nNew );
            if( pPOld == pPNew )
            {   // this box will stay
                rBoxes.Insert( pPNew );
                ++nOld;
                ++nNew;
            }
            else if( pPOld->GetSttIdx() < pPNew->GetSttIdx() )
                ++nOld;
            else
                ++nNew;
        }
    } */
    if( bColumn )
    {
        for( USHORT i = nBottom + 1; i < nLines; ++i )
            lcl_SearchSelBox( *this, rBoxes, nLowerMin, nLowerMax, *aLines[i],
                              bChkProtected, true );
    }
}

void SwTable::ExpandColumnSelection( SwSelBoxes& rBoxes, long &rMin, long &rMax ) const
{
    ASSERT( bNewModel, "Don't call me for old tables" );
    rMin = 0;
    rMax = 0;
    if( !aLines.Count() || !rBoxes.Count() )
        return;

    USHORT nLineCnt = aLines.Count();
    USHORT nBoxCnt = rBoxes.Count();
    USHORT nBox = 0;
    for( USHORT nRow = 0; nRow < nLineCnt && nBox < nBoxCnt; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        ASSERT( pLine, "Missing table line" );
        USHORT nCols = pLine->GetTabBoxes().Count();
        for( USHORT nCol = 0; nCol < nCols; ++nCol )
        {
            SwTableBox* pBox = pLine->GetTabBoxes()[nCol];
            ASSERT( pBox, "Missing table box" );
            if( pBox == rBoxes[nBox] )
            {
                lcl_CheckMinMax( rMin, rMax, *pLine, nCol, nBox == 0 );
                if( ++nBox >= nBoxCnt )
                    break;
            }
        }
    }
    nBox = 0;
    for( USHORT nRow = 0; nRow < nLineCnt; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        USHORT nCols = pLine->GetTabBoxes().Count();
        long nLeft = 0;
        long nRight = 0;
        for( USHORT nCurrBox = 0; nCurrBox < nCols; ++nCurrBox )
        {
            nLeft = nRight;
            SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
            nRight += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
            if( nLeft >= rMin && nRight <= rMax )
                rBoxes.Insert( pBox );
        }
    }
}

void SwTable::PrepareDeleteCol( long nMin, long nMax )
{
    ASSERT( bNewModel, "Don't call me for old tables" );
    if( !aLines.Count() || nMax < nMin )
        return;
    long nMid = nMin ? ( nMin + nMax ) / 2 : 0;
    const SwTwips nTabSize = GetFrmFmt()->GetFrmSize().GetWidth();
    if( nTabSize == nMax )
        nMid = nMax;
    USHORT nLineCnt = aLines.Count();
    for( USHORT nRow = 0; nRow < nLineCnt; ++nRow )
    {
        SwTableLine* pLine = aLines[nRow];
        USHORT nCols = pLine->GetTabBoxes().Count();
        long nLeft = 0;
        long nRight = 0;
        for( USHORT nCurrBox = 0; nCurrBox < nCols; ++nCurrBox )
        {
            nLeft = nRight;
            SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
            nRight += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
            if( nRight < nMin )
                continue;
            if( nLeft > nMax )
                break;
            long nNewWidth = -1;
            if( nLeft < nMin )
            {
                if( nRight <= nMax )
                    nNewWidth = nMid - nLeft;
            }
            else if( nRight > nMax )
                nNewWidth = nRight - nMid;
            else
                nNewWidth = 0;
            if( nNewWidth >= 0 )
            {
                SwFrmFmt* pFrmFmt = pBox->ClaimFrmFmt();
                SwFmtFrmSize aFrmSz( pFrmFmt->GetFrmSize() );
                aFrmSz.SetWidth( nNewWidth );
                pFrmFmt->SetAttr( aFrmSz );
            }
        }
    }
}

bool SwTable::IsSelectionRectangular( const SwSelBoxes& rBoxes ) const
{
    ASSERT( IsNewModel(), "Don't call me for old tables" );
    if( !aLines.Count() || !rBoxes.Count() || !IsNewModel() )
        return true;
    bool bRet = true;
    long nMin = -1;
    long nMax = -1;
    USHORT nBox = 0;
    USHORT nLineCnt = aLines.Count();
    USHORT nBoxCnt = rBoxes.Count();
    SwTableLine* pLine = rBoxes[0]->GetUpper();
    USHORT nLinePos = aLines.C40_GETPOS( SwTableLine, pLine );

    for( USHORT nRow = nLinePos; bRet && nRow < nLineCnt && nBox < nBoxCnt; ++nRow )
    {
        pLine = aLines[nRow];
        USHORT nCols = pLine->GetTabBoxes().Count();
        long nLeft = 0;
        long nRight = 0;
        for( USHORT nCurrBox = 0; bRet && nCurrBox < nCols; ++nCurrBox )
        {
            nLeft = nRight;
            SwTableBox* pBox = pLine->GetTabBoxes()[nCurrBox];
            nRight += pBox->GetFrmFmt()->GetFrmSize().GetWidth();
            if( pBox == rBoxes[nBox] )
            {
                if( nRow == nLinePos )
                {
                    if( nMin < 0 )
                    {
                        nMin = nLeft;
                        nMax = nRight;
                    }
                    else
                    {
                        bRet = nLeft == nMax;
                        nMax = nRight;
                    }
                }
                else
                    bRet = nLeft >= nMin && nRight <= nMax;
                if( ++nBox >= nBoxCnt )
                {
                    if( nRight != nMax )
                        bRet = false;
                    break;
                }
            }
            else if( nMin >= 0 )
                bRet = nLeft >= nMax || nRight <= nMin;
        }
    }
    return bRet;
}

void SwTable::ExpandSelection( SwSelBoxes& rBoxes ) const
{
    for( USHORT i = 0; i < rBoxes.Count(); ++i )
    {
        SwTableBox *pBox = rBoxes[i];
        long nRowSpan = pBox->getRowSpan();
        if( nRowSpan != 1 )
        {
            SwTableBox *pMasterBox = nRowSpan > 0 ? pBox
                    : &pBox->FindStartOfRowSpan( *this, USHRT_MAX );
            lcl_getAllMergedBoxes( *this, rBoxes, *pMasterBox );
        }
    }
}

void SwTable::CheckRowSpan( SwTableLinePtr &rpLine, bool bUp ) const
{
    ASSERT( IsNewModel(), "Don't call me for old tables" );
    USHORT nLineIdx = GetTabLines().C40_GETPOS( SwTableLine, rpLine );
    ASSERT( nLineIdx < GetTabLines().Count(), "Start line out of range" );
    bool bChange = true;
    if( bUp )
    {
        while( bChange )
        {
            bChange = false;
            rpLine = GetTabLines()[ nLineIdx ];
            USHORT nCols = rpLine->GetTabBoxes().Count();
            for( USHORT nCol = 0; !bChange && nCol < nCols; ++nCol )
            {
                SwTableBox* pBox = rpLine->GetTabBoxes()[nCol];
                if( pBox->getRowSpan() > 1 || pBox->getRowSpan() < -1 )
                    bChange = true;
            }
            if( bChange )
            {
                if( nLineIdx )
                    --nLineIdx;
                else
                {
                    bChange = false;
                    rpLine = 0;
                }
            }
        }
    }
    else
    {
        USHORT nMaxLine = GetTabLines().Count();
        while( bChange )
        {
            bChange = false;
            rpLine = GetTabLines()[ nLineIdx ];
            USHORT nCols = rpLine->GetTabBoxes().Count();
            for( USHORT nCol = 0; !bChange && nCol < nCols; ++nCol )
            {
                SwTableBox* pBox = rpLine->GetTabBoxes()[nCol];
                if( pBox->getRowSpan() < 0 )
                    bChange = true;
            }
            if( bChange )
            {
                ++nLineIdx;
                if( nLineIdx >= nMaxLine )
                {
                    bChange = false;
                    rpLine = 0;
                }
            }
        }
    }
}

#ifndef PRODUCT

struct RowSpanCheck
{
    long nRowSpan;
    SwTwips nLeft;
    SwTwips nRight;
};

void SwTable::CheckConsistency() const
{
    if( !IsNewModel() )
        return;
    USHORT nLineCount = GetTabLines().Count();
    const SwTwips nTabSize = GetFrmFmt()->GetFrmSize().GetWidth();
    SwTwips nLineWidth = 0;
    std::list< RowSpanCheck > aRowSpanCells;
    std::list< RowSpanCheck >::iterator aIter = aRowSpanCells.end();
    for( USHORT nCurrLine = 0; nCurrLine < nLineCount; ++nCurrLine )
    {
        SwTwips nWidth = 0;
        SwTableLine* pLine = GetTabLines()[nCurrLine];
        ASSERT( pLine, "Missing Table Line" )
        USHORT nColCount = pLine->GetTabBoxes().Count();
        ASSERT( nColCount, "Empty Table Line" )
        for( USHORT nCurrCol = 0; nCurrCol < nColCount; ++nCurrCol )
        {
            SwTableBox* pBox = pLine->GetTabBoxes()[nCurrCol];
            ASSERT( pBox, "Missing Table Box" );
            SwTwips nNewWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth() + nWidth;
            long nRowSp = pBox->getRowSpan();
            if( nRowSp < 0 )
            {
                ASSERT( aIter != aRowSpanCells.end(), "Missing master box" )
#ifndef PRODUCT
                RowSpanCheck &rCheck = *aIter;
#endif
                ASSERT( aIter->nLeft == nWidth && aIter->nRight == nNewWidth,
                    "Wrong position/size of overlapped table box" );
                --(aIter->nRowSpan);
                ASSERT( aIter->nRowSpan == -nRowSp, "Wrong row span value" );
                if( nRowSp == -1 )
                {
                    std::list< RowSpanCheck >::iterator aEraseIter = aIter;
                    ++aIter;
                    aRowSpanCells.erase( aEraseIter );
                }
                else
                    ++aIter;
            }
            else if( nRowSp != 1 )
            {
                ASSERT( nRowSp, "Zero row span?!" );
                RowSpanCheck aEntry;
                aEntry.nLeft = nWidth;
                aEntry.nRight = nNewWidth;
                aEntry.nRowSpan = nRowSp;
                aRowSpanCells.insert( aIter, aEntry );
            }
            nWidth = nNewWidth;
        }
        if( !nCurrLine )
            nLineWidth = nWidth;
        ASSERT( nWidth == nLineWidth, "Different Line Widths" )
        ASSERT( nWidth == nTabSize, "Boxen der Line zu klein/gross" )
        ASSERT( nWidth >= 0 && nWidth <= USHRT_MAX, "Width out of range" )
        ASSERT( aIter == aRowSpanCells.end(), "Missing overlapped box" )
        aIter = aRowSpanCells.begin();
    }
    bool bEmpty = aRowSpanCells.empty();
    ASSERT( bEmpty, "Open row span detected" )
}

#endif


#ifdef FINDSTARTENDOFROWSPANCACHE
/*
 * A small optimization for FindStartEndOfRowSpan START
 *
 * NOTE: Results of some measurement revealed that this cache
 * does not improve performance!
 */

class SwFindRowSpanCache
{
private:

    struct SwFindRowSpanCacheObj
    {
        const SwTableBox* mpKeyBox;
        const SwTableBox* mpCacheBox;
        USHORT mnSteps;
        bool mbStart;

        SwFindRowSpanCacheObj( const SwTableBox& rKeyBox, const SwTableBox& rCacheBox, USHORT nSteps, bool bStart ) :
            mpKeyBox( &rKeyBox ), mpCacheBox( &rCacheBox ), mnSteps( nSteps ), mbStart( bStart ) {}
    };

    std::list< SwFindRowSpanCacheObj > aCache;
    bool mbUseCache;
    static SwFindRowSpanCache* mpFindRowSpanCache;
    SwFindRowSpanCache();

public:

    static SwFindRowSpanCache& getSwFindRowSpanCache();
    const SwTableBox* FindCachedStartEndOfRowSpan( const SwTableBox& rKeyBox, USHORT nSteps, bool bStart );
    void SetCachedStartEndOfRowSpan( const SwTableBox& rKeyBox, const SwTableBox& rCacheBox, USHORT nSteps, bool bStart );
    void SetUseCache( bool bNew );
};

SwFindRowSpanCache* SwFindRowSpanCache::mpFindRowSpanCache = 0;
SwFindRowSpanCache& SwFindRowSpanCache::getSwFindRowSpanCache()
{
    if ( !mpFindRowSpanCache ) mpFindRowSpanCache = new SwFindRowSpanCache;
    return *mpFindRowSpanCache;
}

SwFindRowSpanCache::SwFindRowSpanCache() : mbUseCache( false )
{
}

void SwFindRowSpanCache::SetUseCache( bool bNew )
{
    mbUseCache = bNew; aCache.clear();
}

const SwTableBox* SwFindRowSpanCache::FindCachedStartEndOfRowSpan( const SwTableBox& rKeyBox,
                                                                   USHORT nSteps,
                                                                   bool bStart )
{
    static nCallCount = 0;
    static nSuccessCount = 0;
    ++nCallCount;

    if ( !mbUseCache ) return 0;

    const SwTableBox* pRet = 0;

    std::list< SwFindRowSpanCacheObj >::const_iterator aIter;
    for ( aIter = aCache.begin(); aIter != aCache.end(); ++aIter )
    {
        if ( aIter->mpKeyBox == &rKeyBox &&
             aIter->mnSteps == nSteps &&
             aIter->mbStart == bStart )
        {
            pRet = aIter->mpCacheBox;
            ++nSuccessCount;
            break;
        }
    }

    return pRet;
}

const int FindBoxCacheSize = 2;

void SwFindRowSpanCache::SetCachedStartEndOfRowSpan( const SwTableBox& rKeyBox,
                                                     const SwTableBox& rCacheBox,
                                                     USHORT nSteps,
                                                     bool bStart )
{
    if ( !mbUseCache ) return;

    const SwFindRowSpanCacheObj aNew( rKeyBox, rCacheBox, nSteps, bStart );
    aCache.push_front( aNew );
    if ( aCache.size() > FindBoxCacheSize )
        aCache.pop_back();
}

/*
 * A small optimization for FindStartEndOfRowSpan END
 */

#endif

