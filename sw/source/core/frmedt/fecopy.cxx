/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fecopy.cxx,v $
 * $Revision: 1.52 $
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

#ifdef WIN
#define NEEDED_BY_FESHVIEW
#endif

#ifndef _GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#include <sot/formats.hxx>
#include <sot/storage.hxx>
#include <svtools/pathoptions.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewsh.hxx>
#include <svx/xexch.hxx>
#include <svx/xflasit.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xflclit.hxx>
#include <svx/brshitem.hxx>
#include <svx/svdocapt.hxx>
#include <svx/svdouno.hxx>
#include <svx/xfillit.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdogrp.hxx>
#include <svx/xoutbmp.hxx>
#include <svx/svdoole2.hxx>
#ifndef _FM_FMMODEL_HXX
#include <svx/fmmodel.hxx>
#endif
#include <svx/unomodel.hxx>
// --> OD 2005-08-03 #i50824#
#include <svx/svditer.hxx>
// <--
// --> OD 2006-03-01 #b6382898#
#include <svx/svdograf.hxx>
// <--
#include <unotools/streamwrap.hxx>
#include <fmtanchr.hxx>
#include <fmtcntnt.hxx>
#include <fmtornt.hxx>
#include <fmtflcnt.hxx>
#include <frmfmt.hxx>
#include <docary.hxx>
#include <txtfrm.hxx>
#include <txtflcnt.hxx>
#include <fesh.hxx>
#include <doc.hxx>
#include <rootfrm.hxx>
#include <ndtxt.hxx>
#include <pam.hxx>
#include <tblsel.hxx>
#include <swtable.hxx>
#include <flyfrm.hxx>
#include <pagefrm.hxx>
#include <fldbas.hxx>
#include <edimp.hxx>
#include <swundo.hxx>
#include <viewimp.hxx>
#include <dview.hxx>
#include <dcontact.hxx>
#include <dflyobj.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <pagedesc.hxx>
#include <mvsave.hxx>
#include <vcl/virdev.hxx>


using namespace ::com::sun::star;

/*************************************************************************
|*
|*	SwFEShell::Copy()	Copy fuer das Interne Clipboard.
|*		Kopiert alle Selektionen in das Clipboard.
|*
|*	Ersterstellung		JP ??
|*	Letzte Aenderung	MA 22. Feb. 95
|
|*************************************************************************/

BOOL SwFEShell::Copy( SwDoc* pClpDoc, const String* pNewClpTxt )
{
    ASSERT( pClpDoc, "kein Clipboard-Dokument"	);

    pClpDoc->DoUndo( FALSE );		// immer auf FALSE !!

    // steht noch Inhalt im ClpDocument, dann muss dieser geloescht werden
    SwNodeIndex aSttIdx( pClpDoc->GetNodes().GetEndOfExtras(), 2 );
    SwTxtNode* pTxtNd = aSttIdx.GetNode().GetTxtNode();
    if( !pTxtNd || pTxtNd->GetTxt().Len() ||
        aSttIdx.GetIndex()+1 != pClpDoc->GetNodes().GetEndOfContent().GetIndex() )
    {
        pClpDoc->GetNodes().Delete( aSttIdx,
            pClpDoc->GetNodes().GetEndOfContent().GetIndex() - aSttIdx.GetIndex() );
        pTxtNd = pClpDoc->GetNodes().MakeTxtNode( aSttIdx,
                            (SwTxtFmtColl*)pClpDoc->GetDfltTxtFmtColl() );
        aSttIdx--;
    }

    // stehen noch FlyFrames rum, loesche auch diese
    for( USHORT n = 0; n < pClpDoc->GetSpzFrmFmts()->Count(); ++n )
    {
        SwFlyFrmFmt* pFly = (SwFlyFrmFmt*)(*pClpDoc->GetSpzFrmFmts())[n];
        pClpDoc->DelLayoutFmt( pFly );
    }
    pClpDoc->GCFieldTypes();		// loesche die FieldTypes

    // wurde ein String uebergeben, so kopiere diesen in das Clipboard-
    // Dokument. Somit kann auch der Calculator das interne Clipboard
    // benutzen.
    if( pNewClpTxt )
    {
        pTxtNd->Insert( *pNewClpTxt, SwIndex( pTxtNd ) );
        return TRUE;				// das wars.
    }

    pClpDoc->LockExpFlds();
    pClpDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_DELETE_REDLINES );
    BOOL bRet;

    // soll ein FlyFrame kopiert werden ?
    if( IsFrmSelected() )
    {
        // hole das FlyFormat
        SwFlyFrm* pFly = FindFlyFrm();
        SwFrmFmt* pFlyFmt = pFly->GetFmt();
        SwFmtAnchor aAnchor( pFlyFmt->GetAnchor() );

        if ( FLY_AT_CNTNT == aAnchor.GetAnchorId() ||
             FLY_AUTO_CNTNT == aAnchor.GetAnchorId() ||
             FLY_AT_FLY == aAnchor.GetAnchorId() ||
             FLY_IN_CNTNT == aAnchor.GetAnchorId() )
        {
            SwPosition aPos( aSttIdx );
            if( FLY_IN_CNTNT == aAnchor.GetAnchorId() )
                aPos.nContent.Assign( pTxtNd, 0 );
            aAnchor.SetAnchor( &aPos );
        }
        pFlyFmt = pClpDoc->CopyLayoutFmt( *pFlyFmt, aAnchor, true, true );

        // sorge dafuer das das "RootFmt" als erstes im SpzArray-steht
        // (Es wurden ggf. Flys in Flys kopiert.
        SwSpzFrmFmts& rSpzFrmFmts = *(SwSpzFrmFmts*)pClpDoc->GetSpzFrmFmts();
        if( rSpzFrmFmts[ 0 ] != pFlyFmt )
        {
            USHORT nPos = rSpzFrmFmts.GetPos( pFlyFmt );
            ASSERT( nPos != USHRT_MAX, "Fly steht nicht im Spz-Array" );

            rSpzFrmFmts.Remove( nPos );
            rSpzFrmFmts.Insert( pFlyFmt, 0 );
        }

        if( FLY_IN_CNTNT == aAnchor.GetAnchorId() )
        {
            // JP 13.02.99 Bug 61863: wenn eine Rahmenselektion ins Clipboard
            //				gestellt wird, so muss beim Pasten auch wieder
            //				eine solche vorgefunden werden. Also muss im Node
            //				das kopierte TextAttribut wieder entfernt werden,
            //				sonst wird es als TextSelektion erkannt
            const SwIndex& rIdx = pFlyFmt->GetAnchor().GetCntntAnchor()->nContent;
            SwTxtFlyCnt* pTxtFly = (SwTxtFlyCnt*)pTxtNd->GetTxtAttr(
                                                rIdx, RES_TXTATR_FLYCNT );
            if( pTxtFly )
            {
                ((SwFmtFlyCnt&)pTxtFly->GetFlyCnt()).SetFlyFmt( 0 );
                pTxtNd->Erase( rIdx, 1 );
            }
        }
        bRet = TRUE;
    }
    else if ( IsObjSelected() )
    {
        SwPosition aPos( aSttIdx, SwIndex( pTxtNd, 0 ));
        const SdrMarkList &rMrkList = Imp()->GetDrawView()->GetMarkedObjectList();
        for ( USHORT i = 0; i < rMrkList.GetMarkCount(); ++i )
        {
            SdrObject *pObj = rMrkList.GetMark( i )->GetMarkedSdrObj();

            if( Imp()->GetDrawView()->IsGroupEntered() ||
                ( !pObj->GetUserCall() && pObj->GetUpGroup()) )
            {
                SfxItemSet aSet( pClpDoc->GetAttrPool(), aFrmFmtSetRange );

                SwFmtAnchor aAnchor( FLY_AT_CNTNT );
                aAnchor.SetAnchor( &aPos );
                aSet.Put( aAnchor );

                SdrObject* pNew = pClpDoc->CloneSdrObj( *pObj, FALSE, TRUE );

                SwPaM aTemp(aPos);
                   pClpDoc->Insert(aTemp, *pNew, &aSet, NULL);
            }
            else
            {
                SwDrawContact *pContact = (SwDrawContact*)GetUserCall( pObj );
                SwFrmFmt *pFmt = pContact->GetFmt();
                SwFmtAnchor aAnchor( pFmt->GetAnchor() );
                if ( FLY_AT_CNTNT == aAnchor.GetAnchorId() ||
                     FLY_AUTO_CNTNT == aAnchor.GetAnchorId() ||
                     FLY_AT_FLY == aAnchor.GetAnchorId() ||
                     FLY_IN_CNTNT == aAnchor.GetAnchorId() )
                {
                    aAnchor.SetAnchor( &aPos );
                }

                pClpDoc->CopyLayoutFmt( *pFmt, aAnchor, true, true );
            }
        }
        bRet = TRUE;
    }
    else
        bRet = _CopySelToDoc( pClpDoc, 0 );		// kopiere die Selectionen

    pClpDoc->SetRedlineMode_intern((RedlineMode_t)0 );
    pClpDoc->UnlockExpFlds();
    if( !pClpDoc->IsExpFldsLocked() )
        pClpDoc->UpdateExpFlds(NULL, true);

    return bRet;
}

const Point &lcl_FindBasePos( const SwFrm *pFrm, const Point &rPt )
{
    const SwFrm *pF = pFrm;
    while ( pF && !pF->Frm().IsInside( rPt ) )
    {
        if ( pF->IsCntntFrm() )
            pF = ((SwCntntFrm*)pF)->GetFollow();
        else
            pF = 0;
    }
    if ( pF )
        return pF->Frm().Pos();
    else
        return pFrm->Frm().Pos();
}

BOOL lcl_SetAnchor( const SwPosition& rPos, const SwNode& rNd, SwFlyFrm* pFly,
                const Point& rInsPt, SwFEShell& rDestShell, SwFmtAnchor& rAnchor,
                Point& rNewPos, BOOL bCheckFlyRecur )
{
    BOOL bRet = TRUE;
    rAnchor.SetAnchor( &rPos );
    SwCntntFrm* pTmpFrm = rNd.GetCntntNode()->GetFrm( &rInsPt, 0, FALSE );
    SwFlyFrm *pTmpFly = pTmpFrm->FindFlyFrm();
    if( pTmpFly && bCheckFlyRecur && pFly->IsUpperOf( *pTmpFly ) )
        bRet = FALSE;
    else if ( FLY_AT_FLY == rAnchor.GetAnchorId() )
    {
        if( pTmpFly )
        {
            const SwNodeIndex& rIdx = *pTmpFly->GetFmt()->GetCntnt().GetCntntIdx();
            SwPosition aPos( rIdx );
            rAnchor.SetAnchor( &aPos );
            rNewPos = pTmpFly->Frm().Pos();
        }
        else
        {
            rAnchor.SetType( FLY_PAGE );
            rAnchor.SetPageNum( rDestShell.GetPageNumber( rInsPt ) );
            const SwFrm *pPg = pTmpFrm->FindPageFrm();
            rNewPos = pPg->Frm().Pos();
        }
    }
    else
        rNewPos = ::lcl_FindBasePos( pTmpFrm, rInsPt );
    return bRet;
}

BOOL SwFEShell::CopyDrawSel( SwFEShell* pDestShell, const Point& rSttPt,
                    const Point& rInsPt, BOOL bIsMove, BOOL bSelectInsert )
{
    BOOL bRet = TRUE;

    //Die Liste muss kopiert werden, weil unten die neuen Objekte
    //selektiert werden.
    const SdrMarkList aMrkList( Imp()->GetDrawView()->GetMarkedObjectList() );
    ULONG nMarkCount = aMrkList.GetMarkCount();
    if( !pDestShell->Imp()->GetDrawView() )
        // sollte mal eine erzeugt werden
        pDestShell->MakeDrawView();
    else if( bSelectInsert )
        pDestShell->Imp()->GetDrawView()->UnmarkAll();

    SdrPageView *pDestPgView = pDestShell->Imp()->GetPageView(),
                *pSrcPgView = Imp()->GetPageView();
    SwDrawView *pDestDrwView = pDestShell->Imp()->GetDrawView(),
                *pSrcDrwView = Imp()->GetDrawView();
    SwDoc* pDestDoc = pDestShell->GetDoc();

    Size aSiz( rInsPt.X() - rSttPt.X(), rInsPt.Y() - rSttPt.Y() );
    for( USHORT i = 0; i < nMarkCount; ++i )
    {
        SdrObject *pObj = aMrkList.GetMark( i )->GetMarkedSdrObj();

        SwDrawContact *pContact = (SwDrawContact*)GetUserCall( pObj );
        SwFrmFmt *pFmt = pContact->GetFmt();
        const SwFmtAnchor& rAnchor = pFmt->GetAnchor();

        BOOL bInsWithFmt = TRUE;

        if( pDestDrwView->IsGroupEntered() )
        {
            // in die Gruppe einfuegen, wenns aus einer betretenen Gruppe
            // kommt oder das Object nicht zeichengebunden ist
            if( pSrcDrwView->IsGroupEntered() ||
                FLY_IN_CNTNT != rAnchor.GetAnchorId() )

            {
                SdrObject* pNew = pDestDoc->CloneSdrObj( *pObj, bIsMove &&
                                        GetDoc() == pDestDoc, FALSE );
                pNew->NbcMove( aSiz );
                pDestDrwView->InsertObjectAtView( pNew, *pDestPgView );
                bInsWithFmt = FALSE;
            }
        }

        if( bInsWithFmt )
        {
            SwFmtAnchor aAnchor( rAnchor );
            Point aNewAnch;

            if ( FLY_AT_CNTNT == aAnchor.GetAnchorId() ||
                    FLY_AUTO_CNTNT == aAnchor.GetAnchorId() ||
                    FLY_AT_FLY == aAnchor.GetAnchorId() ||
                    FLY_IN_CNTNT == aAnchor.GetAnchorId() )
            {
                if ( this == pDestShell )
                {
                    //gleiche Shell? Dann erfrage die Position an der
                    //uebergebenen DokumentPosition
                    SwPosition aPos( *GetCrsr()->GetPoint() );
                    Point aPt( rInsPt );
                    aPt -= rSttPt - pObj->GetSnapRect().TopLeft();
                    SwCrsrMoveState aState( MV_SETONLYTEXT );
                    GetLayout()->GetCrsrOfst( &aPos, aPt, &aState );
                    const SwNode *pNd;
                    if( (pNd = &aPos.nNode.GetNode())->IsNoTxtNode() )
                        bRet = FALSE;
                    else
                        bRet = ::lcl_SetAnchor( aPos, *pNd, 0, rInsPt,
                                *pDestShell, aAnchor, aNewAnch, FALSE );
                }
                else
                {
                    SwPaM *pCrsr = pDestShell->GetCrsr();
                    if( pCrsr->GetNode()->IsNoTxtNode() )
                        bRet = FALSE;
                    else
                        bRet = ::lcl_SetAnchor( *pCrsr->GetPoint(),
                                                *pCrsr->GetNode(), 0, rInsPt,
                                                *pDestShell, aAnchor,
                                                aNewAnch, FALSE );
                }
            }
            else if( FLY_PAGE == aAnchor.GetAnchorId() )
            {
                aAnchor.SetPageNum( pDestShell->GetPageNumber( rInsPt ) );
                const SwRootFrm* pTmpRoot = pDestShell->GetLayout();
                const SwFrm* pPg = pTmpRoot->GetPageAtPos( rInsPt, 0, true );
                if ( pPg )
                    aNewAnch = pPg->Frm().Pos();
            }

            if( bRet )
            {
                if( pSrcDrwView->IsGroupEntered() ||
                    ( !pObj->GetUserCall() && pObj->GetUpGroup()) )
                {
                    SfxItemSet aSet( pDestDoc->GetAttrPool(),aFrmFmtSetRange);
                    aSet.Put( aAnchor );
                    SdrObject* pNew = pDestDoc->CloneSdrObj( *pObj, bIsMove &&
                                                GetDoc() == pDestDoc, TRUE );
                    pFmt = pDestDoc->Insert( *pDestShell->GetCrsr(),
                                            *pNew, &aSet, NULL );
                }
                else
                    pFmt = pDestDoc->CopyLayoutFmt( *pFmt, aAnchor, true, true );

                //Kann 0 sein, weil Draws in Kopf-/Fusszeilen nicht erlaubt sind.
                if ( pFmt )
                {
                    SdrObject* pNew = pFmt->FindSdrObject();
                    if( FLY_IN_CNTNT != aAnchor.GetAnchorId() )
                    {
                        Point aPos( rInsPt );
                        aPos -= aNewAnch;
                        aPos -= rSttPt - pObj->GetSnapRect().TopLeft();
                        // OD 2004-04-05 #i26791# - change attributes instead of
                        // direct positioning
                        pFmt->SetAttr( SwFmtHoriOrient( aPos.X(), text::HoriOrientation::NONE, text::RelOrientation::FRAME ) );
                        pFmt->SetAttr( SwFmtVertOrient( aPos.Y(), text::VertOrientation::NONE, text::RelOrientation::FRAME ) );
                        // --> OD 2005-04-15 #i47455# - notify draw frame format
                        // that position attributes are already set.
                        if ( pFmt->ISA(SwDrawFrmFmt) )
                        {
                            static_cast<SwDrawFrmFmt*>(pFmt)->PosAttrSet();
                        }
                        // <--
                    }
                    if( bSelectInsert )
                        pDestDrwView->MarkObj( pNew, pDestPgView );
                }
            }
        }
    }

    if ( bIsMove && bRet )
    {
        if( pDestShell == this )
        {
            const SdrMarkList aList( pSrcDrwView->GetMarkedObjectList() );
            pSrcDrwView->UnmarkAll();

            ULONG nMrkCnt = aMrkList.GetMarkCount();
            USHORT i;
            for ( i = 0; i < nMrkCnt; ++i )
            {
                SdrObject *pObj = aMrkList.GetMark( i )->GetMarkedSdrObj();
                pSrcDrwView->MarkObj( pObj, pSrcPgView );
            }
            DelSelectedObj();
            nMrkCnt = aList.GetMarkCount();
            for ( i = 0; i < nMrkCnt; ++i )
            {
                SdrObject *pObj = aList.GetMark( i )->GetMarkedSdrObj();
                pSrcDrwView->MarkObj( pObj, pSrcPgView );
            }
        }
        else
            DelSelectedObj();
    }

    return bRet;
}

BOOL SwFEShell::Copy( SwFEShell* pDestShell, const Point& rSttPt,
                    const Point& rInsPt, BOOL bIsMove, BOOL bSelectInsert )
{
    BOOL bRet = FALSE;

    ASSERT( pDestShell, "Copy ohne DestShell." );
    ASSERT( this == pDestShell || !pDestShell->IsObjSelected(),
            "Dest-Shell darf nie im Obj-Modus sein" );

    SET_CURR_SHELL( pDestShell );

    pDestShell->StartAllAction();
    pDestShell->GetDoc()->LockExpFlds();

    // Referenzen sollen verschoben werden.
    BOOL bCopyIsMove = pDoc->IsCopyIsMove();
    if( bIsMove )
        // am Doc ein Flag setzen, damit in den TextNodes
        pDoc->SetCopyIsMove( TRUE );

    RedlineMode_t eOldRedlMode = pDestShell->GetDoc()->GetRedlineMode();
    pDestShell->GetDoc()->SetRedlineMode_intern( (RedlineMode_t)(eOldRedlMode | nsRedlineMode_t::REDLINE_DELETE_REDLINES));

    // sind Tabellen-Formeln im Bereich, dann muss erst die Tabelle
    // angezeigt werden, damit die Tabellen-Formel den neuen Wert errechnen
    // kann (bei Bereichen wird sich ueber das Layout die einzelnen Boxen
    // besorgt)
    SwFieldType* pTblFldTyp = pDestShell->GetDoc()->GetSysFldType( RES_TABLEFLD );

    if( IsFrmSelected() )
    {
        SwFlyFrm* pFly = FindFlyFrm();
        SwFrmFmt* pFlyFmt = pFly->GetFmt();
        SwFmtAnchor aAnchor( pFlyFmt->GetAnchor() );
        bRet = TRUE;
        Point aNewAnch;

        if ( FLY_AT_CNTNT == aAnchor.GetAnchorId() ||
             FLY_AUTO_CNTNT == aAnchor.GetAnchorId() ||
             FLY_AT_FLY == aAnchor.GetAnchorId() ||
             FLY_IN_CNTNT == aAnchor.GetAnchorId() )
        {
            if ( this == pDestShell )
            {
                // gleiche Shell? Dann erfrage die Position an der
                // uebergebenen DokumentPosition
                SwPosition aPos( *GetCrsr()->GetPoint() );
                Point aPt( rInsPt );
                aPt -= rSttPt - pFly->Frm().Pos();
                SwCrsrMoveState aState( MV_SETONLYTEXT );
                GetLayout()->GetCrsrOfst( &aPos, aPt, &aState );
                const SwNode *pNd;
                if( (pNd = &aPos.nNode.GetNode())->IsNoTxtNode() )
                    bRet = FALSE;
                else
                {	//Nicht in sich selbst kopieren
                    const SwNodeIndex *pTmp = pFlyFmt->GetCntnt().GetCntntIdx();
                    if ( aPos.nNode > *pTmp && aPos.nNode <
                        pTmp->GetNode().EndOfSectionIndex() )
                    {
                        bRet = FALSE;
                    }
                    else
                        bRet = ::lcl_SetAnchor( aPos, *pNd, pFly, rInsPt,
                                        *pDestShell, aAnchor, aNewAnch, TRUE );
                }
            }
            else
            {
                const SwPaM *pCrsr = pDestShell->GetCrsr();
                if( pCrsr->GetNode()->IsNoTxtNode() )
                    bRet = FALSE;
                else
                    bRet = ::lcl_SetAnchor( *pCrsr->GetPoint(), *pCrsr->GetNode(),
                                            pFly, rInsPt, *pDestShell, aAnchor,
                                    aNewAnch, GetDoc() == pDestShell->GetDoc());
            }
        }
        else if( FLY_PAGE == aAnchor.GetAnchorId() )
        {
            aAnchor.SetPageNum( pDestShell->GetPageNumber( rInsPt ) );
            const SwRootFrm* pTmpRoot = pDestShell->GetLayout();
            const SwFrm* pPg = pTmpRoot->GetPageAtPos( rInsPt, 0, true );
            if ( pPg )
                aNewAnch = pPg->Frm().Pos();
        }
        else {
            ASSERT( !this, "was fuer ein Anchor ist es denn?" );
        }

        if( bRet )
        {
            SwFrmFmt *pOldFmt = pFlyFmt;
            pFlyFmt = pDestShell->GetDoc()->CopyLayoutFmt( *pFlyFmt, aAnchor, true, true );

            if( FLY_IN_CNTNT != aAnchor.GetAnchorId() )
            {
                Point aPos( rInsPt );
                aPos -= aNewAnch;
                aPos -= rSttPt - pFly->Frm().Pos();
                pFlyFmt->SetAttr( SwFmtHoriOrient( aPos.X(),text::HoriOrientation::NONE, text::RelOrientation::FRAME ) );
                pFlyFmt->SetAttr( SwFmtVertOrient( aPos.Y(),text::VertOrientation::NONE, text::RelOrientation::FRAME ) );
            }

            const Point aPt( pDestShell->GetCrsrDocPos() );

            if( bIsMove )
                GetDoc()->DelLayoutFmt( pOldFmt );

            // nur selektieren wenn es in der gleichen Shell verschoben/
            //	kopiert wird
            if( bSelectInsert )
            {
                SwFlyFrm* pFlyFrm = ((SwFlyFrmFmt*)pFlyFmt)->GetFrm( &aPt, FALSE );
                if( pFlyFrm )
                {
                    //JP 12.05.98: sollte das nicht im SelectFlyFrm stehen???
                    pDestShell->Imp()->GetDrawView()->UnmarkAll();
                    pDestShell->SelectFlyFrm( *pFlyFrm, TRUE );
                }
            }

            if( this != pDestShell && !pDestShell->HasShFcs() )
                pDestShell->Imp()->GetDrawView()->hideMarkHandles();
        }
    }
    else if ( IsObjSelected() )
        bRet = CopyDrawSel( pDestShell, rSttPt, rInsPt, bIsMove, bSelectInsert );
    else if( IsTableMode() )
    {
        // kopiere Teile aus einer Tabelle: lege eine Tabelle mit der Breite
        // von der Originalen an und kopiere die selectierten Boxen.
        // Die Groessen werden prozentual korrigiert.

        // lasse ueber das Layout die Boxen suchen
        const SwTableNode* pTblNd;
        SwSelBoxes aBoxes;
        GetTblSel( *this, aBoxes );
        if( aBoxes.Count() &&
            0 != (pTblNd = aBoxes[0]->GetSttNd()->FindTableNode()) )
        {
            SwPosition* pDstPos = 0;
            if( this == pDestShell )
            {
                // gleiche Shell? Dann erzeuge einen Crsr an der
                // uebergebenen DokumentPosition
                pDstPos = new SwPosition( *GetCrsr()->GetPoint() );
                Point aPt( rInsPt );
                GetLayout()->GetCrsrOfst( pDstPos, aPt );
                if( !pDstPos->nNode.GetNode().IsNoTxtNode() )
                    bRet = TRUE;
            }
            else if( !pDestShell->GetCrsr()->GetNode()->IsNoTxtNode() )
            {
                pDstPos = new SwPosition( *pDestShell->GetCrsr()->GetPoint() );
                bRet = TRUE;
            }

            if( bRet )
            {
                if( GetDoc() == pDestShell->GetDoc() )
                    ParkTblCrsr();

                bRet = pDestShell->GetDoc()->InsCopyOfTbl( *pDstPos, aBoxes,0,
                                        bIsMove && this == pDestShell &&
                                        aBoxes.Count() == pTblNd->GetTable().
                                        GetTabSortBoxes().Count(),
                                        this != pDestShell );

                if( this != pDestShell )
                    *pDestShell->GetCrsr()->GetPoint() = *pDstPos;

                // wieder alle geparkten Crsr erzeugen?
                if( GetDoc() == pDestShell->GetDoc() )
                    GetCrsr();

                // JP 16.04.99: Bug 64908 - InsPos setzen, damit der geparkte
                //				Cursor auf die EinfuegePos. positioniert wird
                if( this == pDestShell )
                    GetCrsrDocPos() = rInsPt;
            }
            delete pDstPos;
        }
    }
    else
    {
        bRet = TRUE;
        if( this == pDestShell )
        {
            // gleiche Shell? Dann erfrage die Position an der
            // uebergebenen DokumentPosition
            SwPosition aPos( *GetCrsr()->GetPoint() );
            Point aPt( rInsPt );
            GetLayout()->GetCrsrOfst( &aPos, aPt );
            bRet = !aPos.nNode.GetNode().IsNoTxtNode();
        }
        else if( pDestShell->GetCrsr()->GetNode()->IsNoTxtNode() )
            bRet = FALSE;

        if( bRet )
            bRet = 0 != SwEditShell::Copy( pDestShell );
    }

    pDestShell->GetDoc()->SetRedlineMode_intern( eOldRedlMode );
    pDoc->SetCopyIsMove( bCopyIsMove );

    // wurden neue Tabellenformeln eingefuegt ?
    if( pTblFldTyp->GetDepends() )
    {
        // alte Actions beenden; die Tabellen-Frames werden angelegt und
        // eine SSelection kann erzeugt werden
        USHORT nActCnt;
        for( nActCnt = 0; pDestShell->ActionPend(); ++nActCnt )
            pDestShell->EndAllAction();

        for( ; nActCnt; --nActCnt )
            pDestShell->StartAllAction();
    }
    pDestShell->GetDoc()->UnlockExpFlds();
    pDestShell->GetDoc()->UpdateFlds(NULL, false);

    pDestShell->EndAllAction();
    return bRet;
}

/*************************************************************************
|*
|*	SwFEShell::Paste()	Paste fuer das Interne Clipboard.
|*		Kopiert den Inhalt vom Clipboard in das Dokument.
|*
|*	Ersterstellung		JP ??
|*	Letzte Aenderung	MA 22. Feb. 95
|
|*************************************************************************/

namespace {
    typedef boost::shared_ptr<SwPaM> PaMPtr;
    typedef boost::shared_ptr<SwPosition> PositionPtr;
    typedef std::pair< PaMPtr, PositionPtr > Insertion;
}

BOOL SwFEShell::Paste( SwDoc* pClpDoc, BOOL bIncludingPageFrames )
{
    SET_CURR_SHELL( this );
    ASSERT( pClpDoc, "kein Clipboard-Dokument"	);
    const USHORT nStartPageNumber = GetPhyPageNum();
    // dann bis zum Ende vom Nodes Array
    SwNodeIndex aIdx( pClpDoc->GetNodes().GetEndOfExtras(), 2 );
    SwPaM aCpyPam( aIdx ); //DocStart

    // sind Tabellen-Formeln im Bereich, dann muss erst die Tabelle
    // angezeigt werden, damit die Tabellen-Formel den neuen Wert errechnen
    // kann (bei Bereichen wird sich ueber das Layout die einzelnen Boxen
    // besorgt)
    SwFieldType* pTblFldTyp = GetDoc()->GetSysFldType( RES_TABLEFLD );

    SwTableNode *pDestNd, *pSrcNd = aCpyPam.GetNode()->GetTableNode();
    if( !pSrcNd )								// TabellenNode ?
    {											// nicht ueberspringen!!
        SwCntntNode* pCNd = aCpyPam.GetNode()->GetCntntNode();
        if( pCNd )
            aCpyPam.GetPoint()->nContent.Assign( pCNd, 0 );
        else if( !aCpyPam.Move( fnMoveForward, fnGoNode ))
            aCpyPam.Move( fnMoveBackward, fnGoNode );
    }

    aCpyPam.SetMark();
    aCpyPam.Move( fnMoveForward, fnGoDoc );

    BOOL bRet = TRUE, bDelTbl = TRUE;
    StartAllAction();
    GetDoc()->StartUndo( UNDO_INSGLOSSARY, NULL );
    GetDoc()->LockExpFlds();

    // When the clipboard content has been created by a rectangular selection
    // the pasting is more sophisticated:
    // every paragraph will be inserted into another position.
    // The first positions are given by the actual cursor ring,
    // if there are more text portions to insert than cursor in this ring,
    // the additional insert positions will be created by moving the last
    // cursor position into the next line (like pressing the cursor down key)
    if( pClpDoc->IsColumnSelection() && !IsTableMode() )
    {
        // Creation of the list of insert positions
        std::list< Insertion > aCopyList;
        // The number of text portions of the rectangular selection
        const sal_uInt32 nSelCount = aCpyPam.GetPoint()->nNode.GetIndex()
                       - aCpyPam.GetMark()->nNode.GetIndex();
        sal_uInt32 nCount = nSelCount;
        SwNodeIndex aClpIdx( aIdx );
        SwPaM* pStartCursor = GetCrsr();
        SwPaM* pCurrCrsr = pStartCursor;
        sal_uInt32 nCursorCount = pStartCursor->numberOf();
        // If the target selection is a multi-selection, often the last and first
        // cursor of the ring points to identical document positions. Then
        // we should avoid double insertion of text portions...
        while( nCursorCount > 1 && *pCurrCrsr->GetPoint() == 
            *(dynamic_cast<SwPaM*>(pCurrCrsr->GetPrev())->GetPoint()) )
        {
            --nCursorCount;
            pCurrCrsr = dynamic_cast<SwPaM*>(pCurrCrsr->GetNext());
            pStartCursor = pCurrCrsr;
        }
        SwPosition aStartPos( *pStartCursor->GetPoint() );
        SwPosition aInsertPos( aStartPos ); // first insertion position
        bool bCompletePara = false;
        USHORT nMove = 0;
        while( nCount )
        {
            --nCount;
            ASSERT( aIdx.GetNode().GetCntntNode(), "Who filled the clipboard?!" )
            if( aIdx.GetNode().GetCntntNode() ) // robust
            {
                Insertion aInsertion( PaMPtr( new SwPaM( aIdx ) ),
                    PositionPtr( new SwPosition( aInsertPos ) ) );
                ++aIdx;
                aInsertion.first->SetMark();
                if( pStartCursor == pCurrCrsr->GetNext() )
                {   // Now we have to look for insertion positions...
                    if( !nMove ) // Annotate the last given insert position
                        aStartPos = aInsertPos;
                    SwCursor aCrsr( aStartPos, 0, false);
                    // Check if we find another insert position by moving
                    // down the last given position
                    if( aCrsr.UpDown( FALSE, ++nMove, 0, 0 ) )
                        aInsertPos = *aCrsr.GetPoint();
                    else // if there is no paragraph we have to create it
                        bCompletePara = nCount > 0;
                    nCursorCount = 0;
                }
                else // as long as we find more insert positions in the cursor ring
                {    // we'll take them
                    pCurrCrsr = dynamic_cast<SwPaM*>(pCurrCrsr->GetNext());
                    aInsertPos = *pCurrCrsr->GetPoint();
                    --nCursorCount;
                }
                // If there are no more paragraphs e.g. at the end of a document,
                // we insert complete paragraphs instead of text portions
                if( bCompletePara )
                    aInsertion.first->GetPoint()->nNode = aIdx;
                else
                    aInsertion.first->GetPoint()->nContent = 
                        aInsertion.first->GetCntntNode()->Len();
                aCopyList.push_back( aInsertion );
            }
            // If there are no text portions left but there are some more
            // cursor positions to fill we have to restart with the first
            // text portion
            if( !nCount && nCursorCount )
            {
                nCount = std::min( nSelCount, nCursorCount );
                aIdx = aClpIdx; // Start of clipboard content
            }
        }
        std::list< Insertion >::const_iterator pCurr = aCopyList.begin();
        std::list< Insertion >::const_iterator pEnd = aCopyList.end();
        while( pCurr != pEnd )
        {
            SwPosition& rInsPos = *pCurr->second;
            SwPaM& rCopy = *pCurr->first;
            const SwStartNode* pBoxNd = rInsPos.nNode.GetNode().FindTableBoxStartNode();
            if( pBoxNd && 2 == pBoxNd->EndOfSectionIndex() - pBoxNd->GetIndex() &&
                rCopy.GetPoint()->nNode != rCopy.GetMark()->nNode )
            {
                // if more than one node will be copied into a cell
                // the box attributes have to be removed
                GetDoc()->ClearBoxNumAttrs( rInsPos.nNode );
            }
            {
                SwNodeIndex aIndexBefore(rInsPos.nNode);
                aIndexBefore--;
                pClpDoc->Copy( rCopy, rInsPos );
                {
                    aIndexBefore++;
                    SwPaM aPaM(SwPosition(aIndexBefore, 0),
                               SwPosition(rInsPos.nNode, 0));
                    aPaM.GetDoc()->MakeUniqueNumRules(aPaM);
                }
            }
            SaveTblBoxCntnt( &rInsPos );
            ++pCurr;
        }
    }
    else
    {
        FOREACHPAM_START(this)

        if( pSrcNd &&
            0 != ( pDestNd = GetDoc()->IsIdxInTbl( PCURCRSR->GetPoint()->nNode )))
        {
            SwPosition aDestPos( *PCURCRSR->GetPoint() );

            BOOL bParkTblCrsr = FALSE;
            const SwStartNode* pSttNd =  PCURCRSR->GetNode()->FindTableBoxStartNode();

            // TABLE IN TABLE: Tabelle in Tabelle kopieren
            // lasse ueber das Layout die Boxen suchen
            SwSelBoxes aBoxes;
            if( IsTableMode() )		// Tabellen-Selecktion ??
            {
                GetTblSel( *this, aBoxes );
                ParkTblCrsr();
                bParkTblCrsr = TRUE;
            }
            else if( !PCURCRSR->HasMark() && PCURCRSR->GetNext() == PCURCRSR &&
                     ( !pSrcNd->GetTable().IsTblComplex() ||
                       pDestNd->GetTable().IsNewModel() ) )
            {
                // dann die Tabelle "relativ" kopieren
                SwTableBox* pBox = pDestNd->GetTable().GetTblBox(
                                        pSttNd->GetIndex() );
                ASSERT( pBox, "Box steht nicht in dieser Tabelle" );
                aBoxes.Insert( pBox );
            }

            SwNodeIndex aNdIdx( *pDestNd->EndOfSectionNode());
            if( !bParkTblCrsr )
            {
                // erstmal aus der gesamten Tabelle raus
// ????? was ist mit Tabelle alleine im Rahmen ???????
                SwCntntNode* pCNd = GetDoc()->GetNodes().GoNext( &aNdIdx );
                SwPosition aPos( aNdIdx, SwIndex( pCNd, 0 ));
                // #i59539: Don't remove all redline
                ::PaMCorrAbs( SwNodeIndex( *pDestNd ),
                            SwNodeIndex( *pDestNd->EndOfSectionNode() ),
                            aPos );
            }

            bRet = GetDoc()->InsCopyOfTbl( aDestPos, aBoxes, &pSrcNd->GetTable(),
                                            FALSE, FALSE );

            if( bParkTblCrsr )
                GetCrsr();
            else
            {
                // und wieder in die Box zurueck
                aNdIdx = *pSttNd;
                SwCntntNode* pCNd = GetDoc()->GetNodes().GoNext( &aNdIdx );
                SwPosition aPos( aNdIdx, SwIndex( pCNd, 0 ));
                // #i59539: Don't remove all redline
                ::PaMCorrAbs( PCURCRSR->GetPoint()->nNode, aPos );
            }

            break;		// aus der "while"-Schleife heraus
        }
        else if( *aCpyPam.GetPoint() == *aCpyPam.GetMark() &&
                 pClpDoc->GetSpzFrmFmts()->Count() )
        {
            // so langsam sollte mal eine DrawView erzeugt werden
            if( !Imp()->GetDrawView() )
                MakeDrawView();

            for ( USHORT i = 0; i < pClpDoc->GetSpzFrmFmts()->Count(); ++i )
            {
                BOOL bInsWithFmt = TRUE;
                const SwFrmFmt& rCpyFmt = *(*pClpDoc->GetSpzFrmFmts())[i];

                if( Imp()->GetDrawView()->IsGroupEntered() &&
                    RES_DRAWFRMFMT == rCpyFmt.Which() &&
                    FLY_IN_CNTNT != rCpyFmt.GetAnchor().GetAnchorId() )
                {
                    const SdrObject* pSdrObj = rCpyFmt.FindSdrObject();
                    if( pSdrObj )
                    {
                        SdrObject* pNew = GetDoc()->CloneSdrObj( *pSdrObj,
                                                            FALSE, FALSE );

                        // Insert object sets any anchor position to 0.
                        // Therefore we calculate the absolute position here
                        // and after the insert the anchor of the object
                        // is set to the anchor of the group object.
                        Rectangle aSnapRect = pNew->GetSnapRect();
                        if( pNew->GetAnchorPos().X() || pNew->GetAnchorPos().Y() )
                        {
                            const Point aPoint( 0, 0 );
                            // OD 2004-04-05 #i26791# - direct drawing object
                            // positioning for group members
                            pNew->NbcSetAnchorPos( aPoint );
                            pNew->NbcSetSnapRect( aSnapRect );
                        }

                        Imp()->GetDrawView()->InsertObjectAtView( pNew, *Imp()->GetPageView() );

                        Point aGrpAnchor( 0, 0 );
                        SdrObjList* pList = pNew->GetObjList();
                        if ( pList )
                        {
                            SdrObject* pOwner = pList->GetOwnerObj();
                            if ( pOwner )
                            {
                                SdrObjGroup* pThisGroup = PTR_CAST(SdrObjGroup, pOwner);
                                aGrpAnchor = pThisGroup->GetAnchorPos();
                            }
                        }

                        // OD 2004-04-05 #i26791# - direct drawing object
                        // positioning for group members
                        pNew->NbcSetAnchorPos( aGrpAnchor );
                        pNew->SetSnapRect( aSnapRect );

                        bInsWithFmt = FALSE;
                    }
                }

                if( bInsWithFmt  )
                {
                    SwFmtAnchor aAnchor( rCpyFmt.GetAnchor() );
                    if( FLY_AT_CNTNT == aAnchor.GetAnchorId() ||
                        FLY_AUTO_CNTNT == aAnchor.GetAnchorId() ||
                        FLY_IN_CNTNT == aAnchor.GetAnchorId() )
                    {
                        SwPosition* pPos = PCURCRSR->GetPoint();
                        // #108784# allow shapes (no controls) in header/footer
                        if( RES_DRAWFRMFMT == rCpyFmt.Which() &&
                            GetDoc()->IsInHeaderFooter( pPos->nNode ) &&
                            CheckControlLayer( rCpyFmt.FindSdrObject() ) )
                            continue;

                        aAnchor.SetAnchor( pPos );
                    }
                    else if( FLY_PAGE == aAnchor.GetAnchorId() )
                    {
                        aAnchor.SetPageNum( GetPhyPageNum() );
                    }
                    else if( FLY_AT_FLY == aAnchor.GetAnchorId() )
                    {
                        Point aPt;
                        lcl_SetAnchor( *PCURCRSR->GetPoint(), *PCURCRSR->GetNode(),
                                        0, aPt, *this, aAnchor, aPt, FALSE );
                    }

                    SwFrmFmt * pNew = GetDoc()->CopyLayoutFmt( rCpyFmt, aAnchor, true, true );

                    if( pNew )
                    {
                        if( RES_FLYFRMFMT == pNew->Which() )
                        {
                            const Point aPt( GetCrsrDocPos() );
                            SwFlyFrm* pFlyFrm = ((SwFlyFrmFmt*)pNew)->
                                                        GetFrm( &aPt, FALSE );
                            if( pFlyFrm )
                                SelectFlyFrm( *pFlyFrm, TRUE );
                            // immer nur den ersten Fly-Frame nehmen; die anderen
                            // wurden ueber Fly in Fly ins ClipBoard kopiert !
                            break;
                        }
                        else
                        {
                            ASSERT( RES_DRAWFRMFMT == pNew->Which(), "Neues Format.");
                            // --> OD 2005-09-01 #i52780# - drawing object has
                            // to be made visible on paste.
                            {
                                SwDrawContact* pContact =
                                    static_cast<SwDrawContact*>(pNew->FindContactObj());
                                pContact->MoveObjToVisibleLayer( pContact->GetMaster() );
                            }
                            // <--
                            SdrObject *pObj = pNew->FindSdrObject();
                            SwDrawView  *pDV = Imp()->GetDrawView();
                            pDV->MarkObj( pObj, pDV->GetSdrPageView() );
                            // --> OD 2005-04-15 #i47455# - notify draw frame format
                            // that position attributes are already set.
                            if ( pNew->ISA(SwDrawFrmFmt) )
                            {
                                static_cast<SwDrawFrmFmt*>(pNew)->PosAttrSet();
                            }
                            // <--
                        }
                    }
                }
            }
        }
        else
        {
            if( bDelTbl && IsTableMode() )
            {
                SwEditShell::Delete();
                bDelTbl = FALSE;
            }

            SwPosition& rInsPos = *PCURCRSR->GetPoint();
            const SwStartNode* pBoxNd = rInsPos.nNode.GetNode().
                                                    FindTableBoxStartNode();
            if( pBoxNd && 2 == pBoxNd->EndOfSectionIndex() -
                                pBoxNd->GetIndex() &&
                aCpyPam.GetPoint()->nNode != aCpyPam.GetMark()->nNode )
            {
                // es wird mehr als 1 Node in die akt. Box kopiert. Dann
                // muessen die BoxAttribute aber entfernt werden.
                GetDoc()->ClearBoxNumAttrs( rInsPos.nNode );
            }
            //find out if the clipboard document starts with a table
            bool bStartWithTable = 0 != aCpyPam.Start()->nNode.GetNode().FindTableNode();
            SwPosition aInsertPosition( rInsPos );

            {
                SwNodeIndex aIndexBefore(rInsPos.nNode);

                aIndexBefore--;

                pClpDoc->Copy( aCpyPam, rInsPos );

                {
                    aIndexBefore++;
                    SwPaM aPaM(SwPosition(aIndexBefore, 0),
                               SwPosition(rInsPos.nNode, 0));

                    aPaM.GetDoc()->MakeUniqueNumRules(aPaM);
                }
            }

            SaveTblBoxCntnt( &rInsPos );
            if(bIncludingPageFrames && bStartWithTable)
            {
                //remove the paragraph in front of the table
                SwPaM aPara(aInsertPosition);
                GetDoc()->DelFullPara(aPara);
            }
            //additionally copy page bound frames
            if( bIncludingPageFrames && pClpDoc->GetSpzFrmFmts()->Count() )
            {
                // create a draw view if necessary
                if( !Imp()->GetDrawView() )
                    MakeDrawView();

                for ( USHORT i = 0; i < pClpDoc->GetSpzFrmFmts()->Count(); ++i )
                {
                    BOOL bInsWithFmt = TRUE;
                    const SwFrmFmt& rCpyFmt = *(*pClpDoc->GetSpzFrmFmts())[i];
                    if( bInsWithFmt  )
                    {
                        SwFmtAnchor aAnchor( rCpyFmt.GetAnchor() );
                        if( FLY_PAGE == aAnchor.GetAnchorId() )
                        {
                            aAnchor.SetPageNum( aAnchor.GetPageNum() + nStartPageNumber - 1 );
                        }
                        else
                            continue;
                        GetDoc()->CopyLayoutFmt( rCpyFmt, aAnchor, true, true );
                    }
                }
            }
        }

        FOREACHPAM_END()
    }
    
    GetDoc()->EndUndo( UNDO_INSGLOSSARY, NULL );

    // wurden neue Tabellenformeln eingefuegt ?
    if( pTblFldTyp->GetDepends() )
    {
        // alte Actions beenden; die Tabellen-Frames werden angelegt und
        // eine Selection kann erzeugt werden
        USHORT nActCnt;
        for( nActCnt = 0; ActionPend(); ++nActCnt )
            EndAllAction();

        for( ; nActCnt; --nActCnt )
            StartAllAction();
    }
    GetDoc()->UnlockExpFlds();
    GetDoc()->UpdateFlds(NULL, false);
    EndAllAction();

    return bRet;
}

/*-- 14.06.2004 13:31:17---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwFEShell::PastePages( SwFEShell& rToFill, USHORT nStartPage, USHORT nEndPage)
{
    Push();
    if(!GotoPage(nStartPage))
    {
        Pop(sal_False);
        return FALSE;
    }
    MovePage( fnPageCurr, fnPageStart );
    SwPaM aCpyPam( *GetCrsr()->GetPoint() );
    String sStartingPageDesc = GetPageDesc( GetCurPageDesc()).GetName();
    SwPageDesc* pDesc = rToFill.FindPageDescByName( sStartingPageDesc, TRUE );
    if( pDesc )
        rToFill.ChgCurPageDesc( *pDesc );

    if(!GotoPage(nEndPage))
    {
        Pop(sal_False);
        return FALSE;
    }
    //if the page starts with a table a paragraph has to be inserted before
    SwNode* pTableNode = aCpyPam.GetNode()->FindTableNode();
    if(pTableNode)
    {
        //insert a paragraph
        StartUndo(UNDO_INSERT);
        SwNodeIndex aTblIdx(  *pTableNode, -1 );
        SwPosition aBefore(aTblIdx);
        if(GetDoc()->AppendTxtNode( aBefore ))
        {
            SwPaM aTmp(aBefore);
            aCpyPam = aTmp;
        }
        EndUndo(UNDO_INSERT);
    }

    MovePage( fnPageCurr, fnPageEnd );
    aCpyPam.SetMark();
    *aCpyPam.GetMark() = *GetCrsr()->GetPoint();

    SET_CURR_SHELL( this );

    StartAllAction();
    GetDoc()->LockExpFlds();
    SetSelection(aCpyPam);
    // copy the text of the selection
    SwEditShell::Copy(&rToFill);

    if(pTableNode)
    {
        //remove the inserted paragraph
        Undo();
        //remove the paragraph in the second doc, too
        SwNodeIndex aIdx( rToFill.GetDoc()->GetNodes().GetEndOfExtras(), 2 );
        SwPaM aPara( aIdx ); //DocStart
        rToFill.GetDoc()->DelFullPara(aPara);
    }
    // now the page bound objects
    //additionally copy page bound frames
    if( GetDoc()->GetSpzFrmFmts()->Count() )
    {
        // create a draw view if necessary
        if( !rToFill.Imp()->GetDrawView() )
            rToFill.MakeDrawView();

        for ( USHORT i = 0; i < GetDoc()->GetSpzFrmFmts()->Count(); ++i )
        {
            const SwFrmFmt& rCpyFmt = *(*GetDoc()->GetSpzFrmFmts())[i];
            SwFmtAnchor aAnchor( rCpyFmt.GetAnchor() );
            if( FLY_PAGE == aAnchor.GetAnchorId() &&
                    aAnchor.GetPageNum() >= nStartPage && aAnchor.GetPageNum() <= nEndPage)
            {
                aAnchor.SetPageNum( aAnchor.GetPageNum() - nStartPage + 1);
            }
            else
                continue;
            rToFill.GetDoc()->CopyLayoutFmt( rCpyFmt, aAnchor, true, true );
        }
    }
    GetDoc()->UnlockExpFlds();
    GetDoc()->UpdateFlds(NULL, false);
    Pop(sal_False);
    EndAllAction();

    return TRUE;
}

BOOL SwFEShell::GetDrawObjGraphic( ULONG nFmt, Graphic& rGrf ) const
{
    ASSERT( Imp()->HasDrawView(), "GetDrawObjGraphic without DrawView?" );
    const SdrMarkList &rMrkList = Imp()->GetDrawView()->GetMarkedObjectList();
    BOOL bConvert = TRUE;
    if( rMrkList.GetMarkCount() )
    {
        if( rMrkList.GetMarkCount() == 1 &&
            rMrkList.GetMark( 0 )->GetMarkedSdrObj()->ISA(SwVirtFlyDrawObj) )
        {
            // Rahmen selektiert
            if( CNT_GRF == GetCntType() )
            {
                // --> OD 2005-02-09 #119353# - robust
                const Graphic* pGrf( GetGraphic() );
                if ( pGrf )
                {
                    Graphic aGrf( *pGrf );
                    if( SOT_FORMAT_GDIMETAFILE == nFmt )
                    {
                        if( GRAPHIC_BITMAP != aGrf.GetType() )
                        {
                            rGrf = aGrf;
                            bConvert = FALSE;
                        }
                        else if( GetWin() )
                        {
                            Size aSz;
                            Point aPt;
                            GetGrfSize( aSz );

                            VirtualDevice aVirtDev;
                            aVirtDev.EnableOutput( FALSE );

                            MapMode aTmp( GetWin()->GetMapMode() );
                            aTmp.SetOrigin( aPt );
                            aVirtDev.SetMapMode( aTmp );

                            GDIMetaFile aMtf;
                            aMtf.Record( &aVirtDev );
                            aGrf.Draw( &aVirtDev, aPt, aSz );
                            aMtf.Stop();
                            aMtf.SetPrefMapMode( aTmp );
                            aMtf.SetPrefSize( aSz );
                            rGrf = aMtf;
                        }
                    }
                    else if( GRAPHIC_BITMAP == aGrf.GetType() )
                    {
                        rGrf = aGrf;
                        bConvert = FALSE;
                    }
                    else
                    {
                        //fix(23806): Nicht die Originalgroesse, sondern die
                        //aktuelle. Anderfalls kann es passieren, dass z.B. bei
                        //Vektorgrafiken mal eben zig MB angefordert werden.
                        const Size aSz( FindFlyFrm()->Prt().SSize() );
                        VirtualDevice aVirtDev( *GetWin() );

                        MapMode aTmp( MAP_TWIP );
                        aVirtDev.SetMapMode( aTmp );
                        if( aVirtDev.SetOutputSize( aSz ) )
                        {
                            aGrf.Draw( &aVirtDev, Point(), aSz );
                            rGrf = aVirtDev.GetBitmap( Point(), aSz );
                        }
                        else
                        {
                            rGrf = aGrf;
                            bConvert = FALSE;
                        }
                    }
                }
                // <--
            }
        }
        else if( SOT_FORMAT_GDIMETAFILE == nFmt )
            rGrf = Imp()->GetDrawView()->GetAllMarkedMetaFile();
        else if( SOT_FORMAT_BITMAP == nFmt )
            rGrf = Imp()->GetDrawView()->GetAllMarkedBitmap();
    }
    return bConvert;
}

// --> OD 2005-08-03 #i50824#
// --> OD 2006-03-01 #b6382898#
// replace method <lcl_RemoveOleObjsFromSdrModel> by <lcl_ConvertSdrOle2ObjsToSdrGrafObjs>
void lcl_ConvertSdrOle2ObjsToSdrGrafObjs( SdrModel* _pModel )
{
    for ( sal_uInt16 nPgNum = 0; nPgNum < _pModel->GetPageCount(); ++nPgNum )
    {
        // setup object iterator in order to iterate through all objects
        // including objects in group objects, but exclusive group objects.
        SdrObjListIter aIter(*(_pModel->GetPage( nPgNum )));
        while( aIter.IsMore() )
        {
            SdrOle2Obj* pOle2Obj = dynamic_cast< SdrOle2Obj* >( aIter.Next() );
            if( pOle2Obj )
            {
                // found an ole2 shape
                SdrObjList* pObjList = pOle2Obj->GetObjList();

                // get its graphic
                Graphic aGraphic;
                pOle2Obj->Connect();
                Graphic* pGraphic = pOle2Obj->GetGraphic();
                if( pGraphic )
                    aGraphic = *pGraphic;
                pOle2Obj->Disconnect();

                // create new graphic shape with the ole graphic and shape size
                SdrGrafObj* pGraphicObj = new SdrGrafObj( aGraphic, pOle2Obj->GetCurrentBoundRect() );
                // apply layer of ole2 shape at graphic shape
                pGraphicObj->SetLayer( pOle2Obj->GetLayer() );

                // replace ole2 shape with the new graphic object and delete the ol2 shape
                SdrObject* pRemovedObject = pObjList->ReplaceObject( pGraphicObj, pOle2Obj->GetOrdNum() );
                SdrObject::Free( pRemovedObject );
            }
        }
    }
}
// <--
void SwFEShell::Paste( SvStream& rStrm, USHORT nAction, const Point* pPt )
{
    SET_CURR_SHELL( this );
    StartAllAction();
    StartUndo();

    SvtPathOptions aPathOpt;
    FmFormModel* pModel = new FmFormModel( aPathOpt.GetPalettePath(),
                                            0, GetDoc()->GetDocShell() );
    pModel->GetItemPool().FreezeIdRanges();

    rStrm.Seek(0);

    uno::Reference< io::XInputStream > xInputStream( new utl::OInputStreamWrapper( rStrm ) );
    SvxDrawingLayerImport( pModel, xInputStream );

    if ( !Imp()->HasDrawView() )
        Imp()->MakeDrawView();

    Point aPos( pPt ? *pPt : GetCharRect().Pos() );
    SdrView *pView = Imp()->GetDrawView();

    //Drop auf bestehendes Objekt: Objekt ersetzen oder neu Attributieren.
    if( pModel->GetPageCount() > 0 &&
        1 == pModel->GetPage(0)->GetObjCount() &&
        1 == pView->GetMarkedObjectList().GetMarkCount() )
    {
        // OD 10.07.2003 #110742# - replace a marked 'virtual' drawing object
        // by its corresponding 'master' drawing object in the mark list.
        SwDrawView::ReplaceMarkedDrawVirtObjs( *pView );

        SdrObject* pClpObj = pModel->GetPage(0)->GetObj(0);
        SdrObject* pOldObj = pView->GetMarkedObjectList().GetMark( 0 )->GetMarkedSdrObj();

        if( SW_PASTESDR_SETATTR == nAction && pOldObj->ISA(SwVirtFlyDrawObj) )
            nAction = SW_PASTESDR_REPLACE;

        switch( nAction )
        {
        case SW_PASTESDR_REPLACE:
            {
                const SwFrmFmt* pFmt(0);
                const SwFrm* pAnchor(0);
                if( pOldObj->ISA(SwVirtFlyDrawObj) )
                {
                    pFmt = FindFrmFmt( pOldObj );

                    Point aNullPt;
                    SwFlyFrm* pFlyFrm = ((SwFlyFrmFmt*)pFmt)->GetFrm( &aNullPt );
                    pAnchor = pFlyFrm->GetAnchorFrm();

                    if( pAnchor->FindFooterOrHeader() )
                    {
                        // wenn TextRahmen in der Kopf/Fusszeile steht, dann
                        // nicht ersetzen, sondern nur einfuegen
                        nAction = SW_PASTESDR_INSERT;
                        break;
                    }
                }

                SdrObject* pNewObj = pClpObj->Clone();
                Rectangle aOldObjRect( pOldObj->GetCurrentBoundRect() );
                Size aOldObjSize( aOldObjRect.GetSize() );
                Rectangle aNewRect( pNewObj->GetCurrentBoundRect() );
                Size aNewSize( aNewRect.GetSize() );

                Fraction aScaleWidth( aOldObjSize.Width(), aNewSize.Width() );
                Fraction aScaleHeight( aOldObjSize.Height(), aNewSize.Height());
                pNewObj->NbcResize( aNewRect.TopLeft(), aScaleWidth, aScaleHeight);

                Point aVec = aOldObjRect.TopLeft() - aNewRect.TopLeft();
                pNewObj->NbcMove(Size(aVec.X(), aVec.Y()));

                if( pNewObj->ISA( SdrUnoObj ) )
                    pNewObj->SetLayer( GetDoc()->GetControlsId() );
                else if( pOldObj->ISA( SdrUnoObj ) )
                    pNewObj->SetLayer( GetDoc()->GetHeavenId() );
                else
                    pNewObj->SetLayer( pOldObj->GetLayer() );

                if( pOldObj->ISA(SwVirtFlyDrawObj) )
                {
                    // Attribute sichern und dam SdrObject setzen
                    SfxItemSet aFrmSet( pDoc->GetAttrPool(),
                                            RES_SURROUND, RES_ANCHOR );
                    aFrmSet.Set( pFmt->GetAttrSet() );

                    Point aNullPt;
                    if( pAnchor->IsTxtFrm() && ((SwTxtFrm*)pAnchor)->IsFollow() )
                    {
                        const SwTxtFrm* pTmp = (SwTxtFrm*)pAnchor;
                        do {
                            pTmp = pTmp->FindMaster();
                            ASSERT( pTmp, "Where's my Master?" );
                        } while( pTmp->IsFollow() );
                        pAnchor = pTmp;
                    }
                    if( pOldObj->ISA( SdrCaptionObj ))
                        aNullPt = ((SdrCaptionObj*)pOldObj)->GetTailPos();
                    else
                        aNullPt = aOldObjRect.TopLeft();

                    Point aNewAnchor = pAnchor->GetFrmAnchorPos( ::HasWrap( pOldObj ) );
                    // OD 2004-04-05 #i26791# - direct positioning of Writer
                    // fly frame object for <SwDoc::Insert(..)>
                    pNewObj->NbcSetRelativePos( aNullPt - aNewAnchor );
                    pNewObj->NbcSetAnchorPos( aNewAnchor );

                    pOldObj->GetOrdNum();

                    DelSelectedObj();

                    pFmt = GetDoc()->Insert( *GetCrsr(), *pNewObj, &aFrmSet, NULL );
                }
                else
                    pView->ReplaceObjectAtView( pOldObj, *Imp()->GetPageView(), pNewObj, TRUE );
            }
            break;

        case SW_PASTESDR_SETATTR:
            {
                SfxItemSet aSet( GetAttrPool() );
                aSet.Put(pClpObj->GetMergedItemSet());
                pView->SetAttributes( aSet, FALSE );
            }
            break;

        default:
            nAction = SW_PASTESDR_INSERT;
            break;
        }
    }
    else
        nAction = SW_PASTESDR_INSERT;

    if( SW_PASTESDR_INSERT == nAction )
    {
        GetDoc()->SetNoDrawUndoObj( TRUE );

        sal_Bool bDesignMode = pView->IsDesignMode();
        if( !bDesignMode )
            pView->SetDesignMode( sal_True );

        // --> OD 2005-08-03 #i50824#
        // --> OD 2006-03-01 #b6382898#
        // method <lcl_RemoveOleObjsFromSdrModel> replaced by <lcl_ConvertSdrOle2ObjsToSdrGrafObjs>
        lcl_ConvertSdrOle2ObjsToSdrGrafObjs( pModel );
        // <--
        pView->Paste( *pModel, aPos );

        ULONG nCnt = pView->GetMarkedObjectList().GetMarkCount();
        if( nCnt )
        {
            const Point aNull( 0, 0 );
            for( ULONG i=0; i < nCnt; ++i )
            {
                SdrObject *pObj = pView->GetMarkedObjectList().GetMark(i)->GetMarkedSdrObj();
                pObj->ImpSetAnchorPos( aNull );
            }

            pView->SetCurrentObj( OBJ_GRUP, SdrInventor );
            if ( nCnt > 1 )
                pView->GroupMarked();
            SdrObject *pObj = pView->GetMarkedObjectList().GetMark(0)->GetMarkedSdrObj();
            if( pObj->ISA( SdrUnoObj ) )
            {
                pObj->SetLayer( GetDoc()->GetControlsId() );
                bDesignMode = sal_True;
            }
            else
                pObj->SetLayer( GetDoc()->GetHeavenId() );
            const Rectangle &rSnap = pObj->GetSnapRect();
            const Size aDiff( rSnap.GetWidth()/2, rSnap.GetHeight()/2 );
            pView->MoveMarkedObj( aDiff );
            ImpEndCreate();
            if( !bDesignMode )
                pView->SetDesignMode( sal_False );
        }
        GetDoc()->SetNoDrawUndoObj( FALSE );
    }
    EndUndo();
    EndAllAction();
    delete pModel;
}

BOOL SwFEShell::Paste( const Graphic &rGrf )
{
    SET_CURR_SHELL( this );
    SdrObject* pObj;
    SdrView *pView = Imp()->GetDrawView();

    BOOL bRet = 1 == pView->GetMarkedObjectList().GetMarkCount() &&
        (pObj = pView->GetMarkedObjectList().GetMark( 0 )->GetMarkedSdrObj())->IsClosedObj() &&
        !pObj->ISA( SdrOle2Obj );

    if( bRet )
    {
        XOBitmap aXOBitmap( rGrf.GetBitmap() );
        SfxItemSet aSet( GetAttrPool(), XATTR_FILLSTYLE, XATTR_FILLBITMAP );
        aSet.Put( XFillStyleItem( XFILL_BITMAP ));
        aSet.Put( XFillBitmapItem( aEmptyStr, aXOBitmap ));
        pView->SetAttributes( aSet, FALSE );
    }
    return bRet;
}
