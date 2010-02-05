/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: apphdl.cxx,v $
 * $Revision: 1.68 $
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
#include <hintids.hxx>
#include <tools/urlobj.hxx>

#include <tools/debug.hxx>
#include <tools/link.hxx>

#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#include <svl/urihelper.hxx>
#include <unotools/undoopt.hxx>
#include <unotools/pathoptions.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/event.hxx>
#include <sfx2/objitem.hxx>
#include <svx/dataaccessdescriptor.hxx>
#include <svl/srchitem.hxx>
#include <svtools/colorcfg.hxx>
#include <svl/eitem.hxx>
#include <svl/whiter.hxx>
#include <svl/isethint.hxx>
#include <svx/hyprlink.hxx>
#include <sfx2/request.hxx>
#include <sfx2/fcontnr.hxx>
#include <svl/stritem.hxx>
#include <svl/ctloptions.hxx>
#include <unotools/useroptions.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/wrkwin.hxx>
#include <svx/insctrl.hxx>
#include <svx/selctrl.hxx>
#include <com/sun/star/document/UpdateDocMode.hpp>
#include <sfx2/docfile.hxx>
#include <svx/xmlsecctrl.hxx>
#include <navicfg.hxx>

#include <sfx2/objface.hxx>
#include <sfx2/app.hxx>

#include <view.hxx>
#include <pview.hxx>
#include <srcview.hxx>
#include <wrtsh.hxx>
#include <docsh.hxx>
#ifndef _CMDID_H
#include <cmdid.h>		  	// Funktion-Ids
#endif
#include <initui.hxx>
#include <uitool.hxx>
#include <swmodule.hxx>
#include <wdocsh.hxx>
#include <wview.hxx>
#include <usrpref.hxx>
#include <gloslst.hxx>      // SwGlossaryList
#include <glosdoc.hxx>      // SwGlossaryList
#include <doc.hxx>
#include <cfgitems.hxx>
#include <prtopt.hxx>
#include <modcfg.hxx>
#include <globals.h>		// globale Konstanten z.B.
#ifndef _APP_HRC
#include <app.hrc>
#endif
#include <fontcfg.hxx>
#include <barcfg.hxx>
#include <uinums.hxx>
#include <dbconfig.hxx>
#include <mmconfigitem.hxx>
#include <mailmergechildwindow.hxx>
#include <linguistic/lngprops.hxx>
#include <editeng/unolingu.hxx>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XPropertyStateChangeListener.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/sdbc/XDataSource.hpp>
#include <swabstdlg.hxx>


#include <vcl/status.hxx>

#include "salhelper/simplereferenceobject.hxx"
#include "rtl/ref.hxx"

#include <unomid.h>

using namespace ::com::sun::star;

/*--------------------------------------------------------------------
    Beschreibung: Slotmaps fuer Methoden der Applikation
 --------------------------------------------------------------------*/


// hier werden die SlotID's included
// siehe Idl-File
//
#define SwModule
#define ViewSettings
#define WebViewSettings
#define PrintSettings
#define _ExecAddress ExecOther
#define _StateAddress StateOther
#include <sfx2/msg.hxx>
#include <svx/svxslots.hxx>
#include "swslots.hxx"
#include <cfgid.h>

#include <shells.hrc>

SFX_IMPL_INTERFACE( SwModule, SfxModule, SW_RES(RID_SW_NAME) )
{
    SFX_CHILDWINDOW_REGISTRATION(SvxHyperlinkDlgWrapper::GetChildWindowId());
    SFX_STATUSBAR_REGISTRATION(SW_RES(CFG_STATUSBAR));
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_APPLICATION |
            SFX_VISIBILITY_DESKTOP | SFX_VISIBILITY_STANDARD | SFX_VISIBILITY_CLIENT | SFX_VISIBILITY_VIEWER,
            SW_RES(RID_MODULE_TOOLBOX) );
}


/*--------------------------------------------------------------------
    Beschreibung: Andere States
 --------------------------------------------------------------------*/


void SwModule::StateOther(SfxItemSet &rSet)
{
    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();

    SwView* pActView = ::GetActiveView();
    sal_Bool bWebView = 0 != PTR_CAST(SwWebView, pActView);

    while(nWhich)
    {
        switch(nWhich)
        {
            case FN_BUSINESS_CARD:
            case FN_LABEL:
            case FN_ENVELOP:
            {
                sal_Bool bDisable = sal_False;
                SfxViewShell* pCurrView = SfxViewShell::Current();
                if( !pCurrView || (pCurrView && !pCurrView->ISA(SwView)) )
                    bDisable = sal_True;
                SwDocShell *pDocSh = (SwDocShell*) SfxObjectShell::Current();
                if ( bDisable ||
                    (pDocSh &&    (pDocSh->IsReadOnly() ||
                                  pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED)) )
                    rSet.DisableItem( nWhich );

            }
            break;
            case FN_XFORMS_INIT:
                // slot is always active!
                break;
            case FN_EDIT_FORMULA:
                {
                    SwWrtShell* pSh = 0;
                    int nSelection = 0;
                    if( pActView )
                        pSh = &pActView->GetWrtShell();
                    if( pSh )
                        nSelection = pSh->GetSelectionType();

                    if( (pSh && pSh->HasSelection()) ||
                        !(nSelection & (nsSelectionType::SEL_TXT | nsSelectionType::SEL_TBL)))
                        rSet.DisableItem(nWhich);
                }
            break;
            case SID_ATTR_METRIC:
                rSet.Put( SfxUInt16Item( SID_ATTR_METRIC, static_cast< UINT16 >(::GetDfltMetric(bWebView))));
            break;
            case FN_SET_MODOPT_TBLNUMFMT:
                rSet.Put( SfxBoolItem( nWhich, pModuleConfig->
                                            IsInsTblFormatNum( bWebView )));
            break;
            default:
                DBG_ERROR("::StateOther: default");
        }
        nWhich = aIter.NextWhich();
    }
}

/*-- 06.04.2004 15:21:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwView* lcl_LoadDoc(SwView* pView, const String& rURL)
{
    SwView* pNewView = 0;
    if(rURL.Len())
    {
        SfxStringItem aURL(SID_FILE_NAME, rURL);
        SfxStringItem aTargetFrameName( SID_TARGETNAME, String::CreateFromAscii("_blank") );
        SfxBoolItem aHidden( SID_HIDDEN, TRUE );
        SfxStringItem aReferer(SID_REFERER, pView->GetDocShell()->GetTitle());
        SfxObjectItem* pItem = (SfxObjectItem*)pView->GetViewFrame()->GetDispatcher()->
                Execute(SID_OPENDOC, SFX_CALLMODE_SYNCHRON,
                            &aURL, &aHidden, &aReferer, &aTargetFrameName, 0L);
        SfxShell* pShell = pItem ? pItem->GetShell() : 0;

        if(pShell)
        {
            SfxViewShell* pViewShell = pShell->GetViewShell();
            if(pViewShell)
            {
                if( pViewShell->ISA(SwView) )
                {
                    pNewView = PTR_CAST(SwView,pViewShell);
                    pNewView->GetViewFrame()->GetFrame()->Appear();
                }
                else
                {
                    pViewShell->GetViewFrame()->DoClose();
                }
            }
        }
    }
    else
    {
        SfxStringItem aFactory(SID_NEWDOCDIRECT, SwDocShell::Factory().GetFilterContainer()->GetName());
        const SfxFrameItem* pItem = (SfxFrameItem*)
                            pView->GetViewFrame()->GetDispatcher()->Execute(SID_NEWDOCDIRECT,
                                SFX_CALLMODE_SYNCHRON, &aFactory, 0L);
        SfxFrame* pFrm = pItem ? pItem->GetFrame() : 0;
        SfxViewFrame* pFrame = pFrm ? pFrm->GetCurrentViewFrame() : 0;
        pNewView = pFrame ? PTR_CAST(SwView, pFrame->GetViewShell()) : 0;
    }

    return pNewView;
}
/*--------------------------------------------------------------------
    Beschreibung:	Felddialog starten
 --------------------------------------------------------------------*/

void NewXForms( SfxRequest& rReq ); // implementation: below

namespace
{

class SwMailMergeWizardExecutor : public salhelper::SimpleReferenceObject
{
    SwView*                  m_pView;    	// never owner
    SwView*                  m_pView2Close; // never owner
    SwMailMergeConfigItem*   m_pMMConfig; 	// sometimes owner
    AbstractMailMergeWizard* m_pWizard;   	// always owner

    DECL_LINK( EndDialogHdl, AbstractMailMergeWizard* );
    DECL_LINK( DestroyDialogHdl, AbstractMailMergeWizard* );
    DECL_LINK( DestroyWizardHdl, AbstractMailMergeWizard* );
    DECL_LINK( CancelHdl, AbstractMailMergeWizard* );
    DECL_LINK( CloseFrameHdl, AbstractMailMergeWizard* );

    void ExecutionFinished( bool bDeleteConfigItem );
    void ExecuteWizard();

public:
    SwMailMergeWizardExecutor();
    ~SwMailMergeWizardExecutor();

    void ExecuteMailMergeWizard( const SfxItemSet * pArgs );
};

SwMailMergeWizardExecutor::SwMailMergeWizardExecutor()
    : m_pView( 0 ),
      m_pView2Close( NULL ),
      m_pMMConfig( 0 ),
      m_pWizard( 0 )
{
}

SwMailMergeWizardExecutor::~SwMailMergeWizardExecutor()
{
    DBG_ASSERT( m_pWizard == 0, "SwMailMergeWizardExecutor: m_pWizard must be Null!" );
    DBG_ASSERT( m_pMMConfig == 0, "SwMailMergeWizardExecutor: m_pMMConfig must be Null!" );
}

void SwMailMergeWizardExecutor::ExecuteMailMergeWizard( const SfxItemSet * pArgs )
{
    if ( m_pView )
    {
        DBG_ERROR( "SwMailMergeWizardExecutor::ExecuteMailMergeWizard: Already executing the wizard!" );
        return;
    }

    m_pView = ::GetActiveView(); // not owner!
    DBG_ASSERT(m_pView, "no current view?");
    if(m_pView)
    {
        // keep self alive until done.
        acquire();

        // if called from the child window - get the config item and close the ChildWindow, then restore
        // the wizard
        SwMailMergeChildWindow* pChildWin =
            static_cast<SwMailMergeChildWindow*>(m_pView->GetViewFrame()->GetChildWindow(FN_MAILMERGE_CHILDWINDOW));
        bool bRestoreWizard = false;
        sal_uInt16 nRestartPage = 0;
        if(pChildWin && pChildWin->IsVisible())
        {
            m_pMMConfig = m_pView->GetMailMergeConfigItem();
            nRestartPage = m_pView->GetMailMergeRestartPage();
            if(m_pView->IsMailMergeSourceView())
                m_pMMConfig->SetSourceView( m_pView );
            m_pView->SetMailMergeConfigItem(0, 0, sal_True);
            SfxViewFrame* pViewFrame = m_pView->GetViewFrame();
            pViewFrame->ShowChildWindow(FN_MAILMERGE_CHILDWINDOW, FALSE);
            DBG_ASSERT(m_pMMConfig, "no MailMergeConfigItem available");
            bRestoreWizard = true;
        }
        // to make it bullet proof ;-)
        if(!m_pMMConfig)
        {
            m_pMMConfig = new SwMailMergeConfigItem;
            m_pMMConfig->SetSourceView(m_pView);

            //set the first used database as default source on the config item
            const SfxPoolItem* pItem = 0;
            if(pArgs && SFX_ITEM_SET == pArgs->GetItemState(
                   FN_PARAM_DATABASE_PROPERTIES, sal_False, &pItem))
            {
                //mailmerge has been called from the database beamer
                uno::Sequence< beans::PropertyValue> aDBValues;
                if(static_cast<const SfxUsrAnyItem*>(pItem)->GetValue() >>= aDBValues)
                {
                    SwDBData aDBData;
                    svx::ODataAccessDescriptor aDescriptor(aDBValues);
                    aDescriptor[svx::daDataSource]   >>= aDBData.sDataSource;
                    aDescriptor[svx::daCommand]      >>= aDBData.sCommand;
                    aDescriptor[svx::daCommandType]  >>= aDBData.nCommandType;

                    uno::Sequence< uno::Any >                   aSelection;
                    uno::Reference< sdbc::XConnection>          xConnection;
                    uno::Reference< sdbc::XDataSource>          xSource;
                    uno::Reference< sdbcx::XColumnsSupplier>    xColumnsSupplier;
                    if ( aDescriptor.has(svx::daSelection) )
                        aDescriptor[svx::daSelection] >>= aSelection;
                    if ( aDescriptor.has(svx::daConnection) )
                        aDescriptor[svx::daConnection] >>= xConnection;
                    uno::Reference<container::XChild> xChild(xConnection, uno::UNO_QUERY);
                    if(xChild.is())
                        xSource = uno::Reference<sdbc::XDataSource>(
                            xChild->getParent(), uno::UNO_QUERY);
                    m_pMMConfig->SetCurrentConnection(
                        xSource, SharedConnection( xConnection, SharedConnection::NoTakeOwnership ),
                        xColumnsSupplier, aDBData);
                }
            }
            else
            {
                SvStringsDtor aDBNameList(5, 1);
                SvStringsDtor aAllDBNames(5, 5);
                m_pView->GetWrtShell().GetAllUsedDB( aDBNameList, &aAllDBNames );
                if(aDBNameList.Count())
                {
                    String sDBName = *aDBNameList[0];
                    SwDBData aDBData;
                    aDBData.sDataSource = sDBName.GetToken(0, DB_DELIM);
                    aDBData.sCommand = sDBName.GetToken(1, DB_DELIM);
                    aDBData.nCommandType = sDBName.GetToken(2, DB_DELIM ).ToInt32();
                    //set the currently used database for the wizard
                    m_pMMConfig->SetCurrentDBData( aDBData );
                }
            }
        }

        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
        m_pWizard = pFact->CreateMailMergeWizard(*m_pView, *m_pMMConfig);

        if(bRestoreWizard)
        {
            m_pWizard->ShowPage( nRestartPage );
        }

        ExecuteWizard();
    }
}

void SwMailMergeWizardExecutor::ExecutionFinished( bool bDeleteConfigItem )
{
    m_pMMConfig->Commit();
    if ( bDeleteConfigItem ) // owner?
        delete m_pMMConfig;

    m_pMMConfig = 0;

    // release/destroy asynchronously
    Application::PostUserEvent( LINK( this, SwMailMergeWizardExecutor, DestroyDialogHdl ) );
}

void SwMailMergeWizardExecutor::ExecuteWizard()
{
    m_pWizard->StartExecuteModal(
        LINK( this, SwMailMergeWizardExecutor, EndDialogHdl ) );
}

#if OSL_DEBUG_LEVEL > 1
IMPL_LINK( SwMailMergeWizardExecutor, EndDialogHdl, AbstractMailMergeWizard*, pDialog )
#else
IMPL_LINK( SwMailMergeWizardExecutor, EndDialogHdl, AbstractMailMergeWizard*, EMPTYARG )
#endif
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT( pDialog == m_pWizard, "wrong dialog passed to EndDialogHdl!" );
    (void) pDialog;
#endif

    long nRet = m_pWizard->GetResult();
    sal_uInt16 nRestartPage = m_pWizard->GetRestartPage();

    switch ( nRet )
    {
    case RET_LOAD_DOC:
        {
            SwView* pNewView = lcl_LoadDoc(m_pView, m_pWizard->GetReloadDocument());

            // destroy wizard asynchronously
            Application::PostUserEvent(
                LINK( this, SwMailMergeWizardExecutor, DestroyWizardHdl ), m_pWizard );

            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            if(pNewView)
            {
                m_pView = pNewView;
                m_pMMConfig->DocumentReloaded();
                //new source view!
                m_pMMConfig->SetSourceView( m_pView );
                m_pWizard = pFact->CreateMailMergeWizard(*m_pView, *m_pMMConfig);
                m_pWizard->ShowPage( nRestartPage );
            }
            else
            {
                m_pWizard = pFact->CreateMailMergeWizard(*m_pView, *m_pMMConfig);
            }

            // execute the wizard again
            ExecuteWizard();
            break;
        }
    case RET_TARGET_CREATED:
        {
            SwView* pTargetView = m_pMMConfig->GetTargetView();
            uno::Reference< frame::XFrame > xFrame =
                m_pView->GetViewFrame()->GetFrame()->GetFrameInterface();
            xFrame->getContainerWindow()->setVisible(sal_False);
            DBG_ASSERT(pTargetView, "No target view has been created");
            if(pTargetView)
            {
                // destroy wizard asynchronously
                Application::PostUserEvent(
                    LINK( this, SwMailMergeWizardExecutor, DestroyWizardHdl ), m_pWizard );

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                m_pWizard = pFact->CreateMailMergeWizard(*pTargetView, *m_pMMConfig);
                m_pWizard->ShowPage( nRestartPage );

                // execute the wizard again
                ExecuteWizard();
            }
            else
            {
                //should not happen - just in case no target view has been created
                ExecutionFinished( true );
            }
            break;
        }
    case RET_EDIT_DOC:
    case RET_EDIT_RESULT_DOC:
        {
            //create a non-modal dialog that allows to return to the wizard
            //the ConfigItem ownership moves to this dialog
            bool bResult = nRet == RET_EDIT_RESULT_DOC && m_pMMConfig->GetTargetView();
            SwView* pTempView = bResult ? m_pMMConfig->GetTargetView() : m_pMMConfig->GetSourceView();
            pTempView->SetMailMergeConfigItem(m_pMMConfig, m_pWizard->GetRestartPage(), !bResult);
            SfxViewFrame* pViewFrame = pTempView->GetViewFrame();
            pViewFrame->GetDispatcher()->Execute(
                FN_MAILMERGE_CHILDWINDOW, SFX_CALLMODE_SYNCHRON);
            ExecutionFinished( false );
            break;
        }
    case RET_REMOVE_TARGET:
        {
            SwView* pTargetView = m_pMMConfig->GetTargetView();
            SwView* pSourceView = m_pMMConfig->GetSourceView();
            DBG_ASSERT(pTargetView && pSourceView, "source or target view not available" );
            if(pTargetView && pSourceView)
            {
                m_pView2Close = pTargetView;
                pTargetView->GetViewFrame()->GetTopViewFrame()->GetWindow().Hide();
                pSourceView->GetViewFrame()->GetFrame()->AppearWithUpdate();
                // the current view has be be set when the target is destroyed
                m_pView = pSourceView;
                m_pMMConfig->SetTargetView(0);

                // destroy wizard asynchronously
                Application::PostUserEvent(
                    LINK( this, SwMailMergeWizardExecutor, CloseFrameHdl ), m_pWizard );

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                m_pWizard = pFact->CreateMailMergeWizard(*pSourceView, *m_pMMConfig);
                m_pWizard->ShowPage( nRestartPage );

                // execute the wizard again
                ExecuteWizard();
            }
            else
            {
                //should not happen - just in case no target view has been created
                ExecutionFinished( true );
            }
            break;
        }
    case RET_CANCEL:
        {
            // close frame and destroy wizard asynchronously
            Application::PostUserEvent(
                LINK( this, SwMailMergeWizardExecutor, CancelHdl ), m_pWizard );
            break;
        }
    default: //finish
        {
            SwView* pSourceView = m_pMMConfig->GetSourceView();
            if(pSourceView)
            {
                SwDocShell* pDocShell = pSourceView->GetDocShell();
                if(pDocShell->HasName() && !pDocShell->IsModified())
                    m_pMMConfig->GetSourceView()->GetViewFrame()->DoClose();
                else
                    m_pMMConfig->GetSourceView()->GetViewFrame()->GetFrame()->Appear();
            }
            ExecutionFinished( true );
            break;
        }

    } // switch

    return 0L;
}

IMPL_LINK( SwMailMergeWizardExecutor, DestroyDialogHdl, AbstractMailMergeWizard*, EMPTYARG )
{
    delete m_pWizard;
    m_pWizard = 0;

    release();
    return 0L;
}

IMPL_LINK( SwMailMergeWizardExecutor, DestroyWizardHdl, AbstractMailMergeWizard*, pDialog )
{
    delete pDialog;
    return 0L;
}

IMPL_LINK( SwMailMergeWizardExecutor, CancelHdl, AbstractMailMergeWizard*, EMPTYARG )
{
    if(m_pMMConfig->GetTargetView())
    {
        m_pMMConfig->GetTargetView()->GetViewFrame()->DoClose();
        m_pMMConfig->SetTargetView(0);
    }
    if(m_pMMConfig->GetSourceView())
        m_pMMConfig->GetSourceView()->GetViewFrame()->GetFrame()->AppearWithUpdate();

    m_pMMConfig->Commit();
    delete m_pMMConfig;
    m_pMMConfig = 0;
    // m_pWizard already deleted by closing the target view
    m_pWizard = 0;
    release();

    return 0L;
}

IMPL_LINK( SwMailMergeWizardExecutor, CloseFrameHdl, AbstractMailMergeWizard*, EMPTYARG )
{
    if ( m_pView2Close )
    {
        m_pView2Close->GetViewFrame()->DoClose();
        m_pView2Close = NULL;
    }

    return 0L;
}

} // namespace

void SwModule::ExecOther(SfxRequest& rReq)
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;

    sal_uInt16 nWhich = rReq.GetSlot();
    switch (nWhich)
    {
        case FN_ENVELOP:
            InsertEnv( rReq );
            break;

        case FN_BUSINESS_CARD:
        case FN_LABEL:
            InsertLab(rReq, nWhich == FN_LABEL);
            break;

        case FN_XFORMS_INIT:
            NewXForms( rReq );
            break;

        case SID_ATTR_METRIC:
        if(pArgs && SFX_ITEM_SET == pArgs->GetItemState(nWhich, sal_False, &pItem))
        {
            FieldUnit eUnit = (FieldUnit)((const SfxUInt16Item*)pItem)->GetValue();
            switch( eUnit )
            {
                case FUNIT_MM:
                case FUNIT_CM:
                case FUNIT_INCH:
                case FUNIT_PICA:
                case FUNIT_POINT:
                {
                    SwView* pActView = ::GetActiveView();
                    sal_Bool bWebView = 0 != PTR_CAST(SwWebView, pActView);
                    ::SetDfltMetric(eUnit, bWebView);
                }
                break;
                default:;//prevent warning
            }
        }
        break;

        case FN_SET_MODOPT_TBLNUMFMT:
            {
                sal_Bool bWebView = 0 != PTR_CAST(SwWebView, ::GetActiveView() ),
                     bSet;

                if( pArgs && SFX_ITEM_SET == pArgs->GetItemState(
                        nWhich, sal_False, &pItem ))
                    bSet = ((SfxBoolItem*)pItem)->GetValue();
                else
                    bSet = !pModuleConfig->IsInsTblFormatNum( bWebView );

                pModuleConfig->SetInsTblFormatNum( bWebView, bSet );
            }
            break;
        case FN_MAILMERGE_WIZARD:
        {
            rtl::Reference< SwMailMergeWizardExecutor > xEx( new SwMailMergeWizardExecutor );
            xEx->ExecuteMailMergeWizard( pArgs );
        }
        break;
    }
}

/*--------------------------------------------------------------------
    Beschreibung: Notifies abfangen
 --------------------------------------------------------------------*/


    // Hint abfangen fuer DocInfo
void SwModule::Notify( SfxBroadcaster& /*rBC*/, const SfxHint& rHint )
{
    if( rHint.ISA( SfxEventHint ) )
    {
        SfxEventHint& rEvHint = (SfxEventHint&) rHint;
        SwDocShell* pDocSh = PTR_CAST( SwDocShell, rEvHint.GetObjShell() );
        if( pDocSh )
        {
            SwWrtShell* pWrtSh = pDocSh ? pDocSh->GetWrtShell() : 0;
            switch( rEvHint.GetEventId() )
            {
/*          MA 07. Mar. 96: UpdateInputFlds() nur noch bei Dokument neu.
                                (Und bei Einfuegen Textbaust.)
                case SFX_EVENT_OPENDOC:
                // dann am aktuellen Dokument die Input-Fedler updaten
                if( pWrtSh )
                    pWrtSh->UpdateInputFlds();
                break;
*/
            case SFX_EVENT_CREATEDOC:
                // alle FIX-Date/Time Felder auf akt. setzen
                if( pWrtSh )
                {
                    SFX_ITEMSET_ARG( pDocSh->GetMedium()->GetItemSet(), pUpdateDocItem, SfxUInt16Item, SID_UPDATEDOCMODE, sal_False);
                    sal_Bool bUpdateFields = sal_True;
                    if( pUpdateDocItem &&  pUpdateDocItem->GetValue() == document::UpdateDocMode::NO_UPDATE)
                        bUpdateFields = sal_False;
                    pWrtSh->SetFixFields();
                    if(bUpdateFields)
                    {
                        pWrtSh->UpdateInputFlds();

                        // Sind Datenbankfelder enthalten?
                        // Erstmal alle verwendeten Datenbanken holen
                        SwDoc *pDoc = pDocSh->GetDoc();
                        SvStringsDtor aDBNameList;
                        pDoc->GetAllUsedDB( aDBNameList );
                        sal_uInt16 nCount = aDBNameList.Count();
                        if (nCount)
                        {   // Datenbankbeamer oeffnen
                            ShowDBObj(pWrtSh->GetView(), pDoc->GetDBData());
                        }
                    }
                }
                break;
            }
        }
    }
    else if(rHint.ISA(SfxItemSetHint))
    {
        if(	SFX_ITEM_SET == ((SfxItemSetHint&)rHint).GetItemSet().GetItemState(SID_ATTR_PATHNAME))
        {
            ::GetGlossaries()->UpdateGlosPath( sal_False );
            SwGlossaryList* pList = ::GetGlossaryList();
            if(pList->IsActive())
                pList->Update();
        }
    }
    else if(rHint.ISA(SfxSimpleHint))
    {
        USHORT nHintId = ((SfxSimpleHint&)rHint).GetId();
        if(SFX_HINT_DEINITIALIZING == nHintId)
        {
            DELETEZ(pWebUsrPref);
            DELETEZ(pUsrPref)   ;
            DELETEZ(pModuleConfig);
            DELETEZ(pPrtOpt)      ;
            DELETEZ(pWebPrtOpt)   ;
            DELETEZ(pChapterNumRules);
            DELETEZ(pStdFontConfig)     ;
            DELETEZ(pNavigationConfig)  ;
            DELETEZ(pToolbarConfig)     ;
            DELETEZ(pWebToolbarConfig)  ;
            DELETEZ(pAuthorNames)       ;
            DELETEZ(pDBConfig);
            pColorConfig->RemoveListener(this);
            DELETEZ(pColorConfig);
            pAccessibilityOptions->RemoveListener(this);
            DELETEZ(pAccessibilityOptions);
            pCTLOptions->RemoveListener(this);
            DELETEZ(pCTLOptions);
            pUserOptions->RemoveListener(this);
            DELETEZ(pUserOptions);
            pUndoOptions->RemoveListener(this);
            DELETEZ(pUndoOptions);
        }
    }
}

void SwModule::ConfigurationChanged( utl::ConfigurationBroadcaster* pBrdCst, sal_uInt32 )
{
    if( pBrdCst == pUserOptions )
    {
        bAuthorInitialised = FALSE;
    }
    else if( pBrdCst == pUndoOptions )
    {
        const int nNew = GetUndoOptions().GetUndoCount();
        const int nOld = SwEditShell::GetUndoActionCount();
        if(!nNew || !nOld)
        {
            sal_Bool bUndo = nNew != 0;
            //ueber DocShells iterieren und Undo umschalten

            TypeId aType(TYPE(SwDocShell));
            SwDocShell* pDocShell = (SwDocShell*)SfxObjectShell::GetFirst(&aType);
            while( pDocShell )
            {
                pDocShell->GetDoc()->DoUndo( bUndo );
                pDocShell = (SwDocShell*)SfxObjectShell::GetNext(*pDocShell, &aType);
            }
        }
        SwEditShell::SetUndoActionCount( static_cast< USHORT >(nNew));
    }
    else if ( pBrdCst == pColorConfig || pBrdCst == pAccessibilityOptions )
    {
        sal_Bool bAccessibility = sal_False;
        if( pBrdCst == pColorConfig )
            SwViewOption::ApplyColorConfigValues(*pColorConfig);
        else
            bAccessibility = sal_True;

        //invalidate all edit windows
        const TypeId aSwViewTypeId = TYPE(SwView);
        const TypeId aSwPreViewTypeId = TYPE(SwPagePreView);
        const TypeId aSwSrcViewTypeId = TYPE(SwSrcView);
        SfxViewShell* pViewShell = SfxViewShell::GetFirst();
        while(pViewShell)
        {
            if(pViewShell->GetWindow())
            {
                if((pViewShell->IsA(aSwViewTypeId) ||
                    pViewShell->IsA(aSwPreViewTypeId) ||
                    pViewShell->IsA(aSwSrcViewTypeId)))
                {
                    if(bAccessibility)
                    {
                        if(pViewShell->IsA(aSwViewTypeId))
                            ((SwView*)pViewShell)->ApplyAccessiblityOptions(*pAccessibilityOptions);
                        else if(pViewShell->IsA(aSwPreViewTypeId))
                            ((SwPagePreView*)pViewShell)->ApplyAccessiblityOptions(*pAccessibilityOptions);
                    }
                    pViewShell->GetWindow()->Invalidate();
                }
            }
            pViewShell = SfxViewShell::GetNext( *pViewShell );
        }
    }
    else if( pBrdCst == pCTLOptions )
    {
        const SfxObjectShell* pObjSh = SfxObjectShell::GetFirst();
        while( pObjSh )
        {
            if( pObjSh->IsA(TYPE(SwDocShell)) )
            {
                const SwDoc* pDoc = ((SwDocShell*)pObjSh)->GetDoc();
                ViewShell* pVSh = 0;
                pDoc->GetEditShell( &pVSh );
                if ( pVSh )
                    pVSh->ChgNumberDigits();
            }
            pObjSh = SfxObjectShell::GetNext(*pObjSh);
        }
    }

}

/* -----------------------------20.02.01 12:43--------------------------------

 ---------------------------------------------------------------------------*/
SwDBConfig*	SwModule::GetDBConfig()
{
    if(!pDBConfig)
        pDBConfig = new SwDBConfig;
    return pDBConfig;
}
/* -----------------------------11.04.2002 15:27------------------------------

 ---------------------------------------------------------------------------*/
svtools::ColorConfig& SwModule::GetColorConfig()
{
    if(!pColorConfig)
    {
        pColorConfig = new svtools::ColorConfig;
        SwViewOption::ApplyColorConfigValues(*pColorConfig);
        pColorConfig->AddListener(this);
    }
    return *pColorConfig;
}
/* -----------------------------06.05.2002 09:42------------------------------

 ---------------------------------------------------------------------------*/
SvtAccessibilityOptions& SwModule::GetAccessibilityOptions()
{
    if(!pAccessibilityOptions)
    {
        pAccessibilityOptions = new SvtAccessibilityOptions;
        pAccessibilityOptions->AddListener(this);
    }
    return *pAccessibilityOptions;
}
/* -----------------06.05.2003 14:52-----------------

 --------------------------------------------------*/
SvtCTLOptions& SwModule::GetCTLOptions()
{
    if(!pCTLOptions)
    {
        pCTLOptions = new SvtCTLOptions;
        pCTLOptions->AddListener(this);
    }
    return *pCTLOptions;
}
/* -----------------07.07.2003 09:31-----------------

 --------------------------------------------------*/
SvtUserOptions& SwModule::GetUserOptions()
{
    if(!pUserOptions)
    {
        pUserOptions = new SvtUserOptions;
        pUserOptions->AddListener(this);
    }
    return *pUserOptions;
}
/* -----------------18.07.2003 13:31-----------------

 --------------------------------------------------*/
SvtUndoOptions& SwModule::GetUndoOptions()
{
    if(!pUndoOptions)
    {
        pUndoOptions = new SvtUndoOptions;
        pUndoOptions->AddListener(this);
    }
    return *pUndoOptions;
}
/*-----------------30.01.97 08.30-------------------

--------------------------------------------------*/
const SwMasterUsrPref *SwModule::GetUsrPref(sal_Bool bWeb) const
{
    SwModule* pNonConstModule = (SwModule*)this;
    if(bWeb && !pWebUsrPref)
    {
        // im Load der SwMasterUsrPref wird der SpellChecker gebraucht, dort darf
        // er aber nicht angelegt werden #58256#
        pNonConstModule->pWebUsrPref = new SwMasterUsrPref(TRUE);
    }
    else if(!bWeb && !pUsrPref)
    {
        pNonConstModule->pUsrPref = new SwMasterUsrPref(FALSE);
    }
    return  bWeb ? pWebUsrPref : pUsrPref;
}



void NewXForms( SfxRequest& rReq )
{
    // copied & excerpted from SwModule::InsertLab(..)

    // create new document
    SfxObjectShellRef xDocSh( new SwDocShell( SFX_CREATE_MODE_STANDARD) );
    xDocSh->DoInitNew( 0 );

    // initialize XForms
    static_cast<SwDocShell*>( &xDocSh )->GetDoc()->initXForms( true );

    // put document into frame
    const SfxItemSet* pArgs = rReq.GetArgs();
    DBG_ASSERT( pArgs, "no arguments in SfxRequest");
    if( pArgs != NULL )
    {
        const SfxPoolItem* pFrameItem = NULL;
        pArgs->GetItemState( SID_DOCFRAME, FALSE, &pFrameItem );
        if( pFrameItem != NULL )
        {
            SfxFrame* pFrame =
                static_cast<const SfxFrameItem*>( pFrameItem )->GetFrame();
            DBG_ASSERT( pFrame != NULL, "no frame?" );
            pFrame->InsertDocument( xDocSh );
        }
    }

    // set return value
    rReq.SetReturnValue( SfxVoidItem( rReq.GetSlot() ) );
}
