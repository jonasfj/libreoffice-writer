 /*************************************************************************
 *
 *  $RCSfile: accfrmobjmap.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mib $ $Date: 2002-08-23 11:34:07 $
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

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif

#ifndef _ACCFFRMOBJMAP_HXX
#include <accfrmobjmap.hxx>
#endif

::std::pair< SwFrmOrObjMap::iterator, bool > SwFrmOrObjMap::insert( 
        sal_uInt32 nPos, const SwFrmOrObj& rLower )
{
    SwFrmOrObjMapKey aKey( SwFrmOrObjMapKey::TEXT, nPos );
    value_type aEntry( aKey, rLower );
    return _SwFrmOrObjMap::insert( aEntry );
}

::std::pair< SwFrmOrObjMap::iterator, bool > SwFrmOrObjMap::insert( 
        const SdrObject *pObj, const SwFrmOrObj& rLower, const SwDoc *pDoc )
{
    if( !bLayerIdsValid )
    {
        nHellId = pDoc->GetHellId();
        nControlsId = pDoc->GetControlsId();
        bLayerIdsValid = sal_True;
    }

    SdrLayerID nLayer = pObj->GetLayer();
    SwFrmOrObjMapKey::LayerId eLayerId = (nHellId == nLayer)
                    ? SwFrmOrObjMapKey::HELL	
                    : ((nControlsId == nLayer) ? SwFrmOrObjMapKey::CONTROLS
                                               : SwFrmOrObjMapKey::HEAVEN);
    SwFrmOrObjMapKey aKey( eLayerId, pObj->GetOrdNum() );
    value_type aEntry( aKey, rLower );
    return _SwFrmOrObjMap::insert( aEntry );
}

SwFrmOrObjMap::SwFrmOrObjMap(
        const SwRect& rVisArea, const SwFrm *pFrm ) :
    bLayerIdsValid( sal_False )
{
    SwFrmOrObj aFrm( pFrm );
    sal_Bool bVisibleOnly = aFrm.IsVisibleChildrenOnly();

    sal_uInt32 nPos = 0;
    SwFrmOrObj aLower( pFrm->GetLower() );
    while( aLower.GetSwFrm() )
    {
        if( !bVisibleOnly || aLower.GetBox().IsOver( rVisArea ) )
            insert( nPos++, aLower );

        aLower = aLower.GetSwFrm()->GetNext();
    }

    if( pFrm->IsPageFrm() )
    {
        ASSERT( bVisibleOnly, "page frame within tab frame???" );
        const SwPageFrm *pPgFrm = 
            static_cast< const SwPageFrm * >( pFrm );
        const SwSortDrawObjs *pObjs = pPgFrm->GetSortedObjs();
        if( pObjs )
        {
            const SwDoc *pDoc = pPgFrm->GetFmt()->GetDoc();
            for( sal_uInt16 i=0; i<pObjs->Count(); i++ )
            {
                aLower = (*pObjs)[i];
                if( aLower.GetBox().IsOver( rVisArea ) )
                    insert( aLower.GetSdrObject(), aLower, pDoc );
            }
        }
    }
    else if( pFrm->IsTxtFrm() )
    {
        const SwDoc *pDoc = static_cast< const SwTxtFrm * >( pFrm )->GetNode()
                                                                   ->GetDoc();
        const SwDrawObjs *pObjs = pFrm->GetDrawObjs();
        if( pObjs )
        {
            for( sal_uInt16 i=0; i<pObjs->Count(); i++ )
            {
                aLower = (*pObjs)[i];
                if( aLower.IsBoundAsChar() &&
                    (!bVisibleOnly || aLower.GetBox().IsOver( rVisArea )) )
                    insert( aLower.GetSdrObject(), aLower, pDoc );
            }
        }
    }
}
