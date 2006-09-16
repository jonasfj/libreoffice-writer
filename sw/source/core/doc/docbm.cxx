/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docbm.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 20:50:01 $
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


#include <tools/pstm.hxx>
#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif

#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _BOOKMRK_HXX
#include <bookmrk.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _MVSAVE_HXX
#include <mvsave.hxx>
#endif
#ifndef _SWSERV_HXX
#include <swserv.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif
// OD 2004-05-24 #i28701#
#ifndef _SORTEDOBJS_HXX
#include <sortedobjs.hxx>
#endif
#ifndef _NDTXT_HXX
#include "ndtxt.hxx" // for lcl_FixPosition
#endif


SV_IMPL_OP_PTRARR_SORT(SwBookmarks, SwBookmarkPtr)

#define PCURCRSR (_pCurrCrsr)
#define FOREACHPAM_START(pSttCrsr) \
    {\
        SwPaM *_pStartCrsr = pSttCrsr, *_pCurrCrsr = pSttCrsr; \
        do {

#define FOREACHPAM_END() \
        } while( (_pCurrCrsr=(SwPaM *)_pCurrCrsr->GetNext()) != _pStartCrsr ); \
    }
#define PCURSH ((SwCrsrShell*)_pStartShell)
#define FOREACHSHELL_START( pEShell ) \
    {\
        register ViewShell *_pStartShell = pEShell; \
        do { \
            if( _pStartShell->IsA( TYPE( SwCrsrShell )) ) \
            {

#define FOREACHSHELL_END( pEShell ) \
            } \
        } while((_pStartShell=(ViewShell*)_pStartShell->GetNext())!= pEShell ); \
    }


void lcl_FixPosition( SwPosition& rPos )
{
    // make sure the position has 1) the proper node, and 2) a proper index
    SwTxtNode* pTxtNode = rPos.nNode.GetNode().GetTxtNode();

    if( rPos.nContent.GetIndex() > ( pTxtNode == NULL ? 0 : pTxtNode->Len() ) )
    {
        DBG_ERROR( "illegal position" );
        xub_StrLen nLen = rPos.nContent.GetIndex();
        if( pTxtNode == NULL )
            nLen = 0;
        else if( nLen >= pTxtNode->Len() )
            nLen = pTxtNode->Len();
        rPos.nContent.Assign( pTxtNode, nLen );
    }
}

/** IDocumentBookmarkAccess ssc
*/
const SwBookmarks& SwDoc::getBookmarks() const
{
    return *pBookmarkTbl;
}

SwBookmark* SwDoc::makeBookmark( /*[in]*/const SwPaM& rPaM, /*[in]*/const KeyCode& rCode,
                                 /*[in]*/ const String& rName, /*[in]*/const String& rShortName,
                                 /*[in]*/IDocumentBookmarkAccess::BookmarkType eMark )
{
    SwBookmark *pBM;
    if( MARK == eMark )
        pBM = new SwMark( *rPaM.GetPoint(), rCode, rName, rShortName );
    else if( BOOKMARK == eMark || HIDDEN_BOOKMARK == eMark)
    {
        pBM = new SwBookmark(*rPaM.GetPoint(), rCode, rName, rShortName);
        if( rPaM.HasMark() )
            pBM->pPos2 = new SwPosition( *rPaM.GetMark() );
    }
    else
    {
        pBM = new SwUNOMark(*rPaM.GetPoint(), rCode, rName, rShortName);
        if( rPaM.HasMark() )
            pBM->pPos2 = new SwPosition( *rPaM.GetMark() );
    }

    // fix bookmark positions if they are invalid
    lcl_FixPosition( *pBM->pPos1 );
    if( pBM->pPos2 != NULL )
        lcl_FixPosition( *pBM->pPos2 );

    if( !pBookmarkTbl->Insert( pBM ) )
        delete pBM, pBM = 0;
    else
    {
        if( BOOKMARK == eMark && DoesUndo() )
        {
            ClearRedo();
            AppendUndo( new SwUndoInsBookmark( *pBM ));
        }
        switch( eMark )
        {
            case UNO_BOOKMARK:
            case HIDDEN_BOOKMARK:
            break;
            default:
                SetModified();
        }
    }
    return pBM;
}

void SwDoc::deleteBookmark( /*[in]*/sal_uInt16 nPos )
{
    SwBookmark *pBM = (*pBookmarkTbl)[nPos];
    if( DoesUndo() && !pBM->IsUNOMark())
    {
        ClearRedo();
        AppendUndo( new SwUndoDelBookmark( *pBM ));
    }

    // #108964# UNO bookmark don't contribute to the document state,
    // and hence changing them shouldn't set the document modified
    if( !pBM->IsUNOMark() )
        SetModified();

    pBookmarkTbl->Remove(nPos);

    SwServerObject* pServObj = pBM->GetObject();
    if( pServObj )			// dann aus der Liste entfernen
        GetLinkManager().RemoveServer( pServObj );

    delete pBM;
}

void SwDoc::deleteBookmark( /*[in]*/const String& rName )
{
    USHORT nFnd = findBookmark( rName );
    if( USHRT_MAX != nFnd )
        deleteBookmark( nFnd );
}

sal_uInt16 SwDoc::findBookmark( /*[in]*/const String& rName )
{
    ASSERT( rName.Len(), "wo ist der Name?" );
    for( USHORT n = pBookmarkTbl->Count(); n ; )
        if( rName.Equals( (*pBookmarkTbl)[ --n ]->GetName() ) )
            return n;
    return USHRT_MAX;
}

// Zur Vereinfachung gibt es auch den direkten Zugriff
// auf die "echten" Bookmarks

sal_uInt16 SwDoc::getBookmarkCount( /*[in]*/bool bBkmrk) const
{
    USHORT nRet = pBookmarkTbl->Count();
    if(bBkmrk)
    {
        for( USHORT i = nRet; i; --i )
        {
            if(!(*pBookmarkTbl)[i - 1]->IsBookMark())
                nRet--;
        }
    }
    return nRet;
}


SwBookmark& SwDoc::getBookmark( /*[in]*/sal_uInt16 nPos,  /*[in]*/bool bBkmrk)
{
    if( bBkmrk )
    {
        USHORT i = 0;
        do {
            if(!(*pBookmarkTbl)[i]->IsBookMark())
                nPos++;

            i++;
        }
        while( i < nPos || !(*pBookmarkTbl)[nPos]->IsBookMark() );
    }
    return *(*pBookmarkTbl)[nPos];
}

void SwDoc::makeUniqueBookmarkName( String& rNm )
{
    ASSERT( rNm.Len(), "es sollte ein Name vorgegeben werden!" );

    // wir erzeugen uns eine temp. Bookmark
    String sTmp;
    USHORT nCnt = 0, n;
    USHORT nBookCnt = pBookmarkTbl->Count();
    do {
        sTmp = rNm;
        // #i35726# try without number extension first
        if(nCnt)
            sTmp += String::CreateFromInt32( nCnt );
        nCnt++;
        for( n = 0; n < nBookCnt; ++n )
            if( (*pBookmarkTbl)[ n ]->GetName().Equals( sTmp ))
                break;
    } while( n < nBookCnt );
    rNm = sTmp;
}

/*  */

SaveBookmark::SaveBookmark( int eType, const SwBookmark& rBkmk,
                            const SwNodeIndex & rMvPos,
                            const SwIndex* pIdx )
    : aName( rBkmk.GetName() ), aShortName( rBkmk.GetShortName() ),
    aCode( rBkmk.GetKeyCode() ), eBkmkType( (SaveBookmarkType)eType ),
    eOrigBkmType(rBkmk.GetType())
{
    nNode1 = rBkmk.GetPos().nNode.GetIndex();
    nCntnt1 = rBkmk.GetPos().nContent.GetIndex();

    if( BKMK_POS & eBkmkType )
    {
        nNode1 -= rMvPos.GetIndex();
        if( pIdx && !nNode1 )
            nCntnt1 -= pIdx->GetIndex();
    }

    if( rBkmk.GetOtherPos() )
    {
        nNode2 = rBkmk.GetOtherPos()->nNode.GetIndex();
        nCntnt2 = rBkmk.GetOtherPos()->nContent.GetIndex();

        if( BKMK_POS_OTHER & eBkmkType )
        {
            nNode2 -= rMvPos.GetIndex();
            if( pIdx && !nNode2 )
                nCntnt2 -= pIdx->GetIndex();
        }
    }
    else
        nNode2 = ULONG_MAX, nCntnt2 = STRING_NOTFOUND;
}

void SaveBookmark::SetInDoc( SwDoc* pDoc, const SwNodeIndex& rNewPos,
                            const SwIndex* pIdx )
{
    SwPaM aPam( rNewPos.GetNode() );
    if( pIdx )
        aPam.GetPoint()->nContent = *pIdx;

    if( ULONG_MAX != nNode2 )
    {
        aPam.SetMark();

        if( BKMK_POS_OTHER & eBkmkType )
        {
            aPam.GetMark()->nNode += nNode2;
            if( pIdx && !nNode2 )
                aPam.GetMark()->nContent += nCntnt2;
            else
                aPam.GetMark()->nContent.Assign( aPam.GetCntntNode( FALSE ),
                                                        nCntnt2 );
        }
        else
        {
            aPam.GetMark()->nNode = nNode2;
            aPam.GetMark()->nContent.Assign( aPam.GetCntntNode( FALSE ),
                                                    nCntnt2 );
        }
    }

    if( BKMK_POS & eBkmkType )
    {
        aPam.GetPoint()->nNode += nNode1;

        if( pIdx && !nNode1 )
            aPam.GetPoint()->nContent += nCntnt1;
        else
            aPam.GetPoint()->nContent.Assign( aPam.GetCntntNode(), nCntnt1 );
    }
    else
    {
        aPam.GetPoint()->nNode = nNode1;
        aPam.GetPoint()->nContent.Assign( aPam.GetCntntNode(), nCntnt1 );
    }

    if( !aPam.HasMark() ||
        CheckNodesRange( aPam.GetPoint()->nNode, aPam.GetMark()->nNode, TRUE ))
        pDoc->makeBookmark( aPam, aCode, aName, aShortName, eOrigBkmType );
}


inline int GreaterThan( const SwPosition& rPos, const SwNodeIndex& rNdIdx,
                        const SwIndex* pIdx )
{
    return pIdx ? ( rPos.nNode > rNdIdx || ( rPos.nNode == rNdIdx &&
                                        rPos.nContent >= pIdx->GetIndex() ))
                : rPos.nNode >= rNdIdx;
}
inline int Lower( const SwPosition& rPos, const SwNodeIndex& rNdIdx,
                        const SwIndex* pIdx )
{
    return rPos.nNode < rNdIdx || ( pIdx && rPos.nNode == rNdIdx &&
                                        rPos.nContent < pIdx->GetIndex() );
}
inline int Greater( const SwPosition& rPos, const SwNodeIndex& rNdIdx,
                        const SwIndex* pIdx )
{
    return rPos.nNode > rNdIdx || ( pIdx && rPos.nNode == rNdIdx &&
                                        rPos.nContent > pIdx->GetIndex() );
}

void _DelBookmarks( const SwNodeIndex& rStt, const SwNodeIndex& rEnd,
                    SaveBookmarks* pSaveBkmk,
                    const SwIndex* pSttIdx, const SwIndex* pEndIdx )
{
    // kein gueltiger Bereich ??
    if( rStt.GetIndex() > rEnd.GetIndex() || ( rStt == rEnd &&
        (!pSttIdx || pSttIdx->GetIndex() >= pEndIdx->GetIndex())) )
        return;

    // kopiere alle Bookmarks, die im Move Bereich stehen in ein
    // Array, das alle Angaben auf die Position als Offset speichert.
    // Die neue Zuordung erfolgt nach dem Moven.
    SwDoc* pDoc = rStt.GetNode().GetDoc();
    const SwBookmarks& rBkmks = pDoc->getBookmarks();
    USHORT nCnt;

    for( nCnt = 0; nCnt < rBkmks.Count(); ++nCnt )
    {
        // liegt auf der Position ??
        int eType = BKMK_POS_NONE;
        SwBookmark* pBkmk = rBkmks[ nCnt ];
        //simple marks should not be moved
        if(pBkmk->IsMark())
            continue;
        if( GreaterThan( pBkmk->GetPos(), rStt, pSttIdx ) &&
            Lower( pBkmk->GetPos(), rEnd, pEndIdx ))
            eType = BKMK_POS;
        if( pBkmk->GetOtherPos() &&
            GreaterThan( *pBkmk->GetOtherPos(), rStt, pSttIdx ) &&
            Lower( *pBkmk->GetOtherPos(), rEnd, pEndIdx ))
            eType |= BKMK_POS_OTHER;

        if( BKMK_POS_NONE == eType )		// auf zum naechsten
            continue;

        if( pSaveBkmk )
        {
                // Besonderheit: komplett eingeschlossen? dann mitnehmen
            if( pEndIdx && (BKMK_POS_OTHER | BKMK_POS) != eType &&
                ( ( BKMK_POS_OTHER & eType &&
                    pBkmk->GetPos().nNode == rEnd &&
                    pBkmk->GetPos().nContent == *pEndIdx ) ||
                ( BKMK_POS & eType && pBkmk->GetOtherPos() &&
                    pBkmk->GetOtherPos()->nNode == rEnd &&
                    pBkmk->GetOtherPos()->nContent == *pEndIdx ) ) )
                    eType = BKMK_POS_OTHER | BKMK_POS;

            SaveBookmark * pSBkmk = new SaveBookmark( eType, *pBkmk, rStt, pSttIdx );
            pSaveBkmk->C40_INSERT( SaveBookmark, pSBkmk, pSaveBkmk->Count() );
            pDoc->deleteBookmark( nCnt-- );
        }
        else if( (BKMK_POS_OTHER | BKMK_POS ) == eType ||
                ( BKMK_POS == eType && !pBkmk->GetOtherPos() ) )
            pDoc->deleteBookmark( nCnt-- );
        else
        {
            SwPosition* pPos = (SwPosition*)(BKMK_POS & eType
                                    ? &pBkmk->GetPos()
                                    : pBkmk->GetOtherPos());
            pPos->nNode = rEnd;
            if( pEndIdx )
                pPos->nContent = *pEndIdx;
            else
            {
                SwCntntNode* pCNd = pPos->nNode.GetNode().GetCntntNode();
                BOOL bStt = TRUE;
                if( !pCNd && 0 == ( pCNd = pDoc->GetNodes().GoNext( &pPos->nNode )) )
                {
                    bStt = FALSE;
                    pPos->nNode = rStt;
                    if( 0 == ( pCNd = pDoc->GetNodes().GoPrevious( &pPos->nNode )) )
                    {
                        pPos->nNode = BKMK_POS == eType
                                        ? pBkmk->GetOtherPos()->nNode
                                        : pBkmk->GetPos().nNode;
                        pCNd = pPos->nNode.GetNode().GetCntntNode();
                    }
                }
                xub_StrLen nTmp = bStt ? 0 : pCNd->Len();
                pPos->nContent.Assign( pCNd, nTmp );
            }

            // keine ungueltigen Selektionen zulassen!
            if( pBkmk->GetOtherPos() &&
                pBkmk->GetOtherPos()->nNode.GetNode().FindTableBoxStartNode() !=
                pBkmk->GetPos().nNode.GetNode().FindTableBoxStartNode() )
            {
                SwPaM aPam( pPos == pBkmk->GetOtherPos()
                                ? pBkmk->GetPos() : *pBkmk->GetOtherPos()  );
                String sNm( pBkmk->GetName() ), sShortNm( pBkmk->GetShortName() );
                KeyCode aKCode( pBkmk->GetKeyCode() );

                pDoc->deleteBookmark( nCnt-- );
                pDoc->makeBookmark( aPam, aKCode, sNm, sShortNm, IDocumentBookmarkAccess::BOOKMARK );
            }
        }
    }

    // kopiere alle Redlines, die im Move Bereich stehen in ein
    // Array, das alle Angaben auf die Position als Offset speichert.
    // Die neue Zuordung erfolgt nach dem Moven.
    SwRedlineTbl& rTbl = (SwRedlineTbl&)pDoc->GetRedlineTbl();
    for( nCnt = 0; nCnt < rTbl.Count(); ++nCnt )
    {
        // liegt auf der Position ??
        SwRedline* pRedl = rTbl[ nCnt ];

        SwPosition *pRStt = &pRedl->GetBound(TRUE),
                   *pREnd = &pRedl->GetBound(FALSE);
        if( *pRStt > *pREnd )
        {
            SwPosition *pTmp = pRStt; pRStt = pREnd, pREnd = pTmp;
        }

        if( Greater( *pRStt, rStt, pSttIdx ) && Lower( *pRStt, rEnd, pEndIdx ))
        {
            pRStt->nNode = rEnd;
            if( pEndIdx )
                pRStt->nContent = *pEndIdx;
            else
            {
                BOOL bStt = TRUE;
                SwCntntNode* pCNd = pRStt->nNode.GetNode().GetCntntNode();
                if( !pCNd && 0 == ( pCNd = pDoc->GetNodes().GoNext( &pRStt->nNode )) )
                {
                    bStt = FALSE;
                    pRStt->nNode = rStt;
                    if( 0 == ( pCNd = pDoc->GetNodes().GoPrevious( &pRStt->nNode )) )
                    {
                        pRStt->nNode = pREnd->nNode;
                        pCNd = pRStt->nNode.GetNode().GetCntntNode();
                    }
                }
                xub_StrLen nTmp = bStt ? 0 : pCNd->Len();
                pRStt->nContent.Assign( pCNd, nTmp );
            }
        }
        if( Greater( *pREnd, rStt, pSttIdx ) && Lower( *pREnd, rEnd, pEndIdx ))
        {
            pREnd->nNode = rStt;
            if( pSttIdx )
                pREnd->nContent = *pSttIdx;
            else
            {
                BOOL bStt = FALSE;
                SwCntntNode* pCNd = pREnd->nNode.GetNode().GetCntntNode();
                if( !pCNd && 0 == ( pCNd = pDoc->GetNodes().GoPrevious( &pREnd->nNode )) )
                {
                    bStt = TRUE;
                    pREnd->nNode = rEnd;
                    if( 0 == ( pCNd = pDoc->GetNodes().GoNext( &pREnd->nNode )) )
                    {
                        pREnd->nNode = pRStt->nNode;
                        pCNd = pREnd->nNode.GetNode().GetCntntNode();
                    }
                }
                xub_StrLen nTmp = bStt ? 0 : pCNd->Len();
                pREnd->nContent.Assign( pCNd, nTmp );
            }
        }
    }
}

/*  */


// Aufbau vom Array: 2 longs,
//	1. Long enthaelt Type und Position im DocArray,
//	2. die ContentPosition
//
//	CntntType --
//			0x8000 = Bookmark Pos1
//			0x8001 = Bookmark Pos2
//			0x2000 = Absatzgebundener Rahmen
//			0x2001 = Auto-Absatzgebundener Rahmen, der umgehaengt werden soll
//			0x1000 = Redline Mark
//			0x1001 = Redline Point
//			0x0800 = Crsr aus der CrsrShell Mark
//			0x0801 = Crsr aus der CrsrShell Point
//			0x0400 = UnoCrsr Mark
//			0x0401 = UnoCrsr Point
//

class _SwSaveTypeCountContent
{
    union {
        struct { USHORT nType, nCount; } TC;
        ULONG nTypeCount;
        } TYPECOUNT;
    xub_StrLen nContent;

public:
    _SwSaveTypeCountContent() { TYPECOUNT.nTypeCount = 0; nContent = 0; }
    _SwSaveTypeCountContent( USHORT nType )
        {
            SetTypeAndCount( nType, 0 );
            nContent = 0;
        }
    _SwSaveTypeCountContent( const SvULongs& rArr, USHORT& rPos )
        {
            TYPECOUNT.nTypeCount = rArr[ rPos++ ];
            nContent = rArr[ rPos++ ];
        }
    void Add( SvULongs& rArr )
    {
        rArr.Insert( TYPECOUNT.nTypeCount, rArr.Count() );
        rArr.Insert( nContent, rArr.Count() );
    }

    void SetType( USHORT n )		{ TYPECOUNT.TC.nType = n; }
    USHORT GetType() const 			{ return TYPECOUNT.TC.nType; }
    void IncType() 	 				{ ++TYPECOUNT.TC.nType; }
    void DecType() 	 				{ --TYPECOUNT.TC.nType; }

    void SetCount( USHORT n ) 		{ TYPECOUNT.TC.nCount = n; }
    USHORT GetCount() const 		{ return TYPECOUNT.TC.nCount; }
    USHORT IncCount()  				{ return ++TYPECOUNT.TC.nCount; }
    USHORT DecCount()  				{ return --TYPECOUNT.TC.nCount; }

    void SetTypeAndCount( USHORT nT, USHORT nC )
        { TYPECOUNT.TC.nCount = nC; TYPECOUNT.TC.nType = nT; }

    void SetContent( xub_StrLen n )		{ nContent = n; }
    xub_StrLen GetContent() const		{ return nContent; }
};


void _ChkPaM( SvULongs& rSaveArr, ULONG nNode, xub_StrLen nCntnt,
                const SwPaM& rPam, _SwSaveTypeCountContent& rSave,
                BOOL bChkSelDirection )
{
    // SelektionsRichtung beachten
    BOOL bBound1IsStart = !bChkSelDirection ? TRUE :
                        ( *rPam.GetPoint() < *rPam.GetMark()
                            ? rPam.GetPoint() == &rPam.GetBound()
                            : rPam.GetMark() == &rPam.GetBound());

    const SwPosition* pPos = &rPam.GetBound( TRUE );
    if( pPos->nNode.GetIndex() == nNode &&
        ( bBound1IsStart ? pPos->nContent.GetIndex() < nCntnt
                            : pPos->nContent.GetIndex() <= nCntnt ))
    {
        rSave.SetContent( pPos->nContent.GetIndex() );
        rSave.Add( rSaveArr );
    }

    pPos = &rPam.GetBound( FALSE );
    if( pPos->nNode.GetIndex() == nNode &&
        ( (bBound1IsStart && bChkSelDirection)
                    ? pPos->nContent.GetIndex() <= nCntnt
                    : pPos->nContent.GetIndex() < nCntnt ))
    {
        rSave.SetContent( pPos->nContent.GetIndex() );
        rSave.IncType();
        rSave.Add( rSaveArr );
        rSave.DecType();
    }
}

// #i59534: If a paragraph will be splitted we have to restore some redline positions
// This help function checks a position compared with a node and an content index

const int BEFORE_NODE = 0;          // Position before the given node index
const int BEFORE_SAME_NODE = 1;     // Same node index but content index before given content index
const int SAME_POSITION = 2;        // Same node index and samecontent index
const int BEHIND_SAME_NODE = 3;     // Same node index but content index behind given content index
const int BEHIND_NODE = 4;          // Position behind the given node index

int lcl_RelativePosition( const SwPosition& rPos, ULONG nNode, xub_StrLen nCntnt )
{
    ULONG nIndex = rPos.nNode.GetIndex();
    int nReturn = BEFORE_NODE;
    if( nIndex == nNode )
    {
        xub_StrLen nCntIdx = rPos.nContent.GetIndex();
        if( nCntIdx < nCntnt )
            nReturn = BEFORE_SAME_NODE;
        else if( nCntIdx == nCntnt )
            nReturn = SAME_POSITION;
        else
            nReturn = BEHIND_SAME_NODE;
    }
    else if( nIndex > nNode )
        nReturn = BEHIND_NODE; 
    return nReturn;
}

void _SaveCntntIdx( SwDoc* pDoc, ULONG nNode, xub_StrLen nCntnt,
                    SvULongs& rSaveArr, BYTE nSaveFly )
{
    // 1. Bookmarks
    _SwSaveTypeCountContent aSave;
    aSave.SetTypeAndCount( 0x8000, 0 );

    const SwBookmarks& rBkmks = pDoc->getBookmarks();
    for( ; aSave.GetCount() < rBkmks.Count(); aSave.IncCount() )
    {
        const SwBookmark* pBkmk = rBkmks[ aSave.GetCount() ];
        if( pBkmk->GetPos().nNode.GetIndex() == nNode &&
            pBkmk->GetPos().nContent.GetIndex() < nCntnt )
        {
            aSave.SetContent( pBkmk->GetPos().nContent.GetIndex() );
            aSave.Add( rSaveArr );
        }

        if( pBkmk->GetOtherPos() && pBkmk->GetOtherPos()->nNode.GetIndex() ==
            nNode && pBkmk->GetOtherPos()->nContent.GetIndex() < nCntnt )
        {
            aSave.SetContent( pBkmk->GetOtherPos()->nContent.GetIndex() );
            aSave.IncType();
            aSave.Add( rSaveArr );
            aSave.DecType();
        }
    }

    // 2. Redlines
    aSave.SetTypeAndCount( 0x1000, 0 );
    const SwRedlineTbl& rRedlTbl = pDoc->GetRedlineTbl();
    for( ; aSave.GetCount() < rRedlTbl.Count(); aSave.IncCount() )
    {
        const SwRedline* pRdl = rRedlTbl[ aSave.GetCount() ];
        ULONG nPointIdx = pRdl->GetPoint()->nNode.GetIndex();
        int nPointPos = lcl_RelativePosition( *pRdl->GetPoint(), nNode, nCntnt );
        int nMarkPos = pRdl->HasMark() ? lcl_RelativePosition( *pRdl->GetMark(), nNode, nCntnt ) :
                                          nPointPos;
        // #i59534: We have to store the positions inside the same node before the insert position
        // and the one at the insert position if the corresponding Point/Mark position is before
        // the insert position.
        if( nPointPos == BEFORE_SAME_NODE ||
            ( nPointPos == SAME_POSITION && nMarkPos < SAME_POSITION ) )
        {
            aSave.SetContent( pRdl->GetPoint()->nContent.GetIndex() );
            aSave.IncType();
            aSave.Add( rSaveArr );
            aSave.DecType();
        }
        if( pRdl->HasMark() && ( nMarkPos == BEFORE_SAME_NODE ||
            ( nMarkPos == SAME_POSITION && nPointPos < SAME_POSITION ) ) )
        {
            aSave.SetContent( pRdl->GetMark()->nContent.GetIndex() );
            aSave.Add( rSaveArr );
        }
    }

    // 4. Absatzgebundene Objekte
    {
        SwCntntNode *pNode = pDoc->GetNodes()[nNode]->GetCntntNode();
        if( pNode )
        {
            const SwPosition* pAPos;

            SwFrm* pFrm = pNode->GetFrm();
#if OSL_DEBUG_LEVEL > 1
            static BOOL bViaDoc = FALSE;
            if( bViaDoc )
                pFrm = NULL;
#endif
            if( pFrm ) // gibt es ein Layout? Dann ist etwas billiger...
            {
                if( pFrm->GetDrawObjs() )
                {
                    const SwSortedObjs& rDObj = *pFrm->GetDrawObjs();
                    for( sal_uInt32 n = rDObj.Count(); n; )
                    {
                        SwAnchoredObject* pObj = rDObj[ --n ];
                        const SwFrmFmt& rFmt = pObj->GetFrmFmt();
                        const SwFmtAnchor& rAnchor = rFmt.GetAnchor();
                        if( ( ( nSaveFly && FLY_AT_CNTNT == rAnchor.GetAnchorId() ) ||
                              FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ) &&
                            ( 0 != ( pAPos = rAnchor.GetCntntAnchor() ) ) )
                        {
                            aSave.SetType( 0x2000 );
                            aSave.SetContent( pAPos->nContent.GetIndex() );

                            ASSERT( nNode == pAPos->nNode.GetIndex(),
                                    "_SaveCntntIdx: Wrong Node-Index" );
                            if( FLY_AUTO_CNTNT == rAnchor.GetAnchorId() )
                            {
                                if( nCntnt <= aSave.GetContent() )
                                {
                                    if( SAVEFLY_SPLIT == nSaveFly )
                                        aSave.IncType(); // = 0x2001;
                                    else
                                        continue;
                                }
                            }
                            aSave.SetCount( pDoc->GetSpzFrmFmts()->Count() );
                            while( aSave.GetCount() &&
                                   &rFmt != (*pDoc->GetSpzFrmFmts())[
                                                aSave.DecCount() ] )
                                ; // nothing
                            ASSERT( &rFmt == (*pDoc->GetSpzFrmFmts())[
                                                    aSave.GetCount() ],
                                    "_SaveCntntIdx: Lost FrameFormat" );
                            aSave.Add( rSaveArr );
                        }
                    }
                }
            }
            else // Schade, kein Layout, dann ist es eben etwas teurer...
            {
                for( aSave.SetCount( pDoc->GetSpzFrmFmts()->Count() );
                        aSave.GetCount() ; )
                {
                    SwFrmFmt* pFrmFmt = (*pDoc->GetSpzFrmFmts())[
                                                aSave.DecCount() ];
                    if ( RES_FLYFRMFMT != pFrmFmt->Which() &&
                            RES_DRAWFRMFMT != pFrmFmt->Which() )
                        continue;

                    const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
                    if( ( FLY_AT_CNTNT == rAnchor.GetAnchorId() ||
                            FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ) &&
                        0 != ( pAPos = rAnchor.GetCntntAnchor()) &&
                        nNode == pAPos->nNode.GetIndex() )
                    {
                        aSave.SetType( 0x2000 );
                        aSave.SetContent( pAPos->nContent.GetIndex() );
                        if( FLY_AUTO_CNTNT == rAnchor.GetAnchorId() )
                        {
                            if( nCntnt <= aSave.GetContent() )
                            {
                                if( SAVEFLY_SPLIT == nSaveFly )
                                    aSave.IncType(); // = 0x2001;
                                else
                                    continue;
                            }
                        }
                        aSave.Add( rSaveArr );
                    }
                }
            }
        }
    }
    // 5. CrsrShell
    {
        SwCrsrShell* pShell = pDoc->GetEditShell();
        if( pShell )
        {
            aSave.SetTypeAndCount( 0x800, 0 );
            FOREACHSHELL_START( pShell )
                register SwPaM *_pStkCrsr = PCURSH->GetStkCrsr();
                if( _pStkCrsr )
                do {
                    ::_ChkPaM( rSaveArr, nNode, nCntnt, *_pStkCrsr,
                                aSave, FALSE );
                    aSave.IncCount();
                } while ( (_pStkCrsr != 0 ) &&
                    ((_pStkCrsr=(SwPaM *)_pStkCrsr->GetNext()) != PCURSH->GetStkCrsr()) );

                FOREACHPAM_START( PCURSH->_GetCrsr() )
                    ::_ChkPaM( rSaveArr, nNode, nCntnt, *PCURCRSR,
                                aSave, FALSE );
                    aSave.IncCount();
                FOREACHPAM_END()

            FOREACHSHELL_END( pShell )
        }
    }
    // 6. UnoCrsr
    {
        aSave.SetTypeAndCount( 0x400, 0 );
        register const SwUnoCrsrTbl& rTbl = pDoc->GetUnoCrsrTbl();
        for( USHORT n = 0; n < rTbl.Count(); ++n )
        {
            FOREACHPAM_START( rTbl[ n ] )
                ::_ChkPaM( rSaveArr, nNode, nCntnt, *PCURCRSR, aSave, FALSE );
                aSave.IncCount();
            FOREACHPAM_END()

            SwUnoTableCrsr* pUnoTblCrsr = (SwUnoTableCrsr*)*rTbl[ n ];
            if( pUnoTblCrsr )
            {
                FOREACHPAM_START( &pUnoTblCrsr->GetSelRing() )
                    ::_ChkPaM( rSaveArr, nNode, nCntnt, *PCURCRSR, aSave, FALSE );
                    aSave.IncCount();
                FOREACHPAM_END()
            }
        }
    }
}


void _RestoreCntntIdx( SwDoc* pDoc, SvULongs& rSaveArr,
                        ULONG nNode, xub_StrLen nOffset, BOOL bAuto )
{
    SwCntntNode* pCNd = pDoc->GetNodes()[ nNode ]->GetCntntNode();
    const SwBookmarks& rBkmks = pDoc->getBookmarks();
    const SwRedlineTbl& rRedlTbl = pDoc->GetRedlineTbl();
    SwSpzFrmFmts* pSpz = pDoc->GetSpzFrmFmts();
    USHORT n = 0;
    while( n < rSaveArr.Count() )
    {
        _SwSaveTypeCountContent aSave( rSaveArr, n );
        SwPosition* pPos = 0;
        switch( aSave.GetType() )
        {
        case 0x8000:
            pPos = (SwPosition*)&rBkmks[ aSave.GetCount() ]->GetPos();
            break;
        case 0x8001:
            pPos = (SwPosition*)rBkmks[ aSave.GetCount() ]->GetOtherPos();
            break;
        case 0x1001:
            pPos = (SwPosition*)rRedlTbl[ aSave.GetCount() ]->GetPoint();
            break;
        case 0x1000:
            pPos = (SwPosition*)rRedlTbl[ aSave.GetCount() ]->GetMark();
            break;
        case 0x2000:
            {
                SwFrmFmt *pFrmFmt = (*pSpz)[ aSave.GetCount() ];
                const SwFmtAnchor& rFlyAnchor = pFrmFmt->GetAnchor();
                if( rFlyAnchor.GetCntntAnchor() )
                {
                    SwFmtAnchor aNew( rFlyAnchor );
                    SwPosition aNewPos( *rFlyAnchor.GetCntntAnchor() );
                    aNewPos.nNode = *pCNd;
                    if( FLY_AUTO_CNTNT == rFlyAnchor.GetAnchorId() )
                        aNewPos.nContent.Assign( pCNd,
                                                 aSave.GetContent() + nOffset );
                    else
                        aNewPos.nContent.Assign( 0, 0 );
                    aNew.SetAnchor( &aNewPos );
                    pFrmFmt->SetAttr( aNew );
                }
            }
            break;
        case 0x2001:
            if( bAuto )
            {
                SwFrmFmt *pFrmFmt = (*pSpz)[ aSave.GetCount() ];
                SfxPoolItem *pAnchor = (SfxPoolItem*)&pFrmFmt->GetAnchor();
                pFrmFmt->SwModify::Modify( pAnchor, pAnchor );
            }
            break;

        case 0x0800:
        case 0x0801:
            {
                USHORT nCnt = 0;
                SwCrsrShell* pShell = pDoc->GetEditShell();
                if( pShell )
                {
                    FOREACHSHELL_START( pShell )
                        register SwPaM *_pStkCrsr = PCURSH->GetStkCrsr();
                        if( _pStkCrsr )
                        do {
                            if( aSave.GetCount() == nCnt )
                            {
                                pPos = &_pStkCrsr->GetBound( 0x0800 ==
                                                    aSave.GetType() );
                                break;
                            }
                            ++nCnt;
                        } while ( (_pStkCrsr != 0 ) &&
                            ((_pStkCrsr=(SwPaM *)_pStkCrsr->GetNext()) != PCURSH->GetStkCrsr()) );

                        if( pPos )
                            break;

                        FOREACHPAM_START( PCURSH->_GetCrsr() )
                            if( aSave.GetCount() == nCnt )
                            {
                                pPos = &PCURCRSR->GetBound( 0x0800 ==
                                                    aSave.GetType() );
                                break;
                            }
                            ++nCnt;
                        FOREACHPAM_END()
                        if( pPos )
                            break;

                    FOREACHSHELL_END( pShell )
                }
            }
            break;

        case 0x0400:
        case 0x0401:
            {
                USHORT nCnt = 0;
                register const SwUnoCrsrTbl& rTbl = pDoc->GetUnoCrsrTbl();
                for( USHORT i = 0; i < rTbl.Count(); ++i )
                {
                    FOREACHPAM_START( rTbl[ i ] )
                        if( aSave.GetCount() == nCnt )
                        {
                            pPos = &PCURCRSR->GetBound( 0x0400 ==
                                                    aSave.GetType() );
                            break;
                        }
                        ++nCnt;
                    FOREACHPAM_END()
                    if( pPos )
                        break;

                    SwUnoTableCrsr* pUnoTblCrsr = (SwUnoTableCrsr*)*rTbl[ i ];
                    if( pUnoTblCrsr )
                    {
                        FOREACHPAM_START( &pUnoTblCrsr->GetSelRing() )
                            if( aSave.GetCount() == nCnt )
                            {
                                pPos = &PCURCRSR->GetBound( 0x0400 ==
                                                    aSave.GetType() );
                                break;
                            }
                            ++nCnt;
                        FOREACHPAM_END()
                    }
                    if( pPos )
                        break;
                }
            }
            break;
        }

        if( pPos )
        {
            pPos->nNode = *pCNd;
            pPos->nContent.Assign( pCNd, aSave.GetContent() + nOffset );
        }
    }
}

void _RestoreCntntIdx( SvULongs& rSaveArr, const SwNode& rNd,
                        xub_StrLen nLen, xub_StrLen nChkLen )
{
    const SwDoc* pDoc = rNd.GetDoc();
    const SwBookmarks& rBkmks = pDoc->getBookmarks();
    const SwRedlineTbl& rRedlTbl = pDoc->GetRedlineTbl();
    const SwSpzFrmFmts* pSpz = pDoc->GetSpzFrmFmts();
    SwCntntNode* pCNd = (SwCntntNode*)rNd.GetCntntNode();

    while( rSaveArr.Count() >= 2 )
    {
        USHORT n = 0;
        _SwSaveTypeCountContent aSave( rSaveArr, n );
        if( aSave.GetContent() >= nChkLen )
            rSaveArr[ 1 ] -= nChkLen;
        else
        {
            SwPosition* pPos = 0;
            switch( aSave.GetType() )
            {
            case 0x8000:
                pPos = (SwPosition*)&rBkmks[ aSave.GetCount() ]->GetPos();
                break;
            case 0x8001:
                pPos = (SwPosition*)rBkmks[ aSave.GetCount() ]->GetOtherPos();
                break;
            case 0x1001:
                pPos = (SwPosition*)rRedlTbl[ aSave.GetCount() ]->GetPoint();
                break;
            case 0x1000:
                pPos = (SwPosition*)rRedlTbl[ aSave.GetCount() ]->GetMark();
                break;
            case 0x2000:
            case 0x2001:
                {
                    SwFrmFmt *pFrmFmt = (*pSpz)[ aSave.GetCount() ];
                    const SwFmtAnchor& rFlyAnchor = pFrmFmt->GetAnchor();
                    if( rFlyAnchor.GetCntntAnchor() )
                    {
                        SwFmtAnchor aNew( rFlyAnchor );
                        SwPosition aNewPos( *rFlyAnchor.GetCntntAnchor() );
                        aNewPos.nNode = rNd;
                        if( FLY_AUTO_CNTNT == rFlyAnchor.GetAnchorId() )
                            aNewPos.nContent.Assign( pCNd, Min(
                                                     aSave.GetContent(), nLen ) );
                        else
                            aNewPos.nContent.Assign( 0, 0 );
                        aNew.SetAnchor( &aNewPos );
                        pFrmFmt->SetAttr( aNew );
                    }
                }
                break;

            case 0x0800:
            case 0x0801:
                {
                    USHORT nCnt = 0;
                    SwCrsrShell* pShell = pDoc->GetEditShell();
                    if( pShell )
                    {
                        FOREACHSHELL_START( pShell )
                            register SwPaM *_pStkCrsr = PCURSH->GetStkCrsr();
                            if( _pStkCrsr )
                            do {
                                if( aSave.GetCount() == nCnt )
                                {
                                    pPos = &_pStkCrsr->GetBound( 0x0800 ==
                                                aSave.GetType() );
                                    break;
                                }
                                ++nCnt;
                            } while ( (_pStkCrsr != 0 ) &&
                                ((_pStkCrsr=(SwPaM *)_pStkCrsr->GetNext()) != PCURSH->GetStkCrsr()) );

                            if( pPos )
                                break;

                            FOREACHPAM_START( PCURSH->_GetCrsr() )
                                if( aSave.GetCount() == nCnt )
                                {
                                    pPos = &PCURCRSR->GetBound( 0x0800 ==
                                                aSave.GetType() );
                                    break;
                                }
                                ++nCnt;
                            FOREACHPAM_END()
                            if( pPos )
                                break;

                        FOREACHSHELL_END( pShell )
                    }
                }
                break;

            case 0x0400:
            case 0x0401:
                {
                    USHORT nCnt = 0;
                    register const SwUnoCrsrTbl& rTbl = pDoc->GetUnoCrsrTbl();
                    for( USHORT i = 0; i < rTbl.Count(); ++i )
                    {
                        FOREACHPAM_START( rTbl[ i ] )
                            if( aSave.GetCount() == nCnt )
                            {
                                pPos = &PCURCRSR->GetBound( 0x0400 ==
                                                    aSave.GetType() );
                                break;
                            }
                            ++nCnt;
                        FOREACHPAM_END()
                        if( pPos )
                            break;

                        SwUnoTableCrsr* pUnoTblCrsr = (SwUnoTableCrsr*)*rTbl[ i ];
                        if( pUnoTblCrsr )
                        {
                            FOREACHPAM_START( &pUnoTblCrsr->GetSelRing() )
                                if( aSave.GetCount() == nCnt )
                                {
                                    pPos = &PCURCRSR->GetBound( 0x0400 ==
                                                    aSave.GetType() );
                                    break;
                                }
                                ++nCnt;
                            FOREACHPAM_END()
                        }
                        if( pPos )
                            break;
                    }
                }
                break;
            }

            if( pPos )
            {
                pPos->nNode = rNd;
                pPos->nContent.Assign( pCNd, Min( aSave.GetContent(), nLen ) );
            }

            rSaveArr.Remove( 0, 2 );
        }
    }
}


