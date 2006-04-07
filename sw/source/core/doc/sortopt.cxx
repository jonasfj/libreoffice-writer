/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sortopt.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 15:09:06 $
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


#pragma hdrstop

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif

#ifndef _SORTOPT_HXX
#include <sortopt.hxx>
#endif


SV_IMPL_PTRARR(SwSortKeys, SwSortKey*)

/*--------------------------------------------------------------------
    Beschreibung:	Sortier-Schluessel
 --------------------------------------------------------------------*/

SwSortKey::SwSortKey() :
    eSortOrder( SRT_ASCENDING ),
    nColumnId( 0 ),
    bIsNumeric( TRUE )
{
}

SwSortKey::SwSortKey(USHORT nId, const String& rSrtType, SwSortOrder eOrder) :
    eSortOrder( eOrder ),
    sSortType( rSrtType ),
    nColumnId( nId ),
    bIsNumeric( 0 == rSrtType.Len() )
{
}


SwSortKey::SwSortKey(const SwSortKey& rOld) :
    eSortOrder( rOld.eSortOrder ),
    sSortType( rOld.sSortType ),
    nColumnId( rOld.nColumnId ),
    bIsNumeric( rOld.bIsNumeric )
{
}

/*--------------------------------------------------------------------
    Beschreibung: Sortieroptionen fuers Sortieren
 --------------------------------------------------------------------*/


SwSortOptions::SwSortOptions()
    : eDirection( SRT_ROWS ),
    nLanguage( LANGUAGE_SYSTEM ),
    cDeli( 9 ),
    bTable( FALSE ),
    bIgnoreCase( FALSE )
{
}


SwSortOptions::SwSortOptions(const SwSortOptions& rOpt) :
    eDirection( rOpt.eDirection ),
    cDeli( rOpt.cDeli ),
    nLanguage( rOpt.nLanguage ),
    bTable( rOpt.bTable ),
    bIgnoreCase( rOpt.bIgnoreCase )
{
    for( USHORT i=0; i < rOpt.aKeys.Count(); ++i )
    {
        SwSortKey* pNew = new SwSortKey(*rOpt.aKeys[i]);
        aKeys.C40_INSERT( SwSortKey, pNew, aKeys.Count());
    }
}


SwSortOptions::~SwSortOptions()
{
    aKeys.DeleteAndDestroy(0, aKeys.Count());
}



