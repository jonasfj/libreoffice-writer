/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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


#include "ndtxt.hxx"
#include "flyfrm.hxx"
#include "paratr.hxx"
#include "errhdl.hxx"
#include <vcl/outdev.hxx>
#include <svx/paravertalignitem.hxx>

#include "pormulti.hxx"
#include <pagefrm.hxx>
#include <pagedesc.hxx> // SwPageDesc
#include <tgrditem.hxx>
#include <porfld.hxx>

#include "txtcfg.hxx"
#include "itrtxt.hxx"
#include "txtfrm.hxx"
#include "porfly.hxx"

#if OSL_DEBUG_LEVEL > 1
# include "txtfrm.hxx"      // GetFrmID,
#endif

/*************************************************************************
 *						SwTxtIter::CtorInitTxtIter()
 *************************************************************************/

void SwTxtIter::CtorInitTxtIter( SwTxtFrm *pNewFrm, SwTxtInfo *pNewInf )
{
#ifdef DBGTXT
    // nStopAt laesst sich vom CV bearbeiten.
    static MSHORT nStopAt = 0;
    if( nStopAt == pNewFrm->GetFrmId() )
    {
        int i = pNewFrm->GetFrmId();
    }
#endif

    SwTxtNode *pNode = pNewFrm->GetTxtNode();

    ASSERT( pNewFrm->GetPara(), "No paragraph" );

    CtorInitAttrIter( *pNode, pNewFrm->GetPara()->GetScriptInfo(), pNewFrm );

    pFrm = pNewFrm;
    pInf = pNewInf;
    // --> OD 2008-01-17 #newlistlevelattrs#
    aLineInf.CtorInitLineInfo( pNode->GetSwAttrSet(), *pNode );
    // <--
    nFrameStart = pFrm->Frm().Pos().Y() + pFrm->Prt().Pos().Y();
    SwTxtIter::Init();
    if( pNode->GetSwAttrSet().GetRegister().GetValue() )
        bRegisterOn = pFrm->FillRegister( nRegStart, nRegDiff );
    else
        bRegisterOn = sal_False;
}

/*************************************************************************
 *                      SwTxtIter::Init()
 *************************************************************************/

void SwTxtIter::Init()
{
    pCurr = pInf->GetParaPortion();
    nStart = pInf->GetTxtStart();
    nY = nFrameStart;
    bPrev = sal_True;
    pPrev = 0;
    nLineNr = 1;
}

/*************************************************************************
 *				   SwTxtIter::_GetHeightAndAscent()
 *************************************************************************/

void SwTxtIter::CalcAscentAndHeight( KSHORT &rAscent, KSHORT &rHeight ) const
{
    rHeight = GetLineHeight();
    rAscent = pCurr->GetAscent() + rHeight - pCurr->Height();
}

/*************************************************************************
 *					  SwTxtIter::_GetPrev()
 *************************************************************************/

SwLineLayout *SwTxtIter::_GetPrev()
{
    pPrev = 0;
    bPrev = sal_True;
    SwLineLayout *pLay = pInf->GetParaPortion();
    if( pCurr == pLay )
        return 0;
    while( pLay->GetNext() != pCurr )
        pLay = pLay->GetNext();
    return pPrev = pLay;
}

/*************************************************************************
 *                    SwTxtIter::GetPrev()
 *************************************************************************/

const SwLineLayout *SwTxtIter::GetPrev()
{
    if(! bPrev)
        _GetPrev();
    return pPrev;
}

/*************************************************************************
 *                    SwTxtIter::Prev()
 *************************************************************************/

const SwLineLayout *SwTxtIter::Prev()
{
    if( !bPrev )
        _GetPrev();
    if( pPrev )
    {
        bPrev = sal_False;
        pCurr = pPrev;
        nStart = nStart - pCurr->GetLen();
        nY = nY - GetLineHeight();
        if( !pCurr->IsDummy() && !(--nLineNr) )
            ++nLineNr;
        return pCurr;
    }
    else
        return 0;
}

/*************************************************************************
 *                      SwTxtIter::Next()
 *************************************************************************/

const SwLineLayout *SwTxtIter::Next()
{
    if(pCurr->GetNext())
    {
        pPrev = pCurr;
        bPrev = sal_True;
        nStart = nStart + pCurr->GetLen();
        nY += GetLineHeight();
        if( pCurr->GetLen() || ( nLineNr>1 && !pCurr->IsDummy() ) )
            ++nLineNr;
        return pCurr = pCurr->GetNext();
    }
    else
        return 0;
}

/*************************************************************************
 *                      SwTxtIter::NextLine()
 *************************************************************************/

const SwLineLayout *SwTxtIter::NextLine()
{
    const SwLineLayout *pNext = Next();
    while( pNext && pNext->IsDummy() && pNext->GetNext() )
    {
        DBG_LOOP;
        pNext = Next();
    }
    return pNext;
}

/*************************************************************************
 *						SwTxtIter::GetNextLine()
 *************************************************************************/

const SwLineLayout *SwTxtIter::GetNextLine() const
{
    const SwLineLayout *pNext = pCurr->GetNext();
    while( pNext && pNext->IsDummy() && pNext->GetNext() )
    {
        DBG_LOOP;
        pNext = pNext->GetNext();
    }
    return (SwLineLayout*)pNext;
}

/*************************************************************************
 *						SwTxtIter::GetPrevLine()
 *************************************************************************/

const SwLineLayout *SwTxtIter::GetPrevLine()
{
    const SwLineLayout *pRoot = pInf->GetParaPortion();
    if( pRoot == pCurr )
        return 0;
    const SwLineLayout *pLay = pRoot;

    while( pLay->GetNext() != pCurr )
        pLay = pLay->GetNext();

    if( pLay->IsDummy() )
    {
        const SwLineLayout *pTmp = pRoot;
        pLay = pRoot->IsDummy() ? 0 : pRoot;
        while( pTmp->GetNext() != pCurr )
        {
            if( !pTmp->IsDummy() )
                pLay = pTmp;
            pTmp = pTmp->GetNext();
        }
    }

    // Wenn sich nichts getan hat, dann gibt es nur noch Dummys
    return (SwLineLayout*)pLay;
}

/*************************************************************************
 *                      SwTxtIter::PrevLine()
 *************************************************************************/

const SwLineLayout *SwTxtIter::PrevLine()
{
    const SwLineLayout *pMyPrev = Prev();
    if( !pMyPrev )
        return 0;

    const SwLineLayout *pLast = pMyPrev;
    while( pMyPrev && pMyPrev->IsDummy() )
    {
        DBG_LOOP;
        pLast = pMyPrev;
        pMyPrev = Prev();
    }
    return (SwLineLayout*)(pMyPrev ? pMyPrev : pLast);
}

/*************************************************************************
 *                      SwTxtIter::Bottom()
 *************************************************************************/

void SwTxtIter::Bottom()
{
    while( Next() )
    {
        DBG_LOOP;
    }
}

/*************************************************************************
 *                      SwTxtIter::CharToLine()
 *************************************************************************/

void SwTxtIter::CharToLine(const xub_StrLen nChar)
{
    while( nStart + pCurr->GetLen() <= nChar && Next() )
        ;
    while( nStart > nChar && Prev() )
        ;
}

/*************************************************************************
 *                      SwTxtIter::CharCrsrToLine()
 *************************************************************************/

// 1170: beruecksichtigt Mehrdeutigkeiten:
const SwLineLayout *SwTxtCursor::CharCrsrToLine( const xub_StrLen nPosition )
{
    CharToLine( nPosition );
    if( nPosition != nStart )
        bRightMargin = sal_False;
    sal_Bool bPrevious = bRightMargin && pCurr->GetLen() && GetPrev() &&
        GetPrev()->GetLen();
    if( bPrevious && nPosition && CH_BREAK == GetInfo().GetChar( nPosition-1 ) )
        bPrevious = sal_False;
    return bPrevious ? PrevLine() : pCurr;
}

/*************************************************************************
 *                      SwTxtCrsr::AdjustBaseLine()
 *************************************************************************/

USHORT SwTxtCursor::AdjustBaseLine( const SwLineLayout& rLine,
                                    const SwLinePortion* pPor,
                                    USHORT nPorHeight, USHORT nPorAscent,
                                    const sal_Bool bAutoToCentered ) const
{
    if ( pPor )
    {
        nPorHeight = pPor->Height();
        nPorAscent = pPor->GetAscent();
    }

    USHORT nOfst = rLine.GetRealHeight() - rLine.Height();

    GETGRID( pFrm->FindPageFrm() )
    const sal_Bool bHasGrid = pGrid && GetInfo().SnapToGrid();

    if ( bHasGrid )
    {
        const USHORT nRubyHeight = pGrid->GetRubyHeight();
        const sal_Bool bRubyTop = ! pGrid->GetRubyTextBelow();

        if ( GetInfo().IsMulti() )
            // we are inside the GetCharRect recursion for multi portions
            // we center the portion in its surrounding line
            nOfst = ( pCurr->Height() - nPorHeight ) / 2 + nPorAscent;
        else
        {
            // We have to take care for ruby portions.
            // The ruby portion is NOT centered
            nOfst = nOfst + nPorAscent;

            if ( ! pPor || ! pPor->IsMultiPortion() ||
                 ! ((SwMultiPortion*)pPor)->IsRuby() )
            {
                // Portions which are bigger than on grid distance are
                // centered inside the whole line.
                
                //for text refactor
                const USHORT nLineNetto =  rLine.Height() - nRubyHeight;
                //const USHORT nLineNetto = ( nPorHeight > nGridWidth ) ?
                 //                           rLine.Height() - nRubyHeight :
                 //                           nGridWidth;
                nOfst += ( nLineNetto - nPorHeight ) / 2;
                if ( bRubyTop )
                    nOfst = nOfst + nRubyHeight;
            }
        }
    }
    else
    {
        switch ( GetLineInfo().GetVertAlign() ) {
            case SvxParaVertAlignItem::TOP :
                nOfst = nOfst + nPorAscent;
                break;
            case SvxParaVertAlignItem::CENTER :
                ASSERT( rLine.Height() >= nPorHeight, "Portion height > Line height");
                nOfst += ( rLine.Height() - nPorHeight ) / 2 + nPorAscent;
                break;
            case SvxParaVertAlignItem::BOTTOM :
                nOfst += rLine.Height() - nPorHeight + nPorAscent;
                break;
            case SvxParaVertAlignItem::AUTOMATIC :
                if ( bAutoToCentered || GetInfo().GetTxtFrm()->IsVertical() )
                {
                    nOfst += ( rLine.Height() - nPorHeight ) / 2 + nPorAscent;
                    break;
                }
            case SvxParaVertAlignItem::BASELINE :
                // base line
                nOfst = nOfst + rLine.GetAscent();
                break;
        }
    }

    return nOfst;
}

/*************************************************************************
 *                      SwTxtIter::TwipsToLine()
 *************************************************************************/

const SwLineLayout *SwTxtIter::TwipsToLine( const SwTwips y)
{
    while( nY + GetLineHeight() <= y && Next() )
        ;
    while( nY > y && Prev() )
        ;
    return pCurr;
}

//
// Local helper function to check, if pCurr needs a field rest portion:
//
sal_Bool lcl_NeedsFieldRest( const SwLineLayout* pCurr )
{
    const SwLinePortion *pPor = pCurr->GetPortion();
    sal_Bool bRet = sal_False;
    while( pPor && !bRet )
    {
        bRet = pPor->InFldGrp() && ((SwFldPortion*)pPor)->HasFollow();
        if( !pPor->GetPortion() || !pPor->GetPortion()->InFldGrp() )
            break;
        pPor = pPor->GetPortion();
    }
    return bRet;
}

/*************************************************************************
 *						SwTxtIter::TruncLines()
 *************************************************************************/

void SwTxtIter::TruncLines( sal_Bool bNoteFollow )
{
    SwLineLayout *pDel = pCurr->GetNext();
    const xub_StrLen nEnd = nStart + pCurr->GetLen();

    if( pDel )
    {
        pCurr->SetNext( 0 );
        if( GetHints() && bNoteFollow )
        {
            GetInfo().GetParaPortion()->SetFollowField( pDel->IsRest() ||
                                                        lcl_NeedsFieldRest( pCurr ) );

            // bug 88534: wrong positioning of flys
            SwTxtFrm* pFollow = GetTxtFrm()->GetFollow();
            if ( pFollow && ! pFollow->IsLocked() &&
                 nEnd == pFollow->GetOfst() )
            {
                xub_StrLen nRangeEnd = nEnd;
                SwLineLayout* pLine = pDel;

                // determine range to be searched for flys anchored as characters
                while ( pLine )
                {
                    nRangeEnd = nRangeEnd + pLine->GetLen();
                    pLine = pLine->GetNext();
                }

                SwpHints* pTmpHints = GetTxtFrm()->GetTxtNode()->GetpSwpHints();

                // examine hints in range nEnd - (nEnd + nRangeChar)
                for( USHORT i = 0; i < pTmpHints->Count(); i++ )
                {
                    const SwTxtAttr* pHt = pTmpHints->GetTextHint( i );
                    if( RES_TXTATR_FLYCNT == pHt->Which() )
                    {
                        // check, if hint is in our range
                        const USHORT nTmpPos = *pHt->GetStart();
                        if ( nEnd <= nTmpPos && nTmpPos < nRangeEnd )
                            pFollow->_InvalidateRange(
                                SwCharRange( nTmpPos, nTmpPos ), 0 );
                    }
                }
            }
        }
        delete pDel;
    }
    if( pCurr->IsDummy() &&
        !pCurr->GetLen() &&
         nStart < GetTxtFrm()->GetTxt().Len() )
        pCurr->SetRealHeight( 1 );
    if( GetHints() )
        pFrm->RemoveFtn( nEnd );
}

/*************************************************************************
 *						SwTxtIter::CntHyphens()
 *************************************************************************/

void SwTxtIter::CntHyphens( sal_uInt8 &nEndCnt, sal_uInt8 &nMidCnt) const
{
    nEndCnt = 0;
    nMidCnt = 0;
    if ( bPrev && pPrev && !pPrev->IsEndHyph() && !pPrev->IsMidHyph() )
         return;
    SwLineLayout *pLay = pInf->GetParaPortion();
    if( pCurr == pLay )
        return;
    while( pLay != pCurr )
    {
        DBG_LOOP;
        if ( pLay->IsEndHyph() )
            nEndCnt++;
        else
            nEndCnt = 0;
        if ( pLay->IsMidHyph() )
            nMidCnt++;
        else
            nMidCnt = 0;
        pLay = pLay->GetNext();
    }
}

/*************************************************************************
 *                          SwHookOut
 *
 * Change current output device to formatting device, this has to be done before
 * formatting.
 *************************************************************************/

SwHookOut::SwHookOut( SwTxtSizeInfo& rInfo ) :
     pInf( &rInfo ),
     pOut( rInfo.GetOut() ),
     bOnWin( rInfo.OnWin() )
{
    ASSERT( rInfo.GetRefDev(), "No reference device for text formatting" )

    // set new values
    rInfo.SetOut( rInfo.GetRefDev() );
    rInfo.SetOnWin( sal_False );
}

SwHookOut::~SwHookOut()
{
    pInf->SetOut( pOut );
    pInf->SetOnWin( bOnWin );
}
