/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: crstrvl1.cxx,v $
 * $Revision: 1.9 $
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


#include <crsrsh.hxx>
#include <viscrs.hxx>
#include <callnk.hxx>

BOOL SwCrsrShell::IsStartWord() const
{
    return pCurCrsr->IsStartWord();
}
BOOL SwCrsrShell::IsEndWord() const
{
    return pCurCrsr->IsEndWord();
}
BOOL SwCrsrShell::IsStartSentence() const
{
    return pCurCrsr->IsStartEndSentence( false );
}
BOOL SwCrsrShell::IsEndSentence() const
{
    return pCurCrsr->IsStartEndSentence( true );
}
BOOL SwCrsrShell::IsInWord() const
{
    return pCurCrsr->IsInWord();
}


BOOL SwCrsrShell::GoStartWord()
{
    return CallCrsrFN( &SwCursor::GoStartWord );
}
BOOL SwCrsrShell::GoEndWord()
{
    return CallCrsrFN( &SwCursor::GoEndWord );
}
BOOL SwCrsrShell::GoNextWord()
{
    return CallCrsrFN( &SwCursor::GoNextWord );
}
BOOL SwCrsrShell::GoPrevWord()
{
    return CallCrsrFN( &SwCursor::GoPrevWord );
}
BOOL SwCrsrShell::GoNextSentence()
{
    return CallCrsrFN( &SwCursor::GoNextSentence );
}
BOOL SwCrsrShell::GoEndSentence()
{
    return CallCrsrFN( &SwCursor::GoEndSentence );
}

BOOL SwCrsrShell::GoPrevSentence()
{
    return CallCrsrFN( &SwCursor::GoPrevSentence );
}
BOOL SwCrsrShell::GoStartSentence()
{
    return CallCrsrFN( &SwCursor::GoStartSentence );
}

BOOL SwCrsrShell::SelectWord( const Point* pPt )
{
    return pCurCrsr->SelectWord( pPt );
}


