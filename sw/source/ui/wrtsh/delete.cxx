/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: delete.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2008-02-19 14:01:26 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _SWCRSR_HXX
#include <swcrsr.hxx>
#endif
#include <svx/lrspitem.hxx> // #i23725#
// --> OD 2006-07-10 #134369#
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DRAWBASE_HXX
#include <drawbase.hxx>
#endif
// <--

inline void SwWrtShell::OpenMark()
{
    StartAllAction();
    ResetCursorStack();
    KillPams();
    SetMark();
}

inline void SwWrtShell::CloseMark( BOOL bOkFlag )
{
    if( bOkFlag )
        UpdateAttr();
    else
        SwapPam();

    ClearMark();
    EndAllAction();
}

// #i23725#
BOOL SwWrtShell::TryRemoveIndent()
{
    BOOL bResult = FALSE;

    SfxItemSet aAttrSet(GetAttrPool(), RES_LR_SPACE, RES_LR_SPACE);
    GetAttr(aAttrSet);

    SvxLRSpaceItem aItem = (const SvxLRSpaceItem &)aAttrSet.Get(RES_LR_SPACE);
    short aOldFirstLineOfst = aItem.GetTxtFirstLineOfst();

    if (aOldFirstLineOfst > 0)
    {
        aItem.SetTxtFirstLineOfst(0);
        bResult = TRUE;
    }
    else if (aOldFirstLineOfst < 0)
    {
        aItem.SetTxtFirstLineOfst(0);
        aItem.SetLeft(aItem.GetLeft() + aOldFirstLineOfst);

        bResult = TRUE;
    }
    else if (aItem.GetLeft() != 0)
    {
        aItem.SetLeft(0);
        bResult = TRUE;
    }

    if (bResult)
    {
        aAttrSet.Put(aItem);
        SetAttr(aAttrSet);
    }

    return bResult;
}

/*------------------------------------------------------------------------
 Beschreibung:	Zeile loeschen
------------------------------------------------------------------------*/



long SwWrtShell::DelLine()
{
    ACT_KONTEXT(this);
    ResetCursorStack();
        // alten Cursor merken
    Push();
    ClearMark();
    SwCrsrShell::LeftMargin();
    SetMark();
    SwCrsrShell::RightMargin();
//Warum soll hier noch ein Zeichen in der naechsten Zeile geloescht werden?
//	if(!IsEndOfPara())
//		SwCrsrShell::Right();
    long nRet = Delete();
    Pop(FALSE);
    if( nRet )
        UpdateAttr();
    return nRet;
}



long SwWrtShell::DelToStartOfLine()
{
    OpenMark();
    SwCrsrShell::LeftMargin();
    long nRet = Delete();
    CloseMark( 0 != nRet );
    return nRet;
}



long SwWrtShell::DelToEndOfLine()
{
    OpenMark();
    SwCrsrShell::RightMargin();
    long nRet = Delete();
    CloseMark( 0 != nRet );
    return 1;
}

long SwWrtShell::DelLeft()
{
    // wenns denn ein Fly ist, wech damit
    int nSelType = GetSelectionType();
    const int nCmp = nsSelectionType::SEL_FRM | nsSelectionType::SEL_GRF | nsSelectionType::SEL_OLE | nsSelectionType::SEL_DRW;
    if( nCmp & nSelType )
    {
        /*  #108205# Remember object's position. */
        Point aTmpPt = GetObjRect().TopLeft();

        DelSelectedObj();

        /*  #108205# Set cursor to remembered position. */
        SetCrsr(&aTmpPt);

        LeaveSelFrmMode();
        UnSelectFrm();

        nSelType = GetSelectionType();
        if ( nCmp & nSelType )
        {
            EnterSelFrmMode();
            GotoNextFly();
        }

        return 1L;
    }

    // wenn eine Selektion existiert, diese loeschen.
    if ( IsSelection() )
    {
        if( !IsBlockMode() || HasSelection() )
        {
             //OS: wieder einmal Basic: ACT_KONTEXT muss vor
            //EnterStdMode verlassen werden!
            {
                ACT_KONTEXT(this);
                ResetCursorStack();
                Delete();
                UpdateAttr();
            }
            if( IsBlockMode() )
            {
                NormalizePam();
                ClearMark();
                EnterBlockMode();
            }
            else
                EnterStdMode();
            return 1L;
        }
        else
            EnterStdMode();
    }

    // JP 29.06.95: nie eine davor stehende Tabelle loeschen.
    BOOL bSwap = FALSE;
    const SwTableNode * pWasInTblNd = SwCrsrShell::IsCrsrInTbl();

    if( SwCrsrShell::IsSttPara())
    {
        // --> FME 2007-02-15 #i4032# Don't actually call a 'delete' if we
        // changed the table cell, compare DelRight().
        const SwStartNode * pSNdOld = pWasInTblNd ?
                                      GetSwCrsr()->GetNode()->FindTableBoxStartNode() :
                                      0;
        // <--

        /* If the cursor is at the beginning of a paragraph, try to step
           backwards. On failure we are done. */
        if( !SwCrsrShell::Left(1,CRSR_SKIP_CHARS) )
            return 0;

        /* If the cursor entered or left a table (or both) we are done. No step
           back. */
        const SwTableNode* pIsInTblNd = SwCrsrShell::IsCrsrInTbl();
        if( pIsInTblNd != pWasInTblNd )
            return 0;

        const SwStartNode* pSNdNew = pIsInTblNd ?
                                     GetSwCrsr()->GetNode()->FindTableBoxStartNode() :
                                     0;

        // --> FME 2007-02-15 #i4032# Don't actually call a 'delete' if we
        // changed the table cell, compare DelRight().
        if ( pSNdOld != pSNdNew )
            return 0;
        // <--

        OpenMark();
        SwCrsrShell::Right(1,CRSR_SKIP_CHARS);
        SwCrsrShell::SwapPam();
        bSwap = TRUE;
    }
    else
    {
        OpenMark();
        SwCrsrShell::Left(1,CRSR_SKIP_CHARS);
    }
    long nRet = Delete();
    if( !nRet && bSwap )
        SwCrsrShell::SwapPam();
    CloseMark( 0 != nRet );
    return nRet;
}

long SwWrtShell::DelRight()
{
        // werden verodert, wenn Tabellenselektion vorliegt;
        // wird hier auf nsSelectionType::SEL_TBL umgesetzt.
    long nRet = 0;
    int nSelection = GetSelectionType();
    if(nSelection & nsSelectionType::SEL_TBL_CELLS)
        nSelection = nsSelectionType::SEL_TBL;
    if(nSelection & nsSelectionType::SEL_TXT)
        nSelection = nsSelectionType::SEL_TXT;

    const SwTableNode * pWasInTblNd = NULL;

    switch( nSelection & ~(nsSelectionType::SEL_BEZ) )
    {
    case nsSelectionType::SEL_POSTIT:
    case nsSelectionType::SEL_TXT:
    case nsSelectionType::SEL_TBL:
    case nsSelectionType::SEL_NUM:
            //	wenn eine Selektion existiert, diese loeschen.
        if( IsSelection() )
        {
            if( !IsBlockMode() || HasSelection() )
            {
                //OS: wieder einmal Basic: ACT_KONTEXT muss vor
                //EnterStdMode verlassen werden!
                {
                    ACT_KONTEXT(this);
                    ResetCursorStack();
                    Delete();
                    UpdateAttr();
                }
                if( IsBlockMode() )
                {
                    NormalizePam();
                    ClearMark();
                    EnterBlockMode();
                }
                else
                    EnterStdMode();
                nRet = 1L;
                break;
            }
            else
                EnterStdMode();
        }

        pWasInTblNd = IsCrsrInTbl();

        if( nsSelectionType::SEL_TXT & nSelection && SwCrsrShell::IsSttPara() &&
            SwCrsrShell::IsEndPara() )
        {
            // save cursor
            SwCrsrShell::Push();

            bool bDelFull = false;
            if ( SwCrsrShell::Right(1,CRSR_SKIP_CHARS) )
            {
                const SwTableNode * pCurrTblNd = IsCrsrInTbl();
                bDelFull = pCurrTblNd && pCurrTblNd != pWasInTblNd;
            }

            // restore cursor
            SwCrsrShell::Pop( FALSE );

            if( bDelFull )
            {
                DelFullPara();
                UpdateAttr();
                break;
            }
        }

        {
            /* #108049# Save the startnode of the current cell */
            const SwStartNode * pSNdOld;
            pSNdOld = GetSwCrsr()->GetNode()->
                FindTableBoxStartNode();

            if ( SwCrsrShell::IsEndPara() )
            {
                // --> FME 2005-01-28 #i41424# Introduced a couple of
                // Push()-Pop() pairs here. The reason for this is that a
                // Right()-Left() combination does not make sure, that
                // the cursor will be in its initial state, because there
                // may be a numbering in front of the next paragraph.
                SwCrsrShell::Push();
                // <--

                if ( SwCrsrShell::Right(1, CRSR_SKIP_CHARS) )
                {
                    if (IsCrsrInTbl() || (pWasInTblNd != IsCrsrInTbl()))
                    {
                        /* #108049# Save the startnode of the current
                            cell. May be different to pSNdOld as we have
                            moved. */
                        const SwStartNode * pSNdNew = GetSwCrsr()
                            ->GetNode()->FindTableBoxStartNode();

                        /* #108049# Only move instead of deleting if we
                            have moved to a different cell */
                        if (pSNdOld != pSNdNew)
                        {
                            SwCrsrShell::Pop( TRUE );
                            break;
                        }
                    }
                }

                // restore cursor
                SwCrsrShell::Pop( FALSE );
            }
        }

        OpenMark();
        SwCrsrShell::Right(1,CRSR_SKIP_CELLS);
        nRet = Delete();
        CloseMark( 0 != nRet );
        break;

    case nsSelectionType::SEL_FRM:
    case nsSelectionType::SEL_GRF:
    case nsSelectionType::SEL_OLE:
    case nsSelectionType::SEL_DRW:
    case nsSelectionType::SEL_DRW_TXT:
    case nsSelectionType::SEL_DRW_FORM:
        {
            /*  #108205# Remember object's position. */
            Point aTmpPt = GetObjRect().TopLeft();

            DelSelectedObj();

            /*  #108205# Set cursor to remembered position. */
            SetCrsr(&aTmpPt);

            LeaveSelFrmMode();
            UnSelectFrm();
            // --> OD 2006-07-06 #134369#
            ASSERT( !IsFrmSelected(),
                    "<SwWrtShell::DelRight(..)> - <SwWrtShell::UnSelectFrm()> should unmark all objects" )
            // <--
            // --> OD 2006-07-10 #134369#
            // leave draw mode, if necessary.
            {
                if (GetView().GetDrawFuncPtr())
                {
                    GetView().GetDrawFuncPtr()->Deactivate();
                    GetView().SetDrawFuncPtr(NULL);
                }
                if ( GetView().IsDrawMode() )
                {
                    GetView().LeaveDrawCreate();
                }
            }
            // <--
        }

        // --> OD 2006-07-07 #134369#
        // <IsFrmSelected()> can't be true - see above.
        // <--
        {
            nSelection = GetSelectionType();
            if ( nsSelectionType::SEL_FRM & nSelection ||
                 nsSelectionType::SEL_GRF & nSelection ||
                 nsSelectionType::SEL_OLE & nSelection ||
                 nsSelectionType::SEL_DRW & nSelection )
            {
                EnterSelFrmMode();
                GotoNextFly();
            }
        }
        nRet = 1;
        break;
    }
    return nRet;
}



long SwWrtShell::DelToEndOfPara()
{
    ACT_KONTEXT(this);
    ResetCursorStack();
    Push();
    SetMark();
    if( !MovePara(fnParaCurr,fnParaEnd))
    {
        Pop(FALSE);
        return 0;
    }
    long nRet = Delete();
    Pop(FALSE);
    if( nRet )
        UpdateAttr();
    return nRet;
}



long SwWrtShell::DelToStartOfPara()
{
    ACT_KONTEXT(this);
    ResetCursorStack();
    Push();
    SetMark();
    if( !MovePara(fnParaCurr,fnParaStart))
    {
        Pop(FALSE);
        return 0;
    }
    long nRet = Delete();
    Pop(FALSE);
    if( nRet )
        UpdateAttr();
    return nRet;
}
/*
 * alle Loeschoperationen sollten mit Find statt mit
 * Nxt-/PrvDelim arbeiten, da letzteren mit Wrap Around arbeiten
 * -- das ist wohl nicht gewuenscht.
 */



long SwWrtShell::DelToStartOfSentence()
{
    if(IsStartOfDoc())
        return 0;
    OpenMark();
    long nRet = _BwdSentence() ? Delete() : 0;
    CloseMark( 0 != nRet );
    return nRet;
}



long SwWrtShell::DelToEndOfSentence()
{
    if(IsEndOfDoc())
        return 0;
    OpenMark();
    long nRet = _FwdSentence() ? Delete() : 0;
    CloseMark( 0 != nRet );
    return nRet;
}



long SwWrtShell::DelNxtWord()
{
    if(IsEndOfDoc())
        return 0;
    ACT_KONTEXT(this);
    ResetCursorStack();
    EnterStdMode();
    SetMark();
    if(IsEndWrd() && !IsSttWrd())
        _NxtWrd();
    if(IsSttWrd() || IsEndPara())
        _NxtWrd();
    else
        _EndWrd();

    long nRet = Delete();
    if( nRet )
        UpdateAttr();
    else
        SwapPam();
    ClearMark();
    return nRet;
}



long SwWrtShell::DelPrvWord()
{
    if(IsStartOfDoc())
        return 0;
    ACT_KONTEXT(this);
    ResetCursorStack();
    EnterStdMode();
    SetMark();
    if( !IsSttWrd() || !_PrvWrd() )
    {
        if( IsEndWrd() )
        {
            if( _PrvWrd() )
            {
                // skip over all-1 spaces
                short n = -1;
                while( ' ' == GetChar( FALSE, n ))
                    --n;

                if( ++n )
                    ExtendSelection( FALSE, -n );
            }
        }
        else if( IsSttPara())
            _PrvWrd();
        else
            _SttWrd();
    }
    long nRet = Delete();
    if( nRet )
        UpdateAttr();
    else
        SwapPam();
    ClearMark();
    return nRet;
}




