/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mvsave.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 16:20:52 $
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
#ifndef _MVSAVE_HXX
#define _MVSAVE_HXX


#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _KEYCOD_HXX //autogen
#include <vcl/keycod.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

#ifndef IDOCUMENTBOOKMARKACCESS_HXX_INCLUDED
#include <IDocumentBookmarkAccess.hxx>
#endif

class SvNumberFormatter;
class SvULongs;
class SwBookmark;
class SwDoc;
class SwFmtAnchor;
class SwFrmFmt;
class SwIndex;
class SwNodeIndex;
class SwNodeRange;
class SwPaM;
struct SwPosition;

enum SaveBookmarkType { BKMK_POS_NONE 	= 0x00,
                        BKMK_POS 		= 0x01,
                        BKMK_POS_OTHER	= 0x02
                         };

class SaveBookmark
{
    String	aName, aShortName;
    ULONG nNode1, nNode2;
    xub_StrLen nCntnt1, nCntnt2;
    KeyCode aCode;
    SaveBookmarkType eBkmkType;
    IDocumentBookmarkAccess::BookmarkType eOrigBkmType;

public:
    SaveBookmark( int, const SwBookmark&, const SwNodeIndex&,
                                    const SwIndex* pIdx = 0 );
    void SetInDoc( SwDoc* pDoc, const SwNodeIndex&, const SwIndex* pIdx = 0);
    IDocumentBookmarkAccess::BookmarkType GetOriginalBkmType() const {return eOrigBkmType;}
};

SV_DECL_PTRARR_DEL( SaveBookmarks, SaveBookmark*, 0, 10 )

void _DelBookmarks( const SwNodeIndex& rStt,
                    const SwNodeIndex& rEnd,
                    SaveBookmarks* pSaveBkmk = 0,
                    const SwIndex* pSttIdx = 0,
                    const SwIndex* pEndIdx = 0 );


#define SAVEFLY 1
#define SAVEFLY_SPLIT 2

void _SaveCntntIdx( SwDoc* pDoc, ULONG nNode, xub_StrLen nCntnt,
                    SvULongs& rSaveArr, BYTE nSaveFly = 0 );
void _RestoreCntntIdx( SwDoc* pDoc, SvULongs& rSaveArr,
                        ULONG nNode, xub_StrLen nOffset = 0,
                        BOOL bAuto = FALSE );
void _RestoreCntntIdx( SvULongs& rSaveArr, const SwNode& rNd,
                        xub_StrLen nLen, xub_StrLen nCorrLen );


/** data structure to temporarily hold fly anchor positions relative to some
 *  location. */
struct _SaveFly
{
    ULONG nNdDiff;              /// relative node difference
    SwFrmFmt* pFrmFmt;          /// the fly's frame format
    sal_Bool bInsertPosition;   /// if true, anchor _at_ insert position

    _SaveFly( ULONG nNodeDiff, SwFrmFmt* pFmt, sal_Bool bInsert )
        : nNdDiff( nNodeDiff ), pFrmFmt( pFmt ), bInsertPosition( bInsert )
    { }
};

SV_DECL_VARARR( _SaveFlyArr, _SaveFly, 0, 10 )

void _RestFlyInRange( _SaveFlyArr& rArr, const SwNodeIndex& rSttIdx,
                      const SwNodeIndex* pInsPos );
void _SaveFlyInRange( const SwNodeRange& rRg, _SaveFlyArr& rArr );
void _SaveFlyInRange( const SwPaM& rPam, const SwNodeIndex& rInsPos,
                       _SaveFlyArr& rArr, sal_Bool bMoveAllFlys );

void DelFlyInRange( const SwNodeIndex& rMkNdIdx,
                    const SwNodeIndex& rPtNdIdx );


class SwDataChanged
{
    const SwPaM* pPam;
    const SwPosition* pPos;
    SwDoc* pDoc;
    ULONG nNode;
    xub_StrLen nCntnt;
    USHORT nType;		// Insert/Move/Delete/... (UndoIds)

public:
    SwDataChanged( const SwPaM& rPam, USHORT nType );
    SwDataChanged( SwDoc* pDoc, const SwPosition& rPos, USHORT nType );
    ~SwDataChanged();

    ULONG GetNode() const 			{ return nNode; }
    xub_StrLen GetCntnt() const 	{ return nCntnt; }
};


// Funktions-Deklaration damit auch alles unter der CrsrShell mal die
// Crsr verschieben kann
// die Funktionen rufen nicht die SwDoc::Corr - Methoden!

    // Setzt alle PaMs an OldPos auf NewPos + Offset
void PaMCorrAbs( const SwPosition &rOldPos,
                const SwPosition &rNewPos,
                const xub_StrLen nOffset = 0 );

    // Setzt alle PaMs in OldNode auf NewPos + Offset
void PaMCorrAbs( const SwNodeIndex &rOldNode,
                const SwPosition &rNewPos,
                const xub_StrLen nOffset = 0 );

    // Setzt alle PaMs im Bereich vom Range nach NewPos
void PaMCorrAbs( const SwPaM& rRange,
                 const SwPosition& rNewPos );

    // Setzt alle PaMs im Bereich von [StartNode, EndNode] nach NewPos
void PaMCorrAbs( const SwNodeIndex &rStartNode,
                 const SwNodeIndex &rEndNode,
                 const SwPosition &rNewPos );

    // Setzt alle PaMs in OldNode auf relative Pos
void PaMCorrRel( const SwNodeIndex &rOldNode,
                 const SwPosition &rNewPos,
                 const xub_StrLen nOffset = 0 );


// Hilfsklasse zum kopieren von absatzgebundenen Flys. Durch die Sortierung
// nach der Ordnungsnummer wird versucht die layout seitige Anordnung
// bei zu behalten
class _ZSortFly
{
    const SwFrmFmt* pFmt;
    const SwFmtAnchor* pAnchor;
    UINT32 nOrdNum;

public:
    _ZSortFly( const SwFrmFmt* pFrmFmt, const SwFmtAnchor* pFlyAnchor,
                UINT32 nArrOrdNum );
    _ZSortFly& operator=( const _ZSortFly& rCpy )
    {
        pFmt = rCpy.pFmt, pAnchor = rCpy.pAnchor, nOrdNum = rCpy.nOrdNum;
        return *this;
    }

    int operator==( const _ZSortFly& ) const { return FALSE; }
    int operator<( const _ZSortFly& rCmp ) const
        { return nOrdNum < rCmp.nOrdNum; }

    const SwFrmFmt* GetFmt() const				{ return pFmt; }
    const SwFmtAnchor* GetAnchor() const		{ return pAnchor; }
};

SV_DECL_VARARR_SORT( _ZSortFlys, _ZSortFly, 0, 10 )


class SwTblNumFmtMerge
{
    SvNumberFormatter* pNFmt;
public:
    SwTblNumFmtMerge( const SwDoc& rSrc, SwDoc& rDest );
    ~SwTblNumFmtMerge();
};


class _SaveRedlEndPosForRestore
{
    SvPtrarr* pSavArr;
    SwNodeIndex* pSavIdx;
    xub_StrLen nSavCntnt;

    void _Restore();
public:
    _SaveRedlEndPosForRestore( const SwNodeIndex& rInsIdx, xub_StrLen nCntnt );
    ~_SaveRedlEndPosForRestore();
    void Restore() { if( pSavArr ) _Restore(); }
};


#endif	// _MVSAVE_HXX

