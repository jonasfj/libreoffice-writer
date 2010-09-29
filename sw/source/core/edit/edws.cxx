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




#include <vcl/window.hxx>
#include <editsh.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <docary.hxx>
#include <acorrect.hxx>
#include <swtable.hxx>
#include <ndtxt.hxx>
#include <swundo.hxx>
#include <SwRewriter.hxx>

/********************************************************
 * Ctor/Dtor
 ********************************************************/
// verkleideter Copy-Constructor


SwEditShell::SwEditShell( SwEditShell& rEdSH, Window *pWindow )
    : SwCrsrShell( rEdSH, pWindow )
{
}

// ctor/dtor


SwEditShell::SwEditShell( SwDoc& rDoc, Window *pWindow, const SwViewOption *pOptions )
    : SwCrsrShell( rDoc, pWindow, pOptions )
{
    GetDoc()->DoUndo(true);
}


SwEditShell::~SwEditShell() // USED
{
}

/******************************************************************************
 *					sal_Bool SwEditShell::IsModified() const
 ******************************************************************************/


sal_Bool SwEditShell::IsModified() const
{
    return GetDoc()->IsModified();
}
/******************************************************************************
 *					  void SwEditShell::SetModified()
 ******************************************************************************/


void SwEditShell::SetModified()
{
    GetDoc()->SetModified();
}
/******************************************************************************
 *					 void SwEditShell::ResetModified()
 ******************************************************************************/


void SwEditShell::ResetModified()
{
    GetDoc()->ResetModified();
}

void SwEditShell::SetUndoNoResetModified()
{
    GetDoc()->SetModified();
    GetDoc()->SetUndoNoResetModified();
}

/******************************************************************************
 *				   void SwEditShell::StartAllAction()
 ******************************************************************************/


void SwEditShell::StartAllAction()
{
    ViewShell *pSh = this;
    do {
        if( pSh->IsA( TYPE( SwEditShell ) ) )
            ((SwEditShell*)pSh)->StartAction();
        else
            pSh->StartAction();
        pSh = (ViewShell *)pSh->GetNext();
    } while(pSh != this);
}
/******************************************************************************
 *					void SwEditShell::EndAllAction()
 ******************************************************************************/


void SwEditShell::EndAllAction()
{
    ViewShell *pSh = this;
    do {
        if( pSh->IsA( TYPE( SwEditShell ) ) )
            ((SwEditShell*)pSh)->EndAction();
        else
            pSh->EndAction();
        pSh = (ViewShell *)pSh->GetNext();
    } while(pSh != this);
}

/******************************************************************************
 *					void SwEditShell::CalcLayout()
 ******************************************************************************/


void SwEditShell::CalcLayout()
{
    StartAllAction();
    ViewShell::CalcLayout();

    ViewShell *pSh = this;
    do
    {
        if ( pSh->GetWin() )
            pSh->GetWin()->Invalidate();
        pSh = (ViewShell*)pSh->GetNext();

    } while ( pSh != this );

    EndAllAction();
}

/******************************************************************************
 *						Inhaltsform bestimmen, holen
 ******************************************************************************/
// OPT: wird fuer jedes Attribut gerufen?


sal_uInt16 SwEditShell::GetCntType() const
{
    // nur noch am SPoint ist der Inhalt interessant
    sal_uInt16 nRet = 0;
    if( IsTableMode() )
        nRet = CNT_TXT;
    else
        switch( GetCrsr()->GetNode()->GetNodeType() )
        {
        case ND_TEXTNODE:   nRet = CNT_TXT;	break;
        case ND_GRFNODE:    nRet = CNT_GRF; break;
        case ND_OLENODE:    nRet = CNT_OLE; break;
        }

    ASSERT( nRet, ERR_OUTOFSCOPE );
    return nRet;
}

//------------------------------------------------------------------------------


sal_Bool SwEditShell::HasOtherCnt() const

{
    if ( GetDoc()->GetSpzFrmFmts()->Count() )
        return sal_True;

    const SwNodes &rNds = GetDoc()->GetNodes();
    const SwNode *pNd;

    pNd = &rNds.GetEndOfInserts();
    if ( 1 != (pNd->GetIndex() - pNd->StartOfSectionIndex()) )
        return sal_True;

    pNd = &rNds.GetEndOfAutotext();
    if ( 1 != (pNd->GetIndex() - pNd->StartOfSectionIndex()) )
        return sal_True;

    return sal_False;
}

/******************************************************************************
 *				Zugriffsfunktionen fuer Filename-Behandlung
 ******************************************************************************/


SwActKontext::SwActKontext(SwEditShell *pShell)
    : pSh(pShell)
{
    pSh->StartAction();
}


SwActKontext::~SwActKontext()
{
    pSh->EndAction();
}

/******************************************************************************
 * 			Klasse fuer den automatisierten Aufruf von Start- und
 * 								EndCrsrMove();
 ******************************************************************************/


SwMvKontext::SwMvKontext(SwEditShell *pShell ) : pSh(pShell)
{
    pSh->SttCrsrMove();
}


SwMvKontext::~SwMvKontext()
{
    pSh->EndCrsrMove();
}


SwFrmFmt *SwEditShell::GetTableFmt()	// OPT: schnellster Test auf Tabelle?
{
    const SwTableNode* pTblNd = IsCrsrInTbl();
    return pTblNd ? (SwFrmFmt*)pTblNd->GetTable().GetFrmFmt() : 0;
}

// OPT: wieso 3x beim neuen Dokument


sal_uInt16 SwEditShell::GetTOXTypeCount(TOXTypes eTyp) const
{
    return pDoc->GetTOXTypeCount(eTyp);
}


void SwEditShell::InsertTOXType(const SwTOXType& rTyp)
{
    pDoc->InsertTOXType(rTyp);
}



void SwEditShell::DoUndo( sal_Bool bOn )
{ GetDoc()->DoUndo( bOn ); }


sal_Bool SwEditShell::DoesUndo() const
{ return GetDoc()->DoesUndo(); }


void SwEditShell::DoGroupUndo( sal_Bool bOn )
{ GetDoc()->DoGroupUndo( bOn ); }


sal_Bool SwEditShell::DoesGroupUndo() const
{ return GetDoc()->DoesGroupUndo(); }


void SwEditShell::DelAllUndoObj()
{
    GetDoc()->DelAllUndoObj();
}

// Zusammenfassen von Kontinuierlichen Insert/Delete/Overwrite von
// Charaktern. Default ist sdbcx::Group-Undo.

// setzt Undoklammerung auf, liefert nUndoId der Klammerung


SwUndoId SwEditShell::StartUndo( SwUndoId eUndoId,
                                   const SwRewriter *pRewriter )
{ return GetDoc()->StartUndo( eUndoId, pRewriter ); }

// schliesst Klammerung der nUndoId, nicht vom UI benutzt


SwUndoId SwEditShell::EndUndo(SwUndoId eUndoId,
                                const SwRewriter *pRewriter)
{ return GetDoc()->EndUndo(eUndoId, pRewriter); }

// liefert die Id der letzten undofaehigen Aktion zurueck
// fuellt ggf. VARARR mit sdbcx::User-UndoIds


SwUndoId SwEditShell::GetUndoIds(String* pStr,SwUndoIds *pUndoIds) const
{ return GetDoc()->GetUndoIds(pStr,pUndoIds); }

String SwEditShell::GetUndoIdsStr(String* pStr,SwUndoIds *pUndoIds) const
{ return GetDoc()->GetUndoIdsStr(pStr,pUndoIds); }

// liefert die Id der letzten Redofaehigen Aktion zurueck
// fuellt ggf. VARARR mit RedoIds


SwUndoId SwEditShell::GetRedoIds(String* pStr,SwUndoIds *pRedoIds) const
{ return GetDoc()->GetRedoIds(pStr,pRedoIds); }

String SwEditShell::GetRedoIdsStr(String* pStr,SwUndoIds *pRedoIds) const
{ return GetDoc()->GetRedoIdsStr(pStr,pRedoIds); }

// liefert die Id der letzten Repeatfaehigen Aktion zurueck
// fuellt ggf. VARARR mit RedoIds


SwUndoId SwEditShell::GetRepeatIds(String* pStr, SwUndoIds *pRedoIds) const
{ return GetDoc()->GetRepeatIds(pStr,pRedoIds); }

String SwEditShell::GetRepeatIdsStr(String* pStr, SwUndoIds *pRedoIds) const
{ return GetDoc()->GetRepeatIdsStr(pStr,pRedoIds); }



// AutoKorrektur - JP 27.01.94
void SwEditShell::AutoCorrect( SvxAutoCorrect& rACorr, sal_Bool bInsert,
                                sal_Unicode cChar )
{
    SET_CURR_SHELL( this );

    StartAllAction();

    SwPaM* pCrsr = getShellCrsr( true );
    SwTxtNode* pTNd = pCrsr->GetNode()->GetTxtNode();

    SwAutoCorrDoc aSwAutoCorrDoc( *this, *pCrsr, cChar );
    rACorr.AutoCorrect( aSwAutoCorrDoc,
                    pTNd->GetTxt(), pCrsr->GetPoint()->nContent.GetIndex(),
                    cChar, bInsert, GetWin() );
    if( cChar )
        SaveTblBoxCntnt( pCrsr->GetPoint() );
    EndAllAction();
}


void SwEditShell::SetNewDoc(sal_Bool bNew)
{
    GetDoc()->SetNewDoc(bNew);
}


sal_Bool SwEditShell::GetPrevAutoCorrWord( SvxAutoCorrect& rACorr, String& rWord )
{
    SET_CURR_SHELL( this );

    sal_Bool bRet;
    SwPaM* pCrsr = getShellCrsr( true );
    xub_StrLen nPos = pCrsr->GetPoint()->nContent.GetIndex();
    SwTxtNode* pTNd = pCrsr->GetNode()->GetTxtNode();
    if( pTNd && nPos )
    {
        SwAutoCorrDoc aSwAutoCorrDoc( *this, *pCrsr, 0 );
        bRet = rACorr.GetPrevAutoCorrWord( aSwAutoCorrDoc,
                                            pTNd->GetTxt(), nPos, rWord );
    }
    else
        bRet = sal_False;
    return bRet;
}

SwAutoCompleteWord& SwEditShell::GetAutoCompleteWords()
{
    return SwDoc::GetAutoCompleteWords();
}



