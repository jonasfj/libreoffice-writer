/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: flyfrms.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:47:05 $
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
#ifndef _FLYFRMS_HXX
#define _FLYFRMS_HXX
#include "flyfrm.hxx"

// OD 11.11.2003 #i22341#
class SwFmtAnchor;

// --> OD 2004-06-23 #i28701#
class SwFlyAtCntFrm;
class SwTxtFrm;

//Basisklasse fuer diejenigen Flys, die sich relativ frei Bewegen koennen -
//also die nicht _im_ Inhalt gebundenen Flys.
class SwFlyFreeFrm : public SwFlyFrm
{
    SwPageFrm *pPage;   //Bei dieser Seite ist der Fly angemeldet.

    // --> OD 2004-11-15 #i34753# - flag for at-page anchored Writer fly frames
    // to prevent a positioning - call of method <MakeObjPos()> -, if Writer
    // fly frame is already clipped during its format by the object formatter.
    bool mbNoMakePos;
    // <--
    // --> OD 2004-11-12 #i37068# - flag to prevent move in method
    // <CheckClip(..)>
    bool mbNoMoveOnCheckClip;
    // <--
    void CheckClip( const SwFmtFrmSize &rSz );  //'Emergency' Clipping.

    /** determines, if direct environment of fly frame has 'auto' size

        OD 07.08.2003 #i17297#, #111066#, #111070#
        start with anchor frame and search for a header, footer, row or fly frame
        stopping at page frame.
        return <true>, if such a frame is found and it has 'auto' size.
        otherwise <false> is returned.

        @author OD

        @return boolean indicating, that direct environment has 'auto' size
    */
    bool HasEnvironmentAutoSize() const;

protected:
    // OD 2004-05-12 #i28701# - new friend class <SwFlyNotify> for access to
    // method <NotifyBackground>
    friend class SwFlyNotify;
    virtual void NotifyBackground( SwPageFrm *pPage,
                                   const SwRect& rRect, PrepareHint eHint);

    SwFlyFreeFrm( SwFlyFrmFmt*, SwFrm *pAnchor );

public:
    // --> OD 2004-06-29 #i28701#
    TYPEINFO();

    virtual ~SwFlyFreeFrm();

    virtual void MakeAll();

    // --> OD 2004-11-12 #i37068# - accessors for member <mbNoMoveOnCheckClip>
    inline void SetNoMoveOnCheckClip( const bool _bNewNoMoveOnCheckClip )
    {
        mbNoMoveOnCheckClip = _bNewNoMoveOnCheckClip;
    }
    inline bool IsNoMoveOnCheckClip() const
    {
        return mbNoMoveOnCheckClip;
    }
    // <--
    // --> OD 2004-11-15 #i34753# - accessors for member <mbNoMakePos>
    inline void SetNoMakePos( const bool _bNoMakePos )
    {
        if ( IsFlyLayFrm() )
        {
            mbNoMakePos = _bNoMakePos;
        }
    }
    inline bool IsNoMakePos() const
    {
        if ( IsFlyLayFrm() )
        {
            return mbNoMakePos;
        }
        else
        {
            return false;
        }
    }
    // <--

    /** method to determine, if a format on the Writer fly frame is possible

        OD 2004-05-11 #i28701#
        refine 'IsFormatPossible'-conditions of method
        <SwFlyFrm::IsFormatPossible()> by:
        format isn't possible, if Writer fly frame isn't registered at a page frame
        and its anchor frame isn't inside another Writer fly frame.

        @author OD
    */
    virtual bool IsFormatPossible() const;
};


//Die Fly's, die an einem Layoutfrm haengen und nicht inhaltsgebunden sind
class SwFlyLayFrm : public SwFlyFreeFrm
{
public:
    // --> OD 2004-06-29 #i28701#
    TYPEINFO();

    SwFlyLayFrm( SwFlyFrmFmt*, SwFrm *pAnchor );
    SwFlyLayFrm( SwFlyLayFrm& );

    virtual	void Modify( SfxPoolItem*, SfxPoolItem* );

};

//Die Flys, die an einem Cntnt haengen nicht aber im Inhalt
class SwFlyAtCntFrm : public SwFlyFreeFrm
{
protected:
    virtual void MakeAll();

    // OD 2004-05-12 #i28701#
    virtual bool _InvalidationAllowed( const InvalidationType _nInvalid ) const;

    /** method to assure that anchored object is registered at the correct
        page frame

        OD 2004-07-02 #i28701#

        @author OD
    */
    virtual void RegisterAtCorrectPage();

public:
    // --> OD 2004-06-29 #i28701#
    TYPEINFO();

    SwFlyAtCntFrm( SwFlyFrmFmt*, SwFrm *pAnchor );

    virtual	void Modify( SfxPoolItem*, SfxPoolItem* );

    void SetAbsPos( const Point &rNew );

    // OD 2004-03-23 #i26791#
    virtual void MakeObjPos();

    /** method to determine, if a format on the Writer fly frame is possible

        OD 2004-05-11 #i28701#
        refine 'IsFormatPossible'-conditions of method
        <SwFlyFreeFrm::IsFormatPossible()> by:
        format isn't possible, if method <MakeAll()> is already in progress.

        @author OD
    */
    virtual bool IsFormatPossible() const;
};

//Die Flys, die an einem Zeichen in einem Cntnt haengen.
class SwFlyInCntFrm : public SwFlyFrm
{
    Point aRef;  //Relativ zu diesem Point wird die AbsPos berechnet.
    long  nLine; //Zeilenhoehe, Ref.Y() - nLine == Zeilenanfang.

    BOOL bInvalidLayout :1;
    BOOL bInvalidCntnt	:1;

protected:
    virtual void NotifyBackground( SwPageFrm *pPage,
                                   const SwRect& rRect, PrepareHint eHint);
    virtual void MakeAll();

public:
    // --> OD 2004-06-29 #i28701#
    TYPEINFO();

    SwFlyInCntFrm( SwFlyFrmFmt*, SwFrm *pAnchor );

    virtual ~SwFlyInCntFrm();
    virtual void  Format(  const SwBorderAttrs *pAttrs = 0 );
    virtual	void  Modify( SfxPoolItem*, SfxPoolItem* );

    void SetRefPoint( const Point& rPoint, const Point &rRelAttr,
        const Point &rRelPos );
    const Point &GetRefPoint() const { return aRef; }
    const Point GetRelPos() const;
          long   GetLineHeight() const { return nLine; }

    inline void InvalidateLayout() const;
    inline void InvalidateCntnt() const;
    inline void ValidateLayout() const;
    inline void ValidateCntnt() const;
    BOOL IsInvalid() const { return (bInvalidLayout || bInvalidCntnt); }
    BOOL IsInvalidLayout() const { return bInvalidLayout; }
    BOOL IsInvalidCntnt() const { return bInvalidCntnt; }


    //BP 26.11.93: vgl. tabfrm.hxx, gilt bestimmt aber fuer andere auch...
    //Zum Anmelden der Flys nachdem ein FlyCnt erzeugt _und_ eingefuegt wurde.
    //Muss vom Erzeuger gerufen werden, denn erst nach dem Konstruieren wird
    //Das Teil gepastet; mithin ist auch erst dann die Seite zum Anmelden der
    //Flys erreichbar.
    void RegistFlys();

    //siehe layact.cxx
    void AddRefOfst( long nOfst ) { aRef.Y() += nOfst; }

    // OD 2004-03-23 #i26791#
    virtual void MakeObjPos();

    // --> OD 2004-12-02 #115759# - invalidate anchor frame on invalidation
    // of the position, because the position is calculated during the
    // format of the anchor frame
    virtual void _ActionOnInvalidation( const InvalidationType _nInvalid );
    // <--
};

inline void SwFlyInCntFrm::InvalidateLayout() const
{
    ((SwFlyInCntFrm*)this)->bInvalidLayout = TRUE;
}
inline void SwFlyInCntFrm::InvalidateCntnt() const
{
    ((SwFlyInCntFrm*)this)->bInvalidCntnt = TRUE;
}
inline void SwFlyInCntFrm::ValidateLayout() const
{
    ((SwFlyInCntFrm*)this)->bInvalidLayout = FALSE;
}
inline void SwFlyInCntFrm::ValidateCntnt() const
{
    ((SwFlyInCntFrm*)this)->bInvalidCntnt = FALSE;
}

#endif
