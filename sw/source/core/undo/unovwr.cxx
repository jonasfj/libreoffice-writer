/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unovwr.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: rt $ $Date: 2006-12-01 15:49:54 $
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


#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer die UndoIds
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _ROLBCK_HXX
#include <rolbck.hxx>
#endif
#ifndef _ACORRECT_HXX
#include <acorrect.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif

#include <tools/resid.hxx>
#include <comcore.hrc> // #111827#
#include <undo.hrc>

using namespace ::com::sun::star::i18n;
using namespace ::com::sun::star::uno;

//------------------------------------------------------------------

// zwei Zugriffs-Funktionen
inline SwPosition* IterPt( SwUndoIter& rUIter )
{	return rUIter.pAktPam->GetPoint();	}
inline SwPosition* IterMk( SwUndoIter& rUIter )
{	return rUIter.pAktPam->GetMark();	}

inline SwDoc& SwUndoIter::GetDoc() const { return *pAktPam->GetDoc(); }

//------------------------------------------------------------


// OVERWRITE


SwUndoOverwrite::SwUndoOverwrite( SwDoc* pDoc, SwPosition& rPos,
                                    sal_Unicode cIns )
    : SwUndo(UNDO_OVERWRITE), bGroup( FALSE ),
      pRedlSaveData( 0 )
{
    if( !pDoc->IsIgnoreRedline() && pDoc->GetRedlineTbl().Count() )
    {
        SwPaM aPam( rPos.nNode, rPos.nContent.GetIndex(),
                    rPos.nNode, rPos.nContent.GetIndex()+1 );
        pRedlSaveData = new SwRedlineSaveDatas;
        if( !FillSaveData( aPam, *pRedlSaveData, FALSE ))
            delete pRedlSaveData, pRedlSaveData = 0;
    }

    nSttNode = rPos.nNode.GetIndex();
    nSttCntnt = rPos.nContent.GetIndex();

    SwTxtNode* pTxtNd = rPos.nNode.GetNode().GetTxtNode();
    ASSERT( pTxtNd, "Overwrite nicht im TextNode?" );

    bInsChar = TRUE;
    xub_StrLen nTxtNdLen = pTxtNd->GetTxt().Len();
    if( nSttCntnt < nTxtNdLen )		// kein reines Einfuegen ?
    {
        aDelStr.Insert( pTxtNd->GetTxt().GetChar( nSttCntnt ) );
        if( !pHistory )
            pHistory = new SwHistory;
        SwRegHistory aRHst( *pTxtNd, pHistory );
        pHistory->CopyAttr( pTxtNd->GetpSwpHints(), nSttNode, 0,
                            nTxtNdLen, FALSE );
        rPos.nContent++;
        bInsChar = FALSE;
    }

    BOOL bOldExpFlg = pTxtNd->IsIgnoreDontExpand();
    pTxtNd->SetIgnoreDontExpand( TRUE );

    pTxtNd->Insert( cIns, rPos.nContent );
    aInsStr.Insert( cIns );

    if( !bInsChar )
    {
        const SwIndex aTmpIndex( rPos.nContent, -2 );
        pTxtNd->Erase( aTmpIndex, 1 );
    }
    pTxtNd->SetIgnoreDontExpand( bOldExpFlg );

    bCacheComment = false;
}

SwUndoOverwrite::~SwUndoOverwrite()
{
    delete pRedlSaveData;
}

BOOL SwUndoOverwrite::CanGrouping( SwDoc* pDoc, SwPosition& rPos,
                                    sal_Unicode cIns )
{
///  ?? was ist mit nur eingefuegten Charaktern ???

    // es kann nur das Loeschen von einzelnen char's zusammengefasst werden
    if( rPos.nNode != nSttNode || !aInsStr.Len()  ||
        ( !bGroup && aInsStr.Len() != 1 ))
        return FALSE;

    // ist der Node ueberhaupt ein TextNode?
    SwTxtNode * pDelTxtNd = rPos.nNode.GetNode().GetTxtNode();
    if( !pDelTxtNd ||
        ( pDelTxtNd->GetTxt().Len() != rPos.nContent.GetIndex() &&
            rPos.nContent.GetIndex() != ( nSttCntnt + aInsStr.Len() )))
        return FALSE;

    CharClass& rCC = GetAppCharClass();

    // befrage das einzufuegende Charakter
    if( ( CH_TXTATR_BREAKWORD == cIns && CH_TXTATR_INWORD == cIns ) ||
        rCC.isLetterNumeric( String( cIns ), 0 ) !=
        rCC.isLetterNumeric( aInsStr, aInsStr.Len()-1 ) )
        return FALSE;

    {
        SwRedlineSaveDatas* pTmpSav = new SwRedlineSaveDatas;
        SwPaM aPam( rPos.nNode, rPos.nContent.GetIndex(),
                    rPos.nNode, rPos.nContent.GetIndex()+1 );

        if( !FillSaveData( aPam, *pTmpSav, FALSE ))
            delete pTmpSav, pTmpSav = 0;

        BOOL bOk = ( !pRedlSaveData && !pTmpSav ) ||
                   ( pRedlSaveData && pTmpSav &&
                        SwUndo::CanRedlineGroup( *pRedlSaveData, *pTmpSav,
                            nSttCntnt > rPos.nContent.GetIndex() ));
        delete pTmpSav;
        if( !bOk )
            return FALSE;

        pDoc->DeleteRedline( aPam, false, USHRT_MAX );
    }

    // Ok, die beiden 'Overwrites' koennen zusammen gefasst werden, also
    // 'verschiebe' das enstprechende Zeichen
    if( !bInsChar )
    {
        if( rPos.nContent.GetIndex() < pDelTxtNd->GetTxt().Len() )
        {
            aDelStr.Insert( pDelTxtNd->GetTxt().GetChar(rPos.nContent.GetIndex()) );
            rPos.nContent++;
        }
        else
            bInsChar = TRUE;
    }

    BOOL bOldExpFlg = pDelTxtNd->IsIgnoreDontExpand();
    pDelTxtNd->SetIgnoreDontExpand( TRUE );

    pDelTxtNd->Insert( cIns, rPos.nContent );
    aInsStr.Insert( cIns );

    if( !bInsChar )
    {
        const SwIndex aTmpIndex( rPos.nContent, -2 );
        pDelTxtNd->Erase( aTmpIndex, 1 );
    }
    pDelTxtNd->SetIgnoreDontExpand( bOldExpFlg );

    bGroup = TRUE;
    return TRUE;
}





void SwUndoOverwrite::Undo( SwUndoIter& rUndoIter )
{
    SwPaM* pAktPam = rUndoIter.pAktPam;
    SwDoc* pDoc = pAktPam->GetDoc();
    pAktPam->DeleteMark();
    pAktPam->GetPoint()->nNode = nSttNode;
    SwTxtNode* pTxtNd = pAktPam->GetNode()->GetTxtNode();
    ASSERT( pTxtNd, "Overwrite nicht im TextNode?" );
    SwIndex& rIdx = pAktPam->GetPoint()->nContent;
    rIdx.Assign( pTxtNd, nSttCntnt );

    SwAutoCorrExceptWord* pACEWord = pDoc->GetAutoCorrExceptWord();
    if( pACEWord )
    {
        if( 1 == aInsStr.Len() && 1 == aDelStr.Len() )
            pACEWord->CheckChar( *pAktPam->GetPoint(), aDelStr.GetChar( 0 ) );
        pDoc->SetAutoCorrExceptWord( 0 );
    }

    // wurde nicht nur ueberschieben sondern auch geinsertet, so loesche
    // den Ueberhang
    if( aInsStr.Len() > aDelStr.Len() )
    {
        rIdx += aDelStr.Len();
        pTxtNd->Erase( rIdx, aInsStr.Len() - aDelStr.Len() );
        rIdx = nSttCntnt;
    }

    if( aDelStr.Len() )
    {
        String aTmpStr( '1' );
        sal_Unicode* pTmpStr = aTmpStr.GetBufferAccess();

        BOOL bOldExpFlg = pTxtNd->IsIgnoreDontExpand();
        pTxtNd->SetIgnoreDontExpand( TRUE );

        rIdx++;
        for( xub_StrLen n = 0; n < aDelStr.Len(); n++  )
        {
            // einzeln, damit die Attribute stehen bleiben !!!
            *pTmpStr = aDelStr.GetChar( n );
            pTxtNd->Insert( aTmpStr, rIdx /*???, SETATTR_NOTXTATRCHR*/ );
            rIdx -= 2;
            pTxtNd->Erase( rIdx, 1 );
            rIdx += 2;
        }
        pTxtNd->SetIgnoreDontExpand( bOldExpFlg );
        rIdx--;
    }
    if( pHistory )
    {
        if( pTxtNd->GetpSwpHints() )
            pTxtNd->ClearSwpHintsArr( false );
        pHistory->TmpRollback( pDoc, 0, FALSE );
    }

    if( pAktPam->GetMark()->nContent.GetIndex() != nSttCntnt )
    {
        pAktPam->SetMark();
        pAktPam->GetMark()->nContent = nSttCntnt;
    }

    if( pRedlSaveData )
        SetSaveData( *pDoc, *pRedlSaveData );
}

void SwUndoOverwrite::Repeat( SwUndoIter& rUndoIter )
{
    rUndoIter.pLastUndoObj = this;
    SwPaM* pAktPam = rUndoIter.pAktPam;
    if( !aInsStr.Len() || pAktPam->HasMark() )
        return;

    SwDoc& rDoc = *pAktPam->GetDoc();

    BOOL bGroupUndo = rDoc.DoesGroupUndo();
    rDoc.DoGroupUndo( FALSE );
    rDoc.Overwrite( *pAktPam, aInsStr.GetChar( 0 ));
    rDoc.DoGroupUndo( bGroupUndo );
    for( xub_StrLen n = 1; n < aInsStr.Len(); ++n )
        rDoc.Overwrite( *pAktPam, aInsStr.GetChar( n ) );
}



void SwUndoOverwrite::Redo( SwUndoIter& rUndoIter )
{
    SwPaM* pAktPam = rUndoIter.pAktPam;
    SwDoc* pDoc = pAktPam->GetDoc();
    pAktPam->DeleteMark();
    pAktPam->GetPoint()->nNode = nSttNode;
    SwTxtNode* pTxtNd = pAktPam->GetNode()->GetTxtNode();
    ASSERT( pTxtNd, "Overwrite nicht im TextNode?" );
    SwIndex& rIdx = pAktPam->GetPoint()->nContent;

    if( pRedlSaveData )
    {
        rIdx.Assign( pTxtNd, nSttCntnt );
        pAktPam->SetMark();
        pAktPam->GetMark()->nContent += aInsStr.Len();
        pDoc->DeleteRedline( *pAktPam, false, USHRT_MAX );
        pAktPam->DeleteMark();
    }
    rIdx.Assign( pTxtNd, aDelStr.Len() ? nSttCntnt+1 : nSttCntnt );

    BOOL bOldExpFlg = pTxtNd->IsIgnoreDontExpand();
    pTxtNd->SetIgnoreDontExpand( TRUE );

    for( xub_StrLen n = 0; n < aInsStr.Len(); n++  )
    {
        // einzeln, damit die Attribute stehen bleiben !!!
        pTxtNd->Insert( aInsStr.GetChar( n ), rIdx );
        if( n < aDelStr.Len() )
        {
            rIdx -= 2;
            pTxtNd->Erase( rIdx, 1 );
            rIdx += n+1 < aDelStr.Len() ? 2 : 1;
        }
    }
    pTxtNd->SetIgnoreDontExpand( bOldExpFlg );

    // alte Anfangs-Position vom UndoNodes-Array zurueckholen
    if( pHistory )
        pHistory->SetTmpEnd( pHistory->Count() );
    if( pAktPam->GetMark()->nContent.GetIndex() != nSttCntnt )
    {
        pAktPam->SetMark();
        pAktPam->GetMark()->nContent = nSttCntnt;
    }
}

SwRewriter SwUndoOverwrite::GetRewriter() const
{
    SwRewriter aResult;

    String aString;

    aString += String(SW_RES(STR_START_QUOTE));
    aString += ShortenString(aInsStr, nUndoStringLength,
                             String(SW_RES(STR_LDOTS)));
    aString += String(SW_RES(STR_END_QUOTE));

    aResult.AddRule(UNDO_ARG1, aString);

    return aResult;
}

//------------------------------------------------------------

struct _UndoTransliterate_Data
{
    String sText;
    _UndoTransliterate_Data* pNext;
    SwHistory* pHistory;
    Sequence <sal_Int32>* pOffsets;
    ULONG nNdIdx;
    xub_StrLen nStart, nLen;

    _UndoTransliterate_Data( ULONG nNd, xub_StrLen nStt, xub_StrLen nStrLen,
                                const String& rTxt )
        : pNext( 0 ), pOffsets( 0 ), pHistory( 0 ), sText( rTxt ),
        nStart( nStt ), nNdIdx( nNd ), nLen( nStrLen )
    {}
    ~_UndoTransliterate_Data() { delete pOffsets; delete pHistory; }

    void SetChangeAtNode( SwDoc& rDoc );
};

SwUndoTransliterate::SwUndoTransliterate( const SwPaM& rPam,
                            const utl::TransliterationWrapper& rTrans )
    : SwUndo( UNDO_TRANSLITERATE ), SwUndRng( rPam ),
    nType( rTrans.getType() ), pData( 0 ), pLastData( 0 )
{
}

SwUndoTransliterate::~SwUndoTransliterate()
{
    _UndoTransliterate_Data* pD = pData;
    while( pD )
    {
        pData = pD;
        pD = pD->pNext;
        delete pData;
    }
}

void SwUndoTransliterate::Undo( SwUndoIter& rUndoIter )
{
    SwDoc& rDoc = rUndoIter.GetDoc();
    BOOL bUndo = rDoc.DoesUndo();
    rDoc.DoUndo( FALSE );

    for( _UndoTransliterate_Data* pD = pData; pD; pD = pD->pNext )
        pD->SetChangeAtNode( rDoc );

    rDoc.DoUndo( bUndo );
    SetPaM( rUndoIter, TRUE );
}

void SwUndoTransliterate::Redo( SwUndoIter& rUndoIter )
{
/* ??? */	rUndoIter.SetUpdateAttr( TRUE );

    SetPaM( *rUndoIter.pAktPam );
    Repeat( rUndoIter );
}

void SwUndoTransliterate::Repeat( SwUndoIter& rUndoIter )
{
    SwPaM& rPam = *rUndoIter.pAktPam;
    SwDoc& rDoc = rUndoIter.GetDoc();

    utl::TransliterationWrapper aTrans(
                        ::comphelper::getProcessServiceFactory(), nType );
    rDoc.TransliterateText( rPam, aTrans );

    rUndoIter.pLastUndoObj = this;
}

void SwUndoTransliterate::AddChanges( const SwTxtNode& rTNd,
                    xub_StrLen nStart, xub_StrLen nLen,
                     ::com::sun::star::uno::Sequence <sal_Int32>& rOffsets )
{
    long nOffsLen = rOffsets.getLength();
    _UndoTransliterate_Data* pNew = new _UndoTransliterate_Data(
                        rTNd.GetIndex(), nStart, (xub_StrLen)nOffsLen,
                        rTNd.GetTxt().Copy( nStart, nLen ));
    if( pData )
        pLastData->pNext = pNew;
    else
        pData = pNew;
    pLastData = pNew;

    const sal_Int32* pOffsets = rOffsets.getConstArray();
    // where did we need less memory ?
    // check for 1-1 mappings:
    BOOL bOneToOne = TRUE;
    const sal_Int32* p = pOffsets;
    for( long n = 0; n < nOffsLen; ++n, ++p )
        if( *p != ( nStart + n ))
        {
            // create the Offset array
            pNew->pOffsets = new Sequence <sal_Int32> ( nLen );
            sal_Int32* pIdx = pNew->pOffsets->getArray();
            p = pOffsets;
            long nMyOff, nNewVal = nStart;
            for( n = 0, nMyOff = nStart; n < nOffsLen; ++p, ++n, ++nMyOff )
            {
                if( *p < nMyOff )
                {
                    // something is deleted
                    nMyOff = *p;
                    *(pIdx-1) = nNewVal++;
                }
                else if( *p > nMyOff )
                {
                    for( ; *p > nMyOff; ++nMyOff )
                        *pIdx++ = nNewVal;
                    --nMyOff;
                    --n;
                    --p;
                }
                else
                    *pIdx++ = nNewVal++;
            }

            // and then we need to save the attributes/bookmarks
            // but this data must moved every time to the last in the chain!
            _UndoTransliterate_Data* pD = pData;
            while( pD != pNew )
            {
                if( pD->nNdIdx == pNew->nNdIdx && pD->pHistory )
                {
                    // same node and have a history?
                    pNew->pHistory = pD->pHistory;
                    pD->pHistory = 0;
                    break;		    // more can't exist
                }
                pD = pD->pNext;
            }

            if( !pNew->pHistory )
            {
                pNew->pHistory = new SwHistory;
                SwRegHistory aRHst( rTNd, pNew->pHistory );
                pNew->pHistory->CopyAttr( rTNd.GetpSwpHints(),
                        pNew->nNdIdx, 0, rTNd.GetTxt().Len(), FALSE );
            }
            break;
        }
}

void _UndoTransliterate_Data::SetChangeAtNode( SwDoc& rDoc )
{
    SwTxtNode* pTNd = rDoc.GetNodes()[ nNdIdx ]->GetTxtNode();
    if( pTNd )
    {
        Sequence <sal_Int32> aOffsets( pOffsets ? pOffsets->getLength() : nLen );
        if( pOffsets )
            aOffsets = *pOffsets;
        else
        {
            sal_Int32* p = aOffsets.getArray();
            for( xub_StrLen n = 0; n < nLen; ++n, ++p )
                *p = n + nStart;
        }
        pTNd->ReplaceTextOnly( nStart, nLen, sText, aOffsets );

        if( pHistory )
        {
            if( pTNd->GetpSwpHints() )
                pTNd->ClearSwpHintsArr( false );
            pHistory->TmpRollback( &rDoc, 0, FALSE );
            pHistory->SetTmpEnd( pHistory->Count() );
        }
    }
}


