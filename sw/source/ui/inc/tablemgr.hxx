/*************************************************************************
 *
 *  $RCSfile: tablemgr.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 09:02:31 $
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
#ifndef _TABLEMGR_HXX
#define _TABLEMGR_HXX

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _SWTYPES_HXX
#include "swtypes.hxx"
#endif
#ifndef _TABCOL_HXX
#include "tabcol.hxx"
#endif

class SwFrmFmt;
class SwWrtShell;
class Window;
class SchMemChart;
class SfxItemSet;

const SwTwips lAutoWidth = INVALID_TWIPS;
const char cParaDelim = 0x0a;

class SW_DLLPUBLIC SwTableFUNC
{
    SwFrmFmt	*pFmt;
    SwWrtShell	*pSh;
    BOOL		bCopy;
    SwTabCols	aCols;

private:
    SW_DLLPRIVATE int GetRightSeparator(int nNum) const;

public:
    inline SwTableFUNC(SwFrmFmt &);
           SwTableFUNC(SwWrtShell *pShell, BOOL bCopyFmt = FALSE);
           ~SwTableFUNC();

    void	InitTabCols();
    void	ColWidthDlg(Window *pParent );
    SwTwips GetColWidth(USHORT nNum) const;
    SwTwips GetMaxColWidth(USHORT nNum) const;
    void	SetColWidth(USHORT nNum, SwTwips nWidth );
    USHORT	GetColCount() const;
    USHORT	GetCurColNum() const;

    BOOL IsTableSelected() const { return pFmt != 0; }

    const SwFrmFmt *GetTableFmt() const { return pFmt; }

    SwWrtShell* GetShell() const { return pSh; }

    void InsertChart( SchMemChart& rData, const SfxItemSet* pSet = 0 );
    void UpdateChart();
};

inline SwTableFUNC::SwTableFUNC(SwFrmFmt &rFmt) :
    pFmt(&rFmt),
    bCopy(FALSE),
    pSh(0)
{
}

#endif
