/*************************************************************************
 *
 *  $RCSfile: tabfrm.cxx,v $
 *
 *  $Revision: 1.31 $
 *
 *  last change: $Author: fme $ $Date: 2002-11-13 09:45:35 $
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

#include "pagefrm.hxx"
#include "rootfrm.hxx"
#include "cntfrm.hxx"
#include "viewsh.hxx"
#include "doc.hxx"
#include "docsh.hxx"
#include "viewimp.hxx"
#include "swtable.hxx"
#include "dflyobj.hxx"
#include "flyfrm.hxx"
#include "frmtool.hxx"
#include "frmfmt.hxx"
#include "dcontact.hxx"
#include "viewopt.hxx"
#include "hints.hxx"
#include "dbg_lay.hxx"

#include <ftnidx.hxx>

#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SVX_KEEPITEM_HXX //autogen
#include <svx/keepitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif

#ifndef _FMTTSPLT_HXX //autogen
#include <fmtlsplt.hxx>
#endif
#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#include "tabfrm.hxx"
#include "rowfrm.hxx"
#include "cellfrm.hxx"
#include "flyfrms.hxx"
#include "txtfrm.hxx"		//HasFtn()
#include "htmltbl.hxx"
#include "frmsh.hxx"
#include "sectfrm.hxx"	//SwSectionFrm

extern void AppendObjs( const SwSpzFrmFmts *pTbl, ULONG nIndex,
                        SwFrm *pFrm, SwPageFrm *pPage );

/*************************************************************************
|*
|*	SwTabFrm::SwTabFrm(), ~SwTabFrm()
|*
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 30. May. 96
|*
|*************************************************************************/
SwTabFrm::SwTabFrm( SwTable &rTab ):
    SwLayoutFrm( rTab.GetFrmFmt() ),
    SwFlowFrm( (SwFrm&)*this ),
    pTable( &rTab )
{
    bComplete = bCalcLowers = bONECalcLowers = bLowersFormatted = bLockBackMove =
    bResizeHTMLTable = FALSE;
    BFIXHEIGHT = FALSE;     //Nicht nochmal auf die Importfilter hereinfallen.
    nType = FRMC_TAB;

    //Gleich die Zeilen erzeugen und einfuegen.
    const SwTableLines &rLines = rTab.GetTabLines();
    SwFrm *pPrev = 0;
    for ( USHORT i = 0; i < rLines.Count(); ++i )
    {
        SwRowFrm *pNew = new SwRowFrm( *rLines[i] );
        if( pNew->Lower() )
        {
            pNew->InsertBehind( this, pPrev );
            pPrev = pNew;
        }
        else
            delete pNew;
    }
}

SwTabFrm::SwTabFrm( SwTabFrm &rTab ) :
    SwLayoutFrm( rTab.GetFmt() ),
    SwFlowFrm( (SwFrm&)*this ),
    pTable( rTab.GetTable() )
{
    bIsFollow = TRUE;
    bLockJoin = bComplete = bONECalcLowers = bCalcLowers = bLowersFormatted = bLockBackMove =
    bResizeHTMLTable = FALSE;
    BFIXHEIGHT = FALSE;     //Nicht nochmal auf die Importfilter hereinfallen.
    nType = FRMC_TAB;

    SetFollow( rTab.GetFollow() );
    rTab.SetFollow( this );
}

SwTabFrm::~SwTabFrm()
{
}

/*************************************************************************
|*
|*	SwTabFrm::JoinAndDelFollows()
|*
|*	Ersterstellung		MA 30. May. 96
|*	Letzte Aenderung	MA 30. May. 96
|*
|*************************************************************************/
void SwTabFrm::JoinAndDelFollows()
{
    SwTabFrm *pFoll = GetFollow();
    if ( pFoll->HasFollow() )
        pFoll->JoinAndDelFollows();
    pFoll->Cut();
    SetFollow( pFoll->GetFollow() );
    delete pFoll;
}

/*************************************************************************
|*
|*	SwTabFrm::RegistFlys()
|*
|*	Ersterstellung		MA 08. Jul. 93
|*	Letzte Aenderung	MA 27. Jan. 99
|*
|*************************************************************************/
void SwTabFrm::RegistFlys()
{
    ASSERT( Lower() && Lower()->IsRowFrm(), "Keine Zeilen." );

    SwPageFrm *pPage = FindPageFrm();
    if ( pPage )
    {
        SwRowFrm *pRow = (SwRowFrm*)Lower();
        do
        {	pRow->RegistFlys( pPage );
            pRow = (SwRowFrm*)pRow->GetNext();
        } while ( pRow );
    }
}

/*************************************************************************
|*
|*	SwTabFrm::Split(), Join()
|*
|*	Ersterstellung		MA 03. Jun. 93
|*	Letzte Aenderung	MA 03. Sep. 96
|*
|*************************************************************************/
SwTwips SwTabFrm::Split( const SwTwips nCutPos )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
    ASSERT( bVert ? nCutPos >= Frm().Left()
            && nCutPos <= Frm().Left() + Frm().Width() :
            nCutPos >= Frm().Top() && nCutPos <= Frm().Bottom(),
            "SplitLine out of table." );
#else
    ASSERT( nCutPos >= Frm().Top() && nCutPos <= Frm().Bottom(),
            "SplitLine ausserhalb der Tabelle." );
#endif

    //Um die Positionen der Zellen mit der CutPos zu vergleichen muessen sie
    //ausgehend von der Tabelle nacheinander berechnet werden. Sie koennen
    //wg. Positionsaenderungen der Tabelle durchaus ungueltig sein.

    SwFrm *pRow = Lower();
    if( !pRow )
        return 0;
#ifdef VERTICAL_LAYOUT
    SwTwips nCut = (*fnRect->fnYDiff)( nCutPos, (Frm().*fnRect->fnGetTop)() );
    nCut -= (this->*fnRect->fnGetTopMargin)();
    SwTwips nRowPos = (pRow->Frm().*fnRect->fnGetHeight)();
#else
    SwTwips nRowPos = Frm().Top() + Prt().Top() + pRow->Frm().Height();
#endif
    const BOOL bRepeat	= GetTable()->IsHeadlineRepeat();
    pRow = pRow->GetNext();
    if( pRow && bRepeat )
    {
#ifdef VERTICAL_LAYOUT
        nRowPos += (pRow->Frm().*fnRect->fnGetHeight)();
#else
        nRowPos += pRow->Frm().Height();
#endif
        pRow = pRow->GetNext();
    }
    // No break before the first row and, in case of repeated headlines,
    // before the the second row.
    if( !pRow )
        return 0;

#ifdef VERTICAL_LAYOUT
    while( pRow && nCut >= ( nRowPos + (pRow->Frm().*fnRect->fnGetHeight)() ) )
    {
        nRowPos += (pRow->Frm().*fnRect->fnGetHeight)();
#else
    while ( pRow && nCutPos > (nRowPos + pRow->Frm().Height() - 1) )
    {
        nRowPos += pRow->Frm().Height();
#endif
        pRow = pRow->GetNext();
    }

    if ( !pRow )
    {
#ifdef DEBUG
        ASSERT( FALSE, "Tablesplit out of rows?" );
#endif
        pRow = Lower();
        while ( pRow && pRow->GetNext() )
            pRow = pRow->GetNext();
    }

    //Wenn es bereits einen Follow gibt so geht's dort hinein andernfalls
    //muss eben einer erzeugt werden.
    FASTBOOL bNewFollow;
    SwTabFrm *pFoll;
    if ( GetFollow() )
    {
        pFoll = GetFollow();
        bNewFollow = FALSE;
    }
    else
    {
        bNewFollow = TRUE;
        pFoll = new SwTabFrm( *this );
        pFoll->InsertBehind( GetUpper(), this );

        if( bRepeat )
        {	//Ueberschrift wiederholen.
            ASSERT( GetTable()->GetTabLines()[0], "Table ohne Zeilen?" );
            bDontCreateObjects = TRUE;				//frmtool
            SwRowFrm *pHeadline = new SwRowFrm(
                                    *GetTable()->GetTabLines()[0] );
            bDontCreateObjects = FALSE;
            pHeadline->InsertBefore( pFoll, 0 );

            SwPageFrm *pPage = pHeadline->FindPageFrm();
            const SwSpzFrmFmts *pTbl = GetFmt()->GetDoc()->GetSpzFrmFmts();
            if( pTbl->Count() )
            {
                ULONG nIndex;
                SwCntntFrm* pFrm = pHeadline->ContainsCntnt();
                while( pFrm )
                {
                    nIndex = pFrm->GetNode()->GetIndex();
                    AppendObjs( pTbl, nIndex, pFrm, pPage );
                    pFrm = pFrm->GetNextCntntFrm();
                    if( !pHeadline->IsAnLower( pFrm ) )
                        break;
                }
            }
        }
    }
    SwTwips nRet = 0;
    SwFrm *pNxt;

    //Optimierung beim neuen Follow braucht's kein Paste und dann kann
    //das Optimierte Insert verwendet werden (nur dann treten gluecklicher weise
    //auch groessere Mengen von Rows auf).
    if ( bNewFollow )
    {
        SwFrm *pPrv = GetTable()->IsHeadlineRepeat() ? pFoll->Lower() : 0;
        while ( pRow )
        {
            pNxt = pRow->GetNext();
#ifdef VERTICAL_LAYOUT
            nRet += (pRow->Frm().*fnRect->fnGetHeight)();
#else
            nRet += pRow->Frm().Height();
#endif
            pRow->Remove();
            pRow->InsertBehind( pFoll, pPrv );
            pRow->_InvalidateAll();
            pPrv = pRow;
            pRow = pNxt;
        }
    }
    else
    {
        SwFrm *pPrv = pFoll->Lower();
        if ( pPrv && GetTable()->IsHeadlineRepeat() )
            pPrv = pPrv->GetNext();
        while ( pRow )
        {
            pNxt = pRow->GetNext();
#ifdef VERTICAL_LAYOUT
            nRet += (pRow->Frm().*fnRect->fnGetHeight)();
            pRow->Remove();
            pRow->Paste( pFoll, pPrv );
            pRow->CheckDirChange();
            pRow = pNxt;
        }
    }
    ASSERT( !bNewFollow || !(pFoll->Frm().*fnRect->fnGetHeight)(),
            "Dont care about Performance");
    Shrink( nRet );
#else
            nRet += pRow->Frm().Height();
            pRow->Remove();
            pRow->Paste( pFoll, pPrv );
            pRow = pNxt;
        }
    }
    ASSERT( !bNewFollow || !pFoll->Frm().Height(), "Dont care about Performance");
    Shrink( nRet PHEIGHT );
#endif
    return nRet;
}

SwTwips SwTabFrm::Join()
{
    SwTabFrm *pFoll = GetFollow();
    SwTwips nHeight = 0;	//Gesamthoehe der eingefuegten Zeilen als Return.

    if ( !pFoll->IsJoinLocked() )
    {
#ifdef VERTICAL_LAYOUT
        SWRECTFN( this )
#endif
        pFoll->Cut();	//Erst ausschneiden um unuetze Benachrichtigungen zu
                        //minimieren.

        SwFrm *pRow = pFoll->Lower(),
              *pNxt;

        if ( pRow && GetTable()->IsHeadlineRepeat() )
            pRow = pRow->GetNext();

        SwFrm *pPrv = Lower();
        while ( pPrv && pPrv->GetNext() )
            pPrv = pPrv->GetNext();
        while ( pRow )
        {
            pNxt = pRow->GetNext();
#ifdef VERTICAL_LAYOUT
            nHeight += (pRow->Frm().*fnRect->fnGetHeight)();
#else
            nHeight += pRow->Frm().Height();
#endif
            pRow->Remove();
            pRow->_InvalidateAll();
            pRow->InsertBehind( this, pPrv );
#ifdef VERTICAL_LAYOUT
            pRow->CheckDirChange();
#endif
            pPrv = pRow;
            pRow = pNxt;
        }
        SetFollow( pFoll->GetFollow() );
        delete pFoll;
        Grow( nHeight PHEIGHT );
    }
    return nHeight;
}

/*************************************************************************
|*
|*	SwTabFrm::MakeAll()
|*
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 10. Apr. 97
|*
|*************************************************************************/
void MA_FASTCALL SwInvalidatePositions( SwFrm *pFrm, long nBottom )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pFrm )
#endif
    do
    {	pFrm->_InvalidatePos();
        pFrm->_InvalidateSize();
        if( pFrm->IsLayoutFrm() )
        {
            if ( ((SwLayoutFrm*)pFrm)->Lower() )
                ::SwInvalidatePositions( ((SwLayoutFrm*)pFrm)->Lower(), nBottom);
        }
        else
            pFrm->Prepare( PREP_ADJUST_FRM );
        pFrm = pFrm->GetNext();
#ifdef VERTICAL_LAYOUT
    } while ( pFrm && (*fnRect->fnYDiff)( (pFrm->Frm().*fnRect->fnGetTop)(),
              nBottom ) < 0 );
#else
    } while ( pFrm && pFrm->Frm().Top() < nBottom );
#endif
}

BOOL MA_FASTCALL lcl_CalcLowers( SwLayoutFrm *pLay, long nBottom )
{
    BOOL bRet = FALSE;
    SwCntntFrm *pCnt = pLay->ContainsCntnt();
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pLay )
#endif
    while ( pCnt && pLay->GetUpper()->IsAnLower( pCnt ) )
    {
        bRet |= !pCnt->IsValid();
        pCnt->CalcFlys( FALSE );
        pCnt->Calc();
        pCnt->GetUpper()->Calc();
#ifdef VERTICAL_LAYOUT
        if( (*fnRect->fnYDiff)((pCnt->Frm().*fnRect->fnGetTop)(), nBottom) > 0 )
#else
        if ( pCnt->Frm().Top() > nBottom )
#endif
            break;
        pCnt = pCnt->GetNextCntntFrm();
    }
    return bRet;
}

BOOL MA_FASTCALL lcl_InnerCalcLayout( SwFrm *pFrm, long nBottom )
{
    BOOL bRet = FALSE;
    const SwFrm* pOldUp = pFrm->GetUpper();
    SWRECTFN( pFrm )
    do
    {
        if( pFrm->IsLayoutFrm() )
        {
            bRet |= !pFrm->IsValid();
            pFrm->Calc();
            if( ((SwLayoutFrm*)pFrm)->Lower() )
                bRet |= lcl_InnerCalcLayout( ((SwLayoutFrm*)pFrm)->Lower(), nBottom);
        }
        pFrm = pFrm->GetNext();
    } while( pFrm &&
            (*fnRect->fnYDiff)((pFrm->Frm().*fnRect->fnGetTop)(), nBottom) < 0
            && pFrm->GetUpper() == pOldUp );
    return bRet;
}

void MA_FASTCALL lcl_CalcLayout( SwLayoutFrm *pLay, long nBottom )
{
    BOOL bCheck = TRUE;
    do
    {
        while( lcl_InnerCalcLayout( pLay, nBottom ) )
            bCheck = TRUE;
        if( bCheck )
        {
            bCheck = FALSE;
            if( lcl_CalcLowers( pLay, nBottom ) )
                continue;
        }
        break;
    } while( TRUE );
}

void MA_FASTCALL lcl_FirstTabCalc( SwTabFrm *pTab )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pTab )
    if ( !pTab->IsFollow() && !pTab->GetTable()->IsTblComplex() )
    {
        SwLayoutFrm *pRow = (SwLayoutFrm*)pTab->Lower();
        do
        {
            SwLayoutFrm *pCell = (SwLayoutFrm*)pRow->Lower();
            SwFrm *pCnt = pCell->Lower();
            pCnt->Calc();
            const long nCellHeight = (pCell->Frm().*fnRect->fnGetHeight)();
            const long nCellY      = (pCell->Frm().*fnRect->fnGetTop)()-1;
            const long nCntHeight  = (pCnt->Frm().*fnRect->fnGetHeight)();
            const long nCntY       = (pCnt->Frm().*fnRect->fnGetTop)()-1;
            if ( 0 != (pCell = (SwLayoutFrm*)pCell->GetNext()) )
                do
                {   (pCell->Frm().*fnRect->fnSetTopAndHeight)
                                                        ( nCellY, nCellHeight );
                    (pCell->Prt().*fnRect->fnSetHeight)( nCellHeight );
                    pCell->_InvalidateAll();

                    pCnt = pCell->Lower();
                    (pCnt->Frm().*fnRect->fnSetTopAndHeight)(nCntY, nCntHeight);
                    (pCnt->Prt().*fnRect->fnSetHeight)( nCntHeight );
                    pCnt->_InvalidateAll();

                    pCell = (SwLayoutFrm*)pCell->GetNext();
                } while ( pCell );

            SwTwips nRowTop = (pRow->Frm().*fnRect->fnGetTop)();
            SwTwips nUpBot = (pTab->GetUpper()->Frm().*fnRect->fnGetBottom)();
            if( (*fnRect->fnYDiff)( nUpBot, nRowTop ) < 0 )
                break;
            pRow = (SwLayoutFrm*)pRow->GetNext();

        } while ( pRow );
    }
    SwFrm *pUp = pTab->GetUpper();
    long nBottom = (pUp->*fnRect->fnGetPrtBottom)();
    if ( pTab->GetFmt()->GetDoc()->IsBrowseMode() )
        nBottom += pUp->Grow( LONG_MAX, TRUE );
    lcl_CalcLowers( (SwLayoutFrm*)pTab->Lower(), nBottom );
#else
    //Ersteinmal koennen wir die Strukturen auf die richtige Groesse
    //bringen.
    if ( !pTab->IsFollow() && !pTab->GetTable()->IsTblComplex() )
    {
        SwLayoutFrm *pRow = (SwLayoutFrm*)pTab->Lower();
        do
        {
            SwLayoutFrm *pCell = (SwLayoutFrm*)pRow->Lower();
            SwFrm *pCnt = pCell->Lower();
            pCnt->Calc();
            const long nCellHeight = pCell->Frm().Height();
            const long nCellY	   = pCell->Frm().Top()-1;
            const long nCntHeight  = pCnt->Frm().Height();
            const long nCntY 	   = pCnt->Frm().Top()-1;
            if ( 0 != (pCell = (SwLayoutFrm*)pCell->GetNext()) )
                do
                {	pCell->Frm().SSize().Height() =
                    pCell->Prt().SSize().Height() = nCellHeight;
                    pCell->Frm().Pos().Y() = nCellY;
                    pCell->_InvalidateAll();

                    pCnt = pCell->Lower();
                    pCnt->Frm().SSize().Height() =
                    pCnt->Prt().SSize().Height() = nCntHeight;
                    pCnt->Frm().Pos().Y() = nCntY;
                    pCnt->_InvalidateAll();

                    pCell = (SwLayoutFrm*)pCell->GetNext();
                } while ( pCell );

            SwTwips nRowTop = pRow->Frm().Top();
            SwTwips nUpBot = pTab->GetUpper()->Frm().Bottom();
            if( nUpBot < nRowTop )
                break;
            pRow = (SwLayoutFrm*)pRow->GetNext();

        } while ( pRow );
    }
    //MA 28. Nov. 95: einen weiteren habe ich noch: Es braucht
    //nur soweit formatiert werden, wie Platz vorhanden ist.
    SwFrm *pUp = pTab->GetUpper();
    long nBottom = pUp->Prt().Bottom() + pUp->Frm().Top();
    if ( pTab->GetFmt()->GetDoc()->IsBrowseMode() )
        nBottom += pUp->Grow( LONG_MAX PHEIGHT, TRUE );
    lcl_CalcLowers( (SwLayoutFrm*)pTab->Lower(), nBottom );
#endif
}

void MA_FASTCALL lcl_Recalc( SwTabFrm *pTab,
                             SwLayoutFrm *pFirstRow,
                             SwLayNotify &rNotify )
{
    if ( pTab->Lower() )
    {
#ifdef VERTICAL_LAYOUT
        SWRECTFN( pTab )
        const SwTwips nOldHeight = (pTab->Frm().*fnRect->fnGetHeight)();
        const SwTwips nOldWidth  = (pTab->Frm().*fnRect->fnGetWidth)();
        if ( !pFirstRow )
        {
            pFirstRow = (SwLayoutFrm*)pTab->Lower();
            rNotify.SetLowersComplete( TRUE );
        }
        ::SwInvalidatePositions( pFirstRow, LONG_MAX );
        ::lcl_CalcLayout( pFirstRow, LONG_MAX );
        SwTwips nNew = (pTab->Frm().*fnRect->fnGetHeight)();
        if ( nOldHeight < nNew )
            rNotify.AddHeightOfst( nNew - nOldHeight );
        else if ( nOldHeight > nNew )
            rNotify.SubtractHeightOfst( nOldHeight - nNew );
        nNew = (pTab->Frm().*fnRect->fnGetWidth)();
        if ( nOldWidth < nNew )
            rNotify.AddWidthOfst( nNew - nOldWidth );
        else if ( nOldWidth > nNew )
            rNotify.SubtractWidthOfst( nOldWidth - nNew );
#else
        const SwTwips nOldHeight = pTab->Frm().Height();
        const SwTwips nOldWidth  = pTab->Frm().Width();
        if ( !pFirstRow )
        {
            pFirstRow = (SwLayoutFrm*)pTab->Lower();
            rNotify.SetLowersComplete( TRUE );
        }
        ::SwInvalidatePositions( pFirstRow, LONG_MAX );
        ::lcl_CalcLayout( pFirstRow, LONG_MAX );
        if ( nOldHeight < pTab->Frm().Height() )
            rNotify.AddHeightOfst( pTab->Frm().Height() - nOldHeight );
        else if ( nOldHeight > pTab->Frm().Height() )
            rNotify.SubtractHeightOfst( nOldHeight - pTab->Frm().Height() );
        if ( nOldWidth < pTab->Frm().Width() )
            rNotify.AddWidthOfst( pTab->Frm().Width() - nOldWidth );
        else if ( nOldWidth > pTab->Frm().Width() )
            rNotify.SubtractWidthOfst( nOldWidth - pTab->Frm().Width() );
#endif
    }
}

#define KEEPTAB ( !GetFollow() && !IsFollow() )

void SwTabFrm::MakeAll()
{
    if ( IsJoinLocked() || StackHack::IsLocked() || StackHack::Count() > 50 )
        return;

    PROTOCOL_ENTER( this, PROT_MAKEALL, 0, 0 )

    LockJoin(); //Ich lass mich nicht unterwegs vernichten.
    SwLayNotify aNotify( this );	//uebernimmt im DTor die Benachrichtigung
    // If pos is invalid, we have to call a SetInvaKeep at aNotify.
    // Otherwise the keep atribute would not work in front of a table.
    const BOOL bOldValidPos = GetValidPosFlag();

    //Wenn mein direkter Nachbar gleichzeitig mein Follow ist
    //verleibe ich mir das Teil ein.
    if ( GetNext() && GetNext() == GetFollow() )
        aNotify.AddHeightOfst( Join() );

    if ( bResizeHTMLTable )	//Optimiertes Zusammenspiel mit Grow/Shrink des Inhaltes
    {
        bResizeHTMLTable = FALSE;
        SwHTMLTableLayout *pLayout = GetTable()->GetHTMLTableLayout();
        if ( pLayout )
            bCalcLowers = pLayout->Resize(
                            pLayout->GetBrowseWidthByTabFrm( *this ), FALSE );
    }


    BOOL bMakePage	= TRUE;		//solange TRUE kann eine neue Seite
                                //angelegt werden (genau einmal)
    BOOL bMovedBwd	= FALSE;	//Wird TRUE wenn der Frame zurueckfliesst
    BOOL bMovedFwd	= FALSE;	//solange FALSE kann der Frm zurueck-
                                //fliessen (solange, bis er einmal
                                //vorwaerts ge'moved wurde).
    BOOL bSplit		= FALSE;	//Wird TRUE wenn der Frm gesplittet wurde.
    BOOL bFtnsInDoc = 0 != GetFmt()->GetDoc()->GetFtnIdxs().Count();
    BOOL bMoveable;
    const BOOL bRepeat	= GetTable()->IsHeadlineRepeat();
    const BOOL bFly		= IsInFly();

    SwBorderAttrAccess  *pAccess= new SwBorderAttrAccess( SwFrm::GetCache(), this );
    const SwBorderAttrs *pAttrs = pAccess->Get();

    const BOOL bKeep = IsKeep( *pAttrs );
    const BOOL bDontSplit = !IsFollow() && !GetFmt()->GetLayoutSplit().GetValue();

    if ( bDontSplit )
        while ( GetFollow() )
            aNotify.AddHeightOfst( Join() );

    //Einen Frischling moven wir gleich schon einmal vorwaerts...
    if ( !Frm().Top() && IsFollow() )
    {
        SwFrm *pPre = GetPrev();
        if ( pPre && pPre->IsTabFrm() && ((SwTabFrm*)pPre)->GetFollow() == this)
        {
            if ( !MoveFwd( bMakePage, FALSE ) )
                bMakePage = FALSE;
            bMovedFwd = TRUE;
        }
    }

    SWRECTFN( this )

    while ( !bValidPos || !bValidSize || !bValidPrtArea )
    {
        if ( TRUE == (bMoveable = IsMoveable()) )
            if ( CheckMoveFwd( bMakePage, bKeep && KEEPTAB, bMovedBwd ) )
            {
                bMovedFwd = TRUE;
                bCalcLowers = TRUE;
            }

        Point aOldPos( (Frm().*fnRect->fnGetPos)() );
        MakePos();
        if ( aOldPos != (Frm().*fnRect->fnGetPos)() )
        {
            if ( aOldPos.Y() != (Frm().*fnRect->fnGetTop)() )
            {
                SwHTMLTableLayout *pLayout = GetTable()->GetHTMLTableLayout();
                if( pLayout )
                {
                    delete pAccess;
                    bCalcLowers |= pLayout->Resize(
                        pLayout->GetBrowseWidthByTabFrm( *this ), FALSE );
                    pAccess= new SwBorderAttrAccess( SwFrm::GetCache(), this );
                    pAttrs = pAccess->Get();
                }

                bValidPrtArea = FALSE;
                aNotify.SetLowersComplete( FALSE );
            }
            SwFrm *pPre;
            if ( bKeep || (0 != (pPre = FindPrev()) &&
                           pPre->GetAttrSet()->GetKeep().GetValue()) )
            {
                bCalcLowers = TRUE;
            }
        }

        //Wir muessen die Hoehe der ersten Zeile kennen, denn nur wenn diese
        //kleiner wird muss ggf. der Master angestossen werden um noetigenfalls
        //die Zeile aufzunehmen.
        long n1StLineHeight = 0;
        if ( IsFollow() )
        {
            SwFrm *pFrm = Lower();
            if ( bRepeat && pFrm )
                pFrm = pFrm->GetNext();
            if ( pFrm )
                n1StLineHeight = (pFrm->Frm().*fnRect->fnGetHeight)();
        }

        if ( !bValidSize || !bValidPrtArea )
        {
            const BOOL bOptLower = (Frm().*fnRect->fnGetHeight)() == 0;

            const long nOldPrtWidth = (Prt().*fnRect->fnGetWidth)();
            const long nOldFrmWidth = (Frm().*fnRect->fnGetWidth)();
            const Point aOldPrtPos  = (Prt().*fnRect->fnGetPos)();
            Format( pAttrs );

            SwHTMLTableLayout *pLayout = GetTable()->GetHTMLTableLayout();
            if ( /*!bOptLower &&*/ pLayout &&
                 ((Prt().*fnRect->fnGetWidth)() != nOldPrtWidth ||
                  (Frm().*fnRect->fnGetWidth)() != nOldFrmWidth) )
            {
                delete pAccess;
                bCalcLowers |= pLayout->Resize(
                        pLayout->GetBrowseWidthByTabFrm( *this ), FALSE );
//					GetFmt()->GetDoc()->GetDocShell()->IsReadOnly() ? FALSE : TRUE );
                pAccess= new SwBorderAttrAccess( SwFrm::GetCache(), this );
                pAttrs = pAccess->Get();
            }
            if ( !bOptLower && aOldPrtPos != (Prt().*fnRect->fnGetPos)() )
                aNotify.SetLowersComplete( FALSE );

            if ( bOptLower )
            {
                //MA 24. May. 95: Optimierungsversuch!
                //Ganz nigel nagel neu das Teil. Damit wir nicht n-fach
                //MakeAll'en formatieren wir flugs den Inhalt.
                //Das erste Format mussten wir allerdings abwarten, damit
                //die Breiten Stimmen!
                //MA: Fix, Kein Calc wenn evtl. noch Seitengebunde Flys
                //an den Cntnt haengen (siehe frmtool.cxx, ~SwCntntNotify).
                SwDoc *pDoc = GetFmt()->GetDoc();
                if ( !pDoc->GetSpzFrmFmts()->Count() ||
                     pDoc->IsLoaded() || pDoc->IsNewDoc() )
                {
                    //MA 28. Nov. 95: Und wieder ein Trick, gleich mal sehen
                    //ob ein Rueckfluss lohnt.
                    if ( bMoveable && !GetPrev() )
                    {
                        GetLeaf( MAKEPAGE_NONE, FALSE ); //setzt das BackMoveJump
                        if ( SwFlowFrm::IsMoveBwdJump() )
                        {
                            BOOL bDummy;
                            SwFtnBossFrm *pOldBoss = bFtnsInDoc ?
                                FindFtnBossFrm( TRUE ) : 0;
                            const FASTBOOL bOldPrev = GetPrev() != 0;
                            if ( MoveBwd( bDummy ) )
                            {
                                SWREFRESHFN( this )
                                bMovedBwd = TRUE;
                                if ( bFtnsInDoc )
                                    MoveLowerFtns( 0, pOldBoss, 0, TRUE );

                                long nOldTop = (Frm().*fnRect->fnGetTop)();
                                MakePos();
                                if( nOldTop != (Frm().*fnRect->fnGetTop)() )
                                {
                                    SwHTMLTableLayout *pLayout =
                                        GetTable()->GetHTMLTableLayout();
                                    if( pLayout )
                                    {
                                        delete pAccess;
                                        bCalcLowers |= pLayout->Resize(
                                            pLayout->GetBrowseWidthByTabFrm(
                                                            *this ), FALSE );
                                        pAccess= new SwBorderAttrAccess(
                                                    SwFrm::GetCache(), this );
                                        pAttrs = pAccess->Get();
                                    }
                                }

                                if ( bOldPrev != (0 != GetPrev()) )
                                {
                                    //Abstand nicht vergessen!
                                    bValidPrtArea = FALSE;
                                    Format( pAttrs );
                                }
                                if ( bKeep && KEEPTAB )
                                {
                                    SwFrm *pNxt = FindNextCnt();
                                    // FindNextCnt geht ggf. in einen Bereich
                                    // hinein, in eine Tabelle allerdings auch
                                    if( pNxt && pNxt->IsInTab() )
                                        pNxt = pNxt->FindTabFrm();
                                    if ( pNxt )
                                    {
                                        pNxt->Calc();
                                        if ( !GetNext() )
                                            bValidPos = FALSE;
                                    }
                                }
                            }
                        }
                    }
                    ::lcl_FirstTabCalc( this );
                    bValidSize = bValidPrtArea = FALSE;
                    Format( pAttrs );
                    aNotify.SetLowersComplete( TRUE );
                }
            }
        }

        //Wenn ich der erste einer Kette bin koennte ich mal sehen ob
        //ich zurueckfliessen kann (wenn ich mich ueberhaupt bewegen soll).
        //Damit es keine Oszillation gibt, darf ich nicht gerade vorwaerts
        //geflosssen sein.
        if ( !GetIndPrev() && !bMovedFwd && (bMoveable || bFly) )
        {
            //Bei Follows muss der Master benachrichtigt
            //werden. Der Follow muss nur dann Moven, wenn er leere Blaetter
            //ueberspringen muss.
            if ( IsFollow() )
            {
                //Nur wenn die Hoehe der ersten Zeile kleiner geworder ist.
                SwFrm *pFrm = Lower();
                if ( bRepeat && pFrm )
                    pFrm = pFrm->GetNext();
                if(pFrm && n1StLineHeight >(pFrm->Frm().*fnRect->fnGetHeight)())
                {
                    SwTabFrm *pMaster = (SwTabFrm*)FindMaster();
                    BOOL bDummy;
                    if ( ShouldBwdMoved( pMaster->GetUpper(), FALSE, bDummy ) )
                        pMaster->InvalidatePos();
                }
            }
            SwFtnBossFrm *pOldBoss = bFtnsInDoc ? FindFtnBossFrm( TRUE ) : 0;
            BOOL bReformat;
            if ( MoveBwd( bReformat ) )
            {
                SWREFRESHFN( this )
                bMovedBwd = TRUE;
                aNotify.SetLowersComplete( FALSE );
                if ( bFtnsInDoc )
                    MoveLowerFtns( 0, pOldBoss, 0, TRUE );
                if ( bReformat || bKeep )
                {
                    long nOldTop = (Frm().*fnRect->fnGetTop)();
                    MakePos();
                    if( nOldTop != (Frm().*fnRect->fnGetTop)() )
                    {
                        SwHTMLTableLayout *pLayout =
                            GetTable()->GetHTMLTableLayout();
                        if( pLayout )
                        {
                            delete pAccess;
                            bCalcLowers |= pLayout->Resize(
                                pLayout->GetBrowseWidthByTabFrm( *this ),
                                FALSE );
                            pAccess= new SwBorderAttrAccess(
                                        SwFrm::GetCache(), this );
                            pAttrs = pAccess->Get();
                        }

                        bValidPrtArea = FALSE;
                        Format( pAttrs );
                    }
                    ::lcl_Recalc( this, 0, aNotify );
                    bLowersFormatted = TRUE;
                    if ( bKeep && KEEPTAB )
                    {
                        SwFrm *pNxt = FindNextCnt();
                        if( pNxt && pNxt->IsInTab() )
                            pNxt = pNxt->FindTabFrm();
                        if ( pNxt )
                        {
                            pNxt->Calc();
                            if ( !GetNext() )
                                bValidPos = FALSE;
                        }
                    }
                }
            }
        }

        //Wieder ein Wert ungueltig? - dann nochmal das ganze...
        if ( !bValidPos || !bValidSize || !bValidPrtArea )
            continue;

        // check, if calculation of table frame is ready.

        /// OD 23.10.2002 #103517# - Local variable <nDistanceToUpperPrtBottom>
        ///     Introduce local variable and init it with the distance from the
        ///     table frame bottom to the bottom of the upper printing area.
        /// Note: negative values denotes the situation that table frame doesn't
        ///     fit in its upper.
        SwTwips nDistanceToUpperPrtBottom =
                (Frm().*fnRect->fnBottomDist)( (GetUpper()->*fnRect->fnGetPrtBottom)());

        /// OD 23.10.2002 #103517# - In online layout try to grow upper of table
        /// frame, if table frame doesn't fit in its upper.
        if ( nDistanceToUpperPrtBottom < 0 &&
             GetFmt()->GetDoc()->IsBrowseMode() )
        {
            if ( GetUpper()->Grow( -nDistanceToUpperPrtBottom ) )
            {
                // upper is grown --> recalculate <nDistanceToUpperPrtBottom>
                nDistanceToUpperPrtBottom =
                    (Frm().*fnRect->fnBottomDist)( (GetUpper()->*fnRect->fnGetPrtBottom)());
            }
        }

        if( nDistanceToUpperPrtBottom >= 0)
        {
            // OD 23.10.2002 - translate german commentary
            // If there is space left in the upper printing area, join as for trial
            // at least one further row of an existing follow.
            if ( !bSplit && GetFollow() )
            {
                BOOL bDummy;
                if ( GetFollow()->ShouldBwdMoved( GetUpper(), FALSE, bDummy ) )
                {
                    SwFrm *pTmp = GetUpper();
                    SwTwips nDeadLine = (pTmp->*fnRect->fnGetPrtBottom)();
                    if ( GetFmt()->GetDoc()->IsBrowseMode() )
                        nDeadLine += pTmp->Grow( LONG_MAX, TRUE );
                    if( (Frm().*fnRect->fnBottomDist)( nDeadLine ) > 0 )
                    {
                        SwFrm *pRow = GetFollow()->Lower();
                        if ( bRepeat )
                            pRow = pRow->GetNext();
                        const SwTwips nOld = (Frm().*fnRect->fnGetHeight)();

                        const BOOL bMoveFtns = bFtnsInDoc && pRow &&
                                               !GetFollow()->IsJoinLocked();

                        SwFtnBossFrm *pOldBoss;
                        if ( bMoveFtns )
                            pOldBoss = pRow->FindFtnBossFrm( TRUE );

                        //fix(8680): Row kann 0 werden.
                        if ( !pRow || !pRow->GetNext() )
                            //Der Follow wird leer und damit ueberfluessig.
                            aNotify.AddHeightOfst( Join() );
                        else
                        {
                            pRow->Cut();
                            pRow->Paste( this );
                            aNotify.AddHeightOfst(
                                        (pRow->Frm().*fnRect->fnGetHeight)() );
                        }
                        //Die Fussnoten verschieben!
                        if ( pRow && bMoveFtns )
                            if ( ((SwLayoutFrm*)pRow)->MoveLowerFtns(
                                 0, pOldBoss, FindFtnBossFrm( TRUE ), TRUE ) )
                                GetUpper()->Calc();

                        if ( pRow && nOld != (Frm().*fnRect->fnGetHeight)() )
                            ::lcl_Recalc( this, (SwLayoutFrm*)pRow, aNotify );
                        continue;
                    }
                }
            }
            else if ( bKeep && KEEPTAB )
            {
                SwFrm *pNxt = FindNextCnt();
                if( pNxt && pNxt->IsInTab() )
                    pNxt = pNxt->FindTabFrm();
                if ( pNxt )
                    pNxt->Calc();
            }
            if ( IsValid() )
            {
                if ( bCalcLowers )
                {
                    ::lcl_Recalc( this, 0, aNotify );
                    bLowersFormatted = TRUE;
                    bCalcLowers = FALSE;
                }
                else if ( bONECalcLowers )
                {
                    lcl_CalcLayout( (SwLayoutFrm*)Lower(), LONG_MAX );
                    bONECalcLowers = FALSE;
                }
            }
            continue;
        }

        //Ich passe nicht mehr in meinen Uebergeordneten, also ist es jetzt
        //an der Zeit moeglichst konstruktive Veranderungen vorzunehmen

        //Wenn ich den uebergeordneten Frm nicht verlassen darf, habe
        //ich ein Problem; Frei nach Artur Dent tun wir das einzige das man
        //mit einen nicht loesbaren Problem tun kann: wir ignorieren es - und
        //zwar mit aller Kraft.
        if ( !bMoveable )
        {
            if ( bCalcLowers && IsValid() )
            {
                lcl_Recalc( this, 0, aNotify );
                bLowersFormatted = TRUE;
                bCalcLowers = FALSE;
            }
            else if ( bONECalcLowers )
            {
                lcl_CalcLayout( (SwLayoutFrm*)Lower(), LONG_MAX );
                bONECalcLowers = FALSE;
            }
            continue;
        }

        if ( bCalcLowers && IsValid() )
        {
            ::lcl_Recalc( this, 0, aNotify );
            bLowersFormatted = TRUE;
            bCalcLowers = FALSE;
            if( !IsValid() )
                continue;
        }

        //Der erste Versuch muss natuerlich das Aufspalten der Tabelle sein.
        //Das funktioniert natuerlich nur dann, wenn die Tabelle mehr als eine
        //Zeile enthaelt und wenn die Unterkante des Upper unter der ersten
        //Zeile liegt.
        SwFrm *pIndPrev = GetIndPrev();
        if ( Lower()->GetNext() && (!bDontSplit || !pIndPrev) )
        {
            //Damit der Schatten nicht extra herausgerechnet werden muss,
            //lassen wir das Spiel gleich wenn es ein HeadlineRepeat gibt und
            //nur noch eine nicht Headline Zeile vorhanden ist.
            if ( !bRepeat || Lower()->GetNext()->GetNext() )
            {
                SwTwips nDeadLine = (GetUpper()->*fnRect->fnGetPrtBottom)();
                if( IsInSct() )
                    nDeadLine = (*fnRect->fnYInc)( nDeadLine,
                                        GetUpper()->Grow( LONG_MAX, TRUE ) );
                ::lcl_CalcLayout( (SwLayoutFrm*)Lower(), nDeadLine );
                bLowersFormatted = TRUE;
                aNotify.SetLowersComplete( TRUE );
                if( (Frm().*fnRect->fnBottomDist)( nDeadLine ) > 0 )
                    continue;

                SwTwips nBreakLine = (Frm().*fnRect->fnGetTop)();
                nBreakLine = (*fnRect->fnYInc)( nBreakLine,
                        (this->*fnRect->fnGetTopMargin)() +
                        (Lower()->Frm().*fnRect->fnGetHeight)() +
                        ( bRepeat ?
                          (Lower()->GetNext()->Frm().*fnRect->fnGetHeight)()
                          : 0 ) );
                if( (*fnRect->fnYDiff)(nDeadLine, nBreakLine) >=0 || !pIndPrev )
                {
                    aNotify.SubtractHeightOfst( Split( nDeadLine ) );
                    if ( aNotify.GetHeightOfst() < 0 )
                        aNotify.ResetHeightOfst();
                    aNotify.SetLowersComplete( FALSE );
                    bSplit = TRUE;
                    //Damit es nicht zu Oszillationen kommt, muss der
                    //Follow gleich gueltig gemacht werden.
                    if ( GetFollow() )
                    {
                        static BYTE nStack = 0;
                        if ( !StackHack::IsLocked() && nStack < 4 )
                        {
                            ++nStack;
                            StackHack aHack;
                            delete pAccess;
                            GetFollow()->MakeAll();
                            pAccess= new SwBorderAttrAccess( SwFrm::GetCache(),
                                                             this );
                            pAttrs = pAccess->Get();
                            ((SwTabFrm*)GetFollow())->SetLowersFormatted(FALSE);
                            ::lcl_CalcLayout((SwLayoutFrm*)GetFollow()->Lower(),
                                GetFollow()->GetUpper()->Frm().Bottom() );
                            if ( !GetFollow()->GetFollow() )
                            {
                                SwFrm *pNxt = ((SwFrm*)GetFollow())->FindNext();
                                if ( pNxt )
                                    pNxt->Calc();
                            }
                            --nStack;
                        }
                        else if ( GetFollow() == GetNext() )
                            ((SwTabFrm*)GetFollow())->MoveFwd( TRUE, FALSE );
                        ViewShell *pSh;
                        if ( 0 != (pSh = GetShell()) )
                            pSh->Imp()->ResetScroll();
                    }
                    continue;
                }
            }
        }

        if( IsInSct() && bMovedFwd && bMakePage && GetUpper()->IsColBodyFrm() &&
            GetUpper()->GetUpper()->GetUpper()->IsSctFrm() &&
            ( GetUpper()->GetUpper()->GetPrev() || GetIndPrev() ) &&
            ((SwSectionFrm*)GetUpper()->GetUpper()->GetUpper())->MoveAllowed(this) )
            bMovedFwd = FALSE;

        //Mal sehen ob ich irgenwo Platz finde...
        if ( !bMovedFwd && !MoveFwd( bMakePage, FALSE ) )
            bMakePage = FALSE;
        SWREFRESHFN( this )
        bMovedFwd = bCalcLowers = TRUE;
        aNotify.SetLowersComplete( FALSE );
        if ( IsFollow() )
        {	//Um Oszillationen zu vermeiden sollte kein ungueltiger Master
            //zurueckbleiben.
            SwTabFrm *pTab = FindMaster();
            if ( pTab->GetUpper() )
                pTab->GetUpper()->Calc();
            pTab->Calc();
            pTab->SetLowersFormatted( FALSE );
        }

        //Wenn mein direkter Nachbar jetzt gleichzeitig mein Follow ist
        //verleibe ich mir das Teil ein.
        if ( GetNext() && GetNext() == GetFollow() )
            aNotify.AddHeightOfst( Join() );

        if ( bMovedBwd && GetUpper() )
            //Beim zurueckfliessen wurde der Upper angeregt sich vollstaendig
            //zu Painten, dass koennen wir uns jetzt nach dem hin und her
            //fliessen sparen.
            GetUpper()->ResetCompletePaint();

        if ( bCalcLowers && IsValid() )
        {
            ::lcl_Recalc( this, 0, aNotify );
            bLowersFormatted = TRUE;
            bCalcLowers = FALSE;
        }

    } //while ( !bValidPos || !bValidSize || !bValidPrtArea )

    //Wenn mein direkter Vorgaenger jetzt mein Master ist, so kann er mich
    //bei der nachstbesten Gelegenheit vernichten.
    if ( IsFollow() )
    {
        SwFrm *pPre = GetPrev();
        if ( pPre && pPre->IsTabFrm() && ((SwTabFrm*)pPre)->GetFollow() == this)
            pPre->InvalidatePos();
    }

    bCalcLowers = bONECalcLowers = FALSE;
    delete pAccess;
    UnlockJoin();
    if ( bMovedFwd || bMovedBwd || ! bOldValidPos )
        aNotify.SetInvaKeep();
}

/*************************************************************************
|*
|*	SwTabFrm::CalcFlyOffsets()
|*
|*	Beschreibung:		Berechnet die Offsets, die durch FlyFrames
|*						entstehen.
|*	Ersterstellung		MA/MIB 14. Apr. 99
|*	Letzte Aenderung
|*
|*************************************************************************/
BOOL SwTabFrm::CalcFlyOffsets( SwTwips& rUpper,
                               long& rLeftOffset,
                               long& rRightOffset ) const
{
    BOOL bInvalidatePrtArea = FALSE;
    const SwPageFrm *pPage = FindPageFrm();
    const SwFlyFrm* pMyFly = FindFlyFrm();
    if ( pPage->GetSortedObjs() )
    {
#ifdef VERTICAL_LAYOUT
        SWRECTFN( this )
        long nPrtPos = (Frm().*fnRect->fnGetTop)();
        nPrtPos = (*fnRect->fnYInc)( nPrtPos, rUpper );
        SwRect aRect( Frm() );
        long nYDiff = (*fnRect->fnYDiff)( (Prt().*fnRect->fnGetTop)(), rUpper );
        if( nYDiff > 0 )
            (aRect.*fnRect->fnAddBottom)( -nYDiff );
        for ( USHORT i = 0; i < pPage->GetSortedObjs()->Count(); ++i )
        {
            SdrObject *pObj = (*pPage->GetSortedObjs())[i];
            if ( pObj->IsWriterFlyFrame() )
            {
                SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
                const SwRect aFlyRect = pFly->AddSpacesToFrm();
                if ( WEIT_WECH != (pFly->Frm().*fnRect->fnGetTop)() &&
                     pFly->IsFlyAtCntFrm() && aFlyRect.IsOver( aRect ) &&
                     (*fnRect->fnYDiff)(
                            (pFly->GetAnchor()->Frm().*fnRect->fnGetBottom)(),
                            (Frm().*fnRect->fnGetTop)() ) < 0 &&
                     !IsAnLower( pFly ) && !pFly->IsAnLower( this ) &&
                     ( !pMyFly || pMyFly->IsAnLower( pFly ) ) &&
                     pPage->GetPhyPageNum() >=
                     pFly->GetAnchor()->FindPageFrm()->GetPhyPageNum() )
                {
                    const SwFmtSurround   &rSur = pFly->GetFmt()->GetSurround();
                    const SwFmtHoriOrient &rHori= pFly->GetFmt()->GetHoriOrient();
                    if ( SURROUND_NONE == rSur.GetSurround() )
                    {
                        long nBottom = (aFlyRect.*fnRect->fnGetBottom)();
                        if( (*fnRect->fnYDiff)( nPrtPos, nBottom ) < 0 )
                            nPrtPos = nBottom;
                        bInvalidatePrtArea = TRUE;
                    }
                    if ( (SURROUND_RIGHT	== rSur.GetSurround() ||
                          SURROUND_PARALLEL == rSur.GetSurround())&&
                         HORI_LEFT == rHori.GetHoriOrient() )
                    {
                        const long nWidth = (*fnRect->fnXDiff)(
                            (aFlyRect.*fnRect->fnGetRight)(),
                            (pFly->GetAnchor()->Frm().*fnRect->fnGetLeft)() );
                        rLeftOffset = Max( rLeftOffset, nWidth );
                        bInvalidatePrtArea = TRUE;
                    }
                    if ( (SURROUND_LEFT		== rSur.GetSurround() ||
                          SURROUND_PARALLEL == rSur.GetSurround())&&
                         HORI_RIGHT == rHori.GetHoriOrient() )
                    {
                        const long nWidth = (*fnRect->fnXDiff)(
                            (pFly->GetAnchor()->Frm().*fnRect->fnGetRight)(),
                            (aFlyRect.*fnRect->fnGetLeft)() );
                        rRightOffset = Max( rRightOffset, nWidth );
                        bInvalidatePrtArea = TRUE;
                    }
                }
            }
        }
        rUpper = (*fnRect->fnYDiff)( nPrtPos, (Frm().*fnRect->fnGetTop)() );
#else
        long nPrtPos = Frm().Top() + rUpper;
        SwRect aRect( Frm() );
        if ( Prt().Top() - rUpper > 0 )
            aRect.SSize().Height() -= Prt().Top() - rUpper;
        for ( USHORT i = 0; i < pPage->GetSortedObjs()->Count(); ++i )
        {
            SdrObject *pObj = (*pPage->GetSortedObjs())[i];
            if ( pObj->IsWriterFlyFrame() )
            {
                SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
                const SwRect aFlyRect = pFly->AddSpacesToFrm();
                if ( WEIT_WECH != pFly->Frm().Top() &&
                     pFly->IsFlyAtCntFrm() && aFlyRect.IsOver( aRect ) &&
                     pFly->GetAnchor()->Frm().Bottom() < Frm().Top() &&
                     !IsAnLower( pFly ) && !pFly->IsAnLower( this ) &&
                     ( !pMyFly || pMyFly->IsAnLower( pFly ) ) &&
                     pPage->GetPhyPageNum() >=
                     pFly->GetAnchor()->FindPageFrm()->GetPhyPageNum() )
                {
                    const SwFmtSurround   &rSur = pFly->GetFmt()->GetSurround();
                    const SwFmtHoriOrient &rHori= pFly->GetFmt()->GetHoriOrient();
                    if ( SURROUND_NONE == rSur.GetSurround() )
                    {
                        nPrtPos = Max( nPrtPos, aFlyRect.Bottom() + 1L );
                        bInvalidatePrtArea = TRUE;
                    }
                    if ( (SURROUND_RIGHT	== rSur.GetSurround() ||
                          SURROUND_PARALLEL == rSur.GetSurround())&&
                         HORI_LEFT == rHori.GetHoriOrient() )
                    {
                        //Der Rahmen kann auch noch einem anderen Rahmen
                        //ausgewichen sein.
                        const long nWidth = aFlyRect.Width() +
                                    aFlyRect.Left() - pFly->GetAnchor()->Frm().Left();
                        rLeftOffset = Max( rLeftOffset, nWidth );
                        bInvalidatePrtArea = TRUE;
                    }
                    if ( (SURROUND_LEFT		== rSur.GetSurround() ||
                          SURROUND_PARALLEL == rSur.GetSurround())&&
                         HORI_RIGHT == rHori.GetHoriOrient() )
                    {
                        const long nWidth = aFlyRect.Width() +
                                    pFly->GetAnchor()->Frm().Right() - aFlyRect.Right();
                        rRightOffset = Max( rRightOffset, nWidth );
                        bInvalidatePrtArea = TRUE;
                    }
                }
            }
        }
        rUpper = nPrtPos - Frm().Top();
#endif
    }

    return bInvalidatePrtArea;
}

/*************************************************************************
|*
|*	SwTabFrm::Format()
|*
|*	Beschreibung:		"Formatiert" den Frame; Frm und PrtArea
|*						Die Fixsize wird hier nicht eingestellt.
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 18. Jun. 97
|*
|*************************************************************************/
void SwTabFrm::Format( const SwBorderAttrs *pAttrs )
{
    ASSERT( pAttrs, "TabFrm::Format, pAttrs ist 0." );

#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
    if ( !bValidSize )
    {
        long nDiff = (GetUpper()->Prt().*fnRect->fnGetWidth)() -
                     (Frm().*fnRect->fnGetWidth)();
        if( nDiff )
            (aFrm.*fnRect->fnAddRight)( nDiff );
    }
#else
    //FixSize einstellen
    if ( !bValidSize )
        aFrm.Width( GetUpper()->Prt().Width() );
#endif

    //VarSize ist immer die Hoehe.
    //Fuer den oberen/unteren Rand gelten die selben Regeln wie fuer
    //cntfrms (sie MakePrtArea() von diesen).

    SwTwips nUpper = CalcUpperSpace( pAttrs );

    //Wir wollen Rahmen ausweichen. Zwei Moeglichkeiten:
    //1. Es gibt Rahmen mit SurroundNone, diesen wird vollsaendig ausgewichen
    //2. Es gibt Rahmen mit Umlauf nur rechts bzw. nur links und diese sind
    //   rechts bzw. links ausgerichtet, diese geben ein Minimum fuer die
    //	 Raender vor.
    long nTmpRight = -1000000,
         nLeftOffset  = 0;
    if( CalcFlyOffsets( nUpper, nLeftOffset, nTmpRight ) )
        bValidPrtArea = FALSE;
    long nRightOffset = Max( 0L, nTmpRight );

    SwTwips nLower = pAttrs->CalcBottomLine();

    if ( !bValidPrtArea )
    {	bValidPrtArea = TRUE;

        //Die Breite der PrtArea wird vom FrmFmt vorgegeben, die Raender
        //sind entsprechend einzustellen.
        //Mindestraender werden von Umrandung und Schatten vorgegeben.
        //Die Rander werden so eingestellt, dass die PrtArea nach dem
        //angegebenen Adjustment im Frm ausgerichtet wird.
        //Wenn das Adjustment 0 ist, so werden die Rander anhand des
        //Randattributes eingestellt.

#ifdef VERTICAL_LAYOUT
        const SwTwips nOldHeight = (Prt().*fnRect->fnGetHeight)();
        const SwTwips nMax = (aFrm.*fnRect->fnGetWidth)();
#else
        const SwTwips nOldHeight = Prt().Height();
        const SwTwips nMax = aFrm.Width();
#endif

        SwTwips nLeft  = pAttrs->CalcLeftLine();
        SwTwips nRight = pAttrs->CalcRightLine();

        //Die Breite ist evtl. eine Prozentangabe. Wenn die Tabelle irgendwo
        //'drinsteckt bezieht sie sich auf die Umgebung. Ist es der Body, so
        //bezieht sie sich in der BrowseView auf die Bildschirmbreite.
        const SwFmtFrmSize &rSz = GetFmt()->GetFrmSize();
        const SwTwips nWish = CalcRel( rSz, TRUE );

        BOOL bCheckBrowseWidth = FALSE;

        switch ( GetFmt()->GetHoriOrient().GetHoriOrient() )
        {
            case HORI_LEFT:
                {
                    const SwTwips nTmp = nMax-nLeftOffset-nRightOffset -
                                         (nWish + nLeft);
                    nLeft  += nLeftOffset;
                    nRight += nRightOffset;
                    nRight -= nLeftOffset;
                    if ( nTmp > nRight )
                        nRight = nTmp;
                    nRight = Max( nRight, 0L );
                }
                break;
            case HORI_RIGHT:
                {
                    const SwTwips nTmp = nMax-nLeftOffset-nRightOffset -
                                         (nWish + nRight);
                    nRight += nRightOffset;
                    nLeft  += nLeftOffset;
                    nLeft  -= nRightOffset;
                    if ( nTmp > nLeft )
                        nLeft = nTmp;
                    nLeft = Max( nLeft, 0L );
                }
                break;
            case HORI_CENTER:
                {
                    if( nLeftOffset || nRightOffset )
                    {
                        const SwTwips nTmp = ( nMax - nLeftOffset - nRightOffset
                                               - nWish) / 2;
                        nLeft += nLeftOffset;
                        nRight+= nRightOffset;
                        if( nTmp > nLeft )
                            nLeft = nTmp;
                        if ( nTmp > nRight )
                            nRight = nTmp;
                        nRight= Max( nRight, 0L );
                        nLeft = Max( nLeft, 0L );
                    }
                    else
                    {
                        nLeft = ( nMax - nWish ) / 2;
                        nRight = nLeft;
                    }
                }
                break;
            case HORI_FULL:
                    //Das Teil dehnt sich ueber die gesamte Breite aus.
                    //Nur die fuer die Umrandung benoetigten Freiraeume
                    //werden beruecksichtigt.
                    //Die Attributwerte von LRSpace werden bewusst missachtet!
                    bCheckBrowseWidth = TRUE;
                    nLeft  += nLeftOffset;
                    nRight += nRightOffset;
                break;
            case HORI_NONE:
                {
                    //Die Raender werden vom Randattribut bestimmt.
                    nLeft = pAttrs->CalcLeft( this );
                    if( nLeftOffset )
                    {
                        if( nLeft < 0 )
                            nLeft = 0;
                        nLeft += nLeftOffset;
                    }
                    nRight = pAttrs->CalcRight();
                    if( nRightOffset )
                    {
                        if( nRight < 0 )
                            nRight = 0;
                        nRight += nRightOffset;
                    }
                    if ( !pAttrs->GetLRSpace().GetRight() )
                        nRight = Max( nRight, nMax - (nWish + nLeft + nRight));
                }
                break;
            case HORI_LEFT_AND_WIDTH:
                    //Linker Rand und die Breite zaehlen (Word-Spezialitaet)
                    bCheckBrowseWidth = TRUE;
                    nLeft = pAttrs->CalcLeft( this );
                    if( nLeftOffset )
                    {
                        if( nLeft < 0 )
                            nLeft = 0;
                        nLeft += nLeftOffset;
                    }
                    nRight = Max( nMax - nLeft - nWish, nTmpRight );
                break;
            default:
                ASSERT( FALSE, "Ungueltige orientation fuer Table." );
        }
#ifdef VERTICAL_LAYOUT
        (this->*fnRect->fnSetYMargins)( nUpper, nLower );
        if( (nMax - MINLAY) < (nLeft + nRight) )
            (this->*fnRect->fnSetXMargins)( 0, 0 );
        else
            (this->*fnRect->fnSetXMargins)( nLeft, nRight );
#else
        Prt().Top( nUpper );
        Prt().Height( aFrm.Height() - (nUpper + nLower) );
        if ( (nMax - MINLAY) < (nLeft + nRight) )
        {	//Wenn die Raender garnicht passen lasse ich sie lieber gleich weg.
            Prt().Left( 0 );
            Prt().Width( nMax );
        }
        else
        {
            Prt().Left( nLeft );
            Prt().Width( nMax - (nLeft + nRight) );
        }
#endif
        ViewShell *pSh;
        if ( bCheckBrowseWidth && GetFmt()->GetDoc()->IsBrowseMode() &&
             GetUpper()->IsPageBodyFrm() &&  // nur PageBodyFrms, nicht etwa ColBodyFrms
             0 != (pSh = GetShell()) && pSh->VisArea().Width() )
        {
            //Nicht ueber die Kante des sichbaren Bereiches hinausragen.
            //Die Seite kann breiter sein, weil es Objekte mit "ueberbreite"
            //geben kann (RootFrm::ImplCalcBrowseWidth())
            const Size aBorder = pSh->GetOut()->PixelToLogic( pSh->GetBrowseBorder() );
            long nWidth = pSh->VisArea().Width() - 2 * aBorder.Width();
            nWidth -= Prt().Left();
            nWidth -= pAttrs->CalcRightLine();
            Prt().Width( Min( nWidth, Prt().Width() ) );
        }

#ifdef VERTICAL_LAYOUT
        if ( nOldHeight != (Prt().*fnRect->fnGetHeight)() )
#else
        if ( nOldHeight != Prt().Height() )
#endif
            bValidSize = FALSE;
    }

    if ( !bValidSize )
    {
        bValidSize = TRUE;

        //Die Groesse wird durch den Inhalt plus den Raendern bestimmt.
        SwTwips nRemaining = 0, nDiff;
        SwFrm *pFrm = pLower;
        while ( pFrm )
#ifdef VERTICAL_LAYOUT
        {   nRemaining += (pFrm->Frm().*fnRect->fnGetHeight)();
#else
        {	nRemaining += pFrm->Frm().Height();
#endif
            pFrm = pFrm->GetNext();
        }
        //Jetzt noch die Raender addieren
        nRemaining += nUpper + nLower;

#ifdef VERTICAL_LAYOUT
        nDiff = (Frm().*fnRect->fnGetHeight)() - nRemaining;
#else
        nDiff = Frm().Height() - nRemaining;
#endif
        if ( nDiff > 0 )
            Shrink( nDiff PHEIGHT );
        else if ( nDiff < 0 )
            Grow( -nDiff PHEIGHT );
    }
}
/*************************************************************************
|*
|*	SwTabFrm::GrowFrm()
|*
|*	Ersterstellung		MA 12. Mar. 93
|*	Letzte Aenderung	MA 23. Sep. 96
|*
|*************************************************************************/
#ifdef VERTICAL_LAYOUT
SwTwips SwTabFrm::GrowFrm( SwTwips nDist, BOOL bTst, BOOL bInfo )
{
    SWRECTFN( this )
    SwTwips nHeight =(Frm().*fnRect->fnGetHeight)();
    if( nHeight > 0 && nDist > ( LONG_MAX - nHeight ) )
        nDist = LONG_MAX - nHeight;
#else
SwTwips SwTabFrm::GrowFrm( SwTwips nDist, const SzPtr pDirection,
                           BOOL bTst, BOOL bInfo )
{
    if( pDirection == pWidth )
        return 0;

    if ( Frm().SSize().*pDirection > 0 &&
         nDist > (LONG_MAX - Frm().SSize().*pDirection) )
        nDist = LONG_MAX - Frm().SSize().*pDirection;
#endif

    //Tabelle waechst immer (sie kann ja ggf. aufgespalten werden).
    if ( !bTst )
    {
        if ( GetUpper() )
        {
            //Der Upper wird nur soweit wie notwendig gegrowed. In nReal wird erstmal
            //die bereits zur Verfuegung stehende Strecke bereitgestellt.
#ifdef VERTICAL_LAYOUT
            SwTwips nReal = (GetUpper()->Prt().*fnRect->fnGetHeight)();
            SwFrm *pFrm = GetUpper()->Lower();
            while ( pFrm )
            {   nReal -= (pFrm->Frm().*fnRect->fnGetHeight)();
                pFrm = pFrm->GetNext();
            }

#ifdef ACCESSIBLE_LAYOUT
            SwRect aOldFrm( Frm() );
#endif
            nHeight = (Frm().*fnRect->fnGetHeight)();
            (Frm().*fnRect->fnSetHeight)( nHeight + nDist );
            if( IsVertical() && !IsReverse() )
                Frm().Pos().X() -= nDist;
            if ( nReal < nDist )
                GetUpper()->Grow( nDist - (nReal>0 ? nReal : 0), bTst, bInfo );

#ifdef ACCESSIBLE_LAYOUT
            SwRootFrm *pRootFrm = FindRootFrm();
            if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
                pRootFrm->GetCurrShell() )
            {
                pRootFrm->GetCurrShell()->Imp()->MoveAccessibleFrm( this, aOldFrm );
            }
#endif
        }
        else
        {
            ASSERT( !this, "Table without Upper" );
#ifdef ACCESSIBLE_LAYOUT
            SwRect aOldFrm( Frm() );
#endif
            nHeight = (Frm().*fnRect->fnGetHeight)();
            (Frm().*fnRect->fnSetHeight)( nHeight + nDist );
            if( IsVertical() && !IsReverse() )
                Frm().Pos().X() -= nDist;
#ifdef ACCESSIBLE_LAYOUT
            SwRootFrm *pRootFrm = FindRootFrm();
            if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
                pRootFrm->GetCurrShell() )
            {
                pRootFrm->GetCurrShell()->Imp()->MoveAccessibleFrm( this, aOldFrm );
            }
#endif
#else
            SwTwips nReal = GetUpper()->Prt().SSize().*pDirection;
            SwFrm *pFrm = GetUpper()->Lower();
            while ( pFrm )
            {   nReal -= pFrm->Frm().SSize().*pDirection;
                pFrm = pFrm->GetNext();
            }

            Frm().SSize().*pDirection += nDist;

            if ( nReal < nDist )
                GetUpper()->Grow( nDist - (nReal > 0 ? nReal : 0),
                                                pDirection, bTst, bInfo );
        }
        else
        {
            ASSERT( !this, "Table without Upper" );
            Frm().SSize().*pDirection += nDist;
#endif
        }

        SwPageFrm *pPage = FindPageFrm();
        if ( GetNext() )
        {
            GetNext()->_InvalidatePos();
            if ( GetNext()->IsCntntFrm() )
                GetNext()->InvalidatePage( pPage );
        }
        _InvalidateAll();
        InvalidatePage( pPage );
        SetComplete();

        const SvxGraphicPosition ePos = GetFmt()->GetBackground().GetGraphicPos();
        if ( GPOS_NONE != ePos && GPOS_TILED != ePos )
            SetCompletePaint();
    }
    return nDist;
}
/*************************************************************************
|*
|*	  SwTabFrm::Modify()
|*
|*	  Ersterstellung	MA 14. Mar. 93
|*	  Letzte Aenderung	MA 06. Dec. 96
|*
|*************************************************************************/
void SwTabFrm::Modify( SfxPoolItem * pOld, SfxPoolItem * pNew )
{
    BYTE nInvFlags = 0;
    BOOL bAttrSetChg = pNew && RES_ATTRSET_CHG == pNew->Which();

    if( bAttrSetChg )
    {
        SfxItemIter aNIter( *((SwAttrSetChg*)pNew)->GetChgSet() );
        SfxItemIter aOIter( *((SwAttrSetChg*)pOld)->GetChgSet() );
        SwAttrSetChg aOldSet( *(SwAttrSetChg*)pOld );
        SwAttrSetChg aNewSet( *(SwAttrSetChg*)pNew );
        while( TRUE )
        {
            _UpdateAttr( (SfxPoolItem*)aOIter.GetCurItem(),
                         (SfxPoolItem*)aNIter.GetCurItem(), nInvFlags,
                         &aOldSet, &aNewSet );
            if( aNIter.IsAtEnd() )
                break;
            aNIter.NextItem();
            aOIter.NextItem();
        }
        if ( aOldSet.Count() || aNewSet.Count() )
            SwLayoutFrm::Modify( &aOldSet, &aNewSet );
    }
    else
        _UpdateAttr( pOld, pNew, nInvFlags );

    if ( nInvFlags != 0 )
    {
        SwPageFrm *pPage = FindPageFrm();
        InvalidatePage( pPage );
//		if ( nInvFlags & 0x01 )
//			SetCompletePaint();
        if ( nInvFlags & 0x02 )
            _InvalidatePrt();
        if ( nInvFlags & 0x40 )
            _InvalidatePos();
        SwFrm *pTmp;
        if ( 0 != (pTmp = GetIndNext()) )
        {
            if ( nInvFlags & 0x04 )
            {
                pTmp->_InvalidatePrt();
                if ( pTmp->IsCntntFrm() )
                    pTmp->InvalidatePage( pPage );
            }
            if ( nInvFlags & 0x10 )
                pTmp->SetCompletePaint();
        }
        if ( nInvFlags & 0x08 && 0 != (pTmp = GetPrev()) )
        {
            pTmp->_InvalidatePrt();
            if ( pTmp->IsCntntFrm() )
                pTmp->InvalidatePage( pPage );
        }
        if ( nInvFlags & 0x20  )
        {
            if ( pPage && pPage->GetUpper() && !IsFollow() )
                ((SwRootFrm*)pPage->GetUpper())->InvalidateBrowseWidth();
        }
        if ( nInvFlags & 0x80 )
            InvalidateNextPos();
    }
}

void SwTabFrm::_UpdateAttr( SfxPoolItem *pOld, SfxPoolItem *pNew,
                            BYTE &rInvFlags,
                            SwAttrSetChg *pOldSet, SwAttrSetChg *pNewSet )
{
    BOOL bClear = TRUE;
    const USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
    switch( nWhich )
    {
        case RES_TBLHEADLINECHG:
            //Es wird getoggelt.
            if ( IsFollow() )
            {
                if ( GetTable()->IsHeadlineRepeat() )
                {
                    bDontCreateObjects = TRUE;			//frmtool
                    SwFrm *pRow = new SwRowFrm( *GetTable()->GetTabLines()[0] );
                    bDontCreateObjects = FALSE;
                    pRow->Paste( this, Lower() );
                }
                else if ( Lower() )
                {
                    SwFrm *pLow = Lower();
                    pLow->Cut();
                    delete pLow;
                }
            }
            else if ( !HasFollow() )
                rInvFlags |= 0x02;
            break;

        case RES_FRM_SIZE:
        case RES_HORI_ORIENT:
            rInvFlags |= 0x22;
            break;

        case RES_PAGEDESC:						//Attributaenderung (an/aus)
            if ( IsInDocBody() )
            {
                rInvFlags |= 0x40;
                SwPageFrm *pPage = FindPageFrm();
                if ( !GetPrev() )
                    CheckPageDescs( pPage );
                if ( pPage && GetFmt()->GetPageDesc().GetNumOffset() )
                    ((SwRootFrm*)pPage->GetUpper())->SetVirtPageNum( TRUE );
                SwDocPosUpdate aMsgHnt( pPage->Frm().Top() );
                GetFmt()->GetDoc()->UpdatePageFlds( &aMsgHnt );
            }
            break;

        case RES_BREAK:
            rInvFlags |= 0xC0;
            break;

        case RES_LAYOUT_SPLIT:
            if ( !IsFollow() )
                rInvFlags |= 0x40;
            break;
        case RES_FRAMEDIR :
            SetDerivedR2L( sal_False );
            CheckDirChange();
            break;
        case RES_UL_SPACE:
            rInvFlags |= 0x1C;
            /* kein Break hier */

        default:
            bClear = FALSE;
    }
    if ( bClear )
    {
        if ( pOldSet || pNewSet )
        {
            if ( pOldSet )
                pOldSet->ClearItem( nWhich );
            if ( pNewSet )
                pNewSet->ClearItem( nWhich );
        }
        else
            SwLayoutFrm::Modify( pOld, pNew );
    }
}

/*************************************************************************
|*
|*	  SwTabFrm::GetInfo()
|*
|*	  Ersterstellung	MA 06. Dec. 96
|*	  Letzte Aenderung	MA 26. Jun. 98
|*
|*************************************************************************/
BOOL SwTabFrm::GetInfo( SfxPoolItem &rHnt ) const
{
    if ( RES_VIRTPAGENUM_INFO == rHnt.Which() && IsInDocBody() )
    {
        SwVirtPageNumInfo &rInfo = (SwVirtPageNumInfo&)rHnt;
        const SwPageFrm *pPage = FindPageFrm();
        if ( pPage  )
        {
            if ( pPage == rInfo.GetOrigPage() && !GetPrev() )
            {
                //Das sollte er sein (kann allenfalls temporaer anders sein,
                //					  sollte uns das beunruhigen?)
                rInfo.SetInfo( pPage, this );
                return FALSE;
            }
            if ( pPage->GetPhyPageNum() < rInfo.GetOrigPage()->GetPhyPageNum() &&
                 (!rInfo.GetPage() || pPage->GetPhyPageNum() > rInfo.GetPage()->GetPhyPageNum()))
            {
                //Das koennte er sein.
                rInfo.SetInfo( pPage, this );
            }
        }
    }
    return TRUE;
}

/*************************************************************************
|*
|*	  SwTabFrm::FindLastCntnt()
|*
|*	  Ersterstellung	MA 13. Apr. 93
|*	  Letzte Aenderung	MA 15. May. 98
|*
|*************************************************************************/
SwCntntFrm *SwTabFrm::FindLastCntnt()
{
    SwFrm *pRet = pLower;
    while ( pRet && !pRet->IsCntntFrm() )
    {
        SwFrm *pOld = pRet;

        SwFrm *pTmp = pRet;             // To skip empty section frames
        while ( pRet->GetNext() )
        {
            pRet = pRet->GetNext();
            if( !pRet->IsSctFrm() || ((SwSectionFrm*)pRet)->GetSection() )
                pTmp = pRet;
        }
        pRet = pTmp;

        if ( pRet->GetLower() )
            pRet = pRet->GetLower();
        if ( pRet == pOld )
        {	// Wenn am Ende der letzten Zelle ein spaltiger Bereich steht,
            // der eine leere letzte Spalte hat, muessen wir noch die anderen
            // Spalten abklappern, dies erledigt SwSectionFrm::FindLastCntnt
            if( pRet->IsColBodyFrm() )
            {
#ifndef PRODUCT
                SwSectionFrm* pSect = pRet->FindSctFrm();
                ASSERT( pSect, "Wo kommt denn die Spalte her?")
                ASSERT( IsAnLower( pSect ), "Gespaltene Zelle?" );
#endif
                return pRet->FindSctFrm()->FindLastCntnt();
            }
            return 0;	//Hier geht es nicht weiter. Inkonsistenter Zustand
                        //der Tabelle (z.B. Undo TextToTable).
        }
    }
//	ASSERT( pRet && pRet->IsCntntFrm(), "Letzter Lower von Tab kein Cnt." );
    if ( pRet ) //#50235#
        while ( pRet->GetNext() )
            pRet = pRet->GetNext();
    if( pRet->IsSctFrm() )
        pRet = ((SwSectionFrm*)pRet)->FindLastCntnt();
    ASSERT( pRet && pRet->IsCntntFrm(), "Letzter Lower von Tab kein Cnt." );
    return (SwCntntFrm*)pRet;
}

/*************************************************************************
|*
|*	SwTabFrm::GetLeaf()
|*
|*	Ersterstellung		MA 19. Mar. 93
|*	Letzte Aenderung	MA 25. Apr. 95
|*
|*************************************************************************/
SwLayoutFrm *SwTabFrm::GetLeaf( MakePageType eMakePage, BOOL bFwd )
{
    SwLayoutFrm *pRet;
    if ( bFwd )
    {
        pRet = GetNextLeaf( eMakePage );
        while ( IsAnLower( pRet ) )
            pRet = pRet->GetNextLeaf( eMakePage );
    }
    else
        pRet = GetPrevLeaf();
    if ( pRet )
        pRet->Calc();
    return pRet;
}

/*************************************************************************
|*
|*	SwTabFrm::ShouldBwdMoved()
|*
|* 	Beschreibung		Returnwert sagt ob der Frm verschoben werden sollte
|*	Ersterstellung		MA 10. Jul. 95
|*	Letzte Aenderung	MA 04. Mar. 97
|*
|*************************************************************************/
BOOL SwTabFrm::ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL bHead, BOOL &rReformat )
{
    rReformat = FALSE;
    if ( (SwFlowFrm::IsMoveBwdJump() || !IsPrevObjMove()) )
    {
        //Das zurueckfliessen von Frm's ist leider etwas Zeitintensiv.
        //Der haufigste Fall ist der, dass dort wo der Frm hinfliessen
        //moechte die FixSize die gleiche ist, die der Frm selbst hat.
        //In diesem Fall kann einfach geprueft werden, ob der Frm genug
        //Platz fuer seine VarSize findet, ist dies nicht der Fall kann
        //gleich auf das Verschieben verzichtet werden.
        //Die Pruefung, ob der Frm genug Platz findet fuehrt er selbst
        //durch, dabei wird beruecksichtigt, dass er sich moeglicherweise
        //aufspalten kann.
        //Wenn jedoch die FixSize eine andere ist oder Flys im Spiel sind
        //(an der alten oder neuen Position) hat alle Prueferei keinen Sinn
        //der Frm muss dann halt Probehalber verschoben werden (Wenn ueberhaupt
        //etwas Platz zur Verfuegung steht).

        //Die FixSize der Umgebungen in denen Tabellen herumlungern ist immer
        //Die Breite.

        SwPageFrm *pOldPage = FindPageFrm(),
                  *pNewPage = pNewUpper->FindPageFrm();
        BOOL bMoveAnyway = FALSE;
        SwTwips nSpace = 0;

#ifdef VERTICAL_LAYOUT
        SWRECTFN( this )
        if ( !SwFlowFrm::IsMoveBwdJump() )
        {

            long nOldWidth = (GetUpper()->Prt().*fnRect->fnGetWidth)();
            SWRECTFNX( pNewUpper );
            long nNewWidth = (pNewUpper->Prt().*fnRectX->fnGetWidth)();
            if( Abs( nNewWidth - nOldWidth ) < 2 )
            {
                if( FALSE ==
                    ( bMoveAnyway = BwdMoveNecessary( pOldPage, Frm() ) > 1 ) )
                {
                    SwRect aRect( pNewUpper->Prt() );
                    aRect.Pos() += pNewUpper->Frm().Pos();
                    const SwFrm *pPrevFrm = pNewUpper->Lower();
                    while ( pPrevFrm )
                    {
                        (aRect.*fnRectX->fnSetTop)( (pPrevFrm->Frm().*fnRectX->
                                                    fnGetBottom)() );
                        pPrevFrm = pPrevFrm->GetNext();
                    }
                    bMoveAnyway = BwdMoveNecessary( pNewPage, aRect) > 1;
                    nSpace = (aRect.*fnRectX->fnGetHeight)();
                    if ( GetFmt()->GetDoc()->IsBrowseMode() )
                        nSpace += pNewUpper->Grow( LONG_MAX, TRUE );
                }
            }
            else if( !bLockBackMove )
                bMoveAnyway = TRUE;
        }
        else if( !bLockBackMove )
            bMoveAnyway = TRUE;

        if ( bMoveAnyway )
            return rReformat = TRUE;
        else if ( !bLockBackMove )
        {	const BOOL bRepeat = GetTable()->IsHeadlineRepeat();
            SwTwips nHeight = bRepeat && Lower()->GetNext() ?
                    (Lower()->GetNext()->Frm().*fnRect->fnGetHeight)()
                    : (Lower()->Frm().*fnRect->fnGetHeight)();
            if ( bHead && bRepeat && Lower()->GetNext() )
                nHeight += (Lower()->Frm().*fnRect->fnGetHeight)();
            return nHeight <= nSpace;
        }
#else
        if ( !SwFlowFrm::IsMoveBwdJump() &&
             Abs(pNewUpper->Prt().Width() - GetUpper()->Prt().Width()) < 2 )
        {
            if ( FALSE == (bMoveAnyway = BwdMoveNecessary( pOldPage, Frm() ) > 1) )
            {
                //Das Rechteck, in dem ich landen wuerde berechenen.
                SwRect aRect( pNewUpper->Prt() );
                aRect.Pos() += pNewUpper->Frm().Pos();
                const SwFrm *pPrevFrm = pNewUpper->Lower();
                while ( pPrevFrm )
                {
                    aRect.Top( pPrevFrm->Frm().Bottom() );
                    pPrevFrm = pPrevFrm->GetNext();
                }
                bMoveAnyway = BwdMoveNecessary( pNewPage, aRect) > 1;
                nSpace = aRect.Height();
                if ( GetFmt()->GetDoc()->IsBrowseMode() )
                    nSpace += pNewUpper->Grow( LONG_MAX PHEIGHT, TRUE );
            }
        }
        else if ( SwFlowFrm::IsMoveBwdJump() || !bLockBackMove )
            bMoveAnyway = TRUE;

        if ( bMoveAnyway )
            return rReformat = TRUE;
        else if ( !bLockBackMove )
        {	const BOOL bRepeat = GetTable()->IsHeadlineRepeat();
            SwTwips nHeight = bRepeat && Lower()->GetNext() ?
                    Lower()->GetNext()->Frm().Height() : Lower()->Frm().Height();
            if ( bHead && bRepeat && Lower()->GetNext() )
                nHeight += Lower()->Frm().Height();
            return nHeight <= nSpace;
        }
#endif
    }
    return FALSE;
}

/*************************************************************************
|*
|*	SwTabFrm::Cut()
|*
|*	Ersterstellung		MA 23. Feb. 94
|*	Letzte Aenderung	MA 09. Sep. 98
|*
|*************************************************************************/
void SwTabFrm::Cut()
{
    ASSERT( GetUpper(), "Cut ohne Upper()." );

    SwPageFrm *pPage = FindPageFrm();
    InvalidatePage( pPage );
    SwFrm *pFrm = GetNext();
    if( pFrm )
    {	//Der alte Nachfolger hat evtl. einen Abstand zum Vorgaenger
        //berechnet der ist jetzt wo er der erste wird obsolete
        pFrm->_InvalidatePrt();
        pFrm->_InvalidatePos();
        if ( pFrm->IsCntntFrm() )
            pFrm->InvalidatePage( pPage );
        if( IsInSct() && !GetPrev() )
        {
            SwSectionFrm* pSct = FindSctFrm();
            if( !pSct->IsFollow() )
            {
                pSct->_InvalidatePrt();
                pSct->InvalidatePage( pPage );
            }
        }
    }
    else
    {
        InvalidateNextPos();
        //Einer muss die Retusche uebernehmen: Vorgaenger oder Upper
        if ( 0 != (pFrm = GetPrev()) )
        {	pFrm->SetRetouche();
            pFrm->Prepare( PREP_WIDOWS_ORPHANS );
            pFrm->_InvalidatePos();
            if ( pFrm->IsCntntFrm() )
                pFrm->InvalidatePage( pPage );
        }
        //Wenn ich der einzige FlowFrm in meinem Upper bin (war), so muss
        //er die Retouche uebernehmen.
        //Ausserdem kann eine Leerseite entstanden sein.
        else
        {	SwRootFrm *pRoot = (SwRootFrm*)pPage->GetUpper();
            pRoot->SetSuperfluous();
            GetUpper()->SetCompletePaint();
            if( IsInSct() )
            {
                SwSectionFrm* pSct = FindSctFrm();
                if( !pSct->IsFollow() )
                {
                    pSct->_InvalidatePrt();
                    pSct->InvalidatePage( pPage );
                }
            }
        }
    }

    //Erst removen, dann Upper Shrinken.
    SwLayoutFrm *pUp = GetUpper();
#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
#endif
    Remove();
    if ( pUp )
    {
        ASSERT( !pUp->IsFtnFrm(), "Tabelle in Fussnote." );
        SwSectionFrm *pSct;
        if( !pUp->Lower() && pUp->IsInSct() &&
            !(pSct = pUp->FindSctFrm())->ContainsCntnt() )
        {
            if ( pUp->GetUpper() )
            {
                pSct->DelEmpty( FALSE );
                pSct->_InvalidateSize();
            }
        }
#ifdef VERTICAL_LAYOUT
        else if( (Frm().*fnRect->fnGetHeight)() )
#else
        else if ( Frm().Height() )
#endif
            pUp->Shrink( Frm().Height() PHEIGHT );
    }

    if ( pPage && !IsFollow() && pPage->GetUpper() )
        ((SwRootFrm*)pPage->GetUpper())->InvalidateBrowseWidth();
}

/*************************************************************************
|*
|*	SwTabFrm::Paste()
|*
|*	Ersterstellung		MA 23. Feb. 94
|*	Letzte Aenderung	MA 09. Sep. 98
|*
|*************************************************************************/
void SwTabFrm::Paste( SwFrm* pParent, SwFrm* pSibling )
{
    ASSERT( pParent, "Kein Parent fuer Paste." );
    ASSERT( pParent->IsLayoutFrm(), "Parent ist CntntFrm." );
    ASSERT( pParent != this, "Bin selbst der Parent." );
    ASSERT( pSibling != this, "Bin mein eigener Nachbar." );
    ASSERT( !GetPrev() && !GetNext() && !GetUpper(),
            "Bin noch irgendwo angemeldet." );

    //In den Baum einhaengen.
    InsertBefore( (SwLayoutFrm*)pParent, pSibling );

    _InvalidateAll();
    SwPageFrm *pPage = FindPageFrm();
    InvalidatePage( pPage );

    if ( GetNext() )
    {
        GetNext()->_InvalidatePos();
        GetNext()->_InvalidatePrt();
        if ( GetNext()->IsCntntFrm() )
            GetNext()->InvalidatePage( pPage );
    }

#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
    if( (Frm().*fnRect->fnGetHeight)() )
        pParent->Grow( (Frm().*fnRect->fnGetHeight)() );

    if( (Frm().*fnRect->fnGetWidth)() != (pParent->Prt().*fnRect->fnGetWidth)() )
        Prepare( PREP_FIXSIZE_CHG );
#else
    if ( Frm().Height() )
        pParent->Grow( Frm().Height() PHEIGHT );

    if ( Frm().Width() != pParent->Prt().Width() )
        Prepare( PREP_FIXSIZE_CHG );
#endif
    if ( GetPrev() )
    {
        if ( !IsFollow() )
        {
            GetPrev()->InvalidateSize();
            if ( GetPrev()->IsCntntFrm() )
                GetPrev()->InvalidatePage( pPage );
        }
    }
    else if ( GetNext() )
        //Bei CntntFrm's gilt es den Abstand zum Vorgaenger/Nachfolger
        //zu beachten. Faelle (beide treten immer gleichzeitig auf):
        //a) Der Cntnt wird der erste einer Kette
        //b) Der neue Nachfolger war vorher der erste einer Kette
        GetNext()->_InvalidatePrt();

    if ( pPage && !IsFollow() )
    {
        if ( pPage->GetUpper() )
            ((SwRootFrm*)pPage->GetUpper())->InvalidateBrowseWidth();

        if ( !GetPrev() )//Mindestens fuer HTML mit Tabelle am Anfang notwendig.
        {
            const SwPageDesc *pDesc = GetFmt()->GetPageDesc().GetPageDesc();
            if ( (pDesc && pDesc != pPage->GetPageDesc()) ||
                 (!pDesc && pPage->GetPageDesc() != &GetFmt()->GetDoc()->GetPageDesc(0)) )
                CheckPageDescs( pPage, TRUE );
        }
    }
}

#ifdef VERTICAL_LAYOUT
/*************************************************************************
|*
|*  SwTabFrm::Prepare()
|*
|*  Created        AMA 01/10/02
|*  Last Change    AMA 01/10/02
|*
|*************************************************************************/
void SwTabFrm::Prepare( const PrepareHint eHint, const void *, BOOL )
{
    if( PREP_BOSS_CHGD == eHint )
        CheckDirChange();
}
#endif

/*************************************************************************
|*
|*	SwRowFrm::SwRowFrm(), ~SwRowFrm()
|*
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 30. May. 96
|*
|*************************************************************************/
SwRowFrm::SwRowFrm( const SwTableLine &rLine ):
    SwLayoutFrm( rLine.GetFrmFmt() ),
    pTabLine( &rLine )
{
    nType = FRMC_ROW;

    //Gleich die Boxen erzeugen und einfuegen.
    const SwTableBoxes &rBoxes = rLine.GetTabBoxes();
    SwFrm *pPrev = 0;
    for ( USHORT i = 0; i < rBoxes.Count(); ++i )
    {
        SwCellFrm *pNew = new SwCellFrm( *rBoxes[i] );
        pNew->InsertBehind( this, pPrev );
#ifndef VERTICAL_LAYOUT
        pNew->bVarHeight = FALSE;
#endif
        pPrev = pNew;
    }
}

SwRowFrm::~SwRowFrm()
{
    SwModify* pMod = GetFmt();
    if( pMod )
    {
        pMod->Remove( this );			// austragen,
        if( !pMod->GetDepends() )
            delete pMod;				// und loeschen
    }
}

/*************************************************************************
|*
|*	SwRowFrm::RegistFlys()
|*
|*	Ersterstellung		MA 08. Jul. 93
|*	Letzte Aenderung	MA 08. Jul. 93
|*
|*************************************************************************/
void SwRowFrm::RegistFlys( SwPageFrm *pPage )
{
    ::RegistFlys( pPage ? pPage : FindPageFrm(), this );
}

/*************************************************************************
|*
|*	  SwRowFrm::Modify()
|*
|*	  Ersterstellung	MA 12. Nov. 97
|*	  Letzte Aenderung	MA 12. Nov. 97
|*
|*************************************************************************/
void SwRowFrm::Modify( SfxPoolItem * pOld, SfxPoolItem * pNew )
{
    BOOL bAttrSetChg = pNew && RES_ATTRSET_CHG == pNew->Which();
    const SfxPoolItem *pItem = 0;

    if( bAttrSetChg )
        ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState( RES_FRM_SIZE, FALSE, &pItem);
    else if ( RES_FRM_SIZE == pNew->Which() )
        pItem = pNew;

    if ( pItem )
    {
        SwTabFrm *pTab = FindTabFrm();
        if ( pTab && pTab->IsFollow() &&
             (!GetPrev() ||
              (pTab->GetTable()->IsHeadlineRepeat() && !GetPrev()->GetPrev())))
        {
            pTab->FindMaster()->InvalidatePos();
        }
    }

    SwLayoutFrm::Modify( pOld, pNew );
}



/*************************************************************************
|*
|*	SwRowFrm::MakeAll()
|*
|*	Ersterstellung		MA 01. Mar. 94
|*	Letzte Aenderung	MA 01. Mar. 94
|*
|*************************************************************************/
void SwRowFrm::MakeAll()
{
    if ( !GetNext() )
        bValidSize = FALSE;
    SwLayoutFrm::MakeAll();
}

/*************************************************************************
|*
|*	SwRowFrm::Format()
|*
|*	Ersterstellung		MA 13. Mar. 93
|*	Letzte Aenderung	MA 20. Jun. 96
|*
|*************************************************************************/
long MA_FASTCALL CalcHeightWidthFlys( const SwFrm *pFrm )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pFrm )
#endif
    long nHeight = 0;
    const SwFrm* pTmp = pFrm->IsSctFrm() ?
            ((SwSectionFrm*)pFrm)->ContainsCntnt() : pFrm;
    while( pTmp )
    {
        if ( pTmp->GetDrawObjs() )
        {
            for ( USHORT i = 0; i < pTmp->GetDrawObjs()->Count(); ++i )
            {
                const SdrObject *pO = (*pTmp->GetDrawObjs())[i];
                if ( pO->IsWriterFlyFrame() )
                {
                    const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                    if( !pFly->IsFlyInCntFrm() && pFly->Frm().Top()!=WEIT_WECH )
                    {
                        const SwFmtFrmSize &rSz = pFly->GetFmt()->GetFrmSize();
                        if( !rSz.GetHeightPercent() )
                        {
                            const SwTwips nFlyWidth =
                                (pFly->Frm().*fnRect->fnGetHeight)() +
                                    ( bVert ?
                                      pFly->GetCurRelPos().X() :
                                      pFly->GetCurRelPos().Y() );

                            const SwTwips nFrmDiff =
                                (*fnRect->fnYDiff)(
                                    (pTmp->Frm().*fnRect->fnGetTop)(),
                                    (pFrm->Frm().*fnRect->fnGetTop)() );

                            nHeight = Max( nHeight, nFlyWidth + nFrmDiff -
                                            (pFrm->Frm().*fnRect->fnGetHeight)() );
                        }
                    }
                }
            }
        }
        if( !pFrm->IsSctFrm() )
            break;
        pTmp = pTmp->FindNextCnt();
        if( !((SwSectionFrm*)pFrm)->IsAnLower( pTmp ) )
            break;
    }
    return nHeight;
}

SwTwips MA_FASTCALL lcl_CalcMinRowHeight( SwLayoutFrm *pRow );

SwTwips MA_FASTCALL lcl_CalcMinCellHeight( SwLayoutFrm *pCell,
                                  const SwBorderAttrs *pAttrs = 0 )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pCell )
#endif
    SwTwips nHeight = 0;
    SwFrm *pLow = pCell->Lower();
    if ( pLow )
    {
        long nFlyAdd = 0;
        while ( pLow )
        {
            if( pLow->IsCntntFrm() || pLow->IsSctFrm() )
            {
#ifdef VERTICAL_LAYOUT
                long nLowHeight = (pLow->Frm().*fnRect->fnGetHeight)();
                nHeight += nLowHeight;
                nFlyAdd = Max( 0L, nFlyAdd - nLowHeight );
#else
                nHeight += pLow->Frm().Height();
                nFlyAdd = Max( 0L, nFlyAdd - pLow->Frm().Height() );
#endif
                nFlyAdd = Max( nFlyAdd, ::CalcHeightWidthFlys( pLow ) );
            }
            else
                nHeight += ::lcl_CalcMinRowHeight( (SwLayoutFrm*)pLow );

            pLow = pLow->GetNext();
        }
        if ( nFlyAdd )
            nHeight += nFlyAdd;
    }
    //Der Border will natuerlich auch mitspielen, er kann leider nicht
    //aus PrtArea und Frm errechnet werden, da diese in beliebiger
    //Kombination ungueltig sein koennen.
    if ( pAttrs )
        nHeight += pAttrs->CalcTop() + pAttrs->CalcBottom();
    else
    {
        SwBorderAttrAccess aAccess( SwFrm::GetCache(), pCell );
        const SwBorderAttrs &rAttrs = *aAccess.Get();
        nHeight += rAttrs.CalcTop() + rAttrs.CalcBottom();
    }
    return nHeight;
}

SwTwips MA_FASTCALL lcl_CalcMinRowHeight( SwLayoutFrm *pRow )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pRow )
    if ( pRow->HasFixSize() )
        return (pRow->Frm().*fnRect->fnGetHeight)();
#else
    if ( pRow->HasFixSize( pHeight ) )
        return pRow->Frm().Height();
#endif

    SwTwips nHeight = 0;
    SwLayoutFrm *pLow = (SwLayoutFrm*)pRow->Lower();
    while ( pLow )
    {
        SwTwips nTmp = ::lcl_CalcMinCellHeight( pLow );
        if ( nTmp > nHeight )
            nHeight = nTmp;
        pLow = (SwLayoutFrm*)pLow->GetNext();
    }
    const SwFmtFrmSize &rSz = pRow->GetFmt()->GetFrmSize();
    if ( rSz.GetSizeType() == ATT_MIN_SIZE )
        nHeight = Max( nHeight, rSz.GetHeight() );
    return nHeight;
}

void SwRowFrm::Format( const SwBorderAttrs *pAttrs )
{
#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
#endif
    ASSERT( pAttrs, "SwRowFrm::Format ohne Attrs." );

    const BOOL bFix = BFIXHEIGHT;

    if ( !bValidPrtArea )
    {
        //RowFrms haben keine Umrandung usw. also entspricht die PrtArea immer
        //dem Frm.
        bValidPrtArea = TRUE;
        aPrt.Left( 0 );
        aPrt.Top( 0 );
        aPrt.Width ( aFrm.Width() );
        aPrt.Height( aFrm.Height() );
    }

    while ( !bValidSize )
    {
        bValidSize = TRUE;

#ifdef VERTICAL_LAYOUT
#ifndef PRODUCT
        if ( HasFixSize() )
        {
            const SwFmtFrmSize &rFrmSize = GetFmt()->GetFrmSize();
            ASSERT( rFrmSize.GetSize().Height() > 0, "Hat ihn" );
        }
#endif
        const SwTwips nDiff = (Frm().*fnRect->fnGetHeight)() - (HasFixSize() ?
                                                pAttrs->GetSize().Height() :
                                                ::lcl_CalcMinRowHeight( this ));
#else
#ifndef PRODUCT
        if ( HasFixSize( pHeight ) )
        {
            const SwFmtFrmSize &rFrmSize = GetFmt()->GetFrmSize();
            ASSERT( rFrmSize.GetSize().Height() > 0, "Hat ihn" );
        }
#endif
        const SwTwips nDiff = Frm().Height() - (HasFixSize( pHeight ) ?
                                                    pAttrs->GetSize().Height() :
                                                    ::lcl_CalcMinRowHeight( this ));
#endif
        if ( nDiff )
        {
            BFIXHEIGHT = FALSE;
            if ( nDiff > 0 )
                Shrink( nDiff PHEIGHT, FALSE, TRUE );
            else if ( nDiff < 0 )
                Grow( -nDiff PHEIGHT );
            BFIXHEIGHT = bFix;
        }
    }
    if ( !GetNext() )
    {
        //Der letzte fuellt den verbleibenden Raum im Upper aus.
#ifdef VERTICAL_LAYOUT
        SwTwips nDiff = (GetUpper()->Prt().*fnRect->fnGetHeight)();
        SwFrm *pSibling = GetUpper()->Lower();
        do
        {   nDiff -= (pSibling->Frm().*fnRect->fnGetHeight)();
#else
        SwTwips nDiff = GetUpper()->Prt().Height();
        SwFrm *pSibling = GetUpper()->Lower();
        do
        {	nDiff -= pSibling->Frm().Height();
#endif
            pSibling = pSibling->GetNext();
        } while ( pSibling );
        if ( nDiff > 0 )
        {
            BFIXHEIGHT = FALSE;
            Grow( nDiff PHEIGHT );
            BFIXHEIGHT = bFix;
            bValidSize = TRUE;
        }
    }
}

/*************************************************************************
|*
|*	SwRowFrm::AdjustCells()
|*
|*	Ersterstellung		MA 10. Aug. 93
|*	Letzte Aenderung	MA 16. Dec. 96
|*
|*************************************************************************/
void SwRowFrm::AdjustCells( const SwTwips nHeight, const BOOL bHeight )
{
    SwFrm *pFrm = Lower();
    if ( bHeight )
    {
#ifdef ACCESSIBLE_LAYOUT
        SwRootFrm *pRootFrm = 0;
#endif
#ifdef VERTICAL_LAYOUT
        SWRECTFN( this )
        while ( pFrm )
        {
            long nDiff = nHeight - (pFrm->Frm().*fnRect->fnGetHeight)();
            if( nDiff )
            {
#ifdef ACCESSIBLE_LAYOUT
                SwRect aOldFrm( pFrm->Frm() );
#endif
                (pFrm->Frm().*fnRect->fnAddBottom)( nDiff );
#ifdef ACCESSIBLE_LAYOUT
                if( !pRootFrm )
                    pRootFrm = FindRootFrm();
                if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
                    pRootFrm->GetCurrShell() )
                {
                    pRootFrm->GetCurrShell()->Imp()->MoveAccessibleFrm( pFrm, aOldFrm );
                }
#endif
#else
        while ( pFrm )
        {	if ( pFrm->Frm().Height() != nHeight )
            {
                pFrm->Frm().Height( nHeight );
#endif
                pFrm->_InvalidatePrt();
            }
            pFrm = pFrm->GetNext();
        }
    }
    else
    {	while ( pFrm )
        {
            pFrm->_InvalidateAll();
            pFrm = pFrm->GetNext();
        }
    }
    InvalidatePage();
}

/*************************************************************************
|*
|*	SwRowFrm::Cut()
|*
|*	Ersterstellung		MA 12. Nov. 97
|*	Letzte Aenderung	MA 12. Nov. 97
|*
|*************************************************************************/
void SwRowFrm::Cut()
{
    SwTabFrm *pTab = FindTabFrm();
    if ( pTab && pTab->IsFollow() &&
         (!GetPrev() ||
          (pTab->GetTable()->IsHeadlineRepeat() && !GetPrev()->GetPrev())))
    {
        pTab->FindMaster()->InvalidatePos();
    }
    SwLayoutFrm::Cut();
}

/*************************************************************************
|*
|*	SwRowFrm::GrowFrm()
|*
|*	Ersterstellung		MA 15. Mar. 93
|*	Letzte Aenderung	MA 05. May. 94
|*
|*************************************************************************/

#ifdef VERTICAL_LAYOUT

SwTwips SwRowFrm::GrowFrm( SwTwips nDist, BOOL bTst, BOOL bInfo )
{
    const SwTwips nReal = SwLayoutFrm::GrowFrm( nDist, bTst, bInfo);

    //Hoehe der Zellen auf den neuesten Stand bringen.
    if ( !bTst )
    {
        SWRECTFN( this )
        AdjustCells( (Prt().*fnRect->fnGetHeight)() + nReal, TRUE );
        if ( nReal )
            SetCompletePaint();
    }
    return nReal;
}
/*************************************************************************
|*
|*	SwRowFrm::ShrinkFrm()
|*
|*	Ersterstellung		MA 15. Mar. 93
|*	Letzte Aenderung	MA 20. Jun. 96
|*
|*************************************************************************/
SwTwips SwRowFrm::ShrinkFrm( SwTwips nDist, BOOL bTst, BOOL bInfo )
{
    SWRECTFN( this )
    if( HasFixSize() )
    {
        AdjustCells( (Prt().*fnRect->fnGetHeight)(), TRUE );
        return 0L;
    }

    //bInfo wird ggf. vom SwRowFrm::Format auf TRUE gesetzt, hier muss dann
    //entsprechend reagiert werden
    const BOOL bShrinkAnyway = bInfo;

    //Nur soweit Shrinken, wie es der Inhalt der groessten Zelle zulaesst.
    SwTwips nRealDist = nDist;
    {
        const SwFmtFrmSize &rSz = GetFmt()->GetFrmSize();
        SwTwips nMinHeight = rSz.GetSizeType() == ATT_MIN_SIZE ?
                             rSz.GetHeight() : 0;
        SwLayoutFrm *pCell = (SwLayoutFrm*)Lower();
        if( nMinHeight < (Frm().*fnRect->fnGetHeight)() )
        {
            SwLayoutFrm *pCell = (SwLayoutFrm*)Lower();
            while ( pCell )
            {
                SwTwips nAct = ::lcl_CalcMinCellHeight( pCell );
                if ( nAct > nMinHeight )
                    nMinHeight = nAct;
                if ( nMinHeight >= (Frm().*fnRect->fnGetHeight)() )
                    break;
                pCell = (SwLayoutFrm*)pCell->GetNext();
            }
        }
        if ( ((Frm().*fnRect->fnGetHeight)() - nRealDist) < nMinHeight )
            nRealDist = (Frm().*fnRect->fnGetHeight)() - nMinHeight;
    }
    if ( nRealDist < 0 )
        nRealDist = 0;

    SwTwips nReal = nRealDist;
    if ( nReal )
    {
        if ( !bTst )
        {
            SwTwips nHeight = (Frm().*fnRect->fnGetHeight)();
            (Frm().*fnRect->fnSetHeight)( nHeight - nReal );
            if( IsVertical() && !bRev )
                Frm().Pos().X() += nReal;
        }

        SwTwips nTmp = GetUpper()->Shrink( nReal, bTst );
        if ( !bShrinkAnyway && !GetNext() && nTmp != nReal )
        {
            //Der letzte bekommt den Rest im Upper und nimmt deshalb
            //ggf. Ruecksichten (sonst: Endlosschleife)
            if ( !bTst )
            {
                nReal -= nTmp;
                SwTwips nHeight = (Frm().*fnRect->fnGetHeight)();
                (Frm().*fnRect->fnSetHeight)( nHeight + nReal );
                if( IsVertical() && !bRev )
                    Frm().Pos().X() -= nReal;
            }
            nReal = nTmp;
        }
    }

    //Geeignet invalidieren und die Hoehe der Zellen auf den neuesten
    //Stand bringen.
    if ( !bTst )
    {
        if ( nReal )
        {
            if ( GetNext() )
                GetNext()->_InvalidatePos();
            _InvalidateAll();
            SetCompletePaint();

            SwTabFrm *pTab = FindTabFrm();
            if ( pTab->IsFollow() &&
                 (!GetPrev() ||
                  (pTab->GetTable()->IsHeadlineRepeat() && !GetPrev()->GetPrev())))
            {
                pTab->FindMaster()->InvalidatePos();
            }
        }
        AdjustCells( (Prt().*fnRect->fnGetHeight)() - nReal, TRUE );
    }
    return nReal;
}

#else

SwTwips SwRowFrm::GrowFrm( SwTwips nDist, const SzPtr pDirection,
                           BOOL bTst, BOOL bInfo )
{
    const SwTwips nReal = SwLayoutFrm::GrowFrm( nDist, pDirection, bTst, bInfo);

    //Hoehe der Zellen auf den neuesten Stand bringen.
    if ( !bTst )
    {
        AdjustCells( Prt().SSize().*pDirection + nReal,
             pDirection == pHeight ? TRUE : FALSE );
        if ( nReal )
            SetCompletePaint();
    }
    return nReal;
}
/*************************************************************************
|*
|*	SwRowFrm::ShrinkFrm()
|*
|*	Ersterstellung		MA 15. Mar. 93
|*	Letzte Aenderung	MA 20. Jun. 96
|*
|*************************************************************************/
SwTwips SwRowFrm::ShrinkFrm( SwTwips nDist, const SzPtr pDirection,
                             BOOL bTst, BOOL bInfo )
{
    BOOL bVariable =
        pDirection == pHeight;
    if ( HasFixSize( pDirection ) )
    {
        AdjustCells( Prt().SSize().*pDirection, bVariable );
        return 0L;
    }

    //bInfo wird ggf. vom SwRowFrm::Format auf TRUE gesetzt, hier muss dann
    //entsprechend reagiert werden
    const BOOL bShrinkAnyway = bInfo;

    //Nur soweit Shrinken, wie es der Inhalt der groessten Zelle zulaesst.
    SwTwips nRealDist = nDist;
    if ( bVariable )
    {
        const SwFmtFrmSize &rSz = GetFmt()->GetFrmSize();
        SwTwips nMinHeight = rSz.GetSizeType() == ATT_MIN_SIZE ? rSz.GetHeight() : 0;
        SwLayoutFrm *pCell = (SwLayoutFrm*)Lower();
        if ( nMinHeight < Frm().SSize().*pDirection )
        {
            SwLayoutFrm *pCell = (SwLayoutFrm*)Lower();
            while ( pCell )
            {
                SwTwips nAct = ::lcl_CalcMinCellHeight( pCell );
                if ( nAct > nMinHeight )
                    nMinHeight = nAct;
                if ( nMinHeight >= Frm().SSize().*pDirection )
                    break;
                pCell = (SwLayoutFrm*)pCell->GetNext();
            }
        }
        if ( (Frm().SSize().*pDirection - nRealDist) < nMinHeight )
            nRealDist = Frm().SSize().*pDirection - nMinHeight;
    }
    if ( nRealDist < 0 )
        nRealDist = 0;

    SwTwips nReal = nRealDist;
    if ( nReal )
    {
        if ( !bTst )
            Frm().SSize().*pDirection -= nReal;

        SwTwips nTmp = GetUpper()->Shrink( nReal, pDirection, bTst );
        if ( !bShrinkAnyway && !GetNext() && nTmp != nReal )
        {
            //Der letzte bekommt den Rest im Upper und nimmt deshalb
            //ggf. Ruecksichten (sonst: Endlosschleife)
            if ( !bTst )
            {
                nReal -= nTmp;
                Frm().SSize().*pDirection += nReal;
            }
            nReal = nTmp;
        }
    }

    //Geeignet invalidieren und die Hoehe der Zellen auf den neuesten
    //Stand bringen.
    if ( !bTst )
    {
        if ( nReal )
        {
            if ( GetNext() )
                GetNext()->_InvalidatePos();
            _InvalidateAll();
            SetCompletePaint();

            SwTabFrm *pTab = FindTabFrm();
            if ( pTab->IsFollow() &&
                 (!GetPrev() ||
                  (pTab->GetTable()->IsHeadlineRepeat() && !GetPrev()->GetPrev())))
            {
                pTab->FindMaster()->InvalidatePos();
            }
        }
        AdjustCells( Prt().SSize().*pDirection - nReal, bVariable );
    }
    return nReal;
}


#endif

/*************************************************************************
|*
|*	SwCellFrm::SwCellFrm(), ~SwCellFrm()
|*
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 30. May. 96
|*
|*************************************************************************/
SwCellFrm::SwCellFrm( const SwTableBox &rBox ) :
    SwLayoutFrm( rBox.GetFrmFmt() ),
    pTabBox( &rBox )
{
    nType = FRMC_CELL;

    //Wenn ein StartIdx vorhanden ist, so werden CntntFrms in der Zelle
    //angelegt, andernfalls muessen Rows vorhanden sein und diese werden
    //angelegt.
    if ( rBox.GetSttIdx() )
    {
        ULONG nIndex = rBox.GetSttIdx();
        ::_InsertCnt( this, rBox.GetFrmFmt()->GetDoc(), ++nIndex );
    }
    else
    {	const SwTableLines &rLines = rBox.GetTabLines();
        SwFrm *pPrev = 0;
        for ( USHORT i = 0; i < rLines.Count(); ++i )
        {
            SwRowFrm *pNew = new SwRowFrm( *rLines[i] );
            pNew->InsertBehind( this, pPrev );
            pPrev = pNew;
        }
    }
}

SwCellFrm::~SwCellFrm()
{
    SwModify* pMod = GetFmt();
    if( pMod )
    {
#ifdef ACCESSIBLE_LAYOUT
        // At this stage the lower frames aren't destroyed already,
        // therfor we have to do a recursive dispose.
        SwRootFrm *pRootFrm = FindRootFrm();
        if( pRootFrm && pRootFrm->IsAnyShellAccessible() &&
            pRootFrm->GetCurrShell() )
        {
            pRootFrm->GetCurrShell()->Imp()->DisposeAccessibleFrm( this, sal_True );
        }
#endif
        pMod->Remove( this );			// austragen,
        if( !pMod->GetDepends() )
            delete pMod;				// und loeschen
    }
}

/*************************************************************************
|*
|*	SwCellFrm::Format()
|*
|*	Ersterstellung		MA 09. Mar. 93
|*	Letzte Aenderung	MA 29. Jan. 98
|*
|*************************************************************************/
BOOL lcl_ArrangeLowers( SwLayoutFrm *pLay, long lYStart, BOOL bInva )
{
    BOOL bRet = FALSE;
    SwFrm *pFrm = pLay->Lower();
    SwPageFrm* pPg = NULL;
#ifdef VERTICAL_LAYOUT
    SWRECTFN( pLay )
    while ( pFrm )
    {
        long nFrmTop = (pFrm->Frm().*fnRect->fnGetTop)();
        if( nFrmTop != lYStart )
        {
            bRet = TRUE;
            const long lDiff = (*fnRect->fnYDiff)( lYStart, nFrmTop );
            const long lDiffX = lYStart - nFrmTop;
            (pFrm->Frm().*fnRect->fnSubTop)( -lDiff );
            (pFrm->Frm().*fnRect->fnAddBottom)( lDiff );
            pFrm->SetCompletePaint();
            if ( !pFrm->GetNext() )
                pFrm->SetRetouche();
            if( bInva )
                pFrm->Prepare( PREP_POS_CHGD );
            if ( pFrm->IsLayoutFrm() && ((SwLayoutFrm*)pFrm)->Lower() )
                lcl_ArrangeLowers( (SwLayoutFrm*)pFrm,
                    (((SwLayoutFrm*)pFrm)->Lower()->Frm().*fnRect->fnGetTop)()
                    + lDiffX, bInva );
            if ( pFrm->GetDrawObjs() )
            {
                for ( USHORT i = 0; i < pFrm->GetDrawObjs()->Count(); ++i )
                {
                    SdrObject *pO = (*pFrm->GetDrawObjs())[i];
                    if ( pO->IsWriterFlyFrame() )
                    {
                        SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                        if( WEIT_WECH != pFly->Frm().Top() )
                        {
                            (pFly->Frm().*fnRect->fnSubTop)( -lDiff );
                            (pFly->Frm().*fnRect->fnAddBottom)( lDiff );
                        }
                        pFly->GetVirtDrawObj()->_SetRectsDirty();
                        if ( pFly->IsFlyInCntFrm() )
                            ((SwFlyInCntFrm*)pFly)->AddRefOfst( lDiff );
                        else
                        {
                            if( !pPg )
                                pPg = pLay->FindPageFrm();
                            SwPageFrm* pOld = pFly->FindPageFrm();
                            if( pPg != pOld )
                                pOld->MoveFly( pFly, pPg );
                            if( pFly->IsAutoPos() )
                                ((SwFlyAtCntFrm*)pFly)->AddLastCharY( lDiff );
                        }
                        if( ::lcl_ArrangeLowers( pFly,
                            (pFly->*fnRect->fnGetPrtTop)(), bInva ) )
                            pFly->SetCompletePaint();
                    }
                    else
                        pO->SetAnchorPos( pFrm->GetAnchorPos() );
                }
            }
        }
        // Columns and cells are ordered horizontal, not vertical
        if( !pFrm->IsColumnFrm() && !pFrm->IsCellFrm() )
            lYStart = (*fnRect->fnYInc)( lYStart,
                                        (pFrm->Frm().*fnRect->fnGetHeight)() );
        pFrm = pFrm->GetNext();
    }
#else
    while ( pFrm )
    {
        if ( pFrm->Frm().Top() != lYStart )
        {
            bRet = TRUE;
            const long lDiff = lYStart - pFrm->Frm().Top();
            pFrm->Frm().Pos().Y() = lYStart;
            pFrm->SetCompletePaint();
            if ( !pFrm->GetNext() )
                pFrm->SetRetouche();
            if( bInva )
                pFrm->Prepare( PREP_POS_CHGD );
            if ( pFrm->IsLayoutFrm() && ((SwLayoutFrm*)pFrm)->Lower() )
                lcl_ArrangeLowers( (SwLayoutFrm*)pFrm,
                    ((SwLayoutFrm*)pFrm)->Lower()->Frm().Top()+lDiff, bInva );
            if ( pFrm->GetDrawObjs() )
            {
                for ( USHORT i = 0; i < pFrm->GetDrawObjs()->Count(); ++i )
                {
                    SdrObject *pO = (*pFrm->GetDrawObjs())[i];
                    if ( pO->IsWriterFlyFrame() )
                    {
                        SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                        if ( WEIT_WECH != pFly->Frm().Top() )
                            pFly->Frm().Pos().Y() += lDiff;
                        pFly->GetVirtDrawObj()->_SetRectsDirty();
                        if ( pFly->IsFlyInCntFrm() )
                            ((SwFlyInCntFrm*)pFly)->AddRefOfst( lDiff );
                        else
                        {
                            if( !pPg )
                                pPg = pLay->FindPageFrm();
                            SwPageFrm* pOld = pFly->FindPageFrm();
                            if( pPg != pOld )
                                pOld->MoveFly( pFly, pPg );
                            if( pFly->IsAutoPos() )
                                ((SwFlyAtCntFrm*)pFly)->AddLastCharY( lDiff );
                        }
                        if ( ::lcl_ArrangeLowers( pFly, pFly->Frm().Top() +
                                pFly->Prt().Top(), bInva ) )
                            pFly->SetCompletePaint();
                    }
                    else
                        pO->SetAnchorPos( pFrm->Frm().Pos() );
                }
            }
        }
        // Columns and cells are ordered horizontal, not vertical
        if( !pFrm->IsColumnFrm() && !pFrm->IsCellFrm() )
            lYStart += pFrm->Frm().Height();
        pFrm = pFrm->GetNext();
    }
#endif
    return bRet;
}

void SwCellFrm::Format( const SwBorderAttrs *pAttrs )
{
    ASSERT( pAttrs, "CellFrm::Format, pAttrs ist 0." );
#ifdef VERTICAL_LAYOUT
    SWRECTFN( this )
    if ( !bValidPrtArea )
    {
        bValidPrtArea = TRUE;

        //Position einstellen.
        long nTmp1 = pAttrs->CalcLeft( this );
        long nTmp2 = pAttrs->CalcRight();
        (this->*fnRect->fnSetXMargins)( nTmp1, nTmp2 );
        nTmp1 = pAttrs->CalcTop();
        nTmp2 = pAttrs->CalcBottom();
        (this->*fnRect->fnSetYMargins)( nTmp1, nTmp2 );
    }
#else
    if ( !bValidPrtArea )
    {
        bValidPrtArea = TRUE;

        //Position einstellen.
        aPrt.Left( pAttrs->CalcLeft( this ) );
        aPrt.Top(  pAttrs->CalcTop() );

        //Sizes einstellen; die Groesse gibt der umgebende Frm vor, die
        //die Raender werden einfach abgezogen.
        aPrt.Width ( aFrm.Width() - (aPrt.Left() + pAttrs->CalcRight()) );
        aPrt.Height( aFrm.Height()- (aPrt.Top()	 + pAttrs->CalcBottom()));
    }
#endif
    long nRemaining = ::lcl_CalcMinCellHeight( this, pAttrs );
    if ( !bValidSize )
    {
        bValidSize = TRUE;

        //Die VarSize der CellFrms ist immer die Breite.
        //Tatsaechlich ist die Breite jedoch nicht Variabel, sie wird durch das
        //Format vorgegeben. Dieser Vorgegebene Wert muss aber nun wiederum
        //nicht der tatsaechlichen Breite entsprechen. Die Breite wird auf
        //Basis des Attributes errechnet, der Wert im Attribut passt zu dem
        //gewuenschten Wert des TabFrms. Anpassungen die dort vorgenommen
        //wurden werden hier Proportional beruecksichtigt.
        //Wenn die Celle keinen Nachbarn mehr hat beruecksichtigt sie nicht
        //die Attribute, sonder greift sich einfach den Rest des
        //Uppers.
        SwTwips nWidth;
        if ( GetNext() )
        {
            const SwTabFrm *pTab = FindTabFrm();
            SwTwips nWish = pTab->GetFmt()->GetFrmSize().GetWidth();
            nWidth = pAttrs->GetSize().Width();

            ASSERT( nWish, "Tabelle ohne Breite?" );
            ASSERT( nWidth <= nWish, "Zelle breiter als Tabelle." );
            ASSERT( nWidth > 0, "Box without width" );

#ifdef VERTICAL_LAYOUT
            long nPrtWidth = (pTab->Prt().*fnRect->fnGetWidth)();
            if ( nWish != nPrtWidth )
            {
                nWidth *= nPrtWidth;
                nWidth /= nWish;
            }
        }
        else
        {
            ASSERT( pAttrs->GetSize().Width() > 0, "Box without width" );
            nWidth = (GetUpper()->Prt().*fnRect->fnGetWidth)();
            SwFrm *pPre = GetUpper()->Lower();
            while ( pPre != this )
            {   nWidth -= (pPre->Frm().*fnRect->fnGetWidth)();
                pPre = pPre->GetNext();
            }
        }
        const long nDiff = nWidth - (Frm().*fnRect->fnGetWidth)();
        if( IsNeighbourFrm() && IsRightToLeft() )
            (Frm().*fnRect->fnSubLeft)( nDiff );
        else
            (Frm().*fnRect->fnAddRight)( nDiff );
        (Prt().*fnRect->fnAddRight)( nDiff );

        //Jetzt die Hoehe einstellen, sie wird vom Inhalt und den Raendern
        //bestimmt.
        const long nDiffHeight = nRemaining - (Frm().*fnRect->fnGetHeight)();
#else
            if ( nWish != pTab->Prt().Width() )
            {
                nWidth *= pTab->Prt().Width();
                nWidth /= nWish;
            }
        }
        else
        {
            ASSERT( pAttrs->GetSize().Width() > 0, "Box without width" );
            nWidth = GetUpper()->Prt().Width();
            SwFrm *pPre = GetUpper()->Lower();
            while ( pPre != this )
            {	nWidth -= pPre->Frm().Width();
                pPre = pPre->GetNext();
            }
        }
        const long nDiff = nWidth - Frm().Width();
        Frm().Width( nWidth );
        Prt().Width( Prt().Width() + nDiff );

        //Jetzt die Hoehe einstellen, sie wird vom Inhalt und den Raendern
        //bestimmt.
        const long nDiffHeight = nRemaining - Frm().Height();
#endif
        if ( nDiffHeight )
        {
            if ( nDiffHeight > 0 )
            {
                //Wieder validieren wenn kein Wachstum stattgefunden hat.
                //Invalidiert wird durch AdjustCells von der Row.
                if ( !Grow( nDiffHeight PHEIGHT ) )
                    bValidSize = bValidPrtArea = TRUE;
            }
            else
            {
                //Nur dann invalidiert lassen, wenn tatsaechlich
                //geshrinkt wurde; das kann abgelehnt werden, weil alle
                //nebeneinanderliegenden Zellen gleichgross sein muessen.
                if ( !Shrink( -nDiffHeight PHEIGHT ) )
                    bValidSize = bValidPrtArea = TRUE;
            }
        }
    }
    const SwFmtVertOrient &rOri = pAttrs->GetAttrSet().GetVertOrient();
    if ( VERT_NONE != rOri.GetVertOrient() )
    {
        if ( !Lower()->IsCntntFrm() && !Lower()->IsSctFrm() )
        {
            //ASSERT fuer HTML-Import!
            ASSERT( !this, "VAlign an Zelle ohne Inhalt" );
            return;
        }
        BOOL bVertDir = TRUE;
        //Keine Ausrichtung wenn Rahmen mit Umlauf in die Zelle ragen.
        SwPageFrm *pPg = FindPageFrm();
        if ( pPg->GetSortedObjs() )
        {
            SwRect aRect( Prt() ); aRect += Frm().Pos();
            for ( USHORT i = 0; i < pPg->GetSortedObjs()->Count(); ++i )
            {
                const SdrObject *pObj = (*pPg->GetSortedObjs())[i];
                SwRect aTmp( pObj->GetBoundRect() );
                if ( aTmp.IsOver( aRect ) )
                {
                    SdrObjUserCall *pUserCall;
                    const SwFmtSurround &rSur = ((SwContact*)
                               (pUserCall=GetUserCall(pObj)))->GetFmt()->GetSurround();
                    if ( SURROUND_THROUGHT != rSur.GetSurround() )
                    {
                        const SwFrm *pAnch;
                        if ( pObj->IsWriterFlyFrame() )
                        {
                            const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
                            if ( pFly->IsAnLower( this ) )
                                continue;
                            pAnch = pFly->GetAnchor();
                        }
                        else
                            pAnch = ((SwDrawContact*)pUserCall)->GetAnchor();
                        if ( !IsAnLower( pAnch ) )
                        {
                            bVertDir = FALSE;
                            break;
                        }
                    }
                }
            }
        }
#ifdef VERTICAL_LAYOUT
        long nPrtHeight = (Prt().*fnRect->fnGetHeight)();
        if( ( bVertDir && ( nRemaining -= (pAttrs->CalcTop() +
                            pAttrs->CalcBottom())) < nPrtHeight ) ||
            (Lower()->Frm().*fnRect->fnGetTop)() !=
            (this->*fnRect->fnGetPrtTop)() )
        {
            long lTopOfst = 0,
                    nDiff = (Prt().*fnRect->fnGetHeight)() - nRemaining;
#else
        if ( (bVertDir &&
                (nRemaining -= (pAttrs->CalcTop() + pAttrs->CalcBottom())) < Prt().Height()) ||
                Lower()->Frm().Top() > Frm().Top()+Prt().Top() )
        {
            long lTopOfst = 0,
                    nDiff = Prt().Height() - nRemaining;
#endif
            if ( nDiff >= 0 )
            {
                if ( bVertDir )
                {
                    switch ( rOri.GetVertOrient() )
                    {
                        case VERT_CENTER:	lTopOfst = nDiff / 2; break;
                        case VERT_BOTTOM:	lTopOfst = nDiff;	  break;
                    };
                }
#ifdef VERTICAL_LAYOUT
                long nTmp = (*fnRect->fnYInc)(
                                    (this->*fnRect->fnGetPrtTop)(), lTopOfst );
                if ( lcl_ArrangeLowers( this, nTmp, !bVertDir ) )
#else
                if ( lcl_ArrangeLowers( this, Frm().Top()+Prt().Top()+lTopOfst,
                      !bVertDir ) )
#endif
                    SetCompletePaint();
            }
        }
    }
    else
    {
        //Ist noch eine alte Ausrichtung beruecksichtigt worden?
        if ( Lower()->IsCntntFrm() )
        {
#ifdef VERTICAL_LAYOUT
            const long lYStart = (this->*fnRect->fnGetPrtTop)();
#else
            const long lYStart = Frm().Top()+Prt().Top();
#endif
            lcl_ArrangeLowers( this, lYStart, TRUE );
        }
    }
}

/*************************************************************************
|*
|*	  SwCellFrm::Modify()
|*
|*	  Ersterstellung	MA 20. Dec. 96
|*	  Letzte Aenderung	MA 20. Dec. 96
|*
|*************************************************************************/
void SwCellFrm::Modify( SfxPoolItem * pOld, SfxPoolItem * pNew )
{
    BOOL bAttrSetChg = pNew && RES_ATTRSET_CHG == pNew->Which();
    const SfxPoolItem *pItem = 0;

    if( bAttrSetChg )
        ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState( RES_VERT_ORIENT, FALSE, &pItem);
    else if ( RES_VERT_ORIENT == pNew->Which() )
        pItem = pNew;

    if ( pItem )
    {
        BOOL bInva = TRUE;
        if ( VERT_NONE == ((SwFmtVertOrient*)pItem)->GetVertOrient() &&
             Lower()->IsCntntFrm() )
        {
#ifdef VERTICAL_LAYOUT
            SWRECTFN( this )
            const long lYStart = (this->*fnRect->fnGetPrtTop)();
#else
            const long lYStart = Frm().Top()+Prt().Top();
#endif
            bInva = lcl_ArrangeLowers( this, lYStart, FALSE );
        }
        if ( bInva )
        {
            SetCompletePaint();
            InvalidatePrt();
        }
    }

#ifdef ACCESSIBLE_LAYOUT
    if( (bAttrSetChg &&
         SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->GetItemState( RES_PROTECT, FALSE )) ||
        RES_PROTECT == pNew->Which() )
    {
        ViewShell *pSh = GetShell();
        if( pSh && pSh->GetLayout()->IsAnyShellAccessible() )
            pSh->Imp()->InvalidateAccessibleEditableState( sal_True, this );
    }
#endif

    SwLayoutFrm::Modify( pOld, pNew );
}

