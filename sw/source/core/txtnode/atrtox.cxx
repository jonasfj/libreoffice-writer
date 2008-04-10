/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: atrtox.cxx,v $
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


#include <doc.hxx>
#include <txttxmrk.hxx>
#include <swfont.hxx>
#include <tox.hxx>
#include <ndtxt.hxx>

SwTxtTOXMark::SwTxtTOXMark( const SwTOXMark& rAttr,
                    xub_StrLen nStartPos, xub_StrLen* pEnde )
    : SwTxtAttrEnd( rAttr, nStartPos, nStartPos ),
    pEnd( 0 ),
    pMyTxtNd( 0 )
{
    ((SwTOXMark&)rAttr).pTxtAttr = this;
    if( !rAttr.GetAlternativeText().Len() )
    {
        nEnd = *pEnde;
        pEnd = &nEnd;
    }
    SetDontMergeAttr( TRUE );
    SetDontMoveAttr( TRUE );
    SetOverlapAllowedAttr( TRUE );
}

SwTxtTOXMark::~SwTxtTOXMark()
{
}

xub_StrLen* SwTxtTOXMark::GetEnd()
{
    return pEnd;
}

void SwTxtTOXMark::CopyTOXMark( SwDoc* pDoc )
{
    SwTOXMark& rTOX = (SwTOXMark&)GetTOXMark();
    TOXTypes    eType   = rTOX.GetTOXType()->GetType();
    USHORT      nCount  = pDoc->GetTOXTypeCount( eType );
    const SwTOXType* pType = 0;
    const XubString& rNm = rTOX.GetTOXType()->GetTypeName();

    // kein entsprechender Verzeichnistyp vorhanden -> anlegen
    // sonst verwenden
    for(USHORT i=0; i < nCount; ++i)
    {
        const SwTOXType* pSrcType = pDoc->GetTOXType(eType, i);
        if(pSrcType->GetTypeName() == rNm )
        {
            pType = pSrcType;
            break;
        }
    }
    // kein entsprechender Typ vorhanden -> neu erzeugen
    //
    if(!pType)
    {
        pDoc->InsertTOXType( SwTOXType( eType, rNm ) );
        pType = pDoc->GetTOXType(eType, 0);
    }
    // Verzeichnistyp umhaengen
    //
    ((SwTOXType*)pType)->Add( &rTOX );
}
