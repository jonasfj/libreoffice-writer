/*************************************************************************
 *
 *  $RCSfile: laycache.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jp $ $Date: 2001-05-30 16:33:16 $
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

#include <hintids.hxx>
#ifndef _SVX_BRKITEM_HXX //autogen
#include <svx/brkitem.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCSTAT_HXX //autogen
#include <docstat.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _LAYCACHE_HXX
#include <laycache.hxx>
#endif
#ifndef _LAYHELP_HXX
#include <layhelp.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _ROWFRM_HXX
#include <rowfrm.hxx>
#endif
#ifndef _COLFRM_HXX
#include <colfrm.hxx>
#endif
#ifndef _BODYFRM_HXX
#include <bodyfrm.hxx>
#endif
#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _FMTCNTNT_HXX //autogen
#include <fmtcntnt.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif

/*-----------------28.5.2001 10:06------------------
 *  Reading and writing of the layout cache.
 *  The layout cache is not necessary, but it improves
 *  the performance and reduces the text flow during
 *  the formatting.
 *  The layout cache contains the index of the paragraphs/tables
 *  at the top of every page, so it's possible to create
 *  the right count of pages and to distribute the document content
 *  to this pages before the formatting starts.
 *--------------------------------------------------*/

void SwLayoutCache::Read( SvStream &rStream )
{
    if( !pImpl )
    {
        pImpl = new SwLayCacheImpl;
        if( !pImpl->Read( rStream ) )
        {
            delete pImpl;
            pImpl = 0;
        }
    }
}

//-----------------------------------------------------------------------------

void SwLayCacheImpl::Insert( USHORT nType, ULONG nIndex, xub_StrLen nOffset )
{
    aType.Insert( nType, aType.Count() );
    SvULongs::Insert( nIndex, SvULongs::Count() );
    aOffset.Insert( nOffset, aOffset.Count() );
}

BOOL SwLayCacheImpl::Read( SvStream& rStream )
{
    SwLayCacheIoImpl aIo( rStream, FALSE );
    if( aIo.GetMajorVersion() > SW_LAYCACHE_IO_VERSION_MAJOR )
        return FALSE;

    BYTE cFlags;
    UINT32 nIndex, nOffset;

    aIo.OpenRec( SW_LAYCACHE_IO_REC_PAGES );
    aIo.OpenFlagRec();
    aIo.CloseFlagRec();
    while( aIo.BytesLeft() && !aIo.HasError() )
    {
        switch( aIo.Peek() )
        {
        case SW_LAYCACHE_IO_REC_PARA:
            aIo.OpenRec( SW_LAYCACHE_IO_REC_PARA );
            cFlags = aIo.OpenFlagRec();
            aIo.GetStream() >> nIndex;
            if( (cFlags & 0x01) != 0 )
                aIo.GetStream() >> nOffset;
            else
                nOffset = STRING_LEN;
            aIo.CloseFlagRec();
            Insert( SW_LAYCACHE_IO_REC_PARA, nIndex, (xub_StrLen)nOffset );
            aIo.CloseRec( SW_LAYCACHE_IO_REC_PARA );
            break;
        case SW_LAYCACHE_IO_REC_TABLE:
            aIo.OpenRec( SW_LAYCACHE_IO_REC_TABLE );
            aIo.OpenFlagRec();
            aIo.GetStream() >> nIndex
                             >> nOffset;
            Insert( SW_LAYCACHE_IO_REC_TABLE, nIndex, (xub_StrLen)nOffset );
            aIo.CloseFlagRec();
            aIo.CloseRec( SW_LAYCACHE_IO_REC_TABLE );
            break;
        default:
            aIo.SkipRec();
            break;
        }
    }
    aIo.CloseRec( SW_LAYCACHE_IO_REC_PAGES );

    return !aIo.HasError();
}

/*-----------------28.5.2001 10:19------------------
 * SwLayoutCache::Write(..)
 * writes the index (more precise: the difference between
 * the index and the first index of the document content)
 * of the first paragraph/table at the top of every page.
 * If at the top of a page is the rest of a paragraph/table
 * from the bottom of the previous page, the character/row
 * number is stored, too.
 * --------------------------------------------------*/

void SwLayoutCache::Write( SvStream &rStream, const SwDoc& rDoc )
{
    if( rDoc.GetRootFrm() ) // the layout itself ..
    {
        SwLayCacheIoImpl aIo( rStream, TRUE );
        // We want to save the relative index, so we need the index
        // of the first content
        ULONG nStartOfContent = rDoc.GetNodes().GetEndOfContent().
                                FindStartNode()->GetIndex();
        // The first page..
        SwPageFrm* pPage = (SwPageFrm*)rDoc.GetRootFrm()->Lower();
        //.. is not very interesting, cause it's clear that the first
        // page starts with the first paragraph/table, do we get the
        if( pPage ) // second page
            pPage = (SwPageFrm*)pPage->GetNext();

        aIo.OpenRec( SW_LAYCACHE_IO_REC_PAGES );
        aIo.OpenFlagRec( 0, 0 );
        aIo.CloseFlagRec();
        while( pPage )
        {
            SwLayoutFrm* pLay = pPage->FindBodyCont();
            SwFrm* pTmp = pLay ? pLay->ContainsAny() : NULL;
            // We are only interested in paragraph or table frames,
            // a section frames contains paragraphs/tables.
            if( pTmp && pTmp->IsSctFrm() )
                pTmp = ((SwSectionFrm*)pTmp)->ContainsAny();

            if( pTmp ) // any content
            {
                if( pTmp->IsTxtFrm() )
                {
                    ULONG nNdIdx = ((SwTxtFrm*)pTmp)->GetNode()->GetIndex();
                    if( nNdIdx > nStartOfContent )
                    {
                        /*  Open Paragraph Record */
                        aIo.OpenRec( SW_LAYCACHE_IO_REC_PARA );
                        BOOL bFollow = ((SwTxtFrm*)pTmp)->IsFollow();
                        aIo.OpenFlagRec( bFollow ? 0x01 : 0x00,
                                           bFollow ? 8 : 4 );
                        nNdIdx -= nStartOfContent;
                        aIo.GetStream() << nNdIdx;
                        if( bFollow )
                            aIo.GetStream() << (ULONG)((SwTxtFrm*)pTmp)->GetOfst();
                        aIo.CloseFlagRec();
                        /*  Close Paragraph Record */
                        aIo.CloseRec( SW_LAYCACHE_IO_REC_PARA );
                    }
                }
                else if( pTmp->IsTabFrm() )
                {
                    SwTabFrm* pTab = (SwTabFrm*)pTmp;
                    ULONG nOfst = STRING_LEN;
                    if( pTab->IsFollow() )
                    {
                        // If the table is a follow, we have to look for the
                        // master and to count all rows to get the row number
                        nOfst = 0;
                        while( pTab->IsFollow() )
                            pTab = pTab->FindMaster();
                        while( pTab != pTmp )
                        {
                            SwFrm* pSub = pTab->Lower();
                            while( pSub )
                            {
                                ++nOfst;
                                pSub = pSub->GetNext();
                            }
                            pTab = pTab->GetFollow();
                            ASSERT( pTab, "Table follow without master" );
                        }
                    }
                    do
                    {
                        ULONG nNdIdx =
                                pTab->GetTable()->GetTableNode()->GetIndex();
                        if( nNdIdx > nStartOfContent )
                        {
                            /* Open Table Record */
                            aIo.OpenRec( SW_LAYCACHE_IO_REC_TABLE );
                            aIo.OpenFlagRec( 0, 8 );
                            nNdIdx -= nStartOfContent;
                            aIo.GetStream() << nNdIdx
                                            << nOfst;
                            aIo.CloseFlagRec();
                            /* Close Table Record  */
                            aIo.CloseRec( SW_LAYCACHE_IO_REC_TABLE );
                        }
                        // If the table has a follow on the next page,
                        // we know already the row number and store this
                        // immediately.
                        if( pTab->GetFollow() )
                        {
                            if( nOfst == STRING_LEN )
                                nOfst = 0;
                            do
                            {
                                SwFrm* pSub = pTab->Lower();
                                while( pSub )
                                {
                                    ++nOfst;
                                    pSub = pSub->GetNext();
                                }
                                pTab = pTab->GetFollow();
                                SwPageFrm *pTabPage = pTab->FindPageFrm();
                                if( pTabPage != pPage )
                                {
                                    ASSERT( pPage->GetPhyPageNum() <
                                            pTabPage->GetPhyPageNum(),
                                            "Looping Tableframes" );
                                    pPage = pTabPage;
                                    break;
                                }
                            } while ( pTab->GetFollow() );
                        }
                        else
                            break;
                    } while( pTab );
                }
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        aIo.CloseRec( SW_LAYCACHE_IO_REC_PAGES );
    }
}

#ifndef PRODUCT
sal_Bool SwLayoutCache::CompareLayout( const SwDoc& rDoc ) const
{
    sal_Bool bRet = sal_True;
    if( pImpl && rDoc.GetRootFrm() )
    {
        USHORT nIndex = 0;
        ULONG nStartOfContent = rDoc.GetNodes().GetEndOfContent().
                                FindStartNode()->GetIndex();
        SwPageFrm* pPage = (SwPageFrm*)rDoc.GetRootFrm()->Lower();
        if( pPage )
            pPage = (SwPageFrm*)pPage->GetNext();
        while( pPage )
        {
            if( nIndex >= pImpl->Count() )
            {
                if( bRet )
                    bRet = sal_False;
                break;
            }
            SwLayoutFrm* pLay = pPage->FindBodyCont();
            SwFrm* pTmp = pLay ? pLay->ContainsAny() : NULL;
            if( pTmp && pTmp->IsSctFrm() )
                pTmp = ((SwSectionFrm*)pTmp)->ContainsAny();
            if( pTmp )
            {
                if( pTmp->IsTxtFrm() )
                {
                    ULONG nNdIdx = ((SwTxtFrm*)pTmp)->GetNode()->GetIndex();
                    if( nNdIdx > nStartOfContent )
                    {
                        BOOL bFollow = ((SwTxtFrm*)pTmp)->IsFollow();
                        nNdIdx -= nStartOfContent;
                        if( pImpl->GetBreakIndex( nIndex ) != nNdIdx ||
                            SW_LAYCACHE_IO_REC_PARA !=
                            pImpl->GetBreakType( nIndex ) ||
                            ( bFollow ? ((SwTxtFrm*)pTmp)->GetOfst()
                              : STRING_LEN ) != pImpl->GetBreakOfst( nIndex ) )
                        {
                            if( bRet )
                                bRet = sal_False;
                        }
                        ++nIndex;
                    }
                }
                else if( pTmp->IsTabFrm() )
                {
                    SwTabFrm* pTab = (SwTabFrm*)pTmp;
                    ULONG nOfst = STRING_LEN;
                    if( pTab->IsFollow() )
                    {
                        nOfst = 0;
                        while( pTab->IsFollow() )
                            pTab = pTab->FindMaster();
                        while( pTab != pTmp )
                        {
                            SwFrm* pSub = pTab->Lower();
                            while( pSub )
                            {
                                ++nOfst;
                                pSub = pSub->GetNext();
                            }
                            pTab = pTab->GetFollow();
                        }
                    }
                    do
                    {
                        ULONG nNdIdx =
                                pTab->GetTable()->GetTableNode()->GetIndex();
                        if( nNdIdx > nStartOfContent )
                        {
                            nNdIdx -= nStartOfContent;
                            if( pImpl->GetBreakIndex( nIndex ) != nNdIdx ||
                                SW_LAYCACHE_IO_REC_TABLE !=
                                pImpl->GetBreakType( nIndex ) ||
                               nOfst != pImpl->GetBreakOfst( nIndex ) )
                            {
                                if( bRet )
                                    bRet = sal_False;
                            }
                            ++nIndex;
                        }
                        if( pTab->GetFollow() )
                        {
                            if( nOfst == STRING_LEN )
                                nOfst = 0;
                            do
                            {
                                SwFrm* pSub = pTab->Lower();
                                while( pSub )
                                {
                                    ++nOfst;
                                    pSub = pSub->GetNext();
                                }
                                pTab = pTab->GetFollow();
                                SwPageFrm *pTabPage = pTab->FindPageFrm();
                                if( pTabPage != pPage )
                                {
                                    pPage = pTabPage;
                                    break;
                                }
                            } while ( pTab->GetFollow() );
                        }
                        else
                            break;
                    } while( pTab );
                }
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
    }
    return bRet;
}
#endif

void SwLayoutCache::ClearImpl()
{
    if( !IsLocked() )
    {
        delete pImpl;
        pImpl = 0;
    }
}


SwLayoutCache::~SwLayoutCache()
{
    ASSERT( !nLockCount, "Deleting a locked SwLayoutCache!?" );
    delete pImpl;
}

/*-----------------28.5.2001 10:47------------------
 * SwActualSection,
 *  a help class to create not nested section frames
 *  for nested sections.
 * --------------------------------------------------*/

SwActualSection::SwActualSection( SwActualSection *pUp,
                                  SwSectionFrm	  *pSect,
                                  SwSectionNode	  *pNd ) :
    pUpper( pUp ),
    pSectFrm( pSect ),
    pSectNode( pNd )
{
    if ( !pSectNode )
    {
        const SwNodeIndex *pIndex = pSect->GetFmt()->GetCntnt().GetCntntIdx();
        pSectNode = pSect->GetFmt()->GetDoc()->GetNodes()[*pIndex]->
                                                            FindSectionNode();
    }
}

/*-----------------28.5.2001 11:09------------------
 * SwLayHelper
 *  is the helper class, which utilizes the layout cache information
 *  to distribute the document content to the rigth pages.
 * It's used by the _InsertCnt(..)-function.
 * If there's no layout cache, the distibution to the pages is more
 * a guess, but a guess with statistical background.
 * --------------------------------------------------*/

SwLayHelper::SwLayHelper( SwDoc *pD, SwFrm* &rpF, SwFrm* &rpP, SwPageFrm* &rpPg,
            SwLayoutFrm* &rpL, SwActualSection* &rpA, BOOL &rB,
            ULONG nNodeIndex, BOOL bCache )
    : rpFrm( rpF ), rpPrv( rpP ), rpPage( rpPg ), rpLay( rpL ),
      rpActualSection( rpA ), rbBreakAfter(rB), pDoc(pD), nMaxParaPerPage( 25 ),
      nParagraphCnt( bCache ? 0 : USHRT_MAX ), bFirst( bCache )
{
    pImpl = pDoc->GetLayoutCache() ? pDoc->GetLayoutCache()->LockImpl() : NULL;
    if( pImpl )
    {
        nStartOfContent = pDoc->GetNodes().GetEndOfContent().FindStartNode()
                          ->GetIndex();
        nNodeIndex -= nStartOfContent;
        nIndex = 0;
        while( nIndex < pImpl->Count() && (*pImpl)[ nIndex ] < nNodeIndex )
            ++nIndex;
        if( nIndex >= pImpl->Count() )
        {
            pDoc->GetLayoutCache()->UnlockImpl();
            pImpl = NULL;
        }
    }
    else
    {
        pImpl = NULL;
        nIndex = USHRT_MAX;
        nStartOfContent = ULONG_MAX;
    }
}

SwLayHelper::~SwLayHelper()
{
    if( pImpl )
    {
        ASSERT( pDoc && pDoc->GetLayoutCache(), "Missing layoutcache" );
        pDoc->GetLayoutCache()->UnlockImpl();
    }
}

/*-----------------23.5.2001 16:40------------------
 * SwLayHelper::CalcPageCount() does not really calculate the page count,
 * it returns the page count value from the layout cache, if available,
 * otherwise it estimates the page count.
 * --------------------------------------------------*/

ULONG SwLayHelper::CalcPageCount()
{
    ULONG nPgCount;
    SwLayCacheImpl *pCache = pDoc->GetLayoutCache() ?
                             pDoc->GetLayoutCache()->LockImpl() : NULL;
    if( pCache )
    {
        nPgCount = pCache->Count() + 1;
        pDoc->GetLayoutCache()->UnlockImpl();
    }
    else
    {
        nPgCount = pDoc->GetDocStat().nPage;
        if ( nPgCount <= 10 ) // no page insertion for less than 10 pages
            nPgCount = 0;
        ULONG nNdCount = pDoc->GetDocStat().nPara;
        if ( nNdCount <= 1 )
        {
            //Estimates the number of paragraphs.
            ULONG nTmp = pDoc->GetNodes().GetEndOfContent().GetIndex() -
                        pDoc->GetNodes().GetEndOfExtras().GetIndex();
            //Tables have a little overhead..
            nTmp -= pDoc->GetTblFrmFmts()->Count() * 25;
            //Fly frames, too ..
            nTmp -= (pDoc->GetNodes().GetEndOfAutotext().GetIndex() -
                       pDoc->GetNodes().GetEndOfInserts().GetIndex()) / 3 * 5;
            if ( nTmp > 0 )
                nNdCount = nTmp;
        }
        if ( nNdCount > 100 ) // no estimation below this value
        {
            if ( nPgCount > 0 )
                nMaxParaPerPage = nNdCount / nPgCount;
            else
            {
                nMaxParaPerPage = Max( ULONG(20),
                                       ULONG(20 + nNdCount / 1000 * 3) );
#ifdef PM2
                const ULONG nMax = 49;
#elif MAC
                const ULONG nMax = 56;
#elif UNIX
                const ULONG nMax = 57;
#else
                const ULONG nMax = 53;
#endif
                nMaxParaPerPage = Min( nMaxParaPerPage, nMax );
                nPgCount = nNdCount / nMaxParaPerPage;
            }
            if ( nNdCount < 1000 )
                nPgCount = 0;// no progress bar for small documents
            if ( pDoc->IsBrowseMode() )
                nMaxParaPerPage *= 6;
        }
    }
    return nPgCount;
}

/*-----------------23.5.2001 16:44------------------
 * SwLayHelper::CheckInsertPage()
 * inserts a page and return TRUE, if
 * - the break after flag is set
 * - the actual content wants a break before
 * - the maximum count of paragraph/rows is reached
 *
 * The break after flag is set, if the actual content
 * wants a break after.
 * --------------------------------------------------*/

BOOL SwLayHelper::CheckInsertPage()
{
    FASTBOOL bEnd = 0 == rpPage->GetNext();
    const SwAttrSet *pAttr = rpFrm->GetAttrSet();
    const SvxFmtBreakItem &rBrk = pAttr->GetBreak();
    const SwFmtPageDesc &rDesc = pAttr->GetPageDesc();
    const SwPageDesc *pDesc = rDesc.GetPageDesc();

    BOOL bBrk = nParagraphCnt > nMaxParaPerPage || rbBreakAfter;
    rbBreakAfter = rBrk.GetBreak() == SVX_BREAK_PAGE_AFTER ||
                   rBrk.GetBreak() == SVX_BREAK_PAGE_BOTH;
    if ( !bBrk )
        bBrk = rBrk.GetBreak() == SVX_BREAK_PAGE_BEFORE ||
               rBrk.GetBreak() == SVX_BREAK_PAGE_BOTH;

    if ( bBrk || pDesc )
    {
        USHORT nPgNum = 0;
        if ( !pDesc )
            pDesc = rpPage->GetPageDesc()->GetFollow();
        else
        {
            if ( 0 != (nPgNum = rDesc.GetNumOffset()) )
                ((SwRootFrm*)rpPage->GetUpper())->SetVirtPageNum(TRUE);
        }
        BOOL bOdd = !rpPage->OnRightPage();
        BOOL bInsertEmpty = FALSE;
        if( nPgNum && bOdd != ( ( nPgNum % 2 ) != 0 ) )
        {
            bOdd = !bOdd;
            bInsertEmpty = TRUE;
        }
        ::InsertNewPage( (SwPageDesc&)*pDesc, rpPage->GetUpper(),
                         bOdd, bInsertEmpty, FALSE, rpPage->GetNext() );
        if ( bEnd )
        {
            ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
            do
            {	rpPage = (SwPageFrm*)rpPage->GetNext();
            } while ( rpPage->GetNext() );
        }
        else
        {
            ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
            rpPage = (SwPageFrm*)rpPage->GetNext();
            if ( rpPage->IsEmptyPage() )
            {
                ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
                rpPage = (SwPageFrm*)rpPage->GetNext();
            }
        }
        rpLay = rpPage->FindBodyCont();
        while( rpLay->Lower() )
            rpLay = (SwLayoutFrm*)rpLay->Lower();
        return TRUE;
    }
    return FALSE;
}

/*-----------------28.5.2001 11:31------------------
 * SwLayHelper::CheckInsert
 *  is the entry point for the _InsertCnt-function.
 *  The document content index is checked either it is
 *  in the layout cache either it's time to insert a page
 *  cause the maximal estimation of content per page is reached.
 *  A really big table or long paragraph may contains more than
 *  one page, in this case the needed count of pages will inserted.
 * --------------------------------------------------*/

BOOL SwLayHelper::CheckInsert( ULONG nNodeIndex )
{
    BOOL bRet = FALSE;
    nNodeIndex -= nStartOfContent;
    USHORT nRows;
    if( rpFrm->IsTabFrm() )
    {
        //Inside a table counts every row as a paragraph
        SwFrm *pLow = ((SwTabFrm*)rpFrm)->Lower();
        nRows = 0;
        do
        {
            ++nRows;
            pLow = pLow->GetNext();
        } while ( pLow );
        nParagraphCnt += nRows;
    }
    else
        ++nParagraphCnt;
    if( bFirst && pImpl && nIndex < pImpl->Count() &&
        pImpl->GetBreakIndex( nIndex ) == nNodeIndex &&
        ( pImpl->GetBreakOfst( nIndex ) < STRING_LEN ||
          ( ++nIndex < pImpl->Count() &&
          pImpl->GetBreakIndex( nIndex ) == nNodeIndex ) ) )
        bFirst = FALSE;
    if( !bFirst )
    {
        ULONG nRowCount = 0;
        do
        {
            if( pImpl )
            {
                xub_StrLen nOfst = STRING_LEN;
                USHORT nType = SW_LAYCACHE_IO_REC_PAGES;
                while( nIndex < pImpl->Count() &&
                       pImpl->GetBreakIndex(nIndex) < nNodeIndex)
                    nIndex += 2;
                if( nIndex < pImpl->Count() &&
                    pImpl->GetBreakIndex(nIndex) == nNodeIndex )
                {
                    nType = pImpl->GetBreakType( nIndex );
                    nOfst = pImpl->GetBreakOfst( nIndex++ );
                    rbBreakAfter = sal_True;
                }
                if( nOfst < STRING_LEN )
                {
                    sal_Bool bSplit = sal_False;
                    sal_Bool bRepeat;
                    if( rpFrm->IsTxtFrm() && SW_LAYCACHE_IO_REC_PARA == nType &&
                        nOfst<((SwTxtFrm*)rpFrm)->GetTxtNode()->GetTxt().Len() )
                        bSplit = sal_True;
                    else if( rpFrm->IsTabFrm() && nRowCount < nOfst &&
                             SW_LAYCACHE_IO_REC_TABLE == nType )
                    {
                        bRepeat = ((SwTabFrm*)rpFrm)->
                                  GetTable()->IsHeadlineRepeat();
                        bSplit = nOfst < nRows;
                    }
                    if( bSplit )
                    {
                        rpFrm->InsertBehind( rpLay, rpPrv );
                        rpFrm->Frm().Pos() = rpLay->Frm().Pos();
                        rpFrm->Frm().Pos().Y() += 1;
                        rpPrv = rpFrm;
                        if( rpFrm->IsTabFrm() )
                        {
                            SwTabFrm* pTab = (SwTabFrm*)rpFrm;
                            SwFrm *pRow = pTab->Lower();
                            SwTabFrm *pFoll = new SwTabFrm( *pTab );

                            SwFrm *pPrv;
                            if( bRepeat )
                            {
                                bDontCreateObjects = TRUE; //frmtool
                                SwRowFrm *pHeadline = new SwRowFrm(
                                        *pTab->GetTable()->GetTabLines()[0] );
                                pHeadline->InsertBefore( pFoll, 0 );
                                bDontCreateObjects = FALSE;
                                pPrv = pFoll->Lower();
                                ++nRows;
                            }
                            else
                                pPrv = 0;
                            while( pRow && nRowCount < nOfst )
                            {
                                pRow = pRow->GetNext();
                                ++nRowCount;
                            }
                            while ( pRow )
                            {
                                SwFrm* pNxt = pRow->GetNext();
                                pRow->Remove();
                                pRow->InsertBehind( pFoll, pPrv );
                                pPrv = pRow;
                                pRow = pNxt;
                            }
                            rpFrm = pFoll;
                        }
                        else
                        {
                            SwTxtFrm *pNew = new SwTxtFrm( ((SwTxtFrm*)rpFrm)->
                                                           GetTxtNode() );
                            pNew->_SetIsFollow( sal_True );
                            pNew->ManipOfst( nOfst );
                            pNew->SetFollow( ((SwTxtFrm*)rpFrm)->GetFollow() );
                            ((SwTxtFrm*)rpFrm)->SetFollow( pNew );
                            rpFrm = pNew;
                        }
                    }
                }

#ifndef DEBUG
                if( nIndex >= pImpl->Count() )
                {
                    pImpl = NULL;
                    pDoc->GetLayoutCache()->UnlockImpl();
                }
#endif
            }

            if( CheckInsertPage() )
            {
                if( rpPrv && rpPrv->IsTxtFrm() && !rpPrv->GetValidSizeFlag() )
                    rpPrv->Frm().Height( rpPrv->GetUpper()->Prt().Height() );

                bRet = TRUE;
                rpPrv = 0;
                nParagraphCnt = 0;

                if ( rpActualSection )
                {
                    //Hatte der SectionFrm ueberhaupt Inhalt? Wenn
                    //nicht kann er gleich umgehaengt werden.
                    SwSectionFrm *pSct;
                    if ( !rpActualSection->GetSectionFrm()->ContainsCntnt())
                    {
                        pSct = rpActualSection->GetSectionFrm();
                        pSct->Remove();
                    }
                    else
                    {
                        pSct = new SwSectionFrm(
                            *rpActualSection->GetSectionFrm(), FALSE );
                        rpActualSection->GetSectionFrm()->SimpleFormat();
                        pSct->Frm().Width( rpLay->Prt().Width() );
                        pSct->Prt().Width( rpLay->Prt().Width() );
                    }
                    rpActualSection->SetSectionFrm( pSct );
                    pSct->InsertBehind( rpLay, 0 );
                    pSct->Frm().Pos() = rpLay->Frm().Pos();
                    pSct->Frm().Pos().Y() += 1; //wg. Benachrichtigungen.

                    rpLay = pSct;
                    if ( rpLay->Lower() && rpLay->Lower()->IsLayoutFrm() )
                        rpLay = rpLay->GetNextLayoutLeaf();
                }
            }
        } while( pImpl && nIndex < pImpl->Count() &&
                 (*pImpl)[ nIndex ] == nNodeIndex );
    }
    bFirst = FALSE;
    return bRet;
}

// -----------------------------------------------------------------------------

SwLayCacheIoImpl::SwLayCacheIoImpl( SvStream& rStrm, BOOL bWrtMd ) :
    pStream( &rStrm ),
    nMajorVersion(SW_LAYCACHE_IO_VERSION_MAJOR),
    nMinorVersion(SW_LAYCACHE_IO_VERSION_MINOR),
    bWriteMode( bWrtMd ),
    bError( FALSE  )
{
    if( bWriteMode )
        *pStream << nMajorVersion
                 <<	nMinorVersion;

    else
        *pStream >> nMajorVersion
                 >>	nMinorVersion;
}

BOOL SwLayCacheIoImpl::OpenRec( BYTE cType )
{
    BOOL bRes = TRUE;
    UINT16 nLvl = aRecTypes.Count();
    ASSERT( nLvl == aRecSizes.Count(), "OpenRec: Level" );
    UINT32 nPos = pStream->Tell();
    if( bWriteMode )
    {
        aRecTypes.Insert( cType, nLvl );
        aRecSizes.Insert( nPos, nLvl );
        *pStream << (UINT32) 0;
    }
    else
    {
        UINT32 nVal;
        *pStream >> nVal;
        BYTE cRecTyp = (BYTE)nVal;
        aRecTypes.Insert( cRecTyp, nLvl );
        sal_uInt32 nSize = nVal >> 8;
        aRecSizes.Insert( nPos + nSize, nLvl );
        if( !nVal || cRecTyp != cType ||
            pStream->GetErrorCode() != SVSTREAM_OK || pStream->IsEof() )
        {
            ASSERT( nVal, "OpenRec: Record-Header is 0" );
            ASSERT( cRecTyp == cType,
                    "OpenRec: Wrong Record Type" );
            aRecTypes[nLvl] = 0;
            aRecSizes[nLvl] = pStream->Tell();
            bRes = sal_False;
            bError = TRUE;
        }
    }
    return bRes;
}

// Close record

BOOL SwLayCacheIoImpl::CloseRec( BYTE cType )
{
    BOOL bRes = TRUE;
    UINT16 nLvl = aRecTypes.Count();
    ASSERT( nLvl == aRecSizes.Count(), "CloseRec: wrong Level" );
    ASSERT( nLvl, "CloseRec: no levels" );
    if( nLvl )
    {
        nLvl--;
        ASSERT( cType == aRecTypes[nLvl],
                "CloseRec: Wrong Block-Header" );
        UINT32 nPos = pStream->Tell();
        if( bWriteMode )
        {
            UINT32 nBgn = aRecSizes[nLvl];
            pStream->Seek( nBgn );
            UINT32 nSize = nPos - nBgn;
            UINT32 nVal = ( nSize << 8 ) | aRecTypes[nLvl];
            *pStream << nVal;
            pStream->Seek( nPos );
            if( pStream->GetError() != SVSTREAM_OK )
                 bRes = FALSE;
        }
        else
        {
            UINT32 n = aRecSizes[nLvl];
            ASSERT( n >= nPos, "CloseRec: to much data read" );
            if( n != nPos )
            {
                pStream->Seek( n );
                if( n < nPos )
                     bRes = FALSE;
            }
            if( pStream->GetErrorCode() != SVSTREAM_OK )
                bRes = FALSE;
        }

        aRecTypes.Remove( nLvl, 1 );
        aRecSizes.Remove( nLvl, 1 );
    }

    if( !bRes )
        bError = TRUE;

    return bRes;
}

UINT32 SwLayCacheIoImpl::BytesLeft()
{
    UINT16 nLvl = aRecSizes.Count();
    UINT32 n = 0;
    if( !bError && nLvl )
    {
        UINT32 nEndPos = aRecSizes[ nLvl-1 ];
        UINT32 nPos = pStream->Tell();
        if( nEndPos > nPos )
            n = nEndPos - nPos;
    }

    return n;
}

BYTE SwLayCacheIoImpl::Peek()
{
    BYTE c = 0;
    if( !bError )
    {
        UINT32 nPos = pStream->Tell();
        *pStream >> c;
        pStream->Seek( nPos );
        if( pStream->GetErrorCode() != SVSTREAM_OK )
        {
            c = 0;
            bError = TRUE;
        }
    }
    return c;
}

void SwLayCacheIoImpl::SkipRec()
{
    BYTE c = Peek();
    OpenRec( c );
    pStream->Seek( aRecSizes[aRecSizes.Count()-1] );
    CloseRec( c );
}

BYTE SwLayCacheIoImpl::OpenFlagRec()
{
    ASSERT( !bWriteMode, "OpenFlagRec illegal in write  mode" );
    BYTE cFlags;
    *pStream >> cFlags;
    nFlagRecEnd = pStream->Tell() + ( cFlags & 0x0F );
    return (cFlags >> 4);
}

void SwLayCacheIoImpl::OpenFlagRec( BYTE nFlags, BYTE nLen )
{
    ASSERT( bWriteMode, "OpenFlagRec illegal in read  mode" );
    ASSERT( (nFlags & 0xF0) == 0, "illegal flags set" );
    ASSERT( nLen < 16, "wrong flag record length" );
    BYTE cFlags = (nFlags << 4) + nLen;
    *pStream << cFlags;
    nFlagRecEnd = pStream->Tell() + nLen;
}

void SwLayCacheIoImpl::CloseFlagRec()
{
    if( bWriteMode )
    {
        ASSERT( pStream->Tell() == nFlagRecEnd, "Wrong amount of data written" );
    }
    else
    {
        ASSERT( pStream->Tell() <= nFlagRecEnd, "To many data read" );
        if( pStream->Tell() != nFlagRecEnd )
            pStream->Seek( nFlagRecEnd );
    }
}
