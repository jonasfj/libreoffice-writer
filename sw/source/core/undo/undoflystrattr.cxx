/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SwUndoPageDesc.cxx,v $
 * $Revision: 1.12 $
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

#include <undoflystrattr.hxx>
#include <frmfmt.hxx>

SwUndoFlyStrAttr::SwUndoFlyStrAttr( SwFlyFrmFmt& rFlyFrmFmt,
                                    const SwUndoId eUndoId,
                                    const String& sOldStr,
                                    const String& sNewStr )
    : SwUndo( eUndoId ),
      mrFlyFrmFmt( rFlyFrmFmt ),
      msOldStr( sOldStr ),
      msNewStr( sNewStr )
{
    ASSERT( eUndoId == UNDO_FLYFRMFMT_TITLE ||
            eUndoId == UNDO_FLYFRMFMT_DESCRIPTION,
            "<SwUndoFlyStrAttr::SwUndoFlyStrAttr(..)> - unexpected undo id --> Undo will not work" );
}

SwUndoFlyStrAttr::~SwUndoFlyStrAttr()
{
}

void SwUndoFlyStrAttr::Undo( SwUndoIter& )
{
    switch ( GetId() )
    {
        case UNDO_FLYFRMFMT_TITLE:
        {
            mrFlyFrmFmt.SetObjTitle( msOldStr, true );
        }
        break;
        case UNDO_FLYFRMFMT_DESCRIPTION:
        {
            mrFlyFrmFmt.SetObjDescription( msOldStr, true );
        }
        break;
        default:
        {
        }
    }
}

void SwUndoFlyStrAttr::Redo( SwUndoIter& )
{
    switch ( GetId() )
    {
        case UNDO_FLYFRMFMT_TITLE:
        {
            mrFlyFrmFmt.SetObjTitle( msNewStr, true );
        }
        break;
        case UNDO_FLYFRMFMT_DESCRIPTION:
        {
            mrFlyFrmFmt.SetObjDescription( msNewStr, true );
        }
        break;
        default:
        {
        }
    }
}

void SwUndoFlyStrAttr::Repeat( SwUndoIter& )
{
    // no repeat
}

SwRewriter SwUndoFlyStrAttr::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule( UNDO_ARG1, mrFlyFrmFmt.GetName() );

    return aResult;
}
