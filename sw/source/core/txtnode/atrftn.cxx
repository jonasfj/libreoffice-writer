/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: atrftn.cxx,v $
 * $Revision: 1.9 $
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



#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <svtools/svstdarr.hxx>
#include <doc.hxx>
#include <cntfrm.hxx>       // ASSERT in ~SwTxtFtn()
#include <pagefrm.hxx>      // RemoveFtn()
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <ftnidx.hxx>
#include <ftninfo.hxx>
#include <swfont.hxx>
#include <ndtxt.hxx>
#include <poolfmt.hxx>
#include <ftnfrm.hxx>
#include <ndindex.hxx>
#include <fmtftntx.hxx>
#include <section.hxx>

/*************************************************************************
|*
|*    class SwFmtFtn
|*
|*    Beschreibung
|*    Ersterstellung    JP 09.08.94
|*    Letzte Aenderung  JP 08.08.94
|*
*************************************************************************/


SwFmtFtn::SwFmtFtn( BOOL bEN )
    : SfxPoolItem( RES_TXTATR_FTN ),
    pTxtAttr( 0 ),
    nNumber( 0 ),
    bEndNote( bEN )
{
}


int SwFmtFtn::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    return nNumber	== ((SwFmtFtn&)rAttr).nNumber &&
           aNumber	== ((SwFmtFtn&)rAttr).aNumber &&
           bEndNote	== ((SwFmtFtn&)rAttr).bEndNote;
}


SfxPoolItem* SwFmtFtn::Clone( SfxItemPool* ) const
{
    SwFmtFtn* pNew	= new SwFmtFtn;
    pNew->aNumber	= aNumber;
    pNew->nNumber	= nNumber;
    pNew->bEndNote	= bEndNote;
    return pNew;
}

void SwFmtFtn::SetEndNote( BOOL b )
{
    if ( b != bEndNote )
    {
        if ( GetTxtFtn() )
            GetTxtFtn()->DelFrms();
        bEndNote = b;
    }
}

SwFmtFtn::~SwFmtFtn()
{
}


void SwFmtFtn::GetFtnText( XubString& rStr ) const
{
    if( pTxtAttr->GetStartNode() )
    {
        SwNodeIndex aIdx( *pTxtAttr->GetStartNode(), 1 );
        SwCntntNode* pCNd = aIdx.GetNode().GetTxtNode();
        if( !pCNd )
            pCNd = aIdx.GetNodes().GoNext( &aIdx );

        if( pCNd->IsTxtNode() )
            rStr = ((SwTxtNode*)pCNd)->GetExpandTxt();
    }
}

    // returnt den anzuzeigenden String der Fuss-/Endnote
XubString SwFmtFtn::GetViewNumStr( const SwDoc& rDoc, BOOL bInclStrings ) const
{
    XubString sRet( GetNumStr() );
    if( !sRet.Len() )
    {
        // dann ist die Nummer von Interesse, also ueber die Info diese
        // besorgen.
        BOOL bMakeNum = TRUE;
        const SwSectionNode* pSectNd = pTxtAttr
                    ? SwUpdFtnEndNtAtEnd::FindSectNdWithEndAttr( *pTxtAttr )
                    : 0;

        if( pSectNd )
        {
            const SwFmtFtnEndAtTxtEnd& rFtnEnd = (SwFmtFtnEndAtTxtEnd&)
                pSectNd->GetSection().GetFmt()->GetFmtAttr(
                                IsEndNote() ?
                                static_cast<USHORT>(RES_END_AT_TXTEND) :
                                static_cast<USHORT>(RES_FTN_AT_TXTEND) );

            if( FTNEND_ATTXTEND_OWNNUMANDFMT == rFtnEnd.GetValue() )
            {
                bMakeNum = FALSE;
                sRet = rFtnEnd.GetSwNumType().GetNumStr( GetNumber() );
                if( bInclStrings )
                {
                    sRet.Insert( rFtnEnd.GetPrefix(), 0 );
                    sRet += rFtnEnd.GetSuffix();
                }
            }
        }

        if( bMakeNum )
        {
            const SwEndNoteInfo* pInfo;
            if( IsEndNote() )
                pInfo = &rDoc.GetEndNoteInfo();
            else
                pInfo = &rDoc.GetFtnInfo();
            sRet = pInfo->aFmt.GetNumStr( GetNumber() );
            if( bInclStrings )
            {
                sRet.Insert( pInfo->GetPrefix(), 0 );
                sRet += pInfo->GetSuffix();
            }
        }
    }
    return sRet;
}

/*************************************************************************
 *						class SwTxt/FmtFnt
 *************************************************************************/

SwTxtFtn::SwTxtFtn( const SwFmtFtn& rAttr, xub_StrLen nStartPos )
    : SwTxtAttr( rAttr, nStartPos ),
    pStartNode( 0 ),
    pMyTxtNd( 0 ),
    nSeqNo( USHRT_MAX )
{
    ((SwFmtFtn&)rAttr).pTxtAttr = this;
}


SwTxtFtn::~SwTxtFtn()
{
    SetStartNode( 0 );
}



void SwTxtFtn::SetStartNode( const SwNodeIndex *pNewNode, BOOL bDelNode )
{
    if( pNewNode )
    {
        if( !pStartNode )
            pStartNode = new SwNodeIndex( *pNewNode );
        else
            *pStartNode = *pNewNode;
    }
    else if( pStartNode )
    {
        // Zwei Dinge muessen erledigt werden:
        // 1) Die Fussnoten muessen bei ihren Seiten abgemeldet werden
        // 2) Die Fussnoten-Sektion in den Inserts muss geloescht werden.
        SwDoc* pDoc;
        if( pMyTxtNd )
            pDoc = pMyTxtNd->GetDoc();
        else
        {
            //JP 27.01.97: der sw3-Reader setzt einen StartNode aber das
            //				Attribut ist noch nicht im TextNode verankert.
            //				Wird es geloescht (z.B. bei Datei einfuegen mit
            //				Ftn in einen Rahmen), muss auch der Inhalt
            //				geloescht werden
            pDoc = pStartNode->GetNodes().GetDoc();
        }

        // Wir duerfen die Fussnotennodes nicht loeschen
        // und brauchen die Fussnotenframes nicht loeschen, wenn
        // wir im ~SwDoc() stehen.
        if( !pDoc->IsInDtor() )
        {
            if( bDelNode )
            {
                // 1) Die Section fuer die Fussnote wird beseitigt
                // Es kann sein, dass die Inserts schon geloescht wurden.
                pDoc->DeleteSection( &pStartNode->GetNode() );
            }
            else
                // Werden die Nodes nicht geloescht mussen sie bei den Seiten
                // abmeldet (Frms loeschen) werden, denn sonst bleiben sie
                // stehen (Undo loescht sie nicht!)
                DelFrms();
        }
        DELETEZ( pStartNode );

        // loesche die Fussnote noch aus dem Array am Dokument
        for( USHORT n = 0; n < pDoc->GetFtnIdxs().Count(); ++n )
            if( this == pDoc->GetFtnIdxs()[n] )
            {
                pDoc->GetFtnIdxs().Remove( n );
                // gibt noch weitere Fussnoten
                if( !pDoc->IsInDtor() && n < pDoc->GetFtnIdxs().Count() )
                {
                    SwNodeIndex aTmp( pDoc->GetFtnIdxs()[n]->GetTxtNode() );
                    pDoc->GetFtnIdxs().UpdateFtn( aTmp );
                }
                break;
            }
    }
}


void SwTxtFtn::SetNumber( const USHORT nNewNum, const XubString* pStr )
{
    SwFmtFtn& rFtn = (SwFmtFtn&)GetFtn();
    if( pStr && pStr->Len() )
        rFtn.aNumber = *pStr;
    else
    {
        rFtn.nNumber = nNewNum;
        rFtn.aNumber = aEmptyStr;
    }

    ASSERT( pMyTxtNd, "wo ist mein TextNode?" );
    SwNodes &rNodes = pMyTxtNd->GetDoc()->GetNodes();
    pMyTxtNd->Modify( 0, &rFtn );
    if( pStartNode )
    {
        // Wir muessen ueber alle TxtNodes iterieren, wegen der
        // Fussnoten, die auf anderen Seiten stehen.
        SwNode* pNd;
        ULONG nSttIdx = pStartNode->GetIndex() + 1,
              nEndIdx = pStartNode->GetNode().EndOfSectionIndex();
        for( ; nSttIdx < nEndIdx; ++nSttIdx )
        {
            // Es koennen ja auch Grafiken in der Fussnote stehen ...
            if( ( pNd = rNodes[ nSttIdx ] )->IsTxtNode() )
                ((SwTxtNode*)pNd)->Modify( 0, &rFtn );
        }
    }
}

// Die Fussnoten duplizieren
void SwTxtFtn::CopyFtn( SwTxtFtn *pDest )
{
    if( pStartNode && pDest->GetStartNode() )
    {
        // die Fussnoten koennen in unterschiedlichen Dokumenten stehen !!
        SwDoc* pDstDoc = pDest->pMyTxtNd->GetDoc();
        SwNodes &rDstNodes = pDstDoc->GetNodes();

        // Wir kopieren nur den Inhalt der Sektion
        SwNodeRange aRg( *pStartNode, 1,
                    *pStartNode->GetNode().EndOfSectionNode() );

        // Wir fuegen auf dem Ende von pDest ein, d.h. die Nodes
        // werden angehaengt. nDestLen haelt die Anzahl der CntNodes
        // in pDest _vor_ dem Kopieren.
        SwNodeIndex aStart( *(pDest->GetStartNode()) );
        SwNodeIndex aEnd( *aStart.GetNode().EndOfSectionNode() );
        ULONG  nDestLen = aEnd.GetIndex() - aStart.GetIndex() - 1;

        pMyTxtNd->GetDoc()->CopyWithFlyInFly( aRg, aEnd, TRUE );

        // Wenn die Dest-Sektion nicht leer war, so muessen die alten
        // Nodes geloescht werden:
        // Vorher:   Src: SxxxE,  Dst: SnE
        // Nachher:  Src: SxxxE,  Dst: SnxxxE
        // und       Src: SxxxE,  Dst: SxxxE
        aStart++;
        rDstNodes.Delete( aStart, nDestLen );
    }

    // Der benutzerdefinierte String muss auch uebertragen werden.
    if( GetFtn().aNumber.Len() )
        ((SwFmtFtn&)pDest->GetFtn()).aNumber = GetFtn().aNumber;
}


    // lege eine neue leere TextSection fuer diese Fussnote an
void SwTxtFtn::MakeNewTextSection( SwNodes& rNodes )
{
    if( pStartNode )
        return;

    // Nun verpassen wir dem TxtNode noch die Fussnotenvorlage.
    SwTxtFmtColl *pFmtColl;
    const SwEndNoteInfo* pInfo;
    USHORT nPoolId;

    if( GetFtn().IsEndNote() )
    {
        pInfo = &rNodes.GetDoc()->GetEndNoteInfo();
        nPoolId = RES_POOLCOLL_ENDNOTE;
    }
    else
    {
        pInfo = &rNodes.GetDoc()->GetFtnInfo();
        nPoolId = RES_POOLCOLL_FOOTNOTE;
    }

    if( 0 == (pFmtColl = pInfo->GetFtnTxtColl() ) )
        pFmtColl = rNodes.GetDoc()->GetTxtCollFromPool( nPoolId );

    SwStartNode* pSttNd = rNodes.MakeTextSection( SwNodeIndex( rNodes.GetEndOfInserts() ),
                                        SwFootnoteStartNode, pFmtColl );
    pStartNode = new SwNodeIndex( *pSttNd );
}


void SwTxtFtn::DelFrms()
{
    // loesche die Ftn-Frames aus den Seiten
    ASSERT( pMyTxtNd, "wo ist mein TextNode?" );
    if( !pMyTxtNd )
        return ;

    BOOL bFrmFnd = FALSE;
    {
        SwClientIter aIter( *pMyTxtNd );
        for( SwCntntFrm* pFnd = (SwCntntFrm*)aIter.First( TYPE( SwCntntFrm ));
                pFnd; pFnd = (SwCntntFrm*)aIter.Next() )
        {
            SwPageFrm* pPage = pFnd->FindPageFrm();
            if( pPage )
            {
                pPage->RemoveFtn( pFnd, this );
                bFrmFnd = TRUE;
            }
        }
    }
    //JP 13.05.97: falls das Layout vorm loeschen der Fussnoten entfernt
    //				wird, sollte man das ueber die Fussnote selbst tun
    if( !bFrmFnd && pStartNode )
    {
        SwNodeIndex aIdx( *pStartNode );
        SwCntntNode* pCNd = pMyTxtNd->GetNodes().GoNext( &aIdx );
        if( pCNd )
        {
            SwClientIter aIter( *pCNd );
            for( SwCntntFrm* pFnd = (SwCntntFrm*)aIter.First( TYPE( SwCntntFrm ));
                    pFnd; pFnd = (SwCntntFrm*)aIter.Next() )
            {
                SwPageFrm* pPage = pFnd->FindPageFrm();

                SwFrm *pFrm = pFnd->GetUpper();
                while ( pFrm && !pFrm->IsFtnFrm() )
                    pFrm = pFrm->GetUpper();

                SwFtnFrm *pFtn = (SwFtnFrm*)pFrm;
                while ( pFtn && pFtn->GetMaster() )
                    pFtn = pFtn->GetMaster();
                ASSERT( pFtn->GetAttr() == this, "Ftn mismatch error." );

                while ( pFtn )
                {
                    SwFtnFrm *pFoll = pFtn->GetFollow();
                    pFtn->Cut();
                    delete pFtn;
                    pFtn = pFoll;
                }

                // #i20556# During hiding of a section, the connection
                // to the layout is already lost. pPage may be 0:
                if ( pPage )
                    pPage->UpdateFtnNum();
            }
        }
    }
}


USHORT SwTxtFtn::SetSeqRefNo()
{
    if( !pMyTxtNd )
        return USHRT_MAX;

    SwDoc* pDoc = pMyTxtNd->GetDoc();
    if( pDoc->IsInReading() )
        return USHRT_MAX;

    USHORT n, nFtnCnt = pDoc->GetFtnIdxs().Count();

    const BYTE nTmp = 255 < nFtnCnt ? 255 : static_cast<BYTE>(nFtnCnt);
    SvUShortsSort aArr( nTmp, nTmp );

    // dann testmal, ob die Nummer schon vergeben ist oder ob eine neue
    // bestimmt werden muss.
    SwTxtFtn* pTxtFtn;
    for( n = 0; n < nFtnCnt; ++n )
        if( (pTxtFtn = pDoc->GetFtnIdxs()[ n ]) != this )
            aArr.Insert( pTxtFtn->nSeqNo );

    // teste erstmal ob die Nummer schon vorhanden ist:
    if( USHRT_MAX != nSeqNo )
    {
        for( n = 0; n < aArr.Count(); ++n )
            if( aArr[ n ] > nSeqNo )
                return nSeqNo;			// nicht vorhanden -> also benutzen
            else if( aArr[ n ] == nSeqNo )
                break;					// schon vorhanden -> neue erzeugen

        if( n == aArr.Count() )
            return nSeqNo;			// nicht vorhanden -> also benutzen
    }

    // alle Nummern entsprechend geflag, also bestimme die richtige Nummer
    for( n = 0; n < aArr.Count(); ++n )
        if( n != aArr[ n ] )
            break;

    return nSeqNo = n;
}

void SwTxtFtn::SetUniqueSeqRefNo( SwDoc& rDoc )
{
    USHORT n, nStt = 0, nFtnCnt = rDoc.GetFtnIdxs().Count();

    const BYTE nTmp = 255 < nFtnCnt ? 255 : static_cast<BYTE>(nFtnCnt);
    SvUShortsSort aArr( nTmp, nTmp );

    // dann alle Nummern zusammensammeln die schon existieren
    SwTxtFtn* pTxtFtn;
    for( n = 0; n < nFtnCnt; ++n )
        if( USHRT_MAX != (pTxtFtn = rDoc.GetFtnIdxs()[ n ])->nSeqNo )
            aArr.Insert( pTxtFtn->nSeqNo );


    for( n = 0; n < nFtnCnt; ++n )
        if( USHRT_MAX == (pTxtFtn = rDoc.GetFtnIdxs()[ n ])->nSeqNo )
        {
            for( ; nStt < aArr.Count(); ++nStt )
                if( nStt != aArr[ nStt ] )
                {

                    pTxtFtn->nSeqNo = nStt;
                    break;
                }

            if( USHRT_MAX == pTxtFtn->nSeqNo )
                break;	// nichts mehr gefunden
        }

    // alle Nummern schon vergeben, also mit nStt++ weitermachen
    for( ; n < nFtnCnt; ++n )
        if( USHRT_MAX == (pTxtFtn = rDoc.GetFtnIdxs()[ n ])->nSeqNo )
            pTxtFtn->nSeqNo = nStt++;
}

void SwTxtFtn::CheckCondColl()
{
//FEATURE::CONDCOLL
    if( GetStartNode() )
        ((SwStartNode&)GetStartNode()->GetNode()).CheckSectionCondColl();
//FEATURE::CONDCOLL
}




