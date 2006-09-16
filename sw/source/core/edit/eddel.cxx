/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: eddel.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 21:05:01 $
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
#include "precompiled_sw.hxx"


#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>		// fuer die UndoIds
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>
#endif
#ifndef _BOOKMRK_HXX
#include <bookmrk.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SW_REWRITER_HXX
#include <SwRewriter.hxx>
#endif
#ifndef _UNOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif

#include <comcore.hrc>

/************************************************************
 * Loeschen
 ************************************************************/

void SwEditShell::DeleteSel( SwPaM& rPam, BOOL* pUndo )
{
    // nur bei Selektion
    if( !rPam.HasMark() || *rPam.GetPoint() == *rPam.GetMark())
        return;

    // besteht eine Selection in einer Tabelle ?
    // dann nur den Inhalt der selektierten Boxen loeschen
    // jetzt gibt es 2 Faelle die beachtet werden muessen:
    //	1. Point und Mark stehen in einer Box, Selection normal loeschen
    //	2. Point und Mark stehen in unterschiedlichen Boxen, alle
    // selektierten Boxen suchen in den Inhalt loeschen
    if( rPam.GetNode()->FindTableNode() &&
        rPam.GetNode()->StartOfSectionNode() !=
        rPam.GetNode(FALSE)->StartOfSectionNode() )
    {
        // in Tabellen das Undo gruppieren
        if( pUndo && !*pUndo )
        {
            GetDoc()->StartUndo( UNDO_START, NULL );
            *pUndo = TRUE;
        }
        SwPaM aDelPam( *rPam.Start() );
        const SwPosition* pEndSelPos = rPam.End();
        do {
            aDelPam.SetMark();
            SwNode* pNd = aDelPam.GetNode();
            const SwNode& rEndNd = *pNd->EndOfSectionNode();
            if( pEndSelPos->nNode.GetIndex() <= rEndNd.GetIndex() )
            {
                *aDelPam.GetPoint() = *pEndSelPos;
                pEndSelPos = 0;		// Pointer als Flag missbrauchen
            }
            else
            {
                // dann ans Ende der Section
                aDelPam.GetPoint()->nNode = rEndNd;
                aDelPam.Move( fnMoveBackward, fnGoCntnt );
            }
                // geschuetze Boxen ueberspringen !
            if( !pNd->IsCntntNode() ||
                !((SwCntntNode*)pNd)->GetFrm()->IsProtected() )
            {
                // alles loeschen
                GetDoc()->DeleteAndJoin( aDelPam );
                SaveTblBoxCntnt( aDelPam.GetPoint() );
            }

            if( !pEndSelPos )				// am Ende der Selection
                break;
            aDelPam.DeleteMark();
            aDelPam.Move( fnMoveForward, fnGoCntnt );	// naechste Box
        } while( pEndSelPos );
    }
    else
    {
            // alles loeschen
        GetDoc()->DeleteAndJoin( rPam );
        SaveTblBoxCntnt( rPam.GetPoint() );
    }

    // Selection wird nicht mehr benoetigt.
    rPam.DeleteMark();
}


long SwEditShell::Delete()
{
    SET_CURR_SHELL( this );
    long nRet = 0;
    if( !HasReadonlySel() )
    {
        StartAllAction();

        BOOL bUndo = GetCrsr()->GetNext() != GetCrsr();
        if( bUndo )     // mehr als eine Selection ?
        {
            SwRewriter aRewriter;
            aRewriter.AddRule(UNDO_ARG1, String(SW_RES(STR_MULTISEL)));

            GetDoc()->StartUndo( UNDO_DELETE, &aRewriter );            
        }

        FOREACHPAM_START(this)
            DeleteSel( *PCURCRSR, &bUndo );
        FOREACHPAM_END()

        // falls eine Undo-Klammerung, dann hier beenden
        if( bUndo )
            GetDoc()->EndUndo( UNDO_DELETE, NULL );
        EndAllAction();
        nRet = 1;
    }
    return nRet;
}

long SwEditShell::Copy( SwEditShell* pDestShell )
{
    if( !pDestShell )
        pDestShell = this;

    SET_CURR_SHELL( pDestShell );

    // ueberpruefe ob Selectionen in sich selbst kopiert werden
    if( pDestShell->GetDoc() == GetDoc() )
    {
        SwPosition * pPos = 0;
        FOREACHPAM_START(this)

            if( !pPos )
            {
                if( pDestShell == this )
                {
                    // der 1.Cursor ist die Position, wohin verschoben wird !!
                    PCURCRSR->DeleteMark();
                    pPos = (SwPosition*)PCURCRSR->GetPoint();
                    continue;
                }
                else
                    pPos = pDestShell->GetCrsr()->GetPoint();
            }
            if( *PCURCRSR->Start() <= *pPos && *pPos < *PCURCRSR->End() )
                return FALSE;		// Position im Bereich einer Selection
                                    // --> Kopieren in sich selbst
        FOREACHPAM_END()
    }

    pDestShell->StartAllAction();
    SwPosition * pPos = 0;
    BOOL bRet = FALSE;
    BOOL bFirstMove = TRUE;
    SwNodeIndex aSttNdIdx( pDestShell->GetDoc()->GetNodes() );
    xub_StrLen nSttCntIdx = 0;

    pDestShell->GetDoc()->StartUndo( UNDO_START, NULL );
    FOREACHPAM_START(this)

        if( !pPos )
        {
            if( pDestShell == this )
            {
                // der 1.Cursor ist die Position, wohin verschoben wird !!
                PCURCRSR->DeleteMark();
                pPos = (SwPosition*)PCURCRSR->GetPoint();
                continue;
            }
            else
                pPos = pDestShell->GetCrsr()->GetPoint();
        }

        // nur bei Selektion (nicht Textnodes haben Selection,
        // aber Point/GetMark sind gleich
        if( !PCURCRSR->HasMark() || *PCURCRSR->GetPoint() == *PCURCRSR->GetMark())
            continue;

        if( bFirstMove )
        {
            // Anfangs-Position vom neuen Bereich merken
            aSttNdIdx = pPos->nNode.GetIndex()-1;
            nSttCntIdx = pPos->nContent.GetIndex();
            bFirstMove = FALSE;
        }

        if( !GetDoc()->Copy( *PCURCRSR, *pPos ))
            continue;

        SwPaM aInsertPaM(*pPos, aSttNdIdx);
        pDestShell->GetDoc()->MakeUniqueNumRules(aInsertPaM);

        bRet = TRUE;
    FOREACHPAM_END()


    // wurde ueberhaupt etwas verschoben ?
    if( !bFirstMove )
    {
        SwPaM* pCrsr = pDestShell->GetCrsr();
        pCrsr->SetMark();
        pCrsr->GetPoint()->nNode = aSttNdIdx.GetIndex()+1;
        pCrsr->GetPoint()->nContent.Assign( pCrsr->GetCntntNode(),nSttCntIdx);
        pCrsr->Exchange();
    }
    else
    {
        // falls beim Move der Cursor "gewandert" ist, so setze hier auch
        // seinen GetMark um, damit dieser nie in den Wald zeigt.
        pDestShell->GetCrsr()->SetMark();
        pDestShell->GetCrsr()->DeleteMark();
    }
#if OSL_DEBUG_LEVEL > 1
// pruefe ob die Indizies auch in den richtigen Nodes angemeldet sind
{
    SwPaM* pCmp = (SwPaM*)pDestShell->GetCrsr();        // sicher den Pointer auf Cursor
    do {
        ASSERT( pCmp->GetPoint()->nContent.GetIdxReg()
                    == pCmp->GetCntntNode(), "Point im falschen Node" );
        ASSERT( pCmp->GetMark()->nContent.GetIdxReg()
                    == pCmp->GetCntntNode(FALSE), "Mark im falschen Node" );
        BOOL bTst = *pCmp->GetPoint() == *pCmp->GetMark();
    } while( pDestShell->GetCrsr() != ( pCmp = (SwPaM*)pCmp->GetNext() ) );
}
#endif

    // Undo-Klammerung hier beenden
    pDestShell->GetDoc()->EndUndo( UNDO_END, NULL );
    pDestShell->EndAllAction();

    pDestShell->SaveTblBoxCntnt( pDestShell->GetCrsr()->GetPoint() );

    return (long)bRet;
}


    // Ersetz einen selektierten Bereich in einem TextNode mit dem
    // String. Ist fuers Suchen&Ersetzen gedacht.
    // bRegExpRplc - ersetze Tabs (\\t) und setze den gefundenen String
    //				 ein ( nicht \& )
    // 				z.B.: Fnd: "zzz", Repl: "xx\t\\t..&..\&"
    //						--> "xx\t<Tab>..zzz..&"
BOOL SwEditShell::Replace( const String& rNewStr, BOOL bRegExpRplc )
{
    SET_CURR_SHELL( this );

    BOOL bRet = FALSE;
    if( !HasReadonlySel() )
    {
        StartAllAction();
        GetDoc()->StartUndo(0, NULL);

        FOREACHPAM_START(this)

//JP 02.12.97: muss das noch sein??
            // sollten mehrere Node selektiert sein, dann loesche diese
            // erst, fuege ein Zeichen ein und ersetze dann dieses
            if( PCURCRSR->GetPoint()->nNode != PCURCRSR->GetMark()->nNode )
            {
                BOOL bForward = PCURCRSR->GetPoint()->nNode.GetIndex() >
                                PCURCRSR->GetMark()->nNode.GetIndex();
                DeleteSel( *PCURCRSR );
                pDoc->Insert( *PCURCRSR, ' ' );
                PCURCRSR->SetMark();
                if( bForward )
                    PCURCRSR->GetMark()->nContent--;
                else
                    PCURCRSR->GetPoint()->nContent--;
            }
//JP 02.12.97: muss das noch sein??

            if( PCURCRSR->HasMark() && *PCURCRSR->GetMark() != *PCURCRSR->GetPoint() )
            {
                bRet |= GetDoc()->Replace( *PCURCRSR, rNewStr, bRegExpRplc );
                SaveTblBoxCntnt( PCURCRSR->GetPoint() );
            }
        FOREACHPAM_END()

        // Undo-Klammerung hier beenden
        GetDoc()->EndUndo(0, NULL);
        EndAllAction();
    }
    return bRet;
}


    // Special-Methode fuer JOE's- Wizzards
BOOL SwEditShell::DelFullPara()
{
    BOOL bRet = FALSE;
    if( !IsTableMode() )
    {
        SwPaM* pCrsr = GetCrsr();
        // keine Mehrfach-Selection
        if( pCrsr->GetNext() == pCrsr && !HasReadonlySel() )
        {
            SET_CURR_SHELL( this );
            StartAllAction();
            bRet = GetDoc()->DelFullPara( *pCrsr );
            EndAllAction();
        }
    }
    return bRet;
}



