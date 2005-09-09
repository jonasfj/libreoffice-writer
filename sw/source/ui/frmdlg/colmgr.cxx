/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: colmgr.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 07:49:02 $
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

#include "hintids.hxx"

#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif

#include "frmmgr.hxx"
#include "frmfmt.hxx"
#include "colmgr.hxx"


// PRIVATE METHODES ------------------------------------------------------
/*------------------------------------------------------------------------
 Beschreibung:	Spaltenbreite auf aktuelle Breite einstellen
------------------------------------------------------------------------*/



void FitToActualSize(SwFmtCol& rCol, USHORT nWidth)
{
    const USHORT nCount = rCol.GetColumns().Count();
    for(USHORT i = 0; i < nCount; ++i)
    {
        const USHORT nTmp = rCol.CalcColWidth(i, nWidth);
        rCol.GetColumns()[i]->SetWishWidth(nTmp);
    }
    rCol.SetWishWidth(nWidth);
}


// PUBLIC METHODES -------------------------------------------------------
/*------------------------------------------------------------------------
 Beschreibung:	Setzen Spaltenanzahl und Gutterwidth
------------------------------------------------------------------------*/



void SwColMgr::SetCount(USHORT nCount, USHORT  nGutterWidth)
{
    aFmtCol.Init(nCount, nGutterWidth, nWidth);
    aFmtCol.SetWishWidth(nWidth);
    aFmtCol.SetGutterWidth(nGutterWidth, nWidth);
}



USHORT SwColMgr::GetGutterWidth( USHORT nPos ) const
{
    USHORT nRet;
    if(nPos == USHRT_MAX )
        nRet = GetCount() > 1 ? aFmtCol.GetGutterWidth() : DEF_GUTTER_WIDTH;
    else
    {
        DBG_ASSERT(nPos < GetCount() - 1, "Spalte ueberindiziert" )
        const SwColumns& rCols = aFmtCol.GetColumns();
        nRet = rCols.GetObject(nPos)->GetRight() + rCols.GetObject(nPos + 1)->GetLeft();
    }
    return nRet;
}

/*-----------------22.10.96 14.28-------------------

--------------------------------------------------*/


void SwColMgr::SetGutterWidth(USHORT nGutterWidth, USHORT nPos )
{
    if(nPos == USHRT_MAX)
        aFmtCol.SetGutterWidth(nGutterWidth, nWidth);
    else
    {
        DBG_ASSERT(nPos < GetCount() - 1, "Spalte ueberindiziert" )
        SwColumns& rCols = aFmtCol.GetColumns();
        USHORT nGutterWidth2 = nGutterWidth / 2;
        rCols.GetObject(nPos)->SetRight(nGutterWidth2);
        rCols.GetObject(nPos + 1)->SetLeft(nGutterWidth2);
    }
}

/*------------------------------------------------------------------------
 Beschreibung:	Hoehe Trennlinie
------------------------------------------------------------------------*/



short SwColMgr::GetLineHeightPercent() const
{
    return (short)aFmtCol.GetLineHeight();
}



void SwColMgr::SetLineHeightPercent(short nPercent)
{
    ASSERT(nPercent <= 100, LineHeight darf nur bis 100 % gross  sein);
    aFmtCol.SetLineHeight((BYTE)nPercent);
}
/*------------------------------------------------------------------------
 Beschreibung:	Spaltenbreite
------------------------------------------------------------------------*/



USHORT SwColMgr::GetColWidth(USHORT nIdx) const
{
    ASSERT(nIdx < GetCount(), Spaltenarray ueberindiziert.);
    return aFmtCol.CalcPrtColWidth(nIdx, nWidth);
}



void SwColMgr::SetColWidth(USHORT nIdx, USHORT nWd)
{
    ASSERT(nIdx < GetCount(), Spaltenarray ueberindiziert.);
    aFmtCol.GetColumns()[nIdx]->SetWishWidth(nWd);

}

/*--------------------------------------------------------------------
    Beschreibung: 	Groesse neu setzen
 --------------------------------------------------------------------*/



void SwColMgr::SetActualWidth(USHORT nW)
{
    nWidth = nW;
    ::FitToActualSize(aFmtCol, nW);
}

/*--------------------------------------------------------------------
    Beschreibung: ctor
 --------------------------------------------------------------------*/



SwColMgr::SwColMgr(const SfxItemSet& rSet, USHORT nActWidth) :
    aFmtCol((const SwFmtCol&)rSet.Get(RES_COL)),
    nWidth(nActWidth)
{
    if(nWidth == USHRT_MAX)
    {
        nWidth = (USHORT)((const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE)).GetWidth();
        if (nWidth < MINLAY)
            nWidth = USHRT_MAX;
        const SvxLRSpaceItem &rLR = (const SvxLRSpaceItem&)rSet.Get(RES_LR_SPACE);
        nWidth -= (USHORT)rLR.GetLeft();
        nWidth -= (USHORT)rLR.GetRight();
    }
    ::FitToActualSize(aFmtCol, nWidth);
}




SwColMgr::~SwColMgr() {}






