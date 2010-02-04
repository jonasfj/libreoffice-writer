/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: flypos.cxx,v $
 * $Revision: 1.6 $
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



#include "doc.hxx"
#include "node.hxx"
#include <docary.hxx>


#include <fmtanchr.hxx>
#include "flypos.hxx"
#include "frmfmt.hxx"
#include "dcontact.hxx"
#include "dview.hxx"
#include "flyfrm.hxx"
#include "dflyobj.hxx"
#include "ndindex.hxx"



SV_IMPL_OP_PTRARR_SORT( SwPosFlyFrms, SwPosFlyFrmPtr )

SwPosFlyFrm::SwPosFlyFrm( const SwNodeIndex& rIdx, const SwFrmFmt* pFmt,
                            USHORT nArrPos )
    : pFrmFmt( pFmt ), pNdIdx( (SwNodeIndex*) &rIdx )
{
    BOOL bFnd = FALSE;
    const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
    if (FLY_AT_PAGE == rAnchor.GetAnchorId())
    {
        pNdIdx = new SwNodeIndex( rIdx );
    }
    else if( pFmt->GetDoc()->GetRootFrm() )
    {
        SwClientIter aIter( (SwFmt&)*pFmt );
        if( RES_FLYFRMFMT == pFmt->Which() )
        {
            // Schauen, ob es ein SdrObject dafuer gibt
            if( aIter.First( TYPE( SwFlyFrm) ) )
                nOrdNum = ((SwFlyFrm*)aIter())->GetVirtDrawObj()->GetOrdNum(),
                bFnd = TRUE;
        }
        else if( RES_DRAWFRMFMT == pFmt->Which() )
        {
            // Schauen, ob es ein SdrObject dafuer gibt
            if( aIter.First( TYPE(SwDrawContact) ) )
                nOrdNum = ((SwDrawContact*)aIter())->GetMaster()->GetOrdNum(),
                bFnd = TRUE;
        }
    }

    if( !bFnd )
    {
        nOrdNum = pFmt->GetDoc()->GetSpzFrmFmts()->Count();
        nOrdNum += nArrPos;
    }
}

SwPosFlyFrm::~SwPosFlyFrm()
{
    const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
    if (FLY_AT_PAGE == rAnchor.GetAnchorId())
    {
        delete pNdIdx;
    }
}

BOOL SwPosFlyFrm::operator==( const SwPosFlyFrm& )
{
    return FALSE;	// FlyFrames koennen auf der gleichen Position stehen
}

BOOL SwPosFlyFrm::operator<( const SwPosFlyFrm& rPosFly )
{
    if( pNdIdx->GetIndex() == rPosFly.pNdIdx->GetIndex() )
    {
        // dann entscheidet die Ordnungsnummer!
        return nOrdNum < rPosFly.nOrdNum;
    }
    return pNdIdx->GetIndex() < rPosFly.pNdIdx->GetIndex();
}



