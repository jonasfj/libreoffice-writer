/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: visiturl.cxx,v $
 * $Revision: 1.10 $
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


#include <sfx2/docfile.hxx>
#include <svtools/inethist.hxx>
#include <fmtinfmt.hxx>
#include <txtinet.hxx>
#include <doc.hxx>
#include <visiturl.hxx>
#include <hints.hxx>
#include <ndtxt.hxx>
#include <editsh.hxx>
#include <docsh.hxx>


SwURLStateChanged::SwURLStateChanged( const SwDoc* pD )
    : pDoc( pD )
{
    StartListening( *INetURLHistory::GetOrCreate() );
}

SwURLStateChanged::~SwURLStateChanged()
{
    EndListening( *INetURLHistory::GetOrCreate() );
}

void SwURLStateChanged::Notify( SfxBroadcaster& , const SfxHint& rHint )
{
    if( rHint.ISA( INetURLHistoryHint ) && pDoc->GetRootFrm() )
    {
        // diese URL wurde veraendert:
        const INetURLObject* pIURL = ((INetURLHistoryHint&)rHint).GetObject();
        String sURL( pIURL->GetMainURL( INetURLObject::NO_DECODE ) ), sBkmk;

        SwEditShell* pESh = pDoc->GetEditShell();

        if( pDoc->GetDocShell() && pDoc->GetDocShell()->GetMedium() &&
            // falls das unser Doc ist, kann es auch lokale Spruenge geben!
            sURL == pDoc->GetDocShell()->GetMedium()->GetName() )
            (sBkmk = pIURL->GetMark()).Insert( INET_MARK_TOKEN, 0 );

        BOOL bAction = FALSE, bUnLockView = FALSE;
        const SwFmtINetFmt* pItem;
        const SwTxtINetFmt* pTxtAttr;
        const SwTxtNode* pTxtNd;
        USHORT n, nMaxItems = pDoc->GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
        for( n = 0; n < nMaxItems; ++n )
            if( 0 != (pItem = (SwFmtINetFmt*)pDoc->GetAttrPool().GetItem(
                RES_TXTATR_INETFMT, n ) ) &&
                ( pItem->GetValue() == sURL ||
                    ( sBkmk.Len() && pItem->GetValue() == sBkmk )) &&
                0 != ( pTxtAttr = pItem->GetTxtINetFmt()) &&
                0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) )
            {
                if( !bAction && pESh )
                {
                    pESh->StartAllAction();
                    bAction = TRUE;
                    bUnLockView = !pESh->IsViewLocked();
                    pESh->LockView( TRUE );
                }
                const_cast<SwTxtINetFmt*>(pTxtAttr)->SetVisitedValid( false );
                const SwTxtAttr* pAttr = pTxtAttr;
                SwUpdateAttr aUpdateAttr( *pAttr->GetStart(),
                                          *pAttr->GetEnd(),
                                          RES_FMT_CHG );
                ((SwTxtNode*)pTxtNd)->Modify( &aUpdateAttr, &aUpdateAttr );
            }

        if( bAction )
            pESh->EndAllAction();
         if( bUnLockView )
             pESh->LockView( FALSE );
    }
}

    // erfrage ob die URL besucht war. Uebers Doc, falls nur ein Bookmark
    // angegeben ist. Dann muss der Doc. Name davor gesetzt werden!
BOOL SwDoc::IsVisitedURL( const String& rURL ) const
{
#if OSL_DEBUG_LEVEL > 1
    static long nTmp = 0;
    ++nTmp;
#endif

    BOOL bRet = FALSE;
    if( rURL.Len() )
    {
        INetURLHistory *pHist = INetURLHistory::GetOrCreate();
        if( '#' == rURL.GetChar( 0 ) && pDocShell && pDocShell->GetMedium() )
        {
            INetURLObject aIObj( pDocShell->GetMedium()->GetURLObject() );
            aIObj.SetMark( rURL.Copy( 1 ) );
            bRet = pHist->QueryUrl( aIObj );
        }
        else
            bRet = pHist->QueryUrl( rURL );

        // dann  wollen wird auch ueber Statusaenderungen in der History
        // informiert werden!
        if( !pURLStateChgd )
        {
            SwDoc* pD = (SwDoc*)this;
            pD->pURLStateChgd = new SwURLStateChanged( this );
        }
    }
    return bRet;
}



