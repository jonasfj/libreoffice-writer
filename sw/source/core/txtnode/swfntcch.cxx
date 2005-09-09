/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swfntcch.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:13:46 $
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

#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#include "swfntcch.hxx"
#include "fmtcol.hxx"
#include "swfont.hxx"

// aus atrstck.cxx
extern const BYTE StackPos[];

// globale Variablen, werden in SwFntCch.Hxx bekanntgegeben
// Der FontCache wird in TxtInit.Cxx _TXTINIT erzeugt und in _TXTEXIT geloescht
SwFontCache *pSwFontCache = NULL;

/*************************************************************************
|*
|*	SwFontObj::SwFontObj(), ~SwFontObj()
|*
|*	Ersterstellung		AMA 25. Jun. 95
|*	Letzte Aenderung	AMA 25. Jun. 95
|*
|*************************************************************************/

SwFontObj::SwFontObj( const void *pOwner, ViewShell *pSh ) :
    SwCacheObj( (void*)pOwner ),
    aSwFont( &((SwTxtFmtColl *)pOwner)->GetAttrSet(), pSh ? pSh->GetDoc() : 0 )
{
    aSwFont.GoMagic( pSh, aSwFont.GetActual() );
    const SwAttrSet& rAttrSet = ((SwTxtFmtColl *)pOwner)->GetAttrSet();
    for (USHORT i = RES_CHRATR_BEGIN; i < RES_CHRATR_END; i++)
        pDefaultArray[ StackPos[ i ] ] = &rAttrSet.Get( i, TRUE );
}

SwFontObj::~SwFontObj()
{
}

/*************************************************************************
|*
|*	SwFontAccess::SwFontAccess()
|*
|*	Ersterstellung		AMA 25. Jun. 95
|*	Letzte Aenderung	AMA 25. Jun. 95
|*
|*************************************************************************/

SwFontAccess::SwFontAccess( const void *pOwner, ViewShell *pSh ) :
    SwCacheAccess( *pSwFontCache, pOwner,
            (BOOL) ((SwTxtFmtColl*)pOwner)->IsInSwFntCache() ),
    pShell( pSh )
{
}

SwFontObj *SwFontAccess::Get( )
{
    return (SwFontObj *) SwCacheAccess::Get( );
}

SwCacheObj *SwFontAccess::NewObj( )
{
    ((SwTxtFmtColl*)pOwner)->SetInSwFntCache( TRUE );
    return new SwFontObj( pOwner, pShell );
}


