/*************************************************************************
 *
 *  $RCSfile: edattr.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: jp $ $Date: 2000-11-20 16:40:11 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_TSTPITEM_HXX //autogen
#include <svx/tstpitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
#ifndef _TXTFTN_HXX //autogen
#include <txtftn.hxx>
#endif
#ifndef _FMTFTN_HXX //autogen
#include <fmtftn.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>	// fuer MACROS
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>	// fuer UNDO-Ids
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FTNIDX_HXX
#include <ftnidx.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _SCRPMTCH_HXX
#include <scrpmtch.hxx>
#endif

using namespace ::com::sun::star::i18n;

/*************************************
 * harte Formatierung (Attribute)
 *************************************/

// wenn Selektion groesser Max Nodes oder mehr als Max Selektionen
// => keine Attribute
static const USHORT nMaxLookup = 255;

BOOL SwEditShell::GetAttr( SfxItemSet& rSet ) const
{
    if( GetCrsrCnt() > nMaxLookup )
    {
        rSet.InvalidateAllItems();
        return FALSE;
    }

    SfxItemSet aSet( *rSet.GetPool(), rSet.GetRanges() );
    SfxItemSet *pSet = &rSet;

    FOREACHPAM_START(this)

        ULONG nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
              nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();
        xub_StrLen nSttCnt = PCURCRSR->GetMark()->nContent.GetIndex(),
                   nEndCnt = PCURCRSR->GetPoint()->nContent.GetIndex();

        if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
        {
            ULONG nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
            nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (xub_StrLen)nTmp;
        }

        if( nEndNd - nSttNd >= nMaxLookup )
        {
            rSet.ClearItem();
            rSet.InvalidateAllItems();
            return FALSE;
        }

        // beim 1.Node traegt der Node die Werte in den GetSet ein (Initial)
        // alle weiteren Nodes werden zum GetSet zu gemergt
        for( ULONG n = nSttNd; n <= nEndNd; ++n )
        {
            SwNode* pNd = GetDoc()->GetNodes()[ n ];
            switch( pNd->GetNodeType() )
            {
            case ND_TEXTNODE:
                {
                    xub_StrLen nStt = n == nSttNd ? nSttCnt : 0,
                                  nEnd = n == nEndNd ? nEndCnt
                                        : ((SwTxtNode*)pNd)->GetTxt().Len();
                    ((SwTxtNode*)pNd)->GetAttr( *pSet, nStt, nEnd );
                }
                break;
            case ND_GRFNODE:
            case ND_OLENODE:
                ((SwCntntNode*)pNd)->GetAttr( *pSet );
                break;

            default:
                pNd = 0;
            }

            if( pNd )
            {
                if( pSet != &rSet )
                    rSet.MergeValues( aSet );

                if( aSet.Count() )
                    aSet.ClearItem();

#ifdef JP_NEWCORE
            // vieleicht sollte man hier noch erfragen, ob schon alle Attribute
            // "DontCare" sind. Dann kann man abbrechen!
#endif
            }
            pSet = &aSet;
        }

    FOREACHPAM_END()

    return TRUE;
}


BOOL lcl_GetFmtColl( const SwNodePtr& rpNd, void* pArgs )
{
    if( rpNd->IsTxtNode() )
    {
        SwFmtColl** ppColl = (SwFmtColl**)pArgs;
        if( !*ppColl )
            *ppColl = ((SwTxtNode*)rpNd)->GetTxtColl();
        else if( *ppColl == ((SwTxtNode*)rpNd)->GetTxtColl() )
            return FALSE;
    }
    return TRUE;
}


SwTxtFmtColl* SwEditShell::GetCurTxtFmtColl() const
{
    SwTxtFmtColl *pFmt = 0;

    if ( GetCrsrCnt() > nMaxLookup )
        return 0;

    FOREACHPAM_START(this)

        ULONG nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
              nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();
        xub_StrLen nSttCnt = PCURCRSR->GetMark()->nContent.GetIndex(),
                   nEndCnt = PCURCRSR->GetPoint()->nContent.GetIndex();

        if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
        {
            ULONG nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
            nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (xub_StrLen)nTmp;
        }

        if( nEndNd - nSttNd >= nMaxLookup )
        {
            pFmt = 0;
            break;
        }

        for( ULONG n = nSttNd; n <= nEndNd; ++n )
        {
            SwNode* pNd = GetDoc()->GetNodes()[ n ];
            if( pNd->IsTxtNode() )
            {
                if( !pFmt )
                    pFmt = ((SwTxtNode*)pNd)->GetTxtColl();
                else if( pFmt == ((SwTxtNode*)pNd)->GetTxtColl() ) // ???
                    break;
            }
        }

    FOREACHPAM_END()
    return pFmt;
}



BOOL SwEditShell::GetCurFtn( SwFmtFtn* pFillFtn )
{
    // der Cursor muss auf dem akt. Fussnoten-Anker stehen:
    SwPaM* pCrsr = GetCrsr();
    SwTxtNode* pTxtNd = pCrsr->GetNode()->GetTxtNode();
    if( !pTxtNd )
        return FALSE;

    SwTxtAttr *pFtn = pTxtNd->GetTxtAttr( pCrsr->GetPoint()->nContent,
                                            RES_TXTATR_FTN );
    if( pFtn && pFillFtn )
    {
        // Daten vom Attribut uebertragen
        const SwFmtFtn &rFtn = ((SwTxtFtn*)pFtn)->GetFtn();
        pFillFtn->SetNumber( rFtn );
        pFillFtn->SetEndNote( rFtn.IsEndNote() );
    }
    return 0 != pFtn;
}


BOOL SwEditShell::SetCurFtn( const SwFmtFtn& rFillFtn )
{
    BOOL bChgd = FALSE;
    StartAllAction();

    SwPaM* pCrsr = GetCrsr(), *pFirst = pCrsr;
    do {
        bChgd |=  pDoc->SetCurFtn( *pCrsr, rFillFtn.GetNumStr(),
                                            rFillFtn.GetNumber(),
                                            rFillFtn.IsEndNote() );

    } while( pFirst != ( pCrsr = (SwPaM*)pCrsr->GetNext() ));

    EndAllAction();
    return bChgd;
}



/*USHORT SwEditShell::GetFtnCnt( BOOL bEndNotes = FALSE ) const
{
    const SwFtnIdxs &rIdxs = pDoc->GetFtnIdxs();
    USHORT nCnt = 0;
    for ( USHORT i = 0; i < rIdxs.Count(); ++i )
    {
        const SwFmtFtn &rFtn = rIdxs[i]->GetFtn();
        if ( bEndNotes == rFtn.IsEndNote() )
            nCnt++;
    }
    return nCnt;
} */


BOOL SwEditShell::HasFtns( BOOL bEndNotes ) const
{
    const SwFtnIdxs &rIdxs = pDoc->GetFtnIdxs();
    for ( USHORT i = 0; i < rIdxs.Count(); ++i )
    {
        const SwFmtFtn &rFtn = rIdxs[i]->GetFtn();
        if ( bEndNotes == rFtn.IsEndNote() )
            return TRUE;
    }
    return FALSE;
}


    // gebe Liste aller Fussnoten und deren Anfangstexte
USHORT SwEditShell::GetSeqFtnList( SwSeqFldList& rList, BOOL bEndNotes )
{
    if( rList.Count() )
        rList.Remove( 0, rList.Count() );

    USHORT n, nFtnCnt = pDoc->GetFtnIdxs().Count();
    SwTxtFtn* pTxtFtn;
    for( n = 0; n < nFtnCnt; ++n )
    {
        pTxtFtn = pDoc->GetFtnIdxs()[ n ];
        const SwFmtFtn& rFtn = pTxtFtn->GetFtn();
        if ( rFtn.IsEndNote() != bEndNotes )
            continue;

        SwNodeIndex* pIdx = pTxtFtn->GetStartNode();
        if( pIdx )
        {
            SwNodeIndex aIdx( *pIdx, 1 );
            SwTxtNode* pTxtNd = aIdx.GetNode().GetTxtNode();
            if( !pTxtNd )
                pTxtNd = (SwTxtNode*)pDoc->GetNodes().GoNext( &aIdx );

            if( pTxtNd )
            {
                String sTxt( rFtn.GetViewNumStr( *pDoc ));
                if( sTxt.Len() )
                    sTxt += ' ';
                sTxt += pTxtNd->GetExpandTxt( 0, USHRT_MAX, FALSE );

                _SeqFldLstElem* pNew = new _SeqFldLstElem( sTxt,
                                            pTxtFtn->GetSeqRefNo() );
                while( rList.InsertSort( pNew ) )
                    pNew->sDlgEntry += ' ';
            }
        }
    }

    return rList.Count();
}


// linken Rand ueber Objectleiste einstellen (aenhlich dem Stufen von
// Numerierungen)
BOOL SwEditShell::IsMoveLeftMargin( BOOL bRight, BOOL bModulus ) const
{
    BOOL bRet = TRUE;

    const SvxTabStopItem& rTabItem = (SvxTabStopItem&)GetDoc()->
                                GetDefault( RES_PARATR_TABSTOP );
    USHORT nDefDist = rTabItem.Count() ? rTabItem[0].GetTabPos() : 1134;
    if( !nDefDist )
        return FALSE;

    FOREACHPAM_START(this)

        ULONG nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
              nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();

        if( nSttNd > nEndNd )
        {
            ULONG nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
        }

        SwCntntNode* pCNd;
        for( ULONG n = nSttNd; bRet && n <= nEndNd; ++n )
            if( 0 != ( pCNd = GetDoc()->GetNodes()[ n ]->GetTxtNode() ))
            {
                const SvxLRSpaceItem& rLS = (SvxLRSpaceItem&)
                                            pCNd->GetAttr( RES_LR_SPACE );
                if( bRight )
                {
                    USHORT nNext = rLS.GetTxtLeft() + nDefDist;
                    if( bModulus )
                        nNext = ( nNext / nDefDist ) * nDefDist;
                    SwFrm* pFrm = pCNd->GetFrm();
                    bRet = pFrm && pFrm->Frm().Width() > ( nNext + MM50 );
                }
                else if( bModulus )
                    bRet = 0 != rLS.GetLeft();
                else
                    bRet = nDefDist <= rLS.GetLeft();
            }

        if( !bRet )
            break;

    FOREACHPAM_END()
    return bRet;
}


void SwEditShell::MoveLeftMargin( BOOL bRight, BOOL bModulus )
{
    StartAllAction();
    StartUndo( UNDO_START );

    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() != pCrsr )			// Mehrfachselektion ?
    {
        SwPamRanges aRangeArr( *pCrsr );
        SwPaM aPam( *pCrsr->GetPoint() );
        for( USHORT n = 0; n < aRangeArr.Count(); ++n )
            GetDoc()->MoveLeftMargin( aRangeArr.SetPam( n, aPam ),
                                        bRight, bModulus );
    }
    else
        GetDoc()->MoveLeftMargin( *pCrsr, bRight, bModulus );

    EndUndo( UNDO_END );
    EndAllAction();
}


inline USHORT lcl_SetScriptFlags( USHORT nType )
{
    USHORT nRet;
       switch( nType )
    {
    case ScriptType::LATIN:		nRet = SCRIPTTYPE_LATIN;	break;
    case ScriptType::ASIAN:		nRet = SCRIPTTYPE_ASIAN;	break;
    case ScriptType::COMPLEX:	nRet = SCRIPTTYPE_COMPLEX;	break;
    default: nRet = 0;
    }
    return nRet;
}

// returns the scripttpye of the selection
USHORT SwEditShell::GetScriptType() const
{
    USHORT nRet = SCRIPTTYPE_NONE;
    if( pBreakIt->xBreak.is() )
    {
        FOREACHPAM_START(this)

            const SwPosition *pStt = PCURCRSR->Start(),
                             *pEnd = pStt == PCURCRSR->GetMark()
                                    ? PCURCRSR->GetPoint()
                                    : PCURCRSR->GetMark();
            if( pStt == pEnd || *pStt == *pEnd )
            {
                if( pStt->nNode.GetNode().IsTxtNode() )
                {
                    const String& rTxt = pStt->nNode.GetNode().
                                                    GetTxtNode()->GetTxt();
                    nRet |= lcl_SetScriptFlags( pBreakIt->xBreak->
                        getScriptType( rTxt, pStt->nContent.GetIndex() ));
                }
            }
            else
            {
                ULONG nEnd = pEnd->nNode.GetIndex();
                SwNodeIndex aIdx( pStt->nNode );
                for( ; aIdx.GetIndex() <= nEnd; aIdx++ )
                    if( aIdx.GetNode().IsTxtNode() )
                    {
                        const String& rTxt = aIdx.GetNode().GetTxtNode()->GetTxt();
                        xub_StrLen nChg = aIdx == pStt->nNode
                                                ? pStt->nContent.GetIndex()
                                                : 0,
                                    nLen = aIdx == nEnd
                                                ? pEnd->nContent.GetIndex()
                                                : rTxt.Len();

                        USHORT nScript = pBreakIt->xBreak->getScriptType(
                                                                rTxt, nChg );
                        if( ScriptType::WEAK == nScript )
                            nChg = pBreakIt->xBreak->endOfScript(
                                                        rTxt, nChg, nScript );

                        while( nChg < nLen )
                        {
                            nScript = pBreakIt->xBreak->getScriptType( rTxt, nChg );
                            nRet |= lcl_SetScriptFlags( nScript );
                            if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
                                SCRIPTTYPE_COMPLEX) == nRet )
                                break;
                            nChg = pBreakIt->xBreak->endOfScript(
                                                        rTxt, nChg, nScript );
                        }
                        if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
                                SCRIPTTYPE_COMPLEX) == nRet )
                            break;
                    }
            }
            if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
                                SCRIPTTYPE_COMPLEX) == nRet )
                break;

        FOREACHPAM_END()
    }
    return nRet;
}


GetLatinAsianComplexAttr::GetLatinAsianComplexAttr( USHORT nWhich )
    : nWhId( nWhich ), pSet( 0 )
{
    Init();
}

GetLatinAsianComplexAttr::GetLatinAsianComplexAttr( USHORT nWhich,
                                                    SwEditShell& rSh )
    : nWhId( nWhich )
{
    Init();
    pSet = new SfxItemSet( rSh.GetAttrPool(), nWhId, nWhId,
                                        nCJKWhId, nCJKWhId,
                                        nCTLWhId, nCTLWhId,
                                        0 );
    rSh.GetAttr( *pSet );
}
GetLatinAsianComplexAttr::~GetLatinAsianComplexAttr()
{
    delete pSet;
}

void GetLatinAsianComplexAttr::Init()
{
    switch( nWhId )
    {
    case RES_CHRATR_FONT:
        nCJKWhId = RES_CHRATR_CJK_FONT;
        nCTLWhId = RES_CHRATR_CTL_FONT;
        break;
    case RES_CHRATR_FONTSIZE:
        nCJKWhId = RES_CHRATR_CJK_FONTSIZE;
        nCTLWhId = RES_CHRATR_CTL_FONTSIZE;
        break;
    case RES_CHRATR_POSTURE:
        nCJKWhId = RES_CHRATR_CJK_POSTURE;
        nCTLWhId = RES_CHRATR_CTL_POSTURE;
        break;
    case RES_CHRATR_WEIGHT:
        nCJKWhId = RES_CHRATR_CJK_WEIGHT;
        nCTLWhId = RES_CHRATR_CTL_WEIGHT;
        break;
    case RES_CHRATR_LANGUAGE:
        nCJKWhId = RES_CHRATR_CJK_LANGUAGE;
        nCTLWhId = RES_CHRATR_CTL_LANGUAGE;
        break;

    default:
        ASSERT( !this, "wrong WhichId for this class" );
        nWhId = 0;
    }
}


const SfxItemSet* GetLatinAsianComplexAttr::GetItemSet( USHORT nScriptType,
                                                const SfxItemSet* pGetSet )
{
    if( !pGetSet )
        pGetSet = pSet;

    const USHORT nArrLen = 3;
    struct {
        const SfxPoolItem* pItem;
        USHORT nWhichId, nClearId;
    } aItemArr[ nArrLen ] = {
        { 0, nWhId, nWhId },
        { 0, nCJKWhId, nCJKWhId },
        { 0, nCTLWhId, nCTLWhId }
    };


    for( USHORT nArrIdx = 0; nArrIdx < nArrLen; ++nArrIdx )
    {
        USHORT nWId = aItemArr[ nArrIdx ].nWhichId;
        SfxItemState eState = pGetSet->GetItemState( nWId, FALSE,
                                            &aItemArr[ nArrIdx ].pItem );
        if( SFX_ITEM_DONTCARE == eState )
            aItemArr[ nArrIdx ].pItem = 0;
        else if( SFX_ITEM_SET != eState || pGetSet != pSet )
        {
            pSet->Put( pSet->Get( nWId ));
            aItemArr[ nArrIdx ].pItem = &pSet->Get( nWId );
        }
    }

#define SCRIPTTYPE_NONE		0
#define SCRIPTTYPE_LATIN	1
#define SCRIPTTYPE_ASIAN	2
#define SCRIPTTYPE_COMPLEX	4

    switch( nScriptType )
    {
    case SCRIPTTYPE_NONE:		//no one valid -> match to latin
    case SCRIPTTYPE_LATIN:
        if( aItemArr[ 0 ].pItem )
            aItemArr[ 0 ].nClearId = 0;
        break;
    case SCRIPTTYPE_ASIAN:
        if( aItemArr[ 1 ].pItem )
            aItemArr[ 1 ].nClearId = 0;
        break;
    case SCRIPTTYPE_COMPLEX:
        if( aItemArr[ 2 ].pItem )
            aItemArr[ 2 ].nClearId = 0;
        break;

    case SCRIPTTYPE_LATIN|SCRIPTTYPE_ASIAN:
        if(  aItemArr[ 0 ].pItem &&  aItemArr[ 1 ].pItem &&
            *aItemArr[ 0 ].pItem == *aItemArr[ 1 ].pItem )
            aItemArr[ 0 ].nClearId = aItemArr[ 1 ].nClearId = 0;
        break;

    case SCRIPTTYPE_LATIN|SCRIPTTYPE_COMPLEX:
        if(  aItemArr[ 0 ].pItem &&  aItemArr[ 2 ].pItem &&
            *aItemArr[ 0 ].pItem == *aItemArr[ 2 ].pItem )
            aItemArr[ 0 ].nClearId = aItemArr[ 2 ].nClearId = 0;
        break;

    case SCRIPTTYPE_ASIAN|SCRIPTTYPE_COMPLEX:
        if(  aItemArr[ 1 ].pItem &&  aItemArr[ 2 ].pItem &&
            *aItemArr[ 1 ].pItem == *aItemArr[ 2 ].pItem )
            aItemArr[ 1 ].nClearId = aItemArr[ 2 ].nClearId = 0;
        break;

    case SCRIPTTYPE_LATIN|SCRIPTTYPE_ASIAN|SCRIPTTYPE_COMPLEX:
        if( aItemArr[0].pItem && aItemArr[1].pItem && aItemArr[2].pItem &&
            *aItemArr[0].pItem == *aItemArr[1].pItem &&
            *aItemArr[0].pItem == *aItemArr[2].pItem )
            aItemArr[0].nClearId = aItemArr[1].nClearId =
                aItemArr[2].nClearId = 0;
        break;
    }

    for( nArrIdx = 0; nArrIdx < nArrLen; ++nArrIdx )
    {
        USHORT nWId = aItemArr[nArrIdx].nClearId;
        if( nWId )
            pSet->ClearItem( nWId );
    }
    if( !pSet->Count() )
        delete pSet, pSet = 0;

    return pSet;
}

const SfxItemSet* GetLatinAsianComplexAttr::GetItemSet(USHORT nScriptType)
{
    return ( pSet && nWhId ) ? GetItemSet( nScriptType, 0 ) : 0;
}

const SfxItemSet* GetLatinAsianComplexAttr::GetItemSet( USHORT nScriptType,
                                                const SfxItemSet& rSet )
{
    if( !pSet && nWhId )
        pSet = new SfxItemSet( *rSet.GetPool(), nWhId, nWhId,
                                        nCJKWhId, nCJKWhId,
                                        nCTLWhId, nCTLWhId,
                                        0 );
    return nWhId ? GetItemSet( nScriptType, &rSet ) : 0;
}


