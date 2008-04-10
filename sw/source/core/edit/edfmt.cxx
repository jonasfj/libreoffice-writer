/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: edfmt.cxx,v $
 * $Revision: 1.10 $
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
#include "editsh.hxx"
#include "swtable.hxx"
#include "pam.hxx"
#include <docary.hxx>
#include <fchrfmt.hxx>
#include <frmfmt.hxx>
#include <charfmt.hxx>
#include "ndtxt.hxx"    // Fuer GetXXXFmt
#include "hints.hxx"

/*************************************
 * Formate
 *************************************/
// Char
// OPT: inline


USHORT SwEditShell::GetCharFmtCount() const
{
    return GetDoc()->GetCharFmts()->Count();
}


SwCharFmt& SwEditShell::GetCharFmt(USHORT nFmt) const
{
    return *((*(GetDoc()->GetCharFmts()))[nFmt]);
}


SwCharFmt* SwEditShell::GetCurCharFmt() const
{
    SwCharFmt *pFmt = 0;
    SfxItemSet aSet( GetDoc()->GetAttrPool(), RES_TXTATR_CHARFMT,
                                                RES_TXTATR_CHARFMT );
    const SfxPoolItem* pItem;
    if( GetCurAttr( aSet ) && SFX_ITEM_SET ==
        aSet.GetItemState( RES_TXTATR_CHARFMT, FALSE, &pItem ) )
        pFmt = ((SwFmtCharFmt*)pItem)->GetCharFmt();

    return pFmt;
}


void SwEditShell::FillByEx(SwCharFmt* pCharFmt, BOOL bReset)
{
    if ( bReset )
    {
        // --> OD 2007-01-25 #i73790# - method renamed
        pCharFmt->ResetAllFmtAttr();
        // <--
    }

    SwPaM* pPam = GetCrsr();
    const SwCntntNode* pCNd = pPam->GetCntntNode();
    if( pCNd->IsTxtNode() )
    {
        xub_StrLen nStt, nEnd;
        if( pPam->HasMark() )
        {
            const SwPosition* pPtPos = pPam->GetPoint();
            const SwPosition* pMkPos = pPam->GetMark();
            if( pPtPos->nNode == pMkPos->nNode )		// im selben Node ?
            {
                nStt = pPtPos->nContent.GetIndex();
                if( nStt < pMkPos->nContent.GetIndex() )
                    nEnd = pMkPos->nContent.GetIndex();
                else
                {
                    nEnd = nStt;
                    nStt = pMkPos->nContent.GetIndex();
                }
            }
            else
            {
                nStt = pMkPos->nContent.GetIndex();
                if( pPtPos->nNode < pMkPos->nNode )
                {
                    nEnd = nStt;
                    nStt = 0;
                }
                else
                    nEnd = ((SwTxtNode*)pCNd)->GetTxt().Len();
            }
        }
        else
            nStt = nEnd = pPam->GetPoint()->nContent.GetIndex();

        SfxItemSet aSet( pDoc->GetAttrPool(),
                            pCharFmt->GetAttrSet().GetRanges() );
        ((SwTxtNode*)pCNd)->GetAttr( aSet, nStt, nEnd );
        pCharFmt->SetAttr( aSet );
    }
    else if( pCNd->HasSwAttrSet() )
        pCharFmt->SetAttr( *pCNd->GetpSwAttrSet() );
}

// Frm
USHORT SwEditShell::GetTblFrmFmtCount(BOOL bUsed) const
{
    return GetDoc()->GetTblFrmFmtCount(bUsed);
}

SwFrmFmt& SwEditShell::GetTblFrmFmt(USHORT nFmt, BOOL bUsed ) const
{
    return GetDoc()->GetTblFrmFmt(nFmt, bUsed );
}

String SwEditShell::GetUniqueTblName() const
{
    return GetDoc()->GetUniqueTblName();
}


SwCharFmt* SwEditShell::MakeCharFmt( const String& rName,
                                    SwCharFmt* pDerivedFrom )
{
    if( !pDerivedFrom )
        pDerivedFrom = GetDoc()->GetDfltCharFmt();

    return GetDoc()->MakeCharFmt( rName, pDerivedFrom );
}

//----------------------------------
// inlines im Product


SwTxtFmtColl* SwEditShell::GetTxtCollFromPool( USHORT nId )
{
    return GetDoc()->GetTxtCollFromPool( nId );
}


    // return das geforderte automatische  Format - Basis-Klasse !
SwFmt* SwEditShell::GetFmtFromPool( USHORT nId )
{
    return GetDoc()->GetFmtFromPool( nId );
}


SwPageDesc* SwEditShell::GetPageDescFromPool( USHORT nId )
{
    return GetDoc()->GetPageDescFromPool( nId );
}


BOOL SwEditShell::IsUsed( const SwModify& rModify ) const
{
    return pDoc->IsUsed( rModify );
}

const SwFlyFrmFmt* SwEditShell::FindFlyByName( const String& rName, BYTE nNdTyp ) const
{
    return pDoc->FindFlyByName(rName, nNdTyp);
}

SwCharFmt* SwEditShell::FindCharFmtByName( const String& rName ) const
{
    return pDoc->FindCharFmtByName( rName );
}

SwTxtFmtColl* SwEditShell::FindTxtFmtCollByName( const String& rName ) const
{
    return pDoc->FindTxtFmtCollByName( rName );
}
