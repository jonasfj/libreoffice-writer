/*************************************************************************
 *
 *  $RCSfile: colex.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: os $ $Date: 2002-02-11 12:30:58 $
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
#ifndef _COLEX_HXX
#define _COLEX_HXX

#ifndef _SVX_PAGECTRL_HXX //autogen
#include <svx/pagectrl.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif

#include "frmatr.hxx"

class SwColMgr;
class SfxItemSet;

/* -----------------------------08.02.2002 11:32------------------------------

 ---------------------------------------------------------------------------*/
class SwPageExample : public SvxPageWindow
{
public:
    SwPageExample(Window* pPar, const ResId& rResId ) :
                                SvxPageWindow(pPar, rResId )
                                {SetSize(Size(11907, 16433));/*DIN A4*/}

    void UpdateExample( const SfxItemSet& rSet );
};
/* -----------------------------08.02.2002 11:34------------------------------

 ---------------------------------------------------------------------------*/
class SwTextGridItem;
class SwPageGridExample : public SwPageExample
{
    SwTextGridItem*     pGridItem;
protected:
    virtual void DrawPage( const Point& rPoint,
                           const BOOL bSecond,
                           const BOOL bEnabled );
public:
    SwPageGridExample(Window* pPar, const ResId& rResId ) :
                                SwPageExample(pPar, rResId ),
                                pGridItem(0){}
    ~SwPageGridExample();
    void UpdateExample( const SfxItemSet& rSet );
};
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class SwColExample : public SwPageExample
{
    SwColMgr* 	pColMgr;
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
class SwColumnOnlyExample : public Window
{
private:
    Size 		aWinSize;

    Size		aFrmSize;
    SwFmtCol   	aCols;

protected:
    virtual void Paint( const Rectangle& rRect );

public:
    SwColumnOnlyExample( Window* , const ResId& );

    void        SetColumns(const SwFmtCol& rCol);

};

#endif // _COLEX_HXX
