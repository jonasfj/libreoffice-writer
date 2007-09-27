/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mmpreparemergepage.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 11:35:23 $
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

#ifndef _MAILMERGEPREPAREMERGEPAGE_HXX
#include <mmpreparemergepage.hxx>
#endif
#ifndef _MAILMERGEWIZARD_HXX
#include <mailmergewizard.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _DBUI_HRC
#include <dbui.hrc>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#include <svx/dataaccessdescriptor.hxx>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _SW_ABSTDLG_HXX
#include <swabstdlg.hxx>
#endif


#include <mmpreparemergepage.hrc>
#include <dbui.hrc>

#include <unomid.h>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbc;
using namespace ::rtl;


/*-- 02.04.2004 16:42:49---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergePrepareMergePage::SwMailMergePrepareMergePage( SwMailMergeWizard* _pParent) :
    svt::OWizardPage( _pParent, SW_RES(DLG_MM_PREPAREMERGE_PAGE)),
#ifdef MSC
#pragma warning (disable : 4355)
#endif
    m_aHeaderFI(this,  SW_RES(     FI_HEADER ) ),
    m_aPreviewFI(this, SW_RES(     FI_PREVIEW ) ),
    m_aRecipientFT(this, SW_RES(   FT_RECIPIENT ) ),
    m_aFirstPB(this, SW_RES(       PB_FIRST   ) ),
    m_aPrevPB(this, SW_RES(        PB_PREV    ) ),
    m_aRecordED(this, SW_RES(      ED_RECORD  ) ),
    m_aNextPB(this, SW_RES(        PB_NEXT    ) ),
    m_aLastPB(this, SW_RES(        PB_LAST    ) ),
    m_ExcludeCB(this, SW_RES(      CB_EXCLUDE    ) ),
    m_aNoteHeaderFL(this, SW_RES(  FL_NOTEHEADER ) ),
    m_aEditFI(this, SW_RES(        FI_EDIT       ) ),
    m_aEditPB(this, SW_RES(        PB_EDIT       ) ),
#ifdef MSC
#pragma warning (default : 4355)
#endif
    m_pWizard(_pParent)
{
    FreeResource();
    m_aEditPB.SetClickHdl( LINK( this, SwMailMergePrepareMergePage, EditDocumentHdl_Impl));
    Link aMoveLink(LINK( this, SwMailMergePrepareMergePage, MoveHdl_Impl));
    m_aFirstPB.SetClickHdl( aMoveLink );
    m_aPrevPB.SetClickHdl( aMoveLink );
    m_aNextPB.SetClickHdl( aMoveLink );
    m_aLastPB.SetClickHdl( aMoveLink );
    m_aRecordED.SetActionHdl( aMoveLink );
    m_ExcludeCB.SetClickHdl(LINK(this, SwMailMergePrepareMergePage, ExcludeHdl_Impl));
    aMoveLink.Call(&m_aRecordED);
}
/*-- 02.04.2004 16:42:49---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergePrepareMergePage::~SwMailMergePrepareMergePage()
{
}
/*-- 13.05.2004 15:36:48---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwMailMergePrepareMergePage, EditDocumentHdl_Impl, PushButton*, EMPTYARG)
{
    m_pWizard->SetRestartPage(MM_PREPAREMERGEPAGE);
    m_pWizard->EndDialog(RET_EDIT_DOC);
    return 0;
}
/*-- 27.05.2004 14:16:37---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwMailMergePrepareMergePage, MoveHdl_Impl, void*, pCtrl)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    sal_Int32 nPos = rConfigItem.GetResultSetPosition();
    if(pCtrl == &m_aFirstPB)
    {
        rConfigItem.MoveResultSet(1);
    }
    else if(pCtrl == &m_aPrevPB)
    {
        rConfigItem.MoveResultSet(nPos - 1);
    }
    else if(pCtrl == &m_aRecordED)
    {
        rConfigItem.MoveResultSet( static_cast< sal_Int32 >(m_aRecordED.GetValue()) );
    }
    else if(pCtrl == &m_aNextPB)
        rConfigItem.MoveResultSet(nPos + 1);
    else if(pCtrl == &m_aLastPB)
        rConfigItem.MoveResultSet(-1);

    nPos = rConfigItem.GetResultSetPosition();
    m_aRecordED.SetValue(nPos);
    bool bIsFirst;
    bool bIsLast;
    bool bValid = rConfigItem.IsResultSetFirstLast(bIsFirst, bIsLast);
    m_aFirstPB.Enable(bValid && !bIsFirst);
    m_aPrevPB.Enable(bValid && !bIsFirst);
    m_aNextPB.Enable(bValid && !bIsLast);
    m_aLastPB.Enable(bValid && !bIsLast);
    m_ExcludeCB.Check(rConfigItem.IsRecordExcluded( rConfigItem.GetResultSetPosition() ));
    //now the record has to be merged into the source document
    const SwDBData& rDBData = rConfigItem.GetCurrentDBData();

    Sequence< PropertyValue > aArgs(7);
    Sequence<Any> aSelection(1);
    aSelection[0] <<= rConfigItem.GetResultSetPosition();
    aArgs[0].Name = C2U("Selection");
    aArgs[0].Value <<= aSelection;
    aArgs[1].Name = C2U("DataSourceName");
    aArgs[1].Value <<= rDBData.sDataSource;
    aArgs[2].Name = C2U("Command");
    aArgs[2].Value <<= rDBData.sCommand;
    aArgs[3].Name = C2U("CommandType");
    aArgs[3].Value <<= rDBData.nCommandType;
    aArgs[4].Name = C2U("ActiveConnection");
    aArgs[4].Value <<=  rConfigItem.GetConnection().getTyped();
    aArgs[5].Name = C2U("Filter");
    aArgs[5].Value <<= rConfigItem.GetFilter();
    aArgs[6].Name = C2U("Cursor");
    aArgs[6].Value <<= rConfigItem.GetResultSet();

    ::svx::ODataAccessDescriptor aDescriptor(aArgs);
    SwWrtShell& rSh = m_pWizard->GetSwView()->GetWrtShell();
    SwMergeDescriptor aMergeDesc( DBMGR_MERGE, rSh, aDescriptor );
    rSh.GetNewDBMgr()->MergeNew(aMergeDesc);
    return 0;
}
/*-- 27.05.2004 14:46:28---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwMailMergePrepareMergePage, ExcludeHdl_Impl, CheckBox*, pBox)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    rConfigItem.ExcludeRecord( rConfigItem.GetResultSetPosition(), pBox->IsChecked());
    return 0;
};
/*-- 18.08.2004 10:36:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwMailMergePrepareMergePage::ActivatePage()
{
    MoveHdl_Impl(&m_aRecordED);
}
/*-- 13.05.2004 15:38:32---------------------------------------------------
    merge the data into a new file
  -----------------------------------------------------------------------*/
sal_Bool  SwMailMergePrepareMergePage::commitPage(COMMIT_REASON _eReason)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    if(CR_TRAVEL_NEXT == _eReason && !rConfigItem.IsMergeDone())
    {
        m_pWizard->CreateTargetDocument();
        m_pWizard->SetRestartPage(MM_MERGEPAGE);
        m_pWizard->EndDialog(RET_TARGET_CREATED);
    }
    return sal_True;
}
