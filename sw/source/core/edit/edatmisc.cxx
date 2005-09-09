/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edatmisc.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:25:54 $
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

#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>      // fuer aNodes
#endif
#ifndef _PAM_HXX
#include <pam.hxx>      // fuer SwPaM
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>    // fuer MACROS
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>   // fuer die UndoIds
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>    // fuer Get-/ChgFmt Set-/GetAttrXXX
#endif



/*************************************
 * harte Formatierung (Attribute)
 *************************************/


void SwEditShell::ResetAttr()
{
    SET_CURR_SHELL( this );
    StartAllAction();
    BOOL bUndoGroup = GetCrsr()->GetNext() != GetCrsr();
    if( bUndoGroup )
        GetDoc()->StartUndo(UNDO_RESETATTR);

        FOREACHPAM_START(this)
            // if ( PCURCRSR->HasMark() )
                GetDoc()->ResetAttr(*PCURCRSR);
        FOREACHPAM_END()

    if( bUndoGroup )
        GetDoc()->EndUndo(UNDO_RESETATTR);
    CallChgLnk();
    EndAllAction();
}



void SwEditShell::GCAttr()
{
//JP 04.02.97: wozu eine Action-Klammerung - ein Formatierung sollte nicht
//				ausgeloest werden, so dass es hier ueberfluessig ist.
//				Sonst Probleme im MouseBut.DownHdl - Bug 35562
//	StartAllAction();
    FOREACHPAM_START(this)
        SwTxtNode *pTxtNode;
        if ( !PCURCRSR->HasMark() )
        {
            if( 0 != (pTxtNode = GetDoc()->GetNodes()[
                                PCURCRSR->GetPoint()->nNode]->GetTxtNode()))
                pTxtNode->GCAttr();
        }
        else
        {
            const SwNodeIndex& rEnd = PCURCRSR->End()->nNode;
            SwNodeIndex aIdx( PCURCRSR->Start()->nNode );
            SwNode* pNd = &aIdx.GetNode();
            do {
                if( pNd->IsTxtNode() )
                    ((SwTxtNode*)pNd)->GCAttr();
            }
            while( 0 != ( pNd = GetDoc()->GetNodes().GoNext( &aIdx )) &&
                    aIdx <= rEnd );
        }
    FOREACHPAM_END()
//	EndAllAction();
}

// Setze das Attribut als neues default Attribut im Dokument.


void SwEditShell::SetDefault( const SfxPoolItem& rFmtHint )
{
    // 7502: Action-Klammerung
    StartAllAction();
    GetDoc()->SetDefault( rFmtHint );
    EndAllAction();
}

/*

void SwEditShell::SetDefault( const SfxItemSet& rSet )
{
    // 7502: Action-Klammerung
    StartAllAction();
    GetDoc()->SetDefault( rSet );
    EndAllAction();
}
*/

// Erfrage das Default Attribut in diesem Dokument.

const SfxPoolItem& SwEditShell::GetDefault( USHORT nFmtHint ) const
{
    return GetDoc()->GetDefault( nFmtHint );

}


void SwEditShell::SetAttr( const SfxPoolItem& rHint, USHORT nFlags )
{
    SET_CURR_SHELL( this );
    StartAllAction();
    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr ) 	// Ring von Cursorn
    {
        FASTBOOL bIsTblMode = IsTableMode();
        GetDoc()->StartUndo(UNDO_INSATTR);

        FOREACHPAM_START(this)
            if( PCURCRSR->HasMark() && ( bIsTblMode ||
                *PCURCRSR->GetPoint() != *PCURCRSR->GetMark() ))
                GetDoc()->Insert(*PCURCRSR, rHint, nFlags );
        FOREACHPAM_END()

        GetDoc()->EndUndo(UNDO_INSATTR);
    }
    else
    {
        if( !HasSelection() )
            UpdateAttr();
        GetDoc()->Insert( *pCrsr, rHint, nFlags );
    }
    EndAllAction();
}


void SwEditShell::SetAttr( const SfxItemSet& rSet, USHORT nFlags )
{
    SET_CURR_SHELL( this );
    StartAllAction();
    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr ) 	// Ring von Cursorn
    {
        FASTBOOL bIsTblMode = IsTableMode();
        GetDoc()->StartUndo(UNDO_INSATTR);

        FOREACHPAM_START(this)
            if( PCURCRSR->HasMark() && ( bIsTblMode ||
                *PCURCRSR->GetPoint() != *PCURCRSR->GetMark() ))
                GetDoc()->Insert(*PCURCRSR, rSet, nFlags );
        FOREACHPAM_END()

        GetDoc()->EndUndo(UNDO_INSATTR);
    }
    else
    {
        if( !HasSelection() )
            UpdateAttr();
        GetDoc()->Insert( *pCrsr, rSet, nFlags );
    }
    EndAllAction();
}




