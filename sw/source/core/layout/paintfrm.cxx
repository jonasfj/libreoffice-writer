/*************************************************************************
 *
 *  $RCSfile: paintfrm.cxx,v $
 *
 *  $Revision: 1.55 $
 *
 *  last change: $Author: fme $ $Date: 2002-12-10 09:34:03 $
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

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER
#include <hintids.hxx>

#ifndef _SOUND_HXX //autogen
#include <vcl/sound.hxx>
#endif
#ifndef _POLY_HXX //autogen
#include <vcl/poly.hxx>
#endif
#define _SVSTDARR_LONGS
#include <svtools/svstdarr.hxx>

#ifndef _XOUTBMP_HXX //autogen
#include <svx/xoutbmp.hxx>
#endif
#ifndef _SFX_PROGRESS_HXX //autogen
#include <sfx2/progress.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_OPAQITEM_HXX //autogen
#include <svx/opaqitem.hxx>
#endif
#ifndef _SVX_PRNTITEM_HXX //autogen
#include <svx/prntitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef SW_TGRDITEM_HXX
#include <tgrditem.hxx>
#endif


#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>
#endif
#ifndef _SWATRSET_HXX
#include <swatrset.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _DVIEW_HXX
#include <dview.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _FTNFRM_HXX
#include <ftnfrm.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _NOTXTFRM_HXX
#include <notxtfrm.hxx>
#endif
#ifndef _FRMSH_HXX
#include <frmsh.hxx>
#endif
#ifndef _SWREGION_HXX
#include <swregion.hxx>
#endif
#ifndef _LAYACT_HXX
#include <layact.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _PTQUEUE_HXX
#include <ptqueue.hxx>
#endif
#ifndef _NOTEURL_HXX
#include <noteurl.hxx>
#endif
#ifndef _VIRTOUTP_HXX
#include <virtoutp.hxx>
#endif
#ifndef _LINEINFO_HXX
#include <lineinfo.hxx>
#endif
#ifndef _DBG_LAY_HXX
#include <dbg_lay.hxx>
#endif
#ifndef _ACCESSIBILITYOPTIONS_HXX
#include <accessibilityoptions.hxx>
#endif

#define GETOBJSHELL()		((SfxObjectShell*)rSh.GetDoc()->GetDocShell())

//Tabellenhilfslinien an?
#define IS_SUBS_TABLE \
    (pGlobalShell->GetViewOptions()->IsTable() && \
    !pGlobalShell->GetViewOptions()->IsPagePreview()&&\
    !pGlobalShell->GetViewOptions()->IsReadonly()&&\
     SwViewOption::IsTableBoundaries())
//sonstige Hilfslinien an?
#define IS_SUBS (!pGlobalShell->GetViewOptions()->IsPagePreview() && \
        !pGlobalShell->GetViewOptions()->IsReadonly() && \
        SwViewOption::IsDocBoundaries())
//Hilfslinien fuer Bereiche
#define IS_SUBS_SECTION (!pGlobalShell->GetViewOptions()->IsPagePreview() && \
                            !pGlobalShell->GetViewOptions()->IsReadonly()&&\
                            SwViewOption::IsSectionBoundaries())
#define IS_SUBS_FLYS (!pGlobalShell->GetViewOptions()->IsPagePreview() && \
                            !pGlobalShell->GetViewOptions()->IsReadonly()&&\
                                SwViewOption::IsObjectBoundaries())

#define SW_MAXBORDERCACHE 20

//Klassendeklarationen. Hier weil sie eben nur in diesem File benoetigt
//werden.

#define SUBCOL_PAGE     0x01    //Helplines of the page
#define SUBCOL_BREAK    0x02    //Helpline for a page or column break
#define SUBCOL_TAB      0x08    //Helplines inside tables
#define SUBCOL_FLY      0x10    //Helplines inside fly frames
#define SUBCOL_SECT     0x20    //Helplines inside sections

//----- Klassen zum Sammeln von Umrandungen und Hilfslinien ---
class SwLineRect : public SwRect
{
    const Color	   *pColor;
    const SwTabFrm *pTab;
          BYTE		nSubColor;  //Hilfslinien einfaerben
          BOOL		bPainted;	//schon gepaintet?
          BYTE		nLock;		//Um die Linien zum Hell-Layer abzugrenzen.
public:
    SwLineRect( const SwRect &rRect, const Color *pCol,
                const SwTabFrm *pT , const BYTE nSCol );

    const Color			*GetColor() const { return pColor;}
    const SwTabFrm		*GetTab()	const { return pTab;  }
    void  SetPainted()					  { bPainted = TRUE; }
    void  Lock( BOOL bLock )			  { if ( bLock )
                                                ++nLock;
                                            else if ( nLock )
                                                --nLock;
                                          }
    BOOL  IsPainted()				const { return bPainted; }
    BOOL  IsLocked()				const { return nLock != 0;  }
    BYTE  GetSubColor()				const { return nSubColor;}

    BOOL MakeUnion( const SwRect &rRect );
};

SV_DECL_VARARR( SwLRects, SwLineRect, 100, 100 );

class SwLineRects : public SwLRects
{
    USHORT nLastCount;	//unuetze Durchlaeufe im PaintLines verhindern.
public:
    SwLineRects() : nLastCount( 0 ) {}
    void AddLineRect( const SwRect& rRect,	const Color *pColor,
                      const SwTabFrm *pTab, const BYTE nSCol );
    void ConnectEdges( OutputDevice *pOut );
    void PaintLines  ( OutputDevice *pOut );
    void LockLines( BOOL bLock );

    /// OD 13.08.2002 - correct type of function
    const USHORT Free() const { return nFree; }
};

class SwSubsRects : public SwLineRects
{
    void RemoveSuperfluousSubsidiaryLines( const SwLineRects &rRects ); //;-)
public:
    void PaintSubsidiary( OutputDevice *pOut, const SwLineRects *pRects );

    inline void Ins( const SwRect &rRect, const BYTE nSCol );
};

//----------------- End Klassen Umrandungen ----------------------

static ViewShell *pGlobalShell = 0;

/// OD 05.09.2002 #102912# - <bPageOnly> no longer needed
/*
//Fuer PaintBackground, eigentlich lokal, aber fuer SwSavePaintStatics jetzt global
static FASTBOOL bPageOnly = FALSE;
*/

//Wenn durchsichtige FlyInCnts im PaintBackground gepainted werden so soll der
//Hintergrund nicht mehr retouchiert werden.
//static FASTBOOL bLockFlyBackground = FALSE;

//Wenn vom Fly ein Metafile abgezogen wird, so soll nur der FlyInhalt und vor
//nur hintergrund vom FlyInhalt gepaintet werden.
static FASTBOOL bFlyMetafile = FALSE;
static OutputDevice *pFlyMetafileOut = 0;

//Die Retouche fuer Durchsichtige Flys wird vom Hintergrund der Flys
//erledigt. Dabei darf der Fly selbst natuerlich nicht ausgespart werden.
//siehe PaintBackground und lcl_SubtractFlys()
static SwFlyFrm *pRetoucheFly  = 0;
static SwFlyFrm *pRetoucheFly2 = 0;

//Groesse eines Pixel und die Haelfte davon. Wird jeweils bei Eintritt in
//SwRootFrm::Paint neu gesetzt.
static long nPixelSzW = 0, nPixelSzH = 0;
static long nHalfPixelSzW = 0, nHalfPixelSzH = 0;
static long nMinDistPixelW = 0, nMinDistPixelH = 0;
static FASTBOOL  bPixelHeightOdd;
static FASTBOOL  bPixelWidthOdd;

//Aktueller Zoomfaktor
static double aScaleX = 1.0;
static double aScaleY = 1.0;
static double aMinDistScale = 0.73;
static double aEdgeScale = 0.5;

//In pLines werden Umrandungen waehrend des Paint gesammelt und soweit
//moeglich zusammengefasst.
//In pSubsLines werden Hilfslinien gesammelt und zusammengefasst. Diese
//werden vor der Ausgabe mit pLines abgeglichen, so dass moeglichst keine
//Umrandungen von den Hilfslinen verdeckt werden.
//bTablines ist waerend des Paints einer Tabelle TRUE.
static SwLineRects *pLines = 0;
static SwSubsRects *pSubsLines = 0;
// OD 18.11.2002 #99672# - global variable for sub-lines of body, header, footer,
// section and footnote frames.
static SwSubsRects *pSpecSubsLines = 0;

static SfxProgress *pProgress = 0;

//Nicht mehr als ein Beep pro Paint, wird auch im Textbereich benutzt!
FASTBOOL bOneBeepOnly = TRUE;

static SwFlyFrm *pFlyOnlyDraw = 0;

//Damit die Flys auch fuer den Hack richtig gepaintet werden koennen.
static FASTBOOL bTableHack = FALSE;

//Um das teure Ermitteln der RetoucheColor zu optimieren
Color aGlobalRetoucheColor;

//Statics fuer Umrandungsalignment setzen.
void SwCalcPixStatics( OutputDevice *pOut )
{
    Size aSz( pOut->PixelToLogic( Size( 1,1 )) );
    nPixelSzW = aSz.Width(), nPixelSzH = aSz.Height();
    if( !nPixelSzW )
        nPixelSzW = 1;
    if( !nPixelSzH )
        nPixelSzH = 1;
    nHalfPixelSzW = nPixelSzW / 2 + 1;
    nHalfPixelSzH = nPixelSzH / 2 + 1;
    nMinDistPixelW = nPixelSzW * 2 + 1;
    nMinDistPixelH = nPixelSzH * 2 + 1;
    bPixelHeightOdd = nPixelSzH % 2 ? TRUE : FALSE;
    bPixelWidthOdd	= nPixelSzW % 2 ? TRUE : FALSE;

    const MapMode &rMap = pOut->GetMapMode();
    aScaleX = rMap.GetScaleX();
    aScaleY = rMap.GetScaleY();
}

//Zum Sichern der statics, damit das Paint (quasi) reentrant wird.
class SwSavePaintStatics
{
//	FASTBOOL			bSLockFlyBackground,
    FASTBOOL			bSFlyMetafile,
                        bSPixelHeightOdd,
                        bSPixelWidthOdd,
                        bSOneBeepOnly,
                        bSPageOnly;
    ViewShell		   *pSGlobalShell;
    OutputDevice	   *pSFlyMetafileOut;
    SwFlyFrm		   *pSRetoucheFly,
                       *pSRetoucheFly2,
                       *pSFlyOnlyDraw;
    SwLineRects		   *pSLines;
    SwSubsRects		   *pSSubsLines;
    SfxProgress		   *pSProgress;
    long				nSPixelSzW,
                        nSPixelSzH,
                        nSHalfPixelSzW,
                        nSHalfPixelSzH,
                        nSMinDistPixelW,
                        nSMinDistPixelH;
    Color				aSGlobalRetoucheColor;
    double				aSScaleX,
                        aSScaleY;
public:
    SwSavePaintStatics();
    ~SwSavePaintStatics();
};

SwSavePaintStatics::SwSavePaintStatics() :
//	bSLockFlyBackground	( bLockFlyBackground),
    bSFlyMetafile       ( bFlyMetafile      ),
    bSPixelHeightOdd    ( bPixelHeightOdd	),
    bSPixelWidthOdd     ( bPixelWidthOdd    ),
    bSOneBeepOnly       ( bOneBeepOnly      ),
/// OD 05.09.2002 #102912# - <bPageOnly> no longer needed
//   bSPageOnly          ( bPageOnly         ),
    pSGlobalShell		( pGlobalShell		),
    pSFlyMetafileOut    ( pFlyMetafileOut   ),
    pSRetoucheFly       ( pRetoucheFly      ),
    pSRetoucheFly2      ( pRetoucheFly2     ),
    pSFlyOnlyDraw       ( pFlyOnlyDraw      ),
    pSLines             ( pLines            ),
    pSSubsLines         ( pSubsLines        ),
    pSProgress          ( pProgress         ),
    nSPixelSzW          ( nPixelSzW         ),
    nSPixelSzH          ( nPixelSzH         ),
    nSHalfPixelSzW      ( nHalfPixelSzW     ),
    nSHalfPixelSzH      ( nHalfPixelSzH     ),
    nSMinDistPixelW     ( nMinDistPixelW    ),
    nSMinDistPixelH     ( nMinDistPixelH    ),
    aSGlobalRetoucheColor( aGlobalRetoucheColor ),
    aSScaleX            ( aScaleX           ),
    aSScaleY            ( aScaleY           )
{
    /// OD 05.09.2002 #102912# - <bPageOnly> no longer needed
    /*bPageOnly = *//*bLockFlyBackground = */bFlyMetafile = FALSE;
    pFlyMetafileOut = 0;
    pRetoucheFly  = 0;
    pRetoucheFly2 = 0;
    nPixelSzW = nPixelSzH =
    nHalfPixelSzW = nHalfPixelSzH =
    nMinDistPixelW = nMinDistPixelH = 0;
    aScaleX = aScaleY = 1.0;
    aMinDistScale = 0.73;
    aEdgeScale = 0.5;
    pLines = 0;
    pSubsLines = 0;
    pProgress = 0;
    bOneBeepOnly = TRUE;
}

SwSavePaintStatics::~SwSavePaintStatics()
{
//	bLockFlyBackground = bSLockFlyBackground;
    pGlobalShell	   = pSGlobalShell;
    bFlyMetafile       = bSFlyMetafile;
    bPixelHeightOdd    = bSPixelHeightOdd;
    bPixelWidthOdd     = bSPixelWidthOdd;
    bOneBeepOnly       = bSOneBeepOnly;
/// OD 05.09.2002 #102912# - <bPageOnly> no longer needed
//   bPageOnly          = bSPageOnly;
    pFlyMetafileOut    = pSFlyMetafileOut;
    pRetoucheFly       = pSRetoucheFly;
    pRetoucheFly2      = pSRetoucheFly2;
    pFlyOnlyDraw       = pSFlyOnlyDraw;
    pLines             = pSLines;
    pSubsLines         = pSSubsLines;
    pProgress          = pSProgress;
    nPixelSzW          = nSPixelSzW;
    nPixelSzH          = nSPixelSzH;
    nHalfPixelSzW      = nSHalfPixelSzW;
    nHalfPixelSzH      = nSHalfPixelSzH;
    nMinDistPixelW     = nSMinDistPixelW;
    nMinDistPixelH     = nSMinDistPixelH;
    aGlobalRetoucheColor = aSGlobalRetoucheColor;
    aScaleX            = aSScaleX;
    aScaleY            = aSScaleY;
}

//----------------- Implementierungen fuer Tabellenumrandung --------------

SV_IMPL_VARARR( SwLRects, SwLineRect );


SwLineRect::SwLineRect( const SwRect &rRect, const Color *pCol,
                        const SwTabFrm *pT, const BYTE nSCol ) :
    SwRect( rRect ),
    pColor( pCol ),
    pTab( pT ),
    bPainted( FALSE ),
    nSubColor( nSCol ),
    nLock( 0 )
{
}

BOOL SwLineRect::MakeUnion( const SwRect &rRect )
{
    //Es wurde bereits ausserhalb geprueft, ob die Rechtecke die gleiche
    //Ausrichtung (horizontal bzw. vertikal), Farbe usw. besitzen.
    if ( Height() > Width() ) //Vertikale Linie
    {
        if ( Left()  == rRect.Left() && Width() == rRect.Width() )
        {
            //Zusammenfassen wenn kein Luecke zwischen den Linien ist.
            const long nAdd = nPixelSzW + nHalfPixelSzW;
            if ( Bottom() + nAdd >= rRect.Top() &&
                 Top()	  - nAdd <= rRect.Bottom()	)
            {
                Bottom( Max( Bottom(), rRect.Bottom() ) );
                Top   ( Min( Top(),	   rRect.Top()	  ) );
                return TRUE;
            }
        }
    }
    else
    {
        if ( Top()	== rRect.Top() && Height() == rRect.Height() )
        {
            //Zusammenfassen wenn kein Luecke zwischen den Linien ist.
            const long nAdd = nPixelSzW + nHalfPixelSzW;
            if ( Right() + nAdd >= rRect.Left() &&
                 Left()  - nAdd <= rRect.Right() )
            {
                Right( Max( Right(), rRect.Right() ) );
                Left ( Min( Left(),  rRect.Left()  ) );
                return TRUE;
            }
        }
    }
    return FALSE;
}

void SwLineRects::AddLineRect( const SwRect &rRect, const Color *pCol,
                               const SwTabFrm *pTab, const BYTE nSCol )
{
    //Rueckwaerts durch, weil Linien die zusammengefasst werden koennen i.d.R.
    //im gleichen Kontext gepaintet werden.
    for ( USHORT i = Count(); i ; )
    {
        SwLineRect &rLRect = operator[](--i);
        //Pruefen von Ausrichtung, Farbe, Tabelle.
        if ( rLRect.GetTab() == pTab &&
             !rLRect.IsPainted() && rLRect.GetSubColor() == nSCol &&
             (rLRect.Height() > rLRect.Width()) == (rRect.Height() > rRect.Width()) &&
             ((!rLRect.GetColor() && !pCol) ||
              (rLRect.GetColor() && pCol && *rLRect.GetColor() == *pCol)) )
        {
            if ( rLRect.MakeUnion( rRect ) )
                return;
        }
    }
    Insert( SwLineRect( rRect, pCol, pTab, nSCol ), Count() );
}

void SwLineRects::ConnectEdges( OutputDevice *pOut )
{
    if ( pOut->GetOutDevType() != OUTDEV_PRINTER )
    {
        //Fuer einen zu kleinen Zoom arbeite ich nicht.
        if ( aScaleX < aEdgeScale || aScaleY < aEdgeScale )
            return;
    }

    static const long nAdd = 20;

    SvPtrarr   aCheck( 64, 64 );

    for ( int i = 0; i < (int)Count(); ++i )
    {
        SwLineRect &rL1 = operator[](USHORT(i));
        if ( !rL1.GetTab() || rL1.IsPainted() || rL1.IsLocked() )
            continue;

        aCheck.Remove( 0, aCheck.Count() );

        const FASTBOOL bVert = rL1.Height() > rL1.Width();
        long nL1a, nL1b, nL1c, nL1d;

        if ( bVert )
        {
            nL1a = rL1.Top();   nL1b = rL1.Left();
            nL1c = rL1.Right(); nL1d = rL1.Bottom();
        }
        else
        {
            nL1a = rL1.Left();   nL1b = rL1.Top();
            nL1c = rL1.Bottom(); nL1d = rL1.Right();
        }

        //Alle moeglicherweise mit i1 zu verbindenden Linien einsammeln.
        for ( USHORT i2 = 0; i2 < Count(); ++i2 )
        {
            SwLineRect &rL2 = operator[](i2);
            if ( rL2.GetTab() != rL1.GetTab() ||
                 rL2.IsPainted() 			  ||
                 rL2.IsLocked()				  ||
                 bVert == rL2.Height() > rL2.Width() )
                continue;

            long nL2a, nL2b, nL2c, nL2d;
            if ( bVert )
            {
                nL2a = rL2.Top();   nL2b = rL2.Left();
                nL2c = rL2.Right(); nL2d = rL2.Bottom();
            }
            else
            {
                nL2a = rL2.Left();   nL2b = rL2.Top();
                nL2c = rL2.Bottom(); nL2d = rL2.Right();
            }

            if ( (nL1a - nAdd < nL2d && nL1d + nAdd > nL2a) &&
                  ((nL1b >	nL2b && nL1c		< nL2c) ||
                   (nL1c >= nL2c && nL1b - nAdd < nL2c) ||
                   (nL1b <= nL2b && nL1c + nAdd > nL2b)) )
            {
                SwLineRect *pMSC = &rL2;
                aCheck.Insert( (void*&)pMSC, aCheck.Count() );
            }
        }
        if ( aCheck.Count() < 2 )
            continue;

        FASTBOOL bRemove = FALSE;

        //Fuer jede Linie jede alle folgenden checken.
        for ( USHORT k = 0; !bRemove && k < aCheck.Count(); ++k )
        {
            SwLineRect &rR1 = (SwLineRect&)*(SwLineRect*)aCheck[k];

            for ( USHORT k2 = k+1; !bRemove && k2 < aCheck.Count(); ++k2 )
            {
                SwLineRect &rR2 = (SwLineRect&)*(SwLineRect*)aCheck[k2];
                if ( bVert )
                {
                    SwLineRect *pLA = 0, *pLB;
                    if ( rR1.Top() < rR2.Top() )
                    {
                        pLA = &rR1; pLB = &rR2;
                    }
                    else if ( rR1.Top() > rR2.Top() )
                    {
                        pLA = &rR2; pLB = &rR1;
                    }
                    //beschreiben k1 und k2 eine Doppellinie?
                    if ( pLA && pLA->Bottom() + 60 > pLB->Top() )
                    {
                        if ( rL1.Top() < pLA->Top() )
                        {
                            if ( rL1.Bottom() == pLA->Bottom() )
                                continue;	//kleiner Irrtum (woher?)

                            SwRect aIns( rL1 );
                            aIns.Bottom( pLA->Bottom() );
                            if ( !rL1.IsInside( aIns ) )
                                continue;
                            const USHORT nFree = Free();
                            Insert( SwLineRect( aIns, rL1.GetColor(),
                                        rL1.GetTab(), SUBCOL_TAB ), Count() );
                            if ( !nFree )
                            {
                                --i;
                                k = aCheck.Count();
                                break;
                            }
                        }

                        if ( rL1.Bottom() > pLB->Bottom() )
                            rL1.Top( pLB->Top() );	//i1 nach oben verlaengern
                        else
                            bRemove = TRUE;			//abbrechen, i1 entfernen
                    }
                }
                else
                {
                    SwLineRect *pLA = 0, *pLB;
                    if ( rR1.Left() < rR2.Left() )
                    {
                        pLA = &rR1; pLB = &rR2;
                    }
                    else if ( rR1.Left() > rR2.Left() )
                    {
                        pLA = &rR2; pLB = &rR1;
                    }
                    //Liegt eine 'doppellinie' vor?
                    if ( pLA && pLA->Right() + 60 > pLB->Left() )
                    {
                        if ( rL1.Left() < pLA->Left() )
                        {
                            if ( rL1.Right() == pLA->Right() )
                                continue;	//kleiner irrtum

                            SwRect aIns( rL1 );
                            aIns.Right( pLA->Right() );
                            if ( !rL1.IsInside( aIns ) )
                                continue;
                            const USHORT nFree = Free();
                            Insert( SwLineRect( aIns, rL1.GetColor(),
                                        rL1.GetTab(), SUBCOL_TAB ), Count() );
                            if ( !nFree )
                            {
                                --i;
                                k = aCheck.Count();
                                break;
                            }
                        }
                        if ( rL1.Right() > pLB->Right() )
                            rL1.Left( pLB->Left() );
                        else
                            bRemove = TRUE;
                    }
                }
            }
        }
        if ( bRemove )
        {
            Remove( i, 1 );
            --i;			//keinen auslassen!
        }
    }
}

inline void SwSubsRects::Ins( const SwRect &rRect, const BYTE nSCol )
{
    //Linien die kuerzer als die breiteste Linienbreite sind werden
    //nicht aufgenommen.
    if ( rRect.Height() > DEF_LINE_WIDTH_4 || rRect.Width() > DEF_LINE_WIDTH_4 )
        Insert( SwLineRect( rRect, 0, 0, nSCol ), Count());
}

void SwSubsRects::RemoveSuperfluousSubsidiaryLines( const SwLineRects &rRects )
{
    //Alle Hilfslinien, die sich mit irgendwelchen Umrandungen decken werden
    //entfernt bzw. zerstueckelt..
    for ( USHORT i = 0; i < Count(); ++i )
    {
        // OD 18.11.2002 #99672# - get a copy instead of a reference, because
        // an <insert> may destroy the object due to a necessary array resize.
        const SwLineRect aSubsLineRect = SwLineRect( operator[](i) );

        if ( aSubsLineRect.IsPainted() )
            continue;

        const bool bVerticalSubs = aSubsLineRect.Height() > aSubsLineRect.Width();
        SwRect aSubsRect( aSubsLineRect );
        if ( bVerticalSubs )
        {
            aSubsRect.Left  ( aSubsRect.Left()  - (nPixelSzW+nHalfPixelSzW) );
            aSubsRect.Right ( aSubsRect.Right() + (nPixelSzW+nHalfPixelSzW) );
        }
        else
        {
            aSubsRect.Top   ( aSubsRect.Top()    - (nPixelSzH+nHalfPixelSzH) );
            aSubsRect.Bottom( aSubsRect.Bottom() + (nPixelSzH+nHalfPixelSzH) );
        }
        for ( USHORT k = 0; k < rRects.Count(); ++k )
        {
            SwLineRect &rLine = rRects[k];

            if ( !bVerticalSubs == rLine.Height() > rLine.Width() ) //gleiche Ausrichtung?
                continue;

            if ( aSubsRect.IsOver( rLine ) )
            {
                if ( bVerticalSubs ) //Vertikal?
                {
                    if ( aSubsRect.Left()  <= rLine.Right() &&
                         aSubsRect.Right() >= rLine.Left() )
                    {
                        long nTmp = rLine.Top()-(nPixelSzH+1);
                        if ( aSubsLineRect.Top() < nTmp )
                        {
                            SwRect aNewSubsRect( aSubsLineRect );
                            aNewSubsRect.Bottom( nTmp );
                            Insert( SwLineRect( aNewSubsRect, 0, 0,
                                                aSubsLineRect.GetSubColor() ), Count());
                        }
                        nTmp = rLine.Bottom()+nPixelSzH+1;
                        if ( aSubsLineRect.Bottom() > nTmp )
                        {
                            SwRect aNewSubsRect( aSubsLineRect );
                            aNewSubsRect.Top( nTmp );
                            Insert( SwLineRect( aNewSubsRect, 0, 0,
                                                aSubsLineRect.GetSubColor() ), Count());
                        }
                        Remove( i, 1 );
                        --i;
                        break;
                    }
                }
                else									//Horizontal
                {
                    if ( aSubsRect.Top() <= rLine.Bottom() &&
                         aSubsRect.Bottom() >= rLine.Top() )
                    {
                        long nTmp = rLine.Left()-(nPixelSzW+1);
                        if ( aSubsLineRect.Left() < nTmp )
                        {
                            SwRect aNewSubsRect( aSubsLineRect );
                            aNewSubsRect.Right( nTmp );
                            Insert( SwLineRect( aNewSubsRect, 0, 0,
                                                aSubsLineRect.GetSubColor() ), Count());
                        }
                        nTmp = rLine.Right()+nPixelSzW+1;
                        if ( aSubsLineRect.Right() > nTmp )
                        {
                            SwRect aNewSubsRect( aSubsLineRect );
                            aNewSubsRect.Left( nTmp );
                            Insert( SwLineRect( aNewSubsRect, 0, 0,
                                                aSubsLineRect.GetSubColor() ), Count());
                        }
                        Remove( i, 1 );
                        --i;
                        break;
                    }
                }
            }
        }
    }
}

void SwLineRects::LockLines( BOOL bLock )
{
    for ( USHORT i = 0; i < Count(); ++i )
        operator[](i).Lock( bLock );
}

void SwLineRects::PaintLines( OutputDevice *pOut )
{
    //Painten der Umrandungen. Leider muessen wir zweimal durch.
    //Einmal fuer die innenliegenden und einmal fuer die Aussenkanten
    //der Tabellen.
    if ( Count() != nLastCount )
    {
        pOut->Push( PUSH_FILLCOLOR );

        ConnectEdges( pOut );
        const Color *pLast = 0;

        FASTBOOL bPaint2nd = FALSE;
        USHORT nMinCount = Count();
        for ( USHORT i = 0; i < Count(); ++i )
        {
            SwLineRect &rLRect = operator[](i);

            if ( rLRect.IsPainted() )
                continue;

            if ( rLRect.IsLocked() )
            {
                nMinCount = Min( nMinCount, i );
                continue;
            }

            //Jetzt malen oder erst in der zweiten Runde?
            FASTBOOL bPaint = TRUE;
            if ( rLRect.GetTab() )
            {
                if ( rLRect.Height() > rLRect.Width() )
                {
                    //Senkrechte Kante, ueberlappt sie mit der TabellenKante?
                    SwTwips nLLeft	= rLRect.Left()  - 30,
                            nLRight = rLRect.Right() + 30,
                            nTLeft	= rLRect.GetTab()->Frm().Left() + rLRect.GetTab()->Prt().Left(),
                            nTRight = rLRect.GetTab()->Frm().Left() + rLRect.GetTab()->Prt().Right();
                    if ( (nTLeft >= nLLeft && nTLeft <= nLRight) ||
                         (nTRight>= nLLeft && nTRight<= nLRight) )
                        bPaint = FALSE;
                }
                else
                {	//Waagerechte Kante, ueberlappt sie mit der Tabellenkante?
                    SwTwips nLTop	 = rLRect.Top()    - 30,
                            nLBottom = rLRect.Bottom() + 30,
                            nTTop	 = rLRect.GetTab()->Frm().Top()  + rLRect.GetTab()->Prt().Top(),
                            nTBottom = rLRect.GetTab()->Frm().Top()  + rLRect.GetTab()->Prt().Bottom();
                    if ( (nTTop    >= nLTop && nTTop	  <= nLBottom) ||
                         (nTBottom >= nLTop && nTBottom <= nLBottom) )
                        bPaint = FALSE;
                }
            }
            if ( bPaint )
            {
                if ( !pLast || *pLast != *rLRect.GetColor() )
                {
                    pLast = rLRect.GetColor();

                    ULONG nOldDrawMode = pOut->GetDrawMode();
                    if( pGlobalShell->GetWin() &&
                        Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
                        pOut->SetDrawMode( 0 );

                    pOut->SetFillColor( *pLast );
                    pOut->SetDrawMode( nOldDrawMode );
                }
                if( !rLRect.IsEmpty() )
                    pOut->DrawRect( rLRect.SVRect() );
                rLRect.SetPainted();
            }
            else
                bPaint2nd = TRUE;
        }
        if ( bPaint2nd )
            for ( i = 0; i < Count(); ++i )
            {
                SwLineRect &rLRect = operator[](i);
                if ( rLRect.IsPainted() )
                    continue;

                if ( rLRect.IsLocked() )
                {
                    nMinCount = Min( nMinCount, i );
                    continue;
                }

                if ( !pLast || *pLast != *rLRect.GetColor() )
                {
                    pLast = rLRect.GetColor();

                    ULONG nOldDrawMode = pOut->GetDrawMode();
                    if( pGlobalShell->GetWin() &&
                        Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
                    {
                        pOut->SetDrawMode( 0 );
                    }

                    pOut->SetFillColor( *pLast );
                    pOut->SetDrawMode( nOldDrawMode );
                }
                if( !rLRect.IsEmpty() )
                    pOut->DrawRect( rLRect.SVRect() );
                rLRect.SetPainted();
            }
        nLastCount = nMinCount;
        pOut->Pop();
    }
}

void SwSubsRects::PaintSubsidiary( OutputDevice *pOut,
                                   const SwLineRects *pRects )
{
    if ( Count() )
    {
        //Alle Hilfslinien, die sich fast decken entfernen (Tabellen)
        for ( USHORT i = 0; i < Count(); ++i )
        {
            SwLineRect &rLi = operator[](i);
            const bool bVerticalSubs = rLi.Height() > rLi.Width();

            for ( USHORT k = i+1; k < Count(); ++k )
            {
                SwLineRect &rLk = operator[](k);
                if ( rLi.SSize() == rLk.SSize() )
                {
                    if ( bVerticalSubs == rLk.Height() > rLk.Width() )
                    {
                        if ( bVerticalSubs )
                        {
                            long nLi = rLi.Right();
                            long nLk = rLk.Right();
                            if ( rLi.Top() == rLk.Top() &&
                                 ((nLi < rLk.Left() && nLi+21 > rLk.Left()) ||
                                  (nLk < rLi.Left() && nLk+21 > rLi.Left())))
                            {
                                Remove( k, 1 );
                                //Nicht mit der inneren Schleife weiter, weil
                                //das Array schrumpfen koennte!
                                --i; k = Count();
                            }
                        }
                        else
                        {
                            long nLi = rLi.Bottom();
                            long nLk = rLk.Bottom();
                            if ( rLi.Left() == rLk.Left() &&
                                 ((nLi < rLk.Top() && nLi+21 > rLk.Top()) ||
                                  (nLk < rLi.Top() && nLk+21 > rLi.Top())))
                            {
                                Remove( k, 1 );
                                --i; k = Count();
                            }
                        }
                    }
                }
            }
        }


        if ( pRects && pRects->Count() )
            RemoveSuperfluousSubsidiaryLines( *pRects );

        if ( Count() )
        {
            pOut->Push( PUSH_FILLCOLOR );

            for ( USHORT i = 0; i < Count(); ++i )
            {
                SwLineRect &rLRect = operator[](i);
                if ( !rLRect.IsPainted() )
                {
                    const Color *pCol;
                    switch ( rLRect.GetSubColor() )
                    {
                        case SUBCOL_PAGE: pCol = &SwViewOption::GetDocBoundariesColor(); break;
                        case SUBCOL_FLY: pCol = &SwViewOption::GetObjectBoundariesColor(); break;
                        case SUBCOL_TAB: pCol = &SwViewOption::GetTableBoundariesColor(); break;
                        case SUBCOL_SECT: pCol = &SwViewOption::GetSectionBoundColor(); break;
                        case SUBCOL_BREAK:    pCol = &SwViewOption::GetPageBreakColor(); break;
                    }
                    if ( pOut->GetFillColor() != *pCol )
                        pOut->SetFillColor( *pCol );
                    pOut->DrawRect( rLRect.SVRect() );
                    rLRect.SetPainted();
                }
            }
            pOut->Pop();
        }
    }
}

//-------------------------------------------------------------------------
//Diverse Functions die in diesem File so verwendet werden.

void MA_FASTCALL SwAlignRect( SwRect &rRect, ViewShell *pSh )
{
    if( !rRect.HasArea() )
        return;

    /// OD 03.09.2002 #102450#
    /// Assure that view shell (parameter <pSh>) exists, if the output device
    /// is taken from this view shell --> no output device, no alignment.
    /// Output device taken from view shell <pSh>, if <bFlyMetafile> not set.
    if ( !bFlyMetafile && !pSh )
    {
        return;
    }

    const OutputDevice *pOut = bFlyMetafile ?
                        pFlyMetafileOut : pSh->GetOut();
    Rectangle aTmp( rRect.SVRect() );
    //Bei ungeraden Pixelwerten korrigieren.
    if ( bPixelHeightOdd )
    {
        aTmp.Top()	  += 1;
        aTmp.Bottom() -= 1;
    }
    if ( bPixelWidthOdd )
    {
        aTmp.Left()	 += 1;
        aTmp.Right() -= 1;
    }
    aTmp = pOut->PixelToLogic( pOut->LogicToPixel( aTmp ) );

    SwRect aRect( aTmp );

    if ( rRect.Top() > aRect.Top() )
        rRect.Top( aRect.Top() + nHalfPixelSzH );
    else
        rRect.Top( aRect.Top() );

    if ( rRect.Bottom() < aRect.Bottom() )
        rRect.Bottom( aRect.Bottom() - nHalfPixelSzH );
    else
        rRect.Bottom( aRect.Bottom() );

    if ( rRect.Left() > aRect.Left() )
        rRect.Left( aRect.Left() + nHalfPixelSzW );
    else
        rRect.Left( aRect.Left() );

    if ( rRect.Right() < aRect.Right() )
        rRect.Right( aRect.Right() - nHalfPixelSzW );
    else
        rRect.Right( aRect.Right() );

    /// OD 11.10.2002 #103636# - consider negative width/height
    /// check, if aligned SwRect has negative width/height.
    /// If Yes, adjust it to width/height = 0 twip.
    /// NOTE: A SwRect with negative width/height can occur, if the width/height
    ///     of the given SwRect in twip was less than a pixel in twip and that
    ///     the alignment calculates that the aligned SwRect should not contain
    ///     the pixels the width/height is on.
    if ( rRect.Width() < 0 )
    {
        rRect.Width(0);
    }
    if ( rRect.Height() < 0 )
    {
        rRect.Height(0);
    }
}

/** OD 25.09.2002 #99739# - method to pixel-align rectangle for drawing graphic object

    Because for drawing a graphic left-top-corner and size coordinations are
    used, these coordinations have to be determined on pixel level.
    Thus, convert rectangle to pixel and then convert left-top-corner and
    size of pixel rectangle back to logic.
    This calculation is necessary, because there exists a different between
    the convert from logic to pixel of a normal rectangle with its left-top-
    and right-bottom-corner and the same convert of the same rectangle
    with left-top-corner and size.
    Call this method before each <GraphicObject.Draw(...)>

    @author OD
*/
void SwAlignGrfRect( SwRect *pGrfRect, const OutputDevice &rOut )
{
    Rectangle aPxRect = rOut.LogicToPixel( pGrfRect->SVRect() );
    pGrfRect->Pos( rOut.PixelToLogic( aPxRect.TopLeft() ) );
    pGrfRect->SSize( rOut.PixelToLogic( aPxRect.GetSize() ) );
}

long MA_FASTCALL lcl_AlignWidth( const long nWidth )
{
    if ( nWidth )
    {
        const long nW = nWidth % nPixelSzW;

        if ( !nW || nW > nHalfPixelSzW )
            return Max(1L, nWidth - nHalfPixelSzW);
    }
    return nWidth;
}

long MA_FASTCALL lcl_AlignHeight( const long nHeight )
{
    if ( nHeight )
    {
        const long nH = nHeight % nPixelSzH;

        if ( !nH || nH > nHalfPixelSzH )
            return Max(1L, nHeight - nHalfPixelSzH);
    }
    return nHeight;
}

long MA_FASTCALL lcl_MinHeightDist( const long nDist )
{
    if ( aScaleX < aMinDistScale || aScaleY < aMinDistScale )
        return nDist;
    return ::lcl_AlignHeight( Max( nDist, nMinDistPixelH ));
}

long MA_FASTCALL lcl_MinWidthDist( const long nDist )
{
    if ( aScaleX < aMinDistScale || aScaleY < aMinDistScale )
        return nDist;
    return ::lcl_AlignWidth( Max( nDist, nMinDistPixelW ));
}


//Ermittelt PrtArea plus Umrandung plus Schatten.
void MA_FASTCALL lcl_CalcBorderRect( SwRect &rRect, const SwFrm *pFrm,
                                        const SwBorderAttrs &rAttrs,
                                        const BOOL bShadow )
{
    if( pFrm->IsSctFrm() )
        rRect = pFrm->Frm();
    else
    {
        rRect = pFrm->Prt();
        rRect.Pos() += pFrm->Frm().Pos();

        if ( rAttrs.IsLine() || rAttrs.IsBorderDist() ||
             (bShadow && rAttrs.GetShadow().GetLocation() != SVX_SHADOW_NONE) )
        {
            SwRectFn fnRect = pFrm->IsVertical() ? fnRectVert : fnRectHori;
            const SvxBoxItem &rBox = rAttrs.GetBox();
            const FASTBOOL bTop = 0 != (pFrm->*fnRect->fnGetTopMargin)();
            if ( bTop )
            {
                SwTwips nDiff = rBox.GetTop() ?
                    rBox.CalcLineSpace( BOX_LINE_TOP ) :
                    ( rAttrs.IsBorderDist() ?
                      rBox.GetDistance( BOX_LINE_TOP ) + 1 : 0 );
                if( nDiff )
                    (rRect.*fnRect->fnSubTop)( nDiff );
            }

            const FASTBOOL bBottom = 0 != (pFrm->*fnRect->fnGetBottomMargin)();
            if ( bBottom )
            {
                SwTwips nDiff = rBox.GetBottom() ?
                    rBox.CalcLineSpace( BOX_LINE_BOTTOM ) :
                    ( rAttrs.IsBorderDist() ?
                      rBox.GetDistance( BOX_LINE_BOTTOM ) + 1 : 0 );
                if( nDiff )
                    (rRect.*fnRect->fnAddBottom)( nDiff );
            }

            if ( rBox.GetLeft() )
                (rRect.*fnRect->fnSubLeft)( rBox.CalcLineSpace(BOX_LINE_LEFT) );
            else if ( rAttrs.IsBorderDist() )
                (rRect.*fnRect->fnSubLeft)(rBox.GetDistance( BOX_LINE_LEFT )+1);

            if ( rBox.GetRight() )
                (rRect.*fnRect->fnAddRight)(rBox.CalcLineSpace(BOX_LINE_RIGHT));
            else if ( rAttrs.IsBorderDist() )
                (rRect.*fnRect->fnAddRight)(rBox.GetDistance(BOX_LINE_RIGHT)+1);

            if ( bShadow && rAttrs.GetShadow().GetLocation() != SVX_SHADOW_NONE )
            {
                const SvxShadowItem &rShadow = rAttrs.GetShadow();
                if ( bTop )
                    (rRect.*fnRect->fnSubTop)(rShadow.CalcShadowSpace(SHADOW_TOP));
                (rRect.*fnRect->fnSubLeft)(rShadow.CalcShadowSpace(SHADOW_LEFT));
                if ( bBottom )
                    (rRect.*fnRect->fnAddBottom)
                                    (rShadow.CalcShadowSpace( SHADOW_BOTTOM ));
                (rRect.*fnRect->fnAddRight)(rShadow.CalcShadowSpace(SHADOW_RIGHT));
            }
        }
    }

    ::SwAlignRect( rRect, pGlobalShell );
}

void MA_FASTCALL lcl_ExtendLeftAndRight( SwRect &rRect, const SwFrm *pFrm,
                          const SwBorderAttrs &rAttrs, SwRectFn& rRectFn )
{
    //In der Hoehe aufbohren wenn TopLine bzw. BottomLine entfallen.
    if ( rAttrs.GetBox().GetTop() && !rAttrs.GetTopLine( pFrm ) )
    {
        const SwFrm *pPre = pFrm->GetPrev();
        (rRect.*rRectFn->fnSetTop)( (pPre->*rRectFn->fnGetPrtBottom)() );
    }
    if ( rAttrs.GetBox().GetBottom() && !rAttrs.GetBottomLine( pFrm ) )
    {
        const SwFrm *pNxt = pFrm->GetNext();
        (rRect.*rRectFn->fnSetBottom)( (pNxt->*rRectFn->fnGetPrtTop)() );
    }
}


void MA_FASTCALL lcl_SubtractFlys( const SwFrm *pFrm, const SwPageFrm *pPage,
                           const SwRect &rRect, SwRegionRects &rRegion )
{
    const SwSortDrawObjs &rObjs = *pPage->GetSortedObjs();
    const SwFlyFrm *pSelfFly = pFrm->IsInFly() ? pFrm->FindFlyFrm() : pRetoucheFly2;
    if ( !pRetoucheFly )
        pRetoucheFly = pRetoucheFly2;

    for ( USHORT j = 0; (j < rObjs.Count()) && rRegion.Count(); ++j )
    {
        SdrObject *pO = rObjs[j];
        if ( !pO->IsWriterFlyFrame() )
            continue;

        const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();

        if ( pSelfFly == pFly || pRetoucheFly == pFly || !rRect.IsOver( pFly->Frm() ) )
            continue;

        if ( !pFly->GetFmt()->GetPrint().GetValue() &&
                (OUTDEV_PRINTER == pGlobalShell->GetOut()->GetOutDevType() ||
                pGlobalShell->IsPreView()))
            continue;

        const FASTBOOL bLowerOfSelf = pSelfFly && pFly->IsLowerOf( pSelfFly ) ?
                                            TRUE : FALSE;

        //Bei zeichengebundenem Fly nur diejenigen betrachten, in denen er
        //nicht selbst verankert ist.
        //#33429# Warum nur bei zeichengebundenen? Es macht doch nie Sinn
        //Rahmen abzuziehen in denen er selbst verankert ist oder?
        if ( pSelfFly && pSelfFly->IsLowerOf( pFly ) )
            continue;

        //#57194# Und warum gilt das nicht analog fuer den RetoucheFly?
        if ( pRetoucheFly && pRetoucheFly->IsLowerOf( pFly ) )
            continue;


#ifndef PRODUCT
        //Flys, die innerhalb des eigenen verankert sind, muessen eine
        //groessere OrdNum haben oder Zeichengebunden sein.
        if ( pSelfFly && bLowerOfSelf )
        {
            ASSERT( pFly->IsFlyInCntFrm() ||
                    pO->GetOrdNumDirect() > pSelfFly->GetVirtDrawObj()->GetOrdNumDirect(),
                    "Fly with wrong z-Order" );
        }
#endif

        BOOL bStopOnHell = TRUE;
        if ( pSelfFly )
        {
            const SdrObject *pTmp = pSelfFly->GetVirtDrawObj();
            if ( pO->GetLayer() == pTmp->GetLayer() )
            {
                if ( pO->GetOrdNumDirect() < pTmp->GetOrdNumDirect() )
                    //Im gleichen Layer werden nur obenliegende beachtet.
                    continue;
            }
            else
            {
                if ( !bLowerOfSelf && !pFly->GetFmt()->GetOpaque().GetValue() )
                    //Aus anderem Layer interessieren uns nur nicht transparente
                    //oder innenliegende
                    continue;
                bStopOnHell = FALSE;
            }
        }
        if ( pRetoucheFly )
        {
            const SdrObject *pTmp = pRetoucheFly->GetVirtDrawObj();
            if ( pO->GetLayer() == pTmp->GetLayer() )
            {
                if ( pO->GetOrdNumDirect() < pTmp->GetOrdNumDirect() )
                    //Im gleichen Layer werden nur obenliegende beachtet.
                    continue;
            }
            else
            {
                if ( !pFly->IsLowerOf( pRetoucheFly ) && !pFly->GetFmt()->GetOpaque().GetValue() )
                    //Aus anderem Layer interessieren uns nur nicht transparente
                    //oder innenliegende
                    continue;
                bStopOnHell = FALSE;
            }
        }

        //Wenn der Inhalt des Fly Transparent ist, wird er nicht abgezogen, es sei denn
        //er steht im Hell-Layer (#31941#)
        BOOL bHell = pO->GetLayer() == pFly->GetFmt()->GetDoc()->GetHellId();
        if ( (bStopOnHell && bHell) ||
             /// OD 05.08.2002 - change internal order of condition
             ///    first check "!bHell", then "..->Lower()" and "..->IsNoTxtFrm()"
             ///    have not to be performed, if frame is in "Hell"
             ( !bHell && pFly->Lower() && pFly->Lower()->IsNoTxtFrm() &&
               ( ((SwNoTxtFrm*)pFly->Lower())->IsTransparent() ||
                 ((SwNoTxtFrm*)pFly->Lower())->HasAnimation() ||
                 pFly->GetFmt()->GetSurround().IsContour()
               )
             )
           )
            continue;

        /// OD 08.10.2002 #103898#
        /// Own if-statements for transparent background/shadow of fly frames
        /// (#99657#) in order to handle special conditions.
        if ( pFly->IsBackgroundTransparent() )
        {
            /// Background <pFly> is transparent drawn. Thus normally, its region
            /// have not to be substracted from given region.
            /// But, if method is called for a fly frame and
            /// <pFly> is a direct lower of this fly frame and
            /// <pFly> inherites its transparent background brush from its parent,
            /// then <pFly> frame area have to be subtracted from given region.
            /// NOTE: Because in Status Quo transparent backgrounds can only be
            ///     assigned to fly frames, the handle of this special case
            ///     avoids drawing of transparent areas more than once, if
            ///     a fly frame inherites a transparent background from its
            ///     parent fly frame.
            if ( pFrm->IsFlyFrm() &&
                 (pFly->GetAnchor()->FindFlyFrm() == pFrm) &&
                 static_cast<const SwFlyFrmFmt*>(pFly->GetFmt())->IsBackgroundBrushInherited()
               )
            {
                SwRect aRect;
                SwBorderAttrAccess aAccess( SwFrm::GetCache(), (SwFrm*)pFly );
                const SwBorderAttrs &rAttrs = *aAccess.Get();
                ::lcl_CalcBorderRect( aRect, pFly, rAttrs, TRUE );
                rRegion -= aRect;
                continue;
            }
            else
            {
                continue;
            }
        }
        if ( pFly->IsShadowTransparent() )
        {
            continue;
        }

        if ( bHell && pFly->GetAnchor()->IsInFly() )
        {
            //Damit die Umrandung nicht vom Hintergrund des anderen Flys
            //zerlegt wird.
            SwRect aRect;
            SwBorderAttrAccess aAccess( SwFrm::GetCache(), (SwFrm*)pFly );
            const SwBorderAttrs &rAttrs = *aAccess.Get();
            ::lcl_CalcBorderRect( aRect, pFly, rAttrs, TRUE );
            rRegion -= aRect;
        }
        else
        {
            SwRect aRect( pFly->Prt() );
            aRect += pFly->Frm().Pos();
            rRegion -= aRect;
        }
    }
    if ( pRetoucheFly == pRetoucheFly2 )
        pRetoucheFly = 0;
}

inline FASTBOOL IsShortCut( const SwRect &rRect, const SwRect &rFrmRect )
{
    //Wenn der Frm vollstaendig rechts neben bzw. unter dem
    //Rect sitzt ist's genug mit Painten.
    return ( (rFrmRect.Top()  > rRect.Bottom()) ||
             (rFrmRect.Left() > rRect.Right()) );
}


void lcl_PaintShadow( const SwRect& aFrm, ViewShell* pSh )
{
    const Color& rColor = SwViewOption::GetFontColor();
    Color aFill( pSh->GetOut()->GetFillColor() );
    Color aLine( pSh->GetOut()->GetLineColor() );
    pSh->GetOut()->SetFillColor( Color( COL_TRANSPARENT ) );
    pSh->GetOut()->SetLineColor( rColor );
    Rectangle aPageRect( aFrm.SVRect() );
    pSh->GetOut()->DrawRect( aPageRect );
    aPageRect = pSh->GetOut()->LogicToPixel( aPageRect );
    Rectangle aShadow( aPageRect.Left()+2, aPageRect.Bottom()+1,
                        aPageRect.Right()+2, aPageRect.Bottom()+2 );
    pSh->GetOut()->SetFillColor( rColor );
    aShadow = pSh->GetOut()->PixelToLogic( aShadow );
    pSh->GetOut()->DrawRect( aShadow );
    aShadow.Left() = aPageRect.Right() + 1;
    aShadow.Right() = aShadow.Left() + 1;
    aShadow.Top() = aPageRect.Top() + 2;
    aShadow.Bottom() = aPageRect.Bottom();
    aShadow = pSh->GetOut()->PixelToLogic( aShadow );
    pSh->GetOut()->DrawRect( aShadow );
    pSh->GetOut()->SetFillColor( aFill );
    pSh->GetOut()->SetLineColor( aLine );
}

//---------------- Ausgabe fuer das BrushItem ----------------

/** lcl_DrawGraphicBackgrd - local help method to draw a background for a graphic

    OD 17.10.2002 #103876#
    Under certain circumstances we have to draw a background for a graphic.
    This method takes care of the conditions and draws the background with the
    corresponding color.
    Method introduced for bug fix #103876# in order to optimize drawing tiled
    background graphics. Previously, this code was integrated in method
    <lcl_DrawGraphic>.
    Method implemented as a inline, checking the conditions and calling method
    method <lcl_implDrawGraphicBackgrd(..)> for the intrinsic drawing.

    @author OD

    @param _rBackgrdBrush
    background brush contain the color the background has to be drawn.

    @param _pOut
    output device the background has to be drawn in.

    @param _rPaintRect
    paint retangle in the output device, which has to be drawn with the background.
    rectangle have to be aligned by method ::SwAlignRect

    @param _rGraphicObj
    graphic object, for which the background has to be drawn. Used for checking
    the transparency of its bitmap, its type and if the graphic is drawn transparent

    @param _bNumberingGraphic
    boolean indicating that graphic is used as a numbering.

    @param _bBackgrdAlreadyDrawn
    boolean (optional; default: false) indicating, if the background is already drawn.
*/
void lcl_implDrawGraphicBackgrd( const SvxBrushItem& _rBackgrdBrush,
                                 OutputDevice* _pOut,
                                 const SwRect& _rAlignedPaintRect,
                                 const GraphicObject& _rGraphicObj )
{
    /// determine color of background
    ///     If color of background brush is not "no fill"/"auto fill" or
    ///     <bFlyMetafile> is set, use color of background brush, otherwise
    ///     use global retouche color.
    const Color aColor( ( (_rBackgrdBrush.GetColor() != COL_TRANSPARENT) || bFlyMetafile )
                        ? _rBackgrdBrush.GetColor()
                        : aGlobalRetoucheColor );

    /// determine, if background color have to be drawn transparent
    /// and calculate transparency percent value
    sal_Int8 nTransparencyPercent = 0;
    bool bDrawTransparent = false;
    if ( aColor.GetTransparency() != 0 )
    ///     background color is transparent --> draw transparent.
    {
        bDrawTransparent = true;
        nTransparencyPercent = (aColor.GetTransparency()*100 + 0x7F)/0xFF;
    }
    else if ( (_rGraphicObj.GetAttr().GetTransparency() != 0) &&
                (_rBackgrdBrush.GetColor() == COL_TRANSPARENT) )
    ///     graphic is drawn transparent and background color is
    ///     "no fill"/"auto fill" --> draw transparent
    {
        bDrawTransparent = true;
        nTransparencyPercent = (_rGraphicObj.GetAttr().GetTransparency()*100 + 0x7F)/0xFF;
    }

    if ( bDrawTransparent )
    {
        /// draw background transparent
        if( _pOut->GetFillColor() != aColor.GetRGBColor() )
            _pOut->SetFillColor( aColor.GetRGBColor() );
        PolyPolygon aPoly( _rAlignedPaintRect.SVRect() );
        _pOut->DrawTransparent( aPoly, nTransparencyPercent );
    }
    else
    {
        /// draw background opaque
        if ( _pOut->GetFillColor() != aColor )
            _pOut->SetFillColor( aColor );
        _pOut->DrawRect( _rAlignedPaintRect.SVRect() );
    }
}

inline void lcl_DrawGraphicBackgrd( const SvxBrushItem& _rBackgrdBrush,
                                    OutputDevice* _pOut,
                                    const SwRect& _rAlignedPaintRect,
                                    const GraphicObject& _rGraphicObj,
                                    bool _bNumberingGraphic,
                                    bool _bBackgrdAlreadyDrawn = false )
{
    /// draw background with background color, if
    ///     (1) graphic is not used as a numbering AND
    ///     (2) background is not already drawn AND
    ///     (3) intrinsic graphic is transparent OR intrinsic graphic doesn't exists
    if ( !_bNumberingGraphic &&
         !_bBackgrdAlreadyDrawn &&
         ( _rGraphicObj.IsTransparent() || _rGraphicObj.GetType() == GRAPHIC_NONE  )
       )
    {
        lcl_implDrawGraphicBackgrd( _rBackgrdBrush, _pOut, _rAlignedPaintRect, _rGraphicObj );
    }
}

/// OD 06.08.2002 #99657# - Note: the transparency of the background graphic
///     is saved in SvxBrushItem.GetGraphicObject(<shell>).GetAttr().Set/GetTransparency()
///     and is considered in the drawing of the graphic.
///     Thus, to provide transparent background graphic for text frames nothing
///     has to be coded.
/// OD 25.09.2002 #99739# - use align rectangle for drawing graphic
/// OD 25.09.2002 #99739# - pixel-align coordinations for drawing graphic.
/// OD 17.10.2002 #103876# - outsource code for drawing background of the graphic
///     with a background color in method <lcl_DrawGraphicBackgrd>
///     Also, change type of <bGrfNum> and <bClip> from <BOOL> to <bool>.
void lcl_DrawGraphic( const SvxBrushItem& rBrush, OutputDevice *pOut,
                      ViewShell &rSh, const SwRect &rGrf, const SwRect &rOut,
                      bool bClip, bool bGrfNum,
                      bool bBackgrdAlreadyDrawn = false )
                      /// OD 02.09.2002 #99657#
                      /// add parameter <bBackgrdAlreadyDrawn> to indicate
                      /// that the background is already drawn.
{
    /// OD 25.09.2002 #99739# - calculate align rectangle from parameter <rGrf>
    ///     and use aligned rectangle <aAlignedGrfRect> in the following code
    SwRect aAlignedGrfRect = rGrf;
    ::SwAlignRect( aAlignedGrfRect, &rSh );

    /// OD 17.10.2002 #103876# - change type from <FASTBOOL> to <bool>.
    const bool bNotInside = bClip && !rOut.IsInside( aAlignedGrfRect );
    if ( bNotInside )
    {
        pOut->Push( PUSH_CLIPREGION );
        pOut->IntersectClipRegion( rOut.SVRect() );
    }

    //Hier kein Link, wir wollen die Grafik synchron laden!
    ((SvxBrushItem&)rBrush).SetDoneLink( Link() );
    GraphicObject *pGrf = (GraphicObject*)rBrush.GetGraphicObject(
                                                    GETOBJSHELL() );

    /// OD 17.10.2002 #103876# - outsourcing drawing of background with a background color.
    ::lcl_DrawGraphicBackgrd( rBrush, pOut, aAlignedGrfRect, *pGrf, bGrfNum, bBackgrdAlreadyDrawn );

    /// OD 25.09.2002 #99739# -
    /// Because for drawing a graphic left-top-corner and size coordinations are
    /// used, these coordinations have to be determined on pixel level.
    ::SwAlignGrfRect( &aAlignedGrfRect, *pOut );
    pGrf->Draw( pOut, aAlignedGrfRect.Pos(), aAlignedGrfRect.SSize() );

    if ( bNotInside )
        pOut->Pop();
} // end of method <lcl_DrawGraphic>

void MA_FASTCALL DrawGraphic( const SvxBrushItem *pBrush,
                              OutputDevice *pOutDev,
                              const SwRect &rOrg,
                              const SwRect &rOut,
                              const BYTE nGrfNum,
                              const sal_Bool bConsiderBackgroundTransparency )
    /// OD 05.08.2002 #99657# - add 6th parameter to indicate that method should
    ///   consider background transparency, saved in the color of the brush item
{
    ViewShell &rSh = *pGlobalShell;
    /// OD 17.10.2002 #103876# - change type from <BOOL> to <bool>
    bool bReplaceGrfNum = GRFNUM_REPLACE == nGrfNum;
    bool bGrfNum = GRFNUM_NO != nGrfNum;
    Size aGrfSize;
    SvxGraphicPosition ePos = GPOS_NONE;
    if( pBrush && !bReplaceGrfNum )
    {
        if( rSh.GetViewOptions()->IsGraphic() )
        {
            ((SvxBrushItem*)pBrush)->SetDoneLink( STATIC_LINK(
                                    rSh.GetDoc(), SwDoc, BackgroundDone ) );
            SfxObjectShell &rObjSh = *GETOBJSHELL();
            const Graphic* pGrf = pBrush->GetGraphic( &rObjSh );
            if( pGrf && GRAPHIC_NONE != pGrf->GetType() )
            {
                ePos = pBrush->GetGraphicPos();
                if( pGrf->IsSupportedGraphic() )
                    // don't the use the specific output device! Bug 94802
                    aGrfSize = ::GetGraphicSizeTwip( *pGrf, 0 );
            }
        }
        else
            bReplaceGrfNum = bGrfNum;
    }

    SwRect aGrf;
    aGrf.SSize( aGrfSize );
    FASTBOOL bDraw = TRUE;
    FASTBOOL bRetouche = TRUE;
    switch ( ePos )
    {
    case GPOS_LT:
        aGrf.Pos() = rOrg.Pos();
        break;

    case GPOS_MT:
        aGrf.Pos().Y() = rOrg.Top();
        aGrf.Pos().X() = rOrg.Left() + rOrg.Width()/2 - aGrfSize.Width()/2;
        break;

    case GPOS_RT:
        aGrf.Pos().Y() = rOrg.Top();
        aGrf.Pos().X() = rOrg.Right() - aGrfSize.Width();
        break;

    case GPOS_LM:
        aGrf.Pos().Y() = rOrg.Top() + rOrg.Height()/2 - aGrfSize.Height()/2;
        aGrf.Pos().X() = rOrg.Left();
        break;

    case GPOS_MM:
        aGrf.Pos().Y() = rOrg.Top() + rOrg.Height()/2 - aGrfSize.Height()/2;
        aGrf.Pos().X() = rOrg.Left() + rOrg.Width()/2 - aGrfSize.Width()/2;
        break;

    case GPOS_RM:
        aGrf.Pos().Y() = rOrg.Top() + rOrg.Height()/2 - aGrfSize.Height()/2;
        aGrf.Pos().X() = rOrg.Right() - aGrfSize.Width();
        break;

    case GPOS_LB:
        aGrf.Pos().Y() = rOrg.Bottom() - aGrfSize.Height();
        aGrf.Pos().X() = rOrg.Left();
        break;

    case GPOS_MB:
        aGrf.Pos().Y() = rOrg.Bottom() - aGrfSize.Height();
        aGrf.Pos().X() = rOrg.Left() + rOrg.Width()/2 - aGrfSize.Width()/2;
        break;

    case GPOS_RB:
        aGrf.Pos().Y() = rOrg.Bottom() - aGrfSize.Height();
        aGrf.Pos().X() = rOrg.Right() - aGrfSize.Width();
        break;

    case GPOS_AREA:
        aGrf = rOrg;
        /// OD 05.09.2002 #102912#
        /// In spite the fact that the background graphic have to fill the complete
        /// area, it has been checked, if the graphic will completely fill out
        /// the region to be painted <rOut> and thus, nothing has to be retouched.
        /// For example, this is the case for a fly frame without a background
        /// brush positioned on the border of the page and inherited the
        /// background brush from the page.
        bRetouche = !rOut.IsInside( aGrf );
        break;

    case GPOS_TILED:
        {
            // OD 17.10.2002 #103876# - draw background of tiled graphic
            // before drawing tiled graphic in loop
            // determine graphic object
            GraphicObject* pGraphicObj = const_cast< GraphicObject* >(pBrush->GetGraphicObject( GETOBJSHELL() ));
            // calculate aligned paint rectangle
            SwRect aAlignedPaintRect = rOut;
            ::SwAlignRect( aAlignedPaintRect, &rSh );
            // OD 25.10.2002 #103876# - draw background color for aligned paint rectangle
            lcl_DrawGraphicBackgrd( *pBrush, pOutDev, aAlignedPaintRect, *pGraphicObj, bGrfNum );

            // set left-top-corner of background graphic to left-top-corner of the
            // area, from which the background brush is determined.
            aGrf.Pos() = rOrg.Pos();
            // setup clipping at output device
            pOutDev->Push( PUSH_CLIPREGION );
            pOutDev->IntersectClipRegion( rOut.SVRect() );
            // OD 28.10.2002 #103876# - use new method <GraphicObject::DrawTiled(::)>
            {
                // calculate paint offset
                Point aPaintOffset( aAlignedPaintRect.Pos() - aGrf.Pos() );
                // draw background graphic tiled for aligned paint rectangle
                pGraphicObj->DrawTiled( pOutDev,
                                        aAlignedPaintRect.SVRect(),
                                        aGrf.SSize(),
                                        Size( aPaintOffset.X(), aPaintOffset.Y() ) );
            }
            // reset clipping at output device
            pOutDev->Pop();
            // set <bDraw> and <bRetouche> to false, indicating that background
            // graphic and background are already drawn.
            bDraw = bRetouche = FALSE;
        }
        break;

    case GPOS_NONE:
        bDraw = FALSE;
        break;

    default: ASSERT( !pOutDev, "new Graphic position?" );
    }

    /// OD 02.09.2002 #99657#
    /// init variable <bGrfBackgrdAlreadDrawn> to indicate, if background of
    /// graphic is already drawn or not.
    bool bGrfBackgrdAlreadyDrawn = false;
    if ( bRetouche )
    {
        /// OD 08.08.2002 #99657# - create region - which has to be drawn - later,
        ///     because it has to be considered transparent drawing.
        ///     Thus, commenting the following two lines of code here.
        /*
        SwRegionRects aRegion( rOut, 4 );
        aRegion -= aGrf;
        */
        pOutDev->Push( PUSH_FILLCOLOR );

        /// OD 07.08.2002 #99657# #GetTransChg#
        ///     check, if a existing background graphic (not filling the complete
        ///     background) is transparent drawn and the background color is
        ///     "no fill" respectively "auto fill", if background transparency
        ///     has to be considered.
        ///     If YES, memorise transparency of background graphic.
        ///     check also, if background graphic bitmap is transparent.
        bool bTransparentGrfWithNoFillBackgrd = false;
        sal_Int32 nGrfTransparency = 0;
        bool bGrfIsTransparent = false;
        if ( (ePos != GPOS_NONE) &&
             (ePos != GPOS_TILED) && (ePos != GPOS_AREA)
           )
        {
            GraphicObject *pGrf = (GraphicObject*)pBrush->GetGraphicObject(
                                                    GETOBJSHELL() );
            if ( bConsiderBackgroundTransparency )
            {
                GraphicAttr pGrfAttr = pGrf->GetAttr();
                if ( (pGrfAttr.GetTransparency() != 0) &&
                     ( pBrush && (pBrush->GetColor() == COL_TRANSPARENT) )
                   )
                {
                    bTransparentGrfWithNoFillBackgrd = true;
                    nGrfTransparency = pGrfAttr.GetTransparency();
                }
            }
            if ( pGrf->IsTransparent() )
            {
                bGrfIsTransparent = true;
            }
        }

        /// OD 06.08.2002 #99657# #GetTransChg# - to get color of brush,
        ///     check background color against COL_TRANSPARENT ("no fill"/"auto fill")
        ///     instead of checking, if transparency is not set.
        const Color aColor( pBrush &&
                            ( !(pBrush->GetColor() == COL_TRANSPARENT) ||
                              bFlyMetafile )
                    ? pBrush->GetColor()
                    : aGlobalRetoucheColor );

        /// OD 08.08.2002 #99657# - determine, if background region have to be
        ///     drawn transparent.
        ///     background region has to be drawn transparent, if
        ///         background transparency have to be considered
        ///     AND
        ///       ( background color is transparent OR
        ///         background graphic is transparent and background color is "no fill"
        ///       )
        sal_Bool bDrawTransparent = bConsiderBackgroundTransparency &&
                                ( ( aColor.GetTransparency() != 0) ||
                                    bTransparentGrfWithNoFillBackgrd );

        /// OD 06.08.2002 #99657# - if background region have to be drawn
        ///     transparent, set only the RGB values of the background color as
        ///     the fill color for the output device.
        if ( bDrawTransparent )
        {
            if( pOutDev->GetFillColor() != aColor.GetRGBColor() )
                pOutDev->SetFillColor( aColor.GetRGBColor() );
        }
        else
        {
            if( pOutDev->GetFillColor() != aColor )
                pOutDev->SetFillColor( aColor );
        }

        /// OD 02.09.2002 #99657#
        if ( bDrawTransparent )
        {
            /// background region have to be drawn transparent.
            /// Thus, create a poly-polygon from the region and draw it with
            /// the corresponding transparency precent.
            PolyPolygon aDrawPoly( rOut.SVRect() );
            if ( aGrf.HasArea() )
            {
                if ( !bGrfIsTransparent )
                {
                    /// substract area of background graphic from draw area
                    /// OD 08.10.2002 #103898# - consider only that part of the
                    ///     graphic area that is overlapping with draw area.
                    SwRect aTmpGrf = aGrf;
                    aTmpGrf.Intersection( rOut );
                    if ( aTmpGrf.HasArea() )
                    {
                        Polygon aGrfPoly( aTmpGrf.SVRect() );
                        aDrawPoly.Insert( aGrfPoly );
                    }
                }
                else
                    bGrfBackgrdAlreadyDrawn = true;
            }
            /// calculate transparency percent:
            /// ( <transparency value[0x01..0xFF]>*100 + 0x7F ) / 0xFF
            /// If there is a background graphic with a background color "no fill"/"auto fill",
            /// the transparency value is taken from the background graphic,
            /// otherwise take the transparency value from the color.
            sal_Int8 nTransparencyPercent =
              (( bTransparentGrfWithNoFillBackgrd ? nGrfTransparency : aColor.GetTransparency()
               )*100 + 0x7F)/0xFF;
            /// draw poly-polygon transparent
            pOutDev->DrawTransparent( aDrawPoly, nTransparencyPercent );
        }
        else
        {
            SwRegionRects aRegion( rOut, 4 );
            if ( !bGrfIsTransparent )
                aRegion -= aGrf;
            else
                bGrfBackgrdAlreadyDrawn = true;
            /// loop rectangles of background region, which has to be drawn
            for( USHORT i = 0; i < aRegion.Count(); ++i )
            {
                pOutDev->DrawRect( aRegion[i].SVRect() );
            }
        }
       pOutDev ->Pop();
    }

    if( bDraw && aGrf.IsOver( rOut ) )
        /// OD 02.09.2002 #99657#
        /// add parameter <bGrfBackgrdAlreadyDrawn>
        lcl_DrawGraphic( *pBrush, pOutDev, rSh, aGrf, rOut, true, bGrfNum,
                         bGrfBackgrdAlreadyDrawn );

    if( bReplaceGrfNum )
    {
        const Bitmap& rBmp = SwNoTxtFrm::GetBitmap( FALSE );
        Font aTmp( pOutDev->GetFont() );
        ((Graphic*)0)->Draw( pOutDev, aEmptyStr, aTmp, rBmp,
                             rOrg.Pos(), rOrg.SSize() );
    }
}

//------------------------------------------------------------------------

/** local help method for SwRootFrm::Paint(..) - Adjust given rectangle to pixel size

    By OD at 27.09.2002 for #103636#
    In order to avoid paint errors caused by multiple alignments - e.g. method
    ::SwAlignRect(..) - and other changes to the rectangle to be painted,
    this method is called for the rectangle to be painted in order to
    adjust it to the pixel it is overlapping.

    @author OD
*/
void lcl_AdjustRectToPixelSize( SwRect& io_aSwRect, const OutputDevice &aOut )
{
    /// local constant object of class <Size> to determine number of Twips
    /// representing a pixel.
    const Size aTwipToPxSize( aOut.PixelToLogic( Size( 1,1 )) );

    /// local object of class <Rectangle> in Twip coordinates
    /// calculated from given rectangle aligned to pixel centers.
    const Rectangle aPxCenterRect = aOut.PixelToLogic(
            aOut.LogicToPixel( io_aSwRect.SVRect() ) );

    /// local constant object of class <Rectangle> representing given rectangle
    /// in pixel.
    const Rectangle aOrgPxRect = aOut.LogicToPixel( io_aSwRect.SVRect() );

    /// calculate adjusted rectangle from pixel centered rectangle.
    /// Due to rounding differences <aPxCenterRect> doesn't exactly represents
    /// the Twip-centers. Thus, adjust borders by half of pixel width/height plus 1.
    /// Afterwards, adjust calculated Twip-positions of the all borders.
    Rectangle aSizedRect = aPxCenterRect;
    aSizedRect.Left() -= (aTwipToPxSize.Width()/2 + 1);
    aSizedRect.Right() += (aTwipToPxSize.Width()/2 + 1);
    aSizedRect.Top() -= (aTwipToPxSize.Height()/2 + 1);
    aSizedRect.Bottom() += (aTwipToPxSize.Height()/2 + 1);

    /// adjust left()
    while ( (aOut.LogicToPixel(aSizedRect)).Left() < aOrgPxRect.Left() )
    {
        ++aSizedRect.Left();
    }
    /// adjust right()
    while ( (aOut.LogicToPixel(aSizedRect)).Right() > aOrgPxRect.Right() )
    {
        --aSizedRect.Right();
    }
    /// adjust top()
    while ( (aOut.LogicToPixel(aSizedRect)).Top() < aOrgPxRect.Top() )
    {
        ++aSizedRect.Top();
    }
    /// adjust bottom()
    while ( (aOut.LogicToPixel(aSizedRect)).Bottom() > aOrgPxRect.Bottom() )
    {
        --aSizedRect.Bottom();
    }

    io_aSwRect = SwRect( aSizedRect );

#ifndef PRODUCT
    Rectangle aTestOrgPxRect = aOut.LogicToPixel( io_aSwRect.SVRect() );
    Rectangle aTestNewPxRect = aOut.LogicToPixel( aSizedRect );
    ASSERT( aTestOrgPxRect == aTestNewPxRect,
            "Error in lcl_AlignRectToPixelSize(..): Adjusted rectangle has incorrect position or size");
#ifdef DEBUG
    Rectangle aTestNewRect( aSizedRect );
    /// check Left()
    --aSizedRect.Left();
    aTestNewPxRect = aOut.LogicToPixel( aSizedRect );
    ASSERT( aTestOrgPxRect.Left() == (aTestNewPxRect.Left()+1),
            "Error in lcl_AlignRectToPixelSize(..): Left() not correct adjusted");
    ++aSizedRect.Left();
    /// check Right()
    ++aSizedRect.Right();
    aTestNewPxRect = aOut.LogicToPixel( aSizedRect );
    ASSERT( aTestOrgPxRect.Right() == (aTestNewPxRect.Right()-1),
            "Error in lcl_AlignRectToPixelSize(..): Right() not correct adjusted");
    --aSizedRect.Right();
    /// check Top()
    --aSizedRect.Top();
    aTestNewPxRect = aOut.LogicToPixel( aSizedRect );
    ASSERT( aTestOrgPxRect.Top() == (aTestNewPxRect.Top()+1),
            "Error in lcl_AlignRectToPixelSize(..): Top() not correct adjusted");
    ++aSizedRect.Top();
    /// check Bottom()
    ++aSizedRect.Bottom();
    aTestNewPxRect = aOut.LogicToPixel( aSizedRect );
    ASSERT( aTestOrgPxRect.Bottom() == (aTestNewPxRect.Bottom()-1),
            "Error in lcl_AlignRectToPixelSize(..): Bottom() not correct adjusted");
    --aSizedRect.Bottom();
#endif
#endif
}

/*************************************************************************
|*
|*	SwRootFrm::Paint()
|*
|*	Beschreibung
|*		Fuer jede sichtbare Seite, die von Rect ber�hrt wird einmal Painten.
|*		1. Umrandungen und Hintergruende Painten.
|*		2. Den DrawLayer (Ramen und Zeichenobjekte) der unter dem Dokument
|* 		   liegt painten (Hoelle).
|*		3. Den Dokumentinhalt (Text) Painten.
|*		4. Den Drawlayer der ueber dem Dokuemnt liegt painten.
|*
|*	Ersterstellung		MA 01. Jun. 92
|*	Letzte Aenderung	MA 10. Oct. 97
|*
|*************************************************************************/

void SwRootFrm::Paint( const SwRect& rRect ) const
{
    ASSERT( Lower() && Lower()->IsPageFrm(), "Lower der Root keine Seite." );

    PROTOCOL( this, PROT_FILE_INIT, 0, 0)

    FASTBOOL bResetRootPaint = FALSE;
    ViewShell *pSh = pCurrShell;

    if ( pSh->GetWin() )
    {
        if ( pSh->GetOut() == pSh->GetWin() && !pSh->GetWin()->IsVisible() )
        {
            return;
        }
        if ( SwRootFrm::bInPaint )
        {
            SwPaintQueue::Add( pSh, rRect );
            return;
        }
    }
    else
        SwRootFrm::bInPaint = bResetRootPaint = TRUE;

    SwSavePaintStatics *pStatics = 0;
    if ( pGlobalShell )
        pStatics = new SwSavePaintStatics();
    pGlobalShell = pSh;

    bOneBeepOnly = pSh->GetWin() != 0 && pSh->GetDoc()->IsFrmBeepEnabled();
    if( !pSh->GetWin() )
        pProgress = SfxProgress::GetActiveProgress( (SfxObjectShell*) pSh->GetDoc()->GetDocShell() );

    ::SwCalcPixStatics( pSh->GetOut() );
    aGlobalRetoucheColor = pSh->Imp()->GetRetoucheColor();

    //Ggf. eine Action ausloesen um klare Verhaeltnisse zu schaffen.
    //Durch diesen Kunstgriff kann in allen Paints davon ausgegangen werden,
    //das alle Werte gueltigt sind - keine Probleme, keine Sonderbehandlung(en).
    if ( !pSh->IsInEndAction() && !pSh->IsPaintInProgress() &&
         (!pSh->Imp()->IsAction() || !pSh->Imp()->GetLayAction().IsActionInProgress() ) )
    {
        ((SwRootFrm*)this)->ResetTurbo();
        SwLayAction aAction( (SwRootFrm*)this, pSh->Imp() );
        aAction.SetPaint( FALSE );
        aAction.SetComplete( FALSE );
        aAction.SetReschedule( pProgress ? TRUE : FALSE );
        aAction.Action();
        ((SwRootFrm*)this)->ResetTurboFlag();
        if ( !pSh->ActionPend() )
            pSh->Imp()->DelRegions();
    }

    SwRect aRect( rRect );
    aRect.Intersection( pSh->VisArea() );

    FASTBOOL bExtraData = ::IsExtraData( GetFmt()->GetDoc() );

    pLines = new SwLineRects;	//Sammler fuer Umrandungen.

    // #104289#. During painting, something (OLE) can
    // load the linguistic, which in turn can cause a reformat
    // of the document. Dangerous! We better set this flag to
    // avoid the reformat.
    const sal_Bool bOldAction = IsCallbackActionEnabled();
    ((SwRootFrm*)this)->SetCallbackActionEnabled( FALSE );

    const SwPageFrm *pPage = pSh->Imp()->GetFirstVisPage();

    while ( pPage && !::IsShortCut( aRect, pPage->Frm() ) )
    {
        if ( !pPage->IsEmptyPage() && aRect.IsOver( pPage->Frm() ) )
        {
            if ( pSh->GetWin() )
            {
                pSubsLines = new SwSubsRects;
                // OD 18.11.2002 #99672# - create array for special sub-lines
                pSpecSubsLines = new SwSubsRects;
            }

            SwRect aPaintRect( pPage->Frm() );
            aPaintRect._Intersection( aRect );

            if ( bExtraData )
            {
                //Ja, das ist grob, aber wie macht man es besser?
                SWRECTFN( pPage )
                (aPaintRect.*fnRect->fnSetLeftAndWidth)(
                    (pPage->Frm().*fnRect->fnGetLeft)(),
                    (pPage->Frm().*fnRect->fnGetWidth)() );
                aPaintRect._Intersection( pSh->VisArea() );
            }

            /// OD 27.09.2002 #103636# - changed method SwLayVout::Enter(..)
            /// 2nd parameter is no longer <const> and will be set to the
            /// rectangle the virtual output device is calculated from <aPaintRect>,
            /// if the virtual output is used.
            pVout->Enter( pSh, aPaintRect, !bNoVirDev );

            /// OD 27.09.2002 #103636# - adjust paint rectangle to pixel size
            /// Thus, all objects overlapping on pixel level with the unadjusted
            /// paint rectangle will be considered in the paint.
            lcl_AdjustRectToPixelSize( aPaintRect, *(pSh->GetOut()) );

            pVout->SetOrgRect( aPaintRect );

            pPage->PaintBaBo( aPaintRect, pPage, TRUE );

            /// OD 29.08.2002 #102450#
            /// determine background color of page for <PaintLayer> method
            /// calls, paint <hell> or <heaven>
            const Color aPageBackgrdColor = pPage->GetDrawBackgrdColor();

            if ( pSh->Imp()->HasDrawView() )
            {
                pLines->LockLines( TRUE );
                /// OD 29.08.2002 #102450# - add 3rd parameter
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetHellId(), aPaintRect,
                                        &aPageBackgrdColor );
                pLines->PaintLines( pSh->GetOut() );
                pLines->LockLines( FALSE );
            }

            if( pSh->GetWin() )
            {
                // OD 18.11.2002 #99672# - collect sub-lines
                pPage->RefreshSubsidiary( aPaintRect );
                // OD 18.11.2002 #99672# - paint special sub-lines
                pSpecSubsLines->PaintSubsidiary( pSh->GetOut(), NULL );
            }

            pPage->Paint( aPaintRect );
            if( pSh->GetWin() )
                lcl_PaintShadow( pPage->Frm(), pSh );
            pLines->PaintLines( pSh->GetOut() );

            BOOL bControlExtra = FALSE;
            if ( pSh->Imp()->HasDrawView() )
            {
                /// OD 29.08.2002 #102450# - add 3rd parameter
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetHeavenId(), aPaintRect,
                                        &aPageBackgrdColor );
                if( pVout->IsFlushable() )
                    bControlExtra = TRUE;
                else
                    pSh->Imp()->PaintLayer( pSh->GetDoc()->GetControlsId(), aPaintRect );
                pLines->PaintLines( pSh->GetOut() );
            }

            if ( bExtraData )
                pPage->RefreshExtraData( aPaintRect );

            if ( pSh->GetWin() )
            {
                pSubsLines->PaintSubsidiary( pSh->GetOut(), pLines );
                DELETEZ( pSubsLines );
                DELETEZ( pSpecSubsLines );
            }
            pVout->Leave();
            if( bControlExtra )
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetControlsId(), aPaintRect );
        }
        ASSERT( !pPage->GetNext() || pPage->GetNext()->IsPageFrm(),
                "Nachbar von Seite keine Seite." );
        pPage = (SwPageFrm*)pPage->GetNext();
    }

    DELETEZ( pLines );

    if ( pSh->GetWin() && pSh->Imp()->HasDrawView() &&
         pSh->Imp()->GetDrawView()->IsGridVisible() )
    {
        pSh->Imp()->GetDrawView()->GetPageViewPgNum(0)->DrawGrid(
            *pSh->GetOut(), rRect.SVRect(), SwViewOption::GetTextGridColor() );
    }

    if ( bResetRootPaint )
        SwRootFrm::bInPaint = FALSE;
    if ( pStatics )
        delete pStatics;
    else
    {
        pProgress = 0;
        pGlobalShell = 0;
    }

    if ( ViewShell::IsLstEndAction() && pSh->GetWin() && pSh->Imp()->HasDrawView() )
        pSh->Imp()->GetDrawView()->PostPaint();

    ((SwRootFrm*)this)->SetCallbackActionEnabled( bOldAction );
}

/*************************************************************************
|*
|*	SwRootFrm::HackPrepareLongTblPaint()
|*
|*	Ersterstellung		MA 27. Sep. 96
|*	Letzte Aenderung	MA 18. Nov. 97
|*
|*************************************************************************/

void SwRootFrm::HackPrepareLongTblPaint( int nMode )
{
    switch ( nMode )
    {
        case HACK_TABLEMODE_INIT       : ASSERT( !pLines, "HackPrepare: already prepared" );
                                         pLines = new SwLineRects;
                                         ASSERT( !pGlobalShell, "old GlobalShell lost" );
                                         pGlobalShell = GetShell();
                                         bTableHack = TRUE;
                                         break;
        case HACK_TABLEMODE_LOCKLINES  : pLines->LockLines( TRUE ); break;
        case HACK_TABLEMODE_PAINTLINES : pLines->PaintLines( GetShell()->GetOut() );
                                         break;
        case HACK_TABLEMODE_UNLOCKLINES: pLines->LockLines( FALSE ); break;
        case HACK_TABLEMODE_EXIT       : pLines->PaintLines( GetShell()->GetOut() );
                                         DELETEZ( pLines );
                                         pGlobalShell = 0;
                                         bTableHack = FALSE;
                                         break;
    }
}


/*************************************************************************
|*
|*	SwLayoutFrm::Paint()
|*
|*	Ersterstellung		MA 19. May. 92
|*	Letzte Aenderung	MA 19. Apr. 95
|*
|*************************************************************************/

void MA_FASTCALL lcl_EmergencyFormatFtnCont( SwFtnContFrm *pCont )
{
    //Es kann sein, dass der Cont vernichtet wird.
    SwCntntFrm *pCnt = pCont->ContainsCntnt();
    while ( pCnt && pCnt->IsInFtn() )
    {
        pCnt->Calc();
        pCnt = pCnt->GetNextCntntFrm();
    }
}

class SwShortCut
{
    SwRectDist fnCheck;
    long nLimit;
public:
    SwShortCut( const SwFrm& rFrm, const SwRect& rRect );
    BOOL Stop( const SwRect& rRect ) const
        { return (rRect.*fnCheck)( nLimit ) > 0; }
};

SwShortCut::SwShortCut( const SwFrm& rFrm, const SwRect& rRect )
{
    BOOL bVert = rFrm.IsVertical();
    BOOL bR2L = rFrm.IsRightToLeft();
    if( rFrm.IsNeighbourFrm() && bVert == bR2L )
    {
        if( bVert )
        {
            fnCheck = &SwRect::GetBottomDistance;
            nLimit = rRect.Top();
        }
        else
        {
            fnCheck = &SwRect::GetLeftDistance;
            nLimit = rRect.Left() + rRect.Width();
        }
    }
    else if( bVert == rFrm.IsNeighbourFrm() )
    {
        fnCheck = &SwRect::GetTopDistance;
        nLimit = rRect.Top() + rRect.Height();
    }
    else
    {
        fnCheck = &SwRect::GetRightDistance;
        nLimit = rRect.Left();
    }
}

void SwLayoutFrm::Paint( const SwRect& rRect ) const
{
    const SwFrm *pFrm = Lower();
    if ( !pFrm )
        return;

    SwShortCut aShortCut( *pFrm, rRect );
    BOOL bCnt;
    if ( TRUE == (bCnt = pFrm->IsCntntFrm()) )
        pFrm->Calc();

    if ( pFrm->IsFtnContFrm() )
    {
        ::lcl_EmergencyFormatFtnCont( (SwFtnContFrm*)pFrm );
        pFrm = Lower();
    }

    const SwPageFrm *pPage = 0;
    const FASTBOOL bWin   = pGlobalShell->GetWin() ? TRUE : FALSE;

    while ( IsAnLower( pFrm ) )
    {
        SwRect aPaintRect( pFrm->PaintArea() );
        if( aShortCut.Stop( aPaintRect ) )
            break;
        if ( bCnt && pProgress )
            pProgress->Reschedule();

        //Wenn ein Frm es explizit will muss retouchiert werden.
        //Erst die Retouche, denn selbige koennte die aligned'en Raender
        //plaetten.
        if ( pFrm->IsRetouche() )
        {
            if ( pFrm->IsRetoucheFrm() && bWin && !pFrm->GetNext() )
            {	if ( !pPage )
                    pPage = FindPageFrm();
                pFrm->Retouche( pPage, rRect );
            }
            pFrm->ResetRetouche();
        }
        if ( rRect.IsOver( aPaintRect ) )
        {
            if ( bCnt && pFrm->IsCompletePaint() &&
                 !rRect.IsInside( aPaintRect ) && GetpApp()->AnyInput( INPUT_KEYBOARD ) )
            {
                //fix(8104): Es kann vorkommen, dass die Verarbeitung nicht
                //vollstaendig war, aber trotzdem Teile des Absatzes gepaintet
                //werden. In der Folge werden dann evtl. wiederum andere Teile
                //des Absatzes garnicht mehr gepaintet. Einziger Ausweg scheint
                //hier ein Invalidieren der Windows zu sein.
                //Um es nicht alzu Heftig werden zu lassen versuche ich hier
                //das Rechteck zu begrenzen indem der gewuenschte Teil gepaintet
                //und nur die uebrigen Absatzanteile invalidiert werden.
                if ( aPaintRect.Left()	== rRect.Left() &&
                     aPaintRect.Right() == rRect.Right() )
                {
                    aPaintRect.Bottom( rRect.Top() - 1 );
                    if ( aPaintRect.Height() > 0 )
                        pGlobalShell->InvalidateWindows(aPaintRect);
                    aPaintRect.Top( rRect.Bottom() + 1 );
                    aPaintRect.Bottom( pFrm->Frm().Bottom() );
                    if ( aPaintRect.Height() > 0 )
                        pGlobalShell->InvalidateWindows(aPaintRect);
                    aPaintRect.Top( pFrm->Frm().Top() );
                    aPaintRect.Bottom( pFrm->Frm().Bottom() );
                }
                else
                {
                    pGlobalShell->InvalidateWindows( aPaintRect );
                    pFrm = pFrm->GetNext();
                    if ( pFrm && (TRUE == (bCnt = pFrm->IsCntntFrm())) )
                        pFrm->Calc();
                    continue;
                }
            }
            pFrm->ResetCompletePaint();
            aPaintRect._Intersection( rRect );
            pFrm->Paint( aPaintRect );

            if ( Lower() && Lower()->IsColumnFrm() )
            {
                //Ggf. die Spaltentrennlinien malen. Fuer den Seitenbody ist
                //nicht der Upper sondern die Seite Zustaendig.
                const SwFrmFmt *pFmt = GetUpper() && GetUpper()->IsPageFrm()
                                            ? GetUpper()->GetFmt()
                                            : GetFmt();
                const SwFmtCol &rCol = pFmt->GetCol();
                if ( rCol.GetLineAdj() != COLADJ_NONE )
                {
                    if ( !pPage )
                        pPage = pFrm->FindPageFrm();
                    PaintColLines( aPaintRect, rCol, pPage );
                }
            }

            //Nur: Body, Column, Tab, Header, Footer, FtnCont, Ftn
            if ( bOneBeepOnly && (pFrm->GetType() & 0x08FC))
            {
                //Wenn der Frm ueber seinen Upper hinausragt gibts die gelbe Karte.
                const long nDeadline = Frm().Top() + Prt().Top() +
                                       Prt().Height() - 1;
                if ( pFrm->Frm().Bottom() > nDeadline )
                {
                    if ( pGlobalShell->VisArea().Bottom() > nDeadline &&
                         pGlobalShell->VisArea().Top() < nDeadline )
                    {
                        bOneBeepOnly = FALSE;
                        Sound::Beep();
                    }
                }
            }
        }
        if ( !bCnt && pFrm->GetNext() && pFrm->GetNext()->IsFtnContFrm() )
            ::lcl_EmergencyFormatFtnCont( (SwFtnContFrm*)pFrm->GetNext() );

        pFrm = pFrm->GetNext();
        if ( pFrm && (TRUE == (bCnt = pFrm->IsCntntFrm())) )
            pFrm->Calc();
    }
}


/** FlyFrm::IsBackgroundTransparent - for feature #99657#

    OD 12.08.2002
    determines, if background of fly frame has to be drawn transparent
    declaration found in /core/inc/flyfrm.cxx
    OD 08.10.2002 #103898# - If the background of the fly frame itself is not
    transparent and the background is inherited from its parent/grandparent,
    the background brush, used for drawing, has to be investigated for transparency.

    @author OD

    @return true, if background is transparent drawn.
*/
const sal_Bool SwFlyFrm::IsBackgroundTransparent() const
{
    sal_Bool bBackgroundTransparent = GetFmt()->IsBackgroundTransparent();
    if ( !bBackgroundTransparent &&
         static_cast<const SwFlyFrmFmt*>(GetFmt())->IsBackgroundBrushInherited() )
    {
        const SvxBrushItem* pBackgrdBrush = 0;
        const Color* pSectionTOXColor = 0;
        SwRect aDummyRect;
        if ( GetBackgroundBrush( pBackgrdBrush, pSectionTOXColor, aDummyRect, false) )
        {
            if ( pSectionTOXColor &&
                 (pSectionTOXColor->GetTransparency() != 0) &&
                 (pSectionTOXColor->GetColor() != COL_TRANSPARENT) )
            {
                bBackgroundTransparent = sal_True;
            }
            else if ( pBackgrdBrush )
            {
                if ( (pBackgrdBrush->GetColor().GetTransparency() != 0) &&
                     (pBackgrdBrush->GetColor() != COL_TRANSPARENT) )
                {
                    bBackgroundTransparent = sal_True;
                }
                else
                {
                    const GraphicObject *pTmpGrf =
                            static_cast<const GraphicObject*>(pBackgrdBrush->GetGraphicObject());
                    if ( (pTmpGrf) &&
                         (pTmpGrf->GetAttr().GetTransparency() != 0)
                       )
                    {
                        bBackgroundTransparent = sal_True;
                    }
                }
            }
        }
    }

    return bBackgroundTransparent;
};

/** FlyFrm::IsShadowTransparent - for feature #99657#

    OD 13.08.2002
    determine, if shadow color of fly frame has to be drawn transparent
    declaration found in /core/inc/flyfrm.cxx

    @author OD

    @return true, if shadow color is transparent.
*/
const sal_Bool SwFlyFrm::IsShadowTransparent() const
{
    return GetFmt()->IsShadowTransparent();
};

/*************************************************************************
|*
|*	SwFlyFrm::IsPaint()
|*
|*	Ersterstellung		MA 16. Jan. 97
|*	Letzte Aenderung	MA 16. Jan. 97
|*
|*************************************************************************/

BOOL SwFlyFrm::IsPaint( SdrObject *pObj, const ViewShell *pSh )
{
    SdrObjUserCall *pUserCall;

    if ( 0 == ( pUserCall = GetUserCall(pObj) ) )
        return TRUE;

    //Attributabhaengig nicht fuer Drucker oder PreView painten
    FASTBOOL bPaint =  pFlyOnlyDraw ||
                       ((SwContact*)pUserCall)->GetFmt()->GetPrint().GetValue();
    if ( !bPaint )
        bPaint = pSh->GetWin() && !pSh->IsPreView();

    if ( bPaint )
    {
        //Das Paint kann evtl. von von uebergeordneten Flys verhindert werden.
        SwFrm *pAnch = 0;
        if ( pObj->IsWriterFlyFrame() )
        {
            SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
            if ( pFlyOnlyDraw && pFlyOnlyDraw == pFly )
                return TRUE;

            //Die Anzeige eines Zwischenstadiums vermeiden, Flys die nicht mit
            //der Seite auf der sie verankert sind ueberlappen werden auch
            //nicht gepaintet.
            //HACK: Ausnahme: Drucken von Rahmen in Tabellen, diese koennen
            //bei uebergrossen Tabellen (HTML) schon mal auserhalb der Seite
            //stehen.
            SwPageFrm *pPage = pFly->FindPageFrm();
            if ( pPage )
            {
                if ( pPage->Frm().IsOver( pFly->Frm() ) )
                    pAnch = pFly->GetAnchor();
                else if ( bTableHack &&
                          pFly->Frm().Top() >= pFly->GetAnchor()->Frm().Top() &&
                            pFly->Frm().Top() < pFly->GetAnchor()->Frm().Bottom() &&
                          long(pSh->GetOut()) == long(pSh->GetPrt()) )
                {
                    pAnch = pFly->GetAnchor();
                }
            }

        }
        else
        {
            pAnch = ((SwDrawContact*)pUserCall)->GetAnchor();
            if ( pAnch )
            {
                if ( !pAnch->GetValidPosFlag() )
                    pAnch = 0;
                else if ( long(pSh->GetOut()) == long(pSh->GetPrt()) )
                {
                    //HACK: fuer das Drucken muessen wir ein paar Objekte
                    //weglassen, da diese sonst doppelt gedruckt werden.
                    //Die Objekte sollen gedruckt werden, wenn der TableHack
                    //gerade greift. In der Folge duerfen sie nicht gedruckt werden
                    //wenn sie mit der Seite dran sind, ueber der sie von der
                    //Position her gerade schweben.
                    SwPageFrm *pPage = pAnch->FindPageFrm();
                    if ( !bTableHack &&
                         !pPage->Frm().IsOver( pObj->GetBoundRect() ) )
                        pAnch = 0;
                }
            }
        }
        if ( pAnch )
        {
            if ( pAnch->IsInFly() )
                bPaint = SwFlyFrm::IsPaint( pAnch->FindFlyFrm()->GetVirtDrawObj(),
                                            pSh );
            else if ( pFlyOnlyDraw )
                bPaint = FALSE;
        }
        else
            bPaint = FALSE;
    }
    return bPaint;
}

/*************************************************************************
|*
|*	SwFlyFrm::Paint()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 16. Jan. 97
|*
|*************************************************************************/

//Weiter unten definiert
void MA_FASTCALL lcl_PaintLowerBorders( const SwLayoutFrm *pLay,
                               const SwRect &rRect, const SwPageFrm *pPage );

void SwFlyFrm::Paint( const SwRect& rRect ) const
{
    //wegen der Ueberlappung von Rahmen und Zeichenobjekten muessen die
    //Flys ihre Umrandung (und die der Innenliegenden) direkt ausgeben.
    //z.B. #33066#
    pLines->LockLines(TRUE);

    SwRect aRect( rRect );
    aRect._Intersection( Frm() );

    OutputDevice *pOut = pGlobalShell->GetOut();
    pOut->Push( PUSH_CLIPREGION );
    pOut->SetClipRegion();
    const SwPageFrm *pPage = FindPageFrm();

    const SwNoTxtFrm *pNoTxt = Lower() && Lower()->IsNoTxtFrm()
                                                ? (SwNoTxtFrm*)Lower() : 0;
    /// OD 19.08.2002 #99657# - rename local variable
    ///     <bTransparent> --> <bIsGraphicTransparent>
    FASTBOOL bIsGraphicTransparent = pNoTxt ? pNoTxt->IsTransparent() : FALSE,
             bContour	  = GetFmt()->GetSurround().IsContour(),
             bHell, bPaintBack;

    if ( bIsGraphicTransparent &&
         GetVirtDrawObj()->GetLayer() == GetFmt()->GetDoc()->GetHellId() &&
         GetAnchor()->FindFlyFrm() )
    {
        SwFlyFrm *pOldRet = pRetoucheFly2; pRetoucheFly2 = (SwFlyFrm*)this;
        const SwFrm *pFrm = GetAnchor()->FindFlyFrm();
        SwBorderAttrAccess aAccess( SwFrm::GetCache(), pFrm );
        const SwBorderAttrs &rAttrs = *aAccess.Get();
        pFrm->PaintBackground( aRect, pPage, rAttrs, FALSE, FALSE );
        pRetoucheFly2 = pOldRet;
    }
//#33429#			else
    {
        PolyPolygon aPoly;
        if ( bContour )
            bContour = GetContour( aPoly );

        //Hintergrund painten fuer:
        bPaintBack = !pNoTxt || Prt().SSize() != Frm().SSize();
        //sowie fuer Transparente und Contour in der Hoelle
        bPaintBack = bPaintBack ||
                ((bIsGraphicTransparent || bContour ) &&
                TRUE == (bHell = GetVirtDrawObj()->GetLayer() == GetFmt()->GetDoc()->GetHellId()));
        //sowie fuer Transparente und Contour mit eigener Brush
        if ( !bPaintBack && (bIsGraphicTransparent||bContour) )
        {
            const SvxBrushItem &rBack = GetFmt()->GetBackground();
            /// OD 07.08.2002 #99657# #GetTransChg#
            ///     to determine, if background has to be painted, by checking, if
            ///     background color is not COL_TRANSPARENT ("no fill"/"auto fill")
            ///     or a background graphic exists.
            bPaintBack = !(rBack.GetColor() == COL_TRANSPARENT) ||
            ///bPaintBack = !rBack.GetColor().GetTransparency() ||
                         rBack.GetGraphicPos() != GPOS_NONE;
        }

        if ( bPaintBack )
        {
            //#24926# JP 01.02.96, PaintBaBo in teilen hier, damit PaintBorder
            //das orig. Rect bekommt, aber PaintBackground das begrenzte.

            pOut->Push( PUSH_FILLCOLOR );

            pPage = FindPageFrm();

            SwBorderAttrAccess aAccess( SwFrm::GetCache(), (SwFrm*)this );
            const SwBorderAttrs &rAttrs = *aAccess.Get();

/*			//Das Paint der FlyInCnt's wird vom Paint der Lines gerufen. Damit
            //der Rand (UL-/LRSpace) mit der richtigen Farbe retouchiert wird,
            //muss Das Paint an den Anker weitergereicht werden.
            FASTBOOL bUnlock = FALSE;
            if ( IsFlyInCntFrm() )
            {
                bLockFlyBackground = bUnlock = TRUE;
                SwBorderAttrAccess aAccess( SwFrm::GetCache(), GetAnchor());
                const SwBorderAttrs &rAttrs = *aAccess.Get();
                GetAnchor()->PaintBackground( aRect, pPage, rAttrs, FALSE );
            }
*/

            /// OD 06.08.2002 #99657# - paint border before painting background
            /// paint border
            {
                SwRect aTmp( rRect );
//??        aTmp._Intersection( Frm() );
                PaintBorder( aTmp, pPage, rAttrs );
/*          if ( bUnlock )
                    bLockFlyBackground = FALSE;
*/
            }

            /// paint background
            {
                SwRegionRects aRegion( aRect );
                if ( pNoTxt && !bIsGraphicTransparent )
                {
                    //Was wir eigentlich Painten wollen ist der schmale Streifen
                    //zwischen PrtArea und aeusserer Umrandung.
                    SwRect aTmp( Prt() ); aTmp += Frm().Pos();
                    aRegion -= aTmp;
                }
                if ( bContour )
                {
                    pOut->Push();
                    if ( !pOut->GetConnectMetaFile() || pOut->GetOutDevType() == OUTDEV_PRINTER )
                    {
                        pOut->SetClipRegion( aPoly );
                    }
                    for ( USHORT i = 0; i < aRegion.Count(); ++i )
                        PaintBackground( aRegion[i], pPage, rAttrs, FALSE, TRUE );
                    pOut->Pop();
                }
                else
                    for ( USHORT i = 0; i < aRegion.Count(); ++i )
                        PaintBackground( aRegion[i], pPage, rAttrs, FALSE, TRUE );
            }

            pOut->Pop();
        }
    }

    SwLayoutFrm::Paint( aRect );
    Validate();

    pLines->LockLines( FALSE );
    pLines->PaintLines( pOut );

    pOut->Pop();

    if ( pProgress && pNoTxt )
        pProgress->Reschedule();
}
/*************************************************************************
|*
|*	  SwTabFrm::Paint()
|*
|*	  Ersterstellung	MA 11. May. 93
|*	  Letzte Aenderung	MA 23. Mar. 95
|*
|*************************************************************************/

void SwTabFrm::Paint( const SwRect& rRect ) const
{
    if( pGlobalShell->GetViewOptions()->IsTable() )
        SwLayoutFrm::Paint( rRect );
    else if( pGlobalShell->GetWin() )
    {
        pGlobalShell->GetViewOptions()->
                DrawRect( pGlobalShell->GetOut(), rRect, COL_LIGHTGRAY );
    }
    ((SwTabFrm*)this)->ResetComplete();
}

/*************************************************************************
|*
|*	SwFrm::PaintShadow()
|*
|*	Beschreibung		Malt einen Shatten wenns das FrmFormat fordert.
|* 		Der Schatten wird immer an den auesseren Rand des OutRect gemalt.
|* 		Das OutRect wird ggf. so verkleinert, dass auf diesem das
|* 		malen der Umrandung stattfinden kann.
|*	Ersterstellung		MA 21. Dec. 92
|*	Letzte Aenderung	MA 29. May. 97
|*
|*************************************************************************/
/// OD 23.08.2002 #99657#
///     draw full shadow rectangle for frames with transparent drawn backgrounds.
void SwFrm::PaintShadow( const SwRect& rRect, SwRect& rOutRect,
                         const SwPageFrm *pPage,
                         const SwBorderAttrs &rAttrs ) const
{
    const SvxShadowItem &rShadow = rAttrs.GetShadow();
    const long nWidth  = ::lcl_AlignWidth ( rShadow.GetWidth() );
    const long nHeight = ::lcl_AlignHeight( rShadow.GetWidth() );

    SwRects aRegion( 2, 2 );
    SwRect aOut( rOutRect );

    const FASTBOOL bCnt	   = IsCntntFrm();
    const FASTBOOL bTop    = !bCnt || rAttrs.GetTopLine	 ( this ) ? TRUE : FALSE;
    const FASTBOOL bBottom = !bCnt || rAttrs.GetBottomLine( this ) ? TRUE : FALSE;

    SvxShadowLocation eLoc = rShadow.GetLocation();

    SWRECTFN( this )
    if( IsVertical() )
    {
        switch( eLoc )
        {
            case SVX_SHADOW_BOTTOMRIGHT: eLoc = SVX_SHADOW_BOTTOMLEFT;  break;
            case SVX_SHADOW_TOPLEFT:     eLoc = SVX_SHADOW_TOPRIGHT;    break;
            case SVX_SHADOW_TOPRIGHT:    eLoc = SVX_SHADOW_BOTTOMRIGHT; break;
            case SVX_SHADOW_BOTTOMLEFT:  eLoc = SVX_SHADOW_TOPLEFT;     break;
        }
    }

    /// OD 23.08.2002 #99657# - determine, if full shadow rectangle have to
    ///     be drawn or only two shadow rectangles beside the frame.
    ///     draw full shadow rectangle, if frame background is drawn transparent.
    ///     Status Quo:
    ///         SwLayoutFrm can have transparent drawn backgrounds. Thus,
    ///         "asked" their frame format.
    sal_Bool bDrawFullShadowRectangle =
            ( IsLayoutFrm() &&
              (static_cast<const SwLayoutFrm*>(this))->GetFmt()->IsBackgroundTransparent()
            );
    switch ( eLoc )
    {
        case SVX_SHADOW_BOTTOMRIGHT:
            {
                if ( bDrawFullShadowRectangle )
                {
                    /// OD 06.08.2002 #99657# - draw full shadow rectangle
                    aOut.Top( aOut.Top() + nHeight );
                    aOut.Left( aOut.Left() + nWidth );
                    aRegion.Insert( aOut, aRegion.Count() );
                }
                else
                {
                    aOut.Top ( aOut.Bottom() - nHeight );
                    aOut.Left( aOut.Left()   + nWidth );
                    if ( bBottom )
                        aRegion.Insert( aOut, aRegion.Count() );
                    aOut.Left( aOut.Right()   - nWidth );
                    aOut.Top ( rOutRect.Top() + nHeight );
                    if ( bBottom )
                        aOut.Bottom( aOut.Bottom() - nHeight );
                    if ( bCnt && (!bTop || !bBottom) )
                        ::lcl_ExtendLeftAndRight( aOut, this, rAttrs, fnRect );
                    aRegion.Insert( aOut, aRegion.Count() );
                }

                rOutRect.Right ( rOutRect.Right() - nWidth );
                rOutRect.Bottom( rOutRect.Bottom()- nHeight );
            }
            break;
        case SVX_SHADOW_TOPLEFT:
            {
                if ( bDrawFullShadowRectangle )
                {
                    /// OD 06.08.2002 #99657# - draw full shadow rectangle
                    aOut.Bottom( aOut.Bottom() - nHeight );
                    aOut.Right( aOut.Right() - nWidth );
                    aRegion.Insert( aOut, aRegion.Count() );
                }
                else
                {
                    aOut.Bottom( aOut.Top()   + nHeight );
                    aOut.Right ( aOut.Right() - nWidth );
                    if ( bTop )
                        aRegion.Insert( aOut, aRegion.Count() );
                    aOut.Right ( aOut.Left() + nWidth );
                    aOut.Bottom( rOutRect.Bottom() - nHeight );
                    if ( bTop )
                        aOut.Top( aOut.Top() + nHeight );
                    if ( bCnt && (!bBottom || !bTop) )
                        ::lcl_ExtendLeftAndRight( aOut, this, rAttrs, fnRect );
                    aRegion.Insert( aOut, aRegion.Count() );
                }

                rOutRect.Left( rOutRect.Left() + nWidth );
                rOutRect.Top(  rOutRect.Top() + nHeight );
            }
            break;
        case SVX_SHADOW_TOPRIGHT:
            {
                if ( bDrawFullShadowRectangle )
                {
                    /// OD 06.08.2002 #99657# - draw full shadow rectangle
                    aOut.Bottom( aOut.Bottom() - nHeight);
                    aOut.Left( aOut.Left() + nWidth );
                    aRegion.Insert( aOut, aRegion.Count() );
                }
                else
                {
                    aOut.Bottom( aOut.Top() + nHeight );
                    aOut.Left (  aOut.Left()+ nWidth );
                    if ( bTop )
                        aRegion.Insert( aOut, aRegion.Count() );
                    aOut.Left  ( aOut.Right() - nWidth );
                    aOut.Bottom( rOutRect.Bottom() - nHeight );
                    if ( bTop )
                        aOut.Top( aOut.Top() + nHeight );
                    if ( bCnt && (!bBottom || bTop) )
                        ::lcl_ExtendLeftAndRight( aOut, this, rAttrs, fnRect );
                    aRegion.Insert( aOut, aRegion.Count() );
                }

                rOutRect.Right( rOutRect.Right() - nWidth );
                rOutRect.Top( rOutRect.Top() + nHeight );
            }
            break;
        case SVX_SHADOW_BOTTOMLEFT:
            {
                if ( bDrawFullShadowRectangle )
                {
                    /// OD 06.08.2002 #99657# - draw full shadow rectangle
                    aOut.Top( aOut.Top() + nHeight );
                    aOut.Right( aOut.Right() - nWidth );
                    aRegion.Insert( aOut, aRegion.Count() );
                }
                else
                {
                    aOut.Top  ( aOut.Bottom()- nHeight );
                    aOut.Right( aOut.Right() - nWidth );
                    if ( bBottom )
                        aRegion.Insert( aOut, aRegion.Count() );
                    aOut.Right( aOut.Left() + nWidth );
                    aOut.Top( rOutRect.Top() + nHeight );
                    if ( bBottom )
                        aOut.Bottom( aOut.Bottom() - nHeight );
                    if ( bCnt && (!bTop || !bBottom) )
                        ::lcl_ExtendLeftAndRight( aOut, this, rAttrs, fnRect );
                    aRegion.Insert( aOut, aRegion.Count() );
                }

                rOutRect.Left( rOutRect.Left() + nWidth );
                rOutRect.Bottom( rOutRect.Bottom() - nHeight );
            }
            break;
#ifndef PRODUCT
        default:	ASSERT( !this, "new ShadowLocation() ?" );
#endif
    }

    OutputDevice *pOut = pGlobalShell->GetOut();

    ULONG nOldDrawMode = pOut->GetDrawMode();
    Color aShadowColor( rShadow.GetColor() );
    if( aRegion.Count() && pGlobalShell->GetWin() &&
        Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
    {
        // Is heigh contrast mode, the output device has already set the
        // DRAWMODE_SETTINGSFILL flag. This causes the SetFillColor function
        // to ignore the setting of a new color. Therefore we have to reset
        // the drawing mode
        pOut->SetDrawMode( 0 );
        aShadowColor = SwViewOption::GetFontColor();
    }

    if ( pOut->GetFillColor() != aShadowColor )
        pOut->SetFillColor( aShadowColor );

    pOut->SetDrawMode( nOldDrawMode );

    for ( USHORT i = 0; i < aRegion.Count(); ++i )
    {
        SwRect &rOut = aRegion[i];
        aOut = rOut;
        /// OD 30.09.2002 #103636# - no SwAlign of shadow rectangle
        /// no alignment necessary, because (1) <rRect> is already aligned
        /// and because (2) paint of border and background will occur later.
        /// Thus, (1) assures that no conflicts with neighbour object will occure
        /// and (2) assures that border and background is not affected by the
        /// shadow paint.
        /*
        ::SwAlignRect( aOut, pGlobalShell );
        */
        if ( rRect.IsOver( aOut ) && aOut.Height() > 0 && aOut.Width() > 0 )
        {
            aOut._Intersection( rRect );
            pOut->DrawRect( aOut.SVRect() );
        }
    }
}

/*************************************************************************
|*
|*	SwFrm::PaintBorderLine()
|*
|*	Ersterstellung		MA 22. Dec. 92
|*	Letzte Aenderung	MA 22. Jan. 95
|*
|*************************************************************************/

void SwFrm::PaintBorderLine( const SwRect& rRect,
                             const SwRect& rOutRect,
                             const SwPageFrm *pPage,
                             const Color *pColor ) const
{
    if ( !rOutRect.IsOver( rRect ) )
        return;

    SwRect aOut( rOutRect );
    aOut._Intersection( rRect );

    const SwTabFrm *pTab = IsCellFrm() ? FindTabFrm() : 0;
    BYTE nSubCol = ( IsCellFrm() || IsRowFrm() ) ? SUBCOL_TAB :
                   ( IsInSct() ? SUBCOL_SECT :
                   ( IsInFly() ? SUBCOL_FLY : SUBCOL_PAGE ) );
    if( pColor && pGlobalShell->GetWin() &&
        Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
    {
        pColor = &SwViewOption::GetFontColor();
    }

    if ( pPage->GetSortedObjs() )
    {
        SwRegionRects aRegion( aOut, 4, 1 );
        ::lcl_SubtractFlys( this, pPage, aOut, aRegion );
        for ( USHORT i = 0; i < aRegion.Count(); ++i )
            pLines->AddLineRect( aRegion[i], pColor, pTab, nSubCol );
    }
    else
        pLines->AddLineRect( aOut, pColor, pTab, nSubCol );
}

/*************************************************************************
|*
|*	SwFrm::PaintBorderLines()
|*
|*	Beschreibung		Nur alle Linien einfach oder alle Linien doppelt!!!!
|*	Ersterstellung		MA 22. Dec. 92
|*	Letzte Aenderung	MA 22. Mar. 95
|*
|*************************************************************************/

void MA_FASTCALL lcl_SubTopBottom( SwRect &rRect, const SvxBoxItem &rBox,
            const SwBorderAttrs &rAttrs, const SwFrm *pFrm, SwRectFn& rRectFn )
{
    const BOOL bCnt = pFrm->IsCntntFrm();
    if ( rBox.GetTop() && rBox.GetTop()->GetInWidth() &&
         (!bCnt || rAttrs.GetTopLine( pFrm )) )
    {
        const long nDist = ::lcl_MinHeightDist( rBox.GetTop()->GetDistance() ) +
                           ::lcl_AlignHeight( rBox.GetTop()->GetOutWidth() );
        (rRect.*rRectFn->fnSubTop)( -nDist );
    }

    if ( rBox.GetBottom() && rBox.GetBottom()->GetInWidth() &&
         (!bCnt || rAttrs.GetBottomLine( pFrm)))
    {
        const long nDist = ::lcl_MinHeightDist( rBox.GetBottom()->GetDistance())
                           +::lcl_AlignHeight( rBox.GetBottom()->GetOutWidth());
        (rRect.*rRectFn->fnAddBottom)( -nDist );
    }
}

void MA_FASTCALL lcl_SubLeftRight( SwRect &rRect, const SvxBoxItem &rBox,
                                   SwRectFn& rRectFn )
{
    if ( rBox.GetLeft() && rBox.GetLeft()->GetInWidth() )
    {
        const long nDist = ::lcl_MinWidthDist( rBox.GetLeft()->GetDistance() )
                           + ::lcl_AlignWidth( rBox.GetLeft()->GetOutWidth() );
        (rRect.*rRectFn->fnSubLeft)( -nDist );
    }

    if ( rBox.GetRight() && rBox.GetRight()->GetInWidth() )
    {
        const long nDist = ::lcl_MinWidthDist( rBox.GetRight()->GetDistance() )
                           + ::lcl_AlignWidth( rBox.GetRight()->GetOutWidth() );
        (rRect.*rRectFn->fnAddRight)( -nDist );
    }
}

void MA_FASTCALL lcl_PaintLeftLine( const SwFrm *pFrm, const SwPageFrm *pPage,
                           const SwRect &rOutRect, const SwRect &rRect,
                           const SwBorderAttrs &rAttrs, SwRectFn& rRectFn )
{
    const SvxBoxItem &rBox = rAttrs.GetBox();
    const sal_Bool bR2L = pFrm->IsCellFrm() && pFrm->IsRightToLeft();
    const SvxBorderLine *pLeft = bR2L ? rBox.GetRight() : rBox.GetLeft();

    if ( !pLeft )
        return;

    SwRect aRect( rOutRect );
    (aRect.*rRectFn->fnAddRight)( ::lcl_AlignWidth( pLeft->GetOutWidth() ) -
                              (aRect.*rRectFn->fnGetWidth)() );

    const BOOL bCnt = pFrm->IsCntntFrm();

    if ( bCnt )
        ::lcl_ExtendLeftAndRight( aRect, pFrm, rAttrs, rRectFn );

    if ( !pLeft->GetInWidth() )
        ::lcl_SubTopBottom( aRect, rBox, rAttrs, pFrm, rRectFn );

    pFrm->PaintBorderLine( rRect, aRect, pPage, &pLeft->GetColor() );

    if ( pLeft->GetInWidth() )
    {
        const long nDist = ::lcl_MinWidthDist( pLeft->GetDistance() );
        long nWidth = ::lcl_AlignWidth( pLeft->GetInWidth() );
        (aRect.*rRectFn->fnAddRight)( nDist + nWidth );
        (aRect.*rRectFn->fnSubLeft)( nWidth - (aRect.*rRectFn->fnGetWidth)() );
        ::lcl_SubTopBottom( aRect, rBox, rAttrs, pFrm, rRectFn );
        pFrm->PaintBorderLine( rRect, aRect, pPage, &pLeft->GetColor() );
    }
}

void MA_FASTCALL lcl_PaintRightLine( const SwFrm *pFrm, const SwPageFrm *pPage,
                            const SwRect &rOutRect, const SwRect &rRect,
                            const SwBorderAttrs &rAttrs, SwRectFn& rRectFn )
{
    const SvxBoxItem &rBox = rAttrs.GetBox();
    const sal_Bool bR2L = pFrm->IsCellFrm() && pFrm->IsRightToLeft();
    const SvxBorderLine *pRight = bR2L ? rBox.GetLeft() : rBox.GetRight();

    if ( !pRight )
        return;

    SwRect aRect( rOutRect );
    (aRect.*rRectFn->fnSubLeft)( ::lcl_AlignWidth( pRight->GetOutWidth() ) -
                              (aRect.*rRectFn->fnGetWidth)() );

    const BOOL bCnt = pFrm->IsCntntFrm();

    if ( bCnt )
        ::lcl_ExtendLeftAndRight( aRect, pFrm, rAttrs, rRectFn );

    if ( !pRight->GetInWidth() )
        ::lcl_SubTopBottom( aRect, rBox, rAttrs, pFrm, rRectFn );

    pFrm->PaintBorderLine( rRect, aRect, pPage, &pRight->GetColor() );

    if ( pRight->GetInWidth() )
    {
        const long nDist = ::lcl_MinWidthDist( pRight->GetDistance() );
        long nWidth = ::lcl_AlignWidth( pRight->GetInWidth() );
        (aRect.*rRectFn->fnSubLeft)( nDist + nWidth );
        (aRect.*rRectFn->fnAddRight)( nWidth - (aRect.*rRectFn->fnGetWidth)() );
        ::lcl_SubTopBottom( aRect, rBox, rAttrs, pFrm, rRectFn );
        pFrm->PaintBorderLine( rRect, aRect, pPage, &pRight->GetColor() );
    }
}

void MA_FASTCALL lcl_PaintTopLine( const SwFrm *pFrm, const SwPageFrm *pPage,
                          const SwRect &rOutRect, const SwRect &rRect,
                          const SwBorderAttrs &rAttrs, SwRectFn& rRectFn )
{
    const SvxBoxItem &rBox = rAttrs.GetBox();
    const SvxBorderLine *pTop = rBox.GetTop();

    if ( !pTop )
        return;

    SwRect aRect( rOutRect );
    (aRect.*rRectFn->fnAddBottom)( ::lcl_AlignHeight( pTop->GetOutWidth() ) -
                                   (aRect.*rRectFn->fnGetHeight)() );
    pFrm->PaintBorderLine( rRect, aRect, pPage, &pTop->GetColor() );

    if ( pTop->GetInWidth() )
    {
        const long nDist = ::lcl_MinHeightDist( pTop->GetDistance() );
        long nHeight = ::lcl_AlignHeight( pTop->GetInWidth() );
        (aRect.*rRectFn->fnAddBottom)( nDist + nHeight );
        (aRect.*rRectFn->fnSubTop)( nHeight - (aRect.*rRectFn->fnGetHeight)() );
        ::lcl_SubLeftRight( aRect, rBox, rRectFn );
        pFrm->PaintBorderLine( rRect, aRect, pPage, &pTop->GetColor() );
    }
}

void MA_FASTCALL lcl_PaintBottomLine( const SwFrm *pFrm, const SwPageFrm *pPage,
                             const SwRect &rOutRect, const SwRect &rRect,
                             const SwBorderAttrs &rAttrs, SwRectFn& rRectFn )
{
    const SvxBoxItem &rBox = rAttrs.GetBox();
    const SvxBorderLine *pBottom = rBox.GetBottom();

    if ( !pBottom )
        return;

    SwRect aRect( rOutRect );
    (aRect.*rRectFn->fnSubTop)( ::lcl_AlignHeight( pBottom->GetOutWidth() ) -
                                (aRect.*rRectFn->fnGetHeight)() );
    pFrm->PaintBorderLine( rRect, aRect, pPage, &pBottom->GetColor() );

    if ( pBottom->GetInWidth() )
    {
        const long nDist = ::lcl_MinHeightDist( pBottom->GetDistance() );
        long nHeight = ::lcl_AlignHeight( pBottom->GetInWidth()  );
        (aRect.*rRectFn->fnSubTop)( nDist + nHeight );
        (aRect.*rRectFn->fnAddBottom)( nHeight -(aRect.*rRectFn->fnGetHeight)() );
        ::lcl_SubLeftRight( aRect, rBox, rRectFn );
        pFrm->PaintBorderLine( rRect, aRect, pPage, &pBottom->GetColor() );
    }
}

/*************************************************************************
|*
|*	SwFrm::PaintBorder()
|*
|*	Beschreibung		Malt Schatten und Umrandung
|*	Ersterstellung		MA 23.01.92
|*	Letzte Aenderung	MA 29. Jul. 96
|*
|*************************************************************************/

void SwFrm::PaintBorder( const SwRect& rRect, const SwPageFrm *pPage,
                         const SwBorderAttrs &rAttrs ) const
{

    //fuer (Row,Body,Ftn,Root,Column,NoTxt) gibt's hier nix zu tun
    if ( (GetType() & 0x90C5) || (Prt().SSize() == Frm().SSize()) )
        return;

    if ( (GetType() & 0x2000) && 	//Cell
         !pGlobalShell->GetViewOptions()->IsTable() )
        return;

    const FASTBOOL bLine   = rAttrs.IsLine();
    const FASTBOOL bShadow = rAttrs.GetShadow().GetLocation() != SVX_SHADOW_NONE;
    if ( bLine || bShadow )
    {
        //Wenn das Rechteck vollstandig innerhalb der PrtArea liegt,
        //so braucht kein Rand gepainted werden.
        //Fuer die PrtArea muss der Aligned'e Wert zugrunde gelegt werden,
        //anderfalls wuerden u.U. Teile nicht verarbeitet.
        SwRect aRect( Prt() );
        aRect += Frm().Pos();
        ::SwAlignRect( aRect, pGlobalShell );
        /// OD 27.09.2002 #103636# - new local boolean variable in order to
        /// suspend border paint under special cases - see below.
        /// NOTE: This is a fix for the implementation of feature #99657#.
        bool bDrawOnlyShadowForTransparentFrame = false;
        if ( aRect.IsInside( rRect ) )
        {
            /// OD 27.09.2002 #103636# - paint shadow, if background is transparent.
            /// Because of introduced transparent background for fly frame #99657#,
            /// the shadow have to be drawn if the background is transparent,
            /// in spite the fact that the paint rectangle <rRect> lies fully
            /// in the printing area.
            /// NOTE to chosen solution:
            ///     On transparent background, continue processing, but suspend
            ///     drawing of border by setting <bDrawOnlyShadowForTransparentFrame>
            ///     to true.
            if ( IsLayoutFrm() &&
                 static_cast<const SwLayoutFrm*>(this)->GetFmt()->IsBackgroundTransparent() )
            {
                 bDrawOnlyShadowForTransparentFrame = true;
            }
            else
            {
                return;
            }
        }

        if ( !pPage )
            pPage = FindPageFrm();

        ::lcl_CalcBorderRect( aRect, this, rAttrs, TRUE );
        rAttrs.SetGetCacheLine( TRUE );
        if ( bShadow )
            PaintShadow( rRect, aRect, pPage, rAttrs );
        /// OD 27.09.2002 #103636# - suspend drawing of border
        /// add condition < NOT bDrawOnlyShadowForTransparentFrame > - see above
        if ( bLine && !bDrawOnlyShadowForTransparentFrame )
        {
            SWRECTFN( this )
            ::lcl_PaintLeftLine  ( this, pPage, aRect, rRect, rAttrs, fnRect );
            ::lcl_PaintRightLine ( this, pPage, aRect, rRect, rAttrs, fnRect );
            if ( !IsCntntFrm() || rAttrs.GetTopLine( this ) )
                ::lcl_PaintTopLine( this, pPage, aRect, rRect, rAttrs, fnRect );
            if ( !IsCntntFrm() || rAttrs.GetBottomLine( this ) )
                ::lcl_PaintBottomLine(this, pPage, aRect, rRect, rAttrs,fnRect);
        }
        rAttrs.SetGetCacheLine( FALSE );
    }
}
/*************************************************************************
|*
|*	SwFtnContFrm::PaintBorder()
|*
|*	Beschreibung		Spezialimplementierung wg. der Fussnotenlinie.
|* 		Derzeit braucht nur der obere Rand beruecksichtigt werden.
|* 		Auf andere Linien und Schatten wird verzichtet.
|*	Ersterstellung		MA 27. Feb. 93
|*	Letzte Aenderung	MA 08. Sep. 93
|*
|*************************************************************************/

void SwFtnContFrm::PaintBorder( const SwRect& rRect, const SwPageFrm *pPage,
                                const SwBorderAttrs & ) const
{
    //Wenn das Rechteck vollstandig innerhalb der PrtArea liegt, so gibt es
    //keinen Rand zu painten.
    SwRect aRect( Prt() );
    aRect.Pos() += Frm().Pos();
    if ( !aRect.IsInside( rRect ) )
        PaintLine( rRect, pPage );
}
/*************************************************************************
|*
|*	SwFtnContFrm::PaintLine()
|*
|*	Beschreibung		Fussnotenline malen.
|*	Ersterstellung		MA 02. Mar. 93
|*	Letzte Aenderung	MA 28. Mar. 94
|*
|*************************************************************************/

void SwFtnContFrm::PaintLine( const SwRect& rRect,
                              const SwPageFrm *pPage ) const
{
    //Laenge der Linie ergibt sich aus der prozentualen Angabe am PageDesc.
    //Die Position ist ebenfalls am PageDesc angegeben.
    //Der Pen steht direkt im PageDesc.

    if ( !pPage )
        pPage = FindPageFrm();
    const SwPageFtnInfo &rInf = pPage->GetPageDesc()->GetFtnInfo();

    SWRECTFN( this )
    SwTwips nPrtWidth = (Prt().*fnRect->fnGetWidth)();
    Fraction aFract( nPrtWidth, 1 );
    const SwTwips nWidth = (long)(aFract *= rInf.GetWidth());

    SwTwips nX = (this->*fnRect->fnGetPrtLeft)();
    switch ( rInf.GetAdj() )
    {
        case FTNADJ_CENTER:
            nX += nPrtWidth/2 - nWidth/2; break;
        case FTNADJ_RIGHT:
            nX += nPrtWidth - nWidth; break;
        case FTNADJ_LEFT:
            /* do nothing */; break;
        default:
            ASSERT( !this, "Neues Adjustment fuer Fussnotenlinie?" );
    }
    SwTwips nLineWidth = rInf.GetLineWidth();
    const SwRect aLineRect = bVert ?
        SwRect( Point(Frm().Left()+Frm().Width()-rInf.GetTopDist()-nLineWidth,
                      nX), Size( nLineWidth, nWidth ) )
            : SwRect( Point( nX, Frm().Pos().Y() + rInf.GetTopDist() ),
                            Size( nWidth, rInf.GetLineWidth()));
    if ( aLineRect.HasArea() )
        PaintBorderLine( rRect, aLineRect , pPage, &rInf.GetLineColor() );
}

/*************************************************************************
|*
|*	SwLayoutFrm::PaintColLines()
|*
|*	Beschreibung		Painted die Trennlinien fuer die innenliegenden
|* 						Spalten.
|*	Ersterstellung		MA 21. Jun. 93
|*	Letzte Aenderung	MA 28. Mar. 94
|*
|*************************************************************************/

void SwLayoutFrm::PaintColLines( const SwRect &rRect, const SwFmtCol &rFmtCol,
                                 const SwPageFrm *pPage ) const
{
    const SwFrm *pCol = Lower();
    if ( !pCol || !pCol->IsColumnFrm() )
        return;

    SwRectFn fnRect = pCol->IsVertical() ? fnRectVert : fnRectHori;
    SwRect aLineRect = Prt();
    aLineRect += Frm().Pos();

    SwTwips nTop = ((aLineRect.*fnRect->fnGetHeight)()*rFmtCol.GetLineHeight())
                   / 100 - (aLineRect.*fnRect->fnGetHeight)();
    SwTwips nBottom = 0;

    switch ( rFmtCol.GetLineAdj() )
    {
        case COLADJ_CENTER:
            nBottom = nTop / 2; nTop -= nBottom; break;
        case COLADJ_TOP:
            nBottom = nTop; nTop = 0; break;
        case COLADJ_BOTTOM:
            break;
        default:
            ASSERT( !this, "Neues Adjustment fuer Spaltenlinie?" );
    }

    if( nTop )
        (aLineRect.*fnRect->fnSubTop)( nTop );
    if( nBottom )
        (aLineRect.*fnRect->fnAddBottom)( nBottom );

    SwTwips nPenHalf = rFmtCol.GetLineWidth();
    (aLineRect.*fnRect->fnSetWidth)( nPenHalf );
    nPenHalf /= 2;

    //Damit uns nichts verlorengeht muessen wir hier etwas grosszuegiger sein.
    SwRect aRect( rRect );
    (aRect.*fnRect->fnSubLeft)( nPenHalf + nPixelSzW );
    (aRect.*fnRect->fnAddRight)( nPenHalf + nPixelSzW );
    SwRectGet fnGetX = IsRightToLeft() ? fnRect->fnGetLeft : fnRect->fnGetRight;
    while ( pCol->GetNext() )
    {
        (aLineRect.*fnRect->fnSetPosX)
            ( (pCol->Frm().*fnGetX)() - nPenHalf );
        if ( aRect.IsOver( aLineRect ) )
            PaintBorderLine( aRect, aLineRect , pPage, &rFmtCol.GetLineColor());
        pCol = pCol->GetNext();
    }
}

void SwPageFrm::PaintGrid( OutputDevice* pOut, SwRect &rRect ) const
{
    if( !bHasGrid || pRetoucheFly || pRetoucheFly2 )
        return;
    GETGRID( this )
    if( pGrid && ( OUTDEV_PRINTER != pOut->GetOutDevType() ?
        pGrid->GetDisplayGrid() : pGrid->GetPrintGrid() ) )
    {
        const SwLayoutFrm* pBody = FindBodyCont();
        if( pBody )
        {
            SwRect aGrid( pBody->Prt() );
            aGrid += pBody->Frm().Pos();
         // ::SwAlignRect( aGrid, pGlobalShell );
            SwRect aInter( aGrid );
            aInter.Intersection( rRect );
            if( aInter.HasArea() )
            {
                BOOL bGrid = pGrid->GetRubyTextBelow();
                BOOL bCell = GRID_LINES_CHARS == pGrid->GetGridType();
                long nGrid = pGrid->GetBaseHeight();
                long nRuby = pGrid->GetRubyHeight();
                long nSum = nGrid + nRuby;
                const Color *pCol = &pGrid->GetColor();

                SwTwips nRight = aInter.Left() + aInter.Width();
                SwTwips nBottom = aInter.Top() + aInter.Height();
                if( IsVertical() )
                {
                    SwTwips nOrig = aGrid.Left() + aGrid.Width();
                    SwTwips nY = nOrig + nSum *
                                 ( ( nOrig - aInter.Left() ) / nSum );
                    SwRect aTmp( Point( nY, aInter.Top() ),
                                Size( 1, aInter.Height() ) );
                    SwTwips nX = aGrid.Top() + nGrid *
                                ( ( aInter.Top() - aGrid.Top() )/ nGrid );
                    if( nX < aInter.Top() )
                        nX += nGrid;
                    SwTwips nGridBottom = aGrid.Top() + aGrid.Height();
                    BOOL bLeft = aGrid.Top() >= aInter.Top();
                    BOOL bRight = nGridBottom <= nBottom;
                    BOOL bBorder = bLeft || bRight;
                    while( nY > nRight )
                    {
                        aTmp.Pos().X() = nY;
                        if( bGrid )
                        {
                            nY -= nGrid;
                            SwTwips nPosY = Max( aInter.Left(), nY );
                            SwTwips nHeight = Min(nRight, aTmp.Pos().X())-nPosY;
                            if( nHeight > 0 )
                            {
                                if( bCell )
                                {
                                    SwRect aVert( Point( nPosY, nX ),
                                                Size( nHeight, 1 ) );
                                    while( aVert.Top() <= nBottom )
                                    {
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                        aVert.Pos().Y() += nGrid;
                                    }
                                }
                                else if( bBorder )
                                {
                                    SwRect aVert( Point( nPosY, aGrid.Top() ),
                                                  Size( nHeight, 1 ) );
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().Y() = nGridBottom;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        else
                        {
                            nY -= nRuby;
                            if( bBorder )
                            {
                                SwTwips nPos = Max( aInter.Left(), nY );
                                SwTwips nW = Min(nRight, aTmp.Pos().X()) - nPos;
                                SwRect aVert( Point( nPos, aGrid.Top() ),
                                              Size( nW, 1 ) );
                                if( nW > 0 )
                                {
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().Y() = nGridBottom;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        bGrid = !bGrid;
                    }
                    while( nY >= aInter.Left() )
                    {
                        aTmp.Pos().X() = nY;
                        PaintBorderLine( rRect, aTmp, this, pCol);
                        if( bGrid )
                        {
                            nY -= nGrid;
                            SwTwips nHeight = aTmp.Pos().X()
                                              - Max(aInter.Left(), nY );
                            if( nHeight > 0 )
                            {
                                if( bCell )
                                {
                                    SwRect aVert( Point(aTmp.Pos().X()-nHeight,
                                                  nX ), Size( nHeight, 1 ) );
                                    while( aVert.Top() <= nBottom )
                                    {
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                        aVert.Pos().Y() += nGrid;
                                    }
                                }
                                else if( bBorder )
                                {
                                    SwRect aVert( Point(aTmp.Pos().X()-nHeight,
                                            aGrid.Top() ), Size( nHeight, 1 ) );
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().Y() = nGridBottom;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        else
                        {
                            nY -= nRuby;
                            if( bBorder )
                            {
                                SwTwips nPos = Max( aInter.Left(), nY );
                                SwTwips nW = Min(nRight, aTmp.Pos().X()) - nPos;
                                SwRect aVert( Point( nPos, aGrid.Top() ),
                                              Size( nW, 1 ) );
                                if( nW > 0 )
                                {
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().Y() = nGridBottom;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        bGrid = !bGrid;
                    }
                }
                else
                {
                    SwTwips nOrig = aGrid.Top();
                    SwTwips nY = nOrig + nSum *( (aInter.Top()-nOrig)/nSum );
                    SwRect aTmp( Point( aInter.Left(), nY ),
                                Size( aInter.Width(), 1 ) );
                    SwTwips nX = aGrid.Left() + nGrid *
                                ( ( aInter.Left() - aGrid.Left() )/ nGrid );
                    if( nX < aInter.Left() )
                        nX += nGrid;
                    SwTwips nGridRight = aGrid.Left() + aGrid.Width();
                    BOOL bLeft = aGrid.Left() >= aInter.Left();
                    BOOL bRight = nGridRight <= nRight;
                    BOOL bBorder = bLeft || bRight;
                    while( nY < aInter.Top() )
                    {
                        aTmp.Pos().Y() = nY;
                        if( bGrid )
                        {
                            nY += nGrid;
                            SwTwips nPosY = Max( aInter.Top(), aTmp.Pos().Y() );
                            SwTwips nHeight = Min(nBottom, nY ) - nPosY;
                            if( nHeight )
                            {
                                if( bCell )
                                {
                                    SwRect aVert( Point( nX, nPosY ),
                                                Size( 1, nHeight ) );
                                    while( aVert.Left() <= nRight )
                                    {
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                        aVert.Pos().X() += nGrid;
                                    }
                                }
                                else if ( bBorder )
                                {
                                    SwRect aVert( Point( aGrid.Left(), nPosY ),
                                                Size( 1, nHeight ) );
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().X() = nGridRight;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        else
                        {
                            nY += nRuby;
                            if( bBorder )
                            {
                                SwTwips nPos = Max(aInter.Top(),aTmp.Pos().Y());
                                SwTwips nH = Min( nBottom, nY ) - nPos;
                                SwRect aVert( Point( aGrid.Left(), nPos ),
                                            Size( 1, nH ) );
                                if( nH > 0 )
                                {
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().X() = nGridRight;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        bGrid = !bGrid;
                    }
                    while( nY <= nBottom )
                    {
                        aTmp.Pos().Y() = nY;
                        PaintBorderLine( rRect, aTmp, this, pCol);
                        if( bGrid )
                        {
                            nY += nGrid;
                            SwTwips nHeight = Min(nBottom, nY) - aTmp.Pos().Y();
                            if( nHeight )
                            {
                                if( bCell )
                                {
                                    SwRect aVert( Point( nX, aTmp.Pos().Y() ),
                                                Size( 1, nHeight ) );
                                    while( aVert.Left() <= nRight )
                                    {
                                        PaintBorderLine( rRect, aVert, this, pCol);
                                        aVert.Pos().X() += nGrid;
                                    }
                                }
                                else if( bBorder )
                                {
                                    SwRect aVert( Point( aGrid.Left(),
                                        aTmp.Pos().Y() ), Size( 1, nHeight ) );
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().X() = nGridRight;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        else
                        {
                            nY += nRuby;
                            if( bBorder )
                            {
                                SwTwips nPos = Max(aInter.Top(),aTmp.Pos().Y());
                                SwTwips nH = Min( nBottom, nY ) - nPos;
                                SwRect aVert( Point( aGrid.Left(), nPos ),
                                            Size( 1, nH ) );
                                if( nH > 0 )
                                {
                                    if( bLeft )
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    if( bRight )
                                    {
                                        aVert.Pos().X() = nGridRight;
                                        PaintBorderLine(rRect,aVert,this,pCol);
                                    }
                                }
                            }
                        }
                        bGrid = !bGrid;
                    }
                }
            }
        }
    }
}

/** paint margin area of a page

    OD 20.11.2002 for #104598#:
    implement paint of margin area; margin area will be painted for a
    view shell with a window and if the document is not in online layout.

    @author OD

    @param _rOutputRect
    input parameter - constant instance reference of the rectangle, for
    which an output has to be generated.

    @param _pViewShell
    input parameter - instance of the view shell, on which the output
    has to be generated.
*/
void SwPageFrm::PaintMarginArea( const SwRect& _rOutputRect,
                                 ViewShell* _pViewShell ) const
{
    if ( _pViewShell->GetWin() &&
         !_pViewShell->GetDoc()->IsBrowseMode()
       )
    {
        SwRect aPgPrtRect( Prt() );
        aPgPrtRect.Pos() += Frm().Pos();
        if ( !aPgPrtRect.IsInside( _rOutputRect ) )
        {
            SwRect aPgRect = Frm();
            aPgRect._Intersection( _rOutputRect );
            SwRegionRects aPgRegion( aPgRect );
            aPgRegion -= aPgPrtRect;
            const SwPageFrm* pPage = static_cast<const SwPageFrm*>(this);
            if ( pPage->GetSortedObjs() )
                ::lcl_SubtractFlys( this, pPage, aPgRect, aPgRegion );
            if ( aPgRegion.Count() )
            {
                OutputDevice *pOut = _pViewShell->GetOut();
                if ( pOut->GetFillColor() != aGlobalRetoucheColor )
                    pOut->SetFillColor( aGlobalRetoucheColor );
                for ( USHORT i = 0; i < aPgRegion.Count(); ++i )
                {
                    if ( 1 < aPgRegion.Count() )
                    {
                        ::SwAlignRect( aPgRegion[i], pGlobalShell );
                        if( !aPgRegion[i].HasArea() )
                            continue;
                    }
                    pOut->DrawRect( aPgRegion[i].SVRect() );
                }
            }
        }
    }
}

/*************************************************************************
|*
|*	SwFrm::PaintBaBo()
|*
|*	Ersterstellung		MA 22. Oct. 93
|*	Letzte Aenderung	MA 19. Jun. 96
|*
|*************************************************************************/

void SwFrm::PaintBaBo( const SwRect& rRect, const SwPageFrm *pPage,
                       const BOOL bLowerBorder ) const
{
    if ( !pPage )
        pPage = FindPageFrm();

    OutputDevice *pOut = pGlobalShell->GetOut();
    pOut->Push( PUSH_FILLCOLOR );

    SwBorderAttrAccess aAccess( SwFrm::GetCache(), (SwFrm*)this );
    const SwBorderAttrs &rAttrs = *aAccess.Get();

/*	//FlyInCnt's kommen hier aus der ::Retouche hier an.
    FASTBOOL bUnlock = FALSE;
    if ( IsFlyFrm() && ((SwFlyFrm*)this)->IsFlyInCntFrm() )
    {
        bLockFlyBackground = bUnlock = TRUE;
        const SwFlyFrm *pFly = (const SwFlyFrm*)this;
        SwBorderAttrAccess aAccess( SwFrm::GetCache(), pFly );
        const SwBorderAttrs &rAttrs = *aAccess.Get();
        pFly->GetAnchor()->PaintBackground( rRect, pPage, rAttrs, FALSE );
    }
*/
    // OD 20.11.2002 #104598# - take care of page margin area
    // Note: code move from <SwFrm::PaintBackground(..)> to new method
    // <SwPageFrm::Paintmargin(..)>.
    if ( IsPageFrm() )
    {
        static_cast<const SwPageFrm*>(this)->PaintMarginArea( rRect, pGlobalShell );
    }

    // OD 06.08.2002 #99657# - paint border before painting background
    // paint grid for page frame and paint border
    {
        SwRect aRect( rRect );
        if( IsPageFrm() )
            ((SwPageFrm*)this)->PaintGrid( pOut, aRect );
        PaintBorder( aRect, pPage, rAttrs );
    /*  if ( bUnlock )
            bLockFlyBackground = FALSE;
    */
    }

    // paint background
    {
        PaintBackground( rRect, pPage, rAttrs, FALSE, bLowerBorder );
    }

    pOut->Pop();
}

/*************************************************************************
|*
|*	SwFrm::PaintBackground()
|*
|*	Ersterstellung		MA 04. Jan. 93
|*	Letzte Aenderung	MA 06. Feb. 97
|*
|*************************************************************************/
/// OD 05.09.2002 #102912#
/// Do not paint background for fly frames without a background brush by
/// calling <PaintBaBo> at the page or at the fly frame its anchored
void SwFrm::PaintBackground( const SwRect &rRect, const SwPageFrm *pPage,
                              const SwBorderAttrs & rAttrs,
                             const BOOL bLowerMode,
                             const BOOL bLowerBorder ) const
{
    ViewShell *pSh = pGlobalShell;
    const FASTBOOL bWin = pSh->GetWin() ? TRUE : FALSE;
    const SvxBrushItem* pItem;
    /// OD 05.09.2002 #102912#
    /// temporary background brush for a fly frame without a background brush
    SvxBrushItem* pTmpBackBrush = 0;
    const Color* pCol;
    SwRect aOrigBackRect;
    const FASTBOOL bPageFrm = IsPageFrm();
    FASTBOOL bLowMode = TRUE;

    FASTBOOL bBack = GetBackgroundBrush( pItem, pCol, aOrigBackRect, bLowerMode );
    //- Ausgabe wenn ein eigener Hintergrund mitgebracht wird.
    bool bNoFlyBackground = !bFlyMetafile && !bBack && IsFlyFrm();
    if ( bNoFlyBackground )
    {
        /// OD 05.09.2002 #102912# - Fly frame has no background.
        /// Try to find background brush at parents, if previous call of
        /// <GetBackgroundBrush> disabled this option with the parameter <bLowerMode>
        if ( bLowerMode )
        {
            bBack = GetBackgroundBrush( pItem, pCol, aOrigBackRect, false );
        }
        /// If still no background found for the fly frame, initialize the
        /// background brush <pItem> with global retouche color and set <bBack>
        /// to TRUE, that fly frame will paint its background using this color.
        if ( !bBack )
        {
            pTmpBackBrush = new SvxBrushItem( aGlobalRetoucheColor );
            pItem = pTmpBackBrush;
            bBack = true;
        }
    }

    SwRect aPaintRect( Frm() );
    if( IsTxtFrm() )
        aPaintRect = UnionFrm( TRUE );

    if ( aPaintRect.IsOver( rRect ) )
    {
        if ( bBack || bPageFrm || !bLowerMode )
        {
            const FASTBOOL bBrowse = pSh->GetDoc()->IsBrowseMode();

            SwRect aRect;
            if ( (bPageFrm && bBrowse) ||
                 (IsTxtFrm() && Prt().SSize() == Frm().SSize()) )
            {
                aRect = Frm();
                ::SwAlignRect( aRect, pGlobalShell );
            }
            else
            {
                ::lcl_CalcBorderRect( aRect, this, rAttrs, FALSE );
                if ( (IsTxtFrm() || IsTabFrm()) && GetPrev() )
                {
                    if ( GetPrev()->GetAttrSet()->GetBackground() ==
                         GetAttrSet()->GetBackground() )
                    {
                        aRect.Top( Frm().Top() );
                    }
                }
            }
            aRect.Intersection( rRect );

            OutputDevice *pOut = pSh->GetOut();

            if ( aRect.HasArea() )
            {
                SvxBrushItem* pNewItem;
                SwRegionRects aRegion( aRect );
                if( pCol )
                {
                    pNewItem = new SvxBrushItem( *pCol );
                    pItem = pNewItem;
                }
                if ( pPage->GetSortedObjs() )
                    ::lcl_SubtractFlys( this, pPage, aRect, aRegion );

                {
                    /// OD 06.08.2002 #99657# - determine, if background transparency
                    ///     have to be considered for drawing.
                    ///     --> Status Quo: background transparency have to be
                    ///        considered for fly frames
                    const sal_Bool bConsiderBackgroundTransparency = IsFlyFrm();
                    for ( USHORT i = 0; i < aRegion.Count(); ++i )
                    {
                        if ( 1 < aRegion.Count() )
                        {
                            ::SwAlignRect( aRegion[i], pGlobalShell );
                            if( !aRegion[i].HasArea() )
                                continue;
                        }
                        /// OD 06.08.2002 #99657# - add 6th parameter to indicate, if
                        ///     background transparency have to be considered
                        ///     Set missing 5th parameter to the default value GRFNUM_NO
                        ///         - see declaration in /core/inc/frmtool.hxx.
                        ::DrawGraphic( pItem, pOut, aOrigBackRect, aRegion[i], GRFNUM_NO,
                                bConsiderBackgroundTransparency );
                    }
                }
                if( pCol )
                    delete pNewItem;
            }
        }
        else
            bLowMode = bLowerMode ? TRUE : FALSE;
    }

    /// OD 05.09.2002 #102912#
    /// delete temporary background brush.
    delete pTmpBackBrush;

    //Jetzt noch Lower und dessen Nachbarn.
    //Wenn ein Frn dabei die Kette verlaesst also nicht mehr Lower von mir ist
    //so hoert der Spass auf.
    const SwFrm *pFrm = GetLower();
    if ( pFrm )
    {
        SwRect aFrmRect;
        SwRect aRect( PaintArea() );
        aRect._Intersection( rRect );
        SwRect aBorderRect( aRect );
        SwShortCut aShortCut( *pFrm, aBorderRect );
        do
        {   if ( pProgress )
                pProgress->Reschedule();

            aFrmRect = pFrm->PaintArea();
            if ( aFrmRect.IsOver( aBorderRect ) )
            {
                SwBorderAttrAccess aAccess( SwFrm::GetCache(), (SwFrm*)pFrm );
                const SwBorderAttrs &rAttrs = *aAccess.Get();
                /// OD 06.08.2002 #99657# - paint border before painting background
                if ( bLowerBorder )
                    pFrm->PaintBorder( aBorderRect, pPage, rAttrs );
                if ( ( pFrm->IsLayoutFrm() && bLowerBorder ) ||
                     aFrmRect.IsOver( aRect ) )
                    pFrm->PaintBackground( aRect, pPage, rAttrs, bLowMode,
                                           bLowerBorder );
            }
            pFrm = pFrm->GetNext();
        } while ( pFrm && pFrm->GetUpper() == this &&
                  !aShortCut.Stop( aFrmRect ) );
    }
}

/*************************************************************************
|*
|*	SwPageFrm::RefreshSubsidiary()
|*
|*	Beschreibung		Erneuert alle Hilfslinien der Seite.
|*	Ersterstellung		MA 04. Nov. 92
|*	Letzte Aenderung	MA 10. May. 95
|*
|*************************************************************************/

void SwPageFrm::RefreshSubsidiary( const SwRect &rRect ) const
{
    if ( IS_SUBS || IS_SUBS_TABLE || IS_SUBS_SECTION || IS_SUBS_FLYS )
    {
        SwRect aRect( rRect );
        ::SwAlignRect( aRect, pGlobalShell );
        if ( aRect.HasArea() )
        {
            //Beim Paint ueber die Root wird das Array von dort gesteuert.
            //Anderfalls kuemmern wir uns selbst darum.
            FASTBOOL bDelSubs = FALSE;
            if ( !pSubsLines )
            {
                pSubsLines = new SwSubsRects;
                bDelSubs = TRUE;
            }

            RefreshLaySubsidiary( this, aRect );
            if ( GetSortedObjs() )
            {
                const SwSortDrawObjs &rObjs = *GetSortedObjs();
                for ( USHORT i = 0; i < rObjs.Count(); ++i )
                {
                    SdrObject *pO = rObjs[i];
                    if ( pO->IsWriterFlyFrame() )
                    {
                        const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                        if ( pFly->Frm().IsOver( aRect ) )
                        {
                            if ( !pFly->Lower() || !pFly->Lower()->IsNoTxtFrm() ||
                                    !((SwNoTxtFrm*)pFly->Lower())->HasAnimation())
                                pFly->RefreshLaySubsidiary( this, aRect );
                        }
                    }
                }
            }
            if ( bDelSubs )
            {
                pSubsLines->PaintSubsidiary( pGlobalShell->GetOut(), pLines );
                DELETEZ( pSubsLines );
            }
        }
    }
}

/*************************************************************************
|*
|*	SwLayoutFrm::RefreshLaySubsidiary()
|*
|*	Ersterstellung		MA 04. Nov. 92
|*	Letzte Aenderung	MA 22. Jan. 95
|*
|*************************************************************************/
void SwLayoutFrm::RefreshLaySubsidiary( const SwPageFrm *pPage,
                                        const SwRect &rRect ) const
{
    const FASTBOOL bNoLowerColumn = !Lower() || !Lower()->IsColumnFrm();
    const FASTBOOL bSubsOpt	  = IS_SUBS;
    const FASTBOOL bSubsTable = ((GetType() & (FRM_ROW | FRM_CELL)) && IS_SUBS_TABLE);
    const FASTBOOL bSubsOther = (GetType() & (FRM_HEADER | FRM_FOOTER | FRM_FTN )) && bSubsOpt;
    const FASTBOOL bSubsSect  = IsSctFrm() &&
                                bNoLowerColumn &&
                                IS_SUBS_SECTION;
    const FASTBOOL bSubsFly   = IS_SUBS_FLYS &&
                                (GetType() & FRM_FLY) &&
                                bNoLowerColumn &&
                                (!Lower() || !Lower()->IsNoTxtFrm() ||
                                 !((SwNoTxtFrm*)Lower())->HasAnimation());
    FASTBOOL bSubsBody = FALSE;
    if ( GetType() & FRM_BODY )
    {
        if ( IsPageBodyFrm() )
            bSubsBody = bSubsOpt && bNoLowerColumn;									//nur ohne Spalten
        else	//Spaltenbody
        {
            if ( GetUpper()->GetUpper()->IsSctFrm() )
                bSubsBody = IS_SUBS_SECTION;
            else
                bSubsBody = bSubsOpt;
        }
    }

    if ( bSubsOther || bSubsSect  || bSubsBody || bSubsTable || bSubsFly )
        PaintSubsidiaryLines( pPage, rRect );

    const SwFrm *pLow = Lower();
    if( !pLow )
        return;
    SwShortCut aShortCut( *pLow, rRect );
    while( pLow && !aShortCut.Stop( pLow->Frm() ) )
    {
        if ( pLow->Frm().IsOver( rRect ) && pLow->Frm().HasArea() )
        {
            if ( pLow->IsLayoutFrm() )
                ((const SwLayoutFrm*)pLow)->RefreshLaySubsidiary( pPage, rRect);
            else if ( pLow->GetDrawObjs() )
            {
                const SwDrawObjs &rObjs = *pLow->GetDrawObjs();
                for ( USHORT i = 0; i < rObjs.Count(); ++i )
                {
                    SdrObject *pO = rObjs[i];
                    if ( pO->IsWriterFlyFrame() )
                    {
                        const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                        if ( pFly->IsFlyInCntFrm() && pFly->Frm().IsOver( rRect ) )
                        {
                            if ( !pFly->Lower() || !pFly->Lower()->IsNoTxtFrm() ||
                                 !((SwNoTxtFrm*)pFly->Lower())->HasAnimation())
                                pFly->RefreshLaySubsidiary( pPage, rRect );
                        }
                    }
                }
            }
        }
        pLow = pLow->GetNext();
    }
}

/*************************************************************************
|*
|*	SwLayoutFrm::PaintSubsidiaryLines()
|*
|*	Beschreibung		Hilfslinien um die PrtAreas malen
|* 		Nur die LayoutFrm's die direkt Cntnt enthalten.
|*	Ersterstellung		MA 21. May. 92
|*	Letzte Aenderung	MA 22. Jan. 95
|*
|*************************************************************************/

//Malt die angegebene Linie, achtet darauf, dass keine Flys uebermalt werden.
PointPtr pX = &Point::nA;
PointPtr pY = &Point::nB;
SizePtr pWidth = &Size::nA;
SizePtr pHeight = &Size::nB;

// OD 18.11.2002 #99672# - new parameter <_pSubsLines>
void MA_FASTCALL lcl_RefreshLine( const SwLayoutFrm *pLay,
                                  const SwPageFrm *pPage,
                                  const Point &rP1,
                                  const Point &rP2,
                                  const BYTE nSubColor,
                                  SwLineRects* _pSubsLines )
{
    //In welche Richtung gehts? Kann nur Horizontal oder Vertikal sein.
    ASSERT( ((rP1.X() == rP2.X()) || (rP1.Y() == rP2.Y())),
            "Schraege Hilfslinien sind nicht erlaubt." );
    const PTPTR pDirPt = rP1.X() == rP2.X() ? pY : pX;
    const PTPTR pOthPt = pDirPt == pX ? pY : pX;
    const SIZEPTR pDirSz = pDirPt == pX ? pWidth : pHeight;
    const SIZEPTR pOthSz = pDirSz == pWidth ? pHeight : pWidth;
    Point aP1( rP1 ),
          aP2( rP2 );

    while ( aP1.*pDirPt < aP2.*pDirPt )
    {	//Der Startpunkt wird jetzt, falls er in einem Fly sitzt, direkt
        //hinter den Fly gesetzt.
        //Wenn der Endpunkt in einem Fly sitzt oder zwischen Start und Endpunkt
        //ein Fly sitzt, so wird der Endpunkt eben an den Start herangezogen.
        //Auf diese art und weise wird eine Portion nach der anderen
        //ausgegeben.

        //Wenn ich selbst ein Fly bin, weiche ich nur denjenigen Flys aus,
        //die 'ueber' mir sitzen; d.h. die in dem Array hinter mir stehen.
        //Auch wenn ich in einem Fly sitze oder in einem Fly im Fly usw. weiche
        //ich keinem dieser Flys aus.
        SwOrderIter aIter( pPage );
        const SwFlyFrm *pMyFly = pLay->FindFlyFrm();
        if ( pMyFly )
        {
            aIter.Current( pMyFly->GetVirtDrawObj() );
            while ( 0 != (pMyFly = pMyFly->GetAnchor()->FindFlyFrm()) )
            {
                if ( aIter()->GetOrdNum() > pMyFly->GetVirtDrawObj()->GetOrdNum() )
                    aIter.Current( pMyFly->GetVirtDrawObj() );
            }
        }
        else
            aIter.Bottom();

        while ( aIter() )
        {
            const SwVirtFlyDrawObj *pObj = (SwVirtFlyDrawObj*)aIter();
            const SwFlyFrm *pFly = pObj ? pObj->GetFlyFrm() : 0;

            //Mir selbst weiche ich natuerlich nicht aus. Auch wenn ich
            //_in_ dem Fly sitze weiche ich nicht aus.
            if ( !pFly || (pFly == pLay || pFly->IsAnLower( pLay )) )
            {	aIter.Next();
                continue;
            }

            //Sitzt das Obj auf der Linie
            const Rectangle &rBound = pObj->GetBoundRect();
            const Point aDrPt( rBound.TopLeft() );
            const Size  aDrSz( rBound.GetSize() );
            if ( rP1.*pOthPt >= aDrPt.*pOthPt &&
                 rP1.*pOthPt <= (aDrPt.*pOthPt + aDrSz.*pOthSz) )
            {
                if ( aP1.*pDirPt >= aDrPt.*pDirPt &&
                         aP1.*pDirPt <= (aDrPt.*pDirPt + aDrSz.*pDirSz) )
                    aP1.*pDirPt = aDrPt.*pDirPt + aDrSz.*pDirSz;

                if ( aP2.*pDirPt >= aDrPt.*pDirPt &&
                     aP1.*pDirPt < (aDrPt.*pDirPt - 1) )
                    aP2.*pDirPt = aDrPt.*pDirPt - 1;
            }
            aIter.Next();
        }

        if ( aP1.*pDirPt < aP2.*pDirPt )
        {
            SwRect aRect( aP1, aP2 );
            // OD 18.11.2002 #99672# - use parameter <_pSubsLines> instead of
            // global variable <pSubsLines>.
            _pSubsLines->AddLineRect( aRect, 0, 0, nSubColor );
        }
        aP1 = aP2;
        aP1.*pDirPt += 1;
        aP2 = rP2;
    }
}

void SwLayoutFrm::PaintSubsidiaryLines( const SwPageFrm *pPage,
                                        const SwRect &rRect ) const
{
    const bool bFlys = pPage->GetSortedObjs() ? true : false;

    const bool bCell = IsCellFrm() ? true : false;
    // use frame area for cells
    SwRect aOriginal( bCell ? Frm() : Prt() );
    if ( !bCell )
        aOriginal.Pos() += Frm().Pos();
    ::SwAlignRect( aOriginal, pGlobalShell );

    if ( !aOriginal.IsOver( rRect ) )
        return;

    SwRect aOut( aOriginal );
    aOut._Intersection( rRect );
    aOut.Intersection( PaintArea() );

    const SwTwips nRight = aOut.Right();
    const SwTwips nBottom= aOut.Bottom();

    const Point aRT( nRight, aOut.Top() );
    const Point aRB( nRight, nBottom );
    const Point aLB( aOut.Left(), nBottom );

    BYTE nSubColor = ( bCell || IsRowFrm() ) ? SUBCOL_TAB :
                     ( IsInSct() ? SUBCOL_SECT :
                     ( IsInFly() ? SUBCOL_FLY : SUBCOL_PAGE ) );

    // OD 05.11.2002 #102406# - body frames are responsible for page/column breaks.
    BOOL bBreak = FALSE;
    if ( IsBodyFrm() )
    {
        const SwCntntFrm *pCnt = ContainsCntnt();
        if ( pCnt )
        {
            // OD 05.11.2002 #102406# - adjust setting of <bBreak>.
            bBreak = pCnt->IsPageBreak( TRUE ) ||
                     ( IsColBodyFrm() && pCnt->IsColBreak( TRUE ) );
        }
    }

    // OD 18.11.2002 #99672# - collect body, header, footer, footnote and section
    // sub-lines in <pSpecSubsLine> array.
    const bool bSpecialSublines = IsBodyFrm() || IsHeaderFrm() || IsFooterFrm() ||
                                  IsFtnFrm() || IsSctFrm();
    SwLineRects* pUsedSubsLines = bSpecialSublines ? pSpecSubsLines : pSubsLines;

    // NOTE: for cell frames only left and right (horizontal layout) respectively
    //      top and bottom (vertical layout) lines painted.
    const bool bVert = IsVertical() ? true : false;
    if ( bFlys )
    {
        // OD 14.11.2002 #104822# - add control for drawing left and right lines
        if ( !bCell || !bVert )
        {
            if ( aOriginal.Left() == aOut.Left() )
                ::lcl_RefreshLine( this, pPage, aOut.Pos(), aLB, nSubColor,
                                   pUsedSubsLines );
            // OD 14.11.2002 #104821# - in vertical layout set page/column break at right
            if ( aOriginal.Right() == nRight )
                ::lcl_RefreshLine( this, pPage, aRT, aRB,
                                   (bBreak && bVert) ? SUBCOL_BREAK : nSubColor,
                                   pUsedSubsLines );
        }
        // OD 14.11.2002 #104822# - adjust control for drawing top and bottom lines
        if ( !bCell || bVert )
        {
            if ( aOriginal.Top() == aOut.Top() )
                // OD 14.11.2002 #104821# - in horizontal layout set page/column break at top
                ::lcl_RefreshLine( this, pPage, aOut.Pos(), aRT,
                                   (bBreak && !bVert) ? SUBCOL_BREAK : nSubColor,
                                   pUsedSubsLines );
            if ( aOriginal.Bottom() == nBottom )
                ::lcl_RefreshLine( this, pPage, aLB, aRB, nSubColor,
                                   pUsedSubsLines );
        }
    }
    else
    {
        // OD 14.11.2002 #104822# - add control for drawing left and right lines
        if ( !bCell || !IsVertical() )
        {
            if ( aOriginal.Left() == aOut.Left() )
            {
                SwRect aRect( aOut.Pos(), aLB );
                pUsedSubsLines->AddLineRect( aRect, 0, 0, nSubColor );
            }
            // OD 14.11.2002 #104821# - in vertical layout set page/column break at right
            if ( aOriginal.Right() == nRight )
            {
                SwRect aRect( aRT, aRB );
                pUsedSubsLines->AddLineRect( aRect, 0, 0,
                        (bBreak && bVert) ? SUBCOL_BREAK : nSubColor );
            }
        }
        // OD 14.11.2002 #104822# - adjust control for drawing top and bottom lines
        if ( !bCell || IsVertical() )
        {
            if ( aOriginal.Top() == aOut.Top() )
            {
                // OD 14.11.2002 #104821# - in horizontal layout set page/column break at top
                SwRect aRect( aOut.Pos(), aRT );
                pUsedSubsLines->AddLineRect( aRect, 0, 0,
                        (bBreak && !bVert) ? SUBCOL_BREAK : nSubColor );
            }
            if ( aOriginal.Bottom() == nBottom )
            {
                SwRect aRect( aLB, aRB );
                pUsedSubsLines->AddLineRect( aRect, 0, 0, nSubColor );
            }
        }
    }
}

/*************************************************************************
|*
|*	SwPageFrm::RefreshExtraData(), SwLayoutFrm::RefreshExtraData()
|*
|*	Beschreibung		Erneuert alle Extradaten (Zeilennummern usw) der Seite.
|* 						Grundsaetzlich sind nur diejenigen Objekte beruecksichtig,
|* 						die in die seitliche Ausdehnung des Rects ragen.
|*	Ersterstellung		MA 20. Jan. 98
|*	Letzte Aenderung	MA 18. Feb. 98
|*
|*************************************************************************/

void SwPageFrm::RefreshExtraData( const SwRect &rRect ) const
{
    const SwLineNumberInfo &rInfo = GetFmt()->GetDoc()->GetLineNumberInfo();
    FASTBOOL bLineInFly = rInfo.IsPaintLineNumbers() && rInfo.IsCountInFlys()
        || (SwHoriOrient)SW_MOD()->GetRedlineMarkPos() != HORI_NONE;

    SwRect aRect( rRect );
    ::SwAlignRect( aRect, pGlobalShell );
    if ( aRect.HasArea() )
    {
        SwLayoutFrm::RefreshExtraData( aRect );

        if ( bLineInFly && GetSortedObjs() )
            for ( USHORT i = 0; i < GetSortedObjs()->Count(); ++i )
            {
                SdrObject *pO = (*GetSortedObjs())[i];
                if ( pO->IsWriterFlyFrame() )
                {
                    const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                    if ( pFly->Frm().Top() <= aRect.Bottom() &&
                         pFly->Frm().Bottom() >= aRect.Top() )
                        pFly->RefreshExtraData( aRect );
                }
            }
    }
}

void SwLayoutFrm::RefreshExtraData( const SwRect &rRect ) const
{

    const SwLineNumberInfo &rInfo = GetFmt()->GetDoc()->GetLineNumberInfo();
    FASTBOOL bLineInBody = rInfo.IsPaintLineNumbers(),
             bLineInFly  = bLineInBody && rInfo.IsCountInFlys(),
             bRedLine = (SwHoriOrient)SW_MOD()->GetRedlineMarkPos()!=HORI_NONE;

    const SwCntntFrm *pCnt = ContainsCntnt();
    while ( pCnt && IsAnLower( pCnt ) )
    {
        if ( pCnt->IsTxtFrm() && ( bRedLine ||
             ( !pCnt->IsInTab() &&
               ((bLineInBody && pCnt->IsInDocBody()) ||
               (bLineInFly  && pCnt->IsInFly())) ) ) &&
             pCnt->Frm().Top() <= rRect.Bottom() &&
             pCnt->Frm().Bottom() >= rRect.Top() )
        {
            ((SwTxtFrm*)pCnt)->PaintExtraData( rRect );
        }
        if ( bLineInFly && pCnt->GetDrawObjs() )
            for ( USHORT i = 0; i < pCnt->GetDrawObjs()->Count(); ++i )
            {
                SdrObject *pO = (*pCnt->GetDrawObjs())[i];
                if ( pO->IsWriterFlyFrame() )
                {
                    const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                    if ( pFly->IsFlyInCntFrm() &&
                         pFly->Frm().Top() <= rRect.Bottom() &&
                         pFly->Frm().Bottom() >= rRect.Top() )
                        pFly->RefreshExtraData( rRect );
                }
        }
        pCnt = pCnt->GetNextCntntFrm();
    }
}

/** SwPageFrm::GetDrawBackgrdColor - for #102450#

    determine the color, that is respectively will be drawn as background
    for the page frame.
    Using existing method SwFrm::GetBackgroundBrush to determine the color
    that is set at the page frame respectively is parent. If none is found
    return the global retouche color

    @author OD

    @return Color
*/
const Color& SwPageFrm::GetDrawBackgrdColor() const
{
    const SvxBrushItem* pBrushItem;
    const Color* pDummyColor;
    SwRect aDummyRect;
    if ( GetBackgroundBrush( pBrushItem, pDummyColor, aDummyRect, true) )
        return pBrushItem->GetColor();
    else
        return aGlobalRetoucheColor;
}


/*************************************************************************
|*
|*	  SwFrm::Retouche
|*
|*	  Beschreibung		Retouche fuer einen Bereich.
|*		Retouche wird nur dann durchgefuehrt, wenn der Frm der letzte seiner
|* 		Kette ist. Der Gesamte Bereich des Upper unterhalb des Frm wird
|* 		per PaintBackground gecleared.
|*	  Ersterstellung	MA 13. Apr. 93
|*	  Letzte Aenderung	MA 25. Jul. 96
|*
|*************************************************************************/

void SwFrm::Retouche( const SwPageFrm * pPage, const SwRect &rRect ) const
{
    if ( bFlyMetafile )
        return;

    ASSERT( GetUpper(), "Retoucheversuch ohne Upper." );
    ASSERT( GetShell() && pGlobalShell->GetWin(), "Retouche auf dem Drucker?" );

    SwRect aRetouche( GetUpper()->PaintArea() );
    aRetouche.Top( Frm().Top() + Frm().Height() );
    aRetouche.Intersection( pGlobalShell->VisArea() );

    if ( aRetouche.HasArea() )
    {
        //Uebergebenes Rect ausparen. Dafuer brauchen wir leider eine Region
        //zum ausstanzen.
        SwRegionRects aRegion( aRetouche );
        aRegion -= rRect;
        ViewShell *pSh = GetShell();
        for ( USHORT i = 0; i < aRegion.Count(); ++i )
        {
            SwRect &rRetouche = aRegion[i];

            GetUpper()->PaintBaBo( rRetouche, pPage, TRUE );

            //Hoelle und Himmel muessen auch refreshed werden.
            //Um Rekursionen zu vermeiden muss mein Retouche Flag zuerst
            //zurueckgesetzt werden!
            ResetRetouche();
            SwRect aRetouchePart( rRetouche );
            if ( aRetouchePart.HasArea() )
            {
                /// OD 30.08.2002 #102450#
                /// determine background color of page for <PaintLayer> method
                /// calls, painting <hell> or <heaven>
                const Color aPageBackgrdColor = pPage->GetDrawBackgrdColor();
                /// OD 29.08.2002 #102450#
                /// add 3rd parameter to <PaintLayer> method calls
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetHellId(),
                                        aRetouchePart, &aPageBackgrdColor );
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetHeavenId(),
                                        aRetouchePart, &aPageBackgrdColor );
                pSh->Imp()->PaintLayer( pSh->GetDoc()->GetControlsId(),
                                        aRetouchePart );
            }

            SetRetouche();

            //Da wir uns ausserhalb aller Paint-Bereiche begeben muessen hier
            //leider die Hilfslinien erneuert werden.
            pPage->RefreshSubsidiary( aRetouchePart );
        }
    }
    if ( ViewShell::IsLstEndAction() )
        ResetRetouche();
}

/** SwFrm::GetBackgroundBrush

    @descr
    determine the background brush for the frame:
    the background brush is taken from it-self or from its parent (anchor/upper).
    Normally, the background brush is taken, which has no transparent color or
    which has a background graphic. But there are some special cases:
    (1) No background brush is taken from a page frame, if view option "IsPageBack"
        isn't set.
    (2) Background brush from a index section is taken under special conditions.
        In this case parameter <rpCol> is set to the index shading color.
    (3) New (OD 20.08.2002) - Background brush is taken, if on background drawing
        of the frame transparency is considered and its color is not "no fill"/"auto fill"
    ---- old description in german:
    Beschreibung        Liefert die Backgroundbrush fuer den Bereich des
        des Frm. Die Brush wird entweder von ihm selbst oder von einem
        Upper vorgegeben, die erste Brush wird benutzt.
        Ist fuer keinen Frm eine Brush angegeben, so wird FALSE zurueck-
        geliefert.
    Ersterstellung      MA 23. Dec. 92
    Letzte Aenderung    MA 04. Feb. 97

    @param rpBrush
    output parameter - constant reference pointer the found background brush

    @param rpCol
    output parameter - constant reference pointer to the color of the index shading
    set under special conditions, if background brush is taken from an index section.

    @param rOrigRect
    in-/output parameter - reference to the retangle the background brush is
    considered for - adjusted to the frame, from which the background brush is
    taken.

    @parem bLowerMode
    input parameter - boolean indicating, if background brush should *not* be
    taken from parent.

    @author MA
    @change 20.08.2002 by OD
    @docdate 20.08.2002

    @return true, if a background brush for the frame is found
*/
BOOL SwFrm::GetBackgroundBrush( const SvxBrushItem* & rpBrush,
                                const Color*& rpCol,
                                SwRect &rOrigRect,
                                BOOL bLowerMode ) const
{
    const SwFrm *pFrm = this;
    ViewShell *pSh = GetShell();
    const SwViewOption *pOpt = pSh->GetViewOptions();
    rpBrush = 0;
    rpCol = NULL;
    do
    {	if ( pFrm->IsPageFrm() && !pOpt->IsPageBack() )
            return FALSE;

        const SvxBrushItem &rBack = pFrm->GetAttrSet()->GetBackground();
        if( pFrm->IsSctFrm() )
        {
            const SwSection* pSection = ((SwSectionFrm*)pFrm)->GetSection();
            /// OD 20.08.2002 #99657# #GetTransChg#
            ///     Note: If frame <pFrm> is a section of the index and
            ///         it its background color is "no fill"/"auto fill" and
            ///         it has no background graphic and
            ///         we are not in the page preview and
            ///         we are not in read-only mode and
            ///         option "index shadings" is set and
            ///         the output is not the printer
            ///         then set <rpCol> to the color of the index shading
            if( pSection && (   TOX_HEADER_SECTION == pSection->GetType() ||
                                TOX_CONTENT_SECTION == pSection->GetType() ) &&
                (rBack.GetColor() == COL_TRANSPARENT) &&
                ///rBack.GetColor().GetTransparency() &&
                rBack.GetGraphicPos() == GPOS_NONE &&
                !pOpt->IsPagePreview() && !pOpt->IsReadonly() &&
                SwViewOption::IsIndexShadings() &&
                !pOpt->IsPDFExport() &&
                pSh->GetOut()->GetOutDevType() != OUTDEV_PRINTER )
            {
                rpCol = &SwViewOption::GetIndexShadingsColor();
            }
        }

        /// OD 20.08.2002 #99657#
        ///     determine, if background draw of frame <pFrm> considers transparency
        ///     --> Status Quo: background transparency have to be
        ///                     considered for fly frames
        const sal_Bool bConsiderBackgroundTransparency = pFrm->IsFlyFrm();
        /// OD 20.08.2002 #99657#
        ///     add condition:
        ///     If <bConsiderBackgroundTransparency> is set - see above -,
        ///     return brush of frame <pFrm>, if its color is *not* "no fill"/"auto fill"
        if ( !rBack.GetColor().GetTransparency() ||
             rBack.GetGraphicPos() != GPOS_NONE ||
             rpCol ||
             (bConsiderBackgroundTransparency && (rBack.GetColor() != COL_TRANSPARENT))
           )
        {
            rpBrush = &rBack;
            if ( pFrm->IsPageFrm() && pSh->GetDoc()->IsBrowseMode() )
                rOrigRect = pFrm->Frm();
            else
            {
                if ( pFrm->Frm().SSize() != pFrm->Prt().SSize() )
                {
                    SwBorderAttrAccess aAccess( SwFrm::GetCache(), pFrm );
                    const SwBorderAttrs &rAttrs = *aAccess.Get();
                    ::lcl_CalcBorderRect( rOrigRect, pFrm, rAttrs, FALSE );
                }
                else
                {
                    rOrigRect = pFrm->Prt();
                    rOrigRect += pFrm->Frm().Pos();
                }
            }
            return TRUE;
        }

        if ( bLowerMode )
            /// Do not try to get background brush from parent (anchor/upper)
            return FALSE;

        /// get parent frame - anchor or upper - for next loop
        if ( pFrm->IsFlyFrm() )
            /// OD 20.08.2002 - use "static_cast" instead of "old C-cast"
            pFrm = (static_cast<const SwFlyFrm*>(pFrm))->GetAnchor();
            ///pFrm = ((SwFlyFrm*)pFrm)->GetAnchor();
        else
            pFrm = pFrm->GetUpper();

    } while ( pFrm );

    return FALSE;
}

/*************************************************************************
|*
|*	SwFrmFmt::GetGraphic()
|*
|*	Ersterstellung		MA 23. Jul. 96
|*	Letzte Aenderung	MA 23. Jul. 96
|*
|*************************************************************************/

void SetOutDevAndWin( ViewShell *pSh, OutputDevice *pO,
                      Window *pW, USHORT nZoom )
{
    pSh->pOut = pO;
    pSh->pWin = pW;
    pSh->pOpt->SetZoom( nZoom );
}

Graphic SwFrmFmt::MakeGraphic( ImageMap* pMap )
{
    return Graphic();
}

Graphic SwFlyFrmFmt::MakeGraphic( ImageMap* pMap )
{
    Graphic aRet;
    //irgendeinen Fly suchen!
    SwClientIter aIter( *this );
    SwClient *pFirst = aIter.First( TYPE(SwFrm) );
    ViewShell *pSh;
    if ( pFirst && 0 != ( pSh = ((SwFrm*)pFirst)->GetShell()) )
    {
        ViewShell *pOldGlobal = pGlobalShell;
        pGlobalShell = pSh;

        FASTBOOL bNoteURL = pMap &&
            SFX_ITEM_SET != GetAttrSet().GetItemState( RES_URL, TRUE );
        if( bNoteURL )
        {
            ASSERT( !pNoteURL, "MakeGraphic: pNoteURL already used? " );
            pNoteURL = new SwNoteURL;
        }
        SwFlyFrm *pFly = (SwFlyFrm*)pFirst;

        OutputDevice *pOld = pSh->GetOut();
        VirtualDevice aDev( *pOld );
        aDev.EnableOutput( FALSE );

        GDIMetaFile aMet;
        MapMode aMap( pOld->GetMapMode().GetMapUnit() );
        aDev.SetMapMode( aMap );
        aMet.SetPrefMapMode( aMap );

        ::SwCalcPixStatics( pSh->GetOut() );
        aMet.SetPrefSize( pFly->Frm().SSize() );

        aMet.Record( &aDev );
        aDev.SetLineColor();
        aDev.SetFillColor();
        aDev.SetFont( pOld->GetFont() );

        Window *pWin = pSh->GetWin();
        USHORT nZoom = pSh->GetViewOptions()->GetZoom();
        ::SetOutDevAndWin( pSh, &aDev, 0, 100 );
        bFlyMetafile = TRUE;
        pFlyMetafileOut = pWin;

        SwViewImp *pImp = pSh->Imp();
        pFlyOnlyDraw = pFly;
        pLines = new SwLineRects;

        //Rechteck ggf. ausdehnen, damit die Umrandunge mit aufgezeichnet werden.
        SwRect aOut( pFly->Frm() );
        SwBorderAttrAccess aAccess( SwFrm::GetCache(), pFly );
        const SwBorderAttrs &rAttrs = *aAccess.Get();
        if ( rAttrs.CalcRightLine() )
            aOut.SSize().Width() += 2*nPixelSzW;
        if ( rAttrs.CalcBottomLine() )
            aOut.SSize().Height()+= 2*nPixelSzH;

        /// OD 30.08.2002 #102450#
        /// determine color of page, the fly frame is on, for <PaintLayer> method
        /// calls, painting <hell> or <heaven>
        const Color aPageBackgrdColor = pFly->FindPageFrm()->GetDrawBackgrdColor();
        /// OD 30.08.2002 #102450# - add 3rd parameter
        pImp->PaintLayer( pSh->GetDoc()->GetHellId(), aOut, &aPageBackgrdColor );
        pLines->PaintLines( &aDev );
        if ( pFly->IsFlyInCntFrm() )
            pFly->Paint( aOut );
        pLines->PaintLines( &aDev );
        /// OD 30.08.2002 #102450# - add 3rd parameter
        pImp->PaintLayer( pSh->GetDoc()->GetHeavenId(), aOut, &aPageBackgrdColor );
        pLines->PaintLines( &aDev );
        if( pSh->GetViewOptions()->IsControl() )
        {
            pImp->PaintLayer( pSh->GetDoc()->GetControlsId(), aOut );
            pLines->PaintLines( &aDev );
        }
        DELETEZ( pLines );
        pFlyOnlyDraw = 0;

        pFlyMetafileOut = 0;
        bFlyMetafile = FALSE;
        ::SetOutDevAndWin( pSh, pOld, pWin, nZoom );

        aMet.Stop();
        aMet.Move( -pFly->Frm().Left(), -pFly->Frm().Top() );
        aRet = Graphic( aMet );

        if( bNoteURL )
        {
            ASSERT( pNoteURL, "MakeGraphic: Good Bye, NoteURL." );
            pNoteURL->FillImageMap( pMap, pFly->Frm().Pos(), aMap );
            delete pNoteURL;
            pNoteURL = NULL;
        }
        pGlobalShell = pOldGlobal;
    }
    return aRet;
}

Graphic SwDrawFrmFmt::MakeGraphic( ImageMap* pMap )
{
    Graphic aRet;
    SdrModel *pMod = GetDoc()->GetDrawModel();
    if ( pMod )
    {
        SdrObject *pObj = FindSdrObject();
        SdrView *pView = new SdrView( pMod );
        SdrPageView *pPgView = pView->ShowPagePgNum( 0, Point() );
        pView->MarkObj( pObj, pPgView );
        aRet = pView->GetMarkedObjBitmap();
        pView->HidePage( pPgView );
        delete pView;
    }
    return aRet;
}
