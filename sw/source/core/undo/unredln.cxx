/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unredln.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 09:32:34 $
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


#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer die UndoIds
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _ROLBCK_HXX
#include <rolbck.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SORTOPT_HXX
#include <sortopt.hxx>
#endif

extern void lcl_JoinText( SwPaM& rPam, BOOL bJoinPrev );
extern void lcl_GetJoinFlags( SwPaM& rPam, BOOL& rJoinTxt, BOOL& rJoinPrev );

//------------------------------------------------------------------

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }


SwUndoRedline::SwUndoRedline( SwUndoId nUsrId, const SwPaM& rRange )
    : SwUndo( UNDO_REDLINE ), SwUndRng( rRange ),
    pRedlData( 0 ), pRedlSaveData( 0 ), nUserId( nUsrId ),
    bHiddenRedlines( FALSE )
{
    // Redline beachten
    SwDoc& rDoc = *rRange.GetDoc();
    if( rDoc.IsRedlineOn() )
    {
        switch( nUserId )
        {
        case UNDO_DELETE:
        case UNDO_REPLACE:
            pRedlData = new SwRedlineData( nsRedlineType_t::REDLINE_DELETE, rDoc.GetRedlineAuthor() );
            break;
        default:
            ;
        }
        SetRedlineMode( rDoc.GetRedlineMode() );
    }

    ULONG nEndExtra = rDoc.GetNodes().GetEndOfExtras().GetIndex();

    pRedlSaveData = new SwRedlineSaveDatas;
    if( !FillSaveData( rRange, *pRedlSaveData, FALSE,
                        UNDO_REJECT_REDLINE != nUserId ))
        delete pRedlSaveData, pRedlSaveData = 0;
    else
    {
        bHiddenRedlines = HasHiddenRedlines( *pRedlSaveData );
        if( bHiddenRedlines ) 			// dann muessen die NodeIndizies
        {   							// vom SwUndRng korrigiert werden
            nEndExtra -= rDoc.GetNodes().GetEndOfExtras().GetIndex();
            nSttNode -= nEndExtra;
            nEndNode -= nEndExtra;
        }
    }
}

SwUndoRedline::~SwUndoRedline()
{
    delete pRedlData;
    delete pRedlSaveData;
}

void SwUndoRedline::Undo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    SetPaM( *rIter.pAktPam );

// RedlineMode setzen?
    _Undo( rIter );

    if( pRedlSaveData )
    {
        ULONG nEndExtra = pDoc->GetNodes().GetEndOfExtras().GetIndex();
        SetSaveData( *pDoc, *pRedlSaveData );
        if( bHiddenRedlines )
        {
            pRedlSaveData->DeleteAndDestroy( 0, pRedlSaveData->Count() );

            nEndExtra = pDoc->GetNodes().GetEndOfExtras().GetIndex() - nEndExtra;
            nSttNode += nEndExtra;
            nEndNode += nEndExtra;
        }
        SetPaM( *rIter.pAktPam, TRUE );
    }
}


void SwUndoRedline::Redo( SwUndoIter& rIter )
{
    SwDoc* pDoc = &rIter.GetDoc();
    RedlineMode_t eOld = pDoc->GetRedlineMode();
    pDoc->SetRedlineMode_intern((RedlineMode_t)(( eOld & ~nsRedlineMode_t::REDLINE_IGNORE) | nsRedlineMode_t::REDLINE_ON ));

    SetPaM( *rIter.pAktPam );
    if( pRedlSaveData && bHiddenRedlines )
    {
        ULONG nEndExtra = pDoc->GetNodes().GetEndOfExtras().GetIndex();
        FillSaveData( *rIter.pAktPam, *pRedlSaveData, FALSE,
                        UNDO_REJECT_REDLINE != nUserId );

        nEndExtra -= pDoc->GetNodes().GetEndOfExtras().GetIndex();
        nSttNode -= nEndExtra;
        nEndNode -= nEndExtra;
    }
    _Redo( rIter );

    SetPaM( *rIter.pAktPam, TRUE );
    pDoc->SetRedlineMode_intern( eOld );
}

// default ist leer
void SwUndoRedline::_Undo( SwUndoIter& )
{
}

// default ist Redlines entfernen
void SwUndoRedline::_Redo( SwUndoIter& rIter )
{
    rIter.GetDoc().DeleteRedline( *rIter.pAktPam, true, USHRT_MAX );
}


/*  */

SwUndoRedlineDelete::SwUndoRedlineDelete( const SwPaM& rRange, SwUndoId nUsrId )
    : SwUndoRedline( nUsrId = (nUsrId ? nUsrId : UNDO_DELETE), rRange ),
    bCanGroup( FALSE ), bIsDelim( FALSE ), bIsBackspace( FALSE )
{
    const SwTxtNode* pTNd;
    if( UNDO_DELETE == nUserId &&
        nSttNode == nEndNode && nSttCntnt + 1 == nEndCntnt &&
        0 != (pTNd = rRange.GetNode()->GetTxtNode()) )
    {
        sal_Unicode cCh = pTNd->GetTxt().GetChar( nSttCntnt );
        if( CH_TXTATR_BREAKWORD != cCh && CH_TXTATR_INWORD != cCh )
        {
            bCanGroup = TRUE;
            bIsDelim = !GetAppCharClass().isLetterNumeric( pTNd->GetTxt(),
                                                            nSttCntnt );
            bIsBackspace = nSttCntnt == rRange.GetPoint()->nContent.GetIndex();
        }
    }

    bCacheComment = false;
}

void SwUndoRedlineDelete::_Undo( SwUndoIter& rIter )
{
    rIter.GetDoc().DeleteRedline( *rIter.pAktPam, true, USHRT_MAX );
}

void SwUndoRedlineDelete::_Redo( SwUndoIter& rIter )
{
    if( *rIter.pAktPam->GetPoint() != *rIter.pAktPam->GetMark() )
        rIter.GetDoc().AppendRedline( new SwRedline( *pRedlData, *rIter.pAktPam ), FALSE );
}

BOOL SwUndoRedlineDelete::CanGrouping( const SwUndoRedlineDelete& rNext )
{
    BOOL bRet = FALSE;
    if( UNDO_DELETE == nUserId && nUserId == rNext.nUserId &&
        bCanGroup == rNext.bCanGroup &&
        bIsDelim == rNext.bIsDelim &&
        bIsBackspace == rNext.bIsBackspace &&
        nSttNode == nEndNode &&
        rNext.nSttNode == nSttNode &&
        rNext.nEndNode == nEndNode )
    {
        int bIsEnd = 0;
        if( rNext.nSttCntnt == nEndCntnt )
            bIsEnd = 1;
        else if( rNext.nEndCntnt == nSttCntnt )
            bIsEnd = -1;

        if( bIsEnd &&
            (( !pRedlSaveData && !rNext.pRedlSaveData ) ||
             ( pRedlSaveData && rNext.pRedlSaveData &&
                SwUndo::CanRedlineGroup( *pRedlSaveData,
                            *rNext.pRedlSaveData, 1 != bIsEnd )
             )))
        {
            if( 1 == bIsEnd )
                nEndCntnt = rNext.nEndCntnt;
            else
                nSttCntnt = rNext.nSttCntnt;
            bRet = TRUE;
        }
    }
    return bRet;
}

/*  */

SwUndoRedlineSort::SwUndoRedlineSort( const SwPaM& rRange,
                                    const SwSortOptions& rOpt )
    : SwUndoRedline( UNDO_SORT_TXT, rRange ),
    pOpt( new SwSortOptions( rOpt ) ),
    nSaveEndNode( nEndNode ), nOffset( 0 ), nSaveEndCntnt( nEndCntnt )
{
}

SwUndoRedlineSort::~SwUndoRedlineSort()
{
    delete pOpt;
}

void SwUndoRedlineSort::_Undo( SwUndoIter& rIter )
{
    // im rIter.pAktPam ist der sortiete Bereich,
    // im aSaveRange steht der kopierte, sprich der originale.
    SwDoc& rDoc = rIter.GetDoc();

    if( 0 == ( nsRedlineMode_t::REDLINE_SHOW_DELETE & rDoc.GetRedlineMode()) )
    {
        // die beiden Redline Objecte suchen und diese dann anzeigen lassen,
        // damit die Nodes wieder uebereinstimmen!
        // das Geloeschte ist versteckt, also suche das INSERT
        // Redline Object. Dahinter steht das Geloeschte
        USHORT nFnd = rDoc.GetRedlinePos(
                            *rDoc.GetNodes()[ nSttNode + nOffset + 1 ],
                            nsRedlineType_t::REDLINE_INSERT );
        ASSERT( USHRT_MAX != nFnd && nFnd+1 < rDoc.GetRedlineTbl().Count(),
                    "kein Insert Object gefunden" );
        ++nFnd;
        rDoc.GetRedlineTbl()[nFnd]->Show();
        SetPaM( *rIter.pAktPam );
    }

    {
        SwPaM aTmp( *rIter.pAktPam->GetMark() );
        aTmp.SetMark();
        aTmp.GetPoint()->nNode = nSaveEndNode;
        aTmp.GetPoint()->nContent.Assign( aTmp.GetCntntNode(), nSaveEndCntnt );
        rDoc.DeleteRedline( aTmp, true, USHRT_MAX );
    }

    rDoc.DelFullPara( *rIter.pAktPam );
    SetPaM( *rIter.pAktPam );
}

void SwUndoRedlineSort::_Redo( SwUndoIter& rIter )
{
    SwPaM& rPam = *rIter.pAktPam;
    rIter.GetDoc().SortText( rPam, *pOpt );
    SetPaM( rPam );
    rPam.GetPoint()->nNode = nSaveEndNode;
    rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), nSaveEndCntnt );
}

void SwUndoRedlineSort::Repeat( SwUndoIter& rIter )
{
    rIter.GetDoc().SortText( *rIter.pAktPam, *pOpt );
}

void SwUndoRedlineSort::SetSaveRange( const SwPaM& rRange )
{
    const SwPosition& rPos = *rRange.End();
    nSaveEndNode = rPos.nNode.GetIndex();
    nSaveEndCntnt = rPos.nContent.GetIndex();
}

void SwUndoRedlineSort::SetOffset( const SwNodeIndex& rIdx )
{
    nOffset = rIdx.GetIndex() - nSttNode;
}

/*  */

SwUndoAcceptRedline::SwUndoAcceptRedline( const SwPaM& rRange )
    : SwUndoRedline( UNDO_ACCEPT_REDLINE, rRange )
{
}

void SwUndoAcceptRedline::_Redo( SwUndoIter& rIter )
{
    rIter.GetDoc().AcceptRedline( *rIter.pAktPam, false );
}

void SwUndoAcceptRedline::Repeat( SwUndoIter& rIter )
{
    rIter.GetDoc().AcceptRedline( *rIter.pAktPam, true );
}

SwUndoRejectRedline::SwUndoRejectRedline( const SwPaM& rRange )
    : SwUndoRedline( UNDO_REJECT_REDLINE, rRange )
{
}

void SwUndoRejectRedline::_Redo( SwUndoIter& rIter )
{
    rIter.GetDoc().RejectRedline( *rIter.pAktPam, false );
}

void SwUndoRejectRedline::Repeat( SwUndoIter& rIter )
{
    rIter.GetDoc().RejectRedline( *rIter.pAktPam, true );
}

/*  */

SwUndoCompDoc::SwUndoCompDoc( const SwPaM& rRg, BOOL bIns )
    : SwUndo( UNDO_COMPAREDOC ), SwUndRng( rRg ), pRedlData( 0 ),
    pUnDel( 0 ), pUnDel2( 0 ), pRedlSaveData( 0 ), bInsert( bIns )
{
    SwDoc* pDoc = (SwDoc*)rRg.GetDoc();
    if( pDoc->IsRedlineOn() )
    {
        RedlineType_t eTyp = bInsert ? nsRedlineType_t::REDLINE_INSERT : nsRedlineType_t::REDLINE_DELETE;
        pRedlData = new SwRedlineData( eTyp, pDoc->GetRedlineAuthor() );
        SetRedlineMode( pDoc->GetRedlineMode() );
    }
}

SwUndoCompDoc::SwUndoCompDoc( const SwRedline& rRedl )
    : SwUndo( UNDO_COMPAREDOC ), SwUndRng( rRedl ), pRedlData( 0 ),
    pUnDel( 0 ), pUnDel2( 0 ), pRedlSaveData( 0 ),
    // fuers MergeDoc wird aber der jeweils umgekehrte Zweig benoetigt!
    bInsert( nsRedlineType_t::REDLINE_DELETE == rRedl.GetType() )
{
    SwDoc* pDoc = (SwDoc*)rRedl.GetDoc();
    if( pDoc->IsRedlineOn() )
    {
        pRedlData = new SwRedlineData( rRedl.GetRedlineData() );
        SetRedlineMode( pDoc->GetRedlineMode() );
    }

    pRedlSaveData = new SwRedlineSaveDatas;
    if( !FillSaveData( rRedl, *pRedlSaveData, FALSE, TRUE ))
        delete pRedlSaveData, pRedlSaveData = 0;
}

SwUndoCompDoc::~SwUndoCompDoc()
{
    delete pRedlData;
    delete pUnDel;
    delete pUnDel2;
    delete pRedlSaveData;
}

void SwUndoCompDoc::Undo( SwUndoIter& rIter )
{
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    SetPaM( *pPam );

    if( !bInsert )
    {
        // die Redlines loeschen
        RedlineMode_t eOld = pDoc->GetRedlineMode();
        pDoc->SetRedlineMode_intern((RedlineMode_t)(( eOld & ~nsRedlineMode_t::REDLINE_IGNORE) | nsRedlineMode_t::REDLINE_ON));

        pDoc->DeleteRedline( *pPam, true, USHRT_MAX );

        pDoc->SetRedlineMode_intern( eOld );

        //per definition Point is end (in SwUndRng!)
        SwCntntNode* pCSttNd = pPam->GetCntntNode( FALSE );
        SwCntntNode* pCEndNd = pPam->GetCntntNode( TRUE );

        // if start- and end-content is zero, then the doc-compare moves
        // complete nodes into the current doc. And then the selection
        // must be from end to start, so the delete join into the right
        // direction.
        if( !nSttCntnt && !nEndCntnt )
            pPam->Exchange();

        BOOL bJoinTxt, bJoinPrev;
        ::lcl_GetJoinFlags( *pPam, bJoinTxt, bJoinPrev );

        pUnDel = new SwUndoDelete( *pPam, FALSE );

        if( bJoinTxt )
            ::lcl_JoinText( *pPam, bJoinPrev );

        if( pCSttNd && !pCEndNd)
        {
            // #112139# Do not step behind the end of content.
            SwNode * pTmp = pPam->GetNode(TRUE);
            if (pTmp)
            {
                SwNode * pEnd = pDoc->GetNodes().DocumentSectionEndNode(pTmp);

                if (pTmp != pEnd)
                {
                    pPam->SetMark();
                    pPam->GetPoint()->nNode++;
                    pPam->GetBound( TRUE ).nContent.Assign( 0, 0 );
                    pPam->GetBound( FALSE ).nContent.Assign( 0, 0 );
                    pUnDel2 = new SwUndoDelete( *pPam, TRUE );
                }
            }
        }
        pPam->DeleteMark();
    }
    else
    {
        if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        {
            pDoc->DeleteRedline( *pPam, true, USHRT_MAX );

            if( pRedlSaveData )
                SetSaveData( *pDoc, *pRedlSaveData );
        }
        SetPaM( rIter, TRUE );
    }
}

void SwUndoCompDoc::Redo( SwUndoIter& rIter )
{
    // setze noch den Cursor auf den Redo-Bereich
    SwPaM* pPam = rIter.pAktPam;
    SwDoc* pDoc = pPam->GetDoc();

    rIter.pLastUndoObj = 0;

    if( bInsert )
    {
        SetPaM( *pPam );

        if( pRedlData && IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        {
            SwRedline* pTmp = new SwRedline( *pRedlData, *pPam );
            ((SwRedlineTbl&)pDoc->GetRedlineTbl()).Insert( pTmp );
            pTmp->InvalidateRange();

/*
            SwRedlineMode eOld = pDoc->GetRedlineMode();
            pDoc->SetRedlineMode_intern( eOld & ~REDLINE_IGNORE );
            pDoc->AppendRedline( new SwRedline( *pRedlData, *pPam ));
            pDoc->SetRedlineMode_intern( eOld );
*/
        }
        else if( !( nsRedlineMode_t::REDLINE_IGNORE & GetRedlineMode() ) &&
                pDoc->GetRedlineTbl().Count() )
            pDoc->SplitRedline( *pPam );
    }
    else
    {
//		SwRedlineMode eOld = pDoc->GetRedlineMode();
//		pDoc->SetRedlineMode_intern( ( eOld & ~REDLINE_IGNORE) | REDLINE_ON );

        if( pUnDel2 )
        {
            pUnDel2->Undo( rIter );
            delete pUnDel2, pUnDel2 = 0;
        }
        pUnDel->Undo( rIter );
        delete pUnDel, pUnDel = 0;

        SetPaM( *pPam );

        SwRedline* pTmp = new SwRedline( *pRedlData, *pPam );
        ((SwRedlineTbl&)pDoc->GetRedlineTbl()).Insert( pTmp );
        if (pTmp) // #i19649#
            pTmp->InvalidateRange();

//		pDoc->SetRedlineMode_intern( eOld );
    }

    SetPaM( rIter, TRUE );
}


