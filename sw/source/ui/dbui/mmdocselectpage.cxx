/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mmdocselectpage.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 11:33:36 $
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
#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


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

#include "com/sun/star/ui/dialogs/TemplateDescription.hpp"
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif

using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace svt;

/*-- 02.04.2004 09:40:14---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeDocSelectPage::SwMailMergeDocSelectPage( SwMailMergeWizard* _pParent ) :
    svt::OWizardPage(_pParent, SW_RES(DLG_MM_DOCSELECT_PAGE)),
#ifdef MSC
#pragma warning (disable : 4355)
#endif
    m_aHeaderFI(this,          SW_RES(  FI_HEADER           ) ),
    m_aHowToFT          (this, SW_RES( FT_HOWTO )),
    m_aCurrentDocRB     (this, SW_RES( RB_CURRENTDOC )),
    m_aNewDocRB         (this, SW_RES( RB_NEWDOC )),
    m_aLoadDocRB        (this, SW_RES( RB_LOADDOC )),
    m_aLoadTemplateRB   (this, SW_RES( RB_LOADTEMPLATE )),
    m_aRecentDocRB      (this, SW_RES( RB_RECENTDOC )),
    m_aBrowseDocPB      (this, SW_RES( PB_LOADDOC )),
    m_aBrowseTemplatePB (this, SW_RES( PB_BROWSETEMPLATE )),
    m_aRecentDocLB      (this, SW_RES( LB_RECENTDOC )),
#ifdef MSC
#pragma warning (default : 4355)
#endif
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

    m_pWizard->UpdateRoadmap();
    m_pWizard->enableButtons(WZB_NEXT, m_pWizard->isStateEnabled(MM_OUTPUTTYPETPAGE));

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
            m_sLoadTemplateName = pNewFileDlg->GetTemplateFileName();
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

                // #i40125
                if(pFlt->GetFilterFlags() & SFX_FILTER_DEFAULT)
                    xFltMgr->setCurrentFilter( pFlt->GetUIName() ) ;
            }


            pFlt = aIter.Next();
        }

        if( ERRCODE_NONE == aDlgHelper.Execute() )
        {
            m_sLoadFileName = xFP->getFiles().getConstArray()[0];
        }
    }
    m_pWizard->UpdateRoadmap();
    m_pWizard->enableButtons(WZB_NEXT, m_pWizard->isStateEnabled(MM_OUTPUTTYPETPAGE));

    return 0;
}

/*-- 06.04.2004 12:52:24---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwMailMergeDocSelectPage::commitPage(COMMIT_REASON _eReason)
{
    sal_Bool bReturn = sal_False;
    bool bNext = _eReason == CR_TRAVEL_NEXT;
    if(bNext || _eReason == CR_VALIDATE )
    {
        ::rtl::OUString sReloadDocument;
        bReturn = m_aCurrentDocRB.IsChecked() ||
                m_aNewDocRB.IsChecked() ||
                ((sReloadDocument = m_sLoadFileName).getLength() && m_aLoadDocRB.IsChecked() )||
                ((sReloadDocument = m_sLoadTemplateName).getLength() && m_aLoadTemplateRB.IsChecked())||
                (m_aRecentDocRB.IsChecked() && (sReloadDocument = m_aRecentDocLB.GetSelectEntry()).getLength());
        if(bNext && !m_aCurrentDocRB.IsChecked())
        {
            if(sReloadDocument.getLength())
                m_pWizard->SetReloadDocument( sReloadDocument );
            m_pWizard->SetRestartPage(MM_OUTPUTTYPETPAGE);
            m_pWizard->EndDialog(RET_LOAD_DOC);
        }
    }
    return bReturn;
}
