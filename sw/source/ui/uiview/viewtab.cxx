/*************************************************************************
 *
 *  $RCSfile: viewtab.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: svesik $ $Date: 2004-04-21 10:02:02 $
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



#pragma hdrstop

#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif

#include <hintids.hxx>
#include "uiparam.hxx"
#include "uitool.hxx"

#ifndef _SVX_RULRITEM_HXX //autogen
#include <svx/rulritem.hxx>
#endif
#ifndef _SVX_TSPTITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SVX_RULER_HXX //autogen
#include <svx/ruler.hxx>
#endif
#ifndef _SVX_PROTITEM_HXX //autogen
#include <svx/protitem.hxx>
#endif
#ifndef _SFXRECTITEM_HXX //autogen
#include <svtools/rectitem.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif

#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#include "view.hxx"
#include "wrtsh.hxx"
#include "basesh.hxx"
#include "cmdid.h"
#include "viewopt.hxx"
#include "tabcol.hxx"
#include "frmfmt.hxx"		// FrameFormat
#include "pagedesc.hxx"		// Aktuelles Seitenformat
#include "wview.hxx"
#include "fmtcol.hxx"
#include "section.hxx"

// -> #i23726#
#include "ndtxt.hxx" 
#include "pam.hxx"
// <- #i23726#

/*--------------------------------------------------------------------
    Beschreibung:	Debug-Methode
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
    Beschreibung:	Columns eintueten
 --------------------------------------------------------------------*/


void lcl_FillSvxColumn(const SwFmtCol& rCol,
                          USHORT nTotalWidth,
                          SvxColumnItem& rColItem,
                          long nDistance)
{
    const SwColumns& rCols = rCol.GetColumns();
    USHORT nWidth = 0;

    BOOL bOrtho = rCol.IsOrtho() && rCols.Count();
    long nInnerWidth;
    if( bOrtho )
    {
        nInnerWidth = nTotalWidth;
        for ( USHORT i = 0; i < rCols.Count(); ++i )
        {
            SwColumn* pCol = rCols[i];
            nInnerWidth -= pCol->GetLeft() + pCol->GetRight();
        }
        if( nInnerWidth < 0 )
            nInnerWidth = 0;
        else
            nInnerWidth /= rCols.Count();
    }
    for ( USHORT i = 0; i < rCols.Count(); ++i )
    {
        SwColumn* pCol = rCols[i];
        const USHORT nStart = USHORT(pCol->GetLeft() + nWidth + nDistance);
        if( bOrtho )
            nWidth += nInnerWidth + pCol->GetLeft() + pCol->GetRight();
        else
            nWidth += rCol.CalcColWidth(i, nTotalWidth);
        const USHORT nEnd = USHORT(nWidth - pCol->GetRight() + nDistance);

        SvxColumnDescription aColDesc(nStart, nEnd, TRUE);
        rColItem.Append(aColDesc);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	ColumnItem in ColumnInfo ueberfuehren
 --------------------------------------------------------------------*/


void lcl_ConvertToCols(const SvxColumnItem& rColItem,
                          USHORT nTotalWidth,
                          SwFmtCol& rCols)
{
    ASSERT( rCols.GetNumCols() == rColItem.Count(), "Column count mismatch" );

    USHORT nLeft 	= 0;
    SwTwips nSumAll= 0;  // Summiere alle Spalten und Raender auf

    SwColumns& rArr = rCols.GetColumns();

    // Tabcols der Reihe nach
    for( USHORT i=0; i < rColItem.Count()-1; ++i )
    {
        DBG_ASSERT(rColItem[i+1].nStart >= rColItem[i].nEnd,"\201berlappende Spalten" );
        USHORT nStart = rColItem[i+1].nStart;
        USHORT nEnd = rColItem[i].nEnd;
        if(nStart < nEnd)
            nStart = nEnd;
        const USHORT nDiff	= nStart - nEnd;
        const USHORT nRight = nDiff / 2;

        USHORT nWidth = rColItem[i].nEnd - rColItem[i].nStart;
        nWidth += nLeft + nRight;

        SwColumn* pCol = rArr[i];
        pCol->SetWishWidth( USHORT(long(rCols.GetWishWidth()) * long(nWidth) /
                                                            long(nTotalWidth) ));
        pCol->SetLeft( nLeft );
        pCol->SetRight( nRight );
        nSumAll += pCol->GetWishWidth();

        nLeft = nRight;
    }
    rArr[rColItem.Count()-1]->SetLeft( nLeft );

    //Die Differenz aus der Gesamtwunschbreite und der Summe der bisher berechneten
    // Spalten und Raender sollte die Breite der letzten Spalte ergeben.
    rArr[rColItem.Count()-1]->SetWishWidth( rCols.GetWishWidth() - (USHORT)nSumAll );

    rCols.SetOrtho(FALSE, 0, 0 );
}

/*--------------------------------------------------------------------
    Beschreibung:	Tabs loeschen
 --------------------------------------------------------------------*/


void lcl_EraseDefTabs(SvxTabStopItem& rTabStops)
{
    // Def Tabs loeschen
    for ( USHORT i = 0; i < rTabStops.Count(); )
    {
        // Hier auch den DefTab auf Null rausschmeissen
        if ( SVX_TAB_ADJUST_DEFAULT == rTabStops[i].GetAdjustment() ||
            rTabStops[i].GetTabPos() == 0 )
        {
            rTabStops.Remove(i);
            continue;
        }
        ++i;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Seitenrand umdrehen
 --------------------------------------------------------------------*/


void SwView::SwapPageMargin(const SwPageDesc& rDesc, SvxLRSpaceItem& rLRSpace)
{
    USHORT nPhyPage, nVirPage;
    GetWrtShell().GetPageNum( nPhyPage, nVirPage );

    if ( rDesc.GetUseOn() == PD_MIRROR && (nPhyPage % 2) == 0 )
    {
        long nTmp = rLRSpace.GetRight();
        rLRSpace.SetRight( rLRSpace.GetLeft() );
        rLRSpace.SetLeft( nTmp );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Wenn der Rahmenrand verschoben wird, sollen die
                    Spaltentrenner an der gleichen absoluten Position bleiben
 --------------------------------------------------------------------*/


void lcl_Scale(long& nVal, long nScale)
{
    nVal *= nScale;
    nVal >>= 8;
}

void ResizeFrameCols(SwFmtCol& rCol,
                    long nOldWidth,
                    long nNewWidth,
                    long nLeftDelta )
{
    SwColumns& rArr = rCol.GetColumns();
    long nWishSum = (long)rCol.GetWishWidth();
    long nWishDiff = (nWishSum * 100/nOldWidth * nNewWidth) / 100 - nWishSum;
    long nNewWishWidth = nWishSum + nWishDiff;
    if(nNewWishWidth > 0xffffl)
    {
        // wenn die Wunschbreite zu gross wird, dann muessen alle Werte passend skaliert werden
        long nScale = (0xffffl << 8)/ nNewWishWidth;
        for(USHORT i = 0; i < rArr.Count(); i++)
        {
            SwColumn* pCol = rArr.GetObject(i);
            long nVal = pCol->GetWishWidth();
            lcl_Scale(nVal, nScale);
            pCol->SetWishWidth((USHORT) nVal);
            nVal = pCol->GetLeft();
            lcl_Scale(nVal, nScale);
            pCol->SetLeft((USHORT) nVal);
            nVal = pCol->GetRight();
            lcl_Scale(nVal, nScale);
            pCol->SetRight((USHORT) nVal);
        }
        lcl_Scale(nNewWishWidth, nScale);
        lcl_Scale(nWishDiff, nScale);
    }
    rCol.SetWishWidth( (USHORT) (nNewWishWidth) );

    if( nLeftDelta >= 2 || nLeftDelta <= -2)
        rArr[0]->SetWishWidth(rArr[0]->GetWishWidth() + (USHORT)nWishDiff);
    else
        rArr[rArr.Count()-1]->SetWishWidth(rArr[rArr.Count()-1]->GetWishWidth() + (USHORT)nWishDiff);
    //reset auto width
    rCol.SetOrtho(FALSE, 0, 0 );
}
/*--------------------------------------------------------------------
    Beschreibung:	Hier werden alle Aenderungen der Tableiste
                    wieder in das Modell geschossen
 --------------------------------------------------------------------*/


void SwView::ExecTabWin( SfxRequest& rReq )
{
    SwWrtShell &rSh 		= GetWrtShell();
    const USHORT nFrmType	= rSh.IsObjSelected() ?
                                    FRMTYPE_DRAWOBJ :
                                        rSh.GetFrmType(0,TRUE);
    const BOOL 	bFrmSelection = rSh.IsFrmSelected();
    BOOL bBrowse = rSh.IsBrowseMode();


    const USHORT nSlot 	  	= rReq.GetSlot();
    const USHORT nDescId  	= rSh.GetCurPageDesc();
    const SwPageDesc& rDesc = rSh.GetPageDesc( nDescId );

    const SvxFrameDirectionItem& rFrameDir = rDesc.GetMaster().GetFrmDir();
    const BOOL bVerticalWriting = rSh.IsInVerticalText();
    const SwFmtHeader& rHeaderFmt = rDesc.GetMaster().GetHeader();
    SwFrmFmt *pHeaderFmt = (SwFrmFmt*)rHeaderFmt.GetHeaderFmt();

    const SwFmtFooter& rFooterFmt = rDesc.GetMaster().GetFooter();
    SwFrmFmt *pFooterFmt = (SwFrmFmt*)rFooterFmt.GetFooterFmt();

    const SwFmtFrmSize &rFrmSize = rDesc.GetMaster().GetFrmSize();

    const SwRect& rPrtRect = rSh.GetAnyCurRect(RECT_PAGE);
    const long nPageWidth  = bBrowse ?
                                rPrtRect.Width() :
                                    rFrmSize.GetWidth();
    const long nPageHeight = bBrowse ?
                                rPrtRect.Height() :
                                    rFrmSize.GetHeight();
    const SfxItemSet* pArgs = rReq.GetArgs();

    BOOL bUnlockView = FALSE;
    rSh.StartAllAction();
    BOOL bSect = 0 != (nFrmType & FRMTYPE_COLSECT);

    switch	( nSlot )
    {
    case SID_ATTR_LONG_LRSPACE:
    {
        SvxLongLRSpaceItem aLongLR( (const SvxLongLRSpaceItem&)rReq.GetArgs()->
                                                    Get( SID_ATTR_LONG_LRSPACE ) );
        SvxLRSpaceItem aLR;
        BOOL bSect = 0 != (nFrmType & FRMTYPE_COLSECT);
        if ( !bSect && (bFrmSelection || nFrmType & FRMTYPE_FLY_ANY) )
        {
            SwFrmFmt* pFmt = ((SwFrmFmt*)rSh.GetFlyFrmFmt());
            const SwRect &rRect = rSh.GetAnyCurRect(RECT_FLY_EMBEDDED);

            BOOL bRTL;
            BOOL bVerticalFrame = (bFrmSelection && rSh.IsFrmVertical(TRUE, bRTL))|| (!bFrmSelection && bVerticalWriting);
            long nDeltaX = bVerticalFrame ?
                rRect.Right() - nPageWidth - DOCUMENTBORDER + aLongLR.GetRight() :
                DOCUMENTBORDER + aLongLR.GetLeft() - rRect.Left();

            SfxItemSet aSet( GetPool(), RES_FRM_SIZE, RES_FRM_SIZE,
                                        RES_VERT_ORIENT, RES_HORI_ORIENT,
                                        RES_COL, RES_COL, 0 );

            if(bVerticalFrame)
            {
                SwFmtVertOrient aVertOrient(pFmt->GetVertOrient());
                aVertOrient.SetVertOrient(VERT_NONE);
                aVertOrient.SetPos(aVertOrient.GetPos() + nDeltaX );
                aSet.Put( aVertOrient );
            }
            else
            {
                SwFmtHoriOrient aHoriOrient( pFmt->GetHoriOrient() );
                aHoriOrient.SetHoriOrient( HORI_NONE );
                aHoriOrient.SetPos( aHoriOrient.GetPos() + nDeltaX );
                aSet.Put( aHoriOrient );
            }

            SwFmtFrmSize aSize( pFmt->GetFrmSize() );
            long nOldWidth = (long) aSize.GetWidth();

            if(aSize.GetWidthPercent())
            {
                SwRect aRect;
                rSh.CalcBoundRect(aRect, FLY_IN_CNTNT);
                long nPrtWidth = aRect.Width();
                aSize.SetWidthPercent(BYTE((nPageWidth - aLongLR.GetLeft() - aLongLR.GetRight()) * 100 /nPrtWidth));
            }
            else
                aSize.SetWidth( nPageWidth -
                        (aLongLR.GetLeft() + aLongLR.GetRight()));

            if( nFrmType & FRMTYPE_COLUMN )
            {
                SwFmtCol aCol(pFmt->GetCol());

                ::ResizeFrameCols(aCol, nOldWidth, (long)aSize.GetWidth(), nDeltaX );
                aSet.Put(aCol);
            }

            aSet.Put( aSize );

            rSh.StartAction();
            rSh.Push();
            rSh.SetFlyFrmAttr( aSet );
            //die Rahmenselektion wieder aufheben
            if(!bFrmSelection && rSh.IsFrmSelected())
            {
                rSh.UnSelectFrm();
                rSh.LeaveSelFrmMode();
            }
            rSh.Pop();
            rSh.EndAction();
        }
        else if ( nFrmType & ( FRMTYPE_HEADER | FRMTYPE_FOOTER ))
        {
            // Seitenraender rausrechnen
            long nOld = rDesc.GetMaster().GetLRSpace().GetLeft();
            aLongLR.SetLeft( nOld > aLongLR.GetLeft() ? 0 : aLongLR.GetLeft() - nOld );

            nOld = rDesc.GetMaster().GetLRSpace().GetRight();
            aLongLR.SetRight( nOld > (USHORT)aLongLR.GetRight() ? 0 : aLongLR.GetRight() - nOld );
            aLR.SetLeft((USHORT)aLongLR.GetLeft());
            aLR.SetRight((USHORT)aLongLR.GetRight());

            if ( nFrmType & FRMTYPE_HEADER )
                pHeaderFmt->SetAttr( aLR );
            else
                pFooterFmt->SetAttr( aLR );
        }
        else if( nFrmType == FRMTYPE_DRAWOBJ)
        {
            SwRect aRect( rSh.GetObjRect() );
            aRect.Left( aLongLR.GetLeft() + DOCUMENTBORDER );
            aRect.Right( nPageWidth + DOCUMENTBORDER - aLongLR.GetRight());
            rSh.SetObjRect( aRect );
        }
        else if(bSect || rSh.IsDirectlyInSection())
        {
            //change the section indents and the columns if available
            //at first determine the changes
            SwRect aSectRect = rSh.GetAnyCurRect(RECT_SECTION_PRT, 0);
            const SwRect aTmpRect = rSh.GetAnyCurRect(RECT_SECTION, 0);
            aSectRect.Pos() += aTmpRect.Pos();
            long nLeftDiff = aLongLR.GetLeft() - (long)(aSectRect.Left() - DOCUMENTBORDER);
            long nRightDiff = aLongLR.GetRight() - (long)(nPageWidth + DOCUMENTBORDER - aSectRect.Right());
            //change the LRSpaceItem of the section accordingly
            const SwSection* pCurrSect = rSh.GetCurrSection();
            const SwSectionFmt* pSectFmt = pCurrSect->GetFmt();
            SvxLRSpaceItem aLR = pSectFmt->GetLRSpace();
            aLR.SetLeft(aLR.GetLeft() + nLeftDiff);
            aLR.SetRight(aLR.GetRight() + nRightDiff);
            SfxItemSet aSet(rSh.GetAttrPool(), RES_LR_SPACE, RES_LR_SPACE, RES_COL, RES_COL, 0L);
            aSet.Put(aLR);
            //change the first/last column
            if(bSect)
            {
                SwFmtCol aCols( pSectFmt->GetCol() );
                long nDiffWidth = nLeftDiff + nRightDiff;
                ::ResizeFrameCols(aCols, aSectRect.Width(), aSectRect.Width() - nDiffWidth, nLeftDiff );
                aSet.Put( aCols );
            }
            rSh.ChgSection( rSh.GetSectionFmtPos(*pSectFmt), *pCurrSect, &aSet );

        }
        else
        {	// Seitenraender einstellen
            aLR.SetLeft((USHORT)aLongLR.GetLeft());
            aLR.SetRight((USHORT)aLongLR.GetRight());
            SwapPageMargin( rDesc, aLR );
            SwPageDesc aDesc( rDesc );
            aDesc.GetMaster().SetAttr( aLR );
            rSh.ChgPageDesc( nDescId, aDesc );
        }
    }
    break;
    case SID_ATTR_LONG_ULSPACE:
    {
        SvxLongULSpaceItem aLongULSpace( (const SvxLongULSpaceItem&)rReq.GetArgs()->
                                                        Get( SID_ATTR_LONG_ULSPACE ));

        if( bFrmSelection || nFrmType & FRMTYPE_FLY_ANY )
        {
            SwFrmFmt* pFmt = ((SwFrmFmt*)rSh.GetFlyFrmFmt());
            const SwRect &rRect = rSh.GetAnyCurRect(RECT_FLY_EMBEDDED);
            const long nDeltaY =
                    DOCUMENTBORDER + aLongULSpace.GetUpper() -
                                    rRect.Top();
            const long nHeight = nPageHeight -
                            (aLongULSpace.GetUpper() + aLongULSpace.GetLower());

            SfxItemSet aSet( GetPool(), RES_FRM_SIZE, RES_FRM_SIZE,
                                        RES_VERT_ORIENT, RES_HORI_ORIENT, 0 );
            //which of the orientation attributes is to be put depends on the frame's environment
            BOOL bRTL;
            if((bFrmSelection && rSh.IsFrmVertical(TRUE, bRTL))|| (!bFrmSelection && bVerticalWriting))
            {
                SwFmtHoriOrient aHoriOrient(pFmt->GetHoriOrient());
                aHoriOrient.SetHoriOrient(HORI_NONE);
                aHoriOrient.SetPos(aHoriOrient.GetPos() + nDeltaY );
                aSet.Put( aHoriOrient );
            }
            else
            {
                SwFmtVertOrient aVertOrient(pFmt->GetVertOrient());
                aVertOrient.SetVertOrient(VERT_NONE);
                aVertOrient.SetPos(aVertOrient.GetPos() + nDeltaY );
                aSet.Put( aVertOrient );
            }
            SwFmtFrmSize aSize(pFmt->GetFrmSize());
            if(aSize.GetHeightPercent())
            {
                SwRect aRect;
                rSh.CalcBoundRect(aRect, FLY_IN_CNTNT);
                long nPrtHeight = aRect.Height();
                aSize.SetHeightPercent(BYTE(nHeight * 100 /nPrtHeight));
            }
            else
                aSize.SetHeight(nHeight );

            aSet.Put( aSize );
            rSh.SetFlyFrmAttr( aSet );
        }
        else if( nFrmType == FRMTYPE_DRAWOBJ )
        {
            SwRect aRect( rSh.GetObjRect() );
            const SwRect &rPageRect = rSh.GetAnyCurRect(RECT_PAGE);
            aRect.Top( aLongULSpace.GetUpper() + rPageRect.Top() );
            aRect.Bottom( rPageRect.Bottom() - aLongULSpace.GetLower() );
            rSh.SetObjRect( aRect ) ;
        }
        else if(bVerticalWriting && (bSect || rSh.IsDirectlyInSection()))
        {
            //change the section indents and the columns if available
            //at first determine the changes
            SwRect aSectRect = rSh.GetAnyCurRect(RECT_SECTION_PRT, 0);
            const SwRect aTmpRect = rSh.GetAnyCurRect(RECT_SECTION, 0);
            aSectRect.Pos() += aTmpRect.Pos();
            long nLeftDiff = aLongULSpace.GetUpper() - (long)(aSectRect.Top() - rPrtRect.Top());
            long nRightDiff = aLongULSpace.GetLower() - (long)(nPageHeight - aSectRect.Bottom() + rPrtRect.Top());
            //change the LRSpaceItem of the section accordingly
            const SwSection* pCurrSect = rSh.GetCurrSection();
            const SwSectionFmt* pSectFmt = pCurrSect->GetFmt();
            SvxLRSpaceItem aLR = pSectFmt->GetLRSpace();
            aLR.SetLeft(aLR.GetLeft() + nLeftDiff);
            aLR.SetRight(aLR.GetRight() + nRightDiff);
            SfxItemSet aSet(rSh.GetAttrPool(), RES_LR_SPACE, RES_LR_SPACE, RES_COL, RES_COL, 0L);
            aSet.Put(aLR);
            //change the first/last column
            if(bSect)
            {
                SwFmtCol aCols( pSectFmt->GetCol() );
                long nDiffWidth = nLeftDiff + nRightDiff;
                ::ResizeFrameCols(aCols, aSectRect.Height(), aSectRect.Height() - nDiffWidth, nLeftDiff );
                aSet.Put( aCols );
            }
            rSh.ChgSection( rSh.GetSectionFmtPos(*pSectFmt), *pCurrSect, &aSet );

        }
        else
        {	SwPageDesc aDesc( rDesc );

            if ( nFrmType & ( FRMTYPE_HEADER | FRMTYPE_FOOTER ))
            {

                const BOOL bHead = nFrmType & FRMTYPE_HEADER ? TRUE : FALSE;
                SvxULSpaceItem aUL( rDesc.GetMaster().GetULSpace() );
                if ( bHead )
                    aUL.SetUpper( (USHORT)aLongULSpace.GetUpper() );
                else
                    aUL.SetLower( (USHORT)aLongULSpace.GetLower() );
                aDesc.GetMaster().SetAttr( aUL );

                SwFmtFrmSize aSz( bHead ? pHeaderFmt->GetFrmSize() :
                                          pFooterFmt->GetFrmSize() );
                aSz.SetSizeType( ATT_FIX_SIZE );
                aSz.SetHeight(nPageHeight - aLongULSpace.GetLower() -
                                            aLongULSpace.GetUpper() );
                if ( bHead )
                    pHeaderFmt->SetAttr( aSz );
                else
                    pFooterFmt->SetAttr( aSz );
            }
            else
            {
                SvxULSpaceItem aUL;
                aUL.SetUpper((USHORT)aLongULSpace.GetUpper());
                aUL.SetLower((USHORT)aLongULSpace.GetLower());
                aDesc.GetMaster().SetAttr(aUL);
            }

            rSh.ChgPageDesc( nDescId, aDesc );
        }
    }
    break;
    case SID_ATTR_TABSTOP_VERTICAL:
    case SID_ATTR_TABSTOP:
    {
        USHORT nWhich = GetPool().GetWhich(nSlot);
        SvxTabStopItem aTabStops( (const SvxTabStopItem&)rReq.GetArgs()->
                                                    Get( nWhich ));
        aTabStops.SetWhich(RES_PARATR_TABSTOP);
         const SvxTabStopItem& rDefTabs =
                    (const SvxTabStopItem&)rSh.GetDefault(RES_PARATR_TABSTOP);

        // Default-Tab an Pos 0
        SfxItemSet aSet( GetPool(), RES_LR_SPACE, RES_LR_SPACE );
        rSh.GetAttr( aSet );
        const SvxLRSpaceItem& rLR = (const SvxLRSpaceItem&)aSet.Get(RES_LR_SPACE);

        if ( rLR.GetTxtFirstLineOfst() < 0 )
        {
            SvxTabStop aSwTabStop( 0, SVX_TAB_ADJUST_DEFAULT );
            aTabStops.Insert( aSwTabStop );
        }

        // auffuellen mit Default-Tabs
        USHORT nDef = ::GetTabDist( rDefTabs );
        ::MakeDefTabs( nDef, aTabStops );

        SwTxtFmtColl* pColl = rSh.GetCurTxtFmtColl();
        if(	pColl && pColl->IsAutoUpdateFmt() )
        {
            SfxItemSet aSet(GetPool(), RES_PARATR_TABSTOP, RES_PARATR_TABSTOP);
            aSet.Put(aTabStops);
            rSh.AutoUpdatePara( pColl, aSet);
        }
        else
            rSh.SetAttr( aTabStops );
        break;
    }
    case SID_ATTR_PARA_LRSPACE_VERTICAL:
    case SID_ATTR_PARA_LRSPACE:
    {
        SvxLRSpaceItem aParaMargin((const SvxLRSpaceItem&)rReq.
                                        GetArgs()->Get(nSlot));
        if(nFrmType & FRMTYPE_FLY_ANY)
        {
            sal_Bool bFirstColumn = sal_True;
            sal_Bool bLastColumn = sal_True;
            if(nFrmType & FRMTYPE_COLUMN)
            {
                USHORT nCurFrameCol = rSh.GetCurColNum() - 1;
                bFirstColumn = !nCurFrameCol;
                const SwFrmFmt* pFmt =  rSh.GetFlyFrmFmt();
                const SwFmtCol* pCols = &pFmt->GetCol();
                const SwColumns& rCols = pCols->GetColumns();
                USHORT nColumnCount = rCols.Count();
                bLastColumn = nColumnCount == nCurFrameCol + 1;
            }
        }
        aParaMargin.SetRight( aParaMargin.GetRight() - nRightBorderDistance );
        aParaMargin.SetTxtLeft(aParaMargin.GetTxtLeft() - nLeftBorderDistance );

        aParaMargin.SetWhich( RES_LR_SPACE );
        SwTxtFmtColl* pColl = rSh.GetCurTxtFmtColl();        

        // #i23726#
        if (pNumRuleNodeFromDoc)
        {
            SwPosition aPos(*pNumRuleNodeFromDoc);
            rSh.NumIndent(aParaMargin.GetTxtLeft(), aPos);
        }
        else if(	pColl && pColl->IsAutoUpdateFmt() )
        {
            SfxItemSet aSet(GetPool(), RES_LR_SPACE, RES_LR_SPACE);
            aSet.Put(aParaMargin);
            rSh.AutoUpdatePara( pColl, aSet);
        }
        else
            rSh.SetAttr( aParaMargin );

        if ( aParaMargin.GetTxtFirstLineOfst() < 0 )
        {
            SfxItemSet aSet( GetPool(), RES_PARATR_TABSTOP, RES_PARATR_TABSTOP );

            rSh.GetAttr( aSet );
            const SvxTabStopItem&  rTabStops = (const SvxTabStopItem&)aSet.Get(RES_PARATR_TABSTOP);

            // Haben wir einen Tab an Stelle Null
            USHORT i;

            for ( i = 0; i < rTabStops.Count(); ++i )
                if ( rTabStops[i].GetTabPos() == 0 )
                    break;

            if ( i >= rTabStops.Count() )
            {
                // Kein DefTab
                SvxTabStopItem aTabStops;
                aTabStops = rTabStops;

                ::lcl_EraseDefTabs(aTabStops);

                SvxTabStop aSwTabStop( 0, SVX_TAB_ADJUST_DEFAULT );
                aTabStops.Insert(aSwTabStop);

                const SvxTabStopItem& rDefTabs =
                    (const SvxTabStopItem&)rSh.GetDefault(RES_PARATR_TABSTOP);
                USHORT nDef = ::GetTabDist(rDefTabs);
                ::MakeDefTabs( nDef, aTabStops );

                if(	pColl && pColl->IsAutoUpdateFmt())
                {
                    SfxItemSet aSet(GetPool(), RES_PARATR_TABSTOP, RES_PARATR_TABSTOP);
                    aSet.Put(aTabStops);
                    rSh.AutoUpdatePara( pColl, aSet);
                }
                else
                    rSh.SetAttr( aTabStops );
            }
        }
    }
    break;
    case SID_RULER_BORDERS_VERTICAL:
    case SID_RULER_BORDERS:
    {
        SvxColumnItem aColItem((const SvxColumnItem&)rReq.
                                            GetArgs()->Get(nSlot));

        BOOL bSect = 0 != (nFrmType & FRMTYPE_COLSECT);
        if( bSetTabColFromDoc || !bSect && rSh.GetTableFmt() )
        {
            ASSERT(aColItem.Count(), "ColDesc ist leer!!");

            const BOOL bSingleLine = ((const SfxBoolItem&)rReq.
                            GetArgs()->Get(SID_RULER_ACT_LINE_ONLY)).GetValue();

            SwTabCols aTabCols;
            if ( bSetTabColFromDoc )
                rSh.GetMouseTabCols( aTabCols, aTabColFromDocPos );
            else
                rSh.GetTabCols(aTabCols);

            // linker Tabellenrand
            long nBorder = long(aColItem.GetLeft()) -
                             (long(aTabCols.GetLeftMin()) - DOCUMENTBORDER);
            aTabCols.SetLeft( nBorder );

            nBorder = (bVerticalWriting ? nPageHeight : nPageWidth) - aTabCols.GetLeftMin() +
                              DOCUMENTBORDER - aColItem.GetRight();

#ifdef DEBUG
            long nTmp1 = DOCUMENTBORDER + nPageWidth;
            long nTmp2 = aTabCols.GetLeftMin() + nBorder;
#endif

            if ( aColItem.GetRight() > 0 )
                aTabCols.SetRight( nBorder );

            // Tabcols der Reihe nach
            // Die letzte Col wird durch den Rand definiert
            //columns in right-to-left tables need to be mirrored
            BOOL bIsTableRTL =
                IsTabColFromDoc() ?
                      rSh.IsMouseTableRightToLeft(aTabColFromDocPos)
                    : rSh.IsTableRightToLeft();
            if(bIsTableRTL)
            {
                USHORT nColCount = aColItem.Count() - 1;
                for ( USHORT i = 0; i < nColCount; ++i )
                {
                    const SvxColumnDescription& rCol = aColItem[nColCount - i];
                    aTabCols[i] = aTabCols.GetRight() - rCol.nStart;
                    aTabCols.SetHidden( i, !rCol.bVisible );
                }
            }
            else
            {
                for ( USHORT i = 0; i < aColItem.Count()-1; ++i )
                {
                    const SvxColumnDescription& rCol = aColItem[i];
                    aTabCols[i] = rCol.nEnd + aTabCols.GetLeft();
                    aTabCols.SetHidden( i, !rCol.bVisible );
                }
            }

            if ( bSetTabColFromDoc )
            {
                if( !rSh.IsViewLocked() )
                {
                    bUnlockView = TRUE;
                    rSh.LockView( TRUE );
                }
                rSh.SetMouseTabCols( aTabCols, bSingleLine,
                                               aTabColFromDocPos );
            }
            else
                rSh.SetTabCols(aTabCols, bSingleLine);

        }
        else
        {
            if ( bFrmSelection || nFrmType & FRMTYPE_FLY_ANY || bSect)
            {
                SwSectionFmt *pSectFmt = 0;
                SfxItemSet aSet( GetPool(), RES_COL, RES_COL );
                if(bSect)
                {
                    const SwSection *pSect = rSh.GetAnySection();
                    ASSERT( pSect, "Welcher Bereich?");
                    pSectFmt = pSect->GetFmt();
                }
                else
                {
                    rSh.GetFlyFrmAttr( aSet );
                }
                SwFmtCol aCols(
                    bSect ?
                        pSectFmt->GetCol() :
                            (const SwFmtCol&)aSet.Get( RES_COL, FALSE ));
                SwRect aCurRect = rSh.GetAnyCurRect(bSect ? RECT_SECTION_PRT : RECT_FLY_PRT_EMBEDDED);
                const long lWidth = bVerticalWriting ? aCurRect.Height() : aCurRect.Width();
                ::lcl_ConvertToCols( aColItem, USHORT(lWidth), aCols );
                aSet.Put( aCols );
                if(bSect)
                    rSh.SetSectionAttr( aSet, pSectFmt );
                else
                {
                    rSh.StartAction();
                    rSh.Push();
                    rSh.SetFlyFrmAttr( aSet );
                    //die Rahmenselektion wieder aufheben
                    if(!bFrmSelection && rSh.IsFrmSelected())
                    {
                        rSh.UnSelectFrm();
                        rSh.LeaveSelFrmMode();
                    }
                    rSh.Pop();
                    rSh.EndAction();
                }
            }
            else
            {
                SwFmtCol aCols( rDesc.GetMaster().GetCol() );
                const SwRect aPrtRect = rSh.GetAnyCurRect(RECT_PAGE_PRT);
                ::lcl_ConvertToCols( aColItem,
                    USHORT(bVerticalWriting ? aPrtRect.Height() : aPrtRect.Width()),
                                aCols );
                SwPageDesc aDesc( rDesc );
                aDesc.GetMaster().SetAttr( aCols );
                rSh.ChgPageDesc( rSh.GetCurPageDesc(), aDesc );
            }
        }
        break;
    }
        case SID_RULER_ROWS : 
        case SID_RULER_ROWS_VERTICAL:
        {
            SvxColumnItem aColItem((const SvxColumnItem&)rReq.
                                                GetArgs()->Get(nSlot));

            BOOL bSect = 0 != (nFrmType & FRMTYPE_COLSECT);
            if( bSetTabColFromDoc || !bSect && rSh.GetTableFmt() )
            {
                ASSERT(aColItem.Count(), "ColDesc ist leer!!");

                SwTabCols aTabCols;
                if ( bSetTabRowFromDoc )
                    rSh.GetMouseTabRows( aTabCols, aTabColFromDocPos );
                else
                    rSh.GetTabRows(aTabCols);

                if ( bVerticalWriting )
                {
                    aTabCols.SetRight(nPageWidth - aColItem.GetRight() - aColItem.GetLeft());
                    aTabCols.SetLeftMin(aColItem.GetLeft() + DOCUMENTBORDER);
                }
                else
                {
                    long nBorder = nPageHeight - aTabCols.GetLeftMin() +
                              DOCUMENTBORDER - aColItem.GetRight();
                    aTabCols.SetRight( nBorder );
                }

                if(bVerticalWriting)
                {
                    const long nPixelTwip = pEditWin->PixelToLogic(Size(1,1), MAP_TWIP).Width();
                    for ( USHORT i = aColItem.Count() - 1; i; --i )
                    {
                        const SvxColumnDescription& rCol = aColItem[i - 1];
                        long nColumnPos = aTabCols.GetRight() - rCol.nEnd ;
                        aTabCols[i - 1] = nColumnPos;
                        aTabCols.SetHidden( i - 1, !rCol.bVisible );
                    }
                }
                else
                {
                    for ( USHORT i = 0; i < aColItem.Count()-1; ++i )
                    {
                        const SvxColumnDescription& rCol = aColItem[i];
                        aTabCols[i] = rCol.nEnd + aTabCols.GetLeft();
                        aTabCols.SetHidden( i, !rCol.bVisible );
                    }
                }
                BOOL bSingleLine = FALSE;
                const SfxPoolItem* pSingleLine;
                if( SFX_ITEM_SET == rReq.GetArgs()->GetItemState(SID_RULER_ACT_LINE_ONLY, FALSE, &pSingleLine))
                    bSingleLine = ((const SfxBoolItem*)pSingleLine)->GetValue();
                if ( bSetTabRowFromDoc )
                {
                    if( !rSh.IsViewLocked() )
                    {
                        bUnlockView = TRUE;
                        rSh.LockView( TRUE );
                    }
                    rSh.SetMouseTabRows( aTabCols, bSingleLine, aTabColFromDocPos );
                }
                else
                    rSh.SetTabRows(aTabCols, bSingleLine);
            }
        }
        break;
    default:
        ASSERT( !this, "Falsche SlotId");
    }
    rSh.EndAllAction();

    if( bUnlockView )
        rSh.LockView( FALSE );

    bSetTabColFromDoc = bSetTabRowFromDoc = bTabColFromDoc = bTabRowFromDoc = FALSE;
    SetNumRuleNodeFromDoc(NULL);
}

/*--------------------------------------------------------------------
    Beschreibung:	Hier wird der Status der Tableiste ermittelt
                    sprich alle relevanten Attribute an der CursorPos
                    werden der Tableiste uebermittelt
 --------------------------------------------------------------------*/


void SwView::StateTabWin(SfxItemSet& rSet)
{
    SwWrtShell &rSh 		= GetWrtShell();

    const Point* pPt = IsTabColFromDoc() || IsTabRowFromDoc() ? &aTabColFromDocPos : 0;
    const USHORT nFrmType	= rSh.IsObjSelected()
                ? FRMTYPE_DRAWOBJ
                : rSh.GetFrmType( pPt, TRUE );

    const BOOL 	bFrmSelection = rSh.IsFrmSelected();

    BOOL bBrowse = rSh.IsBrowseMode();
    // PageOffset/Begrenzer
    const SwRect& rPageRect = rSh.GetAnyCurRect( RECT_PAGE, pPt );
    const SwRect& rPagePrtRect = rSh.GetAnyCurRect( RECT_PAGE_PRT, pPt );
    const long nPageWidth  = rPageRect.Width();
    const long nPageHeight = rPageRect.Height();

    const SwPageDesc& rDesc = rSh.GetPageDesc(
                IsTabColFromDoc() || bTabRowFromDoc ? 
                    rSh.GetMousePageDesc(aTabColFromDocPos) : rSh.GetCurPageDesc() );

    const SvxFrameDirectionItem& rFrameDir = rDesc.GetMaster().GetFrmDir();
    const BOOL bVerticalWriting = rSh.IsInVerticalText();

    //enable tab stop display on the rulers depending on the writing direction
    WinBits nRulerStyle = pHRuler->GetStyle() & ~WB_EXTRAFIELD;
    pHRuler->SetStyle(bVerticalWriting||bBrowse ? nRulerStyle : nRulerStyle|WB_EXTRAFIELD);

    nRulerStyle = pVRuler->GetStyle() & ~WB_EXTRAFIELD;
    pVRuler->SetStyle(bVerticalWriting ? nRulerStyle|WB_EXTRAFIELD : nRulerStyle);

    SvxLRSpaceItem aPageLRSpace( rDesc.GetMaster().GetLRSpace() );
    SwapPageMargin( rDesc, aPageLRSpace );

    SfxItemSet aCoreSet( GetPool(), RES_PARATR_TABSTOP, RES_PARATR_TABSTOP,
                                    RES_LR_SPACE, 		 RES_UL_SPACE, 0 );
    rSh.GetAttr( aCoreSet );
    USHORT nSelectionType = rSh.GetSelectionType();

    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    sal_Bool bPutContentProtection = sal_False;

    while ( nWhich )
    {
        switch ( nWhich )
        {
//        case RES_LR_SPACE:
//        case SID_ATTR_LRSPACE:
        case SID_ATTR_LONG_LRSPACE:
        {
            SvxLongLRSpaceItem aLongLR( (long)aPageLRSpace.GetLeft(),
                                        (long)aPageLRSpace.GetRight(),
                                        SID_ATTR_LONG_LRSPACE);
            if(bBrowse)
            {
                aLongLR.SetLeft(rPagePrtRect.Left());
                aLongLR.SetRight(nPageWidth - rPagePrtRect.Right());
            }
            if ( ( nFrmType & FRMTYPE_HEADER || nFrmType & FRMTYPE_FOOTER ) &&
                 !(nFrmType & FRMTYPE_COLSECT) )
            {
                SwFrmFmt *pFmt = (SwFrmFmt*) (nFrmType & FRMTYPE_HEADER ?
                                rDesc.GetMaster().GetHeader().GetHeaderFmt() :
                                rDesc.GetMaster().GetFooter().GetFooterFmt());
                SwRect aRect( rSh.GetAnyCurRect( RECT_HEADERFOOTER, pPt));
                aRect.Pos() -= rSh.GetAnyCurRect( RECT_PAGE, pPt ).Pos();
                const SvxLRSpaceItem& aLR = pFmt->GetLRSpace();
                aLongLR.SetLeft ( (long)aLR.GetLeft() + (long)aRect.Left() );
                aLongLR.SetRight( (nPageWidth -
                                    (long)aRect.Right() + (long)aLR.GetRight()));
            }
            else
            {
                SwRect aRect;
                if( !bFrmSelection && ((nFrmType & FRMTYPE_COLSECT) || rSh.IsDirectlyInSection()) )
                {
                    aRect = rSh.GetAnyCurRect(RECT_SECTION_PRT, pPt);
                    const SwRect aTmpRect = rSh.GetAnyCurRect(RECT_SECTION, pPt);
                    aRect.Pos() += aTmpRect.Pos();
                }

                else if ( bFrmSelection || nFrmType & FRMTYPE_FLY_ANY )
                    aRect = rSh.GetAnyCurRect(RECT_FLY_EMBEDDED, pPt);
                else if( nFrmType & FRMTYPE_DRAWOBJ)
                    aRect = rSh.GetObjRect();

                if( aRect.Width() )
                {
                    aLongLR.SetLeft ((long)(aRect.Left() - DOCUMENTBORDER));
                    aLongLR.SetRight((long)(nPageWidth + DOCUMENTBORDER - aRect.Right()));
                }
            }
            if( nWhich == SID_ATTR_LONG_LRSPACE )
                rSet.Put( aLongLR );
            else
            {
                SvxLRSpaceItem aLR( aLongLR.GetLeft(),
                                    aLongLR.GetRight(),
                                    nWhich);
                rSet.Put(aLR);
            }
            break;
        }
        case SID_ATTR_LONG_ULSPACE:
//        case SID_ATTR_ULSPACE:
//        case RES_UL_SPACE:
        {
            // Rand Seite Oben Unten
            SvxULSpaceItem aUL( rDesc.GetMaster().GetULSpace() );
            SvxLongULSpaceItem aLongUL( (long)aUL.GetUpper(),
                                        (long)aUL.GetLower(),
                                        SID_ATTR_LONG_ULSPACE);

            if ( bFrmSelection || nFrmType & FRMTYPE_FLY_ANY )
            {
                // Dokumentkoordinaten Frame auf Seitenkoordinaten umbrechen
                const SwRect &rRect = rSh.GetAnyCurRect(RECT_FLY_EMBEDDED, pPt);
                aLongUL.SetUpper((USHORT)(rRect.Top() - rPageRect.Top() ));
                aLongUL.SetLower((USHORT)(rPageRect.Bottom() - rRect.Bottom() ));
            }
            else if ( nFrmType & FRMTYPE_HEADER || nFrmType & FRMTYPE_FOOTER )
            {
                SwRect aRect( rSh.GetAnyCurRect( RECT_HEADERFOOTER, pPt));
                aRect.Pos() -= rSh.GetAnyCurRect( RECT_PAGE, pPt ).Pos();
                aLongUL.SetUpper( (USHORT)aRect.Top() );
                aLongUL.SetLower( (USHORT)(nPageHeight - aRect.Bottom()) );
            }
            else if( nFrmType & FRMTYPE_DRAWOBJ)
            {
                const SwRect &rRect = rSh.GetObjRect();
                aLongUL.SetUpper((rRect.Top() - rPageRect.Top()));
                aLongUL.SetLower((rPageRect.Bottom() - rRect.Bottom()));
            }
            else if(bBrowse)
            {
                aLongUL.SetUpper(rPagePrtRect.Top());
                aLongUL.SetLower(nPageHeight - rPagePrtRect.Bottom());
            }
            if( nWhich == SID_ATTR_LONG_ULSPACE )
                rSet.Put( aLongUL );
            else
            {
                SvxULSpaceItem aUL((USHORT)aLongUL.GetUpper(),
                                    (USHORT)aLongUL.GetLower(),
                                    nWhich);
                rSet.Put(aUL);
            }
            break;
        }
        case SID_ATTR_TABSTOP_VERTICAL :
        case RES_PARATR_TABSTOP:
        {
            if ( ISA( SwWebView ) ||
                 IsTabColFromDoc() ||
                 IsTabRowFromDoc() ||
                 ( nSelectionType & SwWrtShell::SEL_GRF) ||
                    (nSelectionType & SwWrtShell::SEL_FRM) ||
                    (nSelectionType & SwWrtShell::SEL_OLE) ||
                    SFX_ITEM_AVAILABLE > aCoreSet.GetItemState(RES_LR_SPACE)||
                    !bVerticalWriting && (SID_ATTR_TABSTOP_VERTICAL == nWhich)||
                    bVerticalWriting && (RES_PARATR_TABSTOP == nWhich)
                 )
                rSet.DisableItem( nWhich );
            else
            {	SvxTabStopItem aTabStops((const SvxTabStopItem&)
                                            aCoreSet.Get( RES_PARATR_TABSTOP ));

                const SvxTabStopItem& rDefTabs = (const SvxTabStopItem&)
                                            rSh.GetDefault(RES_PARATR_TABSTOP);

                DBG_ASSERT(pHRuler, "warum ist das Lineal nicht da?")
                long nDefTabDist = ::GetTabDist(rDefTabs);
                pHRuler->SetDefTabDist( nDefTabDist );
                pVRuler->SetDefTabDist( nDefTabDist );
                ::lcl_EraseDefTabs(aTabStops);
                rSet.Put(aTabStops, nWhich);
            }
            break;
        }
        case SID_ATTR_PARA_LRSPACE_VERTICAL:
        case SID_ATTR_PARA_LRSPACE:
        {
            if ( nSelectionType & SwWrtShell::SEL_GRF ||
                    nSelectionType & SwWrtShell::SEL_FRM ||
                    nSelectionType & SwWrtShell::SEL_OLE ||
                    nFrmType == FRMTYPE_DRAWOBJ ||
                    !bVerticalWriting && (SID_ATTR_PARA_LRSPACE_VERTICAL == nWhich)||
                    bVerticalWriting && (SID_ATTR_PARA_LRSPACE == nWhich)
                    )
            {
                rSet.DisableItem(nWhich);
            }
            else
            {
                SvxLRSpaceItem aLR;
                if ( !IsTabColFromDoc() )
                {
                    aLR = (const SvxLRSpaceItem&)aCoreSet.Get(RES_LR_SPACE);

                    // #i23726#
                    if (pNumRuleNodeFromDoc)
                    {
                        short nFLOffset =
                            pNumRuleNodeFromDoc->GetLeftMarginWithNum();
                        aLR.SetLeft(nFLOffset);
                    }
                }
                aLR.SetWhich(nWhich);
                rSet.Put(aLR);
            }
        break;
        }
        case SID_RULER_BORDER_DISTANCE:
        {
            nLeftBorderDistance = 0;
            nRightBorderDistance = 0;
            if ( nSelectionType & SwWrtShell::SEL_GRF ||
                    nSelectionType & SwWrtShell::SEL_FRM ||
                    nSelectionType & SwWrtShell::SEL_OLE ||
                    nFrmType == FRMTYPE_DRAWOBJ )
                rSet.DisableItem(SID_RULER_BORDER_DISTANCE);
            else
            {
                SvxLRSpaceItem aDistLR(SID_RULER_BORDER_DISTANCE);
                if(nFrmType & FRMTYPE_FLY_ANY)
                {
                    if( IsTabColFromDoc() )
                    {
                        const SwRect& rPrtRect = rSh.GetAnyCurRect(
                                        RECT_FLY_PRT_EMBEDDED, pPt );
                        aDistLR.SetLeft(rPrtRect.Left());
                        aDistLR.SetRight(rPrtRect.Left());
                    }
                    else
                    {
                        SfxItemSet aCoreSet( GetPool(),
                                                RES_BOX, RES_BOX,
                                                SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER, 0 );
                        SvxBoxInfoItem aBoxInfo;
                        aCoreSet.Put( aBoxInfo );
                        rSh.GetFlyFrmAttr( aCoreSet );
                        const SvxBoxItem& rBox = (const SvxBoxItem&)aCoreSet.Get(RES_BOX);
                        aDistLR.SetLeft((USHORT)rBox.GetDistance(BOX_LINE_LEFT ));
                        aDistLR.SetRight((USHORT)rBox.GetDistance(BOX_LINE_RIGHT));

                        //add the paragraph border distance
                        rSh.GetAttr( aCoreSet );
                        const SvxBoxItem& rParaBox = (const SvxBoxItem&)aCoreSet.Get(RES_BOX);
                        aDistLR.SetLeft(aDistLR.GetLeft() + (USHORT)rParaBox.GetDistance(BOX_LINE_LEFT ));
                        aDistLR.SetRight(aDistLR.GetRight() + (USHORT)rParaBox.GetDistance(BOX_LINE_RIGHT));
                    }
                    rSet.Put(aDistLR);
                    nLeftBorderDistance = aDistLR.GetLeft();
                    nRightBorderDistance = aDistLR.GetRight();
                }
                else if ( IsTabColFromDoc() ||
                    ( rSh.GetTableFmt() && !bFrmSelection &&
                    !(nFrmType & FRMTYPE_COLSECT ) ) )
                {
                    SfxItemSet aCoreSet( GetPool(),
                                            RES_BOX, RES_BOX,
                                            SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER, 0 );
                    SvxBoxInfoItem aBoxInfo;
                    aBoxInfo.SetTable(FALSE);
                    aBoxInfo.SetDist((BOOL) TRUE);
                    aCoreSet.Put(aBoxInfo);
                    rSh.GetTabBorders( aCoreSet );
                    const SvxBoxItem& rBox = (const SvxBoxItem&)aCoreSet.Get(RES_BOX);
                    SvxLRSpaceItem aDistLR(SID_RULER_BORDER_DISTANCE);
                    aDistLR.SetLeft((USHORT)rBox.GetDistance(BOX_LINE_LEFT ));
                    aDistLR.SetRight((USHORT)rBox.GetDistance(BOX_LINE_RIGHT));
                    
                    //add the border distance of the paragraph
                    rSh.GetAttr( aCoreSet );
                    const SvxBoxItem& rParaBox = (const SvxBoxItem&)aCoreSet.Get(RES_BOX);
                    aDistLR.SetLeft(aDistLR.GetLeft() + (USHORT)rParaBox.GetDistance(BOX_LINE_LEFT ));
                    aDistLR.SetRight(aDistLR.GetRight() + (USHORT)rParaBox.GetDistance(BOX_LINE_RIGHT));
                    rSet.Put(aDistLR);
                    nLeftBorderDistance = aDistLR.GetLeft();
                    nRightBorderDistance = aDistLR.GetRight();
                }
                else if ( !rSh.IsDirectlyInSection() )
                {
                    //get the page/header/footer border distance 
                    const SwFrmFmt& rMaster = rDesc.GetMaster();
                    const SvxBoxItem& rBox = (const SvxBoxItem&)rMaster.GetAttrSet().Get(RES_BOX);
                    SvxLRSpaceItem aDistLR(SID_RULER_BORDER_DISTANCE);
                    aDistLR.SetLeft((USHORT)rBox.GetDistance(BOX_LINE_LEFT ));
                    aDistLR.SetRight((USHORT)rBox.GetDistance(BOX_LINE_RIGHT));
                    
                    const SvxBoxItem* pBox = 0;
                    if(nFrmType & FRMTYPE_HEADER)
                    {        
                        rMaster.GetHeader();
                        const SwFmtHeader& rHeaderFmt = rMaster.GetHeader();
                        SwFrmFmt *pHeaderFmt = (SwFrmFmt*)rHeaderFmt.GetHeaderFmt();
                        pBox = & (const SvxBoxItem&)pHeaderFmt->GetBox();
                    }
                    else if(nFrmType & FRMTYPE_FOOTER )
                    {        
                        const SwFmtFooter& rFooterFmt = rMaster.GetFooter();
                        SwFrmFmt *pFooterFmt = (SwFrmFmt*)rFooterFmt.GetFooterFmt();
                        pBox = & (const SvxBoxItem&)pFooterFmt->GetBox();
                    }
                    if(pBox)
                    {
                        aDistLR.SetLeft((USHORT)pBox->GetDistance(BOX_LINE_LEFT ));
                        aDistLR.SetRight((USHORT)pBox->GetDistance(BOX_LINE_RIGHT));
                    }            
                    
                    //add the border distance of the paragraph
                    SfxItemSet aCoreSet( GetPool(),
                                            RES_BOX, RES_BOX,
                                            SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER, 0 );
                    rSh.GetAttr( aCoreSet );
                    const SvxBoxItem& rParaBox = (const SvxBoxItem&)aCoreSet.Get(RES_BOX);
                    aDistLR.SetLeft(aDistLR.GetLeft() + (USHORT)rParaBox.GetDistance(BOX_LINE_LEFT ));
                    aDistLR.SetRight(aDistLR.GetRight() + (USHORT)rParaBox.GetDistance(BOX_LINE_RIGHT));
                    rSet.Put(aDistLR);
                    nLeftBorderDistance = aDistLR.GetLeft();
                    nRightBorderDistance = aDistLR.GetRight();
                }        
            }
        }
        break;
        case SID_RULER_TEXT_RIGHT_TO_LEFT:
        {
            if ( nSelectionType & SwWrtShell::SEL_GRF ||
                    nSelectionType & SwWrtShell::SEL_FRM ||
                    nSelectionType & SwWrtShell::SEL_OLE ||
                    nFrmType == FRMTYPE_DRAWOBJ)
                rSet.DisableItem(nWhich);
            else
            {
                BOOL bFlag = rSh.IsInRightToLeftText();
                rSet.Put(SfxBoolItem(nWhich, bFlag));
            }
        }
        break;
        case SID_RULER_BORDERS_VERTICAL:
        case SID_RULER_BORDERS:
        {
            BOOL bFrameRTL;
            BOOL bFrameHasVerticalColumns =  rSh.IsFrmVertical(FALSE, bFrameRTL) && bFrmSelection;
            BOOL bHasTable = ( IsTabColFromDoc() || 
                    ( rSh.GetTableFmt() && !bFrmSelection &&
                    !(nFrmType & FRMTYPE_COLSECT ) ) );

            BOOL bTableVertical = bHasTable && rSh.IsTableVertical();

            if((SID_RULER_BORDERS_VERTICAL == nWhich) &&
                    ((bHasTable && !bTableVertical)||
                        (!bVerticalWriting && !bFrmSelection && !bHasTable ) || (bFrmSelection && !bFrameHasVerticalColumns)) ||
                ((SID_RULER_BORDERS == nWhich) &&
                    ((bHasTable && bTableVertical)||
                        (bVerticalWriting && !bFrmSelection&& !bHasTable) || bFrameHasVerticalColumns)))
                rSet.DisableItem(nWhich);
            else if ( bHasTable )
            {
                SwTabCols aTabCols;
                USHORT	  nNum;
                if ( 0 != ( bSetTabColFromDoc = IsTabColFromDoc() ) )
                {
                    rSh.GetMouseTabCols( aTabCols, aTabColFromDocPos );
                    nNum = rSh.GetCurMouseTabColNum( aTabColFromDocPos );
                }
                else
                {
                    rSh.GetTabCols( aTabCols );
                    nNum = rSh.GetCurTabColNum();
                    if(rSh.IsTableRightToLeft())
                        nNum = aTabCols.Count() - nNum;
                }

                ASSERT(nNum <= aTabCols.Count(), "TabCol not found");
                int nLft = aTabCols.GetLeftMin() -
                                  USHORT(DOCUMENTBORDER) +
                                  aTabCols.GetLeft();

                int nRgt = (USHORT)(bTableVertical ? nPageHeight : nPageWidth) -
                                  (aTabCols.GetLeftMin() +
                                  aTabCols.GetRight() -
                                  USHORT(DOCUMENTBORDER) );

                const USHORT nL = nLft > 0 ? nLft : 0;
                const USHORT nR = nRgt > 0 ? nRgt : 0;

                SvxColumnItem aColItem(nNum, nL, nR);

                USHORT nStart = 0,
                       nEnd;

                //columns in right-to-left tables need to be mirrored
                BOOL bIsTableRTL =
                    IsTabColFromDoc() ?
                          rSh.IsMouseTableRightToLeft(aTabColFromDocPos)
                        : rSh.IsTableRightToLeft();
                if(bIsTableRTL)
                {
                    for ( USHORT i = aTabCols.Count(); i ; --i )
                    {
                        const SwTabColsEntry& rEntry = aTabCols.GetEntry( i - 1 );
                        nEnd  = (USHORT)aTabCols.GetRight();
                        nEnd  -=  (USHORT)rEntry.nPos;
                        SvxColumnDescription aColDesc( nStart, nEnd,
                                    (USHORT(aTabCols.GetRight() - rEntry.nMax)), 
                                    (USHORT(aTabCols.GetRight() - rEntry.nMin)),
                                                    !aTabCols.IsHidden(i - 1) );
                        aColItem.Append(aColDesc);
                        nStart = nEnd;
                    }
                    SvxColumnDescription aColDesc(nStart,
                                    aTabCols.GetRight() - aTabCols.GetLeft(), TRUE);
                    aColItem.Append(aColDesc);
                }
                else
                {
                    for ( USHORT i = 0; i < aTabCols.Count(); ++i )
                    {
                        const SwTabColsEntry& rEntry = aTabCols.GetEntry( i );
                        nEnd  = rEntry.nPos - aTabCols.GetLeft();
                        SvxColumnDescription aColDesc( nStart, nEnd,
                                rEntry.nMin - aTabCols.GetLeft(), rEntry.nMax - aTabCols.GetLeft(), 
                                                    !aTabCols.IsHidden(i) );
                        aColItem.Append(aColDesc);
                        nStart = nEnd;
                    }
                    SvxColumnDescription aColDesc(nStart, aTabCols.GetRight() - aTabCols.GetLeft(), 
                                0, 0,
                                    TRUE);
                    aColItem.Append(aColDesc);
                }
                rSet.Put(aColItem, nWhich);
            }
            else if ( bFrmSelection || nFrmType & ( FRMTYPE_COLUMN | FRMTYPE_COLSECT ) )
            {
                // Aus Rahmen oder Seite ?
                USHORT nNum = 0;
                if(bFrmSelection)
                {
                    const SwFrmFmt* pFmt = rSh.GetFlyFrmFmt();
                    if(pFmt)
                        nNum = pFmt->GetCol().GetNumCols();
                }
                else
                    nNum = rSh.GetCurColNum();

                if(
                    //eigentlich sollte FRMTYPE_COLSECT nicht enthalten sein, wenn der Rahmen selektiert ist!
                    !bFrmSelection &&
                     nFrmType & FRMTYPE_COLSECT )
                {
                    const SwSection *pSect = rSh.GetAnySection(FALSE, pPt);
                    ASSERT( pSect, "Welcher Bereich?");
                    if( pSect )
                    {
                        SwSectionFmt *pFmt = pSect->GetFmt();
                        const SvxLRSpaceItem& rLRSpaceItem = pFmt->GetLRSpace();
                        const SwFmtCol& rCol = pFmt->GetCol();
                        if(rSh.IsInRightToLeftText())
                            nNum = rCol.GetColumns().Count() - nNum;
                        else
                            --nNum;
                        SvxColumnItem aColItem(nNum);
                        SwRect aRect = rSh.GetAnyCurRect(RECT_SECTION_PRT, pPt);
                        const SwRect aTmpRect = rSh.GetAnyCurRect(RECT_SECTION, pPt);

                        ::lcl_FillSvxColumn(rCol, USHORT(bVerticalWriting ? aRect.Height() : aRect.Width()), aColItem, 0);

                        if(bVerticalWriting)
                        {
                            aRect.Pos() += Point(aTmpRect.Left(), aTmpRect.Top());
                            aRect.Pos().Y() -= rPageRect.Top();
                            aColItem.SetLeft ((USHORT)(aRect.Top()));
                            aColItem.SetRight((USHORT)(nPageHeight   - aRect.Bottom() ));
                        }
                        else
                        {
                            aRect.Pos() += aTmpRect.Pos();
                            aColItem.SetLeft ((USHORT)(aRect.Left() - DOCUMENTBORDER ));
                            aColItem.SetRight((USHORT)(nPageWidth   - aRect.Right() +
                                                                        DOCUMENTBORDER ));
                        }
                        aColItem.SetOrtho(aColItem.CalcOrtho());

                        rSet.Put(aColItem, nWhich);
                    }
                }
                else if( bFrmSelection || nFrmType & FRMTYPE_FLY_ANY )
                {
                    // Spalten in Rahmen
                    if ( nNum  )
                    {
                        const SwFrmFmt* pFmt = rSh.GetFlyFrmFmt() ;

                        const SwFmtCol& rCol = pFmt->GetCol();
                        if(rSh.IsInRightToLeftText())
                            nNum = rCol.GetColumns().Count() - nNum;
                        else 
                            nNum--;
                        SvxColumnItem aColItem(nNum);
                        const SwRect &rSizeRect = rSh.GetAnyCurRect(RECT_FLY_PRT_EMBEDDED, pPt);

                        BOOL bUseVertical = bFrameHasVerticalColumns || (!bFrmSelection && bVerticalWriting);
                        const long lWidth = bUseVertical ? rSizeRect.Height() : rSizeRect.Width();
                        const SwRect &rRect = rSh.GetAnyCurRect(RECT_FLY_EMBEDDED, pPt);
                        long nDist2 = ((bUseVertical ? rRect.Height() : rRect.Width()) - lWidth) /2;
                        ::lcl_FillSvxColumn(rCol, USHORT(lWidth), aColItem, nDist2);

                        SfxItemSet aFrameSet(GetPool(), RES_LR_SPACE, RES_LR_SPACE);
                        rSh.GetFlyFrmAttr( aFrameSet );

                        if(bUseVertical)
                        {
                            aColItem.SetLeft ((USHORT)(rRect.Top()- rPageRect.Top()));
                            aColItem.SetRight((USHORT)(nPageHeight + rPageRect.Top() - rRect.Bottom() ));
                        }
                        else
                        {
                            aColItem.SetLeft ((USHORT)(rRect.Left() - DOCUMENTBORDER ));
                            aColItem.SetRight((USHORT)(nPageWidth	- rRect.Right() -
                                                                    DOCUMENTBORDER ));
                        }

                        aColItem.SetOrtho(aColItem.CalcOrtho());

                        rSet.Put(aColItem, nWhich);
                    }
                    else
                        rSet.DisableItem(nWhich);
                }
                else
                {	// Spalten auf der Seite
                    const SwFrmFmt& rMaster = rDesc.GetMaster();
                    SwFmtCol aCol(rMaster.GetCol());
                    if(rFrameDir.GetValue() == FRMDIR_HORI_RIGHT_TOP)
                        nNum = aCol.GetColumns().Count() - nNum;
                    else
                        nNum--;

                    SvxColumnItem aColItem(nNum);
                    const SwRect aPrtRect = rSh.GetAnyCurRect(RECT_PAGE_PRT, pPt);
                    const SvxBoxItem& rBox = (const SvxBoxItem&)rMaster.GetAttr(RES_BOX);
                    long nDist = rBox.GetDistance();
                    ::lcl_FillSvxColumn(aCol,
                        USHORT(bVerticalWriting ? aPrtRect.Height() : aPrtRect.Width() ),
                        aColItem, nDist);

                    if(bBrowse)
                    {
                        aColItem.SetLeft((USHORT)rPagePrtRect.Left());
                        aColItem.SetRight(USHORT(nPageWidth - rPagePrtRect.Right()));
                    }
                    else
                    {
                        aColItem.SetLeft (aPageLRSpace.GetLeft());
                        aColItem.SetRight(aPageLRSpace.GetRight());
                    }
                    aColItem.SetOrtho(aColItem.CalcOrtho());

                    rSet.Put(aColItem, nWhich);
                }
            }
            else
                rSet.DisableItem(nWhich);
            break;
        }
        case SID_RULER_ROWS : 
        case SID_RULER_ROWS_VERTICAL:
        {    
            BOOL bFrameRTL;
            BOOL bFrameHasVerticalColumns =  rSh.IsFrmVertical(FALSE, bFrameRTL) && bFrmSelection;

            if((SID_RULER_ROWS == nWhich) &&
                    ((!bVerticalWriting && !bFrmSelection) || (bFrmSelection && !bFrameHasVerticalColumns)) ||
                ((SID_RULER_ROWS_VERTICAL == nWhich) &&
                    ((bVerticalWriting && !bFrmSelection) || bFrameHasVerticalColumns)))
                rSet.DisableItem(nWhich);
            else if ( IsTabRowFromDoc() ||
                    ( rSh.GetTableFmt() && !bFrmSelection &&
                    !(nFrmType & FRMTYPE_COLSECT ) ) )
            {
                SwTabCols aTabCols;
                //no current value necessary
                USHORT    nNum = 0;
                if ( 0 != ( bSetTabRowFromDoc = IsTabRowFromDoc() ) )
                {
                    rSh.GetMouseTabRows( aTabCols, aTabColFromDocPos );
                }
                else
                {
                    rSh.GetTabRows( aTabCols );
                }

//                ASSERT(nNum <= aTabCols.Count(), "TabCol not found");
                int nLft = aTabCols.GetLeftMin() -
                                  USHORT(DOCUMENTBORDER);

                int nRgt = (USHORT)(bVerticalWriting ? nPageWidth : nPageHeight) -
                                  (aTabCols.GetLeftMin() +
                                  aTabCols.GetRight() -
                                  USHORT(DOCUMENTBORDER) );

                const USHORT nL = nLft > 0 ? nLft : 0;
                const USHORT nR = nRgt > 0 ? nRgt : 0;

                SvxColumnItem aColItem(nNum, nL, nR);

                USHORT nStart = 0,
                       nEnd;

                for ( USHORT i = 0; i < aTabCols.Count(); ++i )
                {
                    const SwTabColsEntry& rEntry = aTabCols.GetEntry( i );
                    if(bVerticalWriting)
                    {
                        nEnd  = USHORT(aTabCols.GetRight() - rEntry.nPos);
                        SvxColumnDescription aColDesc( nStart, nEnd, 
                            aTabCols.GetRight() - rEntry.nMax, aTabCols.GetRight() - rEntry.nMin, 
                                                    !aTabCols.IsHidden(i) );
                        aColItem.Append(aColDesc);
                    }
                    else
                    {
                        nEnd  = USHORT(rEntry.nPos - aTabCols.GetLeft());
                        SvxColumnDescription aColDesc( nStart, nEnd, 
                                USHORT(rEntry.nMin - aTabCols.GetLeft()), USHORT(rEntry.nMax - aTabCols.GetLeft()),
                                                    !aTabCols.IsHidden(i) );
                        aColItem.Append(aColDesc);
                    }
                    nStart = nEnd;
                }
                if(bVerticalWriting)
                    nEnd = aTabCols.GetRight();
                else
                    nEnd = aTabCols.GetLeft();
                // put a position protection when the last row cannot be moved
                // due to a page break inside of a row
                if(!aTabCols.IsLastRowAllowedToChange())
                    bPutContentProtection = sal_True;
                        
                SvxColumnDescription aColDesc( nStart, nEnd, 
                    aTabCols.GetRight(), aTabCols.GetRight(), 
                                            FALSE );
                aColItem.Append(aColDesc);
                            
                rSet.Put(aColItem, nWhich);
            }
            else
                rSet.DisableItem(nWhich);
        }
        break;
        case SID_RULER_PAGE_POS:
        {
            SvxPagePosSizeItem aPagePosSize(
                    Point( DOCUMENTBORDER , rPageRect.Top()) ,
                    nPageWidth, nPageHeight);
            rSet.Put(aPagePosSize);
            break;
        }
        case SID_RULER_LR_MIN_MAX:
        {
            Rectangle aRectangle;
            if( ( nFrmType & FRMTYPE_COLSECT ) && !IsTabColFromDoc() &&
                ( nFrmType & ( FRMTYPE_TABLE|FRMTYPE_COLUMN ) ) )
            {
                if( nFrmType & FRMTYPE_TABLE )
                {
                    const USHORT nNum = rSh.GetCurTabColNum();
                    SwTabCols aTabCols;
                    rSh.GetTabCols( aTabCols );

                    int nLft = aTabCols.GetLeftMin() -
                                    USHORT(DOCUMENTBORDER) +
                                    aTabCols.GetLeft();

                    int nRgt = (USHORT)nPageWidth -
                                    (aTabCols.GetLeftMin() +
                                    aTabCols.GetRight() -
                                    USHORT(DOCUMENTBORDER) );

                    const USHORT nL = nLft > 0 ? nLft : 0;
                    const USHORT nR = nRgt > 0 ? nRgt : 0;

                    aRectangle.Left() = nL;
                    if(nNum > 1)
                        aRectangle.Left() += aTabCols[nNum - 2];
                    if(nNum)
                        aRectangle.Left() += MINLAY;
                    if(aTabCols.Count() <= nNum + 1 )
                        aRectangle.Right() = nR;
                    else
                        aRectangle.Right() = nPageWidth - (nL + aTabCols[nNum + 1]);

                    if(nNum < aTabCols.Count())
                        aRectangle.Right() += MINLAY;
                }
                else
                {
                    const SwFrmFmt* pFmt =  rSh.GetFlyFrmFmt();
                    const SwFmtCol* pCols = pFmt ? &pFmt->GetCol():
                                                   &rDesc.GetMaster().GetCol();
                    const SwColumns& rCols = pCols->GetColumns();
                    USHORT nNum = rSh.GetCurOutColNum();
                    USHORT nCount = Min(USHORT(nNum + 1), rCols.Count());
                    const SwRect aRect( rSh.GetAnyCurRect( pFmt
                                                    ? RECT_FLY_PRT_EMBEDDED
                                                    : RECT_PAGE_PRT, pPt ));
                    const SwRect aAbsRect( rSh.GetAnyCurRect( pFmt
                                                    ? RECT_FLY_EMBEDDED
                                                    : RECT_PAGE, pPt ));

                    //die Breite im Rahmen bzw. innerhalbe der Seitenraender
                    const USHORT nTotalWidth = (USHORT)aRect.Width();
                    //die gesamte Rahmenbreite - die Differenz ist der doppelte Abstand zum Rand
                    const USHORT nOuterWidth = (USHORT)aAbsRect.Width();
                    int nWidth = 0,
                        nStart = 0,
                        nEnd = 0;
                    aRectangle.Left() = 0;
                    for ( int i = 0; i < nCount; ++i )
                    {
                        SwColumn* pCol = rCols[i];
                        nStart = pCol->GetLeft() + nWidth;
                        if(i == nNum - 2)
                            aRectangle.Left() = nStart;
                        nWidth += pCols->CalcColWidth( i, nTotalWidth );
                        nEnd = nWidth - pCol->GetRight();
                    }
                    aRectangle.Right() = nPageWidth - nEnd;
                    aRectangle.Left() -= DOCUMENTBORDER;
                    aRectangle.Right() += DOCUMENTBORDER;
                    if(nNum > 1)
                    {
                        aRectangle.Left() += MINLAY;
                        aRectangle.Left() += aRect.Left();
                    }
                    if(pFmt) //Bereich in Rahmen - hier darf man bis zum Rand
                        aRectangle.Left()  = aRectangle.Right() = 0;
                    else
                    {
                        // das Rechteck an die richtige absolute Position verschieben
                        aRectangle.Left() += aAbsRect.Left();
                        aRectangle.Right() -= aAbsRect.Left();
                        // Abstand zur Umrandung mit einbeziehen
                        aRectangle.Right() -= (nOuterWidth - nTotalWidth) / 2;
                    }

                    if(nNum < rCols.Count())
                    {
                        aRectangle.Right() += MINLAY;
                    }
                    else
                        // rechts ist jetzt nur noch der Seitenrand
                        aRectangle.Right() = 0;


                }
            }
            else if ( ((nFrmType & FRMTYPE_TABLE) || IsTabColFromDoc()) &&
                 !bFrmSelection )
            {
                BOOL bColumn;
                if ( IsTabColFromDoc() )
                    bColumn = rSh.GetCurMouseColNum( aTabColFromDocPos ) != 0;
                else
                    bColumn = (nFrmType & (FRMTYPE_COLUMN|FRMTYPE_FLY_ANY|
                                            FRMTYPE_COLSECTOUTTAB)) ?
                                            TRUE : FALSE;
                if ( !bColumn )
                {
                    if( nFrmType & FRMTYPE_FLY_ANY && IsTabColFromDoc() )
                    {
                        SwRect aRect( rSh.GetAnyCurRect(
                                            RECT_FLY_PRT_EMBEDDED, pPt ) );
                        aRect.Pos() += rSh.GetAnyCurRect( RECT_FLY_EMBEDDED,
                                                                pPt ).Pos();

                        aRectangle.Left()  = aRect.Left() - DOCUMENTBORDER;
                        aRectangle.Right() = nPageWidth - ( aRect.Right()
                                                         - DOCUMENTBORDER );
                    }
                    else if( bBrowse )
                    {
                        aRectangle.Left()  = rPagePrtRect.Left();
                        aRectangle.Right() = nPageWidth - rPagePrtRect.Right();
                    }
                    else
                    {
                        aRectangle.Left()  = aPageLRSpace.GetLeft();
                        aRectangle.Right() = aPageLRSpace.GetRight();
                    }
                }
                else
                {	//hier nur fuer Tabelle in mehrspaltigen Seiten und Rahmen
                    BOOL bSectOutTbl = (nFrmType & FRMTYPE_TABLE) ? TRUE : FALSE;
                    BOOL bFrame = (nFrmType & FRMTYPE_FLY_ANY) ? TRUE : FALSE;
                    BOOL bColSct = 	(nFrmType & ( bSectOutTbl
                                                    ? FRMTYPE_COLSECTOUTTAB
                                                    : FRMTYPE_COLSECT )
                                                ) ? TRUE : FALSE;
                    //Damit man auch mit der Mouse ziehen kann,
                    //ohne in der Tabelle zu stehen
                    CurRectType eRecType = RECT_PAGE_PRT;
                    int nNum = IsTabColFromDoc() ?
                                rSh.GetCurMouseColNum( aTabColFromDocPos ):
                                rSh.GetCurOutColNum();
                    const SwFrmFmt* pFmt = NULL;
                    if( bColSct )
                    {
                        eRecType = bSectOutTbl ? RECT_OUTTABSECTION
                                               : RECT_SECTION;
                        const SwSection *pSect = rSh.GetAnySection( bSectOutTbl, pPt );
                        ASSERT( pSect, "Welcher Bereich?");
                        pFmt = pSect->GetFmt();
                    }
                    else if( bFrame )
                    {
                        pFmt = rSh.GetFlyFrmFmt();
                        eRecType = RECT_FLY_PRT_EMBEDDED;
                    }

                    const SwFmtCol* pCols = pFmt ? &pFmt->GetCol():
                                                   &rDesc.GetMaster().GetCol();
                    const SwColumns& rCols = pCols->GetColumns();
                    const USHORT nBorder = pFmt ? pFmt->GetBox().GetDistance() :
                                                  rDesc.GetMaster().GetBox().GetDistance();

                    /* RECT_FLY_PRT_EMBEDDED returns the relative position to
                        RECT_FLY_EMBEDDED
                        the absolute position must be added here
                    */
                    SwRect aRect( rSh.GetAnyCurRect( eRecType, pPt ) );
                    if(RECT_FLY_PRT_EMBEDDED == eRecType)
                        aRect.Pos() += rSh.GetAnyCurRect( RECT_FLY_EMBEDDED,
                                                                pPt ).Pos();

                    const USHORT nTotalWidth = (USHORT)aRect.Width();
                    //nStart und nEnd initialisieren fuer nNum == 0
                    int nWidth = 0,
                        nStart = 0,
                        nEnd = nTotalWidth;

                    if( nNum > rCols.Count() )
                    {
                        ASSERT( !this, "es wird auf dem falschen FmtCol gearbeitet!" );
                        nNum = rCols.Count();
                    }

                    for( int i = 0; i < nNum; ++i )
                    {
                        SwColumn* pCol = rCols[i];
                        nStart = pCol->GetLeft() + nWidth;
                        nWidth += pCols->CalcColWidth( i, nTotalWidth );
                        nEnd = nWidth - pCol->GetRight();
                    }
                    if( bFrame | bColSct )
                    {
                        aRectangle.Left()  = aRect.Left()
                                                - DOCUMENTBORDER + nStart;
                        aRectangle.Right() = nPageWidth
                                                - aRectangle.Left()
                                                    - nEnd + nStart;
                    }
                    else if(!bBrowse)
                    {
                        aRectangle.Left()  = aPageLRSpace.GetLeft() + nStart;
                        aRectangle.Right() = nPageWidth - nEnd - aPageLRSpace.GetLeft();
                    }
                    else
                    {
                        long nLeft = rPagePrtRect.Left();
                        aRectangle.Left()  = nStart + nLeft;
                        aRectangle.Right() = nPageWidth - nEnd - nLeft;
                    }
                    if(!bFrame)
                    {
                        aRectangle.Left() += nBorder;
                        aRectangle.Right() -= nBorder;
                    }
                }
            }
            else if ( nFrmType & ( FRMTYPE_HEADER  | FRMTYPE_FOOTER ))
            {
                aRectangle.Left()  = aPageLRSpace.GetLeft();
                aRectangle.Right() = aPageLRSpace.GetRight();
            }
            else
                aRectangle.Left()  = aRectangle.Right() = 0;

            SfxRectangleItem aLR( SID_RULER_LR_MIN_MAX , aRectangle);
            rSet.Put(aLR);
        }
        break;
        case SID_RULER_PROTECT:
        {
            if(bFrmSelection)
            {
                BYTE nProtect = pWrtShell->IsSelObjProtected((FlyProtectType)(FLYPROTECT_SIZE|FLYPROTECT_POS|FLYPROTECT_CONTENT));

                SvxProtectItem aProt(SID_RULER_PROTECT);
                aProt.SetCntntProtect((nProtect & FLYPROTECT_CONTENT)	!= 0);
                aProt.SetSizeProtect ((nProtect & FLYPROTECT_SIZE)		!= 0);
                aProt.SetPosProtect  ((nProtect & FLYPROTECT_POS)		!= 0);
                rSet.Put(aProt);
            }
            else
            {
                SvxProtectItem aProtect(SID_RULER_PROTECT);
                if(bBrowse && !(nFrmType & (FRMTYPE_DRAWOBJ|FRMTYPE_COLUMN)) && !rSh.GetTableFmt())
                {
                    aProtect.SetSizeProtect(TRUE);
                    aProtect.SetPosProtect(TRUE);
                }
                rSet.Put(aProtect);
            }
        }
        break;
        }
        nWhich = aIter.NextWhich();
    }
    if(bPutContentProtection)
    {        
        SvxProtectItem aProtect(SID_RULER_PROTECT);
        aProtect.SetCntntProtect(TRUE);
        rSet.Put(aProtect);
    }
}


