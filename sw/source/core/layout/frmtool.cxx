/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: frmtool.cxx,v $
 * $Revision: 1.105 $
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
#include <tools/bigint.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdpage.hxx>
#include <svx/brshitem.hxx>
#include <svx/keepitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/boxitem.hxx>
#include <sfx2/printer.hxx>
// OD 08.01.2004 #i11859#
#include <svx/lspcitem.hxx>


#include <fmtornt.hxx>
#include <fmtanchr.hxx>
#include <fmthdft.hxx>
#include <fmtcntnt.hxx>
#include <fmtfsize.hxx>
#include <fmtsrnd.hxx>
#include <docary.hxx>
#include <lineinfo.hxx>
#include <swmodule.hxx>
#include "pagefrm.hxx"
#include "colfrm.hxx"
#include "doc.hxx"
#include "fesh.hxx"
#include "viewimp.hxx"
#include "pam.hxx"
#include "dflyobj.hxx"
#include "dcontact.hxx"
#include "frmtool.hxx"
#include "docsh.hxx"
#include "tabfrm.hxx"
#include "rowfrm.hxx"
#include "ftnfrm.hxx"
#include "txtfrm.hxx"
#include "notxtfrm.hxx"
#include "flyfrms.hxx"
#include "layact.hxx"
#include "pagedesc.hxx"
#include "section.hxx"
#include "sectfrm.hxx"
#include "node2lay.hxx"
#include "ndole.hxx"
#include "ndtxt.hxx"
#include "swtable.hxx"
#include "hints.hxx"
#include <layhelp.hxx>
#include <laycache.hxx>

#include <rootfrm.hxx>

#include "mdiexp.hxx"
#include "statstr.hrc"
// OD 21.05.2003 #108789#
#include <paratr.hxx>
// OD 2004-05-24 #i28701#
#include <sortedobjs.hxx>
// --> OD 2005-03-04 #b6234250#
#include <objectformatter.hxx>
// <--

// ftnfrm.cxx:
void lcl_RemoveFtns( SwFtnBossFrm* pBoss, BOOL bPageOnly, BOOL bEndNotes );

using namespace ::com::sun::star;


BOOL bObjsDirect = TRUE;
BOOL bDontCreateObjects = FALSE;
BOOL bSetCompletePaintOnInvalidate = FALSE;

BYTE StackHack::nCnt = 0;
BOOL StackHack::bLocked = FALSE;



/*************************************************************************
|*
|*	SwFrmNotify::SwFrmNotify()
|*
|*	Ersterstellung		MA 27. Nov. 92
|*	Letzte Aenderung	MA 09. Apr. 97
|*
|*************************************************************************/

SwFrmNotify::SwFrmNotify( SwFrm *pF ) :
    pFrm( pF ),
    aFrm( pF->Frm() ),
    aPrt( pF->Prt() ),
    bInvaKeep( FALSE ),
    bValidSize( pF->GetValidSizeFlag() ),
    // --> OD 2005-07-29 #i49383#
    mbFrmDeleted( false )
    // <--
{
    if ( pF->IsTxtFrm() )
    {
        mnFlyAnchorOfst = ((SwTxtFrm*)pF)->GetBaseOfstForFly( sal_True );
        mnFlyAnchorOfstNoWrap = ((SwTxtFrm*)pF)->GetBaseOfstForFly( sal_False );
    }
    else
    {
        mnFlyAnchorOfst = 0;
        mnFlyAnchorOfstNoWrap = 0;
    }

    bHadFollow = pF->IsCntntFrm() ?
                    (((SwCntntFrm*)pF)->GetFollow() ? TRUE : FALSE) :
                    FALSE;
}

/*************************************************************************
|*
|*	SwFrmNotify::~SwFrmNotify()
|*
|*	Ersterstellung		MA 27. Nov. 92
|*	Letzte Aenderung	MA 09. Apr. 97
|*
|*************************************************************************/

SwFrmNotify::~SwFrmNotify()
{
    // --> OD 2005-07-29 #i49383#
    if ( mbFrmDeleted )
    {
        return;
    }
    // <--

    SWRECTFN( pFrm )
    const BOOL bAbsP = POS_DIFF( aFrm, pFrm->Frm() );
    const BOOL bChgWidth =
            (aFrm.*fnRect->fnGetWidth)() != (pFrm->Frm().*fnRect->fnGetWidth)();
    const BOOL bChgHeight =
            (aFrm.*fnRect->fnGetHeight)()!=(pFrm->Frm().*fnRect->fnGetHeight)();
    const BOOL bChgFlyBasePos = pFrm->IsTxtFrm() &&
       ( ( mnFlyAnchorOfst != ((SwTxtFrm*)pFrm)->GetBaseOfstForFly( sal_True ) ) ||
         ( mnFlyAnchorOfstNoWrap != ((SwTxtFrm*)pFrm)->GetBaseOfstForFly( sal_False ) ) );

    if ( pFrm->IsFlowFrm() && !pFrm->IsInFtn() )
    {
        SwFlowFrm *pFlow = SwFlowFrm::CastFlowFrm( pFrm );

        if ( !pFlow->IsFollow() )
        {
            if ( !pFrm->GetIndPrev() )
            {
                if ( bInvaKeep )
                {
                    SwFrm *pPre = pFrm->FindPrev();
                    if ( pPre && pPre->IsFlowFrm() )
                    {
                        // 1. pPre wants to keep with me:
                        bool bInvalidPrePos = SwFlowFrm::CastFlowFrm( pPre )->IsKeep( *pPre->GetAttrSet() ) && pPre->GetIndPrev();

                        // 2. pPre is a table and the last row wants to keep with me:
                        if ( !bInvalidPrePos && pPre->IsTabFrm() )
                        {
                            SwTabFrm* pPreTab = static_cast<SwTabFrm*>(pPre);
                            if ( pPreTab->GetFmt()->GetDoc()->get(IDocumentSettingAccess::TABLE_ROW_KEEP) )
                            {
                                SwRowFrm* pLastRow = static_cast<SwRowFrm*>(pPreTab->GetLastLower());
                                if ( pLastRow && pLastRow->ShouldRowKeepWithNext() )
                                    bInvalidPrePos = true;
                            }
                        }

                        if ( bInvalidPrePos )
                            pPre->InvalidatePos();
                    }
                }
            }
            else if ( !pFlow->HasFollow() )
            {
                long nOldHeight = (aFrm.*fnRect->fnGetHeight)();
                long nNewHeight = (pFrm->Frm().*fnRect->fnGetHeight)();
                if( (nOldHeight > nNewHeight) || (!nOldHeight && nNewHeight) )
                    pFlow->CheckKeep();
            }
        }
    }

    if ( bAbsP )
    {
        pFrm->SetCompletePaint();

        SwFrm* pNxt = pFrm->GetIndNext();
        // --> OD 2005-05-20 #121888# - skip empty section frames
        while ( pNxt &&
                pNxt->IsSctFrm() && !static_cast<SwSectionFrm*>(pNxt)->GetSection() )
        {
            pNxt = pNxt->GetIndNext();
        }
        // <--

        if ( pNxt )
            pNxt->InvalidatePos();
        else
        {
            // OD 04.11.2002 #104100# - correct condition for setting retouche
            // flag for vertical layout.
            if( pFrm->IsRetoucheFrm() &&
                (aFrm.*fnRect->fnTopDist)( (pFrm->Frm().*fnRect->fnGetTop)() ) > 0 )
            {
                pFrm->SetRetouche();
            }

            // A fresh follow frame does not have to be invalidated, because
            // it is already formatted:
            if ( bHadFollow || !pFrm->IsCntntFrm() || !((SwCntntFrm*)pFrm)->GetFollow() )
            {
                if ( !pFrm->IsTabFrm() || !((SwTabFrm*)pFrm)->GetFollow() )
                    pFrm->InvalidateNextPos();
            }
        }
    }

    //Fuer Hintergrundgrafiken muss bei Groessenaenderungen ein Repaint her.
    const BOOL bPrtWidth =
            (aPrt.*fnRect->fnGetWidth)() != (pFrm->Prt().*fnRect->fnGetWidth)();
    const BOOL bPrtHeight =
            (aPrt.*fnRect->fnGetHeight)()!=(pFrm->Prt().*fnRect->fnGetHeight)();
    if ( bPrtWidth || bPrtHeight )
    {
        const SvxGraphicPosition ePos = pFrm->GetAttrSet()->GetBackground().GetGraphicPos();
        if ( GPOS_NONE != ePos && GPOS_TILED != ePos )
            pFrm->SetCompletePaint();
    }
    else
    {
        // OD 13.11.2002 #97597# - consider case that *only* margins between
        // frame and printing area has changed. Then, frame has to be repainted,
        // in order to force paint of the margin areas.
        if ( !bAbsP && (bChgWidth || bChgHeight) )
        {
            pFrm->SetCompletePaint();
        }
    }

    const BOOL bPrtP = POS_DIFF( aPrt, pFrm->Prt() );
    if ( bAbsP || bPrtP || bChgWidth || bChgHeight ||
         bPrtWidth || bPrtHeight || bChgFlyBasePos )
    {
        if( pFrm->IsAccessibleFrm() )
        {
            SwRootFrm *pRootFrm = pFrm->FindRootFrm();
            if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
                pRootFrm->GetCurrShell() )
            {
                pRootFrm->GetCurrShell()->Imp()->MoveAccessibleFrm( pFrm, aFrm );
            }
        }

        // Notification of anchored objects
        if ( pFrm->GetDrawObjs() )
        {
            const SwSortedObjs &rObjs = *pFrm->GetDrawObjs();
            SwPageFrm* pPageFrm = 0;
            for ( sal_uInt32 i = 0; i < rObjs.Count(); ++i )
            {
                // OD 2004-03-31 #i26791# - no general distinction between
                // Writer fly frames and drawing objects
                bool bNotify = false;
                bool bNotifySize = false;
                SwAnchoredObject* pObj = rObjs[i];
                SwContact* pContact = ::GetUserCall( pObj->GetDrawObj() );
                // --> OD 2004-12-08 #115759#
                const bool bAnchoredAsChar = pContact->ObjAnchoredAsChar();
                if ( !bAnchoredAsChar )
                // <--
                {
                    // Notify object, which aren't anchored as-character:

                    // always notify objects, if frame position has changed
                    // or if the object is to-page|to-fly anchored.
                    if ( bAbsP ||
                         pContact->ObjAnchoredAtPage() ||
                         pContact->ObjAnchoredAtFly() )
                    {
                        bNotify = true;

                        // assure that to-fly anchored Writer fly frames are
                        // registered at the correct page frame, if frame
                        // position has changed.
                        if ( bAbsP && pContact->ObjAnchoredAtFly() &&
                             pObj->ISA(SwFlyFrm) )
                        {
                            // determine to-fly anchored Writer fly frame
                            SwFlyFrm* pFlyFrm = static_cast<SwFlyFrm*>(pObj);
                            // determine page frame of to-fly anchored
                            // Writer fly frame
                            SwPageFrm* pFlyPageFrm = pFlyFrm->FindPageFrm();
                            // determine page frame, if needed.
                            if ( !pPageFrm )
                            {
                                pPageFrm = pFrm->FindPageFrm();
                            }
                            if ( pPageFrm != pFlyPageFrm )
                            {
                                ASSERT( pFlyPageFrm, "~SwFrmNotify: Fly from Nowhere" );
                                if( pFlyPageFrm )
                                    pFlyPageFrm->MoveFly( pFlyFrm, pPageFrm );
                                else
                                    pPageFrm->AppendFlyToPage( pFlyFrm );
                            }
                        }
                    }
                    // otherwise the objects are notified in dependence to
                    // its positioning and alignment
                    else
                    {
                        const SwFmtVertOrient& rVert =
                                        pContact->GetFmt()->GetVertOrient();
                        if ( ( rVert.GetVertOrient() == text::VertOrientation::CENTER ||
                               rVert.GetVertOrient() == text::VertOrientation::BOTTOM ||
                               rVert.GetRelationOrient() == text::RelOrientation::PRINT_AREA ) &&
                             ( bChgHeight || bPrtHeight ) )
                        {
                            bNotify = true;
                        }
                        if ( !bNotify )
                        {
                            const SwFmtHoriOrient& rHori =
                                        pContact->GetFmt()->GetHoriOrient();
                            if ( ( rHori.GetHoriOrient() != text::HoriOrientation::NONE ||
                                   rHori.GetRelationOrient()== text::RelOrientation::PRINT_AREA ||
                                   rHori.GetRelationOrient()== text::RelOrientation::FRAME ) &&
                                 ( bChgWidth || bPrtWidth || bChgFlyBasePos ) )
                            {
                                bNotify = true;
                            }
                        }
                    }
                }
                else if ( bPrtWidth )
                {
                    // Notify as-character anchored objects, if printing area
                    // width has changed.
                    bNotify = true;
                    bNotifySize = true;
                }

                // perform notification via the corresponding invalidations
                if ( bNotify )
                {
                    if ( pObj->ISA(SwFlyFrm) )
                    {
                        SwFlyFrm* pFlyFrm = static_cast<SwFlyFrm*>(pObj);
                        if ( bNotifySize )
                            pFlyFrm->_InvalidateSize();
                        // --> OD 2004-12-08 #115759# - no invalidation of
                        // position for as-character anchored objects.
                        if ( !bAnchoredAsChar )
                        {
                            pFlyFrm->_InvalidatePos();
                        }
                        // <--
                        pFlyFrm->_Invalidate();
                    }
                    else if ( pObj->ISA(SwAnchoredDrawObject) )
                    {
                        // --> OD 2004-12-08 #115759# - no invalidation of
                        // position for as-character anchored objects.
                        if ( !bAnchoredAsChar )
                        {
                            pObj->InvalidateObjPos();
                        }
                        // <--
                    }
                    else
                    {
                        ASSERT( false,
                                "<SwCntntNotify::~SwCntntNotify()> - unknown anchored object type. Please inform OD." );
                    }
                }
            }
        }
    }
    else if( pFrm->IsTxtFrm() && bValidSize != pFrm->GetValidSizeFlag() )
    {
        SwRootFrm *pRootFrm = pFrm->FindRootFrm();
        if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
            pRootFrm->GetCurrShell() )
        {
            pRootFrm->GetCurrShell()->Imp()->InvalidateAccessibleFrmContent( pFrm );
        }
    }

    // #i9046# Automatic frame width
    SwFlyFrm* pFly = 0;
    // --> FME 2004-10-21 #i35879# Do not trust the inf flags. pFrm does not
    // necessarily have to have an upper!
    if ( !pFrm->IsFlyFrm() && 0 != ( pFly = pFrm->ImplFindFlyFrm() ) )
    // <--
    {
        // --> OD 2006-05-08 #i61999#
        // no invalidation of columned Writer fly frames, because automatic
        // width doesn't make sense for such Writer fly frames.
        if ( pFly->Lower() && !pFly->Lower()->IsColumnFrm() )
        {
            const SwFmtFrmSize &rFrmSz = pFly->GetFmt()->GetFrmSize();

            // This could be optimized. Basically the fly frame only has to
            // be invalidated, if the first line of pFrm (if pFrm is a content
            // frame, for other frame types its the print area) has changed its
            // size and pFrm was responsible for the current width of pFly. On
            // the other hand, this is only rarely used and re-calculation of
            // the fly frame does not cause too much trouble. So we keep it this
            // way:
            if ( ATT_FIX_SIZE != rFrmSz.GetWidthSizeType() )
            {
                // --> OD 2005-07-29 #i50668#, #i50998# - invalidation of position
                // of as-character anchored fly frames not needed and can cause
                // layout loops
                if ( !pFly->ISA(SwFlyInCntFrm) )
                {
                    pFly->InvalidatePos();
                }
                // <--
                pFly->InvalidateSize();
            }
        }
        // <--
    }
}

/*************************************************************************
|*
|*	SwLayNotify::SwLayNotify()
|*
|*	Ersterstellung		MA 17. Nov. 92
|*	Letzte Aenderung	MA 03. Jun. 93
|*
|*************************************************************************/


SwLayNotify::SwLayNotify( SwLayoutFrm *pLayFrm ) :
    SwFrmNotify( pLayFrm ),
    bLowersComplete( FALSE )
{
}

/*************************************************************************
|*
|*	SwLayNotify::~SwLayNotify()
|*
|*	Ersterstellung		MA 17. Nov. 92
|*	Letzte Aenderung	MA 13. Jun. 96
|*
|*************************************************************************/

// OD 2004-05-11 #i28701# - local method to invalidate the position of all
// frames inclusive its floating screen objects, which are lowers of the given
// layout frame
void lcl_InvalidatePosOfLowers( SwLayoutFrm& _rLayoutFrm )
{
    if( _rLayoutFrm.IsFlyFrm() && _rLayoutFrm.GetDrawObjs() )
    {
        _rLayoutFrm.InvalidateObjs( true, false );
    }

    SwFrm* pLowerFrm = _rLayoutFrm.Lower();
    while ( pLowerFrm )
    {
        pLowerFrm->InvalidatePos();
        if ( pLowerFrm->IsTxtFrm() )
        {
            static_cast<SwTxtFrm*>(pLowerFrm)->Prepare( PREP_POS_CHGD );
        }
        else if ( pLowerFrm->IsTabFrm() )
        {
            pLowerFrm->InvalidatePrt();
        }

        pLowerFrm->InvalidateObjs( true, false );

        pLowerFrm = pLowerFrm->GetNext();
    };
}

SwLayNotify::~SwLayNotify()
{
    // --> OD 2005-07-29 #i49383#
    if ( mbFrmDeleted )
    {
        return;
    }
    // <--

    SwLayoutFrm *pLay = GetLay();
    SWRECTFN( pLay )
    BOOL bNotify = FALSE;
    if ( pLay->Prt().SSize() != aPrt.SSize() )
    {
        if ( !IsLowersComplete() )
        {
            BOOL bInvaPercent;

            if ( pLay->IsRowFrm() )
            {
                bInvaPercent = TRUE;
                long nNew = (pLay->Prt().*fnRect->fnGetHeight)();
                if( nNew != (aPrt.*fnRect->fnGetHeight)() )
                     ((SwRowFrm*)pLay)->AdjustCells( nNew, TRUE);
                if( (pLay->Prt().*fnRect->fnGetWidth)()
                    != (aPrt.*fnRect->fnGetWidth)() )
                     ((SwRowFrm*)pLay)->AdjustCells( 0, FALSE );
            }
            else
            {
                //Proportionale Anpassung der innenliegenden.
                //1. Wenn der Formatierte kein Fly ist
                //2. Wenn er keine Spalten enthaelt
                //3. Wenn der Fly eine feste Hoehe hat und die Spalten in der
                //	 Hoehe danebenliegen.
                //4. niemals bei SectionFrms.
                BOOL bLow;
                if( pLay->IsFlyFrm() )
                {
                    if ( pLay->Lower() )
                    {
                        bLow = !pLay->Lower()->IsColumnFrm() ||
                            (pLay->Lower()->Frm().*fnRect->fnGetHeight)()
                             != (pLay->Prt().*fnRect->fnGetHeight)();
                    }
                    else
                        bLow = FALSE;
                }
                else if( pLay->IsSctFrm() )
                {
                    if ( pLay->Lower() )
                    {
                        if( pLay->Lower()->IsColumnFrm() && pLay->Lower()->GetNext() )
                            bLow = pLay->Lower()->Frm().Height() != pLay->Prt().Height();
                        else
                            bLow = pLay->Prt().Width() != aPrt.Width();
                    }
                    else
                        bLow = FALSE;
                }
                else if( pLay->IsFooterFrm() && !pLay->HasFixSize() )
                    bLow = pLay->Prt().Width() != aPrt.Width();
                else
                    bLow = TRUE;
                bInvaPercent = bLow;
                if ( bLow )
                {
                    pLay->ChgLowersProp( aPrt.SSize() );
                }
                //Wenn die PrtArea gewachsen ist, so ist es moeglich, dass die
                //Kette der Untergeordneten einen weiteren Frm aufnehmen kann,
                //mithin muss also der 'moeglicherweise passende' Invalidiert werden.
                //Das invalidieren lohnt nur, wenn es sich beim mir bzw. meinen
                //Uppers um eine Moveable-Section handelt.
                //Die PrtArea ist gewachsen, wenn die Breite oder die Hoehe groesser
                //geworden ist.
                if ( (pLay->Prt().Height() > aPrt.Height() ||
                      pLay->Prt().Width()  > aPrt.Width()) &&
                     (pLay->IsMoveable() || pLay->IsFlyFrm()) )
                {
                    SwFrm *pTmpFrm = pLay->Lower();
                    if ( pTmpFrm && pTmpFrm->IsFlowFrm() )
                    {
                        while ( pTmpFrm->GetNext() )
                            pTmpFrm = pTmpFrm->GetNext();
                        pTmpFrm->InvalidateNextPos();
                    }
                }
            }
            bNotify = TRUE;
            //TEUER!! aber wie macht man es geschickter?
            if( bInvaPercent )
                pLay->InvaPercentLowers( pLay->Prt().Height() - aPrt.Height() );
        }
        if ( pLay->IsTabFrm() )
            //Damit _nur_ der Shatten bei Groessenaenderungen gemalt wird.
            ((SwTabFrm*)pLay)->SetComplete();
        else if ( !pLay->GetFmt()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) ||
                  !(pLay->GetType() & (FRM_BODY | FRM_PAGE)) )
            //Damit die untergeordneten sauber retouchiert werden.
            //Problembsp: Flys an den Henkeln packen und verkleinern.
            //Nicht fuer Body und Page, sonst flackerts beim HTML-Laden.
            pLay->SetCompletePaint();

    }
    //Lower benachrichtigen wenn sich die Position veraendert hat.
    const BOOL bPrtPos = POS_DIFF( aPrt, pLay->Prt() );
    const BOOL bPos = bPrtPos || POS_DIFF( aFrm, pLay->Frm() );
    const BOOL bSize = pLay->Frm().SSize() != aFrm.SSize();

    if ( bPos && pLay->Lower() && !IsLowersComplete() )
        pLay->Lower()->InvalidatePos();

    if ( bPrtPos )
        pLay->SetCompletePaint();

    //Nachfolger benachrichtigen wenn sich die SSize geaendert hat.
    if ( bSize )
    {
        if( pLay->GetNext() )
        {
            if ( pLay->GetNext()->IsLayoutFrm() )
                pLay->GetNext()->_InvalidatePos();
            else
                pLay->GetNext()->InvalidatePos();
        }
        else if( pLay->IsSctFrm() )
            pLay->InvalidateNextPos();
    }
    if ( !IsLowersComplete() &&
         !((pLay->GetType()&FRM_FLY|FRM_SECTION) &&
            pLay->Lower() && pLay->Lower()->IsColumnFrm()) &&
         (bPos || bNotify) && !(pLay->GetType() & 0x1823) )  //Tab, Row, FtnCont, Root, Page
    {
        // --> OD 2005-03-11 #i44016# - force unlock of position of lower objects.
        // --> OD 2005-03-30 #i43913# - no unlock of position of objects,
        // if <pLay> is a cell frame, and its table frame resp. its parent table
        // frame is locked.
        // --> OD 2005-04-15 #i47458# - force unlock of position of lower objects,
        // only if position of layout frame has changed.
        bool bUnlockPosOfObjs( bPos );
        if ( bUnlockPosOfObjs && pLay->IsCellFrm() )
        {
            SwTabFrm* pTabFrm( pLay->FindTabFrm() );
            if ( pTabFrm &&
                 ( pTabFrm->IsJoinLocked() ||
                   ( pTabFrm->IsFollow() &&
                     pTabFrm->FindMaster()->IsJoinLocked() ) ) )
            {
                bUnlockPosOfObjs = false;
            }
        }
        // --> OD 2005-05-18 #i49383# - check for footnote frame, if unlock
        // of position of lower objects is allowed.
        else if ( bUnlockPosOfObjs && pLay->IsFtnFrm() )
        {
            bUnlockPosOfObjs = static_cast<SwFtnFrm*>(pLay)->IsUnlockPosOfLowerObjs();
        }
        // <--
        // --> OD 2005-07-29 #i51303# - no unlock of object positions for sections
        else if ( bUnlockPosOfObjs && pLay->IsSctFrm() )
        {
            bUnlockPosOfObjs = false;
        }
        // <--
        pLay->NotifyLowerObjs( bUnlockPosOfObjs );
        // <--
    }
    if ( bPos && pLay->IsFtnFrm() && pLay->Lower() )
    {
        // OD 2004-05-11 #i28701#
        ::lcl_InvalidatePosOfLowers( *pLay );
    }
    if( ( bPos || bSize ) && pLay->IsFlyFrm() && ((SwFlyFrm*)pLay)->GetAnchorFrm()
          && ((SwFlyFrm*)pLay)->GetAnchorFrm()->IsFlyFrm() )
        ((SwFlyFrm*)pLay)->AnchorFrm()->InvalidateSize();
}

/*************************************************************************
|*
|*	SwFlyNotify::SwFlyNotify()
|*
|*	Ersterstellung		MA 17. Nov. 92
|*	Letzte Aenderung	MA 26. Aug. 93
|*
|*************************************************************************/

SwFlyNotify::SwFlyNotify( SwFlyFrm *pFlyFrm ) :
    SwLayNotify( pFlyFrm ),
    // --> OD 2004-11-24 #115759# - keep correct page frame - the page frame
    // the Writer fly frame is currently registered at.
    pOldPage( pFlyFrm->GetPageFrm() ),
    // <--
    aFrmAndSpace( pFlyFrm->GetObjRectWithSpaces() )
{
}

/*************************************************************************
|*
|*	SwFlyNotify::~SwFlyNotify()
|*
|*	Ersterstellung		MA 17. Nov. 92
|*	Letzte Aenderung	MA 09. Nov. 95
|*
|*************************************************************************/

SwFlyNotify::~SwFlyNotify()
{
    // --> OD 2005-07-29 #i49383#
    if ( mbFrmDeleted )
    {
        return;
    }
    // <--

    SwFlyFrm *pFly = GetFly();
    if ( pFly->IsNotifyBack() )
    {
        ViewShell *pSh = pFly->GetShell();
        SwViewImp *pImp = pSh ? pSh->Imp() : 0;
        if ( !pImp || !pImp->IsAction() || !pImp->GetLayAction().IsAgain() )
        {
            //Wenn in der LayAction das IsAgain gesetzt ist kann es sein,
            //dass die alte Seite inzwischen vernichtet wurde!
            ::Notify( pFly, pOldPage, aFrmAndSpace, &aPrt );
            // --> OD 2004-10-20 #i35640# - additional notify anchor text frame,
            // if Writer fly frame has changed its page
            if ( pFly->GetAnchorFrm()->IsTxtFrm() &&
                 pFly->GetPageFrm() != pOldPage )
            {
                pFly->AnchorFrm()->Prepare( PREP_FLY_LEAVE );
            }
            // <--
        }
        pFly->ResetNotifyBack();
    }

    //Haben sich Groesse oder Position geaendert, so sollte die View
    //das wissen.
    SWRECTFN( pFly )
    const bool bPosChgd = POS_DIFF( aFrm, pFly->Frm() );
    const bool bFrmChgd = pFly->Frm().SSize() != aFrm.SSize();
    const bool bPrtChgd = aPrt != pFly->Prt();
    if ( bPosChgd || bFrmChgd || bPrtChgd )
    {
        pFly->NotifyDrawObj();
    }
    if ( bPosChgd && aFrm.Pos().X() != WEIT_WECH )
    {
        // OD 2004-05-10 #i28701# - no direct move of lower Writer fly frames.
        // reason: New positioning and alignment (e.g. to-paragraph anchored,
        // but aligned at page) are introduced.
        // <SwLayNotify::~SwLayNotify()> takes care of invalidation of lower
        // floating screen objects by calling method <SwLayoutFrm::NotifyLowerObjs()>.

        if ( pFly->IsFlyAtCntFrm() )
        {
            SwFrm *pNxt = pFly->AnchorFrm()->FindNext();
            if ( pNxt )
            {
                pNxt->InvalidatePos();
            }
        }

        // --> OD 2004-11-05 #i26945# - notify anchor.
        // Needed for negative positioned Writer fly frames
        if ( pFly->GetAnchorFrm()->IsTxtFrm() )
        {
            pFly->AnchorFrm()->Prepare( PREP_FLY_LEAVE );
        }
        // <--
    }

    // OD 2004-05-13 #i28701#
    // --> OD 2005-03-21 #i45180# - no adjustment of layout process flags and
    // further notifications/invalidations, if format is called by grow/shrink
    if ( pFly->ConsiderObjWrapInfluenceOnObjPos() &&
         ( !pFly->ISA(SwFlyFreeFrm) ||
           !static_cast<SwFlyFreeFrm*>(pFly)->IsNoMoveOnCheckClip() ) )
    // <--
    {
        // --> OD 2005-09-05 #i54138# - suppress restart of the layout process
        // on changed frame height.
        // Note: It doesn't seem to be necessary and can cause layout loops.
        if ( bPosChgd )
        // <--
        {
            // indicate a restart of the layout process
            pFly->SetRestartLayoutProcess( true );
        }
        else
        {
            // lock position
            pFly->LockPosition();

            if ( !pFly->ConsiderForTextWrap() )
            {
                // indicate that object has to be considered for text wrap
                pFly->SetConsiderForTextWrap( true );
                // invalidate 'background' in order to allow its 'background'
                // to wrap around it.
                pFly->NotifyBackground( pFly->GetPageFrm(),
                                        pFly->GetObjRectWithSpaces(),
                                        PREP_FLY_ARRIVE );
                // invalidate position of anchor frame in order to force
                // a re-format of the anchor frame, which also causes a
                // re-format of the invalid previous frames of the anchor frame.
                pFly->AnchorFrm()->InvalidatePos();
            }
        }
    }
}

/*************************************************************************
|*
|*	SwCntntNotify::SwCntntNotify()
|*
|*	Ersterstellung		MA 24. Nov. 92
|*	Letzte Aenderung	MA 16. May. 95
|*
|*************************************************************************/

SwCntntNotify::SwCntntNotify( SwCntntFrm *pCntntFrm ) :
    SwFrmNotify( pCntntFrm ),
    // OD 08.01.2004 #i11859#
    mbChkHeightOfLastLine( false ),
    mnHeightOfLastLine( 0L ),
    // OD 2004-02-26 #i25029#
    mbInvalidatePrevPrtArea( false ),
    mbBordersJoinedWithPrev( false )
{
    // OD 08.01.2004 #i11859#
    if ( pCntntFrm->IsTxtFrm() )
    {
        SwTxtFrm* pTxtFrm = static_cast<SwTxtFrm*>(pCntntFrm);
        if ( !pTxtFrm->GetTxtNode()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::OLD_LINE_SPACING) )
        {
            const SwAttrSet* pSet = pTxtFrm->GetAttrSet();
            const SvxLineSpacingItem &rSpace = pSet->GetLineSpacing();
            if ( rSpace.GetInterLineSpaceRule() == SVX_INTER_LINE_SPACE_PROP )
            {
                mbChkHeightOfLastLine = true;
                mnHeightOfLastLine = pTxtFrm->GetHeightOfLastLine();
            }
        }
    }
}

/*************************************************************************
|*
|*	SwCntntNotify::~SwCntntNotify()
|*
|*	Ersterstellung		MA 24. Nov. 92
|*	Letzte Aenderung	MA 09. Apr. 97
|*
|*************************************************************************/

SwCntntNotify::~SwCntntNotify()
{
    // --> OD 2005-07-29 #i49383#
    if ( mbFrmDeleted )
    {
        return;
    }
    // <--

    SwCntntFrm *pCnt = GetCnt();
    if ( bSetCompletePaintOnInvalidate )
        pCnt->SetCompletePaint();

    SWRECTFN( pCnt )
    if ( pCnt->IsInTab() && ( POS_DIFF( pCnt->Frm(), aFrm ) ||
                             pCnt->Frm().SSize() != aFrm.SSize()))
    {
        SwLayoutFrm* pCell = pCnt->GetUpper();
        while( !pCell->IsCellFrm() && pCell->GetUpper() )
            pCell = pCell->GetUpper();
        ASSERT( pCell->IsCellFrm(), "Where's my cell?" );
        if ( text::VertOrientation::NONE != pCell->GetFmt()->GetVertOrient().GetVertOrient() )
            pCell->InvalidatePrt();	//fuer vertikale Ausrichtung.
    }

    // OD 2004-02-26 #i25029#
    if ( mbInvalidatePrevPrtArea && mbBordersJoinedWithPrev &&
         pCnt->IsTxtFrm() &&
         !pCnt->IsFollow() && !pCnt->GetIndPrev() )
    {
        // determine previous frame
        SwFrm* pPrevFrm = pCnt->FindPrev();
        // skip empty section frames and hidden text frames
        {
            while ( pPrevFrm &&
                    ( ( pPrevFrm->IsSctFrm() &&
                        !static_cast<SwSectionFrm*>(pPrevFrm)->GetSection() ) ||
                      ( pPrevFrm->IsTxtFrm() &&
                        static_cast<SwTxtFrm*>(pPrevFrm)->IsHiddenNow() ) ) )
            {
                pPrevFrm = pPrevFrm->FindPrev();
            }
        }

        // Invalidate printing area of found previous frame
        if ( pPrevFrm )
        {
            if ( pPrevFrm->IsSctFrm() )
            {
                if ( pCnt->IsInSct() )
                {
                    // Note: found previous frame is a section frame and
                    //       <pCnt> is also inside a section.
                    //       Thus due to <mbBordersJoinedWithPrev>,
                    //       <pCnt> had joined its borders/shadow with the
                    //       last content of the found section.
                    // Invalidate printing area of last content in found section.
                    SwFrm* pLstCntntOfSctFrm =
                            static_cast<SwSectionFrm*>(pPrevFrm)->FindLastCntnt();
                    if ( pLstCntntOfSctFrm )
                    {
                        pLstCntntOfSctFrm->InvalidatePrt();
                    }
                }
            }
            else
            {
                pPrevFrm->InvalidatePrt();
            }
        }
    }

    BOOL bFirst = (aFrm.*fnRect->fnGetWidth)() == 0;

    if ( pCnt->IsNoTxtFrm() )
    {
        //Aktive PlugIn's oder OLE-Objekte sollten etwas von der Veraenderung
        //mitbekommen, damit sie Ihr Window entsprechend verschieben.
        ViewShell *pSh  = pCnt->GetShell();
        if ( pSh )
        {
            SwOLENode *pNd;
            if ( 0 != (pNd = pCnt->GetNode()->GetOLENode()) &&
                 (pNd->GetOLEObj().IsOleRef() ||
                  pNd->IsOLESizeInvalid()) )
            {
                ASSERT( pCnt->IsInFly(), "OLE not in FlyFrm" );
                SwFlyFrm *pFly = pCnt->FindFlyFrm();
                svt::EmbeddedObjectRef& xObj = pNd->GetOLEObj().GetObject();
                SwFEShell *pFESh = 0;
                ViewShell *pTmp = pSh;
                do
                {	if ( pTmp->ISA( SwCrsrShell ) )
                    {
                        pFESh = (SwFEShell*)pTmp;
                        // #108369#: Here used to be the condition if (!bFirst).
                        // I think this should mean "do not call CalcAndSetScale"
                        // if the frame is formatted for the first time.
                        // Unfortunately this is not valid anymore since the
                        // SwNoTxtFrm already gets a width during CalcLowerPreps.
                        // Nevertheless, the indention of !bFirst seemed to be
                        // to assure that the OLE objects have already been notified
                        // if necessary before calling CalcAndSetScale.
                        // So I replaced !bFirst by !IsOLESizeInvalid. There is
                        // one additional problem specific to the word import:
                        // The layout is calculated _before_ calling PrtOLENotify,
                        // and the OLE objects are not invalidated during import.
                        // Therefore I added the condition !IsUpdateExpFld,
                        // have a look at the occurence of CalcLayout in
                        // uiview/view.cxx.
                        if ( !pNd->IsOLESizeInvalid() &&
                             !pSh->GetDoc()->IsUpdateExpFld() )
                            pFESh->CalcAndSetScale( xObj, &pFly->Prt(), &pFly->Frm());
                    }
                    pTmp = (ViewShell*)pTmp->GetNext();
                } while ( pTmp != pSh );

                if ( pFESh && pNd->IsOLESizeInvalid() )
                {
                    pNd->SetOLESizeInvalid( FALSE );
                    //TODO/LATER: needs OnDocumentPrinterChanged
                    //xObj->OnDocumentPrinterChanged( pNd->GetDoc()->getPrinter( false ) );
                    pFESh->CalcAndSetScale( xObj );//Client erzeugen lassen.
                }
            }
            //dito Animierte Grafiken
            if ( Frm().HasArea() && ((SwNoTxtFrm*)pCnt)->HasAnimation() )
            {
                ((SwNoTxtFrm*)pCnt)->StopAnimation();
                pSh->InvalidateWindows( Frm() );
            }
        }
    }

    if ( bFirst )
    {
        pCnt->SetRetouche();	//fix(13870)

        SwDoc *pDoc = pCnt->GetNode()->GetDoc();
        if ( pDoc->GetSpzFrmFmts()->Count() &&
             !pDoc->IsLoaded() && !pDoc->IsNewDoc() )
        {
            //Der Frm wurde wahrscheinlich zum ersten mal formatiert.
            //Wenn ein Filter Flys oder Zeichenobjekte einliest und diese
            //Seitengebunden sind, hat er ein Problem, weil er i.d.R. die
            //Seitennummer nicht kennt. Er weiss lediglich welches der Inhalt
            //(CntntNode) an dieser Stelle ist.
            //Die Filter stellen dazu das Ankerattribut der Objekte so ein, dass
            //sie vom Typ zwar Seitengebunden sind, aber der Index des Ankers
            //auf diesen CntntNode zeigt.
            //Hier werden diese vorlauefigen Verbindungen aufgeloest.

            const SwPageFrm *pPage = 0;
            SwNodeIndex   *pIdx  = 0;
            SwSpzFrmFmts *pTbl = pDoc->GetSpzFrmFmts();

            for ( USHORT i = 0; i < pTbl->Count(); ++i )
            {
                if ( !pPage )
                    pPage = pCnt->FindPageFrm();
                SwFrmFmt *pFmt = (*pTbl)[i];
                const SwFmtAnchor &rAnch = pFmt->GetAnchor();

                if ( FLY_PAGE 		!= rAnch.GetAnchorId() &&
                     FLY_AT_CNTNT	!= rAnch.GetAnchorId() )
                    continue;	//#60878# nicht etwa zeichengebundene.

                BOOL bCheckPos = FALSE;
                if ( rAnch.GetCntntAnchor() )
                {
                    if ( !pIdx )
                    {
                        pIdx = new SwNodeIndex( *pCnt->GetNode() );
                    }
                    if ( rAnch.GetCntntAnchor()->nNode == *pIdx )
                    {
                        bCheckPos = TRUE;
                        if ( FLY_PAGE == rAnch.GetAnchorId() )
                        {
                            ASSERT( false, "<SwCntntNotify::~SwCntntNotify()> - to page anchored object with content position. Please inform OD." );
                            SwFmtAnchor aAnch( rAnch );
                            aAnch.SetAnchor( 0 );
                            aAnch.SetPageNum( pPage->GetPhyPageNum() );
                            pFmt->SetAttr( aAnch );
                            if ( RES_DRAWFRMFMT != pFmt->Which() )
                                pFmt->MakeFrms();
                        }
                    }
                }
            }
            delete pIdx;
        }
    }

    // OD 12.01.2004 #i11859# - invalidate printing area of following frame,
    //  if height of last line has changed.
    if ( pCnt->IsTxtFrm() && mbChkHeightOfLastLine )
    {
        if ( mnHeightOfLastLine != static_cast<SwTxtFrm*>(pCnt)->GetHeightOfLastLine() )
        {
            pCnt->InvalidateNextPrtArea();
        }
    }

    // --> OD 2005-03-07 #i44049#
    if ( pCnt->IsTxtFrm() && POS_DIFF( aFrm, pCnt->Frm() ) )
    {
        pCnt->InvalidateObjs( true );
    }
    // <--

    // --> OD 2005-04-12 #i43255# - move code to invalidate at-character
    // anchored objects due to a change of its anchor character from
    // method <SwTxtFrm::Format(..)>.
    if ( pCnt->IsTxtFrm() )
    {
        SwTxtFrm* pMasterFrm = pCnt->IsFollow()
                               ? static_cast<SwTxtFrm*>(pCnt)->FindMaster()
                               : static_cast<SwTxtFrm*>(pCnt);
        if ( pMasterFrm && !pMasterFrm->IsFlyLock() &&
             pMasterFrm->GetDrawObjs() )
        {
            SwSortedObjs* pObjs = pMasterFrm->GetDrawObjs();
            for ( sal_uInt32 i = 0; i < pObjs->Count(); ++i )
            {
                SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
                if ( pAnchoredObj->GetFrmFmt().GetAnchor().GetAnchorId()
                        == FLY_AUTO_CNTNT )
                {
                    pAnchoredObj->CheckCharRectAndTopOfLine( !pMasterFrm->IsEmpty() );
                }
            }
        }
    }
    // <--
}

/*************************************************************************
|*
|*	InsertCnt
|*
|*	Beschreibung		Hilfsfunktionen, die friend von irgendwem sind, damit
|*						nicht immer gleich 'ne ganze Klasse befreundet werden
|*						muss.
|*	Ersterstellung		MA 13. Apr. 93
|*	Letzte Aenderung	MA 11. May. 95
|*
|*************************************************************************/

void AppendObjs( const SwSpzFrmFmts *pTbl, ULONG nIndex,
                        SwFrm *pFrm, SwPageFrm *pPage )
{
    for ( USHORT i = 0; i < pTbl->Count(); ++i )
    {
        SwFrmFmt *pFmt = (SwFrmFmt*)(*pTbl)[i];
        const SwFmtAnchor &rAnch = pFmt->GetAnchor();
        if ( rAnch.GetCntntAnchor() &&
             (rAnch.GetCntntAnchor()->nNode.GetIndex() == nIndex) )
        {
            const bool bFlyAtFly = rAnch.GetAnchorId() == FLY_AT_FLY; // LAYER_IMPL
            //Wird ein Rahmen oder ein SdrObject beschrieben?
            const bool bSdrObj = RES_DRAWFRMFMT == pFmt->Which();
            // OD 23.06.2003 #108784# - append also drawing objects anchored
            // as character.
            const bool bDrawObjInCntnt = bSdrObj &&
                                         rAnch.GetAnchorId() == FLY_IN_CNTNT;

            if( bFlyAtFly ||
                rAnch.GetAnchorId() == FLY_AT_CNTNT ||
                rAnch.GetAnchorId() == FLY_AUTO_CNTNT ||
                bDrawObjInCntnt )
            {
                SdrObject* pSdrObj = 0;
                if ( bSdrObj && 0 == (pSdrObj = pFmt->FindSdrObject()) )
                {
                    ASSERT( !bSdrObj, "DrawObject not found." );
                    pFmt->GetDoc()->DelFrmFmt( pFmt );
                    --i;
                    continue;
                }
                if ( pSdrObj )
                {
                    if ( !pSdrObj->GetPage() )
                    {
                        pFmt->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage(0)->
                                InsertObject(pSdrObj, pSdrObj->GetOrdNumDirect());
                    }

                    SwDrawContact* pNew =
                        static_cast<SwDrawContact*>(GetUserCall( pSdrObj ));
                    if ( !pNew->GetAnchorFrm() )
                    {
                        pFrm->AppendDrawObj( *(pNew->GetAnchoredObj( 0L )) );
                    }
                    // OD 19.06.2003 #108784# - add 'virtual' drawing object,
                    // if necessary. But control objects have to be excluded.
                    else if ( !::CheckControlLayer( pSdrObj ) &&
                              pNew->GetAnchorFrm() != pFrm &&
                              !pNew->GetDrawObjectByAnchorFrm( *pFrm ) )
                    {
                        SwDrawVirtObj* pDrawVirtObj = pNew->AddVirtObj();
                        pFrm->AppendDrawObj( *(pNew->GetAnchoredObj( pDrawVirtObj )) );

                        // for repaint, use new ActionChanged()
                        // pDrawVirtObj->SendRepaintBroadcast();
                        pDrawVirtObj->ActionChanged();
                    }

                }
                else
                {
                    SwFlyFrm *pFly;
                    if( bFlyAtFly )
                        pFly = new SwFlyLayFrm( (SwFlyFrmFmt*)pFmt, pFrm );
                    else
                        pFly = new SwFlyAtCntFrm( (SwFlyFrmFmt*)pFmt, pFrm );
                    pFly->Lock();
                    pFrm->AppendFly( pFly );
                    pFly->Unlock();
                    if ( pPage )
                        ::RegistFlys( pPage, pFly );
                }
            }
        }
    }
}

BOOL MA_FASTCALL lcl_ObjConnected( SwFrmFmt *pFmt )
{
    SwClientIter aIter( *pFmt );
    if ( RES_FLYFRMFMT == pFmt->Which() )
        return 0 != aIter.First( TYPE(SwFlyFrm) );
    else
    {
        SwDrawContact *pContact;
        if ( 0 != (pContact = (SwDrawContact*)aIter.First( TYPE(SwDrawContact))))
            return pContact->GetAnchorFrm() != 0;
    }
    return FALSE;
}

/** helper method to determine, if a <SwFrmFmt>, which has an object connected,
    is located in header or footer.

    OD 23.06.2003 #108784#

    @author OD
*/
bool lcl_InHeaderOrFooter( SwFrmFmt& _rFmt )
{
    ASSERT( lcl_ObjConnected( &_rFmt ),
            "::lcl_InHeaderOrFooter(..) - <SwFrmFmt> has no connected object" );

    bool bRetVal = false;

    const SwFmtAnchor& rAnch = _rFmt.GetAnchor();

    if ( rAnch.GetAnchorId() != FLY_PAGE )
    {
        bRetVal = _rFmt.GetDoc()->IsInHeaderFooter( rAnch.GetCntntAnchor()->nNode );
    }

    return bRetVal;
}

void AppendAllObjs( const SwSpzFrmFmts *pTbl )
{
    //Verbinden aller Objekte, die in der SpzTbl beschrieben sind mit dem
    //Layout.
    //Wenn sich nix mehr tut hoeren wir auf. Dann koennen noch Formate
    //uebrigbleiben, weil wir weder zeichengebunde Rahmen verbinden noch
    //Objecte die in zeichengebundenen verankert sind.

    SwSpzFrmFmts aCpy( 255, 255 );
    aCpy.Insert( pTbl, 0 );

    USHORT nOldCnt = USHRT_MAX;

    while ( aCpy.Count() && aCpy.Count() != nOldCnt )
    {
        nOldCnt = aCpy.Count();
        for ( int i = 0; i < int(aCpy.Count()); ++i )
        {
            SwFrmFmt *pFmt = (SwFrmFmt*)aCpy[ USHORT(i) ];
            const SwFmtAnchor &rAnch = pFmt->GetAnchor();
            BOOL bRemove = FALSE;
            if ( rAnch.GetAnchorId() == FLY_PAGE || rAnch.GetAnchorId() == FLY_IN_CNTNT )
                //Seitengebunde sind bereits verankert, zeichengebundene
                //will ich hier nicht.
                bRemove = TRUE;
            else if ( FALSE == (bRemove = ::lcl_ObjConnected( pFmt )) ||
                      ::lcl_InHeaderOrFooter( *pFmt ) )
            {
            // OD 23.06.2003 #108784# - correction: for objects in header
            // or footer create frames, in spite of the fact that an connected
            // objects already exists.
                //Fuer Flys und DrawObjs nur dann ein MakeFrms rufen wenn noch
                //keine abhaengigen Existieren, andernfalls, oder wenn das
                //MakeFrms keine abhaengigen erzeugt, entfernen.
                pFmt->MakeFrms();
                bRemove = ::lcl_ObjConnected( pFmt );
            }
            if ( bRemove )
            {
                aCpy.Remove( USHORT(i) );
                --i;
            }
        }
    }
    aCpy.Remove( 0, aCpy.Count() );
}

/** local method to set 'working' position for newly inserted frames

    OD 12.08.2003 #i17969#

    @author OD
*/
void lcl_SetPos( SwFrm&             _rNewFrm,
                 const SwLayoutFrm& _rLayFrm )
{
    SWRECTFN( (&_rLayFrm) )
    (_rNewFrm.Frm().*fnRect->fnSetPos)( (_rLayFrm.Frm().*fnRect->fnGetPos)() );
    // move position by one SwTwip in text flow direction in order to get
    // notifications for a new calculated position after its formatting.
    if ( bVert )
        _rNewFrm.Frm().Pos().X() -= 1;
    else
        _rNewFrm.Frm().Pos().Y() += 1;
}

void MA_FASTCALL _InsertCnt( SwLayoutFrm *pLay, SwDoc *pDoc,
                             ULONG nIndex, BOOL bPages, ULONG nEndIndex,
                             SwFrm *pPrv )
{
    pDoc->BlockIdling();
    SwRootFrm* pLayout = pDoc->GetRootFrm();
    const BOOL bOldCallbackActionEnabled = pLayout ? pLayout->IsCallbackActionEnabled() : sal_False;
    if(pLayout)
        pLayout->SetCallbackActionEnabled( FALSE );

    //Bei der Erzeugung des Layouts wird bPages mit TRUE uebergeben. Dann
    //werden schon mal alle x Absaetze neue Seiten angelegt. Bei umbruechen
    //und/oder Pagedescriptorwechseln werden gleich die entsprechenden Seiten
    //angelegt.
    //Vorteil ist, das einerseits schon eine annaehernd realistische Zahl von
    //Seiten angelegt wird, vor allem aber gibt es nicht mehr eine schier
    //lange Kette von Absaetzen teuer verschoben werden muss, bis sie sich auf
    //ertraegliches mass reduziert hat.
    //Wir gehen mal davon aus, da? 20 Absaetze auf eine Seite passen
    //Damit es in extremen Faellen nicht gar so heftig rechenen wir je nach
    //Node noch etwas drauf.
    //Wenn in der DocStatistik eine brauchebare Seitenzahl angegeben ist
    //(wird beim Schreiben gepflegt), so wird von dieser Seitenanzahl
    //ausgegengen.
    const BOOL bStartPercent = bPages && !nEndIndex;

    SwPageFrm *pPage = pLay->FindPageFrm();
    const SwSpzFrmFmts *pTbl = pDoc->GetSpzFrmFmts();
    SwFrm		*pFrm = 0;
    BOOL   bBreakAfter	 = FALSE;

    SwActualSection *pActualSection = 0;
    SwLayHelper *pPageMaker;

    //Wenn das Layout erzeugt wird (bPages == TRUE) steuern wir den Progress
    //an. Flys und DrawObjekte werden dann nicht gleich verbunden, dies
    //passiert erst am Ende der Funktion.
    if ( bPages )
    {
        // Attention: the SwLayHelper class uses references to the content-,
        // page-, layout-frame etc. and may change them!
        pPageMaker = new SwLayHelper( pDoc, pFrm, pPrv, pPage, pLay,
                pActualSection, bBreakAfter, nIndex, 0 == nEndIndex );
        if( bStartPercent )
        {
            const ULONG nPageCount = pPageMaker->CalcPageCount();
            if( nPageCount )
                bObjsDirect = FALSE;
        }
    }
    else
        pPageMaker = NULL;

    if( pLay->IsInSct() &&
        ( pLay->IsSctFrm() || pLay->GetUpper() ) ) // Hierdurch werden Frischlinge
            // abgefangen, deren Flags noch nicht ermittelt werden koennen,
            // so z.B. beim Einfuegen einer Tabelle
    {
        SwSectionFrm* pSct = pLay->FindSctFrm();
        // Wenn Inhalt in eine Fussnote eingefuegt wird, die in einem spaltigen
        // Bereich liegt, so darf der spaltige Bereich nicht aufgebrochen werden.
        // Nur wenn im Innern der Fussnote ein Bereich liegt, ist dies ein
        // Kandidat fuer pActualSection.
        // Gleiches gilt fuer Bereiche in Tabellen, wenn innerhalb einer Tabelle
        // eingefuegt wird, duerfen nur Bereiche, die ebenfalls im Innern liegen,
        // aufgebrochen werden.
        if( ( !pLay->IsInFtn() || pSct->IsInFtn() ) &&
            ( !pLay->IsInTab() || pSct->IsInTab() ) )
        {
            pActualSection = new SwActualSection( 0, pSct, 0 );
            ASSERT( !pLay->Lower() || !pLay->Lower()->IsColumnFrm(),
                "_InsertCnt: Wrong Call" );
        }
    }

    //If a section is "open", the pActualSection points to an SwActualSection.
    //If the page breaks, for "open" sections a follow will created.
    //For nested sections (which have, however, not a nested layout),
    //the SwActualSection class has a member, which points to an upper(section).
    //When the "inner" section finishs, the upper will used instead.

    while( TRUE )
    {
        SwNode *pNd = pDoc->GetNodes()[nIndex];
        if ( pNd->IsCntntNode() )
        {
            SwCntntNode* pNode = (SwCntntNode*)pNd;
            pFrm = pNode->IsTxtNode() ? new SwTxtFrm( (SwTxtNode*)pNode ) :
                                        pNode->MakeFrm();
            if( pPageMaker )
                pPageMaker->CheckInsert( nIndex );

            pFrm->InsertBehind( pLay, pPrv );
            // --> OD 2005-12-01 #i27138#
            // notify accessibility paragraphs objects about changed
            // CONTENT_FLOWS_FROM/_TO relation.
            // Relation CONTENT_FLOWS_FROM for next paragraph will change
            // and relation CONTENT_FLOWS_TO for previous paragraph will change.
            if ( pFrm->IsTxtFrm() )
            {
                ViewShell* pViewShell( pFrm->GetShell() );
                // no notification, if <ViewShell> is in construction
                if ( pViewShell && !pViewShell->IsInConstructor() &&
                     pViewShell->GetLayout() &&
                     pViewShell->GetLayout()->IsAnyShellAccessible() )
                {
                    pViewShell->InvalidateAccessibleParaFlowRelation(
                        dynamic_cast<SwTxtFrm*>(pFrm->FindNextCnt( true )),
                        dynamic_cast<SwTxtFrm*>(pFrm->FindPrevCnt( true )) );
                    // --> OD 2006-08-28 #i68958#
                    // The information flags of the text frame are validated
                    // in methods <FindNextCnt(..)> and <FindPrevCnt(..)>.
                    // The information flags have to be invalidated, because
                    // it is possible, that the one of its upper frames
                    // isn't inserted into the layout.
                    pFrm->InvalidateInfFlags();
                    // <--
                }
            }
            // <--
            // OD 12.08.2003 #i17969# - consider horizontal/vertical layout
            // for setting position at newly inserted frame
            lcl_SetPos( *pFrm, *pLay );
            pPrv = pFrm;

            if ( pTbl->Count() && bObjsDirect && !bDontCreateObjects )
                AppendObjs( pTbl, nIndex, pFrm, pPage );
        }
        else if ( pNd->IsTableNode() )
        {	//Sollten wir auf eine Tabelle gestossen sein?
            SwTableNode *pTblNode = (SwTableNode*)pNd;

            // #108116# loading may produce table structures that GCLines
            // needs to clean up. To keep table formulas correct, change
            // all table formulas to internal (BOXPTR) representation.
            SwTableFmlUpdate aMsgHnt( &pTblNode->GetTable() );
            aMsgHnt.eFlags = TBL_BOXPTR;
            pDoc->UpdateTblFlds( &aMsgHnt );
            pTblNode->GetTable().GCLines();

            pFrm = pTblNode->MakeFrm();

            if( pPageMaker )
                pPageMaker->CheckInsert( nIndex );

            pFrm->InsertBehind( pLay, pPrv );
            // --> OD 2005-12-01 #i27138#
            // notify accessibility paragraphs objects about changed
            // CONTENT_FLOWS_FROM/_TO relation.
            // Relation CONTENT_FLOWS_FROM for next paragraph will change
            // and relation CONTENT_FLOWS_TO for previous paragraph will change.
            {
                ViewShell* pViewShell( pFrm->GetShell() );
                // no notification, if <ViewShell> is in construction
                if ( pViewShell && !pViewShell->IsInConstructor() &&
                     pViewShell->GetLayout() &&
                     pViewShell->GetLayout()->IsAnyShellAccessible() )
                {
                    pViewShell->InvalidateAccessibleParaFlowRelation(
                            dynamic_cast<SwTxtFrm*>(pFrm->FindNextCnt( true )),
                            dynamic_cast<SwTxtFrm*>(pFrm->FindPrevCnt( true )) );
                }
            }
            // <--
            if ( bObjsDirect && pTbl->Count() )
                ((SwTabFrm*)pFrm)->RegistFlys();
            // OD 12.08.2003 #i17969# - consider horizontal/vertical layout
            // for setting position at newly inserted frame
            lcl_SetPos( *pFrm, *pLay );

            pPrv = pFrm;
            //Index auf den Endnode der Tabellensection setzen.
            nIndex = pTblNode->EndOfSectionIndex();

            SwTabFrm* pTmpFrm = (SwTabFrm*)pFrm;
            while ( pTmpFrm )
            {
                pTmpFrm->CheckDirChange();
                pTmpFrm = pTmpFrm->IsFollow() ? pTmpFrm->FindMaster() : NULL;
            }

        }
        else if ( pNd->IsSectionNode() )
        {
            SwSectionNode *pNode = (SwSectionNode*)pNd;
            if( pNode->GetSection().CalcHiddenFlag() )
                // ist versteckt, ueberspringe den Bereich
                nIndex = pNode->EndOfSectionIndex();
            else
            {
                pFrm = pNode->MakeFrm();
                pActualSection = new SwActualSection( pActualSection,
                                                (SwSectionFrm*)pFrm, pNode );
                if ( pActualSection->GetUpper() )
                {
                    //Hinter den Upper einsetzen, beim EndNode wird der "Follow"
                    //des Uppers erzeugt.
                    SwSectionFrm *pTmp = pActualSection->GetUpper()->GetSectionFrm();
                    pFrm->InsertBehind( pTmp->GetUpper(), pTmp );
                    // OD 25.03.2003 #108339# - direct initialization of section
                    // after insertion in the layout
                    static_cast<SwSectionFrm*>(pFrm)->Init();
                }
                else
                {
                    pFrm->InsertBehind( pLay, pPrv );
                    // OD 25.03.2003 #108339# - direct initialization of section
                    // after insertion in the layout
                    static_cast<SwSectionFrm*>(pFrm)->Init();

                    // --> FME 2004-09-08 #i33963#
                    // Do not trust the IsInFtn flag. If we are currently
                    // building up a table, the upper of pPrv may be a cell
                    // frame, but the cell frame does not have an upper yet.
                    if( pPrv && 0 != pPrv->ImplFindFtnFrm() )
                    // <--
                    {
                        if( pPrv->IsSctFrm() )
                            pPrv = ((SwSectionFrm*)pPrv)->ContainsCntnt();
                        if( pPrv && pPrv->IsTxtFrm() )
                            ((SwTxtFrm*)pPrv)->Prepare( PREP_QUOVADIS, 0, FALSE );
                    }
                }
                // --> OD 2005-12-01 #i27138#
                // notify accessibility paragraphs objects about changed
                // CONTENT_FLOWS_FROM/_TO relation.
                // Relation CONTENT_FLOWS_FROM for next paragraph will change
                // and relation CONTENT_FLOWS_TO for previous paragraph will change.
                {
                    ViewShell* pViewShell( pFrm->GetShell() );
                    // no notification, if <ViewShell> is in construction
                    if ( pViewShell && !pViewShell->IsInConstructor() &&
                         pViewShell->GetLayout() &&
                         pViewShell->GetLayout()->IsAnyShellAccessible() )
                    {
                        pViewShell->InvalidateAccessibleParaFlowRelation(
                            dynamic_cast<SwTxtFrm*>(pFrm->FindNextCnt( true )),
                            dynamic_cast<SwTxtFrm*>(pFrm->FindPrevCnt( true )) );
                    }
                }
                // <--
                pFrm->CheckDirChange();

                // OD 12.08.2003 #i17969# - consider horizontal/vertical layout
                // for setting position at newly inserted frame
                lcl_SetPos( *pFrm, *pLay );

                // OD 20.11.2002 #105405# - no page, no invalidate.
                if ( pPage )
                {
                    // OD 18.09.2002 #100522#
                    // invalidate page in order to force format and paint of
                    // inserted section frame
                    pFrm->InvalidatePage( pPage );

                    // FME 10.11.2003 #112243#
                    // Invalidate fly content flag:
                    if ( pFrm->IsInFly() )
                        pPage->InvalidateFlyCntnt();

                    // OD 14.11.2002 #104684# - invalidate page content in order to
                    // force format and paint of section content.
                    pPage->InvalidateCntnt();
                }

                pLay = (SwLayoutFrm*)pFrm;
                if ( pLay->Lower() && pLay->Lower()->IsLayoutFrm() )
                    pLay = pLay->GetNextLayoutLeaf();
                pPrv = 0;
            }
        }
        else if ( pNd->IsEndNode() && pNd->StartOfSectionNode()->IsSectionNode() )
        {
            ASSERT( pActualSection, "Sectionende ohne Anfang?" );
            ASSERT( pActualSection->GetSectionNode() == pNd->StartOfSectionNode(),
                            "Sectionende mit falschen Start Node?" );

            //Section schliessen, ggf. die umgebende Section wieder
            //aktivieren.
            SwActualSection *pTmp = pActualSection->GetUpper();
            delete pActualSection;
            pLay = pLay->FindSctFrm();
            if ( 0 != (pActualSection = pTmp) )
            {
                //Koennte noch sein, das der letzte SectionFrm leer geblieben
                //ist. Dann ist es jetzt an der Zeit ihn zu entfernen.
                if ( !pLay->ContainsCntnt() )
                {
                    SwFrm *pTmpFrm = pLay;
                    pLay = pTmpFrm->GetUpper();
                    pPrv = pTmpFrm->GetPrev();
                    pTmpFrm->Remove();
                    delete pTmpFrm;
                }
                else
                {
                    pPrv = pLay;
                    pLay = pLay->GetUpper();
                }

                // new section frame
                pFrm = pActualSection->GetSectionNode()->MakeFrm();
                pFrm->InsertBehind( pLay, pPrv );
                static_cast<SwSectionFrm*>(pFrm)->Init();

                // OD 12.08.2003 #i17969# - consider horizontal/vertical layout
                // for setting position at newly inserted frame
                lcl_SetPos( *pFrm, *pLay );

                SwSectionFrm* pOuterSectionFrm = pActualSection->GetSectionFrm();

                // a follow has to be appended to the new section frame
                SwSectionFrm* pFollow = pOuterSectionFrm->GetFollow();
                if ( pFollow )
                {
                    pOuterSectionFrm->SetFollow( NULL );
                    pOuterSectionFrm->InvalidateSize();
                    ((SwSectionFrm*)pFrm)->SetFollow( pFollow );
                }

                // Wir wollen keine leeren Teile zuruecklassen
                if( ! pOuterSectionFrm->IsColLocked() &&
                    ! pOuterSectionFrm->ContainsCntnt() )
                {
                    pOuterSectionFrm->DelEmpty( TRUE );
                    delete pOuterSectionFrm;
                }
                pActualSection->SetSectionFrm( (SwSectionFrm*)pFrm );

                pLay = (SwLayoutFrm*)pFrm;
                if ( pLay->Lower() && pLay->Lower()->IsLayoutFrm() )
                    pLay = pLay->GetNextLayoutLeaf();
                pPrv = 0;
            }
            else
            {
                //Nix mehr mit Sections, es geht direkt hinter dem SectionFrame
                //weiter.
                pPrv = pLay;
                pLay = pLay->GetUpper();
            }
        }
        else if( pNd->IsStartNode() &&
                 SwFlyStartNode	== ((SwStartNode*)pNd)->GetStartNodeType() )
        {
            if ( pTbl->Count() && bObjsDirect && !bDontCreateObjects )
            {
                SwFlyFrm* pFly = pLay->FindFlyFrm();
                if( pFly )
                    AppendObjs( pTbl, nIndex, pFly, pPage );
            }
        }
        else
            // Weder Cntnt noch Tabelle noch Section,
            // also muessen wir fertig sein.
            break;

        ++nIndex;
        // Der Endnode wird nicht mehr mitgenommen, es muss vom
        // Aufrufenden (Section/MakeFrms()) sichergestellt sein, dass das Ende
        // des Bereichs vor dem EndIndex liegt!
        if ( nEndIndex && nIndex >= nEndIndex )
            break;
    }

    if ( pActualSection )
    {
        //Kann passieren, dass noch eine leere (Follow-)Section uebrig geblieben ist.
        if ( !(pLay = pActualSection->GetSectionFrm())->ContainsCntnt() )
        {
            pLay->Remove();
            delete pLay;
        }
        delete pActualSection;
    }

    if ( bPages )		//Jetzt noch die Flys verbinden lassen.
    {
        if ( !bDontCreateObjects )
            AppendAllObjs( pTbl );
        bObjsDirect = TRUE;
    }

    if( pPageMaker )
    {
        pPageMaker->CheckFlyCache( pPage );
        delete pPageMaker;
        if( pDoc->GetLayoutCache() )
        {
#ifndef PRODUCT
#if OSL_DEBUG_LEVEL > 1
            pDoc->GetLayoutCache()->CompareLayout( *pDoc );
#endif
#endif
            pDoc->GetLayoutCache()->ClearImpl();
        }
    }

    pDoc->UnblockIdling();
    if(pLayout)
        pLayout->SetCallbackActionEnabled( bOldCallbackActionEnabled );
}


void MakeFrms( SwDoc *pDoc, const SwNodeIndex &rSttIdx,
               const SwNodeIndex &rEndIdx )
{
    bObjsDirect = FALSE;

    SwNodeIndex aTmp( rSttIdx );
    ULONG nEndIdx = rEndIdx.GetIndex();
    SwNode* pNd = pDoc->GetNodes().FindPrvNxtFrmNode( aTmp,
                                            pDoc->GetNodes()[ nEndIdx-1 ]);
    if ( pNd )
    {
        BOOL bApres = aTmp < rSttIdx;
        SwNode2Layout aNode2Layout( *pNd, rSttIdx.GetIndex() );
        SwFrm* pFrm;
        while( 0 != (pFrm = aNode2Layout.NextFrm()) )
        {
            SwLayoutFrm *pUpper = pFrm->GetUpper();
            SwFtnFrm* pFtnFrm = pUpper->FindFtnFrm();
            BOOL bOldLock, bOldFtn;
            if( pFtnFrm )
            {
                bOldFtn = pFtnFrm->IsColLocked();
                pFtnFrm->ColLock();
            }
            else
                bOldFtn = TRUE;
            SwSectionFrm* pSct = pUpper->FindSctFrm();
            // Es sind innerhalb von Fussnoten nur die Bereiche interessant,
            // die in den Fussnoten liegen, nicht etwa die (spaltigen) Bereiche,
            // in denen die Fussnoten(Container) liegen.
            // #109767# Table frame is in section, insert section in cell frame.
            if( pSct && ( pFtnFrm && !pSct->IsInFtn() ) || pUpper->IsCellFrm() )
                pSct = NULL;
            if( pSct )
            {   // damit der SectionFrm nicht zerstoert wird durch pTmp->MoveFwd()
                bOldLock = pSct->IsColLocked();
                pSct->ColLock();
            }
            else
                bOldLock = TRUE;

            // Wenn pFrm sich nicht bewegen kann, koennen wir auch niemanden
            // auf die naechste Seite schieben. Innerhalb eines Rahmens auch
            // nicht ( in der 1. Spalte eines Rahmens waere pFrm Moveable()! )
            // Auch in spaltigen Bereichen in Tabellen waere pFrm Moveable.
            BOOL bMoveNext = nEndIdx - rSttIdx.GetIndex() > 120;
            BOOL bAllowMove = !pFrm->IsInFly() && pFrm->IsMoveable() &&
                 (!pFrm->IsInTab() || pFrm->IsTabFrm() );
            if ( bMoveNext && bAllowMove )
            {
                SwFrm *pMove = pFrm;
                SwFrm *pPrev = pFrm->GetPrev();
                SwFlowFrm *pTmp = SwFlowFrm::CastFlowFrm( pMove );
                ASSERT( pTmp, "Missing FlowFrm" );

                if ( bApres )
                {
                    // Wir wollen, dass der Rest der Seite leer ist, d.h.
                    // der naechste muss auf die naechste Seite wandern.
                    // Dieser kann auch in der naechsten Spalte stehen!
                    ASSERT( !pTmp->HasFollow(), "Follows forbidden" );
                    pPrev = pFrm;
                    // Wenn unser umgebender SectionFrm einen Next besitzt,
                    // so soll dieser ebenfalls gemoved werden!
                    pMove = pFrm->GetIndNext();
                    SwColumnFrm* pCol = (SwColumnFrm*)pFrm->FindColFrm();
                    if( pCol )
                        pCol = (SwColumnFrm*)pCol->GetNext();
                    do
                    {
                        if( pCol && !pMove )
                        {   // Bisher haben wir keinen Nachfolger gefunden
                            // jetzt gucken wir in die naechste Spalte
                            pMove = pCol->ContainsAny();
                            if( pCol->GetNext() )
                                pCol = (SwColumnFrm*)pCol->GetNext();
                            else if( pCol->IsInSct() )
                            {   // Wenn es keine naechste Spalte gibt, wir aber
                                // innerhalb eines spaltigen Bereichs sind,
                                // koennte es noch ausserhalb des Bereich
                                // (Seiten-)Spalten geben
                                pCol = (SwColumnFrm*)pCol->FindSctFrm()->FindColFrm();
                                if( pCol )
                                    pCol = (SwColumnFrm*)pCol->GetNext();
                            }
                            else
                                pCol = NULL;
                        }
                        // Falls hier verschrottete SectionFrms herumgammeln,
                        // muessen diese uebersprungen werden.
                        while( pMove && pMove->IsSctFrm() &&
                               !((SwSectionFrm*)pMove)->GetSection() )
                            pMove = pMove->GetNext();
                    } while( !pMove && pCol );

                    if( pMove )
                    {
                        if ( pMove->IsCntntFrm() )
                            pTmp = (SwCntntFrm*)pMove;
                        else if ( pMove->IsTabFrm() )
                            pTmp = (SwTabFrm*)pMove;
                        else if ( pMove->IsSctFrm() )
                        {
                            pMove = ((SwSectionFrm*)pMove)->ContainsAny();
                            if( pMove )
                                pTmp = SwFlowFrm::CastFlowFrm( pMove );
                            else
                                pTmp = NULL;
                        }
                    }
                    else
                        pTmp = 0;
                }
                else
                {
                    ASSERT( !pTmp->IsFollow(), "Follows really forbidden" );
                    // Bei Bereichen muss natuerlich der Inhalt auf die Reise
                    // geschickt werden.
                    if( pMove->IsSctFrm() )
                    {
                        while( pMove && pMove->IsSctFrm() &&
                               !((SwSectionFrm*)pMove)->GetSection() )
                            pMove = pMove->GetNext();
                        if( pMove && pMove->IsSctFrm() )
                            pMove = ((SwSectionFrm*)pMove)->ContainsAny();
                        if( pMove )
                            pTmp = SwFlowFrm::CastFlowFrm( pMove );
                        else
                            pTmp = NULL;
                    }
                }

                if( pTmp )
                {
                    SwFrm* pOldUp = pTmp->GetFrm()->GetUpper();
                    // MoveFwd==TRUE bedeutet, dass wir auf der gleichen
                    // Seite geblieben sind, wir wollen aber die Seite wechseln,
                    // sofern dies moeglich ist
                    BOOL bTmpOldLock = pTmp->IsJoinLocked();
                    pTmp->LockJoin();
                    while( pTmp->MoveFwd( TRUE, FALSE, TRUE ) )
                    {
                        if( pOldUp == pTmp->GetFrm()->GetUpper() )
                            break;
                        pOldUp = pTmp->GetFrm()->GetUpper();
                    }
                    if( !bTmpOldLock )
                        pTmp->UnlockJoin();
                }
                ::_InsertCnt( pUpper, pDoc, rSttIdx.GetIndex(),
                              pFrm->IsInDocBody(), nEndIdx, pPrev );
            }
            else
            {
                BOOL bSplit;
                SwFrm* pPrv = bApres ? pFrm : pFrm->GetPrev();
                // Wenn in einen SectionFrm ein anderer eingefuegt wird,
                // muss dieser aufgebrochen werden
                if( pSct && rSttIdx.GetNode().IsSectionNode() )
                {
                    bSplit = pSct->SplitSect( pFrm, bApres );
                    // Wenn pSct nicht aufgespalten werden konnte
                    if( !bSplit && !bApres )
                    {
                        pUpper = pSct->GetUpper();
                        pPrv = pSct->GetPrev();
                    }
                }
                else
                    bSplit = FALSE;
                ::_InsertCnt( pUpper, pDoc, rSttIdx.GetIndex(), FALSE,
                              nEndIdx, pPrv );
                // OD 23.06.2003 #108784# - correction: append objects doesn't
                // depend on value of <bAllowMove>
                if( !bDontCreateObjects )
                {
                    const SwSpzFrmFmts *pTbl = pDoc->GetSpzFrmFmts();
                    if( pTbl->Count() )
                        AppendAllObjs( pTbl );
                }

                // Wenn nichts eingefuegt wurde, z.B. ein ausgeblendeter Bereich,
                // muss das Splitten rueckgaengig gemacht werden
                if( bSplit && pSct && pSct->GetNext()
                    && pSct->GetNext()->IsSctFrm() )
                    pSct->MergeNext( (SwSectionFrm*)pSct->GetNext() );
                if( pFrm->IsInFly() )
                    pFrm->FindFlyFrm()->_Invalidate();
                if( pFrm->IsInTab() )
                    pFrm->InvalidateSize();
            }

            SwPageFrm *pPage = pUpper->FindPageFrm();
            SwFrm::CheckPageDescs( pPage, FALSE );
            if( !bOldFtn )
                pFtnFrm->ColUnlock();
            if( !bOldLock )
            {
                pSct->ColUnlock();
                // Zum Beispiel beim Einfuegen von gelinkten Bereichen,
                // die wiederum Bereiche enthalten, kann pSct jetzt leer sein
                // und damit ruhig zerstoert werden.
                if( !pSct->ContainsCntnt() )
                {
                    pSct->DelEmpty( TRUE );
                    pDoc->GetRootFrm()->RemoveFromList( pSct );
                    delete pSct;
                }
            }
        }
    }

    bObjsDirect = TRUE;
}


/*************************************************************************
|*
|*	SwBorderAttrs::Ctor, DTor
|*
|*	Ersterstellung		MA 19. May. 93
|*	Letzte Aenderung	MA 25. Jan. 97
|*
|*************************************************************************/

SwBorderAttrs::SwBorderAttrs( const SwModify *pMod, const SwFrm *pConstructor ) :
    SwCacheObj( pMod ),
    rAttrSet( pConstructor->IsCntntFrm()
                    ? ((SwCntntFrm*)pConstructor)->GetNode()->GetSwAttrSet()
                    : ((SwLayoutFrm*)pConstructor)->GetFmt()->GetAttrSet() ),
    rUL 	( rAttrSet.GetULSpace() ),
    rLR 	( rAttrSet.GetLRSpace() ),
    rBox	( rAttrSet.GetBox() 	),
    rShadow ( rAttrSet.GetShadow()	),
    aFrmSize( rAttrSet.GetFrmSize().GetSize() )
{
    //Achtung: Die USHORTs fuer die gecache'ten Werte werden absichtlich
    //nicht initialisiert!

    //Muessen alle einmal berechnet werden:
    bTopLine = bBottomLine = bLeftLine = bRightLine =
    bTop     = bBottom     = bLine   = TRUE;

    bCacheGetLine = bCachedGetTopLine = bCachedGetBottomLine = FALSE;
    // OD 21.05.2003 #108789# - init cache status for values <bJoinedWithPrev>
    // and <bJoinedWithNext>, which aren't initialized by default.
    bCachedJoinedWithPrev = FALSE;
    bCachedJoinedWithNext = FALSE;

    bBorderDist = 0 != (pConstructor->GetType() & (FRM_CELL));
}

SwBorderAttrs::~SwBorderAttrs()
{
    ((SwModify*)pOwner)->SetInCache( FALSE );
}

/*************************************************************************
|*
|*	SwBorderAttrs::CalcTop(), CalcBottom(), CalcLeft(), CalcRight()
|*
|*	Beschreibung		Die Calc-Methoden errechnen zusaetzlich zu den
|*		von den Attributen vorgegebenen Groessen einen Sicherheitsabstand.
|*		der Sicherheitsabstand wird nur einkalkuliert, wenn Umrandung und/oder
|*		Schatten im Spiel sind; er soll vermeiden, dass aufgrund der
|*		groben physikalischen Gegebenheiten Raender usw. uebermalt werden.
|*	Ersterstellung		MA 19. May. 93
|*	Letzte Aenderung	MA 08. Jul. 93
|*
|*************************************************************************/

void SwBorderAttrs::_CalcTop()
{
    nTop = CalcTopLine() + rUL.GetUpper();
    bTop = FALSE;
}

void SwBorderAttrs::_CalcBottom()
{
    nBottom = CalcBottomLine() + rUL.GetLower();
    bBottom = FALSE;
}

long SwBorderAttrs::CalcRight( const SwFrm* pCaller ) const
{
    long nRight;

    // OD 23.01.2003 #106895# - for cell frame in R2L text direction the left
    // and right border are painted on the right respectively left.
    if ( pCaller->IsCellFrm() && pCaller->IsRightToLeft() )
        nRight = CalcLeftLine();
    else
        nRight = CalcRightLine();

    // for paragraphs, "left" is "before text" and "right" is "after text"
    if ( pCaller->IsTxtFrm() && pCaller->IsRightToLeft() )
        nRight += rLR.GetLeft();
    else
        nRight += rLR.GetRight();

    // --> OD 2008-01-21 #newlistlevelattrs#
    // correction: retrieve left margin for numbering in R2L-layout
    if ( pCaller->IsTxtFrm() && pCaller->IsRightToLeft() )
    {
        nRight += ((SwTxtFrm*)pCaller)->GetTxtNode()->GetLeftMarginWithNum();
    }
    // <--

    return nRight;
}

long SwBorderAttrs::CalcLeft( const SwFrm *pCaller ) const
{
    long nLeft;

    // OD 23.01.2003 #106895# - for cell frame in R2L text direction the left
    // and right border are painted on the right respectively left.
    if ( pCaller->IsCellFrm() && pCaller->IsRightToLeft() )
        nLeft = CalcRightLine();
    else
        nLeft = CalcLeftLine();

    // for paragraphs, "left" is "before text" and "right" is "after text"
    if ( pCaller->IsTxtFrm() && pCaller->IsRightToLeft() )
        nLeft += rLR.GetRight();
    else
        nLeft += rLR.GetLeft();

    // --> OD 2008-01-21 #newlistlevelattrs#
    // correction: do not retrieve left margin for numbering in R2L-layout
//    if ( pCaller->IsTxtFrm() )
    if ( pCaller->IsTxtFrm() && !pCaller->IsRightToLeft() )
    // <--
    {
        nLeft += ((SwTxtFrm*)pCaller)->GetTxtNode()->GetLeftMarginWithNum();
    }

    return nLeft;
}

/*************************************************************************
|*
|*	SwBorderAttrs::CalcTopLine(), CalcBottomLine(),
|*				   CalcLeftLine(), CalcRightLine()
|*
|*	Beschreibung		Berechnung der Groessen fuer Umrandung und Schatten.
|* 						Es kann auch ohne Linien ein Abstand erwuenscht sein,
|* 						dieser wird  dann nicht vom Attribut sondern hier
|* 						beruecksichtigt (bBorderDist, z.B. fuer Zellen).
|*	Ersterstellung		MA 21. May. 93
|*	Letzte Aenderung	MA 07. Jun. 99
|*
|*************************************************************************/

void SwBorderAttrs::_CalcTopLine()
{
    nTopLine = (bBorderDist && !rBox.GetTop())
                            ? rBox.GetDistance  (BOX_LINE_TOP)
                            : rBox.CalcLineSpace(BOX_LINE_TOP);
    nTopLine = nTopLine + rShadow.CalcShadowSpace(SHADOW_TOP);
    bTopLine = FALSE;
}

void SwBorderAttrs::_CalcBottomLine()
{
    nBottomLine = (bBorderDist && !rBox.GetBottom())
                            ? rBox.GetDistance  (BOX_LINE_BOTTOM)
                            : rBox.CalcLineSpace(BOX_LINE_BOTTOM);
    nBottomLine = nBottomLine + rShadow.CalcShadowSpace(SHADOW_BOTTOM);
    bBottomLine = FALSE;
}

void SwBorderAttrs::_CalcLeftLine()
{
    nLeftLine = (bBorderDist && !rBox.GetLeft())
                            ? rBox.GetDistance  (BOX_LINE_LEFT)
                            : rBox.CalcLineSpace(BOX_LINE_LEFT);
    nLeftLine = nLeftLine + rShadow.CalcShadowSpace(SHADOW_LEFT);
    bLeftLine = FALSE;
}

void SwBorderAttrs::_CalcRightLine()
{
    nRightLine = (bBorderDist && !rBox.GetRight())
                            ? rBox.GetDistance  (BOX_LINE_RIGHT)
                            : rBox.CalcLineSpace(BOX_LINE_RIGHT);
    nRightLine = nRightLine + rShadow.CalcShadowSpace(SHADOW_RIGHT);
    bRightLine = FALSE;
}

/*************************************************************************
|*
|*	SwBorderAttrs::_IsLine()
|*
|*	Ersterstellung		MA 29. Sep. 94
|*	Letzte Aenderung	MA 29. Sep. 94
|*
|*************************************************************************/

void SwBorderAttrs::_IsLine()
{
    bIsLine = rBox.GetTop() || rBox.GetBottom() ||
              rBox.GetLeft()|| rBox.GetRight();
    bLine = FALSE;
}

/*************************************************************************
|*
|*	SwBorderAttrs::CmpLeftRightLine(), IsTopLine(), IsBottomLine()
|*
|*		Die Umrandungen benachbarter Absaetze werden nach folgendem
|*		Algorithmus zusammengefasst:
|*
|*		1. Die Umrandung oben faellt weg, wenn der Vorgaenger dieselbe
|*		   Umrandung oben aufweist und 3. Zutrifft.
|*		   Zusaetzlich muss der Absatz mindestens rechts oder links oder
|*		   unten eine Umrandung haben.
|*		2. Die Umrandung unten faellt weg, wenn der Nachfolger dieselbe
|*		   Umrandung untern aufweist und 3. Zustrifft.
|*		   Zusaetzlich muss der Absatz mindestens rechts oder links oder
|*		   oben eine Umrandung haben.
|*		3. Die Umrandungen links und rechts vor Vorgaenger bzw. Nachfolger
|*		   sind identisch.
|*
|*	Ersterstellung		MA 22. Mar. 95
|*	Letzte Aenderung	MA 22. May. 95
|*
|*************************************************************************/
inline int CmpLines( const SvxBorderLine *pL1, const SvxBorderLine *pL2 )
{
    return ( ((pL1 && pL2) && (*pL1 == *pL2)) || (!pL1 && !pL2) );
}

// OD 21.05.2003 #108789# - change name of 1st parameter - "rAttrs" -> "rCmpAttrs"
// OD 21.05.2003 #108789# - compare <CalcRight()> and <rCmpAttrs.CalcRight()>
//          instead of only the right LR-spacing, because R2L-layout has to be
//          considered.
BOOL SwBorderAttrs::CmpLeftRight( const SwBorderAttrs &rCmpAttrs,
                                  const SwFrm *pCaller,
                                  const SwFrm *pCmp ) const
{
    return ( CmpLines( rCmpAttrs.GetBox().GetLeft(), GetBox().GetLeft()  ) &&
             CmpLines( rCmpAttrs.GetBox().GetRight(),GetBox().GetRight() ) &&
             CalcLeft( pCaller ) == rCmpAttrs.CalcLeft( pCmp ) &&
             // OD 21.05.2003 #108789# - compare <CalcRight> with <rCmpAttrs.CalcRight>.
             CalcRight( pCaller ) == rCmpAttrs.CalcRight( pCmp ) );
}

BOOL SwBorderAttrs::_JoinWithCmp( const SwFrm& _rCallerFrm,
                                  const SwFrm& _rCmpFrm ) const
{
    BOOL bReturnVal = FALSE;

    SwBorderAttrAccess aCmpAccess( SwFrm::GetCache(), &_rCmpFrm );
    const SwBorderAttrs &rCmpAttrs = *aCmpAccess.Get();
    if ( rShadow == rCmpAttrs.GetShadow() &&
         CmpLines( rBox.GetTop(), rCmpAttrs.GetBox().GetTop() ) &&
         CmpLines( rBox.GetBottom(), rCmpAttrs.GetBox().GetBottom() ) &&
         CmpLeftRight( rCmpAttrs, &_rCallerFrm, &_rCmpFrm )
       )
    {
        bReturnVal = TRUE;
    }

    return bReturnVal;
}

// OD 21.05.2003 #108789# - method to determine, if borders are joined with
// previous frame. Calculated value saved in cached value <bJoinedWithPrev>
// OD 2004-02-26 #i25029# - add 2nd parameter <_pPrevFrm>
void SwBorderAttrs::_CalcJoinedWithPrev( const SwFrm& _rFrm,
                                         const SwFrm* _pPrevFrm )
{
    // set default
    bJoinedWithPrev = FALSE;

    if ( _rFrm.IsTxtFrm() )
    {
        // text frame can potentially join with previous text frame, if
        // corresponding attribute set is set at previous text frame.
        // OD 2004-02-26 #i25029# - If parameter <_pPrevFrm> is set, take this
        // one as previous frame.
        const SwFrm* pPrevFrm = _pPrevFrm ? _pPrevFrm : _rFrm.GetPrev();
        // OD 2004-02-13 #i25029# - skip hidden text frames.
        while ( pPrevFrm && pPrevFrm->IsTxtFrm() &&
                static_cast<const SwTxtFrm*>(pPrevFrm)->IsHiddenNow() )
        {
            pPrevFrm = pPrevFrm->GetPrev();
        }
        if ( pPrevFrm && pPrevFrm->IsTxtFrm() &&
             pPrevFrm->GetAttrSet()->GetParaConnectBorder().GetValue()
           )
        {
            bJoinedWithPrev = _JoinWithCmp( _rFrm, *(pPrevFrm) );
        }
    }

    // valid cache status, if demanded
    // OD 2004-02-26 #i25029# - Do not validate cache, if parameter <_pPrevFrm>
    // is set.
    bCachedJoinedWithPrev = bCacheGetLine && !_pPrevFrm;
}

// OD 21.05.2003 #108789# - method to determine, if borders are joined with
// next frame. Calculated value saved in cached value <bJoinedWithNext>
void SwBorderAttrs::_CalcJoinedWithNext( const SwFrm& _rFrm )
{
    // set default
    bJoinedWithNext = FALSE;

    if ( _rFrm.IsTxtFrm() )
    {
        // text frame can potentially join with next text frame, if
        // corresponding attribute set is set at current text frame.
        // OD 2004-02-13 #i25029# - get next frame, but skip hidden text frames.
        const SwFrm* pNextFrm = _rFrm.GetNext();
        while ( pNextFrm && pNextFrm->IsTxtFrm() &&
                static_cast<const SwTxtFrm*>(pNextFrm)->IsHiddenNow() )
        {
            pNextFrm = pNextFrm->GetNext();
        }
        if ( pNextFrm && pNextFrm->IsTxtFrm() &&
             _rFrm.GetAttrSet()->GetParaConnectBorder().GetValue()
           )
        {
            bJoinedWithNext = _JoinWithCmp( _rFrm, *(pNextFrm) );
        }
    }

    // valid cache status, if demanded
    bCachedJoinedWithNext = bCacheGetLine;
}

// OD 21.05.2003 #108789# - accessor for cached values <bJoinedWithPrev>
// OD 2004-02-26 #i25029# - add 2nd parameter <_pPrevFrm>, which is passed to
// method <_CalcJoindWithPrev(..)>.
BOOL SwBorderAttrs::JoinedWithPrev( const SwFrm& _rFrm,
                                    const SwFrm* _pPrevFrm ) const
{
    if ( !bCachedJoinedWithPrev || _pPrevFrm )
    {
        // OD 2004-02-26 #i25029# - pass <_pPrevFrm> as 2nd parameter
        const_cast<SwBorderAttrs*>(this)->_CalcJoinedWithPrev( _rFrm, _pPrevFrm );
    }

    return bJoinedWithPrev;
}

BOOL SwBorderAttrs::JoinedWithNext( const SwFrm& _rFrm ) const
{
    if ( !bCachedJoinedWithNext )
    {
        const_cast<SwBorderAttrs*>(this)->_CalcJoinedWithNext( _rFrm );
    }

    return bJoinedWithNext;
}

// OD 2004-02-26 #i25029# - added 2nd parameter <_pPrevFrm>, which is passed to
// method <JoinedWithPrev>
void SwBorderAttrs::_GetTopLine( const SwFrm& _rFrm,
                                 const SwFrm* _pPrevFrm )
{
    USHORT nRet = CalcTopLine();

    // OD 21.05.2003 #108789# - use new method <JoinWithPrev()>
    // OD 2004-02-26 #i25029# - add 2nd parameter
    if ( JoinedWithPrev( _rFrm, _pPrevFrm ) )
    {
        nRet = 0;
    }

    bCachedGetTopLine = bCacheGetLine;

    nGetTopLine = nRet;
}

void SwBorderAttrs::_GetBottomLine( const SwFrm& _rFrm )
{
    USHORT nRet = CalcBottomLine();

    // OD 21.05.2003 #108789# - use new method <JoinWithPrev()>
    if ( JoinedWithNext( _rFrm ) )
    {
        nRet = 0;
    }

    bCachedGetBottomLine = bCacheGetLine;

    nGetBottomLine = nRet;
}

/*************************************************************************
|*
|*	SwBorderAttrAccess::CTor
|*
|*	Ersterstellung		MA 20. Mar. 95
|*	Letzte Aenderung	MA 29. Nov. 95
|*
|*************************************************************************/

SwBorderAttrAccess::SwBorderAttrAccess( SwCache &rCach, const SwFrm *pFrm ) :
    SwCacheAccess( rCach, (pFrm->IsCntntFrm() ?
                                (void*)((SwCntntFrm*)pFrm)->GetNode() :
                                (void*)((SwLayoutFrm*)pFrm)->GetFmt()),
                           (BOOL)(pFrm->IsCntntFrm() ?
                ((SwModify*)((SwCntntFrm*)pFrm)->GetNode())->IsInCache() :
                ((SwModify*)((SwLayoutFrm*)pFrm)->GetFmt())->IsInCache()) ),
    pConstructor( pFrm )
{
}

/*************************************************************************
|*
|*	SwBorderAttrAccess::NewObj, Get
|*
|*	Ersterstellung		MA 20. Mar. 95
|*	Letzte Aenderung	MA 20. Mar. 95
|*
|*************************************************************************/

SwCacheObj *SwBorderAttrAccess::NewObj()
{
    ((SwModify*)pOwner)->SetInCache( TRUE );
    return new SwBorderAttrs( (SwModify*)pOwner, pConstructor );
}

SwBorderAttrs *SwBorderAttrAccess::Get()
{
    return (SwBorderAttrs*)SwCacheAccess::Get();
}

/*************************************************************************
|*
|*	SwOrderIter::Ctor
|*
|*	Ersterstellung		MA 06. Jan. 95
|*	Letzte Aenderung	MA 22. Nov. 95
|*
|*************************************************************************/

SwOrderIter::SwOrderIter( const SwPageFrm *pPg, BOOL bFlys ) :
    pPage( pPg ),
    pCurrent( 0 ),
    bFlysOnly( bFlys )
{
}

/*************************************************************************
|*
|*	SwOrderIter::Top()
|*
|*	Ersterstellung		MA 06. Jan. 95
|*	Letzte Aenderung	MA 22. Nov. 95
|*
|*************************************************************************/

const SdrObject *SwOrderIter::Top()
{
    pCurrent = 0;
    if ( pPage->GetSortedObjs() )
    {
        UINT32 nTopOrd = 0;
        const SwSortedObjs *pObjs = pPage->GetSortedObjs();
        if ( pObjs->Count() )
        {
            (*pObjs)[0]->GetDrawObj()->GetOrdNum();  //Aktualisieren erzwingen!
            for ( USHORT i = 0; i < pObjs->Count(); ++i )
            {
                const SdrObject* pObj = (*pObjs)[i]->GetDrawObj();
                if ( bFlysOnly && !pObj->ISA(SwVirtFlyDrawObj) )
                    continue;
                UINT32 nTmp = pObj->GetOrdNumDirect();
                if ( nTmp >= nTopOrd )
                {
                    nTopOrd = nTmp;
                    pCurrent = pObj;
                }
            }
        }
    }
    return pCurrent;
}

/*************************************************************************
|*
|*	SwOrderIter::Bottom()
|*
|*	Ersterstellung		MA 06. Jan. 95
|*	Letzte Aenderung	MA 22. Nov. 95
|*
|*************************************************************************/

const SdrObject *SwOrderIter::Bottom()
{
    pCurrent = 0;
    if ( pPage->GetSortedObjs() )
    {
        UINT32 nBotOrd = USHRT_MAX;
        const SwSortedObjs *pObjs = pPage->GetSortedObjs();
        if ( pObjs->Count() )
        {
            (*pObjs)[0]->GetDrawObj()->GetOrdNum();  //Aktualisieren erzwingen!
            for ( USHORT i = 0; i < pObjs->Count(); ++i )
            {
                const SdrObject* pObj = (*pObjs)[i]->GetDrawObj();
                if ( bFlysOnly && !pObj->ISA(SwVirtFlyDrawObj) )
                    continue;
                UINT32 nTmp = pObj->GetOrdNumDirect();
                if ( nTmp < nBotOrd )
                {
                    nBotOrd = nTmp;
                    pCurrent = pObj;
                }
            }
        }
    }
    return pCurrent;
}

/*************************************************************************
|*
|*	SwOrderIter::Next()
|*
|*	Ersterstellung		MA 06. Jan. 95
|*	Letzte Aenderung	MA 22. Nov. 95
|*
|*************************************************************************/

const SdrObject *SwOrderIter::Next()
{
    const UINT32 nCurOrd = pCurrent ? pCurrent->GetOrdNumDirect() : 0;
    pCurrent = 0;
    if ( pPage->GetSortedObjs() )
    {
        UINT32 nOrd = USHRT_MAX;
        const SwSortedObjs *pObjs = pPage->GetSortedObjs();
        if ( pObjs->Count() )
        {
            (*pObjs)[0]->GetDrawObj()->GetOrdNum();  //Aktualisieren erzwingen!
            for ( USHORT i = 0; i < pObjs->Count(); ++i )
            {
                const SdrObject* pObj = (*pObjs)[i]->GetDrawObj();
                if ( bFlysOnly && !pObj->ISA(SwVirtFlyDrawObj) )
                    continue;
                UINT32 nTmp = pObj->GetOrdNumDirect();
                if ( nTmp > nCurOrd && nTmp < nOrd )
                {
                    nOrd = nTmp;
                    pCurrent = pObj;
                }
            }
        }
    }
    return pCurrent;
}

/*************************************************************************
|*
|*	SwOrderIter::Prev()
|*
|*	Ersterstellung		MA 06. Jan. 95
|*	Letzte Aenderung	MA 22. Nov. 95
|*
|*************************************************************************/

const SdrObject *SwOrderIter::Prev()
{
    const UINT32 nCurOrd = pCurrent ? pCurrent->GetOrdNumDirect() : 0;
    pCurrent = 0;
    if ( pPage->GetSortedObjs() )
    {
        UINT32 nOrd = 0;
        const SwSortedObjs *pObjs = pPage->GetSortedObjs();
        if ( pObjs->Count() )
        {
            (*pObjs)[0]->GetDrawObj()->GetOrdNum();  //Aktualisieren erzwingen!
            for ( USHORT i = 0; i < pObjs->Count(); ++i )
            {
                const SdrObject* pObj = (*pObjs)[i]->GetDrawObj();
                if ( bFlysOnly && !pObj->ISA(SwVirtFlyDrawObj) )
                    continue;
                UINT32 nTmp = pObj->GetOrdNumDirect();
                if ( nTmp < nCurOrd && nTmp >= nOrd )
                {
                    nOrd = nTmp;
                    pCurrent = pObj;
                }
            }
        }
    }
    return pCurrent;
}

/*************************************************************************
|*
|*	SaveCntnt(), RestoreCntnt()
|*
|*	Ersterstellung		MA 10. Jun. 93
|*	Letzte Aenderung	MA 07. Mar. 95
|*
|*************************************************************************/

//Unterstruktur eines LayoutFrms fuer eine Aktion aufheben und wieder
//restaurieren.
//Neuer Algorithmus: Es ist unuetz jeden Nachbarn einzeln zu betrachten und
//die Pointer sauber zu setzen (Upper, Nachbarn, usw.)
//Es reicht vollkommen jeweils eine Einzelkette zu loesen, und mit dem
//Letzen der Einzelkette nachzuschauen ob noch eine weitere Kette
//angeheangt werden muss. Es brauchen nur die Pointer korrigiert werden,
//die zur Verkettung notwendig sind. So koennen Beipspielsweise die Pointer
//auf die Upper auf den alten Uppern stehenbleiben. Korrigiert werden die
//Pointer dann im RestoreCntnt. Zwischenzeitlich ist sowieso jeder Zugriff
//verboten.
//Unterwegs werden die Flys bei der Seite abgemeldet.

// --> OD 2004-11-29 #115759# - 'remove' also drawing object from page and
// at-fly anchored objects from page
void MA_FASTCALL lcl_RemoveObjsFromPage( SwFrm* _pFrm )
{
    ASSERT( _pFrm->GetDrawObjs(), "Keine DrawObjs fuer lcl_RemoveFlysFromPage." );
    SwSortedObjs &rObjs = *_pFrm->GetDrawObjs();
    for ( USHORT i = 0; i < rObjs.Count(); ++i )
    {
        SwAnchoredObject* pObj = rObjs[i];
        // --> OD 2004-11-29 #115759# - reset member, at which the anchored
        // object orients its vertical position
        pObj->ClearVertPosOrientFrm();
        // <--
        // --> OD 2005-03-03 #i43913#
        pObj->ResetLayoutProcessBools();
        // <--
        // --> OD 2004-11-29 #115759# - remove also lower objects of as-character
        // anchored Writer fly frames from page
        if ( pObj->ISA(SwFlyFrm) )
        {
            SwFlyFrm* pFlyFrm = static_cast<SwFlyFrm*>(pObj);

            // --> OD 2004-11-29 #115759# - remove also direct lowers of Writer
            // fly frame from page
            if ( pFlyFrm->GetDrawObjs() )
            {
                ::lcl_RemoveObjsFromPage( pFlyFrm );
            }
            // <--

            SwCntntFrm* pCnt = pFlyFrm->ContainsCntnt();
            while ( pCnt )
            {
                if ( pCnt->GetDrawObjs() )
                    ::lcl_RemoveObjsFromPage( pCnt );
                pCnt = pCnt->GetNextCntntFrm();
            }
            if ( pFlyFrm->IsFlyFreeFrm() )
            {
                // --> OD 2004-06-30 #i28701# - use new method <GetPageFrm()>
                pFlyFrm->GetPageFrm()->RemoveFlyFromPage( pFlyFrm );
            }
        }
        // <--
        // --> OD 2004-11-29 #115759# - remove also drawing objects from page
        else if ( pObj->ISA(SwAnchoredDrawObject) )
        {
            if ( pObj->GetFrmFmt().GetAnchor().GetAnchorId() != FLY_IN_CNTNT )
            {
                pObj->GetPageFrm()->RemoveDrawObjFromPage(
                                *(static_cast<SwAnchoredDrawObject*>(pObj)) );
            }
        }
        // <--
    }
}

SwFrm *SaveCntnt( SwLayoutFrm *pLay, SwFrm *pStart )
{
    if( pLay->IsSctFrm() && pLay->Lower() && pLay->Lower()->IsColumnFrm() )
        lcl_RemoveFtns( (SwColumnFrm*)pLay->Lower(), TRUE, TRUE );

    SwFrm *pSav;
    if ( 0 == (pSav = pLay->ContainsAny()) )
        return 0;

    if( pSav->IsInFtn() && !pLay->IsInFtn() )
    {
        do
            pSav = pSav->FindNext();
        while( pSav && pSav->IsInFtn() );
        if( !pSav || !pLay->IsAnLower( pSav ) )
            return NULL;
    }

    // Tables should be saved as a whole, expection:
    // The contents of a section or a cell inside a table should be saved
    if ( pSav->IsInTab() && !( ( pLay->IsSctFrm() || pLay->IsCellFrm() ) && pLay->IsInTab() ) )
        while ( !pSav->IsTabFrm() )
            pSav = pSav->GetUpper();

    if( pSav->IsInSct() )
    { // Jetzt wird der oberste Bereich gesucht, der innerhalb von pLay ist.
        SwFrm* pSect = pLay->FindSctFrm();
        SwFrm *pTmp = pSav;
        do
        {
            pSav = pTmp;
            pTmp = pSav->GetUpper() ? pSav->GetUpper()->FindSctFrm() : NULL;
        } while ( pTmp != pSect );
    }

    SwFrm *pFloat = pSav;
    if( !pStart )
        pStart = pSav;
    BOOL bGo = pStart == pSav;
    do
    {
        if( bGo )
            pFloat->GetUpper()->pLower = 0; 	//Die Teilkette ausklinken.

        //Das Ende der Teilkette suchen, unterwegs die Flys abmelden.
        do
        {
            if( bGo )
            {
                if ( pFloat->IsCntntFrm() )
                {
                    if ( pFloat->GetDrawObjs() )
                        ::lcl_RemoveObjsFromPage( (SwCntntFrm*)pFloat );
                }
                else if ( pFloat->IsTabFrm() || pFloat->IsSctFrm() )
                {
                    SwCntntFrm *pCnt = ((SwLayoutFrm*)pFloat)->ContainsCntnt();
                    if( pCnt )
                    {
                        do
                        {	if ( pCnt->GetDrawObjs() )
                                ::lcl_RemoveObjsFromPage( pCnt );
                            pCnt = pCnt->GetNextCntntFrm();
                        } while ( pCnt && ((SwLayoutFrm*)pFloat)->IsAnLower( pCnt ) );
                    }
                }
                else {
                    ASSERT( !pFloat, "Neuer Float-Frame?" );
                }
            }
            if ( pFloat->GetNext()	)
            {
                if( bGo )
                    pFloat->pUpper = NULL;
                pFloat = pFloat->GetNext();
                if( !bGo && pFloat == pStart )
                {
                    bGo = TRUE;
                    pFloat->pPrev->pNext = NULL;
                    pFloat->pPrev = NULL;
                }
            }
            else
                break;

        } while ( pFloat );

        //Die naechste Teilkette suchen und die Ketten miteinander verbinden.
        SwFrm *pTmp = pFloat->FindNext();
        if( bGo )
            pFloat->pUpper = NULL;

        if( !pLay->IsInFtn() )
            while( pTmp && pTmp->IsInFtn() )
                pTmp = pTmp->FindNext();

        if ( !pLay->IsAnLower( pTmp ) )
            pTmp = 0;

        if ( pTmp && bGo )
        {
            pFloat->pNext = pTmp;			//Die beiden Ketten verbinden.
            pFloat->pNext->pPrev = pFloat;
        }
        pFloat = pTmp;
        bGo = bGo || ( pStart == pFloat );
    }  while ( pFloat );

    return bGo ? pStart : NULL;
}

// --> OD 2004-11-29 #115759# - add also drawing objects to page and at-fly
// anchored objects to page
void MA_FASTCALL lcl_AddObjsToPage( SwFrm* _pFrm, SwPageFrm* _pPage )
{
    ASSERT( _pFrm->GetDrawObjs(), "Keine DrawObjs fuer lcl_AddFlysToPage." );
    SwSortedObjs &rObjs = *_pFrm->GetDrawObjs();
    for ( USHORT i = 0; i < rObjs.Count(); ++i )
    {
        SwAnchoredObject* pObj = rObjs[i];

        // --> OD 2004-11-29 #115759# - unlock position of anchored object
        // in order to get the object's position calculated.
        pObj->UnlockPosition();
        // <--
        // --> OD 2004-11-29 #115759# - add also lower objects of as-character
        // anchored Writer fly frames from page
        if ( pObj->ISA(SwFlyFrm) )
        {
            SwFlyFrm* pFlyFrm = static_cast<SwFlyFrm*>(pObj);
            if ( pObj->ISA(SwFlyFreeFrm) )
            {
                _pPage->AppendFlyToPage( pFlyFrm );
            }
            pFlyFrm->_InvalidatePos();
            pFlyFrm->_InvalidateSize();
            pFlyFrm->InvalidatePage( _pPage );

            // --> OD 2004-11-29 #115759# - add also at-fly anchored objects
            // to page
            if ( pFlyFrm->GetDrawObjs() )
            {
                ::lcl_AddObjsToPage( pFlyFrm, _pPage );
            }
            // <--

            SwCntntFrm *pCnt = pFlyFrm->ContainsCntnt();
            while ( pCnt )
            {
                if ( pCnt->GetDrawObjs() )
                    ::lcl_AddObjsToPage( pCnt, _pPage );
                pCnt = pCnt->GetNextCntntFrm();
            }
        }
        // <--
        // --> OD 2004-11-29 #115759# - remove also drawing objects from page
        else if ( pObj->ISA(SwAnchoredDrawObject) )
        {
            if ( pObj->GetFrmFmt().GetAnchor().GetAnchorId() != FLY_IN_CNTNT )
            {
                pObj->InvalidateObjPos();
                _pPage->AppendDrawObjToPage(
                                *(static_cast<SwAnchoredDrawObject*>(pObj)) );
            }
        }
        // <--
    }
}

void RestoreCntnt( SwFrm *pSav, SwLayoutFrm *pParent, SwFrm *pSibling, bool bGrow )
{
    ASSERT( pSav && pParent, "Kein Save oder Parent fuer Restore." );
    SWRECTFN( pParent )

    //Wenn es bereits FlowFrms unterhalb des neuen Parent gibt, so wird die
    //Kette, beginnend mit pSav,  hinter dem letzten angehaengt.
    //Die Teile werden kurzerhand insertet und geeignet invalidiert.
    //Unterwegs werden die Flys der CntntFrms bei der Seite angemeldet.

    SwPageFrm *pPage = pParent->FindPageFrm();

    if ( pPage )
        pPage->InvalidatePage( pPage ); //Invalides Layout anmelden.

    //Vorgaenger festellen und die Verbindung herstellen bzw. initialisieren.
    pSav->pPrev = pSibling;
    SwFrm* pNxt;
    if ( pSibling )
    {
        pNxt = pSibling->pNext;
        pSibling->pNext = pSav;
        pSibling->_InvalidatePrt();
        ((SwCntntFrm*)pSibling)->InvalidatePage( pPage );//Invaliden Cntnt anmelden.
        if ( ((SwCntntFrm*)pSibling)->GetFollow() )
            pSibling->Prepare( PREP_CLEAR, 0, sal_False );
    }
    else
    {	pNxt = pParent->pLower;
        pParent->pLower = pSav;
        pSav->pUpper = pParent;		//Schon mal setzen, sonst ist fuer das
                                    //invalidate der Parent (z.B. ein Fly) nicht klar.
        //Invaliden Cntnt anmelden.
        if ( pSav->IsCntntFrm() )
            ((SwCntntFrm*)pSav)->InvalidatePage( pPage );
        else
        {   // pSav koennte auch ein leerer SectFrm sein
            SwCntntFrm* pCnt = pParent->ContainsCntnt();
            if( pCnt )
                pCnt->InvalidatePage( pPage );
        }
    }

    //Der Parent muss entsprechend gegrow'ed werden.
    SwTwips nGrowVal = 0;
    SwFrm* pLast;
    do
    {	pSav->pUpper = pParent;
        nGrowVal += (pSav->Frm().*fnRect->fnGetHeight)();
        pSav->_InvalidateAll();

        //Jetzt die Flys anmelden, fuer TxtFrms gleich geeignet invalidieren.
        if ( pSav->IsCntntFrm() )
        {
            if ( pSav->IsTxtFrm() &&
                 ((SwTxtFrm*)pSav)->GetCacheIdx() != USHRT_MAX )
                ((SwTxtFrm*)pSav)->Init();	//Ich bin sein Freund.

            if ( pPage && pSav->GetDrawObjs() )
                ::lcl_AddObjsToPage( (SwCntntFrm*)pSav, pPage );
        }
        else
        {	SwCntntFrm *pBlub = ((SwLayoutFrm*)pSav)->ContainsCntnt();
            if( pBlub )
            {
                do
                {	if ( pPage && pBlub->GetDrawObjs() )
                        ::lcl_AddObjsToPage( pBlub, pPage );
                    if( pBlub->IsTxtFrm() && ((SwTxtFrm*)pBlub)->HasFtn() &&
                         ((SwTxtFrm*)pBlub)->GetCacheIdx() != USHRT_MAX )
                        ((SwTxtFrm*)pBlub)->Init();	//Ich bin sein Freund.
                    pBlub = pBlub->GetNextCntntFrm();
                } while ( pBlub && ((SwLayoutFrm*)pSav)->IsAnLower( pBlub ));
            }
        }
        pLast = pSav;
        pSav = pSav->GetNext();

    } while ( pSav );

    if( pNxt )
    {
        pLast->pNext = pNxt;
        pNxt->pPrev = pLast;
    }

    if ( bGrow )
        pParent->Grow( nGrowVal );
}

/*************************************************************************
|*
|*	SqRt()				Berechnung der Quadratwurzel, damit die math.lib
|*		nicht auch noch dazugelinkt werden muss.
|*
|*	Ersterstellung		OK ??
|*	Letzte Aenderung	MA 09. Jan. 97
|*
|*************************************************************************/

ULONG MA_FASTCALL SqRt( BigInt nX )
{
    BigInt nErg = 1;

    if ( !nX.IsNeg() )
    {
        BigInt nOldErg = 1;
        for ( int i = 0; i <= 5; i++ )
        {
            nErg = (nOldErg + (nX / nOldErg)) / BigInt(2);
            nOldErg = nErg;
        }
    }
    return nErg >= BigInt(SAL_MAX_UINT32) ? ULONG_MAX : (ULONG)nErg;
}

/*************************************************************************
|*
|*	InsertNewPage() 	Einsetzen einer neuen Seite.
|*
|*	Ersterstellung		MA 01. Jul. 93
|*	Letzte Aenderung	MA 31. Jul. 95
|*
|*************************************************************************/

SwPageFrm * MA_FASTCALL InsertNewPage( SwPageDesc &rDesc, SwFrm *pUpper,
                          BOOL bOdd, BOOL bInsertEmpty, BOOL bFtn,
                          SwFrm *pSibling )
{
    SwPageFrm *pRet;
    SwDoc *pDoc = ((SwLayoutFrm*)pUpper)->GetFmt()->GetDoc();
    SwFrmFmt *pFmt = bOdd ? rDesc.GetRightFmt() : rDesc.GetLeftFmt();
    //Wenn ich kein FrmFmt fuer die Seite gefunden habe, muss ich eben
    //eine Leerseite einfuegen.
    if ( !pFmt )
    {
        pFmt = bOdd ? rDesc.GetLeftFmt() : rDesc.GetRightFmt();
        ASSERT( pFmt, "Descriptor without any format?!" );
        bInsertEmpty = !bInsertEmpty;
    }
    if( bInsertEmpty )
    {
        SwPageDesc *pTmpDesc = pSibling && pSibling->GetPrev() ?
                ((SwPageFrm*)pSibling->GetPrev())->GetPageDesc() : &rDesc;
        pRet = new SwPageFrm( pDoc->GetEmptyPageFmt(), pTmpDesc );
        pRet->Paste( pUpper, pSibling );
        pRet->PreparePage( bFtn );
    }
    pRet = new SwPageFrm( pFmt, &rDesc );
    pRet->Paste( pUpper, pSibling );
    pRet->PreparePage( bFtn );
    if ( pRet->GetNext() )
        ((SwRootFrm*)pRet->GetUpper())->AssertPageFlys( pRet );
    return pRet;
}


/*************************************************************************
|*
|*	RegistFlys(), Regist()	Die beiden folgenden Methoden durchsuchen rekursiv
|*		eine Layoutstruktur und melden alle FlyFrms, die einen beliebigen Frm
|*		innerhalb der Struktur als Anker haben bei der Seite an.
|*
|*	Ersterstellung		MA 08. Jul. 93
|*	Letzte Aenderung	MA 07. Jul. 95
|*
|*************************************************************************/

void MA_FASTCALL lcl_Regist( SwPageFrm *pPage, const SwFrm *pAnch )
{
    SwSortedObjs *pObjs = (SwSortedObjs*)pAnch->GetDrawObjs();
    for ( USHORT i = 0; i < pObjs->Count(); ++i )
    {
        SwAnchoredObject* pObj = (*pObjs)[i];
        if ( pObj->ISA(SwFlyFrm) )
        {
            SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pObj);
            //Ggf. ummelden, nicht anmelden wenn bereits bekannt.
            // --> OD 2004-06-30 #i28701# - use new method <GetPageFrm()>
            SwPageFrm *pPg = pFly->IsFlyFreeFrm()
                             ? pFly->GetPageFrm() : pFly->FindPageFrm();
            if ( pPg != pPage )
            {
                if ( pPg )
                    pPg->RemoveFlyFromPage( pFly );
                pPage->AppendFlyToPage( pFly );
            }
            ::RegistFlys( pPage, pFly );
        }
        else
        {
            // --> OD 2004-07-02 #i28701#
            if ( pObj->GetPageFrm() )
                pObj->GetPageFrm()->RemoveDrawObjFromPage( *pObj );
            pPage->AppendDrawObjToPage( *pObj );
            // <--
        }

        const SwFlyFrm* pFly = pAnch->FindFlyFrm();
        if ( pFly &&
             pObj->GetDrawObj()->GetOrdNum() < pFly->GetVirtDrawObj()->GetOrdNum() &&
             pObj->GetDrawObj()->GetPage() )
        {
            pObj->DrawObj()->GetPage()->SetObjectOrdNum(
                                pObj->GetDrawObj()->GetOrdNumDirect(),
                                pFly->GetVirtDrawObj()->GetOrdNumDirect() + 1 );
        }
    }
}

void RegistFlys( SwPageFrm *pPage, const SwLayoutFrm *pLay )
{
    if ( pLay->GetDrawObjs() )
        ::lcl_Regist( pPage, pLay );
    const SwFrm *pFrm = pLay->Lower();
    while ( pFrm )
    {
        if ( pFrm->IsLayoutFrm() )
            ::RegistFlys( pPage, (const SwLayoutFrm*)pFrm );
        else if ( pFrm->GetDrawObjs() )
            ::lcl_Regist( pPage, pFrm );
        pFrm = pFrm->GetNext();
    }
}

/*************************************************************************
|*
|*	void Notify()
|*
|*	Beschreibung		Benachrichtigt den Hintergrund je nach der
|*		Veraenderung zwischen altem und neuem Rechteckt.
|*	Ersterstellung		MA 18. Jun. 93
|*	Letzte Aenderung	MA 06. Jun. 96
|*
|*************************************************************************/

void Notify( SwFlyFrm *pFly, SwPageFrm *pOld, const SwRect &rOld,
             const SwRect* pOldPrt )
{
    const SwRect aFrm( pFly->GetObjRectWithSpaces() );
    if ( rOld.Pos() != aFrm.Pos() )
    {	//Positionsaenderung, alten und neuen Bereich invalidieren
        if ( rOld.HasArea() &&
             rOld.Left()+pFly->GetFmt()->GetLRSpace().GetLeft() < WEIT_WECH )
        {
            pFly->NotifyBackground( pOld, rOld, PREP_FLY_LEAVE );
        }
        pFly->NotifyBackground( pFly->FindPageFrm(), aFrm, PREP_FLY_ARRIVE );
    }
    else if ( rOld.SSize() != aFrm.SSize() )
    {	//Groessenaenderung, den Bereich der Verlassen wurde bzw. jetzt
        //ueberdeckt wird invalidieren.
        //Der Einfachheit halber wird hier bewusst jeweils ein Twip
        //unnoetig invalidiert.

        ViewShell *pSh = pFly->GetShell();
        if( pSh && rOld.HasArea() )
            pSh->InvalidateWindows( rOld );

        // --> OD 2005-08-19 #i51941# - consider case that fly frame isn't
        // registered at the old page <pOld>
        SwPageFrm* pPageFrm = pFly->FindPageFrm();
        if ( pOld != pPageFrm )
        {
            pFly->NotifyBackground( pPageFrm, aFrm, PREP_FLY_ARRIVE );
        }
        // <--

        if ( rOld.Left() != aFrm.Left() )
        {
            SwRect aTmp( rOld );
            aTmp.Union( aFrm );
            aTmp.Left(	Min(aFrm.Left(), rOld.Left()) );
            aTmp.Right( Max(aFrm.Left(), rOld.Left()) );
            pFly->NotifyBackground( pOld, aTmp, PREP_FLY_CHGD );
        }
        SwTwips nOld = rOld.Right();
        SwTwips nNew = aFrm.Right();
        if ( nOld != nNew )
        {
            SwRect aTmp( rOld );
            aTmp.Union( aFrm );
            aTmp.Left(	Min(nNew, nOld) );
            aTmp.Right( Max(nNew, nOld) );
            pFly->NotifyBackground( pOld, aTmp, PREP_FLY_CHGD );
        }
        if ( rOld.Top() != aFrm.Top() )
        {
            SwRect aTmp( rOld );
            aTmp.Union( aFrm );
            aTmp.Top(	 Min(aFrm.Top(), rOld.Top()) );
            aTmp.Bottom( Max(aFrm.Top(), rOld.Top()) );
            pFly->NotifyBackground( pOld, aTmp, PREP_FLY_CHGD );
        }
        nOld = rOld.Bottom();
        nNew = aFrm.Bottom();
        if ( nOld != nNew )
        {
            SwRect aTmp( rOld );
            aTmp.Union( aFrm );
            aTmp.Top(	 Min(nNew, nOld) );
            aTmp.Bottom( Max(nNew, nOld) );
            pFly->NotifyBackground( pOld, aTmp, PREP_FLY_CHGD );
        }
    }
    else if ( pOldPrt && *pOldPrt != pFly->Prt() &&
              pFly->GetFmt()->GetSurround().IsContour() )
    {
        // #i24097#
        pFly->NotifyBackground( pFly->FindPageFrm(), aFrm, PREP_FLY_ARRIVE );
    }
}

/*************************************************************************
|*
|*	NotifyBackground()
|*
|*************************************************************************/

void lcl_CheckFlowBack( SwFrm* pFrm, const SwRect &rRect )
{
    SwTwips nBottom = rRect.Bottom();
    while( pFrm )
    {
        if( pFrm->IsLayoutFrm() )
        {
            if( rRect.IsOver( pFrm->Frm() ) )
                lcl_CheckFlowBack( ((SwLayoutFrm*)pFrm)->Lower(), rRect );
        }
        else if( !pFrm->GetNext() && nBottom > pFrm->Frm().Bottom() )
        {
            if( pFrm->IsCntntFrm() && ((SwCntntFrm*)pFrm)->HasFollow() )
                pFrm->InvalidateSize();
            else
                pFrm->InvalidateNextPos();
        }
        pFrm = pFrm->GetNext();
    }
}

void MA_FASTCALL lcl_NotifyCntnt( const SdrObject *pThis, SwCntntFrm *pCnt,
    const SwRect &rRect, const PrepareHint eHint )
{
    if ( pCnt->IsTxtFrm() )
    {
        SwRect aCntPrt( pCnt->Prt() );
        aCntPrt.Pos() += pCnt->Frm().Pos();
        if ( eHint == PREP_FLY_ATTR_CHG )
        {
            // --> OD 2004-10-20 #i35640# - use given rectangle <rRect> instead
            // of current bound rectangle
            if ( aCntPrt.IsOver( rRect ) )
            // <--
                pCnt->Prepare( PREP_FLY_ATTR_CHG );
        }
        // --> OD 2004-11-01 #i23129# - only invalidate, if the text frame
        // printing area overlaps with the given rectangle.
        else if ( aCntPrt.IsOver( rRect ) )
        // <--
            pCnt->Prepare( eHint, (void*)&aCntPrt._Intersection( rRect ) );
        if ( pCnt->GetDrawObjs() )
        {
            const SwSortedObjs &rObjs = *pCnt->GetDrawObjs();
            for ( USHORT i = 0; i < rObjs.Count(); ++i )
            {
                SwAnchoredObject* pObj = rObjs[i];
                if ( pObj->ISA(SwFlyFrm) )
                {
                    SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pObj);
                    if ( pFly->IsFlyInCntFrm() )
                    {
                        SwCntntFrm *pCntnt = pFly->ContainsCntnt();
                        while ( pCntnt )
                        {
                            ::lcl_NotifyCntnt( pThis, pCntnt, rRect, eHint );
                            pCntnt = pCntnt->GetNextCntntFrm();
                        }
                    }
                }
            }
        }
    }
}

void Notify_Background( const SdrObject* pObj,
                        SwPageFrm* pPage,
                        const SwRect& rRect,
                        const PrepareHint eHint,
                        const BOOL bInva )
{

    //Wenn der Frm gerade erstmalig sinnvoll positioniert wurde, braucht der
    //alte Bereich nicht benachrichtigt werden.
    if ( eHint == PREP_FLY_LEAVE && rRect.Top() == WEIT_WECH )
         return;

    SwLayoutFrm* pArea;
    SwFlyFrm *pFlyFrm = 0;
    SwFrm* pAnchor;
    if( pObj->ISA(SwVirtFlyDrawObj) )
    {
        pFlyFrm = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
        pAnchor = pFlyFrm->AnchorFrm();
    }
    else
    {
        pFlyFrm = NULL;
        pAnchor = const_cast<SwFrm*>(
                    GetUserCall(pObj)->GetAnchoredObj( pObj )->GetAnchorFrm() );
    }
    if( PREP_FLY_LEAVE != eHint && pAnchor->IsInFly() )
        pArea = pAnchor->FindFlyFrm();
    else
        pArea = pPage;
    SwCntntFrm *pCnt = 0;
    if ( pArea )
    {
        if( PREP_FLY_ARRIVE != eHint )
            lcl_CheckFlowBack( pArea, rRect );

        //Es reagieren sowieso nur die auf den Anker folgenden auf den Fly, also
        //brauchen diese nicht abgeklappert werden.
        //Ausnahme sind ist natuerlich das LEAVE, denn der Fly koennte ja von
        //"oben" kommen.
        // Wenn der Anker auf der vorhergehenden Seite liegt, muss ebenfalls
        // die gesamte Seite abgearbeitet werden. (47722)
        // OD 2004-05-13 #i28701# - If the wrapping style has to be considered
        // on the object positioning, the complete area has to be processed,
        // because content frames before the anchor frame also have to consider
        // the object for the text wrapping.
        // --> OD 2004-08-25 #i3317# - The complete area has always been
        // processed.
        {
            pCnt = pArea->ContainsCntnt();
        }
        // <--
    }
    SwFrm *pLastTab = 0;

    while ( pCnt && pArea && pArea->IsAnLower( pCnt ) )
    {
        ::lcl_NotifyCntnt( pObj, pCnt, rRect, eHint );
        if ( pCnt->IsInTab() )
        {
            SwLayoutFrm* pCell = pCnt->GetUpper();
            // --> OD 2005-01-14 #i40606# - use <GetLastBoundRect()>
            // instead of <GetCurrentBoundRect()>, because a recalculation
            // of the bounding rectangle isn't intended here.
            if ( pCell->IsCellFrm() &&
                 ( pCell->Frm().IsOver( pObj->GetLastBoundRect() ) ||
                   pCell->Frm().IsOver( rRect ) ) )
            // <--
            {
                const SwFmtVertOrient &rOri = pCell->GetFmt()->GetVertOrient();
                if ( text::VertOrientation::NONE != rOri.GetVertOrient() )
                    pCell->InvalidatePrt();
            }
            SwTabFrm *pTab = pCnt->FindTabFrm();
            if ( pTab != pLastTab )
            {
                pLastTab = pTab;
                // --> OD 2005-01-14 #i40606# - use <GetLastBoundRect()>
                // instead of <GetCurrentBoundRect()>, because a recalculation
                // of the bounding rectangle isn't intended here.
                if ( pTab->Frm().IsOver( pObj->GetLastBoundRect() ) ||
                     pTab->Frm().IsOver( rRect ) )
                // <--
                {
                    if ( !pFlyFrm || !pFlyFrm->IsLowerOf( pTab ) )
                        pTab->InvalidatePrt();
                }
            }
        }
        pCnt = pCnt->GetNextCntntFrm();
    }
// #108745# Sorry, but this causes nothing but trouble. I remove these lines
// taking the risk that the footer frame will have a wrong height
//  if( pPage->Lower() )
//  {
//      SwFrm* pFrm = pPage->Lower();
//      while( pFrm->GetNext() )
//          pFrm = pFrm->GetNext();
//      if( pFrm->IsFooterFrm() &&
//          ( ( pFrm->Frm().IsOver( pObj->GetBoundRect() ) ||
//              pFrm->Frm().IsOver( rRect ) ) ) )
//           pFrm->InvalidateSize();
//  }
    // --> OD 2007-07-24 #128702# - make code robust
    if ( pPage && pPage->GetSortedObjs() )
    // <--
    {
        pObj->GetOrdNum();
        const SwSortedObjs &rObjs = *pPage->GetSortedObjs();
        for ( USHORT i = 0; i < rObjs.Count(); ++i )
        {
            SwAnchoredObject* pAnchoredObj = rObjs[i];
            if ( pAnchoredObj->ISA(SwFlyFrm) )
            {
                if( pAnchoredObj->GetDrawObj() == pObj )
                    continue;
                SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pAnchoredObj);
                if ( pFly->Frm().Top() == WEIT_WECH )
                    continue;

                if ( !pFlyFrm ||
                        (!pFly->IsLowerOf( pFlyFrm ) &&
                        pFly->GetVirtDrawObj()->GetOrdNumDirect() < pObj->GetOrdNumDirect()))
                {
                    pCnt = pFly->ContainsCntnt();
                    while ( pCnt )
                    {
                        ::lcl_NotifyCntnt( pObj, pCnt, rRect, eHint );
                        pCnt = pCnt->GetNextCntntFrm();
                    }
                }
                if( pFly->IsFlyLayFrm() )
                {
                    if( pFly->Lower() && pFly->Lower()->IsColumnFrm() &&
                        pFly->Frm().Bottom() >= rRect.Top() &&
                        pFly->Frm().Top() <= rRect.Bottom() &&
                        pFly->Frm().Right() >= rRect.Left() &&
                        pFly->Frm().Left() <= rRect.Right() )
                     {
                        pFly->InvalidateSize();
                     }
                }
                //Flys, die ueber mir liegen muessen/mussten evtl.
                //ausweichen, wenn sie eine automatische Ausrichtung haben.
                //das ist unabhaengig von meinem Attribut, weil dies sich
                //gerade geaendert haben kann und eben deshalb
                //umformatiert wurde.
                else if ( pFly->IsFlyAtCntFrm() &&
                        pObj->GetOrdNumDirect() <
                        pFly->GetVirtDrawObj()->GetOrdNumDirect() &&
                        pFlyFrm && !pFly->IsLowerOf( pFlyFrm ) )
                {
                    const SwFmtHoriOrient &rH = pFly->GetFmt()->GetHoriOrient();
                    if ( text::HoriOrientation::NONE != rH.GetHoriOrient()  &&
                            text::HoriOrientation::CENTER != rH.GetHoriOrient()  &&
                            ( !pFly->IsAutoPos() || text::RelOrientation::CHAR != rH.GetRelationOrient() ) &&
                            (pFly->Frm().Bottom() >= rRect.Top() &&
                            pFly->Frm().Top() <= rRect.Bottom()) )
                        pFly->InvalidatePos();
                }
            }
        }
    }
    if ( pFlyFrm && pAnchor->GetUpper() && pAnchor->IsInTab() )//MA_FLY_HEIGHT
        pAnchor->GetUpper()->InvalidateSize();

    // --> OD 2008-01-30 #i82258# - make code robust
    ViewShell* pSh = 0;
    if ( bInva && pPage &&
         0 != (pSh = pPage->GetShell()) )
    {
        pSh->InvalidateWindows( rRect );
    }
    // <--
}

/*************************************************************************
|*
|*	GetVirtualUpper() liefert bei absatzgebundenen Objekten den Upper
|*  des Ankers. Falls es sich dabei um verkettete Rahmen oder
|*	Fussnoten handelt, wird ggf. der "virtuelle" Upper ermittelt.
|*
|*************************************************************************/

const SwFrm* GetVirtualUpper( const SwFrm* pFrm, const Point& rPos )
{
    if( pFrm->IsTxtFrm() )
    {
        pFrm = pFrm->GetUpper();
        if( !pFrm->Frm().IsInside( rPos ) )
        {
            if( pFrm->IsFtnFrm() )
            {
                const SwFtnFrm* pTmp = ((SwFtnFrm*)pFrm)->GetFollow();
                while( pTmp )
                {
                    if( pTmp->Frm().IsInside( rPos ) )
                        return pTmp;
                    pTmp = pTmp->GetFollow();
                }
            }
            else
            {
                SwFlyFrm* pTmp = (SwFlyFrm*)pFrm->FindFlyFrm();
                while( pTmp )
                {
                    if( pTmp->Frm().IsInside( rPos ) )
                        return pTmp;
                    pTmp = pTmp->GetNextLink();
                }
            }
        }
    }
    return pFrm;
}

/*************************************************************************
|*
|*	IsLowerOf()
|*
|*************************************************************************/

BOOL Is_Lower_Of( const SwFrm *pCurrFrm, const SdrObject* pObj )
{
    Point aPos;
    const SwFrm* pFrm;
    if( pObj->ISA(SwVirtFlyDrawObj) )
    {
        const SwFlyFrm* pFly = ( (SwVirtFlyDrawObj*)pObj )->GetFlyFrm();
        pFrm = pFly->GetAnchorFrm();
        aPos = pFly->Frm().Pos();
    }
    else
    {
        pFrm = ( (SwDrawContact*)GetUserCall(pObj) )->GetAnchorFrm();
        aPos = pObj->GetCurrentBoundRect().TopLeft();
    }
    ASSERT( pFrm, "8-( Fly is lost in Space." );
    pFrm = GetVirtualUpper( pFrm, aPos );
    do
    {	if ( pFrm == pCurrFrm )
            return TRUE;
        if( pFrm->IsFlyFrm() )
        {
            aPos = pFrm->Frm().Pos();
            pFrm = GetVirtualUpper( ((const SwFlyFrm*)pFrm)->GetAnchorFrm(), aPos );
        }
        else
            pFrm = pFrm->GetUpper();
    } while ( pFrm );
    return FALSE;
}

const SwFrm *FindKontext( const SwFrm *pFrm, USHORT nAdditionalKontextTyp )
{
    //Liefert die Umgebung des Frm in die kein Fly aus einer anderen
    //Umgebung hineinragen kann.
    const USHORT nTyp = FRM_ROOT | FRM_HEADER   | FRM_FOOTER | FRM_FTNCONT  |
                        FRM_FTN  | FRM_FLY      |
                        FRM_TAB  | FRM_ROW		| FRM_CELL |
                        nAdditionalKontextTyp;
    do
    {	if ( pFrm->GetType() & nTyp )
            break;
        pFrm = pFrm->GetUpper();
    } while( pFrm );
    return pFrm;
}

BOOL IsFrmInSameKontext( const SwFrm *pInnerFrm, const SwFrm *pFrm )
{
    const SwFrm *pKontext = FindKontext( pInnerFrm, 0 );

    const USHORT nTyp = FRM_ROOT | FRM_HEADER   | FRM_FOOTER | FRM_FTNCONT  |
                        FRM_FTN  | FRM_FLY      |
                        FRM_TAB  | FRM_ROW 		| FRM_CELL;
    do
    {	if ( pFrm->GetType() & nTyp )
        {
            if( pFrm == pKontext )
                return TRUE;
            if( pFrm->IsCellFrm() )
                return FALSE;
        }
        if( pFrm->IsFlyFrm() )
        {
            Point aPos( pFrm->Frm().Pos() );
            pFrm = GetVirtualUpper( ((const SwFlyFrm*)pFrm)->GetAnchorFrm(), aPos );
        }
        else
            pFrm = pFrm->GetUpper();
    } while( pFrm );

    return FALSE;
}


//---------------------------------

SwTwips MA_FASTCALL lcl_CalcCellRstHeight( SwLayoutFrm *pCell )
{
    if ( pCell->Lower()->IsCntntFrm() || pCell->Lower()->IsSctFrm() )
    {
        SwFrm *pLow = pCell->Lower();
        long nHeight = 0, nFlyAdd = 0;
        do
        {
            long nLow = pLow->Frm().Height();
            if( pLow->IsTxtFrm() && ((SwTxtFrm*)pLow)->IsUndersized() )
                nLow += ((SwTxtFrm*)pLow)->GetParHeight()-pLow->Prt().Height();
            else if( pLow->IsSctFrm() && ((SwSectionFrm*)pLow)->IsUndersized() )
                nLow += ((SwSectionFrm*)pLow)->Undersize();
            nFlyAdd = Max( 0L, nFlyAdd - nLow );
            nFlyAdd = Max( nFlyAdd, ::CalcHeightWidthFlys( pLow ) );
            nHeight += nLow;
            pLow = pLow->GetNext();
        } while ( pLow );
        if ( nFlyAdd )
            nHeight += nFlyAdd;

        //Der Border will natuerlich auch mitspielen, er kann leider nicht
        //aus PrtArea und Frm errechnet werden, da diese in beliebiger
        //Kombination ungueltig sein koennen.
        SwBorderAttrAccess aAccess( SwFrm::GetCache(), pCell );
        const SwBorderAttrs &rAttrs = *aAccess.Get();
        nHeight += rAttrs.CalcTop() + rAttrs.CalcBottom();

        return pCell->Frm().Height() - nHeight;
    }
    else
    {
        long nRstHeight = 0;
        SwFrm *pLow = pCell->Lower();
        do
        {	nRstHeight += ::CalcRowRstHeight( (SwLayoutFrm*)pLow );
            pLow = pLow->GetNext();

        } while ( pLow );

        return nRstHeight;
    }
}

SwTwips MA_FASTCALL CalcRowRstHeight( SwLayoutFrm *pRow )
{
    SwTwips nRstHeight = LONG_MAX;
    SwLayoutFrm *pLow = (SwLayoutFrm*)pRow->Lower();
    while ( pLow )
    {
        nRstHeight = Min( nRstHeight, ::lcl_CalcCellRstHeight( pLow ) );
        pLow = (SwLayoutFrm*)pLow->GetNext();
    }
    return nRstHeight;
}

const SwFrm* MA_FASTCALL FindPage( const SwRect &rRect, const SwFrm *pPage )
{
    if ( !rRect.IsOver( pPage->Frm() ) )
    {
        const SwRootFrm* pRootFrm = static_cast<const SwRootFrm*>(pPage->GetUpper());
        const SwFrm* pTmpPage = pRootFrm ? pRootFrm->GetPageAtPos( rRect.TopLeft(), &rRect.SSize(), true ) : 0;
        if ( pTmpPage )
            pPage = pTmpPage;
    }

    return pPage;
}

SwFrm* GetFrmOfModify( SwModify& rMod, USHORT nFrmType, const Point* pPoint,
                        const SwPosition *pPos, const BOOL bCalcFrm )
{
    SwFrm *pMinFrm = 0, *pTmpFrm;
    SwRect aCalcRect;
    bool bClientIterChanged = false;

    SwClientIter aIter( rMod );
    do {
        pMinFrm = 0;
        sal_uInt64 nMinDist = 0;
        bClientIterChanged = false;

        for( pTmpFrm = (SwFrm*)aIter.First( TYPE( SwFrm )); pTmpFrm;
                pTmpFrm = (SwFrm*)aIter.Next() )
        {
            if( pTmpFrm->GetType() & nFrmType &&
                (!pTmpFrm->IsFlowFrm() ||
                 !SwFlowFrm::CastFlowFrm( pTmpFrm )->IsFollow() ))
            {
                if( pPoint )
                {
                    // --> FME 2006-02-03 #127369#
                    // Set pointer to be watched. If a client is removed from
                    // rMod (e.g., by deleting a frame), the bWatchDeleted flag
                    // is set at the SwClientIter.
                    const bool bWatchClientSet = pMinFrm != 0;
                    aIter.SetWatchClient( pMinFrm );
                    // <--

                    if( bCalcFrm )
                    {
                        // --> OD 2005-03-04 #b6234250# - format parent Writer
                        // fly frame, if it isn't been formatted yet.
                        // Note: The Writer fly frame could be the frame itself.
                        SwFlyFrm* pFlyFrm( pTmpFrm->FindFlyFrm() );
                        if ( pFlyFrm &&
                             pFlyFrm->Frm().Pos().X() == WEIT_WECH &&
                             pFlyFrm->Frm().Pos().Y() == WEIT_WECH )
                        {
                            SwObjectFormatter::FormatObj( *pFlyFrm );
                        }
                        // <--
                        pTmpFrm->Calc();
                    }

                    // --> FME 2006-02-03 #127369#
                    // The SwClientIter list has changed. Restart.
                    // aIter.IsChanged basically checks if pTmpFrm has been
                    // deleted. bWatchClientSet && aIter.GetWatchClient()
                    // checks if pMinFrm has been deleted.
                    // <--
                    if( aIter.IsChanged() || ( bWatchClientSet && !aIter.GetWatchClient() ) )
                    {
                        bClientIterChanged = true;
                        break;
                    }

                    // bei Flys ggfs. ueber den Parent gehen wenn sie selbst
                    // nocht nicht "formatiert" sind
                    if( !bCalcFrm && nFrmType & FRM_FLY &&
                        ((SwFlyFrm*)pTmpFrm)->GetAnchorFrm() &&
                        WEIT_WECH == pTmpFrm->Frm().Pos().X() &&
                        WEIT_WECH == pTmpFrm->Frm().Pos().Y() )
                        aCalcRect = ((SwFlyFrm*)pTmpFrm)->GetAnchorFrm()->Frm();
                    else
                        aCalcRect = pTmpFrm->Frm();

                    if ( aCalcRect.IsInside( *pPoint ) )
                    {
                        pMinFrm = pTmpFrm;
                        break;
                    }

                    // Point not in rectangle. Compare distances:
                    const Point aCalcRectCenter = aCalcRect.Center();
                    const Point aDiff = aCalcRectCenter - *pPoint;
                    const sal_uInt64 nCurrentDist = aDiff.X() * aDiff.X() + aDiff.Y() * aDiff.Y(); // opt: no sqrt
                    if ( !pMinFrm || nCurrentDist < nMinDist )
                    {
                        pMinFrm = pTmpFrm;
                        nMinDist = nCurrentDist;
                    }
                }
                else
                {
                    // Wenn kein pPoint angegeben ist, dann reichen
                    // wir irgendeinen raus: den ersten!
                    pMinFrm = pTmpFrm;
                    break;
                }
            }
        }
    } while( bClientIterChanged );

    if( pPos && pMinFrm && pMinFrm->IsTxtFrm() )
        return ((SwTxtFrm*)pMinFrm)->GetFrmAtPos( *pPos );

    return pMinFrm;
}

BOOL IsExtraData( const SwDoc *pDoc )
{
    const SwLineNumberInfo &rInf = pDoc->GetLineNumberInfo();
    return rInf.IsPaintLineNumbers() ||
           rInf.IsCountInFlys() ||
           ((sal_Int16)SW_MOD()->GetRedlineMarkPos() != text::HoriOrientation::NONE &&
            pDoc->GetRedlineTbl().Count());
}

// OD 22.09.2003 #110978#
const SwRect SwPageFrm::PrtWithoutHeaderAndFooter() const
{
    SwRect aPrtWithoutHeaderFooter( Prt() );
    aPrtWithoutHeaderFooter.Pos() += Frm().Pos();

    const SwFrm* pLowerFrm = Lower();
    while ( pLowerFrm )
    {
        // Note: independent on text direction page header and page footer are
        //       always at top respectively at bottom of the page frame.
        if ( pLowerFrm->IsHeaderFrm() )
        {
            aPrtWithoutHeaderFooter.Top( aPrtWithoutHeaderFooter.Top() +
                                         pLowerFrm->Frm().Height() );
        }
        if ( pLowerFrm->IsFooterFrm() )
        {
            aPrtWithoutHeaderFooter.Bottom( aPrtWithoutHeaderFooter.Bottom() -
                                            pLowerFrm->Frm().Height() );
        }

        pLowerFrm = pLowerFrm->GetNext();
    }

    return aPrtWithoutHeaderFooter;
}

/** method to determine the spacing values of a frame

    OD 2004-03-10 #i28701#

    @author OD
*/
void GetSpacingValuesOfFrm( const SwFrm& _rFrm,
                            SwTwips& _roLowerSpacing,
                            SwTwips& _roLineSpacing )
{
    if ( !_rFrm.IsFlowFrm() )
    {
        _roLowerSpacing = 0L;
        _roLineSpacing = 0L;
    }
    else
    {
        const SvxULSpaceItem& rULSpace = _rFrm.GetAttrSet()->GetULSpace();
        _roLowerSpacing = rULSpace.GetLower();

        _roLineSpacing = 0;
        if ( _rFrm.IsTxtFrm() )
        {
            _roLineSpacing = static_cast<const SwTxtFrm&>(_rFrm).GetLineSpace();
        }

        ASSERT( _roLowerSpacing >= 0 && _roLineSpacing >= 0,
                "<GetSpacingValuesOfFrm(..)> - spacing values aren't positive!" );
    }
}

/** method to get the content of the table cell, skipping content from nested tables
*/
const SwCntntFrm* GetCellCntnt( const SwLayoutFrm& rCell )
{
    const SwCntntFrm* pCntnt = rCell.ContainsCntnt();
    const SwTabFrm* pTab = rCell.FindTabFrm();

    while ( pCntnt && rCell.IsAnLower( pCntnt ) )
    {
        const SwTabFrm* pTmpTab = pCntnt->FindTabFrm();
        if ( pTmpTab != pTab )
        {
            pCntnt = pTmpTab->FindLastCntnt();
            if ( pCntnt )

                pCntnt = pCntnt->FindNextCnt();

        }
        else
            break;
    }
    return pCntnt;
}

/** Can be used to check if a frame has been deleted
 */
bool SwDeletionChecker::HasBeenDeleted()
{
    if ( !mpFrm || !mpRegIn )
        return false;

    SwClientIter aIter( const_cast<SwModify&>(*mpRegIn) );
    const SwClient* pLast = aIter.GoStart();

    while ( pLast )
    {
        if ( pLast->ISA( SwFrm ) && pLast == mpFrm )
            return false;

        pLast = aIter++;
    }

    return true;
}


