/*************************************************************************
 *
 *  $RCSfile: crsrsh.cxx,v $
 *
 *  $Revision: 1.53 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-22 08:16:55 $
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
#ifndef _COM_SUN_STAR_UTIL_SEARCHOPTIONS_HPP_
#include <com/sun/star/util/SearchOptions.hpp>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVDMODEL_HXX //autogen
#include <svx/svdmodel.hxx>
#endif

#ifdef BIDI
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
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
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>			// fuer ASSERT
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif
#ifndef _DVIEW_HXX
#include <dview.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif
#ifndef _CRSRSH_HXX
#include <crsrsh.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
// --> FME 2004-06-22 #114856# edit in readonly sections
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
// <--
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _CALLNK_HXX
#include <callnk.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SCRIPTINFO_HXX
#include <scriptinfo.hxx>
#endif
#ifndef _SWGLOBDOCSH_HXX //autogen
#include <globdoc.hxx>
#endif
#ifndef _PAMTYP_HXX
#include <pamtyp.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>			// ...Percent()
#endif
#ifndef _FMTEIRO_HXX //autogen
#include <fmteiro.hxx>
#endif

#include <globals.hrc>

#include <comcore.hrc>

using namespace com::sun::star;
using namespace com::sun::star::util;

TYPEINIT2(SwCrsrShell,ViewShell,SwModify);


// Funktion loescht, alle ueberlappenden Cursor aus einem Cursor-Ring
void CheckRange( SwCursor* );

//-----------------------------------------------------------------------

/*
 * Ueberpruefe ob der pCurCrsr in einen schon bestehenden Bereich zeigt.
 * Wenn ja, dann hebe den alten Bereich auf.
 */


void CheckRange( SwCursor* pCurCrsr )
{
    const SwPosition *pStt = pCurCrsr->Start(),
        *pEnd = pCurCrsr->GetPoint() == pStt ? pCurCrsr->GetMark() : pCurCrsr->GetPoint();

    SwPaM *pTmpDel = 0,
          *pTmp = (SwPaM*)pCurCrsr->GetNext();

    // durchsuche den gesamten Ring
    while( pTmp != pCurCrsr )
    {
        const SwPosition *pTmpStt = pTmp->Start(),
                        *pTmpEnd = pTmp->GetPoint() == pTmpStt ?
                                        pTmp->GetMark() : pTmp->GetPoint();
        if( *pStt <= *pTmpStt )
        {
            if( *pEnd > *pTmpStt ||
                ( *pEnd == *pTmpStt && *pEnd == *pTmpEnd ))
                pTmpDel = pTmp;
        }
        else
            if( *pStt < *pTmpEnd )
                pTmpDel = pTmp;
        /*
         * liegt ein SPoint oder GetMark innerhalb vom Crsr-Bereich
         * muss der alte Bereich aufgehoben werden.
         * Beim Vergleich ist darauf zu achten, das SPoint nicht mehr zum
         * Bereich gehoert !
         */
        pTmp = (SwPaM*)pTmp->GetNext();
        if( pTmpDel )
        {
            delete pTmpDel;         // hebe alten Bereich auf
            pTmpDel = 0;
        }
    }
}

// -------------- Methoden von der SwCrsrShell -------------

SwPaM * SwCrsrShell::CreateCrsr()
{
    // Innerhalb der Tabellen-SSelection keinen neuen Crsr anlegen
    ASSERT( !IsTableMode(), "in Tabellen SSelection" );

    // neuen Cursor als Kopie vom akt. und in den Ring aufnehmen
    // Verkettung zeigt immer auf den zuerst erzeugten, also vorwaerts
    SwShellCrsr* pNew = new SwShellCrsr( *pCurCrsr );

    // hier den akt. Pam nur logisch Hiden, weil sonst die Invertierung
    // vom kopierten Pam aufgehoben wird !!
    pNew->Insert( pCurCrsr, 0 );
    pCurCrsr->Remove( 0, pCurCrsr->Count() );

    pCurCrsr->DeleteMark();

    UpdateCrsr( SwCrsrShell::SCROLLWIN );
//	return pCurCrsr;
    return pNew;
}

// loesche den aktuellen Cursor und der folgende wird zum Aktuellen


FASTBOOL SwCrsrShell::DestroyCrsr()
{
    // Innerhalb der Tabellen-SSelection keinen neuen Crsr loeschen
    ASSERT( !IsTableMode(), "in Tabellen SSelection" );

    // ist ueberhaupt ein naechtser vorhanden ?
    if(pCurCrsr->GetNext() == pCurCrsr)
        return FALSE;

    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen,
    SwCursor* pNext = (SwCursor*)pCurCrsr->GetNext();
    delete pCurCrsr;
    pCurCrsr = (SwShellCrsr*)*pNext;
    UpdateCrsr();
    return TRUE;
}


// gebe den aktuellen zurueck

SwPaM* SwCrsrShell::GetCrsr( FASTBOOL bMakeTblCrsr ) const
{
    if( pTblCrsr )
    {
        if( bMakeTblCrsr && pTblCrsr->IsCrsrMovedUpdt() )
        {
            // geparkte Cursor werden nicht wieder erzeugt
            const SwCntntNode* pCNd;
            if( pTblCrsr->GetPoint()->nNode.GetIndex() &&
                pTblCrsr->GetMark()->nNode.GetIndex() &&
                0 != ( pCNd = pTblCrsr->GetCntntNode() ) && pCNd->GetFrm() &&
                0 != ( pCNd = pTblCrsr->GetCntntNode(FALSE) ) && pCNd->GetFrm())
            {
                SwShellTableCrsr* pTC = (SwShellTableCrsr*)pTblCrsr;
                GetLayout()->MakeTblCrsrs( *pTC );
            }
        }

        if( pTblCrsr->IsChgd() )
        {
            SwCrsrShell* pThis = (SwCrsrShell*)this;
            pThis->pCurCrsr = (SwShellCrsr*)
                            *pTblCrsr->MakeBoxSels( pThis->pCurCrsr );
        }
    }
    return pCurCrsr;
}


void SwCrsrShell::StartAction()
{
    if( !ActionPend() )
    {
        // fuer das Update des Ribbon-Bars merken
        const SwNode& rNd = pCurCrsr->GetPoint()->nNode.GetNode();
        nAktNode = rNd.GetIndex();
        nAktCntnt = pCurCrsr->GetPoint()->nContent.GetIndex();
        nAktNdTyp = rNd.GetNodeType();
        bAktSelection = *pCurCrsr->GetPoint() != *pCurCrsr->GetMark();
        if( ND_TEXTNODE & nAktNdTyp )
            nLeftFrmPos = SwCallLink::GetFrm( (SwTxtNode&)rNd, nAktCntnt, TRUE );
        else
            nLeftFrmPos = 0;
    }
    ViewShell::StartAction();           // zur ViewShell
}


void SwCrsrShell::EndAction( const BOOL bIdleEnd )
{
/*
//OS: Wird z.B. eine Basic-Action im Hintergrund ausgefuehrt, geht es so nicht
    if( !bHasFocus )
    {
        // hat die Shell nicht den Focus, dann nur das EndAction an
        // die ViewShell weitergeben.
        ViewShell::EndAction( bIdleEnd );
        return;
    }
*/

    FASTBOOL bVis = bSVCrsrVis;

    // Idle-Formatierung ?
    if( bIdleEnd && Imp()->GetRegion() )
    {
        pCurCrsr->Hide();

#ifdef SHOW_IDLE_REGION
if( GetWin() )
{
    GetWin()->Push();
    GetWin()->ChangePen( Pen( Color( COL_YELLOW )));
    for( USHORT n = 0; n < aPntReg.Count(); ++n )
    {
        SwRect aIRect( aPntReg[n] );
        GetWin()->DrawRect( aIRect.SVRect() );
    }
    GetWin()->Pop();
}
#endif

    }

    // vor der letzten Action alle invaliden Numerierungen updaten
    if( 1 == nStartAction )
        GetDoc()->UpdateNumRule();

    // Task: 76923: dont show the cursor in the ViewShell::EndAction() - call.
    //				Only the UpdateCrsr shows the cursor.
    BOOL bSavSVCrsrVis = bSVCrsrVis;
    bSVCrsrVis = FALSE;

    ViewShell::EndAction( bIdleEnd );	//der ViewShell den Vortritt lassen

    bSVCrsrVis = bSavSVCrsrVis;

    if( ActionPend() )
    {
        if( bVis )    // auch SV-Cursor wieder anzeigen
            pVisCrsr->Show();

        // falls noch ein ChgCall vorhanden ist und nur noch die Basic
        // Klammerung vorhanden ist, dann rufe ihn. Dadurch wird die interne
        // mit der Basic-Klammerung entkoppelt; die Shells werden umgeschaltet
        if( !BasicActionPend() )
        {
            //JP 12.01.98: Bug #46496# - es muss innerhalb einer BasicAction
            //				der Cursor geupdatet werden; um z.B. den
            //				TabellenCursor zu erzeugen. Im UpdateCrsr wird
            //				das jetzt beruecksichtigt!
            UpdateCrsr( SwCrsrShell::CHKRANGE, bIdleEnd );

            {
                // Crsr-Moves ueberwachen, evt. Link callen
                // der DTOR ist das interressante!!
                SwCallLink aLk( *this, nAktNode, nAktCntnt, (BYTE)nAktNdTyp,
                                nLeftFrmPos, bAktSelection );

            }
            if( bCallChgLnk && bChgCallFlag && aChgLnk.IsSet() )
            {
                aChgLnk.Call( this );
                bChgCallFlag = FALSE;		// Flag zuruecksetzen
            }
        }
        return;
    }

    USHORT nParm = SwCrsrShell::CHKRANGE;
    if ( !bIdleEnd )
        nParm |= SwCrsrShell::SCROLLWIN;
    UpdateCrsr( nParm, bIdleEnd );		// Cursor-Aenderungen anzeigen

    {
        SwCallLink aLk( *this );        // Crsr-Moves ueberwachen,
        aLk.nNode = nAktNode;           // evt. Link callen
        aLk.nNdTyp = (BYTE)nAktNdTyp;
        aLk.nCntnt = nAktCntnt;
        aLk.nLeftFrmPos = nLeftFrmPos;

        if( !nCrsrMove ||
            ( 1 == nCrsrMove && bInCMvVisportChgd ) )
            ShowCrsrs( bSVCrsrVis ? TRUE : FALSE );    // Cursor & Selektionen wieder anzeigen
    }
    // falls noch ein ChgCall vorhanden ist, dann rufe ihn
    if( bCallChgLnk && bChgCallFlag && aChgLnk.IsSet() )
    {
        aChgLnk.Call( this );
        bChgCallFlag = FALSE;		// Flag zuruecksetzen
    }
}


#if !defined( PRODUCT )

void SwCrsrShell::SttCrsrMove()
{
    ASSERT( nCrsrMove < USHRT_MAX, "To many nested CrsrMoves." );
    ++nCrsrMove;
    StartAction();
}

void SwCrsrShell::EndCrsrMove( const BOOL bIdleEnd )
{
    ASSERT( nCrsrMove, "EndCrsrMove() ohne SttCrsrMove()." );
    EndAction( bIdleEnd );
    if( !--nCrsrMove )
        bInCMvVisportChgd = FALSE;
}

#endif


FASTBOOL SwCrsrShell::LeftRight( BOOL bLeft, USHORT nCnt, USHORT nMode,
                                 BOOL bVisualAllowed )
{
    if( IsTableMode() )
        return bLeft ? GoPrevCell() : GoNextCell();

    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    FASTBOOL bRet = FALSE;

    // #i27615# Handle cursor in front of label.
    const SwTxtNode* pTxtNd = 0;

    //
    // 1. CASE: Cursor is in front of label. A move to the right
    // will simply reset the bInFrontOfLabel flag:
    //
    if ( !bLeft && _GetCrsr()->IsInFrontOfLabel() )
    {
        SetInFrontOfLabel( FALSE );
        bRet = TRUE;
    }
    //
    // 2. CASE: Cursor is at beginning of numbered paragraph. A move
    // to the left will simply set the bInFrontOfLabel flag:
    //
    else if ( bLeft && 0 == _GetCrsr()->GetPoint()->nContent.GetIndex() &&
             !_GetCrsr()->IsInFrontOfLabel() && !_GetCrsr()->HasMark() &&
             0 != ( pTxtNd = _GetCrsr()->GetNode()->GetTxtNode() ) &&
             pTxtNd->HasVisibleNumberingOrBullet() )
    {
        SetInFrontOfLabel( TRUE );
        bRet = TRUE;
    }
    //
    // 3. CASE: Regular cursor move. Reset the bInFrontOfLabel flag:
    //
    else
    {
        const BOOL bSkipHidden = !GetViewOptions()->IsShowHiddenChar();
        bRet = SetInFrontOfLabel( FALSE );
        bRet = pCurCrsr->LeftRight( bLeft, nCnt, nMode, bVisualAllowed,
                                    bSkipHidden,
                                   !IsOverwriteCrsr() ) || bRet;
    }

    if( bRet )
    {
        UpdateCrsr();
    }
    return bRet;
}

// -> #i27615#
void SwCrsrShell::SetMarkedNumLevel(const String & sNumRule, BYTE nLevel)
{
    if (sNumRule != sMarkedNumRule || nLevel != nMarkedNumLevel)
    {
        if (sMarkedNumRule.Len() > 0)
            pDoc->SetMarkedNumLevel(sMarkedNumRule, nMarkedNumLevel, FALSE);

        if (sNumRule.Len() > 0)
        {
            pDoc->SetMarkedNumLevel(sNumRule, nLevel, TRUE);
        }

        sMarkedNumRule = sNumRule;
        nMarkedNumLevel = nLevel;
    }
}

void SwCrsrShell::UpdateMarkedNumLevel()
{
    SwTxtNode * pTxtNd = _GetCrsr()->GetNode()->GetTxtNode();

    if ( pTxtNd ) // #i27615#
    {
        if (! pTxtNd->IsNumbered())
        {
            pCurCrsr->_SetInFrontOfLabel( FALSE );
            SetMarkedNumLevel(String(), 0);
        }
        else if (pCurCrsr->IsInFrontOfLabel())
        {
            SwNumRule * pNumRule = pTxtNd->GetNumRule();

            if (pNumRule)
            {
                const SwNodeNum * pNdNum = pTxtNd->GetNum();
                SetMarkedNumLevel(pNumRule->GetName(),
                                  pNdNum->GetRealLevel());
            }
        }
        else
        {
            SetMarkedNumLevel(String(), 0);
        }
    }
}
// <- #i27615#

FASTBOOL SwCrsrShell::UpDown( BOOL bUp, USHORT nCnt )
{
    SET_CURR_SHELL( this );
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen

    BOOL bTableMode = IsTableMode();
    SwShellCrsr* pTmpCrsr = bTableMode ? pTblCrsr : pCurCrsr;

    FASTBOOL bRet = pTmpCrsr->UpDown( bUp, nCnt );
    // --> FME 2005-01-10 #i40019# UpDown should always reset the
    // bInFrontOfLabel flag:
    bRet = SetInFrontOfLabel(FALSE) || bRet;
    // <--

    if( bRet )
    {
        eMvState = MV_UPDOWN;		// Status fuers Crsr-Travelling - GetCrsrOfst
        if( !ActionPend() )
        {
            CrsrFlag eUpdtMode = SwCrsrShell::SCROLLWIN;
            if( !bTableMode )
                eUpdtMode = (CrsrFlag) (eUpdtMode
                            | SwCrsrShell::UPDOWN | SwCrsrShell::CHKRANGE);
            UpdateCrsr( eUpdtMode );
        }
    }
    return bRet;
}


FASTBOOL SwCrsrShell::LRMargin( BOOL bLeft, BOOL bAPI)
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    SET_CURR_SHELL( this );
    eMvState = MV_LEFTMARGIN;		// Status fuers Crsr-Travelling - GetCrsrOfst

    const BOOL bTableMode = IsTableMode();
    SwShellCrsr* pTmpCrsr = bTableMode ? pTblCrsr : pCurCrsr;

    const FASTBOOL bWasAtLM =
            ( 0 == _GetCrsr()->GetPoint()->nContent.GetIndex() );

    FASTBOOL bRet = pTmpCrsr->LeftRightMargin( bLeft, bAPI );

    if ( bLeft && !bTableMode && bRet && bWasAtLM && !_GetCrsr()->HasMark() )
    {
        const SwTxtNode * pTxtNd = _GetCrsr()->GetNode()->GetTxtNode();
        if ( pTxtNd && pTxtNd->HasVisibleNumberingOrBullet() )
            SetInFrontOfLabel( TRUE );
    }
    else if ( !bLeft )
    {
        bRet = SetInFrontOfLabel( FALSE ) || bRet;
    }

    if( bRet )
    {
        UpdateCrsr();
    }
    return bRet;
}

FASTBOOL SwCrsrShell::IsAtLRMargin( BOOL bLeft, BOOL bAPI ) const
{
    SwShellCrsr* pTmpCrsr = IsTableMode() ? pTblCrsr : pCurCrsr;
    return pTmpCrsr->IsAtLeftRightMargin( bLeft, bAPI );
}


FASTBOOL SwCrsrShell::SttEndDoc( BOOL bStt )
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen

    FASTBOOL bRet = pCurCrsr->SttEndDoc( bStt );
    if( bRet )
    {
        if( bStt )
            pCurCrsr->GetPtPos().Y() = 0;		// expl. 0 setzen (TabellenHeader)

        UpdateCrsr(SwCrsrShell::SCROLLWIN|SwCrsrShell::CHKRANGE|SwCrsrShell::READONLY);
    }
    return bRet;
}



FASTBOOL SwCrsrShell::MovePage( SwWhichPage fnWhichPage, SwPosPage fnPosPage )
{
    FASTBOOL bRet = FALSE;

    // Springe beim Selektieren nie ueber Section-Grenzen !!
    if( !pCurCrsr->HasMark() || !pCurCrsr->IsNoCntnt() )
    {
        SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
        SET_CURR_SHELL( this );

        SwCrsrSaveState aSaveState( *pCurCrsr );
        Point& rPt = pCurCrsr->GetPtPos();
        SwCntntFrm * pFrm = pCurCrsr->GetCntntNode()->
                            GetFrm( &rPt, pCurCrsr->GetPoint() );
        if( pFrm && TRUE == ( bRet = GetFrmInPage( pFrm, fnWhichPage,
                                                fnPosPage, pCurCrsr )  ) &&
            !pCurCrsr->IsSelOvr( SELOVER_TOGGLE | SELOVER_CHANGEPOS ))
            UpdateCrsr();
        else
            bRet = FALSE;
    }
    return bRet;
}


FASTBOOL SwCrsrShell::MovePara(SwWhichPara fnWhichPara, SwPosPara fnPosPara )
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    FASTBOOL bRet = pCurCrsr->MovePara( fnWhichPara, fnPosPara );
    if( bRet )
        UpdateCrsr();
    return bRet;
}


FASTBOOL SwCrsrShell::MoveSection( SwWhichSection fnWhichSect,
                                SwPosSection fnPosSect)
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    FASTBOOL bRet = pCurCrsr->MoveSection( fnWhichSect, fnPosSect );
    if( bRet )
        UpdateCrsr();
    return bRet;

}


// Positionieren des Cursors


SwFrm* lcl_IsInHeaderFooter( const SwNodeIndex& rIdx, Point& rPt )
{
    SwFrm* pFrm = 0;
    SwCntntNode* pCNd = rIdx.GetNode().GetCntntNode();
    if( pCNd )
    {
        pFrm = pCNd->GetFrm( &rPt, 0, FALSE )->GetUpper();
        while( pFrm && !pFrm->IsHeaderFrm() && !pFrm->IsFooterFrm() )
            pFrm = pFrm->IsFlyFrm() ? ((SwFlyFrm*)pFrm)->AnchorFrm()
                                    : pFrm->GetUpper();
    }
    return pFrm;
}

FASTBOOL SwCrsrShell::IsInHeaderFooter( FASTBOOL* pbInHeader ) const
{
    Point aPt;
    SwFrm* pFrm = ::lcl_IsInHeaderFooter( pCurCrsr->GetPoint()->nNode, aPt );
    if( pFrm && pbInHeader )
        *pbInHeader = pFrm->IsHeaderFrm();
    return 0 != pFrm;
}

int SwCrsrShell::SetCrsr( const Point &rLPt, BOOL bOnlyText )
{
    SET_CURR_SHELL( this );

    SwShellCrsr* pCrsr = IsTableMode() ? pTblCrsr : pCurCrsr;
    SwPosition aPos( *pCrsr->GetPoint() );
    Point aPt( rLPt );
    Point & rAktCrsrPt = pCrsr->GetPtPos();
    SwCrsrMoveState aTmpState( IsTableMode() ? MV_TBLSEL :
                                    bOnlyText ?  MV_SETONLYTEXT : MV_NONE );
    aTmpState.bSetInReadOnly = IsReadOnlyAvailable();

    SwTxtNode * pTxtNd = pCrsr->GetNode()->GetTxtNode();

    if ( pTxtNd && !IsTableMode() &&
        // --> FME 2004-11-25 #i37515# No bInFrontOfLabel during selection
        !pCrsr->HasMark() &&
        // <--
        pTxtNd->HasVisibleNumberingOrBullet() )
    {
        aTmpState.bInFrontOfLabel = TRUE; // #i27615#
    }
    else
    {
        aTmpState.bInFrontOfLabel = FALSE;
    }

    int bRet = CRSR_POSOLD |
                ( GetLayout()->GetCrsrOfst( &aPos, aPt, &aTmpState )
                    ? 0 : CRSR_POSCHG );

    const bool bOldInFrontOfLabel = IsInFrontOfLabel();
    const bool bNewInFrontOfLabel = aTmpState.bInFrontOfLabel;

    pCrsr->SetCrsrBidiLevel( aTmpState.nCursorBidiLevel );

    if( MV_RIGHTMARGIN == aTmpState.eState )
        eMvState = MV_RIGHTMARGIN;
    // steht neu Pos im Header/Footer ?
    SwFrm* pFrm = lcl_IsInHeaderFooter( aPos.nNode, aPt );
    if(	IsTableMode() && !pFrm && aPos.nNode.GetNode().FindStartNode() ==
        pCrsr->GetPoint()->nNode.GetNode().FindStartNode() )
        // gleiche Tabellenzelle und nicht im Header/Footer
        // -> zurueck
        return bRet;

    if( !pCrsr->HasMark() )
    {
        // steht an der gleichen Position und wenn im Header/Footer,
        // dann im gleichen
        if( aPos == *pCrsr->GetPoint() &&
            bOldInFrontOfLabel == bNewInFrontOfLabel )
        {
            if( pFrm )
            {
                if( pFrm->Frm().IsInside( rAktCrsrPt ))
                    return bRet;
            }
            else if( aPos.nNode.GetNode().IsCntntNode() )
            {
                // im gleichen Frame gelandet?
                SwFrm* pOld = ((SwCntntNode&)aPos.nNode.GetNode()).GetFrm(
                                        &aCharRect.Pos(), 0, FALSE );
                SwFrm* pNew = ((SwCntntNode&)aPos.nNode.GetNode()).GetFrm(
                                        &aPt, 0, FALSE );
                if( pNew == pOld )
                    return bRet;
            }
        }
    }
    else
    {
        // SSelection ueber nicht erlaubte Sections oder wenn im Header/Footer
        // dann in verschiedene
        if( !CheckNodesRange( aPos.nNode, pCrsr->GetMark()->nNode, TRUE )
            || ( pFrm && !pFrm->Frm().IsInside( pCrsr->GetMkPos() ) ))
            return bRet;

        // steht an der gleichen Position und nicht im Header/Footer
        if( aPos == *pCrsr->GetPoint() )
            return bRet;
    }

    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    SwCrsrSaveState aSaveState( *pCrsr );

    *pCrsr->GetPoint() = aPos;
    rAktCrsrPt = aPt;

    // --> FME 2005-01-31 #i41424# Only update the marked number levels if necessary
    // Force update of marked number levels if necessary.
    if ( bNewInFrontOfLabel || bOldInFrontOfLabel )
        pCurCrsr->_SetInFrontOfLabel( !bNewInFrontOfLabel );
    SetInFrontOfLabel( bNewInFrontOfLabel );
    // <--

    if( !pCrsr->IsSelOvr( SELOVER_CHANGEPOS ) )
    {
        UpdateCrsr( SwCrsrShell::SCROLLWIN | SwCrsrShell::CHKRANGE);
        bRet &= ~CRSR_POSOLD;
    }
    else if( bOnlyText && !pCurCrsr->HasMark() )
    {
        if( FindValidCntntNode( bOnlyText ) )
        {
            // Cursor in einen gueltigen Content stellen
            if( aPos == *pCrsr->GetPoint() )
                bRet = CRSR_POSOLD;
            else
            {
                UpdateCrsr( SwCrsrShell::SCROLLWIN | SwCrsrShell::CHKRANGE );
                bRet &= ~CRSR_POSOLD;
            }
        }
        else
        {
            // es gibt keinen gueltigen Inhalt -> Cursor verstecken
            pVisCrsr->Hide();       // sichtbaren Cursor immer verstecken
            eMvState = MV_NONE;		// Status fuers Crsr-Travelling
            bAllProtect = TRUE;
            if( GetDoc()->GetDocShell() )
            {
                GetDoc()->GetDocShell()->SetReadOnlyUI( TRUE );
                CallChgLnk();			// UI bescheid sagen!
            }
        }
    }

    return bRet;
}


void SwCrsrShell::TblCrsrToCursor()
{
    ASSERT( pTblCrsr, "TblCrsrToCursor: Why?" );
    delete pTblCrsr, pTblCrsr = 0;
}


void SwCrsrShell::ClearMark()
{
    // ist ueberhaupt ein GetMark gesetzt ?
    if( pTblCrsr )
    {
        while( pCurCrsr->GetNext() != pCurCrsr )
            delete pCurCrsr->GetNext();
        pTblCrsr->DeleteMark();

        if( pCurCrsr->HasMark() )
        {
            // falls doch nicht alle Indizies richtig verschoben werden
            //	(z.B.: Kopf-/Fusszeile loeschen) den Content-Anteil vom
            //	Mark aufs Nodes-Array setzen
            SwPosition& rPos = *pCurCrsr->GetMark();
            rPos.nNode.Assign( pDoc->GetNodes(), 0 );
            rPos.nContent.Assign( 0, 0 );
            pCurCrsr->DeleteMark();
        }

        *pCurCrsr->GetPoint() = *pTblCrsr->GetPoint();
        pCurCrsr->GetPtPos() = pTblCrsr->GetPtPos();
        delete pTblCrsr, pTblCrsr = 0;
        pCurCrsr->SwSelPaintRects::Show();
    }
    else
    {
        if( !pCurCrsr->HasMark() )
            return;
        // falls doch nicht alle Indizies richtig verschoben werden
        //	(z.B.: Kopf-/Fusszeile loeschen) den Content-Anteil vom
        //	Mark aufs Nodes-Array setzen
        SwPosition& rPos = *pCurCrsr->GetMark();
        rPos.nNode.Assign( pDoc->GetNodes(), 0 );
        rPos.nContent.Assign( 0, 0 );
        pCurCrsr->DeleteMark();
        if( !nCrsrMove )
            pCurCrsr->SwSelPaintRects::Show();
    }
}


void SwCrsrShell::NormalizePam(BOOL bPointFirst)
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    pCurCrsr->Normalize(bPointFirst);
}

void SwCrsrShell::SwapPam()
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    pCurCrsr->Exchange();
}


// suche innerhalb der Selektierten-Bereiche nach einer Selektion, die
// den angebenen SPoint umschliesst
// Ist das Flag bTstOnly gesetzt, dann wird nur getestet, ob dort eine
// SSelection besteht; des akt. Cursr wird nicht umgesetzt!
// Ansonsten wird er auf die gewaehlte SSelection gesetzt.


FASTBOOL SwCrsrShell::ChgCurrPam( const Point & rPt,
                              BOOL bTstOnly, BOOL bTstHit )
{
    SET_CURR_SHELL( this );

    // Pruefe ob der SPoint in einer Tabellen-Selektion liegt
    if( bTstOnly && pTblCrsr )
        return pTblCrsr->IsInside( rPt );

    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    // Suche die Position rPt im Dokument
    SwPosition aPtPos( *pCurCrsr->GetPoint() );
    Point aPt( rPt );

    SwCrsrMoveState aTmpState( MV_NONE );
    aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
    if ( !GetLayout()->GetCrsrOfst( &aPtPos, aPt, &aTmpState ) && bTstHit )
        return FALSE;

    // suche in allen Selektionen nach dieser Position
    SwShellCrsr* pCmp = (SwShellCrsr*)pCurCrsr;        // sicher den Pointer auf Cursor
    do {
        if( pCmp->HasMark() &&
            *pCmp->Start() <= aPtPos && *pCmp->End() > aPtPos )
        {
            if( bTstOnly || pCurCrsr == pCmp )	   // ist der aktuelle.
                return TRUE;         			   // return ohne Update

            pCurCrsr = pCmp;
            UpdateCrsr();     // Cursor steht schon richtig
            return TRUE;
        }
    } while( pCurCrsr != ( pCmp = (SwShellCrsr*)*((SwCursor*)pCmp->GetNext()) ) );
    return FALSE;
}


void SwCrsrShell::KillPams()
{
    // keiner zum loeschen vorhanden?
    if( !pTblCrsr && pCurCrsr->GetNext() == pCurCrsr )
        return;

    while( pCurCrsr->GetNext() != pCurCrsr )
        delete pCurCrsr->GetNext();

    if( pTblCrsr )
    {
        // Cursor Ring loeschen
        pCurCrsr->DeleteMark();
        *pCurCrsr->GetPoint() = *pTblCrsr->GetPoint();
        pCurCrsr->GetPtPos() = pTblCrsr->GetPtPos();
        delete pTblCrsr;
        pTblCrsr = 0;
    }
    UpdateCrsr( SwCrsrShell::SCROLLWIN );
}


int SwCrsrShell::CompareCursor( CrsrCompareType eType ) const
{
    int nRet = 0;
    const SwPosition *pFirst = 0, *pSecond = 0;
    const SwPaM *pCur = GetCrsr(), *pStk = pCrsrStk;
    if( CurrPtCurrMk != eType && pStk )
    {
        switch ( eType)
        {
        case StackPtStackMk:
            pFirst = pStk->GetPoint();
            pSecond = pStk->GetMark();
            break;
        case StackPtCurrPt:
            pFirst = pStk->GetPoint();
            pSecond = pCur->GetPoint();
            break;
        case StackPtCurrMk:
            pFirst = pStk->GetPoint();
            pSecond = pCur->GetMark();
            break;
        case StackMkCurrPt:
            pFirst = pStk->GetMark();
            pSecond = pCur->GetPoint();
            break;
        case StackMkCurrMk:
            pFirst = pStk->GetMark();
            pSecond = pStk->GetMark();
            break;
        case CurrPtCurrMk:
            pFirst = pCur->GetPoint();
            pSecond = pCur->GetMark();
            break;
        }
    }
    if( !pFirst || !pSecond )
        nRet = INT_MAX;
    else if( *pFirst < *pSecond )
        nRet = -1;
    else if( *pFirst == *pSecond )
        nRet = 0;
    else
        nRet = 1;
    return nRet;
}


FASTBOOL SwCrsrShell::IsSttPara() const
{   return( pCurCrsr->GetPoint()->nContent == 0 ? TRUE : FALSE ); }


FASTBOOL SwCrsrShell::IsEndPara() const
{   return( pCurCrsr->GetPoint()->nContent == pCurCrsr->GetCntntNode()->Len() ? TRUE : FALSE ); }


FASTBOOL SwCrsrShell::IsInFrontOfLabel() const
{
    return pCurCrsr->IsInFrontOfLabel();
}

bool SwCrsrShell::SetInFrontOfLabel( FASTBOOL bNew )
{
    if ( bNew != IsInFrontOfLabel() )
    {
        pCurCrsr->_SetInFrontOfLabel( bNew );
        UpdateMarkedNumLevel();
        return true;
    }
    return false;
}

FASTBOOL SwCrsrShell::GotoPage( USHORT nPage )
{
    SET_CURR_SHELL( this );
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    SwCrsrSaveState aSaveState( *pCurCrsr );
    FASTBOOL bRet = GetLayout()->SetCurrPage( pCurCrsr, nPage ) &&
                    !pCurCrsr->IsSelOvr( SELOVER_TOGGLE | SELOVER_CHANGEPOS );
    if( bRet )
        UpdateCrsr(SwCrsrShell::SCROLLWIN|SwCrsrShell::CHKRANGE|SwCrsrShell::READONLY);
    return bRet;
}


void SwCrsrShell::GetPageNum( USHORT &rnPhyNum, USHORT &rnVirtNum,
                              BOOL bAtCrsrPos, const BOOL bCalcFrm )
{
    SET_CURR_SHELL( this );
    // Seitennummer: die erste sichtbare Seite oder die am Cursor
    const SwCntntFrm* pCFrm;
    const SwPageFrm *pPg = 0;

    if( !bAtCrsrPos || 0 == (pCFrm = GetCurrFrm( bCalcFrm )) ||
                       0 == (pPg   = pCFrm->FindPageFrm()) )
    {
        pPg = Imp()->GetFirstVisPage();
        while( pPg && pPg->IsEmptyPage() )
            pPg = (const SwPageFrm *)pPg->GetNext();
    }
    // Abfrage auf pPg muss fuer den Sonderfall Writerstart mit
    // standard.vor sein.
    rnPhyNum  = pPg? pPg->GetPhyPageNum() : 1;
    rnVirtNum = pPg? pPg->GetVirtPageNum() : 1;
}


USHORT SwCrsrShell::GetNextPrevPageNum( BOOL bNext )
{
    SET_CURR_SHELL( this );

    // Seitennummer: die erste sichtbare Seite oder die am Cursor
    const SwPageFrm *pPg = Imp()->GetFirstVisPage();
    if( pPg )
    {
        if( bNext )
        {
            // erstmal die sichtbare suchen !!
            while( pPg && pPg->IsEmptyPage() )
                pPg = (const SwPageFrm *)pPg->GetNext();
            while( pPg && 0 != (pPg = (const SwPageFrm *)pPg->GetNext() ) &&
                    pPg->IsEmptyPage() )
                ;
        }
        else
        {
            while( pPg && 0 != (pPg = (const SwPageFrm *)pPg->GetPrev() ) &&
                    pPg->IsEmptyPage() )
                ;
        }
    }
    // Abfrage auf pPg muss fuer den Sonderfall Writerstart mit
    // standard.vor sein.
    return pPg ? pPg->GetPhyPageNum() : USHRT_MAX;
}


USHORT SwCrsrShell::GetPageCnt()
{
    SET_CURR_SHELL( this );
    // gebe die Anzahl der Seiten zurueck
    return GetLayout()->GetPageNum();
}

// Gehe zur naechsten SSelection


FASTBOOL SwCrsrShell::GoNextCrsr()
{
    // besteht ueberhaupt ein Ring ?
    if( pCurCrsr->GetNext() == pCurCrsr )
        return FALSE;

    SET_CURR_SHELL( this );
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    pCurCrsr = (SwShellCrsr*)*((SwCursor*)pCurCrsr->GetNext());

    // Bug 24086: auch alle anderen anzeigen
    if( !ActionPend() )
    {
        UpdateCrsr();
        pCurCrsr->Show();
    }
    return TRUE;
}

// gehe zur vorherigen SSelection


FASTBOOL SwCrsrShell::GoPrevCrsr()
{
    // besteht ueberhaupt ein Ring ?
    if( pCurCrsr->GetNext() == pCurCrsr )
        return FALSE;

    SET_CURR_SHELL( this );
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    pCurCrsr = (SwShellCrsr*)*((SwCursor*)pCurCrsr->GetPrev());

    // Bug 24086: auch alle anderen anzeigen
    if( !ActionPend() )
    {
        UpdateCrsr();
        pCurCrsr->Show();
    }

    return TRUE;
}


void SwCrsrShell::Paint( const Rectangle &rRect)
{
    SET_CURR_SHELL( this );

    // beim Painten immer alle Cursor ausschalten
    SwRect aRect( rRect );

    FASTBOOL bVis = FALSE;
    // ist Cursor sichtbar, dann verstecke den SV-Cursor
    if( pVisCrsr->IsVisible() && !aRect.IsOver( aCharRect ) )	//JP 18.06.97: ???
    {
        bVis = TRUE;
        pVisCrsr->Hide();
    }

    ViewShell::Paint( rRect );      // Bereich neu painten
    if( bHasFocus && !bBasicHideCrsr )
    {
        SwShellCrsr* pAktCrsr = pTblCrsr ? pTblCrsr : pCurCrsr;
//		pAktCrsr->Invalidate( aRect );
        if( !ActionPend() )
        {
            // damit nicht rechts/unten die Raender abgeschnitten werden
            pAktCrsr->Invalidate( VisArea() );
            pAktCrsr->Show();
        }
        else
            pAktCrsr->Invalidate( aRect );

    }
    if( bSVCrsrVis && bVis )        // auch SV-Cursor wieder anzeigen
        pVisCrsr->Show();
}



void SwCrsrShell::VisPortChgd( const SwRect & rRect )
{
    SET_CURR_SHELL( this );
    FASTBOOL bVis;      // beim Scrollen immer alle Cursor ausschalten

    // ist Cursor sichtbar, dann verstecke den SV-Cursor
    if( TRUE == ( bVis = pVisCrsr->IsVisible() ))
        pVisCrsr->Hide();

    bVisPortChgd = TRUE;
    aOldRBPos.X() = VisArea().Right();
    aOldRBPos.Y() = VisArea().Bottom();

    //Damit es es keine Probleme mit dem SV-Cursor gibt, wird in
    //ViewShell::VisPo.. ein Update() auf das Window gerufen.
    //Waehrend des Paintens duerfen aber nun wieder keine Selectionen
    //angezeigt werden, deshalb wird der Aufruf hier geklammert.
    ViewShell::VisPortChgd( rRect );        // Bereich verschieben

/*
    SwRect aRect( rRect );
    if( VisArea().IsOver( aRect ) )
        pCurCrsr->Invalidate( aRect );
*/

    if( bSVCrsrVis && bVis )    // auch SV-Cursor wieder anzeigen
        pVisCrsr->Show();

    if( nCrsrMove )
        bInCMvVisportChgd = TRUE;

    bVisPortChgd = FALSE;
}

// aktualisiere den Crsrs, d.H. setze ihn wieder in den Content.
// Das sollte nur aufgerufen werden, wenn der Cursor z.B. beim
// Loeschen von Rahmen irgendwohin gesetzt wurde. Die Position
// ergibt sich aus seiner aktuellen Position im Layout !!


void SwCrsrShell::UpdateCrsrPos()
{
    SET_CURR_SHELL( this );
    ++nStartAction;
    Size aOldSz( GetLayout()->Frm().SSize() );
    SwCntntNode *pCNode = pCurCrsr->GetCntntNode();
    SwCntntFrm  *pFrm = pCNode ?
        pCNode->GetFrm( &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint() ) :0;
    if( !pFrm || (pFrm->IsTxtFrm() && ((SwTxtFrm*)pFrm)->IsHiddenNow()) )
    {
        SwCrsrMoveState aTmpState( MV_NONE );
        aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
        GetLayout()->GetCrsrOfst( pCurCrsr->GetPoint(), pCurCrsr->GetPtPos(),
                                     &aTmpState );
        if( pCurCrsr->HasMark())
            pCurCrsr->DeleteMark();
    }
    --nStartAction;
    if( aOldSz != GetLayout()->Frm().SSize() )
        SizeChgNotify( GetLayout()->Frm().SSize() );
}

// JP 30.04.99: Bug 65475 - falls Point/Mark in versteckten Bereichen
//				stehen, so mussen diese daraus verschoben werden
static void lcl_CheckHiddenSection( SwNodeIndex& rIdx )
{
    const SwSectionNode* pSectNd = rIdx.GetNode().FindSectionNode();
    if( pSectNd && pSectNd->GetSection().IsHiddenFlag() )
    {
        SwNodeIndex aTmp( *pSectNd );
        const SwNode* pFrmNd = rIdx.GetNodes().FindPrvNxtFrmNode(
                                    aTmp, pSectNd->EndOfSectionNode() );
        ASSERT( pFrmNd, "keinen Node mit Frames gefunden" );
        rIdx = aTmp;
    }
}

// Try to set the cursor to the next visible content node.
static void lcl_CheckHiddenPara( SwPosition& rPos )
{
    SwNodeIndex aTmp( rPos.nNode );
    SwTxtNode* pTxtNd = aTmp.GetNode().GetTxtNode();
    while( pTxtNd && pTxtNd->HasHiddenCharAttribute( true ) )
    {
        SwCntntNode* pCntnt = aTmp.GetNodes().GoNext( &aTmp );
        if ( pCntnt && pCntnt->IsTxtNode() )
            pTxtNd = (SwTxtNode*)pCntnt;
        else
            pTxtNd = 0;
    }

    if ( pTxtNd )
        rPos = SwPosition( aTmp, SwIndex( pTxtNd, 0 ) );
}

void SwCrsrShell::UpdateCrsr( USHORT eFlags, BOOL bIdleEnd )
{
    SET_CURR_SHELL( this );

    ClearUpCrsrs();

    // erfrage den Count fuer die Start-/End-Actions und ob die Shell
    // ueberhaupt den Focus hat
//	if( ActionPend() /*|| !bHasFocus*/ )
    //JP 12.01.98: Bug #46496# - es muss innerhalb einer BasicAction der
    //				Cursor geupdatet werden; um z.B. den TabellenCursor zu
    //				erzeugen. Im EndAction wird jetzt das UpdateCrsr gerufen!
    if( ActionPend() && BasicActionPend() )
    {
        if ( eFlags & SwCrsrShell::READONLY )
            bIgnoreReadonly = TRUE;
        return;             // wenn nicht, dann kein Update !!
    }

    if ( bIgnoreReadonly )
    {
        bIgnoreReadonly = FALSE;
        eFlags |= SwCrsrShell::READONLY;
    }

    if( eFlags & SwCrsrShell::CHKRANGE )	// alle Cursor-Bewegungen auf
        CheckRange( pCurCrsr );     	// ueberlappende Bereiche testen

    if( !bIdleEnd )
        CheckTblBoxCntnt();

    // steht der akt. Crsr in einer Tabelle und in unterschiedlichen Boxen
    // (oder ist noch TabellenMode), dann gilt der Tabellen Mode
    SwPaM* pTstCrsr = pTblCrsr ? pTblCrsr : pCurCrsr;
    if( ( pTstCrsr->HasMark() &&
          pDoc->IsIdxInTbl( pTstCrsr->GetPoint()->nNode ) &&
          ( pTblCrsr ||
            pTstCrsr->GetNode( TRUE )->FindStartNode() !=
            pTstCrsr->GetNode( FALSE )->FindStartNode() ))
        )
    {
        SwShellCrsr* pITmpCrsr = pTblCrsr ? pTblCrsr : pCurCrsr;
        Point aTmpPt( pITmpCrsr->GetPtPos() );
        Point aTmpMk( pITmpCrsr->GetMkPos() );
        SwPosition* pPos = pITmpCrsr->GetPoint();

        // JP 30.04.99: Bug 65475 - falls Point/Mark in versteckten Bereichen
        //				stehen, so mussen diese daraus verschoben werden
        lcl_CheckHiddenSection( pPos->nNode );
        lcl_CheckHiddenSection( pITmpCrsr->GetMark()->nNode );

        // Move cursor out of hidden paragraphs
        if ( !GetViewOptions()->IsShowHiddenChar() )
        {
            lcl_CheckHiddenPara( *pPos );
            lcl_CheckHiddenPara( *pITmpCrsr->GetMark() );
        }

        SwCntntFrm *pTblFrm = pPos->nNode.GetNode().GetCntntNode()->
                                                GetFrm( &aTmpPt, pPos ),
                   *pMarkTblFrm;
        ASSERT( pTblFrm, "Tabelle Crsr nicht im Content ??" );

        SwTabFrm *pTab = pTblFrm->FindTabFrm(), *pMarkTab;

       if ( pTab && pTab->GetTable()->GetRowsToRepeat() > 0 )
        {
            if (  ( pTab->IsFollow() && pTab->IsInHeadline( *pTblFrm ) ) ||
                  ( (pMarkTab = (pMarkTblFrm = pITmpCrsr->GetCntntNode( FALSE )->
                    GetFrm( &aTmpMk, pITmpCrsr->GetMark() ))->FindTabFrm())->IsFollow() &&
                     pMarkTab->IsInHeadline( *pMarkTblFrm ) ) )
            {
                // in wiederholten Tabellen-Kopfzeilen wollen wir keine
                // Tabellen-Selektion !!
                pTblFrm = 0;

                SwPosSection fnPosSect = *pPos <  *pITmpCrsr->GetMark()
                                            ? fnSectionStart
                                            : fnSectionEnd;

                // dann nur innerhalb der Box selektieren
                if( pTblCrsr )
                {
                    pCurCrsr->SetMark();
                    *pCurCrsr->GetMark() = *pTblCrsr->GetMark();
                    pCurCrsr->GetMkPos() = pTblCrsr->GetMkPos();
                    pTblCrsr->DeleteMark();
                    pTblCrsr->SwSelPaintRects::Hide();
                }

                *pCurCrsr->GetPoint() = *pCurCrsr->GetMark();
                (*fnSectionCurr)( *pCurCrsr, fnPosSect );
            }
        }

        // wir wollen wirklich eine Tabellen-Selektion
        if( pTab && pTblFrm )
        {
            if( !pTblCrsr )
            {
                pTblCrsr = new SwShellTableCrsr( *this,
                                *pCurCrsr->GetMark(), pCurCrsr->GetMkPos(),
                                *pPos, aTmpPt );
                pCurCrsr->DeleteMark();
                pCurCrsr->SwSelPaintRects::Hide();

                CheckTblBoxCntnt();
            }

            SwCrsrMoveState aTmpState( MV_NONE );
            aTmpState.bRealHeight = TRUE;
            if( !pTblFrm->GetCharRect( aCharRect, *pTblCrsr->GetPoint(), &aTmpState ) )
            {
                Point aCentrPt( aCharRect.Center() );
                aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
                pTblFrm->GetCrsrOfst( pTblCrsr->GetPoint(), aCentrPt, &aTmpState );
                if ( !pTblFrm->GetCharRect( aCharRect, *pTblCrsr->GetPoint() ) )
                    ASSERT( !this, "GetCharRect failed." );
            }
//          ALIGNRECT( aCharRect );

            pVisCrsr->Hide();       // sichtbaren Cursor immer verstecken
            // Curosr in den sichtbaren Bereich scrollen
            if( (eFlags & SwCrsrShell::SCROLLWIN) &&
                (HasSelection() || eFlags & SwCrsrShell::READONLY ||
                 !IsCrsrReadonly()) )
            {
                SwFrm* pBoxFrm = pTblFrm;
                while( pBoxFrm && !pBoxFrm->IsCellFrm() )
                    pBoxFrm = pBoxFrm->GetUpper();
                if( pBoxFrm && pBoxFrm->Frm().HasArea() )
                    MakeVisible( pBoxFrm->Frm() );
                else
                    MakeVisible( aCharRect );
            }

            // lasse vom Layout die Crsr in den Boxen erzeugen
            if( pTblCrsr->IsCrsrMovedUpdt() )
                GetLayout()->MakeTblCrsrs( *pTblCrsr );
            if( bHasFocus && !bBasicHideCrsr )
                pTblCrsr->Show();

            // Cursor-Points auf die neuen Positionen setzen
            pTblCrsr->GetPtPos().X() = aCharRect.Left();
            pTblCrsr->GetPtPos().Y() = aCharRect.Top();

            if( bSVCrsrVis )
            {
                aCrsrHeight.X() = 0;
                aCrsrHeight.Y() = aTmpState.aRealHeight.Y() < 0 ?
                                  -aCharRect.Width() : aCharRect.Height();
                pVisCrsr->Show();           // wieder anzeigen
            }
            eMvState = MV_NONE;		// Status fuers Crsr-Travelling - GetCrsrOfst
#ifdef ACCESSIBLE_LAYOUT
            if( pTblFrm && Imp()->IsAccessible() )
                Imp()->InvalidateAccessibleCursorPosition( pTblFrm );
#endif
            return;
        }
    }

    if( pTblCrsr )
    {
        // Cursor Ring loeschen
        while( pCurCrsr->GetNext() != pCurCrsr )
            delete pCurCrsr->GetNext();
        pCurCrsr->DeleteMark();
        *pCurCrsr->GetPoint() = *pTblCrsr->GetPoint();
        pCurCrsr->GetPtPos() = pTblCrsr->GetPtPos();
        delete pTblCrsr, pTblCrsr = 0;
    }

    pVisCrsr->Hide();       // sichtbaren Cursor immer verstecken

    // sind wir vielleicht in einer geschuetzten/versteckten Section ?
    {
        BOOL bChgState = TRUE;
        const SwSectionNode* pSectNd = pCurCrsr->GetNode()->FindSectionNode();
        if( pSectNd && ( pSectNd->GetSection().IsHiddenFlag() ||
            ( !IsReadOnlyAvailable() &&
              pSectNd->GetSection().IsProtectFlag() &&
             ( !pDoc->GetDocShell() ||
               !pDoc->GetDocShell()->IsReadOnly() || bAllProtect )) ) )
        {
            if( !FindValidCntntNode( !HasDrawView() ||
                    0 == Imp()->GetDrawView()->GetMarkedObjectList().GetMarkCount()))
            {
                // alles ist geschuetzt / versteckt -> besonderer Mode
                if( bAllProtect && !IsReadOnlyAvailable() &&
                    pSectNd->GetSection().IsProtectFlag() )
                    bChgState = FALSE;
                else
                {
                    eMvState = MV_NONE;		// Status fuers Crsr-Travelling
                    bAllProtect = TRUE;
                    if( GetDoc()->GetDocShell() )
                    {
                        GetDoc()->GetDocShell()->SetReadOnlyUI( TRUE );
                        CallChgLnk();		// UI bescheid sagen!
                    }
                    return;
                }
            }
        }
        if( bChgState )
        {
            BOOL bWasAllProtect = bAllProtect;
            bAllProtect = FALSE;
            if( bWasAllProtect && GetDoc()->GetDocShell() &&
                GetDoc()->GetDocShell()->IsReadOnlyUI() )
            {
                GetDoc()->GetDocShell()->SetReadOnlyUI( FALSE );
                CallChgLnk();		// UI bescheid sagen!
            }
        }
    }

    UpdateCrsrPos();


    // #100722# The cursor must always point into content; there's some code
    // that relies on this. (E.g. in SwEditShell::GetScriptType, which always
    // loops _behind_ the last node in the selection, which always works if you
    // are in content.) To achieve this, we'll force cursor(s) to point into
    // content, if UpdateCrsrPos() hasn't already done so.
    SwPaM* pCmp = pCurCrsr;
    do
    {
        // start will move forwards, end will move backwards
        bool bPointIsStart = ( pCmp->Start() == pCmp->GetPoint() );

        // move point; forward if it's the start, backwards if it's the end
        if( ! pCmp->GetPoint()->nNode.GetNode().IsCntntNode() )
            pCmp->Move( bPointIsStart ? fnMoveForward : fnMoveBackward,
                        fnGoCntnt );

        // move mark (if exists); forward if it's the start, else backwards
        if( pCmp->HasMark() )
        {
            if( ! pCmp->GetMark()->nNode.GetNode().IsCntntNode() )
            {
                pCmp->Exchange();
                pCmp->Move( !bPointIsStart ? fnMoveForward : fnMoveBackward,
                            fnGoCntnt );
                pCmp->Exchange();
            }
        }

        // iterate to next PaM in ring
        pCmp = static_cast<SwPaM*>( pCmp->GetNext() );
    }
    while( pCmp != pCurCrsr );


    SwRect aOld( aCharRect );
    FASTBOOL bFirst = TRUE;
    SwCntntFrm *pFrm;
    int nLoopCnt = 100;

    do {
        BOOL bAgainst;
        do {
            bAgainst = FALSE;
            pFrm = pCurCrsr->GetCntntNode()->GetFrm(
                        &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint() );
            // ist der Frm nicht mehr vorhanden, dann muss das gesamte Layout
            // erzeugt werden, weil ja mal hier einer vorhanden war !!
            if ( !pFrm )
            {
                do
                {
                    CalcLayout();
                    pFrm = pCurCrsr->GetCntntNode()->GetFrm(
                                &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint() );
                }  while( !pFrm );
            }
            else if ( Imp()->IsIdleAction() )
                //Wir stellen sicher, dass anstaendig Formatiert wurde #42224#
                pFrm->PrepareCrsr();

            // im geschuetzten Fly? aber bei Rahmenselektion ignorieren
            if( !IsReadOnlyAvailable() && pFrm->IsProtected() &&
                ( !Imp()->GetDrawView() ||
                  !Imp()->GetDrawView()->GetMarkedObjectList().GetMarkCount() ) &&
                (!pDoc->GetDocShell() ||
                 !pDoc->GetDocShell()->IsReadOnly() || bAllProtect ) )
            {
                // dann suche eine gueltige Position
                BOOL bChgState = TRUE;
                if( !FindValidCntntNode(!HasDrawView() ||
                    0 == Imp()->GetDrawView()->GetMarkedObjectList().GetMarkCount()))
                {
                    // alles ist geschuetzt / versteckt -> besonderer Mode
                    if( bAllProtect )
                        bChgState = FALSE;
                    else
                    {
                        eMvState = MV_NONE;     // Status fuers Crsr-Travelling
                        bAllProtect = TRUE;
                        if( GetDoc()->GetDocShell() )
                        {
                            GetDoc()->GetDocShell()->SetReadOnlyUI( TRUE );
                            CallChgLnk();		// UI bescheid sagen!
                        }
                        return;
                    }
                }

                if( bChgState )
                {
                    BOOL bWasAllProtect = bAllProtect;
                    bAllProtect = FALSE;
                    if( bWasAllProtect && GetDoc()->GetDocShell() &&
                        GetDoc()->GetDocShell()->IsReadOnlyUI() )
                    {
                        GetDoc()->GetDocShell()->SetReadOnlyUI( FALSE );
                        CallChgLnk();		// UI bescheid sagen!
                    }
                    bAllProtect = FALSE;
                    bAgainst = TRUE;        // nochmal den richigen Frm suchen
                }
            }
        } while( bAgainst );

        if( !( eFlags & SwCrsrShell::NOCALRECT ))
        {
            SwCrsrMoveState aTmpState( eMvState );
            aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
            aTmpState.bRealHeight = TRUE;
            aTmpState.bRealWidth = IsOverwriteCrsr();
            aTmpState.nCursorBidiLevel = pCurCrsr->GetCrsrBidiLevel();

            // #i27615#,#i30453#
            SwSpecialPos aSpecialPos;
            aSpecialPos.nExtendRange = SP_EXTEND_RANGE_BEFORE;
            if (pCurCrsr->IsInFrontOfLabel())
            {
                aTmpState.pSpecialPos = &aSpecialPos;
            }

            if( !pFrm->GetCharRect( aCharRect, *pCurCrsr->GetPoint(), &aTmpState ) )
            {
                Point& rPt = pCurCrsr->GetPtPos();
                rPt = aCharRect.Center();
                pFrm->GetCrsrOfst( pCurCrsr->GetPoint(), rPt, &aTmpState );
#ifndef VERTICAL_LAYOUT
                if ( !pFrm->GetCharRect(aCharRect, *pCurCrsr->GetPoint(), &aTmpState) )
                    ASSERT( !this, "GetCharRect failed." );
#endif
            }
//			ALIGNRECT( aCharRect );

            if( !pCurCrsr->HasMark() )
                aCrsrHeight = aTmpState.aRealHeight;
            else
            {
                aCrsrHeight.X() = 0;
                aCrsrHeight.Y() = aTmpState.aRealHeight.Y() < 0 ?
                                  -aCharRect.Width() : aCharRect.Height();
            }
        }
        else
        {
            aCrsrHeight.X() = 0;
            aCrsrHeight.Y() = aCharRect.Height();
        }

        if( !bFirst && aOld == aCharRect )
            break;

        // falls das Layout meint, nach dem 100 durchlauf ist man immer noch
        // im Fluss, sollte man die akt. Pos. als gegeben hinnehmen!
        // siehe Bug: 29658
        if( !--nLoopCnt )
        {
            ASSERT( !this, "Endlosschleife? CharRect != OldCharRect ");
            break;
        }
        aOld = aCharRect;
        bFirst = FALSE;

        // Cursor-Points auf die neuen Positionen setzen
        pCurCrsr->GetPtPos().X() = aCharRect.Left();
        pCurCrsr->GetPtPos().Y() = aCharRect.Top();

        if( !(eFlags & SwCrsrShell::UPDOWN ))	// alte Pos. von Up/Down loeschen
        {
            pFrm->Calc();
#ifdef VERTICAL_LAYOUT
            nUpDownX = pFrm->IsVertical() ?
                       aCharRect.Top() - pFrm->Frm().Top() :
                       aCharRect.Left() - pFrm->Frm().Left();
#else
            nUpDownX = aCharRect.Left() - pFrm->Frm().Left();
#endif
        }

        // Curosr in den sichtbaren Bereich scrollen
        if( bHasFocus && eFlags & SwCrsrShell::SCROLLWIN &&
            (HasSelection() || eFlags & SwCrsrShell::READONLY ||
             !IsCrsrReadonly() || GetViewOptions()->IsSelectionInReadonly()) )
        {
            //JP 30.04.99:  damit das EndAction, beim evtuellen Scrollen, den
            //		SV-Crsr nicht wieder sichtbar macht, wird hier das Flag
            //		gesichert und zurueckgesetzt.
            BOOL bSav = bSVCrsrVis; bSVCrsrVis = FALSE;
            MakeSelVisible();
            bSVCrsrVis = bSav;
        }

    } while( eFlags & SwCrsrShell::SCROLLWIN );

    if( !bIdleEnd && bHasFocus && !bBasicHideCrsr )
        pCurCrsr->SwSelPaintRects::Show();

    //Ggf. gescrollten Bereicht korrigieren (Alignment).
    //Nur wenn gescrollt wurde, und wenn keine Selektion existiert.
    if( pFrm && Imp()->IsScrolled() &&
            pCurCrsr->GetNext() == pCurCrsr && !pCurCrsr->HasMark() )
        Imp()->RefreshScrolledArea( aCharRect );


    eMvState = MV_NONE;		// Status fuers Crsr-Travelling - GetCrsrOfst

#ifdef ACCESSIBLE_LAYOUT
    if( pFrm && Imp()->IsAccessible() )
        Imp()->InvalidateAccessibleCursorPosition( pFrm );
#endif

#ifndef REMOTE_APPSERVER

    // switch from blinking cursor to read-only-text-selection cursor
    const long nBlinkTime = GetOut()->GetSettings().GetStyleSettings().
                            GetCursorBlinkTime();

    if ( (IsCrsrReadonly() && GetViewOptions()->IsSelectionInReadonly()) ==
        ( nBlinkTime != STYLE_CURSOR_NOBLINKTIME ) )
    {
        // non blinking cursor in read only - text selection mode
        AllSettings aSettings = GetOut()->GetSettings();
        StyleSettings aStyleSettings = aSettings.GetStyleSettings();
        long nNewBlinkTime = nBlinkTime == STYLE_CURSOR_NOBLINKTIME ?
                             500 :
                             STYLE_CURSOR_NOBLINKTIME;
        aStyleSettings.SetCursorBlinkTime( nNewBlinkTime );
        aSettings.SetStyleSettings( aStyleSettings );
        GetOut()->SetSettings( aSettings );
    }

#endif

    if( bSVCrsrVis )
        pVisCrsr->Show();           // wieder anzeigen
}



// erzeuge eine Kopie vom Cursor und speicher diese im Stack


void SwCrsrShell::Push()
{
    pCrsrStk = new SwShellCrsr( *this, *pCurCrsr->GetPoint(),
                                    pCurCrsr->GetPtPos(), pCrsrStk );

    if( pCurCrsr->HasMark() )
    {
        pCrsrStk->SetMark();
        *pCrsrStk->GetMark() = *pCurCrsr->GetMark();
    }
}

/*
 *  Loescht einen Cursor (gesteuert durch bOldCrsr)
 *      - vom Stack oder    ( bOldCrsr = TRUE )
 *      - den aktuellen und der auf dem Stack stehende wird zum aktuellen
 *
 *  Return:  es war auf dem Stack noch einer vorhanden
 */


FASTBOOL SwCrsrShell::Pop( BOOL bOldCrsr )
{
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen

    // noch weitere vorhanden ?
    if( 0 == pCrsrStk )
        return FALSE;

    SwShellCrsr *pTmp = 0, *pOldStk = pCrsrStk;

    // der Nachfolger wird der Aktuelle
    if( pCrsrStk->GetNext() != pCrsrStk )
        pTmp = (SwShellCrsr*)*((SwCursor*)pCrsrStk->GetNext());

    if( bOldCrsr )              // loesche vom Stack
        delete pCrsrStk;        //

    pCrsrStk = pTmp;            // neu zuweisen

    if( !bOldCrsr )
    {
        SwCrsrSaveState aSaveState( *pCurCrsr );

        // wurde die sichtbare SSelection nicht veraendert
        if( pOldStk->GetPtPos() == pCurCrsr->GetPtPos() ||
            pOldStk->GetPtPos() == pCurCrsr->GetMkPos() )
        {
            // "Selektions-Rechtecke" verschieben
            pCurCrsr->Insert( pOldStk, 0 );
            pOldStk->Remove( 0, pOldStk->Count() );
        }

        if( pOldStk->HasMark() )
        {
            pCurCrsr->SetMark();
            *pCurCrsr->GetMark() = *pOldStk->GetMark();
            pCurCrsr->GetMkPos() = pOldStk->GetMkPos();
        }
        else
            // keine Selection also alte aufheben und auf die alte Pos setzen
            pCurCrsr->DeleteMark();
        *pCurCrsr->GetPoint() = *pOldStk->GetPoint();
        pCurCrsr->GetPtPos() = pOldStk->GetPtPos();
        delete pOldStk;

        if( !pCurCrsr->IsInProtectTable( TRUE ) &&
            !pCurCrsr->IsSelOvr( SELOVER_TOGGLE | SELOVER_CHANGEPOS ) )
            UpdateCrsr();             // akt. Cursor Updaten
    }
    return TRUE;
}

/*
 * Verbinde zwei Cursor miteinander.
 * Loesche vom Stack den obersten und setzen dessen GetMark im Aktuellen.
 */


void SwCrsrShell::Combine()
{
    // noch weitere vorhanden ?
    if( 0 == pCrsrStk )
        return;

    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    SwCrsrSaveState aSaveState( *pCurCrsr );
    if( pCrsrStk->HasMark() )           // nur wenn GetMark gesetzt wurde
    {
        if( !CheckNodesRange( pCrsrStk->GetMark()->nNode, pCurCrsr->GetPoint()->nNode, TRUE ))
            ASSERT( !this, "StackCrsr & akt. Crsr nicht in gleicher Section." );

        // kopiere das GetMark
        if( !pCurCrsr->HasMark() )
            pCurCrsr->SetMark();
        *pCurCrsr->GetMark() = *pCrsrStk->GetMark();
        pCurCrsr->GetMkPos() = pCrsrStk->GetMkPos();
    }

    SwShellCrsr * pTmp = 0;
    if( pCrsrStk->GetNext() != pCrsrStk )
        pTmp = (SwShellCrsr*)*((SwCursor*)pCrsrStk->GetNext());
    delete pCrsrStk;
    pCrsrStk = pTmp;
    if( !pCurCrsr->IsInProtectTable( TRUE ) &&
        !pCurCrsr->IsSelOvr( SELOVER_TOGGLE | SELOVER_CHANGEPOS ) )
        UpdateCrsr();             // akt. Cursor Updaten
}


void SwCrsrShell::HideCrsrs()
{
    if( !bHasFocus || bBasicHideCrsr )
        return;

    // ist Cursor sichtbar, dann verstecke den SV-Cursor
    if( pVisCrsr->IsVisible() )
    {
        SET_CURR_SHELL( this );
        pVisCrsr->Hide();
    }
    // hebe die Invertierung der SSelection auf
    SwShellCrsr* pAktCrsr = pTblCrsr ? pTblCrsr : pCurCrsr;
    pAktCrsr->Hide();
}



void SwCrsrShell::ShowCrsrs( BOOL bCrsrVis )
{
    if( !bHasFocus || bAllProtect || bBasicHideCrsr )
        return;

    SET_CURR_SHELL( this );
    SwShellCrsr* pAktCrsr = pTblCrsr ? pTblCrsr : pCurCrsr;
    pAktCrsr->Show();

    if( bSVCrsrVis && bCrsrVis )    // auch SV-Cursor wieder anzeigen
        pVisCrsr->Show();
}

// Methoden zum Anzeigen bzw. Verstecken des sichtbaren Text-Cursors


void SwCrsrShell::ShowCrsr()
{
    if( !bBasicHideCrsr )
    {
        bSVCrsrVis = TRUE;
        UpdateCrsr();
    }
}


void SwCrsrShell::HideCrsr()
{
    if( !bBasicHideCrsr )
    {
        bSVCrsrVis = FALSE;
        // evt. die sel. Bereiche aufheben !!
        SET_CURR_SHELL( this );
        pVisCrsr->Hide();
    }
}


void SwCrsrShell::ShLooseFcs()
{
    if( !bBasicHideCrsr )
        HideCrsrs();
    bHasFocus = FALSE;
}


void SwCrsrShell::ShGetFcs( BOOL bUpdate )
{
    bHasFocus = TRUE;
    if( !bBasicHideCrsr && VisArea().Width() )
    {
        UpdateCrsr( bUpdate ? SwCrsrShell::CHKRANGE|SwCrsrShell::SCROLLWIN
                            : SwCrsrShell::CHKRANGE );
        ShowCrsrs( bSVCrsrVis ? TRUE : FALSE );
    }
}

// gebe den aktuellen Frame, in dem der Cursor steht, zurueck

#if 0

//MA 03. Nov. 95: Die letzten Anwender habe ich gerade aus wrtsh1.cxx entfernt.
//                Weil's so kunstvoll aussieht lass ich die Funktion vorlauefig
//                hier.


Rectangle SwCrsrShell::GetCurrFrmArea() const
{
    //Sitzt der Crsr ueberhaupt auf einem CntntNode?
    SET_CURR_SHELL( (ViewShell*)this );
    Rectangle aRet;
    SwCntntNode *pNd = GetNode().GetCntntNode();
    if ( pNd )
    {
        const USHORT* pST = &nStartAction;
        ++(*((USHORT*)pST));
        const Size aOldSz( GetLayout()->Frm().SSize() );
        SwCntntFrm *pFrm = pNd->GetFrm(
                            &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint() );
        aRet = pFrm->Frm().SVRect();
        --(*((USHORT*)pST));
        if( aOldSz != GetLayout()->Frm().SSize() )
            ((SwCrsrShell*)this)->SizeChgNotify( GetLayout()->Frm().SSize() );
    }
    return aRet;
}
#endif


SwCntntFrm *SwCrsrShell::GetCurrFrm( const BOOL bCalcFrm ) const
{
    SET_CURR_SHELL( (ViewShell*)this );
    SwCntntFrm *pRet = 0;
    SwCntntNode *pNd = pCurCrsr->GetCntntNode();
    if ( pNd )
    {
        if ( bCalcFrm )
        {
            const USHORT* pST = &nStartAction;
            ++(*((USHORT*)pST));
            const Size aOldSz( GetLayout()->Frm().SSize() );
            pRet = pNd->GetFrm( &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint() );
            --(*((USHORT*)pST));
            if( aOldSz != GetLayout()->Frm().SSize() )
                ((SwCrsrShell*)this)->SizeChgNotify( GetLayout()->Frm().SSize() );
        }
        else
            pRet = pNd->GetFrm( &pCurCrsr->GetPtPos(), pCurCrsr->GetPoint(), FALSE);
    }
    return pRet;
}


// alle Attribut/Format-Aenderungen am akt. Node werden an den
// Link weitergeleitet.


void SwCrsrShell::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    USHORT nWhich = pOld ? pOld->Which()
                         : pNew ? pNew->Which()
                                 : RES_MSG_BEGIN;
    if( bCallChgLnk &&
        ( nWhich < RES_MSG_BEGIN || nWhich >= RES_MSG_END ||
            nWhich == RES_FMT_CHG || nWhich == RES_UPDATE_ATTR ||
            nWhich == RES_ATTRSET_CHG ))
        // die Messages werden nicht weitergemeldet
        //MA 07. Apr. 94 fix(6681): RES_UPDATE_ATTR wird implizit vom
        //SwTxtNode::Insert(SwTxtHint*, USHORT) abgesetzt; hier wird reagiert und
        //vom Insert brauch nicht mehr die Keule RES_FMT_CHG versandt werden.
        CallChgLnk();

    if( aGrfArrivedLnk.IsSet() &&
        ( RES_GRAPHIC_ARRIVED == nWhich || RES_GRAPHIC_SWAPIN == nWhich ))
        aGrfArrivedLnk.Call( this );
}


// Abfrage, ob der aktuelle Cursor eine Selektion aufspannt,
// also, ob GetMark gesetzt und SPoint und GetMark unterschiedlich sind.


FASTBOOL SwCrsrShell::HasSelection() const
{
    SwPaM* pCrsr = IsTableMode() ? pTblCrsr : pCurCrsr;
    return( IsTableMode() || ( pCurCrsr->HasMark() &&
            *pCurCrsr->GetPoint() != *pCrsr->GetMark())
        ? TRUE : FALSE );
}


void SwCrsrShell::CallChgLnk()
{
    // innerhalb von Start-/End-Action kein Call, sondern nur merken,
    // das sich etwas geaendert hat. Wird bei EndAction beachtet.
    if( BasicActionPend() )
        bChgCallFlag = TRUE;		// das Change merken
    else if( aChgLnk.IsSet() )
    {
        if( bCallChgLnk )
            aChgLnk.Call( this );
        bChgCallFlag = FALSE;		// Flag zuruecksetzen
    }
}

// returne den am akt.Cursor selektierten Text eines Nodes.


String SwCrsrShell::GetSelTxt() const
{
    String aTxt;
    if( pCurCrsr->GetPoint()->nNode.GetIndex() ==
        pCurCrsr->GetMark()->nNode.GetIndex() )
    {
        SwTxtNode* pTxtNd = pCurCrsr->GetNode()->GetTxtNode();
        if( pTxtNd )
        {
            xub_StrLen nStt = pCurCrsr->Start()->nContent.GetIndex();
            aTxt = pTxtNd->GetExpandTxt( nStt,
                    pCurCrsr->End()->nContent.GetIndex() - nStt );
        }
    }
    return aTxt;
}

// gebe nur den Text ab der akt. Cursor Position zurueck (bis zum NodeEnde)


String SwCrsrShell::GetText() const
{
    String aTxt;
    if( pCurCrsr->GetPoint()->nNode.GetIndex() ==
        pCurCrsr->GetMark()->nNode.GetIndex() )
    {
        SwTxtNode* pTxtNd = pCurCrsr->GetNode()->GetTxtNode();
        if( pTxtNd )
            aTxt = pTxtNd->GetTxt().Copy(
                    pCurCrsr->GetPoint()->nContent.GetIndex() );
    }
    return aTxt;
}

// hole vom Start/Ende der akt. SSelection das nte Zeichen
sal_Unicode SwCrsrShell::GetChar( BOOL bEnd, long nOffset )
{
    if( IsTableMode() )			// im TabelleMode nicht moeglich
        return 0;

    const SwPosition* pPos = !pCurCrsr->HasMark() ? pCurCrsr->GetPoint()
                                : bEnd ? pCurCrsr->End() : pCurCrsr->Start();
    SwTxtNode* pTxtNd = pPos->nNode.GetNode().GetTxtNode();
    ASSERT( pTxtNd, "kein TextNode, wie soll ein char returnt werden?" );

    xub_StrLen nPos = pPos->nContent.GetIndex();
    const String& rStr = pTxtNd->GetTxt();
    sal_Unicode cCh = 0;

    if( ((nPos+nOffset) >= 0 ) && (nPos+nOffset) < rStr.Len() )
        cCh = rStr.GetChar( nPos+nOffset );

    return cCh;
}

// erweiter die akt. SSelection am Anfang/Ende um n Zeichen


FASTBOOL SwCrsrShell::ExtendSelection( BOOL bEnd, xub_StrLen nCount )
{
    if( !pCurCrsr->HasMark() || IsTableMode() )
        return FALSE;			// keine Selektion

    SwPosition* pPos = bEnd ? pCurCrsr->End() : pCurCrsr->Start();
    SwTxtNode* pTxtNd = pPos->nNode.GetNode().GetTxtNode();
    ASSERT( pTxtNd, "kein TextNode, wie soll erweitert werden?" );

    xub_StrLen nPos = pPos->nContent.GetIndex();
    if( bEnd )
    {
        if( ( nPos + nCount ) <= pTxtNd->GetTxt().Len() )
            nPos += nCount;
        else
            return FALSE;		// nicht mehr moeglich
    }
    else if( nPos >= nCount )
        nPos -= nCount;
    else
        return FALSE;			// nicht mehr moeglich

    SwCallLink aLk( *this );	// Crsr-Moves ueberwachen,

    pPos->nContent = nPos;
    UpdateCrsr();

    return TRUE;
}

// setze nur den sichtbaren Cursor an die angegebene Dokument-Pos.
// returnt FALSE: wenn der SPoint vom Layout korrigiert wurde.

FASTBOOL SwCrsrShell::SetVisCrsr( const Point &rPt )
{
    SET_CURR_SHELL( this );
    Point aPt( rPt );
    SwPosition aPos( *pCurCrsr->GetPoint() );
    SwCrsrMoveState aTmpState( MV_SETONLYTEXT );
    aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
    aTmpState.bRealHeight = TRUE;

    FASTBOOL bRet = GetLayout()->GetCrsrOfst( &aPos, aPt /*, &aTmpState*/ );

    SetInFrontOfLabel( FALSE ); // #i27615#

    // nur in TextNodes anzeigen !!
    SwTxtNode* pTxtNd = aPos.nNode.GetNode().GetTxtNode();
    if( !pTxtNd )
        return FALSE;

    const SwSectionNode* pSectNd = pTxtNd->FindSectionNode();
    if( pSectNd && (pSectNd->GetSection().IsHiddenFlag() ||
                    ( !IsReadOnlyAvailable() &&
                      pSectNd->GetSection().IsProtectFlag())) )
        return FALSE;

    SwCntntFrm *pFrm = pTxtNd->GetFrm( &aPt, &aPos );
    if ( Imp()->IsIdleAction() )
        pFrm->PrepareCrsr();
    SwRect aTmp( aCharRect );

    pFrm->GetCharRect( aCharRect, aPos, &aTmpState );
//	ALIGNRECT( aCharRect );

    if( aTmp == aCharRect && 		// BUG 10137: bleibt der Cursor auf der
        pVisCrsr->IsVisible() )     // Position nicht hidden & showen
        return TRUE;

    pVisCrsr->Hide();       // sichtbaren Cursor immer verstecken
    if( IsScrollMDI( this, aCharRect ))
    {
        MakeVisible( aCharRect );
        pCurCrsr->Show();
    }

    // Bug 29584: bei Rahmenselektion ist der Cursor versteckt, aber den
    //			D&D-Cursor will man trotzdem haben
//	if( bSVCrsrVis )
    {
        if( aTmpState.bRealHeight )
            aCrsrHeight = aTmpState.aRealHeight;
        else
        {
            aCrsrHeight.X() = 0;
            aCrsrHeight.Y() = aCharRect.Height();
        }

        pVisCrsr->SetDragCrsr( TRUE );
        pVisCrsr->Show();           // wieder anzeigen
    }
    return bRet;
}

FASTBOOL SwCrsrShell::IsOverReadOnlyPos( const Point& rPt ) const
{
    Point aPt( rPt );
    SwPaM aPam( *pCurCrsr->GetPoint() );
    GetLayout()->GetCrsrOfst( aPam.GetPoint(), aPt );
    // --> FME 2004-06-29 #114856# Formular view
    return aPam.HasReadonlySel( GetViewOptions()->IsFormView() );
    // <--
}


    // returne die Anzahl der Cursor im Ring (Flag besagt ob man nur
    // aufgepspannte haben will - sprich etwas selektiert ist (Basic))
USHORT SwCrsrShell::GetCrsrCnt( BOOL bAll ) const
{
    Ring* pTmp = GetCrsr()->GetNext();
    USHORT n = (bAll || ( pCurCrsr->HasMark() &&
                    *pCurCrsr->GetPoint() != *pCurCrsr->GetMark())) ? 1 : 0;
    while( pTmp != pCurCrsr )
    {
        if( bAll || ( ((SwPaM*)pTmp)->HasMark() &&
                *((SwPaM*)pTmp)->GetPoint() != *((SwPaM*)pTmp)->GetMark()))
            ++n;
        pTmp = pTmp->GetNext();
    }
    return n;
}


FASTBOOL SwCrsrShell::IsStartOfDoc() const
{
    if( pCurCrsr->GetPoint()->nContent.GetIndex() )
        return FALSE;

    // Hinter EndOfIcons kommt die Content-Section (EndNd+StNd+CntntNd)
    SwNodeIndex aIdx( GetDoc()->GetNodes().GetEndOfExtras(), 2 );
    if( !aIdx.GetNode().IsCntntNode() )
        GetDoc()->GetNodes().GoNext( &aIdx );
    return aIdx == pCurCrsr->GetPoint()->nNode;
}


FASTBOOL SwCrsrShell::IsEndOfDoc() const
{
    SwNodeIndex aIdx( GetDoc()->GetNodes().GetEndOfContent(), -1 );
    SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
    if( !pCNd )
        pCNd = GetDoc()->GetNodes().GoPrevious( &aIdx );

    return aIdx == pCurCrsr->GetPoint()->nNode &&
            pCNd->Len() == pCurCrsr->GetPoint()->nContent.GetIndex();
}


// loesche alle erzeugten Crsr, setze den Tabellen-Crsr und den letzten
// Cursor auf seinen TextNode (oder StartNode?).
// Beim naechsten ::GetCrsr werden sie wieder alle erzeugt
// Wird fuers Drag&Drop / ClipBorad-Paste in Tabellen benoetigt.
FASTBOOL SwCrsrShell::ParkTblCrsr()
{
    if( !pTblCrsr )
        return FALSE;

    pTblCrsr->ParkCrsr();

    while( pCurCrsr->GetNext() != pCurCrsr )
        delete pCurCrsr->GetNext();

    // vom Cursor !immer! SPoint und Mark umsetzen
    pCurCrsr->SetMark();
    *pCurCrsr->GetMark() = *pCurCrsr->GetPoint() = *pTblCrsr->GetPoint();
    pCurCrsr->DeleteMark();

    return TRUE;
}

/***********************************************************************
#*	Class		:  SwCrsrShell
#*	Methode 	:  ParkCrsr
#*	Beschreibung:  Vernichtet Selektionen und zus. Crsr aller Shell der
#*				   verbleibende Crsr der Shell wird geparkt.
#*	Datum		:  MA 05. Nov. 92
#*	Update		:  JP 19.09.97
#***********************************************************************/

void SwCrsrShell::_ParkPams( SwPaM* pDelRg, SwShellCrsr** ppDelRing )
{
    const SwPosition *pStt = pDelRg->Start(),
        *pEnd = pDelRg->GetPoint() == pStt ? pDelRg->GetMark() : pDelRg->GetPoint();

    SwPaM *pTmpDel = 0, *pTmp = *ppDelRing;

    // durchsuche den gesamten Ring
    BOOL bGoNext;
    do {
        const SwPosition *pTmpStt = pTmp->Start(),
                        *pTmpEnd = pTmp->GetPoint() == pTmpStt ?
                                        pTmp->GetMark() : pTmp->GetPoint();
        /*
         * liegt ein SPoint oder GetMark innerhalb vom Crsr-Bereich
         * muss der alte Bereich aufgehoben werden.
         * Beim Vergleich ist darauf zu achten, das End() nicht mehr zum
         * Bereich gehoert !
         */
        if( *pStt <= *pTmpStt )
        {
            if( *pEnd > *pTmpStt ||
                ( *pEnd == *pTmpStt && *pEnd == *pTmpEnd ))
                pTmpDel = pTmp;
        }
        else
            if( *pStt < *pTmpEnd )
                pTmpDel = pTmp;

        bGoNext = TRUE;
        if( pTmpDel )			// ist der Pam im Bereich ?? loesche ihn
        {
            BOOL bDelete = TRUE;
            if( *ppDelRing == pTmpDel )
            {
                if( *ppDelRing == pCurCrsr )
                {
                    if( TRUE == ( bDelete = GoNextCrsr() ))
                    {
                        bGoNext = FALSE;
                        pTmp = (SwPaM*)pTmp->GetNext();
                    }
                }
                else
                    bDelete = FALSE;		// StackCrsr nie loeschen !!
            }

            if( bDelete )
                delete pTmpDel; 		// hebe alten Bereich auf
            else
            {
                pTmpDel->GetPoint()->nContent.Assign( 0, 0 );
                pTmpDel->GetPoint()->nNode = 0;
                pTmpDel->SetMark();
                pTmpDel->DeleteMark();
            }
            pTmpDel = 0;
        }
        else if( !pTmp->HasMark() ) 	// sorge auf jedenfall dafuer, das
        {						// nicht benutzte Indizies beachtet werden!
            pTmp->SetMark();			// SPoint liegt nicht im Bereich,
            pTmp->DeleteMark(); 		// aber vielleicht GetMark, also setzen
        }
        if( bGoNext )
            pTmp = (SwPaM*)pTmp->GetNext();
    } while( !bGoNext || *ppDelRing != pTmp );
}

void SwCrsrShell::ParkCrsr( const SwNodeIndex &rIdx )
{
    SwNode *pNode = &rIdx.GetNode();

    // erzeuge einen neuen Pam
    SwPaM * pNew = new SwPaM( *GetCrsr()->GetPoint() );
    if( pNode->GetStartNode() )
    {
        if( ( pNode = pNode->StartOfSectionNode())->IsTableNode() )
        {
            // der angegebene Node steht in einer Tabelle, also Parke
            // den Crsr auf dem Tabellen-Node (ausserhalb der Tabelle)
            pNew->GetPoint()->nNode = *pNode->StartOfSectionNode();
        }
        else	// also auf dem StartNode selbst.
                // Dann immer ueber seinen EndNode den StartNode erfragen !!!
                // (StartOfSection vom StartNode ist der Parent !)
            pNew->GetPoint()->nNode = *pNode->EndOfSectionNode()->StartOfSectionNode();
    }
    else
        pNew->GetPoint()->nNode = *pNode->StartOfSectionNode();
    pNew->SetMark();
    pNew->GetPoint()->nNode = *pNode->EndOfSectionNode();

    //Alle Shells wollen etwas davon haben.
    ViewShell *pTmp = this;
    do {
        if( pTmp->IsA( TYPE( SwCrsrShell )))
        {
            SwCrsrShell* pSh = (SwCrsrShell*)pTmp;
            if( pSh->pCrsrStk )
                pSh->_ParkPams( pNew, &pSh->pCrsrStk );

            pSh->_ParkPams( pNew, &pSh->pCurCrsr );
            if( pSh->pTblCrsr )
            {
                // setze den Tabellen Cursor immer auf 0, den aktuellen
                // immer auf den Anfang der Tabelle
                SwPaM* pTCrsr = pSh->GetTblCrs();
                SwNode* pTblNd = pTCrsr->GetPoint()->nNode.GetNode().FindTableNode();
                if ( pTblNd )
                {
                    pTCrsr->GetPoint()->nContent.Assign( 0, 0 );
                    pTCrsr->GetPoint()->nNode = 0;
                    pTCrsr->SetMark();
                    pTCrsr->DeleteMark();
                    pSh->pCurCrsr->GetPoint()->nNode = *pTblNd;
                }
            }
        }
    } while ( this != (pTmp = (ViewShell*)pTmp->GetNext() ));
    delete pNew;
}

//=========================================================================

/*
 * der Copy-Constructor
 * Cursor-Position kopieren, in den Ring eingetragen.
 * Alle Ansichten eines Dokumentes stehen im Ring der Shells.
 */

SwCrsrShell::SwCrsrShell( SwCrsrShell& rShell, Window *pWin )
    : ViewShell( rShell, pWin ),
    SwModify( 0 )
{
    SET_CURR_SHELL( this );
    // Nur die Position vom aktuellen Cursor aus der Copy-Shell uebernehmen
    pCurCrsr = new SwShellCrsr( *this, *(rShell.pCurCrsr->GetPoint()) );
    pCurCrsr->GetCntntNode()->Add( this );
    pCrsrStk = 0;
    pTblCrsr = 0;

    nBasicActionCnt = 0;

    pBoxIdx = 0;
    pBoxPtr = 0;

    /*
     * setze die initiale Spalten-Position fuer Up / Down
     */
    nCrsrMove = 0;
    bAllProtect = bVisPortChgd = bChgCallFlag = bInCMvVisportChgd =
    bGCAttr = bIgnoreReadonly = bSelTblCells = bBasicHideCrsr =
    bOverwriteCrsr = FALSE;
    bCallChgLnk = bHasFocus = bSVCrsrVis = bAutoUpdateCells = TRUE;
    bSetCrsrInReadOnly = TRUE;
    eMvState = MV_NONE;		// Status fuers Crsr-Travelling - GetCrsrOfst
    pVisCrsr = new SwVisCrsr( this );
//	UpdateCrsr( 0 );
    // OD 11.02.2003 #100556#
    mbMacroExecAllowed = rShell.IsMacroExecAllowed();
    nMarkedNumLevel = 0; // #i27615#
}


/*
 * der normale Constructor
 */

SwCrsrShell::SwCrsrShell( SwDoc& rDoc, Window *pWin, SwRootFrm *pRoot,
                            const SwViewOption *pOpt )
    : ViewShell( rDoc, pWin, pOpt ),
    SwModify( 0 )
{
    SET_CURR_SHELL( this );
    /*
     * Erzeugen des initialen Cursors, wird auf die erste
     * Inhaltsposition gesetzt
     */
    SwNodes& rNds = rDoc.GetNodes();

    SwNodeIndex aNodeIdx( *rNds.GetEndOfContent().StartOfSectionNode() );
    SwCntntNode* pCNd = rNds.GoNext( &aNodeIdx ); // gehe zum 1. ContentNode

    pCurCrsr = new SwShellCrsr( *this, SwPosition( aNodeIdx, SwIndex( pCNd, 0 )));
    pCrsrStk = 0;
    pTblCrsr = 0;

    nBasicActionCnt = 0;

    pBoxIdx = 0;
    pBoxPtr = 0;

    // melde die Shell beim akt. Node als abhaengig an, dadurch koennen alle
    // Attribut-Aenderungen ueber den Link weiter gemeldet werden.
    pCNd->Add( this );

    /*
     * setze die initiale Spalten-Position fuer Up / Down
     */
    nCrsrMove = 0;
    bAllProtect = bVisPortChgd = bChgCallFlag = bInCMvVisportChgd =
    bGCAttr = bIgnoreReadonly = bSelTblCells = bBasicHideCrsr =
    bOverwriteCrsr = FALSE;
    bCallChgLnk = bHasFocus = bSVCrsrVis = bAutoUpdateCells = TRUE;
    bSetCrsrInReadOnly = TRUE;
    eMvState = MV_NONE;		// Status fuers Crsr-Travelling - GetCrsrOfst

    pVisCrsr = new SwVisCrsr( this );
//	UpdateCrsr( 0 );
    // OD 11.02.2003 #100556#
    mbMacroExecAllowed = true;
    nMarkedNumLevel = 0; // #i27615#
}



SwCrsrShell::~SwCrsrShell()
{
    // wenn es nicht die letzte View so sollte zu mindest das
    // Feld noch geupdatet werden.
    if( GetNext() != this )
        CheckTblBoxCntnt( pCurCrsr->GetPoint() );
    else
        ClearTblBoxCntnt();

    delete pVisCrsr;
    delete pTblCrsr;

    /*
     * Freigabe der Cursor
     */
    while(pCurCrsr->GetNext() != pCurCrsr)
        delete pCurCrsr->GetNext();
    delete pCurCrsr;

    // Stack freigeben
    if( pCrsrStk )
    {
        while( pCrsrStk->GetNext() != pCrsrStk )
            delete pCrsrStk->GetNext();
        delete pCrsrStk;
    }

    // JP 27.07.98: Bug 54025 - ggfs. den HTML-Parser, der als Client in
    // 				der CursorShell haengt keine Chance geben, sich an den
    //				TextNode zu haengen.
    if( GetRegisteredIn() )
        pRegisteredIn->Remove( this );
}



//Sollte fuer das Clipboard der WaitPtr geschaltet werden?
//Warten bei TableMode, Mehrfachselektion und mehr als x Selektieren Absaetzen.

FASTBOOL SwCrsrShell::ShouldWait() const
{
    if ( IsTableMode() || GetCrsrCnt() > 1 )
        return TRUE;

    if( HasDrawView() && GetDrawView()->GetMarkedObjectList().GetMarkCount() )
        return TRUE;

    SwPaM* pPam = GetCrsr();
    return pPam->Start()->nNode.GetIndex() + 10 <
            pPam->End()->nNode.GetIndex();
}


USHORT SwCrsrShell::UpdateTblSelBoxes()
{
    if( pTblCrsr && ( pTblCrsr->IsChgd() || !pTblCrsr->GetBoxesCount() ))
         GetLayout()->MakeTblCrsrs( *pTblCrsr );
    return pTblCrsr ? pTblCrsr->GetBoxesCount() : 0;
}

// zeige das akt. selektierte "Object" an
void SwCrsrShell::MakeSelVisible()
{
    ASSERT( bHasFocus, "kein Focus aber Cursor sichtbar machen?" );
    if( aCrsrHeight.Y() < aCharRect.Height() && aCharRect.Height() > VisArea().Height() )
    {
        SwRect aTmp( aCharRect );
        long nDiff = aCharRect.Height() - VisArea().Height();
        if( nDiff < aCrsrHeight.X() )
            aTmp.Top( nDiff + aCharRect.Top() );
        else
        {
            aTmp.Top( aCrsrHeight.X() + aCharRect.Top() );
            aTmp.Height( aCrsrHeight.Y() );
        }
        if( !aTmp.HasArea() )
        {
            aTmp.SSize().Height() += 1;
            aTmp.SSize().Width() += 1;
        }
        MakeVisible( aTmp );
    }
    else
    {
        if( aCharRect.HasArea() )
            MakeVisible( aCharRect );
        else
        {
            SwRect aTmp( aCharRect );
            aTmp.SSize().Height() += 1; aTmp.SSize().Width() += 1;
            MakeVisible( aTmp );
        }
    }
}


// suche eine gueltige ContentPosition (nicht geschuetzt/nicht versteckt)
FASTBOOL SwCrsrShell::FindValidCntntNode( BOOL bOnlyText )
{
    if( pTblCrsr )		// was soll ich jetzt machen ??
    {
        ASSERT( !this, "TabellenSelection nicht aufgehoben!" );
        return FALSE;
    }

    //JP 28.10.97: Bug 45129 - im UI-ReadOnly ist alles erlaubt
    if( !bAllProtect && GetDoc()->GetDocShell() &&
        GetDoc()->GetDocShell()->IsReadOnlyUI() )
        return TRUE;

    // dann raus da!
    if( pCurCrsr->HasMark()	)
        ClearMark();

    // als erstes mal auf Rahmen abpruefen
    SwNodeIndex& rNdIdx = pCurCrsr->GetPoint()->nNode;
    ULONG nNdIdx = rNdIdx.GetIndex();		// sichern
    SwNodes& rNds = pDoc->GetNodes();
    SwCntntNode* pCNd = rNdIdx.GetNode().GetCntntNode();
    const SwCntntFrm * pFrm;

    if( pCNd && 0 != (pFrm = pCNd->GetFrm(0,pCurCrsr->GetPoint(),FALSE)) &&
        !IsReadOnlyAvailable() && pFrm->IsProtected() &&
        nNdIdx < rNds.GetEndOfExtras().GetIndex() )
    {
        // geschuetzter Rahmen ueberspringen
        SwPaM aPam( *pCurCrsr->GetPoint() );
        aPam.SetMark();
        aPam.GetMark()->nNode = rNds.GetEndOfContent();
        aPam.GetPoint()->nNode = *pCNd->EndOfSectionNode();

        FASTBOOL bFirst = FALSE;
        if( 0 == (pCNd = ::GetNode( aPam, bFirst, fnMoveForward, FALSE )))
        {
            aPam.GetMark()->nNode = *rNds.GetEndOfPostIts().StartOfSectionNode();
            pCNd = ::GetNode( aPam, bFirst, fnMoveBackward, FALSE );
        }

        if( !pCNd )		// sollte nie passieren !!!
        {
            rNdIdx = nNdIdx;		// alten Node zurueck
            return FALSE;
        }
        *pCurCrsr->GetPoint() = *aPam.GetPoint();
    }
    else if( bOnlyText && pCNd && pCNd->IsNoTxtNode() )
    {
        // dann auf den Anfang vom Doc stellen
        rNdIdx = pDoc->GetNodes().GetEndOfExtras();
        pCurCrsr->GetPoint()->nContent.Assign( pDoc->GetNodes().GoNext(
                                                            &rNdIdx ), 0 );
        nNdIdx = rNdIdx.GetIndex();
    }

    BOOL bOk = TRUE;

    // #i9059# cursor may not stand in protected cells
    //         (unless cursor in protected areas is OK.)
    const SwTableNode* pTableNode = rNdIdx.GetNode().FindTableNode();
    if( !IsReadOnlyAvailable()  &&
        pTableNode != NULL  &&  rNdIdx.GetNode().IsProtect() )
    {
        // we're in a table, and we're in a protected area, so we're
        // probably in a protected cell.

        // move forward into non-protected area.
        SwPaM aPam( rNdIdx.GetNode(), 0 );
        while( aPam.GetNode()->IsProtect() &&
               aPam.Move( fnMoveForward, fnGoCntnt ) )
            ; // nothing to do in the loop; the aPam.Move does the moving!

        // didn't work? then go backwards!
        if( aPam.GetNode()->IsProtect() )
        {
            SwPaM aTmpPaM( rNdIdx.GetNode(), 0 );
            aPam = aTmpPaM;
            while( aPam.GetNode()->IsProtect() &&
                   aPam.Move( fnMoveBackward, fnGoCntnt ) )
                ; // nothing to do in the loop; the aPam.Move does the moving!
        }

        // if we're successful, set the new position
        if( ! aPam.GetNode()->IsProtect() )
        {
            *pCurCrsr->GetPoint() = *aPam.GetPoint();
        }
    }

    // in einem geschuetzten Bereich
    const SwSectionNode* pSectNd = rNdIdx.GetNode().FindSectionNode();
    if( pSectNd && ( pSectNd->GetSection().IsHiddenFlag() ||
        ( !IsReadOnlyAvailable() &&
           pSectNd->GetSection().IsProtectFlag() )) )
    {
        typedef SwCntntNode* (SwNodes:: *FNGoSection)( SwNodeIndex *, int, int ) const;
        FNGoSection fnGoSection = &SwNodes::GoNextSection;

        bOk = FALSE;

        for( int nLoopCnt = 0; !bOk && nLoopCnt < 2; ++nLoopCnt )
        {
            BOOL bWeiter;
            do {
                bWeiter = FALSE;
                while( 0 != ( pCNd = (rNds.*fnGoSection)( &rNdIdx,
                                            TRUE, !IsReadOnlyAvailable() )) )
                {
                    // in eine Tabelle verschoben -> pruefe ob die
                    // vielleicht geschuetzt ist
                    if( pCNd->FindTableNode() )
                    {
                        SwCallLink aTmp( *this );
                        SwCrsrSaveState aSaveState( *pCurCrsr );
                        aTmp.nNdTyp = 0;		// im DTOR nichts machen!
                        if( !pCurCrsr->IsInProtectTable( TRUE, TRUE ) )
                        {
                            const SwSectionNode* pSNd = pCNd->FindSectionNode();
                            if( !pSNd || !pSNd->GetSection().IsHiddenFlag()
                                || (!IsReadOnlyAvailable()  &&
                                    pSNd->GetSection().IsProtectFlag() ))
                            {
                                bOk = TRUE;
                                break;		// eine nicht geschuetzte Zelle gef.
                            }
                            continue;		// dann weiter suchen
                        }
                    }
                    else
                    {
                        bOk = TRUE;
                        break;		// eine nicht geschuetzte Zelle gef.
                    }
                }

                if( bOk && rNdIdx.GetIndex() < rNds.GetEndOfExtras().GetIndex() )
                {
                    // Teste mal auf Fly - kann auch noch geschuetzt sein!!
                    if( 0 == (pFrm = pCNd->GetFrm(0,0,FALSE)) ||
                        ( !IsReadOnlyAvailable() && pFrm->IsProtected() ) ||
                        ( bOnlyText && pCNd->IsNoTxtNode() ) )
                    {
                        // dann weiter suchen!
                        bOk = FALSE;
                        bWeiter = TRUE;
                    }
                }
            } while( bWeiter );

            if( !bOk )
            {
                if( !nLoopCnt )
                    fnGoSection = &SwNodes::GoPrevSection;
                rNdIdx = nNdIdx;
            }
        }
    }
    if( bOk )
    {
        pCNd = rNdIdx.GetNode().GetCntntNode();
//		USHORT nCntnt = Min( pCNd->Len(), pCurCrsr->GetPoint()->nContent.GetIndex() );
        xub_StrLen nCntnt = rNdIdx.GetIndex() < nNdIdx ? pCNd->Len() : 0;
        pCurCrsr->GetPoint()->nContent.Assign( pCNd, nCntnt );
    }
    else
    {
        pCNd = rNdIdx.GetNode().GetCntntNode();

        // falls Cursor im versteckten Bereich ist, auf jedenfall schon mal
        // verschieben!!
        if( !pCNd || !pCNd->GetFrm(0,0,FALSE) )
        {
            SwCrsrMoveState aTmpState( MV_NONE );
            aTmpState.bSetInReadOnly = IsReadOnlyAvailable();
            GetLayout()->GetCrsrOfst( pCurCrsr->GetPoint(), pCurCrsr->GetPtPos(),
                                        &aTmpState );
        }
    }
    return bOk;
}


void SwCrsrShell::NewCoreSelection()
{
}


FASTBOOL SwCrsrShell::IsCrsrReadonly() const
{
    if ( GetViewOptions()->IsReadonly() ||
         // --> FME 2004-06-29 #114856# Formular view
         GetViewOptions()->IsFormView() )
         // <--
    {
        SwFrm *pFrm = GetCurrFrm( FALSE );
        const SwFlyFrm* pFly;
        const SwSection* pSection;

        if( pFrm && pFrm->IsInFly() &&
             (pFly = pFrm->FindFlyFrm())->GetFmt()->GetEditInReadonly().GetValue() &&
             pFly->Lower() &&
             !pFly->Lower()->IsNoTxtFrm() &&
             !GetDrawView()->GetMarkedObjectList().GetMarkCount() )
        {
            return FALSE;
        }
        // --> FME 2004-06-22 #114856# edit in readonly sections
        else if ( pFrm && pFrm->IsInSct() &&
                  0 != ( pSection = pFrm->FindSctFrm()->GetSection() ) &&
                  pSection->IsEditInReadonlyFlag() )
        {
            return FALSE;
        }
        // <--

        return TRUE;
    }
    return FALSE;
}


// darf der Cursor in ReadOnlyBereiche?
void SwCrsrShell::SetReadOnlyAvailable( BOOL bFlag )
{
    // im GlobalDoc darf NIE umgeschaltet werden
    if( (!GetDoc()->GetDocShell() ||
         !GetDoc()->GetDocShell()->IsA( SwGlobalDocShell::StaticType() )) &&
        bFlag != bSetCrsrInReadOnly )
    {
        // wenn das Flag ausgeschaltet wird, dann muessen erstmal alle
        // Selektionen aufgehoben werden. Denn sonst wird sich darauf
        // verlassen, das nichts geschuetztes selektiert ist!
        if( !bFlag )
        {
            ClearMark();
        }
        bSetCrsrInReadOnly = bFlag;
        UpdateCrsr();
    }
}

FASTBOOL SwCrsrShell::HasReadonlySel() const
{
    FASTBOOL bRet = FALSE;
    if( IsReadOnlyAvailable() ||
        // --> FME 2004-06-29 #114856# Formular view
        GetViewOptions()->IsFormView() )
        // <--
    {
        if( pTblCrsr )
            bRet = pTblCrsr->HasReadOnlyBoxSel() ||
                   pTblCrsr->HasReadonlySel(
                            // --> FME 2004-06-29 #114856# Formular view
                            GetViewOptions()->IsFormView() );
                            // <--
        else
        {
            const SwPaM* pCrsr = pCurCrsr;

            do {
                if( pCrsr->HasReadonlySel(
                        // --> FME 2004-06-29 #114856# Formular view
                        GetViewOptions()->IsFormView() ) )
                        // <--
                    bRet = TRUE;
            } while( !bRet && pCurCrsr != ( pCrsr = (SwPaM*)pCrsr->GetNext() ));
        }
    }
    return bRet;
}

// SwCursor - Methode !!!!
FASTBOOL SwCursor::IsReadOnlyAvailable() const
{
    const SwShellCrsr* pShCrsr = *this;
    const SwUnoCrsr* pUnoCrsr = *this;
    return pShCrsr ? pShCrsr->GetShell()->IsReadOnlyAvailable() :
        pUnoCrsr ? TRUE : FALSE;
}


FASTBOOL SwCrsrShell::IsSelFullPara() const
{
    FASTBOOL bRet = FALSE;

    if( pCurCrsr->GetPoint()->nNode.GetIndex() ==
        pCurCrsr->GetMark()->nNode.GetIndex() && pCurCrsr == pCurCrsr->GetNext() )
    {
        xub_StrLen nStt = pCurCrsr->GetPoint()->nContent.GetIndex(),
                   nEnd = pCurCrsr->GetMark()->nContent.GetIndex();
        if( nStt > nEnd )
        {
            xub_StrLen nTmp = nStt;
            nStt = nEnd;
            nEnd = nTmp;
        }
        const SwCntntNode* pCNd = pCurCrsr->GetCntntNode();
        bRet = pCNd && !nStt && nEnd == pCNd->Len();
    }
    return bRet;
}

short SwCrsrShell::GetTextDirection( const Point* pPt ) const
{
    SwPosition aPos( *pCurCrsr->GetPoint() );
    Point aPt( pPt ? *pPt : pCurCrsr->GetPtPos() );
    if( pPt )
    {
        SwCrsrMoveState aTmpState( MV_NONE );
        aTmpState.bSetInReadOnly = IsReadOnlyAvailable();

        GetLayout()->GetCrsrOfst( &aPos, aPt, &aTmpState );
    }

    return pDoc->GetTextDirection( aPos, &aPt );
}

FASTBOOL SwCrsrShell::IsInVerticalText( const Point* pPt ) const
{
    const short nDir = GetTextDirection( pPt );
    return FRMDIR_VERT_TOP_RIGHT == nDir || FRMDIR_VERT_TOP_LEFT == nDir;
}

FASTBOOL SwCrsrShell::IsInRightToLeftText( const Point* pPt ) const
{
    const short nDir = GetTextDirection( pPt );
    // GetTextDirection uses FRMDIR_VERT_TOP_LEFT to indicate RTL in
    // vertical environment
    return FRMDIR_VERT_TOP_LEFT == nDir || FRMDIR_HORI_RIGHT_TOP == nDir;
}

//
// If the current cursor position is inside a hidden range, the hidden range
// is selected:
//
bool SwCrsrShell::SelectHiddenRange()
{
    bool bRet = false;
    if ( !GetViewOptions()->IsShowHiddenChar() && !pCurCrsr->HasMark() )
    {
        SwPosition& rPt = *(SwPosition*)pCurCrsr->GetPoint();
        const SwTxtNode* pNode = rPt.nNode.GetNode().GetTxtNode();
        if ( pNode )
        {
            const xub_StrLen nPos = rPt.nContent.GetIndex();

            // check if nPos is in hidden range
            xub_StrLen nHiddenStart;
            xub_StrLen nHiddenEnd;
            SwScriptInfo::GetBoundsOfHiddenRange( *pNode, nPos, nHiddenStart, nHiddenEnd );
            if ( STRING_LEN != nHiddenStart )
            {
                // make selection:
                pCurCrsr->SetMark();
                pCurCrsr->GetMark()->nContent = nHiddenEnd;
                bRet = true;
            }
        }
    }

    return bRet;
}

/*  */

    // die Suchfunktionen
ULONG SwCrsrShell::Find( const SearchOptions& rSearchOpt,
                            SwDocPositions eStart, SwDocPositions eEnde,
                            BOOL& bCancel,
                            FindRanges eRng, int bReplace )
{
    if( pTblCrsr )
        GetCrsr();
    delete pTblCrsr, pTblCrsr = 0;
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    ULONG nRet = pCurCrsr->Find( rSearchOpt, eStart, eEnde, bCancel, eRng, bReplace );
    if( nRet || bCancel )
        UpdateCrsr();
    return nRet;
}

ULONG SwCrsrShell::Find( const SwTxtFmtColl& rFmtColl,
                            SwDocPositions eStart, SwDocPositions eEnde,
                            BOOL& bCancel,
                            FindRanges eRng, const SwTxtFmtColl* pReplFmt )
{
    if( pTblCrsr )
        GetCrsr();
    delete pTblCrsr, pTblCrsr = 0;
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    ULONG nRet = pCurCrsr->Find( rFmtColl, eStart, eEnde, bCancel, eRng, pReplFmt );
    if( nRet )
        UpdateCrsr();
    return nRet;
}

ULONG SwCrsrShell::Find( const SfxItemSet& rSet, FASTBOOL bNoCollections,
                            SwDocPositions eStart, SwDocPositions eEnde,
                            BOOL& bCancel,
                            FindRanges eRng, const SearchOptions* pSearchOpt,
                            const SfxItemSet* rReplSet )
{
    if( pTblCrsr )
        GetCrsr();
    delete pTblCrsr, pTblCrsr = 0;
    SwCallLink aLk( *this );        // Crsr-Moves ueberwachen, evt. Link callen
    ULONG nRet = pCurCrsr->Find( rSet, bNoCollections, eStart, eEnde, bCancel,
                                eRng, pSearchOpt, rReplSet );
    if( nRet )
        UpdateCrsr();
    return nRet;
}

void SwCrsrShell::SetSelection( const SwPaM& rCrsr )
{
    StartAction();
    SwPaM* pCrsr = GetCrsr();
    *pCrsr->GetPoint() = *rCrsr.GetPoint();
    if(rCrsr.HasMark())
    {
        pCrsr->SetMark();
        *pCrsr->GetMark() = *rCrsr.GetMark();
    }
    if((SwPaM*)rCrsr.GetNext() != &rCrsr)
    {
        const SwPaM *_pStartCrsr = (SwPaM*)rCrsr.GetNext();
        do
        {
            SwPaM* pCurCrsr = CreateCrsr();
            *pCurCrsr->GetPoint() = *_pStartCrsr->GetPoint();
            if(_pStartCrsr->HasMark())
            {
                pCurCrsr->SetMark();
                *pCurCrsr->GetMark() = *_pStartCrsr->GetMark();
            }
        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != &rCrsr );
    }
    EndAction();
}

/*  */

#if !defined(PRODUCT) || defined(WIN)

void SwCrsrShell::SetMark()
{
    pCurCrsr->SetMark();
}

FASTBOOL SwCrsrShell::HasMark()
{
    return pCurCrsr->HasMark();
}

SwCursor* SwCrsrShell::GetSwCrsr( FASTBOOL bMakeTblCrsr ) const
{
    return (SwCursor*)GetCrsr( bMakeTblCrsr );
}

// gebe den Stack Cursor zurueck
SwPaM * SwCrsrShell::GetStkCrsr() const			{ return pCrsrStk; }

// gebe den TabellenCrsr zurueck
const	SwPaM* SwCrsrShell::GetTblCrs() const	{ return pTblCrsr; }
        SwPaM* SwCrsrShell::GetTblCrs()			{ return pTblCrsr; }

// Abfrage, ob ueberhaupt eine Selektion existiert, sprich der akt. Cursor
// aufgespannt oder nicht der einzigste ist.

FASTBOOL SwCrsrShell::IsSelection() const
{
    return IsTableMode() || pCurCrsr->HasMark() ||
            pCurCrsr->GetNext() != pCurCrsr;
}
// returns if multiple cursors are available
FASTBOOL SwCrsrShell::IsMultiSelection() const
{
    return pCurCrsr->GetNext() != pCurCrsr;
}

// pruefe ob vom aktuellen Crsr der SPoint/Mark in einer Tabelle stehen
const SwTableNode* SwCrsrShell::IsCrsrInTbl( BOOL bIsPtInTbl ) const
{
    return pCurCrsr->GetNode( bIsPtInTbl )->FindTableNode();
}


FASTBOOL SwCrsrShell::IsCrsrPtAtEnd() const
{
    return pCurCrsr->End() == pCurCrsr->GetPoint();
}


Point& SwCrsrShell::GetCrsrDocPos( BOOL bPoint ) const
{
    return bPoint ? pCurCrsr->GetPtPos() : pCurCrsr->GetMkPos();
}


void SwCrsrShell::UnSetVisCrsr()
{
    pVisCrsr->Hide();
    pVisCrsr->SetDragCrsr( FALSE );
}


FASTBOOL SwCrsrShell::IsSelOnePara() const
{
    return pCurCrsr == pCurCrsr->GetNext() &&
           pCurCrsr->GetPoint()->nNode ==
           pCurCrsr->GetMark()->nNode;
}

SwMoveFnCollection* SwCrsrShell::MakeFindRange(
                            USHORT nStt, USHORT nEnd, SwPaM* pPam ) const
{
    return pCurCrsr->MakeFindRange( (SwDocPositions)nStt,
                                    (SwDocPositions)nEnd, pPam );
}
#endif

/**
   Checks if a position is valid. To be valid the position's node must
   be a content node and the content must not be unregistered.

   @param aPos the position to check.
*/
static bool lcl_PosOk(SwPosition & aPos)
{
    bool bResult = true;
    SwPosition aTmpPos(aPos);
    aTmpPos.nContent.Assign(0, 0);

    if (aPos.nNode.GetNode().GetCntntNode() == NULL ||
        aPos.nContent.GetIdxReg() == aTmpPos.nContent.GetIdxReg())
        bResult = false;

    return bResult;
}

/**
   Checks if a PaM is valid. For a PaM to be valid its point must be
   valid. Additionaly if the PaM has a mark this has to be valid, too.

   @param aPam the PaM to check
*/
static bool lcl_CrsrOk(SwPaM & aPam)
{
    return lcl_PosOk(*aPam.GetPoint()) && (! aPam.HasMark()
        || lcl_PosOk(*aPam.GetMark()));
}

void SwCrsrShell::ClearUpCrsrs()
{
    // start of the ring
    SwPaM * pStartCrsr = GetCrsr();
    // start loop with second entry of the ring
    SwPaM * pCrsr = (SwPaM *) pStartCrsr->GetNext();
    SwPaM * pTmpCrsr;
    bool bChanged = false;

    /*
       For all entries in the ring except the start entry delete the
       entry if it is invalid.
    */
    while (pCrsr != pStartCrsr)
    {
        pTmpCrsr = (SwPaM *) pCrsr->GetNext();

        if ( ! lcl_CrsrOk(*pCrsr))
        {
            delete pCrsr;

            bChanged = true;
        }

        pCrsr = pTmpCrsr;
    }

    /*
      If the start entry of the ring is invalid replace it with a
      cursor pointing to the beginning of the first content node in
      the document.
    */
    if (! lcl_CrsrOk(*pStartCrsr))
    {
        SwNodes & aNodes = GetDoc()->GetNodes();
        SwNodeIndex aIdx(*(aNodes.GetEndOfContent().StartOfSectionNode()));

        SwNode * pNode = aNodes.GoNext(&aIdx);
        bool bFound = (pNode != NULL);

        ASSERT(bFound, "no content node found");

        if (bFound)
        {
            SwPaM aTmpPam(*pNode);
            *pStartCrsr = aTmpPam;
        }

        bChanged = true;
    }

    /*
      If at least one of the cursors in the ring have been deleted or
      replaced, remove the table cursor.
    */
    if (pTblCrsr != NULL && bChanged)
        TblCrsrToCursor();
}

// #111827#
String SwCrsrShell::GetCrsrDescr() const
{
    String aResult;

    if (IsMultiSelection())
        aResult += String(SW_RES(STR_MULTISEL));
    else
        aResult = GetDoc()->GetPaMDescr(*GetCrsr());

    return aResult;
}
