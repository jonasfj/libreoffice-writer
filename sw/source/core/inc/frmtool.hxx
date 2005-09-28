/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: frmtool.hxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-28 11:08:31 $
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
#ifndef _FRMTOOL_HXX
#define _FRMTOOL_HXX

#include "swtypes.hxx"
#include "layfrm.hxx"
#include "frmatr.hxx"
#include "swcache.hxx"

class SwPageFrm;
class SwFlyFrm;
class SwCntntFrm;
class SwFtnContFrm;
class SwDoc;
class SwAttrSet;
class SdrObject;
class BigInt;
class SvxBrushItem;
class SdrMarkList;
class SwNodeIndex;
class OutputDevice;
class SwPageDesc;
class SwCrsrShell;
// OD 21.05.2003 #108789#
class SwTxtFrm;
// --> OD 2004-06-09 #i28701#
class SwLayAction;
class SwAnchoredObject;
// <--
#if defined(MSC)
#define MA_FASTCALL __fastcall
#else
#define MA_FASTCALL
#endif

#define WEIT_WECH 		LONG_MAX - 20000		//Initale Position der Flys.
#define BROWSE_HEIGHT   56700L * 10L               //10 Meter
//#define BROWSE_HEIGHT   1440L * 45L     // 45 inch, maximum size for pdf files

#define GRFNUM_NO 0
#define GRFNUM_YES 1
#define GRFNUM_REPLACE 2

//Painten des Hintergrunds. Mit Brush oder Graphic.
// OD 05.08.2002 #99657# - add 6th parameter to indicate that method should
//     consider background transparency, saved in the color of the brush item
void MA_FASTCALL DrawGraphic( const SvxBrushItem *, OutputDevice *,
      const SwRect &rOrg, const SwRect &rOut, const BYTE nGrfNum = GRFNUM_NO,
      const sal_Bool bConsiderBackgroundTransparency = sal_False );

// OD 24.01.2003 #106593# - method to align rectangle
// Created declaration here to avoid <extern> declarations
void MA_FASTCALL SwAlignRect( SwRect &rRect, ViewShell *pSh );

// OD 24.01.2003 #106593# - method to align graphic rectangle
// Created declaration here to avoid <extern> declarations
void SwAlignGrfRect( SwRect *pGrfRect, const OutputDevice &rOut );

//Fly besorgen, wenn keine List hineingereicht wird, wir die der aktuellen
//Shell benutzt.
//Implementierung in feshview.cxx
SwFlyFrm *GetFlyFromMarked( const SdrMarkList *pLst, ViewShell *pSh );

//Nicht gleich die math.lib anziehen.
ULONG MA_FASTCALL SqRt( BigInt nX );

//CntntNodes besorgen, CntntFrms erzeugen und in den LayFrm haengen.
void MA_FASTCALL _InsertCnt( SwLayoutFrm *pLay, SwDoc *pDoc, ULONG nIndex,
                 BOOL bPages = FALSE, ULONG nEndIndex = 0,
                 SwFrm *pPrv = 0 );

//Erzeugen der Frames fuer einen bestimmten Bereich, verwendet _InsertCnt
void MakeFrms( SwDoc *pDoc, const SwNodeIndex &rSttIdx,
                            const SwNodeIndex &rEndIdx );

//Um z.B. fuer Tabelleheadlines das Erzeugen der Flys in _InsertCnt zu unterbinden.
extern FASTBOOL bDontCreateObjects;

//Fuer FlyCnts, siehe SwFlyAtCntFrm::MakeAll()
extern FASTBOOL bSetCompletePaintOnInvalidate;

//Fuer Tabelleneinstellung per Tastatur.
long MA_FASTCALL CalcRowRstHeight( SwLayoutFrm *pRow );
long MA_FASTCALL CalcHeightWidthFlys( const SwFrm *pFrm ); 	//MA_FLY_HEIGHT

//Neue Seite einsetzen
SwPageFrm * MA_FASTCALL InsertNewPage( SwPageDesc &rDesc, SwFrm *pUpper,
                          BOOL bOdd, BOOL bInsertEmpty, BOOL bFtn,
                          SwFrm *pSibling );

//Flys bei der Seite anmelden.
void RegistFlys( SwPageFrm*, const SwLayoutFrm* );

//Benachrichtung des Fly Hintergrundes wenn Notwendig.
void Notify( SwFlyFrm *pFly, SwPageFrm *pOld, const SwRect &rOld,
             const SwRect* pOldRect = 0 );

void Notify_Background( const SdrObject* pObj,
                        SwPageFrm* pPage,
                        const SwRect& rRect,
                        const PrepareHint eHint,
                        const BOOL bInva );

const SwFrm* GetVirtualUpper( const SwFrm* pFrm, const Point& rPos );

BOOL Is_Lower_Of( const SwFrm *pCurrFrm, const SdrObject* pObj );

const SwFrm *FindKontext( const SwFrm *pFrm, USHORT nAdditionalKontextTyp );

BOOL IsFrmInSameKontext( const SwFrm *pInnerFrm, const SwFrm *pFrm );

const SwFrm * MA_FASTCALL FindPage( const SwRect &rRect, const SwFrm *pPage );

// JP 07.05.98: wird von SwCntntNode::GetFrm und von SwFlyFrm::GetFrm
//				gerufen
SwFrm* GetFrmOfModify( SwModify&, USHORT nFrmType, const Point* = 0,
                        const SwPosition *pPos = 0,
                        const BOOL bCalcFrm = FALSE );

//Sollen ExtraDaten (Reline-Strich, Zeilennummern) gepaintet werden?
FASTBOOL IsExtraData( const SwDoc *pDoc );

// OD 14.03.2003 #i11760# - method declaration <CalcCntnt(..)>
void CalcCntnt( SwLayoutFrm *pLay,
                bool bNoColl = false,
                bool bNoCalcFollow = false );


//Die Notify-Klassen merken sich im CTor die aktuellen Groessen und fuehren
//im DTor ggf. die notwendigen Benachrichtigungen durch.

class SwFrmNotify
{
protected:
    SwFrm *pFrm;
    const SwRect aFrm;
    const SwRect aPrt;
    SwTwips mnFlyAnchorOfst;
    SwTwips mnFlyAnchorOfstNoWrap;
    FASTBOOL     bHadFollow;
    FASTBOOL	 bInvaKeep;
    FASTBOOL	 bValidSize;
protected:
    // --> OD 2005-07-29 #i49383#
    bool mbFrmDeleted;
    // <--

public:
    SwFrmNotify( SwFrm *pFrm );
    ~SwFrmNotify();

    const SwRect &Frm() const { return aFrm; }
    const SwRect &Prt() const { return aPrt; }
    void SetInvaKeep() { bInvaKeep = TRUE; }
    // --> OD 2005-07-29 #i49383#
    void FrmDeleted()
    {
        mbFrmDeleted = true;
    }
    // <--
};

class SwLayNotify : public SwFrmNotify
{
    SwTwips  nHeightOfst;
    SwTwips  nWidthOfst;
    FASTBOOL bLowersComplete;

    SwLayoutFrm *GetLay() { return (SwLayoutFrm*)pFrm; }
public:
    SwLayNotify( SwLayoutFrm *pLayFrm );
    ~SwLayNotify();

    void SetLowersComplete( FASTBOOL b ) { bLowersComplete = b; }
    FASTBOOL IsLowersComplete() 		 { return bLowersComplete; }
};

class SwFlyNotify : public SwLayNotify
{
    SwPageFrm *pOldPage;
    const SwRect aFrmAndSpace;
    SwFlyFrm *GetFly() { return (SwFlyFrm*)pFrm; }
public:
    SwFlyNotify( SwFlyFrm *pFlyFrm );
    ~SwFlyNotify();

    SwPageFrm *GetOldPage() const { return pOldPage; }
};

class SwCntntNotify : public SwFrmNotify
{
private:
    // OD 08.01.2004 #i11859#
    bool    mbChkHeightOfLastLine;
    SwTwips mnHeightOfLastLine;

    // OD 2004-02-26 #i25029#
    bool        mbInvalidatePrevPrtArea;
    bool        mbBordersJoinedWithPrev;

    SwCntntFrm *GetCnt() { return (SwCntntFrm*)pFrm; }

public:
    SwCntntNotify( SwCntntFrm *pCntFrm );
    ~SwCntntNotify();

    // OD 2004-02-26 #i25029#
    void SetInvalidatePrevPrtArea()
    {
        mbInvalidatePrevPrtArea = true;
    }

    void SetBordersJoinedWithPrev()
    {
        mbBordersJoinedWithPrev = true;
    }
};

//SwBorderAttrs kapselt die Berechnung fuer die Randattribute inclusive
//Umrandung. Die Attribute und die errechneten Werte werden gecached.
//Neu: Die gesammte Klasse wird gecached.

//!!!Achtung: Wenn weitere Attribute gecached werden muss unbedingt die
//Methode Modify::Modify mitgepflegt werden!!!

// OD 23.01.2003 #106895# - delete old method <SwBorderAttrs::CalcRight()> and
// the stuff that belongs to it.
class SwBorderAttrs : public SwCacheObj
{
    const SwAttrSet		 &rAttrSet;
    const SvxULSpaceItem &rUL;
    const SvxLRSpaceItem &rLR;
    const SvxBoxItem	 &rBox;
    const SvxShadowItem  &rShadow;
    const Size			  aFrmSize;		//Die FrmSize

    BOOL bBorderDist	:1;				//Ist's ein Frm der auch ohne Linie
                                        //einen Abstand haben kann?

    //Mit den Folgenden Bools werden die gecache'ten Werte fuer UNgueltig
    //erklaert - bis sie einmal berechnet wurden.
    BOOL bTopLine		:1;
    BOOL bBottomLine	:1;
    BOOL bLeftLine		:1;
    BOOL bRightLine		:1;
    BOOL bTop			:1;
    BOOL bBottom		:1;
    BOOL bLine			:1;

    BOOL bIsLine		:1;	//Umrandung an mind. einer Kante?

    BOOL bCacheGetLine		  :1; //GetTopLine(), GetBottomLine() cachen?
    BOOL bCachedGetTopLine	  :1; //GetTopLine() gecached?
    BOOL bCachedGetBottomLine :1; //GetBottomLine() gecached?
    // OD 21.05.2003 #108789# - booleans indicating, if values <bJoinedWithPrev>
    //          and <bJoinedWithNext> are cached and valid.
    //          Caching depends on value of <bCacheGetLine>.
    mutable BOOL bCachedJoinedWithPrev :1;
    mutable BOOL bCachedJoinedWithNext :1;
    // OD 21.05.2003 #108789# - booleans indicating, if borders are joined
    //          with previous/next frame.
    BOOL bJoinedWithPrev :1;
    BOOL bJoinedWithNext :1;

    //Die gecache'ten Werte, undefiniert bis sie einmal berechnet wurden.
    USHORT nTopLine,
           nBottomLine,
           nLeftLine,
           nRightLine,
           nTop,
           nBottom,
           nGetTopLine,
           nGetBottomLine;

    //Nur die Lines + Shadow errechnen.
    void _CalcTopLine();
    void _CalcBottomLine();
    void _CalcLeftLine();
    void _CalcRightLine();

    //Lines + Shadow + Abstaende
    void _CalcTop();
    void _CalcBottom();

    void _IsLine();

    // OD 2004-02-26 #i25029# - add optional 2nd parameter <_pPrevFrm>
    // If set, its value is taken for testing, if borders/shadow have to joined
    // with previous frame.
    void _GetTopLine   ( const SwFrm& _rFrm,
                         const SwFrm* _pPrevFrm = 0L );
    void _GetBottomLine( const SwFrm& _rFrm );

    // OD 21.05.2003 #108789# - private methods to calculate cached values
    // <bJoinedWithPrev> and <bJoinedWithNext>.
    // OD 2004-02-26 #i25029# - add optional 2nd parameter <_pPrevFrm>
    // If set, its value is taken for testing, if borders/shadow have to joined
    // with previous frame.
    void _CalcJoinedWithPrev( const SwFrm& _rFrm,
                              const SwFrm* _pPrevFrm = 0L );
    void _CalcJoinedWithNext( const SwFrm& _rFrm );

    // OD 21.05.2003 #108789# - internal helper method for methods
    // <_CalcJoinedWithPrev> and <_CalcJoinedWithNext>.
    BOOL _JoinWithCmp( const SwFrm& _rCallerFrm,
                       const SwFrm& _rCmpFrm ) const;

     //Rechte und linke Linie sowie LRSpace gleich?
    // OD 21.05.2003 #108789# - change name of 1st parameter - "rAttrs" -> "rCmpAttrs".
    BOOL CmpLeftRight( const SwBorderAttrs &rCmpAttrs,
                       const SwFrm *pCaller,
                       const SwFrm *pCmp ) const;

public:
    DECL_FIXEDMEMPOOL_NEWDEL(SwBorderAttrs)

    SwBorderAttrs( const SwModify *pOwner, const SwFrm *pConstructor );
    ~SwBorderAttrs();

    inline const SwAttrSet		&GetAttrSet() const { return rAttrSet;	}
    inline const SvxULSpaceItem &GetULSpace() const { return rUL;		}
    inline const SvxLRSpaceItem &GetLRSpace() const { return rLR;	  	}
    inline const SvxBoxItem 	&GetBox()	  const { return rBox;		}
    inline const SvxShadowItem	&GetShadow()  const { return rShadow;	}

    inline USHORT CalcTopLine() const;
    inline USHORT CalcBottomLine() const;
    inline USHORT CalcLeftLine() const;
    inline USHORT CalcRightLine() const;
    inline USHORT CalcTop() const;
    inline USHORT CalcBottom() const;
           long CalcLeft( const SwFrm *pCaller ) const;
           long CalcRight( const SwFrm *pCaller ) const;

    inline BOOL IsLine() const;

    inline const Size &GetSize()	 const { return aFrmSize; }

    inline BOOL IsBorderDist() const { return bBorderDist; }

    //Sollen obere bzw. untere Umrandung fuer den Frm ausgewertet werden?
    // OD 2004-02-26 #i25029# - add optional 2nd parameter <_pPrevFrm>
    // If set, its value is taken for testing, if borders/shadow have to joined
    // with previous frame.
    inline USHORT GetTopLine   ( const SwFrm& _rFrm,
                                 const SwFrm* _pPrevFrm = 0L ) const;
    inline USHORT GetBottomLine( const SwFrm& _rFrm ) const;
    inline void	  SetGetCacheLine( BOOL bNew ) const;
    // OD 21.05.2003 #108789# - accessors for cached values <bJoinedWithPrev>
    // and <bJoinedWithPrev>
    // OD 2004-02-26 #i25029# - add optional 2nd parameter <_pPrevFrm>
    // If set, its value is taken for testing, if borders/shadow have to joined
    // with previous frame.
    BOOL JoinedWithPrev( const SwFrm& _rFrm,
                         const SwFrm* _pPrevFrm = 0L ) const;
    BOOL JoinedWithNext( const SwFrm& _rFrm ) const;
};

class SwBorderAttrAccess : public SwCacheAccess
{
    const SwFrm *pConstructor;		//opt: Zur weitergabe an SwBorderAttrs
protected:
    virtual SwCacheObj *NewObj();

public:
    SwBorderAttrAccess( SwCache &rCache, const SwFrm *pOwner );

    SwBorderAttrs *Get();
};

//---------------------------------------------------------------------
//Iterator fuer die DrawObjecte einer Seite. Die Objecte werden Nach ihrer
//Z-Order iteriert.
//Das iterieren ist nicht eben billig, denn fuer alle Operationen muss jeweils
//ueber das gesamte SortArray iteriert werden.
class SwOrderIter
{
    const SwPageFrm *pPage;
    const SdrObject *pCurrent;
    const FASTBOOL bFlysOnly;
public:
    SwOrderIter( const SwPageFrm *pPage, FASTBOOL bFlysOnly = TRUE );

    void 			 Current( const SdrObject *pNew ) { pCurrent = pNew; }
    const SdrObject *Current()	  const { return pCurrent; }
    const SdrObject *operator()() const { return pCurrent; }
    const SdrObject *Top();
    const SdrObject *Bottom();
    const SdrObject *Next();
    const SdrObject *Prev();
};


class StackHack
{
    static BYTE nCnt;
    static BOOL bLocked;
public:
    StackHack()
    {
        if ( ++StackHack::nCnt > 50 )
            StackHack::bLocked = TRUE;
    }
    ~StackHack()
    {
        if ( --StackHack::nCnt < 5 )
            StackHack::bLocked = FALSE;
    }

    static BOOL IsLocked()	{ return StackHack::bLocked; }
    static BYTE Count()		{ return StackHack::nCnt; }
};


//Sollen obere bzw. untere Umrandung fuer den Frm ausgewertet werden?
// OD 2004-02-26 #i25029# - add optional 2nd parameter <_pPrevFrm>
// If set, its value is taken for testing, if borders/shadow have to joined
// with previous frame.
inline USHORT SwBorderAttrs::GetTopLine ( const SwFrm& _rFrm,
                                          const SwFrm* _pPrevFrm ) const
{
    if ( !bCachedGetTopLine || _pPrevFrm )
    {
        const_cast<SwBorderAttrs*>(this)->_GetTopLine( _rFrm, _pPrevFrm );
    }
    return nGetTopLine;
}
inline USHORT SwBorderAttrs::GetBottomLine( const SwFrm& _rFrm ) const
{
    if ( !bCachedGetBottomLine )
        const_cast<SwBorderAttrs*>(this)->_GetBottomLine( _rFrm );
    return nGetBottomLine;
}
inline void	SwBorderAttrs::SetGetCacheLine( BOOL bNew ) const
{
    ((SwBorderAttrs*)this)->bCacheGetLine = bNew;
    ((SwBorderAttrs*)this)->bCachedGetBottomLine =
    ((SwBorderAttrs*)this)->bCachedGetTopLine = FALSE;
    // OD 21.05.2003 #108789# - invalidate cache for values <bJoinedWithPrev>
    // and <bJoinedWithNext>.
    bCachedJoinedWithPrev = FALSE;
    bCachedJoinedWithNext = FALSE;
}

inline USHORT SwBorderAttrs::CalcTopLine() const
{
    if ( bTopLine )
        ((SwBorderAttrs*)this)->_CalcTopLine();
    return nTopLine;
}
inline USHORT SwBorderAttrs::CalcBottomLine() const
{
    if ( bBottomLine )
        ((SwBorderAttrs*)this)->_CalcBottomLine();
    return nBottomLine;
}
inline USHORT SwBorderAttrs::CalcLeftLine() const
{
    if ( bLeftLine )
        ((SwBorderAttrs*)this)->_CalcLeftLine();
    return nLeftLine;
}
inline USHORT SwBorderAttrs::CalcRightLine() const
{
    if ( bRightLine )
        ((SwBorderAttrs*)this)->_CalcRightLine();
    return nRightLine;
}
inline USHORT SwBorderAttrs::CalcTop() const
{
    if ( bTop )
        ((SwBorderAttrs*)this)->_CalcTop();
    return nTop;
}
inline USHORT SwBorderAttrs::CalcBottom() const
{
    if ( bBottom )
        ((SwBorderAttrs*)this)->_CalcBottom();
    return nBottom;
}
inline BOOL SwBorderAttrs::IsLine() const
{
    if ( bLine )
        ((SwBorderAttrs*)this)->_IsLine();
    return bIsLine;
}

/** method to determine the spacing values of a frame

    OD 2004-03-10 #i28701#
    Values only provided for flow frames (table, section or text frames)
    Note: line spacing value is only determined for text frames

    @param _rFrm
    input parameter - frame, for which the spacing values are determined.

    @param _roPrevLowerSpacing
    output parameter - lower spacing of the frame in SwTwips

    @param _roPrevLineSpacing
    output parameter - line spacing of the frame in SwTwips

    @author OD
*/
void GetSpacingValuesOfFrm( const SwFrm& _rFrm,
                            SwTwips& _roLowerSpacing,
                            SwTwips& _roLineSpacing );

/** method to get the content of the table cell

    Content from any nested tables will be omitted.
    Note: line spacing value is only determined for text frames

    @param rCell_
    input parameter - the cell which should be searched for content.

    return
        pointer to the found content frame or 0
*/

const SwCntntFrm* GetCellCntnt( const SwLayoutFrm& rCell_ );

#endif  //_FRMTOOL_HXX
