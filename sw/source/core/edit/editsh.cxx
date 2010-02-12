/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: editsh.cxx,v $
 * $Revision: 1.58 $
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
#include <tools/list.hxx>
#include <tools/urlobj.hxx>
#include <vcl/cmdevt.hxx>
#include <unotools/charclass.hxx>
#include <comphelper/processfactory.hxx>
#include <unotools/transliterationwrapper.hxx>
#include <swwait.hxx>
#include <fmtsrnd.hxx>
#include <fmtinfmt.hxx>
#include <txtinet.hxx>
#include <frmfmt.hxx>
#include <charfmt.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <editsh.hxx>
#include <frame.hxx>
#include <cntfrm.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>			// fuer SwTxtNode
#include <grfatr.hxx>
#include <flyfrm.hxx>
#include <swtable.hxx>
#include <swundo.hxx> 			// UNDO_START, UNDO_END
#include <calc.hxx>
#include <edimp.hxx>
#include <ndgrf.hxx>
#include <ndole.hxx>
#include <txtfrm.hxx>
#include <rootfrm.hxx>
#include <extinput.hxx>
#include <crsskip.hxx>
#include <scriptinfo.hxx>
#include <unocrsrhelper.hxx>
#include <section.hxx>
#include <unochart.hxx>
#include <numrule.hxx>
#include <SwNodeNum.hxx>
#include <unocrsr.hxx>


using namespace com::sun::star;


SV_IMPL_PTRARR(SwGetINetAttrs, SwGetINetAttr*)

/******************************************************************************
 *						void SwEditShell::Insert(char c)
 ******************************************************************************/


void SwEditShell::Insert( sal_Unicode c, BOOL bOnlyCurrCrsr )
{
    StartAllAction();
    FOREACHPAM_START(this)

        const bool bSuccess = GetDoc()->InsertString(*PCURCRSR, c);
        ASSERT( bSuccess, "Doc->Insert() failed." );
        (void) bSuccess;

        SaveTblBoxCntnt( PCURCRSR->GetPoint() );
        if( bOnlyCurrCrsr )
            break;

    FOREACHPAM_END()

    EndAllAction();
}


/******************************************************************************
 *				  void SwEditShell::Insert(const String &rStr)
 ******************************************************************************/


void SwEditShell::Insert2(const String &rStr, const bool bForceExpandHints )
{
    StartAllAction();
    {
        const enum IDocumentContentOperations::InsertFlags nInsertFlags =
            (bForceExpandHints)
            ? static_cast<IDocumentContentOperations::InsertFlags>(
                    IDocumentContentOperations::INS_FORCEHINTEXPAND |
                    IDocumentContentOperations::INS_EMPTYEXPAND)
            : IDocumentContentOperations::INS_EMPTYEXPAND;

        SwPaM *_pStartCrsr = getShellCrsr( true ), *__pStartCrsr = _pStartCrsr;
        do {
            //OPT: GetSystemCharSet
            const bool bSuccess =
                GetDoc()->InsertString(*_pStartCrsr, rStr, nInsertFlags);
            ASSERT( bSuccess, "Doc->Insert() failed." );
            (void) bSuccess;

            SaveTblBoxCntnt( _pStartCrsr->GetPoint() );

        } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != __pStartCrsr );
    }

    // calculate cursor bidi level
    SwCursor* pTmpCrsr = _GetCrsr();
    const BOOL bDoNotSetBidiLevel = ! pTmpCrsr ||
                                ( 0 != dynamic_cast<SwUnoCrsr*>(pTmpCrsr) );

    if ( ! bDoNotSetBidiLevel )
    {
        SwNode& rNode = pTmpCrsr->GetPoint()->nNode.GetNode();
        if ( rNode.IsTxtNode() )
        {
            SwIndex& rIdx = pTmpCrsr->GetPoint()->nContent;
            xub_StrLen nPos = rIdx.GetIndex();
            xub_StrLen nPrevPos = nPos;
            if ( nPrevPos )
                --nPrevPos;

            SwScriptInfo* pSI = SwScriptInfo::GetScriptInfo( ((SwTxtNode&)rNode),
                                                              sal_True );

            BYTE nLevel = 0;
            if ( ! pSI )
            {
                // seems to be an empty paragraph.
                Point aPt;
                SwCntntFrm* pFrm =
                        ((SwTxtNode&)rNode).GetFrm( &aPt, pTmpCrsr->GetPoint(),
                                                    sal_False );

                SwScriptInfo aScriptInfo;
                aScriptInfo.InitScriptInfo( (SwTxtNode&)rNode, pFrm->IsRightToLeft() );
                nLevel = aScriptInfo.DirType( nPrevPos );
            }
            else
            {
                if ( STRING_LEN != pSI->GetInvalidity() )
                    pSI->InitScriptInfo( (SwTxtNode&)rNode );
                nLevel = pSI->DirType( nPrevPos );
            }

            pTmpCrsr->SetCrsrBidiLevel( nLevel );
        }
    }

    SetInFrontOfLabel( FALSE ); // #i27615#

    EndAllAction();
}


/******************************************************************************
 *				void SwEditShell::Overwrite(const String &rStr)
 ******************************************************************************/


void SwEditShell::Overwrite(const String &rStr)
{
    StartAllAction();
    FOREACHPAM_START(this)
        if( !GetDoc()->Overwrite(*PCURCRSR, rStr ) )
        {
            ASSERT( FALSE, "Doc->Overwrite(Str) failed." )
        }
        SaveTblBoxCntnt( PCURCRSR->GetPoint() );
    FOREACHPAM_END()
    EndAllAction();
}


/******************************************************************************
 *						long SwEditShell::SplitNode()
 ******************************************************************************/

long SwEditShell::SplitNode( BOOL bAutoFormat, BOOL bCheckTableStart )
{
    StartAllAction();
    GetDoc()->StartUndo(UNDO_EMPTY, NULL);

    FOREACHPAM_START(this)
        // eine Tabellen Zelle wird jetzt zu einer normalen Textzelle!
        GetDoc()->ClearBoxNumAttrs( PCURCRSR->GetPoint()->nNode );
        GetDoc()->SplitNode( *PCURCRSR->GetPoint(), bCheckTableStart );
    FOREACHPAM_END()

    GetDoc()->EndUndo(UNDO_EMPTY, NULL);

    if( bAutoFormat )
        AutoFmtBySplitNode();

    ClearTblBoxCntnt();

    EndAllAction();
    return(1L);
}

/*-- 11.05.2004 09:41:20---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwEditShell::AppendTxtNode()
{
    sal_Bool bRet = sal_False;
    StartAllAction();
    GetDoc()->StartUndo(UNDO_EMPTY, NULL);

    FOREACHPAM_START(this)
        GetDoc()->ClearBoxNumAttrs( PCURCRSR->GetPoint()->nNode );
        bRet = GetDoc()->AppendTxtNode( *PCURCRSR->GetPoint()) || bRet;
    FOREACHPAM_END()

    GetDoc()->EndUndo(UNDO_EMPTY, NULL);

    ClearTblBoxCntnt();

    EndAllAction();
    return bRet;
}

/******************************************************************************
 *		  liefert einen Pointer auf einen SwGrfNode; dieser wird von
 *		  		GetGraphic() und GetGraphicSize() verwendet.
 ******************************************************************************/


SwGrfNode * SwEditShell::_GetGrfNode() const
{
    SwGrfNode *pGrfNode = 0;
    SwPaM* pCrsr = GetCrsr();
    if( !pCrsr->HasMark() ||
        pCrsr->GetPoint()->nNode == pCrsr->GetMark()->nNode )
        pGrfNode = pCrsr->GetPoint()->nNode.GetNode().GetGrfNode();

    return pGrfNode;
}
/******************************************************************************
 * 		liefert Pointer auf eine Graphic, wenn CurCrsr->GetPoint() auf
 * 			 einen SwGrfNode zeigt (und GetMark nicht gesetzt ist
 * 					 oder auf die gleiche Graphic zeigt)
 ******************************************************************************/

// --> OD 2005-02-09 #119353# - robust
const Graphic* SwEditShell::GetGraphic( BOOL bWait ) const
{
    SwGrfNode* pGrfNode = _GetGrfNode();
    // --> OD 2005-02-09 #119353# - robust
    const Graphic* pGrf( 0L );
    if ( pGrfNode )
    {
        pGrf = &(pGrfNode->GetGrf());
        // --> OD 2007-03-01 #i73788#
        // no load of linked graphic, if its not needed now (bWait = FALSE).
        if ( bWait )
        {
            if( pGrf->IsSwapOut() ||
                ( pGrfNode->IsLinkedFile() && GRAPHIC_DEFAULT == pGrf->GetType() ) )
            {
#ifdef DBG_UTIL
                ASSERT( pGrfNode->SwapIn( bWait ) || !bWait, "Grafik konnte nicht geladen werden" );
#else
                pGrfNode->SwapIn( bWait );
#endif
            }
        }
        else
        {
            if ( pGrf->IsSwapOut() && !pGrfNode->IsLinkedFile() )
            {
#ifdef DBG_UTIL
                ASSERT( pGrfNode->SwapIn( bWait ) || !bWait, "Grafik konnte nicht geladen werden" );
#else
                pGrfNode->SwapIn( bWait );
#endif
            }
        }
        // <--
    }
    return pGrf;
    // <--
}

BOOL SwEditShell::IsGrfSwapOut( BOOL bOnlyLinked ) const
{
    SwGrfNode *pGrfNode = _GetGrfNode();
    return pGrfNode &&
        (bOnlyLinked ? ( pGrfNode->IsLinkedFile() &&
                        ( GRAPHIC_DEFAULT == pGrfNode->GetGrfObj().GetType()||
                          pGrfNode->GetGrfObj().IsSwappedOut()))
                     : pGrfNode->GetGrfObj().IsSwappedOut());
}

// --> OD 2005-02-09 #119353# - robust
const GraphicObject* SwEditShell::GetGraphicObj() const
{
    SwGrfNode* pGrfNode = _GetGrfNode();
    // --> OD 2005-02-09 #119353# - robust
    return pGrfNode ? &(pGrfNode->GetGrfObj()) : 0L;
    // <--
}

USHORT SwEditShell::GetGraphicType() const
{
    SwGrfNode *pGrfNode = _GetGrfNode();
    return static_cast<USHORT>(pGrfNode ? pGrfNode->GetGrfObj().GetType() : GRAPHIC_NONE);
}

/******************************************************************************
 *		liefert die Groesse der Graphic, wenn CurCrsr->GetPoint() auf
 * 			einen SwGrfNode zeigt (und GetMark nicht gesetzt ist
 * 					oder auf die gleiche Graphic zeigt)
 ******************************************************************************/

BOOL SwEditShell::GetGrfSize(Size& rSz) const
{
    SwNoTxtNode* pNoTxtNd;
    SwPaM* pCurrentCrsr = GetCrsr();
    if( ( !pCurrentCrsr->HasMark()
         || pCurrentCrsr->GetPoint()->nNode == pCurrentCrsr->GetMark()->nNode )
         && 0 != ( pNoTxtNd = pCurrentCrsr->GetNode()->GetNoTxtNode() ) )
    {
        rSz = pNoTxtNd->GetTwipSize();
        return TRUE;
    }
    return FALSE;

}
/******************************************************************************
 * 		erneutes Einlesen, falls Graphic nicht Ok ist. Die
 * 		aktuelle wird durch die neue ersetzt.
 ******************************************************************************/

void SwEditShell::ReRead( const String& rGrfName, const String& rFltName,
                    const Graphic* pGraphic, const GraphicObject* pGrfObj )
{
    StartAllAction();
    pDoc->ReRead( *GetCrsr(), rGrfName, rFltName, pGraphic, pGrfObj );
    EndAllAction();
}


/******************************************************************************
 *	liefert den Namen und den FilterNamen einer Graphic, wenn der Cursor
 *	auf einer Graphic steht
 *	Ist ein String-Ptr != 0 dann returne den entsp. Namen
 ******************************************************************************/


void SwEditShell::GetGrfNms( String* pGrfName, String* pFltName,
                            const SwFlyFrmFmt* pFmt ) const
{
    ASSERT( pGrfName || pFltName, "was wird denn nun erfragt?" );
    if( pFmt )
        GetDoc()->GetGrfNms( *pFmt, pGrfName, pFltName );
    else
    {
        SwGrfNode *pGrfNode = _GetGrfNode();
        if( pGrfNode && pGrfNode->IsLinkedFile() )
            pGrfNode->GetFileFilterNms( pGrfName, pFltName );
    }
}


// alternativen Text abfragen/setzen
//const String& SwEditShell::GetAlternateText() const
//{
//    SwPaM* pCrsr = GetCrsr();
//    const SwNoTxtNode* pNd;
//    if( !pCrsr->HasMark() && 0 != ( pNd = pCrsr->GetNode()->GetNoTxtNode()) )
//        return pNd->GetAlternateText();

//    return aEmptyStr;
//}


//void SwEditShell::SetAlternateText( const String& rTxt )
//{
//    SwPaM* pCrsr = GetCrsr();
//    SwNoTxtNode* pNd;
//    if( !pCrsr->HasMark() && 0 != ( pNd = pCrsr->GetNode()->GetNoTxtNode()) )
//    {
//        pNd->SetAlternateText( rTxt, sal_True );
//        GetDoc()->SetModified();
//    }
//}


const PolyPolygon *SwEditShell::GetGraphicPolygon() const
{
    SwNoTxtNode *pNd = GetCrsr()->GetNode()->GetNoTxtNode();
    return pNd->HasContour();
}


void SwEditShell::SetGraphicPolygon( const PolyPolygon *pPoly )
{
    SwNoTxtNode *pNd = GetCrsr()->GetNode()->GetNoTxtNode();
    StartAllAction();
    pNd->SetContour( pPoly );
    SwFlyFrm *pFly = (SwFlyFrm*)pNd->GetFrm()->GetUpper();
    const SwFmtSurround &rSur = pFly->GetFmt()->GetSurround();
    pFly->GetFmt()->SwModify::Modify( (SwFmtSurround*)&rSur,
                                      (SwFmtSurround*)&rSur );
    GetDoc()->SetModified();
    EndAllAction();
}

void SwEditShell::ClearAutomaticContour()
{
    SwNoTxtNode *pNd = GetCrsr()->GetNode()->GetNoTxtNode();
    ASSERT( pNd, "is no NoTxtNode!" );
    if( pNd->HasAutomaticContour() )
    {
        StartAllAction();
        pNd->SetContour( NULL, FALSE );
        SwFlyFrm *pFly = (SwFlyFrm*)pNd->GetFrm()->GetUpper();
        const SwFmtSurround &rSur = pFly->GetFmt()->GetSurround();
        pFly->GetFmt()->SwModify::Modify( (SwFmtSurround*)&rSur,
                                          (SwFmtSurround*)&rSur );
        GetDoc()->SetModified();
        EndAllAction();
    }
}

/******************************************************************************
 * 		liefert Pointer auf ein SvInPlaceObjectRef, wenn CurCrsr->GetPoint() auf
 * 			einen SwOLENode zeigt (und GetMark nicht gesetzt ist
 * 					oder auf das gleiche SvInPlaceObjectRef zeigt)
 * 		besorgt den Pointer vom Doc wenn das Objekt per Namen gesucht werden
 * 		soll
 ******************************************************************************/

svt::EmbeddedObjectRef& SwEditShell::GetOLEObject() const
{
    ASSERT(  CNT_OLE == GetCntType(), "GetOLEObj: kein OLENode." );
    ASSERT( !GetCrsr()->HasMark() ||
            (GetCrsr()->HasMark() &&
                GetCrsr()->GetPoint()->nNode == GetCrsr()->GetMark()->nNode),
            "GetOLEObj: kein OLENode." );

    SwOLENode *pOLENode = GetCrsr()->GetNode()->GetOLENode();
    ASSERT( pOLENode, "GetOLEObj: kein OLENode." );
    SwOLEObj& rOObj = pOLENode->GetOLEObj();
    return rOObj.GetObject();
}


BOOL SwEditShell::HasOLEObj( const String &rName ) const
{
    SwStartNode *pStNd;
    SwNodeIndex aIdx( *GetNodes().GetEndOfAutotext().StartOfSectionNode(), 1 );
    while ( 0 != (pStNd = aIdx.GetNode().GetStartNode()) )
    {
        aIdx++;
        SwNode& rNd = aIdx.GetNode();
        if( rNd.IsOLENode() &&
            rName == ((SwOLENode&)rNd).GetChartTblName() &&
            ((SwOLENode&)rNd).GetFrm() )
            return TRUE;

        aIdx.Assign( *pStNd->EndOfSectionNode(), + 1 );
    }
    return FALSE;
}


void SwEditShell::SetChartName( const String &rName )
{
    SwOLENode *pONd = GetCrsr()->GetNode()->GetOLENode();
    ASSERT( pONd, "ChartNode not found" );
    pONd->SetChartTblName( rName );
}

void SwEditShell::UpdateCharts( const String &rName )
{
    GetDoc()->UpdateCharts( rName );
}


/******************************************************************************
 * 		Aenderung des Tabellennamens
 ******************************************************************************/

void SwEditShell::SetTableName( SwFrmFmt& rTblFmt, const String &rNewName )
{
    GetDoc()->SetTableName( rTblFmt, rNewName );
}

// erfragen des akt. Wortes

String SwEditShell::GetCurWord()
{
    const SwPaM& rPaM = *GetCrsr();
    const SwTxtNode* pNd = rPaM.GetNode()->GetTxtNode();
    String aString = pNd ?
                     pNd->GetCurWord(rPaM.GetPoint()->nContent.GetIndex()) :
                     aEmptyStr;
    return aString;
}

/****************************************************************************
 *			 void SwEditShell::UpdateDocStat( SwDocStat& rStat )
 ****************************************************************************/


void SwEditShell::UpdateDocStat( SwDocStat& rStat )
{
    StartAllAction();
    GetDoc()->UpdateDocStat( rStat );
    EndAllAction();
}

// OPT: eddocinl.cxx


    // returne zum Namen die im Doc gesetzte Referenz
const SwFmtRefMark* SwEditShell::GetRefMark( const String& rName ) const
{
    return GetDoc()->GetRefMark( rName );
}

    // returne die Namen aller im Doc gesetzten Referenzen
USHORT SwEditShell::GetRefMarks( SvStringsDtor* pStrings ) const
{
    return GetDoc()->GetRefMarks( pStrings );
}

/******************************************************************************
 *			DropCap-SS
 ******************************************************************************/


String SwEditShell::GetDropTxt( const USHORT nChars ) const
{
    /**
     * pb: made changes for #i74939#
     *
     * always return a string even though there is a selection
     */

    String aTxt;
    SwPaM* pCrsr = GetCrsr();
    if ( IsMultiSelection() )
    {
        // if a multi selection exists, search for the first line
        // -> it is the cursor with the lowest index
        ULONG nIndex = pCrsr->GetMark()->nNode.GetIndex();
        bool bPrev = true;
        SwPaM* pLast = pCrsr;
        SwPaM* pTemp = pCrsr;
        while ( bPrev )
        {
            SwPaM* pPrev2 = dynamic_cast< SwPaM* >( pTemp->GetPrev() );
            bPrev = ( pPrev2 && pPrev2 != pLast );
            if ( bPrev )
            {
                pTemp = pPrev2;
                ULONG nTemp = pPrev2->GetMark()->nNode.GetIndex();
                if ( nTemp < nIndex )
                {
                    nIndex = nTemp;
                    pCrsr = pPrev2;
                }
            }
        }
    }

    SwTxtNode* pTxtNd = pCrsr->GetNode( !pCrsr->HasMark() )->GetTxtNode();
    if( pTxtNd )
    {
        xub_StrLen nDropLen = pTxtNd->GetDropLen( nChars );
        if( nDropLen )
            aTxt = pTxtNd->GetTxt().Copy( 0, nDropLen );
    }

    return aTxt;
}

void SwEditShell::ReplaceDropTxt( const String &rStr )
{
    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetPoint()->nNode == pCrsr->GetMark()->nNode &&
        pCrsr->GetNode()->GetTxtNode()->IsTxtNode() )
    {
        StartAllAction();

        const SwNodeIndex& rNd = pCrsr->GetPoint()->nNode;
        SwPaM aPam( rNd, rStr.Len(), rNd, 0 );
        if( !GetDoc()->Overwrite( aPam, rStr ) )
        {
            ASSERT( FALSE, "Doc->Overwrite(Str) failed." );
        }

        EndAllAction();
    }
}

/******************************************************************************
 *	Methode		:
 *	Beschreibung:
 *	Erstellt	:	OK 25.04.94 13:45
 *	Aenderung	:
 ******************************************************************************/

String SwEditShell::Calculate()
{
    String 	aFormel;					// die entgueltige Formel
    SwPaM	*pPaMLast = (SwPaM*)GetCrsr()->GetNext(),
            *pPaM = pPaMLast;			// die Pointer auf Cursor
    SwCalc 	aCalc( *GetDoc() );
    const CharClass& rCC = GetAppCharClass();

    do {
        SwTxtNode* pTxtNd = pPaM->GetNode()->GetTxtNode();
        if(pTxtNd)
        {
            const SwPosition *pStart = pPaM->Start(), *pEnd = pPaM->End();
            xub_StrLen nStt = pStart->nContent.GetIndex();
            String aStr = pTxtNd->GetExpandTxt( nStt, pEnd->nContent.
                                                GetIndex() - nStt );

            rCC.toLower( aStr );

            sal_Unicode ch;
            BOOL 	bValidFlds = FALSE;
            xub_StrLen nPos = 0;

            while( nPos < aStr.Len() )
            {
                ch = aStr.GetChar( nPos++ );
                if( rCC.isLetter( aStr, nPos-1 ) || ch == '_' )
                {
                    xub_StrLen nTmpStt = nPos-1;
                    while(  nPos < aStr.Len() &&
                            0 != ( ch = aStr.GetChar( nPos++ )) &&
                           (rCC.isLetterNumeric( aStr, nPos - 1 ) ||
                               ch == '_'|| ch == '.' ))
                        ;

                    if( nPos < aStr.Len() )
                        --nPos;

                    String sVar( aStr.Copy( nTmpStt, nPos - nTmpStt ));
                    if( !::FindOperator( sVar ) &&
                        (::Find( sVar, aCalc.GetVarTable(),TBLSZ) ||
                         aCalc.VarLook( sVar )) )
                    {
                        if( !bValidFlds )
                        {
                            GetDoc()->FldsToCalc( aCalc,
                                                  pStart->nNode.GetIndex(),
                                                  pStart->nContent.GetIndex() );
                            bValidFlds = TRUE;
                        }
                        (( aFormel += '(' ) +=
                                aCalc.GetStrResult( aCalc.VarLook( sVar )
                                                        ->nValue )) += ')';
                    }
                    else
                        aFormel += sVar;
                }
                else
                    aFormel += ch;
            }
        }
    } while( pPaMLast != (pPaM = (SwPaM*)pPaM->GetNext()) );

    return aCalc.GetStrResult( aCalc.Calculate(aFormel) );
}


SvxLinkManager& SwEditShell::GetLinkManager()
{
    return pDoc->GetLinkManager();
}


void *SwEditShell::GetIMapInventor() const
{
    //Als eindeutige Identifikation sollte der Node, auf dem der Crsr steht
    //genuegen.
    return (void*)GetCrsr()->GetNode();
}

// --> OD 2007-03-01 #i73788#
// remove default parameter, because method is always called this default value
Graphic SwEditShell::GetIMapGraphic() const
{
    //Liefert immer eine Graphic, wenn der Crsr in einem Fly steht.
    SET_CURR_SHELL( (ViewShell*)this );
    Graphic aRet;
    SwPaM* pCrsr = GetCrsr();
    if ( !pCrsr->HasMark() )
    {
        SwNode *pNd =pCrsr->GetNode();
        if( pNd->IsGrfNode() )
        {
            const Graphic& rGrf = ((SwGrfNode*)pNd)->GetGrf();
            if( rGrf.IsSwapOut() || ( ((SwGrfNode*)pNd)->IsLinkedFile() &&
                                    GRAPHIC_DEFAULT == rGrf.GetType() ) )
            {
#ifdef DBG_UTIL
                ASSERT( ((SwGrfNode*)pNd)->SwapIn( TRUE ) || !TRUE, "Grafik konnte nicht geladen werden" );
#else
                ((SwGrfNode*)pNd)->SwapIn( TRUE );
#endif
            }
            aRet = rGrf;
        }
        else if ( pNd->IsOLENode() )
        {
            aRet = *((SwOLENode*)pNd)->GetGraphic();
        }
        else
        {
            SwFlyFrm* pFlyFrm = pNd->GetCntntNode()->GetFrm()->FindFlyFrm();
            if(pFlyFrm)
                aRet = pFlyFrm->GetFmt()->MakeGraphic();
        }
    }
    return aRet;
}


BOOL SwEditShell::InsertURL( const SwFmtINetFmt& rFmt, const String& rStr, BOOL bKeepSelection )
{
    // URL und Hinweistext (direkt oder via Selektion) notwendig
    if( !rFmt.GetValue().Len() ||	( !rStr.Len() && !HasSelection() ) )
        return FALSE;
    StartAllAction();
    GetDoc()->StartUndo( UNDO_UI_INSERT_URLTXT, NULL);
    BOOL bInsTxt = TRUE;

    if( rStr.Len() )
    {
        SwPaM* pCrsr = GetCrsr();
        if( pCrsr->HasMark() && *pCrsr->GetPoint() != *pCrsr->GetMark() )
        {
            // Selection vorhanden, MehrfachSelektion?
            BOOL bDelTxt = TRUE;
            if( pCrsr->GetNext() == pCrsr )
            {
                // einfach Selection -> Text ueberpruefen
                String sTxt( GetSelTxt() );
                sTxt.EraseTrailingChars();
                if( sTxt == rStr )
                    bDelTxt = bInsTxt = FALSE;
            }
            else if( rFmt.GetValue() == rStr )		// Name und URL gleich?
                bDelTxt = bInsTxt = FALSE;

            if( bDelTxt )
                Delete();
        }
        else if( pCrsr->GetNext() != pCrsr && rFmt.GetValue() == rStr )
            bInsTxt = FALSE;

        if( bInsTxt )
        {
            Insert2( rStr );
            SetMark();
            ExtendSelection( FALSE, rStr.Len() );
        }
    }
    else
        bInsTxt = FALSE;

    SetAttr( rFmt );
    if (bInsTxt && !IsCrsrPtAtEnd())
        SwapPam();
    if(!bKeepSelection)
        ClearMark();
    if( bInsTxt )
        DontExpandFmt();
    GetDoc()->EndUndo( UNDO_UI_INSERT_URLTXT, NULL );
    EndAllAction();
    return TRUE;
}


USHORT SwEditShell::GetINetAttrs( SwGetINetAttrs& rArr )
{
    if( rArr.Count() )
        rArr.DeleteAndDestroy( 0, rArr.Count() );

    const SwTxtNode* pTxtNd;
    const SwCharFmts* pFmts = GetDoc()->GetCharFmts();
    for( USHORT n = pFmts->Count(); 1 < n; )
    {
        SwClientIter aIter( *(*pFmts)[  --n ] );

        for( SwClient* pFnd = aIter.First(TYPE( SwTxtINetFmt ));
                pFnd; pFnd = aIter.Next() )
            if( 0 != ( pTxtNd = ((SwTxtINetFmt*)pFnd)->GetpTxtNode()) &&
                pTxtNd->GetNodes().IsDocNodes() )
            {
                SwTxtINetFmt& rAttr = *(SwTxtINetFmt*)pFnd;
                String sTxt( pTxtNd->GetExpandTxt( *rAttr.GetStart(),
                                    *rAttr.GetEnd() - *rAttr.GetStart() ) );

                sTxt.EraseAllChars( 0x0a );
                sTxt.EraseLeadingChars().EraseTrailingChars();

                if( sTxt.Len() )
                {
                    SwGetINetAttr* pNew = new SwGetINetAttr( sTxt, rAttr );
                    rArr.C40_INSERT( SwGetINetAttr, pNew, rArr.Count() );
                }
            }
    }
    return rArr.Count();
}


    // ist der Cursor in eine INetAttribut, dann wird das komplett
    // geloescht; inclusive des Hinweistextes (wird beim Drag&Drop gebraucht)
BOOL SwEditShell::DelINetAttrWithText()
{
    BOOL bRet = SelectTxtAttr( RES_TXTATR_INETFMT, FALSE );
    if( bRet )
        DeleteSel( *GetCrsr() );
    return bRet;
}


// setzen an den Textzeichenattributen das DontExpand-Flag
BOOL SwEditShell::DontExpandFmt()
{
    BOOL bRet = FALSE;
    if( !IsTableMode() && GetDoc()->DontExpandFmt( *GetCrsr()->GetPoint() ))
    {
        bRet = TRUE;
        CallChgLnk();
    }
    return bRet;
}

SvNumberFormatter* SwEditShell::GetNumberFormatter()
{
    return GetDoc()->GetNumberFormatter();
}

BOOL SwEditShell::RemoveInvisibleContent()
{
    StartAllAction();
    BOOL bRet = GetDoc()->RemoveInvisibleContent();
    EndAllAction();
    return bRet;
}
BOOL SwEditShell::ConvertFieldsToText()
{
    StartAllAction();
    BOOL bRet = GetDoc()->ConvertFieldsToText();
    EndAllAction();
    return bRet;
}
void SwEditShell::SetNumberingRestart()
{
    StartAllAction();
    Push();
    //iterate over all text contents - body, frames, header, footer, footnote text
    SwPaM* pCrsr = GetCrsr();
    for(sal_uInt16 i = 0; i < 2; i++)
    {
        if(!i)
            MakeFindRange(DOCPOS_START, DOCPOS_END, pCrsr); //body content
        else
            MakeFindRange(DOCPOS_OTHERSTART, DOCPOS_OTHEREND, pCrsr); //extra content
        SwPosition* pSttPos = pCrsr->Start(), *pEndPos = pCrsr->End();
        ULONG nCurrNd = pSttPos->nNode.GetIndex();
        ULONG nEndNd = pEndPos->nNode.GetIndex();
        if( nCurrNd <= nEndNd )
        {
            SwCntntFrm* pCntFrm;
            sal_Bool bGoOn = sal_True;
            //iterate over all paragraphs
            while( bGoOn )
            {
                SwNode* pNd = GetDoc()->GetNodes()[ nCurrNd ];
                switch( pNd->GetNodeType() )
                {
                case ND_TEXTNODE:
                    if( 0 != ( pCntFrm = ((SwTxtNode*)pNd)->GetFrm()) )
                    {
                        //jump over hidden frames - ignore protection!
                        if( !((SwTxtFrm*)pCntFrm)->IsHiddenNow() )
                        {
                            //if the node is numbered and the starting value of the numbering equals the
                            //start value of the numbering rule then set this value as hard starting value

                            //get the node num
                            // OD 2005-11-09
                            SwTxtNode* pTxtNd( static_cast<SwTxtNode*>(pNd) );
                            SwNumRule* pNumRule( pTxtNd->GetNumRule() );

                            if ( pNumRule && pTxtNd->GetNum() &&
                                 ( pTxtNd->HasNumber() || pTxtNd->HasBullet() ) &&
                                 pTxtNd->IsCountedInList() &&
                                 !pTxtNd->IsListRestart() &&
                                 pTxtNd->GetNum()->GetNumber() ==
                                    pNumRule->Get( static_cast<USHORT>(pTxtNd->GetActualListLevel()) ).GetStart() )
                            {
                                //now set a the start value as attribute
                                SwPosition aCurrentNode(*pNd);
                                GetDoc()->SetNumRuleStart( aCurrentNode, sal_True );
                            }
                        }
                    }
                    break;
                case ND_SECTIONNODE:
                    // jump over hidden sections  - ignore protection!
                    if(((SwSectionNode*)pNd)->GetSection().IsHidden() )
                        nCurrNd = pNd->EndOfSectionIndex();
                    break;
                case ND_ENDNODE:
                    {
                        break;
                    }
                }

                bGoOn = nCurrNd < nEndNd;
                ++nCurrNd;
            }
        }
    }


    Pop(FALSE);
    EndAllAction();
}

USHORT SwEditShell::GetLineCount( BOOL bActPos )
{
    USHORT nRet = 0;
    CalcLayout();
    SwPaM* pPam = GetCrsr();
    SwNodeIndex& rPtIdx = pPam->GetPoint()->nNode;
    SwNodeIndex aStart( rPtIdx );
    SwCntntNode* pCNd;
    SwCntntFrm *pCntFrm = 0;
    ULONG nTmpPos;

    if( !bActPos )
        aStart = 0;
    else if( rPtIdx > ( nTmpPos = GetDoc()->GetNodes().GetEndOfExtras().GetIndex()) )
        // BodyBereich => Start ist EndOfIcons + 1
        aStart = nTmpPos + 1;
    else
    {
        if( 0 != ( pCNd = pPam->GetCntntNode() ) &&
            0 != ( pCntFrm = pCNd->GetFrm() ) )
        {
            const SwStartNode *pTmp;
            if( pCntFrm->IsInFly() )						// Fly
                pTmp = pCNd->FindFlyStartNode();
            else if( pCntFrm->IsInFtn() )					// Footnote
                pTmp = pCNd->FindFootnoteStartNode();
            else
            {												// Footer/Header
                const USHORT nTyp = FRM_HEADER | FRM_FOOTER;
                SwFrm* pFrm = pCntFrm;
                while( pFrm && !(pFrm->GetType() & nTyp) )
                    pFrm = pFrm->GetUpper();
                ASSERT( pFrm, "Wo bin ich?" );
                if( pFrm && ( pFrm->GetType() & FRM_FOOTER ) )
                    pTmp = pCNd->FindFooterStartNode();
                else
                    pTmp = pCNd->FindHeaderStartNode();
            }
            ASSERT( pTmp, "Missing StartNode" );
            aStart  = *pTmp;
        }
        ASSERT( pCNd && pCntFrm, "Missing Layout-Information" );
    }

    while( 0 != ( pCNd = GetDoc()->GetNodes().GoNextSection(
                &aStart, TRUE, FALSE )) && ( !bActPos || aStart <= rPtIdx ) )
    {
        if( 0 != ( pCntFrm = pCNd->GetFrm() ) && pCntFrm->IsTxtFrm() )
        {
            xub_StrLen nActPos = bActPos && aStart == rPtIdx ?
                pPam->GetPoint()->nContent.GetIndex() : USHRT_MAX;
            nRet = nRet + ((SwTxtFrm*)pCntFrm)->GetLineCount( nActPos );
        }
    }
    return nRet;
}

long SwEditShell::CompareDoc( const SwDoc& rDoc )
{
    StartAllAction();
    long nRet = GetDoc()->CompareDoc( rDoc );
    EndAllAction();
    return nRet;
}

long SwEditShell::MergeDoc( const SwDoc& rDoc )
{
    StartAllAction();
    long nRet = GetDoc()->MergeDoc( rDoc );
    EndAllAction();
    return nRet;
}


const SwFtnInfo& SwEditShell::GetFtnInfo() const
{
    return GetDoc()->GetFtnInfo();
}

void SwEditShell::SetFtnInfo(const SwFtnInfo& rInfo)
{
    StartAllAction();
    SET_CURR_SHELL( this );
    GetDoc()->SetFtnInfo(rInfo);
    CallChgLnk();
    EndAllAction();
}

const SwEndNoteInfo& SwEditShell::GetEndNoteInfo() const
{
    return GetDoc()->GetEndNoteInfo();
}

void SwEditShell::SetEndNoteInfo(const SwEndNoteInfo& rInfo)
{
    StartAllAction();
    SET_CURR_SHELL( this );
    GetDoc()->SetEndNoteInfo(rInfo);
    EndAllAction();
}

const SwLineNumberInfo& SwEditShell::GetLineNumberInfo() const
{
    return GetDoc()->GetLineNumberInfo();
}

void SwEditShell::SetLineNumberInfo(const SwLineNumberInfo& rInfo)
{
    StartAllAction();
    SET_CURR_SHELL( this );
    GetDoc()->SetLineNumberInfo(rInfo);
    AddPaintRect( GetLayout()->Frm() );
    EndAllAction();
}

USHORT SwEditShell::GetLinkUpdMode(BOOL bDocSettings) const
{
    return getIDocumentSettingAccess()->getLinkUpdateMode( !bDocSettings );
}

void SwEditShell::SetLinkUpdMode( USHORT nMode )
{
    getIDocumentSettingAccess()->setLinkUpdateMode( nMode );
}


// Schnittstelle fuer die TextInputDaten - ( fuer die Texteingabe
// von japanischen/chinesischen Zeichen)
SwExtTextInput* SwEditShell::CreateExtTextInput(LanguageType eInputLanguage)
{
    SwExtTextInput* pRet = GetDoc()->CreateExtTextInput( *GetCrsr() );
    pRet->SetLanguage(eInputLanguage);
    pRet->SetOverwriteCursor( SwCrsrShell::IsOverwriteCrsr() );
    return pRet;
}

String SwEditShell::DeleteExtTextInput( SwExtTextInput* pDel, BOOL bInsText )
{
    if( !pDel )
    {
        const SwPosition& rPos = *GetCrsr()->GetPoint();
        pDel = GetDoc()->GetExtTextInput( rPos.nNode.GetNode(),
                                          rPos.nContent.GetIndex() );
        if( !pDel )
        {
            //JP 25.10.2001: under UNIX the cursor is moved before the Input-
            //				Engine event comes in. So take any - normally there
            //				exist only one at the time. -- Task 92016
            pDel = GetDoc()->GetExtTextInput();
        }
    }
    String sRet;
    if( pDel )
    {
        rtl::OUString sTmp;
        SwUnoCursorHelper::GetTextFromPam(*pDel, sTmp);
        sRet = sTmp;
        SET_CURR_SHELL( this );
        StartAllAction();
        pDel->SetInsText( bInsText );
        SetOverwriteCrsr( pDel->IsOverwriteCursor() );
        const SwPosition aPos( *pDel->GetPoint() );
        GetDoc()->DeleteExtTextInput( pDel );

        // In this case, the "replace" function did not set the cursor
        // to the original position. Therefore we have to do this manually.
        if ( ! bInsText && IsOverwriteCrsr() )
            *GetCrsr()->GetPoint() = aPos;

        EndAllAction();
    }
    return sRet;
}

void SwEditShell::SetExtTextInputData( const CommandExtTextInputData& rData )
{
    const SwPosition& rPos = *GetCrsr()->GetPoint();
    SwExtTextInput* pInput = GetDoc()->GetExtTextInput( rPos.nNode.GetNode()
                                                /*, rPos.nContent.GetIndex()*/ );
    if( pInput )
    {
        StartAllAction();
        SET_CURR_SHELL( this );

        if( !rData.IsOnlyCursorChanged() )
            pInput->SetInputData( rData );
        // Cursor positionieren:
        const SwPosition& rStt = *pInput->Start();
        xub_StrLen nNewCrsrPos = rStt.nContent.GetIndex() + rData.GetCursorPos();

        // zwar unschoen aber was hilfts
        ShowCrsr();
        long nDiff = nNewCrsrPos - rPos.nContent.GetIndex();
        if( 0 > nDiff )
            Left( (xub_StrLen)-nDiff, CRSR_SKIP_CHARS );
        else if( 0 < nDiff )
            Right( (xub_StrLen)nDiff, CRSR_SKIP_CHARS );

        SetOverwriteCrsr( rData.IsCursorOverwrite() );

        EndAllAction();

        if( !rData.IsCursorVisible() )	// must be called after the EndAction
            HideCrsr();
    }
}

void SwEditShell::TransliterateText( sal_uInt32 nType )
{
    utl::TransliterationWrapper aTrans( ::comphelper::getProcessServiceFactory(), nType );
    StartAllAction();
    SET_CURR_SHELL( this );

    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr )
    {
        GetDoc()->StartUndo(UNDO_EMPTY, NULL);
        FOREACHPAM_START( this )

        if( PCURCRSR->HasMark() )
            GetDoc()->TransliterateText( *PCURCRSR, aTrans );

        FOREACHPAM_END()
        GetDoc()->EndUndo(UNDO_EMPTY, NULL);
    }
    else
        GetDoc()->TransliterateText( *pCrsr, aTrans );

    EndAllAction();
}

void SwEditShell::TransliterateText( const String& rModuleName )
{
    utl::TransliterationWrapper aTrans(::comphelper::getProcessServiceFactory(), 0 );
    aTrans.loadModuleByImplName( rModuleName, LANGUAGE_SYSTEM );
    StartAllAction();
    SET_CURR_SHELL( this );

    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr )
    {
        GetDoc()->StartUndo(UNDO_EMPTY, NULL);
        FOREACHPAM_START( this )

        if( PCURCRSR->HasMark() )
            GetDoc()->TransliterateText( *PCURCRSR, aTrans );

        FOREACHPAM_END()
        GetDoc()->EndUndo(UNDO_EMPTY, NULL);
    }
    else
        GetDoc()->TransliterateText( *pCrsr, aTrans );

    EndAllAction();
}


void SwEditShell::CountWords( SwDocStat& rStat ) const
{
    FOREACHPAM_START( this )

        if( PCURCRSR->HasMark() )
            GetDoc()->CountWords( *PCURCRSR, rStat );

    FOREACHPAM_END()
}

