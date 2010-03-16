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


#include <sfx2/linkmgr.hxx>
#include <fmtcntnt.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <pam.hxx>
#include <ndtxt.hxx>
#include <undobj.hxx>
#include <section.hxx>
#include <rolbck.hxx>
#include <redline.hxx>
#include <doctxm.hxx>
#include <ftnidx.hxx>
#include <editsh.hxx>
/// OD 04.10.2002 #102894#
/// class Calc needed for calculation of the hidden condition of a section.
#include <calc.hxx>


inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }

SfxItemSet* lcl_GetAttrSet( const SwSection& rSect )
{
    // Attribute des Formate sichern (Spalten, Farbe, ... )
    // Cntnt- und Protect- Items interessieren nicht (stehen schon in der
    // Section), muessen also entfernen werden
    SfxItemSet* pAttr = 0;
    if( rSect.GetFmt() )
    {
        USHORT nCnt = 1;
        if( rSect.IsProtect() )
            ++nCnt;

        if( nCnt < rSect.GetFmt()->GetAttrSet().Count() )
        {
            pAttr = new SfxItemSet( rSect.GetFmt()->GetAttrSet() );
            pAttr->ClearItem( RES_PROTECT );
            pAttr->ClearItem( RES_CNTNT );
            if( !pAttr->Count() )
                delete pAttr, pAttr = 0;
        }
    }
    return pAttr;
}

SwUndoInsSection::SwUndoInsSection(
        SwPaM const& rPam, SwSectionData const& rNewData,
        SfxItemSet const*const pSet, SwTOXBase const*const pTOXBase)
    : SwUndo( UNDO_INSSECTION ), SwUndRng( rPam )
    , m_pSectionData(new SwSectionData(rNewData))
    , m_pTOXBase( (pTOXBase) ? new SwTOXBase(*pTOXBase) : 0 )
    , m_pAttrSet( (pSet && pSet->Count()) ? new SfxItemSet( *pSet ) : 0 )
    , m_pHistory(0)
    , m_pRedlData(0)
    , m_nSectionNodePos(0)
    , m_bSplitAtStart(false)
    , m_bSplitAtEnd(false)
    , m_bUpdateFtn(false)
{
    SwDoc& rDoc = *(SwDoc*)rPam.GetDoc();
    if( rDoc.IsRedlineOn() )
    {
        m_pRedlData.reset(new SwRedlineData( nsRedlineType_t::REDLINE_INSERT,
                                        rDoc.GetRedlineAuthor() ));
        SetRedlineMode( rDoc.GetRedlineMode() );
    }


    if( !rPam.HasMark() )
    {
        const SwCntntNode* pCNd = rPam.GetPoint()->nNode.GetNode().GetCntntNode();
        if( pCNd && pCNd->HasSwAttrSet() && (
            !rPam.GetPoint()->nContent.GetIndex() ||
            rPam.GetPoint()->nContent.GetIndex() == pCNd->Len() ))
        {
            SfxItemSet aBrkSet( rDoc.GetAttrPool(), aBreakSetRange );
            aBrkSet.Put( *pCNd->GetpSwAttrSet() );
            if( aBrkSet.Count() )
            {
                m_pHistory.reset( new SwHistory );
                m_pHistory->CopyFmtAttr( aBrkSet, pCNd->GetIndex() );
            }
        }
    }
}

SwUndoInsSection::~SwUndoInsSection()
{
}

void SwUndoInsSection::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    RemoveIdxFromSection( rDoc, m_nSectionNodePos );

    SwSectionNode *const pNd =
        rDoc.GetNodes()[ m_nSectionNodePos ]->GetSectionNode();
    ASSERT( pNd, "wo ist mein SectionNode?" );

    if( IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode() ))
        rDoc.DeleteRedline( *pNd, true, USHRT_MAX );

    // lag keine Selektion vor ??
    SwNodeIndex aIdx( *pNd );
    if( ( !nEndNode && STRING_MAXLEN == nEndCntnt ) ||
        ( nSttNode == nEndNode && nSttCntnt == nEndCntnt ))
        // loesche einfach alle Nodes
        rDoc.GetNodes().Delete( aIdx, pNd->EndOfSectionIndex() -
                                        aIdx.GetIndex() );
    else
        // einfach das Format loeschen, der Rest erfolgt automatisch
        rDoc.DelSectionFmt( pNd->GetSection().GetFmt() );

    // muessen wir noch zusammenfassen ?
    if (m_bSplitAtStart)
    {
        Join( rDoc, nSttNode );
    }

    if (m_bSplitAtEnd)
    {
        Join( rDoc, nEndNode );
    }

    if (m_pHistory.get())
    {
        m_pHistory->TmpRollback( &rDoc, 0, false );
    }

    if (m_bUpdateFtn)
    {
        rDoc.GetFtnIdxs().UpdateFtn( aIdx );
    }

    SetPaM( rUndoIter );
}


void SwUndoInsSection::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SetPaM( rUndoIter );

    const SwTOXBaseSection* pUpdateTOX = 0;
    if (m_pTOXBase.get())
    {
        pUpdateTOX = rDoc.InsertTableOf( *rUndoIter.pAktPam->GetPoint(),
                                        *m_pTOXBase, m_pAttrSet.get(), true);
    }
    else
    {
        rDoc.InsertSwSection(*rUndoIter.pAktPam,
                *m_pSectionData, 0, m_pAttrSet.get(), true);
    }

    if (m_pHistory.get())
    {
        m_pHistory->SetTmpEnd( m_pHistory->Count() );
    }

    SwSectionNode *const pSectNd =
        rDoc.GetNodes()[ m_nSectionNodePos ]->GetSectionNode();
    if (m_pRedlData.get() &&
        IDocumentRedlineAccess::IsRedlineOn( GetRedlineMode()))
    {
        RedlineMode_t eOld = rDoc.GetRedlineMode();
        rDoc.SetRedlineMode_intern((RedlineMode_t)(eOld & ~nsRedlineMode_t::REDLINE_IGNORE));

        SwPaM aPam( *pSectNd->EndOfSectionNode(), *pSectNd, 1 );
        rDoc.AppendRedline( new SwRedline( *m_pRedlData, aPam ), true);
        rDoc.SetRedlineMode_intern( eOld );
    }
    else if( !( nsRedlineMode_t::REDLINE_IGNORE & GetRedlineMode() ) &&
            rDoc.GetRedlineTbl().Count() )
    {
        SwPaM aPam( *pSectNd->EndOfSectionNode(), *pSectNd, 1 );
        rDoc.SplitRedline( aPam );
    }

    if( pUpdateTOX )
    {
        // Formatierung anstossen
        SwEditShell* pESh = rDoc.GetEditShell();
        if( pESh )
            pESh->CalcLayout();

        // Seitennummern eintragen
        ((SwTOXBaseSection*)pUpdateTOX)->UpdatePageNum();
    }
}


void SwUndoInsSection::Repeat( SwUndoIter& rUndoIter )
{
    if (m_pTOXBase.get())
    {
        rUndoIter.GetDoc().InsertTableOf( *rUndoIter.pAktPam->GetPoint(),
                                        *m_pTOXBase, m_pAttrSet.get(), true);
    }
    else
    {
        rUndoIter.GetDoc().InsertSwSection( *rUndoIter.pAktPam,
            *m_pSectionData, 0, m_pAttrSet.get());
    }
}


void SwUndoInsSection::Join( SwDoc& rDoc, ULONG nNode )
{
    SwNodeIndex aIdx( rDoc.GetNodes(), nNode );
    SwTxtNode* pTxtNd = aIdx.GetNode().GetTxtNode();
    ASSERT( pTxtNd, "wo ist mein TextNode?" );

    {
        RemoveIdxRel( nNode + 1, SwPosition( aIdx,
                            SwIndex( pTxtNd, pTxtNd->GetTxt().Len() )));
    }
    pTxtNd->JoinNext();

    if (m_pHistory.get())
    {
        SwIndex aCntIdx( pTxtNd, 0 );
        pTxtNd->RstAttr( aCntIdx, pTxtNd->Len(), 0, 0, true );
    }
}


void
SwUndoInsSection::SaveSplitNode(SwTxtNode *const pTxtNd, bool const bAtStart)
{
    if( pTxtNd->GetpSwpHints() )
    {
        if (!m_pHistory.get())
        {
            m_pHistory.reset( new SwHistory );
        }
        m_pHistory->CopyAttr( pTxtNd->GetpSwpHints(), pTxtNd->GetIndex(), 0,
                            pTxtNd->GetTxt().Len(), false );
    }

    if (bAtStart)
    {
        m_bSplitAtStart = true;
    }
    else
    {
        m_bSplitAtEnd = true;
    }
}


// -----------------------------

SwUndoDelSection::SwUndoDelSection( const SwSectionFmt& rFmt )
     : SwUndo( UNDO_DELSECTION )
{
    const SwSection& rSect = *rFmt.GetSection();
    if( rSect.ISA( SwTOXBaseSection ))
    {
        m_pTOXBase.reset(
            new SwTOXBase(static_cast<SwTOXBaseSection const&>(rSect)) );
    }
    m_pSectionData.reset( new SwSectionData(rSect) );

    pAttr = ::lcl_GetAttrSet( rSect );

    const SwNodeIndex* pIdx = rFmt.GetCntnt().GetCntntIdx();
    nSttNd = pIdx->GetIndex();
    nEndNd = pIdx->GetNode().EndOfSectionIndex();
}


SwUndoDelSection::~SwUndoDelSection()
{
    delete pAttr;
}


void SwUndoDelSection::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    if (m_pTOXBase.get())
    {
        rDoc.InsertTableOf( nSttNd, nEndNd-2, *m_pTOXBase, pAttr );
    }
    else
    {
        SwNodeIndex aStt( rDoc.GetNodes(), nSttNd );
        SwNodeIndex aEnd( rDoc.GetNodes(), nEndNd-2 );
        SwSectionFmt* pFmt = rDoc.MakeSectionFmt( 0 );
        if( pAttr )
            pFmt->SetFmtAttr( *pAttr );

        /// OD 04.10.2002 #102894#
        /// remember inserted section node for further calculations
        SwSectionNode* pInsertedSectNd = rDoc.GetNodes().InsertTextSection(
                aStt, *pFmt, *m_pSectionData, 0, & aEnd);

        if( SFX_ITEM_SET == pFmt->GetItemState( RES_FTN_AT_TXTEND ) ||
            SFX_ITEM_SET == pFmt->GetItemState( RES_END_AT_TXTEND ))
        {
            rDoc.GetFtnIdxs().UpdateFtn( aStt );
        }

        /// OD 04.10.2002 #102894#
        /// consider that section is hidden by condition.
        /// If section is hidden by condition,
        /// recalculate condition and update hidden condition flag.
        /// Recalculation is necessary, because fields, on which the hide
        /// condition depends, can be changed - fields changes aren't undoable.
        /// NOTE: setting hidden condition flag also creates/deletes corresponding
        ///     frames, if the hidden condition flag changes.
        SwSection& aInsertedSect = pInsertedSectNd->GetSection();
        if ( aInsertedSect.IsHidden() &&
             aInsertedSect.GetCondition().Len() > 0 )
        {
            SwCalc aCalc( rDoc );
            rDoc.FldsToCalc(aCalc, pInsertedSectNd->GetIndex(), USHRT_MAX);
            bool bRecalcCondHidden =
                    aCalc.Calculate( aInsertedSect.GetCondition() ).GetBool() ? true : false;
            aInsertedSect.SetCondHidden( bRecalcCondHidden );
        }

    }
}


void SwUndoDelSection::Redo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();

    SwSectionNode* pNd = rDoc.GetNodes()[ nSttNd ]->GetSectionNode();
    ASSERT( pNd, "wo ist mein SectionNode?" );
    // einfach das Format loeschen, der Rest erfolgt automatisch
    rDoc.DelSectionFmt( pNd->GetSection().GetFmt() );
}



SwUndoChgSection::SwUndoChgSection( const SwSectionFmt& rFmt, BOOL bOnlyAttr )
     : SwUndo( UNDO_CHGSECTION ), bOnlyAttrChgd( bOnlyAttr )
{
    const SwSection& rSect = *rFmt.GetSection();

    m_pSectionData.reset( new SwSectionData(rSect) );

    pAttr = ::lcl_GetAttrSet( rSect );

    nSttNd = rFmt.GetCntnt().GetCntntIdx()->GetIndex();
}


SwUndoChgSection::~SwUndoChgSection()
{
    delete pAttr;
}


void SwUndoChgSection::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    SwSectionNode* pSectNd = rDoc.GetNodes()[ nSttNd ]->GetSectionNode();
    ASSERT( pSectNd, "wo ist mein SectionNode?" );

    SwSection& rNdSect = pSectNd->GetSection();
    SwFmt* pFmt = rNdSect.GetFmt();

    SfxItemSet* pCur = ::lcl_GetAttrSet( rNdSect );
    if( pAttr )
    {
        // das Content- und Protect-Item muss bestehen bleiben
        const SfxPoolItem* pItem;
        pAttr->Put( pFmt->GetFmtAttr( RES_CNTNT ));
        if( SFX_ITEM_SET == pFmt->GetItemState( RES_PROTECT, TRUE, &pItem ))
            pAttr->Put( *pItem );
        pFmt->DelDiffs( *pAttr );
        pAttr->ClearItem( RES_CNTNT );
        pFmt->SetFmtAttr( *pAttr );
        delete pAttr;
    }
    else
    {
        // dann muessen die alten entfernt werden
        pFmt->ResetFmtAttr( RES_FRMATR_BEGIN, RES_BREAK );
        pFmt->ResetFmtAttr( RES_HEADER, RES_OPAQUE );
        pFmt->ResetFmtAttr( RES_SURROUND, RES_FRMATR_END-1 );
    }
    pAttr = pCur;

    if( !bOnlyAttrChgd )
    {
        const bool bUpdate =
               (!rNdSect.IsLinkType() && m_pSectionData->IsLinkType())
            || (    m_pSectionData->GetLinkFileName().Len()
                &&  (m_pSectionData->GetLinkFileName() !=
                        rNdSect.GetLinkFileName()));

        // swap stored section data with live section data
        SwSectionData *const pOld( new SwSectionData(rNdSect) );
        rNdSect.SetSectionData(*m_pSectionData);
        m_pSectionData.reset(pOld);

        if( bUpdate )
            rNdSect.CreateLink( CREATE_UPDATE );
        else if( CONTENT_SECTION == rNdSect.GetType() && rNdSect.IsConnected() )
        {
            rNdSect.Disconnect();
            rDoc.GetLinkManager().Remove( &rNdSect.GetBaseLink() );
        }
    }
}


void SwUndoChgSection::Redo( SwUndoIter& rUndoIter )
{
    Undo( rUndoIter );
}
