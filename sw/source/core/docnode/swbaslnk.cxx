/*************************************************************************
 *
 *  $RCSfile: swbaslnk.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: kz $ $Date: 2004-10-04 19:05:10 $
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


#pragma hdrstop

#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER

#include <hintids.hxx>

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif

#ifndef _LNKBASE_HXX //autogen
#include <sfx2/lnkbase.hxx>
#endif
#ifndef _LINKMGR_HXX
#include <sfx2/linkmgr.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>		// fuer die EventIds
#endif
#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif
#ifndef _SOERR_HXX
#include <svtools/soerr.hxx>
#endif

#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SWEVENT_HXX
#include <swevent.hxx>
#endif
#ifndef _SWBASLNK_HXX
#include <swbaslnk.hxx>
#endif
#ifndef _SWSERV_HXX
#include <swserv.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _HTMLTBL_HXX
#include <htmltbl.hxx>
#endif

BOOL SetGrfFlySize( const Size& rGrfSz, const Size& rFrmSz, SwGrfNode* pGrfNd );

TYPEINIT1( SwBaseLink, ::sfx2::SvBaseLink );

SV_IMPL_REF( SwServerObject )

SwBaseLink::~SwBaseLink()
{
}

void lcl_CallModify( SwGrfNode& rGrfNd, SfxPoolItem& rItem )
{
    //JP 4.7.2001: call fist all not SwNoTxtFrames, then the SwNoTxtFrames.
    //				The reason is, that in the SwNoTxtFrames the Graphic
    //				after a Paint will be swapped out! So all other "behind"
    //				them havent't a loaded Graphic. - #86501#
    rGrfNd.LockModify();

    SwClientIter aIter( rGrfNd );
    for( int n = 0; n < 2; ++n )
    {
        SwClient * pLast = aIter.GoStart();
        if( pLast ) 	// konnte zum Anfang gesprungen werden ??
        {
            do {
                if( (0 == n) ^ ( 0 != pLast->ISA( SwCntntFrm )) )
                    pLast->Modify( &rItem, &rItem );
            } while( 0 != ( pLast = aIter++ ));
        }
    }
    rGrfNd.UnlockModify();
}


void SwBaseLink::DataChanged( const String& rMimeType,
                            const ::com::sun::star::uno::Any & rValue )
{
    if( !pCntntNode )
    {
        ASSERT(!this, "DataChanged ohne ContentNode" );
        return ;
    }

    SwDoc* pDoc = pCntntNode->GetDoc();
    if( pDoc->IsInDtor() || ChkNoDataFlag() || bIgnoreDataChanged )
    {
        bIgnoreDataChanged = FALSE;
        return ;
    }

    ULONG nFmt = SotExchange::GetFormatIdFromMimeType( rMimeType );

    if( pCntntNode->IsNoTxtNode() &&
        nFmt == SvxLinkManager::RegisterStatusInfoId() )
    {
        // nur eine Statusaenderung - Events bedienen ?
        ::rtl::OUString sState;
        if( rValue.hasValue() && ( rValue >>= sState ))
        {
            USHORT nEvent = 0;
            switch( sState.toInt32() )
            {
            case STATE_LOAD_OK:		nEvent = SVX_EVENT_IMAGE_LOAD;	break;
            case STATE_LOAD_ERROR: 	nEvent = SVX_EVENT_IMAGE_ERROR;	break;
            case STATE_LOAD_ABORT: 	nEvent = SVX_EVENT_IMAGE_ABORT;	break;
            }

            SwFrmFmt* pFmt;
            if( nEvent && 0 != ( pFmt = pCntntNode->GetFlyFmt() ))
            {
                SwCallMouseEvent aCallEvent;
                aCallEvent.Set( EVENT_OBJECT_IMAGE, pFmt );
                pDoc->CallEvent( nEvent, aCallEvent );
            }
        }
        return;			// das wars!
    }

    FASTBOOL bUpdate = FALSE;
    FASTBOOL bGraphicArrived = FALSE;
    FASTBOOL bGraphicPieceArrived = FALSE;
    FASTBOOL bDontNotify = FALSE;
    Size aGrfSz, aFrmFmtSz;

    if( pCntntNode->IsGrfNode() )
    {
        GraphicObject& rGrfObj = ((SwGrfNode*)pCntntNode)->GetGrfObj();

        bDontNotify = ((SwGrfNode*)pCntntNode)->IsFrameInPaint();

        bGraphicArrived = GetObj()->IsDataComplete();
        bGraphicPieceArrived = GetObj()->IsPending();
        ((SwGrfNode*)pCntntNode)->SetGrafikArrived( bGraphicArrived );

        Graphic aGrf;
        if( SvxLinkManager::GetGraphicFromAny( rMimeType, rValue, aGrf ) &&
            ( GRAPHIC_DEFAULT != aGrf.GetType() ||
              GRAPHIC_DEFAULT != rGrfObj.GetType() ) )
        {
            aGrfSz = ::GetGraphicSizeTwip( aGrf, 0 );
            if( static_cast< const SwGrfNode * >( pCntntNode )->IsChgTwipSizeFromPixel() )
            {
                const MapMode aMapTwip( MAP_TWIP );
                aFrmFmtSz =
                    Application::GetDefaultDevice()->PixelToLogic(
                        aGrf.GetSizePixel(), aMapTwip );

            }
            else
            {
                aFrmFmtSz = aGrfSz;
            }
            Size aSz( ((SwGrfNode*)pCntntNode)->GetTwipSize() );

            if( bGraphicPieceArrived && GRAPHIC_DEFAULT != aGrf.GetType() &&
                ( !aSz.Width() || !aSz.Height() ) )
            {
                // wenn nur ein Teil ankommt, aber die Groesse nicht
                // gesetzt ist, dann muss "unten" der Teil von
                // bGraphicArrived durchlaufen werden!
                // (ansonten wird die Grafik in deft. Size gepaintet)
                bGraphicArrived = TRUE;
                bGraphicPieceArrived = FALSE;
            }

            rGrfObj.SetGraphic( aGrf, rGrfObj.GetLink() );
            bUpdate = TRUE;

            // Bug 33999: damit der Node den Transparent-Status
            //		richtig gesetzt hat, ohne auf die Grafik
            //		zugreifen zu muessen (sonst erfolgt ein SwapIn!).
            if( bGraphicArrived )
            {
                // Bug #34735#: immer mit der korrekten Grafik-Size
                //				arbeiten
                if( aGrfSz.Height() && aGrfSz.Width() &&
                    aSz.Height() && aSz.Width() &&
                    aGrfSz != aSz )
                    ((SwGrfNode*)pCntntNode)->SetTwipSize( aGrfSz );
            }
        }
        if ( bUpdate && !bGraphicArrived && !bGraphicPieceArrived )
            ((SwGrfNode*)pCntntNode)->SetTwipSize( Size(0,0) );
    }
    else if( pCntntNode->IsOLENode() )
        bUpdate = TRUE;

    ViewShell *pSh = 0;
    SwEditShell* pESh = pDoc->GetEditShell( &pSh );

    if ( bUpdate && bGraphicPieceArrived && !(bSwapIn || bDontNotify) )
    {
        //Hint ohne Actions verschicken, loest direktes Paint aus.
        if ( (!pSh || !pSh->ActionPend()) && (!pESh || !pESh->ActionPend()) )
        {
            SwMsgPoolItem aMsgHint( RES_GRAPHIC_PIECE_ARRIVED );
            pCntntNode->Modify( &aMsgHint, &aMsgHint );
            bUpdate = FALSE;
        }
    }

    static BOOL bInNotifyLinks = FALSE;
    if( bUpdate && !bDontNotify && (!bSwapIn || bGraphicArrived) &&
        !bInNotifyLinks)
    {
        BOOL bLockView = FALSE;
        if( pSh )
        {
            bLockView = pSh->IsViewLocked();
            pSh->LockView( TRUE );
        }

        if( pESh )
            pESh->StartAllAction();
        else if( pSh )
            pSh->StartAction();

        SwMsgPoolItem aMsgHint( bGraphicArrived ? RES_GRAPHIC_ARRIVED :
                                                  RES_UPDATE_ATTR );

        if ( bGraphicArrived )
        {
            //Alle benachrichtigen, die am gleichen Link horchen.
            bInNotifyLinks = TRUE;

            const ::sfx2::SvBaseLinks& rLnks = pDoc->GetLinkManager().GetLinks();
            for( USHORT n = rLnks.Count(); n; )
            {
                ::sfx2::SvBaseLink* pLnk = &(*rLnks[ --n ]);
                if( pLnk && OBJECT_CLIENT_GRF == pLnk->GetObjType() &&
                    pLnk->ISA( SwBaseLink ) && pLnk->GetObj() == GetObj() )
                {
                    SwBaseLink* pBLink = (SwBaseLink*)pLnk;
                    SwGrfNode* pGrfNd = (SwGrfNode*)pBLink->pCntntNode;

                    if( pBLink != this &&
                        ( !bSwapIn ||
                            GRAPHIC_DEFAULT == pGrfNd->GetGrfObj().GetType()))
                    {
                        pBLink->bIgnoreDataChanged = FALSE;
                        pBLink->DataChanged( rMimeType, rValue );
                        pBLink->bIgnoreDataChanged = TRUE;

                        pGrfNd->SetGrafikArrived( ((SwGrfNode*)pCntntNode)->
                                                    IsGrafikArrived() );

                        // Fly der Grafik anpassen !
                        if( !::SetGrfFlySize( aGrfSz, aFrmFmtSz, pGrfNd ) )
                            ::lcl_CallModify( *pGrfNd, aMsgHint );
                    }
                    else if( pBLink == this &&
                            !::SetGrfFlySize( aGrfSz, aFrmFmtSz, pGrfNd ) )
                        // Fly der Grafik anpassen !
                        ::lcl_CallModify( *pGrfNd, aMsgHint );
                }
            }

            bInNotifyLinks = FALSE;
        }
        else
        {
            pCntntNode->Modify( &aMsgHint, &aMsgHint );
        }


        if( pESh )
        {
            const BOOL bEndActionByVirDev = pESh->IsEndActionByVirDev();
            pESh->SetEndActionByVirDev( TRUE );
            pESh->EndAllAction();
            pESh->SetEndActionByVirDev( bEndActionByVirDev );
        }
        else if( pSh )
            pSh->EndAction();

        if( pSh && !bLockView )
            pSh->LockView( FALSE );
    }
}

FASTBOOL SwBaseLink::IsShowQuickDrawBmp() const
{
    return pCntntNode && pCntntNode->IsGrfNode() &&
#ifdef NEW_GRFOBJ
            ((SwGrfNode*)pCntntNode)->HasMagicId()
#else
            // wie kommt man an die Info dran, das eine Grafik im Cache steht?
            FALSE
#endif
        ;
}


BOOL SetGrfFlySize( const Size& rGrfSz, const Size& rFrmSz, SwGrfNode* pGrfNd )
{
    BOOL bRet = FALSE;
    ViewShell *pSh;
    CurrShell *pCurr = 0;
    if ( pGrfNd->GetDoc()->GetEditShell( &pSh ) )
        pCurr = new CurrShell( pSh );

    Size aSz = pGrfNd->GetTwipSize();
    if ( !(aSz.Width() && aSz.Height()) &&
            rGrfSz.Width() && rGrfSz.Height() )
    {
        SwFrmFmt* pFmt;
        if( pGrfNd->IsChgTwipSize() &&
            0 != (pFmt = pGrfNd->GetFlyFmt()) )
        {
            Size aCalcSz( aSz );
            if ( !aSz.Height() && aSz.Width() )
                //passende Hoehe ausrechnen.
                aCalcSz.Height() = rFrmSz.Height() *
                        aSz.Width() / rFrmSz.Width();
            else if ( !aSz.Width() && aSz.Height() )
                //passende Breite ausrechnen
                aCalcSz.Width() = rFrmSz.Width() *
                        aSz.Height() / rFrmSz.Height();
            else
                //Hoehe und Breite uebernehmen
                aCalcSz = rFrmSz;

            const SvxBoxItem 	 &rBox = pFmt->GetBox();
            aCalcSz.Width() += rBox.CalcLineSpace(BOX_LINE_LEFT) +
                               rBox.CalcLineSpace(BOX_LINE_RIGHT);
            aCalcSz.Height()+= rBox.CalcLineSpace(BOX_LINE_TOP) +
                               rBox.CalcLineSpace(BOX_LINE_BOTTOM);
            const SwFmtFrmSize& rOldAttr = pFmt->GetFrmSize();
            if( rOldAttr.GetSize() != aCalcSz )
            {
                SwFmtFrmSize aAttr( rOldAttr  );
                aAttr.SetSize( aCalcSz );
                pFmt->SetAttr( aAttr );
                bRet = TRUE;
            }

            if( !aSz.Width() )
            {
                // Wenn die Grafik in einer Tabelle verankert ist, muess
                // die Tabellen-Spalten neu berechnet werden
                const SwDoc *pDoc = pGrfNd->GetDoc();
                const SwPosition* pAPos = pFmt->GetAnchor().GetCntntAnchor();
                SwNode *pANd;
                SwTableNode *pTblNd;
                if( pAPos &&
                    0 != (pANd = pDoc->GetNodes()[pAPos->nNode]) &&
                    0 != (pTblNd = pANd->FindTableNode()) )
                {
                    BOOL bLastGrf = !pTblNd->GetTable().DecGrfsThatResize();
                    SwHTMLTableLayout *pLayout =
                        pTblNd->GetTable().GetHTMLTableLayout();
                    if(	pLayout )
                    {
                        USHORT nBrowseWidth =
                            pLayout->GetBrowseWidthByTable( *pDoc );
                        if( nBrowseWidth )
                            pLayout->Resize( nBrowseWidth, TRUE, TRUE,
                                             bLastGrf ? HTMLTABLE_RESIZE_NOW
                                                      : 500 );
                    }
                }
            }
        }

        // SetTwipSize skaliert ggf. eine ImageMap neu und
        // braucht dazu das Frame-Format
        pGrfNd->SetTwipSize( rGrfSz );
    }

    delete pCurr;

    return bRet;
}

FASTBOOL SwBaseLink::SwapIn( BOOL bWaitForData, BOOL bNativFormat )
{
    bSwapIn = TRUE;

    FASTBOOL bRes;

    if( !GetObj() && ( bNativFormat || ( !IsSynchron() && bWaitForData ) ))
    {
        AddNextRef();
        _GetRealObject();
        ReleaseRef();
    }

#if OSL_DEBUG_LEVEL > 1
    {
        String sGrfNm;
        GetLinkManager()->GetDisplayNames( this, 0, &sGrfNm, 0, 0 );
        int x = 0;
    }
#endif

    if( GetObj() )
    {
        String aMimeType( SotExchange::GetFormatMimeType( GetContentType() ));

//!! ??? what have we here to do ????
//!!		if( bNativFormat )
//!!			aData.SetAspect( aData.GetAspect() | ASPECT_ICON );

        ::com::sun::star::uno::Any aValue;
        GetObj()->GetData( aValue, aMimeType, !IsSynchron() && bWaitForData );

        if( bWaitForData && !GetObj() )
        {
            ASSERT( !this, "das SvxFileObject wurde in einem GetData geloescht!" );
            bRes = FALSE;
        }
        else if( 0 != ( bRes = aValue.hasValue() ) )
        {
            //JP 14.04.99: Bug 64820 - das Flag muss beim SwapIn natuerlich
            //				zurueckgesetzt werden. Die Daten sollen ja neu
            //				uebernommen werden
            bIgnoreDataChanged = FALSE;
            DataChanged( aMimeType, aValue );
        }
    }
    else if( !IsSynchron() && bWaitForData )
    {
        SetSynchron( TRUE );
        bRes = Update();
        SetSynchron( FALSE );
    }
    else
        bRes = Update();

    bSwapIn = FALSE;
    return bRes;
}

void SwBaseLink::Closed()
{
    if( pCntntNode && !pCntntNode->GetDoc()->IsInDtor() )
    {
        // wir heben die Verbindung auf
        if( pCntntNode->IsGrfNode() )
            ((SwGrfNode*)pCntntNode)->ReleaseLink();
    }
    SvBaseLink::Closed();
}

const SwNode* SwBaseLink::GetAnchor() const
{
    SwFrmFmt* pFmt;
    if( pCntntNode && 0 != ( pFmt = pCntntNode->GetFlyFmt()) )
    {
        const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
        const SwPosition* pAPos;
        if( 0 != ( pAPos = rAnchor.GetCntntAnchor()) &&
            ( FLY_IN_CNTNT == rAnchor.GetAnchorId() ||
            FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ||
            FLY_AT_FLY == rAnchor.GetAnchorId() ||
            FLY_AT_CNTNT == rAnchor.GetAnchorId() ))
                return &pAPos->nNode.GetNode();
        return 0;
    }

    ASSERT( !this, "GetAnchor nicht ueberlagert" );
    return 0;
}

BOOL SwBaseLink::IsRecursion( const SwBaseLink* pChkLnk ) const
{
    SwServerObjectRef aRef( (SwServerObject*)GetObj() );
    if( aRef.Is() )
    {
        // es ist ein ServerObject, also frage nach allen darin
        // befindlichen Links, ob wir darin enthalten sind. Ansonsten
        // handelt es sich um eine Rekursion.
        return aRef->IsLinkInServer( pChkLnk );
    }
    return FALSE;
}

BOOL SwBaseLink::IsInRange( ULONG, ULONG, xub_StrLen, xub_StrLen ) const
{
    // Grafik oder OLE-Links nicht,
    // Felder oder Sections haben eigene Ableitung!
    return FALSE;
}




