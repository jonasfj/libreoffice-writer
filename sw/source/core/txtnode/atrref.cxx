/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: atrref.cxx,v $
 * $Revision: 1.8 $
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


#include <hintids.hxx>
#include <txtrfmrk.hxx>
#include <fmtrfmrk.hxx>
#include <swfont.hxx>


/****************************************************************************
 *
 *  class SwFmtRefMark
 *
 ****************************************************************************/

SwFmtRefMark::~SwFmtRefMark( )
{
}

SwFmtRefMark::SwFmtRefMark( const XubString& rName )
    : SfxPoolItem( RES_TXTATR_REFMARK ),
    pTxtAttr( 0 ),
    aRefName( rName )
{
}

SwFmtRefMark::SwFmtRefMark( const SwFmtRefMark& rAttr )
    : SfxPoolItem( RES_TXTATR_REFMARK ),
    pTxtAttr( 0 ),
    aRefName( rAttr.aRefName )
{
}

int SwFmtRefMark::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    return aRefName == ((SwFmtRefMark&)rAttr).aRefName;
}

SfxPoolItem* SwFmtRefMark::Clone( SfxItemPool* ) const
{
    return new SwFmtRefMark( *this );
}

/*************************************************************************
 *						class SwTxtRefMark
 *************************************************************************/

// Attribut fuer Inhalts-/Positions-Referenzen im Text

SwTxtRefMark::SwTxtRefMark( SwFmtRefMark& rAttr,
                    xub_StrLen nStartPos, xub_StrLen* pEnd )
    : SwTxtAttrEnd( rAttr, nStartPos, nStartPos )
    , m_pTxtNode( 0 )
    , m_pEnd( 0 )
{
    rAttr.pTxtAttr = this;
    if ( pEnd )
    {
        m_nEnd = *pEnd;
        m_pEnd = & m_nEnd;
    }
    else
    {
        SetHasDummyChar(true);
    }
    SetDontMoveAttr( true );
    SetOverlapAllowedAttr( true );
}

xub_StrLen* SwTxtRefMark::GetEnd()
{
    return m_pEnd;
}

