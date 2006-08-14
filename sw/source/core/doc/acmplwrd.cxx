/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: acmplwrd.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 15:54:28 $
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

#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif
#ifndef _ACMPLWRD_HXX
#include <acmplwrd.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
#ifndef _MySVXACORR_HXX
#include <svx/svxacorr.hxx>
#endif

#include <svx/acorrcfg.hxx>

#include <vector>
/* -----------------------------05.08.2002 12:43------------------------------

 ---------------------------------------------------------------------------*/
class SwAutoCompleteClient : public SwClient
{
    SwAutoCompleteWord* pAutoCompleteWord;
    SwDoc*              pDoc;
#ifdef DBG_UTIL
    static ULONG nSwAutoCompleteClientCount;
#endif
public:
    SwAutoCompleteClient(SwAutoCompleteWord& rToTell, SwDoc& rSwDoc);
    SwAutoCompleteClient(const SwAutoCompleteClient& rClient);
    ~SwAutoCompleteClient();

    SwAutoCompleteClient& operator=(const SwAutoCompleteClient& rClient);

    virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);
    const SwDoc& GetDoc(){return *pDoc;}
#ifdef DBG_UTIL
    static ULONG GetElementCount() {return nSwAutoCompleteClientCount;}
#endif
};
/* -----------------------------05.08.2002 12:48------------------------------

 ---------------------------------------------------------------------------*/
typedef std::vector<SwAutoCompleteClient> SwAutoCompleteClientVector;

class SwAutoCompleteWord_Impl
{
    SwAutoCompleteClientVector  aClientVector;
    SwAutoCompleteWord&         rAutoCompleteWord;
public:
    SwAutoCompleteWord_Impl(SwAutoCompleteWord& rParent) :
        rAutoCompleteWord(rParent){}
    void AddDocument(SwDoc& rDoc);
    void RemoveDocument(const SwDoc& rDoc);
};

/* -----------------------------05.08.2002 14:11------------------------------

 ---------------------------------------------------------------------------*/
typedef const SwDoc* SwDocPtr;
typedef std::vector<SwDocPtr> SwDocPtrVector;
class SwAutoCompleteString : public String
{
#ifdef DBG_UTIL
    static ULONG nSwAutoCompleteStringCount;
#endif
    SwDocPtrVector aSourceDocs;
    public:
        SwAutoCompleteString(const String& rStr, xub_StrLen nPos, xub_StrLen nLen);

        ~SwAutoCompleteString();
        void        AddDocument(const SwDoc& rDoc);
        //returns true if last document reference has been removed
        sal_Bool     RemoveDocument(const SwDoc& rDoc);
#ifdef DBG_UTIL
    static ULONG GetElementCount() {return nSwAutoCompleteStringCount;}
#endif
};
#ifdef DBG_UTIL
    ULONG SwAutoCompleteClient::nSwAutoCompleteClientCount = 0;
    ULONG SwAutoCompleteString::nSwAutoCompleteStringCount = 0;
#endif
/* -----------------------------06.08.2002 08:57------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteClient::SwAutoCompleteClient(SwAutoCompleteWord& rToTell, SwDoc& rSwDoc) :
        pAutoCompleteWord(&rToTell),
        pDoc(&rSwDoc)
{
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
#ifdef DBG_UTIL
    ++nSwAutoCompleteClientCount;
#endif
}
/* -----------------------------05.08.2002 14:07------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteClient::SwAutoCompleteClient(const SwAutoCompleteClient& rClient) :
    pAutoCompleteWord(rClient.pAutoCompleteWord),
    pDoc(rClient.pDoc)
{
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
#ifdef DBG_UTIL
    ++nSwAutoCompleteClientCount;
#endif
}
/* -----------------------------05.08.2002 14:10------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteClient::~SwAutoCompleteClient()
{
#ifdef DBG_UTIL
    --nSwAutoCompleteClientCount;
#endif
}
/* -----------------06.03.2003 15:30-----------------

 --------------------------------------------------*/
SwAutoCompleteClient& SwAutoCompleteClient::operator=(const SwAutoCompleteClient& rClient)
{
    pAutoCompleteWord = rClient.pAutoCompleteWord;
    pDoc = rClient.pDoc;
    if(rClient.GetRegisteredIn())
        rClient.pRegisteredIn->Add(this);
    else if(GetRegisteredIn())
        pRegisteredIn->Remove(this);
    return *this;
}
/* -----------------------------05.08.2002 12:49------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoCompleteClient::Modify(SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    switch( pOld ? pOld->Which() : 0 )
    {
    case RES_REMOVE_UNO_OBJECT:
    case RES_OBJECTDYING:
        if( (void*)GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            ((SwModify*)GetRegisteredIn())->Remove(this);
            pAutoCompleteWord->DocumentDying(*pDoc);
        break;

    }
}
/* -----------------------------05.08.2002 13:03------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoCompleteWord_Impl::AddDocument(SwDoc& rDoc)
{
    SwAutoCompleteClientVector::iterator aIt;
    for(aIt = aClientVector.begin(); aIt != aClientVector.end(); aIt++)
    {
        if(&aIt->GetDoc() == &rDoc)
            return;
    }
    aClientVector.push_back(SwAutoCompleteClient(rAutoCompleteWord, rDoc));
}
/* -----------------------------05.08.2002 14:33------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoCompleteWord_Impl::RemoveDocument(const SwDoc& rDoc)
{
    SwAutoCompleteClientVector::iterator aIt;
    for(aIt = aClientVector.begin(); aIt != aClientVector.end(); aIt++)
    {
        if(&aIt->GetDoc() == &rDoc)
        {
            aClientVector.erase(aIt);
            return;
        }
    }
}
/* -----------------------------06.08.2002 08:54------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteString::SwAutoCompleteString(const String& rStr, xub_StrLen nPos, xub_StrLen nLen) :
            String( rStr, nPos, nLen )
{
#ifdef DBG_UTIL
    ++nSwAutoCompleteStringCount;
#endif
}
/* -----------------------------05.08.2002 14:22------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteString::~SwAutoCompleteString()
{
#ifdef DBG_UTIL
    --nSwAutoCompleteStringCount;
#endif
}
/* -----------------------------05.08.2002 14:17------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoCompleteString::AddDocument(const SwDoc& rDoc)
{
    SwDocPtrVector::iterator aIt;
    for(aIt = aSourceDocs.begin(); aIt != aSourceDocs.end(); aIt++)
    {
        if(*aIt == &rDoc)
            return;
    }
    SwDocPtr pNew = &rDoc;
    aSourceDocs.push_back(pNew);
}
/* -----------------------------05.08.2002 14:36------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwAutoCompleteString::RemoveDocument(const SwDoc& rDoc)
{
    SwDocPtrVector::iterator aIt;
    for(aIt = aSourceDocs.begin(); aIt != aSourceDocs.end(); aIt++)
    {
        if(*aIt == &rDoc)
        {
            aSourceDocs.erase(aIt);
            return !aSourceDocs.size();
        }
    }
    return sal_False;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwAutoCompleteWord::SwAutoCompleteWord( USHORT nWords, USHORT nMWrdLen )
    : aWordLst( 0, 255 ), aLRULst( 0, 255 ),
    pImpl(new SwAutoCompleteWord_Impl(*this)),
    nMaxCount( nWords ),
    nMinWrdLen( nMWrdLen ),
    bLockWordLst( FALSE )
{
}

SwAutoCompleteWord::~SwAutoCompleteWord()
{
    for(USHORT nPos = aWordLst.Count(); nPos; nPos--)
    {
        SwAutoCompleteString* pCurrent = (SwAutoCompleteString*)aWordLst[ nPos - 1 ];
        aWordLst.Remove( nPos - 1 );
        delete pCurrent;
    }
    delete pImpl;
#ifdef DBG_UTIL
    ULONG nStrings = SwAutoCompleteString::GetElementCount();
    ULONG nClients = SwAutoCompleteClient::GetElementCount();
    DBG_ASSERT(!nStrings && !nClients, "AutoComplete: clients or string count mismatch")
#endif
}

BOOL SwAutoCompleteWord::InsertWord( const String& rWord, SwDoc& rDoc )
{
    String aNewWord(rWord);
    aNewWord.EraseAllChars( CH_TXTATR_INWORD );
    aNewWord.EraseAllChars( CH_TXTATR_BREAKWORD );

    pImpl->AddDocument(rDoc);
    BOOL bRet = FALSE;
    xub_StrLen nWrdLen = aNewWord.Len();
    while( nWrdLen && '.' == aNewWord.GetChar( nWrdLen-1 ))
        --nWrdLen;

    if( !bLockWordLst && nWrdLen >= nMinWrdLen )
    {
        SwAutoCompleteString* pAutoString;
        StringPtr pNew = pAutoString = new SwAutoCompleteString( aNewWord, 0, nWrdLen );
        pAutoString->AddDocument(rDoc);
        USHORT nInsPos;
        if( aWordLst.Insert( pNew, nInsPos ) )
        {
            bRet = TRUE;
            if( aLRULst.Count() < nMaxCount )
                aLRULst.Insert( pNew, 0 );
            else
            {
                // der letzte muss entfernt werden
                // damit der neue vorne Platz hat
                String* pDel = (String*)aLRULst[ nMaxCount - 1 ];

                void** ppData = (void**)aLRULst.GetData();
                memmove( ppData+1, ppData, (nMaxCount - 1) * sizeof( void* ));
                *ppData = pNew;

                aWordLst.Remove( pDel );
                delete (SwAutoCompleteString*)pDel;
            }
        }
        else
        {
            delete (SwAutoCompleteString*)pNew;
            // dann aber auf jedenfall nach "oben" moven
            pNew = aWordLst[ nInsPos ];

            //add the document to the already inserted string
            SwAutoCompleteString* pCurrent = (SwAutoCompleteString*)pNew;
            pCurrent->AddDocument(rDoc);

            nInsPos = aLRULst.GetPos( (void*)pNew );
            ASSERT( USHRT_MAX != nInsPos, "String nicht gefunden" );
            if( nInsPos )
            {
                void** ppData = (void**)aLRULst.GetData();
                memmove( ppData+1, ppData, nInsPos * sizeof( void* ) );
                *ppData = pNew;
            }
        }
    }
    return bRet;
}

void SwAutoCompleteWord::SetMaxCount( USHORT nNewMax )
{
    if( nNewMax < nMaxCount && aLRULst.Count() > nNewMax )
    {
        // dann die unten ueberhaengenden entfernen
        USHORT nLRUIndex = nNewMax-1;
        while( nNewMax < aWordLst.Count() && nLRUIndex < aLRULst.Count())
        {
            USHORT nPos = aWordLst.GetPos( (String*)aLRULst[ nLRUIndex++ ] );
            ASSERT( USHRT_MAX != nPos, "String nicht gefunden" );
            void * pDel = aWordLst[nPos];
            aWordLst.Remove(nPos);
            delete (SwAutoCompleteString*)pDel;
        }
        aLRULst.Remove( nNewMax-1, aLRULst.Count() - nNewMax );
    }
    nMaxCount = nNewMax;
}

void SwAutoCompleteWord::SetMinWordLen( USHORT n )
{
    // will man wirklich alle Worte, die kleiner als die neue Min Laenge
    // sind entfernen?
    // JP 02.02.99 - erstmal nicht.

    // JP 11.03.99 - mal testhalber eingebaut
    if( n < nMinWrdLen )
    {
        for( USHORT nPos = 0; nPos < aWordLst.Count(); ++nPos  )
            if( aWordLst[ nPos ]->Len() < n )
            {
                void* pDel = aWordLst[ nPos ];
                aWordLst.Remove(nPos);

                USHORT nDelPos = aLRULst.GetPos( pDel );
                ASSERT( USHRT_MAX != nDelPos, "String nicht gefunden" );
                aLRULst.Remove( nDelPos );
                --nPos;
                delete (SwAutoCompleteString*)pDel;
            }
    }

    nMinWrdLen = n;
}

BOOL SwAutoCompleteWord::GetRange( const String& rWord, USHORT& rStt,
                                    USHORT& rEnd ) const
{
    const StringPtr pStr = (StringPtr)&rWord;
    aWordLst.Seek_Entry( pStr, &rStt );
    rEnd = rStt;

    const ::utl::TransliterationWrapper& rSCmp = GetAppCmpStrIgnore();
    while( rEnd < aWordLst.Count() && rSCmp.isMatch( rWord, *aWordLst[ rEnd ]))
        ++rEnd;

    return rStt < rEnd;
}

void SwAutoCompleteWord::CheckChangedList( const SvStringsISortDtor& rNewLst )
{
    USHORT nMyLen = aWordLst.Count(), nNewLen = rNewLst.Count();
    USHORT nMyPos = 0, nNewPos = 0;

    for( ; nMyPos < nMyLen && nNewPos < nNewLen; ++nMyPos, ++nNewPos )
    {
        const StringPtr pStr = rNewLst[ nNewPos ];
        while( aWordLst[ nMyPos ] != pStr )
        {
            void* pDel = aWordLst[ nMyPos ];
            aWordLst.Remove(nMyPos);

            USHORT nPos = aLRULst.GetPos( pDel );
            ASSERT( USHRT_MAX != nPos, "String nicht gefunden" );
            aLRULst.Remove( nPos );
            delete (SwAutoCompleteString*)pDel;
            if( nMyPos >= --nMyLen )
                break;
        }
    }
    //remove the elements at the end of the array
    if( nMyPos < nMyLen )
    {
        //clear LRU array first then delete the string object
        for( ; nNewPos < nMyLen; ++nNewPos )
        {
            void* pDel = aWordLst[ nNewPos ];
            USHORT nPos = aLRULst.GetPos( pDel );
            ASSERT( USHRT_MAX != nPos, "String nicht gefunden" );
            aLRULst.Remove( nPos );
            delete (SwAutoCompleteString*)pDel;
        }
        //remove from array
        aWordLst.Remove( nMyPos, nMyLen - nMyPos );
    }
}
/* -----------------------------05.08.2002 12:54------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoCompleteWord::DocumentDying(const SwDoc& rDoc)
{
    pImpl->RemoveDocument(rDoc);

    SvxAutoCorrect* pACorr = SvxAutoCorrCfg::Get()->GetAutoCorrect();
    const sal_Bool bDelete = !pACorr->GetSwFlags().bAutoCmpltKeepList;
    for(USHORT nPos = aWordLst.Count(); nPos; nPos--)
    {
        SwAutoCompleteString* pCurrent = (SwAutoCompleteString*)aWordLst[ nPos - 1 ];
        if(pCurrent->RemoveDocument(rDoc) && bDelete)
        {
            aWordLst.Remove( nPos - 1 );
            USHORT nLRUPos = aLRULst.GetPos( (void*)pCurrent );
            DBG_ASSERT(nLRUPos < USHRT_MAX, "word not found in LRU list" )
            aLRULst.Remove( nLRUPos );
            delete pCurrent;
        }
    }
}

