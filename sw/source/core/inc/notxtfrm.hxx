/*************************************************************************
 *
 *  $RCSfile: notxtfrm.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-05-17 16:15:56 $
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
#ifndef _NOTXTFRM_HXX
#define _NOTXTFRM_HXX

#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif

class SwNoTxtNode;
class OutputDevice;
class SwBorderAttrs;
class Bitmap;
struct SwCrsrMoveState;

class SwNoTxtFrm: public SwCntntFrm
{
    friend void _FrmFinit();	//erase pErrorBmp and pReplaceBmp
    static Bitmap *pErrorBmp, *pReplaceBmp;

    short	 nWeight;					// "Wichtigkeit" der Grafik

    const Size& GetSize() const;

    void InitCtor();

    void Format ( const SwBorderAttrs *pAttrs = 0 );
    void PaintCntnt  ( OutputDevice*, const SwRect&, const SwRect& ) const;
    /// OD 25.09.2002 #99739# - delete unused 3rd parameter
    void PaintPicture( OutputDevice*, const SwRect& ) const;
protected:
    virtual void MakeAll();
public:
    SwNoTxtFrm( SwNoTxtNode * const );
    ~SwNoTxtFrm();

    virtual void Modify( SfxPoolItem*, SfxPoolItem* );
    virtual void Paint(const SwRect& ) const;
    virtual BOOL GetCharRect( SwRect &, const SwPosition&,
                              SwCrsrMoveState* = 0) const;
    BOOL GetCrsrOfst(SwPosition* pPos, Point& aPoint,
                     SwCrsrMoveState* = 0) const;

    const Size &GetGrfSize() const	{ return GetSize(); }
    void GetGrfArea( SwRect &rRect, SwRect * = 0, BOOL bMirror = TRUE ) const;

    BOOL IsTransparent() const;

    void StopAnimation( OutputDevice* = 0 ) const;
    BOOL HasAnimation()  const;

    // Routinen fuer den Grafik-Cache
    USHORT GetWeight() { return nWeight; }

    static const Bitmap& GetBitmap( BOOL bError );
};


#endif
