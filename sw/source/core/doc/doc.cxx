/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: doc.cxx,v $
 *
 *  $Revision: 1.39 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-19 12:34:26 $
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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#include <tools/shl.hxx>
#include <tools/globname.hxx>

#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HDL
#include <com/sun/star/i18n/WordType.hdl>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SFXDOCINF_HXX //autogen
#include <sfx2/docinf.hxx>
#endif
#ifndef _SVX_KEEPITEM_HXX //autogen
#include <svx/keepitem.hxx>
#endif
#ifndef _SVX_CSCOITEM_HXX //autogen
#include <svx/cscoitem.hxx>
#endif
#ifndef _SVX_BRKITEM_HXX //autogen
#include <svx/brkitem.hxx>
#endif
#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif

#ifndef _SWMODULE_HXX //autogen
#include <swmodule.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTRFMRK_HXX //autogen
#include <fmtrfmrk.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _DBFLD_HXX
#include <dbfld.hxx>
#endif
#ifndef _TXTINET_HXX //autogen
#include <txtinet.hxx>
#endif
#ifndef _TXTRFMRK_HXX //autogen
#include <txtrfmrk.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _LINKENUM_HXX
#include <linkenum.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer die UndoIds
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx> //DTor
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _ROLBCK_HXX
#include <rolbck.hxx>			// Undo-Attr
#endif
#ifndef _BOOKMRK_HXX
#include <bookmrk.hxx>			// fuer die Bookmarks
#endif
#ifndef _DOCTXM_HXX
#include <doctxm.hxx>			// fuer die Verzeichnisse
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>			// PoolVorlagen-Id's
#endif
#ifndef _MVSAVE_HXX
#include <mvsave.hxx>			// fuer Server-Funktionalitaet
#endif
#ifndef _WRONG_HXX
#include <wrong.hxx>			// fuer OnlineSpell-Invalidierung
#endif
#ifndef _SCRIPTINFO_HXX
#include <scriptinfo.hxx>
#endif
#ifndef _ACORRECT_HXX
#include <acorrect.hxx>			// Autokorrektur
#endif
#ifndef _SECTION_HXX
#include <section.hxx>			//
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>	   		// Statusanzeige
#endif
#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _FLDUPDE_HXX
#include <fldupde.hxx>
#endif
#ifndef _SWBASLNK_HXX
#include <swbaslnk.hxx>
#endif

#ifndef _STATSTR_HRC
#include <statstr.hrc>      	// StatLine-String
#endif

#include <vector>
// -> #111827#
#ifndef _SFXITEMITER_HXX
#include <svtools/itemiter.hxx>
#endif

#include <comcore.hrc>
#include <SwUndoTOXChange.hxx>
// <- #111827#

#include <SwUndoFmt.hxx>
// --> OD 2005-08-29 #TESTING#
#ifndef _LAYOUTER_HXX
#include <layouter.hxx>
#endif
// <--

// Seiten-Deskriptoren
SV_IMPL_PTRARR(SwPageDescs,SwPageDescPtr);
// Verzeichnisse
SV_IMPL_PTRARR( SwTOXTypes, SwTOXTypePtr )
// FeldTypen
SV_IMPL_PTRARR( SwFldTypes, SwFldTypePtr)


/*
 * Dokumenteditieren (Doc-SS) zum Fuellen des Dokuments
 * durch den RTF Parser und fuer die EditShell.
 */
void SwDoc::ChgDBData(const SwDBData& rNewData)
{
    if( rNewData != aDBData )
    {
        aDBData = rNewData;
        SetModified();
    }
    GetSysFldType(RES_DBNAMEFLD)->UpdateFlds();
}

BOOL SwDoc::SplitNode( const SwPosition &rPos, BOOL bChkTableStart )
{
    SwCntntNode *pNode = rPos.nNode.GetNode().GetCntntNode();
    if(0 == pNode)
        return FALSE;

    {
        // Bug 26675:	DataChanged vorm loeschen verschicken, dann bekommt
        //			man noch mit, welche Objecte sich im Bereich befinden.
        //			Danach koennen sie vor/hinter der Position befinden.
        SwDataChanged aTmp( this, rPos, 0 );
    }

    SwUndoSplitNode* pUndo = 0;
    if ( DoesUndo() )
    {
        ClearRedo();
        // einfuegen vom Undo-Object, z.Z. nur beim TextNode
        if( pNode->IsTxtNode() )
            AppendUndo( pUndo = new SwUndoSplitNode( this, rPos, bChkTableStart  ));
    }

    //JP 28.01.97: Sonderfall fuer SplitNode am Tabellenanfang:
    //				steht die am Doc/Fly/Footer/..-Anfang oder direkt
    //				hinter einer Tabelle, dann fuege davor
    //				einen Absatz ein
    if( bChkTableStart && !rPos.nContent.GetIndex() && pNode->IsTxtNode() )
    {
        ULONG nPrevPos = rPos.nNode.GetIndex() - 1;
        const SwTableNode* pTblNd;
        const SwNode* pNd = GetNodes()[ nPrevPos ];
        if( pNd->IsStartNode() &&
            SwTableBoxStartNode == ((SwStartNode*)pNd)->GetStartNodeType() &&
            0 != ( pTblNd = GetNodes()[ --nPrevPos ]->GetTableNode() ) &&
            ((( pNd = GetNodes()[ --nPrevPos ])->IsStartNode() &&
               SwTableBoxStartNode != ((SwStartNode*)pNd)->GetStartNodeType() )
               || ( pNd->IsEndNode() && pNd->FindStartNode()->IsTableNode() )
               || pNd->IsCntntNode() ))
        {
            if( pNd->IsCntntNode() )
            {
                //JP 30.04.99 Bug 65660:
                // ausserhalb des normalen BodyBereiches gibt es keine
                // Seitenumbrueche, also ist das hier kein gueltige
                // Bedingung fuers einfuegen eines Absatzes
                if( nPrevPos < GetNodes().GetEndOfExtras().GetIndex() )
                    pNd = 0;
                else
                {
                    // Dann nur, wenn die Tabelle Umbrueche traegt!
                    const SwFrmFmt* pFrmFmt = pTblNd->GetTable().GetFrmFmt();
                    if( SFX_ITEM_SET != pFrmFmt->GetItemState(RES_PAGEDESC, FALSE) &&
                        SFX_ITEM_SET != pFrmFmt->GetItemState( RES_BREAK, FALSE ) )
                        pNd = 0;
                }
            }

            if( pNd )
            {
                SwTxtNode* pTxtNd = GetNodes().MakeTxtNode(
                                        SwNodeIndex( *pTblNd ),
                                        GetTxtCollFromPool( RES_POOLCOLL_TEXT ));
                if( pTxtNd )
                {
                    ((SwPosition&)rPos).nNode = pTblNd->GetIndex()-1;
                    ((SwPosition&)rPos).nContent.Assign( pTxtNd, 0 );

                    // nur im BodyBereich den SeitenUmbruch/-Vorlage umhaengem
                    if( nPrevPos > GetNodes().GetEndOfExtras().GetIndex() )
                    {
                        SwFrmFmt* pFrmFmt = pTblNd->GetTable().GetFrmFmt();
                        const SfxPoolItem *pItem;
                        if( SFX_ITEM_SET == pFrmFmt->GetItemState( RES_PAGEDESC,
                            FALSE, &pItem ) )
                        {
                            pTxtNd->SwCntntNode::SetAttr( *pItem );
                            pFrmFmt->ResetAttr( RES_PAGEDESC );
                        }
                        if( SFX_ITEM_SET == pFrmFmt->GetItemState( RES_BREAK,
                            FALSE, &pItem ) )
                        {
                            pTxtNd->SwCntntNode::SetAttr( *pItem );
                            pFrmFmt->ResetAttr( RES_BREAK );
                        }
                    }

                    if( pUndo )
                        pUndo->SetTblFlag();
                    SetModified();
                    return TRUE;
                }
            }
        }
    }

    SvULongs aBkmkArr( 15, 15 );
    _SaveCntntIdx( this, rPos.nNode.GetIndex(), rPos.nContent.GetIndex(),
                    aBkmkArr, SAVEFLY_SPLIT );
    if( 0 != ( pNode = pNode->SplitNode( rPos ) ))
    {
        // verschiebe noch alle Bookmarks/TOXMarks/FlyAtCnt
        if( aBkmkArr.Count() )
            _RestoreCntntIdx( this, aBkmkArr, rPos.nNode.GetIndex()-1, 0, TRUE );

        if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
        {
            SwPaM aPam( rPos );
            aPam.SetMark();
            aPam.Move( fnMoveBackward );
            if( IsRedlineOn() )
                AppendRedline( new SwRedline( REDLINE_INSERT, aPam ));
            else
                SplitRedline( aPam );
        }
    }

    SetModified();
    return TRUE;
}

BOOL SwDoc::AppendTxtNode( SwPosition& rPos )
{
    /*
     * Neuen Node vor EndOfContent erzeugen.
     */
    SwTxtNode *pCurNode = GetNodes()[ rPos.nNode ]->GetTxtNode();
    if( !pCurNode )
    {
        // dann kann ja einer angelegt werden!
        SwNodeIndex aIdx( rPos.nNode, 1 );
        pCurNode = GetNodes().MakeTxtNode( aIdx,
                        GetTxtCollFromPool( RES_POOLCOLL_STANDARD ));
    }
    else
        pCurNode = (SwTxtNode*)pCurNode->AppendNode( rPos );

    rPos.nNode++;
    rPos.nContent.Assign( pCurNode, 0 );

    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoInsert( rPos.nNode ));
    }

    if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
    {
        SwPaM aPam( rPos );
        aPam.SetMark();
        aPam.Move( fnMoveBackward );
        if( IsRedlineOn() )
            AppendRedline( new SwRedline( REDLINE_INSERT, aPam ));
        else
            SplitRedline( aPam );
    }

    SetModified();
    return TRUE;
}

BOOL SwDoc::Insert( const SwPaM &rRg, const String &rStr, BOOL bHintExpand )
{
    if( DoesUndo() )
        ClearRedo();

    const SwPosition* pPos = rRg.GetPoint();

    if( pACEWord )					// Aufnahme in die Autokorrektur
    {
        if( 1 == rStr.Len() && pACEWord->IsDeleted() )
            pACEWord->CheckChar( *pPos, rStr.GetChar( 0 ) );
        delete pACEWord, pACEWord = 0;
    }

    SwTxtNode *pNode = pPos->nNode.GetNode().GetTxtNode();
    if(!pNode)
        return FALSE;

    const USHORT nInsMode = bHintExpand ? INS_EMPTYEXPAND
                                     : INS_NOHINTEXPAND;
    SwDataChanged aTmp( rRg, 0 );

    if( !DoesUndo() || !DoesGroupUndo() )
    {
        pNode->Insert( rStr, pPos->nContent, nInsMode );

        if( DoesUndo() )
            AppendUndo( new SwUndoInsert( pPos->nNode,
                                    pPos->nContent.GetIndex(), rStr.Len() ));
    }
    else
    {			// ist Undo und Gruppierung eingeschaltet, ist alles anders !
        USHORT nUndoSize = pUndos->Count();
        xub_StrLen nInsPos = pPos->nContent.GetIndex();
        SwUndoInsert * pUndo = NULL; // #111827#
        CharClass& rCC = GetAppCharClass();

        // -> #111827#
        bool bNewUndo = false;
        if( 0 == nUndoSize)
            bNewUndo = true;
        else
        {
            pUndo = (SwUndoInsert*)(*pUndos)[ --nUndoSize ];

            switch (pUndo->GetId())
            {
            case UNDO_INSERT:
            case UNDO_TYPING:
                bNewUndo = !pUndo->CanGrouping( *pPos );

                break;

            default:
                bNewUndo = true;
            }
        }
        // <- #111827#

        if (bNewUndo)
        {
            pUndo = new SwUndoInsert( pPos->nNode, nInsPos, 0,
                            !rCC.isLetterNumeric( rStr, 0 ) );
            AppendUndo( pUndo );
        }

        pNode->Insert( rStr, pPos->nContent, nInsMode );

        for( xub_StrLen i = 0; i < rStr.Len(); ++i )
        {
            nInsPos++;
            // wenn CanGrouping() TRUE returnt, ist schon alles erledigt
            if( !pUndo->CanGrouping( rStr.GetChar( i ) ))
            {
                pUndo = new SwUndoInsert( pPos->nNode, nInsPos,  1,
                            !rCC.isLetterNumeric( rStr, i ) );
                AppendUndo( pUndo );
            }
        }
    }

    if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
    {
        SwPaM aPam( pPos->nNode, aTmp.GetCntnt(),
                    pPos->nNode, pPos->nContent.GetIndex());
        if( IsRedlineOn() )
            AppendRedline( new SwRedline( REDLINE_INSERT, aPam ));
        else
            SplitRedline( aPam );
    }

    SetModified();
    return TRUE;
}

SwFlyFrmFmt* SwDoc::_InsNoTxtNode( const SwPosition& rPos, SwNoTxtNode* pNode,
                                    const SfxItemSet* pFlyAttrSet,
                                    const SfxItemSet* pGrfAttrSet,
                                    SwFrmFmt* pFrmFmt)
{
    SwFlyFrmFmt *pFmt = 0;
    if( pNode )
    {
        pFmt = _MakeFlySection( rPos, *pNode, FLY_AT_CNTNT,
                                pFlyAttrSet, pFrmFmt );
        if( pGrfAttrSet )
            pNode->SetAttr( *pGrfAttrSet );
    }
    return pFmt;
}

SwFlyFrmFmt* SwDoc::Insert( const SwPaM &rRg,
                            const String& rGrfName,
                            const String& rFltName,
                            const Graphic* pGraphic,
                            const SfxItemSet* pFlyAttrSet,
                            const SfxItemSet* pGrfAttrSet,
                            SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_GRAPHIC );
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeGrfNode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            rGrfName, rFltName, pGraphic,
                            pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet, pFrmFmt );
}
SwFlyFrmFmt* SwDoc::Insert( const SwPaM &rRg, const GraphicObject& rGrfObj,
                            const SfxItemSet* pFlyAttrSet,
                            const SfxItemSet* pGrfAttrSet,
                            SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_GRAPHIC );
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeGrfNode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            rGrfObj, pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet, pFrmFmt );
}

SwFlyFrmFmt* SwDoc::Insert(const SwPaM &rRg, const svt::EmbeddedObjectRef& xObj,
                        const SfxItemSet* pFlyAttrSet,
                        const SfxItemSet* pGrfAttrSet,
                        SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
    {
        USHORT nId = RES_POOLFRM_OLE;
        SvGlobalName aClassName( xObj->getClassID() );
        if (SotExchange::IsMath(aClassName))
            nId = RES_POOLFRM_FORMEL;

        pFrmFmt = GetFrmFmtFromPool( nId );
    }
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeOLENode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            xObj,
                            pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet,
                            pFrmFmt );
}

SwFlyFrmFmt* SwDoc::InsertOLE(const SwPaM &rRg, const String& rObjName,
                        const SfxItemSet* pFlyAttrSet,
                        const SfxItemSet* pGrfAttrSet,
                        SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_OLE );

    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeOLENode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            rObjName,
                            pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet,
                            pFrmFmt );
}


String SwDoc::GetCurWord( SwPaM& rPaM )
{
    SwTxtNode *pNd = rPaM.GetNode()->GetTxtNode();
    if( pNd )
        return pNd->GetCurWord(rPaM.GetPoint()->nContent.GetIndex());
    return aEmptyStr;
}


/*************************************************************************
|*				  SwDoc::GetFldType()
|*	  Beschreibung: liefert den am Doc eingerichteten Feldtypen zurueck
*************************************************************************/

SwFieldType *SwDoc::GetSysFldType( const USHORT eWhich ) const
{
    for( register int i = 0; i < INIT_FLDTYPES; i++ )
        if( eWhich == (*pFldTypes)[i]->Which() )
            return (*pFldTypes)[i];
    return 0;
}

//----- Macro ---------------------------------------------------------

void SwDoc::SetGlobalMacro( USHORT nEvent, const SvxMacro& rMacro )
{
    SvxMacro *pMacro;
    SetModified();
    if ( 0 != (pMacro=pMacroTable->Get(nEvent)) )
    {
        delete pMacro;
        pMacroTable->Replace(nEvent, new SvxMacro(rMacro));
        return;
    }
    pMacroTable->Insert(nEvent, new SvxMacro(rMacro));
}

/*************************************************************************
 *			   void SetDocStat( const SwDocStat& rStat );
 *************************************************************************/

void SwDoc::SetDocStat( const SwDocStat& rStat )
{
    *pDocStat = rStat;
}


sal_uInt16 SwDoc::GetPageCount() const
{
    return GetRootFrm() ? GetRootFrm()->GetPageNum() : 0;
}

const Size SwDoc::GetPageSize( sal_uInt16 nPageNum, bool bSkipEmptyPages ) const
{
    Size aSize;
    if( GetRootFrm() && nPageNum )
    {
        const SwPageFrm* pPage = static_cast<const SwPageFrm*>
                                 (GetRootFrm()->Lower());

        while( --nPageNum && pPage->GetNext() )
            pPage = static_cast<const SwPageFrm*>( pPage->GetNext() );

        if( !bSkipEmptyPages && pPage->IsEmptyPage() && pPage->GetNext() )
            pPage = static_cast<const SwPageFrm*>( pPage->GetNext() );

        aSize = pPage->Frm().SSize();
    }
    return aSize;
}


/*************************************************************************
 *			  void UpdateDocStat( const SwDocStat& rStat );
 *************************************************************************/

void SwDoc::UpdateDocStat( SwDocStat& rStat )
{
    if( rStat.bModified )
    {
        rStat.Reset();
        rStat.nPara = 0;        // Default ist auf 1 !!
        SwNode* pNd;

        for( ULONG n = GetNodes().Count(); n; )
            switch( ( pNd = GetNodes()[ --n ])->GetNodeType() )
            {
            case ND_TEXTNODE:
                ((SwTxtNode*)pNd)->CountWords( rStat, 0, ((SwTxtNode*)pNd)->GetTxt().Len() );
                break;
            case ND_TABLENODE:		++rStat.nTbl;	break;
            case ND_GRFNODE:		++rStat.nGrf;	break;
            case ND_OLENODE:		++rStat.nOLE;	break;
            case ND_SECTIONNODE:	break;
            }

        rStat.nPage 	= GetRootFrm() ? GetRootFrm()->GetPageNum() : 0;
        rStat.bModified = FALSE;
        SetDocStat( rStat );
        // event. Stat. Felder Updaten
        SwFieldType *pType = GetSysFldType(RES_DOCSTATFLD);
        pType->UpdateFlds();
    }
}


// Dokument - Info

void SwDoc::DocInfoChgd( const SfxDocumentInfo& rInfo )
{
    delete pSwgInfo;
    pSwgInfo = new SfxDocumentInfo(rInfo);

    GetSysFldType( RES_DOCINFOFLD )->UpdateFlds();
    GetSysFldType( RES_TEMPLNAMEFLD )->UpdateFlds();
    SetModified();
}

    // returne zum Namen die im Doc gesetzte Referenz
const SwFmtRefMark* SwDoc::GetRefMark( const String& rName ) const
{
    const SfxPoolItem* pItem;
    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    for( USHORT n = 0; n < nMaxItems; ++n )
    {
        if( 0 == (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n ) ))
            continue;

        const SwFmtRefMark* pFmtRef = (SwFmtRefMark*)pItem;
        const SwTxtRefMark* pTxtRef = pFmtRef->GetTxtRefMark();
        if( pTxtRef && &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() &&
            rName.Equals( pFmtRef->GetRefName() ) )
            return pFmtRef;
    }
    return 0;
}

    // returne die RefMark per Index - fuer Uno
const SwFmtRefMark* SwDoc::GetRefMark( USHORT nIndex ) const
{
    const SfxPoolItem* pItem;
    const SwTxtRefMark* pTxtRef;
    const SwFmtRefMark* pRet = 0;

    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    USHORT nCount = 0;
    for( USHORT n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n )) &&
            0 != (pTxtRef = ((SwFmtRefMark*)pItem)->GetTxtRefMark()) &&
            &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() )
        {
            if(nCount == nIndex)
            {
                pRet = (SwFmtRefMark*)pItem;
                break;
            }
            nCount++;
        }
   return pRet;
}

    // returne die Namen aller im Doc gesetzten Referenzen
    //JP 24.06.96: Ist der ArrayPointer 0 dann returne nur, ob im Doc. eine
    //				RefMark gesetzt ist
    // OS 25.06.96: ab jetzt wird immer die Anzahl der Referenzen returnt
USHORT SwDoc::GetRefMarks( SvStringsDtor* pNames ) const
{
    const SfxPoolItem* pItem;
    const SwTxtRefMark* pTxtRef;

    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    USHORT nCount = 0;
    for( USHORT n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n )) &&
            0 != (pTxtRef = ((SwFmtRefMark*)pItem)->GetTxtRefMark()) &&
            &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() )
        {
            if( pNames )
            {
                String* pTmp = new String( ((SwFmtRefMark*)pItem)->GetRefName() );
                pNames->Insert( pTmp, nCount );
            }
            nCount ++;
        }

    return nCount;
}

void SwDoc::SetModified()
{
    // --> OD 2005-08-29 #125370#
    SwLayouter::ClearMovedFwdFrms( *this );
    SwLayouter::ClearObjsTmpConsiderWrapInfluence( *this );
    SwLayouter::ClearFrmsNotToWrap( *this );
    // <--
    // dem Link wird der Status returnt, wie die Flags waren und werden
    // 	Bit 0:	-> alter Zustand
    //	Bit 1: 	-> neuer Zustand
    long nCall = bModified ? 3 : 2;
    bModified = TRUE;
    pDocStat->bModified = TRUE;
    if( aOle2Link.IsSet() )
    {
        bInCallModified = TRUE;
        aOle2Link.Call( (void*)nCall );
        bInCallModified = FALSE;
    }

    if( pACEWord && !pACEWord->IsDeleted() )
        delete pACEWord, pACEWord = 0;
}

void SwDoc::ResetModified()
{
    // dem Link wird der Status returnt, wie die Flags waren und werden
    // 	Bit 0:	-> alter Zustand
    //	Bit 1: 	-> neuer Zustand
    long nCall = bModified ? 1 : 0;
    bModified = FALSE;
    nUndoSavePos = nUndoPos;
    if( nCall && aOle2Link.IsSet() )
    {
        bInCallModified = TRUE;
        aOle2Link.Call( (void*)nCall );
        bInCallModified = FALSE;
    }
}


void SwDoc::ReRead( SwPaM& rPam, const String& rGrfName,
                    const String& rFltName, const Graphic* pGraphic,
                    const GraphicObject* pGrafObj )
{
    SwGrfNode *pGrfNd;
    if( ( !rPam.HasMark()
         || rPam.GetPoint()->nNode.GetIndex() == rPam.GetMark()->nNode.GetIndex() )
         && 0 != ( pGrfNd = rPam.GetPoint()->nNode.GetNode().GetGrfNode() ) )
    {
        if( DoesUndo() )
        {
            ClearRedo();
            AppendUndo( new SwUndoReRead( rPam, *pGrfNd ) );
        }

        // Weil nicht bekannt ist, ob sich die Grafik spiegeln laesst,
        // immer das SpiegelungsAttribut zuruecksetzen
        if( RES_DONT_MIRROR_GRF != pGrfNd->GetSwAttrSet().
                                                GetMirrorGrf().GetValue() )
            pGrfNd->SetAttr( SwMirrorGrf() );

        pGrfNd->ReRead( rGrfName, rFltName, pGraphic, pGrafObj, TRUE );
        SetModified();
    }
}

BOOL lcl_SpellAgain( const SwNodePtr& rpNd, void* pArgs )
{
    SwTxtNode *pTxtNode = (SwTxtNode*)rpNd->GetTxtNode();
    BOOL bOnlyWrong = *(BOOL*)pArgs;
    if( pTxtNode )
    {
        if( bOnlyWrong )
        {
            if( pTxtNode->GetWrong() &&
                pTxtNode->GetWrong()->InvalidateWrong() )
                pTxtNode->SetWrongDirty( TRUE );
        }
        else
        {
            pTxtNode->SetWrongDirty( TRUE );
            if( pTxtNode->GetWrong() )
                pTxtNode->GetWrong()->SetInvalid( 0, STRING_LEN );
        }
    }
    return TRUE;
}

/*************************************************************************
 * 		SwDoc::SpellItAgainSam( BOOL bInvalid, BOOL bOnlyWrong )
 *
 * stoesst das Spelling im Idle-Handler wieder an.
 * Wird bInvalid als TRUE uebergeben, so werden zusaetzlich die WrongListen
 * an allen Nodes invalidiert und auf allen Seiten das SpellInvalid-Flag
 * gesetzt.
 * Mit bOnlyWrong kann man dann steuern, ob nur die Bereiche mit falschen
 * Woertern oder die kompletten Bereiche neu ueberprueft werden muessen.
 ************************************************************************/

void SwDoc::SpellItAgainSam( BOOL bInvalid, BOOL bOnlyWrong )
{
    ASSERT( GetRootFrm(), "SpellAgain: Where's my RootFrm?" );
    if( bInvalid )
    {
        SwPageFrm *pPage = (SwPageFrm*)GetRootFrm()->Lower();
        while ( pPage )
        {
            pPage->InvalidateSpelling();
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        GetNodes().ForEach( lcl_SpellAgain, &bOnlyWrong );
    }
    GetRootFrm()->SetIdleFlags();
}

void SwDoc::InvalidateAutoCompleteFlag()
{
    if( GetRootFrm() )
    {
        SwPageFrm *pPage = (SwPageFrm*)GetRootFrm()->Lower();
        while ( pPage )
        {
            pPage->InvalidateAutoCompleteWords();
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        for( ULONG nNd = 1, nCnt = GetNodes().Count(); nNd < nCnt; ++nNd )
            GetNodes()[ nNd ]->SetAutoCompleteWordDirty( TRUE );
        GetRootFrm()->SetIdleFlags();
    }
}

const SwFmtINetFmt* SwDoc::FindINetAttr( const String& rName ) const
{
    const SwFmtINetFmt* pItem;
    const SwTxtINetFmt* pTxtAttr;
    const SwTxtNode* pTxtNd;
    USHORT n, nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
    for( n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = (SwFmtINetFmt*)GetAttrPool().GetItem(
            RES_TXTATR_INETFMT, n ) ) &&
            pItem->GetName().Equals( rName ) &&
            0 != ( pTxtAttr = pItem->GetTxtINetFmt()) &&
            0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) &&
            &pTxtNd->GetNodes() == &GetNodes() )
        {
            return pItem;
        }

    return 0;
}

void SwDoc::Summary( SwDoc* pExtDoc, BYTE nLevel, BYTE nPara, BOOL bImpress )
{
    const SwOutlineNodes& rOutNds = GetNodes().GetOutLineNds();
    if( pExtDoc && rOutNds.Count() )
    {
        USHORT i;
        ::StartProgress( STR_STATSTR_SUMMARY, 0, rOutNds.Count(), GetDocShell() );
        SwNodeIndex aEndOfDoc( pExtDoc->GetNodes().GetEndOfContent(), -1 );
        for( i = 0; i < rOutNds.Count(); ++i )
        {
            ::SetProgressState( i, GetDocShell() );
            ULONG nIndex = rOutNds[ i ]->GetIndex();
            BYTE nLvl = ((SwTxtNode*)GetNodes()[ nIndex ])->GetTxtColl()
                         ->GetOutlineLevel();
            if( nLvl > nLevel )
                continue;
            USHORT nEndOfs = 1;
            BYTE nWish = nPara;
            ULONG nNextOutNd = i + 1 < rOutNds.Count() ?
                rOutNds[ i + 1 ]->GetIndex() : GetNodes().Count();
            BOOL bKeep = FALSE;
            while( ( nWish || bKeep ) && nIndex + nEndOfs < nNextOutNd &&
                   GetNodes()[ nIndex + nEndOfs ]->IsTxtNode() )
            {
                SwTxtNode* pTxtNode = (SwTxtNode*)GetNodes()[ nIndex+nEndOfs ];
                if( pTxtNode->GetTxt().Len() && nWish )
                    --nWish;
                bKeep = pTxtNode->GetSwAttrSet().GetKeep().GetValue();
                ++nEndOfs;
            }

            SwNodeRange aRange( *rOutNds[ i ], 0, *rOutNds[ i ], nEndOfs );
            GetNodes()._Copy( aRange, aEndOfDoc );
        }
        const SwTxtFmtColls *pColl = pExtDoc->GetTxtFmtColls();
        for( i = 0; i < pColl->Count(); ++i )
            (*pColl)[ i ]->ResetAttr( RES_PAGEDESC, RES_BREAK );
        SwNodeIndex aIndx( pExtDoc->GetNodes().GetEndOfExtras() );
        ++aEndOfDoc;
        while( aIndx < aEndOfDoc )
        {
            SwNode *pNode;
            BOOL bDelete = FALSE;
            if(	(pNode = &aIndx.GetNode())->IsTxtNode() )
            {
                SwTxtNode *pNd = (SwTxtNode*)pNode;
                if( pNd->HasSwAttrSet() )
                    pNd->ResetAttr( RES_PAGEDESC, RES_BREAK );
                if( bImpress )
                {
                    SwTxtFmtColl* pColl = pNd->GetTxtColl();
                    USHORT nHeadLine = pColl->GetOutlineLevel()==NO_NUMBERING ?
                                RES_POOLCOLL_HEADLINE2 : RES_POOLCOLL_HEADLINE1;
                    pColl = pExtDoc->GetTxtCollFromPool( nHeadLine );
                    pNd->ChgFmtColl( pColl );
                }
                if( !pNd->Len() &&
                    pNd->StartOfSectionIndex()+2 < pNd->EndOfSectionIndex() )
                {
                    bDelete = TRUE;
                    pExtDoc->GetNodes().Delete( aIndx );
                }
            }
            if( !bDelete )
                ++aIndx;
        }
        ::EndProgress( GetDocShell() );
    }
}

    // loesche den nicht sichtbaren Content aus dem Document, wie z.B.:
    // versteckte Bereiche, versteckte Absaetze
BOOL SwDoc::RemoveInvisibleContent()
{
    BOOL bRet = FALSE;
    StartUndo( UIUNDO_DELETE_INVISIBLECNTNT );

    {
        SwTxtNode* pTxtNd;
        SwClientIter aIter( *GetSysFldType( RES_HIDDENPARAFLD ) );
        for( SwFmtFld* pFmtFld = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
                pFmtFld; pFmtFld = (SwFmtFld*)aIter.Next() )
            if( pFmtFld->GetTxtFld() &&
                0 != ( pTxtNd = (SwTxtNode*)pFmtFld->GetTxtFld()->GetpTxtNode() ) &&
                pTxtNd->GetpSwpHints() && pTxtNd->HasHiddenParaField() &&
                &pTxtNd->GetNodes() == &GetNodes() )
            {
                bRet = TRUE;
                // ein versteckter Absatz -> entfernen oder Inhalt loeschen?
                SwPaM aPam( *pTxtNd, 0, *pTxtNd, pTxtNd->GetTxt().Len() );

                if( 2 != pTxtNd->EndOfSectionIndex() -
                        pTxtNd->StartOfSectionIndex() )
                {
                    aPam.DeleteMark();
                    DelFullPara( aPam );
                }
                else
                    Delete( aPam );
            }
    }

    //
    // Remove any hidden paragraph (hidden text attribute)
    //
    for( ULONG n = GetNodes().Count(); n; )
    {
        SwTxtNode* pTxtNd = GetNodes()[ --n ]->GetTxtNode();
        if ( pTxtNd )
        {
            if ( pTxtNd->HasHiddenCharAttribute( true ) )
            {
                bRet = TRUE;
                SwPaM aPam( *pTxtNd, 0, *pTxtNd, pTxtNd->GetTxt().Len() );
                aPam.DeleteMark();
                DelFullPara( aPam );
            }
            else if ( pTxtNd->HasHiddenCharAttribute( false ) )
            {
                SwScriptInfo::DeleteHiddenRanges( *pTxtNd );
            }
        }
    }

    {
        // dann noch alle versteckten Bereiche loeschen/leeren
        SwSectionFmts aSectFmts;
        SwSectionFmts& rSectFmts = GetSections();
        USHORT n;

        for( n = rSectFmts.Count(); n; )
        {
            SwSectionFmt* pSectFmt = rSectFmts[ --n ];
            // don't add sections in Undo/Redo
            if( !pSectFmt->IsInNodesArr())
                continue;
            SwSection* pSect = pSectFmt->GetSection();
            if( pSect->CalcHiddenFlag() )
            {
                SwSection* pParent = pSect, *pTmp;
                while( 0 != (pTmp = pParent->GetParent() ))
                {
                    if( pTmp->IsHiddenFlag() )
                        pSect = pTmp;
                    pParent = pTmp;
                }

                if( USHRT_MAX == aSectFmts.GetPos( pSect->GetFmt() ) )
                    aSectFmts.Insert( pSect->GetFmt(), 0 );
            }
            if( pSect->GetCondition().Len() )
            {
                SwSection aSect( pSect->GetType(), pSect->GetName() );
                aSect = *pSect;
                aSect.SetCondition( aEmptyStr );
                aSect.SetHidden( FALSE );
                ChgSection( n, aSect );
            }
        }

        if( 0 != ( n = aSectFmts.Count() ))
        {
            while( n )
            {
                SwSectionFmt* pSectFmt = aSectFmts[ --n ];
                SwSectionNode* pSectNd = pSectFmt->GetSectionNode();
                if( pSectNd )
                {
                    bRet = TRUE;
                    SwPaM aPam( *pSectNd );

                    if( pSectNd->FindStartNode()->StartOfSectionIndex() ==
                        pSectNd->GetIndex() - 1 &&
                        pSectNd->FindStartNode()->EndOfSectionIndex() ==
                        pSectNd->EndOfSectionIndex() + 1 )
                    {
                        // nur den Inhalt loeschen
                        SwCntntNode* pCNd = GetNodes().GoNext(
                                                &aPam.GetPoint()->nNode );
                        aPam.GetPoint()->nContent.Assign( pCNd, 0 );
                        aPam.SetMark();
                        aPam.GetPoint()->nNode = *pSectNd->EndOfSectionNode();
                        pCNd = GetNodes().GoPrevious(
                                                &aPam.GetPoint()->nNode );
                        aPam.GetPoint()->nContent.Assign( pCNd, pCNd->Len() );

                        Delete( aPam );
                    }
                    else
                    {
                        // die gesamte Section loeschen
                        aPam.SetMark();
                        aPam.GetPoint()->nNode = *pSectNd->EndOfSectionNode();
                        DelFullPara( aPam );
                    }

                }
            }
            aSectFmts.Remove( 0, aSectFmts.Count() );
        }
    }

    if( bRet )
        SetModified();
    EndUndo( UIUNDO_DELETE_INVISIBLECNTNT );
    return bRet;
}
/*-- 11.06.2004 08:34:04---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwDoc::ConvertFieldsToText()
{
    BOOL bRet = FALSE;
    StartUndo( UIUNDO_REPLACE );

    const SwFldTypes* pFldTypes = GetFldTypes();
    sal_uInt16 nCount = pFldTypes->Count();
    //go backward, field types are removed
    for(sal_uInt16 nType = nCount;  nType > 0;  --nType)
    {
        const SwFieldType *pCurType = pFldTypes->GetObject(nType - 1);

        if ( RES_POSTITFLD == pCurType->Which() )
            continue;

        SwClientIter aIter( *(SwFieldType*)pCurType );
        const SwFmtFld* pCurFldFmt = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
        ::std::vector<const SwFmtFld*> aFieldFmts;
        while (pCurFldFmt)
        {
            aFieldFmts.push_back(pCurFldFmt);
            pCurFldFmt = (SwFmtFld*)aIter.Next();
        }
        ::std::vector<const SwFmtFld*>::iterator aBegin = aFieldFmts.begin();
        ::std::vector<const SwFmtFld*>::iterator aEnd = aFieldFmts.end();
        while(aBegin != aEnd)
        {
            const SwTxtFld *pTxtFld = (*aBegin)->GetTxtFld();
            // skip fields that are currently not in the document
            // e.g. fields in undo or redo array
            BOOL bSkip = !pTxtFld ||
                         !pTxtFld->GetpTxtNode()->GetNodes().IsDocNodes() ||
                         IsInHeaderFooter(SwNodeIndex(*pTxtFld->GetpTxtNode(), 0));

            if (!bSkip)
            {
                const SwFmtFld& rFmtFld = pTxtFld->GetFld();
                const SwField*  pField = rFmtFld.GetFld();
                String sText = pField->GetCntnt();
                //database fields should not convert their command into text
                if( RES_DBFLD == pCurType->Which() && !static_cast<const SwDBField*>(pField)->IsInitialized())
                    sText.Erase();

                //now remove the field and insert the string
                SwPaM aPam(*pTxtFld->GetpTxtNode(), *pTxtFld->GetStart());
                aPam.SetMark();
                aPam.Move();
                DeleteAndJoin(aPam);
                Insert( aPam, sText );
            }
            ++aBegin;
        }
    }

    if( bRet )
        SetModified();
    EndUndo( UIUNDO_REPLACE );
    return bRet;

}

    // embedded alle lokalen Links (Bereiche/Grafiken)
BOOL SwDoc::EmbedAllLinks()
{
    BOOL bRet = FALSE;
    SvxLinkManager& rLnkMgr = GetLinkManager();
    const ::sfx2::SvBaseLinks& rLnks = rLnkMgr.GetLinks();
    if( rLnks.Count() )
    {
        BOOL bDoesUndo = DoesUndo();
        DoUndo( FALSE );

        for( USHORT n = 0; n < rLnks.Count(); ++n )
        {
            ::sfx2::SvBaseLink* pLnk = &(*rLnks[ n ]);
            if( pLnk &&
                ( OBJECT_CLIENT_GRF == pLnk->GetObjType() ||
                  OBJECT_CLIENT_FILE == pLnk->GetObjType() ) &&
                pLnk->ISA( SwBaseLink ) )
            {
                ::sfx2::SvBaseLinkRef xLink = pLnk;
                USHORT nCount = rLnks.Count();

                String sFName;
                rLnkMgr.GetDisplayNames( xLink, 0, &sFName, 0, 0 );

                INetURLObject aURL( sFName );
                if( INET_PROT_FILE == aURL.GetProtocol() ||
                    INET_PROT_CID == aURL.GetProtocol() )
                {
                    // dem Link sagen, das er aufgeloest wird!
                    xLink->Closed();

                    // falls einer vergessen hat sich auszutragen
                    if( xLink.Is() )
                        rLnkMgr.Remove( xLink );

                    if( nCount != rLnks.Count() + 1 )
                        n = 0;		// wieder von vorne anfangen, es wurden
                                    // mehrere Links entfernt
                    bRet = TRUE;
                }
            }
        }

        DelAllUndoObj();
        DoUndo( bDoesUndo );
        SetModified();
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwDoc::IsInsTblFormatNum() const
{
    return SW_MOD()->IsInsTblFormatNum(IsHTMLMode());
}

BOOL SwDoc::IsInsTblChangeNumFormat() const
{
    return SW_MOD()->IsInsTblChangeNumFormat(IsHTMLMode());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwDoc::IsInsTblAlignNum() const
{
    return SW_MOD()->IsInsTblAlignNum(IsHTMLMode());
}


USHORT SwDoc::GetLinkUpdMode() const
{
    USHORT nRet = nLinkUpdMode;
    if( GLOBALSETTING == nRet )
        nRet = SW_MOD()->GetLinkUpdMode(IsHTMLMode());
    return nRet;
}

USHORT SwDoc::GetFldUpdateFlags() const
{
    USHORT nRet = nFldUpdMode;
    if( AUTOUPD_GLOBALSETTING == nRet )
        nRet = SW_MOD()->GetFldUpdateFlags(IsHTMLMode());
    return nRet;
}

        // setze das InsertDB als Tabelle Undo auf:
void SwDoc::AppendUndoForInsertFromDB( const SwPaM& rPam, BOOL bIsTable )
{
    if( bIsTable )
    {
        const SwTableNode* pTblNd = rPam.GetPoint()->nNode.GetNode().FindTableNode();
        if( pTblNd )
        {
            SwUndoCpyTbl* pUndo = new SwUndoCpyTbl;
            pUndo->SetTableSttIdx( pTblNd->GetIndex() );
            AppendUndo( pUndo );
        }
    }
    else if( rPam.HasMark() )
    {
        SwUndoCpyDoc* pUndo = new SwUndoCpyDoc( rPam );
        pUndo->SetInsertRange( rPam, FALSE );
        AppendUndo( pUndo );
    }
}

void SwDoc::ChgTOX(SwTOXBase & rTOX, const SwTOXBase & rNew)
{
    if (DoesUndo())
    {
        DelAllUndoObj();

        SwUndo * pUndo = new SwUndoTOXChange(&rTOX, rNew);

        AppendUndo(pUndo);
    }

    rTOX = rNew;

    if (rTOX.ISA(SwTOXBaseSection))
    {
        static_cast<SwTOXBaseSection &>(rTOX).Update();
        static_cast<SwTOXBaseSection &>(rTOX).UpdatePageNum();
    }
}

// #111827#
String SwDoc::GetPaMDescr(const SwPaM & rPam) const
{
    String aResult;
    bool bOK = false;

    if (rPam.GetNode(TRUE) == rPam.GetNode(FALSE))
    {
        SwTxtNode * pTxtNode = rPam.GetNode(TRUE)->GetTxtNode();

        if (0 != pTxtNode)
        {
            xub_StrLen nStart = rPam.Start()->nContent.GetIndex();
            xub_StrLen nEnd = rPam.End()->nContent.GetIndex();

            aResult += String(SW_RES(STR_START_QUOTE));
            aResult += ShortenString(pTxtNode->GetTxt().
                                     Copy(nStart, nEnd - nStart),
                                     nUndoStringLength,
                                     String(SW_RES(STR_LDOTS)));
            aResult += String(SW_RES(STR_END_QUOTE));

            bOK = true;
        }
    }
    else if (0 != rPam.GetNode(TRUE))
    {
        if (0 != rPam.GetNode(FALSE))
            aResult += String(SW_RES(STR_PARAGRAPHS));

        bOK = true;
    }

    if (! bOK)
        aResult += String("??", RTL_TEXTENCODING_ASCII_US);

    return aResult;
}

// -> #111840#
SwField * SwDoc::GetField(const SwPosition & rPos)
{
    SwField * pResult = NULL;

    SwTxtFld * pAttr = rPos.nNode.GetNode().GetTxtNode()->
        GetTxtFld(rPos.nContent);

    if (pAttr)
        pResult = (SwField *) pAttr->GetFld().GetFld();

    return pResult;
}

SwTxtFld * SwDoc::GetTxtFld(const SwPosition & rPos)
{
    SwTxtNode *pNode = rPos.nNode.GetNode().GetTxtNode();
    if( pNode )
        return pNode->GetTxtFld( rPos.nContent );
    else
        return 0;
}
// <- #111840#

bool SwDoc::ContainsHiddenChars() const
{
    for( ULONG n = GetNodes().Count(); n; )
    {
        SwNode* pNd = GetNodes()[ --n ];
        if ( ND_TEXTNODE == pNd->GetNodeType() &&
             ((SwTxtNode*)pNd)->HasHiddenCharAttribute( false ) )
            return true;
    }

    return false;
}

// #111955#
void SwDoc::SetOldNumbering(sal_Bool _bOldNumbering)
{
    if (bOldNumbering != _bOldNumbering)
    {
        bOldNumbering = _bOldNumbering;

        const SwNumRuleTbl& rNmTbl = GetNumRuleTbl();
        for( USHORT n = 0; n < rNmTbl.Count(); ++n )
            rNmTbl[n]->SetInvalidRule(TRUE);

        UpdateNumRule();

        if (bOldNumbering)
            GetNodes().UpdateOutlineNodes();
        else
        {
            if (pOutlineRule)
                UpdateNumRule(*pOutlineRule, 0); // #115901#
        }
    }
}
