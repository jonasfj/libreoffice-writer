/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: trvlfrm.cxx,v $
 * $Revision: 1.63 $
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
#include <hints.hxx>
#include <tools/bigint.hxx>
#include <editeng/protitem.hxx>
#include <vcl/settings.hxx>
#include <vcl/outdev.hxx>
#include <fmtpdsc.hxx>
#include <fmtsrnd.hxx>
#include <pagedesc.hxx>
#include <pagefrm.hxx>
#include <rootfrm.hxx>
#include <cntfrm.hxx>
#include <ftnfrm.hxx>
#include <flyfrm.hxx>
#include <tabfrm.hxx>
#include <rowfrm.hxx>
#include <cellfrm.hxx>
#include <txtfrm.hxx>
#include <viewsh.hxx>
#include <viewopt.hxx>
#include <doc.hxx>
#include <viscrs.hxx>
#include <frmfmt.hxx>
#include <swtable.hxx>
#include <dflyobj.hxx>
#include <crstate.hxx>
#include <frmtool.hxx>
#include <ndtxt.hxx>
// OD 2004-05-24 #i28701#
#include <sortedobjs.hxx>

// FLT_MAX
#include <cfloat>
#include <swselectionlist.hxx>

//Fuer SwFlyFrm::GetCrsrOfst
class SwCrsrOszControl
{
public:
    // damit schon der Compiler die Klasse initialisieren kann, keinen
    // DTOR und member als publics:
    const SwFlyFrm *pEntry;
    const SwFlyFrm *pStk1;
    const SwFlyFrm *pStk2;

//public:
//    SwCrsrOszControl() : pStk1( 0 ), pStk2( 0 ) {}; // ; <- ????

    BOOL ChkOsz( const SwFlyFrm *pFly )
        {
            BOOL bRet = TRUE;
            if ( pFly != pStk1 && pFly != pStk2 )
            {
                pStk1 = pStk2;
                pStk2 = pFly;
                bRet  = FALSE;
            }
            return bRet;
        }
    void Entry( const SwFlyFrm *pFly )
        {
            if ( !pEntry )
                pEntry = pStk1 = pFly;
        }
    void Exit( const SwFlyFrm *pFly )
        {
            if ( pFly == pEntry )
                pEntry = pStk1 = pStk2 = 0;
        }
};

static SwCrsrOszControl aOszCtrl = { 0, 0, 0 };

/*************************************************************************
|*
|*	SwLayoutFrm::GetCrsrOfst()
|*
|*	Beschreibung:		Sucht denjenigen CntntFrm, innerhalb dessen
|* 						PrtArea der Point liegt.
|*	Ersterstellung		MA 20. Jul. 92
|*	Letzte Aenderung	MA 23. May. 95
|*
|*************************************************************************/
BOOL SwLayoutFrm::GetCrsrOfst( SwPosition *pPos, Point &rPoint,
                               SwCrsrMoveState* pCMS ) const
{
    BOOL bRet = FALSE;
    const SwFrm *pFrm = Lower();
    while ( !bRet && pFrm )
    {
        pFrm->Calc();

        // --> FME 2005-05-13 #i43742# New function: SW_CONTENT_CHECK
        const bool bCntntCheck = pFrm->IsTxtFrm() && pCMS && pCMS->bCntntCheck;
        const SwRect aPaintRect( bCntntCheck ?
                                 pFrm->UnionFrm() :
                                 pFrm->PaintArea() );
        // <--

        if ( aPaintRect.IsInside( rPoint ) &&
             ( bCntntCheck || pFrm->GetCrsrOfst( pPos, rPoint, pCMS ) ) )
            bRet = TRUE;
        else
            pFrm = pFrm->GetNext();
        if ( pCMS && pCMS->bStop )
            return FALSE;
    }
    return bRet;
}

/*************************************************************************
|*
|*	SwPageFrm::GetCrsrOfst()
|*
|*	Beschreibung:		Sucht die Seite, innerhalb der der gesuchte Point
|*						liegt.
|*	Ersterstellung		MA 20. Jul. 92
|*	Letzte Aenderung	MA 18. Jul. 96
|*
|*************************************************************************/

BOOL SwPageFrm::GetCrsrOfst( SwPosition *pPos, Point &rPoint,
                             SwCrsrMoveState* pCMS ) const
{
    BOOL bRet	  = FALSE;
    Point aPoint( rPoint );

    // check, if we have to adjust the point
    if ( !Frm().IsInside( aPoint ) )
    {
        aPoint.X() = Max( aPoint.X(), Frm().Left() );
        aPoint.X() = Min( aPoint.X(), Frm().Right() );
        aPoint.Y() = Max( aPoint.Y(), Frm().Top() );
        aPoint.Y() = Min( aPoint.Y(), Frm().Bottom() );
    }

    //Koennte ein Freifliegender gemeint sein?
    //Wenn sein Inhalt geschuetzt werden soll, so ist nix mit Crsr
    //hineinsetzen, dadurch sollten alle Aenderungen unmoeglich sein.
    if ( GetSortedObjs() )
    {
        SwOrderIter aIter( this );
        aIter.Top();
        while ( aIter() )
        {
            const SwVirtFlyDrawObj* pObj =
                                static_cast<const SwVirtFlyDrawObj*>(aIter());
            const SwFlyFrm* pFly = pObj ? pObj->GetFlyFrm() : 0;
            if ( pFly &&
                 ( ( pCMS ? pCMS->bSetInReadOnly : FALSE ) ||
                   !pFly->IsProtected() ) &&
                 pFly->GetCrsrOfst( pPos, aPoint, pCMS ) )
            {
                bRet = TRUE;
                break;
            }

            if ( pCMS && pCMS->bStop )
                return FALSE;
            aIter.Prev();
        }
    }

    if ( !bRet )
    {
        //Wenn kein Cntnt unterhalb der Seite 'antwortet', so korrigieren
        //wir den StartPoint und fangen nochmal eine Seite vor der
        //aktuellen an. Mit Flys ist es dann allerdings vorbei.
        if ( SwLayoutFrm::GetCrsrOfst( pPos, aPoint, pCMS ) )
            bRet = TRUE;
        else
        {
            if ( pCMS && (pCMS->bStop || pCMS->bExactOnly) )
            {
                ((SwCrsrMoveState*)pCMS)->bStop = TRUE;
                return FALSE;
            }
            const SwCntntFrm *pCnt = GetCntntPos( aPoint, FALSE, FALSE, FALSE, pCMS, FALSE );
            if ( pCMS && pCMS->bStop )
                return FALSE;

            ASSERT( pCnt, "Crsr is gone to a Black hole" );
            if( pCMS && pCMS->pFill && pCnt->IsTxtFrm() )
                bRet = pCnt->GetCrsrOfst( pPos, rPoint, pCMS );
            else
                bRet = pCnt->GetCrsrOfst( pPos, aPoint, pCMS );

            if ( !bRet )
            {
                // Set point to pCnt, delete mark
                // this may happen, if pCnt is hidden
                *pPos = SwPosition( *pCnt->GetNode(), SwIndex( (SwTxtNode*)pCnt->GetNode(), 0 ) );
                bRet = TRUE;
            }
        }
    }

    if ( bRet )
        rPoint = aPoint;

    return bRet;
}

bool SwLayoutFrm::FillSelection( SwSelectionList& rList, const SwRect& rRect ) const
{
    bool bRet = false;
    if( rRect.IsOver(PaintArea()) )
    {
        const SwFrm* pFrm = Lower();
        while( pFrm )
        {
            pFrm->FillSelection( rList, rRect );
            pFrm = pFrm->GetNext();
        }
    }
    return bRet;
}

bool SwPageFrm::FillSelection( SwSelectionList& rList, const SwRect& rRect ) const
{
    bool bRet = false;
    if( rRect.IsOver(PaintArea()) )
    {
        bRet = SwLayoutFrm::FillSelection( rList, rRect );
        if( GetSortedObjs() )
        {
            const SwSortedObjs &rObjs = *GetSortedObjs();
            for ( USHORT i = 0; i < rObjs.Count(); ++i )
            {
                const SwAnchoredObject* pAnchoredObj = rObjs[i];
                if( !pAnchoredObj->ISA(SwFlyFrm) )
                    continue;
                const SwFlyFrm* pFly = static_cast<const SwFlyFrm*>(pAnchoredObj);
                if( pFly->FillSelection( rList, rRect ) )
                    bRet = true;
            }
        }
    }
    return bRet;
}

bool SwRootFrm::FillSelection( SwSelectionList& aSelList, const SwRect& rRect) const
{
    const SwFrm *pPage = Lower();
    const long nBottom = rRect.Bottom();
    while( pPage )
    {
        if( pPage->Frm().Top() < nBottom )
        {
            if( pPage->Frm().Bottom() > rRect.Top() )
                pPage->FillSelection( aSelList, rRect );
            pPage = pPage->GetNext();
        }
        else
            pPage = 0;
    }
    return !aSelList.isEmpty();
}

/*************************************************************************
|*
|*	SwRootFrm::GetCrsrOfst()
|*
|*	Beschreibung:		Reicht Primaer den Aufruf an die erste Seite weiter.
|*						Wenn der 'reingereichte Point veraendert wird,
|*						so wird FALSE zurueckgegeben.
|*	Ersterstellung		MA 01. Jun. 92
|*	Letzte Aenderung	MA 30. Nov. 94
|*
|*************************************************************************/
BOOL SwRootFrm::GetCrsrOfst( SwPosition *pPos, Point &rPoint,
                             SwCrsrMoveState* pCMS ) const
{
    sal_Bool bOldAction = IsCallbackActionEnabled();
    ((SwRootFrm*)this)->SetCallbackActionEnabled( FALSE );
    ASSERT( (Lower() && Lower()->IsPageFrm()), "Keinen PageFrm gefunden." );
    if( pCMS && pCMS->pFill )
        ((SwCrsrMoveState*)pCMS)->bFillRet = FALSE;
    Point aOldPoint = rPoint;

    // PAGES01
    // search for page containing rPoint. The borders around the pages are considerd
    const SwPageFrm* pPage = GetPageAtPos( rPoint, 0, true );

    // --> OD 2008-12-23 #i95626#
    // special handling for <rPoint> beyond root frames area
    if ( !pPage &&
         rPoint.X() > Frm().Right() &&
         rPoint.Y() > Frm().Bottom() )
    {
        pPage = dynamic_cast<const SwPageFrm*>(Lower());
        while ( pPage && pPage->GetNext() )
        {
            pPage = dynamic_cast<const SwPageFrm*>(pPage->GetNext());
        }
    }
    // <--
    if ( pPage )
    {
        pPage->SwPageFrm::GetCrsrOfst( pPos, rPoint, pCMS );
    }

    ((SwRootFrm*)this)->SetCallbackActionEnabled( bOldAction );
    if( pCMS )
    {
        if( pCMS->bStop )
            return FALSE;
        if( pCMS->pFill )
            return pCMS->bFillRet;
    }
    return aOldPoint == rPoint;
}

/*************************************************************************
|*
|*	SwCellFrm::GetCrsrOfst()
|*
|*	Beschreibung		Wenn es sich um eine Cntnt-tragende Cell handelt wird
|* 						der Crsr notfalls mit Gewalt in einen der CntntFrms
|* 						gesetzt.
|* 						In geschuetzte Zellen gibt es hier keinen Eingang.
|*	Ersterstellung		MA 04. Jun. 93
|*	Letzte Aenderung	MA 23. May. 95
|*
|*************************************************************************/
BOOL SwCellFrm::GetCrsrOfst( SwPosition *pPos, Point &rPoint,
                             SwCrsrMoveState* pCMS ) const
{
    // cell frame does not necessarily have a lower (split table cell)
    if ( !Lower() )
        return FALSE;

    if ( !(pCMS?pCMS->bSetInReadOnly:FALSE) &&
         GetFmt()->GetProtect().IsCntntProtected() )
        return FALSE;

    if ( pCMS && pCMS->eState == MV_TBLSEL )
    {
        const SwTabFrm *pTab = FindTabFrm();
        if ( pTab->IsFollow() && pTab->IsInHeadline( *this ) )
        {
            ((SwCrsrMoveState*)pCMS)->bStop = TRUE;
            return FALSE;
        }
    }

    if ( Lower() )
    {
        if ( Lower()->IsLayoutFrm() )
            return SwLayoutFrm::GetCrsrOfst( pPos, rPoint, pCMS );
        else
        {
            Calc();
            BOOL bRet = FALSE;

            const SwFrm *pFrm = Lower();
            while ( pFrm && !bRet )
            {
                pFrm->Calc();
                if ( pFrm->Frm().IsInside( rPoint ) )
                {
                    bRet = pFrm->GetCrsrOfst( pPos, rPoint, pCMS );
                    if ( pCMS && pCMS->bStop )
                        return FALSE;
                }
                pFrm = pFrm->GetNext();
            }
            if ( !bRet )
            {
                Point *pPoint = pCMS && pCMS->pFill ? new Point( rPoint ) : NULL;
                const SwCntntFrm *pCnt = GetCntntPos( rPoint, TRUE );
                if( pPoint && pCnt->IsTxtFrm() )
                {
                    pCnt->GetCrsrOfst( pPos, *pPoint, pCMS );
                    rPoint = *pPoint;
                }
                else
                    pCnt->GetCrsrOfst( pPos, rPoint, pCMS );
                delete pPoint;
            }
            return TRUE;
        }
    }

    return FALSE;
}

/*************************************************************************
|*
|*	SwFlyFrm::GetCrsrOfst()
|*
|*	Ersterstellung		MA 15. Dec. 92
|*	Letzte Aenderung	MA 23. May. 95
|*
|*************************************************************************/
//Problem: Wenn zwei Flys genau gleich gross sind und auf derselben
//Position stehen, so liegt jeder innerhalb des anderen.
//Da jeweils geprueft wird, ob der Point nicht zufaellig innerhalb eines
//anderen Flys liegt, der sich vollstaendig innerhalb des aktuellen befindet
//und ggf. ein rekursiver Aufruf erfolgt wuerde o.g. Situation zu einer
//endlosen Rekursion fuehren.
//Mit der Hilfsklasse SwCrsrOszControl unterbinden wir die Rekursion. Das
//GetCrsrOfst entscheidet sich bei einer Rekursion fuer denjenigen der
//am weitesten oben liegt.

BOOL SwFlyFrm::GetCrsrOfst( SwPosition *pPos, Point &rPoint,
                            SwCrsrMoveState* pCMS ) const
{
    aOszCtrl.Entry( this );

    //Wenn der Point innerhalb des Fly sitzt wollen wir energisch
    //versuchen den Crsr hineinzusetzen.
    //Wenn der Point allerdings in einem Flys sitzt, der sich vollstaendig
    //innerhalb des aktuellen befindet, so wird fuer diesen das
    //GetCrsrOfst gerufen.
    Calc();
    BOOL bInside = Frm().IsInside( rPoint ) && Lower(),
         bRet = FALSE;

    //Wenn der Frm eine Grafik enthaelt, aber nur Text gewuenscht ist, so
    //nimmt er den Crsr grundsaetzlich nicht an.
    if ( bInside && pCMS && pCMS->eState == MV_SETONLYTEXT &&
         (!Lower() || Lower()->IsNoTxtFrm()) )
        bInside = FALSE;

    const SwPageFrm *pPage = FindPageFrm();
    if ( bInside && pPage && pPage->GetSortedObjs() )
    {
        SwOrderIter aIter( pPage );
        aIter.Top();
        while ( aIter() && !bRet )
        {
            const SwVirtFlyDrawObj* pObj = static_cast<const SwVirtFlyDrawObj*>(aIter());
            const SwFlyFrm* pFly = pObj ? pObj->GetFlyFrm() : 0;
            if ( pFly && pFly->Frm().IsInside( rPoint ) &&
                 Frm().IsInside( pFly->Frm() ) )
            {
                if ( aOszCtrl.ChkOsz( pFly ) ||
                     TRUE == (bRet = pFly->GetCrsrOfst( pPos, rPoint, pCMS )))
                    break;
                if ( pCMS && pCMS->bStop )
                    return FALSE;
            }
            aIter.Next();
        }
    }

    while ( bInside && !bRet )
    {
        const SwFrm *pFrm = Lower();
        while ( pFrm && !bRet )
        {
            pFrm->Calc();
            if ( pFrm->Frm().IsInside( rPoint ) )
            {
                bRet = pFrm->GetCrsrOfst( pPos, rPoint, pCMS );
                if ( pCMS && pCMS->bStop )
                    return FALSE;
            }
            pFrm = pFrm->GetNext();
        }
        if ( !bRet )
        {
            Point *pPoint = pCMS && pCMS->pFill ? new Point( rPoint ) : NULL;
            const SwCntntFrm *pCnt = GetCntntPos(
                                            rPoint, TRUE, FALSE, FALSE, pCMS );
            if ( pCMS && pCMS->bStop )
                return FALSE;
            if( pPoint && pCnt->IsTxtFrm() )
            {
                pCnt->GetCrsrOfst( pPos, *pPoint, pCMS );
                rPoint = *pPoint;
            }
            else
                pCnt->GetCrsrOfst( pPos, rPoint, pCMS );
            delete pPoint;
            bRet = TRUE;
        }
    }
    aOszCtrl.Exit( this );
    return bRet;
}

/*************************************************************************
|*
|*	  Beschreibung		Layoutabhaengiges Cursortravelling
|*	  Ersterstellung	MA 23. Jul. 92
|*	  Letzte Aenderung	MA 06. Sep. 93
|*
|*************************************************************************/
BOOL SwCntntFrm::LeftMargin(SwPaM *pPam) const
{
    if( pPam->GetNode() != (SwCntntNode*)GetNode() )
        return FALSE;
    ((SwCntntNode*)GetNode())->
        MakeStartIndex((SwIndex *) &pPam->GetPoint()->nContent);
    return TRUE;
}

BOOL SwCntntFrm::RightMargin(SwPaM *pPam, BOOL) const
{
    if( pPam->GetNode() != (SwCntntNode*)GetNode() )
        return FALSE;
    ((SwCntntNode*)GetNode())->
        MakeEndIndex((SwIndex *) &pPam->GetPoint()->nContent);
    return TRUE;
}

const SwCntntFrm *lcl_GetNxtCnt( const SwCntntFrm* pCnt )
{
    return pCnt->GetNextCntntFrm();
}

const SwCntntFrm *lcl_GetPrvCnt( const SwCntntFrm* pCnt )
{
    return pCnt->GetPrevCntntFrm();
}

typedef const SwCntntFrm *(*GetNxtPrvCnt)( const SwCntntFrm* );

//Frame in wiederholter Headline?
BOOL lcl_IsInRepeatedHeadline( const SwFrm *pFrm,
                                    const SwTabFrm** ppTFrm = 0 )
{
    const SwTabFrm *pTab = pFrm->FindTabFrm();
    if( ppTFrm )
        *ppTFrm = pTab;
    return pTab && pTab->IsFollow() && pTab->IsInHeadline( *pFrm );
}


//Ueberspringen geschuetzter Tabellenzellen. Optional auch
//Ueberspringen von wiederholten Headlines.
//MA 26. Jan. 98: Chg auch andere Geschuetzte Bereiche ueberspringen.
// FME: Skip follow flow cells
const SwCntntFrm * MA_FASTCALL lcl_MissProtectedFrames( const SwCntntFrm *pCnt,
                                                       GetNxtPrvCnt fnNxtPrv,
                                                       BOOL bMissHeadline,
                                                       BOOL bInReadOnly,
                                                       BOOL bMissFollowFlowLine )
{
    if ( pCnt && pCnt->IsInTab() )
    {
        BOOL bProtect = TRUE;
        while ( pCnt && bProtect )
        {
            const SwLayoutFrm *pCell = pCnt->GetUpper();
            while ( pCell && !pCell->IsCellFrm() )
                pCell = pCell->GetUpper();
            if ( !pCell ||
                    ( ( bInReadOnly || !pCell->GetFmt()->GetProtect().IsCntntProtected() ) &&
                      ( !bMissHeadline || !lcl_IsInRepeatedHeadline( pCell ) ) &&
                      ( !bMissFollowFlowLine || !pCell->IsInFollowFlowRow() ) ) &&
                        !pCell->IsCoveredCell() )
                bProtect = FALSE;
            else
                pCnt = (*fnNxtPrv)( pCnt );
        }
    }
    else if ( !bInReadOnly )
        while ( pCnt && pCnt->IsProtected() )
            pCnt = (*fnNxtPrv)( pCnt );

    return pCnt;
}

BOOL MA_FASTCALL lcl_UpDown( SwPaM *pPam, const SwCntntFrm *pStart,
                    GetNxtPrvCnt fnNxtPrv, BOOL bInReadOnly )
{
    ASSERT( pPam->GetNode() == (SwCntntNode*)pStart->GetNode(),
            "lcl_UpDown arbeitet nicht fuer andere." );

    const SwCntntFrm *pCnt = 0;

    //Wenn gerade eine Tabellenselection laeuft muss ein bischen getricktst
    //werden: Beim hochlaufen an den Anfang der Zelle gehen, beim runterlaufen
    //an das Ende der Zelle gehen.
    BOOL bTblSel = false;
    if ( pStart->IsInTab() &&
        pPam->GetNode( TRUE )->StartOfSectionNode() !=
        pPam->GetNode( FALSE )->StartOfSectionNode() )
    {
        bTblSel = true;
        const SwLayoutFrm  *pCell = pStart->GetUpper();
        while ( !pCell->IsCellFrm() )
            pCell = pCell->GetUpper();

        //
        // Check, if cell has a Prev/Follow cell:
        //
        const bool bFwd = ( fnNxtPrv == lcl_GetNxtCnt );
        const SwLayoutFrm* pTmpCell = bFwd ?
            ((SwCellFrm*)pCell)->GetFollowCell() :
            ((SwCellFrm*)pCell)->GetPreviousCell();

        const SwCntntFrm* pTmpStart = pStart;
        while ( pTmpCell && 0 != ( pTmpStart = pTmpCell->ContainsCntnt() ) )
        {
            pCell = pTmpCell;
            pTmpCell = bFwd ?
                ((SwCellFrm*)pCell)->GetFollowCell() :
                ((SwCellFrm*)pCell)->GetPreviousCell();
        }
        const SwCntntFrm *pNxt = pCnt = pTmpStart;

        while ( pCell->IsAnLower( pNxt ) )
        {
            pCnt = pNxt;
            pNxt = (*fnNxtPrv)( pNxt );
        }
    }

    pCnt = (*fnNxtPrv)( pCnt ? pCnt : pStart );
    pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );


    const SwTabFrm *pStTab = pStart->FindTabFrm();
    const SwTabFrm *pTable = 0;
    const BOOL bTab = pStTab || (pCnt && pCnt->IsInTab()) ? TRUE : FALSE;
    BOOL bEnd = bTab ? FALSE : TRUE;

    const SwFrm* pVertRefFrm = pStart;
    if ( bTblSel && pStTab )
        pVertRefFrm = pStTab;
    SWRECTFN( pVertRefFrm )

    SwTwips nX = 0;
    if ( bTab )
    {
        //
        // pStart or pCnt is inside a table. nX will be used for travelling:
        //
        SwRect aRect( pStart->Frm() );
        pStart->GetCharRect( aRect, *pPam->GetPoint() );
        Point aCenter = aRect.Center();
        nX = bVert ? aCenter.Y() : aCenter.X();

        pTable = pCnt ? pCnt->FindTabFrm() : 0;
        if ( !pTable )
            pTable = pStTab;

        if ( pStTab &&
            !pStTab->GetUpper()->IsInTab() &&
            !pTable->GetUpper()->IsInTab() )
        {
            const SwFrm *pCell = pStart->GetUpper();
            while ( pCell && !pCell->IsCellFrm() )
                pCell = pCell->GetUpper();
            ASSERT( pCell, "Zelle nicht gefunden." );
            nX =  (pCell->Frm().*fnRect->fnGetLeft)() +
                  (pCell->Frm().*fnRect->fnGetWidth)() / 2;

            //Der Fluss fuehrt von einer Tabelle in die nachste. Der X-Wert
            //muss ausgehend von der Mitte der Startzelle um die Verschiebung
            //der Tabellen korrigiert werden.
            if ( pStTab != pTable )
            {
                nX += (pTable->Frm().*fnRect->fnGetLeft)() -
                      (pStTab->Frm().*fnRect->fnGetLeft)();
            }
        }

        //
        // Restrict nX to the left and right borders of pTab:
        // (is this really necessary?)
        //
        if ( !pTable->GetUpper()->IsInTab() )
        {
            const sal_Bool bRTL = pTable->IsRightToLeft();
            const long nPrtLeft = bRTL ?
                                (pTable->*fnRect->fnGetPrtRight)() :
                                (pTable->*fnRect->fnGetPrtLeft)();
            if ( bRTL != nX < nPrtLeft )
                nX = nPrtLeft;
            else
            {
                   const long nPrtRight = bRTL ?
                                    (pTable->*fnRect->fnGetPrtLeft)() :
                                    (pTable->*fnRect->fnGetPrtRight)();
                if ( bRTL != nX > nPrtRight )
                    nX = nPrtRight;
            }
        }
    }

    do
    {
        //Wenn ich im DokumentBody bin, so will ich da auch bleiben
        if ( pStart->IsInDocBody() )
        {
            while ( pCnt && (!pCnt->IsInDocBody() ||
                             (pCnt->IsTxtFrm() && ((SwTxtFrm*)pCnt)->IsHiddenNow())))
            {
                pCnt = (*fnNxtPrv)( pCnt );
                pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );
            }
        }

        //Wenn ich im Fussnotenbereich bin, so versuche ich notfalls den naechsten
        //Fussnotenbereich zu erreichen.
        else if ( pStart->IsInFtn() )
        {
            while ( pCnt && (!pCnt->IsInFtn() ||
                            (pCnt->IsTxtFrm() && ((SwTxtFrm*)pCnt)->IsHiddenNow())))
            {
                pCnt = (*fnNxtPrv)( pCnt );
                pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );
            }
        }

        //In Flys kann es Blind weitergehen solange ein Cntnt
        //gefunden wird.
        else if ( pStart->IsInFly() )
        {
            if ( pCnt && pCnt->IsTxtFrm() && ((SwTxtFrm*)pCnt)->IsHiddenNow() )
            {
                pCnt = (*fnNxtPrv)( pCnt );
                pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );
            }
        }

        //Andernfalls weigere ich mich einfach den derzeitigen Bereich zu
        //verlassen.
        else if ( pCnt )
        {
            const SwFrm *pUp = pStart->GetUpper();				 //Head/Foot
            while ( pUp && pUp->GetUpper() && !(pUp->GetType() & 0x0018 ) )
                pUp = pUp->GetUpper();
            BOOL bSame = FALSE;
            const SwFrm *pCntUp = pCnt->GetUpper();
            while ( pCntUp && !bSame )
            {	if ( pUp == pCntUp )
                    bSame = TRUE;
                else
                    pCntUp = pCntUp->GetUpper();
            }
            if ( !bSame )
                pCnt = 0;
            else if ( pCnt && pCnt->IsTxtFrm() && ((SwTxtFrm*)pCnt)->IsHiddenNow() ) // i73332
            {
                pCnt = (*fnNxtPrv)( pCnt );
                pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );
            }
        }

        if ( bTab )
        {
            if ( !pCnt )
                bEnd = TRUE;
            else
            {	const SwTabFrm *pTab = pCnt->FindTabFrm();
                if( !pTab )
                    bEnd = TRUE;
                else
                {
                    if ( pTab != pTable )
                    {
                        //Der Fluss fuehrt von einer Tabelle in die nachste. Der
                        //X-Wert muss um die Verschiebung der Tabellen korrigiert
                        //werden.
                         if ( pTable &&
                              !pTab->GetUpper()->IsInTab() &&
                            !pTable->GetUpper()->IsInTab() )
                            nX += pTab->Frm().Left() - pTable->Frm().Left();
                        pTable = pTab;
                    }
                    const SwLayoutFrm *pCell = pTable ? pCnt->GetUpper() : 0;
                    while ( pCell && !pCell->IsCellFrm() )
                        pCell = pCell->GetUpper();

                    Point aInsideCell;
                    Point aInsideCnt;
                    if ( pCell )
                    {
                        long nTmpTop = (pCell->Frm().*fnRect->fnGetTop)();
                        if ( bVert )
                        {
                            if ( nTmpTop )
                                --nTmpTop;

                            aInsideCell = Point( nTmpTop, nX );
                        }
                        else
                            aInsideCell = Point( nX, nTmpTop );
                    }

                    long nTmpTop = (pCnt->Frm().*fnRect->fnGetTop)();
                    if ( bVert )
                    {
                        if ( nTmpTop )
                            --nTmpTop;

                        aInsideCnt = Point( nTmpTop, nX );
                    }
                    else
                        aInsideCnt = Point( nX, nTmpTop );

                    if ( pCell && pCell->Frm().IsInside( aInsideCell ) )
                    {
                        bEnd = TRUE;
                        //Jetzt noch schnell den richtigen Cntnt in der Zelle
                        //greifen.
                        if ( !pCnt->Frm().IsInside( aInsideCnt ) )
                        {
                            pCnt = pCell->ContainsCntnt();
                            if ( fnNxtPrv == lcl_GetPrvCnt )
                                while ( pCell->IsAnLower(pCnt->GetNextCntntFrm()) )
                                    pCnt = pCnt->GetNextCntntFrm();
                        }
                    }
                    else if ( pCnt->Frm().IsInside( aInsideCnt ) )
                        bEnd = TRUE;
                }
            }
            if ( !bEnd )
            {
                pCnt = (*fnNxtPrv)( pCnt );
                pCnt = ::lcl_MissProtectedFrames( pCnt, fnNxtPrv, TRUE, bInReadOnly, bTblSel );
            }
        }

    } while ( !bEnd ||
              (pCnt && pCnt->IsTxtFrm() && ((SwTxtFrm*)pCnt)->IsHiddenNow()));

    if( pCnt )
    {	// setze den Point auf den Content-Node
        SwCntntNode *pCNd = (SwCntntNode*)pCnt->GetNode();
        pPam->GetPoint()->nNode = *pCNd;
        if ( fnNxtPrv == lcl_GetPrvCnt )
            pCNd->MakeEndIndex( (SwIndex*)&pPam->GetPoint()->nContent );
        else
            pCNd->MakeStartIndex( (SwIndex*)&pPam->GetPoint()->nContent );
        return TRUE;
    }
    return FALSE;
}

BOOL SwCntntFrm::UnitUp( SwPaM* pPam, const SwTwips, BOOL bInReadOnly ) const
{
    return ::lcl_UpDown( pPam, this, lcl_GetPrvCnt, bInReadOnly );
}

BOOL SwCntntFrm::UnitDown( SwPaM* pPam, const SwTwips, BOOL bInReadOnly ) const
{
    return ::lcl_UpDown( pPam, this, lcl_GetNxtCnt, bInReadOnly );
}

/*************************************************************************
|*
|*	SwRootFrm::GetCurrPage()
|*
|*	Beschreibung:		Liefert die Nummer der aktuellen Seite.
|*			Wenn die Methode einen PaM bekommt, so ist die aktuelle Seite
|*			diejenige in der der PaM sitzt. Anderfalls ist die aktuelle
|*			Seite die erste Seite innerhalb der VisibleArea.
|*			Es wird nur auf den vorhandenen Seiten gearbeitet!
|*	Ersterstellung		MA 20. May. 92
|*	Letzte Aenderung	MA 09. Oct. 97
|*
|*************************************************************************/
USHORT SwRootFrm::GetCurrPage( const SwPaM *pActualCrsr ) const
{
    ASSERT( pActualCrsr, "Welche Seite soll's denn sein?" );
    const SwFrm *pActFrm = GetFmt()->GetDoc()->GetNodes()[pActualCrsr->GetPoint()->nNode]->
                                    GetCntntNode()->GetFrm( 0,
                                                    pActualCrsr->GetPoint(),
                                                    FALSE );
    return pActFrm->FindPageFrm()->GetPhyPageNum();
}

/*************************************************************************
|*
|*	SwRootFrm::SetCurrPage()
|*
|*	Beschreibung:		Liefert einen PaM der am Anfang der gewuenschten
|*			Seite sitzt.
|*			Formatiert wird soweit notwendig
|*			Liefert Null, wenn die Operation nicht moeglich ist.
|*			Der PaM sitzt in der letzten Seite, wenn die Seitenzahl zu gross
|*			gewaehlt wurde.
|*	Ersterstellung		MA 20. May. 92
|*	Letzte Aenderung	MA 09. Oct. 97
|*
|*************************************************************************/
USHORT SwRootFrm::SetCurrPage( SwCursor* pToSet, USHORT nPageNum )
{
    ASSERT( Lower() && Lower()->IsPageFrm(), "Keine Seite vorhanden." );

    SwPageFrm *pPage = (SwPageFrm*)Lower();
    BOOL bEnd =FALSE;
    while ( !bEnd && pPage->GetPhyPageNum() != nPageNum )
    {	if ( pPage->GetNext() )
            pPage = (SwPageFrm*)pPage->GetNext();
        else
        {	//Ersten CntntFrm Suchen, und solange Formatieren bis
            //eine neue Seite angefangen wird oder bis die CntntFrm's alle
            //sind.
            const SwCntntFrm *pCntnt = pPage->ContainsCntnt();
            while ( pCntnt && pPage->IsAnLower( pCntnt ) )
            {
                pCntnt->Calc();
                pCntnt = pCntnt->GetNextCntntFrm();
            }
            //Jetzt ist entweder eine neue Seite da, oder die letzte Seite
            //ist gefunden.
            if ( pPage->GetNext() )
                pPage = (SwPageFrm*)pPage->GetNext();
            else
                bEnd = TRUE;
        }
    }
    //pPage zeigt jetzt auf die 'gewuenschte' Seite. Jetzt muss noch der
    //PaM auf den Anfang des ersten CntntFrm im Body-Text erzeugt werden.
    //Wenn es sich um eine Fussnotenseite handelt, wird der PaM in die erste
    //Fussnote gesetzt.
    const SwCntntFrm *pCntnt = pPage->ContainsCntnt();
    if ( pPage->IsFtnPage() )
        while ( pCntnt && !pCntnt->IsInFtn() )
            pCntnt = pCntnt->GetNextCntntFrm();
    else
        while ( pCntnt && !pCntnt->IsInDocBody() )
            pCntnt = pCntnt->GetNextCntntFrm();
    if ( pCntnt )
    {
        SwCntntNode* pCNd = (SwCntntNode*)pCntnt->GetNode();
        pToSet->GetPoint()->nNode = *pCNd;
        pCNd->MakeStartIndex( (SwIndex*)&pToSet->GetPoint()->nContent );
        pToSet->GetPoint()->nContent = ((SwTxtFrm*)pCntnt)->GetOfst();

        SwShellCrsr* pSCrsr = dynamic_cast<SwShellCrsr*>(pToSet);
        if( pSCrsr )
        {
            Point &rPt = pSCrsr->GetPtPos();
            rPt = pCntnt->Frm().Pos();
            rPt += pCntnt->Prt().Pos();
        }
        return pPage->GetPhyPageNum();
    }
    return 0;
}

/*************************************************************************
|*
|*	  SwCntntFrm::StartxxPage(), EndxxPage()
|*
|*	  Beschreibung		Cursor an Anfang/Ende der aktuellen/vorherigen/
|*		naechsten Seite. Alle sechs Methoden rufen GetFrmInPage() mit der
|*		entsprechenden Parametrisierung.
|*		Zwei Parameter steuern die Richtung: einer bestimmt die Seite, der
|*		andere Anfang/Ende.
|*		Fuer die Bestimmung der Seite und des Cntnt (Anfang/Ende) werden
|*		die im folgenden definierten Funktionen benutzt.
|*	  Ersterstellung	MA 15. Oct. 92
|*	  Letzte Aenderung	MA 28. Feb. 93
|*
|*************************************************************************/
SwCntntFrm *GetFirstSub( const SwLayoutFrm *pLayout )
{
    return ((SwPageFrm*)pLayout)->FindFirstBodyCntnt();
}

SwCntntFrm *GetLastSub( const SwLayoutFrm *pLayout )
{
    return ((SwPageFrm*)pLayout)->FindLastBodyCntnt();
}

SwLayoutFrm *GetNextFrm( const SwLayoutFrm *pFrm )
{
    SwLayoutFrm *pNext =
        (pFrm->GetNext() && pFrm->GetNext()->IsLayoutFrm()) ?
                                            (SwLayoutFrm*)pFrm->GetNext() : 0;
    // #i39402# in case of an empty page
    if(pNext && !pNext->ContainsCntnt())
        pNext = (pNext->GetNext() && pNext->GetNext()->IsLayoutFrm()) ?
                                            (SwLayoutFrm*)pNext->GetNext() : 0;
    return pNext;
}

SwLayoutFrm *GetThisFrm( const SwLayoutFrm *pFrm )
{
    return (SwLayoutFrm*)pFrm;
}

SwLayoutFrm *GetPrevFrm( const SwLayoutFrm *pFrm )
{
    SwLayoutFrm *pPrev =
        (pFrm->GetPrev() && pFrm->GetPrev()->IsLayoutFrm()) ?
                                            (SwLayoutFrm*)pFrm->GetPrev() : 0;
    // #i39402# in case of an empty page
    if(pPrev && !pPrev->ContainsCntnt())
        pPrev = (pPrev->GetPrev() && pPrev->GetPrev()->IsLayoutFrm()) ?
                                            (SwLayoutFrm*)pPrev->GetPrev() : 0;
    return pPrev;
}

//Jetzt koennen auch die Funktionspointer initalisiert werden;
//sie sind in cshtyp.hxx declariert.
SwPosPage fnPageStart = GetFirstSub;
SwPosPage fnPageEnd = GetLastSub;
SwWhichPage fnPagePrev = GetPrevFrm;
SwWhichPage fnPageCurr = GetThisFrm;
SwWhichPage fnPageNext = GetNextFrm;

//Liefert den ersten/den letzten Contentframe (gesteuert ueber
//den Parameter fnPosPage) in der
//aktuellen/vorhergehenden/folgenden Seite (gesteuert durch den
//Parameter fnWhichPage).
BOOL GetFrmInPage( const SwCntntFrm *pCnt, SwWhichPage fnWhichPage,
                   SwPosPage fnPosPage, SwPaM *pPam )
{
    //Erstmal die gewuenschte Seite besorgen, anfangs die aktuelle, dann
    //die die per fnWichPage gewuenscht wurde
    const SwLayoutFrm *pLayoutFrm = pCnt->FindPageFrm();
    if ( !pLayoutFrm || (0 == (pLayoutFrm = (*fnWhichPage)(pLayoutFrm))) )
        return FALSE;

    //Jetzt den gewuenschen CntntFrm unterhalb der Seite
    if( 0 == (pCnt = (*fnPosPage)(pLayoutFrm)) )
        return FALSE;
    else
    {
        // repeated headlines in tables
        if ( pCnt->IsInTab() && fnPosPage == GetFirstSub )
        {
            const SwTabFrm* pTab = pCnt->FindTabFrm();
            if ( pTab->IsFollow() )
            {
                if ( pTab->IsInHeadline( *pCnt ) )
                {
                    SwLayoutFrm* pRow = pTab->GetFirstNonHeadlineRow();
                    if ( pRow )
                    {
                        // We are in the first line of a follow table
                        // with repeated headings.
                        // To actually make a "real" move we take the first content
                        // of the next row
                        pCnt = pRow->ContainsCntnt();
                        if ( ! pCnt )
                            return FALSE;
                    }
                }
            }
        }

        SwCntntNode *pCNd = (SwCntntNode*)pCnt->GetNode();
        pPam->GetPoint()->nNode = *pCNd;
        xub_StrLen nIdx;
        if( fnPosPage == GetFirstSub )
            nIdx = ((SwTxtFrm*)pCnt)->GetOfst();
        else
            nIdx = pCnt->GetFollow() ?
                    ((SwTxtFrm*)pCnt)->GetFollow()->GetOfst()-1 : pCNd->Len();
        pPam->GetPoint()->nContent.Assign( pCNd, nIdx );
        return TRUE;
    }
}

/*************************************************************************
|*
|*	SwLayoutFrm::GetCntntPos()
|*
|*	Beschreibung		Es wird der nachstliegende Cntnt zum uebergebenen
|* 						gesucht. Betrachtet werden die vorhergehende, die
|* 						aktuelle und die folgende Seite.
|* 						Wenn kein Inhalt gefunden wird, so wird der Bereich
 * 						erweitert bis einer gefunden wird.
|* 						Zurueckgegeben wird die 'Semantisch richtige' Position
|* 						innerhalb der PrtArea des gefundenen CntntFrm
|*	Ersterstellung		MA 15. Jul. 92
|*	Letzte Aenderung	MA 09. Jan. 97
|*
|*************************************************************************/
ULONG CalcDiff( const Point &rPt1, const Point &rPt2 )
{
    //Jetzt die Entfernung zwischen den beiden Punkten berechnen.
    //'Delta' X^2 + 'Delta'Y^2 = 'Entfernung'^2
    sal_uInt32 dX = Max( rPt1.X(), rPt2.X() ) -
               Min( rPt1.X(), rPt2.X() ),
          dY = Max( rPt1.Y(), rPt2.Y() ) -
               Min( rPt1.Y(), rPt2.Y() );
    BigInt dX1( dX ), dY1( dY );
    dX1 *= dX1; dY1 *= dY1;
    return ::SqRt( dX1 + dY1 );
}

// lcl_Inside ueberprueft, ob der Punkt innerhalb des Seitenteils liegt, in dem
// auch der CntntFrame liegt. Als Seitenteile gelten in diesem Zusammenhang
// Kopfzeile, Seitenbody, Fusszeile und FussnotenContainer.
// Dies dient dazu, dass ein CntntFrm, der im "richtigen" Seitenteil liegt,
// eher akzeptiert wird als ein anderer, der nicht dort liegt, auch wenn
// dessen Abstand zum Punkt geringer ist.

const SwLayoutFrm* lcl_Inside( const SwCntntFrm *pCnt, Point& rPt )
{
    const SwLayoutFrm* pUp = pCnt->GetUpper();
    while( pUp )
    {
        if( pUp->IsPageBodyFrm() || pUp->IsFooterFrm() || pUp->IsHeaderFrm() )
        {
            if( rPt.Y() >= pUp->Frm().Top() && rPt.Y() <= pUp->Frm().Bottom() )
                return pUp;
            return NULL;
        }
        if( pUp->IsFtnContFrm() )
            return pUp->Frm().IsInside( rPt ) ? pUp : NULL;
        pUp = pUp->GetUpper();
    }
    return NULL;
}

const SwCntntFrm *SwLayoutFrm::GetCntntPos( Point& rPoint,
                                            const BOOL bDontLeave,
                                            const BOOL bBodyOnly,
                                            const BOOL bCalc,
                                            const SwCrsrMoveState *pCMS,
                                            const BOOL bDefaultExpand ) const
{
    //Ersten CntntFrm ermitteln.
    const SwLayoutFrm *pStart = (!bDontLeave && bDefaultExpand && GetPrev()) ?
                                    (SwLayoutFrm*)GetPrev() : this;
    const SwCntntFrm *pCntnt = pStart->ContainsCntnt();

    if ( !pCntnt && (GetPrev() && !bDontLeave) )
        pCntnt = ContainsCntnt();

    if ( bBodyOnly && pCntnt && !pCntnt->IsInDocBody() )
        while ( pCntnt && !pCntnt->IsInDocBody() )
            pCntnt = pCntnt->GetNextCntntFrm();

    const SwCntntFrm *pActual= pCntnt;
    const SwLayoutFrm *pInside = NULL;
    USHORT nMaxPage = GetPhyPageNum() + (bDefaultExpand ? 1 : 0);
    Point aPoint = rPoint;
    ULONG nDistance = ULONG_MAX;

    while ( TRUE ) 	//Sicherheitsschleifchen, damit immer einer gefunden wird.
    {
        while ( pCntnt &&
                ((!bDontLeave || IsAnLower( pCntnt )) &&
                (pCntnt->GetPhyPageNum() <= nMaxPage)) )
        {
            if ( ( bCalc || pCntnt->Frm().Width() ) &&
                 ( !bBodyOnly || pCntnt->IsInDocBody() ) )
            {
                //Wenn der Cntnt in einem geschuetzen Bereich (Zelle, Ftn, Section)
                //liegt, wird der nachste Cntnt der nicht geschuetzt ist gesucht.
                const SwCntntFrm *pComp = pCntnt;
                pCntnt = ::lcl_MissProtectedFrames( pCntnt, lcl_GetNxtCnt, FALSE,
                                        pCMS ? pCMS->bSetInReadOnly : FALSE, FALSE );
                if ( pComp != pCntnt )
                    continue;

                if ( !pCntnt->IsTxtFrm() || !((SwTxtFrm*)pCntnt)->IsHiddenNow() )
                {
                    if ( bCalc )
                        pCntnt->Calc();

                    SwRect aCntFrm( pCntnt->UnionFrm() );
                    if ( aCntFrm.IsInside( rPoint ) )
                    {
                        pActual = pCntnt;
                        aPoint = rPoint;
                        break;
                    }
                    //Die Strecke von rPoint zum dichtesten Punkt von pCntnt wird
                    //jetzt berechnet.
                    Point aCntntPoint( rPoint );

                    //Erst die Vertikale Position einstellen
                    if ( aCntFrm.Top() > aCntntPoint.Y() )
                        aCntntPoint.Y() = aCntFrm.Top();
                    else if ( aCntFrm.Bottom() < aCntntPoint.Y() )
                        aCntntPoint.Y() = aCntFrm.Bottom();

                    //Jetzt die Horizontale Position
                    if ( aCntFrm.Left() > aCntntPoint.X() )
                        aCntntPoint.X() = aCntFrm.Left();
                    else if ( aCntFrm.Right() < aCntntPoint.X() )
                        aCntntPoint.X() = aCntFrm.Right();

                    // pInside ist ein Seitenbereich, in dem der Punkt liegt,
                    // sobald pInside!=0 ist, werden nur noch Frames akzeptiert,
                    // die innerhalb liegen.
                    if( !pInside || ( pInside->IsAnLower( pCntnt ) &&
                        ( !pCntnt->IsInFtn() || pInside->IsFtnContFrm() ) ) )
                    {
                        const ULONG nDiff = ::CalcDiff( aCntntPoint, rPoint );
                        BOOL bBetter = nDiff < nDistance;  // Dichter dran
                        if( !pInside )
                        {
                            pInside = lcl_Inside( pCntnt, rPoint );
                            if( pInside )  // Im "richtigen" Seitenteil
                                bBetter = TRUE;
                        }
                        if( bBetter )
                        {
                            aPoint = aCntntPoint;
                            nDistance = nDiff;
                            pActual = pCntnt;
                        }
                    }
                }
            }
            pCntnt = pCntnt->GetNextCntntFrm();
            if ( bBodyOnly )
                while ( pCntnt && !pCntnt->IsInDocBody() )
                    pCntnt = pCntnt->GetNextCntntFrm();
        }
        if ( !pActual )
        {	//Wenn noch keiner gefunden wurde muss der Suchbereich erweitert
            //werden, irgenwann muessen wir einen Finden!
            //MA 09. Jan. 97: Opt fuer viele leere Seiten, wenn wir nur im
            //Body suchen, koennen wir den Suchbereich gleich in einem
            //Schritt hinreichend erweitern.
            if ( bBodyOnly )
            {
                while ( !pCntnt && pStart->GetPrev() )
                {
                    ++nMaxPage;
                    if( !pStart->GetPrev()->IsLayoutFrm() )
                        return 0;
                    pStart = (SwLayoutFrm*)pStart->GetPrev();
                    pCntnt = pStart->IsInDocBody()
                                ? pStart->ContainsCntnt()
                                : pStart->FindPageFrm()->FindFirstBodyCntnt();
                }
                if ( !pCntnt )	//irgendwann muessen wir mit irgendeinem Anfangen!
                {
                    pCntnt = pStart->FindPageFrm()->GetUpper()->ContainsCntnt();
                    while ( pCntnt && !pCntnt->IsInDocBody() )
                        pCntnt = pCntnt->GetNextCntntFrm();
                    if ( !pCntnt )
                        return 0;	//Es gibt noch keine Dokumentinhalt!
                }
            }
            else
            {
                ++nMaxPage;
                if ( pStart->GetPrev() )
                {
                    if( !pStart->GetPrev()->IsLayoutFrm() )
                        return 0;
                    pStart = (SwLayoutFrm*)pStart->GetPrev();
                    pCntnt = pStart->ContainsCntnt();
                }
                else //irgendwann muessen wir mit irgendeinem Anfangen!
                    pCntnt = pStart->FindPageFrm()->GetUpper()->ContainsCntnt();
            }
            pActual = pCntnt;
        }
        else
            break;
    }

#ifdef DBG_UTIL
    ASSERT( pActual, "Keinen Cntnt gefunden." );
    if ( bBodyOnly )
        ASSERT( pActual->IsInDocBody(), "Cnt nicht im Body." );
#endif

    //Spezialfall fuer das selektieren von Tabellen, nicht in wiederholte
    //TblHedlines.
    if ( pActual->IsInTab() && pCMS && pCMS->eState == MV_TBLSEL )
    {
        const SwTabFrm *pTab = pActual->FindTabFrm();
        if ( pTab->IsFollow() && pTab->IsInHeadline( *pActual ) )
        {
            ((SwCrsrMoveState*)pCMS)->bStop = TRUE;
            return 0;
        }
    }

    //Jetzt noch eine kleine Korrektur beim ersten/letzten
    Size aActualSize( pActual->Prt().SSize() );
    if ( aActualSize.Height() > pActual->GetUpper()->Prt().Height() )
        aActualSize.Height() = pActual->GetUpper()->Prt().Height();

    SWRECTFN( pActual )
    if ( !pActual->GetPrev() &&
         (*fnRect->fnYDiff)( (pActual->*fnRect->fnGetPrtTop)(),
                              bVert ? rPoint.X() : rPoint.Y() ) > 0 )
    {
        aPoint.Y() = pActual->Frm().Top() + pActual->Prt().Top();
        aPoint.X() = pActual->Frm().Left() +
                        ( pActual->IsRightToLeft() || bVert ?
                          pActual->Prt().Right() :
                          pActual->Prt().Left() );
    }
    else if ( !pActual->GetNext() &&
              (*fnRect->fnYDiff)( (pActual->*fnRect->fnGetPrtBottom)(),
                                   bVert ? rPoint.X() : rPoint.Y() ) < 0 )
    {
        aPoint.Y() = pActual->Frm().Top() + pActual->Prt().Bottom();
        aPoint.X() = pActual->Frm().Left() +
                        ( pActual->IsRightToLeft() || bVert ?
                          pActual->Prt().Left() :
                          pActual->Prt().Right() );
    }

    //Und den Point in die PrtArea bringen
    if ( bCalc )
        pActual->Calc();
    const SwRect aRect( pActual->Frm().Pos() + pActual->Prt().Pos(),
                        aActualSize );
    if ( aPoint.Y() < aRect.Top() )
        aPoint.Y() = aRect.Top();
    else if ( aPoint.Y() > aRect.Bottom() )
        aPoint.Y() = aRect.Bottom();
    if ( aPoint.X() < aRect.Left() )
        aPoint.X() = aRect.Left();
    else if ( aPoint.X() > aRect.Right() )
        aPoint.X() = aRect.Right();
    rPoint = aPoint;
    return pActual;
}

/*************************************************************************
|*
|*	SwPageFrm::GetCntntPosition()
|*
|*	Beschreibung		Analog zu SwLayoutFrm::GetCntntPos().
|* 						Spezialisiert fuer Felder in Rahmen.
|*
|*	Ersterstellung		MA 22. Mar. 95
|*	Letzte Aenderung	MA 07. Nov. 95
|*
|*************************************************************************/
void SwPageFrm::GetCntntPosition( const Point &rPt, SwPosition &rPos ) const
{
    //Ersten CntntFrm ermitteln.
    const SwCntntFrm *pCntnt = ContainsCntnt();
    if ( pCntnt )
    {
        //Einen weiter zurueck schauen (falls moeglich).
        const SwCntntFrm *pTmp = pCntnt->GetPrevCntntFrm();
        while ( pTmp && !pTmp->IsInDocBody() )
            pTmp = pTmp->GetPrevCntntFrm();
        if ( pTmp )
            pCntnt = pTmp;
    }
    else
        pCntnt = GetUpper()->ContainsCntnt();

    const SwCntntFrm *pAct = pCntnt;
    Point aAct 		 = rPt;
    ULONG nDist		 = ULONG_MAX;

    while ( pCntnt )
    {
        SwRect aCntFrm( pCntnt->UnionFrm() );
        if ( aCntFrm.IsInside( rPt ) )
        {
            //dichter gehts nimmer.
            pAct = pCntnt;
            break;
        }

        //Die Strecke von rPt zum dichtesten Punkt von pCntnt berechnen.
        Point aPoint( rPt );

        //Erst die vertikale Position einstellen
        if ( aCntFrm.Top() > rPt.Y() )
            aPoint.Y() = aCntFrm.Top();
        else if ( aCntFrm.Bottom() < rPt.Y() )
            aPoint.Y() = aCntFrm.Bottom();

        //Jetzt die horizontale Position
        if ( aCntFrm.Left() > rPt.X() )
            aPoint.X() = aCntFrm.Left();
        else if ( aCntFrm.Right() < rPt.X() )
            aPoint.X() = aCntFrm.Right();

        const ULONG nDiff = ::CalcDiff( aPoint, rPt );
        if ( nDiff < nDist )
        {
            aAct	= aPoint;
            nDist	= nDiff;
            pAct	= pCntnt;
        }
        else if ( aCntFrm.Top() > Frm().Bottom() )
            //Dichter wirds im Sinne der Felder nicht mehr!
            break;

        pCntnt = pCntnt->GetNextCntntFrm();
        while ( pCntnt && !pCntnt->IsInDocBody() )
            pCntnt = pCntnt->GetNextCntntFrm();
    }

    //Und den Point in die PrtArea bringen
    const SwRect aRect( pAct->Frm().Pos() + pAct->Prt().Pos(), pAct->Prt().SSize() );
    if ( aAct.Y() < aRect.Top() )
        aAct.Y() = aRect.Top();
    else if ( aAct.Y() > aRect.Bottom() )
        aAct.Y() = aRect.Bottom();
    if ( aAct.X() < aRect.Left() )
        aAct.X() = aRect.Left();
    else if ( aAct.X() > aRect.Right() )
        aAct.X() = aRect.Right();

    if( !pAct->IsValid() )
    {
        // CntntFrm nicht formatiert -> immer auf Node-Anfang
        SwCntntNode* pCNd = (SwCntntNode*)pAct->GetNode();
        ASSERT( pCNd, "Wo ist mein CntntNode?" );
        rPos.nNode = *pCNd;
        rPos.nContent.Assign( pCNd, 0 );
    }
    else
    {
        SwCrsrMoveState aTmpState( MV_SETONLYTEXT );
        pAct->GetCrsrOfst( &rPos, aAct, &aTmpState );
    }
}

/*************************************************************************
|*
|*	SwRootFrm::GetNextPrevCntntPos()
|*
|*	Beschreibung		Es wird der naechstliegende Cntnt zum uebergebenen
|* 						Point gesucht. Es wird nur im BodyText gesucht.
|*	Ersterstellung		MA 15. Jul. 92
|*	Letzte Aenderung	JP 11.10.2001
|*
|*************************************************************************/

// --> OD 2005-05-25 #123110# - helper class to disable creation of an action
// by a callback event - e.g., change event from a drawing object
class DisableCallbackAction
{
    private:
        SwRootFrm& mrRootFrm;
        BOOL mbOldCallbackActionState;

    public:
        DisableCallbackAction( const SwRootFrm& _rRootFrm ) :
            mrRootFrm( const_cast<SwRootFrm&>(_rRootFrm) ),
            mbOldCallbackActionState( _rRootFrm.IsCallbackActionEnabled() )
        {
            mrRootFrm.SetCallbackActionEnabled( FALSE );
        }

        ~DisableCallbackAction()
        {
            mrRootFrm.SetCallbackActionEnabled( mbOldCallbackActionState );
        }
};
// <--

//!!!!! Es wird nur der vertikal naechstliegende gesucht.
//JP 11.10.2001: only in tables we try to find the right column - Bug 72294
Point SwRootFrm::GetNextPrevCntntPos( const Point& rPoint, BOOL bNext ) const
{
    // --> OD 2005-05-25 #123110# - disable creation of an action by a callback
    // event during processing of this method. Needed because formatting is
    // triggered by this method.
    DisableCallbackAction aDisableCallbackAction( *this );
    // <--
    //Ersten CntntFrm und seinen Nachfolger im Body-Bereich suchen
    //Damit wir uns nicht tot suchen (und vor allem nicht zuviel formatieren)
    //gehen wir schon mal von der richtigen Seite aus.
    SwLayoutFrm *pPage = (SwLayoutFrm*)Lower();
    if( pPage )
        while( pPage->GetNext() && pPage->Frm().Bottom() < rPoint.Y() )
            pPage = (SwLayoutFrm*)pPage->GetNext();

    const SwCntntFrm *pCnt = pPage ? pPage->ContainsCntnt() : ContainsCntnt();
    while ( pCnt && !pCnt->IsInDocBody() )
        pCnt = pCnt->GetNextCntntFrm();

    if ( !pCnt )
        return Point( 0, 0 );

    pCnt->Calc();
    if( !bNext )
    {
        // Solange der Point vor dem ersten CntntFrm liegt und es noch
        // vorhergehende Seiten gibt gehe ich jeweils eine Seite nach vorn.
        while ( rPoint.Y() < pCnt->Frm().Top() && pPage->GetPrev() )
        {
            pPage = (SwLayoutFrm*)pPage->GetPrev();
            pCnt = pPage->ContainsCntnt();
            while ( !pCnt )
            {
                pPage = (SwLayoutFrm*)pPage->GetPrev();
                if ( pPage )
                    pCnt = pPage->ContainsCntnt();
                else
                    return ContainsCntnt()->UnionFrm().Pos();
            }
            pCnt->Calc();
        }
    }

    //Liegt der Point ueber dem ersten CntntFrm?
    if ( rPoint.Y() < pCnt->Frm().Top() && !lcl_IsInRepeatedHeadline( pCnt ) )
        return pCnt->UnionFrm().Pos();

    while ( pCnt )
    {
        //Liegt der Point im aktuellen CntntFrm?
        SwRect aCntFrm( pCnt->UnionFrm() );
        if ( aCntFrm.IsInside( rPoint ) && !lcl_IsInRepeatedHeadline( pCnt ))
            return rPoint;

        //Ist der aktuelle der letzte CntntFrm? ||
        //Wenn der naechste CntntFrm hinter dem Point liegt, ist der
        //aktuelle der gesuchte.
        const SwCntntFrm *pNxt = pCnt->GetNextCntntFrm();
        while ( pNxt && !pNxt->IsInDocBody() )
            pNxt = pNxt->GetNextCntntFrm();

        //Liegt der Point hinter dem letzten CntntFrm?
        if ( !pNxt )
            return Point( aCntFrm.Right(), aCntFrm.Bottom() );

        //Wenn der naechste CntntFrm hinter dem Point liegt ist er der
        //gesuchte.
        const SwTabFrm* pTFrm;
        pNxt->Calc();
        if( pNxt->Frm().Top() > rPoint.Y() &&
            !lcl_IsInRepeatedHeadline( pCnt, &pTFrm ) &&
            ( !pTFrm || pNxt->Frm().Left() > rPoint.X() ))
        {
            if( bNext )
                return pNxt->Frm().Pos();
            return Point( aCntFrm.Right(), aCntFrm.Bottom() );
        }
        pCnt = pNxt;
    }
    return Point( 0, 0 );
}

/*************************************************************************
|*
|*	SwRootFrm::GetPagePos()
|*
|*	Beschreibung:	Liefert die absolute Dokumentpositon der gewuenschten
|*			Seite.
|*			Formatiert wird nur soweit notwendig und nur dann wenn bFormat=TRUE
|*			Liefert Null, wenn die Operation nicht moeglich ist.
|*			Die Pos ist die der letzten Seite, wenn die Seitenzahl zu gross
|*			gewaehlt wurde.
|*	Ersterstellung		MA 01. Jun. 92
|*	Letzte Aenderung	MA 09. Oct. 97
|*
|*************************************************************************/
Point SwRootFrm::GetPagePos( USHORT nPageNum ) const
{
    ASSERT( Lower() && Lower()->IsPageFrm(), "Keine Seite vorhanden." );

    const SwPageFrm *pPage = (const SwPageFrm*)Lower();
    while ( TRUE )
    {
        if ( pPage->GetPhyPageNum() >= nPageNum || !pPage->GetNext() )
            break;
        pPage = (const SwPageFrm*)pPage->GetNext();
    }
    return pPage->Frm().Pos();
}

/** get page frame by phyiscal page number

    OD 14.01.2003 #103492#

    @return pointer to the page frame with the given physical page number
*/
SwPageFrm* SwRootFrm::GetPageByPageNum( sal_uInt16 _nPageNum ) const
{
    const SwPageFrm* pPageFrm = static_cast<const SwPageFrm*>( Lower() );
    while ( pPageFrm && pPageFrm->GetPhyPageNum() < _nPageNum )
    {
          pPageFrm = static_cast<const SwPageFrm*>( pPageFrm->GetNext() );
    }

    if ( pPageFrm && pPageFrm->GetPhyPageNum() == _nPageNum )
    {
        return const_cast<SwPageFrm*>( pPageFrm );
    }
    else
    {
        return 0;
    }
}

/*************************************************************************
|*
|*  SwRootFrm::IsDummyPage(USHORT)
|*
|*  Description: Returns TRUE, when the given physical pagenumber does't exist
|*               or this page is an empty page.
|*************************************************************************/
BOOL SwRootFrm::IsDummyPage( USHORT nPageNum ) const
{
    if( !Lower() || !nPageNum || nPageNum > GetPageNum() )
        return TRUE;

    const SwPageFrm *pPage = (const SwPageFrm*)Lower();
    while( pPage && nPageNum < pPage->GetPhyPageNum() )
        pPage = (const SwPageFrm*)pPage->GetNext();
    return pPage ? pPage->IsEmptyPage() : TRUE;
}


/*************************************************************************
|*
|*	  SwFrm::IsProtected()
|*
|*	  Beschreibung		Ist der Frm bzw. die Section in der er steht
|* 						geschuetzt?
|* 						Auch Fly in Fly in ... und Fussnoten
|*
|*	  Ersterstellung	MA 28. Jul. 93
|*	  Letzte Aenderung	MA 06. Nov. 97
|*
|*************************************************************************/
BOOL SwFrm::IsProtected() const
{
    if (this->IsCntntFrm() && ((SwCntntFrm*)this)->GetNode())
    {
        const SwDoc *pDoc=((SwCntntFrm*)this)->GetNode()->GetDoc();
        bool isFormProtected=pDoc->get(IDocumentSettingAccess::PROTECT_FORM );
        if (isFormProtected)
        {
            return FALSE; // TODO a hack for now, well deal with it laster, I we return true here we have a "double" locking
        }
    }
    //Der Frm kann in Rahmen, Zellen oder Bereichen geschuetzt sein.
    //Geht auch FlyFrms rekursiv hoch. Geht auch von Fussnoten zum Anker.
    const SwFrm *pFrm = this;
    do
    {
        if ( pFrm->IsCntntFrm() )
        {
            if ( ((SwCntntFrm*)pFrm)->GetNode() &&
                 ((SwCntntFrm*)pFrm)->GetNode()->IsInProtectSect() )
                return TRUE;
        }
        else
        {
            if ( ((SwLayoutFrm*)pFrm)->GetFmt() &&
                 ((SwLayoutFrm*)pFrm)->GetFmt()->
                 GetProtect().IsCntntProtected() )
                return TRUE;
            if ( pFrm->IsCoveredCell() )
                return TRUE;
        }
        if ( pFrm->IsFlyFrm() )
        {
            //Der Schutz des Inhaltes kann bei Verkettung vom Master der Kette
            //vorgegeben werden.
            if ( ((SwFlyFrm*)pFrm)->GetPrevLink() )
            {
                SwFlyFrm *pMaster = (SwFlyFrm*)pFrm;
                do
                {   pMaster = pMaster->GetPrevLink();
                } while ( pMaster->GetPrevLink() );
                if ( pMaster->IsProtected() )
                    return TRUE;
            }
            pFrm = ((SwFlyFrm*)pFrm)->GetAnchorFrm();
        }
        else if ( pFrm->IsFtnFrm() )
            pFrm = ((SwFtnFrm*)pFrm)->GetRef();
        else
            pFrm = pFrm->GetUpper();

    } while ( pFrm );

    return FALSE;
}

/*************************************************************************
|*
|*	  SwFrm::GetPhyPageNum()
|*	  Beschreibung:		Liefert die physikalische Seitennummer
|*
|*	  Ersterstellung	OK 06.07.93 08:35
|*	  Letzte Aenderung	MA 30. Nov. 94
|*
|*************************************************************************/
USHORT SwFrm::GetPhyPageNum() const
{
    const SwPageFrm *pPage = FindPageFrm();
    return pPage ? pPage->GetPhyPageNum() : 0;
}

/*-----------------26.02.01 11:25-------------------
 * SwFrm::WannaRightPage()
 * decides if the page want to be a rightpage or not.
 * If the first content of the page has a page descriptor,
 * we take the follow of the page descriptor of the last not empty page.
 * If this descriptor allows only right(left) pages and the page
 * isn't an empty page then it wanna be such right(left) page.
 * If the descriptor allows right and left pages, we look for a number offset
 * in the first content. If there is one, odd number results right pages,
 * even number results left pages.
 * If there is no number offset, we take the physical page number instead,
 * but a previous empty page don't count.
 * --------------------------------------------------*/

BOOL SwFrm::WannaRightPage() const
{
    const SwPageFrm *pPage = FindPageFrm();
    if ( !pPage || !pPage->GetUpper() )
        return TRUE;

    const SwFrm *pFlow = pPage->FindFirstBodyCntnt();
    SwPageDesc *pDesc = 0;
    USHORT nPgNum = 0;
    if ( pFlow )
    {
        if ( pFlow->IsInTab() )
            pFlow = pFlow->FindTabFrm();
        const SwFlowFrm *pTmp = SwFlowFrm::CastFlowFrm( pFlow );
        if ( !pTmp->IsFollow() )
        {
            const SwFmtPageDesc& rPgDesc = pFlow->GetAttrSet()->GetPageDesc();
            pDesc = (SwPageDesc*)rPgDesc.GetPageDesc();
            nPgNum = rPgDesc.GetNumOffset();
        }
    }
    if ( !pDesc )
    {
        SwPageFrm *pPrv = (SwPageFrm*)pPage->GetPrev();
        if( pPrv && pPrv->IsEmptyPage() )
            pPrv = (SwPageFrm*)pPrv->GetPrev();
        if( pPrv )
            pDesc = pPrv->GetPageDesc()->GetFollow();
        else
        {
            const SwDoc* pDoc = pPage->GetFmt()->GetDoc();
            pDesc = (SwPageDesc*)&pDoc->GetPageDesc( 0 );
        }
    }
    ASSERT( pDesc, "No pagedescriptor" );
    BOOL bOdd;
    if( nPgNum )
        bOdd = nPgNum % 2 ? TRUE : FALSE;
    else
    {
        bOdd = pPage->OnRightPage();
        if( pPage->GetPrev() && ((SwPageFrm*)pPage->GetPrev())->IsEmptyPage() )
            bOdd = !bOdd;
    }
    if( !pPage->IsEmptyPage() )
    {
        if( !pDesc->GetRightFmt() )
            bOdd = FALSE;
        else if( !pDesc->GetLeftFmt() )
            bOdd = TRUE;
    }
    return bOdd;
}

/*************************************************************************
|*
|*	  SwFrm::GetVirtPageNum()
|*	  Beschreibung:		Liefert die virtuelle Seitennummer mit Offset
|*
|*	  Ersterstellung	OK 06.07.93 08:35
|*	  Letzte Aenderung	MA 30. Nov. 94
|*
|*************************************************************************/
USHORT SwFrm::GetVirtPageNum() const
{
    const SwPageFrm *pPage = FindPageFrm();
    if ( !pPage || !pPage->GetUpper() )
        return 0;

    USHORT nPhyPage = pPage->GetPhyPageNum();
    if ( !((SwRootFrm*)pPage->GetUpper())->IsVirtPageNum() )
        return nPhyPage;

    //Den am naechsten stehenden Absatz mit virtueller Seitennummer suchen.
    //Da das rueckwaertsuchen insgesamt sehr viel Zeit verschlingt suchen
    //wir jetzt gezielt ueber die Abhaengigkeiten.
    //von den PageDescs bekommen wir die Attribute, von den Attributen
    //wiederum bekommen wir die Absaetze.
    const SwPageFrm *pVirtPage = 0;
    const SwFrm *pFrm = 0;
    const SfxItemPool &rPool = pPage->GetFmt()->GetDoc()->GetAttrPool();
    const SfxPoolItem* pItem;
    USHORT nMaxItems = rPool.GetItemCount( RES_PAGEDESC );
    for( USHORT n = 0; n < nMaxItems; ++n )
    {
        if( 0 == (pItem = rPool.GetItem( RES_PAGEDESC, n ) ))
            continue;

        const SwFmtPageDesc *pDesc = (SwFmtPageDesc*)pItem;
        if ( pDesc->GetNumOffset() && pDesc->GetDefinedIn() )
        {
            const SwModify *pMod = pDesc->GetDefinedIn();
            SwVirtPageNumInfo aInfo( pPage );
            pMod->GetInfo( aInfo );
            if ( aInfo.GetPage() )
            {
                if( !pVirtPage || ( pVirtPage && aInfo.GetPage()->
                    GetPhyPageNum() > pVirtPage->GetPhyPageNum() ) )
                {
                    pVirtPage = aInfo.GetPage();
                    pFrm = aInfo.GetFrm();
                }
            }
        }
    }
    if ( pFrm )
        return nPhyPage - pFrm->GetPhyPageNum() +
               pFrm->GetAttrSet()->GetPageDesc().GetNumOffset();
    return nPhyPage;
}

/*************************************************************************
|*
|*	SwRootFrm::MakeTblCrsrs()
|*
|*	Ersterstellung		MA 14. May. 93
|*	Letzte Aenderung	MA 02. Feb. 94
|*
|*************************************************************************/
//Ermitteln und einstellen derjenigen Zellen die von der Selektion
//eingeschlossen sind.

bool SwRootFrm::MakeTblCrsrs( SwTableCursor& rTblCrsr )
{
    //Union-Rects und Tabellen (Follows) der Selektion besorgen.
    ASSERT( rTblCrsr.GetCntntNode() && rTblCrsr.GetCntntNode( FALSE ),
            "Tabselection nicht auf Cnt." );

    bool bRet = false;

    // For new table models there's no need to ask the layout..
    if( rTblCrsr.NewTableSelection() )
        return true;

    Point aPtPt, aMkPt;
    {
        SwShellCrsr* pShCrsr = dynamic_cast<SwShellCrsr*>(&rTblCrsr);

        if( pShCrsr )
        {
            aPtPt = pShCrsr->GetPtPos();
            aMkPt = pShCrsr->GetMkPos();
        }
    }

    // --> FME 2008-01-14 #151012# Made code robust here:
    const SwCntntNode* pTmpStartNode = rTblCrsr.GetCntntNode();
    const SwCntntNode* pTmpEndNode   = rTblCrsr.GetCntntNode(FALSE);

    const SwFrm* pTmpStartFrm = pTmpStartNode ? pTmpStartNode->GetFrm( &aPtPt, 0, FALSE ) : 0;
    const SwFrm* pTmpEndFrm   = pTmpEndNode   ?   pTmpEndNode->GetFrm( &aMkPt, 0, FALSE ) : 0;

    const SwLayoutFrm* pStart = pTmpStartFrm ? pTmpStartFrm->GetUpper() : 0;
    const SwLayoutFrm* pEnd   = pTmpEndFrm   ? pTmpEndFrm->GetUpper() : 0;

    ASSERT( pStart && pEnd, "MakeTblCrsrs: Good to have the code robust here!" )
    // <--

    /* #109590# Only change table boxes if the frames are
        valid. Needed because otherwise the table cursor after moving
        table cells by dnd resulted in an empty tables cursor.  */
    if ( pStart && pEnd && pStart->IsValid() && pEnd->IsValid())
    {
        SwSelUnions aUnions;
        ::MakeSelUnions( aUnions, pStart, pEnd );

        SwSelBoxes aNew;

        const BOOL bReadOnlyAvailable = rTblCrsr.IsReadOnlyAvailable();

        for ( USHORT i = 0; i < aUnions.Count(); ++i )
        {
            SwSelUnion *pUnion = aUnions[i];
            const SwTabFrm *pTable = pUnion->GetTable();

            // Skip any repeated headlines in the follow:
            SwLayoutFrm* pRow = pTable->IsFollow() ?
                                pTable->GetFirstNonHeadlineRow() :
                                (SwLayoutFrm*)pTable->Lower();

            while ( pRow )
            {
                if ( pRow->Frm().IsOver( pUnion->GetUnion() ) )
                {
                    const SwLayoutFrm *pCell = pRow->FirstCell();

                    while ( pCell && pRow->IsAnLower( pCell ) )
                    {
                        ASSERT( pCell->IsCellFrm(), "Frame ohne Celle" );
                        if( IsFrmInTblSel( pUnion->GetUnion(), pCell ) &&
                            (bReadOnlyAvailable ||
                             !pCell->GetFmt()->GetProtect().IsCntntProtected()))
                        {
                            SwTableBox* pInsBox = (SwTableBox*)
                                ((SwCellFrm*)pCell)->GetTabBox();
                            aNew.Insert( pInsBox );
                        }
                        if ( pCell->GetNext() )
                        {
                            pCell = (const SwLayoutFrm*)pCell->GetNext();
                            if ( pCell->Lower() && pCell->Lower()->IsRowFrm() )
                                pCell = pCell->FirstCell();
                        }
                        else
                        {
                            const SwLayoutFrm* pLastCell = pCell;
                            do
                            {
                                pCell = pCell->GetNextLayoutLeaf();
                            } while ( pCell && pLastCell->IsAnLower( pCell ) );
                            // Fuer (spaltige) Bereiche...
                            if( pCell && pCell->IsInTab() )
                            {
                                while( !pCell->IsCellFrm() )
                                {
                                    pCell = pCell->GetUpper();
                                    ASSERT( pCell, "Where's my cell?" );
                                }
                            }
                        }
                    }
                }
                pRow = (SwLayoutFrm*)pRow->GetNext();
            }
        }

        rTblCrsr.ActualizeSelection( aNew );
        bRet = true;
    }

    return bRet;
}


/*************************************************************************
|*
|*	SwRootFrm::CalcFrmRects
|*
|*	Ersterstellung		MA 24. Aug. 92
|*	Letzte Aenderung	MA 24. Aug. 93
|*
|*************************************************************************/

/*
 * nun koennen folgende Situationen auftreten:
 *	1. Start und Ende liegen in einer Bildschirm - Zeile und im
 * 	   gleichen Node
 *		-> aus Start und End ein Rectangle, dann Ok
 *	2. Start und Ende liegen in einem Frame (dadurch im gleichen Node!)
 *		-> Start nach rechts, End nach links erweitern,
 *		   und bei mehr als 2 Bildschirm - Zeilen, das dazwischen
 *		   liegende berechnen
 * 	3. Start und Ende liegen in verschiedenen Frames
 *		-> Start nach rechts erweitern, bis Frame-Ende Rect berechnen
 *		   Ende nach links erweitern, bis Frame-Start Rect berechnen
 *		   und bei mehr als 2 Frames von allen dazwischen liegenden
 * 		   Frames die PrtArea dazu.
 *	4. Wenn es sich um eine Tabellenselektion handelt wird fuer jeden
 * 	   PaM im Ring der CellFrm besorgt, dessen PrtArea wird zu den
 * 	   Rechtecken addiert.
 *
 * Grosser Umbau wg. der FlyFrm; denn diese muessen ausgespart werden.
 * Ausnahmen: - Der Fly in dem die Selektion stattfindet (wenn sie in einem Fly
 *				stattfindet).
 * 			  - Die Flys, die vom Text unterlaufen werden.
 * Arbeitsweise: Zuerst wird eine SwRegion mit der Root initialisiert.
 * 				 Aus der Region werden die zu invertierenden Bereiche
 * 				 ausgestantzt. Die Region wird Komprimiert und letztlich
 * 				 invertiert. Damit liegen dann die zu invertierenden
 * 				 Rechtecke vor.
 * 				 Am Ende werden die Flys aus der Region ausgestanzt.
 */

inline void Sub( SwRegionRects& rRegion, const SwRect& rRect )
{
    if( rRect.Width() > 1 && rRect.Height() > 1 &&
        rRect.IsOver( rRegion.GetOrigin() ))
        rRegion -= rRect;
}

void SwRootFrm::CalcFrmRects( SwShellCrsr &rCrsr, BOOL bIsTblMode )
{
    const SwNodes &rNds = GetFmt()->GetDoc()->GetNodes();
    SwPosition *pStartPos = rCrsr.Start(),
               *pEndPos   = rCrsr.GetPoint() == pStartPos ?
                            rCrsr.GetMark() : rCrsr.GetPoint();

    ViewShell *pSh = GetShell();

// --> FME 2004-06-08 #i12836# enhanced pdf
    SwRegionRects aRegion( pSh && !pSh->GetViewOptions()->IsPDFExport() ?
                           pSh->VisArea() :
                           Frm() );
// <--
    if( !pStartPos->nNode.GetNode().IsCntntNode() ||
        !pStartPos->nNode.GetNode().GetCntntNode()->GetFrm() ||
        ( pStartPos->nNode != pEndPos->nNode &&
          ( !pEndPos->nNode.GetNode().IsCntntNode() ||
            !pEndPos->nNode.GetNode().GetCntntNode()->GetFrm() ) ) )
    {
        /* For SelectAll we will need something like this later on...
        const SwFrm* pPageFrm = GetLower();
        while( pPageFrm )
        {
            SwRect aTmp( pPageFrm->Prt() );
            aTmp.Pos() += pPageFrm->Frm().Pos();
            Sub( aRegion, aTmp );
            pPageFrm = pPageFrm->GetNext();
        }
        aRegion.Invert();
        rCrsr.Remove( 0, rCrsr.Count() );
        rCrsr.Insert( &aRegion, 0 );
        */
        return;
    }

    //Erstmal die CntntFrms zum Start und End besorgen, die brauch ich auf
    //jedenfall.
    const SwCntntFrm *pStartFrm = rNds[ pStartPos->nNode ]->
        GetCntntNode()->GetFrm( &rCrsr.GetSttPos(), pStartPos );

    const SwCntntFrm *pEndFrm	= rNds[ pEndPos->nNode ]->
        GetCntntNode()->GetFrm( &rCrsr.GetEndPos(), pEndPos );

    ASSERT( (pStartFrm && pEndFrm), "Keine CntntFrms gefunden." );

    //Damit die FlyFrms, in denen selektierte Frames stecken, nicht
    //abgezogen werden
    SwSortedObjs aSortObjs;
    if ( pStartFrm->IsInFly() )
    {
        const SwAnchoredObject* pObj = pStartFrm->FindFlyFrm();
        aSortObjs.Insert( *(const_cast<SwAnchoredObject*>(pObj)) );
        const SwAnchoredObject* pObj2 = pEndFrm->FindFlyFrm();
        aSortObjs.Insert( *(const_cast<SwAnchoredObject*>(pObj2)) );
    }

    //Fall 4: Tabellenselection
    if( bIsTblMode )
    {
        const SwFrm *pCell = pStartFrm->GetUpper();
        while ( !pCell->IsCellFrm() )
            pCell = pCell->GetUpper();
        SwRect aTmp( pCell->Prt() );
        aTmp.Pos() += pCell->Frm().Pos();
        aRegion.ChangeOrigin( aTmp );
        aRegion.Remove( 0, aRegion.Count() );
        aRegion.Insert( aTmp, 0 );
    }
    else
    {
        // falls eine nicht erlaubte Selection besteht, dann korrigiere das
        // nicht erlaubt ist Header/Footer/TableHeadline ueber 2 Seiten
        do {	// middle check loop
            const SwLayoutFrm* pSttLFrm = pStartFrm->GetUpper();
            const USHORT cHdFtTblHd = FRM_HEADER | FRM_FOOTER | FRM_TAB;
            while( pSttLFrm &&
                ! (cHdFtTblHd & pSttLFrm->GetType() ))
                pSttLFrm = pSttLFrm->GetUpper();
            if( !pSttLFrm )
                break;
            const SwLayoutFrm* pEndLFrm = pEndFrm->GetUpper();
            while( pEndLFrm &&
                ! (cHdFtTblHd & pEndLFrm->GetType() ))
                pEndLFrm = pEndLFrm->GetUpper();
            if( !pEndLFrm )
                break;

            ASSERT( pEndLFrm->GetType() == pSttLFrm->GetType(),
                    "Selection ueber unterschiedliche Inhalte" );
            switch( pSttLFrm->GetType() )
            {
            case FRM_HEADER:
            case FRM_FOOTER:
                // auf unterschiedlichen Seiten ??
                // dann immer auf die Start-Seite
                if( pEndLFrm->FindPageFrm() != pSttLFrm->FindPageFrm() )
                {
                    // End- auf den Start-CntntFrame setzen
                    if( pStartPos == rCrsr.GetPoint() )
                        pEndFrm = pStartFrm;
                    else
                        pStartFrm = pEndFrm;
                }
                break;
            case FRM_TAB:
                // auf unterschiedlichen Seiten ??
                // existiert
                // dann teste auf Tabelle-Headline
                {
                    const SwTabFrm* pTabFrm = (SwTabFrm*)pSttLFrm;
                    if( ( pTabFrm->GetFollow() ||
                          ((SwTabFrm*)pEndLFrm)->GetFollow() ) &&
                        pTabFrm->GetTable()->GetRowsToRepeat() > 0 &&
                        pTabFrm->GetLower() != ((SwTabFrm*)pEndLFrm)->GetLower() &&
                        ( lcl_IsInRepeatedHeadline( pStartFrm ) ||
                          lcl_IsInRepeatedHeadline( pEndFrm ) ) )
                    {
                        // End- auf den Start-CntntFrame setzen
                        if( pStartPos == rCrsr.GetPoint() )
                            pEndFrm = pStartFrm;
                        else
                            pStartFrm = pEndFrm;
                    }
                }
                break;
            }
        } while( FALSE );

        SwCrsrMoveState aTmpState( MV_NONE );
        aTmpState.b2Lines = sal_True;
        aTmpState.bNoScroll = sal_True;
        aTmpState.nCursorBidiLevel = pStartFrm->IsRightToLeft() ? 1 : 0;

        //CntntRects zu Start- und EndFrms.
        SwRect aStRect, aEndRect;
        pStartFrm->GetCharRect( aStRect, *pStartPos, &aTmpState );
        Sw2LinesPos *pSt2Pos = aTmpState.p2Lines;
        aTmpState.p2Lines = NULL;
        aTmpState.nCursorBidiLevel = pEndFrm->IsRightToLeft() ? 1 : 0;

        pEndFrm->GetCharRect  ( aEndRect, *pEndPos, &aTmpState );
        Sw2LinesPos *pEnd2Pos = aTmpState.p2Lines;

        SwRect aStFrm ( pStartFrm->UnionFrm( sal_True ) );
        aStFrm.Intersection( pStartFrm->PaintArea() );
        SwRect aEndFrm( pStartFrm == pEndFrm ? aStFrm :
                                               pEndFrm->UnionFrm( sal_True ) );
        if( pStartFrm != pEndFrm )
            aEndFrm.Intersection( pEndFrm->PaintArea() );
        SWRECTFN( pStartFrm )
        const BOOL bR2L = pStartFrm->IsRightToLeft();
        const BOOL bEndR2L = pEndFrm->IsRightToLeft();

        // If there's no doubleline portion involved or start and end are both
        // in the same doubleline portion, all works fine, but otherwise
        // we need the following...
        if( pSt2Pos != pEnd2Pos && ( !pSt2Pos || !pEnd2Pos ||
            pSt2Pos->aPortion != pEnd2Pos->aPortion ) )
        {
            // If we have a start(end) position inside a doubleline portion
            // the surrounded part of the doubleline portion is subtracted
            // from the region and the aStRect(aEndRect) is set to the
            // end(start) of the doubleline portion.
            if( pSt2Pos )
            {
                SwRect aTmp( aStRect );

                // BiDi-Portions are swimming against the current.
                const sal_Bool bPorR2L = ( MT_BIDI == pSt2Pos->nMultiType ) ?
                                           ! bR2L :
                                             bR2L;

                if( MT_BIDI == pSt2Pos->nMultiType &&
                    (pSt2Pos->aPortion2.*fnRect->fnGetWidth)() )
                {
                    // nested bidi portion
                    long nRightAbs = (pSt2Pos->aPortion.*fnRect->fnGetRight)();
                    nRightAbs -= (pSt2Pos->aPortion2.*fnRect->fnGetLeft)();
                    long nLeftAbs = nRightAbs - (pSt2Pos->aPortion2.*fnRect->fnGetWidth)();

                    (aTmp.*fnRect->fnSetRight)( nRightAbs );

                    if ( ! pEnd2Pos || pEnd2Pos->aPortion != pSt2Pos->aPortion )
                    {
                        SwRect aTmp2( pSt2Pos->aPortion );
                        (aTmp2.*fnRect->fnSetRight)( nLeftAbs );
                        aTmp2.Intersection( aEndFrm );
                        Sub( aRegion, aTmp2 );
                    }
                }
                else
                {
                    if( bPorR2L )
                        (aTmp.*fnRect->fnSetLeft)(
                            (pSt2Pos->aPortion.*fnRect->fnGetLeft)() );
                    else
                        (aTmp.*fnRect->fnSetRight)(
                            (pSt2Pos->aPortion.*fnRect->fnGetRight)() );
                }

                if( MT_ROT_90 == pSt2Pos->nMultiType ||
                    (pSt2Pos->aPortion.*fnRect->fnGetTop)() ==
                    (aTmp.*fnRect->fnGetTop)() )
                {
                    (aTmp.*fnRect->fnSetTop)(
                        (pSt2Pos->aLine.*fnRect->fnGetTop)() );
                }

                aTmp.Intersection( aStFrm );
                Sub( aRegion, aTmp );

                SwTwips nTmp = (pSt2Pos->aLine.*fnRect->fnGetBottom)();
                if( MT_ROT_90 != pSt2Pos->nMultiType &&
                    (aStRect.*fnRect->fnBottomDist)( nTmp ) > 0 )
                {
                    (aTmp.*fnRect->fnSetTop)( (aTmp.*fnRect->fnGetBottom)() );
                    (aTmp.*fnRect->fnSetBottom)( nTmp );
                    if( (aStRect.*fnRect->fnBottomDist)(
                        (pSt2Pos->aPortion.*fnRect->fnGetBottom)() ) > 0 )
                    {
                        if( bPorR2L )
                            (aTmp.*fnRect->fnSetRight)(
                                (pSt2Pos->aPortion.*fnRect->fnGetRight)() );
                        else
                            (aTmp.*fnRect->fnSetLeft)(
                                (pSt2Pos->aPortion.*fnRect->fnGetLeft)() );
                    }
                    aTmp.Intersection( aStFrm );
                    Sub( aRegion, aTmp );
                }

                aStRect = pSt2Pos->aLine;
                (aStRect.*fnRect->fnSetLeft)( bR2L ?
                        (pSt2Pos->aPortion.*fnRect->fnGetLeft)() :
                        (pSt2Pos->aPortion.*fnRect->fnGetRight)() );
                (aStRect.*fnRect->fnSetWidth)( 1 );
            }

            if( pEnd2Pos )
            {
                SWRECTFNX( pEndFrm )
                SwRect aTmp( aEndRect );

                // BiDi-Portions are swimming against the current.
                const sal_Bool bPorR2L = ( MT_BIDI == pEnd2Pos->nMultiType ) ?
                                           ! bEndR2L :
                                             bEndR2L;

                if( MT_BIDI == pEnd2Pos->nMultiType &&
                    (pEnd2Pos->aPortion2.*fnRectX->fnGetWidth)() )
                {
                    // nested bidi portion
                    long nRightAbs = (pEnd2Pos->aPortion.*fnRectX->fnGetRight)();
                    nRightAbs = nRightAbs - (pEnd2Pos->aPortion2.*fnRectX->fnGetLeft)();
                    long nLeftAbs = nRightAbs - (pEnd2Pos->aPortion2.*fnRectX->fnGetWidth)();

                    (aTmp.*fnRectX->fnSetLeft)( nLeftAbs );

                    if ( ! pSt2Pos || pSt2Pos->aPortion != pEnd2Pos->aPortion )
                    {
                        SwRect aTmp2( pEnd2Pos->aPortion );
                        (aTmp2.*fnRectX->fnSetLeft)( nRightAbs );
                        aTmp2.Intersection( aEndFrm );
                        Sub( aRegion, aTmp2 );
                    }
                }
                else
                {
                    if ( bPorR2L )
                        (aTmp.*fnRectX->fnSetRight)(
                            (pEnd2Pos->aPortion.*fnRectX->fnGetRight)() );
                    else
                        (aTmp.*fnRectX->fnSetLeft)(
                            (pEnd2Pos->aPortion.*fnRectX->fnGetLeft)() );
                }

                if( MT_ROT_90 == pEnd2Pos->nMultiType ||
                    (pEnd2Pos->aPortion.*fnRectX->fnGetBottom)() ==
                    (aEndRect.*fnRectX->fnGetBottom)() )
                {
                    (aTmp.*fnRectX->fnSetBottom)(
                        (pEnd2Pos->aLine.*fnRectX->fnGetBottom)() );
                }

                aTmp.Intersection( aEndFrm );
                Sub( aRegion, aTmp );

                // The next statement means neither ruby nor rotate(90):
                if( !( MT_RUBY & pEnd2Pos->nMultiType ) )
                {
                    SwTwips nTmp = (pEnd2Pos->aLine.*fnRectX->fnGetTop)();
                    if( (aEndRect.*fnRectX->fnGetTop)() != nTmp )
                    {
                        (aTmp.*fnRectX->fnSetBottom)(
                            (aTmp.*fnRectX->fnGetTop)() );
                        (aTmp.*fnRectX->fnSetTop)( nTmp );
                        if( (aEndRect.*fnRectX->fnGetTop)() !=
                            (pEnd2Pos->aPortion.*fnRectX->fnGetTop)() )
                        if( bPorR2L )
                            (aTmp.*fnRectX->fnSetLeft)(
                                (pEnd2Pos->aPortion.*fnRectX->fnGetLeft)() );
                        else
                            (aTmp.*fnRectX->fnSetRight)(
                                (pEnd2Pos->aPortion.*fnRectX->fnGetRight)() );
                        aTmp.Intersection( aEndFrm );
                        Sub( aRegion, aTmp );
                    }
                }

                aEndRect = pEnd2Pos->aLine;
                (aEndRect.*fnRectX->fnSetLeft)( bEndR2L ?
                        (pEnd2Pos->aPortion.*fnRectX->fnGetRight)() :
                        (pEnd2Pos->aPortion.*fnRectX->fnGetLeft)() );
                (aEndRect.*fnRectX->fnSetWidth)( 1 );
            }
        }
        else if( pSt2Pos && pEnd2Pos &&
                 MT_BIDI == pSt2Pos->nMultiType &&
                 MT_BIDI == pEnd2Pos->nMultiType &&
                 pSt2Pos->aPortion == pEnd2Pos->aPortion &&
                 pSt2Pos->aPortion2 != pEnd2Pos->aPortion2 )
        {
            // This is the ugly special case, where the selection starts and
            // ends in the same bidi portion but one start or end is inside a
            // nested bidi portion.

            if ( (pSt2Pos->aPortion2.*fnRect->fnGetWidth)() )
            {
                SwRect aTmp( aStRect );
                long nRightAbs = (pSt2Pos->aPortion.*fnRect->fnGetRight)();
                nRightAbs -= (pSt2Pos->aPortion2.*fnRect->fnGetLeft)();
                long nLeftAbs = nRightAbs - (pSt2Pos->aPortion2.*fnRect->fnGetWidth)();

                (aTmp.*fnRect->fnSetRight)( nRightAbs );
                aTmp.Intersection( aStFrm );
                Sub( aRegion, aTmp );

                aStRect = pSt2Pos->aLine;
                (aStRect.*fnRect->fnSetLeft)( bR2L ? nRightAbs : nLeftAbs );
                (aStRect.*fnRect->fnSetWidth)( 1 );
            }

            SWRECTFNX( pEndFrm )
            if ( (pEnd2Pos->aPortion2.*fnRectX->fnGetWidth)() )
            {
                SwRect aTmp( aEndRect );
                long nRightAbs = (pEnd2Pos->aPortion.*fnRectX->fnGetRight)();
                nRightAbs -= (pEnd2Pos->aPortion2.*fnRectX->fnGetLeft)();
                long nLeftAbs = nRightAbs - (pEnd2Pos->aPortion2.*fnRectX->fnGetWidth)();

                (aTmp.*fnRectX->fnSetLeft)( nLeftAbs );
                aTmp.Intersection( aEndFrm );
                Sub( aRegion, aTmp );

                aEndRect = pEnd2Pos->aLine;
                (aEndRect.*fnRectX->fnSetLeft)( bEndR2L ? nLeftAbs : nRightAbs );
                (aEndRect.*fnRectX->fnSetWidth)( 1 );
            }
        }

        // The charrect may be outside the paintarea (for cursortravelling)
        // but the selection has to be restricted to the paintarea
        if( aStRect.Left() < aStFrm.Left() )
            aStRect.Left( aStFrm.Left() );
        else if( aStRect.Left() > aStFrm.Right() )
            aStRect.Left( aStFrm.Right() );
        SwTwips nTmp = aStRect.Right();
        if( nTmp < aStFrm.Left() )
            aStRect.Right( aStFrm.Left() );
        else if( nTmp > aStFrm.Right() )
            aStRect.Right( aStFrm.Right() );
        if( aEndRect.Left() < aEndFrm.Left() )
            aEndRect.Left( aEndFrm.Left() );
        else if( aEndRect.Left() > aEndFrm.Right() )
            aEndRect.Left( aEndFrm.Right() );
        nTmp = aEndRect.Right();
        if( nTmp < aEndFrm.Left() )
            aEndRect.Right( aEndFrm.Left() );
        else if( nTmp > aEndFrm.Right() )
            aEndRect.Right( aEndFrm.Right() );

        if( pStartFrm == pEndFrm )
        {
            sal_Bool bSameRotatedOrBidi = pSt2Pos && pEnd2Pos &&
                ( MT_BIDI & pSt2Pos->nMultiType ) &&
                pSt2Pos->aPortion == pEnd2Pos->aPortion;
            //case 1: (Same frame and same row)
            if( bSameRotatedOrBidi ||
                (aStRect.*fnRect->fnGetTop)() == (aEndRect.*fnRect->fnGetTop)() )
            {
                Point aTmpSt( aStRect.Pos() );
                Point aTmpEnd( aEndRect.Right(), aEndRect.Bottom() );
                if( bSameRotatedOrBidi || bR2L )
                {
                    if( aTmpSt.Y() > aTmpEnd.Y() )
                    {
                        long nTmpY = aTmpEnd.Y();
                        aTmpEnd.Y() = aTmpSt.Y();
                        aTmpSt.Y() = nTmpY;
                    }
                    if( aTmpSt.X() > aTmpEnd.X() )
                    {
                        long nTmpX = aTmpEnd.X();
                        aTmpEnd.X() = aTmpSt.X();
                        aTmpSt.X() = nTmpX;
                    }
                }

                SwRect aTmp = SwRect( aTmpSt, aTmpEnd );
                // Bug 34888: falls Inhalt selektiert ist, der keinen Platz
                //			  einnimmt (z.B. PostIts,RefMarks, TOXMarks),
                //			  dann mindestens die Breite des Crsr setzen.
                if( 1 == (aTmp.*fnRect->fnGetWidth)() &&
                    pStartPos->nContent.GetIndex() !=
                    pEndPos->nContent.GetIndex() )
                {
                    OutputDevice* pOut = pSh->GetOut();
                    long nCrsrWidth = pOut->GetSettings().GetStyleSettings().
                                        GetCursorSize();
                    (aTmp.*fnRect->fnSetWidth)( pOut->PixelToLogic(
                                              Size( nCrsrWidth, 0 ) ).Width() );
                }
                aTmp.Intersection( aStFrm );
                Sub( aRegion, aTmp );
            }
            //case 2: (Same frame, but not the same line)
            else
            {
                SwTwips lLeft, lRight;
                if( pSt2Pos && pEnd2Pos && pSt2Pos->aPortion == pEnd2Pos->aPortion )
                {
                    lLeft = (pSt2Pos->aPortion.*fnRect->fnGetLeft)();
                    lRight = (pSt2Pos->aPortion.*fnRect->fnGetRight)();
                }
                else
                {
                    lLeft = (pStartFrm->Frm().*fnRect->fnGetLeft)() +
                            (pStartFrm->Prt().*fnRect->fnGetLeft)();
                    lRight = (pStartFrm->Frm().*fnRect->fnGetLeft)() +
                             (pStartFrm->Prt().*fnRect->fnGetRight)();
                }
                if( lLeft < (aStFrm.*fnRect->fnGetLeft)() )
                    lLeft = (aStFrm.*fnRect->fnGetLeft)();
                if( lRight > (aStFrm.*fnRect->fnGetRight)() )
                    lRight = (aStFrm.*fnRect->fnGetRight)();
                SwRect aSubRect( aStRect );
                //First line
                if( bR2L )
                    (aSubRect.*fnRect->fnSetLeft)( lLeft );
                else
                    (aSubRect.*fnRect->fnSetRight)( lRight );
                Sub( aRegion, aSubRect );

                //If there's at least a twips between start- and endline,
                //so the whole area between will be added.
                SwTwips aTmpBottom = (aStRect.*fnRect->fnGetBottom)();
                SwTwips aTmpTop = (aEndRect.*fnRect->fnGetTop)();
                if( aTmpBottom != aTmpTop )
                {
                    (aSubRect.*fnRect->fnSetLeft)( lLeft );
                    (aSubRect.*fnRect->fnSetRight)( lRight );
                    (aSubRect.*fnRect->fnSetTop)( aTmpBottom );
                    (aSubRect.*fnRect->fnSetBottom)( aTmpTop );
                    Sub( aRegion, aSubRect );
                }
                //and the last line
                aSubRect = aEndRect;
                if( bR2L )
                    (aSubRect.*fnRect->fnSetRight)( lRight );
                else
                    (aSubRect.*fnRect->fnSetLeft)( lLeft );
                Sub( aRegion, aSubRect );
            }
        }
        //case 3: (Different frames, maybe with ohther frames between
        else
        {
            //The startframe first...
            SwRect aSubRect( aStRect );
            if( bR2L )
                (aSubRect.*fnRect->fnSetLeft)( (aStFrm.*fnRect->fnGetLeft)());
            else
                (aSubRect.*fnRect->fnSetRight)( (aStFrm.*fnRect->fnGetRight)());
            Sub( aRegion, aSubRect );
            SwTwips nTmpTwips = (aStRect.*fnRect->fnGetBottom)();
            if( (aStFrm.*fnRect->fnGetBottom)() != nTmpTwips )
            {
                aSubRect = aStFrm;
                (aSubRect.*fnRect->fnSetTop)( nTmpTwips );
                Sub( aRegion, aSubRect );
            }

            //Now the frames between, if there are any
            BOOL bBody = pStartFrm->IsInDocBody();
            const SwTableBox* pCellBox = pStartFrm->GetUpper()->IsCellFrm() ?
                                         ((SwCellFrm*)pStartFrm->GetUpper())->GetTabBox() : 0;
            const SwCntntFrm *pCntnt = pStartFrm->GetNextCntntFrm();
            SwRect aPrvRect;

            // --> OD 2006-01-24 #123908# - introduce robust code:
            // The stacktrace issue reveals that <pCntnt> could be NULL.
            // One root cause found by AMA - see #130650#
            ASSERT( pCntnt,
                    "<SwRootFrm::CalcFrmRects(..)> - no content frame. This is a serious defect -> please inform OD" );
            while ( pCntnt && pCntnt != pEndFrm )
            // <--
            {
                if ( pCntnt->IsInFly() )
                {
                    const SwAnchoredObject* pObj = pCntnt->FindFlyFrm();
                    aSortObjs.Insert( *(const_cast<SwAnchoredObject*>(pObj)) );
                }

                // Consider only frames which have the same IsInDocBody value like pStartFrm
                // If pStartFrm is inside a SwCellFrm, consider only frames which are inside the
                // same cell frame (or its follow cell)
                const SwTableBox* pTmpCellBox = pCntnt->GetUpper()->IsCellFrm() ?
                                                ((SwCellFrm*)pCntnt->GetUpper())->GetTabBox() : 0;
                if ( bBody == pCntnt->IsInDocBody() &&
                    ( !pCellBox || pCellBox == pTmpCellBox ) )
                {
                    SwRect aCRect( pCntnt->UnionFrm( sal_True ) );
                    aCRect.Intersection( pCntnt->PaintArea() );
                    if( aCRect.IsOver( aRegion.GetOrigin() ))
                    {
                        SwRect aTmp( aPrvRect );
                        aTmp.Union( aCRect );
                        if ( (aPrvRect.Height() * aPrvRect.Width() +
                            aCRect.Height()   * aCRect.Width()) ==
                            (aTmp.Height() * aTmp.Width()) )
                        {
                            aPrvRect.Union( aCRect );
                        }
                        else
                        {
                            if ( aPrvRect.HasArea() )
                                Sub( aRegion, aPrvRect );
                            aPrvRect = aCRect;
                        }
                    }
                }
                pCntnt = pCntnt->GetNextCntntFrm();
                // --> OD 2006-01-24 #123908#
                ASSERT( pCntnt,
                        "<SwRootFrm::CalcFrmRects(..)> - no content frame. This is a serious defect -> please inform OD" );
                // <--
            }
            if ( aPrvRect.HasArea() )
                Sub( aRegion, aPrvRect );

            //At least the endframe...
            bVert = pEndFrm->IsVertical();
            bRev = pEndFrm->IsReverse();
            fnRect = bVert ? ( bRev ? fnRectVL2R : fnRectVert ) :
                             ( bRev ? fnRectB2T : fnRectHori );
            nTmpTwips = (aEndRect.*fnRect->fnGetTop)();
            if( (aEndFrm.*fnRect->fnGetTop)() != nTmpTwips )
            {
                aSubRect = aEndFrm;
                (aSubRect.*fnRect->fnSetBottom)( nTmpTwips );
                Sub( aRegion, aSubRect );
            }
            aSubRect = aEndRect;
            if( bEndR2L )
                (aSubRect.*fnRect->fnSetRight)((aEndFrm.*fnRect->fnGetRight)());
            else
                (aSubRect.*fnRect->fnSetLeft)( (aEndFrm.*fnRect->fnGetLeft)() );
            Sub( aRegion, aSubRect );
        }

//		aRegion.Compress( FALSE );
        aRegion.Invert();
        delete pSt2Pos;
        delete pEnd2Pos;
    }

    //Flys mit Durchlauf ausstanzen. Nicht ausgestanzt werden Flys:
    //- die Lower des StartFrm/EndFrm sind (FlyInCnt und alle Flys die wiederum
    //	darin sitzen)
    //- in der Z-Order ueber denjenigen Flys stehen in denen sich der StartFrm
    //	befindet.
    const SwPageFrm *pPage		= pStartFrm->FindPageFrm();
    const SwPageFrm *pEndPage	= pEndFrm->FindPageFrm();

    while ( pPage )
    {
        if ( pPage->GetSortedObjs() )
        {
            const SwSortedObjs &rObjs = *pPage->GetSortedObjs();
            for ( USHORT i = 0; i < rObjs.Count(); ++i )
            {
                SwAnchoredObject* pAnchoredObj = rObjs[i];
                if ( !pAnchoredObj->ISA(SwFlyFrm) )
                    continue;
                const SwFlyFrm* pFly = static_cast<const SwFlyFrm*>(pAnchoredObj);
                const SwVirtFlyDrawObj* pObj = pFly->GetVirtDrawObj();
                const SwFmtSurround &rSur = pFly->GetFmt()->GetSurround();
                if ( !pFly->IsAnLower( pStartFrm ) &&
                     (rSur.GetSurround() != SURROUND_THROUGHT &&
                      !rSur.IsContour()) )
                {
                    if ( aSortObjs.Contains( *pAnchoredObj ) )
                        continue;

                    BOOL bSub = TRUE;
                    const UINT32 nPos = pObj->GetOrdNum();
                    for ( USHORT k = 0; bSub && k < aSortObjs.Count(); ++k )
                    {
                        ASSERT( aSortObjs[k]->ISA(SwFlyFrm),
                                "<SwRootFrm::CalcFrmRects(..)> - object in <aSortObjs> of unexcepted type" );
                        const SwFlyFrm* pTmp = static_cast<SwFlyFrm*>(aSortObjs[k]);
                        do
                        {	if ( nPos < pTmp->GetVirtDrawObj()->GetOrdNumDirect() )
                                bSub = FALSE;
                            else
                                pTmp = pTmp->GetAnchorFrm()->FindFlyFrm();
                        } while ( bSub && pTmp );
                    }
                    if ( bSub )
                        Sub( aRegion, pFly->Frm() );
                }
            }
        }
        if ( pPage == pEndPage )
            break;
        else
            pPage = (SwPageFrm*)pPage->GetNext();
    }

    //Weil's besser aussieht noch die DropCaps ausschliessen.
    SwRect aDropRect;
    if ( pStartFrm->IsTxtFrm() )
    {
        if ( ((SwTxtFrm*)pStartFrm)->GetDropRect( aDropRect ) )
            Sub( aRegion, aDropRect );
    }
    if ( pEndFrm != pStartFrm && pEndFrm->IsTxtFrm() )
    {
        if ( ((SwTxtFrm*)pEndFrm)->GetDropRect( aDropRect ) )
            Sub( aRegion, aDropRect );
    }

    rCrsr.Remove( 0, rCrsr.Count() );
    rCrsr.Insert( &aRegion, 0 );
}


