/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "swundo.hxx"			// fuer die UndoIds
#include "pam.hxx"
#include "ndtxt.hxx"

#include "undobj.hxx"


inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }

SwUndoOutlineLeftRight::SwUndoOutlineLeftRight( const SwPaM& rPam,
                                                short nOff )
    : SwUndo( UNDO_OUTLINE_LR ), SwUndRng( rPam ), nOffset( nOff )
{
}


void SwUndoOutlineLeftRight::Undo( SwUndoIter& rUndoIter )
{
    SetPaM( rUndoIter );
    rUndoIter.GetDoc().OutlineUpDown( *rUndoIter.pAktPam, -nOffset );
}


void SwUndoOutlineLeftRight::Redo( SwUndoIter& rUndoIter )
{
    SetPaM( rUndoIter );
    rUndoIter.GetDoc().OutlineUpDown( *rUndoIter.pAktPam, nOffset );
}


void SwUndoOutlineLeftRight::Repeat( SwUndoIter& rUndoIter )
{
    rUndoIter.GetDoc().OutlineUpDown( *rUndoIter.pAktPam, nOffset );
}



