/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swtablerep.cxx,v $
 * $Revision: 1.7 $
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
#include <tools/list.hxx>
#include <vcl/msgbox.hxx>
#include <svl/stritem.hxx>
#include <svl/intitem.hxx>
#include <svx/htmlmode.hxx>
#include <editeng/keepitem.hxx>
#include <editeng/brkitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/frmdiritem.hxx>
#include <svl/ctloptions.hxx>
#include <swmodule.hxx>
#include <fmtornt.hxx>
#include <fmtpdsc.hxx>
#include <fmtlsplt.hxx>

#include <svx/htmlcfg.hxx>
#include <fmtrowsplt.hxx>
#include <svx/htmlmode.hxx>

#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <wrtsh.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <viewopt.hxx>
#include <uitool.hxx>
#include <frmatr.hxx>

#ifndef _TABLEPG_HXX
#include <tablepg.hxx>
#endif
#include <tablemgr.hxx>
#include <pagedesc.hxx>
#include <poolfmt.hxx>
#include <SwStyleNameMapper.hxx>

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _TABLEDLG_HRC
#include <tabledlg.hrc>
#endif
#ifndef _TABLE_HRC
#include <table.hrc>
#endif
#include "swtablerep.hxx"

#ifdef DEBUG_TBLDLG

void DbgTColumn(TColumn* pTColumn, USHORT nCount)
{
    for(USHORT i = 0; i < nCount; i++)
    {
        String sMsg(i);
        sMsg += pTColumn[i].bVisible ? " v " : " h ";
        sMsg += pTColumn[i].nWidth;
        DBG_ERROR(sMsg)
    }
}
#endif


/*-----------------20.08.96 09.43-------------------
--------------------------------------------------*/
SwTableRep::SwTableRep( const SwTabCols& rTabCol, BOOL bCplx )
    :
    nTblWidth(0),
    nSpace(0),
    nLeftSpace(0),
    nRightSpace(0),
    nAlign(0),
    nWidthPercent(0),
    bComplex(bCplx),
    bLineSelected(FALSE),
    bWidthChanged(FALSE),
    bColsChanged(FALSE)
{
    nAllCols = nColCount = rTabCol.Count();
    pTColumns = new TColumn[ nColCount + 1 ];
    SwTwips nStart = 0,
            nEnd;
    for( USHORT i = 0; i < nAllCols; ++i )
    {
        nEnd  = rTabCol[ i ] - rTabCol.GetLeft();
        pTColumns[ i ].nWidth = nEnd - nStart;
        pTColumns[ i ].bVisible = !rTabCol.IsHidden(i);
        if(!pTColumns[ i ].bVisible)
            nColCount --;
        nStart = nEnd;
    }
    pTColumns[ nAllCols ].nWidth = rTabCol.GetRight() - rTabCol.GetLeft() - nStart;
    pTColumns[ nAllCols ].bVisible = TRUE;
    nColCount++;
    nAllCols++;
}

/*-----------------20.08.96 09.43-------------------
--------------------------------------------------*/
SwTableRep::~SwTableRep()
{
    delete[] pTColumns;
}

/*-----------------20.08.96 13.33-------------------
--------------------------------------------------*/
BOOL SwTableRep::FillTabCols( SwTabCols& rTabCols ) const
{
    long nOldLeft = rTabCols.GetLeft(),
         nOldRight = rTabCols.GetRight();

    BOOL bSingleLine = FALSE;
    USHORT i;

    for ( i = 0; i < rTabCols.Count(); ++i )
        if(!pTColumns[i].bVisible)
        {
            bSingleLine = TRUE;
            break;
        }

#ifdef DEBUG_TBLDLG
#define DbgTColumn(pTColumns, nAllCols);
#endif

    SwTwips nPos = 0;
    SwTwips nLeft = GetLeftSpace();
    rTabCols.SetLeft(nLeft);
    if(bSingleLine)
    {
        // die unsichtbaren Trenner werden aus den alten TabCols genommen
        // die sichtbaren kommen aus pTColumns
        TColumn* 	pOldTColumns = new TColumn[nAllCols + 1];
        SwTwips nStart = 0,
                nEnd;
        for(i = 0; i < nAllCols - 1; i++)
        {
            nEnd  = rTabCols[i] - rTabCols.GetLeft();
            pOldTColumns[i].nWidth = nEnd - nStart;
            pOldTColumns[i].bVisible = !rTabCols.IsHidden(i);
            nStart = nEnd;
        }
        pOldTColumns[nAllCols - 1].nWidth = rTabCols.GetRight() - rTabCols.GetLeft() - nStart;
        pOldTColumns[nAllCols - 1].bVisible = TRUE;

#ifdef DEBUG_TBLDLG
#define DbgTColumn(pOldTColumns, nAllCols);
#endif

        USHORT nOldPos = 0;
        USHORT nNewPos = 0;
        SwTwips nOld = 0;
        SwTwips nNew = 0;
        BOOL bOld = FALSE;
        BOOL bFirst = TRUE;
        i = 0;

        while ( i < nAllCols -1 )
        {
            while((bFirst || bOld ) && nOldPos < nAllCols )
            {
                nOld += pOldTColumns[nOldPos].nWidth;
                nOldPos++;
                if(!pOldTColumns[nOldPos - 1].bVisible)
                    break;
            }
            while((bFirst || !bOld ) && nNewPos < nAllCols )
            {
                nNew += pTColumns[nNewPos].nWidth;
                nNewPos++;
                if(pOldTColumns[nNewPos - 1].bVisible)
                    break;
            }
            bFirst = FALSE;
            // sie muessen sortiert eingefuegt werden
            bOld = nOld < nNew;
            nPos = USHORT(bOld ? nOld : nNew);
            rTabCols[i] = nPos + nLeft;
            rTabCols.SetHidden( i, bOld );
            i++;
        }
        rTabCols.SetRight(nLeft + nTblWidth);

        delete[] pOldTColumns;
    }
    else
    {
        for ( i = 0; i < nAllCols - 1; ++i )
        {
            nPos += pTColumns[i].nWidth;
            rTabCols[i] = nPos + rTabCols.GetLeft();
            rTabCols.SetHidden( i, !pTColumns[i].bVisible );
            rTabCols.SetRight(nLeft + pTColumns[nAllCols - 1].nWidth + nPos);
        }
    }

// Rundungsfehler abfangen
    if(Abs((long)nOldLeft - (long)rTabCols.GetLeft()) < 3)
        rTabCols.SetLeft(nOldLeft);

    if(Abs((long)nOldRight - (long)rTabCols.GetRight()) < 3)
        rTabCols.SetRight(nOldRight);

    if(GetRightSpace() >= 0 &&
            rTabCols.GetRight() > rTabCols.GetRightMax())
        rTabCols.SetRight(rTabCols.GetRightMax());
    return bSingleLine;
}
