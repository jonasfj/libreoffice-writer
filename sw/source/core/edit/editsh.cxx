/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: editsh.cxx,v $
 *
 *  $Revision: 1.46 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 16:26:54 $
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

#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _VCL_CMDEVT_HXX //autogen
#include <vcl/cmdevt.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif

#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _TXTINET_HXX //autogen
#include <txtinet.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _FRAME_HXX
#include <frame.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>			// fuer SwTxtNode
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx> 			// UNDO_START, UNDO_END
#endif
#ifndef _CALC_HXX
#include <calc.hxx>
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _EXTINPUT_HXX
#include <extinput.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _SCRIPTINFO_HXX
#include <scriptinfo.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _UNOCHART_HXX
#include <unochart.hxx>
#endif

using namespace com::sun::star;

SV_IMPL_PTRARR(SwGetINetAttrs, SwGetINetAttr*)

/******************************************************************************
 *						void SwEditShell::Insert(char c)
 ******************************************************************************/


void SwEditShell::Insert( sal_Unicode c, BOOL bOnlyCurrCrsr )
{
    StartAllAction();
    FOREACHPAM_START(this)

        if( !GetDoc()->Insert(*PCURCRSR, c) )
            ASSERT( FALSE, "Doc->Insert(c) failed." );

        SaveTblBoxCntnt( PCURCRSR->GetPoint() );
        if( bOnlyCurrCrsr )
            break;

    FOREACHPAM_END()

    EndAllAction();
}


/******************************************************************************
 *				  void SwEditShell::Insert(const String &rStr)
 ******************************************************************************/


void SwEditShell::Insert(const String &rStr)
{
    StartAllAction();
    FOREACHPAM_START(this)
        //OPT: GetSystemCharSet
        if( !GetDoc()->Insert( *PCURCRSR, rStr, true ) )
            ASSERT( FALSE, "Doc->Insert(Str) failed." );

        SaveTblBoxCntnt( PCURCRSR->GetPoint() );

    FOREACHPAM_END()

    // calculate cursor bidi level
    SwCursor* pTmpCrsr = _GetCrsr();
    const BOOL bDoNotSetBidiLevel = ! pTmpCrsr ||
                                    ( 0 != (SwUnoCrsr*)*pTmpCrsr );

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
            ASSERT( FALSE, "Doc->Overwrite(Str) failed." );
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
    GetDoc()->StartUndo(0, NULL);

    FOREACHPAM_START(this)
        // eine Tabellen Zelle wird jetzt zu einer normalen Textzelle!
        GetDoc()->ClearBoxNumAttrs( PCURCRSR->GetPoint()->nNode );
        GetDoc()->SplitNode( *PCURCRSR->GetPoint(), bCheckTableStart );
    FOREACHPAM_END()

    GetDoc()->EndUndo(0, NULL);

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
    GetDoc()->StartUndo(0, NULL);

    FOREACHPAM_START(this)
        GetDoc()->ClearBoxNumAttrs( PCURCRSR->GetPoint()->nNode );
        bRet |= GetDoc()->AppendTxtNode( *PCURCRSR->GetPoint());
    FOREACHPAM_END()

    GetDoc()->EndUndo(0, NULL);

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
        if( pGrf->IsSwapOut() ||
            ( pGrfNode->IsLinkedFile() && GRAPHIC_DEFAULT == pGrf->GetType() ) )
        {
#ifndef PRODUCT
            ASSERT( pGrfNode->SwapIn( bWait ) || !bWait, "Grafik konnte nicht geladen werden" );
#else
            pGrfNode->SwapIn( bWait );
#endif
        }
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
    return pGrfNode ? pGrfNode->GetGrfObj().GetType() : GRAPHIC_NONE;
}

/******************************************************************************
 *		liefert die Groesse der Graphic, wenn CurCrsr->GetPoint() auf
 * 			einen SwGrfNode zeigt (und GetMark nicht gesetzt ist
 * 					oder auf die gleiche Graphic zeigt)
 ******************************************************************************/

BOOL SwEditShell::GetGrfSize(Size& rSz) const
{
    SwNoTxtNode* pNoTxtNd;
    SwPaM* pCurCrsr = GetCrsr();
    if( ( !pCurCrsr->HasMark()
         || pCurCrsr->GetPoint()->nNode == pCurCrsr->GetMark()->nNode )
         && 0 != ( pNoTxtNd = pCurCrsr->GetNode()->GetNoTxtNode() ) )
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
const String& SwEditShell::GetAlternateText() const
{
    SwPaM* pCrsr = GetCrsr();
    const SwNoTxtNode* pNd;
    if( !pCrsr->HasMark() && 0 != ( pNd = pCrsr->GetNode()->GetNoTxtNode()) )
        return pNd->GetAlternateText();

    return aEmptyStr;
}


void SwEditShell::SetAlternateText( const String& rTxt )
{
    SwPaM* pCrsr = GetCrsr();
    SwNoTxtNode* pNd;
    if( !pCrsr->HasMark() && 0 != ( pNd = pCrsr->GetNode()->GetNoTxtNode()) )
        pNd->SetAlternateText( rTxt, sal_True );
}


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

const String& SwEditShell::GetChartName( const uno::Reference < embed::XEmbeddedObject >& xObj )
{
    if( xObj.is() )
    {
        SwClientIter aIter( *(SwModify*)GetDoc()->GetDfltGrfFmtColl() );
        SwClient *pCli;
        if( 0 != (pCli = aIter.First( TYPE(SwCntntNode) )) )
            do{
                if( ((SwCntntNode*)pCli)->IsOLENode() &&
                    xObj == ((SwOLENode*)pCli)->GetOLEObj().GetOleRef() )
                {
                    return ((SwOLENode*)pCli)->GetChartTblName();
                }
            } while ( 0 != (pCli = aIter.Next()) );
    }
    else
    {
        SwOLENode *pONd = GetCrsr()->GetNode()->GetOLENode();
        if( pONd )
            return pONd->GetChartTblName();
    }

    return aEmptyStr;
}

void SwEditShell::UpdateChartData( const String &rName )
{
    //Fuer das Update brauchen wir die SwTable. Also muessen wir ggf. die
    //gewuenschte Table anspringen.
    const SwTableNode *pTblNd = IsCrsrInTbl();
    if( !pTblNd || rName != pTblNd->GetTable().GetFrmFmt()->GetName() )
    {
        Push();
        GotoTable( rName );
        pTblNd = IsCrsrInTbl();
        Pop( FALSE );
    }

    if( pTblNd )
        pTblNd->GetTable().UpdateCharts();
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

#pragma optimize("",on)

// erfragen des akt. Wortes

String SwEditShell::GetCurWord()
{
    const SwPaM& rPaM = *GetCrsr();
    const SwTxtNode* pNd = rPaM.GetNode()->GetTxtNode();
    String aString = pNd ?
                     pNd->GetCurWord(rPaM.GetPoint()->nContent.GetIndex()) :
                     aEmptyStr;
    aString.EraseAllChars('\xff');
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


const SfxDocumentInfo* SwEditShell::GetInfo() const
{
    return GetDoc()->GetInfo();
}


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
    String aTxt;
    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetPoint()->nNode.GetIndex() ==
        pCrsr->GetMark()->nNode.GetIndex() )
    {
        SwTxtNode* pTxtNd = pCrsr->GetNode()->GetTxtNode();
        if( pTxtNd )
        {
            xub_StrLen nDropLen = pTxtNd->GetDropLen( nChars );
            if( nDropLen )
                aTxt = pTxtNd->GetTxt().Copy( 0, nDropLen );
        }
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
                    xub_StrLen nStt = nPos-1;
                    while(  nPos < aStr.Len() &&
                            0 != ( ch = aStr.GetChar( nPos++ )) &&
                           (rCC.isLetterNumeric( aStr, nPos - 1 ) ||
                               ch == '_'|| ch == '.' ))
                        ;

                    if( nPos < aStr.Len() )
                        --nPos;

                    String sVar( aStr.Copy( nStt, nPos - nStt ));
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


Graphic SwEditShell::GetIMapGraphic( BOOL bWait ) const
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
#ifndef PRODUCT
                ASSERT( ((SwGrfNode*)pNd)->SwapIn( bWait )||!bWait, "Grafik konnte nicht geladen werden" );
#else
                ((SwGrfNode*)pNd)->SwapIn( bWait );
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
    GetDoc()->StartUndo( UIUNDO_INSERT_URLTXT, NULL);
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
            Insert( rStr );
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
    GetDoc()->EndUndo( UIUNDO_INSERT_URLTXT, NULL );
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
    BOOL bRet = SelectTxtAttr( RES_TXTATR_INETFMT );
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
                                 pTxtNd->IsCounted() &&
                                 !pTxtNd->IsRestart() &&
                                 pTxtNd->GetNum()->GetNumber() == pNumRule->Get( pTxtNd->GetLevel() ).GetStart() )
//                            const SwNodeNum* pNodeNum = ((SwTxtNode*)pNd)->_GetNodeNum();
//                            SwNumRule *pNumRule = ((SwTxtNode*)pNd)->GetNumRule();

//                            if(pNodeNum && pNumRule &&
//                                    pNodeNum->IsNum() && pNodeNum->IsShowNum() && !pNodeNum->IsStart() &&
//                                    *pNodeNum->GetLevelVal() == pNumRule->Get(pNodeNum->GetLevel()).GetStart())
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
    SwCntntFrm *pCntFrm;
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
            nRet += ((SwTxtFrm*)pCntFrm)->GetLineCount( nActPos );
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
        SwXTextCursor::getTextFromPam(*pDel, sTmp);
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
    utl::TransliterationWrapper aTrans(
                        ::comphelper::getProcessServiceFactory(), nType );
    StartAllAction();
    SET_CURR_SHELL( this );

    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr )
    {
        GetDoc()->StartUndo(0, NULL);
        FOREACHPAM_START( this )

        if( PCURCRSR->HasMark() )
            GetDoc()->TransliterateText( *PCURCRSR, aTrans );

        FOREACHPAM_END()
        GetDoc()->EndUndo(0, NULL);
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

