/*************************************************************************
 *
 *  $RCSfile: fmtwrapinfluenceonobjpos.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-08-02 13:57:08 $
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
#ifndef _FMTWRAPINFLUENCEONOBJPOS_HXX
#define _FMTWRAPINFLUENCEONOBJPOS_HXX

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _FORMAT_HXX
#include <format.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX
#include <svtools/poolitem.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_WRAPINFLUENCEONPOSITION_HPP_
#include <com/sun/star/text/WrapInfluenceOnPosition.hpp>
#endif
using namespace com::sun::star;

class SwFmtWrapInfluenceOnObjPos: public SfxPoolItem
{
private:
    sal_Int16 mnWrapInfluenceOnPosition;

public:
    TYPEINFO();

    SwFmtWrapInfluenceOnObjPos(
            sal_Int16 _nWrapInfluenceOnPosition =
                text::WrapInfluenceOnPosition::NONE_CONCURRENT_POSITIONED );
    SwFmtWrapInfluenceOnObjPos(
            const SwFmtWrapInfluenceOnObjPos& _rCpy );
    ~SwFmtWrapInfluenceOnObjPos();

    SwFmtWrapInfluenceOnObjPos& operator=(
            const SwFmtWrapInfluenceOnObjPos& _rSource );

    // pure virtual methods of class <SfxPoolItem>
    virtual int operator==( const SfxPoolItem& _rAttr ) const;
    virtual SfxPoolItem* Clone( SfxItemPool* pPool = 0 ) const;

    virtual BOOL QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual BOOL PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    // direct accessors to data
    void SetWrapInfluenceOnObjPos( sal_Int16 _nWrapInfluenceOnPosition );
    sal_Int16 GetWrapInfluenceOnObjPos() const;
};

inline const SwFmtWrapInfluenceOnObjPos& SwAttrSet::GetWrapInfluenceOnObjPos(BOOL bInP) const
    { return (const SwFmtWrapInfluenceOnObjPos&)Get( RES_WRAP_INFLUENCE_ON_OBJPOS,bInP); }

 inline const SwFmtWrapInfluenceOnObjPos& SwFmt::GetWrapInfluenceOnObjPos(BOOL bInP) const
    { return aSet.GetWrapInfluenceOnObjPos(bInP); }

#endif
