/*************************************************************************
 *
 *  $RCSfile: mmdocselectpage.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-27 13:39:38 $
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

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _SFXNEW_HXX 
#include <sfx2/new.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX 
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _SFX_OBJFAC_HXX
#include <sfx2/docfac.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SWVIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>        
#endif
#ifndef _MAILMERGEDOCSELECTPAGE_HXX
#include <mmdocselectpage.hxx>
#endif
#ifndef _MAILMERGEWIZARD_HXX
#include <mailmergewizard.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _SW_ABSTDLG_HXX
#include <swabstdlg.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif

#include <dbui.hrc>
#include <mmdocselectpage.hrc>

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif

using namespace com::sun::star::ui::dialogs;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace svt;

/*-- 02.04.2004 09:40:14---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeDocSelectPage::SwMailMergeDocSelectPage( SwMailMergeWizard* _pParent ) :
    svt::OWizardPage(_pParent, SW_RES(DLG_MM_DOCSELECT_PAGE)),
#pragma warning (disable : 4355)
    m_aHeaderFI(this,          ResId(  FI_HEADER           ) ),
    m_aHowToFT          (this, ResId( FT_HOWTO )),
    m_aCurrentDocRB     (this, ResId( RB_CURRENTDOC )),
    m_aNewDocRB         (this, ResId( RB_NEWDOC )),
    m_aLoadDocRB        (this, ResId( RB_LOADDOC )),
    m_aBrowseDocPB      (this, ResId( PB_LOADDOC )),
    m_aLoadTemplateRB   (this, ResId( RB_LOADTEMPLATE )),
    m_aBrowseTemplatePB (this, ResId( PB_BROWSETEMPLATE )),
    m_aRecentDocRB      (this, ResId( RB_RECENTDOC )),
    m_aRecentDocLB      (this, ResId( LB_RECENTDOC )),
#pragma warning (default : 4355)
    m_pWizard(_pParent)
{
    FreeResource();
    
    m_aCurrentDocRB.Check();
    DocSelectHdl(&m_aNewDocRB);
    
    Link aDocSelectLink = LINK(this, SwMailMergeDocSelectPage, DocSelectHdl);
    m_aCurrentDocRB.SetClickHdl(aDocSelectLink);
    m_aNewDocRB.SetClickHdl(aDocSelectLink);
    m_aLoadDocRB.SetClickHdl(aDocSelectLink);
    m_aLoadTemplateRB.SetClickHdl(aDocSelectLink);
    m_aRecentDocRB.SetClickHdl(aDocSelectLink);
            
    Link aFileSelectHdl = LINK(this, SwMailMergeDocSelectPage, FileSelectHdl);
    m_aBrowseDocPB.SetClickHdl(aFileSelectHdl);
    m_aBrowseTemplatePB.SetClickHdl(aFileSelectHdl);

    const uno::Sequence< ::rtl::OUString >& rDocs = 
                            m_pWizard->GetConfigItem().GetSavedDocuments();
    for(sal_Int32 nDoc = 0; nDoc < rDocs.getLength(); ++nDoc)
    {
        //insert in reverse order
        m_aRecentDocLB.InsertEntry(rDocs[nDoc], 0);
    }            
    m_aRecentDocLB.SelectEntryPos(0);
    if(!rDocs.getLength())
    {
        m_aRecentDocRB.Enable(sal_False);
    }            
}           

/*-- 02.04.2004 09:40:14---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeDocSelectPage::~SwMailMergeDocSelectPage()
{
}

/*-- 05.04.2004 14:21:48---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeDocSelectPage, DocSelectHdl, RadioButton*, pButton)
{
    m_aRecentDocLB.Enable(&m_aRecentDocRB == pButton);
    sal_Bool bEnableNext = m_aNewDocRB.IsChecked() || m_aCurrentDocRB.IsChecked() ||
            (m_sLoadFileName.Len() && (m_aLoadDocRB.IsChecked() || m_aLoadTemplateRB.IsChecked())) ||
            m_aRecentDocLB.GetSelectEntry().Len();
            
    m_pWizard->enableButtons( WZB_NEXT, bEnableNext );


    return 0;
}
/*-- 05.04.2004 14:25:12---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeDocSelectPage, FileSelectHdl, PushButton*, pButton)
{
    bool bTemplate = &m_aBrowseTemplatePB == pButton;
    
    if(bTemplate)
    {        
        m_aLoadTemplateRB.Check();
        SfxNewFileDialog* pNewFileDlg = new SfxNewFileDialog(this, 0);
        //pNewFileDlg->SetTemplateFlags(nFlags);
        USHORT nRet = pNewFileDlg->Execute();
        if(RET_TEMPLATE_LOAD == nRet)
            bTemplate = false;
        else if(RET_CANCEL != nRet)
            m_sLoadFileName = pNewFileDlg->GetTemplateFileName();
        delete pNewFileDlg;
    }
    else
        m_aLoadDocRB.Check();

    if(!bTemplate)
    {
        sfx2::FileDialogHelper aDlgHelper( TemplateDescription::FILEOPEN_SIMPLE, 0 );
        Reference < XFilePicker > xFP = aDlgHelper.GetFilePicker();

        xFP->setDisplayDirectory( SvtPathOptions().GetWorkPath() );

        SfxObjectFactory &rFact = m_pWizard->GetSwView()->GetDocShell()->GetFactory();
        SfxFilterMatcher aMatcher( String::CreateFromAscii(rFact.GetShortName()) );
        SfxFilterMatcherIter aIter( &aMatcher );
        Reference<XFilterManager> xFltMgr(xFP, UNO_QUERY);
        const SfxFilter* pFlt = aIter.First();
        while( pFlt )
        {
            if( pFlt && pFlt->IsAllowedAsTemplate() )
            {
                const String sWild = ((WildCard&)pFlt->GetWildcard()).GetWildCard();
                xFltMgr->appendFilter( pFlt->GetUIName(), sWild );
            }

            if( pFlt->GetUserData().EqualsAscii( GetFILTER_XML() ))
                xFltMgr->setCurrentFilter( pFlt->GetUIName() ) ;

            pFlt = aIter.Next();
        }

        if( ERRCODE_NONE == aDlgHelper.Execute() )
        {
            m_sLoadFileName = xFP->getFiles().getConstArray()[0];
        }
    }
    m_pWizard->enableButtons( WZB_NEXT, m_sLoadFileName.Len() > 0 );

    return 0;
}

/*-- 06.04.2004 12:52:24---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwMailMergeDocSelectPage::commitPage(COMMIT_REASON _eReason)
{
    if(_eReason == CR_TRAVEL_NEXT)
    {        
        sal_Bool bCloseDialog = sal_True;
        if(m_sLoadFileName.Len() && 
                (m_aLoadDocRB.IsChecked() || m_aLoadTemplateRB.IsChecked()))
            m_pWizard->SetReloadDocument(m_sLoadFileName);
        else if(m_aNewDocRB.IsChecked())
            m_pWizard->SetReloadDocument(String());
        else if(m_aRecentDocRB.IsChecked())
        {
            m_pWizard->SetReloadDocument(m_aRecentDocLB.GetSelectEntry());
        }            
        else
            bCloseDialog = sal_False;
        if(bCloseDialog)
        {        
            m_pWizard->SetRestartPage(MM_OUTPUTTYPETPAGE);
            m_pWizard->EndDialog(RET_LOAD_DOC);        
        }
    }
    return sal_True;
}
