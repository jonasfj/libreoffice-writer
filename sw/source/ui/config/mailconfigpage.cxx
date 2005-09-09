/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mailconfigpage.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:42:42 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#pragma hdrstop

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _MAILCONFIGPAGE_HXX
#include <mailconfigpage.hxx>
#endif
#ifndef _BASEDLGS_HXX 
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _SVEDIT_HXX 
#include <svtools/svmedit.hxx>
#endif
#ifndef _STDCTRL_HXX 
#include <svtools/stdctrl.hxx>
#endif
#ifndef _SVTABBX_HXX
#include <svtools/svtabbx.hxx>
#endif
#ifndef _HEADBAR_HXX
#include <svtools/headbar.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _MAILMERGEHELPER_HXX
#include <mailmergehelper.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_MAIL_MAILSERVICETYPE_HPP_
#include "com/sun/star/mail/MailServiceType.hpp"
#endif
#ifndef _COM_SUN_STAR_MAIL_XMAILSERVICE_HPP_
#include "com/sun/star/mail/XMailService.hpp"
#endif
#ifndef _COM_SUN_STAR_MAIL_MAILSERVICEPROVIDER_HPP_
#include "com/sun/star/mail/MailServiceProvider.hpp"
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#include <globals.hrc>
#include <mailconfigpage.hrc>
#include <config.hrc>
#include <helpid.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::mail;
using namespace ::com::sun::star::beans;
using namespace ::rtl;
/*-- 06.05.2004 12:04:11---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwTestAccountSettingsDialog : public SfxModalDialog
{
    FixedInfo           m_aInfoFI;
    
    HeaderBar           m_aStatusHB;
    SvTabListBox        m_aStatusLB;
    
    FixedInfo           m_aErrorFI;
    MultiLineEdit       m_eErrorsED;

    FixedLine           m_aSeparatorFL;
    PushButton          m_aStopPB;
    CancelButton        m_aCancelPB;
    HelpButton          m_aHelpPB;

    ImageList           m_aImageList;

    String              m_sTask        ;
    String              m_sStatus      ;
    String              m_sEstablish   ;
    String              m_sFindServer  ;
    String              m_sCompleted   ;
    String              m_sFailed      ;
    String              m_sErrorNetwork;
    String              m_sErrorServer ;

    SwMailConfigPage*   m_pParent;

    bool                m_bStop;
    
    void                Test();
    DECL_LINK(StopHdl, PushButton*);
    DECL_STATIC_LINK(SwTestAccountSettingsDialog, TestHdl, void*);
public:    
    SwTestAccountSettingsDialog(SwMailConfigPage* pParent);
    ~SwTestAccountSettingsDialog();
};
/*-- 19.08.2004 14:27:33---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwAuthenticationSettingsDialog : public SfxModalDialog
{
    CheckBox        m_aAuthenticationCB;

    RadioButton     m_aSeparateAuthenticationRB;
    RadioButton     m_aSMTPAfterPOPRB;
    
    FixedInfo       m_aOutgoingServerFT;
    FixedText       m_aUserNameFT;
    Edit            m_aUserNameED;
    FixedText       m_aOutPasswordFT;
    Edit            m_aOutPasswordED;
    
    FixedInfo       m_aIncomingServerFT;
    FixedText       m_aServerFT;
    Edit            m_aServerED;
    FixedText       m_aPortFT;
    NumericField    m_aPortNF;
    FixedText       m_aProtocolFT;
    RadioButton     m_aPOP3RB;
    RadioButton     m_aIMAPRB;
    FixedText       m_aInUsernameFT;
    Edit            m_aInUsernameED;
    FixedText       m_aInPasswordFT;
    Edit            m_aInPasswordED;

    FixedLine       m_aSeparatorFL;

    OKButton        m_aOKPB;
    CancelButton    m_aCancelPB;
    HelpButton      m_aHelpPB;
    
    SwMailMergeConfigItem& rConfigItem;

    DECL_LINK( OKHdl_Impl, OKButton*);
    DECL_LINK( CheckBoxHdl_Impl, CheckBox*);
    DECL_LINK( RadioButtonHdl_Impl, RadioButton*);


public:    
    SwAuthenticationSettingsDialog(SwMailConfigPage* pParent, SwMailMergeConfigItem& rItem);
    ~SwAuthenticationSettingsDialog();
};

/*-- 06.05.2004 10:59:40---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailConfigPage::SwMailConfigPage( Window* pParent, const SfxItemSet& rSet ) :
    SfxTabPage(pParent, SW_RES(TP_MAILCONFIG), rSet),
#pragma warning (disable : 4355)
    m_aIdentityFL( this, ResId(       FL_IDENTITY)),
    m_aDisplayNameFT( this, ResId(    FT_DISPLAYNAME)),
    m_aDisplayNameED( this, ResId(    ED_DISPLAYNAME)),
    m_aAddressFT( this, ResId(        FT_ADDRESS)),
    m_aAddressED( this, ResId(        ED_ADDRESS)),
    m_aReplyToCB( this, ResId(        CB_REPLYTO)),
    m_aReplyToFT( this, ResId(        FT_REPLYTO)),
    m_aReplyToED( this, ResId(        ED_REPLYTO)),
    m_aSMTPFL( this, ResId(           FL_SMTP)),
    m_aServerFT( this, ResId(         FT_SERVER)),
    m_aServerED( this, ResId(         ED_SERVER)),
    m_aPortFT( this, ResId(           FT_PORT)),
    m_aPortNF( this, ResId(           NF_PORT)),
    m_aSecureCB( this, ResId(         CB_SECURE)),
    m_aServerAuthenticationPB( this, ResId( PB_AUTHENTICATION )),
    m_aSeparatorFL( this,            ResId( FL_SEPARATOR      )),
    m_aTestPB( this, ResId(           PB_TEST)),
#pragma warning (default : 4355)
    m_pConfigItem( new SwMailMergeConfigItem )
{
    FreeResource();
    m_aReplyToCB.SetClickHdl(LINK(this, SwMailConfigPage, ReplyToHdl));
    m_aServerAuthenticationPB.SetClickHdl(LINK(this, SwMailConfigPage, AuthenticationHdl));
    m_aTestPB.SetClickHdl(LINK(this, SwMailConfigPage, TestHdl));
}
/*-- 06.05.2004 10:59:40---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailConfigPage::~SwMailConfigPage()
{
    delete m_pConfigItem;    
}
/*-- 06.05.2004 10:59:40---------------------------------------------------

  -----------------------------------------------------------------------*/
SfxTabPage*  SwMailConfigPage::Create( Window* pParent, const SfxItemSet& rAttrSet)
{
    return new SwMailConfigPage(pParent, rAttrSet);
}
/*-- 06.05.2004 10:59:41---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwMailConfigPage::FillItemSet( SfxItemSet& rSet )
{
    if(m_aDisplayNameED.GetText() != m_aDisplayNameED.GetSavedValue())
        m_pConfigItem->SetMailDisplayName(m_aDisplayNameED.GetText());
    if(m_aAddressED.GetText() != m_aAddressED.GetSavedValue())
        m_pConfigItem->SetMailAddress(m_aAddressED.GetText());
    String sReplyTo;
    if( m_aReplyToCB.GetSavedValue() != m_aReplyToCB.IsChecked())
        m_pConfigItem->SetMailReplyTo(m_aReplyToCB.IsChecked());
    if(m_aReplyToED.GetText() != m_aReplyToED.GetSavedValue())
        m_pConfigItem->SetMailReplyTo(m_aReplyToED.GetText());
    if(m_aServerED.GetText() != m_aServerED.GetSavedValue())
        m_pConfigItem->SetMailServer(m_aServerED.GetText());
    
    if(m_aPortNF.IsModified())
        m_pConfigItem->SetMailPort((sal_Int16)m_aPortNF.GetValue());
    
    m_pConfigItem->SetSecureConnection(m_aSecureCB.IsChecked());
    
    m_pConfigItem->Commit();
    return sal_True;
}
/*-- 06.05.2004 10:59:41---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailConfigPage::Reset( const SfxItemSet& rSet )
{
    m_aDisplayNameED.SetText(m_pConfigItem->GetMailDisplayName());
    m_aAddressED.SetText(m_pConfigItem->GetMailAddress());
    
    m_aReplyToED.SetText(m_pConfigItem->GetMailReplyTo()) ;
    m_aReplyToCB.Check(m_pConfigItem->IsMailReplyTo());
    m_aReplyToCB.GetClickHdl().Call(&m_aReplyToCB);
    
    m_aServerED.SetText(m_pConfigItem->GetMailServer());
    m_aPortNF.SetValue(m_pConfigItem->GetMailPort());
    
    m_aSecureCB.Check(m_pConfigItem->IsSecureConnection());
    
    m_aDisplayNameED.SaveValue();
    m_aAddressED    .SaveValue();
    m_aReplyToCB    .SaveValue();
    m_aReplyToED    .SaveValue();
    m_aServerED     .SaveValue();
    m_aPortNF       .SaveValue();
    m_aSecureCB     .SaveValue();
}
/*-- 06.05.2004 10:59:41---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailConfigPage, ReplyToHdl, CheckBox*, pBox)
{
    sal_Bool bEnable = pBox->IsChecked();
    m_aReplyToFT.Enable(bEnable);
    m_aReplyToED.Enable(bEnable);
    return 0;
}
/*-- 06.05.2004 10:59:41---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailConfigPage, AuthenticationHdl, PushButton*, pButton)
{
    SwAuthenticationSettingsDialog aDlg(this, *m_pConfigItem);
    aDlg.Execute();
    return 0;
}
/*-- 06.05.2004 10:59:42---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailConfigPage, TestHdl, PushButton*, pButton)
{
    SwTestAccountSettingsDialog(this).Execute();
    return 0;
}
/*-- 06.05.2004 12:11:13---------------------------------------------------

  -----------------------------------------------------------------------*/
SwTestAccountSettingsDialog::SwTestAccountSettingsDialog(SwMailConfigPage* pParent) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_TESTACCOUNTSETTINGS)),
#pragma warning (disable : 4355)
    m_aInfoFI( this, ResId(         FI_INFO )),
    m_aStatusHB( this, WB_BUTTONSTYLE | WB_BOTTOMBORDER),
    m_aStatusLB( this, ResId(       LB_STATUS )),
    m_aErrorFI( this, ResId(        FI_ERROR  )),
    m_eErrorsED( this, ResId(       ED_ERROR  )),
    m_aSeparatorFL( this, ResId(    FL_SEPAPARATOR )),
    m_aStopPB( this, ResId(         PB_STOP   )),
    m_aCancelPB( this, ResId(       PB_CANCEL )),
    m_aHelpPB( this, ResId(         PB_HELP   )),
#pragma warning (default : 4355)
    m_aImageList( ResId( GetSettings().GetStyleSettings().GetWindowColor().IsDark() ? ILIST_HC : ILIST) ),
    m_sTask( ResId(        ST_TASK          )),   
    m_sStatus( ResId(      ST_STATUS        )),
    m_sEstablish( ResId(   ST_ESTABLISH     )),
    m_sFindServer( ResId(  ST_FINDSERVER    )),
    m_sCompleted( ResId(   ST_COMPLETED     )),
    m_sFailed( ResId(      ST_FAILED        )),
    m_sErrorServer( ResId( ST_ERROR_SERVER )),
    m_pParent(pParent),
    m_bStop(false)
{   
    FreeResource();
    m_aStopPB.SetClickHdl(LINK(this, SwTestAccountSettingsDialog, StopHdl));

    Size aLBSize(m_aStatusLB.GetOutputSizePixel());
    m_aStatusHB.SetSizePixel(aLBSize);
    Size aHeadSize(m_aStatusHB.CalcWindowSizePixel());
    aHeadSize.Width() = aLBSize.Width();
    m_aStatusHB.SetSizePixel(aHeadSize);
    Point aLBPos(m_aStatusLB.GetPosPixel());
    m_aStatusHB.SetPosPixel(aLBPos);
    aLBPos.Y() += aHeadSize.Height();
    aLBSize.Height() -= aHeadSize.Height();
    m_aStatusLB.SetPosSizePixel(aLBPos, aLBSize);
    
    Size aSz(m_aStatusHB.GetOutputSizePixel());
    m_aStatusHB.InsertItem( 1, m_sTask,
                            aSz.Width()/2,
                            HIB_LEFT | HIB_VCENTER );
    m_aStatusHB.InsertItem( 2, m_sStatus,
                            aSz.Width()/2,
                            HIB_LEFT | HIB_VCENTER );
                          
    m_aStatusHB.SetHelpId(HID_MM_TESTACCOUNTSETTINGS_HB  );
    m_aStatusHB.Show();

    m_aStatusLB.SetHelpId(HID_MM_TESTACCOUNTSETTINGS_TLB);
    static long nTabs[] = {2, 0, aSz.Width()/2 };
    m_aStatusLB.SetWindowBits( WB_SORT | WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP );
    m_aStatusLB.SetSelectionMode( SINGLE_SELECTION );
    m_aStatusLB.SetTabs(&nTabs[0], MAP_PIXEL);
    short nEntryHeight = m_aStatusLB.GetEntryHeight();
    m_aStatusLB.SetEntryHeight( nEntryHeight * 15 / 10 );

    Application::PostUserEvent( STATIC_LINK( this, SwTestAccountSettingsDialog, TestHdl ), this );
}
/*-- 06.05.2004 12:11:13---------------------------------------------------

  -----------------------------------------------------------------------*/
SwTestAccountSettingsDialog::~SwTestAccountSettingsDialog()
{
}
/*-- 06.05.2004 12:15:43---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwTestAccountSettingsDialog, StopHdl, PushButton*, pButton)
{
    m_bStop = true;
    return 0;        
}
/*-- 07.06.2004 12:44:50---------------------------------------------------
    
  -----------------------------------------------------------------------*/
IMPL_STATIC_LINK(SwTestAccountSettingsDialog, TestHdl, void*, EMPTYARG)
{
    pThis->EnterWait();
    pThis->Test();
    pThis->LeaveWait();
    return 0;
}
/*-- 07.06.2004 12:45:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwTestAccountSettingsDialog::Test()
{
    uno::Reference<XMultiServiceFactory> rMgr = ::comphelper::getProcessServiceFactory();
    
    bool bIsLoggedIn = false;
    bool bIsServer = false;
    if (rMgr.is())
    {                                          
        try
        {
            uno::Reference< mail::XMailService > xInMailService;
            uno::Reference< mail::XMailServiceProvider > xMailServiceProvider = 
                    mail::MailServiceProvider::create(getCurrentCmpCtx(rMgr));
            uno::Reference< mail::XMailService > xMailService = 
                    xMailServiceProvider->create(
                    mail::MailServiceType_SMTP);
            if(m_bStop)
                return;
            uno::Reference<XConnectionListener> xConnectionListener(new SwConnectionListener());
            
            if(m_pParent->m_pConfigItem->IsAuthentication() &&
                    m_pParent->m_pConfigItem->IsSMTPAfterPOP())
            {
                xInMailService = xMailServiceProvider->create(
                        m_pParent->m_pConfigItem->IsInServerPOP() ?
                            mail::MailServiceType_POP3 : mail::MailServiceType_IMAP);
                if(m_bStop)
                    return;
                //authenticate at the POP or IMAP server first
                uno::Reference<XAuthenticator> xAuthenticator = 
                    new SwAuthenticator(
                        m_pParent->m_pConfigItem->GetInServerUserName(), 
                        m_pParent->m_pConfigItem->GetInServerPassword(),
                        this);
                
                xInMailService->addConnectionListener(xConnectionListener);
                //check connection
                uno::Reference< uno::XCurrentContext> xConnectionContext = 
                        new SwConnectionContext(
                            m_pParent->m_pConfigItem->GetInServerName(), 
                            m_pParent->m_pConfigItem->GetInServerPort(), 
                            ::rtl::OUString::createFromAscii( "Insecure" ));
                xInMailService->connect(xConnectionContext, xAuthenticator);  
            }
            if(m_bStop)
                return;
            uno::Reference<XAuthenticator> xAuthenticator;                                
            if(m_pParent->m_pConfigItem->IsAuthentication() &&
                    !m_pParent->m_pConfigItem->IsSMTPAfterPOP() &&
                    m_pParent->m_pConfigItem->GetMailUserName().getLength())
                xAuthenticator = 
                    new SwAuthenticator(
                        m_pParent->m_pConfigItem->GetMailUserName(), 
                        m_pParent->m_pConfigItem->GetMailPassword(),
                        this);
            else
                xAuthenticator =  new SwAuthenticator();
                
            xMailService->addConnectionListener(xConnectionListener);
            if(m_bStop)
                return;
            //just to check if the server exists
            xMailService->getSupportedConnectionTypes();
            if(m_bStop)
                return;
            bIsServer = true;
            //check connection
            uno::Reference< uno::XCurrentContext> xConnectionContext = 
                    new SwConnectionContext(
                        m_pParent->m_aServerED.GetText(), 
                        m_pParent->m_aPortNF.GetValue(), 
                        ::rtl::OUString::createFromAscii( 
                                m_pParent->m_aSecureCB.IsChecked() ? "Ssl" : "Insecure"));
            xMailService->connect(xConnectionContext, xAuthenticator);  
            bIsLoggedIn = xMailService->isConnected();
            if( xInMailService.is() )
                xInMailService->disconnect();
            if( xMailService->isConnected())
                xMailService->disconnect();
        }
        catch(uno::Exception& rEx)
        {
            rEx; //prevent compiler warning
            DBG_ERROR("exception caught")
        }
    }
    
    Image aFailedImg =   m_aImageList.GetImage( FN_FORMULA_CANCEL );
    Image aCompletedImg = m_aImageList.GetImage( FN_FORMULA_APPLY );

    String sTmp(m_sEstablish);
    sTmp += '\t';
    sTmp += bIsServer ? m_sCompleted : m_sFailed;
    m_aStatusLB.InsertEntry(sTmp, 
            bIsServer ? aCompletedImg : aFailedImg, 
            bIsServer ? aCompletedImg : aFailedImg);
    
    sTmp = m_sFindServer;
    sTmp += '\t';
    sTmp += bIsLoggedIn ? m_sCompleted : m_sFailed;
    m_aStatusLB.InsertEntry(sTmp, 
            bIsLoggedIn ? aCompletedImg : aFailedImg, 
            bIsLoggedIn ? aCompletedImg : aFailedImg);

    if(!bIsServer || !bIsLoggedIn )
    {        
        m_eErrorsED.SetText( m_sErrorServer ); 
    }
}
/*-- 18.08.2004 12:18:38---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailConfigDlg::SwMailConfigDlg(Window* pParent, SfxItemSet& rSet ) :
    SfxSingleTabDialog(pParent, rSet, 0)
{
    // TabPage erzeugen
    SfxTabPage* pPage = SwMailConfigPage::Create( this, rSet );
    SetTabPage(pPage);
}
/*-- 18.08.2004 12:18:38---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailConfigDlg::~SwMailConfigDlg()
{
}            
/*-- 19.08.2004 14:33:58---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAuthenticationSettingsDialog::SwAuthenticationSettingsDialog(
        SwMailConfigPage* pParent, SwMailMergeConfigItem& rItem) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_SERVERAUTHENTICATION)),
#pragma warning (disable : 4355)
        m_aAuthenticationCB( this,          ResId( CB_AUTHENTICATION        )),
        m_aSeparateAuthenticationRB( this,  ResId( RB_SEP_AUTHENTICATION   )),
        m_aSMTPAfterPOPRB( this,            ResId( RB_SMPTAFTERPOP         )),
        m_aOutgoingServerFT( this,          ResId( FT_OUTGOINGSERVER        )),
        m_aUserNameFT( this,                ResId( FT_USERNAME             )),
        m_aUserNameED( this,                ResId( ED_USERNAME             )),
        m_aOutPasswordFT( this,             ResId( FT_OUTPASSWORD          )),
        m_aOutPasswordED( this,             ResId( ED_OUTPASSWORD          )),
        m_aIncomingServerFT( this,          ResId( FT_INCOMINGSERVER       )),
        m_aServerFT( this,                  ResId( FT_SERVER               )),
        m_aServerED( this,                  ResId( ED_SERVER               )),
        m_aPortFT( this,                    ResId( FT_PORT                 )),
        m_aPortNF( this,                    ResId( NF_PORT                 )),
        m_aProtocolFT( this,                ResId( FT_PROTOCOL      )),
        m_aPOP3RB( this,                    ResId( RB_POP3          )),
        m_aIMAPRB( this,                    ResId( RB_IMAP          )),
        m_aInUsernameFT( this,              ResId( FT_INUSERNAME            )),
        m_aInUsernameED( this,              ResId( ED_INUSERNAME           )),
        m_aInPasswordFT( this,              ResId( FT_INPASSWORD           )),
        m_aInPasswordED( this,              ResId( ED_INPASSWORD           )),
        m_aSeparatorFL( this,               ResId( FL_SEPARATOR            )),
        m_aOKPB( this,                      ResId( PB_OK                   )),
        m_aCancelPB( this,                  ResId( PB_CANCEL               )),
        m_aHelpPB( this,                    ResId( PB_HELP                 )),
#pragma warning (default : 4355)
        rConfigItem( rItem )
{
    FreeResource();

    m_aAuthenticationCB.SetClickHdl( LINK( this, SwAuthenticationSettingsDialog, CheckBoxHdl_Impl));
    Link aRBLink = LINK( this, SwAuthenticationSettingsDialog, RadioButtonHdl_Impl );
    m_aSeparateAuthenticationRB.SetClickHdl( aRBLink );
    m_aSMTPAfterPOPRB.SetClickHdl( aRBLink );
    m_aOKPB.SetClickHdl( LINK( this, SwAuthenticationSettingsDialog, OKHdl_Impl));
    
    m_aAuthenticationCB.Check( rConfigItem.IsAuthentication() );
    if(rConfigItem.IsSMTPAfterPOP())
        m_aSMTPAfterPOPRB.Check();
    else
        m_aSeparateAuthenticationRB.Check();
    m_aUserNameED.SetText( rConfigItem.GetMailUserName() );
    m_aOutPasswordED.SetText( rConfigItem.GetMailPassword() );
    
    m_aServerED.SetText( rConfigItem.GetInServerName() );
    m_aPortNF.SetValue( rConfigItem.GetInServerPort() );
    if(rConfigItem.IsInServerPOP())
        m_aPOP3RB.Check();
    else
        m_aIMAPRB.Check();
    m_aInUsernameED.SetText( rConfigItem.GetInServerUserName());
    m_aInPasswordED.SetText( rConfigItem.GetInServerPassword() );
    
    CheckBoxHdl_Impl( &m_aAuthenticationCB );
}
/*-- 19.08.2004 14:33:58---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAuthenticationSettingsDialog::~SwAuthenticationSettingsDialog()
{
}
/*-- 19.08.2004 14:33:59---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwAuthenticationSettingsDialog, OKHdl_Impl, OKButton*, EMPTYARG)
{
    rConfigItem.SetAuthentication( m_aAuthenticationCB.IsChecked() );
    rConfigItem.SetSMTPAfterPOP(m_aSMTPAfterPOPRB.IsChecked());
    rConfigItem.SetMailUserName(m_aUserNameED.GetText());
    rConfigItem.SetMailPassword(m_aOutPasswordED.GetText());
    rConfigItem.SetInServerName(m_aServerED.GetText());
    rConfigItem.SetInServerPort(m_aPortNF.GetValue( ) );
    rConfigItem.SetInServerPOP(m_aPOP3RB.IsChecked());
    rConfigItem.SetInServerUserName(m_aInUsernameED.GetText());

    rConfigItem.SetInServerPassword(m_aInPasswordED.GetText());
    EndDialog(RET_OK);
    return 0;
}
/*-- 19.08.2004 14:33:59---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwAuthenticationSettingsDialog, CheckBoxHdl_Impl, CheckBox*, pBox)
{
    sal_Bool bChecked = pBox->IsChecked();
    m_aSeparateAuthenticationRB.Enable(bChecked);
    m_aSMTPAfterPOPRB.Enable(bChecked);
    RadioButtonHdl_Impl( 0 );
    
    return 0;
}
/*-- 19.08.2004 14:33:59---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwAuthenticationSettingsDialog, RadioButtonHdl_Impl, RadioButton*, EMPTYARG)
{
    sal_Bool bSeparate = m_aSeparateAuthenticationRB.IsChecked();
    sal_Bool bIsEnabled = m_aSeparateAuthenticationRB.IsEnabled();
    sal_Bool bNotSeparate = !bSeparate & bIsEnabled;
    bSeparate &= bIsEnabled;

    m_aOutgoingServerFT.Enable(bSeparate);
    m_aUserNameFT.Enable(bSeparate);
    m_aUserNameED.Enable(bSeparate);
    m_aOutPasswordFT.Enable(bSeparate);
    m_aOutPasswordED.Enable(bSeparate);
    
    m_aIncomingServerFT.Enable(bNotSeparate);
    m_aServerFT.Enable(bNotSeparate);
    m_aServerED.Enable(bNotSeparate);
    m_aPortFT.Enable(bNotSeparate);
    m_aPortNF.Enable(bNotSeparate);
    m_aInUsernameFT.Enable(bNotSeparate);
    m_aInUsernameED.Enable(bNotSeparate);
    m_aProtocolFT.Enable(bNotSeparate);
    m_aPOP3RB.Enable(bNotSeparate);
    m_aIMAPRB.Enable(bNotSeparate);
    m_aInPasswordFT.Enable(bNotSeparate);
    m_aInPasswordED.Enable(bNotSeparate);
    
    return 0;
}
