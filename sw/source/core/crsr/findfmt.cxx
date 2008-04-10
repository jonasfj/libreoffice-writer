/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: findfmt.cxx,v $
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


#include <doc.hxx>
#include <pamtyp.hxx>


BOOL SwPaM::Find( const SwFmt& rFmt, SwMoveFn fnMove,
                        const SwPaM *pRegion, BOOL bInReadOnly  )
{
    BOOL bFound = FALSE;
    BOOL bSrchForward = fnMove == fnMoveForward;
    SwPaM* pPam = MakeRegion( fnMove, pRegion );

    // Wenn am Anfang/Ende, aus dem Node moven
    if( bSrchForward
        ? pPam->GetPoint()->nContent.GetIndex() == pPam->GetCntntNode()->Len()
        : !pPam->GetPoint()->nContent.GetIndex() )
    {
        if( !(*fnMove->fnNds)( &pPam->GetPoint()->nNode, FALSE ))
        {
            delete pPam;
            return FALSE;
        }
        SwCntntNode *pNd = pPam->GetPoint()->nNode.GetNode().GetCntntNode();
        xub_StrLen nTmpPos = bSrchForward ? 0 : pNd->Len();
        pPam->GetPoint()->nContent.Assign( pNd, nTmpPos );
    }

    BOOL bFirst = TRUE;
    SwCntntNode* pNode;
    while( !bFound &&
            0 != ( pNode = ::GetNode( *pPam, bFirst, fnMove, bInReadOnly )))
    {
        if( 0 != ( bFound = pNode->GetFmtColl() == &rFmt ))
        {
            // wurde die FormatCollection gefunden, dann handelt es sich auf
            // jedenfall um einen SwCntntNode !!

            // FORWARD:  SPoint an das Ende, GetMark zum Anfanf vom Node
            // BACKWARD: SPoint zum Anfang,	GetMark an das Ende vom Node
            // und immer nach der Logik: inkl. Start, exkl. End !!!
            *GetPoint() = *pPam->GetPoint();
            SetMark();
            pNode->MakeEndIndex( &GetPoint()->nContent );
            GetMark()->nContent = 0;
            if( !bSrchForward )			// rueckwaerts Suche?
                Exchange(); 			// SPoint und GetMark tauschen
            break;
        }
    }
    delete pPam;
    return bFound;
}


