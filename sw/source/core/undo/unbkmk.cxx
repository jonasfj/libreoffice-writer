/*************************************************************************
 *
 *  $RCSfile: unbkmk.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2004-05-18 14:07:02 $
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


#pragma hdrstop

#include "doc.hxx"
#include "docary.hxx"
#include "swundo.hxx"			// fuer die UndoIds
#include "pam.hxx"

#include "undobj.hxx"
#include "bookmrk.hxx"
#include "rolbck.hxx"

#include "SwRewriter.hxx"

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }


SwUndoBookmark::SwUndoBookmark( USHORT nUndoId, const SwBookmark& rBkmk )
    : SwUndo( nUndoId )
{
    int nType = SwHstryBookmark::BKMK_POS;
    if( rBkmk.GetOtherPos() )
        nType |= SwHstryBookmark::BKMK_OTHERPOS;
    pHBookmark = new SwHstryBookmark( rBkmk, nType );
}



SwUndoBookmark::~SwUndoBookmark()
{
    delete pHBookmark;
}


void SwUndoBookmark::SetInDoc( SwDoc* pDoc )
{
    pHBookmark->SetInDoc( pDoc, FALSE );
}


void SwUndoBookmark::ResetInDoc( SwDoc* pDoc )
{
    const SwBookmarks& rBkmkTbl = pDoc->GetBookmarks();
    for( USHORT n = 0; n < rBkmkTbl.Count(); ++n )
        if( pHBookmark->IsEqualBookmark( *rBkmkTbl[ n ] ) )
        {
                pDoc->DelBookmark( n );
                break;
        }
}

SwRewriter SwUndoBookmark::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule(UNDO_ARG1, pHBookmark->GetName());

    return aResult;
}

SwUndoDelBookmark::SwUndoDelBookmark( const SwBookmark& rBkmk )
    : SwUndoBookmark( UNDO_DELBOOKMARK, rBkmk )
{
}


void SwUndoDelBookmark::Undo( SwUndoIter& rUndoIter )
{
    SetInDoc( &rUndoIter.GetDoc() );
}


void SwUndoDelBookmark::Redo( SwUndoIter& rUndoIter )
{
    ResetInDoc( &rUndoIter.GetDoc() );
}


SwUndoInsBookmark::SwUndoInsBookmark( const SwBookmark& rBkmk )
    : SwUndoBookmark( UNDO_INSBOOKMARK, rBkmk )
{
}


void SwUndoInsBookmark::Undo( SwUndoIter& rUndoIter )
{
    ResetInDoc( &rUndoIter.GetDoc() );
}


void SwUndoInsBookmark::Redo( SwUndoIter& rUndoIter )
{
    SetInDoc( &rUndoIter.GetDoc() );
}


