/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: action.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-25 12:11:28 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_starmath.hxx"


#include "action.hxx"
#include "smdll.hxx"
#include "document.hxx"
#include "starmath.hrc"


SmFormatAction::SmFormatAction(SmDocShell *pDocSh,
                               const SmFormat& rOldFormat,
                               const SmFormat& rNewFormat) :
    pDoc( pDocSh ),
    aOldFormat( rOldFormat ),
    aNewFormat( rNewFormat )
{
}

void SmFormatAction::Undo()
{
    pDoc->SetFormat(aOldFormat);
}

void SmFormatAction::Redo()
{
    pDoc->SetFormat(aNewFormat);
}

void SmFormatAction::Repeat(SfxRepeatTarget& rDocSh)
{
    dynamic_cast< SmDocShell & >(rDocSh).SetFormat(aNewFormat);
}

UniString SmFormatAction::GetComment() const
{
    return (SmResId(RID_UNDOFORMATNAME));
}


