/*************************************************************************
 *
 *  $RCSfile: txtfly.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: os $ $Date: 2002-12-05 13:29:53 $
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
#ifndef _TXTFLY_HXX
#define _TXTFLY_HXX

#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

#include "swtypes.hxx"
#include "swrect.hxx"

class OutputDevice;
class VirtualDevice;
class SwFont;
class SwCntntFrm;
class SwFlyFrm;
class SwPageFrm;
class SwTxtFly;
class SdrObject;
class SwWrongList;
class SwTxtPaintInfo;
class SwFmt;
class PolyPolygon;
class TextRanger;
class Color;

// eine kleine Schweinerei, weil enums nicht forward-deklariert werden koennen.
typedef MSHORT _FlyCntnt;

SV_DECL_PTRARR( SwFlyList, SdrObject*, 10, 10 )

/*************************************************************************
 *						class SwFlyIter
 *************************************************************************/
enum PAGESIDE { LEFT_SIDE, RIGHT_SIDE, DONTKNOW_SIDE };

/*************************************************************************
 *						class SwContourCache
 *************************************************************************/

class SwDrawTextInfo;
// Contour-Cache, globale Variable, in txtinit.cxx initialisiert/zerstoert
// und in txtfly.cxx benutzt bei Konturumfluss
class SwContourCache;
extern SwContourCache *pContourCache;

#ifdef VERTICAL_LAYOUT
class SwTxtFrm;
#endif

#define POLY_CNT 20
#define POLY_MIN 5
#define POLY_MAX 4000

class SwContourCache
{
    friend void ClrContourCache();
    const SdrObject *pSdrObj[ POLY_CNT ];
    TextRanger *pTextRanger[ POLY_CNT ];
    long nPntCnt;
    MSHORT nObjCnt;
#ifdef VERTICAL_LAYOUT
    const SwRect ContourRect( const SwFmt* pFmt, const SdrObject* pObj,
        const SwTxtFrm* pFrm, const SwRect &rLine, const long nXPos,
        const sal_Bool bRight );
#else
    const SwRect ContourRect( const SwFmt* pFmt, const SdrObject* pObj,
        const SwRect &rLine, const long nXPos, const sal_Bool bRight );
#endif

public:
    SwContourCache();
    ~SwContourCache();
    const SdrObject* GetObject( MSHORT nPos ){ return pSdrObj[ nPos ]; }
    MSHORT GetCount() const { return nObjCnt; }
    void ClrObject( MSHORT nPos );
#ifdef VERTICAL_LAYOUT
    static const SwRect CalcBoundRect( const SdrObject* pObj,
        const SwRect &rLine, const SwTxtFrm* pFrm, const long nXPos,
        const sal_Bool bRight );
#else
    static const SwRect CalcBoundRect( const SdrObject* pObj,
        const SwRect &rLine, const long nXPos, const sal_Bool bRight );
#endif
#ifndef PRODUCT
    void ShowContour( OutputDevice* pOut, const SdrObject* pObj,
                      const Color& rClosedColor, const Color& rOpenColor );
#endif
};

/*************************************************************************
 *						class SwTxtFly
 *************************************************************************/

class SwTxtFly
{
    const SwPageFrm		*pPage;
    const SdrObject		*pCurrFly;

#ifdef VERTICAL_LAYOUT
    const SwTxtFrm      *pCurrFrm;
#else
    const SwCntntFrm    *pCurrFrm;
#endif

    const SwCntntFrm	*pMaster;
    SwFlyList			*pFlyList;

    long nMinBottom;
    long nNextTop; // Hier wird die Oberkante des "naechsten" Rahmens gespeichert
    ULONG nIndex;
    sal_Bool bOn : 1;
    sal_Bool bLeftSide : 1;
    sal_Bool bTopRule: 1;
    SwRect _GetFrm( const SwRect &rPortion, sal_Bool bTop ) const;
    SwFlyList* InitFlyList();
    // iteriert ueber die Fly-Liste
    sal_Bool ForEach( const SwRect &rRect, SwRect* pRect, sal_Bool bAvoid ) const;
    _FlyCntnt CalcSmart( const SdrObject *pObj ) const;
    // liefert die Order eines FlyFrms
    _FlyCntnt GetOrder( const SdrObject *pObj ) const;
    void CalcRightMargin( SwRect &rFly, MSHORT nPos, const SwRect &rLine ) const;
    void CalcLeftMargin( SwRect &rFly, MSHORT nPos, const SwRect &rLine ) const;
    MSHORT GetPos( const SdrObject *pObj ) const;
    sal_Bool GetTop( const SdrObject *pNew, const sal_Bool bInFtn,
                     const sal_Bool bInFooterOrHeader );
    SwTwips CalcMinBottom() const;
    const SwCntntFrm* _GetMaster();

public:
    inline SwTxtFly() { pFlyList = 0; pMaster = 0; }
#ifdef VERTICAL_LAYOUT
    inline SwTxtFly( const SwTxtFrm *pFrm ) { CtorInit( pFrm ); }
#else
    inline SwTxtFly( const SwCntntFrm *pFrm ) { CtorInit( pFrm ); }
#endif

    SwTxtFly( const SwTxtFly& rTxtFly );
    inline ~SwTxtFly() { delete pFlyList; }
#ifdef VERTICAL_LAYOUT
    void CtorInit( const SwTxtFrm *pFrm );
#else
    void CtorInit( const SwCntntFrm *pFrm );
#endif
    void SetTopRule(){ bTopRule = sal_False; }

    SwFlyList* GetFlyList() const
        { return pFlyList ? pFlyList : ((SwTxtFly*)this)->InitFlyList(); }
    inline SwRect GetFrm( const SwRect &rPortion, sal_Bool bTop = sal_True ) const;
    inline sal_Bool IsOn() const { return bOn; }
    inline sal_Bool Relax( const SwRect &rRect );
    inline sal_Bool Relax();
    inline SwTwips GetMinBottom() const
        { return pFlyList ? nMinBottom : CalcMinBottom(); }
    inline const SwCntntFrm* GetMaster() const
        { return pMaster ? pMaster : ((SwTxtFly*)this)->_GetMaster(); }
    inline long GetNextTop() const { return nNextTop; }
    // Diese temporaere Variable darf auch in const-Methoden manipuliert werden
    inline void SetNextTop( long nNew ) const
        { ((SwTxtFly*)this)->nNextTop = nNew;	}

    // Liefert zu einem SdrObject das von ihm _beanspruchte_ Rect
    // (unter Beruecksichtigung der Order) zurueck.
    SwRect FlyToRect( const SdrObject *pObj, const SwRect &rRect ) const;

    // Die Drawmethoden stellen sicher, dass ueberlappende Frames
    // (ausser bei transparenten Frames) nicht uebergepinselt werden.
    sal_Bool DrawTextOpaque( SwDrawTextInfo &rInf );

    void DrawFlyRect( OutputDevice *pOut, const SwRect &rRect,
                      const SwTxtPaintInfo &rInf, sal_Bool bNoGraphic = sal_False );

    // Liefert zurueck, ob die Zeile von einem Frame ueberlappt wird.
    sal_Bool IsAnyFrm( const SwRect &rLine ) const;
    sal_Bool IsAnyFrm() const;
    //Das Rechteck kann leer sein, es gilt dann der Frm.
    sal_Bool IsAnyObj( const SwRect& ) const;

#ifndef PRODUCT
    void ShowContour( OutputDevice* pOut );
#endif
};

// Wenn in das rRect (meist die aktuelle Zeile) kein freifliegender
// Frame ragt, dann schalten wir uns ab.
// rRect ist dokumentglobal !
inline sal_Bool SwTxtFly::Relax( const SwRect &rRect )
{
    return 0 != (bOn = bOn && IsAnyFrm( rRect ));
}

inline sal_Bool SwTxtFly::Relax()
{
    return 0 != (bOn = bOn && IsAnyFrm());
}

inline SwRect SwTxtFly::GetFrm( const SwRect &rRect, sal_Bool bTop ) const
{
    return bOn ? _GetFrm( rRect, bTop ) : SwRect();
}


#endif
