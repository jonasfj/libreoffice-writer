/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ndnum.cxx,v $
 * $Revision: 1.22 $
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


#include <node.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <fldbas.hxx>			// UpdateFlds der KapitelNummerierung
#include <docary.hxx>

_SV_IMPL_SORTAR_ALG( SwOutlineNodes, SwNodePtr )
BOOL SwOutlineNodes::Seek_Entry( const SwNodePtr rSrch, USHORT* pFndPos ) const
{
    ULONG nIdx = rSrch->GetIndex();

    USHORT nO = Count(), nM, nU = 0;
    if( nO > 0 )
    {
//JP 17.03.98: aufgrund des Bug 48592 - wo unter anderem nach Undo/Redo
//				Nodes aus dem falschen NodesArray im OutlineArray standen,
//				jetzt mal einen Check eingebaut.
#ifndef PRODUCT
        {
            for( USHORT n = 1; n < nO; ++n )
                if( &(*this)[ n-1 ]->GetNodes() !=
                    &(*this)[ n ]->GetNodes() )
                {
                    ASSERT( !this, "Node im falschen Outline-Array" );
                }
        }
#endif

        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;
            if( (*this)[ nM ] == rSrch )
            {
                if( pFndPos )
                    *pFndPos = nM;
                return TRUE;
            }
            else if( (*this)[ nM ]->GetIndex() < nIdx )
                nU = nM + 1;
            else if( nM == 0 )
            {
                if( pFndPos )
                    *pFndPos = nU;
                return FALSE;
            }
            else
                nO = nM - 1;
        }
    }
    if( pFndPos )
        *pFndPos = nU;
    return FALSE;
}

void SwNodes::UpdateOutlineNode(SwNode & rNd)
{
    SwTxtNode * pTxtNd = rNd.GetTxtNode();

    if (pTxtNd && pTxtNd->IsOutlineStateChanged())
    {
        BOOL bFound = pOutlineNds->Seek_Entry(pTxtNd);

        if (pTxtNd->IsOutline())
        {
            if (! bFound)
            {
                // --> OD 2005-11-02 #125329#
                // assure that text is in the correct nodes array
                if ( &(pTxtNd->GetNodes()) == this )
                {
                    pOutlineNds->Insert(pTxtNd);
                }
                else
                {
                    ASSERT( false,
                            "<SwNodes::UpdateOutlineNode(..)> - given text node isn't in the correct nodes array. This is a serious defect -> inform OD" );
                }
                // <--
            }
        }
        else
        {
            if (bFound)
                pOutlineNds->Remove(pTxtNd);
        }

        pTxtNd->UpdateOutlineState();

        // die Gliederungs-Felder Updaten
        GetDoc()->GetSysFldType( RES_CHAPTERFLD )->UpdateFlds();
    }
}

void SwNodes::UpdateOutlineNode( const SwNode& rNd, BYTE nOldLevel,
                                 BYTE nNewLevel )
{
    const SwNodePtr pSrch = (SwNodePtr)&rNd;
    USHORT nSttPos;
    BOOL bSeekIdx = pOutlineNds->Seek_Entry( pSrch, &nSttPos );

    if( NO_NUMBERING == nOldLevel )			// neuen Level einfuegen
    {
        // nicht vorhanden, also einfuegen
        //ASSERT( !bSeekIdx, "Der Node ist schon als OutlineNode vorhanden" );

        //JP 12.03.99: 63293 - Nodes vom RedlineBereich NIE aufnehmen
        ULONG nNd = rNd.GetIndex();
        if( nNd < GetEndOfRedlines().GetIndex() &&
            nNd > GetEndOfRedlines().StartOfSectionNode()->GetIndex() )
            return ;

        // jetzt noch alle nachfolgende Outline-Nodes updaten
        if (! bSeekIdx)
        {
            // --> OD 2005-11-03 #125329#
            // assure that node <pSrch> is in the correct nodes array
            if ( &(pSrch->GetNodes()) == this )
            {
                pOutlineNds->Insert( pSrch );
            }
            else
            {
                ASSERT( false,
                        "<SwNodes::UpdateOutlineNode(..)> - node <pSrch> isn't in correct nodes array. This is a serious defect -> inform OD" );
            }
            // <--
        }

    }
    else if( NO_NUMBERING == nNewLevel )	// Level entfernen
    {
        if( !bSeekIdx )
            return;

        // jetzt noch alle nachfolgende Outline-Nodes updaten
        pOutlineNds->Remove( nSttPos );
    }
    else if( !bSeekIdx )		// Update und Index nicht gefunden ??
        return ;

    {
        SwTxtNode & rTxtNd = (SwTxtNode &) rNd;
        SwPaM aPam(rTxtNd); // #115901#

        if (nNewLevel != NO_NUMBERING) // #115901#
        {
            rTxtNd.SetAttrListLevel(rTxtNd.GetTxtColl()->GetOutlineLevel());

            rTxtNd.NumRuleChgd();
        }
        else
        {
            GetDoc()->DelNumRules(aPam);
        }
    }

    // die Gliederungs-Felder Updaten
    GetDoc()->GetSysFldType( RES_CHAPTERFLD )->UpdateFlds();
    GetDoc()->ChkCondColls();
}

void SwNodes::UpdtOutlineIdx( const SwNode& rNd )
{
    if( !pOutlineNds->Count() )		// keine OutlineNodes vorhanden ?
        return;

    const SwNodePtr pSrch = (SwNodePtr)&rNd;
    USHORT nPos;
    pOutlineNds->Seek_Entry( pSrch, &nPos );
    if( nPos == pOutlineNds->Count() )		// keine zum Updaten vorhanden ?
        return;

    if( nPos )
        --nPos;

    if( !GetDoc()->IsInDtor() && IsDocNodes() )
        UpdateOutlineNode( *(*pOutlineNds)[ nPos ]);
}

const SwOutlineNodes & SwNodes::GetOutLineNds() const
{
    return *pOutlineNds;
}
