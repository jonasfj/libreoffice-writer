/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swserv.cxx,v $
 * $Revision: 1.11 $
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


#include <sot/storage.hxx>
#include <svx/linkmgr.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <doc.hxx>
#include <swtypes.hxx>
#include <swserv.hxx>
#include <swbaslnk.hxx>
#include <mvsave.hxx>
#include <bookmrk.hxx>
#include <pam.hxx>
#include <shellio.hxx>
#ifndef _SWERROR_H
#include <swerror.h>
#endif

using namespace ::com::sun::star;

SV_IMPL_REF( SwServerObject )

SwServerObject::~SwServerObject()
{
}


BOOL SwServerObject::GetData( uno::Any & rData,
                                 const String & rMimeType, BOOL )
{
    BOOL bRet = FALSE;
    WriterRef xWrt;
    switch( SotExchange::GetFormatIdFromMimeType( rMimeType ) )
    {
    case FORMAT_STRING:
        ::GetASCWriter( aEmptyStr, String(), xWrt );
        break;

    case FORMAT_RTF:
        // mba: no BaseURL for data exchange
        ::GetRTFWriter( aEmptyStr, String(), xWrt );
        break;
    }

    if( xWrt.Is() )
    {
        SwPaM* pPam = 0;
        switch( eType )
        {
        case BOOKMARK_SERVER:
            if( CNTNT_TYPE.pBkmk->GetOtherBookmarkPos() )
            {
                // Bereich aufspannen
                pPam = new SwPaM( CNTNT_TYPE.pBkmk->GetBookmarkPos(),
                                *CNTNT_TYPE.pBkmk->GetOtherBookmarkPos() );
            }
            break;

        case TABLE_SERVER:
            pPam = new SwPaM( *CNTNT_TYPE.pTblNd,
                             *CNTNT_TYPE.pTblNd->EndOfSectionNode() );
            break;

        case SECTION_SERVER:
            pPam = new SwPaM( SwPosition( *CNTNT_TYPE.pSectNd ) );
            pPam->Move( fnMoveForward );
            pPam->SetMark();
            pPam->GetPoint()->nNode = *CNTNT_TYPE.pSectNd->EndOfSectionNode();
            pPam->Move( fnMoveBackward );
            break;
        case NONE_SERVER: break;
        }

        if( pPam )
        {
            // Stream anlegen
            SvMemoryStream aMemStm( 65535, 65535 );
            SwWriter aWrt( aMemStm, *pPam, FALSE );
            if( !IsError( aWrt.Write( xWrt )) )
            {
                aMemStm << '\0'; 		// append a zero char
                rData <<= uno::Sequence< sal_Int8 >(
                                        (sal_Int8*)aMemStm.GetData(),
                                        aMemStm.Seek( STREAM_SEEK_TO_END ) );
                bRet = TRUE;
            }

            delete pPam;
        }
    }
    return bRet;
}


BOOL SwServerObject::SetData( const String & ,
                    const uno::Any& )
{
    // set new data into the "server" -> at first nothing to do
    return FALSE;
}


void SwServerObject::SendDataChanged( const SwPosition& rPos )
{
    // ist an unseren Aenderungen jemand interessiert ?
    if( HasDataLinks() )
    {
        int bCall = FALSE;
        const SwStartNode* pNd = 0;
        switch( eType )
        {
        case BOOKMARK_SERVER:
            if( CNTNT_TYPE.pBkmk->GetOtherBookmarkPos() )
            {
                SwBookmark& rBkmk = *CNTNT_TYPE.pBkmk;
                bCall = rBkmk.GetBookmarkPos() < *rBkmk.GetOtherBookmarkPos()
                    ? ( rBkmk.GetBookmarkPos() <= rPos && rPos < *rBkmk.GetOtherBookmarkPos() )
                    : ( *rBkmk.GetOtherBookmarkPos() <= rPos && rPos < rBkmk.GetBookmarkPos() );
            }
            break;

        case TABLE_SERVER: 		pNd = CNTNT_TYPE.pTblNd;	break;
        case SECTION_SERVER:	pNd = CNTNT_TYPE.pSectNd;	break;
        case NONE_SERVER: break;
        }
        if( pNd )
        {
            ULONG nNd = rPos.nNode.GetIndex();
            bCall = pNd->GetIndex() < nNd && nNd < pNd->EndOfSectionIndex();
        }

        if( bCall )
        {
            // Recursionen erkennen und flaggen
            IsLinkInServer( 0 );
            SvLinkSource::NotifyDataChanged();
        }
    }
    // sonst melden wir uns ab !!
// ????? JP 27.06.95: geht das so ????
//	else
//		Closed();
}


void SwServerObject::SendDataChanged( const SwPaM& rRange )
{
    // ist an unseren Aenderungen jemand interessiert ?
    if( HasDataLinks() )
    {
        int bCall = FALSE;
        const SwStartNode* pNd = 0;
        const SwPosition* pStt = rRange.Start(), *pEnd = rRange.End();
        switch( eType )
        {
        case BOOKMARK_SERVER:
            if( CNTNT_TYPE.pBkmk->GetOtherBookmarkPos() )
            {
                SwBookmark& rBkmk = *CNTNT_TYPE.pBkmk;
                const SwPosition* pBkStt = &rBkmk.GetBookmarkPos(),
                                * pBkEnd = rBkmk.GetOtherBookmarkPos();
                if( *pBkStt > *pBkEnd )
                {
                    const SwPosition* pTmp = pBkStt;
                    pBkStt = pBkEnd;
                    pBkEnd = pTmp;
                }
                bCall = *pStt <= *pBkEnd && *pEnd > *pBkStt;
            }
            break;

        case TABLE_SERVER: 		pNd = CNTNT_TYPE.pTblNd;	break;
        case SECTION_SERVER:	pNd = CNTNT_TYPE.pSectNd;	break;
        case NONE_SERVER: break;
        }
        if( pNd )
        {
            // liegt der Start-Bereich im Node Bereich ?
            bCall = pStt->nNode.GetIndex() <  pNd->EndOfSectionIndex() &&
                    pEnd->nNode.GetIndex() >= pNd->GetIndex();
        }

        if( bCall )
        {
            // Recursionen erkennen und flaggen
            IsLinkInServer( 0 );
            SvLinkSource::NotifyDataChanged();
        }
    }
    // sonst melden wir uns ab !!
// ????? JP 27.06.95: geht das so ????
//	else
//		Closed();
}


BOOL SwServerObject::IsLinkInServer( const SwBaseLink* pChkLnk ) const
{
    ULONG nSttNd = 0, nEndNd = 0;
    xub_StrLen nStt = 0;
    xub_StrLen nEnd = 0;
    const SwNode* pNd = 0;
    const SwNodes* pNds = 0;

    switch( eType )
    {
    case BOOKMARK_SERVER:
        if( CNTNT_TYPE.pBkmk->GetOtherBookmarkPos() )
        {
            SwBookmark& rBkmk = *CNTNT_TYPE.pBkmk;
            const SwPosition* pStt = &rBkmk.GetBookmarkPos(),
                            * pEnd = rBkmk.GetOtherBookmarkPos();
            if( *pStt > *pEnd )
            {
                const SwPosition* pTmp = pStt;
                pStt = pEnd;
                pEnd = pTmp;
            }

            nSttNd = pStt->nNode.GetIndex();
            nStt = pStt->nContent.GetIndex();
            nEndNd = pEnd->nNode.GetIndex();
            nEnd = pEnd->nContent.GetIndex();
            pNds = &pStt->nNode.GetNodes();
        }
        break;

    case TABLE_SERVER: 		pNd = CNTNT_TYPE.pTblNd;	break;
    case SECTION_SERVER:	pNd = CNTNT_TYPE.pSectNd;	break;

    case SECTION_SERVER+1:
        return TRUE;
    }

    if( pNd )
    {
        nSttNd = pNd->GetIndex();
        nEndNd = pNd->EndOfSectionIndex();
        nStt = 0, nEnd = USHRT_MAX;
        pNds = &pNd->GetNodes();
    }

    if( nSttNd && nEndNd )
    {
        // LinkManager besorgen:
        const ::sfx2::SvBaseLinks& rLnks = pNds->GetDoc()->GetLinkManager().GetLinks();

// um Rekursionen zu Verhindern: ServerType umsetzen!
SwServerObject::ServerModes eSave = eType;
if( !pChkLnk )
// sowas sollte man nicht tun, wer weiss schon, wie gross ein enum ist
// ICC nimmt keinen int
// #41723#
//	*((int*)&eType) = SECTION_SERVER+1;
    ((SwServerObject*)this)->eType = NONE_SERVER;
        for( USHORT n = rLnks.Count(); n; )
        {
            const ::sfx2::SvBaseLink* pLnk = &(*rLnks[ --n ]);
            if( pLnk && OBJECT_CLIENT_GRF != pLnk->GetObjType() &&
                pLnk->ISA( SwBaseLink ) &&
                !((SwBaseLink*)pLnk)->IsNoDataFlag() &&
                ((SwBaseLink*)pLnk)->IsInRange( nSttNd, nEndNd, nStt, nEnd ))
            {
                if( pChkLnk )
                {
                    if( pLnk == pChkLnk ||
                        ((SwBaseLink*)pLnk)->IsRecursion( pChkLnk ) )
                        return TRUE;
                }
                else if( ((SwBaseLink*)pLnk)->IsRecursion( (SwBaseLink*)pLnk ) )
                    ((SwBaseLink*)pLnk)->SetNoDataFlag();
            }
        }
if( !pChkLnk )
    //	*((int*)&eType) = eSave;
    ((SwServerObject*)this)->eType = eSave;
    }

    return FALSE;
}

/*  */


SwDataChanged::SwDataChanged( const SwPaM& rPam, USHORT nTyp )
    : pPam( &rPam ), pPos( 0 ), pDoc( rPam.GetDoc() ), nType( nTyp )
{
    nNode = rPam.GetPoint()->nNode.GetIndex();
    nCntnt = rPam.GetPoint()->nContent.GetIndex();
}


SwDataChanged::SwDataChanged( SwDoc* pDc, const SwPosition& rPos, USHORT nTyp )
    : pPam( 0 ), pPos( &rPos ), pDoc( pDc ), nType( nTyp )
{
    nNode = rPos.nNode.GetIndex();
    nCntnt = rPos.nContent.GetIndex();
}


SwDataChanged::~SwDataChanged()
{
    // JP 09.04.96: nur wenn das Layout vorhanden ist ( also waehrend der
    //				Eingabe)
    if( pDoc->GetRootFrm() )
    {
        const ::sfx2::SvLinkSources& rServers = pDoc->GetLinkManager().GetServers();

        for( USHORT nCnt = rServers.Count(); nCnt; )
        {
            ::sfx2::SvLinkSourceRef refObj( rServers[ --nCnt ] );
            // noch jemand am Object interessiert ?
            if( refObj->HasDataLinks() && refObj->ISA( SwServerObject ))
            {
                SwServerObject& rObj = *(SwServerObject*)&refObj;
                if( pPos )
                    rObj.SendDataChanged( *pPos );
                else
                    rObj.SendDataChanged( *pPam );
            }

            // sollte jetzt gar keine Verbindung mehr bestehen
            if( !refObj->HasDataLinks() )
            {
                // dann raus aus der Liste (Object bleibt aber bestehen!)
                // falls es noch da ist !!
                if( nCnt < rServers.Count() && &refObj == rServers[ nCnt ] )
                    pDoc->GetLinkManager().RemoveServer( nCnt, 1 );
            }
        }
    }
}




