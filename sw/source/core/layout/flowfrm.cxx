/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: flowfrm.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 04:10:24 $
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
#pragma hdrstop

#include "pam.hxx"
#include "swtable.hxx"
#include "frame.hxx"
#include "pagefrm.hxx"
#include "flyfrm.hxx"
#include "viewsh.hxx"
#include "doc.hxx"
#include "viewimp.hxx"
#include "dflyobj.hxx"
#include "frmtool.hxx"
#include "dcontact.hxx"

#ifndef _SVX_BRKITEM_HXX //autogen
#include <svx/brkitem.hxx>
#endif
#ifndef _SVX_KEEPITEM_HXX //autogen
#include <svx/keepitem.hxx>
#endif

#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef SW_TGRDITEM_HXX
#include <tgrditem.hxx>
#endif
#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#ifndef _TXTFTN_HXX //autogen
#include <txtftn.hxx>
#endif
#ifndef _FMTFTN_HXX //autogen
#include <fmtftn.hxx>
#endif
#ifndef _SVX_PGRDITEM_HXX
#include <svx/pgrditem.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif

#include "ftnfrm.hxx"
#include "txtfrm.hxx"
#include "tabfrm.hxx"
#include "pagedesc.hxx"
#include "layact.hxx"
#include "frmsh.hxx"
#include "fmtornt.hxx"
#include "flyfrms.hxx"
#include "sectfrm.hxx"
#include "section.hxx"
#include "dbg_lay.hxx"
#include "lineinfo.hxx"
// OD 2004-03-02 #106629#
#ifndef _FMTCLBL_HXX
#include <fmtclbl.hxx>
#endif
// --> OD 2004-06-23 #i28701#
#ifndef _SORTEDOBJS_HXX
#include <sortedobjs.hxx>
#endif
#ifndef _LAYOUTER_HXX
#include <layouter.hxx>
#endif
// <--
// --> OD 2004-10-15 #i26945#
#ifndef _FMTFOLLOWTEXTFLOW_HXX
#include <fmtfollowtextflow.hxx>
#endif
// <--

BOOL SwFlowFrm::bMoveBwdJump = FALSE;


/*************************************************************************
|*
|*	SwFlowFrm::SwFlowFrm()
|*
|*	Ersterstellung		MA 26. Apr. 95
|*	Letzte Aenderung	MA 26. Apr. 95
|*
|*************************************************************************/


SwFlowFrm::SwFlowFrm( SwFrm &rFrm ) :
    rThis( rFrm ),
    pFollow( 0 )
{
    bLockJoin = bIsFollow = bCntntLock = bOwnFtnNum =
        bFtnLock = bFlyLock = FALSE;
}


/*************************************************************************
|*
|*  SwFlowFrm::IsFollowLocked()
|*     return TRUE if any follow has the JoinLocked flag
|*
|*************************************************************************/

sal_Bool SwFlowFrm::HasLockedFollow() const
{
    const SwFlowFrm* pFrm = GetFollow();
    while( pFrm )
    {
        if( pFrm->IsJoinLocked() )
            return sal_True;
        pFrm = pFrm->GetFollow();
    }
    return sal_False;
}

/*************************************************************************
|*
|*	SwFlowFrm::IsKeepFwdMoveAllowed()
|*
|*	Ersterstellung		MA 20. Jul. 94
|*	Letzte Aenderung	MA 16. May. 95
|*
|*************************************************************************/


BOOL SwFlowFrm::IsKeepFwdMoveAllowed()
{
    //Wenn der Vorgaenger das KeepAttribut traegt und auch dessen
    //Vorgaenger usw. bis zum ersten der Kette und fuer diesen das
    //IsFwdMoveAllowed ein FALSE liefert, so ist das Moven eben nicht erlaubt.
    SwFrm *pFrm = &rThis;
    if ( !pFrm->IsInFtn() )
        do
        {	if ( pFrm->GetAttrSet()->GetKeep().GetValue() )
                pFrm = pFrm->GetIndPrev();
            else
                return TRUE;
        } while ( pFrm );

                  //Siehe IsFwdMoveAllowed()
    return pFrm ? pFrm->GetIndPrev() != 0 : FALSE;
}

/*************************************************************************
|*
|*	  SwFlowFrm::CheckKeep()
|*
|*	  Beschreibung
|*	  Ersterstellung	MA 20. Jun. 95
|*	  Letzte Aenderung	MA 09. Apr. 97
|*
|*************************************************************************/


void SwFlowFrm::CheckKeep()
{
    //Den 'letzten' Vorgaenger mit KeepAttribut anstossen, denn
    //die ganze Truppe koennte zuruckrutschen.
    SwFrm *pPre = rThis.GetIndPrev();
    if( pPre->IsSctFrm() )
    {
        SwFrm *pLast = ((SwSectionFrm*)pPre)->FindLastCntnt();
        if( pLast && pLast->FindSctFrm() == pPre )
            pPre = pLast;
        else
            return;
    }
    SwFrm* pTmp;
    BOOL bKeep;
    while ( TRUE == (bKeep = pPre->GetAttrSet()->GetKeep().GetValue()) &&
            0 != ( pTmp = pPre->GetIndPrev() ) )
    {
        if( pTmp->IsSctFrm() )
        {
            SwFrm *pLast = ((SwSectionFrm*)pTmp)->FindLastCntnt();
            if( pLast && pLast->FindSctFrm() == pTmp )
                pTmp = pLast;
            else
                break;
        }
        pPre = pTmp;
    }
    if ( bKeep )
        pPre->InvalidatePos();
}

/*************************************************************************
|*
|*	SwFlowFrm::IsKeep()
|*
|*	Ersterstellung		MA 09. Apr. 97
|*	Letzte Aenderung	MA 09. Apr. 97
|*
|*************************************************************************/


BOOL SwFlowFrm::IsKeep( const SwBorderAttrs &rAttrs ) const
{
    // 1. The keep attribute is ignored inside footnotes
    // 2. For compatibility reasons, the keep attribute is
    //    ignored for frames inside table cells
    BOOL bKeep = !rThis.IsInFtn() &&
                 ( !rThis.IsInTab() || rThis.IsTabFrm() ) &&
                 rAttrs.GetAttrSet().GetKeep().GetValue();

    // Ignore keep attribute if there are break situations:
    if ( bKeep )
    {
        switch ( rAttrs.GetAttrSet().GetBreak().GetBreak() )
        {
            case SVX_BREAK_COLUMN_AFTER:
            case SVX_BREAK_COLUMN_BOTH:
            case SVX_BREAK_PAGE_AFTER:
            case SVX_BREAK_PAGE_BOTH:
                bKeep = FALSE;
        }
        if ( bKeep )
        {
            SwFrm *pNxt;
            if( 0 != (pNxt = rThis.FindNextCnt()) &&
                (!pFollow || pNxt != pFollow->GetFrm()))
            {
                const SwAttrSet* pSet = NULL;

                if ( pNxt->IsInTab() )
                {
                    SwTabFrm* pTab = pNxt->FindTabFrm();
                    if ( ! rThis.IsInTab() || rThis.FindTabFrm() != pTab )
                        pSet = &pTab->GetFmt()->GetAttrSet();
                }

                if ( ! pSet )
                    pSet = pNxt->GetAttrSet();

                ASSERT( pSet, "No AttrSet to check keep attribute" )

                if ( pSet->GetPageDesc().GetPageDesc() )
                    bKeep = FALSE;
                else switch ( pSet->GetBreak().GetBreak() )
                {
                    case SVX_BREAK_COLUMN_BEFORE:
                    case SVX_BREAK_COLUMN_BOTH:
                    case SVX_BREAK_PAGE_BEFORE:
                    case SVX_BREAK_PAGE_BOTH:
                        bKeep = FALSE;
                }
            }
        }
    }
    return bKeep;
}

/*************************************************************************
|*
|*	SwFlowFrm::BwdMoveNecessary()
|*
|*	Ersterstellung		MA 20. Jul. 94
|*	Letzte Aenderung	MA 02. May. 96
|*
|*************************************************************************/


BYTE SwFlowFrm::BwdMoveNecessary( const SwPageFrm *pPage, const SwRect &rRect )
{
    // Der return-Wert entscheidet mit,
    // ob auf Zurueckgeflossen werden muss, (3)
    // ob das gute alte WouldFit gerufen werden kann (0, 1)
    // oder ob ein Umhaengen und eine Probeformatierung sinnvoll ist (2)
    // dabei bedeutet Bit 1, dass Objekte an mir selbst verankert sind
    // und Bit 2, dass ich anderen Objekten ausweichen muss.

    //Wenn ein SurroundObj, dass einen Umfluss wuenscht mit dem Rect ueberlappt
    //ist der Fluss notwendig (weil die Verhaeltnisse nicht geschaetzt werden
    //koennen), es kann allerdings ggf. eine TestFormatierung stattfinden.
    //Wenn das SurroundObj ein Fly ist und ich selbst ein Lower bin oder der Fly
    //Lower von mir ist, so spielt er keine Rolle.
    //Wenn das SurroundObj in einem zeichengebunden Fly verankert ist, und ich
    //selbst nicht Lower dieses Zeichengebundenen Flys bin, so spielt der Fly
    //keine Rolle.
    //#32639# Wenn das Objekt bei mir verankert ist kann ich es
    //vernachlaessigen, weil es hoechstwahrscheinlich (!?) mitfliesst,
    //eine TestFormatierung ist dann allerdings nicht erlaubt!
    BYTE nRet = 0;
    SwFlowFrm *pTmp = this;
    do
    {   // Wenn an uns oder einem Follow Objekte haengen, so
        // kann keine ProbeFormatierung stattfinden, da absatzgebundene
        // nicht richtig beruecksichtigt wuerden und zeichengebundene sollten
        // gar nicht zur Probe formatiert werden.
        if( pTmp->GetFrm()->GetDrawObjs() )
            nRet = 1;
        pTmp = pTmp->GetFollow();
    } while ( !nRet && pTmp );
    if ( pPage->GetSortedObjs() )
    {
        // --> OD 2004-07-01 #i28701# - new type <SwSortedObjs>
        const SwSortedObjs &rObjs = *pPage->GetSortedObjs();
        ULONG nIndex = ULONG_MAX;
        for ( USHORT i = 0; nRet < 3 && i < rObjs.Count(); ++i )
        {
            // --> OD 2004-07-01 #i28701# - consider changed type of
            // <SwSortedObjs> entries.
            SwAnchoredObject* pObj = rObjs[i];
            const SwFrmFmt& rFmt = pObj->GetFrmFmt();
            const SwRect aRect( pObj->GetObjRect() );
            if ( aRect.IsOver( rRect ) &&
                 rFmt.GetSurround().GetSurround() != SURROUND_THROUGHT )
            {
                if( rThis.IsLayoutFrm() && //Fly Lower von This?
                    Is_Lower_Of( &rThis, pObj->GetDrawObj() ) )
                    continue;
                if( pObj->ISA(SwFlyFrm) )
                {
                    const SwFlyFrm *pFly = static_cast<const SwFlyFrm*>(pObj);
                    if ( pFly->IsAnLower( &rThis ) )//This Lower vom Fly?
                        continue;
                }

                const SwFrm* pAnchor = pObj->GetAnchorFrm();
                if ( pAnchor == &rThis )
                {
                    nRet |= 1;
                    continue;
                }

                //Nicht wenn das Objekt im Textfluss hinter mir verankert ist,
                //denn dann weiche ich ihm nicht aus.
                if ( ::IsFrmInSameKontext( pAnchor, &rThis ) )
                {
                    if ( rFmt.GetAnchor().GetAnchorId() == FLY_AT_CNTNT )
                    {
                        // Den Index des anderen erhalten wir immer ueber das Ankerattr.
                        ULONG nTmpIndex = rFmt.GetAnchor().GetCntntAnchor()->nNode.GetIndex();
                        // Jetzt wird noch ueberprueft, ob der aktuelle Absatz vor dem
                        // Anker des verdraengenden Objekts im Text steht, dann wird
                        // nicht ausgewichen.
                        // Der Index wird moeglichst ueber einen SwFmtAnchor ermittelt,
                        // da sonst recht teuer.
                        if( ULONG_MAX == nIndex )
                        {
                            const SwNode *pNode;
                            if ( rThis.IsCntntFrm() )
                                pNode = ((SwCntntFrm&)rThis).GetNode();
                            else if( rThis.IsSctFrm() )
                                pNode = ((SwSectionFmt*)((SwSectionFrm&)rThis).
                                        GetFmt())->GetSectionNode();
                            else
                            {
                                ASSERT( rThis.IsTabFrm(), "new FowFrm?" );
                                pNode = ((SwTabFrm&)rThis).GetTable()->
                                    GetTabSortBoxes()[0]->GetSttNd()->FindTableNode();
                            }
                            nIndex = pNode->GetIndex();
                        }
                        if( nIndex < nTmpIndex )
                            continue;
                    }
                }
                else
                    continue;

                nRet |= 2;
            }
        }
    }
    return nRet;
}

/*************************************************************************
|*
|*	SwFlowFrm::CutTree(), PasteTree(), MoveSubTree()
|*
|*	Beschreibung		Eine Spezialisierte Form des Cut() und Paste(), die
|*		eine ganze Kette umhaengt (naehmlich this und folgende). Dabei werden
|* 		nur minimale Operationen und Benachrichtigungen ausgefuehrt.
|*	Ersterstellung		MA 18. Mar. 93
|*	Letzte Aenderung	MA 18. May. 95
|*
|*************************************************************************/


SwLayoutFrm *SwFlowFrm::CutTree( SwFrm *pStart )
{
    //Der Start und alle Nachbarn werden ausgeschnitten, sie werden aneinander-
    //gereiht und ein Henkel auf den ersten wird zurueckgeliefert.
    //Zurueckbleibende werden geeignet invalidiert.

    SwLayoutFrm *pLay = pStart->GetUpper();
    if ( pLay->IsInFtn() )
        pLay = pLay->FindFtnFrm();
    if( pLay )
    {
        SwFrm* pTmp = pStart->GetIndPrev();
        if( pTmp )
            pTmp->Prepare( PREP_QUOVADIS );
    }

    //Nur fix auschneiden und zwar so, dass klare Verhaeltnisse bei den
    //Verlassenen herrschen. Die Pointer der ausgeschnittenen Kette zeigen
    //noch wer weiss wo hin.
    if ( pStart == pStart->GetUpper()->Lower() )
        pStart->GetUpper()->pLower = 0;
    if ( pStart->GetPrev() )
    {
        pStart->GetPrev()->pNext = 0;
        pStart->pPrev = 0;
    }

    if ( pLay->IsFtnFrm() )
    {   if ( !pLay->Lower() && !pLay->IsColLocked() &&
             !((SwFtnFrm*)pLay)->IsBackMoveLocked() )
        {	pLay->Cut();
            delete pLay;
        }
        else
        {   BOOL bUnlock = !((SwFtnFrm*)pLay)->IsBackMoveLocked();
            ((SwFtnFrm*)pLay)->LockBackMove();
            pLay->InvalidateSize();
            pLay->Calc();
            SwCntntFrm *pCnt = pLay->ContainsCntnt();
            while ( pCnt && pLay->IsAnLower( pCnt ) )
            {
                //Kann sein, dass der CntFrm gelockt ist, wir wollen hier nicht
                //in eine endlose Seitenwanderung hineinlaufen und rufen das
                //Calc garnicht erst!
                ASSERT( pCnt->IsTxtFrm(), "Die Graphic ist gelandet." );
                if ( ((SwTxtFrm*)pCnt)->IsLocked() ||
                     ((SwTxtFrm*)pCnt)->GetFollow() == pStart )
                    break;
                pCnt->Calc();
                pCnt = pCnt->GetNextCntntFrm();
            }
            if( bUnlock )
                ((SwFtnFrm*)pLay)->UnlockBackMove();
        }
        pLay = 0;
    }
    return pLay;
}



BOOL SwFlowFrm::PasteTree( SwFrm *pStart, SwLayoutFrm *pParent, SwFrm *pSibling,
                           SwFrm *pOldParent )
{
    //returnt TRUE wenn in der Kette ein LayoutFrm steht.
    BOOL bRet = FALSE;

    //Die mit pStart beginnende Kette wird vor den Sibling unter den Parent
    //gehaengt. Fuer geeignete Invalidierung wird ebenfalls gesorgt.

    //Ich bekomme eine fertige Kette. Der Anfang der Kette muss verpointert
    //werden, dann alle Upper fuer die Kette und schliesslich dass Ende.
    //Unterwegs werden alle geeignet invalidiert.
    if ( pSibling )
    {
        if ( 0 != (pStart->pPrev = pSibling->GetPrev()) )
            pStart->GetPrev()->pNext = pStart;
        else
            pParent->pLower = pStart;
        pSibling->_InvalidatePos();
        pSibling->_InvalidatePrt();
    }
    else
    {
        if ( 0 == (pStart->pPrev = pParent->Lower()) )
            pParent->pLower = pStart;
        else
            pParent->Lower()->pNext = pStart;

        // #i27145#
        if ( pParent->IsSctFrm() )
        {
            // We have no sibling because pParent is a section frame and
            // has just been created to contain some content. The printing
            // area of the frame behind pParent has to be invalidated, so
            // that the correct distance between pParent and the next frame
            // can be calculated.
            pParent->InvalidateNextPrtArea();
        }
    }
    SwFrm *pFloat = pStart;
    SwFrm *pLst;
    SWRECTFN( pParent )
    SwTwips nGrowVal = 0;
    do
    {	pFloat->pUpper = pParent;
        pFloat->_InvalidateAll();
        pFloat->CheckDirChange();

        //Ich bin Freund des TxtFrm und darf deshalb so einiges. Das mit
        //dem CacheIdx scheint etwas riskant!
        if ( pFloat->IsTxtFrm() )
        {
            if ( ((SwTxtFrm*)pFloat)->GetCacheIdx() != USHRT_MAX )
                ((SwTxtFrm*)pFloat)->Init();	//Ich bin sein Freund.
        }
        else
            bRet = TRUE;

        nGrowVal += (pFloat->Frm().*fnRect->fnGetHeight)();
        if ( pFloat->GetNext() )
            pFloat = pFloat->GetNext();
        else
        {
            pLst = pFloat;
            pFloat = 0;
        }
    } while ( pFloat );

    if ( pSibling )
    {
        pLst->pNext = pSibling;
        pSibling->pPrev = pLst;
        if( pSibling->IsInFtn() )
        {
            if( pSibling->IsSctFrm() )
                pSibling = ((SwSectionFrm*)pSibling)->ContainsAny();
            if( pSibling )
                pSibling->Prepare( PREP_ERGOSUM );
        }
    }
    if ( nGrowVal )
    {
        if ( pOldParent && pOldParent->IsBodyFrm() ) //Fuer variable Seitenhoehe beim Browsen
            pOldParent->Shrink( nGrowVal PHEIGHT );
        pParent->Grow( nGrowVal PHEIGHT );
    }

    if ( pParent->IsFtnFrm() )
        ((SwFtnFrm*)pParent)->InvalidateNxtFtnCnts( pParent->FindPageFrm() );
    return bRet;
}



void SwFlowFrm::MoveSubTree( SwLayoutFrm* pParent, SwFrm* pSibling )
{
    ASSERT( pParent, "Kein Parent uebergeben." );
    ASSERT( rThis.GetUpper(), "Wo kommen wir denn her?" );

    //Sparsamer benachrichtigen wenn eine Action laeuft.
    ViewShell *pSh = rThis.GetShell();
    const SwViewImp *pImp = pSh ? pSh->Imp() : 0;
    const BOOL bComplete = pImp && pImp->IsAction() && pImp->GetLayAction().IsComplete();

    if ( !bComplete )
    {
        SwFrm *pPre = rThis.GetIndPrev();
        if ( pPre )
        {
            pPre->SetRetouche();
            // --> OD 2004-11-23 #115759# - follow-up of #i26250#
            // invalidate printing area of previous frame, if it's in a table
            if ( pPre->IsInTab() )
            {
                pPre->_InvalidatePrt();
            }
            // <--
            pPre->InvalidatePage();
        }
        else
        {	rThis.GetUpper()->SetCompletePaint();
            rThis.GetUpper()->InvalidatePage();
        }
    }

    SwPageFrm *pOldPage = rThis.FindPageFrm();

    SwLayoutFrm *pOldParent = CutTree( &rThis );
    const BOOL bInvaLay = PasteTree( &rThis, pParent, pSibling, pOldParent );

    // Wenn durch das Cut&Paste ein leerer SectionFrm entstanden ist, sollte
    // dieser automatisch verschwinden.
    SwSectionFrm *pSct;
    if ( pOldParent && !pOldParent->Lower() &&
         (pOldParent->IsInSct() &&
          !(pSct = pOldParent->FindSctFrm())->ContainsCntnt() ) )
            pSct->DelEmpty( FALSE );

    // In einem spaltigen Bereich rufen wir lieber kein Calc "von unten"
    if( !rThis.IsInSct() &&
        ( !rThis.IsInTab() || ( rThis.IsTabFrm() && !rThis.GetUpper()->IsInTab() ) ) )
        rThis.GetUpper()->Calc();
    else if( rThis.GetUpper()->IsSctFrm() )
    {
        SwSectionFrm* pSct = (SwSectionFrm*)rThis.GetUpper();
        BOOL bOld = pSct->IsCntntLocked();
        pSct->SetCntntLock( TRUE );
        pSct->Calc();
        if( !bOld )
            pSct->SetCntntLock( FALSE );
    }
    SwPageFrm *pPage = rThis.FindPageFrm();

    if ( pOldPage != pPage )
    {
        rThis.InvalidatePage( pPage );
        if ( rThis.IsLayoutFrm() )
        {
            SwCntntFrm *pCnt = ((SwLayoutFrm*)&rThis)->ContainsCntnt();
            if ( pCnt )
                pCnt->InvalidatePage( pPage );
        }
        else if ( pSh && pSh->GetDoc()->GetLineNumberInfo().IsRestartEachPage()
                  && pPage->FindFirstBodyCntnt() == &rThis )
        {
            rThis._InvalidateLineNum();
        }
    }
    if ( bInvaLay || (pSibling && pSibling->IsLayoutFrm()) )
        rThis.GetUpper()->InvalidatePage( pPage );
}

/*************************************************************************
|*
|*	SwFlowFrm::IsAnFollow()
|*
|*	Ersterstellung		MA 26. Apr. 95
|*	Letzte Aenderung	MA 26. Apr. 95
|*
|*************************************************************************/


BOOL SwFlowFrm::IsAnFollow( const SwFlowFrm *pAssumed ) const
{
    const SwFlowFrm *pFoll = this;
    do
    {	if ( pAssumed == pFoll )
            return TRUE;
        pFoll = pFoll->GetFollow();
    } while ( pFoll );
    return FALSE;
}


/*************************************************************************
|*
|*	SwFlowFrm::FindMaster()
|*
|*	Ersterstellung		MA 26. Apr. 95
|*	Letzte Aenderung	MA 26. Apr. 95
|*
|*************************************************************************/

SwTxtFrm* SwCntntFrm::FindMaster() const
{
    ASSERT( IsFollow(), "SwCntntFrm::FindMaster(): !IsFollow" );

    const SwCntntFrm* pCnt = GetPrevCntntFrm();

    while ( pCnt )
    {
        if ( pCnt->HasFollow() && pCnt->GetFollow() == this )
        {
            ASSERT( pCnt->IsTxtFrm(), "NoTxtFrm with follow found" )
            return (SwTxtFrm*)pCnt;
        }
        pCnt = pCnt->GetPrevCntntFrm();
    }

    ASSERT( FALSE, "Follow ist lost in Space." );
    return 0;
}

SwSectionFrm* SwSectionFrm::FindMaster() const
{
    ASSERT( IsFollow(), "SwSectionFrm::FindMaster(): !IsFollow" );

    SwClientIter aIter( *pSection->GetFmt() );
    SwClient *pLast = aIter.GoStart();

    while ( pLast )
    {
        if ( pLast->ISA( SwFrm ) )
        {
            ASSERT( ((SwFrm*)pLast)->IsSctFrm(),
                    "Non-section frame registered in section format" )
            SwSectionFrm* pSect = (SwSectionFrm*)pLast;
            if( pSect->GetFollow() == this )
                return pSect;
        }
        pLast = aIter++;
    }

    ASSERT( FALSE, "Follow ist lost in Space." );
    return 0;
}

SwTabFrm* SwTabFrm::FindMaster( bool bFirstMaster ) const
{
    ASSERT( IsFollow(), "SwTabFrm::FindMaster(): !IsFollow" );

    SwClientIter aIter( *GetTable()->GetFrmFmt() );
    SwClient* pLast = aIter.GoStart();

    while ( pLast )
    {
        if ( pLast->ISA( SwFrm ) )
        {
            ASSERT( ((SwFrm*)pLast)->IsTabFrm(),
                    "Non-table frame registered in table format" )
            SwTabFrm* pTab = (SwTabFrm*)pLast;

            if ( bFirstMaster )
            {
                //
                // Optimization. This makes code like this obsolete:
                // while ( pTab->IsFollow() )
                //     pTab = pTab->FindMaster();
                //
                if ( !pTab->IsFollow() )
                {
                    SwTabFrm* pNxt = pTab;
                    while ( pNxt )
                    {
                        if ( pNxt->GetFollow() == this )
                            return pTab;
                        pNxt = pNxt->GetFollow();
                    }
                }
            }
            else
            {
                if ( pTab->GetFollow() == this )
                    return pTab;
            }
        }
        pLast = aIter++;
    }

    ASSERT( FALSE, "Follow ist lost in Space." );
    return 0;
}

/*************************************************************************
|*
|*	SwFrm::GetLeaf()
|*
|*	Beschreibung		Liefert das naechste/vorhergehende LayoutBlatt,
|* 		das _nicht_ unterhalb von this liegt (oder gar this selbst ist).
|* 		Ausserdem muss dieses LayoutBlatt im gleichen Textfluss wie
|* 		pAnch Ausgangsfrm liegen (Body, Ftn)
|*	Ersterstellung		MA 25. Nov. 92
|*	Letzte Aenderung	MA 25. Apr. 95
|*
|*************************************************************************/


const SwLayoutFrm *SwFrm::GetLeaf( MakePageType eMakePage, BOOL bFwd,
                                   const SwFrm *pAnch ) const
{
    //Ohne Fluss kein genuss...
    if ( !(IsInDocBody() || IsInFtn() || IsInFly()) )
        return 0;

    const SwFrm *pLeaf = this;
    BOOL bFound = FALSE;

    do
    {	pLeaf = ((SwFrm*)pLeaf)->GetLeaf( eMakePage, bFwd );

        if ( pLeaf &&
            (!IsLayoutFrm() || !((SwLayoutFrm*)this)->IsAnLower( pLeaf )))
        {
            if ( pAnch->IsInDocBody() == pLeaf->IsInDocBody() &&
                 pAnch->IsInFtn()	  == pLeaf->IsInFtn() )
            {
                bFound = TRUE;
            }
        }
    } while ( !bFound && pLeaf );

    return (const SwLayoutFrm*)pLeaf;
}

/*************************************************************************
|*
|*	SwFrm::GetLeaf()
|*
|*	Beschreibung		Ruft Get[Next|Prev]Leaf
|*
|*	Ersterstellung		MA 20. Mar. 93
|*	Letzte Aenderung	MA 25. Apr. 95
|*
|*************************************************************************/


SwLayoutFrm *SwFrm::GetLeaf( MakePageType eMakePage, BOOL bFwd )
{
    if ( IsInFtn() )
        return bFwd ? GetNextFtnLeaf( eMakePage ) : GetPrevFtnLeaf( eMakePage );

    if ( IsInTab() && ( !IsTabFrm() || GetUpper()->IsCellFrm() ) ) // TABLE IN TABLE
        return bFwd ? GetNextCellLeaf( eMakePage ) : GetPrevCellLeaf( eMakePage );

    if ( IsInSct() )
        return bFwd ? GetNextSctLeaf( eMakePage ) : GetPrevSctLeaf( eMakePage );

    return bFwd ? GetNextLeaf( eMakePage ) : GetPrevLeaf( eMakePage );
}



BOOL SwFrm::WrongPageDesc( SwPageFrm* pNew )
{
    //Jetzt wirds leider etwas kompliziert:
    //Ich bringe ich evtl. selbst
    //einen Pagedesc mit; der der Folgeseite muss dann damit
    //uebereinstimmen.
    //Anderfalls muss ich mir etwas genauer ansehen wo der
    //Folgepagedesc herkam.
    //Wenn die Folgeseite selbst schon sagt, dass ihr
    //Pagedesc nicht stimmt so kann ich das Teil bedenkenlos
    //auswechseln.
    //Wenn die Seite meint, dass ihr Pagedesc stimmt, so heisst
    //das leider noch immer nicht, dass ich damit etwas anfangen
    //kann: Wenn der erste BodyCntnt einen PageDesc oder einen
    //PageBreak wuenscht, so muss ich ebenfalls eine neue
    //Seite einfuegen; es sein denn die gewuenschte Seite ist
    //die richtige.
    //Wenn ich einen neue Seite eingefuegt habe, so fangen die
    //Probleme leider erst an, denn wahrscheinlich wird die dann
    //folgende Seite verkehrt gewesen und ausgewechselt worden
    //sein. Das hat zur Folge, dass ich zwar eine neue (und
    //jetzt richtige) Seite habe, die Bedingungen zum auswechseln
    //aber leider noch immer stimmen.
    //Ausweg: Vorlaeufiger Versuch, nur einmal eine neue Seite
    //einsetzen (Leerseiten werden noetigenfalls bereits von
    //InsertPage() eingefuegt.
    const SwFmtPageDesc &rFmtDesc = GetAttrSet()->GetPageDesc();

    //Mein Pagedesc zaehlt nicht, wenn ich ein Follow bin!
    SwPageDesc *pDesc = 0;
    USHORT nTmp = 0;
    SwFlowFrm *pFlow = SwFlowFrm::CastFlowFrm( this );
    if ( !pFlow || !pFlow->IsFollow() )
    {
        pDesc = (SwPageDesc*)rFmtDesc.GetPageDesc();
        if( pDesc )
        {
            if( !pDesc->GetRightFmt() )
                nTmp = 2;
            else if( !pDesc->GetLeftFmt() )
                nTmp = 1;
            else if( rFmtDesc.GetNumOffset() )
                nTmp = rFmtDesc.GetNumOffset();
        }
    }

    //Bringt der Cntnt einen Pagedesc mit oder muss zaehlt die
    //virtuelle Seitennummer des neuen Layoutleafs?
    // Bei Follows zaehlt der PageDesc nicht
    const BOOL bOdd = nTmp ? ( nTmp % 2 ? TRUE : FALSE )
                           : pNew->OnRightPage();
    if ( !pDesc )
        pDesc = pNew->FindPageDesc();
    const SwFlowFrm *pNewFlow = pNew->FindFirstBodyCntnt();
    // Haben wir uns selbst gefunden?
    if( pNewFlow == pFlow )
        pNewFlow = NULL;
    if ( pNewFlow && pNewFlow->GetFrm()->IsInTab() )
        pNewFlow = pNewFlow->GetFrm()->FindTabFrm();
    const SwPageDesc *pNewDesc= ( pNewFlow && !pNewFlow->IsFollow() )
            ? pNewFlow->GetFrm()->GetAttrSet()->GetPageDesc().GetPageDesc():0;

    return ( pNew->GetPageDesc() != pDesc ||   //  own desc ?
        pNew->GetFmt() != (bOdd ? pDesc->GetRightFmt() : pDesc->GetLeftFmt()) ||
        ( pNewDesc && pNewDesc == pDesc ) );
}


/*************************************************************************
|*
|*	SwFrm::GetNextLeaf()
|*
|*	Beschreibung		Liefert das naechste LayoutBlatt in den das
|* 		Frame gemoved werden kann.
|*
|*	Ersterstellung		MA 16. Nov. 92
|*	Letzte Aenderung	MA 05. Dec. 96
|*
|*************************************************************************/

SwLayoutFrm *SwFrm::GetNextLeaf( MakePageType eMakePage )
{
    ASSERT( !IsInFtn(), "GetNextLeaf(), don't call me for Ftn." );
    ASSERT( !IsInSct(), "GetNextLeaf(), don't call me for Sections." );

    const BOOL bBody = IsInDocBody();		//Wenn ich aus dem DocBody komme
                                            //Will ich auch im Body landen.

    // Bei Flys macht es keinen Sinn, Seiten einzufuegen, wir wollen lediglich
     // die Verkettung absuchen.
    if( IsInFly() )
        eMakePage = MAKEPAGE_NONE;

    //Bei Tabellen gleich den grossen Sprung wagen, ein einfaches GetNext...
    //wuerde die erste Zellen und in der Folge alle weiteren Zellen nacheinander
    //abklappern....
    SwLayoutFrm *pLayLeaf = 0;
    if ( IsTabFrm() )
    {
        SwCntntFrm* pTmp = ((SwTabFrm*)this)->FindLastCntnt();
        if ( pTmp )
            pLayLeaf = pTmp->GetUpper();
    }
    if ( !pLayLeaf )
        pLayLeaf = GetNextLayoutLeaf();

    SwLayoutFrm *pOldLayLeaf = 0;			//Damit bei neu erzeugten Seiten
                                            //nicht wieder vom Anfang gesucht
                                            //wird.
    BOOL bNewPg = FALSE;	//nur einmal eine neue Seite einfuegen.

    while ( TRUE )
    {
        if ( pLayLeaf )
        {
            //Es gibt noch einen weiteren LayoutFrm, mal sehen,
            //ob er bereit ist mich aufzunehmen.
            //Dazu braucht er nur von der gleichen Art wie mein Ausgangspunkt
            //sein (DocBody bzw. Footnote.)
            if ( pLayLeaf->FindPageFrm()->IsFtnPage() )
            {	//Wenn ich bei den Endnotenseiten angelangt bin hat sichs.
                pLayLeaf = 0;
                continue;
            }
            if ( (bBody && !pLayLeaf->IsInDocBody()) || pLayLeaf->IsInTab()
                 || pLayLeaf->IsInSct() )
            {
                //Er will mich nicht; neuer Versuch, neues Glueck
                pOldLayLeaf = pLayLeaf;
                pLayLeaf = pLayLeaf->GetNextLayoutLeaf();
                continue;
            }
            //Er will mich, also ist er der gesuchte und ich bin fertig.
            //Bei einem Seitenwechsel kann es allerdings noch sein, dass
            //Der Seitentyp nicht der gewuenschte ist, in diesem Fall muessen
            //wir eine Seite des richtigen Typs einfuegen.

            if( !IsFlowFrm() && ( eMakePage == MAKEPAGE_NONE ||
                eMakePage==MAKEPAGE_APPEND || eMakePage==MAKEPAGE_NOSECTION ) )
                return pLayLeaf;

            SwPageFrm *pNew = pLayLeaf->FindPageFrm();
            // #111704# The pagedesc check does not make sense for frames in fly frames
            if ( pNew != FindPageFrm() && !bNewPg && !IsInFly() )
            {
                if( WrongPageDesc( pNew ) )
                {
                    SwFtnContFrm *pCont = pNew->FindFtnCont();
                    if( pCont )
                    {
                        // Falls die Referenz der ersten Fussnote dieser Seite
                        // vor der Seite liegt, fuegen wir lieber keine neue Seite
                        // ein (Bug #55620#)
                        SwFtnFrm *pFtn = (SwFtnFrm*)pCont->Lower();
                        if( pFtn && pFtn->GetRef() )
                        {
                            const USHORT nRefNum = pNew->GetPhyPageNum();
                            if( pFtn->GetRef()->GetPhyPageNum() < nRefNum )
                                break;
                        }
                    }
                    //Erwischt, die folgende Seite ist verkehrt, also
                    //muss eine neue eingefuegt werden.
                    if ( eMakePage == MAKEPAGE_INSERT )
                    {
                        bNewPg = TRUE;

                        SwPageFrm *pPg = pOldLayLeaf ?
                                    pOldLayLeaf->FindPageFrm() : 0;
                        if ( pPg && pPg->IsEmptyPage() )
                            //Nicht hinter, sondern vor der EmptyPage einfuegen.
                            pPg = (SwPageFrm*)pPg->GetPrev();

                        if ( !pPg || pPg == pNew )
                            pPg = FindPageFrm();

                        InsertPage( pPg, FALSE );
                        pLayLeaf = GetNextLayoutLeaf();
                        pOldLayLeaf = 0;
                        continue;
                    }
                    else
                        pLayLeaf = 0;
                }
            }
            break;
        }
        else
        {
            //Es gibt keinen passenden weiteren LayoutFrm, also muss eine
            //neue Seite her.
            if ( eMakePage == MAKEPAGE_APPEND || eMakePage == MAKEPAGE_INSERT )
            {
                InsertPage(
                    pOldLayLeaf ? pOldLayLeaf->FindPageFrm() : FindPageFrm(),
                    FALSE );

                //und nochmal das ganze
                pLayLeaf = pOldLayLeaf ? pOldLayLeaf : GetNextLayoutLeaf();
            }
            else
                break;
        }
    }
    return pLayLeaf;
}

/*************************************************************************
|*
|*	SwFrm::GetPrevLeaf()
|*
|*	Beschreibung		Liefert das vorhergehende LayoutBlatt in das der
|* 		Frame gemoved werden kann.
|*	Ersterstellung		MA 16. Nov. 92
|*	Letzte Aenderung	MA 25. Apr. 95
|*
|*************************************************************************/


SwLayoutFrm *SwFrm::GetPrevLeaf( MakePageType eMakeFtn )
{
    ASSERT( !IsInFtn(), "GetPrevLeaf(), don't call me for Ftn." );

    const BOOL bBody = IsInDocBody();		//Wenn ich aus dem DocBody komme
                                            //will ich auch im Body landen.
    const BOOL bFly  = IsInFly();

    SwLayoutFrm *pLayLeaf = GetPrevLayoutLeaf();
    SwLayoutFrm *pPrevLeaf = 0;

    while ( pLayLeaf )
    {
        if ( pLayLeaf->IsInTab() || 	//In Tabellen geht's niemals hinein.
             pLayLeaf->IsInSct() )		//In Bereiche natuerlich auch nicht!
            pLayLeaf = pLayLeaf->GetPrevLayoutLeaf();
        else if ( bBody && pLayLeaf->IsInDocBody() )
        {
            if ( pLayLeaf->Lower() )
                break;
            pPrevLeaf = pLayLeaf;
            pLayLeaf = pLayLeaf->GetPrevLayoutLeaf();
            if ( pLayLeaf )
                SwFlowFrm::SetMoveBwdJump( TRUE );
        }
        else if ( bFly )
            break;	//Cntnts in Flys sollte jedes Layout-Blatt recht sein.
        else
            pLayLeaf = pLayLeaf->GetPrevLayoutLeaf();
    }
    return pLayLeaf ? pLayLeaf : pPrevLeaf;
}

/*************************************************************************
|*
|*	SwFlowFrm::IsPrevObjMove()
|*
|*	Ersterstellung		MA 20. Feb. 96
|*	Letzte Aenderung	MA 22. Feb. 96
|*
|*************************************************************************/


BOOL SwFlowFrm::IsPrevObjMove() const
{
    //TRUE der FlowFrm soll auf einen Rahmen des Vorgaengers Ruecksicht nehmen
    //     und fuer diesen ggf. Umbrechen.

    //!!!!!!!!!!!Hack!!!!!!!!!!!
    if ( rThis.GetUpper()->GetFmt()->GetDoc()->IsBrowseMode() )
        return FALSE;

    SwFrm *pPre = rThis.FindPrev();

    if ( pPre && pPre->GetDrawObjs() )
    {
        ASSERT( SwFlowFrm::CastFlowFrm( pPre ),	"new flowfrm?" );
        if(	SwFlowFrm::CastFlowFrm( pPre )->IsAnFollow( this ) )
            return FALSE;
        SwLayoutFrm* pPreUp = pPre->GetUpper();
        // Wenn der Upper ein SectionFrm oder die Spalte eines SectionFrms ist,
        // duerfen wir aus diesem durchaus heraushaengen,
        // es muss stattdessen der Upper des SectionFrms beruecksichtigt werden.
        if( pPreUp->IsInSct() )
        {
            if( pPreUp->IsSctFrm() )
                pPreUp = pPreUp->GetUpper();
            else if( pPreUp->IsColBodyFrm() &&
                     pPreUp->GetUpper()->GetUpper()->IsSctFrm() )
                pPreUp = pPreUp->GetUpper()->GetUpper()->GetUpper();
        }
        // --> OD 2004-10-15 #i26945# - re-factoring:
        // use <GetVertPosOrientFrm()> to determine, if object has followed the
        // text flow to the next layout frame
        for ( USHORT i = 0; i < pPre->GetDrawObjs()->Count(); ++i )
        {
            // --> OD 2004-07-01 #i28701# - consider changed type of
            // <SwSortedObjs> entries.
            const SwAnchoredObject* pObj = (*pPre->GetDrawObjs())[i];
            // OD 2004-01-20 #110582# - do not consider hidden objects
            // --> OD 2004-10-15 #i26945# - do not consider object, which
            // doesn't follow the text flow.
            if ( pObj->GetFrmFmt().GetDoc()->IsVisibleLayerId(
                                            pObj->GetDrawObj()->GetLayer() ) &&
                 pObj->GetFrmFmt().GetFollowTextFlow().GetValue() )
            // <--
            {
                const SwLayoutFrm* pVertPosOrientFrm = pObj->GetVertPosOrientFrm();
                if ( pVertPosOrientFrm &&
                     pPreUp != pVertPosOrientFrm &&
                     !pPreUp->IsAnLower( pVertPosOrientFrm ) )
                {
                    return TRUE;
                }
            }
        }
        // <--
    }
    return FALSE;
}

/*************************************************************************
|*
|*	BOOL SwFlowFrm::IsPageBreak()
|*
|*	Beschreibung		Wenn vor dem Frm ein harter Seitenumbruch steht UND
|*		es einen Vorgaenger auf der gleichen Seite gibt, wird TRUE
|*		zurueckgeliefert (es muss ein PageBreak erzeugt werden) FALSE sonst.
|*		Wenn in bAct TRUE uebergeben wird, gibt die Funktion dann TRUE
|*		zurueck, wenn ein PageBreak besteht.
|* 		Fuer Follows wird der harte Seitenumbruch natuerlich nicht
|* 		ausgewertet.
|*		Der Seitenumbruch steht im eigenen FrmFmt (BEFORE) oder im FrmFmt
|*		des Vorgaengers (AFTER). Wenn es keinen Vorgaenger auf der Seite
|*		gibt ist jede weitere Ueberlegung ueberfluessig.
|* 		Ein Seitenumbruch (oder der Bedarf) liegt auch dann vor, wenn
|* 		im FrmFmt ein PageDesc angegeben wird.
|*		Die Implementierung arbeitet zuaechst nur auf CntntFrms!
|*		-->Fuer LayoutFrms ist die Definition des Vorgaengers unklar.
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 21. Mar. 95
|*
|*************************************************************************/


BOOL SwFlowFrm::IsPageBreak( BOOL bAct ) const
{
    const SwAttrSet *pSet;
    if ( !IsFollow() && rThis.IsInDocBody() &&
         ( !rThis.IsInTab() || rThis.IsTabFrm() ) &&
         !(pSet = rThis.GetAttrSet())->GetDoc()->IsBrowseMode() )
    {
        //Vorgaenger ermitteln
        const SwFrm *pPrev = rThis.FindPrev();
        while ( pPrev && ( !pPrev->IsInDocBody() ||
                ( pPrev->IsTxtFrm() && ((SwTxtFrm*)pPrev)->IsHiddenNow() ) ) )
            pPrev = pPrev->FindPrev();

        if ( pPrev )
        {
            ASSERT( pPrev->IsInDocBody(), "IsPageBreak: Not in DocBody?" );
            if ( bAct )
            {	if ( rThis.FindPageFrm() == pPrev->FindPageFrm() )
                    return FALSE;
            }
            else
            {	if ( rThis.FindPageFrm() != pPrev->FindPageFrm() )
                    return FALSE;
            }

            const SvxBreak eBreak = pSet->GetBreak().GetBreak();
            if ( eBreak == SVX_BREAK_PAGE_BEFORE || eBreak == SVX_BREAK_PAGE_BOTH )
                return TRUE;
            else
            {
                const SvxBreak &ePrB = pPrev->GetAttrSet()->GetBreak().GetBreak();
                if ( ePrB == SVX_BREAK_PAGE_AFTER ||
                     ePrB == SVX_BREAK_PAGE_BOTH  ||
                     pSet->GetPageDesc().GetPageDesc() )
                    return TRUE;
            }
        }
    }
    return FALSE;
}

/*************************************************************************
|*
|*	BOOL SwFlowFrm::IsColBreak()
|*
|*	Beschreibung		Wenn vor dem Frm ein harter Spaltenumbruch steht UND
|*		es einen Vorgaenger in der gleichen Spalte gibt, wird TRUE
|*		zurueckgeliefert (es muss ein PageBreak erzeugt werden) FALSE sonst.
|*		Wenn in bAct TRUE uebergeben wird, gibt die Funktion dann TRUE
|*		zurueck, wenn ein ColBreak besteht.
|* 		Fuer Follows wird der harte Spaltenumbruch natuerlich nicht
|* 		ausgewertet.
|*		Der Spaltenumbruch steht im eigenen FrmFmt (BEFORE) oder im FrmFmt
|*		des Vorgaengers (AFTER). Wenn es keinen Vorgaenger in der Spalte
|*		gibt ist jede weitere Ueberlegung ueberfluessig.
|*		Die Implementierung arbeitet zuaechst nur auf CntntFrms!
|*		-->Fuer LayoutFrms ist die Definition des Vorgaengers unklar.
|*	Ersterstellung		MA 11. Jun. 93
|*	Letzte Aenderung	MA 21. Mar. 95
|*
|*************************************************************************/


BOOL SwFlowFrm::IsColBreak( BOOL bAct ) const
{
    if ( !IsFollow() && (rThis.IsMoveable() || bAct) )
    {
        const SwFrm *pCol = rThis.FindColFrm();
        if ( pCol )
        {
            //Vorgaenger ermitteln
            const SwFrm *pPrev = rThis.FindPrev();
            while( pPrev && ( ( !pPrev->IsInDocBody() && !rThis.IsInFly() ) ||
                   ( pPrev->IsTxtFrm() && ((SwTxtFrm*)pPrev)->IsHiddenNow() ) ) )
                    pPrev = pPrev->FindPrev();

            if ( pPrev )
            {
                if ( bAct )
                {	if ( pCol == pPrev->FindColFrm() )
                        return FALSE;
                }
                else
                {	if ( pCol != pPrev->FindColFrm() )
                        return FALSE;
                }

                const SvxBreak eBreak = rThis.GetAttrSet()->GetBreak().GetBreak();
                if ( eBreak == SVX_BREAK_COLUMN_BEFORE ||
                     eBreak == SVX_BREAK_COLUMN_BOTH )
                    return TRUE;
                else
                {
                    const SvxBreak &ePrB = pPrev->GetAttrSet()->GetBreak().GetBreak();
                    if ( ePrB == SVX_BREAK_COLUMN_AFTER ||
                         ePrB == SVX_BREAK_COLUMN_BOTH )
                        return TRUE;
                }
            }
        }
    }
    return FALSE;
}

BOOL SwFlowFrm::HasParaSpaceAtPages( BOOL bSct ) const
{
    if( rThis.IsInSct() )
    {
        const SwFrm* pTmp = rThis.GetUpper();
        while( pTmp )
        {
            if( pTmp->IsCellFrm() || pTmp->IsFlyFrm() ||
                pTmp->IsFooterFrm() || pTmp->IsHeaderFrm() ||
                ( pTmp->IsFtnFrm() && !((SwFtnFrm*)pTmp)->GetMaster() ) )
                return TRUE;
            if( pTmp->IsPageFrm() )
                return ( pTmp->GetPrev() && !IsPageBreak(TRUE) ) ? FALSE : TRUE;
            if( pTmp->IsColumnFrm() && pTmp->GetPrev() )
                return IsColBreak( TRUE );
            if( pTmp->IsSctFrm() && ( !bSct || pTmp->GetPrev() ) )
                return FALSE;
            pTmp = pTmp->GetUpper();
        }
        ASSERT( FALSE, "HasParaSpaceAtPages: Where's my page?" );
        return FALSE;
    }
    if( !rThis.IsInDocBody() || ( rThis.IsInTab() && !rThis.IsTabFrm()) ||
        IsPageBreak( TRUE ) || ( rThis.FindColFrm() && IsColBreak( TRUE ) ) )
        return TRUE;
    const SwFrm* pTmp = rThis.FindColFrm();
    if( pTmp )
    {
        if( pTmp->GetPrev() )
            return FALSE;
    }
    else
        pTmp = &rThis;
    pTmp = pTmp->FindPageFrm();
    return pTmp && !pTmp->GetPrev();
}

/** helper method to determine previous frame for calculation of the
    upper space

    OD 2004-03-10 #i11860#

    @author OD
*/
const SwFrm* SwFlowFrm::_GetPrevFrmForUpperSpaceCalc( const SwFrm* _pProposedPrevFrm ) const
{
    const SwFrm* pPrevFrm = _pProposedPrevFrm
                            ? _pProposedPrevFrm
                            : rThis.GetPrev();

    // Skip hidden paragraphs and empty sections
    while ( pPrevFrm &&
            ( ( pPrevFrm->IsTxtFrm() &&
                static_cast<const SwTxtFrm*>(pPrevFrm)->IsHiddenNow() ) ||
              ( pPrevFrm->IsSctFrm() &&
                !static_cast<const SwSectionFrm*>(pPrevFrm)->GetSection() ) ) )
    {
        pPrevFrm = pPrevFrm->GetPrev();
    }

    // Special case: no direct previous frame is found but frame is in footnote
    // Search for a previous frame in previous footnote,
    // if frame isn't in a section, which is also in the footnote
    if ( !pPrevFrm && rThis.IsInFtn() &&
         ( rThis.IsSctFrm() ||
           !rThis.IsInSct() || !rThis.FindSctFrm()->IsInFtn() ) )
    {
        const SwFtnFrm* pPrevFtnFrm =
                static_cast<const SwFtnFrm*>(rThis.FindFtnFrm()->GetPrev());
        if ( pPrevFtnFrm )
        {
            pPrevFrm = pPrevFtnFrm->Lower();
            while ( pPrevFrm && pPrevFrm->GetNext() )
            {
                pPrevFrm = pPrevFrm->GetNext();
            }
            // Skip hidden paragraphs and empty sections
            while ( pPrevFrm &&
                    ( ( pPrevFrm->IsTxtFrm() &&
                        static_cast<const SwTxtFrm*>(pPrevFrm)->IsHiddenNow() ) ||
                      ( pPrevFrm->IsSctFrm() &&
                        !static_cast<const SwSectionFrm*>(pPrevFrm)->GetSection() ) ) )
            {
                pPrevFrm = pPrevFrm->GetPrev();
            }
        }
    }
    // Special case: found previous frame is a section
    // Search for the last content in the section
    if( pPrevFrm && pPrevFrm->IsSctFrm() )
    {
        const SwSectionFrm* pPrevSectFrm =
                                    static_cast<const SwSectionFrm*>(pPrevFrm);
        pPrevFrm = pPrevSectFrm->FindLastCntnt();
        // If the last content is in a table _inside_ the section,
        // take the table herself.
        // OD 2004-02-18 #106629# - correction:
        // Check directly, if table is inside table, instead of indirectly
        // by checking, if section isn't inside a table
        if ( pPrevFrm && pPrevFrm->IsInTab() )
        {
            const SwTabFrm* pTableFrm = pPrevFrm->FindTabFrm();
            if ( pPrevSectFrm->IsAnLower( pTableFrm ) )
            {
                pPrevFrm = pTableFrm;
            }
        }
        // OD 2004-02-18 #106629# correction: skip hidden text frames
        while ( pPrevFrm &&
                pPrevFrm->IsTxtFrm() &&
                static_cast<const SwTxtFrm*>(pPrevFrm)->IsHiddenNow() )
        {
            pPrevFrm = pPrevFrm->GetPrev();
        }
    }

    return pPrevFrm;
}

// OD 2004-03-12 #i11860# - add 3rd parameter <_bConsiderGrid>
SwTwips SwFlowFrm::CalcUpperSpace( const SwBorderAttrs *pAttrs,
                                   const SwFrm* pPr,
                                   const bool _bConsiderGrid ) const
{
    // OD 2004-03-10 #i11860# - use new method <GetPrevFrmForUpperSpaceCalc(..)>
    const SwFrm* pPrevFrm = _GetPrevFrmForUpperSpaceCalc( pPr );

    SwBorderAttrAccess *pAccess;
    SwFrm* pOwn;
    if( !pAttrs )
    {
        if( rThis.IsSctFrm() )
        {
            SwSectionFrm* pFoll = &((SwSectionFrm&)rThis);
            do
                pOwn = pFoll->ContainsAny();
            while( !pOwn && 0 != ( pFoll = pFoll->GetFollow() ) );
            if( !pOwn )
                return 0;
        }
        else
            pOwn = &rThis;
        pAccess= new SwBorderAttrAccess( SwFrm::GetCache(), pOwn );
        pAttrs = pAccess->Get();
    }
    else
    {
        pAccess = NULL;
        pOwn = &rThis;
    }
    SwTwips nUpper = 0;
    // OD 06.01.2004 #i11859#
    {
        const SwAttrSet* pSet = rThis.GetAttrSet();
        const SvxLineSpacingItem &rSpace = pSet->GetLineSpacing();
        const SwDoc* pDoc = pSet->GetDoc();
        const bool bUseFormerLineSpacing = pDoc->IsFormerLineSpacing();
        if( pPrevFrm )
        {
            // OD 2004-03-10 #i11860# - use new method to determine needed spacing
            // values of found previous frame and use these values.
            SwTwips nPrevLowerSpace = 0;
            SwTwips nPrevLineSpacing = 0;
            GetSpacingValuesOfFrm( (*pPrevFrm), nPrevLowerSpace, nPrevLineSpacing );
            if( pDoc->IsParaSpaceMax() )
            {
                nUpper = nPrevLowerSpace + pAttrs->GetULSpace().GetUpper();
                SwTwips nAdd = nPrevLineSpacing;
                // OD 07.01.2004 #i11859# - consideration of the line spacing
                //      for the upper spacing of a text frame
                if ( bUseFormerLineSpacing )
                {
                    // former consideration
                    if ( pOwn->IsTxtFrm() )
                    {
                        nAdd = Max( nAdd, static_cast<SwTxtFrm&>(rThis).GetLineSpace() );
                    }
                    nUpper += nAdd;
                }
                else
                {
                    // new consideration:
                    //      Only the proportional line spacing of the previous
                    //      text frame is considered for the upper spacing and
                    //      the line spacing values are add up instead of
                    //      building its maximum.
                    if ( pOwn->IsTxtFrm() )
                    {
                        nAdd += static_cast<SwTxtFrm&>(rThis).GetLineSpace( true );
                    }
                    nUpper += nAdd;
                }
            }
            else
            {
                nUpper = Max( static_cast<long>(nPrevLowerSpace),
                              static_cast<long>(pAttrs->GetULSpace().GetUpper()) );
                // OD 07.01.2004 #i11859# - consideration of the line spacing
                //      for the upper spacing of a text frame
                if ( bUseFormerLineSpacing )
                {
                    // former consideration
                    if ( pOwn->IsTxtFrm() )
                        nUpper = Max( nUpper, ((SwTxtFrm*)pOwn)->GetLineSpace() );
                    if ( nPrevLineSpacing != 0 )
                    {
                        nUpper = Max( nUpper, nPrevLineSpacing );
                    }
                }
                else
                {
                    // new consideration:
                    //      Only the proportional line spacing of the previous
                    //      text frame is considered for the upper spacing and
                    //      the line spacing values are add up and added to
                    //      the paragraph spacing instead of building the
                    //      maximum of the line spacings and the paragraph spacing.
                    SwTwips nAdd = nPrevLineSpacing;
                    if ( pOwn->IsTxtFrm() )
                    {
                        nAdd += static_cast<SwTxtFrm&>(rThis).GetLineSpace( true );
                    }
                    nUpper += nAdd;
                }
            }
        }
        else if ( pDoc->IsParaSpaceMaxAtPages() &&
                  CastFlowFrm( pOwn )->HasParaSpaceAtPages( rThis.IsSctFrm() ) )
        {
            nUpper = pAttrs->GetULSpace().GetUpper();
        }
    }

    // OD 2004-02-26 #i25029# - pass previous frame <pPrevFrm>
    // to method <GetTopLine(..)>, if parameter <pPr> is set.
    // Note: parameter <pPr> is set, if method is called from <SwTxtFrm::WouldFit(..)>
    nUpper += pAttrs->GetTopLine( rThis, (pPr ? pPrevFrm : 0L) );

    // OD 2004-03-12 #i11860# - consider value of new parameter <_bConsiderGrid>
    // and use new method <GetUpperSpaceAmountConsideredForPageGrid(..)>
    if ( _bConsiderGrid )
    {
        nUpper += _GetUpperSpaceAmountConsideredForPageGrid( nUpper );
    }

    delete pAccess;
    return nUpper;
}

/** method to detemine the upper space amount, which is considered for
    the page grid

    OD 2004-03-12 #i11860#
    Precondition: Position of frame is valid.

    @author OD
*/
SwTwips SwFlowFrm::_GetUpperSpaceAmountConsideredForPageGrid(
                            const SwTwips _nUpperSpaceWithoutGrid ) const
{
    SwTwips nUpperSpaceAmountConsideredForPageGrid = 0;

    if ( rThis.IsInDocBody() && rThis.GetAttrSet()->GetParaGrid().GetValue() )
    {
        const SwPageFrm* pPageFrm = rThis.FindPageFrm();
        GETGRID( pPageFrm )
        if( pGrid )
        {
            const SwFrm* pBodyFrm = pPageFrm->FindBodyCont();
            if ( pBodyFrm )
            {
                const long nGridLineHeight =
                        pGrid->GetBaseHeight() + pGrid->GetRubyHeight();

                SWRECTFN( (&rThis) )
                const SwTwips nBodyPrtTop = (pBodyFrm->*fnRect->fnGetPrtTop)();
                const SwTwips nProposedPrtTop =
                        (*fnRect->fnYInc)( (rThis.Frm().*fnRect->fnGetTop)(),
                                           _nUpperSpaceWithoutGrid );

                const SwTwips nSpaceAbovePrtTop =
                        (*fnRect->fnYDiff)( nProposedPrtTop, nBodyPrtTop );
                const SwTwips nSpaceOfCompleteLinesAbove =
                        nGridLineHeight * ( nSpaceAbovePrtTop / nGridLineHeight );
                SwTwips nNewPrtTop =
                        (*fnRect->fnYInc)( nBodyPrtTop, nSpaceOfCompleteLinesAbove );
                if ( (*fnRect->fnYDiff)( nProposedPrtTop, nNewPrtTop ) > 0 )
                {
                    nNewPrtTop = (*fnRect->fnYInc)( nNewPrtTop, nGridLineHeight );
                }

                const SwTwips nNewUpperSpace =
                        (*fnRect->fnYDiff)( nNewPrtTop,
                                            (rThis.Frm().*fnRect->fnGetTop)() );

                nUpperSpaceAmountConsideredForPageGrid =
                        nNewUpperSpace - _nUpperSpaceWithoutGrid;

                ASSERT( nUpperSpaceAmountConsideredForPageGrid >= 0,
                        "<SwFlowFrm::GetUpperSpaceAmountConsideredForPageGrid(..)> - negative space considered for page grid!" );
            }
        }
    }
    return nUpperSpaceAmountConsideredForPageGrid;
}

/** method to determent the upper space amount, which is considered for
    the previous frame

    OD 2004-03-11 #i11860#

    @author OD
*/
SwTwips SwFlowFrm::_GetUpperSpaceAmountConsideredForPrevFrm() const
{
    SwTwips nUpperSpaceAmountOfPrevFrm = 0;

    const SwFrm* pPrevFrm = _GetPrevFrmForUpperSpaceCalc();
    if ( pPrevFrm )
    {
        SwTwips nPrevLowerSpace = 0;
        SwTwips nPrevLineSpacing = 0;
        GetSpacingValuesOfFrm( (*pPrevFrm), nPrevLowerSpace, nPrevLineSpacing );
        if ( nPrevLowerSpace > 0 || nPrevLineSpacing > 0 )
        {
            if ( rThis.GetAttrSet()->GetDoc()->IsParaSpaceMax() ||
                 !rThis.GetAttrSet()->GetDoc()->IsFormerLineSpacing() )
            {
                nUpperSpaceAmountOfPrevFrm = nPrevLowerSpace + nPrevLineSpacing;
            }
            else
            {
                nUpperSpaceAmountOfPrevFrm = Max( nPrevLowerSpace, nPrevLineSpacing );
            }
        }
    }

    return nUpperSpaceAmountOfPrevFrm;
}

/** method to determine the upper space amount, which is considered for
    the previous frame and the page grid, if option 'Use former object
    positioning' is OFF

    OD 2004-03-18 #i11860#

    @author OD
*/
SwTwips SwFlowFrm::GetUpperSpaceAmountConsideredForPrevFrmAndPageGrid() const
{
    SwTwips nUpperSpaceAmountConsideredForPrevFrmAndPageGrid = 0;

    if ( !rThis.GetAttrSet()->GetDoc()->IsFormerObjectPositioning() )
    {
        nUpperSpaceAmountConsideredForPrevFrmAndPageGrid =
            _GetUpperSpaceAmountConsideredForPrevFrm() +
            _GetUpperSpaceAmountConsideredForPageGrid( CalcUpperSpace( 0L, 0L, false ) );
    }

    return nUpperSpaceAmountConsideredForPrevFrmAndPageGrid;
}

/** calculation of lower space

    OD 2004-03-02 #106629#

    @author OD
*/
SwTwips SwFlowFrm::CalcLowerSpace( const SwBorderAttrs* _pAttrs ) const
{
    SwTwips nLowerSpace = 0;

    SwBorderAttrAccess* pAttrAccess = 0L;
    if ( !_pAttrs )
    {
        pAttrAccess = new SwBorderAttrAccess( SwFrm::GetCache(), &rThis );
        _pAttrs = pAttrAccess->Get();
    }

    sal_Bool bCommonBorder = sal_True;
    if ( rThis.IsInSct() && rThis.GetUpper()->IsColBodyFrm() )
    {
        const SwSectionFrm* pSectFrm = rThis.FindSctFrm();
        bCommonBorder = pSectFrm->GetFmt()->GetBalancedColumns().GetValue();
    }
    nLowerSpace = bCommonBorder ?
                  _pAttrs->GetBottomLine( rThis ) :
                  _pAttrs->CalcBottomLine();

    // --> OD 2004-07-16 #i26250#
    // - correct consideration of table frames
    // - use new method <CalcAddLowerSpaceAsLastInTableCell(..)>
    if ( ( ( rThis.IsTabFrm() && rThis.GetUpper()->IsInTab() ) ||
           // --> OD 2004-11-16 #115759# - no lower spacing, if frame has a follow
           ( rThis.IsInTab() && !GetFollow() ) ) &&
           // <--
         !rThis.GetIndNext() )
    {
        nLowerSpace += CalcAddLowerSpaceAsLastInTableCell( _pAttrs );
    }
    // <--

    delete pAttrAccess;

    return nLowerSpace;
}

/** calculation of the additional space to be considered, if flow frame
    is the last inside a table cell

    OD 2004-07-16 #i26250#

    @author OD
*/
SwTwips SwFlowFrm::CalcAddLowerSpaceAsLastInTableCell(
                                            const SwBorderAttrs* _pAttrs ) const
{
    SwTwips nAdditionalLowerSpace = 0;

    if ( rThis.GetShell()->GetDoc()->IsAddParaSpacingToTableCells() )
    {
        const SwFrm* pFrm = &rThis;
        if ( pFrm->IsSctFrm() )
        {
            const SwSectionFrm* pSectFrm = static_cast<const SwSectionFrm*>(pFrm);
            pFrm = pSectFrm->FindLastCntnt();
            if ( pFrm && pFrm->IsInTab() )
            {
                const SwTabFrm* pTableFrm = pFrm->FindTabFrm();
                if ( pSectFrm->IsAnLower( pTableFrm ) )
                {
                    pFrm = pTableFrm;
                }
            }
        }

        SwBorderAttrAccess* pAttrAccess = 0L;
        if ( !_pAttrs || pFrm != &rThis )
        {
            pAttrAccess = new SwBorderAttrAccess( SwFrm::GetCache(), pFrm );
            _pAttrs = pAttrAccess->Get();
        }

        nAdditionalLowerSpace += _pAttrs->GetULSpace().GetLower();

        delete pAttrAccess;
    }

    return nAdditionalLowerSpace;
}

/*************************************************************************
|*
|*	BOOL SwFlowFrm::CheckMoveFwd()
|*
|*	Beschreibung		Moved den Frm vorwaerts wenn es durch die aktuellen
|* 		Bedingungen und Attribute notwendig erscheint.
|*	Ersterstellung		MA 05. Dec. 96
|*	Letzte Aenderung	MA 09. Mar. 98
|*
|*************************************************************************/


BOOL SwFlowFrm::CheckMoveFwd( BOOL &rbMakePage, BOOL bKeep, BOOL bMovedBwd )
{
    const SwFrm* pNxt = rThis.GetIndNext();

    if ( bKeep && //!bMovedBwd &&
         ( !pNxt || ( pNxt->IsTxtFrm() && ((SwTxtFrm*)pNxt)->IsEmptyMaster() ) ) &&
         ( 0 != (pNxt = rThis.FindNext()) ) && IsKeepFwdMoveAllowed() )
    {
        if( pNxt->IsSctFrm() )
        {   // Nicht auf leere SectionFrms hereinfallen
            const SwFrm* pTmp = NULL;
            while( pNxt && pNxt->IsSctFrm() &&
                   ( !((SwSectionFrm*)pNxt)->GetSection() ||
                     !( pTmp = ((SwSectionFrm*)pNxt)->ContainsAny() ) ) )
            {
                pNxt = pNxt->FindNext();
                pTmp = NULL;
            }
            if( pTmp )
                pNxt = pTmp; // the content of the next notempty sectionfrm
        }
        if( pNxt && pNxt->GetValidPosFlag() )
        {
            BOOL bMove = FALSE;
            const SwSectionFrm *pSct = rThis.FindSctFrm();
            if( pSct && !pSct->GetValidSizeFlag() )
            {
                const SwSectionFrm* pNxtSct = pNxt->FindSctFrm();
                if( pNxtSct && pSct->IsAnFollow( pNxtSct ) )
                    bMove = TRUE;
            }
            else
                bMove = TRUE;
            if( bMove )
            {
                //Keep together with the following frame
                MoveFwd( rbMakePage, FALSE );
                return TRUE;
            }
        }
    }

    BOOL bMovedFwd = FALSE;

    if ( rThis.GetIndPrev() )
    {
        if ( IsPrevObjMove() ) //Auf Objekte des Prev Ruecksicht nehmen?
        {
            bMovedFwd = TRUE;
            if ( !MoveFwd( rbMakePage, FALSE ) )
                rbMakePage = FALSE;
        }
        else
        {
            if ( IsPageBreak( FALSE ) )
            {
                while ( MoveFwd( rbMakePage, TRUE ) )
                        /* do nothing */;
                rbMakePage = FALSE;
                bMovedFwd = TRUE;
            }
            else if ( IsColBreak ( FALSE ) )
            {
                const SwPageFrm *pPage = rThis.FindPageFrm();
                SwFrm *pCol = rThis.FindColFrm();
                do
                {	MoveFwd( rbMakePage, FALSE );
                    SwFrm *pTmp = rThis.FindColFrm();
                    if( pTmp != pCol )
                    {
                        bMovedFwd = TRUE;
                        pCol = pTmp;
                    }
                    else
                        break;
                } while ( IsColBreak( FALSE ) );
                if ( pPage != rThis.FindPageFrm() )
                    rbMakePage = FALSE;
            }
        }
    }
    return bMovedFwd;
}

/*************************************************************************
|*
|*	BOOL SwFlowFrm::MoveFwd()
|*
|*	Beschreibung		Returnwert sagt, ob der Frm die Seite gewechselt hat.
|*	Ersterstellung		MA 05. Dec. 96
|*	Letzte Aenderung	MA 05. Dec. 96
|*
|*************************************************************************/


BOOL SwFlowFrm::MoveFwd( BOOL bMakePage, BOOL bPageBreak, BOOL bMoveAlways )
{
//!!!!MoveFtnCntFwd muss ggf. mitgepflegt werden.
    SwFtnBossFrm *pOldBoss = rThis.FindFtnBossFrm();
    if ( rThis.IsInFtn() )
        return ((SwCntntFrm&)rThis).MoveFtnCntFwd( bMakePage, pOldBoss );

    if( !IsFwdMoveAllowed() && !bMoveAlways )
    {
        BOOL bNoFwd = TRUE;
        if( rThis.IsInSct() )
        {
            SwFtnBossFrm* pBoss = rThis.FindFtnBossFrm();
            bNoFwd = !pBoss->IsInSct() || ( !pBoss->Lower()->GetNext() &&
                     !pBoss->GetPrev() );
        }
        // Allow the MoveFwd even if we do not have an IndPrev in these cases:
        if ( rThis.IsInTab() &&
            ( !rThis.IsTabFrm() ||
                ( rThis.GetUpper()->IsInTab() &&
                  rThis.GetUpper()->FindTabFrm()->IsFwdMoveAllowed() ) ) &&
                ( NULL != rThis.IsInSplitTableRow() ) )
        {
            bNoFwd = FALSE;
        }
        if( bNoFwd )
        {
            //Fuer PageBreak ist das Moven erlaubt, wenn der Frm nicht
            //bereits der erste der Seite ist.
            if ( !bPageBreak )
                return FALSE;

            const SwFrm *pCol = rThis.FindColFrm();
            if ( !pCol || !pCol->GetPrev() )
                return FALSE;
        }
    }

    BOOL bSamePage = TRUE;
    SwLayoutFrm *pNewUpper =
            rThis.GetLeaf( bMakePage ? MAKEPAGE_INSERT : MAKEPAGE_NONE, TRUE );

    if ( pNewUpper )
    {
        PROTOCOL_ENTER( &rThis, PROT_MOVE_FWD, 0, 0 );
        SwPageFrm *pOldPage = pOldBoss->FindPageFrm();
        //Wir moven uns und alle direkten Nachfolger vor den ersten
        //CntntFrm unterhalb des neuen Uppers.

        // Wenn unser NewUpper in einem SectionFrm liegt, muessen wir
        // verhindern, dass sich dieser im Calc selbst zerstoert
        SwSectionFrm* pSect = pNewUpper->FindSctFrm();
        BOOL bUnlock = FALSE;
        if( pSect )
        {
            // Wenn wir nur innerhalb unseres SectionFrms die Spalte wechseln,
            // rufen wir lieber kein Calc, sonst wird noch der SectionFrm
            // formatiert, der wiederum uns ruft etc.
            if( pSect != rThis.FindSctFrm() )
            {
                bUnlock = !pSect->IsColLocked();
                pSect->ColLock();
                pNewUpper->Calc();
                if( bUnlock )
                    pSect->ColUnlock();
            }
        }
        // Do not calculate split cell frames.
        else if ( !pNewUpper->IsCellFrm() || ((SwLayoutFrm*)pNewUpper)->Lower() )
            pNewUpper->Calc();

        SwFtnBossFrm *pNewBoss = pNewUpper->FindFtnBossFrm();
        BOOL bBossChg = pNewBoss != pOldBoss;
        pNewBoss = pNewBoss->FindFtnBossFrm( TRUE );
        pOldBoss = pOldBoss->FindFtnBossFrm( TRUE );
        SwPageFrm* pNewPage = pOldPage;

        // First, we move the footnotes.
        BOOL bFtnMoved = FALSE;

        // --> FME 2004-04-19 #i27145#
        const bool bOldUpperValid = rThis.GetUpper()->IsValid();
        // <--

        // --> FME 2004-07-15 #i26831#
        // If pSect has just been created, the printing area of pSect has
        // been calculated based on the first content of its follow.
        // In this case we prefer to call a SimpleFormat for this new
        // section after we inserted the contents. Otherwise the section
        // frame will invalidate its lowers, if its printing area changes
        // in SwSectionFrm::Format, which can cause loops.
        const bool bForceSimpleFormat = pSect && pSect->HasFollow() &&
                                       !pSect->ContainsAny();
        // <--

        if ( pNewBoss != pOldBoss )
        {
            pNewPage = pNewBoss->FindPageFrm();
            bSamePage = pNewPage == pOldPage;
            //Damit die Fussnoten nicht auf dumme Gedanken kommen
            //setzen wir hier die Deadline.
            SWRECTFN( pOldBoss )
            SwSaveFtnHeight aHeight( pOldBoss,
                (pOldBoss->Frm().*fnRect->fnGetBottom)() );
            SwCntntFrm* pStart = rThis.IsCntntFrm() ?
                (SwCntntFrm*)&rThis : ((SwLayoutFrm&)rThis).ContainsCntnt();
            ASSERT( pStart || ( rThis.IsTabFrm() && !((SwTabFrm&)rThis).Lower() ),
                    "MoveFwd: Missing Content" );
            SwLayoutFrm* pBody = pStart ? ( pStart->IsTxtFrm() ?
                (SwLayoutFrm*)((SwTxtFrm*)pStart)->FindBodyFrm() : 0 ) : 0;
            if( pBody )
                bFtnMoved = pBody->MoveLowerFtns( pStart, pOldBoss, pNewBoss,
                                                  FALSE);
        }
        // Bei SectionFrms ist es moeglich, dass wir selbst durch pNewUpper->Calc()
        // bewegt wurden, z. B. in den pNewUpper.
        // MoveSubTree bzw. PasteTree ist auf so etwas nicht vorbereitet.
        if( pNewUpper != rThis.GetUpper() )
        {
            // --> FME 2004-04-19 #i27145#
            SwSectionFrm* pOldSct = 0;
            if ( rThis.GetUpper()->IsSctFrm() )
            {
                pOldSct = static_cast<SwSectionFrm*>(rThis.GetUpper());
            }
            // <--

            MoveSubTree( pNewUpper, pNewUpper->Lower() );

            // --> FME 2004-04-19 #i27145#
            if ( pOldSct && pOldSct->GetSection() )
            {
                // Prevent loops by setting the new height at
                // the section frame if footnotes have been moved.
                // Otherwise the call of SwLayNotify::~SwLayNotify() for
                // the (invalid) section frame will invalidate the first
                // lower of its follow, because it grows due to the removed
                // footnotes.
                // Note: If pOldSct has become empty during MoveSubTree, it
                // has already been scheduled for removal. No SimpleFormat
                // for these.
                pOldSct->SimpleFormat();
            }
            // <--

            // --> FME 2004-07-15 #i26831#
            if ( bForceSimpleFormat )
            {
                pSect->SimpleFormat();
            }
            // <--

            if ( bFtnMoved && !bSamePage )
            {
                pOldPage->UpdateFtnNum();
                pNewPage->UpdateFtnNum();
            }

            if( bBossChg )
            {
                rThis.Prepare( PREP_BOSS_CHGD, 0, FALSE );
                if( !bSamePage )
                {
                    ViewShell *pSh = rThis.GetShell();
                    if ( pSh && !pSh->Imp()->IsUpdateExpFlds() )
                        pSh->GetDoc()->SetNewFldLst();	//Wird von CalcLayout() hinterher erledigt!
                    pNewPage->InvalidateSpelling();
                    pNewPage->InvalidateAutoCompleteWords();
                }
            }
        }
        // OD 30.10.2002 #97265# - no <CheckPageDesc(..)> in online layout
        if ( !pNewPage->GetFmt()->GetDoc()->IsBrowseMode() )
        {
            //Bei Sections kann es passieren, das wir gleich  in den Follow geflutscht
            //sind. Dadurch wird nicht vom GetLeaf fuer die richtige Seite gesorgt.
            //Das muessen wir fuer diesen Fall pruefen.
            if ( !bSamePage && pNewUpper->IsInSct() &&
                 ( rThis.GetAttrSet()->GetPageDesc().GetPageDesc() ||
                   pOldPage->GetPageDesc()->GetFollow() != pNewPage->GetPageDesc() ) )
                SwFrm::CheckPageDescs( pNewPage, FALSE );
        }
    }
    return bSamePage;
}


/*************************************************************************
|*
|*	BOOL SwFlowFrm::MoveBwd()
|*
|*	Beschreibung		Returnwert sagt, ob der Frm die Seite wechseln soll.
|*						Sollte von abgeleiteten Klassen gerufen werden.
|* 						Das moven selbst muessen die abgeleiteten uebernehmen.
|*	Ersterstellung		MA 05. Dec. 96
|*	Letzte Aenderung	MA 05. Dec. 96
|*
|*************************************************************************/

BOOL SwFlowFrm::MoveBwd( BOOL &rbReformat )
{
    SwFlowFrm::SetMoveBwdJump( FALSE );

    SwFtnFrm* pFtn = rThis.FindFtnFrm();
    if ( pFtn && pFtn->IsBackMoveLocked() )
        return FALSE;

    // --> OD 2004-11-29 #115759# - text frames, which are directly inside
    // tables aren't allowed to move backward.
    if ( rThis.IsTxtFrm() && rThis.IsInTab() )
    {
        const SwLayoutFrm* pUpperFrm = rThis.GetUpper();
        while ( pUpperFrm )
        {
            if ( pUpperFrm->IsTabFrm() )
            {
                return FALSE;
            }
            else if ( pUpperFrm->IsColumnFrm() && pUpperFrm->IsInSct() )
            {
                break;
            }
            pUpperFrm = pUpperFrm->GetUpper();
        }
    }
    // <--

    SwFtnBossFrm * pOldBoss = rThis.FindFtnBossFrm();
    SwPageFrm * const pOldPage = pOldBoss->FindPageFrm();
    SwLayoutFrm *pNewUpper = 0;
    FASTBOOL bCheckPageDescs = FALSE;
    bool bCheckPageDescOfNextPage = false;

    if ( pFtn )
    {
        //Wenn die Fussnote bereits auf der gleichen Seite/Spalte wie die Referenz
        //steht, ist nix mit zurueckfliessen. Die breaks brauche fuer die
        //Fussnoten nicht geprueft zu werden.

        // --> FME 2004-11-15 #i37084# FindLastCntnt does not necessarily
        // have to have a result != 0
        SwFrm* pRef = 0;
        const BOOL bEndnote = pFtn->GetAttr()->GetFtn().IsEndNote();
        if( bEndnote && pFtn->IsInSct() )
        {
            SwSectionFrm* pSect = pFtn->FindSctFrm();
            if( pSect->IsEndnAtEnd() )
                pRef = pSect->FindLastCntnt( FINDMODE_LASTCNT );
        }
        if( !pRef )
            pRef = pFtn->GetRef();
        // <--

        ASSERT( pRef, "MoveBwd: Endnote for an empty section?" );

        if( !bEndnote )
            pOldBoss = pOldBoss->FindFtnBossFrm( TRUE );
        SwFtnBossFrm *pRefBoss = pRef->FindFtnBossFrm( !bEndnote );
        if ( pOldBoss != pRefBoss &&
             // OD 08.11.2002 #104840# - use <SwLayoutFrm::IsBefore(..)>
             ( !bEndnote ||
               pRefBoss->IsBefore( pOldBoss ) )
           )
            pNewUpper = rThis.GetLeaf( MAKEPAGE_FTN, FALSE );
    }
    else if ( IsPageBreak( TRUE ) )	//PageBreak zu beachten?
    {
        //Wenn auf der vorhergehenden Seite kein Frm im Body steht,
        //so ist das Zurueckfliessen trotz Pagebreak sinnvoll
        //(sonst: leere Seite).
        //Natuerlich muessen Leereseiten geflissentlich uebersehen werden!
        const SwFrm *pFlow = &rThis;
        do
        {	pFlow = pFlow->FindPrev();
        } while ( pFlow && ( pFlow->FindPageFrm() == pOldPage ||
                  !pFlow->IsInDocBody() ) );
        if ( pFlow )
        {
            long nDiff = pOldPage->GetPhyPageNum() - pFlow->GetPhyPageNum();
            if ( nDiff > 1 )
            {
                if ( ((SwPageFrm*)pOldPage->GetPrev())->IsEmptyPage() )
                    nDiff -= 1;
                if ( nDiff > 1 )
                {
                    pNewUpper = rThis.GetLeaf( MAKEPAGE_NONE, FALSE );

                    //
                    // START OF HACK for #i14206#
                    //

                    // Get the bodyframe of the next page.
                    // There was a loop in this situation:
                    // Page 5: Section frame
                    // Page 6: Empty body frame
                    // Page 7: Tab frame with page break before.
                    // Here, the tab frame moves to page 5. Therefore the
                    // section frame on page 5 is invalidated. During further
                    // formatting of the tab frame, it is moved to page 6
                    // because of the page break. During formatting of
                    // the section frame, the tab frame moves to page 7 again and so on.

                    if ( pFlow->IsInSct() && SwFlowFrm::IsMoveBwdJump() && 2 == nDiff &&
                         !((SwPageFrm*)pOldPage->GetPrev())->IsEmptyPage() &&
                         pNewUpper && pNewUpper->IsPageBodyFrm() )
                    {
                        SwPageFrm* pNextPage = (SwPageFrm*)pNewUpper->GetUpper()->GetNext();
                        if ( pNextPage )
                        {
                            SwFrm* pLayout = pNextPage->Lower();
                            if ( pLayout && pLayout->IsHeaderFrm() )
                                pLayout = pLayout->GetNext();

                            if ( pLayout && pLayout->IsBodyFrm() && !((SwLayoutFrm*)pLayout)->Lower() )
                            {
                                pNewUpper = (SwLayoutFrm*)pLayout;
                                SwFlowFrm::SetMoveBwdJump( FALSE );
                                bCheckPageDescOfNextPage = true;
                            }
                        }
                    }

                    //
                    // END OF HACK for #i14206#
                    //

                    bCheckPageDescs = TRUE;
                }
            }
        }
    }
    else if ( IsColBreak( TRUE ) )
    {
        //Wenn in der vorhergehenden Spalte kein CntntFrm steht, so ist
        //das Zurueckfliessen trotz ColumnBreak sinnvoll
        //(sonst: leere Spalte).
        if( rThis.IsInSct() )
        {
            pNewUpper = rThis.GetLeaf( MAKEPAGE_NONE, FALSE );
            if( pNewUpper && !SwFlowFrm::IsMoveBwdJump() &&
                ( pNewUpper->ContainsCntnt() ||
                  ( ( !pNewUpper->IsColBodyFrm() ||
                      !pNewUpper->GetUpper()->GetPrev() ) &&
                    !pNewUpper->FindSctFrm()->GetPrev() ) ) )
                pNewUpper = 0;
        }
        else
        {
            const SwFrm *pCol = rThis.FindColFrm();
            BOOL bGoOn = TRUE;
            BOOL bJump = FALSE;
            do
            {
                if ( pCol->GetPrev() )
                    pCol = pCol->GetPrev();
                else
                {
                    bGoOn = FALSE;
                    pCol = rThis.GetLeaf( MAKEPAGE_NONE, FALSE );
                }
                if ( pCol )
                {
                    // ColumnFrms jetzt mit BodyFrm
                    SwLayoutFrm* pColBody = pCol->IsColumnFrm() ?
                        (SwLayoutFrm*)((SwLayoutFrm*)pCol)->Lower() :
                        (SwLayoutFrm*)pCol;
                    if ( pColBody->ContainsCntnt() )
                    {
                        bGoOn = FALSE; // Hier gibt's Inhalt, wir akzeptieren diese
                        // nur, wenn GetLeaf() das MoveBwdJump-Flag gesetzt hat.
                        if( SwFlowFrm::IsMoveBwdJump() )
                            pNewUpper = pColBody;
                    }
                    else
                    {
                        if( pNewUpper ) // Wir hatten schon eine leere Spalte, haben
                            bJump = TRUE;   // also eine uebersprungen
                        pNewUpper = pColBody;  // Diese leere Spalte kommt in Frage,
                                               // trotzdem weitersuchen
                    }
                }
            } while( bGoOn );
            if( bJump )
                SwFlowFrm::SetMoveBwdJump( TRUE );
        }
    }
    else //Keine Breaks also kann ich zurueckfliessen
        pNewUpper = rThis.GetLeaf( MAKEPAGE_NONE, FALSE );

    // --> OD 2004-06-23 #i27801# - no move backward of 'master' text frame,
    // if - due to its object positioning - it isn't allowed to be on the new page frame
    // --> OD 2005-03-07 #i44049# - add another condition for not moving backward:
    // If one of its objects has restarted the layout process, moving backward
    // isn't sensible either.
    // --> OD 2005-04-19 #i47697# - refine condition made for issue i44049:
    // - allow move backward as long as the anchored object is only temporarily
    //   positions considering its wrapping style.
    if ( pNewUpper &&
         rThis.IsTxtFrm() && !IsFollow() )
    {
        sal_uInt32 nToPageNum( 0L );
        const bool bMoveFwdByObjPos = SwLayouter::FrmMovedFwdByObjPos(
                                                *(pOldPage->GetFmt()->GetDoc()),
                                                static_cast<SwTxtFrm&>(rThis),
                                                nToPageNum );
        if ( bMoveFwdByObjPos &&
             pNewUpper->FindPageFrm()->GetPhyPageNum() < nToPageNum )
        {
            pNewUpper = 0;
        }
        // --> OD 2005-03-07 #i44049# - check, if one of its anchored objects
        // has restarted the layout process.
        else if ( rThis.GetDrawObjs() )
        {
            sal_uInt32 i = 0;
            for ( ; i < rThis.GetDrawObjs()->Count(); ++i )
            {
                SwAnchoredObject* pAnchoredObj = (*rThis.GetDrawObjs())[i];
                // --> OD 2005-04-19 #i47697# - refine condition - see above
                if ( pAnchoredObj->RestartLayoutProcess() &&
                     !pAnchoredObj->IsTmpConsiderWrapInfluence() )
                // <--
                {
                    pNewUpper = 0;
                    break;
                }
            }
        }
        // <--
    }
    // <--

    //Fuer Follows ist das zurueckfliessen nur dann erlaubt wenn in der
    //neuen Umgebung kein Nachbar existiert (denn dieses waere der Master).
    //(6677)Wenn allerdings leere Blaetter uebersprungen wurden wird doch gemoved.
    if ( pNewUpper && IsFollow() && pNewUpper->Lower() )
    {
        if ( SwFlowFrm::IsMoveBwdJump() )
        {
            //Nicht hinter den Master sondern in das naechstfolgende leere
            //Blatt moven.
            SwFrm *pFrm = pNewUpper->Lower();
            while ( pFrm->GetNext() )
                pFrm = pFrm->GetNext();
            pNewUpper = pFrm->GetLeaf( MAKEPAGE_INSERT, TRUE );
            if( pNewUpper == rThis.GetUpper() )	//Landen wir wieder an der gleichen Stelle?
                pNewUpper = NULL;			//dann eruebrigt sich das Moven
        }
        else
            pNewUpper = 0;
    }
    if ( pNewUpper && !ShouldBwdMoved( pNewUpper, TRUE, rbReformat ) )
    {
        if( !pNewUpper->Lower() )
        {
            if( pNewUpper->IsFtnContFrm() )
            {
                pNewUpper->Cut();
                delete pNewUpper;
            }
            else
            {
                SwSectionFrm* pSectFrm = pNewUpper->FindSctFrm();
                if( pSectFrm && !pSectFrm->IsColLocked() && !pSectFrm->ContainsCntnt() )
                {
                    pSectFrm->DelEmpty( TRUE );
                    delete pSectFrm;
                    rThis.bValidPos = TRUE;
                }
            }
        }
        pNewUpper = 0;
    }

    // OD 2004-05-26 #i21478# - don't move backward, if flow frame wants to
    // keep with next frame and next frame is locked.
    // --> OD 2004-12-08 #i38232# - If next frame is a table, do *not* check,
    // if it's locked.
    if ( pNewUpper && !IsFollow() &&
         rThis.GetAttrSet()->GetKeep().GetValue() && rThis.GetIndNext() )
    {
        SwFrm* pIndNext = rThis.GetIndNext();
        // --> OD 2004-12-08 #i38232#
        if ( !pIndNext->IsTabFrm() )
        {
            // get first content of section, while empty sections are skipped
            while ( pIndNext && pIndNext->IsSctFrm() )
            {
                if( static_cast<SwSectionFrm*>(pIndNext)->GetSection() )
                {
                    SwFrm* pTmp = static_cast<SwSectionFrm*>(pIndNext)->ContainsAny();
                    if ( pTmp )
                    {
                        pIndNext = pTmp;
                        break;
                    }
                }
                pIndNext = pIndNext->GetIndNext();
            }
            ASSERT( !pIndNext || pIndNext->ISA(SwTxtFrm),
                    "<SwFlowFrm::MovedBwd(..)> - incorrect next found." );
            if ( pIndNext && pIndNext->IsFlowFrm() &&
                 SwFlowFrm::CastFlowFrm(pIndNext)->IsJoinLocked() )
            {
                pNewUpper = 0L;
            }
        }
        // <--
    }

    if ( pNewUpper )
    {
        PROTOCOL_ENTER( &rThis, PROT_MOVE_BWD, 0, 0 );
        if ( pNewUpper->IsFtnContFrm() )
        {
            //Kann sein, dass ich einen Container bekam.
            SwFtnFrm *pOld = rThis.FindFtnFrm();
            SwFtnFrm *pNew = new SwFtnFrm( pOld->GetFmt(),
                                           pOld->GetRef(), pOld->GetAttr() );
            if ( pOld->GetMaster() )
            {
                pNew->SetMaster( pOld->GetMaster() );
                pOld->GetMaster()->SetFollow( pNew );
            }
            pNew->SetFollow( pOld );
            pOld->SetMaster( pNew );
            pNew->Paste( pNewUpper );
            pNewUpper = pNew;
        }
        if( pNewUpper->IsFtnFrm() && rThis.IsInSct() )
        {
            SwSectionFrm* pSct = rThis.FindSctFrm();
            //Wenn wir in einem Bereich in einer Fussnote stecken, muss im
            //neuen Upper ggf. ein SwSectionFrm angelegt werden
            if( pSct->IsInFtn() )
            {
                SwFrm* pTmp = pNewUpper->Lower();
                if( pTmp )
                {
                    while( pTmp->GetNext() )
                        pTmp = pTmp->GetNext();
                    if( !pTmp->IsSctFrm() ||
                        ((SwSectionFrm*)pTmp)->GetFollow() != pSct )
                        pTmp = NULL;
                }
                if( pTmp )
                    pNewUpper = (SwSectionFrm*)pTmp;
                else
                {
                    pSct = new SwSectionFrm( *pSct, TRUE );
                    pSct->Paste( pNewUpper );
                    pSct->Init();
                    pNewUpper = pSct;
                    pSct->SimpleFormat();
                }
            }
        }
        BOOL bUnlock = FALSE;
        BOOL bFollow;
        //Section locken, sonst kann sie bei Fluss des einzigen Cntnt etwa
        //von zweiter in die erste Spalte zerstoert werden.
        SwSectionFrm* pSect = pNewUpper->FindSctFrm();
        if( pSect )
        {
            bUnlock = !pSect->IsColLocked();
            pSect->ColLock();
            bFollow = pSect->HasFollow();
        }
        pNewUpper->Calc();
        rThis.Cut();
        // --> OD 2005-02-23 #b6229852#
        // optimization: format section, if its size is invalidated and if it's
        // the new parent of moved backward frame.
        bool bFormatSect( false );
        // <--
        if( bUnlock )
        {
            pSect->ColUnlock();
            if( pSect->HasFollow() != bFollow )
            {
                pSect->InvalidateSize();
                // --> OD 2005-02-23 #b6229852# - optimization
                if ( pSect == pNewUpper )
                    bFormatSect = true;
                // <--
            }
        }

        rThis.Paste( pNewUpper );
        // --> OD 2005-02-23 #b6229852# - optimization
        if ( bFormatSect )
            pSect->Calc();
        // <--

        SwPageFrm *pNewPage = rThis.FindPageFrm();
        if( pNewPage != pOldPage )
        {
            rThis.Prepare( PREP_BOSS_CHGD, (const void*)pOldPage, FALSE );
            ViewShell *pSh = rThis.GetShell();
            if ( pSh && !pSh->Imp()->IsUpdateExpFlds() )
                pSh->GetDoc()->SetNewFldLst();	//Wird von CalcLayout() hinterher eledigt!
            pNewPage->InvalidateSpelling();
            pNewPage->InvalidateAutoCompleteWords();
            // OD 30.10.2002 #97265# - no <CheckPageDesc(..)> in online layout
            if ( !pNewPage->GetFmt()->GetDoc()->IsBrowseMode() )
            {
                if ( bCheckPageDescs && pNewPage->GetNext() )
                {
                    SwPageFrm* pStartPage = bCheckPageDescOfNextPage ?
                                            pNewPage :
                                            (SwPageFrm*)pNewPage->GetNext();
                    SwFrm::CheckPageDescs( pStartPage, FALSE);
                }
                else if ( rThis.GetAttrSet()->GetPageDesc().GetPageDesc() )
                {
                    //Erste Seite wird etwa durch Ausblenden eines Bereiches leer
                    SwFrm::CheckPageDescs( (SwPageFrm*)pNewPage, FALSE);
                }
            }
        }
    }
    return pNewUpper != 0;
}

/*************************************************************************
|*
|*	SwFlowFrm::CastFlowFrm
|*
|*	Ersterstellung		MA 03. May. 95
|*	Letzte Aenderung	AMA 02. Dec. 97
|*
|*************************************************************************/

SwFlowFrm *SwFlowFrm::CastFlowFrm( SwFrm *pFrm )
{
    if ( pFrm->IsCntntFrm() )
        return (SwCntntFrm*)pFrm;
    if ( pFrm->IsTabFrm() )
        return (SwTabFrm*)pFrm;
    if ( pFrm->IsSctFrm() )
        return (SwSectionFrm*)pFrm;
    return 0;
}

const SwFlowFrm *SwFlowFrm::CastFlowFrm( const SwFrm *pFrm )
{
    if ( pFrm->IsCntntFrm() )
        return (SwCntntFrm*)pFrm;
    if ( pFrm->IsTabFrm() )
        return (SwTabFrm*)pFrm;
    if ( pFrm->IsSctFrm() )
        return (SwSectionFrm*)pFrm;
    return 0;
}





