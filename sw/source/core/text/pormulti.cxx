/*************************************************************************
 *
 *  $RCSfile: pormulti.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ama $ $Date: 2000-10-17 10:36:46 $
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

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
#ifndef _PORMULTI_HXX
#include <pormulti.hxx> 	// SwMultiPortion
#endif
#ifndef _INFTXT_HXX
#include <inftxt.hxx>		// SwTxtSizeInfo
#endif
#ifndef _ITRPAINT_HXX
#include <itrpaint.hxx>     // SwTxtPainter
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>		// SwViewOptions
#endif
#ifndef _ITRFORM2_HXX
#include <itrform2.hxx>		// SwTxtFormatter
#endif
#ifndef _PORFLD_HXX
#include <porfld.hxx>		// SwFldPortion
#endif

/*-----------------10.10.00 15:23-------------------
 *  class SwMultiPortion
 *
 * A SwMultiPortion is not a simple portion,
 * it's a container, which contains almost a SwLineLayoutPortion.
 * This SwLineLayout could be followed by other textportions via pPortion
 * and by another SwLineLayout via pNext to realize a doubleline portion.
 * --------------------------------------------------*/

SwMultiPortion::~SwMultiPortion()
{
    delete pFldRest;
}

void SwMultiPortion::Paint( const SwTxtPaintInfo &rInf ) const
{
    ASSERT( FALSE,
    "Don't try SwMultiPortion::Paint, try SwTxtPainter::PaintMultiPortion" );
}

/*-----------------13.10.00 16:21-------------------
 * Summarize the internal lines to calculate the (external) size.
 * The internal line has to calculate first.
 * --------------------------------------------------*/

void SwMultiPortion::CalcSize( SwTxtFormatter& rLine )
{
    Width( 0 );
    Height( 0 );
    SetAscent( 0 );
    SwLineLayout *pLay = &GetRoot();
    do
    {
        pLay->CalcLine( rLine );
        Height( Height() + pLay->Height() );
        SetAscent( GetAscent() + pLay->GetAscent() );
        if( Width() < pLay->Width() )
            Width( pLay->Width() );
        pLay = pLay->GetNext();
    } while ( pLay );
}

/*-----------------13.10.00 16:22-------------------
 * If we're inside a double-line-attribute, the result
 * will be the end of the attribute,
 * otherwise the function returns zero.
 * --------------------------------------------------*/

xub_StrLen SwTxtSizeInfo::EndOfMulti( const xub_StrLen nPos ) const
{
    xub_StrLen nRet = 0;
    const SwpHints *pHints = pFrm->GetTxtNode()->GetpSwpHints();
    if( !pHints )
        return sal_False;
    for( MSHORT i = 0; i < pHints->Count(); ++i )
    {
        const SwTxtAttr *pPos = (*pHints)[i];
        xub_StrLen nStart = *pPos->GetStart();
        if( nPos < nStart )
            break;
#ifdef FOR_YOUR_OWN_RISK
        if( RES_CHRATR_UNDERLINE == pPos->Which() )
        {
            nRet = *pPos->GetEnd();
            if( nPos == nStart || nRet > nPos )
                break;
            nRet = 0;
        }
#endif
    }
    return nRet;
}

/*-----------------13.10.00 16:24-------------------
 * SwTxtPainter::PaintMultiPortion manages the paint for a SwMultiPortion.
 * External, for the calling function, it seems to be a normal Paint-function,
 * internal it is like a SwTxtFrm::Paint with multiple DrawTextLines
 * --------------------------------------------------*/

void SwTxtPainter::PaintMultiPortion( const SwRect &rPaint,
    SwMultiPortion& rMulti )
{
    // old values must be saved and restored at the end
    xub_StrLen nOldLen = GetInfo().GetLen();
    KSHORT nOldX = GetInfo().X();
    KSHORT nOldY = GetInfo().Y();
    xub_StrLen nOldIdx = GetInfo().GetIdx();
    SvShorts *pOldSpaceAdd = GetInfo().GetpSpaceAdd();
    GetInfo().SetSpaceAdd( NULL );

    SwLineLayout* pLay = &rMulti.GetRoot();// the first line of the multiportion
    SwLinePortion* pPor = pLay->GetFirstPortion();//first portion of these line

    // GetInfo().Y() is the baseline from the surrounding line. We must switch
    // this temporary to the baseline of the inner lines of the multiportion.
    GetInfo().Y( nOldY - rMulti.GetAscent() + pLay->GetAscent() );
    sal_Bool bRest = pLay->IsRest();
    sal_Bool bFirst = sal_True;
    do
    {
        sal_Bool bSeeked = sal_True;
        GetInfo().SetLen( pPor->GetLen() );
        GetInfo().SetSpecialUnderline( sal_False );
        if( ( bRest && pPor->InFldGrp() && !pPor->GetLen() ) )
            SeekAndChgBefore( GetInfo() );
        else if( pPor->InTxtGrp() || pPor->InFldGrp() || pPor->InTabGrp() )
            SeekAndChg( GetInfo() );
        else if ( !bFirst && pPor->IsBreakPortion() && GetInfo().GetOpt().IsParagraph() )
        {
            if( GetRedln() )
                SeekAndChg( GetInfo() );
            else
                SeekAndChgBefore( GetInfo() );
        }
        else
            bSeeked = sal_False;

        SwLinePortion *pNext = pPor->GetPortion();
        if(GetInfo().OnWin() && pNext && !pNext->Width() )
        {
            if ( !bSeeked )
                SeekAndChg( GetInfo() );
            pNext->PrePaint( GetInfo(), pPor );
        }

        if( pFnt->GetEscapement() && UNDERLINE_NONE != pFnt->GetUnderline() )
            CheckSpecialUnderline();

        pPor->Paint( GetInfo() );

        if( GetFnt()->IsURL() && pPor->InTxtGrp() )
            GetInfo().NotifyURL( *pPor );

        bFirst &= !pPor->GetLen();
        if( pNext || !pPor->IsMarginPortion() )
            pPor->Move( GetInfo() );

        pPor = pNext;

        // If there's no portion left, we go to the next line
        if( !pPor && pLay->GetNext() )
        {
            pLay = pLay->GetNext();
            pPor = pLay->GetFirstPortion();
            bRest = pLay->IsRest();
            GetInfo().X( nOldX );
            // We switch to the baseline of the next inner line
            GetInfo().Y( GetInfo().Y() + rMulti.GetRoot().Height()
                - rMulti.GetRoot().GetAscent() + pLay->GetAscent() );
        }
    } while( pPor );

    // Restore the saved values
    GetInfo().SetLen( nOldLen );
    GetInfo().SetIdx( nOldIdx );
    GetInfo().X( nOldX );
    GetInfo().Y( nOldY );
    GetInfo().SetSpaceAdd( pOldSpaceAdd );
}

/*-----------------13.10.00 16:46-------------------
 * SwTxtPainter::PaintMultiPortion manages the formatting of a SwMultiPortion.
 * External, for the calling function, it seems to be a normal Format-function,
 * internal it is like a SwTxtFrm::_Format with multiple BuildPortions
 * --------------------------------------------------*/

BOOL SwTxtFormatter::BuildMultiPortion( SwTxtFormatInfo &rInf,
    SwMultiPortion& rMulti )
{
    pMulti = &rMulti;
    SwLineLayout *pOldCurr = pCurr;
    xub_StrLen nOldStart = GetStart();
    SwTwips nMinWidth = rInf.X() + 1;
    SwTwips nMaxWidth = rInf.Width();
    SwTwips nActWidth = nMaxWidth;
    SwTxtFormatInfo aInf( rInf, rMulti.GetRoot(), nActWidth );
    xub_StrLen nStartIdx = aInf.GetIdx();
    xub_StrLen nMultiLen = rMulti.GetLen() - rInf.GetIdx();
    SwLinePortion *pRest = NULL;
    BOOL bRet = FALSE;
    do
    {
        pCurr = &rMulti.GetRoot();
        nStart = nStartIdx;
        bRet = FALSE;
        FormatReset( aInf );
        aInf.X( rInf.X() );
        aInf.Width( nActWidth );
        if( rMulti.GetFldRest() )
        {
            SwFldPortion *pFld =
                rMulti.GetFldRest()->Clone( rMulti.GetFldRest()->GetExp() );
            pFld->SetFollow( sal_True );
            aInf.SetRest( pFld );
        }
        BuildPortions( aInf );
        rMulti.CalcSize( *this );
        pCurr->SetRealHeight( pCurr->Height() );
        if( pCurr->GetLen() < nMultiLen || aInf.GetRest() )
        {
            xub_StrLen nFirstLen = pCurr->GetLen();
            pCurr->SetNext( new SwLineLayout() );
            pCurr = pCurr->GetNext();
            nStart = aInf.GetIdx();
            aInf.X( rInf.X() );
            SwTxtFormatInfo aTmp( aInf, *pCurr, nActWidth );
            aTmp.SetRest( aInf.GetRest() );
            aInf.SetRest( NULL );
            BuildPortions( aTmp );
            rMulti.CalcSize( *this );
            pCurr->SetRealHeight( pCurr->Height() );
            pRest = aTmp.GetRest();
            if( nFirstLen + pCurr->GetLen() < nMultiLen || pRest )
                bRet = TRUE;
        }
        if( bRet )
        {
            nMinWidth = nActWidth;
            nActWidth = ( 3 * nMaxWidth + nMinWidth + 3 ) / 4;
            if( nActWidth <= nMinWidth )
                break;
        }
        else
        {
            if( nActWidth > rInf.X() + rMulti.Width() + 1 )
                nActWidth = rInf.X() + rMulti.Width() + 1;
            nMaxWidth = nActWidth;
            nActWidth = ( 3 * nMaxWidth + nMinWidth + 3 ) / 4;
            if( nActWidth >= nMaxWidth )
                break;
        }
        delete pRest;
        pRest = NULL;
    } while ( TRUE );
    pMulti = NULL;
    pCurr = pOldCurr;
    nStart = nOldStart;
    rMulti.SetLen( rMulti.GetRoot().GetLen() + ( rMulti.GetRoot().GetNext() ?
        rMulti.GetRoot().GetNext()->GetLen() : 0 ) );
    if( bRet )
    {
        SwMultiPortion *pTmp = new SwMultiPortion( nMultiLen + rInf.GetIdx() );
        ASSERT( !pRest || pRest->InFldGrp(),
            "BuildMultiPortion: Surprising restportion, field exspected" );
        pTmp->SetFldRest( (SwFldPortion*) pRest );
        rInf.SetRest( pTmp );
    }
    return bRet;
}

