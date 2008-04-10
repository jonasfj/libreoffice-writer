/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: uivwimp.cxx,v $
 * $Revision: 1.25 $
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


#include <cmdid.h>
#include "globals.hrc"

#include <tools/shl.hxx>
#include <com/sun/star/scanner/XScannerManager.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboard.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/msgbox.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>

#include <sfx2/docinsert.hxx>
#include <sfx2/request.hxx>
#include <uivwimp.hxx>
#include <wview.hxx>
#include <unotxvw.hxx>
#include <unodispatch.hxx>
#include <swmodule.hxx>
#include <swdtflvr.hxx>
#include <edtwin.hxx>
#include <mmconfigitem.hxx>

#include <view.hrc>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::scanner;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::datatransfer::clipboard;

/* -----------------02.06.98 15:31-------------------
 *
 * --------------------------------------------------*/
SwView_Impl::SwView_Impl(SwView* pShell) :
        pxXTextView(new uno::Reference<view::XSelectionSupplier>),
        pView(pShell),
        eShellMode(SHELL_MODE_TEXT),
        pConfigItem(0),
        nMailMergeRestartPage(0),
        bMailMergeSourceView(sal_True),
        m_pDocInserter(NULL),
        m_pRequest(NULL),
        m_bSelectObject(false),
        m_bEditingPositionSet(false)
{
    *pxXTextView = new SwXTextView(pView);
    xDisProvInterceptor = new SwXDispatchProviderInterceptor(*pView);
}

/*-----------------13.12.97 09:51-------------------

--------------------------------------------------*/
SwView_Impl::~SwView_Impl()
{
    Reference<XUnoTunnel> xDispTunnel(xDisProvInterceptor, UNO_QUERY);
    SwXDispatchProviderInterceptor* pInterceptor = 0;
    if(xDispTunnel.is() &&
        0 != (pInterceptor = reinterpret_cast< SwXDispatchProviderInterceptor * >(
                    sal::static_int_cast< sal_IntPtr >(
                    xDispTunnel->getSomething(SwXDispatchProviderInterceptor::getUnoTunnelId())))))
    {
        pInterceptor->Invalidate();
    }
    view::XSelectionSupplier* pTextView = pxXTextView->get();
    ((SwXTextView*)pTextView)->Invalidate();
    delete pxXTextView;
    if( xScanEvtLstnr.is() )
           pScanEvtLstnr->ViewDestroyed();
    if( xClipEvtLstnr.is() )
    {
        pClipEvtLstnr->AddRemoveListener( FALSE );
        pClipEvtLstnr->ViewDestroyed();
    }
    delete pConfigItem;

    delete m_pDocInserter;
    delete m_pRequest;
}

/*-----------------13.12.97 09:54-------------------

--------------------------------------------------*/
void SwView_Impl::SetShellMode(ShellModes eSet)
{
    eShellMode = eSet;
}
/*-----------------13.12.97 09:59-------------------

--------------------------------------------------*/
view::XSelectionSupplier*	SwView_Impl::GetUNOObject()
{
    return pxXTextView->get();
}
/* -----------------02.06.98 15:29-------------------
 *
 * --------------------------------------------------*/
SwXTextView*	SwView_Impl::GetUNOObject_Impl()
{
        view::XSelectionSupplier* pTextView = pxXTextView->get();
        return ((SwXTextView*)pTextView);
}
/* -----------------------------29.05.00 09:04--------------------------------

 ---------------------------------------------------------------------------*/
void SwView_Impl::ExecuteScan( SfxRequest& rReq )
{
    USHORT nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case SID_TWAIN_SELECT:
        {
            BOOL bDone = FALSE;
            Reference< XScannerManager > xScanMgr = SW_MOD()->GetScannerManager();

            if( xScanMgr.is() )
            {
                try
                {
                    const Sequence< ScannerContext >
                        aContexts( xScanMgr->getAvailableScanners() );

                    if( aContexts.getLength() )
                    {
                        ScannerContext aContext( aContexts.getConstArray()[ 0 ] );
                        bDone = xScanMgr->configureScanner( aContext );
                    }
                }
                catch(...)
                {
                }

            }
            if( bDone )
                rReq.Done();
            else
            {
                rReq.Ignore();
// KA 04.07.2002
//              InfoBox( 0, SW_RES(MSG_SCAN_NOSOURCE) ).Execute();
            }
        }
        break;

        case SID_TWAIN_TRANSFER:
        {
            BOOL bDone = FALSE;

            Reference< XScannerManager > xScanMgr = SW_MOD()->GetScannerManager();
            if( xScanMgr.is() )
            {
                SwScannerEventListener& rListener = GetScannerEventListener();
                try
                {
                    const Sequence< scanner::ScannerContext >aContexts( xScanMgr->getAvailableScanners() );
                    if( aContexts.getLength() )
                    {
                        Reference< XEventListener > xLstner = &rListener;
                        xScanMgr->startScan( aContexts.getConstArray()[ 0 ], xLstner );
                        bDone = TRUE;
                    }
                }
                catch(...)
                {
                }
            }

            if( !bDone )
            {
                InfoBox( 0, SW_RES(MSG_SCAN_NOSOURCE) ).Execute();
                rReq.Ignore();
            }
            else
            {
                rReq.Done();
                SfxBindings& rBind = pView->GetViewFrame()->GetBindings();
                rBind.Invalidate( SID_TWAIN_SELECT );
                rBind.Invalidate( SID_TWAIN_TRANSFER );
            }
        }
        break;
    }
}

/* -----------------------------29.05.00 08:26--------------------------------

 ---------------------------------------------------------------------------*/
SwScannerEventListener& SwView_Impl::GetScannerEventListener()
{
    if(!xScanEvtLstnr.is())
        xScanEvtLstnr = pScanEvtLstnr = new SwScannerEventListener(*pView);
    return *pScanEvtLstnr;
}


void SwView_Impl::AddClipboardListener()
{
    if(!xClipEvtLstnr.is())
    {
        xClipEvtLstnr = pClipEvtLstnr = new SwClipboardChangeListener( *pView );
        pClipEvtLstnr->AddRemoveListener( TRUE );
    }
}
/* -----------------3/31/2003 11:42AM----------------

 --------------------------------------------------*/
void SwView_Impl::Invalidate()
{
    GetUNOObject_Impl()->Invalidate();
    Reference< XUnoTunnel > xTunnel(xTransferable.get(), UNO_QUERY);
    if(xTunnel.is())

    {
        SwTransferable* pTransferable = reinterpret_cast< SwTransferable * >(
                sal::static_int_cast< sal_IntPtr >(
                xTunnel->getSomething(SwTransferable::getUnoTunnelId())));
        if(pTransferable)
            pTransferable->Invalidate();
    }
}
/* -----------------3/31/2003 12:40PM----------------

 --------------------------------------------------*/
void SwView_Impl::AddTransferable(SwTransferable& rTransferable)
{
    //prevent removing of the non-referenced SwTransferable
    rTransferable.m_refCount++;;
    {
        xTransferable = Reference<XUnoTunnel> (&rTransferable);
    }
    rTransferable.m_refCount--;
}

void SwView_Impl::StartDocumentInserter( const String& rFactory, const Link& rEndDialogHdl )
{
    delete m_pDocInserter;
    m_pDocInserter = new ::sfx2::DocumentInserter( 0, rFactory );
    m_pDocInserter->StartExecuteModal( rEndDialogHdl );
}

SfxMedium* SwView_Impl::CreateMedium()
{
    return m_pDocInserter->CreateMedium();
}

void SwView_Impl::InitRequest( const SfxRequest& rRequest )
{
    delete m_pRequest;
    m_pRequest = new SfxRequest( rRequest );
}

// ------------------------- SwScannerEventListener ---------------------

SwScannerEventListener::~SwScannerEventListener()
{
}

void SAL_CALL SwScannerEventListener::disposing( const EventObject& rEventObject) throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
#if defined WIN || defined WNT || defined UNX
    if( pView )
        pView->ScannerEventHdl( rEventObject );
#endif
}

// ------------------------- SwClipboardChangeListener ---------------------

SwClipboardChangeListener::~SwClipboardChangeListener()
{
}

void SAL_CALL SwClipboardChangeListener::disposing( const EventObject& /*rEventObject*/ )
    throw ( RuntimeException )
{
}

void SAL_CALL SwClipboardChangeListener::changedContents( const CLIP_NMSPC::ClipboardEvent& rEventObject )
    throw ( RuntimeException )

{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if( pView )
    {
        {
            TransferableDataHelper aDataHelper( rEventObject.Contents );
            SwWrtShell& rSh = pView->GetWrtShell();

            pView->nLastPasteDestination = SwTransferable::GetSotDestination( rSh );
            pView->bPasteState = aDataHelper.GetXTransferable().is() &&
                            SwTransferable::IsPaste( rSh, aDataHelper );

            pView->bPasteSpecialState = aDataHelper.GetXTransferable().is() &&
                        SwTransferable::IsPasteSpecial( rSh, aDataHelper );
        }

        SfxBindings& rBind = pView->GetViewFrame()->GetBindings();
        rBind.Invalidate( SID_PASTE );
        rBind.Invalidate( FN_PASTESPECIAL );
        rBind.Invalidate( SID_CLIPBOARD_FORMAT_ITEMS );
    }
}

void SwClipboardChangeListener::AddRemoveListener( BOOL bAdd )
{
    try
    {
        do {

#ifdef _DONT_WORD_FOR_WEBTOP_
JP 4.7.2001: change for WebTop - get Clipboard from the Window.
            Reference< XMultiServiceFactory > xFact(
                                ::comphelper::getProcessServiceFactory() );
            if( !xFact.is() )
                break;
            Reference< XClipboard > xClipboard( xFact->createInstance(
                ::rtl::OUString::createFromAscii(
                    "com.sun.star.datatransfer.clipboard.SystemClipboard" )),
                UNO_QUERY );
#else
            Reference< XClipboard > xClipboard(
                    pView->GetEditWin().GetClipboard() );
#endif
            if( !xClipboard.is() )
                break;

            Reference< XClipboardNotifier > xClpbrdNtfr( xClipboard, UNO_QUERY );
            if( xClpbrdNtfr.is() )
            {
                Reference< XClipboardListener >	xClipEvtLstnr( this );
                if( bAdd )
                    xClpbrdNtfr->addClipboardListener( xClipEvtLstnr );
                else
                    xClpbrdNtfr->removeClipboardListener( xClipEvtLstnr );
            }
        }  while ( FALSE );
    }
    catch( const uno::Exception& )
    {
    }
}


