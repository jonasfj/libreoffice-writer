/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: colex.hxx,v $
 * $Revision: 1.9 $
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
#ifndef _COLEX_HXX
#define _COLEX_HXX

#include <svx/pagectrl.hxx>
#include <svx/paperinf.hxx>
#include "swdllapi.h"
#include <fmtclds.hxx>
#include "frmatr.hxx"

class SwColMgr;
class SfxItemSet;

/* -----------------------------08.02.2002 11:32------------------------------

 ---------------------------------------------------------------------------*/
class SW_DLLPUBLIC SwPageExample : public SvxPageWindow
{
public:
    SwPageExample(Window* pPar, const ResId& rResId ) :
                                SvxPageWindow(pPar, rResId )
                                {SetSize(SvxPaperInfo::GetPaperSize(PAPER_A4));/*DIN A4*/}

    void UpdateExample( const SfxItemSet& rSet );
};
/* -----------------------------08.02.2002 11:34------------------------------

 ---------------------------------------------------------------------------*/
class SwTextGridItem;
class SW_DLLPUBLIC SwPageGridExample : public SwPageExample
{
    SwTextGridItem*     pGridItem;
    sal_Bool            m_bVertical;
protected:
    virtual void DrawPage( const Point& rPoint,
                           const BOOL bSecond,
                           const BOOL bEnabled );
public:
    SwPageGridExample(Window* pPar, const ResId& rResId ) :
                                SwPageExample(pPar, rResId ),
                                pGridItem(0),
                                m_bVertical(sal_False){}
    ~SwPageGridExample();
    void UpdateExample( const SfxItemSet& rSet );
};
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC SwColExample : public SwPageExample
{
    SwColMgr* 	pColMgr;
    
    using SwPageExample::UpdateExample;

protected:
    virtual void DrawPage( const Point& rPoint,
                           const BOOL bSecond,
                           const BOOL bEnabled );

public:
        SwColExample(Window* pPar, const ResId& rResId ) :
                                SwPageExample(pPar, rResId ),
                                pColMgr(0){}

    void UpdateExample( const SfxItemSet& rSet, SwColMgr* pMgr  )
        {   pColMgr = pMgr;
            SwPageExample::UpdateExample(rSet);
        }
};

/*-----------------25.10.96 08.23-------------------

--------------------------------------------------*/
class SW_DLLPUBLIC SwColumnOnlyExample : public Window
{
private:
    Size        m_aWinSize;

    Size        m_aFrmSize;
    SwFmtCol    m_aCols;

protected:
    virtual void Paint( const Rectangle& rRect );

public:
    SwColumnOnlyExample( Window* , const ResId& );

    void        SetColumns(const SwFmtCol& rCol);

};

#endif // _COLEX_HXX
