/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: accfrmobjmap.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:49:33 $
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
#ifndef _ACCFRMOBJMAP_HXX
#define _ACCFRMOBJMAP_HXX

#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _ACCFRMOBJ_HXX
#include <accfrmobj.hxx>
#endif

#include <map>

class SwDoc;

class SwFrmOrObjMapKey
{
public:
    enum LayerId { INVALID, HELL, TEXT, HEAVEN, CONTROLS };
private:

    LayerId eLayerId;
    sal_uInt32 nOrdNum;

public:

    inline SwFrmOrObjMapKey();
    inline SwFrmOrObjMapKey( LayerId eId, sal_uInt32 nOrd );

    inline sal_Bool operator()( const SwFrmOrObjMapKey& r1,
                                 const SwFrmOrObjMapKey& r2 ) const;
};

typedef ::std::map < SwFrmOrObjMapKey, SwFrmOrObj, SwFrmOrObjMapKey >
    _SwFrmOrObjMap;

class SwFrmOrObjMap : public _SwFrmOrObjMap 
{
    SdrLayerID nHellId;
    SdrLayerID nControlsId;
    sal_Bool bLayerIdsValid;

    ::std::pair< iterator, bool > insert( sal_uInt32 nPos, 
                                          const SwFrmOrObj& rLower );	
    ::std::pair< iterator, bool > insert( const SdrObject *pObj,
                                              const SwFrmOrObj& rLower,
                                             const SwDoc *pDoc	);	

public:

    SwFrmOrObjMap( const SwRect& rVisArea, const SwFrm *pFrm );

    inline static sal_Bool IsSortingRequired( const SwFrm *pFrm );
};

inline SwFrmOrObjMapKey::SwFrmOrObjMapKey() :
    eLayerId( INVALID ),
    nOrdNum( 0 )
{
}

inline SwFrmOrObjMapKey::SwFrmOrObjMapKey(
        LayerId eId, sal_uInt32 nOrd ) :
    eLayerId( eId ),
    nOrdNum( nOrd )
{
}

inline sal_Bool SwFrmOrObjMapKey::operator()(
        const SwFrmOrObjMapKey& r1,
        const SwFrmOrObjMapKey& r2 ) const
{
    return (r1.eLayerId == r2.eLayerId) ? (r1.nOrdNum < r2.nOrdNum) : 
           (r1.eLayerId < r2.eLayerId);
}

inline sal_Bool SwFrmOrObjMap::IsSortingRequired( const SwFrm *pFrm )
{
    return ( pFrm->IsPageFrm() &&
             static_cast< const SwPageFrm * >( pFrm )->GetSortedObjs() ) ||
            (pFrm->IsTxtFrm() && pFrm->GetDrawObjs() );
}

#endif

