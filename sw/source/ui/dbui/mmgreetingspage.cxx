/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mmgreetingspage.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2006-01-06 13:02:38 $
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
#ifndef _MAILMERGEGREETINGSPAGE_HXX
#include <mmgreetingspage.hxx>
#endif
#ifndef _MAILMERGEWIZARD_HXX
#include <mailmergewizard.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _MAILMERGEADDRESSBLOCKPAGE_HXX
#include <mmaddressblockpage.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#include <mmgreetingspage.hrc>
#include <dbui.hrc>
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#include <helpid.h>

using namespace svt;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
// -----------------------------------------------------------------------
void lcl_FillGreetingsBox(ListBox& rBox, 
                        SwMailMergeConfigItem& rConfig, 
                        SwMailMergeConfigItem::Gender eType)
{
    const Sequence< ::rtl::OUString> rEntries = rConfig.GetGreetings(eType);
    for(sal_Int32 nEntry = 0; nEntry < rEntries.getLength(); ++nEntry)
        rBox.InsertEntry(rEntries[nEntry]);
    rBox.SelectEntryPos((USHORT)rConfig.GetCurrentGreeting(eType));
}
// -----------------------------------------------------------------------
void lcl_FillGreetingsBox(ComboBox& rBox, 
                        SwMailMergeConfigItem& rConfig, 
                        SwMailMergeConfigItem::Gender eType)
{
    const Sequence< ::rtl::OUString> rEntries = rConfig.GetGreetings(eType);
    for(sal_Int32 nEntry = 0; nEntry < rEntries.getLength(); ++nEntry)
        rBox.InsertEntry(rEntries[nEntry]);
    rBox.SelectEntryPos((USHORT)rConfig.GetCurrentGreeting(eType));
}
// -----------------------------------------------------------------------
void lcl_StoreGreetingsBox(ListBox& rBox, 
                        SwMailMergeConfigItem& rConfig, 
                        SwMailMergeConfigItem::Gender eType)
{
    Sequence< ::rtl::OUString> aEntries(rBox.GetEntryCount());
    ::rtl::OUString* pEntries = aEntries.getArray(); 
    for(USHORT nEntry = 0; nEntry < rBox.GetEntryCount(); ++nEntry)
        pEntries[nEntry] = rBox.GetEntry(nEntry);
    rConfig.SetGreetings(eType, aEntries);
    rConfig.SetCurrentGreeting(eType, rBox.GetSelectEntryPos());
}
// -----------------------------------------------------------------------
void lcl_StoreGreetingsBox(ComboBox& rBox, 
                        SwMailMergeConfigItem& rConfig, 
                        SwMailMergeConfigItem::Gender eType)
{
    Sequence< ::rtl::OUString> aEntries(rBox.GetEntryCount());
    ::rtl::OUString* pEntries = aEntries.getArray(); 
    for(USHORT nEntry = 0; nEntry < rBox.GetEntryCount(); ++nEntry)
        pEntries[nEntry] = rBox.GetEntry(nEntry);
    rConfig.SetGreetings(eType, aEntries);
    rConfig.SetCurrentGreeting(eType, rBox.GetSelectEntryPos());
}
/*-- 30.04.2004 10:42:57---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwGreetingsHandler, IndividualHdl_Impl, CheckBox*, EMPTYARG)
{
    BOOL bIndividual = m_pPersonalizedCB->IsEnabled() && m_pPersonalizedCB->IsChecked();
    m_pFemaleFT->Enable(bIndividual);
    m_pFemaleLB->Enable(bIndividual);
    m_pFemalePB->Enable(bIndividual);
    m_pMaleFT->Enable(bIndividual);
    m_pMaleLB->Enable(bIndividual);
    m_pMalePB->Enable(bIndividual);
    m_pFemaleFI->Enable(bIndividual);
    m_pFemaleColumnFT->Enable(bIndividual);
    m_pFemaleColumnLB->Enable(bIndividual);
    m_pFemaleFieldFT->Enable(bIndividual);
    m_pFemaleFieldCB->Enable(bIndividual);
    return 0;
}
/*-- 30.04.2004 10:42:57---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwGreetingsHandler, GreetingHdl_Impl, PushButton*, pButton)
{
    SwCustomizeAddressBlockDialog* pDlg = 
            new SwCustomizeAddressBlockDialog(pButton, m_pWizard->GetConfigItem(), 
                        pButton == m_pMalePB ? 
                        SwCustomizeAddressBlockDialog::GREETING_MALE :
                        SwCustomizeAddressBlockDialog::GREETING_FEMALE );
    if(RET_OK == pDlg->Execute())
    {
        ListBox* pToInsert = pButton == m_pMalePB ? m_pMaleLB : m_pFemaleLB;
        pToInsert->SelectEntryPos(pToInsert->InsertEntry(pDlg->GetAddress()));
        UpdatePreview();
        if(m_bIsTabPage)
            m_pWizard->enableButtons(WZB_NEXT, m_pWizard->isStateEnabled(MM_PREPAREMERGEPAGE));
    }
    delete pDlg;            
    return 0;
}
/*-- 16.06.2004 10:56:21---------------------------------------------------

  -----------------------------------------------------------------------*/
void    SwGreetingsHandler::UpdatePreview()
{
    //the base class does nothing
}
/*-- 30.04.2004 10:42:59---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeGreetingsPage, AssignHdl_Impl, PushButton*, pButton)
{
    String sPreview = m_aFemaleLB.GetSelectEntry();
    sPreview += '\n';
    sPreview += m_aMaleLB.GetSelectEntry();
    SwAssignFieldsDialog* pDlg = 
            new SwAssignFieldsDialog(pButton, m_pWizard->GetConfigItem(), sPreview, false);
    if(RET_OK == pDlg->Execute())
    {
        UpdatePreview();
        m_pWizard->UpdateRoadmap();
        m_pWizard->enableButtons(WZB_NEXT, m_pWizard->isStateEnabled(MM_PREPAREMERGEPAGE));
    }    
    delete pDlg;
    return 0;
}
/*-- 05.05.2004 15:25:56---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeGreetingsPage, GreetingSelectHdl_Impl, ListBox*, EMPTYARG)
{
    UpdatePreview();
    return 0;
}
/*-- 05.05.2004 15:23:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailMergeGreetingsPage::UpdatePreview()
{
    String sPreview = m_aFemaleLB.GetSelectEntry();
    sPreview += '\n';
    sPreview += m_aMaleLB.GetSelectEntry();

    sPreview = SwAddressPreview::FillData(sPreview, m_pWizard->GetConfigItem());
    m_aPreviewWIN.SetAddress(sPreview);
}
/*-- 17.05.2004 15:44:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void    SwGreetingsHandler::Contains(sal_Bool bContainsGreeting)
{
    m_pPersonalizedCB->Enable(bContainsGreeting);
    sal_Bool bEnablePersonal = bContainsGreeting && m_pPersonalizedCB->IsChecked();
    m_pFemaleFT->Enable(bEnablePersonal);
    m_pFemaleLB->Enable(bEnablePersonal);
    m_pFemalePB->Enable(bEnablePersonal);
    m_pMaleFT->Enable(bEnablePersonal);
    m_pMaleLB->Enable(bEnablePersonal);
    m_pMalePB->Enable(bEnablePersonal);
    m_pFemaleFI->Enable(bEnablePersonal);
    m_pFemaleColumnFT->Enable(bEnablePersonal);
    m_pFemaleColumnLB->Enable(bEnablePersonal);
    m_pFemaleFieldFT->Enable(bEnablePersonal);
    m_pFemaleFieldCB->Enable(bEnablePersonal);
    
    m_pNeutralFT->Enable(bContainsGreeting);
    m_pNeutralCB->Enable(bContainsGreeting);
}
/*-- 02.04.2004 14:17:52---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeGreetingsPage::SwMailMergeGreetingsPage( SwMailMergeWizard* _pParent) :
    svt::OWizardPage(_pParent, SW_RES(DLG_MM_GREETINGS_PAGE)),
#pragma warning (disable : 4355)
    m_aHeaderFI(this, ResId(          FI_HEADER     ) ),
    m_aGreetingLineCB(this, ResId(    CB_GREETINGLINE ) ),
    m_aPersonalizedCB(this, ResId(      CB_PERSONALIZED ) ),
    m_aFemaleFT(this, ResId(            FT_FEMALE   ) ),
    m_aFemaleLB(this, ResId(            LB_FEMALE   ) ),   
    m_aFemalePB(this, ResId(            PB_FEMALE   ) ),   
    m_aMaleFT(this, ResId(              FT_MALE     ) ),   
    m_aMaleLB(this, ResId(              LB_MALE     ) ),
    m_aMalePB(this, ResId(              PB_MALE     ) ),
    m_aFemaleFI(this, ResId(            FI_FEMALE      ) ),
    m_aFemaleColumnFT(this, ResId(      FT_FEMALECOLUMN ) ),
    m_aFemaleColumnLB(this, ResId(      LB_FEMALECOLUMN ) ),
    m_aFemaleFieldFT(this, ResId(       FT_FEMALEFIELD  ) ),
    m_aFemaleFieldCB(this, ResId(       CB_FEMALEFIELD  ) ),
    m_aNeutralFT(this, ResId(           FT_NEUTRAL      ) ),
    m_aNeutralCB(this, ResId(         CB_NEUTRAL      ) ),
    m_aPreviewFI(       this, ResId( FI_PREVIEW     ) ),
    m_aPreviewWIN(      this, ResId( WIN_PREVIEW    ) ),
    m_aAssignPB(        this, ResId( PB_ASSIGN      ) ),
    m_aDocumentIndexFI( this, ResId( FI_DOCINDEX    ) ),
    m_aPrevSetIB(       this, ResId( IB_PREVSET     ) ),
    m_aNextSetIB(       this, ResId( IB_NEXTSET     ) ),
    m_sDocument(        ResId(       STR_DOCUMENT  ) )
#pragma warning (default : 4355)
{
    m_pWizard = _pParent;
    m_pGreetingLineCB = &m_aGreetingLineCB;
    m_pPersonalizedCB = &m_aPersonalizedCB;
    m_pFemaleFT = &      m_aFemaleFT;
    m_pFemaleLB = &      m_aFemaleLB;
    m_pFemalePB = &      m_aFemalePB;
    m_pMaleFT = &        m_aMaleFT;
    m_pMaleLB = &        m_aMaleLB;
    m_pMalePB = &        m_aMalePB;
    m_pFemaleFI = &      m_aFemaleFI;
    m_pFemaleColumnFT = &m_aFemaleColumnFT;
    m_pFemaleColumnLB = &m_aFemaleColumnLB;
    m_pFemaleFieldFT = & m_aFemaleFieldFT;
    m_pFemaleFieldCB = & m_aFemaleFieldCB;
    m_pNeutralFT = &     m_aNeutralFT;
    m_pNeutralCB    = &m_aNeutralCB;
    m_bIsTabPage = true;
    
    m_pPersonalizedCB->SetHelpId(   HID_MM_GREETINGS_CB_PERSONALIZED);
    m_pFemaleLB->SetHelpId(         HID_MM_GREETINGS_LB_FEMALE      );
    m_pFemalePB->SetHelpId(         HID_MM_GREETINGS_PB_FEMALE      );
    m_pMaleLB->SetHelpId(           HID_MM_GREETINGS_LB_MALE        );
    m_pMalePB->SetHelpId(           HID_MM_GREETINGS_PB_MALE        );
    m_pFemaleColumnLB->SetHelpId(   HID_MM_GREETINGS_LB_FEMALECOLUMN);
    m_pFemaleFieldCB->SetHelpId(    HID_MM_GREETINGS_CB_FEMALEFIELD );
    m_pNeutralCB->SetHelpId(        HID_MM_GREETINGS_CB_NEUTRAL     );
    
    FreeResource();
    m_aGreetingLineCB.SetClickHdl(LINK(this, SwMailMergeGreetingsPage, ContainsHdl_Impl));
    Link aIndividualLink = LINK(this, SwGreetingsHandler, IndividualHdl_Impl);
    m_aPersonalizedCB.SetClickHdl(aIndividualLink);
    Link aGreetingLink = LINK(this, SwGreetingsHandler, GreetingHdl_Impl);
    m_aFemalePB.SetClickHdl(aGreetingLink);
    m_aMalePB.SetClickHdl(aGreetingLink);
    m_aAssignPB.SetClickHdl(LINK(this, SwMailMergeGreetingsPage, AssignHdl_Impl));
    Link aLBoxLink = LINK(this, SwMailMergeGreetingsPage, GreetingSelectHdl_Impl);
    m_aFemaleLB.SetSelectHdl(aLBoxLink);
    m_aMaleLB.SetSelectHdl(aLBoxLink);
    Link aDataLink = LINK(this, SwMailMergeGreetingsPage, InsertDataHdl_Impl);
    m_aPrevSetIB.SetClickHdl(aDataLink);
    m_aNextSetIB.SetClickHdl(aDataLink);


    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();
    m_aGreetingLineCB.Check(rConfig.IsGreetingLine(sal_False));
    m_aPersonalizedCB.Check(rConfig.IsIndividualGreeting(sal_False));
    ContainsHdl_Impl(&m_aGreetingLineCB);
    aIndividualLink.Call(0);

    lcl_FillGreetingsBox(m_aFemaleLB, rConfig, SwMailMergeConfigItem::FEMALE);
    lcl_FillGreetingsBox(m_aMaleLB, rConfig, SwMailMergeConfigItem::MALE);
    lcl_FillGreetingsBox(m_aNeutralCB, rConfig, SwMailMergeConfigItem::NEUTRAL);
    
    String sTemp(m_sDocument);
    sTemp.SearchAndReplaceAscii("%1", String::CreateFromInt32(1));
    m_aDocumentIndexFI.SetText(sTemp);
}
/*-- 02.04.2004 14:17:52---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeGreetingsPage::~SwMailMergeGreetingsPage()
{
}
/*-- 18.06.2004 12:13:53---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailMergeGreetingsPage::ActivatePage()
{
    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();

    //try to find the gender setting
    m_aFemaleColumnLB.Clear();
    Reference< sdbcx::XColumnsSupplier > xColsSupp = rConfig.GetColumnsSupplier();
    if(xColsSupp.is())
    {        
        Reference < container::XNameAccess> xColAccess = xColsSupp->getColumns();
        Sequence< ::rtl::OUString > aColumns = xColAccess->getElementNames();
        for(sal_Int32 nName = 0; nName < aColumns.getLength(); ++nName)
            m_aFemaleColumnLB.InsertEntry(aColumns[nName]);
    }

    ::rtl::OUString sGenderColumn = rConfig.GetAssignedColumn(MM_PART_GENDER);
    m_aFemaleColumnLB.SelectEntry(sGenderColumn);
    m_aFemaleColumnLB.SaveValue();

    m_aFemaleFieldCB.SetText(rConfig.GetFemaleGenderValue());
    m_aFemaleFieldCB.SaveValue();

    UpdatePreview();
    m_pWizard->enableButtons(WZB_NEXT, m_pWizard->isStateEnabled(MM_PREPAREMERGEPAGE));
}
/*-- 11.05.2004 14:47:10---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool    SwMailMergeGreetingsPage::commitPage(COMMIT_REASON)
{
    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();
    
    if(m_aFemaleColumnLB.GetSelectEntryPos() != m_aFemaleColumnLB.GetSavedValue())
    {        
        const SwDBData& rDBData = rConfig.GetCurrentDBData();
        Sequence< ::rtl::OUString> aAssignment = rConfig.GetColumnAssignment( rDBData );
        sal_Int32 nPos = m_aFemaleColumnLB.GetSelectEntryPos();
        if(aAssignment.getLength() <= MM_PART_GENDER)
            aAssignment.realloc(MM_PART_GENDER + 1);
        if( nPos > 0 )
            aAssignment[MM_PART_GENDER] = m_aFemaleColumnLB.GetSelectEntry();
        else
            aAssignment[MM_PART_GENDER] = ::rtl::OUString();
        rConfig.SetColumnAssignment( rDBData, aAssignment );
    }
    if(m_aFemaleFieldCB.GetText() != m_aFemaleFieldCB.GetSavedValue())
        rConfig.SetFemaleGenderValue(m_aFemaleFieldCB.GetText());
    
    lcl_StoreGreetingsBox(m_aFemaleLB, rConfig, SwMailMergeConfigItem::FEMALE);
    lcl_StoreGreetingsBox(m_aMaleLB, rConfig, SwMailMergeConfigItem::MALE);
    lcl_StoreGreetingsBox(m_aNeutralCB, rConfig, SwMailMergeConfigItem::NEUTRAL);
    rConfig.SetGreetingLine(m_aGreetingLineCB.IsChecked(), sal_False);
    rConfig.SetIndividualGreeting(m_aPersonalizedCB.IsChecked(), sal_False);
    return sal_True;
}            
/*-- 30.04.2004 10:42:57---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeGreetingsPage, ContainsHdl_Impl, CheckBox*, pBox)
{
    sal_Bool bContainsGreeting = pBox->IsChecked();
    SwGreetingsHandler::Contains(bContainsGreeting);
    m_aPreviewFI. Enable(bContainsGreeting);
    m_aPreviewWIN.Enable(bContainsGreeting);
    m_aAssignPB.  Enable(bContainsGreeting);
    m_aDocumentIndexFI.  Enable(bContainsGreeting);
    m_aPrevSetIB.Enable(bContainsGreeting);
    m_aNextSetIB.Enable(bContainsGreeting);
    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();
    rConfig.SetGreetingLine(m_aGreetingLineCB.IsChecked(), sal_False);
    m_pWizard->UpdateRoadmap();
    return 0;
}
/*-- 07.05.2004 12:56:42---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeGreetingsPage, InsertDataHdl_Impl, ImageButton*, pButton)
{
    //if no pButton is given, the first set has to be pre-set
    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();
    if(!pButton)
    {
        rConfig.GetResultSet();
    }
    else
    {
        BOOL bNext = pButton == &m_aNextSetIB;
        sal_Int32 nPos = rConfig.GetResultSetPosition();
        rConfig.MoveResultSet( bNext ? ++nPos : --nPos);
    }    
    sal_Int32 nPos = rConfig.GetResultSetPosition();
    BOOL bEnable = TRUE;
    if(nPos < 1)
    {
        bEnable = FALSE;
        nPos = 1;
    }
    else
        UpdatePreview();
    m_aPrevSetIB.Enable(bEnable);
    m_aNextSetIB.Enable(bEnable);
    m_aDocumentIndexFI.Enable(bEnable);
    String sTemp(m_sDocument);
    sTemp.SearchAndReplaceAscii("%1", String::CreateFromInt32(nPos));
    m_aDocumentIndexFI.SetText(sTemp);
    return 0;
}
/*-- 17.05.2004 15:11:19---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailBodyDialog::SwMailBodyDialog(Window* pParent, SwMailMergeWizard* _pWizard) : 
    SfxModalDialog(pParent, SW_RES(DLG_MM_MAILBODY)),
#pragma warning (disable : 4355)
    m_aGreetingLineCB(this, ResId(    CB_GREETINGLINE ) ),
    m_aPersonalizedCB(this, ResId(      CB_PERSONALIZED ) ),
    m_aFemaleFT(this, ResId(            FT_FEMALE   ) ),
    m_aFemaleLB(this, ResId(            LB_FEMALE   ) ),   
    m_aFemalePB(this, ResId(            PB_FEMALE   ) ),   
    m_aMaleFT(this, ResId(              FT_MALE     ) ),   
    m_aMaleLB(this, ResId(              LB_MALE     ) ),
    m_aMalePB(this, ResId(              PB_MALE     ) ),
    m_aFemaleFI(this, ResId(            FI_FEMALE      ) ),
    m_aFemaleColumnFT(this, ResId(      FT_FEMALECOLUMN ) ),
    m_aFemaleColumnLB(this, ResId(      LB_FEMALECOLUMN ) ),
    m_aFemaleFieldFT(this, ResId(       FT_FEMALEFIELD  ) ),
    m_aFemaleFieldCB(this, ResId(       CB_FEMALEFIELD  ) ),
    m_aNeutralFT(this, ResId(           FT_NEUTRAL      ) ),
    m_aNeutralCB(this, ResId(         CB_NEUTRAL      ) ),
    m_aBodyFT(   this, ResId(         FT_BODY         ) ),
    m_aBodyMLE(   this, ResId(        MLE_BODY        ) ),
    m_aSeparatorFL(   this, ResId(    FL_SEPARATOR    ) ),
    m_aOK(   this, ResId(             PB_OK           ) ),
    m_aCancel(   this, ResId(         PB_CANCEL       ) ),
    m_aHelp(   this, ResId(           PB_HELP         ) )
#pragma warning (default : 4355)
{
    m_pWizard = _pWizard;
    m_pGreetingLineCB = &m_aGreetingLineCB;
    m_pPersonalizedCB = &m_aPersonalizedCB;
    m_pFemaleFT = &      m_aFemaleFT;
    m_pFemaleLB = &      m_aFemaleLB;
    m_pFemalePB = &      m_aFemalePB;
    m_pMaleFT = &        m_aMaleFT;
    m_pMaleLB = &        m_aMaleLB;
    m_pMalePB = &        m_aMalePB;
    m_pFemaleFI = &      m_aFemaleFI;
    m_pFemaleColumnFT = &m_aFemaleColumnFT;
    m_pFemaleColumnLB = &m_aFemaleColumnLB;
    m_pFemaleFieldFT = & m_aFemaleFieldFT;
    m_pFemaleFieldCB = & m_aFemaleFieldCB;
    m_pNeutralFT = &     m_aNeutralFT;
    m_pNeutralCB    = &m_aNeutralCB;
    m_bIsTabPage = false;

    m_pPersonalizedCB->SetHelpId(   HID_MM_BODY_CB_PERSONALIZED     );
    m_pFemaleLB->SetHelpId(         HID_MM_BODY_LB_FEMALE           );
    m_pFemalePB->SetHelpId(         HID_MM_BODY_PB_FEMALE           );
    m_pMaleLB->SetHelpId(           HID_MM_BODY_LB_MALE             );
    m_pMalePB->SetHelpId(           HID_MM_BODY_PB_MALE             );
    m_pFemaleColumnLB->SetHelpId(   HID_MM_BODY_LB_FEMALECOLUMN     );
    m_pFemaleFieldCB->SetHelpId(    HID_MM_BODY_CB_FEMALEFIELD      );
    m_pNeutralCB->SetHelpId(        HID_MM_BODY_CB_NEUTRAL          );
    
    FreeResource();
    m_aGreetingLineCB.SetClickHdl(LINK(this, SwMailBodyDialog, ContainsHdl_Impl));
    Link aIndividualLink = LINK(this, SwGreetingsHandler, IndividualHdl_Impl);
    m_aPersonalizedCB.SetClickHdl(aIndividualLink);
    Link aGreetingLink = LINK(this, SwGreetingsHandler, GreetingHdl_Impl);
    m_aFemalePB.SetClickHdl(aGreetingLink);
    m_aMalePB.SetClickHdl(aGreetingLink);
    m_aOK.SetClickHdl(LINK(this, SwMailBodyDialog, OKHdl));

    SwMailMergeConfigItem& rConfig = m_pWizard->GetConfigItem();
    m_aGreetingLineCB.Check(rConfig.IsGreetingLine(sal_True));
    m_aPersonalizedCB.Check(rConfig.IsIndividualGreeting(sal_True));
    ContainsHdl_Impl(&m_aGreetingLineCB);
    aIndividualLink.Call(0);

    lcl_FillGreetingsBox(m_aFemaleLB, rConfig, SwMailMergeConfigItem::FEMALE);
    lcl_FillGreetingsBox(m_aMaleLB, rConfig, SwMailMergeConfigItem::MALE);
    lcl_FillGreetingsBox(m_aNeutralCB, rConfig, SwMailMergeConfigItem::NEUTRAL);

    //try to find the gender setting
    m_aFemaleColumnLB.Clear();
    Reference< sdbcx::XColumnsSupplier > xColsSupp = rConfig.GetColumnsSupplier();
    if(xColsSupp.is())
    {        
        Reference < container::XNameAccess> xColAccess = xColsSupp->getColumns();
        Sequence< ::rtl::OUString > aColumns = xColAccess->getElementNames();
        for(sal_Int32 nName = 0; nName < aColumns.getLength(); ++nName)
            m_aFemaleColumnLB.InsertEntry(aColumns[nName]);
    }

    ::rtl::OUString sGenderColumn = rConfig.GetAssignedColumn(MM_PART_GENDER);
    m_aFemaleColumnLB.SelectEntry(sGenderColumn);
    m_aFemaleColumnLB.SaveValue();

    m_aFemaleFieldCB.SetText(rConfig.GetFemaleGenderValue());
    m_aFemaleFieldCB.SaveValue();
}    
/*-- 17.05.2004 15:13:07---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailBodyDialog::~SwMailBodyDialog()
{
}

/*-- 30.04.2004 10:42:57---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailBodyDialog, ContainsHdl_Impl, CheckBox*, pBox)
{
    SwGreetingsHandler::Contains(pBox->IsChecked());
    m_pWizard->GetConfigItem().SetGreetingLine(pBox->IsChecked(), sal_True);
    return 0;
}
/*-- 28.06.2004 11:22:42---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailBodyDialog, OKHdl, PushButton*, EMPTYARG)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    rConfigItem.SetGreetingLine(
                m_aGreetingLineCB.IsChecked(), sal_False);
    rConfigItem.SetIndividualGreeting(
                m_aPersonalizedCB.IsChecked(), sal_False);
    
    if(m_aFemaleColumnLB.GetSelectEntryPos() != m_aFemaleColumnLB.GetSavedValue())
    {        
        const SwDBData& rDBData = rConfigItem.GetCurrentDBData();
        Sequence< ::rtl::OUString> aAssignment = rConfigItem.GetColumnAssignment( rDBData );
        sal_Int32 nPos = m_aFemaleColumnLB.GetSelectEntryPos();
        if(aAssignment.getLength() < MM_PART_GENDER)
            aAssignment.realloc(MM_PART_GENDER);
        if( nPos > 0 )
            aAssignment[MM_PART_GENDER] = m_aFemaleColumnLB.GetSelectEntry();
        else
            aAssignment[MM_PART_GENDER] = ::rtl::OUString();
        rConfigItem.SetColumnAssignment( rDBData, aAssignment );
    }
    if(m_aFemaleFieldCB.GetText() != m_aFemaleFieldCB.GetSavedValue())
        rConfigItem.SetFemaleGenderValue(m_aFemaleFieldCB.GetText());
    
    EndDialog(RET_OK);
    return 0;
}

