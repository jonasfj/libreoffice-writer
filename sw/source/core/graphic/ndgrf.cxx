/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ndgrf.cxx,v $
 *
 *  $Revision: 1.43 $
 *
 *  last change: $Author: obo $ $Date: 2008-02-26 10:40:28 $
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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>             // FRound
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _UNDO_HXX //autogen
#include <svtools/undo.hxx>
#endif
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svtools/fstathelper.hxx>
#endif
#ifndef _IMAP_HXX //autogen
#include <svtools/imap.hxx>
#endif
#ifndef _FILTER_HXX //autogen
#include <svtools/filter.hxx>
#endif
#include <sot/storage.hxx>
#ifndef _SFXDOCINF_HXX //autogen
#include <sfx2/docinf.hxx>
#endif
#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX //autogen
#include <svx/impgrf.hxx>
#endif
#ifndef _SOT_FORMATS_HXX
#include <sot/formats.hxx>
#endif

#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif
#ifndef _FMTURL_HXX
#include <fmturl.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _SWBASLNK_HXX
#include <swbaslnk.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif

#include <unotools/ucbstreamhelper.hxx>

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif
#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif
// --> OD 2007-03-28 #i73788#
#ifndef _RETRIEVEINPUTSTREAMCONSUMER_HXX
#include <retrieveinputstreamconsumer.hxx>
#endif
// <--

using namespace com::sun::star;

// --------------------
// SwGrfNode
// --------------------
SwGrfNode::SwGrfNode(
        const SwNodeIndex & rWhere,
        const String& rGrfName, const String& rFltName,
        const Graphic* pGraphic,
        SwGrfFmtColl *pGrfColl,
        SwAttrSet* pAutoAttr ) :
    SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
    // --> OD 2007-01-23 #i73788#
    mbLinkedInputStreamReady( false ),
    mbIsStreamReadOnly( sal_False )
    // <--
{
    aGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ) );
    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel = bLoadLowResGrf =
        bFrameInPaint = bScaleImageMap = FALSE;

    bGrafikArrived = TRUE;
    ReRead(rGrfName,rFltName, pGraphic, 0, FALSE);
}

SwGrfNode::SwGrfNode( const SwNodeIndex & rWhere,
                          const GraphicObject& rGrfObj,
                      SwGrfFmtColl *pGrfColl, SwAttrSet* pAutoAttr ) :
    SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
    // --> OD 2007-01-23 #i73788#
    mbLinkedInputStreamReady( false ),
    mbIsStreamReadOnly( sal_False )
    // <--
{
    aGrfObj = rGrfObj;
    aGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ) );
    if( rGrfObj.HasUserData() && rGrfObj.IsSwappedOut() )
        aGrfObj.SetSwapState();
    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel= bLoadLowResGrf =
        bFrameInPaint = bScaleImageMap = FALSE;
    bGrafikArrived = TRUE;
}

// Konstruktor fuer den SW/G-Reader. Dieser ctor wird verwendet,
// wenn eine gelinkte Grafik gelesen wird. Sie liest diese NICHT ein.


SwGrfNode::SwGrfNode( const SwNodeIndex & rWhere,
                      const String& rGrfName, const String& rFltName,
                      SwGrfFmtColl *pGrfColl,
                      SwAttrSet* pAutoAttr ) :
    SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
    // --> OD 2007-01-23 #i73788#
    mbLinkedInputStreamReady( false ),
    mbIsStreamReadOnly( sal_False )
    // <--
{
    aGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ) );

    Graphic aGrf; aGrf.SetDefaultType();
    aGrfObj.SetGraphic( aGrf, rGrfName );

    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel = bLoadLowResGrf =
        bFrameInPaint = bScaleImageMap = FALSE;
    bGrafikArrived = TRUE;

    InsertLink( rGrfName, rFltName );
    if( IsLinkedFile() )
    {
        INetURLObject aUrl( rGrfName );
        if( INET_PROT_FILE == aUrl.GetProtocol() &&
            FStatHelper::IsDocument( aUrl.GetMainURL( INetURLObject::NO_DECODE ) ))
        {
            // File vorhanden, Verbindung herstellen ohne ein Update
            ((SwBaseLink*)&refLink)->Connect();
        }
    }
}

BOOL SwGrfNode::ReRead(
    const String& rGrfName, const String& rFltName,
    const Graphic* pGraphic, const GraphicObject* pGrfObj,
    BOOL bNewGrf )
{
    BOOL bReadGrf = FALSE, bSetTwipSize = TRUE;

    ASSERT( pGraphic || pGrfObj || rGrfName.Len(),
            "GraphicNode without a name, Graphic or GraphicObject" );

    // ReadRead mit Namen
    if( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "ReRead: stehe noch im SwapIn" );

        if( rGrfName.Len() )
        {
            // Besonderheit: steht im FltNamen DDE, handelt es sich um eine
            //					DDE-gelinkte Grafik
            String sCmd( rGrfName );
            if( rFltName.Len() )
            {
                USHORT nNewType;
                if( rFltName.EqualsAscii( "DDE" ))
                    nNewType = OBJECT_CLIENT_DDE;
                else
                {
                    sfx2::MakeLnkName( sCmd, 0, rGrfName, aEmptyStr, &rFltName );
                    nNewType = OBJECT_CLIENT_GRF;
                }

                if( nNewType != refLink->GetObjType() )
                {
                    refLink->Disconnect();
                    ((SwBaseLink*)&refLink)->SetObjType( nNewType );
                }
            }

            refLink->SetLinkSourceName( sCmd );
        }
        else		// kein Name mehr, Link aufheben
        {
            GetDoc()->GetLinkManager().Remove( refLink );
            refLink.Clear();
        }

        if( pGraphic )
        {
            aGrfObj.SetGraphic( *pGraphic, rGrfName );
            bReadGrf = TRUE;
        }
        else if( pGrfObj )
        {
            aGrfObj = *pGrfObj;
            if( pGrfObj->HasUserData() && pGrfObj->IsSwappedOut() )
                aGrfObj.SetSwapState();
            aGrfObj.SetLink( rGrfName );
            bReadGrf = TRUE;
        }
        else
        {
            // MIB 25.02.97: Daten der alten Grafik zuruecksetzen, damit
            // die korrekte Ersatz-Darstellung erscheint, wenn die
            // der neue Link nicht geladen werden konnte.
            Graphic aGrf; aGrf.SetDefaultType();
            aGrfObj.SetGraphic( aGrf, rGrfName );

            if( refLink.Is() )
            {
                if( GetFrm() )
                {
                    SwMsgPoolItem aMsgHint( RES_GRF_REREAD_AND_INCACHE );
                    Modify( &aMsgHint, &aMsgHint );
                }
                // --> OD 2006-11-03 #i59688#
                // do not load linked graphic, if it isn't a new linked graphic.
//                else {
                else if ( bNewGrf )
                // <--
                {
                    //TODO refLink->setInputStream(getInputStream());
                    ((SwBaseLink*)&refLink)->SwapIn();
                }
            }
            bSetTwipSize = FALSE;
        }
    }
    else if( pGraphic && !rGrfName.Len() )
    {
        // MIB 27.02.2001: Old stream must be deleted before the new one is set.
        if( HasStreamName() )
            DelStreamName();

        aGrfObj.SetGraphic( *pGraphic );
        bReadGrf = TRUE;
    }
    else if( pGrfObj && !rGrfName.Len() )
    {
        // MIB 27.02.2001: Old stream must be deleted before the new one is set.
        if( HasStreamName() )
            DelStreamName();

        aGrfObj = *pGrfObj;
        if( pGrfObj->HasUserData() && pGrfObj->IsSwappedOut() )
            aGrfObj.SetSwapState();
        bReadGrf = TRUE;
    }
        // Import einer Grafik:
        // Ist die Grafik bereits geladen?
    else if( !bNewGrf && GRAPHIC_NONE != aGrfObj.GetType() )
        return TRUE;

    else
    {
        if( HasStreamName() )
            DelStreamName();

        // einen neuen Grafik-Link anlegen
        InsertLink( rGrfName, rFltName );

        if( GetNodes().IsDocNodes() )
        {
            if( pGraphic )
            {
                aGrfObj.SetGraphic( *pGraphic, rGrfName );
                bReadGrf = TRUE;
                // Verbindung herstellen ohne ein Update; Grafik haben wir!
                ((SwBaseLink*)&refLink)->Connect();
            }
            else if( pGrfObj )
            {
                aGrfObj = *pGrfObj;
                aGrfObj.SetLink( rGrfName );
                bReadGrf = TRUE;
                // Verbindung herstellen ohne ein Update; Grafik haben wir!
                ((SwBaseLink*)&refLink)->Connect();
            }
            else
            {
                // MIB 25.02.97: Daten der alten Grafik zuruecksetzen, damit
                // die korrekte Ersatz-Darstellung erscheint, wenn die
                // der neue Kink nicht geladen werden konnte.
                Graphic aGrf; aGrf.SetDefaultType();
                aGrfObj.SetGraphic( aGrf, rGrfName );
                // --> OD 2006-11-03 #i59688#
                // do not load linked graphic, if it isn't a new linked graphic.
//                //TODO refLink->setInputStream(getInputStream());
//                ((SwBaseLink*)&refLink)->SwapIn();
                if ( bNewGrf )
                {
                    ((SwBaseLink*)&refLink)->SwapIn();
                }
                // <--
            }
        }
    }

    // Bug 39281: Size nicht sofort loeschen - Events auf ImageMaps
    //			  sollten nicht beim Austauschen nicht ins "leere greifen"
    if( bSetTwipSize )
        SetTwipSize( ::GetGraphicSizeTwip( aGrfObj.GetGraphic(), 0 ) );

    // erzeuge noch einen Update auf die Frames
    if( bReadGrf && bNewGrf )
    {
        SwMsgPoolItem aMsgHint( RES_UPDATE_ATTR );
        Modify( &aMsgHint, &aMsgHint );
    }

    return bReadGrf;
}


SwGrfNode::~SwGrfNode()
{
    // --> OD 2007-03-30 #i73788#
    mpThreadConsumer.reset();
    // <--

    SwDoc* pDoc = GetDoc();
    if( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "DTOR: stehe noch im SwapIn" );
        pDoc->GetLinkManager().Remove( refLink );
        refLink->Disconnect();
    }
    else
    {
        // --> OD 2005-01-19 #i40014# - A graphic node, which are in linked
        // section, whose link is another section is the document, doesn't
        // have to remove the stream from the storage.
        // Because it's hard to detect this case here and it would only fix
        // one problem with shared graphic files - there are also problems,
        // a certain graphic file is referenced by two independent graphic nodes,
        // brush item or drawing objects, the stream isn't no longer removed here.
        // To do this stuff correct, a reference counting on shared streams
        // inside one document have to be implemented.
//        if( !pDoc->IsInDtor() && HasStreamName() )
//          DelStreamName();
        // <--
    }
    //#39289# Die Frames muessen hier bereits geloescht weil der DTor der
    //Frms die Grafik noch fuer StopAnimation braucht.
    if( GetDepends() )
        DelFrms();
}


SwCntntNode *SwGrfNode::SplitCntntNode( const SwPosition & )
{
    return this;
}


SwGrfNode * SwNodes::MakeGrfNode( const SwNodeIndex & rWhere,
                                const String& rGrfName,
                                const String& rFltName,
                                const Graphic* pGraphic,
                                SwGrfFmtColl* pGrfColl,
                                SwAttrSet* pAutoAttr,
                                BOOL bDelayed )
{
    ASSERT( pGrfColl, "MakeGrfNode: Formatpointer ist 0." );
    SwGrfNode *pNode;
    // Delayed erzeugen nur aus dem SW/G-Reader
    if( bDelayed )
        pNode = new SwGrfNode( rWhere, rGrfName,
                                rFltName, pGrfColl, pAutoAttr );
    else
        pNode = new SwGrfNode( rWhere, rGrfName,
                                rFltName, pGraphic, pGrfColl, pAutoAttr );
    return pNode;
}

SwGrfNode * SwNodes::MakeGrfNode( const SwNodeIndex & rWhere,
                                const GraphicObject& rGrfObj,
                                SwGrfFmtColl* pGrfColl,
                                SwAttrSet* pAutoAttr )
{
    ASSERT( pGrfColl, "MakeGrfNode: Formatpointer ist 0." );
    return new SwGrfNode( rWhere, rGrfObj, pGrfColl, pAutoAttr );
}


Size SwGrfNode::GetTwipSize() const
{
    return nGrfSize;
}



BOOL SwGrfNode::ImportGraphic( SvStream& rStrm )
{
    Graphic aGraphic;
    if( !GetGrfFilter()->ImportGraphic( aGraphic, String(), rStrm ) )
    {
        const String aUserData( aGrfObj.GetUserData() );

        aGrfObj.SetGraphic( aGraphic );
        aGrfObj.SetUserData( aUserData );
        return TRUE;
    }

    return FALSE;
}

// Returnwert:
// -1 : ReRead erfolgreich
//  0 : nicht geladen
//  1 : Einlesen erfolgreich

short SwGrfNode::SwapIn( BOOL bWaitForData )
{
    if( bInSwapIn )					// nicht rekuriv!!
        return !aGrfObj.IsSwappedOut();

    short nRet = 0;
    bInSwapIn = TRUE;
    SwBaseLink* pLink = (SwBaseLink*)(::sfx2::SvBaseLink*) refLink;

    if( pLink )
    {
        if( GRAPHIC_NONE == aGrfObj.GetType() ||
            GRAPHIC_DEFAULT == aGrfObj.GetType() )
        {
            // noch nicht geladener Link
            //TODO pLink->setInputStream(getInputStream());
            if( pLink->SwapIn( bWaitForData ) )
                nRet = -1;
            else if( GRAPHIC_DEFAULT == aGrfObj.GetType() )
            {
                // keine default Bitmap mehr, also neu Painten!
                aGrfObj.SetGraphic( Graphic() );
                SwMsgPoolItem aMsgHint( RES_GRAPHIC_PIECE_ARRIVED );
                Modify( &aMsgHint, &aMsgHint );
            }
        }
        else if( aGrfObj.IsSwappedOut() ) {
            // nachzuladender Link
            //TODO pLink->setInputStream(getInputStream());
            nRet = pLink->SwapIn( bWaitForData ) ? 1 : 0;
        }
        else
            nRet = 1;
    }
    else if( aGrfObj.IsSwappedOut() )
    {
        // Die Grafik ist im Storage oder im TempFile drin
        if( !HasStreamName() )
            nRet = (short)aGrfObj.SwapIn();
        else
        {

            // --> OD 2005-05-04 #i48434# - usage of new method <_GetStreamForEmbedGrf(..)>
            try
            {
                // --> OD, MAV 2005-08-17 #i53025# - needed correction of new
                // method <_GetStreamForEmbedGrf(..)>
//                bool bGraphic(false);
//                SvStream* pStrm = _GetStreamForEmbedGrf( bGraphic );
                String aStrmName, aPicStgName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference < embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
                SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
                if ( pStrm )
                {
                    if ( ImportGraphic( *pStrm ) )
                        nRet = 1;
                    delete pStrm;
                }
                // <--
            }
            catch ( uno::Exception& )
            {
                // --> OD 2005-04-25 #i48434#
                ASSERT( false, "<SwGrfNode::SwapIn(..)> - unhandled exception!" );
                // <--
            }
            // <--
        }

        if( 1 == nRet )
        {
            SwMsgPoolItem aMsg( RES_GRAPHIC_SWAPIN );
            SwCntntNode::Modify( &aMsg, &aMsg );
        }
    }
    else
        nRet = 1;
    DBG_ASSERTWARNING( nRet, "Grafik kann nicht eingeswapt werden" );

    if( nRet )
    {
        if( !nGrfSize.Width() && !nGrfSize.Height() )
            SetTwipSize( ::GetGraphicSizeTwip( aGrfObj.GetGraphic(), 0 ) );
    }
    bInSwapIn = FALSE;
    return nRet;
}


short SwGrfNode::SwapOut()
{
    if( aGrfObj.GetType() != GRAPHIC_DEFAULT &&
        aGrfObj.GetType() != GRAPHIC_NONE &&
        !aGrfObj.IsSwappedOut() && !bInSwapIn )
    {
        if( !refLink.Is() )
        {
            // Das Swapping brauchen wir nur fuer Embedded Pictures
            // Die Grafik wird in eine TempFile geschrieben, wenn
            // sie frisch eingefuegt war, d.h. wenn es noch keinen
            // Streamnamen im Storage gibt.
            if( !HasStreamName() )
                if( !aGrfObj.SwapOut() )
                    return 0;
        }
        // Geschriebene Grafiken oder Links werden jetzt weggeschmissen
        return (short) aGrfObj.SwapOut( NULL );
    }
    return 1;
}


BOOL SwGrfNode::GetFileFilterNms( String* pFileNm, String* pFilterNm ) const
{
    BOOL bRet = FALSE;
    if( refLink.Is() && refLink->GetLinkManager() )
    {
        USHORT nType = refLink->GetObjType();
        if( OBJECT_CLIENT_GRF == nType )
            bRet = refLink->GetLinkManager()->GetDisplayNames(
                    refLink, 0, pFileNm, 0, pFilterNm );
        else if( OBJECT_CLIENT_DDE == nType && pFileNm && pFilterNm )
        {
            String sApp, sTopic, sItem;
            if( refLink->GetLinkManager()->GetDisplayNames(
                    refLink, &sApp, &sTopic, &sItem ) )
            {
                ( *pFileNm = sApp ) += sfx2::cTokenSeperator;
                ( *pFileNm += sTopic ) += sfx2::cTokenSeperator;
                *pFileNm += sItem;
                pFilterNm->AssignAscii( RTL_CONSTASCII_STRINGPARAM( "DDE" ));
                bRet = TRUE;
            }
        }
    }
    return bRet;
}


// Eine Grafik Undo-faehig machen. Falls sie sich bereits in
// einem Storage befindet, muss sie geladen werden.

BOOL SwGrfNode::SavePersistentData()
{
    if( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "SavePersistentData: stehe noch im SwapIn" );
        GetDoc()->GetLinkManager().Remove( refLink );
        return TRUE;
    }

    // Erst mal reinswappen, falls sie im Storage ist
    if( HasStreamName() && !SwapIn() )
        return FALSE;

    // --> OD 2005-04-19 #i44367#
    // Do not delete graphic file in storage, because the graphic file could
    // be referenced by other graphic nodes.
    // Because it's hard to detect this case here and it would only fix
    // one problem with shared graphic files - there are also problems,
    // a certain graphic file is referenced by two independent graphic nodes,
    // brush item or drawing objects, the stream isn't no longer removed here.
    // To do this stuff correct, a reference counting on shared streams
    // inside one document have to be implemented.
    // Important note: see also fix for #i40014#
//    if( HasStreamName() )
//        DelStreamName();
    // <--

    // Und in TempFile rausswappen
    return (BOOL) SwapOut();
}


BOOL SwGrfNode::RestorePersistentData()
{
    if( refLink.Is() )
    {
        IDocumentLinksAdministration* pIDLA = getIDocumentLinksAdministration();
        refLink->SetVisible( pIDLA->IsVisibleLinks() );
        pIDLA->GetLinkManager().InsertDDELink( refLink );
        if( getIDocumentLayoutAccess()->GetRootFrm() )
            refLink->Update();
    }
    return TRUE;
}


void SwGrfNode::InsertLink( const String& rGrfName, const String& rFltName )
{
    refLink = new SwBaseLink( sfx2::LINKUPDATE_ONCALL, FORMAT_GDIMETAFILE, this );

    IDocumentLinksAdministration* pIDLA = getIDocumentLinksAdministration();
    if( GetNodes().IsDocNodes() )
    {
        refLink->SetVisible( pIDLA->IsVisibleLinks() );
        if( rFltName.EqualsAscii( "DDE" ))
        {
            USHORT nTmp = 0;
            String sApp, sTopic, sItem;
            sApp = rGrfName.GetToken( 0, sfx2::cTokenSeperator, nTmp );
            sTopic = rGrfName.GetToken( 0, sfx2::cTokenSeperator, nTmp );
            sItem = rGrfName.Copy( nTmp );
            pIDLA->GetLinkManager().InsertDDELink( refLink,
                                            sApp, sTopic, sItem );
        }
        else
        {
            BOOL bSync = rFltName.EqualsAscii( "SYNCHRON" );
            refLink->SetSynchron( bSync );
            refLink->SetContentType( SOT_FORMATSTR_ID_SVXB );

            pIDLA->GetLinkManager().InsertFileLink( *refLink,
                                            OBJECT_CLIENT_GRF, rGrfName,
                                (!bSync && rFltName.Len() ? &rFltName : 0) );
        }
    }
    aGrfObj.SetLink( rGrfName );
}


void SwGrfNode::ReleaseLink()
{
    if( refLink.Is() )
    {
        // erst die Grafik reinswappen!
//		if( aGraphic.IsSwapOut() || !refLink->IsSynchron() )
        {
            bInSwapIn = TRUE;
            SwBaseLink* pLink = (SwBaseLink*)(::sfx2::SvBaseLink*) refLink;
            //TODO pLink->setInputStream(getInputStream());
            pLink->SwapIn( TRUE, TRUE );
            bInSwapIn = FALSE;
        }
        getIDocumentLinksAdministration()->GetLinkManager().Remove( refLink );
        refLink.Clear();
        aGrfObj.SetLink();
    }
}


void SwGrfNode::SetTwipSize( const Size& rSz )
{
    nGrfSize = rSz;
    if( IsScaleImageMap() && nGrfSize.Width() && nGrfSize.Height() )
    {
        // Image-Map an Grafik-Groesse anpassen
        ScaleImageMap();

        // Image-Map nicht noch einmal skalieren
        SetScaleImageMap( FALSE );
    }
}

        // Prioritaet beim Laden der Grafik setzen. Geht nur, wenn der Link
        // ein FileObject gesetzt hat
void SwGrfNode::SetTransferPriority( USHORT nPrio )
{
    if( refLink.Is() && refLink->GetObj() )
        SvxLinkManager::SetTransferPriority( *refLink, nPrio );
}


void SwGrfNode::ScaleImageMap()
{
    if( !nGrfSize.Width() || !nGrfSize.Height() )
        return;

    // dann die Image-Map skalieren
    SwFrmFmt* pFmt = GetFlyFmt();

    if( !pFmt )
        return;

    SwFmtURL aURL( pFmt->GetURL() );
    if ( !aURL.GetMap() )
        return;

    BOOL bScale = FALSE;
    Fraction aScaleX( 1, 1 );
    Fraction aScaleY( 1, 1 );

    const SwFmtFrmSize& rFrmSize = pFmt->GetFrmSize();
    const SvxBoxItem& rBox = pFmt->GetBox();

    if( !rFrmSize.GetWidthPercent() )
    {
        SwTwips nWidth = rFrmSize.GetWidth();

        nWidth -= rBox.CalcLineSpace(BOX_LINE_LEFT) +
                  rBox.CalcLineSpace(BOX_LINE_RIGHT);

        ASSERT( nWidth>0, "Gibt es 0 twip breite Grafiken!?" );

        if( nGrfSize.Width() != nWidth )
        {
            aScaleX = Fraction( nGrfSize.Width(), nWidth );
            bScale = TRUE;
        }
    }
    if( !rFrmSize.GetHeightPercent() )
    {
        SwTwips nHeight = rFrmSize.GetHeight();

        nHeight -= rBox.CalcLineSpace(BOX_LINE_TOP) +
                   rBox.CalcLineSpace(BOX_LINE_BOTTOM);

        ASSERT( nHeight>0, "Gibt es 0 twip hohe Grafiken!?" );

        if( nGrfSize.Height() != nHeight )
        {
            aScaleY = Fraction( nGrfSize.Height(), nHeight );
            bScale = TRUE;
        }
    }

    if( bScale )
    {
        aURL.GetMap()->Scale( aScaleX, aScaleY );
        pFmt->SetAttr( aURL );
    }
}


void SwGrfNode::DelStreamName()
{
    if( HasStreamName() )
    {
        // Dann die Grafik im Storage loeschen
        uno::Reference < embed::XStorage > xDocStg = GetDoc()->GetDocStorage();
        if( xDocStg.is() )
        {
            try
            {
                String aPicStgName, aStrmName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference < embed::XStorage > refPics = xDocStg;
                if ( aPicStgName.Len() )
                    refPics = xDocStg->openStorageElement( aPicStgName, embed::ElementModes::READWRITE );
                refPics->removeElement( aStrmName );
                uno::Reference < embed::XTransactedObject > xTrans( refPics, uno::UNO_QUERY );
                if ( xTrans.is() )
                    xTrans->commit();
            }
            catch ( uno::Exception& )
            {
                // --> OD 2005-04-25 #i48434#
                ASSERT( false, "<SwGrfNode::DelStreamName()> - unhandled exception!" );
                // <--
            }
        }

        aGrfObj.SetUserData();
    }
}

/** helper method to get a substorage of the document storage for readonly access.

    OD, MAV 2005-08-17 #i53025#
    A substorage with the specified name will be opened readonly. If the provided
    name is empty the root storage will be returned.
*/
uno::Reference< embed::XStorage > SwGrfNode::_GetDocSubstorageOrRoot( const String& aStgName ) const
{
    uno::Reference < embed::XStorage > refStor =
        const_cast<SwGrfNode*>(this)->GetDoc()->GetDocStorage();
    ASSERT( refStor.is(), "Kein Storage am Doc" );

    if ( aStgName.Len() )
    {
        if( refStor.is() )
            return refStor->openStorageElement( aStgName, embed::ElementModes::READ );
    }

    return refStor;
}

/** helper method to determine stream for the embedded graphic.

    OD 2005-05-04 #i48434#
    Important note: caller of this method has to handle the thrown exceptions
    OD, MAV 2005-08-17 #i53025#
    Storage, which should contain the stream of the embedded graphic, is
    provided via parameter. Otherwise the returned stream will be closed
    after the the method returns, because its parent stream is closed and deleted.
    Proposed name of embedded graphic stream is also provided by parameter.

    @author OD
*/
SvStream* SwGrfNode::_GetStreamForEmbedGrf(
            const uno::Reference< embed::XStorage >& _refPics,
            String& _aStrmName ) const
{
    SvStream* pStrm( 0L );

    if( _refPics.is() && _aStrmName.Len() )
    {
        // If stream doesn't exist in the storage, try access the graphic file by
        // re-generating its name.
        // A save action can have changed the filename of the embedded graphic,
        // because a changed unique ID of the graphic is calculated.
        // --> OD 2006-01-30 #b6364738#
        // recursive calls of <GetUniqueID()> have to be avoided.
        // Thus, use local static boolean to assure this.
        static bool bInRegenerateStrmName( false );
        if ( !bInRegenerateStrmName &&
             ( !_refPics->hasByName( _aStrmName ) ||
               !_refPics->isStreamElement( _aStrmName ) ) )
        {
            bInRegenerateStrmName = true;
            xub_StrLen nExtPos = _aStrmName.Search( '.' );
            String aExtStr = _aStrmName.Copy( nExtPos );
            Graphic aGraphic( GetGrfObj().GetGraphic() );
            if ( aGraphic.GetType() != GRAPHIC_NONE )
            {
                _aStrmName = String( GetGrfObj().GetUniqueID(), RTL_TEXTENCODING_ASCII_US );
                _aStrmName += aExtStr;
            }
            bInRegenerateStrmName = false;
        }
        // <--

        // assure that graphic file exist in the storage.
        if ( _refPics->hasByName( _aStrmName ) &&
             _refPics->isStreamElement( _aStrmName ) )
        {
            uno::Reference < io::XStream > refStrm = _refPics->openStreamElement( _aStrmName, embed::ElementModes::READ );
            pStrm = utl::UcbStreamHelper::CreateStream( refStrm );
        }
        else
        {
            ASSERT( false, "<SwGrfNode::_GetStreamForEmbedGrf(..)> - embedded graphic file not found!" );
        }
    }

    return pStrm;
}


// --> OD 2005-08-17 #i53025# - stream couldn't be in a 3.1 - 5.2 storage any more.
// Thus, removing corresponding code.
void SwGrfNode::_GetStreamStorageNames( String& rStrmName,
                                        String& rStorName ) const
{
    rStorName.Erase();
    rStrmName.Erase();

    String aUserData( aGrfObj.GetUserData() );
    if( !aUserData.Len() )
        return;

    String aProt( RTL_CONSTASCII_STRINGPARAM( "vnd.sun.star.Package:" ) );
    if( 0 == aUserData.CompareTo( aProt, aProt.Len() ) )
    {
        // 6.0 (XML) Package
        xub_StrLen nPos = aUserData.Search( '/' );
        if( STRING_NOTFOUND == nPos )
        {
            rStrmName = aUserData.Copy( aProt.Len() );
        }
        else
        {
            xub_StrLen nPathStart = aProt.Len();
            if( 0 == aUserData.CompareToAscii( "./", 2 ) )
                nPathStart += 2;
            rStorName = aUserData.Copy( nPathStart, nPos-nPathStart );
            rStrmName = aUserData.Copy( nPos+1 );
        }
    }
    else
    {
        ASSERT( false,
                "<SwGrfNode::_GetStreamStorageNames(..)> - unknown graphic URL type. Code for handling 3.1 - 5.2 storages has been deleted by issue i53025." );
    }
    ASSERT( STRING_NOTFOUND == rStrmName.Search( '/' ),
            "invalid graphic stream name" );
}
// <--

SwCntntNode* SwGrfNode::MakeCopy( SwDoc* pDoc, const SwNodeIndex& rIdx ) const
{
    // kopiere die Formate in das andere Dokument:
    SwGrfFmtColl* pColl = pDoc->CopyGrfColl( *GetGrfColl() );

    Graphic aTmpGrf;
    SwBaseLink* pLink = (SwBaseLink*)(::sfx2::SvBaseLink*) refLink;
    if( !pLink && HasStreamName() )
    {
        // --> OD 2005-05-04 #i48434# - usage of new method <_GetStreamForEmbedGrf(..)>
        try
        {
            // --> OD, MAV 2005-08-17 #i53025# - needed correction of new
            // method <_GetStreamForEmbedGrf(..)>
//            bool bGraphic(false);
//            SvStream* pStrm = _GetStreamForEmbedGrf( bGraphic );
            String aStrmName, aPicStgName;
            _GetStreamStorageNames( aStrmName, aPicStgName );
            uno::Reference < embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
            SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
            if ( pStrm )
            {
                GetGrfFilter()->ImportGraphic( aTmpGrf, String(), *pStrm );
                delete pStrm;
            }
            // <--
        }
        catch ( uno::Exception& )
        {
            // --> OD 2005-04-25 #i48434#
            ASSERT( false, "<SwGrfNode::MakeCopy(..)> - unhandled exception!" );
            // <--
        }
        // <--
    }
    else
    {
        if( aGrfObj.IsSwappedOut() )
            const_cast<SwGrfNode*>(this)->SwapIn();
        aTmpGrf = aGrfObj.GetGraphic();
    }

    const sfx2::SvLinkManager& rMgr = getIDocumentLinksAdministration()->GetLinkManager();
    String sFile, sFilter;
    if( IsLinkedFile() )
        rMgr.GetDisplayNames( refLink, 0, &sFile, 0, &sFilter );
    else if( IsLinkedDDE() )
    {
        String sTmp1, sTmp2;
        rMgr.GetDisplayNames( refLink, &sTmp1, &sTmp2, &sFilter );
        sfx2::MakeLnkName( sFile, &sTmp1, sTmp2, sFilter );
        sFilter.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "DDE" ));
    }

    SwGrfNode* pGrfNd = pDoc->GetNodes().MakeGrfNode( rIdx, sFile, sFilter,
                                                    &aTmpGrf, pColl,
                                            (SwAttrSet*)GetpSwAttrSet() );
    pGrfNd->SetAlternateText( GetAlternateText() );
    pGrfNd->SetContour( HasContour(), HasAutomaticContour() );
    return pGrfNd;
}

IMPL_LINK( SwGrfNode, SwapGraphic, GraphicObject*, pGrfObj )
{
    SvStream* pRet;

    // #101174#: Keep graphic while in swap in. That's at least important
    // when breaking links, because in this situation a reschedule call and
    // a DataChanged call lead to a paint of the graphic.
    if( pGrfObj->IsInSwapOut() && (IsSelected() || bInSwapIn) )
        pRet = GRFMGR_AUTOSWAPSTREAM_NONE;
    else if( refLink.Is() )
    {
        if( pGrfObj->IsInSwapIn() )
        {
            // then make it by your self
            if( !bInSwapIn )
            {
                BOOL bIsModifyLocked = IsModifyLocked();
                LockModify();
                SwapIn( FALSE );
                if( !bIsModifyLocked )
                    UnlockModify();
            }
            pRet = GRFMGR_AUTOSWAPSTREAM_NONE;
        }
        else
            pRet = GRFMGR_AUTOSWAPSTREAM_LINK;
    }
    else
    {
        pRet = GRFMGR_AUTOSWAPSTREAM_TEMP;

        if( HasStreamName() )
        {
            // --> OD 2005-05-04 #i48434# - usage of new method <_GetStreamForEmbedGrf(..)>
            try
            {
                // --> OD, MAV 2005-08-17 #i53025# - needed correction of new
                // method <_GetStreamForEmbedGrf(..)>
//                bool bGraphic(false);
//                SvStream* pStrm = _GetStreamForEmbedGrf( bGraphic );
                String aStrmName, aPicStgName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference < embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
                SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
                if ( pStrm )
                {
                    if( pGrfObj->IsInSwapOut() )
                    {
                        pRet = GRFMGR_AUTOSWAPSTREAM_LINK;
                    }
                    else
                    {
                        ImportGraphic( *pStrm );
                        pRet = GRFMGR_AUTOSWAPSTREAM_LOADED;
                    }
                    delete pStrm;
                }
                // <--
            }
            catch ( uno::Exception& )
            {
                // --> OD 2005-04-25 #i48434#
                ASSERT( false, "<SwapGraphic> - unhandled exception!" );
                // <--
            }
            // <--
        }
    }

    return (long)pRet;
}


// alle QuickDraw-Bitmaps eines speziellen Docs loeschen
void DelAllGrfCacheEntries( SwDoc* pDoc )
{
    if( pDoc )
    {
        // alle Graphic-Links mit dem Namen aus dem Cache loeschen
        const SvxLinkManager& rLnkMgr = pDoc->GetLinkManager();
        const ::sfx2::SvBaseLinks& rLnks = rLnkMgr.GetLinks();
        SwGrfNode* pGrfNd;
        String sFileNm;
        for( USHORT n = rLnks.Count(); n; )
        {
            ::sfx2::SvBaseLink* pLnk = &(*rLnks[ --n ]);
            if( pLnk && OBJECT_CLIENT_GRF == pLnk->GetObjType() &&
                rLnkMgr.GetDisplayNames( pLnk, 0, &sFileNm ) &&
                pLnk->ISA( SwBaseLink ) && 0 != ( pGrfNd =
                ((SwBaseLink*)pLnk)->GetCntntNode()->GetGrfNode()) )
            {
                pGrfNd->GetGrfObj().ReleaseFromCache();
            }
        }
    }
}

// returns the with our graphic attributes filled Graphic-Attr-Structure
GraphicAttr& SwGrfNode::GetGraphicAttr( GraphicAttr& rGA,
                                        const SwFrm* pFrm ) const
{
    const SwAttrSet& rSet = GetSwAttrSet();

    rGA.SetDrawMode( (GraphicDrawMode)rSet.GetDrawModeGrf().GetValue() );

    const SwMirrorGrf & rMirror = rSet.GetMirrorGrf();
    ULONG nMirror = BMP_MIRROR_NONE;
    if( rMirror.IsGrfToggle() && pFrm && !pFrm->FindPageFrm()->OnRightPage() )
    {
        switch( rMirror.GetValue() )
        {
        case RES_MIRROR_GRAPH_DONT:     nMirror = BMP_MIRROR_HORZ; break;
        case RES_MIRROR_GRAPH_VERT:     nMirror = BMP_MIRROR_NONE; break;
        case RES_MIRROR_GRAPH_HOR:  nMirror = BMP_MIRROR_HORZ|BMP_MIRROR_VERT;
                                    break;
        default: 					nMirror = BMP_MIRROR_VERT; break;
        }
    }
    else
        switch( rMirror.GetValue() )
        {
        case RES_MIRROR_GRAPH_BOTH:     nMirror = BMP_MIRROR_HORZ|BMP_MIRROR_VERT;
                                    break;
        case RES_MIRROR_GRAPH_VERT: nMirror = BMP_MIRROR_HORZ; break;
        case RES_MIRROR_GRAPH_HOR:  nMirror = BMP_MIRROR_VERT; break;
        }

    rGA.SetMirrorFlags( nMirror );

    const SwCropGrf& rCrop = rSet.GetCropGrf();
    rGA.SetCrop( TWIP_TO_MM100( rCrop.GetLeft() ),
                 TWIP_TO_MM100( rCrop.GetTop() ),
                 TWIP_TO_MM100( rCrop.GetRight() ),
                 TWIP_TO_MM100( rCrop.GetBottom() ));

    const SwRotationGrf& rRotation = rSet.GetRotationGrf();
    rGA.SetRotation( rRotation.GetValue() );

    rGA.SetLuminance( rSet.GetLuminanceGrf().GetValue() );
    rGA.SetContrast( rSet.GetContrastGrf().GetValue() );
    rGA.SetChannelR( rSet.GetChannelRGrf().GetValue() );
    rGA.SetChannelG( rSet.GetChannelGGrf().GetValue() );
    rGA.SetChannelB( rSet.GetChannelBGrf().GetValue() );
    rGA.SetGamma( rSet.GetGammaGrf().GetValue() );
    rGA.SetInvert( rSet.GetInvertGrf().GetValue() );

    const sal_uInt16 nTrans = rSet.GetTransparencyGrf().GetValue();
    rGA.SetTransparency( (BYTE) FRound(
                                Min( nTrans, (USHORT) 100 )  * 2.55 ) );

    return rGA;
}

BOOL SwGrfNode::IsTransparent() const
{
    BOOL bRet = aGrfObj.IsTransparent();
    if( !bRet )	// ask the attribut
        bRet = 0 != GetSwAttrSet().GetTransparencyGrf().GetValue();

    // --> OD 2007-12-13 #i80822#
    // Fix is no longer needed and causes wrong results for non-transparent graphics
//    /// OD 17.09.2002 #102099# - if return value is still FALSE and
//    ///     graphic is swapped out, assume that graphic is transparent.
//    ///     Thus, for safety reasons, paint errors are avoided, because the
//    ///     background is painted not only by the graphic node.
//    if ( !bRet && aGrfObj.IsSwappedOut() )
//    {
//        bRet = true;
//    }
    // <--

    return bRet;
}


BOOL SwGrfNode::IsSelected() const
{
    BOOL bRet = FALSE;
    const SwEditShell* pESh = GetDoc()->GetEditShell();
    if( pESh )
    {
        const SwNode* pN = this;
        const ViewShell* pV = pESh;
        do {
            if( pV->ISA( SwEditShell ) && pN == &((SwCrsrShell*)pV)
                                ->GetCrsr()->GetPoint()->nNode.GetNode() )
            {
                bRet = TRUE;
                break;
            }
        }
        while( pESh != ( pV = (ViewShell*)pV->GetNext() ));
    }
    return bRet;
}

// --> OD 2006-12-22 #i73788#
boost::weak_ptr< SwAsyncRetrieveInputStreamThreadConsumer > SwGrfNode::GetThreadConsumer()
{
    return mpThreadConsumer;
}

void SwGrfNode::TriggerAsyncRetrieveInputStream()
{
    if ( !IsLinkedFile() )
    {
        ASSERT( false,
                "<SwGrfNode::TriggerAsyncLoad()> - Method is misused. Method call is only valid for graphic nodes, which refer a linked graphic file" );
        return;
    }

    if ( mpThreadConsumer.get() == 0 )
    {
        mpThreadConsumer.reset( new SwAsyncRetrieveInputStreamThreadConsumer( *this ) );

        String sGrfNm;
        refLink->GetLinkManager()->GetDisplayNames( refLink, 0, &sGrfNm, 0, 0 );

        mpThreadConsumer->CreateThread( sGrfNm );
    }
}

bool SwGrfNode::IsLinkedInputStreamReady() const
{
    return mbLinkedInputStreamReady;
}

void SwGrfNode::ApplyInputStream(
    com::sun::star::uno::Reference<com::sun::star::io::XInputStream> xInputStream,
    const sal_Bool bIsStreamReadOnly )
{
    if ( IsLinkedFile() )
    {
        if ( xInputStream.is() )
        {
            mxInputStream = xInputStream;
            mbIsStreamReadOnly = bIsStreamReadOnly;
            mbLinkedInputStreamReady = true;
            SwMsgPoolItem aMsgHint( RES_LINKED_GRAPHIC_STREAM_ARRIVED );
            Modify( &aMsgHint, &aMsgHint );
        }
    }
}

void SwGrfNode::UpdateLinkWithInputStream()
{
    if ( IsLinkedFile() )
    {
        GetLink()->setStreamToLoadFrom( mxInputStream, mbIsStreamReadOnly );
        GetLink()->Update();
        SwMsgPoolItem aMsgHint( RES_GRAPHIC_ARRIVED );
        Modify( &aMsgHint, &aMsgHint );

        mbLinkedInputStreamReady = false;
        mpThreadConsumer.reset();
    }
}
// <--
