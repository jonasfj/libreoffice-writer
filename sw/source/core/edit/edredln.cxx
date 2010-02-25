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


#include <docary.hxx>
#include <vcl/window.hxx>
#include "redline.hxx"
#include "doc.hxx"
#include "swundo.hxx"
#include "editsh.hxx"
#include "edimp.hxx"
#include "frmtool.hxx"


USHORT SwEditShell::GetRedlineMode() const
{
    return GetDoc()->GetRedlineMode();
}

void SwEditShell::SetRedlineMode( USHORT eMode )
{
    if( eMode != GetDoc()->GetRedlineMode() )
    {
        SET_CURR_SHELL( this );
        StartAllAction();
        GetDoc()->SetRedlineMode( (RedlineMode_t)eMode );
        EndAllAction();
    }
}

BOOL SwEditShell::IsRedlineOn() const
{
    return GetDoc()->IsRedlineOn();
}

USHORT SwEditShell::GetRedlineCount() const
{
    return GetDoc()->GetRedlineTbl().Count();
}

const SwRedline& SwEditShell::GetRedline( USHORT nPos ) const
{
    return *GetDoc()->GetRedlineTbl()[ nPos ];
}

void lcl_InvalidateAll( ViewShell* pSh )
{
    ViewShell *pStop = pSh;
    do
    {
        if ( pSh->GetWin() )
            pSh->GetWin()->Invalidate();
        pSh = (ViewShell*)pSh->GetNext();

    } while ( pSh != pStop );
}

BOOL SwEditShell::AcceptRedline( USHORT nPos )
{
    SET_CURR_SHELL( this );
    StartAllAction();
    BOOL bRet = GetDoc()->AcceptRedline( nPos, true );
    if( !nPos && !::IsExtraData( GetDoc() ) )
        lcl_InvalidateAll( this );
    EndAllAction();
    return bRet;
}

BOOL SwEditShell::RejectRedline( USHORT nPos )
{
    SET_CURR_SHELL( this );
    StartAllAction();
    BOOL bRet = GetDoc()->RejectRedline( nPos, true );
    if( !nPos && !::IsExtraData( GetDoc() ) )
        lcl_InvalidateAll( this );
    EndAllAction();
    return bRet;
}

// Kommentar am Redline setzen
BOOL SwEditShell::SetRedlineComment( const String& rS )
{
    BOOL bRet = FALSE;
    FOREACHPAM_START(this)
        bRet = bRet || GetDoc()->SetRedlineComment( *PCURCRSR, rS );
    FOREACHPAM_END()

    return bRet;
}

const SwRedline* SwEditShell::GetCurrRedline() const
{
    return GetDoc()->GetRedline( *GetCrsr()->GetPoint(), 0 );
}

void SwEditShell::UpdateRedlineAttr()
{
    if( ( nsRedlineMode_t::REDLINE_SHOW_INSERT | nsRedlineMode_t::REDLINE_SHOW_DELETE ) ==
        ( nsRedlineMode_t::REDLINE_SHOW_MASK & GetDoc()->GetRedlineMode() ))
    {
        SET_CURR_SHELL( this );
        StartAllAction();

        GetDoc()->UpdateRedlineAttr();

        EndAllAction();
    }
}

    // suche das Redline zu diesem Data und returne die Pos im Array
    // USHRT_MAX wird returnt, falls nicht vorhanden
USHORT SwEditShell::FindRedlineOfData( const SwRedlineData& rData ) const
{
    const SwRedlineTbl& rTbl = GetDoc()->GetRedlineTbl();

    for( USHORT i = 0, nCnt = rTbl.Count(); i < nCnt; ++i )
        if( &rTbl[ i ]->GetRedlineData() == &rData )
            return i;
    return USHRT_MAX;
}



